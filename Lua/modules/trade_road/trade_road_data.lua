TradeRoadData = TradeRoadData or BaseClass()
function TradeRoadData:__init()
    if TradeRoadData.Instance ~= nil then
        Debuger.LogError("[TradeRoadData] attempt to create singleton twice!")
        return
    end
    TradeRoadData.Instance = self

    self.is_to_next_city = false

    --总信息
    self.road_info = SmartData.New()
    --城市列表：数量，城市信息，价格信息
    self.city_info = SmartData.New({count = 0, list = {}, price_list = {}})
    --记录列表：数量，记录列表
    self.record_info = SmartData.New({count = 0, list = {}})
    --排行信息：数量，列表
    self.rank_info = SmartData.New({count = 0, list = {}})

    self.map_move = SmartData.New({flush = false})
    self.over_smart = SmartData.New({flush = false})

    self.item_show_list = {}
end

function TradeRoadData:__delete()
    TradeRoadData.Instance = nil
end

function TradeRoadData:Config()
    return Config.silk_roads_auto
end
function TradeRoadData:OtherConfig()
    return self:Config().other[1]
end

function TradeRoadData:GetAcceptNpcSeq()
    return self:OtherConfig().act_npc -- 586
end
function TradeRoadData:GetCityCfg(seq)
    return self:Config().city_cfg[seq]
end
function TradeRoadData:GetBuffCfg(skill_id)
    return self:Config().silk_roads_skill[skill_id]
end

--是否打开地图后前往下个城市
function TradeRoadData:SetToNextCity()
    self.map_move.flush = not self.map_move.flush
end

function TradeRoadData:SetOverSmart()
    self.over_smart.flush = not self.over_smart.flush
end

--是否正在打游戏
function TradeRoadData:IsJoined()
    return self.road_info.cur_city_index ~= nil and self.road_info.cur_city_index > -1
end

--已参与次数，最大可参与次数
function TradeRoadData:JoinTimes()
    return self.road_info.finish_num or 0
end
function TradeRoadData:CanJoinTimes()
    return self:OtherConfig().act_num
end

--地图适配坐标
function TradeRoadData.ConverPos(point)
    local origin = 1425 / 750
    local current = UnityEngine.Screen.width / UnityEngine.Screen.height
    if IS_IPHONEX or EditorSettings:SimularIphoneX() then
        current = (UnityEngine.Screen.width - 120) / UnityEngine.Screen.height
    end
    if current > origin then
        point.x = point.x * current / origin
    else
        point.y = point.y * origin / current
    end
    return point
end

----------总信息
function TradeRoadData:SetAllInfo(protocol)
    self.road_info.cumulation_coin_count = protocol.cumulation_coin_count
    self.road_info.cumulation_trading_count = protocol.cumulation_trading_count
    self.road_info.finish_num = protocol.finish_num

    self.road_info.line_group_id = protocol.line_group_id
    self.road_info.buy_warehouse_count = protocol.buy_warehouse_count
    self.road_info.buy_supply_count = protocol.buy_supply_count
    self.road_info.have_coin_count = protocol.have_coin_count
    self.road_info.have_supply_count = protocol.have_supply_count

    self.road_info.init_skill = protocol.init_skill
    self.road_info.cur_city_index = protocol.cur_city_index
    self.road_info.trading_num = protocol.trading_num
    self.road_info.news_pool_count = protocol.news_pool_count

    --格子信息
    self.road_info.cur_max_grid_count = protocol.cur_max_grid_count
    self.road_info.cur_has_commodity_count = protocol.cur_has_commodity_count
    self.road_info.grid_list = protocol.grid_list

    --日志信息
    self.record_info.count = protocol.trading_log_count
    self.record_info.list = protocol.trading_log_list

    --城镇信息
    self.city_info.count = protocol.city_count
    self.city_info.list = protocol.city_list
    self.city_info.price_list = protocol.item_price_list
end

--活动结束清除信息
function TradeRoadData:ClearInfo()
    self.road_info.cur_city_index = -1
    self.road_info.init_skill = 0
