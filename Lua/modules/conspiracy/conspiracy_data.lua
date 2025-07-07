ConspiracyData = ConspiracyData or BaseClass()
function ConspiracyData:__init()
    if ConspiracyData.Instance ~= nil then
        Debuger.LogError("[ConspiracyData] attempt to create singleton twice!")
        return
    end
    ConspiracyData.Instance = self
    self.dexter_auto = Config.dexter_auto
    self.dexter_reward = self.dexter_auto.dexter_reward
    self.pass_reward = self.dexter_auto.pass_reward
    self.sp_show  = self.dexter_auto.sp_show
    self.boss_sp_show = self.dexter_auto.boss_sp_show

    self.reward = self.dexter_auto.reward
    self.scene = self.dexter_auto.scene
    self.other = self.dexter_auto.other[1]
    self.boss_list = {}
    self.reward_list = {}
    self.pass_info = SmartData.New()
    self.fetch_flag = SmartData.New()
    self.scene_info = {}
    self.target_info = {}
    self.pass_reward_list = {}
    self.sp_show_list = {}
    self.special_reward = {}
    self:checkconfig()
    self.select_scene = SmartData.New()
    self.finish_reward = SmartData.New()
    self.cur_open_task = -1
    self.other_info = SmartData.New({weekly_pass_count = 0, day_pass_count = 0, click_panel = false})
    self.choose_info = nil
end
function ConspiracyData:__delete()
    -- body
end
function ConspiracyData:checkconfig()
    self.special_reward = string.split(self.other.special_reward_1, ":")
    self.special_reward2 = string.split(self.other.special_reward_2, ":")
    for _, value in ipairs(self.dexter_reward) do
        local index = value.seq + 1
        if self.boss_list[index] == nil then
            self.boss_list[index] = {}
        end
        self.boss_list[index][value.index] = value
    end
    for _, value in ipairs(self.pass_reward) do
        if self.pass_reward_list[value.level_group_id] == nil then
            self.pass_reward_list[value.level_group_id] = {}
        end
        table.insert(self.pass_reward_list[value.level_group_id], value)
    end
    for _, value in ipairs(self.reward) do
        if self.reward_list[value.group_id] == nil then
            self.reward_list[value.group_id] = {}
        end
        for k,v in pairs(value.reward) do--改成列表了
            table.insert(self.reward_list[value.group_id], Item.Create(v ))
        end
    end
    for i,v in ipairs(self.scene) do
        if self.scene_info[v.scene_id] == nil then
            self.scene_info[v.scene_id] = {}
        end
        self.scene_info[v.scene_id] = v
        self.target_info[v.target_id] = v
    end
    for i,v in ipairs(self.sp_show) do
        if self.sp_show_list[v.index] == nil then
            self.sp_show_list[v.index] = {--[[ {},{},{},{} ]]}
        end
        local spe_cfg = Cfg.SpecialEffect(v.sp_id)
        if spe_cfg then
            local vo = {}
            vo.sp_id = v.sp_id
            vo.qua = spe_cfg.sp_priority
            vo.tab_index = v.page
            vo.desc = spe_cfg.reward_description
            vo.icon = spe_cfg.icon_id
            vo.icon_type = ICON_TYPE.BUFF
            vo.name = spe_cfg.reward_name
            table.insert(self.sp_show_list[v.index]--[[ [v.page] ]], vo)
        else
            LogError("不存在spe", v.sp_id)
        end
    end
    self.boss_sp_show_list = {}
    for i,v in ipairs(self.boss_sp_show) do
        if self.boss_sp_show_list[v.sp_group] == nil then
            self.boss_sp_show_list[v.sp_group] = {}
        end
        local spe_cfg = Cfg.SpecialEffect(v.sp_id)
        if spe_cfg then
            local vo = {}
            vo.sp_id = v.sp_id
            vo.qua = spe_cfg.sp_priority
            vo.tab_index = v.page
            vo.desc = spe_cfg.reward_description
            vo.icon = spe_cfg.icon_id
            vo.icon_type = ICON_TYPE.BUFF
            vo.name = spe_cfg.reward_name
            table.insert(self.boss_sp_show_list[v.sp_group]--[[ [v.page] ]], vo)
        else
            LogError("不存在spe", v.sp_id)
        end
    end
end
function ConspiracyData:ChooseBoss()
    return self.choose_info
end
function ConspiracyData:SetChooseBoss(info)
    self.choose_info = info
    local first = self:FirstKill(self.choose_info.seq + 1,self.choose_info.index)
    local pass = self:WeekPass(self.choose_info.seq + 1,self.choose_info.index)
    -- LogDG("首次通关",first,"周击杀",pass)
    self.is_first = (first == false)
    self.is_week_first = (pass == false)
