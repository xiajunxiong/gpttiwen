-- 和服务器对应
ITEM_BIG_TYPE = {
    INVALID = -1, -- -1 无效
    EXPENSE = 0, -- 0 消耗类 能直接使用
    OTHER = 1, -- 1 被动使用类 type value 最好不要直接使用
    GIFT = 2, -- 2 礼包类
    -- GIFT2 = 3,                                       -- 3 礼包类 临时策划改完删掉
    MEDAL = 3, -- 勋章
    VIRTUAL = 4,
    EQUIP_SPIRIT = 6,         --铸灵
    PARTNER_PACT = 7, -- 伙伴元神契约
    LING_BAO = 8, --灵宝
    EQUIPMENT = 100, -- 装备
    PET = 101, -- 宠物
    ELE_CRYSTAL = 102, -- 元素水晶
    JEWELRY = 103, -- 灵饰
    EXP_CRYSTAL = 104, -- 打卡水晶
    TREASURE_MAP = 105, -- 藏宝图
    PARTNER_EQUIPMENT = 106, -- 伙伴装备
    PET_EQUIPMENT = 107,        --宠物装备
    PET_WEAPON = 109,   -- 宠物魂器
    PET_STAMP = 110,    -- 神印
    ACCRSSORY = 111,    --配饰
    DIAN_HUA_SHI = 112, --点化石
    PET_BAGDE = 113,    --宠物徽章
    PEAK_TOKEN = 114,    --天下第一
    MARBAS = 115,      -- 造化装备
    SHENG_QI = 116,     -- 圣器
    FU_SHI = 117,       -- 符石
    MARBAS_GEM = 118,  -- 元魂珠
    FU_YU = 119,    --符玉
    HuanShouBook = 120,     -- 幻兽技能书
    DRAGON_SOUL = 121,   --龙魂
    YouMing = 122,      --幽冥
    YouMingEquip = 123, --冥装
}

-- 虚拟背包类型 对应 ：： bag_classification字段
BAG_TYPE = {
    ROLE = 0,       -- 人物背包
    PET = 1,        -- 宠物
    PET_STAMP = 2,  -- 神印
    PARTNER = 3,    -- 伙伴
    ZHAO_HUA = 4,   -- 造化
    SHENG_QI = 5,   -- 圣器
    FUSHI = 6,      -- 符石
    HUANSHOU = 7,   -- 幻兽背包
}

local hide_limit_level = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 3, 1, 1, 1, 4, 1, 1, 1, 1, 2, 1, 1, 1}

local  item_all_config = {
    [ITEM_BIG_TYPE.EXPENSE] = Config.expense_auto,
    [ITEM_BIG_TYPE.OTHER] = Config.other_auto, --其他
    [ITEM_BIG_TYPE.GIFT] = Config.gift_auto, --消耗
    --[ITEM_BIG_TYPE.GIFT2] = Config.gift_2_auto,
    [ITEM_BIG_TYPE.VIRTUAL] = Config.virtual_auto,
    [ITEM_BIG_TYPE.PARTNER_PACT] = Config.star_deed_auto,
    [ITEM_BIG_TYPE.EQUIPMENT] = Config.equipment_auto,
    [ITEM_BIG_TYPE.PET] = Config.pet_auto,
    [ITEM_BIG_TYPE.ELE_CRYSTAL] = Config.element_crystal_cfg_auto,
    [ITEM_BIG_TYPE.JEWELRY] = Config.ornament_item_auto,
    [ITEM_BIG_TYPE.EXP_CRYSTAL] = Config.exp_crystal_auto,
    [ITEM_BIG_TYPE.MEDAL] = Config.medal_item_auto,
    [ITEM_BIG_TYPE.TREASURE_MAP] = Config.treasure_map_item_auto,
    [ITEM_BIG_TYPE.PARTNER_EQUIPMENT] = Config.partner_equipment_auto,
    [ITEM_BIG_TYPE.PET_EQUIPMENT] = Config.pet_equipment_item_auto,
    [ITEM_BIG_TYPE.PET_WEAPON] = Config.pet_weapon_item_auto,
	[ITEM_BIG_TYPE.EQUIP_SPIRIT] = Config.spirit_stone_auto,
    [ITEM_BIG_TYPE.PET_STAMP] = Config.pet_god_print_item_auto,
    [ITEM_BIG_TYPE.ACCRSSORY] = Config.peishi_item_auto,
    [ITEM_BIG_TYPE.DIAN_HUA_SHI] = Config.dianhuashi_auto,
    [ITEM_BIG_TYPE.PET_BAGDE] = Config.pet_badges_item_auto,
    [ITEM_BIG_TYPE.PEAK_TOKEN] = Config.supreme_token_cfg_auto,
    [ITEM_BIG_TYPE.MARBAS] = Config.immortal_equip_auto,
    [ITEM_BIG_TYPE.SHENG_QI] = Config.shengqi_item_auto,
    [ITEM_BIG_TYPE.FU_SHI] = Config.fushi_auto,
    [ITEM_BIG_TYPE.MARBAS_GEM] = Config.yuan_soul_pearl_auto,
    [ITEM_BIG_TYPE.FU_YU] = Config.fuyu_item_auto,
    [ITEM_BIG_TYPE.HuanShouBook] = Config.huanshou_skill_auto,
    [ITEM_BIG_TYPE.LING_BAO] = Config.lingbao_item_auto,
    [ITEM_BIG_TYPE.DRAGON_SOUL] = Config.dragon_soul_item_auto,
    [ITEM_BIG_TYPE.YouMing] = Config.dim_pet_auto,
    [ITEM_BIG_TYPE.YouMingEquip] = Config.nether_costume_auto,
}

local function item_plus_create(type,vo)
    if ITEM_BIG_TYPE.EQUIPMENT == type then 
        return Equip.New(vo)
    elseif ITEM_BIG_TYPE.ELE_CRYSTAL == type then 
        return EleCrystal.New(vo)
    elseif ITEM_BIG_TYPE.JEWELRY == type then 
        return Jewelry.New(vo)
    elseif ITEM_BIG_TYPE.PARTNER_EQUIPMENT == type then 
        return PartnerEquipment.New(vo)
    elseif ITEM_BIG_TYPE.MEDAL == type then 
        return Medal.New(vo)
    elseif ITEM_BIG_TYPE.PET_WEAPON == type then
        return PetWeapon.New(vo)
    elseif ITEM_BIG_TYPE.EQUIP_SPIRIT == type then
        return EquipSpiritBagItem.New(vo)
    elseif ITEM_BIG_TYPE.PET_STAMP == type then
        return PetStamp.New(vo, vo.index)
    elseif ITEM_BIG_TYPE.PARTNER_PACT == type then
        return PartnerPact.New(vo, vo.index)
    elseif ITEM_BIG_TYPE.ACCRSSORY == type then
        return Accrssory.New(vo)
    elseif ITEM_BIG_TYPE.DIAN_HUA_SHI == type then
        return NevealStone.New(vo)
    elseif ITEM_BIG_TYPE.PET_BAGDE == type then
        return PetBagde.New(vo)
    elseif ITEM_BIG_TYPE.MARBAS == type then
        return MarbasEquip.New(vo)
    elseif ITEM_BIG_TYPE.SHENG_QI == type then
        return ShengQi.New(vo)
    elseif ITEM_BIG_TYPE.FU_SHI == type then
        return FuShi.New(vo)
    elseif ITEM_BIG_TYPE.MARBAS_GEM == type then
        return MarbasGem.New(vo)
    elseif ITEM_BIG_TYPE.FU_YU == type then
        return FuYuItem.New(vo)
    elseif ITEM_BIG_TYPE.LING_BAO == type then
        return LingBao.New(vo)
    elseif ITEM_BIG_TYPE.DRAGON_SOUL == type then
        return FairyDragonSoul.New(vo)
    elseif ITEM_BIG_TYPE.YouMingEquip == type then
        return YouMingEquip.New(vo)
    else
        return Item.New(vo)
    end 
end
--========================继承Item须知========================
--单独模块的逻辑需在自身重写函数实现，不可在Item做特殊判断！！
--如Name() 如果Equip的Name()与Item的逻辑不同，在Equip重写Name()即可
--============================================================
Item = Item or BaseClass()

function Item.Create(vo)
    vo = vo or RO_EMTPY_TAB
    return item_plus_create(Item.GetBigType(vo.item_id),vo)
end

function Item.Init(item_id, num, is_bind)
    local vo = Item.InitData(item_id, num, is_bind)
    return Item.Create(vo)
end

function Item.InitData(item_id, num, is_bind)
    return {
        item_id = item_id or 0,
        num = num or 1,
        is_bind = is_bind or 0                      
    }
end

function Item:__init(vo)
    vo = vo or RO_EMTPY_TAB
    self.vo = vo
    self.index = vo.index
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 0
    self.param = vo.param-- or {}
    self.is_grey = vo.is_grey
    self.item_unique_id_1 = vo.item_unique_id_1
    self.item_unique_id_2 = vo.item_unique_id_2
    self.need_num = vo.need_num or 0
    self.jump_need_num = vo.jump_need_num or 0
    self.invalid_time = vo.invalid_time or 0
end

function Item:Name()
    return Item.GetName(self.item_id)
end

--是否浅色背景
--shallow true代表浅色背景
function Item:QuaName(shallow)
    local co = Item.GetConfig(self.item_id)
    if co == nil then
        LogError(tostring(self.item_id) .. " 不存在")
        return ""
    end
    --危险！！！！！！！！！！！！！！！！！！！！！！！！
    --单独模块的逻辑需在自身重写函数实现，不可在Item的函数做特殊判断！！！
    --如Name() 如果Equip的Name()与Item的逻辑不同，在Equip重写Name()即可
    if self:IsGrayEleCrystal() then
        return ColorStr(Format(Language.Common.ZeroCount, co.name),COLORSTR.Gray2)
    else
        return ColorStr(co.name,self:ColorStr(shallow))
    end
end

function Item:ColorStr(shallow)
    local co = Item.GetConfig(self.item_id)
    local shallowcolor = shallow and QualityColorStrShallow or QualityColorStr
    return co and shallowcolor[co.color] or shallowcolor[ItemColor.White]
end

function Item:IsGrayEleCrystal()
    return self:Type() == ItemType.EleCrystal and self.param and self.param.used_times and self.param.used_times == 0
end

function Item:SpecialId()
    if self.param then
        return self.param.sp_id
    end
    return 0
end

function Item:GridIndex()
    return self.index
end

function Item:GridColumn()
    return self.column_type
end

function Item:NameId()
    if DEBUG_MODE then
        return string.format("%s (%d)",Item.GetName(self.item_id),self.item_id)
    else
        return Item.GetName(self.item_id)
    end
end

function Item:Color()
    return Item.GetColor(self.item_id)
end

function Item:Type()
    return Item.GetType(self.item_id)
end

function Item:Desc()
    return Item.GetDesc(self.item_id)
end

function Item:GiftDesc()
    if Item.GetDesc(self.item_id) == "" and BagData.IsHasGift(self.item_id) then
        -- item_num = item_num == 0 and 1 or item_num
        local config = BagData.Instance:GetGiftItemList(self.item_id)
        local ItemGiftDesc = Language.ItemInfo.ItemGiftDesc
        local temp = ItemGiftDesc.item_num
        if config.select_num and config.select_num ~= 0 then
            temp = Format(ItemGiftDesc.select_num, config.select_num)
        elseif config.rand_num and config.rand_num ~= 0 then
            temp = Format(ItemGiftDesc.rand_num, config.rand_num)
        end

        for k, v in pairs(config.item_list) do
            temp = Format("%s\n%s x%s", temp, Item.GetQuaName(v.item_id), v.num)
        end
        return temp
    end
    return Item.GetDesc(self.item_id)
end

function Item:DescInfoRect(width,font)
    local str = self:Desc()
    if str == "" then return {w = width , h = 1} end 
    local str_list = string.split(str, "\n")
    local line_num = 0
    for k,v in pairs(str_list) do 
        local num_desc = DataHelper.GetStringWordNum(v)
        local num_limited = width / font
        line_num = line_num + (math.floor(num_desc % num_limited) > 0 and math.floor(num_desc / num_limited) + 1 or math.floor(num_desc / num_limited))
    end 
    local high = line_num * font + (line_num - 1) * (font/2) + 10
    return {w = width , h = high}
end
function Item:GetData()
    return self
end
function Item:Bind()
    if type(self.is_bind) == "number" then
        return self.is_bind == 1
    end
    return self.is_bind
end

function Item:Config()
    return Item.GetConfig(self.item_id)
end

function Item:IsVirtualItem()
    return nil ~= Config.virtual_auto[self.item_id]
end

--危险！！！！！！！！！！！！！！！！！！！！！！！！
--单独模块的逻辑需在自身重写函数实现，不可在Item的函数做特殊判断！！！
--如Name() 如果Equip的Name()与Item的逻辑不同，在Equip重写Name()即可
function Item:TypeDesc()
    local type_id = Item.GetTypeDesc(self.item_id)
    if self.IsEquip and self:IsEquip() then
        -- local co = Item.GetConfig(self.item_id)
        -- local limit_prof = co.limit_prof
        -- front = Language.ItemInfo.TypeDesc.Equip
        -- return Language.Common.EquipType[co.equip_type2]
        return nil
    end
    if self.IsJewelry and self:IsJewelry() then
        return nil
        -- return Language.Common.Jewelry[Jewelry.GetJewelryType(self.item_id)]
    end
    local co = Item.GetConfig(self.item_id)
    if co and co.show_type then
		if co.show_type == ShowType.AttributeDan then
			local attributedan = AttributeDanData.Instance:GetAttibuteDanByItemId(self.item_id)
			if attributedan then
				if attributedan.attribute_pellet_type == 0 then
				    return {string.format(Language.Workshop.AttrStr2, Language.Common.AttrList[attributedan.attr_type], attributedan.attr_value)}
				end
			end
		end
        return Language.Common.ShowType[co.show_type]
    end
    if co and co.show_class then
        if co.show_class ~= "" then
            return co.show_class
        else
            return nil
        end
    end
    return Language.Common.ItemType[type_id]
end
--危险！！！！！！！！！！！！！！！！！！！！！！！！
--单独模块的逻辑需在自身重写函数实现，不可在Item的函数做特殊判断！！！
--如Name() 如果Equip的Name()与Item的逻辑不同，在Equip重写Name()即可
function Item:LimitProfDesc()
    if (self.IsEquip and self:IsEquip()) or (self.IsJewelry and self:IsJewelry()) or (self.IsMarbas and self:IsMarbas()) then
        local co = Item.GetConfig(self.item_id)
        local limit_prof = co.limit_prof
        local prof = limit_prof > 0 and limit_prof / 100 or limit_prof
        return RichTextHelper.ColorStr(Language.ItemInfo.ProfDescEquip .. Language.Common.Prof[prof], (prof > 0 and RoleData.Instance:ProfType() ~= prof) and COLORSTR.Red10 or COLORSTR.Yellow13)
    end
end

--危险！！！！！！！！！！！！！！！！！！！！！！！！
--单独模块的逻辑需在自身重写函数实现，不可在Item的函数做特殊判断！！！
--如Name() 如果Equip的Name()与Item的逻辑不同，在Equip重写Name()即可
function Item:Level()
    local co = Item.GetConfig(self.item_id)
    if co == nil then
        return
    end
    if co.show_type then
        if hide_limit_level[co.show_type] == 1 then
            return
        else
            local res = Language.Common.ShowTypeDesc[hide_limit_level[co.show_type]]
            if co.show_type == ItemType.Dimensity then
                return {string.format(res, self.param.used_times or co.num_use)}
            elseif co.show_type == ItemType.EleCrystal then
                local info = WorkshopData.Instance:GetStoneInfo(self.item_id)
                return {string.format(Language.Workshop.AttrStr2, Language.Common.AttrList[info.type_1], info.value_1)--[[ string.format(res, info.level) ]]}
            else
                --[[ if self.param.scene_id then
                    local cfg = SceneData.Instance:GetSceneCfg(self.param.scene_id)
                    return {string.format(res, cfg.name, self.param.x, self.param.y)}
                end ]]
            end
        end
    end
    if co.limit_level == nil then
        return nil
    end
    return self:LimitLv()
end

--危险！！！！！！！！！！！！！！！！！！！！！！！！
--单独模块的逻辑需在自身重写函数实现，不可在Item的函数做特殊判断！！！
--如Name() 如果Equip的Name()与Item的逻辑不同，在Equip重写Name()即可
function Item:LevelDesc()
    local cfg = Item.GetConfig(self.item_id)
    if self:Level() ~= nil then 
        if type(self:Level()) == "table" then 
            if cfg.without_level_limit and cfg.without_level_limit > 0 then -- 需要替换展示 
                return Language.ItemInfo.ItemTitles.WithoutLevelLimit
            end
            return self:Level()[1]
        else
            if self.param and self.param.strengthen_level and self.param.strengthen_level > 0 and self:Color() >= 7 then
                -- 神装
				local res = Language.Common.ShowTypeDesc[self:Color()]
                local equip_level = DivineCostumeData.Instance:GetChangeShowLevel(self)
				return Format(res, self.param.strengthen_level, equip_level)
            elseif cfg.without_level_limit and cfg.without_level_limit > 0 then -- 需要替换展示
                if self:IsStarGod() then
                    return Language.ItemInfo.ItemTitles.WithoutLevelLimit
                else 
                    return Language.ItemInfo.ItemTitles.SpecialWithoutLevelLimit
                end 
            elseif cfg.show_type == ShowType.GiftBag then
                -- 礼包需要显示使用等级，并且等级不足用红色
                if cfg.limit_level ~= nil and cfg.limit_level ~= "" then
                    local lv_limit = RoleLevel() >= cfg.limit_level and cfg.limit_level or ColorStr(cfg.limit_level, COLORSTR.Red8)
                    return Format(Language.ItemInfo.ItemTitles.UseLevel, lv_limit) 
                end
            end
            return string.format(Language.ItemInfo.ItemTitles.UseLevel, self:Level()) 
        end 
    elseif ( ViewMgr:IsOpen(PreviewGiftView) or ViewMgr:IsOpen(ChooseGiftView)) 
        and (self:Type() >= ItemType.Normal and self:Type() <= ItemType.Cloth ) then
            local show_type = Item.GetShowType(self.item_id)
            local num = 0
            if show_type == ShowType.Pet then
                num = PetData.Instance:GetPetNumByItemId(self.item_id)
            else
                num = Item.GetNum(self.item_id)
            end
        return Language.ItemInfo.ItemTitles.HaveNum .. num
    else
        return self:Level()
    end 
end

function Item:PreRange()
    local flag = (self:IsEquip() or self:IsJewelry()) and self.param ~= nil and not self.block_pre_range
    return flag == true and flag or nil
end

function Item:LimitLv()
    return Item.GetLimitLv(self.item_id)
end

function Item:Price()
    return Item.GetPrice(self.item_id)
end

function Item:MinPrice()
    return Item.GetMinPrice(self.item_id)
end

function Item:MaxPrice()
    return Item.GetMaxPrice(self.item_id)
end

function Item:RecommendPrice()
    return Item.GetRecommendPrice(self.item_id)
end

function Item:IconId()
    return Item.GetIconId(self.item_id)
end

function Item:ModKey()
    return Item.GetModKey(self.item_id)
end

function Item:Num()
    if self.num == 0 then
        return Item.GetNum(self.item_id)
    end
    return self.num
end

function Item:IsEquip()
    return Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.EQUIPMENT
end

function Item:IsCrystal()
    return Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.ELE_CRYSTAL
end

function Item:IsTreasure()
    return Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.TREASURE_MAP
end
function Item:IsMedal()
    return Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.MEDAL
end

function Item:IsPet()
    return Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.PET
end

function Item:IsJewelry()
    return Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.JEWELRY
end

function Item:IsNevealStone()
    return Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.DIAN_HUA_SHI
end

function Item:IsAccrssory()
    return Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.ACCRSSORY
end

function Item:IsGem()
    return Item.GetType(self.item_id) == ItemType.Gem
end

function Item:IsMarbas()
    return Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.MARBAS
end

function Item:IsMarbasGem()
    return Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.MARBAS_GEM
end

function Item:IsFuYu()
    return Item.GetType(self.item_id) == ItemType.FuYu--Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.FU_YU
end

function Item:MapType()
    return Item.GetConfig(self.item_id).is_sp_map
end

function Item:IsSuperMap()
    return Item.GetConfig(self.item_id).is_sp_map == 1
end

function Item:IsNormalMap()
    return Item.GetConfig(self.item_id).is_sp_map == 2
end

function Item:IsStarTreasureMap()
    return Item.GetConfig(self.item_id).is_sp_map == 3
end
function Item:IsShengQiMap()
    return Item.GetConfig(self.item_id).is_sp_map == 4
end

function Item:IsActiveMap()
    return self.param.is_active_map == 1
end

function Item:IsDeposit()
    return Item.GetConfig(self.item_id).is_deposit == 1
end

--是否是神装
function Item:IsStarGod()
    return (self:IsEquip() or self:IsJewelry()) and self:Color() >= ItemColor.Star
end

function Item:TypeName()
    return Language.Common.ItemType[self:Type()]
end

function Item:Compare()
    return nil
end

function Item:AnotherCompare()
    return nil
end

function Item:IsCanUse()
    return Item.GetIsCanUse(self.item_id)
end

function Item:IncAttr()
    -- if self.param == nil then
        return false, nil
    -- end
    --获取到之后进行范围取品质
    --[[ for i, v in ipairs(self.param.attr_extra_inc) do
        if v.attr_value > 0 then
            WorkshopData.Instance:SmeltAttrQua(Equip.GetEquipLevel(), v)
        end
    end
    return self.param.inc_num > 0, self.param.attr_extra_inc ]]
end

function Item:GetWay()
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


function Item:CfgAttr(item_id)
    local co = Item.GetConfig(item_id or self.item_id)
    local temp = {}
    if co then
        for k, v in pairs(AttrTypeForName) do
            if co[k] and co[k] > 0 then
                temp[v] = co[k]
            end
        end
        return temp
    end
    return {}
end
function Item:ConfigAttr(item_id)
    local co = Item.GetConfig(item_id or self.item_id)
    local list = {}
    if co then
        for k, v in pairs(AttrTypeForName) do
            if co[k] and co[k] > 0 then
                table.insert(list, {attr_type = v, attr_value = co[k]})
            end
        end
        return list
    end
    return list
end

function Item:AttrSection()
    local co = Item.GetConfig(self.item_id)
    local temp = {}
    for key, value in pairs(AttrTypeForName) do
        if co["max_" .. key] ~= nil and co["max_" .. key] > 0 then
            table.insert(temp, {attr_type = value, min = co["min_" .. key], max = co["max_" .. key]})
        end
    end
    return temp
end

function Item:BaseAttr()
    return self.param and self.param.attr_list
end

-- function Item:AttrJewelrySection()
--     local co = Item.GetConfig(self.item_id)
--     local temp = {}
--     if co.attr_min > 0 or co.attr_max > 0 then 
--         table.insert(temp, {attr_type = co.attr_type, min = co.attr_min, max = co.attr_max})
--     end 

--     if  co.attr_min_2 > 0 or co.attr_max_2 > 0 then 
--         table.insert(temp, {attr_type = co.attr_type_2, min = co.attr_min_2, max = co.attr_max_2})
--     end 

--     return temp
-- end

