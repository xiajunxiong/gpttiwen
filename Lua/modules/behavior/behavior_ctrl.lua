FILE_REQ("modules/behavior/behavior_data")
FILE_REQ("modules/behavior/touch_behavior")
BehaviorCtrl = BehaviorCtrl or BaseClass(BaseCtrl)
Behavior = nil
function BehaviorCtrl:__init()
    if BehaviorCtrl.Instance ~= nil then
        Debuger.LogError("[BehaviorCtrl] attempt to create singleton twice!")
        return
    end
    BehaviorCtrl.Instance = self
    Behavior = self
	self.touch_behavior = TouchBehavior.New()
	self.data = BehaviorData.New()
end
function BehaviorCtrl:__delete()
    self.touch_behavior:DeleteMe()
end


BehaviorCtrl.InSceneObjInteraction = false


InteractionNpcTalk = InteractionNpcTalk or {}

--param1:TalkId
--param2:按钮文本,|号隔开
--param3:按钮行为id,|号隔开
--task_info(dynamic param)
--talk_list在野外Boss要用到,因为存在多npc_id的野外boss说同样的话
--beh_btn_list自定义按钮
function InteractionNpcTalk.Exe(beh_cfg, obj_vo)
	NpcDialogData.Instance:SetCurNpcSeq(obj_vo and obj_vo.seq or 0)
    local param_t = {talk_id = beh_cfg.param1, beh_btn_list = beh_cfg.beh_btn_list or {}, talk_list = beh_cfg.talk_list}
    if BehaviorCtrl.InSceneObjInteraction and obj_vo then
        local task_infos = TaskCtrl.Instance:GetNpcDialogTaskButtonListByNpcId(obj_vo.id)
        for _, t_in in pairs(task_infos) do
			if task_infos[1] and task_infos[1].IsFirstBountyTask and task_infos[1]:IsFirstBountyTask() then
    		beh_cfg = TableCopy(beh_cfg)
    		beh_cfg.task_info = task_infos[1]
		end
            if UN_SHOW_TASK_BUTTON[ t_in.task_type] then
			else
				table.insert(param_t.beh_btn_list, NpcDialogBtnTask.New(obj_vo, t_in))	
			end
        end
    end
	local btn_strs = string.split(beh_cfg.param2, "|")
	local btn_behid_strs = string.split(beh_cfg.param3, "|")
	for i=1,#btn_behid_strs do
		if tonumber(btn_behid_strs[i]) ~= 0 then
			table.insert(param_t.beh_btn_list,NpcDialogBehBtnData.BehIdCreate(
				btn_strs[i],tonumber(btn_behid_strs[i]) or 0,obj_vo))
		end
	end
	local team_btn_co = Config.npc_behavior_auto.team_btn[beh_cfg.param3]
	if team_btn_co then
		if beh_cfg.task_info and beh_cfg.task_info:IsSkipTalk() then
			param_t.is_skip = true
		end
		local button = NpcDialogBehBtnData.FuncCreate(
				team_btn_co.name,
				function ()
					local target = TeamData.Instance:GetTeamGoalWithId(team_btn_co.target_id or 10)
					if LoginData.Instance:IsOnCrossSever() and TeamConfig.CrossType.not_cross == target.is_cross then
						PublicPopupCtrl.Instance:Center(Language.Team.MatchCrossError)
						return
					end
					if RoleData.Instance:GetRoleLevel() < target.low_level then
						PublicPopupCtrl.Instance:Center(Language.Team.MatchLevelError)
						return
					end
					if TeamData.Instance:InTeam() then
						TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
						ViewMgr:OpenView(TeamView)
					else
						TeamData.Instance.cur_platform_target_id = target.target_id
						PublicPopupCtrl.Instance:Center(string.format(Language.Team.NumberNotEnough, team_btn_co.recommend_num))
						TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
					end
				end,
				true
			)
		table.insert(param_t.beh_btn_list, button)
	end
	--重写NPC对话按钮
	if not (beh_cfg.task_info and beh_cfg.task_info:IsShowTaskButton()) then
		param_t.beh_btn_list = BehaviorData.Instance:SetupBehaviourBtn(param_t.beh_btn_list,obj_vo) or param_t.beh_btn_list
	end
	
	if obj_vo and not BigDipperData.Instance:BossIsOpenBySeq(obj_vo.seq) then
		param_t.beh_btn_list = nil
	end

	--重写对话
	param_t.talk_list = BehaviorData.Instance:SetupTalkList(param_t.talk_list,obj_vo) or param_t.talk_list

    if beh_cfg.task_info ~= nil then
	    NpcDialogCtrl.Instance.data:SetCurTaskInfo(beh_cfg.task_info)
	end
	if param_t.talk_id == 5545 and RushDungeonData.Instance:MinLevel() <= 1 then
		param_t.talk_id = 5546
	end
	if BattleData.Instance:IsServerFighting() then
		return
	end
	if AnecdoteCtrl.IsHasTaskInfo(param_t,obj_vo) then 
		return
	end
	-- 山海灯会特殊处理对话
	if param_t.talk_id == 11050 and MSLanternData.Instance:IsTimesEnought() then
		param_t.talk_id = MSLanternData.Instance:GetBattleTalkId()
	end
	-- 科举考试特殊处理对话
	if param_t.talk_id == 11100 and ImperialExamData.Instance:IsTimesEnought() then
		param_t.talk_id = ImperialExamData.Instance:GetBattleTalkId()
	end

	-- --重写NPC对话行为（自定义NPC行为）
	-- if BehaviorData.Instance:SetupNpcDialogBeh(param_t,obj_vo) then
	-- 	return
	-- end
	local end_func = nil
	--[[ if obj_vo and obj_vo.obj_type == SceneObjType.YinYangObj then
		end_func = function ()
			YinYangCtrl.CSYinYangEyeOpReq(obj_vo.show_type == 0 and 1 or 2, obj_vo.seq)
		end
	end ]]
	NpcDialogCtrl.Instance:NpcDialogView(param_t,end_func,true, obj_vo and obj_vo.id or nil)
end

InteractionNpcTalk2 = InteractionNpcTalk2 or {}

function InteractionNpcTalk2.Exe(beh_cfg,obj_vo)
	NpcDialogData.Instance:SetCurNpcSeq(obj_vo and obj_vo.seq or 0)
	local param_t = {talk_id = beh_cfg.param1,beh_btn_list = {},talk_list = beh_cfg.talk_list}
	local param2 = beh_cfg.param2
	if string.find(param2, ":") then
		param2 = string.gsub(param2, ":", "|")
	end
	local btn_ids = string.split(param2, "|")
	for i=1, #btn_ids do
		local co = Cfg.NpcButtonListByButtonId(tonumber(btn_ids[i]))
		if co then
			local button = NpcDialogBehBtnData.BehIdCreate(co.show_word, co.button_behavior or 0, obj_vo)
			if (12 == co.icon_type or 13 == co.icon_type) and co.target_id > 0 then
				button.detail_info = NpcDialogData.CreateDetailInfo1{custom_type = DetailInfo1Custom.quick_team, tips = Language.Fight.DetailTips, custom_func = function ()
					local target = TeamData.Instance.team_goal_list[co.target_id]
					ViewMgr:CloseView(NpcDialogView)
					if RoleData.Instance:GetRoleLevel() < target.low_level then
						PublicPopupCtrl.Instance:Center(Language.Team.MatchLevelError)
						return
					end
					if TeamData.Instance:InTeam() then
						ViewMgr:OpenView(TeamView)
						TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
						TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
					else
						TeamData.Instance.cur_platform_target_id = target.target_id
						TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
					end
				end, custom_show = Language.Team.FastTeam, rewards = FightData.Instance:GetButtonRewardsBySceneSeq(co.param_1)}
			end
			button:SetIconType(co.icon_type)
			table.insert(param_t.beh_btn_list, button)
		end
	end
	param_t.beh_btn_list = BehaviorData.Instance:SetupBehaviourBtn(param_t.beh_btn_list,obj_vo) or param_t.beh_btn_list
	
	param_t.talk_list = BehaviorData.Instance:SetupTalkList(param_t.talk_list,obj_vo) or param_t.talk_list

	if beh_cfg.task_info ~= nil then
	    NpcDialogCtrl.Instance.data:SetCurTaskInfo(beh_cfg.task_info)
	end
	if not BattleData.Instance:IsServerFighting() then
		NpcDialogCtrl.Instance:NpcDialogView(param_t)
	end
end
InteractionNpcTalk3 = InteractionNpcTalk3 or {}
function InteractionNpcTalk3.Exe(beh_cfg,obj_vo)
	NpcDialogData.Instance:SetCurNpcSeq(obj_vo and obj_vo.seq or 0)
	local new_param_t = {
        talk_id = beh_cfg.param1,
        not_block_close_func = true
    }
	NpcDialogCtrl.Instance:NpcDialogView(
		new_param_t,
    function()
        local param_t = NpcDialogData.Instance.cahce_chck_data
        if param_t then
            NpcDialogCtrl.Instance:NpcDialogView(param_t)
        end
    end, true)
end

InteractionGather = InteractionGather or {}

--param1 采集描述类型 param2 采集时间 param3 行为id（非服务端采集完成可加）
function InteractionGather.Exe(beh_cfg, obj_vo)
	local finish_beh
	local param3_is_empty = nil == beh_cfg.param3 or 0 == beh_cfg.param3
	if not param3_is_empty then
		finish_beh = tonumber(beh_cfg.param3)
	end
    local param_t = {
		finish_func = function()
			-- VoMgr:DeleteNpc(obj_vo)
			if finish_beh then
				BehaviorCtrl.Exe(finish_beh, obj_vo)
			end
		end,
		desc = beh_cfg.param1,
		time = beh_cfg.param2,
		seq =  obj_vo.seq,
		obj_id = obj_vo.obj_id,
		id = obj_vo.id,
	}
	param_t.seq = param3_is_empty and param_t.seq or nil
	MainOtherCtrl.Instance:OpenGather(param_t)
	if 45001 == obj_vo.id and beh_cfg.task_info then
		-- 主线任务试炼采集(血脉测试)时，触发特效
		if beh_cfg.task_info:TaskEffect() then return end
		local client_objs = SceneCtrl.Instance:Objs().client_objs
		local gather = client_objs[obj_vo.obj_id]
		SE:Play(SceneEffectConfig.EffectList.TaskGatherRole, nil, nil, MainPlayer.root_node)
		if gather then
			SE:Play(SceneEffectConfig.EffectList.TaskGatherMono, gather:GetPos())
		end
	end
