--跳转相关
--[[
	选中标签
	StoreData.Instance:SetJumpIndex({conver_type = 0})
	ViewMgr:OpenViewByKey(Mod.Store.Main)
	选中道具
	StoreData.Instance:SetJumpIndex({conver_type = 0},item_id)
    ViewMgr:OpenViewByKey(Mod.Store.Main)
]]
VIEW_REQ("modules/mall/shop/store_view")
VIEW_REQ("modules/mall/shop/commerce_view")

ShoppingCtrl = ShoppingCtrl or BaseClass(BaseCtrl)

function ShoppingCtrl:__init()
	if ShoppingCtrl.Instance ~= nil then
		Debuger.LogError("[ShoppingCtrl] attempt to create singleton twice!")
		return
    end
    ShoppingCtrl.Instance = self
	self.store_data = StoreData.New()
	self.commerce_data = CommerceData.New()
	self:RegisterAllProtocols()
end

function ShoppingCtrl:__delete()
	self:UnRegisterAllProtocols()
	ShoppingCtrl.Instance = nil
	Delete(self,"store_data")
	Delete(self,"commerce_data")
	EventSys:UnBind(self.login_handle)
    EventSys:UnBind(self.level_handle)
end

function ShoppingCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSConvertShopBuyReq)
	self:RegisterProtocol(SCConvertShopBuyRecord,"OnConvertShopBuyRecord")
	self:RegisterProtocol(SCConvertShopAddRecord,"OnConvertShopAddRecord")
	--注册红点逻辑
	RemindCtrl.Instance:Remind():Register(Mod.Store.Main,self.store_data.red_point_data,
	BindTool.Bind(self.CheckStoreRemind, self))
	EventSys:Bind(GlobalEventKeys.RoleLevelChange, function()
		self.store_data:CheckRedPiont()
	end)

	self.login_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.LevelChange, self))
	self.level_handle = EventSys:Bind(GlobalEventKeys.RoleLevelChange, BindTool.Bind(self.LevelChange, self))
end

function ShoppingCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSConvertShopBuyReq)
	self:UnRegisterProtocol(SCConvertShopBuyRecord)
	self:UnRegisterProtocol(SCConvertShopAddRecord)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Store.Main)
end

function ShoppingCtrl:LevelChange()
	self.store_data:CheckPagaData()
end

----------------------新百货--新修改------------------------------
--请求购买百货商品
function ShoppingCtrl:SendShopBuyReq(conver_type,seq,item_id,num)
	local protocol = GetProtocol(CSConvertShopBuyReq)
	protocol.conver_type = conver_type or 0
	protocol.seq = seq or 0
	protocol.item_id = item_id or 0
	protocol.num = num or 1
	SendProtocol(protocol)
end

function ShoppingCtrl:SendStoreBuyReq(data)
	self:SendShopBuyReq(data.conver_type,data.seq,data.item_id,data.make_num)
end

--下发全部信息
function ShoppingCtrl:OnConvertShopBuyRecord(protocol)
	self.store_data:SetConvertShopBuyRecord(protocol)
end

--下发单个信息
function ShoppingCtrl:OnConvertShopAddRecord(protocol)
	self.store_data:SetConvertShopAddRecord(protocol)
end

function ShoppingCtrl:OpenStoreView(page)
	local index,config = StoreData.Instance:GetShopTypeIndex(page)
	if config and RoleData.Instance:GetRoleLevel() < config.limit_level then
		PublicPopupCtrl.Instance:Center(Format(Language.Shop.LevelLimitTip,config.limit_level))
		return
	end
	StoreData.Instance:SetSelectType(index)
	ViewMgr:OpenViewByKey(Mod.Store.Main)
end

function ShoppingCtrl:CheckStoreRemind()
	if not FunOpen.Instance:GetFunIsOpened(Mod.Store.Main) then
        return 0
	end
	return self.store_data:GetRedNum()
end

-------------------------新修改百货-------------------------
GOLD_STORE_TYPE = 5 --元宝商店类型
BATTLEFIELD_SHOP_TYPE = 8 --幻界商店类型
FAIRY_DRAGON_SHOP_TYPE = 14 --龙魂碎片兑换商店
GOD_SPACE_EXPLOITS_SHOP_TYPE = 15 --神域战功兑换商店
DIAMOND_STORE_TYPE = 16 --钻石商店类型

