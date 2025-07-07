VIEW_REQ("modules/serveractivity/plan_activity/plan_activity_view")

PlanActivityCtrl = PlanActivityCtrl or BaseClass(BaseCtrl)

function PlanActivityCtrl:__init()
	if PlanActivityCtrl.Instance ~= nil then
		Debuger.LogError("[PlanActivityCtrl] attempt to create singleton twice!")
		return
	end	
    PlanActivityCtrl.Instance = self
    self.data = PlanActivityData.New()
end

--返回值 true为成功跳转界面,处理关闭界面
function PlanActivityCtrl:ActGetWayViewById(act_type, item_id)
	local cfg_list = self:ActGetWayCfgListById(act_type, item_id)
	for i, jump_cfg in ipairs(cfg_list) do
		if jump_cfg.limit_type == 1 then --打开道具详情页
			MainOtherCtrl.Instance:GetWayViewById(jump_cfg.item_id)
		elseif jump_cfg.limit_type == 2 then --跳转活动主界面
			if ActivityData.IsOpen(jump_cfg.act_type) then
				-- ActivityRandData.SwitchView(jump_cfg.act_type, true)
				local config = ActivityRandData.Instance:GetConfig(jump_cfg.act_type)
				if config and GetView(config.view_name) then
					local key, param_t = Mod.ParseKey(config.mod_key)
					local opened, open_tip = FunOpen.Instance:GetFunIsOpened(config.mod_key or 0)
					if not opened then
						PublicPopupCtrl.Instance:Center(open_tip)
						return
					end
					if not ActivityRandData.Instance:GetRegisterOpen(config.mod_key) then
						return
					end
					ViewMgr:OpenView(GetView(config.view_name), param_t)
					return true
				end
			else
				if i == #cfg_list then
					PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
				end
			end
		end
	end
end

function PlanActivityCtrl:ActGetWayCfgListById(act_type, item_id)
	local cfg_list = {}
	for i, v in pairs(Config.wanlingyinxiang_auto.rand_pool_jump) do
		if v.activity == act_type and (item_id == nil or item_id == v.item_id) then
			table.insert(cfg_list, v)
		end
	end
	return cfg_list
end

--排期汇总-活跃道具跳转
function PlanActivityCtrl:ActGetWayCfgById(act_type, item_id)
	for i, v in pairs(Config.wanlingyinxiang_auto.rand_pool_jump) do
		if v.activity == act_type and (item_id == nil or item_id == v.item_id) then
			return v
		end
	end
	if DEBUG_MODE then
		LogError(Format("未在【活动配置->活跃道具跳转】找到【%s】活动的获取道具跳转  item_id -> %s", act_type, (item_id or "nil")))
	end
end

PlanActivityData = PlanActivityData or BaseClass()

function PlanActivityData:__init()
    if PlanActivityData.Instance ~= nil then
        Debuger.LogError("[PlanActivityData] attempt to create singleton twice!")
        return
    end
    PlanActivityData.Instance = self
	self.plan_config = Config.activity_main_auto.plan_activity
    self.mountain_sea_config = Config.activity_main_auto.recall_summary

	self:CacheSpecialMod()
end

function PlanActivityData:GetPlanActivityTip()
	for k, v in pairs(ActivityRandData.Instance.activity_rand_list or {}) do
		local config = Config.wanlingyinxiang_auto.rand_pool
		for k1,v1 in pairs(config) do
			if v1.act_type == v.act_type then
				if v1.parameter == 1 then
					if ActivityData.Instance:IsHasSign(v.act_type) then
						if ACTIVITY_TYPE.RAND_HELP_OTHERS == v.act_type then --桃源一三事开启时间特殊处理
							local open_day = PlanActivityData.Instance:GetPlanActivityOpenDay()
							if open_day >= v1.section_start and open_day <= v1.section_end then
								return true
							end
						else
							return true
						end
					end
				end
			end
		end
	end
	return false
end

function PlanActivityData:GetPlanDayOpenStampTime(act_type)
	local info_list = {}
	local config = nil
	for k,v in pairs(Config.randactivityopencfg_1_auto.base_on_day_cfg) do
		if v.activity_type == act_type then
			table.insert(info_list,v)
		end
	end
	local function T(content)
		local t = Split(content,"_") or {}
		return {year = tonumber(t[1]),month = tonumber(t[2]),day = tonumber(t[3])}
	end
	for k,v in pairs(info_list) do
		if TimeCtrl.Instance:ChangDataStampTime(T(v.begin_day)) > TimeHelper.GetServerTime() then
			config = v
			break
		end
	end
	if config == nil then
		return 0,0
	end
	local open_time = TimeCtrl.Instance:ChangDataStampTime(T(config.begin_day)) or 0
	local end_time = TimeCtrl.Instance:ChangDataStampTime(T(config.end_day)) or 0
	if end_time > 0 then
		end_time = end_time - 1
	end
	return open_time,end_time
