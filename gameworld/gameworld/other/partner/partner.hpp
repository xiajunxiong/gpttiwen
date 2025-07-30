#ifndef __PARTNER_HPP__
#define __PARTNER_HPP__

#include "servercommon/partnerdef.h"
#include "other/attributelist.hpp"
#include "protocol/msgpartner.h"
#include "servercommon/rolepetpartnerdef.hpp"

#include <vector>
#include <set>

struct AttrCon
{
	AttrCon() { this->Reset(); }

	void Reset()
	{
		capability = 0;
		memset(attr_list, 0, sizeof(attr_list));
	}

	int capability;		// ս����
	Attribute attr_list[BATTLE_ATTR_MAX];		//������
};

//Ԫ����Լ��Ϣ
class PartnerPactInfo
{
	//pact		��Լ   ���������������
	//bepact	����Լ ����Լ������������
	//
	static const int PER_MIL_BASE = 1000;
public:
	PartnerPactInfo();
	~PartnerPactInfo();

	//--����--��������  ��������
	//----����:��ǰ������ ���������������)
	//----����:�¸�������
	void ReCalcPactAttr(AttrCon &total_attr, Attribute attach_attr_list[PARTNER_ATTR_MAX]);
	void AddNowPactTo(AttrCon &total_attr);

	//�����Լ����
	void ReSetPact();
	//�Ƴ���Լ����
	void PactAttrOff(AttrCon &total_attr);

	//������Լ��������
	void SetBePactBase(Attribute base_attr_list[PARTNER_ATTR_MAX]);
	//�����Լ�������Ե� (������, ǧ�ֱ�)
	void AddBePactTo(Attribute attach_attr_list[PARTNER_ATTR_MAX], int attr_per_mil);

	//����Լ
	bool IsPact(int partner_id);
	//��Լ�»��
	bool PactAdd(int partner_id);
	//�����Լ
	void PactOff(int partner_id);

	//�����������Լ
	void BePactAdd(int partner_id);
	//�������������Լ
	void BePactOff(int partner_id);

	const std::set<int> &GetNowPactSet() const { return m_now_pact_partner_id_set; }
	const std::set<int> &GetBePactSet() const { return m_be_pact_partner_id_set; }

	void GetNowPactAttr(AttrCon &now_pact_attr);
	void GetNowPactAttr(AttributeList &now_pact_attr);
	void GetBePactBaseAttr(Attribute base_attr_list[PARTNER_ATTR_MAX]);


private:
	//��ǰװ����Լ��<���id>
	std::set<int> m_now_pact_partner_id_set;
	//�û���ʱ�����ӵ���Լ����(�Ѿ����ӵ�AttrCon)
	Attribute m_now_pact_attr_list[PARTNER_ATTR_MAX];

	//װ���˸û����Լ��<���id>
	std::set<int> m_be_pact_partner_id_set;
	//�û�����ڼ��������ԼĿ�����ӵ�����(100%)
	Attribute m_be_pact_base_attr_list[PARTNER_ATTR_MAX];
};

class RoleModuleManager;
struct PlatformBattlePartner;
struct BattleCharacterData;
struct QualityUpgradeCfg;
struct PartnerBasicCfg;
struct PartnerUseElementCrytalFightInfo;
struct PartnerSkinCfgInfo;

class Partner
{
public:
	Partner();
	~Partner();

	void* operator new(size_t c);
	void operator delete(void* m);

	RoleModuleManager * GetRoleModuleManager() { return m_role_module_mgr; }

	void Init(RoleModuleManager * role_module_manager,
		const PartnerParam & p_partner,
		const NewPartnerParam & new_p_partner,
		const PartnerGiftTreeParam & partner_gift_tree,
		const PartnerSpiritPactParam & partner_pact_param
		);
	void InitEnd();
	void GetPartnerParam(PartnerParam * p_partner_param,
		NewPartnerParam * new_p_partner_param,
		PartnerGiftTreeParam * partner_gift_tree_param,
		PartnerSpiritPactParam * partner_pact_param);
	NewPartnerParam GetPartnerParam() { return m_new_partner; }
	std::vector<int> GetActiveParnerList();

	void GetRolePartnerInfoParam(RolePartnerInfoParam * role_partner_info_param);

