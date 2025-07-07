--定义主场景玩法模式的执行行为
--OnBegin调用时机：切换到玩法场景时
--OnEnd 切换成其它玩法模式时,一般用于对OnBegin所做逻辑的还原

--key:scene_auto-scene_type
--value:ModeTab
SceneModeNormal = {}
function SceneModeNormal.OnBegin()
	-- MainViewData.Instance:ShowRbButton()
end

function SceneModeNormal.OnSceneLoaded()
	--[[ SceneModeNormal.CheckMainLine() ]]
	if DuelData.Instance:IsOpen() then
		local scene_id = SceneData.Instance:SceneId()
		if DuelData.Instance:IsDuelScene() and DuelData.Instance:GetLastSceneId() ~= scene_id then
			PublicPopupCtrl.Instance:Center(Language.Duel.EnterDuelScene)
		end
		DuelData.Instance:SetLastSceneId(scene_id)
	end
	SceneModeNormal.CheckBountyTask()
end

function SceneModeNormal.CheckMainLine()
	local list = TaskData.Instance:AcceptedTask()
	local scene_id = SceneData.Instance:SceneId()
	GuideManager.Instance:DisableArrowGuide()
	if list[MAIN_LINE_SP_TASK] == 1 and scene_id == MAIN_LINE_SP_MAP then
		local main = TaskData.Instance:GetMainlineAccepted()
		if main and not StringIsEmpty(main.info.tasks_dialog_beh) then
			local params = string.split(main.info.tasks_dialog_beh, "|")
			local npc_seq = tonumber(params[5]) or 0
			local vo = VoMgr:NpcVoBySeq(npc_seq)
			if vo then
				GuideManager.Instance:DisableArrowGuide()
				local y = SceneCtrl.Instance:CurScene():GetPosHeight(vo.x,vo.y)
				GuideManager.Instance:EnableArrowGuide(Vector3.New(vo.x,y,vo.y))
			end
		end
	end
end

function SceneModeNormal.CheckBountyTask()
	local list = TaskData.Instance:AcceptedTask()  
	local scene_id = SceneData.Instance:SceneId()
	if list[56270] == 1 and scene_id == Config.bounty_tasklist_auto.other[1].array_scene then
		BountyTaskData.Instance:PlayEffectArray(false)
	end
end

function SceneModeNormal.OnEnd()
end

--勇气的挑战
SceneModeCourageChallenge = {}
function SceneModeCourageChallenge.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.courage_challenge)
end

function SceneModeCourageChallenge.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeConspiracy = {}
function SceneModeConspiracy.OnBegin()
	ChatData.Instance:SetShowBack(true)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.conspiracy)
	-- ViewMgr:OpenView(ConspiracyWeekFirstKill)
end

function SceneModeConspiracy.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ChatData.Instance:SetShowBack(false)
end

SceneModeFightCrystal = {}
function SceneModeFightCrystal.OnBegin()
	ChatData.Instance:SetShowBack(true)
	if not MainViewCtrl.Instance:IsInGameState(MainViewGameState.fight_crystal) then
		TeamData.Instance:MenuArrow(0)
	end
	MainViewCtrl.Instance:SetGameState(MainViewGameState.fight_crystal)
	MainViewData.Instance:SetHideButton()
	if ViewMgr:IsOpen(FightRareDungeonView) then
		ViewMgr:CloseView(FightRareDungeonView)
	end
end

function SceneModeFightCrystal.OnSceneLoaded()
	--显示指引箭头
	local crystal_inifo = FightData.Instance:GetFightCrystalInfo()
	if crystal_inifo.scene_id ~= SceneMgr:LoadedSceneId() then
		GuideManager.Instance:DisableArrowGuide()
		return
	end
	local guide_cfg = Config.function_guide_auto.crystal_scene_guide
	local need_guide = false
	for _,v in pairs(guide_cfg) do
		if v.arrows_seq == crystal_inifo.level_seq then
			need_guide = true
			break
		end
	end
	if need_guide then
		local seq_cfg = FightData.Instance:GetFightCrystalConfigBySeq(crystal_inifo.level_seq)
		local objs = FightData.Instance:GetCrystalFbObjInfo()
		for key, val in pairs(objs) do
			if val.obj_type == FightConfig.CrystalObjType.Door or val.obj_type == FightConfig.CrystalObjType.Boss then--指引到BOSS或者门
				local door_pos = SceneCtrl.Instance:CurScene():PosToClint(val.pos_x, val.pos_y)
				GuideManager.Instance:DisableArrowGuide()
				GuideManager.Instance:EnableArrowGuide(door_pos)
				break
			end
		end				
	else
		GuideManager.Instance:DisableArrowGuide()
	end
end

function SceneModeFightCrystal.OnEnd()
	GuideManager.Instance:DisableArrowGuide()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeArenaMatch = {}
function SceneModeArenaMatch.OnBegin()
	ChatData.Instance:SetShowBack(true)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.arena_match)
end

function SceneModeArenaMatch.OnEnd()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

function SceneModeArenaMatch.OnSceneLoaded()
	--ArenaMatchCtrl.Instance:SendPlatformBattleRankFirstInfoReqSceneId(SceneData.Instance:SceneId())
	ArenaMatchCtrl.Instance:SendPlatformBattleRankFirstInfoReq(Config.leitai_cfg_auto.leitai)
end

SceneModeAnimalRace = {}
function SceneModeAnimalRace.OnBegin()
	MainPlayer:SetVisible(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.animal_race)
	ViewMgr:MainViewVisible(false)
	ViewMgr:OpenView(AnimalRaceView)
	ResMgr:LoadGameObject(DrawerHelper.GetStoryPath("animal_race"), function (obj)
		AnimalRaceData.Instance:LoadShotsObj(obj)
	end, true)
end

function SceneModeAnimalRace.OnEnd()
	MainPlayer:SetVisible(true)
	AnimalRaceData.Instance:ClearScene()
	ViewMgr:CloseView(AnimalRaceView)
	ViewMgr:MainViewVisible(true)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeShiTuFB = {}
function SceneModeShiTuFB.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.shitu_fb)
	PublicPopupCtrl.Instance:OpenWarningView(nil, {
		desc = Config.language_cfg_auto.textdesc[83].desc,
		warning = "",
	},{height = 280})