end

InteractionEnterBattle = InteractionEnterBattle or {}
--以行为配置为主
--param1:monster group id
--param2:如果有队伍限制则配target_id 例如世界boss
--param3：队伍人数
function InteractionEnterBattle.Exe(beh_cfg,obj_vo)
	if not StringIsEmpty(beh_cfg.param2) and beh_cfg.param2 > 0 then
		-- 家族任务挑战逻辑块
		if GuildData.Instance:CheckIsCooBattle(beh_cfg.param1) then 
			local team_limit_type = GuildData.Instance:GetCooBattleTeamLimit()
			local type_limit =  GuildData.Instance:GetCooBattleTypeLimit()

			if not TeamData.Instance:InTeam() and team_limit_type == 2 then 
				local end_func = function ()
					if TeamCtrl.Instance:Data():InTeam() then
						if not TeamCtrl.Instance:Data():IsLeader() then
							PublicPopupCtrl.Instance:Center(ErrorText[57])
							return
						end
					end
					local cfg_tar=TeamData.Instance.team_goal_list[beh_cfg.param2]
					if type_limit then 
						GuildCtrl.SendGuildCooperateReq({req_type = 5})
					else 
						if NPC_FIGHT_EXA_PARAM_TASKID[cfg_tar.team_type] then 
							SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1,cfg_tar.task_id)
						else
							SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
						end
					end 
				end
				local re = TeamCtrl.Instance:TeamDetectByTargetId(beh_cfg.param2, end_func, tonumber(beh_cfg.param3), end_func)
				return 
			elseif TeamData.Instance:InTeam() and team_limit_type == 2 and TeamData.Instance:MemberNumRole()< beh_cfg.param3  then 
				local end_func = function ()
					if TeamCtrl.Instance:Data():InTeam() then
						if not TeamCtrl.Instance:Data():IsLeader() then
							PublicPopupCtrl.Instance:Center(ErrorText[57])
							return
						end
					end
					local cfg_tar=TeamData.Instance.team_goal_list[beh_cfg.param2]
					if type_limit then 
						GuildCtrl.SendGuildCooperateReq({req_type = 5})
					else
						if NPC_FIGHT_EXA_PARAM_TASKID[cfg_tar.team_type] then 
							SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1,cfg_tar.task_id)
						else
							SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
						end
					end 
					
				end
				local re = TeamCtrl.Instance:TeamDetectByTargetId(beh_cfg.param2, end_func, tonumber(beh_cfg.param3), end_func)
				return 
			else
				GuildCtrl.SendGuildCooperateReq({req_type = 5})
				return 
			end 
		end

		-- 原挑战逻辑块
		if TeamData.Instance:IsChallengeTaskTarget(beh_cfg.param2) then
			if TeamCtrl.Instance:Data():InTeam() then
				PublicPopupCtrl.Instance:DialogTips{
					content = Language.Team.ChallengeTaskCheck,
					confirm = {
						func =  function()
							TeamCtrl.Instance:SendExitTeam()
							SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
							ViewMgr:CloseView(DialogTipsView)
						end 
					},
				}
			else
				SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
			end
		else
			local end_func = function ()
				if TeamCtrl.Instance:Data():InTeam() then
					if not TeamCtrl.Instance:Data():IsLeader() then
						PublicPopupCtrl.Instance:Center(ErrorText[57])
						return
					end
				end
				local cfg_tar=TeamData.Instance.team_goal_list[beh_cfg.param2]
				if NPC_FIGHT_EXA_PARAM_TASKID[cfg_tar.team_type] then 
					SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1,cfg_tar.task_id)
				else
					SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
				end
			end
			if beh_cfg.param2 == 8 then
				local config = FightFieldBossData.Instance:GetBossConfigById(beh_cfg.id)
				if config then
					local is_wild = FightFieldBossData.Instance:IsSceneWild(beh_cfg.id)
					if not is_wild then
						if TeamCtrl.Instance:Data():InTeam() then
							local param_t = {
							content = Language.FightFieldBoss.ChallengeInTeam,
							confirm = {
									func = function()
										TeamCtrl.Instance:SendExitTeam()
										SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
										ViewMgr:CloseView(DialogTipsView)
									end
								},
							}
							PublicPopupCtrl.Instance:DialogTips(param_t)    
						else
							SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
						end
						return
					end
				end
			end
			local re = TeamCtrl.Instance:TeamDetectByTargetId(beh_cfg.param2, end_func, tonumber(beh_cfg.param3), end_func)
		end
		return
	elseif obj_vo and obj_vo.id == ServerActivityData.Instance:GetNianOther().npc_id 
		and not TeamData.Instance:InTeam() then 
		local item = Item.New({item_id = ServerActivityData.Instance:GetNianOther().item_id})
			local param_t = {
				content =  string.format(Language.Team.ItemCheckSelf,item:ColorStr(),item:Name()),
				cancel = {
					name = Language.Nian.BtnBuyTip,
					func = function ()
						ViewMgr:OpenView(LustrateNianView)
						ViewMgr:CloseView(DialogTipsView)
						return
					end
				},
				confirm = {
					func =  function()
						SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
						ViewMgr:CloseView(DialogTipsView)
						return 
					end 
				},
				tnr = DTTodayNotRemind.TeamItemCheck
			}		
		if item:Num() == 0 then 
		    PublicPopupCtrl.Instance:DialogTips(param_t)
			return 
		end 
	elseif TeamData.Instance:GetTeamItemCHeckResult() ~= nil 
		and TeamData.Instance:InTeam() and TeamData.Instance:IsLeader() then 
		local item = Item.New({item_id = TeamData.Instance:GetTeamItemCHeckResult().target})
		local result = TeamData.Instance:GetTeamItemCHeckResult().result
		local param_t = {
				content =  string.format(Language.Team.ItemCheckResult,item:ColorStr(),item:Name()),
				confirm = {
					func =  function()
						ViewMgr:CloseView(DialogTipsView)
						SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
					end 
				},
				tnr = DTTodayNotRemind.TeamItemCheck
			}
			TeamData.Instance:ClearItemCheck()
			if not result then 
			    PublicPopupCtrl.Instance:DialogTips(param_t)
			return 
		end
	end
	if GuildData.Instance:CheckIsCooBattle(beh_cfg.param1) then 
		local team_limit_type = GuildData.Instance:GetCooBattleTeamLimit()
		local type_limit =  GuildData.Instance:GetCooBattleTypeLimit()
		if TeamData.Instance:InTeam() and team_limit_type == 1 then 
			PublicPopupCtrl.Instance:DialogTips{
				content = Language.Team.ChallengeTaskCheck,
				confirm = {
					func =  function()
						TeamCtrl.Instance:SendExitTeam()
						if type_limit then 
							-- SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
							GuildCtrl.SendGuildCooperateReq({req_type = 5})
						else 
							SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
						end 
						
						ViewMgr:CloseView(DialogTipsView)
					end 
				},
			}
			return 
		else 
			GuildCtrl.SendGuildCooperateReq({req_type = 5})
			return
		end 
	end

	if obj_vo then
		SceneCtrl.Instance:RequestGroupIdBattle(obj_vo.seq, beh_cfg.param1)
	else
		SceneCtrl.Instance:RequestGroupIdBattleNoNS(beh_cfg.param1)
	end
end

InteractionOpenModView = InteractionOpenModView or {}

