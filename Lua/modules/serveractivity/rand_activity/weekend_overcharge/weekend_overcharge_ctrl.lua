VIEW_REQ("modules/serveractivity/rand_activity/weekend_overcharge/weekend_overcharge_view")

WeekendOverChargeCtrl = WeekendOverChargeCtrl or BaseClass(BaseCtrl)
function WeekendOverChargeCtrl:__init()
	if WeekendOverChargeCtrl.Instance ~= nil then
		Debuger.LogError("[WeekendOverChargeCtrl] attempt to create singleton twice!")
		return
	end	
	WeekendOverChargeCtrl.Instance = self
	self.data = WeekendOverChargeData.New()
	self:RegisterAllProtocols()
end

function WeekendOverChargeCtrl:__delete()
	self:UnRegisterAllProtocols()
	WeekendOverChargeCtrl.Instance = nil
	Delete(self, "data")
end

function WeekendOverChargeCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAWeekendLianChongInfo, "OnRAWeekendLianChongInfo")
    self:RegisterProtocol(SCRAWeekendLianChongSingleInfo, "OnRAWeekendLianChongSingleInfo")
end

function WeekendOverChargeCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCRAWeekendLianChongInfo)
    self:UnRegisterProtocol(SCRAWeekendLianChongSingleInfo)
end

function WeekendOverChargeCtrl:OnInit()
    self:OnRemind()
end

function WeekendOverChargeCtrl:OnRemind()
    Remind.Instance:Register(Mod.RandActivity.WeekendOverCharge, self.data.WeekendInfo, function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_ZHOU_MO_LIAN_CHONG_LI) then
			return 0
		end
		return self.data:GetRemind()
	end)
end

function WeekendOverChargeCtrl:OnRAWeekendLianChongInfo(protocol)
    LogDG("?杰哥哥@这是周末连充礼 信息",protocol)
    self.data:SetWeekendLianChongInfo(protocol)
    -- self.data:SetLimitRechargeInfo(protocol)
end

function WeekendOverChargeCtrl:OnRAWeekendLianChongSingleInfo(protocol)
    LogDG("?杰哥哥@这是周末连充礼 单条信息",protocol)
    self.data:SetWeekendLianChongSingleInfo(protocol)
    -- self.data:SetLimitRechargeInfo(protocol)
end

function WeekendOverChargeCtrl:SendInfo()
    LogDG("杰哥哥@这是周末连充礼请求信息哦")
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ZHOU_MO_LIAN_CHONG_LI, 0)
end

function WeekendOverChargeCtrl:SendReward(param)
    LogDG("杰哥哥@这是周末连充礼请求领取天奖励哦",param)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ZHOU_MO_LIAN_CHONG_LI, 1,param.seq,param.index -1)
end

function WeekendOverChargeCtrl:SendExtraReward(param)
    LogDG("杰哥哥@这是周末连充礼请求领取额外奖励哦",param)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_ZHOU_MO_LIAN_CHONG_LI, 2,param.seq)
end
--------------------WeekendOverChargeData--------------------
WeekendOverChargeData = WeekendOverChargeData or BaseClass()

function WeekendOverChargeData:__init()
	if WeekendOverChargeData.Instance ~= nil then
		Debuger.LogError("[WeekendOverChargeData] attempt to create singleton twice!")
		return
	end
	WeekendOverChargeData.Instance = self
    self.WeekendInfo = SmartData.New({flush = 0})
end

function WeekendOverChargeData:GetRemind()
    local list = self:GetConfigList()
    for day_index = 0,2 do 
        for k,v in pairs(list) do 
            if self.WeekendInfo.day_can_fetch_flag_list[day_index][v.seq] == 1 and 
                self.WeekendInfo.day_fecth_flag_list[day_index][v.seq] == 0 then 
                return 1
            end 
        end 
    end 


    -- 额外奖励的开启需要连续三天为可领取or已领取
    for  k,v in pairs(list) do 
        local show_lock_flag = false
        for i = 1,3 do 
            if self.WeekendInfo.day_fecth_flag_list[i-1][v.seq] == 0 then 
                show_lock_flag = true
                break
            end 
        end 
        if self.WeekendInfo.extra_reward_flag[v.seq] == 0 and not show_lock_flag then 
            return 1
        end 
    end 
    
    return 0
    -- return 1
end

