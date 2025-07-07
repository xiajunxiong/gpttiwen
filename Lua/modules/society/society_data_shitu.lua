
function SocietyData:ShiTuConfig()
    self.shitu_game_show_list = {}
    for k, v in ipairs(Config.mentor_cfg_auto.introduce) do
        v.desc_show = Config.language_cfg_auto.textdesc[v.desc_seq] and Config.language_cfg_auto.textdesc[v.desc_seq].desc or ""
        local master_reward_str = string.split(v.master_reward, ":")
        local apprentice_reward_str = string.split(v.apprentice_reward, ":")
        local master_reward_show = {}
        local apprentice_reward_show = {}
        for i = 1, #master_reward_str do
            table.insert(master_reward_show, Item.Init(tonumber(master_reward_str[i])))
        end
        for i = 1, #apprentice_reward_str do
            table.insert(apprentice_reward_show, Item.Init(tonumber(apprentice_reward_str[i])))
        end
        v.master_reward_show = master_reward_show
        v.apprentice_reward_show = apprentice_reward_show
        table.insert(self.shitu_game_show_list, v)
    end

    self.shitu_achieve_show_list = {}
    for k, v in ipairs(Config.mentor_cfg_auto.milepost) do
        local master_reward_show = {}
        local apprentice_reward_show = {}
        for k, v in pairs(v.master_reward) do
            table.insert(master_reward_show, Item.Init(v.item_id, v.num, v.is_bind))
        end
        for k, v in pairs(v.apprentice_reward) do
            table.insert(apprentice_reward_show, Item.Init(v.item_id, v.num, v.is_bind))
        end
        v.master_reward_show = master_reward_show
        v.apprentice_reward_show = apprentice_reward_show
        table.insert(self.shitu_achieve_show_list, v)
    end

    self.shitu_chuan_gong_reward_list = {}
    self.shitu_chuan_gong_score_max = 0

    for k, v in ipairs(Config.mentor_cfg_auto.game_reward) do
        if v.grade > 0 then
            local master_reward_show = {}
            local apprentice_reward_show = {}
            for k, v in pairs(v.master_reward) do
                table.insert(master_reward_show, Item.Init(v.item_id, v.num, v.is_bind))
            end
            for k, v in pairs(v.apprentice_reward) do
                table.insert(apprentice_reward_show, Item.Init(v.item_id, v.num, v.is_bind))
            end
            v.master_reward_show = master_reward_show
            v.apprentice_reward_show = apprentice_reward_show
            table.insert(self.shitu_chuan_gong_reward_list, v)
        end
        self.shitu_chuan_gong_score_max = math.max(self.shitu_chuan_gong_score_max, v.score)
    end
end

function SocietyData:ShiTuData()
    self.shitu_fb_progress_t = {}
    self.shitu_sync_info = {}

    self.shitu_info_sm = SmartData.New{
        is_get_shi_tu_reward = 0,

        day_mile_cap_fetch_num = 0,
        day_mile_pass_day_fetch_num = 0,

        week_chuan_gong_flag = 0,
        week_chuan_gong_exp_flag = 0,
        week_chuan_gong_score_max = 0,
        week_chuan_gong_progress = 0,
        mile_reward_flag = 0,
    }

    self.shitu_chuangong_begin_time = 0

    self.shitu_chuangong_scene_info = {init = true}
    self.shitu_chuangong_loop = true
    self.shitu_chuangong_seek_master = true
end

function SocietyData:GetShiTuGameShowList()
    return self.shitu_game_show_list
end

