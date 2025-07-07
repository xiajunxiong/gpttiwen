
ChatInfo = ChatInfo or BaseClass()

function ChatInfo:__init()
	self.info_handles = {}
	self.rec_data = {
		key = "",
		time = 0,
	}
end

function ChatInfo:ChannelInfo(channel_info)
	self.channel_info = channel_info
	return self
end

function ChatInfo:ChatInfo(info)
	self.info = info
	self.info.msg_timestamp = self.info.msg_timestamp or TimeHelper.GetServerTime()
	self.info.sort_index = ChatData.Instance:ChatInfoSortIndex(self.info.msg_timestamp)
	self.height_count = 0

	self.show_channel_name = false
	self.heights = {}

	self.block_role = info.block_role

	self:Content()
	self:MainViewChat()

	self.is_item = ("table" == type(self.content)) or self:IsRecord()
	self.is_pet = self.is_item and RichTextType.Pet == self.content.type
	
	self.is_sheng_qi = self.is_item and RichTextType.ShengQi == self.content.type
	info.face_chatframe = info.face_chatframe or -1
	info.face_signature = info.face_signature or -1
	self.bubble_id = 0
	self.tail_id = 0
	if info.face_chatframe > -1 then
		local co = Config.face_cfg_auto.resource[info.face_chatframe]
		if co and 0 == co.res_type then
			self.bubble_id = co.res_id
		end
	end
	if info.face_signature > -1 then
		local co = Config.face_cfg_auto.resource[info.face_signature]
		if co and 1 == co.res_type then
			self.tail_id = co.res_id
		end
	end
	self.is_guild_task = info.is_guild_task or false
	return self
end

function ChatInfo:EmptyInfo()
	self.info = nil
	self.channel_info = nil

	for _, handle in pairs(self.info_handles) do
		RichTextHelper.DeleteInfo(handle)
	end
	self.info_handles = {}
	self.rec_data.key = ""
	self.rec_data.time = 0
	self.bubble_id = 0
	self.tail_id = 0
end

function ChatInfo:ChannelColorName()
	if nil ~= self.info and nil ~= self.info.msg_type then
		local cns = Language.Chat.ChannnelNameSpecial[self.info.msg_type]
		if ChatConfig.MsgType.trumpet == self.info.msg_type then
			cns = cns[self:ChannelId()] or cns[1]
		end
		if cns then return cns end
	end
	return self.channel_info and self.channel_info:ChannelColorName() or ""
end

function ChatInfo:ChannelColorNameShow()
	return self.channel_info and self.channel_info:ChannelColorNameShow() or ""
end

function ChatInfo:MsgTimestamp()
	return self.info.msg_timestamp
end

function ChatInfo:SortIndex()
	return self.info.sort_index or 1
end

function ChatInfo:ChannelId()
	return self.channel_info:ChannelId()
end

function ChatInfo:SelChannelId()
	return self.channel_info:SelChannelId()
end

function ChatInfo:IsSystem()
	return (self.channel_info and self.channel_info:IsSystem()) or self.block_role
end

function ChatInfo:BlockRoleAvatar()
	return self:IsHongBaoGet() or self:IsSystem()
end

function ChatInfo:IsSelf()
	return self.info.from_uid == RoleData.Instance:GetBaseData().role_id
end

function ChatInfo:IsRecord()
	return self.info ~= nil and self.info.content_type == ChatConfig.ChatContentType.audio or false
end

function ChatInfo:RecordKey()
	return self.rec_data.key
end

function ChatInfo:RecordTime()
	return self.rec_data.time
end

function ChatInfo:IsGuildTask()
	return self.is_guild_task
end

function ChatInfo:MainViewChat()
	if nil == self.channel_info or self.info.hide_main then return end
	if nil ~= self.channel_info and self:IsSystem() then
		self.main_view_chat = RichTextFormat.FormatChatMainViewSystem(self.info, self.channel_info, self)
	elseif self.from_uid ~= 0 then
		local content = ""
		if not self:IsRecord() then
			content = RichTextHelper.ParseAllFunc(RichTextHelper.ColorStr(self.info.content, COLORSTR.Yellow8), true, function (param_t)
				return RichTextFormat.FormatChatMainViewSpecial(self, param_t)
			end)
		end
		self.main_view_chat = RichTextFormat.FormatChatMainViewNormal(self.info, self.channel_info, content)
	end
end

function ChatInfo:Content()
	if nil ~= self.channel_info and self:IsSystem() or ChatConfig.MsgType.bullet == self.info.msg_type then
		self.content, self.show_channel_name = RichTextFormat.FormatChatContentSystem(self.info, self.channel_info, self)
	elseif self:IsRecord() then
		local sp_ret = Split(self.info.content,";")
		self.rec_data.key = sp_ret[1]
		self.rec_data.time = tonumber(sp_ret[2])
		self.content = ""
	elseif nil ~= self.info.content then
		self.content = RichTextHelper.ParseAllFunc(self.info.is_society and self.info.content or 
			RichTextHelper.ColorStr(self.info.content, COLORSTR.Yellow8), true, function (param_t)
			return RichTextFormat.FormatChatContentSpecial(self, param_t)
		end)
	end
