#include "binscanner.h"

const uint8_t LOCAL_FILE_HEADER[] = { 0x50, 0x4b, 0x03, 0x04 };
const uint8_t END_OF_CENTRAL_DIR_HEADER[] = { 0x50, 0x4b, 0x05, 0x06 };

bool CheckLFHeader(uint8_t* data, uint64_t offset, uint64_t datac)
{
	const uint8_t len = sizeof(LOCAL_FILE_HEADER) / sizeof(*LOCAL_FILE_HEADER);
	for (uint8_t i = 0; i < len; i++)
	{
		if (offset + i >= datac) return false;
		if (LOCAL_FILE_HEADER[i] != Read8(data + offset + i))
			return false;
	}
	return true;
}

bool CheckEOCDHeader(uint8_t* data, uint64_t offset, uint64_t datac)
{
	const uint8_t len = sizeof(END_OF_CENTRAL_DIR_HEADER) / sizeof(*END_OF_CENTRAL_DIR_HEADER);
	for (uint8_t i = 0; i < len; i++)
	{
		if (offset + i >= datac) return false;
		if (END_OF_CENTRAL_DIR_HEADER[i] != Read8(data + offset + i))
			return false;
	}
	return true;
}

bool BinScanner::FindZipEntry(BinEntry* entry, uint8_t* data, uint64_t offset, uint64_t datac)
{
	if (!CheckLFHeader(data, offset, datac))
		return false;

	uint64_t end;
	bool found = false;
	for (end = offset; end < datac; end++)
	{
		if (end >= datac) return false;
		if (CheckEOCDHeader(data, end, datac))
		{
			found = true;
			break;
		}
	}

	if (!found) return false;
	if ((end += 20) + 2 >= datac)
		return false;

	end += 2 + (uint64_t)Read16(data + end);
	if (end >= datac) return false;

	entry->ext = ".zip";
	entry->offset = offset;
	entry->length = end - offset;
	entry->data = data;
	return true;
}

