
function LBFestivalData:GetTreasureRewardParam()
    local param  = {}

    param.reward_list = self:GetTreasureRewardList()
    param.stamp_time = self:CheckTreasureFinishTime()
    param.pro_un_lock = self.season_pass_info.is_active_senior == 1 
    param.pro_price = self.treasure_config.other[1].buy_senior / 10 -- 元宝要除10
    param.can_one_key = self:CheckTreasureWithReward()

    return param
end 

function LBFestivalData:GetTreasureRewardList()
    if self.season_pass_info.flush == 0 then 
        LBFestivalCtrl.Instance:ReqTreasureInfo()
        return nil
    end 
    self.wait_reward_index = 0
    if self.t_reward_list == nil then 
        local list = {} 
        for k,v in pairs(self.config_t_r) do 
            local vo = {}
            vo = TableCopy(v)
            vo.nomal_level_flag = not (v.open_grade <= self.season_pass_info.level)
            vo.pro_levek_flag = not (self.season_pass_info.is_active_senior == 1 and v.open_grade <= self.season_pass_info.level)
            vo.pro_lock_flag  = not (self.season_pass_info.is_active_senior == 1)
            vo.nomal_reward = {
                reward_item = BagData.Instance:ItemBuild(v.ordinary_item),
                state = self:GetTreasureRewardState(v.seq,v.open_grade,0),
                type = 0,
                seq = v.seq,
                level = v.open_grade,
            }
            vo.pro_reward = {}
            for i,j in pairs(v.senior_item) do 
                local p_vo = {}
                p_vo.reward_item = BagData.Instance:ItemBuild(j)
                p_vo.state = self:GetTreasureRewardState(v.seq,v.open_grade,1)
                p_vo.type = 1
                p_vo.seq = v.seq
                p_vo.level = v.open_grade
                table.insert(vo.pro_reward ,p_vo)
            end 
            if v.senior_gold_num ~= nil and v.senior_gold_num > 0 then 
                local p_vo = {}
                p_vo.reward_item =  BagData.Instance:ItemBuild({item_id = CommonItemId.Gold,num = v.senior_gold_num,is_bind = 0})
                p_vo.state = self:GetTreasureRewardState(v.seq,v.open_grade,1)
                p_vo.type = 1
                p_vo.seq = v.seq
                p_vo.level = v.open_grade
                table.insert(vo.pro_reward ,p_vo)
            end

            if self.wait_reward_index == 0  and not vo.nomal_level_flag and vo.nomal_reward.state == GodTokenData.REWARD_STATE.CAN then 
                self.wait_reward_index = k
            end  
            table.insert(list,vo)
        end 
        self.t_reward_list = list
    else 
        for k,v in pairs(self.t_reward_list) do 
            v.nomal_level_flag = not (v.open_grade <= self.season_pass_info.level)
            v.pro_levek_flag = not (self.season_pass_info.is_active_senior == 1 and v.open_grade <= self.season_pass_info.level)
            v.pro_lock_flag = not (self.season_pass_info.is_active_senior == 1)
            v.nomal_reward.state = self:GetTreasureRewardState(v.seq,v.open_grade,0)
            for i,j in pairs(v.pro_reward) do 
                j.state = self:GetTreasureRewardState(v.seq,v.open_grade,1)
            end 

            if self.wait_reward_index == 0 and not v.nomal_level_flag and v.nomal_reward.state == GodTokenData.REWARD_STATE.CAN then 
                self.wait_reward_index = k
            end  
        end 
    end 

    return self.t_reward_list
end

function LBFestivalData:GetTreasureRewardState(seq,need_level,type)
    local oper_list = type == 0 and self.season_pass_info.fetch_common_flag or self.season_pass_info.fetch_senior_flag
    
    -- 未领取 且 等级比配置高
    if oper_list[seq] == 0 and need_level <= self.season_pass_info.level and self.season_pass_info.is_active_senior == 1 and type == 1 then 
        return GodTokenData.REWARD_STATE.CAN
    -- 未领取 且 等级比配置低
    elseif oper_list[seq] == 0 and need_level > self.season_pass_info.level or (self.season_pass_info.is_active_senior == 0 and type == 1) then 
        return GodTokenData.REWARD_STATE.NOT
    elseif oper_list[seq] == 0 and need_level <= self.season_pass_info.level and type == 0 then 
        return GodTokenData.REWARD_STATE.CAN
    elseif oper_list[seq] == 0 and need_level > self.season_pass_info.level and type == 0 then 
        return GodTokenData.REWARD_STATE.NOT
    -- 已领取
    else
        return GodTokenData.REWARD_STATE.GETED
    end 
