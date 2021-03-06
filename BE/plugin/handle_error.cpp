#include <iostream>
#include <cstdlib>

#include <errno.h>

#include <iostream>

#include "handle_error.h"

#define DEBUG 1

void dev_log(const char *str)
{
#ifdef DEBUG
	std::cerr << str << std::endl;
#endif
}

void handle_error(const char *str)
{
#ifdef DEBUG
	std::cerr << str << std::endl;
#endif
}

void handle_syscall_error(const char *str)
{
#ifdef DEBUG
	std::cerr << str << std::endl;
#endif
}

void handle_fatal_error(const char *str)
{
#ifdef DEBUG
	std::cerr << str << std::endl;
#endif
	std::exit(-1);
}

void handle_fatal_syscall_error(const char *str)
{
#ifdef DEBUG
	std::cerr << str << std::endl;
#endif
	std::exit(-1);
}

