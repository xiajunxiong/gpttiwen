#ifndef __GATHER_DREAM_DEF_HPP__
#define __GATHER_DREAM_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/guilddef.hpp"

#pragma pack(push, 4)

static const int MAX_GATHER_DREAM_GIFT_NUM = 10;
static const int MAX_GATHER_DREAM_SEND_LIST_NUM = 30;		//��¼ֻ�·�ǰ30��
static const int GATHER_DREAM_RECORD_SAVE_TO_DB_TIME = 60;	

enum GATHER_DREAM_INFO_TYPE
{
	GATHER_DREAM_INFO_TYPE_GET = 0,			//����
	GATHER_DREAM_INFO_TYPE_GIFT = 1,		//����

	GATHER_DREAM_INFO_TYPE_MAX,
};
UNSTD_STATIC_CHECK(GATHER_DREAM_INFO_TYPE_MAX == 2);	//�˴�����޸���ע�Ᵽ������

struct GatherDreamParam
{

	GatherDreamParam(){ this->Reset();}
	
	void Reset()
	{
		gather_dream_publish_is_send_flag = 0;
		today_is_publish_gather_dream_flag = 0;
		count = 0;
		memset(gift_role_uid_list, 0, sizeof(gift_role_uid_list));
		memset(reserve_int, 0, sizeof(reserve_int));
	}

	char gather_dream_publish_is_send_flag;				//�������弯Ը�Ƿ�����Ϣ������Ƶ�� 0 :Ĭ�Ϲ�ѡ����  1:ȡ����ѡ,������
	char today_is_publish_gather_dream_flag;			//�����Ƿ��ѷ������弯Ը	0:δ���� 1:�ѷ���
	short count;
	int gift_role_uid_list[MAX_GATHER_DREAM_GIFT_NUM];	//������������uid�б�
	int reserve_int[10];								//������
};

typedef char GatherDreamParamHex[sizeof(GatherDreamParam) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(GatherDreamParamHex) < 2048);

struct SynGatherDreamRecord
{
	SynGatherDreamRecord() { this->Reset(); }

	void Reset()
	{
		memset(name, 0, sizeof(name));
		post = 0;
		headshot_id = 0;
		avatar_type = 0;
	}

	GameName name;
	int post;				//ְλ
	short headshot_id;		//ͷ��
	short avatar_type;		
};


struct GatherDreamRecordInfo
{
	GatherDreamRecordInfo() { this->Reset(); }

	void SetData(const SynGatherDreamRecord info);

	void Reset()
	{
		role_id = 0;
		post = 0;
		memset(name, 0, sizeof(name));
		headshot_id = 0;
		avatar_type = 0;
		gather_dream_type = 0;
		item_id = 0;
		timestamp = 0;

	}
	bool IsValid()
	{
		return role_id > 0;
	}

	int role_id;			//������	/ ������
	int post;				//ְλ
	GameName name;
	short headshot_id;		//ͷ��
	short avatar_type;
	short gather_dream_type;
	ItemID item_id;
	unsigned int timestamp;
};

#pragma pack(pop)

class GatherDreamRecordParam		// ��Ը��¼
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	struct GatherDreamRecordSaveEntry
	{
		GatherDreamRecordSaveEntry() { this->Reset(); }

		void Reset()
		{
			index = 0;
			change_state = 0;
			data_info.Reset();
		}
		int index;
		int change_state;
		GatherDreamRecordInfo data_info;
	};

	GatherDreamRecordParam() { this->Reset(); }
	void Reset()
	{
		get_data_list.clear();
		gift_data_list.clear();
	}

	std::map<int, std::vector<GatherDreamRecordSaveEntry> > get_data_list;	//������¼
	std::map<int, std::vector<GatherDreamRecordSaveEntry> >	gift_data_list;	//�����¼
};

#endif