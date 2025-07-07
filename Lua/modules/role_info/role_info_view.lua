RoleInfoView = RoleInfoView or DeclareView("RoleInfoView","role_info/role_info")
VIEW_DECLARE_MASK(RoleInfoView,ViewMask.BlockClose)

function RoleInfoView:RoleInfoView()
	self.data = RoleInfoCtrl.Instance:Data()
	self.role_info = self.data:RoleInfo()
	self.info = self.data:RoleInfo().info
	self.func_info = self.info.func_info or {}
	self.auto_setting = {
		half_info_pos_width = 313 / 2,
		info_pos_height = {
			base = 130,
			family = 30,
			line = 20,
			per_button = 58,
		}
	}

	GuideManager.Instance:RegisterGetGuideUi("SocietyChatQuickAdd", function ()
		if self.Functions:SetupCompleted() and self.Functions.item_list[5] then
			return self.Functions.item_list[5].ButtonSp.gameObject
		end
	end)
end

function RoleInfoView:LoadCallback()
	local hide_base = self.role_info.hide_base
	if not hide_base then
		if RoleInfoType.role == self.info.type then
			local prof_type = RoleData.ProfIdToProfType(self.info.prof)
			local prof_stage = RoleData.ProfIdToProfStage(self.info.prof)
			local prof_sp = RoleData.GetProfSp(prof_type)
			self.Name.text = self.info.name
			if self.info.top_level and self.info.top_level > 0 then
				self.PeakLevel:SetActive(true)
				self.Level.text = self.info.top_level
				UH.Color(self.Level, COLORS.Red8)
			else
				self.PeakLevel:SetActive(false)
				UH.Color(self.Level, COLORS.White)
				self.Level.text = self.info.level
			end

			self.ProfI.SpriteName = prof_sp
			self.ProfI:SetObjActive(nil ~= prof_sp)
			self.Prof.text = RichTextHelper.ColorStr(RolePromoteData.GetProfName(self.info.prof_id or self.info.prof), QualityColorStr[prof_stage])
			if nil ~= self.info.uid then self.Uid.text = "ID:" .. self.info.uid end
			if nil ~= self.info.avatar_type then
				UH.SetAvatar(self.IconSp, self.info.avatar_type, self.info.avatar_id,self.info.avatar_quality)
				AvatarEffect.SetImageEnabled(self.PeakImg,self.info.avatar_quality)
			end
			if nil ~= self.info.family and not StringIsEmpty(self.info.family) then
				self.FamilyObj:SetActive(true)
				self.Family.text = string.format(Language.ItemInfo.RoleInfoDesc.family, self.info.family)
			end
			if self.func_info.rank_config then
				UH.SetText(self.Rank,Format(Language.Battlefield.RoleRank,BattlefieldData.GetRankName(self.func_info.rank_config,self.func_info.is_top_race or 0)))
			end
			self.RankGame:SetActive(self.func_info.rank_config ~= nil)
			self.RoleInfoObj:SetActive(true)
		elseif RoleInfoType.group == self.info.type then
			self.GroupName.text = self.info.group_name
			self.MemberCount.text = string.format(Language.Society.ChatGroupInfo.MemberCount, self.info.member_count, SocietyConfig.CHAT_GROUP_MEMBER_MAX)
			for i = 1, self.GroupAvatars:Length() do
				if 0 == self.info.group_avatars[i] then
					self.GroupAvatars[i].SpriteName = SocietyConfig.ChatGroupIconNone
				else
					UH.SetAvatar(self.GroupAvatars[i], self.info.group_avatars[i], self.info.group_avatars[i])
				end
			end
			self.GroupInfoObj:SetActive(true)
		end
	end
	self.UpObj:SetActive(not hide_base)
	self.LineImage.enabled = not hide_base
	self.button_show = nil ~= self.role_info.funcs and nil ~= next(self.role_info.funcs)
	self.LineObj:SetActive(self.button_show)
	self.input_t = {
		input_x = UnityEngine.Input.mousePosition.x / UnityEngine.Screen.width * self.ViewSize.rect.width,
		input_y = UnityEngine.Input.mousePosition.y / UnityEngine.Screen.height * self.ViewSize.rect.height,
	}
	local button_num = self.button_show and #self.role_info.funcs or 0
	if nil ~= self.role_info.pos then
		self.InfoPosition.position = Vector2.New(self.role_info.pos.x, self.role_info.pos.y)
		if hide_base and  1 == button_num then
			self.InfoPosition.localPosition = Vector3.New(self.InfoPosition.localPosition.x + 72, self.InfoPosition.localPosition.y, 0)
			self.InfoPosition.sizeDelta = Vector2.New(169, 0)
			self.FuncsLayout.constraintCount = 1
		end
	else
		self:AutoToPos(button_num)
	end
	self.Functions:SetData(self.button_show and self:CheckButtons(self.role_info.funcs) or {})
