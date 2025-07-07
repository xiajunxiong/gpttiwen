--兑换标签页
ExchangePage = {
	Chivalrous = 3,--侠义值
	Grocery = 1,   --杂货店
	Prestige = 2   --声望商店
}
ExchangeData = ExchangeData or BaseClass()

function ExchangeData:__init()
	if ExchangeData.Instance ~= nil then
		Debuger.LogError("[ExchangeData] attempt to create singleton twice!")
		return
	end
	ExchangeData.Instance = self
	self:Initialization()
	self:InItPageData()
end

function ExchangeData:__delete()
	ExchangeData.Instance = nil
end

--初始化数据
function ExchangeData:Initialization()
	self.select_data = SmartData.New({list = {},type = 1,village_id = 0,index = {}})
end

function ExchangeData:InItPageData()
	self.page_data_list = {}
	for i,v in ipairs(Language.Exchange.BtnTypeList) do
		self.page_data_list[i] = self:GetExchangePageData(i)
	end
end

----------服务端下发数据-------------
function ExchangeData:SetExchangeAllInfo(protocol)
	self.count_data = SmartData.New(protocol.buy_count)
end

--单个信息下发
function ExchangeData:SetExchangeSingleInfo(protocol)
	if self.count_data then
		self.count_data[protocol.seq] = protocol.count
	end
end

--获取已经购买的次数
function ExchangeData:GetLimitTimes()
	local data = self:GetSelectData()
	local type = self:GetSelectType()
	if type == ExchangePage.Chivalrous then
		return self.count_data[data.seq] or 0
	elseif type == ExchangePage.Grocery then
		return PrestigeData.Instance:GetCommonShopBuyCount(data.index)
	else
		return PrestigeData.Instance:GetShopBuyCount(data.index)
	end
end

--设置选中的数据
function ExchangeData:SetSelectData(data)
	self.select_data.list = data
end

--获取选中的数据
function ExchangeData:GetSelectData()
	return self.select_data.list
end

function ExchangeData:SetSelectType(type)
	self.select_data.type = type
end

--商店标签选择类型
function ExchangeData:GetSelectType()
	return self.select_data.type
end

function ExchangeData:SetSelctItemIndex(type,index)
	self.select_data.index[type] = index
end

function ExchangeData:GetSelctItemIndex(type)
	return self.select_data.index[type] or 1
end

function ExchangeData:SetSelectVillage(village_id)
	self.select_data.village_id = village_id
end

--获取侠义值
function ExchangeData:GetChivalrous()
	local value = RoleData.Instance:GetCurrency(CurrencyType.Chivalrous)
	local limit = Config.sundries_auto.constant[1].chivalrous or 0
	return Format(Language.Exchange.Chivalrous,value,limit)
end

--侠义值获取途径
function ExchangeData:GetChivalrousWayList()
	local list = {}
	local key = {"ZuDui","HuoBan"}
	local config = Config.getway_cfg_auto.get_way
	for i=1,2 do
		local info = config[146 + i]
		info.icon_name = key[i]
		list[i] = info
	end
	return list
end

--新修改---------------------------------

--商店标签配置
function ExchangeData:GetTypeData()
	local list = {}
	for i,v in ipairs(Language.Exchange.BtnTypeList) do
		list[i] = {page = i,name = v}
	end
	return list
end

function ExchangeData:GetExchangePageData(type)
	if type == ExchangePage.Chivalrous then
		return Config.exchange_cfg_auto.exchange_page
	elseif type == ExchangePage.Grocery then
		return self:GetGroceryData()
	else
		return self:GetPrestigeData(1)--目前是村子为1
	end
end

function ExchangeData:GetPageDataList(type)
	return self.page_data_list[type] or {}
end

--获取杂货店数据
function ExchangeData:GetGroceryData()
	local list = Config.shop_cfg_auto.shop
	table.sort(list,function(a,b)return a.seq < b.seq end)
	return list
end

--获取声望商店数据 家族贡献
function ExchangeData:GetPrestigeData(village_id)
	village_id = village_id or self.select_data.village_id
	return CacheList(self,"prestige_list",village_id,function()
		local list = {}
		for i,v in ipairs(Config.prestige_cfg_auto.shop) do
			if v.village_id == village_id then
				table.insert(list,v)
			end
		end
		return list
	end)
end

function ExchangeData:GetNpcId()
	return 40570
end

--通过标签类型和物品id找到item的位置
function ExchangeData:GetItemIndexById(type,item_id)
	for k,v in pairs(self.page_data_list[type]) do
		if v.item_id == item_id then
			return k
		end
	end
end

function ExchangeData:GetItemIndexByType(item_id)
	for i,v in ipairs(Language.Exchange.BtnTypeList) do
		local index = self:GetItemIndexById(i,item_id)
		if index ~= nil then
			return i,index
		end
	end
end