end

function ChatInfo:GetMainViewChat()
	if self.tail_id > 0 then
		local co = AppearanceData.Instance.tail_layout_list[self.tail_id]
		if co then
			return self.main_view_chat .. RichTextHelper.SpriteStrAtlas(string.format(ChatConfig.TailTag, self.tail_id), co.w, co.h, {rate = 0.8})
		end
	end
	return self.main_view_chat
end

function ChatInfo:GetContent()
	if not self:IsPet() and not self:IsShengQi() and not self:IsSocietyGift() and not self:IsSocietyGiftFlower() and not self:IsSocietyShiTuInvite() and not self:IsHongbao() and not self:IsHongBaoGet() and self.tail_id > 0 then
		local co = AppearanceData.Instance.tail_layout_list[self.tail_id]
		if co then
			return self.content .. RichTextHelper.ColorStr("tail", COLORSTR.Transparent) .. RichTextHelper.SpriteStrAtlas(string.format(ChatConfig.TailTag, self.tail_id), co.w, co.h, {size = 22})
		end
	end
	return self.content
end

function ChatInfo:HideMain()
	return self.info and self.info.hide_main or false
end

function ChatInfo:GetShowChannelName()
	return self.show_channel_name or false
end

function ChatInfo:ItemList()
	return self.info.item_list
end

function ChatInfo:IsItem()
	return self.is_item
end

function ChatInfo:IsPet()
	return self.is_pet
end

function ChatInfo:IsShengQi()
	return self.is_sheng_qi
end

function ChatInfo:IsHongbao()
	return self.content.type == RichTextType.HongBao
end

function ChatInfo:IsSocietyGift()
	return self.content.type == RichTextType.SocietyGift
end

function ChatInfo:IsSocietyGiftFlower()
	return self.content.type == RichTextType.SocietyGiftFlower
end

function ChatInfo:IsSocietyShiTuInvite()
	return self.content.type == RichTextType.SocietyShiTuInvite
end

--抢红包红包信息
function ChatInfo:IsHongBaoGet()
	return self.content.type == RichTextType.HongBaoGet
end

function ChatInfo:BubbleId()
	return self.bubble_id
end

function ChatInfo:TailId()
	return self.tail_id
end

function ChatInfo:SelMsgTypes(msg_types)
	return msg_types[ChatConfig.MsgType.normal] or msg_types[self.info.msg_type]
end

function ChatInfo:IsMsgType(msg_type)
	return self.info.msg_type == msg_type
end

function ChatInfo:InGeneral()
	return nil ~= self.info.from_uid or ChatConfig.MsgType.bullet == self.info.msg_type or self:IsHongBaoGet()
end

function ChatInfo:HeightChatView(rich_text)
	if nil == self.heights.chat_view then
		if self.is_pet or self.is_sheng_qi then
			self.heights.chat_view = ChatConfig.ItemHeights.Pet + ChatConfig.ItemHeights.AvatarBase
		elseif self:IsHongbao() then
			self.heights.chat_view = ChatConfig.ItemHeights.HongBao + ChatConfig.ItemHeights.AvatarBase		--红包预制体高度
		-- elseif self.is_guild_task then
		-- 	self.heights.chat_view = 116
		elseif self:IsHongBaoGet() then
			self.heights.chat_view = ChatConfig.ItemHeights.HongBaoGet
		else
			local is_system = self:IsSystem()
			self.heights.chat_view = (RichTextMgr.CalcPreferredHeight(rich_text, self:GetContent(), is_system and 390 or (self.bubble_id > 0 and 290 or 310)) + (is_system and 15 or 70))
		end

		-- self.heights.chat_view = self.is_pet and (161 + 35) or 
		-- 	(RichTextMgr.CalcPreferredHeight(rich_text, self:GetContent(), is_system and 390 or (self.bubble_id > 0 and 290 or 310)) + (is_system and 15 or 70))
		self.heights.chat_view = self.is_guild_task and 116 or self.heights.chat_view
		-- self.heights.chat_view = self:IsHongbao() and 120 or self.heights.chat_view
	end
	return self.heights.chat_view
end

function ChatInfo:HeightMainViewChat(rich_text)
	local main_view_chat = self.heights.main_view_chat
	if nil == self.heights.main_view_chat then
		main_view_chat = RichTextMgr.CalcPreferredHeight(rich_text, self:GetMainViewChat())
		self.heights.main_view_chat = main_view_chat
		self.height_count = self.height_count + 1
	end
	return main_view_chat
end