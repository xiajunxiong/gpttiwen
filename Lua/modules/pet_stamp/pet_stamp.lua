-- 内含宠物神印 、 宠物方案

-- 宠物神印
PetStamp = PetStamp or BaseClass(Item)

function PetStamp.DecodeInfo()
    local info = {}
    info.lock = MsgAdapter.ReadChar()   -- 是否锁定
    info.color = MsgAdapter.ReadChar()  -- 品质
    info.level = MsgAdapter.ReadShort() -- 等级
    info.exp = MsgAdapter.ReadInt()     -- 经验
    info.max_exp = MsgAdapter.ReadInt() -- 最大经验
    info.pet_index = MsgAdapter.ReadInt()

    info.item_unique_id_1 = MsgAdapter.ReadUInt()
    info.item_unique_id_2 = MsgAdapter.ReadUInt()

    info.pos_index = MsgAdapter.ReadShort() -- 位置
    
    info.item_id = MsgAdapter.ReadUShort()

    info.main_attri = PetStamp.DecodeAttriInfo()
    info.main_attri.is_main = info.level

    info.suit_id = MsgAdapter.ReadShort()   -- 套装id
    info.attri_num = MsgAdapter.ReadUShort()
    info.add_attri_list = {}
    for i = 1, PetStampCfg.MaxAddAttriCount do
        local attri = PetStamp.DecodeAttriInfo(i)
        if attri.attri_value > 0 then
            if info.add_attri_list[attri.attri_type] then
                info.add_attri_list[attri.attri_type].attri_value = info.add_attri_list[attri.attri_type].attri_value + attri.attri_value
            else
                info.add_attri_list[attri.attri_type] = attri
            end
        end
    end
    --吞噬属性数量及属性（服务器字段为devour_num、devour_attr_list）
    info.swallow_num = MsgAdapter.ReadInt()
    info.swallow_attr = PetStamp.DecodeAttriInfo()
    return info
end

function PetStamp.DecodeAttriInfo(i)
    local attri_info = {}
    attri_info.attri_type = MsgAdapter.ReadShort()
    attri_info.attri_value = MsgAdapter.ReadShort()
    attri_info.attri_index = i and i - 1 or nil            --副属性的index，主属性和吞噬属性无index(服务器上序号为0~3)
    return attri_info
end

-- 必要属性 item_id
function PetStamp:__init(info,index)
    info = info or {item_id = 0, level = 1} 
    if info.param and info.param.stamp_info then
        self.info = info.param.stamp_info
    elseif info.vo then
        self.info = info.vo
    else
        self.info = info
    end
    index = index or 0
    self.index = index
    self.item_id = info.item_id or 0
    self.level = info.level or 1
    self.num = 1

    self.show_type = ShowType.PetStamp
end

-- 信息
function PetStamp:Info()
    return self.info
end

-- 背包位置
function PetStamp:Index()
    return self.index
end

-- 位置
function PetStamp:PosIndex()
    return self:ItemCfg().param2 or self.info.pos_index
end

-- 品质
function PetStamp:QuaLevel()
    return self:ItemCfg().param3 or self.info.qua_level or 0
end

-- 配置
function PetStamp:ItemCfg()
    if self.item_cfg == nil then
        self.item_cfg = PetStampData.Instance.item_cfg[self.item_id]
    end
    return self.item_cfg or {}
end

function PetStamp:StampCfg()
    if self.stamp_cfg == nil then
        self.stamp_cfg = Cfg.PetStampCfg(self:PosIndex(), self:QuaLevel())
    end
    return self.stamp_cfg
end

function PetStamp:ShowType()
    return self:ItemCfg().show_type or self.show_type
end

-- 名称
function PetStamp:Name()
    local cfg = self:ItemCfg()
    return cfg.name
end

function PetStamp:Type()
    local cfg = self:ItemCfg()
    return cfg.item_type
end

-- 等级
function PetStamp:Level()
    return self.level
end

-- 最大等级
function PetStamp:MaxLevel()
    local cfg = self:StampCfg()
    return cfg.level_max
end

-- 是否满级
function PetStamp:IsFullLevel()
    return self:Level() >= self:MaxLevel()
end

-- 经验
function PetStamp:Exp()
    if self.info.exp and self.info.exp < 0 then
        self.info.exp = 0
    end
    return self.info.exp or 0
end

function PetStamp:PriceExp()
    return PetStampData.Instance:GetStampPriceExp(self)
end

-- 图标
function PetStamp:IconId()
   
end

-- 图标
function PetStamp:Icon()
    return self:ItemCfg().icon_id
end

-- 品质框名字
function PetStamp:QuaFrameName()
    if self.qua_frame_name == nil then
        local qua = self:QuaLevel()
        local pos = self:PosIndex()
        self.qua_frame_name = string.format("pet_stamp_%s",qua)
    end
    return self.qua_frame_name
end

-- 是否锁定
function PetStamp:IsLock()
    if self.info.lock == nil or self.info.lock == 0 then
        return false
    else
        return true
    end
end

function PetStamp:TypeDesc()
    return
