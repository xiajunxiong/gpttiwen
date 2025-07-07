FILE_REQ("modules/main_view/main_view_battle")
FILE_REQ("modules/main_view/main_view_chat")
FILE_REQ("modules/main_view/main_view_chat_quick")
FILE_REQ("modules/main_view/main_view_scene")
FILE_REQ("modules/main_view/main_view_usually")


--GameTestBtns======GameObject
--BattleTestBtns====GameObject
--BattleDebugLog====GameObject
MainView = MainView or DeclareView("MainView", "main", 0, ViewLayer.Normal - 1)
VIEW_DECLARE_MASK(MainView, ViewMask.None)
VIEW_DECLARE_LEVEL(MainView, ViewLevel.L0)

function MainView:LoadCallback()
    -- if DEBUG_MODE then
        self.GameTestBtns:SetActive(DEBUG_MODE)
        self.BattleTestBtns:SetActive(DEBUG_MODE)
        self.BattleDebugLog:SetActive(DEBUG_MODE)
    -- end
    --LogError("loaded_rich_text_atlases", self.loaded_rich_text_atlases)
    if IS_MICRO_PLAYER() then
        if not MainViewData.Instance.loaded_rich_text_atlases.rich_text_loaded then
            ResMgr:LoadObject("atlas/rich_text",function ()
                -- LogError("rich_text load done")
                MainViewData.Instance.loaded_rich_text_atlases.rich_text_loaded = true
            end)
        end
        if not MainViewData.Instance.loaded_rich_text_atlases.emoj_loaded then
            ResMgr:LoadObject("atlas/emoj",function ()
                -- LogError("emoj load done")
                MainViewData.Instance.loaded_rich_text_atlases.emoj_loaded = true
            end)
        end
    end
end

function MainView:SetVisible(visible)
    BaseView.SetVisible(self, visible)
    self.BattleAutoBehObj1:SetActive(visible)
    self.BattleAutoBehObj2:SetActive(visible)
end

function MainView:OnClickBag()
    ViewMgr:OpenView(BagView)
end
function MainView:OnClickTempBag()
    ViewMgr:OpenView(TempBag)
end

function MainView:OnClickTreasureHunt()
    ViewMgr:OpenView(TreasureHuntView)
end

function MainView:TestClickArena()
    ViewMgr:OpenView(ArenaView)
end

function MainView:TestRoleAppear()
    ViewMgr:OpenView(RoleAppearChangeView)
end

function MainView:OnClickGuild()
    local view
    if GuildData.Instance:GetGuildID() > 0 then
        view = GuildView
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_BASE_INFO})
    else
        -- 请求家族列表
        GuildCtrl.SendGetGuildList({req_type = GUILD_LIST_REQ_TYPE.GET_GUILD_LIST})
        view = GuildList
    end
    ViewMgr:OpenView(view)
end

function MainView:OnClickTestView(str)
    ViewMgr:OpenView(MonoTypeOf(str))
end

function MainView:OnClickBattleTest()
    GMCmdCtrl.Instance:SendGMCommand("testfightmonster", "2")
end

function MainView:OnClickDebugBattleTest()
    BattleCtrl.Instance:RequestDemo()
end

function MainView:OnClickRequestAll()
    -- LoginCtrl.Instance:RequestAllInfo()
    -- SceneCtrl.Instance:RequestEnterFb(FB_TYPE.FISHING_GROUND, 25)
end

function MainView:OnClickCourageChallenge()
    CourageChallengeCtrl.Instance:EnterScene()
end

function MainView:OnClickRoleProf()
    if RoleData.Instance:GetRoleProfId() > 0 then
        ViewMgr:OpenView(RoleProfessionView)
    else
        ViewMgr:OpenView(RoleSelectProfessionView)
    end
end


function MainView:OnClickUITestBtn()
    ViewMgr:OpenView(YouMingPractice)
end

function MainView:OnClickStoryTest1()
    ViewMgr:OpenView(BattlefieldGainView,{
        old_data = BattlefieldData.Instance:GetRankingConfig(0),
        new_data = BattlefieldData.Instance:GetRankingConfig(4),
    })
end

function MainView:OnClickStoryTest2()
    ViewMgr:OpenView(BattlefieldGainView,{
        old_data = BattlefieldData.Instance:GetRankingConfig(0),
        new_data = BattlefieldData.Instance:GetRankingConfig(8),
    })
end

function MainView:OnClickArenaMacth()
    ViewMgr:OpenView(ArenaMatchView)
end

function MainView:OnClickCombat()
    CombatCtrl.OpenCombatView()
end

local part_t_opened = false

function MainView:TestRolePart(btn_list_obj)
    if btn_list_obj.activeSelf then
        btn_list_obj:SetActive(false)
    else
        btn_list_obj:SetActive(true)
        if part_t_opened == false then
            part_t_opened = true
            local list = btn_list_obj:GetLuaComponent("UIWidgetList")
            list:SetData(
                {
                    {"Main", 30001},
                    {"Main", 50001},
                    {"Wing", 5},
                    {"Wing", 8},
                    {"Wing", nil},
                    {"Ride", 104},
                    {"Ride", 210},
                    {"Ride", nil},
                    {"Weapon", nil},
                    {"Weapon", 10003},
                    -- {"Weapon",20002},
                    {"Weapon", 30003},
                    {"Weapon", 40001},
                    {"Weapon", 50001},
                    {"Weapon", 60009, 60010}
                }
            )
        end
    end
end

function MainView:OnClickFight()
    ViewMgr:OpenView(FightFBView)
end

--打开商城
function MainView:OnClickMallView()
    ViewMgr:OpenView(MallView)
end

TestRolePartBtn = DeclareMono("TestRolePartBtn", UIWidgetBaseItem)

function TestRolePartBtn:TestRolePartBtn()
end

function TestRolePartBtn:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Text.text = data[1] .. "\n" .. (data[2] ~= nil and tostring(data[2]) or "Clear")
end

function TestRolePartBtn:OnClick()
    local par = self.data[1]
    local res_id = self.data[2]
    if par == "Main" then
        MainPlayer:TestSetMain(res_id)
    elseif par == "Wing" then
        MainPlayer:TestSetWing(res_id)
    elseif par == "Ride" then
        MainPlayer:TestSetRide(res_id)
    elseif par == "Weapon" then
        MainPlayer:TestSetWeapon(res_id, self.data[3])
    end
end

MainViewPanel = MainViewPanel or DeclareMono("MainViewPanel", UIWFlushPanel)
---BNSwitch======Animator
---CrossDesc=====Obj
function MainViewPanel:MainViewPanel()
    self.data = MainViewData.Instance
    self.base_data = self.data:BaseData()
    self.fight_den_data = FightDenData.Instance
    self.bag_data = BagData.Instance
    self.guild_data = GuildData.Instance
    self.battle_data = BattleData.Instance
    self.data_cares = {
        {data = self.base_data, func = self.OnChangeBattleState, keys = {"is_battle"}},
        {data = self.data.cur_game_state, func = self.onFlush, init_call = false},
        {data = SceneLogic.Instance.move_to_pos_cache, func = self.OpenGuaji, keys = {"guaji"}, init_call = false},
        -- {data = self.bag_data:GetColumnGrids(ItemColumnType.Temporary), func = self.FlushTempBag, init_call = true},
        {data = LoginData.Instance:BaseData(), func = self.FlushCrossDesc, keys = {"is_on_crosssever"}},
        --1 {data = self.guild_data.question_show, func = self.FlushRemindObj, keys = {"show_panel"}},
        -- {data = RoleData.Instance:GetBaseData(),func = self.FlushTreasure}
        {data = self.base_data, func = self.FlushShowHide, keys = {"is_battle"}},
        -- {data = self.battle_data.story_data, func = self.FlushStory, keys = {"is_start"}},
        {data = self.battle_data:BaseData(), func = self.OnBattleFightingChange, keys = {"server_fighting"}},
        {data = RoleSettingData.Instance.joy_is_on, func = self.OnJoyShow},
        -- {data = TeamData.Instance:IsAutoMatch(), func = self.FlushTeamMatching, keys = {"value"}}
    }
    -- self:FlushHeadRedPoint()
    --[[ if SceneCtrl.Instance:CurScene():GetClassType() ~= BattleScene then
        self.BulletBtn:SetActive(false)
        self.BulletSender:SetActive(false)
    end ]]
    self.BattleEnterAnim:ListenEvent(
        "enter_battle",
        function(eve_param, _)
            if eve_param == "end" then
                if BattleData.Instance:BaseData().server_fighting == true and not self.data:IsBattle()
                and not BattleSimulator.Instance:IsPlaying() then
                    BattleCtrl.Instance:EnterBattle()
                    if self.handle_runer then
                        Runner.Instance:RemoveRunObj(self.handle_runer)
                        self.handle_runer = nil
                    end
                    local effect_switch = true
                    self.handle_runer = Runner.Instance:RunUntilTrue(
                    function()
                        if
                            (SceneCtrl.Instance:CurScene():GetClassType() == BattleScene and
                                SceneCtrl.Instance:CurScene():LoadingComplete()) or ViewMgr:IsOpen(LoadingView)
                        then
                            if self.BEE then
                                local BE = self.BEE:FindChild("BattleEnter")
                                BE:SetActive(false)
                                if effect_switch and BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_CHALLENGE_TEST then
                                    ViewMgr:CloseView(ChallengeTestView)
                                    effect_switch = false
                                    self.EffectTool:Play(2161023)
                                end
                                if effect_switch and BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_NETHER_WORLD_PURGATORY then
                                    ViewMgr:CloseView(DeepDarkFantasyView)
                                end 
                            end
                            -- self:EnterBefore()
                            self.BattleEnter:SetActive(false)
                            if GuildHongBaoPanel.Instance then GuildHongBaoPanel.Instance.hide.count = 1 end
                            self:PlayDuelEnter(false)
                            return true
                        end
                        return false
                    end)
                else
                    self.BattleEnter:SetActive(false)
                    if GuildHongBaoPanel.Instance then GuildHongBaoPanel.Instance.hide.count = 1 end
                    self:PlayDuelEnter(false)
                end
            end
        end
    )
