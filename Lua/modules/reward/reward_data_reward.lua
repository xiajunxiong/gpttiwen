function RewardData:RewardConfig()
	ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoDayReward,function () 	
		self.reward_config = Config.day_reward_auto
		self.reward_config_reward_list = self.reward_config.reward_list
		self.reward_config_online_reward = self.reward_config.online_reward
		self.reward_config_sign_reward = self.reward_config.sign_reward
		self.reward_config_find_reward = self.reward_config.find_reward
		self.reward_config_level_reward = self.reward_config.level_reward
		self.reward_config_find_price = self.reward_config.find_price
		self.reward_config_anti_fraud = Config.sundries_auto.gift_configuration[1]
		Cfg.UnRegister("RewardFindBackbyType")
		Cfg.Register("RewardFindBackbyType", Cfg.RealTimeSearch(Config.day_reward_auto.find_reward, "find_type"))
		self.reward_find_price_list = {}
		for _, info in ipairs(self.reward_config_find_price) do
			self.reward_find_price_list[info.find_type] = self.reward_find_price_list[info.find_type] or {}
			table.insert(self.reward_find_price_list[info.find_type], info)
		end	
	end,true)			
	ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoImmortalCoinFund,function () 
		self.all_immortal_fund_config = nil
	end,true)
end

function RewardData:RewardDataReward()
	self:RewardOnlineList()
	self:RewardDailySignInList()
	self:RewardActivityBackList()
	self:RewardUpgradeList()
	self:RewardAntiFraudList()


	self.today = tonumber(os.date("%d", TimeHelper.GetServerTime()))
	self.today_online_time = 0
	self.online_protocol_time = 0
	self.daily_sign_in_count = 0
	self.daily_sign_in_flag = 0
	self.month_zero_open_day = 0

	self.online_red_point = SmartData.New{val = false}
	self.daily_sign_in_red_point = SmartData.New{val = false}
	self.activity_back_red_point = SmartData.New{val = false}
	self.upgrade_red_point = SmartData.New{val = false}
	self.score_red_point = SmartData.New{val = false}
	self.roller_red_point = SmartData.New{val = false}
	self.fortune_bag_red_point = SmartData.New{val = false}
	self.immortal_fund_red_point = SmartData.New{val = false}
	self.month_card_red_point = SmartData.New{val = false}
	self.online_change = SmartData.New{val = false}
	self.daily_sign_change = SmartData.New{val = false}
	self.upgrade_change = SmartData.New{val = false}

	self.score_data = SmartData.New{score=0,flag={}}
	self.roller_data = SmartData.New{info={},index=1,val=false,change = false}
	self.immortal_fund_data = SmartData.New({receive_flag = {}, phase = -1, is_show = false, is_first = true, level_show_flag = false})
	self.month_card_data = SmartData.New({info = {}})
	self.month_card_list = {cur_level = -1, show_list = nil}
	
	self.anti_fraud_info = SmartData.New{flag = 0}
	self.anti_fraud_red_point = SmartData.New{val = false}

	self.have_day_food_info = false
	self.day_food_info = SmartData.New({flag = 0})
	self.day_food_red_point = SmartData.New{val = false}

	-- 宠物强化福利
	self.pet_intensify_data = SmartData.New({info = {level = 0, flag_list = {}}})
	-- 神兽恩泽
	self.god_beast_grace_data = SmartData.New({remind_flag = true})
	self:ResetGodBeastGraceData()
end

function RewardData:Today()
	return self.today
end

function RewardData:TodayOnlineTime()
	return self.today_online_time
end

function RewardData:OnlineProtocolTime()
	return self.online_protocol_time
end

function RewardData:DailySignInCountFlag()
	return self.daily_sign_in_count, self.daily_sign_in_flag
end

function RewardData:SetOnlineRewardInfo(protocol)
	self.today_online_time = protocol.today_online_time
	self.online_protocol_time = TimeHelper.RealTime()
	local real_online_time = TimeHelper:RealTime() - self.online_protocol_time + self.today_online_time
	for k, v in pairs(self.reward_online_list) do
		local get_flag = bit:_rshift(protocol.reward_fetch_flag, v.seq) % 2
		self.reward_online_list[k].get_flag = get_flag
		TimeHelper:CancelTimer(self.reward_online_list[k].timer_dt)
		if v.time > real_online_time then
			self.reward_online_list[k].online_red_point.val = false
			self.reward_online_list[k].timer_dt = TimeHelper:AddDelayTimer(function ()
				self.reward_online_list[k].online_red_point.val = true
				self:OnlineRedPoint()
			end, v.time - real_online_time)
		else
			self.reward_online_list[k].online_red_point.val = 0 == get_flag
		end
	end
	self:OnlineRedPoint()
	self.online_change.val = not self.online_change.val
end

function RewardData:SetSignRewardInfo(protocol)
	if self.today ~= tonumber(os.date("%d", TimeHelper.GetServerTime())) then
		self.today = tonumber(os.date("%d", TimeHelper.GetServerTime()))
		self:RewardDailySignInList()
	end
	self.daily_sign_in_count = protocol.sign_count
	self.daily_sign_in_flag = protocol.reward_fetch_flag
	self.month_zero_open_day = protocol.month_zero_open_day
	self:DailySignInRedPoint()
	self.daily_sign_change.val = not self.daily_sign_change.val
end

function RewardData:SetLevelRewardInfo(protocol)
	self.upgrade_fetch_flag = bit:d2b(protocol.reward_fetch_flag)
	self:CheckLevelReward()
end

function RewardData:CheckLevelReward()
	if self.upgrade_fetch_flag == nil then return end
	local role_level = RoleData.Instance:GetRoleLevel()
	for k, v in pairs(self.reward_upgrade_list) do
		local get_flag = self.upgrade_fetch_flag[32 - v.seq] or 0
		self.reward_upgrade_list[k].get_flag = get_flag
		self.reward_upgrade_list[k].upgrade_red_point.val = role_level >= v.level and 0 == get_flag
	end
	self:UpgradeRedPoint()
	self.upgrade_change.val = not self.upgrade_change.val
	-- self:CheckLevelUseShow()
