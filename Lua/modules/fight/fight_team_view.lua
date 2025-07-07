
FightTeamView = FightTeamView or DeclareView("FightTeamView", FightConfig.ResPath .. "fight_team", Mod.Fight.Team)

VIEW_DECLARE_LEVEL(FightTeamView,ViewLevel.L1)

function FightTeamView:FightTeamView()
	self.data = FightCtrl.Instance:Data()
end

function FightTeamView:LoadCallback()
	self.Board:SetData2(self:GetType(), self.data:GetFightBoardTeamTitle(), self.data:GetFightBoardTeamTitle2(), function ()
		TeamCtrl.Instance:SendTeamEnterFbRet(false)
	end)
end

FightTeamViewOffset = FightTeamViewOffset or DeclareMono("FightTeamViewOffset", UIWFlushPanel)

function FightTeamViewOffset:FightTeamViewOffset()
	self.data = TeamCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data:GetEnterFBTeamInfoList(), func = self.onFlush, init_call = false},
	}
end

function FightTeamViewOffset:Awake()
	UIWFlushPanel.Awake(self)
	if not self.data:InTeam() then return end
	local team_info = self.data:GetTeamInfo()
	local member_list = team_info:MemberListSort()
	for i = 1, self.TeamItems:Length() do
		self.TeamItems[i]:SetData(member_list[i])
	end
	UH.SetText(self.LeaveTex, Language.PublicPopup.ReplyType.refuse)
	UH.SetText(self.ConfirmTex, Language.PublicPopup.ReplyType.confirm)

	local enter_fb_info = self.data:GetEnterFBInfo()
	local agree_enter_end_time = enter_fb_info.agree_enter_end_time
	if enter_fb_info.agree_enter_total_time > 0 then
        self.timer_ct = TimeHelper:AddCountDownCT(BindTool.Bind(self.TimerCTUpdate, self, enter_fb_info), BindTool.Bind(self.TimerCTComplete, self), agree_enter_end_time, 1)
	end
	if 1 == enter_fb_info.enter_type then
		if FB_TYPE.CRYSTAL_FB == enter_fb_info.enter_type_param then
			self.NotCanTip:SetActive(true)
			local tips = Language.Fight.FightTeamCrystalTips
			local dungeon = FightData.Instance.dungeon_seq_list[enter_fb_info.param1]
			if dungeon then
				if 2 == dungeon.scene_type_1 then
					self.NotCanTip:SetActive(false)
					local have_num = Item.GetNum(dungeon.expend_item)
					if 0 == have_num then
						tips = Language.Fight.FightTeamCrystalJYTips2
					else
						tips = string.format(Language.Fight.FightTeamCrystalJYTips3, Item.GetQuaName(dungeon.expend_item), have_num)
					end
				elseif 3 == dungeon.scene_type_1 then
					tips = 0 == FightData.Instance.fight_crystal_sweep.new_xiyou_play_times_remain and Language.Fight.FightTeamCrystalXYTips1
						or string.format(Language.Fight.FightTeamCrystalXYTips2, FightData.Instance.fight_crystal_sweep.new_xiyou_play_times_remain, FightData.Instance.fight_crystal_sweep.new_xiyou_play_times)
				end
			end
			UH.SetText(self.NotCanText, tips)
		elseif FB_TYPE.RUSH_DUNGEON == enter_fb_info.enter_type_param then
			local rush_team_info = RushDungeonData.Instance:TeamInfo()
			if rush_team_info ~= nil then
				self.HorLayout.padding = RectOffset.New(60,10,0,0)
				self.HorLayout.enabled = true
				self.RushObj:SetActive(true)
				for i = 1, self.TeamItems:Length() do
					self.TeamItems[i]:FlushLayer(rush_team_info[i].reached_level)
				end
				UH.SetText(self.LeaveTex, Language.Fight.BtnName[1])
				UH.SetText(self.ConfirmTex, Language.Fight.BtnName[2])
				UH.SetText(self.RushDesc, Language.RushDungeon.EnterTip)
			end
		elseif FB_TYPE.MIYI == enter_fb_info.enter_type_param then
			local num = ConspiracyData.Instance:GetOffList()[1].time - ConspiracyData.Instance:JoinTimes()
			if num < 0 then
				self.NotCanTip:SetActive(true)
				UH.SetText(self.NotCanText, Language.Fight.FightTeamConspTips)
			end
		elseif enter_fb_info.enter_type_param == FB_TYPE.GO_THROUGH then
			local boss_data = GoThroughData.Instance:GetEnterInfo(enter_fb_info)
			if boss_data ~= nil then
				self.BossItem:SetObjActive(true)
				self.BossItem:SetData(boss_data)
				UH.LocalPos(self.LineRect,Vector3.New(0,120,0))
				UH.LocalPos(self.ListRect,Vector3.New(0,55,0))
				UH.LocalPos(self.Countdown,Vector3.New(0,-60,0))
			end
		elseif FB_TYPE.LOVERS_MATCH == enter_fb_info.enter_type_param then
			local num = LoversMatchData.Instance:GetRemainTimes()
			if num <= 0 then
				self.NotCanTip:SetActive(true)
				UH.SetText(self.NotCanText, Language.LoversMatch.TeamError2)
			end
		end
	elseif 2 == enter_fb_info.enter_type then--万妖谷特殊处理 写死
		if 4 == enter_fb_info.enter_type_param then
			self.NotCanTip:SetActive(FightDenData.Instance:GetLeaderLevel() ~= nil)
			UH.SetText(self.NotCanText,Format(Language.Fight.FightTeamTip,NpcBehaviorBtn[2]))
		elseif 49 == enter_fb_info.enter_type_param then
			self.BossItem:SetObjActive(true)
			self.BossItem:SetData(enter_fb_info)
			UH.LocalPos(self.LineRect,Vector3.New(0,120,0))
			UH.LocalPos(self.ListRect,Vector3.New(0,55,0))
			UH.LocalPos(self.Countdown,Vector3.New(0,-60,0))
		elseif 65 == enter_fb_info.enter_type_param then
			self.NotCanTip:SetActive(false)
			--[[ if BigDipperData.Instance:BossIsPassBySn(enter_fb_info.param2) then
				self.NotCanTip:SetActive(true)
				local tips = Language.Fight.FightTeamCrystalXYTips1
				UH.SetText(self.NotCanText, tips)
			end ]]
		end
	elseif 3 == enter_fb_info.enter_type then
		if enter_fb_info.param3 > 0 then
			self.KnightItem:SetData({value = enter_fb_info.param3})
		end
        if BATTLE_MODE.BATTLE_MODE_TEAM_CHALLENGE_TASK == enter_fb_info.enter_type_param then
            self.BossItem:SetObjActive(true)
            local boss_info = ChallengeTaskData.Instance:GetEnterInfo(enter_fb_info.param1)
            self.BossItem:SetData(boss_info)
            UH.LocalPos(self.LineRect, Vector3.New(0, 120, 0))
            UH.LocalPos(self.ListRect, Vector3.New(0, 55, 0))
            UH.LocalPos(self.Countdown, Vector3.New(0, -60, 0))
        end
	end
