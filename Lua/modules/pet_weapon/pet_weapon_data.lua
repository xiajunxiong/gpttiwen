PetWeaponViewStatus = {
	Normal = 1,		-- 
	Refine = 2,		-- 精炼
	Intensify = 3,  -- 强化
}

-- 是百分比的属性
PetWeaponAttriCfg = {
    [15] = true,       -- name = "必杀:"
    [16] = true,       -- name = "命中:"
    [17] = true,       -- name = "闪避:"
    [18] = true,       -- name = "反击:"
    [22] = true,       -- name = "坚韧:"
	[21] = true,       -- name = "必杀伤害:"
	[23] = true,	   -- name = "控制命中:"
	[25] = true,	   -- name = "法术穿透:"
	[26] = true,	   -- name = "控制抵抗率:"
    [30] = true,       -- name = "中毒抗性:"
    [31] = true,       -- name = "石化抗性:"
    [32] = true,       -- name = "昏睡抗性:"
    [33] = true,       -- name = "混乱抗性:"
    [34] = true,       -- name = "酒醉抗性:"
}

--  ========= 升级就是强化，强化就是升级 ！！！！！！！！！！！！！

PetWeaponData = PetWeaponData or BaseClass()

function PetWeaponData:__init()
    if PetWeaponData.Instance ~= nil then
		Debuger.LogError("[PetWeaponData] attempt to create singleton twice!")
		return
	end
	PetWeaponData.Instance = self

	self.cfg = Config.pet_weapon_auto
	self:InitCfg()

	self.all_pet_weapon_list = SmartData.New()

	-- -- 记录操作状态，仓库根据状态显示不同的逻辑
	self.view_status_data = SmartData.New({status_type = PetWeaponViewStatus.Intensify})
	
	--选中的最大容量
	self.max_sel_count = 30
	self.max_sel_weapon_count = 20
	-- 仓库选中的格子列表
	self.bag_selected_list = SmartData.New({cell_list = {}, cur_item = nil})

	-- 仓库排序信息
	self.bag_sort_info = SmartData.New({sort_type = 2, sel_qua = 0})
	-- 魂器替换界面排序信息
	self.replace_sort_info = SmartData.New({sort_type = 2, sel_qua = 0})

	-- 自动放入的品质
	self.auto_put_qua = SmartData.New({qua = 0})

	-- 当前需要操作的weapon
	self.cur_ctrl_weapon = nil

	-- 选中的精炼材料
	self.sel_refine_stuff = nil

	-- 选中的转换魂器
	self.sel_change_weapon = SmartData.New({pet_weapon = {}})

	-- 红点数据
	self.remind_data = SmartData.New()
	-- 选择的魂器格子
	self.weapon_idx = 0
end

function PetWeaponData:__delete()

end

function PetWeaponData:InitCfg()
	-- 魂器基础配置
	Cfg.Register("PetWeaponBaseCfg",Cfg.RealTimeSearch(self.cfg.pet_weapon_cfg,"pet_weapon_item"))
	-- 魂器强化配置
	Cfg.Register("PetWeaponIntensifyCfg",Cfg.RealTimeSearch2Key(self.cfg.pet_weapon_intensify,"quality","levels"))
	-- 魂器突破配置
	Cfg.Register("PetWeaponBreakCfg",Cfg.RealTimeSearch2Key(self.cfg.pet_weapon_steps,"quality","steps"))
	-- 魂器精炼配置
	Cfg.Register("PetWeaponRefineCfg",Cfg.RealTimeSearch2Key(self.cfg.pet_weapon_refine,"seq","refine"))
	-- 魂器品质配置
    Cfg.Register("PetWeaponQuaCfg",Cfg.RealTimeSearch(self.cfg.pet_weapon_quality,"quality"))
	-- 强化属性加成配置
	Cfg.Register("PetWeaponIntensifyAttriCfg",Cfg.RealTimeSearch2Key(self.cfg.intensify_att,"quality","att_type"))
	-- 突破属性加成配置
	Cfg.Register("PetWeaponBreakAttriCfg",Cfg.RealTimeSearch2Key(self.cfg.steps_att,"quality","att_type"))

	-- 魂器材料配置
	self.weapon_stuff_cfg = KeyList(self.cfg.item_id_exp, "item_id")

	-- 精炼组
	self.refine_group = self:ConvertGroup(self.cfg.pet_weapon_refine, "seq")
	-- 突破组
	self.break_group = self:ConvertGroup(self.cfg.pet_weapon_steps, "quality")

	-- 强化属性组
	self.intensify_attri_group = self:ConvertGroup(self.cfg.intensify_att, "quality")
	-- 突破属性组
	self.break_attri_group = self:ConvertGroup(self.cfg.steps_att, "quality")
	-- 基础属性组
	self.base_attri_group = self:ConvertGroup(self.cfg.att_group, "attribute_group")
	-- 技能特效组
	self.skill_effect_group = KeyList(self.cfg.skill_buff, "seq")