end

function RewardData:SetFindRewardInfo(protocol)
	for type, v in pairs(self.reward_activity_back_list) do
		local sum = protocol.count_list[type] or 0
		local finish = protocol.today_find_count[type] or 0
		self.reward_activity_back_list[type].sum = sum
		self.reward_activity_back_list[type].finish = finish
		self.reward_activity_back_list[type].cur_num = sum
		self.reward_activity_back_list[type].get_flag = 1 == bit:_rshift(protocol.find_reward_flag, type) % 2
	end
	self:ActivityBackRedPoint()
	--找回首次指引
	-- if not ViewMgr:IsOpen(ActiveGuide) and PrefsInt([[ActiveActivityBackGuide]].. RoleId()) == 0 then
	-- 	if self:GetActivityBackRedPointNum() == 1 then
	-- 		TimeHelper:CancelTimer(self.ActivityBackGuideTimer)
	-- 		self.ActivityBackGuideTimer = TimeHelper:AddDelayTimer(function()
	-- 			ViewMgr:OpenView(ActiveGuide,{desc = Language.Activity.ActivityBackGuideTip})
	-- 		end,3)
	-- 	end
	-- end
end

function RewardData:RewardOnlineList()
	self.reward_online_list = {}
	for _, info in pairs(self.reward_config_online_reward) do
		local data = {}
		data.seq = info.seq
		data.index = info.index
		data.time = info.time
		data.section_start = info.section_start
		data.section_end = info.section_end
		data.rewards = {}
		for k, v in pairs(info.online_reward) do
			local item_data = Item.New()
			item_data.item_id = v.item_id
			item_data.num = v.num
			item_data.is_bind = v.is_bind
			table.insert(data.rewards, item_data)
		end
		if 0 ~= info.bind_coin then
			local item_data = Item.New()
			item_data.item_id = CommonItemId.Coin
			item_data.num = info.bind_coin
			item_data.is_bind = 0
			table.insert(data.rewards, item_data)
		end
		data.online_red_point = SmartData.New{ val = false }
		table.insert(self.reward_online_list, data)
	end
end

function RewardData:GetRewardOnlineList()
	local list = ServerActivityData.Instance:GetActivityOpenCfg(nil, nil, self.reward_online_list)
	table.sort(list, function(a, b)
		if not a or not b then return a end
		return a.get_flag == b.get_flag and a.seq < b.seq or a.get_flag < b.get_flag
	end)
	return list
end

function RewardData:RewardDailySignInList()
	self.reward_daily_sign_in_list = {}
	local month = tonumber(os.date("%m", TimeHelper.GetServerTime()))
	for _, info in pairs(self.reward_config_sign_reward) do
		if info.month == month then
			local item_data = Item.New()
			if info.online_reward and next(info.online_reward) then
				item_data.item_id = info.online_reward.item_id
				item_data.num = info.online_reward.num
				item_data.is_bind = info.online_reward.is_bind
			else
				item_data.item_id = CommonItemId.Coin
				item_data.num = info.bind_coin
				item_data.is_bind = 0
			end
			local data = {
				day = info.day,
				section_start = info.section_start,
				section_end = info.section_end,
				reward = item_data,
			}
			table.insert(self.reward_daily_sign_in_list, data)
		end
	end

	-- table.sort(self.reward_daily_sign_in_list, function(a, b)
	-- 	return a.day < b.day
	-- end)
end

function RewardData:GetRewardDailySignInList()
	return ServerActivityData.Instance:GetSectionList(self.reward_daily_sign_in_list, "day", self.month_zero_open_day)
end

function RewardData:RewardActivityBackList()
	self.reward_activity_back_list = {}
	for _, info in pairs(self.reward_config_find_reward) do
		if info.is_display == 1 then
			local data = info
			data.back_price = {
				[1] = info.coin_price,
				[2] = info.gold_price,
			}
			data.cur_num = 1
			self.reward_activity_back_list[info.find_type] = info
		end
	end

	local info = self.reward_activity_back_list[next(self.reward_activity_back_list)]
	self.reward_activity_back_type_list = {}
	self.reward_activity_back_type_list[1] = { type = 1, back_scale = info.coin_scale }
	self:ActivityBackFunc()
end

function RewardData:GetRewardActivityBackRewardList(info, reward_list, sum)
	local list = {}
	for abt, rewards in pairs(reward_list) do
		list[abt] = list[abt] or {} 
		for _, reward in ipairs(rewards) do
			if FindType.BountyTask ~= info.find_type or CommonItemId.Coin == reward.item_id or CommonItemId.Exp == reward.item_id then
				local num = sum or info.sum
				num = 0 == num and info.finish or num
				reward.num = reward.num * (FindType.SilkRoad == info.find_type and 1 or num)
				table.insert(list[abt], reward)
			end
		end
	end
	return list
end

function RewardData:GetRewardActivityBackList()
	local list = {}
	local role_level = RoleData.Instance:GetRoleLevel()
	for _, info in pairs(self.reward_activity_back_list) do
		if RewardConfig.BackType.coin == self.reward_activity_back_type_cur or 2 ~= info.find_type then
			if info.find_level <= role_level then
				local func = self.activity_back_func[info.find_type]
				if func then
					local cur_num = info.cur_num
					if FindType.FantasyFB == info.find_type then
						cur_num = FightData.Instance:GetFantasyBackRewardInfo(info.sum)
					end
					info.reward_list = self:GetRewardActivityBackRewardList(info, self.activity_back_func[info.find_type]())
					if info.find_type == FindType.FantasyFB then info.out_sum = true end
					if FindType.SkillRoadAct == info.find_type then
						info.sp_reward = TableCopy(SilkRoadData.Instance:FindBackItem(), 2)
						info.sp_reward.num = info.sp_reward.num * info.sum
					end
					if cur_num > 0 or info.get_flag then
						table.insert(list, info)
					end
				end
			end
		end
	end
	table.sort(list, function(a, b)
		if not a or not b then return a end
        local as = ((a.sum > 0 and not a.out_sum) or (a.sum == 0 and a.out_sum)) and 1 or 0
		local bs = ((b.sum > 0 and not b.out_sum) or (b.sum == 0 and b.out_sum)) and 1 or 0
		if as == bs then return a.index < b.index end
        return as > bs
	end)
	return list