end

--资金，补给，最大仓位
function TradeRoadData:SetCurrencyInfo(protocol)
    self.road_info.have_coin_count = protocol.have_coin_count
    self.road_info.have_supply_count = protocol.have_supply_count
    self.road_info.cur_max_grid_count = protocol.cur_max_grid_count
end

function TradeRoadData:OnReady(protocol)
    self.road_info.init_skill = protocol.param1
    self.road_info.line_group_id = protocol.param2
end

--开始，继续出发
function TradeRoadData:OnStartOutset(protocol)
    self.road_info.cur_city_index = protocol.param1
end
function TradeRoadData:OnKeepGoing(protocol)
    self.road_info.cur_city_index = protocol.param1
    self.road_info.trading_num = self.road_info.trading_num + 1
    self.road_info.news_pool_count = protocol.param2
end
function TradeRoadData:GetTradingNum()
    return self.road_info.trading_num
end
function TradeRoadData:GetTradingMaxNum()
    return self:OtherConfig().max_deal_num
end

--购买消息，背包，补给
function TradeRoadData:OnBuyNews(protocol)
    local city_info = self:CurCityInfo()
    if city_info ~= nil then
        city_info.buy_news_count = protocol.param1
    end
    self.road_info.news_pool_count = protocol.param2
end
function TradeRoadData:OnBuyBag(protocol)
    self.road_info.buy_warehouse_count = protocol.param1
end
function TradeRoadData:OnBuySupply(protocol)
    self.road_info.buy_supply_count = protocol.param1
end

--背包物品改变
function TradeRoadData:OnGridChange(protocol)
    self.road_info.cur_has_commodity_count = self.road_info.cur_has_commodity_count + protocol.commodity_count
    local old_data = self.road_info.grid_list[protocol.grid_index]
    if old_data ~= nil then
        self.road_info.cur_has_commodity_count = self.road_info.cur_has_commodity_count - old_data.commodity_count
    end
    if protocol.commodity_count == 0 then
        self.road_info.grid_list[protocol.grid_index] = nil
    else
        if old_data == nil then
            self.road_info.grid_list[protocol.grid_index] = {}
        end
        local data = self.road_info.grid_list[protocol.grid_index]
        data.grid_index = protocol.grid_index
        data.commodity_seq = protocol.commodity_seq
        data.commodity_count = protocol.commodity_count
        data.commodity_cost = protocol.commodity_cost
    end
end

function TradeRoadData:OnQuitGame(protocol)
    self:ClearInfo()
end

--当前所在城市和路线
function TradeRoadData:CurCityInfo()
    local cur_index = self.road_info.cur_city_index or -1
    return self.city_info.list[cur_index]
end
function TradeRoadData:CurCitySeq()
    local city_info = self:CurCityInfo()
    return city_info and city_info.city_map_seq or 0
end
function TradeRoadData:CurLine()
    return self.road_info.line_group_id
end

--当前城市配置
function TradeRoadData:GetCurCityCfg()
    return self:GetCityCfg(self:CurCitySeq())
end

--背包容量和已使用大小
function TradeRoadData:CurBagMaxSize()
    return self.road_info.cur_max_grid_count or 0
end
function TradeRoadData:CurBagUseSize()
    return self.road_info.cur_has_commodity_count or 0
end

--背包是否还有空位
function TradeRoadData:IsHasBagEmpty()
    return self:CurBagUseSize() < self:CurBagMaxSize()
end

--剩余资金，补给
function TradeRoadData:CurMoney()
    return self.road_info.have_coin_count or 0
end
function TradeRoadData:CurSupply()
    return self.road_info.have_supply_count or 0
end

function TradeRoadData:CurBuffId()
    return self.road_info.init_skill or 0
end

function TradeRoadData:GetInitPrice()
    local money = self:OtherConfig().start_icon
    local buff_cfg = self:CurBuffCfg()
    if buff_cfg and buff_cfg.silk_roads_skill == 2 then
        money = money + buff_cfg.silk_roads_skill_num
    end
    return money
