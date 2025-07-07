ActDuanYangZiXuanLiData = ActDuanYangZiXuanLiData or BaseClass()
ActDuanYangZiXuanLiData.GiftIcon = { "_LocLiBao-Zi","_LocLiBao-Cheng","_LocLiHe-Zi","_LocLiHe-Cheng"}
ActDuanYangZiXuanLiData.WuPinKuang = {"WuPinKuangPinZhiZi","WuPinKuangPinZhiCheng","WuPinKuangPinZhiZi","WuPinKuangPinZhiCheng"}
-- 初始化
function ActDuanYangZiXuanLiData:__init()
    if ActDuanYangZiXuanLiData.Instance ~= nil then
        Debuger.LogError("[ActDuanYangZiXuanLiData] attempt to create singleton twice!")
        return
    end
    ActDuanYangZiXuanLiData.Instance = self

    -- local cfg_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI, Config.duanyangzixuanli_auto.basis_configuration)
    -- self.section = cfg_list
    -- LogError("端阳自选礼sectionsectionsectionsection",cfg_list)

    self.duanyangzixuanli_smart = SmartData.New()
    
    -- self.actOpenDay = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI)
    -- LogError("actOpenDay",self.actOpenDay)

    -- -- 红点刷新
    self.remind_data = SmartData.New({v = false})
end

function ActDuanYangZiXuanLiData:__delete()
	ActDuanYangZiXuanLiData.Instance = nil
end


function ActDuanYangZiXuanLiData:ClearData()
end

-- 设置端阳自选礼信息到smartdata里
function ActDuanYangZiXuanLiData:SetDuanYangZiXuanLiInfo(protocol)

    self.duanyangzixuanli_smart:Set(protocol.info)

end

-- 获得端阳自选礼大item列表
function ActDuanYangZiXuanLiData:GetDuanYangZiXuanLiBigItemList()

    local cfg_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI, Config.duanyangzixuanli_auto.basis_configuration)
    self.section = cfg_list
    self.actOpenDay = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI)

    local duanyangzixuanli_big_item_list={}
    -- 插入每一个基础配置当大item
    for basis_configuration_key, basis_configuration_value in pairs(self.section) do
        -- flag等于smartdata的当前天有没有领取奖励，假如获取奖励了fetch_reward_flag[basis_configuration_value.activity_day]就会为1，否则为0
        local flag = self.duanyangzixuanli_smart.fetch_reward_flag[basis_configuration_value.activity_day] or 0

        -- 假如flag不等于0 说明当前天领了奖励，isReceive=true
        basis_configuration_value.isReceive = (flag ~= 0)
        table.insert(duanyangzixuanli_big_item_list,basis_configuration_value)  
    end
    return duanyangzixuanli_big_item_list
end

-- 获得端阳自选礼.item列表
-- function ActDuanYangZiXuanLiData:GetDuanYangZiXuanLiItemList(reward_group,gift_icon,gift_desc,ChaseButtonYellow,BigItem)
function ActDuanYangZiXuanLiData:GetDuanYangZiXuanLiItemList(reward_group,ChaseButtonYellow,BigItem)

    local cfg_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI, Config.duanyangzixuanli_auto.basis_configuration)
    self.section = cfg_list
    self.actOpenDay = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI)

    local duanyangzixuanli_item_list={}
    for reward_group_key, reward_group_value in pairs(Config.duanyangzixuanli_auto.reward_group) do
        if reward_group_value.reward_group == reward_group then
            -- reward_group_value.gift_icon = gift_icon
            -- reward_group_value.gift_desc = gift_desc
            reward_group_value.ChaseButtonYellow = ChaseButtonYellow
            reward_group_value.BigItem = BigItem
            table.insert(duanyangzixuanli_item_list,reward_group_value)
            -- LogError(" reward_group_value.gift_desc", reward_group_value.gift_desc)
        end
    end    
    -- LogError("duanyangzixuanli_item_list",duanyangzixuanli_item_list)
    return duanyangzixuanli_item_list
end

-- 获得全部购买花费
-- 	返回配置中值
function ActDuanYangZiXuanLiData:GetBuyAllMoneyCost()
    return Config.duanyangzixuanli_auto.other[1].price/10;
end


function ActDuanYangZiXuanLiData:GetSelfSectionLength()
    local cfg_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI, Config.duanyangzixuanli_auto.basis_configuration)
    self.section = cfg_list
    -- LogError("self:GetLength(self.section)长度",self:GetLength(self.section))
    return self:GetLength(self.section)
end

function ActDuanYangZiXuanLiData:GetLength(t)
    local res=0
    for k,v in pairs(t) do
        res=res+1
    end
    return res
end

-- 红点通知 取反 相当于setdirty
function ActDuanYangZiXuanLiData:RemindNotice()
    self.remind_data.v = not self.remind_data.v
end


-- 红点逻辑
function ActDuanYangZiXuanLiData:RemindNum()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI) then
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


function ActDuanYangZiXuanLiData:CanGetRemind()
    local BigItemList = self:GetDuanYangZiXuanLiBigItemList()
    for k,v in pairs(BigItemList) do
        local flag = self:CanGetFlag(v)
        if flag > 0 then
            return 1
        end
    end

    return 0
end

function ActDuanYangZiXuanLiData:CanGetFlag(data)
    if TableIsEmpty(self.duanyangzixuanli_smart) then
        return 0
    end
    -- LogError("self.actOpenDay ",self.actOpenDay)
    local startime,endtime =ActivityRandData.GetBaseDayOpenStampTime(ACTIVITY_TYPE.RAND_DUANYANGZIXUANLI)
    local CanReceiveTimeMeterOver = TimeManager.GetServerTime() > endtime-86400 * (self:GetSelfSectionLength()+1-data.activity_day)
    local flag = (not data.isReceive) and (CanReceiveTimeMeterOver == true)

    if self.duanyangzixuanli_smart.is_buy_reward == 1 and 
    self.duanyangzixuanli_smart.fetch_reward_flag[data.activity_day]  == 0 and
    flag == true
    then        
        return 1
    else
        return 0
    end

end