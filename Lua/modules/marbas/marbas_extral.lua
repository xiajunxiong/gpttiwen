--造化装备·额外内容
--=========================MarbasEquip=======================
MarbasEquip = MarbasEquip or BaseClass(Item)
function MarbasEquip:__init(vo)
    vo = vo or RO_EMTPY_TAB
    self.index = vo.index or 0
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 0
    self.param = vo.param-- or {}
    self.in_bag = true
end

function MarbasEquip:TypeDesc()
    return nil
end 

function MarbasEquip:LevelDesc()
    local cfg = Item.GetConfig(self.item_id)
    return string.format(Language.Marbas.ItemInfo.EquipLevel,cfg.level_show)
end

function MarbasEquip:LimitProf()
    return Equip.GetLimitProf(self.item_id)
end

function MarbasEquip:LimitProfDesc()
    local co = Item.GetConfig(self.item_id)
    local prof = RoleData.ProfIdToProfType(co.limit_prof)
    return RichTextHelper.ColorStr(Language.ItemInfo.ProfDescEquip .. Language.Common.Prof[prof], COLORSTR.Yellow13)
end

function MarbasEquip:QuaName(shallow)
    local co = Item.GetConfig(self.item_id)
    if DEBUG_MODE then
        return ColorStr(co.name,self:ColorStr(shallow))
    else
        return ColorStr(co.name,self:ColorStr(shallow))
    end
end 

function MarbasEquip:BaseAttr() 
    local cfg = Item.GetConfig(self.item_id)
    local list = {}
    for i = 1,2 do 
        local vo = {}
        vo.attr_type = cfg["attr_type_"..i]
        vo.attr_value = cfg["attr_value_"..i]
        table.insert(list,vo)
    end 
    return list
    
end 

-- 装备基础信息 
function MarbasEquip:TypedAttrList()
    local list = self:BaseAttr()
    local fix_list = {}
    for k,v in pairs(list) do 
        if v.attr_value ~= 0 then 
            fix_list[v.attr_type] = v
        end 
    end

    return fix_list
end

-- 装备部位
function MarbasEquip:EquipType()
    local cfg = Item.GetConfig(self.item_id)
    return cfg ~= nil and cfg.equip_type or -1
end

-- 装备部位描述
function MarbasEquip:EquipTypeDesc()
    local cfg = Item.GetConfig(self.item_id)
    return cfg ~= nil and  Language.Common.Marbas[cfg.equip_type] or ""
end

-- 装备档次
function MarbasEquip:EquipGrade()
    local cfg = Item.GetConfig(self.item_id)
    return cfg ~= nil and cfg.equip_grade or 0
end

-- 装备档次描述
function MarbasEquip:EquipGradeDesc()
    local cfg = Item.GetConfig(self.item_id)
    return cfg ~= nil and Language.Marbas.Build.ATitle[cfg.equip_grade] or ""
end

-- 装等
function MarbasEquip:LevelShow()
    local cfg = Item.GetConfig(self.item_id)
    return cfg ~= nil and  cfg.level_show or 0
end

-- 有没有血祭石头
function MarbasEquip:HadStone()
    for k,v in pairs(self:GetStoneList()) do 
        if v.item_id > 0 then 
            return true
        end 
    end 
    return false
end

-- 卸下血祭的石头
function MarbasEquip:UnStone()
    if #self:GetStoneList() == 0 then return end 
    for i = 1,4 do 
        MarbasCtrl.Instance:SendBloodGemOper({
            req_type = BloodGemType.Take_Off,
            param1 = self.in_bag and BloodSrcType.EquipBag or BloodSrcType.Equiped,
            param2 = self:GridIndex(),
            param3 = 0,
            param4 = i-1,
            param5 = self.in_bag and BloodSrcType.EquipBag or BloodSrcType.Equiped,
            param6 = 0,
            param7 = 0,
            param8 = 0,
        })
    end 
end

-- 检查灵力值
function MarbasEquip:CheckMana()
    if self.param == nil or self.param.ling_li_value == nil then 
        return 0
    end 
    return self.param.ling_li_value
end

function MarbasEquip:ManaDesc()
    return Language.Marbas.BloodManaShow..self:CheckMana()
end

-- 检查魂力值
function MarbasEquip:CheckForce()
    if self.param == nil or self.param.hun_li_value == nil then 
        return 0
    end 
    return self.param.hun_li_value
end

function MarbasEquip:IsMaxForce()
    if self.param == nil or self.param.hun_li_value == nil then 
        return false
    end 
    return self.param.hun_li_value >= Config.zaohua_equip_up_auto.other[1].max_soul_power
end

-- 获取石头组
function MarbasEquip:GetStoneList()
    if self.param == nil or self.param.yuan_hun_zhu == nil then 
        return {}
    end 

    return self.param.yuan_hun_zhu
end

-- 获取石头（指定
function MarbasEquip:GetStone(index)
    if self.param == nil or self.param.yuan_hun_zhu == nil then 
        return nil
    end 
    return self.param.yuan_hun_zhu[index]
end

function MarbasEquip:CheckUnique(param)
    local flag = true
    for k,v in pairs(param) do 
        flag = flag or v == self.vo["item_unique_id_"..i] 
    end 

    return flag
end 

function MarbasEquip:Unique() 
    return {[1] = self.vo.item_unique_id_1,[2] = self.vo.item_unique_id_2}
end 

function MarbasEquip:Compare()
    if self.in_bag then
        return MarbasData.Instance:MarbasEquip(self:EquipType())
    end 
    return nil
end

function MarbasEquip:GetWay()
    if not self.param or not self.param.show_way then 
        return nil
    end 

    local config = Item.GetConfig(self.item_id)
    if config == nil then
        return nil
    end
    if config.get_way == nil then
        return nil
    end
    if config.get_way == "" then
        return nil
    end
    return string.split(config.get_way or "", "|")
end

-- 元魂珠
--=========================MarbasGem=======================
MarbasGem = MarbasGem or BaseClass(Item)
function MarbasGem:__init(vo)
    self.vo = vo or RO_EMTPY_TAB
    self.index = vo.index or 0
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 0
    self.param = vo.param-- or {}
    self.in_bag = true
end 

function MarbasGem:GemInfo()
    if self.param == nil or self.param.attrs == nil then 
        return {}
    end 

    return self.param.attrs
end

function MarbasGem:GemAttrSeqInfo()
    if self.param == nil or self.param.attr_seq == nil then 
        return {}
    end 

    return self.param.attr_seq
end

function MarbasGem:GemLevel()
    local pearl_cfg = MarbasData.Instance:GetPearlCfg(self.item_id)
    return pearl_cfg ~= nil and pearl_cfg.yuan_soul_level or 0
end

-- 元魂珠碎片
--=========================MarbasGemClip=======================
MarbasGemClip = MarbasGemClip or BaseClass(Item)
function MarbasGemClip:__init(vo)
    self.vo = vo or RO_EMTPY_TAB
    self.index = vo.index or 0
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 0
    self.param = vo.param-- or {}
    self.in_bag = true
end