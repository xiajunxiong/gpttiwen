DialogTipsView = DialogTipsView or DeclareView("DialogTipsView", "public_popup/dialog_tips",nil,ViewLayer.Top + 1)
VIEW_DECLARE_CACHE(DialogTipsView)

function DialogTipsView:DialogTipsView()
    self.data = PublicPopupCtrl.Instance.dialog_tips_data
    self.beh_data = BehaviorData.Instance
    self.reply_type = Language.PublicPopup.ReplyType
    self.bg_v2 = Vector2.New(446, 241)
    self.today_v2 = Vector2.New(-70, -5)
    self.btn_v2 = Vector2.New(0, -70)
    self.content_v2 = Vector2.New(0, 17.5)

    GuideManager.Instance:RegisterGetGuideUi("DialogTipsOK", function()
        return self.BtnConfirmObj, function (  )
            self:OnClickConfirm()
        end
    end)
end

function DialogTipsView:LoadCallback()
    self:FlushInfo(true)
    if self.beh_data.auto_behavior > 0 then
        self.timer_auto_beh = TimeHelper:AddDelayTimer(function ()
            self:OnClickConfirm()
        end,
        self.beh_data.auto_behavior)
    end
end

function DialogTipsView:CloseCallback()
    if not self.data:InfoEmpty() then
        ViewMgr:OpenView(DialogTipsView)
    end
    TimeHelper:CancelTimer(self.timer_tt_confirm)
    TimeHelper:CancelTimer(self.timer_tt_cancel)
    TimeHelper:CancelTimer(self.timer_auto_beh)
    TimeHelper:CancelTimer(self.confirm_wait_handle)
    TimeHelper:CancelTimer(self.cancel_wait_handle)
end

function DialogTipsView:OnFlush()
    self:FlushInfo(false)
end