end

function RewardData:GetRewardActivityBackInfoByFindType(find_type, sum)
	local role_level = RoleData.Instance:GetRoleLevel()
	for _, info in pairs(self.reward_activity_back_list) do
		if info.find_type == find_type then
			if 2 ~= info.find_type then
				if info.find_level <= role_level then
					local func = self.activity_back_func[info.find_type]
					if func then
						info.reward_list = self:GetRewardActivityBackRewardList(info, self.activity_back_func[info.find_type](), sum)
						return info
					end
				end
			end
		end
	end
end

function RewardData:GetRewardActivityBackTypeList()
	return self.reward_activity_back_type_list
end

function RewardData:GetRewardActivityBackAllNeedByType(type)
	local list = self:GetRewardActivityBackList()
	local all_need = 0
	for _, info in pairs(list) do
		-- 对四象幻境进行特殊处理
		local sum = info.find_type == FindType.FantasyFB and FightData.Instance:GetFantasyBackRewardInfo(info.sum) or info.sum
		-- all_need = all_need + info.back_price[type] * sum
		local price = self:GetRewardActivityBackFindPrice(info.find_type)
		all_need = all_need + (info.find_type == FindType.FightEnergy and self:ConfigRewardFindCopperCoinPrice() or (price * sum))
	end
	return all_need
end

function RewardData:RewardUpgradeList()
	self.reward_upgrade_list = {}
	for _, info in pairs(self.reward_config_level_reward) do
		local data = {}
		data.seq = info.seq
		data.level = info.level
		data.show = info.show
        data.show_level= info.show_level
        data.show_name= info.show_name
        data.show_descr= info.show_descr
        data.icon =  info.icon
		data.rewards = {}
		for i = 0, #info.online_reward do
			local item = info.online_reward[i]
			local item_data = Item.New()
			item_data.item_id = item.item_id
			item_data.num = item.num
			item_data.is_bind = item.is_bind
			item_data.old_id = Cfg.RewardJobBagHasId(item_data.item_id, false) and item_data.item_id or nil
			table.insert(data.rewards, item_data)
		end
		if info.bind_coin > 0 then
			local item_data = Item.New()
			item_data.item_id = CommonItemId.Coin
			item_data.num = info.bind_coin
			item_data.is_bind = 0
			table.insert(data.rewards, item_data)
		end
		data.get_flag = 0
		data.upgrade_red_point = SmartData.New{ val = false }
		table.insert(self.reward_upgrade_list, data)
	end
end

function RewardData:GetRewardUpgradeList()
	table.sort(self.reward_upgrade_list, function(a, b)
		if not a or not b then return a end
		return a.get_flag == b.get_flag and a.seq < b.seq or a.get_flag < b.get_flag
	end)
	return self.reward_upgrade_list
end

function RewardData:OnlineRedPoint()
	local orp = false
	for _, reward in pairs(self.reward_online_list) do
		orp = orp or reward.online_red_point.val
	end
	self.online_red_point.val = orp
end

function RewardData:DailySignInRedPoint()
	self.daily_sign_in_red_point.val = 0 == self.daily_sign_in_flag
end

function RewardData:UpgradeRedPoint()
	local urp = false
	for i,v in ipairs(self.reward_upgrade_list) do
		urp = urp or v.upgrade_red_point.val
	end
	self.upgrade_red_point.val = urp
end

function RewardData:ActivityBackRedPoint()
	local urp = false
	for k,v in pairs(self.reward_activity_back_list) do
		local sum = v.find_type == FindType.FantasyFB and FightData.Instance:GetFantasyBackRewardInfo(v.sum) or v.sum
		if sum ~= 0 then
			urp = true
			break
		end
	end
	if not urp then
		-- 红点过一遍所有的超特殊的找回
		if self:ActivityBackSpRemind() > 0 then
			urp = true
		end
	end
	self.activity_back_red_point.val = urp
end

-- 超特殊找回的红点判断
function RewardData:ActivityBackSpRemind(find_type)
	local sp_data_list = self:ActivityBackSpData(find_type)
	if find_type and (not TableIsEmpty(sp_data_list)) then
		local remind_num = sp_data_list.remind_fun()
		return remind_num
	elseif (not TableIsEmpty(sp_data_list)) then
		for k, v in pairs(sp_data_list) do
			local remind_num = v.remind_fun()
			if remind_num > 0 then
				return 1
			end
		end
	end
	return 0
end

-- 超特殊的找回的领取
function RewardData:ActivityBackSpReceive(find_type)
	local sp_data_list = self:ActivityBackSpData(find_type)
	if find_type and (not TableIsEmpty(sp_data_list)) then
		sp_data_list.receive_fun()
	elseif (not TableIsEmpty(sp_data_list)) then
		for k, v in pairs(sp_data_list) do
			local remind_num = v.remind_fun()
			if remind_num > 0 then
				v.receive_fun()
			end
		end
	end
end

function RewardData:GetActivityBackRedPointNum()
	if not FunOpen.Instance:IsFunOpen(Mod.Activity.ActivityBack) then
		return 0
	end
	return self.activity_back_red_point.val and 1 or 0
end

--==========================评分奖励==========================
function RewardData:SetScoreRewardInfo(protocol)
	self.score_data.score = protocol.score
	self.score_data.flag = bit:d2b(protocol.flag)
	self.score_red_point.val = self:GetScoreRedNum() ~= 0
	if self:GetScoreEnd() == false then
		self.reward_list_data:Notify()
	end
end

function RewardData:GetScoreRewardFlag(seq)
	return self.score_data.flag[32 - seq] or 0
