--商店
ShopData = ShopData or BaseClass()

function ShopData:__init()
	if ShopData.Instance ~= nil then
		Debuger.LogError("[ShopData] attempt to create singleton twice!")
		return
	end
	ShopData.Instance = self

	self.count_data = SmartData.New()
	self.limit_buy_data = SmartData.New()
	self.notify_data = SmartData.New()

	self.limit_item_list = {}

	self:FormatData()
end

function ShopData:__delete()
	ShopData.Instance = nil
end

--初始化数据
function ShopData:Initialization()
	self.select_data = SmartData.New({list = {},type = 1,index = {}})
end

--商店页面数据
function ShopData:FormatData()
	self.shop_page_data = {}
	for i,v in ipairs(Config.shop_cfg_auto.gold_shop) do
		if self.shop_page_data[v.page] == nil then
			self.shop_page_data[v.page] = {}
		end
		table.insert(self.shop_page_data[v.page],v)
		if v.limit_times > 0 then
			table.insert(self.limit_item_list,v)
		end
	end
	for k,v in pairs(Config.shop_cfg_auto.shop_group) do
		if v.is_show == 1 then
			self.shop_page_data[v.page] = self:GetShopData(v.page)
		end
	end
	self:Initialization()
end

--服务端下发数据
function ShopData:SetGoldShopAllInfo(protocol)
	self.count_data = SmartData.New(protocol.shop_buy_count)
end

--单个信息下发
function ShopData:SetGoldShopSingleInfo(protocol)
	self.count_data[protocol.index] = protocol.count
end

function ShopData:SetTimeLimitStoreInfo(protocol)
	self.limit_buy_data = SmartData.New(protocol.limit_buy_count)
end

--红点显示flag下发
function ShopData:SetGoldShopPurchaseNotifys(protocol)
	self.notify_data:Set(protocol.notify_flag)
end

--获取已经购买的次数
function ShopData:GetLimitTimes()
	local data = self:GetSelectData()
	if data == nil then 
		return 0 
	end
	if self:IsLimitType() and self:IsTimeLimit() then
		return self.limit_buy_data[data.seq] or 0
	end
	return self.count_data[data.index] or 0
end

--设置商店选择数据
function ShopData:SetSelectData(data)
	self.select_data.list = data
end

--获取商店选择数据
function ShopData:GetSelectData()
	return self.select_data.list
end

function ShopData:SetSelectType(type)
	self.select_data.type = type
end

--商店标签选择类型
function ShopData:GetSelectType()
	return self.select_data.type
end

function ShopData:SetSelctItemIndex(type,index)
	self.select_data.index[type] = index
end

function ShopData:GetSelctItemIndex(type)
	return self.select_data.index[type] or 1
end

--获取商店配置
function ShopData:GetShopData(type)
	if self:IsTimeLimit() and self.shop_page_data[type] == nil then
		self.shop_page_data[type] = ServerActivityData.Instance:GetActivityOpenCfg("time_limited_buy","seq")
	end
	local list = self.shop_page_data[type] or {}
	table.sort(list,function(a,b)
		a.weight = a.seq
		b.weight = b.seq
		if a.limit_times > 0 then
			a.weight = -1000 + a.seq
		end
		if b.limit_times > 0 then
			b.weight = -1000 + b.seq
		end
		return a.weight < b.weight
	end)
	return list
end

--商店标签配置
function ShopData:GetTypeData()
	local data = {}
	for i,v in ipairs(Config.shop_cfg_auto.shop_group) do
		if v.is_show == 1 then
			table.insert(data,v)
		end
	end
	if self:IsTimeLimit() then
		table.insert(data,{page = 4,name = Language.Shop.TimeLimit})
	end
	return data
end

function ShopData:IsTimeLimit()
	return ActivityData.Instance:GetActivityStatus(ACTIVITY_TYPE.RAND_TIME_LIMIT_BUY) == ActStatusType.Open
end

function ShopData:IsLimitType()
	return self.select_data.type == 4
end

--根据物品id获取快速商店配置
function ShopData.GetFastBuyData(item_id)
	return Config.shop_cfg_auto.fast_buy[item_id]
end

function ShopData:GetNotifyFlag(index)
	if self.notify_data then
		return self.notify_data[index] or 0
	end
	return 0
end

function ShopData:GetShopRemindNum()
	local num = 0
	for k,v in pairs(self.limit_item_list) do
		num = num + self:GetNotifyFlag(v.index)
	end
	return num
end

--通过标签类型和物品id找到item的位置
function ShopData:GetItemIndexById(type,item_id)
	for k,v in pairs(self.shop_page_data[type]) do
		if v.item_id == item_id then
			return k
		end
	end
end

--通过id找到标签的位置
function ShopData:GetItemIndexByType(item_id)
	for k,v in pairs(Config.shop_cfg_auto.shop_group) do
		if v.is_show == 1 then
			local index = self:GetItemIndexById(v.page,item_id)
			if index ~= nil then
				return v.page,index
			end
		end
	end
end