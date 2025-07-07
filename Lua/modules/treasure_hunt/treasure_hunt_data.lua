TreasureHuntData = TreasureHuntData or BaseClass()
TreasureHuntData.MaxHuntTypeNum = 6

HUNT_TYPE = {
    HUAN_LING = 1,
    XUN_QI = 2
}

-- 寻器下需做枚举区分
XUN_QI_TYPE = {
    HUN_QI = 1, --魂器
    SHENG_QI = 2, --圣器（永久屏蔽）
}

-- 寻器读下次免费抽奖时间的index，服务端说写死
XUN_QI_NEXT_TIME_INDEX = {
    HUN_QI = 7, --魂器
    SHENG_QI = 10, --圣器
}

TREASURE_HUNT_ITEM_TYPE = {
    Item = 0,
    Pet = 1,
    Partner = 2,
}

HuntNotPet = {
    [5] = true,
}

function TreasureHuntData:__init()
    if TreasureHuntData.Instance ~= nil then
        Debuger.LogError("[TreasureHuntData] attempt to create singleton twice===>AchieveData:__init")
        return 
    end
    TreasureHuntData.Instance = self

    self.index = 0
    self.ran_index = {}
    self.save_index = {}
    self.cur_count = 0
    self.treasure_hunt_count = 0
    self.smart_data = SmartData.New({
        is_change = false,
        is_get = false,
        node_item_list = {},
        cur_treasure_id = 1,
        flush_effect = false,
        FlushBtn = false,                --主界面寻宝按钮刷新监听key
        sended_req = false,
        is_guide_draw = false,
        delay_flush = false,
        FreeRemind = false,             --主界面寻宝按钮免费次数红点
        hunt_type = 0,                  --0唤灵 1寻器
    })
    self.exchange_smart = SmartData.New({
        comsume_type1 = -1,             --消耗材料所在队列类型（0宠物列表，1宠物仓库）
        comsume_type2 = -1,
        comsume_type_index1 = -1,       --置换宠物材料序号（对应队列中的index）
        comsume_type_index2 = -1,
        comsume_index1 = -1,            --消耗材料在对应消耗组的序号
        comsume_index2 = -1,           
        comsume_pet_id1 = 0,            --消耗材料的宠物id
        comsume_pet_id2 = 0,

        compose_index = 0,              --置换目标宠物在对应获得组中的序号
        compose_pet_id = 0,             --合成选择的宠物id
        clicked_index = 0,              --宠物置换点击的格子（材料1、材料2、合成目标）
    })
    self.pets_bless_info = SmartData.New({xun_bao_bless_pet_id_list = {}, cur_pray_index = 1, cur_pray_id = 0})
    self.exchange_request = SmartData.New({sended_req = -1})
    self.treasure_group_times = SmartData.New()
    self.xi_you_floor_curr_times = SmartData.New()
    self.first_draw_flag = SmartData.New()
    self.second_draw_flag = SmartData.New()
    self.is_detail_opened = false

    self.all_config = Config.treasure_hunt_auto
    self.other_info = self.all_config.other[1]
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoTreasureHunt,function ()
        self:initConfigByGroup()
    end, false)

    self.next_id_open_time_info = {timeStemp = 0}
    self.next_id_end_time_info_list = {}
    self.noticed_list = {}
    self.auto_buy_data = {}
    self.free_timestamp_list = {}
    self.noOneHasOpen = true
    self.not_show_anim = false
    self.last_index = -1
    self.opened_xunqi_id = {}
end

