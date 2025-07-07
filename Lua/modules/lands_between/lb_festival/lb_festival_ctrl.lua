VIEW_REQ("modules/lands_between/lb_festival/lb_festival_view")
VIEW_REQ("modules/lands_between/lb_festival/lb_festival_data")
VIEW_REQ("modules/lands_between/lb_festival/lb_festival_treasure")
VIEW_REQ("modules/lands_between/lb_festival/lb_festival_treasure_data")
VIEW_REQ("modules/lands_between/lb_festival/lb_festival_treasure_buy_level")
VIEW_REQ("modules/lands_between/lb_festival/lb_festival_treasure_welfare")
VIEW_REQ("modules/lands_between/lb_festival/lb_festival_treasure_surplus_reward")

LBFestivalCtrl = LBFestivalCtrl or BaseClass(BaseCtrl)
function LBFestivalCtrl:__init()
	if LBFestivalCtrl.Instance ~= nil then
		Debuger.LogError("[LBFestivalCtrl] attempt to create singleton twice!")
		return
	end
	LBFestivalCtrl.Instance = self

    self.data = LBFestivalData.New()

    self:RegisterAllProtocols()
end 

function LBFestivalCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSGodSpaceUserSeasonPassOperReq)
	self:RegisterProtocol(SCGodSpaceUserSeasonPassFuliInfo,"OnGodSpaceUserSeasonPassFuliInfo")
	self:RegisterProtocol(SCGodSpaceSeasonPassInfo,"OnGodSpaceSeasonPassInfo")
	self:RegisterProtocol(SCGodSpaceUserSeasonPassBuyNumInfo,"OnGodSpaceUserSeasonPassBuyNumInfo")
end

function LBFestivalCtrl:OnGodSpaceUserSeasonPassFuliInfo(protocol)
	LogDG("林哥哥，这是神域宝藏的福利信息下发哦",protocol)
    self.data:SetFuliInfo(protocol)
end

function LBFestivalCtrl:OnGodSpaceUserSeasonPassBuyNumInfo(protocol)
	LogDG("林哥哥，这是神域狂欢宝藏购买次数下发哦",protocol)
    self.data:SetFuliBuyTimeInfo(protocol)
end

function LBFestivalCtrl:OnGodSpaceSeasonPassInfo(protocol)
	LogDG("林哥哥，这是神域宝藏的赛季信息下发哦",protocol)
    self.data:SetSeasonPassInfo(protocol)
end

function LBFestivalCtrl:SendFestivalReq(param_t)
	LogDG("林哥哥，这是神域宝藏的请求哦",param_t)
    local protocol = GetProtocol(CSGodSpaceUserSeasonPassOperReq)
	protocol.oper_type = param_t.oper_type or 0
	protocol.param_1 = param_t.param_1 or 0
	protocol.param_2 = param_t.param_2 or 0
    SendProtocol(protocol)
end

function LBFestivalCtrl:ReqTreasureInfo()
	self:SendFestivalReq({
		oper_type = LB_FESTIVALl_OPER_TYPE.INFO_REQ
	})
end

function LBFestivalCtrl:ReqTreasureReward(seq,type)
	self:SendFestivalReq({
		oper_type = LB_FESTIVALl_OPER_TYPE.FETCH_LEVEL_REWARD,
		param_1 = seq,
		param_2 = type,
	})
end

function LBFestivalCtrl:ReqTreasureBuyLevel(num)
	self:SendFestivalReq({
		oper_type = LB_FESTIVALl_OPER_TYPE.BUY_EXP,
		param_1 = num,
	})
end

function LBFestivalCtrl:ReqTreasureBuySurplus()
	self:SendFestivalReq({
		oper_type = LB_FESTIVALl_OPER_TYPE.FETCH_SURPLUS,
	})
end

function LBFestivalCtrl:ReqTreasureFetchPass(seq)
	LandsBetweenCtrl.Instance:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_FETCH_SEASON_PASS_FULI,
		param_1 = seq,
	})
end