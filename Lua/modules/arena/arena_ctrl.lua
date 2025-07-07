FILE_REQ("modules/arena/arena_data")
VIEW_REQ("modules/arena/arena_view")

-- VIEW_REQ("modules/arena/arena_rank_view")
-- VIEW_REQ("modules/arena/arena_reward_rule")
VIEW_REQ("modules/arena/arena_fight_report_view")
VIEW_REQ("modules/arena/arena_rank_reward_view")
VIEW_REQ("modules/arena/arena_won_view")
VIEW_REQ("modules/arena/arena_achievement")

ARENA_REQ_TYPE = {
    REQ_TYPE_FIHGT = 0,             --战斗请求  param[0,3]
    REQ_TYPE_REFRESH_TARGET = 1,    --刷新对手
    REQ_TYPE_ROLE_INFO = 2,         --个人信息
    REQ_TYPE_TARGET_INFO = 3,       --对手信息
    REQ_TYPE_TOP_INFO = 4,          --前4信息
    REQ_TYPE_RANK_INFO = 5,         --排行榜信息
    REQ_TYPE_REPORT_INFO = 6,       --战报信息
    REQ_TYPE_FETCH_EXP = 7,         --领取经验
    REQ_TYPE_GET_AWARD = 8,         --领取每日排名奖励
    REQ_TYPE_ACHIEVEMENT = 9,       --成就信息下发
    REQ_TYPE_GET_ACHIEVEMENT = 10,  --领取成就奖励
    REQ_TYPE_GET_RANK_TOP_INFO = 11, --第一名信息下发 2025
    REQ_TYPE_SWEEP = 12,            --扫荡请求  param[0,3]
}

ArenaCtrl = ArenaCtrl or BaseClass(BaseCtrl)

function ArenaCtrl:__init()
    if ArenaCtrl.Instance ~= nil then
        Debuger.LogError("[ArenaCtrl] attempt to create singleton twice!")
        return
    end
    ArenaCtrl.Instance = self
    self.data = ArenaData.New()

    self:RegisterProtocol(SCColosseumRoleInfo,"OnSCColosseumRoleInfo")
    self:RegisterProtocol(SCColosseumTopInfo,"OnSCColosseumTopInfo")
    self:RegisterProtocol(SCColosseumTargetInfo,"OnSCColosseumTargetInfo")
    self:RegisterProtocol(SCColosseumRankInfo,"OnSCColosseumRankInfo")
    self:RegisterProtocol(SCColosseumReportInfo,"OnSCColosseumReportInfo")
    self:RegisterProtocol(SCColosseumRankRewardInfo,"OnSCColosseumRankRewardInfo")
    self:RegisterProtocol(SCColosseumPvPsResultInfo,"OnSCColosseumPvPsResultInfo")
    self:RegisterProtocol(SCColosseumOpenStatusInfo,"OnSCColosseumOpenStatusInfo")
    self:RegisterProtocol(SCColosseumInPoolListInfo,"OnSCColosseumInPoolListInfo")
    self:RegisterProtocol(SCColosseumNewReport, "OnSCColosseumNewReport")
    self:RegisterProtocol(SCColosseumEveryDayRewardInfo, "OnSCColosseumEveryDayRewardInfo")
    self:RegisterProtocol(SCColosseumClientRefreshTargetNotice,"OnSCColosseumClientRefreshTargetNotice")
    self:RegisterProtocol(SCArenaAchievementInfo,"OnSCArenaAchievementInfo")
    self:RegisterProtocol(SCArenaDetalInfo,"OnSCArenaDetalInfo")
    self:RegisterProtocol(SCHistoryHighestRankChangeNotice, "OnSCHistoryHighestRankChangeNotice")
    self:RegisterProtocol(SCSweepResultNotice, "OnSCSweepResultNotice")
    self:RegisterProtocol(SCBattleRewardNotice, "SCBattleRewardNotice")
    self:RegisterProtocol(CSColosseumReq)
    ActivityData.Instance:RegisterTopCondition(ActType.Arena,function()
        return string.format(Language.Arena.TodayChallengeLeftTimes, COLORSTR.Yellow17, self.data:GetChallengeLeftTimes())
    end)
    ActivityData.Instance:CustomClickHandle(ActType.Arena,function()
        ViewMgr:OpenView(ArenaView)
    end)
    ActivityData.Instance:CustomClickHandle(ActType.ArenaAgain,function()
        ViewMgr:OpenView(ArenaView)
    end)
    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_ARENA,function()
        BagData.Instance:SetHideCurrencyCenterFlag(false)
        if self.data.guide_fight then
            TimeHelper:AddDelayTimer(function ()
                local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.mainline)
                if task then
                    task:GoTask()
                end
            end,1)
            self.data.guide_fight = false
        elseif self.data:HistoryHighestRankChanged() then
            ViewMgr:OpenView(ArenaHistoryHighestScoreChange)
        else
            ViewMgr:OpenView(ArenaView)
        end
        --[[ BattleCtrl.Instance:DelayFinish(function()
            if self.data.guide_fight then
                TimeHelper:AddDelayTimer(function ()
                    local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.mainline)
                    if task then
                        task:GoTask()
                    end
                end,1)
                self.data.guide_fight = false
			elseif self.data:HistoryHighestRankChanged() then
                ViewMgr:OpenView(ArenaHistoryHighestScoreChange)
            else
                ViewMgr:OpenView(ArenaView)
            end
        end) ]]
    end)
    ActivityData.Instance:RegisterRemind(ActType.Arena,function()
        if ActivityData.Instance:IsNoEnoughLevel(ActType.Arena,true) then
            return 0
        end
        return self.data:GetRemindNum()
    end)
    FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_ARENA,{
        value_func = function()
            local textList = self.data:GetResultInfo()
            local value_list = {}
            value_list[1] = {name = Language.Arena.RankUp, num = textList[1], sprite = "TiShengJianTou"}
            value_list[2] = {name = Language.Arena.CurRank, num = textList[3]}
            return value_list
        end,
        reward_func = function()
            return self.data:FinishBattleReward()
        end,
        show_rank = function()
            return true
        end
    })
	ActivityData.Instance:RegisterCondition(ActType.Arena,function()
        return Language.Arena.CurRank .. self.data:GetRankNum()
	end)

    ActFormationData.Instance:RegistFunctions(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_COLOSSEUM,
       {MainType = ArenaCtrl.Instance, challenge_func = "ArenaChallenge", pet_on_limit_func = "PetOnLimitNum", assist_text_func = "AssistTextFunc"}
    )
