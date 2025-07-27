#include "msgserialize.hpp"
#include <string.h>

bool WriteChar(const char& val, char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(char)) > max_len) return false;

	*(char*)(buffer + *out_len) = val;
	*out_len += sizeof(char);

	return true;
}

bool WriteUChar(const unsigned char& val, char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(unsigned char)) > max_len) return false;

	*(unsigned char*)(buffer + *out_len) = val;
	*out_len += sizeof(unsigned char);	

	return true;
}

bool WriteShort(const short& val, char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(short)) > max_len) return false;

	*(short*)(buffer + *out_len) = val;
	*out_len += sizeof(short);

	return true;
}

bool WriteUShort(const unsigned short& val, char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(unsigned short)) > max_len) return false;

	*(unsigned short*)(buffer + *out_len) = val;
	*out_len += sizeof(unsigned short);

	return true;
}

bool WriteInt(const int& val, char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(int)) > max_len) return false;

	*(int*)(buffer + *out_len) = val;
	*out_len += sizeof(int);

	return true;
}

bool WriteUInt(const unsigned int& val, char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(unsigned int)) > max_len) return false;

	*(unsigned int*)(buffer + *out_len) = val;
	*out_len += sizeof(unsigned int);

	return true;
}

bool WriteLL(const long long& val, char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(long long)) > max_len) return false;

	*(long long*)(buffer + *out_len) = val;
	*out_len += sizeof(long long);

	return true;
}

bool WriteFloat(const float& val, char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(float)) > max_len) return false;

	*(float*)(char*)(buffer + *out_len) = val;
	*out_len += sizeof(float);

	return true;
}

bool WriteDouble(const double& val, char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(double)) > max_len) return false;

	*(double*)(char*)(buffer + *out_len) = val;
	*out_len += sizeof(double);

	return true;
}

bool WriteStrN(const char* str, int str_len, char* buffer, int max_len, int* out_len)
{
	if (str_len < 0 || *out_len + str_len > max_len) return false;

	if (str_len == 0) return true;

	memcpy(buffer + *out_len, str, str_len);
	*out_len += str_len;

	return true;
}

bool ReadChar(char& val, const char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(char)) > max_len) return false;

	val = *(char*)(buffer + *out_len);
	*out_len += sizeof(char);

	return true;
}

bool ReadUChar(unsigned char& val, const char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(unsigned char)) > max_len) return false;

	val = *(unsigned char*)(buffer + *out_len);
	*out_len += sizeof(unsigned char);

	return true;
}

bool ReadShort(short& val, const char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(short)) > max_len) return false;

	val = *(short*)(buffer + *out_len);
	*out_len += sizeof(short);

	return true;
}

bool ReadUShort(unsigned short& val, const char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(unsigned short)) > max_len) return false;

	val = *(unsigned short*)(buffer + *out_len);
	*out_len += sizeof(unsigned short);

	return true;
}

bool ReadInt(int& val, const char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(int)) > max_len) return false;

	val = *(int*)(buffer + *out_len);
	*out_len += sizeof(int);

	return true;
}

bool ReadUInt(unsigned int& val, const char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(unsigned int)) > max_len) return false;

	val = *(unsigned int*)(buffer + *out_len);
	*out_len += sizeof(unsigned int);

	return true;
}

bool ReadLL(long long& val, const char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(long long)) > max_len) return false;

	val = *(long long*)(buffer + *out_len);
	*out_len += sizeof(long long);

	return true;
}

bool ReadFloat(float& val, const char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(float)) > max_len) return false;

	val = *(float*)(char*)(buffer + *out_len);
	*out_len += sizeof(float);

	return true;
}

bool ReadDouble(double& val, const char* buffer, int max_len, int* out_len)
{
	if (*out_len + static_cast<int>(sizeof(double)) > max_len) return false;

	val = *(double*)(char*)(buffer + *out_len);
	*out_len += sizeof(double);

	return true;
}

bool ReadStrN(char* str, int read_len, int str_max_len, const char* buffer, int max_buffer_len, int* out_len)
{
	if (read_len < 0 || *out_len + read_len > max_buffer_len) return false;
	if (read_len > str_max_len) return false;

	if (read_len == 0) return true;

	memcpy(str, buffer + *out_len, read_len);
	*out_len += read_len;

	return true;
}

bool ReadString(std::string& str, int read_len, const char* buffer, int max_buffer_len, int* out_len)
{
	if (read_len < 0 || *out_len + read_len > max_buffer_len) return false;

	str.assign(buffer + *out_len, read_len);
	*out_len += read_len;

	return true;
}
