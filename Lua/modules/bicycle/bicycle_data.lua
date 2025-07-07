--@TODO 2022-08-23 09:53:28
--灵骑技能改版，洗练技能弃用 #60705 子需求 G6-LineA3（新版本开发线-3） 灵骑技能改版-客户端


-- 灵骑
Bicycle = Bicycle or BaseClass()

-- 解码协议成灵骑单个信息
function Bicycle.DecodeToInfo()
    local info = {}
    info.id = MsgAdapter.ReadShort()
    info.level = MsgAdapter.ReadUShort()
    --info.remain_skill_point = MsgAdapter.ReadShort()  -- 剩余技能点
    info.energy_value = MsgAdapter.ReadShort()
    info.reveal_level = MsgAdapter.ReadUShort()         -- 点化等级
    info.is_breach_flag = MsgAdapter.ReadChar()					
    --info.is_refine_flag = MsgAdapter.ReadChar()       -- 是否有洗练
    info.model_type = MsgAdapter.ReadChar()             -- 当前形象类型 0为普通形象，1为穿戴灵饰的进阶形象

    info.halidom_info = {}
    info.halidom_info.level = MsgAdapter.ReadChar()     -- 圣物等级
	info.is_auto_recover = MsgAdapter.ReadChar()        -- 是否自动补充体力												
    info.capability = MsgAdapter.ReadInt()              -- 评分
    
    -- 单号62139 灵骑每提升1级，默认增加100固定评分。该评分仅作为评分展示，无实际意义
    info.capability = info.capability + 100 * (info.level - 1)
    
    -- 饰品信息
    info.ornament_info = BicycleBaseEquip.DecodeToInfo()
    -- 装备信息
    info.equip_info_list = {}
    for i = 0, BicycleCfg.MAX_EQUIP_NUM - 1 do
        info.equip_info_list[i] = BicycleBaseEquip.DecodeToInfo()
    end
    -- 基础技能信息
    info.base_skill_list = {}                                   
    for i = 0,BicycleCfg.MAX_BASE_SKILL_CUR_NUM - 1 do
        info.base_skill_list[i] = BicycleSkill.DecodeToInfo()
    end
    -- -- 洗练技能信息
	-- info.new_base_skill_list = {}						
    -- for i = 0,BicycleCfg.MAX_BASE_SKILL_CUR_NUM - 1 do
    --     local skill_id = MsgAdapter.ReadUShort()
    --     info.new_base_skill_list[i] = skill_id
    -- end
	info.pet_index_list = {}				
    for i = 0,BicycleCfg.MAX_EXCLUSIVE_SKILL_NUM - 1 do
        info.pet_index_list[i] = MsgAdapter.ReadChar()
    end
    MsgAdapter.ReadShort()
    --LOG("灵骑解析", info)
    return info
end

function Bicycle.ProtocolInfoToObj(protocol_info)
    local info = protocol_info
    return Bicycle.New(info)
end


function Bicycle:__init(info)
    self.info = info or {}
    self.id = info.id or 0      -- 服务端id从0开始，配置是从1开始
    self.level = self:Level(info.level)                                         -- 等级
    self.remain_skill_point = 0     --self:SkillPoint(info.remain_skill_point)          -- 剩余技能点 <<弃用>>
    self.energy_value = self:Energy(info.energy_value)                          -- 精力值
    self.is_breach_flag = self:Breach(info.is_breach_flag)			            -- 是否经过突破 1:完成突破
    self.is_refine_flag = self:Refine(info.is_refine_flag)                      -- 是否存在洗练 1:有洗练内容
    self.model_type = self:CurModelType(info.model_type)
    self.capability = self:Capability(info.capability)                          -- 评分
    self.dianhua_level = info.reveal_level or 0     -- 点化等级

    self.ornament = self:Ornament(info.ornament_info or {},true)                           -- 饰品
    self.equip_list = self:Equip(info.equip_info_list or {},true)                          -- 两个装备，下标0开始
    self.halidom = self:Halidom(info.halidom_info or {}, true)                  -- 圣物
    self.base_skill_list = self:BaseSkillList(info.base_skill_list or {},true)              -- 基础技能
    self.new_base_skill_list = self:NewBaseSkillList(info.new_base_skill_list or {},true)   -- 洗练的基础技能
    self.pet_index_list = self:PetIndexList(info.pet_index_list)                 -- 统御宠物的id

    self.attri_t = self:Attri()
    self.cfg = self:Cfg()
    self.index = info.index or self:Index()
end

-- 等级
function Bicycle:Level(value)
    if value then
        self.level = value
    end
    return self.level or 0
end

-- 剩余技能点
function Bicycle:SkillPoint(value)
    -- if value then
    --     self.remain_skill_point = value
    -- end
    -- return self.remain_skill_point or 0
    return self.remain_skill_point
end

-- 精力值
function Bicycle:Energy(value)
    if value then
        self.energy_value = value
    end
    return self.energy_value or 0
end

-- 当前形象类型 0为普通形象，1为穿戴灵饰的进阶形象
function Bicycle:CurModelType(value)
    if value then
        self.model_type = value
    end
    return self.model_type or 0
end

-- 当前模型id
function Bicycle:ModelId(model_type)
    local model_type = model_type or self:CurModelType()
    if model_type == 1 then
        return self:Model2()
    else
        return self:Model1()
    end
end

function Bicycle:Model1()
    local cfg = self:Cfg()
    return cfg.model_id_1
end

function Bicycle:Model2()
    local cfg = self:Cfg()
    return cfg.model_id_2
end

-- 灵性
function Bicycle:Nature(is_show_percent)
    local ornament = self:Ornament()
    local nature = 1
    if ornament:IsShow() then
        nature = ornament:Nature()
    else
        nature = self:Cfg().bicycle_grow_min
    end
    if is_show_percent then
        return (nature * 100) .. "%"
    end
    return nature
end

-- 评分
function Bicycle:Capability(value)
    if value then
        self.capability = value
    end
    return self.capability or 0
end

-- 专属技能等级
function Bicycle:ExclusiveLevelList(value)
    if value then
        self.exclusive_level_list = value
    end
    return self.exclusive_level_list or {[0] = 0,[1] = 0}
end

-- 基础技能 -- is_need_instance : 是否需要实例成技能BicycleSkill
function Bicycle:BaseSkillList(value,is_need_instance)
    if value then
        if not is_need_instance then
            self.base_skill_list = value
        else
            self.base_skill_list = {}
            for i = 0, BicycleCfg.MAX_BASE_SKILL_CUR_NUM - 1 do
                local v = value[i] or {}
                local base_skill_info = v or {}
                base_skill_info.skill_id = v.skill_id or 0
                base_skill_info.bicycle_id = v.bicycle_id or self.id
                base_skill_info.index = i
                base_skill_info.skill_type = BicycleCfg.SKILL_TYPE.BASE
                base_skill_info.is_unlock = v.is_unlock
                self.base_skill_list[i] = BicycleSkill.New(base_skill_info)
            end
        end
    end
    return self.base_skill_list
end

function Bicycle:GetBaseSkillByIndex(skill_index)
    local skill_list = self:BaseSkillList() or {}
    return skill_list[skill_index]
end

-- 洗练技能 <<弃用>>
function Bicycle:NewBaseSkillList(value,is_need_instance)
    if value then
        if not is_need_instance then
            self.new_base_skill_list = value
        else
            self.new_base_skill_list = {}
            for i,v in pairs(value) do
                local info = {}
                info.skill_id = v
                info.skill_type = BicycleCfg.SKILL_TYPE.BASE
                info.index = i
                info.bicycle_id = self.id
                local base_skill = self:GetSkill(nil,info.skill_type,info.index)
                info.level = base_skill:Level()
                self.new_base_skill_list[i] = BicycleSkill.New(info)
            end
        end
    end
    return self.new_base_skill_list or {}
end

-- 装备
function Bicycle:Equip(value_list,is_info)
    if value_list then
        if is_info then
            self.equip_list = {}
            value_list = next(value_list) == nil and {[0] = {},[1]= {}} or value_list
            local cfg = self:Cfg()
            local equip_id_list = Split(tostring(cfg.bicycle_equip),"|")
            for i,value in pairs(value_list) do
                local equip_id = equip_id_list[i + 1]
                equip_id = tonumber(equip_id)
                if value.is_active_flag ~= 1 then
                    local equip_cfg = Cfg.GetEquipCfg(equip_id,1)
                    self.equip_list[i] = BicycleEquip.New(nil,1,self.id,equip_cfg,i)
                else
                    local equip_cfg = Cfg.GetEquipCfg(equip_id,value.level)
                    self.equip_list[i] = BicycleEquip.New(equip_cfg.equip_icon,value.level,self.id,equip_cfg,i)
                end
            end
        else
            self.equip_list = value_list
        end
    end
    return self.equip_list
end
-- 饰品
function Bicycle:Ornament(value,is_info)
    if value then
        if is_info then
            local orn_id = self:Cfg().jewelry_id
            if next(value) == nil or value.is_active_flag ~= 1 then
                local orn_cfg = Cfg.GetOrnamentCfg(orn_id,1)
                self.ornament = BicycleOrnament.New(nil,1,self.id,orn_cfg)
            else
                local orn_cfg = Cfg.GetOrnamentCfg(orn_id,value.level)
                self.ornament = BicycleOrnament.New(orn_cfg.jewelry_icon,value.level,self.id,orn_cfg)
            end
        else
            self.ornament = value
        end
    end
    return self.ornament
end

-- 圣物
function Bicycle:Halidom(value,is_info)
    if value then
        if is_info then
            local id = self:Cfg().bicycle_hol
            if next(value) == nil or value.level < 1 then
                local halidom_cfg = Cfg.GetHalidomCfg(id,1)
                self.halidom = BicycleHalidom.New(nil,1,self.id,halidom_cfg)
            else
                local halidom_cfg = Cfg.GetHalidomCfg(id,value.level)
                self.halidom = BicycleHalidom.New(halidom_cfg.holy_icon,value.level,self.id,halidom_cfg)
            end
        else
            self.halidom = value
        end
    end
    return self.halidom
end

-- 御宠列表
function Bicycle:PetIndexList(value)
    if value then
        self.pet_index_list = value
    end
    return self.pet_index_list or {[0] = -1,[1] = -1}
end

