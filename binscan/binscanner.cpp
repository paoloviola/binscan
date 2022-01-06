#include "binscanner.h"

uint8_t Read8(uint8_t* addr)
{ // convert 1-byte value
	return addr[0];
}

uint16_t Read16(uint8_t* addr)
{ // convert 2-byte value
	return ((uint16_t)addr[0] << 8) 
		| ((uint16_t)addr[1] << 0);
}

uint32_t Read32(uint8_t* addr)
{ // convert 4-byte value
	return ((uint32_t)addr[0] << 24)
		| ((uint32_t)addr[1] << 16)
		| ((uint32_t)addr[2] << 8)
		| ((uint32_t)addr[3] << 0);
}

uint64_t Read64(uint8_t* addr)
{ // convert 8-byte value
	return ((uint64_t)addr[0] << 56)
		| ((uint64_t)addr[1] << 48)
		| ((uint64_t)addr[2] << 40)
		| ((uint64_t)addr[3] << 32)
		| ((uint64_t)addr[4] << 24)
		| ((uint64_t)addr[5] << 16)
		| ((uint64_t)addr[6] << 8)
		| ((uint64_t)addr[7] << 0);
}

BinScanner::BinScanner(uint64_t datac, uint8_t* data)
{
	this->datac = datac;
	this->data = data;
	this->offset = 0;
}

bool BinScanner::HasNextEntry()
{
	return this->offset < this->datac;
}

bool BinScanner::NextEntry(BinEntry* entry)
{
	for (; this->offset < this->datac; this->offset++) 
	{
		if (FindZipEntry(entry, this->data, this->offset, this->datac)
			|| FindClassEntry(entry, this->data, this->offset, this->datac))
		{
			this->offset += entry->length;
			return true;
		}
	}

	return false;
}

