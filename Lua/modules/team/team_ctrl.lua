
FILE_REQ("modules/team/team_config")
FILE_REQ("modules/team/team_info")
FILE_REQ("modules/team/team_member_info")
FILE_REQ("modules/team/team_data")
VIEW_REQ("modules/team/team_view")
VIEW_REQ("modules/team/team_menu_view")
VIEW_REQ("modules/team/heal_menu_view")
VIEW_REQ("modules/team/team_goal_view")
VIEW_REQ("modules/team/team_platform_view")
VIEW_REQ("modules/team/team_view_approve")
-- VIEW_REQ("modules/team/team_view_invite")
VIEW_REQ("modules/team/team_view_notice")
VIEW_REQ("modules/team/team_knightly_view")
VIEW_REQ("modules/team/team_salary_view")
VIEW_REQ("modules/team/team_invite_view")
VIEW_REQ("modules/team/team_invited_view")
VIEW_REQ("modules/team/team_sort_view")
VIEW_REQ("modules/item_info/team_oper_info_view")
-- VIEW_REQ("modules/item_info/team_partner_info_view")

TeamCtrl = TeamCtrl or BaseClass(BaseCtrl)

function TeamCtrl:__init()
	if TeamCtrl.Instance ~= nil then
		Debuger.LogError("[TeamCtrl] attempt to create singleton twice!")
		return
	end	
	TeamCtrl.Instance = self
	self.data = TeamData.New()

	self:RegisterAllProtocols()
	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))
end

function TeamCtrl:__delete()
	if TeamCtrl.Instance == self then
		TeamCtrl.Instance = nil
	end

	EventSys:UnBind(self.complete_handle)
	self:UnRegisterAllProtocols()
end

function TeamCtrl:OnInit()
	RemindCtrl.Instance:Remind():Register(Mod.Team.Approve, self.data.new_approve_red_point, function ()
		return (self.data.new_approve_red_point.val and 1 or 0)
	end)
end

function TeamCtrl:OnUnloadGameLogic()
	self.data:ClearData()
	if self.guide_dragon then
		self.guide_dragon:DeleteMe()
		self.guide_dragon = nil
	end
end

function TeamCtrl:RoleNoticeComplete()
	self.data.team_invited_list:Set{}
end

function TeamCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSCreateTeam)
	self:RegisterProtocol(CSDismissTeam)
	self:RegisterProtocol(CSUnfullTeamListByTypeReq)
	self:RegisterProtocol(CSInviteJoinReq)
	self:RegisterProtocol(CSExitTeamReq)
	self:RegisterProtocol(CSKickOutOfTeamReq)
	self:RegisterProtocol(CSJoinTeamReq)
	self:RegisterProtocol(CSJoinTeamReqRet)
	self:RegisterProtocol(CSInviteJoinRet)
	self:RegisterProtocol(CSChangeTeamLeader)
	self:RegisterProtocol(CSChangeMustCheck)
	self:RegisterProtocol(CSTeamMatchOp)
	self:RegisterProtocol(CSChangeTeamLimit)
	self:RegisterProtocol(CSTemporarilyPartReq)
	self:RegisterProtocol(CSTeamInvitationReq)
	self:RegisterProtocol(CSTeamInvationRet)
	self:RegisterProtocol(CSTeamRecruit)
	self:RegisterProtocol(CSTeamEnterFbRet)
	self:RegisterProtocol(CSTeamSwitchToCross)
	self:RegisterProtocol(CSTeamSwitchPosition)
	self:RegisterProtocol(CSRoleSalaryInfo)
	self:RegisterProtocol(CSTeamLaunchLeaderVote)
	self:RegisterProtocol(CSTeamVote)
	self:RegisterProtocol(CSTeamLeaderNpcReq)
	self:RegisterProtocol(CSUseGoldRecovery)
	self:RegisterProtocol(CSTeamLeaderSummonMember)
	self:RegisterProtocol(CSTeamMemberApplyLeaderSummon)
	self:RegisterProtocol(CSLeaderCheckMemberItem)
	self:RegisterProtocol(CSReqJoinTeamInfo)

	self:RegisterProtocol(SCTeamListAck, "OnTeamListAck", true)
	self:RegisterProtocol(SCTeamInfo, "OnTeamInfo", true)
	self:RegisterProtocol(SCOutOfTeam, "OnOutOfTeam", true)
	self:RegisterProtocol(SCInviteNotice, "OnInviteNotice")
	self:RegisterProtocol(SCJoinReq, "OnJoinReq", true)
	self:RegisterProtocol(SCTeamMatchInfo, "OnTeamMatchInfo", true)
	self:RegisterProtocol(SCTeamInvitationNotice, "OnTeamInvitationNotice")
	self:RegisterProtocol(SCTeamEnterFbRoute, "OnTeamEnterFbRoute")
	self:RegisterProtocol(SCTeamEnterFbInfo, "OnTeamEnterFbInfo")
	self:RegisterProtocol(SCTeamMemberAppearanceChange, "OnTeamMemberAppearanceChange")
	self:RegisterProtocol(SCTeamRemoveApply, "OnTeamRemoveApply")
	self:RegisterProtocol(SCTeamLeaderChangeNoticeArea, "OnTeamLeaderChangeNoticeArea")
	self:RegisterProtocol(SCRoleSalaryInfo, "OnRoleSalaryInfo")
	self:RegisterProtocol(SCRoleSalaryDay, "OnRoleSalaryDay")
	self:RegisterProtocol(SCUpdateMemberHPMP, "OnUpdateMemberHPMP")
	self:RegisterProtocol(SCUpdateMemberPetHPMP, "OnUpdateMemberPetHPMP")
	self:RegisterProtocol(SCTeamMemberAddHpMpNotice, "OnTeamMemberAddHpMpNotice")
	self:RegisterProtocol(SCTeamVoteStart, "OnTeamVoteStart")
	self:RegisterProtocol(SCTeamLeaderNpcInfo, "OnTeamLeaderNpcInfo")
	self:RegisterProtocol(SCTeamSimpleInfoChangeNotice, "OnTeamSimpleInfoChangeNotice")
	self:RegisterProtocol(SCTeamMemberBaseInfoChangeNotice, "OnTeamMemberBaseInfoChangeNotice")
	self:RegisterProtocol(SCTeamMemberIncompatibleNotice, "OnTeamMemberIncompatibleNotice")
	self:RegisterProtocol(SCTeamLeaderSummonMemberNotice, "OnTeamLeaderSummonMemberNotice")
	self:RegisterProtocol(SCLeaderCheckMemberItemRet, "OnLeaderCheckMemberItemRet")

	--战斗开始时如果能获得侠义值则后端发包提示
	self:RegisterProtocol(SCSalaryBattleStateInfo,"OnSalaryBattleStateInfo")
	--队伍邀请弹窗
	self:RegisterProtocol(SCTeamRoleRecruitInfo, "SCTeamRoleRecruitInfo")
	self:RegisterProtocol(CSTeamRoleRecruitAnswer)
end

function TeamCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCTeamListAck)
	self:UnRegisterProtocol(SCTeamInfo)
	self:UnRegisterProtocol(SCOutOfTeam)
	self:UnRegisterProtocol(SCInviteNotice)
	self:UnRegisterProtocol(SCJoinReq)
	self:UnRegisterProtocol(SCTeamMatchInfo)
	self:UnRegisterProtocol(SCTeamInvitationNotice)
	self:UnRegisterProtocol(SCTeamEnterFbRoute)
	self:UnRegisterProtocol(SCTeamEnterFbInfo)
	self:UnRegisterProtocol(SCUpdateMemberHPMP)
	self:UnRegisterProtocol(SCTeamVoteStart)
	self:UnRegisterProtocol(SCTeamLeaderNpcInfo)

	self:UnRegisterProtocol(SCSalaryBattleStateInfo)
end

function TeamCtrl:OnTeamListAck(protocol)
    self.data:SetTeamList(protocol)
end

function TeamCtrl:OnTeamInfo(protocol)
	self.data:SetTeamInfo(protocol)
	if self.guide_dragon == nil then
		self.guide_dragon = GuideDragon.New()
	end
	self.guide_dragon:Start(true)
	if ViewMgr:IsOpen(KnightlyView) then
		ViewMgr:CloseView(KnightlyView)
	end
	if ViewMgr:IsOpen(TeamInvitedView) then
		ViewMgr:CloseView(TeamInvitedView)
	end
