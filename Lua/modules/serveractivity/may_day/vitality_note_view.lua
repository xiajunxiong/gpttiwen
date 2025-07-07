
VitalityNoteView = VitalityNoteView or DeclareView("VitalityNoteView", MayDayConfig.ResPath .. "vitality_note", Mod.MayDay.VitalityNote)
-- VIEW_DECLARE_LEVEL(VitalityNoteView, ViewLevel.L0)

function VitalityNoteView:VitalityNoteView()
    PeriodActivityData.Instance:SetRemindData(Mod.MayDay.VitalityNote)
end

function VitalityNoteView:LoadCallback()
end

function VitalityNoteView:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(VitalityNoteView)
end

VitalityNoteViewF = VitalityNoteViewF or DeclareMono("VitalityNoteViewF", UIWFlushPanel)

function VitalityNoteViewF:VitalityNoteViewF()
    self.data = MayDayCtrl.Instance:Data()
    self.act_data = ActivityData.Instance
    self.language = Language.MayDay.VitalityNote

    self.data_cares = {
        {data = self.data.vitality_note_info, func = self.FlushInfoShow},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false}
    }

    self.RewardList:SetCompleteCallBack(function()
        if self.RewardIsComplete == nil then
            self.RewardListAnim:Play()
            self.RewardIsComplete = false
        end
    end)
end

function VitalityNoteViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushShow()
end

function VitalityNoteViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function VitalityNoteViewF:FlushShow()
    local task_list = self.data:VitalityNoteTaskList()
    self.ShowList:SetData(task_list)

    local reward_list = self.data:VitalityNoteRewardList()
    self.RewardList:SetData(reward_list)

    self.TimeShow.text = string.format(self.language.TimeShow, self.data:ActTimeShow(ACTIVITY_TYPE.RAND_VITALITY_NOTE))
    self.BtnBuyShow.text = string.format(self.language.BtnBuyShow, self.data:VitalityNoteConfigOtherPrice() / 10)
end

function VitalityNoteViewF:FlushInfoShow()
    local info = self.data.vitality_note_info
    if info.init then return end
    local can_buy = 0 == info.is_jinjie_jiangli
    self.ValueShow.text = string.format(self.language.ValueShow, info.combat)
    self.BtnBuyObj:SetActive(can_buy)
    self.MaskType2:SetActive(can_buy)
end

function VitalityNoteViewF:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_VITALITY_NOTE, self.data:VitalityNoteConfigOtherPrice())
end

function VitalityNoteViewF:OnClickBuy()
    local fee = self.data:VitalityNoteConfigOtherPrice() / 10

    if ServerActivityCtrl.Instance:IsCanItemBuy(ACTIVITY_TYPE.RAND_VITALITY_NOTE, fee) then
        ServerActivityCtrl.Instance:SendBuyCmdReq(ACTIVITY_TYPE.RAND_VITALITY_NOTE, fee)
    else
        MayDayCtrl.Instance:SendRAVitalityNoteBuy()
    end
end

function VitalityNoteViewF:OnClickArrow()
    local pos = self.ContentPos.localPosition
    local size = self.ContentPos.sizeDelta
    self.ContentPos.localPosition = Vector2.New(pos.x - math.min(283, size.x + pos.x), pos.y)
end

VitalityNoteViewShowItem = VitalityNoteViewShowItem or DeclareMono("VitalityNoteViewShowItem", UIWidgetBaseItem)
function VitalityNoteViewShowItem:VitalityNoteViewShowItem()
    self.md_data = MayDayCtrl.Instance:Data()
    self.language = Language.MayDay.VitalityNote
end

function VitalityNoteViewShowItem:OnDestroy()
    self:uncareData()
end

function VitalityNoteViewShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self:uncareData()
    self.care_handles = {}
    self:careData(self.md_data.vitality_note_info, BindTool.Bind(self.FlushShow, self))

    self.ValueShow.text = string.format(self.language.ValueShow1, data.combat)
end

function VitalityNoteViewShowItem:FlushShow()
    local vni = self.md_data.vitality_note_info
    if vni.init then return end
    local task_list = vni.task_list
    local is_complete = 1 == bit:_rshift(vni.complete_flag, self.data.seq1) % 2

    self.DescShow.text = string.format(self.language.TaskDescShow, self.data.describe, vni.task_list[self.data.seq1], self.data.parameter)
    self.BtnGoObj:SetActive(not is_complete)
    self.GetedObj:SetActive(is_complete)