end

function SceneModeShiTuFB.OnEnd()
	ViewMgr:CloseView(PublicWarningView)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeDuChuangZeiYing = {}
function SceneModeDuChuangZeiYing.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.du_chuang_zei_ying)
	ChatData.Instance:SetShowBack(true)
	MainViewData.Instance:SetHideButton()
	DuChuangZeiYingData.Instance:PrepareScene()
end

function SceneModeDuChuangZeiYing.OnEnd()
	ChatData.Instance:SetShowBack(false)
	DuChuangZeiYingData.Instance:ClearScene()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeWenXiangLou = {}
function SceneModeWenXiangLou.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.wen_xiang_lou)
	ChatData.Instance:SetShowBack(true)
	MainViewData.Instance:SetHideButton()
end

function SceneModeWenXiangLou.OnEnd()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	WenXiangLouData.Instance:ResetDetailInfo()
end

SceneModeArrayGhost = {}
function SceneModeArrayGhost.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.array_ghost)
	ChatData.Instance:SetShowBack(true)
	MainViewData.Instance:SetHideButton()
end

function SceneModeArrayGhost.OnEnd()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	if not ArrayGhostData.Instance.end_info.is_succ then
		AnecdoteCtrl.Instance:StrangerTaskFailTalk(AnecdoteType.ArrayGhost)
	end
end

SceneModePetGod = {}
function SceneModePetGod.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.pet_god)
	ChatData.Instance:SetShowBack(true)
	MainViewData.Instance:SetHideButton()
end

function SceneModePetGod.OnEnd()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ViewMgr:OpenViewByKey(Mod.FightFB.PetGod)
end

SceneModeGuildFB = {}
function SceneModeGuildFB.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.guild_fb)
end

function SceneModeGuildFB.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeMazeRun = {}
function SceneModeMazeRun.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.maze_run)
end

function SceneModeMazeRun.OnSceneLoaded()
	TimeHelper:CancelTimer(SceneModeMazeRun.handle_time)
	--倒计时1分30秒后显示
	GuideManager.Instance:DisableArrowGuide()
	local info = MazeRunData.Instance:DoorInfo()
	if info and TeamData.Instance:IsLeader() then
		SceneModeMazeRun.handle_time = TimeHelper:AddCountDownCT(function ()
		end, function ()
				local door_pos = SceneCtrl.Instance:CurScene():PosToClint(info.x, info.y)
				GuideManager.Instance:DisableArrowGuide()
				GuideManager.Instance:EnableArrowGuide(door_pos)
		end, info.time + 90)
	end
end

function SceneModeMazeRun.OnEnd()
	GuideManager.Instance:DisableArrowGuide()
	TimeHelper:CancelTimer(SceneModeMazeRun.handle_time)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeMazeRunWait = {}
function SceneModeMazeRunWait.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.maze_run)
	PublicPopupCtrl.Instance:OpenWarningView(ActType.MzeRun)
end

function SceneModeMazeRunWait.OnEnd()
	PublicPopupCtrl.Instance:CloseWarningView()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeFightGather = {}
function SceneModeFightGather.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.fight_gather)
	MainViewData.Instance:SetHideButton()
	local scene_id = RoleData.Instance:GetBaseData().scene_id
	local skill_type = LiveData.Instance:GetSkillBySceneId(scene_id)
	local desc = LiveData.Instance:GetSceneAllRewardDesc(scene_id)
	local title = Language.Live.SkillSceneTitle[skill_type] or ""
	local data  = {title = title, desc = desc}
	PublicPopupCtrl.PopupLifeMsg(data)
end

function SceneModeFightGather.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	MainOtherCtrl.Instance:CloseGather()
	FightCtrl.Instance:Data():ClearSceneGathers()
end

SceneModeTeamSport = {}
function SceneModeTeamSport.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.team_sport)
	PublicPopupCtrl.Instance:OpenWarningView(ActType.TeamSport,nil,{pos = Vector3.New(-30,70,0)})
	MainViewData.Instance:SetHideButton()
end

function SceneModeTeamSport.OnEnd()
	PublicPopupCtrl.Instance:CloseWarningView()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ViewMgr:CloseView(TeamSportView)
end
SceneModeRushDungeon = {}

function SceneModeRushDungeon.OnBegin()
	RushDungeonCtrl.Instance:SendBraveGroundReq()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.rush_dungeon)
	ViewMgr:OpenView(RushDungeonName)
	MainViewData.Instance:SetHideButton()
end

function SceneModeRushDungeon.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeGuildMelee = {}
function SceneModeGuildMelee.OnBegin()
	PublicPopupCtrl.Instance:OpenWarningView(ActType.GuildMelee,nil,{pos = Vector3.New(30,25,0)})
	MainViewCtrl.Instance:SetGameState(MainViewGameState.guild_melee)
	MainViewData.Instance:SetHideButton()
end

function SceneModeGuildMelee.OnEnd()
	PublicPopupCtrl.Instance:CloseWarningView()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeChiefPrimary = {}
function SceneModeChiefPrimary.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.chief_primary)
	PublicPopupCtrl.Instance:OpenWarningView(ActType.ChiefElection,nil,{pos = Vector3.New(15,25,0),height = 280})
	MainViewData.Instance:SetHideButton()
	ChatData.Instance:SetShowBack(true)
end

function SceneModeChiefPrimary.OnEnd()
	PublicPopupCtrl.Instance:CloseWarningView()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	if ChiefElectionData.Instance:GetMatchState() ~= 0 then
		ChiefElectionData.Instance:SetLeaveFlag(true)
	end
	ChatData.Instance:SetShowBack(false)
end

SceneModeChiefElection = {}
function SceneModeChiefElection.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.chief_elelction)
	ChatData.Instance:SetShowBack(true)
end

function SceneModeChiefElection.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ChiefElectionData.Instance:SetLeaveFlag(true)
	ChatData.Instance:SetShowBack(false)
end

SceneModeChiefPeakBattle = {}
function SceneModeChiefPeakBattle.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.chief_peak_battle)
	ChatData.Instance:SetShowBack(true)
end

function SceneModeChiefPeakBattle.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ChiefElectionData.Instance:SetLeaveFlag(true)
	ChatData.Instance:SetShowBack(false)
