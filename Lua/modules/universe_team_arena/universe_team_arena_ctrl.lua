FILE_REQ("modules/universe_team_arena/universe_team_arena_data")
FILE_REQ("modules/universe_team_arena/universe_team_arena_cfg")
FILE_REQ("modules/universe_team_arena/universe_team_arena_class")

VIEW_REQ("modules/universe_team_arena/universe_team_arena_main")
VIEW_REQ("modules/universe_team_arena/universe_team_arena")
VIEW_REQ("modules/universe_team_arena/universe_team_arena_poster")
VIEW_REQ("modules/universe_team_arena/uta_team_list_panel")
VIEW_REQ("modules/universe_team_arena/uta_knockout_panel")
VIEW_REQ("modules/universe_team_arena/uta_second_panel")
VIEW_REQ("modules/universe_team_arena/uta_battle_array")
VIEW_REQ("modules/universe_team_arena/universe_team_arena_reward")
VIEW_REQ("modules/universe_team_arena/universe_team_arena_vs")
VIEW_REQ("modules/universe_team_arena/uta_all_match")
VIEW_REQ("modules/universe_team_arena/uta_champion_team")
VIEW_REQ("modules/universe_team_arena/uta_end_rank")

VIEW_REQ("modules/universe_team_arena/guessing/uta_guessing")
VIEW_REQ("modules/universe_team_arena/guessing/uta_guessing_store")
VIEW_REQ("modules/universe_team_arena/guessing/uta_guessing_bet")
VIEW_REQ("modules/universe_team_arena/guessing/uta_guessing_record")


--  >>>> Gm命令 <<<<
-- uta_restart_season:    进入下一赛季
-- fake_uta_teams:32 1    加入32个队伍（1表是我自己也进入其中一个队伍中，如果填0则不会加入任何一个队伍）
-- uta_next_status:       进入下一个阶段（这里是资格赛阶段）
-- uta_mock_battles:      随机本轮战斗所有比赛的结果
-- uta_next_qualification: 进入下一轮资格赛比赛
-- 重复uta_mock_battles和uta_next_qualification 经过10轮战斗后......

-- uta_next_status:       进入下一个阶段（这里是正赛阶段）
-- uta_mock_battles:      随机本轮战斗所有比赛的结果
-- uta_next_main: 		   进入下一轮正赛比赛
-- 重复uta_mock_battles和uta_next_main 经过5轮战斗后.......   

-- uta_next_status:       进入下一个阶段（结算阶段）
-- uta_next_status:       进入下一个阶段（休赛阶段）


-- 若需要进入战斗，在uta_mock_battles之前输入以下指令，即可进入战斗
-- uta_start_battle:   开启当前轮次战斗
-- utaenterscene:      进入战斗场景
-- uta_fight_now:      立即进入战斗

-- addutacr:4		-- 添加冠军队伍

-- ==================== 天下第一比武大会 ================
UniverseTeamArenaCtrl = UniverseTeamArenaCtrl or BaseClass(BaseCtrl)
function UniverseTeamArenaCtrl:__init()
	if UniverseTeamArenaCtrl.Instance ~= nil then
		Debuger.LogError("[UniverseTeamArenaCtrl] attempt to create singleton twice!")
		return
	end	
    UniverseTeamArenaCtrl.Instance = self
	UniverseTeamArenaData.New()
	self.data = UniverseTeamArenaData.Instance
	self:RegisterAllProtocols()
	self.is_poster = false
	ActivityData.Instance:ActStatus(ActType.UTArena, ActStatusType.Close)
end

function UniverseTeamArenaCtrl:__delete()
end

