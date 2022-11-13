#include "VirtServ.hpp"

#include <sstream>
#include <vector>
#include <string>

std::ostream& operator<<(std::ostream& os, std::vector<std::string>& vec)
{
	for (int i = 0; i < vec.size() ; ++i)
	{
		os << vec[i] << "\t";
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, const VirtServ& servinfo)
{
	os	<< "VIRTUAL SERVER: \n" 
		<< "\tserver_name:\t" << servinfo.m_server_name[0] << '\n'
		<< "\troot:\t" << servinfo.m_root[0] << '\n'
		<< "\tindex:\t" << servinfo.m_index[0] << '\n';
	return os;
}


