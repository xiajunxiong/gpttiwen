
FILE_REQ("modules/society/society_config")
FILE_REQ("modules/society/society_info")
FILE_REQ("modules/society/society_data")
FILE_REQ("modules/society/society_data_chat")
FILE_REQ("modules/society/society_data_friend")
FILE_REQ("modules/society/society_data_mail")
FILE_REQ("modules/society/society_data_master_apprentice")
FILE_REQ("modules/society/society_data_shitu")
FILE_REQ("modules/society/society_data_shitu_fb")
FILE_REQ("modules/society/society_data_first_kill")
FILE_REQ("modules/society/society_data_rank")
FILE_REQ("modules/society/society_data_share")

VIEW_REQ("modules/society/society_view")
VIEW_REQ("modules/society/society_view_friend_group")
VIEW_REQ("modules/society/society_view_master_apprentice_group")
VIEW_REQ("modules/society/society_view_chat")
VIEW_REQ("modules/society/society_view_declaration")
VIEW_REQ("modules/society/society_view_flower")
VIEW_REQ("modules/society/society_view_report")


VIEW_REQ("modules/society/society_view_friend_add")
VIEW_REQ("modules/society/society_view_friend_apply")
VIEW_REQ("modules/society/society_view_friend_edit_new_group")

VIEW_REQ("modules/society/society_group_create_view")
VIEW_REQ("modules/society/society_group_trans_view")
VIEW_REQ("modules/society/society_group_invite_view")
VIEW_REQ("modules/society/society_group_manage_view")
VIEW_REQ("modules/society/society_group_info_view")

VIEW_REQ("modules/society/society_view_master_apprentice_apply")
VIEW_REQ("modules/society/society_view_master_apprentice_decl")
VIEW_REQ("modules/society/society_view_master_apprentice_search")
VIEW_REQ("modules/society/society_view_shitu_info")
VIEW_REQ("modules/society/society_maq_view")
VIEW_REQ("modules/society/society_view_master_relation_broken")

VIEW_REQ("modules/society/society_view_mail_left")
VIEW_REQ("modules/society/society_view_mail_detail")

VIEW_REQ("modules/society/rank/rank_view")
VIEW_REQ("modules/society/rank/society_view_rank")
VIEW_REQ("modules/society/rank/society_view_first_kill")

VIEW_REQ("modules/society/society_dialog_view")

VIEW_REQ("modules/society/shitu_fb/society_shitu_fb_guide_view")
VIEW_REQ("modules/society/shitu_fb/society_shitu_cg_guide_view")
VIEW_REQ("modules/society/shitu_fb/society_shitu_achieve_guide_view")
VIEW_REQ("modules/society/shitu_fb/society_shitu_fb_progress_view")
VIEW_REQ("modules/society/shitu_fb/shitu_fb_menu_view")
VIEW_REQ("modules/society/shitu_fb/society_shitu_fb_result_view")

VIEW_REQ("modules/society/share/society_send_view")
VIEW_REQ("modules/society/share/society_share_view")
VIEW_REQ("modules/society/share/society_gift_view")
VIEW_REQ("modules/society/share/society_flower_get_view")

SocietyCtrl = SocietyCtrl or BaseClass(BaseCtrl)

function SocietyCtrl:__init()
	if SocietyCtrl.Instance ~= nil then
		Debuger.LogError("[SocietyCtrl] attempt to create singleton twice!")
		return
	end	
	SocietyCtrl.Instance = self
	self.data = SocietyData.New()

	self:RegisterAllProtocols()


	RemindCtrl.Instance:Remind():Register(Mod.Society.Friend, self.data.new_friend_red_point, function ()
		return self.data.new_friend_red_point.val and 1 or 0
	end)

	RemindCtrl.Instance:Remind():Register(Mod.Society.Friend, self.data.friend_chat_red_point, function ()
		return self.data.friend_chat_red_point.val and 1 or 0
	end)
	RemindCtrl.Instance:Remind():Register(Mod.Society.Friend, self.data.friend_gift_red_point, function ()
		return self.data.friend_gift_red_point.val and 1 or 0
	end)

	RemindCtrl.Instance:Remind():Register(Mod.Society.Friend, self.data.friend_chat_group_red_point, function ()
		return self.data.friend_chat_group_red_point.val and 1 or 0
	end)


	RemindCtrl.Instance:Remind():Register(Mod.Society.Mail, self.data.mail_red_point, function ()
		return self.data.mail_red_point.val and 1 or 0
	end)

	RemindCtrl.Instance:Remind():Register(Mod.Society.Master, self.data.new_master_red_point, function ()
		return self.data.new_master_red_point.val and 1 or 0
	end)

	RemindCtrl.Instance:Remind():Register(Mod.Society.Master, self.data.shitu_achieve_red_point, function ()
		return self.data.shitu_achieve_red_point.val and 1 or 0
	end)

	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))

	LoginData.Instance:BaseData():Care(BindTool.Bind(self.OnUnloadGameLogic, self), "is_on_crosssever")
end

function SocietyCtrl:__delete()
	if SocietyCtrl.Instance == self then
		SocietyCtrl.Instance = nil
	end
	
	EventSys:UnBind(self.complete_handle)

	self:UnRegisterAllProtocols()
end

function SocietyCtrl:OnInit()
	BehaviorData.Instance:CustomNpcTalkBtnsSeq(self.data:GetShiTuFBNpcSeq(), BindTool.Bind(self.SetupBehaviourBtn, self))
end

function SocietyCtrl:OnUnloadGameLogic()
	self.data:SocietyDataR()
end

function SocietyCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSFriendReq)

	self:RegisterProtocol(CSFriendRecommendReq)
	self:RegisterProtocol(CSFriendRecommendAddOnListReq)
	self:RegisterProtocol(CSRefreshFriendRecommend)

	self:RegisterProtocol(CSCreateGroup)
	self:RegisterProtocol(CSMoveGroup)
	-- self:RegisterProtocol(CSRenameGroup)

	self:RegisterProtocol(CSGetRoleGroupList)
	self:RegisterProtocol(CSGetGroupInfo)
	self:RegisterProtocol(CSCreateGroupReq)
	self:RegisterProtocol(CSRoleInviteEnterGroup)
	self:RegisterProtocol(CSEnterGroupReq)
	self:RegisterProtocol(CSGroupKickMember)
	self:RegisterProtocol(CSGroupDissmiss)
	self:RegisterProtocol(CSGroupLeaveReq)
	self:RegisterProtocol(CSChangeGroupNoticeReq)
	self:RegisterProtocol(CSTempChatGroupBecomeOfficialReq)
	self:RegisterProtocol(CSHandoverChatGroupLeadership)

	self:RegisterProtocol(CSRoleSystemSetChatGroupReq)

	self:RegisterProtocol(CSFuzzySearchRoleByName)

	self:RegisterProtocol(CSShiTuMakeRelationReq)
	self:RegisterProtocol(CSShiTuMakeRelationAck)
	self:RegisterProtocol(CSShiTuBreakRelationReq)
	self:RegisterProtocol(CSShiTuBreakRelationAck)
	self:RegisterProtocol(CSShiTuReq)
	self:RegisterProtocol(CSShiTuSeekingReq)

	self:RegisterProtocol(CSShiTuChuanGongFBReq)

	self:RegisterProtocol(CSMailGetList)
	self:RegisterProtocol(CSMailRead)
	self:RegisterProtocol(CSMailFetchAttachment)
	self:RegisterProtocol(CSMailDelete)
	self:RegisterProtocol(CSMailOneKeyFetchAttachment)
	self:RegisterProtocol(CSMailClean)
	
	self:RegisterProtocol(CSServerFirstKillReq)

	self:RegisterProtocol(CSGetPersonRankList)
	self:RegisterProtocol(CSGetPetRankList)
	self:RegisterProtocol(CSCrossGetPersonRankBaseInfo)
	self:RegisterProtocol(CSCrossGetPersonAllRankList)
	self:RegisterProtocol(CSCrossGetPetAllRankList)
	self:RegisterProtocol(CSCrossGetPersonRankList)
	self:RegisterProtocol(CSCrossGetPetRankList)
	self:RegisterProtocol(CSGetRankPetDetailedInfo)
	self:RegisterProtocol(CSCrossGetRankPetDetailedInfo)
	self:RegisterProtocol(CSCrossGetRankPetAllDetailedInfo)

	self:RegisterProtocol(CSChatRecordSet)
	self:RegisterProtocol(CSGetRankRefreshTime)

	self:RegisterProtocol(CSGiftReq)

	self:RegisterProtocol(SCGetRankRefreshTime,"OnGetRankRefreshTime")
	self:RegisterProtocol(SCFriendInfo, "OnFriendInfo")
	self:RegisterProtocol(SCFriendListChange, "OnFriendListChange", true)

	self:RegisterProtocol(SCAddFriendRoute, "OnAddFriendRoute", true)

	self:RegisterProtocol(SCChangeBlack, "OnChangeBlack", true)
	self:RegisterProtocol(SCBlackInfo, "OnBlackInfo")

	self:RegisterProtocol(SCFriendGroupInfo, "OnFriendGroupInfo", true)
	
	self:RegisterProtocol(SCRoleGroupInfo, "OnRoleGroupInfo", true)
	self:RegisterProtocol(SCGroupAllInfo, "OnGroupAllInfo", true)
	self:RegisterProtocol(SCRoleOneGroupInfo, "OnRoleOneGroupInfo", true)
	self:RegisterProtocol(SCRoleInviteEnterGroup, "OnRoleInviteEnterGroup", true)
	self:RegisterProtocol(SCTempGroupInfo, "OnTempGroupInfo", true)
	self:RegisterProtocol(SCTempGroupLeaveNotice, "OnTempGroupLeaveNotice", true)
	self:RegisterProtocol(SCChatGroupMsgNotice, "OnChatGroupMsgNotice", true)
	self:RegisterProtocol(SCTempChatGroupBecomeOfficialNotice, "OnTempChatGroupBecomeOfficialNotice")
	self:RegisterProtocol(SCRoleSystemSetChatGroupInfo, "OnRoleSystemSetChatGroupInfo")

	self:RegisterProtocol(SCRecentlyTeamInfo, "OnRecentlyTeamInfo")
	self:RegisterProtocol(SCOneRecentlyTeamInfo, "OnOneRecentlyTeamInfo", true)
	self:RegisterProtocol(SCRecentlyChatAdd, "OnRecentlyChatAdd", true)

	self:RegisterProtocol(SCFriendRecommendList, "OnFriendRecommendList", true)
	-- self:RegisterProtocol(SCFriendRecommendOne, "OnFriendRecommendOne", true)

	self:RegisterProtocol(SCFuzzySearchRolesInfo, "OnFuzzySearchRolesInfo", true)

	self:RegisterProtocol(SCShiTuMakeRelationRoute, "OnShiTuMakeRelationRoute", true)
	self:RegisterProtocol(SCShiTuRelationInfo, "OnShiTuRelationInfo", true)
	self:RegisterProtocol(SCShiTuBreakRelationRoute, "OnShiTuBreakRelationRoute")
	self:RegisterProtocol(SCShiTuSeekingInfo, "OnShiTuSeekingInfo", true)
	self:RegisterProtocol(SCShiTuPreferenceInfo, "OnShiTuPreferenceInfo", true)

	self:RegisterProtocol(SCShiTuChuanGongGameInfo, "OnShiTuChuanGongGameInfo", true)
	self:RegisterProtocol(SCShiTuChuanGongSceneInfo, "OnShiTuChuanGongSceneInfo", true)
	self:RegisterProtocol(SCShiTuSyncInfo, "OnShiTuSyncInfo", true)

	self:RegisterProtocol(SCShiTuFbSceneInfo, "OnShiTuFbSceneInfo")
	self:RegisterProtocol(SCShiTuFbCommonInfo, "OnShiTuFbCommonInfo", true)
	self:RegisterProtocol(SCShiTuFbInvitationNotice, "OnShiTuFbInvitationNotice")
	self:RegisterProtocol(SCShiTuFbFinishNotice, "OnShiTuFbFinishNotice")
	self:RegisterProtocol(SCShiTuFbBattleResult, "OnShiTuFbBattleResult")

	self:RegisterProtocol(SCMailListAck, "OnMailListAck", true)
	self:RegisterProtocol(SCMailDetailAck, "OnMailDetailAck", true)
	self:RegisterProtocol(SCFetchAttachmentAck, "OnFetchAttachmentAck", true)
	self:RegisterProtocol(SCRecvNewMail, "OnRecvNewMail", true)
	self:RegisterProtocol(SCMailDeleteAck, "OnMailDeleteAck", true)

	self:RegisterProtocol(SCServerFirstKillInfo, "OnServerFirstKillInfo")

	
	self:RegisterProtocol(SCReplyRoleIsOnline, "OnReplyRoleIsOnline")
	
	self:RegisterProtocol(SCGetPersonRankListAck, "OnGetPersonRankListAck")
	self:RegisterProtocol(SCGetPetRankListAck, "OnGetPetRankListAck")
	self:RegisterProtocol(SCCrossPersonRankBaseInfo, "OnCrossPersonRankBaseInfo")
	self:RegisterProtocol(SCGetCrossPersonAllServerRankListAck, "OnGetCrossPersonAllServerRankListAck")
	self:RegisterProtocol(SCGetCrossPetAllServerRankListAck, "OnGetCrossPetAllServerRankListAck")
	self:RegisterProtocol(SCGetCrossPersonRankListAck, "OnGetCrossPersonRankListAck")
	self:RegisterProtocol(SCGetCrossPetRankListAck, "OnGetCrossPetRankListAck")
	self:RegisterProtocol(SCRankPetDetailedInfo, "OnRankPetDetailedInfo")
	
	self:RegisterProtocol(SCChatRecordList, "OnChatRecordList")
	
	self:RegisterProtocol(SCGiftRoleInfo, "OnGiftRoleInfo")
	self:RegisterProtocol(SCGetGiftInfo, "OnGetGiftInfo")

	self:RegisterProtocol(SCGiveFlowerNotice, "OnGiveFlowerNotice")

	self:RegisterProtocol(SCFriendIntimacyListInfo, "OnFriendIntimacyListInfo")
	self:RegisterProtocol(SCFriendIntimacyInfoChange, "OnFriendIntimacyInfoChange", true)

	self:RegisterProtocol(SCShiTuOpNotice, "SCShiTuOpNotice")
end

function SocietyCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCFriendInfo)
	self:UnRegisterProtocol(SCFriendListChange)
	self:UnRegisterProtocol(SCAddFriendRoute)
	self:UnRegisterProtocol(SCChangeBlack)
	self:UnRegisterProtocol(SCBlackInfo)
	self:UnRegisterProtocol(SCFriendGroupInfo)
	self:UnRegisterProtocol(SCRoleGroupInfo)
	self:UnRegisterProtocol(SCGroupAllInfo)
	self:UnRegisterProtocol(SCRoleOneGroupInfo)
	self:UnRegisterProtocol(SCRoleInviteEnterGroup)
	self:UnRegisterProtocol(SCRecentlyTeamInfo)
	self:UnRegisterProtocol(SCOneRecentlyTeamInfo)
	self:UnRegisterProtocol(SCRecentlyChatAdd)
	self:UnRegisterProtocol(SCFriendRecommendList)
	-- self:UnRegisterProtocol(SCFriendRecommendOne)
	self:UnRegisterProtocol(SCFuzzySearchRolesInfo)
	self:UnRegisterProtocol(SCShiTuMakeRelationRoute)
	self:UnRegisterProtocol(SCShiTuRelationInfo)
	self:UnRegisterProtocol(SCShiTuBreakRelationRoute)
	self:UnRegisterProtocol(SCShiTuSeekingInfo)
	self:UnRegisterProtocol(SCShiTuPreferenceInfo)
	self:UnRegisterProtocol(SCShiTuChuanGongGameInfo)
	self:UnRegisterProtocol(SCShiTuChuanGongSceneInfo)
	self:UnRegisterProtocol(SCShiTuFbSceneInfo)
	self:UnRegisterProtocol(SCShiTuFbCommonInfo)
	self:UnRegisterProtocol(SCShiTuFbInvitationNotice)
	self:UnRegisterProtocol(SCShiTuFbFinishNotice)
	self:UnRegisterProtocol(SCShiTuFbBattleResult)
	self:UnRegisterProtocol(SCMailListAck)
	self:UnRegisterProtocol(SCMailDetailAck)
	self:UnRegisterProtocol(SCFetchAttachmentAck)
	self:UnRegisterProtocol(SCRecvNewMail)
	self:UnRegisterProtocol(SCMailDeleteAck)
	self:UnRegisterProtocol(SCGiveFlowerNotice)
	self:UnRegisterProtocol(SCGetRankRefreshTime)
	self:UnRegisterProtocol(SCFriendIntimacyInfoChange)
end

function SocietyCtrl:RoleNoticeComplete()
	self.data:SocietyData()
	self:SendCrossGetPersonRankBaseInfo()
	self:SendMyRankShow()
	self.data.quick_chat_info.cur_sel = ""
end

function SocietyCtrl:OnFriendInfo(protocol)
	self.data:SetFriendInfo(protocol)
end

function SocietyCtrl:OnFriendListChange(protocol)
	self.data:SetFriendListChange(protocol)	
end

function SocietyCtrl:OnAddFriendRoute(protocol)
    self.data:SetAddFriendRoute(protocol)
end

function SocietyCtrl:OnChangeBlack(protocol)
    self.data:SetChangeBlack(protocol)
end

function SocietyCtrl:OnBlackInfo(protocol)
    self.data:SetBlackInfo(protocol)
end

function SocietyCtrl:OnFriendGroupInfo(protocol)
	self.data:SetFriendGroupInfo(protocol)
end

function SocietyCtrl:OnRoleGroupInfo(protocol)
	self.data:SetRoleGroupInfo(protocol)
end

function SocietyCtrl:OnGroupAllInfo(protocol)
	self.data:SetGroupAllInfo(protocol)
end

function SocietyCtrl:OnRoleOneGroupInfo(protocol)
	self.data:SetRoleOneGroupInfo(protocol)
end

function SocietyCtrl:OnRoleInviteEnterGroup(protocol)
	local info = protocol.info
	PublicPopupCtrl.Instance:DialogTips{
		content = string.format(Language.Society.ChatGroupInviteDesc, info.invite_name, info.group_name),
		confirm = {
			func = function()
				SocietyCtrl.Instance:SendEnterGroupReq(info.is_cross, info.server_id, info.invite_uid, info.group_id)
				ViewMgr:CloseView(DialogTipsView)
			end
		}
	}
end

function SocietyCtrl:OnTempGroupInfo(protocol)
	self.data:SetTempGroupInfo(protocol)
end

function SocietyCtrl:OnTempGroupLeaveNotice(protocol)
	self.data:SetTempGroupLeaveNotice()
end

function SocietyCtrl:OnChatGroupMsgNotice(protocol)
	self.data:SetChatGroupMsgNotice(protocol)
end

function SocietyCtrl:OnTempChatGroupBecomeOfficialNotice(protocol)
	self.data:SetTempChatGroupBecomeOfficialNotice(protocol)
end

function SocietyCtrl:OnRoleSystemSetChatGroupInfo(protocol)
	self.data:SetRoleSystemSetChatGroupInfo(protocol)
end

function SocietyCtrl:OnRecentlyTeamInfo(protocol)
	self.data:SetRecentlyTeamInfo(protocol)
end

function SocietyCtrl:OnOneRecentlyTeamInfo(protocol)
	self.data:SetOneRecentlyTeamInfo(protocol)
end

function SocietyCtrl:OnRecentlyChatAdd(protocol)
	self.data:SetRecentlyChatAdd(protocol)
end

function SocietyCtrl:OnFriendRecommendList(protocol)
    self.data:SetFriendRecommendList(protocol)
end

-- function SocietyCtrl:OnFriendRecommendOne(protocol)
--     self.data:SetFriendRecommendOne(protocol)
-- end

function SocietyCtrl:OnFuzzySearchRolesInfo(protocol)
    self.data:SetFuzzySearchRolesInfo(protocol)
end

function SocietyCtrl:OnShiTuMakeRelationRoute(protocol)
    self.data:SetShiTuMakeRelationRoute(protocol)
end

function SocietyCtrl:OnShiTuRelationInfo(protocol)
    self.data:SetShiTuRelationInfo(protocol)
end

function SocietyCtrl:OnShiTuBreakRelationRoute(protocol)
	self.data:SetShiTuBreakRelationRoute(protocol)
end

function SocietyCtrl:OnShiTuSeekingInfo(protocol)
    self.data:SetShiTuSeekingInfo(protocol)
end

function SocietyCtrl:OnShiTuPreferenceInfo(protocol)
    self.data:SetShiTuPreferenceInfo(protocol)
end

function SocietyCtrl:OnShiTuChuanGongGameInfo(protocol)
    self.data:SetShiTuChuanGongGameInfo(protocol)
end

function SocietyCtrl:OnShiTuChuanGongSceneInfo(protocol)
    self.data:SetShiTuChuanGongSceneInfo(protocol)
end

function SocietyCtrl:OnShiTuSyncInfo(protocol)
    self.data:SetShiTuSyncInfo(protocol)
end

function SocietyCtrl:OnShiTuFbSceneInfo(protocol)
    self.data:SetShiTuFbSceneInfo(protocol)
end

function SocietyCtrl:OnShiTuFbCommonInfo(protocol)
    self.data:SetShiTuFbCommonInfo(protocol)
end

