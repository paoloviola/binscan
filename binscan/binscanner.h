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

	/// <summary>
	/// Checks if there is any space in memory for a possible new entry.
	/// </summary>
	bool HasNextEntry();

	/// <summary>
	/// Scans for the next entry.
	/// </summary>
	/// <returns>true if an entry was found, otherwise false</returns>
	bool NextEntry(BinEntry* entry);
private:
	uint64_t datac;
	uint8_t* data;
	uint64_t offset;

	/// <summary>
	/// Checks if data at an specific offset is a ZipEntry
	/// </summary>
	/// <returns>true if an entry was successfully parsed, otherwise false</returns>
	static bool FindZipEntry(BinEntry* entry, uint8_t* data, uint64_t offset, uint64_t datac);

	/// <summary>
	/// Checks if data at an specific offset is a ClassEntry
	/// </summary>
	/// <returns>true if an entry was successfully parsed, otherwise false</returns>
	static bool FindClassEntry(BinEntry* entry, uint8_t* data, uint64_t offset, uint64_t datac);
};

