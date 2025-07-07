
FILE_REQ("modules/chat/chat_config")
FILE_REQ("modules/chat/chat_info")
FILE_REQ("modules/chat/chat_item")
FILE_REQ("modules/chat/chat_channel_info")
FILE_REQ("modules/chat/chat_role_info")
if VersionTag.HasTag(VersionTag.LuaPegLib) then
	FILE_REQ("modules/chat/chat_filter_lpeg")
end
FILE_REQ("modules/chat/chat_filter")

FILE_REQ("modules/chat/chat_data")
FILE_REQ("modules/chat/chat_data_quick")
FILE_REQ("modules/chat/chat_data_world_question")
FILE_REQ("modules/chat/voice_recorder")
FILE_REQ("modules/chat/chat_sub_custom")

VIEW_REQ("modules/chat/chat_view")
VIEW_REQ("modules/chat/chat_view_channel_sel")
VIEW_REQ("modules/chat/chat_quick_view")
VIEW_REQ("modules/chat/chat_view_voice_tips")
VIEW_REQ("modules/chat/chat_trumpet_view")

ChatCtrl = ChatCtrl or BaseClass(BaseCtrl)

function ChatCtrl:__init()
	if ChatCtrl.Instance ~= nil then
		Debuger.LogError("[ChatCtrl] attempt to create singleton twice!")
		return
	end	
	ChatCtrl.Instance = self
	self.chat_filter = ChatFilter.New()
	self.data = ChatData.New()
	self.stop = false
	self:RegisterAllProtocols()

	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))
	self.handle_level = RoleData.Instance:GetBaseData():KeysCare({"level"}, BindTool.Bind(self.OnRoleLevelChange, self))
end

function ChatCtrl:__delete()
	if ChatCtrl.Instance == self then
		ChatCtrl.Instance = nil
	end

	EventSys:UnBind(self.complete_handle)
	RoleData.Instance:GetBaseData():Uncare(self.handle_level)

	self:UnRegisterAllProtocols()
end

function ChatCtrl:OnUnloadGameLogic()
	self.data:ChatDataR()
end

function ChatCtrl:OnReboot()
	VoiceRecorder.StopPlayRecord()
end

function ChatCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSChannelChat)
	self:RegisterProtocol(CSSingleChat)
	self:RegisterProtocol(CSSpeaker)
	self:RegisterProtocol(CSChatItemInfoReq)
	self:RegisterProtocol(CSChatSimpleRecordSet)
	self:RegisterProtocol(CSUserChatRecord)

	self:RegisterProtocol(SCChannelChat, "OnChannelChat")
	self:RegisterProtocol(SCSingleChat, "OnSingleChat", true)
	self:RegisterProtocol(SCTeamChannelInfo, "OnTeamChannelInfo")
	self:RegisterProtocol(SCSpeaker, "OnSpeaker")
	self:RegisterProtocol(SCChatItemInfoRet, "OnChatItemInfoRet")
	self:RegisterProtocol(SCNoticeClientReloadSensitiveWords, "OnSCNoticeClientReloadSensitiveWords")
	self:RegisterProtocol(SCWorldQuestionNotice, "OnWorldQuestionNotice")
	self:RegisterProtocol(SCPlayerIsForbidden,"OnSCPlayerIsForbidden")
	self:RegisterProtocol(SCChatSimpleRecordInfo,"OnChatSimpleRecordInfo")
	self:RegisterProtocol(SCMutePlayerDeleteChatRecord,"OnMutePlayerDeleteChatRecord")
	self:RegisterProtocol(SCRoleAutoTalkInfo,"OnRoleAutoTalkInfo")
end

function ChatCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCChannelChat)
	self:UnRegisterProtocol(SCSingleChat)
	self:UnRegisterProtocol(SCTeamChannelInfo)
	self:UnRegisterProtocol(SCSpeaker)
end

function ChatCtrl:RoleNoticeComplete()
	if not LoginData.Instance:IsOnCrossSever() then
		self.data:ClearGuildRecord()
	end
	self:GetUserChatRecord()
end

function ChatCtrl:GetUserChatRecord()
	local protocol = GetProtocol(CSUserChatRecord)
	SendProtocol(protocol)
end

