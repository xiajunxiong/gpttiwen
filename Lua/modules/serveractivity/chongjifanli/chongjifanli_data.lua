ChongJiFanLiData = ChongJiFanLiData or BaseClass()

function ChongJiFanLiData:__init()
    if ChongJiFanLiData.Instance ~= nil then
        Debuger.LogError("[ChongJiFanLiData] attempt to create singleton twice!")
        return
    end
    ChongJiFanLiData.Instance = self
    self.chongjifanli_smart = SmartData.New()

    -- -- 红点刷新
    self.remind_data = SmartData.New({v = false})
end

function ChongJiFanLiData:ClearData()
    --self.first_remind = false
end


function ChongJiFanLiData:SetChongJiFanLiInfo(protocol)
    self.chongjifanli_smart:Set(protocol.info)
    self:RemindNotice()
end

function ChongJiFanLiData:GetRewardCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_CHONGJIFANLI, Config.chongjifanli_auto.reward)
    return cfg
end

function ChongJiFanLiData:GetChongJiFanLiItemList()
    local list = self:GetRewardCfg()
    return list
end


-- 获取奖励标记 -1已领取，0未达成 1可领取
function ChongJiFanLiData:GetRewardFlag(data)
    if TableIsEmpty(self.chongjifanli_smart) then
        return 0
    end
    if self.chongjifanli_smart.reward_fetch_flag[data.seq] == 1 then
        return -1
    end

    if self.chongjifanli_smart.meet_cond_day_num >= data.day then
        return 1
    else
        return 0
    end
end


-- 活动是否开启
function ChongJiFanLiData:IsOpen()
    if RoleLevel() < FunOpen.Instance:GetFunOpenLevel(Mod.Reward.ChongJiFanLi)then
        return false
    end
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_CHONGJIFANLI) then
		return false
    end
    if TableIsEmpty(self.chongjifanli_smart.reward_fetch_flag) then
        return false
    end
    local cfg = self:GetRewardCfg()
    for i,v in pairs(cfg) do
        if self.chongjifanli_smart.reward_fetch_flag[v.seq] == 0 then
            return true
        end
    end

    return false
end

-- 充值多少天
function ChongJiFanLiData:GetMeetDay()
    if TableIsEmpty(self.chongjifanli_smart) then
        return 0
    end
    return self.chongjifanli_smart.meet_cond_day_num or 0
end

-- 当天是否充值了
function ChongJiFanLiData:IsDayMeetFlag()
    if TableIsEmpty(self.chongjifanli_smart) then
        return false
    end
    return self.chongjifanli_smart.is_day_meet_flag == 1
end

-- 获取奖励预览
function ChongJiFanLiData:GetPreviewItemList()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_CHONGJIFANLI, Config.chongjifanli_auto.reward)
    local mark_item_list = {}
    local item_list = {}
    local list = {}
    for i,v in pairs(cfg) do
        if not TableIsEmpty(v.reward1_item) then
            local item_id = v.reward1_item.item_id
            if not mark_item_list[item_id] then
                mark_item_list[item_id] = {item_id = item_id, num = 0, mark = 1}
                table.insert(list,mark_item_list[item_id])
            end
            local data = mark_item_list[item_id]
            data.num = data.num + v.reward1_item.num
        end
        for j,v2 in pairs(v.reward2_item) do
            local item_id = v2.item_id
            if not item_list[item_id] then
                item_list[item_id] = {item_id = item_id, num = 0, mark = 0}
                table.insert(list,item_list[item_id])
            end
            local data = item_list[item_id]
            data.num = data.num + v2.num
        end
    end

    table.sort(list, function (a,b)
        return a.mark > b.mark
    end)
    return list
end

-- 获取下一个大奖配置，还需要多少天
function ChongJiFanLiData:GetNextMarkReward()
    local day = self:GetMeetDay()
    local reward_list = self:GetRewardCfg()
    for i, v in pairs(reward_list) do
        if day < v.day and v.mark == 1 then
            return v, v.day - day
        end
    end
    return reward_list[#reward_list],0
end


-- 红点通知 取反 相当于setdirty
function ChongJiFanLiData:RemindNotice()
    self.remind_data.v = not self.remind_data.v
end

-- 红点逻辑
function  ChongJiFanLiData:RemindNum()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_CHONGJIFANLI) then
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


function ChongJiFanLiData:CanGetRemind()
    local meet_cond_day_num =  self:GetMeetDay()
    local reward_cfg = self:GetRewardCfg()

    for i,v in pairs(reward_cfg) do
        local flag = self:GetRewardFlag(v)
        if flag > 0 then
            return 1
        end
    end
    return 0
end


function ChongJiFanLiData:GetOffset()
    if TableIsEmpty(self.chongjifanli_smart) then
        return 0
    end
    for i=0, #(self.chongjifanli_smart.reward_fetch_flag)-1 do  
        if self.chongjifanli_smart.reward_fetch_flag[i] == 0 then
            return i
        end
    end
    return 0
end





