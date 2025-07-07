DailyActGuideCtrl = DailyActGuideCtrl or BaseClass(BaseCtrl)
--10002 门派任务
--10042 九州秘宝
--10041 万灵话本
--10009 三界除魔

function DailyActGuideCtrl:__init()
    if DailyActGuideCtrl.Instance ~= nil then
        Debuger.LogError("[DailyActGuideCtrl] attempt to create singleton twice!")
        return
    end
    DailyActGuideCtrl.Instance = self
    self.flushTaskListFunc = BindTool.Bind(self.FlushTaskList,self)
    --根据创角天数决定是否有任务显示
    self.create_day_lis = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.onRoleCreateTimeChange,self),"create_time")
    -- self:addListCare()
    self.wait_server_time_handle = nil
end

function DailyActGuideCtrl:OnInit()
    
end

function DailyActGuideCtrl:closeList()
    if not self:isOpening() then
        return
    end
    -- LogError("!!!close LIST")
    self:clearListCare()
    TaskData.Instance:DelAllTypeTask(TaskConfig.TaskType.day_act);
end

function DailyActGuideCtrl:openList()
    if self:isOpening() then
        return
    end
    -- LogError("@@@open LIST")

    self:addListCare()
    self.flushTaskListFunc()
end

local ex_task_cfg = nil

local function get_ex_task_cfg()
    if ex_task_cfg == nil then
        local cfg = Config.function_guide_auto.pet_skill[1]
        ex_task_cfg = {
            activity_id = cfg.activity_id,
            task_dec = cfg.activity_dec,
            guide_id = cfg.open_guide,
        }
    end
    return ex_task_cfg
end

local function ex_pref_key(task_id)
    return string.format("EXDailyActGuideTaskFlag_%d_%d",RoleData.Instance:GetRoleId(),task_id) 
end

local function ex_task_guide_lv()
    return Config.function_guide_auto.pet_skill[1].guide_level
end

local function ex_task_guide_day()
    return Config.function_guide_auto.pet_skill[1].guide_day
end

function DailyActGuideCtrl:clearListCare()
    if self.sm_handle1 ~= nil then
        ActivityData.Instance.active_data:Uncare(self.sm_handle1)
        self.sm_handle1 = nil
    end
    if self.sm_handle2 ~= nil then
        RoleData.Instance:GetBaseData():Uncare(self.sm_handle2)
        self.sm_handle2 = nil
    end
end

function DailyActGuideCtrl:addListCare()
    if self.sm_handle1 == nil then
        self.sm_handle1 = ActivityData.Instance.active_data:Care(self.flushTaskListFunc,"active_num","flag_list","reward_flag");
    end
    if self.sm_handle2 == nil then
        self.sm_handle2 = RoleData.Instance:GetBaseData():Care(self.flushTaskListFunc,"level")
    end
end

function DailyActGuideCtrl:internalOnRoleCreateTimeChange()
    local cfg = Config.function_guide_auto.pet_skill[1]
    local create_day = RoleData.Instance:CreateDayEx()
    -- RoleData.Instance:CreateDayEx()
    -- LogError("create_day===",create_day,RoleData.Instance:GetCreateTime(),TimeCtrl.Instance:IsServerTimeReady(),
    --     TimeHelper.GetServerTime() - RoleData.Instance:GetCreateTime())
    if (cfg.start_time <= create_day and cfg.end_time > create_day) or (create_day == ex_task_guide_day()) then
        -- LogError("1111111")
        self:openList()
    else
        -- LogError("222222222")
        self:closeList()
    end
end

function DailyActGuideCtrl:onRoleCreateTimeChange()
    if self.wait_server_time_handle ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.wait_server_time_handle)
        self.wait_server_time_handle = nil
    end
    if TimeCtrl.Instance:IsServerTimeReady() then
        self:internalOnRoleCreateTimeChange()
    else
        self.wait_server_time_handle = Runner.Instance:RunUntilTrue(function()
            if TimeCtrl.Instance:IsServerTimeReady() then
                self.wait_server_time_handle = nil
                self:internalOnRoleCreateTimeChange()
                return true
            else
                return false
            end
        end)
    end
end

function DailyActGuideCtrl:isOpening()
    return self.sm_handle1 ~= nil
end


function DailyActGuideCtrl:__delete()
    self:clearListCare()
    if DailyActGuideCtrl.Instance == self then
        DailyActGuideCtrl.Instance = nil
    end
end

function DailyActGuideCtrl:OnUnloadGameLogic()
    self:clearListCare()
    -- self.data:ResetData()
end


local function pref_key(task_id)
    return string.format("DailyActGuideTaskFlag_%d_%d",RoleData.Instance:GetRoleId(),task_id) 
end

--任务是会每天刷新，为false则表示 每个任务只会出现一次
local function fresh_flag()
    return Config.function_guide_auto.pet_skill[1].is_refersh == 1
end

