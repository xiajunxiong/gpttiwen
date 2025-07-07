SingleRebateView = SingleRebateView or DeclareView("SingleRebateView", "single_rebate/single_rebate", Mod.RandActivity.SingleRebate)
VIEW_DECLARE_LEVEL(SingleRebateView, ViewLevel.L1)
VIEW_DECLARE_MASK(SingleRebateView, ViewMask.BgBlock)
function SingleRebateView:SingleRebateView()
    self.Data = SingleRebateData.Instance
    self.language = Language.SingleRebate
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_SINGLE_REBATE, TimeType.Type_Time_3, self.language.Time)
end

function SingleRebateView:LoadCallback(param_t)
    
end

function SingleRebateView:CloseCallback()

end

function SingleRebateView:OnCloseClick()
    ViewMgr:CloseView(SingleRebateView)
end


----------------------------SingleRebatePanel----------------------------
SingleRebatePanel = SingleRebatePanel or DeclareMono("SingleRebatePanel", UIWFlushPanel)
function SingleRebatePanel:SingleRebatePanel()
    self.Data = SingleRebateData.Instance
    self.language = Language.SingleRebate
    self.data = self.Data.info_data
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = false},
    }
end

function SingleRebatePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.RectMask2D.enabled = true
    UH.SetText(self.TxtDesc, Format(self.language.Desc, self.Data:TitleShow()))
    self.Data:ClearFirstRemind()
    self:FlushAll()
end

function SingleRebatePanel:FlushAll()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SINGLE_REBATE) then
        PublicPopupCtrl.Instance:Center(self.language.ActEnd)
        ViewMgr:CloseView(SingleRebateView)
        ViewMgr:CloseView(SingleRebateBuyView)
        return
    end
    self.list_data = self.Data:PoolCfg()
    local nums = table.nums(self.list_data)
    self.ScrollRect.enabled = nums > 3
    --self.RectMask2D.enabled = nums > 3
    self.List:SetData(self.list_data)
end

function SingleRebatePanel:OnBuyClick()
    ViewMgr:OpenView(SingleRebateBuyView)
end

function SingleRebatePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

--------------SingleRebatePoolItem--------------
SingleRebatePoolItem = SingleRebatePoolItem or DeclareMono("SingleRebatePoolItem", UIWidgetBaseItem)
function SingleRebatePoolItem:SingleRebatePoolItem()
    self.Data = SingleRebateData.Instance
    self.language = Language.SingleRebate
end

function SingleRebatePoolItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ObjRoot.name = "SingleRebatePoolItem"..(data.gold_seq + 1)
    self.m_type = data.icon
    UH.SpriteName(self.ImgBg, "ItemBg"..self.m_type)
    UH.SetText(self.TxtTip1, Format(self.language.Tip1[self.m_type], data.tips1))
    self.OutlineTip2.effectColor = SingleRebateLineColor[self.m_type]
    UH.SetText(self.TxtPercent, data.tips2 .. "%")
    self.OutlinePercent.effectColor = SingleRebateLineColor[self.m_type]
    UH.SetText(self.TxtDesc, Format(self.language.DescItem, data.tips2))
    local can_times = self.Data:CanBuyTimes(data)
    local format_lang = can_times > 0 and self.language.LimitBuy[data.limit_type] or self.language.LimitBuyEd[data.limit_type]
    UH.SetText(self.TxtLimit, Format(format_lang, can_times, data.times))
    UH.SpriteName(self.IconSp, HuoBi[data.type])
    UH.SpriteName(self.IconSp2, HuoBi[data.type])
end

function SingleRebatePoolItem:OnGoClick()
    self.Data:NowShowPoolData(self.data)
    self.Data:NowShowPoolRewardData(self.data.gold_seq)
    ViewMgr:OpenView(SingleRebateBuyView)
end

function SingleRebatePoolItem:OnDestroy()
    
end