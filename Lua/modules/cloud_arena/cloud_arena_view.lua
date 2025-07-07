CloudArenaView = CloudArenaView or DeclareView("CloudArenaView","cloud_arena/cloud_arena",Mod.CloudArena.Main)
VIEW_DECLARE_MASK(CloudArenaView, ViewMask.Block)
VIEW_DECLARE_LEVEL(CloudArenaView, ViewLevel.Lu)
function CloudArenaView:CloudArenaView()
    self.is_close_callback = false
    CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_SELF_INFO)
    CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_TARGET_INFO)
    GpQualityMgr:SetShadowEnable(false)
    local quality_obj = SceneMgr.FindGameObjectWithTag("Quality")
    if quality_obj then
        self.light_coms = quality_obj:GetComponentsInChildren(typeof(UnityEngine.Light))
        for i = 0, self.light_coms.Length - 1 do
            self.light_coms[i].enabled = false
        end
    end
    self.ambientLight = UnityEngine.RenderSettings.ambientLight
    self.pixelLightCount = UnityEngine.QualitySettings.pixelLightCount
    self.anisotropicFiltering = UnityEngine.QualitySettings.anisotropicFiltering
    self.shadows = UnityEngine.QualitySettings.shadows
    self.shadowResolution = UnityEngine.QualitySettings.shadowResolution
    self.shadowCascades = UnityEngine.QualitySettings.shadowCascades

    UnityEngine.RenderSettings.ambientLight = COLORS.CloudArena
    UnityEngine.QualitySettings.pixelLightCount = 20
    UnityEngine.QualitySettings.anisotropicFiltering = 2
    UnityEngine.QualitySettings.shadows = 3
    UnityEngine.QualitySettings.shadowResolution = 2
    UnityEngine.QualitySettings.shadowCascades = 4
    -- ViewMgr:MainViewVisible(false)
    ResMgr:LoadGameObject("actor/other/KFJJC_1.prefab", function (obj)
        self.obj = obj
        self.obj.transform.localPosition = Vector3.New(1000,1000,1000)
        self.obj:SetParent(UnityGameObj.Find("ResidentObjectManager"))
        if self:Destroyed() or self.is_close_callback == true then
            if self.obj then
                UnityGameObj.Destroy(self.obj)
            end
        end
    end, true, false)
    self.data = CloudArenaData.Instance

    self.scene_care_handle = SceneMgr:CareLoadedSceneIdChange(BindTool.Bind(self.OnLoadedSceneIdChange,self))
end

function CloudArenaView:LoadCallback()
    CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_RECORD_LIST, 0, 20)
    CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_ACHIEVEMENT_INFO)
    CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_OPEN_UI)
    -- CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_REQ_FORMATION)
    ActFormationCtrl.Instance:SendGetFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_CLOUD_ARENA)

    for i=0,2 do
        CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_RANK_LIST, i)
    end
    if self.data.notice_smart.dan_change then
        ViewMgr:OpenView(CloudArenaReachRankingView, {view_type = 1})
        self.data.notice_smart.dan_change = false
    end
    if self.data:GetTargetList() == nil then
        return  
    end
    if GuideData.Instance:GetGuideFlag(FuncGuideFlagType.FUNC_GUIDE_CLOUD_ARENA) == 0 then
        GuideManager.Instance:Start(83)
    elseif self.data.guide_fight then
        self.data.guide_fight = false
        GuideManager.Instance:Start(84)
    end
    if MainOtherCtrl.Instance.stop == true then
        MainOtherCtrl.Instance.stop = false
    end
    --self.Currency:SetData(CurrencyType.Gold, CurrencyType.CloudArena)
end

function CloudArenaView:CloseCallback()
    self.is_close_callback = true
    -- EventSys:Fire(GlobalEventKeys.CloudArenaClose)
    
    -- ViewMgr:MainViewVisible(true)
    local quality = RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_PICTUER_QUALITY)
    if quality > 0 then
        GpQualityMgr:SetShadowEnable(true)
    end
    if self.light_coms then
        for i=0, self.light_coms.Length - 1 do
            if self.light_coms[i] ~= nil and not self.light_coms[i]:Equals(nil) then
                self.light_coms[i].enabled = true
            end
        end
    end
    UnityEngine.RenderSettings.ambientLight = self.ambientLight
    UnityEngine.QualitySettings.pixelLightCount = self.pixelLightCount
    UnityEngine.QualitySettings.anisotropicFiltering = self.anisotropicFiltering
    UnityEngine.QualitySettings.shadows = self.shadows
    UnityEngine.QualitySettings.shadowResolution = self.shadowResolution
    UnityEngine.QualitySettings.shadowCascades = self.shadowCascades

    if self.obj then
        UnityGameObj.Destroy(self.obj)
    end

    if nil ~= self.scene_care_handle then
        SceneMgr:UncareLoadedSceneIdChange(self.scene_care_handle)
        self.scene_care_handle = nil
    end
    if MainOtherCtrl.Instance.stop == false then
        MainOtherCtrl.Instance.stop = true
    end
