#ifndef __HUASHEN_MANAGER_HPP__
#define __HUASHEN_MANAGER_HPP__

#include "servercommon/servercommon.h"
#include "servercommon/huashendef.hpp"
#include "item/itemgriddata.h"
#include "servercommon/struct/itemlistparam.h"
#include "other/skill/roleskill.hpp"
#include "other/attributelist.hpp"
#include "protocol/msghuanjiezhanchang.h"
#include "other/pet/pet.hpp"
#include "other/medal/medal.hpp"
#include <set>
#include "servercommon/rankdef.hpp"
#include "global/activity/activityimpl/activityhuanjiezhanchang.hpp"

using namespace ItemNamespace;

enum HUASHEN_KNAPSACK_OP_TYPE
{
	HUASHEN_KNAPSACK_OP_TYPE_UPDATE = 0,		//���±�������
	HUASHEN_KNAPSACK_OP_TYPE_ADD    = 1,		//��ӵ���
	HUASHEN_KNAPSACK_OP_TYPE_CONSUME = 2,		//���ĵ���
	HUASHEN_KNAPSACK_OP_TYPE_KEY_SYN = 3,		//һ��ͬ��

	HUASHEN_KNAPSACK_OP_TYPE_MAX,
};
UNSTD_STATIC_CHECK(HUASHEN_KNAPSACK_OP_TYPE_MAX == 4);

//
static const int HUASHEN_GRID_INDEX_TYPE_BASE = 1000;		
enum HUASHEN_GRID_INDEX_TYPE
{
	HUASHEN_GRID_INDEX_TYPE_KNAPSACK = 0,		//����
	HUASHEN_GRID_INDEX_TYPE_EQUIPMENT = 1,		//װ��
	HUASHEN_GRID_INDEX_TYPE_JEWELRY = 2,		//����
	HUASHEN_GRID_INDEX_TYPE_CRYSTAL = 3,		//Ԫ��ˮ��
	HUASHEN_GRID_INDEX_TYPE_MEDAL = 4,			//ѫ��
	HUASHEN_GRID_INDEX_TYPE_PET = 5,			//����
	HUASHEN_GRID_INDEX_TYPE_PET_EQUIPMENT= 6,	//����װ��
};

class RoleModuleManager;
class HuaShenManager
{
public:
	HuaShenManager();
	~HuaShenManager();

	void* operator new(size_t c);
	void operator delete(void* m);

	void Init(RoleModuleManager* module_mgr, int param_list_num, const ItemListParam::ItemListData *param_data_list[], const HuaShenParam & param, 
				int pet_param_list_num, const ItemListParam::ItemListData *pet_param_data_list[], int medal_param_list_num, const ItemListParam::ItemListData *medal_param_data_list[]);	
	void GetInitParam(HuaShenParam * param);
	void InitEnd();

	void Update(time_t now_second);
	
	void OnSeasonFirstReset();			//������ѡ��ս��������һЩ��Ϣ��ͬ����Ϣ
	void OnSeasonEndReset(bool is_reset_dingji_flag = true);			//����������������

	void OnMonthChange();				
	void OnEnterKeySynchronize(bool is_notice = true);		//����ý�ս��һ��ͬ������
	void OnSynKnapackData(bool is_element_crystal = false);	//������湺����ߺ�ͬ������
	bool IsSeasonInTime();				//����Ƿ��ѻ�û���
	void OnRoleLogin();					
	void SetRoleInfo(int rank_score, int is_finis_ding_ji);	
	int GetRankScore() { return m_param.rank_score; }
	QueryHuanJieInfo GetQueryInfo();

	void ReCalcAttr(int recalc_reason, const char* recalc_reason_str, bool is_init = false);
	void SendRoleAttrReasonList(int param1);					//�·������ɫ������Դ�б�
	int GetPetAttrReasonList(int pet_index);					//��ó���������Դ�б�
	void SendPetAttrReasonList(int pet_index);					//�·�����������Դ�б�
	void OnRoleChangeProf();		//��ɫתְ,����ϴ��

