ActiveDailyAllFinishView = ActiveDailyAllFinishView or DeclareView("ActiveDailyAllFinishView", "active/active_daily_all_finish")
VIEW_DECLARE_MASK(ActiveDailyAllFinishView, ViewMask.BgBlock)
function ActiveDailyAllFinishView:ActiveDailyAllFinishView()
    self.Board:SetData(self:GetType(), Language.ActiveDailyAllFinish.Title, Vector2.New(843, 631))
end

function ActiveDailyAllFinishView:LoadCallback(param_t)
    
end

function ActiveDailyAllFinishView:CloseCallback()

end

function ActiveDailyAllFinishView:OnCloseClick()
    ViewMgr:CloseView(ActiveDailyAllFinishView)
end

----------------------------ActiveDailyAllFinishPanel----------------------------
ActiveDailyAllFinishPanel = ActiveDailyAllFinishPanel or DeclareMono("ActiveDailyAllFinishPanel", UIWFlushPanel)
function ActiveDailyAllFinishPanel:ActiveDailyAllFinishPanel()
    self.Data = ActiveData.Instance
    self.language = Language.ActiveDailyAllFinish
    self.data = self.Data.info_data
    self.data_cares = {
        {data = ActivityData.Instance.active_data, func = self.FlushAll, init_call = false},
    }
end

function ActiveDailyAllFinishPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.is_buy = RewardData.Instance:IsBuyWeekAndMonth()
    self.ObjNoCardShow:SetActive(not self.is_buy)
    self.cost = 0
    self:FlushAll()
end

function ActiveDailyAllFinishPanel:FlushAll()
    self.list_data = self.Data:GetCanAllFinishList()
    if TableIsEmpty(self.list_data) then
        ViewMgr:CloseView(ActiveDailyAllFinishView)
        return
    end
    self.List:SetData(self.list_data)
    self:FlushNeedConsume()
end

function ActiveDailyAllFinishPanel:OnAllClick(value)
    for k, v in pairs(self.List.item_list) do
        v:SetToggle(value)
    end
end

function ActiveDailyAllFinishPanel:OnSelect(value, mono)
    self:FlushNeedConsume()
end

function ActiveDailyAllFinishPanel:FlushNeedConsume()
    self.cost = 0
    local select_data = self.List:GetSelectedData()
    if not self.is_buy then
        for k, v in pairs(select_data) do
            self.cost = self.cost + self.Data:GetAllFinishCostGold(v.act_type)
        end
    end
    UH.SetText(self.TxtNum, self.cost)
end

function ActiveDailyAllFinishPanel:OnAllFinishClick()
    if MainViewData.Instance:IsBattle(true) then
        return
    end
    local select_data = self.List:GetSelectedData()
    if TableIsEmpty(select_data) then
        PublicPopupCtrl.Instance:Center(self.language.Tip)
        return
    end
    local select_num = #select_data
    if MallCtrl.IsNotCurrency(CurrencyType.Gold, self.cost) then
        local act_bit = bit:d2b(0)
        for i, v in pairs(select_data) do
            local seq = self.Data:GetAllFinishSeq(v.act_type)
            if seq then
                act_bit[32 - seq] = 1
            end
        end
        local param = bit:b2d(act_bit)
        if self.cost > 0 then
            PublicPopupCtrl.Instance:AlertTip(Format(self.language.CheckTip, self.cost, select_num), function()
                ActiveCtrl.Instance:SendSignUpReq(2, param)
            end)
        else
            ActiveCtrl.Instance:SendSignUpReq(2, param)
        end
    end
end

function ActiveDailyAllFinishPanel:GoToMonthCard()
    ViewMgr:OpenViewByKey(Mod.Reward.MonthCard)
    ViewMgr:CloseView(ActiveView)
    ViewMgr:CloseView(ActiveDailyAllFinishView)
end

function ActiveDailyAllFinishPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

----------------------------ActiveDailyAllFinishItem----------------------------
ActiveDailyAllFinishItem = ActiveDailyAllFinishItem or DeclareMono("ActiveDailyAllFinishItem", UIWidgetBaseItem)

