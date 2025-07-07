AchieveNewData = AchieveNewData or BaseClass()

AchieveNewData.StarCountPerStage = 5        --每阶段的星级
AchieveNewData.StageCount = 0              --阶段数,运行时算出

AchieveNewData.StarAchieveCount = 9        --单个星级阶段的成就数
AchieveNewData.BossState = {
    CantChanllage = -1,
    Chanllagalble = 0,
    Awardable = 1,
    AwardGetted = 2
}
local stage_start_cfg_index = {}
function AchieveNewData:__init()
    if AchieveNewData.Instance ~= nil then
        Debuger.LogError("[AchieveNewData] attempt to create singleton twice===>AchieveNewData:__init")
        return 
    end
    AchieveNewData.Instance = self
    self.cfg = Config.accomplishment_auto
    self.all_achieve_config = Config.accomplishment_auto.accomplishment
    self.achieve_challenge_config = Config.accomplishment_auto.promotion_challenge
    self:CreateAchieveStruct()
    
    self.click_stage = 0
    self.click_star = 0
    self.click_pindex = 0

    self.base = SmartData.New({
        cur_stage = 1,   --当前阶段    最后一个阶段完成之后 会升到下一个没有配置的阶段
        cur_star = 1,   --当前星级
        reward_stage = 0,--最近的哪个阶段奖励未领
        reward_star = 0,--最近的哪个星级奖励未领
        -- boss_state = -1, -- -1:不可挑战,0:可以挑战,1:可以领取,2:已经领取
        can_challenge = false,
        can_getreward = false,
        task_prog = {}, --key:index value:{prog[int],award_flag[bool]}
        flush_logic = false,--用于刷新是否可以挑战和是否可以领取
   })
    self.view_data = SmartData.New({
        flush_chanllage_btn = false,    --是否需要刷新晋级按钮显示
    })
    self.remind_handle_list = {}
    self.show_stage_up_flag = false      --下次打开成就界面的时候是否显示成就段位提升特效
    self.battle_map_id = nil

    -- ======= 历程数据
    self.course_base_data = SmartData.New({
        level = 0,                  -- 等级
        fraction = 0,               -- 历程点
        server_role_count = 0,      -- 全服玩家人数
        level_reward_flag = {},     -- 历程奖励等级领取标记
        finish_flag_list = {},      -- 历程完成标记
        progress_list = {},         -- 历程进度数据
        role_finish_count_list = {},-- 历程全服玩家完成数据
    })

    self:InitCourseCfg()
end

function AchieveNewData:__delete()
    if AchieveNewData.Instance == self then
        AchieveNewData.Instance = nil
    end
end

function AchieveNewData:Clear()
    self.base.task_prog = {}
    self.base.can_challenge = false
    self.base.can_getreward = false
    self.base.flush_logic = false
    self.base.cur_stage = 1
    self.base.cur_star = 1
    self.base.reward_stage = 0
    self.base.reward_star = 0
    self.view_data.flush_chanllage_btn = false
end

--no_overflow 如果当前数据超出配置最大值，则返回最大值
function AchieveNewData:GetCurStage(no_overflow)
    if no_overflow == true and self.base.cur_stage > AchieveNewData.StageCount then
        return AchieveNewData.StageCount
    end
    return self.base.cur_stage
end

function AchieveNewData:GetCurStar(no_overflow)
    if no_overflow == true and self.base.cur_stage > AchieveNewData.StageCount then
        return AchieveNewData.StarCountPerStage
    end
    return self.base.cur_star
end

function AchieveNewData:GetRewardStage()
    return self.base.reward_stage
end

function AchieveNewData:GetRewardStar()
    return self.base.reward_star
end

-- function AchieveNewData:GetBossState()
--     return self.base.boss_state
-- end

-- function AchieveNewData:SetBossState(boss_state)
--     self.base.boss_state = boss_state
-- end


