VitalityFireworkView = VitalityFireworkView or DeclareView("VitalityFireworkView", "serveractivity/may_day/vitality_firework", Mod.MayDay.VitalityFirework)
VIEW_DECLARE_LEVEL(VitalityFireworkView, ViewLevel.L1)
VIEW_DECLARE_MASK(VitalityFireworkView, ViewMask.BgBlock)
function VitalityFireworkView:VitalityFireworkView()
    
end

function VitalityFireworkView:CloseCallback()

end

function VitalityFireworkView:OnCloseClick()
    ViewMgr:CloseView(VitalityFireworkView)
end

----------------------------VitalityFireworkPanel----------------------------
VitalityFireworkPanel = VitalityFireworkPanel or DeclareMono("VitalityFireworkPanel", UIWFlushPanel)
function VitalityFireworkPanel:VitalityFireworkPanel()
    self.Data = VitalityFireworkData.Instance
    self.language = Language.VitalityFirework
    self.data = self.Data.info_data
    self.other = self.Data:Other()
    self.data_cares = {
        {data = self.data, func = self.FlushAll, init_call = true},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function VitalityFireworkPanel:Awake()
    UIWFlushPanel.Awake(self)

    local open_time = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_VITALITY_FIREWORK)
    if open_time > 0 then
        local open_t = os.date("*t", open_time)
        local end_t = os.date("*t", ActivityData.GetSwitchStampTime(ACTIVITY_TYPE.RAND_VITALITY_FIREWORK) - 1)
        UH.SetText(self.TxtActTime, Format(self.language.ActTime, open_t.month, open_t.day, end_t.month, end_t.day))
    else
        UH.SetText(self.TxtActTime, Format(self.language.ActTime, 1, 1, 1, 1))
    end

    UH.SetText(self.TxtStartTime, self:GetTimeShow(self.other.time_start))
    UH.SetText(self.TxtEndTime, self:GetTimeShow(self.other.time_end))
    UH.SetText(self.TxtPrice, self.other.price / 10)
    UH.SetText(self.TxtDesc, self.other.desc)
    self.ModelUi:SetData({item_id = Item.Create(self.other.reward_item)})
end

function VitalityFireworkPanel:GetTimeShow(num)
    local hour = math.floor(num / 100)
    local sec = num % 100
    if sec < 10 then
        sec = "0" .. sec
    end
    return hour .. ":" .. sec
end

function VitalityFireworkPanel:FlushAll()
    self.List:SetData(self.Data:RewardConfig())
    self.ObjBuy:SetActive(not self.Data:IsBuy())
    self.ObjFree:SetActive(not self.Data:IsReceiveFree())
    UH.LocalPosG(self.TxtActTime.gameObject, Vector3.New(166, self.Data:IsBuy() and -231 or -284, 0))
    self.FreeRemind:SetNum(self.Data:IsReceiveFree() and 0 or 1)
end

function VitalityFireworkPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function VitalityFireworkPanel:OnBuyClick()
    local fee =  self.other.price / 10
    ServerActivityCtrl.Instance:SendBuyCmd(ACTIVITY_TYPE.RAND_VITALITY_FIREWORK, fee, ACTIVITY_TYPE.RAND_VITALITY_FIREWORK, VitalityFireworkReqType.Buy)
end

function VitalityFireworkPanel:OnFreeClick()
    VitalityFireworkCtrl.Instance:SendReq(VitalityFireworkReqType.Free)
end

function VitalityFireworkPanel:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_VITALITY_FIREWORK, self.other.price)
end

function VitalityFireworkPanel:OnDetailClick()
    CellClicks.BagGridNoButton({GetData = function()
        return Item.Create(self.other.reward_item)
    end})
end

--------------VitalityFireworkItem--------------
VitalityFireworkItem = VitalityFireworkItem or DeclareMono("VitalityFireworkItem", UIWidgetBaseItem)
function VitalityFireworkItem:VitalityFireworkItem()
    self.Data = VitalityFireworkData.Instance
end

function VitalityFireworkItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Root.name = "VitalityFireworkItem" .. (data.seq + 1)

    local reward_item = {}
    for k, v in pairs(data.reward_item) do
        local t = {}
        t.seq = data.seq
        t.item = v
        table.insert(reward_item, t)
    end
    self.List:SetData(reward_item)
    
    self.ObjGeted:SetActive(self.Data:RewardFlag(data.seq) == 1)
    self.ObjBtn:SetActive(self.Data:IsCanReceive(data.seq))
    if self.Data:IsBuy() then
        self.Remind:SetNum(self.Data:IsCanReceive(data.seq) and 1 or 0)
    else
        self.Remind:SetNum(0)
    end
    local open_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_VITALITY_FIREWORK)
    self.TxtTip:SetObjActive(open_day < (data.seq + 1))
    UH.SetText(self.TxtTip, Format(Language.VitalityFirework.Tip, DataHelper.GetDaXie(data.seq + 1)))
end

function VitalityFireworkItem:OnReceiveClick()
    if self.Data:IsBuy() then
        VitalityFireworkCtrl.Instance:SendReq(VitalityFireworkReqType.Receive, self.data.seq)
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
        PublicPopupCtrl.Instance:Center(Language.VitalityFirework.BuyTip)
    end
end

--------------VitalityFireworkCellItem--------------
VitalityFireworkCellItem = VitalityFireworkCellItem or DeclareMono("VitalityFireworkCellItem", UIWidgetBaseItem)
function VitalityFireworkCellItem:VitalityFireworkCellItem()
    self.Data = VitalityFireworkData.Instance
end

function VitalityFireworkCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(Item.Create(data.item))
    self.ObjEffect:SetActive(self.Data:IsCanReceive(data.seq))
end


--------------VitalityFireworkUseView--------------
VitalityFireworkUseView = VitalityFireworkUseView or DeclareView("VitalityFireworkUseView", "serveractivity/may_day/vitality_firework_use")
VIEW_DECLARE_LEVEL(VitalityFireworkUseView, ViewLevel.L1)
VIEW_DECLARE_MASK(VitalityFireworkUseView, ViewMask.Block)
function VitalityFireworkUseView:VitalityFireworkUseView()

end

function VitalityFireworkUseView:LoadCallback(param_t)
    TimeHelper:CancelTimer(self.ht)
    self.ht = Invoke(function()
        if param_t.show_list then
            ViewMgr:OpenView(GiftOpenView, {gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS, item_list = param_t.show_list})
        end
        ViewMgr:CloseView(VitalityFireworkUseView)
    end, 3)
end

function VitalityFireworkUseView:CloseCallback()
    TimeHelper:CancelTimer(self.ht)
end