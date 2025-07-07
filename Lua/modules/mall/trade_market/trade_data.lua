TradeData = TradeData or BaseClass()

function TradeData:__init()
	if TradeData.Instance ~= nil then
		Debuger.LogError("[TradeData] attempt to create singleton twice!")
		return
	end
    TradeData.Instance = self
	
	self.congfig = Config.trade_market_auto
	--交易所配置
	self.constant = self.congfig.exchange_constant[1]
	--筛选配置
	self.filtrate = self.congfig.trade_line_filtrate
	--出售上架的列表
	self.sell_data = SmartData.New({list = {}})
	--正在出售中记录
	self.item_sell_data = SmartData.New({list = {}})
	--购买数据列表
	self.shop_data = SmartData.New({shop_list = SmartData.New({{},{}}),focus_list = SmartData.New({{},{}})})
	--交易记录
	self.record_data = SmartData.New({record_list = SmartData.New({{},{}})})
	--最近记录
	self.deal_record_data = SmartData.New({list = {}})

	self.sell_list = {}

	self.select_data = SmartData.New({list = {}})

	self.big_search_data = SmartData.New({list = {}})

	self.credit_data = SmartData.New({val = 0})

	self.pet_price_config = TabGroup(self.congfig.Trading_fioor_price,[[id]],[[level]])

	self.recommend_data = SmartData.New({info = {},val = false})

	self:InitSpecialList()
end

function TradeData:__delete()
	TradeData.Instance = nil
end

----------------------服务端数据下发----------------------
--下发购买页面
function TradeData:SetTradeMarketSearchResult(protocol)
	self:SetPageData(protocol.cur_page + 1,protocol.total_page)
	self.shop_data.shop_list[protocol.announce] = protocol.item_list
	self.shop_data.announce = protocol.announce
end

--下发出售列表
function TradeData:SetTradeMarketUserShelveInfo(protocol)
	self.sell_data.list = protocol.item_list
end

--下发物品交易记录
function TradeData:SetRecentTransactionRecord(protocol)
	self.deal_record_data.list = protocol.record_list
end

--下发关注列表 区分购买 + 公示
function TradeData:SetTradeMarketGetFocusList(protocol)
	self.shop_data.focus_list = protocol.focus_list
end

--下发单个关注信息
function TradeData:SetTradeMarketGetFocusItemInfo(protocol)
	local focus_info = protocol.focus_info
	local is_replace = false
	for i,v in ipairs(self.shop_data.focus_list[self:GetAnnounce()]) do
		if v.upshelve_time == focus_info.upshelve_time then
			v = focus_info
			is_replace = true
		end
	end
	if not is_replace then
		table.insert(self.shop_data.focus_list[self:GetAnnounce()],focus_info)
	end
end

--下发正在售卖中
function TradeData:SetTradeMarketGetSellingList(protocol)
	self.item_sell_data.list = protocol.record_list
end

--下发玩家记录
function TradeData:SetTradeMarketGetRoleTransactionRecord(protocol)
	self.record_data.record_list[1] = DataHelper.ReverseList(protocol.buy_list)
	self.record_data.record_list[2] = DataHelper.ReverseList(protocol.sold_list)
	self.record_data.gold_list = protocol.gold_list
end

function TradeData:SetTradeMarketGetMerchandiseNum(protocol)
	self.big_search_data.list = protocol.info_list
end

--交易信用
function TradeData:SetTradeMarketCreditInfo(protocol)
	self.credit_data.val = protocol.total_credit
end

--获取出售列表
function TradeData:GetSellListData()
	local list = TableCopy(self.sell_data.list)
	if #list < GameEnum.TRADE_LIST_SHOW_MAX_NUM then
		for i=#list + 1 ,GameEnum.TRADE_LIST_SHOW_MAX_NUM do
			list[i] = {item_id = 0}
		end
	end
	return list
end

--获取关注列表
function TradeData:GetFocusList()
	return self.shop_data.focus_list[self.shop_data.announce or 1] or {}
end

--获取购买列表 后面还可能要改协议
function TradeData:GetShopList()
    return self.shop_data.shop_list[self.shop_data.announce or 1] or {}
end

