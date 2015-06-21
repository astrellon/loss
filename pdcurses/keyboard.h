#pragma once

#ifdef _WIN32
#include <conio.h>
#include <Windows.h>
#else
#include <sgtty.h>

unsigned int _kbhit()
{
	static unsigned int i;
	ioctl(0, FIONREAD, &i);
	return i;
}

#endif