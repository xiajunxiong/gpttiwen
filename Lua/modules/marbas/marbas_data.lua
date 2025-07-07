-- 造化装备·data组件
MarbasData = MarbasData or BaseClass()

-- 1640请求类型
MarbasReqType = {
	Info = 0,		--请求下发
	PutOn = 1,		--穿戴，p1：index
	TakeOff = 2,	--脱下，p1：equip_type
	Order = 3,		--整理背包
	Build = 4,		--打造，p1：合成表的seq，p2：是否穿戴，p3：index或者equip_type
	Resolve = 5,	--分解，p1：index
	ActiveApp = 6,	--激活外观，p1：外观id
	Wear = 7,       --穿戴外观，p1：外观id
	WearOff = 8,    --脱下外观
}

-- 4800请求类型
MarbasReqTypeP = {
	Refine = 0,			--精炼，p1：equip_type
	Force = 1,			--魂力，p1：item_src，p2：index，p3：column
	BatheDraw = 2,		--珑铸抽奖，p1：equip_type，p2：slot_index（0~5），p3：次数1或10
	BatheChange = 3,	--珑铸替换，p1：equip_type，p2：slot_index（0~5），p3：词条index（1~10）-1
	BatheRemove = 4,	--珑铸清除一个，p1：equip_type，p2：slot_index（0~5），p3：词条index（1~10）-1
	BatheRemoveAll = 5,	--珑铸清除全部，p1：equip_type，p2：slot_index（0~5）
	BatheUnLock = 6,	--珑铸解锁孔位，p1：equip_type，p2：slot_index（3~4），前三个默认解锁
	Demons = 7,         --入魔 p1 item_src,p2 index
	GemDecompose = 8,   --分解元魂珠 p1 index, p2 column
	GemCompose = 9,     --合成元魂珠 p1 item_id（目标
}

-- 元魂穿脱
BloodGemType = {
	Put_On = 0,
	Take_Off = 1,
}

-- 4802 4802 元魂，装备在哪里
BloodSrcType = {
    Bag = 0,        --背包里
    Equiped = 1,    --装备里-装备在身上
    EquipBag = 2,   --在造化包里(包括在造化装备上)
}

function MarbasData:__init()
	if MarbasData.Instance ~= nil then
		Debuger.LogError("[MarbasData] attempt to create singleton twice!")
		return
	end
	MarbasData.Instance = self
	
	self.config = Config.zaohua_equip_up_auto
	self.other = self.config.other[1]

	-- item_list 可以为空 但item_grid 是常驻实例 
	self.item_list = SmartData.New()
	self.item_grid = SmartData.New()
	-- 注意装备格子信息必须是 SmartData.New(Item.New())
	for i = 0,MarbasConfig.MaxBag-1 do 
		local vo = SmartData.New(Item.New())
		vo.index = i

		self.item_grid[i] = vo
	end 

	-- 基础
	self.equip_list = SmartData.New()
	self.base_view_data = SmartData.New({suit_show = true})
	self.suit_enable = SmartData.New({list = {},flush = false,changes = nil})
	self.better_equip = SmartData.New({is_better = false,mark_str = ""})
	self.base_remind = SmartData.New() -- 先写着

	-- 造化打造红点用
	self.build_remind = SmartData.New()
	self.build_info = SmartData.New()
	-- 这里存放了item_id对应的数量，包括身上穿的也算一件
	self.item_num_data = {}

	-- 造化精炼红点用
	self.refine_remind = SmartData.New()
	self.refine_info = SmartData.New()

	-- 造化珑铸红点用
	self.bathe_remind = SmartData.New()
	self.bathe_info = SmartData.New()

	-- 当前显示的panel和选中的装备，目前只有精炼和珑铸用
    self.left_equip_info = SmartData.New()

	self.blood_view_data = SmartData.New({select = nil,select_index = -1,select_gem = nil,GemEff = false,param_send = nil})

	-- 罡气·界面信息
	self.vigorous_view_data = SmartData.New({select_data = nil,mark_id = nil})
	-- 罡气·当前信息
	self.vigorous_info = SmartData.New()

	self.demons_view_data = SmartData.New({flush = false})

	self.marbas_view_data = SmartData.New({is_block = false})

	self.common_info = SmartData.New({flush = false})

	self:ResetAll()
	self:InitConfig()

	self.unlock_mark = false
end

function MarbasData:InitConfig()
    self.suit_cfg_list = {}
	for k,v in pairs(Config.zaohua_equip_up_auto.equip_suit) do 
		local cfg_il_link = self.CfgLink({v.zaohua_equip_suit,v.limit_prof},2)
		local cfg_cls_link = self.CfgLink({v.color,v.level_show,v.suit_num},3)
		if self.suit_cfg_list[cfg_il_link] == nil then 
            self.suit_cfg_list[cfg_il_link] = {}
        end 
		self.suit_cfg_list[cfg_il_link][cfg_cls_link] = v
	end

	self.Pearl_List = KeyList(Config.zaohua_equip_up_auto.yuan_soul_pearl,"yuan_soul_id")
	self.Pearl_Up_List = KeyList(Config.zaohua_equip_up_auto.yuan_soul_pearl,"next_level_id")

	self.vigorous_show_list = {}
	for k,v in pairs(self.config.vigorous) do 
		if self.vigorous_show_list[v.limit_prof] == nil then 
			self.vigorous_show_list[v.limit_prof] = {}
		end
		local page = math.ceil(v.level / MarbasConfig.PageNum)

		if self.vigorous_show_list[v.limit_prof][page] == nil then 
			self.vigorous_show_list[v.limit_prof][page] = {}
		end 

		table.insert(self.vigorous_show_list[v.limit_prof][page],v)
	end 

	for k,v in pairs(self.vigorous_show_list) do 
		for i,j in pairs(v) do 
			table.sort(j, function (a, b)
				local a_point = a.zaohua_equip_suit * 1000 + a.level * 10
				local b_point = b.zaohua_equip_suit * 1000 + b.level * 10
				return b_point > a_point
			end)
		end 
	end
	
	Cfg.Register("MarbasBuildTab", Cfg.RealTimeSearch2Key(self.config.equip_make_tabs, "equip_grade", "equip_level"))
	Cfg.Register("MarbasBatheAffixLimit", Cfg.RealTimeSearch(self.config.affix_limit, "equipment_level"))
	Cfg.Register("MarbasBatheAffix", Cfg.RealTimeSearch(self.config.cast, "equipment_id"))
	Cfg.Register("MarbasBatheAffixQua", Cfg.RealTimeSearch(self.config.affix_quality, "level"))
end

function MarbasData:ClearData()
	self.item_grid:Clear()
	self.item_list = {}

	self.better_equip.is_better = false
	self.better_equip.mark_str = ""
end

-- 重登后重置
function MarbasData:ResetAll()
	self:ResetBuildInfo()
	self:ResetLeftEquipInfo()
	self:ResetRefineInfo()
	self:ResetBatheInfo()

	self.marbas_view_data.equiped_demons = nil
end

function MarbasData:ResetBuildInfo()
	self.build_info.prof_type = RoleData.Instance:GetRoleProfType()
	self.build_info.prof_type = self.build_info.prof_type == 0 and 100 or self.build_info.prof_type
	self.build_info.equip_grade = 2	--仙装1，魔装2
	self.build_info.level = 0
end

function MarbasData:ResetLeftEquipInfo()
	self.left_equip_info.mod_id = Mod.Marbas.Refine
	self.left_equip_info.part = 0
	self.left_equip_info.flush_flag = true	--用于只是刷新格子数据的flag
end

function MarbasData:ResetBatheInfo()
	self.bathe_info.now_select = 0 --当前选中第几个槽位0~4
	local info = {}
	-- 按照部位，数组从0开始到7
	for i = 0, 7 do
		info[i] = self:BatheOnePartInfo(i)
	end
	self.bathe_info.info = info
end

-- 单个部位的珑铸信息初始化
function MarbasData:BatheOnePartInfo(equip_type)
	local t = {}
	t.equip_type = equip_type                   --装备部位
	local slot_info = {}
	for i = 0, 4 do
		-- 前三个孔一定解锁
		slot_info[i] = self:BatheOneSlotInfo(i, i < 3 and 1 or 0)
	end
	t.slot_info = slot_info
	return t
end

-- 单个部位的单个孔信息初始化
function MarbasData:BatheOneSlotInfo(index, is_unlock)
	local t = {}
	t.index = index
	t.is_unlock = is_unlock	--是否解锁
	t.history_level = 0	--历史最高等级
	t.now_times = 0	--当前抽了多少升级的保底次数
	t.now_attr = self:BatheOneSlotAttrInfo(0)	-- 当前属性
	--10个备选属性
	local attr_list = {}
	for i = 1, 10 do
		attr_list[i] = self:BatheOneSlotAttrInfo(i)
	end
	t.attr_list = attr_list
	return t
end

-- 单个孔单个属性信息
function MarbasData:BatheOneSlotAttrInfo(index)
	local t = {}
	t.index = index
	t.affix_id = 0
	t.level = 0
	return t
end

-- 获取珑铸对应部位对应孔位是否解锁
function MarbasData:GetBatheOneSlotUnLock(equip_type, slot_index)
	return self.bathe_info.info[equip_type].slot_info[slot_index].is_unlock
end

function MarbasData:ResetRefineInfo()
	self.refine_info.info = {}
	-- 按照部位的精炼等级，数组从0开始到7
	for i = 0, 7 do
		self.refine_info.info[i] = 0
	end
	-- 当前共鸣阶段
	self.refine_info.resonance_level = 0
	-- 当前满足件数
	self.refine_info.num = 0
end

function MarbasData:BuildProfData()
	local t = {}
	local index = 1
	while Language.Common.Prof[index] do
		table.insert(t, {name = Language.Common.Prof[index], prof = index, prof_type = index * 100})
		index = index + 1
	end
	return t
end

-- 双表全更
function MarbasData:SetMarbasBag(protocol)	
	for i= 1 , MarbasConfig.MaxBag do 
		if protocol.item_count == 0 or i > protocol.item_count then 
			self.item_grid[i-1] = SmartData.New(MarbasEquip.New())
			self.item_grid[i-1].index = i-1
		else 
		    if protocol.item_list[i] ~= nil then 
			    local item = SmartData.New(Item.Create(protocol.item_list[i]))
		        self.item_list[protocol.item_list[i].index] = item
		        self.item_grid[protocol.item_list[i].index] = item
		    end 
	    end
	end

	self.item_grid:Notify()
	self.item_list:Notify()

	self:CalculateItemNum()
	-- 打造红点刷
	self.build_remind:Notify()
	-- 精炼红点刷
	self.refine_remind:Notify()
	-- 珑铸红点刷
	self.bathe_remind:Notify()

	self:CheckIsBetter(BETTER_CHECK_REASON.BAG_CHANGE)
end

-- 双表单更
function MarbasData:SetItemData(protocol)
	-- LogDG("健哥哥哦~",protocol.item_info.item_id,"的下发理由是",protocol.reason,protocol.put_reason)

	local mark_index = protocol.item_info.index
	local grid_item = self.item_grid[mark_index]

	if grid_item == nil then 
		-- LogDG("造化装备的单更失败了")
		return 
	end 


	-- 先走一次飘字
	if protocol.item_info.item_id ~= 0 and protocol.put_reason > 1 then
		local item_cell = Item.Create(protocol.item_info)
		PublicPopupCtrl.Instance:CenterI({item_id = item_cell.item_id, num = item_cell.num,
		color_str = item_cell:ColorStr(),item = item_cell,icon_id = item_cell:IconId()})
		ChatCtrl.Instance:ChannelChatCustom{item_get = {item = item_cell, num = item_cell.num}}
	end 

	self:FlushItemBefore()

	-- 实例装填
    grid_item.item_id = protocol.item_info.item_id
    grid_item.num = protocol.item_info.num
    grid_item.is_bind = protocol.item_info.is_bind
    grid_item.item_type = grid_item.GetType(grid_item.item_id) or ItemType.Normal
    grid_item.item_unique_id_1 = protocol.item_info.item_unique_id_1
    grid_item.item_unique_id_2 = protocol.item_info.item_unique_id_2
	grid_item.param = protocol.item_info.param

    local item = SmartData.New(Item.Create(protocol.item_info))
    item.item_type = grid_item.item_type
	self.item_grid[protocol.item_info.index] = item

    if grid_item.item_id ~= 0 then
		self.item_list[grid_item.index] = item
	else
		self.item_list[grid_item.index] = nil
	end 	


	self:ShowDemonsSuccess(item)
	self:CheckIsBetter(BETTER_CHECK_REASON.BAG_IN_CHANGE,mark_index)
	self:FlushItemAfter()

	self:CalculateItemNum()
	-- 打造红点刷
	self.build_remind:Notify()
	-- 精炼红点刷
	self.refine_remind:Notify()
	-- 珑铸红点刷
	self.bathe_remind:Notify()

	self.item_grid:Notify()
	self.item_list:Notify()

	