--是否可以交易
function Item:CanTrade()
    local co = Item.GetConfig(self.item_id)
    if co ~= nil then
        if self:IsJewelry() or self:IsEquip() then
            local is_neveal = self:IsNeveal()
            if self.IsBlazeStar and self:IsBlazeStar() then 
                return co.item_trade_money_type ~= 0 and not is_neveal
            else
                return self:IsTreasures() and co.item_trade_money_type ~= 0 and not is_neveal
            end 
        end
        return co.item_trade_money_type ~= 0
    end
    return false
end

function Item:IsTreasures()
    
    if self.param and self.param.is_treasure then
        return self.param.is_treasure == 3
        -- return self.in_bag and self.param.is_treasure == 1
    end
    return false
end

--是否推荐出售
function Item:Recommend()
    local co = Item.GetConfig(self.item_id)
    return co and co.recommend ~= 0 or false
end

--获得出售最低数量
function Item:GroupNum()
    local co = Item.GetConfig(self.item_id)
    return (co and co.group ~= "") and co.group or 1
end

--item_info特殊逻辑配置 不return使用默认配置
function Item:InfoConfig()
    if ItemType.Jewelry == self:Type() then
        if self.param and self.param.lyrics_id == 0 then
            return {"EquipBase", "Desc", "CreateName"}
        end
    end
    if ItemType.Medal == self:Type() then
        return
    end
    if self:Recommend() and self:Price() ~= 0 then
        if ViewMgr:IsOpen(MallView) then
            return {"Desc", "PriceBase"}
        end
    end
    if ShowType.ActItem == self:ShowType() or BagData.Instance.act_item_config[self.item_id] then
        if ConcentricKnotData.Instance:IsSelfItem(self.item_id) then
            return {"Desc"}
		end
        return {"Desc", "LimitTime"}
    end
    if ItemType.Illustration == self:Type() then
        return {"CollectBase","Desc"}
    end
    if ShowType.HeartSkill == self:ShowType() then
        return {"Desc", "HeartSkill"}
    end
    if self.invalid_time and self.invalid_time > 0 then
        return {"Desc", "LimitTime"}
    end
    if AccrssoryData.Instance:CheckIsExtralStuff(self.item_id) then 
        return {"Desc","AccrssoryExtralAttri","AccrssoryExtral"}
    end 
end

function Item:EquipType()
    return Equip.GetEquipType(self.item_id)
end

function Item:ShowType()
    local co = Item.GetConfig(self.item_id)
    return co and co.show_type or nil
end

function Item:ItemInfoEquipBase()
    return self.item_info_equip_base
end
function Item:UltiAttrType()
    local co = Item.GetConfig(self.item_id)
    return co.added_attr_type
end
--强化精工属性上限
function Item:UltiAttr()
    local co = Item.GetConfig(self.item_id)
    return co.min_value, co.max_value
end
--精工一次范围
function Item:UltiAttr1()
    local co = Item.GetConfig(self.item_id)
    return co.min_jg_common, co.max_jg_common
end
--强化精工一次范围
function Item:UltiAttr2()
    local co = Item.GetConfig(self.item_id)
    return co.min_jg_added, co.max_jg_added
end
function Item:LevelView()
    local co = Item.GetConfig(self.item_id)
    if co then
        return co.level_view
    end
end
function Item:LevelShow()
    local co = Item.GetConfig(self.item_id)
    if co then
        return co.level_show
    end
end
function Item:WithUlti()
    return self.param == nil or self.param.ultimate_attr_list == nil
end

-- 是否显示自己的baseInfo
function Item:OneselfBaseIndex()
    if Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.PET_WEAPON then
        return 1
    elseif Item.GetBigType(self.item_id) == ITEM_BIG_TYPE.EQUIP_SPIRIT then
        return 2
    end
    return 0
end

--本地配置
function Item.GetConfig(item_id)
    for _, v in pairs(item_all_config) do
        local co = v[item_id]
        if co ~= nil then
            return co
        end
    end
    if item_id and item_id ~= 0 then
        BuglyReport("item_id = " .. tostring(item_id) .. "配置不存在")
    end
    return nil
end

-- 判断这个道具是不是角色属性丹礼盒，这个道具很特殊，要经过服务器下发获取
function Item.IsPlayerDanGift(item_id)
    local config = Item.GetConfig(item_id)
    if config and config.attr_view == 0 then
        return true
    end
end

--服务端类型BigType
function Item.GetBigType(item_id)
    for k, v in pairs(item_all_config) do
        local co = v[item_id]
        if co ~= nil then
            return k
        end
    end
    return nil
end

--名字
function Item.GetName(item_id)
    local co = Item.GetConfig(item_id)
    if co and ItemType.Fashion == co.item_type then
        return IllusionData.Instance:GetItemName(co.id) or co.name
    end
    return co ~= nil and co.name or "NoNameException "
end

--职业
function Item.GetProfName(item_id)
    local co = Item.GetConfig(item_id)
    if co ~= nil and co.limit_prof and Language.Common.Prof[math.floor(co.limit_prof / 100)] then
        return Language.Common.Prof[math.floor(co.limit_prof / 100)]
    end
    return Language.Common.Prof[-1]
end