end
--[[ function MainViewPanel:FlushTeamMatching()
    self.Matching:SetActive(TeamData.Instance:IsAutoMatch().value)
end ]]
function MainViewPanel:OnBattleFightingChange()
    if self.battle_data:BaseData().battle_mode ~= BATTLE_MODE.BATTLE_MODE_STORY and self.battle_data:BaseData().server_fighting == true
    and not self.data:IsBattle() then
        self.BattleEnter:SetActive(true)
        if GuildHongBaoPanel.Instance then GuildHongBaoPanel.Instance.hide.count = 0 end
        if self.battle_data:BaseData().battle_mode == BATTLE_MODE.BATTLE_MODE_DUEL then
            self:PlayDuelEnter(true)
        end
    else
        if self.BEE then
            local BE = self.BEE:FindChild("BattleEnter")
            BE:SetActive(false)
        end
        self.BattleEnter:SetActive(false)
        if GuildHongBaoPanel.Instance then GuildHongBaoPanel.Instance.hide.count = 1 end
        self:PlayDuelEnter(false)
    end
end

function MainViewPanel:PlayDuelEnter(is_play)
    if is_play then
        local sc = BaseView.GetWindowScale()
        self.DuelEnter:SetLocalScale(Vector3.New(sc, sc, 1))
    end
    self.DuelEnter:SetActive(is_play)
end

function MainViewPanel:OnJoyShow()
    -- LogError("???",RoleSettingData.Instance:GetServerData(ROLE_SETTING_TYPE.SET_INFO_IS_JOY),RoleSettingData.Instance:GetData(ROLE_SETTING_TYPE.SET_INFO_IS_JOY))
    self.JoyStickObj:SetActive(RoleSettingData.Instance:IsShowJoyStick() and not JoyStickData.Instance:IsHideJoyStick())
end

function MainViewPanel:FlushStory()
    --[[ self.StoryPanel:SetActive(self.battle_data.story_data.is_start)
    if self.battle_data.story_data.is_start == false then
        for i = 1, self.HideObjs:Length() do
            self.HideObjs[i]:SetActive(true)
        end
    end ]]
end
function MainViewPanel:FlushShowHide()
    self.SceneEles:SetActive(not MainViewData.Instance:BaseData().is_battle)
end
--[[ function MainViewPanel:FlushRemindObj()
    self.RemindObj:SetActive(not self.guild_data.question_show.show_panel)
end ]]
function MainViewPanel:HeartBeatTest()
    local HeartBeatTest = UnityPlayerPrefs.HasKey(PrefKeys.HeartBeatTest)
    if HeartBeatTest then
        UnityPlayerPrefs.DeleteKey(PrefKeys.HeartBeatTest)
    else
        UnityPlayerPrefs.SetInt(PrefKeys.HeartBeatTest, 1)
    end
end

function MainViewPanel:FlushTreasure()
    local level = RoleData.Instance:GetBaseData().level
    local config_level = Config.treasure_hunt_auto.other[1].open_level
    self.Treasure:SetActive(level >= config_level)
end

function MainViewPanel:FlushCrossDesc()
    local account_key = LoginData.Instance:GetAccountKey()
    local last_login_server = LoginData.Instance:GetLastLoginServer()
    local last_account_spid = LoginData.Instance:GetLastLoginAccount()
    local server_info = LoginData.Instance:GetServerInfoById(last_login_server)
    -- self.CrossDesc:SetActive(LoginData.Instance:IsOnCrossSever())
    if server_info then
        ChatCtrl.Instance:ChannelChatCustom {cross_get = {name = server_info.name}}
    end
    --ch_info.name}}
end
function MainViewPanel:FlushTempBag()
    local flag = self.bag_data:CheckTempBagEmpty()
    self.TempBag:SetActive(flag)
end

function MainViewPanel:OpenGuaji()
    self.AutoGuaji:SetActive(SceneLogic.Instance.move_to_pos_cache.guaji)
    --中断挂机时如有设置自动抓宠，则弹出抓宠进度信息
    if SceneLogic.Instance.move_to_pos_cache.guaji == false then
        if FightData.Instance:IsSetAutoSealPet() and not ViewMgr:IsOpen(FightAutoSealInfoView) and FightData.Instance:IsSealing() then
            --玩家操作时，直接弹出完成界面
            FightCtrl.Instance:OpenAutoSealInfoView(FightData.OPEN_AUTO_SEAL_INFO_TYPE.ACCOMPLISH)
            TaskData.Instance:TaskChainSubContinue()
            GuildData.Instance:PetSealCountinue()
        end
    else
        --挂机时场景id和设置的自动抓宠场景不一样，直接弹出完成界面
        if FightData.Instance:IsSetAutoSealPet() and not ViewMgr:IsOpen(FightAutoSealInfoView) and (RoleData.Instance:GetBaseData().scene_id ~= FightData.Instance:GetSealSceneId()) then
            FightCtrl.Instance:OpenAutoSealInfoView(FightData.OPEN_AUTO_SEAL_INFO_TYPE.ACCOMPLISH)
            TaskData.Instance:TaskChainSubContinue()
            GuildData.Instance:PetSealCountinue()
        end
    end
end

function MainViewPanel:onFlush()
    self.GameState:SetInteger("state", self.data.cur_game_state.state or 0)
    self.AutoGuaji:SetActive(SceneLogic.Instance.move_to_pos_cache.guaji)
    self:OnBattleEnter()
end
function MainViewPanel:OnBattleEnter()
    if self.BEE == nil then
        self.BEE = UnityGameObj.FindGameObjectWithTag("BattleEnterBefore")
    end
end
function MainViewPanel:EnterBefore()
    -- if BATTLE_XINGTIAN_JUQING(self.battle_data:BaseData().monster_group_id) then
        --[[ local info = StoryData.Instance.story_list[105]
        if info and info.param1 == self.battle_data:BaseData().monster_group_id then
            local ch = BattleCtrl.Instance.displayer:getCh(13)
            if ch then
                ch:SetIsShow(false)
            end
        end ]]
    -- end
end
function MainViewPanel:OnChangeBattleState()
    self.BNSwitch:SetBool("is_battle", self.base_data.is_battle)
    local battle_mode = BattleData.Instance:BattleMode()
end

MainViewGame = MainViewGame or DeclareMono("MainViewGame", UIWFlushPanel)
function MainViewGame:OnEnable()
    ViewMgr:FlushView(MainView, "MainViewPanel")
end