--获取交易记录
function TradeData:GetRecordList()
    return self.deal_record_data.list or {}
end

--正在售卖中列表
function TradeData:GetSellRecordList()
    return self.item_sell_data.list
end

--设置页数
function TradeData:SetPageData(cur_page,total_page)
    self.cur_page,self.total_page = cur_page or 0,total_page or 0
end

function TradeData:SetCurPage(cur_page)
	self.cur_page = cur_page or 0
end

--获取页数
function TradeData:GetPage()
    return self.cur_page or 0,self.total_page or 0
end

--设置页面类型
function TradeData:SetViewType(type)
    self.select_view_type = type
end

--页面类型
function TradeData:GetViewType()
    return self.select_view_type or 1
end

--设置排序类型
function TradeData:SetOrderType(type)
	self.select_order_type = type
end

function TradeData:GetOrderType()
	return self.select_order_type
end

--公示类型
function TradeData:GetAnnounce()
    return self:GetViewType() == 1 and 1 or 2
end

--是否公示
function TradeData:IsAnnounce()
    return self:GetViewType() == 1 and 0 or 1
end

function TradeData:SetSelectItemData(data)
	self.select_item_data = data
	self.select_data.list = data
end

function TradeData:GetSelectItemData()
	return self.select_item_data
end

--获取购买列表封装数据
function TradeData:GetShopData(is_focus)
	local list = self:GetShopList()
	if is_focus then
		list = self:GetFocusList()
	end
	if #list == 0 then
		return list
	else
		for i=#list + 1 ,GameEnum.TRADE_LIST_SHOW_MAX_NUM do
			list[i] = {item_id = 0,flush_index = 0}
		end
	end
	return list
end

--获取玩家自身记录
function TradeData:GetRoleRecordList()
	return self.record_data.record_list
end

--是否关注
function TradeData:IsFocusList(time)
	for k,v in pairs(self:GetFocusList()) do
		if v.upshelve_time == time then
			return true
		end
	end
	return false
end

function TradeData:SetSellList(list)
	self.sell_list = list
end

function TradeData:GetSellList()
	return self.sell_list or {}
end

-------------------------配置数据-----------------------
--获取物品类型列表
function TradeData:GetBtnItemList()
	local list = {}
	for k,v in pairs(self.congfig.trade_line_item_type) do
		if v.is_show == 1 then
			table.insert(list,v)
		end
	end
	table.sort(list,DataSorter.KeyLowerSorter("sort"))
	return list
end

--获取选择列表
function TradeData:GetSelectData(type,config)
	local list = {}
	for i,v in ipairs(self.filtrate or {}) do
		if v.item_type == type then
			table.insert(list,v)
		end
	end
	if config and config.is_show_screen == 2 then
		table.insert(list,1,{big_type = config.seq,name = Language.Trade.All})
	end
	return list
end

--获取上架手续费
function TradeData:GetSellFee(price,price_type)
	return self:GetLimitNum(price,self.constant.upper_shelf_price_min,self.constant.upper_shelf_price_max)
end

--获取广告费
function TradeData:GetNoticeFree(price,price_type)
	local free = price * self.constant.advert_price_rate / 1000
	if price_type == 1 then
		return self:GetLimitNum(free,self.constant.gold_advert_min,self.constant.gold_advert_max)
	else
		return self:GetLimitNum(free,self.constant.advert_price_rate,self.constant.immortal_coin_advert_min)
	end
end

function TradeData:GetLimitNum(num,min,max)
	if num <= min then
		return min
	elseif num > min and num <= max then
		return num
	else
		return max
	end
end

--根据页数或者对应数据 搜索
function TradeData:GetPageData(list,index)
	if list and #list > 0 then
		local data = {}
		for i=1,GameEnum.TRADE_LIST_SHOW_MAX_NUM do
			table.insert(data,list[i+(index-1)*GameEnum.TRADE_LIST_SHOW_MAX_NUM] or {item_id = 0,flush_index = 0})
		end
		return data
	end
	return {}
end

function TradeData:GetDesc()
    return Config.language_cfg_auto.textdesc[4].desc
end

