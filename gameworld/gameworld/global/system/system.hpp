/******************************************************************************************
FileName: system.hpp
Author: 
Description: ϵͳ���
Version: 1.0
History: 
<author>    <time>					<version >    <desc>
dreanwang    2011/08/17/ 09:04:05     1.0           ����
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
	enum EffectType					// �Խ�ɫ���������Ͷ���
	{
		EFFECT_AUTHORITY = 0,		// �ı����Ȩ��

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
		NEWNOTICE_TYPE_QUANFU_MAIL,				// ȫ���ʼ�				
		NEWNOTICE_TYPE_PERSON_MAIL,				// ϵͳ���������ʼ�
		NEWNOTICE_TYPE_CHONGZHI,				// ��ֵ

		NEWNOTICE_TYPE_MAX,
	};

	bool OnCmd(int cmd_id, const std::string &creator, const std::string &cmd, ARG_OUT std::string& error_msg, long long opid);
	void NewNoticeCmdHelper(int notice_type, int role_id);

	bool AddDataaccessAllowIPPrefix(int cmd_id, std::istringstream& iss, std::string& error_msg);		// ���ݿ��ض�AllowIPPrefix����
	void StartPublicNoticeCheck();				// �����ݿ����Ƿ��й�����Ϣ
	void StartCmdCheck();						// �����ݿ����Ƿ�������
	void StartTimeOutLogCheck();				// �����ݿ��ȡ��ʱrmi�����command��дlog

	//����
	bool Cmd(int cmd_id, std::istringstream& iss, std::string& error_msg);

	//��ɫ
	bool CmdToRole(int cmd_id, std::istringstream & iss, std::string & error_msg);
	bool Mute(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdToRoleRename(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdToRoleKickOut(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool Retrieve(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdRoleHasNewMessage(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool RetrieveOtherMoney(int cmd_id, std::istringstream& iss, std::string& error_msg);

	//�
	bool CmdToActivity(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdToSetActivityTime(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool ForceSetRandActivityTime(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdToRandActivity(int cmd_id, std::istringstream& iss, std::string& error_msg);

	bool ResetNotice(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdChongzhi(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdNewNotice(int cmd_id, std::istringstream& iss, std::string& error_msg);

	// ������
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
	bool CmdRoute(int cmd_id, std::istringstream & iss, std::string & error_msg);  //������̨->�����->�ͻ���͸��ָ��
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
	bool CmdToWorldTeamArena(int cmd_id, std::istringstream& iss, std::string& error_msg);			//��Ӣ�ۻ������������ת�ɱ�������
	bool CmdToAdjWorldTeamArenaTime(int cmd_id, std::istringstream& iss, std::string& error_msg);	//����Ӣ�ۻ���ʱ��
	bool CmdToMergeCross(int cmd_id, std::istringstream& iss, std::string& error_msg);				//�ϲ����,��������������
	bool CmdToCloseKuaFuYingXiongLu(int cmd_id, std::istringstream& iss, std::string& error_msg);	//�رտ��Ӣ��¼
	bool CmdToSetKuaFuYingXiongLuLockFlag(int cmd_id, std::istringstream& iss, std::string& error_msg);	//���ÿ��Ӣ��¼��״̬
	bool CmdMallocTrim(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdKill(int cmd_id, std::istringstream& iss, std::string& error_msg);
	bool CmdSyncInfoToHidden(int cmd_id, std::istringstream& iss, std::string& error_msg);			//ͬ����Ϣ�����

	time_t m_next_get_public_notice;			// �´����󹫸�ʱ��
	time_t m_next_get_cmd;						// �´���������ʱ��

	PublicNoticeList::NoticeAttr m_normal_noticelist[PublicNoticeList::MAX_PUBLICNOTICE_NUM];	// ��������һ������

	NormalPublicNotice m_normal_notice;			// ��Ϸ������������
	NormalPublicNoticeMap m_hidden_notice_map;	// ���ط�����������

	unsigned int m_next_check_timeout_log_time;
	std::deque<TimeOutLogInfo> m_timeout_log_info_queue;

	DBCommandHandlerMap m_cmd_handler_map;
};

#endif  // __SYSTEM_HPP__