end

function LBFestivalData:CheckTreasureFinishTime()
    return LandsBetweenData.Instance.base_info.season_end_timestamp - TIME_HOUR_SECOND_NUM * 2
end

-- 检查有没有能领取的
function LBFestivalData:CheckTreasureWithReward()
    local list = self:GetTreasureRewardList()
    for k,v in pairs(list) do 
        if v.nomal_reward.state == GodTokenData.REWARD_STATE.CAN then 
            return true
        end 
        for i,j in pairs(v.pro_reward) do 
            if j.state == GodTokenData.REWARD_STATE.CAN then 
                return true
            end  
        end  
    end 

    -- if self.season_pass_info.surplus_receive_count > 0 then 
    --     return true
    -- end

    return false
end

-- 尝试一键领取奖励
function LBFestivalData:TryToOneKeyTresureReward()
    local list = self:GetTreasureRewardList()
    for k,v in pairs(list) do 
        if v.nomal_reward.state == GodTokenData.REWARD_STATE.CAN then 
            LBFestivalCtrl.Instance:ReqTreasureReward(v.nomal_reward.level,0)
        end 
        if v.pro_reward[1].state == GodTokenData.REWARD_STATE.CAN then 
            LBFestivalCtrl.Instance:ReqTreasureReward(v.pro_reward[1].level,1)
        end   
    end     

    -- if self.season_pass_info.surplus_receive_count > 0 then 
    --     LBFestivalCtrl.Instance:ReqTreasureBuySurplus()
    -- end
end

-- 获取等级更新
function LBFestivalData:GetTreasureLevelParam()
    local param = {}
    param.level = self.season_pass_info.level
    param.with_surplus = self:GetTreasureTaskRed()

    local cfg = self.config_t_r[param.level+1]
    local open_exp = 0
    if cfg == nil then 
        open_exp = Config.shenyukuanghuan_auto.surplus_reward[1].open_exp
    else
        open_exp = cfg.open_exp
    end
    param.exp_progress = self.season_pass_info.exp / open_exp
    param.exp_progress_str = self.season_pass_info.exp.."/"..open_exp
    local max_level = #self.config_t_r
    param.show_buy_level = self.season_pass_info.level >= max_level
    
    param.buy_text = param.show_buy_level and Language.LBFestival.Treasure.BtnSurplusReward or Language.LBFestival.Treasure.BtnBuyLevel
    return param
end

function LBFestivalData:CheckIsOpenBuyLevel()
    local max_level = #self.config_t_r
    return self.season_pass_info.level >= max_level
end

function LBFestivalData:GetTreasureDailyTaskCfg(task_seq)
    for k,v in pairs(Config.shenyukuanghuan_auto.dailytask) do 
        if task_seq == v.seq then
            return v
        end
    end 
    return nil
end

function LBFestivalData:GetTreasureSeasonTaskCfg(task_seq)
    for k,v in pairs(Config.shenyukuanghuan_auto.weeklytasks) do 
        if task_seq == v.seq then
            return v
        end
    end 
    return nil
end

function LBFestivalData:GetTreasureDailyTaskProgress(task_index)
    return self.season_pass_info.daily_task_progress[task_index]
end

function LBFestivalData:GetTreasureSeasonTaskProgress(task_index)
    return self.season_pass_info.season_task_progress[task_index]
end