end

-- 更新前操作（接口预留
function MarbasData:FlushItemBefore() end

-- 更新后操作（接口预留
function MarbasData:FlushItemAfter() 
	self.unlock_mark = true
end

function MarbasData:SetCommonData(protocol)
	self.common_info.show_mo_zhuang = protocol.show_mo_zhuang
	self.common_info.flush = not self.common_info.flush
end

-- 内部调用
function MarbasData:ItemBetterCheck(item_grid,mark_param)
	local vo_mark = {}

	local limit_prof = RoleData.Instance:GetRoleProfType()
	local compare = mark_param.is_mark and mark_param.mark_item or item_grid:Compare()

	local attr_item_value = 0
	local attr_compare_value = 0

	local item_prof = item_grid:LimitProf()
	for i,j in pairs(item_grid:BaseAttr()) do 
		local rate_s = item_prof == limit_prof and BagData.Instance:GetRoleScoreRate()[j.attr_type] or 0
		attr_item_value = rate_s * (j.attr_value ) + attr_item_value
	end 

	if compare and compare.param and compare.LimitProf then 
		local compare_prof = compare:LimitProf()
		for i,j in pairs(compare:BaseAttr()) do 
			local rate_s = compare_prof == limit_prof and BagData.Instance:GetRoleScoreRate()[j.attr_type] or 0
			attr_compare_value = rate_s * (j.attr_value) + attr_compare_value
		end 
	end 

	if attr_item_value > attr_compare_value then 
		vo_mark.pos = item_grid.index
		vo_mark.equip_type = item_grid:EquipType() 
		vo_mark.value = attr_item_value
		vo_mark.mana = item_grid:CheckMana()
	elseif attr_item_value == attr_compare_value and 
	    (compare ~= nil and item_grid:CheckMana() > compare:CheckMana() and attr_item_value > 0) then 
		vo_mark.pos = item_grid.index
		vo_mark.equip_type = item_grid:EquipType() 
		vo_mark.value = attr_item_value
		vo_mark.mana = item_grid:CheckMana()
	end 

	return vo_mark
end

function MarbasData:CheckIsBetter(flush_reason,bag_index)
	local result_marks = Split(self.better_equip.mark_str,"|")
	local quick_item = bag_index ~= nil and self.item_grid[bag_index] or nil

	-- 快速检查·降低循环次数
    if flush_reason == BETTER_CHECK_REASON.BAG_IN_CHANGE and quick_item ~= nil and quick_item.item_id > 0 then 
		local mark_param = {}
		local quick_type = Equip.GetEquipType(quick_item.item_id) + 1
        local is_mark = result_marks[quick_type] ~= nil 
		local mark_item = is_mark and self.item_grid[tonumber(result_marks[quick_type])] or nil
		mark_param.is_mark = is_mark
        mark_param.mark_item = mark_item

		local better_mark = self:ItemBetterCheck(quick_item,mark_param)
		if better_mark.value and better_mark.value > 0 then 
            if tonumber(result_marks[better_mark.equip_type + 1]) > -1 then 
                self.item_grid[tonumber(result_marks[better_mark.equip_type + 1])].is_better = false 
            end 
            self.item_grid[better_mark.pos].is_better = true
            result_marks[better_mark.equip_type] = better_mark.pos
        end 

		local str_ = ""
		if result_marks then
			for k,v in pairs(result_marks) do 
				if tonumber(v) > -1 and not self.better_equip.is_better then 
                    self.better_equip.is_better = true
                 end 
				str_ = k ==#result_marks and str_.. v or str_ .. v.."|"
			end 
		end
        self.better_equip.mark_str = str_
	elseif flush_reason == BETTER_CHECK_REASON.BAG_OUT_CHANGE and quick_item ~=nil and quick_item.item_id == 0 then 
        return 
    elseif flush_reason == BETTER_CHECK_REASON.BAG_CHANGE and (quick_item == nil) then 
        -- return 
	end 
	
    local total_marks = {}
    for k,v in pairs(self.item_grid) do 
        if v.is_better then v.is_better = false end 
		if v.item_id > 0 then 
		    local better_mark = self:ItemBetterCheck(v,{})
		    if better_mark.value ~= nil and better_mark.value > 0 then 
		        table.insert( total_marks, better_mark)
		    end 
	    end
    end 

	local try_marks = {}
    if #total_marks == 0 then  
        self.better_equip.is_better = false 
	end 

    for k,v in pairs(total_marks) do 
        if not self.better_equip.is_better then 
            self.better_equip.is_better = true 
        end 

        if try_marks[v.equip_type] == nil then 
            local vo = {index = v.pos , value =v.value ,mana = v.mana}
            try_marks[v.equip_type] = vo
        elseif try_marks[v.equip_type].value < v.value then 
            local vo = {index = v.pos , value =v.value ,mana = v.mana}
            try_marks[v.equip_type] = vo
		elseif try_marks[v.equip_type].value == v.value and try_marks[v.equip_type].mana < v.mana then 
			local vo = {index = v.pos , value =v.value ,mana = v.mana}
            try_marks[v.equip_type] = vo
        end 
    end 

	local str_ = ""
    for i = 0,7 do 
        local pos_ =  try_marks[i] == nil and -1 or try_marks[i].index
        str_ = i == 7 and str_..pos_ or str_ ..pos_.."|"

        if pos_ > -1 then 
            self.item_grid[pos_ ].is_better = true
        end 
		
    end 
    self.better_equip.mark_str = str_
end

-- 装备列表更新
function MarbasData:SetEquipList(protocol)
	local item_list = {}
    for k, v in pairs(protocol.equipment_list) do
        item_list[v.index] = MarbasEquip.New(v)
        item_list[v.index].in_bag = false
    end
	self.equip_list:Set(item_list)

	self:CalculateItemNum()
	-- 打造红点刷
	self.build_remind:Notify()
	-- 精炼红点刷
	self.refine_remind:Notify()
	-- 珑铸红点刷
	self.bathe_remind:Notify()
	self:FlushEquipSuit()

	self.equip_list:Notify()
	self.item_grid:Notify()
	self.item_list:Notify()

	self:CheckIsBetter(BETTER_CHECK_REASON.EQUIP_CHANGE)
	self.unlock_mark = true
end

-- 装备列表单更新
function MarbasData:ChangeEquipList(protocol)
	if protocol.item_info.item_id == 0 then
		self.equip_list[protocol.item_info.index] = nil
	else 
		local equip = MarbasEquip.New(protocol.item_info)
		equip.in_bag = false		
		self.equip_list[equip.index] = equip      
	end

	self:CalculateItemNum()
	-- 打造红点刷
	self.build_remind:Notify()
	-- 精炼红点刷
	self.refine_remind:Notify()
	-- 珑铸红点刷
	self.bathe_remind:Notify()
	self:FlushEquipSuit(true)
	self:CheckIsBetter(BETTER_CHECK_REASON.EQUIP_CHANGE)
end

-- 罡气信息更新
function MarbasData:SetVigorousInfo(protocol)
	self.vigorous_info.wear_app_id = protocol.wear_app_id
	self.vigorous_info.flag = protocol.flag

	for k,v in pairs(self.vigorous_info.flag) do 
		if k == self.vigorous_view_data.mark_id then 
			self:EffActive()
		end 
	end 
end

function MarbasData:BaseTypeGetMainSuitName()
	local color = self.base_view_data.suit_show and MarbasConfig.QuaMaster or MarbasConfig.QuaEnter 

	-- 主套装直接显示8件套
	local ready_cfg = self:CompatibilitySuit(color,8)
	local effect_num = self:GetCfgEffectNumByCfg(ready_cfg)

	local mode_show = RoleData.Instance:GetRoleAvatarType() * 10  + (self.base_view_data.suit_show and 2 or 1)
	
	return string.format( Language.Marbas.SuitFormat, ready_cfg.god_print_name,effect_num.."/"..ready_cfg.suit_num),mode_show
end 

function MarbasData:BaseTypeGetMainModID()
	local master = 0 
	for k,v in pairs(self.equip_list) do 
		if v:Color() == MarbasConfig.QuaMaster then 
			master = master + 1
		end 
	end 
	return RoleData.Instance:GetRoleAvatarType() * 10  + (master >=8 and 2 or 1)
end 

-- 基础界面套装属性获取
function MarbasData:BaseTypeGetCurSuit(num)
	local color = self.base_view_data.suit_show and MarbasConfig.QuaMaster or MarbasConfig.QuaEnter 
	local ready_cfg = self:CompatibilitySuit(color,num)

		local vo = {}
	vo.title = string.format(Language.Marbas.SuitFormat,ready_cfg.god_print_name,ready_cfg.suit_num)

		vo.attr_list = {}
		for i = 1,3 do 
			local attr = {}
		    attr.attr_type = ready_cfg["attr_type_"..i]
	    	attr.attr_value = ready_cfg["attr_add_"..i]
    		attr.active = ready_cfg.act
			if attr.attr_value > 0 then 
			    -- attr.attr_value = attr.attr_value 
				table.insert(vo.attr_list,attr)
			end
		end 

	if ready_cfg.sp_id > 0 then 
		local cfg = Cfg.SpecialEffect(ready_cfg.sp_id)
			vo.sp_effect = Language.ItemInfo.ItemTitles.SuitSp..cfg.description
		end 
	vo.active = ready_cfg.act

	return vo
end

-- 套装更新
-- 获取link-key值
function MarbasData:LinkEquipList()
	local links_list = {}
	for k,v in pairs(self.equip_list) do 
		local cfg = v:Config()
		local vo = {}
		if cfg ~= nil then 
		    vo.il_link = self.CfgLink({cfg.suit_type,cfg.limit_prof},2)
		    vo.lc_link = self.CfgLink({cfg.level_show,cfg.color},2)
		    table.insert(links_list,vo)
		end 
		end 

	return links_list
end 

function MarbasData:FlushEquipSuit(change_check)
	local links_list = self:LinkEquipList()

	local fit_link_list = self:FixKeysNumByLinkKey(links_list)
	local effect_list = self:CatchEffectSuit(fit_link_list)
	local need = false
	-- LogError("?数量呢？",fit_link_list)
	if change_check then 
		local changes = nil
		for index,num in pairs(MarbasConfig.SuitNum) do 
			if self.suit_enable.list[num] ~= effect_list[num] then 
				changes = effect_list[num]
			end 
		end 


		need = changes ~= nil
        self.suit_enable.changes = changes
	else 
		self.suit_enable.changes = nil
	end 

	self.suit_enable.list = effect_list
	if need then 
	    self.suit_enable.flush = not self.suit_enable.flush
	end 
	-- LogError("?呐呐 检索结束了哦~！ ",effect_list)

	if need and change_check and not ViewMgr:IsOpen(MarbasView) then
		ViewMgr:OpenView(MarbasSuitShowView)
	end 
end

function MarbasData:ClearSuitEnableChanges()
	self.suit_enable.changes = nil
end

function MarbasData:GetTypeBaseShouldSuit()
	local flag_1 = self.suit_enable.list[MarbasConfig.SuitNum[1]] ~= nil 
	local flag_2 = flag_1 and self.suit_enable.list[MarbasConfig.SuitNum[1]].color == MarbasConfig.QuaMaster
	return flag_1 and flag_2
end

