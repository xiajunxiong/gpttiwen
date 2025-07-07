
SocietyViewFriendGroup = SocietyViewFriendGroup or DeclareMono("SocietyViewFriendGroup", UIWFlushPanelGroup)

function SocietyViewFriendGroup:SocietyViewFriendGroup()
	self.data = SocietyCtrl.Instance:Data()
	
	GuideManager.Instance:RegisterGetGuideUi("SocietyChatQuickSel", function ()
		if self.GroupList:SetupCompleted() and self.GroupList.item_list[1] then
			return self.GroupList.item_list[1].MoreObj, function ()
				self.GroupList.item_list[1]:OnClickMore()
			end
		end
	end)
end

function SocietyViewFriendGroup:OnEnable()
	self.data.cur_sel_tab = SocietyConfig.SocietyType.friend
	ViewMgr:FlushView(SocietyView, "SocietyViewChat")
	ViewMgr:FlushView(SocietyView, "SocietyViewFriendGroup/SocietyViewFriendGroupMid")
	self.data.cur_sel_role = nil
end

SocietyViewFriendGroupTypeGrid = SocietyViewFriendGroupTypeGrid or DeclareMono("SocietyViewFriendGroupTypeGrid", UIWFlushPanel)

function SocietyViewFriendGroupTypeGrid:SocietyViewFriendGroupTypeGrid()
	self.flush_events = {EventProtocol.ToEventKey(SCFriendGroupInfo)}

	self.data = SocietyCtrl.Instance:Data()
	self.data.friend_cur_sel_group = self.data.guide_group and self.data:GetFriendGroupInfoByType(SocietyConfig.FriendGroupType.group) or (type(self.data.recently_chat_role) == "table" and self.data:GetFriendGroupInfoByType(SocietyConfig.FriendGroupType.recent_contacts) or self.data:GetFriendGroupInfoByType(SocietyConfig.FriendGroupType.friend))
	self.data.guide_group = false
end

function SocietyViewFriendGroupTypeGrid:onFlush()
	local friend_group_list = self.data:GetFriendGroupList()
	self.TypeGrid:SetData(friend_group_list)
	self.TypeGrid:ClickItemData(self.data.friend_cur_sel_group)
end

SocietyViewFriendGroupTypeItem = DeclareMono("SocietyViewFriendGroupTypeItem", UIWidgetBaseItem)

function SocietyViewFriendGroupTypeItem:SocietyViewFriendGroupTypeItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function SocietyViewFriendGroupTypeItem:OnDestroy()
    self:uncareData()
end

function SocietyViewFriendGroupTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
    self.care_handles = {}
	self:careData(data.group_red_point, BindTool.Bind(self.GroupRedPoint, self))

	self.TypeName.text = data.group_name
end

function SocietyViewFriendGroupTypeItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function SocietyViewFriendGroupTypeItem:GroupRedPoint()
	if self.ItemToggle.isOn then 
		self.data:GroupRedPoint(false)
	end
	self.red_point_item = self.red_point_item or UiRedPoint.New(self.ItemObj, Vector3.New(40,20,0))
	self.red_point_item:SetNum(self.data.group_red_point.val and 1 or 0)
end

function SocietyViewFriendGroupTypeItem:OnClickType()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
	self.society_data.cur_sel_role = nil
	self.data:GroupRedPoint(false)
	if self.society_data.friend_cur_sel_group ~= self.data then
		self.society_data.recently_chat_role = nil
	end
	self.society_data.friend_cur_sel_group = self.data
	ViewMgr:FlushView(SocietyView, "SocietyViewFriendGroup/SocietyViewFriendGroupMid")
end

SocietyViewFriendGroupMid = SocietyViewFriendGroupMid or DeclareMono("SocietyViewFriendGroupMid", UIWFlushPanel)

function SocietyViewFriendGroupMid:SocietyViewFriendGroupMid()
    self.flush_events = {EventProtocol.ToEventKey(SCFriendListChange), EventProtocol.ToEventKey(SCChangeBlack), EventProtocol.ToEventKey(SCFriendGroupInfo), EventProtocol.ToEventKey(SCOneRecentlyTeamInfo), EventProtocol.ToEventKey(SCSingleChat), EventProtocol.ToEventKey(SCRoleGroupInfo), EventProtocol.ToEventKey(SCRoleOneGroupInfo)}
	self.data = SocietyCtrl.Instance:Data()

	self.data_cares = {
        {data = self.data.new_friend_red_point, func = self.NewFriendRedPoint },
    }
end

