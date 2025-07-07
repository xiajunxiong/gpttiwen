OneYuanBuyView = OneYuanBuyView or DeclareView("OneYuanBuyView", "serveractivity/one_yuan_buy", Mod.RandActivity.OneYuanBuy)
VIEW_DECLARE_MASK(OneYuanBuyView, ViewMask.BgBlock)
VIEW_DECLARE_LEVEL(OneYuanBuyView, ViewLevel.L1)
function OneYuanBuyView:OneYuanBuyView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_ONE_YUAN_BUY, TimeType.Type_Special_4)
end

function OneYuanBuyView:OnCloseClick()
    ViewMgr:CloseView(OneYuanBuyView)
end


--------------OneYuanBuyPanel--------------
OneYuanBuyPanel = OneYuanBuyPanel or DeclareMono("OneYuanBuyPanel", UIWFlushPanel)
function OneYuanBuyPanel:OneYuanBuyPanel()
    self.data = ServerActivityData.Instance.one_yuan_buy_data
    self.other = Config.buy_one_gold_auto.other[1]
    self.data_cares = {
        {data = RoleData.Instance:GetBaseData(), func = self.FlushShow, keys = {"level"}, init_call = false},
        {data = self.data, func = self.FlushShow, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
    self:FlushShow()
end

function OneYuanBuyPanel:FlushShow()
    self.config = ServerActivityData.Instance:GetOneYuanBuyData()
    -- 买过或者没有配置都不给打开界面
    if not self.config or self.data.buy_times > 0 then
        ViewMgr:CloseView(OneYuanBuyView)
        return
    end
    UH.SetText(self.TxtPrice, self.other.buy_money / 10)
    UH.SetText(self.TxtValue, self.config.value)
    -- 这里把奖励分割开来
    local reward_data = {}
    for k, v in pairs(self.config.reward_item) do
        table.insert(reward_data, {reward_item = v})
    end
    self.ItemList:SetData(reward_data)
end

function OneYuanBuyPanel:OnBuyClick()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ONE_YUAN_BUY, 1, 0, 0, 0)
end

function OneYuanBuyPanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_ONE_YUAN_BUY, self.other.buy_money)
end


--------------OneYuanBuyItem--------------
OneYuanBuyItem = OneYuanBuyItem or DeclareMono("OneYuanBuyItem", UIWidgetBaseItem)
function OneYuanBuyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    self.Cell:SetData(Item.Create(data.reward_item))
    UH.SetText(self.TxtName, item:Name())
end