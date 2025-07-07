ShenYuProbeData = ShenYuProbeData or BaseClass()

function ShenYuProbeData:__init()
    if ShenYuProbeData.Instance ~= nil then
        Debuger.LogError("[ShenYuProbeData] attempt to create singleton twice!")
        return
    end
    ShenYuProbeData.Instance = self
    self.cfg = Config.divine_domain_explore_auto
    self.task_cfg = self.cfg.explore_task
    self.level_cfg = self.cfg.explore_level

    self.camp_data = SmartData.New()    -- 阵营信息
    self.role_data = SmartData.New()    -- 玩家信息
    self.boss_data = SmartData.New()    -- boss信息
    self.rank_data = SmartData.New()    -- 排行信息
    self.help_data = SmartData.New()    -- 协助信息     -- 记录了探索玩法已经用过的宠物或者伙伴，用过就不能用了

    self.well_view_data = SmartData.New()   -- 锁妖井界面信息
    self.well_pos_data = SmartData.New()    -- 锁妖井位置信息

    self.tab_data = SmartData.New({tab = ShenYuProbeView.TabType.Level})   -- 用作神域探索主界面的标签类型  

    self.sel_boss_id = nil    -- 当前选中了哪个boss

    --Cfg.Register("ShenYuProbeBossHpCfg",Cfg.RealTimeSearch(self.cfg.boss_configure,"degree"))

    -- 锁妖井坑位组
    self.well_task_group = DataHelper.ConvertGroup(self.cfg.suoyao_well_task, "seq")

    self.remind_data = SmartData.New({v = false})
end

function ShenYuProbeData:ClearData()
    self.camp_data:Set({})
    self.role_data:Set({})
    self.boss_data:Set({})
    self.rank_data:Set({})
    self.help_data:Set({})
end

function ShenYuProbeData:IsOpen()
    return LandsBetweenData.Instance:ExtralActShow(ActType.Probe)
end

-- 当前难度 暂时用不到
function ShenYuProbeData:GetDegree()
    return nil
end

--任务配置
function ShenYuProbeData:GetTaskCfg(tab_type)
    return self.task_cfg[tab_type]
end

--任务配置
function ShenYuProbeData:GetTaskCfgByTaskType(task_type)
    for i,v in pairs(self.task_cfg) do
        if v.way_type == task_type then
            return v
        end
    end
end


function ShenYuProbeData:GetTaskType(tab_type)
    local cfg = self:GetTaskCfg(tab_type)
    return cfg.way_type
end

function ShenYuProbeData:GetLevel()
    if TableIsEmpty(self.camp_data) then
        return 0
    end
    return self.camp_data.level or 0
end

--探索等级配置
function ShenYuProbeData:GetLevelCfg(level)
    return self.level_cfg[level] 
end

function ShenYuProbeData:GetMaxLevel()
    return table.nums(self.level_cfg)
end

-- 等级奖励领取标记 1可以领取， 0不可领取， -1已领取
function ShenYuProbeData:GetLevelFlag(level)
    if TableIsEmpty(self.role_data) then
        return 0
    end
    local flag_list = self.role_data.reward_flag or {}
    if flag_list[level] == 1 then
        return -1
    end
    local cur_level = self:GetLevel()
    if cur_level >= level then
        return 1 
    end
    return 0
end


function ShenYuProbeData:SetTabType(tab_type)
    self.tab_data.tab_type = tab_type
end
function ShenYuProbeData:GetTabType()
    return self.tab_data.tab_type
end

-- 获取任务完成了多少次
function ShenYuProbeData:GetTaskTimes(task_type)
    local data = self.role_data.reward_times or {}
    return data[task_type] or 0
end

-- 获取任务刷新时间
function ShenYuProbeData:GetTaskFlushTime(task_type)
    local data = self.camp_data.flush_time or {}
    return data[task_type] or 0
end

-- 获取个人目标完成次数
function ShenYuProbeData:GetRoleTargetNum(task_type)
    local data = self.role_data.goals or {}
    return data[task_type] or 0
end

-- 获取神域目标完成次数
function ShenYuProbeData:GetSYTargetNum(task_type)
    local data = self.role_data.gs_goals or {}
    return data[task_type] or 0
end

-- 任务奖励还剩多少次
function ShenYuProbeData:GetTaskRewardRemain(task_type)
    local cfg = self:GetTaskCfgByTaskType(task_type)
    local max_count = cfg.task_number_max
    local count = self:GetTaskTimes(task_type)
    return max_count - count
