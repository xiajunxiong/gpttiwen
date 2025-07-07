SKILL_TYPE = {
    SINGLE = 1,
    POWER = 2,
    SUPER_POWER = 3,
    RANDOM = 4,
    COMBO = 5,
    LINE = 6
}

local id2info_tab = {
    [1] = Cfg.SkillSingle,
    [2] = Cfg.SkillPower,
    [3] = Cfg.SkillSuperPower,
    [4] = Cfg.SkillRandom,
    [5] = Cfg.SkillCombo,
    [6] = Cfg.SkillLine,
    [7] = Cfg.SkillPassive,
    [8] = Cfg.SkillArea,
}

local skill_desc_config = {
    [1] = Cfg.SkillLife,
    [2] = Cfg.SkillDescPet,
    [3] = Cfg.SkillDescLife,
    [4] = Cfg.SkillDescHeart,
    [5] = Cfg.ChangeJobDesc,
    [6] = Cfg.SkillDescPartnerAndRole,
    [7] = Cfg.YouMingSkillDesc,
}

RoleSkillLimitType = {
    None = -1,
    CareerLevel = 0,
    RoleLevel = 1,
    SkillFull = 2,
    SkillRoleLevel = 3,
    PassiveSkill = 4,
}

local SHOW_SKILL_POPUP_LV = 10      --多少等级以上会显示获取新技能的弹框
SKILL_SORT_TYPE = {
    PROF_SKILL = 0,      --职业技能
    COMMON_SKILL = 1,    --通用技能
    ZHUAN_ZHI_SKILL = 2, --专职技能
    CAI_JI_SKILL = 3,    --采集技能
    ASSIST_SKILL = 4,    --辅助技能
}
SkillData = SkillData or BaseClass()

-- 角色技能界面需要显示的被动技能
SkillData.RoleViewShowPassiveSkill = {
    [823] = true,
    [824] = true,
    [7080] = true,
    [7399] = true,
}

function SkillData:__init()
    if SkillData.Instance ~= nil then
        Debuger.LogError("[SkillData] attempt to create singleton twice!")
        return
    end
    SkillData.Instance = self
    self.smart_data = SmartData.New()
    self.change_data = SmartData.New({
        change = false, 
        click_skill_id = 0, 
        skill_page = 1, 
        common_use_change = false, 
        change_level = false, 
        rightType = 1,
        red_dot_flag_change = 1,
        })
    self.role_skill_info_list_slide = false
    self.tab = 1
    self.life_skill_list = {}
    self.pre_click_id = 0
    self.cur_list = false
    self.cur_click_list = 1

    self.new_skill_popup = {}
    self.unlock_skill = nil
    self.careerProfSkillList = {}
    SkillData.audit_skill_index = {}
    local job_type_skill_count = {}
    for k,v in pairs(Config.career_advancements_auto.career_skill_list) do
        self.careerProfSkillList[v.job_type] = self.careerProfSkillList[v.job_type] or {}
        self.careerProfSkillList[v.job_type][v.sort_type] = self.careerProfSkillList[v.job_type][v.sort_type] or {}
        job_type_skill_count[v.job_type] =job_type_skill_count[v.job_type] or {count = 1}
        v.cur_list = v.sort_type + 1
        if SkillData.RoleViewShowPassiveSkill[v.skill_id] then
            v.index2 = 0
        else
            v.index2 = 99
        end
        SkillData.audit_skill_index[v.job_type] = SkillData.audit_skill_index[v.job_type] or {}
        if v.skill_type == 0 then
            SkillData.audit_skill_index[v.job_type][v.skill_id] = job_type_skill_count[v.job_type].count
            job_type_skill_count[v.job_type].count = job_type_skill_count[v.job_type].count + 1
        end
        table.insert(self.careerProfSkillList[v.job_type][v.sort_type],v)
    end
    self.view_data = SmartData.New({
        drag_select = nil,
        drop_select = nil,
    })
    self.skill_num = 0
    self.skill_list = {}
    self.skill_idx_list = {}

    self.prof_view_skill_info = SmartData.New({
		pos =  Vector3.New(0,0,0),
		onclick = false,
		tips_str = "",
	})
end

function SkillData:Clear()
    self.smart_data:Set({})
    self.tab = 1
    self.life_skill_list = {}
    self.pre_click_id = 0
    self.cur_list = false
    self.cur_click_list = 1

    self.new_skill_popup = {}
    self.unlock_skill = nil

    self.skill_num = 0
    self.skill_list = {}
    self.skill_idx_list = {}
