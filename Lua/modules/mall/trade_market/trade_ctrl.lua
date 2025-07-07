FILE_REQ("modules/mall/trade_market/trade_data")
VIEW_REQ("modules/mall/trade_market/trade_view")

--交易所
TradeCtrl = TradeCtrl or BaseClass(BaseCtrl)

function TradeCtrl:__init()
	if TradeCtrl.Instance ~= nil then
		Debuger.LogError("[TradeCtrl] attempt to create singleton twice!")
		return
	end	
    TradeCtrl.Instance = self
    self.data = TradeData.New()
	self:RegisterAllProtocols()
end

function TradeCtrl:OnUnloadGameLogic()
	self.data:ClearData()
end

function TradeCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSAddItemOnShelve)
	self:RegisterProtocol(CSRemoveItemOnShelve)
	self:RegisterProtocol(CSTradeMarketCashOut)
	self:RegisterProtocol(CSTradeMarketSearchByItemID)
	self:RegisterProtocol(CSTradeMarketSearchByItemSubType)
	self:RegisterProtocol(CSTradeMarketUserShelveData)
	self:RegisterProtocol(CSTradeMarketBuyItem)
	self:RegisterProtocol(CSRecentTransactionRecords)
	self:RegisterProtocol(CSTradeMarketGetFocusList)
	self:RegisterProtocol(CSTradeMarketAddFocus)
	self:RegisterProtocol(CSTradeMarketRemoveFocus)
	self:RegisterProtocol(CSTradeMarketGetFocusItemInfo)
	self:RegisterProtocol(CSTradeMarketAdvertise)
	self:RegisterProtocol(CSTradeMarketSearchAdvertise)
	self:RegisterProtocol(CSTradeMarketGetSellingList)
	self:RegisterProtocol(CSTradeMarketGetRoleTransactionRecord)
	self:RegisterProtocol(CSTradeMarketRedoAddItemOnShelve)
	self:RegisterProtocol(CSTradeMarketGetMerchandiseNum)
	self:RegisterProtocol(CSTradeMarketSearchByMedalEffectType)
	self:RegisterProtocol(CSQueryItemRecommendPrice)

	self:RegisterProtocol(SCTradeMarketUserShelveData,"OnTradeMarketUserShelveInfo")
	self:RegisterProtocol(SCTradeMarketSearchResult,"OnTradeMarketSearchResult")
	self:RegisterProtocol(SCRecentTransactionRecords,"OnRecentTransactionRecords")
	self:RegisterProtocol(SCTradeMarketGetFocusList,"OnTradeMarketGetFocusList")
	self:RegisterProtocol(SCTradeMarketGetFocusItemInfo,"OnTradeMarketGetFocusItemInfo")
	self:RegisterProtocol(SCTradeMarketGetSellingList,"OnTradeMarketGetSellingList")
	self:RegisterProtocol(SCTradeMarketGetRoleTransactionRecord,"OnTradeMarketGetRoleTransactionRecord")
	self:RegisterProtocol(SCTradeMarketRefreshNotice,"OnTradeMarketRefreshNotice")
	self:RegisterProtocol(SCTradeMarketGetMerchandiseNum,"OnTradeMarketGetMerchandiseNum")
	self:RegisterProtocol(SCTradeMarketCreditInfo,"OnTradeMarketCreditInfo")
	self:RegisterProtocol(SCQueryItemRecommendPriceResp,"OnQueryItemRecommendPriceResp")
	--注册红点逻辑
	RemindCtrl.Instance:Remind():Register(Mod.Mall.Trade,self.data.sell_data,BindTool.Bind(self.CheckRemind, self))
end

function TradeCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSAddItemOnShelve)
    self:UnRegisterProtocol(CSRemoveItemOnShelve)
    self:UnRegisterProtocol(CSTradeMarketCashOut)
    self:UnRegisterProtocol(CSTradeMarketSearchByItemID)
    self:UnRegisterProtocol(CSTradeMarketSearchByItemSubType)
    self:UnRegisterProtocol(CSTradeMarketUserShelveData)
    self:UnRegisterProtocol(CSTradeMarketBuyItem)
	self:UnRegisterProtocol(CSRecentTransactionRecords)
	self:UnRegisterProtocol(CSTradeMarketGetFocusList)
	self:UnRegisterProtocol(CSTradeMarketAddFocus)
	self:UnRegisterProtocol(CSTradeMarketRemoveFocus)
	self:UnRegisterProtocol(CSTradeMarketGetFocusItemInfo)
	self:UnRegisterProtocol(CSTradeMarketAdvertise)
	self:UnRegisterProtocol(CSTradeMarketSearchAdvertise)
	self:UnRegisterProtocol(CSTradeMarketGetSellingList)
	self:UnRegisterProtocol(CSTradeMarketGetRoleTransactionRecord)
	self:UnRegisterProtocol(CSTradeMarketRedoAddItemOnShelve)
	self:UnRegisterProtocol(CSTradeMarketGetMerchandiseNum)
	self:UnRegisterProtocol(CSTradeMarketSearchByMedalEffectType)
	self:UnRegisterProtocol(CSQueryItemRecommendPrice)

    self:UnRegisterProtocol(SCTradeMarketUserShelveData)
    self:UnRegisterProtocol(SCTradeMarketSearchResult)
    self:UnRegisterProtocol(SCRecentTransactionRecords)
    self:UnRegisterProtocol(SCTradeMarketGetFocusList)
    self:UnRegisterProtocol(SCTradeMarketGetFocusItemInfo)
    self:UnRegisterProtocol(SCTradeMarketGetSellingList)
	self:UnRegisterProtocol(SCTradeMarketGetRoleTransactionRecord)
	self:UnRegisterProtocol(SCTradeMarketRefreshNotice)
	self:UnRegisterProtocol(SCTradeMarketGetMerchandiseNum)
	self:UnRegisterProtocol(SCTradeMarketCreditInfo)
	self:UnRegisterProtocol(SCQueryItemRecommendPriceResp)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Mall.Trade)
end

function TradeCtrl:__delete()
	self:UnRegisterAllProtocols()
	TradeCtrl.Instance = nil
	Delete(self,[[data]])
end

--上架物品
function TradeCtrl:SendAddItemReq(param_t)
	local protocol = GetProtocol(CSAddItemOnShelve)
	protocol.is_from_pet = param_t.is_from_pet or 0
	protocol.index = param_t.index or 0              
	protocol.column = param_t.column or 0       
	protocol.item_num = param_t.item_num or 1      
	protocol.price = param_t.price or 0
	SendProtocol(protocol)
end

--下架物品
function TradeCtrl:SendRemoveItemReq(sale_index)
	local protocol = GetProtocol(CSRemoveItemOnShelve)
	protocol.sale_index = sale_index or 0
	SendProtocol(protocol)
end

--取回物品
function TradeCtrl:SendSellItemReq(sale_index)
	local protocol = GetProtocol(CSTradeMarketCashOut)
	protocol.sale_index = sale_index or 0
	SendProtocol(protocol)
end

--交易所请求
function TradeCtrl:SendTradeMarketReq()
	local protocol = GetProtocol(CSTradeMarketUserShelveData)
	SendProtocol(protocol)
end

--按物品ID搜索 数据请求请求
function TradeCtrl:SendSearchByIDReq(item_id,req_page)
	local protocol = GetProtocol(CSTradeMarketSearchByItemID)
	protocol.item_id = item_id or 0
    protocol.req_page = req_page or self.data:GetPage() - 1
	protocol.is_ammounce = self.data:IsAnnounce()
	protocol.order_type = self.data:GetOrderType() and 1 or 0
	self.SendSearchSeq = function()SendProtocol(protocol)end
	SendProtocol(protocol)
end

--大类型搜索
function TradeCtrl:SendSearchBigTypeReq(type,page)
	self:SendSearchByTypeReq(nil,type,nil,page)
end

--小类型搜索
function TradeCtrl:SendSearchSmallTypeReq(type,level,page)
	self:SendSearchByTypeReq(type,nil,level,page)
end

--按物品类型搜索 数据请求请求
function TradeCtrl:SendSearchByTypeReq(item_sub_type,item_type,level_param,req_page)
	local protocol = GetProtocol(CSTradeMarketSearchByItemSubType)
	protocol.item_sub_type = item_sub_type or 0  --小类型搜索
    protocol.req_page = req_page or self.data:GetPage() - 1
	protocol.is_ammounce = self.data:IsAnnounce()
	protocol.order_type = self.data:GetOrderType() and 1 or 0	
	protocol.item_type = item_type or -1        --大类型搜索
	protocol.level_param = level_param or -1     --等级搜索
	self.SendSearchSeq = function()SendProtocol(protocol)end
	SendProtocol(protocol)