--类型
function Item.GetType(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.item_type or 0
end

--物品描述
function Item.GetDesc(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.description or ""
end

--百分百经验丹描述
function Item.GetDescExpPercent(item_id)
    local co = Item.GetConfig(item_id)
    if nil ~= co and co.param1 then
        local percent = co.param1 / 100
        local base_data = RoleData.Instance:GetBaseData()
        local next_exp = RoleData.GetNextLevelExp(base_data.level)
        local cur_exp = base_data.cur_exp
        local rate = next_exp ~= 0 and cur_exp / next_exp or 0
        local value = 0
        if rate <= percent then
            value = next_exp * percent
        else
            value = next_exp - cur_exp + (percent + rate - 1) * RoleData.GetNextLevelExp(base_data.level + 1)
        end
        return string.format(Language.Common.DescExpPercent, math.floor(value))
    else
        return ""
    end
end

--类型描述
function Item.GetTypeDesc(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.item_type
end

--使用等级
function Item.GetLimitLv(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.limit_level or 1
end

function Item.GetShowType(item_id)
    local co = Item.GetConfig(item_id)
    return co and co.show_type or 0
end

function Item.GetPrice(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.sellprice or 0
end

function Item.GetMinPrice(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and (tonumber(co.min_price) and co.min_price or Item.GetPrice(item_id)) or Item.GetPrice(item_id)
end

function Item.GetMaxPrice(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and (tonumber(co.max_price) and co.max_price or Item.GetPrice(item_id)) or Item.GetPrice(item_id)
end

function Item.GetRecommendPrice(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and (tonumber(co.recommended_price) and co.recommended_price or Item.GetPrice(item_id)) or Item.GetPrice(item_id)
end

function Item.GetIconId(item_id)
    local co = Item.GetConfig(item_id)
    local icon_id = 0
    if nil ~= co then
        icon_id = co.icon_id
        if co.show_type and (ShowType.Fashion == co.show_type or ShowType.IllusionChange == co.show_type) and icon_id then
            icon_id = IllusionData.Instance:FashionIconId(co.param1, icon_id)
        end
    end
    return icon_id
end

function Item.GetModKey(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.mod_key
end

function Item.GetOpenPanel(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.open_panel
end

function Item.GetNum(item_id)
    local cfg = Item.GetConfig(item_id)
    if TableIsEmpty(cfg) then
        return 0
    end
    local show_type = cfg.show_type
    local bag_type = cfg.bag_classification
    if show_type == ShowType.PartnerPact then
        local re = PartnerData.Instance:GetPatnerPactHaveNum(item_id)
        return re
    end
    if bag_type == BAG_TYPE.HUANSHOU then
        local re = HuanShouData.Instance:GetBagItemNum(item_id)
        return re
    end
    if show_type == ShowType.YouMingEquip then
        local re = YouMingData.Instance:GetNumByItemId(item_id, cfg)
        return re
    end
    local re = BagData.Instance:GetNumByItemId(item_id, cfg)
    return re
end

function Item.GetBagType(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.bag_type or 0
end

function Item.GetSearchType(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.search_type or 0
end

--根据物品名字找到对应的物品ID
function Item.GetItemFindId(name)
    for _, cfg in pairs(item_all_config) do
        for k, v in pairs(cfg) do
            if string.find(tostring(v.name), tostring(name)) ~= nil then
                return v.id or 0
            end
        end
    end
    return 0
end

--获取一组模糊搜索的道具list
function Item.GetItemFindList(name)
    local list = {}
    for _, cfg in pairs(item_all_config) do
        for k, v in pairs(cfg) do
            if string.find(v.name,name) ~= nil then
                table.insert(list, v)
            end
        end
    end
    return list
end

-- -- 勋章兑换物品
-- function Item.IsMedalChange(item_id)
--     return BagData.Instance:GetIsMedalChange(item_id)
-- end

function Item.GetIsCanUse(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.is_can_use or 0
end

--shallow true代表浅色背景
function Item.GetColorStr(item_id,shallow)
    local co = Item.GetConfig(item_id)
    if co == nil then
        return COLORSTR.White
    end
    return shallow and QualityColorStrShallow[co.color] or QualityColorStr[co.color]
end

function Item.GetQuaName(item_id,shallow)
    local co = Item.GetConfig(item_id)
    if shallow == true then
        return co and ColorStr(co.name,QualityColorStrShallow[co.color]) or ""
    else
        return co and ColorStr(co.name,QualityColorStr[co.color]) or ""
    end
end

function Item.GetUseType(item_id)
    local co = Item.GetConfig(item_id)
    return co and co.use_type or 0
end

function Item:BtnList()
    return Item.GetBtnList(self.item_id)
end

function Item.GetBtnList(item_id)
    local co = Item.GetConfig(item_id)
    return (co and co.btn_list) and string.split(co.btn_list, "|") or {}
end

function Item.EquipTypeName(item_id)
    local co = Item.GetConfig(item_id)
    return co and Language.Common.EquipType[co.equip_type2] or ""
end

function Item.GetIsShowInBoxDrop(item_id)
    local config = Cfg.ItemModelGroup(item_id,false)
    if config ~= nil then
        return true, config
    else
        return false
    end
end

function Item.GetCondition(item_id)
    local co = Item.GetConfig(item_id)
    return {use_type = co.use_type or 0,
        param1 = co.param1 or 0,
        param2 = co.param1 or 0,
        param3 = co.param3 or 0
    }
end

function Item.GetColor(item_id)
    local co = Item.GetConfig(item_id)
    return co and co.color or ItemColor.White
end

--是否快捷使用
function Item.IsQuick(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.quick == 1 or false 
end

function Item.GetWayList(item_id)
    local config = Item.GetConfig(item_id)
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

function Item.GetIsVirtualItem(item_id)
    return nil ~= Config.virtual_auto[item_id]
end

--==============Equip=============================
Equip = Equip or BaseClass(Item)
function Equip:__init(vo)
    vo = vo or RO_EMTPY_TAB
    self.index = vo.index or 0
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 0
    self.param = vo.param
    self.is_grey = vo.is_grey
    self.in_bag = true

    self:CacheInfo()
end

function Equip:CacheInfo()
    if self.param == nil then return end 

    self.with_ult_attr = false
    if self.param.ultimate_attr_list ~= nil then 
        for i = 1,EQUIP_ITEM_ATTR_NUM do 
            if self.param.attr_list[i].attr_value ~= 0 and self.param.ultimate_attr_list[i] ~= 0 then 
                self.with_ult_attr = true
            end  
        end
    end 

    local attr_list = (self:WithUlti() or (not self.with_ult_attr)) and self:BaseAttr() or self:UltimateAttrList()
    self.attr_type_list = {}
    for k,v in pairs(attr_list) do 
        if v.attr_value ~= 0 then 
            self.attr_type_list[v.attr_type] = v
        end 
    end
end

function Equip:NameId()
    if self.param and self.param.is_strong_create == 1 then 
        return self:AddedName()
    elseif self.param and self.param.cur_effect and self.param.cur_effect.effect_id > 0 then 
        return self:NevealName()
    else 
        return Item.GetName(self.item_id)
    end 
end

function Equip:QuaName(shallow)
    return ColorStr(self:NameId(),self:ColorStr(shallow))
end

function Equip:EquipLevel()
    return Equip.GetEquipLevel(self.item_id)
end

function Equip:EquipType()
    return Equip.GetEquipType(self.item_id)
end

function Equip:EquipTypeSort()
    return Equip.GetEquipTypeSort(self.item_id)
end

function Equip:LimitProf()
    return Equip.GetLimitProf(self.item_id)
end

function Equip:TypeName()
    if self:EquipType() == 2 then
        return Item.EquipTypeName(self.item_id)
        -- return Item.GetConfig(self.item_id).desc
    end
    return Language.Common.Equip[self:EquipType()]
end

function Equip:Stone()
    return self.param and self.param.stone
end

function Equip:StrengthenLevel()
    return self.param and self.param.strengthen_level
end

function Equip:HadStone()
    if not self.param or not self.param.stone then
        return false
    end
    for i, item_id in pairs(self.param.stone) do
        if item_id > 0 then
            return true
        end
    end
    return false
end

-- 获取对比对象
function Equip:Compare()
    if self.in_bag then
        return BagData.Instance:GetEquipList(self:EquipType())
    end
    return nil
end

-- 获取对比对象
function Equip:AnotherCompare()
    if self.in_bag then
        return AnotherData.Instance:GetEquipList(self:EquipType())
    end
    return nil
end

function Equip:EffectDesc(sp_id)
    if not sp_id and self.param then
        sp_id = self.param.sp_id
    end
    return Equip.GetEffectDesc(sp_id)
end

-- 专精
function Equip:IncAttr()
    if self.param == nil then
        return false, nil
    end
    --获取到之后进行范围取品质
    local flag_with = false
    for i, v in ipairs(self.param.attr_extra_inc or {}) do
        if v.attr_value > 0 then
            WorkshopData.Instance:SmeltAttrQua(self:EquipLevel(self.item_id), v)
            flag_with = true
        end
    end
    return flag_with, self.param.attr_extra_inc
end
function Equip:RefreshIncAttr()
    if self.param == nil then
        return false, nil
    end 
    --获取到之后进行范围取品质
    local flag_with = false
    for i, v in ipairs(self.param.refresh_attr_extra_inc or {}) do
        if v.attr_value > 0 then
            WorkshopData.Instance:SmeltAttrQua(self:EquipLevel(self.item_id), v)
            flag_with = true
        end
    end
    return flag_with, self.param.refresh_attr_extra_inc
end
-- 基础属性区间
function Equip:AttrSection()
    local co = Item.GetConfig(self.item_id)
    local temp = {}
    for key, value in pairs(AttrTypeForName) do
        if co["max_" .. key] ~= nil and co["min_" .. key] ~= nil and not ( co["max_" .. key] == 0 and co["min_" .. key] == 0 ) then
            local vo = {attr_type = value, min = co["min_" .. key], max = co["max_" .. key]}
            for i = 0 ,5 do 
                if co["added_attr_type"..i] == value and not (co["min_value"..i] == 0 and co["max_value"..i] == 0 ) then 
                    vo.with_stren = true
                    vo.stren_min = co["min_value"..i]
                    vo.stren_max = co["max_value"..i]
                    break
                end  
            end 
            table.insert(temp, vo)
        end
    end
    table.sort(temp, DataSorter.KeyLowerSorter("attr_type"))
    return temp
end

function Equip:UnStone()
	for index, value in ipairs(self:Stone()) do
		if value > 0 then
			--宝石所占孔 0或者1
			WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_GEM_DISMOUNT,(self:IsEquip() and 0 or 1),1, self.index, index - 1)
		end
	end
end
function Equip:HasScore()
    local flag = BagData.Instance:CheckWithScore(self.item_id)
    return flag
end

function Equip:CanResolve()
    if self.in_bag and (self:Color() <= 5)  then --and not self:HadStone() --and not self:HasSpirit() and (self:Color() == 4 or self:Color() == 5) 
        local cfg = WorkshopData.Instance:GetForgeComposeCfg(self.item_id)
        if cfg ~= nil then
            return true, cfg
        end
    end
    return false
end

function Equip:CanResolve2()
    if (self:Color() <= 5) then --and not self:HadStone() --and not self:HasSpirit()
        local cfg = WorkshopData.Instance:GetForgeComposeCfg(self.item_id)
        if cfg ~= nil then
            return true, cfg
        end
    end
    return false
end

function Equip:AddedName()
    local co = Item.GetConfig(self.item_id)
    return co.added_name
end

function Equip:NevealName()
    local co = Item.GetConfig(self.item_id)
    local co_e = WorkshopData.Instance:GetNevealCfg(self.param.cur_effect.effect_id,self.param.cur_effect.effect_level)
    return co_e ~= nil and "【"..co_e.effect_name.."】"..Item.GetName(self.item_id) or Item.GetName(self.item_id)
end

function Equip:QuaAddedName(shallow)
    return ColorStr(self:AddedName(),self:ColorStr(shallow))
end

function Equip:QuaNevealName(shallow)
    return ColorStr(self:NevealName(),self:ColorStr(shallow))
end
--是否可转换
function Equip:IsCanConversion()
    if Item.GetColor(self.item_id) == Config.compose_auto.equip_tran[1].tran_color then
        local conversion_cfg = Cfg.EquipConversionSearch(self.item_id, IS_EDITOR)
        return conversion_cfg ~= nil
    else
        return false
    end
end

-- 从配置中取单极值作为值
function Equip.GetSingleAttrSection(item_id,is_max)
    local co = Item.GetConfig(item_id)
    local temp = {}
    for key, value in pairs(AttrTypeForName) do
        if not ( co["max_" .. key] == 0 and co["min_" .. key] == 0 ) then
            local vo = {
                attr_type = value,
                attr_value = is_max and co["max_" .. key] or co["min_" .. key]
            } 
            for i = 0 ,5 do 
                if co["added_attr_type"..i] == value and not (co["min_value"..i] == 0 and co["max_value"..i] == 0 ) then 
                    vo.with_stren = true
                    vo.stren_min = co["min_value"..i]
                    vo.stren_max = co["max_value"..i]
                    break
                end  
            end 
            table.insert(temp, vo)
        end
    end
    table.sort(temp, DataSorter.KeyLowerSorter("attr_type"))
    for i = 1,#temp do temp[i].index = i end 
    return temp
end

function Equip.GetEquipLevel(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.equip_level or 1
end

function Equip.GetEquipType(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.equip_type or -1
end
function Equip.GetEquipTypeSort(item_id)
    local co = Item.GetConfig(item_id)
    local type = co ~= nil and co.equip_type or -1
    return EquipmentSort[type]
end 

function Equip.GetLimitProf(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.limit_prof or -1
end

function Equip.GetEffectDesc(sp_id)
    return BagData.Instance:GetEquipEffectDesc(sp_id, true)
end

function Equip:GetRankScore()
    return self.param and self.param.is_treasure 
end

function Equip:GetEquipSet()
    return self.param and self.param.new_suit_effect_id 
end

function Equip:UltimateAttrList()
    local attr_list = {}
    if self.param and self.param.attr_list and next(self.param.attr_list) ~= nil then
        for i = 1,EQUIP_ITEM_ATTR_NUM do 
            local vo = {
                attr_type = self.param.attr_list[i].attr_type,
                attr_value = self.param.attr_list[i].attr_value,
                index = i,
                ultimate_value = self.param.ultimate_attr_list[i]}
            table.insert(attr_list,vo)
        end
    end
    return attr_list
end

-- 鉴定该装备是否为老装备（老装备不可精工）
function Equip:IsElder()
    local list = self:UltimateAttrList()
    local sec_list = self:AttrSection()
    for k,v in pairs(list) do
        local flag = v.attr_type > 0 or v.attr_value >0
        for i,j in pairs(sec_list) do
            if j.attr_type == v.attr_type then 
                flag = false
            end 
        end 
        if flag then 
            return flag
        end 
    end 

    return false
end

function Equip:CheckWithUltimateAttr()
    return self.with_ult_attr
end 

function Equip:TypedAttrList()
    return self.attr_type_list
end

function Equip.IsDivine(item_id)
    return Item.GetColor(item_id) > 6
end

-- 获取铸灵列表
function Equip:GetSpiritList()
    return self.param.equip_spirit_info or {}
end

-- 判断是否有铸灵
function Equip:HasSpirit()
    return not TableIsEmpty(self:GetHasSpiritList())
end

-- 获取有效的铸灵列表（是否按品质从低到高）
function Equip:GetHasSpiritList(sort)
    local spirit_list = {}
    for k, v in pairs(self:GetSpiritList()) do
        if v.item_id > 0 then
            table.insert(spirit_list, v)
        end
    end
    if sort then
        table.sort(spirit_list, function(a, b)
            local a_config = Item.GetConfig(a.item_id)
            local b_config = Item.GetConfig(b.item_id)
            return a_config.color < b_config.color
        end)
    end
    return spirit_list
end

-- 获取铸灵数量
function Equip:GetSpiritNum()
    local spirit_list = self:GetHasSpiritList()
    if TableIsEmpty(spirit_list) then
        return 0
    end
    return table.nums(spirit_list)
end

-- 获取铸灵列表中最低的等级，没有铸灵就返回nil
function Equip:GetSpiritMinLevel()
    local spirit_list = self:GetHasSpiritList()
    if TableIsEmpty(spirit_list) then
        return
    end
    -- 获取铸灵组合等级
    local level = spirit_list[1].level
    for i, w in pairs(spirit_list) do
        if level > w.level then
            level = w.level
        end
    end
    return level
end

-- 获取共鸣效果表，如果没有共鸣效果返回nil
function Equip:GetSpiritResonance()
    local spirit_list = self:GetHasSpiritList(true)
    if not TableIsEmpty(spirit_list) then
        local color_list = {}
        for k, v in pairs(spirit_list) do
            local config = Item.GetConfig(v.item_id)
            if TableIsEmpty(config) then
                return
            end
            table.insert(color_list, {color = config.color, slot = v.slot})
        end
        local combine_config, level = EquipData.Instance:GetSpiritCombineDataByColorList(color_list, spirit_list)
        if not TableIsEmpty(combine_config) then
            local tab = {}
            tab.combine_config = combine_config
            tab.combine_attr = EquipData.Instance:GetSpiritCombineAttrData(self:EquipType(), tab.combine_config.resonance_group_id, level)
            if TableIsEmpty(tab.combine_attr) then
                LogError("铸灵共鸣组合加成配置出错", RoleData.Instance:GetProfession(), self:EquipType(), tab.combine_config.resonance_group_id, level)
                return
            end
            return tab
        end
    end
end

-- 一键卸下所有铸灵
function Equip:RemoveAllSpirit()
    local spirit_list = self:GetHasSpiritList()
    if TableIsEmpty(spirit_list) then
        return
    end
    local parma_t = {}
    parma_t.req_type = SpiritReqType.Remove
    if self.in_bag then
        parma_t.param1 = SpiritSrcType.Bag
    elseif self:IsEquip() then
        parma_t.param1 = SpiritSrcType.Equip
    elseif self:IsJewelry() then
        parma_t.param1 = SpiritSrcType.Jewelry
    end
    for k, v in pairs(spirit_list) do
        parma_t.param2 = self.index
        parma_t.param3 = self:GridColumn()
        parma_t.param4 = v.slot
        EquipCtrl.Instance:SendSpiritReq(parma_t)
    end
end

-- 警告，目前只有装备有
-- 获取当前点化信息
function Equip:Neveal()
    if self.param then
        return self.param.cur_effect
    end 

    return {effect_id = 0,effect_level = 0}
end

function Equip:IsNeveal()
    if self:IsJewelry() then return false end 
    if self.param and self.param and self.param.cur_effect then 
        return self.param.cur_effect.effect_id > 0 and self.param.cur_effect.effect_level > 0
    end 
    return false
end

-- 获取熔铸信息
-- 能不能熔铸
-- 警告，本方法只能用于非具体装备的判断，不能应用在具体装备
function Equip.CanBlazeStar(item_id)
    -- 注意 这个参疑似被废弃 暂用颜色
    return WorkshopData.Instance:GetBlazeCfg(item_id,1) ~= nil -- Item.GetConfig(item_id).is_star ~= 0
end

-- 是不是熔铸系列
function Equip:IsBlazeStar()
    -- 展示用注入
    if self.param == nil and self.show_star_level ~= nil then 
        return true
    end 
    if self.param == nil then 
        return false
    end 

    if self.param.star_level == nil then 
        return false
    end 	
	
    return self.param.star_level > 0
end

-- 熔铸信息
function Equip:BlazeStar()
    -- 展示用注入
    if self.param == nil and self.show_star_level ~= nil then 
        return self.show_star_level
    end 
    if self.param == nil then return 0 end 

    return self.param.star_level 
end

function Equip:MinPrice()
    if not self:IsBlazeStar() then 
        return Item.GetMinPrice(self.item_id)
    end 

    local blaze_cfg = WorkshopData.Instance:GetBlazeCfg(self.item_id,self:BlazeStar())
    return blaze_cfg ~= nil and blaze_cfg.min_price_times or Item.GetMinPrice(self.item_id)
end 

function Equip:MaxPrice()
    if not self:IsBlazeStar() then 
        return Item.GetMaxPrice(self.item_id)
    end 
    
    local blaze_cfg = WorkshopData.Instance:GetBlazeCfg(self.item_id,self:BlazeStar())
    return blaze_cfg ~= nil and blaze_cfg.max_pricee_times or Item.GetMaxPrice(self.item_id)
end 

-- 获取刷新点化信息
function Equip:RefreshNeveal()
    if self.param then
        return self.param.refresh_effect
    end
    return {effect_id = 0,effect_level = 0}
end

function Equip:IsRefreshNeveal()
    if self.param then 
        return self.param.refresh_effect.effect_id > 0 and self.param.refresh_effect.effect_level > 0
    end 
    return false
end
function Equip:CurNevealScheme()
    if self.param then
        return self.param.cur_dianhua_plan or 0
    end
    return 0
end
function Equip:SchemeUnlock(index)
    if self.param then
        return self.param.unlock_plan[index] == 1
    end
    return false
end

--==============EleCrystal=============================
EleCrystal = EleCrystal or BaseClass(Item)
function EleCrystal:__init(vo)
    vo = vo or RO_EMTPY_TAB
    self.vo = vo
    self.index = vo.index or 0
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or true
    self.num = vo.num or 0
    self.param = vo.param-- or {}
    self.is_grey = vo.is_grey
end

function EleCrystal:Element()
    return EleCrystal.GetElement(self.item_id)
end

--可使用次数
function EleCrystal:UsedTime()
    if self.param ~= nil then
        return self.param.used_times or 8000
    end
    return 8000
end

function EleCrystal:QuaName(shallow)
    local co = Item.GetConfig(self.item_id)
    if co == nil then
        LogError(tostring(self.item_id) .. " 不存在")
        return ""
    end
    if self.param and self.param.used_times and self.param.used_times == 0 then 
        return ColorStr(Format(Language.Common.ZeroCount, co.name),COLORSTR.Gray2)
    else
        return ColorStr(co.name,self:ColorStr(shallow))
    end 
end

--[[ function EleCrystal:CanUse()
	local limit = EleCrystal.GetLimitUse(self.item_id)
	return limit - self:UsedTime()
end ]]
function EleCrystal.GetElement(item_id)
    local res = {}
    local co = Item.GetConfig(item_id)
    if not co then
        return
    end
    for k, v in pairs(ElementType) do
        if co[k] > 0 then
            table.insert(res, v, co[k])
        end
    end
    return res
end

function EleCrystal.GetLimitUse(item_id)
    local co = Item.GetConfig(item_id)
    return co.num_use
end
--灵饰
--==============Jewelry=============================
Jewelry = Jewelry or BaseClass(Equip)
function Jewelry:__init(vo)
    vo = vo or RO_EMTPY_TAB
    self.index = vo.index or 0
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind
    self.num = vo.num or 0
    self.jewelry_type = vo.jewelry_type or 0
    self.param = vo.param
    self.in_bag = true
    self.is_grey = vo.is_grey

    self:CacheInfo()
end

function Jewelry:CacheInfo()
    if self.param == nil then return end 

    self.with_ult_attr = false
    if self.param.ultimate_attr_list ~= nil then 
        for i = 1,EQUIP_ITEM_ATTR_NUM do 
            if self.param.attr_list[i].attr_value ~= 0 and self.param.ultimate_attr_list[i] ~= 0 then 
                self.with_ult_attr = true
            end  
        end
    end 

    local attr_list = (self:WithUlti() or (not self.with_ult_attr)) and self:BaseAttr() or self:UltimateAttrList()
    self.attr_type_list = {}
    for k,v in pairs(attr_list) do 
        if v.attr_value ~= 0 then 
            self.attr_type_list[v.attr_type] = v
        end 
    end
end

function Jewelry:JewelryType()
    return Jewelry.GetJewelryType(self.item_id)
end

function Jewelry:TypeName(jewelry_type)
    if jewelry_type then
        return Language.Common.Jewelry[jewelry_type]
    end
    return Language.Common.Jewelry[self:JewelryType()]
end

function Jewelry:AttrSection()
    local co = Item.GetConfig(self.item_id)
    local temp = {}
    if co.attr_min > 0 or co.attr_max > 0 then 
        local vo = {attr_type = co.attr_type, min = co.attr_min, max = co.attr_max}
        for i = 0 ,5 do 
            if co["added_attr_type"..i] == co.attr_type and not (co["min_value"..i] == 0 and co["max_value"..i] == 0 ) then 
                vo.with_stren = true
                vo.stren_min = co["min_value"..i]
                vo.stren_max = co["max_value"..i]
                break
            end  
        end 
        table.insert(temp, vo)
    end 

    if  co.attr_min_2 > 0 or co.attr_max_2 > 0 then 
        local vo = {attr_type = co.attr_type_2, min = co.attr_min_2, max = co.attr_max_2}
        for i = 0 ,5 do 
            if co["added_attr_type"..i] == co.attr_type_2 and not (co["min_value"..i] == 0 and co["max_value"..i] == 0 ) then 
                vo.with_stren = true
                vo.stren_min = co["min_value"..i]
                vo.stren_max = co["max_value"..i]
                break
            end  
        end 
        table.insert(temp, vo)
    end 

    return temp
end

-- 鉴定该装备是否为老装备（老装备不可精工）
function Jewelry:IsElder()
    local list = self:UltimateAttrList()
    local sec_list = self:AttrSection()
    for k,v in pairs(list) do
        for i,j in pairs(sec_list) do
            if j.attr_type == v.attr_type and j.max == nil then 
                return true
            end 
        end 
    end 

    return false
end
--是否可转换
function Jewelry:IsCanConversion()
    if Item.GetColor(self.item_id) == Config.compose_auto.equip_tran[1].tran_color then
        local conversion_cfg = Cfg.EquipConversionSearch(self.item_id, IS_EDITOR)
        return conversion_cfg ~= nil
    else
        return false
    end
end

function Jewelry:EquipLevel()
    return Equip.GetEquipLevel(self.item_id)
end
function Jewelry:IncAttr()
    if self.param == nil then
        return false, nil
    end
    --获取到之后进行范围取品质
    local flag_with = false
    for i, v in ipairs(self.param.attr_extra_inc or {}) do
        if v.attr_value > 0 then
            WorkshopData.Instance:SmeltAttrQuaJ(self:EquipLevel(self.item_id), v)
            flag_with = true
        end
    end
    return flag_with, self.param.attr_extra_inc
end

function Jewelry:RefreshIncAttr( )
    if self.param == nil then
        return false, nil
    end
    --获取到之后进行范围取品质
    local flag_with = false
    for i, v in ipairs(self.param.refresh_attr_extra_inc or {}) do
        if v.attr_value > 0 then
            WorkshopData.Instance:SmeltAttrQuaJ(self:EquipLevel(self.item_id), v)
            local flag_with = true
        end
    end
    return flag_with, self.param.refresh_attr_extra_inc
end

function Jewelry:Stone()
    return self.param and self.param.stone
end

function Jewelry:StrengthenLevel()
    return self.param and self.param.strengthen_level
end

-- 获取对比对象
function Jewelry:Compare()
    if self.in_bag then
        return BagData.Instance:GetJewelryList(self:JewelryType())
    end
    return nil
end

function Jewelry:AnotherCompare()
    if self.in_bag then
        return AnotherData.Instance:GetJewelryList(self:JewelryType())
    end
    return nil
end 
--test
function Jewelry:NameId()
    if self.param and self.param.is_strong_create == 1 then 
        return self:AddedName()
    else 
        return Item.GetName(self.item_id)
    end 
end

function Jewelry:QuaName(shallow)
    return ColorStr(self:NameId(),self:ColorStr(shallow))
end

function Jewelry:LimitProf()
    return Jewelry.GetLimitProf(self.item_id)
end

function Jewelry:SuitInfo()
    return Jewelry.GetSuitInfo(self.param.lyrics_id)
end

function Jewelry:ActiveSuitInfo()
    local is_active, result = Jewelry.GetActiveSuitInfo()
    if self.in_bag then
        return false, Jewelry.GetSuitInfo(self.param.lyrics_id)
    end
    return is_active, is_active and result or Jewelry.GetSuitInfo(self.param.lyrics_id)
end
function Jewelry:ActiveLegendInfo()
    local suit_info = self:SuitInfo()
    if suit_info and suit_info.legend_attr ~= 0 then
        local _, max = self:AttrSection()
        return self.param.attr_pair.attr_value == max
    else
        return false
    end
end

function Jewelry:IsJewelry()
    return true
end

function Jewelry:EquipTypeSort()
    return Jewelry.GetJewelryTypeSort(self.item_id)
end

function Jewelry:AddedName()
    local co = Item.GetConfig(self.item_id)
    return co.added_name
end

function Jewelry:QuaAddedName(shallow)
    self:CheckRankFlush()
    return ColorStr(self:AddedName(),self:ColorStr(shallow))
end

-- 从配置中取单极值作为值
function Jewelry.GetSingleAttrSection(item_id,is_max)
    local co = Item.GetConfig(item_id)
    local temp = {}
    if co.attr_min > 0 or co.attr_max > 0 then 
        local vo = {
            attr_type = co.attr_type,
            attr_value = is_max and co.attr_max or co.attr_min, 
        }
        for i = 0 ,5 do 
            if co["added_attr_type"..i] == co.attr_type and not (co["min_value"..i] == 0 and co["max_value"..i] == 0 ) then 
                vo.with_stren = true
                vo.stren_min = co["min_value"..i]
                vo.stren_max = co["max_value"..i]
                break
            end  
        end 
        table.insert(temp, vo)
    end 

    if  co.attr_min_2 > 0 or co.attr_max_2 > 0 then 
        local vo = {
            attr_type = co.attr_type_2,
            attr_value = is_max and co.attr_max_2 or co.attr_min_2, 
        }
        for i = 0 ,5 do 
            if co["added_attr_type"..i] == co.attr_type_2 and not (co["min_value"..i] == 0 and co["max_value"..i] == 0 ) then 
                vo.with_stren = true
                vo.stren_min = co["min_value"..i]
                vo.stren_max = co["max_value"..i]
                break
            end  
        end 
        table.insert(temp, vo)
    end 

    for i = 1,#temp do temp[i].index = i end 
    return temp
end


function Jewelry.GetJewelryType(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.ornament_type or 0
end

function Jewelry.GetJewelryAttrSection(item_id)
    local co = Item.GetConfig(item_id)
    if co then
        return co.attr_min, co.attr_max
    end
    return 0, 0
end

function Jewelry.GetSuitInfo(suit_id)
    return WorkshopData.Instance:GetJewelrySuitInfo(suit_id)
end

function Jewelry.GetActiveSuitInfo()
    return WorkshopData.Instance:ActiveSuitInfo()
end

function Jewelry.GetJewelryTypeSort(item_id)
    local co = Item.GetConfig(item_id)
    local type = co ~= nil and co.ornament_type or -1
    return JewerlySort[type]
end

function Jewelry.GetLimitProf(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.limit_prof or -1
end

function Jewelry:EquipType()
    return Jewelry.GetJewelryType(self.item_id) + 5
end

function Jewelry:GetRankScore()
    return self.param and self.param.is_treasure 
end

function Jewelry:GetEquipSet()
    return self.param and self.param.new_suit_effect_id 
end

function Jewelry:HasScore()
    local flag = BagData.Instance:CheckWithScore(self.item_id)
    return flag
end

function Jewelry:CanResolve()
    if self.in_bag and (self:Color() <= 5)  then --and not self:HadStone() --and not self:HasSpirit() and (self:Color() == 4 or self:Color() == 5)
        local cfg = WorkshopData.Instance:GetForgeComposeCfg(self.item_id)
        if cfg~=nil then
            return true, cfg
        end
    end
    return false
end

function Jewelry:CanResolve2()
    if (self:Color() <= 5) then -- and not self:HadStone() --and not self:HasSpirit()
        local cfg = WorkshopData.Instance:GetForgeComposeCfg(self.item_id)
        if cfg~=nil then
            return true, cfg
        end
    end
    return false
end

function Jewelry:UltimateAttrList()
    local attr_list = {}
    if self.param and self.param.attr_list and next(self.param.attr_list) ~= nil then
        for i = 1,EQUIP_ITEM_ATTR_NUM do 
            local vo = {
                attr_type = self.param.attr_list[i].attr_type,
                attr_value = self.param.attr_list[i].attr_value,
                index = i,
                ultimate_value = self.param.ultimate_attr_list[i]}
            table.insert(attr_list,vo)
        end
    end
    return attr_list
end

function Jewelry:CheckWithUltimateAttr()
    return self.with_ult_attr
end 

function Jewelry:TypedAttrList()
    return self.attr_type_list
end

-- 获取铸灵列表
function Jewelry:GetSpiritList()
    return self.param.equip_spirit_info or {}
end

-- 判断是否有铸灵
function Jewelry:HasSpirit()
    return not TableIsEmpty(self:GetHasSpiritList())
end

-- 获取有效的铸灵列表（是否按品质从低到高）
function Jewelry:GetHasSpiritList(sort)
    local spirit_list = {}
    for k, v in pairs(self:GetSpiritList()) do
        if v.item_id > 0 then
            table.insert(spirit_list, v)
        end
    end
    if sort then
        table.sort(spirit_list, function(a, b)
            local a_config = Item.GetConfig(a.item_id)
            local b_config = Item.GetConfig(b.item_id)
            return a_config.color < b_config.color
        end)
    end
    return spirit_list
end

-- 获取铸灵数量
function Jewelry:GetSpiritNum()
    local spirit_list = self:GetHasSpiritList()
    if TableIsEmpty(spirit_list) then
        return 0
    end
    return table.nums(spirit_list)
end

-- 获取铸灵列表中最低的等级，没有铸灵就返回nil
function Jewelry:GetSpiritMinLevel()
    local spirit_list = self:GetHasSpiritList()
    if TableIsEmpty(spirit_list) then
        return
    end
    -- 获取铸灵组合等级
    local level = spirit_list[1].level
    for i, w in pairs(spirit_list) do
        if level > w.level then
            level = w.level
        end
    end
    return level
end

-- 获取共鸣效果表，如果没有共鸣效果返回nil
function Jewelry:GetSpiritResonance()
    local spirit_list = self:GetHasSpiritList(true)
    if not TableIsEmpty(spirit_list) then
        local color_list = {}
        for k, v in pairs(spirit_list) do
            local config = Item.GetConfig(v.item_id)
            if TableIsEmpty(config) then
                return
            end
            table.insert(color_list, {color = config.color, slot = v.slot})
        end
        local combine_config, level = EquipData.Instance:GetSpiritCombineDataByColorList(color_list, spirit_list)
        if not TableIsEmpty(combine_config) then
            local tab = {}
            tab.combine_config = combine_config
            tab.combine_attr = EquipData.Instance:GetSpiritCombineAttrData(self:EquipType(), tab.combine_config.resonance_group_id, level)
            if TableIsEmpty(tab.combine_attr) then
                LogError("铸灵共鸣组合加成配置出错", RoleData.Instance:GetProfession(), self:EquipType(), tab.combine_config.resonance_group_id, level)
                return
            end
            return tab
        end
    end
end

-- 一键卸下所有铸灵
function Jewelry:RemoveAllSpirit()
    local spirit_list = self:GetHasSpiritList()
    if TableIsEmpty(spirit_list) then
        return
    end
    local parma_t = {}
    parma_t.req_type = SpiritReqType.Remove
    if self.in_bag then
        parma_t.param1 = SpiritSrcType.Bag
    elseif self:IsEquip() then
        parma_t.param1 = SpiritSrcType.Equip
    elseif self:IsJewelry() then
        parma_t.param1 = SpiritSrcType.Jewelry
    end
    for k, v in pairs(spirit_list) do
        parma_t.param2 = self.index
        parma_t.param3 = self:GridColumn()
        parma_t.param4 = v.slot
        EquipCtrl.Instance:SendSpiritReq(parma_t)
    end
end

-- 获取熔铸信息
-- 是不是熔铸系列
function Jewelry:IsBlazeStar()    
    -- 展示用注入
    if self.param == nil and self.show_star_level ~= nil then 
        return true
    end 
    
    if self.param == nil then 
        return false
    end 

    if self.param.star_level == nil then
        return false
    end 

    return   self.param.star_level > 0
end

-- 熔铸信息
function Jewelry:BlazeStar()    
    -- 展示用注入
    if self.param == nil and self.show_star_level ~= nil then 
        return self.show_star_level
    end 
    
    if self.param == nil then return 0 end 

    return self.param.star_level 
end

function Jewelry:MinPrice()
    if not self:IsBlazeStar() then 
        return Item.GetMinPrice(self.item_id)
    end 

    local blaze_cfg = WorkshopData.Instance:GetBlazeCfg(self.item_id,self:BlazeStar())
    return blaze_cfg ~= nil and blaze_cfg.min_price_times or Item.GetMinPrice(self.item_id)
end 

function Jewelry:MaxPrice()
    if not self:IsBlazeStar() then 
        return Item.GetMaxPrice(self.item_id)
    end 
    
    local blaze_cfg = WorkshopData.Instance:GetBlazeCfg(self.item_id,self:BlazeStar())
    return blaze_cfg ~= nil and blaze_cfg.max_pricee_times or Item.GetMaxPrice(self.item_id)
end 

--====================PartnerEquipment====================
PartnerEquipment = PartnerEquipment or BaseClass(Item)

function PartnerEquipment:__init(vo)
    self.pe_detail, self.pe_suit = PartnerData.Instance:GetPEConfigByItemId(self.item_id)
    self:PartnerEquipment(vo and vo.param)
    self.pe_actived = false
    self.limit_level = Item.GetLimitLv(self.item_id)
end

function PartnerEquipment:PartnerEquipment(param)
    self.attr_list = RO_EMTPY_TAB
    self.attr_num = 0
    if 0 == self.item_id then return end
    if nil == self.vo.param then
        local card = PartnerData.Instance:GetPartnerLifeHandbookCardInfo(self.pe_suit, self:EquipmentType())
        self.param = {attr_list = card.attr_list}
        self.attr_list = card.attr_list or RO_EMTPY_TAB
        self.attr_num = 0
        for i = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
            local attr = self.attr_list[i] or RO_EMTPY_TAB
            if attr and 0 ~= attr.attr_value then
                self.attr_num = self.attr_num + 1
            end
        end
    else
        self.attr_list = param and param.attr_list or RO_EMTPY_TAB
        self.attr_num = 0
        for i = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
            local attr = self.attr_list[i] or RO_EMTPY_TAB
            if attr and 0 ~= attr.attr_value then
                self.attr_num = self.attr_num + 1
            end
        end
    end
end

function PartnerEquipment:EquipmentType()
    return PartnerEquipment.GetEquipmentType(self.item_id)
end

function PartnerEquipment.GetEquipmentType(item_id)
    local co = Item.GetConfig(item_id)
    return co ~= nil and co.partner_equipment_type or 0
end

-- function PartnerEquipment:ItemInfoEquipBase()
--     return string.format("%s   %s", Language.Common.AttrList[self.attr_type], DataHelper.IsPercent(self.attr_type) and Percent(self.attr_value or 0) or (self.attr_value or 0))
-- end

function PartnerEquipment:ActiveSuitInfo()
    return self.pe_actived, {
        is_pe = true,
        content = self.pe_suit.name .. "\n" .. self.pe_suit.desc
    }
end

function PartnerEquipment:PESuit()
    return "pe_s" .. self.pe_suit.suit_id
end

function PartnerEquipment:PESuitProf()
    return self.pe_suit and PartnerConfig.PESuitId2Prof[self.pe_suit.suit_id] or 0
end

--====================PartnerPact====================
PartnerPact = PartnerPact or BaseClass(Item)

function PartnerPact:__init(vo)
    -- self:PartnerPact(vo and vo.param or {})
end

function PartnerPact:AttrUp()
    if nil == self.vo.attr_up then
        local co = Item.GetConfig(self.item_id)
        self.vo.attr_up = co ~= nil and (co.attr_up / 10) or 0
    end
    return self.vo.attr_up
end

function PartnerPact:AttrUpNext()
    return self:AttrUp() + PartnerConfig.PARTNER_CONTRACT_ATTR_UP_PER
end

function PartnerPact:MaxAttrUp()
    return PartnerConfig.PARTNER_CONTRACT_ATTR_UP_MAX == self:AttrUp()
end

function PartnerPact:StarLevel()
    if nil == self.vo.star_level then
        local co = Item.GetConfig(self.item_id)
        self.vo.star_level = co ~= nil and co.star_level or 1
    end
    return self.vo.star_level
end

function PartnerPact:PromoteExpens()
    if nil == self.vo.promote_expens then
        local co = Item.GetConfig(self.item_id)
        self.vo.promote_expens = co ~= nil and co.promote_expens or 1
    end
    return self.vo.promote_expens
end

function PartnerPact:ExpensNum()
    if nil == self.vo.expens_num then
        local co = Item.GetConfig(self.item_id)
        self.vo.expens_num = co ~= nil and co.expens_num or 1
    end
    return self.vo.expens_num
end

function PartnerPact:PartnerId()
    if nil == self.vo.partner_id then
        local co = Item.GetConfig(self.item_id)
        self.vo.partner_id = co ~= nil and co.partner_id or 1
    end
    return self.vo.partner_id
end

function PartnerPact:PartnerName()
    if nil == self.vo.partner_name then
        local partner_id = self:PartnerId()
        local co = Cfg.PartnerById(partner_id)
        self.vo.partner_name = co ~= nil and co.name or ""
    end
    return self.vo.partner_name
end

function PartnerPact:PartnerProf()
    if nil == self.vo.partner_prof then
        local partner_id = self:PartnerId()
        local co = Cfg.PartnerById(partner_id)
        self.vo.partner_prof = co ~= nil and co.job or 0
    end
    return self.vo.partner_prof
end

function PartnerPact:PartnerIsUnlocked()
    if nil == self.vo.partner_info then
        local partner_id = self:PartnerId()
        local info = PartnerData.Instance:GetPartnerInfoById(partner_id)
        self.vo.partner_info = info
    end
    return self.vo.partner_info and self.vo.partner_info:IsUnlocked()
end


function PartnerPact:ScoreAdd(equip_partner)
    local partner_id = self:PartnerId()
    local contract_attrs = PartnerData.Instance:GetPartnerInfoById(partner_id):GetContractAttrs()
    local scale_score = PartnerData.Instance:GetPartnerInfoById(equip_partner):GetPartnerScaleScore()
    local attr_up = self:AttrUp()
    local value = 0
    for i = 0, PartnerConfig.PARTNER_CONTRACT_ATTR_MAX - 1 do
        local ca = contract_attrs[i]
        if scale_score[ca.attr_type] then
            value = value + math.floor(ca.attr_value * attr_up / 100) * scale_score[ca.attr_type]
        end
    end
    value = math.floor(value * 0.04)
    return value
end

function PartnerPact:CheckLock(partner_id)
    local partner_id = self:PartnerId()
    local value = false
    local partner = PartnerData.Instance:GetPartnerInfoById(partner_id)
    if partner.vo.is_unlocked == PartnerConfig.LockState.lock then
        PublicPopupCtrl.Instance:Center(Language.Partner.Contract.ItemLockTips)
		value = true
	end
    return value
end

--=========================Medal=======================
Medal = Medal or BaseClass(Item)
function Medal:__init(vo)
    vo = vo or RO_EMTPY_TAB
    self.index = vo.index or 0
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or false
    self.num = vo.num or 0
    self.param = vo.param or {}
    self.is_grey = vo.is_grey 
    self.in_bag = vo.in_bag == nil and true or vo.in_bag 

    self:Check()
end

-- 令牌创建期间应当保证持有grade和sp_id
function Medal:Check()
    self.param.grade = self.param.grade ~= nil and self.param.grade or 0
    self.param.sp_id = self.param.sp_id ~= nil and self.param.sp_id or self:Config().sp_id
    -- self:CheckRankFlush()
    -- self.param.attr_list = self:BaseAttr()

    self:CacheInfo()
end

function Medal:CacheInfo()
    
end

function Medal:EffectCfg()
    if self.effect_cfg == nil then
        self.effect_cfg = BagData.Instance:GetEquipEffectCfg(self.param.sp_id)
    end
    return self.effect_cfg
end


function Medal:EffectDesc()
    if self.effect_desc == nil then
        self.effect_desc = Equip.GetEffectDesc(self.param.sp_id)
    end
    return self.effect_desc
end

function Medal:Effect2Desc()
    if self.effect_2_desc == nil then
        self.effect_2_desc =  self:WithSecond() and Equip.GetEffectDesc(self.param.second_sp_id) or ""
    end
    return self.effect_2_desc
end

function Medal:Effect2Cfg()
    if self.effect_2_cfg == nil then
        self.effect_2_cfg = self:WithSecond() and BagData.Instance:GetEquipEffectCfg(self.param.second_sp_id) or nil
    end
    return self.effect_2_cfg
end

function Medal:ColorStr(shallow)
    local shallowcolor = shallow and QualityColorStrShallow or QualityColorStr
    return shallowcolor[self:GetMedalQua()]
end

function Medal:ColorSecondStr(shallow)
    local shallowcolor = shallow and QualityColorStrShallow or QualityColorStr
    return shallowcolor[self:GetMedalSecondQua()]
end

function Medal:Color()
    return self:GetMedalQua()
end

function Medal:TypeDesc()
    return Language.Common.ShowType[23]
end 

function Medal:NameId()
    self:CheckRankFlush()
    local effect_str = self:WithSp() and "("..self:SpEffectCfg().name..self:SecondSpName()..")" or ""

    if DEBUG_MODE then
        return self.rank_cfg.item_name..effect_str..string.format("(%d)",self.item_id)
    else
        return self.rank_cfg.item_name..effect_str
    end
end

function Medal:Name()
    self:CheckRankFlush()
    local effect_str = self:WithSp() and "("..self:SpEffectCfg().name..self:SecondSpName()..")" or ""
    return self.rank_cfg.item_name..effect_str
end

function Medal:SecondSpName()
    if self.param == nil then 
        return ""
    end 
    if self.param.second_sp_id == 0 or self.param.second_sp_id == nil then 
        return ""
    end
    return "&"..self:SpEffectSecondCfg().name
end

--是否浅色背景
--shallow true代表浅色背景
function Medal:QuaName(shallow)
    self:CheckRankFlush()

    local temp_name = ""
    if self:WithSecond() then 
        temp_name = ColorStr(self:SecondSpName(),self:ColorSecondStr(shallow))
    end 
    
    local sp_effect = self:SpEffectCfg() or {name = ""}
    return ColorStr(self.rank_cfg.item_name..(sp_effect.name ~= "" and "("..sp_effect.name..temp_name..")" or ""),self:ColorStr(shallow))
end

function Medal:GetMedalQua()
    local sprite_str = 2
    if self.param.grade ~= nil then 
        sprite_str = math.floor(self.param.grade / 5) + 2 
    end 

    return sprite_str
end

function Medal:GetMedalSecondQua()
    local e2c = self:Effect2Cfg()
    local sprite_str = 2
    if self.param.second_sp_id > 0 then 
        sprite_str = math.floor(e2c.sp_rank / 5) + 2 
    end 

    return sprite_str
end

function Medal:IconId()
    self:CheckRankFlush()
    return self.rank_cfg.item_icon
end

function Medal:BigIcon()
    self:CheckRankFlush()
    return self.rank_cfg.rank_up_icon
end

function Medal:BaseAttr()
    if self.param.grade == nil then 
        return self.param.attr_list ~= nil and self.param.attr_list or self:ConfigAttr()
    end 
    self:CheckRankFlush()
    return self.rank_attr_list
end

function Medal:Level()
    return self:LimitLv()
end

function Medal:LevelDesc()
    return string.format( Language.Medal.MedalRankInfo,self.param.grade) 
end

function Medal:WithSp()
    if self.param.sp_id > 0 and self:SpEffectCfg() == nil then 
        LogError("Warning sp_id",self.param.sp_id,"is nil sp_id")
    end 
    return self.param.sp_id > 0 and self:SpEffectCfg() ~= nil
end

function Medal:MinPrice()
    self:CheckRankFlush()
    return self.rank_cfg ~= nil and (tonumber(MedalData.Instance:GetMedalMarketCfg(self.rank_cfg.rank).min_price) and MedalData.Instance:GetMedalMarketCfg(self.rank_cfg.rank).min_price or 0) or 0 
end

function Medal:MaxPrice()
    self:CheckRankFlush()
    return self.rank_cfg ~= nil and (tonumber(MedalData.Instance:GetMedalMarketCfg(self.rank_cfg.rank).max_price) and MedalData.Instance:GetMedalMarketCfg(self.rank_cfg.rank).max_price or 0) or 0 
end

function Medal:RecommendPrice()
    self:CheckRankFlush()
    return self.rank_cfg ~= nil and (tonumber(MedalData.Instance:GetMedalMarketCfg(self.rank_cfg.rank).recommended_price) and MedalData.Instance:GetMedalMarketCfg(self.rank_cfg.rank).recommended_price or 0) or 0 
end

function Medal:Price()
    self:CheckRankFlush()
    return self.rank_cfg ~= nil and self.rank_cfg.sellprice or 0 
end

function Medal:IsSpecialSp()
    local cfg = self:SpEffectCfg()
    if cfg == nil then return false end 
    return cfg.is_special == 1
end

function Medal:WithSecond()
    return self.param.second_sp_id and self.param.second_sp_id > 0
end

function Medal:SpEffectCfg()
    if self.param.sp_id == 0 then return nil end 
    return BagData.Instance:GetEquipEffectCfg(self.param.sp_id)
end

function Medal:SpEffectSecondCfg()
    if self.param.second_sp_id == 0 or self.param.second_sp_id == nil then return nil end 
    return BagData.Instance:GetEquipEffectCfg(self.param.second_sp_id)
end

function Medal:CheckRankFlush()
    if self.rank_cfg == nil or self.rank_cfg.rank ~= self.param.grade then 
        self.rank_cfg = MedalData.Instance:GetRankCfg(self.param.grade)
        self.rank_attr_list = {}
        local jump_index = 1 

        -- 若还是找不到配置，给0级配置
        if self.rank_cfg == nil then self.rank_cfg = MedalData.Instance:GetRankCfg(0) end 
        for k,v in pairs(Language.Common.AttrList) do 
            if self.rank_cfg[AttrNameForType[k]] ~=nil and self.rank_cfg[AttrNameForType[k]] > 0  then 
                local vo = {}
                vo.attr_type = k
                vo.attr_value = self.rank_cfg[AttrNameForType[k]]
                vo.index = jump_index 
    
                table.insert( self.rank_attr_list, vo )
                jump_index = jump_index + 1
            end 
        end 
    end 
end

function Medal:TypedAttrList()
    if self.attr_type_list == nil then
        self.attr_type_list = {}
        local attr_list = self:BaseAttr()
        for k,v in pairs(attr_list) do 
            if v.attr_value ~= 0 then 
                self.attr_type_list[v.attr_type] = v
            end 
        end
    end
    return self.attr_type_list
end

function Medal:CanAllUpgrade()
    return not self:IsSpecialSp() and self.param.grade < 24
end

--=========================EquipSpiritBagItem=======================
EquipSpiritBagItem = EquipSpiritBagItem or BaseClass(Item)

function EquipSpiritBagItem:__init(vo)
    vo = vo or RO_EMTPY_TAB
    self.index = vo.index or 0
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or true
    self.num = vo.num or 0
    self.param = vo.param or {}
    self.is_grey = vo.is_grey 
    self.in_bag = vo.in_bag == nil and true or vo.in_bag

    self:Check()
end

-- 确保有参数
function EquipSpiritBagItem:Check()
    if self.param ~= nil then
        self.param.item_id = self.item_id
        -- 等级参数我们自己读表
        local config = Cfg.SpiritStoneConfigById(self.item_id)
        self.param.level = config.spirit_stone_level
    end
end
function EquipSpiritBagItem:Level()
    if self.param == nil then
        return 0
    end
    local config = Cfg.SpiritStoneConfigById(self.item_id)
    if config then
        return config.spirit_stone_level
    end
    return 1
end

function EquipSpiritBagItem:TypeDesc()
    return Language.Common.ShowType[41]
end

function EquipSpiritBagItem:LevelDesc()
    return nil
end

function EquipSpiritBagItem:NameId()
    if DEBUG_MODE then
        return self:Name() .. string.format("(%d)", self.item_id)
    else
        return self:Name()
    end
end

function EquipSpiritBagItem:Name()
    local config = self:Config()
    local m_type = EquipData.Instance:GetSpiritLevelData(self.item_id, 0).spirit_stone_type
    local m_type_str = Language.Equip.StoneType[m_type]
    local show_name = Format(Language.Equip.EquipStoneName, m_type_str, config.name)
    return show_name
end

--是否浅色背景
--shallow true代表浅色背景
function EquipSpiritBagItem:QuaName(shallow)
    return ColorStr(self:Name(), self:ColorStr(shallow))
end

function EquipSpiritBagItem:HasLevelName()
    return Format(Language.Equip.EquipStoneLevel3, self.param.level + 1, self:Name())
end

function EquipSpiritBagItem:HasLevelQuaName(shallow)
    return ColorStr(self:HasLevelName(), self:ColorStr(shallow))
end

--- ===========================NevealStone=========================----
NevealStone = NevealStone or BaseClass(Item)
function NevealStone:__init(vo)
    vo = vo or RO_EMTPY_TAB
    self.index = vo.index or 0
    self.item_id = vo.item_id or 0
    self.is_bind = vo.is_bind or true
    self.num = vo.num or 0
    self.param = vo.param-- or {}
    self.is_grey = vo.is_grey 
    self.in_bag = vo.in_bag == nil and true or vo.in_bag
end

function NevealStone:NameId()
    if DEBUG_MODE then
        return self:Name() .. string.format("(%d)", self.item_id)
    else
        return self:Name()
    end
end

function NevealStone:Name()
    local name = Item.GetName(self.item_id)
    if self.param ~= nil and self.param.has_param then 
        local co_e = WorkshopData.Instance:GetNevealCfg(self.param.effect_id,self.param.effect_level)
        
        name = co_e ~= nil and "【"..co_e.effect_name.."】"..name or name
    end 
    return name
end

function NevealStone:QuaName(shallow)
    return ColorStr(self:Name(), self:ColorStr(shallow))
end

function NevealStone:Neveal()
    if self.param and self.param.has_param then 
        return {effect_id = self.param.effect_id,effect_level = self.param.effect_level}
    end  

    local cfg = Item.GetConfig(self.item_id)
    return {effect_id = cfg.effect_type, effect_level = cfg.effect_level}
end

function NevealStone:IsNeveal()
    if self.param and self.param.has_param then
        return self.param.effect_id >0 and self.param.effect_level >0
    end 
    local cfg = Item.GetConfig(self.item_id)
    return cfg.effect_type > 0 and cfg.effect_level > 0
end

function NevealStone:NevealDesc()
    if not self:IsNeveal() then return self:Desc() end 
    -- 直接取满级，但是目前满级是10级 20211201c
    local effect_id = self.param ~= nil and self.param.effect_id or Item.GetConfig(self.item_id).effect_type
    local co_e = WorkshopData.Instance:GetNevealSkillCfg(effect_id,10)
    local str = self:Desc()
    str = str.."\n"..Language.Workshop.Neveal.ItemDescCatch..ColorStr(co_e.name,QualityColorStr[ItemColor.Red])
    str = str.."\n"..Language.Workshop.Neveal.ItemDescSkillDesc..co_e.set_skill_description
    return str
end

--=================BagData===================================
BagData = BagData or BaseClass()

ItemColumnType = {
    None = -1,
    Start = 0,
    Item = 0, --道具
    Supply = 1, --补给品
    Task = 2, --任务
    Material = 3, --材料
    Illustration = 4, --图鉴
    PetWeapon = 5,  -- 宠物魂器
    Accrssory = 6,  --配饰
    Storage = 7, --仓库
    Temporary = 8, --临时背包
    Count = 9
}

local ItemColumnMaxCount = {
    [ItemColumnType.Item] = 300,
    [ItemColumnType.Supply] = 0,
    [ItemColumnType.Task] = 300,
    [ItemColumnType.Material] = 300,
    [ItemColumnType.Storage] = 300,
    [ItemColumnType.Illustration] = 300,
    [ItemColumnType.PetWeapon] = 300,
    [ItemColumnType.Accrssory] = 300,
}

local BagExpandPriceConvert = {
    bag_item = ItemColumnType.Item,
    bag_material = ItemColumnType.Material,
    bag_Illustrations = ItemColumnType.Illustration,
    warehouse = ItemColumnType.Storage,
    bag_fragment = ItemColumnType.Task,
}

function BagData:__init()
    if BagData.Instance ~= nil then
        Debuger.LogError("[BagData] attempt to create singleton twice!")
        return
    end
    BagData.Instance = self

    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoItemExpense,function()
        item_all_config[ITEM_BIG_TYPE.EXPENSE] = Config.expense_auto
    end)
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoItemGift,function()
        item_all_config[ITEM_BIG_TYPE.GIFT] = Config.gift_auto
    end)    
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoItemOther,function()
        item_all_config[ITEM_BIG_TYPE.OTHER] = Config.other_auto
    end)    


    self.bag_cfg_auto = Config.bag_cfg_auto
    self.bag_unlock_cfg = self.bag_cfg_auto.bag
    self.bag_expand_price = self.bag_cfg_auto.expand_price
    self.special_effect_auto = Config.special_effect_auto
    self.medal_sp_cfg = Config.medal_cfg_auto.sp_item_list
    -- self.medal_change = Config.medal_cfg_auto.medal_change
    --index map,empty grid value is nil
    self.item_list = {
        [ItemColumnType.Item] = SmartData.New(),
        [ItemColumnType.Supply] = SmartData.New(),
        [ItemColumnType.Task] = {},
        [ItemColumnType.Material] = {},
        [ItemColumnType.Storage] = {},
        [ItemColumnType.Illustration] = {},
        [ItemColumnType.Temporary] = {},
        [ItemColumnType.PetWeapon] = {},
        [ItemColumnType.Accrssory] = {},
    }

    --grid index map,empty grid item id is 0
    self.item_grid =
        SmartData.New(
        {
            [ItemColumnType.Item] = SmartData.New(),
            [ItemColumnType.Supply] = SmartData.New(),
            [ItemColumnType.Task] = SmartData.New(),
            [ItemColumnType.Material] = SmartData.New(),
            [ItemColumnType.Storage] = SmartData.New(),
            [ItemColumnType.Illustration] = SmartData.New(),
            [ItemColumnType.Temporary] = SmartData.New(),
            [ItemColumnType.PetWeapon] = SmartData.New(),
            [ItemColumnType.Accrssory] = SmartData.New(),
        }
    )
    -- self.item_grid = {
    -- 	[ItemColumnType.Item] = {},
    -- 	[ItemColumnType.Supply] = {},
    -- 	[ItemColumnType.Task] = {},
    -- 	[ItemColumnType.Material] = {},
    -- 	[ItemColumnType.Storage] = {},
    -- }

    self.columns_size = {}

    -- self:SetColumnSize(ItemColumnType.Item,ItemColumnMaxCount[ItemColumnType.Item])
    -- self:SetColumnSize(ItemColumnType.Supply,20)
    -- self:SetColumnSize(ItemColumnType.Task,20)
    -- self:SetColumnSize(ItemColumnType.Material,20)

    self.view_data =
        SmartData.New(
        {
            selected_col = ItemColumnType.None, --当前选择的物品类型标签
            info_item = {},
            element_crystal_id = 0, -- 元素水晶id
            used_times = 0,
            medal_id = 0,
            medal_info = { },
            medal_bag_select = {},
            medal_take_off = false,
        }
    )

    self.new_item = SmartData.New({items_data = {}, is_new = false})
    self.better_equip = SmartData.New({is_better = false,mark_str = ""})
    self.equipment_list = SmartData.New()
    self.equipment_voc_list = SmartData.New()
    self.equip_sort_upgrade = SmartData.New()
    self.jewelry_list = SmartData.New()
    self.medal_item = SmartData.New({item_id = 0})

    self.storage_type = SmartData.New({is_storage = false})
    self.medal_sp_tip = {}
    self.medal_take_off_str = ""
    
    self:CheckConfig()

    self.crystal_red_piont = SmartData.New({val = false})
    self.crystal_red_point_show = true          -- 玩家点击水晶佩戴框后，无论是否佩戴，佩戴框上的红点需要消失；下次登录再重新检测红点逻辑
    self.recv_item_notify_list = {}             -- 通知物品变化的列表

    self.get_way_sweep = {is_mark = false}      --检查是否被获取途径·快速扫荡中
    self.sweep_mark = SmartData.New({item_list = {}})
    self.role_rate_s = {}

    self.cur_storage_select_type = SmartData.New({val = 0})

    self.hide_currency_show_center_flag = false
    if IS_AUDIT_VERSION then
        local icon_replace_list = {}
        for _ , v in pairs(Config.agent_adapt_auto.icon_replace) do
            for i , _ in pairs(AudutSeqIndex) do
                icon_replace_list[i] = icon_replace_list[i] or {}
                if v["audit_seq_"..i] and v["audit_seq_"..i] == 1 then
                    icon_replace_list[i][v.icon_id] = 1
                end
            end
        end
        Config.agent_adapt_auto.icon_replace = icon_replace_list
    end
end

function BagData:GetCrystalRedPiont()
    return (self.crystal_red_piont.val and self.crystal_red_point_show) and 1 or 0
end

function BagData:SetCrystalRedPiont(val)
    self.crystal_red_piont.val = val
end

function BagData:ClearData()
    self.item_grid[ItemColumnType.Item]:Clear()
    self.item_grid[ItemColumnType.Supply]:Clear()
    self.item_grid[ItemColumnType.Task]:Clear()
    self.item_grid[ItemColumnType.Material]:Clear()
    self.item_grid[ItemColumnType.Storage]:Clear()
    self.item_grid[ItemColumnType.Illustration]:Clear()
    self.item_grid[ItemColumnType.Temporary]:Clear()
    self.item_grid[ItemColumnType.Accrssory]:Clear()
    self.item_grid[ItemColumnType.PetWeapon]:Clear()
    
    self.item_list[ItemColumnType.Item]:Clear()
    self.item_list[ItemColumnType.Supply]:Clear()
    self.item_list[ItemColumnType.Task] = {}
    self.item_list[ItemColumnType.Material] = {}
    self.item_list[ItemColumnType.Storage] = {}
    self.item_list[ItemColumnType.Illustration] = {}
    self.item_list[ItemColumnType.Temporary] = {}
    self.item_list[ItemColumnType.Accrssory] = {}

    self.crystal_red_point_show = true
end
function BagData:__delete()
    if BagData.Instance == self then
        BagData.Instance = nil
    end
end

function BagData:ViewData()
    return self.view_data
end

function BagData:CheckConfig()
    self.config_expand_price = {}
    for i = 1, #Config.bag_cfg_auto.expand_price do
        local items = Config.bag_cfg_auto.expand_price[i]
        local times = items.times
        self.config_expand_price[times] = {}
        for k, v in pairs(items) do
            if BagExpandPriceConvert[k] then
                self.config_expand_price[times][BagExpandPriceConvert[k]] = v
            end
        end
    end
    self.medal_sp_list = {}
    for k,v in pairs(self.medal_sp_cfg) do 
        if self.medal_sp_list[v.page] == nil then 
            self.medal_sp_list[v.page] = {}
        end 
        table.insert( self.medal_sp_list[v.page], v)
    end 

    self.item_getway_cfg = KeyList(Config.getway_cfg_auto.get_way,"id")

    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoRandActivityConfig_1,function() 
            self.act_item_config = KeyList(Config.randactivityconfig_1_auto.activity_id,"item_id")
        end,true)

    self.medal_build_level_config = KeyList(Config.medal_cfg_auto.medal_exchange,"seq")
    self.equip_score_config = KeyList(Config.score_cfg_auto.equipment_score,"level")
    self.special_spe_list = KeyList(Config.special_effect_auto.spe_list,"sp_id")

    self.equip_main_attr_config = {}
    for k,v in pairs(Config.score_cfg_auto.main_attr) do
        if self.equip_main_attr_config[v.equip_type] == nil then 
            self.equip_main_attr_config[v.equip_type] = {}
        end 
        self.equip_main_attr_config[v.equip_type][v.job_type] = v.attr_type
    end 

    -- self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))

    -- 即将改版（报废
    self.medal_effect_rank_list = {}
    for k,v in pairs(self.medal_sp_cfg) do 
        local effect_cfg = nil
        for i,j in pairs(self.special_effect_auto.spe_list) do 
            if j.sp_id == v.sp_id then 
                effect_cfg = j
                break
            end 
        end 
        if self.medal_effect_rank_list[effect_cfg.sp_type] == nil then 
            self.medal_effect_rank_list[effect_cfg.sp_type] = {}
        end 
        table.insert( self.medal_effect_rank_list[effect_cfg.sp_type], v)
    end 

end

function BagData:CheckMedalMark()
    self.medal_redtips.mark_point = UnityPlayerPrefs.GetString(PrefKeys.GetMedalBuildMark())
end

function BagData:GetWayCfg(id)
    return self.item_getway_cfg[tonumber(id)]
end

function BagData:GetColumnList(col_type)
    return self.item_list[col_type]
end

function BagData:GetColumnGrids(col_type)
    return self.item_grid[col_type]
end

function BagData:ItemDataCheck()

    for _type , info in pairs(self.item_grid) do
        if self:IsNewItem() and nil ~= self:GetNewItemCols()[_type] then 
            for k, v in pairs(self.new_item.items_data) do
                if _type == v.column_type then
                    if self.item_grid[_type][v.index] ~= nil then 
                        self.item_grid[_type][v.index].is_new = true
                    end 
                end
            end
        end 
    end

end

function BagData:GetStorageBagTypeList()
    if not self.storage_bag_type_list then
        self.storage_bag_type_list = {}
        local type_name = Language.Storage.BagTypes
        for _, type in pairs(StorageConfig.BagSortTypes) do
            local data = {
                type = type,
                name = type_name[type]
            }
            table.insert(self.storage_bag_type_list, data)
        end
        local function sort_type(a, b)
            return a.type < b.type
        end
        table.sort(self.storage_bag_type_list, sort_type)
    end
    return self.storage_bag_type_list
end

function BagData:GetStorageBGridsWithType(type, max)
    local max = max or 25
    local grid = {}

    for _, item in pairs(self.item_list[ItemColumnType.Item]) do
        if item:IsDeposit() then 
            table.insert(grid, item)
        end 
    end
    for _, item in pairs(self.item_list[ItemColumnType.Supply]) do
        if item:IsDeposit() then 
        table.insert(grid, item)
        end
    end
    for _, item in pairs(self.item_list[ItemColumnType.Task]) do
        if item:IsDeposit() then 
        table.insert(grid, item)
        end
    end
    for _, item in pairs(self.item_list[ItemColumnType.Material]) do
        if item:IsDeposit() then 
        table.insert(grid, item)
        end 
    end
    for _, item in pairs(self.item_list[ItemColumnType.Illustration]) do
        if item:IsDeposit() then 
        table.insert(grid, item)
        end 
    end

    local co1, co2
    local types = StorageConfig.BagSortTypes
    if type == types.Equip then
        co1 = ItemType.Weapon
        co2 = ItemType.Armor
    elseif type == types.Illustration then
        co1 = ItemType.Illustration
    elseif type == types.Gem then
        co1 = ItemType.Gem
    end

    local function sort_type(a, b)
        local at = a.item_type
        local bt = b.item_type

        local ac = at == co1 or at == co2
        local bc = bt == co1 or bt == co2

        if ac then
            if bc then
                return false
            else
                return true
            end
        else
            return false
        end
    end

    if type ~= types.Normal then
        local s = sort_type
        table.sort(grid, sort_type)
    end

    local item_block = SmartData.New(Item.New())
    item_block.column_type = ItemColumnType.Item

    --补齐空格子，至少显示max个
    local block = #grid % StorageConfig.BAG_ROW_MAX == 0 and StorageConfig.BAG_ROW_MAX or 2 * StorageConfig.BAG_ROW_MAX - #grid % StorageConfig.BAG_ROW_MAX
    if block + #grid < max then
        block = max - #grid
    end

    for i = 1, block do
        table.insert(grid, item_block)
    end

    return grid
end

function BagData:GetStorageSGridsWithPage(page)
    local grid = {}
    if page < 1 or page > StorageConfig.STORAGE_PAGE_MAX then
        return grid
    end
    local data = self.item_grid[ItemColumnType.Storage]
    local page_min = (page - 1) * StorageConfig.STORAGE_PAGE_PER
    local page_max = page_min + StorageConfig.STORAGE_PAGE_PER - 1
    for i = page_min, page_max do
        table.insert(grid, data[i])
    end
    return grid
end

function BagData:GetStoragePageList()
    if not self.storage_page_list then
        self.storage_page_list = {}
        for i = 1, StorageConfig.STORAGE_PAGE_MAX do
            table.insert(self.storage_page_list, {index = i})
        end
    end
    return self.storage_page_list
end

function BagData:IsNotMaxUnLock(col_type)
    local info = self:GetColumnInfo(col_type)
    if info ~= nil then
        return info.extend_times < self.bag_expand_price[#self.bag_expand_price].times
    end
    return true
end

function BagData:SetColumnSize(col_type, size, extend_times)
    self.columns_size[col_type] = self.columns_size[col_type] or {}
    self.columns_size[col_type].size = size
    self.columns_size[col_type].extend_times = extend_times
    if self.config_expand_price[extend_times + 1] then
        self.columns_size[col_type].expand_price = self.config_expand_price[extend_times + 1][col_type]
    end
    local column_list = self.item_grid[col_type]
    if column_list == nil then
        return
    end
    for i = size - 1, 0, -1 do
        if column_list[i] ~= nil then
            return
        else
            --test data袽
            -- local rnd_flag = GameMath.Ramdon(100)
            -- if rnd_flag > 50 then
            -- 	local iid
            -- 	while iid == nil do
            -- 		local t = 26000 + GameMath.Ramdon(1000)
            -- 		if Item.GetConfig(t) ~= nil then iid = t end
            -- 	end
            -- 	item.item_id = iid
            -- 	item.num = GameMath.Ramdon(20)
            -- end
            local item = SmartData.New(Item.New())
            item.column_type = col_type
            item.index = i
            column_list[i] = item
        end
    end
end

function BagData:GetMaxColumnSize(col_type)
    local config = self:GetUnLockNumConfig(col_type)
    return config.max_num or 0
end

function BagData:GetColumnSize(col_type)
    local size = 0
    if self.columns_size[col_type] and self.columns_size[col_type].size then
        size = self.columns_size[col_type].size
    end
    return size
end

function BagData:GetColumnInfo(col_type)
    return self.columns_size[col_type]
end

function BagData:SetItemData(item_protc, reason, put_reason)
    --LogError("Bag::::", item_protc.item_id, reason, put_reason, "Param:",item_protc.param)

    local col_grids = self.item_grid[item_protc.column]
    if col_grids == nil then
        return
    end
    local grid_item = col_grids[item_protc.index]
    if grid_item == nil then
        return
    end
    if item_protc.item_id ~= 0 and reason == BAGITEM_CHANGEREASON.OUTSIDE_IN then
        local grid_num = grid_item:Num()
        -- 捕获宠物获得宠物碎片不走通用飘字
        if item_protc.num - grid_num > 0 or (grid_num > 0 and item_protc.item_id ~= grid_item.item_id) then
            local item_id, num = item_protc.item_id, item_protc.num - grid_item:Num()
            if nil == put_reason or not BlockCenter[put_reason] then
                local i_cfg = Item.GetConfig(item_id)
                local item = Item.Create(item_protc)
                item.num = num
                if i_cfg == nil then LogError("item ",item_id,"is not exist!") return end 
                if i_cfg.show_type and i_cfg.show_type == 26 then
                    BattleCtrl.Instance:DelayListen(
                        BATTLE_DELAY_REASON.SEAL_PET,
                        function()
                            PublicPopupCtrl.Instance:CenterI({item_id = item_id, num = num,color_str = item:ColorStr()})
                            ChatCtrl.Instance:ChannelChatCustom {item_get = {item = item, num = num}}
                        end
                    )
                else
                    -- local _item = Item.Create(item)
                    PublicPopupCtrl.Instance:CenterI({item_id = item_id, num = num,
                    color_str = item:ColorStr(),item = item,icon_id = item:IconId()})
                    ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}
                end
                if put_reason ~= nil then
                    if put_reason == PUT_REASON_TYPE.PUT_REASON_FU_YU_UPGRADE_SUCC
                    or put_reason == PUT_REASON_TYPE.PUT_REASON_FU_YU_UPGRADE_FAIL then
                        PartnerData.Instance.qiang_hua_jie_guo = (put_reason == PUT_REASON_TYPE.PUT_REASON_FU_YU_UPGRADE_SUCC)
                        PartnerData.Instance:OpenFuYuQiangHua(item, true)                        
                    end
                    -- if put_reason == PUT_REASON_TYPE.PUT_REASON_COLOSSEUM_BATTLE_REWARD then
                    --     --缓存竞技场奖励 
                    -- end
                end
            -- 家族试炼章节奖励获取单独处理（跑马灯延迟）
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_GUILD_BOSS_CHAPTER_REWARD then
                GuildData.Instance.guild_trial_reward_item.item = {item_id = item_id, num = num}
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_CATCH_PET then
                TaskData.Instance:TaskChainSubContinue(true)
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_PARTNER_CRYTALS_BAD then
                local item_id, num = item_protc.item_id, item_protc.num - grid_item:Num()
                local item = Item.Create(item_protc)
                PublicPopupCtrl.Instance:CenterI({item_id = item_id, num = num, item = item, icon_id = 40040, color_str = COLORSTR.Gray2})
                ChatCtrl.Instance:ChannelChatCustom {item_get = {item = item, num = num, is_grey = true}}
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_PET_BREAKTHROUGH then
                --宠物强化吃掉宠物
                local item = Item.Create(item_protc)
                item.num = num
                ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}
            -- 境界突破延时播报
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_REALM_BREAK then
                local item = Item.Create({item_id = item_id, num = num})
                RoleData.Instance:SaveNeedDelayItem(item)
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_PET_WEAPON_CONVERT then
                local item = Item.Create(item_protc)
                local param_t = {
                    data = {
                        old_weapon = item,
                        cur_weapon = item,
                    },
                    show_type = PetWeaponUpShowType.ConvertGet
                }
                ViewMgr:OpenView(PetWeaponUpShowView, param_t)
                ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}
                PublicPopupCtrl.Instance:CenterI({item_id = item_id, num = num,
                    color_str = item:ColorStr(),item = item,icon_id = item:IconId()})
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_PET_WEAPON_CHANGE then
                -- local item = Item.Create(item_protc)
                -- ViewMgr:OpenView(GiftOpenView,{item = item,gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS})
                PublicPopupCtrl.Instance:CenterI({item_id = item_id, num = num == 0 and 1 or num})
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_PET_BOOK_CHANGE or put_reason == PUT_REASON_TYPE.PUT_REASON_PET_STAMP_CHANGE
            or put_reason == PUT_REASON_TYPE.PUT_REASON_SHENG_QI_BOOK_CHANGE
            or put_reason == PUT_REASON_TYPE.PUT_REASON_PET_BAGDE_COM then
                local item
                if put_reason == PUT_REASON_TYPE.PUT_REASON_PET_BAGDE_COM then
                    item = Item.Create(item_protc)
                else
                    item = Item.Create({item_id = item_protc.item_id, num = 1})
                end
                ViewMgr:OpenView(GiftOpenView,{item_list = {item},gift_type = GIFT_OPEN_TYPE.SPECIAL})
                PetData.Instance:ClearChangeBookData()
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_RA_ZHAOCAIJINBAO then
                local item = Item.Create({item_id = item_protc.item_id, num = num, is_bind = item_protc.is_bind and 1 or 0})
                ChatCtrl.Instance:ChannelChatCustom({item_get = {item = item, num = num}})
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_AUSPICIOUS_SNOW_YEAR then
                local item = Item.Create({item_id = item_protc.item_id, num = num, is_bind = item_protc.is_bind and 1 or 0})
                ChatCtrl.Instance:ChannelChatCustom({item_get = {item = item, num = num}})
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_DIAN_HUA_SHI then 
                local item = Item.Create(item_protc)
                WorkshopData.Instance:SetNevealExtractItem(item)
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_RABBIT_RACE then
                local item = Item.Create({item_id = item_protc.item_id, num = num, is_bind = item_protc.is_bind and 1 or 0, index = item_protc.index})
                table.insert(RabbitRaceData.Instance.reward_list, item)
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_SKY_LANTERN then
                local item = Item.Create({item_id = item_protc.item_id, num = num, is_bind = item_protc.is_bind and 1 or 0})
                SkyLanternData.Instance:SaveNeedDelayItem(item)
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_CARD_REWARD or put_reason == PUT_REASON_TYPE.PUT_REASON_COUNT_REWARD then
                local item_id, num = item_protc.item_id, item_protc.num - grid_item:Num()
                local item = Item.Create({item_id = item_protc.item_id, num = num, is_bind = item_protc.is_bind and 1 or 0})
                PublicPopupCtrl.Instance:CenterI({item_id = item_id, num = num,
                    color_str = item:ColorStr(),item = item,icon_id = item:IconId()})
                ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}
                -- if put_reason == PUT_REASON_TYPE.PUT_REASON_CARD_REWARD then
                --     VitalityFlopData.Instance:SetCardReward(item, true)
                -- end
            elseif  put_reason == PUT_REASON_TYPE.PUT_REASON_JIN_YI_TIAN_XIANG  then 
                local item = Item.Create(item_protc)
                if item_id == ClothAromaData.Instance:StuffItemId() then 
                    PublicPopupCtrl.Instance:CenterI({item_id = item_id, num = num,
                    color_str = item:ColorStr(),item = item,icon_id = item:IconId()})
                    ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}
                end
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_JIN_LONG_JU_BAO_DRAW  then 
                -- local item = Item.Create(item_protc)
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_DUAN_YANG_XIANG_NANG then 
                if NoonSachetData.Instance:GetMissionMark() then 
                    local item = Item.Create(item_protc)
                    PublicPopupCtrl.Instance:CenterI({item_id = item_id, num = num,
                    color_str = item:ColorStr(),item = item,icon_id = item:IconId()})
                    ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}

                    NoonSachetData.Instance:SetMissionMark(false)
                end
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_PET_WEAPON_FANLI then
                if PetWeaponFanliData.Instance.stage_cfg then
                    local cfg = PetWeaponFanliData.Instance.stage_cfg
                    local reward_item = Item.Create(cfg.reward1_item)
                    ActRewardTipData.Instance:OpenActRewardTipViewByData(ServeractivityConfig.ActRewerdTip.RewardType.Success,{reward_item})
                    PublicPopupCtrl.Instance:CenterI({item_id = reward_item.item_id, num = reward_item.num,
                        color_str = reward_item:ColorStr(),item = reward_item,icon_id = reward_item:IconId()})
                    ChatCtrl.Instance:ChannelChatCustom{item_get = {item = reward_item, num = reward_item.num}}
                    PetWeaponFanliData.Instance.stage_cfg = nil
                end
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_ZHUI_YUE_SHANG_CHENG_DRAW then
                local item = Item.Create(item_protc)
                ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_HALLOWEEN_PRAY then
                local item = Item.Create(item_protc)
                ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}
            elseif put_reason == PUT_REASON_TYPE.PUT_REASON_SWITCH_EQUIP_OR_JEWELRY then
                local item = Item.Create(item_protc)                      
                MainOtherCtrl.Instance:OpenGiftView({[1]= item}, GIFT_OPEN_TYPE.CONGRATULATIONS, true)
                EquipData.Instance:ClearEquipChange()
            end
            --其他飘字
            -- if SilkRoadData.Instance:IsGatherSubmission(item_id,num) then
            --     ChatCtrl.Instance:ChannelChatCustom {content = Language.SilkRoad.CanSubmissionTip}
            -- end
        end
    end
    if item_protc.item_id ~= 0 and reason == BAGITEM_CHANGEREASON.NOLMAL_OR_PARAM then
        if put_reason == PUT_REASON_TYPE.PUT_REASON_HEART_SKILL_CHIP then
            local item = Item.Create(item_protc)
            item.num = item_protc.num - grid_item:Num()
            MainOtherCtrl.Instance:OpenGiftView({item},GIFT_OPEN_TYPE.SPECIAL)
            MainOtherData.Instance:QuickUseItem(item)
        elseif put_reason == PUT_REASON_TYPE.PUT_REASON_HEART_SKILL_CHANGE then
            local item = Item.Init(item_protc.item_id,item_protc.num - grid_item:Num())
            MainOtherCtrl.Instance:OpenGiftView({item},GIFT_OPEN_TYPE.CONGRATULATIONS)
        elseif put_reason == PUT_REASON_TYPE.PUT_REASON_ONE_KEY_GRADE_MEDAL then
            local item = Item.Create(item_protc)
            MedalData.Instance:CacheMedalAllUpgrade(item)
        elseif put_reason == PUT_REASON_TYPE.PUT_REASON_BICYCLE_BOOK_ALL_UP then
            local item = Item.Create(item_protc)
            BicycleOtherData.Instance:CacheSkillBookAllUpgrade(item)
        elseif put_reason == PUT_REASON_TYPE.PUT_REASON_DEF_GIFT then
            local item = Item.Create(item_protc)
            self:CrystalEquipShowUse(item)
        elseif put_reason == PUT_REASON_TYPE.PUT_REASON_MOUNTAIN_SEA_MINEC then
            local item = Item.Create(item_protc)
            ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = item_protc.num-grid_item:Num()}}
        elseif put_reason == PUT_REASON_TYPE.PUT_REASON_LIVE_MAKE_ITEM then
            -- 生活炼制
            local param_t = {}
            local grid_num = grid_item:Num()
            local num = item_protc.num - grid_num
            local refine_count = LiveData.Instance.refine_count or 1
            if num - refine_count > 0 then
                param_t.tip = string.format(Language.Live.RefineResultTip1, num - refine_count)
            end
            LiveData.Instance.refine_count = 1
            local item = Item.Create(item_protc)
            item.num = num
            param_t.item_list = {item}
            ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}
            if LiveData.Instance:GetSelSkillType() == LiveCtrl.LIVE_SKILL_TYPE.COOK then
                param_t.title = 2
            else
                param_t.title = 1
            end
            ViewMgr:OpenView(LiveRefineResultView, param_t)
        elseif put_reason == PUT_REASON_TYPE.PUT_REASON_GATHER_OBJECT or put_reason == PUT_REASON_TYPE.PUT_REASON_LIVE_FISH then
            local grid_num = grid_item:Num()
            local num = item_protc.num - grid_num
            local item = Item.Create(item_protc)
            item.num = num
            ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item, num = num}}
            PublicPopupCtrl.Instance:CenterI({item_id = item_protc.item_id, num = num,color_str = item:ColorStr(),item = item,icon_id = item:IconId()})
        elseif put_reason == PUT_REASON_TYPE.PUT_BIYCCLE_SKILL_COMPOSE then
            local item
            if put_reason == PUT_REASON_TYPE.PUT_REASON_PET_BAGDE_COM then
                item = Item.Create(item_protc)
            else
                item = Item.Create({item_id = item_protc.item_id, num = 1})
            end
            ViewMgr:OpenView(GiftOpenView,{item_list = {item},gift_type = GIFT_OPEN_TYPE.SPECIAL})
            --BicycleOtherData.Instance:ClearStuff()
        elseif put_reason == PUT_REASON_TYPE.PUT_REASON_FU_YU_SWITCH then  
            local item = Item.Create(item_protc)                      
            MainOtherCtrl.Instance:OpenGiftView({[1]= item}, GIFT_OPEN_TYPE.CONGRATULATIONS, true)
            PartnerData.Instance:ClearFuYuChange()
        elseif put_reason == PUT_REASON_TYPE.PUT_REASON_FU_YU_SUBLIME then
            PartnerData.Instance:SetSubFuYu(Item.Create(item_protc))
        elseif put_reason == PUT_REASON_TYPE.PUT_REASON_PET_AMULET_INSTENSIFY then
            local item = Item.Create(item_protc)
            ViewMgr:OpenView(AmuletAssemblyInfoView,{item = item})
            AmuletData.Instance:SetAmuletIntensifyData(1, item)
        end
    end
    grid_item.item_id = item_protc.item_id
    grid_item.num = item_protc.num
    grid_item.is_bind = item_protc.is_bind
    grid_item.item_type = grid_item.GetType(grid_item.item_id) or ItemType.Normal
    grid_item.item_unique_id_1 = item_protc.item_unique_id_1
    grid_item.item_unique_id_2 = item_protc.item_unique_id_2

    local item = SmartData.New(Item.Create(item_protc))
    item.column_type = grid_item.column_type
    item.item_type = grid_item.item_type
    col_grids[item_protc.index] = item

    if grid_item.item_id ~= 0 then
        self.item_list[grid_item.column_type][grid_item.index] = item
    else
        self.item_list[grid_item.column_type][grid_item.index] = nil
    end

    if grid_item.column_type == ItemColumnType.Temporary and reason == BAGITEM_CHANGEREASON.OUTSIDE_IN then 
        PublicPopupCtrl.Instance:Center(Language.Bag.BagEnterTemporary)
    end 

    --这里检查是否满足装备条件
    if self:ShouldBeCheckItemNew(grid_item.column_type) then
        if reason == BAGITEM_CHANGEREASON.OUTSIDE_IN and put_reason ~= PUT_REASON_TYPE.PUT_REASON_NO_NOTICE then
            self:SetNewItem(grid_item)
        elseif self:ShouldBeTimeToClear(reason) then
            self:ClearOnesNewItem(grid_item)
        end
    end

    if grid_item.column_type == ItemColumnType.Item then
        if reason == BAGITEM_CHANGEREASON.OUTSIDE_IN then
            self:EquipShowUse(grid_item)
            self:CrystalEquipShowUse(grid_item)

        end
        if reason == BAGITEM_CHANGEREASON.NOLMAL_OR_PARAM then
            self:ClearOnesNewItem(grid_item)
            self:CheckIsBetter(BETTER_CHECK_REASON.BAG_CHANGE,item_protc.index)
        elseif reason == BAGITEM_CHANGEREASON.OUTSIDE_IN then
            -- 完成穿着时进行结算
            WorkshopData.Instance:UnMarkDrawOffEquip(grid_item)

            self:CheckIsBetter(BETTER_CHECK_REASON.BAG_IN_CHANGE,item_protc.index)
        elseif reason == BAGITEM_CHANGEREASON.INSIDE_OUT  then 
            self:CheckIsBetter(BETTER_CHECK_REASON.BAG_OUT_CHANGE,item_protc.index)
        elseif reason == BAGITEM_CHANGEREASON.CLEARUP then 
            self:CheckIsBetter(BETTER_CHECK_REASON.BAG_IN_CHANGE)
        end
        
        if reason == BAGITEM_CHANGEREASON.OUTSIDE_IN or reason == BAGITEM_CHANGEREASON.CLEARUP then
            if not BlockItemAutoUse[put_reason] then
                self:GiftShowUse(grid_item)
                self:LingQiOpenFastUse(grid_item)
            end
        end
        if put_reason == PUT_REASON_TYPE.PUT_REASON_RAND_CONSUMER_CAROUSEL or put_reason == PUT_REASON_TYPE.PUT_REASON_KF_LUCK_ROLLER then
            self:GiftShowUse(grid_item)
        end
        if reason == BAGITEM_CHANGEREASON.OUTSIDE_IN then
            if grid_item:IsEquip() or grid_item:IsJewelry() then
                PrestigeData.Instance:CheckDonateCacheNum(grid_item.index)
            end
        
        end
        if put_reason == PUT_REASON_TYPE.PUT_REASON_NOREXITEM_EQUIP and grid_item:IsEquip() then
            ItemInfoCtrl.Instance:EffectShow(Item.Create(item_protc))
        end
        if put_reason == PUT_REASON_TYPE.PUT_REASON_SPIRIT_LEVEL_UP then
            EquipData.Instance.spirit_level_data.spirit_level_bag = item
            EquipData.Instance.spirit_level_data.spirit_level_config = item.param
        end
        
        if put_reason == PUT_REASON_TYPE.PUT_REASON_FU_YU_UPGRADE_SUCC
        or put_reason == PUT_REASON_TYPE.PUT_REASON_FU_YU_UPGRADE_FAIL then
            ViewMgr:OpenView(PartnerFuYuResult)
        end
    end

    if grid_item.column_type == ItemColumnType.Item or grid_item.column_type == ItemColumnType.Material then
        if reason == BAGITEM_CHANGEREASON.OUTSIDE_IN then
            -- 道具检查
            WorkshopData.Instance:CheckIsComppseStuff(item_protc.item_id)
        end 
    end 
    

end

function BagData:SetGetWaySweepMark(data)
    for k,v in pairs(data) do 
        self.get_way_sweep[k] = v
    end 
end

function BagData:ClearGetWaySweepMark()
    self.get_way_sweep = {}
end

-- 检查是否通过获取途径进行扫荡
function BagData:CheckIsSweepByGetWay(item_list)
    if #item_list > 0 then 
        WorkshopData.Instance:ClearUpgradeRemindIgnore()
    end 
    if self.get_way_sweep.is_mark == true and self.get_way_sweep.is_mark ~= nil then 
        if not ViewMgr:IsOpen(SweepResultView) then 
            ViewMgr:OpenView(SweepResultView)
        end 
        self.sweep_mark.item_list = DataHelper.FormatItemList(item_list)
        return false
    end 
    return true
end

-- 输入物品id来获取该背包类型是否有空位
-- 返回true是有空位 -- 返回false是没有空位
function BagData:TryFindEmptyInBag(item_id)
    local item_cfg = Item.GetConfig(item_id)
    if item_cfg.bag_type == nil  then return true end 
    for k, v in pairs(self.item_grid[item_cfg.bag_type] or {}) do
        if v.item_id == 0 and v.num == 0 then
            return true
        end
    end

    return false
end

function BagData:TryFindEmptyInBagByType(bag_type)
    for k, v in pairs(self.item_grid[bag_type or ItemColumnType.Item]) do
        if v.item_id == 0 and v.num == 0 then
            return true
        end
    end
    return false
end

function BagData:TryFindEmptyInBagByTypeNum(bag_type, num)
    local empty_num = 0
    for k, v in pairs(self.item_grid[bag_type or ItemColumnType.Item]) do
        if v.item_id == 0 and v.num == 0 then
            empty_num = empty_num + 1
        end
    end
    return empty_num >= num
end

function BagData:TryFindEmptyInTempBag()
    for k, v in pairs(self.item_grid[ItemColumnType.Temporary]) do
        if v.item_id == 0 and v.num == 0 then
            return true
        end
    end

    return false
end

-- 指定数量的空格
function BagData:TryFindEmptyInTempBagMulit(num)
    local total_num = num
    for k, v in pairs(self.item_grid[ItemColumnType.Temporary]) do
        if v.item_id == 0 and v.num == 0 then
            total_num = total_num - 1
        end

        if total_num == 0 then
            return true
        end
    end

    return false
end

--背包是否有空位放下item_data_list
function BagData:TryFindEmptyForList(bag_type, item_data_list)
    local bag_empty_num = 0
    for k, v in pairs(self.item_grid[bag_type or ItemColumnType.Item]) do
        if v.item_id == 0 and v.num == 0 then
            bag_empty_num = bag_empty_num + 1
        end
    end
    local temp_bag_empty_num = 0
    for k, v in pairs(self.item_grid[ItemColumnType.Temporary]) do
        if v.item_id == 0 and v.num == 0 then
            temp_bag_empty_num = temp_bag_empty_num + 1
        end
    end
    for x, y in pairs(item_data_list) do
        local item_id = y.item_id
        local num = y.num
        local cfg = Item.GetConfig(item_id)
        local grids
        for k, v in pairs(self.item_grid[bag_type or ItemColumnType.Item]) do
            if v.item_id == item_id and v.num < cfg.pile_limit then
                num = num -(cfg.pile_limit- v.num)  
                break
            end
        end
        if num > 0 then
            grids = math.ceil(num / cfg.pile_limit)
            if bag_empty_num > 0 then
                if  bag_empty_num>=grids then
                    num = num - cfg.pile_limit * bag_empty_num
                    bag_empty_num = 0
                else
                    num = 0
                    bag_empty_num = bag_empty_num - grids
                end
            end
        end
        if num > 0 then
            for k, v in pairs(self.item_grid[ItemColumnType.Temporary]) do
                if v.item_id == item_id and v.num < cfg.pile_limit then
                    num = num - (cfg.pile_limit-v.num)
                    break
                end
            end
            if num > 0 then
                grids = math.ceil(num / cfg.pile_limit)
                if temp_bag_empty_num >= grids then
                    temp_bag_empty_num=temp_bag_empty_num-grids
                else
                    return false
                end
            end
        end
    end
    return true
end

function BagData:ShouldBeCheckItemNew(column_type)
    return column_type == ItemColumnType.Item or column_type == ItemColumnType.Supply or column_type == ItemColumnType.Task or column_type == ItemColumnType.Material or
        column_type == ItemColumnType.Illustration
end

function BagData:ShouldBeTimeToClear(reason)
    return reason == BAGITEM_CHANGEREASON.INSIDE_OUT or reason == BAGITEM_CHANGEREASON.INSIDE_MOVE or reason == BAGITEM_CHANGEREASON.CLEARUP
end

function BagData:ClearEmptyItem(new_item_list)
    local item_map = {}
    for k, item in pairs(new_item_list) do
        if item_map[item.column] == nil then
            item_map[item.column] = {}
        end
        item_map[item.column][item.index] = item
    end
    for column, list in pairs(item_map) do
        if self.item_list[column] then
            for index, v in pairs(self.item_list[column]) do
                if list[index] == nil then
                    local item = SmartData.New(Item.New())
                    item.column_type = column
                    item.index = index
                    self.item_grid[column][index] = item
                    self.item_list[column][index] = nil
                end
            end
        end
    end
end

-- 获取背包中的宠物技能书
function BagData:GetSkillGrid()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local skill_list = {}
    for k, v in pairs(col_grids) do
        if v.item_type == ItemType.Skill then
            table.insert(skill_list, v)
        end
    end
    --还要排序
    table.sort(
        skill_list,
        function(a, b)
            return a:Color() > b:Color()
        end
    )
    return skill_list
end

function BagData:GetAllItemsByItemId(item_id)
    -- local grid = {}
    local item_list = {}
    local num = 0
    for _, item in pairs(self.item_list[ItemColumnType.Item]) do
        if item_id == item.item_id then
            num = num + item.num
            table.insert(item_list, item)
        end
    end
    for _, item in pairs(self.item_list[ItemColumnType.Supply]) do
        if item_id == item.item_id then
            num = num + item.num
            table.insert(item_list, item)
        end
    end
    for _, item in pairs(self.item_list[ItemColumnType.Task]) do
        if item_id == item.item_id then
            num = num + item.num
            table.insert(item_list, item)
        end
    end
    for _, item in pairs(self.item_list[ItemColumnType.Material]) do
        if item_id == item.item_id then
            num = num + item.num
            table.insert(item_list, item)
        end
    end
    for _, item in pairs(self.item_list[ItemColumnType.Illustration]) do
        if item_id == item.item_id then
            num = num + item.num
            table.insert(item_list, item)
        end
    end
    for _, item in pairs(self.item_list[ItemColumnType.PetWeapon]) do
        if item_id == item.item_id then
            num = num + item.num
            table.insert(item_list, item)
        end
    end
    -- for k, v in pairs(grid) do
    --     if item_id == v.item_id then
    --         num = num + v.num
    --         table.insert(item_list, v)
    --     end
    -- end
    return num, item_list
end

function BagData:GetStorageNumByItemId(item_id)
    local grid = {}
    for _, item in pairs(self.item_list[ItemColumnType.Storage]) do
        table.insert(grid, item)
    end
    local item_list = {}
    local num = 0
    for k, v in pairs(grid) do
        if item_id == v.item_id then
            num = num + v.num
            table.insert(item_list, v)
        end
    end
    return num
end

function BagData:GetNumByItemId(item_id, cfg)
    if cfg == nil then
        cfg = Item.GetConfig(item_id)
    end
    local bag_list = self.item_list[cfg.bag_type]
    local num = 0 -- self:GetAllItemsByItemId(item_id)
    if bag_list ~= nil then
        for _, item in pairs(bag_list) do
            if item_id == item.item_id then
                num = num + item.num
            end
        end
    end
    return num
end

function BagData:GetItemsByItemId(item_id)
    local grid = {}
    for _, list in pairs(self.item_list) do
        for _, item in pairs(list) do
            if item_id == item.item_id then
                table.insert(grid, item)
            end
        end
    end
    return grid
end

function BagData:GetItemsByShowType(show_type)
    local grid = {}
    for _, list in pairs(self.item_list) do
        for _, item in pairs(list) do
			local co = Item.GetConfig(item.item_id)
			if co and co.show_type and co.show_type == show_type then
                table.insert(grid, item)
            end
        end
    end
    return grid
end

-- 获取水晶物品
function BagData:GetCrystalGrid(item_id, block_bad)
    item_id = item_id or 0
    local col_grids = self.item_grid[ItemColumnType.Item]
    local crystal_list = {}
    if item_id > 0 then
        for k, v in pairs(col_grids) do
            if v.item_id == item_id then
                if not block_bad or v:UsedTime() > 0 then
                    table.insert(crystal_list, v)
                end
            end
        end
    else
        for k, v in pairs(col_grids) do
            if v.item_type == ItemType.EleCrystal then
                if not block_bad or v:UsedTime() > 0 then
                    table.insert(crystal_list, v)
                end
            end
        end
    end
    return crystal_list
end

-- 背包是否有没坏的水晶(红点检测用)
function BagData:GetHaveCrystal()
    local have_crystal = false
    local col_grids = self.item_grid[ItemColumnType.Item]
    for k, v in pairs(col_grids) do
        if v.item_type == ItemType.EleCrystal then
            if v:UsedTime() > 0 then
                have_crystal = true
                break
            end
        end
    end
    return have_crystal
end


-- 获取伙伴命格
function BagData:GetPartnerEquipGrid(type, prof_type)
    local col_grids = self.item_grid[ItemColumnType.Item]
    local role_level = RoleData.Instance:GetRoleLevel()
    local list = {}
    local min_level, max_level
    if nil == prof_type then
        for _, item in pairs(col_grids) do
            if item.item_type == ItemType.PartnerEquip and item:EquipmentType() == type - 1 then
                table.insert(list, item)
            end
        end
    else
        for _, item in pairs(col_grids) do
            if item.item_type == ItemType.PartnerEquip and item:EquipmentType() == type - 1 and prof_type == item:PESuitProf() and item.limit_level <= role_level then
                min_level = min_level or item.limit_level
                min_level = item.limit_level < min_level and item.limit_level or min_level
                max_level = max_level or item.limit_level
                max_level = max_level < item.limit_level and item.limit_level or max_level
                table.insert(list, item)
            end
        end
    end
    return list, min_level or 0, max_level or 0
end

-- 获取伙伴命格列表 level == nil 获取全部 否则获取对应等级的
function BagData:GetPartnerEquipList(level)
    local col_grids = self.item_grid[ItemColumnType.Item]
    local list = {}
    for _, item in pairs(col_grids) do
        if item.item_type == ItemType.PartnerEquip then
            if level then
                if item.limit_level == level then
                    table.insert(list, item)
                end
            else
                table.insert(list, item)
            end
        end
    end
    return list
end

function BagData:GetPetEquipmentList()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local list = {}
    for _,v in pairs(col_grids) do
        if Item.GetBigType(v.item_id) == ITEM_BIG_TYPE.PET_EQUIPMENT then
            table.insert(list,v)
        end
    end
    return list
end

-- 魂器仓库物品
function BagData:GetWeaponBagList()
    local col_grids = self.item_grid[ItemColumnType.PetWeapon] or {}
    local list = {}
    local sel_weapon = PetWeaponData.Instance:GetCurCtrlWeapon()
    for _,v in pairs(col_grids) do
        if Item.GetBigType(v.item_id) == ITEM_BIG_TYPE.PET_WEAPON or PetWeaponData.Instance.weapon_stuff_cfg[v.item_id] then
            if sel_weapon == nil or v ~= sel_weapon then
                table.insert(list,v)
            end
        end
    end
    col_grids = self.item_grid[ItemColumnType.Material]
    for _,v in pairs(col_grids) do
        if PetWeaponData.Instance.weapon_stuff_cfg[v.item_id] then
            table.insert(list,v)
        end
    end
    return list
end

function BagData:GetPetWeaponExpItemList()
    local col_grids = self.item_grid[ItemColumnType.Material]
    local list = {}
    for _,v in pairs(col_grids) do
        if PetWeaponData.Instance.weapon_stuff_cfg[v.item_id] then
            table.insert(list,v)
        end
    end
    return list
end

-- 背包所有魂器
function BagData:GetWeaponList(is_sel, qua)
    local col_grids = self.item_grid[ItemColumnType.PetWeapon] or {}
    local list = {}
    local sel_weapon
    if not is_sel then
        sel_weapon = PetWeaponData.Instance:GetCurCtrlWeapon()
    end
    for _,v in pairs(col_grids) do
        if Item.GetBigType(v.item_id) == ITEM_BIG_TYPE.PET_WEAPON then
            if sel_weapon == nil or v ~= sel_weapon then
                if not qua then
                    table.insert(list,v)
                else
                    if v:QuaType() == qua then
                        table.insert(list,v)
                    end
                end
            end
        end
    end
    return list
end

-- 背包所有神印经验材料
function BagData:GetPetStampExpStuffList()
    local list = {}
    local col_grids = self.item_grid[ItemColumnType.Item]
    for _,v in pairs(col_grids) do
        if PetStampData.Instance.stamp_stuff_cfg[v.item_id] then
            table.insert(list,v)
        end
    end

    col_grids = self.item_grid[ItemColumnType.Material]
    for _,v in pairs(col_grids) do
        if PetStampData.Instance.stamp_stuff_cfg[v.item_id] then
            table.insert(list,v)
        end
    end
    return list
end

-- 背包中所有对应套装的配饰（筛选套装：0或者不填就是默认全部，筛选部位：0或者不填就是默认全部)
function BagData:GetAccrssoryList(select_suit, select_part)
    local col_grids = self.item_grid[ItemColumnType.Accrssory] or {}
    local list = {}
    for _,v in pairs(col_grids) do
        if Item.GetBigType(v.item_id) == ITEM_BIG_TYPE.ACCRSSORY then
            -- 套装筛选和部位筛选
            if select_suit and select_suit ~= 0 then
                if select_part and select_part ~= 0 then
                    if v:AccrssoryType() == select_part and v:SuitType() == select_suit then
                        table.insert(list,v)
                    end
                else
                    if v:SuitType() == select_suit then
                        table.insert(list,v)
                    end
                end
            elseif select_part and select_part ~= 0 then
                if v:AccrssoryType() == select_part then
                    table.insert(list,v)
                end
            else
                table.insert(list,v)
            end
        end
    end
    -- 筛选完后排序
    table.sort(list, function(a_data, b_data)
        local a_index = a_data:Color() * 10000 - a_data:SuitType() * 1000 - a_data:AccrssoryType() * 100
        local b_index = b_data:Color() * 10000 - b_data:SuitType() * 1000 - b_data:AccrssoryType() * 100
        return a_index > b_index
    end)
    return list