function UniverseTeamArenaCtrl:OnInit()
	ActivityRandData.Instance:Register(Mod.UniverTeamArena.Main,function()
		local is_open = self.data:IsOpen()
		UniverseTeamArenaCtrl.Log("刷新了天下第一活动开启 isOpen == ", is_open)
		return is_open
	end,self.data.base_data)

	-- 红点
	Remind.Instance:Register(Mod.UniverTeamArena.Main,self.data.remind_data,function() 
		local num = self.data:RemindNum()
		return num
	end)

	-- npc按钮
	BehaviorData.Instance:CustomNpcTalkBtnsId(self.data:MainNpcId(), BindTool.Bind(self.SetupBehaviourBtn, self))

	-- 活动前往
	ActivityData.Instance:CustomClickHandle(ActType.UTArena,function(is_challenge_view)
		if self.data:IsNeedClose() then
			PublicPopupCtrl.Instance:Error(Language.UTAPanel.Tip12)
			return true
		end
		if is_challenge_view then
			ViewMgr:OpenView(UniverseTeamArenaMain)
		else
			SceneLogic.Instance:AutoToNpc(self.data:MainNpcId(), nil, false, true)
		end
	end)

	ActivityData.Instance:CustomClickHandle(ActType.UTArena2,function()
		local is_open = self.data:UTArena2IsOpen()
		if not is_open then
			return
		end

		local status_info = self.data:GetMatchActInfo()
		local status = status_info.status
		if status ~= ActStatusType.Close then
			SceneLogic.Instance:AutoToNpc(self.data:MainNpcId(), nil, false, true)
		else
			self.data.is_jump_match = true
    		ViewMgr:OpenView(UniverseTeamArenaView)
		end
	end)

	ActivityData.Instance:RegisterTopCondition(ActType.UTArena,function()
        return self.data:GetTopConditionTip()
	end)

	ActiveData.Instance:RegisterActivitySpBtnName(ActType.UTArena2,BindTool.Bind(self.ActiveSpBtn, self))
    -- ActivityData.Instance:RegisterActivitySpStatus(ActType.HeroVanBattle,BindTool.Bind(self.ActiveSpStatus, self))
	-- ActiveData.Instance:RegisterActivitySpBtnUnAct(ActType.HeroVanBattle,BindTool.Bind(self.ActiveSpBtnUnAct, self))

	ActivityData.Instance:CustomStatusButtonName(ActType.UTArena, ActStatusType.Close, Language.UTA.ActEndBtn)
	ActivityData.Instance:CustomStatusButtonName(ActType.UTArena, ActStatusType.Standy, Language.UTA.ActEndBtn)
	ActivityData.Instance:CustomStatusButtonName(ActType.UTArena, ActStatusType.Open, Language.UTA.ActEndBtn)
	
	self.on_scene_data_inited = EventSys:Bind(GlobalEventKeys.SceneDataInited,BindTool.Bind(self.OnSceneDataInited, self))
	self.on_scene_data_disposed = EventSys:Bind(GlobalEventKeys.SceneDataDisposed,BindTool.Bind(self.OnSceneDataDisposed, self))
	self.on_scene_data_battle_end = EventSys:Bind(GlobalEventKeys.BattleEnd,BindTool.Bind(self.OnSceneDataInited, self))
end

function UniverseTeamArenaCtrl:OnUnloadGameLogic()
	self.data:ResetData()
	self.is_champion_creator = nil
end

function UniverseTeamArenaCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSUTATeamListReq)				--//!< 5051 请求参赛队伍列表
	self:RegisterProtocol(CSUTATeamDetailReq)			--//!< 5052 请求某个队伍的详细信息 ::::::: 返回5004协议
	self:RegisterProtocol(CSUTATeamScoreRankListReq)	--//!< 5055 请求淘汰资格赛积分排行榜
	self:RegisterProtocol(CSUTAWinBox)					--//!< 5054领取胜场宝箱
	self:RegisterProtocol(CSUTAMyTeamQualificationScheduleReq)	--//!< 5053 请求我的队伍资格赛赛程 ：：：： 返回 5003协议 5005
	self:RegisterProtocol(CSUTAMainSchedules)			-- //!< 5056 请求正赛赛程
	self:RegisterProtocol(CSUTAFinalRankList)			-- //!< 5057 请求最终榜单
	self:RegisterProtocol(CSUTAQualificationSchedules)	-- //!< 5058 请求资格赛赛程
	self:RegisterProtocol(CSUTAMainSchedulesOverview)	--//!< 5059 请求正赛赛程总览
	self:RegisterProtocol(CSUTAEnterScene)				--//!< 5060 请求进入战斗场景
	self:RegisterProtocol(CSUTAWatchVideo)				--//!< 5061 查看录像
	self:RegisterProtocol(CSUTAStartOB)					--//!< 5062 实时观战
	self:RegisterProtocol(SCUTABaseInfo, "OnSCUTABaseInfo")		--//!< 5001 赛季信息
	self:RegisterProtocol(SCUTATeamList, "OnSCUTATeamList")		--//!< 5002 参赛队伍列表（已有序）
	self:RegisterProtocol(SCUTAMyTeamQualificationSchedules, "OnSCUTAMyTeamQualificationSchedules")	-- //!< 5003 我的队伍资格赛赛程
	self:RegisterProtocol(SCUTATeamDetailInfo, "OnSCUTATeamDetailInfo")					--//!< 5004 查询队伍详情回包
	self:RegisterProtocol(SCUTAMyTeamBaseInfo, "OnSCUTAMyTeamBaseInfo")					--//!< 5005 我的队伍基本信息
	self:RegisterProtocol(SCUTATeamScoreRankList, "OnSCUTATeamScoreRankList")			-- 5006 战队当前积分排名（已有序）
	self:RegisterProtocol(SCUTAMainSchedules, "OnSCUTAMainSchedules")					-- //!< 5007 正赛赛程下发
	self:RegisterProtocol(SCUTAFinalRankList, "OnSCUTAFinalRankList")					--//!< 5008 最终榜单(已有序)
	self:RegisterProtocol(SCUTAMainOverviewSchedules, "OnSCUTAMainOverviewSchedules")	-- //!< 5009 晋级赛赛况总览
	self:RegisterProtocol(SCUTAQualificationSchedules, "OnSCUTAQualificationSchedules")	--//!< 5010 淘汰赛赛程
	self:RegisterProtocol(SCUTASceneInfo, "OnSCUTASceneInfo")			-- 5011 战斗场景内的信息
	self:RegisterProtocol(SCUTABattleVSInfo, "OnSCUTABattleVSInfo")		-- 5012 对阵信息 （自行倒计时3秒）
	self:RegisterProtocol(SCUTAPrepareEnterSceneNotice, "OnSCUTAPrepareEnterSceneNotice")		-- // 5013 准备进场通知
	self:RegisterProtocol(SCUTABattleResultShow, "OnSCUTABattleResultShow")		-- 5014天下第一战斗结果显示

	self:RegisterProtocol(CSGetUTAChampionRecord)		-- 天下第一比武大会获取冠军记录
	self:RegisterProtocol(SCGetUTAChampionRecordAck, "OnSCGetUTAChampionRecordAck")		--4963 冠军记录手册[组队] 信息

	-- 竞猜
	self:RegisterProtocol(CSUTAGuessReq)	-- 请求竞猜相关操作
	self:RegisterProtocol(SCUTAGuessMatchInfo, "OnSCUTAGuessMatchInfo")	-- 某一个比赛的竞猜信息
	self:RegisterProtocol(SCUTAGuessBetRecord, "OnSCUTAGuessBetRecord")	-- 玩家参与下注的列表
	self:RegisterProtocol(SCUTAGuessHistoryEarning, "OnSCUTAGuessHistoryEarning")	-- 4971累计竞猜收益
	self:RegisterProtocol(SCUTAGuessHasBetRewardNotice, "OnSCUTAGuessHasBetRewardNotice")	-- 4972有可领竞猜奖励通知
end