-- 从注入的link_key表中获取实际生效数量
-- 格式 list = { {il_link = str,lc_link = str }}
function MarbasData:FixKeysNumByLinkKey(links_list)
	-- 穿着列表
	local equiped_list = {}
	for k,v in pairs(links_list) do 
		if equiped_list[v.il_link] == nil then equiped_list[v.il_link] = {} end 
		if equiped_list[v.il_link][v.lc_link] == nil then equiped_list[v.il_link][v.lc_link] = 0 end 

		equiped_list[v.il_link][v.lc_link] = equiped_list[v.il_link][v.lc_link] + 1
	end 
	
	-- 记录并拆分
	local fit_list = {}
	-- il_link = id|limit
	for il_link,il_list in pairs(equiped_list) do 
		-- lc_link = level|color
		if fit_list[il_link] == nil then fit_list[il_link] = {} end 
		for lc_link,lc_num in pairs(il_list) do 
			if fit_list[il_link][lc_link] == nil then 
				fit_list[il_link][lc_link] = lc_num
			else
				fit_list[il_link][lc_link] = lc_num + fit_list[il_link][lc_link]
			end 
			local param = self.FixLink(lc_link)
			if param[2] == MarbasConfig.QuaMaster then 
				local plus_lc_link = self.CfgLink({param[1],MarbasConfig.QuaEnter},2)

				if fit_list[il_link][plus_lc_link] == nil then 
					fit_list[il_link][plus_lc_link] = lc_num
				else
					fit_list[il_link][plus_lc_link] = lc_num + fit_list[il_link][plus_lc_link]
				end 
			end 
		end 
	end 

	-- LogError("?数量记录",fit_list)
	-- 兼容数量
	local nums_list = {}
	-- il_link = id|limit
	for il_link,il_list in pairs(fit_list) do 
		-- lc_link = level|color
		if nums_list[il_link] == nil then nums_list[il_link] = {} end 
		for lc_link,lc_num in pairs(il_list) do 
			if nums_list[il_link][lc_link] == nil then nums_list[il_link][lc_link] = lc_num end
			for lc_check_link,lc_check_num in pairs(il_list) do 
			    local equal_flag = lc_link ~= lc_check_link
				local check_param = self.FixLink(lc_check_link)
				local param = self.FixLink(lc_link)

				-- 相同品质的情况下必须等级大于等于才能兼容
				-- LogError("?兼容检查！",lc_link,"兼容",lc_check_link,equal_flag)
				if check_param[1] >= param[1] and check_param[2] == param[2] and equal_flag then 
					nums_list[il_link][lc_link] = nums_list[il_link][lc_link] + lc_check_num
					-- LogError("成功兼容",lc_link,"兼容",lc_check_link,nums_list[il_link][lc_link])
				end 
			end 
		end 
	end
	-- LogError("?兼容数量",nums_list)

	return nums_list
end

function MarbasData:CatchEffectSuit(list)
	local suit_list = {}
	for il_link,il_list in pairs(list) do 
		for lc_link,lc_num in pairs(il_list) do 
			local param = self.FixLink(lc_link)
			-- 直接获取配置，并检查能否触发，能则留下
			for index ,num in pairs(MarbasConfig.SuitNum) do 
				local cls_link = self.CfgLink({param[2],param[1],num},3)
				local cfg = self.suit_cfg_list[il_link][cls_link]

				if cfg ~= nil and lc_num >= cfg.suit_num then 
					table.insert( suit_list, cfg)
				end 
			end 
		end 
	end 

	local effect_list = {}
	for index ,num in pairs(MarbasConfig.SuitNum) do 
		for k,v in pairs(suit_list) do 
			if v.suit_num == num then 
			    if effect_list[num] == nil then 
				    effect_list[num] = v
				elseif v.color > effect_list[num].color then 
					effect_list[num] = v
				elseif v.color == effect_list[num].color and v.level_show < effect_list[num].level_show then 
					effect_list[num] = v
				end
			end 
		end 
	end 

	return effect_list
end

-- 同色兼容位列表
function MarbasData:CompatibilitySuit(color,num)
	local mark_cfg = self.suit_enable.list[num]
	-- 已激活列表里有且颜色和数量都满足 直接返回
	if mark_cfg ~= nil and mark_cfg.color == color then 
		local fit_cfg = TableCopy(mark_cfg)
		fit_cfg.act = true 
		fit_cfg.level = mark_cfg.level
		fit_cfg.color = mark_cfg.color
		fit_cfg.suit_num = mark_cfg.suit_num
		fit_cfg.level_show = mark_cfg.level_show
		fit_cfg.zaohua_equip_suit = mark_cfg.zaohua_equip_suit
		fit_cfg.limit_prof = mark_cfg.limit_prof
		fit_cfg.full_potential = mark_cfg.full_potential
		fit_cfg.god_print_name_1 = mark_cfg.god_print_name_1
		for i = 1,7 do 
			fit_cfg["name"..i] = mark_cfg["name"..i]
			fit_cfg["potential_add_"..i] = mark_cfg["potential_add_"..i]
		end 
		return fit_cfg
		end  

	local result = nil
	-- 方案一，激活表检索所有配置，取其中对应颜色+最低的等级+对应数量
	local lowest_level = 9999
	local mark_cfg= nil 
	for num,cfg in pairs(self.suit_enable.list) do 
		if cfg.level_show < lowest_level then 
			lowest_level = cfg.level_show
			mark_cfg = cfg
		end
	end 

	-- 方案二，在方案一失败的情况下，直接搜索默认配置（id+limit+level+color+num)
	if lowest_level < 9999 then 
		local cfg_il_link = self.CfgLink({mark_cfg.zaohua_equip_suit,mark_cfg.limit_prof},2)
		local cfg_cls_link = self.CfgLink({color,mark_cfg.level_show,num},3)

		local cfg = self.suit_cfg_list[cfg_il_link][cfg_cls_link]
		result = {}
		result.level = cfg.level
		result.color = cfg.color
		result.suit_num = cfg.suit_num
		result.level_show = cfg.level_show
		result.zaohua_equip_suit = cfg.zaohua_equip_suit
		result.limit_prof = cfg.limit_prof
		result.full_potential = cfg.full_potential
		result.god_print_name_1 = cfg.god_print_name_1

		for i = 1,7 do 
			result["name"..i] = cfg["name"..i]
			result["potential_add_"..i] = cfg["potential_add_"..i]
		end 
	else 
		local limit = RoleData.Instance:GetRoleProfType()

		for cfg_il_link,cfg_list in pairs(self.suit_cfg_list) do 
			local param = self.FixLink(cfg_il_link)
			for cfg_cls_link,cfg in pairs(cfg_list) do 
				if param[2] == limit and cfg.level == 1 and cfg.color == color and cfg.suit_num == num then 
					result = cfg 
					result.level = cfg.level
					result.color = cfg.color
					result.suit_num = cfg.suit_num
					result.level_show = cfg.level_show
					result.zaohua_equip_suit = cfg.zaohua_equip_suit
					result.limit_prof = cfg.limit_prof
					result.full_potential = cfg.full_potential
					result.god_print_name_1 = cfg.god_print_name_1

					for i = 1,7 do 
						result["name"..i] = cfg["name"..i]
						result["potential_add_"..i] = cfg["potential_add_"..i]
					end 
					break
				end 
			end 
		end 
	end 


	local fit_cfg = TableCopy(result)
	fit_cfg.act = false 

	return fit_cfg
end

function MarbasData:BaseTypeGetTotalSuit()
	local color = self.base_view_data.suit_show and MarbasConfig.QuaMaster or MarbasConfig.QuaEnter
	local role_prof = RoleData.Instance:GetRoleProfType()

	local fit_list = {}
	for il_link,cfgs in pairs(self.suit_cfg_list) do
		for cls_link,p_cfgs in pairs(cfgs) do
			if p_cfgs.limit_prof == role_prof and p_cfgs.color == color and p_cfgs.is_show == 1 then 
				table.insert(fit_list,p_cfgs)
			end 
		end
	end 

	table.sort(fit_list, DataSorter.KeyLowerSorter("level_show"))	

	return fit_list
end 

function MarbasData:CheckSuitActStatus(data)
	local cur_act = self.suit_enable.list[8]

	if cur_act == nil then return 2 end 

	-- 完全相同为激活
	if cur_act.level_show == data.level_show and cur_act.color == data.color and 
	    cur_act.limit_prof == data.limit_prof and cur_act.zaohua_equip_suit == data.zaohua_equip_suit and
		cur_act.suit_num == data.suit_num then 

		return 1
	end 

	-- 比当前的等级和品质更高 为未激活
	if (cur_act.level_show < data.level_show or cur_act.color < data.color) and 
	    cur_act.limit_prof == data.limit_prof and cur_act.zaohua_equip_suit == data.zaohua_equip_suit and
		cur_act.suit_num == data.suit_num then 

		return 2
	end

	-- 比当前的等级和品质更低 为已激活
	if (cur_act.level_show > data.level_show or cur_act.color > data.color) and 
	    cur_act.limit_prof == data.limit_prof and cur_act.zaohua_equip_suit == data.zaohua_equip_suit and
		cur_act.suit_num == data.suit_num then 

		return 3
	end

	return 2 

end

function MarbasData:GetCfgEffectNumByCfg(cfg)
	local links_list = self:LinkEquipList()
	local fit_link_list = self:FixKeysNumByLinkKey(links_list)

	local cfg_il_link = self.CfgLink({cfg.zaohua_equip_suit,cfg.limit_prof},2)
	local cfg_lc_link = self.CfgLink({cfg.level_show,cfg.color},2)

	local num = 0
	for il_link,il_list in pairs(fit_link_list) do 
		for lc_link,lc_num in pairs(il_list) do 
			local param = self.FixLink(lc_link)
			if param[1] >= cfg.level_show and param[2] == cfg.color then 
				num = lc_num 
			end 
		end 
	end 

	if num > 8 then num = 8 end 
	return  num 
end

function MarbasData:IsEmptyEquipList()
	for k,v in pairs(self.equip_list) do 
		if v then 
			return false
		end 
	end 
	return true
end

-- 获取身上对应部位的装备，没有就为nil
function MarbasData:MarbasEquip(part)
	return self.equip_list[part]
end

function MarbasData:IsWithEquipInBag()
	for k,v in pairs(self.item_list) do 
		return true
	end 
	return false
end

-- 打造所需材料item_list，红点用
function MarbasData:BuildRemindItemList()
	if not self.build_item_id_list then
		self.build_item_id_list = {}
		local item_id_list = {}
		for k, v in pairs(self.config.equip_make) do
			if not item_id_list[v.stuff_id_2] then
				item_id_list[v.stuff_id_2] = v.stuff_id_2
			end
		end
		for k, v in pairs(item_id_list) do
			table.insert(self.build_item_id_list, v)
		end
	end
    return self.build_item_id_list
end

-- 打造总红点
function MarbasData:BuildAllRemind()
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Build) then
        return 0
	end
	-- 0是没勾，显示红点，新号默认没勾
	if self:PrefBuildRemind() == 0 and RoleData.Instance:GetRoleProfType() ~= 0 then
		-- 遍历本职业所有装备判断是否可以打造
		local prof_build_data = self:BuildData(RoleData.Instance:GetRoleProfType())
		for k, v in pairs(prof_build_data or {}) do
			for l, w in pairs(v) do
				for m, x in pairs(w) do
					if self:IsCanBuild(x.seq) then
						return 1
					end
				end
			end
		end
	end
	return 0
end

function MarbasData:BaseAllRemind()
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Base) then
        return 0
	end

	return self.better_equip.is_better and 1 or 0
end

function MarbasData:BloodAllRemind()
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Blood) then
        return 0
	end

	local cost_id = Config.zaohua_equip_up_auto.other[1].blood_sacrifice_consume
	local cost_need = Config.zaohua_equip_up_auto.other[1].consume_num
	local cost_num =  Item.GetNum(cost_id)
	for k,v in pairs(self.equip_list) do 
		if v:CheckMana() > 0 and cost_need <= cost_num and not v:IsMaxForce() then 
			return 1
		end 
	end 

	local gem_list = BagData.Instance:GetItemsByShowType(ShowType.MarbasGem)
	if #gem_list == 0 then return 0 end 
	for k,v in pairs(self.equip_list) do 
		for i = 1,4 do 
			local gem_info = v:GetStone(i)
			local is_lock = MarbasData.Instance:GetBloodSoltLock(v,i)
			if gem_info.item_id == 0 and is_lock then 
				return 1
			end 
		end 
	end 

	return 0
end

function MarbasData:VigorousAllRemind()
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Vigorous) then
        return 0
	end
	
	for i = 1,#MarbasConfig.LevelShow do 
		local level_show = MarbasConfig.LevelShow[i]		
		local level_check = self:GetVigorousLevelCheck(level_show)
		local cfg = self:GetVigorousCfgDetail(i,RoleData.Instance:GetRoleProf())
		local un_lock = self:GetVigorousUnlock(cfg.seq)

		if not un_lock and level_check == 8 then 
			return 1
		end 
	end 
	-- 

	return 0
