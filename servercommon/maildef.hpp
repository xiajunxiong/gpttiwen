/******************************************************************************************
FileName: maildef.hpp
Author: 
Description: �ʼ�ϵͳ����  
Version: 1.0
History: 
<author>    <time>					<version >    <desc>
dreamwang	2011/07/17/ 09:04:05     1.0           ����
*****************************************************************************************/
#ifndef __MAIL_DEF_HPP__
#define __MAIL_DEF_HPP__

#include <memory>

#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"

static const int MIN_GET_MAIL_LIST_INTERVAL = 10;				// �����û���ȡ�ʼ��б��ʱ��
static const int MAIL_SAVE_DB_INTERVAL = 10;					// ��дDB��ʱ����
static const int GET_SYSTEM_MAIL_INTERVAL = 60 * 5;				// ȥDB��ϵͳ�ʼ���ʱ���� 
static const int MAX_GET_MAIL_TIMEOUT_INTERVAL = 15;			// ��ʱ ���ڼ�����״̬�����ʱ����
static const int MAIL_UN_ACTIVE_TIMEOUT_INTERVAL = 60 * 10;		// ���Ĳ���Ծʱ�� ��̭
static const int MAX_MAIL_CHECK_STATE_INTERVAL = 10;			// ����û��ʼ�״̬ʱ����
static const int MAIL_ONEKEY_FETCH_TIME_INTERVAL = 10;			// һ����ȡʱ����

static const int MAX_SUBJECT_LENTH = 128;						// ���ⳤ��
static const int MAX_CONTENT_TXT_LENTH = 1200;					// ���ݳ��� Ҫ�㹻300�֣�һ�����������4�ֽڣ�������1200

static const int MAX_MAIL_NUM = 160;							// �������п�ӵ�е��ʼ�
static const int MAX_MAIL_SEND_LIST_SIZE = 50;					// �ְ��� һ�η��ĸ��� 
static const int MAX_USER_SYSTEM_MAIL_NUM = 10;					// ���ϵͳ�ʼ����ζ�ȡ����

static const int MAX_PERSONAL_MAIL = MAX_MAIL_NUM - 20;			// ˽���ʼ���������֮ǰ20��Ͳ��ܷ���
static const int MAX_LOCK_MAIL_NUM = 10;						// �������10�� 
static const int MAX_MAIL_RESERVER_TIME = 3600 * 24 * 30;		// ��ౣ��30��

static const int MAIL_POSTAGE = 500;							// ���ʣ�500ͭ��
static const int MAIL_POSTAGE_ATTACHMENT = 500;					// ���ʴ�������500ͭ��

static const int MAX_GAMEWORLD_SEND_MAIL_USER = 2048;			// ֧��ͬʱ��2048���˷��ʼ�

static const int MAX_ATTACHMENT_ITEM_NUM = 8;					// ��󸽼���Ʒ�� ����ı���Ҫͬʱ�޸����ݿ�

static const int MAX_VIRTUAL_ITEM_NUM = 15;						// ���������Ʒ�� �������� ս��

static const int MAIL_SEND_OPEN_LEVEL = 20;						// �����ʼ����ŵȼ�

static const int ATTACHMENT_ITEM_STATUS_UNGET = 1;				// ����δ��ȡ״̬
static const int ATTACHMENT_ITEM_STATUS_GET = 0;				// ��������ȡ״̬

typedef char MailSubject[MAX_SUBJECT_LENTH];					// �ʼ�����
typedef char MailContentTxt[MAX_CONTENT_TXT_LENTH];				// �ʼ�����

enum MAIL_TYPE 
{
	MAIL_TYPE_INVALID = 0,
	MAIL_TYPE_PERSONAL,											// �����ʼ�
	MAIL_TYPE_SYSTEM,											// ϵͳ�ʼ�
	MAIL_TYPE_GUILD,											// �����ʼ�
	MAIL_TYPE_CHONGZHI,											// ��ֵ�ʼ�
	MAIL_TYPE_MAX,
};

enum MAIL_STATE
{
	MAIL_STATE_INVALID = 0,										// ��Ч״̬
	MAIL_STATE_LOAD_ING,										// ������
	MAIL_STATE_LOAD_OK,											// ������� 
};

