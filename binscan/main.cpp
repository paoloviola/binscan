#include <iostream>
#include <fstream>
#include <string>

#include "binscanner.h"

void SearchPath(std::string* path, int argc, char* argv[])
{
	if (argc > 1)
		*path = argv[1];
	else 
	{
		std::cout << "File to be scanned: ";
		std::cin >> *path;
	}
}

int main(int argc, char* argv[])
{
	std::string inPath;
	SearchPath(&inPath, argc, argv);

	// Importing file
	std::ifstream input(inPath, std::ifstream::binary);
	if (!input.good()) 
	{
		std::cerr << "File not found!\n";
		return 1;
	}

	// Getting filesize
	input.seekg(0, std::ifstream::end);
	uint64_t datac = (uint64_t)input.tellg() + 1;
	input.seekg(0, std::ifstream::beg);

	std::cout << "Loading " << datac << " bytes...\n";
	
	// Importing filedata...
	uint8_t* data = new uint8_t[datac];
	input.read((char*)data, datac);
	input.close();

	std::cout << "Scanning...\n\n";
	BinScanner scanner(datac, data);
	
	uint32_t count = 0;
	while (scanner.HasNextEntry())
	{
		BinEntry entry;
		if (!scanner.NextEntry(&entry)) break;
		
		std::string outPath = "output#" + std::to_string(count++) + entry.ext;
		printf("Found Addr\(%d - %d\) Len\(%d\) -> %s\n", 
			entry.offset, entry.offset + entry.length, entry.length, outPath);

		std::ofstream output(outPath, std::ofstream::binary);
		output.write((char*)(entry.data + entry.offset), entry.length);
		output.close();
	}

	return 0;
}

