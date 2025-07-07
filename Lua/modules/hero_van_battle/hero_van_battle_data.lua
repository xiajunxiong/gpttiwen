HeroVanBattleData = HeroVanBattleData or BaseClass()

function HeroVanBattleData:__init()
	if HeroVanBattleData.Instance ~= nil then
		Debuger.LogError("[HeroVanBattleData] attempt to create singleton twice!")
		return
	end
	HeroVanBattleData.Instance = self
    
	self.team_simple_info = SmartData.New({})
	self.team_pre_signup = SmartData.New({flush = false})
	-- self.team_pre_signup = SmartData.New({})
	self.team_sign_up_detail_info = SmartData.New({})
	self.team_view_data = SmartData.New({can_enroll = false,select_team = nil})
	self.apply_view_data = SmartData.New({is_leader = false})
	self.team_view_mine_team = SmartData.New({mine_team = false})
	self.act_info = SmartData.New({}) 
	self.battle_scene_info = SmartData.New({flush = false}) 
	self.neo_rank_data = SmartData.New({flush = false}) 
	self.finally_rank_data = SmartData.New({flush = false})
	self.search_team_data = SmartData.New({flush = false,key_word = nil})
	self.other_info = SmartData.New({flush = false})
	self.is_pre_click = false

	self.start_view_data = SmartData.New({})

	self.act_show = false
end 

function HeroVanBattleData:GetCurJoinReward()
	local role_level = RoleData.Instance:GetRoleLevel()
	for k,v in pairs(Config.hero_competition_auto.participation_reward) do 
		if role_level >= v.level_min and role_level <= v.level_max then 
			return v.join_reward
		end 
	end
end

-- 获取排名奖励·列表
function HeroVanBattleData:GetCurRankReward()
	local role_level = RoleData.Instance:GetRoleLevel()
	local show_list = {}
	for k,v in pairs(Config.hero_competition_auto.rank_reward) do 
		if role_level >= v.level_min and role_level <= v.level_max then 
			table.insert( show_list,v )
		end 
	end

	return show_list
end

-- 获取某个排名的奖励（取自己的等级）
function HeroVanBattleData:GetRankReward(rank) 
	local role_level = RoleData.Instance:GetRoleLevel()
	for k,v in pairs(Config.hero_competition_auto.rank_reward) do 
		if role_level >= v.level_min and role_level <= v.level_max and v.rank_min <= rank and v.rank_max >= rank  then 
			return v.reward_show
		end 
	end 

	return {}
end 

-- 协议信息接收
function HeroVanBattleData:SetAllSimpleInfo(protocol)
	self.team_simple_info.team_list = protocol.team_list
	self.team_simple_info.my_team_index = protocol.my_team_index
	self.team_simple_info.know_team_index = protocol.know_team_index

	if self.team_simple_info.my_team_index > -1 then 
		self:SetTeamArenaOtherInfo({is_sign_up = 1})
	end 
end

function HeroVanBattleData:SetPreTeamSignUpInfo(protocol)
	self.team_pre_signup.team_name = protocol.team_name
	self.team_pre_signup.member_list = protocol.member_list

	self.team_pre_signup.flush = not self.team_pre_signup.flush
end

function HeroVanBattleData:SetTeamSignUpDetailedInfo(protocol)
	self.team_sign_up_detail_info.team_unique_id = protocol.team_unique_id
	self.team_sign_up_detail_info.team_name = protocol.team_name
	self.team_sign_up_detail_info.member_list = protocol.member_list

	if protocol.notice_type == 2 or protocol.notice_type == 1 then 
		ViewMgr:OpenView(HeroVanBattlePreTeamView)
	end 
end

function HeroVanBattleData:GetSignUpDetailInfo()
	local result_list = {}
	for i = 1, #self.team_sign_up_detail_info.member_list do 
		local vo = self.team_sign_up_detail_info.member_list[i]
		vo.is_master = i == 1 
		table.insert( result_list, vo )
	end 

	return result_list
