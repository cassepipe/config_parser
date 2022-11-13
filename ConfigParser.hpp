#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP


#include <algorithm>
#include <assert.h>
#include <fstream>
#include <iterator>
#include <map>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <vector>

#include "VirtServ.hpp"
#include "less_sockaddr.hpp"

namespace ft
{
	using std::vector;
	using std::map;

class ConfigParser
{
  public:

	typedef vector<VirtServ>								VirtServVector;
	typedef vector<VirtServ*>								VirtServPtrVector;

	typedef std::map<sockaddr_in, VirtServPtrVector >		VirtServMap;
	typedef VirtServMap::iterator							iterator;

#ifndef CONFIG_DELIMITER_SET
# define CONFIG_DELIMITER_SET ";{}"
#endif

	class configstream_iterator
	{
		public:

		configstream_iterator(std::ifstream& config_stream) : m_ch()
			,m_delim_set(CONFIG_DELIMITER_SET) 
			,m_buffer() 
			,m_stream(config_stream)
			,m_is_delim(false)
		{
			m_stream >> std::noskipws;
			advance();
		}

		const std::string&		operator*() { return m_buffer; };
		const std::string*		operator->() { return &m_buffer; };

		configstream_iterator&	operator++() { advance(); return *this; }

		bool is_delim() { return m_is_delim; }

		private:

		char 		 			m_ch;
		std::string	 			m_delim_set;
		std::string	 			m_buffer;
		std::ifstream&			m_stream;
		bool					m_is_delim;

		void advance()
		{
			static bool send_delim = false;

			if (m_stream.eof())
			{
				m_buffer = "";
				m_is_delim = false;
				return;
			}
			// Must send a delim
			if (send_delim)
			{
				m_buffer = m_ch;
				send_delim = false;
				m_is_delim = true;
				return;
			}
			//Skip whitespace
			for (m_stream >> m_ch ; m_stream and std::isspace(m_ch) ; m_stream >> m_ch)
			{ }
			// Is a delimiter ?
			if (m_delim_set.find(m_ch) != std::string::npos)
			{
				m_buffer = m_ch;
				m_is_delim = true;
				return;
			}
			else
			{
				// Look for token
				m_is_delim = false;
				for (; m_stream and not std::isspace(m_ch); m_stream >> m_ch)
				{
					if (m_delim_set.find(m_ch) != std::string::npos)
					{
						send_delim = true; // m_ch will hold the delimiter
						return;
					}
					m_buffer += m_ch;
				}
			}
		}
	};
#undef CONFIG_DELIMITER_SET

	std::vector<VirtServ>								m_virtserv_vec;
	VirtServMap											m_virtserv_map;

	/* Ctor */ ConfigParser(const char* config);

	iterator	begin()	{	return m_virtserv_map.begin();	}
	iterator	end()	{	return m_virtserv_map.end();	}
	
	const vector<VirtServ*>&	operator[](const VirtServMap::key_type& key) const
	{
		return (m_virtserv_map.at(key));
	}

  private:
	typedef struct token_dispatch
	{
		std::string token;
		void (ConfigParser::*parse_method)(configstream_iterator& it);
	} token_dispatch_t;

	static const token_dispatch_t                m_block_dispatch_table[];
	static const token_dispatch_t                m_server_block_dispatch_table[];
	static const std::vector< token_dispatch_t > m_block_dispatch_vec;
	static const std::vector< token_dispatch_t > m_server_block_dispatch_vec;

	void _parseConfig(const char* config);
	void _match(ConfigParser::configstream_iterator it, const std::vector< token_dispatch_t >& dispatch_table, bool throw_on_not_found);
	void _parseServerBlock(ConfigParser::configstream_iterator& it);
	void _parseListen(ConfigParser::configstream_iterator& it);
	void _parseRoot(ConfigParser::configstream_iterator& it);
	void _parseIndex(ConfigParser::configstream_iterator& it);
	void _parseServerName(ConfigParser::configstream_iterator& it);
};

} // namespace ft

#endif /* CONFIGPARSER_HPP */
