-- =========== 幻兽实例 ==========
-- ** vo 有变化需要重新new一个
HuanShou = HuanShou or BaseClass()

function HuanShou.Decode()
    local t = {}
    t.id = MsgAdapter.ReadShort()
    t.level = MsgAdapter.ReadUShort()
    t.new_skill_grid_idx = MsgAdapter.ReadShort()   -- 打书的数据暂存
    t.new_skill_id = MsgAdapter.ReadUShort()
    t.book_skill_id_list = {}
    for i = 0, MAX_HUAN_SHOU_SKILL_GRID_CUR_NUM - 1 do
        t.book_skill_id_list[i] = MsgAdapter.ReadUShort()
    end

    t.star_level_list = {}
    local point_level_count = MsgAdapter.ReadChar()     -- //幻兽潜能点位列表数量
    for i = 0, point_level_count - 1 do
        t.star_level_list[i] = MsgAdapter.ReadUChar()
    end

    -- 幻兽装备等级
    t.equip_level = {}
    for i = 0, 1 do
        t.equip_level[i] = MsgAdapter.ReadShort()
    end
    return t
end

function HuanShou:__init(vo)
    self.vo = vo or {}
    self.id = vo.id or vo.huanshou_id
    self.level = vo.level or 0
end

function HuanShou:Vo()
    return self.vo
end

function HuanShou:Cfg()
    if not self.cfg then
        self.cfg = HuanShouData.Instance:GetCfgById(self.id)
    end
    return self.cfg
end

function HuanShou:IsActive()
    return self.level > 0
end

-- 是否附身了
function HuanShou:IsFuShen()
    local info = HuanShouData.Instance.other_data
    if TableIsEmpty(info) then
        return false
    end
    return self.id == info.fushen_id
end

function HuanShou:LevelCfg()
    local level = self:Level()
    if level <= 0 then
        level = 1
    end
    local cfg = HuanShouData.Instance:GetLevelCfg(self.id, level)
    return cfg
end

-- 下级配置
function HuanShou:NextLevelCfg()
    if self:IsMaxLevel() then
        return self:LevelCfg()
    end
    local level = self:Level() + 1
    local cfg = HuanShouData.Instance:GetLevelCfg(self.id, level)
    return cfg
end

function HuanShou:Qua()
    local cfg = self:Cfg()
    return cfg.huanshou_quality
end

function HuanShou:Name()
    local cfg = self.cfg
    return cfg.huanshou_name
end

function HuanShou.GetQuaName(id,shallow)
    local cfg = HuanShouData.Instance:GetCfgById(id)
    local qua = cfg.huanshou_quality
    local name = cfg.huanshou_name
    local color_t = shallow and QualityColorStrShallow or QualityColorStr
    local color = color_t[qua]
    return ColorStr(name, color),name,color
end

function HuanShou:ResId()
    local cfg = self:Cfg()
    return cfg.shift_card_res
end

function HuanShou:Level()
    return self.level
end

function HuanShou:MaxLevel()
    if not self.max_level then
        local list = HuanShouData.Instance:GetLevelGroupById(self.id)
        self.max_level = table.nums(list)
    end
    return self.max_level
end

function HuanShou:IsMaxLevel()
    return self:Level() >= self:MaxLevel()
end

function HuanShou:AttriList()
    local level_cfg = self:LevelCfg()
    local list = HuanShouData.Instance:GetAttriListByLevelCfg(level_cfg)
    return list
end

-- 下级属性
function HuanShou:NextAttriList()
    local level_cfg = self:NextLevelCfg()
    local list = HuanShouData.Instance:GetAttriListByLevelCfg(level_cfg)
    return list
end

function HuanShou:ShowAddAttriList()
    local cur_attri_list = self:AttriList()
    local next_attri_list = self:NextAttriList()
    local list = {}
    local is_active = self:IsActive()
    local is_max = self:IsMaxLevel()
    for i,v in pairs(cur_attri_list) do
        local attri = TableCopy(v)
        attri.next_value = 0
        attri.is_active = is_active
        attri.is_max = is_max
        for _,v2 in pairs(next_attri_list) do
            if attri.attri_type == v2.attri_type then
                attri.next_value = v2.attri_value
                break
            end
        end
        table.insert(list, attri)
    end
    return list
end

-- 专属技能
function HuanShou:ExSkill()
    if not self.ex_skill then
        local level_cfg = self:LevelCfg()
        local skill_id = level_cfg.skill_id
        if not skill_id or skill_id <= 0 then
            return
        end
        self.ex_skill = HuanShouSkill.New(HuanShouData.SkillType.Exclusive, skill_id, level_cfg.skill_level)
    end
    self.ex_skill.is_lock = not self:IsActive()
    return self.ex_skill
end