end

-- 任务奖励状态 0不可领取，1已领取 -1为已领取
function ShenYuProbeData:GetTaskRewardFlag(task_type)
    if TableIsEmpty(self.role_data) then
        return 0
    end
    return self.role_data.reward_flags[task_type] or 0
end


-- ===================== boss专区 ==========================

function ShenYuProbeData:GetBossData(boss_id)
    return self.boss_data[boss_id] or {}
end

function ShenYuProbeData:SetSelBossId(id)
    self.sel_boss_id = id
end
function ShenYuProbeData:GetSelBossId()
    return self.sel_boss_id
end

-- boss是否死亡
function ShenYuProbeData:GetBossIsDie(boss_id)
    local boss_data = self:GetBossData()
    if TableIsEmpty(boss_data) then
        return false
    end
    local remain_hp = boss_data.boss_remain_hp or 0
    return remain_hp <= 0 
end

-- boss是否可上阵
function ShenYuProbeData:GetBossIsCanUpFight(boss_id)
    for i = 1, 2 do
        local help_info = self:GetBossCelHelpInfo(boss_id, i)
        if not help_info.show_type then
            return true
        end
    end
    return false
end

-- boss当前血量以及满血量
function ShenYuProbeData:GetBossHpInfo()
    local boss_data = self:GetBossData() or 0
    local cur_hp = boss_data.boss_remain_hp or 0
    local max_hp = boss_data.boss_max_hp or 1
    return cur_hp, max_hp
end

-- boss协助信息
function ShenYuProbeData:GetBossCelHelpInfo(boss_id,cell_index)
    local info = {
        boss_id = boss_id,
        cell_index = cell_index,
        item = nil,
        vo = nil,
        show_type = nil,
    }
    if TableIsEmpty(self.role_data.boss_helper) then
        return info
    end
    local helper = self.role_data.boss_helper[boss_id] or {}
    if TableIsEmpty(self.role_data.boss_helper) then
        return info
    end
    local data = helper[cell_index]
    if TableIsEmpty(data) or data.id < 1 then
        return info
    end
    info.show_type = data.is_partner == 1 and 2 or 1
    if data.is_partner == 1 then
        info.item = PartnerData.Instance:GetPartnerInfoById(data.id)
    else
        local pet_index = PetData.Instance:GetPetIndexByUniqueId(data.pet_unique_id1, data.pet_unique_id2)
        info.item = PetData.Instance:GetPetInfo(pet_index)
    end
    info.vo = data
    if not info.item then
        info.show_type = nil
        LOG("神域探索下发boss助阵数据异常",data)
    end
    return info
end

-- 获取可上阵的宠物列表
function ShenYuProbeData:GetCanFightPetList()
    local list = {}
    local pet_list = PetData.Instance:GetPetList()
    for i,v in pairs(pet_list) do
        if not self:IsPetFight(v) then
            table.insert(list, {show_type = 1, item = v})
        end
    end
    table.sort(list,function (a,b)
        return a.item:GetScore() > b.item:GetScore()
    end)
    return list
end

-- 宠物是否已经上阵
function ShenYuProbeData:IsPetFight(pet, sys_type)
    sys_type = sys_type or ShenYuSystemHelpType.Probe
    local pet_info_list = self.help_data.pet_info_list or {}
    for i,v in pairs(pet_info_list) do
        if v.sys_type == sys_type and pet.pet_id == v.pet_id and pet.info.pet_unique_id1 == v.pet_unique_id1 and pet.info.pet_unique_id2 == v.pet_unique_id2 then
            return true
        end
    end
    return false
end

-- 伙伴是否已经上阵
function ShenYuProbeData:IsPartnerFight(partner, sys_type)
    sys_type = sys_type or ShenYuSystemHelpType.Probe
    local partner_info_list = self.help_data.partner_info_list or {}
    for i,v in pairs(partner_info_list) do
        if v.sys_type == sys_type and partner.id == v.partner_id then
            return true
        end
    end
    return false
end

-- 获取可上阵的伙伴列表
function ShenYuProbeData:GetCanFightPartnerList()
    local list = {}
    local partner_list = PartnerData.Instance:GetPartnerAllInfoLockSort()
    for i,v in pairs(partner_list) do
        local vo = v.vo
        if vo.is_unlocked == PartnerConfig.LockState.unlock and not self:IsPartnerFight(v) then
            table.insert(list, {show_type = 2, item = v})
        end
    end
    table.sort(list,function (a,b)
        return a.item.vo.cap > b.item.vo.cap
    end)
    return list
