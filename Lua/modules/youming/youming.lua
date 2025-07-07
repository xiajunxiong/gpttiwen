-- 幽冥itemdata
YouMing = YouMing or BaseClass(Item)

-- 幽冥净值解析
function YouMing.DecodeParam(vo)
    local p = {}
    p.has_net_value = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    p.id = MsgAdapter.ReadInt()
    p.awake_level = MsgAdapter.ReadUShort() --觉醒等级
    MsgAdapter.ReadUShort()
    -- 三魂等级列表
    p.soul_level_list = {}
    local level = 0
    for i = 1, 3 do
        p.soul_level_list[i] = {}
        p.soul_level_list[i].type = i - 1
        p.soul_level_list[i].level = MsgAdapter.ReadUShort()
        p.soul_level_list[i].id = vo.item_id
        level = level + p.soul_level_list[i].level
    end
    vo.level = level
    -- LogDG("三魂等级列表", p.soul_level_list)
    -- 七魄等级列表
    p.spirits_level_list = {}
    for i = 1, 7 do
        p.spirits_level_list[i] = {}
        p.spirits_level_list[i].type = i - 1
        p.spirits_level_list[i].level = MsgAdapter.ReadUShort()
        p.spirits_level_list[i].id = vo.item_id
    end
    -- LogDG("七魄等级列表", p.spirits_level_list)
    p.spe_score = MsgAdapter.ReadInt()  --幽冥评分
    p.score = MsgAdapter.ReadInt()--特殊评分 调换了一下，客户端只显示特殊评分
    MsgAdapter.ReadInt()
    -- 展示属性
    p.show_attri_list = {}
    for i = 1, 7 do
        p.show_attri_list[i] = {
            attri_type = YouMingCfg.ShowAttriType[i],
            attri_value = MsgAdapter.ReadInt()
        }
    end

    -- 天资列表
    p.aptitude_list = {}
    p.aptitude = 0
    -- p.aptitude_list2 = {}

    for i = 1, YouMingCfg.ADD_POINT_TYPE_MAX do
        local t = {}
        t.attri_type = YouMingCfg.Aptitude2[i]
        t.attri_value = MsgAdapter.ReadUShort()
        p.aptitude_list[i] = t
        p.aptitude = p.aptitude + t.attri_value
        -- p.aptitude_list2[Language.Role.AddPointTypeKey[i - 1]] = t.attri_value
    end
    MsgAdapter.ReadUShort()

    -- 改造信息列表
    p.reform_list = {}
    for i = 1,YouMingCfg.MAX_NETHER_WORLD_REFORM_GROWTH_NUM do
        local info = {}
        MsgAdapter.ReadLL()
        -- MsgAdapter.ReadShort()
        MsgAdapter.ReadChar()
        info.is_reform_flag = MsgAdapter.ReadChar() --是否改造过 0：未改造过
        -- LogDG("改造", i, "是否没改造", info.is_reform_flag == 0)
        info.reform_add_aptitude = MsgAdapter.ReadUShort()     -- 使用果实移出的自由天资点
        -- LOG("改造", i, info.reform_add_aptitude)
        info.aptitude = 0
        -- 改造给的天资列表
        info.reform_aptitude_list = {}
        for i = 1, YouMingCfg.ADD_POINT_TYPE_MAX do
            local t = {}
            t.attri_type = YouMingCfg.Aptitude2[i]
            t.attri_value = MsgAdapter.ReadUShort()
            info.reform_aptitude_list[i] = t
            info.aptitude = info.aptitude + t.attri_value
        end
        -- LogDG("改造", i,"的改造属性", info.reform_aptitude_list)
        -- 重塑暂存的天资列表
        info.remodeling_list = {}
        for i = 1, YouMingCfg.ADD_POINT_TYPE_MAX do
            local t = {}
            t.attri_type = YouMingCfg.Aptitude2[i]
            t.attri_value = MsgAdapter.ReadUShort()
            info.remodeling_list[i] = t
        end
        -- LOG("改造", i,"重塑属性", info.remodeling_list)

        -- 天赋列表
        info.passive_list = {}
        for j = 1, YouMingCfg.MAX_NETHER_WORLD_REFORM_PASSIVE_NUM do
            local t = {}
            t.skill_id = MsgAdapter.ReadShort()
            t.skill_level = MsgAdapter.ReadUChar()
            t.index = j
            t.change_id = i
            MsgAdapter.ReadChar()
            info.passive_list[j] = t
        end
        -- LogDG("改造", i,"改造天赋列表",info.passive_list)
        p.reform_list[i] = info
    end

    -- 临时改造信息
    p.temporary_reform = {}
    MsgAdapter.ReadLL()
    p.temporary_reform.is_has_info = MsgAdapter.ReadChar()  -- 是否有改造信息可保存
    p.temporary_reform.reform_index = MsgAdapter.ReadChar() -- 第几改
    -- 改造的天资列表
    p.temporary_reform.reform_aptitude_list = {}
    for i = 1, YouMingCfg.ADD_POINT_TYPE_MAX  do
        local t = {}
        t.attri_type = YouMingCfg.Aptitude2[i]
        t.attri_value = MsgAdapter.ReadUShort()
        p.temporary_reform.reform_aptitude_list[i] = t--MsgAdapter.ReadUShort()
    end    
    -- 天赋列表
    p.temporary_reform.passive_list = {}
    for i = 1, YouMingCfg.MAX_NETHER_WORLD_REFORM_PASSIVE_NUM do
        local t = {}
        t.skill_id = MsgAdapter.ReadShort()
        t.skill_level = MsgAdapter.ReadUChar()
        t.change_id = p.temporary_reform.reform_index + 1
        t.index = i
        MsgAdapter.ReadChar()
        p.temporary_reform.passive_list[i] = t
    end
    -- LogDG("临时改造", p.temporary_reform)

    -- 穿戴装备列表
    p.equip_list = {}
    for i = 1, YouMingCfg.MAX_NETHER_WORLD_EQUIP_GRID_TYPE_NUM do
        local info = {}
        info.in_bag = false
        info.index = i
        info.item_id = MsgAdapter.ReadInt()
        info.enqiue_id_1 = MsgAdapter.ReadUInt()
        info.enqiue_id_2 = MsgAdapter.ReadUInt()
        info.param = {}
        info.param.attr_type_sort = MsgAdapter.ReadShort()    -- 道具归属类型
        MsgAdapter.ReadShort()
        info.param.level = MsgAdapter.ReadUShort()
        info.param.str_level = MsgAdapter.ReadUShort()        -- 强化等级
        info.param.cur_exp = MsgAdapter.ReadLL()              -- 当前经验值
        info.param.attr_entry_list = {}   -- 副词条列表
        for j = 1, YouMingCfg.MAX_NETHER_WORLD_EQUIP_ATTR_AFFIX_NUM do
            local t = {}
            t.attri_type = MsgAdapter.ReadShort()
            t.attri_value = MsgAdapter.ReadShort()
            info.param.attr_entry_list[j] = t
        end
        p.equip_list[i] = YouMingEquip.New(info)
    end
    -- LOG("冥装信息", p.equip_list)
    return p
