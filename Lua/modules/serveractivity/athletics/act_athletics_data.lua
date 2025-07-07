--新服竞技数据
AthleticsData=AthleticsData or BaseClass()
AthleticsData.REWARD_STATE = {
	GETED = 0,  -- 已经领取
	NOT = 1,     -- 不可领取
	CAN = 2,     -- 可以领取
}

function AthleticsData:__init()
	if AthleticsData.Instance ~= nil then
		Debuger.LogError("[AthleticsData] attempt to create singleton twice!")
		return
	end
	AthleticsData.Instance=self
	self.historyData=SmartData.New({rank=-1,is_get_table={},})
	self.settlement_cfg = ServerActivityData.Instance:GetActivityOpenCfg("sports_settlement")  --结算奖励配置
	self.history_cfg = ServerActivityData.Instance:GetActivityOpenCfg("sports_history")
	Cfg.Register("GetActNewServerSettlementBySeq",Cfg.RealTimeSearch(self.settlement_cfg,"seq"))
end

function AthleticsData:__delete()
	AthleticsData.Instance=nil
	self.historyData:Set({})
	self.historyData=nil
end

function AthleticsData:SetHistoryData(protocol)
	self.historyData.rank=protocol.ra_sports_histroy_top
	self.historyData.is_get_table=protocol.ra_sports_histroy_reward
end

--获取奖励预览
function AthleticsData:GetRewardShowList()
	return ServerActivityData.Instance:GetActivityOpenCfg("other")[1].sports_reward_item
end
--获取结算奖励配置
function AthleticsData:GetSettlementCfg()
	return self.settlement_cfg
end
--根据seq获取结算奖励数据
function AthleticsData:GetSettlementBySeq(seq)
	return Cfg.GetActNewServerSettlementBySeq(seq)
end
--获取历史奖励数据列表
function AthleticsData:GetHistoryRewardList()
	local isget_t = self.historyData.is_get_table
	local cfg = self.history_cfg
	local cfg_len = #cfg
	local list = {}
	for i=1 , cfg_len do
		local v = cfg[i]
		local rank = self:GetHistoryRank()
		if rank > 0 then
			local ind = #isget_t - v.seq
			if isget_t[ind] == 1 then
				v.get_state = AthleticsData.REWARD_STATE.GETED
			else
				if rank <= v.historical_ranking then
					v.get_state = AthleticsData.REWARD_STATE.CAN
				else
					v.get_state = AthleticsData.REWARD_STATE.NOT
				end
			end
		else
			v.get_state = AthleticsData.REWARD_STATE.NOT
		end
		list[i] = v
	end
	table.sort(list,function (a,b)
		if a.get_state == b.get_state then
			return a.seq > b.seq
		else
			return a.get_state > b.get_state
		end
	end)
	return list
end

--是否能领取新服竞技的历史排名奖励
function AthleticsData:GetIsCanHistoryReward()
	if LoginData.Instance:IsOnCrossSever() then
        return 0
    end
	for _,v in pairs(self:GetHistoryRewardList()) do
		if v.get_state then
			if v.get_state == AthleticsData.REWARD_STATE.CAN then
				return 1
			end
		end
	end
	return 0
end

--获取排名
function AthleticsData:GetRank()
	return ArenaData.Instance.RoleInfo.rank
end

--获取历史最高排名
function AthleticsData:GetHistoryRank()
	return self.historyData.rank <= 0 and ArenaData.Instance.RoleInfo.rank or self.historyData.rank
end