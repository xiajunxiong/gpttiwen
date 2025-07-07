ChiefElectionData = ChiefElectionData or BaseClass()

function ChiefElectionData:__init()
	if ChiefElectionData.Instance ~= nil then
		Debuger.LogError("[ChiefElectionData] attempt to create singleton twice!")
		return
	end
	ChiefElectionData.Instance = self

	self.match_data = SmartData.New({
		is_join = 0,
		curr_round = 0,
		end_time = 0,
		round_state = 0,
		is_finish = 0
	})
	self.info_data = SmartData.New({
		curr_round = 0,
		end_time = 0
	})
	self.chage_data = SmartData.New({
		is_leave = false
	})

	self.primary_data = SmartData.New({
		role_num = 0,
		match_state = 0,
		next_match_state_time = 0
	})
	self.primary_role = SmartData.New({
		socre = 0,
		win_times = 0,
        lose_times = 0,
        total_times = 0
	})
	self.rank_data = SmartData.New({list = {}})
	self.pos_data = Config.chief_top_auto.prof_pos	
	self:InitFormatData()
end

function ChiefElectionData:__delete()
	ChiefElectionData.Instance = nil
end

function ChiefElectionData:InitFormatData()
	self.match_list = {}
	for i=1,4 do
		self.match_list[i] = {}
		for j=1,8 / math.pow(2,i - 1) do
			self.match_list[i][j] = {}
		end
	end
	self.pos_list = {}
	for index, value in ipairs(self.pos_data) do
		self.pos_list[value.prof] = value
	end
end

function ChiefElectionData:CancelData()
	self.match_data.is_join = 0
	self.chage_data.is_leave = false
	ChiefCtrl.IsChiefnFinish = false
	ChiefCtrl.EctionSubscribe = false
	ChiefCtrl.OpenChiefElection = nil
	local prof = RoleData.Instance:GetRoleProf()
	if prof ~= 0 then
		LoginCtrl.Instance:RoleSubscribeCancel(prof)
	end
end

--=======================首席竞选======================
function ChiefElectionData:SetChiefElectionRoleInfo(protocol)
	if protocol.is_join ~= 0 then
		self.match_data.is_join = protocol.is_join
	end
end

--战力排行榜
function ChiefElectionData:SetChiefElectionJoinRoleInfo(protocol)
end

function ChiefElectionData:SetChiefElectionMatchInfo(protocol)
	self.match_data.end_time = protocol.end_time
	self.match_data.curr_round = protocol.curr_round
	self.match_data.round_state = protocol.round_state
	self.match_data.match_info = protocol.match_info
	self.match_data.is_finish = protocol.is_finish
	if protocol.curr_round ~= 0 then
		self:InitFormatData()
		self:FlushMatchInfo()
	end
end

function ChiefElectionData:SetChiefElectionTopInfo(protocol)
	self.winner_list = protocol.winner_list
	self.winner_id = protocol.winner_id or 0
end

function ChiefElectionData:SetChiefElectionNotice(protocol)
	self.info_data.end_time = protocol.end_time
	self.info_data.curr_round = protocol.curr_round
end

function ChiefElectionData:GetWinnerId()
	return self.winner_id or 0
end

--获取是否报名
function ChiefElectionData:GetJoin()
	return self.match_data.is_join or 0
end

--匹配时间
function ChiefElectionData:GetMatchTime()
	return self.match_data.end_time or 0
end

function ChiefElectionData:GetCurrRound()
	return self.match_data.curr_round or 0
end

function ChiefElectionData:GetRoundState()
	return self.match_data.round_state or 0
end

function ChiefElectionData:GetMatchList(round,index)
	if self.match_list[round] == nil then return end
	return index == nil and self.match_list[round] or self.match_list[round][index]
end

--获取我的排名
function ChiefElectionData:GetRoloRink(list)
	for k,v in pairs(list) do
		if v.role_id == RoleData.Instance:GetRoleId() then
			return Format(Language.ChiefElection.RoleRink[2],v.rink)
		end
	end
	return Language.ChiefElection.RoleRink[1]
end

--处理分析竞选数据
function ChiefElectionData:FlushMatchInfo()
	local match_info = self.match_data.match_info
	--添加颜色 红 蓝 两组 16
	for i,v in ipairs(match_info or {}) do
		v.color = ChiefConfig.MatchColor[i <= 8 and 1 or 2][i % 2 ~= 0 and 1 or 2]
		--处理16进8 16人分别进入8组
		table.insert(self.match_list[1][math.ceil(i / 2)],v)
	end
	--处理8 ~ 1强数据
	for round=2,4 do
		for i,v in ipairs(self.match_list[round - 1] or {}) do
			local info = v[1].match_round > v[2].match_round and v[1] or v[2]
			if info.state == 1 and v[1].match_round == v[2].match_round then
				info = v[1].state < v[2].state and v[1] or v[2]
			end
			table.insert(self.match_list[round][math.ceil(i / 2)],info)
		end
	end
