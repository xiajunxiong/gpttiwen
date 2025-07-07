PetFeiShengData = PetFeiShengData or BaseClass()
--飞升的前置条件
PetFeiShengData.PreType = {
    PetLv = 1,          --宠物等级
    RoleFlyUpLv = 2,    --角色飞升等级  FlyUpStageType
    PetStrenLv = 3,     --宠物强化等级
    PetScore = 4,       --宠物评分要求
}

--宠物飞升等级
PetFeiShengData.Lv = {
    None = 0,
    One = 1,
    Two = 2,
    Max = 2,    --最大飞升次数
}
PetFeiShengData.PRE_COND_COUNT = 3      --可飞升的前置条件数量
PetFeiShengData.STAGE_COUNT = 3         --飞升所需的任务数
PetFeiShengData.SKILL_COUNT = 3         --飞升技能数

PetFeiShengData.TASK_CONDI_INDEX = 3    --飞升任务作为前置条件索引

function PetFeiShengData:__init()
    if PetFeiShengData.Instance ~= nil then
        Debuger.LogError("[PetFeiShengData] attempt to create singleton twice!")
        return
    end
    PetFeiShengData.Instance = self
    self.feisheng_task_info = SmartData.New({
        pet_unique_id1 = 0,
        pet_unique_id2 = 0,
        today_is_finish = 0,
        task_id = 0,
    })

    self.feisheng_succ_data = SmartData.New({
        pet_index = -1,
        times = PetFeiShengData.Lv.None,
        seled_skill_idx = 0,
        is_change_skill = false,    --决定技能选择界面是否是变更技能
    })
    -- self.attri_cfg_cache = {}
    Cfg.Register("PetFeiShengAttri",Cfg.RealTimeSearch2Key(Config.pet_cfg_auto.fly_att,"pet_id","fly_num"))
    Cfg.Register("PetFeiShengSkillLvup",Cfg.RealTimeSearch2Key(Config.pet_cfg_auto.fly_skill_strengthen,"skill_id","levels"))
    self.skill_max_lv = nil
end

function PetFeiShengData:__delete()
    PetFeiShengData.Instance = nil
end

function PetFeiShengData:SetPetFeiShengTaskInfo(protocol)
    -- LogError("PetFeiShengData:SetPetFeiShengTaskInfo====",protocol)
    local info = protocol.info
    self.feisheng_task_info:Set(info)
end

--对应宠物的飞升模块是是否解锁
--fs_lv全局几级宠物飞升已解锁，目前的条件是主角二段飞升了解锁宠物二段飞升
function PetFeiShengData.ModUnlocked(fs_lv)
    if fs_lv == nil then
        fs_lv = PetFeiShengData.Lv.One
    end
    local need_role_flystage = fs_lv    --这里假设主角飞升等级与宠物飞升等级的值一样
    return FlyUpData.Instance:GetStage() >= need_role_flystage --人物飞升了才能做宠物飞升
end

function PetFeiShengData:Clear()
    self.feisheng_task_info:Set({
        pet_unique_id1 = 0,
        pet_unique_id2 = 0,
        today_is_finish = 0,
        task_id = 0,
    })
end

--re @是否完成(bool) @文字描述(string)
function PetFeiShengData.GetFlyPreInfo(pet_index,pre_i,fs_lv)
    local is_com = false
    local desc = ""
    if pre_i ~= PetFeiShengData.TASK_CONDI_INDEX then
        local cfg = PetFeiShengData.GetPreConditionCfg(pre_i,fs_lv)
        if cfg ~= nil then
            if cfg.fly_type == PetFeiShengData.PreType.PetLv then
                local pet = PetData.Instance:GetPetInfo(pet_index)
                is_com = pet:Level() >= cfg.param1 
            elseif cfg.fly_type == PetFeiShengData.PreType.RoleFlyUpLv then
                is_com = FlyUpData.Instance:GetStage() >= cfg.param1
            elseif cfg.fly_type == PetFeiShengData.PreType.PetStrenLv then
                local pet = PetData.Instance:GetPetInfo(pet_index)
                is_com = pet:StrLevel() >= cfg.param1
            elseif cfg.fly_type == PetFeiShengData.PreType.PetScore then
                local pet = PetData.Instance:GetPetInfo(pet_index)
                is_com = pet:GetScore() >= cfg.param1
            end
            local format_desc = Language.PetFeiSheng.PreDescs[cfg.fly_type]
            if not StringIsEmpty(format_desc) then 
                if cfg.fly_type == PetFeiShengData.PreType.RoleFlyUpLv then
                    desc = string.format(format_desc,Language.FlyUp.Name[cfg.param1] or "")
                else
                    desc = string.format(format_desc,cfg.param1)
                end
            end
        end
    else
        local pet = PetData.Instance:GetPetInfo(pet_index)
        local target_fs_lv = pet:FeiShengTimes() + 1
        is_com = pet:FeiShengJieDuan() / PetFeiShengData.STAGE_COUNT >= target_fs_lv
        local stage_count =  is_com and PetFeiShengData.STAGE_COUNT or (pet:FeiShengJieDuan() % PetFeiShengData.STAGE_COUNT)
        desc = string.format(Language.PetFeiSheng.PreCondi3Desc,
            PetFeiSheng.LvName(target_fs_lv),stage_count,PetFeiShengData.STAGE_COUNT)
    end
    return is_com,desc
