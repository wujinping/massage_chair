#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>
#define UNUSED_PARAMETER(para)	do{}while(0);



#if defined(PRINT_FULL_MESSAGE)
#define dev_err(format, ...)		printf(format, ## __VA_ARGS__)
#define dev_info(format, ...)		printf(format, ## __VA_ARGS__)
#define dev_dbg(format, ...)		printf(format, ## __VA_ARGS__)
#else
#define dev_err(format, ...)		do{}while(0)
#define dev_info(format, ...)		do{}while(0)
#define dev_dbg(format, ...)		do{}while(0)
#endif

