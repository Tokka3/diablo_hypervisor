#pragma once
#include <cstdarg>
#include "includes.h"

void log_debug(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vDbgPrintExWithPrefix("[*] ", DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, fmt, args);
	va_end(args);
}