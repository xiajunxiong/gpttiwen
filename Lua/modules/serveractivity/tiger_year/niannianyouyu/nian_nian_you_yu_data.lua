NianNianYouYuData = NianNianYouYuData or BaseClass()
function NianNianYouYuData:__init()
    if NianNianYouYuData.Instance ~= nil then
        Debuger.LogError("[NianNianYouYuData] attempt to create singleton twice!")
        return
    end
    NianNianYouYuData.Instance = self

    self.act_info = SmartData.New({is_accpet_task = 0, role_level = 0, fish_num = 0})
end

function NianNianYouYuData:__delete()
    NianNianYouYuData.Instance = nil
end

function NianNianYouYuData:Config()
    return Config.niannianyouyu_auto
end
function NianNianYouYuData:OtherConfig()
    return self:Config().other[1]
end

function NianNianYouYuData:FishItemId()
    return self:OtherConfig().fish_icon
end

--活动开启时玩家等级
function NianNianYouYuData:CurRoleLevel()
    if self:IsAcceptTask() then
        return self.act_info.role_level
    end
    return RoleData.Instance:GetRoleLevel()
end

--是否已接取任务
function NianNianYouYuData:IsAcceptTask()
    return self.act_info.is_accpet_task == 1
end

--当前鱼的数量
function NianNianYouYuData:CurFishNum()
    return self.act_info.fish_num
end

function NianNianYouYuData:GetMatchList(list, level)
    level = level or self:CurRoleLevel()
    local show_level = -1
    for i, v in pairs(list) do
        if v.level <= level and v.level > show_level then
            show_level = v.level
        end
    end
    local show_list = {}
    for i, v in pairs(list) do
        if v.level == show_level then
            table.insert(show_list, v)
        end
    end
    return show_list
end

-----NPC配置
function NianNianYouYuData:GetNpcList()
    return self:Config().task_configure
end

--获取当前能去的npc位置
function NianNianYouYuData:GetAcceptNpc()
    local show_list = self:GetMatchList(self:GetNpcList(), RoleData.Instance:GetRoleLevel())
    return show_list[1]
end

-----任务列表
function NianNianYouYuData:GetTaskList()
    local base_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU, self:Config().basis_configure)
    local show_list = self:GetMatchList(base_list)
    table.sort(show_list, DataHelper.WeightSort("seq", function(data)
        if self:IsTaskRewardFetched(data.task_type) then
            return 2000
        elseif self:IsTaskFinished(data.task_type) then
            return 0
        else
            return 1000
        end
    end))
    return show_list
end

--获取任务进度
function NianNianYouYuData:GetTaskProg(task_type)
    if self.act_info.task_extent then
        return self.act_info.task_extent[task_type] or 0
    end
    return 0
end

--任务是否完成
function NianNianYouYuData:IsTaskFinished(task_type)
    if self.act_info.task_status then
        return self.act_info.task_status[task_type] == 1
    end
    return false
end

--任务奖励是否已领取
function NianNianYouYuData:IsTaskRewardFetched(task_type)
    if self.act_info.task_reward_status then
        return self.act_info.task_reward_status[task_type] == 1
    end
    return false
end

-----奖励列表
function NianNianYouYuData:GetRewardList()
    local base_list = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_NIAN_NIAN_YOU_YU, self:Config().gift_configure)
    local show_list = self:GetMatchList(base_list)
    table.sort(show_list, DataHelper.WeightSort("seq", function(data)
        return self:IsRewardFetched(data.seq) and 1000 or 0
    end))
    return show_list
end

--奖励是否已领取
function NianNianYouYuData:IsRewardFetched(seq)
    if self.act_info.reward_stage_status then
        return self.act_info.reward_stage_status[seq] == 1
    end
    return false
end
--奖励是否可领取
function NianNianYouYuData:IsRewardCanFetch(seq)
    if self.act_info.is_receive_reward then
        return self.act_info.is_receive_reward[seq] == 1
    end
    return false
end

-----红点
--1)活动初始有新字提示，点击查看后消失
--2)有奖励可领取时需有红点提示，领取奖励后消失
function NianNianYouYuData:GetRemindNum()
    if self:IsAcceptTask() then
        for i, v in pairs(self:GetTaskList()) do
            if self:IsTaskFinished(v.task_type) and not self:IsTaskRewardFetched(v.task_type) then
                return 1
            end
        end
    end
    for i, v in pairs(self:GetRewardList()) do
        if self:IsRewardCanFetch(v.seq) and not self:IsRewardFetched(v.seq) then
            return 1
        end
    end
    return 0
end

-----协议
function NianNianYouYuData:SetActInfo(protocol)
    self.act_info.is_accpet_task = protocol.is_accpet_task
    self.act_info.role_level = protocol.role_level
    self.act_info.fish_num = protocol.fish_num

    self.act_info.task_status = protocol.task_status
    self.act_info.task_reward_status = protocol.task_reward_status
    self.act_info.task_extent = protocol.task_extent

    self.act_info.is_receive_reward = protocol.is_receive_reward
    self.act_info.reward_stage_status = protocol.reward_stage_status
end
