BattleScene = BattleScene or BaseClass(BaseScene)
local Input = UnityEngine.Input
local touch_helper = Game.SceneTouchHelper
function BattleScene:__init()
    self.scene_loaded = false
    self.vir_cam_obj = nil
    self.real_vir_cam_obj = nil
    self.center_pos_obj = nil
    self.is_touch_begin = false
    self.first_touch_point = nil

    self.lock_camera_data = {}
    --源
    self.recall_camera_data = nil
    self.speed = 2.5
    --速度
    self.lock_camera_rotate = false
    --锁定
    self.relogin_check = true
    Runner.Instance:AddRunObj(self, 4)
end

function BattleScene:__delete()
    self.vir_cam_obj = nil
    self.center_pos_obj = nil
    Runner.Instance:RemoveRunObj(self)
end

function BattleScene:Update()
    self:updateTouch()
end
function BattleScene:updateTouch()
    if self.is_touch_begin then
        if UnityTouchState.IsTouchUp() then
            GmData.Instance:CheckAndOpen()
            self.is_touch_begin = false
            self.first_touch_point = nil
        else
            local dis = Vector3.Distance(self.first_touch_point, Input.mousePosition)
            if dis > 1 then
                if BattleData.Instance:ViewInfo().move_camera == false then
                    BattleData.Instance:ViewInfo().move_camera = true
                end
                self:updateTouchCamera()
            end
        end
    else
        if not UnityTouchState.IsTouchDown() then
            return
        end
        if UnityTouchState.IsTouchOnUI() then
            ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.CommonUI)
            return
        end
        if self:ModuleTouchBlock() then
            return
        end
        if UnityTouchState.IsTouchDown() then
            self.is_touch_begin = true
            ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.Scene)
            EventSys:Fire(GlobalEventKeys.TOUCH_BEGAN)
            self.first_touch_point = Input.mousePosition
        end
    end
end
function BattleScene:updateTouchCamera()
    if Input.GetMouseButton(0) then
        if self.vir_cam_obj and self.center_pos_obj and self.lock_camera_rotate == false then
            local angle = self.vir_cam_obj.transform.localEulerAngles
            local eY = angle.y
            local eX = angle.x
            local mX = Input.GetAxis("Mouse X")
            local mY = Input.GetAxis("Mouse Y")
            local y = eY + (mX * self.speed)
            local x = eX + (mY * -self.speed)
            --x = x > 180 and x - 360 or x
            --y = y > 180 and y - 360 or y
            local ax = (mY * -self.speed)
            local ay = (mX * self.speed)
            if x > 40 then
                ax = ax + (40 - x)
            elseif x < 10 then
                ax = ax + (10 - x)
            end
            if y > 270 then
                ay = ay - (y + 90)
            elseif y < 90 then
                ay = ay - (y - 90)
            end
            self.vir_cam_obj.transform:RotateAround(
                self.center_pos_obj.localPosition,
                self.vir_cam_obj.transform.right,
                ax
            )
            self.vir_cam_obj.transform:RotateAround(self.center_pos_obj.localPosition, Vector3.up, ay)
        end
    end
end
function BattleScene:PlayBG()
    if BattleData.Instance:IsPVPAudio() then
        AudioMgr:PlayBg("pvp_fight")
    elseif BattleData.Instance:IsBossAudio() then
        AudioMgr:PlayBg("boss_fight")
    else
        AudioMgr:PlayBg("normal_fight")
    end
