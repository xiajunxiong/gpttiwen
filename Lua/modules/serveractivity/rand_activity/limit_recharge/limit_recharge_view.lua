LimitRechargeView = LimitRechargeView or DeclareView("LimitRechargeView", "serveractivity/limit_recharge", Mod.RandActivity.LimitRecharge)
VIEW_DECLARE_MASK(LimitRechargeView, ViewMask.BgBlock)
VIEW_DECLARE_LEVEL(LimitRechargeView, ViewLevel.L1)
function LimitRechargeView:LimitRechargeView()
    self.Time:CreateActTime(ACTIVITY_TYPE.RANK_LIMIT_RECHARGE, TimeType.Type_Special_4)
end

function LimitRechargeView:OnCloseClick()
    ViewMgr:CloseView(LimitRechargeView)
end

----------------------LimitRechargePanel----------------------
LimitRechargePanel = LimitRechargePanel or DeclareMono("LimitRechargePanel", UIWFlushPanel)
function LimitRechargePanel:LimitRechargePanel()
    self.Data = LimitRechargeData.Instance
    self.data = self.Data.limit_recharge_data
    self.language = Language.LimitRecharge
    self.data_cares = {
        {data = self.data, func = self.FlushShow},
    }
end

function LimitRechargePanel:Awake()
    UIWFlushPanel.Awake(self)
end

function LimitRechargePanel:FlushShow()
    self.SpriteText.text = self.data.day_recharge_num
    self.List:SetData(self.Data:GetRewardConfig())
end

-------------------LimitRechargeItem-------------------
LimitRechargeItem = LimitRechargeItem or DeclareMono("LimitRechargeItem", UIWidgetBaseItem)
function LimitRechargeItem:LimitRechargeItem()
    self.Data = LimitRechargeData.Instance
end
function LimitRechargeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtTitle, Format(Language.LimitRecharge.ItemTitle, data.acc_price_down))
    self.CellList:SetData(DataHelper.FormatItemList(data.reward_item))
    local can_fetch = self.Data:IsCanFetch(data.seq)
    local fetched = self.Data:GetFetchedFlag(data.seq) > 0
    self.IHandlerBtn.Interactable = can_fetch
    self.ObjEffect:SetActive(can_fetch)
    self.RedPoint:SetObjActive(can_fetch)
    self.RedPoint:SetNum(can_fetch and 1 or 0)
    self.IHandlerBtn:SetObjActive(not fetched)
    self.ObjFetched:SetActive(fetched)
    self.ObjCannotGet:SetActive(self.Data:GetCanFetchFlag(data.seq) == 0)
end

function LimitRechargeItem:OnFetchClick()
    if LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:Center(Language.LimitRecharge.Tip)
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RANK_LIMIT_RECHARGE, 1, self.data.seq)
end