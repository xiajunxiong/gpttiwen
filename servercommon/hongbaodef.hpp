#ifndef __HONG_BAO_DEF_HPP__
#define __HONG_BAO_DEF_HPP__


#include "servercommon/servercommon.h"
#include "common/tlvprotocol.h"

#include <set>

// ���ϵͳ
// ����ʲôϵͳ���͵ĺ�� , ���������ֵ���ܸ��� , ��Ӱ�쵽��ɫ

static const int MAX_HONG_BAO_TYPE_COUNT = 32;			// ������������ֵ

typedef char HongBaoGreeting[128];
typedef long long LL_UniqueHongBaoID;


struct UniqueHongBaoID
{
	UniqueHongBaoID() :server_id(-1), hong_bao_sys_type(-1), hong_bao_id(0) {}
	UniqueHongBaoID(const UniqueHongBaoID & _o) :
		server_id(_o.server_id), hong_bao_sys_type(_o.hong_bao_sys_type), hong_bao_id(_o.hong_bao_id) {}
	UniqueHongBaoID(int _server_id, int _hong_bao_sys_type, int _hong_bao_id) :
		server_id(_server_id), hong_bao_sys_type(_hong_bao_sys_type), hong_bao_id(_hong_bao_id) {}
	UniqueHongBaoID(LL_UniqueHongBaoID _ll_unique_hong_bao_id)
	{
		short temp_server_id = _ll_unique_hong_bao_id >> (16 + 32);
		short temp_sys_type = (_ll_unique_hong_bao_id << 16) >> (16 + 32);
		int temp_hong_bao_id = (_ll_unique_hong_bao_id << 32) >> 32;

		server_id = temp_server_id;
		hong_bao_sys_type = temp_sys_type;
		hong_bao_id = temp_hong_bao_id;
	}

	bool operator<(const UniqueHongBaoID& right) const
	{
		if (server_id < right.server_id)
		{
			return true;
		}
		else if (hong_bao_sys_type < right.hong_bao_sys_type)
		{
			return true;
		}

		return hong_bao_id < right.hong_bao_id;
	}

	bool operator>(const UniqueHongBaoID& right) const
	{
		if (server_id > right.server_id)
		{
			return true;
		}
		else if (hong_bao_sys_type > right.hong_bao_sys_type)
		{
			return true;
		}

		return hong_bao_id > right.hong_bao_id;
	}

	bool operator==(const UniqueHongBaoID &right) const
	{
		return server_id == right.server_id && hong_bao_sys_type == right.hong_bao_sys_type  && hong_bao_id == right.hong_bao_id;
	}

	bool operator!=(const UniqueHongBaoID &right) const
	{
		return server_id != right.server_id || hong_bao_sys_type != right.hong_bao_sys_type || hong_bao_id != right.hong_bao_id;
	}

	short server_id;
	short hong_bao_sys_type;
	int hong_bao_id;
};

inline LL_UniqueHongBaoID UniqueHongBaoIDToLLUniqueHongBaoID(UniqueHongBaoID _unique_hong_bao_id)
{
	LL_UniqueHongBaoID temp_ll = 0;

	temp_ll = _unique_hong_bao_id.server_id;
	temp_ll = temp_ll << 16;

	temp_ll |= _unique_hong_bao_id.hong_bao_sys_type;
	temp_ll = temp_ll << 32;

	temp_ll |= _unique_hong_bao_id.hong_bao_id;

	return temp_ll;
}

inline UniqueHongBaoID LLUniqueHongBaoIDToUniqueHongBaoID(LL_UniqueHongBaoID _ll_unique_hong_bao_id)
{
	short temp_server_id = _ll_unique_hong_bao_id >> (16 + 32);
	short temp_sys_type = (_ll_unique_hong_bao_id << 16) >> (16 + 32);
	int temp_hong_bao_id = (_ll_unique_hong_bao_id << 32) >> 32;

	return UniqueHongBaoID(temp_server_id, temp_sys_type, temp_hong_bao_id);
}

