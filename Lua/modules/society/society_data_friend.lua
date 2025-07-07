
function SocietyData:FriendConfig()
end

function SocietyData:FriendData()
    self:FriendGroupList()
    self:FriendList()
    self:FriendAddList()
    self:FriendApplyList()
    self:FriendEditorNewGroup()
end

function SocietyData:FriendGroupList()
    self.friend_group_list = {}
    for i = 1, SocietyConfig.FRIEND_GROUP_TYPE_MAX do
        self.friend_group_list[i] = SocietyFriendGroupInfo.New(i)
    end
end

function SocietyData:FriendList()
    self.friend_list = {}
end

function SocietyData:FriendAddList()
    self.friend_add_list = {}
end

function SocietyData:FriendApplyList()
    self.friend_apply_list = {}
end

function SocietyData:FriendEditorNewGroup()
    self.friend_editor_new_group = {}
end

function SocietyData:SetFriendInfo(protocol)
    for i = 1, SocietyConfig.FRIEND_GROUP_CUSTOM_MAX do
		self.friend_group_list[SocietyConfig.FriendGroupType["group_custom"..i]]:GroupName(protocol.group_names[i])
    end
    for i = 1, protocol.count do
		self:FriendListChange{friend_info = protocol.friend_list[i], change_type = SocietyConfig.FriendInfoChangeType.update, group_type1 = SocietyConfig.FriendGroupToC[protocol.friend_list[i].group_type]}
	end
end

function SocietyData:SetFriendListChange(protocol)
    self:FriendListChange{friend_info = protocol.friend_info, change_type = protocol.change_type, group_type1 = SocietyConfig.FriendGroupToC[protocol.friend_info.group_type]}
    self:CheckQuickChatShowList(protocol.change_type, protocol.friend_info.uid)
end

-- function SocietyData:FriendListChange(friend_info, change_type, group_type, user_id)
--     local uid = (nil ~= friend_info) and friend_info.uid or user_id
--     if SocietyConfig.FriendInfoChangeType.update == change_type then
--         if nil == self.friend_list[uid] then
--             self.friend_list[uid] = self:RoleInfoGet()
--             friend_info.group_type = bit:_lshift(1, group_type)
--         else
--             local info_type = self.friend_list[uid].info.group_type
--             friend_info.group_type = (1 == bit:_rshift(info_type, group_type) % 2) and info_type or (info_type + bit:_lshift(1, group_type))
--         end
--         self.friend_list[uid]:RoleInfo(friend_info)
--     elseif SocietyConfig.FriendInfoChangeType.remove == change_type and nil ~= self.friend_list[uid] then
--         local info = self.friend_list[uid].info
--         local info_type = info.group_type
--         info.group_type = info_type - bit:_lshift(1, group_type)
--         if friend_info.group_type == 0 then
--             self:RoleInfoRecycle(self.friend_list[uid])
--             self.friend_list[uid] = nil
--         else
--             self.friend_list[uid]:RoleInfo(info)
--         end
--     end
-- end

function SocietyData:FriendListChange(param_t)
    local friend_info = param_t.friend_info
    local uid = (nil ~= friend_info) and friend_info.uid or param_t.user_id
    if SocietyConfig.FriendInfoChangeType.update == param_t.change_type then
        if nil == self.friend_list[uid] then
            self.friend_list[uid] = self:RoleInfoGet()
            friend_info.group_type1 = param_t.group_type1
            friend_info.group_type2 = param_t.group_type2
        else
            local info = self.friend_list[uid].info
            friend_info.group_type1 = param_t.group_type1 or info.group_type1
            friend_info.group_type2 = param_t.group_type2 or info.group_type2
            if nil == friend_info.love and nil ~= info.love then friend_info.love = info.love end
            if SocietyConfig.FriendGroupType.black_list == friend_info.group_type1 then friend_info.group_type2 = nil end
        end
        self.friend_list[uid]:RoleInfo(friend_info)
    elseif SocietyConfig.FriendInfoChangeType.remove == param_t.change_type and nil ~= self.friend_list[uid] then
        local info = self.friend_list[uid].info
        info.group_type1 = (nil == param_t.group_type1) and info.group_type1 or nil
        info.group_type2 = (nil == param_t.group_type2) and info.group_type2 or nil
        if nil == info.group_type1 and nil == info.group_type2 then
            self:RoleInfoRecycle(self.friend_list[uid])
            self.friend_list[uid] = nil
        else
            self.friend_list[uid]:RoleInfo(info)
        end
    end