function WeekendOverChargeData:SetWeekendLianChongInfo(protocol)
    self.WeekendInfo.day_chong_zhi_num = protocol.day_chong_zhi_num
    self.WeekendInfo.day_can_fetch_flag_list = protocol.day_can_fetch_flag_list
    self.WeekendInfo.day_fecth_flag_list = protocol.day_fecth_flag_list
    self.WeekendInfo.extra_reward_flag = protocol.extra_reward_flag

    self.WeekendInfo.flush = self.WeekendInfo.flush + 1
end

function WeekendOverChargeData:SetWeekendLianChongSingleInfo(protocol)
    if protocol.notice_type == 0 then 
        self.WeekendInfo.day_can_fetch_flag_list[protocol.day_index][protocol.seq] = protocol.can_fetch_flag
        self.WeekendInfo.day_fecth_flag_list[protocol.day_index][protocol.seq] = protocol.fetch_flag
    else 
        self.WeekendInfo.extra_reward_flag[protocol.seq] = protocol.fetch_flag
    end 

    self.WeekendInfo.flush = self.WeekendInfo.flush + 1
end

function WeekendOverChargeData:GetTotalChargeMoney()
    return self.WeekendInfo.day_chong_zhi_num --RechargeData.Instance:GoldToMoney(self.WeekendInfo.day_chong_zhi_num)
end

function WeekendOverChargeData:GetConfigList()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_ZHOU_MO_LIAN_CHONG_LI,Config.zhoumolianchongli_auto.reward,"seq")
end

function WeekendOverChargeData:GetOperShowList()
    local list = self:GetConfigList()
    
    local fix_list = {}
    for k,v in pairs(list) do 
        local seq_info = {}
        local money = RechargeData.Instance:GoldToMoney(v.acc_price)
        seq_info.condition_str = string.format(Language.WeekendOverCharge.Condition,money)
        seq_info.day_1_show = self:GetDayShow(1,v.seq,v.reward1_item)
        seq_info.day_2_show = self:GetDayShow(2,v.seq,v.reward2_item)
        seq_info.day_3_show = self:GetDayShow(3,v.seq,v.reward3_item)
        seq_info.day_countine = self:GetCountineShow(v.seq,v.reward4_item)

        table.insert(fix_list,seq_info)
    end 

    return fix_list
end

-- 这里的index是1~3
function WeekendOverChargeData:GetDayShow(index,seq,config)
    local info = {}
    info.item_show = Item.Create(config[0])

    info.show_get = self.WeekendInfo.day_fecth_flag_list[index-1][seq] == 1
    info.show_lock = self:IsToday(index) >= 0 and self.WeekendInfo.day_can_fetch_flag_list[index-1][seq] == 0
    info.show_supply = self:IsToday(index) < 0 and self:CheckIsShowSupply(index,seq)
    info.supply_str =  self:SupplyChargeNeed(index,seq)
    info.can_oper = self:OperShowCheck(index,seq)
    local is_today = self:IsToday(index) == 0
    local is_today_done = self:TodayDone()
    info.show_eff = self.WeekendInfo.day_can_fetch_flag_list[index-1][seq] == 1 and self.WeekendInfo.day_fecth_flag_list[index-1][seq] == 0 -- and ((is_today ) or (not is_today and is_today_done))

    info.send_data = {
        index = index,
        seq = seq,
        is_extra = false,
    }

    return info
end

function WeekendOverChargeData:GetCountineShow(seq,config)
    local info = {}
    info.item_show = Item.Create(config[0])

    info.show_get = self.WeekendInfo.extra_reward_flag[seq] == 1
    local show_lock_flag = false
    -- 额外奖励的开启需要连续三天为可领取or已领取
    for i = 1,3 do 
        if self.WeekendInfo.day_can_fetch_flag_list[i-1][seq] == 0 then 
            show_lock_flag = true
            break
        end 
    end 
    
    info.show_lock = show_lock_flag
    info.show_supply = false
    info.supply_str = ""
    info.can_oper = not show_lock_flag
    info.show_eff = not show_lock_flag and self.WeekendInfo.extra_reward_flag[seq] == 0 -- and self:TodayDone()

    info.send_data = {
        seq = seq,
        is_extra = true,
    }
    return info
end