--param1:mod_key
function InteractionOpenModView.Exe(beh_cfg,obj_vo)
	--提交物品任务道具物品框
	if beh_cfg.param1 == Mod.ItemsSub.Main then
		local config = BehaviorCtrl.GetCfg(beh_cfg.param3) 
		if config and Item.GetNum(config.param2) < config.param3 then
			TaskCtrl.Instance:ItemsSubDiong(beh_cfg,function()
				TaskData.is_shop_auto_close = function()
					if Item.GetNum(config.param2) < config.param3 then
						return
					end
					TaskCtrl.Instance:ItemsSubView()
				end
				beh_cfg.task_info:TaskDialogBeh(beh_cfg,config)
			end)
		else
			TaskCtrl.Instance:ItemsSubDiong(beh_cfg)
		end
		--加自动处理
		if TaskConfig.TaskType.fly_up == beh_cfg.task_info.task_type then
			BehaviorData.Instance.auto_behavior = 3
		end			
		return
	end

	if beh_cfg.param1 == Mod.Fabao.Main and beh_cfg.param3 and beh_cfg.param3 > 0 then
		GuideManager.Instance:Start(beh_cfg.param3)
		return
	end

	if beh_cfg.param1 == Mod.Answer.AnswerView then
		if ActivityData.Instance:IsNoEnoughLevel(ActType.WSTQuesten) then
			return
		end
	end

	if beh_cfg.param1 == Mod.Live.Main then
		LiveData.Instance.view_open_info = {tab_index = beh_cfg.param2, is_refine = true}
	end

	--NPC商店处理
	if beh_cfg.param1 == Mod.Reputation.Main then
		if not tonumber(beh_cfg.param3) then
			ReputationCtrl.Instance:GuideStart(Split(beh_cfg.param3,"|"),beh_cfg.param2)
		else
			ReputationCtrl.Instance:Reputation(beh_cfg.param2,beh_cfg.param3)
		end
	end

	if ModToModule(beh_cfg.param1) == ModToModule(Mod.MainTask.Main) then
		MainTaskData.Instance:SetQuickLocal(beh_cfg.param2)
	end

	if beh_cfg.param1 == Mod.RoleAppear.Main or beh_cfg.param1 == Mod.ProfesstionChange.Change then
		local error_str =  beh_cfg.param1 == Mod.RoleAppear.Main and ErrorText[101] or ErrorText[102]
		if RoleData.Instance:ProfStage() == 0  then 
			PublicPopupCtrl.Instance:Center(error_str)
			return 
		end 
	end
	
	if beh_cfg.param1 == Mod.ExchangeTask.Main then
		if not ExchangeMonoCtrl.Instance:Data():MedalExchange() then
			LogError("当前兑换勋章任务配置有问题！")
		end
	end

	if beh_cfg.param1 == Mod.MedalStart.Main then
		ViewMgr:OpenView(ModToView[beh_cfg.param1], beh_cfg.param2)
		return
	end
	

	if beh_cfg.param1 == Mod.TaskWXLInfo.Main then
		WenXiangLouData.Instance:SetDetailInfoInfoType(WenXiangLouConfig.InfoType.report)
		ViewMgr:OpenView(WenXiangLouInfoView)
	end

	if beh_cfg.param1 == Mod.TaskWXLCut.Main then
		if TeamData.Instance:InTeam() then
			PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.PopupTeamTip,function()
				TeamCtrl.Instance:SendExitTeam()
				ViewMgr:OpenViewByKey(beh_cfg.param1, beh_cfg.behavior_id)
			end)
			return
		end
	end

	if beh_cfg.param1 == Mod.Anecdote.FishAnswer then
		AnerQuestionData.Instance:SetCurTask(beh_cfg.task_info.task_id)
	end

	--市场跳转
	if beh_cfg.param1 == Mod.Mall.Market then
		MarketData.Instance:SetJumpIndex(beh_cfg.param2)
		MallData.Instance:SetJumpItemNum(beh_cfg.param3)
	end

	-- 婚礼答题
	if beh_cfg.param1 == Mod.Love.Answer then
		WeddingAnswerData.Instance:ProgressPlus(beh_cfg.param2)
	end

	-- 遗迹夺宝
	if beh_cfg.param1 == Mod.TombRaider.Main then
		if TombRaiderCtrl.Instance:CheckNeedAdjust() then 
			return 
		end  
	end

	if beh_cfg.param1 == Mod.Workshop.Forging then
		WorkshopData.Instance:SetBehaviorSelect(beh_cfg.param2)
	end

	if beh_cfg.param1 == Mod.Butterfly.Capturing then
		TeamData.Instance:CheckSingle(function ()
			if CatchButterfliesData.Instance:IsCantCatch() then
				PublicPopupCtrl.Instance:Center(Language.Butterfly.CountTip)
				return
			end
			CatchButterfliesData.Instance:SetObjVo(obj_vo)
			ViewMgr:OpenViewByKey(beh_cfg.param1, beh_cfg.behavior_id)
		end)
		return
		--[[ if TeamData.Instance:InTeam() then
			PublicPopupCtrl.Instance:Center(Language.Butterfly.TeamTip)
			return
		end
		if CatchButterfliesData.Instance:IsCantCatch() then
			PublicPopupCtrl.Instance:Center(Language.Butterfly.CountTip)
			return
		end
		CatchButterfliesData.Instance:SetObjVo(obj_vo) ]]
	end
	
	if ViewMgr:IsOpen(MainView) then
		ViewMgr:OpenViewByKey(beh_cfg.param1, beh_cfg.behavior_id)
	else
		local view_info = nil
		local view_key = Mod.ParseKey(beh_cfg.param1)
		if Mod.IsView(beh_cfg.param1) then
			view_info = ViewMgr:getViewInfo(ModToView[beh_cfg.param1])
		elseif Mod.IsView(view_key) then
			view_info = ViewMgr:getViewInfo(ModToView[view_key])
		end
		-- LogError("view_info",view_info)
		if view_info and view_info.view_tab and view_info.view_tab.view_level ~= ViewLevel.L0 then
			ViewMgr:OpenViewByKey(beh_cfg.param1, beh_cfg.behavior_id)
		end
	end
end

InteractionTaskCommitCurrency = InteractionTaskCommitCurrency or {}
function InteractionTaskCommitCurrency.Exe(beh_cfg,obj_vo)
	if nil ~= ItemsSubData.Instance.sub_currency_func then
		Call(ItemsSubData.Instance.sub_currency_func)
    end
end

InteractionOpenView = InteractionOpenView or {}

--param1:view tab
function InteractionOpenView.Exe(beh_cfg,obj_vo)
	ViewMgr:OpenView(beh_cfg.param1)
end
InteractionTreasureMap = InteractionTreasureMap or {}
function InteractionTreasureMap.Exe(beh_cfg,obj_vo)
	--判断正确的
	local list = BagData.Instance:GetItemsByItemId(beh_cfg.param1.item_id)
	for index, value in ipairs(list) do
		if value.param.client_need_uid1 == beh_cfg.param1.param.client_need_uid1 then
			beh_cfg.param1 = value
			break
		end
	end
	MainOtherCtrl.Instance:OpenKeyUseView(beh_cfg.param1)
end

-- NpcBehaviorType = {
-- 	None = 0,
-- 	Talk = 1,
-- 	Gather =3,
-- 	Battle = 4,
-- 	OpenView = 5,
-- }

InteractionSeqTrack = InteractionSeqTrack or {}

--param1:seq_id
--param2:寻路完成后行为的beh_id
function InteractionSeqTrack.Exe(beh_cfg,obj_vo)
	local com_beh_id = tonumber(beh_cfg.param2) or 0
	local com_beh = nil
	if com_beh_id > 0 then
		com_beh = BehaviorCtrl.GetCfg(com_beh_id,true)
		com_beh.task_info = beh_cfg.task_info
	end
	SceneCtrl.Instance:Logic():AutoToNpc(beh_cfg.param1, com_beh, false, true)
end


InteractionNpcIdTrack = InteractionNpcIdTrack or {}

--param1:npc_id
--param2:寻路完成后行为的beh_id
function InteractionNpcIdTrack.Exe(beh_cfg,obj_vo)
	local com_beh_id = tonumber(beh_cfg.param2) or 0
	local com_beh = nil
	if com_beh_id > 0 then
		com_beh = BehaviorCtrl.GetCfg(com_beh_id,true)
		com_beh.task_info = beh_cfg.task_info
	end	
	SceneCtrl.Instance:Logic():AutoToNpc(beh_cfg.param1,com_beh)
end

InteractionEnterBattleSpecial = InteractionEnterBattleSpecial or {}

function InteractionEnterBattleSpecial.Exe(beh_cfg,obj_vo)
	if MarryData.Instance:IsParadeFloat() then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return
    end
	if SPECIAL_BATTLE.BIG_DIPPER == beh_cfg.param1 then
		if not StringIsEmpty(beh_cfg.param3) and beh_cfg.param3 > 0 then
			local re = TeamCtrl.Instance:TeamDetectByTargetId(beh_cfg.param3, function ()
				if TeamCtrl.Instance:Data():InTeam() then
					if not TeamCtrl.Instance:Data():IsLeader() then
						PublicPopupCtrl.Instance:Center(ErrorText[57])
						return
					end		
					BigDipperCtrl.Instance:SendBigDipperReq(1,beh_cfg.param2)
				end
			end, nil, true)
			return
		end	
		BigDipperCtrl.Instance:SendBigDipperReq(1,beh_cfg.param2)
	elseif SPECIAL_BATTLE.FALLEN_GOD == beh_cfg.param1 then
		if not FallenGodData.Instance:IsBossUnlock(beh_cfg.param2) then
			PublicPopupCtrl.Instance:Center(Language.FallenGod.Error1)
			return
		end
		if not StringIsEmpty(beh_cfg.param3) and beh_cfg.param3 > 0 then
			local re = TeamCtrl.Instance:TeamDetectByTargetId(beh_cfg.param3, function ()
				if TeamCtrl.Instance:Data():InTeam() then
					if not TeamCtrl.Instance:Data():IsLeader() then
						PublicPopupCtrl.Instance:Center(ErrorText[57])
						return
					end
					FallenGodCtrl.Instance:SendFight(beh_cfg.param2)
				end
			end, FallenGodData.Instance:GetMinMemberNum(), true)
			return
		end
		FallenGodCtrl.Instance:SendFight(beh_cfg.param2)
	end	
end
InteractionYinYangEye = InteractionYinYangEye or {}
function InteractionYinYangEye.Exe(beh_cfg, obj_vo)
	-- LogDG(beh_cfg, obj_vo)
	if obj_vo and obj_vo.obj_type == SceneObjType.YinYangObj then
		YinYangCtrl.CSYinYangEyeOpReq(obj_vo.show_type == 0 and 1 or 2, obj_vo.seq)
	end
end
--p1开关阴阳眼,p2开关鬼火p3鬼火指引对象
InteractionOpenOrCloseYinYangEye = InteractionOpenOrCloseYinYangEye or {}
function InteractionOpenOrCloseYinYangEye.Exe(beh_cfg, obj_vo)
	if beh_cfg then
		YinYangCtrl.CSYinYangEyeOpReq(0, tonumber(beh_cfg.param1))
		if tonumber(beh_cfg.param3) ~= 0 then
			GuideManager.Instance:DoArrowGuideGuiHuo(tonumber(beh_cfg.param2) == 1,  tonumber(beh_cfg.param3))
		end
	end
end
--门派任务自动，有就去做，没有就去接，不需要考虑今天已经做完的情况
InteractionAutoBountyTask = InteractionAutoBountyTask or {}
function InteractionAutoBountyTask.Exe()
	local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.bounty)
	if task then 
		task:GoTask()
	else
		ActivityData.Instance:SetOpenRemind(ActType.BountyTask)
	end
