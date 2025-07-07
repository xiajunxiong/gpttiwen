ActZyscData = ActZyscData or BaseClass()

-- 追月商城3000协议操作请求类型 
ZyscReqType = {
    Info = 0, -- 请求信息下发
    Buy_3 = 1, -- 直购  p1:奖励reward_seq
    Buy_0 = 2, -- 非直购  p1:配置seq
    ChooseReward = 3, -- 设置自选 p1:格子index[0,6]，p2:reward_seq
    Draw = 4, -- 抽奖 p1：次数
}

function ActZyscData:__init()
    if ActZyscData.Instance ~= nil then
        Debuger.LogError('[ActZyscData] attempt to create singleton twice!')
        return
    end
    ActZyscData.Instance = self
    self.Config = Config.zhuiyueshangcheng_auto
    -- 追月商城数据
    self.SC_ZyscInfo = SmartData.New({data = nil, val = false})
    -- 仲秋追月数据
    self.SC_ZqzyInfo = {
        data = nil, data_Reward = nil, -- 选择奖励列表
        data_Reward_Big = nil, -- 大奖
        data_Reward_Sl_Seq = {}, -- 6个自选奖励 里包含config数据
    }

    self.SD_ZqzyInfo = SmartData.New({
        data_Reward_Sl_length = 0, -- 自选奖励选中个数
        curr_num = 0, -- 抽奖消耗品个数
        val_MsIndex = 1, -- 展示模型选择
        val_sel, -- 是否已选择奖励
    })

    self.SC_ZyscDrawRet = {} -- 抽奖回调数据
    self.data_Reward_Sl_Seq_Save = nil -- 暂存选中的奖励
    self.SC_ZyscCjjlInfo = SmartData.New({})
    self.data_ModuleShow = nil
end

function ActZyscData:__delete()
    ActZyscData.Instance = nil
end

function ActZyscData:ClearData()
    self.SC_ZyscInfo:Set({data = nil, val = false})

    self.SC_ZqzyInfo = {
        data = nil, data_Reward = nil, -- 选择奖励列表
        data_Reward_Big = nil, -- 大奖
        data_Reward_Sl_Seq = {}, -- 选中数据 里包含config数据
    }

    self.SD_ZqzyInfo:Set({data_Reward_Sl_length = 0, curr_num = 0, val_MsIndex = 1, val_sel})
    self.isInit = nil
    self.SC_ZyscDrawRet = {}
end

-- 初始化追月商城配置数据 追月商城页面打开会执行
function ActZyscData:InitData()
    if self.isInit == nil then
        self:GetSC_ZqzyInfo_Data()
        self:GetSC_ZqzyReward_Data()
        self:GetSC_ZqzyRewardBig_Data()
        self:InitMsData()
        self.isInit = true
    end
end

-- 初始化追月商城展示模型数据
function ActZyscData:InitMsData()
    local Config = self.Config.show_item
    local Tabel_P_Data = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG, Config, nil) or {}
    self.data_ModuleShow = DataHelper.FormatItemList(Tabel_P_Data[1].show_reward_item)
end

-- 初始化追月商城当前开服天数的活动配置 {data:{config:表,Config_SC_Reward:?奖励}}
function ActZyscData:InitSCInfoData()
    local config = self.Config.shop_configuration
    local Table_P_Data = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG, config, nil) or {}
    local data_info = {}
    for k, v in pairs(Table_P_Data) do
        local data = {}
        data.config = v
        data.Config_SC_Reward = self:Init_C_Reward(v)
        table.insert(data_info, data)
    end
    return data_info
end

-- 获取追月商城活动数据
function ActZyscData:GetSC_ZyscInfo_Data()
    self.SC_ZyscInfo.data = self:InitSCInfoData()
    return self.SC_ZyscInfo.data
end

-- 解析追月商城奖励列表
function ActZyscData:Init_C_Reward(config)
    local reward = config.reward_item
    -- 商城奖励展示 - 从后往前遍历 先设置下层列表 再设置上层列表
    local Lists = {nil, {}}
    for i = #reward, 0, -1 do
        local itme_reward = reward[i]
        if #Lists[2] >= 2 then
            if Lists[1] == nil then
                Lists[1] = {}
            end
            table.insert(Lists[1], itme_reward)
        else
            table.insert(Lists[2], itme_reward)
        end
    end
    return Lists
end

-- 追月商城信息下发
function ActZyscData:On_SC_ZyscInfo(protocol)
    local data_ZyscInfo = self:GetSC_ZyscInfo_Data()
    for k, v in pairs(data_ZyscInfo) do
        local seq = v.config.seq
        if v.config.limit_type == 1 then
            -- 活动限购
            v.buy_times = v.config.limit_times - protocol.ra_buy_times[seq]
        else
            -- 每天限购
            v.buy_times = v.config.day_times - protocol.ra_day_buy_times[seq]
        end
    end
    table.sort(data_ZyscInfo, function(a, b)
        if a.buy_times == 0 and b.buy_times ~= 0 then
            return false
        elseif a.buy_times ~= 0 and b.buy_times == 0 then
            return true
        end
        return a.config.seq < b.config.seq
    end)
    self.SC_ZqzyInfo.data_Reward_Sl_Seq = protocol.item_list_select
    self.SC_ZyscInfo.val = not self.SC_ZyscInfo.val
    self.SD_ZqzyInfo.val_sel = protocol.val_sel
