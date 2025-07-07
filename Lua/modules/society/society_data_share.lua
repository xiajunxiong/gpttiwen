
function SocietyData:ShareConfig()
    self.config_share = Config.friendly_gift_auto
    self.config_share_send_flowers = self.config_share.send_flowers
    self.config_share_other = self.config_share.other[1]
end

function SocietyData:ShareData()
    self.share_info = SocietyShareInfo.New()
    self.send_info_sm = SmartData.New{
        type = SocietyConfig.ShareType.None,
        uid = 0,
        item_id = 0,
        send_num = 0,
        sel_role = SmartData.New{},
        sel_item = 0,
        role_infos = SmartData.New{},
        flush_msg = SmartData.New{val = false},
    }
    self.gift_info_sm = SmartData.New{
        state = SocietyConfig.GiftMsgType.send,
        info = {},
    }
    self.get_gift_records = {
        [SocietyConfig.GiftMsgType.send] = {},
        [SocietyConfig.GiftMsgType.fetch] = {},
        [SocietyConfig.GiftMsgType.flower] = {},
    }
    self.get_gift_info = {}
    self.gift_send_uid_list = {}

    self.send_list_flower = {}
    self.send_flower_list = {}
    for k, v in ipairs(self.config_share_send_flowers) do
        local item_data = Item.Init(v.item_id)
        table.insert(self.send_list_flower, item_data)
        self.send_flower_list[v.item_id] = v
    end
    self.friend_intimacy_records = {
        [SocietyConfig.IntimacyMsgType.first_to] = {},
        [SocietyConfig.IntimacyMsgType.first_from] = {},
    }
end

function SocietyData:SetGiftRoleInfo(protocol)
    self.send_info_sm.role_infos:Set(protocol.info_list)
end

function SocietyData:SetGetGiftInfo(protocol)
    if SocietyConfig.GiftInfoType.default == protocol.type then
        ChatData.Instance:ClearFriendRecord()
        self.get_gift_info = protocol.info_list
        self.get_gift_records = {
            [SocietyConfig.GiftMsgType.send] = {},
            [SocietyConfig.GiftMsgType.fetch] = {},
            [SocietyConfig.GiftMsgType.flower] = {},
        }
    elseif SocietyConfig.GiftInfoType.update == protocol.type then
        for uuid_key, info in pairs(protocol.info_list) do
            self.get_gift_info[uuid_key] = info
        end
    end
    if SocietyConfig.GiftInfoType.default == protocol.type or SocietyConfig.GiftInfoType.update == protocol.type then
        for uuid_key, info in pairs(protocol.info_list) do
            if SocietyConfig.GifeItemType.normal == info.gift_item_type then
                if not self.get_gift_records[SocietyConfig.GiftMsgType.send][uuid_key] then
                    local content = ChatItemGift.ItemIntro(uuid_key, SocietyConfig.GiftMsgType.send)
                    if info.from_role_id == RoleData.Instance:GetRoleId() and self.friend_list[info.to_role_id] then
                        self.cur_sel_role = self.friend_list[info.to_role_id]
                        self:AddChatInfoLocal(content, nil, nil, info.send_time)
                    elseif self.friend_list[info.from_role_id] then
                        ChatData.Instance:SetSingleChatBridge(self.friend_list[info.from_role_id], content, info.send_time)
                    end
                    self.get_gift_records[SocietyConfig.GiftMsgType.send][uuid_key] = info
                end
                if info.fetch_time > 0 and not self.get_gift_records[SocietyConfig.GiftMsgType.fetch][uuid_key] then
                    local content = ChatItemGift.ItemIntro(uuid_key, SocietyConfig.GiftMsgType.fetch)
                    if info.to_role_id == RoleData.Instance:GetRoleId() and self.friend_list[info.from_role_id] then
                        self.cur_sel_role = self.friend_list[info.from_role_id]
                        self:AddChatInfoLocal(content, nil, nil, info.send_time)
                    elseif self.friend_list[info.to_role_id] then
                        ChatData.Instance:SetSingleChatBridge(self.friend_list[info.to_role_id], content, info.fetch_time)
                    end
                    self.get_gift_records[SocietyConfig.GiftMsgType.fetch][uuid_key] = info
                end
            elseif SocietyConfig.GifeItemType.flower == info.gift_item_type then
                if not self.get_gift_records[SocietyConfig.GiftMsgType.flower][uuid_key] then
                    local content = ChatItemGiftFlower.ItemIntro(uuid_key)
                    if info.from_role_id == RoleData.Instance:GetRoleId() and self.friend_list[info.to_role_id] then
                        self.cur_sel_role = self.friend_list[info.to_role_id]
                        self:AddChatInfoLocal(content, nil, nil, info.send_time)
                    elseif self.friend_list[info.from_role_id] then
                        ChatData.Instance:SetSingleChatBridge(self.friend_list[info.from_role_id], content, info.send_time)
                    end
                    self.get_gift_records[SocietyConfig.GiftMsgType.flower][uuid_key] = info
                end
            end
        end
        self.send_info_sm.flush_msg.val = not self.send_info_sm.flush_msg.val
    end

    self.gift_send_uid_list = {}
    for uuid_key, info in pairs(self.get_gift_records[SocietyConfig.GiftMsgType.send]) do
        if not self.get_gift_records[SocietyConfig.GiftMsgType.fetch][uuid_key] then
            if RoleData.Instance:GetRoleId() == info.to_role_id then
                self.gift_send_uid_list[info.from_role_id] = self.gift_send_uid_list[info.from_role_id] or {}
                table.insert(self.gift_send_uid_list[info.from_role_id], info)
            end
        end
    end
    for uid, v in pairs(self.friend_list) do
        self:SetFriendGiftRedPoint(uid, nil ~= self.gift_send_uid_list[uid], true)
    end