--按照奖励组初始化数据
function TreasureHuntData:initConfigByGroup()
    self.all_config = Config.treasure_hunt_auto
    self.other_info = self.all_config.other[1]
    self.showConfigGroup = {}
    for k,v in pairs(self.all_config.rate_show) do
        self.showConfigGroup[v.reward_group] = self.showConfigGroup[v.reward_group] or {}
        local item_list = {}
        for i,j in pairs(v.reward_id) do
            if Item.GetBigType(j.item_id) == ITEM_BIG_TYPE.PET then
                local vo = Item.InitData(j.item_id, j.num, j.is_bind)
                item_list[i + 1] = Pet.New(vo)
            else
                item_list[i + 1] = Item.Init(j.item_id, j.num, j.is_bind)
            end
        end
        self.showConfigGroup[v.reward_group][v.seq + 1] = {cfg = v, item_list = item_list}
        -- self.showConfigGroup[v.reward_group][v.seq] = v
    end
    self.level_section_group = {}
    for k,v in pairs(self.all_config.level_section) do
        self.level_section_group[v.treasure_id] = self.level_section_group[v.treasure_id] or {}
        table.insert(self.level_section_group[v.treasure_id], v)
    end
    self.treasure_id_config = {}
    self.treasure_cast_item_id = {}
    for k,v in pairs(self.all_config.treasure) do
        self.treasure_id_config[v.treasure_id] = v
        if v.is_limit ~= 1 then
            self.treasure_cast_item_id[v.treasure_type] = self.treasure_cast_item_id[v.treasure_type] or {}
            self.treasure_cast_item_id[v.treasure_type][v.once_item_id] = 5
            self.treasure_cast_item_id[v.treasure_type][v.five_item_id] = 1
        end
    end

    local t_mat = {}
    local index_t = {}
    for i,v in ipairs(self.all_config.dis_material) do
        t_mat[v.material_id] = t_mat[v.material_id] or {}
        index_t[v.material_id] = (index_t[v.material_id] or 0) + 1
        t_mat[v.material_id].strengthen_num = v.strengthen_num
        t_mat[v.material_id][v.pet_id] = { strengthen_num = v.strengthen_num, seq = index_t[v.material_id] }
    end
    self.all_config.dis_material = t_mat
    index_t = {}
    local t_acq = {}
    for i,v in ipairs(self.all_config.dis_acquire) do
        t_acq[v.get_id] = t_acq[v.get_id] or {}
        index_t[v.get_id] = (index_t[v.get_id] or 0) + 1
        t_acq[v.get_id].strengthen_num = v.strengthen_num
        t_acq[v.get_id][v.pet_id] = {strengthen_num = v.strengthen_num, seq = index_t[v.get_id], pet_id = v.pet_id}
    end
    self.all_config.dis_acquire = t_acq
    -- 整理寻器奖励展示表
    local pet_w_t = {}
    for k,v in pairs(self.all_config.pet_weapon_hunt) do
        pet_w_t[v.for_treasure_id] = pet_w_t[v.for_treasure_id] or {}
        table.insert(pet_w_t[v.for_treasure_id], v)
    end
    self.all_config.pet_weapon_hunt = pet_w_t
    -- 整理寻器界面模型展示表
    local pet_d_t = {}
    for k,v in pairs(self.all_config.show_res) do
        pet_d_t[v.treasure_id] = pet_d_t[v.treasure_id] or {}
        table.insert(pet_d_t[v.treasure_id], v)
    end
    self.all_config.show_res = pet_d_t
end

function TreasureHuntData:__delete()
    if TreasureHuntData.Instance == self then
        TreasureHuntData.Instance = nil
    end
    self.noticed_list = {}
    self.next_id_open_time_info = {timeStemp = 0}
    self.next_id_end_time_info_list = {}
end

function TreasureHuntData:GetPetCount()
    local count = 0
    for k, v in pairs(self.smart_data.node_item_list)do
        if v.type == 1 then 
            count = count + 1
        end
    end
    return count
end

function TreasureHuntData:CheckHuntNum(treasure_type)
    if not FunOpen.Instance:GetFunIsOpened(Mod.TreasureHunt.Main) then
        return 0
    elseif treasure_type == HUNT_TYPE.XUN_QI and not self:GetXunQiIsOpen() then
        return 0
    end
    local server_time = TimeCtrl.Instance:GetServerTime()
    -- 圣器是否开启
    local shengqi_is_open = true
    for k, v in pairs(self.free_timestamp_list) do
        if v > 0 then
            local treasure_id_config = self:GetTreasureHuntConfig(k)
            if treasure_id_config.treasure_type == treasure_type then
                if treasure_type == HUNT_TYPE.XUN_QI then
                    -- 寻器的圣器需要特殊判断，功能可能没开启
                    local xunqi_type = self:XunQiType(treasure_id_config.treasure_id)
                    if xunqi_type == XUN_QI_TYPE.SHENG_QI then
                        if self:GetXunQiShengQiIsOpen(treasure_id_config.treasure_id) then
                            if v <= server_time then
                                return 1
                            end
                        else
                            shengqi_is_open = false
                        end
                    else
                        if v <= server_time then
                            return 1
                        end
                    end
                else
                    if v <= server_time then
                        return 1
                    end
                end
            end
        end
    end
    for k, v in pairs(self.treasure_cast_item_id[treasure_type] or {}) do
        if k == HuoBi[CurrencyType.ShengQiYaoShi] or k == HuoBi[CurrencyType.ShengQiYaoShis] then
            -- 寻器的圣器需要特殊判断，功能可能没开启
            if shengqi_is_open then
                if Item.GetNum(k) >= v then
                    return 1
                end
            end
        else
            if Item.GetNum(k) >= v then
                return 1
            end
        end
    end
    return 0
