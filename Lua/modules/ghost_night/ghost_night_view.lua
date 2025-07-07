GhostNightView = GhostNightView or DeclareView("GhostNightView", "ghost_night/ghost_night_view",Mod.GhostNight.Main)

VIEW_DECLARE_LEVEL(GhostNightView,ViewLevel.L1)

function GhostNightView:GhostNightView()
    self.data = GhostNightData.Instance
end

function GhostNightView:LoadCallback(param_t)
    self.Grid:SetData(DataHelper.FormatItemList(Config.ghosts_at_night_auto.other[1].reward))
    UH.SetText(self.Num,Format(Language.GhostNight.NumTip,self.data:GetGhostNightNum()))
    self.Time:CreateActTime(ActType.GhostNight,TimeType.Type_Special_6,Language.GhostNight.TimeTip)
    self.Time:SetCallBack(function()ViewMgr:CloseView(GhostNightView)end)
    self.Toggle.isOn = PrefsInt([[GhostNight]] .. RoleId()) == 0
end

function GhostNightView:OnClickClose()
    ViewMgr:CloseView(GhostNightView)
end

function GhostNightView:OnClickTeam()
    local target_id = Config.ghosts_at_night_auto.other[1].target_id
    local target = TeamData.Instance:GetTeamGoalWithId(target_id)
    if TeamData.Instance:InTeam() then
        TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
        TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
        ViewMgr:OpenView(TeamView)
    else
        TeamData.Instance.cur_platform_target_id = target.target_id
        TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
        ViewMgr:OpenView(TeamPlatformView)
    end
    ViewMgr:CloseView(GhostNightView)
end

function GhostNightView:OnClickEnter()
    GhostNightCtrl.Instance:OnGhostNightScene()
    ViewMgr:CloseView(GhostNightView)
end

function GhostNightView:OnClickToggle()
    PrefsInt([[GhostNight]] .. RoleId(),self.Toggle.isOn and 0 or 1)
    self.data.info_data.val = not self.data.info_data.val
end

function GhostNightView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(152)
end

--===============================GhostNightMenuPanel===============================
GhostNightMenuPanel = GhostNightMenuPanel or DeclareMono("GhostNightMenuPanel", UIWFlushPanel)

function GhostNightMenuPanel:GhostNightMenuPanel()
    self.data = GhostNightData.Instance
    self.data_cares = {
        {data = self.data.info_data,func = self.FlushItemView,init_call = false,keys = {"val"}},
        {data = self.data.info_data,func = self.FlushMenuView,init_call = false,keys = {"num"}},
        {data = ActivityData.Instance:GetActivityEvent(),func = self.FlushItemView,init_call = false,keys = {"act_type", "status"}},
        {data = ActivityData.Instance:GetActivityEvent(),func = self.FlushMenuView,init_call = false,keys = {"act_type", "status"}},
        {data = ActivityData.Instance:GetActivityEvent(),func = self.FlushStatusView,init_call = false,keys = {"act_type", "status"}},
		{data = SceneData.Instance:Base(), func = self.FlushSceneView,init_call = false, keys = {"scene_id"}},
    }
end

function GhostNightMenuPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushItemView()
    self:FlushMenuView()
end

function GhostNightMenuPanel:FlushItemView()
    local is_open = self.data:IsGhostNightOpen()
    if GetActive(self.BaseItem) ~= is_open then
        TaskData.Instance:CheckMenuGuideFlush()
    end
    self.BaseItem:SetActive(is_open)
end

function GhostNightMenuPanel:FlushMenuView()
    UH.SetText(self.Num,Format(Language.GhostNight.NumTips,self.data:GetGhostNightNum()))
    self.Time:CreateActTime(ActType.GhostNight,TimeType.Type_Special_6,Language.GhostNight.TimeTips)
end

function GhostNightMenuPanel:FlushStatusView()
    if ActivityData.Instance:GetActivityStatus(ActType.GhostNight) == ActStatusType.Open then
        PrefsInt([[GhostNight]] .. RoleId(),0)
        if SceneData.Instance:SceneId() == SceneLogic.Instance:MainCityID() then
            self:InitDayAndNight()
        end
    else
        self:OnResumeDayAndNight()
    end
end

function GhostNightMenuPanel:FlushSceneView()
    if SceneData.Instance:SceneId() ~= SceneLogic.Instance:MainCityID() then
        self:OnResumeDayAndNight()
    else
        if ActivityData.Instance:GetActivityStatus(ActType.GhostNight) == ActStatusType.Open then
            self:InitDayAndNight()
        end
    end
end

function GhostNightMenuPanel:OnResumeDayAndNight()
    if ViewMgr:IsOpen(LandsBetweenMainView) then 
        return 
    end 

    if DayAndNight.obj then
        DayAndNight.Restore()
    end
end

function GhostNightMenuPanel:InitDayAndNight()
    DayAndNight.Init()
    local value = DayAndNight.GetCurrTime()
    TimeHelper:CancelTimer(self.ShowTimer)
    self.ShowTimer = UpdateTime(function()
        value = Valve(value + 0.05,1)
        DayAndNight.Stop(value)
    end,function()
        DayAndNight.Stop(1)
    end,2,0.05)
end

function GhostNightMenuPanel:OnClickItem()
    ViewMgr:OpenView(GhostNightView)
end