end

function TeamCtrl:OnOutOfTeam(protocol)
	MainViewData.Instance:ToggleTTTask()
	self.data.cur_select_target = nil
	self.data:SetOutOfTeam(protocol)
	if self.guide_dragon then
		self.guide_dragon:DeleteMe()
		self.guide_dragon = nil
	end
end

function TeamCtrl:OnInviteNotice(protocol)
end

function TeamCtrl:OnJoinReq(protocol)
    self.data:AddApproveMember(protocol)
end

function TeamCtrl:OnTeamMatchInfo(protocol)
    self.data:SetTeamMatchInfo(protocol)
end

--收到组队邀请
function TeamCtrl:OnTeamInvitationNotice(protocol)
    local info = protocol.info
	if MarryData.Instance:IsParadeFloat() then
        self:SendTeamInvationRet(info.leader.uid, false, info.is_cross)
        return
    end
	if info.target == 1 then
		if MainViewData.Instance:IsBattle() then
			return
		end
		if ViewMgr:IsOpen(SwornInvitedView) then
			return
		end
		local friend_info = SocietyData.Instance:GetFriendInfoByUid(info.leader.uid)
		if friend_info then
			ViewMgr:OpenView(SwornInvitedView, {team_info = info})
		end
		return
	elseif info.target == 2 then
		if MainViewData.Instance:IsBattle() or not SceneConfigToMode[SceneData.Instance:GetMode()] then
			self:SendTeamInvationRet(info.leader.uid, false, info.is_cross)
			return
		end
		MarryCtrl.Instance:OnResultTeam(info)
	else
		if not RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_REFUSE_TEAM_REQUEST) then
			self:SendTeamInvationRet(info.leader.uid, false, info.is_cross)
		else
			self.data:SetTeamInvitationNotice(protocol)
		end
	end
end

function TeamCtrl:OnTeamEnterFbRoute(protocol)
    self.data:SetTeamEnterFbRoute(protocol)
end

function TeamCtrl:OnTeamEnterFbInfo(protocol)
    self.data:SetTeamEnterFbInfo(protocol)
end

function TeamCtrl:OnTeamMemberAppearanceChange(protocol)
    self.data:SetTeamMemberAppearanceChange(protocol)
end

function TeamCtrl:OnTeamRemoveApply(protocol)
    self.data:SetTeamRemoveApply(protocol)
end

function TeamCtrl:OnTeamLeaderChangeNoticeArea(protocol)
	VoMgr:ChangeRoleIsLeader(protocol.obj_id, protocol.is_leader)
end

function TeamCtrl:OnRoleSalaryInfo(protocol)
    self.data:SetRoleSalaryInfo(protocol)

	if ViewMgr:IsOpen(KnightlyRankView) then
		KnightlyCtrl.Instance:SendChivalrousRankListReq()
	end
end

function TeamCtrl:OnRoleSalaryDay(protocol)
    self.data:SetRoleSalaryDay(protocol)

	if ViewMgr:IsOpen(KnightlyRankView) then
		KnightlyCtrl.Instance:SendChivalrousRankListReq()
	end
end

function TeamCtrl:OnUpdateMemberHPMP(protocol)
    self.data:SetUpdateMemberHPMP(protocol)
end

function TeamCtrl:OnUpdateMemberPetHPMP(protocol)
    self.data:SetUpdateMemberPetHPMP(protocol)
end

function TeamCtrl:OnTeamMemberAddHpMpNotice(protocol)
	local team_info = self.data:GetTeamInfo()
	local info = protocol.info
	local lang = Language.Team.HealTips
	local co = Item.GetConfig(info.item_id)
	local member_info = team_info:GetMemberInfoByMemberIndex(info.from_team_member_index)
	if nil == co or nil == member_info then return end
	PublicPopupCtrl.Instance:Center(member_info:IsSelf() and
			string.format(lang.tips[1],
				info.partner_id > 0 and lang.object.partner or (1 == info.is_to_use_pet and lang.object.pet or lang.object.role),
				co.param1,
				lang.ut[co.use_type] or ""
			)
		or
			string.format(lang.tips[2],
				member_info.info.name or "",
				info.partner_id > 0 and lang.object.partner or (1 == info.is_to_use_pet and lang.object.pet or lang.object.role),
				co.param1,
				lang.ut[co.use_type] or ""
			)
	)
