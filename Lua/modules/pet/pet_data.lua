--用于服务端发来的宠物信息
Pet = Pet or BaseClass()

function Pet:__init(info)
    self.pet_id = info.pet_id or info.id or info.item_id
    self.name = info.name
    self.index = info.index or -1
    self.level = info.level or 1
    self.str_level = info.str_level or 1
    self.yao_str_level = info.lian_yao_pet_str_lv or 1
    self.lock = info.lock or 0
    self.data = PetData.Instance
    self.config = self:PetCfg()
    self.info = info
    self.info.lian_yao_passive_list = self.info.lian_yao_passive_list or {}
    self.skill_list = info.gift_list == nil and nil or info.gift_list
    self.gift_list = info.gift_list or self:initConfigGiftList()
    self.info.gift_list = self.gift_list
    self.sort = info.sort
    self.info.skill_list = {}
    if info.attr_list then
        self.attr_list = info.attr_list
    end
    if self.config == nil then
        LogError("pet_id",self.pet_id," config is nil")
        return
    end
    for i=1, self.config.skill_num do
        table.insert(self.info.skill_list, self.config["skill_id_"..tostring(i)])
    end

    self.info.equipment_skill_list = self.info.equipment_skill_list or {}
    self.info.equipment_skill_refresh_list = self.info.equipment_skill_refresh_list or {}    --宠物身上的装备洗练后未保存的洗练技能
    if self.info.replace_skill_id and self.info.replace_skill_id ~= 0 and self.info.replace_index > self.config.gift_max then
        self.info.replace_skill_id = 0
    end
    
    self.skill_gift_list = nil
    self.element_gift_same_type = nil
    self.info.skin_id = self.info.skin_id or self.info.pet_skin
    self:initBetterSkill()
    self:initPetEquipItem()
    if info.weapon_info and info.weapon_info.item_id > 0 then
        self.weapon = PetWeapon.New(info.weapon_info)
        self.weapon:GridPos(0)
    end
    if info.weapon_info2 and info.weapon_info2.item_id > 0 then
        self.weapon2 = PetWeapon.New(info.weapon_info2)
        self.weapon2:GridPos(1)
    end
    if info.stamp_blueprint_info then
        self.stamp_blueprint = PetStampBlueprint.New(info.stamp_blueprint_info, -1, self.index)
        local cur_blue = PetStampData.Instance:GetCurBuleprint()
        if cur_blue and cur_blue.pet_index == self.index then
            PetStampData.Instance:SetCurBuleprint(self.stamp_blueprint)
        end
    end

    self.pet_strengthen_eat_index = 0 --宠物强化吃掉的材料序号

    if not TableIsEmpty(info.bagde_item_param) then
        self.bagde_item = PetBagde.New({item_id = info.bagde_item_param.item_id, param = info.bagde_item_param})
    end
end

function Pet:Index()
    return self.index
end

function Pet:IsEquip()
    return false
end

function Pet:Rank()
    return self.info.rank or -1
end

function Pet:ID()
    return self.pet_id
end

function Pet:NormalId()
    return self.config.prototype_id
end

function Pet:SkinId()
    return self.info.skin_id or 0
end

function Pet:SkinIconId()
    if self:SkinId() ~= 0 then
        local co = Cfg.PetSkinAttr(self:SkinId())
        return co and co.head_portrait or nil
    end
end

function Pet:Name()
    return self.name or self:CfgName()
end
function Pet:CfgName()
    return self.config.name
end

function Pet:InitialName()
    if self:IsMutant() then
        local pet_id = self:NormalId()
        local cfg = PetData.Instance:GetPetCfg(pet_id)
        return cfg.name
    else
        return self:CfgName()
    end
end

function Pet:QuaName(is_shallow)
    local quality_color = QualityColorStr
    if is_shallow and is_shallow == true then
        quality_color = QualityColorStrShallow
    end
    return ColorStr(self:Name(),quality_color[self:Quality()])
end

function Pet:Level()
    return self.level
end

function Pet:PetCfg()
    return self.data:GetPetCfg(self.pet_id)
end

function Pet:MainAttr()
    return self.config.main_attribute
end

function Pet:ResId()
    if self.info and self.info.skin_id and self.info.skin_id ~= 0 then
        local skin_cfg = Cfg.PetSkinAttr(self.info.skin_id)
        if skin_cfg and skin_cfg.res_id then
            return skin_cfg.res_id
        end
    end
    return self.config and self.config.res_id or 508
end

function Pet:StrLevel()
    return self.str_level
end

function Pet:YaoStrLevel()
    return self.yao_str_level
end

function Pet:Exp()
    return self.info.exp
end

function Pet:MP()
    return self.info.mp or self.config.maxmp
end

function Pet:HP()
    return self.info.hp or self.config.maxhp
end

function Pet:IsBind()
    return self.info.is_bind == 1
end

function Pet:EleAttr()
    return PetData.GetElement(self.config)
end

function Pet:MaxAttr()
    return self.config.max_attr or 0
end

function Pet:BaseAttr()
    return PetData.BaseAttr(self.config)
end

function Pet:GetGiftNum()
    return self.gift_num
end

-- 成长值 配置成长值+强化加成+共鸣加成
function Pet:ConfigGrowth(str_level)
    local total = self.data:GetConfigGrowth(self.pet_id)
    total = total + self:StrGrowth(str_level)
    total = total + self:EchoGrowth()
    return total
end

-- 总成长 = 基础成长+强化成长+共鸣成长 
function Pet:TotalGrowth(str_level)
    local total = self:BaseGrowth() + self:StrGrowth(str_level) + self:EchoGrowth()
    return total
end

-- 基础成长
function Pet:BaseGrowth()
    return PetData.GetGrowth(self:BaseGrowthList())
end

-- 强化成长
function Pet:StrGrowth(str_level)
    return PetData.GetGrowth(self:StrGrowthList(str_level))
end

-- 共鸣成长
function Pet:EchoGrowth()
    -- ********* 2022/6/1 #58689宠物强化共鸣改版 *****
    return PetData.GetGrowth(self:EchoGrowthList())
end

-- 已激活的皮肤成长值
function Pet:SkinGrowth()
    local growth_cfg = {}
    local skin_list_cfg = PetData.Instance.pet_skin_cfg[self.pet_id]
    if skin_list_cfg ~= nil and next(skin_list_cfg) then
        for k,v in pairs(skin_list_cfg) do
            if PetData.Instance:GetSkinHasIsActive(v.pet_skin_id) then
                growth_cfg = PetData.AddGrowth(growth_cfg, v)
            end
        end
    end
    return growth_cfg
end

--当前是否有待保存的洗炼结果（比较暴力，直接看洗炼结果里的属性是否位零）
function Pet:HasXilian()
    return self.info.new_con_growth ~= 0 or self.info.new_str_growth ~= 0 or self.info.new_def_growth ~= 0 or self.info.new_agi_growth ~= 0 or self.info.new_magic_growth ~= 0
end

-- 总配置资质
function Pet:ConfigGrowthList(str_level)
    return PetData.AddGrowth(self:SkinGrowth(), PetData.AddGrowth(self.config, PetData.AddGrowth(self:StrGrowthList(str_level), self:EchoGrowthList())))
end

-- 洗炼后总资质 = 洗炼后基础资质+强化+共鸣
function Pet:TotalXilianGrowthList(str_level)
    return PetData.AddGrowth(self:XilianBaseGrowthList(), PetData.AddGrowth(self:StrGrowthList(str_level), self:EchoGrowthList()))
end

-- 总资质 = 基础+强化+共鸣
function Pet:TotalGrowthList(str_level)
    return PetData.AddGrowth(self:SkinGrowth(), PetData.AddGrowth(self:BaseGrowthList(), PetData.AddGrowth(self:StrGrowthList(str_level), self:EchoGrowthList())))
end

-- 强化资质
function Pet:StrGrowthList(str_level)
    local level = str_level or self.str_level
    return self.data:ConvertStrGrowth(self.config.strengthen_id, tonumber(level) or 0)
end

-- 洗炼后基础资质
function Pet:XilianBaseGrowthList()
    local res = {}
    res.con = self.info.new_con_growth
    res.str = self.info.new_str_growth
    res.def = self.info.new_def_growth
    res.agi = self.info.new_agi_growth
    res.magic = self.info.new_magic_growth
    return res
end

-- 基础资质
function Pet:BaseGrowthList()
    return PetData.ConvertGrowth(self.info)
end

-- 设置共鸣等级
function Pet:SetComLevel(level)
    self.com_level = level
end

-- 共鸣资质
function Pet:EchoGrowthList()
    if self.com_level then
        local cfg = PetData.Instance.pet_strengthen_buff[self.com_level]
        if cfg then
            local list = PetData.ConvertBuff(cfg)
            return list
        end
    end
    local _, list = self.data:CheckPetStrBuff(self.index)
    return list
end

-- 基础状态 是否跟随
function Pet:State()
    if self:PrayMainIndex() ~= nil then
        return PetState.Pray
    end
    return self.data:GetPetState(self.index)
end

-- 是否擂台
function Pet:ArenaState()
    return self.data:GetIsArena(self.index)
end

-- 是否共鸣
function Pet:IsEcho()
    return self.data:CheckPetStrBuff(self.index)
end

-- 共鸣等级
function Pet:BuffLevel()
    local _, _, buff_level = self.data:CheckPetStrBuff(self.index)
    return buff_level
end

-- 服务端加点
function Pet:Point()
    return PetData.ConvertPoint(self.info)
end

-- 总资质资质换算点
function Pet:AptiPoint()
    return PetData.GetAptiPoint(self:TotalGrowthList(), self.level)
end

-- 总加点
function Pet:TotalPoint()
    return PetData.GetTotalPoint(self:Point(), self:AptiPoint())
end

-- 加点属性（资质属性）
function Pet:AptiAttr()
    return PetData.GetAptiAttr(self:TotalPoint())
end
-- 强化属性
function Pet:StrengthAttr()
    return self.data:GetStrengthenCfg(self.config.strengthen_id, self.str_level)
end
-- 魂器属性
function Pet:WeaponAttr()
    local weapon = self:Weapon()
    if weapon then
        return weapon:AttriList()
    end
end
-- 神印属性
function Pet:StampAttr()
    local stamp_blueprint = self:StampBlueprint()
    if stamp_blueprint and stamp_blueprint:IsOk() then
        return PetStampData.Instance:GetBlueprintAddition(stamp_blueprint)
    end
end

-- 排行榜中的宠物属性列表由服务器下发在下发时处理接口
function Pet:SetAttrList(attr_list)
    self.attr_list = attr_list
end

function Pet:GiftListAttr()
    local attr_list = {}
    if self.gift_list == nil then
        return attr_list
    end
    for k,v in pairs(self.gift_list) do
        if v.gift_id ~= 0 then
            local gift_cfg = Cfg.SkillPassive(v.gift_id)
            if gift_cfg ~= nil then
                --获得对应天赋技能的特殊效果
                local special_effect_cfg = Cfg.SpecialEffect(gift_cfg.sp_id)
                if special_effect_cfg ~= nil then
                    for i=1,special_effect_cfg.effect_num do                    --技能的特殊效果数量可能不唯一，遍历处理
                        if special_effect_cfg["effect"..i.."_type"] == 3 then   --类型3的效果类型为加属性和技能等级的特效
                            --每个效果id影响的属性配置
                            local effect_attr_cfg = Cfg.SpecialAddAttr(special_effect_cfg["effect"..i.."_id"])
                            if effect_attr_cfg ~= nil then
                                if effect_attr_cfg.type == 1 then --直接属性值增加固定点数
                                    attr_list[effect_attr_cfg.param1] = (attr_list[effect_attr_cfg.param1] or 0) + effect_attr_cfg.param2
                                elseif effect_attr_cfg.type == 4 then --受宠物等级影响的
                                    attr_list[effect_attr_cfg.param1] = (attr_list[effect_attr_cfg.param1] or 0) + self.level * effect_attr_cfg.param2 / 100
                                end
                            end
                        end
                    end
                end
            end
        end
    end

    --主动技能的属性加成
    local skill_list = self:SkillList()
    for k,v in pairs(skill_list) do
        --不同的主动技能等级属性加成数值不同
        local skill_cfg = SkillData.GetConfig(v.skill_id, v.skill_level)
        if skill_cfg and skill_cfg.sp_id then
            local special_effect_cfg = Cfg.SpecialEffect(skill_cfg.sp_id)
            if special_effect_cfg ~= nil then
                for i=1,special_effect_cfg.effect_num do                    --技能的特殊效果数量可能不唯一，遍历处理
                    if special_effect_cfg["effect"..i.."_type"] == 3 then   --类型3的效果类型为加属性和技能等级的特效
                        --每个效果id影响的属性配置
                        local effect_attr_cfg = Cfg.SpecialAddAttr(special_effect_cfg["effect"..i.."_id"])
                        if effect_attr_cfg ~= nil then
                            if effect_attr_cfg.type == 1 then --直接属性值增加固定点数
                                attr_list[effect_attr_cfg.param1] = (attr_list[effect_attr_cfg.param1] or 0) + effect_attr_cfg.param2
                            elseif effect_attr_cfg.type == 4 then --受宠物等级影响的
                                attr_list[effect_attr_cfg.param1] = (attr_list[effect_attr_cfg.param1] or 0) + self.level * effect_attr_cfg.param2 / 100
                            end
                        end
                    end
                end
            end
        end
    end
    return attr_list
end

-- 总属性
function Pet:TotalAttr()
    if self.attr_list ~= nil then
        return self.attr_list
    else
        local attr_list = PetData.TotalAttr(self:BaseAttr(), self:AptiAttr(), self:StrengthAttr(), self:WeaponAttr(), self:StampAttr())
        local gift_list_attr = self:GiftListAttr()
        for i=0, PET_ATTR_LIST_COUNT do
            local g_at = gift_list_attr[i] or 0
            local a_at = attr_list[i] or 0
            attr_list[i] = math.floor(a_at + g_at)
        end
        return attr_list
    end
end

function Pet:initBetterSkill()
    self.pet_skill_score = 0
    if self.info.skill_list ~= nil and self.info.gift_list ~= nil then
        self.skill_same_type_list = {}
        self.gift_skill_list = {}
        local lower_skill_list = {}
        local skill_score_cfg = Config.pet_cfg_auto.pet_hitbook_score[self.level]
        if skill_score_cfg == nil then
            skill_score_cfg = {}
            LogError("pet level ",self.level, " skill_score_cfg is nil(宠物等级"..self.level.."级的pet_hitbook_score配置为空)")
        end
        self.gift_num = 0
        for k,v in pairs(self.info.gift_list) do
            if v.gift_id ~= 0 then
                self.gift_num = self.gift_num + 1
                local gift_cfg = self.data:GetGiftDetail(v.gift_id)
                if gift_cfg then 
                    local skill_type_cfg = Cfg.SkillPassive(gift_cfg.param_1)
                    if skill_type_cfg then
                        local skill_type = skill_type_cfg.skill_type
                        if skill_type_cfg.skill_type <= 4 then
                            skill_type = 0
                            self.element_gift_same_type = skill_type_cfg.is_same_type
                        end
                        if self.skill_same_type_list[skill_type] == nil or self.skill_same_type_list[skill_type] < skill_type_cfg.is_same_type then
                            self.skill_same_type_list[skill_type] = skill_type_cfg.is_same_type
                        end
                    end
                end
            end
            self.gift_skill_list[v.gift_id] = 1
        end
        for i = 1, MAX_PET_EQUIPMENT_SKILL_NUM do
            if self.info.equipment_skill_list[i] ~= 0 and self.info.equipment_skill_list[i] ~= nil then
                local skill_id = self.info.equipment_skill_list[i]
                local level = AmuletData.Instance:GetIntensifySkillLevel(skill_id)
                local gift_cfg = self.data:GetGiftDetail(skill_id, level)
                if gift_cfg then
                    local skill_type_cfg = Cfg.SkillPassive(gift_cfg.param_1)
                    local skill_type = skill_type_cfg.skill_type
                    if skill_type_cfg.skill_type <= 4 then
                        skill_type = 0
                    end
                    if self.skill_same_type_list[skill_type] == nil or self.skill_same_type_list[skill_type] < skill_type_cfg.is_same_type then
                        self.skill_same_type_list[skill_type] = skill_type_cfg.is_same_type
                    end
                end
            end
        end
        if self:IsLianYao() and self.info.lian_yao_passive_list then
            for i, v in ipairs(self.info.lian_yao_passive_list) do
                if v.skill_id ~= 0 then
                    local gift_cfg = self.data:GetGiftDetail(v.skill_id)
                    if gift_cfg then 
                        local skill_type_cfg = Cfg.SkillPassive(gift_cfg.param_1)
                        if skill_type_cfg then
                            local skill_type = skill_type_cfg.skill_type
                            if skill_type_cfg.skill_type <= 4 then
                                skill_type = 0
                                self.element_gift_same_type = skill_type_cfg.is_same_type
                            end
                            if self.skill_same_type_list[skill_type] == nil or self.skill_same_type_list[skill_type] < skill_type_cfg.is_same_type then
                                self.skill_same_type_list[skill_type] = skill_type_cfg.is_same_type
                            end
                        end
                    end
                end
                self.gift_skill_list[v.skill_id] = 1
            end
        end

        -- for k,v in pairs(lower_skill_list) do
        --     if not self:hasBetterSkill(k) then
        --         self.pet_skill_score = self.pet_skill_score +  (skill_score_cfg.lower_pet_hitbook or 0)
        --     end
        -- end
        
        for k,v in pairs(self.skill_same_type_list) do
            local temp_score = skill_score_cfg.lower_pet_hitbook
            if v == 1 then
                temp_score = skill_score_cfg.high_pet_hitbook
            elseif v == 2 then
                temp_score = skill_score_cfg.super_pet_hitbook_1
            end
            self.pet_skill_score = self.pet_skill_score +  temp_score
        end
    end
end

function Pet:initConfigGiftList()
    local gift_list = {}
    if self.config then
        for i, v in ipairs(Split(self.config.gift_group_id, "|")) do
            local skill_id = self.data:GetGiftIdByGroupId(tonumber(v))
            local config = {}
            config.gift_id = skill_id
            config.gift_source = 0
            table.insert(gift_list, config)
        end
        for i = #gift_list + 1, PET_GIFT_MAX_COUNT do
            table.insert(gift_list,{gift_id = 0, gift_source = 0})
        end
    end
    return gift_list
end

function Pet:checkEquipSkillUseless(gift_id, level)
    if self.skill_same_type_list[0] ~= nil then
        local gift_cfg = self.data:GetGiftDetail(gift_id, level)
        if gift_cfg and gift_cfg.param_1 then
            local skill_type_cfg = Cfg.SkillPassive(gift_cfg.param_1)
            if skill_type_cfg and skill_type_cfg.skill_type and skill_type_cfg.skill_type <= 4 then
                if self.element_gift_same_type then --天赋列表中有元素技能时护符技能的元素技能视等级失效
                    return skill_type_cfg.is_same_type <= self.element_gift_same_type
                end
                return skill_type_cfg.is_same_type < self.skill_same_type_list[0]
            end
        end
    end
    
    return self.gift_skill_list[gift_id] == 1
end

function Pet:initPetEquipItem()
    if self.info.pet_equipment_id and self.info.pet_equipment_id ~= 0 then
        local id_is_ok = false
        for i,v in pairs(Config.pet_equipment_auto.refining) do
            if self.info.pet_equipment_id == v.id then
                id_is_ok = true
                break
            end
        end
        if not id_is_ok then
            LogError(string.format("宠物护符数据异常！！！Index = %s %s, 护符id = %s", self.index, self.name, self.info.pet_equipment_id))
            --self.info.pet_equipment_id = 0
            self.equip_item = Item.New()
            return
        end
        local item_t = {}
        item_t.item_id = self.info.pet_equipment_id
        item_t.param = {}
        item_t.param.equipment_skill_list = {}
        item_t.param.equipment_skill_refresh_list={}
        item_t.param.equipment_skill_num=self.info.equipment_skill_num
        item_t.param.pet_index=self.index
        for i=1, MAX_PET_EQUIPMENT_SKILL_NUM do
            item_t.param.equipment_skill_list[i] = self.info.equipment_skill_list[i]
            item_t.param.equipment_skill_refresh_list[i] = self.info.equipment_skill_refresh_list[i]
        end
        self.equip_item = SmartData.New(Item.New(item_t))
    end
end
--改命信息
function Pet:GetGaiMingInfo()
    return self.info.pet_gai_ming_info
end
--剩余改命点
function Pet:SurplusGaiMingPoint()
    local info = self:GetGaiMingInfo()
    local total = info.level
    for i, used in ipairs(info.attr_level) do
        total = total - used
    end
    return total
end
--改命是否满级
function Pet:IsMaxGaiMing()
    local info = self:GetGaiMingInfo()
    return info.level == PetData.Instance:GaiMingMaxLevel()
