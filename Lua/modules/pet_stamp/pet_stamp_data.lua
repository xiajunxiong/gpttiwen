PetStampData = PetStampData or BaseClass()
function PetStampData:__init()
    if PetStampData.Instance ~= nil then
		Debuger.LogError("[PetStampData] attempt to create singleton twice!")
		return
	end
	PetStampData.Instance = self
	self:InitCfg()
	-- 背包全部神印信息 下标[1 -100]
	self.pet_stamp_list = SmartData.New({})

	-- 全部方案信息
	self.blueprint_list = SmartData.New({})

	-- 经验信息
	self.exp_info = SmartData.New({})

	-- 背包的筛选信息
	self.bag_sel_data = SmartData.New({pos_index = 0, show_type = 2, sort_type = 1, suit_id = 0})

	-- 强化放入的经验
	self.intensify_put_exp_data = SmartData.New({all_exp = 0, exp = 0, item_exp = 0, stuff_exp = 0})

	-- 刷新背包
	self.flush_bag_data = SmartData.New({value = false})


	-- 祈神数据
	self.pray_data = SmartData.New({})
	-- 祈神日志数据
	self.pray_log_data = SmartData.New({log_list = {}})
	-- 祈神兑换信息
	self.pray_convert_data = SmartData.New({chip_num = 0})
	-- 祈神位置
	self.pray_pos_index = 1
	-- 祈神分解设置
	self.pray_reslove_set_data = {}

	-- 神印吞噬
	self.swallow_smart = SmartData.New({co_index = -1, main_index = 0, attri_index = -1})
	-- 玩家有多少经验
	--self.bag_exp = 0
	-- 选择需要强化的神印
	self.intensify_pet_stamp = nil

	-- 套装新字标记
	self.suit_new_falg_data = {}
	-- 飘字提醒
	self.bag_change_notif = true

	-- 是否兑换中
	self.is_converting = false

	-- 是否执行祈神动画中
	self.is_pray_animation = false

	PetStampResolveData.New()
end

function PetStampData:InitCfg()
	-- 物品配置
	self.item_cfg = Config.pet_god_print_item_auto
	-- 神印配置
	self.stamp_cfg = Config.pet_god_print_auto
	-- 神印配置
	Cfg.Register("PetStampCfg",Cfg.RealTimeSearch2Key(self.stamp_cfg.pet_god_print_cfg, "seq", "quality"))
	-- 套装配置
	--Cfg.Register("PetStampSuitCfg",Cfg.RealTimeSearch2Key("seq", "suit_num"))
	-- 强化配置
	Cfg.Register("PetStampIntensifyCfg",Cfg.RealTimeSearch2Key(self.stamp_cfg.pet_god_intensify, "quality", "levels"))

	-- 祈神奖励
	Cfg.Register("PetStampPrayRewardCfg",Cfg.RealTimeSearch2Key(self.stamp_cfg.pray_item_group, "item_group", "item_seq"))

	-- 强化材料配置
	self.stamp_stuff_cfg = KeyList(self.stamp_cfg.pet_god_intensify_item, "intensify_item_id")
	-- 分解获得经验配置
	self.reslove_stamp_cfg = KeyList(self.stamp_cfg.resolve_exp, "seq")

	-- 强化加成组
	self.intensify_group = DataHelper.ConvertGroup(self.stamp_cfg.intensify_att, "quality")
	-- 套装组
	local suit_group = DataHelper.ConvertGroup(self.stamp_cfg.pet_god_suit, "pet_god_suit")
	self.suit_group = {}
	for i,v in pairs(suit_group) do
		self.suit_group[i] = DataHelper.ConvertGroup(v, "star_level")
	end
	-- 套装图鉴组
	self.suit_show_group = DataHelper.ConvertGroup(self.stamp_cfg.pet_god_show, "pet_god_suit")

	-- 分解设置
	for qua = 1, 4 do
		local flag = self:GetResloveRecord(qua) > 0 and true or false
		self:SetResloveData(qua, flag)
	end
end

--获取角色默认的套装品质
function PetStampData:DefaultSuitQua()
	local role_level = RoleData.Instance:GetRoleLevel()
	if role_level >= self.stamp_cfg.other[1].red_pet_god then
		return ItemColor.Red
	else
		return ItemColor.Orange
	end
end

function PetStampData:GetStampItemCfg(item_id)
	return self.item_cfg[item_id]
end

-- 刷新背包
function PetStampData:FlushBag()
	self.flush_bag_data.value = not self.flush_bag_data.value