end

-- 获取背包里所有的铸灵
function BagData:GetBagSpiritList()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local list = {}
    for _,v in pairs(col_grids) do
        if Item.GetBigType(v.item_id) == ITEM_BIG_TYPE.EQUIP_SPIRIT then
            table.insert(list,v)
        end
    end
    return list
end

-- 获取背包里所有的灵骑装备
function BagData:GetBagBicycleEquipList()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local list = {}
    for _,v in pairs(col_grids) do
        if Item.GetShowType(v.item_id) == ShowType.BicycleEquip then
            table.insert(list, v)
        end
    end
    return list
end

-- 获取背包里所有的灵骑圣物
function BagData:GetBagBicycleHalidomList()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local list = {}
    for _,v in pairs(col_grids) do
        if Item.GetShowType(v.item_id) == ShowType.Halidom then
            table.insert(list, v)
        end
    end
    return list
end

function BagData:GetBagMarbasGemList()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local list = {}
    for _,v in pairs(col_grids) do
        if v:IsMarbasGem() then
            table.insert(list,v)
        end
    end
    return list
end

function BagData:GetFuYuBagList()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local list = {}
    for _,v in pairs(col_grids) do
        if v:Type() == ItemType.FuYu then
            table.insert(list,v)
        end
    end
    return list
