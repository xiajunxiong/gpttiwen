FILE_REQ("modules/guild_building/guild_building_data")

VIEW_REQ("modules/guild_building/guild_building_view")

GuildBuildingCtrl = GuildBuildingCtrl or BaseClass(BaseCtrl)

function GuildBuildingCtrl:__init()
    if GuildBuildingCtrl.Instance ~= nil then
        Debuger.LogError("[GuildBuildingCtrl] attempt to create singleton twice!")
        return
    end
    GuildBuildingCtrl.Instance = self
    self.data = GuildBuildingData.New()
    self:OnRemind()
    self:RegisterAllProtocols()
end

function GuildBuildingCtrl:OnRemind()
    RemindCtrl.Instance:Remind():Register(
        Mod.Guild.GuildBuilding,
        self.data.remind_data,
        BindTool.Bind(self.AllRemind, self)
    )
end

function GuildBuildingCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCGuildConstructInfo, "OnSCGuildConstructInfo")
    self:RegisterProtocol(CSGuildConstructReq)
    self:RegisterProtocol(SCGuildBuildingFinishInfo, "OnSCGuildBuildingFinishInfo")
    self:RegisterProtocol(SCGuildUseBuildMaterialInfo, "OnSCGuildUseBuildMaterialInfo")
    self:RegisterProtocol(SCGuildConstructAllInfo, "OnSCGuildConstructAllInfo")
    self:RegisterProtocol(SCGuildSpeedUpInfo, "OnSCGuildSpeedUpInfo")
    self:RegisterProtocol(SCGuildConstructTimeInfo, "OnSCGuildConstructTimeInfo")
    self:RegisterProtocol(SCGuildConstructOpenSUPInfo, "OnSSCGuildConstructOpenSUPInfo")
end

function GuildBuildingCtrl:__delete()
    --注销网络事件
    self:UnRegisterProtocol(SCGuildConstructInfo)
    self:UnRegisterProtocol(CSGuildConstructReq)
    self:UnRegisterProtocol(SCGuildBuildingFinishInfo)
    self:UnRegisterProtocol(SCGuildUseBuildMaterialInfo)
    self:UnRegisterProtocol(SCGuildConstructAllInfo)
    self:UnRegisterProtocol(SCGuildSpeedUpInfo)
    self:UnRegisterProtocol(SCGuildConstructTimeInfo)
    self:UnRegisterProtocol(SCGuildConstructOpenSUPInfo)
    
    RemindCtrl.Instance:Remind():UnRegister(Mod.Guild.GuildBuilding)
    if GuildBuildingCtrl.Instance == self then
        GuildBuildingCtrl.Instance = nil
    end
end

function GuildBuildingCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function GuildBuildingCtrl:AllRemind()
    local num = self.data:AllRemind()
    return num
end

function GuildBuildingCtrl:SendGuildBuildingReq(build_type, req_type)
    local protocol = ProtocolPool.Instance:GetProtocol(CSGuildConstructReq)
	protocol.build_type = build_type or GuildBuildingType.Hall
    protocol.req_type = req_type or GuildBuildingReqType.Up
	SendProtocol(protocol)
end

function GuildBuildingCtrl:OnSCGuildConstructInfo(protocol)
    -- LogError("OnSCGuildConstructInfo",protocol)
    self.data:SetOneBuildingInfo(protocol)
end

function GuildBuildingCtrl:OnSCGuildBuildingFinishInfo(protocol)
    -- LogError("OnSCGuildBuildingFinishInfo",protocol)
    self.data:SetNowBuildFinish(protocol)
end

function GuildBuildingCtrl:OnSCGuildUseBuildMaterialInfo(protocol)
    -- LogError("OnSCGuildUseBuildMaterialInfo",protocol)
    ChatData.Instance:SetGuildUseMaterial(protocol)
end

function GuildBuildingCtrl:OnSCGuildConstructAllInfo(protocol)
    -- LogError("OnSCGuildConstructAllInfo",protocol)
    self.data:SetAllBuildingInfo(protocol)
end

function GuildBuildingCtrl:OnSCGuildSpeedUpInfo(protocol)
    -- LogError("OnSCGuildSpeedUpInfo",protocol)
    self.data:SetSpeedInfo(protocol)
end

function GuildBuildingCtrl:OnSCGuildConstructTimeInfo(protocol)
    -- LogError("OnSCGuildConstructTimeInfo",protocol)
    self.data:NowBuildingData(protocol.now_building_type, protocol.finish_time, protocol.building_count)
end

function GuildBuildingCtrl:OnSSCGuildConstructOpenSUPInfo(protocol)
    -- LogError("OnSSCGuildConstructOpenSUPInfo",protocol)
    self.data:SetOpenSpeedInfo(protocol)
end