end

-- 背包变化处理
function PetStampData:BagChangeEvent()
	local list = self:GetPetStampList()
	self.pet_stamp_count_data = {}
	self.sum_count = 0
	for i,v in pairs(list) do
		if v.item_id > 0 then
			if not self.pet_stamp_count_data[v.item_id] then
				self.pet_stamp_count_data[v.item_id] = 0
			end
			self.pet_stamp_count_data[v.item_id] = self.pet_stamp_count_data[v.item_id] + 1
			self.sum_count = self.sum_count + 1
		end
	end
end

-- 背包神印数量
function PetStampData:Count()
	return self.sum_count or 0
end

-- 背包提醒设置
function PetStampData:SetNotif(value)
	self.bag_change_notif = value
end

function PetStampData:GetNum(item_id)
	return self.pet_stamp_count_data[item_id] or 0
end

function PetStampData:SetPetStampList(pet_stamp_list)
	self.pet_stamp_list:Set(pet_stamp_list)
	self:BagChangeEvent()
end

function PetStampData:GetPetStampList()
	return self.pet_stamp_list
end

function PetStampData:SetPetStamp(index, pet_stamp,put_reason)
	if put_reason ~= PUT_REASON_TYPE.PUT_REASON_ENDLESS_TOWER then
		if self.bag_change_notif and (self.pet_stamp_list[index] == nil or self.pet_stamp_list[index].item_id <= 0) then
			if self.is_converting then
				ViewMgr:OpenView(PetStampShowView, {pet_stamp = pet_stamp})
				self.is_converting = false
			end
			PublicPopupCtrl.Instance:CenterI({item_id = pet_stamp.item_id , num = 1,color_str = pet_stamp:ColorStr()})
		end
	end
	self.pet_stamp_list[index] = pet_stamp
	self:BagChangeEvent()
end

function PetStampData:GetPetStampByIndex(index)
	return self.pet_stamp_list[index]
end

function PetStampData:SetBlueprintList(blueprint_list)
	self.blueprint_list:Set(blueprint_list)
end

function PetStampData:SetBlueprint(blueprint, index)
	self.blueprint_list[index] = blueprint
end

function PetStampData:GetBlueprintList()
	return self.blueprint_list
end

function PetStampData:GetBlueprintByIndex(index)
	return self.blueprint_list[index]
end

function PetStampData:SetExpInfo(info, is_first)
	local exp = self.exp_info.exp or 0
	if not is_first and info.exp > exp then
		if self:GetResloveData(0) and not ViewMgr:IsOpen(PetStampResolveSetView) then
			if self.auto_get_exp == nil then
				self.auto_get_exp = 0	-- 自动分解时产生的经验
			end
			self.auto_get_exp = self.auto_get_exp + (info.exp - exp)
		else
			PublicPopupCtrl.Instance:Center(string.format(Language.PetStamp.Tip3,info.exp - exp))
		end
	end
	self.exp_info:Set(info)
end

-- 玩家有多少经验
function PetStampData:GetExp()
	return self.exp_info.exp or 0
end

-- 获取当前方案
function PetStampData:GetCurBuleprint()
	return self.sel_buleprint
end

function PetStampData:SetCurBuleprint(buleprint)
	self.sel_buleprint = buleprint
end

-- 获取背包格子数据 筛选有套装类型、位置筛选、排序类型
function PetStampData:GetBagGridList(suit_id, pos_index, sort_type, is_pet)
	local min_count = 15
	local stamp_list = self:GetPetStampList()
	local list = {}
	local cur_ctrl_blueprint = self:GetCurBuleprint()
	for i,v in pairs(stamp_list) do
		if v.item_id > 0 then
			local is_insert = true
			if suit_id and suit_id > 0 and v:SuitId() ~= suit_id then
				is_insert = false
			end
			if pos_index and pos_index > 0 and v:PosIndex() ~= pos_index then
				is_insert = false
			end
			if not is_pet and v:PetIndex() then
				is_insert = false
			end
			-- 屏蔽掉当前选中方案内所记录的神印
			if cur_ctrl_blueprint then
				for _,_index in pairs(cur_ctrl_blueprint:Stamps()) do
					if v:Index() == _index then
						is_insert = false
						break
					end
				end
			end
			if is_insert then
				table.insert(list, v)
			end
		end
	end
	-- 排序
	list = self:SortPetStampList(list, sort_type)
	-- 补齐
	if #list < min_count then
		local len = #list
		for i = len + 1, min_count do
			table.insert(list, PetStamp.New())
		end
	end
	local len = #list % 3
	len = len > 0 and 3 - len or 0
	for i = 1, len do
		table.insert(list, PetStamp.New())
	end
	return list