end

function RewardData:GetScoreNum()
	return self.score_data.score
end

function RewardData:GetScoreRedNum()
	if self.score_data.score == 0 then return 0 end
	for i,v in ipairs(ServerActivityData.Instance:GetScoreData()) do
		if self.score_data.score >= v.score then
			if self:GetScoreRewardFlag(v.seq) == 0 then
				return 1
			end
		end
	end
	return 0
end

function RewardData:GetScoreEnd()
	for i,v in ipairs(ServerActivityData.Instance:GetScoreData()) do
		if self:GetScoreRewardFlag(v.seq) == 0 then
			return true
		end
	end
	return false
end


function RewardData:GetInvestEnd()
	return self:GetInvestRemainNum() >= 0
end
--==========================消费转盘==========================
function RewardData:SetConsumeCarouselInfo(protocol)
	self.roller_data.info = protocol.consume_info
	self.roller_red_point.val = protocol.consume_info.last_num ~= 0
end

function RewardData:SetConsumeCarouselReward(protocol)
	self.roller_data.index = protocol.index
	self.roller_data.change = not self.roller_data.change
end

function RewardData:SetConsumeCarouselShow(protocol)
	self.roller_data.get = protocol.get_list or {}
	self.roller_data.val = not self.roller_data.val
end

function RewardData:GetConsumeShowList()
	return self.roller_data.get or {}
end

function RewardData:ClearRebateData()
	self.roller_data:Clear()
end

--找回红点
function RewardData:SetRewardFindNotify(protocol)
	self.activity_back_notify = protocol.notify
end

function RewardData:GetRewardFindNotify()
	return self.activity_back_notify or 0
end

--福袋红点
function RewardData:CheckFortuneBag(val)
	self.fortune_bag_red_point.val = not self.fortune_bag_red_point.val
end

--等级弹窗
function RewardData:CheckLevelUseShow()
	if ViewMgr:IsOpen(RewardView) then
		return
	end
	if MainViewData.Instance:IsBattle() then
		return
	end
	if RewardData.CanUpgradeData then
		return
	end
	local level = 0
	local role_level = RoleData.Instance:GetRoleLevel()
	for i,v in ipairs(self.reward_upgrade_list) do
		if role_level >= v.level and 0 == v.get_flag then
			if level == 0 then
				level = v.level
			end
		end
	end
	if level ~= 0 then
		TimeHelper:AddDelayTimer(function()
			if FunOpen.Instance:GetFunIsOpened(Mod.Reward.Upgrade) then
				MainOtherCtrl.Instance:OpenKeyUseView(Item.New({item_id = 28506,level = level,num = 1}))
			end
		end, 2)
	end
end

function RewardData:ClearUpgrade()
	for k, v in pairs(self.reward_upgrade_list) do
		self.reward_upgrade_list[k].get_flag = 0
		self.reward_upgrade_list[k].upgrade_red_point.val = false
	end
	RewardData.CanUpgradeData = nil
	self.upgrade_fetch_flag = nil
end

function RewardData:ClearActivityBack()
	self.activity_back_notify = nil
end

function RewardData:ClearPointData()
	self.upgrade_red_point:Clear()
	self.activity_back_red_point:Clear()
	self.roller_red_point:Clear()
	self.month_card_red_point:Clear()
	self.day_food_red_point.val = false
end

--获取找回奖励
function RewardData:GetFindConfig(find_type)
	for k,v in pairs(self.reward_config_find_reward) do
		if v.find_type == find_type then
			return v
		end
	end
end

--获取找回花费
function RewardData:GetFindFree(find_type,cost_type,num)
	local config = self:GetFindConfig(find_type) or {}
	return cost_type == 0 and config.coin_price * num or config.gold_price * num
end

--是否满足等级
function RewardData:IsFindOpen(find_type)
	local config = self:GetFindConfig(find_type) or {}
	return RoleData.Instance:GetRoleLevel() >= config.open_level or false
end

function RewardData:IsRewardActivityBackAuto()
	local value = false
	local role_level = RoleData.Instance:GetRoleLevel()
	if FunOpen.Instance:GetFunIsOpened(Mod.Activity.ActivityBack) and 0 == self.activity_back_notify then
		for type, info in pairs(self.reward_activity_back_list) do
			if RewardConfig.BackType.coin == self.reward_activity_back_type_cur or 2 ~= info.find_type then
				if info.find_level <= role_level and self.activity_back_func[info.find_type] and not info.get_flag then
					value = true
					break
				end
			end
		end
	end
	return value
end

function RewardData:GetRewardActivityBackFindPrice(find_type)
	local price = 0
	local list = self.reward_find_price_list[find_type]
	if nil ~= list then
		local role_level = RoleData.Instance:GetRoleLevel()
		for _, info in ipairs(list) do
			if info.min_level <= role_level and info.max_level >= role_level then
				price = info.coin_price
				break
			end
		end
	end
	return price
end

-- 灵玉基金
function RewardData:SetImmortalFundInfo(protocol)
	for i, v in ipairs(protocol.active_flags) do
		-- 如果一个阶段都没买
		if v == 0 and i == 1 then
			self.immortal_fund_data.phase = -1
			break
		end
		local next_active_flag = protocol.active_flags[i + 1]
		if next_active_flag then
			if v == 1 and next_active_flag == 0 then
				self.immortal_fund_data.phase = i - 1
				break
			end
		else
			self.immortal_fund_data.phase = i - 1
			break
		end
	end
	self.immortal_fund_data.receive_flag = protocol.fetch_flags
	self:CheckImmortalFund()
end

function RewardData:CheckImmortalFund()
	self:CheckImmortalFundIsOpen()
	self.immortal_fund_red_point.val = not self.immortal_fund_red_point.val
end

