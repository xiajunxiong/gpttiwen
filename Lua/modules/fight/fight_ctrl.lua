
FILE_REQ("modules/fight/fight_config")
FILE_REQ("modules/fight/fight_data")
FILE_REQ("modules/fight/fight_fb_data")
FILE_REQ("modules/fight/fight_data_crystal")
FILE_REQ("modules/fight/fight_data_fantasy")
FILE_REQ("modules/fight/fight_data_splace_anec")
FILE_REQ("modules/fight/fight_data_collect")
FILE_REQ("modules/fight/fight_data_guild")
FILE_REQ("modules/fight/fight_data_pet_island")
VIEW_REQ("modules/fight/board_fight_team")
VIEW_REQ("modules/fight/fight_monster_item")
VIEW_REQ("modules/fight/fight_view")
VIEW_REQ("modules/fight/fight_skill_view")
VIEW_REQ("modules/fight/fight_single_view")
VIEW_REQ("modules/fight/fight_team_view")
VIEW_REQ("modules/fight/fight_crystal_menu_view")
VIEW_REQ("modules/fight/fight_gather_menu_view")
VIEW_REQ("modules/fight/fight_fantasy_menu_view")
VIEW_REQ("modules/fight/fight_medal_menu_view")
VIEW_REQ("modules/fight/fight_fantasy_box_view")
VIEW_REQ("modules/fight/fight_gather_menu_result_view")
VIEW_REQ("modules/fight/fight_map_view")
VIEW_REQ("modules/fight/fight_shop_view")
VIEW_REQ("modules/fight/guild_fb_menu_view")
VIEW_REQ("modules/fight/advanced_fb_menu_view")
VIEW_REQ("modules/fight/fight_auto_seal_set_view")
VIEW_REQ("modules/fight/fight_auto_seal_info_view")
VIEW_REQ("modules/fight/fight_reward_view")
VIEW_REQ("modules/fight/fight_crystal_sweep_view")
VIEW_REQ("modules/fight/fight_crystal_sweep_item_view")
VIEW_REQ("modules/fight/fight_sweep_reward_view")
VIEW_REQ("modules/fight/item_consume_tip")
VIEW_REQ("modules/fight/fight_pet_pray_select")
VIEW_REQ("modules/fight/fight_fb_view")
VIEW_REQ("modules/fight/fight_fb_view_ymz")
VIEW_REQ("modules/fight/fight_fb_view_pet_god")
VIEW_REQ("modules/fight/fight_rare_dungeon_view")
VIEW_REQ("modules/fight/fight_elite_view")
VIEW_REQ("modules/fight/fight_data_soul")
VIEW_REQ("modules/fight/fight_soul_view")
VIEW_REQ("modules/fight/guaji_pl_desc_view")

FightCtrl = FightCtrl or BaseClass(BaseCtrl)

