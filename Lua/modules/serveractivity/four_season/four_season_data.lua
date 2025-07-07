FourSeasonData = FourSeasonData or BaseClass()
-- 四季花牌，四季宝箱，四季宝盒
-- 四季花牌操作请求类型
FourSeasonCardReqType = {
    Info = 0,       --请求下发
    Buy = 1,        --购买，p1：seq
    Free = 2,       --领取免费礼包
}

-- 四季宝盒操作请求类型
FourSeasonBoxReqType = {
    Info = 0,       --请求下发
    Buy = 1,        --购买，p1：seq
    Free = 2,       --领取免费礼包
}

-- 四季宝箱操作请求类型
FourSeasonChestReqType = {
    Info = 0,       --请求下发
    Buy = 1,        --购买，p1：seq，p2：选中的道具列表转化成2进制
    Free = 2,       --领取免费礼包
}

function FourSeasonData:__init()
    if FourSeasonData.Instance ~= nil then
        Debuger.LogError("[FourSeasonData] attempt to create singleton twice!")
        return
    end
    FourSeasonData.Instance = self
    self.card_cfg = Config.sijihuapai_auto
    self.card_other = self.card_cfg.other[1]
    self.card_data = SmartData.New()
    self.card_max_show = 3
    self.card_style_num = #self.card_cfg.activity_interface

    self.box_cfg = Config.sijibaohe_auto
    self.box_other = self.box_cfg.other[1]
    self.box_data = SmartData.New()
    self.box_max_show = 3
    self.box_style_num = #self.box_cfg.activity_interface

    self.chest_cfg = Config.sijibaoxiang_auto
    self.chest_other = self.chest_cfg.other[1]
    self.chest_data = SmartData.New()
    self.chest_max_show = 3
    self.chest_select_flag = {}

    self:ResetData()
end

function FourSeasonData:__delete()
    FourSeasonData.Instance = nil
end

function FourSeasonData:ResetData()
    self:ResetCard()
    self:ResetBox()
    self:ResetChest()
end

-- 处理元宝道具返回奖励列表
function FourSeasonData:DealItem(cfg)
    local reward = TableCopy(cfg.reward_item, 2)
    if cfg.gold_num and cfg.gold_num > 0 then
        table.insert(reward, {item_id = CommonItemId.Gold, num = cfg.gold_num, is_bind = 1})
    end
    return DataHelper.FormatItemList(reward), table.nums(reward)
end

--------------------四季花牌--------------------{
function FourSeasonData:ResetCard()
    local card = {}
    card.day_buy_times = {}
    card.act_buy_times = {}
    card.free_flag = 1
    self.card_data.info = card
end

function FourSeasonData:CardRemind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FOUR_SEASON_CARD) then
        return 0
    end
    return self.card_data.info.free_flag == 0 and 1 or 0
end

function FourSeasonData:CardBaseData()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_FOUR_SEASON_CARD, self.card_cfg.gift_configure, "type")
    local card_data = {}
    for k, v in pairs(cfg) do
        local index1, index2 = math.floor(v.seq / self.card_max_show) + 1, v.seq % self.card_max_show + 1
        if not card_data[index1] then
            card_data[index1] = {}
        end
        if not card_data[index1][index2] then
            card_data[index1][index2] = {}
        end
        card_data[index1][index2] = v
    end
    return card_data, #card_data
end

function FourSeasonData:CardTip(page)
    for k, v in pairs(self.card_cfg.page) do
        if v.page == page then
            return v.tips
        end
    end
    return self.card_cfg.page[1].tips or ""
end

function FourSeasonData:CardMaxShow()
    return self.card_max_show
end

function FourSeasonData:CardLimitTimes(cfg)
    if cfg.limit_type == 1 then
        return cfg.buy_times - self:CardActTimes(cfg.seq), cfg.buy_times
    else
        return cfg.today_times - self:CardDayTimes(cfg.seq), cfg.today_times
    end
end

function FourSeasonData:SetCardInfo(protocol)
    self.card_data.info = protocol.info
end

function FourSeasonData:CardDayTimes(seq)
    return self.card_data.info.day_buy_times[seq] or 0
end

function FourSeasonData:CardActTimes(seq)
    return self.card_data.info.act_buy_times[seq] or 0
end

function FourSeasonData:CardIsCanBuy(cfg)
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FOUR_SEASON_CARD) then
        return false
    end
    local times = self:CardLimitTimes(cfg)
    return times > 0
end

function FourSeasonData:CardStyle()
    for k, v in pairs(self.card_cfg.activity_interface) do
        if v.is_show > 0 then
            return v.interface
        end
    end
    return 1
end