end

SceneModeAdvanced = {}
function SceneModeAdvanced.OnBegin()
	ChatData.Instance:SetShowBack(true)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.advanced)
	MainViewData.Instance:SetHideButton()
end

function SceneModeAdvanced.OnEnd()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	TimeHelper:CancelTimer(SceneModeAdvanced.handle_time)
	GuideManager.Instance:DisableArrowGuide()
end

function SceneModeAdvanced.OnSceneLoaded()
	TimeHelper:CancelTimer(SceneModeAdvanced.handle_time)
	GuideManager.Instance:DisableArrowGuide()
	SceneModeAdvanced.handle_time = TimeHelper:AddDelayTimer(function()
		if MainViewData.Instance:IsBattle() then
			return
		end
		for k, v in pairs(FightData.Instance:GetAdvanceFbObjInfo()) do
			if v.obj_type == 1 then
				local door_pos = SceneCtrl.Instance:CurScene():PosToClint(v.pos_x, v.pos_y)
				GuideManager.Instance:DisableArrowGuide()
				GuideManager.Instance:EnableArrowGuide(door_pos)
				break
			end
		end
	end, 60)
end

SceneModeGuildEmbattle = {}
function SceneModeGuildEmbattle.OnBegin()
	PublicPopupCtrl.Instance:OpenWarningView(ActType.GuildEmbattle)
	ChatData.Instance:SetShowBack(true)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.guild_embattle)
	MainViewData.Instance:SetHideButton()
end

function SceneModeGuildEmbattle.OnEnd()
	PublicPopupCtrl.Instance:CloseWarningView()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeGuildAnswer = {}
function SceneModeGuildAnswer.OnBegin()
	local status = ActivityData.Instance:GetActivityStatus(ActType.GuildAnswer)
    if status == ActStatusType.Open then
		ViewMgr:OpenView(GuildQuestion)
	end
	ViewMgr:CloseView(GuildView)
	ViewMgr:OpenView(GuideTips, {desc = Language.Task.GuideDesc.Answer})
	-- SceneCtrl.Instance:Objs():SetTouchAbleAboutAnswer()
	ChatData.Instance:SetShowBack(true)
end

function SceneModeGuildAnswer.OnSceneLoaded()
	TimeHelper:AddDelayTimer(BindTool.Bind(SceneModeGuildAnswer.PlayEffect, SceneModeGuildAnswer), 1)
end

function SceneModeGuildAnswer:PlayEffect()
	SceneModeGuildAnswer.effect_list = {}
	SE:PlayA(SceneEffectConfig.EffectList.GuildAnswer1,function (cfg,obj)
		table.insert(SceneModeGuildAnswer.effect_list,obj)
	end)
	SE:PlayA(SceneEffectConfig.EffectList.GuildAnswer2,function (cfg,obj)
		table.insert(SceneModeGuildAnswer.effect_list,obj)
	end)
	SE:PlayA(SceneEffectConfig.EffectList.GuildAnswer3,function (cfg,obj)
		table.insert(SceneModeGuildAnswer.effect_list,obj)
	end)
	SE:PlayA(SceneEffectConfig.EffectList.GuildAnswer4,function (cfg,obj)
		table.insert(SceneModeGuildAnswer.effect_list,obj)
	end)
end

function SceneModeGuildAnswer.OnEnd()
	ViewMgr:CloseView(GuildQuestion)
	ChatData.Instance:SetShowBack(false)
	-- SceneCtrl.Instance:Objs():SetTouchAbleAboutAnswer()
	for key, value in pairs(SceneModeGuildAnswer.effect_list) do
		SE:Stop(value)
	end
end

SceneModeGuildHome = {}
function SceneModeGuildHome.OnBegin()
	ChatData.Instance:SetShowBack(true)
end
function SceneModeGuildHome.OnEnd()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeMedalFB = {}
function SceneModeMedalFB.OnBegin()
	ChatData.Instance:SetShowBack(true)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.madel_fb)
end 

function SceneModeMedalFB.OnSceneLoaded()
end

function SceneModeMedalFB.OnEnd()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end 

SceneModeFightFantasy = {}
function SceneModeFightFantasy.OnBegin()
	ChatData.Instance:SetShowBack(true)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.fight_fantasy)
	
	FightCtrl.Instance:FightFantasyAuto()
	if TaskData.Instance.task_list[CUSTOM_OBJID.FantasyFB] then
		TaskData.Instance:TraceTask(TaskConfig.TaskType.dragon, CUSTOM_OBJID.FantasyFB)
	end
end 

function SceneModeFightFantasy.OnSceneLoaded()
	local Fantasy_info = FightData.Instance:GetFightFantasyInfo()
end

function SceneModeFightFantasy.OnEnd()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	TimeHelper:AddDelayTimer(function ()
		local dragon = TeamCtrl.Instance:Dragon()
		if dragon then
			local count = FightData.Instance:GetTodayFantasyFetch()
			if count >= 1 and count < 4 then
				--[[ if DEBUG_MODE then
					LogError("太菜了 没打赢算了算了")
				end ]]
			else
				dragon:Start()
			end
		else
			TaskData.Instance:ClearDragon()
			--[[ if DEBUG_MODE then
				LogError("没有一条龙服务2222222222")
			end ]]
		end
		--[[ if dragon and dragon:Step() > 2 and dragon:TeamType() == 10 then
			local act_info = ActivityData.Instance:GetActivityInfo(ActType.FightDen)
			local limit = act_info:LevelRestrict()
			if limit > RoleData.Instance:GetRoleLevel() then
				PublicPopupCtrl.Instance:Center(Language.Common.NotEnoughLevel)
				return
			end
			local pass_wave = FightDenData.Instance:PassWave()
			local max_wave = FightDenData.Instance:GetMaxWave()
			if pass_wave < max_wave then
				dragon:Step(7)
				TaskData.Instance:TraceTask(TaskConfig.TaskType.dragon, CUSTOM_OBJID.FightDen)
				BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(820))
			end
		end ]]
	end, 1)
end 

