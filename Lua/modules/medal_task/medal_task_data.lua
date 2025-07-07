MedalTaskData = MedalTaskData or BaseClass()
function MedalTaskData:__init()
    if MedalTaskData.Instance ~= nil then
        Debuger.LogError("[MedalTaskData] attempt to create singleton twice!")
        return
    end
    MedalTaskData.Instance = self


    self.task_config_list = Config.medaltask_list_auto.task_list
    self.task_view_list = Config.medaltask_list_auto.medal_task
    self.task_scene_config = Config.medaltask_scene_auto.medaltask
    self.task_limited_scene_config = Config.medaltask_scene_auto.limitnum_scene
    self.task_mail_config = Config.medaltask_list_auto.mail
    self.task_award_config = Config.medaltask_list_auto.task_award

    self.task_group_list = {}

    self.view_data = SmartData.New({
        cur_select_main_task = nil,
    })

    self.medal_task_help_data = SmartData.New({type = 1})
    self.task_scene_info =  nil

    self.scene_task_config = KeyList(self.task_scene_config,"scene_id")
    self:InitTaskGroup()
end

function MedalTaskData:InitTaskGroup()
    for k,v in pairs(self.task_config_list) do 
        if self.task_group_list[v.camp_left_value] == nil then 
            self.task_group_list[v.camp_left_value] = {}
        end 

        if self.task_group_list[v.camp_left_value][v.min_level] == nil then 
            self.task_group_list[v.camp_left_value][v.min_level] = {}
        end 
        table.insert( self.task_group_list[v.camp_left_value][v.min_level], v )
    end 

    self.medal_task_help_list = {}
	for k,v in pairs(Config.medaltask_list_auto.strategy_desc) do
		if self.medal_task_help_list[v.type] == nil then
			self.medal_task_help_list[v.type] = {}
		end
		self.medal_task_help_list[v.type][v.seq] = v
    end
    
    self.medal_sp_reward_list = {}
    for key, value in ipairs(self.task_award_config) do
        if value.left_value > 0 then
            if self.medal_sp_reward_list[value.left_value] == nil then
                self.medal_sp_reward_list[value.left_value] = {}
                self.medal_sp_reward_list[value.left_value][value.task_id] = {}
            end
            self.medal_sp_reward_list[value.left_value][value.task_id] = value
        end
    end
end

function MedalTaskData:GetTaskList()
    local cur_prof_type = RoleData.Instance:GetRoleProfType()
    local cur_prof = RoleData.Instance:GetRoleProf()
    local task_selected_list = {}

    for k,v in pairs(self.task_view_list) do 
        if v.job_type == cur_prof or v.job_type == -1 then 
            table.insert( task_selected_list, v )
        end 
    end 

    -- 一群屑
    for k,v in pairs(task_selected_list) do 
        -- v.task_list = self.task_group_list[cur_prof_type][v.level]
        v.task_list = self.task_group_list[-1][v.level]
    end 

    table.sort(task_selected_list,function(a,b)return a.level < b.level end)

    for i = 1,#task_selected_list do 
        task_selected_list[i].index = i
    end 
    return task_selected_list
end

function MedalTaskData:SetCurSelectMainTask(task_info)
    self.view_data.cur_select_main_task = task_info
end

function MedalTaskData:GetCurSelectMainTask()
    return self.view_data.cur_select_main_task
end

--获取勋章任务攻略
function MedalTaskData:GetMedalTaskHelpData()
	local type = self.medal_task_help_data.type
	return self.medal_task_help_list[type] or {}
end