end

function TreasureHuntData:SetOpenInfo(open_info)
    self.open_info = open_info
end

function TreasureHuntData:SetGroupTimes(protocol)
    self.treasure_group_times:Set(protocol.treasure_group_times)
    self.xi_you_floor_curr_times:Set(protocol.xi_you_floor_curr_times)
end
function TreasureHuntData:SetFreeTimes(protocol)
    local server_time = TimeCtrl.Instance:GetServerTime()
    local next_free_time = -1
    for k,v in pairs(protocol) do
        local treasure_id_config = self.treasure_id_config[k]
        if treasure_id_config and treasure_id_config.time_treasure_free and treasure_id_config.time_treasure_free > 0 then
            if treasure_id_config.treasure_type == HUNT_TYPE.HUAN_LING
            or k == XUN_QI_NEXT_TIME_INDEX.HUN_QI or k == XUN_QI_NEXT_TIME_INDEX.SHENG_QI then
                self.free_timestamp_list[k] = v ~= 0 and v or treasure_id_config.time_treasure_free
                if next_free_time < 0 or next_free_time > v - server_time then
                    next_free_time = v - server_time
                end
                -- LogError("SetFreeTimes",k,treasure_id_config.time_treasure_free,self.free_timestamp_list[k], next_free_time)
            end
            
        else
            self.free_timestamp_list[k] = 0
        end
    end
    if self.free_remind_timer ~= nil then
        TimeHelper:CancelTimer(self.free_remind_timer)
    end
    -- LogError("next_free_time",next_free_time)
    self.free_remind_timer = TimeHelper:AddDelayTimer(function ()
        -- LogError("SetDirty FreeRemind")
        self.smart_data:SetDirty("FreeRemind")
    end, next_free_time)
    self.smart_data:SetDirty("FreeRemind")
end

function TreasureHuntData:GetFreeTimeStamp(treasure_id, treasure_type)
    if treasure_type == HUNT_TYPE.HUAN_LING then
        return self.free_timestamp_list[treasure_id]
    elseif treasure_type == HUNT_TYPE.XUN_QI then
        local xunqi_type = self:XunQiType(treasure_id)
        if xunqi_type == XUN_QI_TYPE.HUN_QI then
            return self.free_timestamp_list[XUN_QI_NEXT_TIME_INDEX.HUN_QI]
        elseif xunqi_type == XUN_QI_TYPE.SHENG_QI then
            return self.free_timestamp_list[XUN_QI_NEXT_TIME_INDEX.SHENG_QI]
        end
    end
end

function TreasureHuntData:CheckItemNum(item_id)
    local item = Item.Create({item_id = item_id})
    return item:Num()
end

function TreasureHuntData:GetShowRateList()
    -- return {[1] = {name = "施工中", cfg = {name_id = 1, rate = 1}, item_list = {}}}
    local groupId = self:getRewardGroupId()
    return self.showConfigGroup[groupId]
end

function TreasureHuntData:getRewardGroupId()
    local roleLevel = RoleData.Instance:GetRoleLevel()
    for k,v in pairs(self.level_section_group[self.smart_data.cur_treasure_id]) do
        if roleLevel >= v.level_min and roleLevel <= v.level_max then
            return v.reward_group
        end
    end
    return 1
end

function TreasureHuntData:FlushNeedItem()
    self.need_item_list = {}
    for k,v in pairs(self.open_info) do
        if v.is_open == 1 then
            local treasure_id_config = self.treasure_id_config[v.treasure_id]
            self.need_item_list[treasure_id_config.once_item_id] = 1
            self.need_item_list[treasure_id_config.five_item_id] = 1
        end
    end