end

function RoleInfoView:CheckButtons(buttons)
	local function sp_condition(button)
		local value = false
		if TeamData.Instance:MemberInTeam(self.info.name) and "TeamInvite" == button then value = true end
		return value
	end
	local list = {}
	for _, button in ipairs(buttons or {}) do
		if not sp_condition(button) then
			table.insert(list, button)
		end
	end
	return list
end

function RoleInfoView:OnClickBlank()
	ViewMgr:CloseView(RoleInfoView)
end

function RoleInfoView:AutoToPos(button_num)
	local pos_t = {
		input_x = self.input_t.input_x,
		input_y = self.input_t.input_y,
		view_w = self.ViewSize.rect.width,
		view_h = self.ViewSize.rect.height,
		x = 0,
		y = 0,
	}

	local info_pos_height = self.auto_setting.info_pos_height
	local info_h = info_pos_height.base
		+ ((nil ~= self.info.family and not StringIsEmpty(self.info.family)) and info_pos_height.family or 0)
		+ (button_num > 0 and info_pos_height.line or 0)
		+ math.floor((button_num + 1 ) / 2) * info_pos_height.per_button
	local y = pos_t.input_y - pos_t.view_h/2
	pos_t.y = info_h < pos_t.input_y and y or y + info_h - pos_t.input_y

	local plus_x = {
		right = self.auto_setting.half_info_pos_width,
		left =  self.auto_setting.half_info_pos_width
	}                                                                                                
	if pos_t.input_x + plus_x.right > pos_t.view_w then
		pos_t.x = pos_t.input_x - (pos_t.input_x + plus_x.right - pos_t.view_w)
	elseif pos_t.input_x - plus_x.left < 0 then
		pos_t.x = pos_t.input_x - (pos_t.input_x - plus_x.left)
	else
		pos_t.x = pos_t.input_x
	end
	pos_t.x = pos_t.x - pos_t.view_w/2
	
	self.InfoPosition.localPosition = Vector3.New(pos_t.x, pos_t.y, 0)
end

RoleInfoViewFuncItem = DeclareMono("RoleInfoViewFuncItem", UIWidgetBaseItem)

function RoleInfoViewFuncItem:RoleInfoViewFuncItem()
	self.info_data = RoleInfoCtrl.Instance:Data()
	self.info = self.info_data:RoleInfo().info
	self.func_info = self.info.func_info
	self.language = Language.ItemInfo.RoleButtonName
	self.society_data = SocietyCtrl.Instance:Data()
end

function RoleInfoViewFuncItem:SetData(data)
	local btn_flag = true
	if data == "Accuse" then
		if GuildData.Instance:CanAccuseGuildLeader() then
			btn_flag = GuildData.Instance.guild_info.is_join_tan_he_patriarch ~= 1
			self.custom_name = GuildData.Instance.guild_info.is_join_tan_he_patriarch == 1 and self.language.AccuseSucc or nil
		else
			btn_flag = false
		end			
	end
	self.Interactor.Interactable = btn_flag
	UIWidgetBaseItem.SetData(self, data)
	self.Name.text = StringIsEmpty(self.custom_name) and self.language[self.data] or self.custom_name
	local type = ItemInfoConfig.RoleInfoButton[data]
	if nil == type then return end
	local config = ItemInfoConfig.ButtonConfig[type]
	self.Name.color = config.LbColor
	self.ButtonSp.SpriteName = config.SpName
end

function RoleInfoViewFuncItem:OnClick()
	self[self.data](self)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

-- 查看信息
function RoleInfoViewFuncItem:ViewEquip()
	self.info_data.view_equip_uid = self.func_info.uid
	RoleInfoCtrl.Instance:SendReferRoleEquipment(self.func_info.uid)
	-- ViewMgr:OpenView(RoleInfoEquipView)
	ViewMgr:CloseView(RoleInfoView)
