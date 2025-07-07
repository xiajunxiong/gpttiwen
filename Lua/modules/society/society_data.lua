
SocietyData = SocietyData or BaseClass()

function SocietyData:__init()
    if SocietyData.Instance ~= nil then
		Debuger.LogError("[SocietyData] attempt to create singleton twice!")
		return
	end
    SocietyData.Instance = self

    self.mail_red_point = SmartData.New{val = false}
    self.friend_chat_red_point = SmartData.New{val = false}
    self.friend_gift_red_point = SmartData.New{val = false}
    self.friend_chat_group_red_point = SmartData.New{val = false}
    self.new_friend_red_point = SmartData.New{val = false}
    self.new_master_red_point = SmartData.New{val = false}
    self.shitu_achieve_red_point = SmartData.New{val = false}
    
    self.chat_group_list = {}
    self.chat_group_temp = nil
    self.chat_group_detail = SmartData.New{group_id = 0, group_key = "a", member_list = {}}
    self.chat_group_sels = SmartData.New{
        uid_list = {},
        num = 0,
    }
    self.chat_group_channels = SmartData.New()
    
    self.first_flood_sm = SmartData.New{
        type = 0,
        level_id = 0,
    }

    self.rank_sm = SmartData.New()

    self.rank_flush = SmartData.New{
        list = false,
    }

    self.maq_info = SmartData.New()
    self.maq_options = SmartData.New()

    self.shitu_fb_sm = SmartData.New{
        end_time = 0,
        progress = 0,
    }

    self.quick_chat_info = SmartData.New{
        show_list = {},
        show_id_list = {
            friends = {},
            groups = {},
        },
        cur_sel = "",
        flush_records = false,
        flush_items = false,
        is_editing = false,
    }

    self.quick_chat_fly = {
		state = SocietyConfig.ChatQuickFlyState.finish,
        data = nil,
        item = nil,
    }
    self.is_mail_shield_tip = false  -- 是否屏蔽邮件提示
    self.mail_shield_tip_list = {}   -- 屏蔽邮件提示的时候缓存的提示
    self.my_rank_show = SmartData.New{val = 0}

    self.gift_effect = SmartData.New({type = 0, val = 0})

    self.friend_intimacy_list = SmartData.New()

    self:FriendConfig()
    self:MailConfig()
	self:MasterApprenticeConfig()
    self:FirstKillConfig()
    self:ShiTuConfig()
    self:ShiTuFBConfig()
    self:ShareConfig()
    self:SocietyData()
    self.rank_refresh_time = SmartData.New({time = TimeCtrl.Instance:GetServerTime()})
end

function SocietyData:__delete()
    SocietyData.Instance = nil
end

function SocietyData:SocietyData()
    self.role_cache_list = {}
    self.mail_cache_list = {}

    self:ChatData()    
    self:FriendData()
    self:MailData()
    self:MasterApprenticeData()
    self:FirstKillData()
    self:ShiTuData()
    self:ShiTuFBData()
    self:RankData()
    self:ShareData()

    self.mail_red_point.val = false
    self.friend_chat_red_point.val = false
    self.friend_gift_red_point.val = false
    self.friend_chat_group_red_point.val = false
    self.new_friend_red_point.val = false
    self.new_master_red_point.val = false
    self.shitu_achieve_red_point.val = false

    self.guide_group = false
    self.guide_group_create = SocietyConfig.ChatQuickCreateState.state0
end

function SocietyData:SocietyDataR()
	for _, channel in pairs(self.chat_group_channels) do
		channel:EmptyChatList()
    end
    self:SetTempGroupLeaveNotice(true)
end

function SocietyData:RoleInfoGet()
    if next(self.role_cache_list) then
        return table.remove(self.role_cache_list, #self.role_cache_list)
    else
        return SocietyRoleInfo.New()    
    end
end

function SocietyData:RoleInfoRecycle(role)
    role:EmptyInfo()
    table.insert(self.role_cache_list, role)
end

function SocietyData:MailInfoGet()
    if next(self.mail_cache_list) then
        return table.remove(self.mail_cache_list, #self.mail_cache_list)
    else
        return SocietyMailInfo.New()    
    end
end

function SocietyData:MailInfoRecycle(mail)
    mail:EmptyInfo()
    table.insert(self.mail_cache_list, mail)
end

function SocietyData.SearchOne(list, text)
    local t = {}
    local uid_tn = tonumber(text)
    for _, v in pairs(list) do
        local info = v.info
        if text == v.info.name or uid_tn == v.info.uid then
            table.insert(t, v)
        end
    end
    return t
end

function SocietyData:CurSelRole(info)
    self.cur_sel_role = {
        info = info
    }
end

function SocietyData.GroupId2Key(group_id)
    return string.format("%sa%s", group_id[1], group_id[2])
end

function SocietyData.GroupKey2Id(group_key)
    local group_id = string.split(group_key, "a")
    return {
        [1] = tonumber(group_id[1]),
        [2] = tonumber(group_id[2]),
    }
end

function SocietyData:SetRankRefreshTime(protocol)
    self.rank_refresh_time.time = protocol.time
end

function SocietyData:GetRankRefreshTime()
    return self.rank_refresh_time.time - TimeCtrl.Instance:GetServerTime()
end