	bool IsPartnerValid(int partner_id) const;		// ��id�Ƿ���Ч
	bool IsPartnerInvalid(int partner_id) const;		// ��id�Ƿ���Ч

	void Update(time_t now_second);
	void OnPartnerDayChange(unsigned int old_dayid, unsigned int new_dayid);
	void OnRoleLogin();
	const AttributeList & ReCalcAttr(AttributeList &base_add, bool is_recalc);
	void CheckPartnerCollection();


	bool IsPartnerAlreadyActive(int partner_id);								// ��ѯ����Ƿ񼤻�
	int GetCurFightPartnerNum();
	void ActivePartnerByConsumingItem(int partner_id);							// �õ��߼�����
	bool ActiveAppointedPartner(int param, int unlock_type, int *activity_partner_id = NULL, bool is_notice = true);	// ����ָ���Ļ��
	bool ActivePartnerByOther(int partner_id, const char * reason);				// ������ģ�鼤����
	void SendAllPartnerInfo();													// �������л�����Ϣ
	void SendSynfightInfoList();												// ���ͻ���ս��Ϣ�б�
	void ChangePartnerFightState(int partner_id, const char * reason);			// �ı����ս״̬
	void PartnerBreakTAndUpGrade(int partner_id);								// �������ͻ�ƽ��ף�����
	void PartnerFettersBaptize(int partner_id);									// �����Եϴ��				// [ע��]
	void PartnerFettersResultSave(int partner_id);								// ��������Եϴ�����		// [ע��]
	void PartnerSkillSeletorSave(int partner_id, int slot, int skill);			// �ı���Я������			// [ע��]
	void PartnerSynFightListAlter(int insert_id, int target_id);				// ��������ս�б�
	void PartnerSwitchFightPost(int partnerA, int partnerB);					// �����������ĳ�ս˳��
	void PartnerSkillActivate(int partner_id, short is_activate, short skill_idx); // ��鼤���

	void MaxPartnerHPOrMP(int partner_id, int max_type, bool is_notify = true);	// ˢ����ǰ HP �� MP
	bool AlterPartnerHPOrMPValue(int partner_id, int r_type, int r_value, bool is_percent = false);		// ����ֵ�䶯ָ������Ѫħ��������Ѫҩ��ҩ�������¼��ȣ�value ��Ϊ�ָ�����Ϊ����
	void AlterPartnerHPOrMPPerce(int partner_id, int r_type, double r_value);	// �������䶯ָ������Ѫħ��value Ϊ ����\ħ������ ռ�ȣ�
	void SetPartnerHPOrMPValue(int partner_id, int cur_hp, int cur_mp);
	void AddAllPartnerHpMp(double percent);
	void AddFightPartnerHpMp(double percent);
	void SyncPartnerInfoToTeam(int partner_id, int cur_hp, int max_hp, int cur_mp, int max_mp, int skin_id, int level);

	Attribute GetAttrByPartnerID(int partner_id, int attr_type);				// ��ȡ��鵱ǰ��������
	Attribute* GetAttrListByPartnerID(int partner_id);
	const PartnerSql* GetPartnerDataById(int partner_id);
	const NewPartnerBaseInfo* GetPartnerBaseInfoById(int partner_id);
	int GetPartnerAttrListReason(int partner_id);								// ��ȡ���������Դ�б�
	void SendPartnerAttrListReason(short param1);								// �·����������Դ�б�
	void SendPartnerSingleInfo(int partner_id);									// �·����������Ϣ

	//��鴫��
	void OnOpenBiography(int partner_id, int biography_id);						// �򿪴���
	void OnFetchBiography(int partner_id, int biography_id);					// ��ȡ���ǽ���

	void GetSynFightPartnerIdList(int max_out_num, int * out_list, int * out_num);// ��ȡ��ս�Ļ���б�
	std::vector<int> GetSynFightPartnerIdList2() const;
	std::set<int> GetSynFightPartnerIdList3() const;
	bool GetBattleCharacterData(int partner_id, PartnerBattleData * out_data, int * fight_list, int fight_num, bool is_use_cur_hp_mp);// ��ȡ��Ӧ���Ļ�������

	int GetPartnerCurCount();										// ��ȡ�Ѿ�����Ļ������
	int GetPartnerCurCount(int quality);							// ��ôﵽ�Ƚ׵Ļ������
	int GetMaxParnterGrade();										// ��õ�ǰ�������ߵĽ��׵ȼ�
	int GetMaxPartnerNumByGrade(int grade);							// ��ôﵽ�ý��׵ȼ��Ļ������
	int GetActiveJiBanNum() { return m_partner_jiban_num; }			// ��ô�ʱ�����Ļ������
	int GetWearSetEquipNum();										// ��ô���һ�����񿨵Ļ������(�������Ƿ���ͬ��װЧ��)
	int GetPartnerNumByQualityNum(int quailty_num);					// ��ôﵽ����(��Ʒ)�����Ļ������(�����ӻ�鱾���ʼƷ�ʿ�ʼ����)

	void ReCalAllPartnerAttr(int notice_type);		// ��ҵȼ�����������������
	int GetMaxFourPartnerCap();	// ��ȡ4��������ս���ܺ�
	int GetFightParatnerCap();	// ��ȡ��ǰ��ս���ս���ܺ�

	void OnRoleGuildInbornChange();

	void SyncFightListFromFormation(ARG_IN int* fight_partner_list, ARG_IN int* battle_row_list, int max_num);

	//----------------------------����----------------------------
	int UpLevel(int partner_id, int num);
	int ResetLevel(int partner_id);

	/************************************************************************/
	/*                               �����Ʒ                               */
	/************************************************************************/
	int GetCurrentQuality(int partner_id);																					// ��õ�ǰ����Ʒ��
	bool QualityUpgrade(int partner_id, int upgrade_type, bool is_p = true/*�Ƿ�Ҫ�����ж��ܲ�����Ʒ*/);						// �����Ʒ
	void AddAttrByQualityUp(AttrCon &_p_att, const QualityUpgradeCfg *_q_cfg);												// ��������
	void AddAttrByQualityUp(AttributeList &_p_att, const QualityUpgradeCfg *_q_cfg);										// ��������
	void AddQualityByQualityUp(int partner_id);																				// ����Ʒ��
	bool CanQualityUpgrade(int _partner_id, int _upgrade_type, int quality_segment);										// �Ƿ��е�ǰ�ף��Ƿ������Ʒ
	void SendQualityUpInfo(int _partner_id, int _upgrade_type, char notice_type);								// ��Ʒ��Ϣ�·�

	// -------------------------------------------------------------- ���װ�� -------------------------------------------------------------- //
	void PartnerEqUnlockEquipment(int partnet_id, int slot_id);					// ����
	void PartnerEqPullOnEquipment(int partner_id, int grid_id);					// ����װ��
	void PartnerEqGetOffEquipment(int partner_id, int slot_id);					// ����װ��
	void PartnerEqResolveEquipment(int grid_idx);								// �ֽ�װ��

	/************************************************************************/
	/*                               Ԫ��ˮ��                               */
	/************************************************************************/
	void UseElementCrystalById(int _partner_id,ItemID _crystal_id);														// ʹ�û��ˮ��
	void UseElementCrystal(int _partner_id, int index /* ��Ʒ�����*/, bool is_send = true);							// ʹ�û��ˮ��
	void UseElementCrystalToFight(int index_1, int index_2, int index_3, int index_4);									// ��ս���ʹ�û��ˮ��
	void RemoveElementCrystal(int _partner_id, bool is_send = true, bool is_put_knaspack = true);														// ж��Ԫ��ˮ��
	void RemoveAllCurPartnerElementCrystal();																			// һ��ж�����м������Ԫ��ˮ��
	void ElementCrystalCountSubByPartnerId(int partner_id);																// �û���ˮ������-1
	void SendElementCrystalInfo();																						// ���ˮ����Ϣ�·�
	int ClearMixedElementCrystal();				//�������������õĻ��Ԫ��ˮ�����Զ����գ�����Ҫ����ÿ�����Ԫ��ˮ��ת��Ϊ5000ͭ��ͨ���ʼ����� ;  ��Ҫ���ĵط��б���,�ֿ�,��ɫ�������ˮ��,�Ѽ������������ˮ��

	void GMSetCountSub(int _partner_id,int _count);																		// GM�������ˮ������			
	

