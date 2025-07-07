SocietyViewSworn = SocietyViewSworn or DeclareMono("SocietyViewSworn", UIWFlushPanel)
function SocietyViewSworn:SocietyViewSworn()
    self.data_cares = {
        {data = SwornData.Instance.sworn_view_tab, func = self.ChangeTab, init_call = false}
    }
end

function SocietyViewSworn:OnEnable()
    local is_sworn = SwornData.Instance:IsHaveSworn()
    self.SwornCreateObj:SetActive(not is_sworn)
    self.SwornMainObj:SetActive(is_sworn)
end

function SocietyViewSworn:ChangeTab()
    local index = SwornData.Instance.sworn_view_tab.index or 1
    if index == 2 and not SwornData.Instance:IsHaveSworn() then
        index = 1
    end
    self.SwornCreateObj:SetActive(index == 1)
    self.SwornMainObj:SetActive(index == 2)
end

--结义说明创建界面
SwornCreatePanel = SwornCreatePanel or DeclareMono("SwornCreatePanel", UIWFlushPanel)
function SwornCreatePanel:OnClickToggle(param)
end

----------结义说明
SwornCreateExplainPanel = SwornCreateExplainPanel or DeclareMono("SwornCreateExplainPanel", UIWFlushPanel)
function SwornCreateExplainPanel:Awake()
    UIWFlushPanel.Awake(self)
    for i = 1, 3 do
        UH.SetText(self.DescTexts[i], Config.language_cfg_auto.textdesc[170 + i].desc)
    end
end

function SwornCreateExplainPanel:OnClickRule()
    PublicPopupCtrl.Instance:HelpTip(174)
end

function SwornCreateExplainPanel:OnClickInvite()
    SwornCtrl.Instance:OpenInviteFriendView()
end

function SwornCreateExplainPanel:OnClickGoTo()
    SceneLogic.Instance:AutoToNpc(SwornData.Instance:GetSwornNpcSeq(), nil, false, true)
    ViewMgr:CloseView(SocietyView)
end

----------自荐宣言
SwornCreateResumePanel = SwornCreateResumePanel or DeclareMono("SwornCreateResumePanel", UIWFlushPanel)
function SwornCreateResumePanel:Awake()
    self.data_cares = {
        {data = SwornData.Instance.sworn_info, func = self.FlushView, init_call = true},
        {data = SwornData.Instance.sworn_role_decl_list, func = self.FlushView, init_call = false}
    }
    UIWFlushPanel.Awake(self)
end

function SwornCreateResumePanel:FlushView()
    local show_list = SwornData.Instance.sworn_role_decl_list.list or {}
    self.List:SetData(show_list)
    self.EmptyObj:SetActive(#show_list == 0)
end

function SwornCreateResumePanel:OnClickEdit()
    if SwornData.Instance:IsHaveSworn() then
        PublicPopupCtrl.Instance:Center(Language.Sworn.ResumeError1)
        return
    end
    ViewMgr:OpenView(SwornEditDeclView, {decl_type = 1})
end

function SwornCreateResumePanel:OnClickApply()
    local select_data = self.List:GetSelectedData()[1]
    if select_data == nil then
        PublicPopupCtrl.Instance:Center(Language.Sworn.ResumeError2)
        return
    end
    local is_friend = SocietyData.Instance:IsFriend(select_data.uid)
    if is_friend then
        PublicPopupCtrl.Instance:Center(Language.Sworn.AddFriendError2)
        return
    end
    SocietyCtrl.Instance:SendFriendAdd(select_data.uid)
end

SwornCreateResumeItem = SwornCreateResumeItem or DeclareMono("SwornCreateResumeItem", UIWidgetBaseItem)
function SwornCreateResumeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local prof_sp = RoleData.GetProfSp(data.prof)
    UH.SpriteName(self.ProfI, prof_sp)
    self.ProfI.gameObject:SetActive(nil ~= prof_sp)
    UH.SetText(self.Name, data.name)
    self.SwornAvatar:SetData({avatar_type = data.avatar_type, avatar_id = data.avatar_id, avatar_quality = data.avatar_quality,level = data.level})
    UH.SetText(self.Family, data.family)
    UH.SetText(self.Content, Format(Language.Sworn.SwornPrefix, data.text))

    local is_self = data.uid == RoleData.Instance:GetRoleId()
    self.Selector:SetObjActive(not is_self)
    -- self.RecoObj:SetActive(not is_self)
end

function SwornCreateResumeItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function SwornCreateResumeItem:OnSelect(selected)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    UIWidgetBaseItem.OnSelect(self, selected)
end

function SwornCreateResumeItem:OnClickIcon()
end

----------纳新宣言
SwornCreateInvitePanel = SwornCreateInvitePanel or DeclareMono("SwornCreateInvitePanel", UIWFlushPanel)
function SwornCreateInvitePanel:Awake()
    self.data_cares = {
        {data = SwornData.Instance.sworn_info, func = self.FlushView, init_call = true},
        {data = SwornData.Instance.sworn_team_decl_list, func = self.FlushView, init_call = false}
    }
    UIWFlushPanel.Awake(self)
end

function SwornCreateInvitePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    SwornCtrl.Instance.watch_jie_yi_zu_id = nil
    SwornCtrl.Instance.add_friend_zu_id = nil
    --LogError("销毁了。")
end

function SwornCreateInvitePanel:FlushView()
    local show_list = SwornData.Instance.sworn_team_decl_list.list or {}
    self.List:SetData(show_list)
    self.EmptyObj:SetActive(#show_list == 0)
end

function SwornCreateInvitePanel:OnClickEdit()
    if not SwornData.Instance:IsHaveSworn() then
        PublicPopupCtrl.Instance:Center(Language.Sworn.InviteError3)
        return
    elseif not SwornData.Instance:IsSwornLeader() then
        PublicPopupCtrl.Instance:Center(Language.Sworn.InviteError2)
        return
    end
    ViewMgr:OpenView(SwornEditDeclView, {decl_type = 2})
end

function SwornCreateInvitePanel:OnClickApply()
    local select_data = self.List:GetSelectedData()[1]
    if select_data == nil then
        PublicPopupCtrl.Instance:Center(Language.Sworn.InviteError1)
        return
    end
    SwornCtrl.Instance:SendAddFriend(select_data.jie_yi_zu_id)
end

SwornCreateInviteItem = SwornCreateInviteItem or DeclareMono("SwornCreateInviteItem", UIWidgetBaseItem)
function SwornCreateInviteItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local name = data.zu_name.sworn_name
    local member_num = data.zu_name.member_num
    UH.SetText(self.Name, Format(Language.Sworn.SwornInvitePrifx, name, member_num))
    UH.SetText(self.Content, Format(Language.Sworn.SwornPrefix, data.text))
end

function SwornCreateInviteItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function SwornCreateInviteItem:OnSelect(selected)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    UIWidgetBaseItem.OnSelect(self, selected)
end

function SwornCreateInviteItem:OnClickWatch()
    SwornCtrl.Instance:SendWatchTeam(self.data.jie_yi_zu_id)
end
