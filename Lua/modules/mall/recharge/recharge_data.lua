RechargeData = RechargeData or BaseClass()

function RechargeData:__init()
	if RechargeData.Instance ~= nil then
		Debuger.LogError("[RechargeData] attempt to create singleton twice!")
		return
	end
	RechargeData.Instance = self
	self.currency_type = 0--0人名币1新台币2美金
	self.reward_data = SmartData.New({list = {}})
	self.recharge_data = SmartData.New({list = {}})
	self:InitConfig()
end

function RechargeData:InitConfig()
	local reward_list = {}
	for k,v in pairs(Config.recharge_cfg_auto.reward_0) do
		local v_copy = TableCopy(v)
		v_copy.extra_reward = v.first_reward_num
		v_copy.money = v.rmb_show
		v_copy.dollar_show = tonumber(string.format("%.2f", v.dollar_show + 1e-6))
		-- LogDG("计算后保留两位小数的美元 = ", v_copy.dollar_show)
		table.insert(reward_list, v_copy)
	end
	table.sort(reward_list, DataSorter.KeyLowerSorter("money"))
	Config.recharge_cfg_auto.reward_0 = reward_list
end

function RechargeData:SetRechargeConfigInfo(protocol)
	self.currency_type = protocol.currency_type or 0
	self.recharge_data.list = self:GetAuditList(protocol.recharge_list)
end

function RechargeData:GetExtraReward(index)
	local info = self.recharge_data.list[index]
	return info and info.extra_reward or 0
end

-- 添加审核版本控制充值列表的显示处理
-- IOS版本也屏蔽掉大于1000充值金额列表
function RechargeData:GetAuditList(list)
	if IS_AUDIT_VERSION or IS_IOS() then
		local check_list = nil
		if IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan then
			list = Config.recharge_cfg_auto.reward_0
			if GLOBAL_CONFIG:Urls().recharge_list ~= nil and not StringIsEmpty(GLOBAL_CONFIG:Urls().recharge_list) then
				for k, v in pairs(GLOBAL_CONFIG:Urls().recharge_list) do
					check_list = check_list or {}
					check_list[v] = 1
				end
			end
		end
		local t = {}
		for i, v in ipairs(list) do
			if v.money < 1000 then
				if IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan then
					v.index = i
					if check_list == nil or check_list[v.money] == 1 then
						table.insert(t, v)
					end
				else
					table.insert(t, v)
				end
				
			end
		end
		return t
	end
	-- 从小到大排序
	table.sort(list, DataSorter.KeyLowerSorter("money"))
	return list
end

--获取充值列表
function RechargeData:GetRechargeList()
	return self.recharge_data.list
end

--获取充值类型 区分地区 金额类型（0 ：人民币 1：新台币 2 ：美金）
function RechargeData:GetCurrencyType()
	return self.currency_type
end

--获取当前货币类型
function RechargeData:GetLocalType()
	if self:GetCurrencyType() == 0 then
		return "rmb"
	end
	if self:GetCurrencyType() == 2 then
		return "dollar"
	end
end

--获取当前货币符号
function RechargeData:GetLocalSign()
	return Language.Recharge.GoldType[self:GetLocalType()]
end

--获取当前货币汇率结果 货币转元宝
function RechargeData:MoneyToGold(money)
	return self:MoneyToRealGold(money)
	-- if self:GetCurrencyType() == 0 then
	-- 	return money * 10
	-- end
	-- return money
end

--获取当前货币汇率结果 元宝转货币
function RechargeData:GoldToMoney(gold)
	return self:GoldToRealMoney(gold)
	-- if self:GetCurrencyType() == 0 then
	-- 	return gold / 10
	-- end
	-- if self:GetCurrencyType() == 2 then
	-- 	return self:GoldToRealMoney(gold)
	-- end
	-- return gold
end
--根据具体档位的元宝获取实际价格
function RechargeData:GoldToRealMoney(gold)
	--比self:GetRechargeList() 全面
	local list = Config.recharge_cfg_auto.reward_0
	local money_type = nil
	if next(list) then
		for k, v in pairs(list) do
			if v.chongzhi == gold then
				money_type = Language.Recharge.MoneyType[self:GetCurrencyType()]
				if money_type then
					return v[money_type]
				end
			end
		end
	end
	LogDG("未获取到当前对应档位的实际价格，元宝档位：", gold)
	return gold / 10
end
--配置档位价格不得重复，重复必出问题
function RechargeData:MoneyToRealGold(money)
	local list = Config.recharge_cfg_auto.reward_0
	local money_type = nil
	if next(list) then
		for k, v in pairs(list) do
			money_type = Language.Recharge.MoneyType[self:GetCurrencyType()]
			if money == v[money_type] then
				return v.chongzhi
			end
		end
	end
	LogDG("未获取到当前价格对应的元宝档位，价格：", money)
	return money * 10
end

--===========================充值返利===========================
--获取首充奖励配置
function RechargeData:GetFirstData()
	local list = {}
	local level = self:GetResetLevel()
	for k,v in pairs(Config.recharge_cfg_auto.rebate_recharge) do
		if level >= v.start_level and level <= v.end_level then
			table.insert(list,v)
		elseif level >= v.start_level and v.end_level == 0 then
			table.insert(list, v)
		end
	end
	table.sort(list,function(a,b)
		return a.seq < b.seq
	end)
	return list
end

function RechargeData:SetChongzhiInfo(protocol)
	self.recharge_num = protocol.recharge_num or 0
	self.reward_data.list = bit:d2b(protocol.reward_fatch_flag or 0) or {}
	if FirstChargeData.Instance:GetFirstChargeFlag() == 0 then
		FirstChargeData.Instance.first_charge_red_point.val = self.recharge_num > 0
	else
		FirstChargeData.Instance.first_charge_red_point.val = false
	end
	self.today_recharge_num = protocol.today_recharge_num
	self.reset_level_num = protocol.reset_level_num
	SuperVipCtrl.Instance.data.red_piont_data:Notify()
	FirstChargeData.Instance.first_charge_red_point:Notify()
end

--获取充值的数量
function RechargeData:GetRechargeNum()
	return self.recharge_num or 0
end

--获取今日充值数量
function RechargeData:GetTodayRechargeNum()
	return self.today_recharge_num or 0
end

--获取充值返利领取状态
function RechargeData:GetRewardFatchFlag(index)
	return self.reward_data.list[32-index] or 0
end

function RechargeData:GetResetLevel()
	return self.reset_level_num or RoleData.Instance:GetRoleLevel()
end

--封装一个礼包和物品的数据
function RechargeData:GetRewardItemList(reward_list)
	local item_list = {}
	for i=0,#reward_list do
		if reward_list[i] then
			table.insert(item_list,BagData.Instance:ItemBuild(reward_list[i]))
		end
	end
	return item_list
end

--获取充值返利的红点
function RechargeData:GetRewardNum()
	local num = 0
    local recharge_num = self:GetTodayRechargeNum()
	for k,v in pairs(self:GetFirstData()) do
		if recharge_num >= v.gold and self:GetRewardFatchFlag(v.seq) == 0 then
			num = num + 1
		end
	end
	return num
end
--======================================================