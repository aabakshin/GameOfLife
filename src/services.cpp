#ifndef SERVICES_CPP
#define SERVICES_CPP

#include "../includes/services.hpp"


const std::string getResourcePath(const char* str)
{
	std::string result;

	result.append(str);
	result.append("/");

	return result;
}

#endif
