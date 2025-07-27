#ifndef __CROSS_GAME_PROTOCAL_H__
#define __CROSS_GAME_PROTOCAL_H__

#include "servercommon/serverdef.h"
#include "servercommon/crossdef.hpp"
#include "servercommon/teamdef.hpp"
#include "servercommon/activitydef.hpp"
#include "servercommon/maildef.hpp"
#include "servercommon/chatserverdef.h"
#include "servercommon/guilddef.hpp"
#include "servercommon/struct/global/worldstatusparam.hpp"
#include "servercommon/commondata.hpp"
#include "servercommon/rankdef.hpp"
#include "servercommon/msgserialize.hpp"
#include "servercommon/platformbattledef.hpp"
#include "servercommon/struct/global/couragechallengerankdef.hpp"
#include "servercommon/struct/system/publicnoticelist.h"
#include "servercommon/gatherdreamdef.hpp"
#include "servercommon/commonsavedef.h"
#include "servercommon/roleactivity/raquanmindiscountparam.hpp"
#include "servercommon/struct/global/serverfirstkilldef.h"
#include "servercommon/struct/global/marketmanagerparamhpp.h"
#include "gameworld/gameworld/battle/battle_video_manager.hpp"
#include "servercommon/frienddef.hpp"
#include "servercommon/hongbaodef.hpp"
#include "servercommon/bigdipperdef.h"
#include "servercommon/struct/friendparam.h"
#include "servercommon/struct/friendintimacyparam.h"
#include "servercommon/struct/cowreportgoodnewsparam.h"
#include "servercommon/struct/global/worldarenaparam.hpp"
#include "servercommon/struct/global/worldteamarenaparam.hpp"
#include "servercommon/cycleactivity/lanternriddieparam.hpp"
#include "gameworld/gameworld/protocol/msgguild.hpp"
#include "servercommon/struct/bigcross/universeteamarena/universeteamarenadef.hpp"
#include "servercommon/roleactivity/ragodbeastadventparam.hpp"
#include "gameworld/gameworld/protocol/randactivity/msgraservercompetition.hpp"
#include "servercommon/roleactivity/rahorcruxtianchengparam.hpp"
#include "servercommon/struct/global/kuafuyingxiongludef.hpp"

#pragma pack(push) 
#pragma pack(4)

namespace crossgameprotocal
{

	/*
	* Э����������
	* GAME_CROSS ��ʾ����Ϸ���������
	* CROSS_HIDDEN ��ʾ�ӿ���������ط�
	* HIDDEN_CROSS ��ʾ�����ط��������
	* CROSS_GAME ��ʾ�ӿ��������Ϸ��
	* GAME_HIDDEN ��ʾ����Ϸ�����ɿ��ת�������ط�
	* HIDDEN_GAME ��ʾ�����ط����ɿ��ת������Ϸ��
	* GAME_TO_GAME ��ʾ����Ϸ�������������һ����Ϸ��
	*/
	//ADDMARK 
	enum _MsgType
	{
		MT_INVALID,

		MT_CROSS_GAME_BEGIN = 29000,
		MT_GAME_CROSS_REGISTER_GAME_SERVER,				// ע��gameserver
		MT_CROSS_GAME_HELLO,							// cross��game��������
		MT_CROSS_HIDDEN_REGISTER_GAME_SERVER,			// ���ط�ע��gameserver
		MT_HIDDEN_GAME_REGISTER_GAME_SERVER_RET,		// ���ط�ע��gameserver�������
		MT_GAME_CROSS_START_CROSS_REQ,					// ���������ɫ���
		MT_CROSS_HIDDEN_START_CROSS_REQ,				// ����ѱ���������ת�������ط�
		MT_HIDDEN_CROSS_USER_LOGIN,						// ֪ͨcross��ɫ�ѵ�¼�����ط�
		MT_HIDDEN_CROSS_USER_LOGOUT,					// ֪ͨcross��ɫ�˳����ط�
		MT_HIDDEN_CROSS_START_CROSS_ACK,				// ���ط����ؿ������
		MT_CROSS_GAME_START_CROSS_ACK,					// ����������ط���������󷵻�
		MT_CROSS_GAME_SYNC_CROSS_USER,					// ���ͬ����Ϸ�����ڿ������ҵ�����
		MT_GAME_CROSS_KICK_CROSS_USER,					// game����cross�ߵ������ɫ
		MT_CROSS_HIDDEN_KICK_USER,						// cross����hidden����
		MT_HIDDEN_CROSS_KICK_USER_ACK,					// hidden���˷���cross
		MT_CROSS_GAME_KICK_USER_ACK,					// cross���˷���game
		MT_CROSS_GAME_USER_ENTERING_HIDDEN,				// �����֪ԭ����������ڵ�¼���ط�
		MT_HIDDEN_CROSS_USER_ENTERING_HIDDEN,			// ���ط���֪�����������ڵ�¼���ط�
		MT_GAME_CROSS_USER_ALLOWED_ENTER_HIDDEN,		// ԭ����֪�������ұ������¼���ط�
		MT_CROSS_HIDDEN_USER_ALLOWED_ENTER_HIDDEN,		// �����֪���ط�����ұ������¼���ط�
		MT_HIDDEN_CROSS_TEAMMATE_JOIN_TEAM,				// ���ط���֪�������ҿ��Կ�ʼ����������
		MT_CROSS_GAME_TEAMMATE_JOIN_TEAM,				// �����֪��Ϸ������ҿ��Կ�ʼ����������
		MT_GAME_HIDDEN_MESSAGE,							// �����������ط����������ת��
		MT_HIDDEN_GAME_MESSAGE,							// ���ط����򱾷����������ת��
		MT_GAME_HIDDEN_USER_START_CROSS_MATCH,			// ��ҿ�ʼ���ƥ��
		MT_GAME_HIDDEN_USER_STOP_CROSS_MATCH,			// ���ֹͣ���ƥ��
		MT_HIDDEN_GAME_SYNC_CROSS_TEAM_INFO,			// ���ط���ԭ��ͬ�����������Ϣ
		MT_GAME_HIDDEN_TRADE_MARKET_USER_SHELVE_INFO,	// ԭ�������ط�������ҵĽ�������Ϣ
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_ID,		// ԭ�������ط�������������ID����
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_TYPE,		// ԭ�������ط�������������type����
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_BIG_TYPE,	// ԭ�������ط�������������bigtype����
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_TYPE_WITH_LEVEL, // ԭ�������ط�������������type��level����
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_MEDAL_BY_COLOR,	// ���ط���ԭ��������������color������
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_MEDAL_BY_SP_TYPE_AND_COLOR, // ���ط���ԭ����������������Ч���ͺ���ɫ������
		MT_GAME_HIDDEN_TRADE_MARKET_GET_TRANSACTION_RECORD,	// ԭ�������ط�������������ȡ��������
		MT_GAME_HIDDEN_TRADE_MARKET_GET_ROLE_TRADE_RECORD,	// ԭ�������ط�������������ȡ�Լ��Ľ��׼�¼
		MT_GAME_HIDDEN_TRADE_MARKET_GET_ROLE_FOCUS_LIST,	// ԭ�������ط�������������ȡ�Լ��Ĺ�ע�б�
		MT_GAME_HIDDEN_TRADE_MARKET_GET_FOCUS_ITEM_INFO,	// ԭ�������ط�������������ȡ������Ǹ���Ʒ����Ϣ
		MT_GAME_HIDDEN_TRADE_MARKET_REMOVE_FOCUS_ITEM,		// ԭ�������ط��������������һ����ע����Ʒ
		MT_GAME_HIDDEN_TRADE_MARKET_ADD_FOCUS_ITEM,			// ԭ�������ط��������������һ����ע����Ʒ
		MT_GAME_HIDDEN_TRADE_MARKET_GET_MERCHANDISE_NUM,	// ԭ�������ط�������������ȡ��Ʒ������Ϣ
		MT_GAME_HIDDEN_TRADE_MARKET_GET_SELLING_LIST,		// ԭ�������ط�������������ȡĳ����Ʒ������������Ϣ

		MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_BUY,			// ԭ�������ط�����������������
		MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_RESP,		// ���ط���ԭ�����ع���������
		MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_BUY_CONFIRM,	// ԭ�������ط�����������������ȷ�ϵ�
		MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_CONFIRM_RESP, // ���ط���ԭ�����ع���ȷ��������

		MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_CONFIRM_ERROR_RESP, // ���ط���ԭ�����ع������������

		MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_UPSHELVE,		// ԭ�������ط������ϼ�
		MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_UPSHELVE_RESP,	// ���ط���ԭ�����󷵻��ϼܽ��

		MT_GAME_HIDDEN_TRADE_MARKET_OFFSHELVE_REQ,			// ԭ�������ط������¼� 
		MT_HIDDEN_GAME_TRADE_MARKET_OFFSHELVE_RESP,			// ���ط���ԭ�����󷵻��¼ܽ��

		MT_GAME_HIDDEN_TRADE_MARKET_REDO_UPSHELVE_REQ,		// ԭ�������ط����������ϼ�
		MT_HIDDEN_GAME_TRADE_MARKET_REDO_UPSHELVE_RESP,		// ���ط���ԭ�����󷵻������ϼܽ��

		MT_GAME_HIDDEN_TRADE_MARKET_CASH_OUT_REQ,			// ԭ�������ط���������
		MT_HIDDEN_GAME_TRADE_MARKET_CASH_OUT_RESP,			// ���ط���ԭ���������ֽ��

		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_PET_FOR_QUICK_BUY,// ԭ�������ط����������������ڿ��ٹ���
		MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_ADVERTISE_REQ,	// ԭ�������ط������������˴������Ʒ

		MT_GAME_HIDDEN_TRADE_MARKET_ADVERTISE_REQ,			// ԭ�������ط����󷢹��
		MT_HIDDEN_GAME_TRADE_MARKET_ADVERTISE_RESP,			// ���ط���ԭ�����ط������ۿ�
		MT_GAME_HIDDEN_TRADE_MARKET_ADVERTISE_CONFIRM_BOARDCAST, // ԭ���ۿ�������ط�ȷ�Ϸ��͹㲥


		MT_GAME_HIDDEN_USER_APPLY_JOIN_CROSS_TEAM,			// ����������������
		MT_HIDDEN_GAME_SEND_MAIL_TRANSFER,					// ���ط���ԭ��ת���ʼ�
		MT_GAME_TO_GAME_ROLE_CHAT_TRANSFER,					// ��Ҵ���Ϸ����������Ϣ���͵���һ����Ϸ��

		//> -- [����] ------------
		MT_GAME_HIDDEN_GET_FRIEND_INFO,						// ԭ�������ط�  ��ȡ��ҵĺ�����Ϣ
		MT_HIDDEN_GAME_FRIEND_LIST_TO_ROLE,					// ���ط���ԭ��	 ͬ����ҵĺ����б�
		MT_GAME_HIDDEN_ADD_FRIEND_REQ,						// ԭ�������ط�  ͬ�����������Ӻ��Ѳ���
		MT_HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE,				// ���ط���ԭ��  ͬ����ҵĺ��������б���
		MT_GAME_HIDDEN_ADD_FRIEND_REPLY_REQ,				// ԭ�������ط�  ͬ������Ӧ�����
		MT_GAME_HIDDEN_REMOVE_FRIEND_REQ,					// ԭ�������ط�  ͬ��ɾ�����Ѳ���
		MT_GAME_HIDDEN_ADD_BLACK_REQ,						// ԭ�������ط�  ͬ����Ӻ���������
		MT_HIDDEN_GAME_BLACK_LIST,							// ���ط���ԭ��  ͬ����ҵĺ�������Ϣ
		MT_HIDDEN_GAME_BLACK_LIST_CHANGE,					// ���ط���ԭ��  ͬ����ҵĺ�������Ϣ���
		MT_GAME_HIDDEN_REMOVE_BLACK_REQ,					// ԭ�������ط�  ͬ��ɾ������������
		MT_GAME_HIDDEN_FRIEND_GROUP_REQ,					// ԭ�������ط�  ͬ�����������
		MT_HIDDEN_GAME_ASK_LIST_TO_ROLE,					// ���ط���ԭ��  ͬ����ҵĺ��������б�
		MT_GAME_HIDDEN_RECENTLY_REQ,						// ԭ�������ط�  ͬ����Ҵ��������ز���
		MT_HIDDEN_GAME_RECENTLY_INFO,						// ���ط���ԭ��  ͬ����ҵĽ��������Ϣ
		MT_HIDDEN_GAME_FRIEND_CHANGE_TO_ROLE,				// ���ط���ԭ��  ͬ�����ѱ����Ϣ
		MT_GAME_HIDDEN_ROLE_LOGIN_LOGOUT_FRIENDS_NOTIFY,	// ԭ�������ط�  ͬ�������||����֪ͨ�����ط�
		MT_SYNC_FRIEND_INFO,								// ͬ��������Ϣ  , ��֪������ͬ������..��Ҫ���������
		MT_SYNC_ADD_FRINED_OFFLINE_EVENT,					// ͬ���������ʱ��Ҵ�������״̬ , ������ʱ��Ҫ����һ�³ɾ͵���صĲ���
		
		//> -- [���� - ������] ------------
		MT_SYNC_BLACK_INFO,								// ͬ����������Ϣ  , ��֪������ͬ������..��Ҫ���������


		MT_HIDDEN_GAME_REQUEST_GUILD_LIST,				// ���ط�����Ϸ���������м����б�
		MT_GAME_HIDDEN_RESPONSE_GUILD_LIST,				// ��Ϸ�������ط��������м����б�
		MT_GAME_HIDDEN_SYNC_GUILD_MEMBER_CHANGE,		// ��Ϸ�������ط�ͬ�������Ա��� 
		MT_GAME_HIDDEN_SYNC_GUILD_CHANGE,				// ��Ϸ�������ط�ͬ��������
		MT_HIDDEN_GAME_ADD_TITLE_TO_GUILD_MEMBER,		// ���ط�����Ϸ�����󷢷ųƺ�(������Ҷ�)
		MT_GAME_HIDDEN_REQ_SYNC_WORLD_STATUS,			// ��Ϸ�������ط���������״̬
		MT_GAME_HIDDEN_SYNC_WORLD_STATUS,				// ��Ϸ�������ط�����״̬
		MT_GAME_HIDDEN_ACTIVITY_FORCE_TO_NEXT_STATE,	// ��Ϸ�������ط��gm����
		MT_GAME_HIDDEN_REQ_SYNC_ACTIVITY_STATUS,		// ��Ϸ�������ط�����״̬	
		MT_HIDDEN_GAME_SYNC_ACTIVITY_STATUS,			// ���ط�����Ϸ���״̬					
		MT_GAME_HIDDEN_USERCACHE_CHANGE_SYNC,			// ��Ϸ������ͬ��usercache��Ϣ
		MT_HIDDEN_GAME_USERCACHE_CHANGE_SYNC,			// �������Ϸ��ͬ��usercache��Ϣ
		MT_CROSS_HIDDEN_SERVER_DISCONNECT,				// ֪ͨ����������������Ͽ�����
		MT_GAME_CROSS_RA_INFO_SYNC,						// ��Ϸ������ͬ����Ӫ���Ϣ
		MT_CROSS_GAME_RA_INFO_RET,						// �������Ϸ��������Ӫ���Ϣ
		MT_CROSS_HIDDEN_RA_INFO_SYNC,					// ��������ط�ͬ��ԭ������Ӫ���Ϣ
		MT_GAME_HIDDEN_FORCE_GET_GOLD,					// ��Ϸ�������ط�ת����ֵcmd
		MT_GAME_HIDDEN_SYNC_CROSS_PERSON_RANK_INFO,		// ͬ���������а���Ϣ�����
		MT_GAME_HIDDEN_SYNC_CROSS_PET_RANK_INFO,		// ͬ���������а���Ϣ�����
		MT_GAME_HIDDEN_ONLINE_STATUS_SYNC,				// ͬ���������״̬
		MT_CROSS_GAME_USER_MSG,							// cross���û�����Ϣ Protocol
		MT_GAME_CROSS_USER_MSG,							// �û���cross����Ϣ Protocol

		//------------------��ϯ��ѡ-----------------------
		MT_HIDDEN_GAME_CHIEF_ELECTION_REQ,			// ��Ҵ���Ϸ�������������ط�
		//------------------��ϯ�۷�ս-----------------------
		MT_HIDDEN_GAME_CHIEF_PEAK_BATTLE_REQ,			// ��Ҵ���Ϸ�������������ط�
		//------------------��������ս-----------------------
		MT_HIDDEN_GAME_GUILD_HONOR_BATTLE_REQ_ROUTE,
		MT_HIDDEN_GAME_GUILD_HONOR_BATTLE_MATCH_RECORD_SYNC_INFO,	//��������ս���ط�����Ϸ��ͬ��������¼
		//------------------����-----------------------
		MT_HIDDEN_GAME_CHANGE_GUILD_INFO,
		//---------------------------------------
		MT_HIDDEN_GAME_CHAT_ITEM_INFO_REQ,
		MT_GAME_HIDDEN_CHAT_ITEM_INFO_REQ,
		MT_GAME_HIDDEN_CHAT_ITEM_INFO_SYN,

		MT_HIDDEN_GAME_TEAM_MATCH_SUCC_JOIN_TEAM_REQ,
		// -----------------��һ�а�(���˰������Ҷ�)--------------
		MT_HIDDEN_GAME_ADD_TITLE_TO_ROLE,					// ���ط�����Ϸ�����󷢷ųƺ�(��һ�а�)

		MT_CROSS_GAME_HIDDEN_NOT_CONNECTED,					// ���������Ϸ��֪ͨ������ط���δ����
		MT_GAME_CROSS_SEND_ALL_USER,						// ��Ϸ������������ɿ��ת����������Ϸ����ң��������ط���
		MT_GAME_HIDDEN_QUERY_REQ,							// ��Ϸ�������ط�������Ҳ�ѯ
		
		MT_HIDDEN_GAME_NOTICE_LOG,							// ���ط�����Ϸ����log
		MT_HIDDEN_GAME_MAZER_ACER_REQ,						// �Թ����ٴ���Ϸ�������������ط�
		MT_HIDDEN_GAME_COMMON_INFO_NOTICE_HIDDEN,				// ��ɫ��¼��Ϸ��֪ͨ���ط�
		MT_HIDDEN_GAME_FIRST_KILL_INFO_REQ,					// ���ط�����Ϸ��������ɱ��Ϣ
		MT_GAME_HIDDEN_FIRST_KILL_INFO_REQ,					// ��Ϸ�������ط�������ɱ��Ϣ

		MT_GAME_HIDDEN_CHAT_GROUP,							// ��Ϸ�������ط�������������Ϣ
		MT_HIDDEN_GAME_CHAT_GROUP,							// ���ط�����Ϸ��������������Ϣ
		MT_GAME_HIDDEN_CHAT_REQ,							// ��Ϸ�������ط��������������
		MT_HIDDEN_GAME_CHAT_REQ,							// ���ط�����Ϸ���������������

		MT_GAME_HIDDEN_TEMP_GROUP_BECOME_OFFICIAL_REQ,		// ��Ϸ�������ط�������ʱ����Ⱥ��ת������
		MT_HIDDEN_GAME_TEMP_GROUP_BECOME_OFFICIAL_RESP,		// ���ط�����Ϸ��������ʱ����Ⱥ��ת������
		
		MT_HIDDEN_GAME_VIDEO_SIMPLE_INFO_REQ,				// �����ԭ������򵥵�ս����Ϣ
		MT_GAME_HIDDEN_VIDEO_SIMPLE_INFO_REQ,				// ԭ�������ط�����򵥵�ս����Ϣ
		MT_HIDDEN_GAME_VIDEO_DETAIL_INFO_REQ,				// �����ԭ��������ϸ��ս����Ϣ
		MT_GAME_HIDDEN_VIDEO_DETAIL_INFO_RESP,				// ԭ������������ϸ��ս����Ϣ
		MT_GAME_HIDDEN_VIDEO_DETAIL_INFO_REQ,				// ԭ�������ط�������ϸ��ս����Ϣ
		MT_HIDDEN_GAME_VIDEO_DETAIL_INFO_RESP,				// ���ط���ԭ��������ϸ��ս����Ϣ

		MT_HIDDEN_GAME_HUAN_JIE_ZHAN_CHANG_REQ,				// �ý�ս����Ϸ�������������ط�
		MT_HIDDEN_GAME_HUAN_JIE_ZHAN_CHANG_REQ_2,			// �ý�ս����Ϸ�������������ط�2

		MT_HIDDEN_GAME_GUILD_EVENTHANDLER_SEND,				// ���ط�����Ϸ�����ʹ�ʱ��Ҫ��ӵļ�����־��Ϣ
		
		MT_GAME_HIDDEN_SERVER_LOGIN_CHATGROUP_DATA_SYN,		// ��Ϸ����������ͬ��Ⱥ����Ϣ
		MT_HIDDEN_GAME_SERVER_LOGIN_CHATGROUP_DATA_SYN,		// ������ؿ��Ⱥ����Ϣ
		MT_HIDDEN_GAME_CHATGROUP_INFO_SYN,					// �������Ϸ��ͬ��Ⱥ����Ϣ

		MT_HIDDEN_GAME_ONLY_FIGHT_SYN_INFO,					// ���ط�����Ϸ��ͬ����һ�а���Ϣ
		MT_GAME_HIDDEN_ONLY_FIGHT_SYN_INFO_REQ,				// ��Ϸ����������ͬ����һ�а���Ϣ

		MT_GAME_HIDDEN_PLATFORM_BATTLE_REQ,					// ��̨ս��Ϸ�������ط�����
		MT_HIDDEN_GAME_RET_PLATFORM_BATTLE_ROLE_INFO,		// ���ط�������Ϸ����̨ս��ɫ��Ϣ
			
		MT_HIDDEN_GAME_COURAGE_CHALLENGE_RANK_REQ,			// ���ط�����Ϸ��������ħ֮��������Ϣ
		MT_HIDDEN_GAME_COURAGE_CHALLENGE_ROLE_CHANGE,		// ���ط�����Ϸ��֪ͨ��ɫ�й���ħ֮����Ϣ�ı�

		MT_HIDDEN_GAME_SYNC_LOG_QUICK,						// ���ط�����Ϸ��ͬ��LogQuick
		MT_HIDDEN_GAME_SYNC_LOG_TO_ORIGIN_SERVER,			// ���ط�����Ϸ��ͬ����־

		MT_HIDDEN_GAME_GUILD_BOSS_CHAPTER_RANK_REQ,			// ���ط�����Ϸ�����������������������Ϣ

		MT_GAME_HIDDEN_PUBLIC_NOTICE_SYNC,					// ��Ϸ�������ط�ͬ������
		MT_GAME_HIDDEN_ONLINE_INFO_SYNC,					// ��Ϸ�������ط�ͬ���������

		MT_HIDDEN_GAME_INVITE_JOIN_TEAM,				// ���ط�����Ϸ�������������
		MT_GAME_HIDDEN_INVITE_JOIN_TEAM_RET,			// ��Ϸ�������ط�����������ӽ��

		MT_GAME_HIDDEN_INVITE_JOIN_TEAM_IN_ORIGIN,			// ��Ϸ�������ط�����ԭ���������
		MT_HIDDEN_GAME_INVITE_JOIN_TEAM_IN_ORIGIN_RET,		// ���ط�����Ϸ������ԭ��������ӽ��

		MT_GAME_HIDDEN_SYSTEM_MAIL_SYNC,				// ��Ϸ�������ط�����ϵͳ�ʼ�
		MT_HIDDEN_GAME_SYSTEM_MAIL_SYNC,				// ���ط�����Ϸ������ϵͳ�ʼ�

		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_PUBLISH_LIST_REQ,	// ���ط�����Ϸ��������弯Ը�б�
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_RECORD_LIST_REQ,	// ���ط�����Ϸ��������Ҽ�Ը��¼�б�
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_PUBLISH_REQ,		// ���ط�����Ϸ�����󷢲���Ը
		MT_GAME_HIDDEN_GUILD_GATHER_DREAM_PUBLISH_RET,		// ��Ϸ�������ط����ط�����Ը
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_GIFT_REQ,			// ���ط�����Ϸ���������ͼ�Ը
		MT_GAME_HIDDEN_GUILD_GATHER_DREAM_GIFT_RET,			// ��Ϸ�������ط��������ͼ�Ը
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_GIFT_ADD_RECORD,	// ���ط�����Ϸ����Ӽ�Ը��¼
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_FETCH_REQ,		// ���ط�����Ϸ��������ȡ��Ը
		MT_GAME_HIDDEN_GUILD_GATHER_DREAM_FETCH_RET,		// ��Ϸ�������ط�������ȡ���
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_FETCH_CHANGE,		// ���ط�����Ϸ����ȡ��Ը�仯
		MT_HIDDEN_GAME_BRAVE_GROUND_REQ,					// �´��ع���Ϸ�������������ط�
		MT_HIDDEN_GAME_SYNC_HUAN_JIE_ACTIVITY_STATUS,		// ���ط�����Ϸ���ý�ս���״̬		
		MT_HIDDEN_GAME_SYNC_HUAN_JIE_ROLE_INFO,				// ���ط�����Ϸ��ͬ�������Ϣ
		MT_HIDDEN_GAME_GUILD_GATHER_DREAM_ROLE_INFO_CHANGE,	// ���ط�����Ϸ��ͬ����ɫ��Ը��Ϣ

		MT_HIDDEN_GAME_HUAN_JIE_SYNC_TIAN_TI_RECORD,			// ���ط�����Ϸ��ͬ����һý��ս��¼
		MT_GAME_HIDDEN_SYNC_TIAN_TI_RECORD_ACK,				// ��Ϸ������Ϸ��ͬ���ý��ս��¼ȷ��
			
		MT_HIDDEN_GAME_GUILD_CHAT,							// ���ط�����Ϸ������������
		MT_GAME_HIDDEN_GUILD_CHAT,							// ��Ϸ�������ط�����������

		MT_GAME_HIDDEN_SENSITIVE_WORDS_RELOAD,				// ��Ϸ�������ط�֪ͨ�������б���Ҫ�ͻ����ض�
		MT_GAME_HIDDEN_CMD_KICK_ROLE,						// ��Ϸ�������ط����ͣ��ߵ�ĳ���
		MT_GAME_HIDDEN_CMD_MUTE_ROLE,						// ��Ϸ�������ط����ͣ�����ĳ���
		MT_HIDDEN_GAME_CMD_MUTE_ROLE,						// ���ط�����Ϸ�����ͣ�����ĳ��ң����ط��Ҳ�����ң����ܸպ����߻򷵻���ԭ����������ԭ�����ԣ�

		MT_GAME_HIDDEN_CMD_NOTICE_NEW_MESSAGE,				// ��Ϸ�������ط����ͣ�֪ͨ���µĿͷ���Ϣ
		MT_HIDDEN_GAME_CMD_NOTICE_NEW_MESSAGE,				// ���ط�����Ϸ�����ͣ�֪ͨ���µĿͷ���Ϣ

		MT_GAME_HIDDEN_RA_TIANMING,				// ��Ϸ�������ط���������
		MT_HIDDEN_GAME_RA_TIANMING,				// ���ط�����Ϸ����������

		MT_GAME_HIDDEN_CONSUME_CAROUSEL_ADD_RECORD,			// ��Ϸ�������ط������µ�����ת�̼�¼
		MT_HIDDEN_GAME_CONSUME_CAROUSEL_ADD_RECORD,			// ���ط�����Ϸ�������µ�����ת�̼�¼
		// [�������а�:����] --------------------------  
		MT_HIDDEN_GAME_ROLE_INFO_CHANGE,					// �ӿ��ͬ����ɫ��Ϣ��������� (��������ڿ��������Ӱ���������ݱ������Ϣͬ��)

		// [�������а�:����] --------------------------  
		MT_HIDDEN_GAME_PET_INFO_CHANGE,					// �ӿ��ͬ���������а���Ϣ��������� (��������ڿ��������Ӱ���������ݱ������Ϣͬ��)

		MT_GAME_HIDDEN_RESET_PUBLIC_NOTICE,					// ��Ϸ�������ط��������ù���ָ��
		MT_HIDDEN_GAME_SYNC_PLAT_INCROSS_ROLE_NUM,			// ���ط�����Ϸ����������������ڿ��������
		
		MT_HIDDEN_GAME_SYNC_GUILD_MEMBER_CHANGE,			// ���ط�����Ϸ��ͬ�������Ա��� 

		MT_HIDDEN_GAME_SYNC_WORLD_BOSS_INFO,		// ����bossͬ��������
		MT_HIDDEN_GAME_SYNC_WORLD_BOSS2_INFO,		// ����boss2ͬ��������
		MT_HIDDEN_GAME_SYNC_WORLD_BOSS3_INFO,		// ����boss3ͬ��������


		MT_HIDDEN_GAME_WORLD_BOSS_SYNC_RANK_RESULT_MSG,	// ���ط�����Ϸ��ͬ��WorldBoss����Э��
		MT_HIDDEN_GAME_WORLD_BOSS_2_SYNC_RANK_RESULT_MSG,//���ط�����Ϸ��ͬ��WorldBoss2����Э��
		MT_HIDDEN_GAME_WORLD_BOSS_3_SYNC_RANK_RESULT_MSG,//���ط�����Ϸ��ͬ��WorldBoss3����Э��

		MT_HIDDEN_GAME_WORLD_BOSS_SYNC_PARTICIPATE_UID_LIST,	// ���ط�����Ϸ��ͬ��WorldBoss������Ա�б�
		MT_HIDDEN_GAME_WORLD_BOSS_2_SYNC_PARTICIPATE_UID_LIST,	// ��Ϸ�������ط�ͬ��WorldBoss2������Ա�б�
		MT_HIDDEN_GAME_WORLD_BOSS_3_SYNC_PARTICIPATE_UID_LIST,	// ��Ϸ�������ط�ͬ��WorldBoss3������Ա�б�

		MT_HIDDEN_GAME_REQEST_QUANMIN_ACTIVITY_INFO,	// ���ط�����Ϸ������ȫ������
		MT_GAME_HIDDEN_RESPONSE_QUANMIN_ACTIVITY_INFO,	// ��Ϸ�������ط�����ȫ������

		MT_GAME_HIDDEN_WILD_BOSS_INFO_REQ,						// ԭ�������ط�������Ұ��BOSS��Ϣ
		MT_HIDDEN_GAME_SYNC_BRAVE_GROUND_INFO,					// ���ط�ͬ������Ϸ���´��ع�һЩ��Ϣ
		MT_GAME_HIDDEN_FIRST_KILL_REQ,							// ��Ϸ�������ط���ɱ����

		MT_HIDDEN_GAME_ROLE_RETURN_ORIGIN_SERVER_REQ,			// ���ط���ԭ��������ҷ���ԭ��
		MT_GAME_HIDDEN_ROLE_RETURN_ORIGIN_SERVER_ACK,			// ԭ�������ط��ظ���ҷ���ԭ��

		MT_GAME_HIDDEN_TEAMMATE_JOIN_ORIGIN_SERVER_TEAM,		// ԭ�������ط����ͣ��ö��ѻص�ԭ������ӳ��Ķ���
		MT_HIDDEN_GAME_SYNC_HUAN_JIE_INFO,					// ���ط�ͬ������Ϸ���ý�ս��һЩ��Ϣ
		MT_GAME_CROSS_QUERY_RANK_INFO_REQ,						// ��Ϸ�������ط����������ѯ���������Ϣ
		MT_GAME_HIDDEN_BRAVE_GROUND_GET_INFO,		// ԭ�������ط���ȡ�´��ع���Ϣ
		MT_HIDDEN_GAME_BRAVE_GROUND_INFO_RET,		// ���ط���ԭ�������´��ع���Ϣ
		MT_GAME_HIDDEN_BRAVE_GROUND_SET_INFO,		// ԭ�������ط������´��ع���Ϣ

		MT_GAME_HIDDEN_TEST_NETWORK,						// ���Է�������

		MT_HIDDEN_GAME_SPECIAL_LOGIC_ACTIVITY_INFO_SYNC,		// �����һ���Ϣͬ��
		MT_HIDDEN_GAME_GET_RANK_LIST_REQ,						//  ���ط���ԭ���������а�

		MT_GAME_HIDDEN_TRADE_MARKET_HISTORY_TOP_PRICE_REQ,		// ��Ϸ�������ط�����������ʷ��߼���Ʒ��Ϣ�б�

		MT_HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ,			//  ���ط���ԭ�����޽�������
		MT_GAME_HIDDEN_SHEN_SHOU_JIANG_LIN_DRAW_RET,			//  ԭ�������ط��������޽��ٵĳ齱���

		MT_HIDDEN_GAME_HEART_BEAT,								// ���ط�����Ϸ����������
		MT_GAME_HIDDEN_ROLE_LOGIN_SEND_CROSS_CHAT_GROUP_SIMPLE_INFO,// ��Ϸ�������ط���������ҵĿ��Ⱥ����Ϣ
		MT_HIDDEN_GAME_COLOSSEUM_REQ_ROUTER,						// ���ط�ת�����Ǿ�����������Ϸ��
		MT_GAME_HIDDEN_DELETE_MUTE_PLAYER_CHAT_RECORD,					// ��Ϸ�������ط�����ɾ�����Խ�ɫ�������¼

		MT_GAME_HIDDEN_TRADE_MARKET_SALE_ITEM_EVENTHANDLER,				// �����е������۳ɹ�,���ԭ��������,ԭ�������ط������¼�����
		MT_HIDEEN_GAME_TRADE_MARKET_SALE_ITEM_EVENTHANDLER,				// �����е������۳ɹ�,���ԭ��������,���ط���ԭ�������¼�����

		MT_GAME_HIDDEN_TRADE_CREDIT_ADD_CREDIT_EVENTHANDLER,			// ������������,���ԭ��������,ԭ�������ط������¼�����
		MT_GAME_HIDDEN_CHAT_GROUP_LEADERSHIP_HANDOVER,					// �ƽ�Ⱥ��Ⱥ��

		MT_HIDDEN_GAME_OTHER_SERVER_ROLE_ONLINE_STATUS_SYNC,			// ͬ������������״̬

		MT_GAME_HIDDEN_SHAN_HAI_BOSS_INFO_REQ,							// ԭ�������ط�������ɽ��BOSS��Ϣ
		MT_GAME_HIDDEN_ROLE_INFO_NOTICE_REQ_ROUTE,							
		MT_GAME_HIDDEN_RELIC_LOOTING_REQ,								// ��Ϸ�������ط������ż��ᱦ����
		MT_HIDDEN_GAME_RELIC_LOOTING_REQ,								// ���ط�����Ϸ�������ż��ᱦ����

		MT_GAME_HIDDEN_SYNC_CLOUD_ARENA_USER_DATA_REQ,					// ��Ϸ�������ط�����ͬ���������
		MT_HIDDEN_GAME_SYNC_CLOUD_ARENA_USER_DATA_RESP,					// ���ط�����Ϸ�������������
		
		MT_GAME_HIDDEN_CLOUD_ARENA_REAL_TIME_INFO_REQ,					// ��Ϸ�������ط��������ʵʱ����������Ϣ
		MT_GAME_HIDDEN_ROLE_LOGIN_CLOUD_ARENA_USER_INFO_REQ,			// ��Ϸ�������ط����������Ϣ

		MT_GAME_HIDDEN_MARKET_BUY_ITEM_RECORD_SYN_REQ,					// ��Ϸ�������ط�ͬ���¹���Ľ��׼�¼
		MT_HIDDEN_GAME_MARKET_BUY_ITEM_RECORD_SYN_REQ,					// ���ط�����Ϸ��ͬ���¹���Ľ��׼�¼
		MT_GAME_HIDDEN_MARKET_ALL_TRANSACTION_RECORD_SYN_REQ,			// ��Ϸ�������ط�ͬ�����н��׼�¼
		MT_HIDDEN_GAME_MARKET_ALL_TRANSACTION_RECORD_SYN_REQ,			// ���ط�����Ϸ��ͬ�����н��׼�¼

