ActivityRandData = ActivityRandData or BaseClass()

function ActivityRandData:__init()
    if ActivityRandData.Instance ~= nil then
        Debuger.LogError("[ActivityRandData] attempt to create singleton twice!")
        return
    end
	ActivityRandData.Instance = self
	self:InitActivityRandData()
	ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoActivityMain,BindTool.Bind(self.InitActivityRandList, self),true)
end

function ActivityRandData:InitActivityRandData()
	self.register_list = {}
	self.origin_list = {}
	self.origin_handle = {}
	self.activity_click_list = {}
	self.activity_count_down_list = {}
	self.activity_base_data_list = {}

	self.rand_child_data = SmartData.New({val = false})
	self.rand_open_data = SmartData.New({val = false})
	self.all_config_key = [[search_config_list]]
end

function ActivityRandData:InitActivityRandList()
	self.main_config = {}
	self.acts_config = {}
	self.battle_config = {}
	for k,v in pairs(Config.activity_main_auto.rand) do
		if v.position == 1 then
			table.insert(self.acts_config,v)
		elseif v.position == 2 then
			table.insert(self.main_config,v)
		end
	end
	for i, v in ipairs(Config.activity_main_auto.battle_show) do
		table.insert(self.battle_config, v)
	end
	if #self.acts_config > 1 then
		table.sort(self.acts_config,function(a,b)return a.seq < b.seq end)
	end
	if #self.main_config > 1 then
		table.sort(self.main_config,function(a,b)return a.seq < b.seq end)
	end
	if #self.battle_config > 1 then
		table.sort(self.battle_config,function(a,b)return a.weight > b.weight end)
	end
	self.activity_btn_list = nil
	self.all_config_list = {
		Config.activity_main_auto.rand,           --精彩活动
		Config.activity_main_auto.reward_list,    --奖励活动
		Config.activity_main_auto.open_sever_cfg, --新服活动
		Config.activity_main_auto.spring_festival,--新春活动-温情端午-万圣节
		Config.activity_main_auto.burning_may_day,--五一活动
		Config.activity_main_auto.summer_first,   --暑期活动
		Config.activity_main_auto.national_day,   --国庆活动
		Config.activity_main_auto.plan_activity,  --万灵共盈
		Config.activity_main_auto.recall_summary, --山海回想
		Config.activity_main_auto.yunzemengce,    --云泽梦册
		Config.activity_main_auto.tiger_spring_festival,  --虎年春节
		Config.activity_main_auto.yuanqiqingdian,  --元气庆典
		Config.activity_main_auto.duanwu_festival, --端午活动
		Config.activity_main_auto.charm_tanabata,  --七夕活动
        Config.activity_main_auto.moon_mid_autumn -- 月满中秋
	}
	self:InitFinishTaskConfig()
	self:InitBaseDateConfig()
end

function ActivityRandData:InitFinishTaskConfig()
	self.finish_task_config = {}
	for _,config in pairs(self.all_config_list) do
		for k,v in pairs(config) do
			local data = FunOpen.Instance:GetOpenData(v.mod_key or 0)
			if data and data.task ~= 0 then
				self.finish_task_config[v.mod_key] = data.task
			end
		end
	end
	for k,v in pairs(Config.activity_main_auto.btn_list) do
		local data = FunOpen.Instance:GetOpenData(v.mod_key or 0)
		if data and data.task ~= 0 then
			self.finish_task_config[v.mod_key] = data.task
		end
	end
end

function ActivityRandData:InitBaseDateConfig()
	self.base_date_config = {}
	local function T(content)
		local t = Split(content,"_") or {}
		return {year = tonumber(t[1]),month = tonumber(t[2]),day = tonumber(t[3])}
	end
	for k,v in pairs(Config.randactivityopencfg_1_auto.base_on_day_cfg) do
		if self.base_date_config[v.activity_type] == nil then
			self.base_date_config[v.activity_type] = {}
		end
		local param_t = {}
		param_t.open_time = TimeCtrl.Instance:ChangDataStampTime(T(v.begin_day))
		param_t.end_time = TimeCtrl.Instance:ChangDataStampTime(T(v.end_day))
		table.insert(self.base_date_config[v.activity_type],param_t)
	end
	for k,v in pairs(self.base_date_config) do
		table.sort(v,DataHelper.SortFunc([[open_time]]))
	end
end

--外部不可调用
function ActivityRandData:GetFinishTaskConfig()
	return self.finish_task_config or {}
end

--精彩活动开启check
function ActivityRandData:CheckGroupOpen()
	for k,v in pairs(self.acts_config) do
		if self:IsRandOpen(v) then
			return true
		end
	end
	return false
end

--主界面精彩活动 手动关闭 弹窗，前往NPC时可用
function ActivityRandData:CheckRandChildData()
	self.rand_child_data.val = not self.rand_child_data.val
end

--主界面随机活动图标强制手动刷新
function ActivityRandData:CheckRandOpenData()
	self.rand_open_data.val = not self.rand_open_data.val
end