end

-- 判断两个装备是否是同一个
function BagData:JudgeEquipIsEqual(equip1, equip2)
    if equip1 == nil or equip2 == nil then return false end
    if equip1.item_unique_id_1 > 0 and equip2.item_unique_id_1 > 0 and equip1.item_unique_id_2 > 0 and equip2.item_unique_id_2 > 0 then
        return (equip1.item_unique_id_1 == equip2.item_unique_id_1) and (equip1.item_unique_id_2 == equip2.item_unique_id_2)
    end
    return false
end

--设置装备列表
function BagData:SetEquipList(list)
    local item_list = {}
    for k, v in pairs(list) do
        item_list[k] = Equip.New(v)
        item_list[k].in_bag = false
    end
    self.equipment_list:Set(item_list)
    self:CheckIsBetter(BETTER_CHECK_REASON.EQUIP_CHANGE)
    -- self:CheckBetterEqiup(self.item_grid[ItemColumnType.Item])
    self.is_complete_equip = true
end

function BagData:GetEquipList(index)
    if index then
        return self.equipment_list[index]
    end
    return self.equipment_list
end
--是否存在10或以下装备
function BagData:IsHasLowEquip()
    local list = self:GetEquipList()
    for k,v in pairs(list) do
        if v:EquipLevel() <= 10 then
            return true
        end
    end
    return false
