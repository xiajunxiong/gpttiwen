GuildMeleeMenuView = GuildMeleeMenuView or DeclareView("GuildMeleeMenuView", "guild/guild_melee_menu")

GuildMeleeMenu = GuildMeleeMenu or DeclareMono("GuildMeleeMenu", UIWFlushPanel)
function GuildMeleeMenu:GuildMeleeMenu()
    self.lang = Language.Guild
    self.data = GuildData.Instance
    self.team_data = TeamData.Instance
    self.data_cares = {
        -- {data = self.data:MeleeRankAllInfo(), func = BindTool.Bind(self.FlushRoleAndGuild, self)},
        {data = self.data:MeleeMyRankInfo(), func = BindTool.Bind(self.FlushMyRank, self)},
        {data = self.data.melee_nums_info, func = BindTool.Bind(self.FlushNumsInfo, self)},
        {data = ActivityData.Instance.enter_data, func = BindTool.Bind(self.FlushActStatus, self)},
        {data = MainViewData.Instance.tt_team, func = self.FlushToggleShow, init_call = false},
        {data = MainViewData.Instance.tt_task, func = self.FlushToggleShow, init_call = false},
    }
end

function GuildMeleeMenu:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushActStatus()
    self:FlushRoleAndGuild()
    self:FlushMyRank()
    self:FlushNumsInfo()
    self:FlushEffectShow()
end

function GuildMeleeMenu:FlushActStatus()
    local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.GuildMelee)
    local stamp_time = enter_info.enter_end_timestamp or 0
    if stamp_time ~= 0 then
        self.TimeStr:SetCallBack(BindTool.Bind(self.OnTimeFinish, self))
        self.TimeStr:StampTime(stamp_time, TimeType.Type_Time_0)
        self.StartObj:SetActive(false)
        self.ReadyObj:SetActive(true)
    else
        self.LeftTime:CreateActTime(ActType.GuildMelee, TimeType.Type_Time_0)
        self.ReadyObj:SetActive(false)
        self.StartObj:SetActive(true)
    end
end

function GuildMeleeMenu:OnTimeFinish()
    self.ReadyObj:SetActive(false)
    self.StartObj:SetActive(true)
end

function GuildMeleeMenu:OnClickOut()
	PublicPopupCtrl.Instance:AlertTip(Language.Chat.BackTips .. FightCtrl.GetWarnTips(SceneModeType.GuildMelee),function()
        SceneCtrl.Instance:RequestLeaveFb()
        LoginCtrl.Instance:SendBackToOriginServer()
	end)
end
function GuildMeleeMenu:OnClickPreview()
    local reward_list = self.data:GetMeleeTotalPreReward()
    ActivityCtrl.Instance:OpenTimeLimitReward(reward_list)
end
function GuildMeleeMenu:OnClickRank()
    GuildCtrl.SendActivityGuildFightOperatorReq(GUILD_FIGHT_OPERATOR_TYPE.ALL_RANK_INFO)
    ViewMgr:OpenView(GuildMelee)
end
function GuildMeleeMenu:FlushRoleAndGuild()
    --刷新人数
    local info = self.data:MeleeRankAllInfo()
    UH.SetText(self.RoleNum, info.resi_role)
    UH.SetText(self.GuildNum, info.resi_guild)
    self:FlushEffectShow()
end
function GuildMeleeMenu:FlushNumsInfo()
    local info = self.data.melee_nums_info
    UH.SetText(self.RoleNum, info.resi_role)
    UH.SetText(self.GuildNum, info.resi_guild)
    self:FlushEffectShow()
end
function GuildMeleeMenu:FlushEffectShow()
    --[[ if self.data.survival_award_flag == nil then
        return
    end ]]
    local is_can_get = false
    local enter_info = ActivityData.Instance:GetActivityEnterInfo(ActType.GuildMelee)
    local stamp_time = enter_info.enter_end_timestamp or 0
    if stamp_time == 0 then
        --人数刷新时判断奖励是否有领没有就显示特效
        local info = self.data.melee_nums_info
        --[[ if info.resi_role == nil then
            return 
        end ]]
        local list = self.data:MeleeAliveReward()
        for index, value in ipairs(list) do
            if info.resi_role <= value.live_num and self.data.survival_award_flag[#self.data.survival_award_flag + 1 - index] == 0 then
                is_can_get = true
                break
            end
        end
        --如果flag没变成1就是可领取
        if self.data.particip_award_flag == 0 then
            is_can_get = true
        end
    end
    if is_can_get then
        if self.handle_effect == nil then
            self.handle_effect = self.EffectTool:Play(100108205)
        end
    else
        if self.handle_effect then
            self.EffectTool:Stop(self.handle_effect)
            self.handle_effect = nil
        end
    end
end
function GuildMeleeMenu:FlushMyRank()
    local info = self.data:MeleeMyRankInfo()
    UH.SetText(self.MyRank, info.rank or 0)
end

function GuildMeleeMenu:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[14].desc})
end

function GuildMeleeMenu:CloseCallback()
    self.data:MeleeRankAllInfo():Uncare(self.all_rank_care)
    self.data:MeleeMyRankInfo():Uncare(self.my_rank_care)
end

function GuildMeleeMenu:OnToggleTeam()
    local in_team = self.team_data:InTeam()
    local is_crystal = self.team_data:IsInFBTeam()
    if not in_team then
        TeamData.Instance.cur_platform_target_id = 165
        ViewMgr:OpenView(TeamPlatformView)
    end
end
function GuildMeleeMenu:FlushToggleShow()
    local in_team = self.team_data:InTeam()
    if not in_team then
        self.WaitToggle.isOn = true
    end
end
function GuildMeleeMenu:OnClickTeam()
    local in_team = self.team_data:InTeam()
    local is_crystal = self.team_data:IsInFBTeam()
    if not in_team then
        TeamData.Instance.cur_platform_target_id = 165
        ViewMgr:OpenView(TeamPlatformView)
    else
        ViewMgr:OpenView(TeamView)
    end
end