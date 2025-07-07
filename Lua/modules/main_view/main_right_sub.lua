
MainRightTTMenu = MainRightTTMenu or DeclareMono("MainRightTTMenu", UIWFlushPanel)

function MainRightTTMenu:MainRightTTMenu()
    self.base_data = MainViewData.Instance:BaseData()
    self.task_data = TaskData.Instance
    self.team_data = TeamData.Instance
    self.config = {
        {btn = "Task", mod = Mod.MainTask},
        {btn = "Team", mod = Mod.Team},
    }
    self.data_cares = {
        {data = MainOtherData.Instance.right_menu_data, func = self.FlushRightMenuView},-- init_call = false},
        {data = MainViewData.Instance.cur_game_state, func = self.FlushShow},
        {data = MainViewData.Instance.tt_task, func = self.ToggleTask, init_call = false},
        {data = MainViewData.Instance.tt_team, func = self.ToggleTeam, init_call = false},
        {data = self.base_data, func = self.FlushShow, keys = {"is_battle"}, init_call = false},
        {data = TeamData.Instance:IsAutoMatch(), func = self.FlushTeamMatching, keys = {"value"}},
        {data = TeamData.Instance:IsAutoMatch(), func = self.FlushTeamMatching, keys = {"value"}},
        {data = ActivityData.Instance:GetActivityEvent(), func = self.FlushEmbattleTime},
        {data = GuildEmbattleData.Instance:Base(), func = self.FlushEmbattleTime, keys = {"state","next_round_state_time"}},
        {data = MainViewData.Instance.rb_toggle, func = self.FlushTaskMenuShow, init_call = false},
        {data = MainViewData.Instance.view_data, func = self.FlushTaskShow, keys = {"show_task"}, init_call = false},
        {data = MainViewData.Instance.view_data, func = self.FlushTaskHide, keys = {"hide_task"}, init_call = false},

    }
    self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,BindTool.Bind(self.RoleNoticeComplete, self))

    GuideManager.Instance:RegisterGetGuideUi("BtnMainTask", 
        function()
            return self.Task , function() 
                self:OnClickTask()
            end 
	end)
end

function MainRightTTMenu:FlushTaskShow()
    if self.ArrowTog.isOn == false then
        self.ArrowEvent:Trigger()
    end
end
function MainRightTTMenu:FlushTaskHide()
    if self.ArrowTog.isOn == true then
        self.ArrowEvent:Trigger()
    end
end
function MainRightTTMenu:FlushTaskMenuShow()
    -- LogDG(MainViewData.Instance.rb_toggle.isOn)
    -- LogDG(self.ArrowTog.isOn)
    if MainViewData.Instance.rb_toggle.isOn == true then
        if self.ArrowTog.isOn == true and MainViewData.Instance.view_data.force_show_task == false then
            self.ArrowEvent:Trigger()
            -- self.ArrowTog.isOn = false
        end
    else
        if self.ArrowTog.isOn == false and MainViewData.Instance.view_data.force_hide_task == false then
            self.ArrowEvent:Trigger()
            -- self.ArrowTog.isOn = false
        end
    end
end
function MainRightTTMenu:FlushEmbattleTime()
    --刷新倒计时
    local info = ActivityData.Instance:GetActivityStatusInfo(ActType.GuildEmbattle)
    if info and info.status == ActStatusType.Close then
        MainOtherData.Instance:SetRightMenuData({key = "Embattle",is_active = false})
        return
    end
    local base = GuildEmbattleData.Instance:Base()
    self.EmbattleTime:StampTime(base.next_round_state_time, TimeType.Type_Special_0)
end
function MainRightTTMenu:FlushTeamMatching()
    local value = self.team_data:IsAutoMatch().value
    local in_team = self.team_data:InTeam()
    self.Matching2:SetActive(false)
    self.Matching:SetActive(false)
    if self.TeamTog.isOn == true then
        TimeHelper:CancelTimer(self.timer_rt_match)
        self.TeamDesc2:SetActive(not value)
        self.Matching2:SetActive(value)
        if value == true and not in_team then
            self.MatchingObj:SetActive(true)
            local info = self.team_data:GetTeamGoalWithId(self.team_data.cur_platform_target_id) or {}
            self.GoalDesc.text = info.desc or ""
            local show_match_desc = nil ~= Config.robot_auto.run_robot[self.team_data.cur_platform_target_id]
            show_match_desc = show_match_desc and 2 ~= info.target_id and TeamConfig.TeamTargetType.Storybook ~= info.team_type
            self.MatchDescObj:SetActive(show_match_desc)
            self.timer_rt_match = TimeHelper:AddRunTimer(function ()
                local match_time = self.team_data.match_time
                if 60 == match_time or 180 == match_time then
                    PublicPopupCtrl.Instance:DialogTips{
                        content = Language.Team.MatchTimeTips,
                        confirm = {
                            func = function ()
                                ViewMgr:CloseView(DialogTipsView)
                                TeamCtrl.Instance:SendCreateTeam(self.team_data.cur_sel_cross, 1)
                            end
                        },
                        cancel = {
                            time = 15,
                        }
                    }
                end
                local t = TimeManager.FormatDHMS(self.team_data.match_time)
                self.team_data.match_time = self.team_data.match_time + 1
                self.MatchingTime.text = string.format(Language.Team.MatchTime, t.min, t.s)
            end)
        end
    else
        self.TeamDesc:SetActive(not value)
        self.Matching:SetActive(value)
        self.MatchingObj:SetActive(false)
    end
    if in_team then
        self.MatchingObj:SetActive(false)
    end
    if not value or in_team then
        TimeHelper:CancelTimer(self.timer_rt_match)
    end