end

function SocietyData:GetFriendGroupList()
    return self.friend_group_list
end

function SocietyData:GetFriendGroupInfoByType(type)
    return self.friend_group_list[type]
end

function SocietyData:GetFriendListByType(type, is_sort_online, is_remove_not_online)
    local list = {}
    for _, v in pairs(self.friend_list) do
        if v.info.group_type1 == type or v.info.group_type2 == type then
            -- 是否去除掉离线好友
            if is_remove_not_online then
                if v.info.is_online > 0 then
                    table.insert(list, v)
                end
            else
                table.insert(list, v)
            end
        end
    end
    if is_sort_online then
        table.sort(list, function (a, b)
            local ai = a.info
            local bi = b.info
            if ai.is_online == bi.is_online then
                if 0 == ai.is_online then
                    return ai.last_online_timestamp > bi.last_online_timestamp
                else
                    local ats = a.chat_ts or 0
                    local bts = b.chat_ts or 0
                    return ats > bts
                end
            end
            return ai.is_online > bi.is_online
        end)
    end
    return list
end

function SocietyData:GetFriendListFriendTeamInvite()
    local list = {}
    local config = SocietyConfig.FriendGroupType
    local limit_level = TeamData.Instance:InTeam() and TeamData.Instance:GetTeamInfo():LimitLevelLow() or TeamData.Instance:LimitLevelLow()
    for _, v in pairs(self.friend_list) do
        local group_type1 = v.info.group_type1
        local is_online = v.info.is_online
        if is_online >= 1 and nil ~= group_type1 and config.black_list ~= group_type1 then
            if v.info.level >= limit_level then
                table.insert(list, v)
            end
        end
    end
    return list
end

function SocietyData:GetFriendListFriendWeddingInvite()
    local list = {}
    local config = SocietyConfig.FriendGroupType
    local invite_list = MarryData.Instance:GetWeddingInviteList()
    local invite_info = MarryData.Instance:GetWeddingInviteInfo()
    local limit_level = MarryData.Instance:ConfigOtherPartakeLevelLimit()
    for uid, v in pairs(self.friend_list) do
        local group_type1 = v.info.group_type1
        if not invite_list[uid] and nil ~= group_type1 and config.black_list ~= group_type1 then
            if not invite_info.hosts[uid] and v.info.level >= limit_level then
                table.insert(list, v)
            end
        end
    end
    return list
end

function SocietyData:GetFriendGroupNumShow()
    local num = 0
    local config = SocietyConfig.FriendGroupType
    for _, v in pairs(self.friend_list) do
        local group_type1 = v.info.group_type1
        if nil ~= group_type1 and config.black_list ~= group_type1 then
            num = num + 1
        end
    end
    return num
end

function SocietyData:IsFriend(uid)
    return self.friend_list[uid] and nil ~= self.friend_list[uid].info.group_type1 and SocietyConfig.FriendGroupType.black_list ~= self.friend_list[uid].info.group_type1
end

function SocietyData:ResetFriendEditorNewGroup()
    self.friend_editor_new_group.uid_list = {}
    self.friend_editor_new_group.num = 0
end

function SocietyData:GetFriendEditorNewGroup()
    return self.friend_editor_new_group
end

function SocietyData:GetFriendEditorNewGroupUidList()
    return self.friend_editor_new_group.uid_list
end

function SocietyData:SetFriendEditorNewGroupInfo(uid, value)
    self.friend_editor_new_group.uid_list[uid] = value and uid or nil
    local num = self.friend_editor_new_group.num
    local num = 0
    for k, v in pairs(self.friend_editor_new_group.uid_list) do
        num = num + 1
    end
    self.friend_editor_new_group.num = num
end

function SocietyData:SetAddFriendRoute(protocol)
    local uid = protocol.friend_info.uid
    if nil == self.friend_apply_list[uid] then
        self.friend_apply_list[uid] = self:RoleInfoGet()
    end
    self.friend_apply_list[uid]:RoleInfo(protocol.friend_info)
    self:NewFriendRedPoint(true)
end

function SocietyData:SetChangeBlack(protocol)
    self:FriendListChange{friend_info = protocol.black_info, change_type = protocol.change_type, group_type1 = SocietyConfig.FriendGroupType.black_list}
