LBShopData = LBShopData or BaseClass()

function LBShopData:__init()
    if LBShopData.Instance ~= nil then
        Debuger.LogError("[LBShopData] attempt to create singleton twice!")
        return
    end
    LBShopData.Instance = self

    self.shop_info = SmartData.New({flush = 0}) -- 5524
    self.buff_info = SmartData.New({flush = 0}) -- 5525
    self.buff_shop_info = SmartData.New({flush = 0}) -- 5526

    self.buff_shop_oper = SmartData.New({flush = 0}) 
    self.exchange_oper = SmartData.New({flush = 0})  
    self:InitConfig()

    self.open_mark = false
end 

function LBShopData:InitConfig()
    self.config = Config.divine_domain_shop_auto

    self.shop_unique_lit = KeyList(self.config.item_group,"unique_seq")
    self.buff_shop_seq_lit = KeyList(self.config.buff_shop,"seq")
    self.buff_shop_buff_seq_lit = KeyList(self.config.buff_shop,"buff_seq")
    self.buff_shop_buff_base_seq_lit = KeyList(self.config.buff_parameter,"seq")
    -- self.exchange_shop_seq_list = KeyList(self.config.exchange_shop,"seq")
end 

function LBShopData:SetShopInfo(protocol)
    self.shop_info.next_refresh_timestamp = protocol.next_refresh_timestamp
    self.shop_info.manual_refresh_times = protocol.manual_refresh_times
    self.shop_info.daily_reward_flag = protocol.daily_reward_flag
    self.shop_info.item_flag = protocol.item_flag
    self.shop_info.items = protocol.items

    self.shop_info.flush = self.shop_info.flush +1
end

function LBShopData:SetBuffInfo(protocol)
    local cache_num = self.buff_info.buff_num
    local cache = self.buff_info.buffs

    self.buff_info.buff_num = protocol.buff_num
    self.buff_info.buffs = protocol.buffs

    -- 首下发不作数
    if cache ~= nil and cache_num ~= nil then 
        -- 取获得buff的下发
        if self.buff_info.buff_num > cache_num then 
            local cfg = LBShopData.Instance:GetShopBuffBaseCfg(self.buff_info.buffs[self.buff_info.buff_num].buff_seq)
            PublicPopupCtrl.Instance:Center(string.format(Language.LandsBetween.Desc.ShopBuyBuffTips,Item.GetQuaName(cfg.item.item_id)))
        end 
    end 

    self.buff_info.flush = self.buff_info.flush +1
end

function LBShopData:SetBuffShop(protocol)
    self.buff_shop_info.buy_times = protocol.buy_times

    self.buff_shop_info.flush = self.buff_shop_info.flush +1
end

function LBShopData:GetShopParam()
    local param = {}
    param.shop_list = self:GetShopList()

    local flush_param = self:GetFlushLimit()
    param.flush_cost = flush_param.flush_cost
    param.flush_time = string.format(Language.LBShop.FlushTimeDesc,self.config.other[1].refresh_times_max - flush_param.flush_time,self.config.other[1].refresh_times_max)
    param.flush_icon = flush_param.flush_icon
    param.flush_done = flush_param.flush_time >= self.config.other[1].refresh_times_max

    param.flush_type = flush_param.flush_type
    param.next_flush_time = self.shop_info.next_refresh_timestamp
    param.reward_num = self.shop_info.daily_reward_flag == 0 and 1 or 0

    return param
end

function LBShopData:GetShopList()
    local list = {}
    for k,v in pairs(self.shop_info.items) do 
        local vo = {}
        vo.index = k - 1 
        vo.buyed = self.shop_info.item_flag[vo.index] == 1
        vo.unique_seq = v.unique_seq
        vo.discount = v.discount
        vo.with_discount = vo.discount < 10
        vo.discount_str = vo.discount ..Language.LBShop.DiscountDesc

        local item_cfg = self:GetShopItemCfg(vo.unique_seq)
        vo.item_call = BagData.Instance:ItemBuild(item_cfg.item) 
        vo.name = vo.item_call:QuaName(true)

        vo.origin_price = item_cfg.price
        vo.now_price = 0.1*(vo.discount) * vo.origin_price

        vo.price_type = item_cfg.price_type
        vo.origin_price_type = HuoBi[vo.price_type]
        vo.now_price_type = HuoBi[vo.price_type]
        table.insert(list,vo)
    end 
    return list
