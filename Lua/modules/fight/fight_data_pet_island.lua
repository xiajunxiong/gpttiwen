FIGHT_PET_ISLAND_OPER_TYPE = 
	{
		OPER_TYPE_INFO = 0,				--//!< 请求棋盘信息
		OPER_TYPE_REWARD_RECORD = 1,	--//!< 查看奖励
		OPER_TYPE_SET_EXPECT_PET = 2,	--//!< 设置祈福宠物 param1 宠物ID
		OPER_TYPE_CLEAR_EXPECT_PET = 3,	--//!< 清除祈福宠物 param1 是否金宠(1:是，0:否）
		OPER_TYPE_ROLL_DICE = 4,		--//!< 摇骰子
		OPER_TYPE_FETCH_GRID_REWARD = 5,--//!< 获取当前格子的奖励
		OPER_TYPE_START_PET_BATTLE = 6, --//!< 开始抓宠战斗
		OPER_TYPE_TURN_TABLE = 7,		--//!< 开始转盘
		OPER_TYPE_ENTER_NEXT_LEVEL = 8, --//!< 进入下一层
		OPER_TYPE_SHOW_TRUNTABLE_REWARDS = 9, --//!< 查看转盘奖励
		OPER_TYPE_TURN_TABLE_END = 10, --//!< 转盘动画结束
		OPER_TYPE_ADD_EXP = 11,			--加经验
		OPER_TYPE_SPECIAL_ITEM = 12, 	--//!< 使用特殊道具 param1:道具类型 param2:使用道具附加（鲁班骰点数/心愿卡选择的物品id）
		OPER_TYPE_SPECIAL_ITEM_NUM = 13, --//!< 获取全部特殊道具数量
		OPER_TYPE_FAST_MOVE_REWARD = 14, --//!< 使用神行符申请奖励
	}
FIGHT_PET_ISLAND_DICE_MODEL_ID = {
	[1] = 1010,
	[2] = 1011,
	[3] = 1012,
	[4] = 1013,
	[5] = 1014,
	[6] = 1015,
}

FIGHT_PET_MONOPOLY_GRID_TYPE =    --//!< 格子类型（也可以理解为格子的等级）
{
  PET_MONOPOLY_GRID_TYPE_STARTING_POINT = 0,  --//!< 起点
  PET_MONOPOLY_GRID_TYPE_ENDING_POINT = 1,  --//!< 终点
  PET_MONOPOLY_GRID_TYPE_NORMAL_GRID = 2,    --//!< 普通格子
  PET_MONOPOLY_GRID_TYPE_MEDIUM_GRID = 3,    --//!< 中级格子
  PET_MONOPOLY_GRID_TYPE_ADVANCE_GRID = 4,  --//!< 高级格子
  PET_MONOPOLY_GRID_TYPE_MAX = 5
}

FIGHT_PET_MONOPOLY_GRID_BG_NAME = {
	[FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_STARTING_POINT] = "KuaiHuang", --起点
	[FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_ENDING_POINT] = "KuaiZhongDian", --终点
	[FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_NORMAL_GRID] = "KuaiPu", --普通
	[FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_MEDIUM_GRID] = "KuaiZhong", --中级
	[FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_ADVANCE_GRID] = "KuaiGao", --高级
}

FIGHT_PET_MONOPOLY_GRID_REWARD =  --//!< 格子奖励类型
{
  PET_MONOPOLY_GRID_REWARD_NONE = 0,      --//!< 无奖励或奖励已被领取
  PET_MONOPOLY_GRID_REWARD_ITEM = 1,      --//!< 道具奖励
  PET_MONOPOLY_GRID_REWARD_PET_MONSTER = 2,  --//!< 宠物怪物组
  PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT = 3,  --//!< 宠物碎片
  PET_MONOPOLY_GRID_REWARD_TURNTABLE = 4,    --//!< 转盘奖励
  PET_MONOPOLY_GRID_REWARD_OPPORTUNITY= 5,		--//!< 机会点
  PET_MONOPOLY_GRID_REWARD_MAX = 6,
}
--道具枚举
PET_ISLAND_SPECIAL_ITEM_TYPE = {
	SP_ITEM_LUBANDICE = 0,		--//!< 鲁班骰
	SP_ITEM_DOUBLECARD = 1,		--//!< 双倍卡
	SP_ITEM_FASTMOVEPAPER = 2,	--//!< 神行符
	SP_ITEM_WISHCARD = 3,		--//!< 心愿卡
	SP_ITEM_PORTAL = 4,			--//!< 传送门
}

