FILE_REQ("modules/lands_between/lb_shop/lb_shop_data")
VIEW_REQ("modules/lands_between/lb_shop/lb_shop_view")

LBShopCtrl = LBShopCtrl or BaseClass(BaseCtrl)
function LBShopCtrl:__init()
	if LBShopCtrl.Instance ~= nil then
		Debuger.LogError("[LBShopCtrl] attempt to create singleton twice!")
		return
	end
	LBShopCtrl.Instance = self

	self.data = LBShopData.New()
	self:RegisterAllProtocols()

end

function LBShopCtrl:RegisterAllProtocols()
	
	self:RegisterProtocol(SCGodSpaceUserShopInfo,"RecvGodSpaceUserShopInfo")
	self:RegisterProtocol(SCGodSpaceUserBuffInfo,"RecvGodSpaceUserBuffInfo")
	self:RegisterProtocol(SCGodSpaceUserBuffShop,"RecvGodSpaceUserBuffShop")
end

function LBShopCtrl:RecvGodSpaceUserShopInfo(protocol)
	LogDG("--------------林哥哥，这是神域商店的下发哦",protocol)
	self.data:SetShopInfo(protocol)
end

function LBShopCtrl:RecvGodSpaceUserBuffInfo(protocol)
	LogDG("---------------林哥哥，这是神域Buff信息的下发哦",protocol)
	self.data:SetBuffInfo(protocol)
end

function LBShopCtrl:RecvGodSpaceUserBuffShop(protocol)
	LogDG("----------------林哥哥，这是神域Buff商店的下发哦",protocol)
	self.data:SetBuffShop(protocol)
end

-- 请求刷新
function LBShopCtrl:ReqFlushShop()
	LandsBetweenCtrl.Instance:GodSpaceOperate(
		{
			oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_USER_SHOP_REFRESH,
		}
	)
end

-- 请求购买
function LBShopCtrl:ReqBuyShopItem(param_t)
	LandsBetweenCtrl.Instance:GodSpaceOperate(
		{
			oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_USER_SHOP_BUY_ITEM,
			param_1 = param_t.index
		}
	)
end

-- 请求领取奖励
function LBShopCtrl:ReqDailyShopReward()
	LandsBetweenCtrl.Instance:GodSpaceOperate(
		{
			oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_USER_SHOP_FETCH_DAILY,
		}
	)
end

-- 请求购买buff
function LBShopCtrl:ReqBuyBuff(param_t)
	LandsBetweenCtrl.Instance:GodSpaceOperate(
		{
			oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_BUFF_SHOP_BUY,
			param_1 = param_t.seq,
			param_2 = param_t.num,
		}
	)
end