end

function TreasureHuntData:GetPartnerShowNameText()
    local reward_group = nil
    local roleLevel = RoleData.Instance:GetRoleLevel()
    for k,v in pairs(self.level_section_group[5]) do --5 对应伙伴召唤寻宝
        if roleLevel >= v.level_min and roleLevel <= v.level_max then
            reward_group = self.showConfigGroup[v.reward_group]
            break
        end
    end
    if reward_group == nil then
        return nil
    end
    local show_name = ""
    local name_count = 0
    for k,v in pairs(reward_group) do
        for m,n in pairs(v.item_list) do
            show_name = show_name .." "..n:QuaName(true)
            if n:Color() >= 5 then
                show_name = string.format(Language.Common.Size, 26, show_name)
            end
            name_count = name_count + 1
            if name_count == 2 then
                show_name = show_name .. "\n"
            elseif name_count == 5 then
                return show_name
            end
        end
    end
    return show_name
end

function TreasureHuntData:GetOpenedTreasureID()
    if self.open_info == nil then
        return {}, false
    end
    self.noOneHasOpen = true
    self.next_id_open_time_info = {timeStemp = 0}
    self.next_id_end_time_info_list = {}
    local corner = false

    self.open_list = {}
    self.sort_id = TreasureHuntData.MaxHuntTypeNum + 1
    self.prefer_treasure_id = 0
    self:ResetXunQiId()
    local cur_is_open = false
    local server_open_day = TimeCtrl.Instance:GetCurOpenServerDay()
    local week_num = ActivityData.Instance:GetWeekNum()
    week_num = week_num == 7 and 0 or week_num
    local server_time = TimeCtrl.Instance:GetServerTime()
    local server_time_t = TimeCtrl.Instance:GetServerDate()
    -- LogError("server_time_t",server_time_t)
    for k,v in pairs(self.open_info) do
        local cfg = self.treasure_id_config[v.treasure_id]
        local open_by_level = true
        if RoleData.Instance:GetRoleLevel() < cfg.display_level then
            open_by_level = false
        end
        if v.is_open == 1 and open_by_level then
            --限时寻宝 按照运营配置的固定时间戳开启和关闭
            if cfg.open_type == 1 and server_open_day >= v.open_day then
                if v.end_time > server_time and v.start_time < server_time then
                    self.open_list[cfg.sort] = v
                    
                    if self.sort_id > cfg.sort then
                        self.sort_id = cfg.sort
                        self.prefer_treasure_id = v.treasure_id
                    end
                    if v.treasure_id == self.smart_data.cur_treasure_id then
                        self.prefer_treasure_id = self.smart_data.cur_treasure_id
                    end
                    if not corner then
                        corner = cfg.is_limit == 1
                    end
                    if cfg.treasure_type == HUNT_TYPE.XUN_QI then
                        local xunqi_type = self:XunQiType(v.treasure_id)
                        self.opened_xunqi_id[xunqi_type] = v.treasure_id
                    end
                    self.noOneHasOpen = false
                elseif v.start_time > server_time then
                    local nextOpenTimeStemp = v.start_time - server_time
                    if self.next_id_open_time_info.timeStemp >= nextOpenTimeStemp or self.next_id_open_time_info.timeStemp == 0 then
                        self.next_id_open_time_info.timeStemp =  nextOpenTimeStemp
                        self.next_id_open_time_info.id = v.treasure_id
                        self.next_id_open_time_info.chat_send = false
                    end
                end
                table.insert(self.next_id_end_time_info_list, {timeStemp = math.floor(v.end_time - server_time), id = v.treasure_id})
            end

            -- 普通寻宝，按照固定天数的固定时间段开启
            if cfg.open_type == 2 and server_open_day >= v.open_day and v.week_open_flag[32 - week_num] == 1 then
                local start_time = math.floor(tonumber(v.day_begin_time) / 100)
                local start_min = tonumber(v.day_begin_time) - start_time * 100

                local end_time = math.floor(tonumber(v.day_end_time) / 100)
                local end_min = tonumber(v.day_end_time) - end_time * 100

                local is_start = server_time_t.hour == start_time and server_time_t.min >= start_min or server_time_t.hour > start_time
                local not_end = server_time_t.hour == end_time and server_time_t.min < end_min or server_time_t.hour < end_time
                if is_start and not_end or cfg.treasure_type == HUNT_TYPE.XUN_QI then
                    local end_time_stemp = os.time({year = server_time_t.year, month = server_time_t.month, day = server_time_t.day, hour = end_time,min = end_min,sec = 0})
                    v.left_time = end_time_stemp - server_time
                    table.insert(self.next_id_end_time_info_list, {timeStemp = math.floor(v.left_time), id = v.treasure_id})
                    self.open_list[cfg.sort] = v
                    
                    if self.sort_id > cfg.sort then
                        self.sort_id = cfg.sort
                        self.prefer_treasure_id = v.treasure_id
                    end
                    if v.treasure_id == self.smart_data.cur_treasure_id then
                        self.prefer_treasure_id = self.smart_data.cur_treasure_id
                    end
                    if not corner then
                        corner = cfg.is_limit == 1
                    end
                    if cfg.treasure_type == HUNT_TYPE.XUN_QI then
                        local xunqi_type = self:XunQiType(v.treasure_id)
                        self.opened_xunqi_id[xunqi_type] = v.treasure_id
                    end
                    self.noOneHasOpen = false
                elseif server_time_t.hour == start_time and server_time_t.min < start_min or server_time_t.hour < start_time then
                    local nextOpenTimeStemp = os.time({year = server_time_t.year, month = server_time_t.month, day = server_time_t.day, hour = start_time, min = start_min,sec = 0}) - server_time
                    -- LogError("open_type 2 nextOpenTimeStemp",nextOpenTimeStemp)
                    if self.next_id_open_time_info.timeStemp >= nextOpenTimeStemp or self.next_id_open_time_info.timeStemp == 0 then
                        self.next_id_open_time_info.timeStemp = nextOpenTimeStemp
                        self.next_id_open_time_info.id = v.treasure_id
                        self.next_id_open_time_info.chat_send = true
                    end
                end
            end
        end
    end
    if not ViewMgr:IsOpen(TreasureHuntView) and not ViewMgr:IsOpen(TreasureHuntDetailView) then
        self:SetCurSelectId(self.prefer_treasure_id ~= 0 and self.prefer_treasure_id or 1)
    end
    self:SetOpenAndEndCountDown()
    return self.open_list, corner