end

-- sel_type ： 1 品质 2 ：等级，sort_type = 1 降序， 2 = 升序
function PetStampData:GetIntensifyList(sel_pet_stamp,sel_type, sort_type)
	sel_type = sel_type or 1
	sort_type = sort_type or 2
	local list = {}
	local stamp_list = self:GetPetStampList()
	for i,v in pairs(stamp_list) do
		if v.item_id > 0 and not v:PetIndex() and not v:IsLock() and v:Index() ~= sel_pet_stamp:Index() then  -- 穿戴过的和锁定的也不放入
			table.insert(list, v)
		end
	end
	local sort_func
	if sel_type == 1 then
		sort_func = function (a, b)
			if sort_type == 2 then
				return a:QuaLevel() < b:QuaLevel()
			else
				return a:QuaLevel() > b:QuaLevel()
			end
		end
	else
		sort_func = function (a, b)
			if a:Level() == b:Level() then
				if sort_type == 2 then
					return a:QuaLevel() < b:QuaLevel()
				else
					return a:QuaLevel() > b:QuaLevel()
				end
			end
			if sort_type == 2 then
				return a:Level() < b:Level()
			else
				return a:Level() > b:Level()
			end
		end
	end
	table.sort(list, sort_func)

	-- 全部道具材料
	local stuff_list = BagData.Instance:GetPetStampExpStuffList()

	list = AndList(stuff_list, list)

	return list
end

-- 获取全部套装id
function PetStampData:GetSuitIdList()
	if self.suit_id_list == nil then
		self.suit_id_list = {}
		for i,v in pairs(self.suit_group) do
			table.insert(self.suit_id_list,i)
		end
	end
	return self.suit_id_list
end

-- 获取套装分类数据
function PetStampData:GetSuitListData(is_num,qua)
	local suit_list = self:GetSuitIdList()
	local stamp_list = self:GetPetStampList()
	local list_data = {}
	for _,suit_id in pairs(suit_list) do
		local data = {id = suit_id, num = 0, qua = qua}
		for i,v in pairs(stamp_list) do
			if v.item_id > 0 and v:PetIndex() == nil then
				local cfg = v:ItemCfg()
				if cfg.param1 == suit_id then
					data.num = data.num + 1
				end
			end
		end
		if is_num then
			if data.num > 0 then
				table.insert(list_data,data)
			end
		else
			table.insert(list_data,data)
		end
	end
	return list_data
end


-- 获取神印列表加成的属性
function PetStampData:GetStampListAddition(pet_stamp_list)
	local attri_list = {}
	for i,v in pairs(pet_stamp_list) do
		local stamp_attri_list = v:AttriList()
		for j,v2 in pairs(stamp_attri_list) do
			if attri_list[j] == nil then
				attri_list[j] = {attri_type = j, attri_value = 0}
			end
			attri_list[j].attri_value = attri_list[j].attri_value + v2.attri_value
		end
	end
	return attri_list
end

-- 属性排序
function PetStampData:AttriSort(attri_list)
	local t = TableCopy(attri_list)
	table.sort(t, function (a, b)
		return a.attri_type < b.attri_type
	end)
	return t
end

-- 排序神印 p1: PetStamp[] p2: PetStampCfg.SortType 默认为配置里面的第一个
-- p3：是否copy一个新的stamp_list进行排序
function PetStampData:SortPetStampList(stamp_list, sort_type, is_copy)
	if is_copy then
		stamp_list = TableCopy(stamp_list)
	end
	sort_type = sort_type or PetStampCfg.SortType.Level
	local sort_func
	if sort_type == PetStampCfg.SortType.Qua then
		sort_func = function (a, b)
			if a:QuaLevel() == b:QuaLevel() then
				return a:Level() > b:Level()
			end
			return a:QuaLevel() > b:QuaLevel()
		end
	elseif sort_type == PetStampCfg.SortType.Level then
		sort_func = function (a, b)
			if a:Level() == b:Level() then
				return a:QuaLevel() > b:QuaLevel()
			end
			return a:Level() > b:Level()
		end
	elseif sort_type == PetStampCfg.SortType.Attri0 then
		sort_func = function (a, b)
			return a:Attri(0) > b:Attri(0)
		end
	elseif sort_type == PetStampCfg.SortType.Attri1 then
		sort_func = function (a, b)
			return a:Attri(1) > b:Attri(1)
		end
	elseif sort_type == PetStampCfg.SortType.Attri3 then
		sort_func = function (a, b)
			return a:Attri(3) > b:Attri(3)
		end
	elseif sort_type == PetStampCfg.SortType.Attri4 then
		sort_func = function (a, b)
			return a:Attri(4) > b:Attri(4)
		end
	end
	if sort_func then
		table.sort(stamp_list, sort_func)
	end
	return stamp_list
