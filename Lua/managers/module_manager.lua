ModulesManager = ModulesManager or BaseClass()

local lua_mono_auto_release_interval = 100

if IS_EDITOR then
    lua_mono_auto_release_interval = 1
elseif UnityEngine.SystemInfo.systemMemorySize <= 2500 then
    lua_mono_auto_release_interval = 50
end

function ModulesManager:__init()
    if ModulesManager.Instance ~= nil then
        Debuger.LogError("[ModulesManager] attempt to create singleton twice!")
        return
    end
    ModulesManager.Instance = self

    self.smart_data_notifier = SmartDataNotifier.New()

    self.ctrl_list = {}
    self:createCtrls()
    self:onInitCtrls()
    -- LogError("REGISTER LowMEMFUNC===!!!!!!!!!!!!!!!!!!")

    self.low_mem_warning_func = BindTool.Bind(self.OnLowMemoryWarning, self)
    UnityApp.lowMemory = UnityApp.lowMemory + self.low_mem_warning_func
end

function ModulesManager:__delete()
    if self.low_mem_warning_func ~= nil then
        UnityApp.lowMemory = UnityApp.lowMemory - self.low_mem_warning_func
        self.low_mem_warning_func = nil
        -- LogError("Release MemoryWARING===11111111111111111")
    end

    self.smart_data_notifier:DeleteMe()
    self.smart_data_notifier = nil
    self:deleteCtrls()

    if ModulesManager.Instance == self then
        ModulesManager.Instance = nil
    end
end

function ModulesManager:OnLowMemoryWarning()
    -- LogError("####################LowMemoryWARNING!!!#####################")
    SceneMgr:ResetReleaseData()
    LuaMonoCheckAndCleanCache()
    collectgarbage("collect")
    UnityEngine.Resources.UnloadUnusedAssets();
end