end

function ChiefElectionData:GetBattleRewardData(list,reward_type,is_win)
	local level = RoleData.Instance:GetRoleLevel()
	for k,v in pairs(Config.chief_election_auto.battle_reward) do
		if v.reward_type == reward_type and level >= v.min_level and level <= v.max_level then
			self:GetMergeData(list,{item_id = CommonItemId.Exp,num = is_win and v.winner_exp or v.loser_exp})
			self:GetMergeData(list,{item_id = CommonItemId.Coin,num = is_win and v.winner_coin or v.loser_coin})
			if not is_win then
				for _,item in pairs(v.loser_reward) do
					self:GetMergeData(list,item)
				end
			end
			if reward_type == 4 and is_win then
				for _,item in pairs(self:GetWinRewardList()) do
					self:GetMergeData(list,item)
				end
			end
		end
	end
end

--合并奖励
function ChiefElectionData:GetMergeData(list,item)
	local is_merge = false
	for k,v in pairs(list) do
		if v.item_id == item.item_id then
			v.num = v.num + item.num
			is_merge = true
			break
		end
	end
	if not is_merge then
		table.insert(list,item)
	end
end

--最终奖励
function ChiefElectionData:GetWinRewardList()
	local prof_type = RoleData.Instance:GetRoleProfType()
	for k,v in pairs(Config.chief_election_auto.title) do
		if v.type == prof_type then
			return v.reward_show
		end
	end
end
--提供给巅峰战
function ChiefElectionData:GetMatchProf(role_id)
	for k,v in pairs(self.winner_list or {}) do
		if v.role_id == role_id then
			return v.prof
		end
	end
	return 1
end

--是否是首席竞选首席玩家
function ChiefElectionData:IsChief()
	local role_id = RoleData.Instance:GetRoleId()
	for k,v in pairs(self.winner_list or {}) do
		if v.role_id == role_id then
			return true
		end
	end
	return false
end

function ChiefElectionData:GetBattleWarningText()
	local curr_round = self.match_data.curr_round
	if not self:IsAbandon() then
		curr_round = self.info_data.curr_round
	end
	return Language.ChiefElection.BattleTip[curr_round] .. Language.ChiefElection.BattleTip[0]
end

--失败描述
function ChiefElectionData:GetFailTip()
	if FinishData.Instance:IsDraw() then
		return Language.ChiefElection.DrawTip
	else
		local curr_round = self.info_data.curr_round
		return Format(Language.ChiefElection.FailTip[0],Language.ChiefElection.FailTip[curr_round])
	end
end

--特殊处理 获取Cell的index
function ChiefElectionData:GetMatchIndex(role_id)
	for _,v in ipairs(self:GetMatchList(3) or {}) do
		for i,info in ipairs(v) do
			if info.role_id == role_id then
				return i
			end
		end
	end
end

--获取当前的对手信息
function ChiefElectionData:GetOpponent()
	local role_id = RoleData.Instance:GetRoleId()
	for i,v in ipairs(self:GetMatchList(self:GetCurrRound()) or {}) do
		for index,info in ipairs(v or {}) do
			if info.role_id == role_id then
				return v[index == 1 and 2 or 1]
			end
		end
	end
end

--竞选是否结束
function ChiefElectionData:IsFinish()
	return self.match_data.is_finish == 1 or false
end

--是否放弃和输了的
function ChiefElectionData:IsAbandon()
	local scene_id = SceneData.Instance:SceneId()
	for k,v in pairs(Config.chief_election_auto.waiting_room) do
		if v.scene_id == scene_id then
			return false
		end
	end
	return true
end

function ChiefElectionData:SetWinerInfo(role_id)
	if self.winner_info == nil then
		self.winner_info = {}
	end
	local round = self:GetCurrRound()
	if self.winner_info[round] == nil then
		self.winner_info[round] = {}
	end
	table.insert(self.winner_info[round],role_id)
end

function ChiefElectionData:IsWinerFinsh(round,rold_id)
	if self.winner_info and self.winner_info[round]then
		for k,v in pairs(self.winner_info[round]) do
			if v == rold_id then
				return true
			end
		end
	end
	return false
