

ChallengeData = ChallengeData or BaseClass()
ChallengeData.PROTOCOL_FBID_MAX = 10

function ChallengeData:__init()
    if ChallengeData.Instance ~= nil then
        Debuger.LogError("[ChallengeData] attempt to create singleton twice===>AchieveData:__init")
        return 
    end
    ChallengeData.Instance = self
    self.task_group = self.CreateTaskGroup()
    self.complete_task_list = SmartData.New()
    self.task_list = {}
    self.click_task_id = 0
    self.have_task_id = false   --判断服务端发来的任务列表中
    self.tmp_pre_task_id = 0
    self.click_task_level = 0

    self.fb_info = {
        cur_sel_fbid = 0,
        pass = SmartData.New(),
        next_refresh_timestamp = 0,
    }

    --{item_id,item_num}[]
    self.fb_reward = {

    }

    --通过副本id和chapter_id查找chapter信息
    Cfg.Register("ChallengeFbChapter",Cfg.CacheGet2Key(Config.challenge_fuben_auto.challenge_content,"fuben_id","chapter"))

end

function ChallengeData:__delete()
    if ChallengeData.Instance == self then
        ChallengeData.Instance = nil
    end
end

function ChallengeData:GetTaskList()
    -- if not self.have_task_id then return {} end
    local task_list = {}
    -- local config = Config.challengetask_list_auto.task_list
    -- local index = 21000
    -- while(index <= 21149)do
    --     if config[index].task_type == 2 then
    --         table.insert(task_list,config[index])
    --     end
    --     index = index + 1
    -- end
    for k,v in pairs(self.task_group)do
        if type(v.final_task) == "table" then
            table.insert(task_list,v.final_task)
        elseif v.final_task == nil then
            table.insert(task_list,v[1])
        elseif v.final_task == 1 then
            v[#v].complete = true
            table.insert(task_list,v[#v])
        end
    end
    self.task_list = task_list
    return task_list
end

function ChallengeData:SetFinalTask()
    -- local complete_tasks = {21000,21001,21002,21003,21004,21007,21008,21011}
    for k, v in pairs(self.complete_task_list)do
        for x,y in pairs(self.task_group)do
            for i,j in pairs(y)do
                if type(j) == "table" and v == j.task_id then
                    if y[i+1] then
                        y.final_task = y[i+1]
                    elseif i == #y then
                        y.final_task = 1
                    end
                end
            end
        end
    end
end

function ChallengeData:IsHaveChallengeTask()
    local config = Config.challengetask_list_auto.task_list
    for k, v in pairs(self.complete_task_list)do
        for x,y in pairs(config)do
            if v == y.task_id then
                self.have_task_id = true
                return true
            end
        end
    end
    self.have_task_id = false
    return false
end

function ChallengeData:CreateTaskGroup()
    local config = Config.challengetask_list_auto.task_list
    local index = 21000
    local count = 0
    local pre_task_name = config[index].task_name
    for k,v in pairs(config)do
        count = count + 1
    end
    count = index + count - 1
    -- Debuger.LogError("count----------"..count)
    local task_groups = {}
    local task_group = {}
    local group_index = 0
    local group_index2 = 1
    while(index <= count)do
        if config[index] == nil then
            -- Debuger.LogError("task_id :"..index.."canot find")
        else
            if pre_task_name == config[index].task_name then
                if task_groups[group_index2] == nil then task_groups[group_index2] = {} end
                table.insert(task_groups[group_index2],config[index])
            else
                group_index2 = group_index2 + 1
                if task_groups[group_index2] == nil then task_groups[group_index2] = {} end
                table.insert(task_groups[group_index2],config[index])
                pre_task_name = config[index].task_name
            end
            -- if config[index].pretaskid1 == "" then
            --     local task_group = {}
            --     group_index = group_index + 1
            --     table.insert(task_group,config[index])
            --     table.insert(task_groups,task_group)
            -- else
            --     table.insert(task_groups[group_index],config[index])
            -- end
        end
        index = index + 1
    end
    for k, v in pairs(task_groups)do
        for x,y in pairs(v)do
            y.first_pre_task = v[1].pretaskid1
        end
    end
    return task_groups
end

function ChallengeData:CheckIsTaskComplete(task_id)
    for k, v in pairs(self.complete_task_list)do
        if task_id == v then
            return true
        end
    end
    return false
end

function ChallengeData:GetNpc(task_id)
    for k, v in pairs(self.task_group)do
        for x,y in pairs(v)do
            if type(y) == "table" and task_id == y.task_id then
                if x == 1 then
                    return v[1].commit_npc
                else
                    return (v[1].commit_npc + x - 1)
                end
            end
        end
    end
    Debuger.LogError("[ERROR] can not find task id:"..task_id)
    return 0
end


function ChallengeData:SetFbPassInfo(fb_id,chapter_id)
    self.fb_info.pass[fb_id] = chapter_id
end

function ChallengeData:GetFbPassedChapter(fb_id)
    return self.fb_info.pass[fb_id] or 0
end

function ChallengeData:GetCurSelFbid()
    return self.fb_info.cur_sel_fbid
end

function ChallengeData:SetCurSelFbid(fbid)
    self.fb_info.cur_sel_fbid = fbid
end

function ChallengeData:GetFreshTS()
    return self.fb_info.next_refresh_timestamp
end

function ChallengeData:SetFreshTS(timestamp)
    self.fb_info.next_refresh_timestamp = timestamp
end

--{item_id,item_num}[]
function ChallengeData:SetFbReward(item_list)
    self.fb_reward = item_list
end
function ChallengeData:GetFbReward(item_list)
    return self.fb_reward
end

function ChallengeData:GetFubenList()
    local fuben_list = {}
    local pre_id = -1
    local config = Config.challenge_fuben_auto.challenge_content
    for k, v in pairs(config)do
        if v.fuben_id ~= pre_id then
            table.insert(fuben_list,v)
            pre_id = v.fuben_id
        end
    end
    return fuben_list
end

function ChallengeData:GetBossList(fuben_id)
    local config = Config.challenge_fuben_auto.challenge_content
    local boss_list = {}
    for k, v in pairs(config)do
        if v.fuben_id == fuben_id then
            table.insert(boss_list,v)
        end
    end
    return boss_list
end

function ChallengeData.GetFbStageReward(fb_id)
    local cfg = Config.challenge_fuben_auto.stage_reward[fb_id]
    if cfg == nil or cfg.stage_reward == nil then
        Debuger.LogErrorSD("ChallengeData.GetFbStageReward ERROR NoExist! fb_id=%s",tostring(fb_id))
    end
    return cfg and cfg.stage_reward and cfg.stage_reward or {}
end

function ChallengeData:GetTaskAwardItemList(task_id)
    local config = Config.challengetask_list_auto.task_award
    for k, v in pairs(config)do
        if v.task_id == task_id then
            return v.item_list
        end
    end
    Debuger.LogError("[ERROR] 在任务奖励列表中找不到ID："..task_id)
    return nil
end

function ChallengeData:GetTaskById(task_id)
    local config = Config.challengetask_list_auto.task_list
    for k, v in pairs(config)do
        if v.task_id == task_id then
            return v.task_name
        end
    end
    return ""
end

function ChallengeData:GetTaskNameByID(task_id)
    local config = Config.challengetask_list_auto.task_list
    if config[task_id] then
        return config[task_id].task_name
    end
    return ""
end

function ChallengeData:JudgeIsChallengeTaskComplete()
    local config = Config.challengetask_list_auto.task_list
    local count = 0
    for k, v in pairs(config)do
        count = count + 1
    end
    local screen = {}
    for x,y in pairs(self.complete_task_list)do
        if y/10000 == 2 then
            table.insert(screen,y)
        end
    end
    if #screen == count then
        return true
    end
    return false
end

function ChallengeData:GetConditionTip()
    return Language.Challenge.GetConditionTip[self:JudgeIsChallengeTaskComplete() and 2 or 1]
end