	int GetHuaShenLevel() { return m_param.max_level; }

	bool SynBenZunItemData(ItemDataWrapper * item_grid_data, int op_type = HUASHEN_KNAPSACK_OP_TYPE_UPDATE, bool is_notice = false);		// ͬ�����������Ϣ
	bool SynBenZunItemData(ItemGridData * item_grid_data, int op_type = HUASHEN_KNAPSACK_OP_TYPE_UPDATE, bool is_notice = false);			// ͬ�����������Ϣ
	bool IsCheckItemIdentical(long long item_unique_id, int key_index);

	//����������(���ϻ���ĵ��߶��ǲ��ɶѵ�)
	bool ConsumeItem(long long item_unique_id, bool is_notice = false);							
	bool ConsumeItem(int key_index, bool is_notice = false, bool is_key_syn_chronize = false);
	bool InOrder(bool ignore_time_interval = false);											// ������
	bool Put(ItemDataWrapper & item_grid_data, bool is_notice = false, int *out_index = NULL);	// �����Ʒ	���������������̶��ȱ�����������100,�˴��Ͳ�����ⱳ��
	bool Consume(int index, bool is_notice = false);											// ���ĵ���
	bool GetItemGridData(int index, ItemGridData* item_grid_data);								// ��ȡ����������Ϣ
	const ItemGridData * GetItemGridData2(int index) const;										// ��ȡ����������Ϣ
	void GetChangeItemList(int *count, ItemListParam::ItemListData *save_list);					// ���汳����Ϣ
	void ClearDirtyMark();
	void SetItemDirty(int index, bool to_notice_client, short reason_type, short put_reason = 0);
	void SendKnapsackInfo(short reason = Protocol::SCHuaShenKnapsackInfoAck::HUASHEN_SEND_REASON_NORMAL);
	void SendKnapsackSingleItemChange(int index, short reason_type, short put_reason = 0);
	
	//����װ������(�߼��ڱ���װ��ģ��)
	bool PutOn(short index, long long unique_id, int grid_index_type);			//��׼���������Ʒ���hashӳ��   �����ĵ���,�����ӳ��
	bool TakeOff(long long unique_id);											//ɾ��֮ǰװ��ӳ��			   ��ɾ��ӳ��,����ӵ���

	//����������
	void SynBenZunAllPetData();																			//������ʼͬ�����������������
	bool PetDataUpdate(int pet_index, ItemDataWrapper * item_data);										//�������ݸ���
	bool PetEquipmentUpdate(int pet_index, ItemDataWrapper * item_data);								//����װ�����ݸ���
	bool PetEquipmentReset(int pet_index, bool is_notice = false);										//����װ������ 
	bool AddPet(ItemDataWrapper & pet_data);															//��ӳ���
	bool DeletePet(int pet_index, bool is_notice = false, bool is_key_syn_chronize = false);			//ɾ������
	bool SetPetState(int pet_index, int state);															//���ó���״̬	
	bool AssignAttributePoints(int pet_index, short con, short str, short def, short agi, short magic);	//�������Ե�		
	bool OnRefreshPoints(int pet_index, bool is_notice = true);											//ϴ��		
	bool PutOnPetEquipment(int pet_index, int equipment_bag_index);										//��������װ��
	bool PutOffPetEquipment(int pet_index);																//���³���װ��
	void AddPetBendiction(long long vice_item_unique_id);												//��ӳ���ף��
	//void OnPetUp(int pet_index, int cur_level);														//��������
	//void OnPetIntensi(int pet_index, int pet_level);													//����ǿ��
	//void OnPetChangeName(int pet_index, GameName name);												//�������
	//void OnUseSkillBook(int pet_index, int replace_gift_index, int skill_id);							//����ʹ�ü�����
	//void OnPetChangeTalent(int pet_index, PetParam * pet_param);										//����ϴ���츳����
	//void OnComprehendPassiveSkill(int pet_index, PetParam * pet_param, int f_index);					//����������
	bool OnPetGridInfoReset(int pet_index);																//�����������Ϣ����
	void ReCalcPetAttr(int pet_index);																	//�����������
	int GetPetAttributeList(int pet_index, AttributeList& attr);										//��ó��������б�
	const AttributeList * GetPetAttrList(long long item_unique_id);										//��ȡ���������б�
	int GetPetStrBuffLevel(int pet_index);																//���ڹ���(���ش�ʱ���ǿ���ȼ�,���жϸó���״̬)
	bool IsWaitForFightPet(int pet_index);																//�жϳ����Ƿ�Ϊ��Ϣ״̬
	int GetPetCapability(int pet_index);																//���ĳ�������ս��
	int GetPetSkillCapability(int pet_index);															//��ȡ���＼�ܵ�ս��
	static int ReCalcHuaShenPetCapability(const Attribute* attrs);										//��ȡ�������Ե�ս��
	bool PetInvalid(int index);
	void GetChangePetItemList(int *count, ItemListParam::ItemListData *save_list);						//�����������
	void SetPetItemDirty(int index, bool to_notice_client = true, int dirty_type = 0);
	void ClearPetDirtyMark();
	void SendPetAllInfo(int reason = Protocol::SCHuaShenSendPetAllInfo::HUASHEN_PET_ALL_INFO_REASON_DEFAULT);
	void SendPetSingleInfo(int pet_index, int notie_type = Protocol::SCHuaShenSendPetSingleInfo::PET_NOTIE_TYPE_NO_TYPE);
	void SendPetOtherInfo();
	bool SendPetAttrInfo(int pet_index);

	bool GetBattleCharacterData(int pet_index, PetBattleData* out_data, bool is_use_cur_hp_mp);			//��ó���ս������
	PetEffectSkillId GetPetEffectiveSkill(int pet_index);												//��ȡ������Ч�ļ���id��������ͬ����Ч���ߵļ���id

	//������������
	const AttributeList & ReCalcTrainAttr(AttributeList &base_add, bool is_recalc);
	const AttributeList & ReCalcTrainAttrPer(AttributeList &base_add, const AttributeList & cpy_attr_list);
	void SynBenZunTrain();							//ͬ����������������������
	void OnTrainUp(int type, int skill_level);		//��������������������
	void OnPetTrainUp(int skill_level);				//�������������������

	//���������� (��������ʱ����)
//	void ReCalcFaBaoAttr(AttributeList &base_add, bool is_recalc);
//	void FaBaoPutOn(int equip_bar_index, int fabao_index);
//	void FabaoTakeOff(int equip_bar_index);

	//������ײ���
	const AttributeList & ReCalcAdvAttr(AttributeList &base_add, bool is_recalc);
	void CalcMaxAdvTimes();					//�����ʱ����Ľ��׵ȼ�

	//�����ܲ���
	void OnLoginSynBenZunSkillInfo(RoleSkill::SkillItem * skill_list);				//��ҵ�½��ʼ��ʱͬ��������Ϣ
	RoleSkill::SkillItem* GetSkill(short skill_index);
	void AddSkill(RoleSkill::SkillItem * skill_item);								//����ѧϰ�¼���
	void SkillUp(int skill_id, int skill_level);									//����ѧϰ��������
	const AttributeList & ReCalcSkillAttr(AttributeList &base_add, bool is_recalc);

	//����Ǳ�ܲ���
	void AddAttributePointReq(short vitality, short strength, short power, short speed, short magic, int cur_plan_index);	//�������Ե�
	void ClearAttributePointReq(int cur_plan_index);																		//ϴ��
	void SwitchAttrPlan(int plan_index);																					//�л����Է���
	void AttrPlanChangeName(int plan_index, const char * change_name);														//��������
	const AttributeList & ReCalcAttrPlan(AttributeList &base_add, bool is_recalc);
	void SendPlanInfo(int cur_index);
	void SendPlanListInfo();