end

function SkillData:__delete()
    if SkillData.Instance == self then
        SkillData.Instance = nil
    end
end

-- 激活红点
function SkillData:CacuRemindCount()
    local config = Config.career_advancements_auto.career_skill_list
    local profession = RoleData.Instance:ProfType()
    local level = RoleData.Instance:GetRoleLevel()
    local has_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    for k,v in pairs(config) do
        if (v.sort_type == 0 or v.sort_type == 1) and v.job_type == profession and level >= v.skill_learn_level then
            local skill_cfg = SkillData.GetConfig(v.skill_id,1) or {}
            -- 策划说激活不消耗铜币
            if self:getSkillDataById(v.skill_id) == nil then--and skill_cfg.coin_consume ~= nil and has_coin >= skill_cfg.coin_consume then
                return 1
            end
        end
    end
    return 0
end

-- 升级红点
function SkillData:SkillCanUpgradeRemind()
    local config = Config.career_advancements_auto.career_skill_list
    local profession = RoleData.Instance:ProfType()
    local level = RoleData.Instance:GetRoleLevel()
    local has_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    for k,v in pairs(config) do
        if (v.sort_type == 0 or v.sort_type == 1) and v.job_type == profession and level >= v.skill_learn_level then
            local skill_data = self:getSkillDataById(v.skill_id)
            if skill_data and not self:GetSkillLimit(v.skill_id) then
                local skill_cfg = SkillData.GetConfig(v.skill_id,skill_data.skill_level + 1) or {}
                if skill_cfg.coin_consume ~= nil and has_coin >= skill_cfg.coin_consume then
                    return 1
                end
            end
        end
    end
    return 0
end

function SkillData:SetClickSkillId(skill_id)
    self.change_data.click_skill_id = skill_id
end

function SkillData:SetSkillPage(index)
    self.tab = index
    self.change_data.skill_page = index
end

function SkillData:SetRightType(rightType)
    self.change_data.rightType = rightType
end

function SkillData:GetClickSkillId()
    return self.change_data.click_skill_id
end

function SkillData:GetLifeSkillDataFromServer(skill_id)
    for k, v in pairs(self.life_skill_list) do
        if v.skill_id == skill_id then
            return v
        end
    end
end

function SkillData:OnSCSkillList(protocol)
    for k,v in pairs(self.smart_data) do
        v.check_flag = false
    end

    local id_list = SkillData.GetProfSkillIdList(RoleData.Instance:ProfType())
    for i, v in ipairs(protocol.skill_list) do
        --客户端做一层筛选，不是本职业的技能不放入技能列表
        if id_list[v.skill_id] ~= nil then
            v.index2 = 99
            self.smart_data[v.index] = v
            self.smart_data[v.index].check_flag = true
        end
    end
    --移除转职之后本地技能数据的残留
    for k,v in pairs(self.smart_data) do
        if not v.check_flag then
            self.smart_data[k] = nil
        end
    end
    for i, v in pairs(protocol.common_use_skill_index_list) do
        if self.smart_data[v] ~= nil then
            self.smart_data[v].index2 = i
        elseif DEBUG_MODE then
            Debuger.LogWarning(string.format("OnSCSkillList %s in protocol.common_use_skill_index_list is not has v.index in protocol.skill_list",v))
        end
    end
    self.skill_list = protocol.skill_list
    self.skill_num = protocol.common_use_skill_num
    self.skill_idx_list = protocol.common_use_skill_index_list
end

function SkillData:SkillInfo(skill_id)
    for key, value in pairs(self.skill_list) do
        if value.skill_id == skill_id then
            return value
        end
    end
end

function SkillData:OnSCSkillChangeNotice(protocol)
    local is_new = true
    local old_index = 1
    for index, value in ipairs(self.skill_list) do
        if value.skill_id == protocol.skill.skill_id then
            is_new = false
            old_index = index
        end
    end
    if is_new then
        table.insert(self.skill_list, protocol.skill)
    else
        self.skill_list[old_index] = protocol.skill
    end

    local skill = protocol.skill
    --升级或者学新
    local index2 = 99
    if self.smart_data[skill.index] == nil then
        self.unlock_skill = skill.skill_id
        if RoleData.Instance:GetRoleLevel() >= SHOW_SKILL_POPUP_LV then
            RoleCtrl.Instance:NewSkillPopup(skill.skill_id)
        end
        self.change_data.change_level = not self.change_data.change_level
    else
        if self.smart_data[skill.index].skill_level ~= skill.skill_level then
            self.change_data.change_level = not self.change_data.change_level
        end
        index2 = self.smart_data[skill.index].index2 or 99
    end
    self.smart_data[skill.index] = skill
    self.smart_data[skill.index].index2 = index2
    self.change_data.change = not self.change_data.change