PET_MONOPOLY_SPECIAL_IMG_NAME = {
	[PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_LUBANDICE] = "_LocLuBanTou",
	[PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_FASTMOVEPAPER] = "_LocShenXingFu",
	[PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_PORTAL] = "_LocChuanSongMen",
}

function FightData:FightDataPetIsland()
	self.pet_prayed_id_list = {}
	self.pet_monopoly_smart = SmartData.New({
		cur_pray_id = 0,
		pray_info = false,
		show_change_level = false,
		flush_turn_reward = false,
		select_special_item_id = -1,	--特殊道具id
		special_param = -1,			--使用道具时需额外发送的参数
		use_special_item = false,
		double_left_times = 0,		--双倍奖励次数
		special_item_num = 0,
		show_get_special_item = false,
	})
	self.pet_monopoly_gird_info = SmartData.New()
	self.pet_monopoly_trigger_smart = SmartData.New{
		show_get_special_item = false,
	}
	self.pet_mo_sweep_cose = Config.pet_island_auto.other[1].cost
	--预处理表
	self.pet_pray_pool = {}
	for k,v in pairs(Config.pet_island_auto.monster_group) do
		self.pet_pray_pool[v.quality] = self.pet_pray_pool[v.quality] or {}
		self.pet_pray_pool[v.quality][v.id] = v
	end
	self.rare_item_list = {}
	for k,v in pairs(Config.pet_island_auto.reward_group) do
		if v.is_rare == 1 then
			self.rare_item_list[v.reward_item.item_id] = 1
		end
	end
end

function FightData:ResetPetMonopolyData()
	self.pet_monopoly_gird_info:Clear()
	self.pet_monopoly_smart:Set({
		cur_pray_id = 0,
		pray_info = false,
		show_change_level = false,
		flush_turn_reward = false,
		select_special_item_id = -1,	--特殊道具id
		special_param = -1,			--使用道具时需额外发送的参数
		use_special_item = false,
		double_left_times = 0,		--双倍奖励次数
		special_item_num = 0
	})
end

function FightData:SetPetMonopolyInfo(protocol)
	self:SetPetMonopolyPrayResult(protocol)
	self.pet_monopoly_info = protocol
	self.pet_monopoly_gird_info:Set(protocol.grid_list)
	if self.pet_monopoly_smart.cur_level ~= nil and self.pet_monopoly_smart.cur_level ~= protocol.cur_level then
		if not self.pet_monopoly_smart.show_change_level then
			self.pet_monopoly_smart.show_change_level = true
		else
			self.pet_monopoly_smart:SetDirty("show_change_level")
		end
	end
	self.pet_monopoly_smart.cur_level = protocol.cur_level
	self.pet_monopoly_smart.cur_grid_index = protocol.cur_grid_index + 1
end

function FightData:GetPetMonopolyGridInfo(index)
	if index then
		return self.pet_monopoly_gird_info[index]
	end
	return self.pet_monopoly_gird_info
end

function FightData:GetPetMoCurIndexData()
	return self:GetPetMonopolyGridInfo(self.pet_monopoly_smart.cur_grid_index)
end

function FightData:SetPetMonopolyPrayResult(protocol)
	self.pet_prayed_id_list[2] = protocol.expect_silver_pet_id
	self.pet_prayed_id_list[3] = protocol.expect_gold_pet_id
	self.pet_monopoly_smart:SetDirty("pray_info")
end

function FightData:GetPetPreyPool(quality)
	local show_pool = {}
	for k,v in pairs(self.pet_pray_pool[quality]) do
		local pet_cfg = PetData.Instance:GetPetCfg(v.id)
		if pet_cfg.mutant_times < 1 then
			table.insert(show_pool, v)
		end
	end
	return show_pool
end

function FightData:GetPetPreyId(quality)
	return self.pet_prayed_id_list[quality] or 0
end

function FightData:GetPetMoCurIndex()
	return self.pet_monopoly_smart.cur_grid_index
end

--丢骰子/鲁班骰
function FightData:SetPetMonopolyRollDice(protocol)
	if protocol.dice < 0 then
		self.pre_grid_index = self.pet_monopoly_smart.cur_grid_index
	else
		self.pre_grid_index = nil
	end
	self.pet_monopoly_smart.cur_grid_index = protocol.cur_grid_index + 1
	if self.pet_monopoly_smart.dice == protocol.dice then
		self.pet_monopoly_smart:SetDirty("dice")
	else
		self.pet_monopoly_smart.dice = protocol.dice
	end
