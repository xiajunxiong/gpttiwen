#ifndef __DRAW_DEF_HPP__
#define __DRAW_DEF_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/struct/itemlistparam.h"
#include "servercommon/msgserialize.hpp"

//����ƽ�����
static const int DRAW_GROUP_MAX_ITEM_NUM = 5;

//������ Ĭ�ϳ�ʱʱ��(��)
static const unsigned int DRAW_GROUP_DEF_OUT_TIME = 20;
static const int MAX_ITEM_LENGTH_NUM = 1256;

//����������
enum DrawGroupType
{
	DRAW_GROUP_TYPE_DEF = 0,					//���������� Ĭ��
	DRAW_GROUP_TYPE_MI_YI = 1,					//Զ��
	DRAW_GROUP_TYPE_FIELD_BOSS = 2,				//Ұ��boss
	DRAW_GROUP_TYPE_SHAN_HAI = 3,				//ɽ������
	DRAW_GROUP_TYPE_CRYSTAL = 4,				//ˮ������ͨ��
	DRAW_GROUP_TYPE_CRYSTAL_FAST_PASS = 5,		//ˮ������ɨ��
	DRAW_GROUP_TYPE_PET_GOD_FB = 6,				//��ӡ֮��
	DRAW_GROUP_TYPE_PET_GOD_FB_AUTO = 7,		//��ӡ֮�� - ɨ��
	DRAW_GROUP_TYPE_HUNDRED_GHOST = 8,			//�ٹ�ҹ��

	DRAW_GROUP_TYPE_MAX,
};

//���ƽ�����Ʒvector
typedef std::vector<ItemDataWrapper> DrawGroupRewardItemVec;

struct ItemDrawInfo
{
	ItemDrawInfo()
	{
		this->Reset();
	}

	bool Serialize(TLVSerializer2 & outstream) const ;
	void Reset()
	{
		param_len = 0;
		item_info.Reset();
	}
	int param_len;
	ItemDataWrapper item_info;
};

//���ƽ�������map<DrawGroupVirtualType, num>
typedef std::map<int, long long> DrawGroupRewardMoneyMap;
enum DrawGroupVirtualType
{
	DRAW_GROUP_VIRTUAL_TYPE_LUCK_COIN = 0,		//����ֵ

	DRAW_GROUP_VIRTUAL_TYPE_MAX,
};

//���ƽ�������(����û�� ׼��ͨ��type����һЩ���⴦��  Ҫ��ʱ���ڼӰ�)
typedef std::vector<int> DrawGroupRewardParamVec;

//��������Ϣ ��vector��Ա
struct DrawGroupInfo
{
	struct DrawItem
	{
		DrawItem()
		{
			role_uid = 0;
			draw_index = -1;
		}
		int role_uid;		//ӵ����id
		
		int draw_index;		//��ȡindex
							//-1��ʾδ��ȡ [0, DRAW_GROUP_MAX_ITEM_NUM) ��ʾ��� �ֶ� �� ϵͳ��� ����ʱѡ������

		//������Ϣ
		//��Ʒ
		DrawGroupRewardItemVec item_list;
		//����
		DrawGroupRewardMoneyMap money_type;

		//���ƽ�������(����û�� ׼��ͨ��type(�����ܷ���)����һЩ���⴦�� Ҫ��ʱ���ڼӰ�)
 		//int param_type;
 		//DrawGroupRewardParamVec param_list;
		//����һ����̬����ָ������֧��һЩ���⴦��
		//static void (*special_fun)(Role *role , DrawInfo &this_info); 
	};
	
	DrawGroupInfo()
	{
		draw_group_id = 0;
		out_time = 0;
		draw_group_type = 0;

		draw_count = 0;
	}

	int draw_group_id;		//������id �Զ�����
	unsigned int out_time;			//��ʱʱ���
	int draw_group_type;			//������ ����

	//���ƽ�������(�����������߿���Ϊ�� 0==role_uid )
	int draw_count;
	DrawItem draw_info_list[DRAW_GROUP_MAX_ITEM_NUM];
	
};

#endif // ����   __DRAW_DEF_HPP__