end

function CloudArenaView:OnLoadedSceneIdChange()
    local quality_obj = SceneMgr.FindGameObjectWithTag("Quality")
    if quality_obj then
        self.light_coms = quality_obj:GetComponentsInChildren(typeof(UnityEngine.Light))
        for i = 0, self.light_coms.Length - 1 do
            self.light_coms[i].enabled = false
        end
    end
end

function CloudArenaView:OnClickReturn()
    ViewMgr:CloseView(CloudArenaView)
end

function CloudArenaView:OnClickWenHao()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[124].desc})
end

function CloudArenaView:OnClickRefreshTarget()
    if self.data.next_manual_refresh_timestamp <= TimeCtrl.Instance:GetServerTime() then
        if self.data.notice_smart.refresh_left <= 0 then
            PublicPopupCtrl.Instance:Center(Language.CloudArena.RefreshTimesNotEnough)
            return
        end
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.BagTidy)
        self.data.notice_smart.cur_target_index = 1
        CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_REFRESH_TARGET)
    else
        PublicPopupCtrl.Instance:Center(Language.CloudArena.CannotRefreshTarget)
    end
end

function CloudArenaView:OnClickRank()
    ViewMgr:OpenView(CloudArenaRankView)
end

function CloudArenaView:OnClickReport()
    ViewMgr:OpenView(CloudArenaReportView)
end

function CloudArenaView:OnClickReward()
    ViewMgr:OpenView(CloudArenaRewardView)
end

function CloudArenaView:OnClickAchievement()
    ViewMgr:OpenView(CloudArenaAchievementView)
end

function CloudArenaView:OnClickBox()
    -- if self.data.mySelfInfo.day_reward_status == 1 then
    --     AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemGet)
    --     CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_FETCH_DAY_REWARD)
    --     self.data.mySelfInfo.day_reward_status = 2
    -- else
        ViewMgr:OpenView(CloudArenaDayRewardView)
    -- end
end

function CloudArenaView:OnClickBoxTip()
    -- ViewMgr:OpenView(CloudArenaReachRankingView,{view_type = 2})
end

function CloudArenaView:OnClickShop()
    StoreData.Instance:SetJumpIndex({conver_type = 10})
    ViewMgr:OpenViewByKey(Mod.Store.Main)
end

function CloudArenaView:OnClickAddTimes()
    ViewMgr:OpenView(CloudArenaBuyTimesView)
end

function CloudArenaView:OnClickTime()
    self.ChallengeTimeGroup:SetActive(not self.ChallengeTimeGroup.activeSelf)
end

function CloudArenaView:OnClickChallenge()
    local function enterFight()
        if self.data.mySelfInfo.remain_fight_times > 0 then
            ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_CLOUD_ARENA)
        else
            PublicPopupCtrl.Instance:Center(Language.CloudArena.NoChallengeTimes)
        end
    end
    --判断是否组队
    if TeamCtrl.Instance:Data():InTeam() then
        local param_t = {
        content = Language.CloudArena.ChallengeInTeam,
        confirm = {
                func = function()
                    ViewMgr:CloseView(DialogTipsView)
                    TeamCtrl.Instance:SendExitTeam()
                    enterFight()
                end
            },
        }
        BehaviorData.Instance.auto_behavior = 0
        PublicPopupCtrl.Instance:DialogTips(param_t)    
    else
        enterFight()
    end    
end

function CloudArenaView:OnClickFormation()
    -- self:OnClickChallenge()
    ActFormationCtrl.Instance:OpenView(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_CLOUD_ARENA)
end


