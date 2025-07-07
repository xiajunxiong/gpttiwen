VIEW_REQ("modules/reward/sports_cards/sports_cards_view")
VIEW_REQ("modules/reward/sports_cards/sports_cards_bar")

SPORTS_CARDS_NOTICES = {
	FETCH_COMMON_SUCC = 0,      -- 领取普通奖励成功 P1:card_level
	FETCH_SENIOR_SUCC = 1,      -- 领取高级奖励成功 P1:card_level
	BUY_LEVEL = 2,              -- 购买等级 P1:cur_role_card_level
	BUY_CMD_SUCC = 3,           -- 直购购买成功 P1:is_active_buy_flag
	DAY_TASK_FINISH = 4,        -- 每日任务完成 P1:task_index
	WEEK_TASK_FINISH = 5,       -- 每周任务完成 P1:task_index
	SEASON_TASK_FINISH = 6,     -- 赛季任务完成 P1:task_index
	LEVEL_UP = 7,               -- 等级提升 P1:cur_role_card_level
	EXP_CHANGE = 8,             -- 经验变化 P1:role_card_exp
}

SportsCardsCtrl = SportsCardsCtrl or BaseClass(BaseCtrl)
function SportsCardsCtrl:__init()
	if SportsCardsCtrl.Instance ~= nil then
		Debuger.LogError("[SportsCardsCtrl] attempt to create singleton twice!")
		return
	end	
	SportsCardsCtrl.Instance = self
	self.data = SportsCardsData.New()
    self:RegisterAllProtocols()

	local sports_func = BindTool.Bind(self.FlushSportsRemind, self)
	RemindCtrl.Instance:Remind():Register(
        Mod.Reward.SportsCards,
        self.data.base_info,
        sports_func
    )
end

function SportsCardsCtrl:FlushSportsRemind()
	local num = self.data:CheckSportsRemind()
    return num 
end

function SportsCardsCtrl:RegisterAllProtocols()
	self:RegisterProtocol(SCRAJingJiYueKaAllInfo, "RecvJingJiYueKaAllInfo")
    self:RegisterProtocol(SCRAJingJiYueKaNotice, "RecvJingJiYueKaNotice")
    self:RegisterProtocol(SCRAJingJiYueKaRefreshNotice, "RecvJingJiYueKaRefreshNotice")
	self:RegisterProtocol(SCRAJingJiYueKaSingleTypeInfo, "RecvJingJiYueKaSingleTypeInfo")
	self:RegisterProtocol(SCRAJingJiYueKaTaskListInfo, "RecvJingJiYueKaTaskListInfo")
end

function SportsCardsCtrl:RecvJingJiYueKaAllInfo(protocol)
	self.data:SetAllInfo(protocol)
end

function SportsCardsCtrl:RecvJingJiYueKaNotice(protocol)
	self.data:FlushInfoByNotice(protocol)
end

function SportsCardsCtrl:RecvJingJiYueKaRefreshNotice(protocol)
	self.data:SetTaskInfo(protocol)
end

function SportsCardsCtrl:RecvJingJiYueKaSingleTypeInfo(protocol)
	self.data:SetTaskProgress(protocol)
end

function SportsCardsCtrl:RecvJingJiYueKaTaskListInfo(protocol)
	self.data:SetTaskListInfo(protocol)
end

--===========================SportsCardsData===========================
SportsCardsData = SportsCardsData or BaseClass()
function SportsCardsData:__init()
    if SportsCardsData.Instance ~= nil then
        Debuger.LogError("[SportsCardsData] attempt to create singleton twice!")
        return
    end
    SportsCardsData.Instance = self
	-- 基础信息
	self.base_info = SmartData.New({
		active_role_level = -1,
		next_switch_timestamp = 0,
		last_switch_timestamp = 0,
		is_active_buy_flag = 0,
		role_card_level = 0,
		role_card_exp = 0,
		fetch_common_flag = {},
		fetch_adv_flag = {},
	})
	-- 任务信息
	self.task_info = SmartData.New({
		day_list = {},
		week_list = {},
		season_list = {},
	})
	-- 进度列表
	self.progress_info = SmartData.New({
		list = {},
	})
	-- 界面交互信息
	self.view_data = SmartData.New({
		flush_num = -1,
	})
end

