FabaoLotteryData = FabaoLotteryData or BaseClass()
FabaoLotteryData.MAX_STOVE_COUNT = 4--总共有几个炉子
FabaoLotteryData.FILTER_FILL_CAPACITY = 5 --抽奖材料填充容量
function FabaoLotteryData:__init()
    if FabaoLotteryData.Instance ~= nil then
        Debuger.LogError("[FabaoLotteryData] attempt to create singleton twice!")
        return
    end
    FabaoLotteryData.Instance = self
    self.talisman_cfg_auto = Config.talisman_cfg_auto
    self.consume_reward = self.talisman_cfg_auto.consume_reward
    self.talisman_lottery = self.talisman_cfg_auto.talisman_lottery
    self.talisman_attribute = self.talisman_cfg_auto.talisman_attribute
    self.other = self.talisman_cfg_auto.other[1]
    

    self.rate_list = {26414, 26413, 26412}
    self.fill_keys = {26412, 26413, 26414}
    self.fill_list = SmartData.New({0, 0, 0})
    self.fill_item_list = {}
    self.draw_info = SmartData.New()
    self.stove_info = SmartData.New({index = 0, count = 0})
    self.effect_action = SmartData.New({play = false, 
                            stop = false,
                            close_act_flag = false, 
                            next_display = false, 
                            next_display_color = 1,
                            show_visual_flag = false,
                            show_reward_eff = false,
                        })

    self.view_data = SmartData.New({visual_click_index = 0,
                            visual_click_trigger = false,})
    self.reward_list = SmartData.New({})
    self.fabao_reward_info = {}
    self.icon_list = {}
    self.talisman_change_list = {}

    self.delay_attri_change = {change_list = nil,title_info = nil}

    self.finish_countdown_handle = nil --炼制时间本地倒计时timer，只记最近一次即将完成的
    self:init_config()
end
function FabaoLotteryData:init_config()
    self.consume_reward_list = {}
    for i, v in ipairs(self.consume_reward) do
        self.consume_reward_list[v.store_itme] = self.consume_reward_list[v.store_itme] or {}
        table.insert(self.consume_reward_list[v.store_itme], Item.Init(v.show_item_id, v.reward_num))
    end
    -- LogDG(self.consume_reward_list)
    self.talisman_lottery_list = {}
    for index, value in ipairs(self.talisman_lottery) do
        self.talisman_lottery_list[value.store_item] = value
        self.talisman_change_list[value.color] = value.transform_num
    end
    for i, v in pairs(self.talisman_attribute) do
        self.icon_list[v.id] = v.big_icon
        self.icon_list[v.talisman_virtual_item] = v.big_icon
        self.icon_list[v.talisman_fragment_item] = v.big_fragment_icon
    end
    -- LogDG(self.talisman_lottery_list)
end

function FabaoLotteryData:ConsumeReward()
    return self.rate_list
end
function FabaoLotteryData:RewardList(item_id)
    return self.consume_reward_list[item_id]
end
function FabaoLotteryData:UpPercent(item_id)
    return self.talisman_lottery_list[item_id]
end
function FabaoLotteryData:MaterialInfo()
    local list = {}
    local num = BagData.Instance:GetNumByItemId(CommonItemId.BlueStone)
    table.insert(list, Item.Create({item_id = CommonItemId.BlueStone, num = tostring((num - self.fill_list[1]))}))
    num = BagData.Instance:GetNumByItemId(CommonItemId.PurpleStone)
    table.insert(list, Item.Create({item_id = CommonItemId.PurpleStone, num = tostring((num - self.fill_list[2]))}))
    num = BagData.Instance:GetNumByItemId(CommonItemId.OrangeStone)
    table.insert(list, Item.Create({item_id = CommonItemId.OrangeStone, num = tostring((num - self.fill_list[3]))}))
    return list
end
function FabaoLotteryData:FillList()
    return self.fill_list
end
function FabaoLotteryData:FillItemList()
    return self.fill_item_list
end
function FabaoLotteryData:TotalNum()
    local num = BagData.Instance:GetNumByItemId(CommonItemId.BlueStone)
    num = num + BagData.Instance:GetNumByItemId(CommonItemId.PurpleStone)
    num = num + BagData.Instance:GetNumByItemId(CommonItemId.OrangeStone)
    return num
end
function FabaoLotteryData:FillNum()
    local total = 0
    for _, v in ipairs(self.fill_list:Val()) do
        total = total + v
    end
    return total
