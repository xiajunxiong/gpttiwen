--加点数据管理
RoleAddPointData = RoleAddPointData or BaseClass()

function RoleAddPointData:__init()
	if RoleAddPointData.Instance ~= nil then
		Debuger.LogError("[RoleAddPointData] attempt to create singleton twice!")
		return
	end
    RoleAddPointData.Instance = self
    
    self.config = Config.roleexp_auto

    self.attributes_exchange = Config.roleexp_auto.attributes_exchange 		-- 资质点换算

    self.point_data = SmartData.New{
        plan_list = {},
        cur_plan_index = 0,
        plan_info = {},
        plan_num = 0,
    }
    self.red_add_piont = SmartData.New({val = false})
    self.cur_auto_add = {}
    self.role_point_data = SmartData.New{add_list = {},add_change = false, auto_add = false, changed = false}
end

function RoleAddPointData:__delete()
	RoleAddPointData.Instance = nil
end

function RoleAddPointData:SetOtherInfo()
    self:ResetCurAutoAddPoint()
    self.auto_add = false
    for k,v in pairs(self.cur_auto_add) do
        if v ~= 0 then
            self.auto_add = true
            break
        end
    end
    if self.auto_add then
        if self.role_point_data.auto_add then
            self.role_point_data:SetDirty("auto_add")
        else
            self.role_point_data.auto_add = true
        end
    elseif self.role_point_data.auto_add then
        self.role_point_data.auto_add = false
    else
        self.role_point_data:SetDirty("auto_add")
    end
end

function RoleAddPointData:ResetCurAutoAddPoint()
    if self.tui_jian_auto_add == 1 and self:GetRoleAllPiontIndex() == 0 then
        self:SetSuggestAutoAddNum()
    else
        self.cur_auto_add = TableCopy(self.point_data.plan_info.custom_add_point_list)
    end
end

function RoleAddPointData:SetSuggestAutoAddNum()
    local suggest_add = self:GetSimpleRecomPointList()
    for i = 0, ROLE_ATTRIBUTE_PLAN_MAX_NUM - 1 do
        self.cur_auto_add[i] = suggest_add[Language.Role.AddPointTypeKey[i]]
    end
end

function RoleAddPointData:GetAutoAddNum(index)
    if index then
        return self.cur_auto_add[index] or 0
    else
        return self.cur_auto_add
    end
end

--是否可以加点，根据结果不同，返回值不同
function RoleAddPointData:CanAddAutoNum(index, add_num)
    local total = 0
    for k,v in pairs(self.cur_auto_add) do
        total = total + v
    end
    if total + add_num > 4 then
        return false, Language.Role.AddPointLevelMax
    -- elseif self.cur_auto_add[index] + add_num > 2 then
    --     return false, Language.Role.AddPointNotMoreThenHalp
    end
    self.cur_auto_add[index] = self.cur_auto_add[index] + add_num
    return true, self.cur_auto_add[index]
end

function RoleAddPointData:SetRolePlanInfo(protocol)
    local reset = false
    if self.point_data and self.point_data.cur_plan_index == protocol.cur_plan_index then
        if self.reset_plan_times and self.reset_plan_times < protocol.reset_plan_times then
            reset = true
        end
    end
    self.point_data.plan_list = protocol.plan_list
    self.point_data.plan_info = protocol.plan_info
    self.point_data.plan_num = protocol.plan_num
    self.point_data.cur_plan_index = protocol.cur_plan_index
    self.reset_plan_times = protocol.reset_plan_times or 0
    self.red_add_piont.val = protocol.plan_info.add_point_num ~= 0
    self.tui_jian_auto_add = protocol.tui_jian_auto_add
    self:SetOtherInfo()
    BianQiangData.Instance:FlushListRemind(BianQiangData.ListType.JueSeQianNengDian)
    if reset then
        RoleCtrl.Instance:SendSetAutoAddPoint(SET_AUTO_ADD_POINT_FLAG.SET_AUTO_ADD_POINT_FLAG_ROLE,{index = self.point_data.cur_plan_index})
    end
end

function RoleAddPointData:GetRedAddPiontNum()
    return self.red_add_piont.val and 1 or 0
end

function RoleAddPointData:GetRoleAddPlanInfo()
    return self.point_data.plan_info or {}
end

function RoleAddPointData:GetRoleAddPlanList()
    return self.point_data.plan_list or {}
end

function RoleAddPointData:GetRoleAllPiontIndex()
    return self.point_data.cur_plan_index
end

function RoleAddPointData:GetRoleAddPlanNum()
    return self.point_data.plan_num
end

function RoleAddPointData:SetRolePointAddNum(type,num)
    self.role_point_data.add_list[type] = num
    self.role_point_data.add_change = not self.role_point_data.add_change
