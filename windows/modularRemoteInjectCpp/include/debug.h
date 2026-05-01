// include/debug.h
#pragma once
#include <cstdio>

#ifdef ENABLE_DEBUG
	#define DBG(fmt, ...) printf("[*] " fmt "\n", ##__VA_ARGS__)
#else
	#define DBG(fmt, ...) ((void)0)
#endif