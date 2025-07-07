MarryFancyView = MarryFancyView or DeclareView("MarryFancyView", "marry/marry_fancy")

function MarryFancyView:MarryFancyView()
    self.Board:SetData(self:GetType(), Language.Marry.PopupViewTitle)
end

function MarryFancyView:LoadCallback(param_t)
    local list = SocietyCtrl.Instance:Data():GetFriendListFriendTeamInvite()
    self:OnClickItem(list[1])
end

function MarryFancyView:OnClickItem(data)
    if data and data.info then
        UH.SetText(self.Name,data.info.name)
    end
    self.select_data = data
    self.Panel:SetActive(false)
end

function MarryFancyView:OnClickParent()
    local list = SocietyCtrl.Instance:Data():GetFriendListFriendTeamInvite()
    if IsEmpty(list) then
        PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[6])
        return
    end
    self.Panel:SetActive(not GetActive(self.Panel))
    self.List:SetData(list)
end

function MarryFancyView:OnClickCancel()
    self:OnCrossSever()
end

function MarryFancyView:OnClickConfirm()
    if not self.select_data then
        PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[2])
        return
    end
    TeamCtrl.Instance:OnSendCreateTeam(function()
        TeamCtrl.Instance:SendTeamInvitationReq(self.select_data.info.uid,2)
        self:OnCrossSever()
    end)
end

function MarryFancyView:OnCrossSever()
    SceneLogic.Instance:AutoToNpcs(MarryData.Instance:GetNpcId())
    ViewMgr:CloseView(MarryFancyView)
    ViewMgr:CloseView(SocietyView)
end

----------------------------MarryFancyItem----------------------------
MarryFancyItem = MarryFancyItem or DeclareMono("MarryFancyItem", UIWidgetBaseItem)

function MarryFancyItem:SetData(data)
    UH.SetText(self.Name,data.info.name)
    UIWidgetBaseItem.SetData(self, data)
end