end

function TeamCtrl:OnTeamVoteStart(protocol)
	PublicPopupCtrl.Instance:DialogTips{
		content = string.format(Language.Team.TeamLedApplyTips, protocol.campaigner_name),
		confirm = {
			func = function()
				self:SendTeamVote(true)
				ViewMgr:CloseView(DialogTipsView)
			end,
			time = protocol.vote_end_timestamp - TimeHelper.GetServerTime()
		},
		cancel = {
			func = function()
				self:SendTeamVote(false)
				ViewMgr:CloseView(DialogTipsView)
			end,
		},
	}
end

function TeamCtrl:OnTeamLeaderNpcInfo(protocol)
	self.data:SetTeamLeaderNpcInfo(protocol)
end

function TeamCtrl:OnTeamSimpleInfoChangeNotice(protocol)
	self.data:SetTeamSimpleInfoChangeNotice(protocol)
end

function TeamCtrl:OnTeamMemberBaseInfoChangeNotice(protocol)
	self.data:SetTeamMemberBaseInfoChangeNotice(protocol)
end

function TeamCtrl:OnTeamMemberIncompatibleNotice(protocol)
	if not self.data:InTeam() then return end
	local team_info = self.data:GetTeamInfo()
	local member_info = team_info:GetMemberInfoByMemberIndex(protocol.team_member_index).info
	local type = TeamConfig.TeamIncompatibleType
	if type.has_jie_yi_zu == protocol.notice_type then --结义组是按人数下发判断的
		member_info = TeamData.Instance:GetMemberRoleList()[protocol.team_member_index + 1]
		if member_info ~= nil then
			local info = member_info.info
			PublicPopupCtrl.Instance:Center(string.format(Language.Team.IncompatibleTips[protocol.notice_type], info.name))
		end
		return
	end
	if member_info then
		if type.world_boss1 == protocol.notice_type or type.world_boss2 == protocol.notice_type or type.times == protocol.notice_type then
			PublicPopupCtrl.Instance:Center(string.format(Language.Team.IncompatibleTips[protocol.notice_type], member_info.name))
		elseif type.level == protocol.notice_type then
			PublicPopupCtrl.Instance:Center(string.format(Language.Team.IncompatibleTips[protocol.notice_type], member_info.name))
		elseif type.item == protocol.notice_type then
			PublicPopupCtrl.Instance:Center(string.format(Language.Team.IncompatibleTips[protocol.notice_type], member_info.name, Item.GetQuaName(protocol.param1)))
		elseif type.max_num == protocol.notice_type then
			PublicPopupCtrl.Instance:Center(string.format(Language.Team.IncompatibleTips[protocol.notice_type], protocol.param1))
		elseif type.not_num == protocol.notice_type then
			PublicPopupCtrl.Instance:Center(string.format(Language.Team.IncompatibleTips[protocol.notice_type], protocol.param1))
		elseif type.not_apprentice == protocol.notice_type then
			PublicPopupCtrl.Instance:Center(string.format(Language.Team.IncompatibleTips[protocol.notice_type], member_info.name))
		elseif type.team_member_incompatible_no_task_record == protocol.notice_type then
			local task_info=   Config.huanshi_challenge_auto.task_list[protocol.param1] 
			local task_name=task_info and task_info.task_name or ""
			PublicPopupCtrl.Instance:Center(string.format(Language.Team.IncompatibleTips[protocol.notice_type], member_info.name, task_name))
		end
	end
end

function TeamCtrl:OnTeamLeaderSummonMemberNotice(protocol)
	PublicPopupCtrl.Instance:DialogTips{
		content = Language.Team.SummonTips,
		confirm = {
			func = function()
				self:SendTeamMemberApplyLeaderSummon(true)
				ViewMgr:CloseView(DialogTipsView)
			end
		},
		cancel = {
			func = function()
				self:SendTeamMemberApplyLeaderSummon(false)
				ViewMgr:CloseView(DialogTipsView)
			end
		}
	}