end
--三仙归洞开始游戏
InteractionStartSanXianGuiDong = InteractionStartSanXianGuiDong or {}
function InteractionStartSanXianGuiDong.Exe()
	LookingForRabbitsCtrl.EnterFB()
end

InteractionEnterFb = InteractionEnterFb or {}
--param1:fb_type
function InteractionEnterFb.Exe(beh_cfg,obj_vo,ignore)
	--截一下勇闯地宫
	if FB_TYPE.MIYI == beh_cfg.param1 then
		--[[ if not TeamData.Instance:InTeam() or TeamData.Instance:MemberNumRole() < RushDungeonData.Instance:MiniNum() then
			PublicPopupCtrl.Instance:DialogTips({
					content = Language.PublicPopup.ResTask,
					confirm = {
						func = function ()
							if TeamData.Instance:InTeam() then
								TeamCtrl.Instance:SendTeamRecruit()
								PublicPopupCtrl.Instance:Center(ErrorText[86])
								PublicPopupCtrl.Instance:CloseDialogTips()
								return
							end
							TeamData.Instance.cur_platform_target_id = 3
                            ViewMgr:OpenView(TeamPlatformView)
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
					}
				})
			return
		end ]]
		--[[ if RushDungeonData.Instance.play_times >= 1 then
			PublicPopupCtrl.Instance:Center(ErrorText[125])
			return
		end ]]
		--TeamCtrl.Instance:TeamDetectByActType(ActType.RushDungeon,function ()
		local level = RoleData.Instance:GetRoleLevel()
		if level < beh_cfg.param3 then
			PublicPopupCtrl.Instance:Center(string.format(Language.NpcDialog.level_tips,beh_cfg.param3 ))
		else
			SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2)
		end
		--end)
		return
	end
	--水晶副本
	if FB_TYPE.CRYSTAL_FB == beh_cfg.param1 then
		local config = FightData.Instance:GetFightCrystalConfigBySeq(beh_cfg.param2)
		if config then
			local is_show_confim = true
			if config.scene_type_1 > 1 then
				if TeamData.Instance:InTeam() and TeamData.Instance:MemberNumRole() > 1 then is_show_confim = false end
			end
			if config.min_level > RoleData.Instance:GetRoleLevel() then
				if 2 == config.scene_type_1 then
					PublicPopupCtrl.Instance:Center(string.format(Language.Fight.CrystalLevelTips2, config.min_level))
				else
					PublicPopupCtrl.Instance:Center(Language.Fight.CrystalLevelTips)
				end
				return
			end
			if BagData.Instance:GetNumByItemId(config.consume_item) < 1 then
				if (1 == beh_cfg.param3 and not FightData.Instance:IsFightCrystalEnergyEnough()) or 0 == beh_cfg.param3 then
					SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2)
					return
				end
			end
			if 1 == config.scene_type_1 and TeamData.Instance:InTeam() and not ignore then
				PublicPopupCtrl.Instance:DialogTips{
					content = Language.Fight.CrystalTeamTips,
					confirm = {
						func = function()
							TeamCtrl.Instance:SendExitTeam()
							InteractionEnterFb.Exe(beh_cfg, obj_vo, true) 
							ViewMgr:CloseView(DialogTipsView)
						end
					}
				}
				return
			end
			if is_show_confim then
				if FightData.Instance:IsFightCrystalEnergyEnough() or BagData.Instance:GetNumByItemId(config.consume_item) >= 1 then
					local pass_time_max = FightData.Instance:GetCrystalFbPassTimeMax()
					local num = pass_time_max + (FightData.Instance.fight_fb_ymz.normal_fb_record_bt_list[beh_cfg.param2] or 0) - (FightData.Instance.fight_fb_ymz.normal_fb_record_list[beh_cfg.param2] or 0)
					if config.recommend_score > RoleData.Instance:GetSumScore() and num > 0 then
						PublicPopupCtrl.Instance:DialogTips({
							content = Language.Fight.CrystalDungen.score_tips,
							confirm = {
								func = function()
									ViewMgr:CloseView(DialogTipsView)
									-- FightData.Instance:FightSingleBeh(beh_cfg)
									-- ViewMgr:OpenView(FightSingleView)
									SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2)
									ViewMgr:CloseView(FightFBView)
								end
							}
						})
					else
						-- FightData.Instance:FightSingleBeh(beh_cfg)
						-- ViewMgr:OpenView(FightSingleView)
						SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2)
						if num > 0 then ViewMgr:CloseView(FightFBView) end
					end
					return
				end
			end
		end
	end
	if FB_TYPE.FIGHT_COLLECT == beh_cfg.param1 and TeamData.Instance:InTeam() then
		PublicPopupCtrl.Instance:Center(Language.Fight.GatherTeamTips)
		return
	end
	if FB_TYPE.FANTASYFB == beh_cfg.param1 and (not TeamData.Instance:InTeam() or (TeamData.Instance:InTeam() and TeamData.Instance:MemberNumRole() < 3) )then
		PublicPopupCtrl.Instance:DialogTips{
			content = Language.Fight.FantasyTeamTips,
			confirm = {
				func = function()
					TeamCtrl.Instance:TeamDetectByActType(ActType.FantasyFB,function()
					end)
					ViewMgr:CloseView(DialogTipsView)
				end
			}
		}
		return
	end 
	if FB_TYPE.MEDAL == beh_cfg.param1 then
		local task_id = nil
		if beh_cfg.task_info then task_id = beh_cfg.task_info.task_id end 
		MedalTaskData.Instance:SetCurMedalDungeonInfo(beh_cfg.behavior_id,task_id) 

		local info = MedalTaskData.Instance:GetCurMedalDungeonInfo()
		if nil ~= info and 0 == BagData.Instance:GetNumByItemId(info.consume_item) then
			local item = Item.New({item_id = info.consume_item})
			PublicPopupCtrl.Instance:DialogTips{
				content = string.format(Language.Fight.MedalConsumeTips, item:QuaName()),
				confirm = {
					func = function()
						SceneLogic.Instance:AutoToNpc(Config.medaltask_scene_auto.other[1].npc_seq,nil,nil,true)
						ViewMgr:CloseView(DialogTipsView)
					end
				}
			}
		    return 
		end 

		if MedalTaskData.Instance:IsLimitedMedalTask(beh_cfg.behavior_id,task_id) and TeamData.Instance:InTeam() then 
			PublicPopupCtrl.Instance:DialogTips{
				content = Language.Fight.MedalSoloTips,
				confirm = {
					func = function()
						TeamCtrl.Instance:SendExitTeam()
						SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2)
						ViewMgr:CloseView(DialogTipsView)
					end
				}
			}
			return
		elseif MedalTaskData.Instance:IsLimitedMedalTask(beh_cfg.behavior_id,task_id) and not TeamData.Instance:InTeam() then 
			FightData.Instance:FightSingleBeh(beh_cfg)
			ViewMgr:OpenView(FightSingleView)
			return
		elseif not MedalTaskData.Instance:IsLimitedMedalTask(beh_cfg.behavior_id,task_id) and not TeamData.Instance:InTeam() then 
			PublicPopupCtrl.Instance:DialogTips{
				content = Language.Fight.MedalTeamTips,
				confirm = {
					func = function()
						if beh_cfg.param3 then
							local target = TeamData.Instance:GetTeamGoalWithId(beh_cfg.param3) 
							TeamData.Instance.cur_platform_target_id = beh_cfg.param3
							TeamCtrl.Instance:SendTeamMatch(beh_cfg.param3, target.is_cross, TeamConfig.MatchType.start)
						end 
						-- ViewMgr:OpenView(TeamPlatformView)
						ViewMgr:CloseView(DialogTipsView)
					end
				},
				cancel = {
					func = function()
						FightData.Instance:FightSingleBeh(beh_cfg)
						ViewMgr:OpenView(FightSingleView)
						ViewMgr:CloseView(DialogTipsView)
					end
				}
			}
			return
		elseif not MedalTaskData.Instance:IsLimitedMedalTask(beh_cfg.behavior_id,task_id) and TeamData.Instance:InTeam() then 
			local info = MedalTaskData.Instance:GetCurMedalDungeonInfo()
			local task_scene_cfg = MedalTaskData.Instance.scene_task_config[info.map_id]
			if TeamData.Instance:MemberNumRole() < task_scene_cfg.player_num then 
				PublicPopupCtrl.Instance:DialogTips{
					content = Language.Fight.MedalTeamTips,
					confirm = {
						func = function()
							if beh_cfg.param3 then
								local target = TeamData.Instance:GetTeamGoalWithId(beh_cfg.param3) 
								-- TeamData.Instance.cur_platform_target_id = beh_cfg.param3
								TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
								TeamCtrl.Instance:SendTeamMatch(beh_cfg.param3, target.is_cross, TeamConfig.MatchType.start)
							end 
							-- ViewMgr:OpenView(TeamPlatformView)
							ViewMgr:CloseView(DialogTipsView)
						end
					},
					cancel = {
						func = function()
							if TeamData.Instance:MemberNumRole() == 1 then
							    FightData.Instance:FightSingleBeh(beh_cfg)
								ViewMgr:OpenView(FightSingleView)
							else 
								SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2)
							end 
							ViewMgr:CloseView(DialogTipsView)
						end
					},
				}
				return 
			end 
		end
	end
	--进阶副本
	if FB_TYPE.ADVANCED_FB == beh_cfg.param1 then
		local target = TeamData.Instance:GetTeamGoalWithId(beh_cfg.param3)
		local adv_config = FightData.Instance:GetAdvancementsData(beh_cfg.param2,1)
		if not TeamData.Instance:InTeam() then
			PublicPopupCtrl.Instance:DialogTips{
				content = Language.PublicPopup.AdvancedTaskFb,
				cancel = {
					name = Language.PublicPopup.AdvancedCancel,
					func = function()
						SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2)
						ViewMgr:CloseView(DialogTipsView)
					end
				},
				confirm = {
					name = Language.PublicPopup.AdvancedConfirm,
					func = function()
						ViewMgr:CloseView(DialogTipsView)
						if LoginData.Instance:IsOnCrossSever() and TeamConfig.CrossType.not_cross == target.is_cross then
							PublicPopupCtrl.Instance:Center(Language.Team.MatchCrossError)
							return
						end
						TeamData.Instance.need_auto_match = true
						TeamData.Instance.cur_sel_target = target
						TeamData.Instance.cur_platform_target_id = beh_cfg.param3
						TeamCtrl.Instance:SendCreateTeam(target.is_cross)
		
						TimeHelper:CancelTimer(InteractionEnterFb.PromoteResTaskTime)
						InteractionEnterFb.PromoteResTaskTime = Invoke(function()
							TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
							TeamCtrl.Instance:SendTeamMatch(beh_cfg.param3, target.is_cross, TeamConfig.MatchType.start)
							ViewMgr:OpenView(TeamView)
							TeamCtrl.Instance:OnSendNotice()
						end,0.5)
					end
				},
				close = true,
				change_btn_pos = true,
			}
		else
			if TeamData.Instance:MemberNumRole() < adv_config.player_num then
				PublicPopupCtrl.Instance:DialogTips{
					content = Language.PublicPopup.ResAutoTaskNumNotEnough,
					cancel = {
						name = Language.PublicPopup.AdvancedCancel,
						func = function()
							SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2)
							ViewMgr:CloseView(DialogTipsView)
						end
					},
					confirm = {
						name = Language.PublicPopup.AdvancedConfirm,
						func = function()
							ViewMgr:CloseView(DialogTipsView)
							if LoginData.Instance:IsOnCrossSever() and TeamConfig.CrossType.not_cross == target.is_cross then
								PublicPopupCtrl.Instance:Center(Language.Team.MatchCrossError)
								return
							end
							TeamData.Instance.need_auto_match = true
							TeamData.Instance.cur_sel_target = target
							TeamData.Instance.cur_platform_target_id = beh_cfg.param3
							
							TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
							TeamCtrl.Instance:SendTeamMatch(beh_cfg.param3, target.is_cross, TeamConfig.MatchType.start)
							ViewMgr:OpenView(TeamView)
							TeamCtrl.Instance:OnSendNotice()
						end
					},
					close = true,
					change_btn_pos = true,
				}
			else
				if beh_cfg.param2 == 2 and TeamData.Instance:MemberNumRole() > adv_config.player_num then
					PublicPopupCtrl.Instance:AlertTip(Language.Fight.AdvancedFbTips,function()
						SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2)
					end)
					return
				end
				SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2)
			end
		end
		return
	end
	SceneCtrl.Instance:RequestEnterFb(beh_cfg.param1,beh_cfg.param2)
