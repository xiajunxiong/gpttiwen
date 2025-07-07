FavorData = FavorData or BaseClass()

function FavorData:__init()
    if FavorData.Instance ~= nil then
        Debuger.LogError("[FavorData] attempt to create singleton twice!")
        return
    end
    FavorData.Instance = self
    self.favor_data_change=SmartData.New({val = false,is_show=false})
    self.favor_gift_select_data = SmartData.New()

    self.favor_data = {}
    self.old_val=nil
    self:InItConfig()

    self.npc_seq = nil
    self.npc_config = nil
end


function  FavorData:SetNpcFavorInfo(protocol)
     self.favor_data.info = protocol.info
     self.favor_data_change:SetDirty("val")
end

function FavorData:SetNpcFavorItemInfo(protocol)
    if self.old_val ~= nil and self.old_val ~= protocol.favor_item.favor_value then
        local old_level = self:GetNpcFavorLevel(self.old_val)
        local cur_level = self:GetNpcFavorLevel(protocol.favor_item.favor_value)
        if old_level == cur_level then
            if ViewMgr:IsOpen(FavorNpcUpView) ~= true then
                ViewMgr:OpenView(FavorNpcUpView, {old_val = self.old_val})
            end
        else
            if ViewMgr:IsOpen(FavorNpcLevelUpView) ~= true then
                ViewMgr:OpenView(FavorNpcLevelUpView, {old_val = self.old_val})
            end
        end
    end
    self.favor_data.info.favor_list[protocol.favor_index + 1] = protocol.favor_item
    self.favor_data_change:SetDirty("val")
end


function  FavorData:SetNpcFavorCommonInfo(protocol)
    self.favor_data.info.action_value = protocol.action_value
    self.favor_data.info.next_add_action_value_time = protocol.next_add_action_value_time
    self.favor_data.info.total_gift_times = protocol.total_gift_times
    self.favor_data.info.zone_fetch_flag = protocol.zone_fetch_flag
    self.favor_data_change:SetDirty("val")
end


function FavorData:InItConfig()
    Cfg.Register("NpcFavor", Cfg.CacheGet(Config.npc_mood_auto.mood_list, "npc_seq"))
end

function FavorData:GetNpcFavorBySeq()
    if not self.npc_list_cfg then
        self.npc_list_cfg = {}
        for _, v in pairs(Config.npc_mood_auto.mood_list) do
            self.npc_list_cfg[v.seq] = v
        end
    end
    return self.npc_list_cfg
end

function FavorData:SetCurNpcData(npc_seq)
    self.npc_seq=npc_seq
    self.npc_config=self:GetFavorNpcConfig(npc_seq)
end

--好感npc配置
function FavorData:GetFavorNpcConfig(npc_seq)
    return Cfg.NpcFavor(npc_seq,false) 
end

function FavorData:GetFavorShow(npc_seq)
    local cfg=self:GetFavorNpcConfig(npc_seq)
    local role_level=RoleData.Instance:GetRoleLevel()
    if cfg ~= nil and role_level >= cfg.need_level then
        return true
    end
    return false
end

function FavorData:GetCurFavorNpcConfig()
    return self.npc_config
end

--npc好感度信息
function FavorData:GetFavorInfo(cfg)
    if cfg == nil then
        cfg = self.npc_config
    end
    if self.favor_data.info then
        return self.favor_data.info.favor_list[cfg.seq + 1]
    end
    return {}
end


--好感等级
function FavorData:GetNpcFavorLevel(val,cfg)
    if cfg ==nil then
        cfg = self.npc_config
    end
    if cfg and self.favor_data.info then
        if val == nil then
            val = self.favor_data.info.favor_list[cfg.seq + 1].favor_value
        end
        if val >= cfg.upgrade_5_need then
            return 5
        end
        if val >= cfg.upgrade_4_need then
            return 4
        end
        if val >= cfg.upgrade_3_need then
            return 3
        end
        if val >= cfg.upgrade_2_need then
            return 2
        end
    end
    return 1  
end

--npc好感等级奖励(领取)进度
function FavorData:GetNpcFavorLevelPro(cfg)
    if cfg == nil then
        cfg = self.npc_config
    end
    local npc_favor_data = self.favor_data.info.favor_list[cfg.seq + 1]
    local flag = npc_favor_data.fetch_flag
    local level = 5
    local next_target = cfg.upgrade_5_need
    for i = 1, 4 do
        if flag[i] == 0 then
            level = i
            next_target = cfg["upgrade_" .. (level + 1) .. "_need"]
            break
        end
    end
    local val = self.favor_data.info.favor_list[cfg.seq + 1].favor_value
    if val > next_target then
        val = next_target
    end
    return {level = level, next_target = next_target, val = val}
end


