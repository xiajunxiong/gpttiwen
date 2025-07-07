FILE_REQ("modules/main_view/main_view_battle_rb")
FILE_REQ("modules/main_view/main_view_battle_rb2")
FILE_REQ("modules/main_view/main_view_battle_scene")
FILE_REQ("modules/main_view/main_view_battle_skill_sel")
FILE_REQ("modules/main_view/main_view_battle_item_sel")
FILE_REQ("modules/main_view/main_view_battle_pet_sel")
FILE_REQ("modules/main_view/main_view_battle_pet_seal")
FILE_REQ("modules/main_view/main_view_battle_fabao_sel")
FILE_REQ("modules/main_view/main_view_battle_strategy_sel")
FILE_REQ("modules/main_view/main_view_battle_top")
FILE_REQ("modules/main_view/main_view_battle_warning")

MainViewBattle = MainViewBattle or DeclareMono("MainViewBattle", UIWFlushPanel)
---RoundTxt===Text
---SkillSel===GameObject
---ItemSel===GameObject
---PetSel===GameObject
---DianLiangImg===Image

function MainViewBattle:MainViewBattle()
    self.status_flush_timer = nil
    self.battle_data = BattleData.Instance
    self.fight_den_data = FightDenData.Instance
    self.base_data = self.battle_data:BaseData()
    self.other_data = self.battle_data:GetOtherData()
    self.guild_data = GuildData.Instance
    self.data_cares = {
        {data = self.base_data, func = self.OnRoundChange, keys = {"cur_round"}},
        {data = self.battle_data:ViewInfo(), func = self.OnOpenedViewChange, keys = {"opened_sub"}},
        -- {data = self.base_data,func = self.OnRoundPhaseChange,keys = {"round_phase"}},
        {data = self.fight_den_data:GetWave(), func = self.OnFightDenWaveChange, keys = {"num"}},
        {data = self.guild_data.honor_zone_nums, func = self.OnZoneNumChange},
        {data = self.base_data, func = self.OnBattleEnter, keys = {"battle_mode"}},
        -- {data = self.base_data, func = self.OnBossCome, keys = {"is_boss"}},
        {data = self.base_data, func = self.OnMoveTypeChange, keys = {"sel_tar_type"}},
        {data = self.base_data, func = self.OnOBStatusChange, keys = {"is_ob"}},
        {data = self.base_data, func = self.OnOBStatusChange, keys = {"embattle_state"}},
        {data = self.base_data, func = self.OnLimitTimeView, keys = {"limit_timestamp"}},
        {data = self.base_data, func = self.OnPlayerIndexChange, keys = {"player_idx"}},
        -- {data = SceneData.Instance.base, func = self.OnSceneModeChange, keys = {"scene_mode"}},
        {data = self.other_data, func = self.PlayBattleEffect, keys = {"play_battle_effect"}},
    }
    self.role_setting_change_handle = EventSys:Bind(GlobalEventKeys.RoleSettingChange, BindTool.Bind(self.RoleSettingChange, self))
    if DEBUG_MODE then
        table.insert(self.data_cares,{data = self.battle_data.debug_logs, func = self.OnDebugLogChange})
    end
    self.battle_effect_callback_list = {}
    self.cur_time = 0
    Runner.Instance:AddRunObj(self, 1)

    -- MainViewBattle.BulletBtn = self.BulletBtn
    -- MainViewBattle.BulletSender = self.BulletSender
     
    GuideManager.Instance:RegisterGetGuideUi("BtnStrategy", function ()
        if self.StrategyBtn.activeSelf == false then
            self.StrategyBtn:SetActive(true)
        end
        return self.BtnStrategy
    end)
    GuideManager.Instance:RegisterGetGuideUi("BtnStrategy2", function ()
        if self.StrategyBtn2.activeSelf == false then
            self.StrategyBtn2:SetActive(true)
        end
        return self.StrategyBtn2
    end)
    if IS_STANDALONE() then
        self.Signal:SetActive(false)
        self.DianLiangImg.transform.parent.gameObject:SetActive(false)
        UH.AnchoredPosition(self.TimeTex.transform, Vector3.New(-80,0,0))
        UH.LocalScale(self.RbFuncsRect, STANDALONE_UI_VECTOR3)
    end
end
function MainViewBattle:PlayBattleEffect()
    for _, eff_data in pairs(self.other_data.battle_effect_play_list) do
        local handle = self.EffectTool:Play(eff_data.eff_key)
        if eff_data.on_begin then
            eff_data.on_begin()
        end
        if eff_data.on_end then
            self.battle_effect_callback_list[handle] = eff_data.on_end
        end
        -- if eff_key == BattleEffectKey.Hit then
        --     self:SwitchDOF(true)
        -- end
    end
    self.battle_data:ClearBattleEffect()
end

function MainViewBattle:OnEnable()
    self:StopRoundAnim()
    UIWFlushPanel.OnEnable(self)
    if IS_EDITOR or PLATFORM ~= PlatformType.Standalone then
        self.status_flush_timer = TimeHelper:AddRunTimer(function() 
            local lv = Nirvana.DeviceTool.GetBatteryLevel()
            self.DianLiangImg.fillAmount = Nirvana.DeviceTool.GetBatteryLevel() / 100
        end,10,-1,true)
    end
end

function MainViewBattle:OnDisable()
    self:StopRoundAnim()
    TimeHelper:CancelTimer(self.status_flush_timer)
    self.status_flush_timer = nil
    for _, callback in pairs(self.battle_effect_callback_list) do
        callback()
    end
    self.battle_effect_callback_list = {}
end

function MainViewBattle:Update()
    if self.cur_time ~= tonumber(os.date("%M")) then
        UH.SetText(self.TimeTex, os.date("%H:%M"))
        self.cur_time = tonumber(os.date("%M"))
    end
end

function MainViewBattle:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    Runner.Instance:RemoveRunObj(self)
end

function MainViewBattle:OnBattleEffectFinished(handle, key, time)
    if self.battle_effect_callback_list[handle] then
        self.battle_effect_callback_list[handle]()
        self.battle_effect_callback_list[handle] = nil
    end
    -- if handle.effId ==  then

    -- end
    -- handle.obj:SetActive(false)
    -- self:SwitchDOF(false)
    -- if handle.effId == BattleEffectKey.Hit then
    -- end
end

local OBingSpName = {
    default = "_locGuanZhanZhong",
    [BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE] = "_LocDuiZhanZhong",
    [BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY] = "_LocDuiZhanZhong",
}
function MainViewBattle:RoleSettingChange(key, value)
    if BattleData.Instance:IsServerFighting() then
        PublicPopupCtrl.Instance:Center(Language.RoleSetting.InBattle)
        return
    end
    if key == ROLE_SETTING_TYPE.SYSTEM_SET_INFO_BATTLE_CONTROL_MODE then
        local is_ob = BattleData.Instance:IsOB() ~= 0 and BattleData.Instance:IsOB() ~= -1
        if RoleSettingData.IsNewMode() then
            self.RBFuncs:SetActive(false)
            self.BattleNewMode:SetActive(self.base_data.battle_mode ~= BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE 
            and self.base_data.battle_mode ~= BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY 
            and not is_ob)
        else
            self.BattleNewMode:SetActive(false)
            self.RBFuncs:SetActive(self.base_data.battle_mode ~= BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE 
            and self.base_data.battle_mode ~= BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY 
            and not is_ob)
        end
    end
