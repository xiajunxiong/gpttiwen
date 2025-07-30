#include <stdlib.h>
#include "gamecommon.h"
#include "globalconfig/globalconfig.h"
#include "servercommon/servercommon.h"

namespace gamecommon
{

	Posf g_move_dir_vec[Dir_Count_Reserved];

	void GlobalInit()
	{
		// g_move_dir_vec[Dir_S] =	 Posf( 0,	 0  );
		g_move_dir_vec[Dir_L] =  Posf(-1.0,	 0  );
		g_move_dir_vec[Dir_R] =  Posf( 1.0,	 0  );
		g_move_dir_vec[Dir_U] =  Posf( 0,	 1.0);
		g_move_dir_vec[Dir_D] =  Posf( 0,   -1.0);
		g_move_dir_vec[Dir_LU] = Posf(-1.0,	 1.0);
		g_move_dir_vec[Dir_LD] = Posf(-1.0, -1.0);
		g_move_dir_vec[Dir_RU] = Posf( 1.0,	 1.0);
		g_move_dir_vec[Dir_RD] = Posf( 1.0, -1.0);
		g_move_dir_vec[Dir_RESERVED] = Posf( 0, 0);

		for (int i = 1; i < Dir_Count_Reserved; ++i)
		{
			g_move_dir_vec[i].Normalize();
		}
	}

	GameStat g_game_stat;

	Dir GetMoveDir(const Posi &deltaPos, Scalar *distance)
	{
		if (deltaPos.x == 0 && deltaPos.y == 0)
		{
			if (NULL != distance) *distance = 0;
			return Dir_S;
		}

		if (NULL != distance) 
		{
			*distance = static_cast<Scalar>(deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y);
			*distance = sqrt(*distance);
		}

		return atan2((float)deltaPos.y, (float)deltaPos.x);

		//Posi absP((deltaPos.x >= 0 ? deltaPos.x : -deltaPos.x), (deltaPos.y >= 0 ? deltaPos.y : -deltaPos.y));
		//Axis s, l;
		//bool x_s = true;
		//if (absP.x < absP.y)
		//{
		//	s = absP.x;
		//	l = absP.y;
		//}
		//else
		//{
		//	s = absP.y;
		//	l = absP.x;
		//	x_s = false;
		//}
		//const float sqrt2 = 1.41421356f;

		//Dir dir;
		//Axis deltaSL = l - s;
		//float slashD = sqrt2 * s;
		//if (deltaSL > slashD)
		//{
		//	*distance = (Scalar)deltaSL;
		//	dir = (x_s ? (deltaPos.y > 0 ? Dir_U : Dir_D) : (deltaPos.x > 0 ? Dir_R : Dir_L));
		//}
		//else
		//{
		//	*distance = (Scalar)slashD;
		//	dir = ((deltaPos.x > 0) ? (deltaPos.y > 0 ? Dir_RU : Dir_RD) : (deltaPos.y > 0 ? Dir_LU : Dir_LD));
		//}
		// return dir;
	}

	Dir GetMoveDirFloat(const Posf &deltaPos, Scalar *distance)
	{
		if (fabs(deltaPos.x - 0.0) < 1e-6 && fabs(deltaPos.y - 0.0) < 1e-6)
		{
			if (NULL != distance) *distance = 0;
			return Dir_S;
		}

		if (NULL != distance) 
		{
			*distance = static_cast<Scalar>(deltaPos.x * deltaPos.x + deltaPos.y * deltaPos.y);
			*distance = sqrt(*distance);
		}

		return atan2(deltaPos.y, deltaPos.x);
	}

	//const Posi GetDisRandPos(const Posi &curr_pos, int rand_dis)
	//{
	//	if (rand_dis <= 0) return curr_pos;

	//	int x = curr_pos.x + (rand() % (rand_dis * 2) - rand_dis);
	//	int y = curr_pos.y + (rand() % (rand_dis * 2) - rand_dis);

	//	return Posi(x, y);
	//}

}