function UniverseTeamArenaCtrl:SetupBehaviourBtn(btn_list,obj_vo)
	if obj_vo and obj_vo.id == self.data:MainNpcId() then

        local button = NpcDialogBehBtnData.FuncCreate(
			Language.UTA.NPCBtn.Btn1,function()
				-- 首先判断活动是否开启
				-- local state = UniverseTeamArenaData.Instance:ActState()
				-- if state == UTACfg.ActStatus.Pause or state ~= UTACfg.ActStatus.Nothing then
				-- 	PublicPopupCtrl.Instance:Center("当前不是比赛阶段")
				-- 	return
				-- end
				-- 是否有资格
				-- 判断是否处于准备阶段
				UniverseTeamArenaCtrl.Instance:SendCSUTAEnterScene()
            end)
		button:SetIconType(NpcIconType.Act)
		table.insert(btn_list, button)

		local status = self.data:ActState()

		local button = NpcDialogBehBtnData.FuncCreate(
			Language.UTA.NPCBtn.Btn2,function()
				ViewMgr:OpenView(UniverseTeamArenaView)
			end)
		if status ~= UTACfg.ActStatus.Nothing and status ~= UTACfg.ActStatus.Pause and status ~= UTACfg.ActStatus.Ready then
			table.insert(btn_list, button)
		end

		local button = NpcDialogBehBtnData.FuncCreate(
			Language.UTA.NPCBtn.Btn3,function()
				ViewMgr:OpenView(UtaEndRank)
			end)
		if status == UTACfg.ActStatus.Pause then
			table.insert(btn_list, button)
		end
		
		local button = NpcDialogBehBtnData.FuncCreate(
			Language.UTA.NPCBtn.Btn4,function()
				ViewMgr:OpenView(UniverseTeamArenaMain, {tabIndex = 1})
            end)
		table.insert(btn_list, button)
    end
end

function UniverseTeamArenaCtrl:ActiveSpBtn()
	local is_open = self.data:UTArena2IsOpen()
	if not is_open then
		return Language.UTAPanel.Ctrl.Text1
	end

	local status_info = self.data:GetMatchActInfo()
	local status = status_info.status
	if status == ActStatusType.Close then
		if self.data.base_data.is_in_progress == 1 then
			return Language.UTAPanel.Ctrl.Text2
		else
			return Language.UTAPanel.Ctrl.Text3
		end
	elseif status == ActStatusType.Standy then
		return Language.UTAPanel.Ctrl.Text4
	else
		return Language.UTAPanel.Ctrl.Text5
	end
end

function UniverseTeamArenaCtrl:OnSceneDataInited(scene_id)
	scene_id = scene_id or SceneData.Instance.scene_id
	--LogError("OnSceneDataInited::::",scene_id,SceneLogic.Instance:MainCityID(),self.data:GetCurChampionData())
    if scene_id ~= SceneLogic.Instance:MainCityID() then
        return
	end

	if TableIsEmpty(self.data:GetCurChampionData()) then
		-- 请求冠军数据
		local index = self.data:GetShowObjIndex()
		if index and index > 0 then
			self:SendCSGetUTAChampionRecord(0, index - 1)
		end
	else
		Invoke(function ()
			self:CreateChampionObj()
		end,0.1)
	end

	-- 这里是每次都请求，这样太会浪费了
	-- local index = self.data:GetShowObjIndex()
	-- if index and index > 0 then
	-- 	self:SendCSGetUTAChampionRecord(index - 1, index - 1)
	-- end
end

function UniverseTeamArenaCtrl:OnSceneDataDisposed(scene_id)
	--LogError("OnSceneDataDisposed",scene_id)
	if not TableIsEmpty(self.data.handle_runners) then
		for _, value in pairs(self.data.handle_runners) do
			Runner.Instance:RemoveRunUntilTrue(value)
		end
	end
	self.data.handle_runners = {}
	if not TableIsEmpty(self.data.state_objs) then
		for _, value in pairs(self.data.state_objs) do
			if value.vo then
				local obj_id = value.vo.obj_id
				SceneCtrl.Instance:Objs():DeleteClientObj(obj_id)
			end
		end
	end
	self.data.state_objs = {}
end

function UniverseTeamArenaCtrl:CreateChampionObj()
	self.data:CreateChampionObj()
end


-- 请求参赛名单
function UniverseTeamArenaCtrl:SendCSUTATeamListReq()
	local protocol = GetProtocol(CSUTATeamListReq)
    SendProtocol(protocol)
end

-- 请求某个队伍的详细信息
function UniverseTeamArenaCtrl:SendCSUTATeamDetailReq(team_id_t)
	if not team_id_t then
		return
	end
	local protocol = GetProtocol(CSUTATeamDetailReq)
	protocol.team_id_t = team_id_t
    SendProtocol(protocol)
end