function DailyActGuideCtrl:FlushTaskList()
    local cfg = Config.function_guide_auto.pet_skill[1]
    local create_day = RoleData.Instance:CreateDayEx()
    if ex_task_guide_day() == create_day then
        if ex_task_guide_lv() > RoleData.Instance:GetRoleLevel() then  --等级没到不刷新
            return
        end
        local ex_cfg = get_ex_task_cfg()
        local ex_type = ex_cfg.activity_id
        local ex_daily_cfg = ActiveData.Instance:GetActiveConfig(ex_type)
        if ex_daily_cfg ~= nil then
            local ex_act_type = ex_daily_cfg.act_type;
            local ex_task_id = DailyActGuideCtrl.TaskId(ex_act_type)
            if UnityPlayerPrefs.GetInt(ex_pref_key(ex_task_id),0) == 1 then
                return
            end
            local ex_old_task_info = TaskData.Instance:GetTaskInfoByTaskId(ex_task_id)
            local ex_lv_con = not ActivityData.Instance:IsNoEnoughLevel(ex_act_type,true)
            local ex_day_con = create_day >= ex_daily_cfg.open_day
            if not ex_lv_con or not ex_day_con then 
                return
            end
            local ex_flag = ActivityData.Instance:GetActiveFlag(ex_type)
            if ex_flag == ActiveConfig.FlagType.CantReward then
                if ex_old_task_info == nil then
                    local ex_act_cfg = ActivityData.GetConfig(ex_act_type)
                        TaskCtrl.Instance:CustomTask({
                            task_id = ex_task_id,
                            task_type = TaskConfig.TaskType.day_act,
                            task_name =  ex_act_cfg.name_text,
                            task_goal =  ex_task_cfg.task_dec,
                            guide_step = ex_task_cfg.guide_id,
                        })
                end
            else
                if ex_old_task_info ~= nil then
                    TaskData.Instance:TaskInfoDeleteByTaskId(ex_task_id)
                end
                UnityPlayerPrefs.SetInt(ex_pref_key(ex_task_id),1)
            end
        end
        return
    end


    if TimeCtrl.Instance:IsServerTimeReady() then
        if cfg.start_time > create_day or cfg.end_time <= create_day then
            -- LogError("FlushTaskList: TRIGGER CLOSE create_day===",create_day)
            self:closeList()
            return 
        end
    end
    if cfg.start_level > RoleData.Instance:GetRoleLevel() then  --等级没到不刷新
        return
    end
    local old_task_info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.day_act)
    local task_cfgs = Config.function_guide_auto.day_task_guide
    for k,task_cfg in ipairs(task_cfgs) do
        local type = task_cfg.activity_id
        local daily_cfg = ActiveData.Instance:GetActiveConfig(type)
        -- LogError("BBBB!!act===",type,daily_cfg)
        if daily_cfg ~= nil then
            local act_type = daily_cfg.act_type;
            local lv_con = not ActivityData.Instance:IsNoEnoughLevel(act_type,true)
            local day_con = create_day >= daily_cfg.open_day
            -- LogError("AAA!!act===",act_type,lv_con,day_con)
            if lv_con and day_con then
                local flag = ActivityData.Instance:GetActiveFlag(type)
                -- LogError("!!act===",act_type,flag)
                if flag == ActiveConfig.FlagType.CantReward then    --该每日未完成，加到任务列表里\
                    local task_id = DailyActGuideCtrl.TaskId(act_type)
                    -- if fresh_flag() == false 
                    --LogError("fresh_flag===",not fresh_flag(),UnityPlayerPrefs.GetInt(pref_key(task_id),0))
                    if fresh_flag() or 
                        UnityPlayerPrefs.GetInt(pref_key(task_id),0) == 0 then  --判断是否已经做过此任务
                        if old_task_info ~= nil then
                            if task_id ~= old_task_info.task_id then    --清空任务添加新任务
                                TaskData.Instance:DelAllTypeTask(TaskConfig.TaskType.day_act)
                                old_task_info = nil
                            else    --已有任务，不变
                                return
                            end
                        end
                        local act_cfg = ActivityData.GetConfig(act_type)
                        TaskCtrl.Instance:CustomTask({
                            task_id = task_id,
                            -- list_type = TaskConfig.ListType.accepted,
                            task_type = TaskConfig.TaskType.day_act,
                            task_name =  act_cfg.name_text,
                            -- task_desc =  "日常描述",
                            task_goal =  task_cfg.task_dec,
                            guide_step = task_cfg.guide_id,
                            -- npc_id = t.npc_id or 0,
                            -- expire_time = t.expire_time or 0,
                            -- reward_list = t.reward_list or {}
                        })
                        return
                    end
                else
                    local task_id = DailyActGuideCtrl.TaskId(act_type)
                    local pk = pref_key(task_id)
                    UnityPlayerPrefs.SetInt(pk,1)
                end
            end
        end
    end
    if old_task_info ~= nil then        --走到这儿表示没有需要显示的任务
        TaskData.Instance:DelAllTypeTask(TaskConfig.TaskType.day_act)
    end
end

-- function DailyActGuideCtrl:AddTask()
--     TaskCtrl.Instance:CustomTask({
--         task_id = 2110042,
--         -- list_type = TaskConfig.ListType.accepted,
--         task_type = TaskConfig.TaskType.day_act,
--         task_name =  "日常名字",
--         task_desc =  "日常描述",
--         task_goal =  "日常目标",
--         -- npc_id = t.npc_id or 0,
--         -- expire_time = t.expire_time or 0,
--         -- reward_list = t.reward_list or {}
--     })
-- end



function DailyActGuideCtrl.TaskId(act_type)
    return CUSTOM_OBJID.DAY_ACT + act_type
end

