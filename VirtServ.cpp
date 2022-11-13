#include "VirtServ.hpp"

#include <sstream>
#include <vector>
#include <string>

std::ostream& operator<<(std::ostream& os, std::vector<std::string>& vec)
{
	for (size_t i = 0; i < vec.size() ; ++i)
	{
		os << vec[i] << "\t";
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const VirtServ::RouteOptions& routeinfo)
{
	os << "\t\t-------------------------\n";
	os	<< "\t\turi:\t\t" ;
	os << routeinfo.m_uri;
	os << '\n';
	os	<< "\t\troot:\t\t" ;
	os << routeinfo.m_root;
	os << '\n';
	os	<< "\t\tautoindex:\t";
	os << routeinfo.m_autoindex;
	os << '\n';
	os << "\t\t-------------------------\n";

	return os;
}

std::ostream& operator<<(std::ostream& os, const VirtServ& servinfo)
{
	os	<< "VIRTUAL SERVER: \n";
	os	<< "\tserver_name:\t";
	for (size_t i = 0; i < servinfo.m_server_name_vec.size(); ++i)
	{
		os << servinfo.m_server_name_vec[i] << '\t';
	}
	os << '\n';
	os	<< "\troot:\t\t" ;
	os << servinfo.m_root << '\t';
	os << '\n';
	os	<< "\tindex:\t\t";
	for (size_t i = 0; i < servinfo.m_index_vec.size(); ++i)
	{
		os << servinfo.m_index_vec[i] << '\t';
	}
	os << '\n';
	os	<< "\tlocation blocks:\n";
	for (size_t i = 0; i < servinfo.m_routes_vec.size(); ++i)
	{
		os << servinfo.m_routes_vec[i] << '\t';
	}
	os << '\n';

	return os;
}