function ModulesManager:createCtrls()
    table.insert(self.ctrl_list, TimeCtrl.New())
    table.insert(self.ctrl_list, PreloadCtrl.New())
    table.insert(self.ctrl_list, BundleProviderCtrl.New())
    table.insert(self.ctrl_list, GMCmdCtrl.New())
    table.insert(self.ctrl_list, NetworkCtrl.New())
    table.insert(self.ctrl_list, LoadingCtrl.New())
    table.insert(self.ctrl_list, SceneCtrl.New())
    table.insert(self.ctrl_list, LoginCtrl.New())
    table.insert(self.ctrl_list, BehaviorCtrl.New())
    table.insert(self.ctrl_list, RemindCtrl.New())
    table.insert(self.ctrl_list, QualityCtrl.New())
    table.insert(self.ctrl_list, ExternalLuaCtrl.New()) -- 不重启热更代码模块

    table.insert(self.ctrl_list, PublicPopupCtrl.New())
    table.insert(self.ctrl_list, ItemInfoCtrl.New())
    table.insert(self.ctrl_list, RoleInfoCtrl.New())
    table.insert(self.ctrl_list, RoleCtrl.New())
    table.insert(self.ctrl_list, RoleSkillCtrl.New())
    table.insert(self.ctrl_list, MainViewCtrl.New())
    table.insert(self.ctrl_list, BattleCtrl.New())
    table.insert(self.ctrl_list, LimitTimeActivityCtrl.New())
    table.insert(self.ctrl_list, FunOpen.New())
    table.insert(self.ctrl_list, GuideManager.New())
    table.insert(self.ctrl_list, FinishCtrl.New())

    table.insert(self.ctrl_list, BagCtrl.New())
    table.insert(self.ctrl_list, AutoInvokerCtrl.New())
    table.insert(self.ctrl_list, ChatCtrl.New())
    table.insert(self.ctrl_list, ExperienceCtrl.New())
    table.insert(self.ctrl_list, ChallengeNaviCtrl.New())

    table.insert(self.ctrl_list, SocietyCtrl.New())
    table.insert(self.ctrl_list, PartnerCtrl.New())
    table.insert(self.ctrl_list, TaskCtrl.New())
    table.insert(self.ctrl_list, DuChuangZeiYingCtrl.New())
    table.insert(self.ctrl_list, WenXiangLouCtrl.New())
    table.insert(self.ctrl_list, TreasureTaskCtrl.New())
    table.insert(self.ctrl_list, BountyTaskCtrl.New())
    table.insert(self.ctrl_list, StoryCtrl.New())
    table.insert(self.ctrl_list, VideoCtrl.New())
    table.insert(self.ctrl_list, NpcDialogCtrl.New())
    table.insert(self.ctrl_list, ActivityCtrl.New())
    table.insert(self.ctrl_list, TeamCtrl.New())
    table.insert(self.ctrl_list, KnightlyCtrl.New())
    table.insert(self.ctrl_list, IllusionCtrl.New())
    table.insert(self.ctrl_list, ActFormationCtrl.New())
    table.insert(self.ctrl_list, CourageChallengeCtrl.New())
    table.insert(self.ctrl_list, GuildHofCtrl.New())
    table.insert(self.ctrl_list, GuildBuildingCtrl.New())
    table.insert(self.ctrl_list, GuildCtrl.New())
    table.insert(self.ctrl_list, GuildRedBagCtrl.New())
    table.insert(self.ctrl_list, GuildEmbattleCtrl.New())
    table.insert(self.ctrl_list, FightCtrl.New())
    table.insert(self.ctrl_list, HarmonyHeavenCtrl.New())
    table.insert(self.ctrl_list, FormationCtrl.New())
    table.insert(self.ctrl_list, MapCtrl.New())
    table.insert(self.ctrl_list, ExchangeMonoCtrl.New())
    table.insert(self.ctrl_list, EscortCtrl.New())
    table.insert(self.ctrl_list, WorldBossCtrl.New())
    table.insert(self.ctrl_list, PetFeiShengCtrl.New())
    table.insert(self.ctrl_list, PetCtrl.New())
    table.insert(self.ctrl_list, PetStampCtrl.New())
    table.insert(self.ctrl_list, PetWeaponCtrl.New())
    table.insert(self.ctrl_list, ArenaMatchCtrl.New())
    table.insert(self.ctrl_list, AnimalRaceCtrl.New())
    table.insert(self.ctrl_list, SwordZitherCtrl.New())
    table.insert(self.ctrl_list, WorkshopCtrl.New())
    table.insert(self.ctrl_list, EquipCtrl.New())
    table.insert(self.ctrl_list, AccrssoryCtrl.New())
    table.insert(self.ctrl_list, LingBaoCtrl.New())
    table.insert(self.ctrl_list, MiniGameCtrl.New())
    table.insert(self.ctrl_list, TowerAdventureCtrl.New())
    table.insert(self.ctrl_list, CowBoyCtrl.New())
    table.insert(self.ctrl_list, AnimalWorldCtrl.New())
    table.insert(self.ctrl_list, VitalityFireworkCtrl.New())
    table.insert(self.ctrl_list, BravingCtrl.New())
    table.insert(self.ctrl_list, AppraiseHasGiftCtrl.New())
    table.insert(self.ctrl_list, ReedIncenseCityCtrl.New())
    table.insert(self.ctrl_list, SweetSaltyWarCtrl.New())
    table.insert(self.ctrl_list, SoupBathCtrl.New())
    table.insert(self.ctrl_list, QixiFestivalCtrl.New())
    table.insert(self.ctrl_list, SevenBuyCtrl.New())
    table.insert(self.ctrl_list, FourSeasonCtrl.New())
    table.insert(self.ctrl_list, KFHeroRecordCtrl.New())
    table.insert(self.ctrl_list, GodClothesCtrl.New())
    table.insert(self.ctrl_list, SingleRebateCtrl.New())
    table.insert(self.ctrl_list, RoleSettingCtrl.New())
    table.insert(self.ctrl_list, MainOtherCtrl.New())
    table.insert(self.ctrl_list, FightDenCtrl.New())
    table.insert(self.ctrl_list, FabaoCtrl.New())
    table.insert(self.ctrl_list, CombatCtrl.New())
    table.insert(self.ctrl_list, ConspiracyCtrl.New())
    table.insert(self.ctrl_list, AchieveCtrl.New())
    table.insert(self.ctrl_list, TreasureMapCtrl.New())
    table.insert(self.ctrl_list, ResearchTaskCtrl.New())
    table.insert(self.ctrl_list, AmuletCtrl.New())
    table.insert(self.ctrl_list, MSLanternCtrl.New())
    table.insert(self.ctrl_list, SkyTreasureHuntCtrl.New())
    table.insert(self.ctrl_list, ImperialExamCtrl.New())

    table.insert(self.ctrl_list, MallCtrl.New())
    table.insert(self.ctrl_list, TradeCtrl.New())
    table.insert(self.ctrl_list, ChongJiFanLiCtrl.New())
    table.insert(self.ctrl_list, RechargeCtrl.New())
    table.insert(self.ctrl_list, ExchangeCtrl.New())
    table.insert(self.ctrl_list, ShopCtrl.New())
    table.insert(self.ctrl_list, MedalTaskCtrl.New())

    table.insert(self.ctrl_list, RewardCtrl.New())
    table.insert(self.ctrl_list, FirstChargeCtrl.New())
    table.insert(self.ctrl_list, RoleAppearChangeCtrl.New())
    table.insert(self.ctrl_list, SinglePopupCtrl.New())
    table.insert(self.ctrl_list, ChallengeCtrl.New())
    table.insert(self.ctrl_list, UnlockNewMapCtrl.New())
    table.insert(self.ctrl_list, MazeRunCtrl.New())
    table.insert(self.ctrl_list, TeamSportCtrl.New())
    table.insert(self.ctrl_list, TreasureHuntCtrl.New())
    table.insert(self.ctrl_list, RushDungeonCtrl.New())
    table.insert(self.ctrl_list, ArenaCtrl.New())
    table.insert(self.ctrl_list, ChallengeTestCtrl.New())
    table.insert(self.ctrl_list, GodLandTracesCtrl.New())

    table.insert(self.ctrl_list, PeriodActivityCtrl.New())
    table.insert(self.ctrl_list, ServerActivityCtrl.New()) -- 活动相关页面加在这里
    table.insert(self.ctrl_list, ActWordGiftCtrl.New())
    table.insert(self.ctrl_list, SevenDayCtrl.New())
    table.insert(self.ctrl_list, DivinationCtrl.New()) -- 天命卜卦
    table.insert(self.ctrl_list, AthleticsCtrl.New()) -- 新服竞技
    table.insert(self.ctrl_list, GodTokenCtrl.New()) -- 封神战令
    table.insert(self.ctrl_list, CustomizedTreasureCtrl.New()) -- 定制宝箱
    table.insert(self.ctrl_list, CowGoodLuckCtrl.New()) -- 牛运无边
    table.insert(self.ctrl_list, FarewellAskNeoCtrl.New()) -- 辞旧迎新
    table.insert(self.ctrl_list, ActLuckyCowCtrl.New()) -- 福牛报喜
    table.insert(self.ctrl_list, ActNewServerPetCtrl.New())   --新服灵宠
    table.insert(self.ctrl_list, LoveFashionCtrl.New()) -- 情缘时装

    table.insert(self.ctrl_list, LimitRechargeCtrl.New()) -- 限时累充
    table.insert(self.ctrl_list, ActLuckyTigerCtrl.New()) -- 寅虎纳福
    table.insert(self.ctrl_list, TigerGreatLuckCtrl.New()) -- 虎行大运
    table.insert(self.ctrl_list, TigerThankCtrl.New()) -- 寅虎拜岁
    table.insert(self.ctrl_list, TigerYearTotalCtrl.New()) -- 寅虎汇总
    table.insert(self.ctrl_list, TigerSpringTravelCtrl.New()) -- 寅虎探春

    table.insert(self.ctrl_list, ChiefCtrl.New())
    table.insert(self.ctrl_list, EventNodeProcessCenter.New())
    table.insert(self.ctrl_list, ReputationCtrl.New())
    table.insert(self.ctrl_list, AppearanceCtrl.New())
    table.insert(self.ctrl_list, AnswerCtrl.New())
    table.insert(self.ctrl_list, CutChapterCtrl.New())
    table.insert(self.ctrl_list, MonsterCtrl.New())
    table.insert(self.ctrl_list, MainTaskCtrl.New())
    table.insert(self.ctrl_list, SilkRoadCtrl.New())
    table.insert(self.ctrl_list, JoySeeksFavorsCtrl.New())
    table.insert(self.ctrl_list, IllustrateCtrl.New())
    table.insert(self.ctrl_list, OnlyFightCtrl.New())
    table.insert(self.ctrl_list, BattlefieldCtrl.New())
    table.insert(self.ctrl_list, AnotherCtrl.New())
    table.insert(self.ctrl_list, MysteryStoreCtrl.New())
    table.insert(self.ctrl_list, AnnouncementCtrl.New())
    table.insert(self.ctrl_list, ServiceCtrl.New())
    table.insert(self.ctrl_list, LevelComplementCtrl.New())

    table.insert(self.ctrl_list, PublicInfoShowCtrl.New())
    table.insert(self.ctrl_list, NeoCharaChangeCtrl.New())
    table.insert(self.ctrl_list, AnecdoteCtrl.New())
    table.insert(self.ctrl_list, PokerCtrl.New())
    table.insert(self.ctrl_list, SchoolExerciseCtrl.New())
    table.insert(self.ctrl_list, DreamNotesCtrl.New())
    table.insert(self.ctrl_list, DivineCostumeCtrl.New())
    table.insert(self.ctrl_list, StarTreasureMapCtrl.New())
    table.insert(self.ctrl_list, BicycleCtrl.New())
    table.insert(self.ctrl_list, ActiveCtrl.New())
    table.insert(self.ctrl_list, FightFieldBossCtrl.New())
    table.insert(self.ctrl_list, MaterialCtrl.New())
    table.insert(self.ctrl_list, TombRaiderCtrl.New())
    table.insert(self.ctrl_list, FabaoLotteryCtrl.New())
    table.insert(self.ctrl_list, CloudArenaCtrl.New())
    table.insert(self.ctrl_list, MarketCtrl.New())
    table.insert(self.ctrl_list, HeartSkillCtrl.New())
    table.insert(self.ctrl_list, PeakRoadCtrl.New())
    table.insert(self.ctrl_list, BianQiangCtrl.New())
    table.insert(self.ctrl_list, GhostNightCtrl.New())
    table.insert(self.ctrl_list, DeepDarkFantasyCtrl.New())
    table.insert(self.ctrl_list, BigDipperCtrl.New())
    table.insert(self.ctrl_list, AttributeDanCtrl.New())
    table.insert(self.ctrl_list, DuelCtrl.New())
    table.insert(self.ctrl_list, YunZeCtrl.New())
    table.insert(self.ctrl_list, MayDayCtrl.New())
    table.insert(self.ctrl_list, GoThroughCtrl.New())
    table.insert(self.ctrl_list, YinYangCtrl.New())
    table.insert(self.ctrl_list, InviteFightCtrl.New())
    table.insert(self.ctrl_list, LiveCtrl.New())
    table.insert(self.ctrl_list, EndlessTowerCtrl.New())
    table.insert(self.ctrl_list, SwornCtrl.New())
    table.insert(self.ctrl_list, ActZhaoCaiJinBaoCtrl.New())
    table.insert(self.ctrl_list, ActDuanYangZiXuanLiCtrl.New())
    table.insert(self.ctrl_list, ActDuanYangFuLiCtrl.New())
    table.insert(self.ctrl_list, ActMeiLiQiXiCtrl.New())
    table.insert(self.ctrl_list, SeaChainBoxCtrl.New())
    table.insert(self.ctrl_list, SeaGiftBoxCtrl.New())
    table.insert(self.ctrl_list, SeaCustomizedBoxCtrl.New())
    table.insert(self.ctrl_list, YunZeChainBoxCtrl.New())
    table.insert(self.ctrl_list, YunZeGiftBoxCtrl.New())
    table.insert(self.ctrl_list, YunZeCustomizedBoxCtrl.New())
    table.insert(self.ctrl_list, EnjoyRedoubleCtrl.New())
    table.insert(self.ctrl_list, AllSoulsStorybookCtrl.New())
    table.insert(self.ctrl_list, ActFlowerTouchCtrl.New())
    table.insert(self.ctrl_list, FishCtrl.New())
    table.insert(self.ctrl_list, FlyUpCtrl.New())
    table.insert(self.ctrl_list, RoleXiuWeiCtrl.New())
    table.insert(self.ctrl_list, RoleAngrySoulCtrl.New())
    table.insert(self.ctrl_list, TradeRoadCtrl.New())
    table.insert(self.ctrl_list, MengChongJiaDaoCtrl.New())
    table.insert(self.ctrl_list, PetBagdeCtrl.New())
    table.insert(self.ctrl_list, HeroVanBattleCtrl.New())

    table.insert(self.ctrl_list, FightMirageTerritoryCtrl.New())
    table.insert(self.ctrl_list, ActTigerBornCtrl.New())
    table.insert(self.ctrl_list, NianNianYouYuCtrl.New())
    table.insert(self.ctrl_list, TigerMoraCtrl.New())
    table.insert(self.ctrl_list, TigerNewCowOldCtrl.New())
    table.insert(self.ctrl_list, TigerGiftMoneyCtrl.New())
    table.insert(self.ctrl_list, AuspiciousSnowYearCtrl.New())
    table.insert(self.ctrl_list, MarryCtrl.New())
    table.insert(self.ctrl_list, MarryPartyCtrl.New())
    table.insert(self.ctrl_list, MusicGameCtrl.New())
    table.insert(self.ctrl_list, LunJianCtrl.New())
    table.insert(self.ctrl_list, SportsCardsCtrl.New())
    table.insert(self.ctrl_list, MarryRechargeCtrl.New())
    table.insert(self.ctrl_list, ActEquipSmeltCtrl.New())
    table.insert(self.ctrl_list, FallenGodCtrl.New())
    table.insert(self.ctrl_list, PetRefiningCtrl.New())
    table.insert(self.ctrl_list, UniverseTeamArenaCtrl.New())
    table.insert(self.ctrl_list, PeakTokenCtrl.New())
    table.insert(self.ctrl_list, LoversMatchCtrl.New())
    table.insert(self.ctrl_list, GhostBustCtrl.New())
    table.insert(self.ctrl_list, RabbitRaceCtrl.New())
    table.insert(self.ctrl_list, LoversShopCtrl.New())
    table.insert(self.ctrl_list, ConcentricKnotCtrl.New())
    table.insert(self.ctrl_list, LuckyRingCtrl.New())
    table.insert(self.ctrl_list, LookingForRabbitsCtrl.New()) -- 三鲜鬼懂
    table.insert(self.ctrl_list, HundredGodBossCtrl.New())
    table.insert(self.ctrl_list, SkyLanternCtrl.New())
    table.insert(self.ctrl_list, CatchButterfliesCtrl.New())
    table.insert(self.ctrl_list, SowRewardCtrl.New())
    table.insert(self.ctrl_list, VitalityFlopCtrl.New())
    table.insert(self.ctrl_list, AllSoulCastCtrl.New())
    table.insert(self.ctrl_list, ClothAromaCtrl.New())
    table.insert(self.ctrl_list, YuanQiSpeedCtrl.New())
    table.insert(self.ctrl_list, DailyActGuideCtrl.New())
    table.insert(self.ctrl_list, MarbasCtrl.New()) -- 造化装备
    table.insert(self.ctrl_list, ShengQiCtrl.New())
    table.insert(self.ctrl_list, ThreeHundredSkinCtrl.New())
    table.insert(self.ctrl_list, NoonSachetCtrl.New())
    table.insert(self.ctrl_list, GoldRMinecCtrl.New())
    table.insert(self.ctrl_list, GotoSchoolCtrl.New())
    table.insert(self.ctrl_list, ShenYuCtrl.New())
    table.insert(self.ctrl_list, FairyDragonCtrl.New())
    table.insert(self.ctrl_list, SanXianGuiDongCtrl.New())
    table.insert(self.ctrl_list, StarMapCtrl.New())
    table.insert(self.ctrl_list, TaoQuanNaJiCtrl.New())
    table.insert(self.ctrl_list, MusicStationCtrl.New())
    table.insert(self.ctrl_list, YxdmCtrl.New())
    table.insert(self.ctrl_list, ActZyscCtrl.New())
    table.insert(self.ctrl_list, ActYxlcCtrl.New())
    table.insert(self.ctrl_list, HuanShouCtrl.New())
    table.insert(self.ctrl_list, RewardOnlineGiftCtrl.New())
    table.insert(self.ctrl_list, GiveHundredCtrl.New())
    table.insert(self.ctrl_list, LandsBetweenCtrl.New())
    table.insert(self.ctrl_list, LBFestivalCtrl.New())
    table.insert(self.ctrl_list, GodPosCtrl.New())
    table.insert(self.ctrl_list, GodAreasActsCtrl.New())
    table.insert(self.ctrl_list, RuinsCtrl.New())
    table.insert(self.ctrl_list, LBDragonPluseCtrl.New())
    table.insert(self.ctrl_list, ShenYuRankCtrl.New())
    table.insert(self.ctrl_list, LBHandBooksCtrl.New())
    table.insert(self.ctrl_list, LBShopCtrl.New())
    table.insert(self.ctrl_list, PalacesCtrl.New())
    table.insert(self.ctrl_list, ShenYuProbeCtrl.New())
    table.insert(self.ctrl_list, ActHalloweenCtrl.New())
    table.insert(self.ctrl_list, HalloweenPrayCtrl.New())
    table.insert(self.ctrl_list, QQBigPlayerCtrl.New())
    table.insert(self.ctrl_list, SpecialPowerCtrl.New())
    table.insert(self.ctrl_list, InviteFriendCtrl.New())
    table.insert(self.ctrl_list, WeekendOverChargeCtrl.New()) 
    table.insert(self.ctrl_list, YouMingCtrl.New())
    table.insert(self.ctrl_list, FavorCtrl.New())
    table.insert(self.ctrl_list, YingChunCtrl.New())
    table.insert(self.ctrl_list, XingXingDianDengCtrl.New())