SceneModeOnlyFight = {}
function SceneModeOnlyFight.OnBegin()
	ChatData.Instance:SetShowBack(true)
	PublicPopupCtrl.Instance:OpenWarningView(ActType.OnlyFight,nil,{pos = Vector3.New(30,25,0)})
	OnlyFightCtrl.ActivityOnlyFightOperatorReq(0)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.only_fight)
	SceneCtrl.Instance:Objs():ShieldingPet(true)
	MainViewData.Instance:SetHideButton()
end

function SceneModeOnlyFight.OnEnd()
	PublicPopupCtrl.Instance:CloseWarningView()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	SceneCtrl.Instance:Objs():ShieldingPet(false)
end

SceneModeFoxCatchFB = {}
function SceneModeFoxCatchFB.OnBegin()
	ChatData.Instance:SetShowBack(true)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.fox_catch)
	MainViewData.Instance:SetHideButton()
	if CatchFoxData.SendTaskFinishReq == nil then
		ViewMgr:OpenView(GuideFox)
	end
	ViewMgr:OpenView(FoxCompassView)
end 

function SceneModeFoxCatchFB.OnEnd()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)

	ViewMgr:CloseView(FoxCompassView)
	CatchFoxData.Instance:ClearCatched()
end 

SceneModeStarLifeFB = {}
function SceneModeStarLifeFB.OnBegin()
	ViewMgr:MainViewVisible(false)
	ViewMgr:OpenView(SmxjSceneView)
	SceneCtrl.Instance:Objs():ShieldingPet(true)
	MainPlayer:ShieldRide(true)
end

function SceneModeStarLifeFB.OnEnd()
	ViewMgr:CloseView(SmxjSceneView)
	ViewMgr:MainViewVisible(true)
	SceneCtrl.Instance:Objs():ShieldingPet(false)
	if MainPlayer ~= nil then
		MainPlayer:ShieldRide(false)
	end
end 

SceneModeDreamNotes = {}
function SceneModeDreamNotes.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.dream_notes)
	MainViewData.Instance:SetHideButton()
	ChatData.Instance:SetShowBack(true)
end

function SceneModeDreamNotes:OnSceneLoaded()
	if SceneModeDreamNotes.IsOnceDreamNotesScene == nil then
		Invoke(function()
			MainOtherCtrl.Instance:WaitViewFunc()
		end,2)
		SceneModeDreamNotes.IsOnceDreamNotesScene = true
	end
end

function SceneModeDreamNotes.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	SceneModeDreamNotes.IsOnceDreamNotesScene = nil
	ChatData.Instance:SetShowBack(false)
	MainOtherCtrl.Instance:SetWaitViewFunc(nil)
end

SceneLuckyMazeFB = {}
function SceneLuckyMazeFB.OnBegin()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.LoginStartClick)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.lucky_maze)
	MainViewData.Instance:SetHideButton()
    ChatData.Instance:SetShowBack(true)
end

function SceneLuckyMazeFB.OnEnd()
	ViewMgr:OpenView(LuckyMazeView)
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	-- 设置一个没有通关的拼图
    LuckyMazeData.Instance:SetNotPassId()
end


SceneRabbitRaceFB = {}
function SceneRabbitRaceFB.OnBegin()
	GuideManager.Instance:RevertYinYangEye(true)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.rabbit_race)
	MainViewData.Instance:SetHideButton()
	ChatData.Instance:SetShowBack(true)
	RabbitRaceData.Instance:ResetMain()
	if not RabbitRaceData.Instance.is_main then
		TeamCtrl.Instance:SendTemporarilyPartReq()
	end
	if MainPlayer ~= nil then
		MainPlayer:ShieldRide(true)
	end
end

function SceneRabbitRaceFB.OnSceneLoaded()
	TimeHelper:CancelTimer(SceneRabbitRaceFB.time_ht)
	SceneRabbitRaceFB.time_ht = TimeHelper:AddRunTimer(function()
		for obj_id,v in pairs(RabbitRaceData.Instance.obj_list or {}) do
			local obj = SceneCtrl.Instance:Objs():GetObjectByObjId(obj_id)
			if obj and obj.vo.is_rest then
				local npc_id = obj.vo.npc_id
				if npc_id and RabbitRaceData.Instance:IsSelfRabbitNpc(npc_id) then
					if MainPlayer then
						local dis = MainPlayer:GetDistance(obj:GetPos())
						if dis <= 5 and v == 0 and not RabbitRaceData.Instance.oper_record then
							if dis ~= SceneRabbitRaceFB.main_dis then
								SceneRabbitRaceFB.main_dis = dis
								RabbitRaceData.Instance.oper_obj_id = obj_id
								ViewMgr:OpenView(RabbitRaceOperView)
							end
						elseif obj_id == RabbitRaceData.Instance.oper_obj_id or RabbitRaceData.Instance.oper_record ~= nil then
							if ViewMgr:IsOpen(RabbitRaceOperView) then
								ViewMgr:CloseView(RabbitRaceOperView)
							end
						end
					end
				end
			end
			if obj then
				-- 检查里兔子终点的距离
				if obj.CheckDes then
					obj:CheckDes()
				end
			end
		end
    end, 0.1)
end

function SceneRabbitRaceFB.OnEnd()
	-- GuideManager.Instance:ShowYinYangEye(true)
	TimeHelper:CancelTimer(SceneRabbitRaceFB.time_ht)
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	if MainPlayer ~= nil then
		MainPlayer:ShieldRide(false)
	end
	if not RabbitRaceData.Instance.is_main then
		--TeamCtrl.Instance:SendTemporarilyPartReq()
	end
end



-- 我要上学
SceneModeGotoSchool = {}
function SceneModeGotoSchool.OnBegin()
	GuideManager.Instance:RevertYinYangEye(true)
	LogDG("SceneModeGotoSchool.OnBegin()")
	GotoSchoolLogic.Instance:Ready()
end

function SceneModeGotoSchool.OnSceneLoaded()
	LogDG("SceneModeGotoSchool.OnSceneLoaded()")
end

function SceneModeGotoSchool.OnEnd()
	LogDG("SceneModeGotoSchool.OnEnd()")
	GotoSchoolLogic.Instance:Destroy()
	TimeHelper:CancelTimer(SceneModeGotoSchool.time_ht)
end