end

function SocietyData:SetBlackInfo(protocol)
    for i = 1, protocol.count do
		self:FriendListChange{friend_info = protocol.black_list[i], change_type = SocietyConfig.FriendInfoChangeType.update, group_type1 = SocietyConfig.FriendGroupType.black_list}
	end
end

function SocietyData:FriendApplyRemoveByUid(uid)
    self:RoleInfoRecycle(self.friend_apply_list[uid])
    self.friend_apply_list[uid] = nil
    self:NewFriendRedPoint()
end

function SocietyData:GetFriendApplyList()
    return self.friend_apply_list
end

function SocietyData:SetFriendGroupInfo(protocol)
    for i = 1, SocietyConfig.FRIEND_GROUP_CUSTOM_MAX do
        local type = SocietyConfig.FriendGroupType["group_custom"..i]
        local group = self.friend_group_list[type]
        if false == group.not_create and StringIsEmpty(protocol.group_names[i]) then
            for uid, v in pairs(self.friend_list) do
                if type == v.info.group_type1 then
                    self.friend_list[uid].info.group_type1 = SocietyConfig.FriendGroupType.friend
                end
            end
        end
        group:GroupName(protocol.group_names[i])
    end
end

function SocietyData:SetRoleGroupInfo(protocol)
    local chat_group_list = {}
    local chat_group_channels = {}
    for i = 1, protocol.count do
        local info = protocol.groups[i]
        local group_key = SocietyData.GroupId2Key(info.group_id)

        chat_group_list[group_key] = self.chat_group_list[group_key] and self.chat_group_list[group_key]:GroupInfo(info) or SocietyChatGroupInfo.New(info, group_key)
        chat_group_channels[group_key] = self.chat_group_channels[group_key] or SmartData.New(ChatChannelInfo.New{info = ChatData.Instance:ChatGroupChannelInfo(), chat_data = ChatData.Instance})
    end
    if nil == self.cur_sel_chat_group or nil == chat_group_list[self.cur_sel_chat_group:GroupKey()] then
        ViewMgr:CloseView(SocietyGroupInfoView)
        ViewMgr:CloseView(SocietyGroupInviteView)
    end
    if self.chat_group_temp then
        local group_key = self.chat_group_temp:GroupKey()
        chat_group_channels[group_key] = self.chat_group_channels[group_key]
    end
    self.chat_group_list = chat_group_list
    self.chat_group_channels:Set(chat_group_channels)
    if 0 == protocol.reason then
        self:CheckQuickChatShowList(SocietyConfig.FriendInfoChangeType.remove)
        if SocietyConfig.ChatQuickFlyState.flying ~= self.quick_chat_fly.state then
            self:FlushChatQuickItems()
        end
    end
end

function SocietyData:GetChatGroupCount()
    local not_cross, is_cross = 0, 0
    for _, group in pairs(self.chat_group_list) do
        if group:IsCross() then
            is_cross = is_cross + 1
        else
            not_cross = not_cross + 1
        end
    end
    return not_cross, is_cross
end

function SocietyData:GetChatGroupCountAll()
    local not_cross, is_cross = self:GetChatGroupCount()
    return not_cross + is_cross
end

function SocietyData:GetChatGroupList()
    local chat_group_list = {}
    for _, info in pairs(self.chat_group_list) do
        table.insert(chat_group_list, info)
    end
    if nil == self.cur_sel_chat_group or nil == self.chat_group_list[self.cur_sel_chat_group:GroupKey()] then
        self.cur_sel_chat_group = chat_group_list[1]
    end
    return chat_group_list
end

function SocietyData:SetGroupAllInfo(protocol)
    local info = protocol.info
    local group_key = SocietyData.GroupId2Key(info.group_id)
    if nil ~= self.chat_group_list[group_key] then
        self.chat_group_list[group_key]:SetGroupNotice(info.group_notice)
    elseif nil ~= self.chat_group_temp then
    end
    self.chat_group_detail:Set(info)
end

