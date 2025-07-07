

FILE_REQ("modules/invite_fight/invite_fight_data")
VIEW_REQ("modules/invite_fight/invite_fight_view")

InviteFightCtrl = InviteFightCtrl or BaseClass(BaseCtrl)

InviteFightCtrl.ReqType = {
	BeginFight = 0,
	GetReward = 1,
}

function InviteFightCtrl:__init()
	if InviteFightCtrl.Instance ~= nil then
		Debuger.LogError("[InviteFightCtrl] attempt to create singleton twice!")
		return
	end
	InviteFightCtrl.Instance = self
    self.data = InviteFightData.New()
	self:RegisterAllProtocols()
end

function InviteFightCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSCapacityTargetReq)
	self:RegisterProtocol(SCCapacityTargetInfo,"OnSCCapacityTargetInfo")
	-- RemindCtrl.Instance:Remind():Register(Mod.Illustrate.Collect,self.data.included_red_piont,BindTool.Bind(self.CheckIncludedRemind, self))
end

function InviteFightCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSCapacityTargetReq)
	self:UnRegisterProtocol(SCCapacityTargetInfo)
	-- RemindCtrl.Instance:Remind():UnRegister(Mod.Illustrate.Collect)
end

function InviteFightCtrl:__delete()
	self:UnRegisterAllProtocols()
    InviteFightCtrl.Instance = nil
end

function InviteFightCtrl:OnSCCapacityTargetInfo(protocol)
	-- LogError("InviteFightCtrl:OnSCCapacityTargetInfo==",protocol)
	self.data:OnSCCapacityTargetInfo(protocol)
	if self.data:CheckAndClearkWaitDataFlag() then
		ViewMgr:OpenView(InviteFightView)
	end
end

function InviteFightCtrl:OnUnloadGameLogic()
	self.data:CheckAndClearkWaitDataFlag()
	-- self.data:ClearData()
end


function InviteFightCtrl:BeginFightReq()
	local protoc = GetProtocol(CSCapacityTargetReq)
	protoc.req_type = InviteFightCtrl.ReqType.BeginFight
	SendProtocol(protoc)
end

function InviteFightCtrl:GetRewardReq()
	-- LogError("GET_REWARD===!!!===",self.data:CurState())
	local protoc = GetProtocol(CSCapacityTargetReq)
	protoc.req_type = InviteFightCtrl.ReqType.GetReward
	SendProtocol(protoc)
end
