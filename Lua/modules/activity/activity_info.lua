ActivityInfo = ActivityInfo or BaseClass()

function ActivityInfo:__init(param_t)
    self.info = param_t.info
    self.config = param_t.config
    self:ActivityInfo()
end

function ActivityInfo:ActivityInfo()
    self.act_seq = self.info.act_seq
    self.act_type = self.info.act_type
    self.is_show = 1 == self.info.is_show
    self.act_name = self.info.name_text
    self.group_info = {}
    self.total_num = 0
    self.is_open_new = false
    self.state_buttons = {}
    self.status = self.act_type > 10000 and ActStatusType.Open or ActStatusType.Close
    self.art_red_point = SmartData.New {val = false}
    self.rp_ignore_status = false   --ignore_status 红点是否忽略活动状态都判断，默认为false
    self.activity_type_list = {}
    self:CheckClickHandle()
    self:CheckPushInfo()
end

function ActivityInfo:CheckLevelData()
    local config = ActivityData.Instance:GetActRewardData(self.act_type)
    if config == nil then 
        LogError("活动奖励预览配置不存在 act_type：" .. tostring(self.act_type))
        return 
    end
    self:CheckRewardList(config)
    self:CheckActivityTypeList(config)
    self:CheckLabelList(config)
end

function ActivityInfo:CheckActivityGroup()
    local ActivityGroupType = ActivityConfig.ActivityGroupType
    local act_groups = self.info.activity_group
    self.group_info.is_in_daily = self.is_show and act_groups == ActivityGroupType.daily or false
    self.group_info.is_in_time_limit = self.is_show and act_groups == ActivityGroupType.time_limit or false
    self.group_info.is_in_challenge = self.is_show and act_groups == ActivityGroupType.challenge or false
    self.group_info.is_in_future = self.is_show and self.info.level_restrict > RoleData.Instance:GetRoleLevel()
    self:ActRedPoint()
end

function ActivityInfo:CheckIsToday()
    if self.is_today == nil then
        self.is_today = self.group_info.is_in_time_limit and (ActivityData.Instance:IsDayOpen(self.info.open_day) and 1 or 0) or 0
    end
end

function ActivityInfo:GroupInfo()
    self:CheckActivityGroup()
    self:CheckIsToday()
    return self.group_info
end

function ActivityInfo:IsRemind()
    if nil == self.is_remind then
        self.is_remind = UnityPlayerPrefs.GetInt(PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(),self.act_seq)) == 0
    end
    return self.is_remind
end

function ActivityInfo:SetIsRemind(value)
    self.is_remind = value
    UnityPlayerPrefs.SetInt(PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(),self.act_seq),self.is_remind and 0 or 1)
end

function ActivityInfo:CheckRewardList(config)
    self.reward_list = {}
    local reward_ids = string.split(config.reward_id or self.info.reward_id, ":")
    for i = 1, #reward_ids do
        local item_data = Item.New()
        item_data.item_id = tonumber(reward_ids[i])
        item_data.num = 1
        item_data.is_bind = 0
        self.reward_list[i] = item_data
    end
end

function ActivityInfo:CheckActivityTypeList(config)
    self.activity_type_list = {}
    local activity_types = string.split(config.activity_type or self.info.activity_type, ",")
    for i = 1, #activity_types do
        self.activity_type_list[tonumber(activity_types[i])] = true
    end
    self.is_high_yield = self.activity_type_list[2]
end

function ActivityInfo:CheckLabelList(config)
    self.laber_list = {}
    local label_shows = string.split(config.label_show or self.info.label_show, ",")
    for i = 1, #label_shows do
        table.insert(self.laber_list, self.config.config_activity_label_show[tonumber(label_shows[i])])
    end
end

function ActivityInfo:CheckClickHandle()
    local beh_id = self.info.fun_name
    if beh_id and beh_id > 0 then
        self.click_handle = BindTool.Bind(BehaviorCtrl.ExeByCfgVo, BehaviorCtrl.GetCfg(beh_id, true))
    end
end

function ActivityInfo:CheckPushInfo()
    self.push_info = {}
    self.push_info.begin_times = string.split(self.info.begin_time, ",")
    self.push_info.end_times = string.split(self.info.end_time, ",")
    self.push_info.open_days = string.split(self.info.open_day, ":")
    self.push_info.is_everyday = 7 == #self.push_info.open_days
end
--主界面左侧倒计时
function ActivityInfo:CustomRemindClickHandle(func)
    self.click_remind_handle = func
end
function ActivityInfo:DoClickRemindHandle()
    if self.click_remind_handle then
        self.click_remind_handle()
        return true
    end
    return false
end
function ActivityInfo:CustomClickHandle(func)
    self.click_handle = func
end

function ActivityInfo:DoClickHandle()
    if nil ~= self.click_handle then
        SceneLogic.Instance:ClearMoveCache()
		self.click_handle()
    end
end

function ActivityInfo:OnClickHandle()
    if nil ~= self.click_handle then
        SceneLogic.Instance:ClearMoveCache()
        return self.click_handle()
    end
end

function ActivityInfo:ActStatus(status)
	self.status = status
end

function ActivityInfo:CustomStatusButtonName(status, button_name)
    self.state_buttons[status] = button_name
end

function ActivityInfo:CustomButtonNameFunc(func)
    if func == nil then
        if self.button_name_func then
            return self.button_name_func(self.info)
        end
    else
        self.button_name_func = func
    end
end

function ActivityInfo:ActRedPoint()
    local val = false
    if self.group_info.is_in_future then
        val = false
    else
        if (self.rp_ignore_status == true) or (ActStatusType.Open == self.status) then
            if self.group_info.is_in_challenge then
                local red_func = ActivityData.Instance:GetRegisterList(self.act_type)
                if red_func ~= nil then
                    val = red_func() > 0
                end
            end
        end
    end
    self.art_red_point.val = val
    if nil == next(self.group_info) then self:CheckActivityGroup() end
    if self.group_info.is_in_challenge then ActivityData.Instance:ChallengeRedPoint() end
end

function ActivityInfo:TeamTargetShow()
    local is_show = true
    if ActType.WorldBoss == self.act_type or ActType.ResearchTask == self.act_type then return is_show end
    is_show = is_show and ActStatusType.Open == self.status
    is_show = is_show or ActStatusType.Standy == ActivityData.Instance:GetActivityStatus(self.act_type)
    is_show = is_show and (0 == self.total_num or self.cur_num < self.total_num)
    is_show = is_show and (not self.group_info.is_in_future)
    return not is_show
end

function ActivityInfo:CheckLevelOpen()
    if not self.is_show then
        return
    end
    local role_level = RoleData.Instance:GetRoleLevel()
    local level_open = role_level >= self.info.level_restrict
    if self.level_open == false and level_open == true then
        self.is_open_new = true
    else
        self.is_open_new = false
    end
    self.level_open = level_open
end

function ActivityInfo:LevelRestrict()
    return self.info.level_restrict
end

function ActivityInfo:GetTimesCondition()
    if self.total_num == 0 then
        return Language.Active.Condition
    end
    return Format(Language.Active.Times,Valve(self.cur_num,self.total_num),self.total_num)
end

function ActivityInfo:IsComplete()
    if self.total_num == 0 then
        return false
    end
    return self.cur_num >= self.total_num
end

function ActivityInfo:IsToday()
    return ActivityData.Instance:IsDayOpen(self.info.open_day)
end

function ActivityInfo:GetTimesProg()
    return Valve(self.cur_num,self.total_num), self.total_num
end