end

--配置数据分组 例如：{{seq = 1, value = 1}, {seq = 1, value = 2}, {seq = 2, value = 1}, {seq = 2, value = 2}}
-- => {[1] = {{seq = 1, value = 1}, {seq = 1, value = 2}}, [2] = {{seq = 2, value = 1}, {seq = 2, value = 2}}}
function PetWeaponData:ConvertGroup(base_cfg, key)
    local new_cfg = {}
	for i, v in pairs(base_cfg) do
		if new_cfg[v[key]] == nil then
			new_cfg[v[key]] = {}
		end
		table.insert(new_cfg[v[key]], v)
	end
    return new_cfg
end

function PetWeaponData:CurWeapon()
	local pet = PetData.Instance:GetCurSelect()
	if TableIsEmpty(pet) then
		return
	end
    local weapon = pet:Weapon()
	if TableIsEmpty(weapon) then
		return
	end
	return weapon
end

function PetWeaponData:GetStatusData()
	return self.view_status_data
end

function PetWeaponData:SetStatusData(status_type)
	local data = {status_type = status_type}
	self.view_status_data:Set(data)
end

function PetWeaponData:AddSelectedBagCell(item, num)
	num = num or 0
	if num > 0 then
		self.bag_selected_list.cell_list[item] = num
		self.bag_selected_list.cur_item = item
	else
		self.bag_selected_list.cell_list[item] = nil
		self.bag_selected_list.cur_item = nil
	end
	self.bag_selected_list.check = not self.bag_selected_list.check and true or false
	self.sel_list_item_price_exp = nil
end

function PetWeaponData:ClearSelectedBagCell()
	self.bag_selected_list.cell_list = {}
	self.bag_selected_list.cur_index = nil
	self.sel_list_item_price_exp = nil
end

function PetWeaponData:IsInSelectedBagCell(item)
	return self.bag_selected_list.cell_list[item] or false
end

-- 当前背包选中的格子 is_sel是否选择一个数量大于0的物品
function PetWeaponData:CurBagSelCell(is_sel)
	if self.bag_selected_list.cur_item then
		--return BagData.Instance:GetItemByIndex(ItemColumnType.Item, self.bag_selected_list.cur_index)
		return self.bag_selected_list.cur_item
	end
	if is_sel then
		for i,v in pairs(self.bag_selected_list.cell_list) do
			if v > 0 then
				--return BagData.Instance:GetItemByIndex(ItemColumnType.Item, i)
				return i
			end
		end
	end
	return Item.Create(Item.Create({item_id = 0}))
end

-- 获取材料配置
function PetWeaponData:GetStuffCfg(item_id)
	return self.weapon_stuff_cfg[item_id]
end

-- 获取背包格子显示列表
function PetWeaponData:GetWeaponGridListData()
	local min_count = 25
	local all_soul_list = BagData.Instance:GetWeaponBagList() or {}
	self:SortWeaponList(all_soul_list, self.bag_sort_info.sort_type, self.bag_sort_info.sel_qua)	-- 排序
	local soul_count = #all_soul_list
	if soul_count > min_count then
		min_count = math.ceil(soul_count / 5) * 5
	end
	local grid_list = {}
	for i = 1,min_count do
		local item = all_soul_list[i] or Item.Create({item_id = 0})
		table.insert(grid_list, item)
	end
	return grid_list
end

