FILE_REQ("modules/cloud_arena/cloud_arena_data")
VIEW_REQ("modules/cloud_arena/cloud_arena_view")
VIEW_REQ("modules/cloud_arena/cloud_arena_rank_view")
VIEW_REQ("modules/cloud_arena/cloud_arena_reward_view")
VIEW_REQ("modules/cloud_arena/cloud_arena_report_view")
VIEW_REQ("modules/cloud_arena/cloud_arena_achievement_view")

CLOUD_ARENA_REQ_TYPE = {
    OPER_TYPE_SELF_INFO = 0,             --获取自身信息
    OPER_TYPE_TARGET_INFO = 1,           --获取目标信息
    OPER_TYPE_REFRESH_TARGET = 2,        --刷新目标列表
    OPER_TYPE_START_FIGHT = 3,           --开始挑战     param1：挑战第几个目标[0,4)
    OPER_TYPE_BUY_FIGHT_TIMES = 4,       --购买挑战次数
    OPER_TYPE_RECORD_LIST = 5,           --请求战报列表 param1：从第几条战报开始 param2:请求多少条战报
    OPER_TYPE_RANK_LIST = 6,             --排行信息     param1：排行榜类型（0:总榜 1:本服榜 2:段位榜）
    OPER_TYPE_FETCH_RANING_REWARD = 7,   --领取段位奖励 param1：段位
    OPER_TYPE_OPEN_UI = 8,               --查询是否是当前赛季第一次打开
    OPER_TYPE_ACHIEVEMENT_INFO = 9,      --获取成就信息
    OPER_TYPE_FETCH_ACHIEVE = 10,        --领取成就奖励 param1：成就类型[0,10)
    OPER_TYPE_FETCH_DAY_REWARD = 11,     --领取日奖励
    OPER_TYPE_REQ_FORMATION = 12,        --穹顶布阵信息请求
}

CloudArenaCtrl = CloudArenaCtrl or BaseClass(BaseCtrl)

function CloudArenaCtrl:__init()
    if CloudArenaCtrl.Instance ~= nil then
        Debuger.LogError("[CloudArenaCtrl] attempt to create singleton twice!")
        return
    end
    CloudArenaCtrl.Instance = self
    self.data = CloudArenaData.New()
    self:RegisterAllProtocols()

    ActivityData.Instance:CustomClickHandle(ActType.CloudArena,function()
        if TombRaiderData.Instance:InTombRaider() then 
            PublicPopupCtrl.Instance:Center(Language.TombRaider.CloudCancel)  
            return 
        end 
        self:OpenCloudArenaView()
    end)
    ActivityData.Instance:CustomClickHandle(ActType.CloudArenaDaily,function()
        self:OpenCloudArenaView()
    end)
    ActivityData.Instance:RegisterCondition(ActType.CloudArena,function()
        return Language.CloudArena.CurRank .. self.data:GetCurRanking()
    end)
    ActivityData.Instance:RegisterTopCondition(ActType.CloudArena,function()
        return self.data:GetTopConditionTip()
    end)
    ActivityData.Instance:RegisterRemind(ActType.CloudArena,function()
        if ActivityData.Instance:IsNoEnoughLevel(ActType.CloudArena,true) then
            return 0
        end
        return self.data:CloudArenaRemind()
    end)
    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA, function()
        self:OpenCloudArenaView()
        -- ViewMgr:OpenView(CloudArenaView)
    end)
    ActivityRandData.Instance:Register(Mod.CloudArena.Main,function ()
        return self.data:GetShowSeasonLeftDay()
    end,self.data.main_view_btn_check)

    ActFormationData.Instance:RegistFunctions(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_CLOUD_ARENA, 
       {MainType = CloudArenaCtrl.Instance, challenge_func = "CloudArenaChallenge", pet_on_limit_func = "PetOnLimitNum", assist_text_func = "AssistTextFunc"}
       )
end

function CloudArenaCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCCloudArenaSelfInfo, "OnSCCloudArenaSelfInfo")
    self:RegisterProtocol(SCCloudArenaTargetInfo, "OnSCCloudArenaTargetInfo")
    self:RegisterProtocol(SCCloudArenaNewFightRecordNotice,"OnSCCloudArenaNewFightRecordNotice")
    self:RegisterProtocol(SCCloudArenaFightRecordList,"OnSCCloudArenaFightRecordList")
    self:RegisterProtocol(SCCloudArenaRankList,"OnSCCloudArenaRankList")
    self:RegisterProtocol(SCCloudArenaNoticeSeasonInitialScore,"OnSCCloudArenaNoticeSeasonInitialScore")
    self:RegisterProtocol(SCCloudArenaNoticeDanChange,"OnSCCloudArenaNoticeDanChange")
    self:RegisterProtocol(SCCloudArenaAchievementInfo,"OnSCCloudArenaAchievementInfo")
    self:RegisterProtocol(SCCloudArenaAchievementUpdateNotice,"OnSCCloudArenaAchievementUpdateNotice")
    self:RegisterProtocol(SCCloudArenaRoleSeasonHighestData, "OnSCCloudArenaRoleSeasonHighestData")
    self:RegisterProtocol(SCCloudArenaRealTimeInfo, "OnSCCloudArenaRealTimeInfo")
    self:RegisterProtocol(SCCloudArenaFormationInfo, "OnSCCloudArenaFormationInfo")
    self:RegisterProtocol(CSCloudArenaOperate)
    self:RegisterProtocol(CSCloudArenaRealTimeInfo)
    self:RegisterProtocol(CSCloudArenaSetFormation)