function DialogTipsView:FlushInfo(is_next)
    local info = is_next and self.data:NextInfo() or self.data:GetCurInfo() or {}

    UH.SizeDelta2(self.BgRect, info.bg_v2 or self.bg_v2)
    UH.AnchoredPosition(self.TodayRect, info.today_v2 or self.today_v2)
    UH.AnchoredPosition(self.BtnRect, info.btn_v2 or self.btn_v2)
    UH.AnchoredPosition(self.ContentRect, info.content_v2 or self.content_v2)
    
    self.new_tnr = info.new_tnr
    self.tnr = info.tnr
    self.TodayNotRemind:SetObjActive(nil ~= self.new_tnr or nil ~= self.tnr)
    if nil ~= self.new_tnr then
        self.TodayNotRemind.isOn = DialogTipsData.Instance:TodayIsTrue(info.new_tnr)
    elseif nil ~= self.tnr then
        self.TodayNotRemind.isOn = self.data.today_not_reminds[self.tnr]
    end
    if nil ~= self.new_tnr or nil ~= self.tnr then
        UH.SetText(self.TodayNotRemindText, info.trn_str or Language.Common.TodayNotRemind)
    end

    self.BtnClose:SetActive(info.close)
    self.BtnConfirmObj:SetActive(not info.no_confirm)
    self.BtnCanelObj:SetActive(not info.no_cancel)
    self.BtnConfirmObj:SetSiblingIndex(info.change_btn_pos and 0 or 1)
    self.Content.text = info.content

    local info_cancel = info.cancel or {}
    self.cancel_func = info_cancel.func
    self.CancelName.text = info_cancel.name or self.reply_type.cancel

    local info_confirm = info.confirm or {}
    self.confirm_func = info_confirm.func
    self.ConfirmName.text = info_confirm.name or self.reply_type.confirm
    self.content_func = info.content_func
    self:SetRemain(info.remain_type)
    self.ConfirmInteractor.Interactable = true
    self.CancelInteractor.Interactable = true
    if info.confirm then
        --自动执行倒计时
        if info.confirm.time then
            -- self.ConfirmTime:SetCallBack(function()ViewMgr:CloseView(DialogTipsView)end)
            -- self.ConfirmTime:TotalTime(info.confirm.time,TimeType.Type_Special_2,info_confirm.name)
            TimeHelper:CancelTimer(self.timer_tt_confirm)
            self.confirm_tt = math.floor(info.confirm.time)
            self.timer_tt_confirm = TimeHelper:AddCountDownTT(function ()
                self.ConfirmName.text = string.format("%s(%s)", info_confirm.name or self.reply_type.confirm, self.confirm_tt)
                self.confirm_tt = self.confirm_tt - 1
            end,function ()
                self:OnClickConfirm()
                ViewMgr:CloseView(DialogTipsView)
            end, self.confirm_tt, 1)
        
        --强制等待时间
        elseif info.confirm.wait_time then
            if self.confirm_wait_handle ~= nil then
                TimeHelper:CancelTimer(self.confirm_wait_handle)
            end
            self.confirm_count_down = info.confirm.wait_time
            self.ConfirmInteractor.Interactable = false
            self.confirm_wait_handle = TimeHelper:AddCountDownTT(function ()
                UH.SetText(self.ConfirmName, (info_confirm.name or self.reply_type.confirm).."(".. self.confirm_count_down..")")
                self.confirm_count_down = self.confirm_count_down - 1
            end,function ()
                self.ConfirmInteractor.Interactable = true
                UH.SetText(self.ConfirmName, info_confirm.name or self.reply_type.confirm)
            end, info.confirm.wait_time, 1, true)
        end
    end

    if info.cancel then
        if info.cancel.time then
            TimeHelper:CancelTimer(self.timer_tt_cancel)
            self.cancel_tt = math.floor(info.cancel.time)
            self.timer_tt_cancel = TimeHelper:AddCountDownTT(function ()
                self.CancelName.text = string.format("%s(%s)", info_cancel.name or self.reply_type.cancel, self.cancel_tt)
                self.cancel_tt = self.cancel_tt - 1
            end,function ()
                ViewMgr:CloseView(DialogTipsView)
            end, self.cancel_tt, 1)
        elseif info.cancel.wait_time then
            if self.cancel_wait_handle ~= nil then
                TimeHelper:CancelTimer(self.cancel_wait_handle)
            end
            self.cancel_count_down = info.cancel.wait_time
            self.CancelInteractor.Interactable = false
            self.cancel_wait_handle = TimeHelper:AddCountDownTT(function ()
                UH.SetText(self.CancelName, (info_cancel.name or self.reply_type.cancel).."(".. self.cancel_count_down..")")
                self.cancel_count_down = self.cancel_count_down - 1
            end,function ()
                UH.SetText(self.CancelName, info_cancel.name or self.reply_type.cancel)
                self.CancelInteractor.Interactable = true
            end, info.cancel.wait_time, 1, true)
        end
    end
    self.RichText:SetObjActive(nil ~= info.rich_text)
    self.rich_text_fun = info.rich_text_fun
    if info.rich_text then
        UH.SetText(self.RichText, info.rich_text)
    end
end

function DialogTipsView:OnClickCancel()
    if not self.CancelInteractor.Interactable then
        return
    end
    if self.cancel_func ~= nil and type(self.cancel_func) == "function" then
        self.cancel_func()
    else
        ViewMgr:CloseView(DialogTipsView)
    end
end

function DialogTipsView:OnClickConfirm()
    if not self.ConfirmInteractor.Interactable then
        return
    end
    if nil ~= self.new_tnr then
        if self.TodayNotRemind.isOn then
            DialogTipsData.Instance:SetTodayRemind(self.new_tnr)
        end
    elseif nil ~= self.tnr then
        self.data.today_not_reminds[self.tnr] = self.TodayNotRemind.isOn
    end
    if self.confirm_func ~= nil and type(self.confirm_func) == "function" then
        self.confirm_func()
    end
    if self.content_func ~= nil and type(self.content_func) == "function" then
        self.content_func()
        ViewMgr:CloseView(DialogTipsView)
    end
end

function DialogTipsView:SetRemain(type)
    if type ~= nil then
        local name = Language.Common.HuoBi[type or 0]
        local num = RoleData.Instance:GetCurrency(type or 0)
        UH.SetGold(self.RemainIcon,type or 0)
        UH.SetText(self.RemainDesc,Format(Language.PublicPopup.Remain,name,num))
    end
    self.Remain:SetActive(type ~= nil)
end

function DialogTipsView:OnClickClose()
    ViewMgr:CloseView(DialogTipsView)
end

function DialogTipsView:OnRichTextClick()
    if nil ~= self.rich_text_fun and "function" == type(self.rich_text_fun) then
        self.rich_text_fun()
    end
end