-- --//!< 5054 请求胜场宝箱
function UniverseTeamArenaCtrl:SendCSUTAWinBox(sn)
	local protocol = GetProtocol(CSUTAWinBox)
	protocol.sn = sn
    SendProtocol(protocol)
end

-- --//!< 5055 请求淘汰资格赛积分排行榜
function UniverseTeamArenaCtrl:SendCSUTATeamScoreRankListReq()
	local protocol = GetProtocol(CSUTATeamScoreRankListReq)
    SendProtocol(protocol)
end

--//!< 5053 请求我的队伍资格赛赛程 ：：：： 返回 5003协议
function UniverseTeamArenaCtrl:SendCSUTAMyTeamQualificationScheduleReq()
	local protocol = GetProtocol(CSUTAMyTeamQualificationScheduleReq)
    SendProtocol(protocol)
end

-- 请求天下第一比武大会获取冠军记录
function UniverseTeamArenaCtrl:SendCSGetUTAChampionRecord(begin_index, end_index)
	local protocol = GetProtocol(CSGetUTAChampionRecord)
	protocol.begin_index = begin_index
	protocol.end_index = end_index
    SendProtocol(protocol)
end

-- //!< 5056 请求正赛赛程 -- 第X轮正赛 (0:32进16  1:16进8  2:8进4  3：4进2  4：总决赛
function UniverseTeamArenaCtrl:SendCSUTAMainSchedules(round_idx)
	local protocol = GetProtocol(CSUTAMainSchedules)
	protocol.round_idx = round_idx
    SendProtocol(protocol)
end

-- //!< 5057 请求最终榜单
function UniverseTeamArenaCtrl:SendCSUTAFinalRankList()
	local protocol = GetProtocol(CSUTAFinalRankList)
    SendProtocol(protocol)
end

--//!< 5058 请求资格赛赛程
function UniverseTeamArenaCtrl:SendCSUTAQualificationSchedules(round_idx)
	local protocol = GetProtocol(CSUTAQualificationSchedules)
	protocol.round_idx = round_idx
    SendProtocol(protocol)
end

--//!< 5059 请求正赛赛程总览
function UniverseTeamArenaCtrl:SendCSUTAMainSchedulesOverview(overview_type, is_last)
	local protocol = GetProtocol(CSUTAMainSchedulesOverview)
	protocol.overview_type = overview_type
	protocol.is_last_season = is_last or 0
    SendProtocol(protocol)
end

--//!< 5060 请求进入战斗场景
function UniverseTeamArenaCtrl:SendCSUTAEnterScene()
	local protocol = GetProtocol(CSUTAEnterScene)
    SendProtocol(protocol)
end

--//!< 5061 查看录像
function UniverseTeamArenaCtrl:SendCSUTAWatchVideo(video_id_t)
	local protocol = GetProtocol(CSUTAWatchVideo)
	protocol.video_id_t = video_id_t
    SendProtocol(protocol)
end

--//!< 5062 实时观战
function UniverseTeamArenaCtrl:SendCSUTAStartOB(match_id_t)
	local protocol = GetProtocol(CSUTAStartOB)
	protocol.match_id_t = match_id_t
    SendProtocol(protocol)
end

-- 请求竞猜相关操作
function UniverseTeamArenaCtrl:SendCSUTAGuessReq(req_type, p1_t, p2_t, p3)
	local protocol = GetProtocol(CSUTAGuessReq)
	protocol.req_type = req_type
	protocol.p1_t = p1_t or {}
	protocol.p2_t = p2_t or {}
	protocol.p3 = p3 or 0
    SendProtocol(protocol)
end