--npc好感进度
function FavorData:GetNpcProGress(cfg, level)
    if cfg == nil then
        cfg = self.npc_config
    end
    if level == nil then
        level = self:GetNpcFavorLevel()
    end
    local val, target
    if level == 5 then
        val = cfg.upgrade_5_need
        target = cfg.upgrade_5_need
    else
        local favor_data = self:GetFavorInfo(cfg)
        val = favor_data.favor_value
        target = cfg["upgrade_" .. (level + 1) .. "_need"]
    end
    if val > target then
        val = target
    end
    return {val = val, target = target}
end


--今日剩余赠礼次数
function  FavorData:TodayGiftTimes()
    local max=Config.npc_mood_auto.other[1].gift_total_times
    local times = max - self.favor_data.info.total_gift_times
    return times
end

--好感体力
function  FavorData:ActionTimes()
    local times =  self.favor_data.info.action_value
    return times
end

--npc今日剩余赠礼次数
function FavorData:NpcTodayGiftTimes()
    local cfg = self.npc_config
    local npc_favor_data = self.favor_data.info.favor_list[cfg.seq + 1]
    local max = Config.npc_mood_auto.other[1].gift_times
    local times=max-npc_favor_data.gift_times;
    return times
end

-- npc今日剩余宴请次数
function FavorData:NpcTodayBanquetTimes()
    local cfg = self.npc_config
    local npc_favor_data = self.favor_data.info.favor_list[cfg.seq + 1]
    local max = Config.npc_mood_auto.other[1].feast_times
    local times = max - npc_favor_data.banquet_times;
    return times
end

-- npc 礼物列表
function FavorData:NpcGiftList()
    local list = {}
    local cfg = self.npc_config
    local favor_type = cfg.like_type
    for k, v in pairs(Config.npc_mood_auto.gift_list) do
        local grid = BagData.Instance:GetItemsByItemId(v.item_id)
        for _, v2 in pairs(grid) do
            local info = {}
            local num = Item.GetNum(v.item_id)
            info.item_id = v.item_id
            info.is_favor = v.type == favor_type and 1 or 0
            info.mood_value = v.mood_value
            if info.is_favor == 1 then
                info.mood_value = info.mood_value + v.like_addup
            end
            info.type = v.type
            info.num = num
            table.insert(list, info)
            break;
        end
    end
    table.sort(list, function(a, b)
        if a.is_favor == b.is_favor then
            if a.type == b.type then
                local a_qua = Item.GetColor(a.item_id)
                local b_qua = Item.GetColor(b.item_id)
                if a_qua == b_qua then
                    return a.mood_value > b.mood_value
                else
                    return a_qua > b_qua
                end
            else
                return a.type < b.type
            end
        else
            return a.is_favor > b.is_favor
        end
    end)
    return list
end

function FavorData:InitNpcSelectGiftData()
    self.favor_gift_select_data.list = {}
    self.favor_gift_select_data.val = 0
end

--npc选中赠礼列表
function FavorData:NpcSelectGiftValue(item_id, num, add_val,col)
    local grid = BagData.Instance:GetItemsByItemId(item_id)
    local add_num = num
    self.favor_gift_select_data.list[item_id] = {}
    for _, v in pairs(grid) do
        if add_num > 0 then
            local index = v.index
            local select_num = math.min(add_num,v.num) 
            add_num=add_num-select_num
            self.favor_gift_select_data.list[item_id][index] =  {col=v.column_type, item_id = item_id, num = select_num}
        end
    end
    self.favor_gift_select_data.val = self.favor_gift_select_data.val + add_val
end

--npc选中赠礼数量
function FavorData:NpcSelectGiftNum(item_id)
    local num = 0
    if self.favor_gift_select_data.list and self.favor_gift_select_data.list[item_id] then
        for _, v in pairs(self.favor_gift_select_data.list[item_id]) do
            num = num + v.num
        end
    end
    return num
end

-- npc赠礼
function FavorData:NpcGift()
    self.old_val = self:GetFavorInfo().favor_value
    if self.favor_gift_select_data.list then
        local npc_index = self.npc_config.seq
        local list = {}
        for _, v in pairs(self.favor_gift_select_data.list) do
            for index, v2 in pairs(v) do
                table.insert(list,{col=v2.col,index=index,times=v2.num})
            end
        end
        FavorCtrl.Instance:SendFavorReq(FAVOR_NPC_OP_TYPE.GIFT, npc_index, 0, list)
    end
    self.favor_gift_select_data.list = {}
    self.favor_gift_select_data.val = 0
end

--npc选中赠礼好感度
function FavorData:GetNpcSelectGiftValue()
    return self.favor_gift_select_data.val or 0
end

--npc选中赠礼次数
function FavorData:GetNpcSelectGiftTimes()
    local times = 0;
    if self.favor_gift_select_data.list then
        for _, v in pairs(self.favor_gift_select_data.list) do
            for _,v2 in pairs(v) do
                times = times + v2.num
            end
        end
    end
    return times
end

--赠礼消耗体力
function FavorData:NpcGiftCost()
    return Config.npc_mood_auto.other[1].gift_cost