end
function YouMing.DecodeEquipParam(vo)
    local p = {}
    p.has_net_value = MsgAdapter.ReadShort()
    p.attr_type_sort = MsgAdapter.ReadShort()
    MsgAdapter.ReadInt()
    p.level = MsgAdapter.ReadUShort()
    p.str_level = MsgAdapter.ReadUShort()
    p.cur_exp = MsgAdapter.ReadLL()
    p.attr_entry_list = {}   -- 副词条列表
    for j = 1, YouMingCfg.MAX_NETHER_WORLD_EQUIP_ATTR_AFFIX_NUM do
        local t = {}
        t.attri_type = MsgAdapter.ReadShort()
        t.attri_value = MsgAdapter.ReadShort()
        p.attr_entry_list[j] = t
    end
    return p
end 

function YouMing:__init(vo)
    vo = vo or {}
    self.vo = vo
    self.param = vo.param or {}
    self.id = vo.id or vo.item_id
    self.item_id = self.id
    self.index = vo.index or -1
    self.level = self.param.level or 1
end

function YouMing:Cfg()
    return YouMingData.Instance:GetYouMingCfg(self.id)
end

function YouMing:Name()
    local cfg = self:Cfg()
    if DEBUG_MODE then
        return Format("%s[%s]", cfg.name, self.index) 
    else
        return cfg.name
    end
end

function YouMing:Level()
    if self.param and self.param.soul_level_list then
        local level = 0
        for i, v in ipairs(self.param.soul_level_list) do
            level = level + v.level
        end
        self.level = level
    end
    return self.level
end

function YouMing:Score()
    return self.param.score or self:Cfg().base_score or 0
end

function YouMing:Qua()
    local cfg = self:Cfg()
    return cfg.quality or 1
end
--定位
function YouMing:Pos()
    local cfg = self:Cfg()
    return cfg.dim_pet_pos
end

-- 附身宠物信息
function YouMing:SetPetIndex(index)
    self.param.pet_index = index
end
function YouMing:GetPetIndex()
    return self.param.pet_index
end
function YouMing:IsBePet()
    return self.param.pet_index ~= nil
end
function YouMing:BePet()
    if self.param.pet_index then
        return PetData.Instance:GetPetInfo(self.param.pet_index)
    end
end

-- 觉醒等级
function YouMing:AwakeLevel()
    return self.param.awake_level or 1