function FightCtrl:__init()
	if FightCtrl.Instance ~= nil then
		Debuger.LogError("[FightCtrl] attempt to create singleton twice!")
		return
	end	
	FightCtrl.Instance = self
	self.data = FightData.New()
	self:RegisterAllProtocols()

	RemindCtrl.Instance:Remind():Register(Mod.Fight.Dungeon, self.data.dungeon_red_points, function ()
		local value = false
		for k, v in pairs(self.data.dungeon_red_points) do
			value = value or v
		end
		return (FunOpen.Instance:GetFunIsOpened(Mod.Fight.Dungeon) and value) and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Fight.Dungeon, self.data.fight_crystal_sweep, function ()
		return self.data:IsFightCrystalEnergyMaxRedPoint() and 1 or 0
	end)
	-- local function chekc_patrol()
	-- 	return (FunOpen.Instance:GetFunIsOpened(Mod.Fight.PatrolTask) and 1 == self.data.patrol_task_info.task_status) and 1 or 0
	-- end
	-- RemindCtrl.Instance:Remind():Register(Mod.Fight.Field, self.data.patrol_task_info, chekc_patrol)
	-- RemindCtrl.Instance:Remind():Register(Mod.Fight.Field, RoleData.Instance:GetBaseData(), chekc_patrol)
	RemindCtrl.Instance:Remind():Register(Mod.Challenge.RareDungeon, self.data.fight_crystal_sweep, function ()
		return self.data:GetRareDungeonRemindNum()
	end)
	RemindCtrl.Instance:Remind():Register(Mod.FightFB.Energy, self.data.fight_crystal_sweep, function ()
		return (self.data.fight_crystal_sweep.role_enter_energy > 30 and RoleData.Instance:GetRoleLevel() > 33) and 1 or 0
	end)
	FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_CRYSTAL_FB,{reward_func = function()
		return self.data:GetCrystalRewardData()
	end})
	--建议使用 PokerCtrl.Instance:OnOpenPokerView(翻牌列表mypoker,type)
	-- FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_CRYSTAL_FB,function()
	-- 	self.data:SetCrystalRewardData()
	-- 	--判断翻牌
	-- 	local info = PokerData.Instance:GetFinishReward()
	-- 	if info.mypoker ~= nil then
	-- 		local all_cant = true
	-- 		local role_id = RoleData.Instance:GetRoleId()
	-- 		for i, v in pairs(info.mypoker) do
	-- 			if v.role_uid == role_id and v.index == -1 then
	-- 				all_cant = false
	-- 				break
	-- 			end
	-- 		end
	-- 		if not all_cant then
	-- 			PokerCtrl.Instance:OpenPokerView(POKER_REQ_OPER_TYPE.CRYSTAL)
	-- 		end
	-- 	end
	-- end)

	FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_FANTASY_FB,{reward_func = function()
		return self.data:GetFantasyRewardData()
	end})
	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_FANTASY_FB,function()
		local req_type , param1 = FightData.Instance:GetFinishPassInFantasy()
		local cur_layer,max_layer = self.data:GetFantasyLayer()
		if cur_layer == max_layer and self.data:GetFantasyCurBossKill() then 
		    self:SendCrystalFbReq(req_type,param1)
		end 
		self.data:SetFantasyRewardData()
	end)
	-- FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_PET_GOD,function(is_win)
	-- 	if is_win == 1 then
	-- 		-- SceneCtrl.Instance:RequestLeaveFb()
	-- 		local info = PokerData.Instance:GetFinishReward()
	-- 		if info.mypoker ~= nil then
	-- 			local all_cant = true
	-- 			local role_id = RoleData.Instance:GetRoleId()
	-- 			for i, v in pairs(info.mypoker) do
	-- 				if v.role_uid == role_id and v.index == -1 then
	-- 					all_cant = false
	-- 					break
	-- 				end
	-- 			end
	-- 			if not all_cant then
	-- 				PokerCtrl.Instance:OpenPokerView(POKER_REQ_OPER_TYPE.PET_GOD)
	-- 			end
	-- 		end
	-- 	end
	-- end)
	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_REALM_BREAK_FIGHT,function(is_win)
		if FinishData.Instance:GetMonsterGroupId() ~= BATTLE_MONSTER_GROUP.FirstRealmBreak then
			if 1 == is_win then
				FightData.Instance:SoulDisappear(function ()
					local func = function()
						MainViewData.Instance:ShowTask()
						local reward_list = self.data:GetFightSoulRewardData()
						if #reward_list > 0 then
							RoleData.Instance.break_info.state = OfflineRewardState.ShowReward
							ViewMgr:OpenView(OfflineRewardView)
						else
							ViewMgr:OpenView(OfflineRewardView)
							ViewMgr:OpenView(FightSoulView)
						end	
					end
					-- 有获得宠物就需要打开宠物获得界面
					if not TableIsEmpty(self.data:GetFightSoulRewardPetData()) then
						ViewMgr:OpenView(PetGain, {is_xilian = false, close_call = func})
						self.data:SetFightSoulRewardPetData()
					else
						func()
					end
				end)
			else
				MainViewData.Instance:ShowTask()
				FightData.Instance:ClearSoulData()
				ViewMgr:OpenView(OfflineRewardView)
				ViewMgr:OpenView(FightSoulView)
			end
		else
			MainViewData.Instance:ShowTask()
			if 1 == is_win and PrefsInt(PrefKeys.FightSoulFirstClear()) == 0 then
				-- GuideManager.Instance:Start(105)				
				PrefsInt(PrefKeys.FightSoulFirstClear(), 1)
			else
				ViewMgr:OpenView(OfflineRewardView)
				ViewMgr:OpenView(FightSoulView)
			end
		end
	end)
	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_DUEL, function(is_win)
		-- 决斗结束后如果再自动采集依然给他自动
		local auto_state = AutoInvokerCtrl.Instance:AutoState()
		if auto_state == AutoCommandState.Gather then
			local config = self.data.gather_scene_list[SceneData.Instance:SceneId()]
			if config ~= nil then
				AutoInvokerCtrl.Instance:SetCommands({AutoCommandGather.New(config.gather_id)}, 0)
			end
		end
    end)
	-- FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_PET_MONOPOLY,function()
	-- 	ViewMgr:OpenView(FightView,{open_param = 4})
 --        --BattleCtrl.Instance:DelayFinish(function()ViewMgr:OpenView(ArenaView)end,true)
 --    end)
	ActivityData.Instance:CustomClickHandle(ActType.FantasyFB, BindTool.Bind(self.GoToFantasyFB, self))
	-- 四象幻境的npc
	BehaviorData.Instance:CustomNpcTalkBtnsId(self.data.fantasy_fb_npc_id, BindTool.Bind(self.SetupBehaviourBtnForFantasyFB, self), true)
	RoleData.Instance:GetBaseData():Care(function ()
		self.data.fight_crystal_sweep.role_level = RoleData.Instance:GetRoleLevel()
	end, "level")

	-- 活跃那边的注册，心灵之境
    ActivityData.Instance:CustomClickHandle(ActType.FightSoul, function()
		ViewMgr:OpenView(FightSoulView)
    end)
    ActivityData.Instance:RegisterCondition(ActType.FightSoul,function()
        local can_fight_num, can_fight_total_num = self.data:GetCanFightAndTotal()
        return Format(Language.Active.Times2, can_fight_num, can_fight_total_num)
	end)

	self.login_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.SetRoleCompleteData, self))
