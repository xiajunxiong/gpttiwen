WorkShopQuickBuy = WorkShopQuickBuy or DeclareView("WorkShopQuickBuy", "workshop/workshop_quickbuy")
function WorkShopQuickBuy:WorkShopQuickBuy()
    self.data = WorkshopData.Instance
    self.buy_num = 1
    self.is_level_up = false
end
function WorkShopQuickBuy:LoadCallback(level_up)
    -- self.Alert:SetData(self:GetType(), Vector2.New(471, 332))
    self.CostCell:SetData({})
    self.is_level_up = level_up
    -- if self.is_level_up == true then
    UH.SetText(self.TipDesc, "")
    -- end

    LoginCtrl.Instance:RoleSubscribe(RoleSubscribeTopicType.Market)
end
function WorkShopQuickBuy:OnClickClose()
    ViewMgr:CloseView(WorkShopQuickBuy)
    AudioMgr:PlayEffect(AudioType.UI,  CommonAudio.ClickGeneral2)
end

function WorkShopQuickBuy:CloseCallback()
    LoginCtrl.Instance:RoleSubscribeCancel(RoleSubscribeTopicType.Market)
end
function WorkShopQuickBuy:OnFlush()
    self.info = self.data:MateData()
    if self.info.fin_gem_id then
        -- local cur_gem = self.info.target_gem - 1
        self.CostCell:SetData(Item.Create({item_id = self.info.target_gem--[[ , num = self.info.fin_need_num ]]}))
        self.Choose:SetActive(false)
        if self.is_level_up == true then
            local own_str = ""
            for i, v in ipairs(self.info.own_list) do
                own_str =
                    own_str ..
                    string.format(Language.Workshop.CostName, v.num, Item.Create({item_id = v.item_id}):QuaName())
            end
            local str = string.format(Language.Workshop.CostLevelUp, own_str, Item.GetQuaName(self.info.target_gem))
            UH.SetText(self.CostNum, str)
            UH.SetText(self.CostNum2, str)
            UH.SetText(self.OkDesc, Language.Common.LevelUp)
        else
            local own_str = ""
            for i, v in ipairs(self.info.own_list) do
                own_str = own_str .. string.format(Language.Workshop.CostName, v.num, Item.GetQuaName(v.item_id))
            end
            local str
            if own_str == "" then
                str =
                    string.format(
                    Language.Workshop.CostLevelUp3,
                    MarketData.Instance:GetItemPrice(self.info.fin_gem_id) * self.info.fin_need_num,
                    Language.Common.HuoBi[self.info.money_type],
                    self.info.fin_need_num,
                    Item.GetQuaName(self.info.fin_gem_id),
                    Item.GetQuaName(self.info.target_gem)
                )
            else
                str =
                    string.format(
                    Language.Workshop.CostLevelUp2,
                    own_str,
                    MarketData.Instance:GetItemPrice(self.info.fin_gem_id) * self.info.fin_need_num,
                    Language.Common.HuoBi[self.info.money_type],
                    self.info.fin_need_num,
                    Item.GetQuaName(self.info.fin_gem_id),
                    Item.GetQuaName(self.info.target_gem)
                )
            end
            UH.SetText(self.CostNum, str)
            UH.SetText(self.CostNum2, str)
        end
        self.buy_num = self.info.fin_need_num
        UH.SetText(self.BuyNum, self.buy_num)
    else
        self.CostCell:SetData(self.info)
        self.Choose:SetActive(true)
        self:FlushResult()
    end
end
function WorkShopQuickBuy:FlushResult()
    local data = ShopData.GetFastBuyData(self.info.item_id)
    if data == nil then
        Debuger.LogErrorSD("快速购买配置表没配")
        self:OnClickCancel()
        return
    end
    local str =
        string.format(
        Language.Workshop.CostBuyDesc,
        self.buy_num,
        Item.GetQuaName(self.info.item_id),
        MarketData.Instance:GetItemPrice(self.info.item_id) * self.buy_num,
        Language.Common.HuoBi[data.money_type]
    )
    UH.SetText(self.CostNum, str)
    UH.SetText(self.CostNum2, str)
    UH.SetText(self.BuyNum, self.buy_num)
end
function WorkShopQuickBuy:OnClickOk()
    -- LogError("???OnClickOk",MallCtrl.IsNotCurrency(CurrencyType.ImmortalCoin,self.buy_num))
    local need_fun = function (buy_num)
        if not MallCtrl.IsNotCurrency(CurrencyType.ImmortalCoin,buy_num) then
            PublicPopupCtrl.Instance:Center(Language.Common.ImmortalCoinLack)
            MallData.JumpItemNum = MarketData.Instance:GetNeedGoldNum(buy_num)
            return true 
        end        
        return false
    end


    if self.info.fin_gem_id then
        if need_fun(MarketData.Instance:GetItemPrice(self.info.fin_gem_id) * self.info.fin_need_num) then 
            return 
        end 
        WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_GEM_CONF_UGD)
    else
        if self.is_level_up == true then
            if need_fun(MarketData.Instance:GetItemPrice(self.info.fin_gem_id) * self.info.fin_need_num) then 
                return 
            end 
            WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_GEM_CONF_UGD)
        else
            if need_fun(MarketData.Instance:GetItemPrice(self.info.item_id) * self.buy_num) then 
                return 
            end 
            
            MarketCtrl.Instance:SendMarketReq(1,self.info.item_id,self.buy_num)
        end
    end
    ViewMgr:CloseView(WorkShopQuickBuy)
    AudioMgr:PlayEffect(AudioType.UI,  CommonAudio.ComposeSucc)
end
function WorkShopQuickBuy:OnClickCancel()
    AudioMgr:PlayEffect(AudioType.UI,  CommonAudio.ClickGeneral2)
    ViewMgr:CloseView(WorkShopQuickBuy)
end

function WorkShopQuickBuy:OnClickAdd(num)
    self.buy_num = self.buy_num + num
    if self.buy_num <= 0 then
        self.buy_num = 1
    end
    if self.buy_num >= 99 then
        self.buy_num = 99
    end
    UH.SetText(self.BuyNum, self.buy_num)
    self:FlushResult()
end

WorkShopQuickBuyPanel = WorkShopQuickBuyPanel or DeclareMono("WorkShopQuickBuyPanel", UIWFlushPanel)
function WorkShopQuickBuyPanel:WorkShopQuickBuyPanel()
    self.data_cares = {
        {data = MarketData.Instance.count_data,  func = self.FlushView,init_call = false,keys = {"val"}},
    }
end

function WorkShopQuickBuyPanel:FlushView()
    self.View:OnFlush()
end