end

--------------------------------  强化  -----------------------------
function PetStampData:SetIntensifyStamp(pet_stamp)
	self.intensify_pet_stamp = pet_stamp
end

function PetStampData:GetIntensifyStamp()
	return self.intensify_pet_stamp
end

-- 强化到指定等级还需要多少经验
function PetStampData:GetIntensifyNeedExp(pet_stamp, up_level)
	local self_exp = pet_stamp:Exp()
	local need_exp = 0
	local level = pet_stamp:Level()
	local qua = pet_stamp:QuaLevel()
	local cur_cfg = Cfg.PetStampIntensifyCfg(qua, level)
	need_exp = cur_cfg.exp - self_exp
	for i = level + 1, up_level - 1 do
		local cfg = Cfg.PetStampIntensifyCfg(qua, i)
		need_exp = need_exp + cfg.exp
	end
	return need_exp
end

-- 指定经验可以强化到多少级, limit_type = 1 最大等级，limit_type = 2 背包经验
function PetStampData:GetExpToLevel(pet_stamp, exp)
	local self_exp = pet_stamp:Exp()
	exp = exp + self_exp
	local level = pet_stamp:Level()
	local max_level = pet_stamp:MaxLevel()
	local qua = pet_stamp:QuaLevel()
	for i = level, max_level - 1 do
		local cfg = Cfg.PetStampIntensifyCfg(qua, i)
		if exp < cfg.exp then
			return i, 2
		else
			exp = exp - cfg.exp
		end
	end
	return max_level, 1
end

-- 加多少值，加多少属性条
function PetStampData:GetIntensifyAddition(pet_stamp, add_level)
	local level = pet_stamp:Level()
	local qua = pet_stamp:QuaLevel()
	local list = {}
	local main_attri = pet_stamp:MainAttri()
	local lerp_level = add_level - level
	local main_add_value = self:GetIntensifyAddCfg(qua, main_attri.attri_type, lerp_level)
	table.insert(list, {type = 1,attri_type = main_attri.attri_type, attri_value = main_attri.attri_value, add_value = main_add_value})
	local add_attri = pet_stamp:AddAttri()
	local count = 0
	for i,v in pairs(add_attri) do
		count = count + 1
	end

	local is_add_attri = false
	local new_attri
	for i = level + 1 , add_level do
		local add_cfg = Cfg.PetStampIntensifyCfg(qua, i)
		if add_cfg.deputy_attribute_up == 1 then
			if  count < PetStampCfg.MaxAddAttriCount then
				new_attri = {type = 2}
				break
			else
				is_add_attri = false --true  -- 副属性随机加一条属性值，又不需要预览那个提升箭头了
			end
		end
	end

	for i,v in pairs(add_attri) do
		-- 副属性大于等于4条才会加属性，服务端写死
		local add_value = {min = 0, max = 0}
		if is_add_attri then
			if count >= PetStampCfg.MaxAddAttriCount then
				add_value = self:GetIntensifyAddCfg(qua, v.attri_type, lerp_level)
			end
		end
		table.insert(list, {type = 2,attri_type = v.attri_type, attri_value = v.attri_value, add_value = add_value})
	end
	table.insert(list, new_attri)
	return list
end

-- 获取强化加成的值
function PetStampData:GetIntensifyAddCfg(qua, attri_type, num)
	num = num or 1
	local cfg = self.intensify_group[qua]
	for i,v in pairs(cfg) do
		if v.att_type == attri_type then
			return {min = v.intensify_att_min * num, max = v.intensify_att_max * num}
		end
	end
	return {min = 0, max = 0}
end

