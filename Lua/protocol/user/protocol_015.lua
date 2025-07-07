local function TreasureMapItemParam()
	return {
		is_set_rand = MsgAdapter.ReadShort(),
		is_active_map = MsgAdapter.ReadChar(),--仅限星尘宝图 0未激活 1已激活
		reserve_ch = MsgAdapter.ReadChar(),
		scene_id = MsgAdapter.ReadInt(),
		x = MsgAdapter.ReadInt(),
		y = MsgAdapter.ReadInt(),
		monster_id = MsgAdapter.ReadInt(),
		client_need_uida = MsgAdapter.ReadInt(),
		client_need_uidb = MsgAdapter.ReadInt(),
		is_rand_map = MsgAdapter.ReadInt(),
		param_type = MsgAdapter.ReadInt(),--0怪物 1铜币 2物品 3随机物品
		param1 = MsgAdapter.ReadInt(),
		param2 = MsgAdapter.ReadInt(),
		param3 = MsgAdapter.ReadInt(),
	}
end
local function ExpCrystalParam()
	return {
		remain_times = MsgAdapter.ReadShort(),
		is_turn_on = MsgAdapter.ReadShort()
	}
end
local function ElementCrystalParam()
	local t = {
		used_times = MsgAdapter.ReadShort(),
		reserve_sh = MsgAdapter.ReadShort()
	}
	return t
end
local function AttrInfo()
    local t = {
        attr_type = MsgAdapter.ReadShort(),
        attr_value = MsgAdapter.ReadShort()
    }
    return t
end

local function AttrVocInfo()
    local t = {
		lock = MsgAdapter.ReadShort(),
		empty = MsgAdapter.ReadShort(),
        attr_type = MsgAdapter.ReadShort(),
        attr_value = MsgAdapter.ReadShort()
    }
    return t
end

function MedalParam()
	local t = {}
	t.has_net_value = MsgAdapter.ReadShort()
	t.attr_num = MsgAdapter.ReadShort()
	t.sp_id = MsgAdapter.ReadInt()
	t.attr_list = {}
	for i = 1 ,MEDAL_ITEM_ATTR_NUM do 
		t.attr_list[i] = AttrInfo()
		t.attr_list[i].index = i
	end 
	t.is_move_data = MsgAdapter.ReadShort()
 	t.grade = MsgAdapter.ReadShort()
	
	t.special_medal_grade = MsgAdapter.ReadShort()
	t.second_sp_id = MsgAdapter.ReadShort() 
	return t
end

