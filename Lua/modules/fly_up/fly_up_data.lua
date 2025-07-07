FlyUpStageType = {None = 0,Low = 1,High = 2}
FLYUP_MAX_TASK_NUM = 10

FlyUpData = FlyUpData or BaseClass()

function FlyUpData:__init()
    if FlyUpData.Instance ~= nil then
        Debuger.LogError("[FlyUpData] attempt to create singleton twice!")
        return
    end
    FlyUpData.Instance = self
    self.Config = Config.role_feisheng_auto
    self.info_data = SmartData.New({
        fly_flag = 0,
        element = {},
        organ = {},
        is_open = 0,
        task_index = 0,
        skill_exp = 0,
        skill_level = 0,
    })
    self.add_data = SmartData.New({info = {},val = false})
    self.red_piont_data = SmartData.New({val = false})
end

function FlyUpData:ClearData()
    self.info_data:Set({
        fly_flag = 0,
        element = {},
        organ = {},
        is_open = 0,
        task_index = 0,
        skill_exp = 0,
        skill_level = 0,
    })
end

--飞升信息全部下发
function FlyUpData:SetFlyUpSystemInfo(protocol)
    self.info_data.stage = protocol.stage
    self.info_data.organ = protocol.organ
    self.info_data.fly_flag = protocol.stage
    self.info_data.element = protocol.element
    self.info_data.is_open = protocol.is_open
    self.info_data.task_index = protocol.task_index
    self.info_data.skill_level = protocol.skill_level
end

--飞升技能信息下发
function FlyUpData:SetFlyUpSkillSigleInfo(protocol)
    self.info_data.skill_exp = protocol.skill_exp
    self.info_data.skill_level = protocol.skill_level
    self.red_piont_data = SmartData.New({val = false})
end

--飞升进阶信息下发
function FlyUpData:SetFlyUpSystemAdvInfo(info)
    if info.stage == 0 then
        self.info_data.element[info.index] = self.info_data.element[info.index] + info.value
    else
        self.info_data.organ[info.index] = self.info_data.organ[info.index] + info.value
    end
    self.add_data.info = info
    self.add_data.val = not self.add_data.val
end

--是否填满元素
function FlyUpData:IsElementFinish(stage)
    for k,num in pairs(self:GetElementList(stage)) do
        if self:GetMaxElementNum(stage) > num then
            return false
        end
    end
    return true
end

--获取单个元素属性值
function FlyUpData:GetAttrItemData(stage,sub_type,num)
    for k,v in pairs(self.Config.shilian_attributes) do
        if v.seq == stage and RoleData.Instance:GetRoleProf() == v.type_jobs and v.sub_type == sub_type + 1 then
            return {attr_type = v.attr_type,attr_value = v.attr_val * num,attr_name = Language.Common.AttrList[v.attr_type]}
        end
    end
end

--飞升增加的属性
function FlyUpData:GetAttrAddData(stage)
    for k,v in pairs(self.Config.feisheng_attributes) do
        if v.shilian_type == stage then
            return DataHelper.ResetAttrList(v)
        end
    end
end

--飞升成功属性
function FlyUpData:GetAttrSuccData()
    local attr_list = {}
    local base_list = RoleData.Instance:AllBattleAttri()
    local add_list = self:GetAttrAddData(self:GetStage())
    for k,value in pairs(add_list) do
        if value ~= 0 then
            local attr_value = base_list[k] or 0
            table.insert(attr_list,{
                attr_type = k,
                attr_name = Language.Common.AttrList[k],
                old_value = attr_value - value,
                new_value = attr_value
            })
        end
    end
    table.sort(attr_list,DataHelper.SortFunc([[attr_type]]))
    return attr_list
end

--获取元素总数
function FlyUpData:GetTaskNum(stage)
    local num = 0
    local info = self:GetElementList(stage)
    for k,v in pairs(info) do
        num = num + v
    end
    if self:GetMaxElementNum(stage) then
        return num,table.nums(info) * self:GetMaxElementNum(stage)
    else
        return num,table.nums(info) * self:GetOtherConfig().max_exp2
    end
end

