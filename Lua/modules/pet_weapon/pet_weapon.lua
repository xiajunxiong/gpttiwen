PetWeaponType = {
    Common = 1,     -- 普武，全部宠物可用
    Only = 2,       -- 专武，特定宠物可用
}

-- ============= 宠物魂器 ============
-- 有属性修改应当重新实例化魂器

PetWeapon = PetWeapon or BaseClass(Item)

function PetWeapon:__init(vo)
    --LogError("魂器PetWeapon VO:: ",vo)
    vo = vo or {}
    self.index = vo.index or -1
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or 1
    self.num = vo.num or 1
    self.param = vo.param or {}
    self.is_grey = vo.is_grey 
    self.in_bag = vo.in_bag == nil and true or vo.in_bag
    self.weapon_idx = vo.weapon_idx or 0
    self.pet = vo.pet
end

function PetWeapon:Seq()
    local base_cfg = self:BaseCfg()
    return base_cfg.seq
end

-- 在哪个宠物身上，可用于判断是否穿戴了
function PetWeapon:PetIndex()
    return self.param.pet_index
end

-- 魂器在宠物身上的位置 0,1,默认为0，1只有炼妖才有
function PetWeapon:GridPos(value)
    if value then
        self.weapon_idx = value
    end
    return self.weapon_idx or 0
end

function PetWeapon:SetPet(pet)
    self.pet = pet
end

function PetWeapon:GetPet()
    return self.pet
end

function PetWeapon:IsLianYao()
    local pet = self.pet or PetData.Instance:GetPetInfo(self:PetIndex())
    if pet then
        return pet:IsLianYao()
    end
    return false
end

-- 使用者的宠物id
function PetWeapon:PetId()
    return self.param.use_pet_id or 0
end
function PetWeapon:YaoPetId()
    local pet = self.pet or PetData.Instance:GetPetInfo(self:PetIndex())
    if pet then
        return pet:YaoPetId()
    end
    return 0
end

function PetWeapon:TypeDesc()
    return nil
end

function PetWeapon:LevelDesc()
    return nil
end

-- 获取武器类型 return ：PetWeaponType , pet_id
function PetWeapon:WeaponType()
    return self:BaseCfg().pet_weapon_type, self:BaseCfg().pet_weapon_pet_id
end

-- 品质分类
function PetWeapon:QuaType()
    local base_cfg = self:BaseCfg()
    return base_cfg.quality
end

-- 名字描述
function PetWeapon:NameDesc()
    local name = self:Name()
    return name
end

function PetWeapon:EffectName()
    local weapon_type, pet_id = self:WeaponType()
    if weapon_type == PetWeaponType.Common then
        return self:Name()
    end
    local pet_name = Item.GetName(pet_id)
    return string.format("%s·%s", pet_name, self:Name())
end

-- 突破阶段
function PetWeapon:Step()
    return self.param.step or 0
end

-- 精炼等级
function PetWeapon:RefineLevel()
    return self.param.refine or 0
end

-- 等级
function PetWeapon:Level()
    return self.param.level or 1
end

-- 当前突破阶段最大的等级
function PetWeapon:MaxLimitLevel()
    local break_cfg = self:BreakCfg()
    if TableIsEmpty(break_cfg) then
        return self:BaseCfg().level_max
    end
    return break_cfg.max_level
end

-- 是否满级
function PetWeapon:IsFullLevel()
    return self:Level() >= self:BaseCfg().level_max
end

-- 全部属性
function PetWeapon:AttriList()
    if self.param.attri_list == nil then
        self.param.attri_list = {}
        local attri_group_id = self:BaseCfg().attribute_group or 0
        local attri_group = PetWeaponData.Instance.base_attri_group[attri_group_id]
        if attri_group then
            for i,v in pairs(attri_group) do
                local info = {}
                info.attri_type = v.att_type
                info.attri_value = v.att_max
                self.param.attri_list[info.attri_type] = info
            end
        end
    end
    return self.param.attri_list
end

-- 根据魂器配置里面的顺序显示的属性list
function PetWeapon:AttriListSort()
    if self.attri_list_sort == nil then
        self.attri_list_sort = {}
        local attri_group_id = self:BaseCfg().attribute_group or 0
        local attri_group = PetWeaponData.Instance.base_attri_group[attri_group_id]
        if attri_group then
            local attri_list = self:AttriList()
            for i,v in ipairs(attri_group) do
                local attri = attri_list[v.att_type]
                if attri then
                    table.insert(self.attri_list_sort,attri)
                end
            end
        end
        --self.attri_list_sort = PetWeaponData.Instance:AttriSort(self.attri_list_sort)
    end
    return self.attri_list_sort
end

-- 单个属性
function PetWeapon:Attri(attri_type)
    local attri_list = self:AttriList()
    local attri = attri_list[attri_type]
    return attri
end

-- 溢出的经验
function PetWeapon:Exp()
    if self.param.exp == nil or self.param.exp < 0 then
        return 0
    end
    return self.param.exp