end

function MainRightTTMenu:Awake()
    UIWFlushPanel.Awake(self)
    if IS_AUDIT_VERSION and GLOBAL_CONFIG:Flags().use_auto_create then
        self:SetObjActive(false)
    end
    for _, v in pairs(self.config) do
        local handle,red_point = v.btn .. "Handle",v.btn .. "RedPoint"
        if self[v.btn] then
            if self[red_point] == nil and v.btn ~= "Task" then
                self[red_point] = UiRedPoint.New(self[v.btn], Vector3.New(55, 25), false,1)
                self[red_point]:SetNum(0)
            end
            if v.mod  and v.btn ~= "Task" then
                RemindCtrl.Instance:Remind():UnRegisterGroup(v.mod, self[handle])
                self[handle] = RemindCtrl.Instance:Remind():RegisterGroup(v.mod,function()
                    local num = RemindCtrl.Instance:Remind():GetGroupNum(v.mod)
                    self[red_point]:SetNum(num)
                end, true)
            end
        end
    end
end

function MainRightTTMenu:RoleNoticeComplete()
    if SceneData.Instance:GetMode() == SceneModeType.None then
        self.TaskTog.isOn = true
        self.TeamTog.isOn = false
    else
        self.TaskTog.isOn = false
        self.TeamTog.isOn = true
    end
end
function MainRightTTMenu:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if SceneData.Instance:GetMode() == SceneModeType.None then
        self.TaskTog.isOn = true
        self.TeamTog.isOn = false
    else
        self.TaskTog.isOn = false
        self.TeamTog.isOn = true
    end
end
function MainRightTTMenu:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_rt_match)
    EventSys:UnBind(self.complete_handle)
    self.complete_handle = nil
    TimeHelper:CancelTimer(self.ShowTimer)
end

function MainRightTTMenu:FlushShow()
    TimeHelper:AddDelayFrameTimer(function ()
        if TeamData.Instance:IsInFBTeam() then
            self.TeamTog.isOn = true
            self.TaskPos:SetObjActive(false)
            self.TeamPos.localPosition = Vector3.New(-200, 0, 0)
        else
            self.TeamPos.localPosition = Vector3.New(-67, 0, 0)
            self.TaskPos:SetObjActive(true)
            -- self.TaskTog.isOn = not self.TeamTog.isOn
            -- self.TeamTog.isOn = not self.TaskTog.isOn
            self.TaskTog.isOn = true
            self.TeamTog.isOn = false
        end
        local is_fantasy = SceneData.Instance:GetMode() == SceneModeType.FightFantasy
        self.ButtonObj:SetActive(not is_fantasy)
        self:FlushTeamMatching()
        self:FlushEmbattleTime()
    end)
end

function MainRightTTMenu:OnClickTask()
    ViewMgr:OpenView(MainTaskView)
end

function MainRightTTMenu:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end

function MainRightTTMenu:FlushRightMenuView()
    --embattle
    local data = MainOtherData.Instance:GetRightMenuData()
    if data.key and self[data.key] then
        self[data.key]:SetActive(data.is_active)
    end
end

function MainRightTTMenu:ToggleTask()
    self.TaskTog.isOn = true
    self:FlushTeamMatching()
end

function MainRightTTMenu:ToggleTeam()
    self.TeamTog.isOn = true
    self:FlushTeamMatching()
end

function MainRightTTMenu:OnToggleArrow()
    local is_on = self.ArrowTog.isOn
    self.task_data.menu_guide.arrow = is_on
    self.team_data.menu_arrow.arrow = is_on
    self.team_data:ResetHealMenu()
    if is_on then
        self.RedPoint:SetActive(false)
    else
        self.RedPoint:SetActive(self:FlushRedPoint())
    end
end
--卡顿
function MainRightTTMenu:FlushRedPoint()
    local total = 0
    for _, v in ipairs(self.config) do
        local num = RemindCtrl.Instance:Remind():GetGroupNum(v.mod)
        total = total + num
    end
    return (total > 0)
end

function MainRightTTMenu:OnToggleTeam()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    local in_team = self.team_data:InTeam()
    local is_crystal = self.team_data:IsInFBTeam()
    local is_match = self.team_data:IsAutoMatch().value
    local is_can_solo = self.team_data:IsCanSolo()
    if not in_team and not is_crystal and not is_match and not is_can_solo then
        -- ViewMgr:OpenView(TeamPlatformView)
    elseif is_match then
        self.MatchingObj:SetActive(true)
    end
    self:FlushTeamMatching()
