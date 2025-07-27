#ifndef __BASEDEF_H__
#define __BASEDEF_H__

typedef char					Int8;
typedef unsigned char			UInt8;
typedef short					Int16;
typedef unsigned short			UInt16;
typedef int						Int32;
typedef unsigned int			UInt32;
typedef long long				Int64;
typedef unsigned long long		UInt64;

typedef float	  				Float;
typedef double					Double;

static const Int32 MAX_INT = ((UInt32)(-1))>>1;
static const Int32 MIN_INT = ~MAX_INT;
static const UInt32 MAX_UINT32 = (UInt32)(-1);

static const Int64 MAX_INT64 = ((UInt64)(-1))>>1;
static const Int64 MIN_INT64 = ~MAX_INT64;
static const UInt64 MAX_UINT64 = (UInt64)(-1);

static const Int16 MAX_INT16 = ((UInt16)(-1)) >> 1;
static const Int16 MIN_INT16 = ~MAX_INT16;
static const UInt16 MAX_UINT16 = (UInt16)(-1);

static const Int8 MAX_INT8 = ((UInt8)(-1)) >> 1;
static const Int8 MIN_INT8 = ~MAX_INT8;
static const UInt8 MAX_UINT8 = (UInt8)(-1);

#ifdef __GNUC__
#define __FUNCTION__ __PRETTY_FUNCTION__
#endif

#ifdef __GNUC__
#define barrier() __asm__ __volatile__("": : :"memory");
#else
#define barrier()  
#endif


#endif // __BASEDEF_H__