function AchieveNewData:CreateAchieveStruct()
    local config = self.all_achieve_config
    -- for k, v in pairs(config)do
    --     v.index = k
    -- end
    local achieve_group = {}
    self.auto_index2achieve = {}

    for k, v in pairs(config)do
        v.index = k
        if achieve_group[v.stage] == nil then           
            achieve_group[v.stage] = {} 
            if v.stage > AchieveNewData.StageCount then
                AchieveNewData.StageCount = v.stage
            end
        end
        if achieve_group[v.stage][v.star] == nil then achieve_group[v.stage][v.star] = {} end
        achieve_group[v.stage][v.star][v.seq] = v
        self.auto_index2achieve[v.index] = v
    end
    self.achieve_group = achieve_group
    -- AchieveNewData.StageCount = 2
end

function AchieveNewData:GetTaskProgData(pindex,stage,star)
    if stage == nil then stage = self:GetCurStage(true) end
    if star == nil then star = self:GetCurStar(true) end
    local cfg = self:GetAchieveCfg(stage,star,pindex)
    if cfg ~= nil then
        return self:GetTaskProgDataByIndex(cfg.index)
    else
        return nil
    end
end

function AchieveNewData:SetAllInformations(protocol)
    self.base.cur_stage = protocol.cur_prog
    self.base.cur_star = protocol.cur_star
    self.base.reward_stage = protocol.cur_reward_prog
    self.base.reward_star = protocol.cur_reward_star
    if table.nums(self.remind_handle_list) > 0 then
        for i,v in pairs(self.remind_handle_list) do
            RemindCtrl.Instance:Remind():UnRegister(Mod.Achieve.Target, v)
        end
        self.remind_handle_list = {}
    end
    for i=1,#protocol.task_prog_list do
        local cfg = self:GetAchieveCfg(protocol.cur_prog,protocol.cur_star,i-1)
        local cfg_idx = cfg and cfg.index or -1
        local da = self.base.task_prog[cfg_idx]
        if da == nil then
            da = SmartData.New({cfg_idx = cfg_idx,
                    prog = protocol.task_prog_list[i],
                    award_flag = protocol.award_fetch_flag[i] == 1,
                })
            self.base.task_prog[cfg_idx] = da
        end
        local handle = RemindCtrl.Instance:Remind():Register(
            Mod.Achieve.Target,
            da,
            BindTool.Bind(self.CacuRemindCount, self, da.cfg_idx)
        )
        table.insert( self.remind_handle_list, handle)
    end
    self:MarkChanllageBtnFlush()
    -- if protocol.boss_button_stat > 0 then
    --     self.base.boss_state = protocol.boss_button_stat      
    -- else
    --     self.base.boss_state = self:CheckIsCanChallenge() and 0 or -1
    -- end
end

function AchieveNewData:GetOrCreateCompletedTaskProgData(pindex,stage,star)
    local cfg = self:GetAchieveCfg(stage,star,pindex)
    if cfg ~= nil then
        local re = self.base.task_prog[cfg.index]
        if re == nil then
            re = SmartData.New({cfg_idx = cfg.index,
                    prog = 99999999,
                    award_flag = true,
                })
            self.base.task_prog[cfg.index] = re
        end
        return re
    else
        return nil
    end
   
end

function AchieveNewData:MarkChanllageBtnFlush()
    self.view_data.flush_chanllage_btn = not self.view_data.flush_chanllage_btn
end

function AchieveNewData:GetTaskProgDataByIndex(index,ignore_err)
    local re = self.base.task_prog[index]
    if re == nil and (ignore_err == nil or ignore_err == false) then
        Debuger.LogErrorSD("AchieveNewData:GetTaskProgDataByIndex ERROR|index=%s",index)
    end
    return re
end

function AchieveNewData:IsAchieveAwardGetted(index)
    local prog_data = self:GetTaskProgDataByIndex(index)
    if prog_data == nil then
        return false
    end
    return prog_data.award_flag
end

function AchieveNewData:CheckAchieveIsComplete(index)
    local prog_data = self:GetTaskProgDataByIndex(index)
    if prog_data == nil then
        return false
    end
    -- Debuger.LogError("CheckAchieveIsComplete===index=%s,max=%s,cur_stage=%s",tostring(index),tostring(self:GetAchieveMaxProg(index)),tostring(prog_data.prog))
    return self:GetAchieveMaxProg(index) <= prog_data.prog
