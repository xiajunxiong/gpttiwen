DuelDialogView = DuelDialogView or DeclareView("DuelDialogView", "duel/duel_dialog")
function DuelDialogView:LoadCallback(param_t)
    self.info = param_t
    self:FlushInfo()
end

function DuelDialogView:CloseCallback()
end

function DuelDialogView:OnFlush()
    self:FlushInfo()
end

function DuelDialogView:FlushInfo()
    UH.SetText(self.Content, self.info.content)

    local item_info = self.info.item_info
    self.CurrencyObj:SetActive(item_info ~= nil)
    if item_info ~= nil then
        self.Item:SetData(Item.New(item_info.item))
        UH.SetText(self.CurrencyText, item_info.cost)
    end

    local info_cancel = self.info.cancel or {}
    self.cancel_func = info_cancel.func

    local info_confirm = self.info.confirm or {}
    self.confirm_func = info_confirm.func
end

function DuelDialogView:OnClickCancel()
    if self.cancel_func ~= nil and type(self.cancel_func) == "function" then
        self.cancel_func()
    else
        ViewMgr:CloseView(DuelDialogView)
    end
end

function DuelDialogView:OnClickConfirm()
    if self.confirm_func ~= nil and type(self.confirm_func) == "function" then
        self.confirm_func()
    end
end

function DuelDialogView:OnClickClose()
    ViewMgr:CloseView(DuelDialogView)
end
