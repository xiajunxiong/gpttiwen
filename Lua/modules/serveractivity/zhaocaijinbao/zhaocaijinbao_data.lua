ActZhaoCaiJinBaoData = ActZhaoCaiJinBaoData or BaseClass()

MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM = 15

MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM = 10

MAX_RA_ZHAOCAIJINBAO_BUY_NUM = 10

function ActZhaoCaiJinBaoData:__init()
	if ActZhaoCaiJinBaoData.Instance ~= nil then
		Debuger.LogError("[ActZhaoCaiJinBaoData] attempt to create singleton twice!")
		return
	end
    ActZhaoCaiJinBaoData.Instance = self
    self.zhaocai_smart = SmartData.New({draw_result = -1, storeFlush = false, flush = false, show_index = 1, skip_rolling = false, reward_flag = false})

    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoZhaoCaiJinBao,function ()
        self:initConfig()
    end,false)

end

function ActZhaoCaiJinBaoData:initConfig()
    self.show_reward_group_list = {}
    self.pool_config_list = {}
    for k, v in pairs(Config.zhaocaijinbao_auto.reward_group) do
        if v.is_show == 1 then
            self.show_reward_group_list[v.reward_group] = self.show_reward_group_list[v.reward_group] or {}
            local item_cfg = Item.GetConfig(v.reward_item.item_id)
            if item_cfg.show_type ~= ShowType.GiftBag then
                table.insert(self.show_reward_group_list[v.reward_group],item_cfg)
            else
                local item_list = BagData.Instance:GiftItemList(v.reward_item.item_id)
                for key, value in pairs(item_list or {}) do
                    local item_cfg = value:Config()
                    if item_cfg.item_type == ItemType.PetWeapon
                        or item_cfg.item_type == ItemType.Appearance
                        or item_cfg.item_type == ItemType.Skin
                        or item_cfg.item_type == ItemType.Fashion
                        or value:IsPet() then
                            table.insert(self.show_reward_group_list[v.reward_group], item_cfg)
                    end
                end
            end
            
        end
        self.pool_config_list[v.reward_group] = self.pool_config_list[v.reward_group] or {}
        self.pool_config_list[v.reward_group][v.seq] = v
        if not StringIsEmpty(v.gold_num) and v.gold_num > 0 then
            self.pool_config_list[v.reward_group][v.seq].reward_item = {item_id = HuoBi[CurrencyType.Gold], num = v.gold_num, is_bind = 1}
        end
    end
end

function ActZhaoCaiJinBaoData:__delete()
	ActZhaoCaiJinBaoData.Instance = nil
end

function ActZhaoCaiJinBaoData:SetZhaoCaiInfo(protocol)
    self.next_refresh_timestamp = protocol.next_refresh_timestamp
    self.pool = protocol.pool
    self.buy_list = protocol.buy_list
    self.chongzhi_list = protocol.chongzhi_list
    self.chongzhi_times = protocol.chongzhi_times
    self.history_chongzhi = protocol.history_chongzhi
    self.day_chongzhi = protocol.day_chongzhi
    self.total_info = TableCopy(protocol)
    if ViewMgr:IsOpen(ActZhaoCaiJinBaoStoreView) then
        self.zhaocai_smart:SetDirty("storeFlush")
    end
    if ViewMgr:IsOpen(ActZhaoCaiJinBaoView) then
        self.zhaocai_smart:SetDirty("flush")
    end
    self.zhaocai_smart:SetDirty("reward_flag")
    local _, section = next(ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ZHAOCAIJINBAO, Config.zhaocaijinbao_auto.basis_configuration))
    self.section = section
end

function ActZhaoCaiJinBaoData:SetDrawResult(protocol)
    if #protocol.seq_list == 1 then
        local draw_result = protocol.seq_list[1]
        for k,v in pairs(self.pool) do
            if v == draw_result then
                self.draw_result_item = Item.Create(self.pool_config_list[self.section.reward_group][v].reward_item)
                if self.zhaocai_smart.draw_result == k + 1 then
                    self.zhaocai_smart:SetDirty("draw_result")
                else
                    self.zhaocai_smart.draw_result = k + 1
                end
                break
            end
        end
    elseif #protocol.seq_list > 1 then
        self.draw_result_item_list = {}
        for k, v in pairs(protocol.seq_list) do
            for key, value in pairs(self.pool) do
                if v == value then
                    table.insert(self.draw_result_item_list, Item.Create(self.pool_config_list[self.section.reward_group][v].reward_item))
                end
            end
        end
    end
end

function ActZhaoCaiJinBaoData:GetDrawResultItemList()
    return self.draw_result_item_list or {}
end

function ActZhaoCaiJinBaoData:GetRefreshCost()
    return Config.zhaocaijinbao_auto.other[1].refresh_gold
end