	//����ѫ�²���
	const AttributeList & ReCalcMedalAttr(AttributeList &base_add, bool is_recalc);
	void MedalPutOn(int grid_index,int index);												//����ѫ��
	bool MedalTakeOff(int grid_index, bool is_notice = true);								//����ѫ��
	bool MedalUpdate(int grid_index, ItemDataWrapper * item_data);							//����ѫ��
	bool MedalReset(int grid_index, bool is_notice);										//����ѫ����		
	void GetChangeMedalItemList(int *count, ItemListParam::ItemListData *save_list);		//����ѫ������
	int GetMedalSpeScore();																	//��ȡ������Ч����
	void GetMedalBattleData(RoleBattleData * role_data);									//�ռ�����ѫ��ս������
	std::set<int> GetEffectSp(std::set<MedalSpPriority> *sp_list = NULL);					//��ȡ�������ѫ���б�					
	void SynBenZunMedalGrid(const ItemDataWrapper& item, int medal_index);
	void ClearMedalDirtyMark();	
	void SendMedalInfo();

	void OnSetBattlePosition(int position);
	void SendBattlePosition();

	void OnInfoReq(Protocol::CSHuaShenInfoReq  req);	
	void SendAllHuaShenInfo();			//���͸������йص���Ϣ
	void SendRoleInfo();				//�·����������Ϣ
	void SendRoleAttrList();			//�·����������б�
	void SendSeasonInfo();				//�·�������Ϣ


	//��װս������
	void CollectRoleBattleData(RoleBattleData* out_data, bool need_partner,bool need_pet);
	bool GetRoleBattleData(BattleCharacterData* out_data, bool is_use_cur_hp_mp);
	bool GetPetBattleData(short max_pet_num, short* out_pet_num, PetBattleData* out_data_list,
		short* out_fight_pet_idx, bool is_use_cur_hp_mp);

	void GetPersonRankInfo(RoleBaseInfo & _role_info);
	void GetPetRankInfo(int _pet_index , RoleBaseInfo & _role_info, PetPro & _pet_param , AttributeList & _pet_attr);
private:
	void RankUpdate();

	void ClearGrid(int index);
	void RefreshValidGridIndexEnd(int change_index);
	void OnDeletePetAddPetEquipDelayHandle(int pet_index);		//һ��ͬ��ʱ��ɾ���ĳ����ʱ�������ų���װ��
	void CheckAllItemIdentical();			//����ʱ�����뻯������װ���Ƿ�һ��,��һ������д���
	void SeasonReset();						//��������
	void ClearAllItemData();				//������е���

	void Reset()
	{
		memset(m_role_attr_reason_list, 0, sizeof(m_role_attr_reason_list));
		memset(m_pet_attr_reason_list, 0, sizeof(m_pet_attr_reason_list));
		m_attr_list.Reset();
		m_attrs_plan_add.Reset();
		m_attrs_medal_add.Reset();
	//	m_attrs_fabao_add.Reset();
		m_attrs_train_add.Reset();
		m_attrs_train_per.Reset();
		m_base_attr_prof.Reset();
		m_attrs_skill_add.Reset();
		m_pet_count = 0;
		for (int i = 0; i < MAX_HUASHEN_PET_GRID_NUM; i++)
		{
			m_pet_base_attr[i].Reset();
			m_pet_attr_no_skill[i].Reset();
			m_pet_grid_list[i].Clear();
			m_pet_old_state[i] = false;
			m_pet_dirty_mark[i] = false;
		}
		m_pet_grid_is_change = false;

		for (int i = 0; i < MAX_MEDAL_WEAR_GRID; i++)
		{
			m_medal_old_state[i] = false;
			m_medal_grid_list[i].Clear();
		}

		m_valid_grid_index_end = 0;
		m_knapsack_last_inorder_time = 0;
		memset(m_old_state, 0, sizeof(m_old_state));
		memset(m_dirty_mark, 0, sizeof(m_dirty_mark));
		m_knapsack_is_change = false;
		memset(m_grid_list, 0, sizeof(m_grid_list));

		m_item_map.clear();
		m_delay_handle_map.clear();
		for (int i = 0; i < MAX_SKILL_NUM; i++)
		{
			m_skill_list[i].Reset();
		}

		max_role_train_level = 0;
		max_pet_train_level = 0;
		max_advance_times = 0;
		role_advance_times = 0;
		m_pet_has_skin_id.clear();
		m_next_rank_update_time = 0;
	}

