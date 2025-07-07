FILE_REQ("modules/lun_jian/lun_jian_data")
VIEW_REQ("modules/lun_jian/lun_jian_advertising_view")
VIEW_REQ("modules/lun_jian/lun_jian_rank_panel")
VIEW_REQ("modules/lun_jian/lun_jian_reward_panel")
VIEW_REQ("modules/lun_jian/lun_jian_schedule_panel")
VIEW_REQ("modules/lun_jian/lun_jian_main_view")
VIEW_REQ("modules/lun_jian/lun_jian_report_view")
VIEW_REQ("modules/lun_jian/lun_jian_signup_view")
VIEW_REQ("modules/lun_jian/lun_jian_countdown_view")
VIEW_REQ("modules/lun_jian/lun_jian_ready_main_sub")

LunJianCtrl = LunJianCtrl or BaseClass(BaseCtrl)

LunJianCtrl.OP = {
    Info = 0,
    Signup = 1,
    ScheduleInfo = 2,
    UserBaseInfo = 3, --查看主角的基本信息
    EnterScene = 4, --前往赛场
    FetchWinBox = 5, --领取胜场宝箱，param1=sn宝箱配置中的sn序号
    GroupRankList = 6,--查询组别排行榜 param1=groupd_idx(级别序号[0,总分组数))
    StandbyForFight = 7,--在赛场中准备 param1 (1:准备,0:取消准备)
    FetchSignupReward = 8,--领取报名奖励
    FetchDailyReward = 9,--领取每日奖励
}

function LunJianCtrl:__init()
    if LunJianCtrl.Instance ~= nil then
        Debuger.LogError("[LunJianCtrl] attempt to create singleton twice!")
        return
    end
    LunJianCtrl.Instance = self
    self.wait_battle_end = nil
    self.data = LunJianData.New()
    self:RegisterProtocol(SCWorldArenaInfo, "OnSCWorldArenaInfo")
    self:RegisterProtocol(SCWorldArenaUserSignupInfo, "OnSCWorldArenaUserSignupInfo")
    self:RegisterProtocol(SCWorldArenaUserScheduleInfo, "OnSCWorldArenaUserScheduleInfo")
    self:RegisterProtocol(SCWorldArenaGroupRankList,"OnSCWorldArenaGroupRankList")
    self:RegisterProtocol(SCWorldArenaUserBaseInfo,"OnSCWorldArenaUserBaseInfo")
    self:RegisterProtocol(SCWorldArenaCountDownToFight,"OnSCWorldArenaCountDownToFight")
    self:RegisterProtocol(SCWorldArenaStartEnterSceneNotice,"OnSCWorldArenaStartEnterSceneNotice")
    self:RegisterProtocol(SCWorldArenaShowUpInfo,"OnSCWorldArenaShowUpInfo")
    self:RegisterProtocol(CSWorldArenaOperate)
    ActivityData.Instance:RegisterCondition(ActType.WorldArena,function()
        local cur_season = self.data:CurSeason()
        if cur_season <= 0 then
            return ""
        end
        if self.data:SeasonStatus() == LunJianData.Status.Signup or self.data:SeasonStatus() == LunJianData.Status.Schedule then
            return string.format(Language.LunJian.Challenge.TopSignup,DataHelper.GetDaXie(cur_season,false))
        elseif self.data:SeasonStatus() == LunJianData.Status.Battle then
            return string.format(Language.LunJian.Challenge.TopBattle,DataHelper.GetDaXie(cur_season,false))
        else
            return string.format(Language.LunJian.Challenge.TopRest,DataHelper.GetDaXie(cur_season,false))
        end
    end)
    
    ActivityData.Instance:RegisterTopCondition(ActType.WorldArena,function()
        local status = self.data:SeasonStatus()
        if status == LunJianData.Status.Battle then
            return Language.LunJian.Challenge.ButtomBattle
        elseif status == LunJianData.Status.Rest or status == LunJianData.Status.Calculate then
            local month = tonumber(os.date("%m"))
            if month >= 12 then
                month = 1
            else
                month = month + 1
            end
            return string.format(Language.LunJian.Challenge.ButtomRest,month)
        else
            return ""
        end
    end)
    ActivityData.Instance:CustomClickHandle(ActType.WorldArena,function()
        if LunJianData.Instance:IsBattleingTime() and not ViewMgr:IsOpen(AthleticsView) then    --在竞技界面点前往不进战斗，而是打开赛程
            PublicPopupCtrl.Instance:Center(Language.LunJian.FormationTip)
            ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_WORLD_ARENA)
            -- self:ReqGotoBattle()
            return
        end
        local status = self.data:SeasonStatus()
        if status == LunJianData.Status.Signup or status == LunJianData.Status.Schedule then
            ViewMgr:OpenView(LunJianSignupView)
        elseif status == LunJianData.Status.Rest then
            if LunJianData.Instance:HasReportData() then
                ViewMgr:OpenView(LunJianReportView)
            else
                PublicPopupCtrl.Instance:Center(Language.LunJian.NoReportDataHint)
            end
        else
            if LunJianData.Instance:HasSignuped() then
                ViewMgr:OpenView(LunJianMainView)
            else
                ViewMgr:OpenView(LunJianSignupView)
            end
        end
    end)
    --obj 是 UIWTimeMeter.New() stamp_time TimeType 自定
    ActivityRandData.Instance:RegisterActCountDown(Mod.LunJian.Signup, function (obj)
        local stamp_time = self.data:NextStatusTS()
        if obj ~= nil then
            obj:StampTime(stamp_time,TimeType.Type_Special_9, "")
        end
    end)
    --is_open true 显示图标 flase 隐藏图标 参数三是刷新源 暂时用了season_info
	ActivityRandData.Instance:Register(Mod.LunJian.Signup,function()
		local is_open = self.data:SeasonStatus() == LunJianData.Status.Signup
		return is_open
	end,self.data:SeasonInfo())

    ActivityData.Instance:RegisterRemind(ActType.WorldArena,BindTool.Bind(LunJianData.RemindFunc,self.data))
    ActivityData.Instance:GetActivityInfo(ActType.WorldArena).rp_ignore_status = true

    ActFormationData.Instance:RegistFunctions(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_WORLD_ARENA,{
        MainType = LunJianCtrl.Instance,
        challenge_func = "OnFormationChallenge",
        pet_on_limit_func = "PetOnLimitNum",
        assist_text_func = "AssistTextFunc",
    })

    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_WORLD_ARENA,function(is_win)
        ViewMgr:OpenView(LunJianMainView)
    end)