	/************************************************************************/
	/*                              ���Ƥ��                                */
	/************************************************************************/
	bool ActiveSkin(int partner_id,short skin_id);																						// ���߼���Ƥ��
	bool AutoActiveSkin(ItemID active_id, short num);																					// �Զ�����	
	void AttrChangeBySkin(AttrCon &p_att, const PartnerSkinCfgInfo *skin_att, char add_or_sub /* 1- ��������  0-�������� */);				// ��������
	void AttrChangeBySkin(AttributeList &p_att, const PartnerSkinCfgInfo *skin_att);													// ��������
	void PutOnSkin(int partner_id, int skin_id, bool is_from_init = false);																// ����Ƥ��
	void PutOffSkin(int partner_id, int skin_id, bool is_from_init = false);															// ж��Ƥ��
	int GetSkinCount(int partner_id);																									// ��õ�ǰ���ӵ��Ƥ������
	bool IsActive(int partner_id,int skin_sq);																							// ��Ƥ��ID�Ƿ�ӵ��
	void GetActiveSkinId(int partner_id,int ARG_OUT skin_list[PARTNER_SKIN_ONCE]);														// ��ȡ��ǰ���ӵ��Ƥ��
	int GetCurPutOnSkin(int partner_id);																								// ��ȡ��ǰ���ڴ�����Ƥ��
	bool IsPutOn(int partner_id, int skin_id);																							// ��Ƥ���Ƿ����ڴ���
	void SendSingleInfo(int partner_id, int skin_id,int reson);																			// ������Ϣ�·�
	void SendAllSkinInfo();																												// �·���Ƥ����������

	//------------------------------------------
	int GetParterCapability(int partner_id);
	void GetPartnerList(std::vector<int> * role_partner_vec);//��ȡ���ӵ�л���id
	void GetUnlockedPartnerSet(std::set<int>* unlocked_set);//��ȡ���ӵ�л���id
															 
	//-------------------��̨ս����
	bool IsInPlatformBattle(int partner_id);
	bool CanJoinPlatformBattle(int partner_id);
	void ClearPartnerPlatformBattleState();
	void SetPartnerPlatformBattleState(int partner_id, bool is_platform_battle);
	void SyncPartnerDataPlatformBattle();				//������Է����仯ͬ�����ݵ���̨ս
	bool GetPlatformBattleCharacterData(int partner_id, PartnerBattleData ARG_OUT * out_data, PlatformBattlePartner ARG_OUT & out_partner, std::set<int> fight_list);// ��ȡ��Ӧ���Ļ�������


	void OnPartnerActive2(int partner_id);				//����鼤��ʱ

	//�����̵�
	void OnStoreBuy(int seq);
	void OnStoreRefresh(bool is_auto);
	void SendStoreInfo();
	unsigned int CalculateNextFreeRefreshTime();
	bool IsFreeRefresh();

	//���о�ϳ�
	void OnPageCompose(int item_id);
	void GmSetTranf(char tranf_id);

	//����츳ϵͳ
	void GmSetReturnFlag() { m_partner.partner_skill_tree_back_flag = 0; }
	void UpLevelGiftTree(int partner_id, int tree_point);
	void ResetGiftTree(int partner_id, int tree_point);
	void ResetTreePoint(int *round_num ,int partner_id, int tree_point, int *back_coin, std::vector<ItemConfigData>* back_item_vec, std::vector<int>* reset_point);//����ĳ���ĳ���츳��--ֻ���������
	void SendGiftTreeInfo();
	void SendSingleGiftTreeInfo(int partner_id);

	//���Ե�
	void OnAttrPelletChange(int partner_id);

	//������
	void OnPartnerSoar(int partner_id);
	void OnPartnerSoarArmsLevelUp(int partner_id);
	int GetUseMaxAttrPellet(int partner_id);
	void GetSoarArmsPassiveSkillById(int partner_id, int max_other_sp_num, int * out_other_sp_num, int * out_other_sp_list);	//�ռ�����������Ч
	void SendPartnerSoarNoitce(int notice_type, int partner_id);

	int GetPartnerSoarArmsLevel(int partner_id);
	int GetPartnerSoarLevel(int partner_id);
	// ������������
	void ReCalcPartnerAttrByFuYu(int partner_id, bool is_notify = true, char notic_type = Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_NO, char add_or_sub = ADD_ATTR);

	///////////////////////////////
	//////���--Ԫ����Լ
	///////////////////////////////
	//����ϵͳ�ֿ�
	bool PactGridPutIn(ItemID item_id, int num, int reason_type);
	//�Ƴ�ϵͳ�ֿ�(��������)
	bool PactGridTakeOut(ItemID item_id, int num, int reason_type);

