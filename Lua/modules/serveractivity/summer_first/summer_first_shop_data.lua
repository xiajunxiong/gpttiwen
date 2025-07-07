SummerFirstShopData = SummerFirstShopData or BaseClass()

-- 商城状态
SummerFirstShopState = {
	NotOpen = 0,	--未开启
	Open = 1,		--开启中
	Closed = 2,		--已关闭
}

function SummerFirstShopData:__init()
	if SummerFirstShopData.Instance ~= nil then
		Debuger.LogError("[SummerFirstShopData] attempt to create singleton twice!")
		return
	end
    SummerFirstShopData.Instance = self

	self:SetData()
	self.red_remind = SmartData.New({flag = false})
	self.shop_item = SmartData.New({now_use = {}})
    self.summer_first_shop_data = SmartData.New({today_buy_times = {}, activity_buy_times = {}, is_need_phase_flag = 1, flag = false})
    self.summer_discount_shop_data = SmartData.New({today_buy_times = {}, activity_buy_times = {}, discount_today_buy_times = {}, discount_activity_buy_times = {}, is_need_phase_flag = 1, flag = false})
end

function SummerFirstShopData:__delete()
	SummerFirstShopData.Instance = nil
end

function SummerFirstShopData:SetData(shop_config, other_config, shop_name_config, act_type, buy_item_type, random_group)
	self.act_type = act_type
	self.buy_item_type = buy_item_type
	self.other_config = other_config
	self.shop_name_config = shop_name_config
	-- 这个是总的
	self.shop_config = {}
	-- 这个用于展示
	self.show_config = {}
	self.random_group = random_group
	self.phase = 0
	self.now_buy_item = nil
	if shop_config then
		self:DealConfig(shop_config)
	end
end

function SummerFirstShopData:DealConfig(shop_config)
	local all_tab = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type, shop_config, "type")
	for i, v in ipairs(all_tab) do
		if not self.shop_config[v.phase] then
			self.shop_config[v.phase] = {}
			self.shop_config[v.phase].index = i
			self.shop_config[v.phase].phase = v.phase
			self.shop_config[v.phase].open_time = nil
			self.shop_config[v.phase].state = SummerFirstShopState.NotOpen
			self.shop_config[v.phase].start_time = v.start_time
			self.shop_config[v.phase].end_time = v.end_time
			self.shop_config[v.phase].phase_tab = {}
		end
		v.is_last = true
		v.times = 0
		table.insert(self.shop_config[v.phase].phase_tab, v)
	end
	-- 超级折扣商城特殊处理
	if self.random_group then
		for k, v in pairs(self.summer_discount_shop_data.rand_discount_list) do
			if v ~= 0 then
				local phase = math.ceil(k / 5)
				self.random_group[v].index = k - 1
				self.random_group[v].seq = (k + 4) % 5
				table.insert(self.shop_config[phase].phase_tab, self.random_group[v])
			end
		end
	end
end

-- 计算商城开启阶段
function SummerFirstShopData:CalculatePhase()
	if self.phase == 0 or self:GetNowSmartData().is_need_phase_flag == 1 then
		self.phase = 0
		local open_time = TimeCtrl.Instance:ChangDataTime(ActivityData.Instance:GetActivityStatusInfo(self.act_type).param_1)
		local act_open_day = TimeCtrl.Instance:ActOpenDay(self.act_type)
		for i, v in ipairs(self.shop_config) do
			if act_open_day >= v.start_time and act_open_day <= v.end_time then
				self.phase = v.phase
				v.state = SummerFirstShopState.Open
			elseif act_open_day > v.end_time then
				v.state = SummerFirstShopState.Closed
			else
				v.state = SummerFirstShopState.NotOpen
				v.open_time = TimeCtrl.Instance:ChangDataTimeDetail(open_time + 86400 * (v.start_time - 1))
			end
		end
		-- 开启完的阶段要后置
		table.sort(self.shop_config, DataHelper.WeightSort("phase", function(data)
			if data.state == SummerFirstShopState.Closed then
				return 100
			end
		end))
		-- 如果阶段数大于3，就要做处理，运营需求只显示3个
		local shop_total_num = #self.shop_config
		if self.other_config.phase > 3 then
			self.show_config = {}
			if self.phase == 1 then
				for i = 1, 3 do
					table.insert(self.show_config, self.shop_config[i])
				end
			elseif self.phase == #self.shop_config then
				table.insert(self.show_config, self.shop_config[shop_total_num])
				for i = shop_total_num - 1, shop_total_num do
					table.insert(self.show_config, self.shop_config[i])
				end
			else
				for i = 1, 2 do
					table.insert(self.show_config, self.shop_config[i])
				end
				table.insert(self.show_config, self.shop_config[shop_total_num])
			end
		else
			self.show_config = self.shop_config
		end
		-- 排序后要重新赋值index
		for i, v in ipairs(self.show_config) do
			v.index = i
		end
    end
end

