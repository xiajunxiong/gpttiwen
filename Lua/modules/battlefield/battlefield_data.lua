BattlefieldData = BattlefieldData or BaseClass()

function BattlefieldData:__init()
	if BattlefieldData.Instance ~= nil then
		Debuger.LogError("[BattlefieldData] attempt to create singleton twice!")
		return
	end
	BattlefieldData.Instance = self

	self.config = Config.illusory_battlefield_auto
	self:InitSmartData()
	self:InitRewardData()
	self:InitShopSmartData()
end

function BattlefieldData:InitSmartData()
	self.result_data = SmartData.New({
		season_num = 1,
		rank_score = 0,
		win_streak_times = 0,
		last_info = {},
		ranks_info = {},
		day_money_num = 0,
		today_win_reward_flag = {}
	})
	self.match_data = SmartData.New({is_match = 0})
	self.rank_data = SmartData.New{ranks_list = {{},{},{}}}
	self.guide_data = SmartData.New({val = false})
	self.report_data = SmartData.New({list = {},rate_list = {}})
	
	self.ranking_num = 0
	self.stamp_time = 0
	self.rank_num = 1
	self.is_top_race = 0
	self.search_list = {}
	self.role_info_data = SmartData.New({val = false})
end

function BattlefieldData:InitShopSmartData()
	self.shop_select_data = SmartData.New({list = {}})
end

function BattlefieldData:InitRewardData()
	self.reward_config = {}
	for k,v in pairs(self.config.ranking_cfg) do
		if self.reward_config[v.ranking] == nil then
			local reward_list = DataHelper.FormatItemList(v.ranking_reward)
			table.insert(reward_list,Item.New({item_id = CommonItemId.BattlefieldPoint,num = v.ranking_reward_money}))
			self.reward_config[v.ranking] = {ranking = v.ranking,ranking_list = v.ranking_list,reward_list = reward_list}
		end
	end
	self.ranking_config = KeyList(self.config.ranking_cfg,"ranking")
	self.score_config = KeyList(self.config.ranking_cfg,"score")
end

function BattlefieldData:__delete()
	BattlefieldData.Instance = nil
end

----------------------服务端数据下发----------------------
function BattlefieldData:SetBattlefieldMatch(protocol)
	self.match_data.is_match = protocol.is_match
end

function BattlefieldData:SetBattlefieldResult(protocol)
	self.result_data.result = protocol.result
	self.result_data.rank_score = protocol.rank_score
	self.result_data.win_streak_times = protocol.win_streak_times
	self.rank_num = (self:GetCurRankConfig(protocol.rank_score).ranking or 0) + 1
	self.ranking_num = protocol.ranking_num
	self.is_top_race = protocol.is_top_race
end

function BattlefieldData:SetBattlefieldRoleInfo(protocol)
	self.result_data.season_num = protocol.season_num
	self.result_data.rank_score = protocol.rank_score
	self.result_data.win_streak_times = protocol.win_streak_times
	self.result_data.ranks_info = protocol.ranks_info
	self.result_data.last_info = protocol.last_info
	self.rank_num = (self:GetCurRankConfig(protocol.rank_score).ranking or 0) + 1
	self.stamp_time = protocol.stamp_time or 0
	self.ranking_num = protocol.ranking_num
	self.is_top_race = protocol.is_top_race
	self.result_data.day_money_num = protocol.day_money_num
end

function BattlefieldData:SetOtherInfo(protocol)
	self.result_data.today_win_times = protocol.today_win_times
	self.result_data.today_win_reward_flag = protocol.today_win_reward_flag
end

--排行榜
function BattlefieldData:SetBattlefieldRankInfo(protocol)
	self.rank_data.ranks_list[protocol.rank_type] = TableCopy(protocol.rank_list)
	if protocol.rank_type == BattlefieldRankType.Main then
		self:CheckFriendList(TableCopy(protocol.rank_list))
	end
end