end
function BattleScene:StartLoading()
    self.scene_loaded = false
    self:PlayBG()
    if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_MINI_GAME_BATTLE then
        BattleLuaFunction.UIHide()
    else
        ViewMgr:MainViewVisible(true)
    end
    ViewMgr:CloseView(NpcDialogView)
    PublicPopupCtrl.Instance:CloseWarningView()

    SceneCtrl.Instance:Objs():Shielding(true)
    GuideManager.Instance:DisableArrowGuide()

    BattleBulletScreenProcesser.Instance:Init()

    local sn = BattleData.Instance:BattleMap()
    if BattleData.Instance:InEmbattleWatching() == true then
        local mode_cfg = Config.world_map_auto.moshi_to_scene[BATTLE_MODE.BATTLE_MODE_GUILD_HONOR_BATTLE]
        sn = mode_cfg and mode_cfg.map_name or sn
        BattleData.Instance:ViewInfo().embattle_watching = false
    end
    -- LogError("BattleData.Instance:BaseData().monster_group_id===",BattleData.Instance:BaseData().monster_group_id)
    ----TEMP_CODE 解决剧情战斗开始时镜头会闪一帧到正式画面的问题
    if StoryCtrl.Instance:IsBattleStory() then
        self.story_prepared = false
        StoryCtrl.Instance:OnBattleBeginLoading(function() 
            self.story_prepared = true
        end)
    else
        self.story_prepared = true
    end
    --------------
    SceneMgr:LoadScene(
        sn,
        function(error)
            if self:Disposed() then
                return
            end
            if not StringIsEmpty(error) then
                Debuger.LogError("BattleScene Load Scene failed===%s:%s", sn, error)
                self.scene_loaded = true
                return
            end
            local battle_ctrl = BattleCtrl.Instance
            TimeHelper:AddDelayFrameTimer(
                function()
                    local ser_data = LuaMonoBehaviour.FindMono(DataHolder)
                    local cam_tran = ser_data.CameraTran
                    if cam_tran then
                        local vir_cam_obj = cam_tran.gameObject
                        if vir_cam_obj then
                            if BattleData.BattleViewData[sn] then
                                self.lock_camera_data.pos = BattleData.BattleViewData[sn].pos
                                self.lock_camera_data.rotate = BattleData.BattleViewData[sn].rotate
                            else
                                if IS_HEXAGON then
                                    self.lock_camera_data.pos = Vector3.New(20.9,43.93,98.01)
                                    self.lock_camera_data.rotate = Vector3.New(40.9964,126.8685,-1.549957)
                                else
                                    self.lock_camera_data.pos = vir_cam_obj.transform.localPosition
                                    self.lock_camera_data.rotate = vir_cam_obj:GetLocalEulerAngles()
                                    if sn ~= "HCZD_1" and sn ~= "SMZD_1" then
                                        self.lock_camera_data.rotate.x = 28.5
                                    end
                                end
                                BattleData.BattleViewData[sn] = {}
                                BattleData.BattleViewData[sn].pos = self.lock_camera_data.pos
                                BattleData.BattleViewData[sn].rotate = self.lock_camera_data.rotate
                            end
                            vir_cam_obj.transform.localPosition = self.lock_camera_data.pos
                            vir_cam_obj:SetLocalEulerAngles(self.lock_camera_data.rotate)

                            self.vir_cam_obj = vir_cam_obj
                            local vir_cam = vir_cam_obj:GetComponent(typeof(Cinemachine.CinemachineVirtualCamera))
                            BattleSkillPool.Instance:SetVirtualCamera(vir_cam)                            
							-- pad上改焦距
                            if DeviceData.ScreenAspect() < 1.5 then
                                local len = vir_cam.m_Lens
                                len.FieldOfView = 25
                                vir_cam.m_Lens = len
                            else
                                local len = vir_cam.m_Lens
                                len.FieldOfView = 22
                                vir_cam.m_Lens = len
                            end
                        end
                    end
                    battle_ctrl:Data():SetSceneData(ser_data)
                    self.center_pos_obj = ser_data.CenterPos
                    self.scene_loaded = true
                    self:PrepareComplete()
                end,
                1
            )

            local quality_obj = SceneMgr.FindGameObjectWithTag("Quality")
            local quality = quality_obj:GetComponent(typeof(Crystal.Quality))
            GpQualityMgr:SetShadowDirection(quality.ShadowDirection)
            GpQualityMgr:SetShadowStrength(quality.Strength)
            MainViewData.Instance:SetBattle(true)
            BattleData.Instance:SetIsBoss()
            GpQualityMgr:SetShadowBattleMode(true)
            StoryCtrl.Instance:EnterBattle()
        end
    )
    self:LoadStream()