function ChatCtrl:OnRoleLevelChange(old)
    if old and 0 == old.level then
        return
    end
    -- local level = RoleData.Instance:GetRoleLevel()
    -- if level == self.data:GetGuideLevel() then
	-- 	local cqsl = SocietyData.Instance.quick_chat_info.show_list
	-- 	if 0 == #cqsl then
	-- 		SocietyData.Instance:QuickChatAdd(SocietyConfig.QuickShowType.add)
	-- 	end
	-- end
end

function ChatCtrl:OnChannelChat(protocol)
	self.data:SetChannelChat(protocol)
	if protocol.info.from_uid == RoleData.Instance:GetRoleId() then
		TimeHelper:CancelTimer(self.handle_check)
		self.send_msg = true
	end
end

function ChatCtrl:OnSingleChat(protocol)
	if not RoleSettingData.IsShield(ROLE_SETTING_TYPE.SET_SHOW_REFUSE_STRANGER_MSG) and not SocietyData.Instance:IsFriend(protocol.info.from_uid) then return end
	self.data:SetSingleChat(protocol)
end

function ChatCtrl:OnTeamChannelInfo(protocol)
	self.data:SetTeamChannelInfo(protocol)
end

function ChatCtrl:OnSpeaker(protocol)
	self.data:SetSpeaker(protocol)
end

function ChatCtrl:OnChatItemInfoRet(protocol)
	local item = Item.Create(protocol.info)
	if item:IsPet() then
		item.param.index = -1
		local pet = Pet.New(item.param)
		--LogError("OnChatItemInfoRet",protocol.info.attr_list)
		pet:SetAttrList(protocol.info.attr_list)
		ViewMgr:OpenView(PetDetailView, pet)
	else
		ItemInfoCtrl.Instance:ItemInfoView{item_list = {item}}
	end
end

function ChatCtrl:OnWorldQuestionNotice(protocol)
	self.data:SetWorldQuestionNotice(protocol)
end

function ChatCtrl:OnChatSimpleRecordInfo(protocol)
	self.data:SetChatSimpleRecordInfo(protocol)
end

function ChatCtrl:OnMutePlayerDeleteChatRecord(protocol)
	self.data:SetMutePlayerDeleteChatRecord(protocol)
end

function ChatCtrl:OnRoleAutoTalkInfo(protocol)
	if 0 == protocol.notice_type or IS_AUDIT_VERSION then return end
	local co = Cfg.RobotAutoTalkByGroupIdTalkId(protocol.talk_groups_id, protocol.talk_id)
	if co then
		self.data:SetChannelChatRobotBridge(co)
	end
end

--服务器通知后台屏蔽词有更新并发送申请获取
function ChatCtrl:OnSCNoticeClientReloadSensitiveWords(protocol)
	ChatFilter.Instance:GetHttpFilterKey()
end

function ChatCtrl:SendChannelChat(param_t)
	if ChatConfig.IS_BLOCK then
		PublicPopupCtrl.Instance:Center(Language.Chat.BlockTips)
		return
	end

	local protocol = GetProtocol(CSChannelChat)
	param_t.content = param_t.content or ""
	protocol.content_type = param_t.content_type or 0
	protocol.channel_type = param_t.channel_type or self.data.cur_sel_channel:ChannelType() or 0
	protocol.msg_length = string.len(param_t.content)
	protocol.content = param_t.content
	protocol.chat_items = param_t.chat_items or {}
	protocol.group_id = param_t.group_id or {}
	SendProtocol(protocol)
	self.send_msg = false
	--当发送时 进行检查如果XX秒没有回复则执行下面
	local send_time = os.clock()
	TimeHelper:CancelTimer(self.handle_check)
	self.handle_check = TimeHelper:AddRunFrameTimer(function ()
		if self.send_msg == false then
			if os.clock() - send_time > 1 then
				self:SendChannelChatError(protocol)
			end
		end
	end, 1, 4)
