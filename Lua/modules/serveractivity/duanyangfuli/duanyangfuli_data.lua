-- require("D:/O/Assets/Game/Lua/config/auto/duanyangfuli_auto")
ActDuanYangFuLiData = ActDuanYangFuLiData or BaseClass()

function ActDuanYangFuLiData:__init()
    if ActDuanYangFuLiData.Instance ~= nil then
        Debuger.LogError("[ActDuanYangFuLiData] attempt to create singleton twice!")
        return
    end
    if Config.duanyangfuli_auto == nil then
        require("config/auto/duanyangfuli_auto")
    end
    ActDuanYangFuLiData.Instance = self
    -- self.actOpenDay = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_DUANYANGFULI)
    self.duanyangfuli_smart = SmartData.New()

    self.IsGetFirst = {}
    self.LastFetchRewardTimes = {}

    
    -- -- 红点刷新
    self.remind_data = SmartData.New({v = false})
end

function ActDuanYangFuLiData:__delete()
	ActDuanYangFuLiData.Instance = nil
end


function ActDuanYangFuLiData:ClearData()
end

function ActDuanYangFuLiData:SetDuanYangFuLiInfo(protocol)
    self.duanyangfuli_smart:Set(protocol.info)
end

-- 获得折扣价
-- 	返回配置中值
function ActDuanYangFuLiData:GetDiscountPrice()
    return Config.duanyangfuli_auto.other[1].discount_price/10;
end

-- 获得折扣价
-- 	返回配置中值
function ActDuanYangFuLiData:GetAllBagMoney()

    if self.section == nil then
        local cfg_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DUANYANGFULI, Config.duanyangfuli_auto.basis_configuration)
        self.section = cfg_list
    end

    local all_bag_money = 0

    -- LogError("GetAllBagMoney()self.section",self.section)


    for basis_configuration_key, basis_configuration_value in pairs(self.section) do
        all_bag_money = basis_configuration_value.price/10 + all_bag_money
    end

    return all_bag_money;
end



function ActDuanYangFuLiData:GetDuanYangFuLiItemList()

    local cfg_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DUANYANGFULI, Config.duanyangfuli_auto.basis_configuration)
    self.section = cfg_list
    self.actOpenDay = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_DUANYANGFULI)

    local duanyangfuli_item_list={}
    -- 插入每一个基础配置当大item
    for basis_configuration_key, basis_configuration_value in pairs(self.section) do
        -- local fetchRewardTimesFlag = self.duanyangfuli_smart.item_list[basis_configuration_value.seq].fetch_reward_times or 0
        -- basis_configuration_value.is_show_button = (fetchRewardTimesFlag == )
        if self.duanyangfuli_smart.item_list == nil then
            return
        end
        local isBuyFlag = self.duanyangfuli_smart.item_list[basis_configuration_value.seq].is_buy or 0
        -- LogError("is_buy",isBuyFlag)
        basis_configuration_value.is_buy = (isBuyFlag==1)

        local fetch_reward_times  =  self.duanyangfuli_smart.item_list[basis_configuration_value.seq].fetch_reward_times or 0 
        basis_configuration_value.fetch_reward_times = fetch_reward_times



        table.insert(duanyangfuli_item_list,basis_configuration_value)  
    end
    return duanyangfuli_item_list

end


function ActDuanYangFuLiData:GetRewardItemList(reward_item)
    local reward_item_list = {}
    for i=0,#reward_item do
        table.insert(reward_item_list, Item.Create(reward_item[i]))
    end
    return reward_item_list
end

function ActDuanYangFuLiData:IsBuyEvenOnlyOne()

    local cfg_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DUANYANGFULI, Config.duanyangfuli_auto.basis_configuration)
    self.section = cfg_list

    if self.duanyangfuli_smart.item_list == nil then
        return
    end

    for basis_configuration_key, basis_configuration_value in pairs(self.section) do
        if  self.duanyangfuli_smart.item_list[basis_configuration_value.seq].is_buy == 1 then
            return true
        end
    end

    return false
end


-- 红点通知 取反 相当于setdirty
function ActDuanYangFuLiData:RemindNotice()
    self.remind_data.v = not self.remind_data.v
end


-- 红点逻辑
function ActDuanYangFuLiData:RemindNum()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_DUANYANGFULI) then
        return 0
    end

    if not self.first_remind then
        return 1
    end

    if self:CanGetRemind() > 0 then
        return 1
    end

    return 0
end


function ActDuanYangFuLiData:CanGetRemind()
    local itemList = self:GetDuanYangFuLiItemList()
    for k,v in pairs(itemList) do
        local flag = self:CanGetFlag(v)
        if flag > 0 then
            return 1
        end
    end

    return 0
end

function ActDuanYangFuLiData:CanGetFlag(data)

    self.actOpenDay = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_DUANYANGFULI)


    if TableIsEmpty(self.duanyangfuli_smart) then
        return 0
    end

    if self.duanyangfuli_smart.item_list[data.seq].is_buy == 1 and 
    -- 且 已领取次数小于最大领取次数 且 活动开启天数-礼包活动时间+1>礼包已领取次数，可以领取 才显示领取按钮
    (self.duanyangfuli_smart.item_list[data.seq].fetch_reward_times < data.receive_days and 
    self.actOpenDay - data.activity_times + 1 > self.duanyangfuli_smart.item_list[data.seq].fetch_reward_times ) == true 
    then        
        return 1
    else
        return 0
    end




end



