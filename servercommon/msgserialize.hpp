#ifndef __MSG_SERIALIZE_HPP__
#define __MSG_SERIALIZE_HPP__

#include <string>
#include "common/tlvprotocol.h"

class IMessageSerializer
{
public:
	virtual ~IMessageSerializer() {};

	virtual int GetMessageType() const = 0;

	virtual bool Serialize(char* buffer, int buffer_size, int* out_length) = 0;
};

class IMessageDeserializer
{
public:
	virtual ~IMessageDeserializer() {};

	virtual bool Deserialize(const char* data, int data_length, int* out_offset) = 0;
};

bool WriteChar(const char& val, char* buffer, int max_len, int* out_len);
bool WriteUChar(const unsigned char& val, char* buffer, int max_len, int* out_len);
bool WriteShort(const short& val, char* buffer, int max_len, int* out_len);
bool WriteUShort(const unsigned short& val, char* buffer, int max_len, int* out_len);
bool WriteInt(const int& val, char* buffer, int max_len, int* out_len);
bool WriteUInt(const unsigned int& val, char* buffer, int max_len, int* out_len);
bool WriteLL(const long long& val, char* buffer, int max_len, int* out_len);
bool WriteFloat(const float& val, char* buffer, int max_len, int* out_len);
bool WriteDouble(const double& val, char* buffer, int max_len, int* out_len);
bool WriteStrN(const char* str, int str_len, char* buffer, int max_len, int* out_len);

bool ReadChar(char& val, const char* buffer, int max_len, int* out_len);
bool ReadUChar(unsigned char& val, const char* buffer, int max_len, int* out_len);
bool ReadShort(short& val, const char* buffer, int max_len, int* out_len);
bool ReadUShort(unsigned short& val, const char* buffer, int max_len, int* out_len);
bool ReadInt(int& val, const char* buffer, int max_len, int* out_len);
bool ReadUInt(unsigned int& val, const char* buffer, int max_len, int* out_len);
bool ReadLL(long long& val, const char* buffer, int max_len, int* out_len);
bool ReadFloat(float& val, const char* buffer, int max_len, int* out_len);
bool ReadDouble(double& val, const char* buffer, int max_len, int* out_len);
bool ReadStrN(char* str, int read_len, int str_max_len, const char* buffer, int max_buffer_len, int* out_len);
bool ReadString(std::string& str, int read_len, const char* buffer, int max_buffer_len, int* out_len);

#define WRITE_BEGIN(msg_type) WriteBegin((msg_type), buffer, buffer_size, out_length)
#define WRITE_CHAR(value) WriteChar((value), buffer, buffer_size, out_length)
#define WRITE_UCHAR(value) WriteUChar((value), buffer, buffer_size, out_length)
#define WRITE_SHORT(value) WriteShort((value), buffer, buffer_size, out_length)
#define WRITE_USHORT(value) WriteUShort((value), buffer, buffer_size, out_length)
#define WRITE_INT(value) WriteInt((value), buffer, buffer_size, out_length)
#define WRITE_UINT(value) WriteUInt((value), buffer, buffer_size, out_length)
#define WRITE_LL(value) WriteLL((value), buffer, buffer_size, out_length)
#define WRITE_FLOAT(value) WriteFloat((value), buffer, buffer_size, out_length)
#define WRITE_DOUBLE(value) WriteDouble((value), buffer, buffer_size, out_length)
#define WRITE_STRN(str, str_len) WriteStrN((str), (str_len), buffer, buffer_size, out_length)

#define READ_BEGIN(msg_type) ReadBegin((msg_type), data, data_length, out_offset)
#define READ_CHAR(value) ReadChar((value), data, data_length, out_offset)
#define READ_UCHAR(value) ReadUChar((value), data, data_length, out_offset)
#define READ_SHORT(value) ReadShort((value), data, data_length, out_offset)
#define READ_USHORT(value) ReadUShort((value), data, data_length, out_offset)
#define READ_INT(value) ReadInt((value), data, data_length, out_offset)
#define READ_UINT(value) ReadUInt((value), data, data_length, out_offset)
#define READ_LL(value) ReadLL((value), data, data_length, out_offset)
#define READ_FLOAT(value) ReadFloat((value), data, data_length, out_offset)
#define READ_DOUBLE(value) ReadDouble((value), data, data_length, out_offset)
#define READ_STRN(str, str_len, max_len) ReadStrN((str), (str_len), (max_len), data, data_length, out_offset)
#define READ_STRING(str, str_len) ReadString((str), (str_len), data, data_length, out_offset)

class IMessageSerializer2
{
public:
	virtual ~IMessageSerializer2() {};
	virtual bool Serialize(TLVSerializer2 * outstream) const = 0;
};

class IMessageDeserializer2
{
public:
	virtual ~IMessageDeserializer2() {};
	virtual bool Unserialize(TLVUnserializer2 & instream) = 0;
};

#endif