end

-- 套装id
function PetStamp:SuitId()
    return self:ItemCfg().param1 or self.info.suit_id
end
-- 全部属性
function PetStamp:AttriList()
    if self.attri_list == nil then
        self.attri_list = {}
        local main_attri = self:MainAttri()
        main_attri = TableCopy(main_attri)
        if main_attri then
            self.attri_list[main_attri.attri_type] = main_attri
        end
        local add_attri = self:AddAttri()
        if add_attri then
            for i,v in pairs(add_attri) do
                if self.attri_list[v.attri_type] then
                    self.attri_list[v.attri_type].attri_value = self.attri_list[v.attri_type].attri_value + v.attri_value
                else
                    self.attri_list[v.attri_type] = v
                end
            end
        end
    end
    return self.attri_list
end

-- 物品信息上显示的属性，主属性和附加属性分开
function PetStamp:AttriListInfo()
    if self.attri_list_info == nil then
        self.attri_list_info = {}
        local main_attri = self:MainAttri()
        if main_attri then
            table.insert(self.attri_list_info,main_attri)
        end
        local add_attri = self:AddAttri()
        local add_attri_count = 0
        if add_attri then
            for i,v in pairs(add_attri) do
                table.insert(self.attri_list_info,v)
                add_attri_count = add_attri_count + 1
            end
        end
        -- 哪一级加新属性条
        if add_attri_count < 4 and add_attri_count > 0 then
            local level = self:Level()
            local max_level = self:MaxLevel()
            local qua = self:QuaLevel()
            for i = level + 1, max_level do
                local cfg = Cfg.PetStampIntensifyCfg(qua, i)
                if cfg and cfg.deputy_attribute_up > 0 then
                    local data = {}
                    data.attri_type = -1
                    data.attri_value = 0
                    data.up_add_attri = cfg.levels
                    table.insert(self.attri_list_info,data)
                    break
                end
            end
        end
    end
    return self.attri_list_info
end

-- 神印提升至哪一级加副属性或者属性
function PetStamp:NextAdditionLevel()
    if self:IsFullLevel() then
        return
    end
    if self.next_addition_level == nil then
        local level = self:Level()
        local max_level = self:MaxLevel()
        local qua = self:QuaLevel()
        for i = level + 1, max_level do
            local cfg = Cfg.PetStampIntensifyCfg(qua, i)
            if cfg and cfg.deputy_attribute_up > 0 then
                return cfg.levels
            end
        end
    end
    return self.next_addition_level
end

function PetStamp:AttriListSort()
    if self.attri_list_sort == nil then
        self.attri_list_sort = {}
        for i,v in pairs(self:AttriList()) do
            table.insert(self.attri_list_sort,v)
        end
        self.attri_list_sort = PetStampData.Instance:AttriSort(self.attri_list_sort)
    end
    return self.attri_list_sort
end
-- 单个属性
function PetStamp:Attri(attri_type)
    local attri_list = self:AttriList()
    if not attri_list[attri_type] then
        return 0
    end
    return attri_list[attri_type].attri_value or 0
end
-- 主属性
function PetStamp:MainAttri()
    return self.info.main_attri
end
-- 附加属性
function PetStamp:AddAttri()
    return self.info.add_attri_list
end
-- 吞噬属性（通过吞噬获得的属性）
function PetStamp:SwallowAttr()
    return self.info.swallow_attr
end
--
function PetStamp:Swallowed()
    return self.info.swallow_num ~= nil and self.info.swallow_num > 0
end


-- 装在了哪个宠物上
function PetStamp:PetIndex()
    if self.info.pet_index and self.info.pet_index >= 0 then
        return self.info.pet_index
    end
end

-- 品质点点
function PetStamp:QuaShowPoint()
    local str = ""
    local qua = self:QuaLevel() or 0
    for i = 1,qua do
        str = str .. "1"
    end
    return str
end

function PetStamp:OneselfBaseIndex()
    return 3
end

function PetStamp:LevelDesc()
    return ""
end

-- 物品上的套装描述
function PetStamp:SuitDesc()
    if self.suit_desc == nil then
        local suit_id = self:SuitId()
        local qua = self:QuaLevel()
        if qua == ItemColor.Orange or qua == ItemColor.Red then
            self.suit_desc = PetStampData.Instance:GetSuitDescByQua(suit_id, 4, qua,Language.PetStamp.ItemTip3) or ""
        else
            self.suit_desc = Language.PetStampBlueprint.SuitTip6
        end
    end
    return self.suit_desc
end


-- =============== 神印方案 ================
-- 神印方案在此分为两种，pet_index > 0 为宠物身上的，否则为背包自定义的方案
PetStampBlueprint = PetStampBlueprint or BaseClass()

function PetStampBlueprint.DecodeInfo(not_name)
    local info = {}
    if not not_name then
        info.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        info.pet_index = MsgAdapter.ReadInt()
    end
    info.stamp_index_list = {}
    for i = 1, PetStampCfg.StampMaxPos do
        info.stamp_index_list[i] = MsgAdapter.ReadInt()
    end
    return info
