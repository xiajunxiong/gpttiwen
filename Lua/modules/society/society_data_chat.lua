
function SocietyData:ChatData()
    self.chat_data = ChatCtrl.Instance:Data()
end

function SocietyData:AddChatInfo(content, chat_items, content_type)
    if nil == self.cur_sel_role then return end
    local info = self.cur_sel_role.info
    if not ChatConfig.IS_BLOCK then
        self.chat_data:AddChatInfoByUid(info.uid, content, chat_items, content_type)
    end
    ChatCtrl.Instance:SendSingleChat(content, chat_items, content_type)
end

function SocietyData:AddChatInfoLocal(content, chat_items, content_type, msg_timestamp)
    if nil == self.cur_sel_role then return end
    local info = self.cur_sel_role.info
    if not ChatConfig.IS_BLOCK then
        self.chat_data:AddChatInfoByUid(info.uid, content, chat_items, content_type, msg_timestamp)
    end
end

function SocietyData:GetChatListByUid(uid, check_group_red_point)
    if nil == uid and nil ~= self.cur_sel_role and nil ~= self.cur_sel_role.info then
        uid = self.cur_sel_role.info.uid
    end
    self:SetFriendChatRedPoint(uid, false, check_group_red_point)
    return self.chat_data:GetRoleChatListByUid(uid)
end

function SocietyData:CheckChatIllegal(content, uid)
    local illegal
    local tip_t = {}
    local info = (nil ~= uid and self.friend_list[uid]) and self.friend_list[uid].info or self.cur_sel_role.info
    illegal = illegal or self:CheckChatRoleInBlackList(info, tip_t)
    illegal = illegal or self:CheckChatRoleOnline(info, tip_t)
    illegal = illegal or self:CheckChatContentEmpty(content, tip_t)
    return illegal, tip_t.t
end

function SocietyData:CheckChatRoleInBlackList(info, tip_t)
    if SocietyConfig.FriendGroupType.black_list == info.group_type then
        tip_t.t = Language.Society.ChatIllegalTips.in_black_list
        return true
    end
end

function SocietyData:CheckChatRoleOnline(info, tip_t)
    if 0 == info.is_online then
        tip_t.t = Language.Society.ChatIllegalTips.not_online
        return true
    end
end

function SocietyData:CheckChatContentEmpty(content, tip_t)
    if StringIsEmpty(content) then
        tip_t.t = Language.Society.ChatIllegalTips.empty
        return true
    end
end

function SocietyData:AddGroupChatInfo(info)
    local group_key = SocietyData.GroupId2Key(info.group_id)
    local group = self.chat_group_channels[group_key]
    if group then
        group:AddChatInfo(info)
        SocietyData.Instance:SetFriendChatGroupRedPoint(group_key, math.abs(TimeHelper.GetServerTime() - info.msg_timestamp) < 3)
    end
end

function SocietyData:GetGroupChannelChatListByGroupId(group_id, group_key)
    local list = {}
    local group_key = group_key or SocietyData.GroupId2Key(group_id)
    local channel = self.chat_group_channels[group_key]
    local chat_list = channel:GetChatList()
    self:SetFriendChatGroupRedPoint(group_key, false)
    for _, chat_info in pairs(chat_list) do
        table.insert(list, chat_info)
    end
    table.sort(list, function (a, b)
        if not a or not b then return a end
        local at = a:MsgTimestamp()
        local bt = b:MsgTimestamp()
        return at < bt
    end)
    return list
end

function SocietyData:SetChatRecordList(protocol)
    local show_temp = self.quick_chat_info.show_list[0]
    self.quick_chat_info.show_list = protocol.show_list
    self.quick_chat_info.show_list[0] = show_temp
    self.quick_chat_info.show_id_list = protocol.show_id_list
    self:CheckQuickChatShowList(SocietyConfig.FriendInfoChangeType.remove)
end

