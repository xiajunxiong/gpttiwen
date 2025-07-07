ChallengPopupView = ChallengPopupView or DeclareView("ChallengPopupView", "challenge/challenge_popup")

VIEW_DECLARE_MASK(ChallengPopupView, ViewMask.None)

function ChallengPopupView:ChallengPopupView()
    GuideManager.Instance:RegisterGetGuideUi(
        "CourageChallengeSelectBtn",
        function()
            return self.gumobtn, function()
                self.challenge_sub:SetActive(false)
                BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(81, true))
            end
        end
    )
    GuideManager.Instance:RegisterGetGuideUi(
        "SchoolExerciseBtn",
        function()
            return self.school_guide, BindTool.Bind(self.ClickSchoolExercese,self)
        end
    )
end
function ChallengPopupView:Awake()
    FunOpen.Instance:Register(
        Mod.Experience.Task,
        function(opened)
            self.task:SetActive(opened)
        end
    )
    FunOpen.Instance:Register(
        Mod.Experience.GuMo,
        function(opened)
            self.gumo:SetActive(opened)
        end
    )
    FunOpen.Instance:Register(
        Mod.Challenge.ShiLian,
        function(opened)
            self.shilian:SetActive(opened)
        end
    )
    --[[ FunOpen.Instance:Register(
        Mod.Challenge.MedalTask,
        function(opened)
            self.medaltask:SetActive(opened)
            local role_id = RoleData.Instance:GetRoleId()
            local pref_key = PrefKeys.MedalChallengeClick(role_id)
            self.medaltaskred:SetActive(opened and UnityPlayerPrefs.GetInt(pref_key) == 0)
        end
    ) ]]
    FunOpen.Instance:Register(
        Mod.Challenge.RushDungeon,
        function(opened)
            self.medaltask:SetActive(opened)
            local role_id = RoleData.Instance:GetRoleId()
            local pref_key = PrefKeys.MedalChallengeClick(role_id)
            local flag = (opened and UnityPlayerPrefs.GetInt(pref_key) == 0)
            if flag == false then
                self.medaltaskred:SetActive(RushDungeonData.Instance:GetRewardNum() == 1)
            else
                self.medaltaskred:SetActive(true)
            end
        end
    )
    FunOpen.Instance:Register(
        Mod.Experience.SchoolExercise,
        function(opened)
            self.school_exercise:SetActive(opened)
        end
    )
    FunOpen.Instance:Register(
        Mod.Experience.ChallengeTest,
        function(opened)
            self.ChallengeTest:SetActive(opened)
        end
    )
    MainOtherData.Instance:RegisterViewFunc("challenge_sub",function()
        self.challenge_sub:SetActive(false)
    end)
end
function ChallengPopupView:OnEnable()
    self.gumored:SetActive(CourageChallengeData.Instance:GetRewardNum() == 1)
    self.shilianred:SetActive(ConspiracyData.Instance:RedPoint() > 0)
    local school_remind_num = SchoolExerciseData.Instance:GetRemindNum()
    self.school_exercise_red:SetActive(school_remind_num > 0)
    local school_exercise_str = school_remind_num > 0 and Language.SchoolExercise.BtnName1 or Language.SchoolExercise.BtnName2
    UH.SetText(self.school_exercise_text,school_exercise_str)
end

function ChallengPopupView:ClickTask()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.challenge_sub:SetActive(false)
    ViewMgr:OpenView(ChallengeTaskView)
end

function ChallengPopupView:ClickFuben()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.challenge_sub:SetActive(false)
    ViewMgr:OpenView(ChallengeView)
end

function ChallengPopupView:ClickGuMo()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.challenge_sub:SetActive(false)
    --判断是否是当前场景了
    if SceneData.Instance:SceneId() == 155 then
        SceneLogic.Instance:AutoToNpc(CourageChallengeData.NPC)
    else
        SceneLogic.Instance:AutoToNpcs(CourageChallengeData.NPC)
    end
end

function ChallengPopupView:ClickShiLian()
    self.challenge_sub:SetActive(false)
    -- BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(111, true))
    ViewMgr:OpenView(ConspiracyModrons)
end

function ChallengPopupView:ClickMedalTask()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.challenge_sub:SetActive(false)

    local role_id = RoleData.Instance:GetRoleId()
    local pref_key = PrefKeys.MedalChallengeClick(role_id)
    if UnityPlayerPrefs.GetInt(pref_key) == 0 then 
        UnityPlayerPrefs.SetInt(pref_key, 1)
    end 
    self.medaltaskred:SetActive(false)
    BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(96, true))
end
function ChallengPopupView:ClickRushDungeon()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.challenge_sub:SetActive(false)

    local role_id = RoleData.Instance:GetRoleId()
    local pref_key = PrefKeys.MedalChallengeClick(role_id)
    if UnityPlayerPrefs.GetInt(pref_key) == 0 then 
        UnityPlayerPrefs.SetInt(pref_key, 1)
    end
    if RushDungeonData.Instance:GetRewardNum() == 1 then
        self.medaltaskred:SetActive(true)
    else
        self.medaltaskred:SetActive(false)
    end
    ViewMgr:OpenView(RushDungeonMenu)
    -- BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(96, true))
end

function ChallengPopupView:ClickSchoolExercese()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.challenge_sub:SetActive(false)
    ViewMgr:OpenView(SchoolExerciseView)
end

