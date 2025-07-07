FinishConfig = {
    --单人副本
    IsNoTeam = {
        [BATTLE_MODE.BATTLE_MODE_ACHIEVE] = true,
        [BATTLE_MODE.BATTLE_MODE_CRYSTAL_FB] = true,
        [BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE] = true,
        [BATTLE_MODE.BATTLE_MODE_SUPER_MAP] = true,
        [BATTLE_MODE.BATTLE_MODE_NPC_FIGHT] = true,
        [BATTLE_MODE.BATTLE_MODE_NEW_COURAGE_CHALLENGE] = true,
    },
    --不显示成功
    IsWinHide = {
        [BATTLE_MODE.BATTLE_MODE_NPC_FIGHT] = true,
        [BATTLE_MODE.BATTLE_MODE_TREASURE_MAP] = true,
        [BATTLE_MODE.BATTLE_MODE_ZHEN_ZHONG] = true,
        [BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE] = true,
        [BATTLE_MODE.BATTLE_MODE_BATTLEFIELD] = true,
        [BATTLE_MODE.BATTLE_MODE_SHITU_FB] = true,
        [BATTLE_MODE.BATTLE_MODE_TASK_CHAIN] = true,
        [BATTLE_MODE.BATTLE_MODE_CATCH_FOX] = true,
        [BATTLE_MODE.BATTLE_MODE_NETHER_WORLD_PURGATORY] = true,
        [BATTLE_MODE.BATTLE_MODE_CHALLENGE_TEST] = true,
        [BATTLE_MODE.BATTLE_MODE_MATERIAL] = true,
        [BATTLE_MODE.BATTLE_MODE_SUMMER_LI_LIAN] = true,
        [BATTLE_MODE.BATTLE_MODE_NEW_COURAGE_CHALLENGE] = true,
    },
    --都不显示
    IsHide = {
        [BATTLE_MODE.BATTLE_MODE_WAVE] = true,
        [BATTLE_MODE.BATTLE_MODE_ARENA] = true,
        [BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE] = true,
        [BATTLE_MODE.BATTLE_MODE_GUILD_HONOR_BATTLE] = true,
        [BATTLE_MODE.BATTLE_MODE_CHIEF_ELECTION] = true,
        [BATTLE_MODE.BATTLE_MODE_DU_CHUANG_ZEI_YING] = true,
        [BATTLE_MODE.BATTLE_MODE_REALM_BREAK] = true,
        [BATTLE_MODE.BATTLE_MODE_PET_MONOPOLY] = true,
        [BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT] = true,
        [BATTLE_MODE.BATTLE_MODE_MAZE_SUMMER] = true,
        [BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER] = true,
        [BATTLE_MODE.BATTLE_MODE_ML_LANTERN] = true,
        [BATTLE_MODE.BATTLE_MODE_IMPERIAL_EXAM] = true,
        [BATTLE_MODE.BATTLE_MODE_UTA] = true,
        [BATTLE_MODE.BATTLE_MODE_MINI_GAME_BATTLE] = true,
        [BATTLE_MODE.GOD_SPACE_OCCUPY_ALTAR] = true,
        [BATTLE_MODE.DRAGON_VEIN_BOSS] = true,
        [BATTLE_MODE.DRAGON_VEIN_MONSTER] = true,
        [BATTLE_MODE.GUI_XU_ZHI_JING_BOSS] = true,
        [BATTLE_MODE.YUN_DING_TIAN_GONG_ATTACK_BOSS] = true,
    },
    -- 魔将入侵，巨龙来袭 失败替换为结束
    -- 2022-6-1 bugcloseID#58743 
    ServerBoss = {
        [BATTLE_MODE.BATTLE_MODE_SERVER_BOSS] = true,
        [BATTLE_MODE.BATTLE_MODE_SERVER_BOSS2] = true,
        [BATTLE_MODE.BATTLE_MODE_SERVER_BOSS3] = true,
        [BATTLE_MODE.GUI_XU_ZHI_JING_BOSS] = true,
    },
    --其他通过公共协议打开自己的结算界面
    FinishFunc = {
        [BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE] = function(is_win)
            ArenaMatchCtrl.Instance:FightFinish(1 == is_win)
        end,
        [BATTLE_MODE.BATTLE_MODE_ACHIEVE] = function(is_win)
            if is_win == 1 then ViewMgr:OpenView(AchieveNewView)end
        end,
        [BATTLE_MODE.BATTLE_MODE_REALM_BREAK] = function(is_win)
            RoleData.Instance.break_info.state = is_win
            ViewMgr:OpenView(OfflineRewardView)
        end,
        [BATTLE_MODE.BATTLE_MODE_SUMMER_LI_LIAN] = function(is_win)
            if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SUMMER_EXPERIENCE) and is_win ~= 1 then
                ViewMgr:OpenViewByKey(Mod.SummerFirst.SummerExperience)
            end
        end,
        [BATTLE_MODE.BATTLE_MODE_SHAN_HAI_ZHENG_FENG] = function(is_win)
            if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SUMMER_EXPERIENCE_TWO) and is_win ~= 1 then
                ViewMgr:OpenViewByKey(Mod.MountainSea.SummerExperienceTwo)
            end
        end,
        [BATTLE_MODE.BATTLE_MODE_PET_MONOPOLY] = function (is_win)
            if FightData.Instance:CheckPetMoCanOpenView() then
                FightCtrl.Instance:OpenPetIsland()
            else
                FinishData.Instance:SetBattleMode(BATTLE_MODE.BATTLE_MODE_PET_MONOPOLY)
            end
        end,
        [BATTLE_MODE.BATTLE_MODE_MATERIAL] = function(is_win)
            ViewMgr:OpenView(MaterialChallengeFinishView,MaterialCtrl.Instance:GetCurrData())
        end,
        [BATTLE_MODE.BATTLE_MODE_CATCH_FOX] = function(is_win)
            if is_win == 1 and CatchFoxData.SendTaskFinishReq then
                Invoke(function()
                    CatchFoxData.SendTaskFinishReq()
                    CatchFoxData.SendTaskFinishReq = nil
                end)
            end
        end,
        [BATTLE_MODE.BATTLE_MODE_NETHER_WORLD_PURGATORY] = function(is_win)
            ViewMgr:OpenView(DeepDarkFantasyFinishView)
        end,
        [BATTLE_MODE.BATTLE_MODE_MAZE_SUMMER] = function(is_win)
            if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_SUMMER_MAZE) then
                ViewMgr:OpenView(SummerMazeView)
            end
        end,
        [BATTLE_MODE.BATTLE_MODE_CHALLENGE_TEST] = function(is_win, param_t, mode)
            if is_win == 1 then
                if 1 == UnityPlayerPrefs.GetInt(PrefKeys.ChallengeTestAuto(RoleData.Instance:GetRoleId())) then
                    ViewMgr:OpenView(FinishPassViewCT)
                else
                    FinishCtrl.Instance:Finish(param_t, mode)
                end
            end
        end,
        [BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER] = function(is_win, param_t, mode)
            FinishCtrl.Instance:Finish(param_t, mode,PrefsInt(PrefKeys.EndlessTowerAuto())==0)
        end,
        [BATTLE_MODE.BATTLE_MODE_ML_LANTERN] = function(is_win, param_t, mode)
            ViewMgr:OpenView(MSLantrenOverView, {is_win = is_win})
        end,
        [BATTLE_MODE.BATTLE_MODE_IMPERIAL_EXAM] = function(is_win, param_t, mode)
            ViewMgr:OpenView(ImperialExamOverView, {is_win = is_win})
        end,
        [BATTLE_MODE.BATTLE_MODE_MINI_GAME_BATTLE] = function(is_win, param_t, mode)
            MiniGameData.Instance:DealBattleEnd(is_win == 1)
        end,
        [BATTLE_MODE.GOD_SPACE_OCCUPY_ALTAR] = function(is_win, param_t, mode)
            ViewMgr:CloseAllViewInBattle()
            LandsBetweenData.Instance:TryChangeMainInitType(true)
            -- ViewMgr:OpenView(LandsBetweenMainView)
            
            LandsBetweenCtrl.Instance:TryOpenMain()
            ViewMgr:OpenView(is_win == 1 and FinishPassView or FinishFailView)
            -- MiniGameData.Instance:DealBattleEnd(is_win == 1)
        end,
        [BATTLE_MODE.DRAGON_VEIN_MONSTER] = function(is_win, param_t, mode)
            ViewMgr:CloseAllViewInBattle()
            LandsBetweenData.Instance:TryChangeMainInitType(true)
            LandsBetweenCtrl.Instance:TryOpenMain()
            ViewMgr:OpenView(is_win == 1 and FinishPassView or FinishFailView)
            -- MiniGameData.Instance:DealBattleEnd(is_win == 1)
        end,
        [BATTLE_MODE.DRAGON_VEIN_BOSS] = function(is_win, param_t, mode)
            ViewMgr:CloseAllViewInBattle()
            LandsBetweenData.Instance:TryChangeMainInitType(true)
            LandsBetweenCtrl.Instance:TryOpenMain()
            ViewMgr:OpenView(is_win == 1 and FinishPassView or FinishFailView)
            -- MiniGameData.Instance:DealBattleEnd(is_win == 1)
        end,
        [BATTLE_MODE.GUI_XU_ZHI_JING_BOSS] = function()
            FinishData.Instance:SetBattleMode(BATTLE_MODE.GUI_XU_ZHI_JING_BOSS)
            ViewMgr:OpenView(FinishPassView)
        end,
        [BATTLE_MODE.YUN_DING_TIAN_GONG_ATTACK_BOSS] = function()
            FinishData.Instance:SetBattleMode(BATTLE_MODE.YUN_DING_TIAN_GONG_ATTACK_BOSS)
            ViewMgr:OpenView(FinishPassView)
        end
    },
    --某怪物组战斗结束后的行为配置，优先级最高，这里的怪物组战斗后不会显示结算界面
    MonsterGroupFinishFunc = {
        [BATTLE_MONSTER_GROUP.FirstXinMo] = function (is_win, param_t, mode)
            -- GuideManager.Instance:Start(76)
        end
    },
    --平局通知结果
    IsDrawResult = {
        [RESULT_TYPE.DRAW] = true,
        [RESULT_TYPE.EXCEED_MAX_ROUND] = true,
        [RESULT_TYPE.EXCEED_MAX_TIME] = true,
    },
    --需要有特殊平局的模块
    IsDrawConfig = {
        [BATTLE_MODE.BATTLE_MODE_GUILD_FIGHT] = true,
		[BATTLE_MODE.BATTLE_MODE_BATTLEFIELD] = true,
		[BATTLE_MODE.BATTLE_MODE_CHIEF_PRIMARY] = true,
		[BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA] = true,
		[BATTLE_MODE.BATTLE_MODE_JOYSEEKS] = true,
		[BATTLE_MODE.BATTLE_MODE_TEAM_SPORT] = true,
        [BATTLE_MODE.BATTLE_MODE_WORLD_ARENA] = true,
        [BATTLE_MODE.BATTLE_MODE_WORLD_TEAM_AREMA] = true,
    },    
    --战力统计结算界面
    HurtView = {
        -- [BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE] = true,
        [BATTLE_MODE.BATTLE_MODE_ACHIEVE] = true,
        [BATTLE_MODE.BATTLE_MODE_NORMAL_MONSTER] = true,
        [BATTLE_MODE.BATTLE_MODE_ARENA] = true,
        [BATTLE_MODE.BATTLE_MODE_CRYSTAL_FB] = true,
        [BATTLE_MODE.BATTLE_MODE_TEAM_SPORT] = true,
        [BATTLE_MODE.BATTLE_MODE_GUILD_FIGHT] = true,
        [BATTLE_MODE.BATTLE_MODE_BRAVE_GTOUND] = true,
        [BATTLE_MODE.BATTLE_MODE_DREAM_NOTES_CHALLENGE] = true,
        [BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA] = true,
        [BATTLE_MODE.BATTLE_MODE_MATERIAL] = true,
        [BATTLE_MODE.BATTLE_MODE_GO_THROUGH] = true,
        [BATTLE_MODE.BATTLE_MODE_REALM_BREAK_FIGHT] = true,
        -- [BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER] = true,
        [BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY] = true,
        [BATTLE_MODE.BATTLE_MODE_FALLEN_GOD] = true,
        [BATTLE_MODE.GUI_XU_ZHI_JING_BOSS] = true,
        [BATTLE_MODE.YUN_DING_TIAN_GONG_ATTACK_BOSS] = true,
        -- [BATTLE_MODE.BATTLE_MODE_YAO_XING_SHI_LIAN] = true,
        [BATTLE_MODE.BATTLE_MODE_TEAM_CHALLENGE_TASK] = true,
    },
    --HurtCustom中的value对应FinishHurtPanel中CustomInfoArray数组中的序号
    HurtCustom = {
        [BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA] = 1,
    },
    CloseTime = {
        [true] = 10,
        [false] = 10,
        [SCENE_TYPE.MIYI] = 2,
    },
}