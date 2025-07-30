#ifndef __EVENT_HANDER_HPP__
#define __EVENT_HANDER_HPP__

#include "gamedef.h"
#include "other/skill/roleskill.hpp"
#include "item/itemgriddata.h"
#include "item/itembase.h"
#include "servercommon/internalprotocal/battleprotocol.h"


class Character;
class Role;
class Robot;
class Team;
struct BattleData_Business;
struct BattleData_StatisticRole;
struct BattleData_Statistic;
struct CrossData_Business;
struct SynGatherDreamRecord;
class Scene;
class NPCGather;
class MonsterCfg;
#define EVENTHANDLE EventHandler::Instance()
class EventHandler
{
public:
	static EventHandler & Instance();

	void OnRoleLogin(Role* role);
	void OnRoleLogout(Role* role);
	void OnRoleChangeStatus(Role* role, int old_status, int cur_status);

	void OnRoleStartFight(Role* role, int battle_mode);
	void OnRoleFinishFight(Role* role, int battle_mode, bool is_win, bool is_flyaway, BattleData_Business& business_data, BattleRoleList battle_role_list);
	void OnRoleFinishFightDetail(Role* role, int battle_mode, bool is_win, bool is_flyaway, 
		BattleData_Business* business_data, BattleData_StatisticRole* role_statistic_data, 
		BattleData_Statistic* statistic_data, int pet_stat_num, BattleData_StatisticRole* pet_stat_list, long long * pet_unique_id_list);
	void OnRoleRunAway(Role* role, bool is_leave_team, int monster_group_id, int battle_mode);
	void OnRobotRunAway(Robot* robot, bool is_leave_team);
	void OnRoleForceFinishBattle(Role* role, int monster_group_id, int battle_mode);
	
	int GetActivityTypeByBattleMode(int battle_mode);

	void OnRoleFightWorldBoss(Role * role, bool is_flyaway, bool is_win);
	void OnRoleFightWorldBoss2(Role * role, bool is_flyaway, bool is_win);
	void OnRoleFightWorldBoss3(Role * role, bool is_flyaway, bool is_win);
	void OnRoleFightWildBoss(Role * role, bool is_flyaway, bool is_win);

	void OnRoleUpLevelEvent(Role* role, int old_level, int cur_level);	// ��ҵȼ������¼�
	void OnRoleProfProEvent(Role* role, int old_prof, int cur_prof);	// ���ְҵ�����¼�
	void OnRoleUpTopLevelEvent(Role* role, int old_level, int cur_level);	// ����۷�ȼ������¼�
	void OnRoleFlyUpEvent(Role* role, int fly_up_flag);					// ��ҽ�ɫ�����¼�

	void OnRoleRealmBreak(Role * role, int cur_level);					// ��Ҿ���ͻ��
	void OnRoleRealmBreakFight(Role * role, int fight_seq);				// �������֮����սʤ��

	void OnRolePutOnMedal(Role *role, int medal_num, int low_color);
	void OnRolePutOffMedal(Role * role);
	//ѫ�½���
	void OnRoleMedalUpGrade(Role * role, int quality_color, int grade);

	void OnYinYangEyeReward(Role * role, bool is_need_open_eye);
	void OnJieYiRoleChnage(Role * role, int join_jie_yi_day);

	void OnTimeInterval(Role * role, int delta_time);

	// �ɼ�
	void OnDig(Role* role);				// �ڿ�
	void OnLumbering(Role* role);		// ��ľ
	void OnPickFlower(Role* role);		// �ɻ�

	// ����
	void OnMakeEquipment(Role* role, ItemID equip_id, short num);		// ����װ��
	void OnGemMounting(Role* role,int item_id);							// ��Ƕ��ʯ
	void OnCompoundGem(Role* role, int item_id);						// �ϳɱ�ʯ
	void OnCompoundItem(Role* role, int item_id, int num);				// �����ϳ�
	void OnCompoundBook(Role* role);									// �ϳɼ�����
	void OnGemDismount(Role* role, int item_id);						// ��ʯ��ж
	void OnCompoundCard(Role* role, int num);							// �ϳɷ�����
	void OnEquipmentSmelt(Role * role);									// װ������
	void OnEquipmentSmeltHigh(Role * role, int quality_type, int num);	// װ��������ø�����

	// �
	void OnResearchTask(Role * role, int num, bool is_find = false);	// �����ħ һ�����+�һ�
	void OnAnswerQuestion(Role * role, bool is_correct, int num, bool is_find = false);		// ����ͨ  �ش�һ����Ŀ
	void OnMakeAWishOnce(Role * role);									// ��Ը��  ����һ��Ը��
	void OnEscortOneTime(Role * role, bool is_succ, int escort_seq);	// ���ڳ�  ����һ���ڳ�(��������)
	void OnEscortDayActivity(Role * role);								// �ճ������һ��
	void OnEscortStart(Role* role, int escort_seq);
	void OnFightTreasureMonster(Role * role, bool is_win);				// ��ͼ���� / ���õ��� ����һ�α�ͼ��
	void OnGetTreasureMap(Role * role);									// ��ͼ���� / ���õ��� �ɹ���ӽ���
	void OnBountyTaskCommit(Role * role, int num, bool is_find = false);// �ͽ�����
	void OnBountyTaskFinish(Role * role);								// �ͽ�����ȫ�����
	void OnTransactionTask(Role * role);								// ˿��֮·�������ȫ�ύ������(δ��ȡ���ս���)
	void OnTransactionTaskCommit(Role * role);							// ˿��֮·����ȫ��ȡ�������ύ
	void OnFantasyFBReferReward(Role * role);							// ����þ���ȡͨ�ؽ�������

	// ����
	void OnMarry(Role * role,int marry_type);				// ���
	void OnDivorce(Role * role);							// ���

	// ����
	void OnUseItem(Role * role, int item_id, int use_num);	// ʹ�õ���
	void OnBuyItem(Role * role, int item_id, int buy_num);	// �������
	void OnGetItem(Role * role, int item_id, int get_num, int reason);	// ��õ���
	void OnConsumeItem(Role* role, int item_id, int consume_num); // ���ĵ���

	// ����
	void OnAddCoinBind(Role * role, long long money);						// ���ͭ��
	void OnAddImmortalCoin(Role * role, long long money);					// ����ɱ�
	void OnAddGold(Role * role, long long money);							// ���Ԫ��
	void OnComsuneCoinBind(Role * role, long long money);					// ����ͭ��
	void OnComsuneImmortalCoin(Role * role, long long money);				// �����ɱ�
	void OnComsuneGold(Role * role, long long money);						// ����Ԫ��(ֻ������ϵͳ����)
	void OnAddOtherCoin(Role * role, int money_type, long long money);	// �����������
	void OnUseOtherCoin(Role *role , int money_type, long long money);	// ������������

	// ����
	void OnAddExp(Role * role, long long add_exp, short add_reason);

	// ����
	void OnRoleJoinTeam(Role * role, Team * team);			// �������
	void OnRoleLeaveTeam(Role * role, int team_index, bool is_team_dismiss);		// �뿪����
	void OnRoleBecomeLeader(Role * role, Team * team);		// ��Ϊ�ӳ�
	void OnTeamChangeLeader(Role * leader, int last_leader_id, Team * team);		// �ı�ӳ�
	void OnRoleRelieveLeader(Role * role, Team * team);		// ж�ζӳ�
	void OnTeamDismiss(Role * role, Team * team);			// �����ɢ
	void OnRoleRejoinTeam(Role* role, Team* team);			// ��һع����

	// ����
	void OnLearnSkill(Role * role, int skill_id);											// ѧϰ����
	void OnAddSkill(Role * role, RoleSkill::SkillItem * skill_item);						// ��ü���
	void OnSkillLevelUp(Role * role, int skill_id, int old_level, int cur_level);			// ��������
	void OnLifeSkillLevelUp(Role * role, int skill_id, int old_level, int cur_level);		// ���������
	void OnPetAquireSkill(Role* role, int pet_id, bool is_fight_pet, UInt16 skill_id, short skill_level);
	void OnPetForgetSkill(Role* role, int pet_id, bool is_fight_pet, UInt16 skill_id, short skill_level);

	// ����
	void OnRoleTrainLevelUp(Role * role, int train_type, int old_lv, int cur_lv);			// ���������ȼ�����
	void OnRoleTrainPetSkillLevelUp(Role* role, int type, int pet_cur_level);
	void OnRoleTrainPartnerSkillLevelUp(Role * role);
	