-- 归墟之境
SceneModeRuins = {}
function SceneModeRuins.OnBegin()
	SceneCtrl.Instance:Objs():ShieldingPet(true)
	GuideManager.Instance:RevertYinYangEye(true)
	ViewMgr:CloseView(LandsBetweenMainView)
end

function SceneModeRuins.OnSceneLoaded()
	ViewMgr:OpenView(RuinsView)
end

function SceneModeRuins.OnEnd()
	SceneCtrl.Instance:Objs():ShieldingPet(false)
	-- GotoSchoolLogic.Instance:Destroy()
	ViewMgr:CloseView(RuinsView)
	ViewMgr:CloseView(RuinsRank)
	ViewMgr:CloseView(RuinsAchi)
	ViewMgr:CloseView(RuinsChallengeRank)
	LandsBetweenCtrl.Instance:TryOpenMain()
end
-- 殿堂10003
SceneModePalaceRank = {}
function SceneModePalaceRank.OnBegin()
	GuideManager.Instance:RevertYinYangEye(true)
	ChatData.Instance:SetShowBack(true)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.palace_rank)
	ViewMgr:CloseView(PalacesHistoryRank)
	ViewMgr:CloseView(PalacesWorship)
	ViewMgr:CloseView(LandsBetweenMainView)
end
function SceneModePalaceRank.OnSceneLoaded()
	GodAreasActsCtrl.Instance:GodSpaceOperatePalaceFirstRank()
	GodAreasActsData.Instance:OnCreatePalaceStatus()
end 
function SceneModePalaceRank.OnEnd()
	ChatData.Instance:SetShowBack(false)
	--删除雕像
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	GodAreasActsData.Instance:OnCreatePalaceStatus()
	LandsBetweenCtrl.Instance:TryOpenMain()
end
--云顶天宫10002
SceneModePalaces = {}
function SceneModePalaces.OnBegin()
	GuideManager.Instance:RevertYinYangEye(true)
	SceneCtrl.Instance:Objs():ShieldingPet(true)
	ViewMgr:CloseView(LandsBetweenMainView)
end

function SceneModePalaces.OnSceneLoaded()
	ViewMgr:OpenView(PalacesView)
end
function SceneModePalaces.OnEnd()
	SceneCtrl.Instance:Objs():ShieldingPet(false)
	ViewMgr:CloseView(PalacesView)
	ViewMgr:CloseView(PalacesRank)
	ViewMgr:CloseView(PalacesAchi)
	ViewMgr:CloseView(PalacesChallengeRank)
	LandsBetweenCtrl.Instance:TryOpenMain()
end

UTArenaStandyBy = {}
function UTArenaStandyBy.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.ut_arena)
	ChatData.Instance:SetShowBack(true)
end

function UTArenaStandyBy.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ChatData.Instance:SetShowBack(false)
end



SceneSplaceAnecdoteFB = {}
function SceneSplaceAnecdoteFB.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.splace_anecdote)
	MainViewData.Instance:SetHideButton()
    ChatData.Instance:SetShowBack(true)
end

function SceneSplaceAnecdoteFB.OnEnd()
	ChatData.Instance:SetShowBack(false)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
    if TeamCtrl.Instance:Data():IsLeader() then
        ServerActivityCtrl.Instance:SendCloseWindosReq({reason = 1})
		ServerActivityCtrl.Instance:SendCloseWindosReq({reason = 2})
    end 
end

TombRaiderFB = {}
function TombRaiderFB.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.tomb_raider)
	JoyStickData.Instance:HideJoyStick()
	-- ViewMgr:CloseAllViewInBattle()
	-- if not ViewMgr:IsOpen(TombRaiderView) and not ViewMgr:IsOpen(SportInfoView) then 
	-- 	ViewMgr:OpenView(TombRaiderBattleView)
	-- end 
	if TombRaiderData.Instance:GetGameStartState().reason_type > 1 and not ViewMgr:IsOpen(TombRaiderBattleView) then
		ViewMgr:CloseAllViewInBattle()
		ViewMgr:OpenView(TombRaiderBattleView)
		ViewMgr:CloseView(TombRaiderView)
	end 
	TombRaiderData.Instance:SetShowMark()
	ViewMgr:CloseView(KeyUseView)
end

function TombRaiderFB.OnSceneLoaded()
	-- BattleLuaFunction.UIHide()
end

function TombRaiderFB.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	TombRaiderData.Instance:EndShowMark()
	JoyStickData.Instance:ShowJoyStick()
	TombRaiderBattleShow.Instance:DeleteAllEffect()
	BattleLuaFunction.UIShow()
	ViewMgr:CloseView(TombRaiderBattleView)
	ViewMgr:CloseView(TombRaiderFinishView)

	ViewMgr:OpenView(TombRaiderView)
end

SceneModeMaterial = {}
function SceneModeMaterial.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.material)
	MainViewData.Instance:SetHideButton()
	ChatData.Instance:SetShowBack(true)
end

function SceneModeMaterial:OnSceneLoaded()
	if MaterialCtrl.IsOnceDreamNotesScene == nil then
		Invoke(function()
			MainOtherCtrl.Instance:WaitViewFunc()
		end,2)
		MaterialCtrl.IsOnceDreamNotesScene = true
	end
end

function SceneModeMaterial.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	MaterialCtrl.IsOnceDreamNotesScene = nil
	ChatData.Instance:SetShowBack(false)
	MainOtherCtrl.Instance:SetWaitViewFunc(nil)
end

SceneModeGoThrough = {}
function SceneModeGoThrough.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.go_through)
	MainViewData.Instance:SetHideButton()
	ChatData.Instance:SetShowBack(true)
end

function SceneModeGoThrough.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ChatData.Instance:SetShowBack(false)
	MainOtherCtrl.Instance:SetWaitViewFunc(nil)
end

SceneModeAllSoulsFB = {}
function SceneModeAllSoulsFB.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.all_souls_storybook)
	MainViewData.Instance:SetHideButton()
	ChatData.Instance:SetShowBack(true)
	BehaviorData.Instance.auto_behavior = 5
	if AllSoulsStorybookData.Instance:SceneInfo().seq <= 1 then		
		ViewMgr:OpenView(AllSoulsStorybookStart)
	end