function FourSeasonData:CardStyleNum()
    return self.card_style_num
end
--------------------四季花牌--------------------}

--------------------四季宝盒--------------------{
function FourSeasonData:ResetBox()
    local box = {}
    box.day_buy_times = {}
    box.act_buy_times = {}
    box.free_flag = 1
    self.box_data.info = box
end

function FourSeasonData:BoxRemind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FOUR_SEASON_BOX) then
        return 0
    end
    return self.box_data.info.free_flag == 0 and 1 or 0
end

function FourSeasonData:BoxBaseData()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_FOUR_SEASON_BOX, self.box_cfg.gift_configure, "type")
    return cfg
end

function FourSeasonData:BoxMaxShow()
    return self.box_max_show
end

function FourSeasonData:BoxLimitTimes(cfg)
    if cfg.limit_type == 1 then
        return cfg.buy_times - self:BoxActTimes(cfg.seq), cfg.buy_times
    else
        return cfg.today_times - self:BoxDayTimes(cfg.seq), cfg.today_times
    end
end

function FourSeasonData:SetBoxInfo(protocol)
    self.box_data.info = protocol.info
end

function FourSeasonData:BoxDayTimes(seq)
    return self.box_data.info.day_buy_times[seq] or 0
end

function FourSeasonData:BoxActTimes(seq)
    return self.box_data.info.act_buy_times[seq] or 0
end

function FourSeasonData:BoxIsCanBuy(cfg)
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FOUR_SEASON_BOX) then
        return false
    end
    local times = self:BoxLimitTimes(cfg)
    return times > 0
end

function FourSeasonData:BoxStyle()
    for k, v in pairs(self.box_cfg.activity_interface) do
        if v.is_show > 0 then
            return v.interface
        end
    end
    return 1
end

function FourSeasonData:BoxStyleNum()
    return self.box_style_num
end
--------------------四季宝盒--------------------}

--------------------四季宝箱--------------------{
function FourSeasonData:ResetChest()
    local chest = {}
    chest.day_buy_times = {}
    chest.act_buy_times = {}
    chest.free_flag = 1
    self.chest_data.info = chest
end

function FourSeasonData:ChestRemind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FOUR_SEASON_CHEST) then
        return 0
    end
    return self.chest_data.info.free_flag == 0 and 1 or 0
end

function FourSeasonData:ChestBaseData()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_FOUR_SEASON_CHEST, self.chest_cfg.box_configure, "type")
    table.sort(cfg, DataHelper.WeightSort("type", function(data)
        return self:ChestIsCanBuy(data) and 0 or 1000
    end))
    return cfg
end

function FourSeasonData:ChestRewardData(group)
    if not self.chest_reaward then
        self.chest_reaward = {}
        self.chest_reaward = DataHelper.Subdivision(self.chest_cfg.reward_pool, {"item_id", "type"})
    end
    return self.chest_reaward[group]
end

function FourSeasonData:ChestMaxShow()
    return self.chest_max_show
end

function FourSeasonData:ChestLimitTimes(cfg)
    if cfg.limit_type == 1 then
        return cfg.buy_times - self:ChestActTimes(cfg.seq), cfg.buy_times
    else
        return cfg.today_times - self:ChestDayTimes(cfg.seq), cfg.today_times
    end
end

function FourSeasonData:SetChestInfo(protocol)
    self.chest_data.info = protocol.info
end

function FourSeasonData:ChestDayTimes(seq)
    return self.chest_data.info.day_buy_times[seq] or 0
end

function FourSeasonData:ChestActTimes(seq)
    return self.chest_data.info.act_buy_times[seq] or 0
end

function FourSeasonData:ChestIsCanBuy(cfg)
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_FOUR_SEASON_CHEST) then
        return false
    end
    local times = self:ChestLimitTimes(cfg)
    return times > 0
end

function FourSeasonData:ResetChestSelectFlag(item_id)
    if not item_id then
        self.chest_select_flag = {}
    else
        if not TableIsEmpty(self.chest_select_flag) then
            self.chest_select_flag[item_id] = {}
        end
    end
end

function FourSeasonData:ChestSelectFlag(item_id, m_type, value)
    if value then
        if TableIsEmpty(self.chest_select_flag[item_id]) then
            self.chest_select_flag[item_id] = {}
        end
        self.chest_select_flag[item_id][m_type] = value
    else
        if item_id then
            if m_type then
                if self.chest_select_flag[item_id] then
                    return self.chest_select_flag[item_id][m_type] or 0
                end
            else
                return self.chest_select_flag[item_id]or {}
            end
        end
        return 0
    end
end
--------------------四季宝箱--------------------}