-- 突破状态
function Bicycle:Breach(value)
    if value then
        self.is_breach_flag = value
    end
    if self.is_breach_flag == 1 or self.is_breach_flag == true then
        return true
    else
        return false
    end
end

-- 洗练技能状态
function Bicycle:Refine(value)
    if value then
        self.is_refine_flag = value
    end
    return self.is_refine_flag == 1 or self.is_refine_flag
end

-- 配置
function Bicycle:Cfg(value)
    if value then
        self.cfg = value
    elseif self.cfg == nil then
        self.cfg = BicycleData.Instance:GetBicycleCfg(self.id)
    end
    if self.cfg == nil then
        LogError(self.id)
    end
    return self.cfg
end

function Bicycle:Index()
    return self:Cfg().seq
end

-- 属性表
function Bicycle:Attri(value,update)
    if update then
        self.attri_t = nil
    end
    if value then
        self.attri_t = value
    elseif self.attri_t == nil then
        self.attri_t = BicycleData.Instance:CalculateAttri(self)
    end
    return self.attri_t
end

-- 根据形象
function Bicycle:Name(model_type)
    model_type = model_type or self:CurModelType()
    if model_type > 0 then
        return self:Cfg().bicycle_name_2
    else
        return self:Cfg().bicycle_name
    end
end

-- 获取技能
function Bicycle:GetSkill(skill_id,skill_type,skill_index)
    local skill_list = skill_type == BicycleCfg.SKILL_TYPE.BASE and self:BaseSkillList()
    if skill_index then
        return skill_list[skill_index]
    end
    for i,v in pairs(skill_list) do
        if skill_id == v.skill_id then
            return v
        end
    end
end

-- 灵骑点化等级
function Bicycle:DianHuaLevel()
    return self.dianhua_level or 0
end