function SocietyCtrl:OnShiTuFbInvitationNotice(protocol)
	local info = protocol.info
	local tips,tips_func = AnecdoteData.Instance:GetCloseTip()
	local info = {
        content = Format(Language.Team.InviteDesc, info.name) .. tips,
        confirm = {
            func = function()
				TeamCtrl.Instance:SendTeamInvationRet(info.uid, true, info.is_cross, info.team_index)
				ViewMgr:CloseView(DialogTipsView)
				if tips_func ~= nil then
					tips_func()
				end
            end
		},
        cancel = {
            func = function()
                TeamCtrl.Instance:SendTeamInvationRet(info.uid, false, info.is_cross, info.team_index)
                ViewMgr:CloseView(DialogTipsView)
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

function SocietyCtrl:OnShiTuFbFinishNotice(protocol)
	if MainViewData.Instance:IsBattle() then
		FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_SHITU_FB, function(is_win)
			ViewMgr:OpenView(SocietyShiTuFBResultView)
			FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_SHITU_FB, nil)
		end)
	else
		ViewMgr:OpenView(SocietyShiTuFBResultView)
	end
end

function SocietyCtrl:OnShiTuFbBattleResult(protocol)
	self.data:SetShiTuFbBattleResult(protocol)
	if 1 == protocol.info.result then
		ViewMgr:OpenView(FinishPassViewShiTuFB)
	end
end

function SocietyCtrl:OnMailListAck(protocol)
    self.data:SetMailListAck(protocol)
end

function SocietyCtrl:OnMailDetailAck(protocol)
	self.data:SetMailDetailAck(protocol)
end

function SocietyCtrl:OnFetchAttachmentAck(protocol)
    self.data:SetFetchAttachmentAck(protocol)
end

function SocietyCtrl:OnRecvNewMail(protocol)
    self.data:SetRecvNewMail(protocol)
end

function SocietyCtrl:OnMailDeleteAck(protocol)
    self.data:SetMailDeleteAck(protocol)
end

function SocietyCtrl:OnServerFirstKillInfo(protocol)
    self.data:SetServerFirstKillInfo(protocol)
end

-- function SocietyCtrl:OnGiveFlowerNotice(protocol)
-- 	local data = SocietyData.Instance:GetFriendFlowerData(protocol.seq)
-- 	if data == nil then return end
-- 	local name = Format(Language.Society.AcceptFlowerRole,protocol.giver_name)
-- 	local center = {item_id = data.item_id,num = 1,content = name .. Language.Society.AcceptFlower}
-- 	PublicPopupCtrl.Instance:CenterI(center)
-- end

function SocietyCtrl:OnReplyRoleIsOnline(protocol)
	GuildCtrl.Instance:OnReplyRoleIsOnline(protocol)
end

--不在跨服中的本服排行榜
function SocietyCtrl:OnGetPersonRankListAck(protocol)
	local rank_type = protocol.info.rank_type
	if SocietyConfig.RankTypeRoleToC[rank_type] then
		self.data:SetGetPersonRankListAck(protocol)
	elseif ChallengeTestConfig.RANK_TYPE == rank_type then
		ChallengeTestCtrl.Instance:OnGetPersonRankListAck(protocol)
	elseif DeepDarkFantasyData.RANK_TYPE == rank_type then
		DeepDarkFantasyCtrl.Instance:OnGetPersonRankListAck(protocol)
	elseif ActMeiLiQiXiData.RANK_TYPE == rank_type then
		ActMeiLiQiXiCtrl.Instance:OnGetMeiLiBangRankList(protocol)
	elseif rank_type == PERSON_RANK_TYPE.NEW_COURAGE_CHALLENGE then
		CourageChallengeData.Instance:SetNewCourageChallengeRankListInfo(protocol)
	else
		--其他本服排行榜 本服排行榜在跨服是在跨服中的排行榜，而不是跨服排行榜
		self.data:SetRankAllInfo(protocol)
	end
end

--在跨服中的本服排行榜
function SocietyCtrl:OnGetCrossPersonRankListAck(protocol)
	local rank_type = protocol.info.rank_type
	if SocietyConfig.RankTypeRoleToC[rank_type] then
		self.data:SetGetCrossPersonRankListAck(protocol)
	elseif ActMeiLiQiXiData.RANK_TYPE == rank_type then
		ActMeiLiQiXiCtrl.Instance:OnGetCrossMeiLiBangRankList(protocol)
	else
		--其他本服排行榜 本服排行榜在跨服是在跨服中的排行榜，而不是跨服排行榜
		self.data:SetRankAllInfo(protocol)
	end
end

--跨服排行榜不是在跨服中的排行榜，而是全服排行榜
function SocietyCtrl:OnGetCrossPersonAllServerRankListAck(protocol)
	local rank_type = protocol.info.rank_type
	if BigDipperData.RANK_TYPE1 <= rank_type and  rank_type <= BigDipperData.RANK_TYPE8 then
		BigDipperCtrl.Instance:OnGetPersonRankListAck(protocol)
	elseif SocietyConfig.RankTypeRoleToC[rank_type] then
		self.data:SetGetCrossPersonAllServerRankListAck(protocol)
	elseif rank_type == PERSON_RANK_TYPE.FALLEN_GOD then
		FallenGodData.Instance:SetRankData(protocol)
	else
		--其他跨服排行榜 不是在跨服中的排行榜
		self.data:SetRankCrossInfo(protocol)
	end

	-- 如果上面的获取满足不了需求，可以在这里自己去定制所需的排行榜
	self.data:CrossPersonAllServerRankListOtherDeal(protocol)
end

function SocietyCtrl:OnGetPetRankListAck(protocol)
	self.data:SetnGetPetRankListAck(protocol)
end

function SocietyCtrl:OnCrossPersonRankBaseInfo(protocol)
	self.data:SetCrossPersonRankBaseInfo(protocol)
end

function SocietyCtrl:OnGetCrossPetAllServerRankListAck(protocol)
	self.data:SetGetCrossPetAllServerRankListAck(protocol)
end

function SocietyCtrl:OnGetCrossPetRankListAck(protocol)
	self.data:SetGetCrossPetRankListAck(protocol)
end

function SocietyCtrl:OnRankPetDetailedInfo(protocol)
	ViewMgr:OpenView(PetDetailView, protocol.pet)
end

function SocietyCtrl:OnChatRecordList(protocol)
	self.data:SetChatRecordList(protocol)
end

function SocietyCtrl:OnGiftRoleInfo(protocol)
	self.data:SetGiftRoleInfo(protocol)
end

function SocietyCtrl:OnGetGiftInfo(protocol)
	self.data:SetGetGiftInfo(protocol)
end

function SocietyCtrl:OnGiveFlowerNotice(protocol)
	if RoleSettingData.IsShield(ROLE_SETTING_TYPE.SYSTEM_SET_INFO_SHIELDING_FLOWERS) then
		self.data:PlayGiftEffect(1, 1)
	end
	self.data:SetGiveFlowerNotice(protocol)
end

function SocietyCtrl:OnFriendIntimacyListInfo(protocol)
	self.data:SetFriendIntimacyListInfo(protocol)
end

function SocietyCtrl:OnFriendIntimacyInfoChange(protocol)
	self.data:SetFriendIntimacyInfoChange(protocol)
end
function SocietyCtrl:SCShiTuOpNotice(protocol)
	if protocol.notice_type == 0 then
		local info = self.data:MasterOrApprenticeInfo(protocol.p1)
		if info then
			local role_info = {}
			role_info.info = function()
				return RoleInfoData.RoleInfoNew{func_info = info, uid = info.uid, name = info.name, level = info.level, prof = info.prof, family = info.family, avatar_type = info.avatar_type, avatar_id = info.avatar_id, top_level = info.top_level}
			end
			RoleInfoData.Instance:SetRoleInfo(role_info)
			ViewMgr:OpenView(SocietyMasterCooling)
		else
			LogDG("没获取到师徒角色信息")
		end
	elseif protocol.notice_type == 1 then
		PublicPopupCtrl.Instance:Center(Language.Society.Master.CancelTip)
	end