end

-- 天资列表
function YouMing:AptitudeList()
    if self.param.aptitude_list then
        if self.param.reform_list then
            --总改造资质
            for i = 1, YouMingCfg.ADD_POINT_TYPE_MAX do
                self.param.aptitude_list[i].add_value = 0
                for j = 1, YouMingCfg.MAX_NETHER_WORLD_REFORM_GROWTH_NUM do
                    self.param.aptitude_list[i].add_value = self.param.aptitude_list[i].add_value + self.param.reform_list[j].reform_aptitude_list[i].attri_value
                end
            end
        end
        return self.param.aptitude_list
    else
        local cfg = self:Cfg()
        local list = {}
        for i,v in pairs(YouMingCfg.Aptitude) do
            local t = {}
            t.attri_type = v
            t.attri_value = cfg[i]
            table.insert(list, t)
        end
        return list
    end
end
--重塑天资列表
function YouMing:ReQuaAptitudeList()
    if self.param.aptitude_list then
        if self.param.reform_list then
            --总改造资质
            local list = self:AptitudeList()
            local temp_list = {}
            for i = 1, YouMingCfg.ADD_POINT_TYPE_MAX do
                temp_list[i] = {}
                temp_list[i].attri_type = list[i].attri_type
                temp_list[i].attri_value = list[i].attri_value
                temp_list[i].add_value = 0
                for j = 1, YouMingCfg.MAX_NETHER_WORLD_REFORM_GROWTH_NUM do
                    temp_list[i].add_value = temp_list[i].add_value + self.param.reform_list[j].remodeling_list[i].attri_value
                end
            end
            return temp_list
        end
    else
        local cfg = self:Cfg()
        local list = {}
        for i,v in pairs(YouMingCfg.Aptitude) do
            local t = {}
            t.attri_type = v
            t.attri_value = cfg[i]
            table.insert(list, t)
        end
        return list
    end
end


--改造是否开启
function YouMing:IsRemodelLock(change_id)
    if change_id == 1 then
        return false
    end
    return self.param.reform_list[change_id - 1].is_reform_flag == 0
end
--是否改造过
function YouMing:IsRemodeled(change_id)
    return self.param.reform_list[change_id].is_reform_flag ~= 0
end
--未保存天资改造id
function YouMing:IsHasTemp()
    local has = false
    local change_id = 0
    -- LogDG(self.param.temporary_reform)
    if self.param.temporary_reform then
        has = self.param.temporary_reform.is_has_info == 1
        change_id = self.param.temporary_reform.reform_index + 1
    end
    return has, change_id
end

function YouMing:HasPerfectRemodel()
    for k, v in pairs(self.param.reform_list) do
        if v.is_reform_flag ~= 0 and self:RemodelLevel(k) >= 4 then
            return true
        end
    end
    return false
end

--改造评级
function YouMing:RemodelLevel(change_id)
    local level = 1
    if self.param.reform_list
    and self.param.reform_list[change_id] then
        local score = 0
        for i, v in ipairs(self.param.reform_list[change_id].reform_aptitude_list) do
            score = score + v.attri_value * YouMingData.Instance.score_1[change_id]
        end
        score = score + self.param.reform_list[change_id].reform_add_aptitude * YouMingData.Instance.score_1[change_id]
        for i, v in ipairs(self.param.reform_list[change_id].passive_list) do
            if v.skill_id > 0 then
                score = score + YouMingData.Instance.score_2[change_id]
            end
        end
        if score < 60 then
            level = 1
        elseif score < 80 then
            level = 2
        elseif score < 100 then
            level = 3
        else
            level = 4
        end
    end
    return level
end
-- 改造天资列表
function YouMing:ChangeList(change_id)
    if self.param.reform_list
    and self.param.reform_list[change_id] then
        --这里还要判断是否有临时信息
        local has, temp_id = self:IsHasTemp()
        local list = self:AptitudeList()
        local temp_list = {}
        for i = 1, YouMingCfg.ADD_POINT_TYPE_MAX do
            temp_list[i] = {}
            temp_list[i].attri_type = list[i].attri_type
            temp_list[i].attri_value = list[i].attri_value
        end
        for i, v in ipairs(temp_list) do
            if has and temp_id == change_id then
                v.add_value = self.param.temporary_reform.reform_aptitude_list[i].attri_value
            else
                v.add_value = self.param.reform_list[change_id].reform_aptitude_list[i].attri_value
            end
        end
        return temp_list
    else
        local cfg = self:Cfg()
        local list = {}
        for i,v in pairs(YouMingCfg.Aptitude) do
            local t = {}
            t.attri_type = v
            t.attri_value = cfg[i]
            table.insert(list, t)
        end
        return list
    end