end

-- 打造档次上红点
function MarbasData:BuildGradeRemind(equip_grade)
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Build) then
        return 0
	end
	if self:PrefBuildRemind() == 0 and RoleData.Instance:GetRoleProfType() ~= 0 then
		-- 遍历本职业对应档次的装备判断是否可以打造
		local prof_build_data = self:BuildData(RoleData.Instance:GetRoleProfType(), equip_grade)
		for k, v in pairs(prof_build_data) do
			for l, w in pairs(v) do
				if self:IsCanBuild(w.seq) then
					return 1
				end
			end
		end
	end
	return 0
end

-- 打造档次对应等级上的红点
function MarbasData:BuildLevelRemind(equip_grade, level)
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Build) then
        return 0
	end
	if self:PrefBuildRemind() == 0 and RoleData.Instance:GetRoleProfType() ~= 0 then
		-- 遍历本职业对应档次和装等的装备判断是否可以打造
		local prof_build_data = self:BuildData(RoleData.Instance:GetRoleProfType(), equip_grade, level)
		for k, v in pairs(prof_build_data) do
			if self:IsCanBuild(v.seq) then
				return 1
			end
		end
	end
	return 0
end

-- 打造用，是否选的是当前职业
function MarbasData:IsSelectMyProf()
	return self.build_info.prof_type == RoleData.Instance:GetRoleProfType()
end

function MarbasData:PrefBuildRemind(value)
	if value then
		PrefsInt(PrefKeys.MarbasBuildRemind(), value)
		self.build_remind:Notify()
	else
		return PrefsInt(PrefKeys.MarbasBuildRemind())
	end
end

-- 复合link
function MarbasData.CfgLink(param,num)
	local str = ""
	for i = 1,num do 
		str = str..param[i]..(i == num and "" or "|")
	end 
	return str
end

-- 解link
function MarbasData.FixLink(str)
	local list = Split(str,"|")
	local result = {}
	for i = 1, #list do 
		table.insert(result,tonumber(list[i]))
	end 
	return result
end

-- 打造用手风琴表
-- 修改备注：需求号58290：屏蔽仙装，is_show=0则不显示
-- 追加优化：将ipairs替换为pairs 并增加了对children的排序
function MarbasData:ABuildData()
	if not self.a_build_data then
		self.a_build_data = {}
		for k, v in pairs(Language.Marbas.Build.ATitle) do
			self.a_build_data[k] = {}
			self.a_build_data[k].name = v
			self.a_build_data[k].id = k
			self.a_build_data[k].children = {}
		end
		for i, v in pairs(self.config.equip_make_tabs) do
			if self.a_build_data[v.equip_grade] and v.is_show == 1 then
				local t = {}
				t.name = v.equip_level
				t.is_show = v.is_show
				table.insert(self.a_build_data[v.equip_grade].children, t)
			end
		end
		for i, v in pairs(self.a_build_data) do
			table.sort(v.children, DataSorter.KeyLowerSorter("name"))	
			local children_index = 0
			for j, w in ipairs(v.children) do
				children_index = children_index + 1
				w.children_index = children_index
				w.parent_index = v.id
			end
		end
	end
	return self.a_build_data
end

-- 打造的红点需要走这里判断是否能打造
function MarbasData:BuildItemIdIsOpen(item_id)
	local item = Item.Create({item_id = item_id})
	local equip_grade = item:EquipGrade()
	local level_show = item:LevelShow()
	local cfg = Cfg.MarbasBuildTab(equip_grade, level_show) or {}
	if not TableIsEmpty(cfg) then
		return cfg.is_show == 1
	end
	return false
end

-- 打造用信息列表，[职业枚举类型，>=100的][装备档次][装等展示][装备部位（0-7）]，是否要筛选出可打造的
function MarbasData:BuildData(prof_type, equip_grade, level_show, part, is_can_build)
	if not self.build_data then
		self.build_data = {}
		for k, v in pairs(self.config.equip_make) do
			local item = Config.immortal_equip_auto[v.target]
			if TableIsEmpty(item) then
				LogError("无法在物品表中找到造化装备打造表中的target"..v.target)
			else
				if not self.build_data[item.limit_prof] then
					self.build_data[item.limit_prof] = {}
				end
				if not self.build_data[item.limit_prof][item.equip_grade] then
					self.build_data[item.limit_prof][item.equip_grade] = {}
				end
				if not self.build_data[item.limit_prof][item.equip_grade][item.level_show] then
					self.build_data[item.limit_prof][item.equip_grade][item.level_show] = {}
				end
				self.build_data[item.limit_prof][item.equip_grade][item.level_show][item.equip_type] = v
			end
		end
	end
	if prof_type then
		if equip_grade then
			if level_show then
				if part then
					return self.build_data[prof_type][equip_grade][level_show][part]
				end
				if is_can_build then
					local t = {}
					-- 筛选出可打造的
					for k, v in pairs(self.build_data[prof_type][equip_grade][level_show]) do
						if self:IsCanBuild(v.seq) then
							table.insert(t, v)
						end
					end
					return t
				end
				return self.build_data[prof_type][equip_grade][level_show]
			end
			return self.build_data[prof_type][equip_grade]
		end
		return self.build_data[prof_type]
	end
	return self.build_data
end

-- 判断打造表对应的seq当前是否可以打造（第一个不足的道具id）
function MarbasData:IsCanBuild(seq)
	local one_build_cfg = self.config.equip_make[seq]
	-- 第一个一定是装备，第二个一定是道具
	if one_build_cfg == nil then return false end

	if not self:BuildItemIdIsOpen(one_build_cfg.target) then
		return false
	end

	if one_build_cfg.stuff_num_1 > 0 then
		if self:ItemNum(one_build_cfg.stuff_id_1) == 0 then
			return false, one_build_cfg.stuff_id_1
		end
	end
	local has_num = Item.GetNum(one_build_cfg.stuff_id_2)
	if has_num < one_build_cfg.stuff_num_2 then
		return false, one_build_cfg.stuff_id_2
	end
	
	local has_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
	if has_coin < one_build_cfg.price then
		return false, HuoBi[CurrencyType.CoinBind]
	end
	return true
end

-- 打造成功
function MarbasData:SetBuildResult(protocol)
	self.build_result = protocol
	self.complete_item = protocol.item
end

function MarbasData:ComposeResult()
    return self.build_result
end

-- 计算item_id对应数量，包括身上的
function MarbasData:CalculateItemNum()
	self.item_num_data = {}
	for k, v in pairs(self.equip_list) do
		if not self.item_num_data[v.item_id] then
			self.item_num_data[v.item_id] = 1
		else
			self.item_num_data[v.item_id] = self.item_num_data[v.item_id] + 1
		end
	end
	for k, v in pairs(self.item_list) do
		if not self.item_num_data[v.item_id] then
			self.item_num_data[v.item_id] = 1
		else
			self.item_num_data[v.item_id] = self.item_num_data[v.item_id] + 1
		end
	end
end

-- 返回该道具数量，身上穿的也算一件
function MarbasData:ItemNum(item_id)
	return self.item_num_data[item_id] or 0
end

-- 返回一件背包的item的index
function MarbasData:BagOneIndex(item_id)
	for k, v in pairs(self.item_list) do
		if v.item_id == item_id then
			return v.index
		end
	end
end

-- 判断是否装备了这个id的装备
function MarbasData:IsEquipThis(item_id)
	local cfg = Item.GetConfig(item_id)
	local equip_part = cfg.equip_type
	if self.equip_list[equip_part] then
		return self.equip_list[equip_part].item_id == item_id
	end
	return false
end

-- 整备一套指定品质，等级的全身8件套
function MarbasData:SearchYourMarbas(color,level)
	local prof = RoleData.Instance:GetRoleProfType()

	local list = {}
	for k,v in pairs(Config.immortal_equip_auto)do 
		if v.level_show == level and color == v.color 
		   and prof == v.limit_prof and list[v.equip_type+1] == nil then 
			list[v.equip_type+1] = v

			local num = 0
			for k,v in pairs(list) do 
				num = num + 1
			end 
			if num == 8 then 
				break
			end 
		end 
	end 

	return list
end

-- 精炼总红点
function MarbasData:RefineAllRemind()
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Refine) then
        return 0
	end
	for i = 0, 7 do
		if self:RefinePartRemind(i) > 0 then
			return 1
		end
	end
	return 0
end

-- 精炼单部位红点
-- 第二个是不足类型，1：功能未开启，2：未穿戴，3：装备等级不足，4：材料不足，5：精炼等级已满
-- 第三个参数是对应第二个参数作用，3：需要的装备等级，4：材料id
-- 第四个参数是对应第二个参数作用，4：材料所需数量
function MarbasData:RefinePartRemind(part)
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Refine) then
        return 0, 1
	end
	-- 穿戴
	if TableIsEmpty(self.equip_list[part]) then
		return 0, 2
	end
	-- 满级
	local next_refine_cfg = self:RefineCfg()[part][self.refine_info.info[part] + 1] or {}
	if TableIsEmpty(next_refine_cfg) then
		return 0, 5
	end
	-- 装等
	local level_show = self.equip_list[part]:LevelShow()
	if next_refine_cfg.level_show > level_show then
		return 0, 3, next_refine_cfg.level_show
	end
	-- 材料
	if next_refine_cfg.refine_consum_1 > 0 then
		local has = Item.GetNum(next_refine_cfg.refine_consum_1)
		if has < next_refine_cfg.refine_consum_num_1 then
			return 0, 4, next_refine_cfg.refine_consum_1, next_refine_cfg.refine_consum_num_1 - has
		end
	end
	if next_refine_cfg.refine_consum_2 > 0 then
		local has = Item.GetNum(next_refine_cfg.refine_consum_2)
		if has < next_refine_cfg.refine_consum_num_2 then
			return 0, 4, next_refine_cfg.refine_consum_2, next_refine_cfg.refine_consum_num_2 - has
		end
	end
	return 1
end

-- 精炼所需材料item_list，红点用
function MarbasData:RefineRemindItemList()
	if not self.refine_item_id_list then
		self.refine_item_id_list = {}
		local item_id_list = {}
		for k, v in pairs(self.config.refine) do
			local index = 1
			while v["refine_consum_"..index] do
				if v["refine_consum_num_"..index] > 0 and not item_id_list[v["refine_consum_"..index]] then
					item_id_list[v["refine_consum_"..index]] = v["refine_consum_"..index]
				end
				index = index + 1
			end
		end
		for k, v in pairs(item_id_list) do
			table.insert(self.refine_item_id_list, v)
		end
	end
    return self.refine_item_id_list
end

-- 用于判断珑铸界面是否需要延迟刷新
function MarbasData:BatheNeedDelay(value)
	if value ~= nil then
		self.bathe_need_delay = value
	else
		return self.bathe_need_delay
	end
end

-- 用于珑铸选择界面播放出现特效，-1不需要，0全部，1最新
function MarbasData:BatheShowEffect(value)
	if value ~= nil then
		self.bathe_show_effect = value
	else
		return self.bathe_show_effect or -1
	end
end

-- 用于珑铸选择界面播放自身更换属性的特效
function MarbasData:BatheNeedShowNowEffect(value)
	if value ~= nil then
		self.bathe_need_show_now_effect = value
	else
		return self.bathe_need_show_now_effect
	end
end

-- 当前选中的孔位有多少可选属性
function MarbasData:BatheNowSlotSelectListNum(value)
	if value ~= nil then
		self.bathe_now_slot_select_list_num = value
	else
		return self.bathe_now_slot_select_list_num or 0
	end
end

-- 珑铸总红点
function MarbasData:BatheAllRemind()
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Bathe) then
        return 0
	end
	for i = 0, 7 do
		if self:BathePartRemind(i) > 0 then
			return 1
		end
	end
	return 0
end

-- 珑铸单部位红点
function MarbasData:BathePartRemind(part)
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Bathe) then
        return 0
	end
	if TableIsEmpty(self.equip_list[part]) then
		return 0
	end
	for i = 0, 4 do
		if self:BatheSlotUnLockRemind(part, i) > 0 then
			return 1
		elseif self:BatheSlotCastingRemind(part, i) > 0 then
			return 1
		elseif self:BatheSlotResultRemind(part, i) > 0 then
			return 1
		end
	end
	return 0
