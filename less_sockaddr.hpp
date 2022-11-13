#ifndef LESS_SOCKADDR_HPP
#define LESS_SOCKADDR_HPP

#include <netinet/in.h>
#include <functional>

namespace std
{

template<>
struct less< sockaddr_in >
	//: public std::binary_function< sockaddr_in, sockaddr_in, bool>
{
	bool operator()(const sockaddr_in& s1, const sockaddr_in& s2) const
	{
		return (
				(s1.sin_addr.s_addr < s2.sin_addr.s_addr and s2.sin_addr.s_addr > s1.sin_addr.s_addr)
				or 	s1.sin_port < s2.sin_port
				);
	}
};

template<>
struct less< const sockaddr_in >
	//: public std::binary_function< const sockaddr_in, const sockaddr_in, bool>
{
	bool operator()(const sockaddr_in& s1, const sockaddr_in& s2) const
	{
		return (
				(s1.sin_addr.s_addr < s2.sin_addr.s_addr and s2.sin_addr.s_addr > s1.sin_addr.s_addr)
				or 	s1.sin_port < s2.sin_port
				);
	}
};
} // namespace std

#endif /* LESS_SOCKADDR_HPP */