end

function PetStampBlueprint:__init(info,index,pet_index)
    info = info or {}
    self.info = info
    self.stamp_index_list = info.stamp_index_list or {0,0,0,0,0}
    self.pet_index = pet_index or info.pet_index
    self.index = index or -1        -- 方案背包下标
    self.stamp_obj_list = info.stamp_obj_list or self:InitStampObjList()
end

function PetStampBlueprint:InitStampObjList()
    self.stamp_obj_list = {}
    for i = 1, PetStampCfg.StampMaxPos do
        self.stamp_obj_list[i] = PetStampData.Instance:GetPetStampByIndex(self.stamp_index_list[i])
    end
    return self.stamp_obj_list
end

function PetStampBlueprint:Index()
    return self.index
end

function PetStampBlueprint:PetIndex()
    if self.pet_index and self.pet_index < 0 then
        return
    end
    return self.pet_index
end

function PetStampBlueprint:Stamps()
    return self.stamp_index_list or {}
end

function PetStampBlueprint:StampObjs()
    return self.stamp_obj_list or {}
end

function PetStampBlueprint:GetStampByPos(pos_index)
    pos_index = pos_index or -1
    local index = self.stamp_index_list[pos_index]
    if index and index > 0 then
        return PetStampData.Instance:GetPetStampByIndex(index)
    end
    local stamp_obj = self.stamp_obj_list[pos_index]
    if stamp_obj and stamp_obj.item_id > 0 then
        return stamp_obj
    end
end

function PetStampBlueprint:Name(name)
    if name then
        self.info.name = name
    end
    if not self:IsOk() and not self:PetIndex() then
        self.info.name = string.format(Language.PetStampBlueprint.Name,self.index + 1)
    end
    return self.info.name
end

-- 方案是否合法
function PetStampBlueprint:IsOk()
    for i,v in pairs(self:StampObjs()) do
        if v.item_id > 0 then
            return true
        end
    end
    for i,v in pairs(self:Stamps()) do
        if v >= 1 then
            return true
        end
    end
    return false
end

-- 放入神印
function PetStampBlueprint:PutStamp(pet_stamp, is_edit_blueprint)
    local pet_index = self:PetIndex()
    if pet_index and not is_edit_blueprint then
        PetCtrl.SendPetOp(PET_OP_TYPE.PetStampUse, pet_index, self.index)
    else
        local pos = pet_stamp:PosIndex()
        if self.stamp_index_list[pos] then
            self.stamp_index_list[pos] = pet_stamp:Index()
            self.pet_index = -1
            PetStampCtrl.Instance:SetBlueprint(self)
        end
        if pet_index then
            self.pet_index = pet_index
        end
    end
end
-- 卸下神印
function PetStampBlueprint:UnfixStamp(pos_index)
    if not pos_index or pos_index > 5 or pos_index < 1 then
        return
    end
    local pet_index = self:PetIndex()
    if pet_index then
        if self.stamp_index_list[pos_index] > 0 then
            PetCtrl.SendPetOp(PET_OP_TYPE.PetStampUnfix, pet_index, pos_index)
        end
    else
        if self.stamp_index_list[pos_index] then
            self.stamp_index_list[pos_index] = -1
            PetStampCtrl.Instance:SetBlueprint(self)
        end
    end
end
-- 删除方案/一键卸下
function PetStampBlueprint:Delete()
    local pet_index = self:PetIndex()
    if not self:IsOk() then
        if pet_index then
            PublicPopupCtrl.Instance:Error(Language.PetStampBlueprint.Error1)
        else
            PublicPopupCtrl.Instance:Error(Language.PetStampBlueprint.Error2)
        end
        return
    end
    if pet_index then
        for i = 1, PetStampCfg.StampMaxPos do
            if self.stamp_index_list[i] > 0 then
                PetCtrl.SendPetOp(PET_OP_TYPE.PetStampUnfix, pet_index, i)
            end
        end
    else
        for i = 1, PetStampCfg.StampMaxPos do
            self.stamp_index_list[i] = -1
        end
        PetStampCtrl.Instance:SetBlueprint(self)
    end
end

-- 套装信息
function PetStampBlueprint:SuitInfo()
    local suit_list = PetStampData.Instance:GetBlueprintSuit(self)
    return suit_list
end

-- 属性信息
function PetStampBlueprint:AttriList()
    local list = PetStampData.Instance:GetBlueprintAddition(self)
    local new_list = {}
    local index = 0
    for i,v in pairs(list) do
        if index >= 4 then
            index = 0
        end
        index = index + 1
        if index > 2 then
            v.is_bg = true
        end
        table.insert(new_list,v)
    end
    return new_list
end

-- 复制
function PetStampBlueprint:Copy(blueprint)
    local pet_stamp_list = blueprint:Stamps()
    for i,v in pairs(pet_stamp_list) do
        self.stamp_index_list[i] = v
    end
    local pet_index = blueprint:PetIndex()
    if pet_index then
        self.pet_index = pet_index
    end
end