function EquipmentParam()
	local t = {}
	t.has_random = MsgAdapter.ReadChar()
	t.is_treasure = MsgAdapter.ReadChar() --0通常 1珍品
	t.attr_num = MsgAdapter.ReadShort()
	t.attr_list = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.attr_list[i] = AttrInfo()
		t.attr_list[i].index = i
	end
	t.stone = {}
	-- 这个宝石已经在下面扩展了新的，旧的不用了，但还是要读
    for i = 1, 2 do
        t.stone[i] = MsgAdapter.ReadUShort()
    end
	t.creater_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	t.sp_id = MsgAdapter.ReadInt()
	-- t.reserve_sh = MsgAdapter.ReadShort()
	t.inc_num = MsgAdapter.ReadInt()
	--[[ t.attr_extra_color = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.attr_extra_color[i] = MsgAdapter.ReadChar()
	end ]]
	t.attr_extra_inc = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.attr_extra_inc[i] = AttrVocInfo()
		t.attr_extra_inc[i].index = i
		t.attr_extra_inc[i].color = 1--t.attr_extra_color[i]
	end
	t.refresh_attr_extra_inc = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.refresh_attr_extra_inc[i] = AttrVocInfo()
		t.refresh_attr_extra_inc[i].index = i
		t.refresh_attr_extra_inc[i].color = 1--t.attr_extra_color[i]
	end
	t.ultimate_attr_list = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.ultimate_attr_list[i] = MsgAdapter.ReadShort()--类型与通用属性对应
	end
	t.suit_effect_id = MsgAdapter.ReadShort()
	t.is_strong_create = MsgAdapter.ReadShort()
	t.strengthen_level = MsgAdapter.ReadInt()
	-- 铸灵属性
	t.equip_spirit_info = {}
	for i = 1, EQUIP_SPIRIT_MAX do
		t.equip_spirit_info[i] = {}
		t.equip_spirit_info[i].slot = i - 1
		t.equip_spirit_info[i].item_id = MsgAdapter.ReadUShort()
		if t.equip_spirit_info[i].item_id > 0 then
			-- 这边多加一重判断，如果下发的这个道具id不是铸灵就置为0
			if Item.GetBigType(t.equip_spirit_info[i].item_id) ~= ITEM_BIG_TYPE.EQUIP_SPIRIT then
				t.equip_spirit_info[i].item_id = 0
			end
		end
		MsgAdapter.ReadUChar()
		-- 等级参数我们自己读表
		if t.equip_spirit_info[i].item_id ~= 0 then
			local config = Cfg.SpiritStoneConfigById(t.equip_spirit_info[i].item_id)
			t.equip_spirit_info[i].level = config and config.spirit_stone_level or 0
		else
			t.equip_spirit_info[i].level = 0
		end
		MsgAdapter.ReadChar()
	end

	-- 新的宝石，可扩展
	-- 宝石迁移标记
	MsgAdapter.ReadChar()
	t.suit_effect_change_flag = MsgAdapter.ReadChar()
	t.new_suit_effect_id = MsgAdapter.ReadShort()
	t.stone = {}
	for i = 1, EQUIP_STONE_COUNT_MAX do
        t.stone[i] = MsgAdapter.ReadUShort()
	end
	-- 宝石保留位，方便以后扩展
	for i = EQUIP_STONE_COUNT_MAX + 1, EQUIP_STONE_EXTEND_MAX do
		MsgAdapter.ReadUShort()
	end
	t.cur_effect = {}
	t.cur_effect.effect_id = MsgAdapter.ReadInt()
	t.cur_effect.effect_level = MsgAdapter.ReadInt()
	
	t.refresh_effect = {}
	t.refresh_effect.effect_id = MsgAdapter.ReadInt()
	t.refresh_effect.effect_level = MsgAdapter.ReadInt()
	t.star_level = MsgAdapter.ReadInt()
	--点化迁移标记
	MsgAdapter.ReadChar()
	t.cur_dianhua_plan = MsgAdapter.ReadChar()--当前点化方案
	t.unlock_plan = MsgAdapter.ShortFlag(0)--点化方案解锁 bit[0-4)
	t.dian_hua_param = {}
	for i = 0, EQUIP_DIAN_HUA_SLOT_NUM - 1, 1 do
		t.dian_hua_param[i] = {}
		t.dian_hua_param[i].index = i
		t.dian_hua_param[i].effect_id = MsgAdapter.ReadInt()--现有
		t.dian_hua_param[i].effect_level = MsgAdapter.ReadInt()
		t.dian_hua_param[i].refresh_effect_id = MsgAdapter.ReadInt()--未保存
		t.dian_hua_param[i].refresh_effect_level = MsgAdapter.ReadInt()
	end
	-- t.cur_effect = {}
	t.cur_effect.effect_id = t.dian_hua_param[t.cur_dianhua_plan].effect_id
	t.cur_effect.effect_level = t.dian_hua_param[t.cur_dianhua_plan].effect_level

	-- t.refresh_effect = {}
	t.refresh_effect.effect_id = t.dian_hua_param[t.cur_dianhua_plan].refresh_effect_id
	t.refresh_effect.effect_level = t.dian_hua_param[t.cur_dianhua_plan].refresh_effect_level
	return t