end

----------城镇信息
function TradeRoadData:GetMapAllList()
    local show_list = {}
    for i, v in ipairs(self:Config().city_cfg) do
        if v.is_show == 1 then
            table.insert(show_list, v.seq)
        end
    end
    return show_list
end

function TradeRoadData:GetMapLineCfg()
    local group_cfg = self:Config().line[self:CurLine()]
    if group_cfg.city_list == nil then
        group_cfg.city_list = DataHelper.SplitList(group_cfg.line, "|")
        table.insert(group_cfg.city_list, 1, self:OtherConfig().start_city)

        --不会经过一个城镇两次，
        group_cfg.city_table = {}
        for i, v in ipairs(group_cfg.city_list) do
            local data = {index = i}
            data.last_seq = group_cfg.city_list[i - 1]
            data.next_seq = group_cfg.city_list[i + 1]
            data.last_index = data.last_seq ~= nil and i - 1 or nil
            data.next_index = data.next_seq ~= nil and i + 1 or nil
            group_cfg.city_table[v] = data
        end
    end
    return group_cfg
end

function TradeRoadData:GetMapList()
    local group_cfg = self:GetMapLineCfg()
    return group_cfg.city_list
end

--获取下个城市seq，返回：数组下标，城市seq
function TradeRoadData:GetNextCitySeq(seq)
    local city_data = self:GetMapLineCfg().city_table[self:CurCitySeq()]
    if city_data ~= nil and city_data.next_seq ~= nil then
        return city_data.next_index, city_data.next_seq
    end
end

--是否已经过该城市
function TradeRoadData:IsCityPass(seq)
    local city_table = self:GetMapLineCfg().city_table
    return city_table[seq].index < city_table[self:CurCitySeq()].index
end
function TradeRoadData:IsCurLastCity()
    local city_data = self:GetMapLineCfg().city_table[self:CurCitySeq()]
    return city_data.next_seq == nil
end

function TradeRoadData:GetBeforeCitySeq(seq)
    local city_table = self:GetMapLineCfg().city_table
    local cur_seq = seq or self:CurCitySeq()
    return city_table[cur_seq].last_seq
end

----------记录列表
function TradeRoadData:SetRecordList(protocol)
    self.record_info.count = protocol.trading_log_count
    self.record_info.list = protocol.trading_log_list
end

function TradeRoadData:AddRecordInfo(protocol)
    self.record_info.count = self.record_info.count + 1
    table.insert(self.record_info.list, protocol.trading_log_info)
end

function TradeRoadData:GetRecordList()
    return self.record_info.list or {}
end

function TradeRoadData:GetFreightCfg(type, freight_id)
    for i, v in pairs(self:Config().shopping) do
        if v.type == type and v.freight_id == freight_id then
            return v
        end
    end
end

----------排行信息
function TradeRoadData:SetRankInfo(protocol)
    self.rank_info.count = protocol.rank_count
    self.rank_info.list = protocol.rank_list
end

function TradeRoadData:GetRankList()
    return self.rank_info.list or {}
end