end

function BagData:ChangeEquipList(equip)
    if equip.item_id == 0 then
        self.equipment_list[equip.index] = nil
    else
        equip = Equip.New(equip)
        equip.in_bag = false
        self.equipment_list[equip.index] = equip      
    end
    self:CheckIsBetter(BETTER_CHECK_REASON.EQUIP_CHANGE)
    -- self:CheckBetterEqiup(self.item_grid[ItemColumnType.Item])

    WorkshopData.Instance:ClearUpgradeRemindIgnore()  
end

function BagData:SetEquipVocLockList(protocol)
    self.equipment_voc_list.equipment_list = protocol.m_equipment_list
    self.equipment_voc_list.jewelry_list = protocol.m_jewelry_list
end

function BagData:SetEquipSortUpgrade(protocol)
    self.equip_sort_upgrade.list = protocol.equip_jewelry_upgreade
    self.equip_sort_upgrade.str_list = protocol.equip_jewelry_strengthen
end

function BagData:GetEquipSortUpgrade(type)
    if self.equip_sort_upgrade.list == nil then 
        return type == nil and {} or 0
    end 
    
    if type == nil then
        return self.equip_sort_upgrade.list
    else
        return self.equip_sort_upgrade.list[type]
    end 
end

function BagData:GetEquipSortStrengthen(type)
    if self.equip_sort_upgrade.str_list == nil then 
        return type == nil and {} or 0
    end 
    
    if type == nil then
        return self.equip_sort_upgrade.str_list
    else
        return self.equip_sort_upgrade.str_list[type]
    end 
end

function BagData:GetMedalItemList()
    local list = {}
    local medal_list = self:ViewData().medal_info
    for k,v in pairs(medal_list) do
        if v.item_id ~= 0 then
            local medal = Item.Create(v)
            medal.grid_index = v.grid_index
            table.insert(list, medal)
        end
    end
    return list
end

--一些区分职业的道具，返回转换的物品id，也可以自己传prof_id进来
function BagData.GetNewItemId(item_id, prof_id)
    local item_cfg = Cfg.RewardJobBagByIdType(item_id, prof_id or RoleData.Instance:ProfType(), false)
    if not TableIsEmpty(item_cfg) then
        return item_cfg.new_id
    end
    return item_id
end

--===================================礼包相关配置接口===================================
--是否是礼包的物品id
function BagData.IsHasGift(gift_id)
    return Config.gift_auto[gift_id] ~= nil or Config.gift_2_auto[gift_id] ~= nil
end

--获取礼包配置
function BagData.GetGiftConfig(gift_id)
    return Config.gift_auto[gift_id] or Config.gift_2_auto[gift_id]
end

--获取礼包配置
function BagData:GetGiftItemList(gift_id)
    local config = BagData.GetGiftConfig(gift_id)
    if config ~= nil then
        local cfg = config--DataHelper.TableCopy(config)
        local data = {select_num = cfg.select_num}
        -- LogError("item--",data,gift_id)
        data.item_list = {}
        for i = 1, cfg.item_num do
            data.item_list[i] = {}
            data.item_list[i].item_id = cfg["item_" .. i .. "_id"]
            data.item_list[i].num = cfg["item_" .. i .. "_num"]
            data.item_list[i].is_bind = cfg["is_bind_" .. i]
            data.item_list[i].param = self:CatchMedalParam(cfg["item_" .. i .. "_id"])
        end
        return data
    end
    return nil
end

-- 快速打包物品信息
function BagData:ItemBuild(item_data) 
    if Item.GetBigType(item_data.item_id) == ITEM_BIG_TYPE.MEDAL then 
        return self:BuildMedalInfo(item_data)
    elseif Item.GetBigType(item_data.item_id) == ITEM_BIG_TYPE.FU_YU then
        return self:BuildFuyuInfo(item_data)
    else 
        return Item.Create(item_data)
    end 
end 

-- 快速打包物品列表信息
function BagData:ItemListBuild(item_list)
    local list = {}
    for k,v in pairs(item_list or {}) do 
        table.insert(list,self:ItemBuild(v))
    end 

    return list
end
-- 对符玉进行参数内容进行拓展
function BagData:BuildFuyuInfo(item_data)
    if item_data.vo and item_data.vo.has_param then return item_data end
    local belong_cfg = Cfg.ParnterFuYuBelong(item_data.item_id)
    if belong_cfg == nil then
        return Item.Create(item_data)
    end
    local param = {}
    param.sp_id = belong_cfg.sp_id or 0
	-- t.has_net_value = MsgAdapter.ReadInt()
	param.expired_timesstamp = 0
    local attr_cfg = Cfg.ParnterFuYuByLevel2(15)
	-- t.sp_id = MsgAdapter.ReadInt()
	param.attr_num = attr_cfg.attri_num
	param.attr_list = {}
	for i = 1, 3, 1 do
		param.attr_list[i] = {
			attr_type = attr_cfg["attri_tpye"..i],
			attr_value = attr_cfg["value_"..i],
		}
	end
    local vo = item_data
    vo.has_param = true
    vo.param = param
	return Item.Create(vo)
end

-- 可以对持有百分百特效的令牌进行构造补完
function BagData:BuildMedalInfo(item_data)
    -- 持有物品param的不进行补完
    if item_data.vo and item_data.vo.has_param then return Item.Create(item_data) end  
    local param = self:CatchMedalParam(item_data.item_id)
    -- 无法匹配param的不进行补完
    if param == nil or param.sp_id == 0 then return Item.Create(item_data) end 

    local vo = item_data 
    vo.has_param = true
    vo.param = param
    return Item.Create(vo)