-- 获取神印值多少经验 (消耗经验 * 0.8) + 本身经验
function PetStampData:GetStampPriceExp(pet_stamp)
	local exp = pet_stamp:Exp()
	local level = pet_stamp:Level()
	local qua = pet_stamp:QuaLevel()
	local price_exp = self.reslove_stamp_cfg[qua].item_exp
	for i = 1, level - 1 do
		local cfg = Cfg.PetStampIntensifyCfg(qua, i)
		price_exp = price_exp + (cfg.exp * self.stamp_cfg.other[1].exp_attenuation_100 / 100)
	end
	exp = exp * self.stamp_cfg.other[1].exp_attenuation_100 / 100
	price_exp = math.floor(price_exp + exp)
	return price_exp
end

-- 获取道具值多少经验
function PetStampData:GetStuffPriceExp(stuff_exp_item, num)
	num = num or stuff_exp_item.num or 1
	local cfg = self.stamp_stuff_cfg[stuff_exp_item.item_id]
	if cfg == nil then
		return 0
	end
	return num * cfg.intensify_item_exp
end

-- 获取神印升满还差多少经验
function PetStampData:GetPetStampNeedMaxExp(pet_stamp)
	local exp = pet_stamp:Exp()
	local level = pet_stamp:Level()
	local qua = pet_stamp:QuaLevel()
	local max_level = pet_stamp:MaxLevel()
	local need_exp = 0
	for i = level, max_level do
		local cfg = Cfg.PetStampIntensifyCfg(qua, i)
		need_exp = need_exp + cfg.exp
	end
	need_exp = need_exp - exp
	return need_exp
end

-- 通过套装id和数量获取套装描述
function PetStampData:GetSuitDesc(suit_id, num, title, title_color)
	title = title or "x%s:"
	local suit_group = PetStampData.Instance.suit_show_group[suit_id]
	if suit_group == nil then
		return Language.PetStampBlueprint.SuitTip6
	end
	local count = 0
	if num >= 4 then
		count = 2
	elseif num >= 2 then
		count = 1
	end
	if count == 0 then
		return ""
	end
	local desc = ""
    for i = 1,count do
		local str = string.format(title,suit_group[i].suit_num)
		if title_color then
			str = ColorStr(str,title_color)
		end
		desc = desc .. str .. suit_group[i].god_print_txt
		if i ~= count then
			desc = desc  .. "\n"
		end
    end
	return desc
end

-- 通过套装id、数量、品质获取套装描述
function PetStampData:GetSuitDescByQua(suit_id, num, qua, title, title_color)
	title = title or "x%s:"
	local suit_group = self.suit_group[suit_id][qua]
	if suit_group == nil then
		return Language.PetStampBlueprint.SuitTip6
	end
	local count = 0
	if num >= 4 then
		count = 2
	elseif num >= 2 then
		count = 1
	end
	if count == 0 then
		return ""
	end
	local desc = ""
    for i = 1,count do
		local str = string.format(title,suit_group[i].suit_num)
		if title_color then
			str = ColorStr(str,title_color)
		end
		desc = desc .. str .. suit_group[i].god_print_txt
		if i ~= count then
			desc = desc  .. "\n"
		end
    end
	return desc
end

-- 通过套装id、数量、品质获取套装描述 suit_list[{id, qua, num}]
function PetStampData:GetSuitDescBySuitList(suit_list, title, title_color)
	local all_desc = ""
	title = title or "x%s:"
	for i,v in pairs(suit_list) do
		local desc = ""--self:GetSuitDescByQua(v.suit_id, v.num, v.qua, title, title_color)
		local suit_group = self.suit_group[v.suit_id][v.qua]
		local str = string.format(title,suit_group[i].suit_num)
		if title_color then
			str = ColorStr(str,title_color)
		end
		desc = desc .. str .. suit_group[i].god_print_txt

		all_desc = all_desc .. desc
		if i ~= #suit_list then
			all_desc = all_desc .. "\n"
		end
	end
	return all_desc
end

-- 设置套装新字标记
function PetStampData:SetSuitNewFlag(suit_id,value)
	self.suit_new_falg_data = self.suit_new_falg_data or {}
	self.suit_new_falg_data[suit_id] = value
end

function PetStampData:GetSuitNewFlag(suit_id)
	self.suit_new_falg_data = self.suit_new_falg_data or {}
	return self.suit_new_falg_data[suit_id] or false
end