enum HongBaoGuildType
{
	HONGBAO_TYPE_INVALID = 0,
	HONGBAO_GUILD_TYPE_FIRST_CHONG = 1,			// �׳���  
	HONGBAO_GUILD_TYPE_CHONG_ZHI_1 = 2,			// ��ֵ��� - ��ֵ���  [��]  
	HONGBAO_GUILD_TYPE_CHONG_ZHI_2 = 3,			// ��ֵ��� - ��ֵ���  [��] 
	HONGBAO_GUILD_TYPE_CHONG_ZHI_3 = 4,			// ��ֵ��� - ��ֵ���  [��] 
	HONGBAO_GUILD_TYPE_CHONG_ZHI_4 = 5,			// ��ֵ��� - ��ֵ���  [��] 
	HONGBAO_GUILD_TYPE_ZHAN_LING = 6,			// ս���� 
	HONGBAO_GUILD_TYPE_CHONG_ZHI_5 = 7,			// ��ֵ��� - ��ֵ���  [��] 
	HONGBAO_GUILD_TYPE_LEI_CHONG_1 = 8,			// �۳��� - ��ֵ���
	HONGBAO_GUILD_TYPE_LEI_CHONG_2 = 9,			// �۳��� - ��ֵ���
	HONGBAO_GUILD_TYPE_LEI_CHONG_3 = 10,		// �۳��� - ��ֵ���
	HONGBAO_GUILD_TYPE_LEI_CHONG_4 = 11,		// �۳��� - ��ֵ���
	HONGBAO_GUILD_TYPE_LEI_CHONG_5 = 12,		// �۳��� - ��ֵ���
	HONGBAO_GUILD_TYPE_MAX,

	HONGBAO_TYPE_MAX = HONGBAO_GUILD_TYPE_MAX,
};

static const int HONGBAO_GUILD_TYPE_CHONG_ZHI_1_VAL = 1280;
static const int HONGBAO_GUILD_TYPE_CHONG_ZHI_2_VAL = 1980;
static const int HONGBAO_GUILD_TYPE_CHONG_ZHI_3_VAL = 3280;
static const int HONGBAO_GUILD_TYPE_CHONG_ZHI_4_VAL = 6480;

UNSTD_STATIC_CHECK(HONGBAO_GUILD_TYPE_MAX <= MAX_HONG_BAO_TYPE_COUNT)

//���ϵͳ����
enum HongBaoSysType
{
	HONGBAO_SYS_TYPE_GUILD = 0,				// ������				infodata:��ʹ��guild_id
	HONGBAO_SYS_TYPE_MAX,
};

static const int MAX_HONG_BAO_GET_MEMBER_COUNT = 50;				// ����������������
static const int MAX_ONCE_HONG_BAO_COUNT = 100;						// һ���Բ����ĺ������

typedef char HongBaoInfoData[16];

class SyncHongBaoItem;

class HongBaoItem
{
public:
	union InfoData				// union�ɴ��й��캯���������Ա�������Զ���Ĺ��캯��
	{
		InfoData()
		{
			this->Reset();
		}
		~InfoData() {}

		void GetData(HongBaoInfoData out_data) const
		{
			if (NULL == out_data) return;

			memcpy(out_data, data, sizeof(HongBaoInfoData));
		}

		void SetData(const HongBaoInfoData in_data)
		{
			if (NULL == in_data) return;

			memcpy(data, in_data, sizeof(HongBaoInfoData));
		}

		void Reset()
		{
			memset(data, 0, sizeof(data));
		}

		HongBaoInfoData data;
		long long param;
		int guild_id;
		int uid;
	};

	struct RewardInfo			// �����ȡ��Ϣ
	{
		RewardInfo() { this->Reset(); }
		void Reset()
		{
			uid = 0;
			memset(name, 0, sizeof(name));
			val = 0;
			times = 0;
		}

		int uid;				// ��ȡ�˵�UID
		GameName name;			// ��ȡ�˵�����
		int val;				// ��ȡ���Ľ��
		unsigned int times;		// ��ȡ��ʱ���
	};

	struct HongBaoRewardInfo
	{
		HongBaoRewardInfo() { this->Reset(); }
		void Reset()
		{
			get_count = 0;
			total_count = 0;
			for (int i = 0; i < MAX_HONG_BAO_GET_MEMBER_COUNT; ++i)
			{
				get_info[i].Reset();
			}
		}

		int get_count;							// ��ȡ�������Ա����
		int total_count;						// �������
		RewardInfo get_info[MAX_HONG_BAO_GET_MEMBER_COUNT];	// ��ȡ�������Ա��Ϣ
	};