enum MAIL_VIRTUAL_ITEM
{
	MAIL_VIRTUAL_ITEM_BEGIN = 0,
	MAIL_VIRTUAL_ITEM_EXP = MAIL_VIRTUAL_ITEM_BEGIN,			// ����
	MAIL_VIRTUAL_ITEM_GUILDGONGXIAN,							// ����
	MAIL_VIRTUAL_ITEM_IMMORTAL_COIN,							// ��Ԫ��
	MAIL_VIRTUAL_ITEM_GOLD,										// Ԫ��
	MAIL_VIRTUAL_ITEM_BIND_COIN,								// ��ͭ��
	MAIL_VIRTUAL_COLOSSEUM_CREDITS,								// ����������
	MAIL_VIRTUAL_PLATFORM_BATTLE_CREDITS,						// ��̨ս����
	MAIL_VIRTUAL_HUAN_JIE_ZHAN_CHANG_CREDITS,					// �ý�ս������
	MAIL_VIRTUAL_LUCK_COIN,										// ����ֵ
	MAIL_VIRTUAL_HUAN_JIE_COIN,									// �ý�Ӳ��
	MAIL_VIRTUAL_TRADE_CREDIT,									// ����������
	MAIL_VIRTUAL_YUAN_SUI_ZHI_LING,								// Ԫ��֮��
	MAIL_VIRTUAL_SILVER_COIN,									// ����(����) 

	MAIL_VIRTUAL_ITEM_MAX,
};

UNSTD_STATIC_CHECK(MAIL_VIRTUAL_ITEM_MAX < MAX_VIRTUAL_ITEM_NUM);

enum MAIL_REASON
{
	MAIL_REASON_DEFAULT = 0,		//!< 0 Ĭ��ԭ��
	MAIL_REASON_GM_TEST,			//!< 1 gm����
	MAIL_REASON_PLATFORM_BATTLE,	//!< 2 ��̨ս
	MAIL_REASON_MAX,
};

enum MAIL_FETCHATTCHMENT_TYPE
{
	MAIL_FETCH_VIRTUAL = -2,
	MAIL_FETCH_ALL = -1,
};

enum SYSMAIL_EVENT_CONDITION
{
	SYSMAIL_EVENT_CONDITION_INVALID = 0,
	SYSMAIL_EVENT_CONDITION_LEVEL_EQUAL_GREATER,				// �ȼ������
	SYSMAIL_EVENT_CONDITION_MAX,
};

enum MAIL_STATUS_COIN_INDEX
{
	MAIL_STATUS_COIN_INDEX_BEGIN = 17,
	MAIL_STATUS_COIN_INDEX_COIN,
	MAIL_STATUS_COIN_INDEX_COIN_BIND,
	MAIL_STATUS_COIN_INDEX_GOLD,
	MAIL_STATUS_COIN_INDEX_GOLD_BIND,
};

struct MailStatus
{
	MailStatus() : sender_uid(0), recv_time(0), kind(0), is_read(0), is_lock(0), is_fetch(0), sender_level(0), sender_avatar(0), reason(0)
	{
		this->Reset();
	}

	void Reset()
	{
		sender_uid = 0;
		memset(sender_name, 0, sizeof(sender_name));

		recv_time = 0u;
		kind = 0;
		is_read = 0;
		is_lock = 0;
		is_fetch = 0;
		sender_level = 0;
		sender_avatar = 0;
		reason = 0;
	}

	int sender_uid;
	GameName sender_name;

	unsigned int recv_time;	

	char kind;
	char is_read;
	char is_lock;
	char is_fetch;

	char sender_level;										// �ֶθ�Ϊ�����ߵȼ�
	char sender_avatar;										// �޸��ֶ�Ϊ������ͷ��
	short reason;											// ԭ����Դ��
};

struct MailBrief
{
	MailBrief() : mail_index(-1), has_attachment(0), subject_length(0)
	{
		this->Reset();
	}

	void Reset()
	{
		mail_index = -1;
		has_attachment = 0;
		subject_length = 0;
		memset(subject, 0, sizeof(subject));

		mail_status.Reset();
	}

	int mail_index;
	short has_attachment;
	short subject_length;

	MailStatus mail_status;
	MailSubject subject;
};

struct MailVirtualItemList
{
	MailVirtualItemList() { this->Reset(); }

	void Reset()
	{
		memset(virtual_item, 0, sizeof(virtual_item));
	}

	int virtual_item[MAX_VIRTUAL_ITEM_NUM];
};

struct MailItemStatusList
{
	MailItemStatusList(){ this->Reset();}

	void Reset()
	{
		//memset(item_status, 1, sizeof(item_status));
		for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM + MAX_VIRTUAL_ITEM_NUM + 4; ++ i)
		{
			this->item_status[i] = 1;
		}
	}

	char item_status[MAX_ATTACHMENT_ITEM_NUM + MAX_VIRTUAL_ITEM_NUM + 4];
};

