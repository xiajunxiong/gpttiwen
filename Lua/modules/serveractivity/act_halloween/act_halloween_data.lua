ActHalloweenData = ActHalloweenData or BaseClass()

function ActHalloweenData:__init()
    if ActHalloweenData.Instance ~= nil then
        Debuger.LogError("[ActHalloweenData] attempt to create singleton twice!")
        return
    end
    ActHalloweenData.Instance = self
    self.lei_chong_data = SmartData.New({chong_zhi_value = 0, fetch_flag = {}})
    self.shop_data = SmartData.New({curr_num=0, ra_day_buy_times={},ra_buy_times = {}})

end

function ActHalloweenData:onFlushCurr()
    local item_id = self:GetShopCfg()[1].give_item.item_id
    self.shop_data.curr_num = Item.GetNum(item_id)
    self.shop_data:Notify()
end

--显示列表
function  ActHalloweenData:GetShowStyle()
    local cfg=Config.activity_main_auto.btn_list;
    for k, v in pairs(cfg) do
        if v.mod_key == Mod.Act_Halloween.Main then
            return v.interface
        end
    end 
    return 2;
end

-- 累充
function ActHalloweenData:GetLeiChongCfg()
    self.cfg_leichong = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG, Config.wanshengleichong_auto.basis_configuration, "seq")
    return  self.cfg_leichong
end


function ActHalloweenData:SetLeiChongData(protocol)
    self.lei_chong_data.chong_zhi_value = protocol.chong_zhi_value
    self.lei_chong_data.fetch_flag = protocol.fetch_flag
end

function ActHalloweenData:RemindLeiChong()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG) or self.lei_chong_data.chong_zhi_value == 0 then
        return 0
    end
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG)
    if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG)) then
        return 1
    end
    local cfg = self:GetLeiChongCfg()
    for k, v in pairs(cfg) do
        if self.lei_chong_data.fetch_flag[v.seq] == 0 and self.lei_chong_data.chong_zhi_value >= v.acc_recharge_down / 10 then
            return 1
        end
    end
    return 0
end

function ActHalloweenData:ClearLeiChongFirstRemind()
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG)
    if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG)) then
        PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_WAN_SHENG_LEI_CHONG), act_start)
        self.lei_chong_data:Notify()
    end
end

function ActHalloweenData:DelLeiChongReward()
    local reward = {[1] = {item_id = 22005, num = 1, is_bind = 1}, [2] = {item_id = 24514, num = 1, is_bind = 1}}
    local list = {}
    for k, v in pairs(reward) do
        table.insert(list, {item =  Item.Create(v), is_xiyou = false})
    end
    return list
end


--商城
function ActHalloweenData:GetShopCfg()
    self.cfg_shop = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP, Config.wanshengshangcheng_auto.basis_configuration, "seq")
    return self.cfg_shop
end

function ActHalloweenData:SetShopData(protocol)
    self.shop_data.ra_day_buy_times=protocol.ra_day_buy_times
    self.shop_data.ra_buy_times=protocol.ra_buy_times
end

function ActHalloweenData:GetShopItemSortList()
    local cfgs = self:GetShopCfg()
    table.sort(cfgs, function(a, b)
        local a_can_buy = self:GetBuyTime(a) > 0 and 1 or 0
        local b_can_buy = self:GetBuyTime(b) > 0 and 1 or 0
        if a_can_buy == b_can_buy then
            return a.seq < b.seq
        end
        return a_can_buy > b_can_buy
    end)
    return cfgs
end

function ActHalloweenData:GetShopRewardList(cfg)
    local rewards=cfg.reward_item
    local list1={}
    local list2={}
    local index = 0
    if cfg.gold > 0 then
        table.insert(#rewards<1 and list2 or list1, Item.InitData(HuoBi[0], cfg.gold, 1))
    end

    for i=#rewards ,0,-1 do
        if #list2>=2 then
            table.insert(list1, rewards[i])
        else
            table.insert(list2, rewards[i])
        end
    end
    return list1,list2
end

function ActHalloweenData:GetBuyTime(cfg)
    local limit_time=cfg.limit_type==1 and cfg.total_limit_times or cfg.day_limit_times
    local buyed_times=cfg.limit_type==1 and self.shop_data.ra_buy_times[cfg.seq] or self.shop_data.ra_day_buy_times[cfg.seq]
    return limit_time - (buyed_times or 0)
end

function ActHalloweenData:RemindShop()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP)  then
        return 0
    end
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP)
    if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP)) then
        return 1
    end
    if self.shop_data.curr_num >= 10 then
        return 1
    end
    return 0
end

function ActHalloweenData:ClearShopFirstRemind()
    local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP)
    if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP)) then
        PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_WAN_SHENG_SHOP), act_start)
        self.shop_data:Notify()
    end
end
