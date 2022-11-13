#include <arpa/inet.h>
#include <cctype>
#include <cstddef>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <netinet/in.h>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <utility>
#include <vector>

#include "ConfigParser.hpp"

namespace ft
{

using std::ifstream;
using std::string;
using std::vector;
using std::ostringstream;

/* STATIC DATA */

#define SIZE(x) sizeof(x) / sizeof(x[0])

const ConfigParser::token_dispatch_t ConfigParser::m_block_dispatch_table[] = {
	{"server", &ConfigParser::_parseServerBlock},
};

// Had to create a C array before because C++98 does not support uniform initialization
// Range constructor
const vector< ConfigParser::token_dispatch_t > ConfigParser::m_block_dispatch_vec(m_block_dispatch_table,
                                                             m_block_dispatch_table + SIZE(m_block_dispatch_table));

const ConfigParser::token_dispatch_t ConfigParser::m_server_block_dispatch_table[] = {
    {"listen", &ConfigParser::_parseListen},
    {"root", &ConfigParser::_parseRoot},
    {"index", &ConfigParser::_parseIndex},
    {"server_name", &ConfigParser::_parseServerName},
};

// Range constructor
const vector< ConfigParser::token_dispatch_t > ConfigParser::m_server_block_dispatch_vec(m_server_block_dispatch_table,
                                                                    m_server_block_dispatch_table +
                                                                        SIZE(m_server_block_dispatch_table));

#undef SIZE

/* METHODS */

#ifndef DEFAULT_CONFIG 
# define DEFAULT_CONFIG "webserv.conf"
#endif

/* Ctor */ ConfigParser::ConfigParser(const char* config)
{
	// Init stream
	std::ifstream ifs;
	if (config != NULL)
	{
		ifs.open(config);
		if (not ifs)
			throw std::runtime_error("Could not open config file passed in paramater");
	}
	else
	{
		ifs.open(DEFAULT_CONFIG);
		if (not ifs)
			throw std::runtime_error("Could not open defaut config file: " DEFAULT_CONFIG );
	}

	ConfigParser::configstream_iterator it(ifs);

	// Where the actual parsing takes place
	while (ifs)
		_match(it, m_block_dispatch_vec, true);

	// Copy the virtserv addresses into a map<sockaddr_in, vec_of_virtserv_refs>
	vector< VirtServ >::iterator	virtserv		= m_virtserv_vec.begin();
	vector< VirtServ >::iterator	end				= m_virtserv_vec.end();
	for (; virtserv != end; ++virtserv)
	{
		vector< sockaddr_in >&			m_sockaddr_vec	= virtserv->m_sockaddr_vec;
		vector< sockaddr_in >::iterator sockaddr		= m_sockaddr_vec.begin();
		vector< sockaddr_in >::iterator vec_end			= m_sockaddr_vec.end();
		for (; sockaddr != vec_end; ++sockaddr)
		{
			m_virtserv_map[*sockaddr].push_back(&(*virtserv));
		}
	}
#ifdef DEBUG // Debugging output 
	{
		map< sockaddr_in, vector< VirtServ* > >::iterator it  = m_virtserv_map.begin();
		map< sockaddr_in, vector< VirtServ* > >::iterator end = m_virtserv_map.end();
		for (; it != end; ++it)
		{
			vector< VirtServ* >&          virtserv_vec = it->second;
			vector< VirtServ* >::iterator vit          = virtserv_vec.begin();
			vector< VirtServ* >::iterator vend         = virtserv_vec.end();
			cout << "=========================================\n";
			cout << "ON " << (long)(it->first.sin_addr.s_addr) <<":"<< it->first.sin_port << '\n';
			cout << "=========================================\n";
			for (; vit != vend; ++vit)
			{
				cout << **vit << '\n';
			}
			cout << "_________________________________________\n";
		}
	}
#endif
}

#define BEFORE false
#define AFTER true

void ConfigParser::_match(ConfigParser::configstream_iterator it, const vector<token_dispatch_t>& dispatch_table, bool throw_on_not_found = false)
{
	size_t i;
	size_t table_size = dispatch_table.size();
	for ( i = 0;  i < table_size and not it->empty(); ++i )
	{
		if ( it->compare(dispatch_table[i].token) == 0)
		{
			(this->*(dispatch_table[i].parse_method))(it); 
			i = -1;
		}
	}
	if (throw_on_not_found and i == table_size)
	{
		throw std::runtime_error("Config file error: Unknown token");
	}
}

void ConfigParser::_parseServerBlock(ConfigParser::configstream_iterator& it)
{
	++it;
	if (*it == "{")
	{
		m_virtserv_vec.push_back(VirtServ());
		_match(it, m_server_block_dispatch_vec);
	}
	else
		throw std::runtime_error("Config file error: Missing '{' delimiter after server block directive");
	if (*it != "}")
	{
		throw std::runtime_error("Config file error: Invalid token");
	}
}

static int xatoi(const string& str)
{
	string::const_iterator it = str.begin();
	string::const_iterator end = str.end();
	for (--end; it != end ; ++it)
		if ( not std::isdigit(*it) )
			throw std::runtime_error("Config file error: invalid port in listen directive ");
	if (*it != ';')
			throw std::runtime_error("Config file error: invalid port in listen directive ");
	return ::atoi(str.c_str());
}

void ConfigParser::_parseListen(ConfigParser::configstream_iterator& it)
{
	++it;
	const string& host_port = *it;
	string host;
	string port;
	size_t colon = host_port.find_last_of(":."); 
	if ( colon == string::npos ) // No colons, then only port information
	{
		host.assign("0");
		port.assign(host_port);
	}
	else if ( host_port[colon] == ':' ) // Is of the form host:port 
	{
		host.assign(*it, 0, colon);
		port.assign(*it, colon + 1, string::npos);
	}
	else if ( host_port[colon] == '.' ) // Only dot-style ip adress
	{
		host.assign(host_port);
		port.assign("80");
	}

	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = inet_addr(host.c_str());
	if ( sockaddr.sin_addr.s_addr == INADDR_NONE )
		throw std::runtime_error("Config file error: listen directive does not have a valid ip address");
	sockaddr.sin_port = htons(xatoi(port.c_str()));

	m_virtserv_vec.back().m_sockaddr_vec.push_back(sockaddr);
}

void ConfigParser::_parseServerName(ConfigParser::configstream_iterator& it)
{
	for (++it; not it.is_delim() ; ++it)
		m_virtserv_vec.back().m_server_name.push_back(*it);
	if (*it != ";")
	{
		throw std::runtime_error("Config file error: missing ; after server_name directive");
	}
}

void ConfigParser::_parseRoot(ConfigParser::configstream_iterator& it)
{
	for (++it; not it.is_delim() ; ++it)
		m_virtserv_vec.back().m_server_name.push_back(*it);
	if (*it != ";")
	{
		throw std::runtime_error("Config file error: missing ; after root directive");
	}
}

void ConfigParser::_parseIndex(ConfigParser::configstream_iterator& it)
{
	for (++it; not it.is_delim() ; ++it)
		m_virtserv_vec.back().m_server_name.push_back(*it);
	if (*it != ";")
	{
		throw std::runtime_error("Config file error: missing ; after index directive");
	}
}

#undef BEFORE 
#undef AFTER

} // namespace ft