function TradeRoadData:GetRankReward(rank, is_fill)
    for i, v in pairs(self:Config().ranking_list) do
        if v.levels_min <= rank and v.levels_max >= rank then
            return v
        end
    end
    if is_fill then
        return self:Config().ranking_list[#self:Config().ranking_list]
    end
end

--自己排行
function TradeRoadData:GetRank()
    local item = DataHelper.GetSearch(self.rank_info.list, "role_id", RoleData.Instance:GetRoleId())
    return item ~= nil and item.index or -1
end

----------分红信息
function TradeRoadData:GetRewardShowList()
    return table.rearrange2(self:Config().settlement_box)
end

--总收益
function TradeRoadData:GetRewardPrice()
    --背包物品在这个城镇全部出售的价值 + 玩家当前身上玉币的数量
    local cur_money = self:CurMoney()
    for i, v in pairs(self:GetBagItemList()) do
        local item_info = self:GetCurCityItemInfo(v.commodity_seq)
        if item_info ~= nil then
            cur_money = cur_money + item_info.price * v.commodity_count
        else
            local item_cfg = self:GetItemCfg(v.commodity_seq)
            cur_money = cur_money + item_cfg.freight_market_price * v.commodity_count
        end
    end
    return cur_money
end

--获取累计资金，贸易次数
function TradeRoadData:GetCumulationMoneyCount()
    return self.road_info.cumulation_coin_count or 0
end
function TradeRoadData:GetCumulationTradingCount()
    return self.road_info.cumulation_trading_count or 0
end

--当前城镇消息列表
function TradeRoadData:GetCurCityRecordList()
    local show_list = {}
    for i, v in ipairs(self:GetRecordList()) do
        if v.from_city_map_seq == self:CurCitySeq() then
            table.insert(show_list, v)
        end
    end
    return show_list
end

function TradeRoadData:GetTargetCityRecordList(target_seq, type_list)
    local show_list = {}
    for i, v in ipairs(self:GetRecordList()) do
        if v.target_city_map_seq == target_seq then
            if type_list == nil or type_list[v.log_type] ~= nil then
                table.insert(show_list, v)
            end
        end
    end
    return show_list
end

----------购买背包
function TradeRoadData:CurBuyBagTimes()
    return self.road_info.buy_warehouse_count or 0
end
function TradeRoadData:MaxBuyBagTimes()
    return #self:Config().buy_warehouse_price
end

function TradeRoadData:GetBuyBagPrice(buy_num)
    local config = self:Config().buy_warehouse_price
    local price = 0
    local cur_num = self:CurBuyBagTimes()
    for i = cur_num + 1, cur_num + buy_num do
        price = price + (config[i] and config[i].price or 0)
    end
    return price
end

function TradeRoadData:GetSupplyExpend()
    local use_size = TradeRoadData.Instance:CurBagUseSize()
    local cost = self:OtherConfig().start_supply_num
    if use_size > 0 then
        cost = cost + self:Config().warehouse_expend[use_size].supply_expend
    end
    return cost
end

----------购买体力
function TradeRoadData:CurBuySupplyTimes()
    return self.road_info.buy_supply_count or 0
end
function TradeRoadData:MaxBuySupplyTimes()
    return #self:Config().buy_supply_peice
end

function TradeRoadData:GetBuySupplyPrice(buy_num)
    local config = self:Config().buy_supply_peice
    local price = 0
    local cur_num = self:CurBuySupplyTimes()
    for i = cur_num + 1, cur_num + buy_num do
        price = price + (config[i] and config[i].price or 0)
    end
    return price
end

----------购买消息
function TradeRoadData:CurBuyMessageTimes()
    local city_info = self:CurCityInfo() or {}
    return city_info.buy_news_count or 0
end
function TradeRoadData:MaxBuyMessageTimes()
    return #self:Config().buy_news
end

function TradeRoadData:GetBuyMessagePrice()
    local buy_num = self:IsCanBuyMessage() and self:CurBuyMessageTimes() + 1 or self:MaxBuyMessageTimes()
    local config = self:Config().buy_news[buy_num]
    return config and config.price or nil
end

function TradeRoadData:IsCanBuyMessage()
    return self:CurBuyMessageTimes() < self:MaxBuyMessageTimes()
end

function TradeRoadData:GetRemainNews()
    return self.road_info.news_pool_count or 0
end

----------城市物品列表
function TradeRoadData:GetShopList()
    local city_cfg = self:GetCurCityCfg()
    if city_cfg.item_list == nil then
        city_cfg.item_list = DataHelper.SplitList(city_cfg.city_specialty, "|")
    end
    return city_cfg.item_list
end

function TradeRoadData:GetItemCfg(seq)
    return self:Config().commodity[seq]
end

function TradeRoadData:GetCityItemInfo(city_map_seq, commodity_seq)
    city_map_seq = city_map_seq or self:CurCitySeq()
    if self.city_info.price_list[city_map_seq] then
        return self.city_info.price_list[city_map_seq][commodity_seq]
    end
    return nil
end

function TradeRoadData:GetCurCityItemInfo(commodity_seq)
    return self:GetCityItemInfo(self:CurCitySeq(), commodity_seq)
end

function TradeRoadData:GetBeforeCityItemPrice(city_seq, commodity_seq)
    local before_seq = self:GetBeforeCitySeq(city_seq)
    if before_seq ~= nil then
        local item_info = self:GetCityItemInfo(before_seq, commodity_seq)
        return item_info ~= nil and item_info.price or -1
    end
    return -1
end

--param: city_seq当前城市，commodity_seq目标商品
--return rate_val倍率
function TradeRoadData:GetItemPriceRate(city_seq, commodity_seq)
    local before_price = self:GetBeforeCityItemPrice(city_seq, commodity_seq)
    if before_price ~= -1 then
        local item_info = self:GetCityItemInfo(city_seq, commodity_seq)
        local rate_val = (item_info.price - before_price) / before_price * 100
        return math.floor(rate_val)
    end
    return 0
end

----------背包物品列表
--物品成本价计算方式：以平均数的形式存在玩家背包里，服务端下发小数，客户端四舍五入取整
function TradeRoadData:GetBagItemList()
    local item_list = {}
    for i, v in pairs(self.road_info.grid_list) do
        if v.commodity_count > 0 then
            table.insert(item_list, v)
        end
    end
    table.sort(item_list, DataHelper.SortFunc("grid_index"))
    return item_list
end

function TradeRoadData:GetBagItemInfo(seq)
    local item = DataHelper.GetSearch(self.road_info.grid_list, "commodity_seq", seq)
    return item
end

function TradeRoadData:GetBagItemByIndex(index)
    return self.road_info.grid_list[index]
end

function TradeRoadData:AddShowItem(data)
    table.insert(self.item_show_list, data)
end
function TradeRoadData:ClearShowItem()
    self.item_show_list = {}
end
function TradeRoadData:GetOneShowItem()
    return table.remove(self.item_show_list, 1)
end
function TradeRoadData:HasReward()
    return #self.item_show_list > 0
end

----------Buff信息
function TradeRoadData:CurBuffCfg()
    return self:GetBuffCfg(self:CurBuffId())
end

--补给消耗减少
function TradeRoadData:GetSupplyCost(value)
    local buff_cfg = self:CurBuffCfg()
    if buff_cfg and buff_cfg.silk_roads_skill == 3 then
        return value - math.floor(buff_cfg.silk_roads_skill_num / 100 * value)
    end
    return value
end

--酒馆打赏消耗
function TradeRoadData:GetTavernRate()
    local buff_cfg = self:CurBuffCfg()
    if buff_cfg and buff_cfg.silk_roads_skill == 4 then
        return 1 - buff_cfg.silk_roads_skill_num / 100
    end
    return 1
end
function TradeRoadData:GetTavernCost(value)
    return math.floor(value * self:GetTavernRate())
end

--购物消耗
function TradeRoadData:GetBuyRate()
    -- local buff_cfg = self:CurBuffCfg()
    return 1
end
function TradeRoadData:GetBuyCost(value)
    return math.floor(value * self:GetBuyRate())
end

----------前往酒馆提示
function TradeRoadData:IsTavernRemind()
    local save_key = "TradeRoadTavernRemind" .. RoleData.Instance:GetRoleId()
    local save_val = PrefsString(save_key)
    local temp_val = ActivityData.GetSwitchStampTime(ActType.TradeRoad) .. "-" .. self:JoinTimes()
    return save_val ~= temp_val
end

function TradeRoadData:SetTavernRemind(is_remind)
    local save_key = "TradeRoadTavernRemind" .. RoleData.Instance:GetRoleId()
    if is_remind then
        PrefsString(save_key, "")
    else
        local save_val = ActivityData.GetSwitchStampTime(ActType.TradeRoad) .. "-" .. self:JoinTimes()
        PrefsString(save_key, save_val)
    end
end