-- 按等级排序魂器 sort_type: 1 降序，2 升序
function PetWeaponData:SortWeaponList(weapon_list, sort_type, target_qua, pet_index)
	target_qua = target_qua or 0
	sort_type = sort_type or 2
	pet_index = pet_index or 9999

	-- 降序
	local sort_drop = function (a, b)
		local a_sort_num = 0
		local b_sort_num = 0
		if a:Type() ~= ItemType.PetWeapon and b:Type() ~= ItemType.PetWeapon then
			return a:Color() > b:Color()
		end
		if a:Type() ~= ItemType.PetWeapon and b:Type() == ItemType.PetWeapon then
			return true
		end
		if a:Type() == ItemType.PetWeapon and b:Type() ~= ItemType.PetWeapon then
			return false
		end
		if a:PetIndex() == pet_index then
			a_sort_num = a_sort_num + 100000
		end
		if b:PetIndex() == pet_index then
			b_sort_num = b_sort_num + 100000
		end
		if a:QuaType() == target_qua then
			a_sort_num = a_sort_num + 100
		end
		if b:QuaType() == target_qua then
			b_sort_num = b_sort_num + 100
		end
		if a:Type() == b:Type() then
			if a_sort_num == b_sort_num then
				if a:Level() == b:Level() then
					if a:RefineLevel() == b:RefineLevel() then
						return a:Seq() > b:Seq()
					end
					return a:RefineLevel() > b:RefineLevel()
				end
				return a:Level() > b:Level()
			end
			return a_sort_num > b_sort_num
		end
		return a:Type() ~= ItemType.PetWeapon
	end

	-- 升序
	local sort_up = function (a, b)
		local a_sort_num = 0
		local b_sort_num = 0
		if a:Type() ~= ItemType.PetWeapon and b:Type() ~= ItemType.PetWeapon then
			return a:Color() > b:Color()
		end
		if a:Type() ~= ItemType.PetWeapon and b:Type() == ItemType.PetWeapon then
			return true
		end
		if a:Type() == ItemType.PetWeapon and b:Type() ~= ItemType.PetWeapon then
			return false
		end
		if a:PetIndex() == pet_index then
			a_sort_num = a_sort_num + 100000
		end
		if b:PetIndex() == pet_index then
			b_sort_num = b_sort_num + 100000
		end
		if a:QuaType() == target_qua then
			a_sort_num = a_sort_num + 100
		end
		if b:QuaType() == target_qua then
			b_sort_num = b_sort_num + 100
		end
		if a:Type() == b:Type() then
			if a_sort_num == b_sort_num then
				if a:Level() == b:Level() then
					if a:RefineLevel() == b:RefineLevel() then
						return a:Seq() < b:Seq()
					end
					return a:RefineLevel() < b:RefineLevel()
				end
				return a:Level() < b:Level()
			end
			return a_sort_num > b_sort_num
		end
		return a:Type() ~= ItemType.PetWeapon
	end

	if sort_type == 1 then
		table.sort(weapon_list, sort_drop)
	elseif sort_type == 2 then
		table.sort(weapon_list, sort_up)
	end
end

-- 获取魂器升满最多需要的经验
function PetWeaponData:GetIntensifyNeedExp(pet_weapon)
	local base_exp = pet_weapon:BaseCfg().levels_1_pet_weapon_exp
	local start_level = pet_weapon:Level()
	local end_level = pet_weapon:MaxLimitLevel() - 1
	local cur_exp = pet_weapon:Exp()
	local qua_type = pet_weapon:QuaType()
	
	local sum_exp = self:CalculateIntensifyExp(qua_type, start_level, end_level)
	sum_exp = sum_exp - cur_exp
	return sum_exp
end

-- 获取魂器值多少经验 = 基础经验值 + (一共消耗的经验 * 衰减系数)
function PetWeaponData:GetPriceExp(pet_weapon)
	local base_exp = pet_weapon:BaseCfg().levels_1_pet_weapon_exp
	local start_level = 1
	local end_level = pet_weapon:Level()

	if end_level == 1 then
		return base_exp
	end
	local cur_exp = pet_weapon:Exp()
	local qua_type = pet_weapon:QuaType()

	end_level = end_level - 1

	local sum_exp = self:CalculateIntensifyExp(qua_type, start_level, end_level)
	base_exp = base_exp + (sum_exp + cur_exp) * PetWeaponData.Instance.cfg.other[1].exp_attenuation_100 / 100
	base_exp = math.floor(base_exp)
	return base_exp
end

-- 计算魂器升级的累计经验
function PetWeaponData:CalculateIntensifyExp(qua_type, start_level, end_level)
	local sum_exp = 0
	for i = start_level, end_level do
		local cfg = PetWeapon.GetLevelCfg(qua_type, i)
		sum_exp = sum_exp + cfg.exp
	end
	return sum_exp
end