end

function ModulesManager:onInitCtrls()
	local co = #self.ctrl_list
	for i = 1,co do
		if nil ~= self.ctrl_list[i].OnInit then
			self.ctrl_list[i]:OnInit()
		end
	end
end

function ModulesManager:Run()
	PreloadCtrl.Preload(function()
		local enter_func = function ()
            -- if BeginVideoScene.NeedPlayCG() then
            -- 	TimeHelper:AddDelayFrameTimer(function() SceneCtrl.Instance:GotoScene(BeginVideoScene) end,1)
            -- else
            if IS_AUDIT_VERSION or GLOBAL_CONFIG:PkgInfo().single then
                local function cb()
                    if GLOBAL_CONFIG:PkgInfo().mini_game_type == AUDIT_MINIGAME_TYPE.SANXIANGUIDONG then
                        SceneCtrl.Instance:GotoScene(SXGD_Scene)
                        return true
                    elseif GLOBAL_CONFIG:PkgInfo().mini_game_type == AUDIT_MINIGAME_TYPE.TAOQANNAJI then
                        SceneCtrl.Instance:GotoScene(TQNJ_Scene)
                        return true
                    elseif GLOBAL_CONFIG:PkgInfo().mini_game_type == AUDIT_MINIGAME_TYPE.YINYOU then
                        SceneCtrl.Instance:GotoScene(MUSICS_Scene)
                        return true
                    else
                        return false
                    end
                end
                if GLOBAL_CONFIG:Flags().need_login_mini_game then
                    local go_mini_game = false
                    
                    for _, v in pairs(AUDIT_MINIGAME_TYPE) do
                        if GLOBAL_CONFIG:PkgInfo().mini_game_type == v then
                            go_mini_game = true
                            --审核服小游戏虚假登录及上报
                            ChannelAgent:Login(function (data)
                                ChannelAgent.BehaviourTrigger(BehaviourType.CreateRole)
                                cb()
                                ChannelAgent.BehaviourTrigger(BehaviourType.RoleLogin)
                            end)
                            break
                        end
                    end

                    if not go_mini_game then
                        SceneCtrl.Instance:GotoScene(LoginScene)
                    end
                else
                    if not cb() then
                        SceneCtrl.Instance:GotoScene(LoginScene)
                    end
                end
            else
                SceneCtrl.Instance:GotoScene(LoginScene)
            end
            -- end	
        end
		if UnityEngine.SystemInfo.deviceModel == "Netease MuMu" then	--mumu模拟器先预热一下videoView,防止x86环境下异常闪退
			VideoCtrl.Instance:StartVideoById(7,function()
                    enter_func()
            end)
		else
			enter_func()
		end
	end)

	TimeHelper:AddRunTimer(function() 
            LuaMonoCheckAndCleanCache()
        end,lua_mono_auto_release_interval,-1,false)
end

function ModulesManager:deleteCtrls()
	local co = #self.ctrl_list
	for i = 1,co do
		self.ctrl_list[i]:DeleteMe()
	end
end

function ModulesManager:OnUnloadGameLogic()
    for _, v in pairs(self.ctrl_list) do
        if v.OnUnloadGameLogic == nil then
            Debuger.LogError("OnUnloadGameLogic no Exist===" .. GetClassName(v))
        else
            v:OnUnloadGameLogic()
        end
    end
    LuaMonoCheckAndCleanCache()
end

function ModulesManager:OnReboot()
    if self.low_mem_warning_func ~= nil then
        UnityApp.lowMemory = UnityApp.lowMemory - self.low_mem_warning_func
        self.low_mem_warning_func = nil
        -- LogError("Release MemoryWARING===222222222222222")
    end
    for _, v in pairs(self.ctrl_list) do
        if v.OnReboot == nil then
            Debuger.LogError("OnReboot no Exist===" .. GetClassName(v))
        else
            v:OnReboot()
        end
    end
end