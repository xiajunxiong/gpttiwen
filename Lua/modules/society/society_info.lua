
SocietyRoleInfo = SocietyRoleInfo or BaseClass()

function SocietyRoleInfo:__init()
	self.chat_red_point = SmartData.New{ val = false }
	self.gift_red_point = SmartData.New{ val = false }
	self.achieve_red_point = SmartData.New{ val = false }
end

function SocietyRoleInfo:RoleInfo(info)
	self.info = info
	self.quick_type = SocietyConfig.QuickShowType.role
	return self
end

function SocietyRoleInfo:EmptyInfo()
	self.info = nil
	self.chat_red_point.val = false
	self.gift_red_point.val = false
	self.achieve_red_point.val = false
end

function SocietyRoleInfo:RecentChat()
	self.chat_ts = TimeHelper.GetServerTime()
end

function SocietyRoleInfo:ChatRedPoint(value)
	self.chat_red_point.val = value
end

function SocietyRoleInfo:ChatRedPointVal()
	return self.chat_red_point.val
end

function SocietyRoleInfo:GiftRedPoint(value)
	self.gift_red_point.val = value
end

function SocietyRoleInfo:GiftRedPointVal()
	return self.gift_red_point.val
end

function SocietyRoleInfo:ShiTuAchieveRedPoint(val_cap, val_dap)
	-- self.achieve_red_point.val = value
	local cap = false
    local dap = false
	if self.info then
		local list = SocietyData.Instance:GetShiTuAchieveShowList3()
		if val_cap then
			for k, v in ipairs(list) do
				if 1 == v.type then
					local value = SocietyData.Instance:GetShiTuInfoAchieveValue2(v.type, self.info)
					local is_get = SocietyData.Instance:GetShiTuInfoAchieveIsGet2(v.seq, self.info)
					local can_get = value >= v.param
					cap = cap or (can_get and not is_get)
				end
			end
		end
		if val_dap then
			for k, v in ipairs(list) do
				if 2 == v.type then
					local value = SocietyData.Instance:GetShiTuInfoAchieveValue2(v.type, self.info)
					local is_get = SocietyData.Instance:GetShiTuInfoAchieveIsGet2(v.seq, self.info)
					local can_get = value >= v.param
					dap = dap or (can_get and not is_get)
				end
			end
		end
	end
	self.achieve_red_point.val = cap or dap
	return self.achieve_red_point.val
end

function SocietyRoleInfo:AchieveRedPointVal()
	return self.achieve_red_point.val
end

function SocietyRoleInfo:Uid()
	return self.info.uid
end

function SocietyRoleInfo:IsSelf()
	return self.info.uid == RoleData.Instance:GetRoleId()
end

function SocietyRoleInfo:QuickVal()
	return self.info.uid
end

function SocietyRoleInfo:IsUid(uid)
	return nil ~= self.info and self.info.uid == uid
end

SocietyChatGroupInfo = SocietyChatGroupInfo or BaseClass()

function SocietyChatGroupInfo:__init(info, group_key)
	self.info = info
	self.group_key = group_key
	self.chat_red_point = SmartData.New{ val = false }
	self.gift_red_point = SmartData.New{ val = false }
	self.notice_show = true
	self.quick_type = SocietyConfig.QuickShowType.group
	SocietyCtrl.Instance:SendGetGroupInfo(self:GroupId())
end

function SocietyChatGroupInfo:GroupInfo(info)
	self.info = info
	return self
end

function SocietyChatGroupInfo:SetGroupNotice(group_notice)
	if self.group_notice and self.group_notice ~= group_notice then
		SocietyData.Instance:AddGroupChatInfo{
			group_id = self:GroupId(),
			msg_type = ChatConfig.MsgType.group_notice,
			content = string.format(Language.Society.ChatGroupNoticeModif, group_notice),
		}
	end
	self.group_notice = group_notice
end

function SocietyChatGroupInfo:SetGroupNoticeT(group_notice)
	-- if self:Master() == RoleData.Instance:GetRoleId() then return end
	if string.find(group_notice, "become_official") then
		group_notice = string.sub(group_notice, 1, -2)
		group_notice = group_notice .. string.format("%s;%s}", self:GroupKey(), RoleData.Instance:GetRoleId() == self:Master() and 1 or 0)
	end
	SocietyData.Instance:AddGroupChatInfo{
		group_id = self:GroupId(),
		msg_type = ChatConfig.MsgType.temp_group_notice,
		content = RichTextHelper.ParseAll(group_notice, true, RTParseType.SocietyChat),
		content_quick = RichTextHelper.ParseAll(group_notice, true, RTParseType.QuickChat),
	}
end

function SocietyChatGroupInfo:GroupId()
	return self.info.group_id
end

function SocietyChatGroupInfo:GroupName()
	return self.info.group_name
end

function SocietyChatGroupInfo:GroupNotice()
	-- return StringIsEmpty(self.group_notice) and Language.Society.ZanWu or self.group_notice
	return self.group_notice
end

function SocietyChatGroupInfo:Avatars()
	return self.info.avatars
end

function SocietyChatGroupInfo:IsCross()
	return 1 == self.info.is_cross
end

function SocietyChatGroupInfo:CrossState()
	return self.info.is_cross + 1
end

function SocietyChatGroupInfo:MemberCount()
	return self.info.member_count
end

function SocietyChatGroupInfo:Master()
	return self.info.master
end

function SocietyChatGroupInfo:ChatRedPoint(value)
	self.chat_red_point.val = value
end

function SocietyChatGroupInfo:ChatRedPointVal()
	return self.chat_red_point.val
end

