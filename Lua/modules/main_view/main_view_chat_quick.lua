
local Input = UnityEngine.Input

MainViewChatQuick = MainViewChatQuick or DeclareMono("MainViewChatQuick", UIWFlushPanel)

function MainViewChatQuick:Awake()
	UIWFlushPanel.Awake(self)
	self.ShowItem:Start()
	if IS_STANDALONE() then
		UH.LocalScale(self.GameRect, STANDALONE_UI_VECTOR3)
		UH.LocalScale(self.BattleRect, STANDALONE_UI_VECTOR3)
		UH.LocalScale(self.BgRect, STANDALONE_UI_VECTOR3)
		UH.LocalScale(self.BattleActRect, STANDALONE_UI_VECTOR3)
		local group_id = ""
		if ChannelAgent.PCAgentType then
			local pc_agent_type = ChannelAgent.PCAgentType()
			if pc_agent_type == "_360PC" then
				group_id = Language.PCNotice.QQGroupId.ThreeSixZero
			elseif pc_agent_type == "QQ" then
				group_id = Language.PCNotice.QQGroupId.qq
			end
		end
		if not StringIsEmpty(group_id) then
			self.PCQQTxtObj:SetActive(true)
			UH.SetText(self.PCQQTxt, Format(Language.PCNotice.QQGroupReward, group_id))
		else
			self.PCQQTxtObj:SetActive(false)
		end
	elseif GLOBAL_CONFIG:PkgInfo().agent_id == "pa3" then
		self.PCQQTxtObj:SetActive(true)
		UH.SetText(self.PCQQTxt, Format(Language.PCNotice.QQGroupReward, Language.PCNotice.QQGroupId.ThreeSixZero))
	else
		self.PCQQTxtObj:SetActive(false)
	end
end

function MainViewChatQuick:MainViewChatQuick()
	self.chat_data = ChatCtrl.Instance:Data()
	self.society_data = SocietyCtrl.Instance:Data()
	self.guild_data = GuildData.Instance

	self.data_cares = {
		{data = self.society_data.quick_chat_info, func = self.FlushShow, keys = {"show_list"}},
		{data = self.society_data.quick_chat_info, func = self.FlushChat, keys = {"cur_sel"}},
		{data = self.society_data.quick_chat_info, func = self.FlushShow, keys = {"flush_items"}, init_call = false},
		{data = self.society_data.quick_chat_info, func = self.FlushEditing, keys = {"is_editing"}, init_call = false},
		{data = self.guild_data.question_show, func = self.FlushRemindObj, keys = {"show_panel"}},
		{data = self.society_data.chat_group_detail, func = self.FlushQuickName, init_call = false},
		{data = self.society_data.quick_chat_info, func = self.FlushQuickName, init_call = false},
	}

	GuideManager.Instance:RegisterGetGuideUi("SocietyChatQuickItem", function ()
		if self.ShowList:SetupCompleted() and self.ShowList.item_list[1] then
			local data = self.ShowList.item_list[1]:GetData()
			if data and SocietyConfig.QuickShowType.add == data.type then
				return self.ShowList.item_list[1].AddObj, function () end
			else
				return self.ShowList.item_list[1]
			end
        end
    end)
end

function MainViewChatQuick:FlushRemindObj()
    self.RemindObj:SetActive(not self.guild_data.question_show.show_panel)
end

function MainViewChatQuick:FlushShow()
	local show_list1, show_list2 = self.society_data:GetChatQuickShowList()
	local show_items = #self.society_data.quick_chat_info.show_list > 3
	self.EmptyObj:SetActive(false)
	self.EmptyObjL:SetActive(false)
	self.ShowList:SetData(show_list1)
	self.ShowItem:SetObjActive(show_items)
	self.ShowLayout.padding.left = show_items and 84 or 4
	self.TempItem:SetObjActive(nil ~= self.society_data.chat_group_temp)
	if show_items then
		self.ShowListL:SetData(show_list2)
	end
	if nil ~= self.society_data.chat_group_temp then
		self.TempItem:SetData(self.society_data.quick_chat_info.show_list[0])
	end
	self.ShowListL:SetObjActive(show_items)
	self.ArrowPos.localPosition = Vector2.New(show_items and -315 or -220, 0)
end

function MainViewChatQuick:FlushChat()
	local cur_sel = self.society_data.quick_chat_info.cur_sel
	local is_show = not StringIsEmpty(cur_sel)
	self.ChatView:SetActive(is_show)
	if is_show then
		if "string" == type(cur_sel) then
			SocietyCtrl.Instance:SendGetGroupInfo(SocietyData.GroupKey2Id(cur_sel))
		elseif SocietyConfig.QuickShowType.temp == cur_sel and nil ~= self.society_data.chat_group_temp then
			SocietyCtrl.Instance:SendGetGroupInfo(self.society_data.chat_group_temp:GroupId())
		end
		self:FlushQuickName()
	end