function HuanShou:NextSkill()
    if not self.next_ex_skill then
        local level_cfg = self:NextLevelCfg()
        local skill_id = level_cfg.skill_id
        if not skill_id or skill_id <= 0 then
            return
        end
        self.next_ex_skill = HuanShouSkill.New(HuanShouData.SkillType.Exclusive, skill_id, level_cfg.skill_level)
    end
    return self.next_ex_skill
end

-- 幻兽被动技能
function HuanShou:BookSkillList()
    if not self.book_skill_list then
        self.book_skill_list = {}
        local skill_type_list = {}
        local skill_list = self.vo.book_skill_id_list or {}
        local skill_cell_count = self:Cfg().skill_lattice
        for i = 0, skill_cell_count - 1 do
            local skill_id = skill_list[i] or 0
            local skill = HuanShouSkill.New(HuanShouData.SkillType.Book, skill_id, 1, i)
            self.book_skill_list[i] = skill
            if i <= 1 then
                skill.not_lock_lock = true
                skill.is_add_effect = true
            end
            if skill_id > 0 then
                local cfg = Cfg.HuanShouSkillBookCfg(skill_id)
                local skill_type = cfg.skill_type
                local qua = skill:Qua()
                if not skill_type_list[skill_type] or qua > skill_type_list[skill_type] then
                    skill_type_list[skill_type] = qua
                end
            end
        end
        for i,v in pairs(self.book_skill_list) do
            if v.skill_id > 0 then
                local cfg = Cfg.HuanShouSkillBookCfg(v.skill_id)
                local skill_type = cfg.skill_type
                local qua = v:Qua()
                if qua < skill_type_list[skill_type] then
                    v.useless = true
                end
            end
        end
    end
    return self.book_skill_list
end

-- 是否没有被动技能
function HuanShou:IsNotBookSkill()
    local skill_list = self.vo.book_skill_id_list or {}
    if skill_list[0] == nil or skill_list[0] < 1 then
        return true
    end
    return false
end

-- 是否有新的未保存的技能
function HuanShou:IsNewBookSkill()
    local new_index = self.vo.new_skill_grid_idx or -1
    local new_skill_id = self.vo.new_skill_id or -1
    return new_index >= 0 and new_skill_id > 0
end

function HuanShou:NewBookSkillList()
    if not self:IsNewBookSkill() then
        return {}
    end
    local list = {}
    local skill_type_list = {}
    local skill_list = self.vo.book_skill_id_list or {}
    local skill_cell_count = self:Cfg().skill_lattice
    for i = 0, skill_cell_count - 1 do
        local skill_id = skill_list[i] or 0
        local is_new = false
        if i == self.vo.new_skill_grid_idx then
            skill_id = self.vo.new_skill_id
            is_new = true
        end
        local skill = HuanShouSkill.New(HuanShouData.SkillType.Book, skill_id, 1, i)
        skill.is_new = is_new
        list[i] = skill

        if skill_id > 0 then
            local cfg = Cfg.HuanShouSkillBookCfg(skill_id)
            local skill_type = cfg.skill_type
            local qua = skill:Qua()
            if not skill_type_list[skill_type] or qua > skill_type_list[skill_type] then
                skill_type_list[skill_type] = qua
            end
        end
    end

    for i,v in pairs(list) do
        if v.skill_id > 0 then
            local cfg = Cfg.HuanShouSkillBookCfg(v.skill_id)
            local skill_type = cfg.skill_type
            local qua = v:Qua()
            if qua < skill_type_list[skill_type] then
                v.useless = true
            end
        end
    end
    return list
end

-- 下一级幻兽 当满级时，下级是自己
function HuanShou:NextObj()
    if not self.next_obj then
        local max_level = self:MaxLevel()
        local level = self:Level() + 1
        if level > max_level then
            return self
        end
        local vo = TableCopy(self.vo)
        vo.level = level
        self.next_obj = HuanShou.New(vo)
    end
    return self.next_obj
end

-- 获取天赋点的等级
function HuanShou:GetStarLevel(id)
    if TableIsEmpty(self.vo.star_level_list) then
        return 0
    end
    return self.vo.star_level_list[id] or 0
end

function HuanShou:StuffList()
    if self:IsMaxLevel() then
        return {}
    end
    local cfg = self:NextLevelCfg()
    return cfg.cost_item
end

-- 天赋列表
function HuanShou:StarList()
    if not self.star_node_list then
        self.star_node_list = {}
        local list_data = Config.huanshou_auto.potential_point
        for i,v in pairs(list_data) do
            local star_node = HuanShouStar.New(v.potential_point, self)
            table.insert(self.star_node_list, star_node)
        end
    end
    return self.star_node_list
end

