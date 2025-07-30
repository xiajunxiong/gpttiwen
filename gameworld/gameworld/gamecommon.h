
#ifndef GAMECOMMON_H
#define GAMECOMMON_H

#include "obj/character/role.h"
#include "gamedef.h"

namespace gamecommon
{

	extern Posf g_move_dir_vec[Dir_Count_Reserved];
	void GlobalInit();

	struct GameStat 
	{
		GameStat():user_packet_count(0), sync_pos_count(0), sync_pos_packege_count(0),
			enter_count(0), leave_count(0){}

		unsigned int user_packet_count;
		unsigned int sync_pos_count;
		unsigned int sync_pos_packege_count;
		unsigned int enter_count;
		unsigned int leave_count;

		void Clear()
		{
			user_packet_count = 0;
			sync_pos_count = 0;
		}
	};
	extern GameStat g_game_stat;

	// 通过位移偏移量来获取下一个运动的方向
	Dir GetMoveDir(const Posi &deltaPos, Scalar *distance);
	Dir GetMoveDirFloat(const Posf &deltaPos, Scalar *distance);

	const Posi GetDisRandPos(const Posi &curr_pos, int rand_dis);
}

//判断某位是否被置
//15.14....3.2.1.0 
#define ISSET0(x) ((x)&0x1)
#define ISSET1(x) ((x)&0x2)
#define ISSET2(x) ((x)&0x4)
#define ISSET3(x) ((x)&0x8)
#define ISSET4(x) ((x)&0x10)
#define ISSET5(x) ((x)&0x20)
#define ISSET6(x) ((x)&0x40)
#define ISSET7(x) ((x)&0x80)
#define ISSET8(x) ((x)&0x100)
#define ISSET9(x) ((x)&0x200)
#define ISSET10(x) ((x)&0x400)
#define ISSET11(x) ((x)&0x800)
#define ISSET12(x) ((x)&0x1000)
#define ISSET13(x) ((x)&0x2000)
#define ISSET14(x) ((x)&0x4000)
#define ISSET15(x) ((x)&0x8000)

// 设置某位
//15.14....3.2.1.0 
#define SET0(x) (x |= 0x1)
#define SET1(x) (x |= 0x2)
#define SET2(x) (x |= 0x4)
#define SET3(x) (x |= 0x8)
#define SET4(x) (x |= 0x10)
#define SET5(x) (x |= 0x20)
#define SET6(x) (x |= 0x40)
#define SET7(x) (x |= 0x80)
#define SET8(x) (x |= 0x100)
#define SET9(x) (x |= 0x200)
#define SET10(x) (x |= 0x400)
#define SET11(x) (x |= 0x800)
#define SET12(x) (x |= 0x1000)
#define SET13(x) (x |= 0x2000)
#define SET14(x) (x |= 0x4000)
#define SET15(x) (x |= 0x8000)

#endif