end

-- 当材料值多少经验
function PetWeapon:PriceExp()
    return PetWeaponData.Instance:GetPriceExp(self) or 0
end

-- 升到最高限制需要多少经验
function PetWeapon:UpToMaxNeedExp()
    if self.max_need_exp == nil then
        self.max_need_exp = PetWeaponData.Instance:GetIntensifyNeedExp(self) or 0
    end
    return self.max_need_exp
end

-- 是否有锁
function PetWeapon:IsLock()
    if self.param.is_lock == 1 then
        return true
    else
        return false
    end
end

-- 是否需要突破
function PetWeapon:IsNeedBreak()
    local cfg = self:BreakCfg()
    if not cfg then
        return false
    end
    local level = self:Level()
    if cfg.max_level <= level then
        return true
    end
    return false
end

-- 最大突破等级
function PetWeapon:MaxBreakLevel()
    local qua_type = self:QuaType()
    local break_group = PetWeaponData.Instance.break_group[qua_type] or {}
    return #break_group
end

-- 基础配置
function PetWeapon:BaseCfg()
    local cfg = PetWeapon.GetBaseCfg(self.item_id)
    return cfg
end

-- 等级配置
function PetWeapon:LevelCfg()
    local qua_type = self:QuaType()
    local level = self:Level()
    local cfg = PetWeapon.GetLevelCfg(qua_type, level)
    return cfg
end

-- 下级突破配置
function PetWeapon:BreakCfg()
    local qua_type = self:QuaType()
    local step = self:Step() + 1
    if step > self:MaxBreakLevel() then
        return
    end
    local cfg = PetWeapon.GetBreakCfg(qua_type, step)
    return cfg
end

-- 强化加成配置
function PetWeapon:IntensifyAttriCfg()
    local qua_type = self:QuaType()
    local cfg = PetWeapon.GetIntensifyAttri(qua_type)
    return cfg
end

-- 最高精炼等级
function PetWeapon:MaxRefineLevel()
    local base_cfg = self:BaseCfg()
    local refine_seq = base_cfg.refine_group
    local refine_group = PetWeaponData.Instance.refine_group[refine_seq]
    if refine_group then
        return #refine_group
    end
    return 0
end

-- 精炼配置
function PetWeapon:RefineCfg()
    local seq = self:Seq()
    local refine_level = self:RefineLevel()

    if refine_level == 0 then
        return 
    end
    local cfg = PetWeapon.GetRefineCfg(seq, refine_level)
    return cfg
end

-- 精炼下一级配置
function PetWeapon:NextRefineCfg()
    local seq = self:Seq()
    local refine_level = self:RefineLevel() + 1

    if refine_level > self:MaxRefineLevel() then
        return nil
    end

    local cfg = PetWeapon.GetRefineCfg(seq, refine_level)
    return cfg
end

-- 品质配置
function PetWeapon:QuaCfg()
    local qua_type = self:QuaType()
    local cfg = PetWeapon.GetQuaCfg(qua_type)
    return cfg
end

-- 品质显示成圆点
function PetWeapon:QuaShowPoint()
    if self.qua_str == nil then
        local qua_type = self:QuaType()
        local point_num = self:QuaCfg().num
        local str = ""
        for i=1,point_num do
            str = str .. qua_type
        end
        self.qua_str = str
    end
    return self.qua_str
end

-- 魂器的特效id
function PetWeapon:SpId()
    if self.param.sp_id == nil then
        local sp_group_id = self:BaseCfg().pet_weapon_skill or 0
        local sp_group = PetWeaponData.Instance.skill_effect_group[sp_group_id] or {}
        self.param.sp_id = sp_group.buff_id or 0
    end
    return self.param.sp_id
end

-- 精炼特效配置
function PetWeapon:RefineEffectCfg()
    local cfg = self:RefineCfg()
    local effect_id = self:SpId()
    if cfg then
        effect_id = cfg.skill_buff
    end
    local effect_cfg = Cfg.SpecialEffect(effect_id)
    if not effect_cfg then
        LogError("魂器特效配置不存在，特效id ==== ",effect_id)
    end
    return effect_cfg
end

-- 精炼特效描述
function PetWeapon:RefineEffectDesc()
    local effect_id = self:SpId()
    local desc
    if effect_id < 1 then
        desc = Language.PetWeapon.EffectEmpty
    else
        local effect_cfg = self:RefineEffectCfg() or {}
        desc = effect_cfg.description or ""
    end
    return desc
end

function PetWeapon:RefineEffectAndAttriDesc()
    local desc = self:RefineEffectDesc()
    local desc_2 = self:RefineAttriDesc()
    return desc .. "\n" .. desc_2
end 

function PetWeapon:RefineAttriList()
    local cfg = self:RefineCfg()
    return PetWeapon.GetRefineAttriList(cfg)
end