end

function FightTeamViewOffset:TimerCTUpdate(enter_fb_info)
	if nil == enter_fb_info.agree_enter_end_time then return end
	local last_time = math.floor(enter_fb_info.agree_enter_end_time - TimeHelper.GetServerTime())
	self.CountdownProgress:SetProgress(last_time / enter_fb_info.agree_enter_total_time)
	self.CountdownNum.text = last_time .. Language.Team.Second
end

function FightTeamViewOffset:TimerCTComplete()
	ViewMgr:CloseView(FightTeamView)
end

function FightTeamViewOffset:onFlush()
	local enter_fb_team_info_list = self.data:GetEnterFBTeamInfoList()
	if enter_fb_team_info_list.is_refuse then
		ViewMgr:CloseView(FightTeamView)
		return
	end
	for i = 1, self.TeamItems:Length() do
		self.TeamItems[i]:FlushReady(enter_fb_team_info_list)
	end
	local member_index = self.data:MemberIndex()
	self.BtnConfirmInter.Interactable = not (enter_fb_team_info_list[member_index] and TeamConfig.FbInfoTeamType.confirm == enter_fb_team_info_list[member_index])
end

function FightTeamViewOffset:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_ct)
end

function FightTeamViewOffset:OnClickLeave()
	TeamCtrl.Instance:SendTeamEnterFbRet(false)
