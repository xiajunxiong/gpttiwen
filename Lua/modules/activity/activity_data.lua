ActivityData = ActivityData or BaseClass()

function ActivityData:__init()
	if ActivityData.Instance ~= nil then
		Debuger.LogError("[ActivityData] attempt to create singleton twice!")
		return
	end	
	ActivityData.Instance = self
	self:ActivityConfig()
	self:ActivityData()
	self:ActivityDataActs()
	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))
	self.handle_role_level = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.RoleLevelGuide, self), "level")
	self.time_limit_act_reward_get_flag = {}
	self.time_limit_act_reward_func = {}
end

function ActivityData:__delete()
	EventSys:UnBind(self.complete_handle)
end

function ActivityData:RoleNoticeComplete()
	ActivityCtrl.Instance:SendActiveSystemReq()
	ActivityCtrl.Instance:SendActiveSystemReq(5)
	for _, item in pairs(self.activity_list) do
		item:CheckLevelData()
	end
end

function ActivityData:RoleLevelGuide()
	for _, item in pairs(self.activity_list) do
		item:CheckLevelOpen()
	end
end

function ActivityData:ActivityConfig()
	self.config = Config.daily_activity_auto
	self.config_daily = self.config.daily
	self.config_activity_label_show = self.config.activity_label_show
	self.config_activity_type_cfg = self.config.activity_type_cfg
	self.config_week_time = self.config.week_time
	self.info_config = {}
	local config_activity_label_show = {}
	for _, v in pairs(self.config_activity_label_show or {}) do
		config_activity_label_show[v.label_seq] = v
	end
	self.info_config.config_activity_label_show = config_activity_label_show
	
	self.activity_show_key_list = KeyList(Config.daily_activity_auto.activity_show,"act_type")
	local activity_show = {}
	for k,v in pairs(self.config.activity_show) do
		activity_show[v.act_type] = v.activity_show
	end
	self.config.activity_show = activity_show
end

function ActivityData:ActivityData()
	self:TypeList()
	self:ActivityList()
	self:ActivityWeekList()
	self:ActivityPushList()

	self.daily_red_point = SmartData.New{ val = false, flush = true }
	self.challenge_red_point = SmartData.New{ val = false }
end

function ActivityData:TypeList()
	self.type_list = {}
	for k, v in pairs(self.config_activity_type_cfg or {}) do
		self.type_list[v.activity_seq] = v
		if 1 == v.activity_seq then self.default_type = v end
	end
end

function ActivityData:GetTypeList()
	return self.type_list
end

function ActivityData:ActivityList()
	self.activity_list = SmartData.New()
	self.activity_status_list = {}
	self.activity_group_list = {}
	self.activity_status_event = SmartData.New()
	for _, v in pairs(self.config_daily) do
		self.activity_list[v.act_type] = SmartData.New(ActivityInfo.New{info = v, config = self.info_config})
	end
end

function ActivityData:GetActivityList()
	return self.activity_list
end

function ActivityData:CheckActivityListGroup()
	for _, item in pairs(self.activity_list) do
		item:ActRedPoint()
	end
end

function ActivityData:GetActivityListByGroup(act_group, act_type)
	local list = {}
	local group_key = ActivityConfig.ActivityGroupKey[act_group]
	for _, item in pairs(self.activity_list) do
		if item:GroupInfo()[group_key] and item.activity_type_list[act_type or 1] then
			table.insert(list, item)
		end
	end
	--开启0 已完成1 准备中1 未开启 2  未解锁3
	table.sort(list, function (a, b)
		if not a or not b then return a end
		local a_weight = ActStatusType.Open == a.status and 0 or 2
		local b_weight = ActStatusType.Open == b.status and 0 or 2
		a_weight = (a.total_num > 0 and a.total_num == a.cur_num) and 1 or a_weight
		b_weight = (b.total_num > 0 and b.total_num == b.cur_num) and 1 or b_weight
		a_weight = ActStatusType.Standy == a.status and 1 or a_weight
		b_weight = ActStatusType.Standy == b.status and 1 or b_weight
		a_weight = a.group_info.is_in_future and 3 or a_weight
		b_weight = b.group_info.is_in_future and 3 or b_weight
		a_weight = a.is_today == 1 and 1.5 or a_weight
		b_weight = b.is_today == 1 and 1.5 or b_weight
		a_weight = (a.is_today == 1 and ActStatusType.Open == a.status) and 0.5 or a_weight
		b_weight = (b.is_today == 1 and ActStatusType.Open == b.status) and 0.5 or b_weight
		if a_weight == b_weight then
			local a_level = a.info.level_restrict
			local b_level = b.info.level_restrict
			if 3 == a_weight and a_level ~= b_level then
				return a_level < b_level
			else
				return a.act_seq < b.act_seq
			end
		else
			return a_weight < b_weight
		end
	end)
	return list
end