end

function MainViewChatQuick:FlushQuickName()
	local cur_sel = self.society_data.quick_chat_info.cur_sel
	local show_intimacy = false
	if "string" == type(cur_sel) then
		local cgd = self.society_data.chat_group_detail
		if cur_sel == cgd.group_key then
			-- self.QuickName.text = string.format(Language.Society.QuickChat.Name, cgd.group_name, cgd.online_count, cgd.member_count)
			self.QuickName.text = cgd.group_name
		end
	elseif SocietyConfig.QuickShowType.temp == cur_sel and nil ~= self.society_data.chat_group_temp then
		local cgd = self.society_data.chat_group_detail
		local group = self.society_data.chat_group_temp
		if group and group:GroupKey() == cgd.group_key then
			-- self.QuickName.text = string.format(Language.Society.QuickChat.Name, cgd.group_name, cgd.online_count, cgd.member_count)
			self.QuickName.text = cgd.group_name
		end
	else
		local frined = self.society_data.friend_list[cur_sel]
		if frined and frined.info then
			self.QuickName.text = frined.info.name
			show_intimacy = true
			self.IntimacyVal.text = self.society_data:GetFriendIntimacyVal(cur_sel)
		end
	end
	self.IntimacyObj:SetActive(show_intimacy)
end

function MainViewChatQuick:OnItemPressDown()
	if self.society_data.quick_chat_info.is_editing then
		Runner.Instance:RemoveRunObj(self)
		Runner.Instance:AddRunObj(self, 3)
	else
		TimeHelper:CancelTimer(self.timer_dt)
		self.timer_dt = TimeHelper:AddDelayTimer(function ()
			if #self.society_data.quick_chat_info.show_list > 0 then
				self.society_data.quick_chat_info.is_editing = true
			end
			-- self.society_data.quick_chat_info.cur_sel = ""
		end, 1)
	end
end

function MainViewChatQuick:OnItemPressUp()
	if self.society_data.quick_chat_info.is_editing then
		
	else
		TimeHelper:CancelTimer(self.timer_dt)
	end
end