-- 天赋全部属性加成
function HuanShou:StarAttriList()
    local attri_list = {}
    local star_list = self:StarList()
    local t = {}
    for i,v in pairs(star_list) do
        if v:IsActive() then
            local attri = v:Attri()
            if not TableIsEmpty(attri) then
                if not t[attri.attri_type] then
                    t[attri.attri_type] = 0
                end
                t[attri.attri_type] = t[attri.attri_type] + attri.attri_value
            end
        end
    end
    for i,v in pairs(t) do
        table.insert(attri_list,{attri_type = i, attri_value = v})
    end
    table.sort(attri_list, function (a,b)
        return a.attri_type < b.attri_type
    end)
    return attri_list
end

-- 是否限定
function HuanShou:IsLimit()
    local cfg = self:Cfg()
    return cfg.is_limit == 1
end

function HuanShou:EquipList()
    if not self.equip_list then
        self.equip_list = {}
        local level_info = self.vo.equip_level or {[0] = 0,[1] = 0}
        if level_info then
            for i, v in pairs(level_info) do
                self.equip_list[i] = HuanShouEquip.New(i, v, self:IsActive())
            end
        end
    end
    return self.equip_list
end

function HuanShou:GetEquip(equip_type)
    local equip_list = self:EquipList()
    return equip_list[equip_type]
end




-- 幻兽技能
HuanShouSkill = HuanShouSkill or BaseClass()
function HuanShouSkill:__init(skill_type, skill_id, skill_level, index)
    self.skill_type = skill_type
    self.skill_id = skill_id
    self.level = skill_level or 1
    self.index = index or -1
end

function HuanShouSkill:Cfg()
    if not self.cfg then
        self.cfg = HuanShouData.Instance:GetSkillCfg(self.skill_type, self.skill_id, self.level)
    end
    return self.cfg
end

function HuanShouSkill:ItemId()
    if self.skill_type == HuanShouData.SkillType.Book then
        local cfg = Cfg.HuanShouSkillBookCfg(self.skill_id)
        if cfg then
            return cfg.skill_item
        else
            return 0
        end
    else
        return 0
    end
end

function HuanShouSkill:Name()
    local cfg = self:Cfg()
    return cfg.skill_name
end

function HuanShouSkill:Icon()
    local cfg = self:Cfg()
    return cfg.skill_icon
end

function HuanShouSkill:Desc()
    local cfg = self:Cfg()
    return cfg.skill_dec
end

function HuanShouSkill:Level()
    return self.level
end

function HuanShouSkill:Qua()
    local cfg = self:Cfg()
    return cfg.skill_quality
end




-- 幻兽的天赋点
HuanShouStar = HuanShouStar or BaseClass()
function HuanShouStar:__init(id, huanshou)
    self.id = id
    self.huanshou = huanshou or HuanShouData.Instance:GetSelHuanShou()
end

-- 0 是属性 1是技能
function HuanShouStar:Type()
    local cfg = self:FirstCfg()
    return cfg.tianfu_type
end


function HuanShouStar:IsActive()
    local level = self:Level()
    return level > 0
end

function HuanShouStar:LevelGroup()
    local cfg = self.huanshou:Cfg()
    return cfg.potential_group or 0
end

function HuanShouStar:Level()
    local level = self.huanshou:GetStarLevel(self.id)
    return level or 0
end

function HuanShouStar:MaxLevel()
    return Config.huanshou_auto.other[1].potential_max or 10
end

function HuanShouStar:IsMax()
    local level = self:Level()
    local max_level = self:MaxLevel()
    return level >= max_level
end

function HuanShouStar:BaseCfg()
    if not self.cfg then
        self.cfg = Cfg.HuanShouStarCfg(self.id)
    end
    return self.cfg
end

function HuanShouStar:FirstCfg()
    local group_id = self:LevelGroup()
    return Cfg.HuanShouStarLevelCfg(group_id, self.id, 1)
end

function HuanShouStar:LevelCfg()
    local level = self:Level()
    if level == 0 then
        return self:FirstCfg()
    end
    local group_id = self:LevelGroup()
    return Cfg.HuanShouStarLevelCfg(group_id, self.id, level)
end

function HuanShouStar:NextLevelCfg()
    if self:IsMax() then
        return self:LevelCfg()
    else
        local level = self:Level() + 1
        local group_id = self:LevelGroup()
        return Cfg.HuanShouStarLevelCfg(group_id, self.id, level)
    end
end

function HuanShouStar:Attri()
    local cfg = self:LevelCfg()
    local attri = HuanShouData.Instance:GetStarAttriListByCfg(cfg)
    return attri
end

function HuanShouStar:NextAttri()
    local cfg = self:NextLevelCfg()
    local attri = HuanShouData.Instance:GetStarAttriListByCfg(cfg)
    return attri
