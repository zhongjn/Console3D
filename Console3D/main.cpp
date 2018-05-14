#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "console.h"

using namespace console3d;

int main(int argc, char **argv) {

	console::initialize(30, 30);

	int a = 0;
	while (true) {
		a++;
		for (int i = 0; i < 30; i++) {
			for (int j = 0; j < 30; j++) {
				console::set_char(i, j, '*'); //(i + j + a) % 32+32
			}
		}
		console::render();
		Sleep(15);
	}

	system("pause");
	return EXIT_SUCCESS;
}