end

--购买物品
function TradeCtrl:SendBuyItemReq(param,buy_num,expect_price)
	if param == nil or param.item == nil then return end
	local protocol = GetProtocol(CSTradeMarketBuyItem)
	protocol.seller_uid = param.seller_uid or 0
	protocol.sale_index = param.sale_index or 0
	protocol.buy_num = buy_num or 1
	protocol.upshelve_time = param.upshelve_time or 0
	protocol.buy_item_id = param.item.item_id or 0
	protocol.expect_price = expect_price or param.price
	protocol.unique_key = param.unique_key or ""
	SendProtocol(protocol)
	if self.SendSearchSeq then
		self.SendSearchSeq()
	end
end

--外部使用交易行购买
function TradeCtrl:SendCommonBuyReq(param,buy_num,expect_price)
	local protocol = GetProtocol(CSTradeMarketBuyItem)
	protocol.seller_uid = param.seller_uid or 0
	protocol.sale_index = param.sale_index or 0
	protocol.upshelve_time = param.upshelve_time or 0
	protocol.buy_item_id = param.item_data.item_id or 0
	protocol.expect_price = expect_price or param.price
	protocol.unique_key = param.unique_key or ""
	protocol.buy_num = buy_num or 1
	SendProtocol(protocol)
end

--交易记录
function TradeCtrl:SendRecordReq(item_id)
	local protocol = GetProtocol(CSRecentTransactionRecords)
	protocol.item_id = item_id or 0
	SendProtocol(protocol)
end

--查询关注列表
function TradeCtrl:SendGetFocusListReq()
	local protocol = GetProtocol(CSTradeMarketGetFocusList)
	self.SendSearchSeq = function()SendProtocol(protocol)end
	SendProtocol(protocol)
end

--添加关注
function TradeCtrl:SendAddFocusReq(seller_uid,sale_index,upshelve_time)
	local protocol = GetProtocol(CSTradeMarketAddFocus)
	protocol.seller_uid = seller_uid or 0
	protocol.sale_index = sale_index or 0
	protocol.upshelve_time = upshelve_time or 0
	SendProtocol(protocol)
end

--取消关注
function TradeCtrl:SendRemoveFocusReq(focus_index)
	local protocol = GetProtocol(CSTradeMarketRemoveFocus)
	protocol.focus_index = focus_index or 0
	protocol.cur_page = self.data:GetPage()
	protocol.is_ammounce = self.data:IsAnnounce()
	SendProtocol(protocol)
end

--获取关注
function TradeCtrl:SendGetFocusReq(focus_index)
	local protocol = GetProtocol(CSTradeMarketGetFocusItemInfo)
	protocol.focus_index = focus_index or 0
	protocol.cur_page = self.data:GetPage()
	protocol.is_ammounce = self.data:IsAnnounce()
	SendProtocol(protocol)
end

--发送广告
function TradeCtrl:SendAdvertiseReq(sale_index)
	local protocol = GetProtocol(CSTradeMarketAdvertise)
	protocol.sale_index = sale_index or 0
	SendProtocol(protocol)
end

--传闻查找广告上的商品
function TradeCtrl:SendSearchAdvertiseReq(param_t)
	param_t = param_t or self.data:GetOpenLinkData()
	local protocol = GetProtocol(CSTradeMarketSearchAdvertise)
	protocol.seller_uid = param_t.seller_uid or 0
	protocol.put_on_shelve_time = param_t.put_on_shelve_time or 0
	protocol.price = param_t.price or 0
	protocol.sale_index = param_t.sale_index or 0
	protocol.item_id = param_t.item_id or 0
	protocol.unique_key = param_t.unique_key or ""
	SendProtocol(protocol)
end

--查询正在售卖的某物品
function TradeCtrl:SendGetSellingListReq(item_id,special_type)
	local protocol = GetProtocol(CSTradeMarketGetSellingList)
	protocol.item_id = item_id or 0
	protocol.special_type = special_type or 0 --特效类型（搜令牌时显示）
	SendProtocol(protocol)
end

--成交记录
function TradeCtrl:SendRoleRecordReq()
	local protocol = GetProtocol(CSTradeMarketGetRoleTransactionRecord)
	SendProtocol(protocol)
