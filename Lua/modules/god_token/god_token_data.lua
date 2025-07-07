GodTokenData = GodTokenData or BaseClass()

--奖励领取状态
GodTokenData.REWARD_STATE = {
	NOT = 0,     -- 不可领取
	CAN = 1,     -- 可以领取
	GETED = 2,  -- 已经领取
}
--选中的标签页 ToggleList的下标对应
GodTokenData.SELECT_TAB = {
	REWARD = 1,
	DAILY_TASK = 2,
	WEEKLY_TASK = 3,
	CONVERT = 4,
}

function GodTokenData:__init()
	if GodTokenData.Instance ~= nil then
		Debuger.LogError("[GodTokenData] attempt to create singleton twice!")
		return
	end	
	GodTokenData.Instance = self
	self.god_token_data = SmartData.New({
		time_seq = 0,
		is_open = 0,
		is_buy_token = 0,
		time_level = 0, 			    -- 活动开启时玩家的等级
		token_level = 0,
		login_day = 0,
		token_exp = 0,
		fetch_common_flag = {},
		fetch_senior_flag = {},
		daily_level = 0,
		weekly_level = 0,
		daily_type_t = {},        		-- 每日任务类型
		weekly_type_t = {},             -- 每周任务类型
		daily_type_finish_count = {},   -- 每日任务完成次数 -1 = 已领取
		weekly_type_finish_count = {},  -- 每周任务完成次数
		surplus_exp = 0,  				-- 溢出经验
		surplus_receive_count = 0,      -- 溢出经验可领取次数
		exchange_count = {},
		})
	self.tab_index_data = SmartData.New()

	self.cur_time_seq = 0 				--当前时间索引
	self.cur_convert_index = 1          --当前兑换选中的下标
	self:InitData()
end

function GodTokenData:__delete()
	self.god_token_data:Set({})
	self.god_token_data = nil
end

function GodTokenData:InitData()

	ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoFengShenZhanLing,function ()
			self.god_token_cfg = Config.fengshenzhanling_auto

			self.reward_cfg = {} 				--等级奖励配置
			self.reward_seq_cfg = {}            --等级奖励表，以self.reward_cfg细分出来的，seq为key
			self.convent_cfg = {}    			--兑换配置
			self.reward_show_cfg = {}    		--大奖展示
			self.daily_task_cfg = {}   			--每日任务配置
			self.weekly_tasl_cfg = {}  			--每周任务配置

			self:InitHelper(self.god_token_cfg.grade_reward,self.reward_cfg,"time_seq")
			self:InitHelper(self.god_token_cfg.itemexchange,self.convent_cfg,"time_seq")
			self:InitHelper(self.god_token_cfg.item_reward,self.reward_show_cfg,"sort_seq")
			self:InitHelper(self.god_token_cfg.dailytask,self.daily_task_cfg,"time_seq")
			self:InitHelper(self.god_token_cfg.weeklytasks,self.weekly_tasl_cfg,"time_seq")
			self.reward_seq_cfg = KeyList(self:GetRewardList(),"seq")
			Cfg.UnRegister("GodTokenResetTimeBySeq")
			Cfg.Register("GodTokenResetTimeBySeq",Cfg.RealTimeSearch(self.god_token_cfg.reset_time,"seq"))
			Cfg.UnRegister("GodTokenItemExchangeBySeq")
			Cfg.Register("GodTokenItemExchangeBySeq",Cfg.RealTimeSearch(self.convent_cfg,"seq"))
		end,
		true)
end
--数据整理
function GodTokenData:InitHelper(base_cfg,cfg,key)
	for i=1,#base_cfg do
		local v = base_cfg[i]
		v.index = i
		if cfg[v[key]] == nil then
			cfg[v[key]] = {}
		end
		table.insert(cfg[v[key]],v)
	end
end

function GodTokenData:SetSelectTab(ind)
	self.tab_index_data:Set({index = ind})
end

function GodTokenData:GetSelectTab()
	return self.tab_index_data.index or 1
end