--获取类型配置
function TradeData:GetTypeData(type)
	local list = {}
	for i,v in ipairs(self.filtrate or {}) do
		if v.item_type == type then
			v.flush_index = MallConfig.TradeShop.Type
			table.insert(list,v)
		end
	end
	return list
end

--等级配置
function TradeData:GetLevelData()
	local list = {}
	for i=4,self.constant.equip_limit_level do
		list[i] = {level = i,name = "Lv." .. (i - 1) * 10}
	end
	return list
end

--筛选职业
function TradeData:GetProfData()
	local list = {
		[1] = {level = -1,name = Language.Trade.All},
		[2] = {level = 0,name = Language.Trade.Common}
	}
	for i,v in ipairs(Language.Common.Prof) do
		table.insert(list,{level = i,name = v})
	end
	return list
end

function TradeData:GetFindList(name)
	local list = {}
	for k,v in pairs(Item.GetItemFindList(name)) do
		if v.item_trade_money_type ~= 0 and Item.GetBigType(v.id) ~= ITEM_BIG_TYPE.MEDAL then
			if v.trade_need_announcement == self:IsAnnounce() then
				table.insert(list,{
					item_id = v.id,
					name = v.name,
					color = v.color + 100,
					flush_index = MallConfig.TradeShop.Search
				})
			end
		end
	end
	--令牌特效搜索结果
	for k,v in pairs(self.special_config) do
		if string.find(v.desc,name) ~= nil then
			table.insert(list, v)
		end
	end
	--搜索大分类
	for k,v in pairs(self.filtrate) do
		if string.find(v.name,name) ~= nil and v.item_type == 15 then
			v.flush_index = MallConfig.TradeShop.Type
			table.insert(list,v)
		end
	end
	table.sort(list,function(a,b)
		if (a.color or a.seq or 0) == (b.color or b.seq or 0) then
			return (a.item_id or 0) < (b.item_id or 0)
		end
		return (a.color or a.seq or 0) < (b.color or b.seq or 0)
	end)
	if #list > 0 then
		for i=#list + 1 ,GameEnum.TRADE_LIST_SHOW_MAX_NUM do
			list[i] = {item_id = 0,flush_index = 0}
		end
	end	
	return list
end

--红点提醒
function TradeData:GetTradeRemindNum()
	local num = 0
	for i,v in ipairs(self.sell_data.list) do
		if v.sold_num > 0 or v.cur_status == SELLVIEW_TYPE.SHELVED then
			num = num + 1
		end
	end
	return num
end

function TradeData:ClearData()
	self.sell_list = {}
end

function TradeData:SetSearchItemId(item_id)
	self.search_item_id = item_id
end

function TradeData:GetSearchItemId()
	return self.search_item_id
end

function TradeData:GetSearchType()
	return self.search_type
end

function TradeData:SetSearchType(type_seq)
	self.search_type = type_seq
end

function TradeData:GetBtnSelectType(seq)
	for k,v in pairs(self:GetBtnItemList()) do
		if v.seq == seq then
			return k
		end
	end
end

--获取大类型配置
function TradeData:GetItemTypeConfig(big_type)
	for k,v in pairs(self.congfig.trade_line_item_type) do
		if v.seq == big_type then
			return v
		end
	end
end

--传闻跳转到玩家物品
function TradeData:SetOpenLinkData(param_t)
	if param_t then
		local data = {}
		data.item_name = param_t[1]
		data.seller_uid = param_t[2]
		data.sale_index = param_t[3]
		data.put_on_shelve_time = param_t[4]
		data.price = param_t[5]
		data.item_id = param_t[6]
		data.unique_key = param_t[7]
		self.open_link_data = data
		if self.open_link_list == nil then
			self.open_link_list = {}
		end
		self.open_link_list[param_t[4]] = data
	else
		self.open_link_data = nil
	end
end

function TradeData:SetOpenLinkList(upshelve_time)
	if upshelve_time == nil then return end
	if self.open_link_list == nil then return end
	self.open_link_data = self.open_link_list[upshelve_time]
end

function TradeData:GetOpenLinkData()
	return self.open_link_data
end

function TradeData:IsOpenLink(upshelve_time)
	if self.open_link_data == nil then return false end
	return self.open_link_data.put_on_shelve_time == tostring(upshelve_time)