StoreData = StoreData or BaseClass()

function StoreData:__init()
	if StoreData.Instance ~= nil then
		Debuger.LogError("[StoreData] attempt to create singleton twice!")
		return
	end
    StoreData.Instance = self
    self:Initialization()
    self:InitPageBtnData()
    self:CheckPagaData()
	self.count_data = SmartData.New()
	self.red_point_data = SmartData.New({val = false})
end

function StoreData:Initialization()
	self.select_data = SmartData.New({type = 1,index = {},page_data = {}})
end

function StoreData:CheckPagaData()
	self.paga_data = {}
	for k,v in pairs(Config.shop_cfg_auto.department_store) do
		if self.paga_data[v.conver_type] == nil then
			self.paga_data[v.conver_type] = {}
		end
		if v.is_show == 1 then
			if v.show_level and v.show_level ~= "" then
				if RoleLevel() >= v.show_level then
					table.insert(self.paga_data[v.conver_type],v)
				end
			else
				table.insert(self.paga_data[v.conver_type],v)
			end
		end
	end
	-- 穹顶商店特殊处理
	if CloudArenaData and CloudArenaData.Instance then
		self:CheckPagaList(10, CloudArenaData.Instance:GetSeasonShopType())
	else
		self:CheckPagaList(10, 0)
	end
end

-- 穹顶商店特殊，需要收到协议后判断
function StoreData:CheckPagaList(page,value)
	self.paga_data[page] = {}
	for k,v in pairs(Config.shop_cfg_auto.department_store) do
		if v.conver_type == page and v.season_type == value and v.is_show == 1 and RoleLevel() >= v.show_level then
			table.insert(self.paga_data[v.conver_type],v)
		end
	end
end

function StoreData:InitPageBtnData()
	local list = {}
	for i,v in ipairs(Config.shop_cfg_auto.store_group) do
		if v.is_show == 1 then
			if  v.page ~= 4 or not GLOBAL_CONFIG:Flags().block_recharge then--屏蔽祈福兑换
				table.insert(list,v)
			end
		end
	end
	table.sort(list,function(a,b)
		return a.sort < b.sort
	end)
	for i,v in ipairs(list) do
		list[i].type = i
	end
    self.btn_list = list
end

-- 现在不需要分页了
function StoreData:GetPageData(type, count)
	local list = {}
	for i,v in ipairs(self.paga_data[type or 0] or {}) do
		if FlyUpData.Instance:GetStage() >= v.feisheng_level then --购买需要飞升阶段
			table.insert(list,v)
		end
	end
	table.sort(list,function(a,b)
		return self:GetSortFunction(a,b)
	end)
	return list
end

function StoreData:GetSortFunction(a,b)
	a.weight = a.sort
	b.weight = b.sort
	if a.limit_type > 0 and a.limit_convert_count - self:GetLimitTimes(a.conver_type,a.seq) == 0 then
		a.weight = 10000 + a.sort
	end
	if b.limit_type > 0  and b.limit_convert_count - self:GetLimitTimes(b.conver_type,b.seq) == 0 then
		b.weight = 10000 + b.sort
	end
	return a.weight < b.weight
end

--获取元宝商城物品
function StoreData:GetGoldItemData(type, count)
    local list = {}
    --分类
    for i, v in pairs(self.paga_data[type or 0] or {}) do
        if list[v.subtype] == nil then
            list[v.subtype] = {}
		end
		if not v.feisheng_level or FlyUpData.Instance:GetStage() >= v.feisheng_level then
			table.insert(list[v.subtype], v)
		end
    end
    --分好类之后第一个物品显示分类信息
    for i, v in pairs(list) do
        local first_index = next(v)
        local first_item = v[first_index]
        if first_item then
            first_item.type_text = Language.Common.ItemSubType[first_item.subtype or 4]
        end
    end
    --拆除分类
    local item_list = {}
    for i, v in pairs(list) do
        for _, item in pairs(v) do
            table.insert(item_list, item)
        end
    end
    --分组
    if count then
        return DataHelper.GetPageData(item_list, count)
    else
        return item_list
    end
