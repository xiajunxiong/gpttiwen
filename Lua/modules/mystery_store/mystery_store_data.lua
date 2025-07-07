MysteryStoreData = MysteryStoreData or BaseClass()

function MysteryStoreData:__init()
	if MysteryStoreData.Instance ~= nil then
		Debuger.LogError("[MysteryStoreData] attempt to create singleton twice!")
		return
	end	
	MysteryStoreData.Instance = self
	self.mystery_store_cfg = Config.mystery_store_auto
	self.item_group_data = SmartData.New({})
	self.item_list = {}
	self.remain_update_time = 0
	self.refresh_num = 0
	self.mystery_store_data = SmartData.New({next_free_refresh_time = -1})
	Cfg.Register("GetMysteryItemByGidAndSeq",Cfg.RealTimeSearch2Key(self.mystery_store_cfg.item_group,"group_id","seq"))
	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RoleNoticeComplete, self))
	self:InitItemUnlock()

	self.anim_data = SmartData.New({val = false})
end

function MysteryStoreData:RoleNoticeComplete()
    MysteryStoreCtrl.Instance:SendMysteryStoreReq(2)
end

function MysteryStoreData:__delete()
	self.item_group_data:Set({})
	self.item_group_data = nil
end

function MysteryStoreData:SetItemGroupData(protocol)
	self.refresh_num = protocol.refresh_num
	self.item_group_data.StoreCardInfos = protocol.StoreCardInfos
	self.remain_update_time = protocol.remain_update_time
	self.mystery_store_data.next_free_refresh_time = protocol.next_free_refresh_time
	self:InitItemData()
end

function MysteryStoreData:GetRefreshNum()
	return self.refresh_num
end

-- 判断是否有免费刷新
function MysteryStoreData:IsHasFree()
	return (TimeHelper.GetServerTime() - self.mystery_store_data.next_free_refresh_time) > 0
end

function MysteryStoreData:GetItemGroupData()
	return self.item_group_data
end

function MysteryStoreData:InitItemData()
	self.item_list = {}
	for i,v in pairs(self.item_group_data.StoreCardInfos) do
		local cfg = Cfg.GetMysteryItemByGidAndSeq(v.group_id,v.seq)
		if cfg == nil then
			return
		end
		self.item_list[i] = v
		self.item_list[i].cfg = cfg
		self.item_list[i].index = i
		self.item_list[i].RemainNum = (self.item_list[i].cfg.limit_convert_count - v.buy_count) <= 0 and 0 or (self.item_list[i].cfg.limit_convert_count - v.buy_count)
	end
end

function MysteryStoreData:GetItemList()
	return self.item_list
end

--刷新需要的配置
function MysteryStoreData:GetUpdateCfg()
	local consume_add = self.mystery_store_cfg.consume_add
	for i, v in ipairs(consume_add) do
		if self.refresh_num < v.manual_refresh_times then
			return v
		end
	end
	return consume_add[#consume_add]
end

--刷新倒计时
function MysteryStoreData:GetUpdateTime()
	return self.remain_update_time
end

function MysteryStoreData:IsHasSpecial()
	for k,v in pairs(self.item_list or {}) do
		if v.cfg.is_special == 1 then
			return true
		end
	end
end

function MysteryStoreData:InitItemUnlock()
	self.item_unlock_config = {}
	for k,v in pairs(Config.mystery_store_auto.item_unlock) do
		if v.is_show == 1 then
			table.insert(self.item_unlock_config,v)
		end
	end
	for k,v in pairs(Config.mystery_store_auto.reward_extra_show) do
		table.insert(self.item_unlock_config,v)
	end
end

function MysteryStoreData:GetItemUnlockList()
	local list = self.item_unlock_config
	for k,v in pairs(list) do
		v.color = v.color or Item.GetColor(v.item_id)
	end
    local pass_level = DreamNotesData.Instance:GetPassLevel()
	table.sort(list,DataHelper.WeightSort([[sort]],function(data)
		if pass_level < data.dream_notes_level then
			return 1000
		end
		return data.color * 100
    end))
	return list
end

function MysteryStoreData:GetItemUnlockData(pass_level)
	return DataHelper.GetSearch(self.item_unlock_config,[[dream_notes_level]],pass_level or DreamNotesData.Instance:GetPassLevel())
end

--------------------- 伙伴装备分解数据 --------------------
PartnerEquiqBreakData = PartnerEquiqBreakData or BaseClass()

function PartnerEquiqBreakData:__init()
	if PartnerEquiqBreakData.Instance ~= nil then
		Debuger.LogError("[PartnerEquiqBreakData] attempt to create singleton twice!")
		return
	end	
	PartnerEquiqBreakData.Instance = self
	self.select_items = SmartData.New({})
	self.show_item = {}
end

function PartnerEquiqBreakData:__delete()
	self.select_items:Set({})
	self.select_items = nil
end

--获取伙伴装备分解等级选择的btn_list
function PartnerEquiqBreakData:GetEquiqBreakLevelList()
	local break_cfg = Config.partner_equipment_cfg_auto.partner_equipment_resolve
	local btn_list = {}
	local levels = {}
	for i,v in ipairs(break_cfg) do
		if not levels[v.level] then
			levels[v.level] = true
			table.insert(btn_list, {
				end_level = v.level,
				start_level = v.level,
				name = string.format(Language.MysteryStore.BtnName1, v.level)
			})
		end
	end
	local len = #btn_list + 1
	btn_list[len] = {}
	btn_list[len].start_level = 0
	btn_list[len].end_level = 10000
	btn_list[len].name = string.format(Language.MysteryStore.BtnName3)
	return DataHelper.ReverseList(btn_list)
end

--获取可分解的最小等级
function PartnerEquiqBreakData:GetCanBreakLevel()
	local break_cfg = Config.partner_equipment_cfg_auto.partner_equipment_resolve
	if self.can_min_level then
		return self.can_min_level
	end
	local min = 10000
	for _,v in pairs(break_cfg) do
		if v.level < min then
			min = v.level
		end
	end
	self.can_min_level = min
	return self.can_min_level
end

--获取选择的items
function PartnerEquiqBreakData:GetSelectItems()
	return self.select_items
end

--设置选中的itms
function PartnerEquiqBreakData:SetSelectItems(index,item)
	if self.select_items[index] == nil then
		self.select_items[index] = item
		--self.select_end_items = item
	else
		self.select_items[index] = nil
		--self.select_end_items = nil
	end
end

--清空选中的items
function PartnerEquiqBreakData:ClearSelectItems()
    self.select_items = {}
end


--获取选择的伙伴装备可分多少个气运值
function PartnerEquiqBreakData:GetAllSelectItemBreakNum()
	local num = 0
	local min_level = self:GetCanBreakLevel()
	local count = 0
	for _,v in pairs(self.select_items or {}) do
		if v.index and v.limit_level >= min_level then
			local break_cfg = self:GetEquipBreakLuck(v:LimitLv(), v:Color())
			num = num + break_cfg.qiyun
			count = count + 1
		end
	end
	return num,count
end
--获取装备分解的运气
function PartnerEquiqBreakData:GetEquipBreakLuck(equip_level, color)
	if equip_level < self:GetCanBreakLevel() then
		return {qiyun = 0}
	end
	local cfg = Config.partner_equipment_cfg_auto.partner_equipment_resolve
	for _,v in pairs(cfg) do
		if v.level == equip_level and v.color == color then
			return v
		end
	end
end

function PartnerEquiqBreakData:SetShowItem(item)
	self.show_item = item
end

function PartnerEquiqBreakData:GetShowItem()
	return self.show_item
end