end

function RoleAddPointData:GetRolePointAddNum()
    local num = 0
    for k,v in pairs(self.role_point_data.add_list) do
        num = num + v
    end
    return num
end

function RoleAddPointData:GetRolePointAddList()
    return self.role_point_data.add_list
end

function RoleAddPointData:SetRolePointAddList(add_list)
    self.role_point_data.add_list = add_list or {}
    if add_list ~= nil then
        self.role_point_data.add_change = not self.role_point_data.add_change
    end
end

--获取方案列表
function RoleAddPointData:GetPlanData(cur_plan_index)
    return self.point_data.plan_list and self.point_data.plan_list[cur_plan_index or self.point_data.cur_plan_index] or {}
end

function RoleAddPointData:GetPointList(cur_plan_index)
    return self:GetPlanData(cur_plan_index).plan_list or {}
end

--获取加点初始分配配置 30
function RoleAddPointData:GetBasePointData(job)
    for k,v in pairs(self.config.base_point) do
        if v.job == job then
            return v
        end   
    end
end

--获取角色加点推荐配置
function RoleAddPointData:GetBestAddPointData(job)
    for k,v in pairs(self.config.best_add_point) do
        if v.job == job then
            return v
        end   
    end
end

--是否已经洗过点
function RoleAddPointData:IsWashPoint()
    return self.reset_plan_times > 0
end

function RoleAddPointData:GetResetPlanTimes()
    return self.reset_plan_times or 0
end

function RoleAddPointData:IsNewPlan(list)
    local num = 0
    for k,v in pairs(list) do
        num = num + v
    end
    return num == 0
end

--计算推荐30点
function RoleAddPointData:GetBestWashPoint(data)
    return self:GetBasePointData(data.job)
end

function RoleAddPointData:SetAllPiontNum(num)
    self.all_piont_num = num / 2
end

function RoleAddPointData:GetAllPiontNum()
    return self.all_piont_num or 0
end

--计算累积数值
function RoleAddPointData:GetMultData(data,num)
    local list = {}
    for k,v in pairs(data or {}) do
        list[k] = v * num
    end
    return list
end

--计算加点属性
function RoleAddPointData:GetPointAttriData(point_list)
    local attr_data = {}
    for k,v in pairs(point_list or {}) do
        if tonumber(k) and v ~= 0 then
            local add_data = self:GetMultData(self.attributes_exchange[k+1],v)
            attr_data = DataHelper.AttrListAdd(attr_data,add_data,true)
        end
    end
    return attr_data
end

--计算差值
function RoleAddPointData:GetPointAttriSubData(plan_info)
    local sub_data = {}
    local piont_data = self:GetPointAttriData(plan_info)
    for k,v in pairs(AttrTypeForName) do
        local piont_num = piont_data[k] or 0
        sub_data[k] = piont_num - math.modf(piont_num / 100) * 100
    end
    return sub_data
end

--计算全部属性差值
function RoleAddPointData:GetPointAllAttriData(add_list,plan_info)
    local add_data = self:GetPointAttriData(add_list)
    local sub_data = self:GetPointAttriSubData(plan_info)
    local attri_data = DataHelper.AttrListAdd(add_data, sub_data, true)
    for k, v in pairs(attri_data) do
        attri_data[k] = math.modf(attri_data[k] / 100)
    end
    return DataHelper.ResetAttrList(attri_data)
end

function RoleAddPointData:GetSimpleRecomPointList()
    local prof = RoleData.Instance:GetRoleProf()
    local point_config = self:GetBestAddPointData(prof)
    return point_config
end

--推荐加点
function RoleAddPointData:GetRecomPointList(plan_info,level)
    local list = {}
    local base_list ={}
    level = level or RoleData.Instance:GetRoleLevel()
    local prof = RoleData.Instance:GetRoleProf()
    local base_config = self:GetBasePointData(prof)
    local point_config = self:GetBestAddPointData(prof)
    for i=0,ROLE_ATTRIBUTE_PLAN_MAX_NUM - 1 do
        local key = Language.Role.AddPointTypeKey[i]
        base_list[i] = base_config[key] + point_config[key] * (level - 1)
    end
    local point_num = plan_info.add_point_num or 0
    local point_list = plan_info.add_point_list or {}

    for i=0,ROLE_ATTRIBUTE_PLAN_MAX_NUM - 1 do
        if point_list[i] and base_list[i] >= point_list[i].value then
            local num = base_list[i] - point_list[i].value
            if num > point_num then
                list[i] = point_num
				break
            else
                point_num = point_num - num
                list[i] = num
            end
        else
            list[i] = 0
        end
    end
    return list
end