--//!< 5001 赛季信息
function UniverseTeamArenaCtrl:OnSCUTABaseInfo(protocol)
	local status = protocol.info.cur_status
	if status == UTACfg.ActStatus.Nothing then
		ActivityData.Instance:ActStatus(ActType.UTArena, ActStatusType.Close)
		self.data:BaseData(protocol.info)
		return
	end
	ActivityData.Instance:ActStatus(ActType.UTArena, ActStatusType.Open)
	self.data:BaseData(protocol.info)
	if self.data:IsOpen() and status ~= UTACfg.ActStatus.Pause and status ~= UTACfg.ActStatus.Nothing and not self.is_poster then
		local flag = UniverseTeamArenaData.Instance:GetPosterPrefFlag()
		if not flag then
			ViewMgr:OpenView(UniverseTeamArenaPoster)
		end
		self.is_poster = true
	end

	local is_open = self.data:UTArena2IsOpen()
	local uta2_status = is_open and ActStatusType.Open or ActStatusType.Close
	--LogError(":::",is_open, uta2_status)
	local status_info = {
        act_type = ActType.UTArena2,
        status = uta2_status,
    }
	ActivityData.Instance.activity_status_list[ActType.UTArena2] = status_info

	-- 这个数据慢，第一次需刷新下冠军队伍数据
	if not self.is_champion_creator then
		self:OnSceneDataInited()
		self.is_champion_creator = true
	end
end

--//!< 5002 参赛队伍列表（已有序）
function UniverseTeamArenaCtrl:OnSCUTATeamList(protocol)
	local team_list = protocol.team_list
	self.data:TeamAllListData(team_list)
end

-- //!< 5003 我的队伍资格赛赛程
function UniverseTeamArenaCtrl:OnSCUTAMyTeamQualificationSchedules(protocol)
	local schedule_list = protocol.schedule_list
	self.data.self_out_match_list:Set(schedule_list)
end

--//!< 5004 查询队伍详情回包
function UniverseTeamArenaCtrl:OnSCUTATeamDetailInfo(protocol)
	local info = protocol.info
	self.data:BattleArrayData(info)
end

--//!< 5005 我的队伍基本信息
function UniverseTeamArenaCtrl:OnSCUTAMyTeamBaseInfo(protocol)
	local info = protocol.info
	self.data.self_team_base_data:Set(info)
end

-- 5006 战队当前积分排名（已有序）
function UniverseTeamArenaCtrl:OnSCUTATeamScoreRankList(protocol)
	self.data:SetOutRankData(protocol)
end

-- //!< 5007 正赛赛程下发
function UniverseTeamArenaCtrl:OnSCUTAMainSchedules(protocol)
	self.data:SetMainAllMatchList(protocol)
end

--//!< 5008 最终榜单(已有序)
function UniverseTeamArenaCtrl:OnSCUTAFinalRankList(protocol)
	self.data:SetEndRankData(protocol)
end

--4963 冠军记录手册[组队] 信息
function UniverseTeamArenaCtrl:OnSCGetUTAChampionRecordAck(protocol)
	self.data:SetChampionData(protocol)
	self:CreateChampionObj()
end

-- //!< 5009 晋级赛赛况总览
function UniverseTeamArenaCtrl:OnSCUTAMainOverviewSchedules(protocol)
	self.data:SetPreviewAllMatchList(protocol)
end

--//!< 5010 淘汰赛赛程
function UniverseTeamArenaCtrl:OnSCUTAQualificationSchedules(protocol)
	self.data:SetKnockoutAllMatchList(protocol)
end

-- 5011 战斗场景内的信息
function UniverseTeamArenaCtrl:OnSCUTASceneInfo(protocol)
	local info = protocol.info
	self.data.ready_scene_data:Set(info)
end

-- 5012 对阵信息 （自行倒计时3秒
function UniverseTeamArenaCtrl:OnSCUTABattleVSInfo(protocol)
	self.data.vs_data:Set(protocol.info)
	ViewMgr:OpenView(UniverseTeamArenaVs)
end

-- 5014天下第一战斗结果显示
function UniverseTeamArenaCtrl:OnSCUTABattleResultShow(protocol)
	local info = protocol.info
	local tip = ""
	local color
	if info.is_win == 2 then
		tip = Language.UTAPanel.Ctrl.Text6
		color = COLORSTR.Yellow6
	elseif info.is_win == 1 then
		if info.win_reason == 1 then
			tip = Language.UTAPanel.Ctrl.Text7
		else
			tip = Language.UTAPanel.Ctrl.Text8
		end
		color = COLORSTR.Yellow6
	else
		tip = Language.UTAPanel.Ctrl.Text6
		color = COLORSTR.Blue10
	end
	if info.match_type == UTACfg.MatchType.Out then
		tip = tip .. string.format(Language.UTAPanel.Ctrl.Text9, info.modify_score, info.cur_score)
	end

	ViewMgr:OpenView(UTAFinishPassView, {is_win = info.is_win, tip = tip})

	local act_info = TopLayerActTipsData.Instance:GetCurShowAct()
	ActivityRemindData.Instance:DeleteRemindItem(ActType.UTArena)
	if next(act_info) and act_info.act_type == ActType.UTArena then
		PublicPopupCtrl.Instance:CloseActTips()
		ViewMgr:CloseView(ActivityRemindTip)
	end
	ActivityData.Instance.activity_status_event:Notify()

	local match_act_state = self.data:GetMatchActInfo()
	match_act_state.status = ActStatusType.Close