function ActivityData:ActivityWeekList()
	self.activity_week_up_list = {}
	local week_up = Language.Activity.WeekUp
	for i = 0, 7 do
		self.activity_week_up_list[i] = {
			name = week_up[i]
		}
	end
	self.activity_week_down_list = {}
	for _, v in pairs(self.config_week_time or {}) do
		self.activity_week_down_list[v.time_seq] = {}
		self.activity_week_down_list[v.time_seq][0] = {
			act_show_time = v.act_show_time,
		}
		for i = 1, 7 do
			self.activity_week_down_list[v.time_seq][i] = {
				act_name = v["week_" .. i],
				info = ActivityData.GetConfig(v["act_id_" .. i]),
				week_num = 7 == i  and 0 or i,
			}
		end
	end
end

function ActivityData:GetActivityWeekList()
	return self.activity_week_up_list, self.activity_week_down_list
end

function ActivityData:GetWeekNum()
	local server_time = TimeManager:GetServerTime()
	local week_num = tonumber(os.date("%w", server_time)) or 0
	return week_num == 0 and 7 or week_num
end

function ActivityData:ActivityPushList()
	self.activity_push_up_list = {}
	local push_up = Language.Activity.PushUp
	for i = 1, 4 do
		self.activity_push_up_list[i] = {
			name = push_up[i]
		}
	end
end

function ActivityData:GetActivityPushList()
	return self.activity_push_up_list, self:GetActivityListByGroup(ActivityConfig.ActivityGroupType.time_limit, 1)
end

function ActivityData:SetActivityStatus(protocol)
	local act_status_info = protocol.act_status_info
	local act_type = act_status_info.act_type
	local status = act_status_info.status
	act_status_info.prefs_value = UnityPlayerPrefs.GetString(PrefKeys.ActNew(act_type))
	self.activity_status_event:Set(act_status_info)
	self.activity_status_list[act_type] = act_status_info
	if self.activity_list[act_type] then
		self.activity_list[act_type]:ActStatus(status)
		if ActStatusType.Standy == status then
			PublicPopupCtrl.Instance:ActTips(self.activity_list[act_type], act_status_info.next_status_switch_time)
		elseif ActStatusType.Open == status and ActivityConfig.NO_SHOW_TIP[act_type] == nil then
			PublicPopupCtrl.Instance:ActTips(self.activity_list[act_type], act_status_info.next_status_switch_time)
		elseif ActStatusType.Close == status then
			ActiveData.Instance:ClearLimitActClickGoto(act_type)
			local act_info = TopLayerActTipsData.Instance:GetCurShowAct()
			ActivityRemindData.Instance:DeleteRemindItem(act_type)
			if not TableIsEmpty(act_info) and act_info.act_type == act_type then
				PublicPopupCtrl.Instance:CloseActTips()
				ViewMgr:CloseView(ActivityRemindTip)
			end
		end
	end
end

function ActivityData:ChallengeRedPoint()
	local crp = false
	for _, act in pairs(self.activity_list) do
		if not crp and act.group_info.is_in_challenge then
			crp = crp or act.art_red_point.val
		end
	end
	if ArenaData.Instance:HasRedPoint() then
		crp = true
	end
	if ArenaMatchData.Instance:GetRemindNum() > 0 then
		crp = true
	end
	self.challenge_red_point.val = crp
end

function ActivityData:ClearData()
	self.activity_status_list = {}
	self.trevi_fountain_list = {}
	self.daily_red_point.val = false
	self.challenge_red_point.val = false
	for _, item in pairs(self.activity_list) do
		item.art_red_point.val = false
	end
	MainViewData.Instance:RemoveBuffs()
	self.active_data:Set({active_num = 0,reward_flag = {},progress_list = {},flag_list = {}})
end

--================================活动外部接口================================
--[[活动状态改变服务端自动下发 [act_type = ActType]
act_status_info = {
	activity_open_type 
	act_type                --活动类型
	status = ActStatusType  --活动状态
	is_broadcast            --是否广播
	next_status_switch_time --活动时间戳
	param_1 param_2
}]]

-- 活动前往按钮注册,不走默认行为,根据协议行为改变的注册
function ActivityData:CustomClickHandle(act_type, func)
	if self.activity_list[act_type] then
		self.activity_list[act_type]:CustomClickHandle(func)
	end
end

-- 主界面左侧活动倒计时按钮注册,不走默认行为,根据协议行为改变的注册
function ActivityData:CustomRemindClickHandle(act_type, func)
	if self.activity_list[act_type] then
		self.activity_list[act_type]:CustomRemindClickHandle(func)
	end
end

function ActivityData:DoClickRemindHandle(act_type)
	if self.activity_list[act_type] then
		return self.activity_list[act_type]:DoClickRemindHandle()
	end
	return false
end

-- 活动状态设置
function ActivityData:ActStatus(act_type, status)
	if self.activity_list[act_type] then
		self.activity_list[act_type]:ActStatus(status)
		self.activity_list[act_type]:ActRedPoint()
	end
end