	//--װ��
	bool PactPutOn(int partner_id, int put_index, int save_index);
	//--ж��
	bool PactGetOff(int partner_id, int put_index);
	//--�滻
	bool PactSwap(int off_partner_id, int off_index, int put_partner_id, int put_index);
	//--����
	void PactUpStar(int partner_id, int put_index);
	//--��Լ�ֿ�֪ͨ
	void SendPactGridList();
	void SendPactGridChange(int save_index, ItemID item_id, int item_num);
	void SendPactInfo(int partner_id, int type, short param);
	void SendPactBaseAttr(int partner_id);

	//���ս�������¼�
	void OnReCalcAttr(int partner_id, AttrCon &partner_attr, bool is_notify,
		char notic_type = Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_NO);

	//������Լ����
	//Ŀǰʹ�ø��Ӻ��Ƴ��ķ�ʽ�ı����Ժ�ս�� ����buff
	//��Ȼֱ�ӵ���������ս��Ч�ʼ��������׵�����ѭ��
	//TODO:��д ���Լ����߼�
	void ReCalcPactAttr(int partner_id, bool calc_cap = false, bool is_notify = false);

	//////////���--Ԫ����Լ
	///////////////////////////////
	///////////////////////////////

	//���ǿ��
	void OnPartnerIntensify(int partner_id, Protocol::CSPartnerIntensifyReq * req);

	// gm������
	void GmPartnerFlyUp(int partner_id, int fly_up_level, bool modify_role_flyup);

private:
	void OnPartnerActive(int partner_id);				//����鼤��ʱ
	void OnPartnerUpgrade(int partner_id);				//���������ʱ
	void OnPartnerQualityUp(int partner_id);			//�������Ʒʱ,��������
	
	void ReCalcPartnerAttr(int partner_id, bool is_notify = true, char notic_type = Protocol::SCPartnerAttrChangeRep::NOTICE_TYPE_NO, char add_or_sub = ADD_ATTR /* 1- ��������  0-�������� */);
	void CalcPartnerCapability(AttrCon &attr_con, int prof_type);		// ������ս��

	//����ս���������
	void OnPartnerReCalcEnd();



	void PartnerAttrBreakThrou(int partner_id);						// �������ͻ��
	void PartnerUPGradeHandler(int partner_id);						// ���Ƚ�����

	void SendPartnerFightState(int partner_id, bool is_show = false);	// ���ͳ�ս״̬ - ����ı�״̬��ʱ��
	void SendPartnerAttrChange(int partner_id, const std::vector<BattleAttrPair> & attr_chan_list, char notic_type = -1);	// ֪ͨ���Ըı�
	void SendPartnerFetterInfo(int partner_id);							// ֪ͨ��Ե��Ϣ
	void SendPartnerGradeBreak(int partner_id, int need_pop);			// ��������ͻ�ƺ͵Ƚ���Ϣ
	void SendPartnerSkillsInfo(int partner_id);							// ����Я���ļ���
	void SendPartnerEquipsInfo(int partner_id, int slot_id);			// ���ʹ�����װ��

private:
	bool m_already_send_all_attr;
	int m_partner_count;										// �����������
	char once_init;

	RoleModuleManager * m_role_module_mgr;

	AttributeList	m_reason_list[MAX_REASON_TYPE_LIST_NUM];	//ʹ�ø�������Դǰ���ȵ���GetPartnerAttrListReason�ӿ�
	PartnerParam m_partner;
	NewPartnerParam m_new_partner;
	AttributeList m_attrs_add;									// ��ɫ���Լ�ֵ
	AttrCon m_partner_attr_list[PARTNER_NUM_MAX];				// ������Ա�
	PartnerPactInfo m_partner_pact_list[PARTNER_NUM_MAX];		// Ԫ����Լ��Ϣ
	
	bool m_platform_battle_partner[PARTNER_NUM_MAX];			// ��̨ս�����л�� �±���partner_id

	int m_partner_jiban_num;		//�����Ļ������
	//����츳��
	PartnerGiftTreeParam m_partner_gift_tree_list;

	//���--Ԫ����Լ�ֿ�
	PartnerSpiritPactParam m_pact_grid_param;
};

#endif