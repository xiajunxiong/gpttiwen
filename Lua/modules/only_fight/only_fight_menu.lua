OnlyFightMenuView = OnlyFightMenuView or DeclareView("OnlyFightMenuView", "only_fight/only_fight_menu")

OnlyFightMenu = OnlyFightMenu or DeclareMono("OnlyFightMenu", UIWFlushPanel)
function OnlyFightMenu:OnlyFightMenu()
    self.lang = Language.OnlyFight
    self.data = OnlyFightData.Instance
    self.data_cares = {
        {data = self.data:RankAllInfo(), func = BindTool.Bind(self.FlushRoleAndGuild, self)},
        {data = self.data:MyRankInfo(), func = BindTool.Bind(self.FlushMyRank, self)},
        {data = self.data.nums_info, func = BindTool.Bind(self.FlushRoleNum, self)},
        {data = ActivityData.Instance.enter_data, func = BindTool.Bind(self.FlushActStatus, self)}
    }
end

function OnlyFightMenu:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if self.RewardBtn == nil then
        self.RewardBtn = UIActivityRewardBtn.New(self.BtnParent, ActType.OnlyFight, function()
            self:OnClickPreview()
        end)
    end
    self:FlushActStatus()
end

function OnlyFightMenu:FlushActStatus()
    self.data:FlushRewardFlags()
    local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.OnlyFight)
    local stamp_time = enter_info.enter_end_timestamp or 0
    if stamp_time ~= 0 then
        self.TimeStr:SetCallBack(BindTool.Bind(self.OnTimeFinish, self))
        self.TimeStr:StampTime(stamp_time,TimeType.Type_Special_0)
        self.StartObj:SetActive(false)
        self.ReadyObj:SetActive(true)
    else
        self.ReadyObj:SetActive(false)
        self.StartObj:SetActive(true)
    end
    self:FlushRoleAndGuild()
    self:FlushMyRank()
    self:FlushRoleNum()
    UH.SetText(self.TitleDesc, string.format( Language.OnlyFight.RankTitle, self.data:MatchLevel()))
end

function OnlyFightMenu:OnTimeFinish()
    self.ReadyObj:SetActive(false)
    self.StartObj:SetActive(true)
end

function OnlyFightMenu:OnClickOut()
    PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackTips .. FightCtrl.GetWarnTips(SceneModeType.OnlyFight),function()
        SceneCtrl.Instance:RequestLeaveFb()
        LoginCtrl.Instance:SendBackToOriginServer()
	end)
end
function OnlyFightMenu:OnClickRank()
    OnlyFightCtrl.ActivityOnlyFightOperatorReq(0)
    ViewMgr:OpenView(OnlyFightRank)
end

function OnlyFightMenu:FlushRoleAndGuild()
    --刷新人数
    local info = self.data:RankAllInfo()
    if info.role_num == nil then
        UH.SetText(self.RoleNum, 0)
    else
        UH.SetText(self.RoleNum, info.role_num - 1)
    end
    self:FlushMyRank()
end
function OnlyFightMenu:FlushRoleNum()
    local info = self.data.nums_info
    if info.role_num == nil then
        UH.SetText(self.RoleNum, 0)
    else
        UH.SetText(self.RoleNum, info.role_num - 1)
    end
end
function OnlyFightMenu:FlushMyRank()
    local info = self.data:MyRankInfo()
    UH.SetText(self.MyRank, info.rank or 0)
    UH.SetText(self.GuildNum, info.total_score or 0)
end

function OnlyFightMenu:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[50].desc})
end

function OnlyFightMenu:CloseCallback()
    --[[ self.data:RankAllInfo():Uncare(self.all_rank_care)
    self.data:MyRankInfo():Uncare(self.my_rank_care) ]]
end

function OnlyFightMenu:OnClickPreview()
    local reward_list = self.data:GetTotalPreviewReward()
    ActivityCtrl.Instance:OpenTimeLimitReward(reward_list)
end