end

function HeroVanBattleData:SetTeamArenaNoticeInfo(protocol)
	if self.team_pre_signup.member_list ~= nil then 
		self.team_pre_signup.member_list[protocol.param1+1].determine_flag = protocol.param2

		self.team_pre_signup.flush = not self.team_pre_signup.flush
	end 
end

-- 这个是活动信息
function HeroVanBattleData:SetTeamArenaInfo(protocol)
	self.act_info.season_num = protocol.season_num
	self.act_info.start_sign_up_timestamp = protocol.start_sign_up_timestamp
	self.act_info.end_sign_up_timestamp = protocol.end_sign_up_timestamp
	self.act_info.activity_open_next_zero_timestamp = protocol.activity_open_next_zero_timestamp

	local time = TimeCtrl.Instance:GetServerTime() 
	if not self.act_show and time >= self.act_info.start_sign_up_timestamp and time <= self.act_info.end_sign_up_timestamp then 
		if RoleData.Instance:GetRoleLevel() >= Config.hero_competition_auto.other[1].level then
		    ViewMgr:OpenView(HeroVanBattlePreShow)
		end 
		self.act_show = true
	end 
end

function HeroVanBattleData:SetTeamArenaRankAllInfo(protocol)
	self.neo_rank_data.resi_team_num = protocol.resi_team_num
	self.neo_rank_data.rank_list = protocol.rank_list
	self.neo_rank_data.flush = not self.neo_rank_data.flush

	self.battle_scene_info.resi_team_num = protocol.resi_team_num
end

function HeroVanBattleData:SetTeamTeamArenaMyInfo(protocol)
	-- if protocol.type == 0 then 
		self.battle_scene_info.rank_posi = protocol.rank_posi
		self.battle_scene_info.my_info = protocol.my_info
		self.battle_scene_info.flush = not self.battle_scene_info.flush
	--else 

	-- end 
	if protocol.type == 1 and self.neo_rank_data.rank_list ~= nil then 
		ViewMgr:OpenView(HeroVanBattleGloryView)
		-- if self.neo_rank_data.rank_list[protocol.rank_posi] ~= nil 
	end
	-- 
end

function HeroVanBattleData:SetTeamTeamArenaSpecialInfo(protocol)
	self.battle_scene_info.resi_team_num = protocol.resi_team_num
	self.battle_scene_info.flush = not self.battle_scene_info.flush
end

function HeroVanBattleData:SetArenaTeamViewSpecialInfo(protocol)
	self.team_view_mine_team.member_list = protocol.member_list
end

function HeroVanBattleData:SetTeamArenaFightInfo(protocol)
	self.start_view_data.start_fight_timestamp = protocol.start_fight_timestamp
	self.start_view_data.fight_info = protocol.fight_info

	
	ViewMgr:OpenView(HeroVanBattleStart)
end 

function HeroVanBattleData:SetTeamArenaFinallyRankInfo(protocol)
	-- LogDG("?杰哥救救我！4614",protocol)
	self.finally_rank_data.my_rank_index = protocol.my_rank_index 
	self.finally_rank_data.my_rank_info = protocol.my_rank_info
	self.finally_rank_data.rank_list = protocol.rank_list
	self.finally_rank_data.flush = not self.finally_rank_data.flush
end

function HeroVanBattleData:SetTeamArenaOtherInfo(protocol)
	self.other_info.is_sign_up = protocol.is_sign_up

	self.other_info.flush = not self.other_info.flush
end

-- 战队界面
-- 以展示队伍的方式打开战队一览
function HeroVanBattleData:TypeTeamViewShowTeam()
	self.team_view_data.can_enroll = false
	ViewMgr:OpenView(HeroVanBattleTeamView)
end