function SocietyData:QuickChatAdd(type, id)
    local show_list = self.quick_chat_info.show_list
    if #show_list >= ChatConfig.RECORD_NUM_MAX then
        PublicPopupCtrl.Instance:Center(string.format(Language.Society.QuickChat.FullTips, ChatConfig.RECORD_NUM_MAX))
        return false
    end
    if SocietyConfig.QuickShowType.add ~= type and self:IsQuickChatInfoAddExit() then
        show_list[1] = {type = type, val = id}
    else
        table.insert(show_list, {type = type, val = id})
    end
    SocietyCtrl.Instance:SendChatRecordSet(show_list)
    if SocietyConfig.QuickShowType.add ~= type then
        self.quick_chat_info.cur_sel = id
        PublicPopupCtrl.Instance:Center(Language.Society.QuickChat.SuccTips)
    end
    return true
end

function SocietyData:QuickChatCurSel(val, selected)
    local value = false
    local index = self:GetChatQuickShowItemIndex(val)
    if selected then
		self.quick_chat_info.cur_sel = val
    elseif val == self.quick_chat_info.cur_sel then
        self.quick_chat_info.cur_sel = (self.quick_chat_set or index > 3) and val or ""
        self.quick_chat_set = false
	end
	return value
end

function SocietyData:QuickChatCurSelS(selected)
    local value = false
    local index = self:GetChatQuickShowItemIndex(self.quick_chat_info.cur_sel)
    if selected then
        if index <= 3 then
            self.quick_chat_info.cur_sel = self.quick_chat_info.show_list[4].val
        end
    elseif index > 3 then
		self.quick_chat_info.cur_sel = ""
	end
	return value
end

function SocietyData:GetChatQuickShowItemIndex(val)
    local index = -1
    for i, v in pairs(self.quick_chat_info.show_list) do
        if v.val == val then
            index = i
            break
        end
    end
	return index
end


function SocietyData:FlushChatQuickRecords()
    self.quick_chat_info.flush_records = not self.quick_chat_info.flush_records
end

function SocietyData:FlushChatQuickItems()
    self.quick_chat_info.flush_items = not self.quick_chat_info.flush_items
end

function SocietyData:CheckValInQuickChatShowList(val)
    local show_id_list = self.quick_chat_info.show_id_list
    local friends, groups = show_id_list.friends, show_id_list.groups
    return friends[val] or groups[val] 
end

function SocietyData:CheckQuickChatShowList(change_type, value)
    local show_id_list = self.quick_chat_info.show_id_list
    local friends, groups = show_id_list.friends, show_id_list.groups
    -- 正在主界面换位操作时不作更改
    if SocietyConfig.ChatQuickFlyState.flying == self.quick_chat_fly.state then return end
    if SocietyConfig.FriendInfoChangeType.remove == change_type then
        for uid, _ in pairs(friends) do
            if not self:IsFriend(uid) then
                self:DeleteQuickChatShowVal(uid)
            end
        end
        for group_key, _ in pairs(groups) do
            if nil == self.chat_group_list[group_key] then
                self:DeleteQuickChatShowVal(group_key)
            end
        end
    elseif SocietyConfig.FriendInfoChangeType.update == change_type then
        if friends[value] then
            self:FlushChatQuickItems()
        end
    end
end

function SocietyData:DeleteQuickChatShowVal(val)
    local show_list = self.quick_chat_info.show_list
    for k, v in ipairs(show_list) do
        if v.val == val then
            if self.quick_chat_info.cur_sel == val then
                self.quick_chat_info.cur_sel = ""
            end
            table.remove(show_list, k)
            SocietyCtrl.Instance:SendChatRecordSet(show_list)
            -- 删除最后一个就自动退出编辑状态
            if 0 == #show_list then self.quick_chat_info.is_editing = false end
            break
        end
    end
end

function SocietyData:ChangeQuickChatShowVal(pos, data)
    local show_list = self.quick_chat_info.show_list
    for k, v in ipairs(show_list) do
        if v.val == data.val then
            table.remove(show_list, k)
            table.insert(show_list, pos < k + 1 and pos or pos - 1, data)
            SocietyCtrl.Instance:SendChatRecordSet(show_list)
            break
        end
    end
end

function SocietyData:IsQuickChatInfoAddExit()
    local show_list = self.quick_chat_info.show_list
    return show_list[1] and SocietyConfig.QuickShowType.add == show_list[1].type
end