end

-- 抽奖回调
function ActZyscData:OnSc_ZyscDrawRet(protocol)
    self.SC_ZyscDrawRet.ItemList = {}
    local time = TimeManager:GetServerTime()
    for i = 1, protocol.count, 1 do
        local reward_index = protocol.item_list[i]
        local reward_data
        if reward_index == -1 then
            reward_data = self.SC_ZqzyInfo.data_Reward_Big.reward_item
        else
            reward_data = self.SC_ZqzyInfo.data_Reward_Sl_Seq[reward_index].config.reward_item
        end
        local reward_item = Item.Create(reward_data)
        reward_item.IsGem = false
        table.insert(self.SC_ZyscDrawRet.ItemList, {item = reward_item, reward_index = reward_index})
        -- ChatCtrl.Instance:ChannelChatCustom{item_get = {item = reward_item, num = num}}

        local cjjl_data = {}
        cjjl_data.time = time
        cjjl_data.reward_index = reward_index
        cjjl_data.sort_param = #self.SC_ZyscCjjlInfo + 1
        table.insert(self.SC_ZyscCjjlInfo, cjjl_data)
    end
    ViewMgr:OpenView(ZyscRewardView, self.SC_ZyscDrawRet.ItemList)
end

-- 抽奖信息下发
function ActZyscData:Sc_ZyscCjjlInfo(protocol)
    self.SC_ZyscCjjlInfo = protocol.record_list
end

-- 抽奖信息处理
function ActZyscData:ZyscCjjlDel()
    local record_list = self.SC_ZyscCjjlInfo
    table.sort(record_list, function(a, b)
        return a.sort_param > b.sort_param
    end)
    local cfg = self.Config
    local list = {}
    for k, v in pairs(record_list) do
        local data_item
        if v.reward_index == -1 then
            data_item = Item.Create(self.SC_ZqzyInfo.data_Reward_Big.reward_item)
        else
            data_item = Item.Create(self.SC_ZqzyInfo.data_Reward_Sl_Seq[v.reward_index].config.reward_item)
        end
        local time = v.time
        local text = os.date(Language.ActZysc.cjjl_time, time) .. "          " .. Language.ActZysc.cjjl_gxhd
        table.insert(list, {text = text, reward_item = data_item})
    end
    return list
end

---抽奖道具数量改变
function ActZyscData:onFlushCurr()
    local config = self:GetSC_ZqzyInfo_Data()
    local need_ItemId = config.consume_item_id
    self.SD_ZqzyInfo.curr_num = Item.GetNum(need_ItemId)
end

-- 获取仲秋追月活动数据-抽奖配置
function ActZyscData:GetSC_ZqzyInfo_Data()
    if self.SC_ZqzyInfo.data == nil then
        local config = self.Config.draw_configuration
        local Table_P_Data = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG, config, nil) or {}
        self.SC_ZqzyInfo.data = Table_P_Data[1]
    end
    return self.SC_ZqzyInfo.data
end

-- 获取仲秋追月活动可选奖励数据
function ActZyscData:GetSC_ZqzyReward_Data()
    if self.SC_ZqzyInfo.data_Reward == nil then
        local data_zqzy = self:GetSC_ZqzyInfo_Data()
        self.SC_ZqzyInfo.data_Reward = self:Get_ZqzyRewardGp(data_zqzy.reward_group)
    end
    return self.SC_ZqzyInfo.data_Reward
end

-- 获取仲秋追月大奖数据
function ActZyscData:GetSC_ZqzyRewardBig_Data()
    if self.SC_ZqzyInfo.data_Reward_Big == nil then
        local data_zqzy = self:GetSC_ZqzyInfo_Data()
        self.SC_ZqzyInfo.data_Reward_Big = self:Get_ZqzyRewardGp(data_zqzy.big_reward_group)[1]
    end
    return self.SC_ZqzyInfo.data_Reward_Big
end

function ActZyscData:Get_ZqzyRewardGp(rew_gp)
    local config = self.Config.reward_group
    local list_reward = {}
    for k, v in pairs(config) do
        if v.reward_group == rew_gp then
            table.insert(list_reward, v)
        end
    end
    return list_reward
end

-- 红点 1：开活动 2：可抽奖
function ActZyscData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG) then
        return 0
    end
    -- 活动红点
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG)
    if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG)) then
        -- 活动首次红点，换设备重新提示
        return 1
    end
    return self:DrawRemind()

end

-- 抽奖红点
function ActZyscData:DrawRemind()
    local config = self:GetSC_ZqzyInfo_Data()
    local num_have = Item.GetNum(config.consume_item_id)
    local num_need = config.consume_num
    return num_have >= num_need and 1 or 0
end

-- 清除开活动查看红点
function ActZyscData:ClearFirstRemind()
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG)
    if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG)) then
        PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_ZHUI_YUE_SHANG_CHENG), act_start)
        self.SD_ZqzyInfo:Notify()
    end
end