end

-- 珑铸单部位单孔位可解锁红点
function MarbasData:BatheSlotUnLockRemind(part, slot)
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Bathe) then
        return 0
	end
	if TableIsEmpty(self.equip_list[part]) then
		return 0
	end
	if self.bathe_info.info[part] == nil then 
		return 0
	end 
	if self.bathe_info.info[part].slot_info[slot] == nil then 
		return 0
	end 
	local affix_num = self:BatheAffixMaxNum(self.equip_list[part].item_id)
    local now_slot_info = self.bathe_info.info[part].slot_info[slot]
	if affix_num >= (now_slot_info.index + 1) and now_slot_info.is_unlock == 0 then
		return 1
	end
	return 0
end

-- 珑铸单部位单孔位可铸造红点
function MarbasData:BatheSlotCastingRemind(part, slot)
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Bathe) then
        return 0
	end
	if TableIsEmpty(self.equip_list[part]) then
		return 0
	end
	local now_slot_info = self.bathe_info.info[part].slot_info[slot]
	local affix_num = self:BatheAffixMaxNum(self.equip_list[part].item_id)
	-- 只有孔位已解锁，并且没有属性，并且已生效的时候才需要红点
	if now_slot_info.is_unlock == 1 and now_slot_info.now_attr.level == 0 and (now_slot_info.index + 1) <= affix_num then
		local has = Item.GetNum(self:BatheRemindItemList()[1])
		local need = Cfg.MarbasBatheAffixQua(now_slot_info.history_level).expends_num
		if has >= need then
			return 1
		end
	end
	return 0
end

-- 珑铸单部位单孔位查看结果红点
function MarbasData:BatheSlotResultRemind(part, slot)
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Bathe) then
        return 0
	end
	if TableIsEmpty(self.equip_list[part]) then
		return 0
	end
	local now_slot_info = self.bathe_info.info[part].slot_info[slot]
	local level = now_slot_info.now_attr.level or 0
	local affix_num = self:BatheAffixMaxNum(self.equip_list[part].item_id)
	-- 只有孔位已解锁并且已生效时才需要红点
	if now_slot_info.is_unlock == 1 and (now_slot_info.index + 1) <= affix_num then
		for k, v in pairs(self:BatheSlotSelectList(part, slot)) do
			if v.level > now_slot_info.now_attr.level then
				return 1
			end
		end
	end
	return 0
end

-- 珑铸单部位单孔位可选属性列表
function MarbasData:BatheSlotSelectList(part, slot)
	local attr_list = {}
	if not FunOpen.Instance:IsFunOpen(Mod.Marbas.Bathe) then
        return {}
	end
	if TableIsEmpty(self.equip_list[part]) then
		return {}
	end
	local now_slot_info = self.bathe_info.info[part].slot_info[slot]
	for i, v in ipairs(now_slot_info.attr_list) do
		if v.affix_id > 0 then
			local t = TableCopy(v, 1)
			t.name = self:BatheAffixAttr(v.affix_id, v.level)
			table.insert(attr_list, t)
		end
	end
	return attr_list
end

function MarbasData:GetBatheInfoByType(equip_type)
	return self.bathe_info.info[equip_type]
end 

-- 珑铸所需材料item_list，红点用
function MarbasData:BatheRemindItemList()
	if not self.bathe_item_id_list then
		self.bathe_item_id_list = {self.other.item_id}
	end
    return self.bathe_item_id_list
end

-- 左侧道具格子红点（部位）
function MarbasData:LeftEquipItemRemind(part)
	if self.left_equip_info.mod_id == Mod.Marbas.Refine then
		return self:RefinePartRemind(part)
	elseif self.left_equip_info.mod_id == Mod.Marbas.Bathe then
		return self:BathePartRemind(part)
	end
	return 0
end

-- 精炼表
function MarbasData:RefineCfg(prof_type)
	prof_type = prof_type or RoleData.Instance:GetRoleProfType()
	-- 防止报错，这里断网时会获取到0
	prof_type = prof_type == 0 and 100 or prof_type
	if TableIsEmpty(self.refine_cfg) then
		self.refine_cfg = DataHelper.Subdivision(self.config.refine, {"limit_prof", "hole_type", "refine_level"})
	end
	return self.refine_cfg[prof_type]
end

-- 精炼共鸣表
function MarbasData:RefineResonanceCfg(prof_type)
	prof_type = prof_type or RoleData.Instance:GetRoleProfType()
	-- 防止报错，这里断网时会获取到0
	prof_type = prof_type == 0 and 100 or prof_type
	if TableIsEmpty(self.refine_resonance_cfg) then
		self.refine_resonance_cfg = DataHelper.Subdivision(self.config.refine_resonante, {"limit_prof", "resonance_stage"})
	end
	return self.refine_resonance_cfg[prof_type]
end

-- 处理当前精炼表返回属性data_list，是否自己加颜色
function MarbasData:RefineAttrData(refine_cfg, color)
	local cfg = {}
	if TableIsEmpty(refine_cfg) then
		return {}
	end
	local index = 1
	while refine_cfg["attr_add_"..index] do
		if refine_cfg["attr_add_"..index] > 0 then
			local attr = CommonStruct.AttrStruct(refine_cfg["attr_type_"..index], refine_cfg["attr_add_"..index])
			attr.color = color
			attr.index = index
			table.insert(cfg, attr)
		end
		index = index + 1
	end
	return cfg
end

-- 处理当前精炼共鸣表返回属性data_list，是否自己加颜色
function MarbasData:RefineResonanceAttrData(refine_resonance_cfg, color)
	local cfg = {}
	if TableIsEmpty(refine_resonance_cfg) then
		return {}
	end
	local index = 1
	while refine_resonance_cfg["attr_add_"..index] do
		if refine_resonance_cfg["attr_add_"..index] > 0 then
			local attr = CommonStruct.AttrStruct(refine_resonance_cfg["attr_type_"..index], refine_resonance_cfg["attr_add_"..index])
			attr.color = color
			table.insert(cfg, attr)
		end
		index = index + 1
	end
	return cfg
end

function MarbasData:SetRefineInfo(protocol)
	self.refine_info.info = protocol.info
	-- 收到协议后会计算一遍共鸣阶段
	self.refine_info.resonance_level = 0
	self.refine_info.num = 8
	for i, v in ipairs(self:RefineResonanceCfg()) do
		local num = 0
		for j, w in pairs(protocol.info) do
			if w >= v.refine_level_all then
				num = num + 1
			end
		end
		self.refine_info.num = num
		if num ~= 8 then
			self.refine_info.resonance_level = v.resonance_stage - 1
			break
		else
			self.refine_info.resonance_level = v.resonance_stage
		end
	end
end

-- 珑铸登录或者孔位解锁全部下发
function MarbasData:SetBatheAllInfo(protocol)
	self.bathe_info.info = protocol.info
end

-- 珑铸单个孔位改变下发
function MarbasData:SetBatheSingleSlotInfo(protocol)
	self.bathe_info.info[protocol.equip_type].slot_info[protocol.slot_index] = protocol.info
	self.bathe_info:SetDirty("info")
end

function MarbasData:GetRefineInfo(part)
	return part and self.refine_info.info[part] or self.refine_info.info
end

-- 根据装备档次和部位返回大图片路径，职业不传为自身
function MarbasData:BigIconPath(equip_grade, part, prof_type)
	prof_type = prof_type or RoleData.Instance:GetRoleProfType()
	-- 防止报错，这里断网时会获取到0
	prof_type = prof_type == 0 and 100 or prof_type
	if not self.big_icon_cfg then
		self.big_icon_cfg = DataHelper.Subdivision(self.config.icon, {"limit_prof", "equip_grade", "hole_type"})
	end
	return Format("ui/textures/marbas/big_icon/%s.png", self.big_icon_cfg[prof_type][equip_grade][part].icon)
end

-- 返回珑铸词缀等级上限
function MarbasData:BatheAffixLimit(level_show)
	return Cfg.MarbasBatheAffixLimit(level_show).affix_limit
end

-- 返回珑铸词缀激活的最大词条数量
function MarbasData:BatheAffixMaxNum(item_id)
	local cfg = Cfg.MarbasBatheAffix(item_id)
	return cfg.affix_num or 0
end

-- 返回珑铸词缀等级上限
function MarbasData:BatheAffixMaxLevel(item_id)
	local cfg = Cfg.MarbasBatheAffix(item_id)
	return cfg.affix_limit or 0
end

-- 返回对应item_id的珑铸词缀id表
function MarbasData:BatheAffixCfg(item_id)
	local cfg = Cfg.MarbasBatheAffix(item_id)
	local group_list = DataHelper.SplitList(cfg.affix_group, "|")
	local affix_list = {}
	for i, v in ipairs(group_list) do
		table.insert(affix_list, self.config.affix_group[v])
	end
	return affix_list
end

-- 返回珑铸词缀的名字和属性表和icon名
function MarbasData:BatheAffixAttr(affix_id, level)
	if not self.bathe_affix_attr then
		self.bathe_affix_attr = {}
		for i, v in ipairs(self.config.affix_group) do
			local t = {}
			t.affix_id = v.affix_id
			t.name = v.name
			t.lowest_level = v.lowest_level
			t.affix_attribute = v.affix_attribute
			t.icon = v.icon
			t.attr_value = {}
			local index = 1
			while v["level_"..index] do
				t.attr_value[index] = v["level_"..index]
				index = index + 1
			end
			self.bathe_affix_attr[v.affix_id] = t
		end
	end
	if self.bathe_affix_attr[affix_id] then
		local attr = self.bathe_affix_attr[affix_id]
		return attr.name, CommonStruct.AttrStruct(attr.affix_attribute, attr.attr_value[level]), attr.icon
	end
end

-- 获得珑铸词缀的最大等级上限
function MarbasData:BatheAffixLevelMax()
	if not self.bathe_affix_level_max then
		local cfg = self.config.affix_group[1]
		local max = 1
		while cfg["level_"..max] do
			max = max + 1
		end
		max = max - 1
		self.bathe_affix_level_max = max
	end
	return self.bathe_affix_level_max
end

-- 计算属性评分，向下取整（属性列表）
function MarbasData:CalculateScore(list)
	local score = 0
	for k, v in pairs(list) do
        score = score + BagData.Instance:GetRoleScoreByAttr(v.attr_type, v.attr_value)
	end
	return math.floor(score)
end

-- 根据枚举类型获得背包格子组（选全部不筛掉空格子
function MarbasData:GetMarbasBag(fix)
	local paras = {}
	for k,v in pairs(self.item_grid) do 
		if v:Color() == fix then 
			table.insert(paras, v)
		elseif fix == -1 then 
			table.insert(paras, v)
		end 
	end 

	for i = MarbasConfig.MaxBag - #paras , 1 ,-1 do 
		table.insert(paras,MarbasEquip.New())
	end 

	-- local list = {}
	-- for k,v in pairs(paras) do 
	-- 	local vo = {}
	-- 	vo.item_id = v.item_id
	-- 	if v.param.ling_li_value ~= nil and v.param.ling_li_value > 0 then 
	-- 	    vo.yuan_hun_zhu = v.param.yuan_hun_zhu
	-- 	end 
	-- 	list[v.index] = vo
	-- end 
	-- LogError("!?",list)

	return paras
end 

-- 根据枚举类型获得背包格子组（过滤掉空格子
function MarbasData:GetMarbasResolve(fix)
	local paras = {}
	for k,v in pairs(self.item_grid) do 
		if v:Color() == fix then 
			table.insert(paras, v)
		elseif fix == -1 and v.item_id > 0 then 
			table.insert(paras, v)
		end 
	end 

	return paras
end 

-- 根据装备列表获取分解物品
function MarbasData:GetResolveList(data)
	local other_cfg = Config.zaohua_equip_up_auto.other[1] 
	-- return cfg.soul_power_add + (mana-1) * cfg.intelligence_added
	
	local marks = {}
	for k,v in pairs(data or {}) do 
		local cfg = v:Config()
		-- 统算分解本体材料
		if cfg ~= nil then 
			local item = Item.Init(cfg.return_item,cfg.return_num)
			if marks[item.item_id] == nil then 
				marks[item.item_id] = item
			else 
				marks[item.item_id].num = item.num + marks[item.item_id].num
			end 
		end 
		-- 统算血祭材料返还
		local force = v:CheckForce() 
		if force > 0 then 
			local mana = v:CheckMana()
			local num = math.floor(force / (other_cfg.soul_power_add + (mana-1)*other_cfg.intelligence_added))
			local item = Item.Init(other_cfg.blood_sacrifice_consume,num)

			if marks[item.item_id] == nil then 
				marks[item.item_id] = item
			else 
				marks[item.item_id].num = item.num + marks[item.item_id].num
			end 
		end
	end 

	local result = {}
	for k,v in pairs(marks) do 
		table.insert(result,v)
	end 
	return result