end

function SocietyCtrl:SendFriendReq(param_t)
	local protocol = GetProtocol(CSFriendReq)
	protocol.req_type = param_t.req_type or 0
	protocol.param1 = param_t.param1 or 0
	protocol.param2 = param_t.param2 or 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendFriendInfo()
	local param_t = {
		req_type = SocietyConfig.FriendOperType.info,
	}
	self:SendFriendReq(param_t)
end

function SocietyCtrl:SendFriendAdd(uid)
	local param_t = {
		req_type = SocietyConfig.FriendOperType.add_friend,
		param1 = uid,
	}
	self:SendFriendReq(param_t)
end

function SocietyCtrl:SendFriendApply(uid, is_agree)
	local param_t = {
		req_type = SocietyConfig.FriendOperType.reply,
		param1 = uid,
		param2 = is_agree and 1 or 0
    }
	self:SendFriendReq(param_t)
	self.data:FriendApplyRemoveByUid(uid)
	ViewMgr:FlushView(SocietyViewFriendApply)
end

function SocietyCtrl:SendFriendDelete(uid)
	local param_t = {
		req_type = SocietyConfig.FriendOperType.delete_friend,
		param1 = uid,
    }
	self:SendFriendReq(param_t)
end

function SocietyCtrl:SendBlackListAdd(uid)
	local param_t = {
		req_type = SocietyConfig.FriendOperType.add_black,
		param1 = uid,
    }
	self:SendFriendReq(param_t)
end

function SocietyCtrl:SendBlackListRemove(uid)
	local param_t = {
		req_type = SocietyConfig.FriendOperType.remove_black,
		param1 = uid,
    }
	self:SendFriendReq(param_t)
end

function SocietyCtrl:SendGroupDelete(group_type)
	local param_t = {
		req_type = SocietyConfig.FriendOperType.delete_group,
		param1 = SocietyConfig.FriendGroupToS[group_type or 1],
    }
	self:SendFriendReq(param_t)
end

function SocietyCtrl:SendAddRecentChat(uid)
	local param_t = {
		req_type = SocietyConfig.FriendOperType.add_recent_chat,
		param1 = uid,
    }
	self:SendFriendReq(param_t)
end

function SocietyCtrl:SendFlowerReq(uid,seq)
	local param_t = {
		req_type = SocietyConfig.FriendOperType.give_flower,
		param1 = uid,
		param2 = seq,
	}
	self:SendFriendReq(param_t)
end

function SocietyCtrl:SendFriendRecommendReq()
	local protocol = GetProtocol(CSFriendRecommendReq)
	SendProtocol(protocol)
end

function SocietyCtrl:SendFriendAddList(is_add_all)
	local protocol = GetProtocol(CSFriendRecommendAddOnListReq)
	protocol.is_add_all = is_add_all and 1 or 0
	protocol.add_index = 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendRefreshFriendRecommend()
	local protocol = GetProtocol(CSRefreshFriendRecommend)
	SendProtocol(protocol)
end

function SocietyCtrl:SendCreateGroup(param_t)
	local protocol = GetProtocol(CSCreateGroup)
	protocol.group_type = SocietyConfig.FriendGroupToS[param_t.group_type or 0]
	protocol.group_name = param_t.group_name or ""
	protocol.uid_list = param_t.uid_list or {}
	SendProtocol(protocol)
end

function SocietyCtrl:SendMoveGroup(param_t)
	local protocol = GetProtocol(CSMoveGroup)
	protocol.group_type = SocietyConfig.FriendGroupToS[param_t.group_type or 0]
	protocol.uid_list = param_t.uid_list or {}
	SendProtocol(protocol)
end

-- function SocietyCtrl:SendRenameGroup(param_t)
-- 	local protocol = GetProtocol(CSRenameGroup)
-- 	protocol.group_type = param_t.group_type or 0
-- 	protocol.group_name = param_t.group_name or ""
-- 	SendProtocol(protocol)
-- end

function SocietyCtrl:SendGetRoleGroupList()
	local protocol = GetProtocol(CSGetRoleGroupList)
	SendProtocol(protocol)
end

function SocietyCtrl:SendGetGroupInfo(group_id)
	local protocol = GetProtocol(CSGetGroupInfo)
	protocol.group_id = group_id or {}
	SendProtocol(protocol)
end

function SocietyCtrl:SendCreateGroupReq(is_cross, group_name)
	local protocol = GetProtocol(CSCreateGroupReq)
	protocol.is_cross = is_cross and 1 or 0
	protocol.group_name = group_name or ""
	SendProtocol(protocol)
end

function SocietyCtrl:SendRoleInviteEnterGroup(group_id, uid)
	local protocol = GetProtocol(CSRoleInviteEnterGroup)
	protocol.group_id = group_id or {}
	protocol.uid = uid or 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendEnterGroupReq(is_cross, server_id, invite_uid, group_id)
	local protocol = GetProtocol(CSEnterGroupReq)
	protocol.is_cross = is_cross or 0
	protocol.server_id = server_id or 0
	protocol.invite_uid = invite_uid or 0
	protocol.group_id = group_id or {}
	SendProtocol(protocol)
end

function SocietyCtrl:SendGroupKickMember(group_id, uid)
	local protocol = GetProtocol(CSGroupKickMember)
	protocol.group_id = group_id or {}
	protocol.uid = uid or 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendGroupDissmiss(group_id)
	local protocol = GetProtocol(CSGroupDissmiss)
	protocol.group_id = group_id or {}
	SendProtocol(protocol)
end

function SocietyCtrl:SendGroupLeaveReq(group_id, server_id)
	local protocol = GetProtocol(CSGroupLeaveReq)
	protocol.group_id = group_id or {}
	protocol.server_id = LoginData.Instance:GetLastLoginServer() or 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendChangeGroupNoticeReq(group_id, group_name, group_notice)
	local protocol = GetProtocol(CSChangeGroupNoticeReq)
	protocol.group_id = group_id or {}
	protocol.group_name = group_name or ""
	protocol.group_notice = group_notice or ""
	SendProtocol(protocol)
end

function SocietyCtrl:SendTempChatGroupBecomeOfficialReq(group_name)
	local protocol = GetProtocol(CSTempChatGroupBecomeOfficialReq)
	protocol.group_name = group_name or ""
	SendProtocol(protocol)
end

function SocietyCtrl:SendHandoverChatGroupLeadership(group_id, new_leader_uid)
	local protocol = GetProtocol(CSHandoverChatGroupLeadership)
	protocol.group_id = group_id or {}
	protocol.new_leader_uid = new_leader_uid or 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendRoleSystemSetChatGroupReq(group_id, set_value)
	local protocol = GetProtocol(CSRoleSystemSetChatGroupReq)
	protocol.group_id = group_id or {}
	protocol.set_value = set_value or 0
	SendProtocol(protocol)
end


function SocietyCtrl:SendFuzzySearchRoleByName(name)
	local protocol = GetProtocol(CSFuzzySearchRoleByName)
	protocol.name = name or ""
	SendProtocol(protocol)
end