end
function BattleScene:LoadStream()
    if self:Disposed() then
        return
    end
    local battle_ctrl = BattleCtrl.Instance
    local stream = battle_ctrl:GetStream()
    self.all_ch_created = false
    stream:Prepare(
        function()
            self.all_ch_created = true
            self:PrepareChComplete()
            self:PrepareComplete()
        end,
        StoryCtrl.Instance:IsBattleStory()
    )
end
function BattleScene:PrepareChComplete()
    local info = StoryData.Instance.story_list[105]
    if info and info.group_id == BattleData.Instance:BaseData().monster_group_id then
        local round_data = BattleData.Instance:GetRoundData()
        local cur_round = BattleData.Instance:BaseData().cur_round
        if not next(round_data) and cur_round == 1 then
            local ch = BattleCtrl.Instance.displayer:getCh(13)
            if ch then
                ch:SetIsShow(false)
            end
            return
        end
    end
    for id, _ in pairs(StoryConfig.SpecialStory) do
        local info = StoryData.Instance.story_list[id]
        if info and info.group_id == BattleData.Instance:BaseData().monster_group_id then
            local round_data = BattleData.Instance:GetRoundData()
            local cur_round = BattleData.Instance:BaseData().cur_round
            if not next(round_data) and cur_round == 1 then
                local ch = BattleCtrl.Instance.displayer:getCh(13)
                if ch then
                    ch:SetIsShow(false)
                end
            end
            break
        end
    end
end
function BattleScene:PrepareComplete()
    if self.scene_loaded and self.all_ch_created then
        local battle_ctrl = BattleCtrl.Instance
        local battle_data = BattleData.Instance
        local stream = battle_ctrl:GetStream()
        local temp = true
        local end_func = function()
            if temp then
                if self:BlockPrepare() == false then
                    BattleCtrl.SendPrepareFinish()
                    if self.relogin_check then
                        stream:OnRoundPhaseChange()
                        self.relogin_check = false
                    end
                end
                battle_ctrl.displayer:CheckBuff()
                temp = false
            end
        end
        for i = 1, 20 do
            if battle_data:IsVideo() then
                battle_ctrl.displayer:FixedPos(i)
            else
                if (i <= 10 or battle_data:IsPVP() or battle_data:OBSide() == 1) then
                    battle_ctrl.displayer:Reinforce(i, end_func)
                else
                    battle_ctrl.displayer:FixedPos(i)
                end
            end
        end
    end
    local flag, cfg = BattleData.BattleTips()
    if flag == true then
        ViewMgr:OpenView(BattleTips, cfg)
    end

    local flag = BattleData.IsHasDamageTips()
    if flag == true then
		ViewMgr:OpenView(BattleDamageTips)
    end	
	
    TeamCtrl.Instance:CheckAndPopBattleSalaryDisplay()
    DuelCtrl.Instance:CheckDuelStartDisplay()
end
function BattleScene:BlockPrepare()
    --注意判断代码的执行顺序
    local value = true
    if ViewMgr:IsOpen(HeroVanBattleStart) and BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_WORLD_TEAM_AREMA then
        value = true
        return value
    end
    if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE then
        value = false
        return value
    end
    if BattleData.Instance:IsOB() == 0 then
        local story_info = StoryCtrl.Instance:PlayStoryByMonsterGroupId(
            BattleData.Instance:BaseData().monster_group_id,
            StoryConfig.TriggerType.BattleStart
        )
        if nil == story_info or story_info == false then
            value = false
            return value
        end
    end
    return value
end

function BattleScene:LoadingComplete()
    return ViewMgr:IsOpen(MainView) and self.scene_loaded and self.story_prepared and self.all_ch_created
end

function BattleScene:ReadyComplete()
    return ViewMgr:IsOpen(MainView) and self.scene_loaded and self.all_ch_created
end