function ActZhaoCaiJinBaoData:GetStarCount()
    return Item.GetNum(Config.zhaocaijinbao_auto.other[1].item_id), Item.GetConfig(Config.zhaocaijinbao_auto.other[1].item_id)
end

function ActZhaoCaiJinBaoData:GetShowList()
    local show_list = self.show_reward_group_list[self.section.reward_group] or {}
    return show_list
end

function ActZhaoCaiJinBaoData:GetPoolList()
    local pool_list = {}
    for k, v in pairs(self.pool or {}) do
        local config = self.pool_config_list[self.section.reward_group][v]
        table.insert(pool_list,Item.Create(config.reward_item))
    end
    return pool_list
end

function ActZhaoCaiJinBaoData:GetSectionPoolCfgList()
    local item_list = {}
    for k, v in pairs(self.pool_config_list[self.section.reward_group]) do
        table.insert(item_list,{item = Item.Create(v.reward_item), rate = string.format(Language.Common.Percent, string.format("%0.2f",v.show_rate * 100)) })
    end
    return item_list
end

function ActZhaoCaiJinBaoData:GetStoreInfoList()
    local store_info_list = {}
    for _, v in pairs(ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ZHAOCAIJINBAO,Config.zhaocaijinbao_auto.buy_configuration)) do
        -- local t = TableCopy(v)
        local t = {}
        t.type = v.type
        t.seq = v.seq
        t.buy_times = v.buy_times
        t.price_type = v.price_type
        t.buy_money = v.buy_money
        t.reward_item = v.reward_item
        t.section_start = v.section_start
        t.section_end = v.section_end
        t.limit_type = v.limit_type
        t.buy_count = self.buy_list[t.seq] or 0
        t.buy_max = t.buy_count >= t.buy_times
        table.insert(store_info_list,t)
    end
    return store_info_list
end

function ActZhaoCaiJinBaoData:GetRewardFlag()
    for _, v in pairs(ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ZHAOCAIJINBAO,Config.zhaocaijinbao_auto.top_up_reward)) do
        -- 累计充值
        local can_get_reward = 0
        if v.top_up_type == 1 then
            if self.history_chongzhi and self.day_chongzhi then
                -- 活动期间累计                                                      每日累计
                if v.get_type == 1 and self.history_chongzhi >= v.top_up_num or (v.get_type == 2 and self.day_chongzhi >= v.top_up_num) then
                    can_get_reward = 1
                end
            end
        -- 单笔充值
        elseif v.top_up_type == 0 and self.chongzhi_times and self.chongzhi_times[v.seq] and self.chongzhi_times[v.seq] > 0 then
            can_get_reward = 1
        end
        if can_get_reward > 0 and self.chongzhi_list and (self.chongzhi_list[v.seq] or 0) < v.get_times then
            return 1
        end
    end
    return 0
end

function ActZhaoCaiJinBaoData:GetRewardInfoList()
    local reward_info_list = {}
    for _, v in pairs(ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ZHAOCAIJINBAO,Config.zhaocaijinbao_auto.top_up_reward)) do
        local t = {}--TableCopy(v)
        t.type = v.type
        t.seq = v.seq
        t.top_up_type = v.top_up_type
        t.top_up_num = v.top_up_num
        t.reward_item = v.reward_item
        t.get_type = v.get_type
        t.get_times = v.get_times
        t.section_start = v.section_start
        t.section_end = v.section_end
        t.reward_list = {}
        for i = 0, #t.reward_item do
            table.insert(t.reward_list, Item.Create(t.reward_item[i]))
        end
        -- 累计充值
        if t.top_up_type == 1 then
            -- 活动期间累计                                                      每日累计
            if t.get_type == 1 and self.history_chongzhi >= t.top_up_num or (t.get_type == 2 and self.day_chongzhi >= t.top_up_num) then
                t.can_get_reward = 1
            else
                t.can_get_reward = 0
            end

        -- 单笔充值
        elseif t.top_up_type == 0 and self.chongzhi_times[t.seq] > 0 then
            t.can_get_reward = 1
        else
            t.can_get_reward = 0
        end
        t.has_get_reward = self.chongzhi_list[t.seq] or 0
        t.get_max = t.can_get_reward > 0 and t.has_get_reward >= t.get_times
        table.insert(reward_info_list, t)
    end
    return reward_info_list
end

function ActZhaoCaiJinBaoData:SetGetDrawResult(item)
    self.draw_result_item = item
end

function ActZhaoCaiJinBaoData:GetRecordList()
    if self.record_list == nil then
        return {}
    end
    local record_list = {}
    for key, value in pairs(self.record_list) do
        local item_cfg = Item.GetConfig(value.item_id)
        if item_cfg then
            value.item_cfg = item_cfg
            table.insert(record_list, value)
        end
    end
    return record_list
    -- return self.record_list
end

function ActZhaoCaiJinBaoData:SetRecordList(protocol)
    self.record_list = protocol.record_list
end