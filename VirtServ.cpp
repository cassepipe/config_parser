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

std::ostream& operator<<(std::ostream& os, const VirtServ& servinfo)
{
	os	<< "VIRTUAL SERVER: \n";
	os	<< "\tserver_name:\t";
	for (size_t i = 0; i < servinfo.m_server_name.size(); ++i)
	{
		os << servinfo.m_server_name[i] << '\t';
	}
	os << '\n';
	os	<< "\troot:\t" ;
	for (size_t i = 0; i < servinfo.m_server_name.size(); ++i)
	{
		os << servinfo.m_server_name[i] << '\t';
	}
	os << '\n';
	os	<< "\tindex:\t";
	for (size_t i = 0; i < servinfo.m_server_name.size(); ++i)
	{
		os << servinfo.m_server_name[i] << '\t';
	}
	os << '\n';
	return os;
}