end

function TeamCtrl:OnLeaderCheckMemberItemRet(protocol)
	self.data:SetTeamItemCheckResult(protocol)
end

function TeamCtrl:SendCheckMemberItem(param_t)
	local protocol = GetProtocol(CSLeaderCheckMemberItem)
	protocol.item_id = param_t.item_id
	protocol.item_num = param_t.item_num
	SendProtocol(protocol)
end

function TeamCtrl:SendCreateTeam(is_cross, is_match, allow_red_join)
	if MarryData.Instance:IsParadeFloat() then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return
    end
	local protocol = GetProtocol(CSCreateTeam)
	protocol.limit_level_low = self.data.cur_sel_target.low_level
	protocol.limit_level_high = self.data.cur_sel_target.high_level
	protocol.team_type = self.data.cur_sel_target.target_id
	protocol.is_cross = is_cross or TeamConfig.CrossType.not_cross
	protocol.is_match = is_match or 0
	protocol.allow_red_join = allow_red_join or 1 --默认允许
	SendProtocol(protocol)
end

function TeamCtrl:SendExitTeam()
	if FightCtrl.IsActFB() then
		return false
	end
	local protocol = GetProtocol(CSExitTeamReq)
	SendProtocol(protocol)
	LogDG("发送了退出队伍协议")
	return true
end

function TeamCtrl:SendTeamMatch(target_id, cross_type, op_type, is_auto_create)
	if MarryData.Instance:IsParadeFloat() then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return
    end
	-- LogError(target_id, cross_type, op_type)
	local protocol = GetProtocol(CSTeamMatchOp)
	protocol.team_type = target_id or self.data.cur_sel_target.target_id or self.data.real_goal_goal.target_id
	protocol.op_type = op_type or (self.data:IsAutoMatch().value and TeamConfig.MatchType.cancel or TeamConfig.MatchType.start)
	protocol.is_cross = cross_type or (self.data:IsCross() and TeamConfig.CrossType.is_cross or TeamConfig.CrossType.not_cross)
	protocol.is_auto_create = is_auto_create or 1
	-- 自动匹配除了巡逻悬赏外需要自动喊话
	self.data.platform_click_num = target_id
	SendProtocol(protocol)
end

function TeamCtrl:SendUnfullTeamListByType(team_type)
	local protocol = GetProtocol(CSUnfullTeamListByTypeReq)
	protocol.team_type = team_type or 1
	SendProtocol(protocol)
end

function TeamCtrl:SendJoinTeam(team_index, cross_type)
	if MarryData.Instance:IsParadeFloat() then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return
    end
	if BattleData.Instance:IsOB() > 0 then
		local info = {
			content = Language.Team.InviteDesc3,
			confirm = {
				func = function()
					local protocol = GetProtocol(CSJoinTeamReq)
					protocol.team_index = team_index or 0
					protocol.is_cross = cross_type or TeamConfig.CrossType.not_cross
					SendProtocol(protocol)
					ViewMgr:CloseView(DialogTipsView)
				end
			}
		}
		PublicPopupCtrl.Instance:DialogTips(info)
		return
	end
	if self.data:InTeam() then
		local info = {
			content = Language.Team.InviteDesc4,
			confirm = {
				func = function()
					local protocol = GetProtocol(CSJoinTeamReq)
					protocol.team_index = team_index or 0
					protocol.is_cross = cross_type or TeamConfig.CrossType.not_cross
					SendProtocol(protocol)
					ViewMgr:CloseView(DialogTipsView)
				end
			}
		}
		PublicPopupCtrl.Instance:DialogTips(info)
		return
	end
	local protocol = GetProtocol(CSJoinTeamReq)
	protocol.team_index = team_index or 0
	protocol.is_cross = cross_type or TeamConfig.CrossType.not_cross
	SendProtocol(protocol)
end

function TeamCtrl:SendChangeMustCheck(check_type)
	local protocol = GetProtocol(CSChangeMustCheck)
	protocol.check_type = check_type or 0
	SendProtocol(protocol)
end