-- 主界面固定图标列表
function ActivityRandData:GetBtnList()
	--if self.activity_btn_list == nil then
		self.activity_btn_list = {}
		for i,v in ipairs(Config.activity_main_auto.btn_list) do
			local func_open = FunOpen.Instance:IsFunOpen(v.mod_key)
			if v.is_open == 1 and func_open and self:GetRegisterOpen(v.mod_key) then
				table.insert(self.activity_btn_list,TableCopy(v))
			end
		end
		for k,v in pairs(self.activity_btn_list) do
			if tonumber(v.mod) then
				v.mod = TabMod(v.mod)
			else
				v.mod = Mod[v.mod]
			end
		end
	--end
	return self.activity_btn_list
end

-- 主界面固定图标是否开启
function ActivityRandData:GetBtnListIsOpen(mod_key)
	for k, v in pairs(Config.activity_main_auto.btn_list) do
		if v.mod_key == mod_key then
			return v.is_open == 1
		end
	end
	return false
end
--外部不可调用
function ActivityRandData:GetRegisterOpen(mod)
	local open_func = self.register_list[mod]
	if open_func then
		return open_func() and self:IsFunOpen(mod)
	end
	return true
end

--获取开启列表
function ActivityRandData:GetRandList(config)
    local list = {}
	for k,v in pairs(config) do
		if self:IsRandOpen(v) then
			v.mod = TabMod(v.mod_key)
			table.insert(list,v)
		end
	end
	self.activity_rand_list = list
	return list
end

--获取精彩活动随机活动表
function ActivityRandData:GetRandDataList()
	return self:GetRandList(self.acts_config)
end

--获取主界面随机活动列表
function ActivityRandData:GetRandMainList()
	return self:GetRandList(self.main_config)
end

--获取战斗中显示随机活动列表
function ActivityRandData:GetRandInBattle()
	local list = {}
	local num = 0
	for i, v in ipairs(self.battle_config) do
		if self:IsRandOpen(v) then
			v.mod = TabMod(v.mod_key)
			table.insert(list, v)
		end
	end 
	if #list > 16 then
		local temp = {}
		for i = 1, 16 do
			temp[i] = list[i]
		end
		list = temp
	end
	return list
end

--获取某个模块是否开启
function ActivityRandData:IsRandOpen(param)
	return self:IsFunOpen(param.mod_key) and ((param.act_type == nil or param.act_type == 0) and true or ActivityData.IsOpen(param.act_type)) and self:GetRegisterOpen(param.mod_key)
end
--外部不可调用
function ActivityRandData:IsFunOpen(modkey)
    if FunOpen.Instance:GetOpenData(modkey) then
        return FunOpen.Instance:GetFunIsOpened(modkey)
    end
    return true
end
--外部不可调用
function ActivityRandData:GetSplendidTip()
	for k, v in pairs(self.activity_rand_list or {}) do
        if ActivityData.Instance:IsHasSign(v.act_type) then
            return true
        end
	end
	return false
end

--外部提供接口使用 获取随机活动名字



function ActivityRandData.GetName(act_type)
	local config = ActivityRandData.Instance:GetConfig(act_type)
	--危险！！！！！！！！！！！！！！！！！公用接口求求不要在这加自己模块的特殊处理
	-- if config then
		-- if act_type == ACTIVITY_TYPE.RAND_JIN_LONG_JU_BAO then
		-- 	local text_list = Split(config.text,"|")
        -- 	local type = GoldRMinecData.Instance:ActivityType()
		-- 	return text_list[type + 1] or ""
		-- end
	-- end
	---！！！！！！！！！！！！！！！！！
	return config and (ActivityRandData.Instance:GetBaseItemName(config) or config.text or config.name or "") or ""
end

--根据act_type获取获得随机活动配置 获取随机活动配置
function ActivityRandData:GetConfig(act_type)
	if act_type and act_type <= SERVER_ACTIVITY_MIN_NUM then
		return
	end
	return CacheList(self,self.all_config_key,act_type,function()
		for _,config in pairs(self.all_config_list) do
			for k,v in pairs(config) do
				if v.act_type == act_type then
					return v
				end
			end
		end
		-- BuglyReport(string.format("随机活动类型=%s的配置不存在",act_type))
	end)
end

--根据mod_key获取活动配置 获取随机活动配置
function ActivityRandData:GetConfigByKey(mod_key)
	return CacheList(self,self.all_config_key,mod_key,function()
		for _,config in pairs(self.all_config_list) do
			for k,v in pairs(config) do
				if v.mod_key == mod_key then
					return v
				end
			end
		end
		-- BuglyReport(string.format("随机活动模块=%s的配置不存在",mod_key))
	end)
end
--外部不可调用
function ActivityRandData.GetBtnConfig(open)
	for k,v in pairs(Config.activity_main_auto.btn_list) do
        if v.open == open then
            return v
        end
    end
end

--外部获取注册自己的监听方式
function ActivityRandData:RegisterCare(modkey,func)
	local check_func = self.register_list[modkey]
	local handle = self.origin_list[modkey]:Care(func)
	self.origin_handle[handle] = self.origin_list[modkey]
	return handle
end

--注销自己的监听源
function ActivityRandData:UnRegisterCare(handle)
	if handle and self.origin_handle[handle] then
		self.origin_handle[handle]:Uncare(handle)
	end
