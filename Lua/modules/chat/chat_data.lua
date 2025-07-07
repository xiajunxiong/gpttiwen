
ChatData = ChatData or BaseClass()

ChatData.AuditViewChannelTextColor = {
	[1] = {
		[1] = _NC(18, 62, 41),
		[2] = _NC(96, 86, 67),
	},
	[2] = {
		[1] = _NC(18, 62, 41),
		[2] = _NC(96, 86, 67),
	},
	[3] = {
		[1] = _NC(229, 159, 177),
		[2] = _NC(155, 98, 69),
	}
}
function ChatData:__init()
	if ChatData.Instance ~= nil then
		Debuger.LogError("[ChatData] attempt to create singleton twice!")
		return
    end
    ChatData.Instance = self

	self:ChatConfig()
	self:ChatConfigQuick()
	self:ChatConfigWorldQuestion()
	self:ChatData()
	self:ChatDataQuick()
	self:ChatDataWorldQuestion()
	self.new_msg = SmartData.New{num = 0}
	self.record_data = SmartData.New({cur_playing_key = ""})
	self.report_list = {}
	self.chat_check_cache1 = {}
	self.chat_check_cache2 = {}
	self.chat_block_cache = {}
end

function ChatData:__delete()
end

function ChatData:ChatConfig()
	self.config = Config.chat_auto
	self.config_channel = self.config.channel
	self.config_report = self.config.report
	self.config_other = self.config.other[1]
	self.team_recruitment_information = {}
	for k, v in pairs(self.config.team_recruitment_information) do
		table.insert(self.team_recruitment_information, {times = v.times * v.repeat_sending_information, cd_time = v.time})
	end

	Cfg.Register("RobotAutoTalkByGroupIdTalkId", Cfg.RealTimeSearch2Key(Config.robot_auto.auto_talk, "talk_groups", "talk_id"))
end

function ChatData:ChatData()
	self:ChatList()
	self:ChatChannelList()
	self:ChatUnread()
	self:TeamChannelList()
	self:TrumpetInfo()
	self:ShowBack()

	self.world_question_info = SmartData.New{
		now_answer_subject = 0,
		answer_tamp = 0,
	}
	self.bubble_list = {}
	self.msg_type_selects = {}

	self.chat_info_sort = {
		recv_time = 0,
		sort_index = 0
	}
	self.send_grey = SmartData.New{val = false}

	self.world_question_ret = 0
	self.role_answer_id = 0
	self.copy_info = SmartData.New{
		is_show = false
	}
	self.chat_copy_pause = false
end

function ChatData:ChatDataR()
	for _, v in pairs(self.config_channel) do
		if ChatConfig.ChannelId.recruit ~= v.channel_id then
			self.chat_channel_list[v.channel_id]:EmptyChatList()
		end
	end
	for _, info in pairs(self.chat_role_list) do
		info:EmptyInfo()
	end

	self.msg_type_selects = {}
	self.chat_role_list = {}
	
	self.world_question_ret = 0
	self.role_answer_id = 0

	self:SetCurPlayingRec("")
	VoiceRecorder.StopPlayRecord()
end