end
function MainViewBattle:OnOBStatusChange()
    -- LogDG("OB 变化了", BattleData.Instance:IsOB())
    -- -1是客户端清理数据的
    local is_ob = BattleData.Instance:IsOB() ~= 0 and BattleData.Instance:IsOB() ~= -1
    BuglyReport("观战状态变化", is_ob, "|", BattleData.Instance:IsOB(), "|", RoleId())
    self.OBing:SetActive(is_ob)
    if is_ob then
        self.OBingSp.SpriteName = (OBingSpName[self.base_data.battle_mode] or OBingSpName.default)
    end
    if not BattleData.IsStory() then
        if RoleSettingData.IsNewMode() then
            self.RBFuncs:SetActive(false)
            self.BattleNewMode:SetActive(self.base_data.battle_mode ~= BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE 
            and self.base_data.battle_mode ~= BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY 
            and not is_ob)
        else
            self.BattleNewMode:SetActive(false)
            self.RBFuncs:SetActive(self.base_data.battle_mode ~= BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE 
            and self.base_data.battle_mode ~= BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY 
            and not is_ob)
        end
    end

    local is_in_honor = GuildData.Instance:IsInGuildHonorFB()
    local is_honnor = self.base_data.battle_mode == BATTLE_MODE.BATTLE_MODE_GUILD_HONOR_BATTLE
    if not is_in_honor and is_honnor and is_ob then
        self.EmBattleInfo[5]:SetActive(true)
    else
        local state = BattleData.Instance:BaseData().embattle_state
        self.EmBattleInfo[5]:SetActive(state >= 2 and  is_honnor and is_ob)
    end
    self:SwitchStrategyBtn()
end

function MainViewBattle:OnDebugLogChange()
    self.DebugLog.text = self.battle_data:PackegeDebugLog()
end

function MainViewBattle:OnPlayerIndexChange()
    self:SwitchStrategyBtn()
end

function MainViewBattle:OnClickRestrainInfo()
    if self.guide_tip then
        self.guide_tip:DeleteMe()
        self.guide_tip = nil
    end
    ViewMgr:OpenView(PetRestraintView)
end

function MainViewBattle:SwitchStrategyBtn()
    local is_show = BattleData.Instance:IsShowStrategyBtn()
    if RoleSettingData.IsNewMode() then
        self.LeftStrategy:SetActive(false)
        self.RightStrategy:SetActive(true)
        self.StrategyBtn2:SetActive(is_show)
    else
        self.LeftStrategy:SetActive(true)
        self.RightStrategy:SetActive(false)
        self.StrategyBtn:SetActive(is_show)
    end
end

function MainViewBattle:OnLimitTimeView()
    local battle_mode = self.base_data.battle_mode
    local time = self.base_data.limit_timestamp - TimeHelper.GetServerTime()
    self.LimitTime:SetActive(time > 0 and battle_mode == BATTLE_MODE.BATTLE_MODE_GUILD_FIGHT)
end

-- function MainViewBattle:OnSceneModeChange()
--     local cur_mode = SceneData.Instance:GetMode()
--     local flag = cur_mode == SceneModeType.MazeRun or cur_mode == SceneModeType.CourageChallenge
--     self.anim_speeds:SetActive(flag)
-- end

function MainViewBattle:OnRoundChange()
    local battle_base_data = self.battle_data:BaseData()
    self.RoundTxt.text = string.format(Language.Main.Battle.RoundShow, battle_base_data.cur_round)
    self:FlushBattleSkip()
    if battle_base_data.cur_round > 1 and battle_base_data.cur_round ~= self.last_round then
        -- 播放回合特效
        self.RountTipText.text = string.format("a%sbc", battle_base_data.cur_round)
        local item = self.RoundTipAnim:get_Item("round_tip")
        item.speed = 1
        self.RoundTipAnim:Play("round_tip")
    end
    self.last_round = battle_base_data.cur_round
end

function MainViewBattle:StopRoundAnim()
    self.RoundTipAnim:Play("round_tip")
    local item = self.RoundTipAnim:get_Item("round_tip")
    item.time = 2
    self.RoundTipAnim:Sample()
    self.RoundTipAnim:Stop()
end

-- function MainViewBattle:OnRoundPhaseChange()

-- end

function MainViewBattle:getSubViewObj(view_type)
    if view_type == BattleSubView.Skill then
        return self.SkillSel
    elseif view_type == BattleSubView.Item then
        return self.ItemSel
    elseif view_type == BattleSubView.Pet then
        return self.PetSel
    elseif view_type == BattleSubView.Seal then
        return self.PetSeal
    elseif view_type == BattleSubView.Fabao then
        return self.FabaoSel
    end
    return nil
end

function MainViewBattle:OnOpenedViewChange(keys)
    if keys ~= nil then
        local old = keys.opened_sub
        local old_view = self:getSubViewObj(old)
        if old_view ~= nil then
            MainViewBattle.sub_view_flag = false
            old_view:SetActive(false)
        end
    end
    local open_view = self:getSubViewObj(self.battle_data:ViewInfo().opened_sub)
    if open_view ~= nil then
        MainViewBattle.sub_view_flag = true
        open_view:SetActive(true)
    end
end

function MainViewBattle:BattleNextRoundTest()
    --[[ if BattleSimulator.Instance:IsPlaying() then
        -- BattleSimulator.Instance:PlayNextRound()
    else
        BattleCtrl.Instance:RoundPlayFinish()
    end ]]
    BattleCtrl.Instance:GetStream():Interrupt()
end

function MainViewBattle:BattlePreviousRoundTest()
    if BattleSimulator.Instance:IsPlaying() then
        -- BattleSimulator.Instance:PlayPreviousRound()
    end
end

function MainViewBattle:BattleDebug()
    BattleCtrl.Instance:LogCurWait()
end

function MainViewBattle:BattleTextDebug()
    if BattleData.Instance:BaseData().video_playing then
        if TeamData.Instance:InTeam() and TeamData.Instance:IsLeader() then
            BattleCtrl.BattleReportStopWatching()
        elseif not TeamData.Instance:InTeam() then
            BattleCtrl.BattleReportStopWatching()
        end
    else
        BattleCtrl.Instance:AudienceLeaveBattle()
    end
    TimeHelper:AddDelayFrameTimer(function ()
        BattleCtrl.BattleReportDetailInfo(5004,0)
    end,20)
   --[[  BattleTextDebug = not BattleTextDebug
    if BattleTextDebug then
    Time.timeScale = 0.5
    else
        Time.timeScale = 1
    end ]]
end
function MainViewBattle:BattleExitTest()
    if BattleSimulator.Instance:IsPlaying() then
        BattleSimulator.Instance:Quit()
    else
        if BattleData.Instance:BaseData().video_playing then
            BattleVideo.Instance:ReportQuit()
            BattleData.Instance:BaseData().video_playing = false
            return
        end
        BattleCtrl.Instance:ExitBattle()
    end
end
function MainViewBattle:FlushRunSpeed()
    if self.base_data.play_speed == 0 then
        self.RunSpeedToggle.isOn = false
    end