end

function SocietyData:SetGiveFlowerNotice(protocol)
    local info = protocol.info
    -- if info.receiver_id == RoleData.Instance:GetRoleId() then
    --     ViewMgr:OpenView(SocietyFlowerGetView, info)
    -- end
end

function SocietyData:SetFriendIntimacyListInfo(protocol)
    self.friend_intimacy_list:Set(protocol.friend_intimacy_list)
    for _, info in pairs(protocol.friend_intimacy_list) do
        self.friend_intimacy_records[SocietyConfig.IntimacyMsgType.first_to][info.uid] = info.day_first_to_singchat_timestamp
        self.friend_intimacy_records[SocietyConfig.IntimacyMsgType.first_from][info.uid] = info.day_first_from_singchat_timestamp
    end
end

function SocietyData:SetFriendIntimacyInfoChange(protocol)
    local info = protocol.friend_intimacy_info
    self.friend_intimacy_list[info.uid] = info
    local today0 = TimeManager.DayStart()
    if info.day_first_to_singchat_timestamp >= today0 and (self.friend_intimacy_records[SocietyConfig.IntimacyMsgType.first_to][info.uid] or 0) < today0 then
        local content = RichTextHelper.ColorStr(string.format(Language.Society.ChatIntimacyAdd[SocietyConfig.IntimacyMsgType.first_to], self:FirstTalkAdd()), COLORSTR.Red14)
        ChatData.Instance:SetSingleChatBridge(self.friend_list[info.uid], content, info.send_time, ChatConfig.MsgType.intimacy_add_notice)
        self.friend_intimacy_records[SocietyConfig.IntimacyMsgType.first_to][info.uid] = info.day_first_to_singchat_timestamp
    end
    if info.day_first_from_singchat_timestamp >= today0 and (self.friend_intimacy_records[SocietyConfig.IntimacyMsgType.first_from][info.uid] or 0) < today0 then
        local content = RichTextHelper.ColorStr(string.format(Language.Society.ChatIntimacyAdd[SocietyConfig.IntimacyMsgType.first_from], self:FirstTalkAdd()), COLORSTR.Red14)
        ChatData.Instance:SetSingleChatBridge(self.friend_list[info.uid], content, info.send_time, ChatConfig.MsgType.intimacy_add_notice)
        self.friend_intimacy_records[SocietyConfig.IntimacyMsgType.first_from][info.uid] = info.day_first_from_singchat_timestamp
    end
end

function SocietyData:SetShareInfo(share_info)
    self.share_info = share_info
end

function SocietyData:SendShareText(cur_sel_role)
    self.cur_sel_role = cur_sel_role
    self:AddChatInfo(self.share_info:ShareText(cur_sel_role.info.uid, RoleData.Instance:GetRoleId()), nil, ChatConfig.ChatContentType.text)
end

function SocietyData:SetSocietySendInfo(type, uid, item_id)
    self.send_info_sm.type = type
    self.send_info_sm.uid = uid
    self.send_info_sm.item_id = item_id
end

function SocietyData:GetSendItemList()
    local list, sel_item = {}
    if SocietyConfig.ShareType.GodLandTrace == self.send_info_sm.type then
        list, sel_item = GodLandTracesData.Instance:GetSendItemList(self.send_info_sm.item_id)
    elseif SocietyConfig.ShareType.GiftFlowers == self.send_info_sm.type then
        list, sel_item = self:GetSendItemListFlower(self.send_info_sm.item_id)
    elseif SocietyConfig.ShareType.LiveItem == self.send_info_sm.type then
        list, sel_item = LiveData.Instance:GetLiveItemList(self.send_info_sm.item_id)
    end
    return list, sel_item
end

function SocietyData:GetSendRemainTimes()
    local remain_times = 0
    if SocietyConfig.ShareType.GodLandTrace == self.send_info_sm.type then
        remain_times = GodLandTracesData.Instance:GetSendRemainTimes(self.send_info_sm.role_infos, self.send_info_sm.sel_role)
    elseif SocietyConfig.ShareType.LiveItem == self.send_info_sm.type then
        local times_group_id = 2
        remain_times = Config.god_land_traces_auto.give_times[times_group_id].give_times
        if self.send_info_sm.role_infos[times_group_id] and self.send_info_sm.sel_role.info then
            local uid = self.send_info_sm.sel_role.info.uid or 0
            remain_times = remain_times - (self.send_info_sm.role_infos[times_group_id][uid] or 0)
        end
    end
    return remain_times
