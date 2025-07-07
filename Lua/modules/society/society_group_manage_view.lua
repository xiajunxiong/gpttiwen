
SocietyGroupManageView = SocietyGroupManageView or DeclareView("SocietyGroupManageView", SocietyConfig.ResPath .. "society_group_manage")

function SocietyGroupManageView:SocietyGroupManageView()
    self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society
end

function SocietyGroupManageView:LoadCallback()
    self.Board:SetData(self:GetType(), "", Vector2.New(905, 602))
end

SocietyGroupManageViewF = SocietyGroupManageViewF or DeclareMono("SocietyGroupManageViewF", UIWFlushPanel)

function SocietyGroupManageViewF:SocietyGroupManageViewF()
	self.data = SocietyCtrl.Instance:Data()
	self.language = Language.Society.ChatGroupInfo
	
	self.data_cares = {
		{data = self.data.chat_group_detail, func = self.FlushRoleList},
	}
end

function SocietyGroupManageViewF:Awake()
    UIWFlushPanel.Awake(self)
	self.CrossType.text = self.language.CrossType[self.data.chat_group_detail.is_cross + 1]
	-- self.BtnRemind.isOn = 1 == self.data.cur_sel_chat_group:DisturbVal()
end

function SocietyGroupManageViewF:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	-- SocietyCtrl.Instance:SendRoleSystemSetChatGroupReq(self.data.chat_group_detail.group_id, self.BtnRemind.isOn and 1 or 0)
	-- RoleSettingCtrl.Instance:SaveRoleSettingInfo()
end

function SocietyGroupManageViewF:FlushRoleList()
	-- if self.data.chat_group_detail.group_id ~= self.data.cur_sel_chat_group:GroupId() then return end
	self.GroupName.text = self.data.chat_group_detail.group_name
	self.MemberCount.text = string.format(self.language.ManageMemberCount, self.data.chat_group_detail.member_count)
	local role_list = self.data:GetFriendChatGroupMemberList()
	self.RoleList:SetData(role_list)
end


function SocietyGroupManageViewF:OnClickDissmiss()
	local info = {
        content = string.format(Language.Society.ChatGroupTips.Dissmiss, self.data.chat_group_detail.group_name),
        confirm = {
            func = function()
				SocietyCtrl.Instance:SendGroupDissmiss(self.data.chat_group_detail.group_id)
				ViewMgr:CloseView(DialogTipsView)
                ViewMgr:CloseView(SocietyGroupManageView)
            end
		}
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

function SocietyGroupManageViewF:OnClickNameModif()
	local word_num = DataHelper.GetStringWordNum(self.InputName.text)
    if 0 == word_num then
        PublicPopupCtrl.Instance:Center(self.language.CreateTips3)
        return
    end
    if word_num > SocietyConfig.CHAT_GROUP_NAME_NUM_MAX then
        PublicPopupCtrl.Instance:Center(string.format(self.language.CreateTips5, SocietyConfig.CHAT_GROUP_NAME_NUM_MAX))
        return
    end
	if ChatFilter.Instance:IsIllegal(self.InputName.text) then
        PublicPopupCtrl.Instance:Center(self.language.IllegalName)
        return
    end
	SocietyCtrl.Instance:SendChangeGroupNoticeReq(self.data.chat_group_detail.group_id, self.InputName.text, self.data.chat_group_detail.group_notice)
end

function SocietyGroupManageViewF:OnClickNoticeModif()
	local word_num = DataHelper.GetStringWordNum(self.InputNotice.text)
    if 0 == word_num then
        PublicPopupCtrl.Instance:Center(self.language.CreateTips4)
        return
	end
	if word_num > SocietyConfig.CHAT_GROUP_NOTICE_NUM_MAX then
        PublicPopupCtrl.Instance:Center(string.format(self.language.CreateTips6, SocietyConfig.CHAT_GROUP_NOTICE_NUM_MAX))
        return
    end
	if ChatFilter.Instance:IsIllegal(self.InputNotice.text) then
        PublicPopupCtrl.Instance:Center(self.language.IllegalContent)
        return
    end
	SocietyCtrl.Instance:SendChangeGroupNoticeReq(self.data.chat_group_detail.group_id, self.data.chat_group_detail.group_name, self.InputNotice.text)
end

function SocietyGroupManageViewF:OnClickRemind()
	
end

function SocietyGroupManageViewF:OnClickShare()
	self.ShareObj:SetActive(true)
end

function SocietyGroupManageViewF:OnClickCloseShare()
	self.ShareObj:SetActive(false)
end

function SocietyGroupManageViewF:OnClickShareGuild()
	-- self.ShareObj:SetActive(false)
	-- if 0 == GuildData.Instance:GetGuildID() then
	-- 	PublicPopupCtrl.Instance:Center(self.language.NoGuildTips)
	-- else
	-- 	ChatCtrl.Instance:SendChannelChatText(self.data.cur_sel_chat_group:GroupInviteStr(), nil, ChatConfig.ChannelType.guild)
	-- end
end

function SocietyGroupManageViewF:OnClickShareTeam()
	-- self.ShareObj:SetActive(false)
	-- if TeamData.Instance:InTeam() then
	-- 	ChatCtrl.Instance:SendChannelChatText(self.data.cur_sel_chat_group:GroupInviteStr(), nil, ChatConfig.ChannelType.team)
	-- else
	-- 	PublicPopupCtrl.Instance:Center(self.language.NoTeamTips)
	-- end
end


SocietyGroupManageViewItem = DeclareMono("SocietyGroupManageViewItem", UIWidgetBaseItem)

function SocietyGroupManageViewItem:SocietyGroupManageViewItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyGroupManageViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data
	local prof_sp = RoleData.GetProfSp(info.prof)
	local is_online = 0 == info.member_logout_tamp
	self.Name.text = info.name
	self.Level.text = info.level
	self.ProfI.SpriteName = prof_sp
	self.ProfI:SetObjActive(nil ~= prof_sp)
	UH.SetAvatar(self.IconSp, info.avatar_type, info.avatar_type,info.avatar_quality)
	self.BtnMoreObj:SetActive(not info.is_master)
	self.MasterObj:SetActive(info.is_master)
	self.MaskObj:SetActive(not is_online)
	self.RoleState.text = is_online and Language.Society.RoleOnline or TimeHelper.Ago(info.member_logout_tamp)
end

function SocietyGroupManageViewItem:OnClickMore()
	local info = self.data
	local config = SocietyConfig.ChatGroupFuncs
	local config = DataHelper.TableCopy(config[3])
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
