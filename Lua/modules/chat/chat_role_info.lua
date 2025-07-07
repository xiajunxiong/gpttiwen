
ChatRoleInfo = ChatRoleInfo or BaseClass()

function ChatRoleInfo:__init(param_t)
    self.chat_data = param_t.chat_data
    self:ChatRoleInfo()
end

function ChatRoleInfo:ChatRoleInfo()
    self.chat_list = {}
    self.max_num = ChatConfig.CHAT_ROLE_NUM_MAX
end

function ChatRoleInfo:ChatRoleVo(uid)
    self.uid = uid
    return self
end

function ChatRoleInfo:AddChatInfo(info)
    if #self.chat_list == self.max_num then
		self.chat_data:ChatInfoRecycle(table.remove(self.chat_list, 1))
    end
    table.insert(self.chat_list, self.chat_data:ChatInfoGet():ChatInfo(info))
    self:FriendRecentChat()
end

function ChatRoleInfo:FriendRecentChat()
    local friend = SocietyData.Instance:GetFriendByUid(self.uid)
    if friend then
        friend:RecentChat()
    end
end

function ChatRoleInfo:AddChatInfoLocal(content, chat_items,content_type, msg_timestamp)
	local info = {
		content = content,
		msg_timestamp = msg_timestamp or TimeHelper.GetServerTime(),
        from_uid = RoleData.Instance:GetRoleId(),
        is_society = true,
        item_list = chat_items,
        content_type = content_type or ChatConfig.ChatContentType.text,
        face_chatframe = AppearanceData.Instance:GetEquipResSeq(AppearanceConfig.Type.Bubble),
        face_signature = AppearanceData.Instance:GetEquipResSeq(AppearanceConfig.Type.TailTag),
	}
	self:AddChatInfo(info)
end

function ChatRoleInfo:GetChatList()
	return self.chat_list
end

function ChatRoleInfo:EmptyInfo()
    for _, chat_info in pairs(self.chat_list) do
        self.chat_data:ChatInfoRecycle(chat_info)
    end
    self.chat_list = {}
end

function ChatRoleInfo:ClearChatInfoByUid(uid)
    if 0 == #self.chat_list then return end
    for i = #self.chat_list, 1, -1 do
        local chat_info = self.chat_list[i]
        local info = chat_info.info
        if nil ~= info and uid == info.from_uid then
            table.remove(self.chat_list, i)
        end
    end
end