end

--交易行背包物品跳转到出售界面
function TradeData:SetJumpItemData(data)
	self.jump_item_data = data
end

function TradeData:GetJumpItemData()
	return self.jump_item_data
end

function TradeData:IsMaxFocus()
	local num = 0
	for k,v in pairs(self:GetFocusList()) do
		if v.item_id == nil then
			num = num + 1
		end
	end
	return num >= 8
end

--当前装备对比数据
function TradeData:SetCompCellData(param)
	self.contrast_param = param
end

--是否有熔炼
function TradeData:HasSmelt(attr_type,attr_value)
	if self.contrast_param == nil then 
		return false
	end
	for k,v in pairs(self.contrast_param.attr_extra_inc or {}) do
		if v.attr_type == attr_type then
			return v.attr_value < attr_value
		end
	end
	return true
end

--是否有提升
function TradeData:HasPromote(attr_type,attr_value)
	if self.contrast_param == nil then 
		return false
	end
	for k,v in pairs(self.contrast_param.attr_list) do
		if v.attr_type == attr_type then
			return v.attr_value < attr_value
		end
	end
	return true
end

function TradeData:GetTradeHelpList()
	local list = {}
	for i=28,34 do
		list[i] = {desc = "         " .. Config.language_cfg_auto.textdesc[i].desc}
	end
	return list
end

--获取筛选大类型
function TradeData:GetItemType(item_id)
	local search_type = Item.GetSearchType(item_id) or 0
	local config = self.filtrate[search_type+1]
	return config and config.item_type or -1
end

function TradeData:GetPublicityList()
	local list = {}
	for k,v in pairs(self.congfig.trade_line_item_type) do
		if v.is_show == 1 and v.is_show_publicity == 1 then
			table.insert(list,v)
		end
	end
	table.sort(list,DataSorter.KeyLowerSorter("sort"))
	return list
end

function TradeData:GetBigSearchNum(samll_type)
	return self.big_search_data.list[samll_type] or 0
end

function TradeData:GetSelectIndex(item_type)
	for k,v in pairs(self:GetBtnItemList()) do
		if v.seq == item_type then
			return k
		end
	end
end

function TradeData:GetFiltrateConfig(search_type)
	for k,v in pairs(self.filtrate) do
		if v.seq == search_type then
			return v
		end
	end
end

--获取交易信用
function TradeData:GetCreditNum()
	return self.credit_data.val or 0
end

-------------------------令牌-----------------------
function TradeData:InitSpecialList()
	local list = {}
	self.special_config = {}
	for k,v in pairs(Config.special_effect_auto.spe_list) do
		if v.is_medal_effect == 1 then
			local config = MedalData.Instance:GetRankCfg(v.sp_rank)
			if config then
				if list[v.sp_type] == nil then
					list[v.sp_type] = {}
				end
				if list[v.sp_type][config.color] == nil then
					local param_t = TradeData.SpecialStruct(config,v)
					list[v.sp_type][config.color] = param_t
					table.insert(self.special_config,param_t)
				end
			end
		end
	end
end

function TradeData:SetSelectBigType(type)
	self.select_big_type = type
end

function TradeData:GetSelectBigType(type)
	return self.select_big_type
end

function TradeData.SpecialStruct(config,data)
	return {
		color = config.color,
		item_id = config.item_icon,
		name = config.item_name,
		desc = data.name,
		icon_name = config.market_type_icon,
		special_type = data.sp_type,
		flush_index = MallConfig.TradeShop.Type
	}
end

function TradeData:GetPetMinPrice(id,level)
	if self.pet_price_config[id] and self.pet_price_config[id][level] then
		return self.pet_price_config[id][level].min_price
	end
end

function TradeData:GetPetMaxPrice(id,level)
	if self.pet_price_config[id] and self.pet_price_config[id][level] then
		return self.pet_price_config[id][level].max_price
	end
end

--推荐价格
function TradeData:SetQueryItemRecommendPriceResp(protocol)
	self.recommend_data.info = protocol.info
	self.recommend_data.val = not self.recommend_data.val
end

function TradeData:GetQueryItemRecommendInfo()
	return self.recommend_data.info
end