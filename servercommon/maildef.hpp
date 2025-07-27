/******************************************************************************************
FileName: maildef.hpp
Author: 
Description: 邮件系统定义  
Version: 1.0
History: 
<author>    <time>					<version >    <desc>
dreamwang	2011/07/17/ 09:04:05     1.0           创建
*****************************************************************************************/
#ifndef __MAIL_DEF_HPP__
#define __MAIL_DEF_HPP__

#include <memory>

#include "servercommon/serverdef.h"
#include "servercommon/struct/itemlistparam.h"

static const int MIN_GET_MAIL_LIST_INTERVAL = 10;				// 限制用户拉取邮件列表的时间
static const int MAIL_SAVE_DB_INTERVAL = 10;					// 回写DB的时间间隔
static const int GET_SYSTEM_MAIL_INTERVAL = 60 * 5;				// 去DB拉系统邮件的时间间隔 
static const int MAX_GET_MAIL_TIMEOUT_INTERVAL = 15;			// 超时 处于加载中状态的最大时间间隔
static const int MAIL_UN_ACTIVE_TIMEOUT_INTERVAL = 60 * 10;		// 最大的不活跃时间 淘汰
static const int MAX_MAIL_CHECK_STATE_INTERVAL = 10;			// 检查用户邮件状态时间间隔
static const int MAIL_ONEKEY_FETCH_TIME_INTERVAL = 10;			// 一键领取时间间隔

static const int MAX_SUBJECT_LENTH = 128;						// 主题长度
static const int MAX_CONTENT_TXT_LENTH = 1200;					// 内容长度 要足够300字，一个中文字最多4字节，所以是1200

static const int MAX_MAIL_NUM = 160;							// 玩家最多有可拥有的邮件
static const int MAX_MAIL_SEND_LIST_SIZE = 50;					// 分包发 一次发的个数 
static const int MAX_USER_SYSTEM_MAIL_NUM = 10;					// 玩家系统邮件单次读取数量

static const int MAX_PERSONAL_MAIL = MAX_MAIL_NUM - 20;			// 私人邮件在邮箱满之前20封就不能发了
static const int MAX_LOCK_MAIL_NUM = 10;						// 最多能锁10封 
static const int MAX_MAIL_RESERVER_TIME = 3600 * 24 * 30;		// 最多保存30天

static const int MAIL_POSTAGE = 500;							// 邮资：500铜币
static const int MAIL_POSTAGE_ATTACHMENT = 500;					// 邮资带附件：500铜币

static const int MAX_GAMEWORLD_SEND_MAIL_USER = 2048;			// 支持同时给2048个人发邮件

static const int MAX_ATTACHMENT_ITEM_NUM = 8;					// 最大附件物品数 如果改变需要同时修改数据库

static const int MAX_VIRTUAL_ITEM_NUM = 15;						// 最大虚拟物品数 比如声望 战魂

static const int MAIL_SEND_OPEN_LEVEL = 20;						// 发送邮件开放等级

static const int ATTACHMENT_ITEM_STATUS_UNGET = 1;				// 附件未领取状态
static const int ATTACHMENT_ITEM_STATUS_GET = 0;				// 附件已领取状态

typedef char MailSubject[MAX_SUBJECT_LENTH];					// 邮件主题
typedef char MailContentTxt[MAX_CONTENT_TXT_LENTH];				// 邮件正文

enum MAIL_TYPE 
{
	MAIL_TYPE_INVALID = 0,
	MAIL_TYPE_PERSONAL,											// 个人邮件
	MAIL_TYPE_SYSTEM,											// 系统邮件
	MAIL_TYPE_GUILD,											// 家族邮件
	MAIL_TYPE_CHONGZHI,											// 充值邮件
	MAIL_TYPE_MAX,
};

enum MAIL_STATE
{
	MAIL_STATE_INVALID = 0,										// 无效状态
	MAIL_STATE_LOAD_ING,										// 加载中
	MAIL_STATE_LOAD_OK,											// 加载完成 
};