function SocietyViewFriendGroupMid:onFlush()
	-- self.ToggleGroupRole:SetAllTogglesOff()
	self.RoleList:SelectAll(false)
	local cur_sel_group = self.data.friend_cur_sel_group
	local group_type = cur_sel_group.group_type
	local friend_group_type = SocietyConfig.FriendGroupType
	local friend_group_funcs = SocietyConfig.FriendGroupFuncs
	local show_list = friend_group_type.group == group_type and self.data:GetChatGroupList() or self.data:GetFriendListByType(group_type, true)
	
	local new_group = (friend_group_type.group_custom1 == group_type or friend_group_type.group_custom2 == group_type) and cur_sel_group.not_create
	if friend_group_type.group == group_type then
		self.GroupList:SetObjActive(true)
		self.RoleList:SetObjActive(false)
		self.GroupList:SetData(show_list)
		self.GroupList:ClickItemData(self.data.cur_sel_chat_group)
	else
		local friend_info = type(self.data.recently_chat_role) == "table" and self.data.recently_chat_role or show_list[1]
		self.RoleList:SetObjActive(not new_group)
		self.GroupList:SetObjActive(false)
		self.RoleList:SetData(show_list)
		if self.data.cur_sel_role == nil then 	
		    self.data.cur_sel_role = friend_info
		end 

		self.RoleList:ClickItemData( self.data.cur_sel_role)
	end
	self.Empty:SetActive(#show_list == 0 and (friend_group_type.recent_contacts == group_type or friend_group_type.friend == group_type))
	
	ViewMgr:FlushView(SocietyView, "SocietyViewChat")

	self.NewGroupObj:SetActive(new_group)
	self.FriendNumObj:SetActive(SocietyConfig.FriendGroupType.friend == group_type)
	if SocietyConfig.FriendGroupType.friend == group_type then
		local friend_num = self.data:GetFriendGroupNumShow()
		self.FriendNum.text = string.format(Language.Society.FriendGroup.FriendNum, friend_num)
	end

	for i = 1, 2 do
		local value = nil ~= friend_group_funcs[group_type] and nil ~= friend_group_funcs[group_type][i]
		if (friend_group_type.group_custom1 == group_type or friend_group_type.group_custom2 == group_type) and cur_sel_group.not_create then value = false end

		self.BtnOperObjs[i]:SetActive(value)
		self.BtnOpers[i].text = value and Language.Society.FriendGroupFuncs[friend_group_funcs[group_type][i]] or ""
	end
end

function SocietyViewFriendGroupMid:NewFriendRedPoint()
	local cur_sel_group = self.data.friend_cur_sel_group
	local group_type = cur_sel_group.group_type
	local friend_group_type = SocietyConfig.FriendGroupType
	self.new_friend_point = self.new_friend_point or UiRedPoint.New(self.BtnOperObjs[2], Vector3.New(77.7,24,0))
	self.new_friend_point:SetNum(((friend_group_type.friend == group_type) and self.data.new_friend_red_point.val) and 1 or 0)
end

function SocietyViewFriendGroupMid:OnClick(index)
	local group_type = self.data.friend_cur_sel_group.group_type
	self[SocietyConfig.FriendGroupFuncs[group_type][index]](self)
end

function SocietyViewFriendGroupMid:AddFriends()
	ViewMgr:OpenView(SocietyViewFriendAdd)
end

function SocietyViewFriendGroupMid:ApplyList()
	ViewMgr:OpenView(SocietyViewFriendApply)
end

function SocietyViewFriendGroupMid:DeleteGroup()
	local group_type = self.data.friend_cur_sel_group.group_type
	local info = {
        content = Language.Society.FriendGroup.DeleteTips,
        confirm = {
            func = function()
				SocietyCtrl.Instance:SendGroupDelete(group_type)
                ViewMgr:CloseView(DialogTipsView)
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

function SocietyViewFriendGroupMid:EditGroup()
	ViewMgr:OpenView(SocietyViewFriendEditNewGroup)
end

function SocietyViewFriendGroupMid:Unblock()
	if nil == self.data.cur_sel_role then
		PublicPopupCtrl.Instance:Center(Language.Society.ChatGroupInfo.NoObjectTips)
		return
	end
	local info = self.data.cur_sel_role.info
    PublicPopupCtrl.Instance:DialogTips({
		content = string.format(Language.Society.FriendFuncTips.delete_black, info.name),
        confirm = {
			func = function()
				SocietyCtrl.Instance:SendBlackListRemove(info.uid)
				ViewMgr:CloseView(DialogTipsView)
            end
		}
    })
end

function SocietyViewFriendGroupMid:NewGroup()
	ViewMgr:OpenView(SocietyViewFriendEditNewGroup)
end

function SocietyViewFriendGroupMid:OnClickGroupCreate()
	ViewMgr:OpenView(SocietyGroupCreateView)
end

SocietyViewFriendRoleItem = DeclareMono("SocietyViewFriendRoleItem", UIWidgetBaseItem)

function SocietyViewFriendRoleItem:SocietyViewFriendRoleItem()
	self.society_data = SocietyCtrl.Instance:Data()
	self.info_data = RoleInfoCtrl.Instance:Data()
end

function SocietyViewFriendRoleItem:OnDestroy()
    self:uncareData()
end

function SocietyViewFriendRoleItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
    self.care_handles = {}
	self:careData(data.chat_red_point, BindTool.Bind(self.ChatRedPoint, self))
	self:careData(data.gift_red_point, BindTool.Bind(self.GiftRedPoint, self))
	self:careData(self.society_data.quick_chat_info, BindTool.Bind(self.FlushQuickSp, self), "show_id_list")

	local info = data.info
	local prof_sp = RoleData.GetProfSp(info.prof)
	local is_online = info.is_online >= 1
	local uid = info.uid
	UH.SetText(self.Name, info.name)
	UH.SetText(self.Love, info.love)
	UH.SpriteName(self.ProfI, prof_sp)
	self.ProfI.gameObject:SetActive(nil ~= prof_sp)
	self.MaskObj:SetActive(not is_online)
	self.OnlineSp:SetObjActive(is_online)
	self.RoleState.text = is_online and Language.Society.RoleOnline or TimeHelper.Ago(info.last_online_timestamp)
	if info.top_level and info.top_level > 0 then
		self.PeakLevel:SetActive(true)
		UH.SetText(self.Level, info.top_level)
		UH.Color(self.Level, COLORS.Red8)
	else
		UH.SetText(self.Level, info.level)
		UH.Color(self.Level, COLORS.White)
		self.PeakLevel:SetActive(false)
	end
	AvatarEffect.SetImageEnabled(self.PeakImg,info.avatar_quality)

	if is_online then
		self.OnlineSp.SpriteName = SocietyConfig.OnlineSp[info.is_online]
		self.OnlineTag.text = Language.Society.OnlineTag[info.is_online]
	end

	UH.SetAvatar(self.IconSp, info.avatar_type,info.avatar_id,info.avatar_quality)
end

function SocietyViewFriendRoleItem:FlushQuickSp()
	local uid = self.data.info and self.data.info.uid or 0
	self.QuickSp.SpriteName = self.society_data:CheckValInQuickChatShowList(uid) and "GuanZhu" or "WeiGuanZhu"
end

function SocietyViewFriendRoleItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function SocietyViewFriendRoleItem:ChatRedPoint()
	self.red_point_item = self.red_point_item or UiRedPoint.New(self.ItemObj, Vector3.New(180,50,0))
	self.red_point_item:SetNum(self.data.chat_red_point.val and 1 or 0)
end

function SocietyViewFriendRoleItem:GiftRedPoint()
	self.BtnGiftObj:SetActive(self.data.gift_red_point.val)
end

function SocietyViewFriendRoleItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.society_data.cur_sel_role = self.data
		ViewMgr:FlushView(SocietyView, "SocietyViewChat")
    end
end

function SocietyViewFriendRoleItem:OnClickMore()
	self:Click()
	local info = self.data.info
	local config = SocietyConfig.FriendFunctions
	local group_type = self.society_data.friend_cur_sel_group.group_type
	local config = DataHelper.TableCopy(config[group_type])
	local is_friend = self.society_data:IsFriend(info.uid)
	local is_master = self.society_data:IsMaster()
	-- if SocietyConfig.FriendGroupType.friend == group_type then
	-- 	if self.society_data:CheckValInQuickChatShowList(info.uid) then
	-- 		table.insert(config, 4, "QuickChatDel")
	-- 	else
	-- 		table.insert(config, 4, "QuickChat")
	-- 	end
	-- end
	if SocietyConfig.FriendGroupType.black_list ~= group_type then
		local num = self.society_data.master_apprentice_num
		local max_num = is_master and SocietyConfig.MasterApprenticeMax.apprentice or SocietyConfig.MasterApprenticeMax.master
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
		if SocietyConfig.FriendGroupType.recent_contacts == group_type or SocietyConfig.FriendGroupType.recent_team == group_type then
			if not is_friend then table.insert(config, 2, "AddFriend") end
		end
		-- if is_friend then table.insert(config, 2, "SendFlower") end
	else
		table.insert(config, "Unblock")
	end
	RoleInfoCtrl.Instance:RoleInfoView{
		info = function ()
				return RoleInfoData.RoleInfoNew{func_info = info, uid = info.uid, name = info.name, level = info.level, prof = info.prof, family = info.family, avatar_type = info.avatar_type, avatar_id = info.avatar_id, top_level = info.top_level}
			end,
		funcs = function()
			return config
		end
	}
end

function SocietyViewFriendRoleItem:OnClickQuick()
	local uid = self.data.info.uid
	if self.society_data:CheckValInQuickChatShowList(uid) then
		SocietyData.Instance:DeleteQuickChatShowVal(uid)
	else
		if SocietyData.Instance:QuickChatAdd(SocietyConfig.QuickShowType.role, uid) then
			-- ViewMgr:CloseView(SocietyView)
		end
	end
end

function SocietyViewFriendRoleItem:OnClickGift()
	local succ, uuid_key = self.society_data:GetGiftUUIDKeyByUid(self.data.info.uid)
	if succ then
		self.society_data:SetSocietyGiftInfo(SocietyConfig.GiftMsgType.send, uuid_key)
		ViewMgr:OpenView(SocietyGiftView)
	end
end

SocietyViewFriendGroupItem = DeclareMono("SocietyViewFriendGroupItem", UIWidgetBaseItem)

function SocietyViewFriendGroupItem:SocietyViewFriendGroupItem()
	self.society_data = SocietyCtrl.Instance:Data()
	-- self.info_data = RoleInfoCtrl.Instance:Data()
end

function SocietyViewFriendGroupItem:OnDestroy()
    self:uncareData()
end

function SocietyViewFriendGroupItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
    self.care_handles = {}
	self:careData(data.chat_red_point, BindTool.Bind(self.ChatRedPoint, self))
	self:careData(self.society_data.quick_chat_info, BindTool.Bind(self.FlushQuickSp, self), "show_id_list")

	-- local info = data.info
	-- local prof_sp = RoleData.GetProfSp(info.prof)
	-- local is_online = info.is_online >= 1
	local avatars = data:Avatars()
	local cross_state = data:CrossState()
	self.Name.text = data:GroupName()
	self.MemberCount.text = string.format(Language.Society.ChatGroupInfo.MemberCount, data:MemberCount(), SocietyConfig.CHAT_GROUP_MEMBER_MAX)
	self.OnlineSp.SpriteName = SocietyConfig.OnlineSp[cross_state]
	self.OnlineTag.text = Language.Society.OnlineTag[cross_state]
	for i = 1, self.Avatars:Length() do
		if 0 == avatars[i] then
			self.Avatars[i].SpriteName = SocietyConfig.ChatGroupIconNone
		else
			UH.SetAvatar(self.Avatars[i], avatars[i], avatars[i])
		end
	end
end

function SocietyViewFriendGroupItem:FlushQuickSp()
	local group_key = self.data:GroupKey()
	self.QuickSp.SpriteName = self.society_data:CheckValInQuickChatShowList(group_key) and "GuanZhu" or "WeiGuanZhu"
end

function SocietyViewFriendGroupItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function SocietyViewFriendGroupItem:ChatRedPoint()
	self.red_point_item = self.red_point_item or UiRedPoint.New(self.ItemObj, Vector3.New(180,50,0))
	self.red_point_item:SetNum(self.data.chat_red_point.val and 1 or 0)
end

function SocietyViewFriendGroupItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        self.society_data.cur_sel_chat_group = self.data
		ViewMgr:FlushView(SocietyView, "SocietyViewChat")
    end
end

function SocietyViewFriendGroupItem:OnClickMore()
	self:Click()
	local config = DataHelper.TableCopy(SocietyConfig.ChatGroupFuncs[RoleData.Instance:GetRoleId() == self.data:Master() and 1 or 2])
	if 1 == self.society_data.cur_sel_chat_group:DisturbVal() then
		table.insert(config, 4, "GroupNoDisturb")
	else
		table.insert(config, 4, "GroupNoDisturbClose")
	end
	-- if self.society_data:CheckValInQuickChatShowList(self.data:GroupKey()) then
	-- 	table.insert(config, 5, "QuickChatDel")
	-- else
	-- 	table.insert(config, 5, "QuickChat")
	-- end
	RoleInfoCtrl.Instance:RoleInfoView{
		info = function ()
				return RoleInfoData.GroupInfoNew{func_info = self.data, group_id = self.data:GroupId(), group_name = self.data:GroupName(), member_count = self.data:MemberCount(), group_avatars = self.data:Avatars()}
			end,
		funcs = function()
			return config
		end
	}
end

function SocietyViewFriendGroupItem:OnClickQuick()
	local group_key = self.data:GroupKey()
	if self.society_data:CheckValInQuickChatShowList(group_key) then
		SocietyData.Instance:DeleteQuickChatShowVal(group_key)
	else
		if SocietyData.Instance:QuickChatAdd(SocietyConfig.QuickShowType.group, group_key) then
			-- ViewMgr:CloseView(SocietyView)
		end
	end
end