end

function FightCtrl:OnUnloadGameLogic()
	self.data:Clear()
end

function FightCtrl:SetRoleCompleteData()
	self.data:SetRoleCompleteData()
end

function FightCtrl:GoToFantasyFB()
	local act = ActivityData.Instance:GetActivityInfo(ActType.FantasyFB)
    if act and act.info and act.info.fun_name then
        BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(act.info.fun_name, true))
    end
	TaskData.Instance:StopTaskState(TaskConfig.TaskType.dragon, CUSTOM_OBJID.FantasyFB)
end

function FightCtrl:__delete()
	if FightCtrl.Instance == self then
		FightCtrl.Instance = nil
	end
	self:UnRegisterAllProtocols()
	EventSys:UnBind(self.login_handle)
end

function FightCtrl:OnInit()
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Challenge.RareDungeon,function()
		local info = self.data.fight_crystal_sweep
		local num = info.new_xiyou_play_times_remain
		local total = info.new_xiyou_play_times
		return num--,total
        -- ChallengeNaviCtrl.SetFormatDescNum(desc_val,num,total)
    end)
	ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Challenge.FightElite,function()
		local num = self.data:FightFBYMZHaveNumAllElite()
        return num
    end)
end

function FightCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSSetBattleAutoMove)
	self:RegisterProtocol(CSSetAutoCatchPet)
	self:RegisterProtocol(CSCrystalFbFastPassInfo)
	self:RegisterProtocol(CSPetMonopolyOperate)
	self:RegisterProtocol(CSPetGodFBReq)

	self:RegisterProtocol(SCBattleAutoInfo, "OnBattleAutoInfo")
	self:RegisterProtocol(SCCrystalFbInfo, "OnCrystalFbInfo")
	self:RegisterProtocol(SCCrystalFbObjInfo, "OnCrystalFbObjInfo")
	self:RegisterProtocol(SCCrystalFbAllObjInfo, "OnCrystalFbAllObjInfo")

	self:RegisterProtocol(CSCrystalFbReq)
	self:RegisterProtocol(SCCrystalFbUseItemInfo, "OnCrystalFbUseItemInfo")

	self:RegisterProtocol(SCGatherFbInfo, "OnGatherFbInfo")
	self:RegisterProtocol(SCGatherFbGatherListInfo, "OnGatherFbGatherListInfo")

	self:RegisterProtocol(SCGuildFbSceneInfo, "OnGuildFbSceneInfo")

	self:RegisterProtocol(CSExpCrystalOp)
	self:RegisterProtocol(SCExpCrystalInfo, "OnExpCrystalInfo")

	self:RegisterProtocol(SCPatrolTaskInfo, "OnPatrolTaskInfo")

	self:RegisterProtocol(SCAdvanceFbObjInfo, "OnAdvanceFbObjInfo")
	self:RegisterProtocol(SCAdvanceFbTaskInfo, "OnAdvanceFbTaskInfo")

	self:RegisterProtocol(SCFantasyFbSceneInfo, "OnFantasyFbSceneInfo")
	self:RegisterProtocol(SCFantasyFbObjInfo, "OnFantasyFbObjInfo")
	self:RegisterProtocol(SCFantasyFBRewardInfo,"OnFantasyFBRewardInfo")
	self:RegisterProtocol(SCAutoCatchPetInfo,"OnAutoCatchPetInfo")

	self:RegisterProtocol(SCCrystalFbEnergyInfo,"OnCrystalFbEnergyInfo")
	
	self:RegisterProtocol(SCMiJingQiWenFbObjInfo,"OnSCMiJingQiWenFbObjInfo")

	self:RegisterProtocol(SCPetMonopolyInfo, "OnSCPetMonopolyInfo")
	self:RegisterProtocol(SCPetMonopolyExpectPetChangeNotice, "OnSCPetMonopolyExpectPetChangeNotice")
	self:RegisterProtocol(SCPetMonopolyRollDice, "OnSCPetMonopolyRollDice")
	self:RegisterProtocol(SCPetMonopolyCurGridChangeNotice, "OnSCPetMonopolyCurGridChangeNotice")
	self:RegisterProtocol(SCPetMonopolyRewardRecord, "OnSCPetMonopolyRewardRecord")
	self:RegisterProtocol(SCPetMonopolyEnterNextLevelNotice, "OnSCPetMonopolyEnterNextLevelNotice")
	self:RegisterProtocol(SCPetMonopolyTurntable, "OnSCPetMonopolyTurntable")
	self:RegisterProtocol(SCPetMonopolyTurntableResult, "OnSCPetMonopolyTurntableResult")
	self:RegisterProtocol(SCPetMonopolySpecialItem, "OnSCPetMonopolySpecialItem")
	self:RegisterProtocol(SCPetMonopolyWishCardGrid, "OnSCPetMonopolyWishCardGrid")

	self:RegisterProtocol(SCPetGodFBRoleInfo, "OnPetGodFBRoleInfo")
	self:RegisterProtocol(SCPetGodFBSceneInfo, "OnPetGodFBSceneInfo")

	self:RegisterProtocol(SCGuaJiExpInfo, "OnSCGuaJiExpInfo")
	self:RegisterProtocol(SCGuaJiPointsInfo, "OnSCGuaJiPointsInfo")
