FILE_REQ("modules/guild_hof/guild_hof_data")

VIEW_REQ("modules/guild_hof/guild_hof_view")

GuildHofCtrl = GuildHofCtrl or BaseClass(BaseCtrl)

function GuildHofCtrl:__init()
    if GuildHofCtrl.Instance ~= nil then
        Debuger.LogError("[GuildHofCtrl] attempt to create singleton twice!")
        return
    end
    GuildHofCtrl.Instance = self
    self.data = GuildHofData.New()
    self:OnRemind()
    self:RegisterAllProtocols()
end

function GuildHofCtrl:OnRemind()
    RemindCtrl.Instance:Remind():Register(
        Mod.Guild.GuildHof,
        self.data.remind_data,
        BindTool.Bind(self.AllRemind, self)
    )
end

function GuildHofCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSGuildGallOfFameReq)
    self:RegisterProtocol(SCGuildHallOfFameInfo, "OnSCGuildHallOfFameInfo")
    self:RegisterProtocol(SCGuildQiFuInfo, "OnSCGuildQiFuInfo")
end

function GuildHofCtrl:__delete()
    --注销网络事件
    self:UnRegisterProtocol(CSGuildGallOfFameReq)
    self:UnRegisterProtocol(SCGuildHallOfFameInfo)
    self:UnRegisterProtocol(SCGuildQiFuInfo)
    
    RemindCtrl.Instance:Remind():UnRegister(Mod.Guild.GuildHof)
    if GuildHofCtrl.Instance == self then
        GuildHofCtrl.Instance = nil
    end
end

function GuildHofCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function GuildHofCtrl:AllRemind()
    local num = self.data:AllRemind()
    return num
end

function GuildHofCtrl:SendGuildHofReq(req_type, rank_type)
    local protocol = ProtocolPool.Instance:GetProtocol(CSGuildGallOfFameReq)
	protocol.req_type = req_type or GuildHofReqType.Info
    protocol.rank_type = rank_type or GuildHofRankType.Battle
	SendProtocol(protocol)
end

function GuildHofCtrl:SendGuildHofRank(rank_type)
    if not rank_type then
        for k, v in pairs(GuildHofRankType) do
            self:SendGuildHofReq(GuildHofReqType.RankInfo, v)
        end
    else
        self:SendGuildHofReq(GuildHofReqType.RankInfo, rank_type)
    end
end

function GuildHofCtrl:OnSCGuildHallOfFameInfo(protocol)
    self.data:SetRankListInfo(protocol)
end

function GuildHofCtrl:OnSCGuildQiFuInfo(protocol)
    self.data:SetBlessingInfo(protocol)
end