end 

-- 可以对百分百特效的令牌进行param追加
function BagData:CatchMedalParam(item_id)
    if not Item.GetBigType(item_id) then 
        return
    end 
    local config = Item.GetConfig(item_id)
    local check_sp = MedalData.Instance:CheckSpGroup(config.sp_group)
    if check_sp == nil then 
        return nil 
    else
        local special_cfg = self:GetEquipEffectCfg(check_sp)
        return {sp_id= check_sp,grade = special_cfg.sp_rank}
    end 
    
end 

--获取礼包信息 解析礼包1、2 返回item_list列表 和 config
function BagData:GiftItemList(item_id)
    local list = {}
    local config = self:GetGiftItemList(item_id)
    if config == nil then
        --尝试获取礼包随机物品
        config = BagData.RandItemList(item_id)
    end
    for i=1,#config.item_list do
        local param = config.item_list[i]
        param.mark_index = i - 1
        param.item_id = BagData.GetNewItemId(param.item_id) --转换一些需要转换的item_id
        table.insert(list,self:ItemBuild(param))
    end
    list = self:SortGiftItemList(list)
    return list,config
end

function BagData:SortGiftItemList(list)
    local result = list
    if #list > 0 and list[1].IsMedal ~=nil and list[1]:IsMedal() then 
        local role_id = RoleData.Instance:ProfType()
        table.sort(result,function(a,b)
            local a_key = a:EffectCfg().role_id == role_id and 200 or 0
            local b_key = b:EffectCfg().role_id == role_id and 200 or 0
            a_key = a_key + (a:EffectCfg().role_id == 0 and 100 or a:EffectCfg().role_id)
            b_key = b_key + (b:EffectCfg().role_id == 0 and 100 or b:EffectCfg().role_id)
            return b_key < a_key 
        end)
    end 
    return result
end

--是否是随机礼包 礼包类型 16 rand_num 主动消耗表 显示类型 21 + 使用类型 2
function BagData.IsRandGift(gift_id)
    local config = BagData.GetGiftConfig(gift_id)
    if config ~= nil then
        return config.rand_num and config.rand_num ~= 0
    else
        return Item.GetShowType(gift_id) == 21 and Item.GetUseType(gift_id) == 2
    end
    return false
end

--开到随机道具是不是珍稀道具 必须配合下发随机道具协议包
function BagData.IsSurprise(gift_id,item_id)
    local box_type = Item.GetConfig(gift_id).param2 or 0
    for k,v in pairs(Config.gift_randitem_auto.randitem) do
        if v.box_type == box_type and v.reward_item.item_id == item_id then
            return v.is_surprise == 1
        end
    end
    return false
end
function BagData.RandItemList(item_id)
    local config = {item_list = {}, select_num = 1, is_expense = true}
    if Item.GetConfig(item_id) == nil then 
        LogError("Warning this is a Nil Item",item_id)
        return config 
    end 

    local box_type = Item.GetConfig(item_id).param2 or 0
    for k,v in ipairs(Config.gift_randitem_auto.randitem) do
        if v.box_type == box_type then
            table.insert(config.item_list, DataHelper.TableCopy(v.reward_item))
        end
    end
    return config
end

--是否是自选礼包 礼包类型 16 select_num
function BagData.IsSelectGift(gift_id)
    local config = BagData.GetGiftConfig(gift_id)
    if config ~= nil then
        return config.select_num and config.select_num ~= 0
    else
        config = Item.GetConfig(gift_id)
        if config ~= nil and config.use_type and config.use_type == 2 then
            return config.param3 and config.param3 ~= 0
        end
    end
    return false
end

function BagData.GetGiftSelectNum(gift_id)
    local config = BagData.GetGiftConfig(gift_id)
    if config ~= nil then
        return config.select_num or 0
    else
        config = Item.GetConfig(gift_id)
        if config ~= nil and config.use_type and config.use_type == 2 then
            return config.param3 or 0
        end
    end
    return 0
end

--是否有礼包弹窗 礼包类型 16
function BagData.IsGiftPopup(gift_id)
    local config = BagData.GetGiftConfig(gift_id)
    if config ~= nil then
        return config.is_congratulations == 1
    end
    return false
end

--礼包弹窗使用
function BagData:GiftShowUse(item)
    if Item.IsQuick(item.item_id) then
        if RoleLevel() >= Item.GetLimitLv(item.item_id) then
            MainOtherCtrl.Instance:OpenKeyUseView(item)
        end
    end
end

-- 灵骑激活道具打开快速使用
function BagData:LingQiOpenFastUse(item)
    if ShowType.BicycleAct == item:ShowType() then
        if RoleLevel() >= Item.GetLimitLv(item.item_id) then
            MainOtherCtrl.Instance:OpenKeyUseView(item)
        end
    end
end

-- 灵骑激活道具使用
function BagData:LingQiFastUse(item)
    if not BicycleData.Instance:BicycleIsActiveByItemId(item.item_id) then
        ViewMgr:OpenViewByKey(Mod.Bicycle.Main)
    else
        BicycleCtrl.Instance:Req(BicycleCfg.OP_TYPE.USE_ACT_ITEM, item:GridColumn(), item:GridIndex())
    end
end

function BagData:CheckWithScore(item_id)
    return self.equip_score_config[item_id] ~= nil and not Equip.CanBlazeStar(item_id)
end

--获取物品类型组合的列表
function BagData:GetGridByType(types)
    local col_grids = self.item_grid[ItemColumnType.Item]
    local new_list = {}
    for k, v in pairs(col_grids) do
        for k2, v2 in pairs(types) do
            if v.item_type == v2 then
                table.insert(new_list, v)
            end
        end
    end
    return new_list
end

function BagData:GetGridByBigType(big_type)
    local col_grids = self.item_grid[ItemColumnType.Item]
    local new_list = {}
    for _, v in pairs(col_grids) do
        if Item.GetBigType(v.item_id) == big_type then
            table.insert(new_list, v)
        end
    end
    return new_list
end

--熔炼专用
function BagData:GetGridByEquipType(equip_type, limit_level)
    local col_grids = self.item_grid[ItemColumnType.Item]
    local new_list = {}
    for _, v in pairs(col_grids) do
        if v:IsEquip() and v:EquipType() == equip_type then
            local limit_prof = Equip.GetLimitProf(v.item_id)
            if limit_prof == RoleData.Instance:GetRoleProfType() and v.param.inc_num ~= 0 then
                if v:LimitLv() <= limit_level and not WorkshopData.Instance:CheckIsAllLock(v) then
                    if AnotherData.Instance:CheckEquipmentUniqueId(v,true) then 
                        table.insert(new_list, v)
                    end 
                end
            end
        end
    end

    return new_list
end

--熔炼专用 还要判断是否是无属性
function BagData:GetGridByJewelryType(jewelry_type, limit_level)
    local col_grids = self.item_grid[ItemColumnType.Item]
    local new_list = {}
    for _, v in pairs(col_grids) do
        if v:IsJewelry() and v:JewelryType() == jewelry_type then
            --[[ local limit_prof = Equip.GetLimitProf(v.item_id)
            if limit_prof == RoleData.Instance:GetRoleProfType() then ]]
            if v:LimitLv() <= limit_level and v.param.inc_num ~= 0 then
                if AnotherData.Instance:CheckEquipmentUniqueId(v,false) then 
                    table.insert(new_list, v)
                end 
            end
        --[[ end ]]
        end
    end
    return new_list
end

-- 通过唯一id搜索对应物品
function BagData:SearchEquipByUniqueId(equip)
    if equip == nil then return nil end 
    local col_grids = self.item_grid[ItemColumnType.Item]
    for _,v in pairs(col_grids) do
        local mark_1 = (v.item_unique_id_1 and v.item_unique_id_1 > 0) and v.item_unique_id_1 or v.vo.item_unique_id_1
        local mark_2 = (v.item_unique_id_2 and v.item_unique_id_2 > 0) and v.item_unique_id_2 or v.vo.item_unique_id_2
        local equip_1 = (equip.item_unique_id_1 and equip.item_unique_id_1 > 0) and equip.item_unique_id_1 or equip.vo.item_unique_id_1
        local equip_2 = (equip.item_unique_id_2 and equip.item_unique_id_2 > 0) and equip.item_unique_id_2 or equip.vo.item_unique_id_2

        if mark_1 == equip_1 and mark_2 == equip_2 then
            return v
        end 
    end 
end

-- 检查装备位置和背包判断是不是自己的装备
function BagData:CheckSameEquipByUniqueIdForAllIn(equip)
    if equip == nil then return false end 
    if equip.item_unique_id_1 == nil or equip.vo.item_unique_id_1 == nil or equip.item_unique_id_2 == nil or  equip.vo.item_unique_id_2 == nil then 
        return false
    end 

    local item = equip:IsEquip() and BagData.Instance:GetEquipList(equip:EquipType()) or BagData.Instance:GetJewelryList(equip:EquipType()-5)
    if item ~= nil and self:CheckSameEquipByUniqueId(item,equip) then return true end 
    local check_item = BagData.Instance:SearchEquipByUniqueId(equip)
    return check_item ~= nil
end

function BagData:CheckSameEquipByUniqueId(equip1,equip2)
    local mark_1 = (equip1.item_unique_id_1 and equip1.item_unique_id_1 > 0) and equip1.item_unique_id_1 or equip1.vo.item_unique_id_1
    local mark_2 = (equip1.item_unique_id_2 and equip1.item_unique_id_2 > 0) and equip1.item_unique_id_2 or equip1.vo.item_unique_id_2
    local mark2_1 = (equip2.item_unique_id_1 and equip2.item_unique_id_1 > 0) and equip2.item_unique_id_1 or equip2.vo.item_unique_id_1
    local mark2_2 = (equip2.item_unique_id_2 and equip2.item_unique_id_2 > 0) and equip2.item_unique_id_2 or equip2.vo.item_unique_id_2

    return mark_1 == mark2_1 and mark_2 == mark2_2
end

function BagData:GetGridForMedal()
    local limit_level = RoleData.Instance:GetRoleLevel()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local new_list = {}
    if self.medal_quick_item and not self.medal_quick_item:WithSp() then 
        for _, v in pairs(col_grids) do
            if self.medal_quick_item.index == v.index then
                table.insert(new_list, v)
                break
            end 
        end 
    end 

    local flag = self.medal_quick_item ~= nil 

    for _, v in pairs(col_grids) do
        if v:IsMedal() then
            if v:LimitLv() <= limit_level and not v:WithSp() then
                if flag and self.medal_quick_item.index ~= v.index then
                    table.insert(new_list, v)
                elseif flag == false then 
                    table.insert(new_list, v)
                end 
            end
        end 
    end 

    return new_list
end

function BagData:SetMedalQuickItem(data)
    self.medal_quick_item = data
end


function BagData:SetJewelryList(list)
    local item_list = {}
    for k, v in pairs(list) do
        item_list[k] = Jewelry.New(v)
        item_list[k].in_bag = false
    end
    self.jewelry_list:Set(item_list)
    self:CheckIsBetter(BETTER_CHECK_REASON.JEWERLY_CHANGE)
    -- self:CheckBetterEqiup(self.item_grid[ItemColumnType.Item])
end

function BagData:ChangeJewelryList(jewelry)
    if jewelry.item_id == 0 then
        self.jewelry_list[jewelry.jewelry_type] = nil
    else
        jewelry = Jewelry.New(jewelry)
        jewelry.in_bag = false
        self.jewelry_list[jewelry:JewelryType()] = jewelry
    end
    self:CheckIsBetter(BETTER_CHECK_REASON.JEWERLY_CHANGE)
    -- self:CheckBetterEqiup(self.item_grid[ItemColumnType.Item])

    WorkshopData.Instance:ClearUpgradeRemindIgnore()
end

function BagData:GetJewelryList(mtype)
    if mtype then
        return self.jewelry_list[mtype]
    end
    return self.jewelry_list
end

-- 统合获取·装备和灵饰
function BagData:GetEquipByType(mtype)
    if mtype == nil then return nil end 
    if mtype < 5 then 
        return self.equipment_list[mtype]
    else
        return self.jewelry_list[mtype - 5]
    end 
end
 
function BagData:SetMedalInfo(protocol)
    local list = {}

    for k,v in pairs(protocol.medal_list) do 
        list[v.grid_index + 1] = v
    end 

    for i = 1,3 do 
        if list[i] == nil then 
            list[i] = {}
            list[i].item_id = 0
            list[i].grid_index = i - 1
        else
            list[i].in_bag = false
        end 
    end 
    self.view_data.medal_info = list
    -- self.view_data.medal_info[1] = protocol.medal_list

    self:CheckIsBetter(BAGITEM_CHANGEREASON.MEDAL_CHANGE)
    -- self:CheckBetterEqiup(self.item_grid[ItemColumnType.Item])
end

function BagData:SetMedalTakeOff(protocol)
    self.medal_take_off_str = protocol.out_column.."|".. protocol.out_index
    self.view_data.medal_take_off = not self.view_data.medal_take_off
end

--获取解锁配置
function BagData:GetUnLockNumConfig(columnType)
    return self.bag_unlock_cfg[columnType]
end

--返回特效配置
function BagData:GetEquipEffectDesc(sp_id, origion)
    -- local info = self.special_effect_auto.spe_list[sp_id]
    local info = nil
    for k,v in pairs(self.special_effect_auto.spe_list) do 
        if v.sp_id == sp_id then 
            info = v
        end 
    end 
    
    if info then
        if origion == true then
            return info.description,info.icon_id,info.name
        else
            return info.reward_description,info.icon_id,info.reward_name
        end
    end
    return ""
end

function BagData:GetEquipEffectCfg(sp_id)
    return self.special_spe_list[tonumber(sp_id)]
end

function BagData:GetMedalEffectRankList(sp_type)
    return self.medal_effect_rank_list[sp_type]
end

function BagData:SetMedalChange(item)
    self.medal_change_item = item
end

function BagData:TreasureMapItem(item)
    local next_item = nil
    if item then
        local col_grids = self.item_grid[ItemColumnType.Item]
        for _, v in pairs(col_grids) do
            if v:IsTreasure() and item:IsSuperMap() == v:IsSuperMap() then
                next_item = v
                break
            end
        end
    end
    return next_item
end

--获取筛选的背包数据 用于交易 1道具 2宠物
function BagData:GetScreenData()
    local list = {}
    for col, item in pairs(self.item_list or {}) do --类型
        if col ~= ItemColumnType.Storage and col ~= ItemColumnType.Temporary and col ~= ItemColumnType.Task then
            for k, v in pairs(item) do
                if v:IsEquip() then
                    -- if v.param.is_treasure == 1 then
                        if not v.is_bind and v:CanTrade() then --非绑定
                            table.insert(list, v)
                        end
                    -- end
                else
                    if not v.is_bind and v:CanTrade() then --非绑定
                        table.insert(list, v)
                    end
                end
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

--筛选可以出售的道具
function BagData:GetScreenSellData()
    local list = {}
    for col, item in pairs(self.item_list or {}) do --类型
        if col ~= ItemColumnType.Storage and col ~= ItemColumnType.Temporary and col ~= ItemColumnType.Task then
            for k, v in pairs(item) do
                if v:Price() ~= 0 and v:Recommend() and not MarketData.Instance:IsMarketSell(v) then
                    table.insert(list, v)
                end
            end
        end
    end
    if #list < 24 then
        for i = #list + 1, 24 do
            list[i] = {item_id = 0}
        end
    end
    return list
end

-- 检查临时背包是否是空
function BagData:CheckTempBagEmpty()
    local col_grids = self.item_grid[ItemColumnType.Temporary]
    for _, v in pairs(col_grids) do
        if v.item_id > 0 then
            return true
        end
    end
    return false
end

-- 检查临时背包是否是空
function BagData:CheckTempBagEmptyNum(num)
    local empty_num = 0

    local col_grids = self.item_grid[ItemColumnType.Temporary]
    for _, v in pairs(col_grids) do
        if v.item_id == 0 and v.num == 0 then
            empty_num = empty_num + 1
        end
    end
    return empty_num >= num
end

function BagData:GetMedalEmpty()
    local pos = 2
    local is_full = true
    for k,v in pairs(self.view_data.medal_info) do
        if v.item_id == 0 then 
            pos = v.grid_index 
            is_full = false
            break
        end 
    end 
    return pos,is_full
end

function BagData:GetMedalSpList(page,is_showbag)
    if page == nil then return {} end 

    if not is_showbag then return self.medal_sp_list[page] end 

    local new_list = {}
    for k,v in pairs(self.medal_sp_list[page]) do 
        if Item.GetNum(v.item_id) > 0 then 
            table.insert( new_list, v )
        end 
    end 
    
    return new_list
end

function BagData:SetMedalSpTip(data)
    self.medal_sp_tip.sp_data = data
end

function BagData:SetMedalSpTipSort(param_t)
    self.medal_sp_tip.send_param_t = param_t
end

function BagData:GetMedalSpTipSort()
    return self.medal_sp_tip.send_param_t
end

function BagData:GetMedalSpTip()
    return self.medal_sp_tip
end

----------背包变化监听 --------------
function BagData:Care(func)
    return self.item_grid:Care(func)
end

function BagData:UnCare(ht)
    self.item_grid:Uncare(ht)
end

--============装备快捷使用===============
function BagData:EquipShowUse(item)
    if not self.is_complete_equip then
        return
    end
    if not (item:IsEquip() or item:IsJewelry()) then
        return
    end
    local guide_id = GuideData.Instance:GetBgaEquipId()
    if guide_id ~= 0 and guide_id == item.item_id then
        return
    end
    --策划说限制等级先暂时10级及以内才弹
    if item:LimitLv() > RoleData.Instance:GetRoleLevel() 
        or item:LimitLv() > Config.function_guide_auto.pet_skill[1].auto_equipment then
        return
    end
    local limit_prof = Equip.GetLimitProf(item.item_id)
    if limit_prof ~= -1 and limit_prof ~= RoleData.Instance:GetRoleProfType() then
        return
    end
    -- 指引使用的装备id直接终止自动穿戴
    if Config.function_guide_auto.pet_skill[1].equipment_id == item.item_id then 
        return 
    end 
    local compare = item:IsEquip() and self:GetEquipList(Equip.GetEquipType(item.item_id)) or self:GetJewelryList(Equip.GetEquipType(item.item_id) - 5)
    if compare ~= nil then
        if Equip.GetEquipLevel(item.item_id) > compare:EquipLevel() then
            MainOtherCtrl.Instance:OpenKeyUseView(item)
        end
    else
        MainOtherCtrl.Instance:OpenKeyUseView(item)
    end
end

function  BagData:CrystalEquipShowUse(item)
    if item:Type() ~= ItemType.CrystalEquip then
        return
    end
    MainOtherCtrl.Instance:OpenKeyUseView(item)
end

-- 内部调用
function BagData:ItemBetterCheck(item_grid,mark_param,is_another)
    local vo_mark = {}
    if item_grid:IsEquip() or item_grid:IsJewelry() then 
        local limit_prof = RoleData.Instance:GetRoleProfType()
        local compare = mark_param.is_mark and mark_param.mark_item or (is_another and item_grid:AnotherCompare() or item_grid:Compare())

        local flag_nomal = self.equip_score_config[item_grid.item_id] ~= nil

        local attr_item_value = 0
        local attr_compare_value = 0


        local item_prof = item_grid:LimitProf()
        for i,j in pairs(item_grid:UltimateAttrList()) do 
            local rate_s = item_prof == limit_prof and self:GetRoleScoreRate()[j.attr_type] or 0
            attr_item_value = rate_s * (j.attr_value + j.ultimate_value or 0) + attr_item_value
        end 

        -- local _,inc_list = item_grid:IncAttr()
        -- for i,j in pairs(inc_list) do 
        --     if j.attr_value > 0 then  
        --         local rate_s = item_prof == limit_prof and ROLE_SCORE_RATE[RoleData.ProfIdToProfType(limit_prof)][j.attr_type] or 0
        --         attr_item_value = rate_s * j.attr_value  + attr_item_value
        --     end 
        -- end 

        if compare and compare.param and compare.LimitProf then 
            local compare_prof = compare:LimitProf()
            for i,j in pairs(compare:UltimateAttrList()) do 
                local rate_s = compare_prof == limit_prof and self:GetRoleScoreRate()[j.attr_type] or 0
                attr_compare_value = rate_s * (j.attr_value + j.ultimate_value) + attr_compare_value
            end 

            -- local _,inc_list = compare:IncAttr()
            -- for i,j in pairs(inc_list) do 
            --     if j.attr_value > 0 then  
            --         local rate_s = compare_prof == limit_prof and ROLE_SCORE_RATE[RoleData.ProfIdToProfType(limit_prof)][j.attr_type] or 0
            --         attr_compare_value = rate_s * j.attr_value  + attr_compare_value
            --     end 
            -- end 
        end 
        -- if compare then 
        --     LogError("评分比较",item_grid:Name(),attr_item_value,compare:Name(),attr_compare_value)
        -- end 
        if attr_item_value > attr_compare_value and not item_grid.is_new 
            and (item_grid:Level() <= RoleData.Instance:GetRoleLevel() 
            or (item_grid:Config().without_level_limit ~= nil and item_grid:Config().without_level_limit > 0)) then 
            vo_mark.is_medal = false
            vo_mark.is_equip = item_grid:IsEquip()
            vo_mark.pos = item_grid.index
            vo_mark.equip_type = item_grid:IsEquip() and item_grid:EquipType() or (item_grid:JewelryType() + 5)
            vo_mark.value = attr_item_value
        end 

    elseif item_grid:IsMedal() then  
        local is_full = true
        for k,v in pairs(self.view_data.medal_info) do 
            if v.item_id == 0 then 
                is_full = false
                break
            end 
        end 

        if not is_full and item_grid:Level() <= RoleData.Instance:GetRoleLevel()
           and not item_grid.is_new  then 
            vo_mark.is_medal = true
            vo_mark.pos = item_grid.index
        end 
    end 

    return vo_mark
end

