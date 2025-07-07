MarketData = MarketData or BaseClass()

function MarketData:__init()
	if MarketData.Instance ~= nil then
		Debuger.LogError("[MarketData] attempt to create singleton twice!")
		return
	end
	MarketData.Instance = self
	self.count_data = SmartData.New({list = {},val = false})
	self:Initialization()
	self:InitMarketData()
	self.cache_list = {}
end

function MarketData:__delete()
	MarketData.Instance = nil
end

function MarketData:InitMarketData()
	self.shop_list = {}
	for k,v in pairs(Config.shop_cfg_auto.market) do
		if self.shop_list[v.conver_type] == nil then
			self.shop_list[v.conver_type] = {}
		end
		if self.shop_list[v.conver_type][v.sub_type] == nil then
			self.shop_list[v.conver_type][v.sub_type] = {}
		end
		if v.is_show == 1 then
			table.insert(self.shop_list[v.conver_type][v.sub_type],v)
		end
	end
	self.shop_title = {}
	for k,v in pairs(Config.shop_cfg_auto.market_mourinho) do
		if v.is_show == 1 then
			if self.shop_title[v.type] == nil then
				self.shop_title[v.type] = {id = v.type,name = v.name,children = {}}
			end
			local children = {
				type = v.type,
				name = v.sub_name,
				sub_type = v.sub_type
			}
			table.insert(self.shop_title[v.type].children,children)
		end
	end
	self.item_list = KeyList(Config.shop_cfg_auto.market,"item_id")
end

--获取出售列表数据
function MarketData:GetSellList()
	local list = {}
    for col, item_list in pairs(BagData.Instance.item_list or {}) do
		if col ~= ItemColumnType.Storage and col ~= ItemColumnType.Temporary then
			for k, item in pairs(item_list) do
				if self.item_list[item.item_id] ~= nil then
					table.insert(list,self:GetParam(item))
				end
			end
		end
    end
	table.sort(list,function(a,b)
		return a.param.index < b.param.index
	end)
	for i,v in pairs(FuShiData.Instance.fu_shi_list) do
		if v.item_id ~= 0 then
			table.insert(list,self:GetParam(v))
		end
	end
	return list
end

function MarketData:GetParam(item)
	if self.cache_list[item] == nil then
		self.cache_list[item] = CfgTableCopy(self.item_list[item.item_id])
		self.cache_list[item].param = item
	else
		self.cache_list[item].param = item
	end
	return self.cache_list[item]
end

function MarketData:Initialization()
	self.select_data = SmartData.New({list = {},val = false})
	self.cache_list = {}
end

function MarketData:GetSubData(type,sub_type)
	local list = {}
	for i,v in ipairs(self.shop_list[type][sub_type] or {}) do
		if FlyUpData.Instance:GetStage() >= v.feisheng_level then --购买需要飞升阶段
			table.insert(list,v)
		end
	end
	return list
end

function MarketData:SetMarketAllInfo(protocol)
	self.count_data.list = protocol.info_list
end

function MarketData:SetMarketSingleInfo(protocol)
	self.count_data.list[protocol.item_id] = protocol.info
	self.count_data.val = not self.count_data.val
end

function MarketData:GetInfo(item_id)
	return self.count_data.list[item_id] or {}
end

function MarketData:GetCount(item_id)
	local info = self:GetInfo(item_id)
	return info.count or 0
end

--设置商店选择数据
function MarketData:SetSelectData(data)
	self.select_data.list = data
	self.select_data.val = not self.select_data.val
end

--获取商店选择数据
function MarketData:GetSelectData()
	return self.select_data.list
end

function MarketData:SetSelectType(type)
	self.select_data.type = type
end

function MarketData:GetSelectType()
	return self.select_data.type or 1
end

--获取浮动值
function MarketData:GetFloatValue(data)
	local count = self:GetCount(data.item_id)
	if self:GetCount(data.item_id) == 0 then
		return 0
	end

	local info = self:GetInfo(data.item_id)
	local yerterday_price = info.yerterday_price or data.price
	if yerterday_price >= data.max_price and count > 0 then
		return 0
	end
	if yerterday_price <= data.min_price and count < 0 then
		return 0
	end

	local count,negative = math.abs(self:GetCount(data.item_id)),GameMath.Negative(self:GetCount(data.item_id))
	local float_value = math.floor(count/data.business_amount) * data.float_range / 10000
	return Valve(float_value,self.GetOther().float_limit_max / 10000) * negative
end

--获取购买价格
function MarketData:GetPrice(data)
	if data == nil then
		return 0
	end
	if data.price == nil then
		return 0
	end
	--return math.floor(self:GetFloatValue(data) * data.price + data.price)
	local info = self:GetInfo(data.item_id)
	return info.cur_price or data.price or 0
end

function MarketData:GetOther()
	return Config.shop_cfg_auto.market_other[1]
end

--获取出售价格
function MarketData:GetSellPrice(data)
	local info = self:GetInfo(data.item_id)
	local yerterday_price = info.yerterday_price or data.price
	--local sell_price = math.floor((self:GetFloatValue(data) + self.GetOther().sell_price_scale / 10000) * yerterday_price)
	local rate = 1 + self:GetFloatValue(data)
	if yerterday_price > data.max_price then
		yerterday_price = data.max_price
	end
	if yerterday_price < data.min_price then
		yerterday_price = data.min_price
	end
	local sell_price = math.floor(rate * yerterday_price * self.GetOther().sell_price_scale / 10000)
	return sell_price
end

function MarketData:GetNeedGoldNum(price)
	return math.ceil((price - RoleData.Instance:GetCurrency(CurrencyType.ImmortalCoin))/Item.GetConfig(CommonItemId.ImmortalCoinId).param2)
end

function MarketData:IsMarketSell(item,is_param)
	if self.item_list[item.item_id] and is_param then
		self:SetSelectData(self:GetParam(item))
	end
    return self.item_list[item.item_id] ~= nil
end

function MarketData:GetJumpData()
	return self.jump_data
end

function MarketData:SetJumpData(data)
	self.jump_data = data
end

function MarketData:SetJumpIndex(item_id)
	local config = self.item_list[item_id] or {}
	local parent,child = config.conver_type or 0,config.sub_type or 0
	local sub_list = self:GetSubData(parent,child)
	local index = DataHelper.SimpleIndex(sub_list,[[item_id]],item_id)
	self:SetJumpData({parent = parent,child = child,index = index})
end

function MarketData:GetItemPrice(item_id)
	return self:GetPrice(self.item_list[item_id])
end