LoversMatchAlertView = LoversMatchAlertView or DeclareView("LoversMatchAlertView", "period_activity_act/lovers_match/lovers_match_alert")
VIEW_DECLARE_MASK(LoversMatchAlertView, ViewMask.None)

function LoversMatchAlertView:LoadCallback(param_t)
    self.Timer:SetCallBack(BindTool.Bind1(self.OnTimeOver, self))
    self.info = param_t.info
    local point_cfg = param_t.point_cfg
    local add_text = Format(Language.LoversMatch.AddFriend, self.info.name, point_cfg.friend_name)
    self.Timer:TotalTime(15, TimeType.Type_Special_5, add_text .. Language.LoversMatch.AddFriendCount)
end

function LoversMatchAlertView:CloseCallback()
end

function LoversMatchAlertView:OnClickClose()
    ViewMgr:CloseView(LoversMatchAlertView)
end

function LoversMatchAlertView:OnTimeOver()
    ViewMgr:CloseView(LoversMatchAlertView)
end

function LoversMatchAlertView:OnClickAdd()
    SocietyCtrl.Instance:SendFriendAdd(self.info.uid)
    ViewMgr:CloseView(LoversMatchAlertView)
end