------------------------------------- 方案 ------------------------------------
function PetStampData:GetBlueprintOkList()
	local pet = PetData.Instance:GetCurSelect()
	local list = {}
	local pet_blueprint = pet:StampBlueprint()
	if pet then
		table.insert(list,pet_blueprint)
		pet_blueprint:Name(Language.PetStampBlueprint.DefaultName)
	end
	local blueprint_list = self:GetBlueprintList()
	for i,v in pairs(blueprint_list) do
		if v:IsOk() then
			if pet_blueprint and v:PetIndex() == pet.index then
				pet_blueprint:Name(v:Name())
			else
				table.insert(list, v)
			end
		end
	end
	return list
end

-- 获取一个空方案 为空说明方案满了
function PetStampData:GetEmptyBlueprint()
	local blueprint_list = self:GetBlueprintList()
	for i,v in pairs(blueprint_list) do
		if not v:IsOk() then
			return v
		end
	end
end

-- 获取方案的属性加成
function PetStampData:GetBlueprintAddition(blueprint)
	local pet_stamp_list = {}
	for i = 1, PetStampCfg.StampMaxPos do
		local stamp = blueprint:GetStampByPos(i)
		if stamp then
			table.insert(pet_stamp_list, stamp)
		end
	end
	local attri_list = self:GetStampListAddition(pet_stamp_list)

	-- 套装属性加成
	local suit_list = self:GetBlueprintSuit(blueprint)
	if not TableIsEmpty(suit_list) then
		for i,v in pairs(suit_list) do
			local suit_group = self.suit_group[v.suit_id][v.qua]
			if not TableIsEmpty(suit_group) then
				for k,suit in pairs(suit_group) do
					if suit.suit_num >= v.num and suit.effect_type == 1 then
						if attri_list[suit.param1] == nil then
							attri_list[suit.param1] = {attri_type = suit.param1, attri_value = suit.param2}
						else
							attri_list[suit.param1].attri_value = attri_list[suit.param1].attri_value + suit.param2
						end
					end
				end
			end
		end
	end
	return attri_list
end


-- 获取方案套装信息 return ====== 套装suit_list[{id, qua, num}], 最大数量为2，最好的套装效果放最后面
-- 当玩家穿戴4枚橙色神印，激活 橙色 神印的两件套属性和四件套属性
-- 当玩家穿戴1枚红色神印，3枚橙色神印时，激活 橙色 神印两件套属性和 橙色 四件套属性
-- 当玩家穿戴2枚红色神印，2枚橙色神印时，激活 红色 神印两件套属性和 橙色 四件套属性
-- 当玩家穿戴3枚红色神印，1枚橙色神印时，激活 红色 神印两件套属性和 橙色 四件套属性
-- 当玩家穿戴4枚红色神印，激活 红色 神印的两件套属性和四件套属性
-- 当玩家穿戴2枚红色神印激活 红色 神印两件套属性
-- 当玩家穿戴2枚橙色神印激活 橙色 神印两件套属性
-- 一橙一红，显示橙色两件套的属性
function PetStampData:GetBlueprintSuit(blueprint)
	if not blueprint:IsOk() then
		return
	end

	-- 汇总套装id列表的神印品质数量
	local info_list = {}
	for i = 1, PetStampCfg.StampMaxPos do
		local stamp = blueprint:GetStampByPos(i)
		if stamp and stamp.item_id > 0 then
			local suit_id = stamp:SuitId()
			local qua = stamp:QuaLevel()
			if suit_id > 0 then
				if info_list[suit_id] == nil then
					info_list[suit_id] = {}
				end
				if info_list[suit_id][qua] == nil then
					info_list[suit_id][qua] = 0
				end
				info_list[suit_id][qua] = info_list[suit_id][qua] + 1
			end
		end
	end

	local suit_list = {}
	-- 筛选生成套装效果的套装id
	for suit_id,v in pairs(info_list) do
		local a = v[5] or 0	-- 表示橙色品质数量
		local b = v[6] or 0	-- 表示红色品质数量
		if a >= 4 or (b == 1 and a >= 3) then
			-- 当玩家穿戴4枚橙色神印，激活 橙色 神印的两件套属性和四件套属性
			-- 当玩家穿戴1枚红色神印，3枚橙色神印时，激活 橙色 神印两件套属性和 橙色 四件套属性
			table.insert(suit_list, {suit_id = suit_id, num = 2, qua = 5})
			table.insert(suit_list, {suit_id = suit_id, num = 4, qua = 5})
		elseif b >= 4 then
			table.insert(suit_list, {suit_id = suit_id, num = 2, qua = 6})
			table.insert(suit_list, {suit_id = suit_id, num = 4, qua = 6})
		elseif b >= 2 and (a + b) >= 4 then
			-- 当玩家穿戴2枚红色神印，2枚橙色神印时，激活 红色 神印两件套属性和 橙色 四件套属性
			-- 当玩家穿戴3枚红色神印，1枚橙色神印时，激活 红色 神印两件套属性和 橙色 四件套属性
			table.insert(suit_list, {suit_id = suit_id, num = 2, qua = 6})
			table.insert(suit_list, {suit_id = suit_id, num = 4, qua = 5})
		elseif b >= 2 and (a + b) < 4 then
			-- 当玩家穿戴2枚红色神印激活 红色 神印两件套属性
			table.insert(suit_list, {suit_id = suit_id, num = 2, qua = 6})
		elseif (a + b) >= 2 and (a + b) < 4 then
			-- 当玩家穿戴2枚橙色神印激活 橙色 神印两件套属性
			-- 一橙一红，显示橙色两件套的属性
			table.insert(suit_list, {suit_id = suit_id, num = 2, qua = 5})
		end
	end

	table.sort(suit_list, function (a,b)
		if a.num == b.num then
			return a.qua < b.qua
		end
		return a.num < b.num
	end)
	return suit_list
