MAX_NETHER_WORLD_GATORY_TASK_NUM = 19
MAX_NETHER_WORLD_GATORY_CHAPTER_NUM = 20

DeepDarkOperate = {
    Info = 0,
    Battle = 1,
    Reward = 2,
}

WeekMark = {
    ["Mon"]=1,
    ["Tue"]=2,
    ["Wed"]=3,
    ["Thu"]=4,
    ["Fri"]=5,
    ["Sat"]=6,
    ["Sun"]=7,
}


DeepDarkFantasyData = DeepDarkFantasyData or BaseClass()
DeepDarkFantasyData.RANK_TYPE = 23
function DeepDarkFantasyData:__init()
    if DeepDarkFantasyData.Instance ~= nil then
        Debuger.LogError("[DeepDarkFantasyData] attempt to create singleton twice!")
        return
    end
    DeepDarkFantasyData.Instance = self

    self.config = Config.netherworld_purgatory_auto
    self:InitConfig()

    self.view_data = SmartData.New({flush_num = 0,send_data = nil,init_data = nil,quick_challenge = 0,reset = 0,chapters = 0})
    self.challenge_data = SmartData.New({pass = 1,info_list = {}}) -- 服务器直接下发的信息
    self.pass_data = {} -- 简化整理信息
    self.finish_data = SmartData.New({challenge_id = 1,flag_list = {}})
    self.model_data = SmartData.New({is_idle = 0, cur_state = 0,is_go_next = 0,is_enter_layer =0,is_go_battle = 0,is_next_enter = 0,start = 0})
    self.rank_data = SmartData.New({flush_num = 0})
    self.miling_data = SmartData.New({mid_key = 0,high_key = 0,silver_reward_flag = {},gold_reward_flag = {},flush = false})

    self.miling_mark = {}
    self.miling_reward_mark = {}

    self.bubble_data = SmartData.New({id = 0,talk_id = 0})
    self.bubble_cache = {}
end 

function DeepDarkFantasyData:InitConfig()
    self.challenge_list = {}
    self.chapter_list = {}    

    for k,v in pairs(self.config.pass_reward) do 
        if self.challenge_list[v.chapters] == nil then 
            self.challenge_list[v.chapters] = {}
        end 

        local vo = {}
        vo.challenge_id = v.challenge_id
        vo.chapter = v.chapters
        vo.reward_list = {}
        for i = 1,3 do 
            local co = {}
            co.reward = v["reward_show_"..i]
            co.challenge_id = v.challenge_id
            co.chapter = v.chapters
            co.index = i
            table.insert( vo.reward_list, co )
        end 

        vo.monster_list = {}
        for i = 1,1 do
            local co = {}
            co.monster_group_id = v["monster_group_id_"..i]
            co.conditions = {}
            for j = 1,3 do 
                co.conditions[j] = v["group"..i.."_condition_id_"..j]
            end 
            vo.monster_list[co.monster_group_id] = co
        end 

        if v.challenge_id == MAX_NETHER_WORLD_GATORY_TASK_NUM then 
            vo.final_reward = {
                reward = v.final_reward,
                chapter = v.chapters, 
                challenge_id = v.challenge_id, 
                index = 4,
            }
        end 
        self.challenge_list[v.chapters][v.challenge_id] = vo

        local vo = {}
        vo.chapter = v.chapters
        self.chapter_list[v.chapters] = vo
    end 

    self.monster_group_list = {}
    for k,v in pairs(self.config.level_monster_group) do 
        local vo = {}
        vo.chapters = v.chapters
        vo.challenge_id = v.challenge_id
        vo.unlock_time = v.unlock_tiem
        vo.group = {}
        for i = 1,1 do 
            local co = {}
            co.monster_group_id = v["monster_group_id_"..i]
            co.recommend = v["recommend_"..i]
            co.challenge_id = v.challenge_id
            co.index = i
            co.conditions = {}
            co.bubble_id = v["bubble_id_"..i]
            co.res_size = v["res_size_"..i]
            for j = 1,3 do 
                co.conditions[j] = v["group"..i.."_condition_id_"..j]
            end 
            table.insert( vo.group, co )
        end 
        if self.monster_group_list[v.chapters] == nil then self.monster_group_list[v.chapters] = {} end 
        self.monster_group_list[v.chapters][v.challenge_id] = vo
    end 
end

function DeepDarkFantasyData:ClearData()
    self.init_pass = nil
    self.init_chapter = nil
    self.cache_list = nil
end 

function DeepDarkFantasyData:ClearInit()
    self.init_pass = nil
    self.init_chapter = nil
end