end

function FightCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCBattleAutoInfo)
	self:UnRegisterProtocol(SCCrystalFbInfo)

	self:UnRegisterProtocol(SCCrystalFbSceneInfo)
	self:UnRegisterProtocol(SCCrystalFbObjInfo)
	self:UnRegisterProtocol(SCCrystalFbAllObjInfo)
	self:UnRegisterProtocol(CSCrystalFbReq)
	self:UnRegisterProtocol(SCCrystalFbUseItemInfo)
	self:UnRegisterProtocol(SCGatherFbInfo)
	self:UnRegisterProtocol(SCAdvanceFbObjInfo)
	self:UnRegisterProtocol(CSSetAutoCatchPet)
	self:UnRegisterProtocol(SCAutoCatchPetInfo)
end

function FightCtrl:OnBattleAutoInfo(protocol)
	self.data:SetBattleAutoInfo(protocol)
end

function FightCtrl:OnCrystalFbInfo(protocol)
	self.data:SetCrystalFbInfo(protocol)
end

function FightCtrl:OnCrystalFbObjInfo(protocol)
	SceneData.Instance:SetCustomSceneName(SceneData.Instance:SceneId(), string.format(Language.Fight.CrystalDungen.layer, SceneData.Instance:SceneName(), DataHelper.GetDaXie(protocol.fight_crystal_info.layer)))
	self.data:SetCrystalFbSceneInfo(protocol.fight_crystal_info)
	self.data:SetCrystalFbObjInfo(protocol.obj_list)
end

function FightCtrl:OnCrystalFbAllObjInfo(protocol)
	self.data:SetCrystalFbAllObjInfo(protocol)
end

function FightCtrl:OnGatherFbInfo(protocol)
	self.data:SetGatherFbInfo(protocol)
end

function FightCtrl:OnGatherFbSceneInfo(protocol)
	self.data:SetGatherFbSceneInfo(protocol)
end

function FightCtrl:OnGatherFbGatherRemoveInfo(protocol)
	VoMgr:DelSNpc(protocol.obj_id)
	self.data:SetGatherFbGatherRemoveInfo(protocol)
end

function FightCtrl:OnGatherFbGatherListInfo(protocol)
	self.data:SetGatherFbGatherListInfo(protocol)
end

function FightCtrl:SendSetBattleAutoMove(param_t)
	local protocol = GetProtocol(CSSetBattleAutoMove)
	param_t = param_t or {}
	param_t[1] = param_t[1] or self.data.skill_protocol
	param_t[2] = param_t[2] or self.data.skill_protocol
	protocol.first_move_type = param_t[1].first_move_type
	protocol.first_skill_id = param_t[1].first_skill_id
	protocol.first_skill_level = param_t[1].first_skill_level
	protocol.second_move_type = param_t[2].second_move_type
	protocol.second_skill_id = param_t[2].second_skill_id
	protocol.second_skill_level = param_t[2].second_skill_level
	SendProtocol(protocol)
end

function FightCtrl:SendSetAutoCatchPet(param_t)
	local protocol = GetProtocol(CSSetAutoCatchPet)
	param_t = param_t or {}
	protocol.scene_id = param_t.scene_id or 0
	protocol.max_seal_card_common = param_t.max_seal_card_common or 0
	protocol.max_seal_card_silver = param_t.max_seal_card_silver or 0
	protocol.max_seal_card_gold = param_t.max_seal_card_gold or 0
	protocol.auto_catch_info_num = param_t.auto_catch_info_num or 0
	protocol.setting_list = param_t.setting_list or {}
	SendProtocol(protocol)
