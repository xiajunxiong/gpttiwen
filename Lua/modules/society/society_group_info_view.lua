
SocietyGroupInfoView = SocietyGroupInfoView or DeclareView("SocietyGroupInfoView", SocietyConfig.ResPath .. "society_group_info")

function SocietyGroupInfoView:SocietyGroupInfoView()
    self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyGroupInfoView:LoadCallback()
    self.Board:SetData(self:GetType(), "", Vector2.New(905, 602))
end

SocietyGroupInfoViewF = SocietyGroupInfoViewF or DeclareMono("SocietyGroupInfoViewF", UIWFlushPanel)

function SocietyGroupInfoViewF:SocietyGroupInfoViewF()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society.ChatGroupInfo
	
	self.data_cares = {
		{data = self.data.chat_group_detail, func = self.FlushRoleList},
	}
end

function SocietyGroupInfoViewF:Awake()
    UIWFlushPanel.Awake(self)

	self.CrossType.text = self.language.CrossType[self.data.chat_group_detail.is_cross + 1]
	-- self.BtnRemind.isOn = 1 == self.data.cur_sel_chat_group:DisturbVal()
end

function SocietyGroupInfoViewF:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	-- SocietyCtrl.Instance:SendRoleSystemSetChatGroupReq(self.data.cur_sel_chat_group:GroupId(), self.BtnRemind.isOn and 1 or 0)
	-- RoleSettingCtrl.Instance:SaveRoleSettingInfo()
end

function SocietyGroupInfoViewF:FlushRoleList()
	-- if self.data.chat_group_detail.group_id ~= self.data.cur_sel_chat_group:GroupId() then return end
	self.GroupName.text = self.data.chat_group_detail.group_name
	self.MemberCount.text = string.format(self.language.ManageMemberCount, self.data.chat_group_detail.member_count)
	local role_list = self.data:GetFriendChatGroupMemberList()
	self.RoleList:SetData(role_list)
end


function SocietyGroupInfoViewF:OnClickDissmiss()
	local info = {
        content = string.format(Language.Society.ChatGroupTips.Dissmiss, self.data.chat_group_detail.group_name),
        confirm = {
            func = function()
				SocietyCtrl.Instance:SendGroupDissmiss(self.data.chat_group_detail.group_id)
                ViewMgr:CloseView(SocietyGroupManageView)
            end
		}
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

function SocietyGroupInfoViewF:OnClickNameModif()
	local word_num = DataHelper.GetStringWordNum(self.InputName.text)
    if 0 == word_num then
        PublicPopupCtrl.Instance:Center(self.language.CreateTips2)
        return
    end
    if word_num > SocietyConfig.CHAT_GROUP_NAME_NUM_MAX then
        PublicPopupCtrl.Instance:Center(self.language.CreateTips1)
        return
    end
	if ChatFilter.Instance:IsIllegal(self.InputName.text) then
        PublicPopupCtrl.Instance:Center(self.language.IllegalName)
        return
    end
	SocietyCtrl.Instance:SendChangeGroupNoticeReq(self.data.chat_group_detail.group_id, self.InputName.text, self.data.chat_group_detail.group_notice)
end

function SocietyGroupInfoViewF:OnClickNoticeModif()
	if ChatFilter.Instance:IsIllegal(self.InputNotice.text) then
        PublicPopupCtrl.Instance:Center(self.language.IllegalContent)
        return
    end
	SocietyCtrl.Instance:SendChangeGroupNoticeReq(self.data.chat_group_detail.group_id, self.data.chat_group_detail.group_name, self.InputNotice.text)
end

function SocietyGroupInfoViewF:OnClickRemind()
	
end

function SocietyGroupInfoViewF:OnClickShare()
	self.ShareObj:SetActive(true)
end

function SocietyGroupInfoViewF:OnClickCloseShare()
	self.ShareObj:SetActive(false)
end

function SocietyGroupInfoViewF:OnClickShareGuild()
	-- self.ShareObj:SetActive(false)
	-- if 0 == GuildData.Instance:GetGuildID() then
	-- 	PublicPopupCtrl.Instance:Center(self.language.NoGuildTips)
	-- else
	-- 	ChatCtrl.Instance:SendChannelChatText(self.data.cur_sel_chat_group:GroupInviteStr(), nil, ChatConfig.ChannelType.guild)
	-- end
end

function SocietyGroupInfoViewF:OnClickShareTeam()
	-- self.ShareObj:SetActive(false)
	-- if TeamData.Instance:InTeam() then
	-- 	ChatCtrl.Instance:SendChannelChatText(self.data.cur_sel_chat_group:GroupInviteStr(), nil, ChatConfig.ChannelType.team)
	-- else
	-- 	PublicPopupCtrl.Instance:Center(self.language.NoTeamTips)
	-- end
end


SocietyGroupInfoViewItem = DeclareMono("SocietyGroupInfoViewItem", UIWidgetBaseItem)

function SocietyGroupInfoViewItem:SocietyGroupInfoViewItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyGroupInfoViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data
	local prof_sp = RoleData.GetProfSp(info.prof)
	local is_online = 0 == info.member_logout_tamp
	self.Name.text = info.name
	self.Level.text = info.level
	self.ProfI.SpriteName = prof_sp
	self.ProfI:SetObjActive(nil ~= prof_sp)
	UH.SetAvatar(self.IconSp, info.avatar_type, info.avatar_type,info.avatar_quality)
	self.BtnMoreObj:SetActive(info.uid ~= RoleData.Instance:GetRoleId())
	self.MasterObj:SetActive(info.is_master)
	self.MaskObj:SetActive(not is_online)
	self.RoleState.text = is_online and Language.Society.RoleOnline or TimeHelper.Ago(info.member_logout_tamp)
end

function SocietyGroupInfoViewItem:OnClickMore()
	local info = self.data
	local config = SocietyConfig.ChatGroupFuncs
	local config = DataHelper.TableCopy(config[4])
	local is_friend = self.society_data:IsFriend(info.uid)
	local is_master = self.society_data:IsMaster()
	local num = self.society_data.master_apprentice_num
	local max_num = is_master and SocietyConfig.MasterApprenticeMax.apprentice or SocietyConfig.MasterApprenticeMax.master
	table.insert(config, 3, is_friend and "DeleteFriend" or "AddFriend")
	-- if max_num > num then
	-- 	table.insert(config, 3, is_master and "ApprenticeInvite" or "MasterInvite")
	-- end
	if not SocietyData.Instance:IsMasterOrApprentice(info.uid) then
		if is_master then
			table.insert(config, 3, "ApprenticeInvite")
		end
		table.insert(config, 3, "MasterInvite")
	end
	if GuildCtrl.Instance:Data():GetGuildID() > 0 then table.insert(config, 3, "FamilyInvite") end
	RoleInfoCtrl.Instance:RoleInfoView{
		info = function ()
				return RoleInfoData.RoleInfoNew{func_info = info, uid = info.uid, name = info.name, level = info.level, prof = info.prof, family = info.family, avatar_type = info.avatar_type, avatar_id = info.avatar_type, top_level = info.top_level}
			end,
		funcs = function()
			return config
		end
	}
end