end
function Pet:GetPetEquipItem()
    return self.equip_item
end
--是否已炼妖过
function Pet:IsLianYao()
    return self.info.lian_yao_pet_id and self.info.lian_yao_pet_id > 0
end
function Pet:YaoPetId()
    return self.info.lian_yao_pet_id
end

function Pet:YaoPet()
    if self:IsLianYao() then
        if not self.yao_pet then
            local info = TableCopy(self.info)
            info.pet_id = self.info.lian_yao_pet_id
            info.str_level = self.yao_str_level
            local pet = Pet.New(info)
            pet.name = pet:CfgName()
            self.yao_pet = pet
        end
        return self.yao_pet
    else
        return self
    end
end

-- 天赋信息(仅显示洗炼出来的，不包括领悟的,带空格子)
function Pet:GiftList()
    local res = {}
    local gift_list = self.info.gift_list
    for i = 1, 11 do
        local t = PetData.GetEmptySkillData(self:Index())
        if gift_list[i] ~= nil and gift_list[i].gift_id ~= nil and gift_list[i].gift_id ~= 0 and gift_list[i].gift_source ~= 1 then
            t.skill_id = gift_list[i].gift_id
            t.skill_level = gift_list[i].skill_level
        end
        table.insert(res, t)
    end
    return res
end

-- 天赋信息（仅有已经学习的技能即 gift_id ~= 0）
function Pet:GiftListNotEmpty()
    if self.gift_list_not_empty == nil then
        self.gift_list_not_empty = {}
        for k,v in pairs(self.info.gift_list) do
            if v.gift_id ~= 0 then
                table.insert(self.gift_list_not_empty,v)
            end
        end
    end
    return self.gift_list_not_empty
end

-- 洗炼出的天赋技能(还未保存的)
function Pet:NewGiftList()
    local gift_list = {}
    for i = 1, self.config.gift_max do
            local gt = PetData.GetEmptySkillData(self:Index())
            if self.info.new_gift_list[i] ~= nil and self.info.new_gift_list[i] ~= 0 then
                gt.skill_id = self.info.new_gift_list[i]
                gt.skill_level = 1
            end
            table.insert(gift_list, gt)
        end
    return gift_list
end

--主动技能列表
function Pet:SkillList()
    if self.had_skill_list == nil then
        self.had_skill_list = {}
        for i=1, #self.info.skill_list do
            local st = {}
            st.skill_id = self.info.skill_list[i]
            st.skill_level = self.str_level or 1
            st.show_lingwu = false
            st.main_skill = true
            st.pet_qua = self:Quality()
            st.mutant_lv = self:MutTimes()
            table.insert(self.had_skill_list, st)
        end
    end
    return self.had_skill_list
end



--获取专属天赋列表(配置)
function Pet:GetConfigGiftList(gift_t)
    local list = gift_t or {}
    for i, v in ipairs(Split(self.config.gift_group_id, "|")) do
        local skill_id = self.data:GetGiftIdByGroupId(tonumber(v))
        local config = self.data:GetGiftDetail(skill_id)
        config.skill_id = skill_id
        table.insert(list, config)
    end
    return list
end

function Pet:HasLearned(skill_id)
    local list = self:GetSkillAndGiftListNoEquip()
    for k, v in pairs(list) do
        if v.skill_id ~= 0 and skill_id == v.skill_id then
            return true
        end
    end
    return false
end

--宠物技能书学习界面
function Pet:GetLearnGiftViewList()
    local gift_list = {}
    local has_skill_count = 0
    for i = 1, self.config.gift_max do
        local gt = PetData.GetEmptySkillData(self:Index())
        if self.info.gift_list[i] and self.info.gift_list[i].gift_id ~= 0 then
            gt.skill_id = self.info.gift_list[i].gift_id
            gt.useLess = self:hasBetterSkill(gt.skill_id)
            gt.index = i
            gt.skill_level = self.info.gift_list[i].skill_level
            table.insert(gift_list, gt)
            has_skill_count = has_skill_count + 1
        --[[ else
            gt.show_lingwu = true
            gt.index = i
            table.insert(gift_list, gt) ]]
        end
    end
    gift_list = self:LianYaoSkillList(gift_list)
    --[[ for _, value in ipairs(gift_list) do
        if value.skill_id > 0 then
            
        end
    end ]]
    return gift_list, has_skill_count
end

--打书学习的技能信息（尚未保存的）
function Pet:NewGiftData()
    local new_list, has_skill_count = self:GetLearnGiftViewList()
    if self.info.replace_skill_id ~= 0 then
        local new_skill_t = PetData.GetEmptySkillData(self:Index())
        new_skill_t.skill_id = self.info.replace_skill_id
        new_skill_t.skill_level = 1
        -- new_skill_t.useLess = self:hasBetterSkill(new_skill_t.skill_id)
        new_skill_t.index = self.info.replace_index
        new_skill_t.replace_type = self.info.replace_type
        new_skill_t.new_flag = true
        new_skill_t.yao_index = has_skill_count + self.info.replace_index
        return new_skill_t
    end
end

--所有技能的信息列表(有领悟有空技能) 仅供宠物系统打书界面使用！！！！！
function Pet:GetHitBookSkillAndGiftList()
    if self.info.skill_list == nil then
        return {}    
    end

    self.skill_hitbook_gift_list = {}
    --主动技能
    for i = 1, 2 do
        local st = {}
        if self.info.skill_list[i] then
            st.skill_id = self.info.skill_list[i]
            st.skill_level = self.str_level or 1
            st.main_skill = true
            st.pet_qua = self:Quality()
            st.mutant_lv = self:MutTimes()
            table.insert(self.skill_hitbook_gift_list, st)
            --修改为空的不占用
        end
        --[[ else
            st = PetData.GetEmptySkillData(self:Index())
        end ]]       
    end
    if self.info.lian_yao_pet_id and self.info.lian_yao_pet_id > 0 then
        local cfg = self.data:GetPetCfg(self.info.lian_yao_pet_id)
        if cfg then
            for i = 1, cfg.skill_num do
                local st = {}
                st.skill_id = cfg["skill_id_"..tostring(i)]
                st.skill_level = self.info.lian_yao_pet_str_lv or 1
                st.show_lingwu = false
                st.main_skill = true
                st.lian_yao_skill = true
                st.pet_qua = self:Quality()
                st.mutant_lv = self:YaoMutTimes()
                st.pet_idx = self:Index()
                table.insert(self.skill_hitbook_gift_list, st)
            end
        end
    end

    self:fillFeiShengSkillData(self.skill_hitbook_gift_list)
    local has_skill_count = 0
    --天赋技能（被动技能）
    local lingwu_red = PetData.Instance:CheckPetLingWuRemind(self.index)
    for i = 1, self.config.gift_max do
        local gt = PetData.GetEmptySkillData(self:Index())
        if self.info.gift_list[i] and self.info.gift_list[i].gift_id ~= 0 then
            gt.skill_id = self.info.gift_list[i].gift_id
            gt.useLess = self:hasBetterSkill(gt.skill_id)
            gt.index = i
            gt.skill_level = self.info.gift_list[i].skill_level
            has_skill_count = has_skill_count + 1
            table.insert(self.skill_hitbook_gift_list, gt)
        elseif has_skill_count ~= 0 then
            gt.show_lingwu = true
            gt.lingwu_red = lingwu_red
            gt.index = has_skill_count + 1
            table.insert(self.skill_hitbook_gift_list, gt)
        end
    end
    --装备技能
    self.skill_hitbook_gift_list = self:GetEquipSkillList(self.skill_hitbook_gift_list)
    --炼妖
    self.skill_hitbook_gift_list = self:LianYaoSkillList(self.skill_hitbook_gift_list, true)
    return self.skill_hitbook_gift_list
end

--已经学会的所有技能的信息列表(无领悟无空技能无装备技能)
function Pet:GetSkillAndGiftListNoEquip()
    if self.info.skill_list == nil then
        return {}
    end
    if self.skill_gift_list_noequip == nil then
        --主动技能
        self.skill_gift_list_noequip = self:GetMainSkillList({})
        --天赋技能（被动技能）
        self.skill_gift_list_noequip = self:GetLearnedGiftList(self.skill_gift_list_noequip)
    end
    return self.skill_gift_list_noequip
end

--已经学会的所有技能的信息列表(无领悟无空技能)
function Pet:GetSkillAndGiftListNoEmpty()
    if self.info.skill_list == nil then
        return {}
    end

    --主动技能
    self.skill_gift_list_noempty = self:GetMainSkillList({})
    self:fillFeiShengSkillData(self.skill_gift_list_noempty)
    --天赋技能（被动技能）
    self.skill_gift_list_noempty = self:GetLearnedGiftList(self.skill_gift_list_noempty, true)
    --装备技能（符）
    self.skill_gift_list_noempty = self:GetEquipSkillList(self.skill_gift_list_noempty)
    --炼妖技能
    self.skill_gift_list_noempty = self:LianYaoSkillList(self.skill_gift_list_noempty)
    return self.skill_gift_list_noempty
end

--炼妖技能
function Pet:LianYaoSkillList(list, is_lingwu)
    if self:IsLianYao() then
        local max_num = PetRefiningData.Instance.other_info.addskill_max
        for index, skill in ipairs(self.info.lian_yao_passive_list) do
            if (skill.skill_id > 0 or is_lingwu) and index <= max_num then
                local st = {}
                st.skill_id = skill.skill_id
                st.skill_level = skill.skill_level + 1---== 0 and 1 or skill.skill_level
                st.show_lingwu = skill.skill_id == 0
                st.pet_qua = self:Quality()
                st.is_yao_skill = true
                st.pet_idx = self:Index()
                st.index = index
                st.new_flag = skill.new_flag or false
                st.useLess = self:checkLianYaoSkillUseless(st)
                st.useLess = st.useLess and st.useLess or self:hasBetterSkill(st.skill_id)
                table.insert(list, st)
            end
        end
    end
    return list
end
function Pet:checkLianYaoSkillUseless(skill)
    for k,v in pairs(self.info.gift_list) do
        if skill.skill_id > 0 and v.gift_id == skill.skill_id then
            return true
        end
    end
    local skill_id

    -- #57594 缺陷 G6-LineR（版本发布线） 超级书显示问题-客户端

    -- for i = 1, MAX_PET_EQUIPMENT_SKILL_NUM do
    --     skill_id = self.info.equipment_skill_list[i]
    --     if skill_id > 0 and skill.skill_id == skill_id then
    --         return 1 >= skill.skill_level
    --     end
    -- end
    
    for i, v in ipairs(self.info.lian_yao_passive_list) do
       if v.index and v.skill_id > 0 and v.skill_id == skill.skill_id then
           return skill.index > v.index
       end
    end
    return false
end

function Pet:fillFeiShengSkillData(list)
    if self:FeiShengSkillId() > PetFeiShengData.Lv.None then
        local st = {}
        st.skill_id = self:FeiShengSkillId()
        st.skill_level = self:FeiShengSkillLv()
        st.show_lingwu = false
        st.pet_qua = self:Quality()
        st.fs_skill = true
        st.pet_idx = self:Index()
        table.insert(list, st)
    end
end

--已经学会的所有技能的信息列表(有领悟无空技能)(pet_xilian)
function Pet:GetSkillAndGiftListNoEmptyWithLingWu(lianyao_lingwu)
    if self.info.skill_list == nil then
        return {}
    end
    --主动技能
    self.skill_gift_list_noempty_lingwu = self:GetMainSkillList({})
    self:fillFeiShengSkillData(self.skill_gift_list_noempty_lingwu)
    --天赋技能（被动技能）
    local lingwu_red = PetData.Instance:CheckPetLingWuRemind(self.index)
    self.skill_gift_list_noempty_lingwu = self:GetLearnedGiftList(self.skill_gift_list_noempty_lingwu ,true, true, lingwu_red, true)
    --装备技能（符）
    self.skill_gift_list_noempty_lingwu = self:GetEquipSkillList(self.skill_gift_list_noempty_lingwu)

    self.skill_gift_list_noempty_lingwu = self:LianYaoSkillList(self.skill_gift_list_noempty_lingwu, lianyao_lingwu)

    return self.skill_gift_list_noempty_lingwu
end

--目前用于洗炼界面右侧显示（当勾选了只洗资质时）
function Pet:GetSkillAndGiftListNoLingWu()
    if self.info.skill_list == nil then
        return {}
    end
    --主动技能
    local skill_gift_list_no_lingwu = self:GetMainSkillList({})
    self:fillFeiShengSkillData(skill_gift_list_no_lingwu)
    --天赋技能（被动技能）
    skill_gift_list_no_lingwu = self:GetLearnedGiftList(skill_gift_list_no_lingwu)
    --装备技能（符）
    skill_gift_list_no_lingwu = self:GetEquipSkillList(skill_gift_list_no_lingwu)

    skill_gift_list_no_lingwu = self:LianYaoSkillList(skill_gift_list_no_lingwu)

    return skill_gift_list_no_lingwu
end
--护符技能
function Pet:GetEquipSkillList(gift_t)
    for i=1, MAX_PET_EQUIPMENT_SKILL_NUM do
        if self.info.equipment_skill_list[i] ~= nil and self.info.equipment_skill_list[i] > 0 then
            local gt = PetData.GetEmptySkillData(self:Index())
            gt.skill_id = self.info.equipment_skill_list[i]
            gt.is_fu_skill = true
            gt.skill_level = AmuletData.Instance:GetIntensifySkillLevel(self.info.equipment_skill_list[i]) or 1
            gt.useLess = self:checkEquipSkillUseless(gt.skill_id, gt.skill_level)
            gt.useLess = gt.useLess and gt.useLess or self:hasBetterSkill(gt.skill_id,gt.skill_level)
            table.insert(gift_t,gt)
        end
    end
    return gift_t
end

--通过索引获得宠物技能数据
function Pet:GetGiftSkillDataByIndex(index, is_yao_skill)
    if is_yao_skill == true then
        for i, v in ipairs(self.info.lian_yao_passive_list) do
            if index == i and v.skill_id > 0 then
                local config = PetData.GetEmptySkillData(self:Index())
                config.skill_id = tonumber(v.skill_id)
                config.index = i
                config.skill_level = v.skill_level + 1
                config.is_yao_skill = true
                return config
            end
        end
        return PetData.GetEmptySkillData(self:Index())
    end
    for i, v in ipairs(self.info.gift_list) do
        if index == i and v.gift_id > 0 then
            local config = PetData.GetEmptySkillData(self:Index())
            config.skill_id = tonumber(v.gift_id)
            config.index = i
            config.skill_level = v.skill_level
            return config
        end
    end
    return PetData.GetEmptySkillData(self:Index())
end

function Pet:GetLearnedGiftList(gift_t, need_useless, need_empty, lingwu_red, xilian_new_flag)
    local has_skill_count = 0
    for i = 1, self.config.gift_max do
        local gt = PetData.GetEmptySkillData(self:Index())
        if self.info.gift_list[i] and self.info.gift_list[i].gift_id ~= 0 then
            gt.skill_id = self.info.gift_list[i].gift_id
            if need_useless then
                gt.useLess = self:hasBetterSkill(gt.skill_id)
            end
            if xilian_new_flag then
                gt.new_flag = PetData.Instance.xilian_new_flag and PetData.Instance.xilian_new_flag[gt.skill_id] == 1
            end
            has_skill_count = has_skill_count + 1
            gt.index = i
            gt.skill_level = self.info.gift_list[i].skill_level
            table.insert(gift_t, gt)
        elseif has_skill_count ~= 0 and need_empty then
            gt.show_lingwu = true
            gt.lingwu_red = lingwu_red
            gt.index = has_skill_count + 1
            table.insert(gift_t, gt)
        end
    end
    return gift_t
end

--当前已经学会的主动技能列表(宠物系统使用)新增了炼妖专属（单纯专属技能使用SkillList且不可修改）
function Pet:GetMainSkillList(gift_t, new_flag)
    for i = 1, #self.info.skill_list do
        local st = {}
        st.skill_id = self.info.skill_list[i]
        st.skill_level = self.str_level or 1
        st.show_lingwu = false
        st.main_skill = true
        st.pet_qua = self:Quality()
        st.mutant_lv = self:MutTimes()
        st.pet_idx = self:Index()
        table.insert(gift_t, st)
    end

    if self.info.lian_yao_pet_id and self.info.lian_yao_pet_id > 0 then
        local cfg = self.data:GetPetCfg(self.info.lian_yao_pet_id)
        if cfg then
            for i = 1, cfg.skill_num do
                local st = {}
                st.skill_id = cfg["skill_id_"..tostring(i)]
                st.skill_level = self.info.lian_yao_pet_str_lv or 1
                st.show_lingwu = false
                st.main_skill = true
                st.lian_yao_skill = true
                st.pet_qua = self:Quality()
                st.mutant_lv = self:YaoMutTimes()
                st.pet_idx = self:Index()
                st.new_flag = new_flag or false
                table.insert(gift_t, st)
            end
        end
    end

    return gift_t
end