end

-- // 5013 准备进场通知
function UniverseTeamArenaCtrl:OnSCUTAPrepareEnterSceneNotice(protocol)
	local info = protocol.info
	self.data:SetMatchActInfo(protocol)
	self.data.enter_scene_notice_data:Set(info)
	local match_act_state = self.data:GetMatchActInfo()

	if match_act_state.status == ActStatusType.Open or match_act_state.status == ActStatusType.Standy then
		local act_info = ActivityData.Instance:GetActivityInfo(ActType.UTArena)
		local stop_time = match_act_state.status == ActStatusType.Standy and info.start_enter_scene_time or info.stop_enter_scene_time
		PublicPopupCtrl.Instance:ActTips(act_info,stop_time)
		ActivityRemindData.Instance:SetRemindList(act_info,stop_time)
		ActivityData.Instance.activity_status_list[ActType.UTArena] = match_act_state
		ActivityData.Instance.activity_status_event:Notify()
	else
		local act_info = TopLayerActTipsData.Instance:GetCurShowAct()
		ActivityRemindData.Instance:DeleteRemindItem(ActType.UTArena)
		if next(act_info) and act_info.act_type == ActType.UTArena then
			PublicPopupCtrl.Instance:CloseActTips()
			ViewMgr:CloseView(ActivityRemindTip)
		end
	end
end

function UniverseTeamArenaCtrl:ReqMatchGuessInfo(match)
	self:SendCSUTAGuessReq(UTACfg.GuessReqType.GetInfo, match.match_id_t)
	self.data.req_guess_match = match
end

-- 某一个比赛的竞猜信息
function UniverseTeamArenaCtrl:OnSCUTAGuessMatchInfo(protocol)
	local info = protocol.info
	local guess_info = {match_id = info.match_id}
	local match = self.data.req_guess_match
	if match then
		-- 拿个队伍数据
		info.l_team_bet_info.team = match.atk_team
		info.r_team_bet_info.team = match.def_team
	end
	guess_info.l_bet_info = GuessBetInfoClass.New(info.l_team_bet_info)
	guess_info.r_bet_info = GuessBetInfoClass.New(info.r_team_bet_info)
	self.data:SetGuessInfo(guess_info)
	if not ViewMgr:IsOpen(UTAGuess) then
		ViewMgr:OpenView(UTAGuess)
	end
end

-- 玩家参与下注的列表
function UniverseTeamArenaCtrl:OnSCUTAGuessBetRecord(protocol)
	local list = protocol.record_list
	for i,v in pairs(list) do
		self.data:SetBetRecord(v)
	end
	self.data.guess_bet_data:Notify()
end

-- 竞猜累计收益
function UniverseTeamArenaCtrl:OnSCUTAGuessHistoryEarning(protocol)
	self.data.guedd_profit_data.num = protocol.guess_profit_num
end

