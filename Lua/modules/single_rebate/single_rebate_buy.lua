SingleRebateBuyView = SingleRebateBuyView or DeclareView("SingleRebateBuyView", "single_rebate/single_rebate_buy")
VIEW_DECLARE_MASK(SingleRebateBuyView, ViewMask.BgBlock)
function SingleRebateBuyView:SingleRebateBuyView()

end

function SingleRebateBuyView:LoadCallback(param_t)
    
end

function SingleRebateBuyView:OnCloseClick()
    ViewMgr:CloseView(SingleRebateBuyView)
end

----------------------------SingleRebateBuyPanel----------------------------
SingleRebateBuyPanel = SingleRebateBuyPanel or DeclareMono("SingleRebateBuyPanel", UIWFlushPanel)
function SingleRebateBuyPanel:SingleRebateBuyPanel()
    self.Data = SingleRebateData.Instance
    self.language = Language.SingleRebate
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function SingleRebateBuyPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.now_pool_data = self.Data:NowShowPoolData()
    self.now_pool_reward_data = self.Data:NowShowPoolRewardData()
    self.m_type = self.now_pool_data.limit_type
    self.total_times = self.now_pool_data.times
    self:FlushAll()
end

function SingleRebateBuyPanel:FlushAll()
    self.can_times = self.Data:CanBuyTimes(self.now_pool_data)
    if self.can_times > 0 then
        UH.SetText(self.TxtDesc, Format(self.language.BuyDesc[self.m_type], self.can_times, self.total_times))
    else
        UH.SetText(self.TxtDesc, Format(self.language.BuyDescEd[self.m_type], self.can_times, self.total_times))
    end
    local num = table.nums(self.now_pool_reward_data)
    if num > 3 then
        self.List:SetData(self.now_pool_reward_data)
    else
        self.List2:SetData(self.now_pool_reward_data)
    end
    self.now_buy_item = nil
end

function SingleRebateBuyPanel:OnBuyClick(data)
    if self.now_buy_item then
        return
    end
    local can_times = self.Data:CanBuyTimes(self.Data:NowShowPoolData())
    if can_times > 0 then
        -- 记录当前购买道具，用于校验
        self.now_buy_item = data
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_SINGLE_REBATE, data.buy_money / 10, ACTIVITY_TYPE.RAND_SINGLE_REBATE, SingleRebateReqType.MoneyBuy, data.gold_seq, data.seq)
    else
        PublicPopupCtrl.Instance:Center(self.language.LimitTip[1])
    end
end

function SingleRebateBuyPanel:SendBuy()
    if self.now_buy_item == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_SINGLE_REBATE, self.now_buy_item.buy_money)
    self.now_buy_item = nil
end

function SingleRebateBuyPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------SingleRebateBuyItem--------------
SingleRebateBuyItem = SingleRebateBuyItem or DeclareMono("SingleRebateBuyItem", UIWidgetBaseItem)
function SingleRebateBuyItem:SingleRebateBuyItem()
    self.Data = SingleRebateData.Instance
    self.language = Language.SingleRebate
end

function SingleRebateBuyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtPercent, data.tips .. "%")
    if data.type == CurrencyType.Gold then
        UH.SetText(self.TxtDesc, Format(self.language.Reward, data.gold_num))
        UH.SetText(self.TxtDesc2, Format(self.language.DescItem2, data.gold_give))
    else
        UH.SetText(self.TxtDesc, Format(self.language.Reward2, data.gold_num))
        UH.SetText(self.TxtDesc2, Format(self.language.DescItem3, data.gold_give))
    end
    UH.SetText(self.TxtPrice, data.buy_money / 10)
    self.ObjBuyed:SetActive(self.Data:CanBuyTimes(self.Data:NowShowPoolData()) == 0)
    if self.effect_handler then
        self.EffectTool:Stop(self.effect_handler)
    end
    if data.type == CurrencyType.Gold then
        self.effect_handler = self.EffectTool:Play(tostring(10010238 + data.gold_icon), self.ImgIcon.gameObject)
        UH.SpriteName(self.ImgIcon, "gold_icon_" .. data.gold_icon)
    else
        UH.SpriteName(self.ImgIcon, "LingYu" .. data.gold_icon)
    end
end