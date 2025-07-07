GuildEmbattleMenu = GuildEmbattleMenu or DeclareView("GuildEmbattleMenu", "guild/guild_embattle_menu")
--[[ function GuildEmbattleMenu:GuildEmbattleMenu()
    -- body
end
function GuildEmbattleMenu:LoadCallback()
    -- body
end
function GuildEmbattleMenu:OnClickLook()
    ViewMgr:OpenView(GuildEmbattleShow)
end
function GuildEmbattleMenu:OnClickStart()
    ViewMgr:OpenView(GuildEmbattle)
end ]]

GuildEmbattleMenuPanel = GuildEmbattleMenuPanel or DeclareMono("GuildEmbattleMenuPanel", UIWFlushPanel)
function GuildEmbattleMenuPanel:GuildEmbattleMenuPanel()
    --监听round变化和时间变化
    self.embattle_data = GuildEmbattleData.Instance
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.embattle_data:Base(), func = self.FlushStateInfo}
    }
end
function GuildEmbattleMenuPanel:OnDisable()
	UIWFlushPanel.OnDisable(self)
    self.FightTime:CloseCountDownTime()
    self.FightTime:SetCallBack(nil)
    -- ViewMgr:CloseView(GuildEmbattleFight)
end

function GuildEmbattleMenuPanel:FlushStateInfo()
    local base = self.embattle_data:Base()
    UH.SetText(self.DescTex,  Language.GuildEmbattle.StateDesc[base.round_state])
    self.Time:StampTime(base.next_round_state_time, TimeType.Type_Special_0)
    if base.round_state == 0 then
        self.FightTime:SetCallBack(function ()
            if GuildData.Instance:IsInGuildHonorFB()
            and base.round_state == 0 then
                ViewMgr:OpenView(GuildEmbattleFight)
            end
        end)
        self.FightTime:StampTime(base.next_round_state_time - 5, TimeType.Type_Special_0)
    else
        self.FightTime:SetCallBack(nil)
    end
    local stage = self.embattle_data:Stage()
    local desc = Language.GuildEmbattle.BattleRound
    UH.SetText(self.TitleTex, string.format(desc, Language.GuildEmbattle.RoundName[stage]))
end
function GuildEmbattleMenuPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if self.RewardBtn == nil then
        self.RewardBtn = UIActivityRewardBtn.New(self.BtnParent, ActType.GuildEmbattle, function()
            self:OnClickPreview()
        end)
    end
    self:FlushStateInfo()
end
function GuildEmbattleMenuPanel:OnClickZone()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    --判断职务显示不同界面
    self.mVo = GuildData.Instance:GetMemberVo()
    --申请列表的显示
    if self.mVo then
        local job_info = GuildData.Instance:GetJobInfo(self.mVo.post)
        if job_info.modify_target == 1 then
            ViewMgr:OpenView(GuildEmbattle)
            return
        end
    end
    ViewMgr:OpenView(GuildEmbattleShow)
end
function GuildEmbattleMenuPanel:OnClickCompetitor()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    ViewMgr:OpenView(GuildEmbattleInfoView)
end
function GuildEmbattleMenuPanel:OnClickMsg()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    ViewMgr:OpenView(GuildEmbattleMsg)
end
function GuildEmbattleMenuPanel:OnClickPreview()
    local reward = self.data:GetEmbattleReward()
    ActivityCtrl.Instance:OpenTimeLimitReward(reward)
end

GuildEmbattleMenuView = GuildEmbattleMenuView or DeclareMono("GuildEmbattleMenuView", UIWFlushPanel)
function GuildEmbattleMenuView:GuildEmbattleMenuView()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.embattle, func = self.FlushStateInfo, keys = {"flushstate"}}
    }
end
function GuildEmbattleMenuView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if self.RewardBtn == nil then
        self.RewardBtn = UIActivityRewardBtn.New(self.BtnParent, ActType.GuildEmbattle, function()
            self:OnClickPreview()
        end)
    end
    self:FlushStateInfo()
end
--0是未分胜负，-1是平手，不然就是胜利的guild_id
local bg_sp = {
    [0] = "honordi1",
    [2] = "honordi2",
    [3] = "honordi3",
    [-1] = "honordi4",
}
function GuildEmbattleMenuView:FlushStateInfo()
    local info = self.data:GuildHonorInfo()
    if info.next_match_state > 0 then
        self.TimeMeter:StampTime(info.next_match_state)
    else
        self.TimeMeter:SetTime("")
    end
    UH.SetText(self.LblTitle, Language.Guild.EmbattleState[info.state])
    self.mVo = self.data:GetMemberVo()
    if self.mVo then
        self.BtnInter.Interactable =
            (info.state == GUILD_EMBATTLE_STATE.SET and (self.mVo.post == 0 or self.mVo.post == 1))
    else
        self.BtnInter.Interactable = false
    end
    self.ShowInter.Interactable = info.state == GUILD_EMBATTLE_STATE.SET
    if info.state == GUILD_EMBATTLE_STATE.SET then
        self.SetObj:SetActive(true)
        self.BattleObj:SetActive(false)
    elseif info.state == GUILD_EMBATTLE_STATE.BATTLE then
        self.SetObj:SetActive(false)
        self.BattleObj:SetActive(true)
        --显示当前状态 
        local state = 0
        for i=1,3 do
            --获取3区状态
            state = self.data:GetZoneInfo(i)
            UH.SpriteName(self.BGList[i], bg_sp[state] or bg_sp[0])
            self.State1List[i]:SetActive(state == 0)
            self.State2List[i]:SetActive(state == 2)
            self.State3List[i]:SetActive(state == 3)
            self.State4List[i]:SetActive(state == -1)
            self.BattleList[i]:SetActive(state == 0)
        end
    end
end
function GuildEmbattleMenuView:OnClickBattle(id)
    GuildData.Instance:SetShowZoneId(id)
    GuildCtrl.GuildHonorReq({req_type = GUILD_EMBATTLE_REQ.OB,p1 = id})
end

function GuildEmbattleMenuView:OnClickPreview()
    local reward = self.data:GetEmbattleReward()
    ActivityCtrl.Instance:OpenTimeLimitReward(reward)
end