end

--设置离开场景
function ChiefElectionData:SetLeaveFlag(val)
	self.chage_data.is_leave = val
end

function ChiefElectionData:IsLeaveFlag()
	return self.chage_data.is_leave or false
end

--混战角色信息
function ChiefElectionData:SetChiefElectionSeaRoleInfo(protocol)
	self.primary_role.socre = protocol.socre
	self.primary_role.win_times = protocol.win_times
	self.primary_role.lose_times = protocol.lose_times
	self.primary_role.total_times = protocol.total_times
end

--混战场景人数信息
function ChiefElectionData:SetChiefElectionSeaSceneInfo(protocol)
	self.primary_data.role_num = protocol.role_num
end

--混战活动状态
function ChiefElectionData:SetChiefElectionStateInfo(protocol)
	self.primary_data.match_state = protocol.match_state
	self.primary_data.next_match_state_time = protocol.next_match_state_time
end

--混战排行榜下发
function ChiefElectionData:SetChiefElectionSeaRank(protocol)
	self.rank_data.list = protocol.rank_list
end

function ChiefElectionData:GetPrimaryRoleInfo()
	return self.primary_role
end

function ChiefElectionData:GetPlayNum()
	return self.primary_data.role_num
end

function ChiefElectionData:GetMatchState()
	return self.primary_data.match_state
end

function ChiefElectionData:GetNextMatchStateTime()
	return self.primary_data.next_match_state_time
end

function ChiefElectionData:GetRankList()
	return self.rank_data.list
end

--排行榜奖励
function ChiefElectionData:GetParticipationRewardData()
	local role_level = RoleData.Instance:GetRoleLevel()
	for k,v in pairs(Config.chief_election_auto.participation_reward) do
		if role_level >= v.level_min and role_level <= v.level_max then
			local reward_list = {}
			if v.exp > 0 then
				table.insert(reward_list,{item_id = CommonItemId.Exp,num = v.exp})
			end
			if v.coin > 0 then
				table.insert(reward_list,{item_id = CommonItemId.Coin,num = v.coin})
			end
			for k,v in pairs(v.join_reward) do
				table.insert(reward_list,v)
			end
			return DataHelper.FormatItemList(reward_list)
		end
	end
end

--玩家排名
function ChiefElectionData:GetRoleRank()
    local role_id = RoleData.Instance:GetRoleId()
    for k,v in pairs(self:GetRankList()) do
        if v.role_id == role_id then
            return k,v
        end
    end
end

function ChiefElectionData:GetPrimaryTip()
	if FinishData.Instance:IsDraw() then
		return Language.ChiefElection.PrimaryTip
	else
		local role_info = self:GetPrimaryRoleInfo() or {}
		return Format(Language.ChiefElection.PrimaryFailTip,DataHelper.GetDaXie(role_info.lose_times or 0))
	end
end

function ChiefElectionData:SetChiefElectionBattleResult(protocol)
	self.buff_info = protocol.buff_info
end

function ChiefElectionData:GetBuffResult()
	local info = self.buff_info
	if info ~= nil then
		local buff_cfg = Config.buff_list_auto.buff_list[info.buff_id]
		local buff_desc = Format(Language.ChiefElection.BattleBuffTip,info.buff_value)
		buff_cfg.desc_1 = buff_cfg.desc .. buff_desc
		return {buff_id = info.buff_id,buff_cfg = buff_cfg,from_view = "battle"}
	end
end

function ChiefElectionData:GetTotalPreviewReward(is_preview)
	local param = {
		activity_type = ActType.ChiefElection,
		op_type = ActivityConfig.TIME_LIMIT_REWARD_GET_OP.JOIN_AWARD,
		activity_tip = Language.ChiefElection.NotCanRewardTip,
	}
	local reward_list = {
        [1] = {
            toggle_name = Language.OnlyFight.BaseReward,
			reward_item_list = {
				{
					title_name = Language.OnlyFight.JoinReward,
					item_list = self:GetParticipationRewardData(),
					func_param = is_preview == nil and param or nil,
					btn_flag = self:GetMatchState() == 0 and 2 or nil,
				}
			}
        },
        [2] = {
            toggle_name = Language.OnlyFight.RankReward,
            reward_item_list = {}
		},
		[3] = {
            toggle_name = Language.Activity.TipsNotice,
            reward_item_list = {{tips_act_type = ActType.ChiefElection}}
        },
	}
	--排名奖励
	local language = Language.ChiefElection.RewardTitle
	for i = #language, 1, -1 do
		local list = {}
		for j = i - 1, 1, -1 do
			self:GetBattleRewardData(list,j,true)
		end
		if i ~= #language then
			self:GetBattleRewardData(list,i,false)
		end
		table.insert(reward_list[2].reward_item_list,{title_name = language[i],item_list = DataHelper.FormatItemList(list)})
	end
	return reward_list