end

function AchieveNewData:GetAchieveCurProg(index)
    local prog_data = self:GetTaskProgDataByIndex(index)
    if prog_data == nil then
        return 0
    end
    return prog_data.prog
end

function AchieveNewData:GetAchieveMaxProg(index)
    return self:GetAchieveCfgByIndex(index) ~= nil and self:GetAchieveCfgByIndex(index).achv_cond_param or 0
end

-- function AchieveNewData:SetAchieveProgBySeq(index,prog)
--     self.achieve_prog[index] = prog 
-- end

function AchieveNewData:GetAchieveCfgByIndex(index)
    local re = self.all_achieve_config[index]
    if re == nil then
        Debuger.LogErrorSD("AchieveNewData:GetAchieveCfgByIndex ERROR Cfg not Exist|index=%s",tostring(index))
    end
    return re
end

--pindex [0,8]
function AchieveNewData:GetAchieveCfg(stage,star,pindex)
    -- LogError("GetAchieveCfg",stage,star,pindex)
    local stage_cfg = self.achieve_group[stage]
    if stage_cfg == nil then
        Debuger.LogErrorSD("AchieveNewData:GetAchieveCfgERROR Unkow Stage|stage=%s,star=%s,idx=%s",
            tostring(stage),tostring(star),tostring(pindex))
        return
    end
    local star_cfg = stage_cfg[star]
     if stage_cfg == nil then
        Debuger.LogErrorSD("AchieveNewData:GetAchieveCfgERROR Unkow Star|stage=%s,star=%s,idx=%s",
            tostring(stage),tostring(star),tostring(pindex))
    end
    local idx_cfg = star_cfg[pindex]
     if stage_cfg == nil then
        Debuger.LogErrorSD("AchieveNewData:GetAchieveCfgERROR Unkow Idx|stage=%s,star=%s,idx=%s",
            tostring(stage),tostring(star),tostring(pindex))
    end
   return idx_cfg
end

function AchieveNewData:GetClickAchieve()
    -- Debuger.LogError("seq:"..self.click_seq.." seq_1:"..self.click_seq_1.." seq_2:"..self.click_seq_2)
    -- if self.click_stage ~= 0 and self.click_star ~= 0 and self.click_pindex ~= 0 then
        return self:GetAchieveCfg(self.click_stage,self.click_star,self.click_pindex)
    -- end
end

function AchieveNewData.GetStarCfg(stage,star)
    local stage_cfg = stage_start_cfg_index[stage]
    if stage_cfg == nil then
        stage_cfg = {}
        stage_start_cfg_index[stage] = stage_cfg
    end
    local star_cfg = stage_cfg[star]
    if star_cfg == nil then
        local config = Config.accomplishment_auto.stage_group
        for k, v in pairs(config)do
            if v.stage_id == stage and v.star_id == star then
                star_cfg = v
                stage_cfg[star] = star_cfg
                break
            end
        end
    end
    if star_cfg == nil then
        Debuger.LogErrorSD("AchieveNewData:GetStarCfg ERROR|stage=%s,star=%s",tostring(stage),tostring(star))
    end
    return star_cfg
end

function AchieveNewData:GetCurStarData()
    return AchieveNewData.GetStarCfg(self:GetCurStage(true),self:GetCurStar(true)) 
end

--获取下一个等级的星级配置
function AchieveNewData:GetNextStarData()
    local next_star = self:GetCurStar() + 1
    local next_stage = self:GetCurStage()
    if next_stage > AchieveNewData.StageCount then
        return nil
    end
    if next_star > AchieveNewData.StarCountPerStage then
        next_star = 1
        next_stage = next_stage + 1
        if next_stage > AchieveNewData.StageCount then
            return nil
        end
    end
    return AchieveNewData.GetStarCfg(next_stage,next_star) 
end