function GodTokenData:SetGodTokenInfo(protocol)
	-- 服务端说初始化会下发个-1过来，重开活动的时候出现
	if protocol.time_seq == nil or protocol.time_seq < 0 then
		return
	end
	self.cur_time_seq = protocol.time_seq
	self.reward_seq_cfg = KeyList(self:GetRewardList(),"seq")
	self.god_token_data.time_seq = protocol.time_seq
	self.god_token_data.is_open = protocol.is_open
	self.god_token_data.is_buy_token = protocol.is_buy_token
	self.god_token_data.token_level = protocol.token_level
	self.god_token_data.login_day = protocol.login_day
	self.god_token_data.token_exp = protocol.token_exp
	self.god_token_data.fetch_common_flag = protocol.fetch_common_flag
	self.god_token_data.fetch_senior_flag = protocol.fetch_senior_flag
	self.god_token_data.daily_level = protocol.daily_level
	self.god_token_data.weekly_level = protocol.weekly_level
	self.god_token_data.daily_type_t = protocol.daily_type_t
	self.god_token_data.weekly_type_t = protocol.weekly_type_t
	self.god_token_data.daily_type_finish_count = protocol.daily_type_finish_count
	self.god_token_data.weekly_type_finish_count = protocol.weekly_type_finish_count
	self.god_token_data.exchange_count = protocol.exchange_count
	self.god_token_data.surplus_exp = protocol.surplus_exp
	self.god_token_data.surplus_receive_count = protocol.surplus_receive_count
	self.god_token_data.time_level = protocol.time_level
	--LogError("战令信息",self.god_token_data)
end

--是否是高级战令
function GodTokenData:IsBuyToken()
	if self.god_token_data.is_buy_token == 0 then
		return false
	else
 		return true
	end
end

function GodTokenData:TimeLevel()
	return self.god_token_data.time_level or 1
end

--获取经验价格
function GodTokenData:GetExpPrice()
	return self.god_token_cfg.other[1].buy_exp
end
--获取高级战令价格
function GodTokenData:GetGodTokenPrice()
	return self.god_token_cfg.other[1].buy_senior
end