end

function CloudArenaCtrl:OnInit()
    ViewMgr:RegisterOpenKeyFunc(Mod.CloudArena.Main, function ()
        self:OpenCloudArenaView()
    end)
end

function CloudArenaCtrl:__delete()
    self:UnRegisterProtocol(SCCloudArenaSelfInfo)
    self:UnRegisterProtocol(SCCloudArenaTargetInfo)
    self:UnRegisterProtocol(SCCloudArenaNewFightRecordNotice)
    self:UnRegisterProtocol(SCCloudArenaFightRecordList)
    self:UnRegisterProtocol(SCCloudArenaRankList)
    self:UnRegisterProtocol(SCCloudArenaNoticeSeasonInitialScore)
    self:UnRegisterProtocol(SCCloudArenaNoticeDanChange)
    self:UnRegisterProtocol(SCCloudArenaAchievementInfo)
    self:UnRegisterProtocol(SCCloudArenaAchievementUpdateNotice)
    self:UnRegisterProtocol(SCCloudArenaRoleSeasonHighestData)
    self:UnRegisterProtocol(SCCloudArenaRealTimeInfo)
    self:UnRegisterProtocol(SCCloudArenaFormationInfo)
    self:UnRegisterProtocol(CSCloudArenaOperate)
    self:UnRegisterProtocol(CSCloudArenaRealTimeInfo)
    self:UnRegisterProtocol(CSCloudArenaSetFormation)
    CloudArenaCtrl.Instance = nil
end

function CloudArenaCtrl.SendRealTimeInfo()
    local protocol = GetProtocol(CSCloudArenaRealTimeInfo)
    SendProtocol(protocol)
end

function CloudArenaCtrl:OnSCCloudArenaRealTimeInfo(protocol)
    self.data:SetRealTimeInfo(protocol)
    local actInfo = ActivityData.Instance:GetActivityInfo(ActType.CloudArena)
    if actInfo then
        actInfo:ActRedPoint()
    end
end

function CloudArenaCtrl:OnSCCloudArenaSelfInfo(protocol)
    self.data:SetSelfInfo(protocol)
    self.hasSelfInfo = true
    if self.RequestCrossServer and self.HasTargetList then
        ViewMgr:OpenView(CloudArenaView)
        self.RequestCrossServer = false
    end
    local actInfo = ActivityData.Instance:GetActivityInfo(ActType.CloudArena)
    if actInfo then
        actInfo:ActRedPoint()
    end
    local act_cfg = ActivityData.Instance:GetActiveData(ActType.CloudArenaDaily)
    ActivityData.Instance:SetActivityFinishInfo(ActType.CloudArenaDaily, protocol.has_today_fight, act_cfg.need_count)
    self.data.main_view_btn_check:SetDirty("val")
    self.data.notice_smart:SetDirty("reward_red")
end

function CloudArenaCtrl:OnSCCloudArenaTargetInfo(protocol)
    self.data:SetTargetInfo(protocol)
    self.hasTargetList = true
    if self.RequestCrossServer and self.hasSelfInfo then
        ViewMgr:OpenView(CloudArenaView)
        self.RequestCrossServer = false
    end
end

function CloudArenaCtrl:OnSCCloudArenaNewFightRecordNotice(protocol)
    self.data:SetNewFightRecordNotice()
    CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_RECORD_LIST, 0, 20)
end

function CloudArenaCtrl:OnSCCloudArenaFightRecordList(protocol)
    self.data:SetFightRecordList(protocol)
    local actInfo = ActivityData.Instance:GetActivityInfo(ActType.CloudArena)
    if actInfo then
        actInfo:ActRedPoint()
    end
end

function CloudArenaCtrl:OnSCCloudArenaRankList(protocol)
    self.data:SetRankList(protocol)
end