-- 最大点化等级
function Bicycle:DianHuaMaxLevel()
    local bicycle_id = self.id
    local group_cfg = BicycleData.Instance:GetDianHuaGroupCfg(bicycle_id)
    local end_cfg = group_cfg[#group_cfg]
    if not end_cfg then
        LogError("灵骑获取最大点化等级异常",self.id)
        return 10
    end
    return end_cfg.reveal_level
end

-- 点化是否满级
function Bicycle:IsDianHuaFullLevel()
    local level = self:DianHuaLevel()
    local max_level = self:DianHuaMaxLevel()
    return level >= max_level
end

-- 灵骑点化评分
function Bicycle:DianHuaScore()
    local bicycle_id = self.id
    local level = self:DianHuaLevel()
    local score = BicycleData.Instance:GetDianHuaScore(bicycle_id, 1, level)
    return score
end

-- 灵骑点化属性加成
function Bicycle:GetDianHuaAttriList()
    local bicycle_id = self.id
    local level = self:DianHuaLevel()
    local attri_list = BicycleData.Instance:GetDianHuaAttrlList(bicycle_id,level)
    return attri_list
end

-- 下一级的灵骑点化属性加成
function Bicycle:GetNextDianHuaAttriList()
    if self:IsDianHuaFullLevel() then
        return {}
    end
    local bicycle_id = self.id
    local level = self:DianHuaLevel()
    local attri_list = BicycleData.Instance:GetDianHuaAttrlList(bicycle_id,level)
    local next_attri_list = BicycleData.Instance:GetDianHuaAttrlList(bicycle_id,level + 1)
    if level > 0 then
        for i,v in pairs(next_attri_list) do
            local cur_t = attri_list[i] or {attri_value = 0}
            v.attri_value = v.attri_value - cur_t.attri_value or 0
        end
    end
    return next_attri_list
end

-- 下一级的灵骑点化配置
function Bicycle:GetNextDianHuaCfg()
    if self:IsDianHuaFullLevel() then
        return {}
    end
    local bicycle_id = self.id
    local level = self:DianHuaLevel()
    local cfg = BicycleData.Instance:GetDianHuaCfg(bicycle_id, level + 1)
    return cfg
end

function Bicycle:IsAutoRecover()
    return self.is_auto_recover == 1
end


-- 灵骑技能
BicycleSkill = BicycleSkill or BaseClass()
-- info = {skill_id,bicycle_id,index,skill_type}
function BicycleSkill:__init(info)
    info = info or {}
    self.info = info
    self.skill_id = info.skill_id or 0                          -- 技能id
    self.bicycle_id = self:BicycleId(info.bicycle_id)           -- 灵骑id
    self.index = self:Index(info.index)                         -- 技能孔
    self.skill_type = self:SkillType(info.skill_type)           -- 技能类型

    self.unlock_level = self:UnlockLevel(info.unlock_level)     -- 解锁等级
    self.level = self:Level(info.level)                         -- 技能等级
    local is_unlock = self.info.is_unlock or self.skill_id > 0 and 1 or 0
    self.is_unlock = is_unlock == 1                   -- 是否解锁
    self.effect_cfg = self:EffectCfg()                          -- 技能效果配置
end

-- 解码协议成灵骑技能信息
function BicycleSkill.DecodeToInfo()
    local info = {}
    info.skill_id = MsgAdapter.ReadUShort()
    info.level = MsgAdapter.ReadChar()
    info.is_unlock = MsgAdapter.ReadChar()  -- 0未解锁，1已解锁
    return info
end

function BicycleSkill.ProtocolInfoToObj(protocol_info)
    local info = protocol_info
    return Bicycle.New(info)
end

function BicycleSkill:Name()
    return self:EffectCfg().name
end

function BicycleSkill:QuaName(is_shallow)
    local qua = self:Color()
    local quality_color = QualityColorStr
    if is_shallow and is_shallow == true then
        quality_color = QualityColorStrShallow
    end
    return ColorStr(self:Name(),quality_color[qua])
end

function BicycleSkill:Icon()
    return string.format("s%s",self.skill_id)
end

-- 技能效果配置
function BicycleSkill:EffectCfg()
    if not self.skill_id or self.skill_id == 0 then
        self.effect_cfg = {}
        return self.effect_cfg
    end
    self.effect_cfg = BicycleData.Instance:GetSkillEffect(self.skill_id,self:Level())
    return self.effect_cfg
end

function BicycleSkill:Desc()
    return self:EffectCfg().description
end

-- 技能等级
function BicycleSkill:Level(value)
    if value then
        self.level = value
    end
    return self.level or 0
end

-- 技能位置
function BicycleSkill:Index(value)
    if value then
        self.index = value
    end
    return self.index
end

-- 技能类型
function BicycleSkill:SkillType(value)
    if value then
        self.skill_type = value
    end
    return self.skill_type
end

-- 解锁等级
function BicycleSkill:UnlockLevel(value)
    if value then
        self.unlock_level = value
    end
    if self.unlock_level == nil and self.bicycle_id and self.index and self.skill_type then
        self.unlock_level = BicycleData.Instance:GetSkillUnlockLevel(self.bicycle_id,self.index,self.skill_type)
    end
    return self.unlock_level
end

-- 品质
function BicycleSkill:Color()
    local cfg = self:Cfg() or {}
    return cfg.color
end
-- 品质框
function BicycleSkill:ColorQua()
    local color = self:Color()
    return BicycleCfg.SkillQua[color]
end
-- 灵骑表里面的配置
function BicycleSkill:Cfg()
    return Cfg.GodBicycleSkillById(self.skill_id or 0)
end

-- 是否解锁
function BicycleSkill:IsUnlock()
    return self.is_unlock
end

-- 在哪个灵骑身上
function BicycleSkill:BicycleId(value)
    if value then
        self.bicycle_id = value
    end
    return self.bicycle_id
end

-- 是否能学习
function BicycleSkill:IsCanStudy()
    if self.skill_id > 0 then
        return false
    end
    if not self.is_unlock then
        return false
    end
    if not self.index or self.index < 0 then
        return false
    end
    local cfg = Config.bicycle_cfg_auto.skill_scroll
    for i,v in pairs(cfg) do
        local item_id = v.item_id
        local item_cfg = Item.GetConfig(item_id)
        if item_cfg.param2 == self.index then
            local item_num = Item.GetNum(item_id)
            if item_num > 0 then
                return true
            end
        end
    end
    return false
end

function BicycleSkill:IsMax()
    local level = self:Level()
    local max_level = BicycleData.Instance:GetSkillMaxLevel(self.skill_id)
    return level >= max_level
end

-- 是否能升级
function BicycleSkill:IsCanUpLevel()
    if self.skill_id <= 0 then
        return false
    end
    if not self.is_unlock then
        return false
    end
    if self:IsMax() then
        return false
    end

    -- local cfg = BicycleOtherData.Instance:GetSkillItemCfg(self.skill_id, self:Level())
    -- local item_id = cfg.item_id
    -- if Item.GetNum(item_id) > 0 then
    --     return true
    -- end

    local cfg = Config.bicycle_cfg_auto.skill_scroll
    for i,v in pairs(cfg) do
        local item_id = v.item_id
        local item_cfg = Item.GetConfig(item_id)
        if item_cfg.param2 == self.index then
            local item_num = Item.GetNum(item_id)
            if item_num > 0 and item_cfg.param3 == self:Level() then
                return true
            end
        end
    end

    return false
end

function BicycleSkill:Remind()
    if self:IsCanUpLevel() or self:IsCanStudy() then
        return 1
    end
    return 0
end


-- 灵骑装备基类
BicycleBaseEquip = BicycleBaseEquip or BaseClass()
function BicycleBaseEquip:__init(icon_id,level,bicycle_id,cfg)
    self.icon_id = icon_id or 0
    self.level = level or 0
    self.bicycle_id = bicycle_id or -1
    self.cfg = cfg or {}
end

function BicycleBaseEquip.DecodeToInfo()
    local info = {}
    info.is_active_flag = MsgAdapter.ReadChar()
    info.level = MsgAdapter.ReadUChar()
    MsgAdapter.ReadShort()
    return info
end

function BicycleBaseEquip:Level(value)
    if value then
        self.level = value
    end
    return self.level
end

function BicycleBaseEquip:IconId()
    return self.icon_id
end

function BicycleBaseEquip:BicycleId(value)
    if value then
        self.bicycle_id = value
    end
    return self.bicycle_id
end

function BicycleBaseEquip:IsShow()
    return self.icon_id and self.icon_id > 0
end

function BicycleBaseEquip:Cfg()
    return self.cfg
end

-- 灵骑装备
BicycleEquip = BicycleEquip or BaseClass(BicycleBaseEquip)
function BicycleEquip:__init(icon_id,level,bicycle_id,cfg,index)
    self.icon_id = icon_id or 0
    self.level = level or 1
    self.bicycle_id = bicycle_id or -1
    self.cfg = cfg or {}
    self.id = self:Id()
    self.index = index
end

function BicycleEquip:Id()
    return self.cfg.equip_id or 0
end

-- 装备在灵骑的哪个位置上，【0,1】可为空
function BicycleEquip:Index()
    return self.index
end

function BicycleEquip:CurEffectDesc(is_show_name)
    if self:Id() < 1 then
        return ""
    end
    local effect_id = self:Cfg().equip_skill_id
    local cfg = Cfg.MedalEffectById(effect_id)
    local desc = cfg.description
    if is_show_name then
        desc = string.format("%s:%s",cfg.name,desc)
    end
    return desc
end

function BicycleEquip:NextEffectDesc(is_show_name)
    local next_cfg = self:NextCfg()
    if not next_cfg then
        return
    end
    local effect_id = next_cfg.equip_skill_id
    local cfg = Cfg.MedalEffectById(effect_id)
    local desc = cfg.description
    if is_show_name then
        desc = string.format("%s:%s",cfg.name,desc)
    end
    return desc 
end

function BicycleEquip:MaxLevel()
    return BicycleData.Instance:GetEquipMaxLevel(self:Id())
end

function BicycleEquip:NextCfg()
    if not self:IsShow() or self:Level() >= self:MaxLevel() then
        return
    end
    local next_level = self:Level() + 1
    local next_cfg = Cfg.GetEquipCfg(self:Id(),next_level)
    return next_cfg
end

function BicycleEquip:IsMaxLevel()
    return self:Level() >= self:MaxLevel()
end

-- 灵骑饰品
BicycleOrnament = BicycleOrnament or BaseClass(BicycleBaseEquip)
function BicycleOrnament:__init(icon_id,level,bicycle_id,cfg)
    self.icon_id = icon_id or 0
    self.level = level or 0
    self.bicycle_id = bicycle_id or -1
    self.cfg = cfg or {}
    self.id = self:Id()
end

function BicycleOrnament:Id()
    return self:Cfg().jewelry_id or 0
end

-- // 灵性值，废弃
function BicycleOrnament:Nature()
    if not self:IsShow() then
        return 0
    else
        return self:Cfg().bicycle_grow / 100
    end
end

function BicycleOrnament:NextCfg()
    if not self:IsShow() or self:Level() >= self:MaxLevel() then
        return
    end
    local level = self:Level() + 1
    local next_cfg = Cfg.GetOrnamentCfg(self:Id(),level)
    return next_cfg
end

function BicycleOrnament:CurrCfg()
    return Cfg.GetOrnamentCfg(self:Id(),self:Level())
end

function BicycleOrnament:MaxLevel()
    local ornament_group = BicycleData.Instance.ornament_group_cfg[self:Id()] or {}
    return #ornament_group
end

function BicycleOrnament:IsMaxLevel()
    return self:Level() >= self:MaxLevel()
end



-- ============ 灵骑圣物 ===============
BicycleHalidom = BicycleHalidom or BaseClass(BicycleBaseEquip)
function BicycleHalidom:__init(icon_id,level,bicycle_id,cfg)
    self.icon_id = icon_id or 0
    self.level = level or 0
    self.bicycle_id = bicycle_id or -1
    self.cfg = cfg or {}
    self.id = self:Id()
end

function BicycleHalidom:Id()
    return self:Cfg().holy_id or 0
end

function BicycleHalidom:NextCfg()
    if not self:IsShow() or self:Level() >= self:MaxLevel() then
        return
    end
    local level = self:Level() + 1
    local next_cfg = Cfg.GetHalidomCfg(self:Id(),level)
    return next_cfg
end

function BicycleHalidom:MaxLevel()
    local halidom_group = BicycleData.Instance.halidom_group_cfg[self:Id()] or {}
    return #halidom_group
end

function BicycleHalidom:IsMaxLevel()
    return self:Level() >= self:MaxLevel()
end

function BicycleHalidom:ModelId()
    local cfg = self:Cfg()
    return cfg.holy_model
end

function BicycleHalidom:ModelColor()
    if self:IsShow() then
        return COLORS.White
    else
        return COLORS.Black1
    end
end

function BicycleHalidom:Attri()
    local cfg = self:Cfg()
    local attri = {
        attri_type = cfg.att_type,
        attri_value = cfg.att_value,
    }
    return attri
end

function BicycleHalidom:BicycleId()
    return self.bicycle_id
end

function BicycleHalidom:Bicycle()
    local id = self:BicycleId()
    if id > 0 then
        return BicycleData.Instance:GetBicycle(id)
    end
end

function BicycleHalidom:Name()
    local cfg = self:Cfg()
    return cfg.holy_name
end

function BicycleHalidom:ItemId()
    local cfg = self:Cfg()
    return cfg.holy_icon
end




-- ======================================================================================
-- ========================== BicycleData ========================
-- ======================================================================================

BicycleData = BicycleData or BaseClass()

function BicycleData:__init()
    if BicycleData.Instance ~= nil then
		Debuger.LogError("BicycleData] attempt to create singleton twice!")
		return
	end
    BicycleData.Instance = self
    self.cfg = Config.bicycle_cfg_auto
    self:FormatCfg()

    self.ride_info = SmartData.New({rede_bicycle_index = -1})  -- rede_bicycle_id == -1 表示未乘坐灵骑
    self.is_guide_flag = 0                                  -- 是否完成功能指引 0:未完成 1:已完成
    self.bicycle_count = 0                                  -- 灵骑拥有的数量

    self.selected_bicycle_data = SmartData.New()
    self.bicycle_list = SmartData.New({})             -- 存取根据服务端下发的数据生成的bicycle实例
    self.change_bicycle = SmartData.New({bicycle = {},last_bicycle = {},other_data = {}}) -- 存取最新变化的灵骑实例

    self.protocol_info_2103 = SmartData.New({})       -- 2103协议信息
    self.protocol_info_2104 = SmartData.New({})
    self.protocol_info_2105 = SmartData.New({})       
    self.protocol_info_2106 = SmartData.New({})
    self.protocol_info_2108 = SmartData.New({})

    self.bicycle_show_type = nil    -- 不为空的时候接收到某些下发时则打开BicycleShowView
    self.selected_skill = SmartData.New()
    self.cur_panel = SmartData.New({mod_key = 0})

    self.bicycle_energy_tip = SmartData.New({info = {}})
    self.bicycle_up_level_remind_flag = {} -- 灵骑检测升级红点标记（就是登录只显示一次红点，每个灵骑还是分开的
    self.bicycle_up_skill_remind_flag = {} -- 灵骑检测升级[技能，吐血]红点标记（就是登录只显示一次红点，每个灵骑还是分开的
    self.bicycle_energy_remind_flag = {}   -- 检测精力补充，登录显示一次就拉到
    self.cur_tab = SmartData.New({mod = Mod.Bicycle.Basice})
    self.ride_event = SmartData.New({value = false})
    self.cur_selected_model_type = SmartData.New({model_type = 0})

    self.remind_data = SmartData.New({value = false})
    self.cur_effect = SmartData.New({info = {effect = BicycleCfg.EFFECT.NULL,skill_index = nil}})

    self.JumpBicycleIndex = nil     -- 跳转选中的灵骑

    -- 灵骑装备圣物置换
    self.change_data = SmartData.New({change_type = BicycleCfg.ChangeItem.Equip, now = {}, change = {}})
end

function BicycleData:__delete()
   
end

-- 整理配置
function BicycleData:FormatCfg()
    --self.breach_cfg = KeyList(self.cfg.breach_attribute,"bicycle_id")
    -- 通过灵骑id和等级获取属性配置
    Cfg.Register("BicycleLvCfg",Cfg.RealTimeSearch2Key(self.cfg.bicycle_level,"bicycle_id","level"))
    -- 通过技能id和技能等级获取到技能配置
    Cfg.Register("GetBicycleSkillCfg",Cfg.RealTimeSearch2Key(Config.passive_skill_auto.passive_level_cfg,"skill_id","level"))
    -- 突破属性
    Cfg.Register("GetBicycleBreachCfg",Cfg.RealTimeSearch2Key(self.cfg.breach_attribute,"bicycle_id","level"))
    -- 饰品配置
    Cfg.Register("GetOrnamentCfg",Cfg.RealTimeSearch2Key(self.cfg.bicycle_jewelry,"jewelry_id","jewelry_level"))
    -- 灵骑装备
    Cfg.Register("GetEquipCfg",Cfg.RealTimeSearch2Key(self.cfg.bicycle_equip,"equip_id","equip_level"))
    -- 灵骑圣物
    Cfg.Register("GetHalidomCfg",Cfg.RealTimeSearch2Key(self.cfg.bicycle_holy,"holy_id","holy_level"))
    -- 灵骑点化
    Cfg.Register("GetDianHuaCfg",Cfg.RealTimeSearch2Key(self.cfg.bicycle_reveal,"bicycle_id","reveal_level"))
    -- 灵骑技能配置
    Cfg.Register("GodBicycleSkillById",Cfg.RealTimeSearch(self.cfg.skill_group,"skill_id"))
	--通过ResId找到坐骑配置，目前用于主角播放移动音效
    Cfg.Register("BicycleBaseInfoByModelId",Cfg.RealTimeSearchOR(self.cfg.bicycle_base,"model_id_1","model_id_2"))

    -- 带等级的被动技能整理
    self.skill_group_cfg = self:InitHelper(Config.passive_skill_auto.passive_level_cfg,"skill_id")
  
    -- 灵骑基础技能组整理
    self.base_skill_group_cfg = self:InitHelper(self.cfg.skill_group,"skill_group")

    -- 灵骑装备组整理
    self.equip_group_cfg = self:InitHelper(self.cfg.bicycle_equip,"equip_id")

    -- 灵骑饰品组整理
    self.ornament_group_cfg = self:InitHelper(self.cfg.bicycle_jewelry,"jewelry_id")

    -- 灵骑圣物组整理
    self.halidom_group_cfg = self:InitHelper(self.cfg.bicycle_holy,"holy_id")

end

-------------灵骑圣物转换-------------{
-- 灵骑装备圣物转换类型
function BicycleData:ChangeItemType(value)
    if value then
        self.change_data.change_type = value
    else
        return self.change_data.change_type
    end
end

-- 灵骑装备圣物转换前数据
function BicycleData:ChangeItemNow(value)
    if value then
        self.change_data.now = value
    else
        return self.change_data.now
    end
end

-- 灵骑装备圣物转换后数据
function BicycleData:ChangeItemChange(value)
    if value then
        self.change_data.change = value
    else
        return self.change_data.change
    end
end

-- 清空灵骑装备圣物转换数据
function BicycleData:ClearChangeItem()
    self.change_data.now = {}
    self.change_data.change = {}
end

-- 用于监听灵骑装备圣物转换道具数量变化
function BicycleData:ChangeItemList(change_type)
    if not self.change_item_list then
        self.change_item_list = {}
        for k, v in pairs(self.cfg.item_change) do
            if not self.change_item_list[v.change_type] then
                self.change_item_list[v.change_type] = v.consume_item
            end
        end
    end
    return change_type and self.change_item_list[change_type] or self.change_item_list
end

-- 用于返回灵骑装备或者圣物列表(消耗格子：1，目标格子：2)
function BicycleData:GetChangeItemList(type)
    if type == 1 then
        if self.change_data.change_type == BicycleCfg.ChangeItem.Equip then
            return BagData.Instance:GetBagBicycleEquipList()
        elseif self.change_data.change_type == BicycleCfg.ChangeItem.Halidom then
            return BagData.Instance:GetBagBicycleHalidomList()
        end
    else
        if TableIsEmpty(self.change_data.now) then
            return {}
        end
        local list = {}
        for k, v in pairs(self.cfg.item_change) do
            if self.change_data.change_type == v.change_type and self.change_data.now.item_id ~= v.convert_item then
                table.insert(list, Item.Init(v.convert_item, 1, 1))
            end
        end
        return list
    end
end
-------------灵骑圣物转换-------------}

--数据整理
function BicycleData:InitHelper(base_cfg,key)
    local new_cfg = {}
	for i=1,#base_cfg do
		local v = base_cfg[i]
		if new_cfg[v[key]] == nil then
			new_cfg[v[key]] = {}
		end
		table.insert(new_cfg[v[key]],v)
	end
    return new_cfg
end

-- 获取灵骑基础配置
function BicycleData:GetBicycleCfg(id)
    return self.cfg.bicycle_base[id]
end

-- 获取灵骑基础配置
function BicycleData:GetBicycleCfgByIndex(index)
    local id = index + 1
    return self.cfg.bicycle_base[id]
end

-- 获取灵骑技能孔解锁配置
function BicycleData:GetSkillOpenCfg(id)
    return self.cfg.Skill_hole_open[id]
end

-- 获取途径|解锁条件配置
function BicycleData:GetConditionCfg(seq)
    return self.cfg.gain_way[seq]
end

-- ==================== 属性区 ===================

-- 设置灵骑实例列表
function BicycleData:SetBicycleList(protocol)
    self.bicycle_count = protocol.count
    local list = {}
    for i,v in pairs(protocol.bicycle_list) do
        list[v.id] = Bicycle.New(v)
    end
    self.bicycle_list:Set(list)
    self:ResetBicycleUpLevelRemindFlag()
    self:UpdatePetBicycle()
end

-- 最新接收的单个灵骑信息
function BicycleData:CurBicycle(bicycle)
    self.change_bicycle.bicycle = bicycle
    self:UpdateBicycleList(bicycle)
end

-- 更新灵骑实例列表
function BicycleData:UpdateBicycleList(bicycle)
    if self.bicycle_list[bicycle.id] then
        self.bicycle_list[bicycle.id] = bicycle
    else
        self.bicycle_list[bicycle.id] = bicycle
    end
    self:UpdatePetBicycle()
end

-- 获取灵骑列表数据
function BicycleData:GetBicycleListData()
    local list_data = TableCopy(self.cfg.bicycle_base)
    for i,v in pairs(list_data) do
        v.lock_flag = self:BicycleIsActive(v) and 1 or 0
    end
    table.sort(list_data,function (a,b)
        if a.lock_flag == b.lock_flag then
            if a.lock_flag == 0 then
                return a.seq < b.seq
            else
                if a.bicycle_color == b.bicycle_color then
                    return a.seq < b.seq
                end
                return a.bicycle_color > b.bicycle_color
            end
        end
        return a.lock_flag > b.lock_flag
    end)
    for i,v in pairs(list_data) do
        v.cell_index = i
    end
    return list_data
end

-- 获取激活的灵骑数量
function BicycleData:GetBicycleCount()
    local count = 0
    for i,v in pairs(self.cfg.bicycle_base) do
        if self:BicycleIsActive(v) then
            count = count + 1
        end
    end
    return count
end

-- 设置和获取灵骑基础配置数据
function BicycleData:SetSelectedData(data)
    data = data or {}
    return self.selected_bicycle_data:Set(data)
end

function BicycleData:GetSelectedData()
    return self.selected_bicycle_data
end

-- 获取全部灵骑实例
function BicycleData:GetBicycleList()
    return self.bicycle_list
end

-- 获取灵骑实例
function BicycleData:GetBicycle(id)
    return self.bicycle_list[id] or Bicycle.New({id = id})
end

-- 获取灵骑实例
function BicycleData:GetBicycleByIndex(index)
    local id = index + 1
    return self.bicycle_list[id] or Bicycle.New({id = id})
end

-- 获取当前选中的灵骑实例
function BicycleData:GetSelectedBicycle()
    local sel_data = self:GetSelectedData()
    return self:GetBicycle(sel_data.bicycle_id)
end

-- 获取已激活的灵骑实例 -- 可为空
function BicycleData:GetActiveBicycle(id)
    return self.bicycle_list[id]
end

-- 当前选中的技能实例
function BicycleData:GetSelectedSkill()
    if self.selected_skill and next(self.selected_skill) ~= nil then
        local skill_type = self.selected_skill:SkillType()
        local index = self.selected_skill:Index()
        local bicycle = self:GetBicycle(self.selected_skill:BicycleId())
        return bicycle:GetSkill(nil,skill_type,index)
    end
end

function BicycleData:SetSelectedSkill(skill)
    skill = skill or {}
    self.selected_skill:Set(skill)
end

-- 判断灵骑是否被乘骑 seq
function BicycleData:IsRide(bicycle_seq)
    return bicycle_seq == self.ride_info.rede_bicycle_index
end

-- 当前准备展示的界面
function BicycleData:BicycleShowType(value)
    if value then
        self.bicycle_show_type = value
    end
    return self.bicycle_show_type
end

-- 当前准备播放的特效
function BicycleData:CurEffect(value)
    if value then
        self.cur_effect.info = value
    end
    return self.cur_effect.info
end

-- 指引标记
function BicycleData:GuideFlag(value)
    if value then
        self.is_guide_flag = value
    end
    return self.is_guide_flag
end

-- 获取和设置当前选中的标签页 value = mod_key
function BicycleData:CurTabMod(value)
    if value then
        self.cur_tab.mod = value
    end
    return self.cur_tab.mod
end

-- 乘坐的灵骑
function BicycleData:RideBicycleIndex(value)
    if value then
        self.ride_info.rede_bicycle_index = value
    end
    return self.ride_info.rede_bicycle_index
end

-- 设置变化之前的灵骑
function BicycleData:LastBicycle(bicycle)
    self.change_bicycle.last_bicycle = bicycle
end

-- 清空变化的灵骑记录
function BicycleData:ClearChangeBicycle()
    self.change_bicycle.bicycle = {}
    self.change_bicycle.last_bicycle = {}
end

-- ==================== 逻辑区 ===================

-- ========== 基础逻辑 ==========

-- 是否激活了
function BicycleData:BicycleIsActive(data)
    if self.bicycle_list[data.bicycle_id] then
        return true
    else
        return false
    end
end

function BicycleData:BicycleIsActiveById(id)
    if self.bicycle_list[id] then
        return true
    else
        return false
    end
end

-- 根据灵骑激活所需的道具id来判断灵骑是否已激活
function BicycleData:BicycleIsActiveByItemId(item_id)
    for k, v in pairs(self.cfg.bicycle_base) do
        if v.activate_use == item_id then
            return self:BicycleIsActiveById(v.bicycle_id)
        end
    end
    return false
end

-- 是否可激活灵骑 1 = 可解锁，0条件不足，-1道具不足
function BicycleData:ActiveBicycleFlag(data)
    local unlock_condition = self:GetUnlockTip(data.bicycle_id)
    if unlock_condition then
        return 0
    end
    local stuff_count = Item.GetNum(data.activate_use)
    if stuff_count < data.use_number then
        return -1
    end
    return 1
end

-- 灵骑激活条件检查 返回的是进度。返回空则不显示进度
function BicycleData:GetConditionProgress(condition_cfg)
    local condition_type = condition_cfg.way_type
    local p1 = condition_cfg.param1
    local p2 = condition_cfg.param2
    local p1_num = 0
    local p2_num = 0
    if condition_type == BicycleCfg.CONDITION_TYPE.ROLE_LEVEL then
        p2_num = RoleData.Instance:GetRoleLevel()
    elseif condition_type == BicycleCfg.CONDITION_TYPE.BICYCLE_LEVEL then
        local bicycle_list = self:GetBicycleList()
        local max_level = 0
        local count = 0
        for i,v in pairs(bicycle_list) do
            local level = v:Level()
            if level > max_level then
                max_level = v:Level()
            end
            if level >= p2 then
                count = count + 1
            end
        end
        p1_num = count
        p2_num = max_level
    elseif condition_type == BicycleCfg.CONDITION_TYPE.BICYCLE_SKILL_LEVEL then
        local bicycle_list = self:GetBicycleList()
        local max_skill_level = 0
        local count = 0
        for i,v in pairs(bicycle_list) do
            local base_skill_list = v:BaseSkillList()
            for j,v2 in pairs(base_skill_list) do
                local level = v2:Level()
                if level > max_skill_level then
                    max_skill_level = level
                end
                if level >= p2 then
                    count = count + 1
                end
            end
        end
        p1_num = count
        p2_num = max_skill_level
    elseif condition_type == BicycleCfg.CONDITION_TYPE.BICYCLE_SKILL_MAX_LEVEL then
        local bicycle_list = self:GetBicycleList()
        local max_skill_level = 0
        local count = 0
        for i,v in pairs(bicycle_list) do
            local base_skill_list = v:BaseSkillList()
            for j,v2 in pairs(base_skill_list) do
                if v2:Level() > max_skill_level then
                    max_skill_level = v2:Level()
                end
            end
        end
        p1_num = 0
        p2_num = max_skill_level
    elseif condition_type == BicycleCfg.CONDITION_TYPE.ACTIVITY then
        p1_num = 1
        p2_num = 1
    end
    return p1_num,p2_num
end

-- 获取灵骑条件解锁提示
function BicycleData:GetUnlockTip(bicycle_id)
    local cfg = self:GetBicycleCfg(bicycle_id)
    local condition_list = Split(tostring(cfg.gain_way),"|")
    for i,v in ipairs(condition_list) do
        v = tonumber(v)
        local condition_cfg = self:GetConditionCfg(v)
        local p1,p2 = self:GetConditionProgress(condition_cfg)
        if condition_cfg.param2 == 0 then
            return
        end
        if condition_cfg.param1 > 1 then
            if p1 < condition_cfg.param1 then
                return v
            end
        else
            if p2 < condition_cfg.param2 then
                return v
            end
        end
    end
end

-- 获取激活灵骑描述
function BicycleData:GetUnlockDesc(bicycle_id)
    local cfg = self:GetBicycleCfg(bicycle_id)
    local condition_list = Split(tostring(cfg.gain_way),"|")
    local all_desc = ""
    for i,v in ipairs(condition_list) do
        v = tonumber(v)
        local condition_cfg = self:GetConditionCfg(v)
        local p1,p2 = self:GetConditionProgress(condition_cfg)
        local progress_str = ""
        if condition_cfg.param1 > 1 then
            if p1 >= condition_cfg.param1 then
                p1 = condition_cfg.param1
                progress_str = string.format("（%s/%s）",ColorStr(p1,COLORSTR.Green4),condition_cfg.param1)
            else
                progress_str = string.format("（%s/%s）",ColorStr(p1,COLORSTR.Red8),condition_cfg.param1)
            end
        elseif condition_cfg.param2 > 0 then
            if p2 >= condition_cfg.param2 then
                p2 = condition_cfg.param2
                progress_str = string.format("（%s/%s）",ColorStr(p2,COLORSTR.Green4),condition_cfg.param2)
            else
                progress_str = string.format("（%s/%s）",ColorStr(p2,COLORSTR.Red8),condition_cfg.param2)
            end
        end
        local desc = string.format("%s.%s%s\n",i,condition_cfg.way_explain,progress_str)
        all_desc = all_desc .. desc
    end
    return all_desc
end

-- 获取装备组
function BicycleData:GetEquipGroup(equip_id)
    return self.equip_group_cfg[equip_id] or {}
end
-- 获取装备最大等级
function BicycleData:GetEquipMaxLevel(equip_id)
    local equip_group = self:GetEquipGroup(equip_id)
    return #equip_group
end
-- 获取装备全部效果
function BicycleData:GetEquipAllEffect(equip_id)
    local effect_list = {}
    local equip_group = self:GetEquipGroup()
    for i,v in ipairs(equip_group) do
        effect_list[i] = v.equip_skill_id
    end
    return effect_list
end
-- 获取装备全部特效描述
function BicycleData:GetEquipAllEffectDesc(equip)
    local id = equip:Id()
    local level = equip:IsShow() and equip:Level() or 0
    local effect_all_desc = ""
    local equip_group = self:GetEquipGroup(id)
    local max_level = #equip_group
    for i,v in ipairs(equip_group) do
        local cfg = Cfg.MedalEffectById(v.equip_skill_id)
        local desc = cfg.description
        desc = string.format(Language.Bicycle.EquipDesc,DataHelper.GetDaXie(v.equip_level),desc)
        if v.equip_level == level then
            desc = ColorStr(desc,COLORSTR.Yellow16)
        elseif v.equip_level < level then
            desc = ColorStr(desc,COLORSTR.Yellow7)
        end
        if i ~= max_level then
            effect_all_desc = string.format("%s%s\n",effect_all_desc,desc)
        else
            effect_all_desc = string.format("%s%s",effect_all_desc,desc)
        end
    end
    return effect_all_desc
end

-- 通过道具id获取装备id
function BicycleData:GetEquipIdByItem(item)
    local item_id = item.item_id
    for i,v in pairs(self.cfg.bicycle_equip) do
        if v.equip_icon == item_id then
            return v.equip_id
        end
    end
end

-- 获取装备全部特效描述List
function BicycleData:GetEquipAllEffectList(equip_id)
    local id = equip_id
    local equip_group = self:GetEquipGroup(id)
    return equip_group
end

-- 通过道具id获取饰品id
function BicycleData:GetOrnamentIdByItem(item)
    local item_id = item.item_id
    for i,v in pairs(self.cfg.bicycle_jewelry) do
        if v.jewelry_icon == item_id then
            return v.jewelry_id
        end
    end
end

-- 获取饰品描述List
function BicycleData:GetOrnamentAllEffectList(id)
    local list = {}
    for i, v in ipairs(self.ornament_group_cfg[id]) do
        local t = {}
        t.equip_level = v.jewelry_level
        t.skill_cfg = {}
        t.skill_cfg.description = Format(Language.Bicycle.NatureDescItemInfo, v.bicycle_grow, v.jewelry_up_percentage)
        table.insert(list, t)
    end
    return list
end

-- 通过道具id获取圣物id
function BicycleData:GetHalidomIdByItem(item)
    local item_id = item.item_id
    for i,v in pairs(self.cfg.bicycle_holy) do
        if v.holy_icon == item_id then
            return v.holy_id
        end
    end
end

-- 获取圣物描述List
function BicycleData:GetHalidomAllEffectList(id)
    local list = {}
    for i, v in ipairs(self.halidom_group_cfg[id]) do
        local t = {}
        t.equip_level = v.holy_level
        t.skill_cfg = {}
        local attri_info = CommonStruct.AttrStruct(v.att_type, v.att_value)
        t.skill_cfg.description = Format(Language.Bicycle.HalidomDescItemInfo, attri_info.attr_name, attri_info.attr_percent_value)
        table.insert(list, t)
    end
    return list
end


-- 获取饰品灵性全部加成描述 -- COLORSTR.Green4
function BicycleData:GetOrnamentAllNatureDesc(ornament)
    local id = ornament:Id()
    local level = ornament:IsShow() and ornament:Level() or 0
    local effect_all_desc = ""
    local ornament_group = self.ornament_group_cfg[id]
    local max_level = #ornament_group
    for i,v in ipairs(ornament_group) do
        local desc
        local up_desc = v.jewelry_up_percentage .. "%"
        if v.jewelry_level == level then
            desc = string.format(Language.Bicycle.NatureDesc,DataHelper.GetDaXie(v.jewelry_level),COLORSTR.Green4,v.bicycle_grow,COLORSTR.Green4,up_desc)
            desc = ColorStr(desc,COLORSTR.Yellow16)
        elseif v.jewelry_level < level then
            desc = string.format(Language.Bicycle.NatureDesc,DataHelper.GetDaXie(v.jewelry_level),COLORSTR.Yellow7,v.bicycle_grow,COLORSTR.Yellow7,up_desc)
            desc = ColorStr(desc,COLORSTR.Yellow7)
        else
            desc = string.format(Language.Bicycle.NatureDesc,DataHelper.GetDaXie(v.jewelry_level),COLORSTR.Green4,v.bicycle_grow,COLORSTR.Green4,up_desc)
        end
        if i ~= max_level then
            effect_all_desc = string.format("%s%s\n",effect_all_desc,desc)
        else
            effect_all_desc = string.format("%s%s",effect_all_desc,desc)
        end
    end
    return effect_all_desc
end


-- 获取圣物全部加成描述 -- COLORSTR.Green4
function BicycleData:GetHalidomAllNatureDesc(halidom)
    local id = halidom:Id()
    local level = halidom:IsShow() and halidom:Level() or 0
    local effect_all_desc = ""
    local halidom_group = self.halidom_group_cfg[id]
    local max_level = #halidom_group
    for i,v in ipairs(halidom_group) do
        local desc
        local attri_info = CommonStruct.AttrStruct(v.att_type, v.att_value)
        if v.holy_level == level then
            desc = string.format(Language.Bicycle.HalidomDesc,DataHelper.GetDaXie(v.holy_level),attri_info.attr_name,COLORSTR.Green4,attri_info.attr_percent_value)
            desc = ColorStr(desc,COLORSTR.Yellow16)
        elseif v.holy_level < level then
            desc = string.format(Language.Bicycle.HalidomDesc,DataHelper.GetDaXie(v.holy_level),attri_info.attr_name,COLORSTR.Yellow7,attri_info.attr_percent_value)
            desc = ColorStr(desc,COLORSTR.Yellow7)
        else
            desc = string.format(Language.Bicycle.HalidomDesc,DataHelper.GetDaXie(v.holy_level),attri_info.attr_name,COLORSTR.Green4,attri_info.attr_percent_value)
        end
        if i ~= max_level then
            effect_all_desc = string.format("%s%s\n",effect_all_desc,desc)
        else
            effect_all_desc = string.format("%s%s",effect_all_desc,desc)
        end
    end
    return effect_all_desc
end

-- ========== 培养逻辑 ==========

-- 是否需要突破
function BicycleData:IsNeedBreach(bicycle)
    if bicycle:Breach() then
        return false
    end
    local cfg = Cfg.BicycleLvCfg(bicycle.id,bicycle:Level())
    if cfg.is_need_breach == 1 then
        return true,cfg
    else
        return false
    end
end

-- 获取突破属性配置
function BicycleData:GetBreachCfg(id,level,is_brech_flag)
    for i = #self.cfg.breach_attribute,1,-1 do
        local v = self.cfg.breach_attribute[i]
        if v.bicycle_id == id then
            if is_brech_flag then
                if level >= v.level then
                    return v
                end
            else
                if level > v.level then
                    return v
                end
            end
        end
    end
end

-- 获取灵骑最大等级
function BicycleData:GetMaxLevel(bicycle_id)
    local cfg = self:GetBicycleCfg(bicycle_id)
    return cfg.level_max or 50
end

-- 灵骑是否可以升级
-- 1 == 可以 0 = 未激活 -1 材料不足 -2 等级不足 -3 最大等级 -4 突破材料不足
function BicycleData:GetIsCanUpLevel(data)
    local is_unlock = self:BicycleIsActive(data)
    if not is_unlock then
        return 0
    end
    local bicycle = self:GetBicycle(data.bicycle_id)
    local is_need_breach = self:IsNeedBreach(bicycle)
    if is_need_breach then
        local breach_cfg = Cfg.GetBicycleBreachCfg(bicycle.id,bicycle:Level())
        local stuff_id = breach_cfg.breach_consume
        if Item.GetNum(stuff_id) < breach_cfg.breach_consume_num then
            return -4
        else
            return 1
        end
    end
    if bicycle:Level() >= self:GetMaxLevel(bicycle.id) then
        return -3
    end
    local level = bicycle:Level()
    local up_cfg = Cfg.BicycleLvCfg(bicycle.id,level)
    local next_up_cfg = Cfg.BicycleLvCfg(bicycle.id,level + 1)
    local stuff_id = up_cfg.level_up_use
    if RoleData.Instance:GetRoleLevel() < next_up_cfg.rolelevel_limit then
        return -2
    end
    if Item.GetNum(stuff_id) < up_cfg.use_number then
        return -1
    end
    
    return 1
end

-- 计算灵骑属性 : 当前最大属性值*(1 - 投放比例)+当前最大属性值*投放比例*((当前灵性 - 初始) / (最大灵性-初始))
function BicycleData:CalculateAttri(bicycle,level)
    if level == nil then
        level = bicycle:Level()
    end
    local max_level = self:GetMaxLevel(bicycle.id)
    level = level > max_level and max_level or level
    if level < 1 then
        level = 1
    end
    local lv_cfg = Cfg.BicycleLvCfg(bicycle.id,level)
    local nature = bicycle:Nature()
    local min_nature = bicycle:Cfg().bicycle_grow_min
    local max_nature = bicycle:Cfg().bicycle_grow_max
    local proportion = bicycle:Cfg().grow_min_proportion / 100  -- 投放比例
    local attri = {}
    local get_cfg_attri = function (t,cfg,nature)
        nature = nature or 1
        for i = 1,5 do
            if cfg["attr_add_" .. i] and cfg["attr_add_" .. i] ~= "" and cfg["attr_add_" .. i] > 0 then
                t[cfg["attr_type_" .. i]] = t[cfg["attr_type_" .. i]] or 0

                -- 主要计算就是下面这两行代码
                local attri = cfg["attr_add_" .. i]
                local attri_num = attri * ((1 - proportion) + proportion * (nature - min_nature)/(max_nature - min_nature))
                --LogError("AAA",attri_num,proportion,"A",nature,"B",min_nature,"C",max_nature)

                t[cfg["attr_type_" .. i]] = t[cfg["attr_type_" .. i]] + attri_num
            end
        end
    end
    get_cfg_attri(attri,lv_cfg,nature)
    local breach_cfg = self:GetBreachCfg(bicycle.id,level,bicycle:Breach())
    if not TableIsEmpty(breach_cfg) then
        get_cfg_attri(attri,breach_cfg,nature)
    end
    for i,v in pairs(attri) do
        attri[i] = math.floor(v)
    end
    --LogError("Attri",attri)
    --BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.ATTR_REQ,bicycle:Index()) -- 向服务端请求属性测试
    return attri