function TeamCtrl:SendJoinTeamReq(uid, is_agree)
	local protocol = GetProtocol(CSJoinTeamReqRet)
	protocol.uid = uid or 0
	protocol.is_agree = is_agree or 1
	SendProtocol(protocol)
end

function TeamCtrl:SendChangeTeamLimit(limit_level_low, limit_level_high, target_id, allow_red_join)
	-- LogError(limit_level_low, limit_level_high, target_id)
	local protocol = GetProtocol(CSChangeTeamLimit)
	protocol.limit_level_low = limit_level_low
	protocol.limit_level_high = limit_level_high
	protocol.team_type = target_id
	protocol.allow_red_join = allow_red_join or (self.data:GetTeamInfo():IsAllowRedJoin() and 1 or 0)
	SendProtocol(protocol)
end

function TeamCtrl:SendChangeTeamLimitAuto()
	self:SendChangeTeamLimit(self.data.real_goal_level_low, self.data.real_goal_level_high, self.data.real_goal_goal.target_id)
end

function TeamCtrl:SendSetAllowRedJoin(is_allow)
	self:SendChangeTeamLimit(self.data.real_goal_level_low, self.data.real_goal_level_high, self.data.real_goal_goal.target_id, is_allow)
end

function TeamCtrl:SendChangeTeamLeader(member_index)
	local protocol = GetProtocol(CSChangeTeamLeader)
	protocol.member_index = member_index
	SendProtocol(protocol)
end

function TeamCtrl:SendKickOutOfTeamReq(member_index)
	local protocol = GetProtocol(CSKickOutOfTeamReq)
	protocol.kick_index = member_index
	SendProtocol(protocol)
	BattleData.FinishAfterBattle()
end

function TeamCtrl:SendTemporarilyPartReq()
	local protocol = GetProtocol(CSTemporarilyPartReq)
	SendProtocol(protocol)
end

--target 0 默认组队 1 结义组队 2 情缘组队
function TeamCtrl:SendTeamInvitationReq(uid, target)
	local protocol = GetProtocol(CSTeamInvitationReq)
	protocol.uid = uid or 0
	protocol.target = target or 0
	SendProtocol(protocol)
end

function TeamCtrl:SendTeamInvationRet(uid, is_agree, is_from_cross, team_index)
	local protocol = GetProtocol(CSTeamInvationRet)
	protocol.uid = uid or 0
	protocol.is_accpet = is_agree and 1 or 0
	protocol.is_from_cross = is_from_cross or 0
	protocol.team_index = team_index or 0
	SendProtocol(protocol)
end

function TeamCtrl:SendTeamRecruit()
	local protocol = GetProtocol(CSTeamRecruit)
	SendProtocol(protocol)
end

function TeamCtrl:SendTeamEnterFbRet(is_determine)
	local protocol = GetProtocol(CSTeamEnterFbRet)
	protocol.is_determine = is_determine and 1 or 0
	SendProtocol(protocol)
end

function TeamCtrl:SendTeamSwitchToCross()
	local protocol = GetProtocol(CSTeamSwitchToCross)
	SendProtocol(protocol)
end

function TeamCtrl:SendTeamSwitchPosition(member_index)
	local protocol = GetProtocol(CSTeamSwitchPosition)
	protocol.pos1 = self.data.sort_index_first.val
	protocol.pos2 = member_index
	SendProtocol(protocol)
	BattleData.FinishAfterBattle()
end

function TeamCtrl:SendRoleSalaryInfo(salary_type)
	local protocol = GetProtocol(CSRoleSalaryInfo)
	protocol.salary_type = salary_type or TeamConfig.SalaryType.salary
	SendProtocol(protocol)
end

function TeamCtrl:SendTeamLaunchLeaderVote()
	local protocol = GetProtocol(CSTeamLaunchLeaderVote)
	SendProtocol(protocol)
end

function TeamCtrl:SendTeamVote(is_agree)
	local protocol = GetProtocol(CSTeamVote)
	protocol.is_yes = is_agree and 1 or 0
	SendProtocol(protocol)
end