-- 活动前往按钮名字自定义
function ActivityData:CustomStatusButtonName(act_type, status, button_name)
	if self.activity_list[act_type] then
		self.activity_list[act_type]:CustomStatusButtonName(status, button_name)
	end
end

-- 活动前往按钮名字自定义 func
function ActivityData:CustomButtonNameFunc(act_type, func)
	if self.activity_list[act_type] then
		self.activity_list[act_type]:CustomButtonNameFunc(func)
	end
end

-- 活动已完成设置(如果活动有次数限制,在每次收到协议时设置活动次数,total_num默认为0,0表示无限)
-- 特殊 0 == total_num and 0 == cur_num 无限 + 已完成
function ActivityData:SetActivityFinishInfo(act_type, cur_num, total_num)
	if self.activity_list[act_type] then
		self.activity_list[act_type].cur_num = cur_num
		self.activity_list[act_type].total_num = total_num
		self.activity_list[act_type]:ActRedPoint()
	end
end

--获取活动信息 活动状态接口 [activity_status_event = SmartData.New(act_status_info)]
function ActivityData:GetActivityStatusInfo(act_type)
	return self.activity_status_list[act_type]
end

function ActivityData:GetActivityInfo(act_type)
	return self.activity_list[act_type]
end

function ActivityData:GetActivityEventType()
	return self.activity_status_event.act_type or 0
end

function ActivityData:GetActivityEventStatus()
	return self.activity_status_event.status or ActStatusType.Close
end

function ActivityData:GetActivityEvent()
	return self.activity_status_event
end

function ActivityData:CareEvent(func)
	self.activity_status_event:Care(func, "act_type", "status")
end

function ActivityData:UnCareEvent(ht)
    self.activity_status_event:Uncare(ht)
end

--获取活动状态 Close/Standy/Open [act_type = ActType]
function ActivityData:GetActivityStatus(act_type)
	local status_info = self:GetActivityStatusInfo(act_type)
	if status_info == nil then
		return ActStatusType.Close
	end
	return status_info.status
end

-- 获取活动特殊状态 Close/Standy/Open [act_type = ActType]
function ActivityData:GetActivitySpStatus(act_type)
	local func = self.sp_status_register_list ~= nil and self.sp_status_register_list[act_type] or nil
	if func ~= nil then 
		return func()
	else 
		return nil -- ActStatusType.Close
	end 
end

-- 注册活动特殊状态
function ActivityData:RegisterActivitySpStatus(act_type,func)
	if self.sp_status_register_list == nil then
		self.sp_status_register_list = {}
	end
	self.sp_status_register_list[act_type] = func
end 

--注册活动红点事件
function ActivityData:RegisterRemind(act_type,func)
	if self.remind_register_list == nil then
		self.remind_register_list = {}
	end
	self.remind_register_list[act_type] = func
end

function ActivityData:GetRegisterList(act_type)
	return self.remind_register_list and self.remind_register_list[act_type]
end

--活动条件描述文字
function ActivityData:RegisterCondition(act_type,func)
	if self.condition_register_list == nil then
		self.condition_register_list = {}
	end
	self.condition_register_list[act_type] = func
end

function ActivityData:GetConditionList(act_type)
	return self.condition_register_list[act_type]
end

--活动条件描述文字 置顶文字
function ActivityData:RegisterTopCondition(act_type,func)
	if self.top_condition_register_list == nil then
		self.top_condition_register_list = {}
	end
	self.top_condition_register_list[act_type] = func
end

function ActivityData:GetTopConditionList(act_type)
	return self.top_condition_register_list[act_type]
end

--等级不足提示
function ActivityData:IsNoEnoughLevel(act_type,is_not_popup,tip)
	local level = RoleData.Instance:GetRoleLevel()
	local limit_level = (ActivityData.GetConfig(act_type) or {}).level_restrict or 0
	if level < limit_level and is_not_popup == nil then
		PublicPopupCtrl.Instance:Center(Format(tip or Language.WST.level_tips,limit_level))
	end
	return level < limit_level,Format(Language.Common.LevelLimit,limit_level)
end

--是否开启
function ActivityData.IsOpen(act_type)
    return ActivityData.Instance:GetActivityStatus(act_type) == ActStatusType.Open
end

-- 是否今日
function ActivityData.IsToday(act_type)
	local act_info = ActivityData.Instance:GetActivityInfo(act_type)
	if act_info then
		return act_info:IsToday()
	end
	return false
end

--获取活动配置
function ActivityData.GetConfig(act_type)
	return Config.daily_activity_auto.daily[act_type]
end

--获取活动开启时间戳
function ActivityData.GetStartStampTime(act_type)
	local act_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
	return act_info and act_info.param_1 or 0
end

--获取活动结束时间戳
function ActivityData.GetSwitchStampTime(act_type)
	local act_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
	return act_info and act_info.next_status_switch_time or 0
end

--是否满足等级
function ActivityData.IsEnoughLevel(act_type)
	return RoleLevel() >= ((ActivityData.GetConfig(act_type) or {}).level_restrict or 0)
end