end

-- 通过id和等级获取属性数据
function BicycleData:GetAttriList(bicycle_id,level)
    level = level or 1
    level = level < 1 and 1 or level
    local lv_cfg = Cfg.BicycleLvCfg(bicycle_id,level)
    local nature = 1
    local attri = {}
    local get_cfg_attri = function (t,cfg,nature)
        nature = nature or 1
        for i = 1,5 do
            if cfg["attr_add_" .. i] and cfg["attr_add_" .. i] ~= "" and cfg["attr_add_" .. i] > 0 then
                t[cfg["attr_type_" .. i]] = t[cfg["attr_type_" .. i]] or 0
                t[cfg["attr_type_" .. i]] = t[cfg["attr_type_" .. i]] + (cfg["attr_add_" .. i] * nature)
            end
        end
    end
    get_cfg_attri(attri,lv_cfg,nature)
    for i,v in pairs(attri) do
        attri[i] = math.floor(v)
    end
    return attri
end

-- 灵骑属性表转成UIWAttrItem2的data_list
function BicycleData:ConvertAttriToData2(attri_t)
    local data_list = {}
    for k,v in pairs(attri_t) do
        local data = {}
        data.attri_type = k
        data.attri_value = v
        data.name = Language.AttrItems.AttrNames[k]
        data.value = v
        table.insert(data_list,data)
    end
    table.sort(data_list,function (a,b)
        return a.attri_type < b.attri_type
    end)
    return data_list