end
--物品变化信息中的宠物结构体，与宠物系统中的类似，但不一样
local function PetParam()
	local t = {}
	t.pet_id = MsgAdapter.ReadInt()
	t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	t.str_level = MsgAdapter.ReadUShort() -- 强化等级
	t.level = MsgAdapter.ReadUShort() -- 等级
	t.exp = MsgAdapter.ReadLL()
	t.add_exp = MsgAdapter.ReadLL()--战斗结束宠物增加的经验
	t.hp = MsgAdapter.ReadInt()
	t.mp = MsgAdapter.ReadInt()
	-- 技能列表 PET_SKILL_MAX_COUNT=10 技能等级*10000+技能id
	--改版后skill_list只有一个主动技能故写死读一个
	t.skill_list = {
		[1] = MsgAdapter.ReadInt()
	}
	t.exclusive_gift_skill = {
		[1] = MsgAdapter.ReadInt()
	}

	-- 当前的天赋列表 PET_GIFT_MAX_COUNT=8
	--旧字段废弃后空读
	for i = 1, 8 do
        MsgAdapter.ReadInt()
    end
    for i = 1, 8 do
        MsgAdapter.ReadInt()
    end
    --旧字段废弃后空读

	-- 资质
	t.con_growth = MsgAdapter.ReadUShort()
	t.str_growth = MsgAdapter.ReadUShort()
	t.def_growth = MsgAdapter.ReadUShort()
	t.agi_growth = MsgAdapter.ReadUShort()
	t.magic_growth = MsgAdapter.ReadUShort()
	-- 洗练出来资质
	t.con_growth_new = MsgAdapter.ReadUShort()
	t.str_growth_new = MsgAdapter.ReadUShort()
	t.def_growth_new = MsgAdapter.ReadUShort()
	t.agi_growth_new = MsgAdapter.ReadUShort()
	t.magic_growth_new = MsgAdapter.ReadUShort()
	-- 领悟技能个数
	t.comprehend_gift_num = MsgAdapter.ReadInt()
	t.comprehend_gift_value = MsgAdapter.ReadInt()
	--0为默认没有上锁  1 为上锁了
	t.lock = MsgAdapter.ReadUChar()
	t.refined_flag = MsgAdapter.ReadUChar()
	-- 已分配属性点
	t.con = MsgAdapter.ReadUShort()
	t.str = MsgAdapter.ReadUShort()
	t.def = MsgAdapter.ReadUShort()
	t.agi = MsgAdapter.ReadUShort()
	t.magic = MsgAdapter.ReadUShort()
	--最大血量与蓝量
	t.max_hp = MsgAdapter.ReadInt()
	t.max_mp = MsgAdapter.ReadInt()

	t.pet_auto_move_type = MsgAdapter.ReadShort()
	t.pet_auto_skill_id = MsgAdapter.ReadShort()
	t.pet_auto_skill_level = MsgAdapter.ReadShort()
	t.guide_pet_flag = MsgAdapter.ReadChar()
	t.reserve_sh2 = MsgAdapter.ReadChar()

	t.item_unique_id = MsgAdapter.ReadLL()
	t.pet_equipment_id = MsgAdapter.ReadUShort()
	t.equipment_skill_num = MsgAdapter.ReadUShort()
	t.equipment_skill_list = {}
	for i=1, MAX_PET_EQUIPMENT_SKILL_NUM do
		t.equipment_skill_list[i] = MsgAdapter.ReadShort()
	end
	t.equipment_skill_refresh_list = {}
	for i=1, MAX_PET_EQUIPMENT_SKILL_NUM do
        t.equipment_skill_refresh_list[i] = MsgAdapter.ReadShort()
	end
	t.skin_id = MsgAdapter.ReadInt()

    t.is_move_data = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()

	--当前宠物的天赋技能列表
    t.gift_list = {}
    for i = 1, PET_GIFT_MAX_COUNT do
        t.gift_list[i] = t.gift_list[i] or {}
        --天赋技能id
        t.gift_list[i].gift_id = MsgAdapter.ReadUShort()
        --天赋来源（0：洗炼出来的自带的，1：开领悟格子送的）
        t.gift_list[i].gift_source = MsgAdapter.ReadUChar()
        --保留字段，空读
        t.gift_list[i].skill_level = MsgAdapter.ReadUChar() + 1
    end

    --未保存的洗炼的结果,只洗资质的话这里的gift_id都是0
    t.new_gift_list = {}
    for i = 1, PET_GIFT_MAX_COUNT do
        t.new_gift_list[i] = MsgAdapter.ReadUShort()
        --洗炼结果的giftlist不需要考虑来源问题，除gift_id之外的直接空读掉
        MsgAdapter.ReadShort()
    end

    --打书替换
    t.replace_skill_id = MsgAdapter.ReadInt()
    t.replace_index = MsgAdapter.ReadUChar() + 1
    t.replace_type = MsgAdapter.ReadUChar()--0普通 1炼妖

    t.fei_sheng_times = MsgAdapter.ReadUChar()      --飞升次数
    t.feisheng_jie_duan = MsgAdapter.ReadUChar()    --当前飞升任务的进度
    
    --魂器1
    t.weapon_info = PetWeapon.DecodeInfo()
    t.weapon_info.param.pet_index = t.index
    t.weapon_info.param.use_pet_id = t.pet_id
    t.weapon_info.weapon_idx = 0
    --LogError("宠物身上的魂器信息：", t.weapon_info)

    --神印方案
    t.stamp_blueprint_info = PetStampBlueprint.DecodeInfo(true)
    t.fei_sheng_select_skill_id = MsgAdapter.ReadInt()
    t.fei_sheng_select_skill_level = MsgAdapter.ReadInt() + 1
    --LogError("宠物神印信息",t.stamp_blueprint_info)

    --宠物徽章
    t.bagde_item_param = PetBagde.DecodeItemParam()
    if not TableIsEmpty(t.bagde_item_param) then
        t.bagde_item_param.pet_index = t.index
    end
    --LogError("宠物徽章：",t.bagde_item_param)

    --魂器2
    t.weapon_info2 = PetWeapon.DecodeInfo()
    t.weapon_info2.param.pet_index = t.index
    t.weapon_info2.param.use_pet_id = t.pet_id
    t.weapon_info2.weapon_idx = 1
    --LogError("宠物身上的魂器2信息：", t.weapon_info2)

    --炼妖信息
    t.lian_yao_pet_id = MsgAdapter.ReadInt()
    t.lian_yao_pet_str_lv = MsgAdapter.ReadUShort()
    t.lian_yao_reserved_sh = MsgAdapter.ReadUShort()
    t.lian_yao_passive_list = {}
    for i = 1, PET_LIAN_YAO_SKILL_MAX do
        t.lian_yao_passive_list[i] = PortocolCommon.PetPassiveItemInfo(i)
	end

	local inborn_decode_func = function (  )
		local t = {}
		for i = 1, MAX_PET_INBORN_GROUP_NUM do
			local info = {}
			info.index = i  -- 天赋下标
			info.inborn_grid = MsgAdapter.ReadUChar()       --天赋位置
			info.inborn_level = MsgAdapter.ReadUChar()      --天赋等级
			MsgAdapter.ReadShort()
			table.insert(t, info)
		end
		return t
	end
	
	t.inborn_info_list = inborn_decode_func()

	t.pet_gai_ming_info = PortocolCommon.GaiMingInfo()
	
	return t