function AchieveNewData:CheckIsCanChallenge()
    local stage_goups = self.achieve_group[self.base.cur_stage]
    if stage_goups == nil then
        return false
    end
    local list = stage_goups[self.base.cur_star]
    if list == nil then
        return false
    end
    for k, v in pairs(list)do
        local grop_data = self:GetTaskProgDataByIndex(v.index)
        if grop_data == nil then
            return false
        end
        if grop_data.award_flag ~= true then
            return false
        end
    end
    return true
end

--是否可以领取阶段奖励
function AchieveNewData:CanGetStageReward()
    local cur_num = self:GetCurStage() * 10 + self:GetCurStar()
    local reward_num = self:GetRewardStage() * 10 + self:GetRewardStar()
    if cur_num <= reward_num then
        return false
    end
    if self:GetRewardStage() >= AchieveNewData.StageCount and 
    self:GetRewardStar() >= AchieveNewData.StarCountPerStage then--可领奖励是最高阶
        return false
    end
    return true
end

function AchieveNewData:CacuRemindCount(index,checkC,checkR)
    if FunOpen.Instance:GetFunIsOpened(Mod.Achieve.Main) == false then
        return 0
    end
    if index then
        local grop_data = self:GetTaskProgDataByIndex(index)
        if grop_data ~= nil then
            if self:CheckAchieveIsComplete(index) then
                if grop_data.award_flag == false then
                    return 1
                end
            end
        end
        return 0
    end
    local re_count = 0
    local chanllagalble = true  --所有成就都已完成且已领取时，可以挑战晋级
    local stage_goups = self.achieve_group[self.base.cur_stage]
    if stage_goups ~= nil then
        local list = stage_goups[self.base.cur_star]
        for k, v in pairs(list)do
            local grop_data = self:GetTaskProgDataByIndex(v.index,true)
            if grop_data ~= nil then
                if self:CheckAchieveIsComplete(v.index) then
                    if grop_data.award_flag == false then
                        -- re_count = re_count + 1
                        chanllagalble = false
                        self.base.can_challenge = false
                        -- Debuger.LogErrorSD("CacuRemindCount=====0000000====111111")
                      return 1
                    end
                else
                    chanllagalble = false
                    self.base.can_challenge = false
                end
            end
        end    
    end

    if checkC and chanllagalble then
       -- Debuger.LogErrorSD("CacuRemindCount=====11111====111111")
       -- re_count = re_count + 1
        self.base.can_challenge = chanllagalble
        return 1
    end

    if checkR then
        local can_get = self:CanGetStageReward()
        self.base.can_getreward = self:CanGetStageReward()
        -- Debuger.LogErrorSD("CacuRemindCount=====22222====111111")
        -- re_count = re_count + 1
        return can_get and 1 or 0
    end
    -- Debuger.LogErrorSD("CacuRemindCount=====3333====0000000")
    return 0
    -- return re_count > 0 and 1 or 0
end

function AchieveNewData.GetTitleSpriteName(stage)
    local max_img = AchieveNewData.StageCount
    if stage > max_img then
        stage = max_img
    end
    return string.format("%d_copy",stage)
end

function AchieveNewData.GetStageName(stage,star)
    local star_cfg = AchieveNewData.GetStarCfg(stage,star == nil and 1 or star)
    local stage_name = star_cfg.stage_name
    if star ~= nil then
        stage_name = string.format(Language.NewAchieve.StageFullNameFormat,stage_name,star)
    end
    return stage_name
end

function AchieveNewData:ShowStageUpFlag()
    return self.show_stage_up_flag
end

function AchieveNewData:SetShowStateUpFlag(flag)
    self.show_stage_up_flag = flag
end

function AchieveNewData:SetChanllageMapId(map_id)
    self.battle_map_id = map_id
end

function AchieveNewData:GetChanllageMapId()
    return self.battle_map_id
end

function AchieveNewData:ClearChanllageMapId()
    self.battle_map_id = nil
end

function AchieveNewData:GetConfigOfChallenge()
    local cur_stage = self:GetCurStage(true)
    local cur_star = self:GetCurStar(true)

    for k,v in pairs(self.achieve_challenge_config) do
        if cur_stage == v.stage and cur_star == v.star then
            return v
        end 
    end 
    return nil