end

function LBShopData:GetShopItemCfg(seq)
    return self.shop_unique_lit[seq]
end

function LBShopData:GetFlushLimit()
    local param = {}

    param.flush_time = self.shop_info.manual_refresh_times

    local cur_time = param.flush_time + 1
    for k,v in pairs(self.config.manual_refresh) do 
        if cur_time == v.manual_refresh_times then
            param.flush_icon = HuoBi[v.manual_price_type]
            param.flush_cost = v.manual_price
            param.flush_type = v.manual_price_type
            break
        end
    end 

    return param
end


function LBShopData:GetBuffShopParam()
    local param = {}

    param.buff_list = self:GetBuffShopItemList()

    param.can_show = self.buff_shop_oper.select ~= nil
    if not param.can_show then 
        return param
    end 

    local buff_cfg = self:GetShopBuffCfg(self.buff_shop_oper.select.seq)
    param.item_call = BagData.Instance:ItemBuild(buff_cfg.item) 
    param.name = param.item_call:QuaName(true)
    param.price_type = HuoBi[buff_cfg.price_type]
    param.desc = param.item_call:Desc()
    param.class_name = param.item_call:TypeDesc()

    param.times_str = buff_cfg.limit_times > 0 and  string.format(Language.LBShop.BuffBuyDetailLimitTips[buff_cfg.limit_type],
    buff_cfg.limit_times - self.buff_shop_info.buy_times[self.buff_shop_oper.select.seq+1],
    buff_cfg.limit_times) or Language.LBShop.BuffBuyDetailLimitTips[buff_cfg.limit_type]
    param.buy_times = self.buff_shop_oper.buy_times
    param.buy_price = "x"..buff_cfg.price

    return param
end

function LBShopData:SetOperItem(data)
    self.buff_shop_oper.select = data
    self.buff_shop_oper.flush = self.buff_shop_oper.flush + 1
    self.buff_shop_oper.buy_times = 1
end

function LBShopData:ChangeBuffBuyTimes(detla)
    self.buff_shop_oper.buy_times = detla + self.buff_shop_oper.buy_times

    if self.buff_shop_oper.buy_times < 0 then 
        self.buff_shop_oper.buy_times = 1
    end 

    local buff_cfg = self:GetShopBuffCfg(self.buff_shop_oper.select.seq)

    if self.buff_shop_oper.buy_times > buff_cfg.limit_times then 
        self.buff_shop_oper.buy_times = buff_cfg.limit_times
    end 

    self.buff_shop_oper.flush = self.buff_shop_oper.flush + 1
end

function LBShopData:GetShopBuffCfg(seq)
    return self.buff_shop_seq_lit[seq]
end

function LBShopData:GetShopBuffSeqCfg(seq)
    return self.buff_shop_buff_seq_lit[seq]
end

function LBShopData:GetShopBuffBaseCfg(seq)
    return self.buff_shop_buff_base_seq_lit[seq]
end

function LBShopData:GetBuyBuffParam()
    if self.buff_shop_oper.select == nil then 
        return 
    end 
    local param = {}
    param.seq = self.buff_shop_oper.select.seq
    local buff_cfg = self:GetShopBuffCfg(param.seq)

    param.num = self.buff_shop_oper.buy_times
    param.name = Item.GetQuaName(buff_cfg.item.item_id)

    param.price_type = buff_cfg.price_type
    param.price = buff_cfg.price

    param.all_time_done = self.buff_shop_info.buy_times[param.seq] == buff_cfg.limit_times
    param.limit_str = Language.LBShop.BuffBuyLimitTips[buff_cfg.limit_type]

    return param 