end
-- 改造技能数、改造技能总数
function YouMing:ChangeSkillNums(change_id)
    local skill_num, total_num = 0, 0
    if self.param.reform_list then
        for i, v in ipairs(self.param.reform_list) do
            if v.is_reform_flag ~= 0 then
                for i2, v2 in ipairs(v.passive_list) do
                    if v2.skill_id > 0 then
                        skill_num = skill_num + 1                        
                    end
                end
                total_num = total_num + i
            end
        end
    end
    return skill_num, total_num
end

-- 天资值
function YouMing:Aptitude()
    return self.param.aptitude or 0, self:MaxAptitude()
end

function YouMing:MaxAptitude()
    local cfg = self:Cfg()
    local num = cfg.con + cfg.str + cfg.def + cfg.agi + cfg.magic
    return num
end
--天资加成
function YouMing:AptitudeBonus()
    local cur, max = self:Aptitude()
    local offset = max - cur
    return Cfg.YouMingAddBuff(offset, false)
end

function YouMing:SkillList()
    -- if not self.skill_list then
        local cfg = self:Cfg()
        local skill_list = {}
        if not StringIsEmpty(cfg.active_skill) then
            local level = self:AwakeLevel()
            local skill = YouMingSkill.New({skill_id = cfg.active_skill, skill_level = level, skill_type = YouMingCfg.SkillType.Initiative})
            table.insert(skill_list, skill)
        end
        if not StringIsEmpty(cfg.appendage_skill) then
            local level, total_level = self:PossessSkillLevel()
            local skill = YouMingSkill.New({skill_id = cfg.appendage_skill, skill_level = level, skill_type = YouMingCfg.SkillType.Appendage,
            is_unactive = total_level < YouMingCfg.PossessSkillLevel})
            table.insert(skill_list, skill)
        end
        if not StringIsEmpty(cfg.inborn_skill) then
            local skill = YouMingSkill.New({skill_id = cfg.inborn_skill, skill_level = 1, skill_type = YouMingCfg.SkillType.Inborn})
            table.insert(skill_list, skill)
        end
    -- end
    return skill_list
end
function YouMing:InbornSkill()
    local list = self:SkillList()
    if list[3] then
        list[3].skill_level = self:InbornSkillLevel()
    end
    return list[3]
end
function YouMing:InbornSkillLevel()
    local level = 9999
    if self.param and self.param.equip_list then
        for i, v in ipairs(self.param.equip_list) do
            if not v:IsEmpty() then
                level = math.min(level, v:StrLevel())
            end
        end
    end
    level = level == 9999 and 1 or level
    return level
end
function YouMing:PossessSkill()
    local list = self:SkillList()
    return list[2]
end
function YouMing:PossessSkillLevel()
    local level = 1
    if self.param.spirits_level_list then
        for k, v in pairs(self.param.spirits_level_list) do
            level = level + v.level
        end
    end
    local skill_id = self:Cfg().appendage_skill
    local skill_level = 1
    for i, v in ipairs(Config.dim_pet_cfg_auto.possess_skill) do
        if skill_id == v.skill_id and v.spirits_total_level <= level then
            skill_level = math.max(skill_level, v.skill_level)
        end
    end
    -- LogDG(skill_level)
    -- level = level <= 0 and 1 or level
    return skill_level, level
end

function YouMing:ActiveSkill()
    local list = self:SkillList()
    return list[1]
end

function YouMing:AttriList()
    if self.param.attri_list then
        return self.param.attri_list
    end
    return self:CfgAttriList()
end

function YouMing:CfgAttriList()
    local list = {}
    local cfg = self:Cfg()
    for k,v in pairs(AttrTypeForName) do
        local value = cfg[k]
        if value and value > 0 then
            table.insert(list, {attri_type = v, attri_value = value})
        end
    end
    table.sort(list, function (a,b)
        return a.attri_type < b.attri_type
    end)
    return list
end

function YouMing:CurHp()
    local t = YouMingData.Instance:GetAttrInfo(self.index)
    return t[BattleAttriType.CurHp] or 0
end
function YouMing:CurMp()
    local t = YouMingData.Instance:GetAttrInfo(self.index)
    return t[BattleAttriType.CurMp] or 0
end
function YouMing:MaxHp()
    local t = YouMingData.Instance:GetAttrInfo(self.index)
    return t[BattleAttriType.MaxHp] or 0
end
function YouMing:MaxMp()
    local t = YouMingData.Instance:GetAttrInfo(self.index)
    return t[BattleAttriType.MaxMp] or 0
end

function YouMing:ResId()
    local cfg = self:Cfg()
    return cfg.res_id
end
function YouMing:ResPath()
    local res_id = self:ResId()
    return DrawerHelper.GetNpcPath(res_id)