function SocietyData:SetRoleOneGroupInfo(protocol)
    local chat_group_list = self.chat_group_list

    local info = protocol.info
    local group_key = SocietyData.GroupId2Key(info.group_id)
    if SocietyConfig.ChatGroupReason.add == protocol.reason then
        chat_group_list[group_key] = SocietyChatGroupInfo.New(info, group_key)
        self.chat_group_channels[group_key] = SmartData.New(ChatChannelInfo.New{info = ChatData.Instance:ChatGroupChannelInfo(), chat_data = ChatData.Instance})
    elseif SocietyConfig.ChatGroupReason.remove == protocol.reason then
        chat_group_list[group_key] = nil
        self.chat_group_channels[group_key] = nil
    elseif SocietyConfig.ChatGroupReason.change == protocol.reason then
        if chat_group_list[group_key] and info.master ~= chat_group_list[group_key]:Master() then
            if ViewMgr:IsOpen(SocietyGroupManageView) then
                ViewMgr:CloseView(SocietyGroupManageView)
            end
            if ViewMgr:IsOpen(SocietyGroupInfoView) then
                ViewMgr:CloseView(SocietyGroupInfoView)
            end
        end
        if self.chat_group_list[group_key] then
            chat_group_list[group_key] = self.chat_group_list[group_key]:GroupInfo(info)
        end
    end
  
    if nil == self.cur_sel_chat_group or nil == chat_group_list[self.cur_sel_chat_group:GroupKey()] then
        ViewMgr:CloseView(SocietyGroupInfoView)
        ViewMgr:CloseView(SocietyGroupInviteView)
    end
    self.chat_group_list = chat_group_list
    if SocietyConfig.ChatGroupReason.remove == protocol.reason then
        self:CheckQuickChatShowList(SocietyConfig.FriendInfoChangeType.remove)
        if SocietyConfig.ChatQuickFlyState.flying ~= self.quick_chat_fly.state then
            self:FlushChatQuickItems()
        end
    elseif SocietyConfig.ChatGroupReason.change == protocol.reason then
        if SocietyConfig.ChatQuickFlyState.flying ~= self.quick_chat_fly.state then
            self:FlushChatQuickItems()
        end
    end
end

function SocietyData:SetTempGroupInfo(protocol)
    local info = protocol.info
    local group_key = SocietyData.GroupId2Key(info.group_id)
    if nil ~= self.chat_group_temp then
        self:SetTempGroupLeaveNotice(true)
    end
    self.quick_chat_set = true
    self.chat_group_temp = SocietyChatGroupInfo.New(info, group_key)
    self.chat_group_channels[group_key] = SmartData.New(ChatChannelInfo.New{info = ChatData.Instance:ChatGroupChannelInfo(), chat_data = ChatData.Instance})
    self.quick_chat_info.show_list[0] = {type = SocietyConfig.QuickShowType.temp, val = SocietyConfig.QuickShowType.temp}
    self:FlushChatQuickItems()
end

function SocietyData:SetTempGroupLeaveNotice(value)
    if nil == self.chat_group_temp then return end
    local group_key = self.chat_group_temp:GroupKey()
    self.chat_group_temp = nil
    self.chat_group_channels[group_key] = nil
    self.quick_chat_info.show_list[0] = nil
    local cur_sel = self.quick_chat_info.cur_sel
    if not value and not ("string" == type(cur_sel)) and SocietyConfig.QuickShowType.temp == cur_sel then
        self.quick_chat_info.cur_sel = ""
    end

    if SocietyConfig.ChatQuickFlyState.flying ~= self.quick_chat_fly.state then
        self:FlushChatQuickItems()
    end
end

function SocietyData:SetChatGroupMsgNotice(protocol)
    local info = protocol.info
    local group_key = SocietyData.GroupId2Key(info.group_id)
    if nil ~= self.chat_group_list[group_key] then
        self.chat_group_list[group_key]:SetGroupNoticeT(info.msg_content)
    elseif nil ~= self.chat_group_temp and self.chat_group_temp:GroupKey() == group_key then
        self.chat_group_temp:SetGroupNoticeT(info.msg_content)
    end
end

function SocietyData:SetTempChatGroupBecomeOfficialNotice(protocol)
    if self.chat_group_temp then
        local group_key = SocietyData.GroupId2Key(protocol.info.group_id)
        if self:QuickChatAdd(SocietyConfig.QuickShowType.group, group_key) then
		end
    end
end

function SocietyData:SetRoleSystemSetChatGroupInfo(protocol)
    for i = 1, protocol.count do
        local info = protocol.groups[i]
        local group_key = SocietyData.GroupId2Key(info.group_id)
        if self.chat_group_list[group_key] then
            self.chat_group_list[group_key]:SetDisturbVal(info.set_value)
        end
    end