end
function MainViewBattle:OnClickSpeed(speed_type)
    --没有用了这个
    -- 默认0 2倍速1
    --[[ if TeamData.Instance:InTeam() and TeamData.Instance:MemberNum() >= 2
    and TeamData.Instance:IsFollow() then
        PublicPopupCtrl.Instance:Center(ErrorText[171])
        self.RunSpeedToggle.isOn = false
        return
    end
    if SceneLogic.Instance:MoveCache().guaji == true then
        PublicPopupCtrl.Instance:Center(ErrorText[171])
        self.RunSpeedToggle.isOn = false
        return
    end
    if BattleData.Instance:IsPVP() then
        PublicPopupCtrl.Instance:Center(ErrorText[171])
        self.RunSpeedToggle.isOn = false
        return
    end ]]
    if BattleData.Instance:IsOB() then
        --允许但不记录
    end
    speed_type = speed_type and 1 or 0
    BattleCtrl.Instance:SetAnimSpeedByType(speed_type)
end
function MainViewBattle:OnClickBuffIcon(data)
    local battle_mode = self.base_data.battle_mode
    local buff_id = data and data.buff_id or self.buff_cfg.buff_id
    if battle_mode == BATTLE_MODE.BATTLE_MODE_ARENA then
        ViewMgr:OpenView(BuffInfoTip, {buff_id = buff_id,from_view = "battle"})
    else
        ViewMgr:OpenView(BuffInfoTip, {buff_id = buff_id,from_view = "battle", buff_cfg = self.buff_cfg})
    end
end
local sp_bg = {
    "EmHongDi","EmLvDi","EmHuangDi"
}

--有其他复杂需求就修改
function MainViewBattle:OnBattleEnter()
    local battle_mode = self.base_data.battle_mode
    self:FlushBattleIconView(battle_mode)
    self:FlushBattleSkip()
    self:FlushElementTip()

    self.WorldBoss:SetActive(battle_mode == BATTLE_MODE.BATTLE_MODE_SERVER_BOSS or battle_mode == BATTLE_MODE.BATTLE_MODE_SERVER_BOSS2 or battle_mode == BATTLE_MODE.BATTLE_MODE_SERVER_BOSS3) 
    self.EmBattleInfo[1]:SetActive(false)
    if battle_mode == BATTLE_MODE.BATTLE_MODE_ARENA then
        if ArenaData.Instance.target_list.data[1] then
            local buff_list = ArenaData.Instance:GetBuffList()
            -- self.buff_cfg = Config.buff_list_auto.buff_list[ArenaData.Instance.target_list.data[1].buff_id_list[1]]
            -- UH.SetIcon(self.BuffIconSp, self.buff_cfg.icon_id, ICON_TYPE.BUFF)
            self.BuffIconObj:SetActive(false)
            self.BuffIconListObj:SetActive(true)
            self.BuffIconList:SetData(buff_list)
            self.BuffIconListObj:GetComponent(typeof(UnityEngine.RectTransform)).anchoredPosition = Vector2.New(467.4399,6.360001)
        else
            self.BuffIconObj:SetActive(false)
            self.BuffIconListObj:SetActive(false)
        end
    elseif battle_mode == BATTLE_MODE.BATTLE_MODE_BRAVE_GTOUND then
        local info = RushDungeonData.Instance:SceneInfo()
        if info == nil then
            self.BuffIconObj:SetActive(false)
            self.BuffIconListObj:SetActive(false)
            self.buff_cfg = nil
        else
            self.buff_cfg = RushDungeonData.Instance:BuffCfg(info.real_buff_id)
            if self.buff_cfg.is_show == 0 then
                self.BuffIconObj:SetActive(false)
                self.BuffIconListObj:SetActive(false)
                self.buff_cfg = nil
            else
                UH.SetIcon(self.BuffIconSp, self.buff_cfg.icon_id, ICON_TYPE.BUFF)
                self.BuffIconObj:SetActive(true)
                self.BuffIconListObj:SetActive(false)
            end
        end
    elseif GuildData.Instance:IsInGuildHonorFB() then -- battle_mode == BATTLE_MODE.BATTLE_MODE_GUILD_HONOR_BATTLE and BattleData.Instance:IsOB() == 0 then
        self.EmBattleInfo[1]:SetActive(true)
    else
        self.BuffIconListObj:SetActive(false)
        self.BuffIconObj:SetActive(false)
    end
    if battle_mode == BATTLE_MODE.BATTLE_MODE_WAVE then
        if TaskData.Instance.task_list[CUSTOM_OBJID.FightDen] then
            TaskData.Instance:TraceTask(TaskConfig.TaskType.dragon, CUSTOM_OBJID.FightDen)
        end
    end
    -- if battle_mode == BATTLE_MODE.BATTLE_MODE_CHALLENGE_TEST then
    --     TimeHelper:AddDelayFrameTimer(function ()
    --         self.EffectTool:Play(2161023)
    --     end)
    -- end
    -- self:SwitchStrategyBtn()
    if battle_mode == BATTLE_MODE.BATTLE_MODE_BRAVE_GTOUND then
        local info = RushDungeonData.Instance.layer_info
        UH.SetText(self.RewardTex,Format(Language.RushDungeon.LevelReward, info.map_level))
    else
        UH.SetText(self.RewardTex,Language.Fight.RewardDropPreview)
    end
    if self.origin_fight_hanle ~= nil then
        AudioMgr:StopEffectHandle(self.origin_fight_hanle)
        self.origin_fight_hanle = nil
    end
    if battle_mode == BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT then
        self.origin_fight_hanle = AudioMgr:PlayEffectHandle(AudioType.Bg, "origin_fight")
    end
end
function MainViewBattle:FlushElementTip()
    if self.base_data.monster_group_id == 5506 then
        local param_t = {
            desc = Language.Guide.Ele,
            pos = Vector2.New(97, -88),
            show = 5,
            hide = 10,
            arrow = 2,
            arrow_pos = Vector2.New(-98.26,17.2)
        }
        self.guide_tip = UiGuideTip.New(self.EleObj,param_t)
    end
end
function MainViewBattle:FlushBattleSkip()
    if Time.realtimeSinceStartup - self.battle_data.record_quick_click_time < 2 then
        return
    end
    if self.battle_data:IsServerFighting() == false then
        return
    end
    local battle_mode = self.base_data.battle_mode
    self.SkipBattle:SetActive(self.battle_data:CanSkipBattle() or self.battle_data:CanQuickBattle())
    UH.SetText(self.SkipBattleTex, BATTLE_SHOW_SKIP_DESC[battle_mode] or Language.Guide.Quick)
    if battle_mode == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE then
        UH.AnchoredPosition(self.SkipBattle.transform, Vector2.New(514, -250))
    else
        
        UH.AnchoredPosition(self.SkipBattle.transform, Vector2.New(482, -127))
    end