function SocietyCtrl:SendShiTuMakeRelationReq(uid, seek_type)
	local protocol = GetProtocol(CSShiTuMakeRelationReq)
	protocol.type = seek_type or (self.data.is_master and SocietyConfig.ShiTuSeekingType.seek_apprentice or SocietyConfig.ShiTuSeekingType.seek_master)
	protocol.uid = uid or 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendShiTuMakeRelationAck(uid, is_agree, type)
	local protocol = GetProtocol(CSShiTuMakeRelationAck)
	protocol.type = 1 - type
	protocol.uid = uid
	protocol.is_agree = is_agree and 1 or 0
	SendProtocol(protocol)
	self.data:MasterApprenticeApplyRemoveByUid(uid, type)
	ViewMgr:FlushView(SocietyViewMasterApprenticeApply)
end

function SocietyCtrl:SendShiTuBreakRelationReq(type, uid, is_consume_gold, relation_type)
	local protocol = GetProtocol(CSShiTuBreakRelationReq)
	protocol.type = type or 0
	protocol.uid = uid
	-- protocol.is_force = is_force or 0
	protocol.is_consume_gold = is_consume_gold or 0
	protocol.relation_type = relation_type or 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendShiTuBreakRelationChuShi(uid)
	self:SendShiTuBreakRelationReq(0, uid, 0, 0)
end

function SocietyCtrl:SendShiTuBreakRelationBiYe(uid)
	self:SendShiTuBreakRelationReq(0, uid, 0, 1)
end

function SocietyCtrl:SendShiTuBreakRelationAck(type, uid, is_agree)
	local protocol = GetProtocol(CSShiTuBreakRelationAck)
	protocol.type = type
	protocol.uid = uid
	protocol.is_agree = is_agree and 1 or 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendShiTuReq(param_t)
	local protocol = GetProtocol(CSShiTuReq)
	protocol.req_type = param_t.req_type
	SendProtocol(protocol)
end

function SocietyCtrl:SendShiTuReqInfo()
	self:SendShiTuReq{
		req_type = SocietyConfig.ShiTuOperType.info,
	}
end

function SocietyCtrl:SendShiTuReqInvite()
	self:SendShiTuReq{
		req_type = SocietyConfig.ShiTuOperType.invite,
	}
end

function SocietyCtrl:SendShiTuReqFinish()
	self:SendShiTuReq{
		req_type = SocietyConfig.ShiTuOperType.finish,
	}
end

function SocietyCtrl:SendShiTuSeekingReq(param_t)
	local protocol = GetProtocol(CSShiTuSeekingReq)
	protocol.req_type = param_t.req_type or 0
	protocol.seeking_type = param_t.seeking_type or (self.data.is_master and SocietyConfig.ShiTuSeekingType.seek_apprentice or SocietyConfig.ShiTuSeekingType.seek_master)
	protocol.seeking_notice = param_t.seeking_notice or ""
	protocol.maq_info = param_t.maq_info or {}
	SendProtocol(protocol)
end

function SocietyCtrl:SendShiTuSeekingInfo()
	self:SendShiTuSeekingReq{
		req_type = SocietyConfig.ShiTuSeekingReqType.info,
		seeking_type = SocietyConfig.ShiTuSeekingType.seek_apprentice,
	}
	self:SendShiTuSeekingReq{
		req_type = SocietyConfig.ShiTuSeekingReqType.info,
		seeking_type = SocietyConfig.ShiTuSeekingType.seek_master,
	}
end

function SocietyCtrl:SendShiTuSeekingInsert(seeking_notice, seeking_type)
	local param_t = {
		req_type = SocietyConfig.ShiTuSeekingReqType.insert,
		seeking_notice = seeking_notice,
		seeking_type = seeking_type,
    }
	self:SendShiTuSeekingReq(param_t)
	self:SendShiTuSeekingInfo()
end

function SocietyCtrl:SendShiTuSeekingRemove(seeking_type)
	local param_t = {
		req_type = SocietyConfig.ShiTuSeekingReqType.remove,
		seeking_type = seeking_type,
    }
	self:SendShiTuSeekingReq(param_t)
	self:SendShiTuSeekingInfo()
end

function SocietyCtrl:SendShiTuSeekingPreference()
	local param_t = {
		req_type = SocietyConfig.ShiTuSeekingReqType.preference,
		maq_info = self.data.maq_options.jobs
	}
	self:SendShiTuSeekingReq(param_t)
end

function SocietyCtrl:SendShiTuChuanGongFBReq(param_t)
	local protocol = GetProtocol(CSShiTuChuanGongFBReq)
	protocol.req_type = param_t.req_type or 0
	protocol.param1 = param_t.param1 or 0
	protocol.param2 = param_t.param2 or 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendShiTuChuanGongFBReqGameReady()
	local param_t = {
		req_type = SocietyConfig.ShiTuChuanGongReqType.game_ready,
	}
	self:SendShiTuChuanGongFBReq(param_t)
end

function SocietyCtrl:SendShiTuChuanGongFBReqGameInfo(base_score, total_score)
	local param_t = {
		req_type = SocietyConfig.ShiTuChuanGongReqType.game_info,
		param1 = base_score,
		param2 = total_score,
	}
	self:SendShiTuChuanGongFBReq(param_t)
end

function SocietyCtrl:SendShiTuChuanGongFBReqFetchMile(uid, seq)
	local param_t = {
		req_type = SocietyConfig.ShiTuChuanGongReqType.fetch_mile,
		param1 = uid,
		param2 = seq,
	}
	self:SendShiTuChuanGongFBReq(param_t)
end

function SocietyCtrl:SendMailGetList()
	local protocol = GetProtocol(CSMailGetList)
	SendProtocol(protocol)
end

function SocietyCtrl:SendMailRead(mail_index)
	local protocol = GetProtocol(CSMailRead)
	protocol.mail_index = mail_index
	SendProtocol(protocol)
end

function SocietyCtrl:SendMailFetchAttachment(param_t)
	local protocol = GetProtocol(CSMailFetchAttachment)
	protocol.mail_index = param_t.mail_index
	protocol.item_index = param_t.item_index or -1
	protocol.is_last = param_t.is_last or 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendMailDelete(mail_index)
	local protocol = GetProtocol(CSMailDelete)
	protocol.mail_index = mail_index
	SendProtocol(protocol)
end

function SocietyCtrl:SendMailOneKeyFetchAttachment()
	local protocol = GetProtocol(CSMailOneKeyFetchAttachment)
	SendProtocol(protocol)
end

function SocietyCtrl:SendMailClean()
	local protocol = GetProtocol(CSMailClean)
	SendProtocol(protocol)
end

function SocietyCtrl:SendServerFirstKillReq(type,level_group)
	local protocol = GetProtocol(CSServerFirstKillReq)
	protocol.type = type or self.data.first_flood_sm.type
	protocol.level_group = level_group or self.data.first_flood_sm.level_id
	SendProtocol(protocol)
end

function SocietyCtrl:SendGetPersonRankList(rank_type,rank_end,special_type)
	local protocol = GetProtocol(CSGetPersonRankList)
	protocol.rank_type = rank_type
	protocol.special_type = special_type or 0
	protocol.rank_start = SocietyConfig.RANK_START
	protocol.rank_end = rank_end or SocietyConfig.RANK_END
	SendProtocol(protocol)
end

function SocietyCtrl:SendGetPetRankList(rank_type)
	local protocol = GetProtocol(CSGetPetRankList)
	protocol.rank_type = rank_type
	SendProtocol(protocol)
end

function SocietyCtrl:SendCrossGetPersonRankBaseInfo()
	local protocol = GetProtocol(CSCrossGetPersonRankBaseInfo)
	protocol.uuid = CommonStruct.UniqueUserID(0, 0)
	protocol.role_level = RoleData.Instance:GetRoleLevel()
	SendProtocol(protocol)