end

function FightTeamViewOffset:OnClickConfirm()
	TeamCtrl.Instance:SendTeamEnterFbRet(true)
	--万妖谷特殊处理 写死
	local enter_fb_info = self.data:GetEnterFBInfo()
	if enter_fb_info.enter_type == 2 and enter_fb_info.enter_type_param == 4 then
        FightDenData.Instance:SetBattleMapId(FightDenData.Instance:GetLeaderLevel())
	end 
end

FightTeamViewOffsetItem = DeclareMono("FightTeamViewOffsetItem", UIWidgetBaseItem)

function FightTeamViewOffsetItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local info = data and data.info or nil
	if nil == info or 1 == info.is_partner then
		self.ItemObj:SetActive(false)
	else
		self.ItemObj:SetActive(true)
		self.Name.alignment = string.len(info.name) > 15 and 0 or 1
		self.Name.text = info.name
		self.Level.text = info.level
		if info.appearance.special_appearance_type == 1 then
			local config = PartnerData.Instance:GetPartnerInfoById(info.appearance.special_appearance_param)
			UH.SetIcon(self.IconSp, config.info.icon_id, ICON_TYPE.ITEM)
		else
			UH.SetAvatar(self.IconSp, info.appearance.avatar_type,info.appearance.avatar_id,info.appearance.avatar_quality)
		end
	end
end

function FightTeamViewOffsetItem:FlushLayer(params)
	if params == nil then
		UH.SetText(self.Layer, "")
	else
		UH.SetText(self.Layer, string.format(Language.RushDungeon.Layer, params))
	end
end

function FightTeamViewOffsetItem:FlushReady(enter_fb_team_info_list)
	if nil == self.data then return end
	local info = self.data.info
	if nil == info then return end
	self.ReadyObj:SetActive(enter_fb_team_info_list[info.member_index] and TeamConfig.FbInfoTeamType.confirm == enter_fb_team_info_list[info.member_index])
end

FightTeamViewKnightItem = DeclareMono("FightTeamViewKnightItem", UIWidgetBaseItem)

function FightTeamViewKnightItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.KnightNum.text = data.value
	self.LinePos.localPosition = Vector3.New(0,-40,0)
	self.ListPos.localPosition = Vector3.New(0,130,0)
	self.CountDownPos.localPosition = Vector3.New(0,-70,0)
	self.ItemObj:SetActive(true)
end

----------------------------FightTeamBossItem----------------------------
FightTeamBossItem = FightTeamBossItem or DeclareMono("FightTeamBossItem", UIWidgetBaseItem)
function FightTeamBossItem:SetData(data)
    local icon
    local name
    if data.icon then
        icon = data.icon
        name = data.name
    else
        local config = Cfg.NpcById(data.npc_id or data.param1)
        name = data.name or (config.name .. FightFieldBossData.Instance:GetHillSeaName(data.param1))
        icon = config.npc_icon
    end
    UH.SetIcon(self.Icon, icon)
    UH.SetText(self.Name, name)
end

FightTeamMusicGameView = FightTeamMusicGameView or DeclareView("FightTeamMusicGameView", FightConfig.ResPath .. "fight_team_music_game")

VIEW_DECLARE_LEVEL(FightTeamMusicGameView,ViewLevel.L1)