--已经学会的主动技能
function Pet:HadSkillList()
    local res = {}
    local skill_list = self.info.skill_list
    if skill_list == nil then
        return res
    end
    for k, v in pairs(skill_list) do
        if v > 0 then
            local temp = {}
            temp.index = k - 1
            temp.skill_id = v
            temp.skill_cfg = self.data:GetSkillCfg(v)
            table.insert(res, temp)
        end
    end
    local index_k = res[#res].index
    if self.info.lian_yao_pet_id and self.info.lian_yao_pet_id > 0 then
        local cfg = self.data:GetPetCfg(self.info.lian_yao_pet_id)
        if cfg then
            for i = 1, cfg.skill_num do
                local st = {}
                st.index = index_k - i
                st.skill_id = cfg["skill_id_"..tostring(i)]
                st.skill_cfg = self.data:GetSkillCfg(st.skill_id)
                table.insert(res, st)
            end    
        end
    end
    return res
end

--配置表中的主动技能+天赋技能组
function Pet:GetSkillAndGiftGroupList()
    if self.skill_gift_config_list == nil then
        self.skill_gift_config_list = self:GetMainSkillList({})
        self.skill_gift_config_list = self:GetConfigGiftList(self.skill_gift_config_list)
    end
    return self.skill_gift_config_list
end

function Pet:hasBetterSkill(skill_id, level)
    if skill_id == 0 then
        return false
    end
    if self.skill_same_type_list == nil or not next(self.skill_same_type_list) then
        return false
    end
    local gift_cfg = self.data:GetGiftDetail(skill_id, level)

    if gift_cfg and (gift_cfg.page == nil or gift_cfg.page > 2) then
        return false
    end

    local low_passive_cfg = Cfg.SkillPassive(skill_id)
    if low_passive_cfg == nil then
        return false
    end
    if low_passive_cfg.skill_type <= 4 then --如果天赋列表中有低级元素技能并且装备上有高级的，那么低级的技能失效
        if self.skill_same_type_list[0] == nil then
            return false
        else
            return low_passive_cfg.is_same_type < self.skill_same_type_list[0]
        end
    else
        if self.skill_same_type_list[low_passive_cfg.skill_type] == nil then
            return false
        end
       return low_passive_cfg.is_same_type < self.skill_same_type_list[low_passive_cfg.skill_type]
    end
end

--今天天赋刷新次数
function Pet:TodayCount()
    return self.info.today_refresh_gift_count
end

function Pet:RefreshConfig()
    return self.data:GetGiftRefreshConfig(self:TodayCount())
end

function Pet:CheckState()
    local state = self:State()
    if state == PetState.Fight then
        PublicPopupCtrl.Instance:Center(ErrorText[10])
        return false
    end
    if state == PetState.StandBy then
        PublicPopupCtrl.Instance:Center(ErrorText[11])
        return false
    end
    if state == PetState.Train then
        PublicPopupCtrl.Instance:Center(ErrorText[12])
        return false
    end
    return true
end

-- 剩余点数
function Pet:PointNum()
    --总点数
    local cfg = self.data:GetPetOtherCfg()
    local d = cfg.att_point
    local t = cfg.start_att_point + (self.level - 1) * d + self:GetActiveSkinPointNum()
    local p = self:Point()
    local s = 0
    for _, v in pairs(p) do
        s = s + v
    end
    return t - s, s
end

function Pet:AutoAddInfo()
    local attr_index = 0
    for k,v in pairs(self.info.auto_add_points_flag) do
        if v ~= 0 then
            attr_index = k
            break
        end
    end
    local info = {index = attr_index}
    return info
end

--已激活的皮肤给的资质加点
function Pet:GetActiveSkinPointNum()
    local skin_point = 0
    local skin_list_cfg = PetData.Instance.pet_skin_cfg[self.pet_id]
    if skin_list_cfg ~= nil and next(skin_list_cfg) then
        for k,v in pairs(skin_list_cfg) do
            if PetData.Instance:GetSkinHasIsActive(v.pet_skin_id) then
                skin_point = skin_point + v.potential
            end
        end
    end
    return skin_point
end

--获取默认技能
function Pet:GetNormalSkillList()
    local skill_id = self.config.skill_id or 0
    local config = PetData.Instance:GetSkillConfig(skill_id)
    return {config}
end

--获取通用天赋 服务端下发数据
function Pet:GetHadGiftList()
    local list = {}
    for i, v in ipairs(self.info.gift_list) do
        if v.gift_id > 0 then
            local config = self.data:GetGiftDetail(tonumber(v))
            config.skill_id = tonumber(v.gift_id)
            config.index = i
            table.insert(list, config)
        end
    end
    return list
end
--图标
function Pet:IconId()
    if self:SkinIconId()then
        return self:SkinIconId()
    end
    return self.config and (self.config.icon_id or 0) or 0
end
--种族
function Pet:RaceId()
    local info = self:PetCfg()
    return info.race
end
--品质
function Pet:Quality()
    if self:PetCfg() then
        return self:PetCfg().quality
    else
        LogError("宠物数据有问题", self.pet_id, self.config)
        return 1
    end
end
--是否变异
function Pet:IsMutant()
    if self:PetCfg() then
        return self:PetCfg().mutant_times >= 1
    else
        return false
    end
end

--变异阶段
function Pet:MutTimes()
    return self:PetCfg().mutant_times
end
--炼妖变异阶段
function Pet:YaoMutTimes()
    return self.data:GetPetCfg(self.info.lian_yao_pet_id).mutant_times
end
--金宠及以上可以变异到二阶段
function Pet.CanMutantMaxLv(quality)
    return quality >= PetData.Quality.Jin and PetFeiShengData.ModUnlocked() and 
        PetData.MutantLv.Two or PetData.MutantLv.One
end

--金宠及以上才可以飞升
function Pet:CanFeiShengMaxLv()
    local quality = self:Quality()
    if quality >= PetData.Quality.Jin then
        for i = PetFeiShengData.Lv.Max,1,-1 do
            if PetFeiShengData.ModUnlocked(i) then
                return i
            end
        end 
        return PetFeiShengData.Lv.None
    else
        return PetFeiShengData.Lv.None
    end
end

--可以强化到的最高等级
function Pet.StrengthMaxLv(quality)
    if quality < PetData.Quality.Jin then
        return MAX_PET_STRENGTHEN_LV_NORMAL
    elseif PetFeiShengData.ModUnlocked(PetFeiShengData.Lv.Two) then
        return MAX_PET_STRENGTHEN_LEVEL
    else PetFeiShengData.ModUnlocked(PetFeiShengData.Lv.One)
        return MAX_PET_STRENGTHEN_FlY_NOE
    end
    return MAX_PET_STRENGTHEN_LV_NORMAL
end

--是否可以继续变异
--飞升等级为1及以上时可以看是否能二段变异
function Pet.CanMutant(quality,cur_mut_lv,fs_lv)
    local mut_max = fs_lv >= PetFeiShengData.Lv.One and Pet.CanMutantMaxLv(quality) or PetData.MutantLv.One
    return cur_mut_lv < mut_max
end

--飞升技能解锁的条件是飞升等级
function Pet.CanLvupGift(fs_lv)
    return fs_lv >= PetFeiShengData.Lv.One
end
function Pet:CanFlyup(check_state)
    if check_state == true and self:State() ~= PetState.Fight and self:State() ~= PetState.StandBy then
        return false
    end
    local target_fs_lv = self:FeiShengTimes() + 1
    if PetFeiShengData.IsAllPreCom(self:Index(),target_fs_lv) == false then
        return false
    end
    local can_feisheng = self:FeiShengJieDuan() / PetFeiShengData.STAGE_COUNT >= target_fs_lv
    if can_feisheng then
        return true
    end
end
--捕捉类型
function Pet:CatchType()
    if self:PetCfg() then
        return self:PetCfg().catch_type
    else
        LogError("宠物数据有问题", self.pet_id, self.config)
        return 1
    end
end
function Pet:QuaIcon()
    return PetData.PinZhi[self:Quality()]
end
function Pet:CatchWay()
    return self:PetCfg().catch_way
end

function Pet:MaxBp()
    local pet_cfg = self:PetCfg()
    if pet_cfg == nil then
        LogError("宠物数据有问题 未找到配置：", self.pet_id)
        return
    end
    local total = pet_cfg.con + pet_cfg.str + pet_cfg.def + pet_cfg.agi + pet_cfg.magic
    return total
end

function Pet:GetScore()
    local pet_score = 0
    if self.info.capaility and self.info.capaility ~= 0 then
        pet_score = self.info.capaility
    else
        local total_attr = self:TotalAttr()
        local pet_cfg = self:PetCfg()
        if pet_cfg == nil then
            LogError("宠物数据有问题 未找到配置：", self.pet_id)
            return
        end
        if self.score and self.score_level then
            return math.floor(self.score), self.score_level
        end
        --属性的评分
        local attack_rate1 = 1 + pet_cfg.bisha_dmg * 0.00081 + (pet_cfg.bisha + pet_cfg.jianren) * 0.000242 + pet_cfg.fanji * 0.0054
        local attack_rate2 = 1 + (pet_cfg.mingzhong + pet_cfg.shanduo) * 0.001
        local jingshen_rate = 1 + 1.2 * pet_cfg.mogong / (pet_cfg.mogong + 220) + 0.8 * pet_cfg.mokang / (pet_cfg.mokang + 300)
        pet_score =
            total_attr[BattleAttriType.MaxMp] * 1.6 + --蓝
            total_attr[BattleAttriType.MaxHp] * 0.25 + --血
            total_attr[BattleAttriType.Attack] * 1.6 * attack_rate1 * attack_rate2 + --攻击
            total_attr[BattleAttriType.Defense] * 1.2 + --防御
            total_attr[BattleAttriType.Mental] * 2.16 * jingshen_rate + --精神
            total_attr[BattleAttriType.Recovery] * 3.63 + --恢复
            total_attr[BattleAttriType.Agility] * 1.85 --敏捷
            
        --技能的评分在宠物实例初始化时完成计算（initBetterSkill）
        pet_score = pet_score + self.pet_skill_score
    end
    local score_level_cfg = Cfg.PetScoreLevel(RoleData.Instance:GetRoleLevel())
    local score_level = 0
    if score_level_cfg ~= nil then
        if pet_score >= score_level_cfg.score_s then
            score_level = 3
        elseif pet_score >= score_level_cfg.score_a then
            score_level = 2
        elseif pet_score >= score_level_cfg.score_b then
            score_level = 1
        end
    end
    self.score = pet_score
    self.score_level = score_level
    return math.floor(pet_score), score_level
end

function Pet:IsNeedBreach(breach_list)
    if self.info.is_up_level_breach ~= nil and self.info.is_up_level_breach == 0 then
        breach_list = breach_list or PetData.Instance:GetBreachList(self)
        for i,v in pairs(breach_list) do
            if v.level == self:Level() then
                return true
            end
        end
        return false
    else
        return false
    end
end

function Pet:LimitType()
    return self:PetCfg().limit_type or 0
end

-- 被灵骑统御的信息 没有信息则会返回空的，外部通过petData设置的
function Pet:Bicycle()
    return self.bicycle_info    -- {bicycle_index,cell_index[0/1]}
end

-- 宠物使用了哪个魂器 传{}等于卸下
function Pet:Weapon(weapon)
    if weapon then
        self.weapon = weapon
    end
    if self.weapon and not next(self.weapon) then
        self.weapon = nil
    end
    return self.weapon
end
function Pet:Weapon2(weapon)
    if weapon then
        self.weapon2 = weapon
    end
    if self.weapon2 and not next(self.weapon2) then
        self.weapon2 = nil
    end
    return self.weapon2
end

-- 专属魂器 默认1级
function Pet:ExclusiveWeapon()
    local pet_id = self:ID()
    if self:IsMutant() then
        pet_id = self:NormalId()
    end
    local weapon = PetWeaponData.Instance:GetExclusiveWeaponByPetId(pet_id)
    return weapon
end

-- 神印方案
function Pet:StampBlueprint()
    return self.stamp_blueprint or PetStampBlueprint.New(nil,nil,self.index)
end

-- 宠物徽章
function Pet:BagdeItem()
    return self.bagde_item
end

--获取某品质技能数量 table类型，根据page分页判断 flags[page]=true
function Pet:GetSkillNumByFlags(flags)
    if self.gift_list == nil then
        return 0
    end
    local match_num = 0
    for k, v in pairs(self.gift_list) do
        if v.gift_id ~= 0 then
            local gift_cfg = self.data:GetGiftDetail(v.gift_id)
            if gift_cfg and gift_cfg.page and flags[gift_cfg.page] then
                match_num = match_num + 1 
            end
        end
    end
    return match_num
end

function Pet:GiftMax()
    return self.config.gift_max
end

function Pet:PrayId()
    return self.config.benediction_id or 0
end

-- 宠物身上祝福的主宠信息
function Pet:PrayMainIndex()
    return PetData.Instance:GetPrayMainPetIndex(self.index or -1)
end

-- 宠物身上祝福的副宠列表
function Pet:PrayVicePetList()
    return PetData.Instance:GetPrayVicePetList(self.index or -1)
end

-- 获取副宠祝福属性加成
function Pet:ViceAttriAddition()
    local pet_list = self:PrayVicePetList()
    if not pet_list then
        return {}
    end
    local attri_t = {}
    for i,v in pairs(pet_list) do
        local pet = PetData.Instance:GetPetInfo(v)
        local attri_data = PetData.Instance:GetPetAttrInfo(v)
        if attri_data == nil then
            --LogError("PetIndex", v, PetData.Instance.pet_attr_list)
            return {}
        end
        for _,attri_type in pairs(PetPrayAdditionAttriType) do
            if attri_t[attri_type] == nil then
                attri_t[attri_type] = 0
            end
            local scale = (pet:PetCfg().benediction_att or 10) / 100
            attri_t[attri_type] = attri_t[attri_type] + math.floor((attri_data[attri_type] * scale))
        end
    end
    local sort_attri_t = {}
    for i,v in pairs(PetPrayAdditionAttriType) do
        table.insert(sort_attri_t, {attri_type = v, attri_value = attri_t[v]})
    end
    return sort_attri_t
end

-- 宠物祝福效果配置
function Pet:PrayActiveCfg()
    local pet_list = self:PrayVicePetList()
    if self:PrayId() < 1 or not pet_list or #pet_list < 3 then
        return {}
    end
    -- local vice_pet_info = {}
    -- for i,v in pet_list do
    --     local pet = PetData.Instance:GetPetInfo(v)
    --     local pet_id = self:NormalId()
    --     local score = self:GetScore()
    --     table.insert(vice_pet_info, {[pet_id] = score})
    -- end
    local pary_cfg_group = PetData.Instance:GetPrayCfgGroup(self:PrayId())
    local cfg = {}
    for i,v in pairs(pary_cfg_group) do
        local is_active, active_info = PetData.Instance:CheckPrayCfgPet(v, pet_list)
        if is_active then
            cfg = v
        else
            return cfg
        end
    end
    return cfg
end

function Pet:CanSubmit()
    return PetState.Relax == self:State() and not self:ArenaState() and 1 == self.level and 1 == self.str_level
end

--飞升次数
function Pet:FeiShengTimes()
    if self.info == nil then
        return PetFeiShengData.Lv.None
    end
    return self.info.fei_sheng_times or self.info.fly_flag or PetFeiShengData.Lv.None
end

--当前飞升任务的完成阶段
function Pet:FeiShengJieDuan()
    return self.info.feisheng_jie_duan
end

function Pet:UniqueId1()
    return self.info.pet_unique_id1
end

function Pet:FeiShengSkillId()
    return self.info.fei_sheng_select_skill_id or 0--#15617653 LuaException
end

function Pet:FeiShengSkillLv()
    return self.info.fei_sheng_select_skill_level
end

function Pet:UniqueId2()
    return self.info.pet_unique_id2
end

-- 天赋信息
function Pet:InbornInfo(inborn_index)
    if inborn_index then
        return self.info.inborn_info_list[inborn_index] or {}
    end
    return self.info.inborn_info_list
end

-- 是否有进行天赋
function Pet:IsInborn()
    local inborn_type = self:PetCfg().inborn_type
    if not inborn_type or inborn_type == 0 then
        return false
    end
    local list = self:InbornInfo()
    for i,v in pairs(list) do
        if v.inborn_level > 0 then
            return true
        end
    end
    return false
end

-- 是否有进行共鸣
function Pet:IsReson()
    return PetResonanceData.Instance:PetIsReson(self)
end

-- 宠物被哪个圣器给保佑了 可为空
function Pet:ShengQi()
    local sheng_qi = ShengQiData.Instance:GetBlessShengQi(self)
    return sheng_qi
end

-- 宠物和哪个幽冥绑在了一起
function Pet:YouMing()
    local youming = YouMingData.Instance:GetBlessYouMing(self)
    return youming
end

--------------------------------------------------------------------------
--------------------------------------------------------------------------
PetData = PetData or BaseClass()

PetData.PetGrowthKey = {
    con = 1,
    str = 2,
    def = 3,
    agi = 4,
    magic = 5
}
PetData.PetLevel = {
    [1] = "_LocPetJibie1",
    [2] = "_LocPetJibie2",
    [3] = "_LocPetJibie3",
    [4] = "_LocPetJibie4",
    [5] = "_LocPetJibie5"
}
PetData.PetLevel2 = {
    [1] = "_LocJieType1",
    [2] = "_LocJieType2",
    [3] = "_LocJieType3",
    [4] = "_LocJieType4",
    [5] = "_LocJieType5"
}
PetData.PetLock = {
    [0] = "PetSuo2",
    [1] = "PetSuo1"
}
PetData.PetStrLevel = {
    [0] = "",
    [1] = "PetNum1",
    [2] = "PetNum2",
    [3] = "PetNum3",
    [4] = "PetNum4",
    [5] = "PetNum5",
    [6] = "PetNum6",
    [7] = "PetNum7",
    [8] = "PetNum8",
    [9] = "PetNum9",
    [10] = "PetNum10",
    [11] = "PetNum11",
    [12] = "PetNum12",
    [13] = "PetNum13",
    [14] = "PetNum14",
    [15] = "PetNum15",
}
PetData.PetJie = {
    [1] = "PetPinJie1",
    [2] = "PetPinJie2",
    [3] = "PetPinJie3",
    [4] = "PetPinJie4",
    [5] = "PetPinJie5"
}
PetData.ZiZhi = {
    [1] = "JinDuTiaoDi1",
    [2] = "JinDuTiaoDi2"
}
PetData.PinZhi = {
    "pet_qua_1",
    "pet_qua_2",
    "pet_qua_3",
    "pet_qua_4",
    "pet_qua_5"
}
-- 潜能点字段对应的宠物突破属性id
PetData.BreachAttriId = {
	con = 0,
	str = 1,
	def = 2,
	agi = 3,
	magic = 4,
}
PetData.LimitType = {
    Null = 0, -- 没有限定标志
    Cow = 1,  -- 牛年限定
    Tiger = 2,-- 虎年限定
    Champion = 3,   -- 冠军限定
    Rahi = 4,   -- 上古异兽
    Rabbit = 5, -- 兔年限定
}
PetData.LimitRes = {
    [PetData.LimitType.Cow] = {img = "_LocNiuNianXianDing",effect_id = "100108204"},
    [PetData.LimitType.Tiger] = {img = "_LocHuNianXianDing",effect_id = "3161056"},
    [PetData.LimitType.Champion] = {img = "_LocGuanJunXianDing",effect_id = "3165109"},
    [PetData.LimitType.Rahi] = {img = nil, effect_id = "3165110"},
    [PetData.LimitType.Rabbit] = {img = nil, effect_id = "2164002"},
}
PetData.GiftListItemFrameName = {
    "HuoBanPinZhiKuangLan",
    "HuoBanPinZhiKuangZi",
    "HuoBanPinZhiKuangCheng"
}

PetData.CatchWayJumpType = {
    JumpTrade = -1,     --跳转交易行（自定义添加）
    JumpNone = 0,       --暂未开放
    JumpScene = 1,      --场景抓取
    JumpShop = 2,       --商会
    JumpView = 3,       --某个系统界面
    JumpExchange = 5,   --兑换
    JumpActivity = 6,   --活动
    JumpNpc = 7,        --npc
}
PetData.Length = 200
PetData.MaxLevel = 12
PetData.SkillFactor = 10000

PetState = {
    Fight = 1,
    StandBy = 2,
    Relax = 3,
    Train = 4,      --训练
    Arena = 5,      --擂台
    Follow = 6,
    Pray = 7,       --祝福了别的宠物      
}

PetStateSort = {
    [PetState.Relax] = 1,
    [PetState.Train] = 2,
    [PetState.Arena] = 3,
    [PetState.Follow] = 4,
    [PetState.StandBy] = 5,
    [PetState.Fight] = 6,
}
-- 0默认 1是砸蛋 2是新手选择宠物界面 3是碎片合成 4是神兽兑换
PetGetReason = {
    DEFAULT = 0,
    PETEGG = 1,
    SPECIAL_PET = 2,
    COMPOSE_PET = 3,
    CONVERT_PET = 4,
    REFINED_MUTANT_PET = 5,             --洗炼获得变异宠物
    PET_BEFALL = 6,                     --神兽降临活动
    XUN_BAO_EXCHANGE = 7,               --寻宝置换
    ITEM_MUTANT_PET = 8,                --宠物丹变异
    MULTI_PIECES = 9,                   --宠物批量合成
    REALM_BREAK = 10,                   --心灵之境获得宠物
}
PetData.State = {
    [1] = "_LocZhan",
    [2] = "_LocDai",
    [3] = "",
    [4] = "_LocXun",
    [5] = "_LocLei"
}
PetData.ClickItem = 1
PetData.ProLength = 166
PetData.Race = 1
PetData.Attr = 2
PetData.Money = {
    [1] = CommonItemId.Coin,
    [2] = 65533,
    [3] = CommonItemId.Gold
}
PetData.TypeBtnList = {
    Race = {
        {-1, PetData.Race},
        {0, PetData.Race},
        {1, PetData.Race},
        {2, PetData.Race},
        {3, PetData.Race},
        {4, PetData.Race},
        {5, PetData.Race},
        {6, PetData.Race},
        {7, PetData.Race},
        {8, PetData.Race}
    },
    Attr = {
        {-1, PetData.Attr},
        {1, PetData.Attr},
        {2, PetData.Attr},
        {3, PetData.Attr},
        {4, PetData.Attr},
        {5, PetData.Attr}
    }
}
--变异阶段
PetData.MutantLv = {
    Zero = 0,
    One = 1,    --一变
    Two = 2,    --二变
}

--宠物品质
PetData.Quality = {
    Tong = 1,   --铜
    Yin = 2,    --银
    Jin = 3,    --金
    Xiyou = 4,  --稀有
    Shen = 5,   --神兽
}

-- 宠物属性配置 -- name 去Language.PetOtherAttriNames找
PetOtherAttriCfg = {
    {attr_type = 19, is_percent = false},      -- name = "法攻:"
    {attr_type = 20, is_percent = false},      -- name = "法防:"
    {attr_type = 15, is_percent = true},       -- name = "必杀:"
    {attr_type = 16, is_percent = true},       -- name = "命中:"
    {attr_type = 17, is_percent = true},       -- name = "闪避:"
    {attr_type = 18, is_percent = true},       -- name = "反击:"
    {attr_type = 22, is_percent = true},       -- name = "坚韧:"
    {attr_type = 27, is_percent = true},       -- name = "法爆:"
    {attr_type = 28, is_percent = true},       -- name = "法爆伤害:"
    {attr_type = 21, is_percent = true},       -- name = "必杀伤害:"
    {attr_type = 30, is_percent = true},       -- name = "中毒抗性:"
    {attr_type = 31, is_percent = true},      -- name = "石化抗性:"
    {attr_type = 32, is_percent = true},      -- name = "昏睡抗性:"
    {attr_type = 33, is_percent = true},      -- name = "混乱抗性:"
    {attr_type = 34, is_percent = true},      -- name = "酒醉抗性:"
    {attr_type = 35, is_percent = true},      -- name = "沉默抗性:"
    {attr_type = 38, is_percent = true},      -- name = "重伤抗性:"
    {attr_type = 11, is_percent = true},      -- name = "物理增伤:"
    {attr_type = 12, is_percent = true},      -- name = "法术增伤:"
    {attr_type = 9, is_percent = true},       -- name = "物伤减免:"
    {attr_type = 10, is_percent = true},      -- name = "法伤减免:"
    {attr_type = 23, is_percent = true},      -- name = "效果命中:"
    {attr_type = 26, is_percent = true},      -- name = "效果抵抗:"
    {attr_type = 24, is_percent = false},     -- name = "真实伤害:"
}

-- 宠物口粮对应市场口粮item_id
PetExpItemMarketData = {
    [26391] = 22059,
    [26392] = 22059,
    [26393] = 22060,
    [26394] = 22061,
}

-- 宠物祝福加成的属性类型
PetPrayAdditionAttriType = {6,8,0,1,2,3,4}

PetData.AuditShowPetId = {
    [50128] = 1,
    [50132] = 1,
    [50145] = 1,
}

function PetData:__init()
    if PetData.Instance ~= nil then
        Debuger.LogError("[PetData] attempt to create singleton twice!")
        return
    end
    PetData.Instance = self
    self.pet_cfg_auto = Config.pet_cfg_auto
    self.bookup_price_cfg = self.pet_cfg_auto.bookup_price
    self.pet_other_cfg = self.pet_cfg_auto.other[1]
    self.pet_gift_cfg = self.pet_cfg_auto.pet_gift
    self.pet_skill_desc = Config.skill_description_auto.pet_skill_description
    self.pet_strengthen = self.pet_cfg_auto.pet_strengthen
    self.pet_strengthen_buff = self.pet_cfg_auto.pet_strengthen_buff
    self.pet_gift_refresh = self.pet_cfg_auto.pet_gift_refresh
    self.pet_catch_way = self.pet_cfg_auto.catch_way
    self.pet_gai_ming_cfg = Config.pet_remould_auto
    self.fragment_item_id = {[1] = self.pet_other_cfg.fragment_item_id, [2] = self.pet_other_cfg.rare_fragment_item_id}
    self.pet_cfg_list = {}
    self.pet_shenshou_list = {}
    self.pet_shenshou_convert_list = {}
    self.pet_list = SmartData.New()
    self.pet_storage_list= SmartData.New()
    self.pet_other_info = SmartData.New()
    self.pet_train_list = {}
    self.pet_attr_list = {}
    --技能
    self.pet_skill_config = {}

    self.pet_notice_no_type = nil

    self.pet_select = SmartData.New({index = -1})
    self.pet_strengthen_level = 1
    -- 图鉴
    self.pet_info_race = {}
    self.pet_info_quality = {}
    self.pet_book_cell = SmartData.New()
    self.pet_race_index = -1
    self.pet_attr_index = -1
    -- 强化提示
    self.pet_str_alert = true
    self.pet_strengthen_detail_str_level = 1
    self.strength_use_protect = 0
    
    --宠物强化选择的宠物
    self.sel_pet_str_data = SmartData.New({pet_index == nil, info = {}, click_index = -1, is_yao = false})

    -- 天赋
    self.pet_gift_index = 0
    -- 选择学习的技能
    self.pet_skill_select = SmartData.New()
    self.pet_gift_replace_s = SmartData.New({lockable = false, jump_anim = false})
    self.exp_item_list = {}
    -- 封印卡
    self.card_list = {}
    -- 新宠
    self.new_pet = nil
    self.new_pet_smart = SmartData.New()
    self.shop_pet_info =
        SmartData.New(
        {
            item_count = 0,
            item_list = {}
        }
    )
    --宠物三选一界面使用
    self.guide_pet_select_cur = SmartData.New({seq = 0, show_pet = 0, is_info_view = false})
    --打书界面配置
    self.hit_book_type = SmartData.New({index = 0, suggest_skill_id = 0, tip_pro_index = 3})
    self.hit_book_page_list = {}
    --洗炼界面
    self.xilian_data = SmartData.New({show_suggest = false,click_save = false, gift_only = false})
    self.cur_sel_detail = SmartData.New()
    --宠物属性界面皮肤
    self.cur_select_skin = SmartData.New({id = -1, try_skin = false, use_skin = false})
    --宠物百科界面皮肤
    self.cur_book_skin = SmartData.New({id = -1, try_skin = false, use_skin = false})
    self.pet_has_skin_list = SmartData.New()

    -- 临时存放宠物皮肤是否激活表
    self.temp_pet_has_skin_list = SmartData.New()

    self.pet_skin_count = -1
    --宠物组
    self.pet_group_list = {}

    self:init_config()

    --宠物装备
    self.pet_equip_select = SmartData.New({item_type = 1, skill_type = 0, item_index = -1, equip = 1})--equip界面按钮显示文字 1、装备 2、替换 3、合成 4、强化

    self.pet_equipment_remind = SmartData.New({flag = false})

    self.pet_lingwu_remind = SmartData.New({flag = false})
    
    --主界面宠物突破提醒红点刷新源
    self.pet_breach_remind_data = SmartData.New({value = false})

    --宠物突破属性
    self.pet_breach_info = SmartData.New({info = {}})

    --超书置换数据
    self.pet_book_change_data = SmartData.New({info = {}, value = false})

    --副宠列表信息  副宠列表，主宠列表
    self.pray_info_data = SmartData.New({vice_pet_list = {}, pet_list = {}})
    --祝福旧数据：因为祝福信息是单独的协议，需要新祝福数据和就祝福数据进行比较
    self.old_pray_info_data = {}

    --设置选中指定宠物
    self.TargetPet = SmartData.New({pet_index = -1})

    --设置选中的标签页
    self.select_tab_data = SmartData.New({mod_key = -1})

    --宠物徽章红点是否提醒标记
    self.bagde_remind_flag = true

    self.show_strengthen_remind = {}
end

function PetData:Clear()
    self.pet_skin_count = -1
    self.pet_list:Set({})
    self.pet_other_info:Set({})
    self.bagde_remind_flag = true
    self.show_strengthen_remind = {}
end

function PetData:init_config()

    for k,v in ipairs(Config.skill_description_auto.pet_skill_description) do
        local rates_t = string.split(tostring(v.skill_value), "|")
        xpcall(function()
            v.description = string.format(v.description, unpack(rates_t))
        end,
        function (err)
            if IS_EDITOR then
                LogError("skill_id",v.skill_id,"skill_level", v.skill_level)
                LogError("hideAttrs ErrorLog:"..err)
            end
        end)
    end

    self.pet_gift_list = {}
    for k,v in pairs(self.pet_cfg_auto.pet_gift_list) do
        self.pet_gift_list[v.gift_id] = v
    end
    self.pet_gift_detail = {}
    for k, v in pairs(self.pet_gift_list) do
        self.hit_book_page_list[v.page] = self.hit_book_page_list[v.page] or {}
        table.insert(self.hit_book_page_list[v.page], v)
        self.pet_gift_detail[v.param_1] = v
    end
    self.gift_group = {}
    for k, v in pairs(self.pet_cfg_auto.gift_group) do
        self.gift_group[v.gift_group_id] = v
    end

    self.attr_exc = {}
    for i,v in ipairs(self.pet_cfg_auto.attributes_exchange) do
        self.attr_exc[v.skill_level] = self.attr_exc[v.skill_level] or {}
        table.insert(self.attr_exc[v.skill_level],v)
    end
    self.pet_gift_detail_cache = {} --key1:skill_id,key2:skill_level
    
    self.allPetSortedList = {}
    self.mutant_pet_list = {}
    self.mutant2_pet_list = {}
    self.normal_pet_list = {}
    for key, value in pairs(self.pet_cfg_auto.pet_info_2) do
        local pet = Pet.New(value)
        self.pet_cfg_list[key] = pet
        self.allPetSortedList[value.sort] = pet
        if value.quality == 5 then
            table.insert(self.pet_shenshou_list, self.pet_cfg_list[key])
        end
        if value.mutant_times == PetData.MutantLv.One then
            self.mutant_pet_list[value.prototype_id] = self.pet_cfg_list[key]
        elseif value.mutant_times >= PetData.MutantLv.Two then
            self.mutant2_pet_list[value.prototype_id] = self.pet_cfg_list[key]
        elseif value.is_show == 1 then
            self.normal_pet_list[key] = self.pet_cfg_list[key]
        end
    end

    self.pet_skin_cfg = {}
    for k,v in pairs(Config.skin_auto.pet_skin) do
        if v.is_display == 1 then
            local pets = string.split(v.pet_id,"|")
            for i, j in pairs(pets) do
                self.pet_skin_cfg[tonumber(j)] = self.pet_skin_cfg[tonumber(j)] or {}
                table.insert(self.pet_skin_cfg[tonumber(j)], v)
            end
        end
    end

    self.pet_strengthen_cfg = {}
    for k,v in pairs(self.pet_strengthen) do
        self.pet_strengthen_cfg[v.strengthen_id] = self.pet_strengthen_cfg[v.strengthen_id] or {}
        self.pet_strengthen_cfg[v.strengthen_id][v.level] = v
    end

    self.pet_get_way_info = {}
    local get_way_t = Split(self.pet_other_cfg.pet_get_way,"|")
    for k,v in pairs(get_way_t) do
        table.insert(self.pet_get_way_info, Config.getway_cfg_auto.get_way[tonumber(v)])
    end
    --宠物技能推荐分表
    local skill_advise = {}
    for k,v in pairs(self.pet_cfg_auto.skill_advise) do
        skill_advise[v.prototype_id] = skill_advise[v.prototype_id] or {}
        skill_advise[v.prototype_id][v.skill_way] = v
        skill_advise[v.prototype_id][v.skill_way].gift_id_list = {}
        local skill_show = string.split(v.skill_show,"|")
        for i,j in ipairs(skill_show) do
            local passive_cfg = Cfg.SkillPassive(tonumber(j))
            local skill_info = Cfg.SkillPassiveByType(passive_cfg.skill_type, v.show_type)
            if skill_info ~= nil then
                table.insert(skill_advise[v.prototype_id][v.skill_way].gift_id_list, {skill_id = skill_info.skill_id})
            else
                table.insert(skill_advise[v.prototype_id][v.skill_way].gift_id_list, {skill_id = j})
            end
        end
    end
    self.pet_cfg_auto.skill_advise = skill_advise

    local strengthen_protect = {}
    for k,v in pairs(self.pet_cfg_auto.strengthen_protect) do
        strengthen_protect[v.quality] = strengthen_protect[v.quality] or {}
        strengthen_protect[v.quality][v.strengthen_level] = v
    end
    self.pet_cfg_auto.strengthen_protect = strengthen_protect
    self.pet_egg_compose = {}
    for k, v in pairs(Config.compose_auto.compose_list) do
        if v.group == ComposeGroup.PetEgg then
            self.pet_egg_compose[v.stuff_id_1] = v
        end
    end
    Cfg.Register("PetSkinAttr",Cfg.RealTimeSearch(Config.skin_auto.pet_skin,"pet_skin_id"))
    Cfg.Register("PetGetWay", Cfg.RealTimeSearch(Config.pet_cfg_auto.catch_way, "way_type"))
    Cfg.Register("PetComposeInfo", Cfg.RealTimeSearch(Config.pet_cfg_auto.pet_compose,"patch_id"))
    Cfg.Register("PetComposeInfoSeq", Cfg.RealTimeSearch(Config.pet_cfg_auto.pet_compose,"seq"))
    Cfg.Register("PetBreachAttriInfo", Cfg.CacheGet2Key(Config.pet_cfg_auto.breach_attributes,"breach_level","id"))
    Cfg.Register("PetStrengthenConsume",Cfg.RealTimeSearch2Key(Config.pet_cfg_auto.strengthen_consume, "quality", "strengthen_level"))
    Cfg.Register("PetPrayEffectCfg",Cfg.RealTimeSearch2Key(Config.pet_cfg_auto.benediction_sp, "benediction_id", "benediction_level"))
    Cfg.Register("PetGiftLvupCfg",Cfg.CacheGet2Key(Config.pet_cfg_auto.skill_book_intensify, "src_skill_id", "intensify_level"))
    Cfg.Register("PetSpecialStrengthenConsume",Cfg.RealTimeSearch2Key(Config.pet_cfg_auto.special_pet_strengthen_consume, "prototype_id", "strengthen_level"))
    Cfg.Register("PetReturnCfg",Cfg.CacheGet2Key(Config.pet_cfg_auto.pet_return, "return_type", "strengthen_level"))

    -- 祝福效果组
    self.pet_pray_effect_group = DataHelper.ConvertGroup(Config.pet_cfg_auto.benediction_sp, "benediction_id")

    self.pet_get_info = {}

    -- self.gift_skill_lvup_cfg = {}   --skill_id/gift_id为key每个值都是一整个pet_cfg_auto.skill_book_intensify里的等级对应的配置,key1:skill_id,key2:lv

    -- for _,v in ipairs(Config.pet_cfg_auto.skill_book_intensify) do
    --     local lv_tab = self.gift_skill_lvup_cfg[v.src_skill_id] or self.gift_skill_lvup_cfg[v.dst_skill_id]
    --     if lv_tab == nil then
    --         lv_tab = {}
    --         lv_tab[1] = {intensify_level = 1}
    --     end
    --     self.gift_skill_lvup_cfg[v.src_skill_id] = lv_tab
    --     self.gift_skill_lvup_cfg[v.dst_skill_id] = lv_tab
    --     lv_tab[v.intensify_level] = v
    --     if v.intensify_level == 2 then
    --         lv_tab[1].dst_skill_id = v.src_skill_id
    --     end
    -- end
    -- LogError("self.gift_skill_lvup_cfg====",self.gift_skill_lvup_cfg)
    --检查宠物技能描述使用，仅在编辑模式生效
    if IS_EDITOR then
        self.pet_skill_desc_id_list = {}
        for k,v in pairs(Config.skill_description_auto.pet_skill_description) do
            self.pet_skill_desc_id_list[v.skill_id] = self.pet_skill_desc_id_list[v.skill_id] or {}
            self.pet_skill_desc_id_list[v.skill_id][v.skill_level] = v
        end
    end
end

function PetData:__delete()
    if PetData.Instance == self then
        PetData.Instance = nil
    end
end

function PetData:PetBagIsFull()
    return table.nums(self:GetPetList()) >= self:GetPetMaxBag()    
end

function PetData:GetStrengthenResult()
    return self.strengthen_result
end

-- 宠物列表选中用
function PetData:GetPetSelect()
    return self.pet_select
end

function PetData:SetHasGetList(data)
    self.pet_get_info = data.has_get_list
end

function PetData:PetHasGeted(pet_id)
    return self.pet_get_info[pet_id] == 1 or 
        self.pet_get_info[self:GetMutantPetCfgByNormalId(pet_id,PetData.MutantLv.One).pet_id] == 1 or
        self.pet_get_info[self:GetMutantPetCfgByNormalId(pet_id,PetData.MutantLv.Two).pet_id] == 1
end

function PetData:SetSelect(index)
    if self.pet_select.index ~= index then
        self.xilian_new_flag = {}
    end
    self.pet_select.index = index
end

function PetData:GetSelectPetIndex()
    return self.pet_select.index
end
-- 返回当前选择
function PetData:GetCurSelect()
    local pet = self:GetPetInfo(self.pet_select.index)
    return pet
end

--属性界面皮肤设置
function PetData:SetCurSkinSelect(id, data)
    self.cur_select_skin.skin_data = data
    self.cur_select_skin.id = id
end

--百科界面皮肤设置
function PetData:SetBookSkinSelect(id, data)
    self.cur_book_skin.skin_data = data
    self.cur_book_skin.id = id
end

-- 宠物图鉴强化用
function PetData:SetPetStrengthenLevel(level)
    self.pet_strengthen_level = level
end

function PetData:GetPetStrengthenLevel()
    return self.pet_strengthen_level
end

-- index从0开始
function PetData:SetPetList(list)
    self.pet_list:Set(list)
    self.pet_unique_id_list = {}
    for k,v in pairs(list) do
        self.pet_unique_id_list[tostring(v.info.pet_unique_id1).."-"..tostring(v.info.pet_unique_id2)] = v.index
    end
end

function PetData:SetPetStorageList(list)
    self.pet_storage_list:Set(list)
end

function PetData:GetPetList()
    return self.pet_list
end

function PetData:GetStorageList()
    return self.pet_storage_list
end

function PetData:ChangePetList(info)
    if info.config ~= nil then
        self.pet_list[info.index] = info
        self.pet_unique_id_list = self.pet_unique_id_list or {}
        self.pet_unique_id_list[tostring(info.info.pet_unique_id1).."-"..tostring(info.info.pet_unique_id2)] = info.index
    end
end

function PetData:ChangeStorageList(info, index)
    self.pet_storage_list[index] = info
end

function PetData:GetStorageInfo(index)
    return self.pet_storage_list[index]
end

function PetData:DeletePetList(index)
    if self.pet_list[index] then
        self.pet_list[index] = nil
    end
    for k,v in pairs(self.pet_unique_id_list) do
        if v == index then
            self.pet_unique_id_list[k] = nil
        end
    end
    if next(self.pet_list) == nil then
        ViewMgr:CloseView(PetView)
    end
end

function PetData:DeleteStorageList(index)
    if self.pet_storage_list[index] then
        self.pet_storage_list[index] = nil
    end
end

function PetData:SetNewPet(pet)
    self.new_pet = pet
    self.new_pet_smart.new_pet = pet
end

function PetData:GetPetInfo(index)
    return self.pet_list[index]
end

function PetData:GetPetInfoByUniqueId(uid_1,uid_2)
   for _,v in pairs(self.pet_list) do
        if v:UniqueId1() == uid_1 and v:UniqueId2() == uid_2 then
            return v
        end
   end 
   return nil
end

function PetData:SetPetTopRank(info)
    for _,v in pairs(self.pet_list) do
        if info[tostring(v:UniqueId1()).."-"..v:UniqueId2()] and info[tostring(v:UniqueId1()).."-"..v:UniqueId2()].pet_id == v.pet_id then
            v.info.rank = info[tostring(v:UniqueId1()).."-"..v:UniqueId2()].rank
        else
            v.info.rank = -1
        end
   end
end

function PetData:GetPetInfoByType(p_type, index)
    if p_type and p_type == 1 then
        return self:GetStorageInfo(index)
    else
        return self:GetPetInfo(index)
    end
end

function PetData:GetEquipSkillChange(info)
    local old_equip_skill_list = self.pet_list[info.index].info.equipment_skill_list
    local add_list = {}
    local lost_list = {}
    for i = 1, MAX_PET_EQUIPMENT_SKILL_NUM do
        if old_equip_skill_list[i] ~= 0 then
            lost_list[old_equip_skill_list[i]] = 1
        end
        if info.info.equipment_skill_list[i] ~= 0 then
            add_list[info.info.equipment_skill_list[i]] = 1
        end
    end
    for k,v in pairs(lost_list) do
        if add_list[k] == 1 then
            lost_list[k] = nil
            add_list[k] = nil
        end
    end
    if next(add_list) then
        for k,v in pairs(add_list) do
            add_list[k] = self:GetGiftDetail(k)
        end
    end
    if next(lost_list) then
        for k,v in pairs(lost_list) do
            lost_list[k] = self:GetGiftDetail(k)
        end
    end
    return add_list, lost_list
end

function PetData:GetNewGiftConfig(info)
    if info == nil or info.index == nil then
        return
    end
    local old_gift_list = self.pet_list[info.index]:GiftListNotEmpty()
    local new_gift_list = info:GiftListNotEmpty()
    local new_skill = nil
    for k,v in pairs(new_gift_list) do
        local temp = v
        for i, j in pairs(old_gift_list) do
            if j.gift_id == temp.gift_id then
                temp = nil
                break
            end
        end
        if temp ~= nil then
            new_skill = temp
        end
    end
    return self:GetGiftDetail(new_skill.gift_id)
end

function PetData:GetOtherInfo()
    return self.pet_other_info
end

function PetData:CopetSelectOnlyQuality()
    if self.sel_pet_str_data.click_index < 0 then
        return
    end
    local pet = self:GetCurSelect()
    if pet == nil then
        return nil
    end
    local strengthen_cfg = self:GetSpecialStrengthen(pet) or Cfg.PetStrengthenConsume(pet:Quality(), pet:StrLevel())

    if strengthen_cfg["is_oneself_"..self.sel_pet_str_data.click_index] == 1 then
        return false
    else
        return true, strengthen_cfg["strengthen_level_"..self.sel_pet_str_data.click_index], strengthen_cfg["quality_"..self.sel_pet_str_data.click_index]
    end
end

function PetData:GetExcepetSelect()
    local result = {}
    if self.sel_pet_str_data.click_index < 0 then
        return result
    end
    local pet = self:GetCurSelect()
    if pet == nil then
        return result
    end
    if self.sel_pet_str_data.is_yao then
        pet = pet:YaoPet()
    end
    local strengthen_cfg = self:GetSpecialStrengthen(pet) or Cfg.PetStrengthenConsume(pet:Quality(), pet:StrLevel())
    if strengthen_cfg.material_num < self.sel_pet_str_data.click_index then
        return result
    else
        local pet_list = self:GetPetList()
        for k, v in pairs(pet_list) do
            --非当前选中的宠物
            if pet.index ~= v.index then
                --强化等级不小于配置要求的副宠强化等级
                if v:StrLevel() >= strengthen_cfg["strengthen_level_"..self.sel_pet_str_data.click_index] and v:Quality() == strengthen_cfg["quality_"..self.sel_pet_str_data.click_index] then
                    --非守擂/出战/待机宠物
                    if not v:ArenaState() and not self:IsFightIndex(v.index) and not self:GetIsStandBy(v.index) and not v:PrayMainIndex() then
                        local is_put = false
                        for _,pet in pairs(self.sel_pet_str_data.info or {}) do
                            if pet.index == v.index then
                                is_put = true
                            end
                        end
                        if not is_put then
                            --是否是消耗本体
                            if strengthen_cfg["is_oneself_"..self.sel_pet_str_data.click_index] == 1 then
                                if pet.config.mutant_id == v.config.mutant_id then
                                    table.insert(result, v)
                                end
                            else
                                table.insert(result, v)
                            end
                        end
                    end
                end
            end
        end
        -- result = self:PetSort(result)
        table.sort(result,function (a, b)
            if a:StrLevel() ~= b:StrLevel() then
                return a:StrLevel() < b:StrLevel()
            elseif a:Quality() ~= b:Quality() then
                return a:Quality() < b:Quality()
            elseif a:Level() ~= b:Level() then
                return a:Level() < b:Level()
            else
                return a.index < b.index
            end
        end)
    end
    return result
end

-- 不等当前选中的同类型高强化宠物列表
function PetData:GetSameTypeList(pet)
    local result = {}
    local pet_list = self:GetPetList()
    for k, v in pairs(pet_list) do
        if pet.index ~= v.index and pet.pet_id == v.pet_id and pet.str_level < v.str_level then
            table.insert(result, v)
        end
    end
    return result
end

function PetData:SetPetAttrList(index, list)
    self.pet_attr_list[index] = list
end

function PetData:GetPetAttrInfo(index)
    return self.pet_attr_list[index]
end

function PetData:SetOtherInfo(info)
    for k,v in pairs(info) do
        if v ~= self.pet_other_info[k] then
            self.pet_other_info[k] = info[k]
        end
    end
    -- self.pet_other_info:Set(info)
end

function PetData:SetArenaMatchIndexList(arena_match_index_list)
    self.arena_match_index_list = TableCopy(arena_match_index_list)
end

function PetData:SetTrainInfo(info)
    local train_list = {}
    for i,v in ipairs(info) do
        train_list[i] = v.pet_index
    end
    self.pet_train_list = train_list
end

function PetData:GetIsArena(index)
    if self.arena_match_index_list == nil then
        return false
    end
    return self.arena_match_index_list[index]
end

function PetData:GetFightPetInfo()
    local index = self:GetFightIndex()
    return self:GetPetInfo(index), index
end

function PetData:GetFollowPetInfo()
    local index = self:GetFollowIndex()
    return self:GetPetInfo(index)
end

function PetData:GetFightIndex()
    if self.pet_other_info == nil then
        return -1
    end
    return self.pet_other_info.fight_index or -1
end

function PetData:IsFightIndex(index)
    if self:GetFightIndex() == -1 then
        return false
    end
    return self:GetFightIndex() == index
end

function PetData:GetFollowIndex()
    if self.pet_other_info == nil then
        return -1
    end
    return self.pet_other_info.follow_index or -1
end

function PetData:GetPetCfg(id)
    if id ~= nil then
        return self.pet_cfg_auto.pet_info_2[id]
    end
    return self.pet_cfg_auto.pet_info_2
end

function PetData:GetPetCfgByNormalId(id)
    if id ~= nil then
        return self.pet_cfg_list[id]
    else
        LogError("can not find pet_id:"..tostring(id).." config")
    end
end

--mut_stage 变异阶段
function PetData:GetMutantPetCfgByNormalId(id,mut_stage)
    if mut_stage == nil then
        --LogError("ERROR PetData:GetMutantPetCfgByNormalId mut_stage is NIL===",id)
        mut_stage = 1
    end

    if id ~= nil then
        if mut_stage == PetData.MutantLv.Two then
            return self.mutant2_pet_list[id] or {}
        else
            return self.mutant_pet_list[id] or {}
        end
    else
        return {}
    end
end

function PetData:GetNeedExp(level)
    return self.pet_cfg_auto.pet_levelup[level] or 0
end

function PetData:SetShowAttrIndex()
    self.trian_skill_attr_change_index = -1
    if self:GetFightIndex() ~= -1 then
        self.trian_skill_attr_change_index = self:GetFightIndex()
    else
    for i,v in ipairs(self:GetStandbyList()) do
            if v ~= -1 and self.trian_skill_attr_change_index == -1 then
            self.trian_skill_attr_change_index = v
        end
    end
    end
    if self.trian_skill_attr_change_index == -1 then
        local _, first_index = self:GetPetShowList()
        self.trian_skill_attr_change_index = first_index
    end
end

-- 转换服务端资质
function PetData.ConvertGrowth(t)
    local res = {}
    res.con = t.con_growth
    res.str = t.str_growth
    res.def = t.def_growth
    res.agi = t.agi_growth
    res.magic = t.magic_growth
    return res
end

function PetData.GetZZIndex(value)
    local index = 0
    local list = {con = 1, str = 2, def = 3, agi = 4, magic = 5}
    index = list[value]
    return index
end
-- 成长值
function PetData.GetGrowth(t)
    local res = 0
    res = res + (t.con or 0)
    res = res + (t.str or 0)
    res = res + (t.def or 0)
    res = res + (t.agi or 0)
    res = res + (t.magic or 0)
    return res
end

function PetData.GetPetMaxBp(pet_cfg)
    local total = 0
    if pet_cfg ~= nil then
        total = pet_cfg.con + pet_cfg.str + pet_cfg.def + pet_cfg.agi + pet_cfg.magic
    end
    return total
end

-- inddex用来获取服务端发来的信息 id获取基础信息
function PetData:GetConfigGrowth(id)
    local pet_cfg = self:GetPetCfg(id)
    if pet_cfg == nil then
        return 0
    end
    local total = pet_cfg.con + pet_cfg.str + pet_cfg.def + pet_cfg.agi + pet_cfg.magic
    return total
end
-- 获取元素属性 return {type = ,type = }
function PetData.GetElement(t)
    local res = {}
    if next(t) == nil then
        return res
    end
    for k, v in pairs(ElementType) do
        if t[k] > 0 then
            table.insert(res, v, t[k])
        end
    end
    return res
end
-- 基础属性
function PetData.BaseAttr(t)
    local res = {}
    res.maxhp = t.maxhp or 0
    res.maxmp = t.maxmp or 0
    res.gongji = t.gongji or 0
    res.fangyu = t.fangyu or 0
    res.minjie = t.minjie or 0
    res.jingshen = t.jingshen or 0
    res.huifu = t.huifu or 0
    return res
end
-- 资质
function PetData.ConvertPoint(t)
    local res = {}
    res.con = t.con or 0
    res.str = t.str or 0
    res.def = t.def or 0
    res.agi = t.agi or 0
    res.magic = t.magic or 0
    return res
end
-- 总资质 资质+强化增加(配置 等级) + 共鸣
function PetData.TotalAptiAttr(t, s, level)
    local res = {}
    local str_cfg = s[level]
    local tt = PetData.ConvertPoint(t)
    res.con = tt.con + (str_cfg.add_con / 100)
    res.str = tt.str + (str_cfg.add_str / 100)
    res.def = tt.def + (str_cfg.add_def / 100)
    res.agi = tt.agi + (str_cfg.add_agi / 100)
    res.magic = tt.magic + (str_cfg.add_magic / 100)
    return res
end

-- 转换强化资质
function PetData:ConvertStrGrowth(strengthen_id, level)
    local str_cfg = self:GetStrengthenCfg(strengthen_id)
    level = level <= 0 and 1 or level
    local res = {}
    if str_cfg[level] ~= nil then
        res.con = str_cfg[level].add_con / 100
        res.str = str_cfg[level].add_str / 100
        res.def = str_cfg[level].add_def / 100
        res.agi = str_cfg[level].add_agi / 100
        res.magic = str_cfg[level].add_magic / 100
    end
    return res
end

function PetData:GetLevelExp(level)
    if self.pet_cfg_auto.pet_levelup[level] then
        return self.pet_cfg_auto.pet_levelup[level].exp
    end
    return 0
end
--获取低等级到高等级的经验之和
function PetData:GetExpTotal(level1, level2)
    local total = 0
    for i = level1, level2 do
        total = total + self.pet_cfg_auto.pet_levelup[i].exp
    end
    return total
end

--换算加点 资质换算
function PetData.GetAptiPoint(t, level)
    local res = {}
    res.con = t.con * 0.25 + (level - 1) * 0.042 * t.con
    res.str = t.str * 0.25 + (level - 1) * 0.042 * t.str
    res.def = t.def * 0.25 + (level - 1) * 0.042 * t.def
    res.agi = t.agi * 0.25 + (level - 1) * 0.042 * t.agi
    res.magic = t.magic * 0.25 + (level - 1) * 0.042 * t.magic
    return res
end
--属性加点 换算加点 + 玩家加点（服务器下发）
function PetData.GetTotalPoint(t1, t2)
    local res = {}
    res.con = t1.con + t2.con
    res.str = t1.str + t2.str
    res.def = t1.def + t2.def
    res.agi = t1.agi + t2.agi
    res.magic = t1.magic + t2.magic
    return res
end

--加点属性值
function PetData.GetAptiAttr(t)
    local res = {}
    res.maxhp = PetData.GetAptiSingleAttr(t, "maxhp")
    res.maxmp = PetData.GetAptiSingleAttr(t, "maxmp")
    res.gongji = PetData.GetAptiSingleAttr(t, "gongji")
    res.fangyu = PetData.GetAptiSingleAttr(t, "fangyu")
    res.minjie = PetData.GetAptiSingleAttr(t, "minjie")
    res.jingshen = PetData.GetAptiSingleAttr(t, "jingshen")
    res.huifu = PetData.GetAptiSingleAttr(t, "huifu")
    return res
end
-- t 加点列表  T * T 做6次 属性值计算
function PetData.GetAptiSingleAttr(t, key)
    local self = PetData.Instance
    local res = 0
    local pet_xiulian_level = RoleSkillData.Instance:GetRolePetXiulianLevel()
    local temp_attr_exc = self.attr_exc[pet_xiulian_level]
    res = res + (t.con or 0) * temp_attr_exc[1][key] / 100
    res = res + (t.str or 0) * temp_attr_exc[2][key] / 100
    res = res + (t.def or 0) * temp_attr_exc[3][key] / 100
    res = res + (t.agi or 0) * temp_attr_exc[4][key] / 100
    res = res + (t.magic or 0) * temp_attr_exc[5][key] / 100
    return res
end
-- 基础属性 + 加点属性 + 强化属性 + 魂器属性 + 神印属性
function PetData.TotalAttr(t1, t2, t3, t4, t5)
    local res = {}
    for k,v in pairs(AttrTypeForName) do
        if t1[k] ~= nil or t2[k] ~= nil or t3[k] ~= nil then
            local t1_k = t1[k] or 0
            local t2_k = t2[k] or 0
            local t3_k = t3[k] or 0
            res[v] = t1_k + t2_k + t3_k
        end
        res[v] = res[v] or 0
        if t4 then
            if t4[v] then
                res[v] = res[v] + t4[v].attri_value
            end
        end
        if t5 then
            if t5[v] then
                res[v] = res[v] + t5[v].attri_value
            end
        end
    end
    return res
end
-- 突破属性计算 --- ****** 》 废弃 《 ***** ---
-- （宠物基础体力属性点数*0.25+宠物基础体力属性点数*0.042*（宠物等级-1）+玩家所加宠物体力属性点数）*maxhp
function PetData.CalculateBreachAttri(pet,breach_lv)
    local breach_attri_list = {}
    local points = pet:Point()
    local total_growth_list = pet:TotalGrowthList()
    local lv = pet:Level()
    local attri_func = function (k,point_num,num)
        local attri_num = ((total_growth_list[k] * 0.25) + (total_growth_list[k] * 0.042 * (lv - 1)) + point_num) * (num / 100)
        return attri_num
    end
    for k,v in pairs(points) do
		local breach_attri_cfg = Cfg.PetBreachAttriInfo(breach_lv,PetData.BreachAttriId[k])
		for attr_k,v2 in pairs(breach_attri_cfg) do
			if Language.PetAttriNames[attr_k] and breach_attri_cfg[attr_k] > 0 then
				if breach_attri_list[attr_k] == nil then
					breach_attri_list[attr_k] = 0
				end
				breach_attri_list[attr_k] = breach_attri_list[attr_k] + attri_func(k,v,v2)
			end
		end
	end
    return breach_attri_list
end

-- 处于 战斗 待机 还是休息  附带 是否是跟随
function PetData:GetPetState(index)
    if index == -1 then
        return PetState.Relax
    end
    local flag = PetState.Relax
    local inArena = self:GetIsArena(index)
    if inArena then
        flag = PetState.Arena
    end
    local fight = self:GetFightIndex()
    if index == fight then
        flag = PetState.Fight
    end
    local standby = self:GetIsStandBy(index)
    if standby then
        flag = PetState.StandBy
    end
    local train = self:GetIsTrain(index)
    if train then
        flag = PetState.Train
    end

    local follow = self:GetFollowIndex()
    return flag, index == follow
end

function PetData:GetIsStandBy(index)
    for k,v in pairs(self:GetStandbyList()) do
        if v == index then
            return true
        end
    end
    return false
end

function PetData:GetIsTrain(index)
    for k,v in pairs(self.pet_train_list or {}) do
        if v == index then
            return true
        end
    end
    return false
end

function PetData:GetStandbyList()
    return self.pet_other_info.standby_list or {}
end

-- 存在可待机位置
function PetData:GetStandbyCanUse()
    local canuse = true
    local standby = 0
    for k,v in pairs(self:GetStandbyList()) do
        if v == -1 then
            standby = standby + 1
        end
    end
    if standby == 1 and self:GetFightIndex() ~= -1 then
        canuse = false
    end
    if standby == 0 then
        canuse = false
    end
    return canuse
end

function PetData:GetCanFreeLifePet()
    local can_free_list = {}
    for k, v in pairs(self.pet_list) do
        if not self:IsFightIndex(v.index) and not self:GetIsStandBy(v.index) and not self:GetIsTrain(v.index) then
            table.insert(can_free_list, v)
        end
    end
    return can_free_list
end

function PetData:GetLockedPet()
    local locked_pet = {}
    for k,v in pairs(self.pet_list) do
        if v.info.lock == 1 and not self:IsFightIndex(v.index) and not self:GetIsStandBy(v.index) then
            locked_pet[v.index] = 1
        end
    end
    return locked_pet
end

function PetData:GetGiftIdByGroupId(group_id)
    return self.gift_group[group_id].gift_id
end

function PetData:GetGiftDetail(id, level)
    local skill_id = id
    if level == nil then
        level = 1
        if AmuletData.Instance then
            level = AmuletData.Instance:GetIntensifySkillLevel(skill_id) or 1
        end
    end
    local skill_id_cfg = self.pet_gift_detail_cache[skill_id]
    if skill_id_cfg == nil then
        skill_id_cfg = {}
        self.pet_gift_detail_cache[skill_id] = skill_id_cfg
    end
    local cfg = skill_id_cfg[level]
    if cfg == nil then
        local gift_cfg = self.pet_gift_list[id]
        if gift_cfg == nil then
            gift_cfg = self.pet_gift_detail[id]
        else 
            skill_id = gift_cfg.param_1     --传进来的是gift_id找技能配置需要用param_1
        end
        cfg = SkillData.GetDecConfig(skill_id,level)
        if cfg ~= nil then
            if gift_cfg ~= nil then
                cfg.page = gift_cfg.page
                cfg.item_id = gift_cfg.item_id
            end
            cfg.name = cfg.skill_name
            cfg.desc_1 = cfg.description_2
            cfg.param_1 = cfg.skill_id
            skill_id_cfg[level] = cfg
        else
            BuglyReport("PetData:GetGiftDetail:GetDecConfig ERROR,skill_id=",id,"level=",level)
        end
    end
    return cfg
end
--五个天赋
function PetData:GetGiftCfg()
    return self.pet_gift_cfg
end

function PetData:GetChangeNameCost()
    return self.pet_other_cfg.pet_rename_need
end

function PetData:GetPetOtherCfg()
    return self.pet_other_cfg
end

function PetData:ExpandPrice()
    return self.pet_cfg_auto.expand_price
end

function PetData:GetPetMaxBag()
    return self.pet_other_cfg.max_pet_bag
end

function PetData:GetCurMaxPetCount()
    return self.pet_other_cfg.start_num + (self.pet_other_info.extend_grid_num or 0 ) * self.pet_other_cfg.add_num
end

function PetData:GetPetMaxStorage()
    return self.pet_other_cfg.max_pet_warehouse
end

function PetData:GetPetStartBag()
    return self.pet_other_cfg.start_num
end

function PetData:GetCardList()
    if next(self.card_list) == nil then
        table.insert(self.card_list, self.pet_other_cfg.normal_sealcard)
        table.insert(self.card_list, self.pet_other_cfg.silver_sealcard)
        table.insert(self.card_list, self.pet_other_cfg.gold_sealcard)
    end
    return self.card_list
end

function PetData:GetSkillCfg(skill_id)
    local skill_level = math.floor(skill_id / PetData.SkillFactor)
    local real_skill_id = math.fmod(skill_id, PetData.SkillFactor)
    skill_level = skill_level < 1 and 1 or skill_level
    if next(self.pet_skill_config) == nil then
        for _, v in pairs(self.pet_skill_desc) do
            if self.pet_skill_config[v.skill_id] == nil then
                self.pet_skill_config[v.skill_id] = {}
            end
            self.pet_skill_config[v.skill_id][v.skill_level] = v
        end
    end
    if self.pet_skill_config[real_skill_id] then
        return self.pet_skill_config[real_skill_id][skill_level]
    end
    return nil
end

--根据技能id和等级获取技能描述
function PetData:GetSkillConfig(skill_id, skill_level)
    local level_list = self:GetSkillLevelCfgList(skill_id)
    if level_list and level_list[skill_level] then
        return level_list[skill_level]
    elseif DEBUG_MODE then
        LogError("Can't find pet skill_id:",skill_id," skill_level:",skill_level," config in Config.skill_description_auto.pet_skill_description")
    end
end
--根据技能id获得技能等级配置描述列表
function PetData:GetSkillLevelCfgList(skill_id)
    return CacheList(self,"pet_skill_level_list",skill_id,function ()
        local level_list = {}
        for k,v in pairs(self.pet_skill_desc) do
            if v.skill_id == skill_id then
                level_list[v.skill_level] = v
            end
        end
        return level_list
    end)
end

function PetData:GetSkillEffect(skill_id)
    for k,v in pairs(self.pet_cfg_auto.skill_effects) do
        if v.skill_id == skill_id then
            return v.effects_id
        end
    end
end

-----------图鉴
--获取全部宠物信息 或者 某类宠物信息
function PetData:GetPetListRace(race)
    if next(self.pet_info_race) == nil then
        for k, v in pairs(self.pet_cfg_list) do
            if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
                if PetData.AuditShowPetId[v.config.id] == 1 and v.config.is_show == 1 and v.config.mutant_times == 0 then
                    if self.pet_info_race[v:RaceId()] == nil then
                        self.pet_info_race[v:RaceId()] = {}
                    end
                    table.insert(self.pet_info_race[v:RaceId()], v)
                end
            else
                if v.config.is_show == 1 and v.config.mutant_times == 0 then --百科界面仅显示非变异宠物
                    if self.pet_info_race[v:RaceId()] == nil then
                        self.pet_info_race[v:RaceId()] = {}
                    end
                    table.insert(self.pet_info_race[v:RaceId()], v)
                end
            end
        end
        for _, v in pairs(self.pet_info_race) do
            table.sort(
                v,
                function(a, b)
                    if a.sort < b.sort then
                        return true
                    else
                        return false
                    end
                end
            )
        end
    end
    if race ~= nil then
        return self.pet_info_race[race]
    end
    return self.pet_info_race
end
--获取全部宠物信息或某个品质宠物信息
function PetData:GetPetListQuality(quality)
    local role_level = RoleData.Instance:GetRoleLevel()
    -- 等级有变化则清掉重新缓存数据
    if role_level ~= self.pet_info_quality_level then
        self.pet_info_quality = {}
    end
    if next(self.pet_info_quality) == nil then
        self.pet_info_quality_level = role_level
        for k, v in pairs(self.pet_cfg_list) do
            if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
                if PetData.AuditShowPetId[v.config.id] == 1 and v.config.is_show == 1 and v.config.mutant_times == 0 then
                    if self.pet_info_quality[v:Quality()] == nil then
                        self.pet_info_quality[v:Quality()] = {}
                    end
                    table.insert(self.pet_info_quality[v:Quality()], v)
                end
            else
                if v.config.is_show == 1 and v.config.mutant_times == 0 then --百科界面仅显示非变异宠物
                    if self.pet_info_quality[v:Quality()] == nil then
                        self.pet_info_quality[v:Quality()] = {}
                    end
                	if not v.config.show_condition_level or role_level >= v.config.show_condition_level then
                    	table.insert(self.pet_info_quality[v:Quality()], v)
                	end
                end
            end
        end
        for _, v in pairs(self.pet_info_quality) do
            table.sort(
                v,
                function(a, b)
                    if a.sort < b.sort then
                        return true
                    else
                        return false
                    end
                end
            )
        end
    end
    if quality ~= nil then
        return self.pet_info_quality[quality]
    end
    return self.pet_info_quality
end
--获取某类或者全部宠物 的某项资质排序信息
function PetData:GetPetListAttr()
    local race_index = self:GetRaceIndex()
    local attr_index = self:GetAttrIndex()
    local result = {}
    if race_index == -1 then
        if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
            for k,v in pairs(self.normal_pet_list) do
                if PetData.AuditShowPetId[v.config.id] == 1 then
                    table.insert(result, v)
                end
            end
        else
            result = self.normal_pet_list
        end
    else
        result = self:GetPetListRace(race_index)
    end
    -- 按某项资质排序
    if attr_index ~= -1 then
        local key = Language.Pet.AttrClass[attr_index][1]
        result = table.rearrange(result)
        table.sort(
            result,
            function(a, b)
                if a:PetCfg()[key] > b:PetCfg()[key] then
                    return true
                else
                    return false
                end
            end
        )
        return result
    end
    return result
end

function PetData:SetRaceIndex(index)
    self.pet_race_index = index
end

function PetData:SetAttrIndex(index)
    self.pet_attr_index = index
end

function PetData:GetRaceIndex()
    return self.pet_race_index
end

function PetData:GetAttrIndex()
    return self.pet_attr_index
end

function PetData:SetBookSelect(data)
    self.pet_book_cell:Set(data)
end

function PetData:GetBookSelect()
    return self.pet_book_cell
end

function PetData:GetIsAllAndDefault()
    return self.pet_race_index == -1 and self.pet_attr_index == -1
end

function PetData:GetStrengthenCfg(strengthen_id, level)
    if strengthen_id ~= nil then
        if level ~= nil then
            return self.pet_strengthen_cfg[strengthen_id][level]
        else
            return self.pet_strengthen_cfg[strengthen_id]
        end
    end
    return self.pet_strengthen
end

function PetData:GetPetExpItemList()
    return self.pet_cfg_auto.pet_exp_pill
end

function PetData:GetExpItemCfg(item_id)
    if next(self.exp_item_list) == nil then
        for k, v in pairs(self.pet_cfg_auto.pet_exp_pill) do
            self.exp_item_list[v.item_id] = v
        end
    end
    return self.exp_item_list[item_id]
end

function PetData.SetStrLevel(bg, num, level)
    num.SpriteName = PetData.PetStrLevel[level]
end

function PetData:GetIsShowStrAlert()
    return self.pet_str_alert
end

function PetData:SetIsShowStrAlert(flag)
    self.pet_str_alert = flag
end

function PetData:SetPetStrSelect(index)
    self.pet_str_select = index
end

function PetData:GetPetStrSelect()
    return self.pet_str_select
end

function PetData:GetHitBookPageList()
    return self.hit_book_page_list[self.hit_book_type.index + 1]
end

function PetData:SetHitBookType(typeIndex)
    self.hit_book_type.index = typeIndex
end

function PetData:SetXilianDataSuggest(show_suggest)
    self.xilian_data.show_suggest = show_suggest
end

function PetData:CheckNewFlag(pet_info)
    self.xilian_new_flag = {}
    if self.pet_select.index ~= pet_info.index or self.xilian_data.gift_only or not self.xilian_data.click_save then
        return
    end
    local old_data = self:GetPetInfo(pet_info.index)
    for i, v in ipairs(pet_info.gift_list) do
        if v.gift_id == 0 then
            break
        else
            self.xilian_new_flag[v.gift_id] = 1
            for _, v2 in ipairs(old_data.gift_list) do
                if v.gift_id == v2.gift_id then
                    self.xilian_new_flag[v.gift_id] = 0
                    break
                end
            end
        end
    end
end

function PetData:CheckXiLianGuide()
    for k,v in pairs(self.pet_list) do 
        if v:State() == PetState.Fight then 
            local flag_clear = false
            for i,j in pairs(v.gift_list) do 
                if j.gift_id > 0 then 
                    flag_clear = true
                    break
                end 
            end 

            return flag_clear
        end 
    end  
    return false
end

--检查是否有宠物共鸣
--是否共鸣
--共鸣资质
--共鸣等级
function PetData:CheckPetStrBuff(index)
    -- ********* 2022/6/1 #58689宠物强化共鸣改版 *****
    return false, {}, 0
    -- -- 判断是否是战斗待机中的一员
    -- local state = self:GetPetState(index)
    -- local res = {}
    -- local cfg = {}
    -- res = PetData.ConvertBuff(cfg)
    -- if state == PetState.Relax then
    --     return false, res, 0
    -- end
    -- local level_t = {}
    -- local fight_pet = self:GetFightPetInfo()
    -- if fight_pet ~= nil then
    --     table.insert(level_t, fight_pet:StrLevel())
    -- end
    -- for i,v in ipairs(self:GetStandbyList()) do
    --     if v ~= -1 then
    --         local pet = self:GetPetInfo(v)
    --         if pet then
    --             local level = pet:StrLevel()
    --             table.insert(level_t, level)
    --         end
    --     end
    -- end
    -- --出战及待机相加不到3只
    -- if #level_t < 3 then
    --     return false, res, 0
    -- end
    -- table.sort(level_t, function (a,b)
    --     if a >= b then
    --         return false
    --     else
    --         return true
    --     end
    -- end)
    -- local remove_num = #level_t - 3
    -- for i=1, remove_num do
    --     table.remove(level_t, 1)
    -- end
    -- if #level_t >= 3 then
    --     cfg = self.pet_strengthen_buff[level_t[1]]
    --     res = PetData.ConvertBuff(cfg)
    --     return true, res, cfg.buff_level
    -- end
end

--计算共鸣等级
function PetData:CalculateBuffLevel(level_t)
    -- ********* 2022/6/1 #58689宠物强化共鸣改版 *****
    return 0

    -- local res = {}
    -- local cfg = {}
    -- if #level_t < 3 then
    --     return 0
    -- end
    -- table.sort(level_t, function (a,b)
    --     if a >= b then
    --         return false
    --     else
    --         return true
    --     end
    -- end)
    -- local remove_num = #level_t - 3
    -- for i=1, remove_num do
    --     table.remove(level_t, 1)
    -- end
    -- if #level_t >= 3 then
    --     cfg = self.pet_strengthen_buff[level_t[1]]
    --     res = PetData.ConvertBuff(cfg)
    --     return cfg.level
    -- end
end

-- 转换共鸣资质资质
function PetData.ConvertBuff(t)
    local res = {}
    res.con = (t.buff_con or 0) / 100
    res.str = (t.buff_str or 0) / 100
    res.def = (t.buff_def or 0) / 100
    res.agi = (t.buff_agi or 0) / 100
    res.magic = (t.buff_magic or 0) / 100
    return res
end

-- 资质加法
function PetData.AddGrowth(t1, t2)
    local res = {}
    res.con = (t1.con or 0) + (t2.con or 0)
    res.str = (t1.str or 0) + (t2.str or 0)
    res.def = (t1.def or 0) + (t2.def or 0)
    res.agi = (t1.agi or 0) + (t2.agi or 0)
    res.magic = (t1.magic or 0) + (t2.magic or 0)
    return res
end

--获取天赋刷新配置
function PetData:GetGiftRefreshConfig(count)
    local key = 1
    -- 逻辑在小于等于count的数组里找到最大的那组
    for k, v in pairs(self.pet_gift_refresh) do
        if v.time <= count then
            key = k >= key and k or key
        end
    end
    return self.pet_gift_refresh[key]
end

function PetData:SetGiftIndex(index)
    self.pet_gift_index = index
end

function PetData:GetGiftIndex()
    return self.pet_gift_index
end

function PetData.GiftItem(id)
    local t = {}
    if type(id) == "table" then
        t = id
    else
        t.gift_id = id
    end
    return t
end

function PetData.AddSkill()
    return {
        is_add = true,
        is_kong = false,
        skill_id = 0
    }
end
-- 学习选择的技能书
function PetData:SetSelectSkill(data)
    self.pet_skill_select:Set(data)
end

function PetData:GetSelectSkill()
    return self.pet_skill_select
end
-- 已学技能点选的技能
function PetData:SetClickSkill(data)
    self.pet_click_skill = data
end

function PetData:GetClickSkill()
    return self.pet_click_skill
end

function PetData:SetLearnPreSkill(data)
    self.pet_learn_pre_skill = data
end

function PetData:GetLearnPreSkill()
    return self.pet_learn_pre_skill
end

-- 选中要替换的技能index
function PetData:SetSelectSkillIndex(index)
    self.select_skill_index = index
end
function PetData:GetSelectSkillIndex()
    return self.select_skill_index or -1
end

--属性
function PetData:RealAttr(index, attr_list)
    local t = self:GetPetAttrInfo(index or self:GetSelectPetIndex())
    if index == -1 then
        t = attr_list
    end
    if t == nil then
        return
    end
    local res = {}
    res.maxhp = t[BattleAttriType.MaxHp] or 0
    res.maxmp = t[BattleAttriType.MaxMp] or 0
    res.gongji = t[BattleAttriType.Attack] or 0
    res.fangyu = t[BattleAttriType.Defense] or 0
    res.minjie = t[BattleAttriType.Agility] or 0
    res.jingshen = t[BattleAttriType.Mental] or 0
    res.huifu = t[BattleAttriType.Recovery] or 0
    return res
end

--天赋增长属性(服务端属性 加点属性)
function PetData:GiftAttr(pet, attrs, add_attrs)
    return {}, {}, {}, {}
end

function PetData:GetCatchWay(mtype)
    local catch_way_t = {}
    local split = string.split(tostring(mtype), "|")
    for k, v in pairs(split) do
        table.insert(catch_way_t, Cfg.PetGetWay(tonumber(v)))
    end
    return catch_way_t
end

function PetData.GetItemIconId(id, skin_id)
    local co = Config.pet_auto[id] or {}
    local icon_id = co and co.icon_id or 0
    if skin_id and skin_id > 0 then
        local co = Cfg.PetSkinAttr(skin_id)
        icon_id = co and co.head_portrait or icon_id
    end
    return icon_id
end

function PetData:GetTrade(id)
    local co = Config.pet_auto[id] or {}
    return co.item_trade_money_type
end

function PetData:SetOldStrLevel(level, is_succ)
    self.old_str_level = level
    self.change_succ = is_succ == 1
end
function PetData:OldStrLevel()
    return self.old_str_level or 1, self.change_succ
end

--交易行可出售的宠物列表
function PetData:GetRearrangeData()
    local list = {}
    for i, v in ipairs(table.rearrange(self.pet_list)) do
        if not self:IsFightIndex(v.index) and not self:GetIsStandBy(v.index) and not v:PrayMainIndex() then
            if not v:ArenaState() and not AnotherData.Instance:IsPetFightIndex(v.index) and self:GetTrade(v.pet_id) ~= 0 then
                table.insert(list, v)
            end
        end
    end
    if #list < TRADE_BAG_MAX_NUM then
        for i = #list + 1, TRADE_BAG_MAX_NUM do
            list[i] = {item_id = 0}
        end
    else
        for i=#list + 1,math.ceil(#list / 4) * 4 do
            list[i] = {item_id = 0}
        end
    end
    return list
end

function PetData:SetShopPetInfo(protocol)
    if self.shop_pet_info.item_count > 0 then
        local index = self.shop_pet_info.item_count
        for i = 1, protocol.item_count do
            self.shop_pet_info.item_list[index + i] = protocol.item_list[i]
        end
        self.shop_pet_info.item_count = self.shop_pet_info.item_count + protocol.item_count
        return
    end
    self.shop_pet_info.item_count = protocol.item_count
    self.shop_pet_info.item_list = protocol.item_list
end

function PetData:ShopPetInfo()
    return self.shop_pet_info
end

function PetData:ClearShopPetInfo()
    self.shop_pet_info.item_count = 0
    self.shop_pet_info.item_list = {}
end

function PetData:GetCanAddExp()
    local exp = 0
    for i, v in ipairs(Config.pet_cfg_auto.pet_exp_pill) do
        local num = BagData.Instance:GetNumByItemId(v.item_id)
        exp = exp + num * v.add_exp
    end
    return exp
end
-----------------红点-------------------------
function PetData:CheckAllUpgradePointRemind()
    -- 每一只都要检查
    local list = self:GetPetList()
    if table.nums(list) == 0 then
        return 0
    end
    local num = 0
    local pets = {}
    for k, v in pairs(list) do
        local up_num = self:CanUpgradePoint(v)
        num = num + up_num
        local breach_num = self:IsCanBreach(v)
        num = num + (breach_num > 0 and 1 or 0)
        if up_num > 0 or breach_num > 0 then
            table.insert(pets, v)
        end
    end
    -- 有几个宠物都可的时候需返回最好的那个宠物出去。
    if next(pets) == nil then
        return 0
    end
    return num, self:PetSort(pets)[1]
end

function PetData:CheckAttributePointRemind()
    -- 每一只都要检查
    local list = self:GetPetList()
    if table.nums(list) == 0 then
        return 0
    end
    for k, v in pairs(list) do
       if self:CanAttributePoint(v) > 0 then
			return 1		
	   end
    end
	return 0
end

function PetData:CanAttributePoint(pet)
    if not AttributeDanData.Instance:GetCanUse() then
        return 0
    end
	local role_id = RoleData.Instance:GetRoleId()
	local red_sign = UnityPlayerPrefs.GetInt(role_id.."pet_attribute_dan"..pet.index) 
	if red_sign == 0 then
		local state = pet:State()
		if state ~= PetState.Fight and state ~= PetState.StandBy then
			return 0
		end
	end
	if red_sign == 1 then
		return 0
	end
		
	local limit = AttributeDanData.Instance:GetAttibuteDanLimit()
	for i=0,6 do
		local use_count = pet.info.use_attr_pellet_list[i + 1]
		local item_id_index = 0
		if use_count < limit then
			if i > 4 then
				item_id_index = 27807 + i - 5
			else
				item_id_index = 27809 + i
			end
			local attribute_pellet_id = AttributeDanData.Instance:GetAttibuteDanByItemId(item_id_index).attribute_pellet_id
			local num = BagData.Instance:GetNumByItemId(attribute_pellet_id)
			if num > 0 then
				return 1
			end
		end
	end
    return 0
end


function PetData:CheckFightUpgradePointRemind()
    -- 每一只都要检查
    local list = self:GetPetList()
    if table.nums(list) == 0 then
        return 0
    end
    local pet, index = self:GetFightPetInfo()
    if index == -1 or pet == nil then
        return 0
    else
        if pet:Level() >= RoleData.Instance:GetRoleLevel() or pet:IsNeedBreach() == true then
            return 0
        end
        local exp = self:GetCanAddExp()
        if exp > 0 and pet:Exp() + exp > self:GetLevelExp(pet:Level()) then
            return 1
        else
            return 0
        end
    end
end

function PetData:CanUpgradePoint(pet)
    local state = pet:State()
    if state ~= PetState.Fight and state ~= PetState.StandBy then
        return 0
    end
    if pet:IsNeedBreach() == true then
        return 0
    end
    if pet:Level() >= RoleData.Instance:GetRoleLevel() then
        return 0
    end
    local exp = self:GetCanAddExp()
    if exp > 0 and pet:Exp() + exp > self:GetLevelExp(pet:Level()) then
        return 1
    end
    return 0
end
function PetData:CheckFightPointRemind()
    if not FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetAddPoint) then
        return 0
    end
    local pet, index = self:GetFightPetInfo()
    if index == -1 or pet == nil or next(pet) == nil then
        return 0
    else
        return pet:PointNum() > 0 and 1 or 0
    end
    