-- 全信息刷新
function SportsCardsData:SetAllInfo(protocol)
	-- LogError("!3140?@杰哥救救我",protocol)
	self.base_info.active_role_level = protocol.active_role_level
	self.base_info.next_switch_timestamp = protocol.next_switch_timestamp
	self.base_info.last_switch_timestamp = protocol.last_switch_timestamp
	self.base_info.is_active_buy_flag = protocol.is_active_buy_flag
	self.base_info.card_level = protocol.role_card_level
	self.base_info.card_exp = protocol.role_card_exp
	self.base_info.common_flag = protocol.fetch_common_flag
	self.base_info.adv_flag = protocol.fetch_adv_flag

	self.task_info.day_list = protocol.day_task_list
	self.task_info.week_list = protocol.week_task_list
	self.task_info.season_list = protocol.season_task_list

	self.progress_info.list = protocol.progress_list

	self:FlushCall()
end

function SportsCardsData:FlushCall()
	self.view_data.flush_num = self.view_data.flush_num + 1
end

function SportsCardsData:FlushInfoByNotice(protocol)
	-- LogError("!3141?@杰哥救救我",protocol)
	if protocol.notice_type == SPORTS_CARDS_NOTICES.FETCH_COMMON_SUCC then 
		self.base_info.common_flag[64-protocol.param1] = 1
	elseif protocol.notice_type == SPORTS_CARDS_NOTICES.FETCH_SENIOR_SUCC then 
		self.base_info.adv_flag[64-protocol.param1] = 1
	elseif protocol.notice_type == SPORTS_CARDS_NOTICES.BUY_LEVEL then 
        self.base_info.card_level = protocol.param1
	elseif protocol.notice_type == SPORTS_CARDS_NOTICES.BUY_CMD_SUCC then 
		self.base_info.is_active_buy_flag = protocol.param1
	elseif protocol.notice_type == SPORTS_CARDS_NOTICES.DAY_TASK_FINISH then 
		if self.task_info.day_list[protocol.param1] then self.task_info.day_list[protocol.param1].finish_flag = 1 end 
	elseif protocol.notice_type == SPORTS_CARDS_NOTICES.WEEK_TASK_FINISH then 
		self.task_info.week_list[protocol.param1].finish_flag = 1
	elseif protocol.notice_type == SPORTS_CARDS_NOTICES.SEASON_TASK_FINISH then 
		self.task_info.season_list[protocol.param1].finish_flag = 1
	elseif protocol.notice_type == SPORTS_CARDS_NOTICES.LEVEL_UP then 
		self.base_info.card_level = protocol.param1
	elseif protocol.notice_type == SPORTS_CARDS_NOTICES.EXP_CHANGE then 
		self.base_info.card_exp = protocol.param1
	end 

	self:FlushCall()
end

-- 本下发主要用于元宝请求刷新
function SportsCardsData:SetTaskInfo(protocol)
	-- LogError("!3142?@杰哥救救我",protocol)
	-- protocol.task_index
	-- protocol.task_info
	self.task_info.day_list[protocol.task_index] = protocol.task_info

	self:FlushCall()
end

function SportsCardsData:SetTaskProgress(protocol)
	-- LogError("!3143?@杰哥救救我",protocol)
	-- protocol.task_type
	-- protocol.prog_list
	for k,v in pairs(self.progress_info.list) do 
		v[protocol.task_type] = protocol.prog_list[k]
	end 

	self:FlushCall()
end

function SportsCardsData:SetTaskListInfo(protocol)
	-- LogError("!3144?@杰哥救救我",protocol)
	-- protocol.task_group_type
	-- protocol.task_list
	if protocol.task_group_type == 0 then 
		self.task_info.season_list = protocol.task_list
	elseif protocol.task_group_type == 1 then 
		self.task_info.day_list = protocol.task_list
	elseif protocol.task_group_type == 2 then 
		self.task_info.week_list  = protocol.task_list
	end 

	self:FlushCall()
end

function SportsCardsData:CheckIsCatch(ids,level)
	if level == 1 then 
		return self.base_info.common_flag[64 - ids] == 1
	else 
		return self.base_info.adv_flag[64 - ids] == 1
	end 
	
end

function SportsCardsData:CheckIsSeniorCard()
	return self.base_info.is_active_buy_flag == 1
end

function SportsCardsData:GetLevelInfo()
	local cfg = self:CatchLevelCfg()
	return {level = self.base_info.card_level,exp = self.base_info.card_exp,total = cfg.upgrade_exp,}
end

-- -- paramt {level_plus}
-- function SportsCardsData:CountLevelInfo(param_t)
-- 	-- 
-- end

