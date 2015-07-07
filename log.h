// win32test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h> 
#include <stdio.h>


#define AMIOS_LOGLVL_DEBUG     1
#define AMIOS_LOGLVL_INFO      2
#define AMIOS_LOGLVL_WARNING   3
#define AMIOS_LOGLVL_ERROR     4
#define LOG_SIZE_MAX 512

#define DE AMIOS_LOGLVL_DEBUG,__FILE__,__FUNCTION__,__LINE__

void log(int loglvl, char *file,  const char *func,int line, const char *format, ...);