end
function PetData:CheckPetPointRemind()
    if not FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetAddPoint) then
        return 0
    end
    -- 每一只都要检查
    local list = self:GetPetList()
    if table.nums(list) == 0 then
        return 0
    end
    local num = 0
    local pets = {}
    for k, v in pairs(list) do
        local up_num = self:CanAddPoint(v)
        num = num + up_num
        if up_num > 0 then
            table.insert(pets, v)
        end
    end
    -- 有几个宠物都可的时候需返回最好的那个宠物出去。
    if next(pets) == nil then
        return 0
    end
    return num, self:PetSort(pets)[1]
end
function PetData:CanAddPoint(pet)
    if not FunOpen.Instance:GetFunIsOpened(Mod.Pet.PetAddPoint) then
        return 0
    end
    if pet and pet:AutoAddInfo().index ~= 0 then
        return 0
    end
    local state = pet:State()
    if state ~= PetState.Fight and state ~= PetState.StandBy then
        return 0
    end
    local total = pet:PointNum()
    if total > 0 then
        return 1
    end
    return 0
end

function PetData:CheckPetGiftRemind()
    local list = self:GetPetList()
    if table.nums(list) == 0 then
        return 0
    end
    local num = 0
    for k, v in pairs(list) do
        num = num + self:CanOpenGift(v)
    end
    return num