function TeamCtrl:ExitTeam()
	local info = {
        content = Language.Team.TipsIsLeaveTeam,
        confirm = {
            func = function()
                self:SendExitTeam()
                ViewMgr:CloseView(DialogTipsView)
            end
        }
    }
	PublicPopupCtrl.Instance:DialogTips(info)
end

function TeamCtrl:SendTeamLeaderNpcReq(oper_type, npc_list)
	if not self.data:IsLeader() then return end
	local protocol = GetProtocol(CSTeamLeaderNpcReq)
	local list = {}
	for _, v in pairs(npc_list) do
		if v.is_not_notice == nil then
			table.insert(list, v.seq)
		end
	end
	protocol.oper_type = oper_type
	protocol.npc_list = list
	SendProtocol(protocol)
end

function TeamCtrl:SendUseGoldRecovery(param1, param2, param3, param4)
	local protocol = GetProtocol(CSUseGoldRecovery)
	protocol.param1 = param1	
	-- 0 加自己
	-- 1 加自己宠物 p2:pet_index
	-- 2 加自己伙伴 p2:partner_id
	-- 3 队员 p2:team_member_index p3:是否对宠物
	-- 4 队长伙伴 p2:伙伴id
	protocol.param2 = param2
	protocol.param3 = param3
	protocol.param4 = param4	-- 1 血 2 蓝
	SendProtocol(protocol)
end

function TeamCtrl:SendTeamLeaderSummonMember(uid)
	local protocol = GetProtocol(CSTeamLeaderSummonMember)
	protocol.summon_role_id = uid
	SendProtocol(protocol)
end

function TeamCtrl:SendTeamMemberApplyLeaderSummon(is_return)
	local protocol = GetProtocol(CSTeamMemberApplyLeaderSummon)
	protocol.is_return = is_return and 1 or 0
	SendProtocol(protocol)
end

function TeamCtrl:SendReqJoinTeamInfo(is_from_cross, team_index, uid)
	local protocol = GetProtocol(CSReqJoinTeamInfo)
	protocol.is_from_cross = is_from_cross or 0
	protocol.team_index = team_index or 0
	protocol.uid = uid or 0
	SendProtocol(protocol)
end

function TeamCtrl:SendTeamSort(member_index)
	local in_team = self.data:InTeam()
	if in_team then
		self:SendTeamSwitchPosition(member_index)
	else
		PartnerCtrl.Instance:SendPartnerSwitchFightPos(self.data.sort_index_first.val, member_index)
	end
	self.data.sort_index_first.val = -1
end

-- 队伍检测
-- 没有队伍 匹配目标队伍
-- 有队伍 人数足够就执行func并修改队伍目标 人数不足则打开队伍界面修改目标并开启自动匹配

-- 队伍检测ActType
-- func 符合条件执行的操作
-- need_num 可以为空 默认为活动表中的number_min
function TeamCtrl:TeamDetectByActType(act_type, func, need_num, target_id)
	local co = Cfg.TeamTypeTargetAct(act_type)
	local act_co = ActivityData.Instance.config_daily[act_type]
	local number_min = act_co and act_co.number_min or 1
	if co then return self:TeamDetectByTargetId(target_id or co.target_id, func, need_num or number_min) end
end