end

InteractionJumpScene = InteractionJumpScene or {}

--param1:scene_id param2 下一个行为
function InteractionJumpScene.Exe(beh_cfg, obj_vo)
	local com_beh_id = beh_cfg.param2
	local com_beh = nil
	if com_beh_id ~= "" and com_beh_id > 0 then
		com_beh = BehaviorCtrl.GetCfg(com_beh_id, true)
		com_beh.task_info = beh_cfg.task_info
	end
	SceneCtrl.Instance:Logic():AutoToPos(beh_cfg.param1, nil, com_beh, false, true)
end

InteractionFlyScene = InteractionFlyScene or {}

--param1:scene_id param2 x param3 y
function InteractionFlyScene.Exe(beh_cfg, obj_vo)
	local target = nil
	if beh_cfg.param2 ~= "" and beh_cfg.param3 ~= "" then
		target = {}
		target.x = tonumber(beh_cfg.param2)
		target.y = tonumber(beh_cfg.param3)
	end
	SceneCtrl.Instance:Logic():AutoFlyToScene(beh_cfg.param1, target)
end

InteractionGuide = InteractionGuide or {}
function InteractionGuide.Exe(beh_cfg, obj_vo)
	if beh_cfg and not StringIsEmpty(beh_cfg.param1) then
		GuideManager.Instance:Start(tonumber(beh_cfg.param1))
	end
end

InteractionBattleStartNpcDemoFight = InteractionBattleStartNpcDemoFight or {}
function InteractionBattleStartNpcDemoFight.Exe(beh_cfg, obj_vo)
	if beh_cfg and not StringIsEmpty(beh_cfg.param1) then
		BattleCtrl.SendBattleStartNpcDemoFight(tonumber(beh_cfg.param1))
	end
end

--奇闻任务交互
InteractionAnecdote = InteractionAnecdote or {}

function InteractionAnecdote.Exe(beh_cfg, obj_vo)
	local SendTaskFinishReq = function()
		AnecdoteCtrl.Instance:SendAnecdoteTaskFinishReq(beh_cfg.param1,beh_cfg.param2)
	end
	if AnecdoteConfig.OpenFunc[beh_cfg.param1] then
		AnecdoteConfig.OpenFunc[beh_cfg.param1](SendTaskFinishReq)
	end
	if beh_cfg.param3 ~= 0 then
		TimeHelper:CancelTimer(InteractionAnecdote.ExeTimer)
		InteractionAnecdote.ExeTimer = Invoke(function()
			AnecdoteData.Instance:OnAutoHandle(beh_cfg.param1)
		end,beh_cfg.param3)
	end
end

InteractionPatrol = InteractionPatrol or {}
function InteractionPatrol.Exe(beh_cfg,obj_vo)
	if SceneData.Instance:SceneId() ~= beh_cfg.param1 and beh_cfg.param1 ~= "" then
		SceneLogic.Instance:AutoToPos(beh_cfg.param1, nil, BehaviorFactory.GoPatrol(), false, true)
	else
		SceneLogic.Instance:AutoToPos(SceneData.Instance:SceneId(), nil, BehaviorFactory.GoPatrol(), false, true)
	end
end

InteractionGoPatrol = InteractionGoPatrol or {}
function InteractionGoPatrol.Exe(beh_cfg, obj_vo)
	SceneLogic.Instance:GoOnPatrol()
end

InteractionBattleMode = InteractionBattleMode or {}
function InteractionBattleMode.Exe(beh_cfg,obj_vo)
	if MarryData.Instance:IsParadeFloat() then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return
    end
	local param_t = {}
	param_t.battle_mode = tonumber(beh_cfg.param1)
	param_t.param1 = tonumber(beh_cfg.param2)
	BattleCtrl.BattleModeFightReq(param_t)
end
InteractionAcceptTask = InteractionAcceptTask or {}
local AcceptTaskFunc = {
	--宝图任务
	[1] = function ()
		local flag = ActivityData.Instance:IsNoEnoughLevel(ActType.TreasureMap)
		if flag then
			return
		end
		--已接取
		if TreasureMapData.Instance:HaveTask() then
			PublicPopupCtrl.Instance:Center(ErrorText[80])
			return
		end
		--次数
		if  TreasureMapData.Instance:IsFinishTask() then
			PublicPopupCtrl.Instance:Center(ErrorText[77])
			return
		end
		TreasureMapCtrl.Instance:SendTreasureMapReq({req_type = TREASURE_MAP_REQ_TYPE.ACCPET}) 
	end,
	--真中任务
	[2] = function ()
		local flag = ActivityData.Instance:IsNoEnoughLevel(ActType.ResearchTask)
		if flag then
			return
		end
		if ResearchTaskData.Instance:Time() <= 0 then
			PublicPopupCtrl.Instance:Center(ErrorText[73])
			return
		end
		if TeamCtrl.Instance:Data():InTeam() then
			local param_t = {
			content = Language.ResearchTask.ChallengeInTeam,
			confirm = {
					func = function()
						ViewMgr:CloseView(DialogTipsView)
						if ResearchTaskData.Instance.is_accept_task == true then
							ResearchTaskCtrl.Instance:AutoTask()
						else
							ResearchTaskCtrl.Instance:SendResearchTaskReq({op_type = RESEARCH_TASK_OPERATE_TYPE.ACCEPT})
							local info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.dragon)
							if info then
								TaskData.Instance:StopTaskState(TaskConfig.TaskType.dragon, CUSTOM_OBJID.RESEARCH_TASK)
								MainViewData.Instance:ToggleTTTask()
								TaskData.Instance:MenuToTaskId(info.task_id)
							end
						end
					end
				},
			}
			PublicPopupCtrl.Instance:DialogTips(param_t)    
		else
			if ResearchTaskData.Instance.is_accept_task == true then
				ResearchTaskCtrl.Instance:AutoTask()
			else
			-- TeamCtrl.Instance:TeamDetectByActType(ActType.ResearchTask,function ()
				ResearchTaskCtrl.Instance:SendResearchTaskReq({op_type = RESEARCH_TASK_OPERATE_TYPE.ACCEPT})
			-- end)
			end
		end
		local info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.dragon)
		if info then
			TaskData.Instance:StopTaskState(TaskConfig.TaskType.dragon, CUSTOM_OBJID.RESEARCH_TASK)
			MainViewData.Instance:ToggleTTTask()
			TaskData.Instance:MenuToTaskId(info.task_id)
		end
		--[[ if not TeamData.Instance:InTeam() or TeamData.Instance:MemberNumRole() < ResearchTaskData.Instance:MiniNum() then
			PublicPopupCtrl.Instance:DialogTips({
				content = Language.PublicPopup.ResTask,
				confirm = {
					func = function ()
						--是否组队
						if TeamData.Instance:InTeam() then
								TeamCtrl.Instance:SendTeamRecruit()
								PublicPopupCtrl.Instance:Center(ErrorText[86])
								PublicPopupCtrl.Instance:CloseDialogTips()
								return
							end
							TeamData.Instance.cur_platform_target_id = 3
                            ViewMgr:OpenView(TeamPlatformView)
                            PublicPopupCtrl.Instance:CloseDialogTips()
                        end
					}
				})
			return
		end ]]
	end,
	-- 山海灯会
	[3] = function()
		MSLanternData.Instance:IsNeedFind(false)
		-- 满足人数就开始接取任务
		MSLanternCtrl.Instance:JudgeStartTask(true)
	end,
	-- 山海灯会灯谜
	[4] = function()
		if MSLanternData.Instance:IsTimesEnought() then
			PublicPopupCtrl.Instance:Center(Language.MSLantern.StartAnswerTip)
			MSLanternData.Instance:ResetOther(true)
		else
			-- 发送开始答题
			MSLanternCtrl:SendReq(MSLanternReqType.StartAnswer)
		end
	end,
	-- 科举考试
	[5] = function()
		ImperialExamData.Instance:IsNeedFind(false)
		-- 满足人数就开始接取任务
		ImperialExamCtrl.Instance:JudgeStartTask(true)
	end,
	-- 科举考试考官
	[6] = function()
		if ImperialExamData.Instance:IsTimesEnought() then
			PublicPopupCtrl.Instance:Center(Language.ImperialExam.StartAnswerTip)
			ImperialExamData.Instance:ResetOther(true)
		else
			-- 发送开始答题
			ImperialExamCtrl:SendReq(ImperialExamReqType.StartAnswer)
		end
	end,
}
function InteractionAcceptTask.Exe(beh_cfg,obj_vo)
	local exe_func = AcceptTaskFunc[beh_cfg.param1]
	if exe_func then
		exe_func()
	end
