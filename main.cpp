#include <iostream>
#include <string>
#include <vector>

#include "xml2c.h"


int main(int argc, char* argv[]) {
	if (argc != 4) {
		std::cerr << "Usage: xml2c <input.xml> <output.c> <prefix>\n";
		return 1;
	}

	try {
		Xml2c xml2c;
		xml2c.Run(argv[1], argv[2], argv[3]);
		return 0;
	}
	catch (const std::exception& e) {
		std::cerr << "Error! " << e.what() << "\n";
		return 1;
	};
}