function BattleScene:Dispose(show_loading)
    if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_MINI_GAME_BATTLE then
    end
    BattleLuaFunction.UIShow()
    if show_loading == true and self.vir_cam_obj then
        local virAnim = self.vir_cam_obj:GetOrAddComponent(typeof(UnityEngine.Animation))
        virAnim:Stop()
        self:RecoverCamera()
    end
    BattleLuaFunction.update_until_true = {}
    BattleLuaFunction.fixedupdate_until_true = {}

    BaseScene.Dispose(self)
    Runner.Instance:RemoveRunObj(self)
    BattleData.Instance:ViewInfo().move_camera = false

    --[[ if self.real_vir_cam_obj then
        self.real_vir_cam_obj:SetParent(nil)
        self.real_vir_cam_obj.transform.localPosition = self.lock_camera_data.pos
        self.real_vir_cam_obj:SetLocalEulerAngles(self.lock_camera_data.rotate)
    end ]]
    self.vir_cam_obj = nil
    self.real_vir_cam_obj = nil
    self.center_pos_obj = nil
    Time.timeScale = TimeHelper:TimeScale()
    SceneCtrl.Instance:Objs():Shielding(false)
    ViewMgr:CloseView(BattleDetail)
    ViewMgr:CloseView(BattleTeam)
    ViewMgr:CloseView(BattleChdetail)
    ViewMgr:CloseView(PetRestraintView)
    ViewMgr:CloseView(ConspiracyPreview)
    ViewMgr:CloseView(StoryShowBossView)
    ViewMgr:CloseView(BattleTips)
	ViewMgr:CloseView(BattleDamageTips)
    ViewMgr:CloseView(WorldBossRank)
    PublicPopupCtrl.Instance:CloseDialogTips()
    BattleData.Instance:ClearDetail()
    GpQualityMgr:SetShadowBattleMode(false)
    BattleData.Instance:ClearSceneData()
end
function BattleScene:OnLoadingComplete()    
    ----TEMP_CODE 解决剧情战斗开始时镜头会闪一帧到正式画面的问题
    if StoryCtrl.Instance:IsBattleStory() then
        StoryCtrl.Instance:OnBattleLoadingComplete()
    end
    -----------------------
    Time.timeScale = BattleData.Instance:BaseData().play_speed * TimeHelper:TimeScale()
    if not BattleData.Instance:IsPVP() then --PVE的时候看看有没有哪只怪是可以封印的，有的话给出相应的提示
        local chs = BattleData.Instance:ChInfo()
        for _, ch in pairs(chs) do
            if BattleData.IsEnemyIdx(ch.grid_idx) and ch.character_type == BATTLE_CH_TYPE.MONSTER and ch.level == 1 then
                PublicPopupCtrl.Instance:Center(Language.Main.Battle.CatchAbleHint)
                break
            end
        end
    end
end
--容错
function BattleScene:GetPosHeight()
    return 0
end
--限制
function BattleScene:ModuleTouchBlock()
    if ViewMgr:IsOpen(WaitView) then
        return true
    end
    if ViewMgr:IsOpen(LoadingView) then
        return true
    end
end
--恢复
function BattleScene:RecoverCamera()
    if self.recall_camera_data == nil and self.vir_cam_obj then
        self.recall_camera_data = {}
        self.recall_camera_data.pos = self.vir_cam_obj.transform.localPosition
        self.recall_camera_data.rotate = self.vir_cam_obj:GetLocalEulerAngles()
    end
    if self.lock_camera_data and self.vir_cam_obj then
        self.vir_cam_obj.transform.localPosition = self.lock_camera_data.pos
        self.vir_cam_obj:SetLocalEulerAngles(self.lock_camera_data.rotate)
    end
end
function BattleScene:RecallCamera()
    if self.recall_camera_data and self.vir_cam_obj then
        self.vir_cam_obj.transform.localPosition = self.recall_camera_data.pos
        self.vir_cam_obj:SetLocalEulerAngles(self.recall_camera_data.rotate)
        self.recall_camera_data = nil
        self.lock_camera_rotate = false
    end
end
--锁住
function BattleScene:LockCamera()
    self.lock_camera_rotate = true
end