-- 处理商店道具
function SummerFirstShopData:DealPhaseTab(data)
	for k, v in pairs(data.phase_tab) do
		v.is_last = self.phase ~= data.phase
		if v.limit_type == 1 then
			if v.reward_group then
				v.times = v.is_last and v.buy_times or v.buy_times - self:GetNowSmartData().discount_activity_buy_times[v.index + 1]
			else
				v.times = v.is_last and v.buy_times or v.buy_times - self:GetNowSmartData().activity_buy_times[v.index + 1]
			end
		elseif v.limit_type == 2 then
			if v.reward_group then
				v.times = v.is_last and v.today_times or v.today_times - self:GetNowSmartData().discount_today_buy_times[v.seq + 1]
			else
				v.times = v.is_last and v.today_times or v.today_times - self:GetNowSmartData().today_buy_times[v.seq + 1]
			end
		end
	end
	table.sort(data.phase_tab, DataHelper.WeightSort("index", function(data)
		if data.reward_group then
			return data.times == 0 and 1000 or -1000
		end
		return data.times == 0 and 2000 or 0
	end))
	-- 排序后要重新赋值item_index
	for i, v in ipairs(data.phase_tab) do
		v.item_index = i
	end
end

function SummerFirstShopData:GetNowSmartData()
	if ACTIVITY_TYPE.RAND_SUMMER_FIRST_SHOP == self.act_type then
		return self.summer_first_shop_data
	elseif ACTIVITY_TYPE.RAND_SUMMER_DISCOUNT_SHOP == self.act_type then
		return self.summer_discount_shop_data
	end
end

function SummerFirstShopData:GetShopName(index)
	
	if self.shop_name_config[index] then
		return self.shop_name_config[index].store_name
	elseif self.shop_name_config[1] then
		return self.shop_name_config[1].store_name
	end
	return ""
end

function SummerFirstShopData:GetShopIconName(index)
	if self.shop_name_config[index] then
		return self.shop_name_config[index].store_icon_name
	end
	return "store_1"
end


-- 每日登陆会有红点
function SummerFirstShopData:GetSummerFirstShopRemind(act_type)
	local role_id = RoleData.Instance:GetRoleId()
	local key = PrefKeys.ActivityIsRemind(role_id, act_type)
	local t0 = TimeManager.FormatUnixTime2Date(os.time())
	local value = Format(Language.SummerFirstShop.RemindValue, ActivityData.Instance:GetActivityStatusInfo(act_type).param_1, t0.year, t0.month, t0.day)
	return UnityPlayerPrefs.GetString(key) == tostring(value) and 0 or 1
end

-- 夏日商城全部信息下发
function SummerFirstShopData:SetSummerFirstShopInfo(protocol)
	self.summer_first_shop_data.today_buy_times = protocol.today_buy_times
	self.summer_first_shop_data.activity_buy_times = protocol.activity_buy_times
	self.summer_first_shop_data.is_need_phase_flag = protocol.is_need_phase_flag
end

-- 夏日商城单个信息下发
function SummerFirstShopData:SetSummerFirstShopSingleInfo(protocol)
	self.summer_first_shop_data.today_buy_times[protocol.seq + 1] = protocol.today_buy_times
	self.summer_first_shop_data.activity_buy_times[protocol.index + 1] = protocol.activity_buy_times
	self.summer_first_shop_data.flag = not self.summer_first_shop_data.flag
end

-- 超级折扣商城全部信息下发
function SummerFirstShopData:SetSummerDiscountShopInfo(protocol)
	self.summer_discount_shop_data.today_buy_times = protocol.today_normal_buy_times
	self.summer_discount_shop_data.activity_buy_times = protocol.activity_normal_buy_times
	self.summer_discount_shop_data.is_need_phase_flag = protocol.is_need_phase_flag
	self.summer_discount_shop_data.rand_discount_list = protocol.phase_rand_disc_item_list
	self.summer_discount_shop_data.discount_today_buy_times = protocol.today_dis_buy_times
	self.summer_discount_shop_data.discount_activity_buy_times = protocol.activity_dis_buy_times
end

-- 超级折扣商城单个信息下发
function SummerFirstShopData:SetSummerDiscountShopSingleInfo(protocol)
	-- 1:正常货架 param1:seq(每日) param2:index(活动)
    -- 2:折扣货架 param1:index(每日) param2:seq(活动)
	if protocol.notice_type == 1 then
		self.summer_discount_shop_data.today_buy_times[protocol.param1 + 1] = protocol.today_buy_times
		self.summer_discount_shop_data.activity_buy_times[protocol.param2 + 1] = protocol.activity_buy_times
	elseif protocol.notice_type == 2 then
		self.summer_discount_shop_data.discount_today_buy_times[protocol.param1 + 1] = protocol.today_buy_times
		self.summer_discount_shop_data.discount_activity_buy_times[protocol.param2 + 1] = protocol.activity_buy_times
	end
	self.summer_discount_shop_data.flag = not self.summer_discount_shop_data.flag
end