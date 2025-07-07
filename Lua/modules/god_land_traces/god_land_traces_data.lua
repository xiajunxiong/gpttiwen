
GodLandTracesData = GodLandTracesData or BaseClass()

function GodLandTracesData:__init()
    if GodLandTracesData.Instance ~= nil then
        Debuger.LogError("[GodLandTracesData] attempt to create singleton twice!")
        return
    end
    GodLandTracesData.Instance = self
    
    self:GodLandTracesConfig()
    self:GodLandTracesData()
end

function GodLandTracesData:GodLandTracesConfig()
    self.config = Config.god_land_traces_auto
    self.config_other = self.config.other[1]
    self.config_give_times = self.config.give_times[1]

    Cfg.Register("GodLandTracesCollectBySeq",Cfg.RealTimeSearch(Config.god_land_traces_auto.collect_group,"seq"))
end

function GodLandTracesData:GodLandTracesData()
    self.show_list = {}
    self.reward_list = {}
    self.send_list = {}
    for i = 1, GodLandTracesConfig.ITEM_EXCHANGE_MAX do
        local item_data = self.config_other["item_list_" .. i]
        self.show_list[i] = {
            item = Item.Init(item_data.item_id, 0, item_data.is_bind),
            item_guide = Item.Init(item_data.item_id, 0, item_data.is_bind),
            index = i,
            item_id = item_data.item_id,
            need_num = 0,
            flag = false,
        }
        table.insert(self.send_list, Item.Init(item_data.item_id, item_data.num, item_data.is_bind))
    end
    for i = 0, #self.config_other.item_list do
        local item_data = self.config_other.item_list[i]
        table.insert(self.reward_list, {
                item = Item.Init(item_data.item_id, item_data.num, item_data.is_bind),
                flag = false,
                index = i,
            }
        )
    end
    self.info_sm = SmartData.New{
        seq = 0,
        is_fetch_reward = 0,
        active_flag = 0,
    }
    self.info_sm_open = SmartData.New{
        is_open = 0,
        next_state_time = 0,
    }
end

function GodLandTracesData:SetTianDiDaoHenRoleInfo(protocol)
    local info = protocol.info
    local co = Cfg.GodLandTracesCollectBySeq(info.seq)
    for i = 1, GodLandTracesConfig.ITEM_EXCHANGE_MAX do
        local show_item = self.show_list[i]
        show_item.need_num = co["num_" .. i]
        show_item.flag = 1 == bit:_rshift(info.active_flag, show_item.index - 1) % 2
    end
    self.info_sm:Set(info)
end

function GodLandTracesData:SetTianDiDaoHenOpenInfo(protocol)
    self.info_sm_open:Set(protocol.info)
end

function GodLandTracesData:GetRewardList()
    local value = false
    if 0 == self.info_sm.is_fetch_reward then
        value = true
        for k, v in ipairs(self.show_list) do
            value = value and v.flag
        end
    end
    for k, v in ipairs(self.reward_list) do
        v.flag = value
    end
    return self.reward_list, value, 0 == self.info_sm.is_fetch_reward
end

function GodLandTracesData:GetRemindNum()
    local value1, value2 = false, false
    if 0 == self.info_sm.is_fetch_reward then
        value2 = true
        for k, v in ipairs(self.show_list) do
            if not v.flag and BagData.Instance:GetNumByItemId(v.item_id) >= v.need_num then
                value1 = true
                break
            end
            value2 = value2 and v.flag
        end
    end
    return (value1 or value2) and 1 or 0
end

function GodLandTracesData:GetSendItemList(item_id)
    local list, sel_item = {}
    for _, item in ipairs(self.send_list) do
        local have_num = BagData.Instance:GetNumByItemId(item.item_id)
        if have_num > 0 then
            if item_id == item.item_id then
                sel_item = item
            end
            item.num = have_num
            table.insert(list, item)
        end
    end
    return list, sel_item or list[1]
end

function GodLandTracesData:GetSendRemainTimes(role_infos, sel_role)
    local remain_times = self.config_give_times.give_times
    local times_group_id = self.config_give_times.times_group_id
    if role_infos[times_group_id] then
        local uid = sel_role and sel_role.info and sel_role.info.uid or 0
        remain_times = remain_times - (role_infos[times_group_id][uid] or 0)
    end
    return remain_times
end