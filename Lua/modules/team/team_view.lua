TeamView = TeamView or DeclareView("TeamView", TeamConfig.ResPath .. "team", Mod.Team.Main)

function TeamView:TeamView()
    self.data = TeamCtrl.Instance:Data()
    self.language = Language.Team
end

function TeamView:LoadCallback()
end

TeamViewUp = TeamViewUp or DeclareMono("TeamViewUp", UIWFlushPanel)

function TeamViewUp:TeamViewUp()
    self.data = TeamCtrl.Instance:Data()
    self.language = Language.Team

    self.data_cares = {
		{data = self.data:GetTeamInfo(), func = self.onFlush},
        {data = self.data:IsAutoMatch(), func = self.AutoMatching},
        {data = self.data.notice_time, func = self.FlushNotice}
	}
end

function TeamViewUp:onFlush()
    local in_team = self.data:InTeam()
    
    self.DetailObj:SetActive(in_team)
    self.BgSetter:SetByPath(string.format(TeamConfig.PngPath, in_team and TeamConfig.BgPng.in_team or TeamConfig.BgPng.no_team))

    if not in_team then return end

    local is_leader = self.data:IsLeader()
    local is_auto_match = self.data:IsAutoMatch()
    local is_cross = self.data:IsCross()

    local team_detail = self.language.TeamDetail
    self.Goal.text = RichTextHelper.ColorStr(team_detail.goal .. "：", COLORSTR.Red7) .. self.data.real_goal_goal.desc
    self.Level.text = RichTextHelper.ColorStr(team_detail.level .. "：", COLORSTR.Red7) .. self.data.real_goal_level_low .. "-" .. self.data.real_goal_level_high
    self.Mode.text = RichTextHelper.ColorStr(team_detail.mode .. "：", COLORSTR.Red7) .. (is_cross and self.language.ModeType.kua_fu or self.language.ModeType.ben_fu)

    for i = 1, self.LeaderObjs:Length() do
        self.LeaderObjs[i]:SetActive(is_leader)
    end
end

function TeamViewUp:AutoMatching()
    TimeHelper:CancelTimer(self.timer_ct)
    local is_auto_match = self.data:IsAutoMatch()
    self.MatchingObj:SetActive(is_auto_match.value)
    self.BtnMatch.text = is_auto_match.value and Language.Team.TeamBtnName.cancel_match or Language.Team.TeamBtnName.match
    self.point_a = self.point_a or 1
    if is_auto_match.value then
        self.timer_ct = TimeHelper:AddRunTimer(function ()
            self.Points[self.point_a].SpriteName = TeamConfig.Dians.HongDian
            self.point_a = self.point_a + 1 > 3 and 1 or self.point_a + 1
            self.Points[self.point_a].SpriteName = TeamConfig.Dians.BaiDian
        end, 0.5)
    end
end

function TeamViewUp:FlushNotice(value)
    if 0 == self.data.notice_time.val or ChatData.Instance:GetTeamInvitationCd() == self.data.notice_time.val or nil == value then
        local is_enabled = 0 == self.data.notice_time.val
        is_enabled = is_enabled and SCENE_TYPE.COMMON_SCENE == SceneData.Instance:GetMode()
        self.BtnNoticeInter.Interactable = is_enabled
        self.BtnNoticeSp.SpriteName = is_enabled and "YiJianHanHuaTuBiao" or "JinZhiHanHua"
    end
    self.BtnNoticeTime.text = self.data.notice_time.val > 0 and string.format(Language.Team.NoticeTimer, self.data.notice_time.val) or ""
end

function TeamViewUp:OnClickClose()
    ViewMgr:CloseView(TeamView)
end

function TeamViewUp:OnClickGoal()
    local is_leader = self.data:IsLeader()
    if not is_leader then return end
    ViewMgr:OpenView(TeamGoalView)
end

function TeamViewUp:OnClickNotice()
    self.data:NoticeTimer()
    TeamCtrl.Instance:SendTeamRecruit()
end

function TeamViewUp:OnClickApprove()
    ViewMgr:OpenView(TeamViewApprove)
end

function TeamViewUp:OnClickMatch()
    TeamCtrl.Instance:SendTeamMatch()
end

function TeamViewUp:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_ct)
end

TeamViewMid = TeamViewMid or DeclareMono("TeamViewMid", UIWFlushPanel)

function TeamViewMid:TeamViewMid()
    self.data = TeamCtrl.Instance:Data()
    self.data_cares = {
		{data = self.data:GetTeamInfo(), func = self.onFlush},
	}
end

function TeamViewMid:onFlush()
    local team_info = self.data:GetTeamInfo()
    local member_list = team_info.member_list
    if not self.data:InTeam() then
        self.data:CheckNoTeamPartnerOrder()
    end
    -- self.ShowList:SetData(member_list)
    for i = 1, 5 do
        self.ShowItems1[i]:SetData(member_list[i])
        self.ShowItems2[i]:SetData(member_list[i])
    end