function SportsCardsData:CatchCfg()
	local list = Config.jingjiyueka_auto.basis_configuration
	local role_level = self.base_info.active_role_level

	local result = {}
	for k,v in pairs(list) do 
		if v.level_up > 0 and role_level >= v.level_down and role_level <= v.level_up then 
			table.insert(result,v)
		elseif v.level_up == 0 and role_level >= v.level_down  then 
			table.insert(result,v)
		end 
	end 
	table.sort(result,DataSorter.KeyLowerSorter("seq"))

	return result
end

function SportsCardsData:CatchLevelCfg(level)
	local oper_level = level == nil and self.base_info.card_level or level
	local oper_list = self:CatchCfg()

	return oper_list[oper_level + 1]
end

function SportsCardsData:GetSportsCardsList()
	local list = self:CatchCfg()

	local result = {}
	for k,v in pairs(list) do 
		local vo = {}
		vo.cfg = v
		vo.common_flag = self:CheckIsCatch(v.seq,1)
		vo.adv_flag = self:CheckIsCatch(v.seq,2)
		if v.seq > 0 then table.insert(result,vo) end 
	end 

	return result
end

function SportsCardsData:CatchTaskCfg(group,task_id)
	-- LogError("!what?happy!",group,task_id)
	for k,v in pairs(Config.jingjiyueka_auto.task_configuration) do 
		if v.task_group == group and v.task_id == task_id then 
			return v
		end 
	end 
	return nil
end

function SportsCardsData:GetTaskList(group)
	local result = {}
	local operate_list = group == 0 and self.task_info.season_list or (group == 1 and self.task_info.day_list or self.task_info.week_list)
	for k,v in pairs(operate_list) do 
		local vo = {}
		vo.cfg = self:CatchTaskCfg(group,v.task_id)
		vo.task_index = k
		vo.task_id = v.task_id 
		vo.finish_flag = v.finish_flag
		vo.refresh_num = v.refresh_num
		if vo.cfg ~= nil then 
		    table.insert(result,vo)
		end 
	end 

	return result
end

function SportsCardsData:CatchTaskInfo(group,task_id)
	local operate = group == 0 and self.task_info.season_list or (group == 1 and self.task_info.day_list or self.task_info.week_list)
	for k,v in pairs(operate) do 
		if v.task_id== task_id then 
			return v
		end 
	end 
end

function SportsCardsData:DayilyTaskInfo(task_index)
	-- LogError("!?付出就哭了！@",task_index, self.task_info.day_list)
	return self.task_info.day_list[task_index]
end

function SportsCardsData:CatchProgress(group,task_type)
	-- LogError("!@?",group,task_type)
	-- LogError("!ewss?",self.progress_info.list)
	return self.progress_info.list[group][task_type]
end

function SportsCardsData:IsLevelMax(pluscheck)
	local list = self:CatchCfg()
	local level = pluscheck == nil and self.base_info.card_level or pluscheck
	return level >= list[#list].seq
end

function SportsCardsData:MaxLevel()
	local list = self:CatchCfg()
	return list[#list].seq
end

function SportsCardsData:GetCurTaskNumCfg()
	local cfg = Config.jingjiyueka_auto.task_num
	local role_level = self.base_info.active_role_level
	for k,v in pairs(cfg) do 
		if (v.level_down <= role_level and role_level <= v.level_up) 
		    or (role_level >= v.level_down and v.level_up == 0) then 
			return v
		end 
	end 
end

function SportsCardsData:GetCanLevelBuyTime()
	local last_time = self.base_info.last_switch_timestamp
	local cfg = Config.jingjiyueka_auto.other[1]
	local day_pass = TimeHelper.FormatDHMS(TimeCtrl.Instance:GetServerTime() - last_time)

	return day_pass.day >= cfg.open_day
end

function SportsCardsData:CheckSportsRemind()
	-- 功能开启
	if FunOpen.Instance:GetFunIsOpened(Mod.Reward.SportsCards) == false then return 0 end
	-- 活动开启
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_JING_JI_YUE_KA) then return 0 end
	-- 信息丢失
	if self.base_info.active_role_level == -1 then return 0 end 

	for i = 1,self.base_info.card_level do 
		if (not self:CheckIsCatch(i,1)) then 
			return 1
		elseif (not self:CheckIsCatch(i,2)) and self.base_info.is_active_buy_flag == 1 then 
			return 1
		end 
	end 

	return 0 
end