end

function SocietyData:SetRecentlyTeamInfo(protocol)
    for i = 1, protocol.count do
		self:FriendListChange{friend_info = protocol.friend_list[i], change_type = SocietyConfig.FriendInfoChangeType.update, group_type2 = SocietyConfig.FriendGroupType.recent_team}
	end
end

function SocietyData:SetOneRecentlyTeamInfo(protocol)
    self:FriendListChange{change_type = SocietyConfig.FriendInfoChangeType.remove, group_type2 = SocietyConfig.FriendGroupType.recent_team, uid = protocol.del_role_id}
    self:FriendListChange{friend_info = protocol.friend_info, change_type = SocietyConfig.FriendInfoChangeType.update, group_type2 = SocietyConfig.FriendGroupType.recent_team}
end

function SocietyData:SetRecentlyChatAdd(protocol)
    self:FriendListChange{change_type = SocietyConfig.FriendInfoChangeType.remove, group_type2 = SocietyConfig.FriendGroupType.recent_contacts, uid = protocol.del_role_id}
    self:FriendListChange{friend_info = protocol.friend_info, change_type = SocietyConfig.FriendInfoChangeType.update, group_type2 = SocietyConfig.FriendGroupType.recent_contacts}
    if self.recently_chat_role and protocol.friend_info.uid == self.recently_chat_role then
        self.recently_chat_role = self.friend_list[self.recently_chat_role]
        ViewMgr:OpenView(SocietyView)
    end
end

function SocietyData:RecentlyChatCheck(info)
    local uid = info.from_uid == RoleId() and info.to_uid or info.from_uid
    local friend = self.friend_list[uid]
    if friend and friend.info.group_type2 == SocietyConfig.FriendGroupType.recent_contacts then return end
    self:SetRecentlyChatAdd({
        del_role_id = 0,
        friend_info = {
            uid = uid,
            name = info.name,
            family = "",
            avatar_type = info.avatar_type,
            avatar_quality = info.avatar_quality,
            top_level = info.top_level,
            is_online = 1,
            vip_level = 0,
            level = info.level,
            prof = info.prof,
            avatar_timestamp = info.avatar_timestamp,
            last_online_timestamp = TimeHelper.GetServerTime(),
        }
    })
end

function SocietyData:SetFriendRecommendList(protocol)
    for _, item in pairs(self.friend_add_list) do
        self:RoleInfoRecycle(item)
    end
    self.friend_add_list = {}
    for _, info in pairs(protocol.friend_info_list) do
        local uid = info.uid
        if uid ~= 0 then
            self.friend_add_list[uid] = self:RoleInfoGet()
            self.friend_add_list[uid]:RoleInfo(info)
        end
    end
end

-- function SocietyData:SetFriendRecommendOne(protocol)
--     local friend_info = protocol.friend_info
--     friend_info.index = self.friend_add_index
--     self.friend_add_list[self.friend_add_index] = self:RoleInfoGet()
--     self.friend_add_list[self.friend_add_index]:RoleInfo(friend_info)
-- end


function SocietyData:SetFuzzySearchRolesInfo(protocol)
    for _, item in pairs(self.friend_add_list) do
        self:RoleInfoRecycle(item)
    end
    self.friend_add_list = {}
    for _, info in pairs(protocol.friend_info_list) do
        local uid = info.uid
        if uid ~= 0 then
            self.friend_add_list[uid] = self:RoleInfoGet()
            self.friend_add_list[uid]:RoleInfo(info)
        end
    end
end

function SocietyData:FriendAddRemoveByUid(uid)
    self:RoleInfoRecycle(self.friend_add_list[uid])
    self.friend_add_list[uid] = nil
end

function SocietyData:FriendAddListEmpty()
    for _, item in pairs(self.friend_add_list) do
        self:RoleInfoRecycle(item)
    end
    self.friend_add_list = {}
end

function SocietyData:GetFriendAddList()
    local list = {}
    local role_level = RoleData.Instance:GetRoleLevel()
    for _, info in pairs(self.friend_add_list) do
        table.insert(list, info)
    end
    table.sort(list, function (a, b)
		if not a or not b then return a end
        local ar = math.abs(a.info.level - role_level) <= 5 and 1 or 0
        local br = math.abs(b.info.level - role_level) <= 5 and 1 or 0
        return ar > br
    end)
    return list
