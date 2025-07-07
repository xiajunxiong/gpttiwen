function PrefKeys.AchieveProgress(role_id,seq)
    return "id"..role_id.."progress"..seq
end

function PrefKeys.AchieveFetch(role_id,seq)
    return "id"..role_id.."fetch"..seq
end

function PrefKeys.ActivityIsRemind(role_id,seq)
    return role_id .. "_ActivityInfo_IsRemind_" .. seq
end

function PrefKeys.ChatChannelIsSel(role_id,channel_id)
    return role_id .. "_ChatChannel_IsSel_" .. channel_id
end

function PrefKeys.ChatMsgTypeIsSel(role_id,msg_type)
    return role_id .. "_ChatMsgType_IsSel_" .. msg_type
end

function PrefKeys.TodayGuildCooRemind()
    return RoleData.Instance:GetRoleId() .. "_TodayGuildCooRemind"
end

function PrefKeys.TodayRemind(str)
    return RoleData.Instance:GetRoleId() .. "_" .. str
end

function PrefKeys.SettingVal(key)
    return "RoleSetting_" .. key
end

function PrefKeys.SuperMapGuide(role_id)
    return role_id.."_supermap"
end

function PrefKeys.MainTaskEliteFirst(chapter,role_id)
    return chapter.."_is_first_"..role_id
end

function PrefKeys.MainTaskEliteFirstShow(role_id)
    return "MainTask_is_first_"..role_id
end

function PrefKeys.PhasePkgCachedI(phase_key)
    return "PhasePkgCachedI_" .. phase_key
end

function PrefKeys.ActTypeClickTime(act_type)
    return "limit_act_"..act_type
end

function PrefKeys.GuMoGuide(layer)
    return "gumo_guide_"..layer..RoleData.Instance:GetRoleId()
end

function PrefKeys.MedalChallengeClick(role_id)
    return "MedalChallenge_Click_"..role_id
end

--本地记录当天是否已经弹出过日常检查
function PrefKeys.ResearchDailyDegree(role_id)
    return "ResearchDailyDegree_"..role_id
end

function PrefKeys.FightCrystal()
    return "fight_crystal_"..RoleData.Instance:GetRoleId()
end

function PrefKeys.RechargeTip()
    return "recharge_tip_"..RoleData.Instance:GetRoleId()
end

function PrefKeys.BattlefieldMatch()
    return "BattlefieldMatch" .. RoleData.Instance:GetRoleId()
end

function PrefKeys.GoPatrol()
    return "go_patrol"..RoleData.Instance:GetRoleId()
end

function PrefKeys.GetMedalBuildMark()
    return "MedalBuildMark" .. RoleData.Instance:GetRoleId()
end

function PrefKeys.ActNew(act_type)
    return "act_new_click_" .. act_type .. RoleData.Instance:GetRoleId()
end

function PrefKeys.ActReward(act_type)
    return "act_reward_" .. act_type .. RoleData.Instance:GetRoleId()
end

function PrefKeys.HeadTips(type,key)
    return "HeadTips" .. RoleData.Instance:GetRoleId() .. type .. key
end

function PrefKeys.SmeltFunOpenMark(role_id)
    return "Smelt_FunOpen_Mark"..role_id
end

function PrefKeys.LoginNoticeNews(account)
    return "LoginNoticeNews"..account
end

function PrefKeys.AppearanceTips(seq)
    return "AppearanceTips".. RoleData.Instance:GetRoleId() .. seq
end

function PrefKeys.CourageChallengeTip(level)
    return "CourageChallengeTip" .. RoleData.Instance:GetRoleId() .. (level or 0)
end
function PrefKeys.DailyTask(task_type, time)
    return "DailyTaskFlag"..task_type..RoleData.Instance:GetRoleId()..time
end
function PrefKeys.TaskGuideAuto(seq)
    return "TaskGuideAuto"..RoleData.Instance:GetRoleId()..seq
end

function PrefKeys.ChallengeTest(role_id)
    return "ChallengeTest"..role_id
end

