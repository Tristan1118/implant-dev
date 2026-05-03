// include/debug.h
#pragma once
#include <cstdio>

#ifdef ENABLE_DEBUG
#define DBG(fmt, ...) printf("[*] [%s] " fmt "\n", \
    (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__), ##__VA_ARGS__)
#else
	#define DBG(fmt, ...) ((void)0)
#endif