-- 计算魂器加的经验可升到多少级
function PetWeaponData:CanAddLevel(weapon, exp)
	exp = exp + weapon:Exp()
	local level = weapon:Level()
	local max_level = weapon:MaxLimitLevel()
	local qua_type = weapon:QuaType()
	local up_level = level
	local num = 0
	for i = level , max_level do
		local cfg = PetWeapon.GetLevelCfg(qua_type, i)
		num = num + cfg.exp
		if exp >= num then
			up_level = cfg.levels + 1
		else
			return up_level
		end
	end
	return up_level
end

-- 计算加的经验需要消耗多少铜币
function PetWeaponData:ExpConsumeCoin(exp)
	local num = exp * self.cfg.other[1].need_coin_100_exp / 100
	num = math.floor(num)
	num = num <= 0 and 0 or num
    return num
end

-- 魂器格子是否置灰
function PetWeaponData:IsGray(weapon)
	local is_sel = self:IsInSelectedBagCell(weapon)
	return is_sel
end

-- !!!!!!!!!!!!!!! 仓库选中逻辑有空优化下 !!!!!!!!!!!!!!!
-- 仓库是否可选中
function PetWeaponData:IsBagCanSel(status, item,cur_weapon)
	cur_weapon = cur_weapon or self:CurWeapon()
	if status == PetWeaponViewStatus.Normal then
		return false
	end


	local sel_count,weapon_count = self:GetSelListCount()
	if item:Type() == ItemType.PetWeapon then
		if weapon_count  >= self.max_sel_weapon_count then
			return false,string.format(Language.PetWeapon.BagSelTip1, self.max_sel_weapon_count)
		end
	else
		if sel_count  >= self.max_sel_count then
			return false,string.format(Language.PetWeapon.BagSelTip2, self.max_sel_count)
		end
	end
	

	if item.IsLock and item:IsLock() then
		return false,Language.PetWeapon.BagSelTip3
	end
	
	if status == PetWeaponViewStatus.Refine then
		
		if cur_weapon.item_id ~= item.item_id then
			return false,Language.PetWeapon.BagSelTip4
		end
		if cur_weapon:RefineLevel() == cur_weapon:MaxRefineLevel() then
			return false,Language.PetWeapon.BagSelTip5
		end
		return true,1
	end

	if status == PetWeaponViewStatus.Intensify then
		-- 是否经验溢出
		-- local sel_exp = self:GetSelListItemPriceExp() + self:GetItemPriceExp(item)
		-- local max_exp = self:GetIntensifyNeedExp(cur_weapon)
		-- if sel_exp > max_exp then
		-- 	return false, Language.PetWeapon.BagSelTip6
		-- end

		-- 是否需要突破
		if cur_weapon:IsNeedBreak() then
			return false, Language.PetWeapon.BagSelTip8
		end

		-- 检查下能放多少个，为0就不能放
		if cur_weapon:IsFullLevel() then
			return false, Language.PetWeapon.Error8
		end
		local num = self:GetStuffCanExpNum(item, cur_weapon)
		if num > 0 then
			return true,num
		else
			return false,Language.PetWeapon.BagSelTip7
		end
	end
end

-- 获取经验材料可以放几个材料到材料列表
function PetWeaponData:GetStuffCanExpNum(item, weapom)
	local max_exp = self:GetIntensifyNeedExp(weapom)
	local exp
	if item:Type() == ItemType.PetWeapon then
		exp = PetWeaponData:GetPriceExp(item)
	else
		exp = self.weapon_stuff_cfg[item.item_id].item_exp
	end
	local exp_pool = self:GetSelListItemPriceExp()--已经放入了多少经验
	if max_exp <= exp_pool then
		return 0
	end
	if item:Type() == ItemType.PetWeapon then
		return 1
	end
	local num = math.ceil((max_exp - exp_pool)/exp)
	num = num > item.num and item.num or num
	return num
end

-- 获取筛选列表数据
function PetWeaponData:GetSelListData(is_all)
	local list_data = {}
	if is_all then
		list_data = {{qua = 0}}
	end
	for i,v in pairs(self.cfg.pet_weapon_quality) do
		table.insert(list_data,{qua = v.quality})
	end
	return list_data
end