end

-- 灵骑属性表转成UIWAttrItem3的data_list
function BicycleData:ConvertAttriToData3(attri_t1,attri_t2)
    local data_list = {}
    local attri_t
    local t1_is_empty
    if TableIsEmpty(attri_t1) then
        t1_is_empty = true
        attri_t = attri_t2
    else
        t1_is_empty = false
        attri_t = attri_t1
    end
    for k,v in pairs(attri_t) do
        local data = {}
        data.attri_type = k
        data.name = Language.AttrItems.AttrNames[k]
        if t1_is_empty then
            data.value = 0
            data.attri_value = 0
        else
            data.value = v
            data.attri_value = v
        end
        data.add_value = attri_t2[k]
        table.insert(data_list,data)
    end
    table.sort(data_list,function (a,b)
        return a.attri_type < b.attri_type
    end)
    return data_list
end

-- ========== 技能逻辑 ==========

-- 获取技能效果
function BicycleData:GetSkillEffect(skill_id,level)
    level = (level == nil or level < 1) and 1 or level
    local cfg = Cfg.GetBicycleSkillCfg(skill_id,level,false)
    if not cfg then
        return {}
    end
    local effect_cfg = Cfg.MedalEffectById(cfg.sp_id)
    return effect_cfg
end

-- 是否有技能
function BicycleData:IsHasSkill(skill_list)
    if TableIsEmpty(skill_list) then
        return false
    end
    for i,v in pairs(skill_list) do
        if v.skill_id > 0 then
            return true
        end
    end
    return false