end

function ArenaCtrl:OnInit()
    if self.battle_care_handle ~= nil then
        BattleData.base:Uncare(self.battle_care_handle)
    end
    self.battle_care_handle = BattleData.Instance:BaseData():KeysCare({"battle_mode"},function ()
        if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_ARENA then
            BagData.Instance:SetHideCurrencyCenterFlag(true)
        end
    end)
end

function ArenaCtrl:__delete()
    ArenaCtrl.Instance = nil
end

function ArenaCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function ArenaCtrl:OnSCColosseumRoleInfo(protocol)
    self.data.RoleInfo.is_out_of_rank_list = protocol.is_out_of_rank_list
    self.data.RoleInfo.rank = protocol.rank
    self.data.RoleInfo.today_fight_count = protocol.today_fight_count
    self.data.RoleInfo.fetch_exp_flag = protocol.fetch_exp_flag
    self.data.RoleInfo.is_change = not self.data.RoleInfo.is_change
    self.data.RoleInfo.player_num_in_pool = protocol.player_num_in_pool
    self.data.RoleInfo.can_get_coin = protocol.can_get_coin
    self.data.RoleInfo.get_colosseum_credits = protocol.get_colosseum_credits
    self.data.RoleInfo.capabolity = protocol.capabolity
    self.data.history_highest_info.cur_rank = protocol.history_highest_rank
    AthleticsData.Instance.historyData.rank=protocol.rank
end

function ArenaCtrl:OnSCColosseumTopInfo(protocol)
    self.data.top_list.data = protocol.top_list
    self.data.top_list.is_change = not self.data.top_list.is_change
end

function ArenaCtrl:OnSCColosseumTargetInfo(protocol)
    self.data.target_list.data = protocol.target_list
    self.data.target_list.is_change = not self.data.target_list.is_change
end

function ArenaCtrl:OnSCColosseumRankInfo(protocol)
    self.data.rank_list = protocol.rank_list
    self.data.rank_list_data:Set(protocol.rank_list)
    -- CompetitionRanklData.Instance:CheckRankListFlush()
end

function ArenaCtrl:OnSCColosseumReportInfo(protocol)
    self.data.record_list = protocol.report_list
end

function ArenaCtrl:OnCSColosseumReq(req_type,param1)
    local protocol = GetProtocol(CSColosseumReq)
    protocol.req_type = req_type
    protocol.param1 = param1 or 0
    SendProtocol(protocol)
end

function ArenaCtrl:OnSCColosseumRankRewardInfo(protocol)
    self.data.history_highest_rank = protocol.history_highest_rank
    self.data.rank_reward_fetch_falg.data = protocol.rank_reward_fetch_falg
    self.data.rank_reward_fetch_falg.is_change = not self.data.rank_reward_fetch_falg.is_change
end