	//����
	void OnRoleActiveSmartMounts(Role * role, int index);			//��������
	void OnRoleStrSmartMountsJewelry(Role * role, int index);		//ǿ��������Ʒ

	//��������
	void OnRoleGuildInbornRoleChange(Role *role, int inborn_type);
	void OnRoleGuildInbornPetChange(Role * role, int inborn_type);
	void OnRoleGuildInbornPartnerChange(Role * role, int inborn_type);

	//�ķ�
	void OnRoleHeartSkillUnlock(Role * role, int heart_skill_seq);						//�ķ����ܼ���
	void OnRoleHeartSkillLevelUp(Role * role, int heart_skill_seq, int level);			//�ķ���������

	void OnFightMonsterGroup(Role* role, int monster_group_id, bool is_win, bool is_flyaway);
	void OnKillMonster(Role* role, int monster_num, int* monster_id_list, short* monster_level_list, bool is_mind_monster);
	void OnRoleStartMove(Role* role);
	void OnRoleStopMove(Role* role);

	void OnReloadConfig(int config_type, int param1, int param2);

	void OnRoleCapabilityChange(Role * role, int old_capability, int new_capability);
	void OnPetCapabilityChange(Role * role, int pet_capability);
	void OnPartnerCapabilityChange(Role * role, int old_partner_capability, int new_partner_capability);
	void OnRoleEquipCapabilityChange(Role * role, int old_equip_capability, int new_quip_capability);
	void OnRoleColosseumRankChange(Role * role, int old_colosseum_rank, int new_colosseum_rank);
	void OnRoleCourageChallengeChange(Role * role, int star_count);

	void OnRoleEnterScene(Role* role);
	void OnRoleLeaveScene(Role* role);

	// �ɼ�
	void OnStartGather(Role * role, int gather_id, int seq_id, const Posi &gather_pos);	// ��ʼ�ɼ�
	void OnSuccsGather(Role * role, int gather_id, int seq_id, const Posi &gather_pos);	// �ɼ��ɹ�
	void OnGetItemByGather(Role * role, int item_id, int item_num);				// ͨ���ɼ���õ���

	//����
	void OnSuccFish(Role * role);				// ����ɹ�

	void OnNpcGatherGatherThis(Role * role, Scene * scene, NPCGather * npc_gather);

	void OnRolePartnerJiBanNum(Role * role,int jiban_num);
	void OnActivePartner(Role * role, int partner_id);		// ������
	void OnPartnerUpLevel(Role * role,int partner_id, int old_level, int new_level);
	void OnPartnerStateChange(Role* role, int partner_id, bool is_dismiss);
	void OnPartnerPutOnEquipment(Role * role, int partner_id);
	void OnPartnerTakeOffEquipment(Role * role, int partner_id);
	void OnPartnerQualityUp(Role * role, int partner_id);
	void OnPartnerIntensify(Role *role, int paranter_id, int str_level, int quality);
	void OnPartnerBreak(Role *role, int paranter_id, int quality);

	void OnAddTitle(Role * role, int title_id, int title_count);				// ��óƺ�
	void OnLoseTitle(Role * role, int title_id, int title_count);				// ʧȥ�ƺ� title_countΪ��ǰ�ƺ�����			
	bool OnWearOrGetOffTitle(Role * role, int cur_title_id, bool is_wear = false);							// ����ƺ� / ���³ƺ� 
	void OnRolePetMonopolyCatchPet(Role* role, const ItemDataWrapper& pet_wrapper);
	void OnRolePetMonopolyGetReward(Role* role, const ItemConfigData& reward);

	void OnAddApprentice(Role * role);
	void OnDeleteApprentice(Role * role);
	void OnAddMaster(Role * role);
	void OnGraduation(Role * role, int apprentice_day_num);


	/**
	 * @brief ͨ�ظ�������� ���������ϴ�ͨ������
	 * @param fb_type �ؿ���id
	 * @param member_count ͨ�س�Ա����
	 * @param member_list ͨ�س�Ա��uid
	 * @param rounds ͨ�ػغ���
	 * @param time ͨ��ʱ��
	 */
	void OnClearanceFB(int fb_type, int member_count, int member_list[MAX_TEAM_MEMBER_NUM], int rounds, time_t time);	//TODO:δ����

