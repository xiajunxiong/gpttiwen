PeakTokenState = {
    None = 0,   --红包雨未开始
    Standy = 1, --红包雨准备阶段
	Open = 2,   --红包雨进行阶段
    Finish = 3, --红包雨结算阶段
}

PeakTokenData = PeakTokenData or BaseClass()

function PeakTokenData:__init()
    if PeakTokenData.Instance ~= nil then
        Debuger.LogError("[PeakTokenData] attempt to create singleton twice!")
        return
    end
    PeakTokenData.Instance = self
    self.game_data = SmartData.New({state = 0,num = 0,item = {}})
    self.info_data = SmartData.New({
        today_use_times = 0,
        info_list = {},
        reward_times = 0
    })
    self.reward_list = {}
    self.role_info_data = {}
end

-- 天下第一比武大会 天下至尊令下发
function PeakTokenData:SetPeaktokenRoleStatusInfo(protocol)
    self.info_data.info_list[protocol.id] = protocol.info
end

function PeakTokenData:SetPeaktokenRoleJoinInfo(protocol)
    self.info_data.info_list[protocol.id] = protocol.info
    self:AddRewardList(protocol.item)
end

function PeakTokenData:SetPeaktokenRoleInfo(protocol)
    self.info_data.today_use_times = protocol.today_use_times
end

function PeakTokenData:SetRoleInfoTimes(times)
    self.info_data.reward_times = times
end

function PeakTokenData:GetRoleInfoTimes()
    return self.info_data.reward_times or 0
end

function PeakTokenData:GetTodayUseTimes()
    return self.info_data.today_use_times
end

function PeakTokenData:SetRainState(state)
    self.game_data.state = state or 0
end

function PeakTokenData:GetRainState()
    return self.game_data.state
end

function PeakTokenData:AddRewardList(data)
    table.insert(self.reward_list,data)
    self.game_data.item = data
    self.game_data.num = #self.reward_list
end

function PeakTokenData:GetRewardList()
    return DataHelper.FormatItemList(self.reward_list)
end

function PeakTokenData:SetRoleInfoData(data)
    self.role_info_data = data
end

function PeakTokenData:GetRoleInfoData()
    return self.role_info_data
end

function PeakTokenData:GetItemData()
    return self.game_data.item
end

function PeakTokenData:GetItemNum()
    return self.game_data.num
end

function PeakTokenData:GetInfoDataById(id)
    return self.info_data.info_list[id]
end

function PeakTokenData:GetTalkText(end_time)
    local t = TimeHelper.FormatDHMS(end_time - TimeHelper:GetServerTime())
    local ConditionTimeTip = BattlefieldData.Instance:GetConditionTimeTip(t)
    local time_desc = Format(Language.PeakToken.CountDownTime,ConditionTimeTip)
    return Language.PeakToken.NpcDialogTip .. "\n" .. time_desc
end

function PeakTokenData:ClearData()
    self.reward_list = {}
    self.role_info_data = {}
    self.game_data.state = 0
    self.game_data.num = 0
    self.game_data.item = {}
    self.info_data.reward_times = 0
end