end

function StoreData:GetDiamondItemData()
	local list = self:GetGoldItemData(DIAMOND_STORE_TYPE)
	local tab_list = {}
	local idx = 1
	for i, v in ipairs(list) do
		tab_list[idx] = tab_list[idx] or {}
		if #tab_list[idx] == 5 then
			idx = idx + 1
			tab_list[idx] = {}
		end
		table.insert(tab_list[idx], v)
	end
	return tab_list
end

--服务端数据下发
function StoreData:SetConvertShopBuyRecord(protocol)
	self.count_data:Set(protocol.buy_count)
end

--单个信息下发
function StoreData:SetConvertShopAddRecord(protocol)
	local info = protocol.count_info or {}
	if self.count_data[info.conver_type] == nil then
		self.count_data[info.conver_type] = SmartData.New{}
	end
	self.count_data[info.conver_type][info.seq] = info.count
	self.red_point_data.val = not self.red_point_data.val
end

function StoreData:GetLimitTimes(conver_type,seq)
	if self.count_data[conver_type] and self.count_data[conver_type][seq] then
		local times = self.count_data[conver_type][seq] or 0
		times = times <= 0 and 0 or times
		return times
	end
	return 0
end

function StoreData:GetShowType()
    return self:GetSelectData().price_type == 0 and 0 or 1
end

function StoreData:SetSelectData(data)
	self:SetSelectType(data.type or 1)
	self.select_data.page_data = data
end

function StoreData:GetSelectData()
	return self.select_data.page_data
end

function StoreData:GetDayNum()
	local open_day = TimeCtrl.Instance.server_data.server_open_day % 5
	return open_day == 0 and 5 or open_day
end

--商城总红点
function StoreData:GetRedNum()
	for k,v in pairs(self.btn_list) do
		if self:GetBtnRedNum(v) ~= 0 then
			return 1
		end
	end
	return 0
end

--商城按钮红点
function StoreData:GetBtnRedNum(data)
	if RoleLevel() < data.limit_level then
		return 0
	end
	for k,v in pairs(self.paga_data[data.page] or {}) do
		if self:GetBtnItemNum(v) ~= 0 then
			return 1
		end
	end
	return 0
end

--获取商城对应的红点
function StoreData:GetBtnPageRedNum(page)
	local data = self.paga_data[page] or {}
	if RoleLevel() < (data.limit_level or 0) then
		return 0
	end
	if self:GetBtnItemNum(data) ~= 0 then
		return 1
	end
	return 0
end

--商城道具红点
function StoreData:GetBtnItemNum(data)
	if data.price == 0 and not MallCtrl.IsCanRequire(data.require_type,data.require_value) then --免费道具
		if self:GetLimitTimes(data.conver_type,data.seq) == 0 then
			return 1
		end
	end
	if data.is_remind == 1 and not self.IsClickRemind and self:GetHasTimes(data) ~= 0 then
		if not MallCtrl.IsCanRequire(data.require_type,data.require_value) then
			if data.conver_item_id == 0 then
				if RoleData.Instance:GetCurrency(data.price_type) >= data.price then
					return 1
				end
			else
				if Item.GetNum(data.conver_item_id) >= data.price then
					return 1
				end
			end
		end
	end
	return 0
end

function StoreData:CheckRedPiont()
	self.red_point_data.val = not self.red_point_data.val
end

--跳转相关===========================================
function StoreData:SetSelectType(type)
	self.select_data.type = type
end

function StoreData:GetSelectType()
	return self.select_data.type or 1
end

function StoreData:SetSelctIndex(type,index)
	self.select_data.index[type] = index
end

function StoreData:GetSelctIndex(type)
	return self.select_data.index[type] or 0
end

--获取跳转的index处理
function StoreData:GetSelectIndex(page,item_id)
	local function GetType(page)
		for i,v in ipairs(self.btn_list) do
			if v.page == page then
				return i
			end
		end
	end
	local function GetIndex(page,item_id)
		for k,v in pairs(self.paga_data[page]) do
			if v.item_id == item_id then
				return math.ceil(k / 6) - 1
			end
		end
	end
	return GetType(page),GetIndex(page,item_id)