function SocietyData:GetShiTuAchieveShowList()
    local list = self.shitu_achieve_show_list
    table.sort(list, function (a, b)
        if not a or not b then return a end
        local a_is_get = self:GetShiTuInfoAchieveIsGet(a.seq) and 1 or 0
        local b_is_get = self:GetShiTuInfoAchieveIsGet(b.seq) and 1 or 0
        if a_is_get == b_is_get then
            local a_can_get = self:GetShiTuInfoAchieveValue(a.type) >= a.param and 1 or 0
            local b_can_get = self:GetShiTuInfoAchieveValue(b.type) >= b.param and 1 or 0
            if a_can_get == b_can_get then
                local ao = a.order
                local bo = b.order
                if ao == bo then
                    local at = a.type
                    local bt = b.type
                    return at < bt
                end
                return ao < bo
            end
            return a_can_get > b_can_get
        end
        return a_is_get < b_is_get
    end)
    return list
end

function SocietyData:GetShiTuAchieveShowList2()
    local list = self.shitu_achieve_show_list
    table.sort(list, function (a, b)
        if not a or not b then return a end
        local ao = a.order
        local bo = b.order
        if ao == bo then
            local at = a.type
            local bt = b.type
            return at < bt
        end
        return ao < bo
    end)
    return list
end

function SocietyData:GetShiTuAchieveShowList3()
    return self.shitu_achieve_show_list
end

function SocietyData:GetShiTuListTeamInvite(seek_master)
    local list = {}
    for i = 1, SocietyConfig.MASTER_APPRENTICE_MAX do
        local role = seek_master and self.master_apprentice_list_m[i] or self.master_apprentice_list_a[i] 
        if role.info and 1 == role.info.is_online then
            table.insert(list, role)
        end
    end
    return list
end

function SocietyData:GetShiTuInfoAchieveValue(type)
    local value = 0
    local role = self.cur_sel_role
    if role and role.info then
        if 1 == type then
            value = role.info.apprentice_cap_value or 0
        elseif 2 == type then
            local ft = TimeHelper.FormatDHMS(TimeHelper.GetServerTime() - (role.info.shitu_time or 0))
            value = ft.day
        end
    end
    return value
end

function SocietyData:GetShiTuInfoAchieveValue2(type, info)
    local value = 0
    if 1 == type then
        value = info.apprentice_cap_value or 0
    elseif 2 == type then
        local ft = TimeHelper.FormatDHMS(TimeHelper.GetServerTime() - (info.shitu_time or 0))
        value = ft.day
    end
    return value
end

function SocietyData:GetShiTuInfoAchieveIsGet(seq)
    local value = false
    -- local role = self.cur_sel_role
    -- if role and role.info then
        -- value = 1 == bit:_rshift(role.info.mile_reward_flag, seq) % 2
        value = 1 == bit:_rshift(self.shitu_info_sm.mile_reward_flag, seq) % 2
    -- end
    return value
end

function SocietyData:GetShiTuInfoAchieveIsGet2(seq, info)
    local value = false
    -- value = 1 == bit:_rshift(info.mile_reward_flag, seq) % 2
    value = 1 == bit:_rshift(self.shitu_info_sm.mile_reward_flag, seq) % 2
    return value
end


function SocietyData:GetShiTuCGDetailInfoRewardShow()
    if nil == self.shitu_cg_detail_info_rewards then
        self.shitu_cg_detail_info_rewards = {}
        for k, v in ipairs(string.split(self.config_ma_other.reward_show2, ":")) do
            table.insert(self.shitu_cg_detail_info_rewards, Item.Init(tonumber(v)))
        end
    end
    return self.shitu_cg_detail_info_rewards
end

function SocietyData:SetShiTuChuanGongGameInfo(protocol)
    if SocietyConfig.ShiTuChuanGongInfoType.game_begin == protocol.type then
        self.shitu_chuangong_begin_time = protocol.param
        MusicGameCtrl.Instance:MusicGame(1)
    elseif SocietyConfig.ShiTuChuanGongInfoType.game_score == protocol.type then
        MusicGameData.Instance:SetScoreAddScale(protocol.param)
    end
end