end

--是否有注册
function ActivityRandData:IsRegister(modkey)
	return self.register_list[modkey] ~= nil
end

--是否有自己的刷新源
function ActivityRandData:IsHasOrigin(modkey)
	return self.origin_list[modkey] ~= nil
end

--获取随机活动开启 配置 固定节日开启时间的 比如国庆节 其他活动不可用
--其他活动用 ActivityRandData.Instance:GetConfig(act_type)或者 ActivityRandData.Instance:GetConfigByKey(mod_key)
function ActivityRandData:GetBaseDayConfig(act_type)
	return self.base_date_config[act_type]
end

--获取随机活动开启和结束时间戳 TimeManager:GetServerTime() 固定节日开启时间的 比如国庆节 其他活动不可用
--获取活动在Config.randactivityopencfg_1_auto.base_on_day_cfg具体的开启日期时间戳 其他不可用
--其他活动 获取活动开启时间戳用 ActivityData.GetStartStampTime(act_type)
--其他活动 获取活动结束时间戳用 ActivityData.GetSwitchStampTime(act_type)
function ActivityRandData.GetBaseDayOpenStampTime(act_type)
	local curr_time = TimeHelper:GetServerTime()
	local config = ActivityRandData.Instance:GetBaseDayConfig(act_type)
	for i,v in ipairs(config or {}) do
		if IsRange(curr_time,v.open_time,v.end_time) then
			return v.open_time,v.end_time
		else
			if curr_time < v.open_time then
				return v.open_time,v.end_time
			end
		end
	end
	if not IsEmpty(config) then
		return config[1].open_time,config[1].end_time
	end
end

--活动打开或者关闭 switch = true  打开 false 关闭
function ActivityRandData.SwitchView(act_type,switch)
	local config = ActivityRandData.Instance:GetConfig(act_type)
	if config and GetView(config.view_name) then
		if switch then
			local key, param_t = Mod.ParseKey(config.mod_key)
			local opened, open_tip = FunOpen.Instance:GetFunIsOpened(config.mod_key or 0)
			if not opened then
				PublicPopupCtrl.Instance:Center(open_tip)
				return
			end
			if not ActivityRandData.Instance:GetRegisterOpen(config.mod_key) then
				return
			end
			ViewMgr:OpenView(GetView(config.view_name),param_t)
		else
			ViewMgr:CloseView(GetView(config.view_name))
		end
	end
end

--设置活动自己的提醒方式
function ActivityRandData.SetRemind(act_type,value)
	UnityPlayerPrefs.SetInt(PrefKeys.ActivityIsRemind(RoleId(),act_type),value)
end

--获取活动自己的提醒方式
function ActivityRandData.GetRemind(act_type)
	return UnityPlayerPrefs.GetInt(PrefKeys.ActivityIsRemind(RoleId(),act_type))
end

--获取活动下一阶段开启的时间戳
function ActivityRandData.GetNextStampTime(act_type,config,phase)
	local result = phase and DataHelper.GetSearch(config,[[phase]],phase) or config
	local t = os.date([[*t]], ActivityData.GetStartStampTime(act_type))
	t.day = t.day + (result.end_time or result.section_end)
	return TimeCtrl.Instance:ChangDataStampTime(t)
end

--=======================注册接口===============================
--外部注册自己的图标解锁条件
function ActivityRandData:Register(modkey,check_func,origin)
	if self.register_list[modkey] == nil then
		self.register_list[modkey] = check_func
	end
	if not self.origin_list[modkey] and origin then
		self.origin_list[modkey] = origin
	end
end

-- 活动前往按钮注册,不走默认行为,根据协议行为改变的注册
function ActivityRandData:CustomClickHandle(act_type, func)
	self.activity_click_list[act_type] = func
end

-- 活动前往按钮注册,不走默认行为,根据协议行为改变的注册 mod_key
function ActivityRandData:CustomClickModHandle(mod_key, func)
	self.activity_click_list[mod_key] = func
end

--获取活动自己的点击前往事件 key为act_type 或 mod_key
function ActivityRandData:OnClickHandle(key)
	if self.activity_click_list[key] then
		Call(self.activity_click_list[key])
		return true
	end
end

-- 注册主界面活动按钮下面倒计时 Time
function ActivityRandData:RegisterActCountDown(mod_key, func)
	self.activity_count_down_list[mod_key or 0] = func
end

function ActivityRandData:GetActCountDown(mod_key)
	return self.activity_count_down_list[mod_key or 0]
end

-- 注册或解析自己基本的图标和图标名字（存在动态名字或需要替换图标,需要返回{sprite = xxx,text = xxx}）
function ActivityRandData:CustomBaseItemData(mod_key, func)
	self.activity_base_data_list[mod_key or 0] = func
end

function ActivityRandData:GetBaseItemData(mod_key)
	return self.activity_base_data_list[mod_key or 0]
end

function ActivityRandData:GetBaseItemName(data)
	local func = self:GetBaseItemData(data.mod_key)
    local data = func and func(data) or nil
	return data and data.text or nil
end