end
--3首杀2周首1高额
function ConspiracyData:BattleResult()
    -- if self.choose_info then
        if self.is_first then
            return 3
        end
        if self.is_week_first then
            return 2
        end
        local total = self:GetOffList()[1].time
        local num = total - self:JoinTimes()
        -- num = num < 0 and 0 or num
        if num >= 0 then
            return 1
        end
        return 0
    -- end
    -- return 0
end
function ConspiracyData:BossList()
    return self.boss_list
end
function ConspiracyData:BossListByIndex(index)
    return self.boss_list[index]
end
function ConspiracyData:RewardList(group_id)
    return self.reward_list[group_id] or {}
end
function ConspiracyData:SetPassInfo(list)
    self.pass_info:Set(list)
end
function ConspiracyData:SetFetchFlag(list)
    self.fetch_flag:Set(list)
end
function ConspiracyData:PassCount(level)
    local count = 0
    self.pass_info[level] = self.pass_info[level] == nil and {} or self.pass_info[level]
    for i = 1, self.other.monster_num do
        if self:WeekPass(level, i) then
            count = count + 1
        end
    end
    return count,self.other.monster_num
end
--首杀现在没有用
function ConspiracyData:FirstKill(level, index)
    self.pass_info[level] = self.pass_info[level] == nil and {} or self.pass_info[level]
    self.pass_info[level][index] = self.pass_info[level][index] == nil and 0 or self.pass_info[level][index]
    return bit:d2b(self.pass_info[level][index])[33 - 1] == 1
end
--这周打没打过
function ConspiracyData:WeekPass(level, index)
    self.pass_info[level] = self.pass_info[level] == nil and {} or self.pass_info[level]
    self.pass_info[level][index] = self.pass_info[level][index] == nil and 0 or self.pass_info[level][index]
    return bit:d2b(self.pass_info[level][index])[33 - 2] == 1
end
--==1表示已获取
function ConspiracyData:CanGet(level, index)
    if self.fetch_flag[level] == nil then
        return false
    end
    return self.fetch_flag[level][33-index] == 1
end
function ConspiracyData:SceneInfo(scene_id)
    return self.scene_info[scene_id]
end
function ConspiracyData:SceneKillInfo(info)
    self.kill_flag = info
end
function ConspiracyData:KillFlag(index)
    if self.kill_flag then
        return self.kill_flag[33-index] == 0
    end
    return true
end
function ConspiracyData:PassRewardList(level_group_id)
    return self.pass_reward_list[level_group_id]
end
function ConspiracyData:PassReward(level_group_id)
    local level = RoleData.Instance:GetRoleLevel()
    local list = self:PassRewardList(level_group_id)
    for index, value in ipairs(list) do
        if level >= value.min_level and level <= value.max_level then
            return string.split(value.reward_show, "|")
        end
    end
    return {}
end

function ConspiracyData:SetSelectScene(params)
    self.select_scene:Set(params)
end
function ConspiracyData:GetSelectScene()
    return self.select_scene
end

function ConspiracyData:FinishReward(params)
    self.finish_reward:Set(params)
end
function ConspiracyData:GetFinishReward()
    return self.finish_reward
end
function ConspiracyData:GetShowList(index)
    return self.sp_show_list[index]
end
function ConspiracyData:GetBossShowList(group)
    return self.boss_sp_show_list[group]
end
function ConspiracyData:IsSR(id)
    for k,v in pairs(self.special_reward) do
        if  tonumber(v) == tonumber(id) then
            return true
        end
    end
    return false
end
function ConspiracyData:IsSSR(id)
    for k,v in pairs(self.special_reward2) do
        if  tonumber(v) == tonumber(id) then
            return true
        end
    end
    return false
end
function ConspiracyData:SetOpenTask(params)
    self.cur_open_task = params
end
function ConspiracyData:OpenTask()
    return self.cur_open_task
end
function ConspiracyData:DailyInfo()
    local level = RoleData.Instance:GetRoleLevel()
    local total = 0
    local count = 0
    for key, value in pairs(self.scene) do
        --统计每个场景的通关次数
        if level >= value.level and TaskData.Instance:GetTaskIsCompleted(value.pre_task) then
            total = total + 1
            count = count + self:PassCount(value.index)
        end
    end
    total = total * self.other.monster_num
    return count, total
end
function ConspiracyData:CheckDaiyInfo()
    local count,total = self:DailyInfo()
    ActivityData.Instance:SetActivityFinishInfo(ActType.Conspiracy, count, total)
end

function ConspiracyData:GetOffList()
    return self.dexter_auto.reward_proportion
end

function ConspiracyData:SetJoinTimes(protocol)
    self.other_info.weekly_pass_count = protocol.weekly_pass_count or 0
    self.other_info.day_pass_count = protocol.day_pass_count or 0
end
function ConspiracyData:JoinTimes()
    return self.other_info.day_pass_count