		MT_GAME_HIDDEN_VIDEO_CHANGE_REQ,								// ��Ϸ��ѯ�ʿ���Ƿ���Ҫ�滻¼��
		MT_HIDDEN_GAME_VIDEO_CHANGE_RET,								// ��������Ƿ���Ҫ�滻¼��

		MT_GAME_CROSS_QUERY_CHIVALROUS_RANK_LIST_TOP100_REQ,			// ��Ϸ��/���ط�ѯ�ʿ��top100����ֵ���а�
		MT_GAME_CROSS_QUERY_CHIVALROUS_RANK_LIST_ROLE_LIST_REQ,			// ��Ϸ��/���ط�ѯ�ʿ��ָ����ɫ�б�����а�����
		MT_CROSS_GAME_CHIVALROUS_RANK_LIST_RESP,						// ���������Ϸ��/���ط�����ֵ���а�����

		MT_GAME_HIDDEN_REQ_JOIN_TEAM_INFO,				// ��Ϸ�������ط����������Ϣ
		MT_HIDDEN_GAME_JOIN_TEAM_INFO_RET,				// ���ط�����Ϸ�����Ͷ�����Ϣ

		MT_HIDDEN_GAME_SYNC_CLOUD_ARENA_SEASON_INFO,	// ���ط�����Ϸ����������Ϣ
		
		//����������а�
		MT_GAME_CROSS_SYNC_CROSS_GUILD_RANK_INFO,				// ����������ͬ��������а���Ϣ
		MT_CROSS_GAME_SYNC_GUILD_RANK_REWARD_RANK_INFO,			// �ɿ��������ͬ������������Ϣ[��Ҫ�������а���]
		MT_HIDDEN_GAME_ROLE_ACTIVE_CHANGE,						// �����ط�ͬ����ɫ��Ծ����Ϣ���������
		MT_GAME_HIDDEN_GUILD_ACTIVE_INFO_CHANGE,				// �����������ط�ͬ�������Ծ�ȱ��
		MT_HIDDEN_GAME_ROLE_ACTIVE_GIFT_INFO,					// �����ط�ͬ����ɫ�����ȡ��Ϣ������
		MT_GM_GAME_CROSS_GUILD_ACTIVE_TEST,						// ���������������һ�ν���
		MT_GM_CROSS_GAME_GUILD_ACTIVE_TEST,						// �ɿ���������������а���Ϣ

		MT_HIDDEN_GAME_PERSON_RANK_FRIEND_REQ,			//���ط�����Ϸ������������к��Ѱ�

		// ���ϵͳ
		MT_GAME_HIDDEN_SYNC_HONG_BAO_INFO,				// �����������ط�ͬ�������Ϣ
		MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_GIVE,			// �����ط�������ͬ�����������
		MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_GET,			// �����ط�������ͬ����������
		MT_GAME_HIDDEN_SYNC_HONG_BAO_REQ_GIVE_RET,		// �����������ط�ͬ��������������
		MT_GAME_HIDDEN_SYNC_HONG_BAO_REQ_GET_RET,		// �����������ط�ͬ�������������
		MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_ERROR,			// ��������ͬ��������������ط�ʧ��ʱ[�Ҳ������],��ͬ��������������
		
		MT_GAME_HIDDEN_GIVE_GIFT_REQ,			// ����Ϸ�������ط������������
		MT_GAME_HIDDEN_GIVE_GIFT_ADD_GIFT,			// ����Ϸ�������ط��������
		MT_GAME_HIDDEN_GIVE_GIFT_INFO_RET,			// ��Ϸ��������ȡ���ﷵ����Ʒ��Ϣ

		MT_HIDDEN_GAME_SHI_TU_SEEK_REQ,			// ��Ϸ��������ȡ���ﷵ����Ʒ��Ϣ

		MT_HIDDEN_GAME_BIG_DIPPER_SYNC_RET,				// ���������������ͬ����ԭ��

		MT_GAME_HIDDEN_HUNDRED_GHOST_BOSS_INFO_REQ,		// ԭ�������ط��������ٹ�ҹ��BOSS��Ϣ

		MT_HIDDEN_GAME_CONTINUE_CROSS_BATTLE,		// ���ط���ԭ����ҵ��������ս��

		// ��ţ��ϲ
		MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_CHECK_CAN_BUY,		// ���ط� ->����  ����ܷ���ţ��ϲ 
		MT_GAME_HIDDEN_COW_REPORT_GOOD_NEWS_CHECK_CAN_BUY_ACK,	// ���� ->���ط�  ����ܷ���ţ��ϲӦ�� 
		MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_BUY_ITEM,			// ���ط� ->����  ����ţ��ϲ 
		MT_GAME_HIDDEN_COW_REPORT_GOOD_NEWS_BUY_ITEM_ACK,		// ���� ->���ط�  ����ţ��ϲ Ӧ�� 
		MT_SYNC_COW_REPORT_GOOD_NEWS_BUY_ITEM_OFFLINE_EVENT ,	// ���������� ->���ط�  Ҳ���������ط� ->����  ���߼�  ����ţ��ϲ����ʧ�ܵ�Ԫ�������¼���
		MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO,			// ���ط� ->����  ��ȡ��ţ��ϲ����Ϣ  


		MT_HIDDEN_GAME_SYNC_CROSS_DATA,				// ���ط���ԭ����ͨ�� , ����ԭ��ͬ��������ݵ����ط�

		MT_GAME_HIDDEN_CREAT_AUTO_ROBOT_INFO,		// ԭ��->���ط� �Զ������� ����֪ͨ
		MT_HIDDEN_GAME_TIAN_DI_DAO_HEN_SYNC_INFO,		// ���ط���ص���ͬ����Ϣ����Ϸ��

		MT_GAME_HIDDEN_SYN_COLOSSENUM_RANK_INFO,		// ��Ϸ�������ط�ͬ��������������Ϣ	

		//// [�������ܶ�] ////////////////////////////////////////////////////////
		MT_SYNC_FRIEND_INTIMACY_INFO,					// ͬ���������ܶ���Ϣ ,���� ���ط�->ԭ��  Ҳ������ ԭ��->���ط�,������Ҫ���߼�
		MT_GAME_HIDDEN_ADD_INTIMACY_REQ,				// �����������ط�   ͬ��������ܶȲ�����Ϣ
		MT_GAME_HIDDEN_INTIMACY_RELATION_REQ,			// �����������ط�   ͬ���������ܹ�ϵ����
		MT_GAME_HIDDEN_INTIMACY_SINGLE_CHAT_TIMES,		// �����������ط�   ͬ���������ܶȵ�˽��ʱ���
		MT_GAME_HIDDEN_GIVE_FLOWER_NOTICE,				// �����������ط�   ͬ�������ͻ���Ϊ�㲥 , ���ڹ㲥��Ч�ʹ���
		MT_HIDDEN_GAME_GIVE_FLOWER_NOTICE,				// �����ط�������   ͬ�������ͻ���Ϊ�㲥 , ���ڹ㲥��Ч�ʹ���
		MT_GAME_HIDDEN_GIVE_FLOWER_BACK_KISS,			// �����������ط�   ͬ�������ͻ�����
		MT_HIDDEN_GAME_GIVE_FLOWER_BACK_KISS,			// �����ط�������   ͬ�������ͻ�����

		MT_HIDDEN_GAME_ACTIVITY_END_INFO,				// ���ط�����Ϸ��ͬ���������Ϣ
		MT_HIDDEN_GAME_SERVER_COURSE_INFO_REQ,			// ���ط�����Ϸ���������������Ϣ
		MT_HIDDEN_GAME_SERVER_COURSE_CHANGE_REQ,		// ���ط�����Ϸ������������Ϣ�ı�
		MT_HIDDEN_GAME_ROLE_WIN_CHIEF_ELECTION_SYN,		// ���ط�����Ϸ��ͬ����ɫ���������ϯ

		MT_GAME_HIDDEN_JIE_YI_ZU_CREATE,		// ��Ϸ�������ط������鴴��
		MT_GAME_HIDDEN_JIE_YI_ZU_REQ,			// ��Ϸ�������ط����������
		MT_HIDDEN_GAME_JIE_YI_ZU_INFO_SYNC,		// ���ط�����Ϸ��ͬ��������Ϣ		
		MT_HIDDEN_GAME_JIE_YI_ZU_DISMISS,		// ���ط�����Ϸ����ɢ		
		MT_GAME_HIDDEN_JIE_YI_ZU_NAME,				// ��Ϸ�������ط�����������	
		MT_GAME_HIDDEN_JIE_YI_ZU_MEMBER_NAME,		// ��Ϸ�������ط��������Ա����
		MT_GAME_HIDDEN_JIE_YI_ZU_TEXT,		// ��Ϸ�������ط�������༭����
		MT_GAME_HIDDEN_JIE_YI_ZU_LEAVE,		// ��Ϸ�������ط��������뿪
		MT_GAME_HIDDEN_JIE_YI_ZU_KICK,		// ��Ϸ�������ط�����������
		MT_GAME_HIDDEN_JIE_YI_ZU_TAN_HE,		// ��Ϸ�������ط������鵯��
		MT_GAME_HIDDEN_JIE_YI_ZU_TAN_HE_VOTE,		// ��Ϸ�������ط������鵯��ͶƱ
		MT_GAME_HIDDEN_JIE_YI_ZU_INVITE_REPLY,
		MT_GAME_HIDDEN_JIE_YI_ZU_ROLE_XUAN_YAN,		// ��Ϸ�������ط��Լ�
		MT_GAME_HIDDEN_JIE_YI_ZU_XUAN_YAN,		// ��Ϸ�������ط�����������
		MT_GAME_HIDDEN_JIE_YI_XUAN_YAN_LIST,		// ��Ϸ�������ط������������б�
		MT_HIDDEN_GAME_JIE_YI_SYNC_ROLE_DATA,		// ���ط�����Ϸ��ͬ�������ɫ����
		MT_HIDDEN_GAME_JIE_YI_ROLE_CHANGE,		// ���ط�����Ϸ��ͬ�������ɫ�仯

		MT_HIDDEN_CROSS_ENDLESS_RANK_REWARD,	// �޾�֮��������н���
		MT_CROSS_HIDDEN_ENDLESS_RANK_REWARD_RET,// �޾�֮��������н���

		MT_HIDDEN_GAME_SYNC_ROLE_BATTLE_DATA,	// ͬ������ս�����ݵ�ԭ��
		MT_GAME_HIDDEN_GIVE_FLOWER_CHANGE,		// ��Ϸ�������ط��޸��ͻ���������Ϣ
		MT_HIDDEN_GAME_GIVE_FLOWER_CHANGE,		// ���ط�����Ϸ���޸��ͻ���������Ϣ

		
		MT_HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ,			//���ط���ԭ�������������
		MT_GAME_HIDDEN_HORCRUX_TIAN_CHENG_DRAW_RET,		//ԭ�������ط����ػ�����ɵĳ齱���

		MT_GAME_HIDDEN_QUERY_ITEM_RECOMMEND_PRICE,	// ԭ�������ط�����������Ʒ�Ƽ��۸�

		MT_HIDDEN_GAME_QUERY_TARGET_GLADIATOR_BEFORE_FIGHT_REQ, // �����ս�����������ط���ԭ������Ŀ��ս������
		MT_GAME_HIDDEN_QUERY_TARGET_GLADIATOR_BEFORE_FIGHT_RESP,// ԭ�������ط�����Ŀ���ս�����ݣ�����ʼս��
		MT_HIDDEN_GAME_COLOSSEUM_SYS_REPORT,		// �����ս��������ͬ��ս�������ԭ��
		MT_GAME_HIDDEN_COLOSSEUM_SYNC_NEW_RANK_AFTER_SYS_REPORT,// ԭ��������ս�������������������ؿ������

		MT_HIDDEN_GAME_JIXING_GAOZHAO_REQ,				//���ط���ԭ�����Ǹ�������
		MT_GAME_HIDDEN_JIXING_GAOZHAO_DRAW_RET,			//ԭ�������ط����ؼ��Ǹ��յĳ齱���

		MT_GAME_CROSS_HOTFIX,							// ֪ͨ�����ȡ�ȸ��´���

		MT_GAME_HIDDEN_QUERY_IF_SYNC_GUILD_SUCCEED,		// ��Ϸ�������ط������ѯ�����Ƿ���ͬ���������ݳɹ�
		MT_HIDDEN_GAME_SYNC_GUILD_SUCC_NOTICE,			// ���ط�����Ϸ��֪ͨ����������ͬ���ɹ�
			
		// �����ɸ�
		MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_CHECK_CAN_BUY,		// ���ط� ->����  ����ܷ��������ɸ�
		MT_GAME_HIDDEN_TIGER_BRINGS_BLESSING_CHECK_CAN_BUY_ACK,	// ���� ->���ط�  ����ܷ��������ɸ�Ӧ�� 
		MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_BUY_ITEM,			// ���ط� ->����  ���������ɸ�
		MT_GAME_HIDDEN_TIGER_BRINGS_BLESSING_BUY_ITEM_ACK,		// ���� ->���ط�  ���������ɸ� Ӧ�� 
		MT_SYNC_TIGER_BRINGS_BLESSING_BUY_ITEM_OFFLINE_EVENT,	// ���������� ->���ط�  Ҳ���������ط� ->����  ���߼�  ���������ɸ�����ʧ�ܵ�Ԫ�������¼���
		MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO,			// ���ط� ->����  ��ȡ�����ɸ�����Ϣ  

		MT_CROSS_GAME_DATA_SYNC_CHECK_ACK,						//  ��� - > ���� �� ���ط� -> ���� ����ͬ����֤��ִ

		MT_GAME_HIDDEN_SYNC_SPECIAL_INFO_REQ,					// ԭ�������ط�����ͬ��һЩ��������
		MT_HIDDEN_GAME_SYNC_SPECIAL_INFO_RET,					// ���ط���ԭ��������������

		MT_GAME_HIDDEN_QING_YUAN_CREATE,			// ��Ϸ�������ط���Ե����
		MT_HIDDEN_GAME_QING_YUAN_DISMISS,		// ���ط�����Ϸ����Ե��ɢ	
		MT_GAME_HIDDEN_QING_YUAN_LEAVE,
		MT_GAME_HIDDEN_QING_YUAN_SINGLE_LEAVE,
		MT_GAME_HIDDEN_QING_YUAN_FORCE_LEAVE,
		MT_GAME_HIDDEN_QING_YUAN_CANCEL_LEAVE,
		MT_HIDDEN_GAME_QING_YUAN_INFO_SYNC,		
		MT_GAME_HIDDEN_QING_YUAN_GIFT_GiVE,		// ��Ϸ�������ط���������
		MT_GAME_HIDDEN_QING_YUAN_GIFT_FETCH_REWARD,		// ��Ϸ�������ط���ȡ�������ｱ��
		MT_HIDDEN_GAME_QING_YUAN_GIFT_FETCH_REWARD_RET,		// ���ط�����Ϸ����ȡ�������ｱ������
		MT_GAME_HIDDEN_QING_YUAN_GIFT_FETCH_DAY_REWARD,
		MT_HIDDEN_GAME_QING_YUAN_GIFT_FETCH_DAY_REWARD_RET,
		MT_GAME_HIDDEN_QING_YUAN_SET_SKILL_FLAG,

		MT_HIDDEN_CROSS_FALLEN_GOD_RANK_REWARD,		// ����֮�ؿ�����н���
		MT_CROSS_HIDDEN_FALLEN_GOD_RANK_REWARD_RET,	// ����֮�ؿ�����н���

		//��Ե-����
		MT_G_H_WEDDING_ROLE_REQ,		//���ֽ�ɫ����ת��
		MT_H_G_WEDDING_INFO_SYNC,		//������Ϣ֪ͨ

		MT_GAME_HIDDEN_QING_YUAN_COMMON_DATA,		// ��Ϸ�������ط� ͬ����������

		MT_GAME_HIDDEN_WORLD_ARENA_OPER_TRANSFER,	// ��Ϸ�������ط�ת���۽�����
		MT_GAME_HIDDEN_WORLD_ARENA_SIGNUP,			// ��Ϸ�������ط������۽�
		MT_HIDDEN_GAME_WORLD_ARENA_GIVE_WIN_BOX_REWARD,	// ���ط�����Ϸ���·�ʤ������
		MT_HIDDEN_GAME_WORLD_ARENA_GIVE_SIGNUP_REWARD,	// ���ط�����Ϸ���·���������
		MT_HIDDEN_GAME_WORLD_ARENA_GIVE_DAILY_REWARD,	// ���ط�����Ϸ���·�ÿ�ս���
		MT_GAME_HIDDEN_WORLD_ARENA_UPDATE_SIGNUP_INFO,  // ��Ϸ�������ط�ͬ����ұ�����Ϣ	

		MT_GAME_CROSS_WORLD_TEAM_ARENA_QUERY_RANK_REQ,			// ��Ϸ��������ѯ���а�
		MT_CROSS_GAME_WORLD_TEAM_ARENA_QUERY_RANK_RET,			// �������Ϸ�����ز�ѯ���
		MT_GAME_HIDDEN_ROLE_INFO_CHANGE_WORLD_TEAM_ARENA_SYN,	// ԭ�������ط�ͬ����ɫ��Ϣ
		MT_HIDDEN_GAME_WORLD_TEAM_ARENA_SYN_ROLE_TITLE_INFO,	// ���ط�����Ϸ��ͬ��Ӣ�ۻ����ɫ�ƺ���Ϣ
		MT_HIDDEN_GAME_WORLD_TEAM_ARENA_SYN_ACTIVITY_INFO,		// ���ط�����Ϸ��ͬ��Ӣ�ۻ�����Ϣ
		MT_GAME_HIDDEN_WORLD_TEAM_ARENA_ACITIVITY_INFO_SYN_REQ, // ��Ϸ�������ط�����ͬ��Ӣ�ۻ�����Ϣ
		MT_GAME_HIDDEN_WORLD_TEAM_ARENA_FINALLY_RANK_REQ,		// ��Ϸ�������ط��������հ���Ϣ

		MT_HIDDEN_GAME_ROLE_QI_FU_CHANGE,						// �����ط�������ֵ����ԭ��
		MT_GAME_HIDDEN_ROLE_QI_FU_CHANGE,						// ԭ����ֵͬ�������

		MT_GAME_HIDDEN_QINGYUAN_DUIDUIPENG_UPDATA_RANK,			// ����Ϸ�������ط� �������а���Ϣ
		MT_GAME_HIDDEN_QINGYUAN_DUIDUIPENG_RANK_REQ,			// ����Ϸ�������ط� �������а���Ϣ
		MT_HIDDEN_GAME_QINGYUAN_DUIDUIPENG_RANK_INFO,			// �����ط�����Ϸ�� �·����а���Ϣ

		MT_GAME_HIDDEN_SYN_LEVEL_COMPLEMENT_INFO,				// ��Ϸ�������ط�ͬ���ȼ����������Ϣ

		MT_HIDDEN_GAME_SYNC_HIDDEN_OPEN_SERVER_TIME,				// ���ط�����Ϸ��ͬ��ͬ�����ط�����ʱ��

		MT_HIDDEN_GAME_GUILD_SPEED_UP_INFO,						// ���ط���ԭ�������������
		MT_GAME_HIDDEN_GUILD_SPEED_UP_INFO,						// ԭ�������ط�ͬ��������Ϣ
		MT_GAME_HIDDEN_GUILD_SYN_BUILD_ALL_INFO,				// ����ͬ�����м��彨����Ϣ
		MT_SYNC_GUILD_BUILDING_NOTIFY_INFO,						// ��ԭ�����߿����һ��ͬ��Ư��Э��

		MT_GAME_HIDDEN_GET_UTA_CHAMPION_RECORD,			// ���� -> ���ط�  ��ȡ���µ�һ����[���] �ھ���¼�ֲ�
		MT_GAME_HIDDEN_TRANSFER_MSG_TO_BIGCROSS,		// ԭ�� -> ���ط� -> ����

		MT_HIDDEN_GAME_SYNC_ROLE_REQ_RET,				// ���ط���ԭ��  ͬ����Ҳ��������Ҫ����errornum

		MT_HIDDEN_GAME_GIVE_LOTTERY_TO_ROLE,			// ���ط���ԭ��  ����ҷ��ž��±�
		MT_HIDDEN_GAME_UTA_GIVE_ITEMS_TO_USER,			// ���ط���ԭ��  ����ҷ���Ʒ

		MT_HIDDEN_GAME_UTA_SYNC_BASE_INFO,		
		MT_HIDDEN_GAME_UTA_SYNC_SCHEDULES,

		MT_GAME_HIDDEN_USER_REQUEST_BIGCROSS_VIDEO,
		MT_HIDDEN_GAME_TRANSFER_BIGCROSS_MSG_TO_ORIGIN,	// ���ط�ת��������Э�鵽ԭ��

		MT_GAME_HIDDEN_USER_LOGIN_IN_ORIGIN,

		MT_GAME_HIDDEN_QUERY_WORLD_TEAM_ARENA_OTHER_INFO,	// ��Ϸ�������ط������ѯӢ�ۻ���������Ϣ
		MT_HIDDEN_GAME_UTA_GIVE_SERVER_ITEMS,

		MT_GAME_HIDDEN_QING_YUAN_LEICHONG_ADD_CHONGZHI,	// ��Ϸ�������ط� ���� ��Ե�۳��ֵ���

		// ��������
		MT_GAME_HIDDEN_GOD_BEAST_ADVENT_BUY_SERVER_LIMIT_ITEM_REQ,	// ԭ��->���ط� ���������޹�����
		MT_HIDDEN_GAME_GOD_BEAST_ADVENT_BUY_SERVER_LIMIT_ITEM_INFO,	// ���ط�->ԭ�� ���ع���������Ϣ
		MT_HIDDEN_GAME_GOD_BEAST_ADVENT_BUY_NUM_INFO,				// ���ط�->ԭ�� ͬ���޹���������,�Լ����������
		MT_GAME_HIDDEN_GOD_BEAST_ADVENT_RECEIVE_TRADE_MARKET_REQ,	// ԭ��->���ط� ������տ�������еĹ��ڵ���

		// UTA
		MT_HIDDEN_CROSS_UTA_QUERY_TEAM_MEMBER_RANK_REQ,				// ���ط���������UTA�����Ա������
		MT_CROSS_HIDDEN_UTA_QUERY_TEAM_MEMBER_RANK_RESP,			// ����������ط�UTA�����Ա������Ϣ

		MT_GAME_HIDDEN_HONGMENG_TIANJIE_REQ,						// ԭ�������ط� ����������� 

		MT_HIDDEN_GAME_SERVER_COMPETITION_RANK_REQ,					// �����ԭ�������·���ƴ���а���Ϣ
		MT_GAME_HIDDEN_SERVER_COMPETITION_RANK_RESP,				// ԭ�����������·���ƴ���а���Ϣ

		MT_GAME_HIDDEN_SALTY_SWEET_BATTLE_CHANGE_REQ,				// ԭ��������������֮�����
		MT_HIDDEN_GAME_SALTY_SWEET_BATTLE_VOTE_REQ,					// �����ԭ����������֮��ͶƱ

		MT_HIDDEN_GAME_TEAM_ROLE_RECRUIT_SYNC,						// �����ԭ��ͬ�������ļ������Ϣ

		MT_GAME_HIDDEN_SYNC_PET_TOP_RANK_LIST,						// ԭ�������ط�ͬ������ǰ10���б�
		MT_GAME_HIDDEN_SYNC_PARTNER_TOP_RANK_LIST,					// ԭ�������ط�ͬ�����ǰ10���б�

		MT_HIDDEN_CROSS_MEILI_RANK_REWARD,							// �����������н�������
		MT_CROSS_HIDDEN_MEILI_RANK_REWARD_RET,						// �����������н�������

		MT_HIDDEN_GAME_FRIEND_INITIMACY_ADD,						// ����ֵ����ԭ��Ѱ������
		MT_GAME_HIDDEN_FRIEND_INITIMACY_ADD,						// ����ֵ�������Ѱ������

		MT_GAME_HIDDEN_SEND_MAIL_TRANSFER_TO_ANOTHER_SERVER,		// ת���ʼ�����һ��ԭ��

		MT_HIDDEN_GAME_QUERY_ORIGIN_SERVER_RA_STATUS,				// ���ط���ԭ������ԭ�����ǰ��״̬
		MT_GAME_HIDDEN_SEND_ORIGIN_SERVER_RA_STATUS,				// ԭ�������ط�����ԭ�����״̬��ֻ�·������ط������ڸ�ԭ�������

		MT_GAME_HIDDEN_CLEAN_QUERY_INFO,							// �����ط������һ���
		MT_HIDDEN_GAME_CLEAN_QUERY_INFO,							// ��ԭ�������һ���

		MT_GAME_HIDDEN_CMD_TO_ROLE,									// ��Ϸ�������ط�����cmd����
		MT_HIDDEN_GAME_CMD_TO_ROLE,									// ���ط�����Ϸ������cmd������ط��Ҳ�����ң����ܸպ����߻򷵻���ԭ�������Է���ԭ��������

		MT_GAME_HIDDEN_QUERY_OFFLINE_BATTLE_DATA_FOR_ROBOT,		
		MT_HIDDEN_GAME_QUERY_OFFLINE_BATTLE_DATA_FOR_ROBOT,	
		MT_GAME_HIDDEN_NOTIFY_OFFLINE_BATTLE_DATA_FOR_ROBOT_RESULT,	
		MT_GAME_HIDDEN_REAL_ROLE_ROBOT_LEAVE_TEAM_NOTICE,
		MT_GAME_HIDDEN_SYNC_OFFLINE_BATTLE_DATA,
		MT_HIDDEN_GAME_QUERY_BATTLE_DATA_FOR_OFFLINE_BATTLE_DATA_SYSTEM, 

		MT_GAME_HIDDEN_REQ_SYN_KUA_FU_YING_XIONG_LU_DATA,			// ԭ����������ͬ�����Ӣ��¼����
		MT_HIDDEN_GAME_RET_SYN_KUA_FU_YING_XIONG_LU_DATA,			// �����ԭ��ͬ�����Ӣ��¼����

		MT_HIDDEN_CROSS_REQ_KUA_FU_YING_XIONG_LU_RANK_END,			// ���ط����Ӣ��¼�񵥽���,�����������Ϣ
		MT_CROSS_HIDDEN_RET_KUA_FU_YING_XIONG_LU_RANK_END,			// ������ؿ��Ӣ��¼�񵥽�������

		MT_HIDDEN_GAME_ADD_SINGLE_CHAT_RECORD,
		MT_HIDDEN_GAME_USER_QUERY_CHAT_RECORD,

		MT_GAME_HIDDEN_HUAN_SHOU_ANN_INFO_REQ,						// ԭ�������ط�������޹�����Ϣ
		MT_GAME_HIDDEN_HUAN_SHOU_ANN_INFO_SYNC,						// ԭ�������ط�ͬ�����޹�����Ϣ
		MT_HIDDEN_GAME_HUAN_SHOU_ANN_INFO_SYNC,						// ���ط���ԭ��ͬ�����޹�����Ϣ

		MT_GC_PROTOCOL_MAX,
		MT_GC_PROTOCAL_BEGIN = 29500,	
	};
	UNSTD_STATIC_CHECK(MT_GC_PROTOCOL_MAX <= MT_GC_PROTOCAL_BEGIN);

	//ADDMARK 
	bool WriteBegin(int msg_type, char* buffer, int max_len, int* out_len);
	bool ReadBegin(int& msg_type, const char* buffer, int max_len, int* out_len);

	struct MessageHeader
	{
		MessageHeader() : msg_type(MT_INVALID) {}
		MessageHeader(int _msg_type) : msg_type(_msg_type) {}
		bool Serialize(TLVSerializer2 * outstream) const { return outstream->Push(msg_type); }
		bool Unserialize(TLVUnserializer2 & instream) { return  instream.Pop(&msg_type); }
			
		int	msg_type;
	};

	struct GameCrossRegisterGameServer
	{
		GameCrossRegisterGameServer() : header(MT_GAME_CROSS_REGISTER_GAME_SERVER) {}
		MessageHeader		header;

		static const int MAX_SERVER_ID_NUM = 1024;

		int					plat_type;
		int					count;
		char				data_access_ip[16];
		int					data_access_port;
		char				gateway_hostname[64];
		unsigned short		gateway_port;
		short				reserve_sh;
		unsigned int		server_open_timestamp;				// ÿ��ĸ��������ʱ��
		long long			role_other_init_param_length;
		int					server_id_list[MAX_SERVER_ID_NUM];
	};

	struct CrossGameHello
	{
		CrossGameHello() : header(MT_CROSS_GAME_HELLO) {}
		MessageHeader	header;

		int is_cross_connected_hidden;
	};

	struct CrossHiddenRegisterGameServer
	{
		CrossHiddenRegisterGameServer() : header(MT_CROSS_HIDDEN_REGISTER_GAME_SERVER) {}
		MessageHeader header;

		static const int MAX_SERVER_ID_NUM = 1024;

		int					plat_type;
		int					count;
		char				data_access_ip[16];
		int					data_access_port;
		char				gateway_hostname[64];
		unsigned short		gateway_port;
		short				reserve_sh;
		unsigned int		server_open_timestamp;				// ÿ��ĸ��������ʱ��
		unsigned int		old_server_open_timestamp;			// ������ϵĿ���ʱ���
		long long			role_other_init_param_length;
		int					server_id_list[MAX_SERVER_ID_NUM];
	};

	enum RegisterGameServerRet
	{
		ROLE_OTHER_INIT_PARAM_ERROR = -2,
		PLAT_TYPE_ERROR = -1,
		SUCC = 0,
	};

	struct HiddenGameRegisterGameServerRet
	{
		HiddenGameRegisterGameServerRet() : header(MT_HIDDEN_GAME_REGISTER_GAME_SERVER_RET) {}
		MessageHeader header;

		struct GatewayInfo
		{
			short				reserve_sh;
			unsigned short		cross_gateway_port;			// ����ת��������loginserver
			char				cross_gateway_hostname[64]; // ����ת��������loginserver
		};

		short				ret;
		short				gateway_num;
		unsigned int		cross_open_server_tamp;//�������ʱ���
		unsigned int		cross_old_server_open_tamp;//����������ϵķ���������ʱ���
		unsigned int		cross_lun_hui_wang_chuan_refresh_tamp;
		int					hidden_server_idx;
		GatewayInfo			gateway_list[5];
	};

	struct GameCrossStartCrossReq
	{
		GameCrossStartCrossReq() : header(MT_GAME_CROSS_START_CROSS_REQ) {}
		MessageHeader header;

		int				plat_type;
		int				uid;
		int				server_id;
		PlatName		pname;

		CrossData_Business cross_business_data;
	};

	struct CrossHiddenStartCrossReq
	{
		CrossHiddenStartCrossReq() : header(MT_CROSS_HIDDEN_START_CROSS_REQ) {}
		MessageHeader header;

		int				plat_type;
		int				uid;
		int				server_id;
		PlatName		pname;

		CrossData_Business cross_business_data;
	};

	struct HiddenCrossUserLogin
	{
		HiddenCrossUserLogin() : header(MT_HIDDEN_CROSS_USER_LOGIN) {}
		MessageHeader	header;

		int				uid;

		int				original_plat_type;
		int				original_uid;
		GameName		original_user_name;
	};

	struct HiddenCrossUserLogout
	{
		HiddenCrossUserLogout() : header(MT_HIDDEN_CROSS_USER_LOGOUT) {}
		MessageHeader	header;

		int				uid;
	};

	struct HiddenCrossStartCrossAck
	{
		HiddenCrossStartCrossAck() : header(MT_HIDDEN_CROSS_START_CROSS_ACK) {}
		MessageHeader header;

		int result;
		int origin_plat_type;
		int origin_uid;
		int origin_server_id;
		PlatName orgin_platname;
		GameName role_name;
		int new_uid;
		int scene_id;
		int last_scene_id;
		unsigned int time;
		
		char gateway_hostname[64];
		int gateway_port;

		SessionKey session_key;
	};

	struct CrossGameStartCrossAck
	{
		CrossGameStartCrossAck() : header(MT_CROSS_GAME_START_CROSS_ACK) {}
		MessageHeader header;

		int result;
		int origin_uid;
		int origin_server_id;

		GameName role_name;
		int new_uid;
		int scene_id;
		int last_scene_id;
		unsigned int time;

		char gateway_hostname[64];
		int gateway_port;

		SessionKey session_key;
	};

	struct CrossGameSyncCrossUser
	{
		CrossGameSyncCrossUser() : header(MT_CROSS_GAME_SYNC_CROSS_USER) {}
		MessageHeader	header;

		enum
		{
			SYNC_CROSS_USER_INIT = 0,
			SYNC_CROSS_USER_ADD,
			SYNC_CROSS_USER_REMOVE,
		};

		static const int MAX_IN_CROSS_USER_NUM = 2048;

		int				sync_type;
		int				count;
		int				in_cross_uid_list[MAX_IN_CROSS_USER_NUM];
	};

	struct GameCrossKickCrossUser
	{
		GameCrossKickCrossUser() : header(MT_GAME_CROSS_KICK_CROSS_USER) {}
		MessageHeader	header;

		int	origin_plat_type;
		int origin_server_id;
		int origin_uid;
	};

	struct CrossHiddenKickUser
	{
		CrossHiddenKickUser() : header(MT_CROSS_HIDDEN_KICK_USER) {}
		MessageHeader	header;

		int	bekick_uid;
		int origin_plat_type;
		int origin_server_id;
		int origin_uid;
	};

	struct CrossHiddenGSDisconnect
	{
	public:
		CrossHiddenGSDisconnect() : header(MT_CROSS_HIDDEN_SERVER_DISCONNECT) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
	};

	struct HiddenCrossKickUserAck
	{
		HiddenCrossKickUserAck() : header(MT_HIDDEN_CROSS_KICK_USER_ACK) {}
		MessageHeader header;

		int	bekick_cross_uid;
		int origin_plat_type;
		int origin_server_id;
		int origin_uid;
	};

	struct CrossGameKickUserAck
	{
		CrossGameKickUserAck() : header(MT_CROSS_GAME_KICK_USER_ACK) {}
		MessageHeader header;

		int	bekick_origin_uid;
	};

	struct CrossGameUserEnteringHidden
	{
		CrossGameUserEnteringHidden() : header(MT_CROSS_GAME_USER_ENTERING_HIDDEN) {}
		MessageHeader header;

		int entering_uid;
		int new_uid;
	};

	struct HiddenCrossUserEnteringHidden
	{
		HiddenCrossUserEnteringHidden() : header(MT_HIDDEN_CROSS_USER_ENTERING_HIDDEN) {}
		MessageHeader header;

		int entering_new_uid;
		int origin_uid;
		int origin_server_id;
		int origin_plat_type;
	};

	struct GameCrossUserAllowedEnterHidden
	{
		GameCrossUserAllowedEnterHidden() : header(MT_GAME_CROSS_USER_ALLOWED_ENTER_HIDDEN) {}
		MessageHeader header;

		int allowed_uid;
		int plat_type;
		int allowed_new_uid;
	};

	struct CrossHiddenUserAllowedEnterHidden
	{
		CrossHiddenUserAllowedEnterHidden() : header(MT_CROSS_HIDDEN_USER_ALLOWED_ENTER_HIDDEN) {}
		MessageHeader header;

		int allowed_uid;
	};

	struct HiddenCrossTeammateJoinCrossTeam
	{
		HiddenCrossTeammateJoinCrossTeam() : header(MT_HIDDEN_CROSS_TEAMMATE_JOIN_TEAM) 
		{
			activity_type = ACTIVITY_TYPE_INVALID;
		}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int cross_team_index;
		int origin_team_index;
		int activity_type;
		int teammate_list[4];
	};