function ChatData:ChatInfoGet()
    if next(self.chat_cache_list) then
        return table.remove(self.chat_cache_list, #self.chat_cache_list)
	else
        return ChatInfo.New()
    end
end

function ChatData:ChatInfoRecycle(chat_info)
	chat_info:EmptyInfo()
    table.insert(self.chat_cache_list, chat_info)
end

function ChatData:ChatRoleInfoGet()
    if next(self.chat_role_cache_list) then
        return table.remove(self.chat_role_cache_list, #self.chat_role_cache_list)
	else
        return ChatRoleInfo.New{chat_data = self}
    end
end

function ChatData:ChatRoleInfoRecycle(role_info)
	role_info:EmptyInfo()
    table.insert(self.chat_role_cache_list, role_info)
end

function ChatData:ChatList()
	self.chat_channel_list = SmartData.New()
	self.chat_role_list = {}
	self.chat_cache_list = {}
	self.chat_role_cache_list = {}

	self.cur_sel_channel = SmartData.New()
end

function ChatData:ChatChannelList()
	for _, v in pairs(self.config_channel) do
		if ChatConfig.ChannelId.recruit ~= v.channel_id then
			self.chat_channel_list[v.channel_id] = SmartData.New(ChatChannelInfo.New{info = v, chat_data = self})
		end
	end
end

function ChatData:ChatGroupChannelInfo()
	if nil == self.chat_group_channel_info then
		for _, v in pairs(self.config_channel) do
			if ChatConfig.ChannelId.group == v.channel_id then
				self.chat_group_channel_info = v
			end
		end
	end
	return self.chat_group_channel_info
end

function ChatData:GetChatChannelList()
	return self.chat_channel_list
end

function ChatData:GetChatChannelListShow()
	local list = {}
	for _, channel in pairs(self.chat_channel_list) do
		if channel:IsShow() then
			table.insert(list, channel)
		end
	end
	table.sort(list, function (a, b)
		if not a or not b then return a end
		return a:SortValue() < b:SortValue()
	end)
	return list
end

function ChatData:SelChannelId(channel_id, is_sp)
	if is_sp then
		self.sel_channel_id_s = channel_id
	else
		self.sel_channel_id = channel_id
	end
end

function ChatData:GetSelChannelList()
	self.sel_channel_list = self.chat_channel_list[self.sel_channel_id_s or ((TeamData.Instance:InTeam() and (TeamData.Instance:MemberNumRole() > 1)) and ChatConfig.ChannelId.team or self.sel_channel_id)] or self.sel_channel_list
	self.sel_channel_id_s = nil
	self.sel_channel_id = nil
	return self.sel_channel_list
end

function ChatData:GetChatChanneSelList()
	local list = {}
	for _, channel in pairs(self.chat_channel_list) do
		if ChatConfig.ChannelId.system  ~= channel:ChannelId() and ChatConfig.ChannelId.general  ~= channel:ChannelId() and channel:IsShow() then
			table.insert(list, channel)
		end
	end
	return list
end

function ChatData:SetChannelChat(protocol)
	local info = protocol.info
	local channel_id = ChatConfig.ChannelTypeToId[info.channel_type]
	if 0 == info.msg_type and RoleData.Instance:GetRoleId() ~= info.from_uid and self:ChatCacheBlock(info.content) then
		return
	end
	if ChatConfig.ChannelId.group == channel_id then
		SocietyData.Instance:AddGroupChatInfo(info)
	else
		self.chat_channel_list[channel_id]:AddChatInfo(info)
		if self.chat_unread[channel_id] then
			self.chat_unread[channel_id] = self.chat_unread[channel_id] + 1
		end
	end
	if ChatConfig.MsgType.trumpet == info.msg_type then
		table.insert(self.trumpet_info_list, 1, info)
		self.trumpet_switch.val = true
	end
end
function ChatData:SetChannelUnread(channel_id)
	if self.chat_unread[channel_id] then
		self.chat_unread[channel_id] = self.chat_unread[channel_id] + 1
	end
end

function ChatData:ChannelChatCustom(info, channel_id)
	self.chat_channel_list[channel_id]:AddChatInfo(info)
end

function ChatData:GetChatListHideMain(list, chat_list)
	for _, chat_info in pairs(chat_list) do
		if not chat_info:HideMain() then
			table.insert(list, chat_info)
		end
	end
	return list
end

function ChatData:GetMainViewChatList()
	local list = {}
	local chat_channel_list = self:GetChatChanneSelList()
	local sel_recruit = self:ChatMsgTypeIsSel(ChatConfig.MsgType.recruit)
	local sel_bullet = self:ChatMsgTypeIsSel(ChatConfig.MsgType.bullet)
	local sel_all = not sel_recruit and not sel_bullet
	for _, channel in pairs(chat_channel_list) do
		sel_all = sel_all and not channel:IsSel()
	end
	self.msg_type_selects[ChatConfig.MsgType.normal] = sel_all

	for _, channel in pairs(chat_channel_list) do
		if sel_all or channel:IsSel() then
			self:GetChatListHideMain(list, channel:GetChatList())
		end
	end
	if sel_all then
		self:GetChatListHideMain(list, self.chat_channel_list[ChatConfig.ChannelId.system]:GetChatList())
	else
		-- 如果只显示招募信息需要把世界频道的招募信息加上
		if not self.chat_channel_list[ChatConfig.ChannelId.world]:IsSel() and sel_recruit then
			for _, chat_info in pairs(self.chat_channel_list[ChatConfig.ChannelId.world]:GetChatList()) do
				if chat_info:SelMsgTypes(self.msg_type_selects) then
					table.insert(list, chat_info)
				end
			end
		end
	end

	for _, chat_info in pairs(self.chat_channel_list[ChatConfig.ChannelId.general]:GetChatList()) do
		if chat_info:SelMsgTypes(self.msg_type_selects) then
			table.insert(list, chat_info)
		end
	end

	table.sort(list, function (a, b)
		if not a or not b then return a end
        local at = a:MsgTimestamp()
		local bt = b:MsgTimestamp()
		if at == bt then
			return a:SortIndex() < b:SortIndex()
		end
        return at < bt
	end)
	local last_info = list[#list]
	self:SelChannelId(last_info and last_info:SelChannelId() or nil, false)
	return list
end

function ChatData:GetChannelChatListByChannelId(channel_id, no_recruit, no_cross)
	if ChatConfig.ChannelId.general == channel_id then
		local list = {}
		for _, channel in pairs(self.chat_channel_list) do
			local channel_id = channel:ChannelId()
			if ChatConfig.ChannelId.system ~= channel:ChannelId() then
				local chat_list = channel:GetChatList()
				for _, chat_info in pairs(chat_list) do
					if chat_info:InGeneral()
						and (not no_recruit or (not chat_info:IsMsgType(ChatConfig.MsgType.recruit)))
						and (not no_cross or ChatConfig.ChannelId.general ~= channel_id or chat_info:IsMsgType(ChatConfig.MsgType.bullet)) then
						table.insert(list, chat_info)
					end
				end
			end
		end
		table.sort(list, function (a, b)
			if not a or not b then return a end
			local at = a:MsgTimestamp()
			local bt = b:MsgTimestamp()
			if at == bt then
				return a:SortIndex() < b:SortIndex()
			end
			return at < bt
		end)
		return list
	else
		return self.chat_channel_list[channel_id]:GetChatList()
	end
end

function ChatData:GetChannelLimitNumByChannelId(channel_id)
	return self.chat_channel_list[channel_id]:LimitNum()
end

function ChatData:GetChannelColorByChannelId(channel_id)
	return self.chat_channel_list[channel_id]:ChannelColor()
end

function ChatData:ChannelNeedFlushByChannelId(channel_id)
	self.chat_channel_list[channel_id]:NeedFlush()
end

function ChatData:SetSingleChat(protocol)
	local info = protocol.info
	info.is_society = true
	local uid = info.from_uid == RoleId() and info.to_uid or info.from_uid
	self.chat_role_list[uid] = self.chat_role_list[uid] or self:ChatRoleInfoGet():ChatRoleVo(uid)
	self.chat_role_list[uid]:AddChatInfo(info)
	SocietyData.Instance:SetFriendChatRedPoint(uid, math.abs(TimeHelper.GetServerTime() - info.msg_timestamp) < 3)
	SocietyData.Instance:RecentlyChatCheck(info)
	if uid == SocietyData.Instance.quick_chat_info.cur_sel then
		SocietyData.Instance:FlushChatQuickRecords()
	end
end

function ChatData:SetChannelChatRobotBridge(auto_talk)
	local t = {}
    t.from_uid = 0
    t.name = auto_talk.name
    t.avatar_type = 0
    t.prof = 0
    t.avatar_id = auto_talk.head_icon
	t.avatar_quality = auto_talk.avatar_quality
    t.content_type = ChatConfig.ChatContentType.text
    t.avatar_timestamp = 0
    t.level = auto_talk.level
    t.channel_type = ChatConfig.ChannelIdToType[ChatConfig.ChannelId.general]
    t.top_level = 0
    t.msg_timestamp = TimeHelper.GetServerTime()

    t.face_chatframe = -1
    t.face_signature = -1

    t.msg_type = ChatConfig.MsgType.normal
    t.group_id = 0

    t.family = ""

    t.item_list = {}
    for i = 1, ChatConfig.CHAT_MSG_ITEM_INFO_NUM_MAX do
        t.item_list[i] = {
			type = 0,
			item_id = 0,
			show = 0,
			show2 = 0,
			sign_id = string.format("%ss%s", 0, 0),
        }
    end

    t.content = auto_talk.talk
	t.msg_length = string.len(t.content)

	self:SetChannelChat({info = t})
end

function ChatData:SetSingleChatBridge(friend, content, msg_timestamp, msg_type)
	local info = friend.info
	local t = {}
	t.from_uid = info.uid
	t.name = info.name
	t.avatar_type = info.avatar_type
	t.prof = info.prof
	t.avatar_id = info.avatar_id
	t.avatar_quality = info.avatar_quality
	t.authority_type = 0
	t.content_type = ChatConfig.ChatContentType.text
	t.level = info.level
	t.top_level = info.top_level
	t.avatar_timestamp = info.avatar_timestamp
	t.msg_timestamp = msg_timestamp or TimeHelper.GetServerTime()

	t.face_chatframe = 0
	t.face_signature = 0

	t.msg_type = msg_type or 0

	t.item_list = {}
	for i = 1, ChatConfig.CHAT_MSG_ITEM_INFO_NUM_MAX do
		t.item_list[i] = {
			type = 0,
			item_id = 0,
			show = 0,
			sign_id = string.format("%ss%s", 0, 0),
		}
	end

	t.content = content
	t.msg_length = string.len(t.content)
	self:SetSingleChat({info = t})
end

function ChatData:GetAutoTalkIsOff()
	return RoleData.Instance:GetRoleLevel() > Config.robot_auto.other[1].off_auto_talk
end

function ChatData:AddChatInfoByUid(uid, content, chat_items, content_type, msg_timestamp)
	self.chat_role_list[uid] = self.chat_role_list[uid] or self:ChatRoleInfoGet():ChatRoleVo(uid)
	self.chat_role_list[uid]:AddChatInfoLocal(content, chat_items, content_type, msg_timestamp)
end

function ChatData:GetRoleChatListByUid(uid)
	if nil == uid or nil == self.chat_role_list[uid] then return {} end
	return self.chat_role_list[uid]:GetChatList()
end

function ChatData:CheckDownState()
	local channel_id = self.cur_sel_channel:ChannelId()
	if ChatConfig.ChannelId.guild == channel_id then
		if GuildData.Instance:GetGuildID() == 0 then return false end
	elseif ChatConfig.ChannelId.team == channel_id then
		if not TeamCtrl.Instance:Data():InTeam() then return false end
	elseif ChatConfig.ChannelId.system == channel_id then
		return false
	elseif ChatConfig.ChannelId.trumpet == channel_id then
		return false
	end
	return true
end

function ChatData:VoiceStart()
	self.voice_ready = false
	self.timer_dt = TimeHelper:AddDelayTimer(function ()
		self.voice_ready = true
		self.is_voice_up = false
		self.voice_start_time = TimeHelper.RealTime()
		self.min_voice_y = UnityEngine.Input.mousePosition.y
		ViewMgr:OpenView(ChatViewVoiceTips)
		VoiceRecorder.StartRecording()
	end, ChatConfig.VOICE_READY_TIME)
end

function ChatData:VoiceFinish(suc_func)
	TimeHelper:CancelTimer(self.timer_dt)
	if not self.voice_ready then return end
	self.voice_ready = false
	ViewMgr:CloseView(ChatViewVoiceTips)
	local interval = TimeHelper.RealTime() - self.voice_start_time
	local stop_func = nil
	if not self.is_voice_up then
		if interval > ChatConfig.VOICE_MIN_INTERVAL then
			-- PublicPopupCtrl.Instance:Center("VoiceFinish!")
			stop_func = function(com_code,rec_key,time)
				if com_code == GVoiceComCode.UploadRecordDone then
					-- ChatCtrl.Instance:SendChannelVoice(rec_key,time)
					-- self.cur_sel_channel:SendGrey()
					if suc_func then
						suc_func(rec_key,time)
					end
				elseif com_code == GVoiceErrorNo.PermissionMicErr then
					PublicPopupCtrl.Instance:Center(Language.Chat.RecordMicPermissionErr)
				end
			end
		else
			PublicPopupCtrl.Instance:Center(string.format(Language.Chat.Tips.min_interval, ChatConfig.VOICE_MIN_INTERVAL))
		end
	else
		-- PublicPopupCtrl.Instance:Center("VoiceCanel!")
	end

	VoiceRecorder.StopRecording(stop_func)
end

function ChatData:VoiceCheck()
	if not self.voice_ready or self.is_voice_up then return end
	local cur_voice_y = UnityEngine.Input.mousePosition.y 
	if cur_voice_y < self.min_voice_y then self.min_voice_y = cur_voice_y end
	if cur_voice_y - self.min_voice_y > ChatConfig.VOICE_SENSITIVITY then
		self.is_voice_up = true
		ViewMgr:FlushView(ChatViewVoiceTips)
	end
end

function ChatData:ChatUnread()
	self.chat_unread = SmartData.New{
		[ChatConfig.ChannelId.guild] = 0,
		[ChatConfig.ChannelId.team] = 0,
	}
end

function ChatData:EmptyChatUnread(channel_id)
	if self.chat_unread[channel_id] then
		self.chat_unread[channel_id] = 0
	end
end

function ChatData:EmptyChannelTeamChatList()
	self.chat_channel_list[ChatConfig.ChannelId.team]:EmptyChatList()
end

function ChatData:TeamChannelList()
	self.team_channel_list = SmartData.New{}
end

function ChatData:SetTeamChannelInfo(protocol)
	if ChatConfig.TeamChannelReason.normal == protocol.reason then
		self.team_channel_list:Set{}
	end
	local role_level = RoleData.Instance:GetRoleLevel()
	for _, info in pairs(protocol.team_list) do
		local target = TeamData.Instance:GetTeamGoalWithId(info.team_type)
		info.target = target
		info.role_level = role_level
		self.team_channel_list[info.team_index] = ChatConfig.TeamChannelReason.remove ~= protocol.reason and info or nil
	end
end

function ChatData:GetTeamChannelList()
	return self.team_channel_list
end

function ChatData:SetSpeaker(protocol)
	table.insert(self.trumpet_info_list, 1, protocol.info)
	self.trumpet_switch.val = true
end

function ChatData:TrumpetInfo()
	self:TrumpetItems()

	self.trumpet_info_list = {}
	self.trumpet_switch = SmartData.New({val = false})
end

function ChatData:TrumpetItems()
	self.trumpet_ic = Item.New()
	self.trumpet_nc = Item.New()
	self.trumpet_ic.item_id = ChatConfig.ItemTrumpet.is_cross
	self.trumpet_nc.item_id = ChatConfig.ItemTrumpet.not_cross
end

function ChatData:NextTrumpetInfo()
    if next(self.trumpet_info_list) then
        return true, table.remove(self.trumpet_info_list, #self.trumpet_info_list)
    else
        self.trumpet_switch.val = false
        return false
    end
end

function ChatData:ShowBack()
	self.show_back = SmartData.New{ val = false }
end

function ChatData:SetShowBack(value)
	self.show_back.val = value
end

function ChatData:GetSocietyLimitNum()
	return self.config_other.limit
end

function ChatData:getTeamSendTimes()
	local day_t = TimeCtrl.Instance:GetServerDate()
	local role_team_notice = UnityPlayerPrefs.GetString(PrefKeys.TeamNotice(RoleData.Instance:GetRoleId()))
	if not StringIsEmpty(role_team_notice) then
		local splited_str = string.split(role_team_notice,"_")
		if splited_str and splited_str[1] ~= day_t.year..day_t.month..day_t.day then
			return 0
		elseif splited_str and tonumber(splited_str[2]) then
			return tonumber(splited_str[2])
		end
	else
		return 0
	end
end

function ChatData:GetTeamInvitationCd(add_times)
	
	local sended_times = self:getTeamSendTimes()
	for k, v in pairs(self.team_recruitment_information) do
		if k == 1 and sended_times < v.times then
			if add_times then
				local day_t = TimeCtrl.Instance:GetServerDate()
				UnityPlayerPrefs.SetString(PrefKeys.TeamNotice(RoleData.Instance:GetRoleId()),day_t.year..day_t.month..day_t.day.."_"..(sended_times + 1))
			end
			return self.config_other.team_invitation_cd
		elseif sended_times - 5 < v.times or k == #self.team_recruitment_information then
			if add_times then
				local day_t = TimeCtrl.Instance:GetServerDate()
				UnityPlayerPrefs.SetString(PrefKeys.TeamNotice(RoleData.Instance:GetRoleId()),day_t.year..day_t.month..day_t.day.."_"..(sended_times + 1))
			end
			return v.cd_time
		end
	end	
end

function ChatData:GetGuideLevel()
	return self.config_other.guide_level
end

function ChatData:SetCurPlayingRec(rec_key)
	self.record_data.cur_playing_key = rec_key
end

function ChatData:RecordData()
	return self.record_data
end

function ChatData:ClearGuildRecord()
	self.chat_channel_list[ChatConfig.ChannelId.guild]:EmptyChatList()
	self:EmptyChatUnread(ChatConfig.ChannelId.guild)
end

function ChatData:ClearFriendRecord()
	for _, info in pairs(self.chat_role_list) do
		info:EmptyInfo()
	end

	SocietyData.Instance:AddShiTuSyncInfo()
end

function ChatData:GetBubbleCo(bubble_id)
	if nil == self.bubble_list[bubble_id] then
		self.bubble_list[bubble_id] = {}
		local co = Config.face_cfg_auto.bubble_pos[bubble_id]
		if co then
			self.bubble_list[bubble_id][1] = ChatData.BubbleParse(co.bubble_lt_)
			self.bubble_list[bubble_id][2] = ChatData.BubbleParse(co.bubble_ld_)
			self.bubble_list[bubble_id][3] = ChatData.BubbleParse(co.bubble_rd_)
			self.bubble_list[bubble_id][4] = ChatData.BubbleParse(co.bubble_rt_)
		end
	end
	return self.bubble_list[bubble_id]
end

function ChatData.BubbleParse(str)
	if StringIsEmpty(str) then return end
	local pos_t = string.split(str, "|")
	return {x = tonumber(pos_t[1]), y = tonumber(pos_t[2])}
end

function ChatData:ChatMsgTypeIsSel(msg_type)
	if nil == self.msg_type_selects[msg_type] then
        self.msg_type_selects[msg_type] = UnityPlayerPrefs.GetInt(PrefKeys.ChatMsgTypeIsSel(RoleData.Instance:GetBaseData().role_id, msg_type)) == 1
    end
    return self.msg_type_selects[msg_type]
end

function ChatData:ChatMsgTypeSetIsSel(msg_type, value)
	self.msg_type_selects[msg_type] = value
    UnityPlayerPrefs.SetInt(PrefKeys.ChatMsgTypeIsSel(RoleData.Instance:GetBaseData().role_id, msg_type), value and 1 or 0)
end

function ChatData:ChatInfoSortIndex(msg_timestamp)
	self.chat_info_sort.sort_index = self.chat_info_sort.recv_time == msg_timestamp and (self.chat_info_sort.sort_index + 1) or 1
	self.chat_info_sort.recv_time = msg_timestamp
	return self.chat_info_sort.sort_index
end

function ChatData:SendGrey()
	self.cur_sel_channel:Val():SendGrey()
	self.send_grey.val = not self.send_grey.val
end

function ChatData:ChatCacheCheck(content, channel_id)
	local value1, value2 = false, false
	local role_id = RoleData.Instance:GetRoleId()
	self.chat_check_cache1[channel_id] = self.chat_check_cache1[channel_id] or {}
	self.chat_check_cache2[channel_id] = self.chat_check_cache2[channel_id] or {}
	self.chat_check_cache1[channel_id][role_id] = self.chat_check_cache1[channel_id][role_id] or {}
	self.chat_check_cache2[channel_id][role_id] = self.chat_check_cache2[channel_id][role_id] or {}
	for k, v in pairs(self.chat_check_cache1[channel_id][role_id]) do
		value1 = value1 or self:CheckCache1(v, content)
	end
	for k, v in pairs(self.chat_check_cache2[channel_id][role_id]) do
		value2 = value2 or self:CheckCache2(v, content)
	end

	if value2 then
		PublicPopupCtrl.Instance:Center(Language.Chat.CacheCheckTips2)
	elseif value1 then
		PublicPopupCtrl.Instance:Center(Language.Chat.CacheCheckTips1)
	else
		table.insert(self.chat_check_cache1[channel_id][role_id], 1, content)
		if DataHelper.GetStringWordNum(content) >= ChatConfig.CHAT_CHACHE_WORD_NUM then
			table.insert(self.chat_check_cache2[channel_id][role_id], 1, content)
		end
		if #self.chat_check_cache1[channel_id][role_id] > ChatConfig.CHAT_CHECK_CHACHE_NUM_MAX then self.chat_check_cache1[channel_id][role_id][ChatConfig.CHAT_CHECK_CHACHE_NUM_MAX + 1] = nil end
		if #self.chat_check_cache2[channel_id][role_id] > ChatConfig.CHAT_CHECK_CHACHE_NUM_MAX then self.chat_check_cache2[channel_id][role_id][ChatConfig.CHAT_CHECK_CHACHE_NUM_MAX + 1] = nil end
	end
	return value1 or value2
end

-- 重复刷屏发言
function ChatData:CheckCache1(cache, content)
	return content == cache
end

-- 对发送的内容进行控制
function ChatData:CheckCache2(cache, content)
	local t = DataHelper.GetStringWordToTable(cache)
	local cache_num = #t
	local half_num = math.floor(cache_num / 2)
	local pattern1, pattern2 = "", ""
	for i = 1, cache_num do
		pattern1 = pattern1 .. (i < half_num and t[i] or "")
		pattern2 = pattern2 .. (i >= half_num and t[i] or "")
	end
	content = RichTextHelper.RemoveSignBeforeMatch(content)
	pattern1 = RichTextHelper.RemoveSignBeforeMatch(pattern1)
	pattern2 = RichTextHelper.RemoveSignBeforeMatch(pattern2)
	return string.find(content, pattern1) or string.find(content, pattern2)
end

function ChatData:ChatCacheBlock(content)
	local value = false
	if DataHelper.GetStringWordNum(content) < ChatConfig.CHAT_CHACHE_WORD_NUM then return value end
	local role_id = RoleData.Instance:GetRoleId()
	self.chat_block_cache[role_id] = self.chat_block_cache[role_id] or {}
	for k, v in pairs(self.chat_block_cache[role_id]) do
		value = value or self:BlockCache(v, content)
	end
	if not value then
		table.insert(self.chat_block_cache[role_id], 1, content)
	end
	if #self.chat_block_cache[role_id] > ChatConfig.CHAT_BLOCK_CHACHE_NUM_MAX then self.chat_block_cache[role_id][ChatConfig.CHAT_BLOCK_CHACHE_NUM_MAX + 1] = nil end
	return value
end

function ChatData:BlockCache(cache, content)
	return content == cache
end

function ChatData:CopyStart(item, chat_info)
	self.chat_copy_pause = true
	TimeHelper:CancelTimer(self.timer_dt_copy)
	self.timer_dt_copy = TimeHelper:AddDelayTimer(function ()
		self.copy_info:Set{
			is_show = true,
			target = item,
			chat_info = chat_info,
		}
	end, ChatConfig.COPY_READY_TIME)
end

function ChatData:CopyFinish()
	TimeHelper:CancelTimer(self.timer_dt_copy)
	self.chat_copy_pause = self.copy_info.is_show
end

-- 遍历传入的频道里所有的chat_info
--func(chat_info) return:返回true表示直接结束遍历
function ChatData:ForeachChatInfo(channel_id,func)
	local channel_info = self.chat_channel_list[channel_id]
	if channel_info == nil then return end
	local chat_list = channel_info:GetChatList()
	for _,v in pairs(chat_list) do
		local ret = func(v)
		if ret == true then
			return
		end
	end
end

--有人领取了红包时的数据处理
--刷新本地聊天红包中红包的领取状态
--显示哪些玩家领取了红包
--protocol[SCHongBaoBaseInfoChange]
function ChatData:OnHongBaoBaseInfoChange(protocol)
	if protocol.change_type ~= 0 then
		Debuger.LogError("ChatCtrl:OnHongBaoBaseInfoChange ERROR,Invalid protocol change_type=%s",tostring(protocol.change_type))
		return
	end
	local chat_info_content = nil
	self:ForeachChatInfo(ChatConfig.ChannelId.guild,function(chat_info) 
		if not chat_info:IsHongbao() then return end
		local content = chat_info:GetContent()
		if content.id == protocol.hong_bao_id then
			chat_info_content = content
			return true
		end
	end)
	if chat_info_content == nil then		--聊天中没有找到相应的红包信息，不做任何逻辑
		return
	end
	local get_player_info = protocol.get_player_info
	if get_player_info.uid == RoleData.Instance:GetRoleId() then		--玩家领取了，刷新对应红包的ChatInfo里的smart_data
		chat_info_content.sm_data.self_getted = true
	end

	--红包领取信息数据桥接
	local cfg = nil
	for _,v in pairs(Config.family_auto.redbag) do
		if v.redbag_type == protocol.hongbao_type then
			cfg = v
			break
		end
	end
	local lucky_type = 0	--0,普通;1,手气王;2,倒霉鬼
	if cfg ~= nil then
		if get_player_info.val >= cfg.redbag_max then
			lucky_type = 1
		elseif get_player_info.val <= cfg.redbag_min then
			lucky_type = 2
		end
	end
	if lucky_type ~= 0 then		--领取提示只显示手气王和倒霉蛋
		local info = {}
		info.channel_type = ChatConfig.ChannelId.guild
		info.content = string.format("{hongbao_get;%s;%d;%d;%d}",get_player_info.name,get_player_info.val,CommonItemId.ImmortalCoin,lucky_type)
		-- info.block_role = true
		info.hide_main = true
		-- self:SetChannelChat(protocol)
		ChatCtrl.Instance:ChannelChatCustom(info,ChatConfig.ChannelId.guild)
	end
end

-- 家族有人使用了建材
function ChatData:SetGuildUseMaterial(protocol)
	local info = {}
	info.channel_type = ChatConfig.ChannelId.guild
    info.hide_main = false
	info.block_role = true
	local role_name = ColorStr(protocol.info.name, COLORSTR.Blue7)
	if protocol.info.op_type == GuildBuildingNotifyType.UseItem then
		local item = Item.Init(protocol.info.item_id)
		local infoHandle = RichTextHelper.StoreInfo(item)
		local item_name = RichTextHelper.LinkStr(item:QuaName(true), "infoHandle", infoHandle, "_ul", QualityColorStrShallow[item:Color()])
		local money = item:Config().param1 * protocol.info.num
		local money_str = ColorStr(money, COLORSTR.Green3)
		info.content = Format(Language.GuildBuilding.UseMaterial, role_name, item_name, protocol.info.num, money_str)
		ChatCtrl.Instance:ChannelChatCustom(info, ChatConfig.ChannelId.guild)
		-- 这里判断是不是自己用的
		if RoleId() == protocol.info.role_id then
			local item = Item.Create({item_id = CommonItemId.GuildMoney, num = money, is_bind = 1})
			-- ChatCtrl.Instance:ChannelChatCustom({item_get = {item = item, num = money}})
			PublicPopupCtrl.Instance:CenterI({content = Language.GuildBuilding.UseMaterialCenter, item_id = CommonItemId.GuildMoney, num = money, color_str = item:ColorStr(), item = item, icon_id = item:IconId()})
		end
	elseif protocol.info.op_type == GuildBuildingNotifyType.StartSpeed then
		local build_name = GuildBuildingData.Instance:BuildingName(protocol.info.build_type)
		local post_name = GuildData.Instance:GetJobName(protocol.info.role_id)
		info.content = Format(Language.GuildBuilding.NowOpenSpeedUpTip, post_name, role_name, build_name)
		ChatCtrl.Instance:ChannelChatCustom(info, ChatConfig.ChannelId.guild)
	elseif protocol.info.op_type == GuildBuildingNotifyType.SpeedUp then
		local build_name = GuildBuildingData.Instance:BuildingName(protocol.info.build_type)
		info.content = Format(Language.GuildBuilding.HelpSpeedUpTip, role_name, build_name)
		ChatCtrl.Instance:ChannelChatCustom(info, ChatConfig.ChannelId.guild)
	elseif protocol.info.op_type == GuildBuildingNotifyType.SpeedUpFinish then
		local build_name = GuildBuildingData.Instance:BuildingName(protocol.info.build_type)
		info.content = Format(Language.GuildBuilding.SpeededFinishTip, build_name)
		ChatCtrl.Instance:ChannelChatCustom(info, ChatConfig.ChannelId.guild)
	end
end