// win32test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "log.h"


#define AMIOS_LOGLVL_DEBUG     1
#define AMIOS_LOGLVL_INFO      2
#define AMIOS_LOGLVL_WARNING   3
#define AMIOS_LOGLVL_ERROR     4
#define LOG_SIZE_MAX 512

#define DE AMIOS_LOGLVL_DEBUG,__FILE__,__FUNCTION__,__LINE__


void WriteFile(char *pFileName, char *pBuf, int bLen)
{
    FILE *pFile = fopen(pFileName,"a+");
    if (!pFile)
        return;
    fwrite(pBuf, bLen, 1, pFile);
    fclose(pFile);
    return ;
}

void log(int loglvl, char *file,  const char *func,int line, 
                     const char *format, ...)
{

    char logBuf[LOG_SIZE_MAX];
    char logStr[LOG_SIZE_MAX];
    char *logLvl = "DEBUG";
    char *pos = NULL;
    int i = 0;
    int len = 0;
    va_list ap;

    if (!file || !func) {
        return;
    }
    memset(logBuf, 0, LOG_SIZE_MAX);
    memset(logStr, 0, LOG_SIZE_MAX);

    switch (loglvl) {
    case AMIOS_LOGLVL_DEBUG:
        logLvl = "DEBUG";
        break;
    case AMIOS_LOGLVL_INFO:
        logLvl = "INFO";
        break;
    case AMIOS_LOGLVL_WARNING:
        logLvl = "WARNING";
        break;
    case AMIOS_LOGLVL_ERROR:
        logLvl = "ERROR";
        break;
    default:
        return;
        break;
    }

    /* ログ文字列展�?*/
    va_start(ap, format);
    _vsnprintf(logStr, LOG_SIZE_MAX, format, ap);
    va_end(ap);
    logStr[LOG_SIZE_MAX - 1] = '\0';

    /* ファイル名のパスを除�?*/
    len = strlen(file);
    for (i = len - 1; i >= 0; i--) {
        pos = &file[i];
        if (*pos == '\\') {
            pos++;
            break;
        }
    }

    /* システム時間を得�?*/
    SYSTEMTIME sys; 
    GetLocalTime( &sys ); 

    sprintf(logBuf, "%02d:%02d:%02d.%03ld %s %s %d %s %s\n",
        sys.wHour,                                /* hour */
        sys.wMinute,                              /* min */
        sys.wSecond,                              /* sec */
        sys.wMilliseconds,                        /* msec */
        logLvl,
        pos,
        line,
        func,
        logStr
        );
    printf( "%s\n", logBuf);
    WriteFile("c:\\123.txt", logBuf, strlen(logBuf));
    return;
}