function PetWeapon:RefineAttriDesc(show_type)
    show_type = show_type or 1
    local attri_list = self:RefineAttriList()
    return PetWeapon.GetRefineAttriDesc(attri_list, show_type)
end

-- 获取圣物描述List
function PetWeapon:GetAllEffectList()
    local list = {}
    local seq = self:Seq()
    local group_cfg = PetWeaponData.Instance.refine_group[seq] or {}
    for i, v in ipairs(group_cfg) do
        local t = {}
        t.equip_level = v.refine
        t.skill_cfg = {}
        t.level_text = string.format(Language.PetWeapon.Step, v.refine)
        
        local skill_cfg = Cfg.SpecialEffect(v.skill_buff) or {}
        local skill_desc = skill_cfg.description or ""
        local attri_list = PetWeapon.GetRefineAttriList(v)
        local attr_desc = PetWeapon.GetRefineAttriDesc(attri_list,2)
        t.skill_cfg.description = skill_desc .. "\n" .. attr_desc
        table.insert(list, t)
    end
    return list
end

-- 专武描述
function PetWeapon:OnelyDesc()
    local weapon_type, pet_id = self:WeaponType()
    if weapon_type == PetWeaponType.Common then
        return ""
    end
    local pet_cfg = Cfg.PetById(pet_id)
    local desc = string.format(Language.PetWeapon.OnelyDesc,Language.Pet.ColorLevel[pet_cfg.quality], pet_cfg.name)
    return desc
end

-- 模型id
function PetWeapon:ModelId()
    return self:BaseCfg().pet_weapon_res or 0
end



-- =============== 静态方法 ============= --
function PetWeapon.DecodeInfo()
    local t = {}
	t.item_unique_id = MsgAdapter.ReadLL()
	t.item_id = MsgAdapter.ReadUShort()
	t.is_bind = MsgAdapter.ReadChar()

    t.param = {}
	t.param.is_lock = MsgAdapter.ReadChar()
	t.param.exp = MsgAdapter.ReadInt()
	t.param.max_exp = MsgAdapter.ReadInt()
	t.param.level = MsgAdapter.ReadShort()
	t.param.step = MsgAdapter.ReadShort()
	t.param.refine = MsgAdapter.ReadInt()

	t.param.sp_id = MsgAdapter.ReadShort()
	t.param.attri_num = MsgAdapter.ReadUShort()
	t.param.attri_list = {}
	for i = 1, 5 do
		local attri = {}
		attri.attri_type = MsgAdapter.ReadShort()
		attri.attri_value = MsgAdapter.ReadShort()
        if attri.attri_value > 0 then
            t.param.attri_list[attri.attri_type] = attri
        end
	end
	return t
end

-- 获取基础配置
function PetWeapon.GetBaseCfg(weapon_item_id)
    local cfg = Cfg.PetWeaponBaseCfg(weapon_item_id)
    return cfg
end

-- 获取等级配置
function PetWeapon.GetLevelCfg(qua_type, level)
    local cfg = Cfg.PetWeaponIntensifyCfg(qua_type, level)
    return cfg
end

-- 获取突破配置
function PetWeapon.GetBreakCfg(qua_type, step)
    local cfg = Cfg.PetWeaponBreakCfg(qua_type, step)
    return cfg
end

-- 获取精炼配置
function PetWeapon.GetRefineCfg(seq, refine_level)
    local cfg = Cfg.PetWeaponRefineCfg(seq, refine_level)
    return cfg
end

-- 获取魂器品质配置
function PetWeapon.GetQuaCfg(qua_type)
    local cfg = Cfg.PetWeaponQuaCfg(qua_type)
    return cfg
end

-- 获取强化后应加的属性配置
function PetWeapon.GetIntensifyAttri(qua_type)
    local cfg = PetWeaponData.Instance:GetIntensifyAttriCfg()
    return cfg[qua_type] or {}
end


function PetWeapon.GetRefineAttriList(cfg)
    if TableIsEmpty(cfg) then
        return {}
    end
    local list = {}
    for i = 1, 2 do
        if cfg["attr_type_" .. i] then
            local attri = {}
            attri.attri_type = cfg["attr_type_" .. i]
            attri.attri_value = cfg["attr_value_" .. i]
            table.insert(list, attri)
        end
    end
    return list
end

function PetWeapon.GetRefineAttriDesc(attri_list, show_type)
    show_type = show_type or 1
    local desc = ""
    for i,v in pairs(attri_list) do
        local attri_info = CommonStruct.AttrStruct(v.attri_type, v.attri_value)
        if show_type == 1 then
            desc = desc .. string.format(Language.PetWeapon.Text1, attri_info.attr_name, attri_info.attr_percent_value)
            if i ~= #attri_list then
                desc = desc .. ","
            end
        elseif show_type == 2 then
            desc = desc .. string.format("%s:+%s", attri_info.attr_name, attri_info.attr_percent_value)
            if i ~= #attri_list then
                desc = desc .. "\n"
            end
        end
    end
    return desc
end