end

-- 获取技能最高等级
function BicycleData:GetSkillMaxLevel(skill_id)
    local skill_group = self.skill_group_cfg[skill_id]
    local max_level = 0
    
    if skill_group == nil then
        return 1
    end
    for i,v in pairs(skill_group) do
        if v.level > max_level then
            max_level = v.level
        end
    end
    return max_level
end

-- 获取灵骑全部激活的技能
function BicycleData:GetBicycleAllSkill(bicycle)
    local skill_list = {}
    local base_skill_list = bicycle:BaseSkillList()
    for i,v in pairs(base_skill_list) do
        if v:IsUnlock() then
            table.insert(skill_list,v)
        end
    end
    return skill_list
end

-- 获取灵骑全部专属技能id  << 废弃 >>
function BicycleData:GetAllExSkill()
    if self.all_ex_skill_list == nil then
        self.all_ex_skill_list = {}
        for i,v in pairs(self.cfg.Skill_hole_open) do
            local skill_1 = v.special_skill_1
            local skill_2 = v.special_skill_2
            if self.all_ex_skill_list[skill_1] == nil then
                self.all_ex_skill_list[skill_1] = {skill_id = skill_1}
                self.all_ex_skill_list[skill_1].max_level = self:GetSkillMaxLevel(skill_1)
            end
            if self.all_ex_skill_list[skill_2] == nil then
                self.all_ex_skill_list[skill_2] = {skill_id = skill_2}
                self.all_ex_skill_list[skill_2].max_level = self:GetSkillMaxLevel(skill_2)
            end
        end
    end
    return self.all_ex_skill_list
end

-- 获取灵骑全部基础技能
function BicycleData:GetAllBaseSkill()
    -- 三个技能孔对应哪些技能组的技能
    if self.all_base_skill_list == nil then
        self.all_base_skill_list = {}
        for i = 0, BicycleCfg.MAX_BASE_SKILL_CUR_NUM - 1 do
            self.all_base_skill_list[i] = {}
            for k,v in pairs(self.cfg.Skill_hole_open) do
                local skill_group = v["skill_group_" .. (i + 1)]
                if self.all_base_skill_list[i][skill_group] == nil then
                    self.all_base_skill_list[i][skill_group] = self.base_skill_group_cfg[skill_group]
                end
            end
        end
    end
    return self.all_base_skill_list
end

-- 获取升级技能最大标记，1，可升级，0，未激活，-1材料不足，-2达到最大等级 -3 技能点不足,-4 灵骑等级不足
function BicycleData:GetUpSkillLevelFlag(skill)
    local level = skill:Level() < 1 and 1 or skill:Level()
    local up_cfg = self:GetSkillUpLevelCfg(skill:BicycleId(),skill:Index(),level,skill:SkillType() + 1)
    if skill:BicycleId() == nil or not skill:IsUnlock() then
        return 0,up_cfg
    end
    local max_level = self:GetSkillIndexMaxLevel(skill:BicycleId(),skill:Index())
    if max_level <= level then
        return -2,up_cfg
    end
    local bicycle = self:GetBicycle(skill:BicycleId())
    local stuff_id = up_cfg.skillup_consume_item[0].item_id
    local need_num = up_cfg.skillup_consume_item[0].num
    local next_up_cfg = self:GetSkillUpLevelCfg(skill:BicycleId(),skill:Index(),level + 1,skill:SkillType() + 1)
    -- if bicycle:Level() < next_up_cfg.bicycle_level_limit then
    --     return -4,up_cfg,next_up_cfg
    -- end
    if Item.GetNum(stuff_id) < need_num then
        return -1,up_cfg
    end

    -- 技能点判断弃用
    -- local skill_point = bicycle:SkillPoint()
    -- if skill_point < up_cfg.skill_point_consume then
    --     return -3,up_cfg
    -- end
    return 1,up_cfg
end