function MainViewChatQuick:FlushEditing()
	local is_editing = self.society_data.quick_chat_info.is_editing
	self.ChatView:SetActive(is_editing and #self.society_data.quick_chat_info.show_list > 3)
	self.ChatObj:SetActive(not is_editing)
	self.EditExitObj:SetActive(is_editing)
end

function MainViewChatQuick:OnClickEditExit()
	self.society_data.quick_chat_info.is_editing = false
end

function MainViewChatQuick:Update()
	if Input.GetMouseButton(0) then
		if SocietyConfig.ChatQuickFlyState.start == self.society_data.quick_chat_fly.state then
			-- if TimeHelper.GetServerTime() - self.data.partner_fly.time > 1 then
				self:OnBeginDragShowItem()
			-- end
		elseif SocietyConfig.ChatQuickFlyState.flying == self.society_data.quick_chat_fly.state then
			self:OnDragShowItem()
		end
	else
		if SocietyConfig.ChatQuickFlyState.flying == self.society_data.quick_chat_fly.state then
			self.society_data.quick_chat_fly.state = SocietyConfig.ChatQuickFlyState.finish
			self:OnUpShowItem()
		end
	end
end

function MainViewChatQuick:OnBeginDragShowItem()
	local quick_chat_fly = self.society_data.quick_chat_fly
	quick_chat_fly.state = SocietyConfig.ChatQuickFlyState.flying
	local si = quick_chat_fly.item:GetSiblingIndex()
	self.fly_from_si = si
	self.fly_from_x = quick_chat_fly.item.transform.localPosition.x
	quick_chat_fly.item:SetActive(false)
	self.EmptyObj:SetSiblingIndex(si)
	self.EmptyObj:SetActive(true)
	self.EmptyObjL:SetSiblingIndex(si)
	self.EmptyObjL:SetActive(true)

	self:OnDragShowItem()

	self.QuickShowFlyItem:SetData(quick_chat_fly.data)
	self.QuickShowFlyItem:SetObjActive(true)
end

function MainViewChatQuick:OnDragShowItem()
	local input_x = UnityEngine.Input.mousePosition.x / UnityEngine.Screen.width * self.ViewSize.rect.width
	local input_y = UnityEngine.Input.mousePosition.y / UnityEngine.Screen.height * self.ViewSize.rect.height - self.ViewSize.rect.height / 2
	self.QuickShowFlyItemPos.localPosition = Vector2.New(input_x, input_y)
	self:FlushEmptySI(input_x, input_y)
end

function MainViewChatQuick:FlushEmptySI(input_x, input_y)
	local count = #self.society_data.quick_chat_info.show_list
	if input_y > 0 and count > 3 then
		local si = 1
		local offset = 315
		if input_y < offset then
			si = math.ceil((offset - input_y) / 78) + 1
			si = si > self.fly_from_si - 1 and si + 1 or si
			si = math.min(si, 6)
		end
		self.EmptyObjL:SetSiblingIndex(si)
	else
		local si = 3
		local offset = count > 3 and 84 or 16
		if input_x > offset then
			si = math.ceil((input_x - offset) / 78) + 2
			si = si > self.fly_from_si - 1 and si + 1 or si
			si = math.min(si, 6)
		end
		self.EmptyObj:SetSiblingIndex(si)
	end
end

function MainViewChatQuick:OnUpShowItem()
	Runner.Instance:RemoveRunObj(self)
	local quick_chat_fly = self.society_data.quick_chat_fly
	-- quick_chat_fly.item:SetActive(true)
	-- self.EmptyObj:SetActive(false)
	self.QuickShowFlyItem:SetObjActive(false)
	local input_y = UnityEngine.Input.mousePosition.y / UnityEngine.Screen.height * self.ViewSize.rect.height - self.ViewSize.rect.height / 2
	if #self.society_data.quick_chat_info.show_list > 3 and input_y > -50 then
		self.society_data:ChangeQuickChatShowVal(self.EmptyObjL:GetSiblingIndex() + 3, quick_chat_fly.data)
	else
		self.society_data:ChangeQuickChatShowVal(self.EmptyObj:GetSiblingIndex() - 2, quick_chat_fly.data)
	end
end

MainViewChatQuickRecordList = MainViewChatQuickRecordList or DeclareMono("MainViewChatQuickRecordList", UIWFlushPanel)

function MainViewChatQuickRecordList:MainViewChatQuickRecordList()
	self.data = ChatCtrl.Instance:Data()
	self.society_data = SocietyCtrl.Instance:Data()
	self.language = Language.Chat

	self.data_cares = {
		{data = self.society_data.quick_chat_info, func = self.FlushRecordList, keys = {"cur_sel"}},
		{data = self.society_data.quick_chat_info, func = self.FlushRecordList, keys = {"flush_records"}, init_call = false},
		{data = self.society_data.chat_group_channels, func = self.FlushRecordList, init_call = false},
	}
end

function MainViewChatQuickRecordList:FlushRecordList()
	local chat_list = {}
	local cur_sel = self.society_data.quick_chat_info.cur_sel
	if StringIsEmpty(cur_sel) then return end
	if "string" == type(cur_sel) then
		chat_list = self.society_data:GetGroupChannelChatListByGroupId(nil, cur_sel)
		self.society_data:CheckFriendChatGroupRedPoint(SocietyConfig.FriendGroupType.group)
	elseif SocietyConfig.QuickShowType.temp == cur_sel and nil ~= self.society_data.chat_group_temp then
		chat_list = self.society_data:GetGroupChannelChatListByGroupId(nil, self.society_data.chat_group_temp:GroupKey())
		self.society_data:CheckFriendChatGroupRedPoint(SocietyConfig.FriendGroupType.group)
	else
		chat_list = self.society_data:GetChatListByUid(cur_sel, true)
	end
	self.RecordList:SetDataList(chat_list, #chat_list)
end

MainViewChatQuickDown = MainViewChatQuickDown or DeclareMono("MainViewChatQuickDown", UIWFlushPanel)

function MainViewChatQuickDown:MainViewChatQuickDown()
    self.data = SocietyCtrl.Instance:Data()
	self.chat_data = ChatCtrl.Instance:Data()
	self.team_data = TeamCtrl.Instance:Data()
	self.world_num_limit = self.chat_data:GetSocietyLimitNum()

    self.data_cares = {
		{data = self.chat_data.chat_quick_input, func = self.FlushInputField, init_call = false},
		{data = self.data.quick_chat_info, func = self.FlushInputShow, keys = {"cur_sel"}},
		{data = self.data.quick_chat_info, func = self.FlushButtons, keys = {"cur_sel"}},
		{data = self.data.chat_group_detail, func = self.FlushRoleList},
    }

    self.chat_helper = ChatHelper.New{
        input_field = self.InputField,
        limit_num = self.chat_data:GetSocietyLimitNum()
	}
	self.input_records = {}
end

function MainViewChatQuickDown:Awake()
	UIWFlushPanel.Awake(self)
	self.BtnVoiceObj:SetActive(GLOBAL_CONFIG:FlagsVoice())
end

function MainViewChatQuickDown:OnDisable()
	UIWFlushPanel.OnDisable(self)
	self.chat_helper:Empty()
	self.input_records = {}
end

function MainViewChatQuickDown:FlushInputShow(old_val)
	local input_record = self.input_records[self.data.quick_chat_info.cur_sel]
	if old_val then
		self.input_records[old_val.cur_sel] = self.chat_helper:CopyInfo()
	end
	if input_record then
		self.chat_helper:PasteInfo(input_record)
	else
		self.chat_helper:Empty()
	end
end

function MainViewChatQuickDown:FlushButtons()
	local cur_sel = self.data.quick_chat_info.cur_sel
	if StringIsEmpty(cur_sel) then return end
	local is_group = "string" == type(cur_sel)
	local is_temp = not is_group and SocietyConfig.QuickShowType.temp == cur_sel and nil ~= self.data.chat_group_temp
	self.BtnDetailsObj:SetActive(is_group or is_temp)
	self.BtnTeamObj:SetActive(not is_temp)
	self.BtnInfoObj:SetActive(not is_temp)
	self.BtnTransObj:SetActive(is_temp)
	if is_group or is_temp then
		local group = is_temp and self.data.chat_group_temp or self.data.chat_group_list[cur_sel]
		self.BtnDetails.text = string.format(Language.Society.QuickChat.Details, group:MemberCount(), is_temp and TeamConfig.TEAM_MEMBER_MAX or SocietyConfig.CHAT_GROUP_MEMBER_MAX)
	elseif self.DetailsObj.activeSelf then
		self.DetailsObj:SetActive(false)
	end
end

function MainViewChatQuickDown:FlushRoleList()
	local role_list = self.data:GetFriendChatGroupMemberList()
	self.DetailList:SetData(role_list)

	local cur_sel = self.data.quick_chat_info.cur_sel
	if StringIsEmpty(cur_sel) then return end
	local is_group = "string" == type(cur_sel)
	local is_temp = not is_group and SocietyConfig.QuickShowType.temp == cur_sel and nil ~= self.data.chat_group_temp
	if is_group or is_temp then
		local group = is_temp and self.data.chat_group_temp or self.data.chat_group_list[cur_sel]
		self.BtnDetails.text = string.format(Language.Society.QuickChat.Details, group:MemberCount(), is_temp and TeamConfig.TEAM_MEMBER_MAX or SocietyConfig.CHAT_GROUP_MEMBER_MAX)

	end
end

function MainViewChatQuickDown:OnClickSend()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    if RoleData.Instance:GetRoleLevel() < GLOBAL_CONFIG:TalkLevelSiLiao() then
		PublicPopupCtrl.Instance:Center(string.format(Language.Chat.ChatLevelLimit, GLOBAL_CONFIG:TalkLevelSiLiao()))
		return
	end
    if self.chat_helper:IsLimitNum() then return end
	local str = self.chat_helper:RealText()
	if StringIsEmpty(str) then
		PublicPopupCtrl.Instance:Center(Language.Society.ChatIllegalTips.empty)
		return
	end
	local cur_sel = self.data.quick_chat_info.cur_sel
	if not self.chat_helper:Switch() and self.chat_data:ChatCacheCheck(str, "string" == type(cur_sel) and ChatConfig.ChannelId.group or ChatConfig.ChannelId.single) then
		return
	end

	local function callback(str_f)
		if "string" == type(cur_sel) then
			if StringIsEmpty(str) then
				PublicPopupCtrl.Instance:Center(Language.Society.ChatIllegalTips.empty)
				return
			end
			ChatCtrl.Instance:SendChannelChatText(str_f, self.chat_helper:ItemList(), ChatConfig.ChannelType.group, SocietyData.GroupKey2Id(cur_sel))
			self.chat_helper:Empty()
		elseif SocietyConfig.QuickShowType.temp == cur_sel and nil ~= self.data.chat_group_temp then
			if StringIsEmpty(str) then
				PublicPopupCtrl.Instance:Center(Language.Society.ChatIllegalTips.empty)
				return
			end
			ChatCtrl.Instance:SendChannelChatText(str_f, self.chat_helper:ItemList(), ChatConfig.ChannelType.group, self.data.chat_group_temp:GroupId())
			self.chat_helper:Empty()
		else
			local illegal, tips = self.data:CheckChatIllegal(str, cur_sel)
			if illegal then
				PublicPopupCtrl.Instance:Center(tips)
				return
			end
			self.data.cur_sel_role = self.data.friend_list[cur_sel]
			self.data:AddChatInfo(str_f, self.chat_helper:ItemList())
			self.chat_helper:Empty()
			self.data:FlushChatQuickRecords()
		end
	end
	if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(str, 2, callback)
	else
		callback(ChatFilter.Instance:Filter(str))
	end
end

function MainViewChatQuickDown:FlushInputField()
	self.chat_helper:AddChatItem(self.chat_data.chat_quick_input:Val())
end

function MainViewChatQuickDown:OnDownVoice()
    if RoleData.Instance:GetRoleLevel() < GLOBAL_CONFIG:TalkLevelSiLiao() then
		PublicPopupCtrl.Instance:Center(string.format(Language.Chat.ChatLevelLimit, GLOBAL_CONFIG:TalkLevelSiLiao()))
		return
	end
	self.chat_data:VoiceStart()
end

function MainViewChatQuickDown:OnUpVoice()
    self.chat_data:VoiceFinish(function (rec_key,time)
		local content = string.format("%s;%d",rec_key,Mathf.Round(time))
		local cur_sel = self.data.quick_chat_info.cur_sel
        if "string" == type(cur_sel) then
            ChatCtrl.Instance:SendChannelVoice(rec_key, time, ChatConfig.ChannelType.group, SocietyData.GroupKey2Id(cur_sel))
		elseif SocietyConfig.QuickShowType.temp == cur_sel and nil ~= self.data.chat_group_temp then
			ChatCtrl.Instance:SendChannelVoice(rec_key, time, ChatConfig.ChannelType.group, self.data.chat_group_temp:GroupId())
		else
			self.data.cur_sel_role = self.data.friend_list[cur_sel]
			self.data:AddChatInfo(content, nil, ChatConfig.ChatContentType.audio)
			self.data:FlushChatQuickRecords()
        end
    end)
end

function MainViewChatQuickDown:OnDragVoice()
	self.chat_data:VoiceCheck()
end

function MainViewChatQuickDown:OnClickEmoj()
    if RoleData.Instance:GetRoleLevel() < GLOBAL_CONFIG:TalkLevelSiLiao() then
		PublicPopupCtrl.Instance:Center(string.format(Language.Chat.ChatLevelLimit, GLOBAL_CONFIG:TalkLevelSiLiao()))
		return
	end
    ViewMgr:OpenView(ChatQuickView)
end

function MainViewChatQuickDown:OnClickArrow()
	self.data.quick_chat_info.cur_sel = ""
end

function MainViewChatQuickDown:OnClickTeam()
	if TeamData.Instance:InTeam() then
		local team_info = self.team_data:GetTeamInfo()
		local cur_sel = self.data.quick_chat_info.cur_sel
        if "string" == type(cur_sel) then
			ChatCtrl.Instance:SendChannelChatText(team_info:RecruitStr(), nil, ChatConfig.ChannelType.group, SocietyData.GroupKey2Id(cur_sel))
		else
			self.data.cur_sel_role = self.data.friend_list[cur_sel]
			self.data:AddChatInfo(team_info:RecruitStr(), nil)
			self.data:FlushChatQuickRecords()
		end
	else
		TeamCtrl.Instance:SendCreateTeam()
	end
end

function MainViewChatQuickDown:OnClickInfo()
	local cur_sel = self.data.quick_chat_info.cur_sel
	if "string" == type(cur_sel) then
		local group = self.data.chat_group_list[cur_sel]
		local config = DataHelper.TableCopy(SocietyConfig.ChatGroupFuncs[RoleData.Instance:GetRoleId() == group:Master() and 1 or 2])
		if 1 == group:DisturbVal() then
			table.insert(config, 4, "GroupNoDisturb")
		else
			table.insert(config, 4, "GroupNoDisturbClose")
		end
		if self.data:CheckValInQuickChatShowList(group:GroupKey()) then
			table.insert(config, 5, "QuickChatDel")
		else
			table.insert(config, 5, "QuickChat")
		end
		RoleInfoCtrl.Instance:RoleInfoView{
			info = function ()
					return RoleInfoData.GroupInfoNew{func_info = group, group_id = group:GroupId(), group_name = group:GroupName(), member_count = group:MemberCount(), group_avatars = group:Avatars()}
				end,
			funcs = function()
				return config
			end
		}
	else
		local friend = self.data.friend_list[cur_sel]
		local info = friend.info
		local config = SocietyConfig.FriendFunctions
		local group_type = SocietyConfig.FriendGroupType.friend
		local config = DataHelper.TableCopy(config[group_type])
		local is_friend = self.data:IsFriend(info.uid)
		local is_master = self.data:IsMaster()
		if self.data:CheckValInQuickChatShowList(info.uid) then
			table.insert(config, 4, "QuickChatDel")
		else
			table.insert(config, 4, "QuickChat")
		end
		if SocietyConfig.FriendGroupType.black_list ~= group_type then
			local num = self.data.master_apprentice_num
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
end

function MainViewChatQuickDown:OnClickDetails()
	local cur_sel = self.data.quick_chat_info.cur_sel
	if not ("string" == type(cur_sel)) and SocietyConfig.QuickShowType.temp == cur_sel and nil ~= self.data.chat_group_temp then
		SocietyCtrl.Instance:SendGetGroupInfo(self.data.chat_group_temp:GroupId())
	end
	self.DetailsObj:SetActive(true)
end

function MainViewChatQuickDown:OnClickTrans()
	ViewMgr:OpenView(SocietyGroupTransView)
end

function MainViewChatQuickDown:OnClickCloseDetail()
	self.DetailsObj:SetActive(false)
end

MainViewChatQuickShowItem = DeclareMono("MainViewChatQuickShowItem", UIWidgetBaseItem)

function MainViewChatQuickShowItem:MainViewChatQuickShowItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function MainViewChatQuickShowItem:OnDestroy()
    self:uncareData()
end

function MainViewChatQuickShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self:uncareData()
	self.care_handles = {}

	self:careData(self.society_data.quick_chat_info, BindTool.Bind(self.FlushSelect, self), "cur_sel")
	self:careData(self.society_data.quick_chat_info, BindTool.Bind(self.FlushEditing, self), "is_editing")

	self.RoleObj:SetActive(SocietyConfig.QuickShowType.role == data.type)
	self.GroupObj:SetActive(SocietyConfig.QuickShowType.group == data.type or SocietyConfig.QuickShowType.temp == data.type)
	self.AddObj:SetActive(SocietyConfig.QuickShowType.add == data.type)
	self.BgImage.enabled = SocietyConfig.QuickShowType.add ~= data.type
	if SocietyConfig.QuickShowType.role == data.type then
		local friend = self.society_data.friend_list[data.val]
		if nil == friend then return end
		self:careData(friend.chat_red_point, BindTool.Bind(self.ChatRedPoint, self))
		local info = friend.info
		UH.SetAvatar(self.RoleIcon, info.avatar_type, info.avatar_id)
		self.ItemInter.Interactable = info.is_online >= 1
	elseif SocietyConfig.QuickShowType.group == data.type then
		local group = self.society_data.chat_group_list[data.val]
		if nil == group then return end
		self:careData(group.chat_red_point, BindTool.Bind(self.ChatRedPoint, self))
		local avatars = group:Avatars()
		for i = 1, self.GroupIcons:Length() do
			if 0 == avatars[i] then
				self.GroupIcons[i].SpriteName = SocietyConfig.ChatGroupIconNone
			else
				UH.SetAvatar(self.GroupIcons[i], avatars[i], avatars[i])
			end
		end
		self.ItemInter.Interactable = true
	elseif SocietyConfig.QuickShowType.temp == data.type then
		local group = self.society_data.chat_group_temp
		if nil == group then return end
		self:careData(group.chat_red_point, BindTool.Bind(self.ChatRedPoint, self))
		local avatars = group:Avatars()
		for i = 1, self.GroupIcons:Length() do
			if 0 == avatars[i] then
				self.GroupIcons[i].SpriteName = SocietyConfig.ChatGroupIconNone
			else
				UH.SetAvatar(self.GroupIcons[i], avatars[i], avatars[i])
			end
		end
		self.ItemInter.Interactable = true
	elseif SocietyConfig.QuickShowType.add == data.type then
		self.ItemInter.Interactable = true
	end
end

function MainViewChatQuickShowItem:FlushSelect()
	if self.data.val == self.society_data.quick_chat_info.cur_sel then
		UIWidgetBaseItem.SetSelected(self, true)
	else
		UIWidgetBaseItem.SetSelected(self, false)
	end
end

function MainViewChatQuickShowItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function MainViewChatQuickShowItem:OnSelect(selected)
	if self.society_data.quick_chat_info.is_editing then return end
    local value = self.society_data:QuickChatCurSel(self.data.val, selected)
	if value then
		UIWidgetBaseItem.SetSelected(self, true)
    end
	self:FlushSelect()
end

function MainViewChatQuickShowItem:FlushEditing()
	local is_editing = self.society_data.quick_chat_info.is_editing
	if SocietyConfig.QuickShowType.temp == self.data.type then return end
	if not is_editing and self.Selector.isOn then self.Selector.isOn = false end
	self.ShakeAnim.enabled = is_editing
	self.ItemPos.rotation = Vector3.New(0, 0, 0)
	self.ShakeObj:SetActive(is_editing)
	self.NormalObj:SetActive(not is_editing)
end

function MainViewChatQuickShowItem:OnItemPressDown()
	if self.society_data.quick_chat_info.is_editing and SocietyConfig.QuickShowType.temp ~= self.data.type then
		self.society_data.quick_chat_fly = {
			state = SocietyConfig.ChatQuickFlyState.start,
			data = self.data,
			item = self.ItemPos.gameObject,
		}
	end
end

function MainViewChatQuickShowItem:OnClickDel()
	self.society_data:DeleteQuickChatShowVal(self.data.val)
end

function MainViewChatQuickShowItem:OnClickAdd()
	if self.society_data.quick_chat_info.is_editing then return end
	GuideManager.Instance:Start(54)
end

function MainViewChatQuickShowItem:ChatRedPoint()
	self.red_point_item = self.red_point_item or UiRedPoint.New(self.NormalObj, Vector3.New(28,28,0))
	local val = false
	if SocietyConfig.QuickShowType.role == self.data.type then
		local friend = self.society_data.friend_list[self.data.val]
		if friend then
			val = friend.chat_red_point.val
		end
	elseif SocietyConfig.QuickShowType.group == self.data.type then
		local group = self.society_data.chat_group_list[self.data.val]
		if group then
			val = group.chat_red_point.val
		end
	elseif SocietyConfig.QuickShowType.temp == self.data.type then
		local group = self.society_data.chat_group_temp
		if group then
			val = group.chat_red_point.val
		end
	end
	self.red_point_item:SetNum(val and 1 or 0)

	if self.ShowItem then
		self.ShowItem:ChatRedPoint()
	end
end

MainViewChatQuickShowItemS = DeclareMono("MainViewChatQuickShowItemS", UIWidgetBaseItem)

function MainViewChatQuickShowItemS:MainViewChatQuickShowItemS()
	self.society_data = SocietyCtrl.Instance:Data()
end

function MainViewChatQuickShowItemS:OnDestroy()
    self:uncareData()
end

function MainViewChatQuickShowItemS:Start()
    self:uncareData()
	self.care_handles = {}

	self:careData(self.society_data.quick_chat_info, BindTool.Bind(self.FlushSelect, self), "cur_sel")
end

function MainViewChatQuickShowItemS:FlushSelect()
	UIWidgetBaseItem.SetSelected(self, self.society_data:GetChatQuickShowItemIndex(self.society_data.quick_chat_info.cur_sel) > 3)
end

function MainViewChatQuickShowItemS:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function MainViewChatQuickShowItemS:OnSelect(selected)
	if self.society_data.quick_chat_info.is_editing then return end
	local value = self.society_data:QuickChatCurSelS(selected)
	if value then
		UIWidgetBaseItem.SetSelected(self, true)
    end
end

function MainViewChatQuickShowItemS:FlushEditing()
	local is_editing = self.society_data.quick_chat_info.is_editing
	if not is_editing and self.Selector.isOn then self.Selector.isOn = false end
end

function MainViewChatQuickShowItemS:ChatRedPoint()
	local cqgrp = false
	local show_list = self.society_data.quick_chat_info.show_list
    if #show_list > 3 then
        for i = 4, #show_list do
            local info = show_list[i]
            local val = info.val
            if "string" == type(val) then
				local group = self.society_data.chat_group_list[val]
                if group then
                    cqgrp = cqgrp or group:ChatRedPointVal()
                end
            else
				local friend = self.society_data.friend_list[val]
                if friend then
                    cqgrp = cqgrp or friend:ChatRedPointVal()
                end
            end
        end
    end

	self.red_point_item = self.red_point_item or UiRedPoint.New(self.NormalObj, Vector3.New(28,28,0))
	self.red_point_item:SetNum(cqgrp and 1 or 0)
end

MainViewChatQuickShowFlyItem = DeclareMono("MainViewChatQuickShowFlyItem", UIWidgetBaseItem)

function MainViewChatQuickShowFlyItem:MainViewChatQuickShowFlyItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function MainViewChatQuickShowFlyItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.RoleObj:SetActive(SocietyConfig.QuickShowType.role == data.type)
	self.GroupObj:SetActive(SocietyConfig.QuickShowType.group == data.type)
	self.AddObj:SetActive(SocietyConfig.QuickShowType.add == data.type)
	self.BgImage.enabled = SocietyConfig.QuickShowType.add ~= data.type
	if SocietyConfig.QuickShowType.role == data.type then
		local friend = self.society_data.friend_list[data.val]
		if nil == friend then return end
		local info = friend.info
		UH.SetAvatar(self.RoleIcon, info.avatar_type, info.avatar_id)
		self.ItemInter.Interactable = info.is_online >= 1
	elseif SocietyConfig.QuickShowType.group == data.type then
		local group = self.society_data.chat_group_list[data.val]
		if nil == group then return end
		local avatars = group:Avatars()
		for i = 1, self.GroupIcons:Length() do
			if 0 == avatars[i] then
				self.GroupIcons[i].SpriteName = SocietyConfig.ChatGroupIconNone
			else
				UH.SetAvatar(self.GroupIcons[i], avatars[i], avatars[i])
			end
		end
		self.ItemInter.Interactable = true
	elseif SocietyConfig.QuickShowType.add == data.type then
		self.ItemInter.Interactable = true
	end
end

MainViewChatQuickDetailItem = DeclareMono("MainViewChatQuickDetailItem", UIWidgetBaseItem)

function MainViewChatQuickDetailItem:MainViewChatQuickDetailItem()
	self.society_data = SocietyCtrl.Instance:Data()
end

function MainViewChatQuickDetailItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data
	local is_online = 0 == info.member_logout_tamp
	self.Name.text = info.name
	self.Level.text = info.level
	UH.SetAvatar(self.IconSp, info.avatar_type, info.avatar_type,info.avatar_quality)
	self.ItemInter.Interactable = is_online
end

function MainViewChatQuickDetailItem:OnClickRole()
	local vo = self.data
	if vo.uid == RoleData.Instance:GetRoleId() then return end
	SocietyCtrl.Instance:ClickToRoleVo{
        role_id = vo.uid,
        name = vo.name,
        level = vo.level,
		guild_name = "",
		profession = vo.prof,
		appearance = {
			prof_base = RoleData.ProfIdToProfType(vo.prof),
			avatar_type = vo.avatar_type,
			avatar_id = vo.avatar_type,
			avatar_quality = vo.avatar_quality
		},
		pingbi_pk = true
    }
end


MainViewChatQuickSimple = MainViewChatQuickSimple or DeclareMono("MainViewChatQuickSimple", UIWFlushPanel)
function MainViewChatQuickSimple:MainViewChatQuickSimple()
	self.chat_data = ChatCtrl.Instance:Data()
	self.society_data = SocietyCtrl.Instance:Data()
	self.guild_data = GuildData.Instance
	self.data_cares = {
		{data = self.society_data.quick_chat_info, func = self.FlushEditing, keys = {"is_editing"}, init_call = false},
	}
end

function MainViewChatQuickSimple:Awake()
	UIWFlushPanel.Awake(self)
	if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		self.SocietyBtnObj:SetActive(false)
	else
		self.SocietyBtnObj:SetActive(true)
	end
end

function MainViewChatQuickSimple:FlushEditing()
	local is_editing = self.society_data.quick_chat_info.is_editing
	self.EditExitObj:SetActive(is_editing)
end

function MainViewChatQuickSimple:OnClickList()
	ViewMgr:OpenView(ChatView)
end
function MainViewChatQuickSimple:OnClickSociety()
	ViewMgr:OpenView(SocietyView)
end
function MainViewChatQuickSimple:OnClickSetting()
	ViewMgr:OpenView(ChatViewChannelSel)
end

MainViewChatRecordList2 = MainViewChatRecordList2 or DeclareMono("MainViewChatRecordList2", UIWFlushPanel)

function MainViewChatRecordList2:MainViewChatRecordList2()
	self.data = ChatCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data:GetChatChannelList(), func = self.FlushRecordList},
	}
	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,BindTool.Bind(self.RoleNoticeComplete, self))
end

function MainViewChatRecordList2:RoleNoticeComplete()
    self:FlushRecordList()
end

function MainViewChatRecordList2:FlushRecordList()
	local chat_list = self.data:GetMainViewChatList()
    self.RecordList:SetDataList(chat_list)
    self.RecordList:JumpToDown()
end

function MainViewChatRecordList2:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	EventSys:UnBind(self.complete_handle)
    self.complete_handle = nil
end