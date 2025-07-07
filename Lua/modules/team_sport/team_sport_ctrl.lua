FILE_REQ("modules/team_sport/team_sport_data")
VIEW_REQ("modules/team_sport/team_sport_view")
VIEW_REQ("modules/team_sport/accu_sport_view")
VIEW_REQ("modules/team_sport/team_sport_rank_view")
VIEW_REQ("modules/team_sport/team_sport_tips_view")
VIEW_REQ("modules/team_sport/team_sport_menu_view")
VIEW_REQ("modules/team_sport/team_sport_popup_view")
VIEW_REQ("modules/team_sport/team_sport_menu")
VIEW_REQ("modules/team_sport/sport_info_view")

TeamSportCtrl = TeamSportCtrl or BaseClass(BaseCtrl)

TeamSportCtrl.ReqType = {
    CancelMatching = 0,  --取消匹配
    Matching = 1,        --请求匹配
    FullInfo = 2,       --获取个人信息
    RankInfo = 3,       --获取排行榜信息
    BattleReport = 4,   --获取战报
    OnlyMyReport = 5,   --只看我的战报
}

function TeamSportCtrl:__init()
    if TeamSportCtrl.Instance ~= nil then
        Debuger.LogError("[TeamSportCtrl] attempt to create singleton twice!")
        return
    end
    self.data = TeamSportData.New()
    TeamSportCtrl.Instance = self

    self:RegisterProtocol(CSActivityEnterReq)
    self:RegisterProtocol(CSActivityTeamFightMatchReq)
    
    self:RegisterProtocol(SCActivityTeamFightRank,"OnActivityTeamFightRank")
    self:RegisterProtocol(SCActivityTeamFightScore,"OnActivityTeamFightScore")
    self:RegisterProtocol(SCActivityTeamFightNotice,"OnActivityTeamFightNotice")
    self:RegisterProtocol(SCActivityTeamFightRecord,"OnActivityTeamFightRecord")
    self:RegisterProtocol(SCActivityTeamFightMatchSucc,"OnActivityTeamFightMatchSucc")
    
    self:RegisterProtocol(SCMatchInfoUpdate,"OnMatchInfoUpdate")
    self:RegisterProtocol(SCStartMatch,"OnStartMatch")
    self:RegisterProtocol(SCMatchEnemy,"OnMatchEnemy")
    self:RegisterProtocol(SCStopMatch,"OnStopMatch")

    BehaviorData.Instance:CustomNpcTalkBtnsId(40122, BindTool.Bind(self.SetupBehaviourBtn, self))
    FinishData.Instance:FailRegister(BATTLE_MODE.BATTLE_MODE_TEAM_SPORT,{
        value_func = function()
            local value_show = "- " .. tostring(math.abs(self.data:GetScoreChangeVal()))
            return {{name = Language.TeamSport.Score,num = value_show}}
        end
    })
    FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_TEAM_SPORT,{
        value_func = function()
            local value_show = "+ " .. tostring(math.abs(self.data:GetScoreChangeVal()))
            return {{name = Language.TeamSport.Score,num = value_show}}
        end
    })    
    self.handle_act = ActivityData.Instance.activity_status_event:KeysCare({"act_type", "status"},BindTool.Bind(self.OnActEvent, self))
    ActivityData.Instance:RegisterRewardGetFunc(ActType.TeamSport,BindTool.Bind(self.data.GetHlepRewardInfo,self.data))
end

function TeamSportCtrl:__delete()
    TeamSportCtrl.Instance = nil
end

function TeamSportCtrl:SendTeamFightMatchReq(request_type,param1)
    local protocol = GetProtocol(CSActivityTeamFightMatchReq)
    protocol.request_type = request_type or 0
    protocol.param1 = param1 or 0
    SendProtocol(protocol)
end

function TeamSportCtrl:SendEnterReq(activity_type,room_index)
    local protocol = GetProtocol(CSActivityEnterReq)
    protocol.activity_type = activity_type
    protocol.room_index = room_index
    SendProtocol(protocol)
end

function TeamSportCtrl:OnActivityTeamFightMatchSucc(protocol)
    self.data.match_data.is_match = 0
    ViewMgr:CloseView(TeamSportView)
end

function TeamSportCtrl:OnActivityTeamFightRank(protocol)
    self.data.rank_data.data = protocol.rank_list
    self.data.rank_data.is_change = not self.data.rank_data.is_change