function RewardData:CheckImmortalFundIsOpen()
	local is_show = self:JudgeImmortalFundIsShow()
	if self.immortal_fund_data.is_show ~= is_show then
		self.immortal_fund_data.is_show = is_show
		if self.immortal_fund_data.is_first then
			self.immortal_fund_data.is_first = false
		else
			-- 等级到了强制显示红点标志位
			self.immortal_fund_data.level_show_flag = true
		end
		self.reward_list_data.val = not self.reward_list_data.val
	end
end

function RewardData:GetImmortalFundRemind()
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_IMMORTAL_FUND) then
		return 0
	else
		if not self:JudgeImmortalFundIsShow() then
			return 0
		end
		if self.immortal_fund_data.level_show_flag then
			return 1
		end
		local key = PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(), ACTIVITY_TYPE.RAND_IMMORTAL_FUND)
		local act_info = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_IMMORTAL_FUND)
		if UnityPlayerPrefs.GetString(key) ~= tostring(act_info.param_1) then
			return 1
		end
	end
	return self:GetImmortalFundIsFecthAll(true) == 0 and 1 or 0
end

function RewardData:SetImmortalFundPref()
	local key = PrefKeys.ActivityIsRemind(RoleData.Instance:GetRoleId(), ACTIVITY_TYPE.RAND_IMMORTAL_FUND)
	local act_info = ActivityData.Instance:GetActivityStatusInfo(ACTIVITY_TYPE.RAND_IMMORTAL_FUND)
	if TableIsEmpty(act_info) then
		return
	end
	UnityPlayerPrefs.SetString(key, tostring(act_info.param_1))
	self.immortal_fund_data.level_show_flag = false
	self:CheckImmortalFund()
end

function RewardData:GetImmortalFundCoinfig(index)
	if not self.all_immortal_fund_config then
		self.all_immortal_fund_config = {}
		for i, v in ipairs(Config.immortal_coin_fund_auto.gift_configure) do
			if not self.all_immortal_fund_config[v.phase] then
				self.all_immortal_fund_config[v.phase] = {}
			end
			table.insert(self.all_immortal_fund_config[v.phase], v)
		end
	end
	if self.all_immortal_fund_config[index] then
		table.sort(self.all_immortal_fund_config[index], DataHelper.WeightSort("seq", function(data)
			return self:GetImmortalFundIsFecth(data.seq) == 1 and 1000 or 0
		end))
	end
	return self.all_immortal_fund_config[index]
end

-- 判断当前阶段是否全部领取完(是否判断等级)
function RewardData:GetImmortalFundIsFecthAll(is_judge_level)
	local config = self:GetImmortalFundCoinfig(self.immortal_fund_data.phase)
	if config then
		for k, v in pairs(config) do
			if self:GetImmortalFundIsFecth(v.seq) == 0 then
				if is_judge_level then
					if RoleData.Instance:GetRoleLevel() >= v.reward_level then
						return 0
					end
				else
					return 0
				end
			end
		end
	end
	return 1
end

function RewardData:GetImmortalFundIsFecth(index)
	if self.immortal_fund_data.receive_flag[self.immortal_fund_data.phase] then
		return self.immortal_fund_data.receive_flag[self.immortal_fund_data.phase][index] or 1
	end
	return 1
end

function RewardData:ResetImmortalFundData()
	self.immortal_fund_data:Set({receive_flag = {}, phase = -1, is_show = false, is_first = true, level_show_flag = false})
end

function RewardData:GetImmortalFundPhaseCoinfig(index)
	for k, v in pairs(Config.immortal_coin_fund_auto.phase_configure) do
		if v.phase == index then
			return v
		end
	end
end

-- 用于检测灵玉基金是否显示
function RewardData:JudgeImmortalFundIsShow()
	local next_phase = self.immortal_fund_data.phase + 1
	if self.immortal_fund_data.phase == -1 then
		return FunOpen.Instance:GetFunIsOpened(Mod.RandActivity.FirstCharge)
	else
		if self:GetImmortalFundIsFecthAll() == 1 then
			if next_phase == Config.immortal_coin_fund_auto.other[1].phase then
				return false
			else
				local config = self:GetImmortalFundPhaseCoinfig(next_phase)
				if not TableIsEmpty(config) then
					if config.show_level then
						return RoleData.Instance:GetRoleLevel() >= self:GetImmortalFundPhaseCoinfig(next_phase).show_level
					else
						return false
					end
				else
					return false
				end
			end
		else
			return true
		end
	end
end

function RewardData:GetImmortalFundIsShow()
	return self.immortal_fund_data.is_show
end

-- 用于货币兑换界面判断是否显示灵玉基金的跳转
function RewardData:GetImmortalFundIsShowInChange()
	if self:JudgeImmortalFundIsShow() then
		-- 判断当前阶段是否有购买
		local phase = self.immortal_fund_data.phase
		if self:GetImmortalFundIsFecthAll() == 1 then
			phase = phase + 1
		end
		phase = (phase == Config.immortal_coin_fund_auto.other[1].phase) and (Config.immortal_coin_fund_auto.other[1].phase - 1) or phase
		if self.immortal_fund_data.phase ~= phase then
			return true, self:GetImmortalFundPhaseCoinfig(phase).multiple or 0
		end
	end
	return false
end

--==========================月卡==========================
function RewardData:SetMonthCardInfo(protocol)
	local info = {}
	info.day_reward_flag = bit:d2b(protocol.is_fetch_card_day_reward_flag)
	info.month_card_level = protocol.month_card_level
	info.week_card_end_timestamp = protocol.week_card_end_timestamp
	info.month_card_end_timestamp = protocol.month_card_end_timestamp
	info.card_shop_buy_times = protocol.card_shop_buy_times
	self.month_card_data.info = info
	self:CheckMonthCardRemind()
end

function RewardData:IsCardActived(card_type)
	if card_type == RewardConfig.MonthCardType.TYPE_WEEK then
		return self:IsWeekCardActived()
	end
	return self:IsMonthCardActived()
end

-- 当前是否有周卡或者月卡
function RewardData:IsBuyWeekAndMonth()
    return self:IsCardActived(RewardConfig.MonthCardType.TYPE_WEEK) or self:IsCardActived(RewardConfig.MonthCardType.TYPE_MONTH)
