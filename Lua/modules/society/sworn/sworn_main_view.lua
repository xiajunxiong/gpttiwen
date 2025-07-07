--结义主界面
SwornMainPanel = SwornMainPanel or DeclareMono("SwornMainPanel", UIWFlushPanel)
function SwornMainPanel:SwornMainPanel()
    self.data_cares = {
        {data = SwornData.Instance.sworn_info, func = self.FlushView}
    }
end

function SwornMainPanel:FlushView()
    if not SwornData.Instance:IsHaveSworn() then
        ViewMgr:CloseView(SwornAdminBtnsView)
        SwornData.Instance.sworn_view_tab.index = 1
        SwornData.Instance.sworn_view_tab:Notify()
        return
    end
    local member_list = SwornData.Instance:GetMemberList()
    if SwornData.Instance:IsTanHe() then
        table.sort(member_list, function (a,b)
            return a.is_leader < b.is_leader
        end)
    end
    self.MemberList:SetData(member_list)
    local sworn_info = SwornData.Instance.sworn_info
    local time_t = os.date("*t", sworn_info.create_time)
    UH.SetText(self.SwornTime, Format(Language.Sworn.SwornTime2, time_t.year, time_t.month, time_t.day))
    UH.SetText(self.DeclText, sworn_info.jie_yi_text)
    UH.SetText(self.TitleText, SwornData.Instance:GetSwornName())

    local is_leader = SwornData.Instance:IsSwornLeader()
    self.BtnEditDecl:SetActive(is_leader)
    self.BtnEditTitle:SetActive(is_leader)

    local sworn_level = SwornData.Instance:GetSwornLevel()
    local sworn_cfg = SwornData.Instance:GetLevelCfg(sworn_level)
    UH.SetText(self.LevelName, sworn_cfg.level_name)
    self.SkillList:SetData(SwornData.Instance:GetSkillShowList())
end

function SwornMainPanel:OnClickRule()
    PublicPopupCtrl.Instance:HelpTip(174)
end

function SwornMainPanel:OnClickAdmin()
    ViewMgr:OpenView(SwornAdminBtnsView)
end

function SwornMainPanel:OnClickEditDesc()
    ViewMgr:OpenView(SwornNoticeView)
end

function SwornMainPanel:OnClickEditTitle()
    ViewMgr:OpenView(SwornRenameTeamView)
end

function SwornMainPanel:OnClickSkillTip()
    ViewMgr:OpenView(SwornLevelTip)
end

SwornMainMemberItem = SwornMainMemberItem or DeclareMono("SwornMainMemberItem", UIWidgetBaseItem)
function SwornMainMemberItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local appearance = data.appearance
    local avatar_data = {
        avatar_type = data.avatar_type,
        avatar_id = data.avatar_id,
        avatar_quality = data.avatar_quality,
        level = data.level
    }
    self.SwornAvatar:SetData(avatar_data)

    UH.SetText(self.Name, data.name)
    local prof_sp = RoleData.GetProfSp(data.prof)
    UH.SpriteName(self.ProfI, prof_sp)
    self.ProfI.gameObject:SetActive(nil ~= prof_sp)

    if data.has_first_member_name == 0 then
        UH.SetText(self.TitleText, Format(Language.Sworn.SwornNameNull, SwornData.Instance:GetSwornName()))
    else
        UH.SetText(self.TitleText, Format(Language.Sworn.SwornNameHave, SwornData.Instance:GetSwornName(), data.member_name))
    end
    self.LeaderObj:SetActive(data.is_leader == 1)
    local is_self = data.uid == RoleData.Instance:GetRoleId()
    self.BtnTitle:SetActive(is_self)
    self.IntimacyObj:SetActive(not is_self)
    UH.SetText(self.IntimacyText, SocietyData.Instance:GetFriendIntimacyVal(data.uid))
    if data.online_state == ONLINE_STATUS_TYPE.TYOE_OFFLINE then
        UH.SetText(self.AgoText, TimeHelper.Ago(data.last_login_time))
        UH.Color(self.AgoText, COLORS.Gray2)
    else
        UH.SetText(self.AgoText, Language.Society.RoleOnline)
        UH.Color(self.AgoText, COLORS.Green3)
    end

    local my_info = SwornData.Instance:MyInfo()
    self.VoteBtn:SetActive(SwornData.Instance:IsTanHe() and my_info.is_vote == 0 and data.is_leader ~= 1)
    self.VoteNum:SetObjActive(SwornData.Instance:IsTanHe() and data.is_leader ~= 1)
    if SwornData.Instance:IsTanHe() and data.is_leader ~= 1 then
        local num = data.votes_num or 0
        UH.SetText(self.VoteNum, string.format(Language.Sworn.VoteNum, num))
    end