end


InteractionArenaMatch = InteractionArenaMatch or {}
function InteractionArenaMatch.Exe(beh_cfg,obj_vo)
	ArenaMatchCtrl.Instance:FightByNpcSeq(obj_vo.seq)
end

InteractionEscort = InteractionEscort or {}
function InteractionEscort.Exe(beh_cfg,obj_vo)
	local accpet_info = {
		npc_seq = tonumber(beh_cfg.param1),
		escort_type = tonumber(beh_cfg.param2),
		task_id = tonumber(beh_cfg.param3),
	}
	local escort_type = tonumber(beh_cfg.param2)
	local escort_data = EscortCtrl.Instance:Data()
	local escort_info = escort_data:GetEscortAllInfo()
	if nil == escort_info or 0 == escort_info.task_seq then
		-- 没镖就去接镖 其他运镖没有提示和奖励 所以没有展示框
		if nil ~= EscortCtrl.Instance.task_btn_handle then
			BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(EscortCtrl.Instance.task_btn_handle)
			EscortCtrl.Instance.task_btn_handle = nil
		end
		EscortCtrl.Instance.task_btn_handle = BehaviorData.Instance:CustomNpcTalkBtnsSeq(accpet_info.npc_seq, function (btn_list, obj_vo)
			if obj_vo.seq == accpet_info.npc_seq then
				local role_level = RoleData.Instance:GetRoleLevel()
					-- local rewards, deposit = escort_data:GetDetailInfo(accpet_info.escort_type, role_level)
					local button = NpcDialogBehBtnData.FuncCreate(
						Language.Escort.EscortDisStrs.escort[accpet_info.escort_type],
						BindTool.Bind(
							EscortCtrl.Instance.SendEscortOp,
							EscortCtrl.Instance,
							{
								op_type = EscortConfig.OperType.accept,
								param = accpet_info.escort_type,
								param2 = accpet_info.task_id,
							}
						),
						true
						-- NpcDialogData.CreateDetailInfo1({string.format(Language.Escort.DetailTips[1], deposit), Language.Escort.DetailTips[2]}, rewards)
					)
					button:SetIconType(NpcIconType.Act)
					return {button}
			end	
		end, true)
		SceneLogic.Instance:AutoToNpc(accpet_info.npc_seq)
	else
		local npc_seq = escort_info.npc_seq
		SceneCtrl.Instance:Logic():AutoToNpc(npc_seq)
	end 
end

InteractionSubItems = InteractionSubItems or {}
function InteractionSubItems.Exe(beh_cfg,obj_vo)
	local data = ItemsSubData.Instance
	local sub_items = data.sub_items:Val()
	local value = true
	for i = 1, #sub_items do
        local info = sub_items[i]
		if info.sub_num > info.num then
			value = false
			PublicPopupCtrl.Instance:Center(string.format(Language.Task.SubTips.fail, Item.GetQuaName(info.item_id), info.sub_num - info.num))
		end
	end
	if value then 
		-- ViewMgr:OpenView(ItemsSubView) 
		TaskCtrl.Instance:ItemsSubView(true)
	end
end

InteractionSkipBattle = InteractionSkipBattle or {}
function InteractionSkipBattle.Exe(beh_cfg,obj_vo)
	TaskCtrl.Instance:SendTaskSkipBattle(tonumber(beh_cfg.param1))
	NpcDialogCtrl.Instance:NpcDialogView{
		talk_id = tonumber(beh_cfg.param2),
	}
end

InteractionTaskEscort = InteractionTaskEscort or {}
function InteractionTaskEscort.Exe(beh_cfg,obj_vo)
	EscortCtrl.Instance:SendEscortOp{
		op_type = beh_cfg.param1,
		param = 4,
		param2 = beh_cfg.param2,
	}
end

-- 任务专用藏宝图 param1场景id param2坐标(x,y) param3到达行为
InteractionTaskTreasureMap = InteractionTaskTreasureMap or {}
function InteractionTaskTreasureMap.Exe(beh_cfg,obj_vo)
	SceneLogic.Instance:AutoToPos(tonumber(beh_cfg.param1), nil, function ()
		local pos = string.split(beh_cfg.param2, ",")
		TreasureMapData.Instance:SetMapInfo(SmartData.New{
			is_task = true,
			param = {
				scene_id = tonumber(beh_cfg.param1),
				x = tonumber(pos[1]),
				y = tonumber(pos[2]),
				beh_id = tonumber(beh_cfg.param3),
			}
		})
		ViewMgr:OpenView(Compass)
	end, nil, true)
end

InteractionTaskBreakAltar = InteractionTaskBreakAltar or {}
function InteractionTaskBreakAltar.Exe(beh_cfg,obj_vo)
	--播放动画后再提交
	if obj_vo then
		local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(obj_vo.obj_id)
		if obj then
			if (TimeHelper.GetServerTime() - TaskData.Instance.bounty_task_breake_altar_stamp) > 10 then
				TaskData.Instance.bounty_task_breake_altar_stamp = TimeHelper.GetServerTime()
				obj:PlayAnim(ChAnimType.Idle)
				TimeHelper:AddDelayTimer(function ()
					obj:PlayAnim(ChAnimType.Run)
				end,0.1)
				TimeHelper:AddDelayTimer(function ()
					TaskCtrl.Instance:SendTaskGameReq(TaskConfig.TaskGameList.BreakAltar)		
				end, 5)
			end
			return
		end
	end
	TaskCtrl.Instance:SendTaskGameReq(TaskConfig.TaskGameList.BreakAltar)
end

InteractionBountyTaskSkip = InteractionBountyTaskSkip or {}
function InteractionBountyTaskSkip.Exe(beh_cfg,obj_vo)
	local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.bounty)
	if task then
		TaskCtrl.Instance:SendTaskSkipReq(task.task_id)
		NpcDialogCtrl.Instance:NpcDialogView{
			talk_id = tonumber(beh_cfg.param1),
		}
	end
end

InteractionMainTaskBounty = InteractionMainTaskBounty or {}
function InteractionMainTaskBounty.Exe(beh_cfg,obj_vo)
	local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.bounty)
	if task then
		task:GoTask()
	end
end

InteractionMainTaskTreasure = InteractionMainTaskTreasure or {}
function InteractionMainTaskTreasure.Exe(beh_cfg,obj_vo)
	TreasureTaskData.Instance:AccpetGoTask()
end

InteractionTaskVButtons = InteractionTaskVButtons or {}
function InteractionTaskVButtons.Exe(beh_cfg, obj_vo)
	local task_info = beh_cfg.task_info or (TaskCtrl.Instance:GetNpcDialogTaskButtonListByNpcId(obj_vo.id) or {})[1]
	if nil == task_info then return end
	local params = string.split(task_info.info.tasks_dialog_beh, "|")
	local param_t = {
		talk_id =  tonumber(params[2]),
		beh_btn_list =  {},
	}
	local btn_ids = string.split(params[3], ":")
	for i = 1, #btn_ids do
		local co = Cfg.NpcButtonListByButtonId(tonumber(btn_ids[i]))
		if co then
			local button = NpcDialogBehBtnData.BehIdCreate(co.show_word, co.button_behavior or 0, obj_vo)
			button:SetIconType(co.icon_type)
			table.insert(param_t.beh_btn_list, button)
		end
	end
	NpcDialogCtrl.Instance.data:SetCurTaskInfo(task_info)
	NpcDialogCtrl.Instance:NpcDialogView(param_t,nil,true)
end

InteractionSpyNpcTalk = InteractionSpyNpcTalk or {}

function InteractionSpyNpcTalk.Exe(beh_cfg,obj_vo)
	if TeamCtrl.Instance:Data():InTeam() then
		PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.PopupTeamTip,function()
			TeamCtrl.Instance:SendExitTeam()
		end)
		return
	end
	local cfg = Config.twyjx_story_auto.other[1]
	local npc_board = MainViewData.Instance:GetNpcBoard(cfg.spy_npc_seq)
	local role_board = MainViewData.Instance:GetBoard("role_id",RoleData.Instance:GetRoleId())
	if npc_board ~= nil then
		npc_board.CanvasGroup.alpha = 1
		npc_board:SetBubble(true,NpcDialogData.Instance:RandBubbleById(cfg.special_bubble_1))
	end
	local talk_timer = Invoke(function()
		if role_board ~= nil then
			role_board:SetBubble(true,NpcDialogData.Instance:RandBubbleById(cfg.special_bubble_2))
		end
	end,1)
	local effect_timer = Invoke(function()
		if npc_board ~= nil then
			npc_board:SetBubble(false)
		end
		if role_board ~= nil then
			role_board:SetBubble(false)
		end
		SE:Play(SceneEffectConfig.EffectList.SpyEffect, 
		Vector3.New(obj_vo.x, 75.28, obj_vo.y))
		SceneCtrl.Instance:DeleteNpc(cfg.spy_npc_seq)
	end,3)
	local add_npc_timer = Invoke(function()
		AnecdoteCtrl.Instance:SendTaskAccept(cfg.special_task_id)
	end,5)