end
function MainViewBattle:OnClickSkipBattle()
    local round = self.battle_data:QuickBattleRound()
    local battle_mode = self.base_data.battle_mode
    local is_can, is_tip = self.battle_data:CanSkipBattle()
    if self.battle_data:CanQuickBattle() then
        local info = {
            content = string.format(Language.Guide.QuickTip, round),
            confirm = {
                func = function()
                    self.battle_data.can_fast_forward = false
                    BattleCtrl.CSBattleFastForwardReq()
                    self.SkipBattle:SetActive(false)
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            },
            tnr = DTTodayNotRemind.QuickBattle,
            trn_str = Language.Common.LoginNotRemind,
            bg_v2 = Vector2.New(446,280),
            today_v2 = Vector2.New(-100,-18),
            btn_v2 = Vector2.New(0,-88),
            content_v2 = Vector2.New(0, 50),
        }
        PublicPopupCtrl.Instance:DialogTips(info)
    elseif is_can then
        if is_tip then
            local func
            if battle_mode == BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA
            or  battle_mode == BATTLE_MODE.BATTLE_MODE_BATTLEFIELD then
                func = function ()
                    BattleCtrl.CSBattleSurrender()
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            else
                func = function ()
                    BattleCtrl.SendSkipBattle()
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            end
            local info = {
                content = Language.CloudArena.SurrenderTip,
                confirm = {
                    func = func
                },
                tnr = DTTodayNotRemind.CloudArena,
                trn_str = Language.Common.LoginNotRemind,
                today_v2 = Vector2.New(-100,-18),
            }
            PublicPopupCtrl.Instance:DialogTips(info)
        else
            BattleCtrl.SendSkipBattle()
        end
    end
end

function MainViewBattle:OnZoneNumChange()
    local battle_mode = self.base_data.battle_mode
    if battle_mode == BATTLE_MODE.BATTLE_MODE_GUILD_HONOR_BATTLE then
        GuildData.Instance:SetShowZoneId(nil)
        local zone, num = GuildData.Instance:GetZoneNum()
        if zone ~= 0 then
            UH.SpriteName(self.EmBattleInfo[2], sp_bg[zone])
            UH.SetText(self.EmBattleInfo[3], Language.Guild.EmZone[zone])
            UH.SetText(self.EmBattleInfo[4], num)
        end
    end
end

function MainViewBattle:OnClickConsp()
    ViewMgr:OpenView(ConspiracyPreview)
end

function MainViewBattle:OnClickRewardPreview()
    ViewMgr:OpenView(FightRewardView)
end
function MainViewBattle:OnClickWorldBossRank()
    WorldBossCtrl.Instance:Subscribe()
    ViewMgr:OpenView(WorldBossRank)
end
function MainViewBattle:OnFightDenWaveChange()
    UH.SetText(self.FightDenWave, string.format(Language.FightDen.Level, self.fight_den_data.wave.num))
end

function MainViewBattle:OnClickFightDenIcon()
    ViewMgr:OpenView(FightDenReward)
end

function MainViewBattle:OnMoveTypeChange()
    if self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.SEAL_PET then
        self.TTName.text = Language.Main.Battle.SealPet
    elseif self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.USE_FABAO then
        self.TTName.text = Language.Main.Battle.UseFabao
    elseif self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.STRATEGY then
        self.TTName.text = Language.Main.Battle.StrategySel
    elseif self.base_data.sel_tar_type == BATTLE_MOVE_TYPE.SEAL_YOUMING then
        self.TTName.text = Language.Main.Battle.SealYouMing
    else
        self.TTName.text = Language.Main.Battle.Attack
    end
    if RoleSettingData.IsNewMode() then
        self.BtnBack2:SetActive(self.base_data.sel_tar_type ~= BATTLE_MOVE_TYPE.NONE)
    else
        self.BtnBack2:SetActive(false)
    end
end

function MainViewBattle:OnClickChiefBuff()
    local result_info = ChiefElectionData.Instance:GetBuffResult()
    if result_info ~= nil then
        ViewMgr:OpenView(BuffInfoTip,result_info)
    end
end

function MainViewBattle:FlushBattleIconView(battle_mode)
    self.FightDenIcon:SetActive(BattleData.Instance:IsOB() == 0 and battle_mode == BATTLE_MODE.BATTLE_MODE_WAVE)
    self.ConspDrop:SetActive(false)-- BattleData.Instance:IsOB() == 0 and battle_mode == BATTLE_MODE.BATTLE_MODE_MI_YI)
    self.RewardPreview:SetActive(BattleData.Instance:IsOB() == 0 and BATTLE_SHOW_REWARD_PREVIEW[battle_mode]==true)
    self.CurRound:SetActive(BATTLE_SHOW_CUR_ROUND[battle_mode] == true)
    self.ChiefBuff:SetActive(BATTLE_SHOW_CHIEF_BUFF[battle_mode] == true)
end
-----------------------------------
MainViewBattleBuffItem = MainViewBattleBuffItem or DeclareMono("MainViewBattleBuffItem", UIWidgetBaseItem)
function MainViewBattleBuffItem:MainViewBattleBuffItem()
end

function MainViewBattleBuffItem:SetData(data)
    UH.SetIcon(self.BuffIconSp, data.icon_id, ICON_TYPE.BUFF)
    self.data = data
end

--===========================MainViewBattleAnimSpeed===========================
MainViewBattleAnimSpeed = MainViewBattleAnimSpeed or DeclareMono("MainViewBattleAnimSpeed")

function MainViewBattleAnimSpeed:OnDisable()
    if self.on_scene_change then
        SceneData.Instance:Base():Uncare(self.on_scene_change)
        self.on_scene_change = nil
    end
    if self.on_speed_change then
        BattleData.Instance:BaseData():Uncare(self.on_speed_change)
        self.on_speed_change = nil
    end
end

local function setPos(obj, x, y)
    local go = obj.gameObject
    local pos = go:GetLocalPosition()
    if x then
        pos.x = x
    end
    if y then
        pos.y = y
    end
    pos = go:SetLocalPosition(pos)
end

local speed_pos = {
    [1] = {x = 0, y = 0},
    [2] = {x = 0, y = 70},
    [3] = {x = 100, y = 70}
}

function MainViewBattleAnimSpeed:OnChange()
    local cur_mode = SceneData.Instance:GetMode()
    if cur_mode == SceneModeType.MazeRun or cur_mode == SceneModeType.CourageChallenge then
        for i = 1, 3 do
            self.Images[i]:SetObjActive(true)
            if cur_mode == SceneModeType.MazeRun then
                self.Images[i].raycastTarget = false
                if BattleData.Instance:BaseData().play_speed_type + 1 ~= i then
                    self.Images[i]:SetObjActive(false)
                    setPos(self.Images[i], speed_pos[i].x, speed_pos[i].y)
                else
                    setPos(self.Images[i], 0, 0)
                end
            else
                self.Images[i].raycastTarget = true
            end
        end
    else
        for i = 1, 3 do
            self.Images[i]:SetObjActive(false)
        end
    end

    local toggle = self.Toggles[BattleData.Instance:BaseData().play_speed_type + 1]
    if toggle --[[  and toggle:IsActiveAndEnabled() ]] then
        toggle:Trigger()
    end
end

