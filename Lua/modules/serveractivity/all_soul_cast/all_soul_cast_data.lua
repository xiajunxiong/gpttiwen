AllSoulCastData = AllSoulCastData or BaseClass()
function AllSoulCastData:__init()
    if AllSoulCastData.Instance ~= nil then
        Debuger.LogError("[AllSoulCastData] attempt to create singleton twice!")
        return
    end
    AllSoulCastData.Instance = self

    self.act_info = SmartData.New({is_buy_jiagou = 0, day_reward_mark = 0, fetch_reward_mark = {}, login_day = 0})
end

function AllSoulCastData:__delete()
    AllSoulCastData.Instance = nil
end

function AllSoulCastData:Config()
    return Config.wanlingzhuli_auto
end
function AllSoulCastData:OtherConfig()
    return self:Config().other[1]
end

--免费奖励 购买价格
function AllSoulCastData:GetFreeReward()
    return self:OtherConfig().reward_item
end
function AllSoulCastData:GetBuyPrice()
    return self:OtherConfig().price
end

function AllSoulCastData:SetActInfo(protocol)
    -- self.act_info.login_day = protocol.login_day
    self.act_info.login_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_ALL_SOUL_COST)
    self.act_info.fetch_reward_mark = protocol.fetch_reward_mark
    self.act_info.day_reward_mark = protocol.day_reward_mark
    self.act_info.is_buy_jiagou = protocol.is_buy_jiagou
end

function AllSoulCastData:GetShowList()
    local show_list = {}
    --后边需要table.remove，所以复制下表
    local base_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ALL_SOUL_COST, self:Config().reward, "seq")
    for i, v in ipairs(base_list) do
        v.need_day = v.seq + 1
        table.insert(show_list, v)
    end
    show_list[#show_list].need_day = show_list[#show_list].need_day - 1
    return show_list
end

--免费奖励是否已领取
function AllSoulCastData:IsFreeFetched()
    return self.act_info.day_reward_mark == 1
end

--是否激活奖励
function AllSoulCastData:IsRewardActived()
    return self.act_info.is_buy_jiagou == 1
end
function AllSoulCastData:LoginDay()
    return self.act_info.login_day
end

function AllSoulCastData:IsRewardFetched(seq)
    return self.act_info.fetch_reward_mark[32 - seq] == 1
end

-----红点
function AllSoulCastData:GetRemindNum()
    if not self:IsFreeFetched() then
        return 1 
    end
    if self:IsRewardActived() then
        local show_list = self:GetShowList()
        local last_reward = table.remove(show_list, #show_list)
        local login_day = self.act_info.login_day
        for i, v in ipairs(self:GetShowList()) do
            if login_day >= v.need_day and self:IsRewardFetched(v.seq) == false then
                return 1 
            end
        end
        if login_day >= last_reward.need_day and self:IsRewardFetched(last_reward.seq) == false then
            return 1
        end
    end
    return 0
end

