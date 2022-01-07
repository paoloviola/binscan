#include "binscanner.h"
#include <unordered_map>

const uint8_t MAGIC_HEADER[] = { 0xCA, 0xFE, 0xBA, 0xBE };

// cp tags
const uint8_t CONSTANT_Class = 7;
const uint8_t CONSTANT_Fieldref = 9;
const uint8_t CONSTANT_Methodref = 10;
const uint8_t CONSTANT_InterfaceMethodref = 11;
const uint8_t CONSTANT_String = 8;
const uint8_t CONSTANT_Integer = 3;
const uint8_t CONSTANT_Float = 4;
const uint8_t CONSTANT_Long = 5;
const uint8_t CONSTANT_Double = 6;
const uint8_t CONSTANT_NameAndType = 12;
const uint8_t CONSTANT_Utf8 = 1;
const uint8_t CONSTANT_MethodHandle = 15;
const uint8_t CONSTANT_MethodType = 16;
const uint8_t CONSTANT_Dynamic = 17;
const uint8_t CONSTANT_InvokeDynamic = 18;
const uint8_t CONSTANT_Module = 19;
const uint8_t CONSTANT_Package = 20;
// cp tags

// Map of all constant pool types with size
const std::unordered_map<uint8_t, uint16_t> constantSize({
	{ CONSTANT_Class, 2 },
	{ CONSTANT_Fieldref, 4 },
	{ CONSTANT_Methodref, 4 },
	{ CONSTANT_InterfaceMethodref, 4 },
	{ CONSTANT_String, 2 },
	{ CONSTANT_Integer, 4 },
	{ CONSTANT_Float, 4 },
	{ CONSTANT_Long, 8 },
	{ CONSTANT_Double, 8 },
	{ CONSTANT_NameAndType, 4 },
	//{ CONSTANT_Utf8, var }, // Size varies
	{ CONSTANT_MethodHandle, 3 },
	{ CONSTANT_MethodType, 2 },
	{ CONSTANT_Dynamic, 4 },
	{ CONSTANT_InvokeDynamic, 4 },
	{ CONSTANT_Module, 2 },
	{ CONSTANT_Package, 2 }
});

// Class structure
// https://docs.oracle.com/javase/specs/jvms/se16/html/jvms-4.html
struct cp_info
{
	uint8_t tag;
	uint8_t* info;
};

struct attribute_info
{
	uint16_t attribute_name_index;
	uint32_t attribute_length;
	uint8_t* info;
};

struct field_info
{
	uint16_t access_flags;
	uint16_t name_index;
	uint16_t descriptor_index;
	uint16_t attributes_count;
	attribute_info* attributes;
};

struct method_info
{
	uint16_t access_flags;
	uint16_t name_index;
	uint16_t descriptor_index;
	uint16_t attributes_count;
	attribute_info* attributes;
};

struct ClassFile
{
	uint32_t magic;
	uint16_t minor_version;
	uint16_t major_version;
	uint16_t constant_pool_count;
	cp_info* constant_pool;
	uint16_t access_flags;
	uint16_t this_class;
	uint16_t super_class;
	uint16_t interfaces_count;
	uint16_t* interfaces;
	uint16_t fields_count;
	field_info* fields;
	uint16_t methods_count;
	method_info* methods;
	uint16_t attributes_count;
	attribute_info* attributes;
};
// Class structure

bool CheckMagicHeader(uint8_t* data, uint64_t offset, uint64_t datac)
{
	// Length of MAGIC_HEADER
	const uint8_t len = sizeof(MAGIC_HEADER) / sizeof(*MAGIC_HEADER);
	for (uint8_t i = 0; i < len; i++)
	{
		if (offset + i >= datac) return false;
		if (MAGIC_HEADER[i] != Read8(data + offset + i))
			return false;
	}
	return true;
}

uint64_t GetCPInfo(cp_info& cp, uint8_t* data, uint64_t offset, uint64_t datac)
{
	if (offset + 1 >= datac) return 0;
	cp.tag = Read8(data + offset);

	uint16_t len = 1;
	if (cp.tag == CONSTANT_Utf8)
	{
		if (offset + 3 >= datac) return 0;
		len += 2 + Read16(data + offset + 1);
	}
	else
	{
		if (constantSize.find(cp.tag) == constantSize.end()) return 0;
		len += constantSize.at(cp.tag);
	}
	//cp.info = new uint8_t[len - 1];

	cp.info = new uint8_t[0]; // don't allocate unnecessary memory
	return len;
}

uint64_t GetAttributeInfo(attribute_info& ai, uint8_t* data, uint64_t offset, uint64_t datac)
{
	if (offset + 2 >= datac) return 0;
	ai.attribute_name_index = Read16(data + offset);
	if (offset + 6 >= datac) return 0;
	ai.attribute_length = Read32(data + offset + 2);
	//ai.info = new uint8_t[ai.attribute_length];

	ai.info = new uint8_t[0]; // don't allocate unnecessary memory
	return 6 + (uint64_t)ai.attribute_length;
}