-- 获取技能孔多少级解锁
function BicycleData:GetSkillUnlockLevel(bicycle_id,index,skill_type)
    local cfg = self:GetSkillOpenCfg(bicycle_id)
    if skill_type == BicycleCfg.SKILL_TYPE.BASE then
        return cfg["openlevel_" .. (index + 1)]
    elseif skill_type == BicycleCfg.SKILL_TYPE.EXCLUSIVE then
        return cfg["openlevel_" .. (index + 4)]
    end
    return 0
end

-- 获取技能孔最大等级
function BicycleData:GetSkillIndexMaxLevel(bicycle_id,index)
    local max_level = 0
    for i,v in pairs(self.cfg.Skill_hole_up) do
        if v.bicycle_id == bicycle_id and v.Skill_hole_index == index then
            if v.Skill_hole_level > max_level then
                max_level = v.Skill_hole_level
            end
        end
    end
    return max_level
end

-- 获取技能升级配置
function BicycleData:GetSkillUpLevelCfg(bicycle_id,index,level,skill_type)
    for i,v in pairs(self.cfg.Skill_hole_up) do
        if v.bicycle_id == bicycle_id and v.Skill_hole_index == index and v.Skill_hole_level == level and v.type == skill_type then
            return v
        end
    end
end

-- ========== 统御逻辑 ==========

-- 获取宠物的列表
function BicycleData:GetPetList()
    local pet_list = PetData.Instance:GetPetList()
    pet_list = table.rearrange(pet_list)
    pet_list = PetData.Instance:PetSort(pet_list)
    return pet_list or {}
end

-- 更新全部宠物的统御信息
function BicycleData:UpdatePetBicycle()
    local bicycle_list = self:GetBicycleList()
    local pet_list = PetData.Instance:GetPetList()
    for i,v in pairs(pet_list) do
        PetData.Instance:SetBicycle(v,nil)
    end
    for i,v in pairs(bicycle_list) do
        local pet_index_list = v:PetIndexList()
        for j,v2 in pairs(pet_index_list) do
            local pet = pet_list[v2]
            if pet then
                local bicycle_info =  {bicycle_index = v:Index(),cell_index = j}           
                PetData.Instance:SetBicycle(pet,bicycle_info)
            end
        end
    end
end

-- 灵骑是否可统御宠物
function BicycleData:IsCanPet(bicycle_id)
    local bicycle = self.bicycle_list[bicycle_id]
    if not bicycle then
        return false
    end
    if not self:HasPet() then
        return false
    end
    if self:GetPetCount(bicycle) < BicycleCfg.MAX_PET_NUM then
        return true
    end
end

-- 宠物背包是否有可统御的宠物
function BicycleData:HasPet()
    local pet_list = PetData.Instance:GetPetList()
    for i,v in pairs(pet_list) do
        if v:Bicycle() == nil then
            return true
        end
    end
    return false
end

-- 获取灵骑带了多少宠物
function BicycleData:GetPetCount(bicycle)
    local pet_index_list = bicycle:PetIndexList()
    local num = 0
    for i,v in pairs(pet_index_list or {}) do
        if v >= 0 then
            num = num + 1
        end
    end
    return num
end

-- 圣物开启等级
function BicycleData:HalidomOpenLevel()
    local level = self.cfg.bicycle_holy[1].role_level_limit or 0
    return level
end

function BicycleData:IsOpenHalidom()
    local limit_level = self:HalidomOpenLevel()
    local role_level = RoleData.Instance:GetRoleLevel()
    return role_level >= limit_level
end

-- 灵骑指引是否打开
function BicycleData:IsOpenGuide()
    -- 判断第一个灵骑是否解锁，且是否有标记
    local role_id = RoleData.Instance:GetRoleId()
    local flag = self:OpenFlag()
    if flag == 1 then
        return false 
    end
    local is_active = self:BicycleIsActiveById(1)
    if not is_active then
        return true
    end
    return false
end

-- 设置打开标记
function BicycleData:OpenFlag(v)
    local role_id = RoleData.Instance:GetRoleId()
    if v then
        UnityPlayerPrefs.SetInt("BicycleData_IsOpenGuide" .. role_id,v)
    end
    
    local flag = UnityPlayerPrefs.GetInt("BicycleData_IsOpenGuide" .. role_id,0)
    return flag
end

-- 宠物统御跳转
function BicycleData:JumpPet(pet)
    if not TableIsEmpty(pet:Bicycle()) then
        PublicPopupCtrl.Instance:AlertTip(Language.Bicycle.RemovePetTip, function ()
            BicycleData.Instance.JumpBicycleIndex = pet:Bicycle().bicycle_index
            ViewMgr:OpenViewByKey(Mod.Bicycle.PetPanel)
        end)
        return true
    end
    return false
end


-- =================== 灵骑点化 =====================

function BicycleData:IsOpenDianHua()
    return FunOpen.Instance:IsFunOpen(Mod.Bicycle.DianHua)
end

-- 单个灵骑全部点化配置
function BicycleData:GetDianHuaGroupCfg(bicycle_id)
    if not self.dian_hua_group_cfg then
        self.dian_hua_group_cfg = DataHelper.ConvertGroup(self.cfg.bicycle_reveal, "bicycle_id")
    end
    return self.dian_hua_group_cfg[bicycle_id] or {}
end

-- 获取灵骑点化配置GetDianHuaCfg
function BicycleData:GetDianHuaCfg(bicycle_id, reveal_level)
    return Cfg.GetDianHuaCfg(bicycle_id, reveal_level)
end

-- 点化评分
function BicycleData:GetDianHuaScore(bicycle_id, start_level, end_level)
    local group_cfg = self:GetDianHuaGroupCfg(bicycle_id)
    local score = 0
    for i = start_level, end_level do
        local cfg = group_cfg[i]
        if cfg then
            score = score + cfg.score
        end
    end
    return score
end

-- 点化属性     -- 弃用，配置结构改了！！！！！！！！
-- function BicycleData:GetDianHuaAttrlList(bicycle_id, start_level, end_level)
--     local group_cfg = self:GetDianHuaGroupCfg(bicycle_id)
--     local attri_list = {}
--     for i = start_level, end_level do
--         local cfg = group_cfg[i]
--         if cfg then
--             for n = 1,5 do
--                 local attri_type = cfg["attr_type_" .. n]
--                 local attri_value = cfg["attr_add_" .. n]
--                 if attri_value > 0 then
--                     if not attri_list[attri_type] then
--                         attri_list[attri_type] = {attri_type = attri_type, attri_value = 0, sort = n}
--                     end
--                     attri_list[attri_type].attri_value = attri_list[attri_type].attri_value + attri_value
--                 end
                
--             end
--         end
--     end
--     attri_list = table.rearrange(attri_list)
--     table.sort(attri_list, function (a, b)
--         return a.sort < b.sort
--     end)
--     return attri_list
-- end


function BicycleData:GetDianHuaAttrlList(bicycle_id, end_level)
    if end_level == 0 then
        return {}
    end
    local group_cfg = self:GetDianHuaGroupCfg(bicycle_id)
    local attri_list = {}
    local cfg = group_cfg[end_level]
    if cfg then
        for n = 1,5 do
            local attri_type = cfg["attr_type_" .. n]
            local attri_value = cfg["attr_add_" .. n]
            if attri_value > 0 then
                if not attri_list[attri_type] then
                    attri_list[attri_type] = {attri_type = attri_type, attri_value = 0, sort = n}
                end
                attri_list[attri_type].attri_value = attri_list[attri_type].attri_value + attri_value
            end
            
        end
    end
    attri_list = table.rearrange(attri_list)
    table.sort(attri_list, function (a, b)
        return a.sort < b.sort
    end)
    return attri_list
end

-- =================================================
-- =================================================






-- ==================== 红点区 ===================

-- 全部红点逻辑
function BicycleData:RemindNum()
    -- 全部解锁红点
    if self:BicycleUnlockRemindNum() >= 1 then
        return 1
    end
    -- 全部升级红点
    if self:BicycleUpLevelRemindNum() >= 1 then
        return 1
    end
    -- 全部技能红点
    if self:BicycleUpSkillRemindNum() >= 1 then
        return 1
    end
    -- 全部装备红点
    if self:BicycleEquipRemindNum() >= 1 then
        return 1
    end
    -- 全部精力红点
    if self:BicycleEnergyRemindNum() >= 1 then
        return 1
    end
    -- 全部灵饰红点
    if self:BicycleOrnamentRemindNum() > 0 then
        return 1
    end
    -- 全部圣物红点
    if self:BicycleHalidomRemindNum() > 0 then
        return 1
    end
    -- 全部点化红点
    if self:BicycleDianHuaRemindNum() > 0 then
        return 1
    end
    return 0
end

-- 检查红点
function BicycleData:CheckRemind()
    self.remind_data.value = not self.remind_data.value
end

-- 重置灵骑升级红点标记和升级技能红点标记，在角色登录游戏收到灵骑全部信息的时候触发
function BicycleData:ResetBicycleUpLevelRemindFlag()
    local bicycle_data_list = self.cfg.bicycle_base
    for i,v in pairs(bicycle_data_list) do
        self.bicycle_up_level_remind_flag[v.bicycle_id] = 0
        self.bicycle_up_skill_remind_flag[v.bicycle_id] = 0
        self.bicycle_energy_remind_flag[v.bicycle_id] = 0
    end
end

-- 灵骑解锁红点 -- 改为标签页红点比较合理
function BicycleData:BicycleUnlockRemindNum()
    local bicycle_data_list = self.cfg.bicycle_base
    for i,v in pairs(bicycle_data_list) do
        local flag = self:BicycleOneUnlockRemindNum(v)
        if flag > 0 then
            return 1
        end
    end
    -- 额外加个装备红点逻辑
    if self:BicycleEquipRemindNum() > 0 then
        return 1
    end
    -- 加个精力红点
    if self:BicycleEnergyRemindNum() > 0 then
        return 1
    end
    -- 加个灵饰红点
    if self:BicycleOrnamentRemindNum() > 0 then
        return 1
    end
    -- 圣物红点
    if self:BicycleHalidomRemindNum() > 0 then
        return 1
    end
    return 0
end

-- 单个灵骑解锁红点
function BicycleData:BicycleOneUnlockRemindNum(data,id)
    if data == nil and id then
        data = self:GetBicycleCfg(id)
    end
    if self:BicycleIsActive(data) then
        return 0
    end
    local flag = self:ActiveBicycleFlag(data)
    if flag > 0 then
        return 1
    end    
    return 0