end
-- 加点属性
function YouMing:Point()
    -- local t = PetData.ConvertPoint(self.param.aptitude_list2)
    -- t = DataHelper.FormAttrList(t, true)
    -- return t
end
function YouMing:BaseAttrPoint()
    local attr = YouMingData.Instance:GetAttrInfo(self.index)
    local t = YouMing.BaseAttr(attr)
    t = DataHelper.FormAttrList(t, true)
    return t
end
function YouMing:DetailAttrPoint()
    local attr = YouMingData.Instance:GetAttrInfo(self.index)
    local t = YouMing.DetailAttr(attr)
    t = DataHelper.FormAttrList(t, true)
    return t
end
--除了血蓝外基础属性
function YouMing.BaseAttr(t)
    local res = {}
    res.gongji = t[BattleAttriType.Attack] or 0--t.gongji or 0
    res.fangyu = t[BattleAttriType.Defense] or 0--t.fangyu or 0
    res.minjie = t[BattleAttriType.Agility] or 0--t.minjie or 0
    res.jingshen = t[BattleAttriType.Mental] or 0--t.jingshen or 0
    res.huifu = t[BattleAttriType.Recovery] or 0--t.huifu or 0
    return res
end
--属性详情
function YouMing.DetailAttr(attr)
    local t = {}
    t.bisha = attr[BattleAttriType.Critical] or 0 -- t.bisha or 0
    t.bisha_dmg = attr[BattleAttriType.CriticalIncValue] or 0 -- t.bisha_dmg or 0
    t.jianren = attr[BattleAttriType.CriticalDecValue] or 0 -- t.jianren or 0
    t.fanji = attr[BattleAttriType.CounterAttack] or 0 -- t.fanji or 0
    t.mingzhong = attr[BattleAttriType.Hit] or 0 -- t.mingzhong or 0
    t.shanduo = attr[BattleAttriType.Dodge] or 0 -- t.shanduo or 0
    t.def_zhongdu = attr[BattleAttriType.AntiPosition] or 0 -- t.def_zhongdu or 0
    t.def_shihua = attr[BattleAttriType.AntiPetrify] or 0 -- t.def_shihua or 0
    t.def_hunshui = attr[BattleAttriType.AntiSleep] or 0 -- t.def_hunshui or 0
    t.def_jiuzui = attr[BattleAttriType.AntiDrunk] or 0 -- t.def_jiuzui or 0
    t.def_hunluan = attr[BattleAttriType.AntiChaos] or 0 -- t.def_hunluan or 0
    t.mogong = attr[BattleAttriType.MagicAttack] or 0 -- t.mogong or 0
    t.mokang = attr[BattleAttriType.MagicDefense] or 0 -- t.mokang or 0
    return t
end
--三魂列表
function YouMing:SanHunList()
    return self.param.soul_level_list
end
function YouMing:SanHunLevelChange(index, level)
    self.param.soul_level_list[index].level = level
    local level = 0
    for i, v in ipairs(self.param.soul_level_list) do
        level = level + v.level
    end
    self.level = level
end
--获取三魂是否为解锁
function YouMing:SanHunIsLock(index)
    if index == 0 then
        return false
    end
    local pre_level = Config.dim_pet_cfg_auto.other[1].pre_soul_level
    return self.param.soul_level_list[index].level < pre_level
end
--七魄列表
function YouMing:QiPoList()
    return self.param.spirits_level_list
end
function YouMing:QiPoLevelChange(type, level)
    self.param.spirits_level_list[type].level = level
    local skill = self:PossessSkill()
    -- local total_level = 1
    -- skill.skill_level, total_level = self:PossessSkillLevel()
    -- skill.is_unactive = total_level < YouMingCfg.PossessSkillLevel
end
function YouMing:AwakeLevelChange(level)
    self.param.awake_level = level
    local skill = self:ActiveSkill()
    skill.skill_level = level
end
--改造天资点
function YouMing:RemodelZiZhiCount(change_id)
    return self.param.reform_list[change_id].aptitude
end
--改造天资总点
function YouMing:TotalRemodelCount()
    local total = 0
    for i, v in ipairs(self.param.reform_list) do
        total = total + v.aptitude
        total = total + v.reform_add_aptitude
    end
    return total
end
--當前改造技能列表
function YouMing:CurSkillList()
    local list = {}
    for i = 1, YouMingCfg.MAX_NETHER_WORLD_REFORM_GROWTH_NUM do
        if self.param.reform_list[i].is_reform_flag ~= 0 then
            for i2 = 1, i do
                --这里要判断已解锁才能存进去
                local skill = YouMingRemodelSkill.New(self.param.reform_list[i].passive_list[i2], self)
                table.insert(list, skill)
            end
        end
    end
    return list