end

function HuanShouStar:ShowAddAttri()
    local cur_attri = self:AttriList()
    local next_attri = self:NextAttriList()
    local attri = TableCopy(cur_attri)
    attri.next_value = 0
    if attri.attri_type == next_attri.attri_type then
        attri.next_value = next_attri.attri_value
    end
    return attri
end

function HuanShouStar:BG()
    -- local is_active = self:IsActive()
    -- local img = is_active and "ShuxingdiJihuo" or "XingWeiDi"
    -- return img
end

function HuanShouStar:AttriIcon()
    local attri = self:Attri()
    return "Attri" .. (attri.attri_type or 0)
end

function HuanShouStar:Name()
    -- local cfg = self:FirstCfg()
    -- return cfg.star_name or ""
    return ""
end

-- 是否能激活天赋
function HuanShouStar:IsCanActive()
    if not self.huanshou:IsActive() then
        return false
    end
    local cfg = self:BaseCfg()
    if cfg.condition == 0 then
        return true
    end
    if self:Level() > 0 then
        return true
    end
    local adjoin_list = {}
    local t = Split(cfg.adjoin, "|")
    for i,v in pairs(t) do
        local seq = tonumber(v)
        local level = self.huanshou:GetStarLevel(seq)
        if level >= cfg.condition then
            return true
        end
    end
    return false
end

function HuanShouStar:StuffList()
    if self:IsMax() then
        return {}
    end
    local cfg = self:NextLevelCfg()
    return cfg.cost_item
end

function HuanShouStar:GetCtrlFlag()
    return HuanShouData.Instance:GetStarUpFlag(star)
end

function HuanShouStar:Condition()
    local cfg = self:BaseCfg()
    local condition = cfg.condition
    return condition
end

function HuanShouStar:Skill()
    local cfg = self:LevelCfg()
    if cfg.tianfu_type == 0 then
        return nil
    end
    local skill = HuanShouSkill.New(HuanShouData.SkillType.Star, cfg.param1, cfg.param2)
    return skill
end

function HuanShouStar:NextSkill()
    local cfg = self:NextLevelCfg()
    if cfg.tianfu_type == 0 then
        return nil
    end
    local skill = HuanShouSkill.New(HuanShouData.SkillType.Star, cfg.param1, cfg.param2)
    return skill
end


-- 幻兽装备
HuanShouEquip = HuanShouEquip or BaseClass()
function HuanShouEquip:__init(type, level, is_active)
    self.equip_type = type
    self.level = level
    self.is_active = is_active or false
end

function HuanShouEquip:IsActive()
    return self.is_active
end

function HuanShouEquip:Cfg()
    return Cfg.HuanShouEquipCfg(self.equip_type, self.level)
end

function HuanShouEquip:Type()
    return self.equip_type
end

function HuanShouEquip:Level()
    return self.level
end

function HuanShouEquip:MaxLevel()
    if not self.max_level then
        self.max_level = 1
        for i,v in pairs(Config.huanshou_auto.huanshou_equip) do
            if v.type == self.equip_type and v.level > self.max_level then
                self.max_level = v.level
            end
        end
    end
    return self.max_level
end

function HuanShouEquip:IsMax()
    return self.level >= self:MaxLevel()
end

function HuanShouEquip:ItemId()
    local cfg = Cfg.HuanShouEquipCfg(self.equip_type, 1)
    return cfg.item_id
end

function HuanShouEquip:GetAttriListByCfg(cfg)
    local attri_list = {}
    for i = 1, 3 do
        local value = cfg["attr_value_" .. i]
        if value and value > 0 then
            local t = {}
            t.attri_type = cfg["att_type_" .. i]
            t.attri_value = value
            table.insert(attri_list, t)
        end
    end
    return attri_list
end

function HuanShouEquip:AttriList()
    if not self.attri_list then
        if self.level == 0 then
            self.attri_list = self:NextAttriList()
        else
            local cfg = self:Cfg()
            self.attri_list = self:GetAttriListByCfg(cfg)
        end
    end
    return self.attri_list
end

function HuanShouEquip:NextAttriList()
    if self:IsMax() then
        return self:AttriList()
    else
        local cfg = Cfg.HuanShouEquipCfg(self.equip_type, self.level + 1)
        return self:GetAttriListByCfg(cfg)
    end
end

-- 1可装备或升级，0材料不足，-1 已满级, -2幻兽未激活
function HuanShouEquip:CtrlFlag()
    if not self:IsActive() then
        return -2
    end
    if self:IsMax() then
        return -1
    end
    local cfg = self:Cfg()
    local has_num = Item.GetNum(cfg.item_id)
    local need_num = cfg.num
    if has_num >= need_num then
        return 1
    else
        return 0
    end
end
