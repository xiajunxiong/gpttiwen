AnimalWorldData = AnimalWorldData or BaseClass()

-- 异兽临世操作请求类型，3000
AnimalWorldType = {
    Info = 0,       --请求下发
    Buy = 1,        --购买门票
    Task = 2,       --领取任务奖励，p1:seq
    Exchange = 3,   --兑换物品，p1:seq
}

function AnimalWorldData:__init()
    if AnimalWorldData.Instance ~= nil then
        Debuger.LogError("[AnimalWorldData] attempt to create singleton twice!")
        return
    end
    AnimalWorldData.Instance = self
    self.config = Config.yishoulinshi_auto
    self.other = self.config.other[1]
    -- 当前显示的panel
    self.now_show_panel = SmartData.New({mod_id = Mod.AnimalWorld.Main})
    self.info_data = SmartData.New()
    self.shop_data = SmartData.New({index = 1})
    
    self:ResetData()
end

function AnimalWorldData:__delete()
    AnimalWorldData.Instance = nil
end

function AnimalWorldData:ResetData()
    local info = {}
    info.is_buy = 0 --是否购买
    info.satisfy = 0    --好感度
    info.today_task_finish = {} --每日任务是否完成
    info.is_get_reward = {} --每日任务奖励是否领取
    info.task_param = {} --每日任务进度
    info.other_buy_count = {}   --限购道具已兑换次数
    self.info_data.info = info
    local base = {}
    base.is_open = 0 --是否开启
    base.type = 0 --对应时间配置的期数索引
    self.info_data.base = base
    self.info_data.animal_buy_count = 0  --剩余神兽全服兑换数量
end

function AnimalWorldData:TodayTaskFinish(seq)
    local today_task_finish = self.info_data.info.today_task_finish[seq] or 0
    return today_task_finish == 1
end

function AnimalWorldData:IsGetReward(seq)
    local is_get_reward = self.info_data.info.is_get_reward[seq] or 0
    return is_get_reward == 1
end

function AnimalWorldData:TaskParam(seq)
    return self.info_data.info.task_param[seq] or 0
end

function AnimalWorldData:OtherBuyCount(seq)
    return self.info_data.info.other_buy_count[seq] or 0
end

-- 返回需要监听的item_list
function AnimalWorldData:GetItemListen()
    if not self.item_listen_list then
        self.item_listen_list = {}
        for k, v in pairs(self.config.recycle_configure) do
            table.insert(self.item_listen_list, v.recycle_item_id)
        end
    end
    return self.item_listen_list
end

-- 判断某个道具是否已换完
function AnimalWorldData:IsEmptyCount(seq)
    local exchange_cfg = self:ExchangeConfig()
    -- 有好感度的就是神兽，需要特殊处理
    if exchange_cfg[seq + 1].unlock_integral > 0 then
        if self.info_data.animal_buy_count > 0 then
            return self:OtherBuyCount(seq) >= exchange_cfg[seq + 1].limit_times
        else
            return true
        end
    end
    return self:OtherBuyCount(seq) >= exchange_cfg[seq + 1].limit_times
end

-- 该道具剩余兑换次数
function AnimalWorldData:ItemCanTimes(seq)
    local exchange_cfg = self:ExchangeConfig()
    -- 有好感度的就是神兽，需要特殊处理
    if exchange_cfg[seq + 1].unlock_integral > 0 then
        return self.info_data.animal_buy_count
    end
    return exchange_cfg[seq + 1].limit_times - self:OtherBuyCount(seq)
end

-- 时间配置
function AnimalWorldData:TimeInfo()
    local t = DataHelper.GetSearch(self.config.times_configure, "type", self.info_data.base.type)
    if TableIsEmpty(t) then
        LogError("无法获取时间配置type", self.info_data.base.type)
    end
    return t or {}
end

-- 兑换组id
function AnimalWorldData:ExchangeGroup()
    local time_info = self:TimeInfo()
    return time_info.exchange_group
end

-- 宠物展示配置
function AnimalWorldData:PetShowInfo()
    local exchange_group = self:ExchangeGroup()
    return DataHelper.GetSearch(self.config.pet_show, "exchange_group", exchange_group)
end

-- 兑换配置
function AnimalWorldData:ExchangeConfig()
    local exchange_group = self:ExchangeGroup()
    local t = {}
    for k, v in pairs(self.config.exchange_configure) do
        if v.exchange_group == exchange_group then
            t[v.seq + 1] = v
        end
    end
    return t
end

-- 购买配置
function AnimalWorldData:BuyConfig()
    return self.config.buy_configure[1]
end

-- 当前的任务配置
function AnimalWorldData:NowTaskConfig()
    if not self.task_config then
        self.task_config = {}
        for i, v in ipairs(self.config.task_configure) do
            if not self.task_config[v.task_group] then
                self.task_config[v.task_group] = {}
            end
            self.task_config[v.task_group][v.seq + 1] = v
        end
    end
    local exchange_group = self:ExchangeGroup()
    return self.task_config[exchange_group]
end

-- 任务配置
function AnimalWorldData:TaskConfig()
    local t = {}
    -- 需要排序
    for k, v in pairs(self:NowTaskConfig()) do
        local t1 = TableCopy(v, 3)
        -- 处理好感度奖励道具
        if v.like > 0 then
            local satisfy_item = {item_id = self:SatisfyId(), is_bind = 1, num = v.like}
            local reward_item = {}
            table.insert(reward_item, satisfy_item)
            for l, w in pairs(t1.reward_item) do
                table.insert(reward_item, w)
            end
            t1.reward_item = reward_item
        end
        table.insert(t, t1)
    end
    -- 排序
    table.sort(t, DataHelper.WeightSort("seq", function(data)
        if self:IsGetReward(data.seq) then
            return 1000
        elseif self:TodayTaskFinish(data.seq) then
            return -1000
        end
    end))
    return t
end

-- 副本配置（需要协议完成来排序，未实现）
function AnimalWorldData:FBConfig()
    local t = {}
    -- 需要排序
    for k, v in pairs(self.config.secret_area_configure) do
        table.insert(t, TableCopy(v, 3))
    end
    return t
end

-- 副本挑战了多少次（未实现）
function AnimalWorldData:FBCount(seq)
    return 0
end


-- 是否展示副本
function AnimalWorldData:IsShowFB()
    return self.other.is_show == 1
end

-- 总满意度
function AnimalWorldData:SatisfyFull()
    return self.other.integral_up
end

-- 满意度道具id
function AnimalWorldData:SatisfyId()
    return self.other.like_item_id
end

-- 红点
function AnimalWorldData:AllRemind()
    if self.info_data.base.is_open then
        if FunOpen.Instance:IsFunOpen(Mod.AnimalWorld.Main) then
            for k, v in pairs(self:NowTaskConfig()) do
                if not self:IsGetReward(v.seq) and self:TodayTaskFinish(v.seq) then
                    return 1
                end
            end
        end
    end
    return 0
end

-- 判断是否开启
function AnimalWorldData:IsOpen()
    return self.info_data.base.is_open > 0 and FunOpen.Instance:IsFunOpen(Mod.AnimalWorld.Main)
end

function AnimalWorldData:SetInfoData(protocol)
    self.info_data.info = protocol.info
end

function AnimalWorldData:SetBaseData(protocol)
    self.info_data.base = protocol.info
    ActivityRandData.Instance:CheckRandOpenData()
end

function AnimalWorldData:SetBuyCountData(protocol)
    self.info_data.animal_buy_count = protocol.animal_buy_count
end