end
--预览改造技能列表
function YouMing:PreviewSkillList(change_id)
    local list = {}
    local has, temp_id = self:IsHasTemp()
    for i = 1, YouMingCfg.MAX_NETHER_WORLD_REFORM_GROWTH_NUM do
        if i <= change_id then
            if has and i == temp_id and temp_id == change_id then
                for i2 = 1, i do
                    --这里要判断已解锁才能存进去
                    local skill = YouMingRemodelSkill.New(self.param.temporary_reform.passive_list[i2], self)
                    table.insert(list, skill)
                end
            else
                for i2 = 1, i do
                    --这里要判断已解锁才能存进去
                    local skill = YouMingRemodelSkill.New(self.param.reform_list[i].passive_list[i2], self)
                    table.insert(list, skill)
                end
            end
        end
    end
    return list
end
--改造技能列表
function YouMing:ChangeSkillList(change_id)
    local list = {}
    local has, temp_id = self:IsHasTemp()
    for i = 1, YouMingCfg.MAX_NETHER_WORLD_REFORM_GROWTH_NUM do
        if i == change_id then
            for i2 = 1, i do
                --这里要判断已解锁才能存进去
                local skill = YouMingRemodelSkill.New(self.param.reform_list[i].passive_list[i2], self)
                table.insert(list, skill)
            end
        end
    end
    return list
end
--已经改造次数
function YouMing:RemodelCount()
    local count = 0
    if self.param.reform_list then
        for i = 1, YouMingCfg.MAX_NETHER_WORLD_REFORM_GROWTH_NUM do
            if self.param.reform_list[i].is_reform_flag ~= 0 then
                count = count + 1
            end
        end
    end
    return count
end
--可自由分配点数
function YouMing:FreePoint()
    local count = 0
    if self.param.reform_list then
        for i = 1, YouMingCfg.MAX_NETHER_WORLD_REFORM_GROWTH_NUM do
            count = count + self.param.reform_list[i].reform_add_aptitude
        end
    end
    return count
end
--重塑信息是否存在
function YouMing:IsHasReshape()
    if self.param.reform_list then
        for i = 1, YouMingCfg.MAX_NETHER_WORLD_REFORM_GROWTH_NUM do
            for i2, v in ipairs(self.param.reform_list[i].remodeling_list) do
                if v.attri_value > 0 then
                    return true
                end
            end 
        end
    end
    return false
end


--装备列表
function YouMing:EquipList()
    return self.param.equip_list
end
--装备列表变化
function YouMing:EquipListChange(index, equip)
    self.param.equip_list[index] = equip
    self:InbornSkill()
end
function YouMing:GetEquip(index)
    return self.param.equip_list[index]
end
--三魂可升级红点
function YouMing:SanHunRemind()
    local config, next_config, num, need_num
    if self.param and self.param.soul_level_list then
        for i, v in ipairs(self.param.soul_level_list) do
            if v.level < YouMingCfg.SanHunLevel then
                --检查是否有材料
                config = Cfg.YouMingSanHunCfg(v.id, v.type, v.level, false)
                if config then
                    next_config = Cfg.YouMingSanHunCfg(v.id, v.type, v.level + 1, false)
                    num = Item.GetNum(config.levelup_consume)
                    need_num = next_config.levelup_consume_num
                    if num >= need_num then
                        return 1
                    end
                end
            end
        end
    end
    return 0
end
--七魄可升级红点
function YouMing:QiPoRemind()
    local config, next_config, num, need_num
    if self.param and self.param.spirits_level_list then
        for i, v in ipairs(self.param.spirits_level_list) do
            if v.level < YouMingCfg.QiPoLevel then
                config = Cfg.YouMingQiPoCfg(v.id, v.type, v.level, false)
                if config then
                    next_config = Cfg.YouMingQiPoCfg(v.id, v.type, v.level + 1, false)
                    num = Item.GetNum(config.levelup_consume)
                    need_num = next_config.levelup_consume_num
                    if num >= need_num then
                        return 1
                    end
                end
            end
        end
    end
    return 0
end
--觉醒可升级红点
function YouMing:JueXingRemind()
    local level = self:AwakeLevel()
    if level >= YouMingCfg.MaxAwakenLevel then
        return 0
    end
    local config = Cfg.YouMingJueXing(self.id, level, false)
    local next_config = Cfg.YouMingJueXing(self.id, level + 1, false)
    if next_config then
        -- 觉醒消耗本体
        local list = YouMingData.Instance:GetJueXingCostList()
        local num = table.nums(list)
        local need_num = next_config.cost_self
        local item = next_config.strengthen_consume_item[0]
        --本体喝消耗物数量
        if num >= need_num and Item.GetNum(item.item_id) >= item.num then
            return 1
        end
    end
    return 0
end