end

function SocietyData:SetSendInfoSendNum(send_num)
    self.send_info_sm.send_num = send_num
end

function SocietyData:GetSendInfoSendNum()
    return self.send_info_sm.send_num
end

function SocietyData:SetSendInfoSelItem(item_id)
    if self.send_info_sm.sel_item ~= item_id then
        self.send_info_sm.send_num = 1
    end
    self.send_info_sm.sel_item = item_id
end

function SocietyData:SetSocietyGiftInfo(state, uuid_key)
    self.gift_info_sm.state = state
    self.gift_info_sm.uuid_key = uuid_key
end

function SocietyData:GetGiftUUIDKeyByUid(uid)
    local list = self.gift_send_uid_list[uid]
    local value, uuid_key = false
    if list then
        value = true
        local info = list[1]
        for k, v in pairs(list) do
            if info.send_time < v.send_time then
                info = v
            end
        end
        uuid_key = info.uuid_key
    end
    return value, uuid_key
end

function SocietyData:GetFriendIntimacyVal(uid)
    return self.friend_intimacy_list[uid] and self.friend_intimacy_list[uid].intimacy_val or 0
end

function SocietyData:GetSendItemListFlower(item_id)
    local list, sel_item = {}
    for _, item in ipairs(self.send_list_flower) do
        local have_num = BagData.Instance:GetNumByItemId(item.item_id)
        if item_id == item.item_id then
            sel_item = item
        end
        item.num = have_num
        item.is_grey = 0 == have_num
        table.insert(list, item)
    end
    return list, sel_item or list[1]
end

function SocietyData:GetSendItemTypeShow(item_id)
    local show_co = SocietyConfig.GiftItemTypeShow.default
    if SocietyConfig.ShareType.GiftFlowers == self.send_info_sm.type then
        local co = self.send_flower_list[item_id]
        if co and 2 == co.type then
            show_co = SocietyConfig.GiftItemTypeShow.flower2
        end
    end
    return show_co
end

function SocietyData:GetSendFlowerMaxNum(item_id, auto_buy)
    local max_num = 999
    if SocietyConfig.ShareType.GiftFlowers == self.send_info_sm.type then
        local co = self.send_flower_list[item_id]
        -- type=2的花不可批量赠送
        if SocietyConfig.GiftItemTypeShow.flower2 == self:GetSendItemTypeShow(item_id) then
            max_num = 1
        else
            local co = ShopData.GetFastBuyData(item_id)
            if nil == co or not auto_buy then
                max_num = BagData.Instance:GetNumByItemId(item_id)
            else
                max_num = BagData.Instance:GetNumByItemId(item_id) + math.floor(RoleData.Instance:GetCurrency(co.money_type) / co.price)
            end
        end
    elseif SocietyConfig.ShareType.LiveItem == self.send_info_sm.type then
        max_num = Item.GetNum(item_id)
        local limit_num = Config.god_land_traces_auto.give_times[2].give_times or 1
        max_num = max_num > limit_num and limit_num or max_num
    end
    max_num = math.max(1, max_num)
    return max_num
end

function SocietyData.GetGiftItemType(item)
    local gift_item_type = SocietyConfig.GifeItemType.normal
    if ShowType.GiftFlower == item:ShowType() then
        gift_item_type = SocietyConfig.GifeItemType.flower
    end
    return gift_item_type
end

function SocietyData:GetGiftFlowerIntimacyAdd(item_id)
    local value = 0
    if ShowType.GiftFlower == Item.GetShowType(item_id) then
        local co = self.send_flower_list[item_id]
        value = co and co.intimacy_add or 0
    end
    return value
end

function SocietyData:PlayGiftEffect(type, val)
    self.gift_effect:Set{type = type, val = val}
end

function SocietyData:FirstTalkAdd()
    return self.config_share_other.first_talk_add
end

function SocietyData:SocietySendGiftNumEnough(language, auto_buy, down_cur_num)
    local value = true
    local type = self.send_info_sm.type
    if SocietyConfig.ShareType.GodLandTrace == type and 0 == self:GetSendRemainTimes() then
		PublicPopupCtrl.Instance:Center(language.EmptyNumTips)
		value = false
	elseif SocietyConfig.ShareType.GiftFlowers == type then
        local item_id = self.send_info_sm.sel_item
        local co = ShopData.GetFastBuyData(item_id)
        local have_num = BagData.Instance:GetNumByItemId(item_id)
        if nil == co or not auto_buy then
            if have_num < down_cur_num then
                value = false
                MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = item_id})})
            end
        else
            if have_num < down_cur_num then
                if (math.floor(RoleData.Instance:GetCurrency(co.money_type) / co.price)) < (down_cur_num - have_num) then
                    value = false
                    MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = HuoBi[co.money_type]})})
                end
            end
        end
    end
    return value
end

function SocietyData:IsSendGiftBlockItemInfo()
    local value = self.is_send_gift_block_item_info
    self.is_send_gift_block_item_info = false
    return value
end