-- 获取材料筛选列表数据
function PetWeaponData:GetStuffSelListData(is_all)
	local list_data = {}
	if is_all then
		list_data = {{qua = 0, str = Language.PetWeapon.StuffQuaDesc[0]}}
	end
	for i = 3 , 6 do
		table.insert(list_data,{qua = i, str = Language.PetWeapon.StuffQuaDesc[i]})
	end
	return list_data
end

-- 获取升级材料信息
function PetWeaponData:GetIntensifyStuffInfo()
	local list = self.bag_selected_list.cell_list
	local stuff_info = {}
	local dialog_tip
	for i,num in pairs(list) do
		if num > 0 then
			local info = {}
			info.bag_colum = i.column_type
			info.bag_idx = i.index
			info.bag_num = num
			table.insert(stuff_info, info)
			if not dialog_tip then
				if i:Type() == ItemType.PetWeapon then
					local level = i:Level()
					local qua = i:QuaType()
					if qua >= 5 then
						dialog_tip = Language.PetWeapon.Tip10
					elseif level > 1 then
						dialog_tip = Language.PetWeapon.Tip11
					end
				end
			end
		end
	end
	return stuff_info, dialog_tip
end

-- 获取仓库格子选中的个数
function PetWeaponData:GetSelListCount()
	local count = 0
	local weapon_count = 0
	for i,v in pairs(self.bag_selected_list.cell_list or {}) do
		if v == true then
			count = count + 1
			if i:Type() == ItemType.PetWeapon then
				weapon_count = weapon_count + 1
			end
		end
	end
	return count,weapon_count
end

-- 获取仓库格子选中的item列表
function PetWeaponData:GetSelListItem()
	local list = {}
	for i,v in pairs(self.bag_selected_list.cell_list or {}) do
		if v > 0 then
			if i:Type() == ItemType.PetWeapon then
				table.insert(list,i)
			else
				local vo = i.vo
				vo.num = v
				vo.item_id = i.item_id
				local item = Item.Create(vo)
				item.key_item = i
				table.insert(list,item)
			end
		end
	end
	return list
end

-- 获取强化材料列表
function PetWeaponData:GetIntensifyStuffList()
	local list = self:GetSelListItem()
	local new_list = list
	local min_count = 5
	local add_count = 0
	if #list >= min_count then
		add_count = 1
	else
		add_count = min_count - #list
	end
	for i = 1,add_count do
		table.insert(new_list, Item.Create({item_id = 0}))
	end
	return new_list
end

-- 获取选中的item列表值多少经验
function PetWeaponData:GetSelListItemPriceExp()
	if self.sel_list_item_price_exp == nil then
		local item_list = self.bag_selected_list.cell_list or {}--self:GetSelListItem()
		local exp = 0
		for i,v in pairs(item_list) do
			exp = exp + self:GetItemPriceExp(i, v)
		end
		self.sel_list_item_price_exp = exp
	end
	return self.sel_list_item_price_exp
end

-- 单个物品值多少经验
function PetWeaponData:GetItemPriceExp(item, num)
	num = num or item.num
	local exp = 0
	if item.PriceExp then
		exp = item:PriceExp()
	elseif self.weapon_stuff_cfg[item.item_id] then
		exp = self.weapon_stuff_cfg[item.item_id].item_exp * num
	end
	return exp
end

function PetWeaponData:ToAttriItem3Data(cur_attri_list, target_attri_list)
	local list = {}
	target_attri_list = target_attri_list or {}
	for i,v in pairs(cur_attri_list) do
		local data = {}
		data.name = Language.AttrItems.AttrNames[v.attri_type]
		data.value = v.attri_value
		if target_attri_list[i] then
			data.add_value = target_attri_list[i].attri_value -- - data.value
		else
			data.add_value = data.value
		end
		data.attri_type = v.attri_type
	    table.insert(list, data)
	end
	list = self:AttriSort(list)
	return list
end

-- 魂器升级加成的属性
function PetWeaponData:GetIntensifyAddAttri(weapon, level)
	local attri_list = {}
	local qua_type = weapon:QuaType()
	local list = weapon:AttriList()
	for i,v in pairs(list) do
		local cfg
		for j,v2 in pairs(self.intensify_attri_group[qua_type] or {}) do
			if v2.quality == qua_type and v2.att_type == i then
				cfg = v2
				break
			end
		end
		--local cfg = Cfg.PetWeaponIntensifyAttriCfg(qua_type, i)
		if cfg then
			attri_list[i] = {attri_type = i, attri_value = v.attri_value + cfg.intensify_att_min * level}
		else
			attri_list[i] = {attri_type = i, attri_value = v.attri_value}
		end
	end
	return attri_list
