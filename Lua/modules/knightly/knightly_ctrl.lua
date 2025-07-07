
FILE_REQ("modules/knightly/knightly_config")
FILE_REQ("modules/knightly/knightly_data")

VIEW_REQ("modules/knightly/knightly_view")
VIEW_REQ("modules/knightly/knightly_reward_view")
VIEW_REQ("modules/knightly/knightly_rank_view")

KnightlyCtrl = KnightlyCtrl or BaseClass(BaseCtrl)

function KnightlyCtrl:__init()
    if KnightlyCtrl.Instance ~= nil then
        Debuger.LogError("[KnightlyCtrl] attempt to create singleton twice!")
        return
    end
    KnightlyCtrl.Instance = self
    self.data = KnightlyData.New()

	self:RegisterProtocol(CSChivalrousTeamListReq)
	self:RegisterProtocol(CSRoleHistoryChivalrousRewardFetch)
	self:RegisterProtocol(CSChivalrousRankListReq)

	self:RegisterProtocol(SCChivalrousTeamList, "OnChivalrousTeamList")
	self:RegisterProtocol(SCRoleHistoryChivalrous, "OnRoleHistoryChivalrous")
	self:RegisterProtocol(SCChivalrousRankList, "OnChivalrousRankList")

    RemindCtrl.Instance:Remind():Register(Mod.Knight.Main, self.data.knightly_reward_red_point, function ()
		return self.data.knightly_reward_red_point.val and 1 or 0
	end)

end

function KnightlyCtrl:__delete()
    if KnightlyCtrl.Instance == self then
        KnightlyCtrl.Instance = nil
    end
end

function KnightlyCtrl:OnChivalrousTeamList(protocol)
    self.data:SetChivalrousTeamList(protocol)
end

function KnightlyCtrl:OnRoleHistoryChivalrous(protocol)
	self.data:SetRoleHistoryChivalrous(protocol)
end

function KnightlyCtrl:OnChivalrousRankList(protocol)
	self.data:SetChivalrousRankList(protocol)
end

function KnightlyCtrl:SendChivalrousTeamListReq()
    local protocol = GetProtocol(CSChivalrousTeamListReq)
	SendProtocol(protocol)
end

function KnightlyCtrl:SendRoleHistoryChivalrousRewardFetch(fetch_seq)
    local protocol = GetProtocol(CSRoleHistoryChivalrousRewardFetch)
    protocol.fetch_seq = fetch_seq
	SendProtocol(protocol)
end

function KnightlyCtrl:SendChivalrousRankListReq(rank_type)
	local protocol = GetProtocol(CSChivalrousRankListReq)
	protocol.rank_type = self.data.rank_type or 0
	SendProtocol(protocol)
end