end

--重新上架
function TradeCtrl:SendRedoAddItemOnShelveReq(sale_index,price)
	local protocol = GetProtocol(CSTradeMarketRedoAddItemOnShelve)
	protocol.sale_index = sale_index or 0
	protocol.price = price or 0
	SendProtocol(protocol)
end

--请求每个大类中所有小类目前有多少商品
function TradeCtrl:SendGetMerchandiseNumReq(big_search_type)
	local protocol = GetProtocol(CSTradeMarketGetMerchandiseNum)
	protocol.is_announce = self.data:IsAnnounce()
	protocol.big_search_type = big_search_type or 0
	SendProtocol(protocol)
end

--按物品类型搜索 数据请求请求
function TradeCtrl:SendSearchByMedalTypeReq(special_type,color,req_page)
	local protocol = GetProtocol(CSTradeMarketSearchByMedalEffectType)
	protocol.special_type = special_type or 0
	protocol.color = color or 0
    protocol.req_page = req_page or self.data:GetPage() - 1
	protocol.is_ammounce = self.data:IsAnnounce()
	protocol.order_type = self.data:GetOrderType() and 1 or 0
	self.SendSearchSeq = function()SendProtocol(protocol)end
	SendProtocol(protocol)
end

--查询物品的推荐价格
function TradeCtrl:SendRecommendPrice(param_t,is_from_pet)
	local protocol = GetProtocol(CSQueryItemRecommendPrice)
	protocol.is_from_pet = is_from_pet or 0
	protocol.index = param_t.index or 0
	protocol.column = param_t.column_type or 0       
	SendProtocol(protocol)
end

----------------------服务端数据下发----------------------
--玩家交易所个人数据
function TradeCtrl:OnTradeMarketUserShelveInfo(protocol)
	self.data:SetTradeMarketUserShelveInfo(protocol)
end

--玩家交易所个人数据
function TradeCtrl:OnTradeMarketSearchResult(protocol)
	self.data:SetTradeMarketSearchResult(protocol)
end

--交易记录下发 最近
function TradeCtrl:OnRecentTransactionRecords(protocol)
	self.data:SetRecentTransactionRecord(protocol)
end

--关注列表下发
function TradeCtrl:OnTradeMarketGetFocusList(protocol)
	self.data:SetTradeMarketGetFocusList(protocol)
end

--单个关注下发
function TradeCtrl:OnTradeMarketGetFocusItemInfo(protocol)
	self.data:SetTradeMarketGetFocusItemInfo(protocol)
end

--正在售卖下发
function TradeCtrl:OnTradeMarketGetSellingList(protocol)
	self.data:SetTradeMarketGetSellingList(protocol)
end

--下发玩家记录
function TradeCtrl:OnTradeMarketGetRoleTransactionRecord(protocol)
	self.data:SetTradeMarketGetRoleTransactionRecord(protocol)
end

--返回每个大类中所有小类目前有多少商品
function TradeCtrl:OnTradeMarketGetMerchandiseNum(protocol)
	self.data:SetTradeMarketGetMerchandiseNum(protocol)
end

--交易信用
function TradeCtrl:OnTradeMarketCreditInfo(protocol)
	self.data:SetTradeMarketCreditInfo(protocol)
end

--推荐价格
function TradeCtrl:OnQueryItemRecommendPriceResp(protocol)
	self.data:SetQueryItemRecommendPriceResp(protocol)
end

function TradeCtrl:CheckRemind()
	if LoginData.Instance:IsOnCrossSever() then
		return 0
	end
	return self.data:GetTradeRemindNum()
end

--打开购买弹窗
function TradeCtrl:OnenPurchaseView()
	local data = self.data:GetSelectItemData()
    if data and data.item then
		ViewMgr:OpenView((data.item:IsEquip() or data.item:IsJewelry() or data.item:IsMedal())and 
		PurchaseEquip or PurchaseView,data)
    end
end

function TradeCtrl:OnTradeMarketRefreshNotice()
	if ViewMgr:IsOpen(PurchaseView) then
		ViewMgr:CloseView(PurchaseView)
	end
	if ViewMgr:IsOpen(PurchaseEquip) then
		ViewMgr:CloseView(PurchaseEquip)
	end
	if self.SendSearchSeq then
		self.SendSearchSeq()
	end
end