-----------------------------------------
CloudArenaOpera = CloudArenaOpera  or DeclareMono("CloudArenaOpera",UIWFlushPanel)
function CloudArenaOpera:CloudArenaOpera()
    self.data = CloudArenaData.Instance
    self.objs = {}
    self.objVos = {}
    self.data_cares = {
        {data = self.data.notice_smart, func = self.CreateActor, keys = {"target_refresh"}},
        {data = self.data.notice_smart, func = self.RotateToTarge, keys = {"cur_target_index"}},
    }
end

function CloudArenaOpera:CreateActor()
    --RoleData.Instance:GetBaseData()
    -- local vo = RoleData.Instance:GetBaseData()
    if self.data:GetTargetList() == nil then
        return
    end
    local appearance
    for i=1,CloudArenaData.TargetCount do
        local target_info = self.data:GetTargetList()[i]
        if target_info ~= nil then
            appearance = CommonStruct.AppearanceParam(target_info.appearance)
        else
            appearance = CommonStruct.AppearanceParam({})
        end

        if self.objs[i] ~= nil then
            self.objs[i]:SetData(appearance)
        else
            self.objs[i] = UIChDrawer.CreateByAppearance(appearance)
            self.objs[i]:Enable()
            self.objs[i]:GetRoot():SetParent(self.TargetPosArray[i])
            self.objs[i]:GetRoot():SetLocalPosition(Vector3.zero)
            self.objs[i]:GetRoot():SetLocalEulerAngles(Vector3.zero)
        end
    end
    --创建4个对象并放好位置
end

function CloudArenaOpera:RotateToTarge()
    --LogError("cur_target_index",self.data.notice_smart.cur_target_index)
    self.rotate_count = 0
    if self.last_index == nil then
        local target_angle = Vector3.New(0, CloudArenaOperaAngles[self.data.notice_smart.cur_target_index], 0)
        self.Cylinder:SetLocalEulerAngles(target_angle)
        self.last_index = self.data.notice_smart.cur_target_index
    else
        if self.single_runner ~= nil then
            TimeHelper:CancelTimer(self.single_runner)
        end
        self.single_runner = TimeHelper:AddRunFrameTimer(function ()
            self.rotate_count = self.rotate_count + 1
            local rot_y = (CloudArenaOperaAngles[self.data.notice_smart.cur_target_index] - CloudArenaOperaAngles[self.last_index]) / 20
            local target_angle = Vector3.New(0, CloudArenaOperaAngles[self.last_index] + rot_y * self.rotate_count, 0)
            self.Cylinder:SetLocalEulerAngles(target_angle)
            if self.rotate_count >= 20 then
                self.last_index = self.data.notice_smart.cur_target_index
            end
        end,1 , 20)
    end
end

function CloudArenaOpera:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    for i=1,CloudArenaData.TargetCount do
        if self.objs[i] ~= nil then
            self.objs[i]:DeleteMe()
            self.objs[i] = nil
            -- self.objVos[i] = nil
        end
    end
    if self.single_runner ~= nil then
        TimeHelper:CancelTimer(self.single_runner)
    end
end