	void OnClearFBOneLayer(Role* role, int fb_type, int param);

	void OnAddFace(Role * role, int res_type);								// �����ֵ���
	void OnLostFace(Role * role, int res_type);								// ʧȥ��ֵ���

	void OnChannelChat(Role * role, int channel_type);						// Ƶ������
	void OnFetchFriendGift(Role * role, int recevice_count = 1);			// ��ȡ��������

	void OnAddPrestige(Role * role, int village_id, int value, int old_level, int new_level);				// ��ô�ׯ����
	void OnPrestigeShopping(Role * role, int village_id, int item_id);		// ���������̵����
	void OnAddSurface(Role* role, int type, int id);						// ��ûû�
	void OnAcceptTask(Role* role, int task_id);
	void OnGiveupTask(Role* role, int task_id);
	void OnCommitTask(Role* role, int id);									// �������
	void OnFinishAnecdoteFinalTask(Role * role, int anecdote_task_type);	// �����������ĳ�����������
	void OnPartnerUpgradeLevel(Role* role, int id, int grade);				// ������
	void OnActivateCollection(Role* role, int id, int page, int color);		// ����ͼ��
	void OnJoinGuild(Role*role, GuildID id);								// �������
	void OnSetGuildPost(Role*role, int post);								// ������
	void OnCommitGuildTask(Role*role);										// ��ɼ�������		TODO:δ����
	void OnFetchGuildHongBao(Role * role);									// ��ȡ������
	void OnCorrectGuildAnswer(Role * role);									// ��Լ������
	void OnCommitSchoolTask(Role * role, int task_index);					// �������/ְҵ����
	void OnCommitTaskChain(Role * role);									// ���������
	void OnAddFriend(Role*role, int now_friend_count);						// �Ӻ���
	void OnFriendIntimacyChange(Role * role);								// ���ܶȱ��

	void OnIntimacyAddBySend(Role * role, int to_uid, int _add_val);

	void OnTradeMarketPutaway(Role * role);															// �������ϼ�(��
	void OnTradeMarketPutawayHour(Role * role);														// �������ϼ�(�����ϼ�,��������ʾ)
	void OnTradeMarketPurchase(Role* role, ItemID buy_item_id, int consume_gold);					// �����й���		
	void OnTradeMarketSaleItem(int seller_uid, ItemID sel_item_id, bool is_myself = false);			// �����г���
	void OnTradeMarketOffline(Role* role, int add_num, const ItemID * item_list);					// �����н��׳ɹ�(���ߵ���)���������߲����ߵ����
	void OnPassFBCourageChallenge(Role*role);								// ��������ս (��ħ֮���޸�,һЩ����������ʱ����)
	void OnEnterCourageChallenge(Role * role);								// ������������ս
	void OnPassCrystalEctype(Role * role, int diff_type, int monster_id = 0, int count = 1); // ͨ��ˮ������
	void OnKillCrystalBoss(Role * role, int boss_id, std::vector<ItemConfigData> * drop_list = NULL, bool is_has_activity_reward = true);	// ��ɱˮ������Boss
	void OnUseCrystalEnergy(Role * role, int num, int enter_energy_change_type);// ˮ��������������
	void OnSpecialUseEnergy(Role * role, int num, int enter_energy_change_type);// ����һЩ�������������߼�(�����ر���)
	void OnPassMedalEctype(Role * role);									// ͨ��ѫ�¸���
	void OnPassChallengeEctype(Role * role, int fb_id, int chapter);		// ͨ����ս����
	void OnPassGuideEctype(Role * role);									// ͨ��ָ������
	void OnPassFantasyEctype(Role *role);									// ͨ������þ�
	void OnPassMazeRacerEctype(Role * role, int maze_id);					// ͨ���Թ�����
	void OnPassMiYiFB(Role * role, int pass_level, int pass_index);			// ͨ����ҽ����ı
	void OnRealKillMiYiFBBoss(Role * role);	// ��ɱ����֮��BOSS(ÿ��boss��ɱ����)