-- return 1~3
function WeekendOverChargeData:GetTodayIndex()
    local server_time = TimeManager:GetServerTime()
    local start_time = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_ZHOU_MO_LIAN_CHONG_LI)
    local server_t = TimeHelper.FormatUnixTime2Date(server_time)
    local start_t = TimeHelper.FormatUnixTime2Date(start_time)
    local day_index = (server_t.day - start_t.day)+ 1

    -- LogError("?asdf",server_t,start_t)
    -- LogError("?day_index",day_index)
    return day_index
end

-- 0 是今天 ，<0是昨天or前天  >0是明天or后天
function WeekendOverChargeData:IsToday(check_day_index)
    local day_index = self:GetTodayIndex()
    local check = check_day_index - day_index

    return check
end

-- 在补领的场合下，需要达成【今天全领取】+【之前每一天全领取】+【日期已过】 = 【日期已过】+ 【显示补领】
-- 在非补领的场合下，需要达成【是今天】+【可领取】
function WeekendOverChargeData:OperShowCheck(index,seq)
    local day_status = self:IsToday(index)

    -- 要是已经领取了默认给可操作
    if self.WeekendInfo.day_fecth_flag_list[index-1][seq] == 1 then 
        return true
    end 

    if day_status == 0 then
        return self.WeekendInfo.day_can_fetch_flag_list[index-1][seq] == 1
    elseif day_status < 0 then
        local supply_flag = self.WeekendInfo.day_can_fetch_flag_list[index-1][seq] == 1
        local today_done = self:TodayDone()

        return supply_flag and today_done 
    elseif day_status > 0 then
        return false
    end
end

function WeekendOverChargeData:SupplyChargeNeed(index,seq)
    local today_index = self:GetTodayIndex() 
    if index == today_index then return "" end 

    local list = self:GetConfigList()
    local need_num = 0
    -- 这一天之前的每一天
    -- for i = 1,index - 1 do 
    --     for k,v in pairs(list) do 
    --         need_num = v.acc_price + need_num
    --     end 
    -- end 

    for k,v in pairs(list) do 
        if v.seq == seq then 
            need_num =  v.acc_price -- + need_num
        end 
    end 

    local check_num = need_num -- self.WeekendInfo.day_chong_zhi_num
    return (self:TodayDone() and self:IsToday(index) <0 and self:CheckIsShowSupply(index,seq)) 
        and string.format(Language.WeekendOverCharge.SupplyNeed,RechargeData.Instance:GoldToMoney((check_num))) or ""
end

function WeekendOverChargeData:CheckIsShowSupply(index,seq)
    local today_index = self:GetTodayIndex() 
    if index == today_index then return false end 

    if today_index <= 3 and not self:TodayDone() then 
        return false
    end 

    local list = self:GetConfigList()

    -- 这一天没充够且前面的seq与index均已充够的情况下可以显示补领
    if self.WeekendInfo.day_can_fetch_flag_list[index-1][seq] == 0 then         
        -- for i = 1,index -1 do 
        --     for k,v in pairs(list) do 
        --         -- 使用这个list 是用来规范seq的取值范围
        --         if self.WeekendInfo.day_fecth_flag_list[i-1][v.seq] == 0 then 
        --             return false
        --         end 
        --     end 
        -- end
        local mark_num = seq * 10 + index
        for i_seq = 0,#list-1  do -- 0~2 or 0~1
            for i = 1,2 do  -- 1~2
                -- LogError("?sff",i,i_seq)
                local check_num = i_seq * 10 + i
                if check_num >= mark_num then
                    break
                end
                if i_seq ~= seq or i ~= index then 
                    if self.WeekendInfo.day_can_fetch_flag_list[i-1][i_seq] == 0 then 
                        return false
                    end 
                end 

                
            end 
        end 

        return self.WeekendInfo.day_can_fetch_flag_list[index-1][seq] == 0
    else 
        return false
    end 
end

function WeekendOverChargeData:TodayDone()
    local list = self:GetConfigList()
    local today_index = self:GetTodayIndex()
    local flag = true
    if today_index > 3 then return true end 

    for k,v in pairs(list) do 
        if self.WeekendInfo.day_can_fetch_flag_list[today_index-1][v.seq] == 0 then 
            flag = false
            break
        end 
    end 

    return flag
end

function WeekendOverChargeData:DayShowDone(index,seq)
    return self.WeekendInfo.day_fecth_flag_list[index-1][seq] == 1
end

function WeekendOverChargeData:CountineShowDone(seq)
    return self.WeekendInfo.extra_reward_flag[seq] == 1
end