end

function RewardData:CardEndTimeStamp(card_type)
	if card_type == RewardConfig.MonthCardType.TYPE_WEEK then
		return self:GetWeekCardEndTimestamp()
	end
	return self:GetMonthCardEndTimestamp()
end

function RewardData:GetWeekCardEndTimestamp()
	return self.month_card_data.info.week_card_end_timestamp or 0
end

function RewardData:IsWeekCardActived()
	return self:GetWeekCardEndTimestamp() > TimeHelper.GetServerTime()
end

function RewardData:GetMonthCardEndTimestamp()
	return self.month_card_data.info.month_card_end_timestamp or 0
end
function RewardData:IsMonthCardActived()
	return self:GetMonthCardEndTimestamp() > TimeHelper.GetServerTime()
end

function RewardData:IsMonthCardDayRewardGet(card_type)
	if self.month_card_data.info.day_reward_flag ~= nil then
		return self.month_card_data.info.day_reward_flag[32 - card_type] == 1
	end
	return false
end

function RewardData:GetMonthCardLevel()
	local active_level = self.month_card_data.info.month_card_level or 0
	if (self:IsCardActived(0) or self:IsCardActived(1)) and active_level ~= 0 then
		return active_level
	end
	return RoleData.Instance:GetRoleLevel()
end

function RewardData:GetMonthCardCfgByLevel()
	local cur_level = self:GetMonthCardLevel()
	if self.month_card_list.cur_level == cur_level then
		return self.month_card_list.show_list
	end
	local show_list = {}
	for i, v in pairs(Config.month_card_auto.shop_configuration) do
		v.limit_times = v.limit_type == 1 and v.buy_times or v.today_times
		if v.level_down <= cur_level and cur_level <= v.level_up then
			table.insert(show_list, v)
		end
	end
	self.month_card_list.cur_level = cur_level
	self.month_card_list.show_list = show_list
	return self.month_card_list.show_list
end

function RewardData:GetMonthCardShopItems()
	local show_list = self:GetMonthCardCfgByLevel()
	local function func_sort(a, b)
		local a_val = 100
		local b_val = 100
		a_val = a.limit_times - self:GetMonthCardBuyTimes(a.seq) < 1 and a_val - 10 or a_val
		b_val = b.limit_times - self:GetMonthCardBuyTimes(b.seq) < 1 and b_val - 10 or b_val
		if a_val == b_val then
			return a.type < b.type
		end
		return a_val > b_val
	end
	table.sort(show_list, func_sort)
	return show_list
end

function RewardData:GetMonthCardBuyTimes(seq)
	if self.month_card_data.info.card_shop_buy_times ~= nil then
		return self.month_card_data.info.card_shop_buy_times[seq] or 0
	end
	return 0
end

function RewardData:GetMonthCardCfg(card_type)
	local cur_level = self:GetMonthCardLevel()
	for i, v in pairs(Config.month_card_auto.month_card_configuration) do
		if v.card_type == card_type then
			if v.level_down <= cur_level and cur_level <= v.level_up then
				return v
			end
		end
	end
end

function RewardData:MonthCardVerCfg()
	return Config.month_card_auto.other[1].cfg_ver
end

function RewardData:GetMonthCardTodayRemind()
	--月卡让改成仅首次提醒一次
	local save_val = UnityPlayerPrefs.GetString(PrefKeys.MonthCardRemind()) or ""
	-- local t0 = TimeManager.FormatUnixTime2Date(os.time())
	-- local next_key = t0.year .. "_" .. t0.month .. "_" .. t0.day
    -- return save_val ~= next_key
	return save_val == ""
end

function RewardData:SetMonthCardTodayRemind()
	local t0 = TimeManager.FormatUnixTime2Date(os.time())
	local next_key = t0.year .. "_" .. t0.month .. "_" .. t0.day
	UnityPlayerPrefs.SetString(PrefKeys.MonthCardRemind(), next_key)
end

function RewardData:CheckMonthCardRemind()
	if self:GetMonthCardTodayRemind() then
		self.month_card_red_point.val = true
	else
		for i = 0, 1 do
			if self:IsCardActived(i) and not self:IsMonthCardDayRewardGet(i) then
				self.month_card_red_point.val = true
				return
			end
		end
		if self:IsCardActived(0) or self:IsCardActived(1) then
			for i, v in pairs(self:GetMonthCardShopItems()) do
				if v.price == 0 and self:GetMonthCardBuyTimes(v.seq) < v.limit_times then
					self.month_card_red_point.val = true
					return
				end
			end
		end
		self.month_card_red_point.val = false
	end
end

function RewardData:MonthCardSeleced(card_type)
	if card_type ~= nil then
		self.month_card_select = card_type
	end
	return self.month_card_select or 0
end

function RewardData:SetFangFraudRewardInfo(protocol)
	local info = protocol.info
	self.anti_fraud_info:Set(info)
	self.anti_fraud_red_point:Set({val = 0 == info.flag})
end

function RewardData:RewardAntiFraudList()
	self.reward_anti_fraud_list = {}
	local reward_item = self.reward_config_anti_fraud.reward_item
	for i = 0, #reward_item do
		local reward_data = reward_item[i]
		table.insert(self.reward_anti_fraud_list, Item.Init(reward_data.item_id, reward_data.num, reward_data.is_bind))
	end
end

function RewardData:GetRewardAntiFraudList()
	return self.reward_anti_fraud_list
end

function RewardData:SetDayFoodInfo(protocol)
	self.day_food_info.flag = protocol.day_food_flag
	self.have_day_food_info = true
	self:ChangeRedPointValue()
end


--==========================灵玉基金兑换==========================
function RewardData:GetFundExchangeData()
	local list = {}
	for k,v in pairs(Config.exchange_jade_auto.gift_configure) do
		if v.is_show == 1 and Item.GetNum(v.item_id) > 0 then
			table.insert(list,v)
		end
	end
	table.sort(list,DataHelper.SortFunc([[seq]]))
	return list