end

function VitalityNoteViewShowItem:OnClickGo()
    if self.data.open_panel > 0 then
        ViewMgr:OpenViewByKey(self.data.open_panel)
        ViewMgr:CloseView(VitalityNoteView)
        ViewMgr:CloseView(MayDayView)
    end
end


VitalityNoteViewRewardItem = VitalityNoteViewRewardItem or DeclareMono("VitalityNoteViewRewardItem", UIWidgetBaseItem)
function VitalityNoteViewRewardItem:VitalityNoteViewRewardItem()
    self.md_data = MayDayCtrl.Instance:Data()
    self.language = Language.MayDay.VitalityNote
end

function VitalityNoteViewRewardItem:OnDestroy()
    self:uncareData()
end

function VitalityNoteViewRewardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self:uncareData()
    self.care_handles = {}
    
    local ri1 = data.reward1_item
    self.reward_item = Item.Init(ri1.item_id, ri1.num, ri1.is_bind)
    
    self.reward_list = {}
    for _, reward in pairs(data.reward2_item) do
        table.insert(self.reward_list, Item.Init(reward.item_id, reward.num, reward.is_bind))
    end

    self:careData(self.md_data.vitality_note_info, BindTool.Bind(self.FlushShow, self))
    
    self.ItemObj.name = "Item" .. data.index
    self.ValueShow.text = string.format(self.language.ValueShow2, data.combat)
end

function VitalityNoteViewRewardItem:FlushShow()
    local vni = self.md_data.vitality_note_info
    if vni.init then return end
    local can_buy = 0 == vni.is_jinjie_jiangli
    local can_get = vni.combat >= self.data.combat
    local is_get1 = 1 == bit:_rshift(vni.quanming_reward_fetch_flag, self.data.seq) % 2
    local is_get2 = 1 == bit:_rshift(vni.jinjie_reward_fetch_flag, self.data.seq) % 2
        
    self.MaskObj:SetActive(can_buy)

    self:ItemDeco(self.reward_item, can_get, is_get1, self.data.seq)

    for _, item in ipairs(self.reward_list) do
        self:ItemDeco(item, can_get and not can_buy, is_get2, self.data.seq)
    end

    self.RewardItem:SetData(self.reward_item)
    self.RewardList:SetData(self.reward_list)
end

function VitalityNoteViewRewardItem:ItemDeco(item, can_get, is_get, seq)
    if item then
        item.vnr_can_get = can_get
        item.vnr_is_get = is_get
        item.vnr_seq = seq
    end
end


VitalityNoteViewRewardCell = VitalityNoteViewRewardCell or DeclareMono("VitalityNoteViewRewardCell", UIWidgetBaseItem)
function VitalityNoteViewRewardCell:VitalityNoteViewRewardCell()
    self.md_data = MayDayCtrl.Instance:Data()
end

function VitalityNoteViewRewardCell:OnDestroy()
end

function VitalityNoteViewRewardCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.RewardCell:SetData(data)

    self.red_point = self.red_point or UiRedPoint.New(self.BtnGetObj, Vector3.New(40,40,0))
    self.red_point:SetNum((data.vnr_can_get and not data.vnr_is_get) and 1 or 0)

    self.BtnGetObj:SetActive(data.vnr_can_get and not data.vnr_is_get)
    self.MaskObj:SetActive(data.vnr_is_get or not data.vnr_can_get)
    self.GouObj:SetActive(data.vnr_is_get)
end

VitalityNoteViewRewardCell1 = VitalityNoteViewRewardCell1 or DeclareMono("VitalityNoteViewRewardCell1", VitalityNoteViewRewardCell)
VitalityNoteViewRewardCell2 = VitalityNoteViewRewardCell2 or DeclareMono("VitalityNoteViewRewardCell2", VitalityNoteViewRewardCell)

function VitalityNoteViewRewardCell1:OnClickGet()
    MayDayCtrl.Instance:SendRAVitalityNoteGet1(self.data.vnr_seq)
end

function VitalityNoteViewRewardCell2:OnClickGet()
    MayDayCtrl.Instance:SendRAVitalityNoteGet2(self.data.vnr_seq)
end