end
function NewJewelryParam()
	local t = {}
	t.has_random = MsgAdapter.ReadChar()
	t.is_treasure = MsgAdapter.ReadChar()
	t.attr_num = MsgAdapter.ReadShort()
	t.attr_list = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.attr_list[i] = AttrInfo()
		t.attr_list[i].index = i
	end
	t.creater_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	t.inc_num = MsgAdapter.ReadInt()
	--[[ t.attr_extra_color = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.attr_extra_color[i] = MsgAdapter.ReadChar()
	end ]]
	t.attr_extra_inc = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.attr_extra_inc[i] =  AttrVocInfo()
		t.attr_extra_inc[i].index = i
		t.attr_extra_inc[i].color = 1--t.attr_extra_color[i]
	end
	t.refresh_attr_extra_inc = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.refresh_attr_extra_inc[i] =  AttrVocInfo()
		t.refresh_attr_extra_inc[i].index = i
		t.refresh_attr_extra_inc[i].color = 1--t.attr_extra_color[i]
	end
	t.stone = {}
	-- 这个宝石已经在下面扩展了新的，旧的不用了，但还是要读
	for i = 1, 2 do
        t.stone[i] = MsgAdapter.ReadUShort()
	end
	t.ultimate_attr_list = {}
	for i = 1, EQUIP_ITEM_ATTR_NUM do
		t.ultimate_attr_list[i] = MsgAdapter.ReadShort()--类型与通用属性对应
	end
	t.suit_effect_id = MsgAdapter.ReadShort()
	t.is_strong_create = MsgAdapter.ReadShort()
	t.strengthen_level = MsgAdapter.ReadInt()
	-- 铸灵属性
	t.equip_spirit_info = {}
	for i = 1, EQUIP_SPIRIT_MAX do
		t.equip_spirit_info[i] = {}
		t.equip_spirit_info[i].slot = i - 1
		t.equip_spirit_info[i].item_id = MsgAdapter.ReadUShort()
		if t.equip_spirit_info[i].item_id > 0 then
			-- 这边多加一重判断，如果下发的这个道具id不是铸灵就置为0
			if Item.GetBigType(t.equip_spirit_info[i].item_id) ~= ITEM_BIG_TYPE.EQUIP_SPIRIT then
				t.equip_spirit_info[i].item_id = 0
			end
		end
		MsgAdapter.ReadUChar()
		-- 等级参数我们自己读表
		if t.equip_spirit_info[i].item_id ~= 0 then
			local config = Cfg.SpiritStoneConfigById(t.equip_spirit_info[i].item_id)
			t.equip_spirit_info[i].level = config and config.spirit_stone_level or 0
		else
			t.equip_spirit_info[i].level = 0
		end
		MsgAdapter.ReadChar()
	end
	-- 新的宝石，可扩展
	t.stone = {}
	-- 宝石迁移标记
	MsgAdapter.ReadChar()
	t.suit_effect_change_flag = MsgAdapter.ReadChar()
	t.new_suit_effect_id = MsgAdapter.ReadShort()
	t.stone = {}
	for i = 1, EQUIP_STONE_COUNT_MAX do
        t.stone[i] = MsgAdapter.ReadUShort()
	end
	-- 宝石保留位，方便以后扩展
	for i = EQUIP_STONE_COUNT_MAX + 1, EQUIP_STONE_EXTEND_MAX do
		MsgAdapter.ReadUShort()
	end
	t.star_level = MsgAdapter.ReadInt()
	return t
