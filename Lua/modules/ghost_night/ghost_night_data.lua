GhostNightData = GhostNightData or BaseClass()

function GhostNightData:__init()
	if GhostNightData.Instance ~= nil then
		Debuger.LogError("[GhostNightData] attempt to create singleton twice!")
		return
	end
    GhostNightData.Instance = self

	self.poker_info = SmartData.New()
	self.info_data = SmartData.New({num = 0,npc_info = {},val = false})
end

function GhostNightData:SetGhostNightInfo(protocol)
	self.info_data.num = protocol.surplus_num
	self.info_data.npc_info = protocol.npc_info
end

function GhostNightData:GetGhostNightNum()
	return self.info_data.num
end

function GhostNightData:IsGhostNightOpen()
	if ActivityData.Instance:IsNoEnoughLevel(ActType.GhostNight,true) then
		return false
	end
	return PrefsInt([[GhostNight]] .. RoleId()) == 0 and ActivityData.Instance:GetActivityStatus(ActType.GhostNight) == ActStatusType.Open
end

--翻牌信息======================================
function GhostNightData:SetPokerInfo(info)
	self.poker_info:Set(info or {})
end

function GhostNightData:GetPokerInfo()
	return self.poker_info
end

function GhostNightData:GetPokerList()
	return self.poker_info.mypoker or {}
end
--=============================================
function GhostNightData:SetWorldBossRewardData(data)
	self.world_boss_reward_data = data
end

function GhostNightData:GetWorldBossRewardData()
	return self.world_boss_reward_data
end