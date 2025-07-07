-- 宠物徽章词条数量
PET_BAGDE_ATTRI_COUNT = 2
PetBagde = PetBagde or BaseClass(Item)
function PetBagde:__init(vo)
    --LogError("宠物徽章ItemVo:",vo)
    vo = vo or {}
    self.index = vo.index or -1
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 1
    self.param = vo.param or {}
end

function PetBagde.DecodeItemParam()
    local info = {}
    MsgAdapter.ReadUChar()
    MsgAdapter.ReadUChar()
    info.item_id = MsgAdapter.ReadUShort()
    info.item_unique_id_1 = MsgAdapter.ReadUInt()  
    info.item_unique_id_2 = MsgAdapter.ReadUInt()
    info.skill_id = MsgAdapter.ReadInt()
    info.attri_list = {}
    for i = 1, PET_BAGDE_ATTRI_COUNT do
        local attri_info = {}
        attri_info.attri_type = MsgAdapter.ReadUShort()
        attri_info.attri_value = MsgAdapter.ReadUShort()
        if attri_info.attri_value > 0 then
            table.insert(info.attri_list, attri_info)
        end
    end
    for i = 1, 16 do
        MsgAdapter.ReadUChar()
    end
    if TableIsEmpty(info.attri_list) then
        return {}
    end
    return info

    --return--PetBagde.CreateVo(9010).param
end

-- 生成一个假的带净值的徽章用于测试
function PetBagde.CreateVo(item_id)
    local info = {}
    info.item_id = item_id
    info.skill_id = 0
    info.attri_list = {}
    for i = 1, PET_BAGDE_ATTRI_COUNT do
        local attri_info = {}
        attri_info.attri_type = GameMath.Ramdon(0, 4)
        attri_info.attri_value = GameMath.Ramdon(10, 20)
        info.attri_list[i] = attri_info
    end
    local vo = {}
    vo.item_id = item_id
    vo.param = info
    return vo
end

-- 被装备的宠物下标，用来判断是否被宠物使用
function PetBagde:PetIndex()
    return self.param.pet_index
end

-- 品质等级
function PetBagde:QuaLevel()
    if not self.qua_level then
        self.qua_level = self:Color()
    end
    return self.qua_level
end

function PetBagde:QuaCfg()
    if not self.qua_cfg then
        local qua_level = self:QuaLevel()
        self.qua_cfg = PetBagdeData.Instance:GetBagdeQuaCfg(qua_level)
    end
    return self.qua_cfg or {}
end

function PetBagde:AttriList()
    local qua_cfg = self:QuaCfg()
    if not qua_cfg then
        return {}
    end
    local attri_list = {}
    if self.param.attri_list then
        for i,v in pairs(self.param.attri_list) do
            attri_list[i] = v
            local attri_group_id = qua_cfg["att_" .. i]
            if attri_group_id then
                v.attri_cfg = PetBagdeData.Instance:GetAttriCfg(attri_group_id, v.attri_type)
            end
        end
    else 
        for i = 1, PET_BAGDE_ATTRI_COUNT do
            table.insert(attri_list, {})
        end
    end
    return attri_list
end

function PetBagde:SkillInfo()
    local qua_cfg = self:QuaCfg()
    if not qua_cfg.rate or qua_cfg.rate <= 0 then
        return nil
    end
    local skill_info = {}
    if self.param.skill_id and self.param.skill_id <= 0 then
        return
    end
    skill_info.skill_id = self.param.skill_id
    skill_info.skill_level = self.param.skill_level
    return skill_info
end

function PetBagde:SkillId()
    return self.param.skill_id
end

function PetBagde:SkillDesc()
    local skill_info = self:SkillInfo() or {}
    local skill_id = skill_info.skill_id
    if skill_id and skill_id > 0 then
        local desc = skill_id
        local cfg = PetData.Instance:GetGiftDetail(skill_id,level)
        if cfg then
            desc = cfg.description
        end
        return desc
    end
end



PetBagdeItemInfo = PetBagdeItemInfo or DeclareMono("PetBagdeItemInfo",ItemInfoViewItem)
function PetBagdeItemInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    UH.SetText(self.Title, Language.PetBagde.Txt5)
    UH.SetText(self.SkillTitle, Language.PetBagde.Txt6)
    self.item = self.info.item_list[self.ShowType]
    self:FlushPanel()
end

function PetBagdeItemInfo:FlushPanel()
    if not self.item.SkillInfo or self.item:ShowType() ~= ShowType.PetBagde then
        return
    end
    local skill_info = self.item:SkillInfo()
    self.EffectDesc:SetObjActive(skill_info ~= nil)
    self.SkillTitle:SetObjActive(skill_info ~= nil)
    if skill_info then
        local str = Language.PetBagde.Txt7
        if not TableIsEmpty(skill_info) then
            str = self.item:SkillDesc()
        end
        UH.SetText(self.EffectDesc, str)
    end

    local attri_list = self.item:AttriList() or {}
    self.AttriList:SetData(attri_list)
end

PetBagdeItemInfoAttriItem = PetBagdeItemInfoAttriItem or DeclareMono("PetBagdeItemInfoAttriItem", UIWidgetBaseItem)
function PetBagdeItemInfoAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if TableIsEmpty(data) then
        UH.SetText(self.NullTip, Language.PetBagde.Txt8)
        self.Info:SetActive(false)
    else
        UH.SetText(self.NullTip, "")
        self.Info:SetActive(true)
        local range_str = ""
        if not TableIsEmpty(data.attri_cfg) then
            local info1 = CommonStruct.AttrStruct(data.attri_type, data.attri_cfg.att_num_min or 0)
            local info2 = CommonStruct.AttrStruct(data.attri_type, data.attri_cfg.att_num_max or 0)
            range_str = string.format(Language.PetBagde.Txt9, info1.attr_percent_value, info2.attr_percent_value)
            range_str = ColorStr(range_str, COLORSTR.Yellow2)
        end

        local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
        UH.SetText(self.AttriName, string.format("%s:", attri_info.attr_name))
        UH.SetText(self.AttriValue, string.format("+%s %s", attri_info.attr_percent_value, range_str))
    end
end