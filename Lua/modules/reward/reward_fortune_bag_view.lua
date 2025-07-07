RewardFortuneBagView = RewardFortuneBagView or DeclareMono("RewardFortuneBagView", UIWFlushPanel)

function RewardFortuneBagView:RewardFortuneBagView()
    self.data_cares = {
        {data = ServerActivityData.Instance.fortune_bag_data, func = self.FlushFortuneBagView, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
    self.now_click_config = nil
end

function RewardFortuneBagView:Awake()
    UIWFlushPanel.Awake(self)
    local config = Config.fortune_bag_auto.other[1]
    local call_data = self:GetShowData(config)
    self.reward_item = Item.New(config.rewards_item)
    for i=1,self.CallList:Length() do
        self.CallList[i]:SetData(call_data[i])
    end
    self:FlushFortuneBagView()
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().show_month_and_fortunebag then
        self.BtnInter:SetObjActive(false)
    end
end

function RewardFortuneBagView:FlushFortuneBagView()
    local reward_config = Config.fortune_bag_auto.reward
    local is_show = true
    for i = 1, self.ItemList:Length() do
        self.ItemList[i]:SetData(reward_config[i])
        if is_show then
            is_show = ServerActivityData.Instance:GetFetchBuyFlag(reward_config[i].seq) == 0
        end
    end
    self.is_buy_day_gift = not is_show
    local has_batch = ServerActivityData.Instance:GetFortuneBagBatchTime() > 0
    if is_show or has_batch then
        UH.SetText(self.Time, Language.Reward.FortuneBagBtnTip[has_batch and 2 or 1])
        self:SetIsShow(not has_batch)
    else
        self.BuyTips:SetActive(false)
        UH.SetText(self.Time)
        self:SetIsShow(true)
    end
    self:FlusnRedNumView()
end

function RewardFortuneBagView:FlusnRedNumView()
    local num = ServerActivityData.Instance:GetFortuneBagNum()
    UH.SetIcon(self.Icon,Item.GetIconId(self.reward_item.item_id), ICON_TYPE.ITEM)
    UH.SetText(self.GoldNum, Format(Language.Reward.FortuneBagNum, Item.GetNum(27397)))
    self.Finish:SetActive(num == 0)
    self.Icon:SetObjActive(num ~= 0)
    self.RedPos:SetNum(num)
end

function RewardFortuneBagView:OnClickExchange()
    StoreData.Instance:SetJumpIndex({conver_type = 4})
    ViewMgr:OpenViewByKey(Mod.Store.Main)
end

function RewardFortuneBagView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[41].desc)
end

function RewardFortuneBagView:OnClickItem()
    if ServerActivityData.Instance:GetFortuneBagNum() == 1 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_FORTUNE_BAG,1)
    else
        CellClicks.BagGridNoButton({GetData = function()
            return self.reward_item
        end})
    end
end

function RewardFortuneBagView:OnBuyClick(data)
    self.now_click_config = data
    local fee = self.now_click_config.money / 10
    ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_REWARD_FORTUNE_BAG_NEW,fee,ACTIVITY_TYPE.RAND_FORTUNE_BAG, 2, self.now_click_config.seq)
end

function RewardFortuneBagView:SendBuy()
    if self.now_click_config then
        if self.is_batch then
            ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_REWARD_FORTUNE_BAG_NEW_BATCH, self.now_click_config.money)
        else
            ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_REWARD_FORTUNE_BAG_NEW, self.now_click_config.money)
        end
        self.now_click_config = nil
        ViewMgr:CloseView(RewardFortuneBagView)
    end
end

function RewardFortuneBagView:OnClickBuyTips()
    if self.is_buy_day_gift then
        PublicPopupCtrl.Instance:Center(Language.Reward.FortuneBagTip)
        return
    end
    self.is_batch = true
    self.now_click_config = Config.fortune_bag_auto.reward[#Config.fortune_bag_auto.reward]

    local fee = self.now_click_config.money / 10
    if ServerActivityCtrl.Instance:IsCanItemBuy(ACTIVITY_TYPE.RAND_FORTUNE_BAG, fee) then
        ServerActivityCtrl.Instance:SendBuyCmdReq(ACTIVITY_TYPE.RAND_FORTUNE_BAG, fee)
    else
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_FORTUNE_BAG, 3)
    end 
end

function RewardFortuneBagView:SetIsShow(state)
    UH.SetInter(self.BtnInter,state)
    self.BuyShow:SetActive(not state)
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().show_month_and_fortunebag then
        self.BuyTips:SetActive(false)
    else
        self.BuyTips:SetActive(state)
    end
    for i=1,self.ItemList:Length() do
        self.ItemList[i]:SetObjActive(state)
    end
end

function RewardFortuneBagView:GetShowData(config)
    local list = {}
    for i=1,4 do
        table.insert(list,config["reward_".. i])
    end
    return list
end

----------------------------RewardFortuneBagItem----------------------------
RewardFortuneBagItem = RewardFortuneBagItem or DeclareMono("RewardFortuneBagItem", UIWidgetBaseItem)

function RewardFortuneBagItem:SetData(data)
    data.item = Item.New(data)
    UH.SetIcon(self.Icon, Item.GetIconId(data.item_id), ICON_TYPE.ITEM)
    UH.SetText(self.Num,"x ".. data.num)
    UIWidgetBaseItem.SetData(self, data)
end

function RewardFortuneBagItem:OnClickItem()
    CellClicks.BagGridNoButton({GetData = function()
        return self.data.item
    end})
end
----------------------------RewardFortuneBagBtnItem----------------------------
RewardFortuneBagBtnItem = RewardFortuneBagBtnItem or DeclareMono("RewardFortuneBagBtnItem", UIWidgetBaseItem)

function RewardFortuneBagBtnItem:SetData(data)
    self.Discount:SetActive(data.discount ~= 0)
    UH.SetText(self.QiFuBiNum, data.praycoin)
    self.BtnInter.Interactable = ServerActivityData.Instance:GetFetchBuyFlag(data.seq) == 0
    UIWidgetBaseItem.SetData(self, data)
end