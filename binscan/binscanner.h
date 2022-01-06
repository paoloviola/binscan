#pragma once
#include <iostream>

struct BinEntry
{
	std::string ext;
	uint64_t offset;
	uint64_t length;
	uint8_t* data;
};

bool FindZipEntry(BinEntry* entry, uint8_t* data, uint64_t offset, uint64_t datac);

class BinScanner
{
public:
	BinScanner(uint64_t datac, uint8_t* data);

	bool HasNextEntry();
	bool NextEntry(BinEntry* entry);

private:
	uint64_t datac;
	uint8_t* data;
	uint64_t offset;
};