end

function FightCtrl:SendCrystalFbFastPassInfo(fb_seq, times)
	local protocol = GetProtocol(CSCrystalFbFastPassInfo)
	protocol.fb_seq = fb_seq
	protocol.times = times
	SendProtocol(protocol)
end

function FightCtrl:SendPetGodFBReq(req_type, param_1)
	local protocol = GetProtocol(CSPetGodFBReq)
	protocol.req_type = req_type
	protocol.param_1 = param_1 or 0
	SendProtocol(protocol)
end

function FightCtrl:OnAutoCatchPetInfo(protocol)
	self.data:SetAutoSealFlag(false)
	self.data:SetAutoSealInfo(protocol)
	if not self.data:IsSetAutoSealPet() then
		self.data:SetAutoSealData({})
	end
end

function FightCtrl:OnCrystalFbEnergyInfo(protocol)
	self.data:SetCrystalFbEnergyInfo(protocol)
end

function FightCtrl:OnPetGodFBRoleInfo(protocol)
	self.data:SetPetGodFBRoleInfo(protocol)
end

function FightCtrl:OnPetGodFBSceneInfo(protocol)
	self.data:SetPetGodFBSceneInfo(protocol)
end

function FightCtrl:OnSCMiJingQiWenFbObjInfo(protocol)
	-- LogError("?OnSCMiJingQiWenFbObjInfo?",protocol)
	self.data:SetSplaceAnecInfo(protocol)
end

function FightCtrl:OnSCGuaJiExpInfo(protocol)
	-- self.data:OnSCGuaJiExpInfo(protocol)
end
function FightCtrl:OnSCGuaJiPointsInfo(protocol)
	self.data:OnSCGuaJiExpInfo(protocol)
end
--检测自动封印进度是否完成
function FightCtrl:CheckAutoSealAccomplish()
	local is_accom = self.data:CheckAccomplishAutoSeal()
	if is_accom then
		ViewMgr:OpenView(FightAutoSealInfoView,{open_info_type = FightData.OPEN_AUTO_SEAL_INFO_TYPE.ACCOMPLISH})
	end
end

--打开自动封印成果信息界面
function FightCtrl:OpenAutoSealInfoView(open_type)
	open_type = open_type or FightData.OPEN_AUTO_SEAL_INFO_TYPE.LOOK
	ViewMgr:OpenView(FightAutoSealInfoView,{open_info_type = open_type})
end

function FightCtrl:GetPveCrystalUseFunc(item_id)
	return self.data.dungeon_consume_items[item_id]
end

function FightCtrl:OnGuildFbSceneInfo(protocol)
	self.data:SetGuildFBInfo(protocol)
end

function FightCtrl:OnCrystalFbUseItemInfo(protocol)
	self.data:SetCrystalFbUseItemInfo(protocol)
end

function FightCtrl:SendCrystalFbReq(req_type,param1)
	local protocol = GetProtocol(CSCrystalFbReq)
	protocol.req_type = req_type or 0
	protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

function FightCtrl:OnExpCrystalInfo(protocol)
	self.data:SetExpCrystalInfo(protocol)
end

function FightCtrl:SendExpCrystalOp()
	local protocol = GetProtocol(CSExpCrystalOp)
	SendProtocol(protocol)
end

function FightCtrl:OnPatrolTaskInfo(protocol)
	self.data:SetPatrolTaskInfo(protocol)
	ActivityData.Instance:SetActivityFinishInfo(ActType.Patrol, protocol.info.task_progress, self.data:PatrolTaskTotal())
end

function FightCtrl:OnAdvanceFbObjInfo(protocol)
	self.data:SetAdvanceFbObjInfo(protocol)
end

function FightCtrl:OnAdvanceFbTaskInfo(protocol)
	self.data:SetAdvanceFbTaskInfo(protocol)
	RoleData.Instance:ChangePromoteRedPiont()
end

function FightCtrl:OnPveCrystalUseFunc()
	local item = ItemInfoCtrl.Instance:Data():GetItemData()
	local click_hanle = self:GetPveCrystalUseFunc(item.item_id)
	if click_hanle then
		click_hanle()
		ViewMgr:CloseView(BagView)
	end
end

-- 四象幻境
function FightCtrl:OnFantasyFbSceneInfo(protocol)
	self.data:SetFantasyFbSceneInfo(protocol)
end

function FightCtrl:OnFantasyFbObjInfo(protocol)
	self.data:SetFantasyFbObjInfo(protocol)
end