end
local function JewelryParam()
	local t = {}
	t.lyrics_id = MsgAdapter.ReadShort()
	MsgAdapter.ReadShort()
	t.anima_val = MsgAdapter.ReadInt() --灵气值
	t.attr_pair = AttrInfo()
	t.attr_pair2 = AttrInfo()--基础属性加成2
	t.lege_attr = AttrInfo()
	t.creater_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	return t
end

local function PartnerEquipParam()
	local t = {}
	t.has_net_value = MsgAdapter.ReadInt()
	-- t.attr_type = MsgAdapter.ReadInt()
	-- t.attr_value = MsgAdapter.ReadInt()
	t.attr_list = {}
	for i = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
		t.attr_list[i] = {
			attr_type = MsgAdapter.ReadInt(),
			attr_value = MsgAdapter.ReadInt(),
		}
	end
	return t
end

local function PetEquipParam()
	local t={}
	t.has_random_net_value=MsgAdapter.ReadChar()
	t.reserve_ch=MsgAdapter.ReadChar()
	t.reserve_sh=MsgAdapter.ReadShort()
	t.pet_equipment_id = MsgAdapter.ReadUShort()
    t.equipment_skill_num = MsgAdapter.ReadUShort()
	t.equipment_skill_list = {}
	t.equipment_skill_refresh_list={}
    for i=1, MAX_PET_EQUIPMENT_SKILL_NUM do
        t.equipment_skill_list[i] = MsgAdapter.ReadShort()
	end
	for i=1, MAX_PET_EQUIPMENT_SKILL_NUM do
        t.equipment_skill_refresh_list[i] = MsgAdapter.ReadShort()
	end
	return t
end

local function PetWeaponItemParam()
	local t = {}
	t.has_random_net_value = MsgAdapter.ReadChar()
	t.is_lock = MsgAdapter.ReadChar()
	t.sp_id = MsgAdapter.ReadShort()

	t.exp = MsgAdapter.ReadInt()
	t.max_exp = MsgAdapter.ReadInt()
	t.refine = MsgAdapter.ReadInt()
	t.level = MsgAdapter.ReadShort()
	t.step = MsgAdapter.ReadShort()

	t.pet_equip = MsgAdapter.ReadUShort() -- 无用
	t.attri_num = MsgAdapter.ReadUShort()
	t.attri_list = {}
	for i = 1, 5 do
		local attri = {}
		attri.attri_type = MsgAdapter.ReadShort()
		attri.attri_value = MsgAdapter.ReadShort()
		if attri.attri_value > 0 then
			t.attri_list[attri.attri_type] = attri
		end
	end
	return t
end

local function PetStampItemParam()
	local t = {}
	t.has_random_net_value = MsgAdapter.ReadChar()
	MsgAdapter.ReadChar()
	MsgAdapter.ReadShort()
	t.stamp_info = PetStamp.DecodeInfo()
	return t
end

local function DianhuashiItemParam()
	local t = {}
	t.has_param = MsgAdapter.ReadInt()      --是否有净值（不管）
	t.effect_id = MsgAdapter.ReadInt()      
	t.effect_level = MsgAdapter.ReadInt()    
	return t
end

local function PeakTokenParam()
	local t = {}
	t.has_param = MsgAdapter.ReadInt()      --是否有净值（不管）
	t.used_times = MsgAdapter.ReadInt()    
	return t
end
local function FuYuParam()
	local t = {}
	t.has_net_value = MsgAdapter.ReadInt()
	t.expired_timesstamp = MsgAdapter.ReadUInt()
	t.sp_id = MsgAdapter.ReadInt()
	t.attr_num = MsgAdapter.ReadInt()
	t.attr_list = {}
	for i = 1, 3, 1 do
		t.attr_list[i] = {
			attr_type = MsgAdapter.ReadShort(),
			attr_value = MsgAdapter.ReadShort(),
		}
	end
	t.extra_sp_id = MsgAdapter.ReadInt()
	return t
end