--获取奖励列表 is_sort_geted : 是否把领过的排序到后面去  is_has_end_reward:是否有最后奖励单独立出来
function GodTokenData:GetRewardList(time_seq,is_sort_geted,is_has_end_reward)
	time_seq = time_seq or self.cur_time_seq
	if time_seq == nil then
		return {},{}
	end
	local cfg = self.reward_cfg[time_seq]
	if cfg == nil then
		LogError("服务端下发的封神战令time_seq有问题：time_seq =",time_seq)
		return {},{}
	end

	local list = {}
	local role_level = self:TimeLevel()
	for i,v in pairs(cfg) do
		if role_level >= v.open_level and role_level <= v.end_level then
			table.insert(list, v)
		end
	end

	if is_sort_geted then
		local geted_t = {}
		local not_get_t = {}
		local end_i = 0
		for i,v in pairs(list) do
			if self:GetOrdinaryRewardState(v.seq) == GodTokenData.REWARD_STATE.GETED and self:GetSeniorRewardState(v.seq) == GodTokenData.REWARD_STATE.GETED then
				v.is_geted = 1
			elseif self:GetOrdinaryRewardState(v.seq) == GodTokenData.REWARD_STATE.GETED and next(v.senior_item) == nil then
				v.is_geted = 1
			else
				v.is_geted = 0
			end
		end
		local end_item = list[#list]
		if is_has_end_reward then
			list[#list] = nil
		end
		return self:Sort(list,"is_geted","seq"),end_item
	else
		local end_item = list[#list]
		if not TableIsEmpty(self.reward_seq_cfg) then
			for i,v in ipairs(list) do
				if v.is_show and v.is_show == 1 then
					local state = self:IsBuyToken() and self:GetSeniorRewardState(v.seq) or self:GetOrdinaryRewardState(v.seq)
					if state ~= GodTokenData.REWARD_STATE.GETED then
						end_item = v
						break
					end
				end
			end
		end
		if is_has_end_reward then
			list[#list] = nil
		end
		return list,end_item
	end
end

--获取第一个普通奖励未领取奖励的下标，计算content的位置,sort_list:排序完毕后的list
function GodTokenData:GetOrdinaryRewardNotIndex(sort_list)
	local index = 1
	local count = #sort_list
	for i,v in pairs(sort_list) do
		local ordinary_state = self:GetOrdinaryRewardState(v.seq)
		local senior_state = self:GetSeniorRewardState(v.seq)
		if self:IsBuyToken() then
			if (ordinary_state == GodTokenData.REWARD_STATE.NOT or ordinary_state == GodTokenData.REWARD_STATE.CAN) or 
			(senior_state == GodTokenData.REWARD_STATE.NOT or senior_state == GodTokenData.REWARD_STATE.CAN) then
				index = i
				break
			end
		else
			if ordinary_state == GodTokenData.REWARD_STATE.NOT or ordinary_state == GodTokenData.REWARD_STATE.CAN then
				index = i
				break
			end
		end
	end
	if index < 4 then
		return 1
	end
	if index > count - 5 then
		index = count - 5
		return index   
	end
	return index - 3   -- -4 是因为前面还需要显示3个置灰的item
end
--排序
function GodTokenData:Sort(list,key,seq)
	local sort_func = function (a,b)
        if a[key] == b[key] and seq then
            return a[seq] < b[seq]
        else
            return a[key] < b[key]
        end
    end
	table.sort(list,sort_func)
	return list
end

--获取奖励
function GodTokenData:GetRewardBySeq(seq)
	return self.reward_seq_cfg[seq]
end

--获取大奖一览信息 seq == 1 精英大奖展示 seq==2 进阶大奖展示
function GodTokenData:GetRewardShowList(seq,is_convert_item)
	if is_convert_item then
		local reward_item_list = {}
		local cfg = self.reward_show_cfg[seq]
		for i,v in pairs(cfg) do
			reward_item_list[i] = Item.Create(v.item_reward)
		end
		return reward_item_list
	else
		return self.reward_show_cfg[seq]
	end
end
--获取活动结束/重置时间
function GodTokenData:GetEndTime(time_seq)
	time_seq = time_seq or self.cur_time_seq
	local next_seq = Valve(time_seq + 1,#self.god_token_cfg.reset_time - 1)
	return Cfg.GodTokenResetTimeBySeq(next_seq).reset_time
end

--获取每日/每周任务的已获得经验和总经验 task_type == 1 每日任务，2 每周任务
function GodTokenData:GetTaskExpInfo(task_type)
	task_type = task_type or 1
	local task_list = task_type == 1 and self:GetDailyTaskList() or self:GetWeeklyTaskList()
	local exp = 0
	local sum_exp = 0
	for i,v in pairs(task_list) do
		if v.sort == 2 then
			exp = exp + v.open_exp
		end
		sum_exp = sum_exp + v.open_exp
	end
	return exp, sum_exp
end

--获取每日任务列表
function GodTokenData:GetDailyTaskList(time_seq)
	time_seq = time_seq or self.cur_time_seq
	local cfg = self.daily_task_cfg[time_seq]
	if cfg == nil then
		LogError("服务端下发的封神战令time_seq有问题：time_seq =",time_seq)
		return {}
	end
	local level = self.god_token_data.daily_level
	local task_t = {}
	for i,v in pairs(self.god_token_data.daily_type_t) do
		for _,va in pairs(cfg) do
			if va.type == v then
				if level >= va.grade_start and (level <= va.grade_end or va.grade_end ==0) then
					local task_data = self:GetDailyTaskAccomplishCount(va.type)
				    if task_data.count < 0 then
				    	va.sort = 2
				    elseif task_data.count >= va.parameter then
				    	va.sort = 0
				    else
				    	va.sort = 1
				    end
				    va.task_tab = GodTokenData.SELECT_TAB.DAILY_TASK
				    va.get_index = task_data.index 
					table.insert(task_t,va)
			    end
			end
		end
	end
	task_t = self:Sort(task_t,"sort","seq")
	return task_t
end

--获取每日任务的达成次数
function GodTokenData:GetDailyTaskAccomplishCount(type)
	return self.god_token_data.daily_type_finish_count[type]
end

--获取每周任务列表
function GodTokenData:GetWeeklyTaskList(time_seq)
	time_seq = time_seq or self.cur_time_seq
	local cfg = self.weekly_tasl_cfg[time_seq]
	if cfg == nil then
		LogError("服务端下发的封神战令time_seq有问题：time_seq =",time_seq)
		return {}
	end
	local level = self.god_token_data.weekly_level
	local task_t = {}
	for i,v in pairs(self.god_token_data.weekly_type_t) do
		for _,va in pairs(cfg) do
			if va.type == v then
				if level >= va.grade_start and (level <= va.grade_end or va.grade_end ==0) then
					local  task_data = self:GetWeeklyTaskAccomplishCount(va.type)
					if task_data.count < 0 then
						va.sort = 2
				    elseif task_data.count >= va.parameter then
				    	va.sort = 0
				    else
				    	va.sort = 1
				    end
				    va.task_tab = GodTokenData.SELECT_TAB.WEEKLY_TASK
				    va.get_index = task_data.index
					table.insert(task_t,va)
			    end
			end
		end
	end
	task_t = self:Sort(task_t,"sort","seq")
	return task_t
end

--获取每周任务的达成次数
function GodTokenData:GetWeeklyTaskAccomplishCount(type)
	return self.god_token_data.weekly_type_finish_count[type]
end

--获取普通奖励领取状态
function GodTokenData:GetOrdinaryRewardState(seq)
	local lv = self.god_token_data.token_level
	local reward_data = self:GetRewardBySeq(seq)
	if lv >= reward_data.open_grade then
        local get_flag = self.god_token_data.fetch_common_flag[seq]
        if get_flag == 0 then
        	return GodTokenData.REWARD_STATE.CAN
        else
  			return GodTokenData.REWARD_STATE.GETED
        end
	else
		return GodTokenData.REWARD_STATE.NOT
	end
end

--获取高级奖励领取状态
function GodTokenData:GetSeniorRewardState(seq)
	if self.god_token_data.is_buy_token == 1 then
	    local lv = self.god_token_data.token_level
		local reward_data = self:GetRewardBySeq(seq)
		if lv >= reward_data.open_grade then
	        local get_flag = self.god_token_data.fetch_senior_flag[seq]
	        if get_flag == 0 then
	        	return GodTokenData.REWARD_STATE.CAN
	        else
	  			return GodTokenData.REWARD_STATE.GETED
	        end
		else
			return GodTokenData.REWARD_STATE.NOT
		end
	else
		return GodTokenData.REWARD_STATE.NOT
	end
end

--获取区间等级奖励列表
function GodTokenData:GetSectionRewardList(start_level,end_level,is_convert)
    local is_senior = self:IsBuyToken()
	local cfg = self:GetRewardList()
	local reward_list = {}
	for i=1,#cfg do
		local v = cfg[i]
		if v.open_grade >= start_level and v.open_grade <= end_level then
		    if is_convert then
		    	table.insert(reward_list,Item.Create(v.ordinary_item))
		    	if is_senior then
		    		for _,value in pairs(v.senior_item) do
		    			table.insert(reward_list,Item.Create(value))
		    		end
		    		if v.senior_gold_num and v.senior_gold_num > 0 then
		    			table.insert(reward_list,Item.Create({item_id = CommonItemId.Gold,num = v.senior_gold_num,is_bind = 0}))
		    		end
		    	end
		    else
		    	table.insert(reward_list,v.ordinary_item)
		    	if is_senior then
		    		for _,value in pairs(v.senior_item) do
		    			table.insert(reward_list,value)
		    		end
		    	end
		    end
	    end
	end
	return reward_list
end

--获取可升的最大的等级
function GodTokenData:GetCanMaxLevel()
	local cfg = self:GetRewardList()
	local max_level = #cfg - self.god_token_data.token_level
	return max_level
end

--获取x级至x级需要的经验 is_not_mimus:是否不减去当前经验
function GodTokenData:GetLevelNeedExp(cur_lv,next_lv,is_not_mimus)
	is_not_mimus = is_not_mimus or false
	local cfg = self:GetRewardList()
	local sum_lv_need_exp = 0
	for _,v in pairs(cfg) do
		if v.open_grade > cur_lv and  v.open_grade <= next_lv then
			sum_lv_need_exp =  sum_lv_need_exp + v.open_exp
		end
	end
	if is_not_mimus then
		return sum_lv_need_exp
	else
		return sum_lv_need_exp - self:GetCurExp()
	end
end

--获取下一级需要的经验
function GodTokenData:GetNextLevelNeedExp()
	return self:GetLevelNeedExp(self.god_token_data.token_level,self.god_token_data.token_level+1,true)
end

--获取当前经验
function GodTokenData:GetCurExp()
	return self.god_token_data.token_exp
end

--获取兑换列表
function GodTokenData:GetConventList(time_seq)
	time_seq = time_seq or self.cur_time_seq
	local list = {}
	for i,v in ipairs(self.convent_cfg[time_seq] or {}) do
		if self.god_token_data.time_level >= v.open_level then
			if v.end_level == nil or self.god_token_data.time_level <= v.end_level or v.end_level == 0 then
				table.insert(list,v)
			end
		end
	end
	return list
end

--获取兑换物品的限制状态 1 是达到可兑换条件，0 是战令等级过高 -1是战令等级过低
function GodTokenData:GetConvertItemState(data)
	local token_level = self.god_token_data.token_level
	if token_level < data.open_token then
		return -1
	-- elseif token_level > data.end_token then
	-- 	return 0
	else
		return 1
	end
end

--获取兑换的物品
function GodTokenData:GetConvertItem(seq)
	return Cfg.GodTokenItemExchangeBySeq(seq)
end

--获取物品兑换的次数
function GodTokenData:GetItemConvertCount(seq)
	return self.god_token_data.exchange_count[seq]
end

--红点逻辑
function GodTokenData:GetRemindNum()
	for i=1,3 do
		local red_num = self["GetReamindNum"..i](self)
		if red_num > 0 then
			return red_num
		end
	end
	if self.god_token_data.surplus_receive_count > 0 then
		return 1
	end
	return 0
end

--每日任务红点逻辑
function GodTokenData:GetReamindNum2()
   for _,v in pairs(self:GetDailyTaskList()) do
   		local task_data = self:GetDailyTaskAccomplishCount(v.type)
   		if task_data.count >= v.parameter then
   			return 1
   		end
   end
   return 0
end

--获取每周任务红点
function GodTokenData:GetReamindNum3()
	 for _,v in pairs(self:GetWeeklyTaskList()) do
   		local task_data = self:GetWeeklyTaskAccomplishCount(v.type)
   		if task_data.count >= v.parameter then
   			return 1
   		end
   end
   return 0
end

--奖励红点
function GodTokenData:GetReamindNum1()
	local num = 0
	local cfg = self:GetRewardList()
	for _,v in pairs(cfg or {}) do
		local is_senior_can_get = self:GetSeniorRewardState(v.seq)
		local is_ordinary_can_get = self:GetOrdinaryRewardState(v.seq)
		if is_senior_can_get == GodTokenData.REWARD_STATE.CAN or is_ordinary_can_get == GodTokenData.REWARD_STATE.CAN then
			return 1
		end
	end
	return num
end

--24点结束,获取任务刷新时间
function GodTokenData:GetTaskUpdateTime()
	local cut_time = os.date("%H-%M-%S",TimeCtrl.Instance:GetServerTime())
	local time_t = Split(cut_time,"-")
	local time = time_t[1] * 60 * 60 + time_t[2] * 60 + time_t[3]
	return 24 * 60 * 60 - time
end

--每周一刷新，获取每周任务刷新时间
function GodTokenData:GetWeeklyTaskUpdateTime()
	-- 使用基姆拉尔森计算公式 W= (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400) mod 7
	-- 把一月和二月看成是上一年的十三月和十四月，例：如果是2004-1-10则换算成：2003-13-10来代入公式计算。(此处有坑)
	-- local cut_time = os.date("%Y-%m-%d",TimeCtrl.Instance:GetServerTime())
	-- local time_t = Split(cut_time,"-")
	-- time_t[1] = tonumber(time_t[1])
	-- time_t[2] = tonumber(time_t[2])
	-- time_t[3] = tonumber(time_t[3])
	-- if time_t[2] <= 2 then
	-- 	time_t[1] = time_t[1] - 1
	-- 	time_t[2] = time_t[2] + 12
	-- end
	-- local week=(time_t[3]+2*time_t[2]+3*(time_t[2]+1)/5+time_t[1]+time_t[1]/4-time_t[1]/100+time_t[1]/400)%7+1
	-- week = math.floor(week)
	-- return (7-week) * 24 * 60 * 60 + self:GetTaskUpdateTime()

	-- 新的写法
	local week_num = ActivityData.Instance:GetWeekNum()
	return (7 - week_num) * 24 * 60 * 60 + self:GetTaskUpdateTime()
end

--获取溢出精英奖励
function GodTokenData:GetOrdinarySurplusReward()
	return self.god_token_cfg.surplus_reward[1].ordinary_item
end

--获取溢出进阶奖励
function GodTokenData:GetSeniorSurplusReward()
	return self.god_token_cfg.surplus_reward[1].senior_item
end

--获取溢出奖励需多少经验触发
function GodTokenData:GetSurplusNeedExp()
	return self.god_token_cfg.surplus_reward[1].open_exp
end

--每日任务是否全部完成
function GodTokenData:GetDailyTaskIsAllAccomplish()
	for _,v in pairs(self:GetDailyTaskList()) do
   		local task_data = self:GetDailyTaskAccomplishCount(v.type)
   		if task_data.count < v.parameter and task_data.count >= 0 then
   			return false
   		end
    end
    return true
end

-- 还有多少天结算赛季
function GodTokenData:GetEndDay()
	local end_time = self:GetEndTime()
	local cur_time = TimeCtrl.Instance:GetServerTime()
	local day = (end_time - cur_time) / 86400
	return day
end

