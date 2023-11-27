#pragma once

#include "../headers/includes.h"

void log_debug(const char* fmt, ...)
{
	DbgPrintEx(0, 0, fmt);
}

void log_success(const char* fmt, ...)
{
	
	DbgPrintEx(0, 0, fmt);
	
}