end
function ChatCtrl:SendChannelChatError(protocol)
	--转化为2132协议格式
	local t = {}
    t.from_uid = RoleData.Instance:GetRoleId()
    t.name = RoleData.Instance:GetRoleName()
    t.avatar_type = RoleData.Instance:GetRoleAvatarType()
    t.prof = RoleData.Instance:GetRoleProfId()
    t.content_type = protocol.content_type
    t.avatar_timestamp = 0
    t.level = RoleData.Instance:GetRoleLevel()
    t.channel_type = protocol.channel_type
    t.msg_timestamp = os.time()

    t.face_chatframe = AppearanceData.Instance:GetEquipResSeq(AppearanceConfig.Type.Bubble)  -- 聊天框|冒泡框
	t.face_signature = AppearanceData.Instance:GetEquipResSeq(AppearanceConfig.Type.TailTag) -- 签名档|尾标
	
	t.msg_type = protocol.msg_type           	-- ChatConfig.MsgType
    t.group_id = protocol.group_id          	-- 群组id 非群组聊天填0

    t.family = RoleData.Instance:GetGuildName()
    t.item_list = {}
	for i = 1, ChatConfig.CHAT_MSG_ITEM_INFO_NUM_MAX do
		local chat_item = protocol.chat_items[i] or ChatItem.EmptyItem()
        t.item_list[i] = {
            type = chat_item:Type(),
            item_id = chat_item.item and chat_item.item.item_id or 0,
            sign_id = 0,
        }
    end
    t.msg_length = protocol.msg_length
	t.content = protocol.content
	t.warn = true
	local res = {info = t}
	self.send_msg = true
	self:OnChannelChat(res)
end
function ChatCtrl:SendChannelChatText(content, chat_items, channel_type, group_id)
	local param_t = {
		content_type = ChatConfig.ChatContentType.text,
		chat_items = chat_items,
		content = content,
		channel_type = channel_type,
		group_id = group_id,
	}
	self:SendChannelChat(param_t)
	local t = {
		channel_id = ChatConfig.ChannelTypeToId[channel_type or self.data.cur_sel_channel:ChannelType()],
		param = group_id and SocietyData.GroupId2Key(group_id) or nil,
		content = content,
	}
	table.insert(self.data.report_list, t)
end

function ChatCtrl:SendChannelVoice(rec_key, time, channel_type, group_id)
	local param_t = {
		content_type = ChatConfig.ChatContentType.audio,
		content = string.format("%s;%d",rec_key,Mathf.Round(time)),
		channel_type = channel_type,
		group_id = group_id,
	}
	self:SendChannelChat(param_t)
	VoiceRecorder.SpeechToText(rec_key,function(com_code,rec_key,ret)
		if com_code == GVoiceComCode.STTSucc and not StringIsEmpty(ret) then
			local t = {
				channel_id = ChatConfig.ChannelTypeToId[channel_type or self.data.cur_sel_channel:ChannelType()],
				param = group_id and SocietyData.GroupId2Key(group_id) or nil,
				content = string.format("voice_%s", ret),
			}
			table.insert(self.data.report_list, t)
        end
    end)
end

function ChatCtrl:SendSingleChat(content, chat_items, content_type)
	if ChatConfig.IS_BLOCK then
		PublicPopupCtrl.Instance:Center(Language.Chat.BlockTips)
		return
	end
	
	local protocol = GetProtocol(CSSingleChat)
	protocol.content_type = content_type or 0
	protocol.to_uid = SocietyData.Instance.cur_sel_role.info.uid or 0
	protocol.msg_length = string.len(content)
	protocol.content = content
	protocol.chat_items = chat_items or {}
	SendProtocol(protocol)
	if ChatConfig.ChatContentType.audio == protocol.content_type then
		VoiceRecorder.SpeechToText(content,function(com_code,rec_key,ret)
			if com_code == GVoiceComCode.STTSucc and not StringIsEmpty(ret) then
				local t = {
					channel_id = ChatConfig.ChannelId.single,
					param = protocol.to_uid,
					content = string.format("voice_%s", ret),
					toName = SocietyData.Instance.cur_sel_role.info.name or ""
				}
				table.insert(self.data.report_list, t)
			end
		end)
	else
		local t = {
			channel_id = ChatConfig.ChannelId.single,
			param = protocol.to_uid,
			content = content,
			toName = SocietyData.Instance.cur_sel_role.info.name or ""
		}
		table.insert(self.data.report_list, t)
	end
end

-- function ChatCtrl:SendSingleChatText(content, chat_items)
-- 	local param_t = {
-- 		content_type = ChatConfig.ChatContentType.text,
-- 		chat_items = chat_items,
-- 		content = content,
-- 	}
-- 	self:SendSingleChat(param_t)
-- end