MainViewBattleTopRight = MainViewBattleTopRight or DeclareMono("MainViewBattleTopRight", UIWFlushPanelGroup)
----RoleHead===UIImageSpriteSet
----RoleLv===Text
----PetHead===UIImageSpriteSet
----PetLv===Text
---PetHp=====Slider
---PetMp=====Slider
---RoleHp=====Slider
---RoleMp=====Slider
--PetHeadObj ==OBJ
--LevelNum ==set
--PetHmp=====GameObject
--PetUnFightTxtObj===GameObject
--PetHeadCell========UIWSubstitute[UIWPetHeadCell]
function MainViewBattleTopRight:MainViewBattleTopRight()
    self.data = MainViewData.Instance
    self.pet_data = PetData.Instance
    self.player_data = RoleData.Instance
    self.player_base_data = self.player_data:GetBaseData()
    self.bat_data = self.player_data:AllBattleAttri()
    self.bag_data = BagData.Instance
    self.base_data = BattleData.Instance:BaseData()
    self.peak_data = PeakRoadData.Instance
    self.data_cares = {
        {data = self.bat_data, func = self.RoleHMp, keys = {BattleAttriType.CurHp, BattleAttriType.MaxHp, BattleAttriType.CurMp, BattleAttriType.MaxMp}},
        {data = self.player_base_data, func = self.PlayerOtherInfo, keys = {"level"}},
        -- {data = self.pet_data:GetOtherInfo(), func = self.FightPetChange, keys = {"fight_index"}},
        -- {data = self.pet_data:GetPetList(), func = self.FlushPetRedPoint, init_call = true},
        -- {data = BattleData.Instance:ChInfo(), func = self.RebindChInfo},
        {data = BattleData.Instance:PetList(), func = self.RebindPetInfo},
        {data = BattleData.Instance:BaseData(), keys = {"total_damage"}, func = self.FlushTotalDamage, init_call = true},
        {data = BattleData.Instance:ViewInfo(), keys = {"move_camera"}, func = self.ChangeRecover},
        {data = BattleData.Instance:ViewInfo(), keys = {"quick_skip"}, func = self.RoleHMp, init_call = false},
        {data = BattleData.Instance:BaseData(), keys = {"round"}, func = self.FlushRound, init_call = true},
        {data = BattleData.Instance:BaseData(), func = self.FlushRunSpeed, keys = {"play_speed", "play_speed_type"}},
        -- {data = self.player_base_data, func = self.FlushRunSpeedShow, keys = {"level"}},
        -- {data = TaskData.Instance:GetTaskList(), func = self.FlushRunSpeedShow},
        -- {data = SceneData.Instance:Base(), func = self.FlushRunSpeedShow, keys = {"scene_mode"}},
        {data = BattleData.Instance:BaseData(), func = self.FlushRunSpeedShow, keys = {"server_fighting"}},
        {data = self.base_data, func = self.OnBattleEnter, keys = {"battle_mode"}},
        {data = self.data:BattleBuffList(), func = self.FlushBuffButton},
        {data = self.data:BattleDebuffList(), func = self.FlushDebuffButton},
        {data = self.peak_data:PeakData(), func = self.FlushPeakLevel},
        {data = RoleAddPointData.Instance.red_add_piont, func = self.FlushRoleRedPoint},
        {data = self.player_base_data, func = self.FlushRoleRedPoint},
		{data = AttributeDanData.Instance.attribute_red_piont, func = self.FlushRoleRedPoint},
        {data = AttributeDanData.Instance.attribute_red_piont_pet, func = self.FlushPetPoint},
        {data = BattleData.Instance:BaseData(), func = self.FlushShengQiSkillShow, keys = {"sheng_qi_show"}, init_call = false}
    }
    self.skill_qualitys = {
        [4] = "JiNengDiZi",
        [5] = "JiNengDiCheng",
        [6] = "JiNengDiHong",
    }
    GuideManager.Instance:RegisterGetGuideUi("RunSpeedBtn", function ()
        if self.RunSpeedToggle then
            return self.RunSpeedToggle
        end
        return nil,nil
    end)
    self.remind_handle_role = RemindCtrl.Instance:Remind():RegisterGroup(Mod.Role, BindTool.Bind1(self.FlushRoleRedPoint, self), true)
    self.remind_handle_pet = RemindCtrl.Instance:Remind():RegisterGroup(Mod.Pet, BindTool.Bind1(self.FlushPetPoint, self), true)
end
function MainViewBattleTopRight:OnDestroy()
    UIWFlushPanelGroup.OnDestroy(self)
    if self.remind_handle_role ~= nil then
        RemindCtrl.Instance:Remind():UnRegisterGroup(Mod.Role, self.remind_handle_role)
        self.remind_handle_role = nil
    end
    if self.remind_handle_pet ~= nil then
        RemindCtrl.Instance:Remind():UnRegisterGroup(Mod.Pet, self.remind_handle_pet)
        self.remind_handle_pet = nil
    end
end
function MainViewBattleTopRight:FlushPetPoint()
    local num = RemindCtrl.Instance:Remind():GetGroupNum(Mod.Pet)
    self.PetRed:SetNum(num)
end
function MainViewBattleTopRight:FlushRoleRedPoint()
    --主角还要监听进阶和潜能点
    local num = RemindCtrl.Instance:Remind():GetGroupNum(Mod.Role)
    if RoleAddPointData.Instance:GetRedAddPiontNum() > 0 then
        self.RoleRed:SetNum(1)
	elseif AttributeDanData.Instance:GetRedAttibutePiontNum() > 0 then
        self.RoleRed:SetNum(1)
    else
        self.RoleRed:SetNum(num)
    end
end
function MainViewBattleTopRight:FlushShengQiSkillShow()
    local show = self.base_data.sheng_qi_show
    local info = BattleData.Instance:GetSQSkill()
    local ch_info = BattleData.Instance:SQOwnerInfo(info.grid_idx)
    if show == true and info and ch_info then
        local appear = ch_info.appearance
        if appear then
            local data = {
                avatar_type = appear.avatar_type,
                avatar_id = appear.avatar_id,
                avatar_quality = appear.avatar_quality,
                top_level = ch_info.top_level,
                avatar_level = 0
            }
            self.SQSkillHead:SetData(data)
        end
        -- 显示技能名字
        local config = Cfg.ShengQiZhuDongSkill(info.hallow_skill_id)
        UH.SpriteName(self.SQSkillSp, config.skill_name_ima)
        UH.SpriteName(self.SQSkillQua, self.skill_qualitys[config.skill_quality])
        if info.assist_num > 1 then
            UH.SetText(self.SQSkillDesc,
            string.format(Language.Main.Battle.ShengQiSkill, DataHelper.GetDaXie(info.assist_num)))
        else
            UH.SetText(self.SQSkillDesc,"")
        end
        self.SQSkill:SetActive(true)
        TimeHelper:CancelTimer(self.handle_show_hallow_skill)
        self.handle_show_hallow_skill = TimeHelper:AddDelayTimer(function()
            BattleData.Instance:BaseData().sheng_qi_show = false
        end, 1)
    elseif show == true then
        LogDG("未获取到圣器拥有者信息")
    else
        LogDG("隐藏圣器技能显示")
        self.SQSkill:SetActive(false)
    end
end

function MainViewBattleTopRight:FlushPeakLevel()
    if self.peak_data:IsPeakAndOpen() then
        self.PeakLevel:SetActive(true)
        UH.SetText(self.RoleLv, RoleData.Instance:GetTopLevel())
        UH.Color(self.RoleLv, COLORS.Red8)
    else
        UH.SetText(self.RoleLv, RoleData.Instance:GetRoleLevel())
        self.PeakLevel:SetActive(false)
        UH.Color(self.RoleLv, COLORS.Yellow16)
    end
	AvatarEffect.SetImageEnabled(self.PeakImg,RoleData.Instance:GetRoleAvatarQuality())