end

function SocietyCtrl:SendCrossGetPersonAllRankList(rank_type,rank_end)
	local protocol = GetProtocol(CSCrossGetPersonAllRankList)
	protocol.rank_type = rank_type
	protocol.last_snapshot_time = 0
	protocol.rank_start = SocietyConfig.RANK_START
	protocol.rank_end = rank_end or SocietyConfig.RANK_END
	SendProtocol(protocol)
end

function SocietyCtrl:SendMyRankShow()
	if LoginData.Instance:IsOnCrossSever() then
		local protocol = GetProtocol(CSCrossGetPersonRankList)
		protocol.usid = {}
		protocol.rank_type = SocietyConfig.PersonRankType.Cap
		protocol.last_snapshot_time = 0
		protocol.rank_start = 0
		protocol.rank_end = 0
		SendProtocol(protocol)
	else
		local protocol = GetProtocol(CSGetPersonRankList)
		protocol.rank_type = SocietyConfig.PersonRankType.Cap
		protocol.rank_start = 0
		protocol.rank_end = 0
		protocol.special_type =0
		SendProtocol(protocol)
	end
end

function SocietyCtrl:SendCrossGetPetAllRankList(rank_type)
	local protocol = GetProtocol(CSCrossGetPetAllRankList)
	protocol.rank_type = rank_type
	protocol.last_snapshot_time = 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendCrossGetPersonRankList(rank_type, server,rank_end)
	local protocol = GetProtocol(CSCrossGetPersonRankList)
	protocol.usid = server or self.data.rank_sm.server
	protocol.rank_type = rank_type or 0
	protocol.last_snapshot_time = 0
	protocol.rank_start = SocietyConfig.RANK_START
	protocol.rank_end = rank_end or SocietyConfig.RANK_END
	SendProtocol(protocol)
end

function SocietyCtrl:SendCrossGetPetRankList(rank_type, server)
	local protocol = GetProtocol(CSCrossGetPetRankList)
	protocol.usid = server or self.data.rank_sm.server
	protocol.rank_type = rank_type
	protocol.last_snapshot_time = 0
	SendProtocol(protocol)
end

function SocietyCtrl:SendGetRankPetDetailedInfo(uid, pet_unique_item_id)
	local protocol = GetProtocol(CSGetRankPetDetailedInfo)
	protocol.uid = uid
	protocol.rank_type = SocietyConfig.PetRankType.Cap
	protocol.pet_unique_item_id = pet_unique_item_id
	SendProtocol(protocol)
end

function SocietyCtrl:SendCrossGetRankPetDetailedInfo(uid, pet_unique_item_id, server)
	local protocol = GetProtocol(CSCrossGetRankPetDetailedInfo)
	protocol.uid = uid
	protocol.usid = server or self.data.rank_sm.server
	protocol.rank_type = SocietyConfig.PetRankType.Cap
	protocol.pet_unique_item_id = pet_unique_item_id
	SendProtocol(protocol)
end

function SocietyCtrl:SendCrossGetRankPetAllDetailedInfo(uid, pet_unique_item_id)
	local protocol = GetProtocol(CSCrossGetRankPetAllDetailedInfo)
	protocol.uid = uid
	protocol.rank_type = SocietyConfig.PetRankType.Cap
	protocol.pet_unique_item_id = pet_unique_item_id
	SendProtocol(protocol)
end

function SocietyCtrl:SendChatRecordSet(show_list)
	self.data.quick_chat_set = true
	local protocol = GetProtocol(CSChatRecordSet)
	protocol.show_list = show_list or {}
	SendProtocol(protocol)
end

function SocietyCtrl:SendGiftReq(param_t)
	local protocol = GetProtocol(CSGiftReq)
	protocol.req_type = param_t.req_type
	protocol.param1 = param_t.param1 or 0
	protocol.param2 = param_t.param2 or 0
	protocol.param3 = param_t.param3 or 0
	protocol.param4 = param_t.param4 or 0
	protocol.str = param_t.str or ""
	SendProtocol(protocol)
end

function SocietyCtrl:SendGiftReqGive(uid, item_id, num, str)
	local param_t = {
		req_type = SocietyConfig.GiftReqType.give,
		param1 = uid,
		param2 = item_id,
		param3 = num,
		str = str,
    }
	self:SendGiftReq(param_t)
end

function SocietyCtrl:SendGiftReqFetch(uuid)
	uuid = uuid or {}
	local param_t = {
		req_type = SocietyConfig.GiftReqType.fetch,
		param1 = uuid[1],
		param2 = uuid[2],
    }
	self:SendGiftReq(param_t)
end

function SocietyCtrl:SendGiftReqSendFlower(uid, item_id, num, is_auto_buy, str)
	local param_t = {
		req_type = SocietyConfig.GiftReqType.send_flower,
		param1 = uid,
		param2 = item_id,
		param3 = num,
		param4 = is_auto_buy and 1 or 0,
		str = str,
    }
	self:SendGiftReq(param_t)
end

function SocietyCtrl:SendGiftReqBackToKiss(uid)
	local param_t = {
		req_type = SocietyConfig.GiftReqType.back_to_kiss,
		param1 = uid,
    }
	self:SendGiftReq(param_t)
end


function SocietyCtrl:SendGetRankList()
	local server = self.data.rank_sm.server
	local next_time = self.data.rank_flush_times[self.data.rank_sm.type][server]
	if next_time and TimeHelper.GetServerTime() < next_time then return end
	if server.plat_type then
		if SocietyConfig.RankTypeRoleToS[self.data.rank_sm.type] then
			self:SendCrossGetPersonRankList(SocietyConfig.RankTypeRoleToS[self.data.rank_sm.type])
		elseif SocietyConfig.RankTypePetToS[self.data.rank_sm.type] then
			self:SendCrossGetPetRankList(SocietyConfig.RankTypePetToS[self.data.rank_sm.type])
		end
	elseif SocietyConfig.ServerSp.login == server.server_id then
		if LoginData.Instance:IsOnCrossSever() then
			if SocietyConfig.RankTypeRoleToS[self.data.rank_sm.type] then
				self:SendCrossGetPersonRankList(SocietyConfig.RankTypeRoleToS[self.data.rank_sm.type], {})
			elseif SocietyConfig.RankTypePetToS[self.data.rank_sm.type] then
				self:SendCrossGetPetRankList(SocietyConfig.RankTypePetToS[self.data.rank_sm.type], {})
			end
		else
			if SocietyConfig.RankTypeRoleToS[self.data.rank_sm.type] then
				self:SendGetPersonRankList(SocietyConfig.RankTypeRoleToS[self.data.rank_sm.type])
			elseif SocietyConfig.RankTypePetToS[self.data.rank_sm.type] then
				self:SendGetPetRankList(SocietyConfig.RankTypePetToS[self.data.rank_sm.type])
			end
		end
	elseif SocietyConfig.ServerSp.all == server.server_id then
		if SocietyConfig.RankTypeRoleToS[self.data.rank_sm.type] then
			self:SendCrossGetPersonAllRankList(SocietyConfig.RankTypeRoleToS[self.data.rank_sm.type])
		elseif SocietyConfig.RankTypePetToS[self.data.rank_sm.type] then
			self:SendCrossGetPetAllRankList(SocietyConfig.RankTypePetToS[self.data.rank_sm.type])
		end
	end
