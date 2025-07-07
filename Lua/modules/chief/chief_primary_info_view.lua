ChiefPrimaryInfoView = ChiefPrimaryInfoView or DeclareMono("ChiefPrimaryInfoView", UIWFlushPanel)
function ChiefPrimaryInfoView:ChiefPrimaryInfoView()
    self.data = ChiefElectionData.Instance
    self.data_cares = {
        {data = self.data.rank_data, func = self.FlushRankView, keys = {"list"}},
        {data = self.data.primary_role, func = self.FlushRoleInfoView, keys = {"socre"}},
        {data = self.data.primary_data, func = self.FlushPlayNumView, keys = {"role_num"}},
        {data = self.data.primary_data, func = self.FlushMatchState, keys = {"match_state"}},
        {data = MainOtherData.Instance.right_menu_data, func = self.FlushRightMenuView, init_call = false},
        {data = ActivityData.Instance.time_limit_reward_point, func = self.FlushEffectView},
    }
    self.Time:SetShowCallBack(function(t) 
        local t_str = string.format(Language.UiTimeMeter.Special, t.min ,t.s)
        return tonumber(t.min) < 5 and ColorStr(t_str,COLORSTR.Red12) or t_str
    end)
    self.language = Language.ChiefElection
end

function ChiefPrimaryInfoView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.Runing:SetActive(false)
    self.Waiting:SetActive(true)
end

function ChiefPrimaryInfoView:OnDisable()
    UIWFlushPanel.OnDisable(self)
    self.Runing:SetActive(false)
    self.Waiting:SetActive(true)
end

function ChiefPrimaryInfoView:OnClickSend()
    ViewMgr:OpenView(ChiefElectionMatch)
end

function ChiefPrimaryInfoView:OnClickMain()
    PublicPopupCtrl.Instance:AlertTip(self.language.ErrorLog[8],function()
        SceneCtrl.Instance:RequestLeaveFb()
        ChiefElectionData.Instance:SetLeaveFlag(true)
    end)
end

function ChiefPrimaryInfoView:FlushRightMenuView()
    local data = MainOtherData.Instance:GetRightMenuData()
    if self[data.key] then
        self[data.key]:SetActive(data.is_active)
    end
end

--剩余选手
function ChiefPrimaryInfoView:FlushPlayNumView()
    UH.SetText(self.PlayNum,self.data:GetPlayNum())
end

--我的分数
function ChiefPrimaryInfoView:FlushRoleInfoView()
    local role_info = self.data:GetPrimaryRoleInfo()
    UH.SetText(self.Score,role_info.socre)
end

--我的排名
function ChiefPrimaryInfoView:FlushRankView()
    UH.SetText(self.Rank,self.data:GetRoleRank() or "--")
end

--预选赛状态
function ChiefPrimaryInfoView:FlushMatchState()
    local match_state = self.data:GetMatchState()
    local stamp_time = self.data:GetNextMatchStateTime()
    local PrimaryTip = self.language.PrimaryTips[match_state]
    if match_state == 0 then
        self.Time:StampTime(stamp_time,TimeType.Type_Special_0,nil,self.language.BattleIngTip)
    elseif match_state == 1 then
        self.Time:StampTime(stamp_time,TimeType.Type_Special_0,nil,self.language.PrimaryEndTip)
    elseif match_state == 2 then
        self.Time:StampTime(stamp_time,TimeType.Type_Special_0,nil,self.language.BattleIngTip)
    else
        UH.SetText(self.RunTitle,Format(self.language.PrimaryTips[3],Language.Common.Prof[RoleData.Instance:GetRoleProf()]))
        PrimaryTip = Format(self.language.PrimaryTips[3],Language.Common.Prof[RoleData.Instance:GetRoleProf()])
    end
    UH.SetText(self.Title,PrimaryTip)
    self.Runing:SetActive(match_state >= 3)
    self.Waiting:SetActive(match_state < 3)
    self:FlushEffectView()
end

function ChiefPrimaryInfoView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[88].desc)
end

function ChiefPrimaryInfoView:OnClickPreview()
    ActivityCtrl.Instance:OpenTimeLimitReward(self.data:GetTotalPreviewReward())
end

function ChiefPrimaryInfoView:FlushEffectView()
    local info = ActivityData.Instance:GetTimeLimitRewardGetFlag(ActType.ChiefElection)
    self.Effect:SetActive(self.data:GetMatchState() ~= 0 and info.particip_award_flag == 0)
end