end
function FabaoLotteryData:IsFullFill()
    return self:FillNum() == FabaoLotteryData.FILTER_FILL_CAPACITY
end
function FabaoLotteryData:ClearFill()
    self.fill_list:Set({0, 0, 0})
    self.fill_item_list = {}
end

function FabaoLotteryData:CalcNeedTime()
    local consume_time = 0
    for i, v in ipairs(self.fill_keys) do
        consume_time = consume_time + (self:UpPercent(v).consume_time * self:FillList()[i])
    end
    return consume_time
end
function FabaoLotteryData:SpeedItem()
    return self.other.speed_up_item
end
function FabaoLotteryData:SpeedTime()
    return self.other.speed_up_time
end
function FabaoLotteryData:IsLock(index)
    local draw_info = self:GetDrawInfo(index)
    if draw_info == nil then
        return true
    end
    local is_lock = not draw_info.is_active
    local lock_layer = nil
    if is_lock then
        lock_layer = self.other["pre_condition_" .. (index - 1)]
    end
    return is_lock, lock_layer
end
function FabaoLotteryData:LockLayer(index)
    return self.other["pre_condition_" .. (index - 1)]
end

function FabaoLotteryData:Clear()
    if self.finish_countdown_handle ~= nil then
        TimeHelper:CancelTimer(self.finish_countdown_handle)
        self.finish_countdown_handle = nil
    end
end

function FabaoLotteryData:SetDrawInfo(info)
    self.draw_info:Set(info)

    --找到最近完成的时间点本地计时用于刷新主界面红点
    if self.finish_countdown_handle ~= nil then
        TimeHelper:CancelTimer(self.finish_countdown_handle)
        self.finish_countdown_handle = nil
    end 

    local nearest_end_ts = nil
    for i=1,FabaoLotteryData.MAX_STOVE_COUNT do
        if not self:IsLock(i) then
            if self:IsFinish(i) then    --如果有已经完成的炉子，则直接不记时了
                nearest_end_ts = nil 
                break
            end
            local info = self:GetDrawInfo(i)
            local end_ts = info.end_time - info.speed_up_time
            nearest_end_ts = nearest_end_ts == nil and end_ts or math.min(end_ts,nearest_end_ts)
        end
    end

    if nearest_end_ts ~= nil then
        -- LogError("SetDrawInfo:AddCountDownTT====",nearest_end_ts,TimeHelper.GetServerTime(),nearest_end_ts-TimeHelper.GetServerTime())
        self.finish_countdown_handle = TimeHelper:AddCountDownCT(function() end,function() 
            -- LogError("CountDownCOM!!!")
                -- self.draw_info.flush_flag = not self.draw_info.flush_flag       --改变draw_info触发计算红点逻辑
                Remind.Instance:DoRemind(Mod.Fabao)
                self.finish_countdown_handle = nil
            end,nearest_end_ts)
    -- else
    --     LogError("Cancel CountDown")
    end
end
function FabaoLotteryData:DrawInfo()
    return self.draw_info
end
function FabaoLotteryData:GetDrawInfo(index)
    return self.draw_info[index]
end
function FabaoLotteryData:SetIndex(index)
    self.stove_info.index = index
end
function FabaoLotteryData:SetCount(count)
    self.stove_info.count = count
end
function FabaoLotteryData:StoveInfo()
    return self.stove_info
end
function FabaoLotteryData:EffectInfo()
    return self.effect_action
end
function FabaoLotteryData:CanUseSpeed()
    local stove_info = self:StoveInfo()
    local draw = self:GetDrawInfo(stove_info.index)
    local item_id = self:SpeedItem()
    local num = Item.GetNum(item_id)
    local need_time = draw.end_time - draw.speed_up_time
    local speed_time = self:SpeedTime()
    local count = math.ceil((need_time - TimeHelper.GetServerTime()) / speed_time)
    local max = num > count and count or num
    max = max <= 0 and 0 or max
    return max
end

function FabaoLotteryData:IsFinish(index)
    local draw = self:GetDrawInfo(index)
    if draw == nil then
        return false
    end
    local surp = draw.end_time - draw.speed_up_time - TimeHelper.GetServerTime()
    return surp <= 0
end
function FabaoLotteryData:RewardInfo()
    return self.reward_list
