
#ifndef GAMEDEF_H
#define GAMEDEF_H

#include "servercommon/servercommon.h"
#include "common/vector2d.h"

typedef unsigned short	TaskID;
typedef float			Dir;
typedef float			Scalar;
typedef TVector2<float>				Posf;
typedef TVector2<unsigned int>		Posu;
typedef char						ObjType;
typedef char						SkillType;

#define INVALID_TASK_ID		0		

enum _Dir
{
	// Dir_S = 0,
	Dir_S_RESERVED = 0,
	Dir_LU,
	Dir_U,
	Dir_RU,
	Dir_L,
	Dir_RESERVED,
	Dir_R,
	Dir_LD,
	Dir_D,
	Dir_RD,
	Dir_Count_Reserved
};

static const float Dir_S = 1000.0f;				// 一个无效的值 比-3.14 3.14要大 
static const float Dir_Max = 3.15f;				// 最大的方向 π的值

struct PosData
{
	PosData() : m_x(0), m_y(0), m_dir(0.0), m_distance(0.0) {}

	PosData(Axis _x, Axis _y, Dir _dir, Scalar distance) :
		m_x(_x), m_y(_y), m_dir(_dir), m_distance(distance) {}

	Axis m_x;
	Axis m_y;
	Dir m_dir;
	Scalar m_distance;
};

struct KnapsackItemIndex
{
	bool operator<(const KnapsackItemIndex & other)const
	{
		if (col < other.col)
			return true;
		if (col > other.col)
			return false;

		if (idx < other.idx)
			return true;
		if (idx > other.idx)
			return false;

		return false;
	}

	short col;
	short idx;
};

#pragma pack(push, 4)

struct MsgRoleBaseInfo
{
	int uid;
	GameName name;
	int role_level;
	int prof;
	short		avatar_type;
	short		headshot_id;
};


#pragma pack(pop)

#endif