-- 4972有可领竞猜奖励通知
function UniverseTeamArenaCtrl:OnSCUTAGuessHasBetRewardNotice(protocol)
	UniverseTeamArenaCtrl.Log("4972有可领竞猜奖励通知!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
	self.data.remind_data.can_guess_num = 1
end

-- 把一个冠军队伍的角色信息转成Appearance格式
function UniverseTeamArenaCtrl:ChampionFormatAppearance(info)
	local appearance = {
		prof_base = info.prof_base,
		advance_time = info.advance_time,
		avatar_type = info.avatar_type,
		color = info.color,
		weapon_id = info.weapon_id,
		sub_weapon_id = info.sub_weapon_id,
		avatar_id = info.headshot_id,	--头像id
		special_appearance_type = info.special_appearance_type,
		special_appearance_param = info.special_appearance_param,
		skin_id = 0,
		fashion_body_id = IllusionData.Instance:FashionIdToResId(info.wear_surface[1]) or 0,
		fashion_weapon_id = IllusionData.Instance:FashionIdToResId(info.wear_surface[2]) or 0,
		ride_id = 0,
		bicycle_index = info.bicycle_index,
        bicycle_model_type = info.bicycle_model_type,
        model_card_flag = 0,
        model_card_seq = 0,
        is_use_huanshou = 0,
        fly_flag = info.fly_flag
	}
	if info.wear_surface[4] > 0 then
		local cfg = IllusionData.Instance:GetFashionInfoById(info.wear_surface[4]) or {}
		appearance.ride_id = cfg.res_id or 0
	end
	appearance.fly_flag = appearance.fly_flag > FlyUpStageType.High and FlyUpStageType.High or appearance.fly_flag
	appearance.avatar_quality = appearance.fly_flag --角色头像品质
	return appearance
end

function UniverseTeamArenaCtrl:OpenUta()
	local status = self.data:ActState()
	-- if status == UTACfg.ActStatus.Ready or self.data.base_data.season_idx == 0 then
	-- 	PublicPopupCtrl.Instance:Center(Language.UTAPanel.Tip11)
	-- 	return
	-- else
	if status == UTACfg.ActStatus.Pause then
        ViewMgr:OpenView(UtaEndRank)
    else
        ViewMgr:OpenView(UniverseTeamArenaMain)
    end
end

-- 竞猜币不足弹获取途径
function UniverseTeamArenaCtrl:ShowGuessItem()
	local item_id = self.data:GetGuessItemId()
    MainOtherCtrl.Instance:GetWayViewById(item_id)
end


-- 天下第一的log打印
function UniverseTeamArenaCtrl.Log(...)
	if UTACfg.IS_LOG then
		LogError(...)
	end
end

-- 进入下一赛季
function UniverseTeamArenaCtrl.GMRestartSeason()
	GMCmdCtrl.Instance:SendGMCommand("uta_restart_season","")
end
-- 加入32个队伍
function UniverseTeamArenaCtrl.GMJoin32Team(param)
	param = param or {}
	local is_self = param[1]
	local num = (is_self == true or is_self == "1") and 1 or 0
	local str = "32 " .. num 
	GMCmdCtrl.Instance:SendGMCommand("fake_uta_teams",str)
end
-- 进行下一阶段
function UniverseTeamArenaCtrl.GMNextStatus()
	GMCmdCtrl.Instance:SendGMCommand("uta_next_status","")
end
-- 随机本轮比赛所有结果
function UniverseTeamArenaCtrl.GMMockBattles()
	GMCmdCtrl.Instance:SendGMCommand("uta_mock_battles","")
end
-- 进入下一轮资格赛
function UniverseTeamArenaCtrl.GMNextQualification()
	GMCmdCtrl.Instance:SendGMCommand("uta_next_qualification","")
end
-- 进入下一轮晋级赛
function UniverseTeamArenaCtrl.GMNextMain()
	GMCmdCtrl.Instance:SendGMCommand("uta_next_main","")
end
-- 开启当前轮次战斗
function UniverseTeamArenaCtrl.GMStartBattle()
	GMCmdCtrl.Instance:SendGMCommand("uta_start_battle","")
end
-- 进入战斗场景
function UniverseTeamArenaCtrl.GMUtaenterscene()
	GMCmdCtrl.Instance:SendGMCommand("utaenterscene","")
end
-- 立即进入战斗
function UniverseTeamArenaCtrl.GMFightNow()
	GMCmdCtrl.Instance:SendGMCommand("uta_fight_now","")
end
-- 开启log
function UniverseTeamArenaCtrl.OpenLog()
	UTACfg.IS_LOG = true
end
-- 关闭log
function UniverseTeamArenaCtrl.CloseLog()
	UTACfg.IS_LOG = false
end