end

function LBShopData:GetBuffShopItemList()
    local list = {}
    for k,v in pairs(self.buff_shop_info.buy_times or {}) do 
        local vo = {}
        local buff_cfg = self:GetShopBuffCfg(k-1)

        if buff_cfg ~= nil then 
            vo.seq = k-1
            vo.item_call = BagData.Instance:ItemBuild(buff_cfg.item) 
            vo.name = vo.item_call:QuaName(true)
            vo.price_type = HuoBi[buff_cfg.price_type]
            vo.limit_type = buff_cfg.limit_type
            vo.all_time_done = v == buff_cfg.limit_times
            vo.price = buff_cfg.price

            local last_time = buff_cfg.limit_times - v
            vo.limit_str = buff_cfg.limit_times > 0 and  string.format(Language.LBShop.BuffBuyLimitType[buff_cfg.limit_type],last_time == 0 and ColorStr(last_time, COLORSTR.Red20) or last_time,buff_cfg.limit_times) or Language.LBShop.BuffBuyLimitType[buff_cfg.limit_type]

            table.insert(list,vo)
        end 
    end 

    return list
end

--------------------------------------------

function LBShopData:GetExchangeShopParam()
    local param = {}

    param.exchange_list = self:GetExchangeShopItemList()

    param.can_show = self.exchange_oper.select ~= nil
    if not param.can_show then 
        return param
    end 

    local ex_cfg = self:GetShopExchangeCfg(self.exchange_oper.select.seq,self.exchange_oper.select.conver_type)

    if ex_cfg == nil then 
        param.can_show = true
        return param 
    end 

    param.item_call = BagData.Instance:ItemBuild({item_id = ex_cfg.item_id,num = ex_cfg.num,is_bind = ex_cfg.is_bind}) 
    param.name = param.item_call:QuaName(true)
    param.price_type = HuoBi[ex_cfg.price_type]
    param.desc = param.item_call:TypeDesc()
    param.item_desc = param.item_call:Desc()

    local oper_type = self.exchange_oper.type == 1 and GOD_SPACE_EXPLOITS_SHOP_TYPE or FAIRY_DRAGON_SHOP_TYPE
    local times = 0
    if StoreData.Instance.count_data[oper_type] ~= nil then 
        times = StoreData.Instance.count_data[oper_type][self.exchange_oper.select.seq] or 0
    end 

    local last_time = ex_cfg.limit_convert_count - times
    param.times_str = ex_cfg.limit_convert_count > 0 and  string.format(Language.LBShop.BuffBuyDetailLimitTips[ex_cfg.limit_type],
    last_time == 0 and ColorStr(last_time, COLORSTR.Red20) or last_time ,ex_cfg.limit_convert_count) or Language.LBShop.BuffBuyDetailLimitTips[ex_cfg.limit_type]

    param.buy_times = self.exchange_oper.buy_times
    param.buy_price = "x"..ex_cfg.price

    return param
end

function LBShopData:GetExchangeShopItemList()
    if self.exchange_oper.type == nil then
        return {}
    end

    local oper_type = self.exchange_oper.type == 1 and  GOD_SPACE_EXPLOITS_SHOP_TYPE or FAIRY_DRAGON_SHOP_TYPE

    local list = {}

    for k,  v in pairs(Config.shop_cfg_auto.department_store) do 
        if v.conver_type == oper_type  then 
            local vo = {}
            vo.seq = v.seq 
            vo.item_call = BagData.Instance:ItemBuild({item_id = v.item_id,num = v.num,is_bind = v.is_bind}) 
            vo.name = vo.item_call:QuaName(true)
            vo.price_type = HuoBi[v.price_type]
            vo.limit_type = v.limit_type

            vo.conver_type = v.conver_type

            local times = 0
            if StoreData.Instance.count_data[oper_type] ~= nil then 
                times = StoreData.Instance.count_data[oper_type][vo.seq] or 0
            end 

            vo.all_time_done = v.limit_convert_count > 0 and times == v.limit_convert_count
            local last_time = v.limit_convert_count - times
            vo.price = v.price
            vo.limit_str = v.limit_convert_count > 0 and  string.format(Language.LBShop.BuffBuyLimitType[v.limit_type] ,last_time == 0 and ColorStr(last_time, COLORSTR.Red20) or last_time ,v.limit_convert_count) or Language.LBShop.BuffBuyLimitType[v.limit_type]
            table.insert(list,vo)
        end 
    end

    return list
