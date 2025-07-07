
ChatChannelInfo = ChatChannelInfo or BaseClass()

function ChatChannelInfo:__init(param_t)
    self.info = param_t.info
    self.chat_data = param_t.chat_data
    self:ChatChannelInfo()
    self.need_flush = false
end

function ChatChannelInfo:ChatChannelInfo()
    self.chat_list = {}
    self.max_num = ChatConfig.ChannelId.group == self.info.channel_id and ChatConfig.CHAT_ROLE_NUM_MAX or self.info.max_record
    self.channel_type = ChatConfig.ChannelIdToType[self.info.channel_id]
    if self.channel_type then
        self.channel_co = Cfg.ChatChannelInfoByChannelId(self.channel_type, false)
    end
    self.interval_time = self.channel_co and self.channel_co.interval_time or 0
    self.remind_time = {val = 0}
    
    self.head_index = 0
    self.cur_index = 0

    self.channel_color = COLORSTR[self.info.color] or COLORSTR.White
    -- self.channel_color_name = RichTextHelper.ColorStr("【" .. self.info.channel_name .."】", self.channel_color)
    self.channel_color_name = RichTextHelper.ColorStr(string.format(Language.Chat.KH, self.info.prefix), self.channel_color)
    self.channel_color_name_show = RichTextHelper.ColorStr(string.format(Language.Chat.KH, self.info.channel_name), self.channel_color)
end

function ChatChannelInfo:ChannelId()
    return self.info.channel_id
end

function ChatChannelInfo:ChannelType()
    return self.channel_type
end

function ChatChannelInfo:ChannelName()
    return self.info.prefix
end

function ChatChannelInfo:ChannelNameShow()
    return self.info.channel_name
end

function ChatChannelInfo:ChannelColor()
    return self.channel_color
end

function ChatChannelInfo:ChannelColorName()
    return self.channel_color_name
end

function ChatChannelInfo:ChannelColorNameShow()
    return self.channel_color_name_show
end

function ChatChannelInfo:IsSystem()
    return ChatConfig.ChannelId.system == self.info.channel_id
end

function ChatChannelInfo:IsLandsBetween()
    return ChatConfig.ChannelId.landsbetween == self.info.channel_id
end

function ChatChannelInfo:IsSel()
    if nil == self.is_sel then
        self.is_sel = UnityPlayerPrefs.GetInt(PrefKeys.ChatChannelIsSel(RoleData.Instance:GetBaseData().role_id ,self.info.channel_id)) == 1
    end
    return self.is_sel
end

function ChatChannelInfo:SetIsSel(value)
    self.is_sel = value
    UnityPlayerPrefs.SetInt(PrefKeys.ChatChannelIsSel(RoleData.Instance:GetBaseData().role_id,self.info.channel_id), self.is_sel and 1 or 0)
end

function ChatChannelInfo:AddChatInfo(info)
    if nil == info then
        LogError("AddChatInfo error info=nil")
        return
    end
    if #self.chat_list == self.max_num then
		self.chat_data:ChatInfoRecycle(table.remove(self.chat_list, 1))
    end
    table.insert(self.chat_list, self.chat_data:ChatInfoGet():ChannelInfo(self):ChatInfo(info))
    self.need_flush = not self.need_flush
end

-- function ChatChannelInfo:AddChatInfo(info)
--     if nil == info then
--         LogError("AddChatInfo error info=nil")
--         return
--     end
--     if self.cur_index - self.head_index == self.max_num then
--         self.head_index = self.head_index + 1
--         self.chat_data:ChatInfoRecycle(self.chat_list[self.head_index])
--         self.chat_list[self.head_index] = nil
--     end
--     self.cur_index = self.cur_index + 1
--     self.chat_list[self.cur_index] = self.chat_data:ChatInfoGet():ChannelInfo(self):ChatInfo(info)
-- end

function ChatChannelInfo:GetChatList()
	return self.chat_list
end

function ChatChannelInfo:EmptyChatList()
    for _, info in pairs(self.chat_list) do
        self.chat_data:ChatInfoRecycle(info)
    end
    self.chat_list = {}
    self.head_index = 0
    self.cur_index = 0
    self.is_sel = nil
end

function ChatChannelInfo:RemindTime()
    return self.remind_time.val
end

function ChatChannelInfo:SendGrey()
    local interval_time = self.interval_time
    -- 世界答题世界频道cd减半
    if ChatConfig.ChannelId.world == self:ChannelId() and ChatData.Instance.world_question_info.answer_tamp > 0 then interval_time = interval_time / 2 end
    TimeHelper:CancelTimer(self.timer_tt_send)
    self.remind_time.val = interval_time + 1
    self.timer_tt_send = TimeHelper:AddCountDownTT(function ()
        self.remind_time.val = self.remind_time.val - 1
    end, function ()
        self.remind_time.val = 0
    end, interval_time, 1)
end

function ChatChannelInfo:LimitNum()
    return self.info.limit_num
end

function ChatChannelInfo:IsShow()
    if self:IsLandsBetween() then 
        return LandsBetweenData.Instance:GetCanAccess()
    end 
    return 1 == self.info.is_show
end

function ChatChannelInfo:SortValue()
    return self.info.sort
end

function ChatChannelInfo:NeedFlush()
    self.need_flush = not self.need_flush
end

function ChatChannelInfo:SelChannelId()
    return ChatConfig.ChannelId.world == self:ChannelId() and ChatConfig.ChannelId.general or self:ChannelId()
end

function ChatChannelInfo:ClearChatInfoByUid(uid)
    if 0 == #self.chat_list then return end
    for i = #self.chat_list, 1, -1 do
        local chat_info = self.chat_list[i]
        local info = chat_info.info
        if nil ~= info and uid == info.from_uid then
            table.remove(self.chat_list, i)
        end
    end
    self.need_flush = not self.need_flush
end