function DeepDarkFantasyData:GetChallengeBossList(chapters)

    -- local role_level = (self.challenge_data.set_level == nil or self.challenge_list[self.challenge_data.set_level] == nil)
    --     and RoleData.Instance:GetRoleLevel() or self.challenge_data.set_level

    local chapters = (chapters == nil or self.challenge_list[chapters] == nil) and 
        ((self.view_data.chapters == 0 or self.view_data.chapters == nil) and self:GetInitChapter() or self.view_data.chapters ) or chapters

    if self.cache_list ~= nil and self.cache_list[chapters] ~= nil then 
        return self.cache_list[chapters]
    end 

    local challenge_list = {}
    for k,v in pairs(self.challenge_list[chapters] or {}) do 
        local cfg = self.monster_group_list[v.chapter][v.challenge_id]
        local vo = {
            challenge_id = v.challenge_id,
            chapter = v.chapter,
            reward_list = v.reward_list,
            final_reward = v.final_reward,
            unlock_time = cfg.unlock_time
        }

        -- local mon_id = self.challenge_data.info_list[v.challenge_id] ~= nil 
        --     and self.challenge_data.info_list[v.challenge_id].monster_id or -1

        local rand_mark = 1
        local list = {}
        for i = 1,1 do
            local mon_group = Cfg.MonsterGroupById(cfg.group[i].monster_group_id)
            local monster = Cfg.MonsterById(mon_group.show_monster)
            local vo = {
                id = monster.monster_id,
                name = monster.name,
                monster_icon = monster.icon_id,
                recommend = cfg.group[i].recommend,
                conditions = v.monster_list[cfg.group[i].monster_group_id].conditions,
                bubble_id = cfg.group[i].bubble_id,
                res_size = cfg.group[i].res_size,
            }
            -- 随机标记，随机怪物三选一功能已删除
            -- rand_mark = cfg.group[i].monster_group_id == mon_id and i or rand_mark
            table.insert(list,vo)
        end

        vo.monster_id = list[rand_mark].id
        vo.monster_icon = list[rand_mark].monster_icon
        vo.monster_recommend = list[rand_mark].recommend
        vo.monster_name = list[rand_mark].name
        vo.monster_bubble_id = list[rand_mark].bubble_id
        vo.monster_res_size = list[rand_mark].res_size

        for i = 1,3 do 
            vo.reward_list[i].condition = list[rand_mark].conditions[i]
        end 
        table.insert(challenge_list,vo)
    end 

    for k,v in pairs(challenge_list) do 
        v.index = k
    end 

    local last_one = challenge_list[#challenge_list]
    table.remove( challenge_list, #challenge_list )

    if self.cache_list == nil then 
        self.cache_list = {}
    end 

    self.cache_list[chapters] = {}

    self.cache_list[chapters].challenge_list = challenge_list
    self.cache_list[chapters].last_one = last_one

    return self.cache_list[chapters]
end

function DeepDarkFantasyData:GetChallengeChapterList()
    return self.chapter_list
end

function DeepDarkFantasyData:GetBossInfoByChallengeId(challenge_id)
    local list = self:GetChallengeBossList()

    for k,v in pairs(list.challenge_list) do
        if v.challenge_id == challenge_id then 
            return v
        end 
    end 
    
    return list.last_one
end

function DeepDarkFantasyData:__delete()
    
end

function DeepDarkFantasyData:GetMiLingGoldReward()
    -- local role_level = (self.challenge_data.set_level == nil or self.challenge_list[self.challenge_data.set_level] == nil)
    -- and RoleData.Instance:GetRoleLevel() or self.challenge_data.set_level
    local chapters = self:GetCurMiLingChapter(true)

    if self.gold_cache_list ~= nil and self.gold_cache_list[chapters] ~= nil then 
        return self.gold_cache_list[chapters]
    end 
    local list = {}
    for k,v in pairs(self.config.gold_reward) do 
        if v.chapters == chapters then
            table.insert( list, v )
        end 
    end 
    list = KeyList(list,"lattice")

    if self.gold_cache_list == nil then 
        self.gold_cache_list = {}
    end 

    self.gold_cache_list[chapters] = list

    return self.gold_cache_list[chapters]
end

function DeepDarkFantasyData:GetMiLingSilverReward()
    -- local role_level = (self.challenge_data.set_level == nil or self.challenge_list[self.challenge_data.set_level] == nil)
    -- and RoleData.Instance:GetRoleLevel() or self.challenge_data.set_level

    local chapters = self:GetCurMiLingChapter(false)

    if self.silver_cache_list ~= nil and self.silver_cache_list[chapters] ~= nil then 
        return self.silver_cache_list[chapters]
    end 
    local list = {}
    for k,v in pairs(self.config.silver_reward) do 
        if v.chapters == chapters then
            table.insert( list, v )
        end 
    end 
    list = KeyList(list,"lattice")

    if self.silver_cache_list == nil then 
        self.silver_cache_list = {}
    end 

    self.silver_cache_list[chapters] = list

    return self.silver_cache_list[chapters]
end

-- 刷新方案组
function DeepDarkFantasyData:FlushCall()
    self.view_data.flush_num = self.view_data.flush_num + 1
end

function DeepDarkFantasyData:FlushView(send_data)
    self.view_data.send_data = send_data
    self:FlushCall()
end

-- 结算界面or其他界面初始化
function DeepDarkFantasyData:OutInitView()
    local init_pass = self.finish_data.challenge_id
    local init_pass_chapter = self.finish_data.chapter_id

    if init_pass == 0 or init_pass > MAX_NETHER_WORLD_GATORY_TASK_NUM or init_pass_chapter > MAX_NETHER_WORLD_GATORY_CHAPTER_NUM then return end 

    local pass_id = init_pass
    local pass_chapter_id = init_pass_chapter
    local check_id = pass_id >= MAX_NETHER_WORLD_GATORY_TASK_NUM and 1 or pass_id + 1
    local check_chapter = pass_id >= MAX_NETHER_WORLD_GATORY_TASK_NUM and init_pass_chapter + 1 or init_pass_chapter
    local is_layer_change = self:CheckBossCanInit(check_id,check_chapter)
    self.init_pass = is_layer_change and check_id or pass_id -- self:GetInitIndex() 
    self.init_chapter = is_layer_change and check_chapter or pass_chapter_id -- self:GetInitChapter()

end

-- 主控界面初始化
function DeepDarkFantasyData:InitView()
    local index = self:GetInitIndex()
    local chapter = self:GetInitChapter()
    local info = self:GetChallengeBossList(chapter)

    local data = index == MAX_NETHER_WORLD_GATORY_TASK_NUM and info.last_one or info.challenge_list[index]
    self.view_data.init_data = data
end

function DeepDarkFantasyData:GetInitChapter()
    if self.challenge_data.pass_chapter > 0 and self.init_chapter ~= nil then 
        return self.init_chapter 
    else 
        self.init_chapter = nil
    end 
    -- 初始化
    -- pass chapter_pass已基本丧失原本的功能！
    -- if self.challenge_data.pass_chapter == 0 then return 1 end 
    local pass_chapter = 1
    for i = 1,MAX_NETHER_WORLD_GATORY_CHAPTER_NUM do 
        local next = self:CTChapterPass(i)
        if next == i then 
            pass_chapter = i
            break
        end 
    end 
    -- local pass_chapter = self:CTChapterPass(self.challenge_data.pass_chapter == 0 and 1 or self.challenge_data.pass_chapter )
    -- pass_chapter = pass_chapter >= MAX_NETHER_WORLD_GATORY_CHAPTER_NUM and MAX_NETHER_WORLD_GATORY_CHAPTER_NUM or pass_chapter
    return pass_chapter 
end

function DeepDarkFantasyData:GetInitIndex()
    if self.init_pass ~= nil then 
        return self.init_pass 
    else 
        self.init_pass = nil
    end 
    -- 初始化
    -- pass chapter_pass已基本丧失原本的功能！
    -- if self.challenge_data.pass == 0 then return 1 end 
    local pass_chapter = self.view_data.chapters == 0 and (self.challenge_data.pass_chapter == 0 and 1 or self.challenge_data.pass_chapter) or self.view_data.chapters

    for i = 1 ,MAX_NETHER_WORLD_GATORY_TASK_NUM  do 
        if self.pass_data[pass_chapter][i][1].pass == 0 and self:CheckBossCanInit(i,pass_chapter,true) then 
            return i
        end 
    end 

    return  1
end

function DeepDarkFantasyData:IsInitComplete()
    return  self.view_data.init_data == nil
end

function DeepDarkFantasyData:InitComplete()
    self.view_data.send_data = self.view_data.init_data
    self.view_data.init_data = nil
end

-- 直接获取当前显示的data
function DeepDarkFantasyData:CacheOperateData()
    return self:IsInitComplete() and self.view_data.send_data or self.view_data.init_data
end

function DeepDarkFantasyData:CheckIsVideoShow()
    return self.challenge_data.video_show == 0
end

function DeepDarkFantasyData:SetChallengeInfo(protocol)
    -- LogError("?SetChallengeInfo!",protocol.show_video)
    self.challenge_data.pass = protocol.pass_challenge_id
    self.challenge_data.pass_chapter = protocol.pass_challenge_chapters
    self.challenge_data.set_level = protocol.set_level
    self.challenge_data.video_show = protocol.show_video
    self.challenge_data.today_pass_times = protocol.today_pass_times
    -- self.challenge_data.week_flag = protocol.week_flag

    self.miling_data.low_key = protocol.low_key
    self.miling_data.mid_key = protocol.mid_key
    self.miling_data.high_key = protocol.high_key

    if self:CheckIsInit() then self:ResetCall() end 

    -- self:FixChallengeInfo()
end

function DeepDarkFantasyData:SetChallengeSingleInfo(protocol)
    local chapter_check = protocol.pass_challenge_chapters == 0 and 1 or protocol.pass_challenge_chapters
    if protocol.send_reason == 1 then 
        self.finish_data.challenge_id = protocol.pass_challenge_id
        self.finish_data.chapter_id =chapter_check
        local list = bit:d2b(protocol.new_pass_flag)
        for i = 1,3 do 
            self.finish_data.flag_list[i] = list[#list - (i-1)]
        end 
        -- 功能改变，这两个参 不参与值刷新
        -- self.challenge_data.pass = protocol.pass_challenge_id > self.challenge_data.pass and protocol.pass_challenge_id or self.challenge_data.pass
        -- self.challenge_data.pass_chapter =chapter_check > self.challenge_data.pass_chapter and chapter_check or self.challenge_data.pass_chapter

        self.finish_data.with_new = self.challenge_data.info_list[chapter_check][protocol.pass_challenge_id].pass_map ~= protocol.challeneg_info.pass_map
    end 
    if self.challenge_data.info_list[chapter_check] == nil or self.challenge_data.info_list[chapter_check][protocol.pass_challenge_id] == nil then 
        LogDG("2632！下发异常 章节", protocol.pass_challenge_chapters," 关卡 ",protocol.pass_challenge_id,"信息无法录入")
        return
    end 
    
    self.challenge_data.today_pass_times = protocol.today_pass_times 
    self.challenge_data.info_list[chapter_check][protocol.pass_challenge_id].round_num = protocol.challeneg_info.round_num
    self.challenge_data.info_list[chapter_check][protocol.pass_challenge_id].pass_map = protocol.challeneg_info.pass_map
    self.challenge_data.info_list[chapter_check][protocol.pass_challenge_id].reward_map = protocol.challeneg_info.reward_map
    
    self:FixChallengeInfo(chapter_check)
end

-- 章节下发
function DeepDarkFantasyData:SetChallengeSingleChapterInfo(protocol) 
    local chapter_check = protocol.chapter_id + 1
    if self.challenge_data.info_list[chapter_check] == nil then 
        self.challenge_data.info_list[chapter_check] = {}
    end 

    for i = 1,MAX_NETHER_WORLD_GATORY_TASK_NUM do 
        if self.challenge_data.info_list[chapter_check][i] == nil then 
            self.challenge_data.info_list[chapter_check][i] = {}
        end 
            
        self.challenge_data.info_list[chapter_check][i].round_num = protocol.chapter_info.challenge_info[i].round_num
        self.challenge_data.info_list[chapter_check][i].pass_map = protocol.chapter_info.challenge_info[i].pass_map
        self.challenge_data.info_list[chapter_check][i].reward_map = protocol.chapter_info.challenge_info[i].reward_map
    end 

    self.miling_data.silver_reward_flag[chapter_check] = {}
    self.miling_data.gold_reward_flag[chapter_check] = {}
    -- for i = 1,MAX_NETHER_WORLD_GATORY_TASK_NUM do 
        --LogError("?载入银库",chapter_check,protocol.chapter_info.silver_reward_flag)
        --LogError("?载入金库",chapter_check,protocol.chapter_info.gold_reward_flag)
    self.miling_data.silver_reward_flag[chapter_check] = protocol.chapter_info.silver_reward_flag
    self.miling_data.gold_reward_flag[chapter_check] = protocol.chapter_info.gold_reward_flag

        --LogError("?金库完成",chapter_check, self.miling_data.gold_reward_flag[chapter_check])
        --LogError("?银库完成",chapter_check, self.miling_data.silver_reward_flag[chapter_check])
    -- end 

    self:FixChallengeInfo(chapter_check)
end 

function DeepDarkFantasyData:CheckIsInit()
    return self.challenge_data.video_show == 0 and ViewMgr:IsOpen(DeepDarkFantasyView)
end

function DeepDarkFantasyData:FixChallengeInfo(chapter)
    -- for chapter = 1 ,MAX_NETHER_WORLD_GATORY_CHAPTER_NUM do

        if self.challenge_data.info_list[chapter] == nil then return end 

        for i= 1,MAX_NETHER_WORLD_GATORY_TASK_NUM do 
            local is_init = self.pass_data[chapter] == nil or self.pass_data[chapter][i] == nil
            if is_init then 
               if self.pass_data[chapter] == nil then self.pass_data[chapter] = {} end 
               self.pass_data[chapter][i] = {}
            end 
            for j = 1,3 do 
                if is_init then 
                    local vo = {}
                    vo.round_num = self.challenge_data.info_list[chapter][ i ].round_num
                    vo.pass = self.challenge_data.info_list[chapter][ i ].pass_map[j]
                    vo.reward = self.challenge_data.info_list[chapter][ i ].reward_map[j]
                    table.insert(self.pass_data[chapter][i],vo)
                else 
                    self.pass_data[chapter][i][j].round_num = self.challenge_data.info_list[chapter][i].round_num
                    self.pass_data[chapter][i][j].pass = self.challenge_data.info_list[chapter][i].pass_map[j]
                    self.pass_data[chapter][i][j].reward = self.challenge_data.info_list[chapter][i].reward_map[j]
                end 
            end 

            if i == MAX_NETHER_WORLD_GATORY_TASK_NUM then 
                if is_init then 
                    local vo = {}
                    vo.round_num = self.challenge_data.info_list[chapter][i].round_num
                    vo.pass = self.challenge_data.info_list[chapter][i].pass_map[1]
                    vo.reward = self.challenge_data.info_list[chapter][i].reward_map[0]
                    table.insert(self.pass_data[chapter][i],vo)
                else 
                    self.pass_data[chapter][i][4].monster_id = self.challenge_data.info_list[chapter][i].monster_id
                    self.pass_data[chapter][i][4].round_num = self.challenge_data.info_list[chapter][i].round_num
                    self.pass_data[chapter][i][4].pass = self.challenge_data.info_list[chapter][i].pass_map[1]
                    self.pass_data[chapter][i][4].reward = self.challenge_data.info_list[chapter][i].reward_map[0]
                end 
            end 
        end 
    -- end 

    self:FlushCall()
end

function DeepDarkFantasyData:SetBubble(id)
    local operate = self.bubble_cache[id]

    if operate == nil then
        operate = {}
        for k,v in pairs(self.config.bubble_list) do
            if v.bubble_id == id then
                table.insert( operate,v)
            end 
        end 
        operate = KeyList(operate,"talk_id")
        self.bubble_cache[id] = operate
    end

    self.bubble_data.id = id 
    self.bubble_data.talk_id = 1
end

function DeepDarkFantasyData:GetBubble()
    return self.bubble_cache[self.bubble_data.id]
end

function DeepDarkFantasyData:TurnBubble()
    local operate = self.bubble_cache[self.bubble_data.id]
    local plus = self.bubble_data.talk_id + 1
    if operate[plus] == nil then 
        self.bubble_data.talk_id = 1
    else 
        self.bubble_data.talk_id = plus
    end 
end

function DeepDarkFantasyData:CheckRemind()
    local boss_list = self:GetChallengeBossList()

    for k,v in pairs(boss_list.challenge_list) do 
        if self:CheckBossIsRed(v) then 
            return 1
        end 
    end 
    return 0
end

-- 判断解锁时间 回nil为已解锁
-- 注意，本功能已取缔
function DeepDarkFantasyData:CheckTransMitsTime(data)
    if self.challenge_data.week_flag > 0 then 
        return nil
    end 

    local server_time = TimeManager:GetServerTime()
    local dates = os.date("%a",server_time)
    local dat_num = WeekMark[dates]
    local check_unlock = data.unlock_time == 0 and 7 or data.unlock_time
    if check_unlock <= dat_num then return nil end 

    local last = check_unlock - dat_num 
    local date_s = os.date("*t",server_time)
    date_s.day = date_s.day + last
    local time = TimeCtrl.Instance:ChangDataStampTime(date_s) or 0
    return time
end

-- 判断解锁时间 回nil为已解锁
-- 注意，本功能已取缔
function DeepDarkFantasyData:CheckNextWeekTime()
    local server_time = TimeManager:GetServerTime()
    local dates = os.date("%a",server_time)
    local dat_num = WeekMark[dates]
    
    local last = 8 - dat_num + (1 - self.challenge_data.week_flag) * 7
    local date_s = os.date("*t",server_time)
    date_s.day = date_s.day + last
    local time = TimeCtrl.Instance:ChangDataStampTime(date_s) or 0
    return time
end

-- 检查boss是否应该显示红点
-- 规则 有箱子拿
function DeepDarkFantasyData:CheckBossIsRed(data)
    if self.pass_data[data.chapter][data.challenge_id] == nil then return false end 

    local pass_info = self.pass_data[data.chapter][data.challenge_id] 
    for k,v in pairs(pass_info) do 
        -- 对应的任务完成了但是箱子还没拿
        if v.pass == 1 and v.reward == 0 then 
            return true
        end 
    end 
    return false
end

-- 检查该箱子是否已完成
function DeepDarkFantasyData:CheckBossBoxPass(param)
    if param == nil or param.challenge_id == nil or param.index == nil then return false end 
    if self.pass_data[param.chapter][param.challenge_id] == nil or self.pass_data[param.chapter][param.challenge_id][param.index] == nil then return false end 

    local operate = self.pass_data[param.chapter][param.challenge_id][param.index]

    return operate.pass == 1 
end

-- 检查该箱子是否显示红点
function DeepDarkFantasyData:CheckBossBoxIsRed(param)
    if param == nil or param.challenge_id == nil or param.index == nil then return false end 
    if self.pass_data[param.chapter][param.challenge_id] == nil or self.pass_data[param.chapter][param.challenge_id][param.index] == nil then return false end 

    local operate = self.pass_data[param.chapter][param.challenge_id][param.index]
    return operate.pass == 1 and operate.reward == 0
end 

-- 检查该箱子是否点亮
function DeepDarkFantasyData:CheckBossBoxIsInter(param)
    if param == nil or param.challenge_id == nil or param.index == nil then return false end 
    if self.pass_data[param.chapter][param.challenge_id] == nil or self.pass_data[param.chapter][param.challenge_id][param.index] == nil then return false end 

    local operate = self.pass_data[param.chapter][param.challenge_id][param.index]
    return operate.pass ~= 1 or operate.reward ~= 1
end

-- boss是否被完美通关
function DeepDarkFantasyData:CheckBossIsComplete(data)
    if data == nil or data.challenge_id == nil then return false end 
    if self.pass_data[data.chapter][data.challenge_id] == nil then return false end 

    for i = 1,3 do 
        if self.pass_data[data.chapter][data.challenge_id][i].reward == 0 then 
            return false
        end  
    end 
    return true
end

-- boss是否被完美完成任务
function DeepDarkFantasyData:CheckBossIsCompleteMission(data)
    if data == nil or data.challenge_id == nil then 
        return false 
    end 
    if self.pass_data[data.chapter][data.challenge_id] == nil then 
        return false 
    end 

    for i = 1,3 do 
        if self.pass_data[data.chapter][data.challenge_id][i].pass == 0 then 
            return false
        end  
    end 
    return true
end

-- boss是否能初始化进入
function DeepDarkFantasyData:CheckBossCanInit(pass_id,pass_chapter,need_mission)

    local info = self:GetChallengeBossList(pass_chapter)
    
    local operate = pass_id >= MAX_NETHER_WORLD_GATORY_TASK_NUM and info.last_one or info.challenge_list[pass_id]
    local flag_lock = not self:CheckBossIsLock(operate) -- 没有锁定
    local flag_know = not self:CheckBossIsUnknow(operate) -- self:CheckTransMitsTime(operate) == nil -- 非未知
    local changed_layer = pass_id == 1 and pass_chapter - 1 > 0
    local flag_complete = need_mission or self:CheckBossIsCompleteMission(info.challenge_list[pass_id-1]) -- 上一个的任务已完成

    if changed_layer then 
        local info = self:GetChallengeBossList(pass_chapter-1)
        flag_complete = need_mission or self:CheckBossIsCompleteMission(info.last_one) -- 上一个的任务已完成
    end 

    return flag_lock and flag_know and flag_complete
end 

function DeepDarkFantasyData:GetMyAllBox()
    local result = 0
    for chapter,list in pairs(self.pass_data) do
        for k,v in pairs(list) do 
            for i,j in pairs(v) do
                if j.reward == 1 then 
                    result = result + 1
                end 
            end  
        end 
    end 

    return result
end

function DeepDarkFantasyData:CheckIsAllNeo(list,is_gold)
    local flag = true
    for k,v in pairs(list) do 
        flag = flag and (not self:CheckBoxRewardGot(v,is_gold))
    end 

    return flag
end

function DeepDarkFantasyData:CheckBoxRewardGot(data,is_gold)
    if data == nil or is_gold == nil then return false end 
    local operate_list = is_gold and self.miling_data.gold_reward_flag[data.chapters] or self.miling_data.silver_reward_flag[data.chapters]

    if operate_list == nil then return false end 
    return operate_list[data.lattice - 1] == 1
end

function DeepDarkFantasyData:CheckUnlockNum()
    local info = self:GetChallengeBossList()
    local num = 0 

    for k,v in pairs(info.challenge_list) do 
        if not self:CheckBossIsUnknow(v) then 
            num = num + 1
        end 
    end 

    if not self:CheckBossIsLock(info.last_one) then 
        num = num + 1
    end 

    return num
end

function DeepDarkFantasyData:CheckBossIsLock(data)
    if data == nil then return false end 

    local pass_check_chapter = self.challenge_data.pass_chapter == 0 and 1 or self.challenge_data.pass_chapter

    -- 新判断：上一关没打一律为未解锁
    local flag_pass = self.pass_data[data.chapter][data.challenge_id - 1] ~= nil and self.pass_data[data.chapter][data.challenge_id - 1][1].pass == 0

    return flag_pass
end

-- 是否未知与是否解锁同步咧！
function DeepDarkFantasyData:CheckBossIsUnknow(data)
    return self:CheckBossIsLock(data)
end

-- 传送注入
function DeepDarkFantasyData:TransMits(data)
    self.model_data.cur_state = 2
    self.cache_data = data
end

-- 传送完成释放
function DeepDarkFantasyData:TransMitsComplete()
    self:FlushView(self.cache_data)
    self.cache_data = nil
end

function DeepDarkFantasyData:SetChapter(data)
    self.view_data.chapters = data.chapter
    self:FlushCall()
end

function DeepDarkFantasyData:Challenge()
    self.model_data.cur_state = 3
end

function DeepDarkFantasyData:EnterLayer()
    self.model_data.cur_state = 1
end

function DeepDarkFantasyData:OperateModelData(type,state)
    self.model_data[type] = state
end

function DeepDarkFantasyData:CheckIsSameData(data)
    -- 放开检查
    if self.view_data.send_data == nil then return true end 
    return data.challenge_id == self.view_data.send_data.challenge_id and data.chapter == self.view_data.send_data.chapter
end

function DeepDarkFantasyData:ResetCall()
    self.view_data.reset = self.view_data.reset + 1
end

-- 密令标记
function DeepDarkFantasyData:MiLingMark(param)
    self.miling_mark = param
    self.miling_mark.is_mark = true
end

function DeepDarkFantasyData:UnMiLingMark()
    if self.miling_mark.is_mark then 
        self.miling_mark.after = self.miling_mark.is_gold and self.miling_data.high_key or self.miling_data.mid_key
        local item_info = {
            item_id = self.miling_mark.is_gold and CommonItemId.GoldFantasyKey or CommonItemId.SilverFantasyKey,
            num = self.miling_mark.after - self.miling_mark.before 
        }

        if item_info.num > 0 then 
            local item = Item.Create(item_info)
            PublicPopupCtrl.Instance:CenterI({item_id = item_info.item_id, num = item_info.num,
            color_str = item:ColorStr(),item = item,icon_id = item:IconId()})
        end 
    end 
    self.miling_mark.is_mark = false
end

-- 密令领取标记
function DeepDarkFantasyData:MiLingRewardMark(param)
    self.miling_reward_mark = param
    self.miling_reward_mark.is_mark = true
end

function DeepDarkFantasyData:UnMiLingRewardMark()
    if self.miling_reward_mark.is_mark then 
        local before_can_list = {}
        for k,v in pairs(self.miling_reward_mark.before) do 
            if k < MAX_NETHER_WORLD_GATORY_TASK_NUM and k > 0 and v == 0 then 
                before_can_list[k] = v
            end 
        end 
        local chapter = self.view_data.chapters
        local operate = self.miling_reward_mark.is_gold and self.miling_data.gold_reward_flag[chapter] or self.miling_data.silver_reward_flag[chapter]
        local mark_list = {}

        local list_data = self.miling_reward_mark.is_gold and self:GetMiLingGoldReward() or self:GetMiLingSilverReward()
        for k,v in pairs(operate) do 
            if before_can_list[k] ~= nil and v == 1 then 
                local item = Item.Create(self.miling_reward_mark.is_gold and list_data[k+1].gold_reward or list_data[k+1].silver_reward)
                table.insert(mark_list,item)
            end 
        end 

        if #mark_list > 0 then 
        ViewMgr:OpenView(GiftOpenView,{
            gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS,
            item_list = mark_list,
            is_not_show = true,})
        end 
    end
    

    self.miling_reward_mark.is_mark = false
end

function DeepDarkFantasyData:GetMiLingCurMaxRollNum(is_gold,chapter)
    local num = 0
    local operate_list = is_gold and self.miling_data.gold_reward_flag[chapter] or self.miling_data.silver_reward_flag[chapter]
    for i = 1,MAX_NETHER_WORLD_GATORY_TASK_NUM do 
        num = operate_list[i-1] == 0 and num + 1 or num
    end 

    return  num > 5 and 5 or num
end

function DeepDarkFantasyData:SetChallengeRankList(protocol)
    self.rank_data.rank_type = protocol.info.rank_type
    self.rank_data.special_type = protocol.info.special_type
    self.rank_data.my_rank_index = protocol.info.my_rank_index
    self.rank_data.rank_infos = protocol.info.rank_infos

    self.rank_data.flush_num = self.rank_data.flush_num + 1
end

function DeepDarkFantasyData:SetMiLingInfo(protocol)
    if self.miling_mark.is_mark then 
        self.miling_mark.before = self.miling_mark.is_gold and self.miling_data.high_key or self.miling_data.mid_key
    end 

    if self.miling_reward_mark.is_mark then 
        self.miling_reward_mark.before = self.miling_reward_mark.is_gold and self.miling_data.gold_reward_flag or self.miling_data.silver_reward_flag
    end 

    self.miling_data.low_key = protocol.low_key
    self.miling_data.mid_key = protocol.mid_key
    self.miling_data.high_key = protocol.high_key

    self.miling_data.silver_reward_flag = {}
    self.miling_data.gold_reward_flag = {}
    for i = 1,MAX_NETHER_WORLD_GATORY_CHAPTER_NUM do 
        --LogError("?章节载入!",i)
        --LogError("?银库!",protocol.silver_reward_flag[i])
        --LogError("?金库!",protocol.gold_reward_flag[i])
        self.miling_data.silver_reward_flag[i] = protocol.silver_reward_flag[i]
        self.miling_data.gold_reward_flag[i] = protocol.gold_reward_flag[i]

        --LogError("?银库结束!",self.miling_data.silver_reward_flag[i])
        --LogError("?金库结束!",self.miling_data.gold_reward_flag[i])
    end 

    self.miling_data.flush = not self.miling_data.flush

    self:UnMiLingMark()
    self:UnMiLingRewardMark()
end

function DeepDarkFantasyData:GetChapterParam(data)
    local chapter = data.chapter

    local vo = {}
    vo.chapter = chapter
    vo.cur_num = 0
    local pass_list = self.pass_data[chapter]
    for index,boss in pairs(pass_list) do 
        for star,box in pairs(boss) do 
            if star <=3 and box.pass == 1 then
                vo.cur_num = vo.cur_num + 1
            end
        end 
    end 

    vo.max_num = 0
    for k,v in pairs(self.challenge_list[chapter]) do 
        vo.max_num = vo.max_num + #v.reward_list
    end 
    vo.is_lock = self:GetChapterIsLock(data)

    return vo
end
function DeepDarkFantasyData:GetChapterIsLock(data)
    local check_chapter = self.challenge_data.pass_chapter == 0 and 1 or self.challenge_data.pass_chapter

    local flag_chapter_pass = self.pass_data[ data.chapter -1] ~= nil
    local flag_all_pass = false
    if flag_chapter_pass then 
        for i =1 ,MAX_NETHER_WORLD_GATORY_TASK_NUM do 
            -- 默认第一个任务为成功通关
            flag_all_pass = flag_all_pass or self.pass_data[data.chapter -1][i][1].pass == 0 
        end
    end 

    -- 条件1 被检查章节比已通过章节大 锁定
    -- 条件2 被检查章节的前一个章节关卡有未完成 锁定
    -- 弃置 data.chapter > check_chapter and not
    -- LogError("fuck!islock",flag_chapter_pass,flag_all_pass)
    return flag_all_pass
end
-- 检查是否能进入下一章
function DeepDarkFantasyData:CTChapterPass(init_chapter)
    local clear = not self:GetChapterIsLock({chapter = init_chapter + 1})
    return clear and init_chapter + 1 or init_chapter
end
function DeepDarkFantasyData:GetTodayTimes()
    return self.config.other[1].times - self.challenge_data.today_pass_times
end
function DeepDarkFantasyData:GetTodayCanChallenge()
    local num = self:GetTodayTimes()
    return num > 0
end
function DeepDarkFantasyData:GetCurMiLingChapter(is_gold)
    local operate_list = is_gold and self.miling_data.gold_reward_flag or self.miling_data.silver_reward_flag
    for i = 1,MAX_NETHER_WORLD_GATORY_CHAPTER_NUM do
        local flag_complete = true
        for j = 1,MAX_NETHER_WORLD_GATORY_TASK_NUM do 
            flag_complete = flag_complete and operate_list[i][j-1] == 1 
        end 


        if not flag_complete then 
            return i
        end 
    end 
    return MAX_NETHER_WORLD_GATORY_CHAPTER_NUM
end
function DeepDarkFantasyData:GetChapterRemind(data)
    if data == nil then return 0 end 

    local num = 0
    local operate_list = self.pass_data[data.chapter]
    for index,boss in pairs(operate_list) do 
        for box_index, box_info in pairs(boss) do 
            if box_info.pass == 1 and box_info.reward == 0 then 
                num = num + 1
            end 
            -- 足够了 比0大就行了
            if num > 0 then 
                break
            end 
        end 
    end 
    return num
end