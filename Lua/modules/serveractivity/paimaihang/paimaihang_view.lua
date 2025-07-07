PaiMaiHangView = PaiMaiHangView or DeclareView("PaiMaiHangView", "serveractivity/paimaihang/paimaihang", Mod.RandActivity.PaiMaiHang)
VIEW_DECLARE_LEVEL(PaiMaiHangView, ViewLevel.L0)

function PaiMaiHangView:LoadCallback()
	LoginCtrl.Instance:RoleSubscribe(RoleSubscribeTopicType.TYPE_RA_PAI_MAI_HANG)
    ServerActivityData.Instance:SavePaiMaiHangTodayRemind()
    ServerActivityData.Instance.paimaihang_other_info.remind_val = 0
    ServerActivityData.Instance.paimaihang_other_info.show_record = false
end

function PaiMaiHangView:CloseCallback()
	LoginCtrl.Instance:RoleSubscribeCancel(RoleSubscribeTopicType.TYPE_RA_PAI_MAI_HANG)
end

PaiMaiHangPanel = PaiMaiHangPanel or DeclareMono("PaiMaiHangPanel", UIWFlushPanel)
function PaiMaiHangPanel:PaiMaiHangPanel()
    self.server_data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.server_data.paimaihang_notice_info, func = self.FlushView},
        {data = self.server_data.paimaihang_user_info, func = self.FlushView, init_call = false}
    }
end

function PaiMaiHangPanel:FlushView()
    self.ItemList:SetData(self.server_data:GetPaiMaiHangShowList())
end

function PaiMaiHangPanel:OnClickHost()
    ViewMgr:OpenView(PaiMaiHangHostView)
end

function PaiMaiHangPanel:OnClickRecord()
    ServerActivityCtrl.Instance:SendPaiMaiHangReq(PaiMaiHangConfig.RA_PAI_MAI_HANG_REQ_TYPE.TYPE_RECORD_INFO)
    ViewMgr:OpenView(PaiMaiHangRecordView)
end

function PaiMaiHangPanel:OnClickTips()
    PublicPopupCtrl.Instance:HelpTip(156)
end

function PaiMaiHangPanel:OnClickClose()
    ViewMgr:CloseView(PaiMaiHangView)
end

PaiMaiHangItem = PaiMaiHangItem or DeclareMono("PaiMaiHangItem", UIWidgetBaseItem)
function PaiMaiHangItem:PaiMaiHangItem()
    self.server_data = ServerActivityData.Instance
end

function PaiMaiHangItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.item = Item.New(self.data.reward_item)
    self.ItemCell:SetData(self.item)
    UH.SetText(self.Name, self.item:QuaName())

    local user_info = self:UserInfo() or {}
    local notice_info = self:NoticeInfo() or {}
    self.OverNode:SetActive(self:IsOver())

    if not self:IsBegin() then
        self.time_str = Language.PaiMaiHang.TimeState[1]
        self.Timer:StampTime(self.server_data:GetPaiMaiHangBeginTime(), TimeType.Type_Special_8, self.time_str)
    elseif self:IsOver() then
        if notice_info.uid ~= 0 then
            UH.SetText(self.OverText, Language.PaiMaiHang.OverState[2])
        else
            UH.SetText(self.OverText, Language.PaiMaiHang.OverState[1])
        end
        self.ItemCell:MakeGrey(true)
        self.Timer:SetTime(Language.PaiMaiHang.TimeOver)
    else
        self.time_str = Language.PaiMaiHang.TimeState[2]
        self.Timer:StampTime(notice_info.end_timestamp, TimeType.Type_Special_8, self.time_str)
    end
    UH.SetText(self.CurVal, notice_info.bid_price or 0)
    UH.SetText(self.OwnVal, user_info.last_bid_gold or 0)
    self.BtnJoin.Interactable = not self:IsUid() and not self:IsOver() and self:IsBegin()
end

function PaiMaiHangItem:OnClickShow()
    if self.item == nil then
        return
    end
    if self.item:ShowType() == ShowType.Pet then
        local pet = Pet.New({pet_id = self.item.item_id})
        pet.view_btn_hide = {"BtnDetail", "BtnStamp", "PetWeapon"}
        ViewMgr:OpenView(PetDetailView, pet)
    else
        local t_table = {
            GetData = function()
                return self.item
            end
        }
        CellClicks.BagGridNoButton(t_table)
    end
end

function PaiMaiHangItem:OnClickBuy()
    if not self:IsBegin() then
        PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error7)
        return
    elseif self:IsOver() then
        PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error6)
        return
    elseif self:IsUid() then
        PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error1)
        return
    end
    ViewMgr:OpenView(PaiMaiHangBuyView, {seq = self.data.seq})
end

function PaiMaiHangItem:NoticeInfo()
    return self.server_data:GetPaiMaiHangNoticeItemInfo(self.data.seq)
end
function PaiMaiHangItem:UserInfo()
    return self.server_data:GetPaiMaiHangUserItemInfo(self.data.seq)
end
function PaiMaiHangItem:IsUid()
    if self:NoticeInfo() ~= nil and self:NoticeInfo().uid == RoleData.Instance:GetRoleId() then
        return true
    end
    return false
end
function PaiMaiHangItem:IsOver()
    return self:NoticeInfo() ~= nil and self:NoticeInfo().is_big_end_flag == 1
end
function PaiMaiHangItem:IsBegin()
    return self:NoticeInfo() ~= nil and TimeHelper.GetServerTime() >= self.server_data:GetPaiMaiHangBeginTime()
end