end

function LunJianCtrl:__delete()
    self:UnRegisterProtocol(SCWorldArenaInfo)
    self:UnRegisterProtocol(SCWorldArenaUserSignupInfo)
    self:UnRegisterProtocol(SCWorldArenaUserScheduleInfo)
    self:UnRegisterProtocol(SCWorldArenaGroupRankLis)
    self:UnRegisterProtocol(SCWorldArenaUserBaseInfo)
    self:UnRegisterProtocol(SCWorldArenaCountDownToFight)
    self:UnRegisterProtocol(SCWorldArenaStartEnterSceneNotice)
    self:UnRegisterProtocol(CSWorldArenaOperate)
    self:UnRegisterProtocol(SCWorldArenaShowUpInfo)

    self.data:DeleteMe()
    self.data = nil
    LunJianCtrl.Instance = nil
end

function LunJianCtrl:OnUnloadGameLogic()
	self.data:Clear()
    if self.wait_battle_end ~= nil then
        EventSys:UnBind(self.wait_battle_end)
        self.wait_battle_end = nil
    end
end

function LunJianCtrl:OnSCWorldArenaInfo(protc)
    local old_season = self.data:CurSeason()
    local old_status = self.data:SeasonStatus()
    self.data:OnSCWorldArenaInfo(protc)
    if old_status ~= protc.cur_status or protc.season_index ~= old_season then
        PosterHelper.OpenPoster()
    end
end

function LunJianCtrl:OnSCWorldArenaUserSignupInfo(protc)
    self.data:OnSCWorldArenaUserSignupInfo(protc)
end

function LunJianCtrl:OnSCWorldArenaUserScheduleInfo(protc)
    self.data:OnSCWorldArenaUserScheduleInfo(protc)
end

function LunJianCtrl:OnSCWorldArenaGroupRankList(protc)
    self.data:OnSCWorldArenaGroupRankList(protc)
end

function LunJianCtrl:OnSCWorldArenaUserBaseInfo(protc)
    self.data:OnSCWorldArenaUserBaseInfo(protc)
end

function LunJianCtrl:OnSCWorldArenaCountDownToFight(protc)
    ViewMgr:OpenView(LunJianCountdownView,{ts = protc.count_down_s  + TimeHelper:GetServerTime(),second = protc.count_down_s })
end

function LunJianCtrl:OnSCWorldArenaStartEnterSceneNotice(protc)
    self.data:OnSCWorldArenaStartEnterSceneNotice(protc)
    self:CheckBattleTipsHint()
end

function LunJianCtrl:OnSCWorldArenaShowUpInfo(protc)
    self.data:OnSCWorldArenaShowUpInfo(protc)
end

