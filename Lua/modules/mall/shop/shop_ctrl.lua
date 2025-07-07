FILE_REQ("modules/mall/shop/shop_data")
FILE_REQ("modules/mall.shop/shopping")
VIEW_REQ("modules/mall/shop/shop_view")
VIEW_REQ("modules/mall/shop/variety_store_view")
VIEW_REQ("modules/mall/shop/gold_store_view")
VIEW_REQ("modules/mall/shop/diamond_store_view")

--商店
ShopCtrl = ShopCtrl or BaseClass(BaseCtrl)

function ShopCtrl:__init()
	if ShopCtrl.Instance ~= nil then
		Debuger.LogError("[ShopCtrl] attempt to create singleton twice!")
		return
	end
	ShopCtrl.Instance = self
	self.data = ShopData.New()
	self:RegisterAllProtocols()
	self.ctrl = ShoppingCtrl.New()
end

function ShopCtrl:__delete()
	self:UnRegisterAllProtocols()
	ShopCtrl.Instance = nil
	Delete(self,"data")
	Delete(self,"ctrl")
end

function ShopCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSGoldShopBuyReq)
	self:RegisterProtocol(CSGoldShopNotifyCancelReq)
	self:RegisterProtocol(SCGoldShopAllInfo,"OnGoldShopAllInfo")
	self:RegisterProtocol(SCGoldShopSingleInfo,"OnGoldShopSingleInfo")
	self:RegisterProtocol(SCGoldShopPurchaseNotifys,"OnGoldShopPurchaseNotifys")
	--注册红点逻辑
	--RemindCtrl.Instance:Remind():Register(Mod.Mall.Shop,self.data.notify_data,BindTool.Bind(self.CheckRemind, self))
end

function ShopCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSAddItemOnShelve)
	self:UnRegisterProtocol(CSGoldShopNotifyCancelReq)
	self:UnRegisterProtocol(SCGoldShopAllInfo)
	self:UnRegisterProtocol(SCGoldShopSingleInfo)
	self:UnRegisterProtocol(SCGoldShopPurchaseNotifys)
	--RemindCtrl.Instance:Remind():UnRegister(Mod.Mall.Shop)
end

--请求购买商品
function ShopCtrl:SendGoldShopBuyReq(index,num)
	local protocol = GetProtocol(CSGoldShopBuyReq)
	protocol.index = index or 0
	protocol.num = num or 1
	protocol.is_another_buy = ViewMgr:IsOpen(AnotherView) and 1 or 0
	SendProtocol(protocol)
end

--红点需求 点击之后消失红点 重新登录不出现
function ShopCtrl:SendGoldShopNotifyCancelReq(cancel_index)
	local protocol = GetProtocol(CSGoldShopNotifyCancelReq)
	protocol.cancel_index = cancel_index or 0
	SendProtocol(protocol)
end

--下发全部信息
function ShopCtrl:OnGoldShopAllInfo(protocol)
	self.data:SetGoldShopAllInfo(protocol)
end

--下发单个信息
function ShopCtrl:OnGoldShopSingleInfo(protocol)
	self.data:SetGoldShopSingleInfo(protocol)
end

function ShopCtrl:OnGoldShopPurchaseNotifys(protocol)
	self.data:SetGoldShopPurchaseNotifys(protocol)
end

--红点监听
function ShopCtrl:CheckRemind()
	return self.data:GetShopRemindNum()
end

function ShopCtrl:SetSelectIndex(item_id)
	local type,index = self.data:GetItemIndexByType(item_id)
	self.data:SetSelectType(type or 1)
	self.data:SetSelctItemIndex(type or 1,index or 1)
end

--商店跳转到某个物品[item_id:物品id]
function ShopCtrl:OpenSelectIndex(item_id)
	self:SetSelectIndex(item_id)
	ViewMgr:OpenViewByKey(Mod.Mall.Shop)
end