--战报
function BattlefieldData:SetBattlefieldReportInfo(protocol)
	if protocol.type == 1 then
		protocol.report_info.index = #self.report_data.list + 1
		table.insert(self.report_data.list,protocol.report_info)
	else
		self.report_data.list = protocol.report_list
	end
end

function BattlefieldData:SetBattlefieldReportRateInfo(protocol)
	self.report_data.rate_list = protocol.report_list
end

--玩家积分
function BattlefieldData:GetRankScore()
	return self.result_data.rank_score or 0
end

function BattlefieldData:GetWinStreakTimes()
	return self.result_data.win_streak_times
end

function BattlefieldData:GetSeasonNum()
	return self.result_data.season_num
end

function BattlefieldData:GetLastInfo()
	return self.result_data.last_info
end

function BattlefieldData:GetRanksInfo()
	return self.result_data.ranks_info
end

function BattlefieldData:GetRankNum()
	return self.rank_num or 1
end

function BattlefieldData:GetMatch()
	return self.match_data.is_match
end

function BattlefieldData:GetRankingList(type)
	return self.rank_data.ranks_list[type] or {}
end

function BattlefieldData:GetRankConfig(rank)
	return self.config.ranking_cfg[rank]
end

function BattlefieldData:GetReportList()
	local list = self.report_data.list
	table.sort(list,function(a,b)
		return a.time > b.time
	end)
	return list
end

function BattlefieldData.GetPrefsKey()
	return BattlefieldData.is_finish_guide or 0
end

function BattlefieldData.SetPrefsKey(value)
	BattlefieldData.is_finish_guide = value
end

function BattlefieldData:GetRewardConfig(ranking)
	return self.reward_config[ranking]
end

function BattlefieldData:GetRewardRankList()
	local list = {}
	for k,v in pairs(self.reward_config) do
		table.insert(list,v)
	end
	table.sort(list,DataHelper.SortFunc([[ranking]],true))
	return list
end

function BattlefieldData:SetShopSelectData(data)
	self.shop_select_data.list = data
end

function BattlefieldData:GetShopSelectData()
	return self.shop_select_data.list
end

function BattlefieldData:SetGuideData(val)
	self.guide_data.val = val
end

function BattlefieldData:GetGuideData()
	return self.guide_data.val
end

function BattlefieldData:GetStampTime()
	return self.stamp_time
end

function BattlefieldData:GetOtherConfig()
	return Config.illusory_battlefield_auto.activity_cfg[1] or {}
end

--当前段位
function BattlefieldData:GetRankingNum()
	return self.ranking_num or 0
end

function BattlefieldData:GetRankingConfig(ranking_num)
	return self.ranking_config[ranking_num or self:GetRankingNum()]
end

function BattlefieldData:GetTopRace()
	return self.is_top_race or 0
end

--获取排行奖励
function BattlefieldData:GetRankingRewardList(ranking)
	ranking = ranking or self:GetCrossRanking()
	for k,v in pairs(self.config.ranking_reward) do
		if ranking >= v.min_rank_pos and ranking <= v.max_rank_pos then
			return v
		end
	end
end

