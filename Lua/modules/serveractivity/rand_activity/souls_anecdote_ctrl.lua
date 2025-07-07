VIEW_REQ("modules/serveractivity/rand_activity/souls_anecdote_view")

SoulsAnecdoteCtrl = SoulsAnecdoteCtrl or BaseClass(BaseCtrl)

function SoulsAnecdoteCtrl:__init()
	if SoulsAnecdoteCtrl.Instance ~= nil then
		Debuger.LogError("[SoulsAnecdoteCtrl] attempt to create singleton twice!")
		return
	end	
    SoulsAnecdoteCtrl.Instance = self
	self:RegisterAllProtocols()
end

function SoulsAnecdoteCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSSoulsAnecdoteBuyItemReq)
    self:RegisterProtocol(CSSoulsAnecdoteQuestionReq)
	-- self:RegisterProtocol(CSSoulsAnecdoteCloseWindosReq)
	-- self:RegisterProtocol(SCSoulsAnecdoteCloseWindosRet, "OnSoulsAnecdoteCloseWindosRet")
	-- self:RegisterProtocol(SCSoulsAnecdoteShopBuyTimesInfo, "OnSoulsAnecdoteShopBuyTimesInfo")
    self:RegisterProtocol(SCSoulsAnecdoteShopInfo, "OnSoulsAnecdoteShopInfo")
	self:RegisterProtocol(SCSoulsAnecdoteQuestionInfo, "OnSoulsAnecdoteQuestionInfo")
	self:RegisterProtocol(SCSoulsAnecdoteFbObjInfo, "OnSoulsAnecdoteFbObjInfo")
end

function SoulsAnecdoteCtrl:SendSplaceBuyItemReq(param_t)
	local protocol = ProtocolPool.Instance:GetProtocol(CSSoulsAnecdoteBuyItemReq)
	protocol.item_idx = param_t.item_idx or 0
	protocol.npc_obj_id = param_t.npc_obj_id or 0
	SendProtocol(protocol)
end

function SoulsAnecdoteCtrl:OnSoulsAnecdoteQuestionInfo(protocol)
    SplaceAnecdoteData.Instance:SplaceAnecQuestionInfo(protocol)
	ViewMgr:OpenView(SplaceAnecQuestView)
end

function SoulsAnecdoteCtrl:SendSplaceQuestionReq(param_t)
	local protocol = ProtocolPool.Instance:GetProtocol(CSSoulsAnecdoteQuestionReq)
	protocol.answer_id = param_t.answer_id or 0
	protocol.npc_obj_id = param_t.npc_obj_id or 0
	SendProtocol(protocol)
end

-- function SoulsAnecdoteCtrl:SendCloseWindosReq(param_t)
-- 	local protocol = ProtocolPool.Instance:GetProtocol(CSSoulsAnecdoteCloseWindosReq)
-- 	protocol.reason = param_t.reason or 0
-- 	SendProtocol(protocol)
-- end

function SoulsAnecdoteCtrl:OnSoulsAnecdoteShopInfo(protocol)
	SplaceAnecdoteData.Instance:SplaceAnecShopInfo(protocol)
	ViewMgr:OpenView(SplaceAnecShopView)
end 

function SoulsAnecdoteCtrl:OnSoulsAnecdoteShopBuyTimesInfo(protocol)
	SplaceAnecdoteData.Instance:SplaceAnecShopBuyTimeInfo(protocol)
end 

function SoulsAnecdoteCtrl:OnCloseWindosRet(protocol)
	SplaceAnecdoteData.Instance:SetCloseWindos(protocol)
end

function SoulsAnecdoteCtrl:OnSoulsAnecdoteFbObjInfo(protocol)
    FightData.Instance:SetSplaceAnecInfo(protocol)
end