FourSeasonCardView = FourSeasonCardView or DeclareView("FourSeasonCardView", "serveractivity/four_season/four_season_card", Mod.PlanActivity.FourSeasonCard)
VIEW_DECLARE_LEVEL(FourSeasonCardView, ViewLevel.L1)
VIEW_DECLARE_MASK(FourSeasonCardView, ViewMask.BgBlock)
function FourSeasonCardView:FourSeasonCardView()
    self.Time:SetShowCallBack(function(t)
        return Format(Language.FourSeasonCard.Time, t.day, t.hour ,t.min)
    end)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_FOUR_SEASON_CARD, TimeType.Type_Time_3)
end

function FourSeasonCardView:OnCloseClick()
    ViewMgr:CloseView(FourSeasonCardView)
end

----------------------------FourSeasonCardPanel----------------------------
FourSeasonCardPanel = FourSeasonCardPanel or DeclareMono("FourSeasonCardPanel", UIWFlushPanel)
function FourSeasonCardPanel:FourSeasonCardPanel()
    self.Data = FourSeasonData.Instance
    self.language = Language.FourSeasonCard
    self.data = self.Data.card_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function FourSeasonCardPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.now_page = 1
    self.card_style_num = self.Data:CardStyleNum()
    self.base_data, self.max_page = self.Data:CardBaseData()
    self.ObjChange:SetActive(self.max_page > 1)
    self:InitStyle()
    self:FlushAll()
end

function FourSeasonCardPanel:InitStyle()
    local style = self.Data:CardStyle()
    local name = ActivityRandData.GetName(ACTIVITY_TYPE.RAND_FOUR_SEASON_CARD)
    for i = 1, self.card_style_num do
        self.ObjBgs[i]:SetActive(i == style)
        self.ObjTitles[i]:SetActive(i == style)
        UH.SetText(self.TxtNames[i], name)
        self.TxtDescs[i]:SetObjActive(i == style)
    end
    UH.SpriteName(self.ImgDesc, "KaiQiTiShi" .. style)
    UH.SpriteName(self.ImgWordBg1, "GuangGaoDi" .. style)
    UH.SpriteName(self.ImgWordBg2, "GuangGaoDi" .. style)
    UH.SpriteName(self.ImgWord1, "_LocGuangGao1" .. style)
    UH.SpriteName(self.ImgWord2, "_LocGuangGao2" .. style)
    UH.SpriteName(self.ImgTime, "ShiJianDi" .. style)
    UH.SpriteName(self.ImgClose, "GuanBi" .. style)
end

function FourSeasonCardPanel:FlushAll()
    self.ObjFree:SetActive(self.Data:CardRemind() > 0)
    self.Remind:SetNum(self.Data:CardRemind())
    self:OnChangeClick(0)
end

function FourSeasonCardPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function FourSeasonCardPanel:OnFreeClick()
    if not self:JudgeCommon() then
        return
    end
    if self.Data:CardRemind() > 0 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_FOUR_SEASON_CARD, FourSeasonCardReqType.Free)
    end
end

function FourSeasonCardPanel:OnChangeClick(value)
    value = value or 1
    self.now_page = self.now_page + value
    if self.now_page > self.max_page then
        self.now_page = 1
    end
    if self.now_page < 1 then
        self.now_page = 1
    end
    for i = 1, self.card_style_num do
        UH.SetText(self.TxtDescs[i], self.Data:CardTip(self.now_page))
    end
    self.List:SetData(self.base_data[self.now_page])
end

function FourSeasonCardPanel:OnBuyClick(data)
    if not self:JudgeCommon() then
        return
    end
    if not self.Data:CardIsCanBuy(data) then
        PublicPopupCtrl.Instance:Center(self.language.NotTimesTip)
        return false
    end
    self.now_buy_data = data
    local fee = data.buy_money / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_FOUR_SEASON_CARD, fee, ACTIVITY_TYPE.RAND_FOUR_SEASON_CARD, FourSeasonCardReqType.Buy, data.seq)
end

function FourSeasonCardPanel:JudgeCommon()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FOUR_SEASON_CARD) then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return false
    end
    return true
end

function FourSeasonCardPanel:SendBuy()
    if TableIsEmpty(self.now_buy_data) then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_FOUR_SEASON_CARD, self.now_buy_data.buy_money)
    self.now_buy_data = nil
end

--------------FourSeasonCardItem--------------
FourSeasonCardItem = FourSeasonCardItem or DeclareMono("FourSeasonCardItem", UIWidgetBaseItem)
function FourSeasonCardItem:FourSeasonCardItem()
    self.Data = FourSeasonData.Instance
end

function FourSeasonCardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjRoot.name = "FourSeasonCardItem" .. (data.seq + 1)
    for i = 1, self.Data:CardMaxShow() do
        self.ObjNodes[i]:SetActive(i == data.gift_icon)
        if i == data.gift_icon then
            UH.SetText(self.TxtTitle[i], data.gift_des)
            UH.SetText(self.TxtTitle2[i], data.gift_des)
            UH.SetText(self.TxtDiscount[i], data.gift_name)
            UH.SetText(self.TxtDiscount2[i], data.gift_name)
            UH.SetText(self.TxtPrice[i], data.buy_money / 10)
            local can_times, total_times = self.Data:CardLimitTimes(data)
            UH.SetText(self.TxtLimit[i], Format(Language.FourSeasonCard.LimitType[data.limit_type], can_times, total_times))
            local reward, max_num = self.Data:DealItem(data)
            local is_show_2 = max_num >= 4
            self.List[i]:SetObjActive(not is_show_2)
            self.List[i]:SetData(not is_show_2 and reward or {})
            self.List2[i]:SetObjActive(is_show_2)
            self.List2[i]:SetData(is_show_2 and reward or {})
            self.IHandler[i].Interactable = can_times > 0
        else
            self.List[i]:SetData({})
            self.List2[i]:SetData({})
            self.IHandler[i].Interactable = true
        end
    end
end