end

function StoreData:SetJumpIndex(config,item_id)
	local type,index = self:GetSelectIndex(config.conver_type or 0,item_id)
	self:SetSelectType(type or 1)
	self:SetSelctIndex(type or 1,index or 0)
	self:SetJumpItemId(item_id)
end

function StoreData:SetJumpItemId(item_id)
	self.jump_item_id = item_id
end

function StoreData:GetJumpItemId()
	return self.jump_item_id
end
--===========================================
--获取page所在index和配置
function StoreData:GetShopTypeIndex(page)
	for i,v in ipairs(self.btn_list) do
		if v.page == page then
			return i,v
		end
	end
end

--获取对应分类商店的配置
function StoreData:GetPageDataList(type)
	return self.paga_data[type] or {}
end

--获取对应分类商店对应物品配置
function StoreData:GetItemConfig(item_id,page)
	for k,v in pairs(self.paga_data[page or 0]) do
		if v.item_id == item_id then
			return v
		end
	end
end

--前置条件
function StoreData:GetItemConfigSeq(page,seq)
	for k,v in pairs(self.paga_data[page or 0]) do
		if v.seq == seq then
			return v
		end
	end
end

--获取当前限购次数
function StoreData:GetHasTimes(data)
	return data.limit_convert_count - self:GetLimitTimes(data.conver_type,data.seq)
end

-------------------新修改 商店改成商会-----------------------------
CommerceData = CommerceData or BaseClass()

function CommerceData:__init()
	if CommerceData.Instance ~= nil then
		Debuger.LogError("[CommerceData] attempt to create singleton twice!")
		return
	end
    CommerceData.Instance = self
    self:Initialization()
end

function CommerceData:__delete()
	CommerceData.Instance = nil
end

--封装商会的配置
function CommerceData:Initialization()
	self.commerce_list = {}
	for k,v in pairs(Config.shop_cfg_auto.mourinho) do
		if self.commerce_list[v.conver_type] == nil then
			self.commerce_list[v.conver_type] = {}
		end
		if self.commerce_list[v.conver_type][v.sub_type] == nil then
			self.commerce_list[v.conver_type][v.sub_type] = {}
		end
		if v.is_show == 1 then
			table.insert(self.commerce_list[v.conver_type][v.sub_type],v)
		end
	end
	
	self.commerce_title = {}
	for k,v in pairs(Config.shop_cfg_auto.jose_mourinho) do
		if v.is_show == 1 then
			if self.commerce_title[v.type] == nil then
				self.commerce_title[v.type] = {id = v.type,name = v.name,children = {},sort = v.type_sort}
			end
			local children = {
				type = v.type,
				name = v.sub_name,
				sub_type = v.sub_type
			}
			table.insert(self.commerce_title[v.type].children,children)
		end
	end
	self.commerce_title_sort = {}
	for k,v in pairs(self.commerce_title) do
		table.insert(self.commerce_title_sort,v)
	end
	table.sort(self.commerce_title_sort,DataHelper.SortFunc([[sort]]))
	self.item_list = KeyList(Config.shop_cfg_auto.mourinho,"item_id")
end

function CommerceData:GetSubData(type,sub_type)
	return self.commerce_list[type][sub_type]
end

function CommerceData:GetMourinhoCfgById(item_id)
	return self.item_list[item_id]
end 

--跳转相关=============================================================
function CommerceData:SetJumpData(data)
	self.commerce_jump_data = data
end

function CommerceData:GetJumpData()
	return self.commerce_jump_data
end

function CommerceData:SetJumpIndex(item_id)
	local config = self.item_list[item_id] or {}
	local parent,child = config.conver_type or 0,config.sub_type or 0
	local sub_list = self:GetSubData(parent,child)
	local index = DataHelper.SimpleIndex(sub_list,[[item_id]],item_id)
	self:SetJumpData({parent = parent,child = child,index = index})
end

function CommerceData:SetJumpSub(conver_type,sub_type)
	self:SetJumpData({parent = conver_type,child = sub_type})
end
--====================================================================