end

-- 魂器突破加成的属性
function PetWeaponData:GetBreakAddAttri(weapon, count)
	count = count or 1
	local attri_list = {}
	local qua_type = weapon:QuaType()
	local list = weapon:AttriList()
	for i,v in pairs(list) do
		local cfg
		for j,v2 in pairs(self.break_attri_group[qua_type] or {}) do
			if v2.quality == qua_type and v2.att_type == i then
				cfg = v2
				break
			end
		end
		--local cfg = Cfg.PetWeaponBreakAttriCfg(qua_type, i)
		if cfg then
			attri_list[i] = {attri_type = i, attri_value = v.attri_value + cfg.intensify_att_min * count}
		else
			attri_list[i] = {attri_type = i, attri_value = v.attri_value}
		end
	end
	return attri_list
end

-- 属性排序
function PetWeaponData:AttriSort(attri_list)
	local t = TableCopy(attri_list)
	table.sort(t, function (a, b)
		return a.attri_type < b.attri_type
	end)
	return t
end

-- 获取全部魂器数据 包括宠物身上的
function PetWeaponData:GetAllWeaponListData(is_sel)
	local bag_weapon_list = BagData.Instance:GetWeaponList(is_sel)
	local pet_weapon_list = PetData.Instance:GetWeaponList()
	local list = AndList(pet_weapon_list, bag_weapon_list)
	
	local pet = PetData.Instance:GetCurSelect() or {}
	-- 排序
	self:SortWeaponList(list, self.replace_sort_info.sort_type, self.replace_sort_info.sel_qua, pet.index)
	return list
end

-- 设置weapon供精炼，强化使用
function PetWeaponData:SetCurCtrlWeapon(weapon)
	self.cur_ctrl_weapon = weapon
end

function PetWeaponData:GetCurCtrlWeapon()
	return self.cur_ctrl_weapon
end
function PetWeaponData:SetWeaponIdx(index)
	--LogDG("当前选择魂器格子 = ", index)
	self.weapon_idx = index or 0
end

function PetWeaponData:GetWeaponIdx()
	return self.weapon_idx
end

-- 获取全部精炼材料
function PetWeaponData:GetAllRefineStuffList(item)
	local item_id = item.item_id
	local all_weapon = BagData.Instance:GetWeaponList() or {}
	local list = {}
	for i,v in pairs(all_weapon) do
		if v.item_id == item_id and not v:IsLock() and v.index ~= item.index then
			table.insert(list, v)
		end
	end
	if #list > 0 then
		table.sort(list, function (a,b)
			return a:RefineLevel() < b:RefineLevel()
		end)
	end
	return list
end

-- 是否在选中的魂器精炼材料或强化材料里面
function PetWeaponData:IsStuff(item)
	-- 强化材料
	local list = self.bag_selected_list.cell_list or {}
	for i,v in pairs(list) do
		if i.index == item.index then
			return true
		end
	end
	-- 精炼材料
	if self.sel_refine_stuff and self.sel_refine_stuff.index == item.index then
		return true
	end
	return false
end

-- 获取可兑换魂器列表
function PetWeaponData:GetConvertDataList(show_type)
	show_type = show_type or 1
	local cfg = self.cfg.pet_weapon_exchange
	local list = {}
	for i,v in pairs(cfg) do
		if v.is_show == show_type then
			table.insert(list, v)
		end
	end
	return list
end

-- 获取魂器最大的属性
function PetWeaponData:GetWeaponMaxAttri(weapon, max_level, max_break_level)
	max_level = max_level or weapon:BaseCfg().level_max
	local add_list = self:GetIntensifyAddAttri(weapon, max_level - 1)
	max_break_level = max_break_level or weapon:MaxBreakLevel()
	local break_add_list = self:GetBreakAddAttri(weapon, max_break_level)

	local attri_list = weapon:AttriList()

	local list = {}
	for i,v in pairs(add_list) do
		local data = {}
		data.attri_type = v.attri_type
		data.attri_value = v.attri_value + break_add_list[i].attri_value - attri_list[i].attri_value
		list[i] = data
	end
	
	return list
end

