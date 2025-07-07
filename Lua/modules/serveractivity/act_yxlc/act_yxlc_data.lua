-- @Author: Ghost
-- @Date: 2022-08-26 14:18:08
-- @Last Modified by:   Ghost
-- @Last Modified time: 2022-08-26 14:18:08
ActYxlcData = ActYxlcData or BaseClass()
function ActYxlcData:__init()
    if ActYxlcData.Instance ~= nil then
        Debuger.LogError('[ActYxlcData] attempt to create singleton twice!')
        return
    end
    ActYxlcData.Instance = self
    -- self.ActYxlc_data = SmartData.New()
    self.Config_Reward = Config.yuexileichong_auto.gift_configure
    self.SC_YXLCInfo = {data = nil}
    self.SD_YXLCInfo = SmartData.New({m_total_chongzhi = 0, val = false, count_Remind = 0}) -- m_total_chongzhi 单位元宝
end

function ActYxlcData:__delete()
    ActYxlcData.Instance = nil
end

function ActYxlcData:ClearData()
    self.SD_YXLCInfo:Set({m_total_chongzhi = 0, val = false, count_Remind = 0})
    self.SC_YXLCInfo.data = nil
end

-- 初始化当前开服天数的活动配置
function ActYxlcData:InitSC_YXLCInfo_Data()
    local Table_P_Data = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_YUE_XI_LEI_CHONG, self.Config_Reward, nil) or {}
    local data_info = {}
    for k, v in pairs(Table_P_Data) do
        local data = {}
        local reward_gold
        data.m_receive_reward_flag = 0
        data.config = v
        data.canRec = false
        if v.gold ~= 0 then
            reward_gold = Item.InitData(CommonItemId.Gold, v.gold, 1)
            local new_reward = {}
            new_reward[0] = reward_gold
            for k, v in pairs(v.reward_item) do
                table.insert(new_reward, v)
            end
            data.reward = new_reward
        else
            data.reward = v.reward_item
        end
        table.insert(data_info, data)
    end
    self.SC_YXLCInfo.data = data_info
end

-- 获取月夕累充活动配置
function ActYxlcData:GetSC_YXLCInfo_Data()
    if self.SC_YXLCInfo.data == nil then
        self:InitSC_YXLCInfo_Data()
    end
    return self.SC_YXLCInfo.data
end

-- 月夕累充数据处理
function ActYxlcData:SC_F_YXLCInfo(protocol)
    self.SD_YXLCInfo.m_total_chongzhi = protocol.m_total_chongzhi / 10 -- 单位元宝 
    local YXLCData = self:GetSC_YXLCInfo_Data()
    local val = self.SD_YXLCInfo.val
    local count_Remind = 0
    for k, v in pairs(YXLCData) do
        local info_item = v
        local config = info_item.config
        local info_flag = info_item.m_receive_reward_flag
        local pro_flag = protocol.m_receive_reward_flag[config.seq] -- 服务度下发领取标识
        local canRec = ActYxlcCtrl.Instance:CheckCanRec(info_item.config.acc_recharge_down / 10)
        if canRec and info_flag == 0 then
            if val == self.SD_YXLCInfo.val then
                val = not self.SD_YXLCInfo.val
            end
            if pro_flag ~= 1 then
                count_Remind = count_Remind + 1
            end
            YXLCData[k].canRec = true
        else
            YXLCData[k].canRec = false
        end
        YXLCData[k].m_receive_reward_flag = protocol.m_receive_reward_flag[config.seq]
    end
    table.sort(YXLCData, function(a, b)
        if a.m_receive_reward_flag == b.m_receive_reward_flag then
            return a.config.acc_recharge_down < b.config.acc_recharge_down
        end
        return a.m_receive_reward_flag < b.m_receive_reward_flag
    end)
    self.SD_YXLCInfo.count_Remind = count_Remind
    self.SD_YXLCInfo.val = not self.SD_YXLCInfo.val
end