function ItemDataWrapper(tab,is_not_unique)
	tab.item_id = MsgAdapter.ReadUShort()
	tab.column = MsgAdapter.ReadShort()
	tab.index = MsgAdapter.ReadShort()
	local get_reason = MsgAdapter.ReadChar() --获得原因
	tab.num = MsgAdapter.ReadInt()
	tab.is_bind = MsgAdapter.ReadChar() ~= 0
	tab.has_param = MsgAdapter.ReadChar() ~= 0
	tab.invalid_time = MsgAdapter.ReadUInt()
	tab.item_unique_id_1 = MsgAdapter.ReadUInt()  
    tab.item_unique_id_2 = MsgAdapter.ReadUInt()
	--有数据残留具体原因未知
	tab.param = nil--TempCode
	if tab.has_param then
		local param_length = MsgAdapter.ReadShort()
		tab.param = ItemDataParam(tab.item_id,param_length)
	end
end

--ItemDataParam 的读取
function ItemDataParam(item_id,param_length)
	local big_type = Item.GetBigType(item_id)
	if Item.GetBigType(item_id) == ITEM_BIG_TYPE.EQUIPMENT then
		return EquipmentParam()
	elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.MEDAL then 
		return MedalParam()
    elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.PET then
        return PetParam()
    elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.ELE_CRYSTAL then
        return ElementCrystalParam()
    elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.JEWELRY then
        return NewJewelryParam()
    elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.EXP_CRYSTAL then
        return ExpCrystalParam()
	elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.TREASURE_MAP then
		local t = TreasureMapItemParam()
		t.client_need_uid1 = t.client_need_uida + t.client_need_uidb
		t.client_need_uid = int64.new(t.client_need_uida, t.client_need_uidb)
		-- LogDG("宝图净值", t)
		return t 
	elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.PARTNER_EQUIPMENT then
		return PartnerEquipParam()
	elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.PET_EQUIPMENT then
		return PetEquipParam()
	elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.PET_WEAPON then
		return PetWeaponItemParam()
	elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.PET_STAMP then
		return PetStampItemParam()
	elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.ACCRSSORY then
		return Accrssory.BagParamDecode()
	elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.DIAN_HUA_SHI then
		return DianhuashiItemParam()
	elseif big_type == ITEM_BIG_TYPE.PET_BAGDE then
		return PetBagde.DecodeItemParam()
	elseif big_type == ITEM_BIG_TYPE.PEAK_TOKEN then
		return PeakTokenParam()
	elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.MARBAS_GEM then
		return YuanHunZhuParam()
	elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.SHENG_QI then
		return ShengQi.DecodeParam()
	elseif Item.GetBigType(item_id) == ITEM_BIG_TYPE.FU_YU then
		local t = FuYuParam()
		-- LogDG("符玉信息", t)
		return t
    else
        return MsgAdapter.ReadStrN(param_length or 256)
    end
end

--背包信息返回
SCKnapsackInfoAck = SCKnapsackInfoAck or BaseClass(BaseProtocolStruct)
function SCKnapsackInfoAck:__init()
	self.msg_type = 1500
	self.valid_grid_num_list = {}
	self.extend_times_list = {}
end

function SCKnapsackInfoAck:Decode()
	for i = ItemColumnType.Start, ItemColumnType.Count - 1 do
		self.valid_grid_num_list[i] = MsgAdapter.ReadShort()
	end
	for i = ItemColumnType.Start, ItemColumnType.Count - 1 do
		self.extend_times_list[i] = MsgAdapter.ReadShort()
	end
	self.reason = MsgAdapter.ReadShort()
	self.reason = self.reason == 1 and BAGITEM_CHANGEREASON.CLEARUP or self.reason
	self.item_count = MsgAdapter.ReadShort()
	self.item_list = {}
	for i = 1, self.item_count do
		local tab = {}
		self.item_list[i] = tab
		ItemDataWrapper(tab)
	end
end
-- 物品改变通知
SCKnapsackItemChange = SCKnapsackItemChange or BaseClass(BaseProtocolStruct)
function SCKnapsackItemChange:__init()
	self.msg_type = 1501
end

function SCKnapsackItemChange:Decode()
	self.reason = MsgAdapter.ReadShort()
	self.put_reason = MsgAdapter.ReadShort()
	ItemDataWrapper(self)
end
SCUseItemSuc = SCUseItemSuc or BaseClass(BaseProtocolStruct)
function SCUseItemSuc:__init()
	self.msg_type = 1504
end
function SCUseItemSuc:Decode()
	self.item_id = MsgAdapter.ReadUShort()
	self.num = MsgAdapter.ReadShort()