function MedalTaskData:GetMedalTaskFbObjInfo(scene_id)
    local cur_scene_config = nil 
    local objs = {}
    for k,v in pairs(self.task_scene_config)do 
        if v.scene_id == scene_id then 
            cur_scene_config = v 
            break
        end 
    end 
    if cur_scene_config == nil then return {} end 
    local vo = {}

    local flag_complete = scene_id == self.cur_killed_scene_id

    if cur_scene_config.boss0_npc_id ~= "" then 
        if flag_complete then 
            -- 传送门
            vo.obj_type = 10
            vo.npc_id = 0
            vo.pos_x = cur_scene_config.pos_x
            vo.pos_y = cur_scene_config.pos_y
            table.insert( objs, vo )
        else
            local vo = {}
            vo.obj_type = 3
            vo.npc_id = cur_scene_config.boss0_npc_id
            vo.pos_x = cur_scene_config.boss_x0
            vo.pos_y = cur_scene_config.boss_y0
            table.insert( objs, vo )
        end 
    else 
        -- 传送门
        vo.obj_type = 10
        vo.npc_id = 0
        vo.pos_x = cur_scene_config.pos_x
        vo.pos_y = cur_scene_config.pos_y
        table.insert( objs, vo )
    end 
    if cur_scene_config.npc_id ~= "" then
        local vo = {}
        vo.obj_type = 2
        vo.npc_id = cur_scene_config.npc_id
        vo.pos_x = cur_scene_config.npc_x
        vo.pos_y = cur_scene_config.npc_y
        table.insert( objs, vo )
    end
    -- Boss
    return objs
end 

function MedalTaskData:SetMedalFbBossKilled(protocol)
    self.cur_killed_scene_id = protocol.scene_id
end

function MedalTaskData:CheckMedalSceneName(name,param)
    if self:IsInScene() then
        local scene_id = SceneData.Instance:SceneId()
        for k, v in pairs(self.task_scene_config) do
            if scene_id == v.scene_id and v.level == param and v.transport_id ~= "" then
                local cfg = SceneData.Instance:GetSceneCfg(v.transport_id)
                name = cfg ~= nil and cfg.name or Language.Fight.Exit
                break
            elseif scene_id == v.scene_id and v.level == param and v.transport_id == "" then
                name = Language.Fight.Exit
                break
            end
        end
    end
    return name
end

function MedalTaskData:GetMedalTaskSceneCfg(scene_id)
    for k,v in pairs (self.task_scene_config) do 
        if v.scene_id == scene_id then 
            return v
        end 
    end 
    return nil
end

function MedalTaskData:IsInScene()
    for k,v in pairs (self.task_scene_config) do 
        if v.scene_id == SceneData.Instance:SceneId() then 
            return true
        end 
    end 

    return false
end

function MedalTaskData:IsLimitedMedalTask(behavior_id,task_id)
    if behavior_id ~= nil then 
        for k,v in pairs(self.task_limited_scene_config) do
            if behavior_id == v.behavior_id and v.limit_num == 1 then 
                return true
            end 
        end 
    elseif behavior_id == nil and task_id ~= nil then 
        for k,v in pairs(self.task_limited_scene_config) do
            local list = Split(v.task_id,"|")
            for i,j in pairs(list) do 
                if tonumber(j) == task_id and v.limit_num == 1 then 
                    return true
                end 
            end 
        end 
    end 
    return false
end

function MedalTaskData:SetCurMedalDungeonInfo(behavior_id,task_id) 
    if behavior_id ~= nil then 
        for k,v in pairs(self.task_limited_scene_config) do
            if behavior_id == v.behavior_id then 
                self.task_scene_info = v
            end 
        end 
    elseif behavior_id == nil and task_id ~= nil then 
        for k,v in pairs(self.task_limited_scene_config) do
            local list = Split(v.task_id,"|")
            for i,j in pairs(list) do 
                if tonumber(j) == task_id then 
                    self.task_scene_info = v
                end 
            end 
        end 
    end 
end 

function MedalTaskData:GetCurMedalDungeonInfo()
    return self.task_scene_info 
end

function MedalTaskData:ClearCurMedalDungeonInfo()
    self.task_scene_info = nil
end

function MedalTaskData:GetCurSceneConfig()
    for k,v in pairs(self.task_scene_config) do 
        if v.scene_id == SceneData.Instance:SceneId() then 
            return v
        end 
    end 

    return nil
end

function MedalTaskData:GetMailTask(seq)
    return self.task_mail_config[seq]
end
function MedalTaskData:GetSpReward(prof_type, task_id)
    prof_type = prof_type or RoleData.Instance:GetRoleProfType()
    return self.medal_sp_reward_list[prof_type][task_id]
end