end

--npc好感等级是否上限
function FavorData:IsMaxLevel(level)
    if level == nil then
        level=self:GetNpcFavorLevel()
    end
    return level == 5
end

--npc好感度称号
function FavorData:GetNpcFavorTitle(level)
    return Config.npc_mood_auto.mood_name[level].mood_name
end 

--npc宴席
function FavorData:GetNpcBanquet(cfg)
    local list={}
    if cfg == nil then
        cfg = self.npc_config
    end
    table.insert(list,Config.npc_mood_auto.feast_list[cfg.feast_1])    
    table.insert(list,Config.npc_mood_auto.feast_list[cfg.feast_2])   
    return  list
end

-- 宴请消耗体力
function FavorData:NpcBanquetTiliCost()
    return Config.npc_mood_auto.other[1].feast_cost
end

--宴席物品列表
function FavorData:NpcBanquetList(cfg)
    local list = {}
    for i = 1, 6 do
        if cfg["food_" .. i] ~= 0 and cfg["food_" .. i] ~= "" then
            table.insert(list, {item_id = cfg["food_" .. i], num = cfg["num_" .. i]})
        end
    end
    return list
end


--好感npc区域列表
function FavorData:GetNpcZoneList()
    local list = {}
    local cfg_zone = Config.npc_mood_auto.sence_reward
    for _, v in pairs(cfg_zone) do
        if v.is_show == 1 then
            local npc_seq_list = Split(v.npc_list, "|")
            local npc_list = {}
            local zhiyou_num = 0
            for _, seq in pairs(npc_seq_list) do
                local cfg_npc = self:GetNpcFavorBySeq()[tonumber(seq)]
                local val = self:GetFavorInfo(cfg_npc).favor_value
                local level = self:GetNpcFavorLevel(val, cfg_npc)
                if self:IsMaxLevel(level) then
                    zhiyou_num = zhiyou_num + 1;
                end
                local npc_info = {}
                npc_info.val = val
                npc_info.level = level
                npc_info.cfg = cfg_npc
                table.insert(npc_list, npc_info)
            end
            local zone_info = {}
            zone_info.cfg_zone_reward = v
            zone_info.zhiyou_num = zhiyou_num
            zone_info.npc_list = npc_list
            table.insert(list, zone_info)
        end
    end
    table.sort(list, function(a, b)
        return a.cfg_zone_reward.sort < b.cfg_zone_reward.sort
    end)
    return list
end

--默认选中的场景 当前场景/第一个场景
function FavorData:GetSelectZone(list)
    local cur_sence_id=SceneLogic.Instance:SceneID()
    local select_zone=1
    for k,v in pairs(list) do
        if v.cfg_zone_reward.sence_id == cur_sence_id then
            select_zone = k
            break
        end 
    end
    return select_zone
end

-- 默认选中的npc 非满级npc/第一个npc
function FavorData:GetSelectNpc(npc_list)
    local select_npc = 1
    for k, v in pairs(npc_list) do
        if not self:IsMaxLevel(v.level) then
            select_npc = k
            break
        end
    end
    return select_npc
end

-- npc区域奖励红点
function FavorData:GetNpcZoneRed(seq)
    if not self.favor_data.info then
        return 0
    end
    if self.favor_data.info.zone_fetch_flag[seq + 1] == 1 then
        return 0
    end
    local cfg_zone = Config.npc_mood_auto.sence_reward[seq + 1]
    local npc_seq_list = Split(cfg_zone.npc_list, "|")
    for _, seq_npc in pairs(npc_seq_list) do
        local cfg_npc = self:GetNpcFavorBySeq()[tonumber(seq_npc) ]
        if cfg_npc ~= nil then
            local val = self:GetFavorInfo(cfg_npc).favor_value
            local level = self:GetNpcFavorLevel(val, cfg_npc)
            if not self:IsMaxLevel(level) then
                return 0
            
            end
        end
    end
    return 1
end

function FavorData:RemindZone()
    if not self.favor_data.info then
        return 0
    end
    local cfg_zone = Config.npc_mood_auto.sence_reward
    for _, v in pairs(cfg_zone) do
        if self:GetNpcZoneRed(v.seq) == 1 then
            return 1
        end
    end
    return 0
end


--npc好感等级奖励是否领取完
function  FavorData:IsNpcLevelFectch(cfg)
    local npc_info=self:GetFavorInfo(cfg)
    local flag=npc_info.fetch_flag
    return flag[4] == 1
end

--npc好感奖励是否可领取
function FavorData:IsCanNpcFavorReward(cfg)
    local progress_data = FavorData.Instance:GetNpcFavorLevelPro(cfg)
    local npc_info = self:GetFavorInfo(cfg)
    local flag = npc_info.fetch_flag
    if progress_data.val >= progress_data.next_target and flag[progress_data.level] == 0 and not self:IsNpcLevelFectch(cfg) then
        return true
    end
    return false
end