end

function SwornMainMemberItem:OnVoteClick()
    PublicPopupCtrl.Instance:AlertTip(Language.Sworn.Text9,function ()
        SwornCtrl.Instance:SendSwornReq(SwornConfig.JIE_YI_REQ_RET_TYPE.TYPE_TAN_HE_VOTE, self.data.uid)
    end)
end

function SwornMainMemberItem:OnClickEditTitle()
    ViewMgr:OpenView(SwornRenameRoleView)
end

function SwornMainMemberItem:OnClickRole()
    local vo = self.data
	if vo.uid == RoleData.Instance:GetRoleId() then return end
    local info = {
        uid = vo.uid,
        name = vo.name,
        level = vo.level,
        prof = vo.prof,
		family = nil,
		avatar_type = vo.avatar_type,
		avatar_id = vo.avatar_id,
		top_level = nil,
		kill_value = nil
    }
	local config = DataHelper.TableCopy(SocietyConfig.SwornMemberFunctions)
	local is_friend = SocietyCtrl.Instance:Data():IsFriend(info.uid)
	table.insert(config, 3, is_friend and "DeleteFriend" or "AddFriend")

    RoleInfoCtrl.Instance:RoleInfoView{
		info = function ()
				return RoleInfoData.RoleInfoNew{func_info = info, uid = info.uid, name = info.name, level = info.level, prof = info.prof, family = info.family, avatar_type = info.avatar_type, avatar_id = info.avatar_id, top_level = info.top_level}
			end,
		funcs = function()
			return config
		end
	}
end

SwornMainSkillItem = SwornMainSkillItem or DeclareMono("SwornMainSkillItem", UIWidgetBaseItem)
function SwornMainSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.Icon, data.icon_id, ICON_TYPE.SKILL)
    UH.SetText(self.Name, "")
    local sworn_level = SwornData.Instance:GetSwornLevel()
    UH.SetInter(self.BtnInter, sworn_level >= data.friendship_level)
end

function SwornMainSkillItem:OnClickSkill()
    ViewMgr:OpenView(SwornSkillTip, {skill_cfg = self.data})
end

SwornAdminBtnsView = SwornAdminBtnsView or DeclareView("SwornAdminBtnsView", SwornConfig.ResPath .. "sworn_admin_btns")
VIEW_DECLARE_MASK(SwornAdminBtnsView, ViewMask.BlockClose)
function SwornAdminBtnsView:SwornAdminBtnsView()
    UH.SetInter(self.TanHeInter, not SwornData.Instance:IsSwornLeader())
end

function SwornAdminBtnsView:OnClickAdminAdd()
    if not SwornData.Instance:IsSwornLeader() then
        PublicPopupCtrl.Instance:Center(Language.Sworn.SwornAdminError1)
        return
    end
    ViewMgr:CloseView(SwornAdminBtnsView)
    ViewMgr:OpenView(SwornInviteView)
end

function SwornAdminBtnsView:OnClickAdminKickOut()
    if not SwornData.Instance:IsSwornLeader() then
        PublicPopupCtrl.Instance:Center(Language.Sworn.SwornAdminError1)
        return
    end
    ViewMgr:CloseView(SwornAdminBtnsView)
    ViewMgr:OpenView(SwornKickOutView)
end

function SwornAdminBtnsView:OnClickAdminQuit()
    ViewMgr:CloseView(SwornAdminBtnsView)
    ViewMgr:OpenView(SwornQuitView)
end

function SwornAdminBtnsView:OnClickAdminBack()
    ViewMgr:CloseView(SwornAdminBtnsView)
    SwornData.Instance.sworn_view_tab.index = 1
    SwornData.Instance.sworn_view_tab:Notify()
end

function SwornAdminBtnsView:OnTanHeClick()
    if not SwornData.Instance:IsSwornLeader() then
        ViewMgr:OpenView(SwornDetailView)
    else
        PublicPopupCtrl.Instance:Error(Language.Sworn.Text8)
    end
end