end
function MainViewBattleTopRight:FlushRunSpeedShow()
    local flag = FunOpen.Instance:GetFunIsOpened(Mod.RunSpeed.Main)
    if GLOBAL_CONFIG:PkgInfo().agent_id == "pc1" then
        flag = true
    end
    self.SpeedToggle:SetObjActive(flag)
    if flag then
        self.SpeedToggle:SetObjActive(self.base_data.can_modify_speed ~= 0)
    else
        self.SpeedToggle:SetObjActive(BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_ARENA)
    end
    local speed = self.base_data.play_speed

    self.SpeedShow:SetObjActive(flag and self.base_data.can_modify_speed == 0)
    if speed == 2 then
        UH.SpriteName(self.SpeedShow, tostring(speed))
        self.SpeedToggle.isOn = true
    else
        self.SpeedToggle.isOn = false
        self.SpeedShow:SetObjActive(false)
    end
    
    --[[ if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE then
        Time.timeScale = 1
        if self.RunSpeedToggle.isOn == true then
            self.RunSpeedToggle.isOn = false
        end
        self.RunSpeedToggle:SetObjActive(false)
    end ]]
end
function MainViewBattleTopRight:FlushRunSpeed()
    local flag = FunOpen.Instance:GetFunIsOpened(Mod.RunSpeed.Main)
    if GLOBAL_CONFIG:PkgInfo().agent_id == "pc1" then
        flag = true
    end
    if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_ARENA or flag then
        if self.base_data.play_speed == 1 or self.base_data.play_speed == 0.5 then
            self.RunSpeedToggle.isOn = false
            self.SpeedToggle.isOn = false
        elseif self.base_data.play_speed == 2 then
            self.RunSpeedToggle.isOn = true
            self.SpeedToggle.isOn = true
        else
            -- self.RunSpeedToggle.isOn = false
        end
    end
end
function MainViewBattleTopRight:OnClickSpeed(speed_type)
    --[[ if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE then
        self:NoChangeSpeedTip()
        return
    end ]]
    --[[ if BattleData.Instance:BattleMode() == BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY then
        self:NoChangeSpeedTip()
        return
    end ]]
    -- 默认0 2倍速1
    --[[ if BattleData.Instance:BattleMode() ~= BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE and TeamData.Instance:InTeam() and TeamData.Instance:MemberNum() >= 2 then
        self:NoChangeSpeedTip()
        return
    end ]]
    
    if SceneLogic.Instance:MoveCache().guaji == true then
        self:NoChangeSpeedTip()
        return
    end
    -- if BattleData.Instance:BattleMode() ~= BATTLE_MODE.BATTLE_MODE_CLOUD_ARENA
    -- and BattleData.Instance:BattleMode() ~= BATTLE_MODE.BATTLE_MODE_ARENA
     -- and BattleData.Instance:IsPVP() and BattleData.Instance:IsOB() == 0 then
    --        self:NoChangeSpeedTip()
    --      return
    --end
    speed_type = speed_type and 1 or 0
    self.SpeedToggle.isOn = not speed_type
    local key = DTTodayNotRemind.LeaderChangeSpeed
    if not DialogTipsData.Instance:GetTodayValue(key) and TeamData.Instance:IsLeader()
    and self.RunSpeedToggle.isOn == false then
        PublicPopupCtrl.Instance:DialogTips{
            content = Language.Team.LeaderChangeSpeed,
            confirm = {
                func = function ()
                    BattleCtrl.CSBattleAnimationSpeedModify(speed_type)
                    ViewMgr:CloseView(DialogTipsView)
                end
            },
            tnr = key,
        }
        return
    end
    if BattleData.Instance:IsOB() ~= 0 then
        --允许但不记录
    else
        BattleCtrl.CSBattleAnimationSpeedModify(speed_type)
    end
    -- BattleCtrl.Instance:SetAnimSpeedByType(speed_type)
end
function MainViewBattleTopRight:NoChangeSpeedTip()
    -- LogError("NoChangeSpeedTip", self.RunSpeedToggle.isOn)
    if self.RunSpeedToggle.isOn == true then
        PublicPopupCtrl.Instance:Center(ErrorText[171])
        self.RunSpeedToggle.isOn = false
    end
end
function MainViewBattleTopRight:OnClickRole()
    ViewMgr:OpenView(RoleView)
end
function MainViewBattleTopRight:OnClickPet()
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

-- 放弃按钮
function MainViewBattleTopRight:FlushRound()
    local base_data = BattleData.Instance:BaseData()
    local round = base_data.round
    local count = Config.sundries_auto.other[1].give_up_battle
    if base_data.is_ob > 0 then
        self.GiveUpObj:SetActive(false)
    elseif base_data.is_pvp == false and base_data.is_team_up ~= false and TeamData.Instance:IsLeader() then
        self.GiveUpObj:SetActive(true)
        self.GiveUpInter.Interactable = round > count
    elseif base_data.is_pvp == false then
        self.GiveUpObj:SetActive(true)
        self.GiveUpInter.Interactable = round > count
    else
        self.GiveUpObj:SetActive(false)
    end
end
-- 镜头恢复
function MainViewBattleTopRight:ChangeRecover()
    self.RecoverObj:SetActive(BattleData.Instance:ViewInfo().move_camera)
end
-- 总伤害
function MainViewBattleTopRight:FlushTotalDamage()
                                                        --小游戏隐藏战斗UI界面的时候不显示总伤害
    if BattleData.Instance:BaseData().total_damage == 0 or CowBoyCtrl.Instance.mini_game_battle then
        self.TotalDamageObj:SetActive(false)
    else
        self.TotalDamageObj:SetActive(true)
        self.TotalDamage.text = BattleData.Instance:BaseData().total_damage
        TimeHelper:AddDelayTimer(
            function()
                BattleData.Instance:BaseData().total_damage = 0
            end,
            2
        )
    end
end
function MainViewBattleTopRight:OnClickDetail()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
    ViewMgr:OpenView(BattleDetail)
end
function MainViewBattleTopRight:OnClickTeam()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanZhanKai)
    ViewMgr:OpenView(BattleTeam)
