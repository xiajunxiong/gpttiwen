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

	void OnRoleUpLevelEvent(Role* role, int old_level, int cur_level);	// 玩家等级提升事件
	void OnRoleProfProEvent(Role* role, int old_prof, int cur_prof);	// 玩家职业进阶事件
	void OnRoleUpTopLevelEvent(Role* role, int old_level, int cur_level);	// 玩家巅峰等级提升事件
	void OnRoleFlyUpEvent(Role* role, int fly_up_flag);					// 玩家角色飞升事件

	void OnRoleRealmBreak(Role * role, int cur_level);					// 玩家境界突破
	void OnRoleRealmBreakFight(Role * role, int fight_seq);				// 玩家心灵之境挑战胜利

	void OnRolePutOnMedal(Role *role, int medal_num, int low_color);
	void OnRolePutOffMedal(Role * role);
	//勋章进阶
	void OnRoleMedalUpGrade(Role * role, int quality_color, int grade);

	void OnYinYangEyeReward(Role * role, bool is_need_open_eye);
	void OnJieYiRoleChnage(Role * role, int join_jie_yi_day);

	void OnTimeInterval(Role * role, int delta_time);

	// 采集
	void OnDig(Role* role);				// 挖矿
	void OnLumbering(Role* role);		// 伐木
	void OnPickFlower(Role* role);		// 采花

	// 工坊
	void OnMakeEquipment(Role* role, ItemID equip_id, short num);		// 打造装备
	void OnGemMounting(Role* role,int item_id);							// 镶嵌宝石
	void OnCompoundGem(Role* role, int item_id);						// 合成宝石
	void OnCompoundItem(Role* role, int item_id, int num);				// 工坊合成
	void OnCompoundBook(Role* role);									// 合成技能书
	void OnGemDismount(Role* role, int item_id);						// 宝石拆卸
	void OnCompoundCard(Role* role, int num);							// 合成封妖卡
	void OnEquipmentSmelt(Role * role);									// 装备熔炼
	void OnEquipmentSmeltHigh(Role * role, int quality_type, int num);	// 装备熔炼获得高属性

	// 活动
	void OnResearchTask(Role * role, int num, bool is_find = false);	// 三界除魔 一键完成+找回
	void OnAnswerQuestion(Role * role, bool is_correct, int num, bool is_find = false);		// 万事通  回答一次题目
	void OnMakeAWishOnce(Role * role);									// 许愿池  许下一次愿望
	void OnEscortOneTime(Role * role, bool is_succ, int escort_seq);	// 运镖车  运送一次镖车(所有运镖)
	void OnEscortDayActivity(Role * role);								// 日常活动运镖一次
	void OnEscortStart(Role* role, int escort_seq);
	void OnFightTreasureMonster(Role * role, bool is_win);				// 宝图任务 / 缉拿盗贼 击败一次宝图怪
	void OnGetTreasureMap(Role * role);									// 宝图任务 / 缉拿盗贼 成功添加进度
	void OnBountyTaskCommit(Role * role, int num, bool is_find = false);// 赏金任务
	void OnBountyTaskFinish(Role * role);								// 赏金任务全部完成
	void OnTransactionTask(Role * role);								// 丝绸之路所需道具全提交完后调用(未领取最终奖励)
	void OnTransactionTaskCommit(Role * role);							// 丝绸之路奖励全领取后任务提交
	void OnFantasyFBReferReward(Role * role);							// 四象幻境领取通关奖励调用

	// 婚姻
	void OnMarry(Role * role,int marry_type);				// 结婚
	void OnDivorce(Role * role);							// 离婚

	// 道具
	void OnUseItem(Role * role, int item_id, int use_num);	// 使用道具
	void OnBuyItem(Role * role, int item_id, int buy_num);	// 购买道具
	void OnGetItem(Role * role, int item_id, int get_num, int reason);	// 获得道具
	void OnConsumeItem(Role* role, int item_id, int consume_num); // 消耗道具

	// 货币
	void OnAddCoinBind(Role * role, long long money);						// 获得铜币
	void OnAddImmortalCoin(Role * role, long long money);					// 获得仙币
	void OnAddGold(Role * role, long long money);							// 获得元宝
	void OnComsuneCoinBind(Role * role, long long money);					// 消耗铜币
	void OnComsuneImmortalCoin(Role * role, long long money);				// 消耗仙币
	void OnComsuneGold(Role * role, long long money);						// 消耗元宝(只计算在系统消费)
	void OnAddOtherCoin(Role * role, int money_type, long long money);	// 获得其它货币
	void OnUseOtherCoin(Role *role , int money_type, long long money);	// 消耗其它货币

	// 经验
	void OnAddExp(Role * role, long long add_exp, short add_reason);

	// 队伍
	void OnRoleJoinTeam(Role * role, Team * team);			// 加入队伍
	void OnRoleLeaveTeam(Role * role, int team_index, bool is_team_dismiss);		// 离开队伍
	void OnRoleBecomeLeader(Role * role, Team * team);		// 成为队长
	void OnTeamChangeLeader(Role * leader, int last_leader_id, Team * team);		// 改变队长
	void OnRoleRelieveLeader(Role * role, Team * team);		// 卸任队长
	void OnTeamDismiss(Role * role, Team * team);			// 队伍解散
	void OnRoleRejoinTeam(Role* role, Team* team);			// 玩家回归队伍

	// 技能
	void OnLearnSkill(Role * role, int skill_id);											// 学习技能
	void OnAddSkill(Role * role, RoleSkill::SkillItem * skill_item);						// 获得技能
	void OnSkillLevelUp(Role * role, int skill_id, int old_level, int cur_level);			// 技能升级
	void OnLifeSkillLevelUp(Role * role, int skill_id, int old_level, int cur_level);		// 生活技能升级
	void OnPetAquireSkill(Role* role, int pet_id, bool is_fight_pet, UInt16 skill_id, short skill_level);
	void OnPetForgetSkill(Role* role, int pet_id, bool is_fight_pet, UInt16 skill_id, short skill_level);

	// 修炼
	void OnRoleTrainLevelUp(Role * role, int train_type, int old_lv, int cur_lv);			// 人物修炼等级提升
	void OnRoleTrainPetSkillLevelUp(Role* role, int type, int pet_cur_level);
	void OnRoleTrainPartnerSkillLevelUp(Role * role);
	
	//灵骑
	void OnRoleActiveSmartMounts(Role * role, int index);			//解锁灵骑
	void OnRoleStrSmartMountsJewelry(Role * role, int index);		//强化灵骑饰品

	//家族灵脉
	void OnRoleGuildInbornRoleChange(Role *role, int inborn_type);
	void OnRoleGuildInbornPetChange(Role * role, int inborn_type);
	void OnRoleGuildInbornPartnerChange(Role * role, int inborn_type);

	//心法
	void OnRoleHeartSkillUnlock(Role * role, int heart_skill_seq);						//心法技能激活
	void OnRoleHeartSkillLevelUp(Role * role, int heart_skill_seq, int level);			//心法技能升级

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

	// 采集
	void OnStartGather(Role * role, int gather_id, int seq_id, const Posi &gather_pos);	// 开始采集
	void OnSuccsGather(Role * role, int gather_id, int seq_id, const Posi &gather_pos);	// 采集成功
	void OnGetItemByGather(Role * role, int item_id, int item_num);				// 通过采集获得道具

	//钓鱼
	void OnSuccFish(Role * role);				// 钓鱼成功

	void OnNpcGatherGatherThis(Role * role, Scene * scene, NPCGather * npc_gather);

	void OnRolePartnerJiBanNum(Role * role,int jiban_num);
	void OnActivePartner(Role * role, int partner_id);		// 激活伙伴
	void OnPartnerUpLevel(Role * role,int partner_id, int old_level, int new_level);
	void OnPartnerStateChange(Role* role, int partner_id, bool is_dismiss);
	void OnPartnerPutOnEquipment(Role * role, int partner_id);
	void OnPartnerTakeOffEquipment(Role * role, int partner_id);
	void OnPartnerQualityUp(Role * role, int partner_id);
	void OnPartnerIntensify(Role *role, int paranter_id, int str_level, int quality);
	void OnPartnerBreak(Role *role, int paranter_id, int quality);

	void OnAddTitle(Role * role, int title_id, int title_count);				// 获得称号
	void OnLoseTitle(Role * role, int title_id, int title_count);				// 失去称号 title_count为当前称号数量			
	bool OnWearOrGetOffTitle(Role * role, int cur_title_id, bool is_wear = false);							// 佩戴称号 / 脱下称号 
	void OnRolePetMonopolyCatchPet(Role* role, const ItemDataWrapper& pet_wrapper);
	void OnRolePetMonopolyGetReward(Role* role, const ItemConfigData& reward);

	void OnAddApprentice(Role * role);
	void OnDeleteApprentice(Role * role);
	void OnAddMaster(Role * role);
	void OnGraduation(Role * role, int apprentice_day_num);


	/**
	 * @brief 通关副本后调用 给荣誉榜上传通关数据
	 * @param fb_type 关卡的id
	 * @param member_count 通关成员人数
	 * @param member_list 通关成员的uid
	 * @param rounds 通关回合数
	 * @param time 通关时间
	 */
	void OnClearanceFB(int fb_type, int member_count, int member_list[MAX_TEAM_MEMBER_NUM], int rounds, time_t time);	//TODO:未调用

	void OnClearFBOneLayer(Role* role, int fb_type, int param);

	void OnAddFace(Role * role, int res_type);								// 获得颜值外观
	void OnLostFace(Role * role, int res_type);								// 失去颜值外观

	void OnChannelChat(Role * role, int channel_type);						// 频道聊天
	void OnFetchFriendGift(Role * role, int recevice_count = 1);			// 领取好友赠礼

	void OnAddPrestige(Role * role, int village_id, int value, int old_level, int new_level);				// 获得村庄声望
	void OnPrestigeShopping(Role * role, int village_id, int item_id);		// 购买声望商店道具
	void OnAddSurface(Role* role, int type, int id);						// 获得幻化
	void OnAcceptTask(Role* role, int task_id);
	void OnGiveupTask(Role* role, int task_id);
	void OnCommitTask(Role* role, int id);									// 完成任务
	void OnFinishAnecdoteFinalTask(Role * role, int anecdote_task_type);	// 完成奇闻任务某类型最后任务
	void OnPartnerUpgradeLevel(Role* role, int id, int grade);				// 伙伴进阶
	void OnActivateCollection(Role* role, int id, int page, int color);		// 激活图鉴
	void OnJoinGuild(Role*role, GuildID id);								// 加入家族
	void OnSetGuildPost(Role*role, int post);								// 被任命
	void OnCommitGuildTask(Role*role);										// 完成家族任务		TODO:未调用
	void OnFetchGuildHongBao(Role * role);									// 领取家族红包
	void OnCorrectGuildAnswer(Role * role);									// 答对家族答题
	void OnCommitSchoolTask(Role * role, int task_index);					// 完成门派/职业任务
	void OnCommitTaskChain(Role * role);									// 完成任务链
	void OnAddFriend(Role*role, int now_friend_count);						// 加好友
	void OnFriendIntimacyChange(Role * role);								// 亲密度变更

	void OnIntimacyAddBySend(Role * role, int to_uid, int _add_val);

	void OnTradeMarketPutaway(Role * role);															// 交易行上架(包
	void OnTradeMarketPutawayHour(Role * role);														// 交易行上架(整点上架,不包括公示)
	void OnTradeMarketPurchase(Role* role, ItemID buy_item_id, int consume_gold);					// 交易行购买		
	void OnTradeMarketSaleItem(int seller_uid, ItemID sel_item_id, bool is_myself = false);			// 交易行出售
	void OnTradeMarketOffline(Role* role, int add_num, const ItemID * item_list);					// 交易行交易成功(离线调用)用于售卖者不在线的情况
	void OnPassFBCourageChallenge(Role*role);								// 勇气的挑战 (锢魔之塔修改,一些关联功能暂时屏蔽)
	void OnEnterCourageChallenge(Role * role);								// 参与勇气的挑战
	void OnPassCrystalEctype(Role * role, int diff_type, int monster_id = 0, int count = 1); // 通关水晶副本
	void OnKillCrystalBoss(Role * role, int boss_id, std::vector<ItemConfigData> * drop_list = NULL, bool is_has_activity_reward = true);	// 击杀水晶副本Boss
	void OnUseCrystalEnergy(Role * role, int num, int enter_energy_change_type);// 水晶副本消耗体力
	void OnSpecialUseEnergy(Role * role, int num, int enter_energy_change_type);// 用于一些特殊消耗体力逻辑(例如秘宝阁)
	void OnPassMedalEctype(Role * role);									// 通关勋章副本
	void OnPassChallengeEctype(Role * role, int fb_id, int chapter);		// 通关挑战副本
	void OnPassGuideEctype(Role * role);									// 通关指引副本
	void OnPassFantasyEctype(Role *role);									// 通关四象幻境
	void OnPassMazeRacerEctype(Role * role, int maze_id);					// 通关迷宫竞速
	void OnPassMiYiFB(Role * role, int pass_level, int pass_index);			// 通关密医的阴谋
	void OnRealKillMiYiFBBoss(Role * role);	// 击杀试炼之地BOSS(每个boss击杀都进)

	void OnEnterPetMonopoly(Role * role, int level);						// 进入灵兽岛新层数
	void OnPassLunHuiWangChuan(Role * role, int level);						// 通关轮回忘川
	void OnPassShanHai(Role * role, int grade_type);						// 通关山海降妖
	void OnKillMiYiFBBoss(Role * role, int level_seq, int boss_index, bool is_first = false);			// 击杀试炼之地BOSS(只有每周首杀boss进)
	void OnPassMaterial(Role * role, int chapter_id, int level_seq, bool is_first = false);				// 通关玉虚境关卡(扫荡也可以进去该接口)
	void OnPassBigDipper(Role * role, int sn);								// 通关北斗七星
	void OnPassNetherWorldPurgatory(Role * role, int pass_chapters, int challenge_id);	// 通关幽冥炼狱
	void OnPassYaoXingShiLian(Role * role, int pass_seq);					// 通关妖行试炼
	void OnPassFBNewCourageChallenge(Role * role);							// 通关弑神之塔
	void OnPassMirageTerritory(Role * role, int area, int pass_seq);		// 通关琉璃蜃境
	void OnPassHongMengTianJie(Role * role, int fb_id, bool is_first);		// 通关鸿蒙天界

	void OnParticipateWorldBoss(Role * role);								// 参与世界BOSS(魔将入侵)
	void OnParticipateWorldBoss2(Role * role);								// 参与世界BOSS(巨龙入侵)
	void OnParticipateWorldBoss3(Role * role);								// 参与世界BOSS(天神降临)
	void OnEnterBraveGround(Role * role);									// 参与勇闯地宫
	void OnEnterTeamFight(Role * role);										// 参与小队竞技
	void OnEnterMazeRacer(Role * role);										// 参与迷宫竞速
	void OnEnterGuildFight(Role * role);									// 参与家族大乱斗
	void OnParticipateChiefElection(Role * role);							// 参与首席竞选
	void OnEnterGuildHonorBattle(Role * role);								// 参与家族荣耀战
	void OnEnterGuildAnswer(Role * role);									// 参与家族答题
	void OnEnterOnlyFight(Role * role);										// 参与以一敌百
	void OnParticipateHuanJieZhanChange(Role * role);						// 参与幻界战场
	void OnParticipateHundredGhost(Role * role);							// 参与百鬼夜行
	void OnParticipateSilkRoads(Role * role);								// 参与丝绸之路
	void OnParticipateRelicLooting(Role * role);							// 参与遗迹夺宝
	void OnParticipateWorldTeamArena(Role * role);							// 参与英雄会武
	
	void OnPassDreamNotesChallenge(Role * role, int level, bool is_first, bool is_fast = false);	// 通关梦渊笔录
	void OnBraveGroundPassNewLevel(Role * role, int level);					// 勇闯地宫通关新关卡(生涯)
	void OnBraveGroundPassLevelChange(Role * role, int level);				// 勇闯地宫本周通关关卡层数变化
	void OnHuanJieZhanChanDataChange(Role * role, int rank_score);			// 角色幻界战场数据改变

	void OnRAGodTokenFinishLimitActivity(Role * role, int activity_type);
	// 参与限时活动 
	void OnParticipateLimitActivity(Role * role, int activity_type, const char * reason_str, int param1 = 0);			

	//新等级助力不受世界等级影响
	void OnFinishLevelComplementRelatedFun(Role *role, long long add_exp, const char * reason);	//完成等级助力相关功能

	void OnWinChiefElection(Role * role, int prof);							//夺得门派首席
	void OnRoleFinishActivityTeamFight(Role * role,int rank);				//活动小队竞技-个人结算排名
	void OnGoldShopConsume(Role * role,int consume_num);					//元宝商场消耗元宝数
	void OnNewShopConsume(Role * role,int consume_num);						//商城消耗的铜币
	void OnOpenTreasureChest(Role * role);									//开启水晶副本宝箱
	void OnTueasureHunt(Role * role, int count, int treasure_type);			//寻宝次数

	void OnRecaleReason(Role *role, int recale_reason, bool is_init);		// 属性重算

	// 宠物
	void OnRoleTrapCreature(Role * role, int pet_id, bool is_succ);				// 捕捉宠物
	void OnRoleGetACreature(Role * role, int pet_id, long long unique_id, PetParam * pet_param = NULL);			// 获得宠物
	void OnSwitchPetToFight(Role * role);
	void OnFightPetToStandby(Role * role); 
	void OnFightPetRest(Role * role);
	void OnPetAllotAttrpoint(Role * role, int con, int str, int def, int agi, int mgc);
	void OnPetUpLevel(Role * role, int pet_index, int pet_level);									// 宠物升级
	void OnPetAbandon(Role * role, int pet_quality);												// 宠物放生
	void OnPetCompose(Role * role, bool is_compose);												// 宠物合成/兑换
	void OnPetIntensi(Role * role, int pet_index, int pet_type, int pet_level);						// 宠物强化
	void OnPetRefined(Role * role, int pet_index, int pet_quality, bool is_mutant);					// 宠物洗练
	void OnPetChangeTalent(Role * role, int pet_index, PetParam * pet_param);						// 宠物洗练保存替换天赋
	void OnUseSkillBook(Role * role, int pet_index, int replace_gift_index, int skill_id, int pet_quality, int skill_type);	// 宠物使用技能书替换天赋
	void OnComprehendPassiveSkill(Role * role, int pet_index, PetParam * pet_param, int f_index);	// 宠物领悟技能
	void OnDeletePet(Role * role, long long unique_id);												// 删除宠物
	void OnPetChangeName(Role * role, int pet_index, GameName name);								// 宠物改名
	void OnPetMoveStorage(Role * role, long long unique_id);										// 背包移动宠物进入仓库
	void OnPetStrorageMoveBag(Role * role, long long unique_id);									// 宠物仓库移入背包
	void OnPetPutEquipment(Role * role);															// 穿戴宠物装备(护符)
	void OnPetTakeOffEquipment(Role * role);														// 脱下宠物装备
	void OnAddPetBenediction(Role * role, long long vice_item_unique_id);							// 宠物祝福
	void OnPetLianYao(Role * role, int pet_quality);												// 宠物炼妖
	void OnPetPutSoulEquip(Role * role);															// 穿戴宠物魂器

	void OnPetBaseInfoChange(Role* role, int pet_index);

	bool MonsterIsLockNotice(Role* role,int battle_type);
	bool TeamNotEnoughMembers(Role* role, int battle_type);
	bool NotInTeam(Role* role, int battle_type);

	void OnPutOnJewelry(Role * role, int jewelry_index);
	void OnTakeOffJewelry(Role * role, int jewelry_index);

	void OnRoleTakeOffEquip(Role* role, int equip_index, bool is_from_puton = false, int role_type = ROLE_TYPE_BENZUN);
	void OnRolePutOnEquip(Role* role, int equip_index, int role_type = ROLE_TYPE_BENZUN);
	void OnRoleWearOrTakeOffSurface(Role * role);

	//装备系统-强化
	void OnRoleEquipStr(Role * role, int equip_jewelry_type, int str_level);

	void OnRoleLifeSkillPropsChange(Role * role, int new_life_skill_props_type, bool is_use);

	//ps. is_add_day_chong_zhi:是否计入每日累充
	void OnAddChongzhi(Role * role, long long chongzhi_gold, bool is_need_mail = true);
	void OnAddChongzhiNewGiftBuy(Role * role, long long chongzhi_gold, bool is_need_mail = true, bool is_add_day_chong_zhi = true);

	//幻兽系统
	void OnOpenHuanShouEgg(Role * role);
	void OnGetNewHuanShou(Role * role, int huanshou_id);
	void OnStrHuanShouLevel(Role * role, int huanshou_id, int level);
	void OnHuanShouLearnSkill(Role * role, int huanshou_id);
	void OnHuanShouLianDan(Role * role);
	void OnHuanShouDecompose(Role * role, int decompose_count);
	void OnHuanShouFuShenChange(Role * role);
	
	/**
	* \brief 用户登录到跨服时通知本服
	* \note 该函数仅用于本服调用
	*/
	void OnUserLoginCrossNoticeGame(const UserID& user_id);
	void OnUserLogoutCrossNoticeGame(const UserID& user_id);

	/**
	* \brief 用户登录到隐藏服
	* \note 该函数仅用于跨服调用
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
	void OnRoleCloudArenaChange(Role * role, int cur_season_high_score);		//玩家穹顶数据变化

	void OnRoleEnterFB(Role* role, int fb_type, int seq);
	
	void OnConsumeGold(Role* role, int consume_num);						//消耗元宝(所有原因)			
	void OnConsumeCoinBind(Role* role, int consume_num);		

	void OnRoleDrop(Role* role, ItemID item_id, int num);

	void OnRoleFinishSpecialBattle(Role *role,int battle_mode,bool is_win);	//特殊战斗完成奖励

	void OnAddFaBao(Role * role, int fabao_id);
	void OnFaBaoLevelUp(Role * role, int fabao_id, int level);				//法宝升级
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

	void OnGetHolyEquip(Role* role, int shengqi_id);		// 获得圣器,注意可能是重复的圣器

	void OnCloudArenaUseTimes(int role_id);
	void OnCloudArenaFightFinish(int role_id, bool is_win);

	void OnFinishFight(battlegameprotocol::BattleGameFightResultAck * _msg);

	void OnPetHelperFormationChanged(Role* role, int type);

	void OnWanLingFBPass(Role * role, int fb_index);	//万灵话本 通关结算

	void OnCareerAdvance(Role* role, int main_lv, int sub_lv);

	void OnPetWishGodPrint(Role* role);

	// 修为
	void OnXiuWeiRoleTypeLevelUp(Role * _role);
	void OnXiuWeiPetTypeLevelUp(Role * _role);
	void OnXiuWeiParnerTypeLevelUp(Role * _role);

	void OnCommitTreasureTask(Role * role);				// 九州秘宝
	void OnFinishTreasureTask(Role * role);				// 完成一轮九州秘宝

	void OnOriginDisconnectFromCrossServer(int plat_type, int server_id);

	void OnQingYuanJieHun(Role * role, long long qingyuan_id);

	void OnRecvNoticeForUserLoginOrigin(int role_id);
	void OnWorldTeamArenaChangeByRole(Role * role);		// 角色相关信息变更同步英雄会武

	void OnHiddenOpenServerTimeChange();
private:
	INSTANCE_FORBID_COPY_AND_EXTERIAL_CREATE(EventHandler);


	void DropElementFaction(Role* role, bool is_mine_monster, const MonsterCfg* cfg, int& total_drop_element_num);
};

#endif // __EVENT_HANDER_HPP__