struct MailContentParam
{
	MailContentParam() { this->Reset(); }
	
	void Reset()
	{
		subject_length = 0;
		contenttxt_length = 0;
		
		memset(subject, 0, sizeof(subject));
		memset(contenttxt, 0, sizeof(contenttxt));
		for (int i = 0; i < ARRAY_ITEM_COUNT(item_list); ++i)
		{
			item_list[i].Reset();
		}
		item_status.Reset();
		virtual_item_list.Reset();

		reserver_coin = 0;
		reserver_coin_bind = 0;
		reserver_gold = 0;
		reserver_gold_bind = 0;
	}

	bool IsInvalid() const 
	{
		for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; ++i)
		{
			if (!item_list[i].Invalid())
			{
				return false;
			}
		}

		for (int i = 0; i < MAX_VIRTUAL_ITEM_NUM; ++i)
		{
			if (virtual_item_list.virtual_item[i] > 0)
			{
				return false;
			}
		}

		return true;
	}

	bool HasAttachment() const
	{
		UNSTD_STATIC_CHECK(8 == MAX_ATTACHMENT_ITEM_NUM);

		for (int i = 0; i < MAX_VIRTUAL_ITEM_NUM; i++)
		{
			if (0 != virtual_item_list.virtual_item[i])
			{
				return true;
			}
		}

		for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; ++i)
		{
			if (!item_list[i].Invalid())
			{
				return true;
			}
		}

		return false;
	}

	void ClearAttachment()
	{
		for (int i = 0; i < MAX_VIRTUAL_ITEM_NUM; i++)
		{
			virtual_item_list.virtual_item[i] = 0;		
		}

		for (int i = 0; i < MAX_ATTACHMENT_ITEM_NUM; ++i)
		{
			item_list[i].Reset();
		}
	}

	MailSubject subject;									//�ʼ�����
	MailContentTxt contenttxt;								//�ı�����

	short subject_length;
	short contenttxt_length;
	ItemDataWrapper item_list[MAX_ATTACHMENT_ITEM_NUM];		//�����Ʒ�б�

	MailVirtualItemList virtual_item_list;					//������Ʒ�б����ڴ�Ž�ң���û��ʵ�����Ʒ
	
	MailItemStatusList item_status;							//��Ʒ״̬

	int reserver_coin;										//�ʼ����͵�ͭ��
	int reserver_coin_bind;									//��ͭ��
	int reserver_gold;										//Ԫ��
	int reserver_gold_bind;									//��Ԫ��
};

struct MailParam
{
	MailParam() : mail_index(-1) 
	{
		this->Reset();
	}

	void Reset()
	{
		mail_index = -1;
		memset(mail_serial, 0, sizeof(mail_serial));
		status.Reset();
		contentparam.Reset();
	}

	int mail_index;											//�ʼ����
	SerialNum mail_serial;									//�ʼ����к�

	MailStatus status;										//�ʼ�״̬(�Ѷ���
	MailContentParam contentparam;							//�ʼ�����param
};

typedef char MailVirtualItemListHex[sizeof(MailVirtualItemList) * 2 + 1];
typedef char MailItemStatusHex[sizeof(MailItemStatusList) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MailVirtualItemListHex) < 128)
UNSTD_STATIC_CHECK(sizeof(MailItemStatusHex) < 128)

//////////////////////////////////////////////////////////////////////////
struct MailSend												//�����ʼ�
{
	int	recver_dbindex;										//�����������ݿ������
	int	recver_roleid;										//�����߽�ɫid

	int	sender_dbindex;										//�����������ݿ������
	int	sender_roleid;										//������id
	GameName sender_name;									//���������֣�32byte

	char kind;												//�ʼ�����
	char sender_level;										//�ֶθ�Ϊ�����ߵȼ�
	char sender_avatar;										//�޸��ֶ�Ϊ������ͷ��
	char reserved;

	MailContentParam contentparam;							//�ʼ�����
};

struct FetchAttachment
{
	int db_index;
	int	role_id;

	int	mail_index;
	int item_index;
	int is_last;
};

struct FetchAttachmentAffirm
{
	int	db_index;
	int	role_id;
	int	mail_index;
	int	item_index;
	int	is_last;
	int	result;					// 0 �ɹ� ���� ʧ��
};

#endif  // __MAIL_DEF_H__