end

TeamViewMidShowItem = DeclareMono("TeamViewMidShowItem", UIWidgetBaseItem)

function TeamViewMidShowItem:TeamViewMidShowItem()
    self.team_data = TeamCtrl.Instance:Data()
end

function TeamViewMidShowItem:OnClickItem()
    local in_team = self.team_data:InTeam()
    local partner_list = self.team_data:GetTeamPartnerList()
    local is_leader = self.team_data:IsLeader()

    local pos = self.ItemPosition.position
    local offset = self.ItemPosition:TransformVector(Vector3.New(200, 0, 0))
    self.team_data.info_position = self.data.index > 3 and Vector3.New(pos.x - offset.x, pos.y, pos.z) or Vector3.New(pos.x + offset.x, pos.y, pos.z)

    if self.data.info then
        if RoleData.Instance:GetBaseData().role_id ~= self.data.info.uid then
            if not in_team or is_leader or self.data.info.is_partner == 0 then
                if self.data.info.appearance and self.data.info.appearance.special_appearance_type == 1 then
                    return
                end
                self.team_data.team_oper_info = self.data
                ViewMgr:OpenView(TeamOperInfoView)
            end
        end
    else
        ViewMgr:OpenView(TeamInviteView)
    end
end


TeamViewMidShowItem1 = DeclareMono("TeamViewMidShowItem1", TeamViewMidShowItem)

function TeamViewMidShowItem1:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = data.info
    local in_team = self.team_data:InTeam()
    local is_leader = in_team and data.is_leader
    local member_type = Language.Team.MemberType
    local role_id = RoleData.Instance:GetBaseData().role_id

    local bg_sp = TeamConfig.TeamMemberBgSp.DuiYou
    if nil ~= info then
        local type, prof, level, name, prof_name
        if info.is_partner == 1 then
            type = member_type.partner

            local partner_info = info.partner_info
            prof = partner_info.job
            name = partner_info.name
            prof_name = Language.Common.Prof[prof]

            local leader_info = self.team_data:GetTeamInfo().leader
            -- level = leader_info.info.level
            level = info.level

            bg_sp = TeamConfig.TeamMemberBgSp.HuoBan
        else
            type = is_leader and member_type.leader or member_type.member
            type = role_id == info.uid and member_type.myself or type
            prof = RoleData.ProfIdToProfType(info.prof)
            prof_name = RolePromoteData.GetProfName(info.prof)
            name = info.name
            level = info.level

            bg_sp = role_id == info.uid and TeamConfig.TeamMemberBgSp.Wo or TeamConfig.TeamMemberBgSp.DuiYou
        end
        local prof_sp = RoleData.GetProfSp(prof)
        self.MemberType.text = type
        self.Prof.text = prof_name
        self.ProfI.SpriteName = prof_sp
        self.Name.text = name
        self.Level.text = "Lv." .. (level or 1)
        local value = info.is_partner ~= 1 and info.is_follow == 0 and not is_leader
        self.FollowObj:SetActive(value)
        if value then
            UH.SetAvatar(self.IconSp, info.appearance.avatar_type,info.appearance.avatar_id,info.appearance.avatar_quality)
            self.FollowShow.text = RichTextHelper.ColorStr(0 == info.is_online and Language.Team.FollowState.Outline or Language.Team.FollowState.Leave, 0 == info.is_online and COLORSTR.Yellow13 or COLORSTR.Green13)
		    self.FollowSp.SpriteName = 0 == info.is_online and TeamConfig.TeamStateSp.outline or TeamConfig.TeamStateSp.leave
        end
    else
        self.FollowObj:SetActive(false)
    end

    self.BgSp.SpriteName = bg_sp
    self.LeaderObj:SetActive(is_leader)
    self.DownObj:SetActive(nil ~= info)
    for i = 1, 2 do
        self.PlusObjs[i]:SetActive(nil == info)
    end
end

TeamViewMidShowItem2 = DeclareMono("TeamViewMidShowItem2", TeamViewMidShowItem)

function TeamViewMidShowItem2:TeamViewMidShowItem2()
    self.partner_data = PartnerData.Instance
end

function TeamViewMidShowItem2:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = data.info
    if info then
        self.ModelShow:SetObjActive(true)
        local aperance = info.appearance
        local card_model_res_id = LiveData.Instance:CardModel(aperance)
        self.ModelShow:SetObjActive(card_model_res_id == nil)
        self.NPCModelShow:SetObjActive(card_model_res_id ~= nil)
        if card_model_res_id then
            self:SetNpcModel(card_model_res_id)
        else
            self:SetChModel(info)
        end
    else
        self.ModelShow:SetObjActive(false)
        self.NPCModelShow:SetObjActive(false)
    end
end