end
function SceneModeAllSoulsFB.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ChatData.Instance:SetShowBack(false)
	MainOtherCtrl.Instance:SetWaitViewFunc(nil)
	BehaviorData.Instance.auto_behavior = 0
end


SceneModeFishGround = {}
function SceneModeFishGround.OnBegin()
	if MainPlayer then
		MainPlayer:BecomeFisherman()
	end
	MainViewCtrl.Instance:SetGameState(MainViewGameState.fish_ground)
	MainViewData.Instance:SetHideButton()
	ChatData.Instance:SetShowBack(true)
	SceneModeFishGround.ChangeTime(1)
	local scene_id = RoleData.Instance:GetBaseData().scene_id
	local skill_type = LiveData.Instance:GetSkillBySceneId(scene_id)
	local desc = LiveData.Instance:GetSceneAllRewardDesc(scene_id)
	local title = Language.Live.SkillSceneTitle[skill_type] or ""
	local data  = {title = title, desc = desc}
	PublicPopupCtrl.PopupLifeMsg(data)
	PublicPopupCtrl.Instance:Center(Language.Live.FishEnter)
end
function SceneModeFishGround.ChangeBlend(styleName)
	Cinemachine.loadassembly()
	local main_cam = SceneMgr.FindGameObjectWithTag("MainCamera")
    local Brain = typeof('Cinemachine.CinemachineBrain')
    local BlendDefinition = typeof('Cinemachine.CinemachineBlendDefinition')
    local BlendStyle = typeof('Cinemachine.CinemachineBlendDefinition+Style')
    local defalut = tolua.getfield(Brain, "m_DefaultBlend")
    local style = tolua.getfield(BlendDefinition,"m_Style")
    local BrainCom = main_cam:GetComponent(Brain)
    local styleValue = defalut:Get(BrainCom)
    local origin_value = style:Get(styleValue)
    local EaseInOut = tolua.getfield(BlendStyle, styleName)
    local EaseInOutValue = EaseInOut:Get(origin_value)
    style:Set(styleValue, EaseInOutValue)
    defalut:Set(BrainCom, styleValue)
    defalut:Destroy()
    style:Destroy()
    EaseInOut:Destroy()
end

function SceneModeFishGround.ChangeTime(time)
	Cinemachine.loadassembly()
	local main_cam = SceneMgr.FindGameObjectWithTag("MainCamera")
    local Brain = typeof('Cinemachine.CinemachineBrain')
    local BlendDefinition = typeof('Cinemachine.CinemachineBlendDefinition')
    local defalut = tolua.getfield(Brain, "m_DefaultBlend")
    local BrainCom = main_cam:GetComponent(Brain)
    local styleValue = defalut:Get(BrainCom)
    local m_Time = tolua.getfield(BlendDefinition,"m_Time")
	m_Time:Set(styleValue, time)
    defalut:Set(BrainCom, styleValue)
    defalut:Destroy()
    m_Time:Destroy()
end

function SceneModeFishGround.OnEnd()
	FishData.Instance.fish_list = {}
	SceneModeFishGround.ChangeTime(0)
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ChatData.Instance:SetShowBack(false)
	MainOtherCtrl.Instance:SetWaitViewFunc(nil)
	if MainPlayer then
		MainPlayer:FlushRoleDrawer()
	end
	-- SceneModeFishGround.ChangeBlend("Cut")
end

SceneModeWeddingFB = {}
function SceneModeWeddingFB.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.wedding_fb)
	MainViewData.Instance:SetHideButton()
	ChatData.Instance:SetShowBack(true)
	MarryData.Instance.ShowRiteEffect = true
	MarryData.Instance.IsShowStory = false
	if MainPlayer then
		MainPlayer:BecomeWeddingman(MarryData.Instance:GetWeddingIsHost())
	end
	GuideManager.Instance:RevertYinYangEye(true)
end

function SceneModeWeddingFB.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ChatData.Instance:SetShowBack(false)
	MarryData.Instance:ClearWeddingFBScene()
	if MainPlayer then
		MainPlayer:FlushRoleDrawer()
	end
	-- GuideManager.Instance:ShowYinYangEye(true)
end

SceneModeMarryParty = {}
function SceneModeMarryParty.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.marry_party)
	MainViewData.Instance:SetHideButton()
	ChatData.Instance:SetShowBack(true)

	GuideManager.Instance:RevertYinYangEye(true)
end

function SceneModeMarryParty.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ChatData.Instance:SetShowBack(false)
	-- GuideManager.Instance:ShowYinYangEye(true)
end

SceneModeHarmonyHeaven = {}
function SceneModeHarmonyHeaven.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.harmony_heaven)
	MainViewData.Instance:SetHideButton()
	ChatData.Instance:SetShowBack(true)
	ViewMgr:CloseView(HarmonyHeavenView)

	GuideManager.Instance:RevertYinYangEye(true)
end

function SceneModeHarmonyHeaven.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	ChatData.Instance:SetShowBack(false)
	-- GuideManager.Instance:ShowYinYangEye(true)
end

function SceneModeHarmonyHeaven.OnSceneLoaded()
	if HarmonyHeavenData.Instance.need_play_story then
		HarmonyHeavenData.Instance.need_play_story = false
		StoryCtrl.Instance:PlayStoryByHHShowScene(SceneData.Instance:SceneId())
	end
end

SceneModeShiTuChuanGong = {}
function SceneModeShiTuChuanGong.OnBegin()
	MainViewData.Instance:SetHideButton()
	ChatData.Instance:SetShowBack(true)
	-- GuideManager.Instance:RevertYinYangEye(true)
	SocietyData.Instance.shitu_chuangong_loop = true
	StoryData.Instance:StartStoryById(110)
	ViewMgr:CloseView(KeyUseView)
	ViewMgr:CloseAllViewInBattle()
end

function SceneModeShiTuChuanGong.OnEnd()
	ChatData.Instance:SetShowBack(false)
	-- GuideManager.Instance:ShowYinYangEye(true)
	SocietyData.Instance:ShiTuChuanGongRewardShow()
	ViewMgr:CloseView(MusicGameView)
end


SceneModeWorldArenaStandyBy = {}
function SceneModeWorldArenaStandyBy.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.world_arena)
end