function LBFestivalData:GetTreasureTaskParam()
    local param = {}
    param.task_list = {}
    local operate_tasks = self.operate_info.task_type == 1 and self.season_pass_info.daily_tasks  or self.season_pass_info.season_tasks 
    for k,v in pairs(operate_tasks) do
        if v > -1 then 
            local vo = {}

            local task_cfg = nil
            if (self.operate_info.task_type == 1) then 
                task_cfg = self:GetTreasureDailyTaskCfg(v) 
            else
                task_cfg = self:GetTreasureSeasonTaskCfg(v)
            end 

            if task_cfg ~= nil then 
                vo.desc = task_cfg.describe
                local progress = self.operate_info.task_type == 1 and self:GetTreasureDailyTaskProgress(k) or self:GetTreasureSeasonTaskProgress(k)
    
                local pro_text = ""
                if progress < task_cfg.parameter_1 then
                    local num = progress < 0 and task_cfg.parameter_1 or progress
                    if progress >= 0 then
                        pro_text = string.format(" (%s/%s)",num,task_cfg.parameter_1)
                    else
                        pro_text = ColorStr(string.format(" (%s/%s)",num,task_cfg.parameter_1),COLORSTR.Green11)
                    end
                else
                    pro_text = ColorStr(string.format(" (%s/%s)",task_cfg.parameter_1,task_cfg.parameter_1),COLORSTR.Green11)
                end
    
                vo.progress_str = pro_text
                vo.accomplish = progress <= task_cfg.parameter_1 and progress >= 0
                vo.open_panel = task_cfg.open_panel
                vo.open_exp = task_cfg.open_exp
                vo.show_lock = self.season_pass_info.is_active_senior == 0 and task_cfg.activation_Supreme == 1 
                local flag = vo.accomplish and 0 or 2
                if task_cfg.activation_Supreme == 1 then 
                    flag = vo.accomplish and (self.season_pass_info.is_active_senior == 0 and 1 or 0) or 2
                end 
                vo.sort_flag = flag
                table.insert(param.task_list,vo)
            end 
        end 
    end

    table.sort(param.task_list, DataSorter.KeyLowerSorter("sort_flag"))
    param.stamp_time = TimeManager.DayEnd()
    return param
end

function LBFestivalData:SetTreasureTaskType(type)
    self.operate_info.task_type = type
    self.operate_info.flush = self.operate_info.flush + 1
end

function LBFestivalData:GetTreasureTaskRed()
    local max_level = #self.config_t_r
    local flag_max = self.season_pass_info.level >= max_level 
    return ( flag_max and self.season_pass_info.surplus_receive_count > 0 ) and 1 or 0 
end

function LBFestivalData:SetTreasureBuyLevelNum(level)
    self.operate_buy_info.buy_level = level
end

function LBFestivalData:GetBuyLevelParam()
    local param = {}
    param.ready_level = self.season_pass_info.level + self.operate_buy_info.buy_level
    param.max_level_str = self:GetMaxBuyLevelStr()
    param.max_level = self:GetMaxBuyLevel()
    param.reward_list = self:GetCurBuyExpRewardList(self.season_pass_info.level,param.ready_level)
    param.with_currency = RoleData.Instance:GetCurrency(CurrencyType.Gold)

    param.exp_count =  math.floor(self:GetLevelNeedExp(self.season_pass_info.level,param.ready_level)/10)
    param.price_num = param.exp_count * self:GetExpPrice()
    param.level_str = string.format(Language.GodToken.Tip3,self:GetExpPrice())
    
    return param
end 

function LBFestivalData:GetMaxBuyLevelStr()
    local level = self:GetMaxBuyLevel()
    if level < 0 then 
        return Language.LBFestival.Treasure.MaxBuyLevelDone
    end 
    return string.format(Language.LBFestival.Treasure.MaxBuyLevel,level)
end

function LBFestivalData:GetMaxBuyLevel()
    return #self.config_t_r - self.season_pass_info.level
end

--获取经验价格
function LBFestivalData:GetExpPrice()
	return self.treasure_config.other[1].buy_exp
end

--获取至尊宝藏价格
function LBFestivalData:GetGodSeniorPrice()
	return self.treasure_config.other[1].buy_senior
end