-- 获取可置换的列表
function PetWeaponData:GetChangeList(weapon)
	local list = {}
	local qua = weapon:QuaType()
	if qua > 4 then
		for i,v in pairs(self.cfg.pet_weapon_cfg) do
			if v.is_show == 1 and v.quality == qua and v.pet_weapon_item ~= weapon.item_id then
				table.insert(list, v)
			end
		end
	end
	return list
end

-- 获取背包能置换的魂器
function PetWeaponData:GetChangeWeaponList()
	local all_weapon = self:GetAllWeaponListData(true) or {}
    local list_data = {}
    for i,v in pairs(all_weapon) do
        if v:QuaType() >= 5 then
            table.insert(list_data, v)
        end
    end
    return list_data
end

-- 通过宠物id获取专属魂器
function PetWeaponData:GetExclusiveWeaponByPetId(pet_id)
	for i,v in pairs(self.cfg.pet_weapon_cfg) do
		if v.pet_weapon_pet_id == pet_id then
			local weapon = PetWeapon.New({item_id = v.pet_weapon_item})
			weapon.param.use_pet_id = pet_id
			return weapon
		end
	end
end


-- 魂器红点逻辑
function PetWeaponData:GetRemind()
	local pet_list = PetData.Instance:GetPetList()
	for i,pet in pairs(pet_list) do
		local remind_num = self:GetPetRemind(pet)
		if remind_num > 0 then
			return 1
		end
		if pet:IsLianYao() then
			remind_num = self:GetPetRemind(pet,pet:Weapon2())
			if remind_num > 0 then
				return 1
			end
		end
	end
	return 0
end

-- 获取宠物身上的魂器红点
function PetWeaponData:GetPetRemind(pet, weapon)
	local is_open = FunOpen.Instance:GetFunIsOpened(Mod.PetWeapon.Main)
	if not is_open then
		return 0
	end
	if pet == nil then
		return 0
	end
	local state = pet:State()
    if state ~= PetState.Fight and state ~= PetState.StandBy then
        return 0
	end
	local weapon = weapon or pet:Weapon()
	if TableIsEmpty(weapon) then
		if not TableIsEmpty(BagData.Instance.item_list[ItemColumnType.PetWeapon]) then
			return 1
		end
		return 0
	end
	if self:GetReplaceRemind(weapon) > 0 then
		return 1
	end
	if self:GetIntensifyRemind(weapon) > 0 then
		return 1
	end
	if self:GetRefineRemind(weapon) > 0 then
		return 1
	end
	return 0
end

-- 获取替换红点
function PetWeaponData:GetReplaceRemind(weapon)
	if not self:IsCanRemind(weapon) then
		return 0
	end
	if self:IsEffect(weapon) then
		return 0
	end
	local use_pet_id = weapon:PetId()
	local cfg = PetData.Instance:GetPetCfg(use_pet_id)
	local old_id = cfg.prototype_id

	-- 炼妖信息
	local is_yao = weapon:IsLianYao()
	local lian_yao_id
	local yao_old_id
	local other_weapon
	if is_yao then
		lian_yao_id = weapon:YaoPetId()
		yao_old_id = PetData.Instance:GetPetCfg(lian_yao_id).prototype_id
		local pet = PetData.Instance:GetPetInfo(weapon:PetIndex())
		local weapon_1 = pet:Weapon() or {}
		local weapon_2 = pet:Weapon2() or {}
		other_weapon = weapon_1.item_id == weapon.item_id and weapon_2 or weapon_1
		-- if not TableIsEmpty(other_weapon) then
		-- 	if not self:IsEffect(other_weapon) then
		-- 		other_weapon = {item_id == 0}
		-- 	end 
		-- end
	end
	

	if not is_yao then
		for _, item in pairs(BagData.Instance.item_list[ItemColumnType.PetWeapon]) do
			local weapon_type, pet_id = item:WeaponType()
			if pet_id == use_pet_id or pet_id == old_id then
				return 1,item.item_id
			end
		end
	else
		local item_list = {}
		local target_list = {}
		for _, item in pairs(BagData.Instance.item_list[ItemColumnType.PetWeapon]) do
			local weapon_type, pet_id = item:WeaponType()
			if pet_id == use_pet_id or pet_id == old_id then
				if not item_list[item.item_id] and item.item_id ~= other_weapon.item_id then
					table.insert(target_list,item.item_id)
				end
				item_list[item.item_id] = 1
			elseif pet_id == lian_yao_id or pet_id == yao_old_id then
				if not item_list[item.item_id] and item.item_id ~= other_weapon.item_id then
					table.insert(target_list,item.item_id)
				end
				item_list[item.item_id] = 1
			end
		end
		if not TableIsEmpty(target_list) then
			return 1, target_list[1],target_list[2]
		end
	end
	return 0