--获取飞升解锁技能id
function FlyUpData:GetSkillId()
    local prof = RoleData.Instance:GetRoleProf()
    for k,v in pairs(self.Config.skill_lock) do
        if prof == v.type_jobs then
            return v.ascension_skills
        end
    end
end

--根据技能等级获取当前技能升级配置
function FlyUpData:GetSkillUpConfig(skill_level)
    for k,v in pairs(self.Config.skill_up) do
        if v.skill_level == skill_level then
            return v
        end
    end
end

--获取飞升技能列表
function FlyUpData:GetSkillList()
    local list = {}
    local skill_id = self:GetSkillId()
    local config = self.Config.skill_up
    for i,v in ipairs(self.Config.skill_up) do
        table.insert(list,{
            skill_id = skill_id,
            skill_level = v.skill_level,
            maxLevel = config[#config].skill_level,
            limit_level = v.role_level,
            curRoleSkillData = {
                skill_id = skill_id,
                skill_level = self:GetSkillLevel(),
            }
        })
    end
    return list
end

--获取任务结算奖励
function FlyUpData:GetTaskRewardData()
    for k,v in pairs(self.Config.last_reward) do
        if v.feisheng_stage == self:GetStage() and v.is_shilian == self:GetIsOpenNum() then
            return DataHelper.FormatItemList(v.show_reward_item)
        end
    end
    return {}
end

--是否显示飞升
function FlyUpData:IsShowed()
    if not self:IsCompletedTask() then
        return false
    end
    if self:GetStage() == FlyUpStageType.High then
        return false
    end
    if self:GetNextStage() == FlyUpStageType.High then
        if not TaskData.Instance:GetTaskIsCompleted(self:GetOtherConfig().feishengtasklist_id2) then
            return false
        end
    end 
    local limit_level = self:GetOtherConfig()["level_limit"..self:GetNextStage()]
    return limit_level and RoleData.Instance:GetRoleLevel() >= limit_level
end

--飞升角色红点
function FlyUpData:GetRoleFlyUpNum()
    if PrefsInt([[RoleFlyUpNum]]..RoleId()) == 1 then
        return 0
    end
    if not FunOpen.Instance:IsFunOpen(Mod.FlyUp.Main) then
        return 0
    end
    local next_config = self:GetSkillUpConfig(self:GetSkillLevel() + 1)
    if next_config ~= nil then
        if RoleLevel() >= next_config.role_level and Item.GetNum(next_config.item_id) >= next_config.consume then
            return 1
        end
    end
    return 0
end

--飞升条件任务
function FlyUpData:GetTaskList()
    local task_list = {}
    local stage = FlyUpData.Instance:GetNextStage()
    local config = FlyUpData.Instance:GetOtherConfig()
    local task_num,target_num = self:GetTaskNum(stage)
    local nofinish = RoleData.Instance:GetRoleLevel() < (config["level_limit"..stage] or 0)
    local color = nofinish and COLORSTR.Yellow11 or COLORSTR.Green3
    table.insert(task_list,{
        integral = Format(Language.Role.PromoteCurrScoreTip,RoleData.Instance:GetRoleLevel()),
        desc = Format(Language.FlyUp.TaskList2[1], color, config["level_limit"..stage]),
        target = RoleData.Instance:GetRoleLevel() < (config["level_limit"..stage] or 0),
    })
    nofinish = RoleData.Instance:GetRoleScore() < (config["score_limit"..stage] or 0)
    color = nofinish and COLORSTR.Yellow11 or COLORSTR.Green3
    table.insert(task_list,{
        integral = Format(Language.Role.PromoteCurrScoreTip,RoleData.Instance:GetRoleScore()),
        desc = Format(Language.FlyUp.TaskList2[2], color, config["score_limit"..stage]),
        target = RoleData.Instance:GetRoleScore() < (config["score_limit"..stage] or 0),
    })
    nofinish = task_num < target_num
    color = nofinish and COLORSTR.Yellow11 or COLORSTR.Green3
    table.insert(task_list,{
        integral = "",
        desc = Format(Language.FlyUp.TaskList2[stage == FlyUpStageType.Low and 3 or 4], color, task_num,target_num),
        target = task_num < target_num,event = function()
            ViewMgr:OpenViewByKey(Mod.FlyUp.Main)
            ViewMgr:CloseView(FlyUpPopupView)
        end
    })
    return task_list
end

function FlyUpData:GetRedPiontNum()
    if not FunOpen.Instance:IsFunOpen(Mod.FlyUp.Main) then
        return 0
    end
    if self:GetStage() >= FlyUpStageType.High then
        return 0
    end
    local task_num,target_num = self:GetTaskNum(self:GetNextStage())
    if task_num ~= target_num and self:IsHasConsumeNum() then
        return 1
    end
    if task_num == target_num and self:GetTaskTip() == nil then
        return 1
    end
    return 0
end

function FlyUpData:IsHasConsumeNum()
    local stage,config = self:GetNextStage(),self:GetOtherConfig()
    local item_id,num = config["shilian_cost" .. stage],config["cost" .. stage]
    if item_id ~= nil and num ~= nil then
        if Item.GetNum(item_id) >= num then
            return true
        end
    end
    return false
end

function FlyUpData:GetTaskTip()
    for k,v in pairs(self:GetTaskList()) do
        if v.target then
            return Language.FlyUp.PromoteTaskTip[k]
        end
    end
end

function FlyUpData:GetCurrTaskNum()
    return self.info_data.task_index,10
end

--获取元素最大值
function FlyUpData:GetMaxElementNum(stage)
    return self:GetOtherConfig()["max_exp"..stage]
end

--获取当前技能经验值
function FlyUpData:GetSkillExp()
    return self.info_data.skill_exp or 0
end

--获取当前试炼状态 是否试炼中
function FlyUpData:GetIsOpenNum()
    return self.info_data.is_open or 0
end

--获取转到的index
function FlyUpData:GetIndex()
    return (self.add_data.info.index or 0) + 1
end

--获取元素信息下发值
function FlyUpData:GetElementNum(index,stage)
    local info = self:GetElementList(stage)
    return info and (info[index] or 0) or 0
end

--获取元素信息列表
function FlyUpData:GetElementList(stage)
    return stage == FlyUpStageType.High and self.info_data.organ or self.info_data.element
end

--获取其他配置
function FlyUpData:GetOtherConfig()
    return self.Config.other[1]
end

--获取飞升当前阶段 0无飞升 1完成初阶飞升 2完成高阶飞升
function FlyUpData:GetStage()
    return self.info_data.stage or FlyUpStageType.None
end

--获取飞升下一阶段（是否不能超过最大值）
function FlyUpData:GetNextStage(not_over_max)
    if not_over_max then
        return (self:GetStage() + 1) > FlyUpStageType.High and FlyUpStageType.High or (self:GetStage() + 1)
    else
        return self:GetStage() + 1
    end
end

--获取当前飞升技能等级
function FlyUpData:GetSkillLevel()
    return self.info_data.skill_level or 0
end

--是否开启飞升前置任务
function FlyUpData:IsCompletedTask()
    return TaskData.Instance:GetTaskIsCompleted(self:GetOtherConfig().feishengtasklist_id)
end

--是否完成飞升
function FlyUpData:IsFinish()
    return self:GetStage() >= FlyUpStageType.High
end

--获取飞升任务接取NPC
function FlyUpData:GetNpcId()
    return self:GetOtherConfig().sunring_npc
end

function FlyUpData:CareInfoDataFlyFlagChange(func)
    return self.info_data:Care(func, "fly_flag")
end

function FlyUpData:UnCareInfoDataChange(ht)
    self.info_data:Uncare(ht)
end


-- 是否处于飞升卡等级中 飞升0级最高90级，1级最高110级，飞升2级无限制
function FlyUpData:IsLevelLimit()
    local fly_level = self:GetStage()
    if fly_level == FlyUpStageType.High then
        return false
    end
    local role_level = RoleData.Instance:GetRoleLevel()
    if fly_level == FlyUpStageType.None and role_level >= 90 then
        return true
    end
    if fly_level == FlyUpStageType.Low and role_level >= 110 then
        return true
    end
    return false
end