end

function SkillData:OnSCCommonUseSkillChangeNotice(protocol)
    self.skill_num = protocol.common_use_skill_num
    self.skill_idx_list = protocol.common_use_skill_index_list
    local index_list = protocol.common_use_skill_index_list
    --先重置再赋值
    for k,v in pairs(self.smart_data) do
        v.index2 = 99
    end
    if protocol.common_use_skill_num ~= 0 then
        for k, v in pairs(index_list) do
            if self.smart_data[v] ~= nil then
                self.smart_data[v].index2 = k
            end
        end
    end
    self.change_data.common_use_change = not self.change_data.common_use_change
end

function SkillData:SetLifeSkillList(list)
    local profession = RoleData.Instance:GetRoleProf()
    if profession == 0 then
        self.life_skill_list = list
        return
    end
    for k, v in pairs(list) do
        local t = self.life_skill_list[v.skill_id]
        --初次下发消息时设置配置信息，之后的消息下发，只更新消息体信息
        if t == nil then
            t = {}
            local skillCareerCfg = Cfg.CareerSkillList(profession, v.skill_id)
            if skillCareerCfg ~= nil then
                t.sort_type = skillCareerCfg.sort_type
                t.is_recommend = skillCareerCfg.is_recommend
                t.skill_level_limit = skillCareerCfg.skill_level_limit
                t.skill_learn_level = skillCareerCfg.skill_learn_level
            end
            self.life_skill_list[v.skill_id] = t
        else
            --生活技能升级提示
            if t.skill_level < v.skill_level then
                if RoleData.Instance:GetRoleLevel() >= SHOW_SKILL_POPUP_LV then
                    RoleCtrl.Instance:NewSkillPopup(v.skill_id, true, v.skill_level)
                end
            end
        end
        self.life_skill_list[v.skill_id].skill_id = v.skill_id
        self.life_skill_list[v.skill_id].skill_level = v.skill_level
        self.life_skill_list[v.skill_id].reserve_sh = v.reserve_sh
        self.life_skill_list[v.skill_id].skill_exp = v.skill_exp
        if self.life_skill_list[v.skill_id].skill_level == 0 and self.life_skill_list[v.skill_id].skill_exp == 0 then
            self.life_skill_list[v.skill_id].index2 = self.life_skill_list[v.skill_id].skill_id + 10000
        else
            self.life_skill_list[v.skill_id].index2 = self.life_skill_list[v.skill_id].skill_id
        end
    end
end

--尽量不要调用这个接口，调用下面那个接口
function SkillData:getExpById(skill_id)
    local data = self:getSkillDataById(skill_id)
    if data ~= nil then
        return data.skill_exp
    end
    return nil
end

--获取角色已学习的技能信息
function SkillData:getSkillDataById(skill_id)
    if self.smart_data == nil then
        return nil
    end
    for k, v in pairs(self.smart_data) do
        if v.skill_id == skill_id then
            return v
        end
    end
    if SkillData.RoleViewShowPassiveSkill[skill_id] then
        return {
            skill_level = 1,
            skill_id = skill_id,
            index2 = 0,
        }
    end
end

function SkillData:PushNewSkillPopup(skill_t)
    table.insert(self.new_skill_popup,skill_t)
end

function SkillData:PopNewSkillPopup()
    if #self.new_skill_popup ~= 0 then
        return table.remove(self.new_skill_popup,1)
    end
end

function SkillData:WaitPopupNewSkillCount()
    return #self.new_skill_popup
end

function SkillData:PopSkillUnlock()
    local unlock = self.unlock_skill
    self.unlock_skill = nil
    return unlock
end