end

function LBShopData:GetShopExchangeCfg(seq,type)
    for k, v in pairs(Config.shop_cfg_auto.department_store) do 
        if v.conver_type == type and v.seq == seq then 
            return v
        end 
    end 
    return nil -- self.exchange_shop_seq_list[seq]
end

function LBShopData:SetExchangeOperItem(data)
    self.exchange_oper.select = data
    self.exchange_oper.flush = self.exchange_oper.flush + 1
    self.exchange_oper.buy_times = 1
end

function LBShopData:ChangeExchangeOperType(type)
    self.exchange_oper.type = type

    self.exchange_oper.flush = self.exchange_oper.flush + 1
end

function LBShopData:GetBuyExchangeParam()
    if self.exchange_oper.select == nil then 
        return 
    end 
    local param = {}
    param.seq = self.exchange_oper.select.seq

    local ex_cfg = self:GetShopExchangeCfg(param.seq,self.exchange_oper.select.conver_type)

    if ex_cfg == nil then return param end 

    param.buy_seq = ex_cfg.seq
    param.num = self.exchange_oper.buy_times

    param.price_type = ex_cfg.price_type
    param.price = ex_cfg.price

    param.limit_str = Language.LBShop.BuffBuyLimitTips[ex_cfg.limit_type]

    param.convert_type = self.exchange_oper.type == 1 and GOD_SPACE_EXPLOITS_SHOP_TYPE or FAIRY_DRAGON_SHOP_TYPE

    param.all_time_done = ex_cfg.limit_convert_count > 0 and StoreData.Instance:GetLimitTimes(param.convert_type,param.seq) == ex_cfg.limit_convert_count
    param.item_id = ex_cfg.item_id
    return param 
end

function LBShopData:ChangeExchangeBuyTimes(detla)
    self.exchange_oper.buy_times = detla + self.exchange_oper.buy_times

    if self.exchange_oper.buy_times < 0 then 
        self.exchange_oper.buy_times = 1
    end 

    local ex_cfg = self:GetShopExchangeCfg(self.exchange_oper.select.seq,self.exchange_oper.select.conver_type)

    if ex_cfg == nil then return end 

    if ex_cfg.limit_convert_count > 0 and self.exchange_oper.buy_times > ex_cfg.limit_convert_count then 
        self.exchange_oper.buy_times = ex_cfg.limit_convert_count
    end 

    self.exchange_oper.flush = self.exchange_oper.flush + 1
end

function LBShopData:GetBuffShowList()
    local list = {}

    for k,v in pairs(self.buff_info.buffs) do 
        local vo = {}
        local buff_cfg = LBShopData.Instance:GetShopBuffBaseCfg(v.buff_seq)
        vo.item_show =  BagData.Instance:ItemBuild(buff_cfg.item) 
        vo.timer_str = v.remain_times > 0 and string.format(Language.LandsBetween.Desc.BuffLastTimer,v.remain_times) or Language.LandsBetween.Desc.BuffLastUnlimit
        vo.time_stamp = v.expire_timestamp
        vo.is_running = v.expire_timestamp > 0

        table.insert(list,vo)
    end 

    return list
end 

-- 本标记用于区分是神域开启还是神域外开启
function LBShopData:SetOpenMark(flag)
    self.open_mark = flag
end

function LBShopData:GetOpenMark()
    return self.open_mark
end