function ChatCtrl:SendSpeaker(content)
	local protocol = GetProtocol(CSSpeaker)
	local trumpet_item = ItemInfoCtrl.Instance:Data():GetItemData()
	protocol.speaker_type = ChatConfig.Trumpet2SpeakerType[trumpet_item and trumpet_item.item_id or ChatConfig.ItemTrumpet.not_cross] or 0
	protocol.msg_length = string.len(content)
	protocol.content = content
	SendProtocol(protocol)
	local t = {
		channel_id = ChatConfig.ChannelId.trumpet,
		param = protocol.speaker_type + 1,
		content = content,
	}
	table.insert(self.data.report_list, t)
end

function ChatCtrl:SendChatItemInfoReq(sign_id)
	local protocol = GetProtocol(CSChatItemInfoReq)
	protocol.sign_id = {}
	for _, val in ipairs(string.split(sign_id, "s")) do
		table.insert(protocol.sign_id, tonumber(val))
	end
	SendProtocol(protocol)
end

function ChatCtrl:SendChatSimpleRecordSet(index, record)
	local protocol = GetProtocol(CSChatSimpleRecordSet)
	protocol.index = index
	protocol.record = record
	SendProtocol(protocol)
end

function ChatCtrl:ChannelChatCustom(info, channel_id)
	if self.stop == true then return end
	if nil == info then
		LogError("ChannelChatCustom error info=nil")
		return
	end
	channel_id = channel_id or ChatConfig.ChannelId.system
	self.data:ChannelChatCustom(info, channel_id)
end

function ChatCtrl:OnSCPlayerIsForbidden(protocol)
	if protocol.is_forbidden == 0 and #self.data.report_list > 0 then
		local t = table.remove(self.data.report_list, 1)
		if nil == t then return end
		self:SendChatSub(t.channel_id, t.param, t.content, t.toName)
	elseif protocol.is_forbidden == 1 then
		if protocol.forbid_talk_type == ChatConfig.FORBID_TALK_TYPE.FORBID_TALK_TYPE_DEFAULT then
			local time_str = os.date(Language.Chat.ChatForbidenTimeFormat, protocol.forbid_talk_end_timestamp)
			local agent_adapt_info = GLOBAL_CONFIG:AgentAdaptInfoById(GLOBAL_CONFIG:PkgInfo().agent_id)
			if agent_adapt_info ~= nil then
				if RoleData.Instance:GetRoleLevel() >= (agent_adapt_info.forbidden_tips_level or 40) then
					PublicPopupCtrl.Instance:Center(string.format(Language.Chat.ChatForbidenTips, time_str))
				end
			elseif IS_EDITOR then
				PublicPopupCtrl.Instance:Center(string.format(Language.Chat.ChatForbidenTips, time_str))
			end
		elseif protocol.forbid_talk_type == ChatConfig.FORBID_TALK_TYPE.FORBIT_TALK_TYPE_SILENCE then
			local t = table.remove(self.data.report_list, 1)
		end
	end
end

-- 聊天上报
-- channel_id	|param
-- 1  -世界		|
-- 2  -当前X	|
-- 3  -家族		|
-- 4  -组队		|
-- 5  -招募X	|
-- 6  -职业		|
-- 7  -系统X	|
-- 8  -喇叭		|1本服2跨服
-- 9  -综合		|
-- 10 -群聊		|群组id
-- 100-私聊		|对方role_id

-- content
-- 语音加前缀voice_
function ChatCtrl:SendChatSub(channel_id, param, content, toName)
	if StringIsEmpty(content) then return end
	local sub_content = GameUtil.EncodeB64(content)
	local uuid = GLOBAL_CONFIG:PkgInfo().device_id
	if VersionTag.HasTag(VersionTag.FileInfoJson) then
		local temp = MainProber.GetUUID()
		if not IS_IOS() then
			uuid = temp
		else
			if tonumber(string.sub(temp,1,8)) ~= nil and tonumber(string.sub(temp,1,8)) == 0 then
			else
				uuid = temp
			end
		end
	end
	MainProber:Step(MainProber.STEP_CHAT, MainProber.user_id, MainProber.server_id, 
		MainProber.role_name, 
		MainProber.role_id, 
		MainProber.role_level, 
		RoleData.Instance:GetCurrency(CurrencyType.Gold), 
		channel_id or 0, 
		sub_content or "", 
		param or 0, 
		RechargeData.Instance:GetRechargeNum(), 
		MainProber.role_authority,
		uuid)
	ChatSubCustom:AgentChatSub(channel_id, param, content, toName)
end