--改造1 幽冥改造材料足够且评分不到"完美"时，对应标签页面上加上红点
function YouMing:GaiZao1Remind()
    --5改
    local num = self:Cfg().change_times
    local level, list
    for i = 1, num do
        level = self:RemodelLevel(i)
        if level ~= 4 then
            --没有完美 且有材料 
            list = YouMingData.Instance:RemodelBooks(i)
            if table.nums(list) > 0 then
                return 1
            end
        end
    end
    return 0
end
-- 冥装可穿戴 
function YouMing:MingZhuangRemind()
   local list = self:EquipList()
   local num = 0
   if list then
        for i, v in ipairs(list) do
            if v.item_id == 0 then
                --没穿
                num = self:IsHasMingZhuang(i)
                if num == 1 then
                    return 1
                end
            else
                num = self:IsHasHighMingZhuang(v)
                if num == 1 then
                    return 1
                end
                --有高品质 品质高红点 同品质判断等级
            end
        end
   end
   return 0
end
--是否有装备
function YouMing:IsHasMingZhuang(index)
    local list = YouMingData.Instance:GetEquipBagList()
    local new_list = {}
    for k, v in pairs(list) do
        if v:Cfg().hole == (index - 1) then
            return 1
        end
    end
    return 0
end
--是否有高级
function YouMing:IsHasHighMingZhuang(equip)
    --index - 1 = type
    local equip_type = equip.index - 1
    local list = YouMingData.Instance:GetEquipBagList()
    local new_list = {}
    for k, v in pairs(list) do
        if v:Cfg().hole == equip_type then
            if v.item_id == equip.item_id then
                if v:Level() > equip:Level() then
                    return 1
                end
            else
                if v:Color() > equip:Color() then
                    return 1
                end
            end
        end
    end
    return self:MingZhuangCanQiangHua(equip)
end
--装备可强化
function YouMing:MingZhuangCanQiangHua(equip)
    local cur_config = equip:QHLevelConfig(equip:StrLevel())
    local next_config = equip:QHLevelConfig(equip:StrLevel() + 1)
    if next_config then
        local num, num2, need_num, need_num2 = 0,0,0,0
        local list = {}
        if next_config.cost_self > 0 then
            num = Item.GetNum(equip.item_id)
            need_num2 = next_config.cost_self
        end
        local item_id = next_config.strengthen_consume_item[0].item_id
        num2 = YouMingData.Instance:GetQHEquipNum(item_id)
        need_num2 = next_config.strengthen_consume_item[0].num
        if num >= need_num and num2 >= need_num2 then
            return 1
        end
    else
        return 0
    end
    return 0
end

--幽冥冥装
YouMingEquip = YouMingEquip or BaseClass(Item)

function YouMingEquip:__init(vo)
    vo = vo or {}
    self.vo = vo
    self.param = vo.param or {}
    self.id = vo.id or vo.item_id
    self.item_id = self.id
    self.index = vo.index or 1
    self.level = self.param.level or 1
    self.in_bag = vo.in_bag
end
function YouMingEquip:Compare()
    if self.in_bag == true then
        local index = YouMingData.Instance:GetEquipIndex()
        local youming = YouMingData.Instance:GetSelectYouMing()
        if not TableIsEmpty(youming) and self:Cfg().hole == (index - 1) then
            local equip = youming:GetEquip(index)
            if equip and equip.item_id > 0 then
                return equip
            end
        end
    end
    return nil
end
function YouMingEquip:InfoConfig()
    return {"YouMingEquip1", "YouMingEquip2", "YouMingEquip3", "Desc"}
end
function YouMingEquip:LevelDesc()
    return Format(Language.ItemInfo.ItemTitles.MZLevel, self.level)
end
function YouMingEquip:IsEmpty()
    return self.item_id == 0
end

function YouMingEquip:Cfg()
    return YouMingData.Instance:GetYouMingEquipCfg(self.id)
end

function YouMingEquip:Name()
    local cfg = self:Cfg()
    if cfg == nil then
        return Language.YouMingPractice.MingZhuang[self.index]
    end
    return cfg.name
end

function YouMingEquip:Level()
    return self.level
end
function YouMingEquip:StrLevel()
    return self.param.str_level or 0
end

function YouMingEquip:Score()
    return self.param.score or self:Cfg().base_score or 0
end

function YouMingEquip:Qua()
    local cfg = self:Cfg()
    return cfg.quality or 1
end

function YouMingEquip:CurConfig(level)
    level = level or self.param.level
    --主属性，要先到YouMingEquipLevel2这里搜索看有没有没有
    --再到YouMingEquipLevel获取
    local config = Cfg.YouMingEquipLevel2(self.id, self.param.attr_type_sort, level, false)
    if config then
        return config
    else
        config = Cfg.YouMingEquipLevel(self.id, level, false)
        if config then
            return config
        end
    end
    return config
