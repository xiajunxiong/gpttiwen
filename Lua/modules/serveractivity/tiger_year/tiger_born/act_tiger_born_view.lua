-- ================= 寅虎问世 ===========
ActTigerBornView = ActTigerBornView or DeclareView("ActTigerBornView", "serveractivity/tiger_year/act_tiger_born_view", Mod.TigerYear.TigerBorn)
function ActTigerBornView:ActTigerBornView()
    
end

function ActTigerBornView:LoadCallback()
    
end

function ActTigerBornView:OnCloseCllick()
    ViewMgr:CloseView(ActTigerBornView)
end

ActTigerBornPanel = ActTigerBornPanel or DeclareMono("ActTigerBornPanel", UIWFlushPanel)
function ActTigerBornPanel:ActTigerBornPanel()
    self.data = ActTigerBornData.Instance
    self.data_cares = {
        {data = self.data.act_info, func = self.FlushPanel, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.FlushBuy, init_call = false},
    }
    self.act_type = ACTIVITY_TYPE.RAND_TIGER_BORN
end

function ActTigerBornPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.buy_cfg = self.data:GetBuyCfg()
    local item = Item.Create(self.buy_cfg.reward_item)
    self.ItemCell:SetData(item)
    self.ModelShow:SetData({item_id = item})
    self.Time:CreateActTime(self.act_type,TimeType.Type_Special_4,Language.TigerBorn.TimeDesc)
    UH.SetText(self.Price, self.buy_cfg.price / 10)

    ActivityData.Instance:SetHasSign(self.act_type)
    self.data.act_info:Notify()
    --self:FlushPanel()
end

function ActTigerBornPanel:FlushPanel()
    local is_active = self.data:IsActive()
    self.ActiveImg:SetActive(not is_active)

    local list_data = self.data:GetCardListData()
    self.CardList:SetData(list_data)

    local buy_tip = is_active and Language.TigerBorn.Tip1 or Language.TigerBorn.Tip2
    UH.SetText(self.BuyTip, buy_tip)
    self.Interactor.Interactable = not is_active
    self.BtnEffect:SetActive(not is_active)

    local reward_list = self.data:GetRewardListData()
    self.RewardList:SetData(reward_list)
end

function ActTigerBornPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function ActTigerBornPanel:FlushBuy()
    if self.buy_cfg == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_TIGER_BORN, self.buy_cfg.price)
end

function ActTigerBornPanel:OnBuyClick()
    local is_active = self.data:IsActive()
    if is_active then
        PublicPopupCtrl.Instance:Center(Language.TigerBorn.Tip1)
        return
    end
    local fee = self.buy_cfg.price / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_TIGER_BORN, fee, ACTIVITY_TYPE.RAND_TIGER_BORN, ActTigerBornCtrl.ReqType.Buy)
end

function ActTigerBornPanel:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[217].desc)
end

ActTigerBornCard = ActTigerBornCard or DeclareMono("ActTigerBornCard", UIWidgetBaseItem)
function ActTigerBornCard:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local flag = ActTigerBornData.Instance:GetCardFlag(data)
    self.InfoObj:SetActive(flag == -1)
    self.Mask1Obj:SetActive(flag == 1)
    self.Mask2Obj:SetActive(flag == 0)
    local day_str = Split(data.show_times, "|")
    if flag ~= -1 then
        UH.SetText(self.Text1, day_str[2])
        UH.SetText(self.Text2, day_str[2])
        UH.SetText(self.Text3, day_str[1])
        UH.SetText(self.Text3, day_str[1])
        self.EffectTool:Play(3161087 + data.seq)
    else
        UH.SetText(self.Title, day_str[1] .. day_str[2])
        local reward_type = ActTigerBornData.Instance:GetCardRewardType(data.seq)
        local item_list
        if reward_type >=0 then
            local reward_cfg = ActTigerBornData.Instance:GetRewardCfg(reward_type)
            item_list = DataHelper.FormatItemList(reward_cfg.reward_item)
            self.ItemList:SetData(item_list)
        end
        if self.flag and flag ~= self.flag and item_list then
            ViewMgr:OpenView(GiftOpenView,{item_list = item_list,gift_type = GIFT_OPEN_TYPE.SPECIAL})
        end
    end
    self.flag = flag
end

function ActTigerBornCard:OnClick()
    local flag = self.flag or ActTigerBornData.Instance:GetCardFlag(self.data)
    if not ActTigerBornData.Instance:IsActive() then
        PublicPopupCtrl.Instance:Error(Language.TigerBorn.Tip2)
        return
    end
    if flag == -1 then
        return
    end
    if flag == 1 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_TIGER_BORN, ActTigerBornCtrl.ReqType.Get, self.data.seq)
    else
        PublicPopupCtrl.Instance:Error(Language.TigerBorn.Tip3)
    end
end