end
-- 背包扩充返回
SCKnapsackExtend = SCKnapsackExtend or BaseClass(BaseProtocolStruct)
function SCKnapsackExtend:__init()
	self.msg_type = 1505
end

function SCKnapsackExtend:Decode()
	self.column = MsgAdapter.ReadShort()
	self.grid_num = MsgAdapter.ReadShort()
	self.extend_times = MsgAdapter.ReadShort()
	MsgAdapter.ReadShort()
end
-- 物品缺乏
SCLackItem = SCLackItem or BaseClass(BaseProtocolStruct)
function SCLackItem:__init()
	self.msg_type = 1506
end
function SCLackItem:Decode()
	self.item_id = MsgAdapter.ReadInt()
	self.count = MsgAdapter.ReadInt()
end

local function ItemInfo()
	local t = {}
	t.item_id = MsgAdapter.ReadUShort()
	t.is_bind = MsgAdapter.ReadShort()
	t.num = MsgAdapter.ReadInt()
	MsgAdapter.ReadShort()
	t.has_param = MsgAdapter.ReadShort() ~= 0
	t.param = nil
	if t.has_param then
		local param_length = MsgAdapter.ReadUInt()
		t.param = ItemDataParam(t.item_id, param_length)
	end
	return t
end

SCItemIntoKnpackSpecialProtocal = SCItemIntoKnpackSpecialProtocal or BaseClass(BaseProtocolStruct)

function SCItemIntoKnpackSpecialProtocal:__init()
	self.msg_type = 1507
end

function SCItemIntoKnpackSpecialProtocal:Decode()
	self.get_reson = MsgAdapter.ReadShort()
	self.count = MsgAdapter.ReadShort()
	self.param1 = MsgAdapter.ReadInt()
	self.param2 = MsgAdapter.ReadInt()
	self.item_list = {}
	for i = 1, self.count do
		self.item_list[i] = ItemInfo()
	end
end

local function ResultItemInfo()
	local t = {}
	t.item_id = MsgAdapter.ReadUShort()
	t.is_bind = MsgAdapter.ReadShort()
	t.num = MsgAdapter.ReadInt()
	t.has_param = MsgAdapter.ReadShort() ~= 0
	t.param = nil
	if t.has_param then
		local param_length = MsgAdapter.ReadUInt()
		t.param = ItemDataParam(t.item_id, param_length)
	end
	return t
end

SCRAResultNotice = SCRAResultNotice or BaseClass(BaseProtocolStruct)

function SCRAResultNotice:__init()
	self.msg_type = 1508
end

function SCRAResultNotice:Decode()
	self.result_type = MsgAdapter.ReadShort()
	self.activity_type = MsgAdapter.ReadShort()
	self.param1 = MsgAdapter.ReadInt()
	self.param2 = MsgAdapter.ReadInt()
	self.count = MsgAdapter.ReadInt()
	self.item_list = {}
	for i = 1, self.count do
		self.item_list[i] = ResultItemInfo()
	end
end


-- 背包请求操作
CSKnapsackOperaReq = CSKnapsackOperaReq or BaseClass(BaseProtocolStruct)
function CSKnapsackOperaReq:__init()
	self.msg_type = 1550
end

function CSKnapsackOperaReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.oper_type)
	MsgAdapter.WriteInt(self.param_1)
	MsgAdapter.WriteInt(self.param_2)
	MsgAdapter.WriteInt(self.param_3)
	MsgAdapter.WriteInt(self.param_4)
end

-- 物品会用 使用类型不同需要传不同的参数
CSUseItem = CSUseItem or BaseClass(BaseProtocolStruct)
function CSUseItem:__init()
	self.msg_type = 1551
end
function CSUseItem:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.column)
	MsgAdapter.WriteShort(self.index)
	MsgAdapter.WriteShort(self.num)
	MsgAdapter.WriteShort(self.param1)
	MsgAdapter.WriteShort(self.param2)
	MsgAdapter.WriteShort(self.param3)
end

--自选礼包
CSUseOptionalGift = CSUseOptionalGift or BaseClass(BaseProtocolStruct)
function CSUseOptionalGift:__init()
	self.msg_type = 1552
	self.select_info = {}
end

function CSUseOptionalGift:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.column)
	MsgAdapter.WriteShort(self.index)
	MsgAdapter.WriteInt(self.num)
	for i=1,#self.select_info do
		MsgAdapter.WriteShort(self.select_info[i].index or 0)
		MsgAdapter.WriteShort(self.select_info[i].num or 0)
	end
end