end

-- 方案属性比较 b2是被比较的方案
function PetStampData:CompareBlueprint(b1, b2)
	local list = {}
	local cur_attri_list = {}
	local target_attri_list = {}
	if b1 and b1.AttriList then
		cur_attri_list = b1:AttriList()
	end
	if b2 and b2.AttriList then
		target_attri_list = b2:AttriList()
	end
	local index = 3
	for i,v in pairs(cur_attri_list) do
		local data = {}
		data.attri_type = v.attri_type
		data.attri_value = v.attri_value
		if target_attri_list[v.attri_type] and target_attri_list[v.attri_type].attri_value then
			data.add_value = target_attri_list[i].attri_value - v.attri_value
		else
			data.add_value = 0
		end
		data.show_bg = index <= 3
		index = index - 1
		if index < 1 then
			index = 6
		end
		table.insert(list,data)
	end
	return list
end


------------------------- 祈神 ---------------------
function PetStampData:SetPrayData(info)
	self.pray_data:Set(info)
end

function PetStampData:GetPrayData()
	return self.pray_data
end

-- 获取祈神的位置
function PetStampData:GetPrayPosIndex()
	return self.pray_pos_index
end

-- 祈神日志信息
function PetStampData:SetPrayLogData(protocol)
	self.pray_log_data.log_list = protocol.log_list
end

function PetStampData:GetPrayLogData()
	return self.pray_log_data.log_list
end

-- 兑换数据
function PetStampData:GetConvertItemListData(qua)
	--if self.convert_item_list == nil then
		local cfg = self.stamp_cfg.exchange_god_print
		local cfg_list = {}
		if qua then
			for i,v in pairs(cfg) do
				if v.star_level == qua then
					table.insert(cfg_list, v)
				end
			end
		else
			cfg_list = TableCopy(cfg_list)
		end
		local list = {}
		local index = 0
		for i,v in ipairs(cfg_list) do
			if (i - 1) % 4 == 0 then
				index = index + 1
				list[index] = {}
			end
			table.insert(list[index], v)
		end
		return list
		--self.convert_item_list = list
	--end
	--return self.convert_item_list
end

-- 根据祈神结果获取item_lsit
function PetStampData:GetPrayResultItemList()
	local data = self:GetPrayData()
	local list = {}
	local item_list = {}
	
	local item_info_list = data.item_info_list

	local reward_num = data.reward_num or 0
	local stamp_list = data.pet_stamp_index_list or {}
	for i = 1, reward_num do
		if stamp_list[i] and  stamp_list[i] > 0 then
			local stamp = self:GetPetStampByIndex(stamp_list[i])
			if stamp and stamp.item_id > 0 then
				stamp.num = 1
				table.insert(list,stamp)
			end
		end
		local info = item_info_list[i]
		if info.group > 0 and info.seq > 0 then
			local cfg = Cfg.PetStampPrayRewardCfg(info.group,info.seq)
			if cfg and not TableIsEmpty(cfg.item_list) then
				for i,v in pairs(cfg.item_list) do
					table.insert(item_list, Item.Create(v))
				end
			end
		end
	end
	self:SetPrayData({})
	return list, item_list
end

-- 获取祈神价格
function PetStampData:GetPrayCoin(seq,curr_type)
	curr_type = curr_type or 1
	local cfg = self.stamp_cfg.pray
	if curr_type == 1 then
		return cfg[seq].expend_coin
	else
		return cfg[seq].expend_gold
	end
