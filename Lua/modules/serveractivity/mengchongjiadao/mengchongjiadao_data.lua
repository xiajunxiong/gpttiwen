MengChongJiaDaoData = MengChongJiaDaoData or BaseClass()
MengChongJiaDaoTaskType = {
    TYPE_BEGIN = 0,
    TYPE_LOGIN = 1, --登录
    TYPE_ACTIVE = 7 --活跃度
}

function MengChongJiaDaoData:__init()
    if MengChongJiaDaoData.Instance ~= nil then
        Debuger.LogError("[MengChongJiaDaoData] attempt to create singleton twice!")
        return
    end
    MengChongJiaDaoData.Instance = self
    self.act_info = SmartData.New({})
    self.battle_info = SmartData.New({})
    self.remind_info = SmartData.New({watch_id = -1})
end

function MengChongJiaDaoData:__delete()
    MengChongJiaDaoData.Instance = nil
end

function MengChongJiaDaoData:ClearData()
    self.remind_info.watch_id = -1
end

function MengChongJiaDaoData:Config()
    return Config.mengchongjiadao_auto
end
function MengChongJiaDaoData:OtherConfig()
    return self:Config().other[1]
end

function MengChongJiaDaoData:GetActInfo()
    return self.act_info
end

function MengChongJiaDaoData:GetRewardList()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_MENG_CHONG_JIA_DAO, self:Config().reward, "seq")
end

function MengChongJiaDaoData:GetTodayTaskList()
    local act_day = TimeCtrl.Instance:GetActOpenDay(ACTIVITY_TYPE.RAND_MENG_CHONG_JIA_DAO)
    local show_list = {}
    for i, v in pairs(self:Config().task) do
        if v.section == act_day then
            table.insert(show_list, v)
        end
    end
    table.sort(show_list, DataHelper.SortFunc("seq1"))
    return show_list
end

function MengChongJiaDaoData:GetMonsterCfg(monster_id)
    return self:Config().basics[monster_id]
end

function MengChongJiaDaoData:SetMengChongInfo(protocol)
    self.act_info.pet_combat_num = protocol.pet_combat_num
    self.act_info.today_fraction = protocol.today_fraction
    self.act_info.task_param_list = protocol.task_param_list
    self.act_info.task_status = protocol.task_status

    self.remind_info:Notify()
end

function MengChongJiaDaoData:SetBattleInfo(protocol)
    self.battle_info.is_challenge_status = protocol.is_challenge_status
    self.battle_info.challenge_status = protocol.challenge_status
    self.battle_info.monster_id = self:CheckMonsterId()

    self.remind_info:Notify()
end

function MengChongJiaDaoData:CheckMonsterId()
    if self.battle_info.challenge_status == nil then
        return 1
    end
    local challenge_status = self.battle_info.challenge_status
    for i = 1, #challenge_status do
        if challenge_status[i] == 0 then
            return i
        end
    end
    return #challenge_status + 1
end

function MengChongJiaDaoData:GetCurMonsterId()
    return self.battle_info.monster_id or 1
end

function MengChongJiaDaoData:GetPKResult()
    return self.battle_info.is_challenge_status or 0
end

--获取任务进度
function MengChongJiaDaoData:GetTaskProg(task_type)
    if self.act_info.task_param_list then
        return self.act_info.task_param_list[task_type] or 0
    end
    return 0
end

--任务是否完成
function MengChongJiaDaoData:GetTaskStatus(seq1)
    if self.act_info.task_status then
        return self.act_info.task_status[seq1] or 0
    end
    return 0
end

--获取该活动总天数
function MengChongJiaDaoData:GetDayNumber()
    return self:Config().task[#self:Config().task].section
end
function MengChongJiaDaoData:GetDayList()
    local day_list = {}
    for i = 1, self:GetDayNumber() do
        table.insert(day_list, {day = i})
    end
    return day_list
end

function MengChongJiaDaoData:GetDayCombat(day)
    if self.act_info.today_fraction then
        return self.act_info.today_fraction[day - 1] or 0
    end
    return 0
end

--是否所有都打败了
function MengChongJiaDaoData:IsAllDefeat()
    return self:GetMonsterCfg(self:GetCurMonsterId()) == nil
end

function MengChongJiaDaoData:GetRemindNum()
    --可挑战时出现一次红点，玩家点进来看之后消失
    if not self:IsAllDefeat() then
        local monster_cfg = self:GetCurMonsterCfg()
        local cur_combat = self.act_info.pet_combat_num or 1
        if cur_combat > monster_cfg.combat and cur_combat ~= self.remind_info.watch_id then
            return 1
        end
    end
    return 0
end

function MengChongJiaDaoData:SaveWatchId()
    self.remind_info.watch_id = self.act_info.pet_combat_num or -1
end

function MengChongJiaDaoData:GetCurMonsterCfg()
    if self:IsAllDefeat() then
        return self:Config().basics[#self:Config().basics]
    end
    return self:GetMonsterCfg(self:GetCurMonsterId())
end

function MengChongJiaDaoData:GetTotalCombat()
    return self.act_info.pet_combat_num or 1
end

function MengChongJiaDaoData:IsBattle()
    return self.is_battle or false
end
function MengChongJiaDaoData:SetIsBattle(is_battle)
    self.is_battle = is_battle
end

function MengChongJiaDaoData:GetRewardByMonsterId(monster_id)
    local monster_cfg = self:GetMonsterCfg(monster_id)
    if monster_cfg == nil then
        return
    end
    for i, v in pairs(self:GetRewardList()) do
        if v.item_id == monster_cfg.reward then
            return v
        end
    end
end