function LunJianCtrl:ReqOperate(oper_type,param1,param2)
	local protocol = GetProtocol(CSWorldArenaOperate)
    protocol.oper_type = oper_type
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

function LunJianCtrl:ReqSignup()
    self:ReqOperate(LunJianCtrl.OP.Signup)
end

function LunJianCtrl:ReqSchedule()
    self:ReqOperate(LunJianCtrl.OP.ScheduleInfo)
end

function LunJianCtrl:ReqFetchSignupReward()
    self:ReqOperate(LunJianCtrl.OP.FetchSignupReward)
end

function LunJianCtrl:ReqGetWinReward(sn)
    self:ReqOperate(LunJianCtrl.OP.FetchWinBox,sn)
end

function LunJianCtrl:ReqGotoBattle()
    local param_t = {
        content = Language.LunJian.GotoBattleHint,
        confirm = {
            func = function()
                ViewMgr:CloseView(DialogTipsView)
                self:ReqOperate(LunJianCtrl.OP.EnterScene)
                ViewMgr:CloseView(ActFormationView)
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)       
end

function LunJianCtrl:ReqRankList(group_id)
    self:ReqOperate(LunJianCtrl.OP.GroupRankList,group_id-1)    --服务端group_id从0开始数
end

function LunJianCtrl:ReqMyInfo()
    self:ReqOperate(LunJianCtrl.OP.UserBaseInfo)
end

function LunJianCtrl:ReqSeasonInfo()
    self:ReqOperate(LunJianCtrl.OP.Info)
end

function LunJianCtrl:ReqFetchDailyReward()
    self:ReqOperate(LunJianCtrl.OP.FetchDailyReward)
end

function LunJianCtrl.OpenMainView(tab)
    if tab ~= nil then
        ViewMgr:OpenView(LunJianMainView,{open_tab = tab})
    else
        ViewMgr:OpenView(LunJianMainView)
    end
end

function LunJianCtrl:CheckBattleTipsHint()
    if not FunOpen.Instance:IsFunOpen(Mod.LunJian.Main) then
        return
    end
    local cur_status = self.data:SeasonStatus()
    if cur_status == LunJianData.Status.Signup or 
        cur_status == LunJianData.Status.Schedule or 
        cur_status == LunJianData.Status.Battle then
        local is_battleing_time = LunJianData.Instance:IsBattleingTime()
        if is_battleing_time then   --可以进战斗的时候提示是否进战斗

            local hint_func = function ()
                local param_t = {
                    content = Language.LunJian.GotoBattleDialog,
                    confirm = {
                        func = function()
                            ViewMgr:CloseView(DialogTipsView)
                            -- self:ReqOperate(LunJianCtrl.OP.EnterScene)
                            PublicPopupCtrl.Instance:Center(Language.LunJian.FormationTip)
                            ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_WORLD_ARENA)
                        end
                    }
                }
                PublicPopupCtrl.Instance:DialogTips(param_t)
            end
            
            if BattleData.Instance:IsServerFighting() then  --战斗中等战斗结束了再提示
                --锢魔之塔，无尽之塔，轮回忘川如果在自动战斗中，则取消自动战斗
                if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE
                or BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_NEW_COURAGE_CHALLENGE then
                    if PrefsInt(PrefKeys.CourageChallengeAuto()) == 1 then
                        PrefsInt(PrefKeys.CourageChallengeAuto(),0)
                    end
                elseif BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_ENDLESS_TOWER then
                    if PrefsInt(PrefKeys.EndlessTowerAuto()) == 1 then
                        PrefsInt(PrefKeys.EndlessTowerAuto(),0)
                    end
                elseif BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_CHALLENGE_TEST then
                    if PrefsInt(PrefKeys.ChallengeTestAuto(RoleData.Instance:GetRoleId())) == 1 then
                        PrefsInt(PrefKeys.ChallengeTestAuto(RoleData.Instance:GetRoleId()),0)
                    end
                end
                self.wait_battle_end = EventSys:Bind(GlobalEventKeys.BattleEnd,
                    function()    
                        if LunJianData.Instance:IsBattleingTime() then
                            hint_func()
                        end
                        EventSys:UnBind(self.wait_battle_end)
                        self.wait_battle_end = nil
                    end
                )
            else
                hint_func()
            end


        end
    end
end

--可助战宠物数量
function LunJianCtrl:PetOnLimitNum()
    return 4
end

function LunJianCtrl:AssistTextFunc(assist_num)
    return string.format(Language.CloudArena.FormationAssistInfo, assist_num)
end

function LunJianCtrl:OnFormationChallenge()
    self:ReqGotoBattle()
end