end

-- 分解设置
function PetStampData:ClearResloveData()
	--self.pray_reslove_set_data = {}
	-- 只把强化材料分解的设置清空
	self:SetResloveData(0, false)
end

function PetStampData:SetResloveData(qua, value)
	self.pray_reslove_set_data = self.pray_reslove_set_data or {}
	self.pray_reslove_set_data[qua] = value
end

function PetStampData:GetResloveData(qua)
	return self.pray_reslove_set_data[qua] or false
end

-- 分解设置记录到本地
function PetStampData:SetResloveRecord(qua, value)
	qua = qua or 999
	if value and value ~= 0 then
		value = 1
	else
		value = 0
	end
	UnityPlayerPrefs.SetInt(string.format("pet_stamp_reslove_record_%s",qua), value)
end

function PetStampData:GetResloveRecord(qua)
	if not qua then
		return 0
	end
	local flag = UnityPlayerPrefs.GetInt(string.format("pet_stamp_reslove_record_%s",qua)) or 0
	return flag
end

--设置吞噬神印信息
function PetStampData:SetSwallowMain(item)
	self.main_swallow_item = item
	if item then
		self.swallow_smart.main_index = item:Index()
	else
		self.swallow_smart.main_index = -1
	end
end

function PetStampData:GetSwallowMain()
	return self.main_swallow_item
end
--设置被吞噬的神印信息
function PetStampData:SetSwallowCo(item)
	self.co_swallow_item = item
	if item then
		self.swallow_smart.co_index = item:Index()
	else
		self.swallow_smart.co_index = -1
	end
end

function PetStampData:GetSwallowCo()
	return self.co_swallow_item
end

function PetStampData:GetSwallowCost()
	return self.stamp_cfg.other[1].swallow_gold_cost or 100
end

function PetStampData:GetSwallowSelectList()
	local select_list = {}
	local main_stamp = self.main_swallow_item
	for _, v in pairs(self.pet_stamp_list) do
		if v.item_id == main_stamp.item_id 	--相同的神印才能吞噬
		 	-- and v:QuaLevel() == main_stamp:QuaLevel()	--被吞噬的与吞噬的要相同品质（item_id相同的品质肯定相同，先留着防止以后策划吃书说不限id限品质）
			and v:Level() == main_stamp:Level()		--被吞噬的与吞噬的要相同（强化）等级
			and v:Index() ~= main_stamp:Index()		--吞噬的不进入列表
			and v:PetIndex() == nil					--已被配戴的不进入列表
			and not v:Swallowed() then				--已经吞噬过的不进入列表

			table.insert(select_list,v)
		end
	end
	return select_list
end

--------------------- 神印分解数据 --------------------
PetStampResolveData = PetStampResolveData or BaseClass()

function PetStampResolveData:__init()
	if PetStampResolveData.Instance ~= nil then
		Debuger.LogError("[PetStampResolveData] attempt to create singleton twice!")
		return
	end	
	PetStampResolveData.Instance = self
	self.select_items = SmartData.New({})
	self.show_item = {}
end

function PetStampResolveData:__delete()
	self.select_items:Set({})
	self.select_items = nil
end

--获取选择的items
function PetStampResolveData:GetSelectItems()
	return self.select_items
end

--设置选中的itms
function PetStampResolveData:SetSelectItems(index,item)
	local index = item:Type() * 10000 + index
	if self.select_items[index] == nil then
		self.select_items[index] = item
		if item:Type() == ItemType.PetStamp then
			self:SetShowItem(item)
		end
	else
		self.select_items[index] = nil
	end
end

--清空选中的items
function PetStampResolveData:ClearSelectItems()
    self.select_items:Set({})
end


--可分多少神印经验
function PetStampResolveData:GetAllSelectItemResExp()
	local exp = 0
	local stuff_count = 0
	local stamp_count = 0
	for i,v in pairs(self.select_items:Val()) do
		if v:Type() == ItemType.PetStamp then
			exp = exp + v:PriceExp()
			stamp_count = stamp_count + 1
		else
			exp = exp + PetStampData.Instance:GetStuffPriceExp(v)
			stuff_count = stuff_count + v:Num()
		end
	end
	return exp,stamp_count,stuff_count
end


function PetStampResolveData:SetShowItem(item)
	self.show_item = item
end

function PetStampResolveData:GetShowItem()
	return self.show_item
end