function TeamViewMidShowItem2:SetChModel(info)
    if not self.ui_obj then
        self.ui_obj = UIChDrawer.New()
        if 0 == info.is_partner then
            self.ui_obj:SetData(info.appearance)
        else
            local res_id = info.partner_info.res_id
            if info.skin_id > 0 and self.partner_data.partner_skin_ids[info.skin_id] then
                res_id = self.partner_data.partner_skin_ids[info.skin_id].res_id
            end
            self.ui_obj:SetMainSolo("actor/npc/" .. res_id ..  ".prefab")
        end
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = false,
            }
        )

        local re_data = Game.RendererEffectData()
        re_data.materialNames = {nil, "Ghost201"}
        re_data.animationClipName = "UI_jueseguang201"

        self.ui_obj:SetRenderEffect(re_data)
        self.Effect:SetActive(true)
    else
        if 0 == info.is_partner then
            self.ui_obj:SetData(info.appearance)
        else
            local res_id = info.partner_info.res_id
            if info.skin_id > 0 and self.partner_data.partner_skin_ids[info.skin_id] then
                res_id = self.partner_data.partner_skin_ids[info.skin_id].res_id
            end
            self.ui_obj:SetMainSolo("actor/npc/" .. res_id ..  ".prefab")
        end 
    end
end

function TeamViewMidShowItem2:SetNpcModel(card_model_res_id)
    if not self.npc_ui_obj then
        self.npc_ui_obj = UIObjDrawer.New()
        self.NPCModelShow:SetShowData({ui_obj = self.npc_ui_obj,view = self})
        self.Effect:SetActive(true)
    end
    self.npc_ui_obj:SetData(DrawerHelper.GetNpcPath(card_model_res_id))
end

function TeamViewMidShowItem2:OnDestroy()
    UIWidgetBaseItem.OnDestroy(self)
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
    if self.npc_ui_obj then
        self.npc_ui_obj:DeleteMe()
        self.npc_ui_obj = nil
    end
end

TeamViewDown = TeamViewDown or DeclareMono("TeamViewDown", UIWFlushPanel)

function TeamViewDown:TeamViewDown()
    self.data = TeamCtrl.Instance:Data()
    self.language = Language.Team

    self.data_cares = {
		{data = self.data:GetTeamInfo(), func = self.onFlush},
        {data = self.data.new_approve_red_point, func = self.NewApproveRedPoint },
    }
end

function TeamViewDown:onFlush()
    local in_team = self.data:InTeam()
    local is_leader = self.data:IsLeader()
    local is_follow = self.data:IsFollow()
    self.NoTeams:SetActive(not in_team)
    self.InTeams:SetActive(in_team)
    self.Leaders:SetActive(not in_team or is_leader)
    if in_team then
        self.BtnApproveObj:SetActive(is_leader)
        self.BtnTBObj:SetActive(not is_leader)
        self.BtnTB.text = is_follow and self.language.FollowState.Leave or self.language.FollowState.Follow
        self.BtnAllowRedJoinObj:SetActive(is_leader)
        self.BtnAllowRedJoinToggle:SetActive(not self.data:GetTeamInfo():IsAllowRedJoin())
    end
end

function TeamViewDown:NewApproveRedPoint()
    self.new_approve_point = self.new_approve_point or UiRedPoint.New(self.BtnApproveObj, Vector3.New(25,25,0))
	self.new_approve_point:SetNum(self.data.new_approve_red_point.val and 1 or 0)
end

function TeamViewDown:OnClickFormation()
    if FunOpen.Instance:GetFunIsOpened(Mod.Formation.Main) then
        ViewMgr:OpenView(FormationView)
    else
        local level = FunOpen.Instance:GetFunOpenLevel(Mod.Formation.Main)
        PublicPopupCtrl.Instance:Center(string.format(Language.Formation.OpenTips, level))
    end
end

function TeamViewDown:OnClickInvite()
    -- ViewMgr:OpenView(TeamViewInvite)
end

function TeamViewDown:OnClickTB()
    TeamCtrl.Instance:SendTemporarilyPartReq()
end

function TeamViewDown:OnClickCL()
    local in_team = self.data:InTeam()
    if in_team then
        TeamCtrl.Instance:ExitTeam()
    else
        TeamCtrl.Instance:SendCreateTeam()
    end
end

function TeamViewDown:OnClickPlatformTpl()
    ViewMgr:CloseView(TeamView)
    ViewMgr:OpenView(TeamPlatformView)
end

function TeamViewDown:OnClickKnightly()
    ViewMgr:OpenView(TeamKnightlyView)
end

function TeamViewDown:OnClickSalary()
    ViewMgr:OpenView(TeamSalaryView)
end

function TeamViewDown:OnClickAllowRedJoin()
    --勾选禁止，反向判断
    local is_allow = self.data:GetTeamInfo():IsAllowRedJoin() and 0 or 1
    TeamCtrl.Instance:SendSetAllowRedJoin(is_allow)
end