end

-- 获取强化红点
function PetWeaponData:GetIntensifyRemind(weapon)
	if not self:IsCanRemind(weapon) then
		return 0
	end
	local flag = self:GetIntensifyFlag(weapon)
	if flag > 0 then
		return 1
	end
	return 0
end

-- 获取精炼红点
function PetWeaponData:GetRefineRemind(weapon)
	if not self:GetRefineToggleFlag() then
		return 0
	end
	if not self:IsCanRemind(weapon) then
		return 0
	end
	if not weapon:NextRefineCfg() then
		return 0
	end
	local num = Item.GetNum(weapon.item_id)
	if num == 0 then
		return 0
	end
	return 1
end

-- 魂器是否支持触发红点
function PetWeaponData:IsCanRemind(weapon)
	if not weapon then
		return false
	end
	if weapon.PetIndex == nil then
		return false
	end
	-- 是否被宠物穿过
	local pet_index = weapon:PetIndex()
	if not pet_index or pet_index < 0 then
		return false
	end
	local pet = PetData.Instance:GetPetInfo(pet_index)
	local state = pet:State()
	if state ~= PetState.Fight and state ~= PetState.StandBy then
        return false
	end
	return true
end

-- 魂器技能是否生效
function PetWeaponData:IsEffect(weapon)
	local weapon_type, pet_id = weapon:WeaponType()
	if weapon_type == PetWeaponType.Only then
		local pet_index = weapon:PetIndex()
		if not pet_index then
			return true
		end
        local mutant_id = PetData.Instance:GetMutantId(pet_id)
        local mutant_id_2 = PetData.Instance:GetMutantId(pet_id,2)
		local use_pet_id = weapon:PetId()

		if use_pet_id ~= pet_id and use_pet_id ~= mutant_id and use_pet_id ~= mutant_id_2 then
			if weapon:IsLianYao() then
				local yao_id = weapon:YaoPetId()
				if yao_id ~= pet_id and yao_id ~= mutant_id and yao_id ~= mutant_id_2 then
					return false
				end

				local pet = PetData.Instance:GetPetInfo(weapon:PetIndex())
				local weapon_1 = pet:Weapon() or {}
				local weapon_2 = pet:Weapon2() or {}
				if weapon_1.item_id == weapon_2.item_id then
					return false
				end
			else
				return false
			end
		end
		return true
	end
	return true
end

-- 魂器强化标记 1 可升级 ，2 可突破，0，材料不足，-1 已满级 exp自己外部计算好经验传进来判断，去除重复计算
function PetWeaponData:GetIntensifyFlag(weapon, has_exp)
	if weapon:IsFullLevel() then
		return -1
	end
	if weapon:IsNeedBreak() then
		local break_cfg = weapon:BreakCfg()
		for i,item_info in pairs(break_cfg.item_list) do
			local num = Item.GetNum(item_info.item_id)
			if num < item_info.num then
				return 0
			end
		end
		return 2
	end

	local num1 = weapon:Exp()
    local num2 = weapon:LevelCfg().exp
	local need_exp = num2 - num1
	local has_exp = has_exp or self:GetBagStuffExp()
	if has_exp >= need_exp then
		return 1
	else
		return 0
	end
end

-- 获取背包有多少材料经验
function PetWeaponData:GetBagStuffExp()
	local has_exp = 0
	for i,v in pairs(self.weapon_stuff_cfg) do
		local num = Item.GetNum(v.item_id)
		local price_exp = v.item_exp * num
		has_exp = has_exp + price_exp
	end
	return has_exp
end

function PetWeaponData:GetStuffItemList()
	local list = {}
	for i,v in pairs(self.weapon_stuff_cfg) do
		table.insert(list,v.item_id)
	end
	return list
end

function PetWeaponData:GetRefineToggleFlag()
	local flag = UnityPlayerPrefs.GetInt("GetRefineToggleFlag")
	if not flag or flag == 0 then
		return true
	end
	return flag == 1
end

function PetWeaponData:SetRefineToggleFlag(bool)
	local flag = bool and 1 or -1
	UnityPlayerPrefs.SetInt("GetRefineToggleFlag", flag)
end