function SceneModeWorldArenaStandyBy.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
end

SceneModeFightFly2 = {}
function SceneModeFightFly2.OnBegin()
end

function SceneModeFightFly2.OnSceneLoaded()
	FightData.Instance:DoFightEliteWaitFunc()
end

function SceneModeFightFly2.OnEnd()
end

SceneModeHeroVanBattle = {}
function SceneModeHeroVanBattle.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.hero_van_battle)
end

function SceneModeHeroVanBattle.OnSceneLoaded()

end

function SceneModeHeroVanBattle.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	PublicPopupCtrl.Instance:CloseWarningView()
	-- ViewMgr:OpenView(HeroVanBattleGloryView)
	-- 离开场景时清空本地已报名信息
	-- HeroVanBattleData.Instance:SetTeamArenaOtherInfo({is_sign_up = 0})
end

--情缘对对碰
SceneModeLoversMatch = {}
function SceneModeLoversMatch.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.lovers_match)
	MainViewData.Instance:SetHideButton()
	LoversMatchCtrl.Instance:BeginCheck()
	ChatData.Instance:SetShowBack(true)
end

function SceneModeLoversMatch.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	LoversMatchCtrl.Instance:EndCheck()
	ChatData.Instance:SetShowBack(false)
	if MainPlayer then
		MainPlayer:FlushRoleDrawer()
	end
end

SceneLookingForRabbits = {}
SceneLookingForRabbits.objs = {}
function SceneLookingForRabbits.OnBegin()
	-- GuideManager.Instance:RevertYinYangEye(true)
	MainOtherCtrl.Instance.stop = true
	PublicPopupCtrl.Instance.stop_act = true
    MainOtherCtrl.Instance:CloseKeyUse()
	SceneLookingForRabbits.is_end = false
	for i = 3, 5, 1 do
		SceneLookingForRabbits.LoadPrefab(i)
	end
	ViewMgr:OpenView(LookingForRabbits)
	if MainPlayer then
		MainPlayer:SetVisible(false)
	end
end
function SceneLookingForRabbits.OnEnd()
	-- GuideManager.Instance:ShowYinYangEye(true)
	MainOtherCtrl.Instance.stop = false
	PublicPopupCtrl.Instance.stop_act = false
	SceneLookingForRabbits.is_end = true
	for i, v in ipairs(SceneLookingForRabbits.objs) do
		UnityGameObj.Destroy(v)
	end
	if MainPlayer then
		MainPlayer:SetVisible(true)
	end
end
function SceneLookingForRabbits.LoadPrefab(index)
	local res_path = string.format("ui/views/looking_for_rabbits/rabbits%d.prefab", index)
	ResMgr:LoadGameObject(res_path, function (obj)
		if SceneLookingForRabbits.is_end == true then
			if obj then
				UnityGameObj.Destroy(obj)
			end
		end
		table.insert(SceneLookingForRabbits.objs, obj)
        obj.transform.localPosition = Vector3.New(400.51,125.76,293.87)
        obj:SetParent(UnityGameObj.Find("ResidentObjectManager"))
    end, true, false)
end

SceneLuckyRingFB = {}
function SceneLuckyRingFB.OnBegin()
	GuideManager.Instance:RevertYinYangEye(true)
	MainOtherCtrl.Instance.stop = true
    MainOtherCtrl.Instance:CloseKeyUse()

	-- MainViewCtrl.Instance:SetGameState(MainViewGameState.lucky_ring)
	-- JoyStickData.Instance:HideJoyStick()
	-- ViewMgr:CloseAllViewInBattle()

	ViewMgr:OpenView(LuckyRingBattleView)

	if MainPlayer then
		MainPlayer:SetVisible(false)
	end

	LuckyRingBattleShow.Instance:StartScene()

	if PrefsInt(PrefKeys.LuckyRingSynopsisView())==0 then 
        ViewMgr:OpenView(LuckyRingSynopsisView)
		PrefsInt(PrefKeys.LuckyRingSynopsisView(),1)
	end 
end

function SceneLuckyRingFB.OnSceneLoaded()
	-- BattleLuaFunction.UIHide()
end

function SceneLuckyRingFB.OnEnd()
	-- GuideManager.Instance:ShowYinYangEye(true)
	MainOtherCtrl.Instance.stop = false
	-- MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)
	if MainPlayer then
		MainPlayer:SetVisible(true)
	end

	LuckyRingCtrl.Instance:OpenFinishView()
	LuckyRingBattleShow.Instance:ExitScene()
	LuckyRingCtrl.Instance:SendLuckyRingReq({req_type = 0,param1 = 0,param2 = 0})
end

SceneModeYouMingSeal = {}
function SceneModeYouMingSeal.OnBegin()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.youming_seal)
end

function SceneModeYouMingSeal.OnEnd()
	MainViewCtrl.Instance:SetGameState(MainViewGameState.normal)	
end

--场景类型（与场景配置中一致
SceneModeType = {
	None = 0,
	CourageChallenge = 2,
	Conspiracy = 3,
	FightCrystal = 4,
	TeamSport = 5,
	MazeRun = 6,
	FightGather = 7,
	MazeRunWait = 8,
	RushDungeon = 9,
	GuildMelee = 10,
	ChiefElelction = 11, 
	ChiefPeakBattle = 12,
	GuildEmbattle = 13,
	GuildAnswer = 14,
	AdvancedFB = 16,
	ArenaMatch = 17,
	GuildHome = 18,
	GuildFB = 19,
	MedalFB = 20,
	FightFantasy = 21,
	AnimalRace = 22,
	OnlyFight = 23,
	ShiTuFB = 24,
	ChiefPrimary = 25,
	DuChuangZeiYing = 27,
	WenXiangLou = 28,
	FoxCatch = 29,
	StarLife = 30,
	DreamNotes = 31,
	LuckyMaze = 32,
	SplaceAnecdote = 33,
	Material = 34,
	TombRaider = 35,
	ArrayGhost = 36,
	PetGod = 37,
	GoThrough = 38,
	SoulsAnecdote = 39,
	FishGround = 40,
	AllSoulsFB = 41,
	WeddingFB = 42,
	WorldArenaStandyBy = 43,	--论剑九州准备房间
	FightFly2 = 44,
	HeroVanBattle = 45,
	MarryParty = 46,
	ShiTuChuanGong = 47,
	LoversMatch = 48,
	SanXianGuiDong = 49,
	RabbitRace = 50,
	LuckyRing = 51,
	UTArena = 52,
	HarmonyHeaven = 53,
	GotoSchool = 54,
	
	--client_special_mode----9000~9999---------
	YouMingSeal = 9000,
	---------------
	Ruins = 10001,
	Palaces = 10002,
	PalaceRank = 10003,
}