function ChallengPopupView:OnClickChallengeTest()
    self.challenge_sub:SetActive(false)

    if 0 == UnityPlayerPrefs.GetInt(PrefKeys.ChallengeTest(RoleData.Instance:GetRoleId())) then
        UnityPlayerPrefs.SetInt(PrefKeys.ChallengeTest(RoleData.Instance:GetRoleId()), 1)
        ViewMgr:OpenView(ChallengeTestVideoView)
        ViewMgr:OpenView(ChallengeTestView)
    else
        ViewMgr:OpenView(ChallengeTestView)
    end
end

function ChallengPopupView:OnClose()
    self.challenge_sub:SetActive(false)
end

IllusionPopupView = IllusionPopupView or DeclareView("IllusionPopupView", "challenge/illusion_sub")

VIEW_DECLARE_MASK(IllusionPopupView, ViewMask.None)

function IllusionPopupView:IllusionPopupView()
    self.illusion_data = IllusionCtrl.Instance:Data()
    AppearanceData.Instance.remind_red_point:Care(BindTool.Bind(self.FlushAppearancePoint,self))
end

function IllusionPopupView:Awake()
    FunOpen.Instance:Register(
        Mod.Illusion.HuanHua,
        function(opened)
            self.HuanHua:SetActive(opened)
        end
    )
    FunOpen.Instance:Register(
        Mod.Illusion.Appearance,
        function(opened)
            self.Appearance:SetActive(opened)
        end
    )
    MainOtherData.Instance:RegisterViewFunc("illusion_sub",function()
        self.illusion_sub:SetActive(false)
    end)
end

function IllusionPopupView:OnEnable()
    self:FlushRedPointHH()
    self:FlushAppearancePoint()
end

function IllusionPopupView:FlushRedPointHH()
    self.red_point_hh = self.red_point_hh or UiRedPoint.New(self.HuanHua, Vector3.New(120,15,0))
    for type, red_point in pairs(self.illusion_data.red_points) do
        if red_point then
            self.red_point_hh:SetNum(1)
            return
        end
    end
    self.red_point_hh:SetNum(0)
end

function IllusionPopupView:FlushAppearancePoint()
    self.appearance_point = self.appearance_point or UiRedPoint.New(self.Appearance, Vector3.New(120,15,0))
    self.appearance_point:SetNum(AppearanceData.Instance:GetRedPointNum())
end

function IllusionPopupView:OnClickIllusion()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.illusion_sub:SetActive(false)
    ViewMgr:OpenView(IllusionView)
end

function IllusionPopupView:OnClickAppearance()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.illusion_sub:SetActive(false)
    ViewMgr:OpenView(AppearanceView)
end

function IllusionPopupView:OnClickExchange()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.illusion_sub:SetActive(false)
    ViewMgr:OpenView(IllusionExchangeView)
end

function IllusionPopupView:OnClose()
    self.illusion_sub:SetActive(false)
end

--精彩活动提醒监听
--================================MainViewSplendidView================================
MainViewSplendidView = MainViewSplendidView or DeclareMono("MainViewSplendidView", UIWFlushPanel)

function MainViewSplendidView:MainViewSplendidView()
    EventSys:Bind(GlobalEventKeys.TOUCH_BEGAN, BindTool.Bind(self.FlushRandIcon, self))
    self.data_cares = {
        {data = ActivityData.Instance:GetActivityEvent(),func = self.OnFlush,keys = {"act_type", "status"}},
        {data = RoleData.Instance:GetBaseData(), func = self.OnFlush, init_call = false, keys = {"level"}},
        {data = ActivityData.Instance.acts_tips_data,func = self.FlushTipsView,init_call = false},
        {data = ActivityRandData.Instance.rand_child_data, func = self.FlushRandIcon, init_call = false},
    }
end

function MainViewSplendidView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    MainOtherData.Instance:RegisterViewFunc("splendid_sub",function()
        if self.ChildPanel.activeSelf then
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
        end
        self.ChildPanel:SetActive(false)
    end)
end

function MainViewSplendidView:OnFlush()
    for k,item in pairs(self.Grid.item_list or {}) do
        if item ~= nil then
            item:OnRelease()
        end
    end
    self.Grid:SetData({})
    self.ChildPanel:SetActive(false)
    self:FlushTipsView()
end

function MainViewSplendidView:FlushTipsView()
    ActivityRandData.Instance:GetRandDataList()
    self.Tips:SetActive(ActivityRandData.Instance:GetSplendidTip())
end

function MainViewSplendidView:OnClick()
    self.ChildPanel:SetActive(not self.ChildPanel.activeSelf)
    if self.ChildPanel.activeSelf then
        self.Grid:SetData(ActivityRandData.Instance:GetRandDataList())
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    else
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    end
    MainOtherData.Instance:MutualViewFunc("splendid_sub")
    self:FlushTipsView()
end

function MainViewSplendidView:FlushRandIcon()
    if self.ChildPanel.activeSelf then
        self.ChildPanel:SetActive(false)
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    end
end

--================================MainViewNationalDayView================================
MainViewNationalDayView = MainViewNationalDayView or DeclareMono("MainViewNationalDayView", UIWFlushPanel)

function MainViewNationalDayView:MainViewNationalDayView()
    self.data_cares = {
        {data = ActivityData.Instance.acts_tips_data,func = self.FlushTipsView,init_call = true},
    }
end

function MainViewNationalDayView:FlushTipsView()
    self.Tips:SetActive(self:GetNationalDayTip())
end

function MainViewNationalDayView:OnClick()
    ViewMgr:OpenViewByKey(Mod.NationalDay.Main)
end

function MainViewNationalDayView:GetNationalDayTip()
    for k, v in pairs(Config.activity_main_auto.national_day or {}) do
        if ActivityData.IsOpen(v.act_type) and ActivityData.Instance:IsHasSign(v.act_type) then
            return true
        end
	end
	return false
end