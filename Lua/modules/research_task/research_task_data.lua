ResearchTaskData = ResearchTaskData or BaseClass()
function ResearchTaskData:__init()
    if ResearchTaskData.Instance ~= nil then
        Debuger.LogError("[ResearchTaskData] attempt to create singleton twice!")
        return
    end
    ResearchTaskData.Instance = self
    self.zhenzhong_cfg_auto = Config.zhenzhong_cfg_auto
    self.reward_find = self.zhenzhong_cfg_auto.reward_find
    self.reward_group = self.zhenzhong_cfg_auto.reward_group
    self.mission_reward = self.zhenzhong_cfg_auto.mission_reward
    self.other_info = self.zhenzhong_cfg_auto.other[1]
    self.monster_info = nil
    self.task_info = SmartData.New()
    self.reward_list = {}
     --一个给找回显示用的列表
    self.cache_task_id = 0
    self.is_accept_task = false
end

function ResearchTaskData:__delete()
    self.cache_task_id = 0
end

function ResearchTaskData:SetMonsterInfo(info)
    self.monster_info = info
end

function ResearchTaskData:MonsterInfo()
    return self.monster_info
end

function ResearchTaskData:initScene(scene_id)
    if self.monster_info and self.monster_info.id > 0 and self.monster_info.scene_id == scene_id then
        VoMgr:AddMonster(self.monster_info)
    end
end

function ResearchTaskData:disposeScene()
    --[[ if self.monster_info then
        VoMgr:DelMonster(self.monster_info.obj_id)
    end ]]
    VoMgr:DelMonster(CUSTOM_OBJID.RESEARCH_TASK)
end

function ResearchTaskData:TaskInfo()
    return self.task_info
end

function ResearchTaskData:GetActivityBackRewardList()
    return RewardData.Instance:GetActivityBackRewardList(self.reward_find, self.reward_group)
end

function ResearchTaskData:TaskVo(vo)
    local taskvo = CommonStruct.CustomTask()
    local name = Cfg.MonsterById(vo.id).name
    local scene = SceneData.Instance:GetSceneCfg(vo.scene_id)
    taskvo.task_id = CUSTOM_OBJID.RESEARCH_TASK
    taskvo.task_name = string.format(Language.Task.TaskName.ZhenZhong, name, self.today_finish_times + 1,self.other_info.daily_times)
    taskvo.task_desc = string.format(Language.Task.TaskGoal.ZhenZhong, scene.name, name)
    taskvo.task_goal = string.format(Language.Task.TaskGoal.ZhenZhong, scene.name, name)
    taskvo.task_goal_menu = string.format(Language.Task.TaskGoal.ZhenZhongMenu, scene.name, name)
    return taskvo
end

--设置真中的研究今天完成的次数
function ResearchTaskData:SetTdayFinishTimes(times)
    self.today_finish_times = times
end
--获取真中的研究今天完成的次数
function ResearchTaskData:GetTdayFinishTimes()
    return self.today_finish_times or 0
end
function ResearchTaskData:MiniNum()
    return self.other_info.min_team_num
end
function ResearchTaskData:Cost()
    return self.other_info.fast_complete_cost
end
function ResearchTaskData:Time()
    return self.other_info.fast_complete_num - self:GetTdayFinishTimes()
end
function ResearchTaskData:Reward()
    local max_time = self:Time()--self.other_info.daily_times
    local level = RoleData.Instance:GetRoleLevel()
    local list = {}
    for index, value in ipairs(self.reward_find) do
        if level >= value.min_level and level <= value.max_level then
            if value.exp > 0 then
                table.insert(list, Item.Create({item_id = CommonItemId.Exp, num = value.exp * max_time}))
            end
            if value.coin > 0 then
                table.insert(list, Item.Create({item_id = CommonItemId.Coin, num = value.coin * max_time}))
            end
            break
        end
    end
    return list
end
function ResearchTaskData:Reward2()
    local max_time = self.other_info.daily_times
    local level = RoleData.Instance:GetRoleLevel()
    local list = {}
    for index, value in ipairs(self.mission_reward) do
        if level >= value.min_level and level <= value.max_level then
            if value.exp > 0 then
                table.insert(list, Item.Create({item_id = CommonItemId.Exp, num = value.exp * max_time}))
            end
            if value.coin > 0 then
                table.insert(list, Item.Create({item_id = CommonItemId.Coin, num = value.coin * max_time}))
            end
            break
        end
    end
    return list
end
function ResearchTaskData:CostType()
    local level = RoleData.Instance:GetRoleLevel()
    for index, value in ipairs(self.reward_find) do
        if level >= value.min_level and level <= value.max_level then
            return value.find_type
        end
    end
end

function ResearchTaskData:RewardNum()
    local max_time = self.other_info.daily_times
    local level = RoleData.Instance:GetRoleLevel()
    local res
    for index, value in ipairs(self.mission_reward) do
        if level >= value.min_level and level <= value.max_level then
            res = value
            break
        end
    end
    return {res.exp * max_time, res.coin * max_time}
end