end
function FabaoLotteryData:SetRewardInfo(info)
    self.fabao_reward_info = info
    -- info.fabao_id
    local list = {}
    for index, value in ipairs(info.item_id) do
        if value > 0 then
            list[index] = Item.Init(value, info.item_num[index])
        end
    end
    if info.fabao_id ~= -1 then
        local cfg = FabaoData.Instance:FabaoAttrInfo(info.fabao_id)
        local is_lock = info.fabao_index == -1 --FabaoData.Instance:IsUnlock(info.fabao_id)
        local temp = Item.Init(cfg.talisman_virtual_item)
        temp.tag_str = Language.Fabao.ChengPin
        if not is_lock then
            local num = self.talisman_change_list[cfg.color]
            list[info.fabao_index + 1] = temp
            list[info.fabao_index + 1].change_item = Item.Init(cfg.talisman_fragment_item, num)
        else
            table.insert(list, temp)
        end
    end
    -- LogDG("奖励数据", list)
    self.reward_list:Set(list)
end
function FabaoLotteryData:FabaoRewardInfo()
    return self.fabao_reward_info
end
function FabaoLotteryData:ClearReward()
    self.fabao_reward_info = {}
end
function FabaoLotteryData:IconId(id)
    return self.icon_list[id]
end

--抽到法宝的属性变化需要延迟显示，这里做个缓存
function FabaoLotteryData:SetDelayAttriChange(change_list,title_info)
    self.delay_attri_change.change_list = change_list
    self.delay_attri_change.title_info = title_info
end

function FabaoLotteryData:ShowDelayAttriChange()
    if self.delay_attri_change.change_list ~= nil and self.delay_attri_change.title_info ~= nil then
        MainOtherCtrl.Instance:OpenAttrChange(self.delay_attri_change.change_list,self.delay_attri_change.title_info)
        RoleData.Instance:ChangeRoleTotalScore()
        self.delay_attri_change.title_info = nil
        self.delay_attri_change.change_list = nil
    end
end


function FabaoLotteryData:TriggerVisualStoveClick(index)
    self.view_data.visual_click_index = index
    self.view_data.visual_click_trigger = not self.view_data.visual_click_trigger
end

function FabaoLotteryData:ViewData()
    return self.view_data
end


--计算主界面红点函数
function FabaoLotteryData:RemindDrawInfo()
    --是否有炉子已完成
    for i=1,FabaoLotteryData.MAX_STOVE_COUNT do
        if self:IsLock(i) then
            break
        end
        local info = self:GetDrawInfo(i)
        if info ~= nil and info.material_num > 0 then
            if self:IsFinish(i) then 
                return 1
            end
        end   
    end
    return self:RemindBagData()
end


function FabaoLotteryData:RemindBagData()

    local has_emtpy = false
    ---看有没有空炉子
    for i=1,FabaoLotteryData.MAX_STOVE_COUNT do
        if self:IsLock(i) then
            break
        end
        local info = self:GetDrawInfo(i)
        if info and info.material_num == 0 then
            has_emtpy = true
            break
        end
    end
    if has_emtpy == false then
        return 0
    end

    --材料是否足够
    -- local mat_num = BagData.Instance:GetNumByItemId(CommonItemId.BlueStone)
    -- if mat_num < FabaoLotteryData.FILTER_FILL_CAPACITY then
    --     mat_num = mat_num + BagData.Instance:GetNumByItemId(CommonItemId.PurpleStone)
    --     if mat_num < FabaoLotteryData.FILTER_FILL_CAPACITY then
    --         mat_num = mat_num + BagData.Instance:GetNumByItemId(CommonItemId.OrangeStone)
    --         if mat_num < FabaoLotteryData.FILTER_FILL_CAPACITY then
    --             return 0
    --         end
    --     end
    -- end
    -- return 1
    return self:RemindIsMaterialEnough()
end

function FabaoLotteryData:RemindIsMaterialEnough()
    local mat_num = BagData.Instance:GetNumByItemId(CommonItemId.BlueStone)
    if mat_num < FabaoLotteryData.FILTER_FILL_CAPACITY then
        mat_num = mat_num + BagData.Instance:GetNumByItemId(CommonItemId.PurpleStone)
        if mat_num < FabaoLotteryData.FILTER_FILL_CAPACITY then
            mat_num = mat_num + BagData.Instance:GetNumByItemId(CommonItemId.OrangeStone)
            if mat_num < FabaoLotteryData.FILTER_FILL_CAPACITY then
                return 0
            end
        end
    end
    return 1    
end