end

--==========================首席巅峰战==========================
ChiefPeakBattleData = ChiefPeakBattleData or BaseClass()

function ChiefPeakBattleData:__init()
	if ChiefPeakBattleData.Instance ~= nil then
		Debuger.LogError("[ChiefPeakBattleData] attempt to create singleton twice!")
		return
	end
	ChiefPeakBattleData.Instance = self
	
	self.match_data = SmartData.New({
        curr_round = 0,
        round_state = 0,
		match_info = {},
		end_time = 0,
		is_finish = 0,
	})
	self.select_data = SmartData.New()
	
	self.info_data = SmartData.New({
		curr_round = 0,
		end_time = 0
	})
	self.chage_data = SmartData.New({
		is_leave = false
	})
	self:InitFormatData()
	
	self.rank_reward = Config.chief_top_auto.rank_reward
end

function ChiefPeakBattleData:__delete()
	ChiefPeakBattleData.Instance = nil
end

function ChiefPeakBattleData:InitFormatData()
	self.match_list = {}
	for i=1,3 do
		self.match_list[i] = {}
		for j=1,8 / math.pow(2,i) do
			self.match_list[i][j] = {}
		end
	end
end

function ChiefPeakBattleData:CancelData()
	self.chage_data.is_leave = false
	ChiefCtrl.IsChiefnFinish = false
	ChiefCtrl.PeakBattleSubscribe = false
	ChiefCtrl.OpenChiefPeakBattle = false
	LoginCtrl.Instance:RoleSubscribeCancel(RoleSubscribeTopicType.ChiefPeakBattle)
end

--设置选中数据
function ChiefPeakBattleData:SetSelectData(data)
	self.select_data:Set(data)
end

--获取选中数据
function ChiefPeakBattleData:GetSelectData()
	return self.select_data
end

function ChiefPeakBattleData:GetCurrRound()
	return self.match_data.curr_round or 0
end

function ChiefPeakBattleData:GetRoundState()
	return self.match_data.round_state or 0
end

function ChiefPeakBattleData:SetChiefPeakBattleNotice(protocol)
	self.info_data.end_time = protocol.end_time
	self.info_data.curr_round = protocol.curr_round
end

function ChiefPeakBattleData:SetChiefPeakBattleMatchInfo(protocol)
    self.match_data.curr_round = protocol.curr_round
	self.match_data.round_state = protocol.round_state
	self.match_data.end_time = protocol.end_time
	self.match_data.match_info = protocol.match_info
	self.match_data.is_finish = protocol.is_finish
	if protocol.curr_round ~= 0 then
		self:InitFormatData()
		self:FlushMatchInfo()
	end
end

function ChiefPeakBattleData:SetChiefPeakBattleRoleInfo(protocol)
	self.is_bet_type = protocol.bet_type or 0
end

function ChiefPeakBattleData:SetChiefPeakBattleInfo(protocol)
	self.winner_id = protocol.winner_id or {}
end

function ChiefPeakBattleData:GetCurrRound()
	return self.match_data.curr_round or 0
end

function ChiefPeakBattleData:GetRoundState()
	return self.match_data.round_state or 0
end

function ChiefPeakBattleData:GetMatchTime()
	return self.match_data.end_time or 0
end

function ChiefPeakBattleData:GetWinnerId()
	return self.winner_id or 0
end

function ChiefPeakBattleData:IsBetType()
	return self.is_bet_type or 0
end

--获取巅峰战参赛选手
function ChiefPeakBattleData:GetRinkList()
	local list = {}
	for i,v in ipairs(self.match_data.match_info or {}) do
		if v.role_id ~= 0 then
			table.insert(list,v)
		end
	end
	return list 
end

function ChiefPeakBattleData:GetMatchList(round,index)
	if self.match_list[round] == nil then return end
	return index == nil and self.match_list[round] or self.match_list[round][index]
end