	void OnEnterPetMonopoly(Role * role, int level);						// �������޵��²���
	void OnPassLunHuiWangChuan(Role * role, int level);						// ͨ���ֻ�����
	void OnPassShanHai(Role * role, int grade_type);						// ͨ��ɽ������
	void OnKillMiYiFBBoss(Role * role, int level_seq, int boss_index, bool is_first = false);			// ��ɱ����֮��BOSS(ֻ��ÿ����ɱboss��)
	void OnPassMaterial(Role * role, int chapter_id, int level_seq, bool is_first = false);				// ͨ�����龳�ؿ�(ɨ��Ҳ���Խ�ȥ�ýӿ�)
	void OnPassBigDipper(Role * role, int sn);								// ͨ�ر�������
	void OnPassNetherWorldPurgatory(Role * role, int pass_chapters, int challenge_id);	// ͨ����ڤ����
	void OnPassYaoXingShiLian(Role * role, int pass_seq);					// ͨ����������
	void OnPassFBNewCourageChallenge(Role * role);							// ͨ��߱��֮��
	void OnPassMirageTerritory(Role * role, int area, int pass_seq);		// ͨ�������׾�
	void OnPassHongMengTianJie(Role * role, int fb_id, bool is_first);		// ͨ�غ������

	void OnParticipateWorldBoss(Role * role);								// ��������BOSS(ħ������)
	void OnParticipateWorldBoss2(Role * role);								// ��������BOSS(��������)
	void OnParticipateWorldBoss3(Role * role);								// ��������BOSS(������)
	void OnEnterBraveGround(Role * role);									// �����´��ع�
	void OnEnterTeamFight(Role * role);										// ����С�Ӿ���
	void OnEnterMazeRacer(Role * role);										// �����Թ�����
	void OnEnterGuildFight(Role * role);									// ���������Ҷ�
	void OnParticipateChiefElection(Role * role);							// ������ϯ��ѡ
	void OnEnterGuildHonorBattle(Role * role);								// ���������ҫս
	void OnEnterGuildAnswer(Role * role);									// ����������
	void OnEnterOnlyFight(Role * role);										// ������һ�а�
	void OnParticipateHuanJieZhanChange(Role * role);						// ����ý�ս��
	void OnParticipateHundredGhost(Role * role);							// ����ٹ�ҹ��
	void OnParticipateSilkRoads(Role * role);								// ����˿��֮·
	void OnParticipateRelicLooting(Role * role);							// �����ż��ᱦ
	void OnParticipateWorldTeamArena(Role * role);							// ����Ӣ�ۻ���
	
	void OnPassDreamNotesChallenge(Role * role, int level, bool is_first, bool is_fast = false);	// ͨ����Ԩ��¼
	void OnBraveGroundPassNewLevel(Role * role, int level);					// �´��ع�ͨ���¹ؿ�(����)
	void OnBraveGroundPassLevelChange(Role * role, int level);				// �´��ع�����ͨ�عؿ������仯
	void OnHuanJieZhanChanDataChange(Role * role, int rank_score);			// ��ɫ�ý�ս�����ݸı�

	void OnRAGodTokenFinishLimitActivity(Role * role, int activity_type);
	// ������ʱ� 
	void OnParticipateLimitActivity(Role * role, int activity_type, const char * reason_str, int param1 = 0);			

	//�µȼ�������������ȼ�Ӱ��
	void OnFinishLevelComplementRelatedFun(Role *role, long long add_exp, const char * reason);	//��ɵȼ�������ع���

	void OnWinChiefElection(Role * role, int prof);							//���������ϯ
	void OnRoleFinishActivityTeamFight(Role * role,int rank);				//�С�Ӿ���-���˽�������
	void OnGoldShopConsume(Role * role,int consume_num);					//Ԫ���̳�����Ԫ����
	void OnNewShopConsume(Role * role,int consume_num);						//�̳����ĵ�ͭ��
	void OnOpenTreasureChest(Role * role);									//����ˮ����������
	void OnTueasureHunt(Role * role, int count, int treasure_type);			//Ѱ������

	void OnRecaleReason(Role *role, int recale_reason, bool is_init);		// ��������

	// ����
	void OnRoleTrapCreature(Role * role, int pet_id, bool is_succ);				// ��׽����
	void OnRoleGetACreature(Role * role, int pet_id, long long unique_id, PetParam * pet_param = NULL);			// ��ó���
	void OnSwitchPetToFight(Role * role);
	void OnFightPetToStandby(Role * role); 
	void OnFightPetRest(Role * role);
	void OnPetAllotAttrpoint(Role * role, int con, int str, int def, int agi, int mgc);
	void OnPetUpLevel(Role * role, int pet_index, int pet_level);									// ��������
	void OnPetAbandon(Role * role, int pet_quality);												// �������
	void OnPetCompose(Role * role, bool is_compose);												// ����ϳ�/�һ�
	void OnPetIntensi(Role * role, int pet_index, int pet_type, int pet_level);						// ����ǿ��
	void OnPetRefined(Role * role, int pet_index, int pet_quality, bool is_mutant);					// ����ϴ��
	void OnPetChangeTalent(Role * role, int pet_index, PetParam * pet_param);						// ����ϴ�������滻�츳
	void OnUseSkillBook(Role * role, int pet_index, int replace_gift_index, int skill_id, int pet_quality, int skill_type);	// ����ʹ�ü������滻�츳
	void OnComprehendPassiveSkill(Role * role, int pet_index, PetParam * pet_param, int f_index);	// ����������
	void OnDeletePet(Role * role, long long unique_id);												// ɾ������
	void OnPetChangeName(Role * role, int pet_index, GameName name);								// �������
	void OnPetMoveStorage(Role * role, long long unique_id);										// �����ƶ��������ֿ�
	void OnPetStrorageMoveBag(Role * role, long long unique_id);									// ����ֿ����뱳��
	void OnPetPutEquipment(Role * role);															// ��������װ��(����)
	void OnPetTakeOffEquipment(Role * role);														// ���³���װ��
	void OnAddPetBenediction(Role * role, long long vice_item_unique_id);							// ����ף��
	void OnPetLianYao(Role * role, int pet_quality);												// ��������
	void OnPetPutSoulEquip(Role * role);															// �����������

	void OnPetBaseInfoChange(Role* role, int pet_index);

	bool MonsterIsLockNotice(Role* role,int battle_type);
	bool TeamNotEnoughMembers(Role* role, int battle_type);
	bool NotInTeam(Role* role, int battle_type);

	void OnPutOnJewelry(Role * role, int jewelry_index);
	void OnTakeOffJewelry(Role * role, int jewelry_index);

	void OnRoleTakeOffEquip(Role* role, int equip_index, bool is_from_puton = false, int role_type = ROLE_TYPE_BENZUN);
	void OnRolePutOnEquip(Role* role, int equip_index, int role_type = ROLE_TYPE_BENZUN);
	void OnRoleWearOrTakeOffSurface(Role * role);

	//װ��ϵͳ-ǿ��
	void OnRoleEquipStr(Role * role, int equip_jewelry_type, int str_level);

	void OnRoleLifeSkillPropsChange(Role * role, int new_life_skill_props_type, bool is_use);

	//ps. is_add_day_chong_zhi:�Ƿ����ÿ���۳�
	void OnAddChongzhi(Role * role, long long chongzhi_gold, bool is_need_mail = true);
	void OnAddChongzhiNewGiftBuy(Role * role, long long chongzhi_gold, bool is_need_mail = true, bool is_add_day_chong_zhi = true);

	//����ϵͳ
	void OnOpenHuanShouEgg(Role * role);
	void OnGetNewHuanShou(Role * role, int huanshou_id);
	void OnStrHuanShouLevel(Role * role, int huanshou_id, int level);
	void OnHuanShouLearnSkill(Role * role, int huanshou_id);
	void OnHuanShouLianDan(Role * role);
	void OnHuanShouDecompose(Role * role, int decompose_count);
	void OnHuanShouFuShenChange(Role * role);
	
	/**
	* \brief �û���¼�����ʱ֪ͨ����
	* \note �ú��������ڱ�������
	*/
	void OnUserLoginCrossNoticeGame(const UserID& user_id);
	void OnUserLogoutCrossNoticeGame(const UserID& user_id);

	/**
	* \brief �û���¼�����ط�
	* \note �ú��������ڿ������
	*/
	void OnUserLoginCrossHidden(const UserID& user_id, const CrossData_Business& business_data);
	void OnRoleAutoJoin(const UserID & user_id, const CrossData_Business & business_data);
	void GameHiddenAutoJoinActivity(Role * role, const CrossData_Business & business_data);
	void GameHiddenAutoJoinFb(Role * role, const CrossData_Business & business_data);