end

function PetFeiShengData.IsAllPreCom(pet_index,fs_times)
    
    for i=1,PetFeiShengData.PRE_COND_COUNT do
        local is_pre_com = PetFeiShengData.GetFlyPreInfo(pet_index,i,fs_times)
        -- LogError(cur_pet:Index(),i,fs_times,is_pre_com)
        if is_pre_com == false then
            return false
        end
    end
    return true
end

function PetFeiShengData.GetPreConditionCfg(pre_i,fs_lv)
    local cfg = Config.pet_cfg_auto.pet_fly
    for _,v in ipairs(cfg) do
        if v.fly_num == fs_lv and 
            ((v.seq-1) % PetFeiShengData.PRE_COND_COUNT + 1 == pre_i) then
            return v
        end
    end
    return nil
end
local fs_attri_cfg_cache = {}

function PetFeiShengData.GetFSTaskAttriCfg(pet_id,fs_lv,fs_stage)
    -- pet_id = 50200      ---TEMP_CODE
    local pet_id_tab = fs_attri_cfg_cache[pet_id]
    if pet_id_tab == nil then
        pet_id_tab = {}
        fs_attri_cfg_cache[pet_id] = pet_id_tab
    end
    local fs_lv_tab = pet_id_tab[fs_lv]
    if fs_lv_tab == nil then
        fs_lv_tab = {}
        pet_id_tab[fs_lv] = fs_lv_tab
    end
    local cfg = fs_lv_tab[fs_stage]
    if cfg == nil then
        for _,v in ipairs(Config.pet_cfg_auto.fs_att) do
            if v.fs_pet_id == pet_id and v.fs_num == fs_lv and v.fs_stage == fs_stage then
                cfg = v
                fs_lv_tab[fs_stage] = cfg
                break
            end
        end
    end
    if cfg == nil then
        BuglyReport("PetFeiShengData:GetFSTaskAttriCfg UnFind in Config.pet_cfg_auto.fs_att:",pet_id,fs_lv,fs_stage)
    end
    return cfg
end

--只有二次飞升后的宠物可以强化到+11及以上
function PetFeiShengData.IsStrengthenNeedFS(str_lv,cur_fs_times)
    local max_lv = MAX_PET_STRENGTHEN_LV_NORMAL
    if cur_fs_times == PetFeiShengData.Lv.Two then
        max_lv = MAX_PET_STRENGTHEN_LEVEL
    elseif cur_fs_times == PetFeiShengData.Lv.One then
        max_lv = MAX_PET_STRENGTHEN_FlY_NOE
    end

    return str_lv >= max_lv
end

function PetFeiShengData:HasCurTaskData()
    return self.feisheng_task_info.task_id ~= 0
end

function PetFeiShengData:IsTodayTaskFinished()
    if not self:HasCurTaskData() then
        return false
    end
    return self.feisheng_task_info.today_is_finish ~= 0
end

function PetFeiShengData:CurTaskId()
    return self.feisheng_task_info.task_id
end

function PetFeiShengData:IsInTask(pet)
    return self.feisheng_task_info.pet_unique_id1 == pet:UniqueId1() and 
        self.feisheng_task_info.pet_unique_id2 == pet:UniqueId2()
end

function PetFeiShengData:CurTaskingPet()
    if not self:HasCurTaskData() then return nil end
    local pet_info =PetData.Instance:GetPetInfoByUniqueId(self.feisheng_task_info.pet_unique_id1,self.feisheng_task_info.pet_unique_id2)
    return pet_info
end

function PetFeiShengData:SuccIdx()
    return self.feisheng_succ_data.pet_index
end

function PetFeiShengData:SuccTimes()
    return self.feisheng_succ_data.times
end

function PetFeiShengData:SetSuccData(pet_idx,times)
    self.feisheng_succ_data.pet_index = pet_idx
    self.feisheng_succ_data.times = times
end

function PetFeiShengData:SuccData()
    return self.feisheng_succ_data
end

function PetFeiShengData:SuccSeledSkillIdx()
    return self.feisheng_succ_data.seled_skill_idx
end

--从1开始
function PetFeiShengData:SetSelectSkill(skill_idx)
    self.feisheng_succ_data.seled_skill_idx = skill_idx
end

--保证选择技能的时候sm_data会发生变化
function PetFeiShengData:CleanSeledSkillIdx()
    self.feisheng_succ_data.seled_skill_idx = 0
end

function PetFeiShengData:MarkIsChangeSkill(is_change_sk)
    self.feisheng_succ_data.is_change_skill = is_change_sk
end

function PetFeiShengData:IsChangeSkillSel()
    return self.feisheng_succ_data.is_change_skill
end

function PetFeiShengData.SkillLvupCfg(skill_id,lv,error_report)
    return Cfg.PetFeiShengSkillLvup(skill_id,lv - 1,error_report)   --配置和服务端都是从0开始数的
end

function PetFeiShengData:SkillMaxLv()
    if self.skill_max_lv == nil then
        for i=1,99 do
            local cfg = PetFeiShengData.SkillLvupCfg(7146,i,false)
            if cfg == nil then
                break
            end
            if self.skill_max_lv == nil or cfg.levels + 1 > self.skill_max_lv then
                self.skill_max_lv = cfg.levels + 1
            end
        end
    end
    return self.skill_max_lv
end