end

-- 获取背包中参与血祭的装备
function MarbasData:GetCanBloodEquipList()
	local result = {}
	for k,v in pairs(self.equip_list) do 
        -- 持有灵力值即可参与血祭
		if v.CheckMana and v:CheckMana() > 0 then 
			-- 目前都没有
			if v.item_id > 0 then 
			    table.insert(result,v)
			end 
	end 
	end 

	for k,v in pairs(self.item_grid) do 
		-- 持有灵力值即可参与血祭
		if v.CheckMana and v:CheckMana() > 0 then 
			-- 目前都没有
			if v.item_id > 0 then 
			    table.insert(result,v)
			end 
	end 
	end 
	return result
end

function MarbasData:GetBloodSoltLock(equip,index,check_force)
	local cfg = self.config.yuan_soul_hole[index]
	if cfg == nil then return false end 

	if check_force then 
		return equip:CheckForce() >= cfg.soul_power and equip:Color() >= cfg.color 
	else
		return equip:CheckForce() >= cfg.soul_power and equip:Color() >= cfg.color and equip:CheckForce() > 0
	end 
end

function MarbasData:GetBloodSoltLockStr(equip,index)
	local cfg = self.config.yuan_soul_hole[index]
	if cfg == nil then 
		return string.format(Language.Marbas.BloodUnLockByPower,Config.zaohua_equip_up_auto.other[1].min_soul_power) 
	end 
	
	if MarbasConfig.QuaEnter == cfg.color then 
		return string.format(Language.Marbas.BloodUnLockByPower,cfg.soul_power)
	else 
		return Language.Marbas.BloodUnLockByDemons
	end 
	
end

function MarbasData:SetBloodSetData(data)
	self.blood_view_data.select = data
end

function MarbasData:TypeBloodGetBloodSelect()
	local mark = self.blood_view_data.select

	if mark == nil then return end 

	if mark.in_bag then 
		for k,v in pairs(self.item_grid) do
			if mark:GridIndex() == v:GridIndex() then 
				mark = v
			end 
		end 
	else 
		mark = self.equip_list[mark.index]
	end 

	return mark
end

function MarbasData:SetBloodSetGemSlot(index)
	self.blood_view_data.select_index = index == nil and -1 or index
end

function MarbasData:SetBloodSetGemPlus(data)
	self.blood_view_data.select_gem = data

	ViewMgr:OpenView(MarbasBloodGemView)
end

function MarbasData:IsMaxForce(equip)
	if equip == nil or equip.CheckForce == nil then 
		return false
	end 

	return equip:CheckForce() >= Config.zaohua_equip_up_auto.other[1].max_soul_power
end

-- 获取血祭加成·
function MarbasData:GetBloodPlusAttrs(equip)
	local force = 0 
	if equip ~= nil and equip.CheckForce ~= nil then 
		force = equip:CheckForce()
	end 

	local role_prof = RoleData.Instance:GetRoleProfType()
	local cfg = Config.zaohua_equip_up_auto.blood_sacrifice
	local lowest_cfg = nil
	local act_cfg = nil
	local next_cfg = nil
	for k,v in pairs(cfg) do 
		if role_prof == v.limit_prof and v.hole_type == equip:Config().equip_type then 
			if v.soul_power == Config.zaohua_equip_up_auto.other[1].min_soul_power then 
				lowest_cfg = v
			end 

			if v.soul_power <= force then 
				act_cfg = v
			end 
		end 
	end 

	-- 趁机获取下一级的配置
	local power = 0
	local next_power = equip:CheckForce() + self:GetNextForce(equip:CheckMana())
	for k,v in pairs (cfg) do 
		if act_cfg == nil then break end 
		-- 职业相同且魂力值比激活的大但是最小的配置
		if v.hole_type == equip:Config().equip_type and role_prof == v.limit_prof and next_power >= v.soul_power and v.soul_power > power then 
			power = v.soul_power
			next_cfg = v

		end
	end 

	-- 指定操作项目
	local oper_attr = act_cfg == nil and lowest_cfg or act_cfg
	local flag_act = act_cfg ~= nil 
	local flag_next = next_cfg ~= nil and next_cfg ~= act_cfg

	if oper_attr == nil then 
		LogDG("铜哥哥，你血祭加成配置还没改好呢")
		return {
			list = {},
			name = "",
			is_act = false,
		}
	end 

	local list = {}
	for i = 1,6 do 
		local vo = {}
		vo.attr_type = oper_attr["attr_type_"..i]
		vo.attr_value = oper_attr["attr_add_"..i]
		vo.is_act = flag_act
		vo.is_next = vo.is_act and flag_next
		if vo.is_next then 
			vo.attr_next_attr = next_cfg["attr_add_"..i]
		end 

		if vo.attr_value > 0 then 
		    table.insert(list,vo)
	    end 
	end 

	local plus = {
		list = list,
		name = oper_attr.soul_power_name,
		is_act = flag_act
	}

	return plus
end

function MarbasData:GetBloodPlusTipsList()
	local role_prof = RoleData.Instance:GetRoleProfType()
	local mark = self.blood_view_data.select
	local equip_type = mark:Config().equip_type
	local plus_list = {}
	for k,v in pairs(self.config.blood_sacrifice) do 
		if v.limit_prof == role_prof and equip_type == v.hole_type then 
			local vo = {}
			vo.level = v.soul_power_name
			vo.force = v.soul_power
			vo.attr_str = ""
			for i = 1,6 do 
				if v["attr_add_"..i] > 0 then
					local str = Language.Common.AttrList[v["attr_type_"..i]] .. (DataHelper.IsPercent(v["attr_type_"..i]) and "+"..Percent(v["attr_add_"..i]) or "+"..v["attr_add_"..i])
					str = str .. "      "
					vo.attr_str = vo.attr_str..str
				end 
			end 
			table.insert(plus_list,vo)
		end 
	end 

	return plus_list
end

-- 获取该元魂珠的配置
function MarbasData:GetPearlCfg(item_id)
	if item_id == nil then return nil end 
	return self.Pearl_List[item_id]
end

-- 获取该元魂珠的材料配置（检索id为升级id
function MarbasData:GetPearlUpCfg(item_id)
	if item_id == nil then return nil end 
	return self.Pearl_Up_List[item_id]
end

-- 根据装备获取该装备激活的属性
function MarbasData:GetForceMix(equip)
	local operate_list = {}
	if equip ~= nil and equip.GetStoneList ~= nil then 
		operate_list = equip:GetStoneList()
	end

	local role_prof = RoleData.Instance:GetRoleProfType()
	-- yuan_soul_resonate
	-- resonate_add
	local mix_cfgs = {}
	local mark_param = {}
	for k,v in pairs(operate_list) do 
		local vo = {}
		if v.item_id > 0 then 
			local cfg = self:GetPearlCfg(v.item_id)
			if cfg == nil then 
				LogDG("不存在的元魂珠配置",v.item_id)
				return {}
			end 
			vo.level = cfg.yuan_soul_level
			vo.color = Item.GetColor(v.item_id)
			table.insert(mark_param,vo)
		end 
	end 

	local eff_level = 9999
	local eff_param = {}
	for k,v in pairs(mark_param) do
		if eff_param[v.color] == nil then 
			eff_param[v.color] = 1
		else 
			eff_param[v.color] = 1 + eff_param[v.color]
		end 
		if v.level < eff_level then 
			eff_level = v.level
		end 
	end 

	for k,v in pairs(self.config.yuan_soul_resonate)do 
		-- 同色数量足够
		if v.yuan_soul_num == eff_param[v.yuan_soul_color] then 
			for i,j in pairs(self.config.resonate_add) do 
				if eff_level == j.combination_level and j.yuan_hun_zhu_combination_id == v.resonance_group_id and
				    j.limit_prof == role_prof then 
					mix_cfgs.soul_resonate = v
					mix_cfgs.resonate_add = j
					break
				end 
			end 
		end 
	end 

	return mix_cfgs
end

function MarbasData:GetBloodGemNeoAttrs(equip)
	local operate_list = {}
	if equip ~= nil and equip.GetStoneList ~= nil then 
		operate_list = equip:GetStoneList()
	end 

	local list = {}
	for k,v in pairs(operate_list) do 
		-- 每个石头里有三个
		for i = 1 ,3 do
			local vo = {}
			vo.attr_type = v.attrs[i].attr_type
			vo.attr_value = v.attrs[i].attr_value
			vo.is_act = true -- 通用词条留用
			vo.is_next = false -- 通用词条留用

			if vo.attr_value > 0 then 
				if list[vo.attr_type] == nil then 
					list[vo.attr_type] = vo
				else 
					list[vo.attr_type].attr_value = list[vo.attr_type].attr_value + vo.attr_value
				end 
			end 
		end 
	end 

	-- 调整成正常list
	local fit_list = {}
	for k,v in pairs(list) do 
		table.insert( fit_list,v)
	end 

	table.sort(fit_list, DataSorter.KeyLowerSorter("attr_type"))	

	local mix_cfgs = self:GetForceMix(equip)
	local flag_resonate = mix_cfgs.soul_resonate ~= nil 

	local sp_desc = ""	
	local sp_list = {}
	if flag_resonate then 
		local cfg_add = mix_cfgs.resonate_add
	    for i = 1,4 do 
		    if cfg_add["value_"..i] > 0 then 
			    local is_percent = DataHelper.IsPercent(cfg_add["type_"..i])
			    sp_desc = sp_desc .. Language.Common.AttrList[cfg_add["type_"..i]].." +"..(is_percent and Percent(cfg_add["value_"..i]) or cfg_add["value_"..i])

				local vo = {}
				vo.type = Language.Common.AttrList[cfg_add["type_"..i]]
				vo.value = "+"..(is_percent and Percent(cfg_add["value_"..i]) or cfg_add["value_"..i])

				table.insert(sp_list,vo)
		    end 
		    if i < 4 then 
    			sp_desc = sp_desc .."\n"
	    	end 
	    end 
    end 
	local eff_id = flag_resonate and mix_cfgs.resonate_add.sp_id or 0
	if eff_id > 0 then 
		local effect = BagData.Instance:GetEquipEffectCfg(eff_id)
		sp_desc = sp_desc .."\n"..effect.description

		local vo = {}
		vo.desc = effect.description
		table.insert(sp_list,vo)
	end 

	local sp_name = ""
	if flag_resonate then 
		sp_name = mix_cfgs.soul_resonate.group_name .. "·"..mix_cfgs.resonate_add.combination_level..Language.Common.Level
	end 

	local plus = {
		list = fit_list,
		with_sp = flag_resonate ,
		name  = sp_name ,
		sp_desc = (flag_resonate) and sp_desc or Language.Marbas.BloodNoActGem,
		sp_list = sp_list,
	}

	return plus
end

