

NpcIconType = {
	Act = 1,
	DungeonNormal = 11,
	DungeonElite = 12,
	DungeonRare = 13,
	ShiLianZhiDi1 = 21,
	ShiLianZhiDi2 = 22,
	ShiLianZhiDi3 = 23,
	ShiLianZhiDi4 = 24,
	ShiLianZhiDi5 = 25,
	ShiLianZhiDi6 = 26,
}

NpcIconTypeToSp = {
	[NpcIconType.Act] = "DuiHuaHuoDongXiaoTuBiao",
	[NpcIconType.DungeonNormal] = "DuiHua_PuTong",
	[NpcIconType.DungeonElite] = "DuiHua_JingYing",
	[NpcIconType.DungeonRare] = "DuiHua_XiYou",
	[NpcIconType.ShiLianZhiDi1] = "ShiLianZhiDi1",
	[NpcIconType.ShiLianZhiDi2] = "ShiLianZhiDi2",
	[NpcIconType.ShiLianZhiDi3] = "ShiLianZhiDi3",
	[NpcIconType.ShiLianZhiDi4] = "ShiLianZhiDi4",
	[NpcIconType.ShiLianZhiDi5] = "ShiLianZhiDi5",
	[NpcIconType.ShiLianZhiDi6] = "ShiLianZhiDi6",
}

NpcDialogData = NpcDialogData or BaseClass()

function NpcDialogData:__init()
	if NpcDialogData.Instance ~= nil then
        Debuger.LogError("[NpcDialogData] attempt to create singleton twice!")
        return
    end
    NpcDialogData.Instance = self
	self:NpcDialogConfig()
	self.cur_task_info = nil
	self.npc_wst_data = {is_end = false,data = {}}
	self.cur_sel_btn = SmartData.New()
	self.sync_info = SmartData.New{talk_id = 0}
end

function NpcDialogData:__delete()
end

function NpcDialogData:NpcDialogConfig()
	self.config = Config.npc_talk_list_auto
	self.config_npc_talk_list = self.config.npc_talk_list
	self.config_npc_bubble_list = self.config.npc_bubble_list
	
	self.npc_config = Config.npc_auto
	self.npc_config_npc_list = self.npc_config.npc_list

	self:NpcListConfig()
	self:NpcTalkListConfig()
end

function NpcDialogData:NpcListConfig()
	self.npc_list_config = {}
	for _, info in pairs(self.npc_config_npc_list) do
		self.npc_list_config[info.id] = info
	end
end