end

function SocietyData:GetFriendGroupRoleList()
    local group_type = self.friend_cur_sel_group.group_type == SocietyConfig.FriendGroupType.group_custom1 and SocietyConfig.FriendGroupType.group_custom2 or SocietyConfig.FriendGroupType.group_custom1
    local list = {}
    local list1 = self:GetFriendListByType(SocietyConfig.FriendGroupType.friend)
    local list2 = self:GetFriendListByType(group_type)
    for _, v  in pairs(list1) do
        table.insert(list, v)
    end
    for _, v  in pairs(list2) do
        table.insert(list, v)
    end
    return list
end

function SocietyData:SetRecentlyChatRole(uid)
    self.recently_chat_role = uid
end

function SocietyData:GetFriendByUid(uid)
    return self.friend_list[uid]
end

--[[好友信息 = {
    uid 
    name 
    prof
    level
    avatar_type 
    avatar_id 
    top_level 
    love 
    sex
    family
    is_online
    vip_level
    group_type 
    avatar_timestamp 
    last_online_timestamp
}]]
--根据好友uid获取好友信息
function SocietyData:GetFriendInfoByUid(uid)
    if self.friend_list and self.friend_list[uid] then
        return self.friend_list[uid].info
    end
end

function SocietyData:IsOnLineByUid(uid)
    local info = self:GetFriendInfoByUid(uid)
    if info ~= nil then
        return info.is_online >= 1
    end
    return false
end

--根据好友uid获取好友亲密度
function SocietyData:GetIntimacy(uid)
    local info = self:GetFriendInfoByUid(uid)
    if info ~= nil then
        return info.love
    end
    return 0
end

--获取好友性别
function SocietyData:GetFriendSex(uid)
    local info = self:GetFriendInfoByUid(uid)
    if info ~= nil then
        return info.sex
    end
    return 0
end

--获取送花配置
function SocietyData:GetFriendFlowerData(seq)
    return Config.sundries_auto.flower[seq or 1]
end

function SocietyData:NewFriendRedPoint(new_friend)
    -- if new_friend then
    --     self.new_friend_red_point.val = true
    --     return
    -- end
    local frp = false
	for _, friend in pairs(self.friend_apply_list) do
		frp = true
	end
	self.new_friend_red_point.val = frp
end

function SocietyData:SetFriendChatRedPoint(uid, value, check_group_red_point)
    local friend = self.friend_list[uid]
    -- 如果该对象是快捷聊天的对象则不显示红点
    value = value and uid ~= self.quick_chat_info.cur_sel
    if nil == friend then return end
    local info = friend.info
    friend:ChatRedPoint(value)
    self:FriendChatRedPoint()
    if value then
        if nil ~= info.group_type1 then self.friend_group_list[info.group_type1]:GroupRedPoint(true) end
        if nil ~= info.group_type2 then self.friend_group_list[info.group_type2]:GroupRedPoint(true) end
    elseif check_group_red_point then
        if nil ~= info.group_type1 then self:CheckFriendChatGroupRedPoint(info.group_type1) end
        if nil ~= info.group_type2 then self:CheckFriendChatGroupRedPoint(info.group_type2) end
    end
end

function SocietyData:SetFriendGiftRedPoint(uid, value, check_group_red_point)
    local friend = self.friend_list[uid]
    -- 如果该对象是快捷聊天的对象则不显示红点
    value = value and uid ~= self.quick_chat_info.cur_sel
    if nil == friend then return end
    local info = friend.info
    friend:GiftRedPoint(value)
    self:FriendGiftRedPoint()
    if value then
        if nil ~= info.group_type1 then self.friend_group_list[info.group_type1]:GroupRedPoint(true) end
        if nil ~= info.group_type2 then self.friend_group_list[info.group_type2]:GroupRedPoint(true) end
    elseif check_group_red_point then
        if nil ~= info.group_type1 then self:CheckFriendChatGroupRedPoint(info.group_type1) end
        if nil ~= info.group_type2 then self:CheckFriendChatGroupRedPoint(info.group_type2) end
    end
end

