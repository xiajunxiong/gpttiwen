--===================================ChiefElectionInfoView===================================
ChiefElectionInfoView = ChiefElectionInfoView or DeclareMono("ChiefElectionInfoView", UIWFlushPanel)
function ChiefElectionInfoView:ChiefElectionInfoView()
    self.data = ChiefElectionData.Instance
    self.data_cares = {
        {data = self.data.info_data, func = self.FlushMatchview, keys = {"end_time"}},
        {data = ActivityData.Instance.time_limit_reward_point, func = self.FlushEffectView},
    }
end

function ChiefElectionInfoView:Awake()
    UIWFlushPanel.Awake(self)
    local battleing_tip = Language.ChiefElection.BattleIngTip
    self.Time:SetCallBack(function()
        if not self.data:IsAbandon() then
            self.Time:TotalTime(600,TimeType.Type_Special_0,nil,battleing_tip)
        end
    end)
end

function ChiefElectionInfoView:OnDisable()
    UIWFlushPanel.OnDisable(self)
    self.Time:CloseCountDownTime()
end

function ChiefElectionInfoView:FlushMatchview()
    local info = self.data.info_data
    local battleing_tip = Language.ChiefElection.BattleIngTip
    self.Time:StampTime(info.end_time, TimeType.Type_Special_0,nil,battleing_tip)
    local prof_name = Language.Common.Prof[RoleData.Instance:GetRoleProf()]
    UH.SetText(self.Title, prof_name .. Language.ChiefElection.WaitPlayTitle[info.curr_round])
    self.Time:SetRemainEvent(function()
        local info = self.data:GetOpponent()
        if info ~= nil and info.role_id and info.role_id ~= 0 then
            if not self.data:IsAbandon() then
                ChiefCtrl.Instance:Cinematics()
            end
        end
    end,2)
    self:FlushEffectView()
end

function ChiefElectionInfoView:OnClickSend()
    ViewMgr:OpenView(ChiefElectionMatch)
end

function ChiefElectionInfoView:OnClickCompetitor()
    ViewMgr:OpenView(ChiefMatcnInfo)
end

function ChiefElectionInfoView:OnClickMain()
    PublicPopupCtrl.Instance:AlertTip(Language.ChiefElection.ErrorLog[8],function()
        SceneCtrl.Instance:RequestLeaveFb()
        ChiefElectionData.Instance:SetLeaveFlag(true)
    end)
end

function ChiefElectionInfoView:OnClickPreview()
    ActivityCtrl.Instance:OpenTimeLimitReward(self.data:GetTotalPreviewReward())
end

function ChiefElectionInfoView:FlushEffectView()
    local info = ActivityData.Instance:GetTimeLimitRewardGetFlag(ActType.ChiefElection)
    self.Effect:SetActive(self.data:GetMatchState() ~= 0 and info and info.particip_award_flag == 0)
end

--===================================ChiefPeakBattleInfoView===================================
ChiefPeakBattleInfoView = ChiefPeakBattleInfoView or DeclareMono("ChiefPeakBattleInfoView", UIWFlushPanel)
function ChiefPeakBattleInfoView:ChiefPeakBattleInfoView()
    self.data = ChiefPeakBattleData.Instance
    self.data_cares = {
        {data = self.data.info_data, func = self.FlushMatchview,keys = {"end_time"}},
    }
end

function ChiefPeakBattleInfoView:Awake()
    UIWFlushPanel.Awake(self)
    local battleing_tip = Language.ChiefElection.BattleIngTip
    self.Time:SetCallBack(function()
        if not self.data:IsAbandon() then
            self.Time:TotalTime(600,TimeType.Type_Special_0,nil,battleing_tip)
        end
    end)
end

function ChiefPeakBattleInfoView:OnDisable()
    UIWFlushPanel.OnDisable(self)
    self.Time:CloseCountDownTime()
end

function ChiefPeakBattleInfoView:FlushMatchview()
    local info = self.data.info_data
    local battleing_tip = Language.ChiefElection.BattleIngTip
    self.Time:StampTime(info.end_time,TimeType.Type_Special_0,nil,battleing_tip)
    UH.SetText(self.Title,Language.ChiefPeakBattle.WaitPlayTitle[info.curr_round])
    self.Time:SetRemainEvent(function()
        local info = self.data:GetOpponent()
        if info ~= nil and info.role_id and info.role_id ~= 0 then
            if not self.data:IsAbandon() then
                ChiefCtrl.Instance:Cinematics()
            end
        end
    end,2)
end

function ChiefPeakBattleInfoView:OnClickSend()
    ViewMgr:OpenView(ChiefPeakBattleMatch)
end

function ChiefPeakBattleInfoView:OnClickCompetitor()
    ViewMgr:OpenView(ChiefMatcnInfo)
end

function ChiefPeakBattleInfoView:OnClickMain()
    PublicPopupCtrl.Instance:AlertTip(Language.ChiefPeakBattle.ErrorLog[6],function()
        SceneCtrl.Instance:RequestLeaveFb()
        ChiefPeakBattleData.Instance:SetLeaveFlag(true)
    end)
end
--对手信息
--===================================ChiefMatcnInfo===================================
ChiefMatcnInfo = ChiefMatcnInfo or DeclareView("ChiefMatcnInfo", "chief/ChiefMatcnInfo")

function ChiefMatcnInfo:ChiefMatcnInfo()
    self.Board:SetData(self:GetType(),Language.ChiefElection.MatchInfoTitle,Vector2.New(790,406))
end

function ChiefMatcnInfo:LoadCallback()
    self:FlushRoleView()
    self:FlushEnemyView()
end

--刷新自己的信息
function ChiefMatcnInfo:FlushRoleView()
    UH.SetText(self.RoleName,RoleData.Instance:GetRoleName())
    UH.SetText(self.RoleLevel,Format("Lv.%s",RoleData.Instance:GetRoleLevel()))
    UH.SetAvatar(self.RoleHead,RoleData.Instance:GetRoleAvatarType(),RoleData.Instance:GetRoleAvatarId(),RoleData.Instance:GetRoleAvatarQuality())
    local prof_name = Language.Common.Prof[RoleData.Instance:GetRoleProf()]
    UH.SetText(self.RoleProf,Format(Language.ChiefElection.ProfTip,prof_name))
end

function ChiefMatcnInfo:FlushEnemyView()
    local data = ChiefCtrl.Instance:GetData():GetOpponent()
    if data ~= nil then
        UH.SetText(self.Name,data.role_name)
        UH.SetAvatar(self.Head, data.avatar_type,data.avatar_id,data.avatar_quality)
        UH.SetText(self.Level,Format("Lv.%s",data.level))
        local prof_name = Language.Common.Prof[data.prof]
        UH.SetText(self.Prof,Format(Language.ChiefElection.ProfTip,prof_name))
        self.EnemyView:SetActive(data.role_id ~= nil and data.role_id ~= 0)
        -- #980868 LuaException 重新进游戏时报错，可能是跨服活动但进本服没收到信息
    end
end