-- （key）场景类型
SceneConfigToMode = {
	[SceneModeType.None] = SceneModeNormal,							-- 普通
	[SceneModeType.CourageChallenge] = SceneModeCourageChallenge,	-- 勇气的挑战
	[SceneModeType.Conspiracy] = SceneModeConspiracy,				-- 密医的阴谋
	[SceneModeType.FightCrystal] = SceneModeFightCrystal,			-- 水晶副本
	[SceneModeType.TeamSport] = SceneModeTeamSport,				    -- 竞技对抗赛
	[SceneModeType.MazeRun] = SceneModeMazeRun,					    -- 迷宫竞速
	[SceneModeType.FightGather] = SceneModeFightGather,				-- 战斗采集
	[SceneModeType.MazeRunWait] = SceneModeMazeRunWait,				-- 迷宫竞速等待
	[SceneModeType.RushDungeon] = SceneModeRushDungeon,				-- 勇闯地宫
	[SceneModeType.GuildMelee] = SceneModeGuildMelee,				-- 家族大乱斗
	[SceneModeType.ChiefPrimary] = SceneModeChiefPrimary,		    -- 首席预选
	[SceneModeType.ChiefElelction] = SceneModeChiefElection,		-- 首席竞选
	[SceneModeType.ChiefPeakBattle] = SceneModeChiefPeakBattle,		-- 首席巅峰战
	[SceneModeType.GuildEmbattle] = SceneModeGuildEmbattle,         -- 家族荣誉战
	[SceneModeType.GuildAnswer] = SceneModeGuildAnswer,             -- 家族答题
	[SceneModeType.AdvancedFB] = SceneModeAdvanced,                 -- 进阶副本
	[SceneModeType.ArenaMatch] = SceneModeArenaMatch,				-- 擂台战
	[SceneModeType.GuildFB] = SceneModeGuildFB,						-- 指引副本
	[SceneModeType.GuildHome] = SceneModeGuildHome,                 -- 家族驻地
	[SceneModeType.MedalFB] = SceneModeMedalFB,           			-- 四象幻境
	[SceneModeType.FightFantasy] = SceneModeFightFantasy,           -- 四象幻境
	[SceneModeType.AnimalRace] = SceneModeAnimalRace,           	-- 神兽径赛
	[SceneModeType.OnlyFight] = SceneModeOnlyFight,					-- 以一敌百
	[SceneModeType.ShiTuFB] = SceneModeShiTuFB,						-- 师徒副本
	[SceneModeType.DuChuangZeiYing] = SceneModeDuChuangZeiYing,		-- 独闯贼营
	[SceneModeType.WenXiangLou] = SceneModeWenXiangLou,				-- 温香楼
	[SceneModeType.FoxCatch] = SceneModeFoxCatchFB,				    -- 捉拿狐妖
	[SceneModeType.StarLife] = SceneModeStarLifeFB,				    -- 司命星君
	[SceneModeType.DreamNotes] = SceneModeDreamNotes,				-- 梦渊笔录
	[SceneModeType.LuckyMaze] = SceneLuckyMazeFB, 					-- 幸运迷宫
	[SceneModeType.SplaceAnecdote] = SceneSplaceAnecdoteFB, 		-- 秘境奇闻
	[SceneModeType.SoulsAnecdote] = SceneSplaceAnecdoteFB, 		    -- 秘境奇闻
	[SceneModeType.TombRaider] = TombRaiderFB, 		                -- 遗迹夺宝
	[SceneModeType.Material] = SceneModeMaterial,				    -- 玉虚境
	[SceneModeType.ArrayGhost] = SceneModeArrayGhost,				-- 阵中之魂
	[SceneModeType.PetGod] = SceneModePetGod,						-- 神印地
	[SceneModeType.GoThrough] = SceneModeGoThrough,				    -- 渡劫
	[SceneModeType.FishGround] = SceneModeFishGround,				-- 钓鱼

	[SceneModeType.AllSoulsFB] = SceneModeAllSoulsFB,				-- 万灵话本
	[SceneModeType.WeddingFB] = SceneModeWeddingFB,					-- 婚宴副本

	[SceneModeType.FightFly2] = SceneModeFightFly2,					-- 化圣秘境
	[SceneModeType.WorldArenaStandyBy] = SceneModeWorldArenaStandyBy,
	[SceneModeType.HeroVanBattle] = SceneModeHeroVanBattle,         -- 英雄会武
	[SceneModeType.ShiTuChuanGong] = SceneModeShiTuChuanGong,      	-- 师徒传功
	[SceneModeType.MarryParty] = SceneModeMarryParty,      			-- 情缘盛会
	[SceneModeType.LoversMatch] = SceneModeLoversMatch,         	-- 情缘对对碰
	[SceneModeType.RabbitRace] = SceneRabbitRaceFB,					-- 兔兔快跑周期活动
	[SceneModeType.LuckyRing] = SceneLuckyRingFB,					-- 套圈纳吉
	[SceneModeType.SanXianGuiDong] = SceneLookingForRabbits,		-- 三仙归洞
	[SceneModeType.UTArena] = UTArenaStandyBy,						-- 天下第一
	[SceneModeType.HarmonyHeaven] = SceneModeHarmonyHeaven,			-- 鸿蒙天界
	[SceneModeType.GotoSchool] = SceneModeGotoSchool,				-- 我要上学
	[SceneModeType.Ruins] = SceneModeRuins,							-- 归墟之境
	[SceneModeType.PalaceRank] = SceneModePalaceRank,				-- 殿堂排行
	[SceneModeType.Palaces] = SceneModePalaces,						-- 云顶天宫
	[SceneModeType.YouMingSeal] = SceneModeYouMingSeal,						-- 云顶天宫
}