-- 以报名的方式打开战队一览
function HeroVanBattleData:TypeTeamViewEnroll()
	self.team_view_data.can_enroll = true
	ViewMgr:OpenView(HeroVanBattleTeamView)
end

function HeroVanBattleData:GetMyTeamInfo()
	if self.team_simple_info.my_team_index ~= nil and self.team_simple_info.my_team_index > -1 then 
		return self.team_simple_info.team_list[self.team_simple_info.my_team_index]
	end 
	return nil
end

-- 结构 {my_team,other_team,knows_team}
function HeroVanBattleData:TeamViewGetTeamList()
	local teams = {
		[1] = {name = Language.HeroVanBattle.TeamName[1],children = {}},
		[2] = {name = Language.HeroVanBattle.TeamName[2],children = {}},
		[3] = {name = Language.HeroVanBattle.TeamName[3],children = {}},
	}
	local function checkIsKnows(index)
		for k,v in pairs(self.team_simple_info.know_team_index or {}) do 
			if v == index then 
				return true
			end 
		end 
		return false
	end
	for k,v in pairs(self.team_simple_info.team_list or {}) do 
		if k == self.team_simple_info.my_team_index then
			table.insert( teams[1].children, v )
			table.insert( teams[2].children, v )
		elseif checkIsKnows(k) then 
			table.insert( teams[2].children, v )
			table.insert( teams[3].children, v )
		else 
			table.insert( teams[2].children, v )
		end 
	end 

	if self.search_team_data.key_word then 
		local teams = self:TypeTeamSearchList(teams)
	end 

	for k,v in pairs(teams) do 
		if #v.children == 0 then
			local vo = {}
			vo.is_empty = true
			vo.is_us = k == 1
			table.insert( v.children, vo )
		end 
	end 

	return teams
end

-- 
function HeroVanBattleData:TypeTeamSearchList(list)
	-- 逆向搜索，倘若不符合便删除
	for k,v in pairs(list) do 
		for i,j in pairs(v.children) do 
			local flag_team_name = string.find(j.team_name,self.search_team_data.key_word or "") == nil
			local flag_member_name = true
			for list,member in pairs(j.member_list) do 
				-- if string.find(member.role_name,self.search_team_data.key_word or "") ~= nil then 
				-- 	flag_member_name = false
				-- end 
			end 
			if flag_member_name and flag_team_name then
				table.remove(v.children,i)
			end 
		end 
	end 

	return list
end

function HeroVanBattleData:SetSearchKeyWord(word)
	self.search_team_data.key_word = word
	self.search_team_data.flush = not self.search_team_data.flush
end

function HeroVanBattleData:IsMyTeamMember(role_id)
	local team_info = TeamData.Instance:GetTeamInfo()
	local team_list = team_info.member_list 

	for k,v in pairs(team_list) do
		if  v.info.uid == role_id then 
			return true
		end 
    end 

	return false
end

function HeroVanBattleData:TeamViewGetMineTeamlist()
	local team_info = TeamData.Instance:GetTeamInfo()
	local team_list = team_info.member_list 

	local member_list = {}
	for k,v in pairs(team_list) do 
		local vo = {}
		if v.info ~= nil then 
		    vo.role_uid = v.info.uid
		    vo.profession = v.info.prof
		    vo.level = v.info.level

		    vo.avatar_type = v.info.appearance.avatar_type
		    vo.headshot_id = v.info.appearance.avatar_id
		    vo.fly_flag = v.info.appearance.avatar_quality
		    vo.role_name = v.info.name

			vo.is_master = k == 1 
		    for i,j in pairs(self.team_view_mine_team.member_list) do 
			    if j.role_uid == vo.role_uid then 
				    vo.rank_list = j.rank_list
			    end 
		    end 
		    table.insert(member_list,vo)
		end
	end 

	return member_list
end

function HeroVanBattleData:IsNeedFlushSign()
	return self.team_simple_info.team_list == nil
end

