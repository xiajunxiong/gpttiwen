PaiMaiHangBuyView = PaiMaiHangBuyView or DeclareView("PaiMaiHangBuyView", "serveractivity/paimaihang/paimaihang_buy")
VIEW_DECLARE_MASK(PaiMaiHangBuyView, ViewMask.BgBlockClose)

function PaiMaiHangBuyView:LoadCallback(param_t)
    self.select_seq = param_t.seq
    self.select_cfg = ServerActivityData.Instance:GetPaiMaiHangCfg(self.select_seq)

    local item = Item.New(self.select_cfg.reward_item)
    self.ItemCell:SetData(item)
    UH.SetText(self.Name, item:QuaName())

    UH.onEndEdit(self.InputField, BindTool.Bind(self.OnTextVallChangeEnd, self))

    self.card_handle = ServerActivityData.Instance.paimaihang_user_info:KeysCare(nil, BindTool.Bind1(self.CheckClose, self))
end

function PaiMaiHangBuyView:CloseCallback()
    ServerActivityData.Instance.paimaihang_user_info:Uncare(self.card_handle)
    self.card_handle = nil
    TimeHelper:CancelTimer(self.check_price_timer)
    self.check_price_timer = nil
end

function PaiMaiHangBuyView:OnFlush()
    local info = self:NoticeInfo()
    if info.bid_price == 0 then
        UH.SetText(self.CurVal, self.select_cfg.base_price)
    else
        UH.SetText(self.CurVal, info.bid_price)
    end
    self:CheckPrice()
end

function PaiMaiHangBuyView:CheckClose()
    ViewMgr:CloseView(PaiMaiHangBuyView)
end

function PaiMaiHangBuyView:OnTextVallChangeEnd()
    TimeHelper:CancelTimer(self.check_price_timer)
    self.check_price_timer = TimeHelper:AddDelayTimer(BindTool.Bind2(self.CheckPrice, self, true), 0.1)
end

function PaiMaiHangBuyView:OnClickOk()
    TimeHelper:CancelTimer(self.check_price_timer)
    if not self:CheckPrice(true) then
        return
    end
    local over_num = tonumber(self.InputField.text) or 0
    ServerActivityCtrl.Instance:SendPaiMaiHangReq(PaiMaiHangConfig.RA_PAI_MAI_HANG_REQ_TYPE.TYPE_BID, self.select_seq, over_num)
end

function PaiMaiHangBuyView:OnClickAddNum(param)
    local add_num = param == 1 and self:MinAdd() or -self:MinAdd()
    self.InputField.text = (tonumber(self.InputField.text) or 0) + add_num
    self:CheckPrice(true)
end

function PaiMaiHangBuyView:MinAdd()
    return ServerActivityData.Instance:GetPaiMaiHangMinAdd()
end

function PaiMaiHangBuyView:MinNum()
    if self:NoticeInfo().bid_price == 0 then
        return self.select_cfg.base_price + self:MinAdd()
    end
    return self:NoticeInfo().bid_price + self:MinAdd()
end

function PaiMaiHangBuyView:NoticeInfo()
    return ServerActivityData.Instance:GetPaiMaiHangNoticeItemInfo(self.select_seq)
end
function PaiMaiHangBuyView:UserInfo()
    return ServerActivityData.Instance:GetPaiMaiHangUserItemInfo(self.select_seq)
end

function PaiMaiHangBuyView:GetCfg()
    return self.select_cfg
end

--is_float 是否弹提示
function PaiMaiHangBuyView:CheckPrice(is_float)
    local is_error = false
    local over_num = tonumber(self.InputField.text) or 0
    if over_num < self:MinNum() then
        over_num = self:MinNum()
        if is_float then
            PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error3)
            is_error = true
        end
    else
        local lerp = (over_num - self:MinNum()) % self:MinAdd()
        if lerp ~= 0 and is_float then
            PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error4)
            is_error = true
        end
        over_num = math.max(over_num - lerp, self:MinNum())
    end
    self.InputField.text = over_num
    return not is_error
end