--获取段位配置
function BattlefieldData:GetCurRankConfig(score)
	score = score or self:GetRankScore()
	local rank_list = self.config.ranking_cfg
	for i=1,#rank_list do
		if score >= rank_list[i].score and rank_list[i+1] and score < rank_list[i+1].score then
			return rank_list[i]
		end
	end
	if score <= 0 then
		return rank_list[1]
	end
	return rank_list[#rank_list]
end

--获取自己的信息
function BattlefieldData:GetRoleData()
	return {
		name = RoleData.Instance:GetRoleName(),
		avatar_id = RoleData.Instance:GetRoleAvatarId(),
		avatar_type = RoleData.Instance:GetRoleAvatarType(),
		avatar_quality = RoleData.Instance:GetRoleAvatarQuality(),
		rank_name = self:GetCurRankConfig().ranking_list,
		is_top_race = self:GetTopRace()
	}
end

function BattlefieldData:GetCrossRanking()
	local role_id = RoleData.Instance:GetRoleId()
	for k,v in pairs(self:GetRankingList(BattlefieldRankType.Cross) or {}) do
		if v.role_id == role_id then
			return k
		end
	end
	return 0
end

function BattlefieldData:CheckFriendList(rank_list)
	local friend_list = {}
	self.rank_data.ranks_list[BattlefieldRankType.Friends] = nil
	for k,v in pairs(rank_list or {}) do
		if SocietyData.Instance:IsFriend(v.role_id) or v.role_id == RoleData.Instance:GetRoleId()then
			table.insert(friend_list,v)
		end
	end
	if #friend_list > 1 then
		table.sort(friend_list,function(a,b)return a.rank < b.rank end)
	end
	for i,v in ipairs(friend_list) do
		v.rank = i
	end
	self.rank_data.ranks_list[BattlefieldRankType.Friends] = friend_list
end

function BattlefieldData:GetConditionTip()
	local ranking_name = self:GetTopRace() == 0 and Language.Battlefield.NotRankTip or self:GetRankingConfig().ranking_list
	return Format(Language.Battlefield.ConditionTip[2],ranking_name)
end

function BattlefieldData:GetConditionTimeTip(t)
	if t.day == 0 and t.hour ~= 0 then
		return t.hour .. Language.Timer.Name.hour
	elseif t.day == 0 and t.hour == 0 then
		return t.min .. Language.Timer.Name.min
	elseif t.day == 0 and t.hour == 0 and t.min == 0 then
		return t.s .. Language.Timer.Name.sec
	end
	return t.day .. Language.Timer.Name.day
end

function BattlefieldData:GetConditionTopTimeTip(t)
	if t.day == 0 then
		return t.hour .. Language.Timer.Name.hour .. t.min .. Language.Timer.Name.min
	end
	return t.day .. Language.Timer.Name.day .. t.hour .. Language.Timer.Name.hour .. t.min .. Language.Timer.Name.min
end

function BattlefieldData:GetConditionTopTimeTips(t)
	if t.hour == 0 then
		return Format(Language.UiTimeMeter.Specia2, t.min ,t.s)
	end
	return Format(Language.UiTimeMeter.Special4, t.hour, t.min)
end

function BattlefieldData.QuaName(config,shallow)
	if config == nil then return ColorStr(Language.Battlefield.NotRankTip,COLORSTR.Gray1) end
	local color = shallow and BattlefieldConfig.BattlefieldRankingColorShallow or BattlefieldConfig.BattlefieldRankingColor
	return ColorStr(config.ranking_list,color[config.rank])
end

function BattlefieldData.GetRankName(config,is_top_race)
	if is_top_race == 0 then
		return BattlefieldData.QuaName(config)
	end
	return ColorStr(Language.Battlefield.NotRankTip,COLORSTR.Gray2)
end

--幻界商店
function BattlefieldData:GetShopListData()
	local list = StoreData.Instance:GetPageDataList(BATTLEFIELD_SHOP_TYPE)
    for k,v in pairs(list) do
        v.has_times = v.limit_type > 0 and v.limit_convert_count - StoreData.Instance:GetLimitTimes(v.conver_type,v.seq) or -1
        v.weight = v.has_times == 0 and v.seq + 1000 or v.seq
    end
    table.sort(list,function(a,b) return a.weight < b.weight end)
    return list
end

--幻界古币奖励
function BattlefieldData:GetFinishNum()
	local result = self.result_data.result or 0
	local config = self:GetOtherConfig()
	if self.result_data.day_money_num >= config.day_money_limit then
		return 0
	end
	if result == 0 then
		return config.victory_money
	elseif result == 1 then
		return config.defeat_money
	else
		return config.play_even_money
	end
end

--获取活动剩余时间戳
function BattlefieldData:GetTopConditionTip()
	if ActivityData.Instance:IsNoEnoughLevel(ActType.Battlefield,true) then
		return
	end
	if not ActivityData.IsOpen(ActType.Battlefield) then
		local config = ActivityData.GetConfig(ActType.BattlefieldAgain)
		return Language.Battlefield.MatchOpenTip .. config.time_text
	else
		local activity_info = ActivityData.Instance:GetActivityStatusInfo(ActType.Battlefield)
		if activity_info ~= nil then
			local time = TimeHelper.FormatDHMS(activity_info.next_status_switch_time - TimeManager:GetServerTime())
			return Format(Language.Battlefield.ConditionTip[1],self:GetConditionTopTimeTip(time))
		end
	end
end

function BattlefieldData:GetDayMoneyNum()
	return self.result_data.day_money_num
end

function BattlefieldData:GetConditionRemindTip()
    return Format(Language.Battlefield.TimeTips[4],DataHelper.GetDaXie(self:GetSeasonNum() or 1))
end

function BattlefieldData:GetTeamLabel(info)
	if info.is_leader == true then
		return 1
	elseif info.is_self == true then
		return 2	
	end
	return SocietyData.Instance:IsFriend(info.uid) and 3 or 0
end

function BattlefieldData:GetReportScoreDesc(data)
	if data.is_top_race == 1 then
		return ""
	end
	if data.is_protect == 1 then
		return ColorStr(Format(Language.Battlefield.AddScoreTip,Language.Battlefield.ProtectTip),COLORSTR.Red5)
	end
	local value_show = data.add_score > 0 and "+" .. data.add_score or data.add_score
	local value_color = data.add_score < 0 and COLORSTR.Red5 or COLORSTR.Green9
	return ColorStr(Format(Language.Battlefield.AddScoreTip,"  " .. value_show),value_color) .. " "
end

--获取当前队伍信息
function BattlefieldData:GetMemberList()
	if TeamCtrl.Instance:Data():InTeam() then
		return TeamCtrl.Instance:Data():GetMemberRoleList()
	else
		local role_info = RoleData.Instance:GetRoleData()
		role_info.rank_config = self:GetTopRace() == 1 and self:GetCurRankConfig() or nil
		return {[1]= {info = role_info,is_self = true,is_leader = false}}
	end
end

function BattlefieldData:GetShopRequireTip(value)
	local config = self.score_config[value]
	return config and config.ranking_list or ""
end

function BattlefieldData:IsShopRequireTip(value)
	local config = self.score_config[value]
	return config and self.ranking_num < config.ranking or false
end

function BattlefieldData:SetBattlefieldRoleItemInfo(protocol)
	for k,v in pairs(protocol.info_list) do
		self.search_list[v.role_id] = v
	end
	self.role_info_data.val = not self.role_info_data.val
end

function BattlefieldData:GetSearchData(role_id)
	return self.search_list[role_id]
end

function BattlefieldData:SetTeamMemberNotice(is_notice)
	self.team_member_notice = is_notice
end

function BattlefieldData:GetTeamMemberNotice()
	return self.team_member_notice or 0
end

function BattlefieldData:GetRewardData()
    local config,list = {},{0}
    for k,v in pairs(self.config.victory_box) do
		table.insert(config,v)
    end
	for i=1,#config do
        list[i + 1] = config[i].victory_field
    end
    table.sort(config,DataHelper.SortFunc([[seq]],true))
    return config,list
end

function BattlefieldData:GetRewardFlag(seq)
	return self.result_data.today_win_reward_flag[seq] or 0
end

function BattlefieldData:GetTodayWinStreakTimes()
	return self.result_data.today_win_times or 0
end

function BattlefieldData:GetRemindNum()
	for k,v in pairs(self.config.victory_box) do
		if self:GetRewardFlag(v.seq) == 0 and self:GetTodayWinStreakTimes() >= v.victory_field then
			return 1
		end
	end
	return 0
end