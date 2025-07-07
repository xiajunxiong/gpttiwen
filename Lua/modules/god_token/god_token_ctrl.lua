--封神战令操作 --ACTIVITY_TYPE == 2081
FILE_REQ("modules/god_token/god_token_data")
VIEW_REQ("modules/god_token/god_token_view")
VIEW_REQ("modules/god_token/god_token_reward_info_view")
VIEW_REQ("modules/god_token/god_token_buy_level_view")
VIEW_REQ("modules/god_token/god_token_tip")
VIEW_REQ("modules/god_token/god_token_surplus_reawrd_view")
GodTokenCtrl = GodTokenCtrl or BaseClass(BaseCtrl)

RA_GOD_TOKEN_OP_TYPE = 
{
	RA_GOD_TOKEN_OP_TYPE_INFO = 0,  			    --信息下发
	RA_GOD_TOKEN_OP_TYPE_FETCH_COMMON = 1,  		--领取普通奖励 p1:seq
	RA_GOD_TOKEN_OP_TYPE_FEICH_SENIOR = 2,  		--领取高级奖励 p1:seq
	RA_GOD_TOKEN_OP_TYPE_BUY_EXP = 3, 				--购买等级     p1:count
	RA_GOD_TOKEN_OP_TYPE_EXCHANGE = 4, 				--兑换  	   p1:seq
	RA_GOD_TOKEN_OP_TYPE_FETCH_SURPKUS = 5, 		--领取溢出奖励
	RA_GOD_TOKEN_OP_TYPE_FETCH_DAILY_TASK = 6, 		--领取每日任务奖励 p1: 协议的数组下标 0 开始
	RA_GOD_TOKEN_OP_TYPE_FEICH_WEEKLY_TASK = 7, 	--领取每周任务奖励 p1: 协议的数组下标 0 开始
}

function GodTokenCtrl:__init()
	if GodTokenCtrl.Instance ~= nil then
		Debuger.LogError("[GodTokenCtrl] attempt to create singleton twice!")
		return
	end	
	GodTokenCtrl.Instance = self
	self.data = GodTokenData.New()
	self:RegisterProtocol(SCRAGodToKenInfo,"OnRAGodToKenInfo")
	RemindCtrl.Instance:Remind():Register(Mod.Reward.GodToken,self.data.god_token_data,function()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_GOD_TOKEN) then
			return 0
		end
		return self.data:GetRemindNum()
	end)
end

function GodTokenCtrl:__delete()
	self.data = nil
	RemindCtrl.Instance:Remind():UnRegister(Mod.Reward.GodToken)
	self:UnRegisterProtocol(SCRAGodToKenInfo)
	GodTokenCtrl.Instance = nil
end

--请求活动信息下发
function GodTokenCtrl:ReqInfo()
	local param_t = {}
	param_t.rand_activity_type = ACTIVITY_TYPE.RAND_GOD_TOKEN
	param_t.opera_type = RA_GOD_TOKEN_OP_TYPE.RA_GOD_TOKEN_OP_TYPE_INFO
	ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

--请求购买高级战令
function GodTokenCtrl:BuyGodToken()
	local fee = self.data:GetGodTokenPrice() / 10
	if ServerActivityCtrl.Instance:IsCanItemBuy(BUY_ITEM_TYPE.TYPE_GOD_TOKEN, fee) then
		ServerActivityCtrl.Instance:SendBuyCmdReq(BUY_ITEM_TYPE.TYPE_GOD_TOKEN, fee)
	else
		ChannelAgent:RequestPay(fee, BUY_ITEM_TYPE.TYPE_GOD_TOKEN)
	end
end

--请求操作
function GodTokenCtrl:SendReq(opera_type,p1,p2,p3)
	local param_t = {}
	param_t.rand_activity_type = ACTIVITY_TYPE.RAND_GOD_TOKEN
	param_t.opera_type = opera_type
	param_t.param_1 = p1 or 0
	param_t.param_2 = p2 or 0
	param_t.param_3 = p3 or 0
	ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

--战令信息协议回调
function GodTokenCtrl:OnRAGodToKenInfo(protocol)
	self.data:SetGodTokenInfo(protocol)
end

--领取奖励
function GodTokenCtrl:GetReawrd(seq)
	local ordinary_state = GodTokenData.Instance:GetOrdinaryRewardState(seq)
	if ordinary_state == GodTokenData.REWARD_STATE.CAN then
		GodTokenCtrl.Instance:SendReq(RA_GOD_TOKEN_OP_TYPE.RA_GOD_TOKEN_OP_TYPE_FETCH_COMMON,seq)
	end
	if GodTokenData.Instance:IsBuyToken() then
		local senior_state = GodTokenData.Instance:GetSeniorRewardState(seq)
		if senior_state == GodTokenData.REWARD_STATE.CAN then
			GodTokenCtrl.Instance:SendReq(RA_GOD_TOKEN_OP_TYPE.RA_GOD_TOKEN_OP_TYPE_FEICH_SENIOR,seq)
		end
	end
end