function FightCtrl:OnFantasyFBRewardInfo(protocol)
	self.data:SetFantasyFBRewardInfo(protocol)
	ActivityData.Instance:SetActivityFinishInfo(ActType.FantasyFB,self.data:GetTodayFantasyFetch(), 4)
	if nil ~= self.dargon_task_role_level_care then
        RoleData.Instance.base_data:Uncare(self.dargon_task_role_level_care)
    end
	if self.data:GetTodayFantasyFetch() == 4 then
        TaskCtrl.Instance.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.FantasyFB)
	else
        local act_info = ActivityData.Instance:GetActivityInfo(ActType.FantasyFB)
        local task = TaskInfoDragon.New():TaskInfo()
		local task_vo = CommonStruct.CustomTask()
		task_vo.task_id = CUSTOM_OBJID.FantasyFB
		task_vo.task_name = Language.Task.DragNames[CUSTOM_OBJID.FantasyFB]
		task_vo.reward_list = FightData.Instance:GetFantasyPreReward()
		local desc_show = string.format("%s(%s/%s)", Language.Task.DragDescs[CUSTOM_OBJID.FantasyFB], self.data:GetTodayFantasyFetch(), 4)
        task_vo.task_desc = desc_show
        task_vo.task_goal = desc_show
		task:TaskVo(task_vo)
		self.dargon_task_role_level_care = task:DragonTaskParam(function ()
			if TeamData.Instance:InTeam() then
				local info = TeamData.Instance:GetTeamInfo()
				if info.info.team_type == 10 and TeamData.Instance:MemberNumRole() >= 3 then
					if TeamCtrl.Instance.guide_dragon == nil then
						TeamCtrl.Instance.guide_dragon = GuideDragon.New()
					end
					TeamCtrl.Instance.guide_dragon:Start()
				else
					TeamCtrl.Instance:TeamDetectByActType(ActType.FantasyFB,function ()
					end, nil, 11)
					if TeamCtrl.Instance.guide_dragon == nil then
						TeamCtrl.Instance.guide_dragon = GuideDragon.New()
					end
					TimeHelper:CancelTimer(self.handle_delay)
                    self.handle_delay = TimeHelper:AddDelayTimer(function ()
                        TeamCtrl.Instance.guide_dragon:Start()
                    end, 0.2)
					--[[ TeamCtrl.Instance:TeamDetectByActType(ActType.FantasyFB,function ()
					end, nil, 11) ]]
				end
			else
				TeamCtrl.Instance:TeamDetectByActType(ActType.FantasyFB,function ()
				end, nil, 11)
			end
        end, act_info:LevelRestrict())
        TaskData.Instance:TaskInfoUpdateByTaskId(CUSTOM_OBJID.FantasyFB, SmartData.New(task), true)
	end
	-- TaskData.Instance:DragonTaskTrace()
end

function FightCtrl:OpenPetIsland()
	ViewMgr:OpenView(FightFBView, {open_param = 3})
end

--灵宠秘境信息下发
function FightCtrl:OnSCPetMonopolyInfo(protocol)
	self.data:SetPetMonopolyInfo(protocol)
	--请求当前层转盘信息
	FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_SHOW_TRUNTABLE_REWARDS)
end
--灵宠秘境操作请求
-- FIGHT_PET_ISLAND_OPER_TYPE
-- {
--     OPER_TYPE_INFO = 0,             //!< 请求棋盘信息
--     OPER_TYPE_REWARD_RECORD = 1,    //!< 查看奖励
--     OPER_TYPE_SET_EXPECT_PET = 2,   //!< 设置祈福宠物 param1 宠物ID
--     OPER_TYPE_CLEAR_EXPECT_PET = 3, //!< 清除祈福宠物 param1 是否金宠(1:是，0:否）
--     OPER_TYPE_ROLL_DICE = 4,        //!< 摇骰子
--     OPER_TYPE_FETCH_GRID_REWARD = 5,//!< 获取当前格子的奖励
--     OPER_TYPE_START_PET_BATTLE = 6, //!< 开始抓宠战斗
--     OPER_TYPE_TURN_TABLE = 7,       //!< 开始转盘
--     OPER_TYPE_ENTER_NEXT_LEVEL = 8, //!< 进入下一层
--     OPER_TYPE_SHOW_TRUNTABLE_REWARDS = 9, --//!< 查看转盘奖励
--     OPER_TYPE_TURN_TABLE_END = 10, --//!< 转盘动画结束
	-- OPER_TYPE_ADD_EXP = 11,			--加经验
	-- OPER_TYPE_SPECIAL_ITEM = 12, 	--//!< 使用特殊道具 param1:道具类型 param2:使用道具附加（鲁班骰点数/心愿卡选择的物品id）
	-- OPER_TYPE_SPECIAL_ITEM_NUM = 13, --//!< 获取全部特殊道具数量
	-- OPER_TYPE_FAST_MOVE_REWARD = 14, --//!< 使用神行符申请奖励
-- };
--灵宠秘境操作请求
function FightCtrl.SendPetMonopolyOperate(op_type, param1, param2)
	-- LogError("send op",op_type, param1, param2)
	local protocol = GetProtocol(CSPetMonopolyOperate)
	protocol.operate_type = op_type
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