end
function MainRightTTMenu:OnClickCancel()
    TeamCtrl.Instance:SendTeamMatch()
    MainViewData.Instance:ToggleTTTask()
end
function MainRightTTMenu:OnToggleTask()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self:FlushTeamMatching()

    TimeHelper:CancelTimer(self.ShowTimer)
    self.ShowTimer = Invoke(function()
        TaskData.Instance:CheckMenuGuideFlush()
    end,0.5)
end

MainRightFightCrystalMenu = MainRightFightCrystalMenu or DeclareMono("MainRightFightCrystalMenu", UIWFlushPanel)

function MainRightFightCrystalMenu:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end

MainRightFightGatherMenu = MainRightFightGatherMenu or DeclareMono("MainRightFightGatherMenu", UIWFlushPanel)

function MainRightFightGatherMenu:OnEnable()
    self.ToggleArrow.isOn = true
    self.ToggleGather.isOn = true
    self.OutcomeGather.isOn = false
end

MainRightShiTuFBMenu = MainRightShiTuFBMenu or DeclareMono("MainRightShiTuFBMenu", UIWFlushPanel)

function MainRightShiTuFBMenu:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end

MainRightTaskDCZYFBMenu = MainRightTaskDCZYFBMenu or DeclareMono("MainRightTaskDCZYFBMenu", UIWFlushPanel)

function MainRightTaskDCZYFBMenu:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end

MainRightTaskWXLFBMenu = MainRightTaskWXLFBMenu or DeclareMono("MainRightTaskWXLFBMenu", UIWFlushPanel)

function MainRightTaskWXLFBMenu:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end

MainRightGuildFBMenu = MainRightGuildFBMenu or DeclareMono("MainRightGuildFBMenu", UIWFlushPanel)

function MainRightGuildFBMenu:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end

MainRightAdvancedMenu = MainRightAdvancedMenu or DeclareMono("MainRightAdvancedMenu", UIWFlushPanel)

function MainRightAdvancedMenu:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end

MainRightFightFantasyMenu = MainRightFightFantasyMenu or DeclareMono("MainRightFightFantasyMenu", UIWFlushPanel)

function MainRightFightFantasyMenu:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end

MainRightFightMedalMenu = MainRightFightMedalMenu or DeclareMono("MainRightFightMedalMenu", UIWFlushPanel)

MainRightRDTMenu = MainRightRDTMenu or DeclareMono("MainRightRDTMenu", UIWFlushPanel)

function MainRightRDTMenu:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end

MainRightFoxCatchMenu = MainRightFoxCatchMenu or DeclareMono("MainRightFoxCatchMenu", UIWFlushPanel)

function MainRightFoxCatchMenu:OnClickTeam()
    ViewMgr:OpenView(TeamView)
end

FightDreamNotesMenu = FightDreamNotesMenu or DeclareMono("FightDreamNotesMenu", UIWFlushPanel)

FightGoThroughMenu = FightGoThroughMenu or DeclareMono("FightGoThroughMenu", UIWFlushPanel)

FightMaterialMenu = FightMaterialMenu or DeclareMono("FightMaterialMenu", UIWFlushPanel)

function FightMaterialMenu:OnEnable()
    UIWFlushPanel.OnEnable(self)
    local seq = MaterialData.Instance:GetChallengeData()
    local config = MaterialData.Instance:GetChapterConfig(seq)
    UH.SetText(self.Title,config.chapter_name)
end

FightPetGodMenu = FightPetGodMenu or DeclareMono("FightPetGodMenu", UIWFlushPanel)

function FightPetGodMenu:FightPetGodMenu()
    self.data = FightCtrl.Instance:Data()
    self.data_cares = {
		{data = self.data.pet_god_info, func = self.FlushScene, keys = {"scene"}},
    }
end

function FightPetGodMenu:FlushScene()
    local scene = self.data.pet_god_info.scene
    local info = self.data.pet_god_seq_list[scene.fb_seq]
    if nil == info then return end
    self.Title.text = info.chapter_name
end

WeddingFBMenu = WeddingFBMenu or DeclareMono("WeddingFBMenu", UIWFlushPanel)


MainRightHeroVanBattleMenu = MainRightHeroVanBattleMenu or DeclareMono("MainRightHeroVanBattleMenu", UIWFlushPanel)
function MainRightHeroVanBattleMenu:OnClickTeam()
    -- ViewMgr:OpenView(TeamView)
end
MainRightMarryPartyMenu = MainRightMarryPartyMenu or DeclareMono("MainRightMarryPartyMenu", UIWFlushPanel)

MainRightHarmonyHeavenMenu = MainRightHarmonyHeavenMenu or DeclareMono("MainRightHarmonyHeavenMenu", UIWFlushPanel)