end

-- 根据treasure_id来返回寻器类型
function TreasureHuntData:XunQiType(treasure_id)
    local treasure_id = treasure_id or self.smart_data.cur_treasure_id
    return self.treasure_id_config[treasure_id] and self.treasure_id_config[treasure_id].xunqi_type or -1
end

-- 判断是否魂器
function TreasureHuntData:IsHunQi(treasure_id)
    local treasure_id = treasure_id or self.smart_data.cur_treasure_id
    return self:XunQiType(treasure_id) == XUN_QI_TYPE.HUN_QI
end

-- 判断是否圣器
function TreasureHuntData:IsShengQi(treasure_id)
    local treasure_id = treasure_id or self.smart_data.cur_treasure_id
    return self:XunQiType(treasure_id) == XUN_QI_TYPE.SHENG_QI
end

-- 返回当前寻器展示的表
function TreasureHuntData:XunQiShowMod(treasure_id)
    local treasure_id = treasure_id or self.smart_data.cur_treasure_id
    return self.all_config.show_res[treasure_id] or {}
end

function TreasureHuntData:GetAutoSelectHuanLing(open_list)
    if self.auto_select ~= nil then
        for k,v in pairs(open_list) do
            if v.treasure_id == self.auto_select then
                self.auto_select = nil
                return k
            end
        end
    end
    return 1
end

function TreasureHuntData:GetAutoSelectXunQi(open_list)
    if self.auto_xunqi_type ~= nil then
        for k,v in pairs(open_list) do
            local xunqi_type = self:XunQiType(v.treasure_id)
            if self.auto_xunqi_type == xunqi_type then
                self.auto_xunqi_type = nil
                return k
            end
        end
    end
    return 1
end

function TreasureHuntData:SetCurSelectId(id)
    self.smart_data.cur_treasure_id = id
end

function TreasureHuntData:ResetCurSelectId()
    self:SetCurSelectId(self.prefer_treasure_id ~= 0 and self.prefer_treasure_id or 1)
end

