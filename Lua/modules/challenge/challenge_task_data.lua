ChallengeTaskData = ChallengeTaskData or BaseClass()

function ChallengeTaskData:__init()
    if ChallengeTaskData.Instance ~= nil then
        Debuger.LogError("[ChallengeTaskData] attempt to create singleton twice!")
        return
    end
    ChallengeTaskData.Instance = self

    self:ChallengeTaskConfig()
    self:ChallengeTaskData()
end

function ChallengeTaskData:ChallengeTaskConfig()
    --挑战任务 配置
    self.config = Config.challengetask_list_auto
    self.config_task_list = self.config.task_list
    self.config_challenge_task = self.config.challenge_task
    self.config_challenge_go_on = self.config.challenge_go_on
    --幻世挑战 配置
    self.config_huanshi = Config.huanshi_challenge_auto
    self.config_task_list_team = self.config_huanshi.task_list
    self.config_challenge_team_task = self.config_huanshi.team_challenge
    Cfg.Register("ChallengeGoOnByWordTask", Cfg.RealTimeSearch(self.config_challenge_go_on, "word_task"))
    Cfg.Register("ChallengeTaskByLastTask", Cfg.RealTimeSearch(self.config_challenge_task, "last_task"))
end

function ChallengeTaskData:ChallengeTaskData()
    self:ChallengeTaskList()
    self.cur_sel_task = SmartData.New() -- 当前选中的挑战任务
    self.cur_team_sel_task = SmartData.New()-- 当前选中的幻世任务
    self.cur_challenge_type = SmartData.New({type = 1}) -- 当前选中的挑战类型
    self.team_succ_reward = nil -- 组队挑战胜利奖励
end

function ChallengeTaskData:ChallengeTaskList()
    self.task_sub_list = {}
    for _, v in pairs(self.config_challenge_go_on) do
        self.task_sub_list[v.last_task] = self.task_sub_list[v.last_task] or {}
        table.insert(self.task_sub_list[v.last_task], v.word_task)
    end

    self.task_list = {}
    local ct = Language.Challenge.Task
    for _, v in pairs(self.config_challenge_task) do
        local data = v
        data.icon_id_m = Cfg.MonsterById(v.monster_id).icon_id
        data.pre_task = RichTextHelper.ColorStr(ct.PreTask, COLORSTR.Red5) .. (self.config_task_list[v.pretaskid] and self.config_task_list[v.pretaskid].task_name or ct.Wu)
        data.open_level = self.config_task_list[v.last_task] and self.config_task_list[v.last_task].min_level or 0
        data.open_level_desc = RichTextHelper.ColorStr(ct.OpenLevel, COLORSTR.Red5) .. string.format(ct.Ji, data.open_level)
        data.desc = v.description
        data.rewards = {}
        data.recommend_num = v.recommend_num
        for _, item in pairs(v.stage_reward) do
            table.insert(data.rewards, Item.New(item))
        end
        table.insert(self.task_list, data)
    end

    self.team_task_list = {}
    for _, v in pairs(self.config_challenge_team_task) do
        local data = v
        data.icon_id_m = Cfg.MonsterById(v.monster_id).icon_id
        data.pre_task = RichTextHelper.ColorStr(ct.PreTask, COLORSTR.Red5) .. (self.config_task_list_team[v.pretaskid] and self.config_task_list_team[v.pretaskid].task_name or ct.Wu)
        data.open_level = self.config_task_list_team[v.last_task] and self.config_task_list_team[v.last_task].min_level or 0
        data.open_level_desc = string.format(ct.Ji, data.open_level)
        data.desc = v.description
        data.rewards = {}
        data.recommend_num = v.recommend_num
        data.recommend_tip = string.format(ct.recommend_tip, data.recommend_num)
        for _, item in pairs(v.stage_reward) do
            table.insert(data.rewards, Item.New(item))
        end
        table.insert(self.team_task_list, data)
    end
end

function ChallengeTaskData:GetChallengeTaskList()
    return self.task_list
end