end

InteractionStarLifeNpcTalk = InteractionStarLifeNpcTalk or {}
local star_life_time = nil
function InteractionStarLifeNpcTalk.Exe(beh_cfg, obj_vo)
	if TeamCtrl.Instance:Data():InTeam() then
		PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.PopupTeamTip,function()
			TeamCtrl.Instance:SendExitTeam()
		end)
		return
	end
	if star_life_time ~= nil and (TimeCtrl.Instance:GetServerTime() - star_life_time < 3) then
		return
	end
	star_life_time = TimeCtrl.Instance:GetServerTime()
	local cfg = Config.smxj_story_auto.other[1]
	local cry_cfg = Cfg.NpcBySeq(cfg.cry_npc_seq)
	local npc_cfg = Cfg.NpcBySeq(cfg.npc_id)
	local scene_cfg = SceneData.Instance:GetSceneCfg(npc_cfg.scene_id)
	local pos_h = 22
	if scene_cfg then
		for i, v in pairs(scene_cfg.npcs) do
			if v.seq == cfg.npc_id then
				pos_h = v.z
				break
			end
		end
	end
	local cry_pos = Vector3.New(cry_cfg.x, pos_h + 1, cry_cfg.y)
	local npc_pos = Vector3.New(npc_cfg.x, pos_h + 1, npc_cfg.y)

	local run_timer = nil
	local eff_obj = nil
	local move_dis = (npc_pos - cry_pos) / 0.75 * 0.03
	local function func_update()
		if eff_obj ~= nil and not eff_obj:Equals(nil) then
			eff_obj.transform.localPosition = eff_obj.transform.localPosition + move_dis
		end
	end

	local function func_time(obj, key, time)
		TimeHelper:CancelTimer(run_timer)
		if key == "start" then
			eff_obj = obj
			-- if run_timer == nil then
				run_timer = TimeHelper:AddRunTimer(func_update, 0.02)
			-- end
		elseif key == "end" then
			eff_obj = nil
			-- TimeHelper:CancelTimer(run_timer)
			GameObjPool:Release(obj)
			SE:Play(SceneEffectConfig.EffectList.StarLifeEffect2, npc_pos)
			AnecdoteCtrl.Instance:SendTaskAccept(cfg.task_id)
			SceneCtrl.Instance:DeleteNpc(cfg.cry_npc_seq)
		end
	end
	local function func_start()
		SE:Play(SceneEffectConfig.EffectList.StarLifeEffect1, cry_pos, func_time)
	end
	Invoke(func_start, 1)
end

InteractionFloating = InteractionFloating or {}
function InteractionFloating.Exe(beh_cfg,obj_vo)
	PublicPopupCtrl.Instance:Center(beh_cfg.param1)
	if beh_cfg.param2 == 1 then
		GuideData.Instance:CheckIsOpenActiveGuide()
	end
end

InterOpenPanel = InterOpenPanel or {}
--param1：获取途径id或mod_key param2:需要跳转的道具id param3:需要跳转道具id的数量
function InterOpenPanel.Exe(beh_cfg,obj_vo)
	local item = nil
	if beh_cfg.param2 ~= 0 then
		item = Item.Init(beh_cfg.param2)
	end
	if item and beh_cfg.param3 ~= 0 then
		if Item.GetNum(beh_cfg.param2) < beh_cfg.param3 then
			item.vo.jump_need_num = beh_cfg.param3 - Item.GetNum(beh_cfg.param2)
		end
	end
	if beh_cfg.param1 > 1000 then
		local open_panel = beh_cfg.param1
		local opened, open_tip = FunOpen.Instance:GetFunIsOpened(open_panel)
		if not opened then
			PublicPopupCtrl.Instance:Center(open_tip)
			return
		end
		local open_func = GetWayOpenPanelFunc[open_panel] or
        GetWayOpenPanelFunc[ModToModule(open_panel)]
        if open_func ~= nil and open_func(info,item) then
            return
        end
        ViewMgr:OpenViewByKey(open_panel)
	else
		local info = BagData.Instance:GetWayCfg(beh_cfg.param1)
		GetWayData.OpenPanel(info,item)
	end
end


InteractionMarryGather = InteractionMarryGather or {}

--param1 采集描述类型 param2 采集时间
function InteractionMarryGather.Exe(beh_cfg, obj_vo)
    local param_t = {
		finish_func = function()
			MarryCtrl.Instance:SendWeddingFBReq({req_type = MarryConfig.FBReqType.sub_act,param1 = MarryConfig.FBSubActType.xun_qing_mi_ai})
		end,
		desc = beh_cfg.param1,
		time = beh_cfg.param2,
		seq = obj_vo.seq,
		obj_id = obj_vo.obj_id,
		id = obj_vo.id,
	}
	GatherData.Instance:SetGatherInfo(param_t)
    ViewMgr:OpenView(GatherView)
end

InteractionMarryTable = InteractionMarryTable or {}

function InteractionMarryTable.Exe(beh_cfg, obj_vo)
    MarryData.Instance:WeddingEatMeal()
end

InteractionMarryPartyGather = InteractionMarryPartyGather or {}

function InteractionMarryPartyGather.Exe(beh_cfg, obj_vo)
    MarryPartyData.Instance:MarryPartyGather()
end

InteractionVitalityHelpOthers = InteractionVitalityHelpOthers or {}
--param1 采集描述类型 param2 采集时间
function InteractionVitalityHelpOthers.Exe(beh_cfg, obj_vo)
    local param_t = {
		finish_func = function()
			local config = VitalityHelpOthersData.Instance:GetConfigGatherId(obj_vo.id)
			if config ~= nil then
				SceneLogic.Instance:AutoToNpcs(config.npc_id,function()
					VitalityHelpOthersCtrl.Instance:OnNpcDialogView(config)
				end)
			end
			-- VoMgr:DeleteNpc(obj_vo)
		end,
		desc = beh_cfg.param1,
		time = beh_cfg.param2,
		seq = obj_vo.seq,
		obj_id = obj_vo.obj_id,
		id = obj_vo.id,
		is_client = true,
	}
	GatherData.Instance:SetGatherInfo(param_t)
    ViewMgr:OpenView(GatherView)
end


InteractionAnecdote2 = InteractionAnecdote2 or {}
function InteractionAnecdote2.Exe(beh_cfg, obj_vo)
	if beh_cfg.param1 == 0 then
		--假如是挑战类型就直接发协议 其他类型之后配了再说
		AnecdoteCtrl.Instance:CSSpecialAnecdoteReq(beh_cfg.param1 , beh_cfg.param2)
	elseif beh_cfg.param3 and beh_cfg.param3 ~= "" then
		AnecdoteData.Instance:SetNewParam(beh_cfg.param1,{
			seq = beh_cfg.param2,
			type = beh_cfg.param1,
		})
		ViewMgr:OpenViewByKey(beh_cfg.param3)
	end
end

-- param1 id param2 scene_id param3 commit npc
InteractionPetSeal = InteractionPetSeal or {}
function InteractionPetSeal.Exe(beh_cfg, obj_vo)

    local pet_id = beh_cfg.param1
	local scene_id = beh_cfg.param2
    local pet_name = Pet.New {pet_id = pet_id}:Name()

	-- --判断一下仓库
    ViewMgr:CloseView(GuildCooperateTaskView)
    ViewMgr:CloseView(GuildView)
	
	for k,v in pairs(PetData.Instance:GetPetList()) do 
		if v.pet_id == pet_id and v.status ~= 0 then 
			if ViewMgr:IsOpen(NpcDialogView) then 
				TaskCtrl.Instance:SendGiveNPCTaskNeedItems {
					task_id = beh_cfg.task_info.task_id,
					commit_list = {{item_id = k,sub_num = 1}}
				}    
				ViewMgr:CloseView(NpcDialogView)
			else
				if ViewMgr:IsOpen(PetGain) then
					ViewMgr:CloseView(PetGain)
				end

				SceneLogic.Instance:AutoToNpc(beh_cfg.param3,nil,nil,true)
			end 
			return 
		end 
	end 

	local goto_scene = function()
		BehaviorCtrl.ExeByCfgVo(
			{
				behavior_type = 9, --behavior_type 是 beh_id_to_tab.key
				param1 = scene_id,
				param2 = "",
				param3 = ""
			}
		)
		BehaviorData.Instance.auto_behavior = 2
		ViewMgr:OpenView(FightAutoSealSetView, {scene_id = scene_id, target_pets = {{pet_id = pet_id, num = 1}}})
		ViewMgr:CloseView(DialogTipsView)
	end
	PublicPopupCtrl.Instance:DialogTips {
		content = string.format(
			Language.TaskChain.NotEnoughTips[4],
			pet_name,
			SceneData.Instance:GetSceneCfg(scene_id).name
		),
		confirm = {func = goto_scene, name = Language.TaskChain.GetPet[1]},
		cancel = {func = function()
			ViewMgr:CloseView(DialogTipsView)
		end},
		close = true,
	}
end 

