/******************************************************************************************
FileName: system.hpp
Author: 
Description: 系统相关
Version: 1.0
History: 
<author>    <time>					<version >    <desc>
dreanwang    2011/08/17/ 09:04:05     1.0           创建
*****************************************************************************************/

#ifndef __SYSTEM_HPP__
#define __SYSTEM_HPP__

#include <string>
#include <queue>
#include <map>
#include <sstream>

#include "iclockmodule.h"
#include "normalpublicnotice.hpp"

struct TimeOutLogInfo
{
	TimeOutLogInfo() : rmi_launch_time(0u), query_timestamp(0u) {}

	unsigned int rmi_launch_time;
	unsigned int query_timestamp;
};
namespace systemcmd
{
	enum EffectType					// 对角色操作的类型定义
	{
		EFFECT_AUTHORITY = 0,		// 改变管理权限

	};
}


class System
{
	friend class RMIGetCmdBackObjectImpl;
	typedef bool (System::*DBCommandHandler)(int cmd_id, std::istringstream& iss, std::string& error_msg);
	typedef std::map<int, System::DBCommandHandler> DBCommandHandlerMap;
	typedef std::map<UniqueServerID, NormalPublicNotice> NormalPublicNoticeMap;

public:
	static System &Instance();

	void Init();
	void Update(unsigned long interval, time_t now_second);
	
	bool NormalNoticeIsEmpty(const UniqueServerID& usid);
	void AddNormalNotice(const UniqueServerID& usid, const PublicNoticeList::NoticeAttr &notice_attr);
	void UpdateNormalNotice(time_t now_second);
	void SyncNoticeFromGame(const UniqueServerID& usid, int count, const PublicNoticeList::NoticeAttr* notice_attr);

	void ResetNotice(const UniqueServerID& usid);
	
	void AddTimeOutLogInfo(unsigned int query_timestamp);

	bool OnCmdToRole(Role * role, int effect_type, int effect_param, int effect_param1);
	bool OnCmdToOfflineRegister(int uid, int effect_type, int effect_param, int effect_param1);
	bool OnCmdToRole(int role_id, int effect_type, int effect_param, int effect_param1);

private:
	System();
	~System();

	enum NEWNOTICE_TYPE
	{
		NEWNOTICE_TYPE_INVALID = 0,
		NEWNOTICE_TYPE_QUANFU_MAIL,				// 全服邮件				
		NEWNOTICE_TYPE_PERSON_MAIL,				// 系统发给个人邮件
		NEWNOTICE_TYPE_CHONGZHI,				// 充值

		NEWNOTICE_TYPE_MAX,
	};

	bool OnCmd(int cmd_id, const std::string &creator, const std::string &cmd, ARG_OUT std::string& error_msg, long long opid);
	void NewNoticeCmdHelper(int notice_type, int role_id);

	bool AddDataaccessAllowIPPrefix(int cmd_id, std::istringstream& iss, std::string& error_msg);		// 数据库重读AllowIPPrefix配置
	void StartPublicNoticeCheck();				// 到数据库检查是否有公告信息
	void StartCmdCheck();						// 到数据库检查是否有命令
	void StartTimeOutLogCheck();				// 到数据库获取超时rmi请求的command并写log

	//命令
	bool Cmd(int cmd_id, std::istringstream& iss, std::string& error_msg);

	//角色
	bool CmdToRole(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool Mute(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdToRoleRename(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdToRoleKickOut(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool Retrieve(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdRoleHasNewMessage(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool RetrieveOtherMoney(int cmd_id, std::istringstream& iss, std::string& error_msg);

	//活动
	bool CmdToActivity(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdToSetActivityTime(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool ForceSetRandActivityTime(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdToRandActivity(int cmd_id, std::istringstream& iss, std::string& error_msg);

	bool ResetNotice(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdChongzhi(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdNewNotice(int cmd_id, std::istringstream& iss, std::string& error_msg);

	// 交易行
	bool CmdTradeMarketUpshelveNormalItem(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdTradeMarketUpshelvePet(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdTradeMarketUpshleveRandomPet(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdTradeMarketUpshelveEquip(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdTradeMarketUpshelveJewel(int cmd_id, std::istringstream& iss, std::string& error_msg);

	bool CmdTradeMarketSetRecycleRule(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdTradeMarketRemoveRecycleRule(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdSetSystemAutoUpshelve(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdUnsetSystemAutoUpshelve(int cmd_id, std::istringstream& iss, std::string& error_msg);

	bool CmdAddTradeCredit(int cmd_id, std::istringstream & iss, std::string & error_msg);

	bool SensitiveWords(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdToReloadCrossConfig(int cmd_id, std::istringstream& iss, std::string& error_msg);

	bool CmdRetrievePet(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdQueryRetrievePet(int cmd_id, std::istringstream& iss, std::string& error_msg);

	bool CmdRetrieveExp(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool CmdRoute(int cmd_id, std::istringstream & iss, std::string & error_msg);  //新增后台->服务端->客户端透传指令
	bool CmdOrderManagerSetOpen(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool CmdHotfix(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool CmdBattleHotfix(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool CmdChangeGuildName(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool CmdChangeGuildNotice(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool CmdChangeGuildPost(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool CmdGuildQuit(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool CmdNoticeClientRestart(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool CmdCrossHotfix(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool CmdBlockClientHeartBeatCheck(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool CmdToWorldTeamArena(int cmd_id, std::istringstream& iss, std::string& error_msg);			//把英雄会武的排名名单转成报名名单
	bool CmdToAdjWorldTeamArenaTime(int cmd_id, std::istringstream& iss, std::string& error_msg);	//调整英雄会武时间
	bool CmdToMergeCross(int cmd_id, std::istringstream& iss, std::string& error_msg);				//合并跨服,往跨服插入的命令
	bool CmdToCloseKuaFuYingXiongLu(int cmd_id, std::istringstream& iss, std::string& error_msg);	//关闭跨服英雄录
	bool CmdToSetKuaFuYingXiongLuLockFlag(int cmd_id, std::istringstream& iss, std::string& error_msg);	//设置跨服英雄录锁状态
	bool CmdMallocTrim(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdKill(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdSyncInfoToHidden(int cmd_id, std::istringstream& iss, std::string& error_msg);			//同步信息到跨服

	time_t m_next_get_public_notice;			// 下次请求公告时间
	time_t m_next_get_cmd;						// 下次请求命令时间

	PublicNoticeList::NoticeAttr m_normal_noticelist[PublicNoticeList::MAX_PUBLICNOTICE_NUM];	// 正常公告一轮数据

	NormalPublicNotice m_normal_notice;			// 游戏服的正常公告
	NormalPublicNoticeMap m_hidden_notice_map;	// 隐藏服的正常公告

	unsigned int m_next_check_timeout_log_time;
	std::deque<TimeOutLogInfo> m_timeout_log_info_queue;

	DBCommandHandlerMap m_cmd_handler_map;
};

#endif  // __SYSTEM_HPP__