function SkillData:CheckSkillLearn()
    local profession = RoleData.Instance:GetRoleProf()
    local level = RoleData.Instance:GetRoleLevel()
    local has_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)

    if profession == 0 then return true end 
    local config = self.careerProfSkillList[profession]
    
    for k,v in pairs(config[0]) do 
        local skill_data = self:getSkillDataById(v.skill_id) or {}

        if not next(skill_data) then 
            local skill_cfg = self.GetConfig(v.skill_id,1) or {}

            if level >= v.skill_learn_level and has_coin >= skill_cfg.bind_coin_consume then 
                return false
            end 
        end 
    end 
    return true
end

--未按id规则来的技能单独做配置表映射逻辑
local id2info_tab_sp_map = {
    [1175] = Cfg.SkillCombo,    --舍身取义从单体改为连击技能，配置放在Combo表里了
    [111] = Cfg.SkillRandom,    --万钧之刃从单体改为随机技能
    [101] = Cfg.SkillCombo,     --天罚从单体改为连击技能
    [134] = Cfg.SkillSuperPower,--净化之咒从单体改为全体技能
    [130] = Cfg.SkillCombo,     --生命汲取从单体改为连击技能
    [1275] = Cfg.SkillCombo,    --生命汲取从单体改为连击技能
    [191] = Cfg.SkillRandom,    --祝融不灭星火从单体改为随机技能
    [193] = Cfg.SkillRandom,    --水月冰冻大地从单体改为随机技能
    [194] = Cfg.SkillRandom,    --方天君飞沙走石从单体改为随机技能
    [1492] = Cfg.SkillRandom,    --藤鞭从单体改为随机技能
    [1498] = Cfg.SkillRandom,    --藤鞭从单体改为随机技能
    [1206] = Cfg.SkillRandom,    --石化巫术从单体改为随机技能
    [1274] = Cfg.SkillRandom,    --石化巫术从单体改为随机技能
    [1178] = Cfg.SkillRandom,    --凝元心经从单体改为随机技能
    [579] = Cfg.SkillRandom,     --黑煌蝶精技能
    [583] = Cfg.SkillRandom,     --一变黑煌蝶精技能
    [588] = Cfg.SkillRandom,     --二变黑煌蝶精技能
    [110] = Cfg.SkillRandom,    --无序之风从单体改为随机技能
    [108] = Cfg.SkillRandom,    --大地之力从单体改为随机技能
    [1253] = Cfg.SkillRandom,    --大地苍龙凝元心经从单体改为随机技能
    [1254] = Cfg.SkillRandom,    --大地苍龙凝元心经从单体改为随机技能
    [1602] = Cfg.SkillRandom,    --大地苍龙凝元心经从单体改为随机技能
    [573]= Cfg.Combo,    --猫妖魅影一击从直线技能改为连击技能
    [574]= Cfg.Combo,    --变异猫妖魅影一击从直线技能改为连击技能
    [337] = Cfg.SkillRandom,    --幻境乱心从超强改为随机技能
    [183]=Cfg.SkillRandom,--医仙儿治愈术从单体改为随机技能
}
function SkillData.idToTable(skill_id)
    local sp_tab = id2info_tab_sp_map[skill_id]
    if sp_tab then
        return sp_tab
    end
    local key = skill_id > 100 and tonumber(string.sub(tostring(skill_id), 1, 1)) or 1
    if skill_id < 1000 and (key == 8 or key == 9) then--被动技能修正
        key = 7
    end
    return id2info_tab[key]
end

function SkillData:SkillSortList()
    if next(self.skill_list) == nil then
        return {}
    end
    local list = {}
    local prof_id_list = SkillData.GetProfSkillIdList(RoleData.Instance:ProfType())
    for _, value in ipairs(self.skill_idx_list) do
        for _, value2 in ipairs(self.skill_list) do
            if value == value2.index and prof_id_list[value2.skill_id] ~= nil then
                local config = SkillData.GetDecConfig(value2.skill_id, 1)
                config.sort_index = value2.index
                table.insert(list, config)
            end
        end
    end
    return list
end

function SkillData:ChangeSortList(key, index, key2)
    if key2 == nil then
        key2 = table.nums(self.skill_idx_list)
    end
    --应该是顺位
    table.remove(self.skill_idx_list, key)
    table.insert(self.skill_idx_list, key2, index)
end

function SkillData:GetSortKey(index)
    for key, value in ipairs(self.skill_idx_list) do
        if index == value then
            return key
        end
    end
end