function NpcDialogData:NpcTalkListConfig()
	self.npc_talk_list_config = {}
	for _, info in pairs(self.config_npc_talk_list) do
		if self.npc_list_config[info.talker] == nil and 0 ~= info.talker then
			LogError("对话配置对话talker角色npc不存在Config.npc_talk_list_auto talker error: ".. info.talk_id,info.talker)
		else
			info.talker_name = 0 == info.talker and 0 or self.npc_list_config[info.talker].name
			info.npc_res_id = 0 ~= info.talker and self.npc_list_config[info.talker].res_id or nil
			self.npc_talk_list_config[info.talk_id] = self.npc_talk_list_config[info.talk_id] or {}
			self.npc_talk_list_config[info.talk_id][info.talk_seq] = info
		end
	end
	for talk_id, info in pairs(self.npc_talk_list_config) do
		self.npc_talk_list_config[talk_id].max_seq = #info
		self.npc_talk_list_config[talk_id].talker = info[#info] and info[#info].talker or 0
	end

	self.npc_bubble_list = {}
	self.npc_bubble_special_list = {}
	for _, info in pairs(self.config_npc_bubble_list) do
		self.npc_bubble_list[info.bubble_id] = self.npc_bubble_list[info.bubble_id] or {}
		self.npc_bubble_special_list[info.bubble_id] = self.npc_bubble_special_list[info.bubble_id] or {}
		if info.special_show_type == 0 then
			self.npc_bubble_list[info.bubble_id][info.talk_id] = info
		else
			self.npc_bubble_special_list[info.bubble_id][info.param] = self.npc_bubble_special_list[info.bubble_id][info.param] or {}
			table.insert(self.npc_bubble_special_list[info.bubble_id][info.param],info)
		end
	end
end

function NpcDialogData:NpcDialogData(param_t)
	if nil ~= param_t.talk_list then
		if nil == param_t.talk_list.max_seq then
			for talk_seq, info in pairs(param_t.talk_list) do
				if type(info) == "table" then
					if info.talker == 0 then
						param_t.talk_list[talk_seq].talker_name = 0
					else
						param_t.talk_list[talk_seq].talker_name = info.talker
						if self.npc_list_config[info.talker] then
							param_t.talk_list[talk_seq].talker_name = self.npc_list_config[info.talker].name
						else
							LogDG("npc 对话获取不到NPC配置", info.talker, "尝试获取怪物配置")
							local config = Cfg.MonsterById(info.talker)
							if config then
								param_t.talk_list[talk_seq].talker_name = config.name
							end
						end
					end
					 -- = 0 == info.talker and 0 or (self.npc_list_config[info.talker] and  or "")
				end
			end
			param_t.talk_list.max_seq = #param_t.talk_list
		end
	end
	self.cur_npc_talk_list = param_t.talk_list or self.npc_talk_list_config[param_t.talk_id]
	if nil == self.cur_npc_talk_list then
		Debuger.LogError("talk_id:".. param_t.talk_id .. "未找到配置！")
		self.cur_npc_talk_list = self.npc_talk_list_config[1]
	end
	self.button_list = param_t.beh_btn_list or {}
	for i = 1, #self.button_list do
		self.button_list[i].btn_index = i
	end

	self.cur_talk_seq = param_t.is_skip and self.cur_npc_talk_list.max_seq or 1
	self.not_block_close = param_t.not_block_close or false
	self.not_block_close_func = param_t.not_block_close_func
	self.need_auto_typing = param_t.need_auto_typing
	self.func_state = NpcDialogConfig.FuncState.undo
	self.block_dofunc = param_t.block_dofunc
end

--当前npc seq
function NpcDialogData:SetCurNpcSeq(seq)
    self.cur_npc_seq = seq
end

function NpcDialogData:GetCurNpcSeq()
    -- if self.button_list ~= nil and self.button_list[1] and  self.button_list[1].obj_vo then
    -- 	return self.button_list[1].obj_vo.seq
    -- end
    -- return 0;
    return self.cur_npc_seq
end


function NpcDialogData:SetCurTaskInfo(task_info)
	self.cur_task_info = task_info
end

function NpcDialogData:CurTaskInfo()
	return self.cur_task_info
end

function NpcDialogData:GetCurTalkSeqInfo(is_next, is_last)
	local re_str = self.cur_npc_talk_list[is_next and self.cur_talk_seq + 1 or self.cur_talk_seq]
	if is_last then
		self.cur_talk_seq = self.cur_npc_talk_list.max_seq
		re_str = self.cur_npc_talk_list[self.cur_talk_seq]
	end
	if self.npc_wst_data.is_end then
		re_str = self.npc_wst_data.data
		if is_next then re_str = nil end
	end
	return re_str
end

function NpcDialogData:GetLastTalkSeqInfo()
	return self.cur_npc_talk_list[self.cur_npc_talk_list.max_seq]
end

function NpcDialogData:GetButtonList()
	return self.button_list
end

function NpcDialogData.GetNpcConfig(data)
	local npc_co = NpcHelper.NpcCo(data.id,false) or Cfg.MonsterById(data.id)
	if data.obj_type == SceneObjType.Monster and data.use_monster_cfg == true and data.monster_group_id and data.client_show_id then
		npc_co = Cfg.MonsterById(data.client_show_id,false) or Cfg.NpcById(data.client_show_id, false)
	end
	return npc_co
end

function NpcDialogData:RandBubbleById(bubble_id)
	local co = self.npc_bubble_list[bubble_id]
	for k,v in pairs(self.npc_bubble_special_list[bubble_id] or {}) do
		if v[1] and ActivityData.Instance:GetActivityStatus(v[1].param) == ActStatusType.Open then
			return v[math.random(#v)].talk_text or ""
		end
	end
	return co and co[math.random(#co)].talk_text or ""
end

function NpcDialogData:GetBubbleData(bubble_id,index)
	local co = self.npc_bubble_list[bubble_id] or {}
	if index then
		return co[index] and co[index].talk_text or ""
	end
	return co
end

function NpcDialogData:SyncTalkInfo(talk_info)
	if talk_info and talk_info.talk_id then
		local co = Config.npc_talk_list_auto.promeval_paradise_talk[talk_info.talk_id]
		if co then
			if 1 == co.is_tianjie then
				HarmonyHeavenCtrl.Instance:SendHongMengTianJieReqTalk(talk_info.talk_id, talk_info.talk_seq)
				if nil == self.sync_end_func then
					self:SetSyncEndFunc(function ()
						HarmonyHeavenCtrl.Instance:SendHongMengTianJieReqTalk()	
					end)
				end
			end
		end
	end
end

function NpcDialogData:SyncEndFunc()
	return self.sync_end_func
end

function NpcDialogData:SetSyncEndFunc(end_func)
	self.sync_end_func = end_func
end

function NpcDialogData:NpcDialogSync(info)
	if TeamData.Instance:InTeam() and not TeamData.Instance:IsLeader() then
		self.sync_info:Set(info)
	end
end

function NpcDialogData:NpcDialogSyncBlock()
	self.sync_info:Set{talk_id = 0}
end

DetailInfo1Custom = {
	normal = 1,
	back = 2,
	quick_team = 3,
}

function NpcDialogData.CreateDetailInfo1(param_t)
	return {
		type = NpcDialogConfig.NpcDialogDetailType.type_1,
		tips = param_t.tips,
		rewards = param_t.rewards or {},
		confirm_show = param_t.confirm_show,
		custom_show = param_t.custom_show,
		custom_func = param_t.custom_func,
		act_type = param_t.act_type,
		custom_type = param_t.custom_type or DetailInfo1Custom.normal
	}
end

function NpcDialogData.GetDetailInfoBtnBackName(back_num,cost_type)
	return string.format(Language.NpcDialog.BtnBackName, RichTextHelper.SpriteStrAtlas(FindHuoBi[cost_type or 1], 40, 40), back_num)
end

function NpcDialogData.GetDetailInfoRewardsByActType(act_type)
	return ActivityData.Instance:GetActivityInfo(act_type).reward_list
end

function NpcDialogData:CustomEndFunc()
	return self.custom_end_func
end
function NpcDialogData:SetCustomEndFunc(end_func)
	self.custom_end_func = end_func
end
function NpcDialogData:GetSpTalkInfo(cur_talk)
	for index, value in ipairs(Config.npc_talk_list_auto.npc_talk_moxing) do
		if value.talk_id == cur_talk.talk_id and value.talk_seq == cur_talk.talk_seq then
			return value
		end
	end
end


NpcDialogBtnTask = NpcDialogBtnTask or BaseClass()
--str 按钮上显示的文字
--beh_id 点击按钮时执行行为的行为id
--obj_vo 交互对象vo
--task_info 任务info
function NpcDialogBtnTask:__init(obj_vo,task_info)
	-- self.beh_id = beh_id
	self.obj_vo = obj_vo
	-- self.dis_str = str
	self.task_info = task_info
	self.detail_info = task_info:DetailInfo()
end

function NpcDialogBtnTask:Exe()
	-- LogError("Exe");
		-- local npc_dia_data = NpcDialogCtrl.Instance:Data()
		-- npc_dia_data:SetCurTaskInfo(self.task_info)	
		-- if self.task_info ~= nil then
		-- local t_st = self.task_info:State()
		-- local t_cfg = self.task_info:AutoCfg()
		-- local beh_id = 0
		-- if t_st ==  TaskConfig.TaskState.done then
		-- 	beh_id = t_cfg.commit_dialog_beh
		-- elseif t_st == TaskConfig.TaskState.undo then
		-- 	beh_id = t_cfg.accept_dialog_beh
		-- elseif t_st == TaskConfig.TaskState.doing then	--正在做任务时找NPC，重复一下接取时的对话
		-- 	beh_id = t_cfg.accept_dialog_beh
		-- end	
		-- if beh_id > 0 then
		-- 	local beh_cfg = BehaviorCtrl.GetCfg(beh_id,true)
		-- 	beh_cfg.task_info = self.task_info
		-- 	BehaviorCtrl.ExeByCfgVo(beh_cfg,self.obj_vo)
		-- end
		-- if (beh_id <= 0 or 
		-- 	BehaviorCtrl.BehType(beh_id) ~= InteractionNpcTalk) and 
		-- 	(t_st ~= TaskConfig.TaskState.doing) then		--纠结点
		-- 	ViewMgr:CloseView(NpcDialogView)
		-- end
		-- end
	TaskData.Instance:TraceTask(self.task_info.task_type, self.task_info.task_id)
	self.task_info:GoTask(true)
end

function NpcDialogBtnTask:DisStr()
	-- return self.dis_str
	-- if self.task_info ~= nil then
	if self.task_info.custom_str then
		return self.task_info.custom_str
	end
	    	local t_st = self.task_info:State()
	    	local t_cfg = self.task_info:AutoCfg()
	    	if t_st ==  TaskConfig.TaskState.done then
	    		return t_cfg.commit_btn_txt
	    	elseif t_st == TaskConfig.TaskState.undo then
	    		return t_cfg.accept_btn_txt
	    	elseif t_st == TaskConfig.TaskState.doing then
	    		return t_cfg.dialog_btn_txt
	    	end
	    -- else
	    -- 	return ""
	    -- end

end


function NpcDialogBtnTask:IconSp()
	-- return TaskConfig.MainViewFuncImgConvert.list_type_new[self.task_info.list_type]
	if self.task_info.task_type == TaskConfig.TaskType.guild then
		return NpcIconTypeToSp[NpcIconType.Act]
	end
end

NpcDialogBehBtnData = NpcDialogBehBtnData or BaseClass()

function NpcDialogBehBtnData:__init(dis_str)
	self.dis_str = dis_str
	self.dis_trans_func = nil
end

function NpcDialogBehBtnData:SetDisstrTransFunc(func)
	self.dis_trans_func = func
end

function NpcDialogBehBtnData:__delete()

end

function NpcDialogBehBtnData:Exe()

end

function NpcDialogBehBtnData:DisStr()
	local re_str = self.dis_trans_func ~= nil and self.dis_trans_func(self.dis_str) or self.dis_str
	return re_str
end

function NpcDialogBehBtnData:SetIconType(icon_type)
	self.icon_type = icon_type
end

function NpcDialogBehBtnData:IconSp()
	return self.icon_type and NpcIconTypeToSp[self.icon_type] or nil
end

function NpcDialogBehBtnData:SetIsGrey(is_grey)
	self.is_grey = is_grey
end

function NpcDialogBehBtnData:IsGrey()
	return self.is_grey
end

NpcDialogBehBtnDataFuncExe = NpcDialogBehBtnDataFuncExe or BaseClass(NpcDialogBehBtnData)

function NpcDialogBehBtnDataFuncExe:__init(dis_str,exe_func,auto_close_npc_dialog, detail_info)
	self.exe_func = exe_func
	self.auto_close_npc_dialog = auto_close_npc_dialog
	self.detail_info = detail_info
end

function NpcDialogBehBtnDataFuncExe:Exe()
	if self.exe_func ~= nil then
		self.exe_func()
	end
	if self.auto_close_npc_dialog == true then
		ViewMgr:CloseView(NpcDialogView)
	end
end

NpcDialogBehBtnDataBehCfg = NpcDialogBehBtnDataBehCfg or BaseClass(NpcDialogBehBtnData)

function NpcDialogBehBtnDataBehCfg:__init(dis_str,beh_cfg,obj_vo, detail_info)
	self.beh_cfg = beh_cfg
	self.obj_vo = obj_vo
	self.detail_info = detail_info
end

function NpcDialogBehBtnDataBehCfg:Exe()
	BehaviorCtrl.ExeByCfgVo(self.beh_cfg,self.obj_vo)
	if BehaviorCtrl.BehCfgType(self.beh_cfg) ~= InteractionNpcTalk then
		ViewMgr:CloseView(NpcDialogView)
	end
end

NpcDialogBehBtnDataBehId = NpcDialogBehBtnDataBehId or BaseClass(NpcDialogBehBtnData)

function NpcDialogBehBtnDataBehId:__init(dis_str,beh_id,obj_vo)
	self.beh_id = beh_id
	self.obj_vo = obj_vo

	self:Specials()
end

function NpcDialogBehBtnDataBehId:Exe()
	BehaviorCtrl.Exe(self.beh_id,self.obj_vo)

	if BehaviorCtrl.BehType(self.beh_id) ~= InteractionNpcTalk and BehaviorCtrl.BehType(self.beh_id) ~= InteractionNpcTalk2 and BehaviorCtrl.BehType(self.beh_id) ~= InteractionSkipBattle and BehaviorCtrl.BehType(self.beh_id) ~= InteractionBountyTaskSkip 
	and BehaviorCtrl.BehType(self.beh_id) ~= InteractionNpcTalk3  then
		ViewMgr:CloseView(NpcDialogView)
	end
end

function NpcDialogBehBtnDataBehId:Specials()
	--宝图任务
	if 5013 == self.beh_id then
		self.detail_info = NpcDialogData.CreateDetailInfo1{tips = Language.TaskDesc[self.beh_id], rewards = TreasureMapData.Instance:Reward2()}
		self.icon_type = NpcIconType.Act
	end
	-- 真中
	if 5011 == self.beh_id then
		self.detail_info = NpcDialogData.CreateDetailInfo1{tips = Language.TaskDesc[self.beh_id], rewards = ResearchTaskData.Instance:Reward2()}
		self.icon_type = NpcIconType.Act
	end
	--世界boss(魔将入侵)
	if 5001 == self.beh_id then
		self.detail_info = WorldBossCtrl.Instance:Data():GetDetailInfo(ActType.WorldBoss, self.obj_vo.name)
	end
	if 5004 == self.beh_id then
		self.detail_info = WorldBossCtrl.Instance:Data():GetDetailInfo(ActType.WorldBoss2, self.obj_vo.name)
	end
	-- 锢魔之塔
	if 93 == self.beh_id then
		self.icon_type = NpcIconType.Act
	end
	-- 勇闯地宫
	if 92 == self.beh_id then
		local params = {}
		params.tips = DataHelper.TableCopy(Language.TaskDesc[self.beh_id])
		-- params.tips[2] = string.format(params.tips[2], RushDungeonData.Instance:GetOpenLevel())
		-- params.tips[2] = string.format(params.tips[2], RushDungeonData.Instance:GetOpenLevel())
		params.rewards = RushDungeonData.Instance:GetPreReward()
		params.custom_show = Language.Team.FastTeam
		params.custom_type = DetailInfo1Custom.quick_team
		params.custom_func = BindTool.Bind(self.FastTeamFunc, self, 7)
		self.detail_info = NpcDialogData.CreateDetailInfo1(params)
		self.icon_type = NpcIconType.Act
	end
	-- 四象幻境
	if 5062 == self.beh_id then 
		self.detail_info = NpcDialogData.CreateDetailInfo1{tips = Language.TaskDesc[self.beh_id], rewards = FightData.Instance:GetFantasyPreReward()}
		self.icon_type = NpcIconType.Act
	end
	-- 山海灯会
	if 31200 == self.beh_id then
		self.detail_info = NpcDialogData.CreateDetailInfo1{tips = Language.TaskDesc[self.beh_id], rewards = MSLanternData.Instance:Reward()}
		self.icon_type = NpcIconType.Act
	end
	-- 科举考试
	if 31210 == self.beh_id then
		self.detail_info = NpcDialogData.CreateDetailInfo1{tips = Language.TaskDesc[self.beh_id], rewards = ImperialExamData.Instance:Reward()}
		self.icon_type = NpcIconType.Act
	end
end
function NpcDialogBehBtnData:FastTeamFunc(target_id)
	local target = TeamData.Instance.team_goal_list[target_id]
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
end

--dis_str npc按钮显示的基础字符串

--exe_func 点击按钮的时候执行的过程
--auto_close_npc_dialog 点击按钮之后是否自动关闭npc框 default:true
function NpcDialogBehBtnData.FuncCreate(dis_str,exe_func,auto_close_npc_dialog, detail_info)
	if auto_close_npc_dialog == nil then
		auto_close_npc_dialog = true
	elseif type(auto_close_npc_dialog) == "function" then
		auto_close_npc_dialog = Call(auto_close_npc_dialog)
	end
	return NpcDialogBehBtnDataFuncExe.New(dis_str,exe_func,auto_close_npc_dialog, detail_info)
end

--beh_cfg behavior_cfg
--obj_vo npc的vo
function NpcDialogBehBtnData.BehCfgCreate(dis_str,beh_cfg,obj_vo, detail_info)
	return NpcDialogBehBtnDataBehCfg.New(dis_str,beh_cfg,obj_vo, detail_info)
end

--beh_id
--obj_vo npc的vo
function NpcDialogBehBtnData.BehIdCreate(dis_str,beh_id,obj_vo)
	return NpcDialogBehBtnDataBehId.New(dis_str,beh_id,obj_vo)
end

-- NpcDialogBehBtnData = NpcDialogBehBtnData or BaseClass()

-- --beh num:beh_id;table:beh_cfg
-- function NpcDialogBehBtnData:__init(dis_str,beh,obj_vo)
-- 	self.dis_str = dis_str
-- 	if type(beh) == "number" then
-- 		self.beh_id = beh
-- 	else
-- 		self.beh_cfg = beh
-- 		self.beh_id = behavior_id
-- 	end
-- 	self.obj_vo = obj_vo

-- 	self:CheckSpecial()
-- end

-- function NpcDialogBehBtnData:behType()
-- 	if self.beh_cfg == nil then
-- 		if self.beh_id == nil or self.beh_id <= 0 then
-- 			return nil
-- 		else
-- 			return BehaviorCtrl.BehType(self.beh_id)
-- 		end
-- 	else
-- 		return BehaviorCtrl.BehCfgType(self.beh_cfg)
-- 	end
-- end

-- function NpcDialogBehBtnData:Exe()
-- 	local beh_type
-- 	if self.beh_cfg == nil then
-- 		BehaviorCtrl.Exe(self.beh_id,self.obj_vo)
-- 	else
-- 		BehaviorCtrl.ExeByCfgVo(self.beh_cfg,self.obj_vo)
-- 	end
-- 	if self:behType() ~= InteractionNpcTalk then
-- 		ViewMgr:CloseView(NpcDialogView)
-- 	end
-- end

-- function NpcDialogBehBtnData:DisStr()
-- 	return self.dis_str
-- end

-- function NpcDialogBehBtnData:CheckSpecial()
-- 	if self:behType() == InteractionWish then
-- 		self.dis_str = self.dis_str .. "(" .. (ActivityData.Instance.act_wish_info.trevi_fountain_today_use_count or 0) .. "/" .. ActivityData.Instance.act_wish_max_num .. ")"
-- 	end
-- end