function TreasureHuntData:GetCurSelectCfg()
    return self:GetTreasureHuntConfig(self.smart_data.cur_treasure_id)
end

function TreasureHuntData:GetTreasureHuntConfig(treasure_id)
    return self.treasure_id_config[treasure_id]
end

function TreasureHuntData:SetOpenAndEndCountDown()
    --传闻倒计时开启
    -- LogError("1 next_id_open_time_info",self.next_id_open_time_info.timeStemp)
    self.next_id_open_time_info.timeStemp = math.floor(self.next_id_open_time_info.timeStemp)
    -- LogError("2 next_id_open_time_info",self.next_id_open_time_info.timeStemp)
    if self.next_id_open_time_info.timeStemp > 0 then
        if self.nextOpenCountDown ~= nil then
            TimeHelper:CancelTimer(self.nextOpenCountDown)
        end
        self.nextOpenCountDown = TimeHelper:AddDelayTimer(function ()
            self.noOneHasOpen = false
            if self.next_id_open_time_info.chat_send then
                ChatCtrl.Instance:ChannelChatCustom({rumor_th = {state = ActStatusType.Open}})
            end
            self.smart_data.FlushBtn = not self.smart_data.FlushBtn
            self:GetOpenedTreasureID()
        end,self.next_id_open_time_info.timeStemp + 10)
    end
    --传闻倒计时关闭提醒
    --每个id独立计时
    for i,v in ipairs(self.next_id_end_time_info_list) do
        if self["nextEndCountDown"..tostring(v.id)] ~= nil then
            TimeHelper:CancelTimer(self["nextEndCountDown"..tostring(v.id)])
            self["nextEndCountDown"..tostring(v.id)] = nil
        end

        if v.timeStemp > 600 then
            self["nextEndCountDown"..tostring(v.id)] = TimeHelper:AddDelayTimer(function ()
                ChatCtrl.Instance:ChannelChatCustom({rumor_th = {state = ActStatusType.Close, sec = 600}})
                self.noticed_list[v.id] = true
                self:GetOpenedTreasureID()
            end, v.timeStemp - 600)
            TimeHelper:CancelTimer(self["btnFlushCountDown"..tostring(v.id)])
            self["btnFlushCountDown"..tostring(v.id)] = TimeHelper:AddDelayTimer(function ()
                self.smart_data.FlushBtn = not self.smart_data.FlushBtn
            end, v.timeStemp + 10)
        elseif self.noticed_list[v.id] == nil and v.timeStemp >= 0 then
            if self["btnFlushCountDown"..tostring(v.id)] == nil then
                self["btnFlushCountDown"..tostring(v.id)] = TimeHelper:AddDelayTimer(function ()
                    self.smart_data.FlushBtn = not self.smart_data.FlushBtn
                end, v.timeStemp + 10)
                ChatCtrl.Instance:ChannelChatCustom({rumor_th = {state = ActStatusType.Close, sec = v.timeStemp}})
            end
        end
    end
end

function TreasureHuntData:CheckRumorOpen()
    TimeHelper:CancelTimer(self.rumor_end_timer)
    if self.open_info == nil then return end
    if TimeHelper.GetServerTime() < 9999 then
        self.rumor_end_timer = TimeHelper:AddDelayTimer(function ()
            self:CheckRumorOpen()
        end, 1)
        return
    end
    local rumor = false
    for k,v in pairs(self.open_info) do
        if v.is_open == 1 and v.end_time > TimeHelper.GetServerTime() then
            rumor = true
        end
    end
    if rumor then
        self.rumor_end_timer = TimeHelper:AddDelayTimer(function ()
            ChatCtrl.Instance:ChannelChatCustom({rumor_th = {state = ActStatusType.Open}})
            self:CheckRumorEnd()
        end, 3)
    end
end

function TreasureHuntData:CheckRumorEnd()
    TimeHelper:CancelTimer(self.rumor_end_timer)
    if self.open_info == nil then return end
    local min_end
    for k, v in pairs(self.open_info) do
        local end_time = v.end_time - 600
        if v.is_open == 1 and end_time > TimeHelper.GetServerTime() then
            min_end = min_end or end_time
            min_end = end_time < min_end and end_time or min_end
        end
    end
    if min_end then
        self.rumor_end_timer = TimeHelper:AddDelayTimer(function ()
            ChatCtrl.Instance:ChannelChatCustom({rumor_th = {state = ActStatusType.Close}})
            self:CheckRumorEnd()
        end, min_end - TimeHelper.GetServerTime())
    end
