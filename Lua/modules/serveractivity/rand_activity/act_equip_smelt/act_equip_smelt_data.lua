ActEquipSmeltData = ActEquipSmeltData or BaseClass()
function ActEquipSmeltData:__init()
    if ActEquipSmeltData.Instance ~= nil then
        Debuger.LogError("[ActEquipSmeltData] attempt to create singleton twice!")
        return
    end
    ActEquipSmeltData.Instance = self

    self.act_info = SmartData.New({})
end

function ActEquipSmeltData:__delete()
    ActEquipSmeltData.Instance = nil
end

function ActEquipSmeltData:Config()
    return Config.ronglianyouli_auto
end

function ActEquipSmeltData:GetTaskList()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_EQUIP_SMELT, self:Config().gift_configure, "seq")
end
function ActEquipSmeltData:GetStageList()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_EQUIP_SMELT, self:Config().show_reward, "seq")
end

function ActEquipSmeltData:SetActInfo(protocol)
    self.act_info.has_task_stage_reward = protocol.has_task_stage_reward
    self.act_info.total_task_finish = protocol.total_task_finish
    self.act_info.task_score = protocol.task_score
    self.act_info.reward_receive_num = protocol.reward_receive_num
end

--任务完成次数，任务进度
function ActEquipSmeltData:GetTaskProg(seq)
    if self.act_info.task_score then
        return self.act_info.task_score[seq] or 0
    end
    return 0
end
function ActEquipSmeltData:GetTaskFetchedTimes(seq)
    if self.act_info.reward_receive_num then
        return self.act_info.reward_receive_num[seq] or 0
    end
    return 0
end

function ActEquipSmeltData:GetRemindNum()
    if ActivityData.Instance:IsHasSign(ACTIVITY_TYPE.RAND_EQUIP_SMELT) then
        return 1
    end
    if self:GetStageRemindNum() == 1 then
        return 1
    end
    for i, v in pairs(self:GetTaskList()) do
        if self:GetTaskFetchedTimes(v.seq) < v.reward_times and self:GetTaskProg(v.seq) >= v.parameter then
            return 1 
        end
    end
    return 0
end

function ActEquipSmeltData:GetStageRemindNum()
    for i, v in ipairs(self:GetStageList()) do
        if not self:IsStageRewardFetched(v.seq) then
            return self:GetFinishTaskNum() >= v.task_num and 1 or 0
        end
    end
    return 0
end

--阶段奖励领取标记
function ActEquipSmeltData:IsStageRewardFetched(seq)
    if self.act_info.has_task_stage_reward then
        return self.act_info.has_task_stage_reward[seq] == 1
    end
    return false
end
function ActEquipSmeltData:GetFinishTaskNum()
    return self.act_info.total_task_finish or 0
end

--返回当前阶段展示奖励表, 1->信息, 2->是否结束
function ActEquipSmeltData:GetShowInfo()
    local all_list = self:GetStageList()
    for i, v in ipairs(self:GetStageList()) do
        if not self:IsStageRewardFetched(v.seq) then
            return v, false
        end
    end
    return all_list[#all_list], true
end
