VIEW_REQ("modules/serveractivity/may_day/articulately_view")

ArticulatelyCtrl = ArticulatelyCtrl or BaseClass(BaseCtrl)

function ArticulatelyCtrl:__init()
	if ArticulatelyCtrl.Instance ~= nil then
		Debuger.LogError("[ArticulatelyCtrl] attempt to create singleton twice!")
		return
	end
	ArticulatelyCtrl.Instance = self
	self.data = ArticulatelyData.New()
    self:RegisterAllProtocols()
end

function ArticulatelyCtrl:__delete()
    ArticulatelyCtrl.Instance = nil
end

function ArticulatelyCtrl:OnUnloadGameLogic()
    ArticulatelyData.Instance.times = 0
end

function ArticulatelyCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCArticulatelyInfo, "OnArticulatelyInfo")
    PeriodActivityCtrl.Instance:RegisterRemind(Mod.MayDay.Articulately,BagData.Instance.item_grid,function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ARTICULATELY) then
            return 0
        end
        if PeriodActivityData.Instance:GetRemindData(Mod.MayDay.Articulately) then
            return 0
        end
        return self.data:GetRemindNum()
    end)
end

function ArticulatelyCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCArticulatelyInfo)
end

function ArticulatelyCtrl:OnArticulatelyInfo(protocol)
    self.data:SetArticulatelyInfo(protocol)
end

--===========================ArticulatelyData===========================
ArticulatelyData = ArticulatelyData or BaseClass()

ArticulatelyData.ArticulatelyState = {
    None = 0,   --未开始
	Open = 1,   --进行阶段
}
ArticulatelyData.ARTICULATELY_MAX_TIMES = 4
ArticulatelyData.ARTICULATELY_MAX_STEP_NUM = 3

function ArticulatelyData:__init()
    if ArticulatelyData.Instance ~= nil then
        Debuger.LogError("[ArticulatelyData] attempt to create singleton twice!")
        return
    end
    ArticulatelyData.Instance = self
    self.game_data = SmartData.New({state = 0,val = false,item_list = {}})
    self.sort_list = {}
    self.times = 0
end

function ArticulatelyData:SetRainState(state)
    self.game_data.state = state or 0
end

function ArticulatelyData:GetRainState()
    return self.game_data.state
end

function ArticulatelyData:GetRewardList()
    local config = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ARTICULATELY,Config.miaobishenghua_auto.reward)
    return DataHelper.FormatItemList(config[1].reward_item or {})
end

function ArticulatelyData:AddItem(data)
    if self.sort_list[data.index] == nil then
        table.insert(self.game_data.item_list,data)
        self.sort_list[data.index] = data
        ArticulatelyData.Instance:CheckGameData()
    end
end

function ArticulatelyData:GetItemList()
    return self.game_data.item_list
end

function ArticulatelyData:IsHasSort()
    local num = 0
    local list = self.game_data.item_list
    for i=1,#list do
        if list[i].index == i then
            num = num + 1
        end
    end
    return num == #list
end

function ArticulatelyData:CheckGameData()
    self.game_data.val = not self.game_data.val
end

function ArticulatelyData:ChearData()
    self.game_data.item_list = {}
    self.sort_list = {}
end

function ArticulatelyData:GetRemindNum()
    return 1--Item.GetNum(Config.miaobishenghua_auto.other[1].item_id) > 0 and 1 or 0
end

function ArticulatelyData:SetArticulatelyInfo(protocol)
    self.times = protocol.count or 0
    MayDayData.Instance:MainBtnState(ACTIVITY_TYPE.RAND_ARTICULATELY, self:IsNextDay() and MayDayConfig.MainBtnState.NextDay or MayDayConfig.MainBtnState.Go)
end

function ArticulatelyData:IsNextDay()
    return self.times >= Config.miaobishenghua_auto.other[1].frequency_day and self:GetRemindNum() == 0
end