end

--宠物装备提示红点
function PetData:CheckPetEquipmentRemind()
    if self:GetFightIndex() == nil or table.nums(self:GetPetList()) <= 0 then
        return 0
    end
    local list = BagData.Instance:GetPetEquipmentList()
    if next(list) then
        if self:GetFightIndex() ~= -1 and self.pet_list[self:GetFightIndex()].info.pet_equipment_id == 0 then
            return 1
        else
            return 0
        end
    else
        return 0
    end
end

-- 宠物徽章全部红点
function PetData:PetBagdeAllRemind()
    for i,v in pairs(self.pet_list) do
        local num = self:PetBagdeRemind(v)
        if num > 0 then
            return 1
        end
    end
    return 0
end
-- 宠物徽章单个红点
function PetData:PetBagdeRemind(pet)
    if not self.bagde_remind_flag then
        return 0
    end
    local state = pet:State()
    if state ~= PetState.Fight and state ~= PetState.StandBy then
        return 0
    end
    local item = pet:BagdeItem()
    local bagde_list = PetBagdeData.Instance:GetAllBagdeOfBag(0,false)
    if item then
        for i,v in pairs(bagde_list) do
            if v:QuaLevel() > item:QuaLevel() then
                return 1
            end
        end
    elseif #bagde_list > 0 then
        return 1
    end
    return 0