	struct HiddenGameTeamMatchSuccJoinTeam
	{
		HiddenGameTeamMatchSuccJoinTeam() : header(MT_HIDDEN_GAME_TEAM_MATCH_SUCC_JOIN_TEAM_REQ) { }

		MessageHeader header;
		int origin_plat_type;
		int origin_server_id;
		int cross_team_index;
		int role_id;
	};
	
	struct CrossGameTeammateJoinCrossTeam
	{
		CrossGameTeammateJoinCrossTeam() : header(MT_CROSS_GAME_TEAMMATE_JOIN_TEAM) 
		{
			cross_business_data.Reset();
		}
		MessageHeader header;

		int cross_team_index;
		CrossData_Business cross_business_data;
	};

	enum GAME_HIDDEN_MESSAGE_TYPE
	{
		GAME_HIDDEN_MESSAGE_TYPE_DEFAULT,
		GAME_HIDDEN_MESSAGE_TYPE_SEND_ALL_GAME_USER,			//�����������
		GAME_HIDDEN_MESSAGE_TYPE_SEND_GUILD_ALL_MEMBER,			//����ָ��������������
		GAME_HIDDEN_MESSAGE_TYPE_SEND_UNIQUE_SERVER_ALL_USER,	//����ָ���������������
		GAME_HIDDEN_MESSAGE_TYPE_SEND_TO_BIG_CROSS,				//���͵�bigcross
	};

	struct GameHiddenMessage
	{
		GameHiddenMessage() : header(MT_GAME_HIDDEN_MESSAGE) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int type;
		union
		{
			struct
			{
		long long send_to_uuid;		// ����Ϊ0����ֱ�Ӱ����ݷ������(��uuidΪsend_to_cross_user_of_uuid��
			};

			struct 
			{
				int guild_id;
			};

		};

		// ����������
	};

	enum HIDDEN_GAME_MESSAGE_TYPE
	{
		HIDDEN_GAME_MESSAGE_TYPE_DEFAULT,
		HIDDEN_GAME_MESSAGE_TYPE_SEND_ALL_GAME_USER,			//�����������
		HIDDEN_GAME_MESSAGE_TYPE_SEND_GUILD_ALL_MEMBER,		//����ָ��������������
	};

	struct HiddenGameMessage
	{
		HiddenGameMessage() : header(MT_HIDDEN_GAME_MESSAGE) 
		{
			type = HIDDEN_GAME_MESSAGE_TYPE_DEFAULT;
		}
		MessageHeader header;

		int type;
		int origin_plat_type;
		int origin_server_id;

		union 
		{
			struct 
			{
				int send_to_uid;			// ����Ϊ0����ֱ�Ӱ����ݷ������
			};

			struct 
			{
				int guild_id;
			};
		};

		// ����������
	};

	struct GameHiddenTransferMsgToBigcross
	{
		GameHiddenTransferMsgToBigcross() : header(MT_GAME_HIDDEN_TRANSFER_MSG_TO_BIGCROSS) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;

		// ����������
	};

	struct HiddenGameTransferBigcrossMsgToOrigin
	{
		HiddenGameTransferBigcrossMsgToOrigin() : header(MT_HIDDEN_GAME_TRANSFER_BIGCROSS_MSG_TO_ORIGIN) {}
		MessageHeader header;

		// ����������
	};

	struct GameHiddenUserStartCrossMatch
	{
		GameHiddenUserStartCrossMatch() : header(MT_GAME_HIDDEN_USER_START_CROSS_MATCH){}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int origin_role_id;
		int role_level;
		int match_team_type;
		int role_guild_id;
		bool is_auto_create;			// һ��ʱ��ƥ�䲻�����Ƿ��Զ���������[0:�� 1 : ��]
		char reserve_ch;
		short reserve_sh;	
	};

	struct GameHiddenUserStopCrossMatch
	{
		GameHiddenUserStopCrossMatch() : header(MT_GAME_HIDDEN_USER_STOP_CROSS_MATCH) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_role_id;
	};

	struct HiddenGameSyncCrossTeamInfo
	{
		HiddenGameSyncCrossTeamInfo() : header(MT_HIDDEN_GAME_SYNC_CROSS_TEAM_INFO) {}
		MessageHeader header;

		enum NOTIFY_REASON
		{
			NOTIFY_REASON_INIT = 0,
			NOTIFY_REASON_INFO_CHANGE = 1,
			NOTIFY_REASON_INIT_AGAIN = 2,   // ������ʼ��   Ӧ��һ�η���������
			NOTIFY_REASON_ADD = 3,			// ����
			NOTIFY_REASON_REMOVE = 4,		// ȥ��
		};

		short notify_reason;
		short team_count;

		TeamItem team_list[MAX_TEAM_LIST_NUM];
	};

	struct GameHiddenTradeMarketUserShelveInfo
	{
		GameHiddenTradeMarketUserShelveInfo() : header(MT_GAME_HIDDEN_TRADE_MARKET_USER_SHELVE_INFO) {}
		MessageHeader header;

		long long uuid;

		int plat_type;					//!< ���ڷ���
		int server_id;					//!< ���ڷ���
	};

	struct GameHiddenTradeMarketSearchById
	{
		GameHiddenTradeMarketSearchById() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_ID) {}
		MessageHeader header;

		long long uuid;					//!< �����ߵ�uuid

		ItemID item_id;					//!< ��ƷID
		short req_page;					//!< �����ҳ��
		short is_announce;				//!< �Ƿ�������ڹ�ʾ�ڵ� 0����   1����
		short order_type;				//!< ��������  0:���۸�͵���   1:���۸�ߵ���
		