-- 获取血祭加成·元魂珠
function MarbasData:GetBloodGemAttrs(equip)
	local operate_list = {}
	if equip ~= nil and equip.GetStoneList ~= nil then 
		operate_list = equip:GetStoneList()
	end 

	local list = {}
	for k,v in pairs(operate_list) do 
		local cfg = self:GetPearlCfg(v.item_id)
		if cfg ~= nil then 
			for i = 1,4 do 
				local vo = {}
				vo.attr_type = cfg["type_"..i]
				vo.attr_value = cfg["value_"..i]
				vo.is_act = true -- 通用词条留用
				vo.is_next = false -- 通用词条留用

				if vo.attr_value > 0 then 
					if list[vo.attr_type] == nil then 
						list[vo.attr_type] = vo
					else 
						list[vo.attr_type].attr_value = list[vo.attr_type].attr_value + vo.attr_value
					end 
				end 
			end 
		end 		
	end 

	-- 调整成正常list
	local fit_list = {}
	for k,v in pairs(list) do 
		table.insert( fit_list,v)
	end 

	table.sort(fit_list, DataSorter.KeyLowerSorter("attr_type"))	

	local mix_cfgs = self:GetForceMix(equip)
	local flag_resonate = mix_cfgs.soul_resonate ~= nil 

	local sp_desc = ""	
	if flag_resonate then 
		local cfg_add = mix_cfgs.resonate_add
	    for i = 1,4 do 
		    if cfg_add["value_"..i] > 0 then 
			    local is_percent = DataHelper.IsPercent(cfg_add["type_"..i])
			    sp_desc = sp_desc .. Language.Common.AttrList[cfg_add["type_"..i]].." +"..(is_percent and Percent(cfg_add["value_"..i]) or cfg_add["value_"..i])
		    end 
		    if i < 4 then 
    			sp_desc = sp_desc .."\n"
	    	end 
	    end 
    end 

	local eff_id = flag_resonate and mix_cfgs.resonate_add.sp_id or 0
	if eff_id > 0 then 
		local effect = BagData.Instance:GetEquipEffectCfg(eff_id)
		sp_desc = sp_desc .."\n"..effect.description
	end 

	local sp_name = Language.Marbas.BloodNoActGem
	if flag_resonate then 
	    for k,v in pairs(self.config.yuan_soul_resonate) do 
		    if mix_cfgs.resonate_add.yuan_hun_zhu_combination_id == v.resonance_group_id then 
			    sp_name = Language.Marbas.GemMix..":".. v.group_name
		    end 
	    end 
    end 

	local plus = {
		list = fit_list,
		with_sp = flag_resonate ,
		name  = flag_resonate and mix_cfgs.soul_resonate.group_name or "" ,
		sp_name = sp_name,
		sp_desc = flag_resonate and sp_desc or Language.Marbas.BloodNoActGem,
	}

	return plus
end

-- 获取下一级的元魂珠
function MarbasData:GetGemNextLevel(item_id)
	local cfg = self:GetPearlCfg(item_id)

	return Item.Init( (cfg ~= nil and cfg.next_level_id > 0) and cfg.next_level_id or item_id)
end

function MarbasData:GetGemQuaCfg(group,seq)
	for k,v in pairs(self.config.yuan_soul_quality) do 
		if v.group_id ==group and v.seq == seq then 
			return v
		end 
	end 
end

-- 返回元魂珠的强化信息新
function MarbasData:GetMarbasGemNPlusInfo()
	local mark = self.blood_view_data.select_gem
	local next = self:GetGemNextLevel(mark.item_id)
	local fit_list = {}

	local cur_cfg = self:GetPearlCfg(mark.item_id)
	local next_cfg = self:GetPearlCfg(next.item_id)

	for index,seq in pairs(mark:GemAttrSeqInfo()) do 
		local cur_qua = self:GetGemQuaCfg(cur_cfg["hole_"..index],seq)
		local next_qua = self:GetGemQuaCfg(next_cfg["hole_"..index],seq)

		if cur_qua ~= nil then 
			local vo = {}
	    	vo.attr_type = cur_qua.type
			vo.attr_value = cur_qua.value
			vo.is_act = true -- 通用词条留用
			vo.is_next = true -- 通用词条留用

			if vo.is_next then 
				vo.attr_next_attr = next_qua.value
			end 

			vo.o_next = 1
			if vo.attr_value > 0 then 
				table.insert(fit_list,vo)
			end 
		end
	end 

	local plus = {
		list = fit_list,
		item_cur = mark,
		item_next = next,
		need_num = cur_cfg.need_num,
	}
	return plus
end

-- 返回元魂珠的强化信息（暂时废弃
function MarbasData:GetMarbasGemPlusInfo()
	local mark = self.blood_view_data.select_gem
	local next = self:GetGemNextLevel(mark.item_id)
	local fit_list = {}

	local cur_cfg = self:GetPearlCfg(mark.item_id)
	local next_cfg = self:GetPearlCfg(next.item_id)
	local list = {}
	for i = 1,4 do 
		local vo = {}
	    vo.attr_type = cur_cfg["type_"..i]
		vo.attr_value = cur_cfg["value_"..i]
		vo.is_act = true -- 通用词条留用
		vo.is_next = true -- 通用词条留用

		if vo.is_next then 
			vo.attr_next_attr = next_cfg["value_"..i]
		end 

		if vo.attr_value > 0 then 
			table.insert(fit_list,vo)
		end 
	end 

	self.total_gem_list = {}
	self:GetRankUpForBloodGemCost(cur_cfg.next_level_id)
	local cost_fit_list = {}
	for k,v in pairs(self.total_gem_list) do
		local mark = nil
		for pos,cell in pairs(cost_fit_list) do
			if v.item_id == cell.item_id and v.is_buy == cell.is_buy then 
				mark = pos
				break
			end 
		end 

		if mark ~= nil then 
			cost_fit_list[mark].need_num = cost_fit_list[mark].need_num + 1
		else 
			local vo = {}
			vo.item_id = v.item_id
			vo.num = v.is_buy and 0 or 1
			vo.need_num = 1
			vo.is_buy = v.is_buy
			table.insert(cost_fit_list,vo)
		end 
	end 

	local buy_param = {}

	for k,v in pairs(self.total_gem_list) do 
		local cfg = self:GetPearlCfg(v.item_id)
		if v.is_buy and cfg ~= nil then 
			if buy_param[0] == nil then buy_param[0] = {} end 

			if buy_param[0][cfg.yuan_soul_level] == nil then 
				buy_param[0][cfg.yuan_soul_level] = 1
			else 
				buy_param[0][cfg.yuan_soul_level] = 1 + buy_param[0][cfg.yuan_soul_level]
			end 
		else 
			if buy_param[1] == nil then buy_param[1] = {} end 

			if buy_param[1][cfg.yuan_soul_level] == nil then 
				buy_param[1][cfg.yuan_soul_level] = 1
			else 
				buy_param[1][cfg.yuan_soul_level] = 1 + buy_param[1][cfg.yuan_soul_level]
			end 
		end 
	end 

	-- 补齐
	for i = 1,cur_cfg.yuan_soul_level do 
		if buy_param[1] and buy_param[1][i] == nil then 
			buy_param[1][i] = mark.src_type == BloodSrcType.Bag and 1 or 0
		end 
	end 

	local plus = {
		list = fit_list,
		item_cur = mark,
		item_next = next,
		item_cost = cost_fit_list,
		param = buy_param
	}

	return plus
end

-- 获取该id的升级素材
function MarbasData:GetRankUpForBloodGem(item_id)

	local cfg = self:GetPearlUpCfg(item_id)	
	if cfg == nil then cfg = self:GetPearlCfg(item_id) end 

	return {item_id = cfg.yuan_soul_id ,level = cfg.yuan_soul_level , num = cfg.need_num}
end

-- 递归方法警告！
-- 注意这里注入的是升级对象
-- 升级对象，材料需求数量，列表
function MarbasData:GetRankUpForBloodGemCost(item_id)
	local show = self.blood_view_data.select_gem
	local cfg = self:GetRankUpForBloodGem(item_id)	-- 材料信息
	local with_num = Item.GetNum(cfg.item_id) -- 持有的材料数
	local is_self = show.item_id == cfg.item_id
	with_num = (is_self and with_num - 1 >= 0) and with_num - 1 or with_num
	-- 自己不列入需求数里
	local total_need = is_self and cfg.num - 1 or cfg.num -- 总共材料需要数
	local is_base = cfg.level == 1 
	-- 是否持有足够的材料
	if with_num >= total_need then 
		for i = 1,total_need do 
			table.insert(self.total_gem_list,{item_id = cfg.item_id,is_buy = false})
		end 
	-- 是否抵达递归尽头
	elseif not is_base then 
		-- 需要升级的材料数
		local plus_num = total_need - with_num
		for i = 1,plus_num do 
			-- 将需要递归的部分继续分发
			self:GetRankUpForBloodGemCost(cfg.item_id)
		end 
		-- 不需要递归的进表
		for i = 1, with_num do 
			table.insert(self.total_gem_list,{item_id = cfg.item_id,is_buy = false})
		end
	-- 抵达尽头
	elseif is_base then 
		for i = 1, total_need do 
			table.insert(self.total_gem_list,{item_id = cfg.item_id,is_buy = is_base})
		end 
	end
end

-- 返回背包中的元魂珠列表
function MarbasData:GetMarbasGemList()
	local data_list = TableCopy(BagData.Instance:GetBagMarbasGemList())
	for k,v in pairs(data_list) do 
		v.src_type = BloodSrcType.Bag
	end 
	return data_list
end

-- 返回本次血祭的加成量
function MarbasData:GetNextForce(mana)
	local cfg = Config.zaohua_equip_up_auto.other[1] 
	return cfg.soul_power_add + (mana-1) * cfg.intelligence_added
end

function MarbasData:GetBloodPlusGemTipsList()
	local show_list = {}
	local total_list = self.config.yuan_soul_resonate
	local add_list = self.config.resonate_add
	local role_prof = RoleData.Instance:GetRoleProfType()
	for k,v in pairs(total_list) do 
		local parent = {}
		parent.name = v.group_name
		parent.plus = string.format(Language.Marbas.BloodGroup,v.yuan_soul_num,Language.Marbas.ColorGemName[v.yuan_soul_color])
		parent.children = {}
		for i,j in pairs(add_list) do 
			local vo = {}
			if j.yuan_hun_zhu_combination_id == v.resonance_group_id and j.limit_prof == role_prof then 
				vo.level = j.combination_level
				 -- .. (config == nil and "" or config.description)
				local sp_desc = ""
				for i = 1,4 do 
					if j["value_"..i] > 0 then 
						local is_percent = DataHelper.IsPercent(j["type_"..i])
						sp_desc = sp_desc .. Language.Common.AttrList[j["type_"..i]].." +"..(is_percent and Percent(j["value_"..i]) or j["value_"..i])
					end 
					if i < 4 then 
						sp_desc = sp_desc .."  "
					end 
				end 
				if j.sp_id > 0 then 
				    local eff_config = Cfg.SpecialEffect(j.sp_id)
					sp_desc = sp_desc .."  "..eff_config.description
				end 
				vo.desc = Language.Marbas.BloodGemEff..sp_desc
				table.insert(parent.children, vo)
			end 
		end 
		table.sort(parent.children ,function(a,b)return a.level < b.level end)

		parent.desc = parent.children[1].desc

		table.insert(show_list,parent)
	end 

	return show_list
end


function MarbasData:GetVigorousShowList()
	local prof = RoleData.Instance:GetRoleProfType()
	return self.vigorous_show_list[prof]
end

-- 造化装备总等级
function MarbasData:GetTotalLevel()
	local total_level = 0
	for k,v in pairs(self.equip_list) do 
		total_level = total_level + v:LevelShow()
	end 

	return total_level
end

-- 造化装备装等数量
function MarbasData:GetVigorousLevelCheck(level)
	local num = 0
	for k,v in pairs(self.equip_list) do 
		if v:LevelShow() >= level then
			num = num + 1
		end 
	end 
	return num
end

-- 获取该id是否已激活
function MarbasData:GetVigorousUnlock(id)
	if self.vigorous_info.flag == nil then return false end 
	return self.vigorous_info.flag[id] == 1
end

-- 获取该id是否穿着
function MarbasData:GetVigorousWeared(id)
	if self.vigorous_info.wear_app_id == nil then return false end 
	return self.vigorous_info.wear_app_id ~= nil and self.vigorous_info.wear_app_id == id
end 

function MarbasData:GetVigorousCanUnlock(id)
	local cfg = self.config.vigorous[id]
	local mark_cfg = self.suit_enable.list[8]

	-- 身上存在8件套时可以激活
	if mark_cfg == nil or cfg == nil then 
		return false
	end 

	return cfg.level <= mark_cfg.level and mark_cfg.zaohua_equip_suit == cfg.zaohua_equip_suit 
	-- self.suit_cfg_list[cfg_il_link][cfg_cls_link] = v
end

function MarbasData:GetVigorousSelect()
	return self.vigorous_view_data.select_data 
end

function MarbasData:SetVigorousSelect(data)
	self.vigorous_view_data.select_data = data
end

function MarbasData:SetVigorousCancelSelect(data)
	if self.vigorous_view_data.select_data == nil then 
		return 
	end 
	if self.vigorous_view_data.select_data.seq ~= data.seq then 
		return
	end 
	self.vigorous_view_data.select_data = nil