--灵宠秘境祈福返回
function FightCtrl:OnSCPetMonopolyExpectPetChangeNotice(protocol)
	self.data:SetPetMonopolyPrayResult(protocol)
	PublicPopupCtrl.Instance:Center(Language.Fight.PetIslandPrayTip)
end

--灵宠秘境投骰子结果返回
function FightCtrl:OnSCPetMonopolyRollDice(protocol)
	--LogError("Dice protocol",protocol)
	if protocol.cur_grid_index == PET_MONOPOLY_GRID_NUM - 1 then
		self.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_SHOW_TRUNTABLE_REWARDS)
	end
	self.data:SetPetMonopolyRollDice(protocol)
end

--灵宠秘境查看奖励返回
function FightCtrl:OnSCPetMonopolyRewardRecord(protocol)
	self.data:SetPetMonopolyRewardRecord(protocol)
end

--灵宠秘境进入下一层通知
function FightCtrl:OnSCPetMonopolyEnterNextLevelNotice(protocol)
	self.data:SetPetMonopolyEnterNextLevel(protocol)
end

--灵宠秘境当前格点内容改变通知
function FightCtrl:OnSCPetMonopolyCurGridChangeNotice(protocol)
	self.data:SetPetMonopolyGridInfo(protocol)
end
-- 转盘信息
function FightCtrl:OnSCPetMonopolyTurntable(protocol)
	self.data:SetPetMonopolyTurntableInfo(protocol)
end

-- 转盘结果下发
function FightCtrl:OnSCPetMonopolyTurntableResult(protocol)
	self.data:SetPetMonopolyTurntableResult(protocol)
end
--使用道具结果下发
function FightCtrl:OnSCPetMonopolySpecialItem(protocol)
	-- LogError("UseSpecialItem",protocol)
	if self.data.special_item_group ~= nil then
		for k, v in pairs(self.data.special_item_group) do
			if v < protocol.special_item_group[k] then
				local special_item_cfg = FightData.GetPetMonopolySpecialItemConfig(k)
				if special_item_cfg then
					PublicPopupCtrl.Instance:Center(string.format(Language.Fight.PetIslandGetSpecial, special_item_cfg.name, protocol.special_item_group[k] - v))
				end
			end
		end
	end
	self.data:SetPetMonopolySpecialItem(protocol)
	if protocol.use_item_type == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_DOUBLECARD
		or protocol.use_item_type == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_WISHCARD then
		PublicPopupCtrl.Instance:Center(Language.Fight.PetIslandSpecialNotice[protocol.use_item_type])
	end
end
--使用心愿卡后变化的格子信息
function FightCtrl:OnSCPetMonopolyWishCardGrid(protocol)
	self.data:SetPetMonopolyWishCardGrid(protocol)
end

function FightCtrl:FantasyAutoGo(param_t)
	-- 读取结算数据，如果存在战败信息则停止本次自动寻路
	if param_t and param_t.check_win and FinishData.Instance:GetFinishInfo().is_win and FinishData.Instance:GetFinishInfo().is_win == 0 then 
		return 
	end 

	local config = self.data:GetFantasyLayerInfo()
	if config == nil then return end 
	if MainPlayer and not MainPlayer:IsInAnim(ChAnimType.Idle) then 
		return 
	end 
	if not self.data:GetFantasyCurBossKill() then 
		SceneLogic.Instance:AutoToNpc({x = config.x,y = config.y,scene_id = config.scene_id,id = config.boss_id},nil,true)
	else
		SceneLogic.Instance:AutoToPos(config.scene_id,{x = config.pos_x, y = config.pos_y},nil)
	end 
end

--检查四象幻境
function FightCtrl:FightFantasyAuto(param_t)
    if SceneData.Instance:GetMode() ~= SceneModeType.FightFantasy then
        return
    end
    if not TeamData.Instance:IsLeader() then
        return
    end
    if FightData.Instance.is_auto_fantasy then
        return
    end
    local start_time = Config.monster_dreamland_auto.other[1].auto_wait
	TimeHelper:AddDelayTimer(function()
		FightCtrl.Instance:FantasyAutoGo(param_t)
	end,start_time)
end


-- 四象幻境增加一键操作
function FightCtrl:SetupBehaviourBtnForFantasyFB(btn_list,obj_vo)
	if obj_vo and obj_vo.id == self.data.fantasy_fb_npc_id and btn_list then
		for i,v in ipairs(btn_list) do
			if v.detail_info ~= nil then
				self:SetupBehaviourBtnForFantasyFBDetail(btn_list[i])
				break
			end
		end
	end 
end

