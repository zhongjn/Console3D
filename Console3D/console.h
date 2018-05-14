#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <io.h>

#define WIN32_LEAN_AND_MEAN

namespace console3d {
	namespace console {
		void initialize(short height, short width);
		void set_char(short i, short j, char c);
		void render();
	}
}
#endif