SwornKickOutView = SwornKickOutView or DeclareView("SwornKickOutView", SwornConfig.ResPath .. "sworn_kick_out")
function SwornKickOutView:LoadCallback()
    self.Board:SetData(self:GetType(), Language.Sworn.KickOut, Vector2.New(810, 421))
end

function SwornKickOutView:OnClickClose()
    ViewMgr:CloseView(SwornKickOutView)
end

SwornKickOutPanel = SwornKickOutPanel or DeclareMono("SwornKickOutPanel", UIWFlushPanel)
function SwornKickOutPanel:SwornKickOutPanel()
    self.data_cares = {
        {data = SwornData.Instance.sworn_info, func = self.FlushView, self}
    }
end

function SwornKickOutPanel:FlushView()
    local show_list = SwornData.Instance:GetCanKickOutList()
    if #show_list > 0 then
        self.List:SetData(show_list)
        self.List:SetectItem(1, true)
    else
        ViewMgr:CloseView(SwornKickOutView)
    end
end

function SwornKickOutPanel:OnClickKickOut()
    local select_data = self.List:GetSelectedData()[1]
    if select_data == nil then
        return
    end
    local show_list = SwornData.Instance:GetCanKickOutList()
    if #show_list <= 1 then
        PublicPopupCtrl.Instance:Center(Language.Sworn.KickOutError2)
        return
    end
    local cooling_period_time = SwornConfig.CAN_KICK_OUT_TIME
    if select_data.online_state == ONLINE_STATUS_TYPE.TYOE_OFFLINE and select_data.last_login_time < (TimeHelper.GetServerTime() - cooling_period_time) then
        SwornCtrl.Instance:SendKickOut(select_data.uid)
        return
    else
        PublicPopupCtrl.Instance:Center(Language.Sworn.KickOutError1)
    end
end

SwornKickOutMemberItem = SwornKickOutMemberItem or DeclareMono("SwornKickOutMemberItem", UIWidgetBaseItem)
function SwornKickOutMemberItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local prof_sp = RoleData.GetProfSp(data.prof)
    UH.SpriteName(self.ProfI, prof_sp)
    self.ProfI.gameObject:SetActive(nil ~= prof_sp)
    UH.SetText(self.Name, data.name)
    self.SwornAvatar:SetData({avatar_type = data.avatar_type, avatar_id = data.avatar_id,avatar_quality = data.avatar_quality, level = data.level})

    UH.SetText(self.IntimacyText, SocietyData.Instance:GetFriendIntimacyVal(data.uid))
end