function LBFestivalData:GetLevelNeedExp(start,next,is_not_mimus)
	is_not_mimus = is_not_mimus or false    
	local sum_lv_need_exp = 0

    for _,v in pairs(self.config_t_r) do
		if v.open_grade > start and  v.open_grade <= next then
			sum_lv_need_exp =  sum_lv_need_exp + v.open_exp
		end
	end
    -- local max_level = #self.config_t_r
    -- if next > max_level then 
    --     -- 溢出了多少级就加多少级
    --     for i = max_level,next do 
    --         sum_lv_need_exp =  sum_lv_need_exp + self.config_t_r_surplus.open_exp
    --     end 
    -- end 
    

	if is_not_mimus then
		return sum_lv_need_exp
	else
		return sum_lv_need_exp - self:GetCurExp()
	end
end

--获取当前经验
function LBFestivalData:GetCurExp()
	return self.season_pass_info.exp
end

function LBFestivalData:IsBuySenior()
    return self.season_pass_info.is_active_senior == 1
end

function LBFestivalData:GetCurBuyExpRewardList(start,next)
    local list = {} 
    for k,v in pairs(self.config_t_r) do 
        if v.open_grade >=start and v.open_grade <= next then 
            local nomal = BagData.Instance:ItemBuild(v.ordinary_item)
            table.insert(list,nomal)

            if self.season_pass_info.is_active_senior == 1 then
                for i,j in pairs(v.senior_item) do 
                    local senior = BagData.Instance:ItemBuild(j)
                    table.insert(list,senior)
                end 
            end 
        end 
    end 

    -- -- 当购买等级大于最大等级时
    -- local max_level = #self.config_t_r
    -- if next >= max_level then 
    --     -- 溢出了多少级就进行多少次处理
    --     for i = max_level ,next do 
    --         local nomal = BagData.Instance:ItemListBuild(self.config_t_r_surplus.ordinary_item)
    --         for index,item in pairs(nomal) do 
    --             table.insert(list,item)
    --         end 

    --         if self.season_pass_info.is_active_senior == 1 then
    --             local senior = BagData.Instance:ItemListBuild(self.config_t_r_surplus.senior_item)
    --             for index,item in pairs(senior) do 
    --                 table.insert(list,item)
    --             end 
    --         end
    --     end 
    -- end 

    return list
end

function LBFestivalData:GetWelfareParam()
    local param = {}
    local list = self:GetWelfareRewardList()
    param.buy_timers = self.fuli_info.fuli_buy_num
    param.stamp_time = self:CheckTreasureFinishTime()

    table.sort(list,function(a,b)
        return (a.sort) < (b.sort)
    end)

    param.list = list
    return param
end

function LBFestivalData:GetWelfareRewardList()
    local list = {} 
    for k,v in pairs(self.treasure_config.universal_welfare) do 
        local vo = {}
        vo.title = v.describe
        vo.reward_list = BagData.Instance:ItemListBuild(v.item_reward)
        vo.is_fetch = self.fuli_info.fuli_flag[k-1] == 1 
        vo.btn_enable = v.purchase_number <= self.fuli_info.fuli_buy_num
        vo.sort = vo.is_fetch and k+10 or k
        vo.index = k-1
        table.insert(list,vo)
    end 

    return list
end

function LBFestivalData:GetSurplusRewardParam()
    return {
        surplus_receive_count = self.season_pass_info.surplus_receive_count,
        nomal_list = BagData.Instance:ItemListBuild(self.config_t_r_surplus.ordinary_item),
        senior_list = BagData.Instance:ItemListBuild(self.config_t_r_surplus.senior_item),
        surplus_exp = self:GetCurExp(),
        total_exp = self.config_t_r_surplus.open_exp,
    }
end

function LBFestivalData:GetTreasureRemind()
    if self.season_pass_info.surplus_receive_count ~= nil and self.season_pass_info.surplus_receive_count > 0 then 
        return 1
    end 
    local list = self:GetTreasureRewardList()
    if list ~= nil then 
        for k,v in pairs(list) do 
            -- LogError("dsa fa",v.nomal_reward.state,v)
            if v.nomal_reward.state == GodTokenData.REWARD_STATE.CAN then 
                return 1
            end 
            for i,j in pairs(v.pro_reward) do 
                -- LogError("dsa fa",j.state,j)
                if j.state == GodTokenData.REWARD_STATE.CAN then 
                    return 1
                end 
            end 
        end 
    end 
    
    return 0
end