function PrefKeys.DeepDarkFantasy(role_id)
    return "DeepDarkFantasy"..role_id
end

function PrefKeys.ChallengeTestAuto(role_id)
    return "ChallengeTestAuto"..role_id
end

function PrefKeys.CourageChallengeAuto()
    return "CourageChallengeAuto"..RoleData.Instance:GetRoleId()
end

function PrefKeys.EndlessTowerAuto()
    return "EndlessTowerAuto"..RoleData.Instance:GetRoleId()
end

function PrefKeys.PartnerBiographyGuide()
    return "PartnerBiographyGuide"..RoleData.Instance:GetRoleId()
end

function PrefKeys.SecondSkillGuide()
    return "SecondSkillGuide"..RoleData.Instance:GetRoleId()
end

function PrefKeys.SecondPetGuide()
    return "SecondPetGuide"..RoleData.Instance:GetRoleId()
end

function PrefKeys.GuildShopItem1Click2()
    return "GuildShopItem1TodayClickTimes"..RoleData.Instance:GetRoleId()
end

function PrefKeys.GuildSkillIsRemind()
    return RoleData.Instance:GetRoleId() .. "_GuildSkillInfo_IsRemind"
end

function PrefKeys.MonthCardRemind()
    return RoleData.Instance:GetRoleId() .. "_MonthCard_IsRemind"
end

function PrefKeys.PaiMaiHangRemind()
    return RoleData.Instance:GetRoleId() .. "_PaiMaiHang_IsRemind"
end

function PrefKeys.StoryCustomBullet()
    return "StoryBullet"..LoginData.Instance:GetPlatformName()    
end

function PrefKeys.GodLandTracesGuide(role_id)
    return "GodLandTracesGuide"..role_id
end

function PrefKeys.ActOpenRemindSp(act_type)
    return "ActOpenRemind" .. act_type .. RoleData.Instance:GetRoleId()
end

function PrefKeys.ThrivingIsRemind()
    return RoleData.Instance:GetRoleId() .. "_Thriving_IsRemind_"..TimeManager.DayStart()
end

function PrefKeys.FightSoulRemind()
    return "FightSoulRemind"..RoleData.Instance:GetRoleId()
end

function PrefKeys.TeamNotice(role_id)
    return "TeamNotice"..role_id
end

function PrefKeys.HorcruxTianchengSkip()
    return "HorcruxTianchengSkip"..RoleData.Instance:GetRoleId()
end

-- 是否第一次通过心灵之境第一层
function PrefKeys.FightSoulFirstClear()
    return "FightSoulFirstClear"..RoleData.Instance:GetRoleId()
end

function PrefKeys.DreamNotesFirstGuid()
    return "DreamNotesFirstGuid"..RoleData.Instance:GetRoleId()
end

function PrefKeys.WeddingFBBlessEffect()
    return "WeddingFBBlessEffect"..RoleData.Instance:GetRoleId()
end

function PrefKeys.PartnerSoarWeaponConsume()
    return "PartnerSoarWeaponConsume"..RoleData.Instance:GetRoleId()
end

function PrefKeys.LunJianHinted()
    return "LunJianHinted".. RoleData.Instance:GetRoleId()
end

function PrefKeys.ActMarryRecharge()
    return "ActMarryRecharge" .. RoleData.Instance:GetRoleId()
end

-- 天下第一比武大会海报
function PrefKeys.UniverseTeamArenaPoster()
    return "UniverseTeamArenaPoster" .. RoleData.Instance:GetRoleId()
end

-- 周期活动海报
function PrefKeys.PeriodActivityPoster()
    return "PeriodActivityPoster" .. RoleData.Instance:GetRoleId()
end

-- 周期活动
function PrefKeys.PeriodActivityFirst()
    return "PeriodActivityFirst" .. RoleData.Instance:GetRoleId()
end


--仙子去哪儿
function PrefKeys.FairyWhere()
    return [[FairyWhere]] .. RoleData.Instance:GetRoleId()
end

--仙子去哪儿
function PrefKeys.FairyWhereType()
    return [[FairyWhereType]] .. RoleData.Instance:GetRoleId()