end
function YouMingEquip:NextConfig()
   return self:CurConfig(self.param.level + 1) 
end
--主要去消耗信息和经验信息
function YouMingEquip:LevelConfig(level)
    level = level or self.param.level
    return Cfg.YouMingEquipLevel(self.id, level, false)
end

function YouMingEquip:CurQHConfig(level)
    level = level or self.param.str_level
    --主属性，要先到YouMingEquipLevel2这里搜索看有没有没有
    --再到YouMingEquipLevel获取
    local config = Cfg.YouMingQiangHua2(self.id, self.param.attr_type_sort, level, false)
    if config then
        return config
    else
        config = Cfg.YouMingQiangHua(self.id, level, false)
        if config then
            return config
        end
    end
    return config
end

function YouMingEquip:NextQHConfig()
    return self:CurQHConfig(self.param.str_level + 1)
end

--主要去消耗信息和经验信息
function YouMingEquip:QHLevelConfig(level)
    level = level or self.param.str_level
    return Cfg.YouMingQiangHua(self.id, level, false)
end

function YouMingEquip:CurExp()
    return self.param.cur_exp
end
function YouMingEquip:MaxExp()
    local config = self:LevelConfig(self.level + 1)
    return config and config.upgrade_exp or 0
end
function YouMingEquip:LevelChange(level, cur_exp)
    self.level = level
    self.param.level = level
    self.param.cur_exp = tonumber(cur_exp)
end
function YouMingEquip:StrLevelChange(level)
    self.param.str_level = level
end

function YouMingEquip:DeputyConfig()
    return self.param.attr_entry_list
end
--[[ function YouMingEquip:Type()
    return ItemType.YouMingEquip
end ]]
-- 幽冥技能item data
YouMingSkill = YouMingSkill or BaseClass()
-- vo = {skill_id,  skill_level, skill_type}
-- skill_type = YouMingCfg.ReqType
function YouMingSkill:__init(vo)
    self.vo = vo
    self.skill_id = vo.skill_id
    self.skill_level = vo.skill_level or 0
    self.skill_type = vo.skill_type
    self.index = vo.index or -1
    self.is_unactive = vo.is_unactive or false
end

function YouMingSkill:Cfg()
    if not self.cfg then
        self.cfg = YouMingData.Instance:GetSkillCfg(self.skill_id, self.skill_level, self.skill_type)
    end
    return self.cfg
end
function YouMingSkill:CurCfg()
    return YouMingData.Instance:GetSkillCfg(self.skill_id, self.skill_level, self.skill_type)
end
function YouMingSkill:NextCfg()
    return YouMingData.Instance:GetSkillCfg(self.skill_id, self.skill_level + 1, self.skill_type)
end
function YouMingSkill:LevelCfg(level)
    return YouMingData.Instance:GetSkillCfg(self.skill_id, level, self.skill_type)
end
function YouMingSkill:Icon()
    local cfg = self:Cfg()
    return cfg.icon
end

function YouMingSkill:Name()
    local cfg = self:Cfg()
    return cfg.name
end

function YouMingSkill:Desc()
    local cfg = self:Cfg()
    return cfg.description
end

--幽冥改造被动技能
YouMingRemodelSkill = YouMingRemodelSkill or BaseClass()
function YouMingRemodelSkill:__init(vo, youming)
    self.vo = vo
    self.skill_id = vo.skill_id
    self.skill_level = vo.skill_level
    self.change_id = vo.change_id or 1
    self.index = vo.index or -1
    self.youming = youming
end
function YouMingRemodelSkill:IsWenHao()
    --没改造显示问号
    return not self.youming:IsRemodeled(self.change_id)
end
function YouMingRemodelSkill:IsLock()
    if self.youming.param.reform_list[self.change_id].is_reform_flag ~= 0 then
        return self.skill_id == 0
    end
    return false
end
function YouMingRemodelSkill:IconType()
    return ICON_TYPE.BUFF
end
function YouMingRemodelSkill:Cfg()
    return SkillData.GetDecConfig(self.skill_id, self.skill_level)--SkillData.GetConfig(self.skill_id, self.skill_level)
end
function YouMingRemodelSkill:DescCfg()
    local config = Cfg.SkillPassive(self.skill_id, false)
    if config then
        config = Cfg.SpecialEffect(config.sp_id, false)
        return config
    end
    return {}
    -- return SkillData.GetDecConfig(self.skill_id, self.skill_level)
end
function YouMingRemodelSkill:Icon()
    return self:DescCfg().icon_id
end
function YouMingRemodelSkill:ChangeId()
    return self.change_id
end
function YouMingRemodelSkill:Name()
    return self:DescCfg().name or ""
end
function YouMingRemodelSkill:Desc()
    local cfg = self:DescCfg()
    return cfg.description or ""
end