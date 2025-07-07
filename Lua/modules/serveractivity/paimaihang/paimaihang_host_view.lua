PaiMaiHangHostView = PaiMaiHangHostView or DeclareView("PaiMaiHangHostView", "serveractivity/paimaihang/paimaihang_host")
VIEW_DECLARE_MASK(PaiMaiHangHostView, ViewMask.BgBlockClose)

function PaiMaiHangHostView:LoadCallback()
    self.select_index = 1
    self.show_list = ServerActivityData.Instance:GetPaiMaiHangShowList()
    self.is_edit = false

    UH.onEndEdit(self.InputAdd, BindTool.Bind(self.OnTextVallChangeEnd, self))

    UH.SetText(self.AddText, self:MinAdd())

    for i, v in ipairs(self.show_list) do
        self.ItemCells[i]:SetData(Item.New(v.reward_item))
    end

    self.ToggleGroup:SetAllTogglesOff()
    self.ItemGroupItems[1]:Trigger()

    self.card_handle = ServerActivityData.Instance.paimaihang_user_info:KeysCare(nil, BindTool.Bind1(self.OnFlush, self))
end

function PaiMaiHangHostView:CloseCallback()
    ServerActivityData.Instance.paimaihang_user_info:Uncare(self.card_handle)
    self.card_handle = nil
end

function PaiMaiHangHostView:OnFlush()
    self.is_edit = false
    self:FlushInfo()
end

function PaiMaiHangHostView:OnClickItem(param)
    self.select_index = param
    self.is_edit = false
    self:FlushInfo()
end

function PaiMaiHangHostView:FlushInfo()
    self.EditMode.gameObject:SetActive(true)
    self.ShowMode.gameObject:SetActive(true)
    self:ShowView()
    self:EditView()

    if self:IsNomal() then
        self.is_edit = true
    end
    self.EditMode.gameObject:SetActive(self.is_edit)
    self.ShowMode.gameObject:SetActive(not self.is_edit)
end

function PaiMaiHangHostView:ShowView()
    local info = self:SelectInfo() or {}
    UH.SetText(self.ShowJiCunVal, info.bid_price)
    UH.SetText(self.ShowAddVal, info.add_price)
    UH.SetText(self.ShowAutoVal, Language.PaiMaiHang.AutoPrice[info.is_auto_bid or 0])
    UH.SetText(self.ShowBackVal, info.retrieve_gold)
    UH.SetText(self.BackText, Language.PaiMaiHang.BackState[2])
end

function PaiMaiHangHostView:EditView()
    local info = self:SelectInfo() or {}

    UH.SetText(self.InputSave, "")
    self.PriceGroup:SetAllTogglesOff()
    local bid_price = info.add_price or 0
    if bid_price == 0 or bid_price == self:MinAdd() then
        self.PriceGroupItems[1]:Trigger()
        UH.SetText(self.InputAdd, "")
    else
        UH.SetText(self.InputAdd, info.add_price)
        self.PriceGroupItems[2]:Trigger()
    end
    self.AutoGroup:SetAllTogglesOff()
    local is_auto = info.is_auto_bid or 1
    if self:IsNomal() then
        is_auto = 1
    end
    self.AutoGroupItems[is_auto == 1 and 1 or 2]:Trigger()

    UH.SetText(self.BackText, Language.PaiMaiHang.BackState[1])
    local show_back = self:IsShowBack()
    self.BackText.gameObject:SetActive(show_back)
    self.InputBack.gameObject:SetActive(show_back)
end

function PaiMaiHangHostView:OnClickClose()
    -- self.is_edit = false
    -- self:FlushInfo()
    ViewMgr:CloseView(PaiMaiHangHostView)
end