end

--仙子去哪儿
function PrefKeys.FairyWhereTask()
    return [[FairyWhereTask]] .. RoleData.Instance:GetRoleId()
end

function PrefKeys.GhostBustAuto()
    return "GhostBustAuto"..RoleData.Instance:GetRoleId()
end

function PrefKeys.LoversMatchTips()
    return "LoversMatchTips" .. RoleData.Instance:GetRoleId()
end

function PrefKeys.LuckyRingSynopsisView()
    return "LuckyRingSynopsisView"..RoleData.Instance:GetRoleId()
end
function PrefKeys.LianYaoLuziOpen(id)
    return "LianYaoLuziOpen"..id..RoleData.Instance:GetRoleId()
end

function PrefKeys.AnecdoteHintGuide()
    return "AnecdoteHintGuide"..RoleData.Instance:GetRoleId()
end
function PrefKeys.HundredGodBossGuide(id)
    return "HundredGodBossGuide"..id..RoleData.Instance:GetRoleId()
end

-- 用于技能页面的toggle勾选
function PrefKeys.RoleSkillAllLearn()
    return "RoleSkillAllLearn" .. RoleId()
end

function PrefKeys.ActiveGuide()
    return "ActiveGuide"..RoleData.Instance:GetRoleId()
end

function PrefKeys.VideoGuide()
    return "VideoGuide"..RoleData.Instance:GetRoleId()
end

function PrefKeys.SkyLanternDayRemind()
    return "SkylanternDayRemind" .. RoleData.Instance:GetRoleId()
end

function PrefKeys.ActiveDailyGuide()
    return "ActiveDailyGuide"..RoleData.Instance:GetRoleId()
end

function PrefKeys.DivineCostumeGuide()
    return "DivineConsumeShow" .. RoleData.Instance:GetRoleId()
end

-- 造化打造红点提示
function PrefKeys.MarbasBuildRemind()
    return "MarbasBuildRemind" .. RoleId()
end

-- 造化打造当前可打造
function PrefKeys.MarbasBuildCanBuild()
    return "MarbasBuildCanBuild" .. RoleId()
end

--首次打开炼妖界面指引
function PrefKeys.LianYaoFirstOpen()
    return "LianYaoFirstOpen" .. RoleData.Instance:GetRoleId()
end

function PrefKeys.TreasureHuntPetPrayEff()
    return "TreasureHuntPetPrayEff"..RoleData.Instance:GetRoleId()
end

function PrefKeys.GodBeastGraceAct()
    return "GodBeastGraceAct"..RoleData.Instance:GetRoleId()
end

function PrefKeys.GodBeastGraceSelected()
    return "GodBeastGraceSelected"..RoleData.Instance:GetRoleId()
end

function PrefKeys.LoginPKToggle()
    return "LoginPKToggle" .. LoginData.Instance:GetAccountKey()
end

function PrefKeys.LoginUserProtocol()
    return "LoginUserProtocol" .. LoginData.Instance:GetAccountKey()
end

--首次打开侠义指引
function PrefKeys.XiaYiFirstGuide()
    return "XiaYiFirstGuide".. RoleData.Instance:GetRoleId()
end
--首次卡级指引
function PrefKeys.TaskBoxGuide()
    return "TaskBoxGuide"..RoleData.Instance:GetRoleId()
end

function PrefKeys.KFHeroRecordFirst()
    return "KFHeroRecordFirst"..RoleData.Instance:GetRoleId()
end

function PrefKeys.HunQiFanLiFirst()
    return "HunQiFanLi"..RoleData.Instance:GetRoleId()
end

-- 活动首次红点
function PrefKeys.ActivityFirstRemind(act_type)
    return "ActivityFirstRemind_"..RoleData.Instance:GetRoleId().."_"..act_type
end

function PrefKeys.MiniGame2JudgeClose()
    return "MiniGame2JudgeClose"..RoleData.Instance:GetRoleId()
end

function PrefKeys.AccountLoginTimes()
    return "AccountLoginTimes"..LoginData.Instance:GetAccountKey()
end