	HongBaoItem() { this->Reset(); }

	void Reset();

	HongBaoItem& operator= (const HongBaoItem & _o);
	HongBaoItem& operator= (const SyncHongBaoItem & _o);


	char change_state;						// ���״̬

	LL_UniqueHongBaoID ll_unique_hong_bao_id;	// ȫ�����ΨһID  [��Ҫ���ںϷ����]

	int hong_bao_id;						// ���ID
	unsigned int invalid_timestamp;			// ���ʧЧʱ��

	int hongbao_type;						// �������	[�߻����õ�����]
	int hongbao_sys_type;					// ��������ڵ�ϵͳ����	[����:����,����ȵ�..]

	int send_uid;							// ������˵�uid
	int send_avatar_type;					// ������˵Ľ�ɫģ��
	int send_headshot_id;					// ������˵�ͷ��ID
	GameName send_name;						// ������˵�����

	HongBaoGreeting	greeting;				// ���ף����
	int used_val;							// ��ʹ�õĽ��

	InfoData info_data;						// ����sys�Ĳ�ͬ�ô���ͬ , Ҳ����û�õ�,���忴sys_type

	HongBaoRewardInfo hongbao_reward_info;	// �����ȡ��Ϣ

	// ����Ĳ�д�����ݿ�
	std::set<int> get_uid_set;				// �Ѿ���ȡ�ú�������uid����
};

#pragma pack(push, 4)

// ���ڿ����Ϣͨ��
class SyncHongBaoItem
{
public:
	SyncHongBaoItem() { this->Reset(); }

	void Reset();

	SyncHongBaoItem& operator= (const HongBaoItem & _o);

	char change_state;						// ���״̬

	LL_UniqueHongBaoID ll_unique_hong_bao_id;	// ȫ�����ΨһID  [��Ҫ���ںϷ����]

	int hong_bao_id;						// ���ID
	unsigned int invalid_timestamp;			// ���ʧЧʱ��

	int hongbao_type;						// �������	[�߻����õ�����]
	int hongbao_sys_type;					// ��������ڵ�ϵͳ����	[����:����,����ȵ�..]

	int send_uid;							// ������˵�uid
	int send_avatar_type;					// ������˵Ľ�ɫģ��
	int send_headshot_id;					// ������˵�ͷ��ID
	GameName send_name;						// ������˵�����
	HongBaoGreeting	greeting;				// ���ף����

	int used_val;							// ��ʹ�õĽ��

	HongBaoItem::InfoData info_data;					// ����sys�Ĳ�ͬ�ô���ͬ , Ҳ����û�õ�,���忴sys_type

	HongBaoItem::HongBaoRewardInfo hongbao_reward_info;	// �����ȡ��Ϣ
};

// ������ - �����
struct GiveHongBaoUserInfo
{
	GiveHongBaoUserInfo() { this->Reset(); }

	void Reset();

	UniqueServerID from_usid;
	int hongbao_type;						// �������	[�߻����õ�����]
	int hongbao_sys_type;					// ��������ڵ�ϵͳ����	[����:����,����ȵ�..]
	int send_uid;							// ������˵�uid
	int send_avatar_type;					// ������˵Ľ�ɫģ��
	int send_headshot_id;					// ������˵�ͷ��ID
	GameName send_name;						// ������˵�����
	HongBaoGreeting	greeting;				// ���ף����
	HongBaoItem::InfoData info_data;		// ����sys�Ĳ�ͬ�ô���ͬ , Ҳ����û�õ�,���忴sys_type
};


class HongBaoParam
{
public:
	bool Serialize(TLVSerializer &outstream) const;
	bool Unserialize(TLVUnserializer &instream);

	HongBaoParam() :count(0) {}

	int count;
	HongBaoItem hong_bao_list[MAX_ONCE_HONG_BAO_COUNT];
};


typedef char HongBaoInfoDataHex[sizeof(HongBaoInfoData) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(HongBaoInfoDataHex) < 128);

typedef char HongBaoRewardInfoHex[sizeof(HongBaoItem::HongBaoRewardInfo) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(HongBaoRewardInfoHex) < 8192);

#pragma pack(pop)

#endif	//__HONG_BAO_DEF_HPP__