end

-- 设置通过阶段战斗的奖励
function AchieveNewData:SetPassStageReward(data)
    self.pass_stage_reward = data
end

function AchieveNewData:GetPassStageReward(data)
    return self.pass_stage_reward
end


-- ========== 历程 ==========

-- 初始化历程配置
function AchieveNewData:InitCourseCfg()
    local new_cfg = {}
    for i,v in pairs(self.cfg.course) do
        if new_cfg[v["stage"]] == nil then
            new_cfg[v["stage"]] = {}
        end
        table.insert(new_cfg[v["stage"]], v)
    end
    for i,v in ipairs(new_cfg) do
        table.sort(v, function (a,b)
            return a.sn < b.sn
        end)
    end
    self.course_child_group = new_cfg
    self.course_parent_group = {
        [0] = {
            id = 0,
            name = Language.AchieveCourse.Btn1,
            children = {
                [1] = {
                    stage = 0,
                    star_name = Language.AchieveCourse.Btn1Child,
                }
            },
        }
    }
    for i,v in pairs(self.cfg.course_group) do
        local index = v.stage
        if self.course_parent_group[index] == nil then
            self.course_parent_group[index] = {}
        end
        if self.course_parent_group[index].children == nil then
            self.course_parent_group[index].children = {}
        end
        if self.course_parent_group[index].id == nil then
            self.course_parent_group[index].id = v.stage
            self.course_parent_group[index].name = v.stage_name
        end
        table.insert(self.course_parent_group[index].children,v)
    end
    for i,v in ipairs(self.course_parent_group) do
        table.sort(v,function (a,b)
            return a.sn < b.sn
        end)
    end
end

function AchieveNewData:GetCourseBtnList()
    return self.course_parent_group
end

-- 获取历程总数据
function AchieveNewData:GetCourseBaseData()
    return self.course_base_data
end

-- -1 已领取 0 未达成 1可领取
-- 获取历程等级奖励标记
function AchieveNewData:GetCourseLevelRewardFlag(level)
    local self_level = self.course_base_data.level
    if level > self_level then
        return 0
    else
        local flag = self.course_base_data.level_reward_flag[level]
        if flag == 0 then
            return 1
        else
            return -1
        end
    end
end

-- 获取历程完成标记 1 为已完成
function AchieveNewData:GetCourseFinishFlag(seq)
    local flag = self.course_base_data.finish_flag_list[seq]
    return flag or 0
end

-- 获取历程条目进度
function AchieveNewData:GetCourseProgress(seq)
    local progress = self.course_base_data.progress_list[seq]
    return progress or 0
end

-- 获取历程条目完成人数
function AchieveNewData:GetCourseRoleCount(seq)
    local role_count = self.course_base_data.role_finish_count_list[seq]
    return role_count or 0
end

-- 是否有历程等级奖励可领
function AchieveNewData:CourseRemind()
    for i,v in pairs(self.cfg.course_reward) do
        if v.need_fraction > 0 then
            local flag = self:GetCourseLevelRewardFlag(v.course_level)
            if flag > 0 then
                return 1
            end
        end
    end
    return 0
end

-- 获得历程分组进度
function AchieveNewData:GetCourseGroupProgress(seq)
    local list_data = self.course_child_group[seq] or {}
    local progress_num = 0
    for i,v in pairs(list_data) do
        local flag = self:GetCourseFinishFlag(v.sn)
        if flag == 1 then
            progress_num = progress_num + 1
        end
    end
    local progress_max_num = #list_data
    return progress_num, progress_max_num
end

-- 获得历程大组进度
function AchieveNewData:GetCourseBigGroupProgress(big_stage)
    local progress_num = 0
    local progress_max_num = 0
    for i,v in pairs(self.cfg.course_group) do
        if v.stage == big_stage then
            local num, max_num = self:GetCourseGroupProgress(v.sn)
            progress_num = progress_num + num
            progress_max_num = progress_max_num + max_num
        end
    end
    return progress_num, progress_max_num
end