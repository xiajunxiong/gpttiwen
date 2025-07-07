MarryGiftView = MarryGiftView or DeclareView("MarryGiftView", "marry/marry_gift")

function MarryGiftView:MarryGiftView()
    self.data = MarryData.Instance
    self.care_handle = self.data.info_data:Care(BindTool.Bind(self.FlushItemView, self))
end

function MarryGiftView:CloseCallback()
	self.data.info_data:Uncare(self.care_handle)
	self.care_handle = nil
end

function MarryGiftView:LoadCallback(param_t)
    self:FlushItemView()
end

function MarryGiftView:FlushItemView()
    local info = self.data:GetSelfInfo()
    local mate_info = self.data:GetMateInfo()
    local config = Config.get_married_auto.marry_gift
    local gift_config = config[info.gift_type] or config[1]
    local self_time = info.gift_type_end_time or 0
    local has_self_time = (info.gift_type_end_time or 0) - TimeHelper:GetServerTime()
    if has_self_time > 0 then
        self.SelfTime:SetShowCallBack(function(t)return BattlefieldData.Instance:GetConditionTimeTip(t)end)
        self.SelfTime:StampTime(self_time,nil,Language.Marry.GiftSelfTime,Language.Marry.NotGiftSelfTime)
    else
        self.SelfTime:SetTime(Language.Marry.NotGiftSelfTime)
    end
    self.SelfTime:SetCallBack(function()
        for i=1,self.RewardInter:Length() do
            UH.SetInter(self.RewardInter[i],false)
        end
        self.Tips:SetActive(true)
        self.data.info_data.val = not self.data.info_data.val
    end)
    for i=1,self.RewardInter:Length() do
        UH.SetInter(self.RewardInter[i],has_self_time > 0 and info.fetch_gift_flag[i] == 0)
    end
    self.Grid:SetData(DataHelper.FormatItemList(gift_config.dayreward))
    self.Tips:SetActive(has_self_time <= 0)

    local mate_config = config[mate_info.gift_type] or config[1]
    local gift_time = mate_info.gift_type_end_time or 0
    local has_gift_time = (mate_info.gift_type_end_time or 0) - TimeHelper:GetServerTime()

    if has_gift_time > 0 then
        self.GiftTime:SetShowCallBack(function(t)return BattlefieldData.Instance:GetConditionTimeTip(t)end)
        self.GiftTime:StampTime(gift_time,nil,Language.Marry.GiftGiftTime,Language.Marry.NotGiftMateTime)
        UH.SetText(self.LimitTime,"")
    else
        self.GiftTime:SetTime(Language.Marry.NotGiftMateTime)
        UH.SetText(self.LimitTime,Format(Language.Marry.GiftLimitTime,mate_config.last_day))
    end
    UH.SpriteName(self.GiftImg,"QingYuanLiHe" .. mate_config.gift_id)
    self.GiftTime:SetCallBack(function()
        self.GiftTime:SetTime(Language.Marry.NotGiftMateTime)
        UH.SetText(self.LimitTime,Format(Language.Marry.GiftLimitTime,mate_config.last_day))
    end)
end

function MarryGiftView:OnClickClose()
    ViewMgr:CloseView(MarryGiftView)
end

function MarryGiftView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(239)
end

function MarryGiftView:OnClickReward(type)
    if type == 1 then
        MarryCtrl.Instance:SendMarryReq(MarryConfig.REQUEST_TYPE.GIFT_FETCH)
    else
        MarryCtrl.Instance:SendMarryReq(MarryConfig.REQUEST_TYPE.GIFT_FETCH_DAY)
    end
    UH.SetInter(self.RewardInter[type],false)
end

function MarryGiftView:OnClickPurchase()
    ViewMgr:OpenView(MarryGiftPopupView)
end

--===========================MarryGiftPopupView===========================
MarryGiftPopupView = MarryGiftPopupView or DeclareView("MarryGiftPopupView", "marry/marry_gift_popup")

function MarryGiftPopupView:MarryGiftPopupView()
    self.Board:SetData(self:GetType(), Language.Marry.TitleNameList[3], Vector2.New(890, 648),true)
end

function MarryGiftPopupView:LoadCallback(param_t)
    local mate_info = MarryData.Instance:GetMateInfo()
    local config = Config.get_married_auto.marry_gift
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetData(config[i])
    end
    if mate_info.gift_type ~= 0 then
        for i=1,self.ItemList:Length() do
            UH.SetToggle(self.Toggle[i],i==mate_info.gift_type)
        end
    end
    UH.SetInter(self.BtnUse,mate_info.gift_type == 0 or (mate_info.gift_type_end_time - TimeHelper:GetServerTime() <= 0))
end

function MarryGiftPopupView:OnClickItem(type)
    self.select_type = type
end

function MarryGiftPopupView:OnClickUse()
    local mate_info = MarryData.Instance:GetMateInfo()
    local has_gift_time = (mate_info.gift_type_end_time or 0) - TimeHelper:GetServerTime()
    if not self.select_type then
        PublicPopupCtrl.Instance:Error(Language.Marry.ErrorText[13])
        return
    end
    if has_gift_time > 0 then
        PublicPopupCtrl.Instance:Error(Language.Marry.ErrorText[22])
        return
    end
    -- if mate_info.gift_type ~= 0 and mate_info.gift_type ~= self.select_type then
    --     PublicPopupCtrl.Instance:Error(Language.Marry.ErrorText[15])
    --     return
    -- end
    local config = Config.get_married_auto.marry_gift[self.select_type]
    if MallCtrl.IsNotCurrency(CurrencyType.Gold, config.gift_price) then
        PublicPopupCtrl.Instance:AlertTip(Format(Language.Marry.ErrorText[14],config.gift_price,config.gift_name),function()
            MarryCtrl.Instance:SendMarryReq(MarryConfig.REQUEST_TYPE.GIFT,config.gift_id)
            PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[16])
            ViewMgr:CloseView(MarryGiftPopupView)
        end,CurrencyType.Gold)
    end
end

----------------------------MarryGiftPopupItem----------------------------
MarryGiftPopupItem = MarryGiftPopupItem or DeclareMono("MarryGiftPopupItem", UIWidgetBaseItem)

function MarryGiftPopupItem:SetData(data)
    UH.SetText(self.Name,data.gift_name)
    UH.SetText(self.Price,data.gift_price .. Language.Common.HuoBi[0])
    UH.SetText(self.LimitTime,data.last_day .. Language.Timer.Name.day)
    UH.SetText(self.GoldNum,data.bug_get .. Language.Common.HuoBi[16])
    self.Grid:SetData(DataHelper.FormatItemList(data.dayreward))
end