InteractionPetSealSimple = InteractionPetSealSimple or {}
function InteractionPetSealSimple.Exe(beh_cfg,obj_vo)
	local npc_seq = beh_cfg.param3
	local pet_id = beh_cfg.param1
    local pet_name = Pet.New {pet_id = pet_id}:Name()

    ViewMgr:CloseView(GuildCooperateTaskView)
    ViewMgr:CloseView(GuildView)

	SceneLogic.Instance:AutoToNpc(
            npc_seq,
            function()
				ViewMgr:CloseView(DialogTipsView)
				
                TaskChainItemSel.item_id = 0
                TaskChainItemSel.npc_seq = npc_seq
                TaskChainItemSel.pet_id = pet_id
				TaskChainItemSel.custom_func = function()
					TaskCtrl.Instance:SendGiveNPCTaskNeedItems {
						task_id = beh_cfg.task_info.task_id,
						commit_list = {{item_id = TaskChainItemSel.cur_sel_data.index,sub_num = 1}}
					}   
					
					ViewMgr:CloseView(TaskChainItemSel)
				end
                ViewMgr:OpenView(TaskChainItemSel)
            end,
            nil,
            true
        )
end

InteractionNpcTalkAfterJuage = InteractionNpcTalkAfterJuage or {}
function InteractionNpcTalkAfterJuage.Exe(beh_cfg, obj_vo)
    if BehaviorCtrl.InSceneObjInteraction and obj_vo then
        local task_cfg = TaskCtrl.Instance:GetTaskCfgByNpcSeq(obj_vo.seq)
        if task_cfg~=nil then
            if RoleData.Instance:GetRoleLevel() < task_cfg.min_level then
                PublicPopupCtrl.Instance:Center(Language.Challenge.tip3 .. "(" .. Language.Task.TaskTypeTitle[task_cfg.task_type] .. ")")
                return
            end
            if not TaskData.Instance:GetTaskIsCompleted(task_cfg.pretaskid1) then
                PublicPopupCtrl.Instance:Center(Language.Task.OpenTip .. "(" .. Language.Task.TaskTypeTitle[task_cfg.task_type] .. ")")
                return
            end
        end
        InteractionNpcTalk.Exe(beh_cfg, obj_vo)
    end
end



local beh_id_to_tab = {
	[1] = InteractionNpcTalk,	--对话 param1 是对话内容 param2是按钮文本 param3按钮行为
	[2] = InteractionNpcTalk2,	--对话 param1 是对话内容 param2是按钮id
	[3] = InteractionGather,	--拾取
	[4] = InteractionEnterBattle,	--group_id战斗
	[5] = InteractionOpenModView,		--打开模块界面	param1:mod key param2:自定义 param3:自定义

	[6] = InteractionSeqTrack,		--通过seq_id去找NPC交互 param1:seq_id,param2:寻路完成后行为的beh_id
	[7] = InteractionNpcIdTrack,	--通过npc_id去找npc交互 param1:npc_id,param2:寻路完成后行为的beh_id

	[8] = InteractionEnterFb,		--请求进入副本 param1:fb_type
	[9] = InteractionJumpScene,		--跳转场景 param1:scene_id param2 是跳转场景后的行为id
	[10] = InterOpenPanel,          --GetWayData.OpenPanel 通过获取途径前往对应的模块 param1 get_way_id|mod_key param2 item_id param3 need_num

	[11] = InteractionPatrol,		--巡逻 param1 是场景id
	[12] = InteractionBattleMode,	--模式战斗 param1 模式id
	[13] = InteractionAcceptTask,	--接受任务 param1 任务类型（客户端定义）
	[14] = InteractionArenaMatch,	--擂台赛交互
	[15] = InteractionEscort,		--运镖(活动、任务等多种运镖行为)
	[16] = InteractionSubItems,		--提交道具
	[18] = InteractionFlyScene,		--直飞场景 param1:scene_id param2 x坐标 param3 y坐标 服务端坐标
	[19] = InteractionGuide,		--执行指引 param1:step_id
	[20] = InteractionBattleStartNpcDemoFight,		--特殊主角配置战斗 param1:seq(npc_demo_auto)
	[21] = InteractionAnecdote,     --奇闻任务交互 param1:奇闻类型type param2:需要要提交的任务id param3:自动完成提交任务time
	[22] = InteractionEnterBattleSpecial,--特殊战斗
	[23] = InteractionYinYangEye,	--阴阳眼对象行为处理
	[24] = InteractionOpenOrCloseYinYangEye,--开关阴阳眼 param1: 0关1开, param2:是否使用鬼火，param3:鬼火目标
	[25] = InteractionTaskCommitCurrency,	--提交货币(通用任务专用) param1: 任务id
	
	[26] = InteractionAutoBountyTask, --自动进行门派任务（列表里没有的自动跑去接，列表里有的自动开跑）
	[27] = InteractionStartSanXianGuiDong,--进副本玩三仙归洞
	[28] = InteractionNpcTalk3,--和行为1一样对话后，新增执行缓存的对话行为如果没有则不执行
	[29] = InteractionNpcTalkAfterJuage, 	--判断等级条件及前置任务条件后执行行为1 
	
	[100] = InteractionOpenView,	--打开view param1:view tab
	[101] = InteractionTreasureMap,	--挖宝图
	[102] = InteractionGoPatrol,	--巡逻
	[103] = InteractionMarryGather,	--情缘采集物
	[104] = InteractionMarryTable,	--情缘餐桌
	[105] = InteractionMarryPartyGather,	--情缘盛会采集
	[106] = InteractionVitalityHelpOthers,--元气助人采集物
	
	[201] = InteractionTaskVButtons,	--任务专用多按钮复杂行为
	[202] = InteractionSkipBattle,		--任务专用有偿跳过战斗 param1任务id param2货币类型 param3货币数量
	[203] = InteractionTaskEscort,		--任务专用运镖 param1接0交1 param2任务id
	[204] = InteractionTaskTreasureMap,	--任务专用藏宝图 param1场景id param2坐标(x,y) param3到达行为id
	[205] = InteractionTaskBreakAltar,	--任务专用摧毁祭坛
	[206] = InteractionBountyTaskSkip,	--门派任务专用跳过
	[207] = InteractionMainTaskBounty,		--主线任务去做门派任务
	[208] = InteractionMainTaskTreasure,	--主线任务去做九州秘宝

	[301] = InteractionSpyNpcTalk,  --稻草人
	[302] = InteractionStarLifeNpcTalk,	--司命星君特效
	[303] = InteractionNpcTalk2,	--鸿蒙天界对话转发

	[400] = InteractionFloating,	--飘字 param1 内容 param2 (可不配置)事件类型：1指引活跃 param3 等级

	[500] = InteractionAnecdote2, 	--挑战类型的奇闻，param1 task_type param2 task_seq param3 mod_key
	[501] = InteractionPetSeal, 	-- 复刻商人的烦恼，param1 pet_id param2 scene_id,param3 任务提交npc,自动抓虫
	[502] = InteractionPetSealSimple, 	-- 简化商人的烦恼，只有提交宠物 --复刻商人的烦恼，param1 pet_id param2 scene_id param3 任务提交npc,自动抓虫
}


function BehaviorCtrl.GetCfg(beh_id,inst)
	local cfg = Config.npc_behavior_auto.npc_behavior_cfg[beh_id]
	if cfg ~= nil and inst == true then
		local re = {
            behavior_id=cfg.behavior_id,
            behavior_type=cfg.behavior_type,
            param1=cfg.param1,
            param2=cfg.param2,
            param3=cfg.param3,
        }
        return re
	end
	return cfg
end

function BehaviorCtrl.BehType(beh_id)
	-- local beh_cfg = Config.npc_auto.npc_behavior_cfg[beh_id]
	local beh_cfg = BehaviorCtrl.GetCfg(beh_id)
	return BehaviorCtrl.BehCfgType(beh_cfg)
end

function BehaviorCtrl.BehCfgType(beh_cfg)
	return beh_cfg ~= nil and beh_id_to_tab[beh_cfg.behavior_type] or nil
end

function BehaviorCtrl.Exe(beh_id,obj_vo)
	-- local beh_cfg = Config.npc_auto.npc_behavior_cfg[beh_id]
	local beh_cfg = Config.npc_behavior_auto.npc_behavior_cfg[beh_id]
	if beh_cfg == nil then
		if beh_id ~= -1 then
			BuglyReport("BehaviorCtrl.Exe ERROR,behavior_id not EXIST:%s",tostring(beh_id))
		end
		return
	end
	BehaviorCtrl.ExeByCfgVo(beh_cfg,obj_vo)
end

function BehaviorCtrl.ExeByVo(obj_vo)
	BehaviorCtrl.InSceneObjInteraction = true
	if obj_vo.obj_type == SceneObjType.Monster then
		local beh_cfg = NpcHelper.NpcCo(obj_vo.id, false)
		if obj_vo.obj_id == CUSTOM_OBJID.RESEARCH_TASK then
			beh_cfg = nil
		end
		if beh_cfg == nil or 0 == beh_cfg.behavior_type then
			if obj_vo.beh_cfg then
                beh_cfg = obj_vo.beh_cfg
            else
                beh_cfg = CommonStruct.BehCfg()
                beh_cfg.behavior_type = obj_vo.behavior_type or 4
                beh_cfg.param1 = obj_vo.param1 or obj_vo.monster_group_id
            end
		end
		LogDG(beh_cfg)
		BehaviorCtrl.ExeByCfgVo(beh_cfg, obj_vo)
	else
		if 1 == obj_vo.fun_type and obj_vo.id ~= Config.where_is_the_fairy_auto.play_config[1].npc_id then
			SceneCtrl.RequestTalk(obj_vo.obj_id)
			return
		end
		local npc_cfg = NpcHelper.NpcCo(obj_vo.id, false)
		BehaviorCtrl.ExeByCfgVo(npc_cfg, obj_vo)
	end
	BehaviorCtrl.InSceneObjInteraction = false
end

function BehaviorCtrl.ExeByCfgVo(beh_cfg,obj_vo)
	if beh_cfg == nil then
		return
	end
	local tab = beh_id_to_tab[beh_cfg.behavior_type]
	if tab == nil then
		return
	end
	if beh_cfg.con_func ~= nil then
		if beh_cfg.con_func() == false then
			return
		end
	end
	tab.Exe(beh_cfg,obj_vo)
end

FILE_REQ("modules/behavior/behavior_factory")