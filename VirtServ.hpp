#ifndef VIRTSERV_HPP
#define VIRTSERV_HPP

#include <netinet/in.h>
#include <string>
#include <vector>

struct VirtServ
{
	struct RouteOptions 
	{
		RouteOptions(std::string uri) : m_uri(uri)
			, m_root()
			, m_autoindex(false)
		{}

		std::string m_uri;
		std::string m_root;
		bool m_autoindex;
	};

	std::vector<std::string> m_server_name_vec;
	std::string m_root;
	std::vector<std::string> m_index_vec;
	std::vector<sockaddr_in> m_sockaddr_vec;
	std::vector<RouteOptions> m_routes_vec;
};

std::ostream& operator<<(std::ostream& os, const VirtServ& servinfo);

#endif /* VIRTSERV_HPP */