-- 组队挑战 界面自动选中的数据 
function ChallengeTaskData:GetSelTeamData(task_id)
    for i, v in pairs(self.team_task_list) do
        local canDo = not TaskData.Instance:GetTaskIsCompleted(v.last_task) and RoleData.Instance:GetRoleLevel() >= v.open_level and TaskData.Instance:GetTaskIsCompleted(v.pretaskid)
        if canDo and ((task_id ~= nil and v.last_task == task_id) or task_id == nil) then
            return i
        end
    end
    return 1
end
-- 挑战任务 界面自动选中的数据 
function ChallengeTaskData:GetSelChallengeData()
    return self.task_list[1]
end

function ChallengeTaskData:GoChallengeTask(last_task)
    TaskData.Instance:GoChallengeTask(self.task_sub_list[last_task], last_task)
end

function ChallengeTaskData:GoTeamTask(last_task)
    local task = TaskData.Instance.task_list[last_task]
    if task then
        AutoInvokerCtrl.Instance:SetCommands({AutoCommandTask.New(task)}, 1)
        else --去往场景
            local task_cfg=self.config_task_list_team[last_task]
            local npc_seq=tonumber (string.split (task_cfg.tasks_dialog_beh,"|")[5])
            SceneLogic.Instance:AutoToNpc(npc_seq, nil, nil,true)
    end
end

function ChallengeTaskData:IsLastChallengeTask(task_id)
    return nil == self.task_sub_list[task_id]
end

function ChallengeTaskData:HaveChallengeTask(last_task)
    local value = false
    local sub_tasks = self.task_sub_list[last_task] or {}
    for _, word_task in pairs(sub_tasks) do
        value = value or (nil ~= TaskData.Instance.task_list[word_task])
    end
    return value
end

function ChallengeTaskData:GetConditionTip()
    local num = 0
    local role_level = RoleData.Instance:GetRoleLevel()
    for k, v in pairs(self.task_list) do
        if role_level >= v.open_level and not TaskData.Instance:GetTaskIsCompleted(v.last_task) then
            num = num + 1
        end
    end
    return Format(Language.Challenge.Task.ConditionTip, num)
end

function ChallengeTaskData:GetPassTask()
    local num, total = 0, 0
    local role_level = RoleData.Instance:GetRoleLevel()
    for _, info in pairs(self.task_list) do
        if TaskData.Instance:GetTaskIsCompleted(info.last_task) then
            num = num + 1
        end
        if role_level >= info.open_level and TaskData.Instance:GetTaskIsCompleted(info.pretaskid) then
            total = total + 1
        end
    end
    return num, total
end

function ChallengeTaskData:GetWordTaskRecommendScore(word_task)
    local recommend_score = 0
    local co = Cfg.ChallengeGoOnByWordTask(word_task)
    if co then
        local last_task = co.last_task
        co = Cfg.ChallengeTaskByLastTask(last_task)
        if co then
            recommend_score = co.recommend_score
        end
    end
    return recommend_score
end

function ChallengeTaskData.IsChallengeFinish(battle_mode)
    if battle_mode == BATTLE_MODE.BATTLE_MODE_NORMAL_MONSTER then
        local group_id = FinishData.Instance:GetMonsterGroupId()
        local group_cfg = Config.monster_group_auto.monster_group[group_id]
        if group_cfg ~= nil and group_cfg.is_challage == 1 then
            return true
        end
        return false
    end
end

--幻世挑战是否开启
function ChallengeTaskData:CheckTeamOpen()
    local level = RoleData.Instance:GetRoleLevel()
    return level >= 65
end

-- 组队确认界面显示的怪物信息
function ChallengeTaskData:GetEnterInfo(monster_gp_id)
    local icon = ""
    local name = ""
    local cfg_monster_gp = Cfg.MonsterGroupById(monster_gp_id)
    if cfg_monster_gp ~= nil then
        local monster_id = cfg_monster_gp.show_monster
        local cfg_monster = Cfg.MonsterById(monster_id)
        if cfg_monster then
            icon = cfg_monster.icon_id
            name = cfg_monster.name
        end
    end
    return {icon = icon, name = name}
end