end

function PetData:CanOpenGift(pet)
    local state = pet:State()
    if state ~= PetState.Fight and state ~= PetState.StandBy then
        return 0
    end
    local gift_list = pet:GiftList()
    for i, v in ipairs(gift_list) do
        if v.is_lock == false and v.is_open == false then
            return 1
        end
    end
    return self:GaiMingRed(pet)
    -- return 0
end
function PetData:CanFlyup(check_state)
    local list = self:GetPetList()
    if table.nums(list) == 0 then
        return 0
    end
    local pets = {}
    for _, pet in pairs(list) do
        if pet:CanFlyup(check_state) then
            table.insert(pets, pet)
        end
    end
    -- 有几个宠物都可的时候需返回最好的那个宠物出去。
    if next(pets) == nil then
        return 0
    end
    return 1, self:PetSort(pets)[1]
end


function PetData:CheckGaiMingPointRemind()
    -- 每一只都要检查
    local list = self:GetPetList()
    if table.nums(list) == 0 then
        return 0
    end
    for k, pet in pairs(list) do
        if self:GaiMingRed(pet) == 1 then
            return 1
        end
    end
    return 0
    --先检查是否存在可加点
    --再检查是否满级
    --再检查材料是否存在
end
function PetData:GaiMingRed(pet)
    local state = pet:State()
    if state ~= PetState.Fight and state ~= PetState.StandBy then
        return 0
    end
    if pet:SurplusGaiMingPoint() > 0 then
        return 1
    end
    if not pet:IsMaxGaiMing() then
        if self:GaiMingIsTuPo(pet) then
            if Item.GetNum(self:GaiMingTuPoItem()) > 0 then
                return 1
            end
        else
            if Item.GetNum(self:GaiMingUpItem()) > 0 then
                return 1
            end
        end        
    end
    return 0
end

function PetData:GetPetSelectViewData()
    return Config.function_guide_auto.choose_pet
end

function PetData:SetGuidePetSelectCurSeq(data)
    self.guide_pet_select_cur.show_pet = data.show_pet
    self.guide_pet_select_cur.seq = data.seq
end

function PetData:SetPetInfoNotie(notie)
    self.pet_single_info_notie_type = notie
end

function PetData:GetPetInfoNotie()
    return self.pet_single_info_notie_type
end