	void OnRoleChangeName(Role* role);

	void OnBattleServerDisconnect(int battle_server_index);

	void OnRoleChangeAppearance(Role* role, bool is_app_change = false);
	void OnRoleChangeGatherDreamInfo(Role* role, int guild_id,SynGatherDreamRecord *syn_info = NULL, int *uid = NULL);
	void OnRoleClearFB(Role* role, int fb_type, int param = 0, int count = 1);
	void OnRolePassCrystalFB(Role* role, int crystal_fb_seq, int count = 1);
	void OnRolePassCrystalFB2(Role * role, int crystal_fb_seq, int shui_jing_type, int fb_index);

	void OnRoleChallengeColosseum(Role* role);
	void OnRoleCloudArenaChange(Role * role, int cur_season_high_score);		//���񷶥���ݱ仯

	void OnRoleEnterFB(Role* role, int fb_type, int seq);
	
	void OnConsumeGold(Role* role, int consume_num);						//����Ԫ��(����ԭ��)			
	void OnConsumeCoinBind(Role* role, int consume_num);		

	void OnRoleDrop(Role* role, ItemID item_id, int num);

	void OnRoleFinishSpecialBattle(Role *role,int battle_mode,bool is_win);	//����ս����ɽ���

	void OnAddFaBao(Role * role, int fabao_id);
	void OnFaBaoLevelUp(Role * role, int fabao_id, int level);				//��������
	void OnCourseOfGrowthProgChange(Role * role,int old_prog,int prog);

	void OnRoleResetName(Role* role, GameName old_name, GameName new_name);

	void OnRoleTopRank(Role *role,int rank_idx);
	void OnAddActive(Role* role, int add_num);

	void OnRoleSettingNumsOfSceenChange(Role* role, int cur_num);

	void OnRoleFormationChange(Role* role, bool is_current);
	void OnSetAutoCatchPet(Role* role);

	void OnMutePlayer(int role_id, int forbid_time_s);
	void OnForbidPlayer(int role_id, int forbid_time_s);

	void OnMarketItemPurchasedByOtherSystem(Role * role, int item_id, int num, int price, int money_type);
	void OnMarketBuyItem(Role * role, int item_id, int num, int price, int money_type);

	void OnRoleChivalrousChange(Role* role,int times);

	void OnPassPetMonopoly(Role * role);

	void OnPassPetGodFB(Role * role, int fb_req);
	
	void OnRAGodTokenBuyToken(Role * _role);

	void OnGetPetSoul(Role * role, int item_id, bool need_sync = true);
	void OnRoleCommitPrestigeTask(Role* role, int task_id);

	void OnGetHolyEquip(Role* role, int shengqi_id);		// ���ʥ��,ע��������ظ���ʥ��

	void OnCloudArenaUseTimes(int role_id);
	void OnCloudArenaFightFinish(int role_id, bool is_win);

	void OnFinishFight(battlegameprotocol::BattleGameFightResultAck * _msg);

	void OnPetHelperFormationChanged(Role* role, int type);

	void OnWanLingFBPass(Role * role, int fb_index);	//���黰�� ͨ�ؽ���

	void OnCareerAdvance(Role* role, int main_lv, int sub_lv);

	void OnPetWishGodPrint(Role* role);

	// ��Ϊ
	void OnXiuWeiRoleTypeLevelUp(Role * _role);
	void OnXiuWeiPetTypeLevelUp(Role * _role);
	void OnXiuWeiParnerTypeLevelUp(Role * _role);

	void OnCommitTreasureTask(Role * role);				// �����ر�
	void OnFinishTreasureTask(Role * role);				// ���һ�־����ر�

	void OnOriginDisconnectFromCrossServer(int plat_type, int server_id);

	void OnQingYuanJieHun(Role * role, long long qingyuan_id);

	void OnRecvNoticeForUserLoginOrigin(int role_id);
	void OnWorldTeamArenaChangeByRole(Role * role);		// ��ɫ�����Ϣ���ͬ��Ӣ�ۻ���

	void OnHiddenOpenServerTimeChange();
private:
	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(EventHandler);


	void DropElementFaction(Role* role, bool is_mine_monster, const MonsterCfg* cfg, int& total_drop_element_num);
};

#endif // __EVENT_HANDER_HPP__