function CloudArenaCtrl:OnSCCloudArenaNoticeSeasonInitialScore(protocol)
    self.data:SetSeasonInitialScore(protocol.season_initial_score)
    if ViewMgr:IsOpen(CloudArenaView) and GuideData.Instance:GetGuideFlag(FuncGuideFlagType.FUNC_GUIDE_CLOUD_ARENA) == 1 and protocol.season_initial_score >= 1000 then
        ViewMgr:OpenView(CloudArenaReachRankingView,{view_type = 2})
    end
end

function CloudArenaCtrl:OnSCCloudArenaNoticeDanChange(protocol)
    self.data:SetDanChange(protocol)
    -- if ViewMgr:IsOpen(CloudArenaView) and protocol.old_dan < protocol.new_dan then
    --     ViewMgr:OpenView(CloudArenaReachRankingView, {view_type = 1})
    -- end
end

function CloudArenaCtrl:OnSCCloudArenaAchievementInfo(protocol)
    self.data:SetAchievementInfo(protocol)
    local actInfo = ActivityData.Instance:GetActivityInfo(ActType.CloudArena)
    if actInfo then
        actInfo:ActRedPoint()
    end
end

function CloudArenaCtrl:OnSCCloudArenaAchievementUpdateNotice(protocol)
    self.data:SetAchievementUpdate(protocol)
    local actInfo = ActivityData.Instance:GetActivityInfo(ActType.CloudArena)
    if actInfo then
        actInfo:ActRedPoint()
    end
end

function CloudArenaCtrl:OnSCCloudArenaRoleSeasonHighestData(protocol)
    local season_shop_type = protocol.season_shop_type or 0
    if self.data:GetSeasonShopType() ~= season_shop_type then
        StoreData.Instance:CheckPagaList(10,season_shop_type)
    end
    self.data:SetSeasonHighestData(protocol)
end

function CloudArenaCtrl.SendCloudArenaReq(oper_type, param1, param2)
    if LoginData.Instance:IsOnCrossSever() then
        local protocol = GetProtocol(CSCloudArenaOperate)
        protocol.operate_type = oper_type
        protocol.param1 = param1 or 0
        protocol.param2 = param2 or 0
        SendProtocol(protocol)
    end
end

function CloudArenaCtrl:OpenCloudArenaView()
    local is_open,open_tip = FunOpen.Instance:GetFunIsOpened(Mod.CloudArena.Main)
    if is_open then
        if SceneData.Instance:GetMode() == SceneModeType.ArenaMatch then
            PublicPopupCtrl.Instance:Center(Language.CloudArena.EnterTip)
            return
        end
        if LoginData.Instance:IsOnCrossSever() then
            ViewMgr:OpenView(CloudArenaView)
        else
            self.RequestCrossServer = true
            LoginCtrl.Instance:SendCrossStartReq()
            -- PublicPopupCtrl.Instance:Center(Language.CloudArena.NeedCrossServer)
        end
    else
        PublicPopupCtrl.Instance:Center(open_tip)
    end
end

function CloudArenaCtrl:OnSCCloudArenaFormationInfo(protocol)
    self.data:SetFormationInfo(protocol)
    if self.data.formation_smart.auto_save then
        if self.data.mySelfInfo.remain_fight_times > 0 then
            CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_START_FIGHT, self.data.notice_smart.cur_target_index - 1)
        else
            PublicPopupCtrl.Instance:Center(Language.CloudArena.NoChallengeTimes)
        end
        self.data.formation_smart.auto_save = false
    end
end

function CloudArenaCtrl:SendSetFormationInfo()
    local formation_info = self.data:GetFormationInfo()
    local protocol = GetProtocol(CSCloudArenaSetFormation)
    protocol.grid_list = formation_info.grid_list
    protocol.standby_list =formation_info.standby_list
    SendProtocol(protocol)
end

function CloudArenaCtrl:CloudArenaChallenge()
    local function enterFight()
        if self.data.mySelfInfo.remain_fight_times > 0 then
            CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_START_FIGHT, self.data.notice_smart.cur_target_index - 1)
        else
            PublicPopupCtrl.Instance:Center(Language.CloudArena.NoChallengeTimes)
        end
    end
    --判断是否组队
    if TeamCtrl.Instance:Data():InTeam() then
        local param_t = {
        content = Language.CloudArena.ChallengeInTeam,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    TeamCtrl.Instance:SendExitTeam()
                    enterFight()
                end
            },
        }
        BehaviorData.Instance.auto_behavior = 0
        PublicPopupCtrl.Instance:DialogTips(param_t)    
    else
        enterFight()
    end 
end

--布阵界面助战信息文字
function CloudArenaCtrl:AssistTextFunc(assist_num)
    return string.format(Language.CloudArena.FormationAssistInfo, assist_num)
end

--可助战宠物数量
function CloudArenaCtrl:PetOnLimitNum()
    return 4
end