function PetData:GetIsPetInfoNotieShowAttrChange()
    return self.pet_single_info_notie_type ~= PetNotieType.PET_NOTIE_OLD_TYPE and
        self.pet_single_info_notie_type ~= PetNotieType.PET_NOTIE_TYPE_FABAO and
        (self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_BREAK_THROUGHS
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_ASSIGN_POINTS
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_SKILL
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_GIFT
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_REFINED_SAVE
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_EXP
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_BATTLE_ADD_EXP
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_GIFT
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_REPLACE_GIFT
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_COMPREHEND_GIFT
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_TRAIN_SKILL
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_PET_EQUIPMENT_CHANGE
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_PET_BREACH
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_AUTO_ASSIGN_POINTS
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_GUILD_INBORN
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_STAMP_DATA
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_WEAPON_DATA
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_WEAPON_CHANGE
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_AUTO_UP_LEVEL
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_PRAY_CHANGE
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_FEISHENG
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_SHENG_QI
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_STAR_MAP
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_GAI_MING
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_GAI_MING_ATTR
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_GAI_MING_ATTR_RESET
        or self.pet_single_info_notie_type == PetNotieType.PET_NOTIE_TYPE_SHENG_HEN_INFO_CHANGE
        )
end

function PetData:GetAllPetListSorted()
    return self.allPetSortedList
end

function PetData:GetAllShenshouConfig()
    return self.pet_shenshou_list
end

function PetData.GetEmptySkillData(pet_idx)
    local t = {}
    t.skill_id = 0
    t.skill_level = 0
    t.show_lingwu = false
    t.is_fu_skill = false
    t.is_yao_skill = false
    t.pet_idx = pet_idx
    return t
end

function PetData.GetSlotTypeStuffCfg(slot_type)
    return Config.pet_cfg_auto.slot_type[slot_type]
end

function PetData.GetComposeInfo(item_id)
    if PetData.Instance.pet_egg_compose[item_id] ~= nil then
        return PetData.Instance.pet_egg_compose[item_id], true
    else
        return Cfg.PetComposeInfo(item_id), false
    end
end

function PetData.PetComposeInfoSeq(seq)
    return Cfg.PetComposeInfoSeq(seq)
end

function PetData:PetGroupById(group)
    if self.pet_group_list[group] and next(self.pet_group_list[group]) then
        return self.pet_group_list[group]
    end
    self.pet_group_list[group] = {}
    local index = 0
    for i,v in ipairs(Config.pet_cfg_auto.pet_group) do
        if v.reward_group == group then
            index = index + 1
            v.index = index
            table.insert(self.pet_group_list[group], v)
        end
    end
    return self.pet_group_list[group]
end

--获取神兽兑换列表
function PetData:ConvertList()
    return Config.pet_cfg_auto.mythical_animals_convert
end

function PetData:ConvertSelect(data)
    if data then
        self.pet_convert_select_data = data
    end
    return self.pet_convert_select_data
end

function PetData:GetConverList()
    local info = PetData.PetComposeInfoSeq(45)
    local info_1 = self:PetGroupById(info.reward_group)
    
    self.pet_shenshou_convert_list = {}
    for k,v in pairs(info_1) do 
        local vo = {} 
        vo.data_ = self.pet_cfg_list[v.pet_id]
        vo.index = k
        table.insert(self.pet_shenshou_convert_list,vo)
    end 
    return self.pet_shenshou_convert_list
end 

function PetData:SetLocalConvertIndex(_index)
    self.local_convert_index = _index
end 

function PetData:GetLocalConvertIndex()
    return self.local_convert_index
end 

function PetData:ClearLocalConvertIndex()
    self.local_convert_index = nil
end 

function PetData:GetEquipItemTypeList()
    if self.equip_item_type_list == nil then
        self.equip_item_type_list = {}
        local item_type = 1
        local t = {}
        t.name = Language.PetEquipSelect.AllItemType
        t.item_type = item_type
        table.insert(self.equip_item_type_list,t)
        local temp_list = {}
        for k,v in pairs(Config.pet_equipment_item_auto) do
            local it = {}
            it.name = v.name
            it.item_id = v.id
            table.insert(temp_list, it)
        end
        table.sort(temp_list,function (a,b)
            return a.item_id > b.item_id
        end)
        for i= 1, #temp_list do
            temp_list[i].item_type = item_type + 1
            item_type = temp_list[i].item_type
            table.insert(self.equip_item_type_list, temp_list[i])
        end
    end
    return self.equip_item_type_list
end

function PetData:GetPetItemTypeId()
    local type_list = self:GetEquipItemTypeList()
    for k,v in pairs(type_list) do
        if self.pet_equip_select.item_type == v.item_type then
            return v.item_id
        end
    end
end

function PetData:GetEquipSkillTypeList()
    local t = {}
        t[0] = {name = Language.PetEquipSelect.SkillTypeList[0], skill_type = 0}
        t[1] = {name = Language.PetEquipSelect.SkillTypeList[2], skill_type = 2}
        t[2] = {name = Language.PetEquipSelect.SkillTypeList[1], skill_type = 1}
    return t
end

function PetData:GetEquipShowList()
    local equip_list = AmuletData.Instance:GetBagAndPetAmuletList(true)
    if #equip_list == 0 then
        return nil
    end

    local item_id = self.pet_equip_select.item_type == 1 and 0 or self:GetPetItemTypeId()

    if item_id == 0 and self.pet_equip_select.skill_type == 0 then
        return equip_list
    end
    local item_type_list = {}
    for k,v in pairs(equip_list) do
        if v.item_id == item_id or item_id == 0 then
            table.insert(item_type_list, v)
        end
    end
    if self.pet_equip_select.skill_type == 0 then
        return item_type_list
    end
    local skill_type_list = {}
    for k,v in pairs(item_type_list) do
        for i=1, MAX_PET_EQUIPMENT_SKILL_NUM do
            if v.param.equipment_skill_list[i] ~= 0 then
                local page
                local skill_id = v.param.equipment_skill_list[i]
                if AmuletData.Instance:GetIntensifySkillLevel(skill_id) then
                    page = 2
                else
                    local cfg = Cfg.PetSkillPageById(skill_id)
                    if cfg then
                        page = cfg.page
                    end
                end
                if page and page == self.pet_equip_select.skill_type then
                    table.insert(skill_type_list, v)
                    break
                end
            end
        end
    end
    return skill_type_list
end

function PetData:GetPetIndexByUniqueId(id1, id2)
    if self.pet_unique_id_list == nil then
        return -1
    end
    local id = tostring(id1).."-"..tostring(id2)
    return self.pet_unique_id_list[id] or -1
end

--获取宠物们已经装备的护符列表
function PetData:GetPetEquipmentList()
    local pets = self:GetPetList()
    local equip_list = {}
    for _, v in pairs(pets) do
        local equip = v:GetPetEquipItem()
        if equip ~= nil and equip.item_id > 0 and equip.param.equipment_skill_num > 0 then
            table.insert(equip_list, equip)
        end
    end
    local sort_func = function(a, b)
        if a.item_id == b.item_id then
            return a.param.pet_index < b.param.pet_index
        else
            return a.item_id > b.item_id
        end
    end
    table.sort(equip_list, sort_func)
    return equip_list

end

--是否有宠物在训宠中状态
function PetData:GetTrainState(pet_info)
    for k,v in pairs(self:GetPetList()) do
        if v.info.pet_unique_id1 == pet_info.pet_unique_id1 and v.info.pet_unique_id2 == pet_info.pet_unique_id2 then
            return self:GetIsTrain(v.index)
        end
    end
    return false
end
--对应宠物的皮肤列表
function PetData:GetPetSkinCfgList(pet_id)
    return self.pet_skin_cfg[pet_id]
end

function PetData:GetSkinHasIsActive(skin_id)
    return self.pet_has_skin_list[skin_id] == 1
end

function PetData:GetPetActivedSkinList()
    return SmartData.GetVal(self.pet_has_skin_list)
end

function PetData:SetPetSkinList(protocol)
    self.pet_has_skin_list:Set(protocol)
end

-------寻宝能抽到皮肤，需要先临时存放-------
function PetData:SetTempPetHasSkinList()
    self.temp_pet_has_skin_list = TableCopy(self.pet_has_skin_list, 2)
end

function PetData:GetTempSkinHasIsActive(skin_id)
    return self.temp_pet_has_skin_list[skin_id] == 1
end

function PetData:SetTempSkinHasIsActive(skin_id, value)
    value = value or 1
    self.temp_pet_has_skin_list[skin_id] = value
end
-------寻宝能抽到皮肤，需要先临时存放-------

function PetData:GetSkinGrowthChange(info)
    local old_skin_attr = {}
    local new_skin_attr = {}
    if self.pet_list[info.index].info.skin_id ~= 0 then
        old_skin_attr = Cfg.PetSkinAttr(self.pet_list[info.index].info.skin_id)
    end
    if info.skin_id ~= 0 then
        new_skin_attr = Cfg.PetSkinAttr(info.skin_id)
    end
    local change_t = {}
    change_t.con = new_skin_attr.con or 0 - old_skin_attr.con or 0
    change_t.str = new_skin_attr.str or 0 - old_skin_attr.str or 0
    change_t.def = new_skin_attr.def or 0 - old_skin_attr.def or 0
    change_t.agi = new_skin_attr.agi or 0 - old_skin_attr.agi or 0
    change_t.magic = new_skin_attr.magic or 0 - old_skin_attr.magic or 0
    return change_t
end

--获取宠物背包剩余容量
function PetData:GetBagRemainNum()
    local other_info = self:GetOtherInfo()
    local total = self:GetPetStartBag() + ( other_info.extend_grid_num or 0 ) - table.nums(self:GetPetList())
    return total
end

--获取背包容量
function PetData:GetBagCapacity()
    local other_info = self:GetOtherInfo()
    local total = self:GetPetStartBag() + ( other_info.extend_grid_num or 0 )
    return total
end

function PetData:GetLearnGiftLockCost()
    local quality = self:GetCurSelect():Quality()
    return self.bookup_price_cfg[quality]
end

function PetData:GetPatchIdByPetId(pet_id)
	local reward_group = 0
	local patch_id = 0
	local pet_group = Config.pet_cfg_auto.pet_group
	for k, v in pairs(pet_group) do
		if v.pet_id == pet_id then
			reward_group = v.reward_group
			break
		end
	end
	
	local pet_compose = Config.pet_cfg_auto.pet_compose
	for k, v in pairs(pet_compose) do
		if v.reward_group == reward_group then
			patch_id = v.patch_id
			break
		end
	end
	return patch_id
end

function PetData:GetPetIdByPatchId(patch_id)
	local reward_group = 0
	local pet_id = 0

	local pet_compose = Config.pet_cfg_auto.pet_compose
	for k, v in pairs(pet_compose) do
		if v.patch_id == patch_id then
			reward_group = v.reward_group
			break
		end
	end	
	
	local pet_group = Config.pet_cfg_auto.pet_group
	for k, v in pairs(pet_group) do
		if v.reward_group == reward_group then
			pet_id = v.pet_id
			break
		end
	end
	
	return pet_id
end

function PetData.IconId(data)
	if data.skin_id ~= 0 then
		local co = Cfg.PetSkinAttr(data.skin_id)
		return co and co.head_portrait or data.icon_id
	end
	return data.icon_id
end

--宠物选择界面要等指引触发了之后再根据是否选择完来打开
function PetData:NeedOpenPetSelect()
	local petsel_guide_cfg = GuideData.Instance:GuideCfg(8)--8是PetSelect指引的编号
	return TaskData.Instance:GetTaskIsCompleted(petsel_guide_cfg.trigger_param_1)--完成过任务了，没选择就打开
end

function PetData.CanLockNum(gift_num)
    if gift_num >= Config.pet_cfg_auto.other[1].skill_stage_2 then
        return 2
    elseif gift_num >= Config.pet_cfg_auto.other[1].skill_stage_1 then
        return 1
    else
        return 0
    end
end

function PetData:GetMaxPetOn()
    return self.pet_other_cfg.max_pet_on
end

function PetData:PetGetWayList()
    return self.pet_get_way_info
end

-- 获取宠物突破等级列表 is_all = ture 则拿self.pet_breach_all_list
function PetData:GetBreachList(pet,is_all)
    local lv = pet:Level()
    local cfg = self.pet_cfg_auto.pet_levelup
    if self.pet_breach_all_list == nil then
        self.pet_breach_all_list = {}
        for i = 1,#cfg do
            if cfg[i].is_need_breach == 1 then
                table.insert(self.pet_breach_all_list,cfg[i])
            end
        end
    end
    if is_all and is_all == true then
        return self.pet_breach_all_list
    end
    local pet_breach_list = {}
    for i = 1,#self.pet_breach_all_list do
        local v = self.pet_breach_all_list[i]
        if v.is_need_breach == 1 and v.level > lv then
            table.insert(pet_breach_list,v)
        end
        -- pet.info.is_up_level_breach == 0 当前等级没有被突破
        if v.is_need_breach == 1 and v.level == lv and pet.info.is_up_level_breach == 0 then
            table.insert(pet_breach_list,v)
        end
    end
    return pet_breach_list
end

-- 获取宠物最大等级
function PetData:GetMaxLv()
    return #self.pet_cfg_auto.pet_levelup
end