function BagData:CheckIsBetter(flush_reason,bag_index)
    -- LogError("CheckIsBetter!!!",flush_reason,bag_index)    
    local result_marks = Split(self.better_equip.mark_str,"|")

    -- 快速检查·降低循环次数
    local quick_item = bag_index ~= nil and self.item_grid[ItemColumnType.Item][bag_index] or nil
    if flush_reason == BETTER_CHECK_REASON.BAG_IN_CHANGE and quick_item ~= nil then 

        local mark_param = {}
        if quick_item:IsEquip() then 
            local quick_type = Equip.GetEquipType(quick_item.item_id) + 1
            local is_mark = result_marks[quick_type] ~= nil 
            local mark_item = is_mark and self.item_grid[ItemColumnType.Item][tonumber(result_marks[quick_type])] or nil
            mark_param.is_mark = is_mark
            mark_param.mark_item = mark_item
        elseif quick_item:IsJewelry() then 
            local quick_type = Jewelry.GetJewelryType(quick_item.item_id) + 6
            local is_mark = result_marks[quick_type] ~= nil 
            local mark_item = is_mark and self.item_grid[ItemColumnType.Item][tonumber(result_marks[quick_type])] or nil
            mark_param.is_mark = is_mark
            mark_param.mark_item = mark_item
        elseif quick_item:IsMedal() then 
            if quick_item.is_new then quick_item.is_better = false end 
        end 

        local better_mark = self:ItemBetterCheck(quick_item,mark_param)
        if better_mark.is_medal ~= nil and better_mark.is_medal == false and better_mark.value > 0 then 
            if tonumber(result_marks[better_mark.equip_type + 1]) > -1 then 
                self.item_grid[ItemColumnType.Item][tonumber(result_marks[better_mark.equip_type + 1])].is_better = false 
            end 
            self.item_grid[ItemColumnType.Item][better_mark.pos].is_better = true
            result_marks[better_mark.equip_type + 1] = better_mark.pos
        end 

        if not self.better_equip.is_better and better_mark.is_medal ~= nil then 
            self.better_equip.is_better = true
            BianQiangData.Instance:FlushListRemind(BianQiangData.ListType.ZhuangBeiKeTiHuang)
        end 

        local str_ = ""
		if result_marks then
			for k,v in pairs(result_marks) do 
				if tonumber(v) > -1 and not self.better_equip.is_better then 
                    self.better_equip.is_better = true
                    BianQiangData.Instance:FlushListRemind(BianQiangData.ListType.ZhuangBeiKeTiHuang)
                 end 
				str_ = k ==#result_marks and str_.. v or str_ .. v.."|"
			end 
		end
        self.better_equip.mark_str = str_

        if better_mark.is_medal ~= nil and better_mark.is_medal == true then 
            self.item_grid[ItemColumnType.Item][better_mark.pos].is_better = true
        end 

        return
    elseif flush_reason == BETTER_CHECK_REASON.BAG_OUT_CHANGE and quick_item ~=nil and quick_item.item_id == 0 then 
        return 
    elseif flush_reason == BETTER_CHECK_REASON.BAG_CHANGE and (quick_item == nil or (not quick_item.IsEquip and not quick_item.IsJewelry and not quick_item.IsMedal)) then 
        return 
    end 

    local total_marks = {}
    for k,v in pairs(self.item_grid[ItemColumnType.Item]) do
        if v.IsEquip == nil then
            LogDG(GetClassName(v.super))
        end
        if v:IsEquip() or v:IsJewelry() or v:IsMedal() then 
            if v.is_better then v.is_better = false end 
            local better_mark = self:ItemBetterCheck(v,{})
            if better_mark.is_medal ~= nil then 
                table.insert( total_marks,better_mark)
            end 
        end 
    end 

    local try_marks = {}
    if #total_marks == 0 then  
        self.better_equip.is_better = false 
        BianQiangData.Instance:FlushListRemind(BianQiangData.ListType.ZhuangBeiKeTiHuang) 
    end 

    for k,v in pairs(total_marks) do 
        if not self.better_equip.is_better then 
            self.better_equip.is_better = true 
            BianQiangData.Instance:FlushListRemind(BianQiangData.ListType.ZhuangBeiKeTiHuang)
        end 

        if not v.is_medal then 
            if try_marks[v.equip_type] == nil then 
                local vo = {index = v.pos , value =v.value }
                try_marks[v.equip_type] = vo
            elseif try_marks[v.equip_type].value < v.value then 
                local vo = {index = v.pos , value =v.value }
                try_marks[v.equip_type] = vo
            end 
        else 
            self.item_grid[ItemColumnType.Item][v.pos].is_better = true
        end 
    end 

    local str_ = ""
    for i = 0,7 do 
        local pos_ =  try_marks[i] == nil and -1 or try_marks[i].index
        str_ = i == 7 and str_..pos_ or str_ ..pos_.."|"

        if pos_ > -1 then 
            self.item_grid[ItemColumnType.Item][pos_ ].is_better = true
        end 
    end 
    self.better_equip.mark_str = str_

end 

function BagData:GetCurEquipScoreNum() 
	local prof_type = RoleData.Instance:ProfType()
    local rate_t = ROLE_SCORE_RATE[prof_type]
    local equip_score = 0

    local list = self:GetEquipList()

    for k,v in pairs(list) do 
        for i,j in pairs(v.param.attr_list) do 
            local rare_ = rate_t[j.attr_type] ~= nil and rate_t[j.attr_type] or 0
            equip_score = equip_score + rare_ * j.attr_value
        end 
    end 

    -- local medal_id = self:ViewData().medal_id
    -- if medal_id ~= 0 then 
    --     local medal = Item.Create({item_id = medal_id})
    --     local attr_list = medal:CfgAttr()
    --     for k,v in pairs(attr_list)do 
    --         local rare_ = rate_t[k] ~= nil and rate_t[k] or 0
    --         equip_score = equip_score + rare_ * v
    --     end 
    -- end 

	local score_level_cfg = Cfg.RoleScoreLevel(prof_type, RoleData.Instance:GetRoleLevel())
    local equip_level = 0
    if score_level_cfg ~= nil then
		if equip_score >= score_level_cfg.score_s then
			equip_level = 3
		elseif equip_score >= score_level_cfg.score_a then
			equip_level = 2
		elseif equip_score >= score_level_cfg.score_b then
			equip_level = 1
		end
    end
    
    return math.floor(equip_score), equip_level
end 

function BagData:SetNewItem(item_data)
    for k, v in pairs(self.new_item.items_data) do
        if item_data.column_type == v.column_type and v.index == item_data.index then
            return
        end
    end
    if not (item_data:IsEquip() or item_data:IsJewelry() or item_data:IsMedal()) then 
        table.insert(self.new_item.items_data, item_data)
        self.new_item.is_new = true
    end 
end

function BagData:ClearNewItem()
    if self.new_item.is_new then
        for k, v in pairs(self.new_item.items_data) do
            self.item_grid[v.column_type][v.index].is_new = nil
        end
    end

    self.new_item.items_data = {}
    self.new_item.is_new = false
end

-- 单独清除
function BagData:ClearOnesNewItem(item_data)
    if not item_data.index then
        return
    end
    if self.item_grid[item_data.column_type] and self.item_grid[item_data.column_type][item_data.index] then 
        self.item_grid[item_data.column_type][item_data.index].is_new = nil
    end 

    for k, v in pairs(self.new_item.items_data) do
        if v.column_type == item_data.column_type and v.index and v.index == item_data.index then
            table.remove(self.new_item.items_data, k)
            break
        end
    end

    if #self.new_item.items_data == 0 then
        self.new_item.is_new = false
    end
end

-- 单独清除某一个类的所有新物品
function BagData:ClearNewItemsByColType(col_type)
    if not col_type then
        return
    end

    -- 从新物品中清理
    local clear_list = {}
    for k, v in pairs(self.new_item.items_data) do
        if v.column_type == col_type then
            if v.index then
                self.item_grid[v.column_type][v.index].is_new = nil
            end
        else
            table.insert(clear_list, v)
        end
    end

    self.new_item.items_data = clear_list

    if #self.new_item.items_data == 0 then
        self.new_item.is_new = false
    end
end

function BagData:GetNewItemsByColType(col_type)
    if not col_type then
        return
    end

    local items = {}

    for k, v in pairs(self.new_item.items_data) do
        if v.column_type == col_type and v.index then
            table.insert(items, v)
        end
    end

    return items
end

function BagData:GetNewItemCols()
    local cols = {}
    for k, v in pairs(self.new_item.items_data) do
        cols[v.column_type] = v.column_type
    end
    return cols
end

function BagData:IsNewItem()
    return self.new_item.is_new or false
end

--根据类型和id返回在背包中的位置
function BagData:GetBagIndexById(col_type, item_id)
    for k, v in pairs(self.item_grid[col_type]) do
        if v.item_id == item_id then
            return k + 1
        end
    end
end

--是否稀有道具
function BagData.IsRare(item_id)
    for k,v in pairs(Config.sundries_auto.rare_item or {}) do
        if v.item == item_id then
            return true
        end
    end
    return false
end

function BagData.IsTreasures(item_id)
    for k,v in pairs(Config.sundries_auto.treasures_item or {}) do
        if v.item == item_id then
            return true
        end
    end
    return false
end

--判断装备是否低于玩家等级
function BagData:IsEquipLower()
    if self:GetEquipList(0) == nil then
        return false
    end
    local rolo_level = RoleData.Instance:GetRoleLevel()
    local limit_level = math.floor(rolo_level / 10) * 10
    for k,v in pairs(self:GetEquipList()) do
        if Config.equipment_auto[v.item_id] or 0 <= limit_level then
            return false
        end
    end
    return true
end

--是否有某个道具 并返回第一个
function BagData:IsHasItemById(item_id)
    for _, item in pairs(self.item_list[ItemColumnType.Item]) do
        if item.item_id == item_id then
            return item
        end
    end
end

--获取有多少个空位
function BagData:GetEmptyNumInBag(bag_type)
    local num = 0
    for k, v in pairs(self.item_grid[bag_type or ItemColumnType.Item]) do
        if v.item_id == 0 then
            num = num + 1
        end
    end
    return num
end
function BagData:GetCountInBag(bag_type)
    return table.nums(self.item_grid[bag_type or ItemColumnType.Item])
end

function BagData:GetItemGridByIndex(column,index)
    if self.item_grid[column] and self.item_grid[column][index] then
        return self.item_grid[column][index]
    end
    return nil
end

--根据下标获取这个格子的信息
function BagData:GetItemByIndex(column,index)
    if self.item_list[column] and self.item_list[column][index] then
        return self.item_list[column][index]
    end
    return nil
end

-- 获取背包中的水晶物品
function BagData:GetCrystalList()
    local crystal_list = self:GetCrystalGrid()
    local id = self:ViewData().element_crystal_id
    if id > 0 then
        local item = Item.Create({item_id = id,used_times = self:ViewData().used_times})
        item.index = -1
        -- item.param.used_times = self:ViewData().used_times
        table.insert(crystal_list, 1, item)
    end
    return crystal_list
end

-- 扩展令牌信息
function BagData:ChatItemShowPlus(item,show_id,show_id2)
    if item:IsMedal() then 
        local rank_cfg = self:GetEquipEffectCfg(show_id)
        if rank_cfg ~= nil then 
            item.param.grade = rank_cfg.sp_rank
            item.param.sp_id = show_id
            item.param.second_sp_id = show_id2 or 0

            item = Item.Create(item)
        elseif item.param.grade > 0 or item.param.sp_id > 0 then 

        else 
            item.param.grade = 0
            item.param.sp_id = show_id

            item = Item.Create(item)
        end 
    end 
end

function BagData:GetActItemConfig(item_id)
    return self.act_item_config[item_id]
end

function BagData:GetMedalList()
    return self.view_data.medal_info or {}
end

function BagData:GetMedalCompItem(special_type)
    for k,v in pairs(self:GetMedalList()) do
        if v.param and v.param.sp_id then
            if MedalData.GetSpecialType(v.param.sp_id) == special_type then
                return {vo = v}
            end
        end
    end
end

function BagData:GetEquipedMedalIsSpFail(index,list)
    if index == nil or list == nil then return false end 

    if list[index].item_id == 0 or list[index].item_id == nil then return false end 
    if list[index].param == nil or list[index].param.sp_id == nil or list[index].param.sp_id == 0 then return false end 

    for k,v in pairs(list) do 
        if k ~= index then 
            local cfg = (v.param and v.param.sp_id and v.param.sp_id > 0) and Cfg.MedalEffectById(v.param.sp_id) or nil
            local oper_cfg = Cfg.MedalEffectById(list[index].param.sp_id)

            if cfg ~= nil then 
                if cfg.sp_type == oper_cfg.sp_type and 
                    (cfg.sp_rank > oper_cfg.sp_rank or 
                    (cfg.sp_rank == oper_cfg.sp_rank and k < index)) then 
                        return true
                end 
            end 
        end 
    end 

    return false
end

function BagData:CheckItemPrice()
    local cfg_list = {
        Config.shop_cfg_auto.mourinho,      -- 商会物品配置
        Config.prestige_cfg_auto.shop,      -- 声望商店物品配置
    }
    for i,v in pairs(cfg_list) do
        for j,v2 in pairs(v) do
            if Item.GetPrice(v2.item_id) >= v2.price then
                LogError("物品" .. v2.item_id .. "出售价格大于等于售卖价格")
            end
        end
    end
    LogError("物品出售价格都正常")
end

-- 监听新增道具
function BagData:ListenItems(item_id_list,func)
    if item_id_list == nil or func == nil then
        return
    end
    local key = func
    local item_list_info = {}
    for i,v in pairs(item_id_list) do
        local info = {}
        table.insert(item_list_info,{item_id = v,num = Item.GetNum(v)})
    end
    self.recv_item_notify_list[func] = item_list_info
    return key
end

-- 移除监听新增道具
function BagData:RemoveListenItems(key)
    if key == nil then
        return
    end
    self.recv_item_notify_list[key] = nil
end
--self.recv_item_notify_list k:func v:item_list {item_id,old_num}[]
function BagData:NotifyItems()
    -- local item_func_map = {}    --key:item_id,value:func[]
    -- local item_num_map = {}     --key:item_id,value:num
    -- UnityProfiler.BeginSample("BagData:NotifyItems====11111111")
    -- for k,v in pairs(self.recv_item_notify_list) do --
    --     for i,item_info in pairs(v) do  --v {item_id,num}
    --         if item_func_map[item_info.item_id] == nil then
    --             item_func_map[item_info.item_id] = {}
    --         end
    --         item_num_map[item_info.item_id] = item_info.num
    --         table.insert(item_func_map[item_info.item_id],k)
    --     end
    -- end
    
    -- local check = function (item_id)
    --     if item_num_map[item_id] then
    --         -- UnityProfiler.BeginSample("BagData:NotifyItems====Item.GetNum")
    --         local new_item_num = Item.GetNum(item_id)
    --         -- UnityProfiler.EndSample()
    --         local old_item_num = item_num_map[item_id]
    --         if new_item_num ~= old_item_num then
    --             return new_item_num
    --         end
    --     end
    -- end
    -- UnityProfiler.EndSample()

    -- -- UnityProfiler.BeginSample("BagData:NotifyItems====22222")
    -- for item_id,v in pairs(item_func_map) do
    --     UnityProfiler.BeginSample("BagData:NotifyItems====check")
    --     local new_num = check(item_id)
    --     if new_num then
    --         for i,func in pairs(v) do
    --             func(item_id)
    --             for j,info in pairs(self.recv_item_notify_list[func]) do
    --                 if info.item_id == item_id then
    --                     info.num = new_num
    --                 end
    --             end
    --         end
    --     end
    --             UnityProfiler.EndSample()

    -- end


    local item_num_map = {}
    for func,items in pairs(self.recv_item_notify_list) do
        for _,it_info in pairs(items) do
            local it_id = it_info.item_id
            local now_num = item_num_map[it_id]
            if now_num == nil then
                now_num = Item.GetNum(it_id)
                item_num_map[it_id] = now_num
            end
            if now_num ~= it_info.num then
                func(it_id)
                it_info.num = now_num
            end
        end
    end

    -- UnityProfiler.EndSample()
end

--获取可一键升华物品
function BagData:GetMedalCanAllUpgrade()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local new_list = {}
    local types = {ItemType.Medal}
    for k, v in pairs(col_grids) do
        for k2, v2 in pairs(types) do
            if v.item_type == v2 and v.CanAllUpgrade and v:CanAllUpgrade() then
                table.insert(new_list, v)
            end
        end
    end
    -- if #new_list < 25 then 
    --     for i = #new_list + 1 , 25 do 
    --         table.insert(new_list,Item.Create({item_id = 0}))
    --     end 
    -- end 
    return new_list
end

-- 根据输入的标记数据返还带标记的全背包uint组
function BagData:GetBagMarkIntGroup(marks)
    local list = {}
    for i= 1,10 do
        table.insert(list, bit:d2b(0))
    end 

    for k,v in pairs(marks) do 
        -- 注意虽然int组是顺序但是具体的单个int内部是倒序
        list[math.floor(v/32) + 1][32 - math.floor(v%32)] = 1
    end 

    local int_list = {}
    for i = 1,10 do 
        table.insert(int_list,bit:b2d(list[i]))
    end 

    return int_list
end

--获取可分解的物品
function BagData:GetCanResolveItem()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local new_list = {}
    local types = {ItemType.Weapon, ItemType.Armor, ItemType.Jewelry}
    for k, v in pairs(col_grids) do
        for k2, v2 in pairs(types) do
            if v.item_type == v2 and v.CanResolve and v:CanResolve() then
                table.insert(new_list, v)
            end
        end
    end
    return new_list
end

function BagData:SetPreResolveItem(data)
    self.pre_resolve_item = data
end

function BagData:GetPreResolveItem()
    return self.pre_resolve_item
end

-- 这边获取对应职业的评分系数表
function BagData:GetRoleScoreRate()
    local prof_limit = RoleData.Instance:GetRoleProfType()
    if self.role_rate_s[prof_limit] ~= nil then 
        return self.role_rate_s[prof_limit]
    end 
    local operate = nil
    for k,v in pairs(Config.score_cfg_auto.scale_score) do 
        if v.prof == prof_limit then 
            operate = v
        end 
    end 
    local list = {}
    --operate由表中得来，部分值可能在元表中
    for k,v in pairs(AttrTypeForName) do
        if operate[k] ~= nil then
            list[v] = operate[k] 
        end
    end
    -- for k,v in pairs(operate) do 
    --     if AttrTypeForName[k]~= nil then 
    --         list[AttrTypeForName[k]] = v
    --     end 
    -- end 

    if self.role_rate_s[prof_limit] == nil then 
        self.role_rate_s[prof_limit] = list
    end 

    return list
end

-- 根据属性值和属性类型返回评分
function BagData:GetRoleScoreByAttr(attr_type, attr_value)
    local rate_t = self:GetRoleScoreRate() or {}
    local rare_ = rate_t[attr_type] ~= nil and rate_t[attr_type] or 0
    -- 特殊属性用该值*系数计算评分
    for k, v in pairs(AttrScoreSpecial) do
        if attr_type == k then
            return rare_ * attr_value
        end
    end
    -- 普通属性用该值*系数*4/100来计算评分
    return (rare_ * attr_value * 4) / 100
end

--部分活动会送灵玉，当客户端需要展示动效时屏蔽中间飘字
function BagData:SetHideCurrencyCenterFlag(flag)
    self.hide_currency_show_center_flag = flag
end

-- 部分抽卡系统会出现灵玉，故联合屏蔽之
function BagData:IsMarkedCenterVirtual()
    -- if ServerActivityData.Instance:GetMinecAnimMark() then 
    --     return false
    -- end 
    -- return true
    return not self.hide_currency_show_center_flag
end

-- 获取当前激活的点化技能
-- 若check_id有值，则对比是否相同（该check_id是否激活了
function BagData:GetNevealSkillActed(check_id,is_mine)
    local mark_list = {}
    local oper_list = (is_mine == nil or is_mine == true) and self.equipment_list or RoleInfoCtrl.Instance:Data():GetRoleEquips().equipment_list
    for k,v in pairs(oper_list) do 
        if v:IsNeveal() then 
            local info = v:Neveal()
            local cfg = WorkshopData.Instance:GetNevealCfg(info.effect_id,info.effect_level)
            if cfg ~= nil then 
                if mark_list[info.effect_id] == nil then 
                    mark_list[info.effect_id] = 1
                else 
                    mark_list[info.effect_id] = mark_list[info.effect_id] + 1 
                end 
            end
        end 
    end 

    local act_list = {}
    for k,v in pairs(mark_list) do 
        local eff_cfg = WorkshopData.Instance:GetNevealCfg(k)
        if eff_cfg ~= nil and eff_cfg.equipmen_num <= v then 
            table.insert(act_list,k)
        end 
    end 

    if check_id == nil then 
        return act_list
    else 
        for k,v in pairs(act_list) do 
            if v == check_id then 
                return true
            end 
        end 
        return false
    end 
end

-- 提取当前人物身上的总等级
function BagData:GetNevealSkillNum(check_id,is_mine)
    local mark_list = {}
    local oper_list = (is_mine == nil or is_mine == true) and self.equipment_list or RoleInfoCtrl.Instance:Data():GetRoleEquips().equipment_list
    for k,v in pairs(oper_list) do 
        if v:IsNeveal() then 
            local info = v:Neveal()
            local cfg = WorkshopData.Instance:GetNevealCfg(info.effect_id,info.effect_level)
            if cfg ~= nil then 
                if mark_list[info.effect_id] == nil then 
                    mark_list[info.effect_id] = 1
                else 
                    mark_list[info.effect_id] = mark_list[info.effect_id] + 1 
                end 
            end
        end 
    end 
    local eff_cfg = WorkshopData.Instance:GetNevealCfg(check_id)

    return mark_list[check_id] == nil and 0 or mark_list[check_id],eff_cfg.equipmen_num
end

-- 提取当前任务身上的总等级
function BagData:GetNevealSkillLevel(check_id,is_mine)
    if check_id == nil then return 0 end 
    local flag = self:GetNevealSkillActed(check_id,is_mine)
    if not flag then return 0 end 

    local level = 0
    local oper_list = (is_mine == nil or is_mine == true) and self.equipment_list or RoleInfoCtrl.Instance:Data():GetRoleEquips().equipment_list
    for k,v in pairs(oper_list) do 
        local info = v:Neveal()
        level = info.effect_level + level
    end 

    return level
end

-- 提取当前人物身上持有的技能等级，并返回数量最大的那个
function BagData:GetNevealSkillEnable()
    local mark_list = {}
    for k,v in pairs(self.equipment_list) do 
        if v:IsNeveal() then 
            local info = v:Neveal()
            local cfg = WorkshopData.Instance:GetNevealCfg(info.effect_id,info.effect_level)
            if cfg ~= nil then 
                if mark_list[info.effect_id] == nil then 
                    mark_list[info.effect_id] = 1
                else 
                    mark_list[info.effect_id] = mark_list[info.effect_id] + 1 
                end 
            end
        end 
    end 

    local maxer = nil
    for k,v in pairs(mark_list) do 
        if maxer == nil or maxer.num < v then 
            maxer = {
                effect_id = k,
                num = v
            }
        end 
    end 

    return maxer
end

--获取可分解的物品
function BagData:GetCanResolveNeveal()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local new_list = {}
    local types = {ItemType.Neveal}
    for k, v in pairs(col_grids) do
        for k2, v2 in pairs(types) do
            if v.item_type == v2 and v.param.has_param == 1 and v.param.effect_id > 0 then
                table.insert(new_list, v)
            end
        end
    end
    return new_list
end


function BagData:SetPreNevealResolveItem(data)
    self.pre_neveal_resolve_item = data
end

function BagData:GetPreNevealResolveItem()
    return self.pre_neveal_resolve_item
end

function BagData:GetConverEquipList()
    local col_grids = self.item_grid[ItemColumnType.Item]
    local tran_color = Config.compose_auto.equip_tran[1].tran_color
    local list = {}
    local equip_list = self:GetEquipList()
    for k, v in pairs(equip_list) do
        if v:Color() == tran_color then
            table.insert(list,v)
        end
    end
    for _,v in pairs(col_grids) do
        if Item.GetBigType(v.item_id) == ITEM_BIG_TYPE.EQUIPMENT
        and v:Color() == tran_color then
            table.insert(list,v)
        end
    end
    --灵饰也要丢进来
    local jewelry_list = self:GetJewelryList()
    for k, v in pairs(jewelry_list) do
        if v:Color() == tran_color then
            table.insert(list,v)
        end
    end
    for _,v in pairs(col_grids) do
        if Item.GetBigType(v.item_id) == ITEM_BIG_TYPE.JEWELRY
        and v:Color() == tran_color then
            table.insert(list,v)
        end
    end
    return list
end