function FightTeamMusicGameView:FightTeamMusicGameView()
	self.data = FightCtrl.Instance:Data()
end

function FightTeamMusicGameView:LoadCallback()
	self.Board:SetData2(self:GetType(), self.data:GetFightBoardTeamTitle(), self.data:GetFightBoardTeamTitle2(), function ()
		TeamCtrl.Instance:SendTeamEnterFbRet(false)
	end)
end


FightTeamMusicGameViewF = FightTeamMusicGameViewF or DeclareMono("FightTeamMusicGameViewF", UIWFlushPanel)

function FightTeamMusicGameViewF:FightTeamMusicGameViewF()
	self.data = TeamCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data:GetEnterFBTeamInfoList(), func = self.onFlush, init_call = false},
	}
end

function FightTeamMusicGameViewF:Awake()
	UIWFlushPanel.Awake(self)
	if not self.data:InTeam() then return end
	local team_info = self.data:GetTeamInfo()
	local member_list = team_info:MemberListSort()
	for i = 1, self.TeamItems:Length() do
		self.TeamItems[i]:SetData(member_list[i])
	end
	UH.SetText(self.LeaveTex, Language.PublicPopup.ReplyType.leave)
	UH.SetText(self.ConfirmTex, Language.PublicPopup.ReplyType.confirm)

	self.DescShow1.text = Language.MusicGame.FightTeamDesc1
	self.DescShow2.text = Language.MusicGame.FightTeamDesc2

	local enter_fb_info = self.data:GetEnterFBInfo()
	local agree_enter_end_time = enter_fb_info.agree_enter_end_time
	if enter_fb_info.agree_enter_total_time > 0 then
        self.timer_ct = TimeHelper:AddCountDownCT(BindTool.Bind(self.TimerCTUpdate, self, enter_fb_info), BindTool.Bind(self.TimerCTComplete, self), agree_enter_end_time, 1)
	end
end

function FightTeamMusicGameViewF:TimerCTUpdate(enter_fb_info)
	if nil == enter_fb_info.agree_enter_end_time then return end
	local last_time = math.floor(enter_fb_info.agree_enter_end_time - TimeHelper.GetServerTime())
	self.CountdownProgress:SetProgress(last_time / enter_fb_info.agree_enter_total_time)
	self.CountdownNum.text = last_time .. Language.Team.Second
end

function FightTeamMusicGameViewF:TimerCTComplete()
	ViewMgr:CloseView(FightTeamMusicGameView)
end

function FightTeamMusicGameViewF:onFlush()
	local enter_fb_team_info_list = self.data:GetEnterFBTeamInfoList()
	if enter_fb_team_info_list.is_refuse then
		ViewMgr:CloseView(FightTeamMusicGameView)
		return
	end
	for i = 1, self.TeamItems:Length() do
		self.TeamItems[i]:FlushReady(enter_fb_team_info_list)
	end
	local member_index = self.data:MemberIndex()
	self.BtnConfirmInter.Interactable = not (enter_fb_team_info_list[member_index] and TeamConfig.FbInfoTeamType.confirm == enter_fb_team_info_list[member_index])
end

function FightTeamMusicGameViewF:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_ct)
end

function FightTeamMusicGameViewF:OnClickLeave()
	TeamCtrl.Instance:SendTeamEnterFbRet(false)
end

function FightTeamMusicGameViewF:OnClickConfirm()
	TeamCtrl.Instance:SendTeamEnterFbRet(true)
end

FightTeamHarmonyHeavenView = FightTeamHarmonyHeavenView or DeclareView("FightTeamHarmonyHeavenView", FightConfig.ResPath .. "fight_team_harmony_heaven")

VIEW_DECLARE_LEVEL(FightTeamHarmonyHeavenView,ViewLevel.L1)

function FightTeamHarmonyHeavenView:FightTeamHarmonyHeavenView()
	self.data = FightCtrl.Instance:Data()
end