-- 是否能突破
function PetData:IsCanBreach(pet,is_check_stuff,is_check_state)
    if is_check_state == nil or is_check_state == true then
        local state = pet:State()
        if state ~= PetState.Fight and state ~= PetState.StandBy then
            return 0
        end
    end
    local pet_breach_list = self:GetBreachList(pet)
    -- 大于最大突破等级
    if next(pet_breach_list) == nil or pet:Level() > pet_breach_list[#pet_breach_list].level then
        return -1
    end
    if pet:IsNeedBreach(pet_breach_list) == true then
        -- 材料是否够
        if is_check_stuff == nil or is_check_stuff == true then
            local stuff_item_num = Item.GetNum(pet_breach_list[1].breach_consume)
            if stuff_item_num < pet_breach_list[1].breach_consume_num then
                return -2
            end
        end
        -- 经验是否满足突破条件
        for i,v in pairs(pet_breach_list) do
            if pet:Level() == v.level then
                return 1
            end
        end
    end
    return 0
end

-- 是否能突破 -- 返回3种情况下的标记 -- 
-- [1] = 检查是否需要突破，是否出战，是否材料够 -- 就是全部检查的意思
-- [2] = 检查是否需要突破，是否出战，
-- [3] = 检查是否需要突破，是否材料够
-- [4] = 检查是否需要突破
-- [5] = 是否到最大等级 -- 最大等级隐藏突破按钮
function PetData:IsCanBreach2(pet)
    local flag = {0,0,0,0,0}
    local state = pet:State()
    local breach_flag = 0    -- 是否需要突破标记
    local stuff_flag = 0     -- 材料是否足够标记
    local state_flag = 0     -- 是否出战标记
    local mac_level_flag = 0 -- 是否最大等级
    if state == PetState.Fight or state == PetState.StandBy then
        state_flag = 1
    end
    local pet_breach_list = self:GetBreachList(pet)
    -- 大于最大突破等级
    if next(pet_breach_list) == nil or pet:Level() > pet_breach_list[#pet_breach_list].level then
        mac_level_flag = 1
    end
    if mac_level_flag ~= 1 and pet:IsNeedBreach(pet_breach_list) == true then
        -- 材料是否够
        local stuff_item_num = Item.GetNum(pet_breach_list[1].breach_consume)
        if stuff_item_num >= pet_breach_list[1].breach_consume_num then
            stuff_flag = 1
        end
        -- 经验是否满足突破条件
        for i,v in pairs(pet_breach_list) do
            if pet:Level() == v.level then
                breach_flag = 1
                break
            end
        end
    end
    flag[1] = (breach_flag == 1 and stuff_flag == 1 and state_flag == 1 and mac_level_flag == 0) and 1 or 0
    flag[2] = (breach_flag == 1 and state_flag == 1 and mac_level_flag == 0) and 1 or 0
    flag[3] = (breach_flag == 1 and stuff_flag == 1 and mac_level_flag == 0) and 1 or 0
    flag[4] = (breach_flag == 1 and mac_level_flag == 0) and 1 or 0
    flag[5] = mac_level_flag
    return flag
end

-- 获取突破消耗的全部材料
function PetData:GetBreachAllStuff(pet)
    local lv = pet:Level()
    local breach_list = self:GetBreachList(pet,true)
    local item_num = 0
    local item = {}
    for i,v in pairs(breach_list) do
        if lv > v.level or (lv == v.level and pet.info.is_up_level_breach == 1) then
            item_num = item_num + v.breach_consume_num
            if item.item_id == nil then
                item.item_id = v.breach_consume
            end
        end
    end
    if item_num > 0 then
        item.num = item_num
        return item
    end
end

-- 突破红点 is_check_stuff：是否检查材料够不够 ，返回的是红点num，触发红点的pet
-- is_check_state 检查出战待机
function PetData:CheckBreachRemind(is_check_stuff, is_check_state)
    is_check_stuff = is_check_stuff == nil and true or is_check_stuff
    -- 每一只都要检查
    local list = self:GetPetList()
    if table.nums(list) == 0 then
        return 0
    end
    local pets = {}
    for k , v in pairs(list) do
        local breach_num = self:IsCanBreach(v,is_check_stuff, is_check_state)
        if breach_num > 0 then
            if not is_check_stuff then
                return 1,v
            else
                -- 有几个宠物都可突破的时候需返回最好的那个宠物出去。
                table.insert(pets,v)
            end
        end
    end
    if next(pets) == nil then
        return 0
    else
        pets = self:PetSort(pets)
        return 1,pets[1]
    end
end

-- 提醒主界面突破红点
function PetData:TriggerBreachRemind()
    self.pet_breach_remind_data.value = not self.pet_breach_remind_data.value
end

-- 突破属性
function PetData:SetPetBreachAttri(info)
    self.pet_breach_info.info = info
end

function PetData:GetPetBreachAttri()
    return self.pet_breach_info.info
end

function PetData:GetComsumePetList(comsume_id)
    local t = {}
    local comsume_list = TreasureHuntData.Instance.all_config.dis_material[comsume_id]
    local compose_pet_id = TreasureHuntData.Instance.exchange_smart.compose_pet_id
    if comsume_list == nil then
        return t
    end
    for k,v in pairs(self.pet_list) do
        if comsume_list[v.info.pet_id] ~= nil and v.info.str_level == comsume_list[v.info.pet_id].strengthen_num and v.pet_id ~= compose_pet_id and v:PrayMainIndex() == nil then
            local pet_t = {}
            pet_t.comsume_type = 0
            pet_t.seq = comsume_list[v.info.pet_id].seq
            pet_t.index = v.index
            pet_t.pet_id = v.info.pet_id
            table.insert(t, pet_t)
        end
    end
    -- for k,v in pairs(self.pet_storage_list) do
    --     if comsume_list[v.info.pet_id] ~= nil and v.info.str_level == comsume_list[v.info.pet_id].strengthen_num and v.pet_id ~= compose_pet_id then
    --         v.comsume_type = 1
    --         v.seq = comsume_list[v.info.pet_id].seq
    --         table.insert(t, v)
    --     end
    -- end
    return t
end

-- 宠物排序
function PetData:PetSort(pet_list)
    local function sort_func(a, b)
        local order_a = 100000
        local order_b = 100000
        --排序优先级（出战等状态>评分>等级>品质>强化等级>序号）
        if a:State() < b:State() then
            order_a = order_a + 100000
        elseif a:State() > b:State() then
            order_b = order_b + 100000
        end
        if a:Quality() > b:Quality() then
            order_a = order_a + 100
        elseif a:Quality() < b:Quality() then
            order_b = order_b + 100
        end
        if a:Level() > b:Level() then
            order_a = order_a + 1000
        elseif a:Level() < b:Level() then
            order_b = order_b + 1000
        end
        if a:GetScore() > b:GetScore() then
            order_a = order_a + 10000
        elseif a:GetScore() < b:GetScore() then
            order_b = order_b + 10000
        end
        if a:StrLevel() > b:StrLevel() then
            order_a = order_a + 10
        elseif a:StrLevel() < b:StrLevel() then
            order_b = order_b + 10
        end
        if a.index < b.index then
            order_a = order_a + 1
        elseif a.index > b.index then
            order_b = order_b + 1
        end
        return order_a > order_b
    end
    table.sort(pet_list, sort_func)
    return pet_list
end

-- 获取宠物评分，目前只是出战宠物的评分
function PetData.GetScore()
    local fight_pet = PetData.Instance:GetFightPetInfo()
    if fight_pet then
        return fight_pet:GetScore()
    else
        return 0
    end
end

function PetData.GetStandByScore()
    local score = 0
    for k,v in pairs(PetData.Instance:GetStandbyList()) do
        local pet = PetData.Instance:GetPetInfo(v)
        if pet then
            score = score + pet:GetScore()
        end
    end
    return score
end
--
function PetData:GetSuggestGroup(pet_normal_id)
    return self.pet_cfg_auto.skill_advise[pet_normal_id]
end

-- 设置宠物灵骑信息
function PetData:SetBicycle(pet,bicycle_info)
    pet.bicycle_info = bicycle_info
end

function PetData.GetRowCount(attr_config, low_attr_config, main_attribute)
    -- local strengthen_add_str = string.format(Language.Pet.PetStrengthGrowthAdd,tostring(attr_config.add_bp / 500), tostring(attr_config.add_bp / 100)).."\n"
    local main_attribute_add = attr_config[Language.Pet.AttrClass[main_attribute][5]] / 100
    local strengthen_add_str = string.format(Language.Pet.PetStrengthGrowthAddMain,tostring(attr_config.add_bp / 100), Language.Pet.AttrClass[main_attribute][4], tostring(main_attribute_add)).."\n"
    local row_count = 3
    if attr_config and low_attr_config and next(attr_config) ~= nil and next(low_attr_config) ~= nil then
        for k,v in pairs(SimpleAttrNameForType) do
            if attr_config[v] ~= nil and attr_config[v] > 0 then
                if low_attr_config then
                    if low_attr_config[v] and attr_config[v] - low_attr_config[v] > 0 then
                        strengthen_add_str = strengthen_add_str..string.format(Language.Pet.PetStrengthAttrAdd, Language.Common.AttrList[k], tostring(attr_config[v] - low_attr_config[v])).."\n"
                        row_count = row_count + 1
                    end
                else
                    strengthen_add_str = strengthen_add_str..string.format(Language.Pet.PetStrengthAttrAdd, Language.Common.AttrList[k], tostring(attr_config[v])).."\n"
                    row_count = row_count + 1
                end
            end
        end
    end
    return row_count, strengthen_add_str
end

function PetData:CheckPetLingWuRemind(pet_index)
    local function check(pet)
        if pet == nil then
            return 0
        end
        if pet:Quality() == ItemColor.White then
            return 0
        end
        local pet_cfg = pet.config
        local stuff_cfg = PetData.GetSlotTypeStuffCfg(pet_cfg.slot_type)
        local has_num = Item.GetNum(stuff_cfg.stuff_id)
        if pet:GetGiftNum() >= pet_cfg.gift_max then
            return 0
        elseif pet:GetGiftNum() == 0 then
            return 0
        end
        local index = pet:GetGiftNum() + 1 < 2 and 2 or pet:GetGiftNum() + 1
        local need_stuff = stuff_cfg["stuff_num"..tostring(index - 2)]
        if need_stuff == nil then
            local stuff_index = 1
            for k,v in pairs(stuff_cfg) do
                if stuff_cfg["stuff_num"..tostring(stuff_index)] ~= nil then
                    need_stuff = stuff_cfg["stuff_num"..tostring(stuff_index)]
                    stuff_index = stuff_index + 1
                else
                    break
                end
            end
        end
        return has_num >= need_stuff and 1 or 0
    end
    if pet_index ~= nil then
        if self:IsFightIndex(pet_index) or self:GetIsStandBy(pet_index) then
            return check(self:GetPetInfo(pet_index))
        else
            return 0
        end
    else
        local check_num = 0
        if self:GetFightIndex() >= 0 then
            check_num = check(self:GetFightPetInfo())
            if check_num > 0 then
                return check_num
            end
        end
        if next(self:GetStandbyList()) then
            for k,v in pairs(self:GetStandbyList()) do
                if v >= 0 then
                    local pet = self:GetPetInfo(v)
                    check_num = check(pet)
                    if check_num > 0 then
                        return check_num
                    end
                end
            end
            return 0
        else
            return 0
        end
    end
end

function PetData:CheckPetEquipRemind(pet_index)
    pet_index = pet_index or self:GetFightIndex()
    if pet_index == nil or pet_index < 0 then
        return
    end
    local pet = self:GetPetInfo(pet_index)
    if not pet then
        return 0
    end
    if next(BagData.Instance:GetPetEquipmentList()) and pet.info.pet_equipment_id == 0 then
        return 1
    end
    return 0
end

function PetData:GetPetStrTabbarTip()
    if next(self:GetPetList()) == nil then
        return Language.Pet.PetTabbarNotOpen
    else
        local open_level = FunOpen.Instance:GetFunOpenLevel(Mod.Pet.PetStrChange)
        local role_level = RoleData.Instance:GetRoleLevel()
        if role_level < open_level then
            return string.format(ErrorText[90], open_level)
        end
    end
end

-- 宠物详细属性数据
function PetData:GetOtherAttriListData()
    return PetOtherAttriCfg
end

function PetData:GetPetAttriValue(attr_type, pet_index)
    pet_index = pet_index or self:GetSelectPetIndex()
    if not pet_index then
        return 0
    end
    local attri_list = self:GetPetAttrInfo(pet_index)
    if TableIsEmpty(attri_list) then
        return 0
    end
    return attri_list[attr_type]
end
function PetData:SetScoreChange(old_score, new_score)
    self.old_score = old_score
    self.new_score = new_score
end
function PetData:GetScoreChange()
    return self.new_score, self.old_score
end

function PetData:GetAutoAddPoint(pet)
    local auto_info = pet:AutoAddInfo()
    local total_point = pet:PointNum()

    local param = {}
    param[Language.Pet.AttrClass[auto_info.index][1]] = total_point
    return param
end

function PetData:GetStrengthProtectCfg(pet)
    pet = pet or self:GetCurSelect()
    local quality_cfg = self.pet_cfg_auto.strengthen_protect[pet:Quality()]
    if quality_cfg then
        return quality_cfg[pet:StrLevel()]
    end
end

function PetData:GetPetShowList()
    local list = self:GetPetList()
    list = table.rearrange(list)
    list = self:PetSort(list)
    return list, #list > 0 and list[1].index or -1
end

function PetData:SetAutoClickGiftBook(book_config)
    self.auto_click_book_config = book_config
    if book_config then
        self.auto_click_book_page = self:GetGiftDetail(self.auto_click_book_config.param1,1).page
    end
end

-- 获取全部宠物的魂器数据
function PetData:GetWeaponList()
    local pet_list = self:GetPetList()
    local list = {}
    for i,v in pairs(pet_list) do
        if v:Weapon() ~= nil then
            table.insert(list, v:Weapon())
        end
        if v:Weapon2() ~= nil then
            table.insert(list, v:Weapon2())
        end
    end
    return list
end

-- 通过宠物原id找到变异id 暂时循环处理(宠物量不多，这地方用的也比较少) mutant_times = 1 普通变异，mutant_times = 2，飞升变异
function PetData:GetMutantId(pet_id, mutant_times)
    mutant_times = mutant_times or 1
    for i,v in pairs(self.pet_cfg_auto.pet_info_2) do
        if v.mutant_times == mutant_times and v.prototype_id == pet_id then
            return v.id
        end
    end
end

-- 设置强化选择的宠物数据
function PetData:SetSelPetStrData(pet_index, pet, pos_index,is_yao)
    pos_index = pos_index or self.sel_pet_str_data.click_index
    pet_index = pet_index or self:GetSelectPetIndex()
    self.sel_pet_str_data.pet_index = pet_index
    self.sel_pet_str_data.info[pos_index] = pet
    if is_yao ~= self.sel_pet_str_data.is_yao then
        self.sel_pet_str_data.is_yao = is_yao
    end
end

function PetData:GetSelPetStrData(pet_index, pos_index)
    if not pet_index or not pos_index then
        return
    end
    if self.sel_pet_str_data.pet_index ~= pet_index then
        return
    end
    return self.sel_pet_str_data.info[pos_index]
end

-- 清空强化选择的宠物数据
function PetData:ClearSelPetStrData(pet_index)
    self.sel_pet_str_data.pet_index = pet_index
    self.sel_pet_str_data.info = {}
end

-- 获取强化特殊配置
function PetData:GetSpecialStrengthen(pet)
    if pet == nil then
        return
    end
    local pet_id = pet:NormalId()
    local pet_str_level = pet:StrLevel()
    local cfg = Cfg.PetSpecialStrengthenConsume(pet_id,pet_str_level,false)
    return cfg
end

-- 获取强化选择副宠配置
function PetData:GetStrCfgListData(pet,is_yao)
    -- data:{qua, str_num, pet, pos_index, is_oneself, cfg}
    local list_data = {}
    if pet == nil then
        return list_data
    end
    local pet_qua = pet:Quality()
    local pet_str_level = pet:StrLevel()
    local cfg = self:GetSpecialStrengthen(pet) or Cfg.PetStrengthenConsume(pet_qua, pet_str_level)
    if PetFeiShengData.IsStrengthenNeedFS(cfg.strengthen_level,pet:FeiShengTimes()) then
        return list_data
    end
    local count = 3 -- cfg.material_num
    for i = 1, count do
        local qua = cfg["quality_" .. i]
        if qua ~= nil and qua > 0 then
            local data = {}
            data.qua = qua
            data.str_num = cfg["strengthen_level_" .. i]
            data.pos_index = i
            data.is_oneself = cfg["is_oneself_" .. i]
            data.pet = self:GetSelPetStrData(pet.index, i)
            data.cfg = cfg
            data.str_pet = pet
            data.is_yao = is_yao
            table.insert(list_data, data)
        end
    end
    return list_data
end

function PetData.GetPetSpecialGetWay(quality)
    for k, v in pairs(Config.pet_cfg_auto.pet_get_way) do
        if v.quality == quality then
            return v
        end
    end
end

-- 添加超书置换数据
function PetData:SetChangeBook(pos_index, item)
    self.pet_book_change_data.info[pos_index] = item
    self.pet_book_change_data.value = not self.pet_book_change_data.value
end

function PetData:GetChangeBook(pos_index)
    return self.pet_book_change_data.info[pos_index]
end

-- 清空超书置换数据
function PetData:ClearChangeBookData()
    self.pet_book_change_data.info = {}
end

-- 获取副宠的主宠index
function PetData:GetPrayMainPetIndex(vice_pet_index)
    local index = self.pray_info_data.vice_pet_list[vice_pet_index] or -1
    if index < 0 then
        return
    else
        return index
    end
end

-- 获取主宠的副宠列表
function PetData:GetPrayVicePetList(main_pet_index)
    local data = self.pray_info_data.pet_list[main_pet_index]
    return data
end

-- 获取祝福配置组
function PetData:GetPrayCfgGroup(pray_id)
    return self.pet_pray_effect_group[pray_id] or {}
end

-- 获取祝福配置
function PetData:GetPrayCfg(pray_id, level)
    return Cfg.PetPrayEffectCfg(pray_id, level)
end

-- 看下祝福特效是否触发，激活了哪几个宠物
function PetData:CheckPrayCfgPet(pray_cfg, pet_index_list)
    pet_index_list = pet_index_list or {}
    local vice_pet_info = {}
    local is_score = true      -- 是否评分都大于配置评分
    for i,v in pairs(pet_index_list) do
        local pet = PetData.Instance:GetPetInfo(v)
        if pet ~= nil then
            local pet_id = pet:NormalId()
            local score = pet:GetScore()
            vice_pet_info[pet_id] = score
            if score < pray_cfg.activate_score then
                is_score = false
            end
        end
    end
    local active_info = {}
    for i = 1, 3 do
        local pet_id = pray_cfg["activate_" .. i]
        local name = self:GetPetCfg(pet_id).name
        if vice_pet_info[pet_id] then
            name = ColorStr(name,COLORSTR.Blue9)
            active_info[i] = {value = true, name = name}
        else
            name = ColorStr(name,COLORSTR.Gray3)
            active_info[i] = {value = false, name = name}
            is_score = false
        end
    end
    return is_score, active_info
end

-- 获取祝福选择列表
function PetData:GetPraySelListData(pet)
    pet = pet or self:GetCurSelect()
    local list_data = {}
    local pet_list = self:GetPetList()
    local vice_list = pet:PrayVicePetList()
    if vice_list then
        for i,v in pairs(vice_list) do
            local pet = self:GetPetInfo(v)
            table.insert(list_data, {pet = pet})
        end
    end
    for i,v in pairs(pet_list or {}) do
        -- 出战和待机的宠物不可选择
        if not self:IsFightIndex(v.index) and not self:GetIsStandBy(v.index) and v.index ~= pet.index then
            -- 成为副宠和主宠也不选择
            if v:PrayMainIndex() == nil and v:PrayVicePetList() == nil then
                table.insert(list_data, {pet = v})
            end
        end
    end
    return list_data
end

-- 获取宠物祝福旧数据
function PetData:GetPrayOldData(pet_index)
    return self.old_pray_info_data[pet_index]
end

-- 宠物是否可销毁
function PetData:IsCanDestroy(pet)
    -- 擂台不可
    if pet:ArenaState() then
        return false, Language.Pet.PetDestroyTip.Tip2
    end
    -- 副宠不可
    if pet:PrayMainIndex() ~= nil then
        return false, Language.Pet.PetDestroyTip.Tip1
    end
    -- 出战不可
    if self:IsFightIndex(pet.index) then
        return false, Language.Pet.PetDestroyTip.Tip4
    end
    -- 待机不可
    if self:GetIsStandBy(pet.index) then
        return false, Language.Pet.PetDestroyTip.Tip3
    end
    return true,""
end


function PetData:GetGiftLvupCfg(skill_id,lv)
    return Cfg.PetGiftLvupCfg(skill_id,lv)
    -- local re = self.gift_skill_lvup_cfg[skill_id]
    -- if re == nil then
    --     BuglyReport("PetData:GetGiftLvupCfg ERROR,skill_id=",skill_id)
    -- end
    -- return re
end

function PetData:HasGiftLvupCfg(skill_id,lv)
    local cfg = Cfg.PetGiftLvupCfg(skill_id,lv or 1,false)
    return cfg ~= nil,cfg
    -- return self.gift_skill_lvup_cfg[skill_id] ~= nil
end

-- function PetData:GetGiftLv(skill_id)
--     local cfg = self:GetGiftLvupCfg(skill_id)
--     if cfg == nil then
--         return 1
--     end
--     for _,v in pairs(cfg) do
--         if v.src_skill_id == skill_id then
--             return v.intensify_level - 1
--         elseif v.dst_skill_id == skill_id then
--             return v.intensify_level
--         end
--     end
--     return 1
-- end

function PetData:GetGiftMaxLv(skill_id)
    local max_lv = 0
    while true do
        local has_cfg = self:HasGiftLvupCfg(skill_id,max_lv + 1)
        if has_cfg == false then break end
        max_lv = max_lv + 1
    end
    return max_lv + 1
end

-- 获取宠物数量
function PetData:GetPetNumByItemId(item_id, block_mutant)
    local pet_id = item_id
    local pet_id_1 = block_mutant and -1 or self:GetMutantId(pet_id, 1)
    local pet_id_2 = block_mutant and -1 or (self:GetMutantId(pet_id, 2) or -1)  -- 有些宠物没有2阶变异
    local num = 0
    for i,v in pairs(self.pet_list or {}) do
        local id = v.pet_id
        if id == pet_id or id == pet_id_1 or id == pet_id_2 then
            num = num + 1
        end
    end
    return num
end

-- 设置选中的标签页 --注意目前只设置了技能书那块的
function PetData:SetSelTab(mod_key)
    self.select_tab_data.mod_key = mod_key
end
function PetData:GetSelTab()
    return self.select_tab_data.mod_key or -1
end

-- 宠物是不是自己背包里面的
function PetData:IsInBag(pet)
    for i,v in pairs(self.pet_list) do
        if v == pet then
            return true
        end
    end
    return false
end

-- 获取可批量合成的宠物合成配置
function PetData:GetMultiPiecesCfg()
    local list = {}
    for i,v in pairs(Config.pet_cfg_auto.pet_compose) do
        if v.is_batch == 1 and Item.GetNum(v.patch_id) >= v.patch_num then
            table.insert(list, v)
        end
    end
    return list
end

-- 1 是批量合成，2是批量合成并分解
function PetData:SetMultiPiecesType(_type)
    self.multi_pieces_type = _type
end
function PetData:GetMultiPiecesType()
    return self.multi_pieces_type or 1
end

-- 宠物背包容量
function PetData:GetPetBagCapacity()
    local other_info = self:GetOtherInfo()
    local total = self:GetPetStartBag() + (other_info.extend_grid_num or 0)
    local has_num = 0
    for i,v in pairs(self.pet_list) do
        has_num = has_num + 1
    end
    return total,has_num
end

-- 是否支持批量合成
function PetData:IsCanMultiPieces(item_id)
    for i,v in pairs(Config.pet_cfg_auto.pet_compose) do
        if v.patch_id == item_id and v.is_batch == 1 and Item.GetNum(item_id) >= v.patch_num then
            return true
        end
    end
    return false
end

-- 本次洗练是否需要提醒
function PetData:IsXiLianTip(pet_index)
    return pet_index == self.xi_lian_tip_index
end


-- 宠物强化提醒
function PetData:GetStrengthenRemind(pet)
    if self.show_strengthen_remind[pet.index] then
        return 0
    end
    -- 状态判断
    local state = pet:State()
    if state ~= PetState.Fight and state ~= PetState.StandBy then
        return 0
    end
    local strengthen_cfg = self:GetSpecialStrengthen(pet) or Cfg.PetStrengthenConsume(pet:Quality(), pet:StrLevel())
    if TableIsEmpty(strengthen_cfg) then
        return 0
    end
    if strengthen_cfg.material_num == 0 then
        return 0
    end
    local max_str_level = Pet.StrengthMaxLv(pet:Quality())
    if pet:StrLevel() >= max_str_level then
        return 0
    end
    local list = {}
    -- 材料判断
    local pet_list = self:GetPetList()
    pet_list = table.rearrange(pet_list)
    table.sort(pet_list,function (a,b)
        return a:StrLevel() < b:StrLevel()
    end)
    for i = 1,strengthen_cfg.material_num do
        for k, v in pairs(pet_list) do
            if v.index ~= pet.index and not list[v.index] then
                if v:StrLevel() >= strengthen_cfg["strengthen_level_"..i] and v:Quality() == strengthen_cfg["quality_"..i] then
                    --非守擂/出战/待机宠物
                    if not v:ArenaState() and not self:IsFightIndex(v.index) and not self:GetIsStandBy(v.index) and not v:PrayMainIndex() then
                        --是否是消耗本体
                        if strengthen_cfg["is_oneself_"..i] == 1 then
                            if pet.config.mutant_id == v.config.mutant_id then
                                list[v.index] = true
                                break
                            end
                        else
                            list[v.index] = true
                            break
                        end
                    end
                end
            end
        end
    end
    local num = 0
    for i,v in pairs(list) do
        num = num + 1
    end
    if num >= strengthen_cfg.material_num then
        return 1
    else
        return 0
    end
end

function PetData:AllStrengthenRemind()
    local pet_list = self:GetPetList()
    for i,v in pairs(pet_list) do
        if self:GetStrengthenRemind(v) > 0 then
            return 1
        end
    end
    return 0
end

function PetData:GetPetRank(index)
    local pet = self:GetPetInfo(index)
    if pet then
        return pet:Rank()
    else
        return -1
    end
end

-- 获取宠物放生配置
function PetData:GetPetReturnCfg(pet)
    local pet_cfg = pet:PetCfg()
    local str_level = pet:StrLevel()
    local return_type = pet_cfg.return_type
    return Cfg.PetReturnCfg(return_type, str_level)
end

function PetData:GetShowPngPathByItemId(item_id)
    local isPet = Item.GetBigType(item_id) == ITEM_BIG_TYPE.PET
    if isPet then
        return string.format(PetConfig.PngPath, item_id)
    end
    return nil
end


function PetData:GetGaiMingAttrs()
    return self.pet_gai_ming_cfg.att_type
end

function PetData:GetGaiMingAttrValue(seq, level)
    local cfg =  self:GetGaiMingAttrs()[seq + 1]
    if level == 0 then
        return DataHelper.Percent(cfg.att_type, 0)
    end
    return DataHelper.Percent(cfg.att_type, cfg["level_"..level])
end
function PetData:GaiMingMax()
    return self.pet_gai_ming_cfg.other[1].up_max
end
function PetData:GaiMingUpExp()
    return self.pet_gai_ming_cfg.other[1].remould_id_exp
end
function PetData:GaiMingLevelInfo(level)
    return self.pet_gai_ming_cfg.remould_level[level + 1]
end
function PetData:GaiMingIsTuPo(pet)
    local info = pet:GetGaiMingInfo()
    local config = self:GaiMingLevelInfo(info.level)
    if config.break_item ~= 0 
    and info.exp >= config.up_exp then
       return true 
    end
    return false
end
function PetData:GaiMingUpItem()
    return self.pet_gai_ming_cfg.other[1].remould_id
end
function PetData:GaiMingTuPoItem()
    return self.pet_gai_ming_cfg.other[1].break_item
end

function PetData:GaiMingMaxLevel()
    return self.pet_gai_ming_cfg.other[1].max_level
end

function PetData:GaiMingFanHuan()
    return (self.pet_gai_ming_cfg.other[1].fangsheng_fanhuan / 100)
end