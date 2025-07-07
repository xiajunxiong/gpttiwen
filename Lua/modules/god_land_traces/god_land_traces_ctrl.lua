
FILE_REQ("modules/god_land_traces/god_land_traces_config")
FILE_REQ("modules/god_land_traces/god_land_traces_data")

VIEW_REQ("modules/god_land_traces/god_land_traces_view")
VIEW_REQ("modules/god_land_traces/god_land_traces_guide_view")

GodLandTracesCtrl = GodLandTracesCtrl or BaseClass(BaseCtrl)

function GodLandTracesCtrl:__init()
    if GodLandTracesCtrl.Instance ~= nil then
        Debuger.LogError("[GodLandTracesCtrl] attempt to create singleton twice!")
        return
    end
    GodLandTracesCtrl.Instance = self
    
    self.data = GodLandTracesData.New()

    self:RegisterAllProtocols()

    self.func_rp = BindTool.Bind(self.data.GetRemindNum, self.data)
    RemindCtrl.Instance:Remind():Register(Mod.GodLandTraces.Main, self.data.info_sm, self.func_rp)
    RemindCtrl.Instance:Remind():Register(Mod.GodLandTraces.Main, BagData.Instance:GetColumnGrids(ItemColumnType.Material), self.func_rp)
end

function GodLandTracesCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSTianDiDaoHenReq)

	self:RegisterProtocol(SCTianDiDaoHenRoleInfo, "OnTianDiDaoHenRoleInfo")
	self:RegisterProtocol(SCTianDiDaoHenOpenInfo, "OnTianDiDaoHenOpenInfo")
end

function GodLandTracesCtrl:OnTianDiDaoHenRoleInfo(protocol)
    self.data:SetTianDiDaoHenRoleInfo(protocol)
end

function GodLandTracesCtrl:OnTianDiDaoHenOpenInfo(protocol)
    self.data:SetTianDiDaoHenOpenInfo(protocol)
end

function GodLandTracesCtrl:SendTianDiDaoHenReq(param_t)
	local protocol = GetProtocol(CSTianDiDaoHenReq)
	protocol.req_type = param_t.req_type or 0
	protocol.param1 = param_t.param1 or 0
	protocol.param2 = param_t.param2 or 0
	SendProtocol(protocol)
end

function GodLandTracesCtrl:SendTianDiDaoHenReqActive(index)
	local param_t = {
        req_type = GodLandTracesConfig.ReqType.active,
        param1 = index,
    }
	self:SendTianDiDaoHenReq(param_t)
end

function GodLandTracesCtrl:SendTianDiDaoHenReqFetch()
	local param_t = {
        req_type = GodLandTracesConfig.ReqType.fetch,
    }
	self:SendTianDiDaoHenReq(param_t)
end