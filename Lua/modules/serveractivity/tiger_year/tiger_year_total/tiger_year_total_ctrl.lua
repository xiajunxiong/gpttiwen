VIEW_REQ("modules/serveractivity/tiger_year/tiger_year_total/tiger_year_total_view")

TigerYearTotalCtrl = TigerYearTotalCtrl or BaseClass(BaseCtrl)
function TigerYearTotalCtrl:__init()
    if TigerYearTotalCtrl.Instance ~= nil then
		Debuger.LogError("[TigerYearTotalCtrl] attempt to create singleton twice!")
		return
	end	
	TigerYearTotalCtrl.Instance = self
end

function TigerYearTotalCtrl:OnInit()
	FunOpen.Instance:SelfCondition(Mod.TigerYear.Main, function ()
		local level = RoleData.Instance:GetRoleLevel()
		local data = FunOpen.Instance:GetOpenData(Mod.TigerYear.Main)

		return level >= data.level and not self:GetTigerActIsOpen()
	end)
end 

-- 获得虎年汇总常驻活动
function TigerYearTotalCtrl:GetTotalKeyActType()
	local config = Config.huyingxinchun_auto.basic_configuration
	for k,v in pairs(config) do
		-- 选定localition为10的活动时间为准
		if v.location == 10 then
			return v.act_type
		end
	end	
end

-- 判断虎年汇总开始与否
function TigerYearTotalCtrl:GetActBeginAndEndAndDayTime()
	local act_type = self:GetTotalKeyActType()
	local begin_time = 0
	local end_time = 0
	local act_info = ActivityData.Instance:GetActivityStatusInfo(act_type)
	if act_info ~= nil and act_info.status == ActStatusType.Open then
		begin_time = act_info.param_1
		end_time = act_info.param_2
	else
		local b_time,e_time = PlanActivityData.Instance:GetPlanDayOpenStampTime(act_type)
		begin_time = b_time or 0
		end_time = e_time or 0
		if end_time > 0 then
			end_time = end_time - 1
		end	
	end		
	return begin_time,end_time
end

function TigerYearTotalCtrl:GetTigerActIsOpen()
	local begin_time,end_time = self:GetActBeginAndEndAndDayTime()

	return TimeHelper.GetServerTime() > end_time or TimeHelper.GetServerTime() < begin_time
end

function TigerYearTotalCtrl:CacheCurActList()
	local act_type = self:GetTotalKeyActType()
	local cfg = Config.huyingxinchun_auto.basic_configuration

	local sort_list = {}
	-- 位置分类
	for k,v in pairs(cfg) do 
		if sort_list[v.location] == nil then sort_list[v.location] = {} end 
		table.insert(sort_list[v.location],v)
	end 

	-- 整理出活动的次序
	for k,v in pairs(sort_list) do 
		if #v > 1 then 
			table.sort(v, DataSorter.KeyLowerSorter("end_time"))
		end 
	end 

	local result = {}
	for k,v in pairs(sort_list) do 
		if #v == 1 then 
			result[v[1].location] = TableCopy(v[1])
		elseif #v > 1 then 
			result[v[#v].location] = TableCopy(v[#v])
			for i,j in pairs(v) do 
				if ActivityData.IsOpen(j.act_type) then 
					result[j.location] = j
					break
				end 
			end 
		end 
	end 

	return result
end

function TigerYearTotalCtrl:GetTigerYearTip()
	for k, v in pairs(ActivityRandData.Instance.activity_rand_list or {}) do
		local config = Config.huyingxinchun_auto.basic_configuration
		for k1,v1 in pairs(config) do
			if v1.act_type == v.act_type then
				if ActivityData.Instance:IsHasSign(v.act_type) then
					return true
				end
			end
		end
	end
	return false
end

function TigerYearTotalCtrl:CheckTigerActNeedClose(mod_key)
	for k,v in pairs(Config.huyingxinchun_auto.basic_configuration) do
		if mod_key == v.mod_key then 
			return v.is_close == 1
		end 
	end 
end

function TigerYearTotalCtrl:AutoCloseTotal(mod_key)
	if self:CheckTigerActNeedClose(mod_key) then 
		ViewMgr:CloseView(TigerYearTotalView)
	end 
end