function FightCtrl:SetupBehaviourBtnForFantasyFBDetail(params)
    local fantasy_cfg = Cfg.RewardFindBackbyType(FindType.FantasyFB)
    local cost_type = self.data:FantasyCostType()
    local time = 4 - self.data:GetTodayFantasyFetch()
	local reward_price = RewardData.Instance:GetRewardActivityBackFindPrice(7)
    local cost = fantasy_cfg.gold_price * (time)
    if cost_type == 0 then
        cost = reward_price * (time)
    end
    if fantasy_cfg == nil then
        return
    end
    params.detail_info.act_type = ActType.FantasyFB
    params.detail_info.custom_type = DetailInfo1Custom.back
    params.detail_info.custom_show = Language.Task.TaskBtn.OneKey
    if time == 0 then
        params.detail_info.custom_func = function()
            PublicPopupCtrl.Instance:Center(ErrorText[73])
        end
    else
        params.detail_info.custom_func = BindTool.Bind(ViewManager.OpenView,ViewMgr,DialogTipsBackView,
            {
                cost = cost,
                cost_type = cost_type,
                name = Language.Task.FantasyFb,
                rewards = self.data:FantasyReward(),
                confirm_func = BindTool.Bind(TaskCtrl.SendOnekeyFinish, TaskCtrl.Instance, OnekeyFinishType.FantasyFB)
            }
        )
    end
end

--清除自动抓宠数据
function FightCtrl:ClearAutoSealData()
	self.data:SetAutoSealData({})
	self:SendSetAutoCatchPet({})    
	--取消自动抓宠
	self.data:SetAutoSealFlag(false)
	self.data:SetSealing(false)
end

--副本退出提示
function FightCtrl.IsRequestFb(request_func,scene_id)
	local mode = SceneData.Instance:GetMode()
	if FightCtrl.IS_IN_FIGHTFB == nil and FightConfig.IS_IN_FIGHTFB[mode] then
		if mode == SceneModeType.FightGather then
			FightData.Instance:SetCollectGatherList({})
			MainOtherCtrl.Instance:CloseGather()
		end
		if scene_id ~= nil and scene_id ~= 0 then
			local cfg = SceneData.Instance:GetSceneCfg(scene_id)
			local scene_type = cfg.scene_type or 0
			if scene_type == mode then
				return false
			end
		end
		PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackTips,function()
			FightCtrl.IS_IN_FIGHTFB = true
			request_func()
			FightCtrl.IS_IN_FIGHTFB = nil
		end)
		return true
	elseif not FightConfig.IS_IN_FIGHTFB[mode] and mode ~= SceneModeType.None then
		if scene_id ~= nil and scene_id ~= 0 then
			local cfg = SceneData.Instance:GetSceneCfg(scene_id)
			local scene_type = cfg.scene_type or 0
			if scene_type == mode then
				return false
			end
		end
		PublicPopupCtrl.Instance:Center(Language.Chat.BackActTips)
		return true
	end
end
function FightCtrl.IsActFB()
	local mode = SceneData.Instance:GetMode()
	if FightConfig.IS_IN_ACTFB[mode] then
		PublicPopupCtrl.Instance:Center(Language.Chat.BackActTips)
		return true
	end
	return false	
end

--副本获取退出警告提示
function FightCtrl.GetWarnTips(mode)
	if FightConfig.IS_IN_ONCE[mode] then
		local act_type = FightConfig.IS_IN_ONCE[mode]
		if ActivityData.Instance:GetActivityStatus(act_type) == ActStatusType.Open then
			if ChiefCtrl.Instance:IsEndChiefPrimary(mode) then
				return ""
			end
			if ActivityCtrl.Instance:IsEndOtherTip(act_type) then
				return ""
			end
			return Language.Chat.WarningTips
		end
	elseif mode == SceneModeType.FightCrystal then
		return FightData.Instance:GetCompleteBackTips()
	end
	return ""
end

function FightCtrl:GuideEnterSeal()
	TimeHelper:CancelTimer(self.guide_seal_ht)
	self.guide_seal_ht = Invoke(function ()
		RoleSettingData.Instance.shield_fight_monster = false
		SceneLogic.Instance.move_to_pos_cache.guaji = false
		RoleCtrl.Instance:SendFuncGuideOperator(FUNCTIONGUIDEOPTYPE.AUTO_SEAL_PET,0)
	end,5)
end

-- 判断是否组队的时候进入单人本，然后弹框提示是否离队 
-- 提示框用AlertTip，别再用DialogTips
function FightCtrl:IsSingleFight(request_func)
	if TeamCtrl.Instance:Data():InTeam() then
        PublicPopupCtrl.Instance:AlertTip(Language.Fight.NeedSingle,function()
            TeamCtrl.Instance:SendExitTeam()
            request_func()
        end)
        return true
    end
    request_func()
end