end

function RewardData:GetFundExchangeNum()
	for k,v in pairs(Config.exchange_jade_auto.gift_configure) do
		if v.is_show == 1 and Item.GetNum(v.item_id) >= v.num then
			return 1
		end
	end
	if RewardData.IsHasFundExchangeNum then
		RewardData.IsHasFundExchangeNum = nil
		self.reward_list_data:Notify()
	end
	return 0
end

function RewardData:GetFundExchangeDetailClickList()
    local list = {}
    for k,v in pairs(Config.exchange_jade_auto.item_page) do 
        table.insert(list,v)
    end 
    return list
end

function RewardData:GetFundExchangeDetailPageList(page)
    local list = {}
    local index = 1
    for k,v in pairs(Config.exchange_jade_auto.details) do 
        if page == v.page then 
            local vo = {}
            vo.page = v.page
            vo.item_id = v.item_id
            vo.price = v.price
            vo.index = index

            table.insert(list, vo)
            index = index + 1
        end 
    end 
    return list
end

function RewardData:CheckShowExchange(item_id)
	local cfg = Item.GetConfig(item_id)
	if cfg and cfg.item_recycling and cfg.item_recycling == 1 then 
		return true
	end 
	-- for k,v in pairs(Config.exchange_jade_auto.gift_configure) do 
	-- 	if v.is_show == 1 and v.item_id == item_id then 

	-- 		return true
	-- 	end 
	-- end 
	
	return false
end

function RewardData:GetFundExchangeEnd()
	return #self:GetFundExchangeData() > 0
end


function RewardData:GetAfterAndNightTime()
	local after_time = ""
	local night_time = ""
	for k,v in pairs(Config.day_reward_auto.day_food) do
		local h_a,m_a = self:ChangeTimeText(v.time_start)
		local h_n,m_n = self:ChangeTimeText(v.time_end)
		local str = h_a..":"..m_a.."-"..h_n..":"..m_n
		if v.seq == 0 then
			after_time = string.format(Language.LunchSupper.TimeStr,str)
		else
			night_time = string.format(Language.LunchSupper.TimeStr,str)
		end
	end
	return after_time,night_time
end

function RewardData:ChangeTimeText(time)
	local str = tostring(time)
	local hour = string.sub(str,1,2)
	local min = string.sub(str,3,4)
	return hour,min
end

function RewardData:GetRewardFlag(seq)
	local is_get = false
	if self.day_food_info.flag[32 - seq] == 1 then
		is_get = true
	end
	return is_get
end

--flag 0时间未到，1可领取，2可购买，3已结束
function RewardData:GetTimeFlag(seq)
	local info
	local flag = 0
	for k,v in pairs(Config.day_reward_auto.day_food) do
		if v.seq == seq then
			info = v
			break
		end
	end
	local now_t = TimeManager.FormatUnixTime2Date(TimeHelper.GetServerTime())
	local begin_h,begin_m = self:ChangeTimeText(info.time_start)
	begin_h = tonumber(begin_h)
	begin_m = tonumber(begin_m)
	local begin_time = os.time{year = now_t.year,month = now_t.month,day = now_t.day,hour = begin_h,min = begin_m,sec = 0}
	local end_h,end_m = self:ChangeTimeText(info.time_end)
	end_h = tonumber(end_h)
	end_m = tonumber(end_m)
	local end_time = os.time{year = now_t.year,month = now_t.month,day = now_t.day,hour = end_h,min = end_m,sec = 0} 
	
	local make_h,make_m = self:ChangeTimeText(info.make_time_end)
	make_h = tonumber(make_h)
	make_m = tonumber(make_m)
	local make_time = os.time{year = now_t.year,month = now_t.month,day = now_t.day,hour = make_h,min = make_m,sec = 0} 
	if TimeHelper.GetServerTime() >= begin_time and TimeHelper.GetServerTime() <= end_time then
		flag = 1
	end
	if TimeHelper.GetServerTime() > end_time and TimeHelper.GetServerTime() <= make_time then
		flag = 2
	end

	if TimeHelper.GetServerTime() > make_time then
		flag = 3
	end	
	
	return flag
end

function RewardData:ChangeRedPointValue()
	if self.have_day_food_info then
		for i = 0, 1 do
			local flag = self:GetTimeFlag(i)
			if not self:GetRewardFlag(i) and flag == 1 then
				self.day_food_red_point.val = true
				return
			end
		end
	end
	self.day_food_red_point.val = false
end

function RewardData:ConfigRewardFindCopperCoinPrice()
	return Config.day_reward_auto.reward_find[1].copper_coin_price
end


--  ========== 宠物强化福利 ============
function RewardData:GetPetIntensifyList()
	return Config.day_reward_auto.pet_strengthen or {}
end

function RewardData:ResetPetIntensifyData()
	self.pet_intensify_data:Set({info = {level = 0, flag_list = {}}})
	self:SetPetIntensifyNewReward(nil)
end

function RewardData:PetIntensifyRewardIsOpen()
	if TableIsEmpty(self.pet_intensify_data.info) then
		return false
	end
	local level = self.pet_intensify_data.info.level or 0
	return level > 0
end

-- 1 可领取， 0条件未达成， -1已领取
function RewardData:PetIntensifyRewardFlag(data)
	local seq = data.seq
	local target_level = data.pet_strengthen
	local info = self.pet_intensify_data.info
	local flag = info.flag_list[seq] or 0
	if flag == 1 then
		return -1
	end
	if info.level >= target_level then
		return 1
	else
		return 0
	end
end

function RewardData:SetPetIntensifyData(protocol)
	self.pet_intensify_data.info = protocol.info
	if self.pet_intensify_data.record_level == nil then
		-- 检查功能是否开启
		if FunOpen.Instance:IsFunOpen(Mod.Reward.PetIntensify) then
			self.pet_intensify_data.record_level = protocol.info.level
		end
	else
		if protocol.info.level > self.pet_intensify_data.record_level then
			self.pet_intensify_data.record_level = protocol.info.level
			local list_data = self:GetPetIntensifyList()
			for i,v in pairs(list_data) do
				if v.pet_strengthen == protocol.info.level then
					self:SetPetIntensifyNewReward(v)
					break
				end
			end
		end
	end