end

function TreasureHuntData:GetBaoDiSpecialPet()
    local cfg = self.treasure_id_config[self.smart_data.cur_treasure_id]
    if cfg.is_num_rare > 0 and self.xi_you_floor_curr_times[self.smart_data.cur_treasure_id] >= cfg.is_num_rare then
        return true
    end
    return false
end

function TreasureHuntData:GetLeftTimes()
    local cfg = self.treasure_id_config[self.smart_data.cur_treasure_id]
    if cfg.can_cumulative_bao_di ~= 0 then
        if self.treasure_group_times[cfg.group_id] ~= nil then
            return cfg.cumulative_bao_di_times - self.treasure_group_times[cfg.group_id] <= 0 and 1 or cfg.cumulative_bao_di_times - self.treasure_group_times[cfg.group_id]
        end
    end
end

function TreasureHuntData:GetGroupDrawTimes(treasure_id)
    treasure_id = treasure_id or self.smart_data.cur_treasure_id
    local cfg = self.treasure_id_config[treasure_id]
    --LogError("treasureId",treasure_id, cfg)
    if cfg.can_cumulative_bao_di ~= 0 then
        if self.treasure_group_times[cfg.group_id] ~= nil then
            return self.treasure_group_times[cfg.group_id]
        end
    end
end

function TreasureHuntData:GetCfgLeftTimes()
    local cfg = self.treasure_id_config[self.smart_data.cur_treasure_id]
    if cfg.can_cumulative_bao_di ~= 0 then
        return cfg.cumulative_bao_di_times
    end
end

function TreasureHuntData:GetXunQiPreviewList(treasure_id)
    if not treasure_id then
        local xunqi_type = self:XunQiType()
        treasure_id = treasure_id or self:GetCurOpenXunQiId(xunqi_type)
    end
    
    local show_list = {}
    local temp_list = self.all_config.pet_weapon_hunt[treasure_id] or {}
    for k,v in pairs(temp_list) do
        table.insert(show_list, Item.Init(v.show_item))
    end
    return show_list
end

function TreasureHuntData:ResetXunQiId()
    for k, v in pairs(XUN_QI_TYPE) do
        self.opened_xunqi_id[v] = -1
    end
end

-- 当前开放的寻器treasure_id
function TreasureHuntData:GetCurOpenXunQiId(xunqi_type)
    return self.opened_xunqi_id[xunqi_type] or -1
end

-- 当前开放的魂器treasure_id
function TreasureHuntData:GetCurOpenHunQiId()
    return self:GetCurOpenXunQiId(XUN_QI_TYPE.HUN_QI)
end

-- 当前开放的圣器treasure_id
function TreasureHuntData:GetCurOpenShengQiId()
    return self:GetCurOpenXunQiId(XUN_QI_TYPE.SHENG_QI)
end

function TreasureHuntData:GetXunQiShowModelId()
    return self.all_config.show_res
end

function TreasureHuntData:SetFirstDrawFlag(protocol)
    self.first_draw_flag:Set(protocol)
end

function TreasureHuntData:SetSecondDrawFlag(protocol)
    self.second_draw_flag:Set(protocol)
end


function TreasureHuntData:GetFirstDrawFlag(index)
    if self.first_draw_flag:Length() == 0 then
        return true
    end
    return self.first_draw_flag[self.first_draw_flag:Length() - index] == 1
end

function TreasureHuntData:GetSecondDrawFlag(index)
    if self.second_draw_flag:Length() == 0 then
        return true
    end
    return self.second_draw_flag[self.second_draw_flag:Length() - index] == 1
end

function TreasureHuntData:GetXunQiIsOpen()
    local roleLevel = RoleData.Instance:GetRoleLevel()
    for k, v in pairs(XUN_QI_TYPE) do
        local treasure_id = self:GetCurOpenXunQiId(v)
        local xun_qi_config = self:GetTreasureHuntConfig(treasure_id)
        if not TableIsEmpty(xun_qi_config) and xun_qi_config.displace_level then
            if v == XUN_QI_TYPE.SHENG_QI then
                -- 圣器特殊判断
                if self:GetXunQiShengQiIsOpen(treasure_id) then
                    return true
                end
            else
                if RoleLevel() >= xun_qi_config.displace_level then
                    return true
                end
            end
        end
    end
    return false