enum MAIL_VIRTUAL_ITEM
{
	MAIL_VIRTUAL_ITEM_BEGIN = 0,
	MAIL_VIRTUAL_ITEM_EXP = MAIL_VIRTUAL_ITEM_BEGIN,			// 经验
	MAIL_VIRTUAL_ITEM_GUILDGONGXIAN,							// 贡献
	MAIL_VIRTUAL_ITEM_IMMORTAL_COIN,							// 绑定元宝
	MAIL_VIRTUAL_ITEM_GOLD,										// 元宝
	MAIL_VIRTUAL_ITEM_BIND_COIN,								// 绑定铜币
	MAIL_VIRTUAL_COLOSSEUM_CREDITS,								// 竞技场积分
	MAIL_VIRTUAL_PLATFORM_BATTLE_CREDITS,						// 擂台战积分
	MAIL_VIRTUAL_HUAN_JIE_ZHAN_CHANG_CREDITS,					// 幻界战场积分
	MAIL_VIRTUAL_LUCK_COIN,										// 气运值
	MAIL_VIRTUAL_HUAN_JIE_COIN,									// 幻界硬币
	MAIL_VIRTUAL_TRADE_CREDIT,									// 交易行信用
	MAIL_VIRTUAL_YUAN_SUI_ZHI_LING,								// 元灵之髓
	MAIL_VIRTUAL_SILVER_COIN,									// 银两(灵玉) 

	MAIL_VIRTUAL_ITEM_MAX,
};

UNSTD_STATIC_CHECK(MAIL_VIRTUAL_ITEM_MAX < MAX_VIRTUAL_ITEM_NUM);

enum MAIL_REASON
{
	MAIL_REASON_DEFAULT = 0,		//!< 0 默认原因
	MAIL_REASON_GM_TEST,			//!< 1 gm测试
	MAIL_REASON_PLATFORM_BATTLE,	//!< 2 擂台战
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
	SYSMAIL_EVENT_CONDITION_LEVEL_EQUAL_GREATER,				// 等级大等于
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

	char sender_level;										// 字段改为发送者等级
	char sender_avatar;										// 修改字段为发送者头像
	short reason;											// 原因（来源）
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

	MailSubject subject;									//邮件主题
	MailContentTxt contenttxt;								//文本内容

	short subject_length;
	short contenttxt_length;
	ItemDataWrapper item_list[MAX_ATTACHMENT_ITEM_NUM];		//获得物品列表

	MailVirtualItemList virtual_item_list;					//虚拟物品列表，用于存放金币，等没有实体的物品
	
	MailItemStatusList item_status;							//物品状态

	int reserver_coin;										//邮件发送的铜币
	int reserver_coin_bind;									//绑定铜币
	int reserver_gold;										//元宝
	int reserver_gold_bind;									//绑定元宝
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

	int mail_index;											//邮件序号
	SerialNum mail_serial;									//邮件序列号

	MailStatus status;										//邮件状态(已读等
	MailContentParam contentparam;							//邮件内容param
};

typedef char MailVirtualItemListHex[sizeof(MailVirtualItemList) * 2 + 1];
typedef char MailItemStatusHex[sizeof(MailItemStatusList) * 2 + 1];
UNSTD_STATIC_CHECK(sizeof(MailVirtualItemListHex) < 128)
UNSTD_STATIC_CHECK(sizeof(MailItemStatusHex) < 128)

//////////////////////////////////////////////////////////////////////////
struct MailSend												//发送邮件
{
	int	recver_dbindex;										//接受者在数据库的序列
	int	recver_roleid;										//接收者角色id

	int	sender_dbindex;										//发送者在数据库的序列
	int	sender_roleid;										//发送者id
	GameName sender_name;									//发送者名字，32byte

	char kind;												//邮件种类
	char sender_level;										//字段改为发送者等级
	char sender_avatar;										//修改字段为发送者头像
	char reserved;

	MailContentParam contentparam;							//邮件内容
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
	int	result;					// 0 成功 其他 失败
};

#endif  // __MAIL_DEF_H__