function ArenaCtrl:OnSCColosseumPvPsResultInfo(protocol)
    -- 收到结果后手动请求一遍对手信息
    self:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_TARGET_INFO)
    -- self.data.RoleInfo.rank = protocol.new_rank
    -- 扫荡的话不走战斗结算界面
    if protocol.is_sweep == 0 then
        if protocol.up_rank ~= 0 then
            self.up_rank = protocol.up_rank > 0 and protocol.up_rank or 0
            self.data:SetResultInfo(protocol)
            FinishCtrl.Instance:Finish({is_win = 1},BATTLE_MODE.BATTLE_MODE_ARENA)
        else
            self.data.up_score = protocol.add_score
            FinishCtrl.Instance:Finish({is_win = 0},BATTLE_MODE.BATTLE_MODE_ARENA)
        end
    else
        -- 扫荡完申请一遍战报
        self:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_REPORT_INFO)
    end
    PublicPopupCtrl.Instance:CenterV {type = CurrencyType.Colosseum, num = self.data:GetBaseBattleRewardScore()}
end

function ArenaCtrl:OnSCColosseumOpenStatusInfo(protocol)
    ActivityData.Instance:ActStatus(ActType.Arena, protocol.open_status == 0 and ActStatusType.Close or ActStatusType.Open)
    self.data:SetNextSwitchTS(protocol.next_switch_timestamp)
end

function ArenaCtrl:OnSCColosseumInPoolListInfo(protocol)
    if protocol.type == 0 then --整张表
        self.data.rank_in_pool_list = DataHelper.TableCopy(protocol.top_list)
    elseif protocol.type == 1 then --尾部新增
        for i = 1, #protocol.top_list do
            protocol.top_list[i].rank = #self.data.rank_in_pool_list + 1
            table.insert(self.data.rank_in_pool_list, protocol.top_list[i])
        end
    end
end

function ArenaCtrl:OnSCColosseumNewReport(protocol)
    self.data.has_new_report = true
    local actInfo = ActivityData.Instance:GetActivityInfo(ActType.Arena)
    if actInfo then
        actInfo:ActRedPoint()
    end
end

function ArenaCtrl:OnSCColosseumEveryDayRewardInfo(protocol)
    self.data.rank_pos_list.data = protocol.rank_pos_list
    self.data.rank_pos_list.is_change = not self.data.rank_pos_list.is_change
    local actInfo = ActivityData.Instance:GetActivityInfo(ActType.Arena)
    if actInfo then
        actInfo:ActRedPoint()
    end
end

function ArenaCtrl:OnSCColosseumClientRefreshTargetNotice(protocol)
    if ViewMgr:IsOpen(ArenaView) then
        self:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_TARGET_INFO)
    end
end

function ArenaCtrl:OnSCArenaAchievementInfo(protocol)
    self.data:SetAchievementFlag(protocol)
end

function ArenaCtrl:OnSCArenaDetalInfo(protocol)
    self.data:SetAchievementCount(protocol)
end

function ArenaCtrl:PetOnLimitNum()
    return 4
end

--布阵界面助战信息文字
function ArenaCtrl:AssistTextFunc(assist_num)
    return string.format(Language.Arena.FormationAssistInfo, assist_num)
end

function ArenaCtrl:ArenaChallenge()
    local function enterFight()
        if self.data:GetChallengeLeftTimes() > 0 then
            ArenaCtrl.Instance:OnCSColosseumReq(ARENA_REQ_TYPE.REQ_TYPE_FIHGT,self.data.index)
        else
            PublicPopupCtrl.Instance:Center(Language.Arena.NotEnoughTimes)
        end
    end
    if LoginData.Instance:IsOnCrossSever() then
        local param_t = {
        content = Language.Arena.CrossingAndReturnTip,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    if self.care_handle ~= nil then
                        LoginData.Instance:BaseData():Uncare(self.care_handle)
                    end
                    self.care_handle = LoginData.Instance:BaseData():KeysCare({"real_is_on_crossserver"},function ()
                        if self.care_handle ~= nil then
                            LoginData.Instance:BaseData():Uncare(self.care_handle)
                        end
                        if not LoginData.Instance:IsOnCrossSever() then
                            enterFight()
                        end
                    end)
                    LoginCtrl:SendBackToOriginServer()
                end
            },
        }
        BehaviorData.Instance.auto_behavior = 0
        PublicPopupCtrl.Instance:DialogTips(param_t)
    else
        enterFight()
    end
end

function ArenaCtrl:OnSCHistoryHighestRankChangeNotice(protocol)
    self.data:SetHistoryHighestRank(protocol)
end

function ArenaCtrl:OnSCSweepResultNotice(protocol)
    BagData.Instance:SetHideCurrencyCenterFlag(false)
    if protocol.is_sweep_success == 1 then
        if ViewMgr:IsOpen(ArenaSweepSuccessShowView) then
            self.data.target_list:SetDirty("is_change")
        else
            ViewMgr:OpenView(ArenaSweepSuccessShowView)
        end
    else
        PublicPopupCtrl.Instance:Center(Language.Arena.SweepTip)
    end
end
function ArenaCtrl:SCBattleRewardNotice(protocol)
    self.data:SetBattleReward(protocol.reward_group)
end