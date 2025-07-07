ActiveWeekRewardView = ActiveWeekRewardView or DeclareMono("ActiveWeekRewardView", UIWFlushPanel)

function ActiveWeekRewardView:ActiveWeekRewardView()
    self.data = ActiveData.Instance
    self.data_cares = {
        {data = self.data.weed_reward_data,func = self.OnFlush},
        {data = ActivityData.Instance:GetActivityEvent(),func = self.FlushListView,keys = {"act_type", "status"},init_call = false},
        {data = RoleData.Instance:GetBaseData(), func = self.FlushRewardView, keys = {"level"}, init_call = false}
    }
end

function ActiveWeekRewardView:OnFlush()
    self:FlushRewardView()
    self:FlushListView()
end

function ActiveWeekRewardView:FlushListView()
    self.FastList:SetData(self.data:GetWeekRewardFastData())
    self.LongList:SetData(self.data:GetWeekRewardLongData())
end

function ActiveWeekRewardView:FlushRewardView()
    local active_num = self.data:GetWeekRewardIntegralNum()
    local reward_list,list = self.data:GetWeekRewardData()
    self.Progress:SetProgress(self:GetProgressNum(active_num,list))
    self.Progress:SetText(active_num)
    for i=1,self.RewardList:Length() do
        self.RewardList[i]:SetData(reward_list[i])
    end
end

function ActiveWeekRewardView:GetProgressNum(num,list)
    local amount_num = (DataHelper.GetFillAmount(num,list) - 0.045)
    return amount_num < 0 and 0 or amount_num
end

----------------------------ActiveWeekRewardItem----------------------------
ActiveWeekRewardItem = ActiveWeekRewardItem or DeclareMono("ActiveWeekRewardItem", UIWidgetBaseItem)

function ActiveWeekRewardItem:ActiveWeekRewardItem()
    self.Language = Language.Active.WeekReward
end

function ActiveWeekRewardItem:SetData(data)
    local act_type = data.act_type
    local config = ActivityData.GetConfig(act_type)
    if config == nil then
        LogError("活动配置不存在：",act_type)
    end
    self:FlushItemView(config,data)
    if ActiveData.Instance:GetWeekRewardFinishFlag(data.type) == 0 and not ActivityData.Instance:IsNoEnoughLevel(data.act_type,true) then
        if data.show_type == 0 or ActivityData.IsOpen(act_type) then
            self.BtnGame:SetActive(true)
        end
    else
        self.BtnGame:SetActive(false)
    end
    self.Down:SetActive(not GetActive(self.BtnGame))
    UIWidgetBaseItem.SetData(self,data)
end

function ActiveWeekRewardItem:FlushItemView(config,data)
    UH.SetText(self.Name,config.name_text)
    UH.SetIcon(self.Icon,config.icon_id,ICON_TYPE.ACT)
    UH.SpriteName(self.Label,ActivityConfig.LabelName[config.title])
    UH.SetText(self.LabelName,Language.Activity.LabelName[config.title])
    local Integral = self.Language.Integral[self:GetIndex(data.show_type == 0)]
    UH.SetText(self.Integral,Format(Integral,data.integral_active))
    UH.SetText(self.Desc,self:GetDesc(config,data))
end

function ActiveWeekRewardItem:GetDesc(config,data)
    if GuildData.Instance:GetGuildID() == 0 and ActiveConfig.GuildConfig[data.act_type] == true then
        return self.Language.GuildTip
    elseif ActivityData.Instance:IsNoEnoughLevel(data.act_type,true) then
        return self:GetNext(2,ActivityData.Instance:IsNoEnoughLevel(data.act_type,true))
    elseif ActiveData.Instance:GetWeekRewardFinishFlag(data.type) == 1 then
        return self.Language.FinishTip[self:GetIndex(data.show_type == 2)]
    elseif ActivityData.Instance:GetActivityInfo(data.act_type):IsToday() then
        local curr_time = self:Time(os.date("%H:%M", TimeManager:GetServerTime()))
        if config.begin_time == "00:00" then
            return config.time_text
        elseif curr_time >= self:Time(config.end_time) then
            return self.Language.EndTip[self:GetIndex(data.show_type == 2)]
        elseif curr_time < self:Time(config.begin_time) then
            return Format(self.Language.OpenTip,config.begin_time)
        end
    else
        return config.time_text
    end
end

function ActiveWeekRewardItem:Time(time)
    local split = Split(time,":")
    return tonumber(split[1]) * 100 + tonumber(split[2])
end

function ActiveWeekRewardItem:GetIndex(condition)
    return condition and 1 or 2
end

function ActiveWeekRewardItem:GetNext(index,...)
    local list = {...}
    return list[index]
end

function ActiveWeekRewardItem:OnClickGo()
    local info = ActivityData.Instance:GetActivityInfo(self.data.act_type)
    if info.click_handle then
        if info.click_handle() then
            return
        end
    end
    ViewMgr:CloseView(ActiveView)
end

function ActiveWeekRewardItem:OnClickItem()
    ItemInfoCtrl.Instance:ActivityInfoView{info = ActivityData.Instance:GetActivityInfo(self.data.act_type)}
end

----------------------------ActiveWeekRewardCall----------------------------
ActiveWeekRewardCall = ActiveWeekRewardCall or DeclareMono("ActiveWeekRewardCall", UIWidgetBaseItem)

function ActiveWeekRewardCall:SetData(data)
	data.item = self:GetItemData(data)
	self.Call:SetData(data.item)
	UH.SetText(self.Num,data.integral_active)
	
	local not_reward = ActiveData.Instance:GetWeekRewardFlag(data.seq) == 0
    local can_reward = ActiveData.Instance:GetWeekRewardIntegralNum() >= data.integral_active

    self.Mark:SetActive(not not_reward)
	self.Effect:SetActive(not_reward and can_reward)
	UIWidgetBaseItem.SetData(self, data)	
end

function ActiveWeekRewardCall:OnClickItem()
    CellClicks.BagGridNoButton({GetData = function()
        return self.data.item
    end})
end

function ActiveWeekRewardCall:OnClickReward()
	ActivityCtrl.Instance:SendActiveSystemReq(2,self.data.seq)
end

function ActiveWeekRewardCall:GetItemData(data)
	if data == nil then return Item.New() end
	for k,v in pairs(data.reward_id or {}) do
		return Item.New(v)
	end
    for k,v in pairs(ActiveConfig.RewardKey) do
        if data[v.key] and data[v.key] > 0 then
            return Item.Init(v.item_id,data[v.key])
        end
    end
    if data.coin and data.coin ~= 0 then
        table.insert(list,{item_id = CommonItemId.Coin,num = data.coin})
    end
end