-- npc是否展示报名按钮
function HeroVanBattleData:TeamViewNpcCouldShow()
	local server_time = TimeManager:GetServerTime()
	if self.act_info.start_sign_up_timestamp == nil then return false end 
	return server_time > self.act_info.start_sign_up_timestamp and server_time < self.act_info.end_sign_up_timestamp
end

-- npc是否展示入场按钮
function HeroVanBattleData:EnterSceneNpcCouldShow()
	local server_time = TimeManager:GetServerTime()
	local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.HeroVanBattle)
	if enter_info == nil or enter_info.enter_end_timestamp == nil then return false end 
	return server_time < enter_info.enter_end_timestamp and ActivityData.IsOpen(ActType.HeroVanBattle)
end

-- npc是否展示实时排名按钮
function HeroVanBattleData:NeoRankNpcCouldShow()
	local server_time = TimeManager:GetServerTime()
	local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.HeroVanBattle)
	if enter_info == nil or enter_info.enter_end_timestamp == nil then return false end 
	return server_time > enter_info.enter_end_timestamp and ActivityData.IsOpen(ActType.HeroVanBattle)
end

-- npc是否展示赛后排名按钮
function HeroVanBattleData:EndRankNpcCouldShow()
	local server_time = TimeManager:GetServerTime()
	local act_flag = ActivityData.IsOpen(ActType.HeroVanBattle)
	if self.act_info.start_sign_up_timestamp == nil then return false end 
	return server_time < self.act_info.start_sign_up_timestamp and not act_flag 
end

function HeroVanBattleData:HeroVanHighMark(act_type)
	if act_type ~= ActType.HeroVanBattle then 
		return false
	end 

	return not self:TypeApplyIsCreateSuccess() and self:TeamViewNpcCouldShow()

end

-- 展示结束时间
function HeroVanBattleData:EndRankMainViewShowTime()
	-- local act_info = ActivityData.Instance:GetActivityStatusInfo(ActType.HeroVanBattle)

	-- local end_stamp = ActivityData.GetSwitchStampTime(ActType.HeroVanBattle)

	-- local server_time = TimeManager:GetServerTime()
	local dates = os.date("*t",self.act_info.activity_open_next_zero_timestamp)
	-- -- local dates = os.date("*t",server_time)
	dates.day = dates.day + 1

	local time = TimeCtrl.Instance:ChangDataStampTime(dates) or 0

	return time
end

function HeroVanBattleData:IsSignTiming()
	local server_time = TimeManager:GetServerTime()

	if self.act_info.start_sign_up_timestamp == nil then 
		return false ,0
	end 

	if self.act_info.start_sign_up_timestamp <= server_time and 
	    self.act_info.end_sign_up_timestamp >= server_time then 
			return true,self.act_info.end_sign_up_timestamp
	end 

	return false,0
end

function HeroVanBattleData:IsShowRankMainView()
	local act_info = ActivityData.Instance:GetActivityStatusInfo(ActType.HeroVanBattle)
	if act_info == nil then 
		return 
	end 
	local dates = os.date("*t",act_info.param_2)

	local server_time = TimeManager:GetServerTime()
	local act_flag = ActivityData.IsOpen(ActType.HeroVanBattle)

	return not act_flag and server_time < self:EndRankMainViewShowTime()
end

-- 报名界面
-- 以队长的方式打开报名界面
-- 此时的data内没有信息
function HeroVanBattleData:TypeApplyViewLeader()
	self.apply_view_data.is_leader = true
	ViewMgr:OpenView(HeroVanBattleApplyView)
end

-- 以队员的方式打开报名界面
-- 此时的data内有信息
function HeroVanBattleData:TypeApplyViewMember()
	self.apply_view_data.is_leader = false
	ViewMgr:OpenView(HeroVanBattleApplyView)
end

function HeroVanBattleData:TeST()
	
end