end

function MarbasData:WaitVigorousActMark(id)
	self.vigorous_view_data.mark_id = id
end

function MarbasData:EffActive()
	local param = {show_id = self.vigorous_view_data.mark_id}
	ViewMgr:OpenView(MarbasVigorousShowView,param)
end

function MarbasData:GetVigorousCfg(id)
	if id == nil then 
		return nil
	end 

	return self.config.vigorous[id]
end

function MarbasData:GetVigorousCfgDetail(level,prof)
	self.checked_vigorous_cfg = {}
	for k,v in pairs(self.config.vigorous) do 
		if v.level == level and v.limit_prof == prof then 
			if self.checked_vigorous_cfg[level] == nil then 
				self.checked_vigorous_cfg[level] = {}
			end 
			self.checked_vigorous_cfg[level][prof] = v
			return v
		end 
	end 
end

-- 根据角色的app信息获取对应的特效编号
function MarbasData:GetVigorousEffId(app)
	
	local cfg = self:GetVigorousCfg(app)

	if cfg == nil then
		return SceneEffectConfig.EffectList.MarbasVSE1
	end

	local eff = SceneEffectConfig.EffectList["MarbasVSE"..cfg.level]

	if eff == nil then 
		return SceneEffectConfig.EffectList.MarbasVSE1
	else 
		return eff
	end 
end

function MarbasData:TypeDemonsSetSelect(data)
	self.demons_view_data.select = data

	self.demons_view_data.flush = not self.demons_view_data.flush 
end 

function MarbasData:TypeDemonsGetEquipList()
	local list = {}
	for k,v in pairs(self.equip_list) do 
		if v:Color() == MarbasConfig.QuaEnter then 
			table.insert(list,v)
		end 
	end 

	for k,v in pairs(self.item_grid) do 
		if v.item_id > 0 and v:Color() == MarbasConfig.QuaEnter then 
			table.insert(list,v)
		end 
	end 

	return list
end 

function MarbasData:TypeDemonsPreDemenos()
	if self.demons_view_data.select == nil then 
		return 
	end 

	local item = nil
	for k,v in pairs(self.config.enchant) do 
		if v.enchant_before == self.demons_view_data.select.item_id then 
			item = Item.Init(v.enchant_after)
			item.param = self.demons_view_data.select.param
		end 
	end 

	return item
end 

function MarbasData:TypeDemonsDemenosSelect()
	local mark = self.demons_view_data.select
	
	if mark == nil then return nil end 

	if mark.in_bag then 
		for k,v in pairs(self.item_grid) do 
			if mark:GridIndex() == v:GridIndex() then 
				mark = v
			end 
		end 
	else 
		mark = self.equip_list[mark:GridIndex()]
	end 
	return mark
end

-- 注入装备
function MarbasData:GetItemInfoMarbasSuitShow(data)
	local equip_cfg = data:Config()

	local in_bag = data.in_bag
	local cfg_il_link = self.CfgLink({equip_cfg.suit_type,equip_cfg.limit_prof},2)
	local suit_nums = {8}
	local show_list = {}
	
	for i = 1,1 do 
		local cfg_cls_link = self.CfgLink({equip_cfg.color,equip_cfg.level_show,suit_nums[i]},3)
		local suit_cfg = self.suit_cfg_list[cfg_il_link][cfg_cls_link]

		if not in_bag and self.suit_enable.list[suit_nums[i]] ~= nil then 
			suit_cfg = self.suit_enable.list[suit_nums[i]]
		end 

		if suit_cfg ~= nil then 
		    table.insert(show_list,suit_cfg)
		end 
	end 

	local suit_list = {}
	for k,v in pairs(show_list) do 
		local vo = {}

		local colors = self.suit_enable.list[v.suit_num] == v and EquipSetData.ColorSet.Act or EquipSetData.ColorSet.Lock

		vo.desc_title = ColorStr(v.god_print_name_1,colors.title)
		vo.attr_desc = ""
		if v.full_potential > 0 then
			vo.attr_desc = ColorStr(v.name1.." +"..v.potential_add_1.."%",colors.attr)
		else
			local num = 1 
			for i = 1,7 do 
				if v["potential_add_"..i] > 0 then 
					local plus = ColorStr(v["name"..i] .. " +"..v["potential_add_"..i].."%",colors.attr)
					if num < 3 then 
					    vo.attr_desc = vo.attr_desc .. plus
					elseif num == 3 then 
						vo.attr_desc = vo.attr_desc .. plus
					end 
				end 
			end 
		end 
		vo.show_sp = false

		table.insert(suit_list, vo)
	end 

    return suit_list
end 

-- 注入装备
function MarbasData:GetMarbasDemonsSuitShow(data)
	local equip_cfg = data:Config()

	local cfg_il_link = self.CfgLink({equip_cfg.suit_type,equip_cfg.limit_prof},2)
	local suit_nums = {8}
	local show_list = {}
	
	for i = 1,1 do 
		local cfg_cls_link = self.CfgLink({equip_cfg.color,equip_cfg.level_show,suit_nums[i]},3)
		local suit_cfg = self.suit_cfg_list[cfg_il_link][cfg_cls_link]

		if suit_cfg ~= nil then 
		    table.insert(show_list,suit_cfg)
		end 
	end 

	local suit_list = {}
	for k,v in pairs(show_list) do 
		local vo = {}

		local colors = EquipSetData.ColorSet.Act -- self.suit_enable.list[v.suit_num] == v and  -- or EquipSetData.ColorSet.Lock

		vo.desc_title = ColorStr(string.format(Language.Marbas.SuitFormatDemons,v.god_print_name,""),colors.title)
		vo.attr_list = {}

		if v.full_potential > 0 then
			local plus = {}
			plus.type = v.name1
			plus.value = "+"..v.potential_add_1.."%"
			plus.is_attr = true
			table.insert(vo.attr_list,plus)
		else
			for i = 1,7 do 
				if v["potential_add_"..i] > 0 then 
					local plus = {}
					plus.type =  v["name"..i]
					plus.value = "+"..v["potential_add_"..i].."%"
					plus.is_attr = true
					table.insert(vo.attr_list,plus)
				end 
			end 
		end 

		table.insert(suit_list, vo)
	end 

    return suit_list
end 

function MarbasData:GetDemonsBefore(item_id)
	for k,v in pairs(self.config.enchant) do 
		if item_id == v.enchant_before then 
			return item_id
		elseif item_id == v.enchant_after then 
			return v.enchant_before
		end 
    end 
end 

function MarbasData:GetDemonsAfter(item_id)
	for k,v in pairs(self.config.enchant) do 
		if item_id == v.enchant_before then 
			return v.enchant_after
		elseif item_id == v.enchant_after then 
			return item_id
		end 
    end 
end 

function MarbasData:ShowDemonsSuccess(data)
	if self.demons_view_data.demons_mark == 1 then 
		local operate = data
		if data == nil then 
			operate = self:TypeDemonsDemenosSelect()
		end 

		self:SetBuildResult({item = operate})
		ViewMgr:OpenView(MarbasNeoComplete,{type = MarbasConfig.NeoType.Demons})
	end
end

function MarbasData:GetMarbasSuitEffingInSuitShow()
	local list = {}

	for i = 1,2 do 
		local color = i == 1 and MarbasConfig.QuaEnter or MarbasConfig.QuaMaster
		for k,v in pairs(MarbasConfig.SuitNum) do 
			local ready_cfg = self:CompatibilitySuit(color,v)
			local suit_cfg = {}
			local colors = (self.suit_enable.list[ready_cfg.suit_num] ~= nil and self.suit_enable.list[ready_cfg.suit_num].color == ready_cfg.color
			    and self.suit_enable.list[ready_cfg.suit_num].level_show == ready_cfg.level_show)
				and EquipSetData.ColorSet.Act or EquipSetData.ColorSet.Lock
			-- local colors = EquipSetData.ColorSet.Act

		    suit_cfg.desc_title = ColorStr(ready_cfg.god_print_name_1,colors.title)
		    suit_cfg.attr_desc = ""
			if ready_cfg.full_potential > 0 then
				suit_cfg.attr_desc = ColorStr(ready_cfg.name1.." +"..ready_cfg.potential_add_1.."%",colors.attr)
			else
				local num = 1 
				for i = 1,7 do 
					if ready_cfg["potential_add_"..i] > 0 then 
						local plus = ColorStr(ready_cfg["name"..i] .. " +"..ready_cfg["potential_add_"..i].."%",colors.attr)
						if num < 3 then 
							suit_cfg.attr_desc = suit_cfg.attr_desc .. plus
						elseif num == 3 then 
							suit_cfg.attr_desc = suit_cfg.attr_desc .. plus
						end 
					end 
				end 
			end 

		    suit_cfg.show_sp = false
			suit_cfg.suit_num = v
			suit_cfg.color = color
			table.insert(list,suit_cfg)
		end 
	end 

	return list
end

function MarbasData:EquipedDemons()
	-- if self.marbas_view_data.equiped_demons ~= nil then 
	-- 	return self.marbas_view_data.equiped_demons
	-- end 
	-- for k,v in pairs(self.equip_list) do
	-- 	if v:Color() == MarbasConfig.QuaMaster then 
	-- 		self.marbas_view_data.equiped_demons = true
	-- 		return true
	-- 	end 
    -- end 
	return self.common_info.show_mo_zhuang == 1
end

function MarbasData:TypeSuitShowGetInfo()
	local mark = self.suit_enable.changes

	if mark == nil then 
		return 
	end 

	local cells = {}
	for k,v in pairs(self.equip_list) do
		local cfg = v:Config()
		local flag = {}
		-- 套装id
		flag[1] = cfg.suit_type == mark.zaohua_equip_suit
		-- 等级
		flag[2] = cfg.level_show >= mark.level_show
		-- 职业
		flag[3] = cfg.limit_prof == mark.limit_prof
		-- 颜色
		flag[4] = cfg.color >= mark.color

		local flag_mix = true
		for i = 1,4 do 
			flag_mix = flag_mix and flag[i]
		end 

		if flag_mix then 
			table.insert(cells,v)
		end 
	end

	local param = {
		suit_num = mark.suit_num,
		mark = mark,
		cell_list = cells
	}
	return param
end 

function MarbasData:GetComposeCfgByStuff(item_id)
	for k,v in pairs(self.config.yuan_soul_compose) do 
		if v.use_item == item_id then
			return v
		end 
	end 

	return nil
end

function MarbasData:GetDeComposeCfgByTarget(item_id)
	for k,v in pairs(self.config.decompose) do 
		if v.id == item_id then
			return v
		end 
	end 

	return nil
end

function MarbasData:GetLevelShowEnoughNeed(mod)
	for k,v in pairs(self.config.open_list) do 
		if v.client_id == mod then
			return v.all_equip_level > 0 
		end 
	end 
	return true
end

function MarbasData:GetLevelShowEnough(mod)
	local total_level = self:GetTotalLevel()

	for k,v in pairs(self.config.open_list) do 
		if v.client_id == mod then
			-- LogError("?!GetLevelShowEnough",mod,total_level >= v.all_equip_level ,v.all_equip_level)
			return total_level >= v.all_equip_level ,v.all_equip_level
			-- return total_level >= 0 ,0
		end 
	end 
	return true
end

function MarbasData:GetLevelShowEnoughWithStr(mod)
	local flag,level = self:GetLevelShowEnough(mod)
	local tips = flag and nil or string.format(Language.Marbas.TotalLevelShowLimit,level)
	return flag,tips
end

function MarbasData:GetIsMarbasGemComposeItem(item_id)
	return self:GetComposeCfgByStuff(item_id) ~= nil
end

function MarbasData:SetMasterBlock(flag)
	self.marbas_view_data.is_block = flag
end

function MarbasData:CheckNeedMark()
	local mark = self.blood_view_data.select

	for k,v in pairs(self.item_list) do 
		if v.item_id > 0 then 
		    if BagData.Instance:JudgeEquipIsEqual(v,mark)  then 
			    return false
		    end 
	    end 
	end 

	for k,v in pairs(self.equip_list) do 
		if BagData.Instance:JudgeEquipIsEqual(v,mark)  then 
			return false
		end 
	end 

    self:SetBloodSetData()
	return true
end

-- function MarbasData:GetSoulQualityCfg(group)
-- 	for k,v in pairs() do 

-- 	end 
-- end
