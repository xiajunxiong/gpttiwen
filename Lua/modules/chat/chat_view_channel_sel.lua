
ChatViewChannelSel = ChatViewChannelSel or DeclareView("ChatViewChannelSel", ChatConfig.ResPath .. "chat_channel_sel")
VIEW_DECLARE_MASK(ChatViewChannelSel,ViewMask.Block)

function ChatViewChannelSel:ChatViewChannelSel()
	self.data = ChatCtrl.Instance:Data()
	self.language = Language.Chat
end
 
function ChatViewChannelSel:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.ChatChannelSel, Vector2.New(525, 297))

	local chat_channel_list = self.data:GetChatChanneSelList()
	for i = 1, self.ChannelItems:Length() do
		self.ChannelItems[i]:SetData(chat_channel_list[i])
	end

	self.ChannelRecruitItem:SetData({msg_type = ChatConfig.MsgType.recruit, color = self.data:GetChannelColorByChannelId(ChatConfig.ChannelId.world), is_sel = self.data:ChatMsgTypeIsSel(ChatConfig.MsgType.recruit)})
	self.ChannelBulletItem:SetData({msg_type = ChatConfig.MsgType.bullet, color = self.data:GetChannelColorByChannelId(ChatConfig.ChannelId.world), is_sel = self.data:ChatMsgTypeIsSel(ChatConfig.MsgType.bullet)})
end

ChatViewChannelSelItem = DeclareMono("ChatViewChannelSelItem", UIWidgetBaseItem)

function ChatViewChannelSelItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data.info
	self.Name.text = "<color=#" .. data:ChannelColor() .. ">".. string.format(Language.Chat.KH, info.channel_name) .. "</color>"
	self.BtnSel.isOn = data:IsSel()
end

function ChatViewChannelSelItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function ChatViewChannelSelItem:OnClickSel()
	self.data:SetIsSel(self.BtnSel.isOn)
end


ChatViewChannelSelMsgTypeItem = DeclareMono("ChatViewChannelSelMsgTypeItem", ChatViewChannelSelItem)

function ChatViewChannelSelMsgTypeItem:ChatViewChannelSelMsgTypeItem()
	self.chat_data = ChatCtrl.Instance:Data()
end

function ChatViewChannelSelMsgTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Name.text = string.format(Language.Chat.ChatMsgTypeSel[data.msg_type], data.color)
	self.BtnSel.isOn = data.is_sel
end

function ChatViewChannelSelMsgTypeItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function ChatViewChannelSelMsgTypeItem:OnClickSel()
	self.chat_data:ChatMsgTypeSetIsSel(self.data.msg_type, self.BtnSel.isOn)
	self.chat_data:ChannelNeedFlushByChannelId(ChatConfig.ChannelId.world)
end