function SocietyData:SetShiTuChuanGongSceneInfo(protocol)
    local list = {
        role_paths = {},
        role_avatars = {},
    }
    for i = 1, protocol.count do
        local info = protocol.host_list[i]
        local appearance = info.appearance
        local role_path = DrawerHelper.GetRolePath(ResHelper.RoleResId(appearance.avatar_type, appearance.fashion_body_id, appearance.advance_time), appearance.fashion_body_id > 0 and 0 or appearance.color)
        table.insert(list.role_paths, role_path)
        table.insert(list.role_avatars, appearance.avatar_type)
        if 1 == i then
            self.shitu_chuangong_seek_master = RoleData.Instance:GetRoleId() == info.uid
        end
    end
    self.shitu_chuangong_scene_info = list
end

function SocietyData:SetShiTuSyncInfo(protocol)
    if 0 == protocol.type then
        for i = 1, SocietyConfig.MASTER_APPRENTICE_MAX do
            local role = self.master_apprentice_list_m[i]
            if role:IsUid(protocol.param_1) then
                -- local content = ChatItemShiTuInvite.ItemIntro()
                table.insert(self.shitu_sync_info, {
                    role = role,
                    state = protocol.param_2
                })
            end
        end
    else
        for i = 1, SocietyConfig.MASTER_APPRENTICE_MAX do
            local role = self.master_apprentice_list_m[i]
            if role:IsUid(protocol.param_1) then
                local content = ChatItemShiTuInvite.ItemIntro(protocol.param_2)
                ChatData.Instance:SetSingleChatBridge(role, content)
            end
        end
    end
end

function SocietyData:AddShiTuSyncInfo()
    for _, info in ipairs(self.shitu_sync_info) do
        local content = ChatItemShiTuInvite.ItemIntro(info.state)
        ChatData.Instance:SetSingleChatBridge(info.role, content)
    end
    self.shitu_sync_info = {}
end

function SocietyData:ShiTuChuanGongConfigOtherSceneId()
    return Config.mentor_cfg_auto.other[1].passmerit_scene
end

function SocietyData:GetShiTuChuanGongSceneInfo()
    local scene_info = self.shitu_chuangong_scene_info
    if scene_info.init then return false end
    return true, scene_info.role_paths, scene_info.role_avatars
end

function SocietyData:SetShiTuChuanGongRewardShow(protocol)
    self.shitu_chuangong_reward_show = protocol
end

function SocietyData:ShiTuChuanGongRewardShow()
    if self.shitu_chuangong_reward_show then
        local scrs = self.shitu_chuangong_reward_show
        MainOtherCtrl.Instance:OpenGifShiTuCGtView(DataHelper.FormatItemList(scrs.item_list),scrs.param1, scrs.param2, 5)
        self.shitu_chuangong_reward_show = nil
    end
end

function SocietyData:GetShiTuAchieveRewardShow()
    local info = self.shitu_game_show_list[1]
    if info then
        return SocietyConfig.ShiTuSeekingType.seek_master == SocietyViewMasterApprenticeGroupList.seek_type and info.apprentice_reward_show or info.master_reward_show
    end
    return {}
end

function SocietyData:SocietyShiTuAchieveRedPoint()
    local info = self.shitu_info_sm
    local dmcfn = info.day_mile_cap_fetch_num
    local dmpdfn = info.day_mile_pass_day_fetch_num

    local arp = false

    for i = 1, SocietyConfig.MASTER_APPRENTICE_MAX do
        local master = self.master_apprentice_list_m[i]
        local apprentice = self.master_apprentice_list_a[i]

        local arp_m = master:ShiTuAchieveRedPoint(dmcfn < SocietyConfig.ACHIEVE_TIMES_MAX, dmpdfn < SocietyConfig.ACHIEVE_TIMES_MAX)
        local arp_a = apprentice:ShiTuAchieveRedPoint(dmcfn < SocietyConfig.ACHIEVE_TIMES_MAX, dmpdfn < SocietyConfig.ACHIEVE_TIMES_MAX)
        arp = arp or arp_m or arp_a
    end
    self.shitu_achieve_red_point.val = arp
end
