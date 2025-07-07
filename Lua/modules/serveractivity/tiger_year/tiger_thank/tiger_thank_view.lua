TigerThankView = TigerThankView or DeclareView("TigerThankView", "serveractivity/tiger_year/tiger_thank", Mod.TigerYear.TigerThank)
function TigerThankView:TigerThankView()
    
end

function TigerThankView:LoadCallback()
    
end

function TigerThankView:OnCloseCllick()
    ViewMgr:CloseView(TigerThankView)
end

function TigerThankView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[218].desc)
end

--------------------TigerThankPanel--------------------
TigerThankPanel = TigerThankPanel or DeclareMono("TigerThankPanel", UIWFlushPanel)
function TigerThankPanel:TigerThankPanel()
    self.Data = TigerThankData.Instance
    self.reward_config = self.Data:GetRewardConfig()
    self.buy_config = self.Data:GetBuyConfig()
    self.other = Config.yinhubaisui_auto.other[1]
    self.language = Language.TigerThank
    self.data = self.Data.tiger_thank_data
    self.act_type = ACTIVITY_TYPE.RAND_TIGER_THANK
    self.data_cares = {
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.FlushBuy, init_call = false},
        {data = self.data, func = self.FlushPanel, init_call = false},
    }
end

function TigerThankPanel:Awake()
    UIWFlushPanel.Awake(self)
    local item = Item.Create(self.buy_config.reward_item)
    self.ItemCell:SetData(item)
    self.ModelShow:SetData({item_id = item})
    self.Time:CreateActTime(self.act_type, TimeType.Type_Special_4, self.language.TimeDesc)
    UH.SetText(self.TxtPrice, self.buy_config.price / 10)

    ActivityData.Instance:SetHasSign(self.act_type)
    self:FlushPanel()
end

function TigerThankPanel:FlushPanel()
    self.CardList:SetData(self.reward_config)
    self.ObjTip:SetActive(self.data.info.is_buy == 1)
end

function TigerThankPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function TigerThankPanel:FlushBuy()
    if TableIsEmpty(self.buy_config) then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_TIGER_THANK, self.buy_config.price)
end

function TigerThankPanel:OnBuyClick()
    TigerThankCtrl.Instance:Send(1)
end

--------------------TigerThankItem--------------------
TigerThankItem = TigerThankItem or DeclareMono("TigerThankItem", UIWidgetBaseItem)
function TigerThankItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Title, data.show_times)
    self.ItemList:SetData(DataHelper.FormatItemList(data.reward_item))
    local receive_flag = TigerThankData.Instance:GetReceiveFlag(data.seq) > 0
    self.ObjGeted:SetActive(receive_flag)
    self.IHandler.Interactable = TigerThankData.Instance:OpenDay() >= self.data.activity_days
    self.Remind:SetNum(TigerThankData.Instance:OpenDay() >= self.data.activity_days and TigerThankData.Instance:GetInfo().is_buy > 0 and 1 or 0)
    self.ObjEffect:SetActive(self.IHandler.Interactable)
end

function TigerThankItem:OnReceiveClick()
    local is_buy = TigerThankData.Instance:GetInfo().is_buy > 0
    if is_buy then
        if TigerThankData.Instance:OpenDay() >= self.data.activity_days then
            TigerThankCtrl.Instance:Send(2, self.data.seq)
        else
            PublicPopupCtrl.Instance:Center(Language.TigerThank.Tip[2])
        end
    else
        PublicPopupCtrl.Instance:Center(Language.TigerThank.Tip[1])
    end
end