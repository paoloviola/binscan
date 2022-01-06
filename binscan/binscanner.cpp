#include "binscanner.h"

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
		if (FindZipEntry(entry, this->data, this->offset, this->datac))
		{
			this->offset += entry->length;
			return true;
		}
	}

	return false;
}

