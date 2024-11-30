#include "gui.h"

int main() {
	setupGui();
	while (true) {
		if (guiLoop())
			break;
	}
	cleanupGui();
	return 0;
}