function ActiveDailyAllFinishItem:SetData(data)
    local act_type = data.act_type
    UH.SetText(self.Name,data.desc)
    UH.RenameGame(self,Format("Item_%s",data.index))
    self.act_info = ActivityData.Instance:GetActivityInfo(act_type)
    -- self.Grid:SetData(ActiveData.Instance:GetItemReardData(data))
    self.Grid:SetData(self.act_info.reward_list or {})
    local is_open = self:FlushItemView(ActivityData.GetConfig(act_type), data) 
    -- 特殊类型需要使用特殊类型的进度检查和领取情况检查
    if data.is_special == 1 then 
        UH.SetText(self.Times,ActiveData.Instance:GetDailyCondition(act_type))
        local reward_flag = ActivityData.Instance:GetSpecialIsGetReward(data.type)
        -- self.BtnEffect:SetActive(self.act_info:IsComplete() and not reward_flag)
        
        -- self.Complete:SetActive(is_open and (self.act_info and self.act_info:IsComplete()) and reward_flag)
        -- self.Mark:SetActive(is_open and (self.act_info and self.act_info:IsComplete()) and reward_flag)
    elseif data.need_count == 0 then
        UH.SetText(self.Times,ActiveData.Instance:GetDailyCondition(act_type))
        -- self.Complete:SetActive(is_open and (self.act_info and self.act_info:IsComplete()))
        -- self.Mark:SetActive(is_open and (self.act_info and self.act_info:IsComplete()))
        -- self.BtnEffect:SetActive(false)
    else
        local FlagType = ActiveConfig.FlagType
        UH.SetText(self.Times,self:GetProgressTimes(data))
        local flag = ActivityData.Instance:GetActiveFlag(data.type)
        --LogError("type==",data.type,"flag===",flag,data)
        -- self.BtnEffect:SetActive(is_open and flag == FlagType.CanReward)
        -- self.Complete:SetActive(is_open and flag == FlagType.Rewarded and data.need_goto == 0)
        -- self.Mark:SetActive(is_open and flag == FlagType.Rewarded and data.need_goto == 0)
    end
    -- self.BtnInter.Interactable = is_open

    self.Time:CloseCountDownTime()
    self.BaseItem:SetActive(data.act_type == ActType.Hint)
    if data.act_type == ActType.Hint then
        self.Time:StampTime(AnecdoteHintData.Instance:EndTimeStamp(),
        TimeType.Type_Special_4,Language.Anecdote.Hint.TimeCounts)
    end
    UIWidgetBaseItem.SetData(self,data)
    self:SetToggle(false)
end

function ActiveDailyAllFinishItem:FlushItemView(config, data)
    UH.SetIcon(self.Icon, config.icon_id,ICON_TYPE.ACT)
    UH.SpriteName(self.Label, ActivityConfig.LabelName[config.title])
    UH.SetText(self.LabelName, Language.Activity.LabelName[config.title])
    self.LabelList:SetData(ActiveData.Instance:GetLabelList(config.act_type))
    local is_enough, limit_level = ActivityData.Instance:IsNoEnoughLevel(config.act_type, true)
    local is_open_btn, open_btn_name = self.act_info:CustomButtonNameFunc()
    local is_open_day = RoleData.Instance:CreatedDay() >= data.open_day
    -- UH.SetText(self.BtnName,is_enough and limit_level or Language.Active.BtnName)
    if not is_enough and not is_open_day then
        -- UH.SetText(self.BtnName,Format(Language.Activity.OpenDayTime[1],DataHelper.GetDaXie(data.open_day)))
        if data.open_day - RoleData.Instance:CreatedDay() == 1 then
            -- UH.SetText(self.BtnName,Language.Activity.OpenDayTime[2])
        end
    end
    if not is_enough and open_btn_name ~= nil then
        -- UH.SetText(self.BtnName,open_btn_name)
        return not is_enough and is_open_btn
    end
    return not is_enough and is_open_day
end

function ActiveDailyAllFinishItem:GetProgressTimes(data)
    local OtherTimes = Language.Active.OtherTimes[data.act_type]
    local num = ActivityData.Instance:GetProgressNum(data.type)
    return Format(OtherTimes or Language.Active.Progress,num,ActiveData.Instance:GetDailyNeedCount(data))
end

function ActiveDailyAllFinishItem:GetActType()
    return self.data.act_type
end

function ActiveDailyAllFinishItem:SetToggle(value)
    self.Selector.isOn = value
end