MainViewTopRight = MainViewTopRight or DeclareMono("MainViewTopRight", UIWFlushPanelGroup)
---PetHp=====Slider
---PetMp=====Slider
---PetHmp====GameObject
---PetUnFightTxtObj==GameObject
---PetLvBg===GameObject
---RoleHp=====Slider
---RoleMp=====Slider
--PetHeadObj ==OBJ
--QuietlyDownloadObj==GameObject
--QuietlyDownloadPer==Text
--QuietlyDownloadRP====GameObject
--WorldBossBtn=========GameObject
--PetHeadCell==========UIWSubstitute[UIWPetHeadCell]
--RoleHeadRect======RecTransform(webgl版使用，R线版本仅保留预设修改，不要删除)
--RoleBlockRect======RecTransform(webgl版使用，R线版本仅保留预设修改，不要删除)
--PetHeadRect======RecTransform(webgl版使用，R线版本仅保留预设修改，不要删除)
--GameRect======RecTransform(webgl版使用，R线版本仅保留预设修改，不要删除)
function MainViewTopRight:MainViewTopRight()
    self.data = MainViewData.Instance
    self.pet_data = PetData.Instance
    self.player_data = RoleData.Instance
    self.player_base_data = self.player_data:GetBaseData()
    self.bat_data = self.player_data:AllBattleAttri()
    self.bag_data = BagData.Instance
    self.fight_data = FightData.Instance
    self.battle_data = BattleData.Instance
    self.peak_data = PeakRoadData.Instance
    self.data_cares = {
        -- {data = self.bat_data, func = self.FlushRoleView, keys = {BattleAttriType.CurHp, BattleAttriType.MaxHp, BattleAttriType.CurMp, BattleAttriType.MaxMp}},
        {data = self.player_base_data, func = self.ChangeGameLevel, keys = {"level"}},
        {data = self.player_base_data, func = self.FlushAvatarCell, keys = {"appearance"}},
        {data = self.player_data.break_info, func = self.FlushOfflineReward, init_call = false},
		{data = FightData.Instance.fight_soul_has_remind, func = self.FlushOfflineReward, init_call = false},
        {data = self.pet_data:GetOtherInfo(), func = self.FightPetChange, keys = {"fight_index"}},
        {data = self.pet_data:GetPetList(), func = self.FlushPetRedPoint, init_call = true},
        {data = MainViewData.Instance.buff_list, func = self.FlushBuffButton, init_call = true},
        {data = MainViewData.Instance.upgrade_list, func = self.FlushUpgradeLayout, init_call = true},
        {data = SceneData.Instance:Base(), func = self.OnSceneModeChange, init_call = true, keys = {"scene_mode"}},
        {data = self.data:QuietlyDownloadStateNotifyFlag(),func = self.FlushQuietlyDoanlowdState,init_call = true},
        {data = ServiceData.Instance.msg_count_data, func = self.FlushService, init_call = false},
        {data = MysteryStoreData.Instance.mystery_store_data, func = self.MysteryStoreCountDown, keys = {"next_free_refresh_time"}, init_call = true},
        {data = AnnouncementData.Instance.is_show_data, func = self.FlushAnnouncement, init_call = true},
        {data = MainViewData.Instance.custom_flush_smart, func = self.FlushTapTap,keys = {"taptap"}, init_call = true},
        {data = self.peak_data:PeakData(),func = self.FlushAvatarCell},
        {data = RoleAddPointData.Instance.red_add_piont, func = self.FlushRoleRedPoint},
        {data = self.player_base_data, func = self.FlushRoleRedPoint},
		{data = AttributeDanData.Instance.attribute_red_piont, func = self.FlushRoleRedPoint},
		{data = AttributeDanData.Instance.attribute_red_piont_pet, func = self.FlushPetPoint},
		{data = AppraiseHasGiftData.Instance:InfoData(), func = self.FlushAppraise},
        {data = RoleData.Instance:GetBaseData(), func = self.FlushExpCrystal, keys = {"level"}},
    }
    GuideManager.Instance:RegisterGetGuideUi("OnHookBtn",function()
        return self.OnHookBtnObj,function ()
            ViewMgr:OpenViewByKey(Mod.Fight.Field)
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("OfflineRewardMain",function()
        return self.OfflineRewardBg,function ()
            ViewMgr:OpenViewByKey(Mod.OffLineReward.Main)
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("RoleHeadObj",function ()
        return self.RoleHead, function()
            ViewMgr:OpenView(RoleView)
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("OnWorldBossBtn",function()
        return self.OnWorldBossBtnObj, function()
            self:OnClickWorldBoss()
        end
    end)
    self:FlushService()
    self.remind_handle_role = RemindCtrl.Instance:Remind():RegisterGroup(Mod.Role, BindTool.Bind1(self.FlushRoleRedPoint, self), true)
    self.remind_handle_pet = RemindCtrl.Instance:Remind():RegisterGroup(Mod.Pet, BindTool.Bind1(self.FlushPetPoint, self), true)
end

function MainViewTopRight:Awake()
    UIWFlushPanel.Awake(self)
    if IS_STANDALONE() then
        UH.LocalScale(self.GameRect, STANDALONE_UI_VECTOR3)
    end
end

function MainViewTopRight:OnEnable()
    UIWFlushPanel.OnEnable(self)
    MainOtherData.Instance:RegisterViewFunc("upgrade_sub",function()
        self.UpgradeView:SetActive(false)
        self.UpgradeBlock:SetActive(false)
    end)
    local agent_adapt_info = GLOBAL_CONFIG:AgentAdaptInfoById(GLOBAL_CONFIG:PkgInfo().agent_id)
    if agent_adapt_info then
        self.GmObj:SetActive(agent_adapt_info.is_gm == 1)
    else
        self.GmObj:SetActive(false)
    end
end

function MainViewTopRight:FlushService()
    if self.service_red_point == nil then
        self.service_red_point = UiRedPoint.New(self.ServiceObj, Vector3.New(25, 4))
    end
    self.service_red_point:SetNum(ServiceData.Instance:GetMsgCount())
    self.ServiceObj:SetActive(ServiceData.Instance:IsOpenService())
end

function MainViewTopRight:FlushTapTap()
    local show_btn = GLOBAL_CONFIG:Flags().show_main_taptap
    self.taptapbtn:SetActive(show_btn)
    if HasTapTapBtn and HasTapTapBtn ~= "0" and show_btn then
        if self.tap_red_point == nil then
            self.tap_red_point = UiRedPoint.New(self.ServiceObj, Vector3.New(27, 22))
        end
        self.tap_red_point:SetNum(1)
    elseif self.tap_red_point ~= nil then
        self.tap_red_point:SetNum(0)
    end
end

function MainViewTopRight:OnBattleFightingGroupChange(old)
end

function MainViewTopRight:OnClickExpCry()
    ViewMgr:OpenViewByKey(Mod.Fight.Field)
end

function MainViewTopRight:FlushPetRedPoint()
    self:FightPetChange()
end

function MainViewTopRight:FlushPetView(pet)
    if pet == nil then
        self.PetUnFightTxtObj:SetActive(true)
    else
        self.PetUnFightTxtObj:SetActive(false)
        local total_attr = PetData.Instance:RealAttr(pet.index)
        if total_attr == nil then
            return
        end
        local cur_hp = pet:HP()
        local max_hp = total_attr.maxhp
        local cur_mp = pet:MP()
        local max_mp = total_attr.maxmp
        self.PetHp.value = max_hp ~= 0 and cur_hp / max_hp or 1
        self.PetMp.value = max_mp ~= 0 and cur_mp / max_mp or 1
    end
end

function MainViewTopRight:FightPetChange(keys)
    local fight_pet_info = self.pet_data:GetFightPetInfo()
    if fight_pet_info == nil then
        self.PetHeadCell:Clean()
        self.PetUnFightTxtObj:SetActive(true)
        self.pet_info_care = self:UncareData(self.pet_info_care)
    else
        self.PetHeadCell:SetDataByPet(fight_pet_info)
        self.PetUnFightTxtObj:SetActive(false)
        self.pet_info_care = self:CareData(self.pet_info_care, self.pet_data:GetPetList(), self.PetInfoChange, {self.pet_data:GetFightIndex()})
    end
end

function MainViewTopRight:PetInfoChange(keys)
    local fight_pet_info = self.pet_data:GetFightPetInfo()
    if fight_pet_info == nil then
        return
    end
    self:FlushPetView(fight_pet_info)
    self.PetHeadCell:SetDataByPet(fight_pet_info)
end

--等级改变需要解锁图标
function MainViewTopRight:ChangeGameLevel()
    self:FlushOfflineReward()
    self:FlushFieldBoss()
    self:FlushAvatarCell()
    self:FlushBianQiangIcon()
    self:FlushWorldBossBtn()
end

-- function MainViewTopRight:FlushRoleView()
--     local cur_hp = self.player_data:GetBattleAttri(BattleAttriType.CurHp)
--     local max_hp = self.player_data:GetBattleAttri(BattleAttriType.MaxHp)
--     local cur_mp = self.player_data:GetBattleAttri(BattleAttriType.CurMp)
--     local max_mp = self.player_data:GetBattleAttri(BattleAttriType.MaxMp)
--     self.RoleHp.value = max_hp ~= 0 and cur_hp / max_hp or 1
--     self.RoleMp.value = max_mp ~= 0 and cur_mp / max_mp or 1
-- end

function MainViewTopRight:FlushBuffButton()
    local buff = MainViewData.Instance:BuffWhichHasLongestRemainingTime()
    if buff then
        self.BuffButton:SetActive(true)
        self.BuffTime:SetCallBack(
            function()
                self.BuffTime:CloseCountDownTime()
                self.BuffButton:SetActive(false)
                MainViewData.Instance:RemoveBuffs()
            end
        )
        --优先显示场景buff就好了
        if buff.source == 0 then
            if SceneData.Instance:GetMode() == 0 then
                MainViewData.Instance:RemoveBuff(buff)
                return
            end
            self.BuffTime:CloseCountDownTime()
            self.BuffTime:SetTime(Language.Main.GameBuff.FBING)
            if buff.buff_cfg then
                UH.SetIcon(self.BuffButtonIcon, buff.buff_cfg.icon_id, ICON_TYPE.BUFF)
            else
                self.BuffButtonIcon.SpriteName = MainViewData.Instance:GetBuffIconName(buff)
            end
        elseif buff.source == 1 then
            self.BuffTime:StampTime(buff.time_left or 0, TimeType.Type_Special_0)
            local info = ActivityData.Instance:GetTreviFountainInfo(buff.end_time)
            local buff_list = Config.trevi_fountain_auto.buff_list
            UH.SetIcon(self.BuffButtonIcon,buff_list[info.buff_id or 1].buff_icon,ICON_TYPE.BUFF)
            --存在刷新问题
            if info.buff_id == nil then
                TimeHelper:AddDelayTimer(function ()
                    self:FlushBuffButton()
                end, 1)
            end
        else
            self.BuffTime:StampTime(buff.time_left or 0, TimeType.Type_Special_0)
            self.BuffButtonIcon.SpriteName = MainViewData.Instance:GetBuffIconName(buff)
        end
    else
        self.BuffTime:CloseCountDownTime()
        self.BuffButton:SetActive(false)
    end
end

function MainViewTopRight:OnClickBuffButton()
    if ViewMgr:IsOpen(BuffView) then
        ViewMgr:CloseView(BuffView)
    else
        ViewMgr:OpenView(BuffView)
    end
end

function MainViewTopRight:FlushUpgradeLayout()
    -- if SceneData.Instance:GetMode() == SceneModeType.None then
    --     self.UpgradeLayout:SetActive(MainViewData.Instance.upgrade_list:Length() > 0)
    -- else
        -- self.UpgradeLayout:SetActive(false)
    -- end
    local button_red = false
    for k, v in pairs(MainViewData.Instance.upgrade_list) do
        if v.red then
            button_red = true
            break
        end
    end
    self.UpgradeRed:SetActive(button_red)
end

function MainViewTopRight:OnSceneModeChange()
    self:FlushUpgradeLayout()
    self:FlushBuffButton()
end

function MainViewTopRight:OnClickUpgradeButton()
    local active = not self.UpgradeView.activeSelf
    self.UpgradeView:SetActive(active)
    self.UpgradeBlock:SetActive(active)
    MainOtherData.Instance:MutualViewFunc("upgrade_sub")
end

function MainViewTopRight:OnClickUpgradeBlock()
    self.UpgradeView:SetActive(false)
    self.UpgradeBlock:SetActive(false)
end

function MainViewTopRight:FlushQuietlyDoanlowdState()
    local prog = BundleProviderCtrl.Instance:QuitelyDownloadingProg()
    if prog == nil then
        if not IS_AUDIT_VERSION and     --审核模式怎么都不显示这个下载按钮
            not BundleProviderCtrl.Instance:HasRewardGetted() and BundleProviderCtrl.Instance:RewardGettable() then    
            self.QuietlyDownloadObj:SetActive(true)
            self.QuietlyDownloadPer.text = "100%"
        else
            self.QuietlyDownloadObj:SetActive(false)
        end
        if self.quietly_download_refresh_timer ~= nil then
            TimeHelper:CancelTimer(self.quietly_download_refresh_timer)
            self.quietly_download_refresh_timer = nil
        end
    else
        self.QuietlyDownloadObj:SetActive(true)
        local per_refresh_func = function () 
            local p = BundleProviderCtrl.Instance:QuitelyDownloadingProg()
            if p == nil then return end
            self.QuietlyDownloadPer.text = string.format("%s%%", math.floor(p.total_percent * 100))
        end
        if self.quietly_download_refresh_timer == nil then
            TimeHelper:AddRunTimer(per_refresh_func, 0.1, -1, false)
        end
        per_refresh_func()
    end
    self.QuietlyDownloadRP:SetActive(not BundleProviderCtrl.Instance:HasRewardGetted() and 
        BundleProviderCtrl.Instance:RewardGettable())
end

function MainViewTopRight:OnClickQuietlyDoanlowd()
    ViewMgr:OpenView(QuietlyDownloadView)
end

function MainViewTopRight:OnDestroy()
    UIWFlushPanelGroup.OnDestroy(self)
    if self.quietly_download_refresh_timer ~= nil then
        TimeHelper:CancelTimer(self.quietly_download_refresh_timer)
        self.quietly_download_refresh_timer = nil
    end
    TimeHelper:CancelTimer(self.mystery_store_free_timer)
    if self.remind_handle_role ~= nil then
        RemindCtrl.Instance:Remind():UnRegisterGroup(Mod.Role, self.remind_handle_role)
        self.remind_handle_role = nil
    end
    if self.remind_handle_pet ~= nil then
        RemindCtrl.Instance:Remind():UnRegisterGroup(Mod.Pet, self.remind_handle_pet)
        self.remind_handle_pet = nil
    end
end

function MainViewTopRight:FlushFieldBoss()
    if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
        self.ShopBtnObj:SetActive(false)
        self.AuditShopBtnObj:SetActive(FunOpen.Instance:GetFunIsOpened(Mod.MysteryStore.Main) == true)
        UH.SpriteName(self.AuditShopIcon, "ShenMiShangDian_"..(AudutSeqIndex[GLOBAL_CONFIG:Urls().audit_seq] and GLOBAL_CONFIG:Urls().audit_seq or 1))
    else
        self.ShopBtnObj:SetActive(false)
        -- self.ShopBtnObj:SetActive(FunOpen.Instance:GetFunIsOpened(Mod.MysteryStore.Main) == true)
        self.AuditShopBtnObj:SetActive(false)
    end
    -- FunOpen.Instance:GetFunIsOpened(Mod.FightFieldBoss.Main) == true)
end

function MainViewTopRight:MysteryStoreCountDown()
    self.MysteryStoreRed:SetNum(0)
    self.AuditMysteryStoreRed:SetNum(0)
    if MysteryStoreData.Instance.mystery_store_data.next_free_refresh_time ~= -1 then
        local stamp_time = MysteryStoreData.Instance.mystery_store_data.next_free_refresh_time - TimeHelper.GetServerTime()
        TimeHelper:CancelTimer(self.mystery_store_free_timer)
        self.mystery_store_free_timer = TimeHelper:AddDelayTimer(function()
            self:SetMysteryStoreRed()
        end, stamp_time)
    else
        TimeHelper:CancelTimer(self.mystery_store_free_timer)
    end
end

function MainViewTopRight:SetMysteryStoreRed()
    self.MysteryStoreRed:SetNum(1)
    self.AuditMysteryStoreRed:SetNum(1)
    TimeHelper:CancelTimer(self.mystery_store_free_timer)
end

function MainViewTopRight:FlushOfflineReward()
    self.OfflineReward:SetActive(FunOpen.Instance:GetFunIsOpened(Mod.OffLineReward.Main) == true)
    self.OfflineEffect:SetActive(self.player_data:BreakRemind())
    self.OfflineRewardRed:SetNum(self.player_data:BreakRemind() and 1 or 0)
    UH.SetText(self.TxtOfflineDesc, self.player_data:GetIconTxt())
end

function MainViewTopRight:FlushAppraise()
    self.ObjAppraise:SetActive(true)
end

function MainViewTopRight:OpenAppraise()
    ViewMgr:OpenView(AppraiseHasGiftView)
end

-- function MainViewTopRight:FlushExpCrystalRed()
--     local info = self.fight_data.patrol_task_info
--     self.ExpCrystalRed:SetNum(1 == info.task_status and 1 or 0)
-- end

function MainViewTopRight:FlushExpCrystal()
    self.ExpCrystal:SetActive(FunOpen.Instance:GetFunIsOpened(Mod.Fight.Field) == true)
end

function MainViewTopRight:FlushWorldBossBtn()
    self.WorldBossBtn:SetActive(RoleData.Instance:GetRoleLevel() >= ActivityData.GetConfig(ActType.FightFieldBoss).level_restrict)
end

function MainViewTopRight:OnClickWorldBoss()
    ViewMgr:OpenView(FightFieldBossView)
end

function MainViewTopRight:FlushAnnouncement()
    self.AnnouncementRed:SetNum(AnnouncementData.Instance.red_remind.flag and 1 or 0)
    self.ObjAnnouncement:SetActive(not TableIsEmpty(AnnouncementData.Instance.config_data2.info))
end

function MainViewTopRight:FlushBianQiangIcon()
    --self.UpgradeLayout:SetActive(FunOpen.Instance:GetFunIsOpened(Mod.BianQiang.Main) == true)
end

function MainViewTopRight:FlushRoleRedPoint()
    --主角还要监听进阶和潜能点
    local num = RemindCtrl.Instance:Remind():GetGroupNum(Mod.Role)
    if RoleAddPointData.Instance:GetRedAddPiontNum() > 0 then
        self.RoleRedPoint:SetNum(1)
	elseif AttributeDanData.Instance:GetRedAttibutePiontNum() > 0 then
        self.RoleRedPoint:SetNum(1)
    else
        self.RoleRedPoint:SetNum(num)
    end
end

function MainViewTopRight:FlushPetPoint()
    local num = RemindCtrl.Instance:Remind():GetGroupNum(Mod.Pet)
    self.PetRedPoint:SetNum(num)
end

function MainViewTopRight:OnClickFieldBoss()
    ViewMgr:OpenViewByKey(Mod.MysteryStore.Main)
    self.MysteryStoreRed:SetNum(0)
end

function MainViewTopRight:OnClickOfflineReward()
    ViewMgr:OpenView(OfflineRewardView)
end

function MainViewTopRight:OnServiceClick()
    ViewMgr:OpenView(ServiceView)
end

function MainViewTopRight:OnTapTapClick()
    if ChannelAgent and ChannelAgent.ToPlatWithParam and not IS_IOS() then
        ChannelAgent.ToPlatWithParam("taptap")
    end
end
function MainViewTopRight:OnAnnouncementClick()
    ViewMgr:OpenView(AnnouncementView)
end

function MainViewTopRight:OnGmClick()
    ViewMgr:OpenView(GmWindow)
end

function MainViewTopRight:OnClickRole()
    ViewMgr:OpenView(RoleView)
end

function MainViewTopRight:OnClickPet()
    local count = table.nums(PetData.Instance:GetPetList())
    local count2 = table.nums(PetData.Instance:GetStorageList())
    if count == 0 and count2 > 0 then
        local open_storage = function ()
            ViewMgr:CloseView(DialogTipsView)
            ViewMgr:OpenView(PetView)
            TimeHelper:AddDelayTimer(function ()
                ViewMgr:OpenView(PetStorage)
            end, 0.5)
        end
        local open_pet = function ()
            ViewMgr:OpenView(PetView)
            ViewMgr:CloseView(DialogTipsView)
        end
        local info = {
            content = ErrorText[127],
            confirm = {
                func = open_storage
            },
            cancel = {
                func = open_pet
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end
    ViewMgr:OpenView(PetView)
end

function MainViewTopRight:OnClickBianQiangIcon()
	ViewMgr:OpenView(BianQiangView)
    --这里修改为展开提升列表 后面最近 我要变强和今日统计
end

--=========================新拓展==================================================
function MainViewTopRight:FlushAvatarCell()
    self.AvatarCell:SetData(RoleData.Instance:GetRoleData())
end

--================================MainViewTestPanel=================================
MainViewTestPanel = MainViewTestPanel or DeclareMono("MainViewTestPanel", UIWFlushPanel)

function MainViewTestPanel:OnClickShowPos()
    self.PosGame:SetActive(not GetActive(self.PosGame))
end

--================================MainViewTopLeft=================================
MainViewTopLeft = MainViewTopLeft or DeclareMono("MainViewTopLeft", UIWFlushPanel)
----WifiSp===UIImageSpriteSet
----TimeTex===Text
----PosTex===Text
----LineTex===Text
----NameTex===Text
----DianLiangImg===image
----IconsObj==obj
-- 网络延迟时间类型
Delay_Type = {
    Nomal = 0.5,
    Bad = 1
}
function MainViewTopLeft:MainViewTopLeft()
    self.time_ctrl = TimeCtrl.Instance
    self.scene_data = SceneData.Instance
    self.map_data = MapCtrl.Instance:Data()
    self.cur_time = 0
    self.cur_wifi = 2
    self.status_flush_timer = nil
    Runner.Instance:AddRunObj(self, 1)
    self.login_data = LoginData.Instance:BaseData()
    self.data_cares = {
        {data = self.time_ctrl.delay_time, func = self.FlushWIFI},
        {data = self.scene_data:Base(), func = self.FlushMap},
        {data = self.scene_data.scene_custom_names, func = self.FlushMapName},
        {data = self.login_data, func = self.FlushMapName, keys = {"is_on_crosssever"}, init_call = false},
        {data = self.login_data, func = self.FlushMap, keys = {"is_on_crosssever"}, init_call = false},
        {data = MainViewData.Instance.cur_game_state, func = self.FlushIcons},
        {data = self.map_data.map_guide, func = self.MapGuide, init_call = false},
        {data = RushDungeonData.Instance.layer_info, func = self.FlushRewardPreName, init_call = false},
        {data = MainViewData.Instance.cur_game_state, func = self.FlushRewardPreName, init_call = false},
    }
    if DEBUG_MODE then
        EventSys:Bind(GlobalEventKeys.RolePosChange, BindTool.Bind(self.onMainRolePosChange, self))
    end
    self.last_update_pos_frame = 0
    --PC版关闭wifi和电量
    if IS_STANDALONE() then
        self.WifiSp.transform.parent.gameObject:SetActive(false)
        self.DianLiangImg.transform.parent.gameObject:SetActive(false)
        self.TimeTex.transform.localPosition = Vector3.New(-80,0,0)
    end
end

function MainViewTopLeft:FlushRewardPreName()
    local state = MainViewData.Instance:GetGameState()
    if state == MainViewGameState.rush_dungeon then
        local info = RushDungeonData.Instance.layer_info
        UH.SetText(self.RewardTex, string.format( Language.RushDungeon.LevelReward, info.map_level))
    else
        UH.SetText(self.RewardTex, Language.Fight.RewardDropPreview)
    end
end

function MainViewTopLeft:FlushIcons()
    self.IconsObj:SetActive(MAIN_NOT_SHOW_MAP_ICON[MainViewData.Instance:GetGameState()]==nil)
    self.RewardPreview:SetActive(MAIN_SHOW_REWARD_PREVIEW[MainViewData.Instance:GetGameState()]==true)
end

function MainViewTopLeft:onMainRolePosChange()
    if MainViewData.Instance:BaseData().is_battle == true then
        return
    end
    if MainPlayer == nil then
        return
    end
    if Time.frameCount - self.last_update_pos_frame < 10 then   --降低刷新主界面位置信息的频率，优化GC Alloc
        return
    end
    self.last_update_pos_frame = Time.frameCount
    local pos = MainPlayer:GetPos()
    local cur_scene = SceneCtrl.Instance:CurScene()
    if cur_scene ~= nil and cur_scene.PosToServer ~= nil then
        local x, y = cur_scene:PosToServer(pos)
        local pos_x = DataHelper.GetPreciseDecimal(pos.x, 1)
        local pos_y = DataHelper.GetPreciseDecimal(pos.z, 1)
        UH.SetText(self.PosTex, Format("(%s,%s)(%s,%s)", pos_x, pos_y, x, y))
    end
end

function MainViewTopLeft:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.status_flush_timer = TimeHelper:AddRunTimer(function()
        self.DianLiangImg.fillAmount = Nirvana.DeviceTool.GetBatteryLevel() / 100
    end,10,-1,true)
end

function MainViewTopLeft:OnDisable()
    TimeHelper:CancelTimer(self.status_flush_timer)
    self.status_flush_timer = nil
end

function MainViewTopLeft:FlushMap()
    local config = self.scene_data:SceneConfig()
    if config == nil then
        return
    end
    if nil == self.guide_tips_crystal_map and self.scene_data:GetMode() == SceneModeType.FightCrystal then
        -- if UnityPlayerPrefs.GetInt(PrefKeys.FightCrystal()) == 0 then
        if RoleData.Instance:GetRoleLevel() < 40 then
            local param_t = {
                desc = Language.Guide.FightCrystal,
                pos = Vector2.New(-64, -115),
                -- show = 5,
                arrow = 2,
                arrow_pos = Vector2.New(-62.3,17.2),
                not_destroy = true,
            }
            self.guide_tips_crystal_map = UiGuideTip.New(self.NameTex.gameObject,param_t)
            if self.EffectTool.isActiveAndEnabled then
                self.guide_handle_31 = self.EffectTool:Play(5161001)
            end
        end
            -- UnityPlayerPrefs.SetInt(PrefKeys.FightCrystal(), 1)
        -- end
    elseif nil ~= self.guide_tips_crystal_map and self.scene_data:GetMode() ~= SceneModeType.FightCrystal then
        self.guide_tips_crystal_map:DeleteMe()
        self.guide_tips_crystal_map = nil
        if nil ~= self.guide_handle_31 then
            self.EffectTool:Stop(self.guide_handle_31)
            self.guide_handle_31 = nil
        end
    end
    local custom_scene_name = SceneData.Instance:GetSceneNameCustomBySceneId(SceneData.Instance:SceneId())
    local scene_name = custom_scene_name or SceneData.Instance:Base().scene_name
    if self.login_data.is_on_crosssever then
        UH.SetText(self.NameTex, string.format("%s(%s)", scene_name, Language.Login.CrossRealm))
    else
        UH.SetText(self.NameTex, scene_name)
    end
    local scene_icon_name = MapData.MapIdToIcon(config.id)
    if not StringIsEmpty(scene_icon_name) then
        UH.SetIcon(self.MapIcon, scene_icon_name, ICON_TYPE.MAP)
    end
    self:onMainRolePosChange()
end

function MainViewTopLeft:FlushMapName()
    local custom_scene_name = SceneData.Instance:GetSceneNameCustomBySceneId(SceneData.Instance:SceneId())
    local temp_name = custom_scene_name or SceneData.Instance:Base().scene_name
    if self.login_data.is_on_crosssever then
        UH.SetText(self.NameTex, string.format("%s(%s)", temp_name, Language.Login.CrossRealm))
    else
        UH.SetText(self.NameTex, temp_name)
    end
end
function MainViewTopLeft:Update()
    local server_time = TimeManager:GetServerTime()
    if self.cur_time ~= tonumber(os.date("%M", server_time)) then
        UH.SetText(self.TimeTex, os.date("%H:%M", server_time))
        self.cur_time = tonumber(os.date("%M", server_time))
		EventSys:Fire(GlobalEventKeys.TimeMinuteChange) --游戏时间分钟改变
    end
end

function MainViewTopLeft:FlushWIFI()
    local delay = self.time_ctrl:GetDelayTime()
    local index = 4
    if delay == nil then
        index = 2
    end
    if delay and delay > Delay_Type.Nomal then
        index = 3
    end
    if delay and delay > Delay_Type.Bad then
        index = 2
        PublicPopupCtrl.Instance:Center("网络不稳定")
    end
    if self.cur_wifi ~= index then
        UH.SpriteName(self.WifiSp, "XiTongXinXi_XinHao" .. index)
        self.cur_wifi = index
    end
end
function MainViewTopLeft:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Runner.Instance:RemoveRunObj(self)
    TimeHelper:CancelTimer(self.timer_map_guide)
end

function MainViewTopLeft:OnClickMap()
    if IS_AUDIT_VERSION then
        return
    end
    if nil ~= self.guide_tips_crystal_map then
        self.guide_tips_crystal_map:DeleteMe()
        self.guide_tips_crystal_map = nil
    end
    if nil ~= self.guide_handle_31 then
        self.EffectTool:Stop(self.guide_handle_31)
        self.guide_handle_31 = nil
    end
    if self:BlockMap() then
        return
    end
    ViewMgr:OpenView(MapView)
end

function MainViewTopLeft:OnClickMapMin()
    if IS_AUDIT_VERSION then
        return
    end
    if self:BlockMap() then
        return
    end
    ViewMgr:OpenView(MapView)
    -- local sceneid = SceneData.Instance:SceneId()
    -- if SceneData.Instance:GetMode() ~= SceneModeType.None then
    --     return
    -- end
    -- if not MapData.Instance:IsHasMiniMap(sceneid) then
    --     return
    -- end
    -- if not UnityGameObj.FindGameObjectWithTag("MinimapCamera") then
    --     LogError("当前场景没有MinimapCamera " .. sceneid)
    --     return
    -- end
    -- MapData.Instance:SetSelectSceneId(sceneid)
    -- ViewMgr:OpenView(MapMiniView, sceneid)
end

function MainViewTopLeft:BlockMap()
    local value = false
	local mode = SceneData.Instance:GetMode()
    if not FightConfig.IS_IN_FIGHTFB[mode] and SceneConfigToMode[mode] and not FightConfig.IS_IN_NONE[mode]then
        value = true
        PublicPopupCtrl.Instance:Center(Language.Map.BlockTips[1])
    end
    if MainViewCtrl.Instance:IsInGameState(MainViewGameState.fight_crystal) then
        value = true
        ViewMgr:OpenView(FightMapView)
        -- if FightData.Instance:IsCrystalCommon() then
        --     ViewMgr:OpenView(FightMapView)
        -- else
        --     PublicPopupCtrl.Instance:Center(Language.Map.BlockTips[3])
        -- end
    end
    if ConspiracyCtrl.Instance:IsInConsp() then
        value = true
        ViewMgr:OpenView(FightMapView)
    end
    if EscortCtrl.Instance:IsEscorting() then
        value = true
        PublicPopupCtrl.Instance:Center(Language.Map.BlockTips[2])
    end
    if MainViewCtrl.Instance:IsInGameState(MainViewGameState.arena_match) or MainViewCtrl.Instance:IsInGameState(MainViewGameState.shitu_fb) then
        value = true
        ViewMgr:OpenView(FightMapView)
    end
    if MainViewCtrl.Instance:IsInGameState(MainViewGameState.advanced) then
        value = true
        if FightData.Instance:IsCanOpenMapAdvancedFB() then
            ViewMgr:OpenView(FightMapView)
        else
            PublicPopupCtrl.Instance:Center(Language.Map.BlockTips[1])
        end
    end
    -- if MainViewCtrl.Instance:IsInGameState(MainViewGameState.rush_dungeon) then
    --     value = true
    --     PublicPopupCtrl.Instance:Center(Language.Map.BlockTips[1])
    -- end
    if MainViewCtrl.Instance:IsInGameState(MainViewGameState.madel_fb) then 
        value = true
        ViewMgr:OpenView(FightMapView)
    end 
    if MainViewCtrl.Instance:IsInGameState(MainViewGameState.fight_fantasy) then 
        value = true
        ViewMgr:OpenView(FightMapView)
    end 
    if MainViewCtrl.Instance:IsInGameState(MainViewGameState.splace_anecdote) then 
        value = true
        ViewMgr:OpenView(FightMapView)
    end 
    if MainViewCtrl.Instance:IsInGameState(MainViewGameState.only_fight) then 
        value = true
        ViewMgr:OpenView(FightMapView)
    end
    if MainViewCtrl.Instance:IsInGameState(MainViewGameState.wen_xiang_lou) then 
        value = true
        ViewMgr:OpenView(FightMapView)
    end 
    if MainViewCtrl.Instance:IsInGameState(MainViewGameState.harmony_heaven) then 
        value = true
        ViewMgr:OpenView(FightMapView)
    end 
    if SceneModeType.FightFly2 == mode then 
        value = true
        PublicPopupCtrl.Instance:Center(Language.Map.BlockTips[1])
    end 
    -- if MainViewCtrl.Instance:IsInGameState(MainViewGameState.maze_run) then 
    --     value = true
    --     PublicPopupCtrl.Instance:Center(Language.Map.BlockTips[3])
    -- end
    if YouMingData.Instance:IsInSealScene() then
        value = true
        PublicPopupCtrl.Instance:Center(Language.Map.BlockTips[1])
    end
    return value
end

function MainViewTopLeft:MapGuide()
    if MapConfig.GuideState.hide == self.map_data.map_guide.state then return end
    TimeHelper:CancelTimer(self.timer_map_guide)
    local is_guide = MapConfig.GuideState.guide == self.map_data.map_guide.state
    self.map_data.map_guide.state = MapConfig.GuideState.hide
    local start = Vector3.New(self.ViewSize.rect.width / 2 - 50, 51 - self.ViewSize.rect.height / 2, 0)
    self.GuideEffect:SetObjActive(true)
    local total_time = 0.5
    local per_time = 0.05
    local max_time = total_time / per_time - 1
    local cur_time = 0
    local curve = Game.CurveParabola(start, Vector3.zero, Vector3.New(start.x / 4, start.y / 2),  0.5)

    self.timer_map_guide = TimeHelper:AddCountDownTT(function ()
        self.GuideEffect.localPosition = curve:GetPos(cur_time / max_time)
        cur_time = cur_time + 1
    end,function ()
        curve = nil
        self.EffectTool:Play(10010346)
        self.GuideEffect:SetObjActive(false)
        if is_guide then
            GuideManager.Instance:Start(28)
        end
    end, total_time, per_time)
end

function MainViewTopLeft:OnClickRewardPreview()
    ViewMgr:OpenView(FightRewardView)
end

-- 这个经验条的预设如果改了，那境界那的也要一起改
MainViewExp = MainViewExp or DeclareMono("MainViewExp", UIWFlushPanel)
----Fill=====UIWProgressBarEx
function MainViewExp:MainViewExp()
    self.player_base_data = RoleData.Instance:GetBaseData()
    self.peak_data = PeakRoadData.Instance
    self.now_exp = 0
    self.data_cares = {
        {data = self.player_base_data, func = self.Exp, keys = {"level", "cur_exp"}},
        {data = self.peak_data:PeakData(), func = self.FlushPeakLevel, keys = {"is_open"}}
    }
end
function MainViewExp:FlushPeakLevel()
    UH.SpriteName(self.ProgSp, self.peak_data:IsPeakAndOpen() and "JinDuTiao" or "JingYanTiao")
end
function MainViewExp:Exp(old_vals)
    local old_lv = old_vals and old_vals.level or nil
    local old_exp = old_vals and old_vals.cur_exp or nil

    local cur_lv = self.player_base_data.level
    local next_exp = RoleData.GetNextLevelExp(cur_lv)
    local cur_exp = self.player_base_data.cur_exp
    if next_exp == 0 then
        return
    end
    local cur_rate = cur_exp / next_exp
    if old_lv ~= nil and old_lv < cur_lv then --升级
        self.Fill:AddToProgress(
            1,
            function()
                self.Fill:SetProgress(0)
                self.exp_handler = TimeHelper:AddDelayFrameTimer(
                    function()
                        self.Fill:AddToProgress(cur_rate)
                    end
                )
            end
        )
    elseif old_exp == nil then
        self.Fill:SetProgress(cur_rate)
    else
        self.Fill:AddToProgress(cur_rate)
    end
end

function MainViewExp:FlushExpPos()
    local cur_lv = self.player_base_data.level
    local next_exp = RoleData.GetNextLevelExp(cur_lv)
    local cur_exp = self.player_base_data.cur_exp
    if next_exp == 0 then
        return
    end
    local cur_rate = cur_exp / next_exp
    UH.AnchoredPosition(self.RectExpTopPos ,Vector2.New(self.RectFill.rect.size.x * cur_rate, 0))
end

-- 获取当前角色经验值达到的位置
function MainViewExp:GetExpPos()
    if self.now_exp ~= self.player_base_data.cur_exp then
        self.now_exp = self.player_base_data.cur_exp
        self:FlushExpPos()
    end
    return UH.GetRootPosition(self.RectExpTopPos)
end

function MainViewExp:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.exp_handler)
end

function MainViewExp:OnClick()
    if FlyUpData.Instance:IsLevelLimit() then
        PublicPopupCtrl.Instance:Center(Language.Role.LevelLimit2)
    end
end

local rb_btn_config
local function get_rb_btn_config()
    -- 上下两行 由下到上排 pos=1右下
    if rb_btn_config == nil then
        rb_btn_config = {
            {view = RolePromoteView, text = Language.Main.Promote, sprite = "JinSheng", mod = Mod.RolePromote, open = Mod.RolePromote, pos = 1},
            {view = EquipView, text = Language.Main.ZhuangBei, sprite = "ZhuangBei", mod = Mod.Equip, open = Mod.Equip, pos = 1},
            {view = DivineCostumeView, text = Language.Main.DivineCostume, sprite = "ShenZhuang", mod = Mod.DivineCostume, open = Mod.DivineCostume, pos = 1},
            {view = PartnerView, text = Language.Main.HuoBan, sprite = "HuoBan", mod = Mod.Partner, open = Mod.Partner, pos = 1},
            {view = BicycleView, text = Language.Main.LingQi, sprite = "LingQi", mod = Mod.Bicycle, open = Mod.Bicycle, pos = 1},
            {view = FabaoView, text = Language.Main.FaBao, sprite = "FaBao", open = Mod.Fabao, mod = Mod.Fabao, pos = 1},
            {view = IllustrateView, text = Language.Main.Illustrate, sprite = "TuJian", mod = Mod.Illustrate, open = Mod.Illustrate, pos = 1},
            {view = AthleticsView, text = Language.Main.JingJi, sprite = "JingJi", mod = Mod.Athletics, open = Mod.Athletics, pos = 1},
            {view = ChallengPopupView, text = Language.Main.TiaoZhan, sprite = "TiaoZhan",open = Mod.Challenge, pos = 1, mod = Mod.Challenge},
            -- {view = RoleSettingView, text = Language.Main.SheZhi, sprite = "SheZhi",mod = Mod.Setting,open = Mod.Setting, pos = 1}
            -- {view=GuildList,text=Language.Main.JiaZu,sprite="JiaZu"},
            -- {view=LoveView,text=Language.Main.Love,sprite="Love"},
        }
    end
    
    return rb_btn_config
end
local function get_rb_btn_open_num()
    local num = 0
    local _opened
    for index, value in ipairs(get_rb_btn_config()) do
        _opened = false
        if value.open then
            _opened = FunOpen.Instance:CheckOpen(value.open)
            if value.lock_func then
                _opened = _opened and value.lock_func()
            end
        else
            if value.lock_func then
                _opened = value.lock_func()
            end
        end
        if _opened == true then
            num = num + 1
        end
    end
    return num
end

MainViewRbBtnsRoot = MainViewRbBtnsRoot or DeclareMono("MainViewRbBtnsRoot", UIWFlushPanel)
function MainViewRbBtnsRoot:MainViewRbBtnsRoot()
    self.data_cares = {
        {data = MainViewData.Instance.view_data, func = self.HideRb, keys = {"hide_rb"}, init_call = false},
        {data = MainViewData.Instance.view_data, func = self.ShowRb, keys = {"show_all"}, init_call = false},
        {data = MainViewData.Instance.rb_toggle, func = self.FlushRbToggle, init_call = false},
        {data = MainViewData.Instance.remind_data, func = self.FlushRbRedPoint, keys = {"flush_rb"}, init_call = false},
        {data = BattleData.Instance:BaseData(), func = self.FlushBattle, keys = {"server_fighting"}, init_call = false}
    }
    ViewMgr:CareTouchDownEvent(BindTool.Bind(self.OnTouchDownTrigger, self))
end

function MainViewRbBtnsRoot:Awake()
    UIWFlushPanel.Awake(self)
    if IS_STANDALONE() then
        UH.LocalScale(self.BtnSwitchRect, STANDALONE_UI_VECTOR3)
    end
end

function MainViewRbBtnsRoot:OnTouchDownTrigger()
    -- LogDG(ViewMgr.touch_down_data.type)
    if GuideManager.Instance:IsGuide() then
        TimeHelper:CancelTimer(self.handle_delay)
        self.handle_delay = nil
        return
    end
    if ViewMgr.touch_down_data.type ~= ViewManager.TouchDownType.RbButton then
        -- if self.handle_delay == nil then
        --     self.handle_delay = TimeHelper:AddDelayTimer(function ()
                if self.RbToggle.isOn == true and not GuideManager.Instance:IsGuide() then
                    self:HideRb()
                end
        --         self.handle_delay = nil
        --     end,2)
        -- end
    else
        TimeHelper:CancelTimer(self.handle_delay)
        self.handle_delay = nil
    end
end
function MainViewRbBtnsRoot:OnPressDown()
    ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.RbButton)
end

function MainViewRbBtnsRoot:FlushBattle()
    -- self.RbBtnAnim.enabled = false
    if BattleData.Instance:BaseData().server_fighting == false then
        return
    end
    self.RbBtnAnim.speed = 3
    self:HideRb()
end
function MainViewRbBtnsRoot:OnToggleRbBtns(flag)
    self:FlushRbRedPoint()
    MainViewData.Instance.rb_toggle.isOn = flag
    self.DrActiveObj:SetActive(not flag)
 end
function MainViewRbBtnsRoot:FlushRbRedPoint()
    local nums = MainViewData.Instance:RbRmindNums()
    self.RbRedPoint:SetNum(0)
    for key, value in pairs(nums) do
        if value > 0 then
            self.RbRedPoint:SetNum(1)
            break
        end
    end
end
function MainViewRbBtnsRoot:HideRb()
    -- LogDG("右下角功能按钮隐藏HideRb")
    self.RbBtnAnim:SetBool("state", false)
    self:OnToggleRbBtns(false)
end
function MainViewRbBtnsRoot:ShowRb()
    -- LogDG("右下角功能按钮展开ShowRb")
    self.RbBtnAnim:SetBool("state", true)
    self:OnToggleRbBtns(true)
end
function MainViewRbBtnsRoot:FlushRbToggle()
    if self.RbToggle.isOn == MainViewData.Instance.rb_toggle.isOn then return end
    self.RbToggle.isOn = MainViewData.Instance.rb_toggle.isOn
end

function MainViewRbBtnsRoot:OnEnable()
    self.RbBtnAnim.speed = 1
    UIWFlushPanel.OnEnable(self)
    if self.first_set == nil then
        Runner.Instance:RunUntilTrue(function ()
            if self.FunctionPanel:IsLoaded() then
                self.RbBtnAnim:Rebind()
                self.FunctionPanel.sub_uobj.gameObject.transform:SetAsFirstSibling() 
                -- TimeHelper:AddDelayFrameTimer(function ()
                --     self.RbBtnAnim:SetBool("state", false)
                --     self.RbBtnAnim:SetTrigger("New Trigger")
                -- end, 5)
                return true
            end
        end)
        self.first_set = true
    end
end




MainViewRbBtnsSub = MainViewRbBtnsSub or DeclareMono("MainViewRbBtnsSub", UIWFlushPanel)
function MainViewRbBtnsSub:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.RbBtnAnim:Rebind()
    self.RbBtnAnim:SetBool("state", false)
    TimeHelper:AddDelayTimer(function ()
        self.RbBtnRoot:FlushRbRedPoint()
    end, 2)
end

--================================MainViewUpgrade================================
MainViewUpgrade = MainViewUpgrade or DeclareMono("MainViewUpgrade", UIWFlushPanel)

function MainViewUpgrade:MainViewUpgrade()
    self.data_cares = {
        {data = MainViewData.Instance.upgrade_list, func = self.FlushUpgradeList}
    }

    GuideManager.Instance:RegisterGetGuideUi(
        "RoleAttrImprove",
        function()
            if self.UpgradeList:SetupCompleted() then
                for k, v in pairs(self.UpgradeList.item_list) do
                    if v.Name.text == Language.Main.UpgradeRemind.RoleAttrImprove then
                        return v.ClickEvent
                    end
                end
            end
            return nil, nil
        end
    )

    GuideManager.Instance:RegisterGetGuideUi(
        "NewSkillUnlocked",
        function()
            if self.UpgradeList:SetupCompleted() then
                for k, v in pairs(self.UpgradeList.item_list) do
                    if v.Name.text == Language.Main.UpgradeRemind.NewSkillUnlocked then
                        return v.ClickEvent
                    end
                end
            end
            return nil, nil
        end
    )
end

function MainViewUpgrade:OnEnable()
    UIWFlushPanel.OnEnable(self)
    PetData.Instance:TriggerBreachRemind()
end

function MainViewUpgrade:FlushUpgradeList()
    -- LogError(MainViewData.Instance.upgrade_list)
    -- local temp = {} 
    -- for i,v in ipairs(MainViewData.Instance.upgrade_list) do
    --     temp[Language.Main.UpgradeRemindSort[v.text]] = v
    -- end
    table.sort(MainViewData.Instance.upgrade_list._val, DataSorter.KeyLowerSorter("order"))
    local list_data = {}
    if table.nums(MainViewData.Instance.upgrade_list) >= 7 then
        for i = 1, 6 do
            list_data[i] = MainViewData.Instance.upgrade_list[i]
        end
        table.insert(list_data, MainViewData.Instance.upgrade_list[table.nums(MainViewData.Instance.upgrade_list)])
    else
        list_data = MainViewData.Instance.upgrade_list
    end
    self.UpgradeList:SetData(list_data)
    if self.on_com_runner then
        Runner.Instance:RemoveRunUntilTrue(self.on_com_runner)
    end
    self.on_com_runner =
        Runner.Instance:RunUntilTrue(
        function()
            if self.UpgradeList:SetupCompleted() then
                for i, v in ipairs(self.UpgradeList.item_list) do
                    v.Line:SetActive(i ~= #self.UpgradeList.item_list)
                end
                self.on_com_runner = nil
                return true
            end
        end
    )
end
--================================MainViewUpgradeItem================================
MainViewUpgradeItem = MainViewUpgradeItem or DeclareMono("MainViewUpgradeItem", UIWidgetBaseItem)

function MainViewUpgradeItem:SetData(data)
    self.Name.text = data.text
    if self.Show1 and self.Show2 and data.info then
        local info = data.info
        self.Show1:SetActive(info.open_panel ~= Mod.BianQiang.Main)
        self.Show2:SetActive(info.open_panel == Mod.BianQiang.Main)
    end
    self.on_click_upgrade = data.on_click
    self.RedPoint:SetActive(data.red)
end

function MainViewUpgradeItem:OnClickUpgrade()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    self.on_click_upgrade()
    self.UpgradeView:SetActive(false)
    self.UpgradeBlock:SetActive(false)
end


--================================MainViewDownRight================================
MainViewDownRight = MainViewDownRight or DeclareMono("MainViewDownRight", UIWFlushPanel)


--------MainViewAuditIconChange------------------------
MainViewAuditIconChange = MainViewAuditIconChange or DeclareMono("MainViewAuditIconChange", UIWFlushPanel)
function MainViewAuditIconChange:MainViewChatQuickAuditIconChange()
end

function MainViewAuditIconChange:Awake()
	UIWFlushPanel.Awake(self)
	if IS_AUDIT_VERSION and GLOBAL_CONFIG:PkgInfo().is_youyan and not StringIsEmpty(self.ImgName) then
		local path, streaming_has = ChannelAgentConsts.GetUIStremingAsset(self.ImgName)
		if streaming_has and self.AuditRawIcon then
			ChannelAgentConsts.SetRawImg(self.AuditRawIcon, path)
			self.AuditRawIconObj:SetActive(true)
			self.IconObj:SetActive(false)
		else
			self.IconObj:SetActive(true)
			self.AuditRawIconObj:SetActive(false)
		end
	end
end

TeamInviteMainPanel = TeamInviteMainPanel or DeclareMono("TeamInviteMainPanel", UIWFlushPanel)
function TeamInviteMainPanel:TeamInviteMainPanel()
    self.data = TeamData.Instance
    self.data_cares = {
        {data = self.data.team_invite_flush, func = self.FlushTeamInvite}
    }
    self.cache_info = nil
end
function TeamInviteMainPanel:FlushTeamInvite()
    if FunOpen.GetConfog().task_id ~= nil and FunOpen.GetConfog().task_id ~= 0 and not TaskData.Instance:GetTaskIsCompleted(FunOpen.GetConfog().task_id) then
        return
    end
    local info = self.data:GetTeamInviteInfo()
    local auto_time = Config.team_cfg_auto.other[1].ignore_time
    if info == nil then
        self.Anima:Play("team_invite_hide")
        return
    end
    local cfg =  Cfg.TeamTypeTargetId(info.team_type)
    if cfg then
        UH.SetText(self.Name, Format(Language.Team.TeamInvites.Name, info.leader_name))
        UH.SetText(self.Target, Format(Language.Team.TeamInvites.Target, cfg.target_name, info.level_low, info.level_high))
        UH.SetText(self.Num, Format(Language.Team.TeamInvites.Num, info.cur_member_num, info.max_member_num))
        self.Time:SetCallBack(function ()
            self.cache_info = nil
            TeamCtrl.Instance:CSTeamRoleRecruitAnswer(2)
            self.Anima:Play("team_invite_hide")
        end)
        if self.cache_info == nil
        or self.cache_info.team_id ~= info.team_id then
            self.Time:TotalTime(auto_time, TimeType.Type_Special_5, Language.Team.TeamInvites.Ignore)
            self.Anima:Play("team_invite_show")
        end
        self.cache_info = info
    end
end
function TeamInviteMainPanel:OnClickJoin()
    self.Time:CloseCountDownTime()
    self.cache_info = nil
    self.Anima:Play("team_invite_hide")
    TeamCtrl.Instance:CSTeamRoleRecruitAnswer(0)
end
function TeamInviteMainPanel:OnClickIgnore()
    self.Time:CloseCountDownTime()
    self.cache_info = nil
    self.Anima:Play("team_invite_hide")
    TeamCtrl.Instance:CSTeamRoleRecruitAnswer(1)
end