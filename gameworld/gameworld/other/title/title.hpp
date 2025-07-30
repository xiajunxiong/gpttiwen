#ifndef __TITLE_HPP__
#define __TITLE_HPP__

#include "servercommon/titledef.hpp"
#include "other/attributelist.hpp"
#include <set>


enum TITLE_NOTICE_TYPE
{
	TITLE_NOTICE_TYPE_DEFAULT,
	TITLE_NOTICE_TYPE_NO_NOTICE,		//����ʾ����
};

class RoleModuleManager;
class Title
{
public:
	Title();
	~Title();
	
	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager *role_module_manager, const TitleParam &param);
	void GetOtherInitParam(TitleParam *param);
	void OnRoleLogin();

	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);

	void Update(unsigned long interval, time_t now_second, unsigned int now_dayid);

	int AddTitle(int title_id, const char * reason, unsigned int due_time = 0, int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	int AddTitleOrRenewDueTime(int title_id, const char * reason, unsigned int due_time = 0, int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	int AddSameSourceTypeTitle(int title_id,int notice_type = TITLE_NOTICE_TYPE_DEFAULT); //�����ͬ��Դ�ƺ� ,�Ḳ����ͬ��Դ�ƺ�
	void MoveTitle(int title_index, const char* reason, int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	bool RemoveTitle(int title_id, const char* reason, int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	bool RemoveTitleBySrcType(int src_type, const char * reason, int notice_type, std::set<int> & exclude_title_id);

	void CreateRoleAddTitle();				//��Ҹմ�����ɫ,����ְҵ�ƺ�

	int OnWearTitle(int index, bool is_face_wear = false, bool is_duel = false);
	bool OnWearTitleById(int title_id, bool is_face_wear = false);
	void OnRoleChangeProf(int old_prof, int new_prof);
	 
	bool HasTitle(int title_id);
	int GetWearTitleIndex() { return m_param.wear_index; }
	int GetWearTitle();
	void SendAllInfo();
	void SendOtherInfo();
	void SendSingleInfo(int seq, int notice_type = 0);
	void SendTitleNotice(int seq,int notice = 0);

	void CalChiefElectionTitle(int last_election = 0, int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	void CalPeakBattleTitle(int last_match_round = 0, int notice_type = TITLE_NOTICE_TYPE_DEFAULT); //last_match_round : �ϴ��Ǽ�ǿ��������ȡ����ɾ���ϴγƺ�
	void CalColosseumTitle(int notice_type = TITLE_NOTICE_TYPE_DEFAULT);		//������񱦿�ƺ�
	void CalOnlyFightTitle(int notice_type = TITLE_NOTICE_TYPE_DEFAULT);		//������һ�аٳƺ�
	void CalGuildFightTitle(int notice_type = TITLE_NOTICE_TYPE_DEFAULT);		//���������Ҷ��ƺ�
	void CalBraveGroundTitle(int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	void CalHuanJieZhanChangeTitle(int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	void CalGuildHonorTitle2(int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	void CalQingYuanTitle(int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	void CalWorldTeamArenaTitle(int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	void CalProfessionTitle(int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	void CalGuildMemberTitle(int notice_type = TITLE_NOTICE_TYPE_DEFAULT);
	bool IsAutoWear();
	void SetAutoWear(bool is_auto_wear);

	const TitleData *GetWearTitleInfo();
	void SendTitleChangeToScene();

	void GmPrintAllTitle();
	
private:
	TitleData* MutableTitleData(int title_id);

	RoleModuleManager *m_role_module_mgr;

	bool m_is_auto_wear;

	int m_title_count;
	TitleParam m_param;
	AttributeList m_attrs_add;
};

#endif // !__TITLE_HPP__