end

-- 判断寻器圣器是否开启（永久屏蔽）
function TreasureHuntData:GetXunQiShengQiIsOpen(treasure_id)
    -- local xunqi_type = self:XunQiType(treasure_id)
    -- if xunqi_type ~= XUN_QI_TYPE.SHENG_QI then
    --     return false
    -- end
    -- local xun_qi_config = self:GetTreasureHuntConfig(treasure_id)
    -- if TableIsEmpty(xun_qi_config) then
    --     return false
    -- end
    -- if ShengQiData.Instance:IsOpen() and RoleLevel() >= xun_qi_config.displace_level then
    --     return true
    -- end
    return false
end

-- 判断寻器圣器是否开启（外部调用）
function TreasureHuntData:JudgeXunQiShengQiIsOpen()
    return self:GetXunQiShengQiIsOpen(self:GetCurOpenShengQiId())
end

-- 返回最早结束的活动
function TreasureHuntData:JumpActivity(treasure_id)
    treasure_id = treasure_id or self.smart_data.cur_treasure_id
    if not self.jump_act_cfg then
        self.jump_act_cfg = {}
        for k, v in pairs(Config.treasure_hunt_auto.jump_activity) do
            if not self.jump_act_cfg[v.treasure_id] then
                self.jump_act_cfg[v.treasure_id] = {}
            end
            table.insert(self.jump_act_cfg[v.treasure_id], v)
        end
    end
    local act
    for k, v in pairs(self.jump_act_cfg[treasure_id]) do
        if ActivityData.IsOpen(v.activity_id) and FunOpen.Instance:IsFunOpen(v.jump_interface) then
            -- 这里还需要判断活动是开服第几天开的，然后确认区间是否正确
            local act_open_day = TimeCtrl.Instance:GetActOpenServerDay(v.activity_id)
            act_open_day = act_open_day <= 0 and 1 or act_open_day
            if (act_open_day >= v.section_start and act_open_day <= v.section_end)
            or (act_open_day >= v.section_start and v.section_end == 0) then
                if not act then
                    act = v
                else
                    local a_end_time = ActivityData.GetSwitchStampTime(act.activity_id)
                    local b_end_time = ActivityData.GetSwitchStampTime(v.activity_id)
                    if a_end_time > b_end_time then
                        act = v
                    end
                end
            end
        end
    end
    return act
end

function TreasureHuntData:NoOneHasOpen()
    return self.noOneHasOpen
end

function TreasureHuntData:GetExchangeAcqList(get_id)
    local t = {}

    for k,v in pairs(self.all_config.dis_acquire[get_id]) do
        if type(v) == "table" and v.pet_id ~= self.exchange_smart.comsume_pet_id1 and v.pet_id ~= self.exchange_smart.comsume_pet_id2 then
            table.insert(t, v)
        end
    end
    return t
end

function TreasureHuntData:ResetExchangeSmart()
    self.exchange_smart:Set({
        comsume_type1 = -1,             --消耗材料所在队列类型（0宠物列表，1宠物仓库）
        comsume_type2 = -1,
        comsume_type_index1 = -1,       --置换宠物材料序号（对应队列中的index）
        comsume_type_index2 = -1,
        comsume_index1 = -1,            --消耗材料在对应消耗组的序号
        comsume_index2 = -1,           
        comsume_pet_id1 = 0,            --消耗材料的宠物id
        comsume_pet_id2 = 0,

        compose_index = 0,              --置换目标宠物在对应获得组中的序号
        compose_pet_id = 0,             --合成选择的宠物id
        clicked_index = 0,
    })
end

function TreasureHuntData:SetBlessInfo(protocol)
    self.pets_bless_info.xun_bao_bless_pet_id_list = protocol.xun_bao_bless_pet_id_list
end

function TreasureHuntData:GetBlessInfo()
    return self.pets_bless_info.xun_bao_bless_pet_id_list
end

function TreasureHuntData.BlessRate(bless_type)
    for k,v in pairs(Config.treasure_hunt_auto.bless_rate) do
        if v.reward_type == bless_type then
            return v.rate_up
        end
    end
end