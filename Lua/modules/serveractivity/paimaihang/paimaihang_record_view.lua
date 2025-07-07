PaiMaiHangRecordView = PaiMaiHangRecordView or DeclareView("PaiMaiHangRecordView", "serveractivity/paimaihang/paimaihang_record")
VIEW_DECLARE_MASK(PaiMaiHangRecordView, ViewMask.BgBlockClose)

function PaiMaiHangRecordView:LoadCallback()
    self.card_handle = ServerActivityData.Instance.paimaihang_record_info:KeysCare(nil, BindTool.Bind1(self.OnFlush, self))
end

function PaiMaiHangRecordView:CloseCallback()
    ServerActivityData.Instance.paimaihang_record_info:Uncare(self.card_handle)
    self.card_handle = nil
end

function PaiMaiHangRecordView:OnFlush()
    local record_list = ServerActivityData.Instance:GetPaiMaiHangRecordList()
    self.RecordList:SetData(record_list)
    self.NoRecord:SetActive(TableIsEmpty(record_list))
end

PaiMaiHangRecordItem = PaiMaiHangRecordItem or DeclareMono("PaiMaiHangRecordItem", UIWidgetBaseItem)
function PaiMaiHangRecordItem:PaiMaiHangRecordItem()
    RichTextHelper.Bind(self.DescText, BindTool.Bind1(self.OnClickLink, self))
end

function PaiMaiHangRecordItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local time_t = os.date("*t", self.data.record_timestamp)
    UH.SetText(self.TimeText, Format(Language.PaiMaiHang.RecordTime, time_t.year, time_t.month, time_t.day, time_t.hour, time_t.min, time_t.sec))

    local item_cfg = ServerActivityData.Instance:GetPaiMaiHangRecordCfg(self.data.record_timestamp, self.data.activity_open_day, self.data.seq)
    local item = nil
    local item_name = ""
    if item_cfg ~= nil then
        item = Item.New(item_cfg.reward_item)
        item_name = item:QuaName()
    end

    local desc_str = ""
    if self.data.type == PaiMaiHangConfig.RA_PAI_MAI_HANG_RECORD_TYPE.TYPE_BID then
        desc_str = string.format(Language.PaiMaiHang.RecordTips[1], self.data.param1, item_name)
    elseif self.data.type == PaiMaiHangConfig.RA_PAI_MAI_HANG_RECORD_TYPE.TYPE_BID_FAILD then
        desc_str = Language.PaiMaiHang.RecordTips[2]
    elseif self.data.type == PaiMaiHangConfig.RA_PAI_MAI_HANG_RECORD_TYPE.TYPE_AUTO_BID then
        desc_str = string.format(Language.PaiMaiHang.RecordTips[3], self.data.param1, item_name, self.data.param2)
    elseif self.data.type == PaiMaiHangConfig.RA_PAI_MAI_HANG_RECORD_TYPE.TYPE_DEPOSIT_SUCC then
        desc_str = string.format(Language.PaiMaiHang.RecordTips[4], item_name, self.data.param1)
    elseif self.data.type == PaiMaiHangConfig.RA_PAI_MAI_HANG_RECORD_TYPE.TYPE_FETCH_DEPOSIT then
        desc_str = string.format(Language.PaiMaiHang.RecordTips[5], item_name, self.data.param1)
    end
    UH.SetText(self.DescText, desc_str)
end

function PaiMaiHangRecordItem:OnClickLink(dict)
    if not ServerActivityData.Instance:IsPaiMaiHangRecordCur(self.data.seq, self.data.record_timestamp) then
        PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error6)
        return
    end
    ViewMgr:CloseView(PaiMaiHangRecordView)
    ViewMgr:OpenView(PaiMaiHangView)
end
