#pragma once
#include <iostream>

struct BinEntry
{
	std::string ext;
	uint64_t offset;
	uint64_t length;
	uint8_t* data;
};

uint8_t Read8(uint8_t* addr);
uint16_t Read16(uint8_t* addr);
uint32_t Read32(uint8_t* addr);
uint64_t Read64(uint8_t* addr);

class BinScanner
{
public:
	BinScanner(uint64_t datac, uint8_t* data);

	bool HasNextEntry();
	bool NextEntry(BinEntry* entry);

	static bool FindZipEntry(BinEntry* entry, uint8_t* data, uint64_t offset, uint64_t datac);
	static bool FindClassEntry(BinEntry* entry, uint8_t* data, uint64_t offset, uint64_t datac);
private:
	uint64_t datac;
	uint8_t* data;
	uint64_t offset;
};