-- 检测分组红点是否应该消失 如果没有红点直接不检测
function SocietyData:CheckFriendChatGroupRedPoint(group_type)
    if not self.friend_group_list[group_type]:GroupRedPointVal() then return end
    local value = false
    if SocietyConfig.FriendGroupType.group == group_type then
        for _, friend in pairs(self.friend_list) do
            local friend_info = friend.info
            if friend_info.group_type1 == group_type or friend_info.group_type2 == group_type then
                value = value or friend:ChatRedPointVal() or friend:GiftRedPointVal()
            end
        end
    else
        for _, chat_group in pairs(self.chat_group_list) do
            value = value or chat_group:ChatRedPointVal()
        end
    end
    self.friend_group_list[group_type]:GroupRedPoint(value)
end

function SocietyData:FriendChatRedPoint()
    local frp = false
	for _, friend in pairs(self.friend_list) do
		frp = frp or friend.chat_red_point.val
	end
	self.friend_chat_red_point.val = frp
end

function SocietyData:FriendGiftRedPoint()
    local frp = false
	for _, friend in pairs(self.friend_list) do
		frp = frp or friend.gift_red_point.val
	end
	self.friend_gift_red_point.val = frp
end

function SocietyData:GetFriendChatGroupInviteList()
    local list = {}
    local member_list = self.chat_group_detail.member_list
    local friend_list = self:GetFriendListByType(SocietyConfig.FriendGroupType.friend)
    for _, role  in pairs(friend_list) do
        if not member_list[role:Uid()] then
            table.insert(list, role)
        end
    end
    table.sort(list, function (a, b)
        local ai = a.info
        local bi = b.info
        if ai.is_online == bi.is_online then
            if 0 == ai.is_online then
                return ai.last_online_timestamp > bi.last_online_timestamp
            else
                local ats = a.chat_ts or 0
                local bts = b.chat_ts or 0
                return ats > bts
            end
        end
        return ai.is_online > bi.is_online
    end)
    return list
end

function SocietyData:GetFriendChatGroupMemberList()
    local list = {}
    local member_list = self.chat_group_detail.member_list
    for _, role  in pairs(member_list) do
        table.insert(list, role)
    end
    table.sort(list, function (a, b)
        local aim = a.is_master and 1 or 0
        local bim = b.is_master and 1 or 0
        return aim > bim
    end)
    return list
end

function SocietyData:ResetFriendChatGroupSels()
    self.chat_group_sels:Set{
        uid_list = {},
        num = 0,
    }
end

function SocietyData:SetFriendChatGroupSels(uid, value)
    self.chat_group_sels.uid_list[uid] = value and uid or nil
    local num = self.chat_group_sels.num
    local num = 0
    for k, v in pairs(self.chat_group_sels.uid_list) do
        num = num + 1
    end
    self.chat_group_sels.num = num
end

function SocietyData:SetFriendChatGroupRedPoint(group_key, value)
    -- 如果该对象是快捷聊天的对象则不显示红点
    value = value and group_key ~= self.quick_chat_info.cur_sel
    local is_temp_rp = nil ~= self.chat_group_temp and self.chat_group_temp:GroupKey() == group_key
    if is_temp_rp then
        self.chat_group_temp:ChatRedPoint(value)
    else
        for _, chat_group in pairs(self.chat_group_list) do
            if chat_group:GroupKey() == group_key and 1 == chat_group:DisturbVal() then
                chat_group:ChatRedPoint(value)
            end
        end
    end
    self:FriendChatGroupRedPoint()
    if value and (not is_temp_rp) then
        self.friend_group_list[SocietyConfig.FriendGroupType.group]:GroupRedPoint(true)
    end
end

function SocietyData:FriendChatGroupRedPoint()
    local cgrp = false
	for _, chat_group in pairs(self.chat_group_list) do
		cgrp = cgrp or chat_group.chat_red_point.val
	end
	self.friend_chat_group_red_point.val = cgrp
end

function SocietyData:GetChatQuickShowList()
    local list1, list2 = {}, {}
    for i = 1, math.min(#self.quick_chat_info.show_list, 3) do
        local info = self.quick_chat_info.show_list[i]
        table.insert(list1, info)
    end
    if #self.quick_chat_info.show_list > 3 then
        for i = 4, #self.quick_chat_info.show_list do
            local info = self.quick_chat_info.show_list[i]
            table.insert(list2, info)
        end
    end
    return list1, list2
end
