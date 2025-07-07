FirstChargeData = FirstChargeData or BaseClass()

-- 协议会用到的参数
FIRST_RECHARGE_REQ_TYPE = {
	INFO = 0,	 			--请求下发
	FETCH_FIRST_REWARD = 1,	--请求领取首充奖励，param_1（配置seq）
	FETCH_EXCLUSIVE = 2,	--请求领取专属奖励，param_1（配置type）
	BUY_CMD_CHECK = 3,		--请求直购校验，param_1（FIRST_RECHARGE_CMD_TYPE），param_2（cfg_ver）
}
FIRST_RECHARGE_TYPE = {
	YIXIANER = 1, --医仙儿
}
FIRST_RECHARGE_CMD_TYPE = {
	GROW = 0,	--首充专属，成长助力里的直购
}

-- 首充专属里面有招财仙子和等级助力
FirstChargeExclusivePanelId = {
    Peri = 1,
    Grow = 2,
}
FirstChargeExclusivePanelIndex = {
    [1] = true,
    [2] = true,
}

function FirstChargeData:__init()
    if FirstChargeData.Instance ~= nil then
        Debuger.LogError("[FirstChargeData] attempt to create singleton twice!")
        return
    end
    FirstChargeData.Instance = self
    -- 首充表
    self.first_recharge_config = Config.recharge_cfg_auto.first_recharge
    -- 首充专属表
    self.exclusive_config = Config.recharge_cfg_auto.first_recharge_exclusive
    -- 充值other表
    self.other = Config.recharge_cfg_auto.other[1]
    -- 首充领取红点
    self.first_charge_red_point = SmartData.New({val = false})
    -- 首充数据
    self.first_charge_data = SmartData.New({now_select = 1, reward_flag = 0, reward_first_flag = {}})
    -- 首充直购标识，二进制，0没买 1买了，FIRST_RECHARGE_CMD_TYPE
    self.first_cmd_buy_flag = SmartData.New({flag = {}})
    -- 首充专属信息（招财仙子累计进度, 成长助力未购买结束时间戳,奖励领取标志位）
    self.exclusive_info = SmartData.New({progress = 0, end_timestamp = 0, flag = {}})
    -- 当前选中的页签id
    self.panel_select = SmartData.New({now_select_id = 0})
    self.guild_create_consume_coin_bind = 0
    self:DealFirstChargeList()
    self:DealExclusiveList()
end

function FirstChargeData:__delete()
	FirstChargeData.Instance = nil
end

function FirstChargeData:ClearData()
    self:ClearPointData()
    self.first_charge_data:Set({now_select = 1, reward_flag = 0, reward_first_flag = {}})
    self.first_cmd_buy_flag:Set({flag = {}})
    self.exclusive_info:Set({progress = 0, end_timestamp = 0, flag = {}})
    self.panel_select:Set({now_select_id = 0})
end

function FirstChargeData:ClearPointData()
	self.first_charge_red_point:Clear()
end

--==========================首充和首充专属==========================
function FirstChargeData:SetFirstChargeInfo(protocol)
	self.first_charge_data.reward_first_flag = protocol.reward_first_flag
    self.first_charge_data.reward_flag = protocol.reward_flag
    self.first_cmd_buy_flag.flag = protocol.first_cmd_buy_flag
    self.exclusive_info.progress = protocol.first_recharge_zhao_cai_progress
    self.exclusive_info.end_timestamp = protocol.first_recharge_grow_end_timestamp
    self.exclusive_info.flag = protocol.first_recharge_exclusive_flag
    self.guild_create_consume_coin_bind = protocol.guild_create_consume_coin_bind
    self.first_charge_data:Notify()
    self.exclusive_info:Notify()
    ActivityRandData.Instance:CheckRandOpenData()
	if protocol.reward_flag == 1 then
		self.first_charge_red_point.val = false
	end
end

function FirstChargeData:GetFirstChargeFlag()
	return self.first_charge_data.reward_flag
end

function FirstChargeData:DealFirstChargeList()
    if self.first_recharge_config == nil then
        return
    end
    self.reward_first_charge_list = {}
    -- 三天的礼包字段
    local str = {"reward_item", "sec_reward_item", "thr_reward_item"}
    for i, v in ipairs(str) do
        local config = self.first_recharge_config[1][v]
        if config then
            local gift = BagData.Instance:GetGiftItemList(config.item_id) or {}
            local tab = TableCopy(gift.item_list, 3)
            table.insert(self.reward_first_charge_list, tab)
        else
            LogError("首充表字段不对", v)
            return
        end
    end
end

function FirstChargeData:DealExclusiveList()
    self.peri_config = {}
    self.grow_config = {}
    for k, v in pairs(self.exclusive_config) do
        if v.activity_type == FirstChargeExclusivePanelId.Peri then
            self.peri_config[v.seq + 1] = v
        elseif v.activity_type == FirstChargeExclusivePanelId.Grow then
            self.grow_config[v.seq + 1] = v
        end
    end