--技能是否锁定（不可学习）
function SkillData:GetSkillLimit(skill_id)
    local skill_data = self:getSkillDataById(skill_id) or {}
    if skill_data and skill_data.index2 == 0 then
        return true, RoleSkillLimitType.PassiveSkill
    end
    local skill_level = skill_data.skill_level or 0
    local profession = RoleData.Instance:GetRoleProf()
    local career_skill_cfg = Cfg.CareerSkillList(profession, skill_id)
    local limit_str = ""
    local limit_type = -1
    local advance_cfg = RolePromoteData.GetRoleProfConfig()
    --先判断职业id
    if advance_cfg == nil then
        return false
    elseif skill_level >= advance_cfg.max_skill_level then --当前技能等级是否达到当前转职级别的上限
        local cur_prof_stage = RoleData.Instance:ProfStage()
        if career_skill_cfg and career_skill_cfg.skill_level_limit == skill_level then --判断当前转职级别是否已是最高（真·已满级）
            limit_type = RoleSkillLimitType.SkillFull
            limit_str = Language.Role.SkillFull
        elseif cur_prof_stage < ROLE_MAX_PROF_STAGE then --转职不满但是不能继续学了
            local next_cfg = RolePromoteData.GetProfConfig(RoleData.Instance:GetRoleProfId() + 1,RolePromoteData.Instance:GetProfGrade())
            -- limit_str = string.format(Language.Role.CanLearn, Language.DataHelper.Word[cur_prof_stage])
            limit_str = string.format(Language.Role.CanLearnNext, next_cfg and next_cfg.job_name or (Language.DataHelper.Word[cur_prof_stage]..Language.Role.CanLearnLevel))
            limit_type = RoleSkillLimitType.CareerLevel
        end
        return true, limit_type, limit_str
    end
    local role_level = RoleData.Instance:GetRoleLevel()
    if career_skill_cfg == nil then
        return false
    elseif career_skill_cfg.skill_level_limit <= skill_level then
        return true, RoleSkillLimitType.SkillFull, Language.Role.SkillFull
    elseif career_skill_cfg.skill_learn_level > role_level then
        return true, RoleSkillLimitType.RoleLevel, career_skill_cfg.skill_learn_level
    end
    local skill_cfg = SkillData.GetConfig(skill_id, skill_level + 1)
    if skill_cfg and skill_cfg.skill_learn_level ~= nil and skill_cfg.skill_learn_level > role_level then
        return true, RoleSkillLimitType.SkillRoleLevel, string.format(Language.Role.RoleLevelNotEnoughTocontinue, skill_cfg.skill_learn_level)
    end
    return false
end

-- 技能从几级升到几级需要消耗多少铜币
function SkillData:GetSkillLvUpCoin(skill_id, from, to)
    local coin = 0
    if not skill_id or from >= to then
        return coin
    end
    local now_from = from + 1
    while now_from <= to do
        -- 从n级升到n+1级需要拿n+1级的配置的铜币
        local cfg = SkillData.GetConfig(skill_id, now_from) or {}
        coin = coin + cfg.coin_consume or 0
        now_from = now_from + 1
    end
    return coin
end


function SkillData:OperateRoleProfViewSkill(data)
    self.prof_view_skill_info.pos = data.pos_info or Vector3.zero
    self.prof_view_skill_info.height = data.height or 0
    self.prof_view_skill_info.onclick = data.onclick or false
    self.prof_view_skill_info.tips_str = data.tips_str or ""
end

function SkillData:GetProfSkillCanMaxLevel()
    local skill_data = self:getSkillDataById(self:GetClickSkillId())
    local skill_level = skill_data ~= nil and skill_data.skill_level or 0
    local skill_exp = skill_data ~= nil and skill_data.skill_exp or 0
    local advance_cfg = RolePromoteData.GetRoleProfConfig()
    local left_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local cost_coin = 0
    for i = skill_level, advance_cfg.max_skill_level do
        local skill_next_level_cfg = SkillData.GetConfig(self:GetClickSkillId(), i + 1)
        if skill_next_level_cfg ~= nil and skill_next_level_cfg.skill_learn_level <= RoleData.Instance:GetRoleLevel() then
            local need_exp = skill_next_level_cfg.up_level_need_exp
            if i == skill_level then
                need_exp = skill_next_level_cfg.up_level_need_exp - skill_exp
            end
            if left_coin < skill_next_level_cfg.bind_coin_consume * need_exp / skill_next_level_cfg.add_exp or i == advance_cfg.max_skill_level then
                return i, cost_coin
            else
                left_coin = left_coin - skill_next_level_cfg.bind_coin_consume * math.ceil(need_exp / skill_next_level_cfg.add_exp)
                cost_coin = cost_coin + skill_next_level_cfg.bind_coin_consume * math.ceil(need_exp / skill_next_level_cfg.add_exp)
            end
        else
            return i, cost_coin
        end
    end