end
function ConspiracyData:RedPoint()
    if MedalData.Instance:GetMedalRedNumQuick() > 0 then
        return 1
    end
    if self:HeightReward() > 0 then
        return 1
    end
    if self:CanGetReward() > 0 then
        return 1
    end
    return 0
end
function ConspiracyData:HeightReward()
    if self.other_info.click_panel == false then
        local num = self:GetOffList()[1].time - self:JoinTimes()
        if num > 0 then
            return 1
        else
            return 0
        end
    else
        return 0
    end
end
function ConspiracyData:PassAndNotGet(level)
    local pass, flag
    for i = 1, 7 do
        pass = self:WeekPass(level, i)
        flag = self:CanGet(level, i)
        if pass and not flag then
            return 1
        end
    end
    if self.scene[level].can_use_pass == 0 then
        return 0
    end
    if self:AllWeekKill(level) then
        return 0
    end
    local can, _ = self:CanSweep(level)
    if can then
        return 1
    end
    return 0
end
function ConspiracyData:CanGetReward()
    local role_level = RoleData.Instance:GetRoleLevel()
    local is_finish
    local open
    local pass
    local flag
    local total = 0
    for index, value in ipairs(self.scene) do
        is_finish = TaskData.Instance:GetTaskIsCompleted(value.pre_task)
        open = (role_level >= value.level and is_finish) == true
        if open then
            if self:PassAndNotGet(index) > 0 then
                return 1
            end
        end
    end
    return 0
end
function ConspiracyData:Finish()
    local num = self:GetOffList()[1].time - self:JoinTimes()
    if num > 0 then
        return false
    else
        return true
    end
end
function ConspiracyData:SetSceneID(scene_id)
    self.scene_id = scene_id or SceneData.Instance:SceneID()
end
function ConspiracyData:SceneID()
    return self.scene_id
end
function ConspiracyData:WeeklySurplus()
    local role_level = RoleData.Instance:GetRoleLevel()
    local is_finish
    local open
    local pass
    local total = 0
    for index, value in ipairs(self.scene) do
        is_finish = TaskData.Instance:GetTaskIsCompleted(value.pre_task)
        open = (role_level >= value.level and is_finish) == true
        if open then
            for i = 1, 7 do
                pass = self:WeekPass(index, i)
                if not pass then
                    total = total + 1
                end
            end
        end
    end
    return total
end
function ConspiracyData:WeeklyTotal()
    local role_level = RoleData.Instance:GetRoleLevel()
    local is_finish
    local open
    local pass
    local total = 0
    for index, value in ipairs(self.scene) do
        is_finish = TaskData.Instance:GetTaskIsCompleted(value.pre_task)
        open = (role_level >= value.level and is_finish) == true
        if open then
            total = total + 7
        end
    end
    return total
end
function ConspiracyData:CanSweep(level)
    local list = self:SweepTip(level)
    for index, value in ipairs(list) do
        if value.green == false then
            return false, value
        end
    end
    return true
end
function ConspiracyData:SweepTip(index)
    local list = {}
    local info = self:SceneInfo(self:SceneID() or SceneData.Instance:SceneId())
    if index then
        info = self.scene[index]
    end
    if info then
        --先判断是否已全通关过
        list[1] = {}
        list[1].desc = Language.Conspiracy.SweepCondition1
        list[1].green = self:AllFirstKill(index)
        list[1].tip = Language.Conspiracy.ConditionAllFirst
        list[2] = {}
        list[2].desc = string.format(Language.Conspiracy.SweepCondition2, info.pass_level)
        list[2].green = RoleData.Instance:GetRoleLevel() >= info.pass_level
        list[2].tip = string.format(Language.Conspiracy.ConditionLevel, info.pass_level)
        list[3] = {}
        list[3].desc = string.format(Language.Conspiracy.SweepCondition3, info.pass_score, RoleData.Instance:GetSumScore())
        list[3].green = RoleData.Instance:GetSumScore() >= info.pass_score
        list[3].tip = string.format(Language.Conspiracy.ConditionScore,info.pass_score)
    end
    return list
end
function ConspiracyData:AllFirstKill(index)
    local info = self:SceneInfo(self:SceneID() or SceneData.Instance:SceneId())
    if index then
        info = self.scene[index]
    end
    if info == nil then
        return false
    end
    local flag = true
    for i = 1, self.other.monster_num do
        if not self:FirstKill(info.index, i) then
            flag = false
        end
    end
    return flag
end
function ConspiracyData:AllWeekKill(index)
    local info = self:SceneInfo(self:SceneID() or SceneData.Instance:SceneId())
    if index then
        info = self.scene[index]
    end
    if info == nil then
        return true
    end
    local flag = true
    for i = 1, self.other.monster_num do
        if not self:WeekPass(info.index, i) then
            flag = false
        end
    end
    return flag
end
function ConspiracyData:TargetAllKill(target_id)
    local info = self.target_info[target_id]
    local pass, total = self:PassCount(info.index)
    return pass >= total
end