end

-- 聊天
function RoleInfoViewFuncItem:Chat()
	SocietyCtrl.Instance:SendAddRecentChat(self.func_info.uid)
	self.society_data:SetRecentlyChatRole(self.func_info.uid)
	ViewMgr:CloseView(RoleInfoView)
end

-- 移交队长
function RoleInfoViewFuncItem:GiveLeader()
	TeamCtrl.Instance:SendChangeTeamLeader(self.func_info.member_index)
	ViewMgr:CloseView(RoleInfoView)
end

-- 队友召回
function RoleInfoViewFuncItem:TeamSummon()
	TeamCtrl.Instance:SendTeamLeaderSummonMember(self.func_info.uid)
	ViewMgr:CloseView(RoleInfoView)
end

-- 移出队伍
function RoleInfoViewFuncItem:KickOut()
	TeamCtrl.Instance:SendKickOutOfTeamReq(self.func_info.member_index)
	ViewMgr:CloseView(RoleInfoView)
end

-- 伙伴休息
function RoleInfoViewFuncItem:PartnerRest()
	PartnerCtrl.Instance:SendPartnerFight(self.func_info.uid)
	ViewMgr:CloseView(RoleInfoView)
end

-- 删除好友
function RoleInfoViewFuncItem:DeleteFriend()
	if MarryData.Instance:GetHusbandOrWifeUid() == self.func_info.uid then
		PublicPopupCtrl.Instance:Center(Language.Marry.ErrorText[10])
		return
	end
	local info = {
        content = string.format(Language.Society.FriendFuncTips.delete_friend, self.func_info.name),
        confirm = {
            func = function()
				SocietyCtrl.Instance:SendFriendDelete(self.func_info.uid)
				ViewMgr:CloseView(RoleInfoView)
                ViewMgr:CloseView(DialogTipsView)
            end
		}
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

-- 黑名单
function RoleInfoViewFuncItem:AddBlackList()
	local info = {
        content = string.format(Language.Society.FriendFuncTips.add_black, self.func_info.name),
        confirm = {
            func = function()
				SocietyCtrl.Instance:SendBlackListAdd(self.func_info.uid)
				ViewMgr:CloseView(RoleInfoView)
                ViewMgr:CloseView(DialogTipsView)
            end
		}
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

-- 移出列表
function RoleInfoViewFuncItem:RomveGroup()
	SocietyCtrl.Instance:SendMoveGroup{group_type = SocietyConfig.FriendGroupType.friend, uid_list = {self.func_info.uid}}
	ViewMgr:CloseView(RoleInfoView)
end

-- 举报
function RoleInfoViewFuncItem:Report()
	-- Debuger.LogError("Report")
	self.society_data:CurSelRole(self.func_info)
	ViewMgr:OpenView(SocietyViewReport)
	ViewMgr:CloseView(RoleInfoView)
end

-- 拜师
function RoleInfoViewFuncItem:MasterInvite()
	SocietyCtrl.Instance:SendShiTuMakeRelationReq(self.func_info.uid, SocietyConfig.ShiTuSeekingType.seek_master)
end

-- 收徒
function RoleInfoViewFuncItem:ApprenticeInvite()
	SocietyCtrl.Instance:SendShiTuMakeRelationReq(self.func_info.uid, SocietyConfig.ShiTuSeekingType.seek_apprentice)
end

-- 组队邀请
function RoleInfoViewFuncItem:TeamInvite()
	ViewMgr:CloseView(RoleInfoView)
	TeamCtrl.Instance:OnTeamInvitationReq(self.func_info.uid)
end

-- 组队邀请
function RoleInfoViewFuncItem:Invite()
	ViewMgr:CloseView(RoleInfoView)
	TeamCtrl.Instance:OnTeamInvitationReq(self.func_info.uid)
end

-- 家族邀请
function RoleInfoViewFuncItem:FamilyInvite()
	-- 家族id和对方uid
	local level = self.func_info.level or 0
	local need_level = 0
	local funopen_config = Config.funopen_auto.funopen
	for _, v in pairs(funopen_config) do
		if v.seq == 24 then
			need_level = v.level
			break
		end
    end
	if level > 0 and level < need_level then
		local show_tip = Language.Guild.CantInviteOtherPlayer
		PublicPopupCtrl.Instance:Center(show_tip)
		return
	end
	local param_t = {
		guild_id = GuildData.Instance:GetGuildID(),
		uid = self.func_info.uid,
	}
	GuildCtrl.SendGuildInvitationReq(param_t)
end

-- 加为好友
function RoleInfoViewFuncItem:AddFriend()
	-- Debuger.LogError("AddFriend")
	local is_online = self.func_info.is_online or 1
	if is_online ~= 1 then
		PublicPopupCtrl.Instance:Center(Language.Society.CommonTips.NotOnline)
		return
    end
	SocietyCtrl.Instance:SendFriendAdd(self.func_info.uid)
	ViewMgr:CloseView(RoleInfoView)
end

-- 赠送鲜花
function RoleInfoViewFuncItem:SendFlower()
	local is_online = self.func_info.is_online or 1
	if is_online ~= 1 then
		PublicPopupCtrl.Instance:Center(Language.Society.CommonTips.NotOnline)
		return
	end
	if self.func_info.sex == RoleData.Instance:GetRoleSex() then
		PublicPopupCtrl.Instance:Center(Language.Society.SendFlowerSexTip)
		return
	end
	ViewMgr:OpenView(SocietyViewFlower)
	ViewMgr:CloseView(RoleInfoView)
end

-- 出师
function RoleInfoViewFuncItem:ChuShi()
	-- Debuger.LogError("ChuShi")
	SocietyCtrl.Instance:SendShiTuBreakRelationChuShi(self.func_info.uid)
	ViewMgr:CloseView(RoleInfoView)
end

-- 毕业
function RoleInfoViewFuncItem:BiYe()
	-- Debuger.LogError("BiYe")
	SocietyCtrl.Instance:SendShiTuBreakRelationBiYe(self.func_info.uid)
	ViewMgr:CloseView(RoleInfoView)
end

-- 切磋
function RoleInfoViewFuncItem:PK()
	-- BattleCtrl.ChallengeOthersReq(BATTLE_MODE.BATTLE_MODE_NULL, self.func_info.uid)
	BattleCtrl.BattleQieCuoWithOthers(self.func_info.uid)
	ViewMgr:CloseView(RoleInfoView)
end

-- 观战
function RoleInfoViewFuncItem:WatchBattle()
	BattleCtrl.Instance:SendWatchBattleReq(self.func_info.uid)
	ViewMgr:CloseView(RoleInfoView)
end

-- 离开队伍
function RoleInfoViewFuncItem:ExitTeam()
	local flag = GuildData.Instance:IsOpenGuildMelee()
	if flag == true then
		PublicPopupCtrl.Instance:Center(ErrorText[158])
		return
	end
	TeamCtrl.Instance:ExitTeam()
	ViewMgr:CloseView(RoleInfoView)
end

-- 暂离队伍
function RoleInfoViewFuncItem:TempLeaveTeam()
	local flag = GuildData.Instance:IsOpenGuildMelee()
	if flag == true then
		PublicPopupCtrl.Instance:Center(ErrorText[158])
		return
	end
	TeamCtrl.Instance:SendTemporarilyPartReq()
	ViewMgr:CloseView(RoleInfoView)
end

-- 回归队伍
function RoleInfoViewFuncItem:BackTeam()
	TeamCtrl.Instance:SendTemporarilyPartReq()
	ViewMgr:CloseView(RoleInfoView)
end

-- 调整站位
function RoleInfoViewFuncItem:TeamSort()
	ViewMgr:OpenView(FormationView)
	ViewMgr:CloseView(RoleInfoView)
end

-- 申请带队
function RoleInfoViewFuncItem:TeamLedApply()
	TeamCtrl.Instance:SendTeamLaunchLeaderVote()
	ViewMgr:CloseView(RoleInfoView)
end

-- 使用药水
function RoleInfoViewFuncItem:UseHeal()
	ViewMgr:OpenView(BagView, {col = ItemColumnType.Supply})
	ViewMgr:CloseView(RoleInfoView)
end

-- 解除屏蔽
function RoleInfoViewFuncItem:Unblock()
	PublicPopupCtrl.Instance:DialogTips({
		content = string.format(Language.Society.FriendFuncTips.delete_black, self.func_info.name),
        confirm = {
			func = function()
				SocietyCtrl.Instance:SendBlackListRemove(self.func_info.uid)
				ViewMgr:CloseView(RoleInfoView)
				ViewMgr:CloseView(DialogTipsView)
            end
		}
    })
end

--任命
function RoleInfoViewFuncItem:ApplyPost()
	local mVo = GuildData.Instance:GetMemberVo()
    if mVo then
        local job_info = GuildData.Instance:GetJobInfo(mVo.post)
		if job_info.appoint ~= 1 then
			PublicPopupCtrl.Instance:Center(ErrorText[148])
			return
		end
	end
	if LoginData.Instance:IsOnCrossSever() then
        local param_t = {
        content = Language.Common.CrossTips,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    LoginCtrl:SendBackToOriginServer()
                end
            },
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
        return
    end
	GuildData.Instance:SetSelectMember(self.func_info)
	ViewMgr:OpenView(GuildPost)
end

--弹劾
function RoleInfoViewFuncItem:Accuse()
	if not GuildData.Instance:CanAccuseGuildLeader() then
		PublicPopupCtrl.Instance:Center(Language.Guild.AccuseTips)
		return
	elseif GuildData.Instance.guild_info.is_join_tan_he_patriarch == 1 then
		PublicPopupCtrl.Instance:Center(Language.Guild.Accused)
		return
	end
	local mVo = GuildData.Instance:GetMemberVo()
	if self.func_info.post == 0 then
		GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.TAN_HE_ZU_ZHANG})
		ViewMgr:CloseView(RoleInfoView)
	end