-- 队伍检测TargetId
-- func 符合条件执行的操作
-- need_num 可以为空 活动默认为默认为活动表中的number_min 其他默认为1
-- change_func 如果人数足够但队伍类型不对，就会发送改变队伍类型的协议然后去执行这个方法，可为空
-- 返回值 {type, need_num} 根据需求来
function TeamCtrl:TeamDetectByTargetId(target_id, func, need_num, change_func)
	if nil == target_id or nil == func or "function" ~= type(func) then return end
	local co = self.data.team_goal_list[target_id]
	if nil == co then return end
	-- if nil == need_num and TeamConfig.TeamTargetType.Activity == co.team_type then
	if nil == need_num and co.act_type > 0 then
		local act_co = ActivityData.Instance.config_daily[co.act_type]
		need_num = act_co and act_co.number_min or 1
	end
	need_num = need_num or 1
	local in_team = self.data:InTeam()
	local member_num_role = self.data:MemberNumRole()
	if LoginData.Instance:IsOnCrossSever() and TeamConfig.CrossType.not_cross == co.is_cross then
		PublicPopupCtrl.Instance:Center(Language.Team.MatchCrossError)
		return
	end
	if RoleData.Instance:GetRoleLevel() < co.low_level then
		PublicPopupCtrl.Instance:Center(Language.Team.MatchLevelError)
		return
	end
	if in_team then
		local team_info = self.data:GetTeamInfo()
		local now_target = team_info.info.team_type
		-- 如果人够了并且队伍类型正确
		if member_num_role >= need_num and now_target == target_id then
			func()
		else
			-- 如果只是人够了，就改类型，否则就继续匹配
			if member_num_role >= need_num then
				self:SendChangeTeamLimit(co.low_level, co.high_level, target_id)
				if "function" == type(change_func) then
					change_func()
				end
			else
				ViewMgr:OpenView(TeamView)
				self.data.need_auto_match = true
				--不满足条件继续匹配
				self:SendChangeTeamLimit(co.low_level, co.high_level, target_id)
				self:SendTeamMatch(target_id, co.is_cross, TeamConfig.MatchType.start)
			end
		end
		-- if self:Dragon() == nil or self:Dragon():Step() == 0 then
			
		-- end
	else
		PublicPopupCtrl.Instance:Center(need_num > 1 and string.format(Language.Team.NumberNotEnough, need_num) or Language.Team.MatchTips)
		self.data.cur_platform_target_id = target_id
		-- ViewMgr:OpenView(TeamPlatformView)
		self:SendTeamMatch(target_id, co.is_cross, TeamConfig.MatchType.start)
		TimeHelper:AddDelayTimer(function()
			MainViewData.Instance:ToggleTTTeam()
		end, 0.5)
		return {type = TeamConfig.TeamDetectType.fail_no, need_num = need_num}
	end
end

-- 组队目标改为组队挂机(一条龙除外)
function TeamCtrl:ChangeTeamLimitToDefault()
	local in_team = self.data:InTeam()
	local is_leader = self.data:IsLeader()
	local dragon = self:Dragon()
	if not in_team or not is_leader then return end
	if dragon and dragon:Step() > 0 then return end
	local co = self.data.team_goal_list[1]
	self:SendChangeTeamLimit(co.low_level, co.high_level, 1)
end

function TeamCtrl:Dragon()
	if self.guide_dragon == nil then
		return nil
	end
	return self.guide_dragon
end

function TeamCtrl:OnSalaryBattleStateInfo(proc)
	self.data:PushBattleSalaryDisplay({seq = proc.seq,salary = proc.salary})
end

function TeamCtrl:CheckAndPopBattleSalaryDisplay()
	local sal_data = TeamData.Instance:PopBattleSalaryDisplay()
	if sal_data ~= nil then
		ViewMgr:OpenView(BattleSalaryDisplayView,sal_data)
	end
end

--创建组队目标队伍
function TeamCtrl:OnSendCreateTeam(wait_func)
	if not self.data:InTeam() and self.data.cur_select_target then
		self.data.cur_sel_target = self.data.cur_select_target
		self:SendCreateTeam(self.data.cur_select_target.is_cross,false)
	end
	Invoke(function()
		wait_func()
	end,LoginData.Instance:IsOnCrossSever() and 0.1 or 1.5)
end

--组队邀请
function TeamCtrl:OnTeamInvitationReq(uid)
	self:OnSendCreateTeam(function()
		self:SendTeamInvitationReq(uid)
	end)
end

--世界喊话按钮
function TeamCtrl:OnSendNotice()
	local notice_time = self.data.notice_time.val
	if 0 == notice_time then
		self:OnSendCreateTeam(function()
			self:SendTeamRecruit()
			self.data:NoticeTimer()
		end)
	else
		PublicPopupCtrl.Instance:Center(Format(Language.Team.RecruitTips,notice_time))
	end
end
function TeamCtrl:SCTeamRoleRecruitInfo(protocol)
	self.data:SetTeamInviteInfo(protocol.info)
end
function TeamCtrl:CSTeamRoleRecruitAnswer(type)
	local protocol = GetProtocol(CSTeamRoleRecruitAnswer)
	protocol.answer_type = type or 0
	SendProtocol(protocol)
end