-----------------------------
CloudArenaTargetPanel = CloudArenaTargetPanel or DeclareMono("CloudArenaTargetPanel", UIWFlushPanel)
function CloudArenaTargetPanel:CloudArenaTargetPanel()
    self.data = CloudArenaData.Instance
    self.data_cares = {
        {data = self.data.notice_smart, func = self.FlushTargetInfo, keys = {"cur_target_index"}, init_call = false},
        {data = self.data.notice_smart, func = self.FlushRefreshTargetCountDown, keys = {"target_refresh"}},
    }

    GuideManager.Instance:RegisterGetGuideUi("CloudArenaTargeItem", function()
        if self.TargetList:GetItem(1) then
            return self.TargetList:GetItem(1)
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("CloudArenaTargetInfo", function ()
        if self.GuideTargetInfo then
            return self.GuideTargetInfo
        end
    end)
end

function CloudArenaTargetPanel:OnClickGuideInfo()
end

function CloudArenaTargetPanel:OnDestroy()
    self.data.notice_smart.cur_target_index = 1
    if self.runner ~= nil then
        TimeHelper:CancelTimer(self.runner)
    end
    -- if self.refresh_cd_delay ~= nil then
    --     TimeHelper:CancelTimer(self.refresh_cd_delay)
    --     self.refresh_cd_delay = nil
    -- end
    GuideManager.Instance:UnRegisterGetGuideUi("CloudArenaTargeItem")
    GuideManager.Instance:UnRegisterGetGuideUi("CloudArenaTargetInfo")
    if GuideManager.Instance:Guide() ~= nil then
        if GuideManager.Instance:Guide().step_id == 84 
            or GuideManager.Instance:Guide().step_id == 83 then
            GuideManager.Instance:ForceStop()
        end
    end
    UIWFlushPanel.OnDestroy(self)
end

function CloudArenaTargetPanel:FlushTargetInfo()
    local cur_target_info = self.data:GetCurTargetInfo()
    if cur_target_info then
        local rank_cfg = self.data:GetRankConfig(cur_target_info.score)
        -- UH.SetText(self.RankLevelValue, string.format(Language.Common.Xstr, QualityColorStr[rank_cfg.rank], rank_cfg.ranking_title))
        -- UH.SetText(self.RankValue, cur_target_info.rank + 1)
        -- UH.SetText(self.ScoreValue, cur_target_info.score)
        -- UH.SetText(self.TotalValue, cur_target_info.capability)
        if cur_target_info.is_robot == 1 then
            UH.SetText(self.TargetRoleName, cur_target_info.name.."_s"..LoginData.Instance:GetLastLoginServer())
        else
            UH.SetText(self.TargetRoleName, cur_target_info.name)
        end
        if cur_target_info.top_level and cur_target_info.top_level > 0 then
            -- self.PeakLevelBg:SetActive(true)
            UH.SetText(self.TargetRoleLevel, cur_target_info.top_level)
        else
            -- self.PeakLevelBg:SetActive(false)
            UH.SetText(self.TargetRoleLevel, cur_target_info.level)
        end
        UH.SpriteName(self.TargetRankLevelIcon, CloudArenaRankName[rank_cfg.rank])
        -- UH.SpriteName(self.TargetProfImg, ProfSettingB[cur_target_info.appearance.prof_base])
    end
    self.RightArrowObj:SetActive(self.data.notice_smart.cur_target_index < 3)
    self.LeftArrowObj:SetActive(self.data.notice_smart.cur_target_index > 1)
end

function CloudArenaTargetPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    if self.data:GetTargetList() == nil then
        return
    end
    self.TargetList:SetData(self.data:GetTargetList())
    self.TargetList:SetectItem(1, true)
end

function CloudArenaTargetPanel:FlushRefreshTargetCountDown()
    if self.data.next_manual_refresh_timestamp == nil then
        return
    end

    self.RefreshTime:SetCallBack(function ()
        self.RefreshBtnInterac.Interactable = true
    end)
    self.RefreshTime:StampTime(self.data.next_manual_refresh_timestamp ,TimeType.Type_Special_2, nil, Language.CloudArena.RefreshTarget)
    self.RefreshBtnInterac.Interactable = self.data.next_manual_refresh_timestamp <= TimeCtrl.Instance:GetServerTime()
    UH.SetText(self.RefreshTimesText, string.format(Language.CloudArena.RefreshTimes, self.data.notice_smart.refresh_left, self.data.other_cfg.refresh_time))
    self.TargetList:SetData(self.data:GetTargetList())
    self.TargetList:SetectItem(self.data.notice_smart.cur_target_index, true)
    self:FlushTargetInfo()
end

function CloudArenaTargetPanel:OnClickArrow(param)
    if self.data.notice_smart.cur_target_index + param > CloudArenaData.TargetCount or self.data.notice_smart.cur_target_index < 1 then
        param = 0
    end
    self.TargetList:ClickItem(self.data.notice_smart.cur_target_index + param)
    -- self.data.notice_smart.cur_target_index = self.data.notice_smart.cur_target_index + param
end

--------------------------------------------
CloudArenaTargetItem = CloudArenaTargetItem or DeclareMono("CloudArenaTargetItem", UIWidgetBaseItem)
function CloudArenaTargetItem:CloudArenaTargetItem()
    self.data_instance = CloudArenaData.Instance
end

function CloudArenaTargetItem:SetData(data)
    self.TargetAvatar:SetData(data)
    UH.SpriteName(self.ProfImg, ProfSettingB[data.appearance.prof_base])
    -- UH.SpriteName(self.CapArrow, self.data_instance.mySelfInfo.capability >= data.capability and "LvJianTou1" or "HongJianTou1")
    local rank_cfg = self.data_instance:GetRankConfig(data.score)
    UH.SetText(self.RankLevel, rank_cfg ~= nil and string.format(Language.Common.Xstr, QualityColorStr[rank_cfg.rank], rank_cfg.ranking_title) or Language.CloudArena.NoRankTemp)
    if data.rank < 0 then
        UH.SetText(self.RankValue, Language.CloudArena.NoInRank)
    else
        UH.SetText(self.RankValue, data.rank + 1)
    end
    local win_add_point = self.data_instance:GetWinAddPoint(data.target_index)
    UH.SetText(self.WinAddText, string.format(Language.CloudArena.WinAddScore, win_add_point))
    if data.top_level and data.top_level > 0 then
        self.PeakLevelBg:SetActive(true)
        UH.SetText(self.RoleLevelText, string.format(Language.Common.Xstr, COLORSTR.Red8, data.top_level))
    else
        self.PeakLevelBg:SetActive(false)
        UH.SetText(self.RoleLevelText, data.level)
    end
    self.data = data
end

function CloudArenaTargetItem:OnClickItem()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self.data_instance.notice_smart.cur_target_index = self.data.target_index
end

function CloudArenaTargetItem:Click()
    self.Selector.isOn = true
    self:OnClickItem()
end
-----------------------------
CloudArenaMyInfoPanel = CloudArenaMyInfoPanel or DeclareMono("CloudArenaMyInfoPanel", UIWFlushPanel)
function CloudArenaMyInfoPanel:CloudArenaMyInfoPanel()
    self.data = CloudArenaData.Instance
    self.data_cares = {
        {data = self.data.mySelfInfo, func = self.FlushMyRankInfo,keys = {"cur_score"}},
        {data = self.data.mySelfInfo, func = self.FlushRedpoint,keys = {"day_reward_status"}, init_call = false},
        {data = self.data.notice_smart, func = self.FlushRedpoint, keys = {"new_record", "achievement_update"}, init_call = false},
        {data = self.data.notice_smart, func = self.CheckInitialView, keys = {"open_initial_view"}},
    }
    GuideManager.Instance:RegisterGetGuideUi("CloudArenaChallengeBtn", function ()
        if self.ChallengeBtnClick then
            return self.ChallengeBtnClick, function ()
                if self.data.mySelfInfo.remain_fight_times > 0 then
                    CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_START_FIGHT, self.data.notice_smart.cur_target_index - 1)
                    self.data.guide_fight = true
                    RoleCtrl.Instance:SendFuncGuideOperator(FUNCTIONGUIDEOPTYPE.FUNC_GUIDE_FINISH_CLOUD_ARENA_GUIDE)
                    -- if IS_EDITOR then
                    --     GuideData.Instance.guide_flag[FuncGuideFlagType.FUNC_GUIDE_CLOUD_ARENA + 1] = 1 --testCode
                    -- end

                else
                    PublicPopupCtrl.Instance:Center(Language.CloudArena.NoChallengeTimes)
                end
            end
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("CloudArenaSelfInfo", function ()
        if self.GuideMyInfo then
            return self.GuideMyInfo
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("CloudArenaFormationBtn", function()
        if self.FormationBtn then
            return self.FormationBtn
        end
    end)
end

function CloudArenaMyInfoPanel:CheckInitialView()
    if self.data.notice_smart.open_initial_view then
        ViewMgr:OpenView(CloudArenaReachRankingView,{view_type = 2})
    end
end

function CloudArenaMyInfoPanel:OnDestroy()
    GuideManager.Instance:UnRegisterGetGuideUi("CloudArenaChallengeBtn")
    GuideManager.Instance:UnRegisterGetGuideUi("CloudArenaSelfInfo")
    UIWFlushPanel.OnDestroy(self)
end

function CloudArenaMyInfoPanel:FlushMyRankInfo()
    local rank_cfg = self.data.cur_rank_cfg
    UH.SetText(self.TotalValue, self.data.mySelfInfo.capability)
    if self.data.mySelfInfo.cur_rank and self.data.mySelfInfo.cur_rank >= 0 then
        UH.SetText(self.RankValue, self.data.mySelfInfo.cur_rank + 1 )
    else
        UH.SetText(self.RankValue, Language.CloudArena.NoRankTemp)
    end
    UH.SetText(self.ScoreValue, self.data.mySelfInfo.cur_score)
    UH.SetText(self.RemainTimes, string.format(Language.CloudArena.RemainTimes, self.data.mySelfInfo.remain_fight_times, self.data.other_cfg.max_challenge))
    --LogError("season_finish_timestamp",self.data.mySelfInfo.season_finish_timestamp)
    self.SeasonLeftTime:StampTime(self.data.mySelfInfo.season_finish_timestamp, TimeType.Type_Time_6)
    UH.SpriteName(self.RankLevelIcon, rank_cfg ~= nil and CloudArenaRankName[rank_cfg.rank] or "WuDuanWei")
    UH.SetText(self.RankLevelText, rank_cfg ~= nil and string.format(Language.Common.Xstr, QualityColorStr[rank_cfg.rank], rank_cfg.ranking_title) or Language.CloudArena.NoRankTemp)
    if self.data.mySelfInfo.remain_fight_times == nil then
        return
    end

    if self.data.finish_rank_change ~= nil and self.data.finish_rank_change ~= 0 then
        self.EffectTool:Play(self.data.finish_rank_change > 0 and 2166002 or 2166003)
        self.data.finish_rank_change = 0
    end

    self:FlushRedpoint()
    --LogError("mySelfInfo",self.data.mySelfInfo)
end

function CloudArenaMyInfoPanel:FlushRedpoint()
    self.redpoint_group = self.redpoint_group or {}
    for i=1,self.RedpointParArray:Length() do
        if self.data[self.data.RedpointNumFunc[i]](self.data) > 0 then
            if self.redpoint_group[i] == nil then
                self.redpoint_group[i] = UiRedPoint.New(self.RedpointParArray[i],self.data.RedpointPos[i])
            end
            self.redpoint_group[i]:SetNum(1)
        elseif self.redpoint_group[i] ~= nil then
            self.redpoint_group[i]:SetNum(0)
        end
    end
    if self.data.mySelfInfo.day_reward_status == 1 then
        if self.box_eff_handle == nil then
            self.box_eff_handle = self.EffectTool:Play(2166001)
        end
        self.DayRewardImg.enabled = false
    else
        if self.box_eff_handle ~= nil then
            self.EffectTool:Stop(self.box_eff_handle)
        end
        self.DayRewardImg.enabled = true
    end
end

function CloudArenaMyInfoPanel:OnClickGuideInfo()
end
-----------------------------------
CloudArenaFinishInfo = CloudArenaFinishInfo or DeclareMono("CloudArenaFinishInfo", UIWFlushPanel)
function CloudArenaFinishInfo:CloudArenaFinishInfo()
    self.data = CloudArenaData.Instance
    self.data_cares = {
        {data = self.data.notice_smart, func = self.FlushFinishInfo}
    }
end

function CloudArenaFinishInfo:FlushFinishInfo()
    local finish_info = CloudArenaData.Instance:GetFinishInfo()
    UH.SetText(self.ScoreValue, finish_info.score_change)
    UH.SetText(self.CoinValue, finish_info.coin_change)
end

----------------------------------
CloudArenaBuyTimesView = CloudArenaBuyTimesView or DeclareView("CloudArenaBuyTimesView", "cloud_arena/cloud_arena_buy_times")
VIEW_DECLARE_MASK(CloudArenaBuyTimesView, ViewMask.BgBlockClose)
function CloudArenaBuyTimesView:CloudArenaBuyTimesView()
    self.data = CloudArenaData.Instance
end

function CloudArenaBuyTimesView:LoadCallback()
    UH.SetText(self.TitleDesc, string.format(Language.CloudArena.BuyTimesTipFormat, self.data.other_cfg.recovery_price))
    UH.SetText(self.TimesValue, string.format(Language.Common.Slash, self.data.mySelfInfo.remain_fight_times, self.data.other_cfg.max_challenge))
    UH.SetText(self.TimesTipValue, string.format(Language.CloudArena.TodayBuyLimit, self.data:ChallengeCanBuyToday()))
	UH.SetText(self.CurHasValue, RoleData.Instance:GetCurrency(CurrencyType.Gold))
end

function CloudArenaBuyTimesView:OnClickConfirm()
	CloudArenaCtrl.SendCloudArenaReq(CLOUD_ARENA_REQ_TYPE.OPER_TYPE_BUY_FIGHT_TIMES)
    ViewMgr:CloseView(CloudArenaBuyTimesView)
end

function CloudArenaBuyTimesView:OnClickCancel()
	ViewMgr:CloseView(CloudArenaBuyTimesView)
end