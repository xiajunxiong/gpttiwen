#ifndef __SHI_TU_DEF_H__
#define __SHI_TU_DEF_H__

#include "servercommon/serverdef.h"
#include "servercommon/servercommon.h"
//------------------------------------------------�����-----------------------------------------------------

struct ShiTuFbMonsterInfoItem
{
	ShiTuFbMonsterInfoItem()
	{
		group = 0;
		index = 0;
		is_kill = 0;
		obj_id = 0;
		pos_x = 0;
		pos_y = 0;
	}

	short group;
	short index;
	short is_kill;
	short obj_id;
	int pos_x;
	int pos_y;
};




//-----------------------------------------------------------------------------------------------------
const static int SHI_TU_SEEKING_MAX_COUNT = 50;							// ����������
static const int SHITU_MAX_APPRENTICE_COUNT = 3;						// ��ӵ������ͽ����
static const int SHITU_MAX_MASTER_COUNT = 2;							// ��ӵ������ʦ����
const static int SHI_TU_SEEKING_BEING_TIME = 3 * 24 * 3600;				// ���������72Сʱ
const static int SHI_TU_SEEKING_SAVE_TO_DB_TIME = 60;					// ÿ��60�뱣��һ��

enum SHITU_GONGFA_TYPE
{
	SHITU_GONGFA_TYPE_STATIC = 0,											// �̶�����
	SHITU_GONGFA_TYPE_NORMAL,												// ��ͨ����

	SHITU_GONGFA_TYPE_MAX
};

enum SHI_TU_SEEKING_TYPE
{
	SHI_TU_SEEKING_TYPE_APPRENTICE,		//ͽ��
	SHI_TU_SEEKING_TYPE_MASTER,			//ʦ��

	SHI_TU_SEEKING_TYPE_MAX
};

//ʦͽ--�������   //������ȡ���� ���������󾭳��޸�. û�д�����, ��Ҫ�ĵ�����
enum SHI_TU_MILE_TYPE
{
	SHI_TU_MILE_TYPE_CAP_UP = 1,	// ��������
	SHI_TU_MILE_TYPE_PASS_DAY,		// ��ʦ����
	SHI_TU_MILE_TYPE_MAX,
};

#pragma pack(push, 4)

struct RelationInfoOld
{
	RelationInfoOld()
	{
		this->Reset();
	}

	void Reset()
	{
		role_id = 0;
		relation_break_time = 0;
		is_graduation = 0;
		reserved_ch = 0;
		reserved_sh = 0;
	}

	int role_id;
	unsigned int relation_break_time;
	unsigned char is_graduation;
	unsigned char reserved_ch;
	short reserved_sh;
};

struct ShiTuRelationDataOld
{
	ShiTuRelationDataOld()
	{
		this->Reset();
	}

	void Reset()
	{
		for (int index = 0; index < SHITU_MAX_MASTER_COUNT; ++index)
		{
			master_list[index].Reset();
		}

		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			apprentice_list[index].Reset();
		}
	}

	int ApprenticeCount()
	{
		int count = 0;
		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			if (apprentice_list[index].role_id > 0)
			{
				++count;
			}
		}

		return count;
	}

	bool IsApprentice(int uid)
	{
		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			if (uid == apprentice_list[index].role_id)
			{
				return true;
			}
		}

		return false;
	}

	void AddApprentice(int uid)
	{
		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			if (apprentice_list[index].role_id <= 0)
			{
				apprentice_list[index].role_id = uid;
				apprentice_list[index].relation_break_time = 0;
				return;
			}
		}
	}

	void DeleteApprentice(int uid)
	{
		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			if (apprentice_list[index].role_id == uid)
			{
				apprentice_list[index].Reset();
				return;
			}
		}
	}

	RelationInfoOld master_list[SHITU_MAX_MASTER_COUNT];
	RelationInfoOld apprentice_list[SHITU_MAX_APPRENTICE_COUNT];
};

struct RelationInfo
{
	RelationInfo()
	{
		this->Reset();
	}

	void Reset()
	{
		role_id = 0;
		relation_break_time = 0;
		shi_tu_time = 0;
		reward_flag = 0;

		apprentice_old_cap = 0;
		mile_reward_flag = 0;
		relation_break_time_type = 0;
		memset(reserved_ch, 0, sizeof(reserved_ch));
	}