end
function MainViewBattleTopRight:OnClickGiveUp()
    if self.GiveUpInter.Interactable == false then
        local count = Config.sundries_auto.other[1].give_up_battle
        PublicPopupCtrl.Instance:Center(string.format( ErrorText[122],count + 1))
        return
    end
    local info = {
        content = Language.Main.Battle.GiveUpTip,
        confirm = {
                func = function ()
                PublicPopupCtrl.Instance:CloseDialogTips()
                BattleCtrl.GiveUp()
                end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end
function MainViewBattleTopRight:OnClickGiveUpInitial()
    local info = {
        content = Language.Main.Battle.GiveUpTip2,
        confirm = {
                func = function ()
                    PublicPopupCtrl.Instance:CloseDialogTips()
                    ViewMgr:CloseView(NpcDialogView)
                    BattleCtrl.GiveUp()
                end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
    -- BattleCtrl.GiveUp()
end
function MainViewBattleTopRight:OnClickUploadBattleMsg()
    BuglyReport("上传战报，角色id:"..RoleData.Instance:GetRoleId())
    BattleSimulator.Instance:UnLoadData()
end
function MainViewBattleTopRight:OnClickBuffButton()
    ViewMgr:OpenView(BuffInfoTip2, {index = 1})
end
function MainViewBattleTopRight:OnClickXingTian()
    ViewMgr:OpenView(LoginRewardView)
end
function MainViewBattleTopRight:FlushBuffButton()
    local list = self.data:BattleBuffList()
    if list:Length() > 0 then
        UH.SetIcon(self.GuildBuffIcon, list[1].icon_id)
        self.BuffBtn:SetActive(true)
        self.XingTianBtn:SetActive(false)
    else
        self.BuffBtn:SetActive(false)
        local is_open,_ = FunOpen.Instance:GetFunIsOpened(Mod.RandActivity.LoginReward)
        self.XingTianBtn:SetActive(is_open and RoleData.Instance:CreateDayEx() == 1)
    end
end
function MainViewBattleTopRight:FlushDebuffButton()
    local list = self.data:BattleDebuffList()
    if list:Length() > 0 then
        UH.SetIcon(self.BuffIcon2, list[1].icon_id)
        self.BuffBtn2:SetActive(true)
        self.XingTianBtn:SetActive(false)
    else
        self.BuffBtn2:SetActive(false)
        local is_open,_ = FunOpen.Instance:GetFunIsOpened(Mod.RandActivity.LoginReward)
        self.XingTianBtn:SetActive(is_open and RoleData.Instance:CreateDayEx() == 1)
    end
end
function MainViewBattleTopRight:OnClickBuffButton2()
    ViewMgr:OpenView(BuffInfoTip2, {index = 2})
end

function MainViewBattleTopRight:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.role_info = BattleData.Instance:GetChInfo(BattleData.Instance:PlayerIdx())
    self.pet_info = BattleData.Instance:GetChInfo(BattleData.Instance:PlayerPetIdx())
    if self.pet_info and self.pet_info.character_type ~= BATTLE_CH_TYPE.PET then
        self.pet_info = nil
    end
    if self.role_info then
        if self.role_info_hmp_care then
            self.role_info:Uncare(self.role_info_hmp_care)
        end
        self.cur_sp = nil
        self.role_info_hmp_care = self.role_info:Care(BindTool.Bind(self.FlushRoleInfo, self), "maxhp", "curhp", "maxmp", "curmp", "spirit_point")
    end
    self:FlushRoleInfo()
    self:FlushPetInfo()
    self:FlushBuffButton()
end
function MainViewBattleTopRight:RebindChInfo()
    self.role_info = BattleData.Instance:GetChInfo(BattleData.Instance:PlayerIdx())
    if self.role_info then
        if self.role_info_hmp_care then
            self.role_info:Uncare(self.role_info_hmp_care)
        end
        self.cur_sp = nil
        self.role_info_hmp_care = self.role_info:Care(BindTool.Bind(self.FlushRoleInfo, self), "maxhp", "curhp", "maxmp", "curmp", "spirit_point")
    end
    self:FlushRoleInfo()
end
function MainViewBattleTopRight:RebindPetInfo()
    self.pet_info = BattleData.Instance:GetChInfo(BattleData.Instance:PlayerPetIdx())
    local no_pet_to_battle = true
    for _, value in ipairs(BattleData.Instance:PetList()) do
        if value.status == BATTLE_PET_STATUS.Zhan then--or value.status == BATTLE_PET_STATUS.Wang then
            no_pet_to_battle = false
        end
    end
    if no_pet_to_battle then
        self.pet_info = nil
    end
    self:FlushPetInfo()
end

function MainViewBattleTopRight:FlushRoleInfo()
    if self.role_info == nil then
        return
    end
    if BattleData.Instance:IsOB() > 0 then
        return
    end
    self:OnHpMpChange(self.RoleHp, self.RoleMp, self.role_info)
    self.RoleHpTxt.text = self.role_info.curhp .. "/" .. self.role_info.maxhp
    self.RoleMpTxt.text = self.role_info.curmp .. "/" .. self.role_info.maxmp
    if self.role_info.spirit_point > -1 then
        self.RoleSpObj:SetActive(true)
        self.RoleHMpBgRect.sizeDelta = Vector2.New(153, 80)
        UH.SetText(self.RoleSpTxt, self.role_info.spirit_point .. "/" .. FightData.FightOtherDate().max_sp)
        
        if self.cur_sp and self.cur_sp < self.role_info.spirit_point then
            self:PlayGetSpiritEffect()
        end
        self.cur_sp = self.role_info.spirit_point
        self.RoleSp.value = self.role_info.spirit_point / FightData.FightOtherDate().max_sp
        if self.spirit_sigh_handle == nil then
            self.spirit_sigh_handle = self.EffectTool:Play(3161073)
        end
        if self.spirit_sp_handle == nil then
            self.spirit_sp_handle = self.EffectTool:Play(3161074)
        end
        if self.spirit_fly_handle == nil then
            self.spirit_fly_handle = self.EffectTool:Play(3161075)
        end
        UH.LocalScale(self.EffFillRect, Vector3.New(self.role_info.spirit_point / FightData.FightOtherDate().max_sp, 1, 1))
    else
        self.RoleSpObj:SetActive(false)
        self.RoleHMpBgRect.sizeDelta = Vector2.New(153, 58)
    end
end
function MainViewBattleTopRight:FlushPetInfo()
    local mode_id = BattleData.Instance:BattleMode()
    self.PetHeadObj:SetActive(not(mode_id == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE or mode_id == BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY))
    self.PetHmp:SetActive(not(mode_id == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE or mode_id == BATTLE_MODE.BATTLE_MODE_MIRAGE_TERRITORY))
    if self.pet_info == nil then
        -- self.PetHead.SpriteName = ""
        self.PetHmp:SetActive(false)
        -- self.PetLvBg:SetActive(false)
        self.PetUnFightTxtObj:SetActive(true)
        -- self.PetLevelFrame:SetActive(false)
        self.PetHeadCell:Clean()
        return
    end
    --[[ if BattleData.Instance:IsOB() > 0 then
        return
    end ]]    
    -- self.PetHmp:SetActive(true)
    self.PetUnFightTxtObj:SetActive(false)
    -- self.PetLvBg:SetActive(true)
    -- self.PetLv.text = tostring(self.pet_info.level)
    -- self.PetLevelFrame:SetActive(true)
    self.PetHeadCell:SetData({
        pet_id = self.pet_info.character_id,
        lv = self.pet_info.level,
        fs_lv = self.pet_info.fly_flag,
        str_lv = self.pet_info.param,
        skin_id = self.pet_info.skin_id,
    })
    -- PetData.SetStrLevel(nil, self.LevelNum, self.pet_info.param)

    self:OnHpMpChange(self.PetHp, self.PetMp, self.pet_info)
    -- self:OnPetIconChange()
end

function MainViewBattleTopRight:OnDisable()
    self.pet_info = nil

    -- self.PetHead.SpriteName = ""
    self.PetHmp:SetActive(false)
    -- self.PetLvBg:SetActive(false)
    self.PetUnFightTxtObj:SetActive(true)
    -- self.PetLevelFrame:SetActive(false)
    self.PetHeadCell:Clean()

    Runner.Instance:RemoveRunUntilTrue(self.sp_eff_fly_handle)
    self.sp_eff_fly_handle = nil
end

function MainViewBattleTopRight:PlayGetSpiritEffect()
    local ch_info = BattleData.Instance:GetChInfo(BattleData.Instance:PlayerIdx())
    if ch_info and ch_info.root_obj then
        local root_obj_pos = ch_info.root_obj.transform.position
        root_obj_pos = root_obj_pos:Clone()

        local from_pos = UH.GetWorldPosToScreen(Vector3.New(root_obj_pos.x, root_obj_pos.y + 1.4, root_obj_pos.z), self.GetSpFlyEffPar:GetComponent(typeof(UnityEngine.RectTransform)))
        self.GetSpFlyEffPar.transform.position = Vector3.New(from_pos.x, from_pos.y,0)
        self.GetSpFlyEffPar:SetActive(true)
        if self.sp_eff_fly_handle ~= nil then
            Runner.Instance:RemoveRunUntilTrue(self.sp_eff_fly_handle)
        end
        self.sp_eff_fly_handle = RunUntilTrue(function()
            local local_pos = self.GetSpFlyEffPar.transform.position
            if GameMath.Equals(local_pos,self.EffFillRect.position) then
                self.GetSpFlyEffPar:SetActive(false)
                Runner.Instance:RemoveRunUntilTrue(self.sp_eff_fly_handle)
                self.sp_eff_fly_handle = nil
            end
            self.GetSpFlyEffPar.transform.position = Vector3.MoveTowards(local_pos, self.EffFillRect.position, 0.5)
        end)
    end
end

-- function MainViewBattleTopRight:OnPetIconChange()
--     if self.pet_info.skin_id ~= nil and self.pet_info.skin_id ~= 0 then
--         local co = Cfg.PetSkinAttr(self.pet_info.skin_id)
--         if co.head_portrait then
--             UH.SetIcon(self.PetHead, co.head_portrait)
--             return
--         end
--     end

--     local cfg = Config.pet_cfg_auto.pet_info_2[self.pet_info.character_id]
--     if not cfg then
--         LogError("缺失宠物配置", self.pet_info.character_id)
--     else
--         UH.SetIcon(self.PetHead, cfg.icon_id)
--     end
-- end

function MainViewBattleTopRight:OnHpMpChange(hpbar, mpbar, info)
    hpbar.value = info.maxhp ~= 0 and info.curhp / info.maxhp or 1
    mpbar.value = info.maxmp ~= 0 and info.curmp / info.maxmp or 1
end

function MainViewBattleTopRight:PlayerOtherInfo()
    -- self.RoleHead.SpriteName = self.player_data:GetHeadSprite()
    -- self.RoleLv.text = self.player_base_data.level
    self:FlushPeakLevel()
end

function MainViewBattleTopRight:RoleHMp()
    local cur_hp = self.player_data:GetBattleAttri(BattleAttriType.CurHp)
    local max_hp = self.player_data:GetBattleAttri(BattleAttriType.MaxHp)
    local cur_mp = self.player_data:GetBattleAttri(BattleAttriType.CurMp)
    local max_mp = self.player_data:GetBattleAttri(BattleAttriType.MaxMp)
    
    self.RoleHp.value = max_hp ~= 0 and cur_hp / max_hp or 1
    self.RoleMp.value = max_mp ~= 0 and cur_mp / max_mp or 1
    self:RebindChInfo()
    self:RebindPetInfo()
end
function MainViewBattleTopRight:RecoverCamera()
    SceneCtrl.Instance:CurScene():RecoverCamera()
end

function MainViewBattleTopRight:OnBattleEnter()
    local battle_mode = BattleData.Instance:BaseData().battle_mode
    self.TopSmalls:SetActive(battle_mode ~= BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT)
    self.TopSmallsInitial:SetActive(battle_mode == BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT)
    self.UploadObj:SetActive(GLOBAL_CONFIG:PkgInfo().agent_id == "dev" or GLOBAL_CONFIG:PkgInfo().agent_id == "ml1")
end

MainViewBattleGuide = MainViewBattleGuide or DeclareMono("MainViewBattleGuide", UIWFlushPanelGroup)
function MainViewBattleGuide:MainViewBattleGuide()
end

MainViewBattleLeft = MainViewBattleLeft or DeclareMono("MainViewBattleLeft", UIWFlushPanelGroup)

function MainViewBattleLeft:MainViewBattleLeft()
    self.data_cares = {
        {data = BattleData.Instance:BaseData(), func = self.OnOBStatusChange, keys = {"is_ob"}},
        {data = BattleData.Instance:BaseData(), func = self.OnBattleEnter, keys = {"battle_mode"}}
    }
end

function MainViewBattleLeft:OnClickQuitOB()
    --判断是否为播放录像是否为队长需告知服务端停止播放
    if BattleData.Instance:BaseData().video_playing then
        if TeamData.Instance:InTeam() and TeamData.Instance:IsLeader() then
            BattleCtrl.BattleReportStopWatching()
        elseif not TeamData.Instance:InTeam() then
            BattleCtrl.BattleReportStopWatching()
        end
    else
        BattleCtrl.Instance:AudienceLeaveBattle()
    end
end

function MainViewBattleLeft:OnOBStatusChange()
    if BattleData.Instance:BaseData().battle_mode ~= BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE then
        self.BtnQuitOB:SetActive(BattleData.Instance:IsOB() ~= 0 and BattleData.Instance:IsOB() ~= -1)
    end
end

function MainViewBattleLeft:OnBattleEnter()
    if BattleData.Instance:BaseData().battle_mode == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE then
        self.BtnQuitOB:SetActive(false)
    end
end

MainViewBattleBulletSender = MainViewBattleBulletSender or DeclareMono("MainViewBattleBulletSender")

function MainViewBattleBulletSender:MainViewBattleBulletSender()
    self.chat_data = ChatCtrl.Instance:Data()
end

function MainViewBattleBulletSender:OnClickClose()
    self:SetObjActive(false)
end

function MainViewBattleBulletSender:OnClickSend()
    local word_num = DataHelper.GetStringWordNum(self.InputField.text)
    if word_num > 30 then
        PublicPopupCtrl.Instance:Center(ErrorText[104])
        return
    end
    if word_num == 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[105])
        return
    end
    if self.chat_data:ChatCacheCheck(self.InputField.text, ChatConfig.ChannelId.bullet) then
		return
	end
    local comm_type = BattleData.Instance:BaseData().server_fighting == true and  ChatConfig.CommentType.COMMENT_TYPE_BATTLE
    or  ChatConfig.CommentType.COMMENT_TYPE_ACT_RACE_HORSE
    if SCENE_TYPE.WEDDING_FB == SceneData.Instance:GetMode() then
        comm_type = ChatConfig.CommentType.COMMENT_TYPE_WEDDING_FB
    end

    local function callback(str)
        BattleCtrl.Instance:SendBulletComment(str or "", comm_type)
        self.InputField.text = ""
    end

    if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(self.InputField.text, 4, callback)
	else
        callback(ChatFilter.Instance:Filter(self.InputField.text))
    end
end

function MainViewBattleBulletSender:OnClickScroll()
    local flag = self.BtnScroll.isOn
    BattleData.Instance:BaseData().bullet_on = flag
    self.ScrollT.localPosition = flag and Vector2.New(20, 0) or Vector2.New(-20, 0)
end

--===============================MainViewBattleTopRightBtn===============================
MainViewBattleTopRightBtn = MainViewBattleTopRightBtn or DeclareMono("MainViewBattleTopRightBtn", UIWFlushPanel)

function MainViewBattleTopRightBtn:MainViewBattleTopRightBtn()
    self.data_cares = {
        {data = BattleData.Instance:BaseData(), func = self.OnBattleView, keys = {"battle_mode"}}
    }
end

function MainViewBattleTopRightBtn:OnBattleView()
    self.DreamNotes:SetActive(BattleData.Instance:IsOB() == 0 and BATTLE_TOP_RIGHT_CONFIG[BattleData.Instance:BattleMode()])
end