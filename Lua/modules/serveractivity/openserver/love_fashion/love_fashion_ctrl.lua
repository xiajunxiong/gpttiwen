VIEW_REQ("modules/serveractivity/openserver/love_fashion/love_fashion_view")

LoveFashionCtrl = LoveFashionCtrl or BaseClass(BaseCtrl)

function LoveFashionCtrl:__init()
	if LoveFashionCtrl.Instance ~= nil then
		Debuger.LogError("[LoveFashionCtrl] attempt to create singleton twice!")
		return
	end	
	LoveFashionCtrl.Instance = self
	self.data = LoveFashionData.New()
	self:RegisterAllProtocols()
	Remind.Instance:Register(Mod.ServerActivity.LoveFashion,self.data.love_fashion_data,function()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.KF_LOVE_FASHION) then
			return 0
		end
		return self.data:GetRemindNum()
	end)
end

function LoveFashionCtrl:__delete()
	self:UnRegisterAllProtocols()
	LoveFashionCtrl.Instance = nil
	Delete(self, "data")
end

function LoveFashionCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRALoveFashion, "OnSCRALoveFashion")
end

function LoveFashionCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCRALoveFashion)
end

function LoveFashionCtrl:OnSCRALoveFashion(protocol)
	self.data:SetInfo(protocol)
end

--0 请求信息
--1 购买时装 p1: shop_index  （非直购）
--2 购买时装 p1：shop_index (直购)
--3 领取时装
--4 打开活动界面
function LoveFashionCtrl:SendReq(req_type,param1)
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_LOVE_FASHION,req_type,param1)
end

--------------------LoveFashionData--------------------
LoveFashionData = LoveFashionData or BaseClass()

function LoveFashionData:__init()
	if LoveFashionData.Instance ~= nil then
		Debuger.LogError("[LoveFashionData] attempt to create singleton twice!")
		return
	end
	LoveFashionData.Instance = self
	self.love_fashion_data = SmartData.New({is_click_sel = false,sel_value = 1,is_open = false})
	self.now_select_data = SmartData.New()
end

function LoveFashionData:OnClickSel(value)
	self.love_fashion_data.sel_value = value
	self.love_fashion_data:SetDirty("is_click_sel")
end

function LoveFashionData:GetRemindNum()
	if self.love_fashion_data.is_open then
		for i,v in pairs(self:GetConfig()) do
			if v.buy_mark and v.receive_mark == 0 then
				return 1
			end	
		end
	end
	return self.love_fashion_data.is_open and 0 or 1
end

function LoveFashionData:GetConfig()
	return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.KF_LOVE_FASHION,Config.qingyuanshizhuang_auto.gift_configure)
end

function LoveFashionData:SetInfo(protocol)
    self.love_fashion_data.is_open = protocol.is_open > 0
	for i,v in pairs(self:GetConfig()) do
		v.buy_mark = protocol.buy_mark[v.seq] > 0
		v.self_buy_mark = protocol.self_buy_mark[v.seq]
		v.receive_mark = protocol.fetch_mark[v.seq]
	end
	self.love_fashion_data:Notify()
end

function LoveFashionData:GetConfirmIndex() --有可领取的  则定位下标
	local index = 1
	for i,v in pairs(self:GetConfig()) do
		if v.buy_mark and v.receive_mark == 0 then
			index = i
			break
		end	
	end
	return index
end