function PaiMaiHangHostView:OnClickOk()
    if self.AutoGroupToggles[1].isOn then
        if self:SelectInfo() and self:SelectInfo().is_auto_bid == 1 then
            local back_num = tonumber(self.InputBack.text) or 0
            local save_num = tonumber(self.InputSave.text) or 0
            local add_price = self.PriceGroupToggles[1].isOn and self:MinAdd() or tonumber(self.InputAdd.text) or 0
            if back_num ~= 0 and save_num ~= 0 then
                PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error9)
                return
            end
            --取回不为空，按取回处理
            --寄存部位空，按寄存处理
            --两个都为空，按加价处理
            if back_num ~= 0 then
                local remain_gold = self:SelectInfo().bid_price
                local notice_info = self:SelectNoticeInfo()
                local lock_num = false
                if notice_info ~= nil and notice_info.uid == RoleData.Instance:GetRoleId() and notice_info.is_auto_bid == 1 then
                    remain_gold = notice_info.remain_deposit_gold
                    lock_num = true
                    -- PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error11)
                    -- return
                end
                if back_num > remain_gold then
                    PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error8)
                    return
                end
                ServerActivityCtrl.Instance:SendPaiMaiHangReq(PaiMaiHangConfig.RA_PAI_MAI_HANG_REQ_TYPE.TYPE_RETRIEVE, self:SelectCfg().seq, back_num, add_price)
                return
            end
            if save_num ~= 0 then
                local save_num = tonumber(self.InputSave.text) or 0
                local add_price = self.PriceGroupToggles[1].isOn and self:MinAdd() or tonumber(self.InputAdd.text) or 0
                ServerActivityCtrl.Instance:SendPaiMaiHangReq(PaiMaiHangConfig.RA_PAI_MAI_HANG_REQ_TYPE.TYPE_AUTO_BID, self:SelectCfg().seq, save_num, add_price)
                return
            end
            if add_price ~= self:SelectInfo().add_price then
                ServerActivityCtrl.Instance:SendPaiMaiHangReq(PaiMaiHangConfig.RA_PAI_MAI_HANG_REQ_TYPE.TYPE_CHANGE_ADD_PRICE, self:SelectCfg().seq, add_price)
            end
        else
            local save_num = tonumber(self.InputSave.text) or 0
            local add_price = self.PriceGroupToggles[1].isOn and self:MinAdd() or tonumber(self.InputAdd.text) or 0
            ServerActivityCtrl.Instance:SendPaiMaiHangReq(PaiMaiHangConfig.RA_PAI_MAI_HANG_REQ_TYPE.TYPE_AUTO_BID, self:SelectCfg().seq, save_num, add_price)
        end
    elseif self.AutoGroupToggles[2].isOn then
        PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error10)
        if self:SelectInfo() and self:SelectInfo().is_auto_bid == 1 then
            ServerActivityCtrl.Instance:SendPaiMaiHangReq(PaiMaiHangConfig.RA_PAI_MAI_HANG_REQ_TYPE.TYPE_CANCEL_AUTO, self:SelectCfg().seq)
        end
    end
end

function PaiMaiHangHostView:OnTextVallChangeEnd()
    self:CheckAdd(true)
end

function PaiMaiHangHostView:CheckAdd(is_float)
    local add_val = tonumber(self.InputAdd.text) or 0
    if is_float and (add_val == 0 or add_val % self:MinAdd() ~= 0) then
        PublicPopupCtrl.Instance:Center(Language.PaiMaiHang.Error5)
    end
    add_val = math.max(add_val - (add_val % self:MinAdd()), self:MinAdd())
    self.InputAdd.text = add_val
end

function PaiMaiHangHostView:SelectCfg()
    return self.show_list[self.select_index]
end

function PaiMaiHangHostView:SelectInfo()
    return ServerActivityData.Instance:GetPaiMaiHangUserItemInfo(self:SelectCfg().seq)
end
function PaiMaiHangHostView:SelectNoticeInfo()
    return ServerActivityData.Instance:GetPaiMaiHangNoticeItemInfo(self:SelectCfg().seq)
end

function PaiMaiHangHostView:OnClickPrice()
    self.PriceGroupItems[2]:Trigger()
end
function PaiMaiHangHostView:OnSelectPrice(param)
end

function PaiMaiHangHostView:IsNomal()
    if self:SelectInfo() == nil then
        return true
    end
    local info = self:SelectInfo()
    -- if info.bid_price ~= 0 or info.add_price ~= 0 or info.retrieve_gold ~= 0 or info.is_auto_bid ~= 0 then
    --     return false
    -- end
    if info.is_auto_bid == 1 then
        return false
    end
    return true
end

function PaiMaiHangHostView:IsShowBack()
    return not self:IsNomal()
end

function PaiMaiHangHostView:MinAdd()
    return ServerActivityData.Instance:GetPaiMaiHangMinAdd()
end

function PaiMaiHangHostView:OnSelectAutoGroup()
end

function PaiMaiHangHostView:OnClickEdit()
    self.is_edit = true
    self:FlushInfo()
end