end

-- 灵骑升级红点 -- 每个升级的灵骑红点登录只显示一次
function BicycleData:BicycleUpLevelRemindNum()
    local bicycle_data_list = self.cfg.bicycle_base
    for i,v in pairs(bicycle_data_list) do
        local flag = self:BicycleOneUpLevelRemindNum(v)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

-- 单个灵骑升级红点
function BicycleData:BicycleOneUpLevelRemindNum(data,id)
    if data == nil and id then
        data = self:GetBicycleCfg(id)
    end
    if not self:BicycleIsActive(data) then
        return 0
    end
    if self.bicycle_up_level_remind_flag[data.bicycle_id] == 1 then
        return 0
    end
    local flag = self:GetIsCanUpLevel(data)
    if flag > 0 then
        return 1
    end
    return 0
end

-- 灵骑技能升级红点
function BicycleData:BicycleUpSkillRemindNum()
    local bicycle_data_list = self.cfg.bicycle_base
    for i,v in pairs(bicycle_data_list) do
        local flag = self:BicycleOneUpSkillRemindNum(v)
        if flag > 0 then
            return 1
        end
    end
    return 0
end

-- 单个灵骑技能升级红点
function BicycleData:BicycleOneUpSkillRemindNum(data,id)
    if data == nil and id then
        data = self:GetBicycleCfg(id)
    end
    if not self:BicycleIsActive(data) then
        return 0
    end
    -- if self.bicycle_up_skill_remind_flag[data.bicycle_id] == 1 then
    --     return 0
    -- end
    local bicycle = self:GetBicycle(data.bicycle_id)
    local base_skill_list = bicycle:BaseSkillList()
    for i,v in pairs(base_skill_list) do
        if v:Remind() > 0 then
            return 1
        end
    end
    return 0
end

-- 全部灵骑统御宠物红点
function BicycleData:BicyclePetRemindNum()
    local bicycle_data_list = self.cfg.bicycle_base
    for i,v in pairs(bicycle_data_list) do
        local flag = self:IsCanPet(v.bicycle_id)
        if flag == true then
            return 1
        end
    end
    return 0
end

-- 穿戴装备标记 -- 1 可激活 -- 2 可升级 -- 0 材料不足 -1 等级不足 -2 未激活灵骑 -3 最大等级
function BicycleData:BicycleOneEquipRemindNum(equip)
    local bicycle = self:GetBicycle(equip:BicycleId())
    if not self:BicycleIsActiveById(bicycle.id) then
        return -2
    end
    if equip:IsMaxLevel() then
        return -3
    end
    local cfg = equip:Cfg()

    local item_data = cfg.equip_consume_item[0]
    local item_num = Item.GetNum(item_data.item_id)
    if item_num >= item_data.num then
        if equip.icon_id > 0 then
            local role_level = RoleData.Instance:GetRoleLevel()
            if role_level < cfg.role_level_limit then
                return -1
            end
            return 2
        else
            return 1
        end
    else
        return 0
    end
end

-- 全部灵骑装备可操作红点
function BicycleData:BicycleEquipRemindNum()
    local bicycle_data_list = self.cfg.bicycle_base
    for i,v in pairs(bicycle_data_list) do
        local bicycle = self.bicycle_list[v.bicycle_id]
        if bicycle then
            for i = 0, 1 do
                local equip = bicycle:Equip()[i]
                local flag = self:BicycleOneEquipRemindNum(equip)
                if flag > 0 then
                    return 1
                end
            end
        end
    end
    return 0
end

-- 精力补充单个红点
function BicycleData:BicycleEnergyOneRemindNum(bicycle)
    if self.bicycle_energy_remind_flag[bicycle.id] == 1 then
        return 0
    end
    if not self:BicycleIsActiveById(bicycle.id) then
        return 0
    end
    if bicycle:Energy() < 500 then
        return 1
    else
        return 0
    end
end
-- 精力补充全部红点
function BicycleData:BicycleEnergyRemindNum()
    local bicycle_data_list = self.cfg.bicycle_base
    for i,v in pairs(bicycle_data_list) do
        local bicycle = self.bicycle_list[v.bicycle_id]
        if bicycle then
            local flag = self:BicycleEnergyOneRemindNum(bicycle)
            if flag > 0 then
                return 1
            end
        end
    end
    return 0
end

-- 单个灵饰红点 0没有饰品, -1 未激活，-2 已达最大等级
function BicycleData:BicycleOrnamentOneRemindNum(bicycle)
    if not self:BicycleIsActiveById(bicycle.id) then
        return -1
    end
    local ornament = bicycle:Ornament()
    if ornament and ornament:IsMaxLevel() then
        return -2
    end
    local stuff_id = bicycle:Cfg().bicycle_jewelry
    local num = Item.GetNum(stuff_id)
    if num > 0 then
        return 1
    end
    return 0
end
-- 全部灵饰红点
function BicycleData:BicycleOrnamentRemindNum()
    local bicycle_data_list = self.cfg.bicycle_base
    for i,v in pairs(bicycle_data_list) do
        local bicycle = self.bicycle_list[v.bicycle_id]
        if bicycle then
            local flag = self:BicycleOrnamentOneRemindNum(bicycle)
            if flag > 0 then
                return 1
            end
        end
    end
    return 0
end

-- 单个圣物红点 0没有圣物, -1 未激活，-2 已达最大等级, -3角色等级不足
function BicycleData:BicycleHalidomOneRemindNum(bicycle)
    if not self:BicycleIsActiveById(bicycle.id) then
        return -1
    end
    local halidom = bicycle:Halidom()
    if halidom and halidom:IsMaxLevel() then
        return -2
    end
    local cfg = halidom:Cfg()
    local role_level = RoleData.Instance:GetRoleLevel()
    local limit_level = cfg.role_level_limit
    if role_level < limit_level then
        return -3
    end
    local stuff_id = cfg.holy_consume_item[0].item_id
    local need_num = cfg.holy_consume_item[0].num
    local num = Item.GetNum(stuff_id)
    if num >= need_num then
        return 1
    end
    return 0
end
-- 全部圣物红点
function BicycleData:BicycleHalidomRemindNum()
    if not self:IsOpenHalidom() then
        return 0
    end
    local bicycle_data_list = self.cfg.bicycle_base
    for i,v in pairs(bicycle_data_list) do
        local bicycle = self.bicycle_list[v.bicycle_id]
        if bicycle then
            local flag = self:BicycleHalidomOneRemindNum(bicycle)
            if flag > 0 then
                return 1
            end
        end
    end
    return 0
end


-- 单个点化红点 1可升级，0道具不足, -1 未激活，-2 已达最大等级, -3角色等级不足
function BicycleData:BicycleDianHuaOneRemindNum(bicycle)
    if not self:BicycleIsActiveById(bicycle.id) then
        return -1
    end
    if bicycle:IsDianHuaFullLevel() then
        return -2
    end
    local role_level = RoleData.Instance:GetRoleLevel()
    local cfg = bicycle:GetNextDianHuaCfg()
    local need_level = cfg.rolelevel_limit
    if role_level < need_level then
        return -3
    end
    local item_id = cfg.level_up_use
    local has_num = Item.GetNum(item_id)
    local need_num = cfg.use_number
    if has_num < need_num then
        return 0
    end
    return 1
end

-- 全部点化红点
function BicycleData:BicycleDianHuaRemindNum()
    if not self:IsOpenDianHua() then
        return 0
    end
    local bicycle_data_list = self.cfg.bicycle_base
    for i,v in pairs(bicycle_data_list) do
        local bicycle = self.bicycle_list[v.bicycle_id]
        if bicycle then
            local flag = self:BicycleDianHuaOneRemindNum(bicycle)
            if flag > 0 then
                return 1
            end
        end
    end
    return 0
end



-- 红点全部涉及到的ItemId
function BicycleData:GetRemindItemList()
    local item_id_list = {}
    -- 所有饰品
    for i,v in pairs(self.ornament_group_cfg) do
        local cfg = v[1]
        table.insert(item_id_list,cfg.bicycle_jewelry)
    end
    -- 所有装备
    for i,v in pairs(self.equip_group_cfg) do
        local cfg = v[1]
        table.insert(item_id_list,cfg.equip_icon)
    end
    -- 所有圣物
    for i,v in pairs(self.halidom_group_cfg) do
        local cfg = v[1]
        table.insert(item_id_list,cfg.holy_icon)
    end
    table.insert(item_id_list,self.cfg.bicycle_level[1].level_up_use)
    table.insert(item_id_list,self.cfg.breach_attribute[1].breach_consume)
    table.insert(item_id_list,self.cfg.Skill_hole_up[1].skillup_consume_item[0].item_id)
    table.insert(item_id_list,self.cfg.bicycle_reveal[1].level_up_use)
    return item_id_list
end

-- 解码协议成灵骑单个信息
function Bicycle.RoleDecodeToInfo()
    local info = {}
    info.id = MsgAdapter.ReadShort()
    info.level = MsgAdapter.ReadUShort()
    info.halidom_info = {}
    info.halidom_info.level = MsgAdapter.ReadShort()
	info.reveal_level = MsgAdapter.ReadUShort()
    info.capability = MsgAdapter.ReadInt()

    -- 单号62139 灵骑每提升1级，默认增加100固定评分。该评分仅作为评分展示，无实际意义
    info.capability = info.capability + 100 * (info.level - 1)

    info.attr_list = {}
    for i=1,BicycleCfg.MAX_ATTR_NUM do
        local attri_type,attri_value = MsgAdapter.ReadShort(),MsgAdapter.ReadShort()
        if info.attr_list[attri_type] == nil and attri_value ~= 0 then
            info.attr_list[attri_type] = attri_value
        end
    end
    -- 基础技能信息
    info.base_skill_list = {}                                   
    for i = 0,BicycleCfg.MAX_BASE_SKILL_CUR_NUM - 1 do
        info.base_skill_list[i] = BicycleSkill.DecodeToInfo()
    end
    -- 饰品信息
    info.ornament_info = BicycleBaseEquip.DecodeToInfo()
    -- 装备信息
    info.equip_info_list = {}
    for i = 0, BicycleCfg.MAX_EQUIP_NUM - 1 do
        info.equip_info_list[i] = BicycleBaseEquip.DecodeToInfo()
    end
    return info
end