end

function FirstChargeData:GetFirstChargeList()
	return self.reward_first_charge_list
end

function FirstChargeData:GetPeriConfig()
	return self.peri_config
end

function FirstChargeData:GetGrowConfig()
	return self.grow_config
end

function FirstChargeData:GetExclusiveActList()
    if not self.exclusive_act_list then
        self.exclusive_act_list = {}
        for k, v in pairs(Config.recharge_cfg_auto.activity_list) do
            local act_info = ActivityData.Instance:GetActivityInfo(v.activity_type)
            for l, w in pairs(act_info.push_info.open_days) do
                local numw = tonumber(w)
                if not self.exclusive_act_list[numw] then
                    self.exclusive_act_list[numw] = {}
                    self.exclusive_act_list[numw].act_type = v.activity_type
                end
            end
        end
    end
    return self.exclusive_act_list
end

function FirstChargeData:GetTomorrowWDay()
    local today = ActivityData.Instance:GetWeekNum()
    return (today + 1) > 7 and 1 or (today + 1)
end

-- 获取下一个表里有配的活动的星期几
function FirstChargeData:GetNextActWDay()
    local act_list = self:GetExclusiveActList()
    local today = ActivityData.Instance:GetWeekNum()
    local next_day = self:GetTomorrowWDay()
    for i = 1, 6 do
        if TableIsEmpty(act_list[next_day]) then
            next_day = (next_day + 1) > 7 and 1 or (next_day + 1)
        else
            return next_day
        end
    end
end

-- 获取仙子任务领取标志位
function FirstChargeData:GetExclusiveFlag(m_type)
    return self.exclusive_info.flag[m_type] or 0
end

-- 获取仙子任务进度
function FirstChargeData:GetPeriProgress()
    return self.exclusive_info.progress
end

-- 获取成长助力是否购买
function FirstChargeData:GetCmdBuyFlag()
    return self.first_cmd_buy_flag.flag[1] or 0
end

-- 获得首充专属面板是否有开启
function FirstChargeData:GetPanelStateList()
    local tab = {}
    tab.list = {}
    tab.open_first = 0
    for i, v in ipairs(FirstChargeExclusivePanelIndex) do
        local val = self:GetExclusivePanelIsOpen(i)
        if tab.open_first == 0 and val then
            tab.open_first = i
        end
        tab.list[i] = val
    end
    return tab
end

-- 判断首充专属是否开
function FirstChargeData:GetExclusiveIsOpen()
    -- 没有领取首充不开
    if not FunOpen.Instance:GetFunIsOpened(Mod.RandActivity.FirstCharge) or self.first_charge_data.reward_flag == 0 then
        return false
    end
    for k, v in pairs(FirstChargeExclusivePanelId) do
        if self:GetExclusivePanelIsOpen(v) then
            return true
        end
    end
    return false
end

-- 判断首充专属面板是否开启
function FirstChargeData:GetExclusivePanelIsOpen(index)
    if index == FirstChargeExclusivePanelId.Peri then
        for k, v in pairs(self.peri_config) do
            if self:GetExclusiveFlag(v.type) == 0 then
                return true
            end
        end
        return false
    elseif index == FirstChargeExclusivePanelId.Grow then
        -- 买了判断有无领取完
        if self:GetCmdBuyFlag() == 1 then
            for k, v in pairs(self.grow_config) do
                if self:GetExclusiveFlag(v.type) == 0 then
                    return true
                end
            end
            return false
        else
            return TimeCtrl.Instance:GetServerTime() < self.exclusive_info.end_timestamp
        end
    end
    return false
end

-- 首充专属红点
function FirstChargeData:GetExclusiveRemind()
    for k, v in pairs(FirstChargeExclusivePanelId) do
        if self:GetExclusivePanelRemind(v) == 1 then
            return 1
        end
    end
    return 0
end

-- 首充专属面板是否有红点
function FirstChargeData:GetExclusivePanelRemind(index)
    if not self:GetExclusivePanelIsOpen(index) then
        return 0
    end
    if index == FirstChargeExclusivePanelId.Peri then
        for k, v in pairs(self.peri_config) do
            if self:GetExclusiveFlag(v.type) == 0 and self:GetPeriProgress() >= v.parameter then
                return 1
            end
        end
        return 0
    elseif index == FirstChargeExclusivePanelId.Grow then
        if self:GetCmdBuyFlag() == 1 then
            for k, v in pairs(self.grow_config) do
                if self:GetExclusiveFlag(v.type) == 0 and RoleData.Instance:GetRoleLevel() >= v.parameter then
                    return 1
                end
            end
        end
        return 0
    end
    return 0
end
function FirstChargeData:GuildCreateConsumeCoinBind()
    return self.guild_create_consume_coin_bind
end