	RoleModuleManager *m_module_mgr;
	HuaShenParam m_param;

	AttributeList m_role_attr_reason_list[MAX_REASON_TYPE_LIST_NUM];	//�����ɫ������Դ�б�
	AttributeList m_pet_attr_reason_list[MAX_REASON_TYPE_LIST_NUM];		//�������������Դ�б� ʹ��֮ǰ ��Ҫ�ȵ���GetPetAttrReasonList�ӿ�

	AttributeList m_attr_list;					//����������
	AttributeList m_attrs_plan_add;				//����Ǳ�ܵ����ӵ�����
	AttributeList m_attrs_medal_add;			//����ѫ������
	//AttributeList m_attrs_fabao_add;			//���������������
	AttributeList m_attrs_train_add;			//������������
	AttributeList m_attrs_train_per;			//������������
	AttributeList m_base_attr_prof;				//�����������
	AttributeList m_attrs_skill_add;			//��������������

	int			m_pet_count;
	//����������(����������)
	AttributeList	m_pet_base_attr[MAX_HUASHEN_PET_GRID_NUM];
	//����������(������������)
	AttributeList	m_pet_attr_no_skill[MAX_HUASHEN_PET_GRID_NUM];
	ItemGridData	m_pet_grid_list[MAX_HUASHEN_PET_GRID_NUM];
	bool			m_pet_old_state[MAX_HUASHEN_PET_GRID_NUM];
	bool			m_pet_dirty_mark[MAX_HUASHEN_PET_GRID_NUM];
	bool			m_pet_grid_is_change;		// ������Ϣ�Ƿ����ı�

	//ѫ��
	bool			m_medal_old_state[MAX_MEDAL_WEAR_GRID];
	ItemGridData	m_medal_grid_list[MAX_MEDAL_WEAR_GRID];	

	short m_valid_grid_index_end;				// ��ǰ�����зǿո��ӵ�����һ���������
	short m_curr_valid_grid_num;				// ��ǰ��������������Ч����
	UInt32 m_knapsack_last_inorder_time;		// �ϴα�������ʱ��

	bool m_old_state[HUASHEN_ITEM_GRID_NUM_MAX];
	bool m_dirty_mark[HUASHEN_ITEM_GRID_NUM_MAX];
	bool m_knapsack_is_change;
	
	ItemGridData* m_grid_list[HUASHEN_ITEM_GRID_NUM_MAX];		//��ʱ��newһ���µ���Ʒ����(����������Ʒ����),������ָ���𱳰�����Ʒ��ַ 
	std::map<long long, int> m_item_map;						//��unique_idΪkey, secondΪHUASHEN_GRID_INDEX_TYPE * HUASHEN_GRID_INDEX_TYPE_BASE + index
	std::map<long long, ItemDataWrapper> m_delay_handle_map;	//�ӳٵ��ߴ���

	RoleSkill::SkillItem m_skill_list[MAX_SKILL_NUM];		//������Ϣ

	int max_role_train_level;					//���������������ȼ�
	int max_pet_train_level;					//���������������ȼ�

	int max_advance_times;						//���ϻ���ȼ����Ƶ������״���[0,6]
	int role_advance_times;						//��ǰ����Ľ��״���

	std::map<int, std::vector<int> >  m_pet_has_skin_id;

	unsigned int m_next_rank_update_time;				
};


#endif