end
--踢出家族
function RoleInfoViewFuncItem:KickOutG()
	local mVo = GuildData.Instance:GetMemberVo()
    if mVo then
        local job_info = GuildData.Instance:GetJobInfo(mVo.post)
		if job_info.kick ~= 1 then
			PublicPopupCtrl.Instance:Center(ErrorText[148])
			return
		end
    end
	if self.func_info.uid == RoleData.Instance:GetRoleId() then
        PublicPopupCtrl.Instance:Center(Language.Guild.GuildMemberOutNot)
        return
    end
	if LoginData.Instance:IsOnCrossSever() then
        local param_t = {
        content = Language.Common.CrossTips,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    LoginCtrl:SendBackToOriginServer()
                end
            },
        }
        PublicPopupCtrl.Instance:DialogTips(param_t)
        return
    end
    PublicPopupCtrl.Instance:AlertTip(Format(Language.Guild.GuildMemberOutTip,self.func_info.name),function()
        local param_t = {
            guild_id = GuildData.Instance:GetGuildID(),
            expel_uid = self.func_info.uid
        }
        GuildCtrl.SendGuildExpelOutReq(param_t)
		GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_MEMBER_LIST})
		ViewMgr:CloseView(RoleInfoView)
    end)
end

--管理群组
function RoleInfoViewFuncItem:GroupManage()
	SocietyCtrl.Instance:SendGetGroupInfo(self.func_info:GroupId())
	ViewMgr:OpenView(SocietyGroupManageView)
	ViewMgr:CloseView(RoleInfoView)