	int role_id;	
	unsigned int relation_break_time;			//ʦͽ�Զ����ʱ��, һ�� �Է���ͬ��Ҳ��72Сʱ��ҵ���ʦ��Ҫ��Ҫ����
	unsigned int shi_tu_time;				//ʦͽ���ʱ��
	unsigned int reward_flag;

	long long apprentice_old_cap;			//��ʦʱ ͽ�ܵ�ս��
	long long mile_reward_flag;				//��̽�����־[���]

	unsigned char day_online_flag;			//��������֪ͨ���
	unsigned char hold_ch;
	unsigned short hold_sh;
	char relation_break_time_type;
	unsigned char reserved_ch[11];
};

struct ShiTuRelationData
{
	ShiTuRelationData()
	{
		this->Reset();
	}

	void Reset()
	{
		for (int index = 0; index < SHITU_MAX_MASTER_COUNT; ++index)
		{
			master_list[index].Reset();
		}

		for (int index = 0; index < SHITU_MAX_APPRENTICE_COUNT; ++index)
		{
			apprentice_list[index].Reset();
		}
	}

	RelationInfo master_list[SHITU_MAX_MASTER_COUNT];
	RelationInfo apprentice_list[SHITU_MAX_APPRENTICE_COUNT];
};

const static int SHI_TU_PARAM_MOD_VER = 1;

#ifdef _DEBUG
const static unsigned int SHI_TU_TIME = 1631808000;
#else
const static unsigned int SHI_TU_TIME = 1632672000;
#endif

struct ShiTuParam
{
	ShiTuParam()
	{
		this->Reset();
	}

	void Reset()
	{
		relation_data_old.Reset();
		shi_tu_fb_reward_flag = 0;		
		relation_data.Reset();
		mod_ver = 0;

		day_mile_cap_fetch_num = 0;

		week_chuan_gong_flag = 0;
		week_chuan_gong_exp_flag = 0;
		week_chuan_gong_score_max = 0;

		day_mile_pass_day_fetch_num = 0;
		mile_reward_flag = 0;
	}

	ShiTuRelationDataOld relation_data_old;
	int shi_tu_fb_reward_flag;
	UNSTD_STATIC_CHECK(240 == sizeof(ShiTuRelationData));
	ShiTuRelationData relation_data;
	int mod_ver;

	int day_mile_cap_fetch_num;	//����ս����� ��ȡ����
	
	int week_chuan_gong_flag;		//���ܴ����������
	int week_chuan_gong_exp_flag;	//���ܴ���������
	int week_chuan_gong_score_max;	//���ܴ�����ߵ÷�

	int day_mile_pass_day_fetch_num;	//���հ�ʦ������� ��ȡ����
	long long mile_reward_flag;				//��̽�����־[���]
};
typedef char ShiTuParamHex[sizeof(ShiTuParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(ShiTuParamHex) < 1024);

struct ShiTuSeeking
{
	ShiTuSeeking()
	{
		this->Reset();
	}

	void Reset()
	{
		uid = 0;
		seeking_type = -1;
		timestamp = 0;
		end_timestamp = 0;
		memset(seeking_notice, 0, sizeof(seeking_notice));
	}

	bool Invalid() const
	{
		return uid <= 0;
	}

	int uid;								
	int seeking_type;						//��������			//1 Ѱ��ͽ�ܣ� 0 Ѱ��ʦ��
	unsigned int timestamp;					//�������Ե�ʱ���
	unsigned int end_timestamp;				//���������ʱ���
	ShiTuSeekingNotice seeking_notice;
};

#pragma pack(pop)

struct ShiTuSeekingParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct ShiTuSeekingEntry
	{
		ShiTuSeekingEntry() : change_state(0), index(0)
		{}

		char change_state;
		int index;

		ShiTuSeeking data;
	};

	ShiTuSeekingEntry apprentice_list[SHI_TU_SEEKING_MAX_COUNT];
	ShiTuSeekingEntry master_list[SHI_TU_SEEKING_MAX_COUNT];
};



#endif