SCTaskItemCommitNotice = SCTaskItemCommitNotice or BaseClass(BaseProtocolStruct)
function SCTaskItemCommitNotice:__init()
	self.msg_type = 1554
end
function SCTaskItemCommitNotice:Decode()
	self.count = MsgAdapter.ReadInt()
	self.item_list = {}
	for i = 1, self.count do
		self.item_list[i] = {
			item_id = MsgAdapter.ReadUShort(),
			num = MsgAdapter.ReadShort(),
		}
	end
end
CSUseOptionalGift2 = CSUseOptionalGift2 or BaseClass(BaseProtocolStruct)
function CSUseOptionalGift2:__init()
	self.msg_type = 1555
end
function CSUseOptionalGift2:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.column)
	MsgAdapter.WriteShort(self.index)
	MsgAdapter.WriteLL(self.select_index_flag)
end

SCTaskItemLackForCommitNotice = SCTaskItemLackForCommitNotice or BaseClass(BaseProtocolStruct)
function SCTaskItemLackForCommitNotice:__init()
	self.msg_type = 1553
end
function SCTaskItemLackForCommitNotice:Decode()
	self.count = MsgAdapter.ReadInt()
	self.item_list = {}
	for i = 1, self.count do
		self.item_list[i] = {
			item_id = MsgAdapter.ReadUShort(),
			num = MsgAdapter.ReadShort(),
		}
	end
end

-- 使用元宝恢复
CSUseGoldRecovery = CSUseGoldRecovery or BaseClass(BaseProtocolStruct)
function CSUseGoldRecovery:__init()
	self.msg_type = 1556
end
function CSUseGoldRecovery:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
	MsgAdapter.WriteInt(self.param3)
	MsgAdapter.WriteInt(self.param4)
end

SCKnapsackGetItemNotice = SCKnapsackGetItemNotice or BaseClass(BaseProtocolStruct)
function SCKnapsackGetItemNotice:__init()
	self.msg_type = 1558
end

function SCKnapsackGetItemNotice:Decode()
	self.notice_type = MsgAdapter.ReadInt()
	self.item_id = MsgAdapter.ReadUShort()
	self.num = MsgAdapter.ReadShort()
	self.p1 = MsgAdapter.ReadInt()
end

-- 使用需要特殊逻辑道具
SCUseSpecialLogicItem = SCUseSpecialLogicItem or BaseClass(BaseProtocolStruct)
function SCUseSpecialLogicItem:__init()
	self.msg_type = 1559
end

function SCUseSpecialLogicItem:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.notice_type)
	MsgAdapter.WriteUShort(self.item_id)
	MsgAdapter.WriteShort(self.num)
end

-- 激活需要特殊逻辑道具
SCActiveSpecialLogicItem = SCActiveSpecialLogicItem or BaseClass(BaseProtocolStruct)
function SCActiveSpecialLogicItem:__init()
	self.msg_type = 1560
end

function SCActiveSpecialLogicItem:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.notice_type)
	MsgAdapter.WriteShort(self.param1)
	MsgAdapter.WriteShort(self.param2)
end

--请求下发传闻 
SCHearsayInfoReq = SCHearsayInfoReq or BaseClass(BaseProtocolStruct)
function SCHearsayInfoReq:__init()
	self.msg_type = 1561
end

function SCHearsayInfoReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.notice_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
	MsgAdapter.WriteInt(self.param3)
end

-- 万能卡直购请求 仅供内网玩家使用
CSBuyCmdGift = CSBuyCmdGift or BaseClass(BaseProtocolStruct)
function CSBuyCmdGift:__init()
	self.msg_type = 1562
end

function CSBuyCmdGift:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.buy_type)
	MsgAdapter.WriteInt(self.p1)
	MsgAdapter.WriteInt(self.p2)
	if DEBUG_MODE then
		LOG("请求万能卡", self)
		local tip = ColorStr("使用了万能卡", COLORSTR.Yellow6)
		PublicPopupCtrl.Instance:Center(tip .. "（测试用，外网不会出现）")
	end
end

-- 红包使用奖励返回
SCHongBaoUseRet = SCHongBaoUseRet or BaseClass(BaseProtocolStruct)
function SCHongBaoUseRet:__init()
	self.msg_type = 1563
end

function SCHongBaoUseRet:Decode()
	local t = {
		count = MsgAdapter.ReadInt(),
		item_list = {},
	}
	for i = 1, t.count do
		t.item_list[i] = {
			money_type = MsgAdapter.ReadInt(),
			num = MsgAdapter.ReadInt(),
		}
	end
	self.info = t
end