function SocietyChatGroupInfo:GiftRedPoint(value)
	self.gift_red_point.val = value
end

function SocietyChatGroupInfo:GiftRedPointVal()
	return self.gift_red_point.val
end

function SocietyChatGroupInfo:GroupKey()
	return self.group_key
end

function SocietyChatGroupInfo:BlockNoticeShow()
	self.notice_show = false
end

function SocietyChatGroupInfo:NoticeShow()
	return self.notice_show
end

function SocietyChatGroupInfo:GroupInviteStr()
	return string.format("{group_invite;%s;%s;%s;%s}", self.group_key, self.info.is_cross, self.info.group_name, RoleData.Instance:GetRoleId())
end

function SocietyChatGroupInfo:QuickVal()
	return self.group_key
end

function SocietyChatGroupInfo:SetDisturbVal(set_value)
	self.disturb_val = set_value
end

function SocietyChatGroupInfo:DisturbVal()
	return self.disturb_val or 1
end

SocietyFriendGroupInfo = SocietyFriendGroupInfo or BaseClass()

function SocietyFriendGroupInfo:__init(type)
	self.group_type = type
	self:FriendGroupInfo(type)
	self.group_red_point = SmartData.New{ val = false }
end

function SocietyFriendGroupInfo:FriendGroupInfo(type)
	self.not_create = true
	self.group_name = Language.Society.FriendGroupType[type]
end

function SocietyFriendGroupInfo:GroupName(group_name)
	self.not_create = StringIsEmpty(group_name)
	self.group_name = (not StringIsEmpty(group_name)) and group_name or Language.Society.FriendGroupType[self.group_type]
end

function SocietyFriendGroupInfo:GroupRedPoint(value)
	-- 屏蔽自定义分组1、2，最近队友红点
	if self.group_type > 4 then return end
	self.group_red_point.val = value
end

function SocietyFriendGroupInfo:GroupRedPointVal()
	return self.group_red_point.val
end

SocietyMailInfo = SocietyMailInfo or BaseClass()

function SocietyMailInfo:__init()
	self.mail_red_point = SmartData.New{ val = false }
end

function SocietyMailInfo:MailInfo(info)
	self.info = info
	self.sort_index = info.sort_index
	self.mail_index = info.mail_index
	self.is_read = 1 == info.is_read
	self.is_fetch = 1 == info.is_fetch
	self.recv_time = info.recv_time
	self.has_attchment = 1 == info.has_attchment
	self.item_list = {}
	self.mail_name = self:CheckMailName()

	self:MailRedPoint()
end

function SocietyMailInfo:CheckMailName()
	return Language.Society.MailSenders[self.info.kind] or ""
	-- return Language.Society.MailSenders[self.info.kind] or self.info.sender_name
end

function SocietyMailInfo:MailDetail(detail)
	self.detail = detail
	self:CheckItemList()
end

function SocietyMailInfo:CheckItemList()
	if nil == self.detail then return end
	local t = {}
	local function CheckReward(reward, item_id)
		if reward ~= 0 then
			local item_data = Item.New()
			item_data.item_id = item_id
			item_data.num = reward
			item_data.is_bind = 0
			item_data.is_grey = self.is_fetch
			table.insert(t, item_data)
		end
	end

	local virtual_item_list = self.detail.virtual_item_list
	for index, num in pairs(virtual_item_list) do
		local item_id = SocietyConfig.MailVirtualItemId[index]
		if num > 0 and nil ~= item_id then
			CheckReward(num, item_id)
		end
	end

	local item_list = self.detail.item_list
	for _, vo in pairs(item_list) do
		local item_id = vo.item_id
		if 0 ~= item_id then
			local item_data = Item.Create(vo)
			item_data.old_id = Cfg.RewardJobBagHasId(item_id, false) and item_id or nil
			item_data.is_grey = self.is_fetch
			table.insert(t, item_data)
		end
	end
	if SocietyConfig.MailReason.arena_match == self.detail.mail_reason then
		self.item_list = ArenaMatchData.RewardsSort(t)
	else
		self.item_list = t
	end
end

function SocietyMailInfo:EmptyInfo()
	self.info = nil
	self.detail = nil
	self.mail_red_point.val = false
end

function SocietyMailInfo:ReadMail()
	self.is_read = true
	self:MailRedPoint(true)
end

function SocietyMailInfo:FetchMail()
	self.is_fetch = true
	self:CheckItemList()
	self:MailRedPoint()
end

function SocietyMailInfo:MailRedPoint(need_check)
	local mrp = true
	if self.is_read then
		if not self.has_attchment then
			mrp = false
		elseif self.is_fetch then
			mrp = false
		end
	end
	self.mail_red_point.val = mrp
	if need_check then
		local society_data = SocietyCtrl.Instance:Data()
		society_data:MailRedPoint()
	end
end

SocietyShareInfo = SocietyShareInfo or BaseClass()

function SocietyShareInfo:__init()
	self.type = SocietyConfig.ShareType.None
end

function SocietyShareInfo:ShareText()
	return ""
end

SocietyShareInfoGodLandTrace = SocietyShareInfoGodLandTrace or BaseClass(SocietyShareInfoItem)

function SocietyShareInfoGodLandTrace:__init(item)
	self.type = SocietyConfig.ShareType.GodLandTrace
	self.item = item
end

function SocietyShareInfoGodLandTrace:ShareText(to_uid, from_uid)
	PublicPopupCtrl.Instance:Center(Language.Society.Share.ShareTips)
	return string.format("{share_god_land_trace;%s;%s;%s;%s}", self.type, to_uid, self.item.item_id, from_uid)
end