end

--获取职业技能id列表
function SkillData.GetProfSkillIdList(prof)
    local list = {}
    for i,v in ipairs(Config.career_advancements_auto.career_skill_list) do
        if v.job_type == prof then
            local config = SkillData.GetDecConfig(v.skill_id,1)
            if config ~= nil and config.show_name ~= nil then
                list[v.skill_id] = config
            end
        end
    end
    return list
end

--获取职业技能列表
function SkillData.GetProfSkillList(prof)
    local list = {}
    for i,v in ipairs(Config.career_advancements_auto.career_skill_list) do
        if v.job_type == prof then
            local config = SkillData.GetDecConfig(v.skill_id,1)
            if config ~= nil and config.show_name ~= nil then
                table.insert(list,config)
            end
        end
    end
    return list
end

--获取技能配置
function SkillData.GetConfig(id, level)
    if id == nil then
        if DEBUG_MODE then
            Debuger.LogErrorSD("SkillData.GetConfig ERROR, id=nil")
        end
        return nil
    end
    level = level or 1
    local tab = SkillData.idToTable(id)
    if tab == nil then
        Debuger.LogErrorSD("SkillData.GetConfig ERROR,skill_tab Get FAILED.id=%s,level=%s", id, level)
        return nil
    end
    local re = tab(id,level,false)
    if re == nil then
        if DEBUG_MODE then
            Debuger.LogErrorSD("SkillData.GetConfig ERROR,skill_tab level not exist.id=%s,level=%s", id, level)
        end
    end
    return re
end

--从1到level的配置
function SkillData.GetConfigUntil(id,level)
    if id == nil then
        Debuger.LogErrorSD("SkillData.GetConfigUntil ERROR, id=nil")
        return nil
    end
    level = level or 1
    local tab = SkillData.idToTable(id)
    if tab == nil then
        Debuger.LogErrorSD("SkillData.GetConfigUntil ERROR,skill_tab Get FAILED.id=%s,level=%s", id, level)
        return nil
    end
    local res = {}
    for i = 1,level do
        local re = tab(id, i, false)
        table.insert(res, re)
    end
    if next(res) == nil then
        Debuger.LogErrorSD("SkillData.GetConfigUntil ERROR,skill_tab level not exist.id=%s,level=%s", id, level)
    end
    return res
end

function SkillData:GetInbornSkillDesc(skill_id, skill_level)
    for k, v in pairs(Config.skill_description_auto.inborn_skill) do
        if v.skill_id == skill_id and v.skill_level == skill_level then
            return v
        end
    end
    LogD("技能描述表未找到灵脉技能配置  skill_id:" .. skill_id .. "   skill_level:" .. skill_level)
    return nil
end

--获取技能类型
function SkillData.GetSkillType(skill_id)
    if skill_id and skill_id >= 1000 then
        return math.floor(skill_id / 1000)
    end
    return math.floor(skill_id / 100)
end

--根据技能id和skill_level获取技能描述配置
function SkillData.GetDecConfig(id, level)
    return SkillData.Instance:GetSkillDescConfig(id, level)
end

--根据技能id和skill_level获取技能描述配置
function SkillData:GetSkillDescConfig(skill_id,skill_level)
    for k,func in pairs(skill_desc_config) do
        local config = func(skill_id,skill_level or 1,false)
        if config ~= nil then
            return config
        end
    end
end

--根据技能id和skill_level获取技能描述技能名字
function SkillData:GetSkillDescName(skill_id,skill_level)
    local co = self:GetSkillDescConfig(skill_id,skill_level)
    return co and co.skill_name or ""
end

function SkillData:GetSpecialGuideSkillCanNotUp()
    local skill_id = GuideData.Instance:GetSkillId()
    return self:GetSkillLimit(skill_id)
end

function SkillData.GetAuditSkillIndex(skill_id)
    return SkillData.audit_skill_index[RoleData.Instance:ProfType()][skill_id]
end