end

--群组信息
function RoleInfoViewFuncItem:GroupInfo()
	SocietyCtrl.Instance:SendGetGroupInfo(self.func_info:GroupId())
	ViewMgr:OpenView(SocietyGroupInfoView)
	ViewMgr:CloseView(RoleInfoView)
end

--邀请好友进群组
function RoleInfoViewFuncItem:GroupInvite()
	SocietyCtrl.Instance:SendGetGroupInfo(self.func_info:GroupId())
	ViewMgr:OpenView(SocietyGroupInviteView)
	ViewMgr:CloseView(RoleInfoView)
end

--退出群组
function RoleInfoViewFuncItem:GroupExit()
	local info = {
        content = string.format(Language.Society.ChatGroupTips.Exit, self.func_info:GroupName()),
        confirm = {
            func = function()
				SocietyCtrl.Instance:SendGroupLeaveReq(self.func_info:GroupId())
				ViewMgr:CloseView(RoleInfoView)
                ViewMgr:CloseView(DialogTipsView)
            end
		}
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

--解散群组
function RoleInfoViewFuncItem:GroupDissmiss()
	local info = {
        content = string.format(Language.Society.ChatGroupTips.Dissmiss, self.func_info:GroupName()),
        confirm = {
            func = function()
				SocietyCtrl.Instance:SendGroupDissmiss(self.func_info:GroupId())
				ViewMgr:CloseView(RoleInfoView)
                ViewMgr:CloseView(DialogTipsView)
            end
		}
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

--分享群组
function RoleInfoViewFuncItem:GroupShare()
	local info = {
        content = Language.Society.ChatGroupTips.ShareContent,
        confirm = {
			name = Language.Society.ChatGroupTips.ShareGuild,
            func = function()
				if 0 == GuildData.Instance:GetGuildID() then
					PublicPopupCtrl.Instance:Center(Language.Society.ChatGroupInfo.NoGuildTips)
				else
					local group = SocietyData.Instance.cur_sel_chat_group
					if not ViewMgr:IsOpen(SocietyView) then
						local cur_sel = SocietyData.Instance.quick_chat_info.cur_sel
						group = SocietyData.Instance.chat_group_list[cur_sel]
					end
					ChatCtrl.Instance:SendChannelChatText(group:GroupInviteStr(), nil, ChatConfig.ChannelType.guild)
					PublicPopupCtrl.Instance:Center(Language.Society.ShareTips)
				end
				ViewMgr:CloseView(RoleInfoView)
				ViewMgr:CloseView(DialogTipsView)
            end
		},
		cancel = {
			name = Language.Society.ChatGroupTips.ShareTeam,
            func = function()
				if TeamData.Instance:InTeam() then
					local group = SocietyData.Instance.cur_sel_chat_group
					if not ViewMgr:IsOpen(SocietyView) then
						local cur_sel = SocietyData.Instance.quick_chat_info.cur_sel
						group = SocietyData.Instance.chat_group_list[cur_sel]
					end
					ChatCtrl.Instance:SendChannelChatText(group:GroupInviteStr(), nil, ChatConfig.ChannelType.team)
					PublicPopupCtrl.Instance:Center(Language.Society.ShareTips)
				else
					PublicPopupCtrl.Instance:Center(Language.Society.ChatGroupInfo.NoTeamTips)
				end
				ViewMgr:CloseView(RoleInfoView)
				ViewMgr:CloseView(DialogTipsView)
            end
		}
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

--消息免打扰
function RoleInfoViewFuncItem:GroupNoDisturb()
	SocietyCtrl.Instance:SendRoleSystemSetChatGroupReq(self.func_info:GroupId(), 0)
	RoleSettingCtrl.Instance:SaveRoleSettingInfo()
	ViewMgr:CloseView(RoleInfoView)
	PublicPopupCtrl.Instance:Center(Language.Society.ChatGroupTips.NoDisturbTips)
end

--关闭免打扰
function RoleInfoViewFuncItem:GroupNoDisturbClose()
	SocietyCtrl.Instance:SendRoleSystemSetChatGroupReq(self.func_info:GroupId(), 1)
	RoleSettingCtrl.Instance:SaveRoleSettingInfo()
	ViewMgr:CloseView(RoleInfoView)
	PublicPopupCtrl.Instance:Center(Language.Society.ChatGroupTips.NoDisturbCloseTips)
end

-- 移交群主
function RoleInfoViewFuncItem:GroupHandover()
	SocietyCtrl.Instance:SendHandoverChatGroupLeadership(SocietyData.Instance.cur_sel_chat_group:GroupId(), self.func_info.uid)
    ViewMgr:CloseView(RoleInfoView)
end

-- 踢出群组
function RoleInfoViewFuncItem:GroupKickOut()
	SocietyCtrl.Instance:SendGroupKickMember(SocietyData.Instance.cur_sel_chat_group:GroupId(), self.func_info.uid)
    ViewMgr:CloseView(RoleInfoView)
end

--快捷聊天
function RoleInfoViewFuncItem:QuickChat()
	if SocietyData.Instance:QuickChatAdd(self.func_info.uid and SocietyConfig.QuickShowType.role or SocietyConfig.QuickShowType.group, self.func_info.uid or self.func_info.group_key) then
		ViewMgr:CloseView(SocietyView)
	end
	ViewMgr:CloseView(RoleInfoView)
end

--取消快捷
function RoleInfoViewFuncItem:QuickChatDel()
	SocietyData.Instance:DeleteQuickChatShowVal(self.func_info.uid or self.func_info.group_key)
	ViewMgr:CloseView(RoleInfoView)
end

--决斗
function RoleInfoViewFuncItem:Duel()
	local item_id = DuelData.Instance:GetDuelToken()
	local item_num = Item.GetNum(item_id)
	if item_num > 0 then
		local info = {
			content = string.format(Language.Duel.DuelTips1, self.info.name),
			confirm = {
				func = function()
					BattleCtrl.ChallengeOthersReq(BATTLE_MODE.BATTLE_MODE_DUEL, self.func_info.uid)
					PublicPopupCtrl.Instance:CloseDialogTips()
				end
			}
		}
		PublicPopupCtrl.Instance:DialogTips(info)
	else
		local shop_cfg = StoreData.Instance:GetItemConfig(item_id, GOLD_STORE_TYPE)
		local info = {
			content = string.format(Language.Duel.DuelTips2, self.info.name),
			confirm = {
				func = function()
					if Item.GetNum(item_id) < 1 then
						if shop_cfg ~= nil then
							if not MallCtrl.IsNotCurrency(CurrencyType.Gold, shop_cfg.price) then
								return
							end
							local data = TableCopy(shop_cfg, 1)
							data.make_num = 1
							ShoppingCtrl.Instance:SendStoreBuyReq(data)
						end
					end
					BattleCtrl.ChallengeOthersReq(BATTLE_MODE.BATTLE_MODE_DUEL, self.func_info.uid)
					ViewMgr:CloseView(DuelDialogView)
				end
			},
			item_info = {
				item = {item_id = item_id, num = 1, is_bind = 0},
				cost = shop_cfg.price
			}
		}
		ViewMgr:OpenView(DuelDialogView, info)
	end
end

--制裁
function RoleInfoViewFuncItem:Sanction()
	local info = {
		content = string.format(Language.Duel.SanctionTips1, self.info.name),
		confirm = {
			func = function()
				PublicPopupCtrl.Instance:CloseDialogTips()
				BattleCtrl.ChallengeOthersReq(BATTLE_MODE.BATTLE_MODE_DUEL, self.func_info.uid)
			end
		}
	}
	PublicPopupCtrl.Instance:DialogTips(info)
end

--劫镖
function RoleInfoViewFuncItem:Escort()
	local item_id = DuelData.Instance:GetDuelToken()
	local item_num = Item.GetNum(item_id)
	if item_num > 0 then
		local info = {
			content = string.format(Language.Duel.EscortTips2, self.info.name),
			confirm = {
				func = function()
					PublicPopupCtrl.Instance:CloseDialogTips()
					BattleCtrl.ChallengeOthersReq(BATTLE_MODE.BATTLE_MODE_DUEL, self.func_info.uid)
				end
			}
		}
		PublicPopupCtrl.Instance:DialogTips(info)
	else
		local shop_cfg = StoreData.Instance:GetItemConfig(item_id, GOLD_STORE_TYPE)
		local info = {
			content = string.format(Language.Duel.EscortTips3, self.info.name),
			confirm = {
				func = function()
					if Item.GetNum(item_id) < 1 then
						if shop_cfg ~= nil then
							if not MallCtrl.IsNotCurrency(CurrencyType.Gold, shop_cfg.price) then
								return
							end
							local data = TableCopy(shop_cfg, 1)
							data.make_num = 1
							ShoppingCtrl.Instance:SendStoreBuyReq(data)
						end
					end
					BattleCtrl.ChallengeOthersReq(BATTLE_MODE.BATTLE_MODE_DUEL, self.func_info.uid)
					ViewMgr:CloseView(DuelDialogView)
				end
			},
			item_info = {
				item = {item_id = item_id, num = 1, is_bind = 0},
				cost = shop_cfg.price
			}
		}
		ViewMgr:OpenView(DuelDialogView, info)
	end
end

function RoleInfoViewFuncItem:RelationBroken()
	--如果有冷静期就显示冷静期没有就显示两按钮
	if self.func_info.relation_break_time_type == 1 and self.func_info.relation_break_time > 0 then
		ViewMgr:OpenView(SocietyMasterCooling)
		ViewMgr:CloseView(RoleInfoView)
		return
	end
	ViewMgr:OpenView(SocietyMasterRelationBroken)
	ViewMgr:CloseView(RoleInfoView)
end