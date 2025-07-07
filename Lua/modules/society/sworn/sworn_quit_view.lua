SwornQuitView = SwornQuitView or DeclareView("SwornQuitView", SwornConfig.ResPath .. "sworn_quit")
function SwornQuitView:LoadCallback()
    self.Alert:SetData(self:GetType(), Vector2.New(466, 317))
    self.ContentTimer:SetShowCallBack(function(t)
        return Format(Language.Sworn.QuitTips2, t.hour, t.min, t.s)
    end)
    self.ContentTimer:SetCallBack(function()
        ViewMgr:CloseView(SwornQuitView)
    end)
    self:FlushView()
end

function SwornQuitView:Awake()
    if self.handle_info == nil then
        self.handle_info = SwornData.Instance.sworn_info:Care(BindTool.Bind1(self.FlushView, self))
    end
end

function SwornQuitView:OnDestroy()
    if self.handle_info ~= nil then
        SwornData.Instance.sworn_info:Uncare(self.handle_info)
        self.handle_info = nil
    end
end

function SwornQuitView:FlushView()
    local info = SwornData.Instance:GetSelfInfo()
    local end_timestamp = info.level_end_time
    local is_exiting = end_timestamp > TimeHelper.GetServerTime()
    self.BtnApply:SetActive(not is_exiting)
    self.BtnCancel:SetActive(not is_exiting)
    self.BtnCancelApply:SetActive(is_exiting)

    if is_exiting then
        self.ContentTimer:StampTime(end_timestamp)
    else
        self.ContentTimer:CloseCountDownTime()
        self.ContentTimer:SetTime(Language.Sworn.QuitTips1)
    end
end

function SwornQuitView:OnClickClose()
    ViewMgr:CloseView(SwornQuitView)
end

function SwornQuitView:OnClickAffirmApply()
    SwornCtrl.Instance:SendQuitReq(1)
end

function SwornQuitView:OnClickCancelApply()
    SwornCtrl.Instance:SendQuitReq(0)
end