--处理分析竞选数据
function ChiefPeakBattleData:FlushMatchInfo()
	local match_info = self.match_data.match_info
	--添加颜色 红 蓝 两组 8
	for i,v in ipairs(match_info or {}) do
		v.color = ChiefConfig.MatchColor[i <= 4 and 1 or 2][i % 2 ~= 0 and 1 or 2]
		--处理8进4 8人分别进入4组
		table.insert(self.match_list[1][math.ceil(i / 2)],v)
	end
	--处理4 ~ 1强数据
	for round=2,3 do
		for i,v in ipairs(self.match_list[round - 1] or {}) do
			local info = v[1].match_round > v[2].match_round and v[1] or v[2]
			if info.state == 1 and v[1].match_round == v[2].match_round then
				info = v[1].state < v[2].state and v[1] or v[2]
			end
			table.insert(self.match_list[round][math.ceil(i / 2)],info)
		end
	end
end

--是否是参赛人员
function ChiefPeakBattleData:IsCompetition()
	local role_id = RoleData.Instance:GetRoleId()
	for k,v in pairs(self.match_data.match_info) do
		if v.role_id == role_id then
			return true
		end
	end
	return false
end

function ChiefPeakBattleData:GetBattleWarningText()
	local curr_round = self.match_data.curr_round
	if not self:IsAbandon() then
		curr_round = self.info_data.curr_round
	end
	return Language.ChiefPeakBattle.BattleTip[curr_round] .. Language.ChiefPeakBattle.BattleTip[0]
end

--失败描述
function ChiefPeakBattleData:GetFailTip()
	if not self:IsRoleLost() then
		return
	end
	if FinishData.Instance:IsDraw() then
		return Language.ChiefPeakBattle.DrawTip
	else
		local curr_round = self.info_data.curr_round
		return Format(Language.ChiefPeakBattle.FailTip[0],Language.ChiefPeakBattle.FailTip[curr_round])
	end
end

--获取冠军奖励
function ChiefPeakBattleData:GetRewardData()
	if not self:IsRoleLost() then
		return
	end
	local list = {}
	local curr_round = self.info_data.curr_round
	local reward_data = self.rank_reward[curr_round].reward
	local coin = self.rank_reward[curr_round].winner_coin
	if ChiefCtrl.IsChiefnFinish then
		reward_data = self.rank_reward[4].reward
		coin = self.rank_reward[4].winner_coin
	end
	for k,v in pairs(reward_data) do
		table.insert(list,v)
	end
	table.insert(list,{item_id = CommonItemId.Coin,num = coin})
	return list
end

--获取当前的对手信息
function ChiefPeakBattleData:GetOpponent()
	local role_id = RoleData.Instance:GetRoleId()
	for i,v in ipairs(self:GetMatchList(self:GetCurrRound()) or {}) do
		for index,info in ipairs(v or {}) do
			if info.role_id == role_id then
				return v[index == 1 and 2 or 1]
			end
		end
	end
end

--特殊处理 获取Cell的index
function ChiefPeakBattleData:GetMatchIndex(role_id)
	for _,v in ipairs(self:GetMatchList(2) or {}) do
		for i,info in ipairs(v or {}) do
			if info.role_id == role_id then
				return i
			end
		end
	end
end

function ChiefPeakBattleData:IsFinish()
	return self.match_data.is_finish == 1 or false
end

--是否放弃和输了的
function ChiefPeakBattleData:IsAbandon()
	local scene_id = SceneData.Instance:SceneId()
	for k,v in pairs(Config.chief_top_auto.waiting_room) do
		if v.scene_id == scene_id then
			return false
		end
	end
	return true
end

function ChiefPeakBattleData:SetWinerInfo(role_id,is_win)
	if self.winner_info == nil then
		self.winner_info = {}
	end
	local round = self:GetCurrRound()
	if self.winner_info[round] == nil then
		self.winner_info[round] = {}
	end
	table.insert(self.winner_info[round],role_id)

	if RoleData.Instance:GetRoleId() == role_id then
		self.is_role_lost = is_win == 0
	end
end

function ChiefPeakBattleData:IsWinerFinsh(round,rold_id)
	if self.winner_info and self.winner_info[round]then
		for k,v in pairs(self.winner_info[round]) do
			if v == rold_id then
				return true
			end
		end
	end
	return false
end

--设置是否离开场景
function ChiefPeakBattleData:SetLeaveFlag(val)
	self.chage_data.is_leave = val
end

function ChiefPeakBattleData:IsLeaveFlag()
	return self.chage_data.is_leave or false
end

--自己是否是输了
function ChiefPeakBattleData:IsRoleLost()
	return self.is_role_lost or false
end

function ChiefPeakBattleData:GetFhishShowType()
	return self:IsRoleLost() and 4 or 1
end