end

function TeamSportCtrl:OnActivityTeamFightScore(protocol)
    self.data.score_data.data = protocol.score_data
    self.data:SetScoreValue(protocol.score_value)
    self.data:SetScoreChangeVal(protocol.change_value)
    self.data.score_data.is_change = not self.data.score_data.is_change
    self:FlushRewardInfo()
end

function TeamSportCtrl:OnActivityTeamFightNotice(protocol)
end

function TeamSportCtrl:OnActivityTeamFightRecord(protocol)
    self.data.fight_info.data = protocol.record_list
    self.data.fight_info.is_change = not self.data.fight_info.is_change
end

function TeamSportCtrl:SetupBehaviourBtn(btn_list,obj_vo)
    if obj_vo and obj_vo.id == 40122 then
        local button = NpcDialogBehBtnData.FuncCreate(
            Language.TeamSport.TeamSport,function()
                local level = RoleData.Instance:GetBaseData().level
		        if level < Config.daily_activity_auto.daily[2].level_restrict then 
			        PublicPopupCtrl.Instance:Center(string.format(Language.WST.level_tips,Config.daily_activity_auto.daily[2].level_restrict))
			        return
                else
                    self:SendEnterReq(2,0)
                end
            end)
        button:SetIconType(NpcIconType.Act)
        table.insert(btn_list, button)
        table.insert(btn_list, NpcDialogBehBtnData.FuncCreate(
            Language.Team.FastTeam,
            function ()
                local target = TeamData.Instance:GetTeamGoalWithId(161)
                if LoginData.Instance:IsOnCrossSever() and TeamConfig.CrossType.not_cross == target.is_cross then
                    PublicPopupCtrl.Instance:Center(Language.Team.MatchCrossError)
                    return
                end
                if TeamData.Instance:InTeam() then
                    TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
                    ViewMgr:OpenView(TeamView)
                else
                    TeamData.Instance.cur_platform_target_id = target.target_id
                    PublicPopupCtrl.Instance:Center(Language.Team.MatchTips)
                    TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
                    ViewMgr:OpenView(TeamPlatformView)
                end
            end,
            true
        ))
    end
end

--活动监听下发
function TeamSportCtrl:OnActEvent()
    if ActivityData.Instance:GetActivityEventType() == ActType.TeamSport then
        if ActivityData.Instance:IsNoEnoughLevel(ActType.TeamSport,true) then
            return
        end
        if ActivityData.Instance:GetActivityStatus(ActType.TeamSport) == ActStatusType.Standy then
            if SceneData.Instance:GetMode() == SceneModeType.TeamSport then
                self.data.match_data.is_match = 0
                SceneCtrl.Instance:RequestLeaveFb()
                ViewMgr:CloseView(TeamSportPopupView)
                ViewMgr:CloseView(TeamSportView)
            end
        end
    end
end

--小队竞技 开始匹配通知
function TeamSportCtrl:OnStartMatch(protocol)
    self.data:SetStartMatch(protocol)
    self.data.match_data.is_match = 1
end

--小队竞技 匹配信息更新（新增队友或队友离开匹配时）
function TeamSportCtrl:OnMatchInfoUpdate(protocol)
    self.data:SetMatchInfoUpdate(protocol)
end

--小队竞技 玩家停止匹配通知
function TeamSportCtrl:OnStopMatch(protocol)
    self.data.match_data.is_match = 0
end

--小队竞技 匹配到对手通知
function TeamSportCtrl:OnMatchEnemy(protocol)
    self.data:SetAllUserList(protocol)
    self.data.match_data.is_complete = 1
    self.data.match_data.is_match = 0
    ViewMgr:OpenView(SportInfoView)
end

-- 参与奖励信息
function TeamSportCtrl:RecvGetReward(protocol)
    self.data:SetJoinData(protocol)
    self:FlushRewardInfo()
end

-- 刷新奖励预览
function TeamSportCtrl:FlushRewardInfo()
    if ViewMgr:IsOpen(TimeLimitRewardPreview) then
        local reward_list = self.data:GetFormatAllReward()
        ActivityCtrl.Instance:FlushTimeLimitReward(reward_list)
    end
end

-- 领取参与奖励
function TeamSportCtrl:GetJoinReward()
    ActivityCtrl.Instance:SendActivityAdvanceFetchReq({
        activity_type = ActType.TeamSport,
        op_type = 0,
    })
end