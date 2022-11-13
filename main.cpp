#include "ConfigParser.hpp"

using namespace ft;

int main(int ac, char **av)
{
	if (ac > 2)
		return -1;

	ConfigParser config(av[1]);
}