		int plat_type;					//!< ���ڷ���
		int server_id;					//!< ���ڷ���
	};

	struct GameHiddenTradeMarketSearchByType
	{
		GameHiddenTradeMarketSearchByType() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_TYPE) {}
		MessageHeader header;

		long long uuid;

		short item_search_type;			//!< ��Ʒ�������� \note �� J-������.xls -> ɸѡsheet
		short req_page;					//!< �����ҳ��

		short is_announce;				//!< �Ƿ�������ڹ�ʾ�ڵ� 0����   1����
		short order_type;				//!< ��������  0:���۸�͵���   1:���۸�ߵ���

		short item_big_search_type;		//!< ֵΪ-1ʱ����С��(item_search_type)�����������򰴴�������
		short param;					//!< //!< ���������������װ��ʱparam����ȼ�������������ʱparam������Чְҵ��(Ϊ-1ʱ��"ȫ��"����)

		int plat_type;					//!< ���ڷ���
		int server_id;					//!< ���ڷ���
	};

	struct GameHiddenTradeMarketSearchByBigType
	{
		GameHiddenTradeMarketSearchByBigType() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_BIG_TYPE) {}
		MessageHeader header;

		long long uuid;

		short item_search_type;			//!< ��Ʒ�������� \note �� J-������.xls -> ɸѡsheet
		short req_page;					//!< �����ҳ��

		short is_announce;				//!< �Ƿ�������ڹ�ʾ�ڵ� 0����   1����
		short order_type;				//!< ��������  0:���۸�͵���   1:���۸�ߵ���

		short item_big_search_type;		//!< ֵΪ-1ʱ����С��(item_search_type)�����������򰴴�������
		short param;					//!< ���������������װ��ʱparam����ȼ�������������ʱparam������Чְҵ��(Ϊ-1ʱ��"ȫ��"����)

		int plat_type;					//!< ���ڷ���
		int server_id;					//!< ���ڷ���
	};

	struct GameHiddenTradeMarketSearchByTypeWithLevel
	{
		GameHiddenTradeMarketSearchByTypeWithLevel() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_BY_TYPE_WITH_LEVEL) {}
		MessageHeader header;

		long long uuid;

		short item_search_type;			//!< ��Ʒ�������� \note �� J-������.xls -> ɸѡsheet
		short req_page;					//!< �����ҳ��

		short is_announce;				//!< �Ƿ�������ڹ�ʾ�ڵ� 0����   1����
		short order_type;				//!< ��������  0:���۸�͵���   1:���۸�ߵ���

		short item_big_search_type;		//!< ֵΪ-1ʱ����С��(item_search_type)�����������򰴴�������
		short level_param;				//!< �ȼ�������������װ��ʱ��(Ϊ0ʱ��"ȫ��"����)

		int plat_type;					//!< ���ڷ���
		int server_id;					//!< ���ڷ���
	};

	struct GameHiddenTradeMarketSearchMedalByColor
	{
		GameHiddenTradeMarketSearchMedalByColor() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_MEDAL_BY_COLOR) {}
		MessageHeader header;

		long long uuid;

		short medal_color;				//!< ������ɫ
		short req_page;					//!< �����ҳ��

		short is_announce;				//!< �Ƿ�������ڹ�ʾ�ڵ� 0����   1����
		short order_type;				//!< ��������  0:���۸�͵���   1:���۸�ߵ���

		int prof_param;					//!< ������Чְҵ���� -1Ϊȫ�� 0Ϊͨ�ã�[1,8]Ϊְҵ

		int plat_type;					//!< ���ڷ���
		int server_id;					//!< ���ڷ���
	};

	struct GameHiddenTradeMarketSearchMedalByEffectTypeAndColor
	{
		GameHiddenTradeMarketSearchMedalByEffectTypeAndColor() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_MEDAL_BY_SP_TYPE_AND_COLOR) {}
		MessageHeader header;

		long long uuid;

		short sp_type;					//!< ��Ч����
		short medal_color;				//!< ������ɫ

		short reserve_sh;				//!< ������
		short req_page;					//!< �����ҳ��

		short is_announce;				//!< �Ƿ�������ڹ�ʾ�ڵ� 0����   1����
		short order_type;				//!< ��������  0:���۸�͵���   1:���۸�ߵ���

		int plat_type;					//!< ���ڷ���
		int server_id;					//!< ���ڷ���
	};

	struct GameHiddenTradeMarketSearchPetForQuickBuy
	{
		GameHiddenTradeMarketSearchPetForQuickBuy() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_PET_FOR_QUICK_BUY) {}
		MessageHeader header;

		int role_id;

		short order_type;						//!< ɸѡ˳�� \see TRADE_PET_INDEX_ORDER
		ItemID item_id;							//!< Ҫ�ѵĳ�����ƷID

		int last_seller_uid;					//!< �ϴ��ѵ������һ����Ʒ������UID ����һ�����������ɸѡ˳��ʱ��0��
		int last_price;							//!< �ϴ��ѵ������һ����Ʒ�ļ۸�	 ����һ�����������ɸѡ˳��ʱ��0��
		unsigned int last_put_on_shelve_time;	//!< �ϴ��ѵ������һ����Ʒ���ϼ�ʱ�䣨��һ�����������ɸѡ˳��ʱ��0��
		short last_str_level;					//!< �ϴ��ѵ������һ����Ʒ��ǿ���ȼ�����һ�����������ɸѡ˳��ʱ��0��
		short quality;							//!< ����Ʒ��

		int plat_type;							//!< ���ڷ���
		int server_id;							//!< ���ڷ���
	};

	struct GameHiddenTradeMarketSearchAdvertiseItemReq
	{
		GameHiddenTradeMarketSearchAdvertiseItemReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_SEARCH_ADVERTISE_REQ) {}
		MessageHeader header;

		int role_id;
		int plat_type;						//!< ���ڷ���
		int server_id;						//!< ���ڷ���

		int seller_uid;						//!< ����uid
		unsigned int put_on_shelve_time;	//!< �ϼ�ʱ��
		int price;							//!< �۸�
		short sale_index;					//!< �ڻ����ϵ����
		ItemID item_id;						//!< ��ƷID
		long long unique_key;				//!< Ψһ��ʶ
	};

	struct GameHiddenTradeMarketGetTransactionRecord
	{
		GameHiddenTradeMarketGetTransactionRecord() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_TRANSACTION_RECORD) {}
		MessageHeader header;

		long long uuid;

		ItemID item_id;						//!< ��ƷID
		short reserve_sh;					//!< ������

		int plat_type;					//!< ���ڷ���
		int server_id;					//!< ���ڷ���
	};

	struct GameHiddenTradeMarketGetRoleTradeRecord
	{
		GameHiddenTradeMarketGetRoleTradeRecord() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_ROLE_TRADE_RECORD) {}
		MessageHeader header;

		long long uuid;

		int plat_type;					//!< ���ڷ���
		int server_id;					//!< ���ڷ���
	};

	struct GameHiddenTradeMarketGetRoleFocusList
	{
		GameHiddenTradeMarketGetRoleFocusList() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_ROLE_FOCUS_LIST){}
		MessageHeader header;

		long long uuid;

		int plat_type;					//!< ���ڷ���
		int server_id;					//!< ���ڷ���
	};

	struct GameHiddenTradeMarketGetFocusItemInfo
	{
		GameHiddenTradeMarketGetFocusItemInfo() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_FOCUS_ITEM_INFO) {}
		MessageHeader header;

		long long uuid;

		int focus_index;					//!< ��Ʒ���ҵĹ�ע�б��е����  \note ���SCTradeMarketGetFocusList�е�focus_index
		short cur_page;						//!< ��ǰ���������һҳ
		short is_announce;					//!< ��ǰ����������ǹ�ʾ�ڻ��Ƿ����ڵĹ�ע�б�

		int plat_type;						//!< ���ڷ���
		int server_id;						//!< ���ڷ���
	};

	struct GameHiddenTradeMarketRemoveFocusItem
	{
		GameHiddenTradeMarketRemoveFocusItem() : header(MT_GAME_HIDDEN_TRADE_MARKET_REMOVE_FOCUS_ITEM) {}
		MessageHeader header;

		long long uuid;

		int focus_index;					//!< ��Ʒ���ҵĹ�ע�б��е����  \note ���SCTradeMarketGetFocusList�е�focus_index
		short cur_page;						//!< ��ǰ���������һҳ
		short is_announce;					//!< ��ǰ����������ǹ�ʾ�ڻ��Ƿ����ڵĹ�ע�б�

		int plat_type;						//!< ���ڷ���
		int server_id;						//!< ���ڷ���
	};

	struct GameHiddenTradeMarketAddFocusItem
	{
		GameHiddenTradeMarketAddFocusItem() : header(MT_GAME_HIDDEN_TRADE_MARKET_ADD_FOCUS_ITEM) {}
		MessageHeader header;

		long long uuid;

		int seller_uid;						//!< ����uid
		short sale_index;					//!< �����һ����ϵ����
		short reserve_sh;					//!< ������
		unsigned int upshelve_time;			//!< ��Ʒ�ϼ�ʱ�� �����ں˶���Ʒ��

		int plat_type;						//!< ���ڷ���
		int server_id;						//!< ���ڷ���
	};

	struct GameHiddenTradeMarketGetMerchandiseNum
	{
		GameHiddenTradeMarketGetMerchandiseNum() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_MERCHANDISE_NUM) {}
		MessageHeader header;

		long long uuid;

		short is_announce;		//!< 0:������   1:��ʾ��
		short big_search_type;	//!< ����  1������	2������  3������  10������

		int plat_type;						//!< ���ڷ���
		int server_id;						//!< ���ڷ���
	};

	struct GameHiddenTradeMarketGetSellingList
	{
		GameHiddenTradeMarketGetSellingList() : header(MT_GAME_HIDDEN_TRADE_MARKET_GET_SELLING_LIST) {}
		MessageHeader header;

		long long uuid;

		ItemID item_id;
		short special_effect_type;

		int plat_type;						//!< ���ڷ���
		int server_id;						//!< ���ڷ���
	};

	struct GameHiddenUserApplyJoinCrossTeam
	{
		GameHiddenUserApplyJoinCrossTeam() : header(MT_GAME_HIDDEN_USER_APPLY_JOIN_CROSS_TEAM) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int origin_role_id;
		int cross_team_index;

		int level;
		int profession;
		long long avatar_timestamp;
		GameName name;
		short avatar_type;
		short headshot_id;

		int guild_id;
	};

	struct HiddenGameSendMailTransfer
	{
		HiddenGameSendMailTransfer() : header(MT_HIDDEN_GAME_SEND_MAIL_TRANSFER) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int recieve_uid;

		MailParam mail_param;
	};

	struct GameHiddenSendMailTransferToAnotherServer
	{
		GameHiddenSendMailTransferToAnotherServer() : header(MT_GAME_HIDDEN_SEND_MAIL_TRANSFER_TO_ANOTHER_SERVER) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int recieve_uid;

		MailParam mail_param;
	};

	struct HiddenGameRequestGuildList
	{
		HiddenGameRequestGuildList() : header(MT_HIDDEN_GAME_REQUEST_GUILD_LIST) {}
		MessageHeader header;
	};

	struct GameHiddenResponseGuildList 
	{
		GameHiddenResponseGuildList() : header(MT_GAME_HIDDEN_RESPONSE_GUILD_LIST) {}
		MessageHeader header;		
		
		int plat_type;
		int server_id;
		short is_first;			// �Ƿ��һ������һ����Ӧ���֮ǰ��Щ��
		short is_last;			// �Ƿ����һ��

		// ������Serialize����AllGuildParam���ݣ�ȡ��ʱ��Ҫ��Deserialize
	};

	struct GameHiddenQueryIfSyncGuildSucceed
	{
		GameHiddenQueryIfSyncGuildSucceed() : header(MT_GAME_HIDDEN_QUERY_IF_SYNC_GUILD_SUCCEED){}
		MessageHeader header;

		int plat_type;
		int server_id;
	};

	struct HiddenGameSyncGuildSuccNotice
	{
		HiddenGameSyncGuildSuccNotice() : header(MT_HIDDEN_GAME_SYNC_GUILD_SUCC_NOTICE) {}
		MessageHeader header;
	};

	// ���͵�cross����crossuser����û����ת����originserver��������ת����hiddenserver
	struct G2GRoleChatTransfer
	{
		bool Push(void * src, unsigned int length)
		{
			if (data_size + length > MAX_DATA_SIZE)
			{
				return false;
			}

			memcpy(data + data_size, src, length);
			data_size += length;

			return true;
		}

		bool Pop(void * dst, unsigned int length)
		{
			if (pop_offset + length > data_size)
			{
				return false;
			}

			memcpy(dst, data + pop_offset, length);
			pop_offset += length;

			return true;
		}

		G2GRoleChatTransfer() : header(MT_GAME_TO_GAME_ROLE_CHAT_TRANSFER) , pop_offset(0), data_size(0){}
		MessageHeader header;

		int plat_type;
		int to_gameserver_id;
		int to_role_id;
		int send_to_hidden_first;		// �Ƿ��ȷ������ط�

		const unsigned int static MAX_DATA_SIZE = 8192;
		unsigned int pop_offset;
		unsigned int data_size;
		char data[MAX_DATA_SIZE];
	};

	enum ReasonType
	{
		SYNC_REASON_ADD = 0,
		SYNC_REASON_REMOVE = 1,
	};

	// ����

	enum GAME_HIDDEN_GET_FRIEND_INFO_TYPE
	{
		GAME_HIDDEN_GET_FRIEND_INFO_TYPE_INVALID= 0,		
		GAME_HIDDEN_GET_FRIEND_INFO_TYPE_FRIEND_LIST = 1,	//�����б�
		GAME_HIDDEN_GET_FRIEND_INFO_TYPE_ASK_LIST = 2,		//���������б�
		GAME_HIDDEN_GET_FRIEND_INFO_TYPE_BLACK_LIST = 3,	//�������б�
	
		GAME_HIDDEN_GET_FRIEND_INFO_TYPE_MAX,
	};

	struct GameHiddenGetFriendInfo						// ԭ�������ط� ͬ���ͻ��� ���Ѳ�������
	{
		GameHiddenGetFriendInfo() : header(MT_GAME_HIDDEN_GET_FRIEND_INFO) {}
		MessageHeader header;

		UniqueServerID	usid;							// �����������������ĸ���
		int role_id;									// ������������ID
		int info_type;									// ��Ҫ��ȡ����Ϣ����
	};
	
	struct HiddenGameFriendListToRole					// ���ط���ԭ�� ͬ����ҵĺ����б�
	{
		HiddenGameFriendListToRole() : header(MT_HIDDEN_GAME_FRIEND_LIST_TO_ROLE) {}
		MessageHeader header;

		UniqueServerID	usid;								// ���ܸ���Ϣ����������ĸ���
		int role_id;										// ���ܸ���Ϣ�����ID
		int				count;								// ��
		FriendInfo		friend_list[MAX_FRIEND_NUM];		//!< ������Ϣ����С100
	};

	struct GameHiddenAddFriendReq						// ԭ�������ط�  ͬ�����������Ӻ��Ѳ���
	{
		GameHiddenAddFriendReq() : header(MT_GAME_HIDDEN_ADD_FRIEND_REQ) {}
		MessageHeader header;

		UniqueServerID usid;
		int role_id;				// ������
		int target_id;				// �������
		bool is_yijian;				// �Ƿ���һ��[0:�� , 1:��]

									// �����˵Ļ�����Ϣ
		GameName role_name;
		int avatar_type;
		int prof;
		int level;
		int sex;
		int headshot_id;
		int top_level;
		unsigned int avatar_timestamp;
	};

	struct HiddenGameSyncRoleReqRet					// ���ط���ԭ��  ͬ����Ҳ��������Ҫ����errornum
	{
		HiddenGameSyncRoleReqRet() : header(MT_HIDDEN_GAME_SYNC_ROLE_REQ_RET) {}
		MessageHeader header;

		int role_id;			// ������
		int error_num;			// �������
	};

	enum HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS
	{
		HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_INVALID = 0,		// ��Ч
		HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_INSERT = 1,		// ���			
		HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_DELETE = 2,		// ɾ��		
		HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE_STATUS_MAX,
	};

	struct HiddenGameFriendAskListChange					// ���ط���ԭ��  ͬ����ҵĺ��������б���
	{
		HiddenGameFriendAskListChange() : header(MT_HIDDEN_GAME_FRIEND_ASK_LIST_CHANGE) {}
		MessageHeader header;

		int role_id;				// ������
		int target_id;				// �������
		char change_type;			// �������
		bool is_yijian;				// �Ƿ���һ��[0:�� , 1:��]
		bool is_send_friend_route;	// �Ƿ��Ѿ����͹����������� [0:û�� , 1:���͹���]

		GameName role_name;
		int avatar_type;
		int prof;
		int level;
		int sex;
		int headshot_id;
		int top_level;
		unsigned int avatar_timestamp;
	};

	struct GameHiddenAddFriendReplyReq			// ͬ���������������� , ��֪������ͬ������..��Ҫ��������� 
	{
		GameHiddenAddFriendReplyReq() : header(MT_GAME_HIDDEN_ADD_FRIEND_REPLY_REQ) {}
		MessageHeader header;

		int role_id;			// ������
		int apply_id;			// Ŀ�����
		int is_accept;			// �Ƿ�ͬ��
	};

	struct GameHiddenRemoveFriendReq			// ԭ�������ط�ͬ�� ɾ�����Ѳ���
	{
		GameHiddenRemoveFriendReq() : header(MT_GAME_HIDDEN_REMOVE_FRIEND_REQ) {}
		MessageHeader header;

		int role_id;			// ������
		int target_id;			// Ŀ�����
	};

	struct GameHiddenAddBlackReq				// ԭ�������ط�  ͬ����Ӻ���������
	{
		GameHiddenAddBlackReq() : header(MT_GAME_HIDDEN_ADD_BLACK_REQ) {}
		MessageHeader header;

		UniqueServerID  usid;	// �����������ĸ���
		int role_id;			// ������
		int target_id;			// Ŀ�����
	};

	struct HiddenGameBlackList								// ���ط���ԭ�� ͬ����ҵĺ������б�
	{
		HiddenGameBlackList() : header(MT_HIDDEN_GAME_BLACK_LIST) {}
		MessageHeader header;

		UniqueServerID	usid;								// ���ܸ���Ϣ����������ĸ���
		int role_id;										// ���ܸ���Ϣ�����ID
		int count;											// ��
		FriendInfo			black_info[MAX_BLACK_NUM];		// ��������Ϣ��
	};

	struct HiddenGameBlackListChange						// ���ط���ԭ�� ͬ����ҵĺ������б���
	{
		HiddenGameBlackListChange() : header(MT_HIDDEN_GAME_BLACK_LIST_CHANGE) {}
		MessageHeader header;

		int role_id;										// ���ܸ���Ϣ�����ID
		int change_type;									// �������
		FriendInfo			black_info;						// ��������ɫ��Ϣ
	};

	struct GameHiddenRemoveBlackReq							// ԭ�������ط�ͬ�� ɾ������������
	{
		GameHiddenRemoveBlackReq() : header(MT_GAME_HIDDEN_REMOVE_BLACK_REQ) {}
		MessageHeader header;

		UniqueServerID usid;	// ���������ڷ�����
		int role_id;			// ������
		int target_id;			// Ŀ�����
	};

	enum GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE
	{
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_INVALID = 0,				// ��Ч
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_DEL_GROUP = 1,			// ɾ����		
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_EDIT_GROUP = 2,			// �༭��	
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_RENAME_GROUP = 3,			// ��������
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_MOVE_GROUP = 4,			// �ƶ���
		GAME_HIDDEN_FRIEND_GROUP_REQ_TYPE_MAX,
	};

	struct GameHiddenFriendGroupReq							// ԭ�������ط�ͬ�� ���������
	{
		GameHiddenFriendGroupReq() : header(MT_GAME_HIDDEN_FRIEND_GROUP_REQ) {}
		MessageHeader header;

		int role_id;						// ������
		int req_type;						// ��������

		int group_type;
		GameName group_name;
		int count;							// ��
		int friend_id_arr[MAX_FRIEND_NUM];	// �ƶ��ĺ��ѽ�ɫid�����ò���100
	};

	struct HiddenGameAskListToRole							// ���ط���ԭ�� ͬ����ҵĺ��������б�
	{
		HiddenGameAskListToRole() : header(MT_HIDDEN_GAME_ASK_LIST_TO_ROLE) {}
		MessageHeader header;

		UniqueServerID	usid;								// ���ܸ���Ϣ����������ĸ���
		int role_id;										// ���ܸ���Ϣ�����ID
		int count;											// ��
		FriendInfo			ask_info[MAX_FRIEND_NUM];		// ���������б���Ϣ
	};
	
	enum GAME_HIDDEN_RECENTLY_REQ_TYPE
	{
		GAME_HIDDEN_RECENTLY_REQ_TYPE_ADD_CHAT,				// ��ӽ��� 
		GAME_HIDDEN_RECENTLY_REQ_TYPE_GET_CHAT_INFO,		// ��ȡ������Ϣ
		GAME_HIDDEN_RECENTLY_REQ_TYPE_GET_TEAM_INFO,		// ��ȡ��������Ϣ
		GAME_HIDDEN_RECENTLY_REQ_TYPE_ADD_TEAM,				// ���������
	};

	struct GameHiddenRecentlyReq							// ԭ�������ط�  ͬ����Ҵ��������ز���
	{
		GameHiddenRecentlyReq() : header(MT_GAME_HIDDEN_RECENTLY_REQ) {}
		MessageHeader header;

		UniqueServerID	usid;								// �����������ĸ���
		int role_id;										// ������
		int req_type;										// ��������

		int count;
		int target_id[MAX_RECENTLY_CHAT_NUM];				// Ŀ��ID
										
	};

	enum HIDDEN_GAME_RECENTLY_INFO_TYPE
	{
		HIDDEN_GAME_RECENTLY_INFO_TYPE_INVALID = 0,			
		HIDDEN_GAME_RECENTLY_INFO_TYPE_ADD_CHAT,			// ������Ϣ���
		HIDDEN_GAME_RECENTLY_INFO_TYPE_GET_CHAT_INFO,		// ��ȡ������Ϣ
		HIDDEN_GAME_RECENTLY_INFO_TYPE_GET_TEAM_INFO,		// ��ȡ��������Ϣ
		HIDDEN_GAME_RECENTLY_INFO_TYPE_ADD_TEAM,			// �����ӱ��
	};

	UNSTD_STATIC_CHECK(MAX_RECENTLY_CHAT_NUM == MAX_RECENTLY_TEAM_NUM);

	struct HiddenGameRecentlyInfo									// ���ط���ԭ��  ͬ����ҵĽ��������Ϣ
	{
		HiddenGameRecentlyInfo() : header(MT_HIDDEN_GAME_RECENTLY_INFO) {}
		MessageHeader header;

		UniqueServerID	usid;										// ���ܸ���Ϣ����������ĸ���
		int role_id;												// ���ܸ���Ϣ�����ID
		int info_type;												// ��Ϣ����

		int count;													// ��
		FriendInfo			info_arr[MAX_RECENTLY_CHAT_NUM];		// ���������б���Ϣ ,���ĺ������Ӷ������
	};

	struct HiddenGameFriendChangeToRole								// ���ط���ԭ��  ͬ�����ѱ����Ϣ
	{
		HiddenGameFriendChangeToRole() : header(MT_HIDDEN_GAME_FRIEND_CHANGE_TO_ROLE) {}
		MessageHeader header;

		int role_id;												// ���ܸ���Ϣ�����ID
		int change_type;											// �������

		FriendInfo	friend_info;									// ������Ϣ
	};

	struct GameHiddenRoleLoginLogoutFriendNotify							// ԭ�������ط�  ͬ�������||����֪ͨ�����ط�
	{	
		GameHiddenRoleLoginLogoutFriendNotify() : header(MT_GAME_HIDDEN_ROLE_LOGIN_LOGOUT_FRIENDS_NOTIFY) {}
		MessageHeader header;

		int role_id;												// ���ID
		int online_status;											// �Ƿ��ǵ�¼
	};


	enum SYNC_FRIEND_TO_CROSS_STATUS
	{
		SYNC_FRIEND_TO_CROSS_STATUS_INVALID = 0,	// ��Ч
		SYNC_FRIEND_TO_CROSS_STATUS_INSERT = 1,		// ���		
		SYNC_FRIEND_TO_CROSS_STATUS_UPDATA = 2,		// ����		
		SYNC_FRIEND_TO_CROSS_STATUS_DELETE = 3,		// ɾ��		
		SYNC_FRIEND_TO_CROSS_STATUS_MAX,
	};

	struct SyncFriendInfo								// ��ԭ�������ط�ͬ����������
	{
		SyncFriendInfo() : header(MT_SYNC_FRIEND_INFO) {}
		MessageHeader header;

		struct FriendItemSyncInfo
		{
			FriendItemSyncInfo() : sync_type(0) {};
			unsigned int sync_type;						// ͬ��״̬ // �����ݲ���Ч , �����ݲ������
			FriendItem friend_item;
		};

		UniqueServerID from_usid;						// �����ĸ�ԭ�� [�����ò���]
		bool is_init;									// ��ʼ������ȫ����
		char is_init_add;								// 0 : ����  1 : ���
		short reserve_sh;

		int count;										// ��
		FriendItemSyncInfo item_list[MAX_ONCE_FRIENDS_COUNT];
	};


	struct SyncAddFriendOfflineEvent			// ͬ���������ʱ��Ҵ�������״̬ , ������ʱ��Ҫ����һ�³ɾ͵���صĲ���
	{
		SyncAddFriendOfflineEvent() : header(MT_SYNC_ADD_FRINED_OFFLINE_EVENT) {}
		MessageHeader header;

		int role_id;			// ������
		int req;				// 0:ɾ�� , 1:���	
	};

	// ������
	enum SYNC_BLACK_TO_CROSS_STATUS
	{
		SYNC_BLACK_TO_CROSS_STATUS_INVALID = 0,		// ��Ч
		SYNC_BLACK_TO_CROSS_STATUS_INSERT = 1,		// ���		
		SYNC_BLACK_TO_CROSS_STATUS_UPDATA = 2,		// ����		
		SYNC_BLACK_TO_CROSS_STATUS_DELETE = 3,		// ɾ��		
		SYNC_BLACK_TO_CROSS_STATUS_MAX,
	};
	struct SyncBlackInfo						// ��ԭ�������ط�ͬ����������Ϣ
	{
		SyncBlackInfo() : header(MT_SYNC_BLACK_INFO) {}
		MessageHeader header;

		struct BlackItemSyncInfo
		{
			BlackItemSyncInfo() : sync_type(0) {};
			unsigned int sync_type;						// ͬ��״̬ // �����ݲ���Ч , �����ݲ������
			BlackItem black_item;
		};

		UniqueServerID from_usid;						// �����ĸ�ԭ�� [�����ò���]
		bool is_init;									// ��ʼ������ȫ����
		char is_init_add;								// 0 : ����  1 : ���
		short reserve_sh;

		int count;										// ��
		BlackItemSyncInfo item_list[MAX_ONCE_BLACK_COUNT];
	};

	struct GameHiddenSyncGuildMemberChange
	{
		GameHiddenSyncGuildMemberChange() : header(MT_GAME_HIDDEN_SYNC_GUILD_MEMBER_CHANGE) {}
		MessageHeader header;

		enum OP_TYPE
		{
			OP_TYPE_ADD = 0,
			OP_TYPE_REMOVE = 1,
			OP_TYPE_UPDATE = 2,
			OP_TYPE_UPDATE_2 = 3,
		};

		int guild_id;
		int plat_type;
		int op_type;
		int param1;
		GuildMemberParam::Member member;
	};

	struct HiddenGameSyncGuildMemberChange
	{
		HiddenGameSyncGuildMemberChange() : header(MT_HIDDEN_GAME_SYNC_GUILD_MEMBER_CHANGE) {}
		MessageHeader header;

		enum OP_TYPE
		{			
			OP_TYPE_UPDATE = 2,
		};

		int guild_id;
		int plat_type;
		int op_type;
		int param1;
		GuildMemberParam::Member member;
	};

	struct GameHiddenSyncGuildChange : public IMessageSerializer, public IMessageDeserializer
	{
		GameHiddenSyncGuildChange() : header(MT_GAME_HIDDEN_SYNC_GUILD_CHANGE) {}
		MessageHeader header;

		virtual ~GameHiddenSyncGuildChange() {}
		virtual int GetMessageType() const { return header.msg_type; }

		enum OP_TYPE
		{
			OP_TYPE_ADD = 0,
			OP_TYPE_REMOVE = 1,
			OP_TYPE_UPDATE = 2,
		};

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(guild_id)
				&& WRITE_INT(plat_type)
				&& WRITE_INT(op_type);

			assert(result);
			if (!result) return false;

			if (OP_TYPE_REMOVE == op_type) return result;

			result = WRITE_STRN((const char*)&param.base_param, sizeof(param.base_param))
				&& WRITE_STRN((const char*)&param.gwt_param, sizeof(param.gwt_param))
				&& WRITE_STRN((const char*)&param.guild_boss_param, sizeof(param.guild_boss_param));

			assert(result);
			if (!result) return false;

			result = WRITE_INT(param.member_param.count);
			if (!result) return false;

			for (int i = 0; i < param.member_param.count && i < ARRAY_ITEM_COUNT(param.member_param.member_list); ++i)
			{
				result = WRITE_STRN((const char*)&param.member_param.member_list[i], sizeof(param.member_param.member_list[i]));
				assert(result);
				if (!result) return false;
			}

			assert(result);
			return result;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool result = true;

			result = READ_BEGIN(header.msg_type)
				&& READ_INT(guild_id)
				&& READ_INT(plat_type)
				&& READ_INT(op_type);

			if (!result) return false;

			if (OP_TYPE_REMOVE == op_type) return result;

			result = READ_STRN((char*)&param.base_param, sizeof(param.base_param), sizeof(param.base_param))
				&& READ_STRN((char*)&param.gwt_param, sizeof(param.gwt_param), sizeof(param.gwt_param))
				&& READ_STRN((char*)&param.guild_boss_param, sizeof(param.guild_boss_param), sizeof(param.guild_boss_param));

			if (!result) return false;

			result = READ_INT(param.member_param.count);
			if (!result) return false;

			for (int i = 0; i < param.member_param.count && i < ARRAY_ITEM_COUNT(param.member_param.member_list); ++i)
			{
				result = READ_STRN((char*)&param.member_param.member_list[i], 
					sizeof(param.member_param.member_list[i]), 
					sizeof(param.member_param.member_list[i]));
				if (!result) return false;
			}

			return result;
		}
		
		int guild_id;
		int plat_type;
		int op_type;

		AllGuildParam::GuildParam param;
	};

	struct HiddenGameGuildMemberAddTitle
	{
		HiddenGameGuildMemberAddTitle() : header(MT_HIDDEN_GAME_ADD_TITLE_TO_GUILD_MEMBER) {}
		MessageHeader header;

		int guild_id;
		int title_id;
		char reason[40];
	};

	//----------------------------------------------------------------
	struct GameHiddenReqSyncWorldStatus
	{
		GameHiddenReqSyncWorldStatus() : header(MT_GAME_HIDDEN_REQ_SYNC_WORLD_STATUS) {}
		MessageHeader	header;

		int server_id;
	};

	struct HiddenGameSyncWorldStatus
	{
		HiddenGameSyncWorldStatus() : header(MT_GAME_HIDDEN_SYNC_WORLD_STATUS) {}
		MessageHeader	header;

		ChiefElectionDataParam chief_election_data;
		ChiefPeakBattleDataParam chief_peak_battle_data;
		int world_wst_group_id;								//���������Ŀ��
		int world_wst_answer_subject;						//�����������ID
		int world_wst_answer;								//��������ID
		long long world_wst_answer_stamp;					//�����������ش�ʱ���
		long long world_wst_show_answer_stamp;				//�������չʾ��ʱ���
		long long world_wst_next_subject_refresh_stamp;		//���������һ����Ŀˢ��ʱ��
		long long answer_end_stamp;							//����������ʱ���
		long long next_answer_stamp;						//���������һ�λ����ʱ���
		unsigned int can_find_end_timestamp_list[MAX_DAY_SPECIAL_LOGIC_ACTIVITY_TYPE_NUM];		//������һؽ�������ʱ���
		WorldStatusGuildFightParam guild_fight_info;
		unsigned int open_cross_server_zero_timestamp;		//������������õĿ���0��ʱ���
	};


	struct HiddenGameActivityForceToNextState
	{
		HiddenGameActivityForceToNextState() : header(MT_GAME_HIDDEN_ACTIVITY_FORCE_TO_NEXT_STATE) {}
		MessageHeader	header;

		int activity_type;
		unsigned int next_status_switch_timestamp;
	};

	struct GameHiddenReqSyncActivityStatus
	{
		GameHiddenReqSyncActivityStatus() : header(MT_GAME_HIDDEN_REQ_SYNC_ACTIVITY_STATUS) {}
		MessageHeader	header;

		int server_id;
	};

	struct HiddenGameSyncActivityStatus
	{
		HiddenGameSyncActivityStatus() : header(MT_HIDDEN_GAME_SYNC_ACTIVITY_STATUS) {}
		MessageHeader	header;

		int activity_type;
		int activity_status;
		unsigned int next_status_switch_time;
	};

	//-------------------------------��ϯ----------------------------
	
	//��ѡ
	struct GameHiddenChiefElectionReq
	{
		GameHiddenChiefElectionReq() : header(MT_HIDDEN_GAME_CHIEF_ELECTION_REQ) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int origin_role_id;
		int base_prof;
		int req_type;
		int p1;
		int p2;
	};

	//�۷�ս
	struct GameHiddenChiefPeakBattleReq
	{
		GameHiddenChiefPeakBattleReq() : header(MT_HIDDEN_GAME_CHIEF_PEAK_BATTLE_REQ) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int origin_role_id;
		int req_type;
		int p1;
		int p2;
	};

	//-------------------------------������ҫս-------------------------
	struct GameHiddenGuildHonorBattleReqRoute
	{
		GameHiddenGuildHonorBattleReqRoute() : header(MT_HIDDEN_GAME_GUILD_HONOR_BATTLE_REQ_ROUTE) {}
		MessageHeader header;

		int role_id;
		int role_guild_id;
		int req_type;
		int p1;
		int p2;
	};

	struct HiddenGameGuildHonorBattleMatchRecordSyncInfo
	{
		HiddenGameGuildHonorBattleMatchRecordSyncInfo() : header(MT_HIDDEN_GAME_GUILD_HONOR_BATTLE_MATCH_RECORD_SYNC_INFO) {}
		MessageHeader header;

		int is_notice;
		CommonSaveGuildHonorRecordData match_record_data;
	};
	//------------------------------------------------------

	// ��һ�����Ϣ�����ı䣬ͬ�������
	struct GameHiddenUserCacheSync
	{
		GameHiddenUserCacheSync() : header(MT_GAME_HIDDEN_USERCACHE_CHANGE_SYNC) {}
		MessageHeader header;

		short is_name_changed;
		short is_new_node;
		int online_status;
		GameName old_name;
		UserCacheParam usercache_param;
	};


	// ��ҵ�¼״̬�����ı䣬ͬ�������
	struct GameHiddenOnlineStatusSync
	{
		GameHiddenOnlineStatusSync() : header(MT_GAME_HIDDEN_ONLINE_STATUS_SYNC) {}
		MessageHeader header;

		int role_id;
		int online_status;
	};

	struct HiddenGameOtherServerRoleOnlineStatusSync
	{
		HiddenGameOtherServerRoleOnlineStatusSync() : header(MT_HIDDEN_GAME_OTHER_SERVER_ROLE_ONLINE_STATUS_SYNC) {}
		MessageHeader header;

		int role_id;
		int online_status;
	};

	//---------------------------------����-------------------------
	enum HIDDEN_GAME_GUILD_CHANGE_TYPE
	{
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ADD_EXP,					//���Ӿ���
		HIDDEN_GAME_GUILD_CHANGE_TYPE_SET_GUILD_HONOR_TITLE,	//���ù�����ҫս�ƺ�
		HIDDEN_GAME_GUILD_CHANGE_TYPE_WEEK_TASK_EVENT,			//�������¼�
		HIDDEN_GAME_GUILD_CHANGE_TYPE_SET_GUILD_FIGHT_TITLE,	//���ü�����Ҷ��ƺ�
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ADD_GONGXIAN,				//���ӹ���
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_FINISH,		//��Ҽ����������		
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_SOLICIT,		//��Ҽ�����������
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_CLEAR_SOLICIT,	//��Ҽ�������ɾ������
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_GIVE,		//��Ҽ����������
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_FETCH,		//��Ҽ���������ȡ����
		HIDDEN_GAME_GUILD_CHANGE_TYPE_ROLE_GUILD_TASK_MAIL_NOTICE,		//��Ҽ�������֪ͨ
	};

	struct HiddenGameChangeGuildInfo
	{
		HiddenGameChangeGuildInfo() : header(MT_HIDDEN_GAME_CHANGE_GUILD_INFO) {}
		MessageHeader header;

		int guild_id;
		int change_type;
		int param;
		int param2;
		int param3;
		int param4;
	};

	//---------------------------------------------------------------
	// �����Ӫ����
	struct GameCrossRandActivityInfoSync
	{
		GameCrossRandActivityInfoSync() : header(MT_GAME_CROSS_RA_INFO_SYNC), count(0) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int is_all_sync;

		int count;
		CrossRandActivityDataParam param_list[RAND_ACTIVITY_TYPE_MAX];
	};

	// ��������Ҵ�����·�������������
	struct CrossGameRandActivityInfoSyncRet
	{
		CrossGameRandActivityInfoSyncRet() : header(MT_CROSS_GAME_RA_INFO_RET), count(0) {}
		MessageHeader header;

		int count;
		CrossRandActivityDataParam param_list[RAND_ACTIVITY_TYPE_MAX];
	};

	// �������ԭ����Ӫ�����󷢸����ط�
	struct CrossHiddenRandActivityInfoSync
	{
		CrossHiddenRandActivityInfoSync() : header(MT_CROSS_HIDDEN_RA_INFO_SYNC) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int is_all_sync;

		int count;
		CrossRandActivityDataParam param_list[RAND_ACTIVITY_TYPE_MAX];
	};

	//---------��������Ʒ��Ϣ-----------
	struct HiddenGameChatItemInfoReq
	{
		HiddenGameChatItemInfoReq() : header(MT_HIDDEN_GAME_CHAT_ITEM_INFO_REQ) {}
		MessageHeader header;

		int role_id;
		long long unique_item_seq;
	};

	struct GameHiddenChatItemInfoReq
	{
		GameHiddenChatItemInfoReq() : header(MT_GAME_HIDDEN_CHAT_ITEM_INFO_REQ) {}
		MessageHeader header;

		int role_id;
		long long unique_item_seq;
	};

	class GameHiddenChatItemSyn : public IMessageSerializer
	{
	public:
		GameHiddenChatItemSyn() : header(MT_GAME_HIDDEN_CHAT_ITEM_INFO_SYN) {}
		virtual ~GameHiddenChatItemSyn() {}
		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool result = true;

			result = result && WRITE_BEGIN(header.msg_type);
			result = result && WRITE_LL(item_info_seq);
			result = result && WRITE_USHORT(item_id);
			result = result && WRITE_SHORT(num);
			result = result && WRITE_CHAR(is_bind);
			result = result && WRITE_CHAR(has_param);
			result = result && WRITE_SHORT(param_length);
			result = result && WRITE_UINT(invalid_time);
			if (0 != has_param)
			{
				result = result && WRITE_STRN(param_data, param_length);
			}

			result = result && WRITE_INT(has_attrs);
			if (has_attrs != 0)
			{
				for (int i = 0; i < ARRAY_LENGTH(attrs); ++i)
				{
					int value = attrs[i];
					result = result && WRITE_INT(value);
				}
			}

			return result;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_LL(item_info_seq)
				&& READ_USHORT(item_id)
				&& READ_SHORT(num)
				&& READ_CHAR(is_bind)
				&& READ_CHAR(has_param)
				&& READ_SHORT(param_length)
				&& READ_UINT(invalid_time);

			if (0 != has_param)
			{
				ret = READ_STRN(param_data, param_length, sizeof(NetValueItemParamData));
			}

			if (!ret) return false;

			ret = READ_INT(has_attrs);
			if (has_attrs != 0)
			{
				for (int i = 0; i < BATTLE_ATTR_MAX; ++i)
				{
					int &value = attrs[i];
					ret = READ_INT(value);
				}
			}

			return ret;
		}

		MessageHeader		header;
		long long	item_info_seq;				// 
		ItemID			item_id;		//!< ��Ʒid
		short				num;			//!< ����
		char				is_bind;		//!< �Ƿ��
		char				has_param;		//!< �Ƿ��о�ֵ����
		short				param_length;	//!< ��ֵ���������ݳ��� ����has_paramΪ1ʱ�ɶ���
		UInt32				invalid_time;	//!< ����ʱ��
		NetValueItemParamData param_data;	//!< ��ֵ���������ݣ�����param_length��ֵ����ȡ ����has_paramΪ1ʱ�ɶ���

		int has_attrs;						//�����Ϊ0��Ҫ�������Ǹ�����
		int attrs[BATTLE_ATTR_MAX];
	};

	struct GameHiddenForceGetGold
	{
		GameHiddenForceGetGold() : header(MT_GAME_HIDDEN_FORCE_GET_GOLD) {}
		MessageHeader header;

		int role_id;
		int server_id;		// ��־��
	};

	struct GameHiddenAddCredit
	{
		GameHiddenAddCredit() : header(MT_GAME_HIDDEN_TRADE_CREDIT_ADD_CREDIT_EVENTHANDLER) {}
		MessageHeader header;

		int role_id;
		int credit_num;
	};

	enum SYNC_RANK_TO_CROSS_STATUS				
	{
		SYNC_RANK_TO_CROSS_STATUS_INVALID = 0,	// ��Ч
		SYNC_RANK_TO_CROSS_STATUS_INSERT = 1,	// ���		index ��Ч, rank_infoΪ������
		SYNC_RANK_TO_CROSS_STATUS_UPDATA = 2,	// ����		index ��index , rank_infoΪ������
		SYNC_RANK_TO_CROSS_STATUS_DELETE = 3,	// ɾ��		index ��index , rank_info ��Ч
		SYNC_RANK_TO_CROSS_STATUS_MAX,
	};

	struct GameCrossSyncCrossPersonRankInfo		//�ɱ������͸������а�ı���Ϣ�����
	{
		GameCrossSyncCrossPersonRankInfo() : header(MT_GAME_HIDDEN_SYNC_CROSS_PERSON_RANK_INFO) {}
		MessageHeader	header;

		struct PersonRankSyncInfo
		{
			PersonRankSyncInfo() : sync_type(0) , index(-1) {}; 
			unsigned int sync_type;						// ͬ��״̬ // �������ݲ���Ч , �������ݲ������
			int index;									// ������ , ����ʱ��Ч, ֻ��ɾ���͸�����Ч
			PersonRankItem rank_info;
		};

		UniqueServerID from_usid;
		bool is_init;									// ��ʼ������ȫ����
		char is_init_add;								// 0 : ����  1 : ���
		short reserve_sh;
		unsigned int last_change_time;
		int rank_type;
		int count;

		PersonRankSyncInfo item_list[CROSS_PERSON_RANK_SYNC_PAGE_ELEMENT_COUNT];
	};

	struct GameCrossSyncCrossPetRankInfo		//�ɱ������͸������а�ı���Ϣ�����
	{
		GameCrossSyncCrossPetRankInfo() : header(MT_GAME_HIDDEN_SYNC_CROSS_PET_RANK_INFO) {}
		MessageHeader	header;

		struct PetRankSyncInfo
		{
			PetRankSyncInfo() : sync_type(0), index(-1) {};

			unsigned int sync_type;						// ͬ��״̬ // �������ݲ���Ч , �������ݲ������
			int index;									// ������ , ����ʱ��Ч, ֻ��ɾ���͸�����Ч
			PetRankItem rank_info;
		};

		UniqueServerID from_usid;
		bool is_init;									// ��ʼ������ȫ����
		char is_init_add;								// 0 : ����  1 : ���
		short reserve_sh;
		unsigned int last_change_time;
		int rank_type;
		int count;

		PetRankSyncInfo item_list[CROSS_PET_RANK_SYNC_PAGE_ELEMENT_COUNT];
	};

	struct CrossGameUserMsg
	{
		CrossGameUserMsg() : header(MT_CROSS_GAME_USER_MSG) {}
		MessageHeader		header;

		int					uid;
		// ������� Protocol Э������
	};

	struct GameCrossUserMsg
	{
		GameCrossUserMsg() : header(MT_GAME_CROSS_USER_MSG) {}
		MessageHeader		header;

		long long			uuid;
		int					merge_server_id;
		// ������� Protocol Э������
	};

	// -----------------��һ�а�(���˰������Ҷ�)--------------
	struct HiddenGameRoleAddTitle
	{
		HiddenGameRoleAddTitle() : header(MT_HIDDEN_GAME_ADD_TITLE_TO_ROLE) {}
		MessageHeader header;

		int uid;
		int title_id;
		char reason[64];
	};

	// -------------------���֪ͨ��Ϸ�����,���ط���δ����-------------
	struct CrossGameHiddenNotConnected
	{
		CrossGameHiddenNotConnected() : header(MT_CROSS_GAME_HIDDEN_NOT_CONNECTED) {}
		MessageHeader header;

		int uid;
	};

	struct GameCrossSyncCrossQuery		//�ɱ����������������Ϣ�����
	{
		GameCrossSyncCrossQuery() : header(MT_GAME_HIDDEN_QUERY_REQ) {}
		MessageHeader	header;

		int target_uid;
		UniqueUserID viewer_uid;
	};

	struct GameCrossSendAllUser		//���͸������Ͽ����������Ϸ��(�������ط�)�е��������
	{
		GameCrossSendAllUser() : header(MT_GAME_CROSS_SEND_ALL_USER) {}
		MessageHeader header;

		int origin_plat_type;
		int origin_server_id;
		int send_to_uuid;
		//���������ProtocolЭ������
	};

	struct GameHiddenSendChatGroup		//��Ϸ��������������Ϣ�����
	{
		GameHiddenSendChatGroup() : header(MT_GAME_HIDDEN_CHAT_GROUP) {}
		MessageHeader header;

		long long group_id;
		int main_server_id;
		//���������ProtocolЭ������
	};

	struct HiddenGameSendChatGroup		//���������������Ϣ����Ϸ��
	{
		HiddenGameSendChatGroup() : header(MT_HIDDEN_GAME_CHAT_GROUP) {}
		MessageHeader header;

		long long group_id;
		int main_server_id;
		//���������ProtocolЭ������
	};

	struct HiddenGameNoticeLog			//���ط�֪ͨ���ط���log(�����log,���ں�̨ͳ�Ʋ�������)
	{
		HiddenGameNoticeLog() : header(MT_HIDDEN_GAME_NOTICE_LOG) {}
		MessageHeader header;

		int uid;
		int activity_type;
		int param1;
		GameName role_name;
		PlatName plat_name;
		char reason_str[64];
	};

	enum CHAT_OPRETA_TYPE
	{
		CHAT_OPRETA_TYPE_DISSMISS_GROUP = 1,				// ��ɢ������
		CHAT_OPRETA_TYPE_DISSMISS_GROUP_RET,				// ��ɢ����

		CHAT_OPRETA_TYPE_TICKOUT_MEMBER,					// �߳�������
		CHAT_OPRETA_TYPE_TICKOUT_MEMBER_RET,				// �߳������鷵��
		CHAT_OPRETA_TYPE_INVITE_MEMBER,						// �������
		CHAT_OPRETA_TYPE_ENTER_GROUP,						// ����������
		CHAT_OPRETA_TYPE_ENTER_GROUP_RET,					// ���������鷵��
		CHAT_OPRETA_TYPE_LOGIN_GROUP,						// �������
		CHAT_OPRETA_TYPE_LOGOUT_GROUP,						// �������
		CHAT_OPRETA_TYPE_CHANGE_NAME,						// ��Ҹ���
		CHAT_OPRETA_TYPE_CHANGE_NAME_RET,					// ��Ҹ�������
		CHAT_OPRETA_TYPE_CHANGE_GROUP_NAME_OR_NOTICE,		// Ⱥ�����������
		CHAT_OPRETA_TYPE_CHANGE_PROF,						// ����޸�ְҵ
		CHAT_OPRETA_TYPE_CHANGE_AVA,						// ����޸�ͷ��

		CHAT_OPRETA_TYPE_LEAVEN_GROUP,						// ����˳�Ⱥ��
		CHAT_OPRETA_TYPE_LEAVEN_GROUP_RET,					// ����˳�Ⱥ�ķ���

		CHAT_OPRETA_TYPE_CREATE_GROUP,						// ��Ҵ������Ⱥ��
		CHAT_OPRETA_TYPE_CREATE_GROUP_RET,					// ��Ҵ������Ⱥ�鷵��
		CHAT_OPERATE_TYPE_LEADER_CHANGE_SYNC_GAME,			// Ⱥ���ı�ͬ����ԭ��
	};

	enum CHAT_OPRETA_RET_TYPE
	{
		CHAT_OPRETA_RET_TYPE_SUCC = 0,						// �ɹ�����
		CHAT_OPRETA_RET_TYPE_GROUP_INVALID = -1,			// Ⱥ�鲻����
		CHAT_OPRETA_RET_TYPE_GROUP_FULL = -2,				// Ⱥ������
		CHAT_OPRETA_RET_TYPE_ALREADY_EXISTS = -3,			// �Ѿ���Ⱥ����
		CHAT_OPRETA_RET_TYPE_ADD_FAIL = -4,					// ����ʧ��
	};

	struct GameCrossSyncChatOpreta		//�ɱ���������������������ط�
	{
		GameCrossSyncChatOpreta() : header(MT_GAME_HIDDEN_CHAT_REQ) {}
		MessageHeader	header;

		int opreta_type;
		int uid;
		long long group_id;
		int is_cross;
		int inviter_id;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
		GameName group_master;
		GameName group_name;
		GuildNotice group_notice;
	};

	struct CrossGameSyncChatOpreta		//�����ط��������������������
	{
		CrossGameSyncChatOpreta() : header(MT_HIDDEN_GAME_CHAT_REQ) {}
		MessageHeader	header;

		int opreta_type;
		int uid;
		long long group_id;
		int is_cross;
		int inviter_id;
		int param1;
		int param2;
		int param3;
		int param4;
		int param5;
		GameName group_master;
		GameName group_name;
		GuildNotice group_notice;
	};

	struct GameHiddenTempGroupBecomeOfficialReq
	{
		GameHiddenTempGroupBecomeOfficialReq() : header(MT_GAME_HIDDEN_TEMP_GROUP_BECOME_OFFICIAL_REQ) {}
		MessageHeader header;

		struct MemberInfo
		{
			int uid;
			int avatar_type;
			int profession;
			int level;
			GameName name;
		};

		int plat_type;				//!< ���ڷ���
		int server_id;				//!< ���ڷ���
		GameName group_name;
		int member_num;
		MemberInfo member_list[5];	//!< ��ͷ����Ⱥ��
	};

	struct HiddenGameTempGroupBecomeOfficialResp
	{
		HiddenGameTempGroupBecomeOfficialResp() : header(MT_HIDDEN_GAME_TEMP_GROUP_BECOME_OFFICIAL_RESP) {}
		MessageHeader header;

		int result;
		long long group_id;
		int member_uid_list[5]; // ��ͷ����Ⱥ��
		GameName group_name;
		GameName creator_name;
	};

	struct GameCrossSyncRATianMing		//�ɱ���������������������ط�
	{
		GameCrossSyncRATianMing() : header(MT_GAME_HIDDEN_RA_TIANMING) {}
		MessageHeader	header;

		UniqueServerID info_from_server_id;
		TianMingDivinationInfo info;
	};

	struct CrossGameSyncRATianMing		//�����ط��������������������
	{
		CrossGameSyncRATianMing() : header(MT_HIDDEN_GAME_RA_TIANMING) {}
		MessageHeader	header;

		UniqueServerID info_from_server_id;
		TianMingDivinationInfo info;
	};

	//�Թ�����
	enum GAME_HIDDEN_MAZE_RACER_REQ_TYPE
	{
		GAME_HIDDEN_MAZE_RACER_REQ_TYPE_RANK,  //���а�
	};


	struct GameHiddenMazerAcerReq
	{
		GameHiddenMazerAcerReq() : header(MT_HIDDEN_GAME_MAZER_ACER_REQ) {}
		MessageHeader header;


		int origin_plat_type;
		int origin_server_id;
		int origin_role_id;
		int req_type;
	};

	//��Ϸ��ĳЩ��Ϣ֪ͨ���ط�
	enum GAME_HIDDEN_COMMON_INFO_NOTICE_TYPE
	{
		GAME_HIDDEN_COMMON_INFO_NOTICE_TYPE_ROLE_LOGIN,
		GAME_HIDDEN_COMMON_INFO_NOTICE_TYPE_ROLE_LOGOUT,
	};

	struct GameHiddenCommonInfoNoticeHidden
	{
		GameHiddenCommonInfoNoticeHidden() : header(MT_HIDDEN_GAME_COMMON_INFO_NOTICE_HIDDEN) {}
		MessageHeader header;

		int server_id;
		int is_all_sync;
		int type;
		int param1;
	};

	//���ط�����Ϸ��������ɱ��Ϣ
	struct HiddenGameFirstKillInfoReq
	{
		HiddenGameFirstKillInfoReq() : header(MT_HIDDEN_GAME_FIRST_KILL_INFO_REQ) {}
		MessageHeader header;

		long long  uuid;
		int type;
		int level_group;
	};

	//��Ϸ�������ط�������ɱ��Ϣ
	struct GameHiddenFirstKillInfoReq
	{
		GameHiddenFirstKillInfoReq() : header(MT_GAME_HIDDEN_FIRST_KILL_INFO_REQ) {}
		MessageHeader header;

		int uid;
		int type;
		int level_group;
	};

	// �����ԭ������򵥵�ս����Ϣ
	struct HiddenGameVideoSimpleInfoReq
	{
		HiddenGameVideoSimpleInfoReq() : header(MT_HIDDEN_GAME_VIDEO_SIMPLE_INFO_REQ) {}
		MessageHeader header;

		long long uuid;

		int monster_group_id;		//!< ������ID
		int param1;					//!< ԭ������ 
		int param2;					//!< ԭ������
		int param3;					//!< ԭ������
		int is_server_first_kill;	//!< �Ƿ���ɱ
	};

	// ԭ�������ط�����򵥵�ս����Ϣ
	struct GameHiddenVideoSimpleInfoReq
	{
		GameHiddenVideoSimpleInfoReq() : header(MT_GAME_HIDDEN_VIDEO_SIMPLE_INFO_REQ) {}
		MessageHeader header;

		int uid;

		int monster_group_id;		//!< ������ID
		int param1;					//!< ԭ������ 
		int param2;					//!< ԭ������
		int param3;					//!< ԭ������
		int is_server_first_kill;	//!< �Ƿ���ɱ
	};

	enum VIDEO_DETAIL_INFO_REQ_TYPE
	{
		VIDEO_DETAIL_INFO_REQ_TYPE_WATCH = 0,		//�ۿ�¼��   ��Ҫuuid
		VIDEO_DETAIL_INFO_REQ_TYPE_SYN = 1,			//ͬ������¼�񵽿��
	};

	// �����ԭ��������ϸ��ս����Ϣ
	struct HiddenGameVideoDetailInfoReq
	{
		HiddenGameVideoDetailInfoReq() : header(MT_HIDDEN_GAME_VIDEO_DETAIL_INFO_REQ){}
		MessageHeader header;

		int type;
		long long uuid;

		int monster_group_id;
		int is_server_first_kill;
	};

	struct GameHiddenVideoDetailInfoResp : public IMessageSerializer, public IMessageDeserializer
	{
		GameHiddenVideoDetailInfoResp() : header(MT_GAME_HIDDEN_VIDEO_DETAIL_INFO_RESP),
			uuid(0), video_length(0), cur_buffer_len(0)
		{
			memset(video_buffer, 0, sizeof(video_buffer));
			info_param.Reset();
		}

		virtual ~GameHiddenVideoDetailInfoResp()
		{
			cur_buffer_len = 0;
		}

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(type)
				&& WRITE_LL(uuid)
				&& WRITE_INT(video_length)
				&& WRITE_SHORT(info_param.is_server_first_skill)
				&& WRITE_SHORT(info_param.version)
				&& WRITE_SHORT(info_param.battle_mode)
				&& WRITE_INT(info_param.monster_group_id)
				&& WRITE_INT(info_param.average_score)
				&& WRITE_UINT(info_param.record_timestamp)
				&& WRITE_LL(info_param.video_file_id);

			if (!ret) return false;

			ret = WRITE_STRN(video_buffer, video_length);
			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(type)
				&& READ_LL(uuid)
				&& READ_INT(video_length)
				&& READ_SHORT(info_param.is_server_first_skill)
				&& READ_SHORT(info_param.version)
				&& READ_SHORT(info_param.battle_mode)
				&& READ_INT(info_param.monster_group_id)
				&& READ_INT(info_param.average_score)
				&& READ_UINT(info_param.record_timestamp)
				&& READ_LL(info_param.video_file_id);

			if (!ret) return false;

			ret = READ_STRN(video_buffer, video_length, MAX_VIDEO_BUFFER);

			return ret;
		}

		static const int MAX_VIDEO_BUFFER = 200000;

		MessageHeader header;

		int type;
		long long uuid;
		int video_length;
		char video_buffer[MAX_VIDEO_BUFFER];
		int cur_buffer_len;	// ��ǰ��buffer���ȣ�������¼

		BattleVideoParam info_param;
	};

	// ԭ�������ط�������ϸ��ս����Ϣ
	struct GameHiddenVideoDetailInfoReq
	{
		GameHiddenVideoDetailInfoReq() : header(MT_GAME_HIDDEN_VIDEO_DETAIL_INFO_REQ) {}
		MessageHeader header;

		int uid;

		int monster_group_id;
		int is_server_first_kill;
	};

	// ���ط���ԭ��������ϸ��ս����Ϣ
	struct HiddenGameVideoDetailInfoResp : public IMessageSerializer, public IMessageDeserializer
	{
		HiddenGameVideoDetailInfoResp() : header(MT_HIDDEN_GAME_VIDEO_DETAIL_INFO_RESP),
			uid(0), video_length(0), cur_buffer_len(0), monster_group_id(0), is_server_first_kill(0)
		{
			memset(video_buffer, 0, sizeof(video_buffer));
		}

		virtual ~HiddenGameVideoDetailInfoResp()
		{
			cur_buffer_len = 0;
		}

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(uid)
				&& WRITE_INT(video_length)
				&& WRITE_INT(monster_group_id)
				&& WRITE_INT(is_server_first_kill);

			if (!ret) return false;

			ret = WRITE_STRN(video_buffer, video_length);
			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(uid)
				&& READ_INT(video_length)
				&& READ_INT(monster_group_id)
				&& READ_INT(is_server_first_kill);

			if (!ret) return false;

			ret = READ_STRN(video_buffer, video_length, MAX_VIDEO_BUFFER);

			return ret;
		}

		static const int MAX_VIDEO_BUFFER = 200000;

		MessageHeader header;

		int uid;
		int video_length;
		char video_buffer[MAX_VIDEO_BUFFER];
		int cur_buffer_len;	// ��ǰ��buffer���ȣ�������¼

		int monster_group_id;
		int is_server_first_kill;
	};

	//�ý�ս��
	struct GameHiddenHuanJieZhanChangeReq
	{
		GameHiddenHuanJieZhanChangeReq() : header(MT_HIDDEN_GAME_HUAN_JIE_ZHAN_CHANG_REQ) {}
		MessageHeader header;

		int origin_role_id;
		int req_type;
		int p1;
		int p2;
	};

	struct GameHiddenHuanJieZhanChangeReq2
	{
		GameHiddenHuanJieZhanChangeReq2() : header(MT_HIDDEN_GAME_HUAN_JIE_ZHAN_CHANG_REQ_2) {}
		MessageHeader header;
		
		int origin_role_id;			//��������
		int count;
		int role_id_list[100];			//Ҫ��ȡ���������Ϣ��role_id�б�
	};

	struct HiddenGameGuildEventhandlerSend
	{
		HiddenGameGuildEventhandlerSend() : header(MT_HIDDEN_GAME_GUILD_EVENTHANDLER_SEND) {}
		MessageHeader header;

		GuildID guild_id;
		GuildEventhandlerParam eventhandler_node;
	};

	struct GameCrossSyncCrossChatGroupInfo		//�ɱ�����������Ⱥ��ı���Ϣ�����
	{
		GameCrossSyncCrossChatGroupInfo() : header(MT_GAME_HIDDEN_SERVER_LOGIN_CHATGROUP_DATA_SYN) {}
		MessageHeader	header;
		const static int MAX_ONE_SEND_LIST = ONCE_SEND_CHAT_GROUP_COUNT / 5;
		UniqueServerID from_usid;
		int count;

		ChatGroupInfo chat_group_list[MAX_ONE_SEND_LIST];
	};

	UNSTD_STATIC_CHECK(sizeof(GameCrossSyncCrossChatGroupInfo) <= 100000);//һ��Э����󲻳���10W

	struct CrossGameSyncCrossChatGroupInfo		//�ɿ����������Ⱥ��ı���Ϣ������
	{
		CrossGameSyncCrossChatGroupInfo() : header(MT_HIDDEN_GAME_SERVER_LOGIN_CHATGROUP_DATA_SYN) {}
		MessageHeader	header;
		const static int MAX_ONE_SEND_LIST = ONCE_SEND_CHAT_GROUP_COUNT / 5;
		UniqueServerID from_usid;
		int count;

		ChatGroupInfo chat_group_list[MAX_ONE_SEND_LIST];
	};

	UNSTD_STATIC_CHECK(sizeof(CrossGameSyncCrossChatGroupInfo) <= 100000);//һ��Э����󲻳���10W

	struct HiddenGameSyncCrossChatGroupInfo		//�ɿ����������Ⱥ����Ϣ-����
	{
		HiddenGameSyncCrossChatGroupInfo() : header(MT_HIDDEN_GAME_CHATGROUP_INFO_SYN) {}
		MessageHeader	header;

		int user_id;
		ChatGroupInfo chat_group_list;
	};

	struct HiddenGameSynOnlyFightInfo
	{
		HiddenGameSynOnlyFightInfo() : header(MT_HIDDEN_GAME_ONLY_FIGHT_SYN_INFO) {}
		MessageHeader header;
		
		OnlyFightDataParam param;
	};

	struct GameHiddenSynOnlyFightInfoReq
	{
		GameHiddenSynOnlyFightInfoReq() : header(MT_GAME_HIDDEN_ONLY_FIGHT_SYN_INFO_REQ) {}
		MessageHeader header;

		int server_id;
	};


	//----------------------��̨ս-----------------------------
	enum GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE
	{
		GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_ROLE_INFO,
		GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_REMOVE,
		GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_RQE_ROUTE_HIDDEN,
		GAME_HIDDEN_PLATFORM_BATTLE_REQ_TYPE_PLATFORM_INFO,
	};

	struct GameHiddenPlatformBattleReq  
	{
		GameHiddenPlatformBattleReq() : header(MT_GAME_HIDDEN_PLATFORM_BATTLE_REQ) {}
		MessageHeader	header;

		int type;
		int param1;
		int param2;
		short param3;
		short param4;
	};
	//---------------------------------------------------


	//----------------------------------��ħ֮������-----------------------------------------
	struct HiddenGameCourageChallengeRankReq				//������󱾷�������Ϣ
	{
		HiddenGameCourageChallengeRankReq() : header(MT_HIDDEN_GAME_COURAGE_CHALLENGE_RANK_REQ) {}
		MessageHeader header;

		long long m_uuid;
		short rank_type;
		short count;
		int role_uid_list[MAX_COURAGE_CHALLENGE_RANK_NUM];	//���������������Ҫ
	};

	struct HiddenGameCourageChallengeRankRoleChange			//���������й���ħ֮��������Ϣ�ı�,����ԭ��
	{
		HiddenGameCourageChallengeRankRoleChange() : header(MT_HIDDEN_GAME_COURAGE_CHALLENGE_ROLE_CHANGE) {}
		MessageHeader header;

		SynCourageChallengeRankInfo role_info;
	};

	static const int SYNC_LOG_BUFFER_SIZE = 4096;
	struct HiddenGameSyncLogToOriginServer
	{
		HiddenGameSyncLogToOriginServer() : header(MT_HIDDEN_GAME_SYNC_LOG_TO_ORIGIN_SERVER) {}
		MessageHeader header;

		int log_type;
		int length;
		char buffer[SYNC_LOG_BUFFER_SIZE];
	};
	
	struct HiddenGameBossChapterRankReq			// ���ط��򱾷��������������������
	{
		HiddenGameBossChapterRankReq() : header(MT_HIDDEN_GAME_GUILD_BOSS_CHAPTER_RANK_REQ) {}
		MessageHeader header;

		long long uuid;
	};

	struct GameHiddenSyncPublicNotice : public IMessageSerializer, public IMessageDeserializer		// ԭ�������ط�
	{
		GameHiddenSyncPublicNotice() : header(MT_GAME_HIDDEN_PUBLIC_NOTICE_SYNC) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type) &&
				WRITE_INT(plat_type) &&
				WRITE_INT(server_id) &&
				WRITE_INT(count);
			if (!ret)
			{
				return false;
			}

			for (int i = 0; i < count && i < PublicNoticeList::MAX_PUBLICNOTICE_NUM; ++i)
			{
				ret = WRITE_INT(notice_list[i].notice_id) &&
					WRITE_INT(notice_list[i].type) &&
					WRITE_INT(notice_list[i].color) &&
					WRITE_INT(notice_list[i].urgent) &&
					WRITE_LL(notice_list[i].begin_time) &&
					WRITE_LL(notice_list[i].end_time) &&
					WRITE_LL(notice_list[i].send_interval) &&
					WRITE_INT(notice_list[i].creator.length()) &&
					WRITE_INT(notice_list[i].notice.length());

				if (!ret) return false;

				ret = WRITE_STRN(notice_list[i].creator.c_str(), notice_list[i].creator.length());
				if (!ret) return false;

				ret = WRITE_STRN(notice_list[i].notice.c_str(), notice_list[i].notice.length());
				if (!ret) return false;
			}

			return true;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type) &&
				READ_INT(plat_type) &&
				READ_INT(server_id) &&
				READ_INT(count);
			if (!ret)
			{
				return false;
			}

			for (int i = 0; i < count && i < PublicNoticeList::MAX_PUBLICNOTICE_NUM; ++i)
			{
				int creator_len = 0;
				int notice_len = 0;
				ret = READ_INT(notice_list[i].notice_id) &&
					READ_INT(notice_list[i].type) &&
					READ_INT(notice_list[i].color) &&
					READ_INT(notice_list[i].urgent) &&
					READ_LL(notice_list[i].begin_time) &&
					READ_LL(notice_list[i].end_time) &&
					READ_LL(notice_list[i].send_interval) &&
					READ_INT(creator_len) &&
					READ_INT(notice_len);

				if (!ret) return false;

				ret = READ_STRING(notice_list[i].creator, creator_len);
				if (!ret) return false;

				ret = READ_STRING(notice_list[i].notice, notice_len);
				if (!ret) return false;
			}

			return true;
		}

		int plat_type;
		int server_id;

		int count;
		PublicNoticeList::NoticeAttr notice_list[PublicNoticeList::MAX_PUBLICNOTICE_NUM];
	};

	struct GameHiddenSyncOnlineInfo
	{
		GameHiddenSyncOnlineInfo() : header(MT_GAME_HIDDEN_ONLINE_INFO_SYNC) {}
		MessageHeader header;

		int plat_type;
		int server_id;

		int online_role_num;
		int incross_role_num;
		int crossing_role_num;
	};

	struct HiddenGameInviteJoinTeam
	{
		HiddenGameInviteJoinTeam() : header(MT_HIDDEN_GAME_INVITE_JOIN_TEAM) {}
		MessageHeader header;

		int cross_team_index;		
		int team_type;
		short team_level_limit_low;
		short team_level_limit_high;
		int team_member_num;
		MsgRoleBaseInfo inviter_info;
		TeamItem team_item;

		int target_uid;		
		int invite_type;
	};

	struct GameHiddenInviteJoinTeamInOrigin
	{
		GameHiddenInviteJoinTeamInOrigin() : header(MT_GAME_HIDDEN_INVITE_JOIN_TEAM_IN_ORIGIN) {}
		MessageHeader header;

		int team_index;
		int team_type;
		short team_level_limit_low;
		short team_level_limit_high;
		int team_member_num;
		MsgRoleBaseInfo inviter_info;
		TeamItem team_item;

		int target_uid;
		int invite_type;
	};

	struct GameHiddenReqJoinTeamInfo
	{
		GameHiddenReqJoinTeamInfo() : header(MT_GAME_HIDDEN_REQ_JOIN_TEAM_INFO) {}
		MessageHeader header;

		int cross_team_index;
		int inviter_uid;
		int target_uid;		
	};

	struct HiddenGameJoinTeamInfoRet
	{
		HiddenGameJoinTeamInfoRet() : header(MT_HIDDEN_GAME_JOIN_TEAM_INFO_RET) {}
		MessageHeader header;

		int is_exist;
		int cross_team_index;
		int team_type;
		int team_member_num;
		MsgRoleBaseInfo inviter_info;
		TeamItem team_item;
		int target_uid;
	};

	struct GameHiddenInvationJoinTeamRet
	{
		GameHiddenInvationJoinTeamRet() : header(MT_GAME_HIDDEN_INVITE_JOIN_TEAM_RET) {}
		MessageHeader header;
		
		int result;
		int notice_num;
		int inviter_uid;
	};

	struct HiddenGameInviteJoinTeamInOriginRet
	{
		HiddenGameInviteJoinTeamInOriginRet() : header(MT_HIDDEN_GAME_INVITE_JOIN_TEAM_IN_ORIGIN_RET) {}
		MessageHeader header;

		int result;
		int notice_num;
		int inviter_uid;
	};

	struct HiddenGameGuildGatherDreamPublishListReq
	{
		HiddenGameGuildGatherDreamPublishListReq() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_PUBLISH_LIST_REQ) {}
		MessageHeader header;

		int uid;
		int guild_id;
	};

	struct HiddenGameGuildGatherDreamRecordListReq
	{
		HiddenGameGuildGatherDreamRecordListReq() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_RECORD_LIST_REQ) {}
		MessageHeader header;

		int uid;
		int type;
	};

	struct HiddenGameGuildGatherDreamPublishReq
	{
		HiddenGameGuildGatherDreamPublishReq() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_PUBLISH_REQ) {}
		MessageHeader header;

		int uid;
		int guild_id;
		int is_publish_send;		//0:������ 1:����
		GuildGatherDreamInfo info;
	};

	struct GameHiddenGuildGatherDreamPublishRet
	{
		GameHiddenGuildGatherDreamPublishRet() : header(MT_GAME_HIDDEN_GUILD_GATHER_DREAM_PUBLISH_RET) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_TYPE_SUCC = 0,			//�����ɹ�
			RESULT_TYPE_CONFIG_ERROR,		//��ȡ����ʧ��
			RESULT_TYPE_NOT_PRAY,			//�ó��ﲻ�ܼ�Ը
		};

		int is_publish_send;		//0:������ 1:����
		int result;		
		int guild_id;
		int uid;
		int item_id;				//�����ɹ�ʱʹ��			
	};
	
	struct HiddenGameGuildGatherDreamGiftReq
	{
		HiddenGameGuildGatherDreamGiftReq() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_GIFT_REQ) {}
		MessageHeader header;

		int gift_role_uid;
		int get_role_uid;			//������ҵ�uid
		int guild_id;
	};

	struct GameHiddenGuildGatherDreamGiftRet
	{
		GameHiddenGuildGatherDreamGiftRet() : header(MT_GAME_HIDDEN_GUILD_GATHER_DREAM_GIFT_RET) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_TYPE_SUCC = 0,			//�ж�˳��ͨ��
			RESULT_TYPE_NOT_PUBLISH,		//���û�з���
			RESULT_TYPE_IS_FINISH,			//����Ѽ���
			RESULT_TYPE_CONFIG_ERROR,		//��ȡ����ʧ��
			RESULT_TYPE_ERR_DIF_GUILD,		//����ͬһ����
		};

		int result;
		SynGuildGatherDreamGiftInfo info;
	};

	struct HiddenGameGuildGatherDreamGiftAddRecord
	{
		HiddenGameGuildGatherDreamGiftAddRecord() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_GIFT_ADD_RECORD) {}
		MessageHeader header;

		SynGuildGatherDreamGiftInfo info;
	};
	
	struct HiddenGameGuildGatherDreamFetchReq
	{
		HiddenGameGuildGatherDreamFetchReq() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_FETCH_REQ) {}
		MessageHeader header;

		int uid;
		int guild_id;
	};

	struct GameHiddenGuildGatherDreamFetchRet
	{
		GameHiddenGuildGatherDreamFetchRet() : header(MT_GAME_HIDDEN_GUILD_GATHER_DREAM_FETCH_RET) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_TYPE_SUCC = 0,				// ˳��ͨ���ж�
			RESULT_TYPE_NOT_PUBLISH,			// ���û�з���
			RESULT_TYPE_NOT_HAS_FETCH,			// û�ж������
		};

		int result;
		int uid;
		int index;
		int guild_id;
		ItemID item_id;
		short fetch_count;
	};

	struct HiddenGameGuildGatherDreamFetchChange
	{
		HiddenGameGuildGatherDreamFetchChange() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_FETCH_CHANGE) {}
		MessageHeader header;

		int uid;
		int index;
		int guild_id;
		short fetch_count;
		short is_succ;
	};

	//�´��ع�
	struct GameHiddenHBraveGroundReq
	{
		GameHiddenHBraveGroundReq() : header(MT_HIDDEN_GAME_BRAVE_GROUND_REQ) {}
		MessageHeader header;

		int origin_role_id;
		int req_type;
		int p1;
		int p2;
	};

	struct HiddenGameSyncHuanJieActivityStatus
	{
		HiddenGameSyncHuanJieActivityStatus() : header(MT_HIDDEN_GAME_SYNC_HUAN_JIE_ACTIVITY_STATUS) {}
		MessageHeader	header;

		int season_num;
	};

	struct GameHiddenSendSystemMail
	{
		GameHiddenSendSystemMail() : header(MT_GAME_HIDDEN_SYSTEM_MAIL_SYNC) {}
		MessageHeader header;

		int plat_type;
		int server_id;

		int send_to_uid;
		MailParam mail_param;
	};

	struct HiddenGameSendSystemMail
	{
		HiddenGameSendSystemMail() : header(MT_HIDDEN_GAME_SYSTEM_MAIL_SYNC) {}
		MessageHeader header;

		int send_to_uid;
		MailParam mail_param;
	};

	struct HiddenGameSyncHuanJieRoleInfo
	{
		HiddenGameSyncHuanJieRoleInfo() : header(MT_HIDDEN_GAME_SYNC_HUAN_JIE_ROLE_INFO) {}
		MessageHeader	header;

		int role_uid;
		int rank_score;
		int is_finish_ding_ji;
	};

	struct HiddenGameGuildGatherDreamRoleChange
	{
		HiddenGameGuildGatherDreamRoleChange() : header(MT_HIDDEN_GAME_GUILD_GATHER_DREAM_ROLE_INFO_CHANGE) {}
		MessageHeader	header;

		int role_uid;
		int guild_id;
		SynGatherDreamRecord role_info;
	};

	struct HiddenGameHuanJieSyncTianTiRecord
	{
		HiddenGameHuanJieSyncTianTiRecord() : header(MT_HIDDEN_GAME_HUAN_JIE_SYNC_TIAN_TI_RECORD) {}
		MessageHeader	header;

		int role_id;
		int record_id;
		CommonSaveHuanJieTianTiReceordData record_data;
	};

	struct GameHiddenSyncSingleRecordAck
	{
		GameHiddenSyncSingleRecordAck() : header(MT_GAME_HIDDEN_SYNC_TIAN_TI_RECORD_ACK) {}
		MessageHeader	header;

		int role_id;
		int record_id;
	};

	struct HiddenGameGuildChatSync
	{
		HiddenGameGuildChatSync() : header(MT_HIDDEN_GAME_GUILD_CHAT) {}
		MessageHeader header;

		int guild_id;
		int chat_msg_len;

		char chat_msg[4096];
	};

	struct GameHiddenGuildChatSync
	{
		GameHiddenGuildChatSync() : header(MT_GAME_HIDDEN_GUILD_CHAT) {}
		MessageHeader header;

		int guild_id;
		int chat_msg_len;

		char chat_msg[4096];
	};

	// ��һ�����Ϣ�ڿ�������ı䣬ͬ������Ϸ��
	struct HiddenGameUserCacheSync
	{
		HiddenGameUserCacheSync() : header(MT_HIDDEN_GAME_USERCACHE_CHANGE_SYNC) {}
		MessageHeader header;

		int online_status;
		UserCacheParam usercache_param;
	};

	struct GameHiddenSensitiveWordsReload
	{
		GameHiddenSensitiveWordsReload() : header(MT_GAME_HIDDEN_SENSITIVE_WORDS_RELOAD) {}
		MessageHeader	header;

		int plat_type;
		int server_id;
	};

	struct GameHiddenCmdKickRole
	{
		GameHiddenCmdKickRole() : header(MT_GAME_HIDDEN_CMD_KICK_ROLE) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int role_id;
	};

	struct GameHiddenCmdMuteRole
	{
		GameHiddenCmdMuteRole() : header(MT_GAME_HIDDEN_CMD_MUTE_ROLE) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int role_id;
		unsigned int mute_second;
		int forbid_talk_type;
	};

	struct HiddenGameCmdMuteRole
	{
		HiddenGameCmdMuteRole() : header(MT_HIDDEN_GAME_CMD_MUTE_ROLE) {}
		MessageHeader header;

		int role_id;
		unsigned int mute_second;
		int forbid_talk_type;
	};

	// [�������а�:����] --------------------------  

	enum HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE
	{
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_INVALID = 0,
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_PROF = 1 ,					//ְҵ���   // param_1 : old_prof  , param_2 : curr_prof
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_NAME = 2,					//���ֱ��   // rank_item.role_info.user_name : ������
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_LEVEL = 3,					//�ȼ����   //  rank_item.rank_value : ��Ӧ���͵ĵȼ� 
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_CAPABILITY = 4,			//ս����	 //  rank_item.rank_value : ��Ӧ���͵�ս����
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_HUA_SHEN = 5,				//����ս���� //  rank_item.rank_value : ����ս����
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_DREAM_NOTES = 6,			//��Ԩ��¼	// rank_item.rank_value : ����
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_LUNHUIWANGCHUAN = 7,		//�ֻ�����	// rank_item.rank_value : ����
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_CHIVALROUS = 8,			//����ֵ	// rank_item.rank_value : �����ۼƻ������ֵ
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_NETHER_REWARD = 9,				//��ڤ����	// rank_item.rank_value : ��ñ�����
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_KILL_BOSS = 10,			//��������	// rank_item.rank_value : ���ܻغ���
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_ENDLESS_TOWER = 11,				//�޾�֮�� // rank_item.rank_value  : �޾�֮��ͨ��seq
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_FALLEN_GOD = 12,					//����֮��	// rank_item.rank_value : ����sn
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_MEILI_QIXI = 13,					//������Ϧ // rank_item_rank_value : ����ֵ
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_NEW_COURAGE_CHALLENGE = 14,		//߱��֮�� // rank_item_rank_value : ������ 
		HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_MAX,
	};

	struct HiddenGameRoleInfoChange
	{
		HiddenGameRoleInfoChange() : header(MT_HIDDEN_GAME_ROLE_INFO_CHANGE) {}
		MessageHeader header;

		void Reset()
		{
			origin_plat_type = -1;
			origin_server_id = -1;
			sync_info_type = HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_INVALID;
			param_1 = 0;
			param_2 = 0;
			rank_item.Reset();
		}

		int origin_plat_type;
		int origin_server_id;

		int sync_info_type;
		int param_1;
		int param_2;

		PersonRankItem rank_item;
	};

	// [�������а�:����] --------------------------  

	enum HIDDEN_GAME_PET_INFO_CHANGE_TYPE
	{
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_INVALID = 0,
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_ROLE_INFO = 1,				//��ɫ������Ϣ���  
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_INDEX = 2,					//����Index���		// ������ 
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_REMOVE = 3,					//���ﱻ�Ƴ�		// param_1 : pet_unique_id��32λ  param_2 : pet_unique_id��32λ 
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_CAPABILITY = 4,				//ս����			//  rank_item.rank_value : ����ս����
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_HUA_SHEN = 5,				//����ս����		//  rank_item.rank_value : ����ս����
		HIDDEN_GAME_PET_INFO_CHANGE_TYPE_CHANGE_MAX,
	};

	struct HiddenGamePetInfoChange
	{
		HiddenGamePetInfoChange() : header(MT_HIDDEN_GAME_PET_INFO_CHANGE) {}
		MessageHeader header;

		void Reset()
		{
			origin_plat_type = -1;
			origin_server_id = -1;
			sync_info_type = HIDDEN_GAME_ROLE_INFO_CHANGE_TYPE_CHANGE_INVALID;
			param_1 = 0;
			param_2 = 0;
			rank_item.Reset();
		}

		int origin_plat_type;
		int origin_server_id;

		int sync_info_type;
		int param_1;
		int param_2;

		PetRankItem rank_item;
	};

	struct GameHiddenCmdNoticeNewMessage
	{
		GameHiddenCmdNoticeNewMessage() : header(MT_GAME_HIDDEN_CMD_NOTICE_NEW_MESSAGE) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int role_id;
		int msg_count;
	};

	struct HiddenGameCmdNoticeNewMessage
	{
		HiddenGameCmdNoticeNewMessage() : header(MT_HIDDEN_GAME_CMD_NOTICE_NEW_MESSAGE){}
		MessageHeader header;

		int role_id;
		int msg_count;
	};

	struct GameHiddenConsumeCarouselAddNewRecord
	{
		GameHiddenConsumeCarouselAddNewRecord() : header(MT_GAME_HIDDEN_CONSUME_CAROUSEL_ADD_RECORD) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		unsigned short item_id;
		short item_num;
		GameName role_name;
	};

	struct HiddenGameConsumeCarouselAddNewRecord
	{
		HiddenGameConsumeCarouselAddNewRecord() : header(MT_HIDDEN_GAME_CONSUME_CAROUSEL_ADD_RECORD) {}
		MessageHeader header;

		unsigned short item_id;
		short item_num;
		GameName role_name;
	};

	struct GameHiddenCmdResetPublicNotice
	{
		GameHiddenCmdResetPublicNotice() : header(MT_GAME_HIDDEN_RESET_PUBLIC_NOTICE) {}
		MessageHeader header;

		int plat_type;
		int server_id;
	};

	struct HiddenGameSyncPlatInCrossRoleNum
	{
		HiddenGameSyncPlatInCrossRoleNum() : header(MT_HIDDEN_GAME_SYNC_PLAT_INCROSS_ROLE_NUM) {}
		MessageHeader header;

		int sub_plat_type;
		int incross_role_num;
	};

	struct CrossGameSyncWorldBossInfo		//�����ط���������boss������
	{
		CrossGameSyncWorldBossInfo() : header(MT_HIDDEN_GAME_SYNC_WORLD_BOSS_INFO) {}
		MessageHeader	header;

		int m_boss_npc_id;
		int m_boss_npc_seq;
		int m_send_mail_boss_npc_id;

		int m_boss_scene_id;
		int m_boss_scene_key;
		Posi m_boss_pos;

		int m_boss_cur_hp;
		int m_boss_max_hp;

		int m_open_day;	
		char m_is_kill;
		char is_send_boss_pos;
		short reserve_sh;
	};

	struct CrossGameSyncWorldBoss2Info		//�����ط���������boss2������
	{
		CrossGameSyncWorldBoss2Info() : header(MT_HIDDEN_GAME_SYNC_WORLD_BOSS2_INFO) {}
		MessageHeader	header;

		int m_boss_npc_id;
		int m_boss_npc_seq;
		int m_send_mail_boss_npc_id;

		int m_boss_scene_id;
		int m_boss_scene_key;
		Posi m_boss_pos;

		int m_boss_cur_hp;
		int m_boss_max_hp;

		int m_open_day;
		char m_is_kill;
		char is_send_boss_pos;
		short reserve_sh;
	};

	struct CrossGameSyncWorldBoss3Info		//�����ط���������boss3������
	{
		CrossGameSyncWorldBoss3Info() : header(MT_HIDDEN_GAME_SYNC_WORLD_BOSS3_INFO) {}
		MessageHeader	header;

		int m_boss_npc_id;
		int m_boss_npc_seq;
		int m_send_mail_boss_npc_id;

		int m_boss_scene_id;
		int m_boss_scene_key;
		Posi m_boss_pos;

		int m_boss_cur_hp;
		int m_boss_max_hp;

		int m_open_day;
		char m_is_kill;
		char is_send_boss_pos;
		short reserve_sh;
	};

	struct HiddenGameWorldBossSyncRankResultMsg
	{
		HiddenGameWorldBossSyncRankResultMsg() : header(MT_HIDDEN_GAME_WORLD_BOSS_SYNC_RANK_RESULT_MSG) {}
		MessageHeader header;

		int length;
		char msg_content[8192];
	};

	struct HiddenGameWorldBoss2SyncRankResultMsg
	{
		HiddenGameWorldBoss2SyncRankResultMsg() : header(MT_HIDDEN_GAME_WORLD_BOSS_2_SYNC_RANK_RESULT_MSG) {}
		MessageHeader header;

		int length;
		char msg_content[8192];
	};

	struct HiddenGameWorldBoss3SyncRankResultMsg
	{
		HiddenGameWorldBoss3SyncRankResultMsg() : header(MT_HIDDEN_GAME_WORLD_BOSS_3_SYNC_RANK_RESULT_MSG) {}
		MessageHeader header;

		int length;
		char msg_content[8192];
	};

	struct HiddenGameWorldBossSyncParticipateUidList
	{
		HiddenGameWorldBossSyncParticipateUidList() : header(MT_HIDDEN_GAME_WORLD_BOSS_SYNC_PARTICIPATE_UID_LIST) {}
		MessageHeader header;

		short is_start;
		short uid_num;
		int uid_list[100];
	};

	struct HiddenGameWorldBoss2SyncParticipateUidList
	{
		HiddenGameWorldBoss2SyncParticipateUidList() : header(MT_HIDDEN_GAME_WORLD_BOSS_2_SYNC_PARTICIPATE_UID_LIST) {}
		MessageHeader header;

		short is_start;
		short uid_num;
		int uid_list[100];
	};

	struct HiddenGameWorldBoss3SyncParticipateUidList
	{
		HiddenGameWorldBoss3SyncParticipateUidList() : header(MT_HIDDEN_GAME_WORLD_BOSS_3_SYNC_PARTICIPATE_UID_LIST) {}
		MessageHeader header;

		short is_start;
		short uid_num;
		int uid_list[100];
	};

	struct HiddenGameQuanMinActivityInfoReq
	{
		HiddenGameQuanMinActivityInfoReq() : header(MT_HIDDEN_GAME_REQEST_QUANMIN_ACTIVITY_INFO) {}
		MessageHeader header;

		int uid;
	};

	struct GameHiddenQuanMinActivityInfoResp
	{
		GameHiddenQuanMinActivityInfoResp() : header(MT_GAME_HIDDEN_RESPONSE_QUANMIN_ACTIVITY_INFO) {}
		MessageHeader header;

		int plat_type;
		int server_id;

		int uid;
		int count;
		double cur_discount[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];
		int buy_count[QUAN_MIN_DISCOUNT_COMMODITY_MAX_NUM];
	};

	struct GameHiddenWildBossInfoReq
	{
		GameHiddenWildBossInfoReq() : header(MT_GAME_HIDDEN_WILD_BOSS_INFO_REQ) {}
		MessageHeader header;

		int uid;
		int req_type;
		int param;
		int param1;
	};

	struct GameHiddenShanHaiBossInfoReq
	{
		GameHiddenShanHaiBossInfoReq() : header(MT_GAME_HIDDEN_SHAN_HAI_BOSS_INFO_REQ) {}
		MessageHeader header;

		int uid;
		int req_type;
		int param;
		int param1;
	};

	struct HiddenGameSyncBraveGroundInfo
	{
		HiddenGameSyncBraveGroundInfo() : header(MT_HIDDEN_GAME_SYNC_BRAVE_GROUND_INFO) {}
		MessageHeader header;
		
		int is_notice;
		int top_role_id[BRAVE_GROUND_TOP_ITEM_NUM][BRAVE_GROUND_TOP_ROLE_NUM];
	};

	struct GameHiddenFirstKillReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameHiddenFirstKillReq() : header(MT_GAME_HIDDEN_FIRST_KILL_REQ), video_length(0) 
		{
			memset(video_buffer, 0, sizeof(video_buffer));
			info.Reset();
		}

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
					&& WRITE_INT(video_length)
					&& WRITE_SHORT(info_param.is_server_first_skill)
					&& WRITE_SHORT(info_param.version)
					&& WRITE_SHORT(info_param.battle_mode)
					&& WRITE_INT(info_param.monster_group_id)
					&& WRITE_INT(info_param.average_score)
					&& WRITE_UINT(info_param.record_timestamp)
					&& WRITE_LL(info_param.video_file_id);
			if (!ret) return false;

			ret = WRITE_STRN(video_buffer, video_length);
			if (!ret) return false;

			ret = WRITE_INT(info.id)
			   && WRITE_LL(info.first_kill_info.battle_video_record_file_id)
			   && WRITE_INT(info.first_kill_info.server_id)
			   && WRITE_SHORT(info.first_kill_info.type)
			   && WRITE_SHORT(info.first_kill_info.level_group)
			   && WRITE_SHORT(info.first_kill_info.seq)
   		  	   && WRITE_SHORT(info.first_kill_info.member_count)
			   && WRITE_SHORT(info.first_kill_info.round_num)
			   && WRITE_SHORT(info.first_kill_info.kill_flag)
		       && WRITE_INT(info.first_kill_info.monster_group_id)
			   && WRITE_LL(info.first_kill_info.kill_timestamp)
			   && WRITE_LL(info.first_kill_info.kill_game_timestamp)
			   && WRITE_INT(info.first_kill_info.reward_data.coin)
			   && WRITE_USHORT(info.first_kill_info.reward_data.data.item_id)
			   && WRITE_SHORT(info.first_kill_info.reward_data.data.is_bind)
			   && WRITE_INT(info.first_kill_info.reward_data.data.item_num);
			if (!ret) return false;

			for (int i = 0; i < info.first_kill_info.member_count && i < MEMBER_NAME_MAX_NUM; i++)
			{
				ret = WRITE_INT(info.first_kill_info.uid[i])
				   && WRITE_STRN(info.first_kill_info.member_name[i], sizeof(info.first_kill_info.member_name[i]));

				if (!ret) return false;
			}

			return ret;
		}
		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
					&& READ_INT(video_length)
					&& READ_SHORT(info_param.is_server_first_skill)
					&& READ_SHORT(info_param.version)
					&& READ_SHORT(info_param.battle_mode)
					&& READ_INT(info_param.monster_group_id)
					&& READ_INT(info_param.average_score)
					&& READ_UINT(info_param.record_timestamp)
					&& READ_LL(info_param.video_file_id);
			if (!ret) return false;

			ret = READ_STRN(video_buffer, video_length, MAX_VIDEO_BUFFER);
			if (!ret) return false;

			ret = READ_INT(info.id)
				&& READ_LL(info.first_kill_info.battle_video_record_file_id)
				&& READ_INT(info.first_kill_info.server_id)
				&& READ_SHORT(info.first_kill_info.type)
				&& READ_SHORT(info.first_kill_info.level_group)
				&& READ_SHORT(info.first_kill_info.seq)
				&& READ_SHORT(info.first_kill_info.member_count)
				&& READ_SHORT(info.first_kill_info.round_num)
				&& READ_SHORT(info.first_kill_info.kill_flag)
				&& READ_INT(info.first_kill_info.monster_group_id)
				&& READ_LL(info.first_kill_info.kill_timestamp)
				&& READ_LL(info.first_kill_info.kill_game_timestamp)
				&& READ_INT(info.first_kill_info.reward_data.coin)
				&& READ_USHORT(info.first_kill_info.reward_data.data.item_id)
				&& READ_SHORT(info.first_kill_info.reward_data.data.is_bind)
				&& READ_INT(info.first_kill_info.reward_data.data.item_num);
			if (!ret) return false;

			for (int i = 0; i < info.first_kill_info.member_count && i < MEMBER_NAME_MAX_NUM; i++)
			{
				ret = READ_INT(info.first_kill_info.uid[i])
					&& READ_STRN(info.first_kill_info.member_name[i], sizeof(info.first_kill_info.member_name[i]), sizeof(GameName));

				if (!ret) return false;
				UNSTD_STATIC_CHECK(sizeof(info.first_kill_info.member_name[i]) <= sizeof(GameName));
			}

			return ret;
		}

		static const int MAX_VIDEO_BUFFER = 200000;
		MessageHeader header;

		int video_length;
		char video_buffer[MAX_VIDEO_BUFFER];

		BattleVideoParam info_param;
		ServerFirstKillParam::ServerFirstKillSaveEntry info;
	};

	struct HiddenGameRoleReturnOriginServerReq
	{
		HiddenGameRoleReturnOriginServerReq() : header(MT_HIDDEN_GAME_ROLE_RETURN_ORIGIN_SERVER_REQ) {}
		MessageHeader header;

		int role_id;
		ReturnOriginData_Business business_data;
	};

	struct GameHiddenRoleReturnOriginServerAck
	{
		GameHiddenRoleReturnOriginServerAck() : header(MT_GAME_HIDDEN_ROLE_RETURN_ORIGIN_SERVER_ACK) {}
		MessageHeader header;

		int result;
		int role_id;
		ReturnOriginData_Business business_data;
	};

	struct GameHiddenTeammateJoinOriginServerTeam
	{
		GameHiddenTeammateJoinOriginServerTeam() : header(MT_GAME_HIDDEN_TEAMMATE_JOIN_ORIGIN_SERVER_TEAM)
		{
			business_data.Reset();
		}
		MessageHeader header;

		int team_index;
		ReturnOriginData_Business business_data;
	};

	struct HiddenGameHuanJieZhanChangeSyncInfo
	{
		HiddenGameHuanJieZhanChangeSyncInfo() : header(MT_HIDDEN_GAME_SYNC_HUAN_JIE_INFO) {}
		MessageHeader header;

		int is_notice;
		int role_id[HUAN_JIE_RANK_TITLE_MAX_NUM];
	};

	struct GameCrossQueryRankInfoReq	 //��Ϸ�������ط����������ѯ���������Ϣ
	{
		GameCrossQueryRankInfoReq() : header(MT_GAME_CROSS_QUERY_RANK_INFO_REQ) {}
		MessageHeader header;

		UniqueUserID unique_viewer_uid;
		UniqueUserID unique_user_id;
		short prof;
		short is_ret_original_place;		//Э��ת���ص�ʱ���Ƿ�ԭ·���� 1:ԭ·����
	};

	struct GameHiddenTradeMarketRequestBuy //!< ԭ������������
	{
		GameHiddenTradeMarketRequestBuy() : header(MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_BUY) {}
		MessageHeader header;

		int plat_type;						//!< ƽ̨
		int server_id;						//!< ������ID
		int role_id;						//!< ���uid

		int seller_uid;						//!< ����uid
		short sale_index;					//!< �����һ����ϵ����
		short buy_num;						//!< ��������
		unsigned int upshelve_time;			//!< ��Ʒ�ϼ�ʱ�� �����ں˶���Ʒ��
		ItemID buy_item_id;					//!< �������ƷID �����ں˶���Ʒ�Լ��Զ���������
		short reserve_sh;					//!< ������
		int expect_price;					//!< Ԥ�ڵļ۸������Զ�����ʱ���˶��Ƿ񳬹�Ԥ�ڼ۸�
		UniqueKeyStr unique_key_str;		//!< Ψһ��ʶ�ַ��� �����ں˶���Ʒ��

		long long order_id;					//!< ����ID
	};

	struct HiddenGameTradeMarketRequestBuyResp //!< �������ԭ���۸�
	{
		HiddenGameTradeMarketRequestBuyResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_RESP) {}
		MessageHeader header;

		int role_id;						//!< ���uid
		int total_price;					//!< �ܼ� ��Ҫ�۵�Ԫ������

		int seller_uid;						//!< ����uid
		short sale_index;					//!< �����һ����ϵ����
		short buy_num;						//!< ��������
		unsigned int upshelve_time;			//!< ��Ʒ�ϼ�ʱ�� �����ں˶���Ʒ��
		ItemID buy_item_id;					//!< �������ƷID �����ں˶���Ʒ�Լ��Զ���������
		short reserve_sh;					//!< ������
		int expect_price;					//!< Ԥ�ڵļ۸������Զ�����ʱ���˶��Ƿ񳬹�Ԥ�ڼ۸�
		UniqueKeyStr unique_key_str;		//!< Ψһ��ʶ�ַ��� �����ں˶���Ʒ��
		long long order_id;					//!< ����ID
	};

	struct GameHiddenTradeMarketRequestBuyConfirm //!< ԭ��ȷ���ѿ�Ǯ���ÿ���ٳɽ���
	{
		GameHiddenTradeMarketRequestBuyConfirm() : header(MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_BUY_CONFIRM) {}
		MessageHeader header;

		int plat_type;						//!< ƽ̨
		int server_id;						//!< ������ID
		int role_id;						//!< ���uid
		GameName role_name;					//!< �������
		PlatName role_pname;				//!< ����˺�

		int seller_uid;						//!< ����uid
		short sale_index;					//!< �����һ����ϵ����
		short buy_num;						//!< ��������
		unsigned int upshelve_time;			//!< ��Ʒ�ϼ�ʱ�� �����ں˶���Ʒ��
		ItemID buy_item_id;					//!< �������ƷID �����ں˶���Ʒ�Լ��Զ���������
		short reserve_sh;					//!< ������
		int expect_price;					//!< Ԥ�ڵļ۸������Զ�����ʱ���˶��Ƿ񳬹�Ԥ�ڼ۸�
		UniqueKeyStr unique_key_str;		//!< Ψһ��ʶ�ַ��� �����ں˶���Ʒ��
		long long order_id;					//!< ����ID
		int consumed_gold;					//!< ������
	};

	struct HiddenGameTradeMarketRequestBuyConfirmResp
	{
		HiddenGameTradeMarketRequestBuyConfirmResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_CONFIRM_RESP) {}
		MessageHeader header;

		int role_id;						//!< ���uid  �ѹ���
		GameName role_name;					//!< �������
		PlatName role_pname;				//!< ����˺�

		int seller_uid;						//!< ����uid
		short sale_index;					//!< �����һ����ϵ����
		short buy_num;						//!< ��������
		unsigned int upshelve_time;			//!< ��Ʒ�ϼ�ʱ�� �����ں˶���Ʒ��
		ItemID buy_item_id;					//!< �������ƷID �����ں˶���Ʒ�Լ��Զ���������
		short reserve_sh;					//!< ������
		int expect_price;					//!< Ԥ�ڵļ۸������Զ�����ʱ���˶��Ƿ񳬹�Ԥ�ڼ۸�
		UniqueKeyStr unique_key_str;		//!< Ψһ��ʶ�ַ��� �����ں˶���Ʒ��

		ItemDataWrapper wrapper;			//!< ����򵽵���Ʒ
		long long consumed_gold;			//!< ������

		long long order_id;					//!< ����ID
	};

	struct HiddenGameTradeMarketRequestBuyConfirmErrorResp
	{
		HiddenGameTradeMarketRequestBuyConfirmErrorResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_BUY_CONFIRM_ERROR_RESP) {}
		MessageHeader header;

		int role_id;						//!< ���uid

		int seller_uid;						//!< ����uid
		short sale_index;					//!< �����һ����ϵ����
		short buy_num;						//!< ��������

		ItemID buy_item_id;					//!< �������ƷID �����ں˶���Ʒ�Լ��Զ���������
		short reserve_sh;					//!< ������
		UniqueKeyStr unique_key_str;		//!< Ψһ��ʶ�ַ��� �����ں˶���Ʒ��

		char error_msg[128];				
		int error_num;
		long long order_id;					//!< ����ID
	};

	struct GameHiddenTradeMarketRequestUpshelve
	{
		GameHiddenTradeMarketRequestUpshelve() : header(MT_GAME_HIDDEN_TRADE_MARKET_REQUEST_UPSHELVE) {}
		MessageHeader header;

		int plat_type;						//!< ƽ̨
		int server_id;						//!< ������ID

		int role_id;						//!< ���uid
		GameName role_name;					//!< ��ҽ�ɫ��
		PlatName role_pname;				//!< ����˺�

		int gold_price;						//!< ����  Ԫ��
		ItemDataWrapper item_wrapper;		//!< ��Ʒ����

		int service_fee;					//!< �ϼ�ͭ�ҷ���
		int credit_fee;						//!< �ϼ����ÿ۳�

		long long order_id;					//!< ����ID
	};

	struct HiddenGameTradeMarketRequestUpshelveResp
	{
		HiddenGameTradeMarketRequestUpshelveResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_REQUEST_UPSHELVE_RESP) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_SUCC = 0,
			RESULT_NO_EMPTY_SPACE = 1,
			RESULT_ADD_SALE_ITEM_FAIL = 2,
		};

		int role_id;
		int result;
		int service_fee;					//!< �����  �����ڷ�����
		int trade_credit;					//!< �������ã����ڷ�����
		ItemDataWrapper item_wrapper;		//!< ��Ʒ���� (���ڷ�����
		long long order_id;					//!< ����ID
	};

	struct GameHiddenTradeMarketOffshelveReq
	{
		GameHiddenTradeMarketOffshelveReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_OFFSHELVE_REQ) {}
		MessageHeader header;

		int plat_type;						//!< ƽ̨
		int server_id;						//!< ������ID

		int role_id;
		GameName role_name;					//!< ��ҽ�ɫ��
		PlatName role_pname;				//!< ����˺�

		int sale_index;						//!< �����ڻ����ϵ���� [0,8)
	};

	struct HiddenGameTradeMarketOffshelveResp
	{
		HiddenGameTradeMarketOffshelveResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_OFFSHELVE_RESP) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_SUCC = 0,
		};

		int role_id;
		int sale_index;
		int result;
		ItemDataWrapper item_wrapper;		
		int return_credit;
	};

	struct GameHiddenTradeMarketRedoUpshelveReq
	{
		GameHiddenTradeMarketRedoUpshelveReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_REDO_UPSHELVE_REQ) {}
		MessageHeader header;

		int plat_type;						//!< ƽ̨
		int server_id;						//!< ������ID

		int role_id;
		GameName role_name;					//!< ��ҽ�ɫ��
		PlatName role_pname;				//!< ����˺�

		int sale_index;						//!< ���
		int price;							//!< ����
	};

	struct HiddenGameTradeMarketRedoUpshelveResp
	{
		HiddenGameTradeMarketRedoUpshelveResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_REDO_UPSHELVE_RESP) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_SUCC = 0,
			RESULT_INPUT_PARAM_ERROR = 1,
			RESULT_GET_INFO_FAILED = 2,
			RESULT_NOT_OFFSHELVE = 3,
			RESULT_CASH_OUT_FIRST = 4,
			RESULT_SOLD_OUT = 5,
			RESULT_CANNOT_TRADE = 6,
			RESULT_ITEM_CONFIG_ERROR = 7,
			RESULT_PRICE_NOT_IN_RANGE = 8,
			RESULT_REDO_ADD_FAILED = 9,
		};

		int role_id;
		int sale_index;
		int result;
	};

	struct GameHiddenTradeMarketCashOutReq
	{
		GameHiddenTradeMarketCashOutReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_CASH_OUT_REQ) {}
		MessageHeader header;

		int plat_type;						//!< ƽ̨
		int server_id;						//!< ������ID

		int role_id;
		GameName role_name;					//!< ��ҽ�ɫ��
		PlatName role_pname;				//!< ����˺�

		int sale_index;						//!< ���
	};

	struct HiddenGameTradeMarketCashOutResp
	{
		HiddenGameTradeMarketCashOutResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_CASH_OUT_RESP) {}
		MessageHeader header;

		enum RESULT_TYPE
		{
			RESULT_SUCC = 0,
			RESULT_ITEM_NOT_FOUND = 1,
			RESULT_NO_SOLD_NUM = 2,
			RESULT_CASH_OUT_FAILED = 3,
		};

		int role_id;
		int result;
		int add_gold;
	};

	struct GameHiddenTradeMarketAdvertiseReq
	{
		GameHiddenTradeMarketAdvertiseReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_ADVERTISE_REQ) {}
		MessageHeader header;

		int role_id;
		int plat_type;
		int server_id;
		int sale_index;
	};

	struct HiddenGameTradeMarketAdvertiseResp
	{
		HiddenGameTradeMarketAdvertiseResp() : header(MT_HIDDEN_GAME_TRADE_MARKET_ADVERTISE_RESP) {}
		MessageHeader header;

		int role_id;
		int gold_cost;

		int price;
		ItemName item_name;
		int sale_index;
		unsigned int put_on_shelve_time;
		int item_id;
		long long unique_key;
	};

	struct GameHiddenTradeMarketAdvertiseConfirmBoardcast
	{
		GameHiddenTradeMarketAdvertiseConfirmBoardcast() : header(MT_GAME_HIDDEN_TRADE_MARKET_ADVERTISE_CONFIRM_BOARDCAST) {}
		MessageHeader header;

		int role_id;
		int plat_type;
		int server_id;

		int price;
		ItemName item_name;
		int sale_index;
		unsigned int put_on_shelve_time;
		int item_id;
		long long unique_key;
	};

	// ���Է�������
	struct GameHiddenTestNetwork
	{
		GameHiddenTestNetwork() : header(MT_GAME_HIDDEN_TEST_NETWORK) {}
		MessageHeader header;
	
		short is_start;
		short is_end;

		int send_length;
		char buffer[200000];
	};

	struct GameHiddenBraveGroundGetInfo
	{
		GameHiddenBraveGroundGetInfo() : header(MT_GAME_HIDDEN_BRAVE_GROUND_GET_INFO) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameBraveGroundInfoRet
	{
		HiddenGameBraveGroundInfoRet() : header(MT_HIDDEN_GAME_BRAVE_GROUND_INFO_RET) {}
		MessageHeader header;

		int role_id;
		int reached_level;
	};

	struct GameHiddenBraveGroundSetInfo
	{
		GameHiddenBraveGroundSetInfo() : header(MT_GAME_HIDDEN_BRAVE_GROUND_SET_INFO) {}
		MessageHeader header;

		int role_id;
		int reached_level;
	};

	struct HiddenGameSpecialLogicActivityInfoSync
	{
		HiddenGameSpecialLogicActivityInfoSync() : header(MT_HIDDEN_GAME_SPECIAL_LOGIC_ACTIVITY_INFO_SYNC) {}
		MessageHeader header;

		int role_id;
		int special_logic_activity_type;
		unsigned int fetch_reward_zero_timestamp;
	};

	struct HiddenGameGetRankListReq
	{
		HiddenGameGetRankListReq() : header(MT_HIDDEN_GAME_GET_RANK_LIST_REQ) {}
		MessageHeader header;

		int role_id;
		int rank_type;
		int special_type;
		short rank_start;
		short rank_end;
	};

	struct HiddenGameGetFriendRankListReq
	{
		HiddenGameGetFriendRankListReq() : header(MT_HIDDEN_GAME_PERSON_RANK_FRIEND_REQ) {}
		MessageHeader header;

		int role_id;
		int rank_type;
		short rank_start;
		short rank_end;
		int friend_count;
		int m_friend_list[MAX_FRIEND_NUM];
	};

	struct GameHiddenTradeMarketHistoryTopPriceReq
	{
		GameHiddenTradeMarketHistoryTopPriceReq() : header(MT_GAME_HIDDEN_TRADE_MARKET_HISTORY_TOP_PRICE_REQ) {}
		MessageHeader header;

		int role_id;
	};

	enum HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ_TYPE
	{
		HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ_TYPE_INFO,
		HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ_TYPE_DRAW,
	};

	struct HiddenGameShenShouJiangLinReq
	{
		HiddenGameShenShouJiangLinReq() : header(MT_HIDDEN_GAME_SHEN_SHOU_JIANG_LIN_REQ) {}
		MessageHeader header;

		long long req_id;  //������log
		int role_id;
		int req_type;
	};

	struct HiddenGameHeartBeat
	{
		HiddenGameHeartBeat() : header(MT_HIDDEN_GAME_HEART_BEAT) {}
		MessageHeader header;
	};

	struct GameHiddenRoleInfoNoticeReqRoute
	{
		GameHiddenRoleInfoNoticeReqRoute() : header(MT_GAME_HIDDEN_ROLE_INFO_NOTICE_REQ_ROUTE) {}
		MessageHeader header;

		int role_id;
	};

	struct GameHiddenCloudArenaRealTimeInfoReq
	{
		GameHiddenCloudArenaRealTimeInfoReq() : header(MT_GAME_HIDDEN_CLOUD_ARENA_REAL_TIME_INFO_REQ) {}
		MessageHeader header;

		int role_id;
	};

	struct GameHiddenRoleLoginSendCrossChatGroupSimpleInfo
	{
	public:
		GameHiddenRoleLoginSendCrossChatGroupSimpleInfo() : header(MT_GAME_HIDDEN_ROLE_LOGIN_SEND_CROSS_CHAT_GROUP_SIMPLE_INFO) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int uid;

		long long cross_chat_group_list[MAX_ROLE_CROSS_GROUP];
	};

	struct GameHiddenDeleteMutePlayerChatRecord
	{
	public:
		GameHiddenDeleteMutePlayerChatRecord() : header(MT_GAME_HIDDEN_DELETE_MUTE_PLAYER_CHAT_RECORD) { }
		MessageHeader header;

		int uid;
		int forbid_seconds;
	};

	struct GameHiddenTradeMarketSaleItemEventHandler
	{
	public:
		GameHiddenTradeMarketSaleItemEventHandler() : header(MT_GAME_HIDDEN_TRADE_MARKET_SALE_ITEM_EVENTHANDLER) { }
		MessageHeader header;

		int uid;
		ItemID sel_item_id;
		short reserve_sh;
	};

	struct HiddenGameTradeMarketSaleItemEventHandler
	{
	public:
		HiddenGameTradeMarketSaleItemEventHandler() : header(MT_HIDEEN_GAME_TRADE_MARKET_SALE_ITEM_EVENTHANDLER) { }
		MessageHeader header;

		int uid;
		ItemID sel_item_id;
		short reserve_sh;
	};

	struct HiddenGameRoleColosseumReqRouter
	{
	public:
		HiddenGameRoleColosseumReqRouter() : header(MT_HIDDEN_GAME_COLOSSEUM_REQ_ROUTER) {}
		MessageHeader header;

		int uid;
		int req_type;
		int param;
	};

	struct GameHiddenChatGroupLeadershipHandover
	{
	public:
		GameHiddenChatGroupLeadershipHandover() : header(MT_GAME_HIDDEN_CHAT_GROUP_LEADERSHIP_HANDOVER) {}
		MessageHeader header;

		long long group_id;
		int old_leader_uid;
		int new_leader_uid;
	};

	struct GameHiddenRelicLootingReq
	{
	public:
		GameHiddenRelicLootingReq() : header(MT_GAME_HIDDEN_RELIC_LOOTING_REQ) {}
		MessageHeader header;

		int uid;
		int req_type;
		int param;
	};

	struct HiddenGameRelicLootingReq
	{
	public:
		HiddenGameRelicLootingReq() : header(MT_HIDDEN_GAME_RELIC_LOOTING_REQ) {}
		MessageHeader header;

		int uid;
		int req_type;
		int param;
	};

	struct GameHiddenSyncCloudArenaUserDataReq
	{
	public:
		GameHiddenSyncCloudArenaUserDataReq() : header(MT_GAME_HIDDEN_SYNC_CLOUD_ARENA_USER_DATA_REQ) {}
		MessageHeader header;

		int uid;
		int plat_type;
		int server_id;
	};

	struct HiddenGameSyncCloudArenaUserDataResp
	{
	public:
		HiddenGameSyncCloudArenaUserDataResp() : header(MT_HIDDEN_GAME_SYNC_CLOUD_ARENA_USER_DATA_RESP) {}
		MessageHeader header;

		int uid;
		unsigned int season_timestamp;
		int last_season_rank;
		int season_highest_dan_id;
		int season_highest_rank;
		int season_highest_score;
	};

	struct GameHiddenRoleLoginSyncCloudArenaUserInfo
	{
		GameHiddenRoleLoginSyncCloudArenaUserInfo() : header(MT_GAME_HIDDEN_ROLE_LOGIN_CLOUD_ARENA_USER_INFO_REQ) {}
		MessageHeader header;

		int uid;
	};

	struct GameHiddenMarketBuyItemRecordSynReq
	{
		GameHiddenMarketBuyItemRecordSynReq() : header(MT_GAME_HIDDEN_MARKET_BUY_ITEM_RECORD_SYN_REQ) {}
		MessageHeader header;

		int server_id;
		TransactionRecordInfo tran_record;
	};

	struct HiddenGameMarketBuyItemRecordSynReq
	{
		HiddenGameMarketBuyItemRecordSynReq() : header(MT_HIDDEN_GAME_MARKET_BUY_ITEM_RECORD_SYN_REQ) {}
		MessageHeader header;

		int server_id;
		int is_has_change_price;		//0:δ�޸� 1:���޸�(�޸���Ҫ�·�)
		MarketItemInfo item_info;
	}; 

	struct GameHiddenHundredGhostBossInfoReq
	{
		GameHiddenHundredGhostBossInfoReq() : header(MT_GAME_HIDDEN_HUNDRED_GHOST_BOSS_INFO_REQ) {}
		MessageHeader header;

		int uid;
	};

	struct GameHiddenMarketAllTransactionRecordSynReq : public IMessageSerializer, public IMessageDeserializer
	{
		GameHiddenMarketAllTransactionRecordSynReq() : header(MT_GAME_HIDDEN_MARKET_ALL_TRANSACTION_RECORD_SYN_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type) &&  WRITE_SHORT(add_count);
			if (!ret) return false;

			for (int i = 0; i < add_count && i < MAX_MARKET_ITEM_NUM; i++)
			{
				ret = WRITE_USHORT(disc_cross_add_record_list[i].item_id) && WRITE_INT(disc_cross_add_record_list[i].transaction_num);

				if (!ret) return false;
			}
			
			return ret;
		}
		virtual bool Deserialize(const char * data, int data_length, int * out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type) && READ_SHORT(add_count);
			if (!ret) return false;

			for (int i = 0; i < add_count && i < MAX_MARKET_ITEM_NUM; i++)
			{
				ret = READ_USHORT(disc_cross_add_record_list[i].item_id) && READ_INT(disc_cross_add_record_list[i].transaction_num);

				if (!ret) return false;
			}

			return ret;
		}

		UniqueServerID usid;
		short add_count;
		TransactionRecordInfo disc_cross_add_record_list[MAX_MARKET_ITEM_NUM];
	};

	struct HiddenGameMarketAllTransactionRecordSynReq : public IMessageSerializer, public IMessageDeserializer
	{
		HiddenGameMarketAllTransactionRecordSynReq() : header(MT_HIDDEN_GAME_MARKET_ALL_TRANSACTION_RECORD_SYN_REQ) {}
		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type) && WRITE_INT(op_type) && WRITE_INT(count);
			if (!ret) return false;

			for (int i = 0; i < count && i < MAX_MARKET_ITEM_NUM; i++)
			{
				ret = WRITE_USHORT(item_list[i].item_id) && 
					WRITE_INT(item_list[i].cur_price) &&
					WRITE_INT(item_list[i].yesterday_price) &&
					WRITE_INT(item_list[i].transaction_num) &&
					WRITE_LL(item_list[i].transaction_count) &&
					WRITE_INT(item_list[i].version);

				if (!ret) return false;
			}

			return ret;
		}
		virtual bool Deserialize(const char * data, int data_length, int * out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type) && READ_INT(op_type) && READ_INT(count);
			if (!ret) return false;

			for (int i = 0; i < count && i < MAX_MARKET_ITEM_NUM; i++)
			{
				ret = READ_USHORT(item_list[i].item_id) &&
					READ_INT(item_list[i].cur_price) &&
					READ_INT(item_list[i].yesterday_price) &&
					READ_INT(item_list[i].transaction_num) &&
					READ_LL(item_list[i].transaction_count) &&
					READ_INT(item_list[i].version);

				if (!ret) return false;
			}
			return ret;
		}

		enum OP_TYPE
		{
			OP_TYPE_HIDDEN_CONNECT_CROSS = 0,			//���ط������Ͽ����������Ϸ��ͬ�����׼�¼
			OP_TYPE_HIDDEN_RECV_GAME_INFO_SYN = 1,		//���ط��յ���Ϸ��������Ϣ��ת����������Ϸ��
		};

		int op_type;
		int count;
		MarketItemInfo item_list[MAX_MARKET_ITEM_NUM];
	};

	struct GameHiddenChangeVideoReq
	{
		GameHiddenChangeVideoReq() : header(MT_GAME_HIDDEN_VIDEO_CHANGE_REQ) {}
		MessageHeader header;

		int uid;
		int monster_group_id;
		int video_score;
	};

	struct HiddenGameChangeVideoRet
	{
		HiddenGameChangeVideoRet() : header(MT_HIDDEN_GAME_VIDEO_CHANGE_RET) {}
		MessageHeader header;
		enum RET_REASON
		{
			RET_REASON_NO = 0,
			RET_REASON_YES = 1,
		};
		int ret_reason;
		int monster_group_id;
	};

	struct GameCrossQueryChivalrousRankListTop100Req
	{
		GameCrossQueryChivalrousRankListTop100Req() : header(MT_GAME_CROSS_QUERY_CHIVALROUS_RANK_LIST_TOP100_REQ) {}
		MessageHeader header;

		int uid;
		int server_id;
		int plat_type;
	};

	struct GameCrossQuerySomeRoleChivalrousRankReq
	{
		GameCrossQuerySomeRoleChivalrousRankReq() : header(MT_GAME_CROSS_QUERY_CHIVALROUS_RANK_LIST_ROLE_LIST_REQ) {}
		MessageHeader header;

		int uid;
		int server_id;
		int plat_type;
		int rank_type;

		int count;
		int role_id_list[CHIVALROUS_RANK_LIST_NUM];
	};

	struct CrossGameChivalrousRankListResp
	{
		CrossGameChivalrousRankListResp() : header(MT_CROSS_GAME_CHIVALROUS_RANK_LIST_RESP) {}
		MessageHeader header;

		int uid;
		int count;
		int rank_type;
		ChivalrousRankInfo rank_list[CHIVALROUS_RANK_LIST_NUM];
	};

	struct HiddenGameSyncCloudArenaSeasonInfo
	{
		HiddenGameSyncCloudArenaSeasonInfo() : header(MT_HIDDEN_GAME_SYNC_CLOUD_ARENA_SEASON_INFO) {}
		MessageHeader header;

		unsigned int cur_season_start_timestamp;
		unsigned int cur_season_end_timestamp;
	};
	
		
	struct GameCrossSyncCrossGuildRankInfo				// 29226  �����������ط�ͬ������������а���Ϣ
	{
		GameCrossSyncCrossGuildRankInfo() : header(MT_GAME_CROSS_SYNC_CROSS_GUILD_RANK_INFO) {}
		MessageHeader header;

		struct GuildRankSyncInfo
		{
			GuildRankSyncInfo() : sync_type(0), index(-1) {};
			unsigned int sync_type;						// ͬ��״̬ // �������ݲ���Ч , �������ݲ������
			int index;									// ������ , ����ʱ��Ч, ֻ��ɾ���͸�����Ч
			GuildRankItem rank_info;
		};

		UniqueServerID from_usid;
		bool is_init;									// ��ʼ������ȫ����
		char is_init_add;								// 0 : ����  1 : ���
		short reserve_sh;
		unsigned int last_change_time;
		int rank_type;
		int count;

		GuildRankSyncInfo item_list[GUILD_RANK_MAX_SIZE];
	};

	struct CrossGameSyncGuildRankRewardRankInfo						// 29227  �ɿ��������ͬ������������Ϣ[��Ҫ�������а���]
	{
		CrossGameSyncGuildRankRewardRankInfo() : header(MT_CROSS_GAME_SYNC_GUILD_RANK_REWARD_RANK_INFO) {}
		MessageHeader header;

		UniqueServerID server_id;							// �ĸ���
		int rank_type;										// ���а�����
		int guild_id;										// ����ID
		int rank_index;										// ����
		long long rank_val;									// ��Ӧ���а����͵���ֵ
	};

	struct HiddenGameRoleActiveChange						// 29228  �����ط�ͬ����ɫ��Ծ����Ϣ���������
	{
		HiddenGameRoleActiveChange() : header(MT_HIDDEN_GAME_ROLE_ACTIVE_CHANGE) {}
		MessageHeader header;

		UniqueServerID server_id;							// �ĸ���
		int guild_id;										// ����ID
		int role_id;										// ��ɫID
		int add_val;										// �����ֵ
	};

	struct GameHiddenGuildActiveInfoChange					// 29229  �����������ط�ͬ�������Ծ�ȱ��
	{
		GameHiddenGuildActiveInfoChange() : header(MT_GAME_HIDDEN_GUILD_ACTIVE_INFO_CHANGE) {}
		MessageHeader header;

		int guild_id;										// ����ID
		int day_active_val;									// �����ջ�Ծ��
		int week_active_val;								// �����ܻ�Ծ��
		unsigned int change_times;							// ���ʱ��

		int role_id;										// ��ɫID		[��0ʱ , ���������Ч]
		int member_week_active_val;							// �ܻ�Ծֵ
		int reward_flag;									// ��ȡÿ�ս������
		
		int member_seven_today_active;						// �����Ա�Ľ��ջ�õ��ܻ�Ծ(�ӽ�����忪ʼ��)
	};

	struct HiddenGameRoleActiveGiftInfo						//  �����ط�ͬ����ɫ��Ծ�����Ϣ���������
	{
		HiddenGameRoleActiveGiftInfo() : header(MT_HIDDEN_GAME_ROLE_ACTIVE_GIFT_INFO) {}
		MessageHeader header;

		int guild_id;										// ����ID
		int role_id;										// ��ɫID
		int flag;											// �����ֵ
		unsigned int get_timestamp;							// ��ȡ��ʱ���
	};

	struct HiddenGameQiFuChange						// 29360  �����ط�������ֵ����ԭ��
	{
		HiddenGameQiFuChange() : header(MT_HIDDEN_GAME_ROLE_QI_FU_CHANGE) {}
		MessageHeader header;

		int guild_id;										// ����ID
		int role_id;										// ��ɫID
		int add_val;										// �����ֵ
	};

	struct GameHiddenGuildQiFuValueChange					// 29361  �����������ط�ͬ����ֵ���
	{
		GameHiddenGuildQiFuValueChange() : header(MT_GAME_HIDDEN_ROLE_QI_FU_CHANGE) {}
		MessageHeader header;

		int guild_id;										// ����ID
		int role_id;										// ��ɫID
		int value;											// ��ֵ
	};

	struct GmGameCrossGuildActiveTest						// 29230  ���������������һ�ν���
	{
		GmGameCrossGuildActiveTest() : header(MT_GM_GAME_CROSS_GUILD_ACTIVE_TEST) {}
		MessageHeader header;

	};

	struct GmCrossGameGuildActiveTest						// 29231  �ɿ���������������а���Ϣ
	{
		GmCrossGameGuildActiveTest() : header(MT_GM_CROSS_GAME_GUILD_ACTIVE_TEST) {}
		MessageHeader header;
	};

	struct HiddenGameBigDipperSynRet
	{
		HiddenGameBigDipperSynRet() : hedaer(MT_HIDDEN_GAME_BIG_DIPPER_SYNC_RET) {}
		MessageHeader hedaer;

		BigDipperMonsterParam info;
	};


	// ���
	enum SYNC_HONG_BAO_TO_CROSS_STATUS
	{
		SYNC_HONG_BAO_TO_CROSS_STATUS_INVALID = 0,	// ��Ч
		SYNC_HONG_BAO_TO_CROSS_STATUS_INSERT = 1,	// ���		
		SYNC_HONG_BAO_TO_CROSS_STATUS_UPDATA = 2,	// ����		
		SYNC_HONG_BAO_TO_CROSS_STATUS_DELETE = 3,	// ɾ��		
		SYNC_HONG_BAO_TO_CROSS_STATUS_MAX,
	};

	struct GameHiddenSyncHongBaoInfo					//�����������ط�ͬ�������Ϣ 
	{
		GameHiddenSyncHongBaoInfo() : header(MT_GAME_HIDDEN_SYNC_HONG_BAO_INFO) {}
		MessageHeader	header;

		struct HongBaoItemSyncInfo
		{
			HongBaoItemSyncInfo() : sync_type(0) {};
			unsigned int sync_type;						// ͬ��״̬ // �����ݲ���Ч , �����ݲ������
			int index;									// ����ʱ����
			SyncHongBaoItem hongbap_item;
		};

		UniqueServerID from_usid;
		bool is_init;									// ��ʼ������ȫ����
		char is_init_add;								// 0 : ����  1 : ���
		short reserve_sh;
		int sys_type;

		int count;
		HongBaoItemSyncInfo item_list[MAX_ONCE_HONG_BAO_COUNT];
	};


	struct HiddenGameSyncHongBaoReqGive					// �����ط�������ͬ����������� 
	{
		HiddenGameSyncHongBaoReqGive() : header(MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_GIVE) {}
		MessageHeader	header;

		GiveHongBaoUserInfo		give_hongbao_user_info;
	};

	struct HiddenGameSyncHongBaoReqGet			// �����ط�������ͬ����������� 
	{
		HiddenGameSyncHongBaoReqGet() : header(MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_GET) {}
		MessageHeader	header;

		UniqueServerID from_usid;

		int target_hongbao_id;					// ��ȡ�ĺ��ID
		int hongbao_sys_type;					// ��������ڵ�ϵͳ����	[����:����,����ȵ�..]
		int uid;								// ��ȡ��UID
		GameName name;							// ��ȡ������

		HongBaoItem::InfoData info_data;		// ����sys�Ĳ�ͬ�ô���ͬ , Ҳ����û�õ�,���忴sys_type
	};

	struct GameHiddenSyncHongBaoReqGiveRet			// �����������ط�ͬ��������������
	{
		GameHiddenSyncHongBaoReqGiveRet() : header(MT_GAME_HIDDEN_SYNC_HONG_BAO_REQ_GIVE_RET) {}
		MessageHeader	header;

		UniqueServerID from_usid;
		int req_uid;							// ������ԱUID

		int ret;								// ��� : 0 ʧ�� , 1 �ɹ�

		int hongbao_id;							// ���_ID
		int sys_type;							// ���ϵͳ����
		int type;								// �������
		HongBaoGreeting greeting;				// ף����
	};

	struct GameHiddenSyncHongBaoReqGetRet		// �����������ط�ͬ�������������
	{
		GameHiddenSyncHongBaoReqGetRet() : header(MT_GAME_HIDDEN_SYNC_HONG_BAO_REQ_GET_RET) {}
		MessageHeader	header;

		UniqueServerID from_usid;
		int req_uid;							// ������ԱUID
		int ret;								// ��� : 0 ʧ�� , 1 �ɹ�

		int hongbao_id;							// ���_ID
		int sys_type;							// ���ϵͳ����
		int val;								// �����ȡ�ɹ� ��Ϊ��ȡ���Ļ������� , ���ʧ�� ���Ǵ�����
	};

	enum HiddenGameSyncHongBaoReqError_TYPE
	{
		HiddenGameSyncHongBaoReqError_TYPE_GIVE = 0,	// �����
		HiddenGameSyncHongBaoReqError_TYPE_GET = 1,		// ����
	};

	struct HiddenGameSyncHongBaoReqError		// �����������ط�ͬ�������������
	{
		HiddenGameSyncHongBaoReqError() : header(MT_HIDDEN_GAME_SYNC_HONG_BAO_REQ_ERROR) {}
		MessageHeader	header;

		UniqueServerID from_usid;
		int req_uid;							// ������ԱUID
		int req_type;							// ����������

		int hongbao_id;							// ���_ID	
		int sys_type;							// ���ϵͳ����
		int type;								// �������

		int val;								// �����ȡ�ɹ� ��Ϊ��ȡ���Ļ�������
	};

	enum GAME_HIDDEN_GIVE_GIFT_REQ_TYPE 
	{
		GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_GM_FETCH,
		GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_FETCH,
		GAME_HIDDEN_GIVE_GIFT_REQ_TYPE_ACK,			//��Ϸ������ȷ����Ʒ�Ž�����		
	};

	struct GameHiddenGiveGiftReq		// // ����Ϸ�������ط������������
	{
		GameHiddenGiveGiftReq() : header(MT_GAME_HIDDEN_GIVE_GIFT_REQ) {}
		MessageHeader	header;

		int req_type;

		int role_id;
		union 
		{
			long long give_gift_id_ll;
			int p1;
			int p2;
		};
		
		int p3;		
	};

	struct GameHiddenGiveGiftAddGift 	// ����Ϸ�������ط������������
	{
		GameHiddenGiveGiftAddGift() : header(MT_GAME_HIDDEN_GIVE_GIFT_ADD_GIFT) {}
		MessageHeader	header;

		CommonSaveGiveGiftData gift_data;
	};

	struct HiddenGameGiveGiftRetInfo		//��Ϸ��������ȡ���ﷵ����Ʒ��Ϣ
	{
		HiddenGameGiveGiftRetInfo() : header(MT_GAME_HIDDEN_GIVE_GIFT_INFO_RET) {}
		MessageHeader	header;

		int role_id;
		long long give_gift_id_ll;
		CommonSaveGiveGiftData gift_data;
	};

	enum HIDDEN_GAME_SHI_TU_SEEK_REQ_TYPE 
	{
		HIDDEN_GAME_SHI_TU_SEEK_REQ_TYPE_INFO,
	};

	struct HiddenGameShiTuSeekReq		//���ظ�����Ϸ������ʦͽ������Ϣ
	{
		HiddenGameShiTuSeekReq() : header(MT_HIDDEN_GAME_SHI_TU_SEEK_REQ) {}
		MessageHeader	header;

		int type;
		int role_id;
		int p1;
	};

	struct HiddenGameContinueCrossBattle
	{
		HiddenGameContinueCrossBattle() : header(MT_HIDDEN_GAME_CONTINUE_CROSS_BATTLE) {}
		MessageHeader header;

		int role_id;
	};

	
	struct HiddenGameCowReportGoodNewsCheckCanBuy			// ���ط� ->����  ����ܷ���ţ��ϲ 
	{
		HiddenGameCowReportGoodNewsCheckCanBuy() : header(MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_CHECK_CAN_BUY) {}
		MessageHeader header;

		int role_id;										// ����������UID
	};

	struct GameHiddenCowReportGoodNewsCheckCanBuyAck			// ���� ->���ط�  ����ܷ���ţ��ϲӦ��
	{
		GameHiddenCowReportGoodNewsCheckCanBuyAck() : header(MT_GAME_HIDDEN_COW_REPORT_GOOD_NEWS_CHECK_CAN_BUY_ACK) {}
		MessageHeader header;

		int role_id;										// ����������UID
		int ret;											// 0 :Ϊ���Թ��� , ��0Ϊ���صĴ�����
	};

	struct HiddenGameCowReportGoodNewsBuyItem			// ���ط� ->����  ����ţ��ϲ
	{
		HiddenGameCowReportGoodNewsBuyItem() : header(MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_BUY_ITEM) {}
		MessageHeader header;

		int role_id;
		short  avatar_type;									// ��������
		short  headshot_id;									// ͷ��ID
		GameName role_name;
	};

	struct GameHiddenCowReportGoodNewsBuyItemAck			// ���� ->���ط�   ����ţ��ϲ���Ӧ��
	{
		GameHiddenCowReportGoodNewsBuyItemAck() : header(MT_GAME_HIDDEN_COW_REPORT_GOOD_NEWS_BUY_ITEM_ACK) {}
		MessageHeader header;

		int role_id;
		int ret;											// 0 :����ɹ� , ��0Ϊ���صĴ����� [��Ҫ����Ԫ����]
	};

	struct SyncCowReportGoodNewsBuyItemOfflineEvent			// ͬ������ţ��ϲ����ʧ�ܵ�Ԫ�������¼���
	{
		SyncCowReportGoodNewsBuyItemOfflineEvent() : header(MT_SYNC_COW_REPORT_GOOD_NEWS_BUY_ITEM_OFFLINE_EVENT) {}
		MessageHeader header;

		int role_id;				// ������
		int return_val;				// 0:ɾ�� , > 0 ��Ҫ��¼�ķ�����Ԫ������
	};

	enum HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE
	{
		HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_SYS_INFO ,	// ��ȡ��ţ��ϲ��ϵͳ��Ϣ
		HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_REWARD_INFO,	// ��ȡ��ţ��ϲ�ĵý��б�
		HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO_TYPE_BUY_INFO,	// ��ȡ��ҵĹ�����Ϣ

	};

	struct HiddenGameCowReportGoodNewsGetInfo			// ���ط� ->����  ��ȡ��ţ��ϲ��ϵͳ��Ϣ  
	{
		HiddenGameCowReportGoodNewsGetInfo() : header(MT_HIDDEN_GAME_COW_REPORT_GOOD_NEWS_GET_INFO) {}
		MessageHeader header;

		int role_id;				// ������
		int type;					// ��ȡ����Ϣ����
	};

	/////////////////////////////////
	struct HiddenGameTigerBringsBlessingCheckCanBuy			// ���ط� ->����  ����ܷ��������ɸ�
	{
		HiddenGameTigerBringsBlessingCheckCanBuy() : header(MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_CHECK_CAN_BUY) {}
		MessageHeader header;

		int role_id;										// ����������UID
	};

	struct GameHiddenTigerBringsBlessingCheckCanBuyAck			// ���� ->���ط�  ����ܷ��������ɸ�Ӧ��
	{
		GameHiddenTigerBringsBlessingCheckCanBuyAck() : header(MT_GAME_HIDDEN_TIGER_BRINGS_BLESSING_CHECK_CAN_BUY_ACK) {}
		MessageHeader header;

		int role_id;										// ����������UID
		int ret;											// 0 :Ϊ���Թ��� , ��0Ϊ���صĴ�����
	};

	struct HiddenGameTigerBringsBlessingBuyItem			// ���ط� ->����  ���������ɸ�
	{
		HiddenGameTigerBringsBlessingBuyItem() : header(MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_BUY_ITEM) {}
		MessageHeader header;

		int role_id;
		short  avatar_type;									// ��������
		short  headshot_id;									// ͷ��ID
		GameName role_name;
	};

	struct GameHiddenTigerBringsBlessingBuyItemAck			// ���� ->���ط�   ���������ɸ����Ӧ��
	{
		GameHiddenTigerBringsBlessingBuyItemAck() : header(MT_GAME_HIDDEN_TIGER_BRINGS_BLESSING_BUY_ITEM_ACK) {}
		MessageHeader header;

		int role_id;
		int ret;											// 0 :����ɹ� , ��0Ϊ���صĴ����� [��Ҫ����Ԫ����]
	};

	struct SyncTigerBringsBlessingBuyItemOfflineEvent			// ͬ�����������ɸ�����ʧ�ܵ�Ԫ�������¼���
	{
		SyncTigerBringsBlessingBuyItemOfflineEvent() : header(MT_SYNC_TIGER_BRINGS_BLESSING_BUY_ITEM_OFFLINE_EVENT) {}
		MessageHeader header;

		int role_id;				// ������
		int return_val;				// 0:ɾ�� , > 0 ��Ҫ��¼�ķ�����Ԫ������
	};

	enum HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO_TYPE
	{
		HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO_TYPE_SYS_INFO,	// ��ȡ�����ɸ���ϵͳ��Ϣ
		HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO_TYPE_REWARD_INFO,	// ��ȡ�����ɸ��ĵý��б�
		HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO_TYPE_BUY_INFO,	// ��ȡ��ҵĹ�����Ϣ

	};

	struct HiddenGameTigerBringsBlessingGetInfo			// ���ط� ->����  ��ȡ�����ɸ���ϵͳ��Ϣ  
	{
		HiddenGameTigerBringsBlessingGetInfo() : header(MT_HIDDEN_GAME_TIGER_BRINGS_BLESSING_GET_INFO) {}
		MessageHeader header;

		int role_id;				// ������
		int type;					// ��ȡ����Ϣ����
	};


	enum DATA_SYNC_CHECK_TYPE
	{
		DATA_SYNC_CHECK_TYPE_CROSS_USER_REGISTER, 
		DATA_SYNC_CHECK_TYPE_GUILD,
		DATA_SYNC_CHECK_TYPE_ACTIVITY,
		DATA_SYNC_CHECK_TYPE_WORLD_STATUS,
		DATA_SYNC_CHECK_TYPE_USER_CACHE,
		DATA_SYNC_CHECK_TYPE_CHAT,
		DATA_SYNC_CHECK_TYPE_RANK,
		DATA_SYNC_CHECK_TYPE_ORDER,
		DATA_SYNC_CHECK_TYPE_MARKET,
		DATA_SYNC_CHECK_TYPE_HONG_BAO,
		DATA_SYNC_CHECK_TYPE_FRIEND_INTIMACY,
		DATA_SYNC_CHECK_TYPE_FRIEND,
		DATA_SYNC_CHECK_TYPE_RAND_ACTIVITY,
		DATA_SYNC_CHECK_TYPE_KUA_FU_YING_XIONG_LU,
		DATA_SYNC_CHECK_TYPE_MAX
	};


	struct CrossGameDataSyncCheckAck					// ��� - > ���� �� ���ط� -> ���� ����ͬ����֤��ִ
	{
		CrossGameDataSyncCheckAck() : header(MT_CROSS_GAME_DATA_SYNC_CHECK_ACK) {}
		MessageHeader header;

		int type;					// ͬ������������
	};
	


	////////////////////////////////////////////////////////////////////////////////////////////////////////////


	struct HiddenGameSyncCrossData
	{
		HiddenGameSyncCrossData() : header(MT_HIDDEN_GAME_SYNC_CROSS_DATA) {}
		MessageHeader header;
	};

	struct GameHiddenCreatAutoRobotInfo
	{
		GameHiddenCreatAutoRobotInfo() : header(MT_GAME_HIDDEN_CREAT_AUTO_ROBOT_INFO) {}
		MessageHeader header;

		UniqueServerID mother_server_usid;
		int can_creat_static_robot;	// �ɴ����Զ�������
	};
	
	struct HiddenGameTianDiDaoHenSyncInfo
	{
		HiddenGameTianDiDaoHenSyncInfo() : header(MT_HIDDEN_GAME_TIAN_DI_DAO_HEN_SYNC_INFO) {}
		MessageHeader header;

		int state;
		long long next_state_time;
	};

	struct GameHiddenColosseumRankInfoSyn
	{
		GameHiddenColosseumRankInfoSyn() : header(MT_GAME_HIDDEN_SYN_COLOSSENUM_RANK_INFO) {}
		MessageHeader header;

		int role_id;
		int colosseum_rank;
	};


	enum SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS
	{
		SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_INVALID = 0,		// ��Ч
		SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_INSERT = 1,		// ���		
		SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_UPDATA = 2,		// ����		
		SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_DELETE = 3,		// ɾ��		
		SYNC_FRIEND_INTIMACY_TO_CROSS_STATUS_MAX,
	};

	struct SyncFriendIntimacyInfo						// ͬ���������ܶ���Ϣ ,���� ���ط�->ԭ��  Ҳ������ ԭ��->���ط�,������Ҫ���߼�
	{
		SyncFriendIntimacyInfo() : header(MT_SYNC_FRIEND_INTIMACY_INFO) {}
		MessageHeader header;

		struct FriendItemSyncInfo
		{
			FriendItemSyncInfo() : sync_type(0) {};
			unsigned int sync_type;						// ͬ��״̬ // �����ݲ���Ч , �����ݲ������
			FriendIntimacyInfo friend_item;
		};

		UniqueServerID from_usid;						// �����ĸ�ԭ�� [�����ò���]
		bool is_init;									// ��ʼ������ȫ����
		char is_init_add;								// 0 : ����  1 : ���
		short reserve_sh;

		int count;										// ��
		FriendItemSyncInfo item_list[MAX_ONCE_FRIENDS_INTIMACY_COUNT];
	};
	
	struct GameHiddenAddIntimacyReq						// �����������ط�   ͬ��������ܶȲ�����Ϣ
	{
		GameHiddenAddIntimacyReq() : header(MT_GAME_HIDDEN_ADD_INTIMACY_REQ) {}
		MessageHeader header;

		int role_uid;									// ������ID
		int friend_uid;									// ����ID
		int add_val;									// �����ֵ
	};
	
	struct GameHiddenIntimacyRelationReq						// �����������ط�   ͬ��������ܶȲ�����Ϣ
	{
		GameHiddenIntimacyRelationReq() : header(MT_GAME_HIDDEN_INTIMACY_RELATION_REQ) {}
		MessageHeader header;

		int role_uid;											// ������ID
		int friend_uid;											// ����ID
		int is_add;												// 0:ɾ�� , 1: ���
	};
	
	struct GameHiddenIntimacySingleChatTimes					// �����������ط�   ͬ���������ܶȵ�˽��ʱ���
	{
		GameHiddenIntimacySingleChatTimes() : header(MT_GAME_HIDDEN_INTIMACY_SINGLE_CHAT_TIMES) {}
		MessageHeader header;

		int role_uid;											// ������ID
		int friend_uid;											// ����ID
		int is_to;												// 0:���ѷ������ , 1: ��ҷ�������
		unsigned int timestamp;									// ��Ϣ��ʱ��
	};
	
	struct GameHiddenGiveFlowerNotice							// ͬ�������ͻ���Ϊ�㲥 , ���ڹ㲥��Ч�ʹ���
	{
		GameHiddenGiveFlowerNotice() : header(MT_GAME_HIDDEN_GIVE_FLOWER_NOTICE) {}
		MessageHeader header;

		int role_uid;											// ������ID
		int friend_uid;											// ����ID
		int item_id;											// ������ƷID
		int type;												// ��������
	};
	
	struct HiddenGameGiveFlowerNotice							// �����ط�������   ͬ�������ͻ���Ϊ�㲥, ���ڹ㲥��Ч�ʹ���
	{
		HiddenGameGiveFlowerNotice() : header(MT_HIDDEN_GAME_GIVE_FLOWER_NOTICE) {}
		MessageHeader header;

		int role_id;											// Ŀ�����
		GameName role_name;										// �ͻ�������
		int friend_id;											// �ջ���ID
		GameName friend_name;									// �ջ�������
		int item_id;											// ������ƷID
		int type;												// ������Ч����
	};

	struct GameHiddenGiveFlowerBackKiss							// ���������ط�ͬ��   ͬ�������ͻ�����
	{
		GameHiddenGiveFlowerBackKiss() : header(MT_GAME_HIDDEN_GIVE_FLOWER_BACK_KISS) {}
		MessageHeader header;

		int role_uid;											// ������ID
		GameName role_name;										// ����������
		int to_role_id;											// ������ID
	};

	struct HiddenGameGiveFlowerBackKiss							// ���ط�������ͬ��   ͬ�������ͻ�����
	{
		HiddenGameGiveFlowerBackKiss() : header(MT_HIDDEN_GAME_GIVE_FLOWER_BACK_KISS) {}
		MessageHeader header;

		int role_uid;											// ������ID
		GameName role_name;										// ����������
		int to_role_id;											// ������ID
	};

	struct HiddenGameActivityEndInfo
	{
		HiddenGameActivityEndInfo() : header(MT_HIDDEN_GAME_ACTIVITY_END_INFO) {}
		MessageHeader header;

		int role_id;
		int activity_type;
		int param1;
	};

	struct HiddenGameServerCourseInfoReq
	{
		HiddenGameServerCourseInfoReq() : header(MT_HIDDEN_GAME_SERVER_COURSE_INFO_REQ) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameServerCourseChangeReq
	{
		HiddenGameServerCourseChangeReq() : header(MT_HIDDEN_GAME_SERVER_COURSE_CHANGE_REQ) {}
		MessageHeader header;

		int role_id;
		int seq;
		int is_add;
	};

	struct HiddenGameRoleWinChiefElectionSyn
	{
		HiddenGameRoleWinChiefElectionSyn() : header(MT_HIDDEN_GAME_ROLE_WIN_CHIEF_ELECTION_SYN) {}
		MessageHeader header;

		int role_id;
		int prof;
	};

	struct GameHiddenJieYiZuCreate
	{
		GameHiddenJieYiZuCreate() : header(MT_GAME_HIDDEN_JIE_YI_ZU_CREATE) {}
		MessageHeader header;

		int zu_zhang_id;
		JieYIZuName jie_yi_name;
		int count;
		CommonSaveJieYiZuRoleInfoItem list[JIE_YI_ZU_MAX_ROLE_ITEM_NUM];
	};
	
	struct HiddenGameJieYiZuSync
	{
		HiddenGameJieYiZuSync() : header(MT_HIDDEN_GAME_JIE_YI_ZU_INFO_SYNC) {}
		MessageHeader header;
		
		int sync_type;
		CommonSaveJieYiZu jie_yi_zu_info;
	};

	struct HiddenGameJieYiZuDismiss
	{
		HiddenGameJieYiZuDismiss() : header(MT_HIDDEN_GAME_JIE_YI_ZU_DISMISS) {}
		MessageHeader header;

		long long jie_yi_zu_id;
	};

	struct GameHiddenJieYiZuName
	{
		GameHiddenJieYiZuName() : header(MT_GAME_HIDDEN_JIE_YI_ZU_NAME) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		JieYIZuName ji_yi_zu_name;
	};

	struct GameHiddenJieYiZuMemberName
	{
		GameHiddenJieYiZuMemberName() : header(MT_GAME_HIDDEN_JIE_YI_ZU_MEMBER_NAME) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		JieYIZuMemberName member_name;
	};

	struct GameHiddenJieYiZuText
	{
		GameHiddenJieYiZuText() : header(MT_GAME_HIDDEN_JIE_YI_ZU_TEXT) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		JieYIZuText text;
	};

	struct GameHiddenJieYiZuLeave
	{
		GameHiddenJieYiZuLeave() : header(MT_GAME_HIDDEN_JIE_YI_ZU_LEAVE) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		int is_cancel;
	};

	struct GameHiddenJieYiZuKick
	{
		GameHiddenJieYiZuKick() : header(MT_GAME_HIDDEN_JIE_YI_ZU_KICK) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		int target_role_id;
	};

	struct GameHiddenJieYiZuTanHe
	{
		GameHiddenJieYiZuTanHe() : header(MT_GAME_HIDDEN_JIE_YI_ZU_TAN_HE) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
	};

	struct GameHiddenJieYiZuTanHeVote
	{
		GameHiddenJieYiZuTanHeVote() : header(MT_GAME_HIDDEN_JIE_YI_ZU_TAN_HE_VOTE) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		int target_uid;
	};

	struct GameHiddenJieYiZuInviteReply
	{
		GameHiddenJieYiZuInviteReply() : header(MT_GAME_HIDDEN_JIE_YI_ZU_INVITE_REPLY) {}
		MessageHeader header;

		long long jie_yi_zu_id;
		int role_id;
		int is_agree;
	};

	struct GameHiddenJieYiRoleXuanYan
	{
		GameHiddenJieYiRoleXuanYan() : header(MT_GAME_HIDDEN_JIE_YI_ZU_ROLE_XUAN_YAN) {}
		MessageHeader header;

		int is_delete;
		int role_id;
		JieYIZuXuanYanText text;
	};

	struct GameHiddenJieYiZuXuanYan
	{
		GameHiddenJieYiZuXuanYan() : header(MT_GAME_HIDDEN_JIE_YI_ZU_XUAN_YAN) {}
		MessageHeader header;

		int is_delete;
		int role_id;
		JieYIZuXuanYanText text;
	};

	struct HiddenGameJieYiSyncRoleData
	{
		HiddenGameJieYiSyncRoleData() : header(MT_HIDDEN_GAME_JIE_YI_SYNC_ROLE_DATA) {}
		MessageHeader header;
		
		CommonSaveJieYiZuRoleInfoItem role_data;
	};

	struct GameHiddenJieYiXuanYanList
	{
		GameHiddenJieYiXuanYanList() : header(MT_GAME_HIDDEN_JIE_YI_XUAN_YAN_LIST) {}
		MessageHeader header;

		int role_id;
		int type;
	};

	struct HiddenGameJieYiRoleChange
	{
		HiddenGameJieYiRoleChange() : header(MT_HIDDEN_GAME_JIE_YI_ROLE_CHANGE) {}
		MessageHeader header;

		int role_id;
		int join_jie_yi_day;
	};

	struct HiddenCrossEndlessRankReward
	{
		HiddenCrossEndlessRankReward() : header(MT_HIDDEN_CROSS_ENDLESS_RANK_REWARD) {}
		MessageHeader header;
	};

	struct CrossHiddenEndlessRankRewardRet
	{
		CrossHiddenEndlessRankRewardRet() : header(MT_CROSS_HIDDEN_ENDLESS_RANK_REWARD_RET) {}
		MessageHeader header;
		int count;
		PersonRankItem rank_list[CHIVALROUS_RANK_LIST_NUM];
	};

	struct GameHiddenGiveFlowerChange
	{
		GameHiddenGiveFlowerChange() : header(MT_GAME_HIDDEN_GIVE_FLOWER_CHANGE) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameGiveFlowerChange
	{
		HiddenGameGiveFlowerChange() : header(MT_HIDDEN_GAME_GIVE_FLOWER_CHANGE) {}
		MessageHeader header;

		int role_id;
	};
	enum HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ_TYPE
	{
		HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ_TYPE_INFO,
		HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ_TYPE_DRAW,
	};

	struct HiddenGameHorcruxTianChengReq
	{
		HiddenGameHorcruxTianChengReq() : header(MT_HIDDEN_GAME_HORCRUX_TIAN_CHENG_REQ) {}
		MessageHeader header;

		long long req_id;  //������log
		int role_id;
		int req_type;
		int rand_times;
		BitMap<MAX_HORCRUX_TIANCHENG_REWARD_GROUP_NUM> floor_bitmap;
	};


	struct GameHiddenQueryItemRecommendPrice
	{
		GameHiddenQueryItemRecommendPrice() : header(MT_GAME_HIDDEN_QUERY_ITEM_RECOMMEND_PRICE) {}
		MessageHeader header;

		int plat_type;						//!< ƽ̨
		int server_id;						//!< ������ID

		int role_id;						//!< ���uid

		ItemDataWrapper item_wrapper;		//!< ��Ʒ����

		// �����ֶ�ԭ������
		char is_from_pet;					//!< �Ƿ���� [0:��  1:��]
		char reserve_ch;					//!< ������
		short reserve_sh;					//!< ������
		short index;						//!< ��� ����Ϊ����ʱ���ǻ����ڳ��ﱳ������ţ������ǻ����ڱ�������ţ�
		short column;						//!< ��Ʒ���� �����������ǵ���ʱ��Ч����ʾ�����ڱ����е��ĸ���Ʒ����
	};

	struct HiddenGameQueryTargetGladiatorBeforeFightReq
	{
		HiddenGameQueryTargetGladiatorBeforeFightReq() : header(MT_HIDDEN_GAME_QUERY_TARGET_GLADIATOR_BEFORE_FIGHT_REQ) {}
		MessageHeader header;

		int role_id;
		int target_index;
		int is_sweep;		// �Ƿ���ɨ��
	};

	struct HiddenGameColosseumFightSysReport
	{
		HiddenGameColosseumFightSysReport() : header(MT_HIDDEN_GAME_COLOSSEUM_SYS_REPORT) {}
		MessageHeader header;

		int role_id;
		int target_uid;
		int is_win;
		int is_sweep;
		ColosseumFightStatistic stat;
	};

	struct GameHiddenColosseumSyncNewRankAfterSysReport
	{
		GameHiddenColosseumSyncNewRankAfterSysReport() : header(MT_GAME_HIDDEN_COLOSSEUM_SYNC_NEW_RANK_AFTER_SYS_REPORT) {}
		MessageHeader header;

		int role_id;
		int target_uid;
		int old_rank;
		int new_rank;
		char is_win;
		char is_in_pool;
		char target_is_in_pool;
		char is_sweep;		//�Ƿ���ɨ��
	};

	enum HIDDEN_GAME_JIXING_GAOZHAO_REQ_TYPE
	{
		HIDDEN_GAME_JIXING_GAOZHAO_REQ_TYPE_INFO,
		HIDDEN_GAME_JIXING_GAOZHAO_REQ_TYPE_DRAW,
	};

	struct HiddenGameJiXingGaoZhaoReq
	{
		HiddenGameJiXingGaoZhaoReq() : header(MT_HIDDEN_GAME_JIXING_GAOZHAO_REQ) {}
		MessageHeader header;

		long long req_id;  //������log
		int role_id;
		int req_type;
		int rand_times;
		BitMap<MAX_HORCRUX_TIANCHENG_REWARD_GROUP_NUM> floor_bitmap;
	};

	struct GameCrossHotfix
	{
		GameCrossHotfix() : header(MT_GAME_CROSS_HOTFIX) {}
		MessageHeader header;

		char so_file_name[128];
		char symbol_str[512];
	};

	struct GameHiddenSyncSpecialInfoReq
	{
		GameHiddenSyncSpecialInfoReq() : header(MT_GAME_HIDDEN_SYNC_SPECIAL_INFO_REQ) {}
		MessageHeader header;

		int server_id;
	};

	struct HiddenGameSyncSpecialInfoRet
	{
		HiddenGameSyncSpecialInfoRet() : header(MT_HIDDEN_GAME_SYNC_SPECIAL_INFO_RET) {}
		MessageHeader header;

		unsigned int cross_open_server_timestamp;
		int hidden_server_idx;
	};

	//--------------��Եbegin
	struct GameHiddenQingYuanCreate
	{
		GameHiddenQingYuanCreate() : header(MT_GAME_HIDDEN_QING_YUAN_CREATE) {}
		MessageHeader header;

		int item_type;
		CommonSaveQingYuanRoleInfoItem list[QING_YUAN_MAX_ROLE_NUM];
	};

	struct GameHiddenQingYuanLeave
	{
		GameHiddenQingYuanLeave() : header(MT_GAME_HIDDEN_QING_YUAN_LEAVE) {}
		MessageHeader header;

		long long qing_yuan_id;
	};

	struct GameHiddenQingYuanSingleLeave
	{
		GameHiddenQingYuanSingleLeave() : header(MT_GAME_HIDDEN_QING_YUAN_SINGLE_LEAVE) {}
		MessageHeader header;

		int role_id;
	};

	struct GameHiddenQingYuanForceLeave
	{
		GameHiddenQingYuanForceLeave() : header(MT_GAME_HIDDEN_QING_YUAN_FORCE_LEAVE) {}
		MessageHeader header;

		int role_id;
	};

	struct GameHiddenQingYuanCancelLeave
	{
		GameHiddenQingYuanCancelLeave() : header(MT_GAME_HIDDEN_QING_YUAN_CANCEL_LEAVE) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameQingYuanSyncInfo
	{
		HiddenGameQingYuanSyncInfo() : header(MT_HIDDEN_GAME_QING_YUAN_INFO_SYNC) { is_create = false; item_type = 0; }
		MessageHeader header;

		int sync_type;
		int is_create;
		int item_type;
		CommonSaveQingYuan qing_yuan_info;
	};

	struct GameHiddenQingYuanGiftGive
	{
		GameHiddenQingYuanGiftGive() : header(MT_GAME_HIDDEN_QING_YUAN_GIFT_GiVE) {}
		MessageHeader header;

		int role_id;
		int gift_type;
	};

	struct GameHiddenQingYuanGiftFetchReward
	{
		GameHiddenQingYuanGiftFetchReward() : header(MT_GAME_HIDDEN_QING_YUAN_GIFT_FETCH_REWARD) {}
		MessageHeader header;

		int role_id;
		int gift_type;
	};

	struct HiddenGameQingYuanGiftFetchRewardRet
	{
		HiddenGameQingYuanGiftFetchRewardRet() : header(MT_HIDDEN_GAME_QING_YUAN_GIFT_FETCH_REWARD_RET) {}
		MessageHeader header;

		int role_id;
		int gift_type;
	};

	struct GameHiddenQingYuanGiftFetchDayReward
	{
		GameHiddenQingYuanGiftFetchDayReward() : header(MT_GAME_HIDDEN_QING_YUAN_GIFT_FETCH_DAY_REWARD) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameQingYuanGiftFetchDayRewardRet
	{
		HiddenGameQingYuanGiftFetchDayRewardRet() : header(MT_HIDDEN_GAME_QING_YUAN_GIFT_FETCH_DAY_REWARD_RET) {}
		MessageHeader header;

		int role_id;
		int gift_type;
	};

	struct GameHiddenQingYuanSetSkillFlag
	{
		GameHiddenQingYuanSetSkillFlag() : header(MT_GAME_HIDDEN_QING_YUAN_SET_SKILL_FLAG) {}
		MessageHeader header;

		int role_id;
		unsigned long long skill_flag;
	};

	//--------------��Եend

	struct CrossHiddenFallenGodRankRewardRet
	{
		CrossHiddenFallenGodRankRewardRet() : header(MT_CROSS_HIDDEN_FALLEN_GOD_RANK_REWARD_RET) {}
		MessageHeader header;
		int count;
		PersonRankItem rank_list[CHIVALROUS_RANK_LIST_NUM];
	};

	struct HiddenGameQingYuanDismiss
	{
		HiddenGameQingYuanDismiss() : header(MT_HIDDEN_GAME_QING_YUAN_DISMISS) {}
		MessageHeader header;

		long long qing_yuan_id;
	};

	struct GameHiddenWeddingRoleReq
	{
		GameHiddenWeddingRoleReq() : header(MT_G_H_WEDDING_ROLE_REQ) {}
		MessageHeader header;

		int uid;
		int type;
		int param_1;
		int param_2;
	};
	

	//MT_H_G_WEDDING_INFO_SYNC,		//������Ϣ֪ͨ
	
	struct HiddenGameWeddingInfoSync
	{
		HiddenGameWeddingInfoSync() : header(MT_H_G_WEDDING_INFO_SYNC) {}
		MessageHeader header;

		int type;
		int param_1;
		int param_2;
		int param_3;

		enum Type
		{
			WEDDING_BEGIN,	//���翪�� P1:wedding_id
			WEDDING_END,	//������� P1:wedding_id
		};
	};

	struct GameHiddenQingYuanCommonData
	{
		GameHiddenQingYuanCommonData() : header(MT_GAME_HIDDEN_QING_YUAN_COMMON_DATA) {}
		MessageHeader header;

		long long qingyuan_id;
		CommonSaveQingYuanCommonData common_data;
	};

	struct GameHiddenQingYuanLeiChongAddChongZhi
	{
		GameHiddenQingYuanLeiChongAddChongZhi() : header(MT_GAME_HIDDEN_QING_YUAN_LEICHONG_ADD_CHONGZHI) {}
		MessageHeader header;

		long long qingyuan_id;
		int add_chongzhi;
	};

	struct GameHiddenWorldArenaOperate
	{
		GameHiddenWorldArenaOperate() : header(MT_GAME_HIDDEN_WORLD_ARENA_OPER_TRANSFER) {}
		MessageHeader header;

		int plat_type;
		int uid;

		int oper_type;
		int param1;
		int param2;
	};

	struct HiddenGameWorldArenaGiveWinBoxReward
	{
		HiddenGameWorldArenaGiveWinBoxReward() : header(MT_HIDDEN_GAME_WORLD_ARENA_GIVE_WIN_BOX_REWARD) {}
		MessageHeader header;

		int uid;
		int sn;
	};

	struct HiddenGameWorldArenaGiveSignupReward
	{
		HiddenGameWorldArenaGiveSignupReward() : header(MT_HIDDEN_GAME_WORLD_ARENA_GIVE_SIGNUP_REWARD) {}
		MessageHeader header;

		int uid;
	};

	struct HiddenGameWorldArenaGiveDailyReward
	{
		HiddenGameWorldArenaGiveDailyReward() : header(MT_HIDDEN_GAME_WORLD_ARENA_GIVE_DAILY_REWARD) {}
		MessageHeader header;

		int uid;
	};

	struct GameCrossWorldTeamArenaQueryRankReq
	{
		GameCrossWorldTeamArenaQueryRankReq() : header(MT_GAME_CROSS_WORLD_TEAM_ARENA_QUERY_RANK_REQ) {}
		MessageHeader header;

		int server_id;
		int plat_type;
		long long team_unqiue_id;
		int send_uid;		//��ѯ�󷢸������
		int notice_type;
		int send_type;		//�����·� 0:ս����ϸ��Ϣ 1:����������Ϣ
		WorldTeamArenaQueryRankUserInfo rank_user_info[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];
	};

	struct CrossGameWorldTeamArenaQueryRankRet
	{
		CrossGameWorldTeamArenaQueryRankRet() : header(MT_CROSS_GAME_WORLD_TEAM_ARENA_QUERY_RANK_RET) {}
		MessageHeader header;

		long long team_unique_id;
		int send_uid;		//��ѯ�󷢸������
		int notice_type;
		int send_type;		//�����·� 0:ս����ϸ��Ϣ 1:����������Ϣ
		WorldTeamArenaQueryRankUserInfo rank_user_info[ACTIVITY_WORLD_TEAM_ARENA_TEAM_MEMBER_NUM];
	};

	struct GameHiddenRoleInfoChangeWorldTeamArenaSyn
	{
		GameHiddenRoleInfoChangeWorldTeamArenaSyn() : header(MT_GAME_HIDDEN_ROLE_INFO_CHANGE_WORLD_TEAM_ARENA_SYN) {}
		MessageHeader header;

		WorldTeamArenaUserInfo member_info;
	};

	struct HiddenGameWorldTeamArenaSynRoleTitleInfo
	{
		HiddenGameWorldTeamArenaSynRoleTitleInfo() : header(MT_HIDDEN_GAME_WORLD_TEAM_ARENA_SYN_ROLE_TITLE_INFO) {}
		MessageHeader header;

		int role_id;
		char reason[64];
		WorldTeamArenaRoleInfo info;
	};

	struct HiddenGameWorldTeamArenaSynActivityInfo
	{
		HiddenGameWorldTeamArenaSynActivityInfo() : header(MT_HIDDEN_GAME_WORLD_TEAM_ARENA_SYN_ACTIVITY_INFO) {}
		MessageHeader header;

		ActivityWorldTeamArenaParam activity_info;
	};

	struct GameHiddenWorldTeamArenaActivityInfoSynReq
	{
		GameHiddenWorldTeamArenaActivityInfoSynReq() : header(MT_GAME_HIDDEN_WORLD_TEAM_ARENA_ACITIVITY_INFO_SYN_REQ) {}
		MessageHeader header;

		int server_id;
	};

	struct GameHiddenWorldTeamArenaFinallyRankReq
	{
		GameHiddenWorldTeamArenaFinallyRankReq() : header(MT_GAME_HIDDEN_WORLD_TEAM_ARENA_FINALLY_RANK_REQ) {}
		MessageHeader header;

		int role_id;
		int hidden_server_idx;
	};

	struct GameHiddenSynLevelComplementInfo
	{
		GameHiddenSynLevelComplementInfo() : header(MT_GAME_HIDDEN_SYN_LEVEL_COMPLEMENT_INFO) {}
		MessageHeader header;

		int server_id;
		int average_level_by_level_complement;
	};


	struct GameHiddenQingYuanDuiDuiPengUpDataRankInfo
	{
		GameHiddenQingYuanDuiDuiPengUpDataRankInfo() : header(MT_GAME_HIDDEN_QINGYUAN_DUIDUIPENG_UPDATA_RANK) {}
		MessageHeader header;

		int role_id;
		GameName name;
		short jifen;
		short pass_times;
	};

	struct GameHiddenQingYuanDuiDuiPengRankReq
	{
		GameHiddenQingYuanDuiDuiPengRankReq() : header(MT_GAME_HIDDEN_QINGYUAN_DUIDUIPENG_RANK_REQ) {}
		MessageHeader header;

		int role_id;
	};

	struct HiddenGameQingYuanDuiDuiPengUpRankInfo
	{
		HiddenGameQingYuanDuiDuiPengUpRankInfo() : header(MT_HIDDEN_GAME_QINGYUAN_DUIDUIPENG_RANK_INFO) {}
		MessageHeader header;

		struct RoleData
		{
			int uid;
			GameName name;
			short jifen;
			short pass_times;
		};

		int role_id;
		int count;
		RoleData rank_list[100];
	};

	struct HiddenGameSyncHiddenOpenServerTime
	{
		HiddenGameSyncHiddenOpenServerTime() : header(MT_HIDDEN_GAME_SYNC_HIDDEN_OPEN_SERVER_TIME) {}
		MessageHeader header;

		unsigned int hidden_open_server_time;
	};

	struct GameHiddenSynGuildSpeedUpInfo
	{
		GameHiddenSynGuildSpeedUpInfo() : header(MT_GAME_HIDDEN_GUILD_SPEED_UP_INFO){}
		MessageHeader header;

		int guild_id;										// ����id
		int user_id;
		int ret;
		unsigned int single_building_finish_timestamp;		// �����������ʱ���
	};

	struct HiddenGameSynGuildSpeedUpInfo
	{
		HiddenGameSynGuildSpeedUpInfo() : header(MT_HIDDEN_GAME_GUILD_SPEED_UP_INFO) {}
		MessageHeader header;

		int guild_id;					// ����id
		int user_id;					
	};

	struct GameHiddenSynGuildBuildAllInfo
	{
		GameHiddenSynGuildBuildAllInfo() : header(MT_GAME_HIDDEN_GUILD_SYN_BUILD_ALL_INFO) {}
		MessageHeader header;

		int guild_id;		// ����id 
		short build_level[GUILD_CONSTRUCT_BUILDING];		// [1-5],�±�0-��ǰ���ڽ���Ľ�����
		unsigned int single_building_finish_timestamp;		// �����������ʱ���
		short quicken_building_count;						// �����ѿ������ٴ���
		short can_speed_up;									// �Ƿ�ɼ���
		GameName opener_name;								// ���ٿ����˵�����
		int opener_post;									// ������ְλ
	};

	struct SyncGuildBuildingNotifyInfo
	{
		SyncGuildBuildingNotifyInfo() : header(MT_SYNC_GUILD_BUILDING_NOTIFY_INFO) {}
		MessageHeader header;

		int guild_id;
		Protocol::SCGuildUseBuildMaterialInfo info;
	};


	struct GameHiddenGetUTAChampionRecord		//	���� -> ���ط�  ��ȡ���µ�һ����[���] �ھ���¼�ֲ�
	{
		GameHiddenGetUTAChampionRecord() : header(MT_GAME_HIDDEN_GET_UTA_CHAMPION_RECORD) {}
		MessageHeader header;

		int uid;
		int begin_index;
		int end_index;
	};

	struct HiddenGameGiveLotteryToRole			// ���ط���ԭ��  ����ҷ��ž��±�
	{
		HiddenGameGiveLotteryToRole() : header(MT_HIDDEN_GAME_GIVE_LOTTERY_TO_ROLE) {}
		MessageHeader header;

		int uid;
		int lottery_id;
		int num;
		int reason_type;
	};

	struct HiddenGameUTABaseInfoSync
	{
		HiddenGameUTABaseInfoSync() : header(MT_HIDDEN_GAME_UTA_SYNC_BASE_INFO) {}
		MessageHeader header;

		short cur_status;
		short season_index;
		short round_idx;
		short reserve_sh;
		unsigned int next_battle_timestamp;
		unsigned int active_reward_begin_timestamp;
		unsigned int active_reward_end_timestamp;
		unsigned int season_end_timestamp;
	};

	struct GameHiddenUserRequestWatchBigCrossVideo
	{
		GameHiddenUserRequestWatchBigCrossVideo() : header(MT_GAME_HIDDEN_USER_REQUEST_BIGCROSS_VIDEO) {}
		MessageHeader header;

		int role_id;
		long long video_id;
	};

	struct GameHiddenUserLoginInOrigin
	{
		GameHiddenUserLoginInOrigin() : header(MT_GAME_HIDDEN_USER_LOGIN_IN_ORIGIN) {}
		MessageHeader header;

		int role_id;
	};

	struct GameHiddenQueryWorldTeamArenaOtherInfo
	{
		GameHiddenQueryWorldTeamArenaOtherInfo() : header(MT_GAME_HIDDEN_QUERY_WORLD_TEAM_ARENA_OTHER_INFO) {}
		MessageHeader header;

		int role_id;
	};
	struct GameHiddenGodBeastAdventReq
	{
		GameHiddenGodBeastAdventReq():header(MT_GAME_HIDDEN_GOD_BEAST_ADVENT_BUY_SERVER_LIMIT_ITEM_REQ){}
		MessageHeader header;
	
		int plat_type;
		int server_id;
		int uid;
		int seq;		// ��������
	};

	struct HiddenGameGodBeastAdventInfo
	{
		HiddenGameGodBeastAdventInfo() :header(MT_HIDDEN_GAME_GOD_BEAST_ADVENT_BUY_SERVER_LIMIT_ITEM_INFO) {}
		MessageHeader header;

		int uid;
		int ret;
		int seq;		// ��������
	};

	
	struct HiddenGameGodBeastAdventNumInfo
	{
		HiddenGameGodBeastAdventNumInfo() :header(MT_HIDDEN_GAME_GOD_BEAST_ADVENT_BUY_NUM_INFO) {}
		MessageHeader header;

		int num;
	};

	struct GameHiddenGodBeastAdventReceiveTradeMarketReq
	{
		GameHiddenGodBeastAdventReceiveTradeMarketReq() :header(MT_GAME_HIDDEN_GOD_BEAST_ADVENT_RECEIVE_TRADE_MARKET_REQ) {}
		MessageHeader header;

		int uid;
		int item_id;
		int exchange_group;
	};

	struct HiddenCrossUTAQueryTeamMemberRankReq
	{
		HiddenCrossUTAQueryTeamMemberRankReq() : header(MT_HIDDEN_CROSS_UTA_QUERY_TEAM_MEMBER_RANK_REQ) {}
		MessageHeader header;

		long long unique_team_id;
		int member_uid_list[UTA_TEAM_MEMBER_NUM];
		int member_profession_list[UTA_TEAM_MEMBER_NUM];
	};

	struct CrossHiddenUTAQueryTeamMemberRankResp
	{
		CrossHiddenUTAQueryTeamMemberRankResp() : header(MT_CROSS_HIDDEN_UTA_QUERY_TEAM_MEMBER_RANK_RESP) {}
		MessageHeader header;

		long long unique_team_id;

		int rank_prof[UTA_TEAM_MEMBER_NUM];			//!< ְҵ���� -1����δ�ϰ�
		int rank_capability[UTA_TEAM_MEMBER_NUM];	//!< �������� -1����δ�ϰ�
		int rank_level[UTA_TEAM_MEMBER_NUM];		//!< �ȼ����� -1����δ�ϰ�
	};
	struct GameHiddenHongMengTianJieReq
	{
		GameHiddenHongMengTianJieReq() : header(MT_GAME_HIDDEN_HONGMENG_TIANJIE_REQ) {}
		MessageHeader header;

		int role_uid;
		int req_type;
		int param;
	};

	struct HiddenGameServerCompetitionRankReq
	{
		HiddenGameServerCompetitionRankReq() : header(MT_HIDDEN_GAME_SERVER_COMPETITION_RANK_REQ) {}
		MessageHeader header;

		int role_uid;
	};

	struct GameHiddenServerCompetitionRankResp
	{
		GameHiddenServerCompetitionRankResp() : header(MT_GAME_HIDDEN_SERVER_COMPETITION_RANK_RESP) {}
		MessageHeader header;

		int role_uid;
		Protocol::SCRAServerCompetitionAllRankInfo msg;
	};

	struct GameHiddenSaltySweetBattleChangeReq
	{
		GameHiddenSaltySweetBattleChangeReq() : header(MT_GAME_HIDDEN_SALTY_SWEET_BATTLE_CHANGE_REQ) {}
		MessageHeader header;

		UniqueServerID unique_server_id;
		int pre_win_type;
		int salty_value;
		int sweet_value;
		int status_mod;
		int to_uid;
	};

	struct HiddenGameSaltySweetBattleVoteReq
	{
		HiddenGameSaltySweetBattleVoteReq() : header(MT_HIDDEN_GAME_SALTY_SWEET_BATTLE_VOTE_REQ) {}
		MessageHeader header;

		int role_uid;
		int add_type;
		int add_value;
	};

	struct HiddenGameTeamRoleRecruitSync
	{
		HiddenGameTeamRoleRecruitSync() : header(MT_HIDDEN_GAME_TEAM_ROLE_RECRUIT_SYNC) {}
		MessageHeader header;

		int team_id;
		int team_type;
		GameName leader_name;
		short limit_lv_low;
		short limit_lv_high;
		short cur_member_num;
		short max_member_num;
	};
	struct HiddenCrossMeiLiRankRewardReq
	{
		HiddenCrossMeiLiRankRewardReq() : header(MT_HIDDEN_CROSS_MEILI_RANK_REWARD) {}
		MessageHeader header;
	};

	struct CrossHiddenMeiLiRankRewardRet
	{
		CrossHiddenMeiLiRankRewardRet() : header(MT_CROSS_HIDDEN_MEILI_RANK_REWARD_RET) {}
		MessageHeader header;

		int count;
		PersonRankItem rank_list[CHIVALROUS_RANK_LIST_NUM];
	};

	struct HiddenGameFriendInitmacy
	{
		HiddenGameFriendInitmacy() : header(MT_HIDDEN_GAME_FRIEND_INITIMACY_ADD) {}
		MessageHeader header;

		int to_uid;										// ����ֵ����
		int add_val;									// �����ֵ
	};

	struct GameHiddenFriendInitmacy
	{
		GameHiddenFriendInitmacy() : header(MT_GAME_HIDDEN_FRIEND_INITIMACY_ADD) {}
		MessageHeader header;

		int to_uid;										// ����ֵ����
		int add_val;									// �����ֵ
	};

	struct HiddenGameQueryOriginServerRAStatus
	{
		HiddenGameQueryOriginServerRAStatus() : header(MT_HIDDEN_GAME_QUERY_ORIGIN_SERVER_RA_STATUS) {}
		MessageHeader header;

		int query_uid;
		int query_activity_type;
	};

	struct GameHiddenSendOriginServerRAStatus
	{
		GameHiddenSendOriginServerRAStatus() : header(MT_GAME_HIDDEN_SEND_ORIGIN_SERVER_RA_STATUS) {}
		MessageHeader header;

		int send_to_plat_type;
		int send_to_server_id;
		int open_type;
		short activity_type;
		short status;
		unsigned int next_status_switch_time;
		unsigned int param_1;
		unsigned int param_2;
	};

	struct GameHiddenQueryOfflineBattleDataForRobot
	{
		GameHiddenQueryOfflineBattleDataForRobot() : header(MT_GAME_HIDDEN_QUERY_OFFLINE_BATTLE_DATA_FOR_ROBOT) {}
		MessageHeader header;

		int team_id;
		int server_id;
		int level_min;
		int level_max;
		int best_level_min;
		int best_level_max;
		int leader_level;
		int exclude_uid_num;
		int exclude_uid_list[MAX_TEAM_MEMBER_NUM];
	};

	struct GameHiddenNotifyOfflineBattleDataForRobotResult
	{
		GameHiddenNotifyOfflineBattleDataForRobotResult() : header(MT_GAME_HIDDEN_NOTIFY_OFFLINE_BATTLE_DATA_FOR_ROBOT_RESULT) {}
		MessageHeader header;

		enum Result
		{
			SUCC = 0,
			FAIL = 1,
		};

		int uid;
		int result; 
	};

	struct GameHiddenRealRoleRobotLeaveTeamNotice
	{
		GameHiddenRealRoleRobotLeaveTeamNotice() : header(MT_GAME_HIDDEN_REAL_ROLE_ROBOT_LEAVE_TEAM_NOTICE) {}
		MessageHeader header;

		int uid;
	};

	struct GameHiddenReqSynKuaFuYingXiongLuData
	{
		GameHiddenReqSynKuaFuYingXiongLuData() : header(MT_GAME_HIDDEN_REQ_SYN_KUA_FU_YING_XIONG_LU_DATA) {}
		MessageHeader header;

		int server_id;
	};

	struct HiddenGameRetSynKuaFuYingXiongLuData
	{
		HiddenGameRetSynKuaFuYingXiongLuData() : hedaer(MT_HIDDEN_GAME_RET_SYN_KUA_FU_YING_XIONG_LU_DATA) {}
		MessageHeader hedaer;

		KuaFuYingXiongLuActivityParam data;
	};
	
	struct HiddenCrossReqKuaFuYingXiongLuRankEnd
	{
		HiddenCrossReqKuaFuYingXiongLuRankEnd() : header(MT_HIDDEN_CROSS_REQ_KUA_FU_YING_XIONG_LU_RANK_END) {}
		MessageHeader header;

		int kfyxl_rank_type;
		int person_rank_type;
		int rank_count;						//��ȡ������
		long long activity_unique_id;		//��ӱ�ʶ����,��ֹ�����ش�ʱ��ؿ���ν���
	};

	struct CrossHiddenRetKuaFuYingXiongLuRankEnd
	{
		CrossHiddenRetKuaFuYingXiongLuRankEnd() : hedaer(MT_CROSS_HIDDEN_RET_KUA_FU_YING_XIONG_LU_RANK_END) {}
		MessageHeader hedaer;

		int kfyxl_rank_type;
		int person_rank_type;
		long long activity_unique_id;		//��ӱ�ʶ����,��ֹ�����ش�ʱ��ؿ���ν���
		int rank_uid_list[MAX_KUA_FU_YING_XIONG_LU_RANK_TYPE_SEQ_NUM];
	};

	struct GameHiddenHuanShouAnnInfoReq
	{
		GameHiddenHuanShouAnnInfoReq() : header(MT_GAME_HIDDEN_HUAN_SHOU_ANN_INFO_REQ) {}
		MessageHeader header;

		int role_uid;
	};

	struct GameHiddenHuanShouAnnInfoSync
	{
		GameHiddenHuanShouAnnInfoSync() : header(MT_GAME_HIDDEN_HUAN_SHOU_ANN_INFO_SYNC) {}
		MessageHeader header;

		HuanShouAnnouncementInfo ann_info;
	};

	struct HiddenGameHuanShouAnnInfoSync
	{
		HiddenGameHuanShouAnnInfoSync() : header(MT_HIDDEN_GAME_HUAN_SHOU_ANN_INFO_SYNC) {}
		MessageHeader header;

		HuanShouAnnouncementInfo ann_info;
	};
}
#pragma pack(pop)

//ADDMARK
namespace crossgameprotocal
{
	struct HiddenGameRetPlatformBattleRoleInfo : public IMessageSerializer, public IMessageDeserializer
	{
		virtual int GetMessageType() const;
		virtual bool Serialize(char * buffer, int buffer_size, int * out_length);
		virtual bool Deserialize(const char * data, int data_length, int * out_offset);

		HiddenGameRetPlatformBattleRoleInfo() : header(MT_HIDDEN_GAME_RET_PLATFORM_BATTLE_ROLE_INFO) {}
		MessageHeader	header;

		int type;
		std::vector<MsgPlatformBattleRoleInfoItemList> all_role_info;
	};

	struct HiddenGameSyncLogQuick : public IMessageSerializer, public IMessageDeserializer
	{
		HiddenGameSyncLogQuick() : header(MT_HIDDEN_GAME_SYNC_LOG_QUICK),
			n1(0LL), n2(0LL), n3(0LL), n4(0LL),
			n5(0LL), n6(0LL), n7(0LL), n8(0LL), n9(0LL) {}

		MessageHeader header;

		virtual int GetMessageType() const { return header.msg_type; }

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_LL(n1)
				&& WRITE_LL(n2)
				&& WRITE_INT(z1.length())
				&& WRITE_STRN(z1.c_str(), z1.length())
				&& WRITE_INT(z2.length())
				&& WRITE_STRN(z2.c_str(), z2.length())
				&& WRITE_LL(n3)
				&& WRITE_LL(n4)
				&& WRITE_INT(z3.length())
				&& WRITE_STRN(z3.c_str(), z3.length())
				&& WRITE_INT(z4.length())
				&& WRITE_STRN(z4.c_str(), z4.length())
				&& WRITE_LL(n5)
				&& WRITE_LL(n6)
				&& WRITE_LL(n7)
				&& WRITE_LL(n8)
				&& WRITE_LL(n9);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int z1_len = 0;
			int z2_len = 0;
			int z3_len = 0;
			int z4_len = 0;

			bool ret = READ_BEGIN(header.msg_type)
				&& READ_LL(n1)
				&& READ_LL(n2)
				&& READ_INT(z1_len)
				&& READ_STRING(z1, z1_len)
				&& READ_INT(z2_len)
				&& READ_STRING(z2, z2_len)
				&& READ_LL(n3)
				&& READ_LL(n4)
				&& READ_INT(z3_len)
				&& READ_STRING(z3, z3_len)
				&& READ_INT(z4_len)
				&& READ_STRING(z4, z4_len)
				&& READ_LL(n5)
				&& READ_LL(n6)
				&& READ_LL(n7)
				&& READ_LL(n8)
				&& READ_LL(n9);

			return ret;
		}

		long long n1;
		long long n2;
		std::string z1;
		std::string z2;
		long long n3;
		long long n4;
		std::string z3;
		std::string z4;
		long long n5;
		long long n6;
		long long n7;
		long long n8;
		long long n9;
	};


	struct GameHiddenShenShouJiangLinDrawRet : public IMessageSerializer2,public IMessageDeserializer2
	{
		GameHiddenShenShouJiangLinDrawRet() : header(MT_GAME_HIDDEN_SHEN_SHOU_JIANG_LIN_DRAW_RET) {}
		MessageHeader header;

		long long req_id;
		int role_id;
		int seq;
		int bao_di_id;
		int gold_num;
		std::vector<ItemConfigData> items;
		std::string mail_str;
		std::string broadcast_str;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(TLVSerializer2 * outstream) const
		{
			bool ret = true;

			TLVSerializer2 & out = *outstream;
			ret = ret && out.Push(header.msg_type);
			ret = ret && out.Push(req_id);
			ret = ret && out.Push(role_id);
			ret = ret && out.Push(seq);
			ret = ret && out.Push(bao_di_id);
			ret = ret && out.Push(gold_num);
			int s = (int)items.size();
			ret = ret && out.Push(s);
			for (int i = 0; ret && i < s; ++i)
			{
				const ItemConfigData & curr = items[i];

				ret = ret && out.Push(curr.item_id);
				ret = ret && out.Push(curr.is_bind);
				ret = ret && out.Push(curr.num);
			}

			ret = ret && out.Push(broadcast_str);
			ret = ret && out.Push(mail_str);

			return ret;
		}

		virtual bool Unserialize(TLVUnserializer2 & instream)
		{
			bool ret = true;

			ret = ret && instream.Pop(&header.msg_type);
			ret = ret && instream.Pop(&req_id);
			ret = ret && instream.Pop(&role_id);
			ret = ret && instream.Pop(&seq);
			ret = ret && instream.Pop(&bao_di_id);
			ret = ret && instream.Pop(&gold_num);
			int s = 0;
			ret = ret && instream.Pop(&s);
			for (int i = 0; ret && i < s; ++i)
			{
				ItemConfigData  curr;
				ret = ret && instream.Pop(&curr.item_id);
				ret = ret && instream.Pop(&curr.is_bind);
				ret = ret && instream.Pop(&curr.num);
				items.push_back(curr);
			}
			ret = ret && instream.Pop(&broadcast_str);
			ret = ret && instream.Pop(&mail_str);

			return ret;
		}
	};

	struct HiddenGameSyncRoleBattleData : public IMessageDeserializer, public IMessageSerializer
	{
		enum BUSINESS_TYPE
		{
			BUSINESS_TYPE_COLOSSEUM = 0,
			BUSINESS_TYPE_MAX
		};

		HiddenGameSyncRoleBattleData() : header(MT_HIDDEN_GAME_SYNC_ROLE_BATTLE_DATA) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}		

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(role_id)
				&& WRITE_INT(business_type)
				&& role_battle_data.Serialize(buffer, buffer_size, out_length);
			
			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(role_id)
				&& READ_INT(business_type)
				&& role_battle_data.Deserialize(data, data_length, out_offset);

			return ret;
		}

		int role_id;
		int business_type;
		RoleBattleData role_battle_data;
	};

	struct GameHiddenQeuryTargetGladiatorBeforeFightResp : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenQeuryTargetGladiatorBeforeFightResp() : header(MT_GAME_HIDDEN_QUERY_TARGET_GLADIATOR_BEFORE_FIGHT_RESP) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(role_id)
				&& WRITE_INT(target_index)
				&& WRITE_INT(target_uid)
				&& WRITE_INT(is_sweep)
				&& WRITE_INT(user_total_capability)
				&& WRITE_INT(target_total_capability)
				&& target_battle_data.Serialize(buffer, buffer_size, out_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(role_id)
				&& READ_INT(target_index)
				&& READ_INT(target_uid)
				&& READ_INT(is_sweep)
				&& READ_INT(user_total_capability)
				&& READ_INT(target_total_capability)
				&& target_battle_data.Deserialize(data, data_length, out_offset);

			return ret;
		}

		int role_id;
		int target_index;
		int target_uid;
		int is_sweep;
		int user_total_capability;
		int target_total_capability;
		RoleBattleData target_battle_data;
	};

	struct GameHiddenHorcruxTianChengDrawRet : public IMessageSerializer2, public IMessageDeserializer2
	{
		GameHiddenHorcruxTianChengDrawRet() : header(MT_GAME_HIDDEN_HORCRUX_TIAN_CHENG_DRAW_RET) {}
		MessageHeader header;

		long long req_id;
		int role_id;
		int seq;
		short is_jackpot;
		short money_type;
		int reward_num;
		int floor_back;
		std::vector<ItemConfigData> items;
		std::string broadcast_str;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(TLVSerializer2 * outstream) const
		{
			bool ret = true;

			TLVSerializer2 & out = *outstream;
			ret = ret && out.Push(header.msg_type);
			ret = ret && out.Push(req_id);
			ret = ret && out.Push(role_id);
			ret = ret && out.Push(seq);
			ret = ret && out.Push(is_jackpot);
			ret = ret && out.Push(money_type);
			ret = ret && out.Push(reward_num);
			ret = ret && out.Push(floor_back);
			int s = (int)items.size();
			ret = ret && out.Push(s);
			for (int i = 0; ret && i < s; ++i)
			{
				const ItemConfigData & curr = items[i];

				ret = ret && out.Push(curr.item_id);
				ret = ret && out.Push(curr.is_bind);
				ret = ret && out.Push(curr.num);
			}

			ret = ret && out.Push(broadcast_str);

			return ret;
		}

		virtual bool Unserialize(TLVUnserializer2 & instream)
		{
			bool ret = true;

			ret = ret && instream.Pop(&header.msg_type);
			ret = ret && instream.Pop(&req_id);
			ret = ret && instream.Pop(&role_id);
			ret = ret && instream.Pop(&seq);
			ret = ret && instream.Pop(&is_jackpot);
			ret = ret && instream.Pop(&money_type);
			ret = ret && instream.Pop(&reward_num);
			ret = ret && instream.Pop(&floor_back);
			int s = 0;
			ret = ret && instream.Pop(&s);
			for (int i = 0; ret && i < s; ++i)
			{
				ItemConfigData  curr;
				ret = ret && instream.Pop(&curr.item_id);
				ret = ret && instream.Pop(&curr.is_bind);
				ret = ret && instream.Pop(&curr.num);
				items.push_back(curr);
			}
			ret = ret && instream.Pop(&broadcast_str);

			return ret;
		}
	};

	struct GameHiddenJiXingGaoZhaoDrawRet : public IMessageSerializer2, public IMessageDeserializer2
	{
		GameHiddenJiXingGaoZhaoDrawRet() : header(MT_GAME_HIDDEN_JIXING_GAOZHAO_DRAW_RET) {}
		MessageHeader header;

		long long req_id;
		int role_id;
		int seq;
		short is_jackpot;
		short reward_type;
		int reward_num;
		int floor_back;
		std::vector<ItemConfigData> items;
		std::string broadcast_str;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(TLVSerializer2 * outstream) const
		{
			bool ret = true;

			TLVSerializer2 & out = *outstream;
			ret = ret && out.Push(header.msg_type);
			ret = ret && out.Push(req_id);
			ret = ret && out.Push(role_id);
			ret = ret && out.Push(seq);
			ret = ret && out.Push(is_jackpot);
			ret = ret && out.Push(reward_type);
			ret = ret && out.Push(reward_num);
			ret = ret && out.Push(floor_back);
			int s = (int)items.size();
			ret = ret && out.Push(s);
			for (int i = 0; ret && i < s; ++i)
			{
				const ItemConfigData & curr = items[i];

				ret = ret && out.Push(curr.item_id);
				ret = ret && out.Push(curr.is_bind);
				ret = ret && out.Push(curr.num);
			}

			ret = ret && out.Push(broadcast_str);

			return ret;
		}

		virtual bool Unserialize(TLVUnserializer2 & instream)
		{
			bool ret = true;

			ret = ret && instream.Pop(&header.msg_type);
			ret = ret && instream.Pop(&req_id);
			ret = ret && instream.Pop(&role_id);
			ret = ret && instream.Pop(&seq);
			ret = ret && instream.Pop(&is_jackpot);
			ret = ret && instream.Pop(&reward_type);
			ret = ret && instream.Pop(&reward_num);
			ret = ret && instream.Pop(&floor_back);
			int s = 0;
			ret = ret && instream.Pop(&s);
			for (int i = 0; ret && i < s; ++i)
			{
				ItemConfigData  curr;
				ret = ret && instream.Pop(&curr.item_id);
				ret = ret && instream.Pop(&curr.is_bind);
				ret = ret && instream.Pop(&curr.num);
				items.push_back(curr);
			}
			ret = ret && instream.Pop(&broadcast_str);

			return ret;
		}
	};

	struct GameHiddenWorldArenaSignup : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenWorldArenaSignup() : header(MT_GAME_HIDDEN_WORLD_ARENA_SIGNUP) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& signup_info.Serialize(buffer, buffer_size, out_length)
				&& role_battle_data.Serialize(buffer, buffer_size, out_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& signup_info.Deserialize(data, data_length, out_offset)
				&& role_battle_data.Deserialize(data, data_length, out_offset);

			return ret;
		}

		WorldArenaSignUpInfo signup_info;
		RoleBattleData role_battle_data;
	};

	struct GameHiddenWorldArenaUpdateSignupInfo : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenWorldArenaUpdateSignupInfo() : header(MT_GAME_HIDDEN_WORLD_ARENA_UPDATE_SIGNUP_INFO) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& signup_info.Serialize(buffer, buffer_size, out_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& signup_info.Deserialize(data, data_length, out_offset);

			return ret;
		}

		WorldArenaSignUpInfo signup_info;
	};

	struct HiddenGameUTAGiveItemsToUser : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameUTAGiveItemsToUser() : header(MT_HIDDEN_GAME_UTA_GIVE_ITEMS_TO_USER) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(role_id)
				&& WRITE_SHORT(put_reason_type)
				&& WRITE_SHORT(only_send_mail)
				&& WRITE_INT((int)itemlist.size())
				&& WRITE_INT(mail_subject.length())
				&& WRITE_INT(mail_content.length())
				&& WRITE_STRN(mail_subject.c_str(), mail_subject.length())
				&& WRITE_STRN(mail_content.c_str(), mail_content.length());
			if (!ret) return false;

			for (size_t i = 0; i < itemlist.size(); ++i)
			{
				ret = WRITE_USHORT(itemlist[i].item_id)
					&& WRITE_INT(itemlist[i].num)
					&& WRITE_SHORT(itemlist[i].is_bind)
					&& WRITE_SHORT(itemlist[i].has_param)
					&& WRITE_UINT(itemlist[i].invalid_time);
				if (!ret) return false;

				if (itemlist[i].has_param > 0)
				{
					ret = WRITE_STRN(itemlist[i].param_data, sizeof(itemlist[i].param_data));
					if (!ret) return false;
				}
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int subject_len = 0;
			int content_len = 0;
			int itemlist_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(role_id)
				&& READ_SHORT(put_reason_type)
				&& READ_SHORT(only_send_mail)
				&& READ_INT(itemlist_size)
				&& READ_INT(subject_len)
				&& READ_INT(content_len);
			if (!ret) return false;
			if (subject_len < 0 || subject_len >= MAX_MSG_SIZE) return false;
			if (content_len < 0 || content_len >= MAX_MSG_SIZE) return false;

			ret = READ_STRING(mail_subject, subject_len)
				&& READ_STRING(mail_content, content_len);
			if (!ret) return false;
		
			for (int i = 0; i < itemlist_size; ++i)
			{
				ItemDataWrapper wrapper;
				ret = READ_USHORT(wrapper.item_id)
					&& READ_INT(wrapper.num)
					&& READ_SHORT(wrapper.is_bind)
					&& READ_SHORT(wrapper.has_param)
					&& READ_UINT(wrapper.invalid_time);
				if (!ret) return false;

				if (wrapper.has_param > 0)
				{
					ret = READ_STRN(wrapper.param_data, sizeof(wrapper.param_data), sizeof(wrapper.param_data));
					if (!ret) return false;
				}

				itemlist.push_back(wrapper);
			}

			return ret;
		}

		int role_id;
		short put_reason_type;
		short only_send_mail;
		std::string mail_subject;
		std::string mail_content;
		std::vector<ItemDataWrapper> itemlist;
	};

	struct MsgScheduleInfo
	{
		MsgScheduleInfo() : schedule_id(0), is_main_match(0), is_win_group(0),
			atk_team_unique_id(0LL), atk_plat_type(0), atk_hidden_server_idx(0),
			def_team_unique_id(0LL), def_plat_type(0), def_hidden_server_idx(0)
		{
			memset(atk_team_name, 0, sizeof(GameName));
			memset(def_team_name, 0, sizeof(GameName));
			memset(atk_member_uid_list, 0, sizeof(atk_member_uid_list));
			memset(def_member_uid_list, 0, sizeof(def_member_uid_list));
		}

		long long schedule_id;
		short is_main_match;
		short is_win_group;

		long long atk_team_unique_id;
		int atk_plat_type;
		int atk_hidden_server_idx;
		GameName atk_team_name;
		int atk_member_uid_list[UTA_TEAM_MEMBER_NUM];

		long long def_team_unique_id;
		int def_plat_type;
		int def_hidden_server_idx;
		GameName def_team_name;
		int def_member_uid_list[UTA_TEAM_MEMBER_NUM];
	};

	struct HiddenGameUTASyncSchedules : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameUTASyncSchedules() : header(MT_HIDDEN_GAME_UTA_SYNC_SCHEDULES) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(cur_status)
				&& WRITE_INT(round_idx)
				&& WRITE_INT((int)schedule_list.size());
			if (!ret) return false;

			for (size_t i = 0; i < schedule_list.size(); ++i)
			{
				ret = WRITE_LL(schedule_list[i].schedule_id)
					&& WRITE_SHORT(schedule_list[i].is_main_match)
					&& WRITE_SHORT(schedule_list[i].is_win_group)
					&& WRITE_LL(schedule_list[i].atk_team_unique_id)
					&& WRITE_INT(schedule_list[i].atk_plat_type)
					&& WRITE_INT(schedule_list[i].atk_hidden_server_idx)
					&& WRITE_STRN(schedule_list[i].atk_team_name, sizeof(schedule_list[i].atk_team_name))
					&& WRITE_LL(schedule_list[i].def_team_unique_id)
					&& WRITE_INT(schedule_list[i].def_plat_type)
					&& WRITE_INT(schedule_list[i].def_hidden_server_idx)
					&& WRITE_STRN(schedule_list[i].def_team_name, sizeof(schedule_list[i].def_team_name));
				if (!ret) return false;

				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule_list[i].atk_member_uid_list); ++k)
				{
					ret = WRITE_INT(schedule_list[i].atk_member_uid_list[k]);
					if (!ret) return false;
				}
				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule_list[i].def_member_uid_list); ++k)
				{
					ret = WRITE_INT(schedule_list[i].def_member_uid_list[k]);
					if (!ret) return false;
				}
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int schdule_list_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(cur_status)
				&& READ_INT(round_idx)
				&& READ_INT(schdule_list_size);
			if (!ret) return false;

			schedule_list.reserve(schdule_list_size);
			for (int i = 0; i < schdule_list_size; ++i)
			{
				MsgScheduleInfo schedule;
				ret = READ_LL(schedule.schedule_id)
					&& READ_SHORT(schedule.is_main_match)
					&& READ_SHORT(schedule.is_win_group)
					&& READ_LL(schedule.atk_team_unique_id)
					&& READ_INT(schedule.atk_plat_type)
					&& READ_INT(schedule.atk_hidden_server_idx)
					&& READ_STRN(schedule.atk_team_name, sizeof(schedule.atk_team_name), sizeof(schedule.atk_team_name))
					&& READ_LL(schedule.def_team_unique_id)
					&& READ_INT(schedule.def_plat_type)
					&& READ_INT(schedule.def_hidden_server_idx)
					&& READ_STRN(schedule.def_team_name, sizeof(schedule.def_team_name), sizeof(schedule.def_team_name));
				if (!ret) return false;

				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule.atk_member_uid_list); ++k)
				{
					ret = READ_INT(schedule.atk_member_uid_list[k]);
					if (!ret) return false;
				}
				for (int k = 0; k < ARRAY_ITEM_COUNT(schedule.def_member_uid_list); ++k)
				{
					ret = READ_INT(schedule.def_member_uid_list[k]);
					if (!ret) return false;
				}
				schedule_list.push_back(schedule);
			}

			return ret;
		}

		int cur_status;
		int round_idx;
		std::vector<MsgScheduleInfo> schedule_list;
	};

	struct HiddenGameUTAGiveItemsToServer : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameUTAGiveItemsToServer() : header(MT_HIDDEN_GAME_UTA_GIVE_SERVER_ITEMS) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT((int)itemlist.size())
				&& WRITE_INT(mail_subject.length())
				&& WRITE_INT(mail_content.length())
				&& WRITE_STRN(mail_subject.c_str(), mail_subject.length())
				&& WRITE_STRN(mail_content.c_str(), mail_content.length());
			if (!ret) return false;

			for (size_t i = 0; i < itemlist.size(); ++i)
			{
				ret = WRITE_USHORT(itemlist[i].item_id)
					&& WRITE_INT(itemlist[i].num)
					&& WRITE_SHORT(itemlist[i].is_bind)
					&& WRITE_SHORT(itemlist[i].has_param)
					&& WRITE_UINT(itemlist[i].invalid_time);
				if (!ret) return false;

				if (itemlist[i].has_param > 0)
				{
					ret = WRITE_STRN(itemlist[i].param_data, sizeof(itemlist[i].param_data));
					if (!ret) return false;
				}
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int subject_len = 0;
			int content_len = 0;
			int itemlist_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(itemlist_size)
				&& READ_INT(subject_len)
				&& READ_INT(content_len);
			if (!ret) return false;
			if (subject_len < 0 || subject_len >= MAX_MSG_SIZE) return false;
			if (content_len < 0 || content_len >= MAX_MSG_SIZE) return false;

			ret = READ_STRING(mail_subject, subject_len)
				&& READ_STRING(mail_content, content_len);
			if (!ret) return false;

			for (int i = 0; i < itemlist_size; ++i)
			{
				ItemDataWrapper wrapper;
				ret = READ_USHORT(wrapper.item_id)
					&& READ_INT(wrapper.num)
					&& READ_SHORT(wrapper.is_bind)
					&& READ_SHORT(wrapper.has_param)
					&& READ_UINT(wrapper.invalid_time);
				if (!ret) return false;

				if (wrapper.has_param > 0)
				{
					ret = READ_STRN(wrapper.param_data, sizeof(wrapper.param_data), sizeof(wrapper.param_data));
					if (!ret) return false;
				}
				itemlist.push_back(wrapper);
			}

			return ret;
		}

		std::vector<ItemDataWrapper> itemlist;
		std::string mail_subject;
		std::string mail_content;
	};

	struct GameHiddenSyncPetTopRankInfo : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenSyncPetTopRankInfo() : header(MT_GAME_HIDDEN_SYNC_PET_TOP_RANK_LIST) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		struct RankInfo
		{
			int owner_uid;
			int pet_id;
			long long pet_unique_id;
		};

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT((int)rank_list.size())
				&& WRITE_INT(server_id);
			if (!ret) return false;

			for (size_t i = 0; i < rank_list.size(); ++i)
			{
				ret = WRITE_INT(rank_list[i].owner_uid)
					&& WRITE_INT(rank_list[i].pet_id)
					&& WRITE_LL(rank_list[i].pet_unique_id);
				if (!ret) return false;
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int list_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(list_size)
				&& READ_INT(server_id);
			if (!ret) return false;

			for (int i = 0; i < list_size; ++i)
			{
				RankInfo rankinfo;
				ret = READ_INT(rankinfo.owner_uid)
					&& READ_INT(rankinfo.pet_id)
					&& READ_LL(rankinfo.pet_unique_id);
				if (!ret) return false;

				rank_list.push_back(rankinfo);
			}

			return ret;
		}

		int server_id;
		std::vector<RankInfo> rank_list;
	};

	struct GameHiddenSyncPartnerTopRankInfo : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenSyncPartnerTopRankInfo() : header(MT_GAME_HIDDEN_SYNC_PARTNER_TOP_RANK_LIST) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		struct RankInfo
		{
			int owner_uid;
			int partner_id;
		};

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT((int)rank_list.size())
				&& WRITE_INT(server_id);
			if (!ret) return false;

			for (size_t i = 0; i < rank_list.size(); ++i)
			{
				ret = WRITE_INT(rank_list[i].owner_uid)
					&& WRITE_INT(rank_list[i].partner_id);
				if (!ret) return false;
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int list_size = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(list_size)
				&& READ_INT(server_id);
			if (!ret) return false;

			for (int i = 0; i < list_size; ++i)
			{
				RankInfo rankinfo;
				ret = READ_INT(rankinfo.owner_uid)
					&& READ_INT(rankinfo.partner_id);
				if (!ret) return false;

				rank_list.push_back(rankinfo);
			}

			return ret;
		}

		int server_id;
		std::vector<RankInfo> rank_list;
	};


	struct GameHiddenClearQueryInfo		//�ɱ����������������Ϣ�����
	{
		GameHiddenClearQueryInfo() : header(MT_GAME_HIDDEN_CLEAN_QUERY_INFO) {}
		MessageHeader	header;

		UniqueUserID clear_target_uid;
	};

	struct HiddenGameClearQueryInfo		//�ɱ����������������Ϣ�����
	{
		HiddenGameClearQueryInfo() : header(MT_HIDDEN_GAME_CLEAN_QUERY_INFO) {}
		MessageHeader	header;

		UniqueUserID clear_target_uid;
	};

	struct GameHiddenCmdToRole
	{
		GameHiddenCmdToRole() : header(MT_GAME_HIDDEN_CMD_TO_ROLE) {}
		MessageHeader header;

		int plat_type;
		int server_id;
		int role_id;
		unsigned int effect_type;
		int effect_param;
		int effect_param1;
	};

	struct HiddenGameCmdToRole
	{
		HiddenGameCmdToRole() : header(MT_HIDDEN_GAME_CMD_TO_ROLE) {}
		MessageHeader header;

		int role_id;
		unsigned int effect_type;
		int effect_param;
		int effect_param1;
	};
	struct HiddenGameQueryOfflineBattleDataForRobot : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameQueryOfflineBattleDataForRobot() : header(MT_HIDDEN_GAME_QUERY_OFFLINE_BATTLE_DATA_FOR_ROBOT){}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(team_id)
				&& WRITE_INT(uid)
				&& rbd.Serialize(buffer, buffer_size, out_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(team_id)
				&& READ_INT(uid)
				&& rbd.Deserialize(data, data_length, out_offset);

			return ret;
		}

		int team_id;
		int uid;
		RoleBattleData rbd;
	};

	struct GameHiddenSyncOfflineBattleData : public IMessageDeserializer, public IMessageSerializer
	{
		GameHiddenSyncOfflineBattleData() : header(MT_GAME_HIDDEN_SYNC_OFFLINE_BATTLE_DATA) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(uid)
				&& rbd.Serialize(buffer, buffer_size, out_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(uid)
				&& rbd.Deserialize(data, data_length, out_offset);

			return ret;
		}

		int uid;
		RoleBattleData rbd;
	};

	struct HiddenGameQueryBattleDataForOfflineBattleDataSystem : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameQueryBattleDataForOfflineBattleDataSystem() : header(MT_HIDDEN_GAME_QUERY_BATTLE_DATA_FOR_OFFLINE_BATTLE_DATA_SYSTEM) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			int count = (int)exclude_uid_set.size();
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(count);

			for (std::set<int>::iterator it = exclude_uid_set.begin(); it != exclude_uid_set.end(); ++it)
			{
				int uid = *it;
				ret =  ret && WRITE_INT(uid);
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int count = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(count);

			for (int i = 0; i < count; ++i)
			{
				int uid = 0;
				ret = ret && READ_INT(uid);
				exclude_uid_set.insert(uid);
			}

			return ret;
		}

		std::set<int> exclude_uid_set;
	};

	struct HiddenGameAddSingleChatRecord : public IMessageDeserializer, public IMessageSerializer
	{
		HiddenGameAddSingleChatRecord() : header(MT_HIDDEN_GAME_ADD_SINGLE_CHAT_RECORD) {}
		MessageHeader header;

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(uid)
				&& WRITE_INT(chat_with_uid)
				&& WRITE_INT(msg_length)
				&& WRITE_STRN(msg, msg_length);

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(uid)
				&& READ_INT(chat_with_uid)
				&& READ_INT(msg_length)
				&& READ_STRN(msg, msg_length, sizeof(msg));

			return ret;
		}

		int uid;
		int chat_with_uid;
		int msg_length;
		char msg[4096];
	};

	struct HiddenGameQueryUserChatRecord : public IMessageDeserializer, public IMessageSerializer
	{
		MessageHeader header;
		HiddenGameQueryUserChatRecord() : header(MT_HIDDEN_GAME_USER_QUERY_CHAT_RECORD) {}

		virtual int GetMessageType() const
		{
			return header.msg_type;
		}

		virtual bool Serialize(char* buffer, int buffer_size, int* out_length)
		{
			int count = (int)chat_with_uid_set.size();
			bool ret = WRITE_BEGIN(header.msg_type)
				&& WRITE_INT(uid)
				&& WRITE_INT(count);

			for (std::set<int>::iterator it = chat_with_uid_set.begin(); it != chat_with_uid_set.end(); ++it)
			{
				int uid = *it;
				ret = ret && WRITE_INT(uid);
			}

			return ret;
		}

		virtual bool Deserialize(const char* data, int data_length, int* out_offset)
		{
			int count = 0;
			bool ret = READ_BEGIN(header.msg_type)
				&& READ_INT(uid)
				&& READ_INT(count);

			for (int i = 0; i < count; ++i)
			{
				int uid = 0;
				ret = ret && READ_INT(uid);
				chat_with_uid_set.insert(uid);
			}

			return ret;
		}

		int uid;
		std::set<int> chat_with_uid_set;
	};
}

//ADDMARK

#endif	// __CROSS_GAME_PROTOCAL_H__