end
--转盘奖励列表
function FightData:SetPetMonopolyTurntableInfo(protocol)
	self.turntable_reward_list = protocol.sector_list
	self.pet_monopoly_smart:SetDirty("flush_turn_reward")
end
--转盘结果
function FightData:SetPetMonopolyTurntableResult(protocol)
	self.pet_monopoly_smart.sector_idx = protocol.sector_idx + 1
end

function FightData:GetPetMoTurntableResult()
	return self.pet_monopoly_smart.sector_idx or 0
end

function FightData:GetPetMonopolyTurntableRewardList()
	return self.turntable_reward_list or {}
end

--格子信息变化
function FightData:SetPetMonopolyGridInfo(protocol)
	self.pet_monopoly_gird_info[protocol.cur_grid_index + 1] = protocol.grid_info
end

--使用心愿卡后格子内容的变化
function FightData:SetPetMonopolyWishCardGrid(protocol)
	self.changed_grid_info = protocol.changed_grid_info
	if self.pet_monopoly_smart.show_special_effect == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_WISHCARD then
		self.pet_monopoly_smart:SetDirty("show_special_effect")
	else
		self.pet_monopoly_smart.show_special_effect = PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_WISHCARD
	end
	for k, v in pairs(protocol.changed_grid_info) do
		self.pet_monopoly_gird_info[v.grid_index + 1] = v
	end
end
--进入下一层 之后会收到新的一层的整套信息
function FightData:SetPetMonopolyEnterNextLevel(protocol)
	self.pet_monopoly_next_level = protocol.next_level
end

--当前层已领取的奖励列表
function FightData:SetPetMonopolyRewardRecord(protocol)
	self.pet_monopoly_reward_list = protocol.reward_list
end

function FightData:GetPetMonopolyRewardList()
	return self.pet_monopoly_reward_list
end

function FightData.GetPetMoBlessRate(quality)
	for k,v in pairs(Config.pet_island_auto.bless_rate) do
		if v.quality == quality then
			return v.rate_up
		end
	end
end

function FightData:CheckPetMoCanOpenView()
	return PetData.Instance.new_pet == nil
end

function FightData:IsRareItemInPetMo(item_id)
	return self.rare_item_list[item_id] == 1
end

function FightData:GetPetMoExp()
	local role_level = RoleData.Instance:GetRoleLevel()
	return Config.crystal_scene_auto.exp[role_level].exp * Config.pet_island_auto.other[1].cost
	-- for k,v in pairs(Config.pet_island_auto.exp) do
	-- 	if role_level >= v.min_level and role_level <= v.max_level then
	-- 		return v.exp
	-- 	end
	-- end
end

function FightData:GetGoldPetShowRate()
	local role_level = RoleData.Instance:GetRoleLevel()
	for k,v in pairs(Config.pet_island_auto.rate_show) do
		if self.pet_monopoly_smart.cur_level >= v.min_layer and self.pet_monopoly_smart.cur_level <= v.max_layer then
			return v.rate_golden
		elseif k == #Config.pet_island_auto.rate_show and self.pet_monopoly_smart.cur_level >= v.max_layer then
			return v.rate_golden
		end
	end
end

function FightData:GetPetMonopolySpecialItemNum(special_item_id)
	special_item_id = special_item_id or self.pet_monopoly_smart.select_special_item_id
	return self.special_item_group and self.special_item_group[special_item_id] or 0
end

function FightData:GetPetMonopolyCurLayerWishList()
	return Config.pet_island_auto.wish_card
end

function FightData:SetPetMonopolySpecialItem(protocol)
	if self.special_item_group ~= nil then
		if self.pet_monopoly_smart.special_item_num < protocol.special_item_num then
			self.pet_monopoly_trigger_smart:SetDirty("show_get_special_item")
		end
	end
	self.pet_monopoly_smart.special_item_num = protocol.special_item_num
	self.pet_monopoly_smart.double_left_times = protocol.double_surplus
	self.special_item_group = protocol.special_item_group
	if protocol.use_item_type > 0 then
		if protocol.use_item_type == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_PORTAL then
			if self.pet_monopoly_smart.show_special_effect == protocol.use_item_type then
				self.pet_monopoly_smart:SetDirty("show_special_effect")
			else
				self.pet_monopoly_smart.show_special_effect = protocol.use_item_type
			end
		end
	end
end

function FightData.GetPetMonopolySpecialItemConfig(special_item_id)
	return Config.pet_island_auto.special_item[special_item_id + 1]
end