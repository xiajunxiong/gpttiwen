ActiveRollerView = ActiveRollerView or DeclareMono("ActiveRollerView", UIWFlushPanel)

function ActiveRollerView:ActiveRollerView()
    self.Data = ActiveData.Instance
    self.language = Language.Active.Roller
    self.data_cares = {
        {data = RoleData.Instance.base_data, func = self.SetRollerData, keys = {"level"}, init_call = false},
        {data = self.Data.active_roller_data, func = self.FlushDown, init_call = false},
        {data = self.Data.active_roller_task_data, func = self.FlushRight, init_call = false},
        {data = self.Data.active_roller_reward_data, func = self.StartRoller, init_call = false},
        {data = ActivityData.Instance:GetActivityEvent(), func = self.FlushRight, keys = {"act_type", "status"}, init_call = false},
        {data = self.Data.weed_reward_data, func = self.FlushRight, init_call = false},
    }
end

function ActiveRollerView:Awake()
    UIWFlushPanel.Awake(self)
    self:SetRollerData()
    self:FlushDown()
    self:FlushRight()
    self:CheckAnim()
end

function ActiveRollerView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.ShowTimer)
end

function ActiveRollerView:CheckAnim()
    if self.ShowTimer == nil and ActiveData.IsListAnim == nil then
        self.ShowTimer = Invoke(function()
            self.ShowAnim:Play()
            ActiveData.IsListAnim = true
        end, 0.1)
    end
end

function ActiveRollerView:SetRollerData()
    self.roller_reward_config = self.Data:GetRollerConfig()
    local roller_data = {}
    roller_data.config = {}
    for k, v in pairs(self.roller_reward_config) do
        roller_data.config[k] = v
    end
    roller_data.end_func = BindTool.Bind(self.RollerEnd, self)
    self.Roller:SetData(roller_data)
end

function ActiveRollerView:FlushDown()
    if self.Data.active_roller_data.can_roller_times > 0 then
        UH.SetText(self.TxtHaveTimes, Format(self.language.HaveTimes[1], self.Data.active_roller_data.can_roller_times))
    else
        UH.SetText(self.TxtHaveTimes, Format(self.language.HaveTimes[2], self.Data.active_roller_data.can_roller_times))
    end
end

function ActiveRollerView:FlushRight()
    self.ItemList:SetData(self.Data:GetRollerTaskConfig())
end

function ActiveRollerView:OnLotteryClick()
    if self.Data.active_roller_data.can_roller_times > 0 then
        ActiveCtrl.Instance:SendActiveRollerReq(1)
    else
        PublicPopupCtrl.Instance:Center(self.language.NotEnoughtTip)
    end
end

function ActiveRollerView:StartRoller()
    self.ObjBlock:SetActive(true)
    local index = 1
    for k, v in pairs(self.roller_reward_config) do
        if self.Data.active_roller_reward_data.seq == v.seq then
            index = v.index
            break
        end
    end
    self.Roller:SetRollerSpReward(self.Data.active_roller_reward_data.item_list)
    self.Roller:PlayRoller(index)
end

function ActiveRollerView:RollerEnd()
    self.ObjBlock:SetActive(false)
end


----------------------------ActiveRollerRightItem----------------------------
ActiveRollerRightItem = ActiveRollerRightItem or DeclareMono("ActiveRollerRightItem", UIWidgetBaseItem)
function ActiveRollerRightItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.RightItem.name = "RightItem" .. data.sort_index
    self.Data = ActiveData.Instance
    self.language = Language.Active.Roller
    self.Language = Language.Active.WeekReward
    UH.SetText(self.TxtAddTimes, Format(self.language.AddTimes, data.task_info.turntable_num))
    self.ImgSetIcon:SetObjActive(data.task_info.task_type == 1)
    -- 1是参与限时活动，2是周活跃达到一定值
    if data.task_info.task_type == 1 then
        local act_config = ActivityData.GetConfig(data.task_info.type_param)
        UH.SetIcon(self.ImgSetIcon, act_config.icon_id, ICON_TYPE.ACT)
        self.ObjFinished:SetActive(self.Data:GetActiveRollerDayTaskFlag(data.task_info.seq) > 0)
        UH.SetText(self.TxtName, act_config.name_text)
        UH.SetText(self.TxtDesc, self:GetDesc(act_config, data.task_info.type_param))
        self.TxtDesc:SetObjActive(not ActivityData.IsOpen(data.task_info.type_param))
    elseif data.task_info.task_type == 2 then
        self.ObjFinished:SetActive(self.Data:GetActiveRollerWeekTaskFlag(data.task_info.seq) > 0)
        UH.SetText(self.TxtName, Format(self.language.WeekTaskReach, data.task_info.type_param))
        self.TxtDesc:SetObjActive(true)
        UH.SetText(self.TxtDesc, Format(self.language.WeekTaskHas, self.Data:GetWeekRewardIntegralNum()))
    end
end

function ActiveRollerRightItem:GetDesc(config, act_type)
    -- 获取周赏config
    local _, week_config = DataHelper.SimpleIndex(Config.activedegree_auto.week_activity, "act_type", act_type)
    local is_not_level_enough, tip = ActivityData.Instance:IsNoEnoughLevel(act_type, true)
    if GuildData.Instance:GetGuildID() == 0 and ActiveConfig.GuildConfig[act_type] == true then
        return self.Language.GuildTip
    elseif is_not_level_enough then
        return tip
    elseif self.Data:GetWeekRewardFinishFlag(week_config.type) == 1 then
        return self.Language.FinishTip[self:GetIndex(week_config.show_type == 2)]
    elseif self.data.act_info:IsToday() then
        local curr_time = self:Time(os.date("%H:%M", TimeManager:GetServerTime()))
        if config.begin_time == "00:00" then
            return config.time_text
        elseif curr_time >= self:Time(config.end_time) then
            return self.Language.EndTip[2]
        elseif curr_time < self:Time(config.begin_time) then
            return Format(self.Language.OpenTip, config.begin_time)
        end
    else
        return config.time_text
    end
end

function ActiveRollerRightItem:GetIndex(condition)
    return condition and 1 or 2
end

function ActiveRollerRightItem:Time(time)
    local split = Split(time, ":")
    return tonumber(split[1]) * 100 + tonumber(split[2])
end


function ActiveRollerRightItem:OnGoToClick()
    if self.data.act_info and self.data.act_info.click_handle then
        self.data.act_info.click_handle()
        ViewMgr:CloseView(ActiveView)
    end
end