function HeroVanBattleData:TypeApplyGetMemberList(init)
	local team_info = TeamData.Instance:GetTeamInfo()
	local team_list = team_info.member_list 
	local member_list = {}
	for k,v in pairs(team_list) do 
		local vo = {}	
		if v.info ~= nil then 
		    vo.avatar_type = v.info.appearance.avatar_type
			vo.avatar_id = v.info.appearance.avatar_id
			vo.fly_flag = v.info.appearance.avatar_quality
			vo.name = v.info.name
			vo.is_leader = k == 1
			if self:TypeApplyIsMasterWaiting() then 
				vo.determine = false
			else 
				for i,j in pairs(self.team_pre_signup.member_list) do 
					if j.role_id == v.info.uid then  
						vo.determine = j.determine_flag == 1
					end 
				end 
			end 
			table.insert(member_list,vo)
		end 
	end 

	return member_list
end

function HeroVanBattleData:TypeApplyIsMasterWaiting()
	return self.team_pre_signup.team_name == nil 
end

-- 直接对接按钮，true则可以点击
function HeroVanBattleData:TypeApplyMineIsDetermine()
	if self.team_pre_signup.team_name == nil then 
		return true
	end 

	for k,v in pairs(self.team_pre_signup.member_list) do 
		if RoleData.Instance:GetRoleData().role_id == v.role_id then 
			return v.determine_flag == 0
		end  
	end 

	return true
end

-- 全部确定
function HeroVanBattleData:TypeApplyAllIsDetermine()
	if self:TypeApplyIsMasterWaiting() then return false end 
	for k,v in pairs(self.team_pre_signup.member_list) do 
		if v.determine_flag == 0 then 
			return false
		end 
	end 
	return true
end

function HeroVanBattleData:TypeApplyIsCreateSuccess()
	-- 如果是空的就走一次协议
	if self.other_info.is_sign_up == nil then
		HeroVanBattleCtrl.Instance:SendSignInfo()
	end 
	return self.other_info.is_sign_up ~= nil and self.other_info.is_sign_up == 1
end

-- 英雄帖界面
function HeroVanBattleData:TypeRuleGetNeedShow()
	local role_id = RoleData.Instance:GetRoleId()
	local pass_time = UnityPlayerPrefs.GetInt(role_id.."VanRulePass") 

	if pass_time ~= self.act_info.season_num then 
	    ViewMgr:OpenView(HeroVanBattleRuleView)
		UnityPlayerPrefs.SetInt(role_id.."VanRulePass",self.act_info.season_num)
	else 
		self:TypeTeamViewEnroll()
	end 
end

function HeroVanBattleData:CheckWorldLevel()
	return RoleData.Instance:GetWorldLevel() >= Config.hero_competition_auto.other[1].level
end

function HeroVanBattleData:MemberTeamCheck()
	-- LogError("@?!@")
	if not LoginData.Instance:IsOnCrossSever() then 
		LogDG("?没跨服")
		LoginCtrl.Instance:SendCrossStartReq()
		return false--self:TypeApplyIsCreateSuccess()
	end 

	if not TeamData.Instance:InTeam() then 
		LogDG("?不在队伍里")
		return false
	end 

	if self.team_simple_info.team_list == nil then 
		HeroVanBattleCtrl.Instance:SendTeamArenaReq(0)
		-- PublicPopupCtrl.Instance:Center(Language.HeroVanBattle.NameLonger)
		LogDG("?没有队伍信息")
		return false
	end 

    local team_info =  TeamData.Instance:GetTeamInfo()
    local member_list = team_info.member_list 

	for k,v in pairs(self.team_simple_info.team_list) do 
		for index,member in pairs(v.member_list) do 
			for i,j in pairs(member_list) do 
				if j.info.uid == member.role_id then 
					LogDG("队员·"..j.info.name.."被检测为已加入战队·"..v.team_name)
					return false
				end 
			end 	
		end 
	end
	
	return true
end