end

function SocietyCtrl:ClickToRoleVo(vo)
	if (vo.role_id or vo.uid) < TeamConfig.RobotUid then 
		PublicPopupCtrl.Instance:Center(Language.ItemInfo.RoleInfoDesc.RobotInfoHint)
		return 
	end
	if GuildData.Instance:IsOpenGuildMelee() then--and vo.guild_name and vo.guild_name ~= GuildData.Instance:GetGuildName() then
		BattleCtrl.ChallengeOthersReq(BATTLE_MODE.BATTLE_MODE_GUILD_FIGHT, vo.role_id or vo.uid)
		return
	end
	if OnlyFightData.Instance:IsInOnlyFight() then
		BattleCtrl.ChallengeOthersReq(BATTLE_MODE.BATTLE_MODE_ONLY_FIGHT, vo.role_id or vo.uid)
		return
	end
	if SceneData.Instance:GetMode() == SCENE_TYPE.CHIEF_PRIMARY then
		BattleCtrl.ChallengeOthersReq(BATTLE_MODE.BATTLE_MODE_CHIEF_PRIMARY, vo.role_id or vo.uid)
		return
	end
	if SceneData.Instance:GetMode() == SCENE_TYPE.TOMB_RAIDER then
		return 
	end
	if SceneData.Instance:GetMode() == SCENE_TYPE.HERO_VAN_BATTLE_FB then
		HeroVanBattleCtrl.Instance:SendTeamTeamArenaSceneReq(2, vo.role_id or vo.uid)
		return 
	end
    local info = {
        uid = vo.role_id or vo.uid,
        name = vo.name,
        level = vo.level,
        prof = vo.profession or vo.appearance.prof_base,
		family = vo.guild_name,
		avatar_type = vo.appearance.avatar_type,
		avatar_id = vo.appearance.avatar_id,
		avatar_quality = vo.appearance.avatar_quality,
		top_level = vo.top_level,
		kill_value = vo.kill_value
    }
    local society_data = SocietyCtrl.Instance:Data()
    local role_info_data = RoleInfoCtrl.Instance:Data()
	local config = DataHelper.TableCopy(SocietyConfig.RoleFunctions)
	if vo.behavior_state == ROLE_STATUS.FIGHT then
		table.insert(config, 4, "WatchBattle")
	end
	if not vo.pingbi_pk then
		local co = SceneData.Instance:GetSceneCfg()
		if co and SCENE_TYPE.COMMON_SCENE == co.scene_type then
			table.insert(config, 4, "PK")
		end
	end
	if DuelData.Instance:IsDuelScene() and vo.behavior_state ~= ROLE_STATUS.FIGHT and vo.kill_value ~= nil 
	and not vo.pingbi_pk and not EscortCtrl.Instance:IsEscorting() and DuelData.Instance:IsCanDuel(vo)
	and TeamData.Instance:TeamIndex() ~= vo.team_id then
		local team_is_red = vo.team_is_red == true
		if vo.escort_id ~= nil and vo.escort_id ~= 0 then
			table.insert(config, 5, "Escort")
		elseif (vo.kill_value ~= nil and vo.kill_value > 0) or team_is_red then
			table.insert(config, 5, "Sanction")
		else
			table.insert(config, 5, "Duel")
		end
	end

	if vo.open_in_member_view == true then
		info.post = vo.post
		config = DataHelper.TableCopy(SocietyConfig.GuildMemberFunctios)
		local mVo = GuildData.Instance:GetMemberVo()
		if vo.post == 0 then --and (mVo.post == 1 or mVo.post == 2) then
			table.remove(config, 4)
		else
			table.remove(config, 5)
		end
	end
	local is_friend = society_data:IsFriend(info.uid)
	local is_guildmelee = GuildData.Instance:IsOpenGuildMelee(true)
	local is_teamsport = TeamSportData.Instance:IsInTeamSport()
	local is_onlyfight = OnlyFightData.Instance:IsInOnlyFight(true)
	if nil == info.family or StringIsEmpty(info.family) and GuildCtrl.Instance:Data():GetGuildID() > 0 then table.insert(config, 3, "FamilyInvite") end
	table.insert(config, 3, is_friend and "DeleteFriend" or "AddFriend")
	local is_master = self.data:IsMaster()

	if not SocietyData.Instance:IsMasterOrApprentice(info.uid) then
		if is_master then
			table.insert(config, 3, "ApprenticeInvite")
		end
		table.insert(config, 3, "MasterInvite")
	end
	if is_guildmelee then
		config = DataHelper.TableCopy(SocietyConfig.GuildMeleeFunctions)
	end
	if is_teamsport then
		config = DataHelper.TableCopy(SocietyConfig.TeamSport)
	end
	if is_onlyfight then
		PublicPopupCtrl.Instance:Center(ErrorText[129])
		return
	end

    RoleInfoCtrl.Instance:RoleInfoView{
		info = function ()
				return RoleInfoData.RoleInfoNew{func_info = info, uid = info.uid, name = info.name, level = info.level, prof = info.prof, family = info.family, avatar_type = info.avatar_type, avatar_id = info.avatar_id,avatar_quality = info.avatar_quality, top_level = info.top_level}
			end,
		funcs = function()
			return config
		end
	}
end

function SocietyCtrl:ClickToRoleVoList(vo_list)
	RoleInfoCtrl.Instance:RoleInfoRoundView(vo_list)
end

function SocietyCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if nil == obj_vo then
        return
    end
    if obj_vo.seq == self.data:GetShiTuFBNpcSeq() then
		local button1 = NpcDialogBehBtnData.FuncCreate(
			Language.Society.SocietyShiTuFBTeam,
			function ()
				-- ViewMgr:OpenView(SocietyShiTuFBGuideView)
				if not TeamData.Instance:InTeam() then
					PublicPopupCtrl.Instance:Center(Language.Team.NumberNotEnoughShiTuFB)
					return
				end
				SceneCtrl.Instance:RequestEnterFb(FB_TYPE.SHITU_FB)
			end,
			true, NpcDialogData.CreateDetailInfo1{tips = Language.Society.ShiTuFB.DetailTips, rewards = self.data:GetShiTuFBDetailInfoRewardShow()}
		)
		if RoleData.Instance:GetRoleLevel() >= SocietyConfig.CHUANG_GONG_OPEN then
			local button2 = NpcDialogBehBtnData.FuncCreate(
				Language.Society.SocietyShiTuCGTeam,
				function ()
					-- ViewMgr:OpenView(SocietyShiTuFBGuideView)
					if not TeamData.Instance:InTeam() then
						PublicPopupCtrl.Instance:Center(Language.Team.NumberNotEnoughShiTuFB)
						return
					end
					SceneCtrl.Instance:RequestEnterFb(FB_TYPE.SHI_TU_CHUANG_GONG)
				end,
				true, NpcDialogData.CreateDetailInfo1{tips = Language.Society.ShiTuCG.DetailTips, rewards = self.data:GetShiTuCGDetailInfoRewardShow()}
			)
			-- button:SetIconType(NpcIconType.Act)
			table.insert(btn_list, 1, button2)
		end
		table.insert(btn_list, 1, button1)
    end
end

function SocietyCtrl:SendRankRefreshTimeReq()
	local protocol = GetProtocol(CSGetRankRefreshTime)
	SendProtocol(protocol)
end

function SocietyCtrl:OnGetRankRefreshTime(protocol)
	self.data:SetRankRefreshTime(protocol)
end