uint64_t GetFieldInfo(field_info& fi, uint8_t* data, uint64_t offset, uint64_t datac)
{
	uint64_t end = offset;
	if (end + 2 >= datac) return 0;
	fi.access_flags = Read16(data + end);
	if (end + 4 >= datac) return 0;
	fi.name_index = Read16(data + end + 2);
	if (end + 6 >= datac) return 0;
	fi.descriptor_index = Read16(data + end + 4);
	if (end + 8 >= datac) return 0;
	fi.attributes_count = Read16(data + end + 6);
	end += 8;

	fi.attributes = new attribute_info[fi.attributes_count];
	for (uint16_t j = 0; j < fi.attributes_count; j++)
	{
		attribute_info ai;
		uint64_t len = GetAttributeInfo(ai, data, end, datac);
		if(len == 0 || (end += len) >= datac) return 0;
		fi.attributes[j] = ai;
	}
	return end - offset;
}

uint64_t GetMethodInfo(method_info& mi, uint8_t* data, uint64_t offset, uint64_t datac)
{
	uint64_t end = offset;
	if (end + 2 >= datac) return 0;
	mi.access_flags = Read16(data + end);
	if (end + 4 >= datac) return 0;
	mi.name_index = Read16(data + end + 2);
	if (end + 6 >= datac) return 0;
	mi.descriptor_index = Read16(data + end + 4);
	if (end + 8 >= datac) return 0;
	mi.attributes_count = Read16(data + end + 6);
	end += 8;

	mi.attributes = new attribute_info[mi.attributes_count];
	for (uint16_t j = 0; j < mi.attributes_count; j++)
	{
		attribute_info ai;
		uint64_t len = GetAttributeInfo(ai, data, end, datac);
		if (len == 0 || (end += len) >= datac) return 0;
		mi.attributes[j] = ai;
	}
	return end - offset;
}

bool BinScanner::FindClassEntry(BinEntry* entry, uint8_t* data, uint64_t offset, uint64_t datac)
{
	if (!CheckMagicHeader(data, offset, datac))
		return false;

	uint64_t end = offset;
	{ // Load class file
		ClassFile file;
		if (end >= datac) return false;
		file.magic = Read32(data + end);

		if (end + 6 >= datac) return false;
		file.minor_version = Read16(data + end + 4);
		
		if (end + 8 >= datac) return false;
		file.major_version = Read16(data + end + 6);

		if (end + 10 >= datac) return false;
		file.constant_pool_count = Read16(data + end + 8);
		end += 10;

		file.constant_pool = new cp_info[file.constant_pool_count];
		for (uint16_t i = 0; i < file.constant_pool_count - 1; i++)
		{
			cp_info cp;
			uint64_t len = GetCPInfo(cp, data, end, datac);
			if (len == 0 || (end += len) >= datac) return false;
			file.constant_pool[i] = cp;
			
			// The constant_pool index n+1 must be valid but is considered unusable.
			if (cp.tag == CONSTANT_Long
				|| cp.tag == CONSTANT_Double) i++;
		}

		if (end + 2 >= datac) return false;
		file.access_flags = Read16(data + end);
		
		if (end + 4 >= datac) return false;
		file.this_class = Read16(data + end + 2);

		if (end + 6 >= datac) return false;
		file.super_class = Read16(data + end + 4);

		if (end + 8 >= datac) return false;
		file.interfaces_count = Read16(data + end + 6);
		end += 8;

		file.interfaces = new uint16_t[file.interfaces_count];
		for (uint16_t i = 0; i < file.interfaces_count; i++)
		{
			uint64_t off = end + 2 * (uint64_t)i;
			if (off + 2 >= datac) return false;
			file.interfaces[i] = Read16(data + off);
		}
		end += (uint64_t)file.interfaces_count * 2;

		if (end + 2 >= datac) return false;
		file.fields_count = Read16(data + end);
		end += 2;

		file.fields = new field_info[file.fields_count];
		for (uint16_t i = 0; i < file.fields_count; i++)
		{
			field_info fi;
			uint64_t len = GetFieldInfo(fi, data, end, datac);
			if (len == 0 || (end += len) >= datac) return false;
			file.fields[i] = fi;
		}

		if (end + 2 >= datac) return false;
		file.methods_count = Read16(data + end);
		end += 2;

		file.methods = new method_info[file.methods_count];
		for (uint16_t i = 0; i < file.methods_count; i++)
		{
			method_info mi;
			uint64_t len = GetMethodInfo(mi, data, end, datac);
			if (len == 0 || (end += len) >= datac) return false;
			file.methods[i] = mi;
		}
		
		if (end + 2 >= datac) return false;
		file.attributes_count = Read16(data + end);
		end += 2;

		file.attributes = new attribute_info[file.attributes_count];
		for (uint16_t i = 0; i < file.attributes_count; i++)
		{
			attribute_info ai;
			uint64_t len = GetAttributeInfo(ai, data, end, datac);
			if (len == 0 || (end += len) >= datac) return false;
			file.attributes[i] = ai;
		}
	}
	entry->ext = ".class";
	entry->offset = offset;
	entry->length = end - offset;
	entry->data = data;
	return true;
}