end

function PlanActivityData:GetActBeginAndEndAndDayTime()
	local act_type = 0
	local config = Config.wanlingyinxiang_auto.rand_pool
	for k,v in pairs(config) do
		if v.plan_type == 6 and v.parameter == 1 then
			act_type = v.act_type
			break
		end
	end
	local begin_time = 0
	local end_time = 0
	local act_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
	if act_info ~= nil and act_info.status == ActStatusType.Open then
		begin_time = act_info.param_1
		end_time = act_info.param_2
	else
		local b_time,e_time = self:GetPlanDayOpenStampTime(act_type)
		begin_time = b_time or 0
		end_time = e_time or 0
	end	
	if end_time > 0 then
		end_time = end_time - 1
	end		
	return begin_time,end_time
end

function PlanActivityData:GetPlanActivityOpenDay()
	local begin_time,end_time = self:GetActBeginAndEndAndDayTime()
	local server_time = TimeHelper.GetServerTime()
	local day_time = server_time - begin_time
	if day_time < 0 then
		day_time = 0
	else
		day_time = TimeManager.FormatDHMS(day_time).day + 1
	end
	return day_time
end

-- 山海回想
function PlanActivityData:GetActMSBeginAndEndAndDayTime()
	local act_type = 0
	local config = Config.recall_shanhai_summary_auto.recall_shanhai
	for k,v in pairs(config) do
		if v.plan_type == 6 and v.parameter == 1 then
			act_type = v.act_type
			break
		end
	end	
	local begin_time = 0
	local end_time = 0
	local act_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
	if act_info ~= nil and act_info.status == ActStatusType.Open then
		begin_time = act_info.param_1
		end_time = act_info.param_2
	else
		local b_time,e_time = self:GetPlanDayOpenStampTime(act_type)
		begin_time = b_time or 0
		end_time = e_time or 0
		if end_time > 0 then
			end_time = end_time - 1
		end	
	end		
	return begin_time,end_time
end

--排期活动开启check
function PlanActivityData:CheckPlanGroupOpen()
	for k,v in pairs(self.plan_config) do
		if ActivityRandData.Instance:IsRandOpen(v) then
			local config = Config.wanlingyinxiang_auto.rand_pool
			for k1,v1 in pairs(config) do
				if v1.plan_type == 6 and v1.act_type == v.act_type then
					return true
				end
			end
		end
	end
	return false
end

-- 检查山海回想能否打开
function PlanActivityData:CheckMountainSeaGroupOpen()
	for k,v in pairs(self.mountain_sea_config) do
		if ActivityRandData.Instance:IsRandOpen(v) then
			return true
		end
	end
	return false
end

-- 检查是否存在非同系列活动
function PlanActivityData:CacheSpecialMod()
	self.mod_plan_special = {}
	for k,v in pairs(self.plan_config) do 
		local is_have = false
		for key,num in pairs(Mod.PlanActivity) do 
			if num == v.mod_key then 
				is_have = true
				break
			end 
		end 
		if not is_have then 
			table.insert(self.mod_plan_special,v.mod_key)
		end 
	end 

	self.mod_sea_special = {}
	for k,v in pairs(self.mountain_sea_config) do 
		for i,j in pairs(Config.activity_main_auto.recall_summary) do 
			if v.act_type == j.act_type then 
				local is_have = false
				for key,num in pairs(Mod.MountainSea) do 
					if num == j.mod_key then 
						is_have = true
						break
					end 
				end 
				if not is_have then 
					table.insert(self.mod_sea_special,j.mod_key)
				end 
			end 
		end 
	end 
end

function PlanActivityData:CheckSpecialModNum(data)
	if data.mod == nil or (data.mod_key ~= Mod.MountainSea.Main and Mod.PlanActivity.Main ~= data.mod_key) then 
		return 0
	end 

	if data.mod_key == Mod.MountainSea.Main then 
		local num = 0 
		for k,v in pairs(self.mod_sea_special) do
			num = num + Remind.Instance:GetRemindNum(v)
		end 
		return num
	end 

	if Mod.PlanActivity.Main == data.mod_key then 
		local num = 0 
		for k,v in pairs(self.mod_plan_special) do
			num = num + Remind.Instance:GetRemindNum(v)
		end 
		return num
	end 

	return 0
end

--获取排期随机活动列表
function PlanActivityData:GetRandPlanList()
	return ActivityRandData.Instance:GetRandList(self.plan_config)
end

--获取山海回想随机活动列表
function PlanActivityData:GetRandMountainSeaList()
	return self:GetRandListWithOutMark(self.mountain_sea_config)
end

--获取开启列表（不标记
function PlanActivityData:GetRandListWithOutMark(config)
    local list = {}
	for k,v in pairs(config) do
		if ActivityRandData.Instance:IsRandOpen(v) then
			v.mod = TabMod(v.mod_key)
			table.insert(list,v)
		end
	end
	return list
end