end


function RewardData:SetPetIntensifyNewReward(data)
	self.pet_intensify_new_reward = data
end

function RewardData:GetPetIntensifyNewReward(data)
	return self.pet_intensify_new_reward
end
-- 宠物强化福利红点
function RewardData:GetPetIntensifyRemind()
	local list_data = self:GetPetIntensifyList()
	for i,v in pairs(list_data) do
		local flag = self:PetIntensifyRewardFlag(v)
		if flag > 0 then
			return 1
		end
	end
	return 0
end

--  ========== 神兽恩泽 ============
GodBeastGraceReq = {
	Info = 0,		--请求下发
	RMBBuy = 1,		--直购，p1：seq
	OtherBuy = 2,	--其他货币购买，p1：seq
	PetReward = 3,	--领取设置宠物
	SetPet = 4,		--设置宠物，p1：seq
	Lottery = 5,	--抽奖，p1：次数1次或者10次
	Record = 6,		--抽奖记录下发
	Reset = 7,		--重置已选神兽
}

function RewardData:ResetGodBeastGraceData()
	local info = {}
	info.day_buy_times = {}
	info.activity_buy_times = {}
	info.is_select = 0
	info.select_seq = 0
	self.god_beast_grace_data.info = info
	self.god_beast_grace_data.lottery_times = 0
	self.god_beast_grace_data.result = {}
	self.god_beast_grace_data.record = {}
	-- 收到抽奖结果后计算本次抽了多少抽
	self.god_beast_grace_data.now_lottery_times = 0
end

-- 用于自己界面选择的宠物，不是下发的
function RewardData:GodBeastGraceDaySelectPetSeq(select_seq)
	if select_seq then
		self.god_beast_grace_select_pet_seq = select_seq
	else
		return self.god_beast_grace_select_pet_seq or 0
	end
end

function RewardData:GodBeastGraceDayTimes(seq)
	return self.god_beast_grace_data.info.day_buy_times[seq] or 0
end

function RewardData:GodBeastGraceActivityTimes(seq)
	return self.god_beast_grace_data.info.activity_buy_times[seq] or 0
end

function RewardData:GodBeastGraceIsOpen()
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_GOD_BEAST_GRACE) then
		return false
	end
	if not FunOpen.Instance:IsFunOpen(Mod.Reward.GodBeastGrace) then
		return false
	end
	return true
end

function RewardData:GodBeastGraceAllRemind()
	if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_GOD_BEAST_GRACE) then
		return 0
	end
	if not FunOpen.Instance:IsFunOpen(Mod.Reward.GodBeastGrace) then
		return 0
	end
	local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_GOD_BEAST_GRACE)
	if tostring(act_start) ~= PrefsString(PrefKeys.GodBeastGraceAct()) then
		-- 活动首次红点，换设备重新提示
		return 1
	end
	if self.god_beast_grace_data.info.is_select == 1 then
		if tostring(act_start) ~= PrefsString(PrefKeys.GodBeastGraceSelected()) then
			-- 已选必得后红点，换设备重新提示
			return 1
		end
	end
	
	if self.god_beast_grace_data.info.is_select == 1 then
		-- 抽奖红点
		local has_num = Item.GetNum(Config.shenshouenze_auto.other[1].consume_item)
		if has_num >= Config.shenshouenze_auto.other[1].num then
			return 1
		end
		-- 领取宠物红点
		local pet_cfg = self:GodBeastGracePetCfg()[self.god_beast_grace_data.info.select_seq + 1]
		if self.god_beast_grace_data.lottery_times >= pet_cfg.times then
			return 1
		end
	end
	return 0
end

-- 清除神兽恩泽活动首次红点或者已选必得后红点
function RewardData:GodBeastGraceClearRemind(str)
	local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_GOD_BEAST_GRACE)
	if tostring(act_start) ~= PrefsString(str) then
		PrefsString(str, tostring(act_start))
		self.god_beast_grace_data:SetDirty("remind_flag")
	end
end

function RewardData:GodBeastGraceBaseCfg()
	local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_GOD_BEAST_GRACE, Config.shenshouenze_auto.reward, "seq")
    return cfg
end

function RewardData:GodBeastGracePetCfg()
	local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_GOD_BEAST_GRACE, Config.shenshouenze_auto.get_reward, "seq")
    return cfg
end

function RewardData:GodBeastGraceBuyCfg()
	local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_GOD_BEAST_GRACE, Config.shenshouenze_auto.prop, "seq")
    return cfg
end

function RewardData:GodBeastGraceLimitTimes(cfg)
	if cfg.limit_type == 1 then
		return cfg.limit_times - self:GodBeastGraceActivityTimes(cfg.seq)
	else
		return cfg.limit_times - self:GodBeastGraceDayTimes(cfg.seq)
	end
end

function RewardData:RewardBlock(obj)
	self.obj_reward_block = obj
end

function RewardData:SetRewardBlock(value)
	if self.obj_reward_block then
		self.obj_reward_block:SetActive(value)
	end
end

function RewardData:SetGodBeastGraceInfo(protocol)
	self.god_beast_grace_data.lottery_times = protocol.lottery_times
	self.god_beast_grace_data.info = protocol.info
end

function RewardData:SetGodBeastGraceResult(protocol)
	self.god_beast_grace_data.now_lottery_times = protocol.lottery_times - self.god_beast_grace_data.lottery_times
	self.god_beast_grace_data.lottery_times = protocol.lottery_times
	self.god_beast_grace_data.result = protocol.result
end

function RewardData:SetGodBeastGraceRecord(protocol)
	self.god_beast_grace_data.record = protocol.record
end
--  ========== 神兽恩泽 ============