end


-- 获取boss上阵信息， 转换成5537协议一样的格式
function ShenYuProbeData:GetBossHelperInfo(id)
    if TableIsEmpty(self.role_data.boss_helper) then
        return {}
    end
    local helper = self.role_data.boss_helper[id] or {}
    if TableIsEmpty(self.role_data.boss_helper) then
        return {}
    end
    local info = {}
    for i,v in pairs(helper) do
        if v.id > 0 then
            if v.is_partner == 1 then
                info["partner_id_" .. i] = v.id
            else
                local pet_index = PetData.Instance:GetPetIndexByUniqueId(v.pet_unique_id1, v.pet_unique_id2)
                if pet_index and pet_index >= 0 then
                    info["pet_index_" .. i] = pet_index
                end
            end
        end
    end
    info.boss_id = id
    return info
end

-- ===============================================
-- ===============================================


-- ============================= 锁妖井专区 ====================

-- 获取任务组 well_type 服务端是0，1，配置是1,2
function ShenYuProbeData:GetWellTaskGroup(well_type)
    well_type = well_type + 1
    return self.well_task_group[well_type] or {}
end

function ShenYuProbeData:GetWellTaskInfo(well_type, index)
    if TableIsEmpty(self.well_view_data) then
        return {}
    end
    if self.well_view_data.well_type ~= well_type then
        return {}
    end
    return self.well_view_data.list[index] or {}
end

function ShenYuProbeData:GetWellPosData(well_type)
    return self.well_pos_data[well_type]
end

-- 锁妖井是否封印完成
function ShenYuProbeData:IsWellFinish(well_type)
    local pos_data = self:GetWellPosData(well_type)
    return pos_data.seal_count >= pos_data.need_count
end


-- ============================================================



-- boss资源
-- 锁妖井资源
function ShenYuProbeData:GetWellResPath()
    local cfg = self:GetTaskCfg(ShenYuProbeView.TabType.CampTask)
    local folder = cfg.route
    local res_ids = Split(cfg.corresponding_model, "|")
    local res_path = string.format("%s/%s.prefab", folder, res_ids[1])
    return res_path
end

-- 矿石资源
function ShenYuProbeData:GetRockResPath()
    local cfg = self:GetTaskCfg(ShenYuProbeView.TabType.RoleTask)
    local folder = cfg.route
    local res_ids = Split(cfg.corresponding_model, "|")
    local res_path = string.format("%s/%s.prefab", folder, res_ids[2])
    return res_path
end
-- 花草资源
function ShenYuProbeData:GetGrasseResPath()
    local cfg = self:GetTaskCfg(ShenYuProbeView.TabType.RoleTask)
    local folder = cfg.route
    local res_ids = Split(cfg.corresponding_model, "|")
    local res_path = string.format("%s/%s.prefab", folder, res_ids[1])
    return res_path
end
-- boss资源
function ShenYuProbeData:GetBossResPath()
    local cfg = self:GetTaskCfg(ShenYuProbeView.TabType.SYTask)
    local folder = cfg.route
    local res_ids = Split(cfg.corresponding_model, "|")
    local res_path = string.format("%s/%s.prefab", folder, res_ids[1])
    return res_path
end


-- =============== 红点逻辑 ===============

-- 红点通知
function ShenYuProbeData:RemindNotice()
    self.remind_data.v = not self.remind_data.v
end

function ShenYuProbeData:GetAllRemind()
    for tab_type = 0 , 3 do
        local red_num = self:GetTabRemind(tab_type)
        if red_num > 0 then
            return 1
        end
    end
    return 0
end

function ShenYuProbeData:GetTabRemind(tab_type)
    if tab_type == ShenYuProbeView.TabType.Level then
        return self:GetAllLevelReawrdRemind()
    else
        local task_type = self:GetTaskType(tab_type)
        local flag = self:GetTaskRewardFlag(task_type)
        return flag > 0 and 1 or 0
    end
end

-- 全部的等级奖励红点
function ShenYuProbeData:GetAllLevelReawrdRemind()
    local list_data = self.cfg.explore_level
    for i,v in pairs(list_data) do
        local flag = self:GetLevelFlag(v.level)
        if flag > 0 then
            return 1
        end
    end
    return 0
end