function FightTeamHarmonyHeavenView:LoadCallback()
	self.Board:SetData4(self:GetType(), self.data:GetFightBoardTeamTitle(), Vector2(831,470))

	ViewMgr:CloseView(HarmonyHeavenView)
end


FightTeamHarmonyHeavenViewF = FightTeamHarmonyHeavenViewF or DeclareMono("FightTeamHarmonyHeavenViewF", UIWFlushPanel)

function FightTeamHarmonyHeavenViewF:FightTeamHarmonyHeavenViewF()
	self.data = TeamCtrl.Instance:Data()
	self.hh_data = HarmonyHeavenCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data:GetEnterFBTeamInfoList(), func = self.onFlush, init_call = false},
	}
	self.language = Language.HarmonyHeaven.FightTeam
end

function FightTeamHarmonyHeavenViewF:Awake()
	UIWFlushPanel.Awake(self)
	if not self.data:InTeam() then return end
	local team_info = self.data:GetTeamInfo()
	local member_list = team_info:MemberListSort()
	for i = 1, self.TeamItems:Length() do
		self.TeamItems[i]:SetData(member_list[i])
	end
	UH.SetText(self.LeaveTex, Language.PublicPopup.ReplyType.leave)
	UH.SetText(self.ConfirmTex, Language.PublicPopup.ReplyType.confirm)

	local enter_fb_info = self.data:GetEnterFBInfo()
	local agree_enter_end_time = enter_fb_info.agree_enter_end_time
	if enter_fb_info.agree_enter_total_time > 0 then
        self.timer_ct = TimeHelper:AddCountDownCT(BindTool.Bind(self.TimerCTUpdate, self, enter_fb_info), BindTool.Bind(self.TimerCTComplete, self), agree_enter_end_time, 1)
	end
	local info = Cfg.PromevalParadiseBossFbTypeSeq(enter_fb_info.param1, enter_fb_info.param2)
	if info then
		self.NameShow.text = info.fb_name
		self.TypeShow.text = self.language.TypeShow[info.fb_type]
	end
	local role_info = self.hh_data.fb_info.role_info
	if not role_info.init then
		self.CountShow.text = string.format(self.language.CountShow, COLORSTR.Green4, role_info.rewards_times_max, self.hh_data:ConfigOtherBossRewardLimit())
	end
end

function FightTeamHarmonyHeavenViewF:TimerCTUpdate(enter_fb_info)
	if nil == enter_fb_info.agree_enter_end_time then return end
	local last_time = math.floor(enter_fb_info.agree_enter_end_time - TimeHelper.GetServerTime())
	self.CountdownProgress:SetProgress(last_time / enter_fb_info.agree_enter_total_time)
	self.CountdownNum.text = last_time .. Language.Team.Second
end

function FightTeamHarmonyHeavenViewF:TimerCTComplete()
	ViewMgr:CloseView(FightTeamHarmonyHeavenView)
end

function FightTeamHarmonyHeavenViewF:onFlush()
	local enter_fb_team_info_list = self.data:GetEnterFBTeamInfoList()
	if enter_fb_team_info_list.is_refuse then
		ViewMgr:CloseView(FightTeamHarmonyHeavenView)
		return
	end
	for i = 1, self.TeamItems:Length() do
		self.TeamItems[i]:FlushReady(enter_fb_team_info_list)
	end
	local member_index = self.data:MemberIndex()
	self.BtnConfirmInter.Interactable = not (enter_fb_team_info_list[member_index] and TeamConfig.FbInfoTeamType.confirm == enter_fb_team_info_list[member_index])
end

function FightTeamHarmonyHeavenViewF:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.timer_ct)

	self.hh_data.need_play_story = true
end

function FightTeamHarmonyHeavenViewF:OnClickLeave()
	TeamCtrl.Instance:SendTeamEnterFbRet(false)
end

function FightTeamHarmonyHeavenViewF:OnClickConfirm()
	TeamCtrl.Instance:SendTeamEnterFbRet(true)
end

