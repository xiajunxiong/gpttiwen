FILE_REQ("modules/battle/battle_config")
FILE_REQ("modules/battle/battle_data")
FILE_REQ("modules/battle/battle_stream")
FILE_REQ("modules/battle/battle_displayer")
FILE_REQ("modules/battle/battle_skill_pool")
FILE_REQ("modules/battle/battle_buff_manager")
FILE_REQ("modules/battle/battle_flyer_path")
FILE_REQ("modules/battle/battle_bullet_screen_processer")
FILE_REQ("modules/battle/battle_shot_effect")
FILE_REQ("modules/battle/battle_video")
FILE_REQ("modules/battle/battle_helper")
VIEW_REQ("modules/battle/battle_detail")
VIEW_REQ("modules/battle/battle_team")
VIEW_REQ("modules/battle/battle_chdetail")
VIEW_REQ("modules/battle/battle_simulator")
VIEW_REQ("modules/battle/battle_tips")
VIEW_REQ("modules/battle/battle_salary_display_view")
VIEW_REQ("modules/battle/battle_strategy")
VIEW_REQ("modules/battle/battle_damage_tips")
VIEW_REQ("modules/battle/battle_operation_tip")
VIEW_REQ("modules/battle/hexagon_floor")
 
BattleCtrl = BattleCtrl or BaseClass(BaseCtrl)

function BattleCtrl:__init()
    if BattleCtrl.Instance ~= nil then
        Debuger.LogError("[BattleCtrl] attempt to create singleton twice!")
        return
    end
    BattleCtrl.Instance = self

    self.data = BattleData.New()
    self.displayer = BattleDisplayer.New(self.data)
    self.skill_manager = BattleSkillPool.New(self.displayer, self.data)

    self.stream = BattleStream.New(self.data, self.displayer)
    self.BattleBulletScreenProcesser = BattleBulletScreenProcesser.New()
    self.video = BattleVideo.New()

    self.simulator = BattleSimulator.New()

    self.round_countdown_handle = nil

    self.delay_func_list = {}

    self.BEE = nil

    self:RegisterProtocol(SCBattleInfo, "OnRecvChInfo")
    self:RegisterProtocol(SCBattleRoleInfo, "OnRecvBattleRoleInfo")
    self:RegisterProtocol(SCBattleRoundPhase, "OnRecvRoundPhase")
    self:RegisterProtocol(SCBattleRoundResult, "OnRecvRoundRet")    -- 回合数据包
    self:RegisterProtocol(SCBattleSetAutoResp, "OnRecveSetAutoResp")
    self:RegisterProtocol(SCBattleSetAutoMoveResp, "OnRecvSetAutoMoveResp")
    self:RegisterProtocol(SCBattleResult, "OnRecvResult")
    self:RegisterProtocol(SCBattleRoleOBSwitch, "OnRecvOBSwitch")
    self:RegisterProtocol(SCBattleCharacterBuffList, "OnRecvBattleCharacterBuffList")

    self:RegisterProtocol(CSAnimationFinish)
    self:RegisterProtocol(CSBattleRoleOperate)
    self:RegisterProtocol(CSCountDownFinish)

    self:RegisterProtocol(SCBattleForceFinishFight, "OnRecvForceFinishFight")

    self:RegisterProtocol(CSBattleDemoStart)
    self:RegisterProtocol(CSBattleForceFinishFight)
    self:RegisterProtocol(CSBattleFlyAwayRoleLeaveBattle)
    -- 按战斗模式开始战斗
    self:RegisterProtocol(CSBattleModeFightReq)
    -- 按战斗模式开始切磋
    self:RegisterProtocol(CSChallengeOthersReq)
    -- 请求与他人切磋
    self:RegisterProtocol(CSBattleQieCuoWithOthers)
    -- 收到他人请求与你切磋
    self:RegisterProtocol(SCOthersReqQieCuoWithYou, "OnOthersReqQieCuoWithYou")
    -- 到底要不要切磋
    self:RegisterProtocol(CSBattleQieCuoMessageRep)

    self:RegisterProtocol(SCBattleStart, "OnBattleStart")
    --观战协议
    self:RegisterProtocol(CSWatchBattleReq)

    self:RegisterProtocol(CSBattleSetStrategy)
    self:RegisterProtocol(CSBattleClearStrategy)
    self:RegisterProtocol(SCBattleStrategyInfo, "OnBattleStrategyInfo")

    self:RegisterProtocol(SCBattleRoleStartInput, "OnBattleRoleStartInput")
    self:RegisterProtocol(SCBattleRoleFinishInputNotice, "OnBattleRoleFinishInputNotice")
    self:RegisterProtocol(SCBattleEndForAudience, "OnSCBattleEndForAudience")
    -- self:RegisterProtocol(SCBattleRoleOperate, "OnBattleRoleOperate")

    self:RegisterProtocol(CSBattlePause)
    self:RegisterProtocol(SCBattlePause, "OnSCBattlePause")
    
    self:RegisterProtocol(CSBattleGiveup)
    --开始前准备
    self:RegisterProtocol(CSBattlePrepareFinish)

    self:RegisterProtocol(CSBattleStartNpcDemoFight)

    self:RegisterProtocol(CSBattleChat)
    self:RegisterProtocol(SCBattleChat, "OnSCBattleChat")
    
    --录像相关
    self:RegisterProtocol(CSBattleReportDetailInfo)
    self:RegisterProtocol(CSBattleReportSimpleInfo)
    self:RegisterProtocol(CSBattleReportStopWatching)
    self:RegisterProtocol(SCBattleReportStopWatching, "OnBattleReportStopWatching")
    self:RegisterProtocol(SCBattleReport, "OnBattleReport")
    self:RegisterProtocol(SCBattleReportSimpleInfo, "OnBattleReportSimpleInfo")

    self:RegisterProtocol(SCBattleAutoInfoList,"OnBattleAutoInfoList")
    self:RegisterProtocol(SCBattleAutoInfoChangeNotice,"OnBattleAutoInfoChangeNotice")
    
    --恢复战斗
    self:RegisterProtocol(SCBattleRoleInBattleNotice,"OnBattleRoleInBattleNotice")
    self:RegisterProtocol(CSRoleRejoinBattleScene)
    self:RegisterProtocol(CSRoleUndo)

    self:RegisterProtocol(SCBattleCharacterSpecialEffectList, "OnBattleCharacterSpecialEffectList")
    self:RegisterProtocol(SCBattleCharacterHaloList, "OnBattleCharacterHaloList")

    --玩家战斗统计数据
    self:RegisterProtocol(SCBattleStatisticResult, "OnBattleStatisticResult")

    --结算翻牌请求
    self:RegisterProtocol(CSPokerReq)
    --倍速修改 0 1 2 一 二 半
    self:RegisterProtocol(CSBattleAnimationSpeedModify)
    self:RegisterProtocol(SCBattleAnimationSpeedModify, "OnBattleAnimationSpeedModify")

    --自定义指令
    self:RegisterProtocol(CSCustomInstructionAdd)
    self:RegisterProtocol(CSCustomInstructionRemove)
    self:RegisterProtocol(CSCustomInstructionBatchRemove)
    self:RegisterProtocol(CSBattleSetCustomInstruction)
    self:RegisterProtocol(SCCustomInstructionInfo,"SCCustomInstructionInfo")
    --跳过战斗
    self:RegisterProtocol(CSBattleSkipReq)
    --速战
    self:RegisterProtocol(CSBattleFastForwardReq)
    self:RegisterProtocol(SCBattleFastForwardResp,"SCBattleFastForwardResp")
    --圣器
    self:RegisterProtocol(SCBattleHallowList, "SCBattleHallowList")
    --家族试炼总伤害
    self:RegisterProtocol(SCBattleGuildBossTotalDamage, "SCBattleGuildBossTotalDamage")
    --认输
    self:RegisterProtocol(CSBattleSurrender)
    -- self:registerResidentSkills()
    self:RegisterProtocol(SCBattleMoveBoardcast, "SCBattleMoveBoardcast")

    --神龙信息
    self:RegisterProtocol(SCBattleGodDragonInfo, "OnSCBattleGodDragonInfo")

    --偃月兔护盾信息
    self:RegisterProtocol(SCBattleShieldGridInfo, "OnSCBattleShieldGridInfo")
end

function BattleCtrl:__delete()
    self:UnRegisterProtocol(SCBattleInfo)
    self:UnRegisterProtocol(SCBattleRoleInfo)
    self:UnRegisterProtocol(SCBattleRoundPhase)
    self:UnRegisterProtocol(SCBattleRoundResult)
    self:UnRegisterProtocol(SCBattleSetAutoResp)
    self:UnRegisterProtocol(SCBattleSetAutoMoveResp)
    self:UnRegisterProtocol(SCBattleResult)

    self:UnRegisterProtocol(SCBattleForceFinishFight)

    self:UnRegisterProtocol(SCBattleStart)
    self:UnRegisterProtocol(SCBattleStatisticResult)

    if BattleCtrl.Instance == self then
        BattleCtrl.Instance = nil
    end
end

--注册常驻技能资源 防御，封印等
function BattleCtrl:registerResidentSkills()
    for _, ass_id in pairs(BattleSkillPool.ConstAssetId) do
        self.skill_manager:Register(ass_id)
    end
end

function BattleCtrl:SetStrategy(strategy_type, target_grid_id)
    local ch_info = self.data:GetChInfo(target_grid_id + 1)
    if ch_info and (ch_info.curhp > 0 or strategy_type == BATTLE_STRATEGY.REALIVE) then
        local proc = GetProtocol(CSBattleSetStrategy)
        proc.strategy_type = strategy_type
        proc.target_grid_id = target_grid_id
        SendProtocol(proc)
    end
end

function BattleCtrl:ClearStrategy(target_grid_id, is_clear_all)
    local proc = GetProtocol(CSBattleClearStrategy)
    proc.target_grid_id = target_grid_id
    proc.is_clear_all = is_clear_all and 1 or 0
    SendProtocol(proc)
end

function BattleCtrl:EnableAllObjShadow(enable)
    self.displayer:EnableAllObjShadow(enable)
end

BATTLE_DELAY_REASON = {
    SEAL_PET = 1
}

function BattleCtrl:DelayLock(reason)
    self.delay_func_list[reason] = self.delay_func_list[reason] or {}
end

function BattleCtrl:DelayUnlock(reason)
    if self.delay_func_list[reason] then
        for _, func in ipairs(self.delay_func_list[reason]) do
            func()
        end
    end
    self.delay_func_list[reason] = nil
end

function BattleCtrl:DelayListen(reason, func)
    if self.delay_func_list[reason] then
        table.insert(self.delay_func_list[reason], func)
    else
        func()
    end
end

function BattleCtrl:OnBattleStrategyInfo(protocol)
    if protocol.reason == 0 then
        local temp = {}
        for _, v in ipairs(protocol.info_list) do
            if v.target_grid_idx > 0 and v.strategy_type ~= -1 then
                temp[v.target_grid_idx] = v
            end
        end
        for k, v in pairs(self.data:ChInfo()) do
            v.strategy = temp[k]
            --[[ v.mark_name = temp[k].name
            if v.strategy == 0 then
                v.istr = temp[k].istr
            end ]]
        end
    end
end

function BattleCtrl:OnBattleRoleStartInput(protocol)
    -- local base_data = BattleData.Instance:BaseData()
    -- if base_data.round_phase == BATTLE_ROUND_PHASE.Command then
    --     if protocol.is_first_input_finish == 1 then
    --         base_data.command_i = 2
    --     end
    --     self.skip_cmd_2 = protocol.is_second_input_finish == 1
    -- end
end

function BattleCtrl:OnBattleRoleFinishInputNotice(protocol)
    -- LogError("OnBattleRoleFinishInputNotice")
    local ch_info = self.data:GetChInfo(protocol.grid_idx)
    if ch_info then
        ch_info.inputting = false
    end
    local pet_idx = BattleData.Instance:GetPetIdxByPlayerIdx(protocol.grid_idx)
    local pet_info = self.data:GetChInfo(pet_idx)
    if pet_info then
        pet_info.inputting = false
    end
end

function BattleCtrl:OnSCBattleEndForAudience(protocol)
    --LogDG("强制退出观战")
    self:CancelCountdownTimer()
    if BattleData.Instance:InEmbattleWatching() then
        SceneCtrl.Instance:GotoScene(BattleScene)
    else
        SceneCtrl.Instance:GotoScene(GameScene)
    end
    -- SceneCtrl.Instance:GotoScene(GameScene)
    self.data:BaseData().server_fighting = false
    self.data.real_server_fight = false
    self.stream:Clear()
end
function BattleCtrl:DevOrMl1()
    return GLOBAL_CONFIG:PkgInfo().agent_id == "dev" or GLOBAL_CONFIG:PkgInfo().agent_id == "ml1"
end
function BattleCtrl:OnRecvChInfo(protocol, isSimulating)
    if LoginData.Instance:LoginState() ~= LoginStateType.Logined then
        LogD("当前登录状态不对无法战斗", LoginData.Instance:LoginState())
        return
    end

    if not isSimulating and (self:DevOrMl1() or protocol.battle_mode ~= BATTLE_MODE.BATTLE_MODE_BIG_DIPPER) then
        local ChInfo = TableCopy(protocol, 5)
        ChInfo._class_type = nil
        ChInfo.GetClassType = nil
        ChInfo.DeleteMe = nil
        local SimulationData = {}
        SimulationData.ChInfo = ChInfo
        SimulationData.RoundData = {}
        BattleSimulator.Instance.SimulationData = SimulationData
    end
    self.data:BaseData().server_fighting = true
    self.data.real_server_fight = true
    self.data:BaseData().bullet_on = true
    self.data:BaseData().battle_mode = protocol.battle_mode
    if protocol.battle_mode == BATTLE_MODE.BATTLE_MODE_PLATFORM_BATTLE then
        Time.timeScale = TimeHelper:TimeScale()
    else
        Time.timeScale = TimeHelper:TimeScale()
    end
    self.data:IsOB(protocol.is_ob)
    self.data:OBSide(protocol.ob_side)
    self.data:BaseData().is_pvp = protocol.is_pvp == 1
    self.data:BaseData().is_team_up = protocol.is_team_up == 1
    self.data:BaseData().limit_timestamp = protocol.limit_timestamp
    self.data:BaseData().monster_group_id = protocol.monster_group_id
    self.data:BaseData().can_modify_speed = protocol.can_modify_speed
    self.data:SetDebugLog("monster_group_id", "怪物组id：" .. protocol.monster_group_id)

    if BattleHelper.BATTLE_XINMO_JUQING(protocol.monster_group_id) then
        MainProber:Step(MainProber.STEP_XINMO_BATTLE_START, MainProber.user_id, MainProber.server_id, MainProber.role_name, MainProber.role_id)
    end
    if BATTLE_MODE.BATTLE_MODE_INITIAL_FIGHT == protocol.battle_mode then
        for _, v in pairs(protocol.character_info_list) do
            if v.character_type == BATTLE_CH_TYPE.ROLE then
                local appearance = v.appearance
                local co = Config.npc_demo_auto.image_id[appearance.avatar_type]
                if co then
                    local weapons = string.split(co["job_"..appearance.prof_base],"|")
                    appearance.special_show = {
                        show_weapon = true,
                        main_res = co.tihuan_waiguan,
                        main_color = co.color,
                        weapon_res1 = tonumber(weapons[1]),
                        weapon_res2 = weapons[2] and tonumber(weapons[2]) or nil,
                    }
                end
            end
        end
    end
    for _, v in pairs(protocol.character_info_list) do
        self.data:SetChInfo(v.grid_idx, v)
    end
    self.data:ViewInfo().quick_skip = not self.data:ViewInfo().quick_skip
    if protocol.battle_mode == BATTLE_MODE.BATTLE_MODE_STORY then
        if self:PlayBS() then
            return
        end
    end
    ViewMgr:CloseViewInBattle()
    ViewMgr:TipInBattle()

    if MainPlayer then
        MainPlayer:StopMove() --进入战斗时停止主角移动
    end
    if SceneCtrl.Instance:CurScene():GetClassType() == GameScene and not isSimulating then
        if self.BEE == nil then
            self.BEE = UnityGameObj.FindGameObjectWithTag("BattleEnterBefore")
        end
        if self.BEE then
            local BE = self.BEE:FindChild("BattleEnter")
            BE:SetActive(true)
            if ViewMgr:IsOpen(KeyUseView) then
                ViewMgr:CloseView(KeyUseView)
            end
        else
            self:EnterBattle()
        end
    elseif SceneCtrl.Instance:CurScene():GetClassType() ~= BattleScene then
        self:EnterBattle()
    else
        self:EnterBattle()
    end
end

function BattleCtrl:PlayBS()
    self.data.story_data.is_start = true
    self:EnterBattle()
    self.shot_effect = BattleShotEffect.New(0,function ()
        self.shot_effect:DeleteMe()
        self.shot_effect = nil
        self.stream:OnRoundPhaseChange()
    end)
    Runner.Instance:RunUntilTrue(function ()
        if SceneCtrl.Instance:CurScene():GetClassType() == BattleScene and SceneCtrl.Instance:CurScene():LoadingComplete() 
        and self.shot_effect:IsLoaded() then
            self.shot_effect:Play()
            return true
        end
        return false
    end)
    return true
end

function BattleCtrl:OnRecvOBSwitch(protocol)
    -- if self.stream:IsPlaying() then
    self.delay_ob = protocol.is_ob
    -- else
    --     self.data:IsOB(protocol.is_ob)
    -- end
end
function BattleCtrl:OnRecvBattleRoleInfo(protocol)
    -- LogError("战斗开始下发的主角相关信息:\n", protocol)
    self.data:BaseData().role_side = protocol.role_side
    self.data:SetRoleInfo(protocol)
    self:OnRecveSetAutoResp(protocol)
    self:OnRecvSetAutoMoveResp(protocol, true)
    self:SetAnimSpeedByType(protocol.animation_speed)
end
function BattleCtrl:OnRecvBattleCharacterBuffList(protocol)
    self.data:SetBuffInfo(protocol.character_buff_list)
end

local anim_speed = {
    [ANIMATION_SPEED.NORMAL] = 1,
    [ANIMATION_SPEED.DOUBLE] = 2,
    [ANIMATION_SPEED.HALF] = 0.5
}

function BattleCtrl:SetAnimSpeedByType(speed_type)
    local speed = anim_speed[speed_type]
    self.data:BaseData().play_speed_type = speed_type
    self.data:BaseData().play_speed = speed
    Time.timeScale = speed * TimeHelper:TimeScale()
    -- BattleCtrl.CSBattleAnimationSpeedModify(speed_type)
end

function BattleCtrl:OnRecvRoundPhase(protocol)
    --LogError("OnRecvRoundPhase",self.round , protocol.round_phase, protocol)
    -- Debuger.LogError("OnRecvRoundPhase:\n%s", Debuger.TableToString(protocol))
    local base_data = self.data:BaseData()
    base_data.round = protocol.round
    if protocol.round ~= 0 then
        base_data.cur_round = protocol.round
    end
    self.data:SetRoundPhase(protocol.round_phase)
    if base_data.round_phase == BATTLE_ROUND_PHASE.Command then
        for _, ch_info in pairs(self.data:ChInfo()) do
            if ch_info.character_type == 0 or ch_info.character_type == 1 then
                ch_info.inputting = true
            end
        end
        -- self.data:FabaoCD(-1)
        base_data.second_move = protocol.is_second_move_avaliable
        base_data.cmd_left_time = protocol.next_round_phase_time - math.floor(TimeHelper.GetServerTime())
        if base_data.cmd_left_time > 30 then
            base_data.cmd_left_time = 30
        end
        if base_data.cmd_left_time > 0 then
            self:CancelCountdownTimer()
            self.round_countdown_handle =
                TimeHelper:AddRunTimer(
                function()
                    base_data.cmd_left_time = base_data.cmd_left_time - 1
                    if base_data.cmd_left_time - BattleData.Instance:GetCountDownOffset() < 0 then
                        self:OnLocalCountDownComplete()
                    end
                end,
                1 / TimeHelper:TimeScale(),
                -1,
                false
            )
        else
           self:OnLocalCountDownComplete()
        end
        BattleBulletScreenProcesser.Instance:CheckAll(0)
    else
        self:CancelCountdownTimer()
    end
end

function BattleCtrl:CancelCountdownTimer()
    if self.round_countdown_handle ~= nil then ---other phase,countdown is not need
        TimeHelper:CancelTimer(self.round_countdown_handle)
        self.round_countdown_handle = nil
    end
end

--enter server caculate phase
function BattleCtrl:OnLocalCountDownComplete()
    self:CancelCountdownTimer()
    -- if self.data:BaseData().round_phase == BATTLE_ROUND_PHASE.Command then
    --     self.data:SetRoundPhase(BATTLE_ROUND_PHASE.Server)
    -- end
    --send count down end
    local proc = GetProtocol(CSCountDownFinish)
    SendProtocol(proc)
end

function BattleCtrl:OnRecvRoundRet(protocol)
    -- LogErrorBT("OnRecvRoundRet:\n", protocol)
    self.data:SetRoundData(protocol.round, protocol.root)
    if self:DevOrMl1() or self.data:BattleMode() ~= BATTLE_MODE.BATTLE_MODE_BIG_DIPPER then
        if BattleSimulator.Instance.SimulationData.RoundData then
            if not DEBUG_MODE and protocol.round % 15 == 0 then
                BattleSimulator.Instance.SimulationData.RoundData = {}
            end
            BattleSimulator.Instance.SimulationData.RoundData[protocol.round] = TableCopy(protocol.root, 5)
        end
    end
end

function BattleCtrl:OnRecveSetAutoResp(protocol)
    local is_auto = protocol.is_auto ~= 0
    self.server_auto = is_auto
    self.data:SetAuto(is_auto)
end

function BattleCtrl:OnRecvSetAutoMoveResp(protocol, from_role_info)
    -- Debuger.LogError("OnRecvSetAutoMoveResp:\n%s", Debuger.TableToString(protocol))
    
    local first_move = self.data:AutoBehaviour(1)
    if protocol.notify_reason and protocol.notify_reason == AUTOMOVE_REASON_TYPE.REASON_TYPE_EMPTY_MP_RESET_MOVE then
        if first_move.type ~= protocol.first_move_type then
            self.data:ViewInfo().move1_nomp = not self.data:ViewInfo().move1_nomp
        end
    end
    if protocol.first_move_type ~= 11 or (protocol.first_move_type == 11 and protocol.notify_reason ==AUTOMOVE_REASON_TYPE.REASON_TYPE_AI_SKILL) then
        first_move.type = protocol.first_move_type
        first_move.skill_id = protocol.first_skill_idx
        first_move.skill_lv = protocol.first_skill_level
        if from_role_info then
            for k,v in pairs(self.data.player_skill_info) do
                if first_move.skill_id == v.skill_id then
                    if first_move.skill_lv < v.skill_level then
                        -- LogError("first skill_id",v.skill_id,v.skill_level,first_move.skill_lv)
                        self:SetAutoMove(1, BATTLE_MOVE_TYPE.PERFORM_SKILL, v.skill_id, v.skill_level)
                        -- table.insert(send_auto_max_level,{})
                    end
                    break
                end
            end
        end
    end
    local sec_move = self.data:AutoBehaviour(2)
    if protocol.notify_reason and protocol.notify_reason == AUTOMOVE_REASON_TYPE.REASON_TYPE_EMPTY_MP_RESET_MOVE then
        if sec_move.type ~= protocol.second_move_type then
            self.data:ViewInfo().move2_nomp = not self.data:ViewInfo().move2_nomp
        end
    end
    if protocol.second_move_type ~= 11 or (protocol.second_move_type == 11 and protocol.notify_reason ==AUTOMOVE_REASON_TYPE.REASON_TYPE_AI_SKILL) then
        sec_move.type = protocol.second_move_type
        sec_move.skill_id = protocol.second_skill_idx
        sec_move.skill_lv = protocol.second_skill_level
        if from_role_info then
            for k,v in pairs(self.data.player_skill_info) do
                if sec_move.skill_id == v.skill_id then
                    if sec_move.skill_lv < v.skill_level then
                        -- LogError("second skill_id",v.skill_id,v.skill_level,sec_move.skill_lv)
                        self:SetAutoMove(2, BATTLE_MOVE_TYPE.PERFORM_SKILL, v.skill_id, v.skill_level)
                        -- table.insert(send_auto_max_level,{})
                    end
                    break
                end
            end
        end
    end

    local player_ch = BattleData.Instance:GetChInfo(BattleData.Instance:PlayerIdx())
    if player_ch == nil then return end

    if protocol.first_skill_idx ~= 0 and protocol.first_skill_level ~= 0 then
        local skill_cfg1 = SkillData.GetConfig(protocol.first_skill_idx, protocol.first_skill_level)
        if skill_cfg1 and skill_cfg1.weapon_type ~= 0 and skill_cfg1.weapon_type ~= player_ch.weapon_type then
            self:SetAutoMove(1, BATTLE_MOVE_TYPE.NORMAL_ATTACK)
        end
    end

    if protocol.second_skill_idx ~= 0 and protocol.second_skill_level ~= 0 then
        local skill_cfg2 = SkillData.GetConfig(protocol.second_skill_idx, protocol.second_skill_level)
        if skill_cfg2 and skill_cfg2.weapon_type ~= 0 and skill_cfg2.weapon_type ~= player_ch.weapon_type then
            self:SetAutoMove(2, BATTLE_MOVE_TYPE.NORMAL_ATTACK)
        end
    end
end

-- 需要延迟结束战斗的模式放这里
local delay_finish_battle_mode = {
    -- [BATTLE_MODE.BATTLE_MODE_ARENA] = true,
}

local function needDelayFinish(battle_mode)
    return delay_finish_battle_mode[battle_mode] and true or false
end

-- 真正结束战斗
function BattleCtrl:DelayFinish(func)
    self:DoOnRecvResult(self.result_protocol_cache)
    if func ~= nil then
        func()
    end
end

function BattleCtrl:OnRecvForceFinishFight(protocol)
    JoySeeksFavorsData.Instance:Clear()
    self.is_force_finish = true
    self:OnRecvResult(protocol)
end

function BattleCtrl:OnRecvResult(protocol)
    self.data.real_server_fight = false
    -- FinishCtrl.Instance:Finish({is_win = 0, result_type = RESULT_TYPE.DRAW}, BATTLE_MODE.BATTLE_MODE_JOYSEEKS)
    JoySeeksFavorsData.Instance:Clear()
    -- EndlessTowerCtrl.Instance:CloseView(self.data:BaseData().battle_mode,protocol.battle_info)
    self.data:SpecialEffectList(nil)
    if needDelayFinish(self.data:BaseData().battle_mode) then
        self.result_protocol_cache = protocol
    else
        self:DoOnRecvResult(protocol)
    end
    LiveCtrl.Instance:CheckUnficCard()
    
    --首席在战斗中会弹窗，导致玩家无法进入下一场
    if ChiefCtrl.OpenRespondFunc then
        ChiefCtrl.OpenRespondFunc()
        ChiefCtrl.OpenRespondFunc = nil
    end
end

function BattleCtrl:DoOnRecvResult(protocol)
    -- 战斗演出未完毕所以做了延迟
    self.data:ViewInfo().opened_sub = BattleSubView.None
    TimeHelper:CancelTimer(self.handle_delay)
    self.handle_delay = TimeHelper:AddDelayTimer(function()
        --这里获取的是上一次的
        local battle_mode = self.data:BaseData().battle_mode
        local monster_group_id = self.data:BaseData().monster_group_id
        -- LogDG("battle_mode", battle_mode, "monster_group_id", monster_group_id)
        if BattleHelper.BATTLE_XINMO_JUQING(monster_group_id) then
            MainProber:Step(MainProber.STEP_XINMO_BATTLE_End, MainProber.user_id, MainProber.server_id, MainProber.role_name, MainProber.role_id)
        end

        self:CancelCountdownTimer()
        Time.timeScale = TimeHelper:TimeScale()
        if not NO_SHOW_BATTLE_PROCESS[protocol.battle_info.battle_mode] then
            SceneCtrl.Instance:GotoScene(GameScene)
        end
        self.data:BaseData().server_fighting = false
        self.data:BaseData().bullet_on = false
        PetCtrl.Instance:PopDelayPetShow()
        YouMingData.Instance:PopDelayPetShow()
        self.stream:Clear()
        MainViewData.Instance:ClearBattleBuff()
        MainViewData.Instance:ClearBattleDebuff()
        GuildData.Instance.guild_trial_total_damage = 0
        if not DEBUG_MODE then
            BattleSimulator.Instance.SimulationData = {}
        end
        EventSys:Fire(GlobalEventKeys.BattleEnd)

        --[[ local func = BATTLE_END_SHOW_BOX[battle_mode]
        if func then
            func(monster_group_id, function()
                if protocol and protocol.battle_info then -- 强制结束没有
                    TimeHelper:AddDelayTimer(function()
                        self:RecvResult(protocol.battle_info, protocol.battle_info.battle_mode)
                    end, 0.2)
                end
            end)
            return
        end ]]
        -- 这里block所有操作执行一段 宝箱打开过程
        -- 战斗结果弹出结算界面
        if protocol and protocol.battle_info then -- 强制结束没有
            TimeHelper:AddDelayTimer(function()
                self:RecvResult(protocol.battle_info, protocol.battle_info.battle_mode)
            end, 0.2)
        end
    end, 0.5)
end


function BattleCtrl:OnBattleStart(protocol)
    --判断是否有pvpbuff
    if protocol.pvp_buff_num > 0 then
        local buff_id, buff_level, basic, cfg
        local temp_buff_list = {}
        for i = 1, protocol.pvp_buff_num do
            buff_id = protocol.pvp_buff_list[i].buff_id
            buff_level = protocol.pvp_buff_list[i].buff_level
            basic = BattleData.BuffConfig(buff_id)
            cfg = BattleData.BuffLevelConfig(buff_id, buff_level)
            temp_buff_list[buff_id] = {buff_name = basic.buff_name, icon_id = basic.icon_id, desc = cfg.desc}
            -- MainViewData.Instance:AddBattleBuff({buff_name = basic.buff_name, icon_id = basic.icon_id, desc = cfg.desc})
        end
        --客户端显示特殊处理，650和763的描述合并显示
        if temp_buff_list[650] and temp_buff_list[763] then
            temp_buff_list[650].desc = temp_buff_list[650].desc .."\n"..temp_buff_list[763].desc
            temp_buff_list[763] = nil
        end
        for k,v in pairs(temp_buff_list) do
            MainViewData.Instance:AddBattleBuff(v)
        end
        -- local basic = BattleData.BuffConfig(protocol.pvp_buff_id)
        -- local cfg = BattleData.BuffLevelConfig(protocol.pvp_buff_id, protocol.pvp_buff_level)
        -- MainViewData.Instance:AddBattleBuff({buff_name = basic.buff_name, icon_id = basic.icon_id, desc = cfg.desc})
    end
    local duel_buff = DuelData.Instance:DuelBuff()
    if duel_buff then
        MainViewData.Instance:AddBattleDebuff(duel_buff)
    end
    self.data:SetBattleStart(protocol)
end
function BattleCtrl:OnOthersReqQieCuoWithYou(protocol)
    -- 打开是否切磋
    local info = {
        content = string.format(Language.Common.Battle, protocol.sponsor_name),
        cancel = {
            name = Language.PublicPopup.ReplyType.refuse,
            func = function()
                self:BattleQieCuoMessageRep(protocol.role_id, 0)
                ViewMgr:CloseView(DialogTipsView)
            end
        },
        confirm = {
            name = Language.PublicPopup.ReplyType.agrree,
            func = function()
                self:BattleQieCuoMessageRep(protocol.role_id, 1)
                ViewMgr:CloseView(DialogTipsView)
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

-- function BattleCtrl:OnBattleRoleOperate(protocol)
--     local base_data = self.data:BaseData()
--     if protocol.is_succ == 0 and base_data.round_phase == BATTLE_ROUND_PHASE.Command then
--         PublicPopupCtrl.Instance:Center(ErrorText[54])
--         -- base_data.command_i
--     end
-- end

function BattleCtrl:RequestDemo()
    local proc = GetProtocol(CSBattleDemoStart)
    SendProtocol(proc)

    ------Local Test--------
    -- for i=1,20 do
    -- 	self.data:SetChInfo(i,{})
    -- end
    -- self:EnterBattle()
    ------Local Test End--------
end

function BattleCtrl:RoundPlayFinish()
    if self.delay_ob then
        self.data:IsOB(self.delay_ob)
        self.delay_ob = nil
    end
    local proc = GetProtocol(CSAnimationFinish)
    if BattleData.IsStory() then
        BattleData.Instance.player_skill_info = Config.origin_auto.skill_info
        --剧情逻辑
        if self.data:BaseData().cur_round == 2 then
            ViewMgr:OpenView(GuideBlack)
        else
            self.data:SetRoundPhase(BATTLE_ROUND_PHASE.None)
            TimeHelper:AddDelayTimer(
                function()
                    LoginCtrl.Instance:SendStoryRoundResult()
                end,
                1
            )
        end
        return
    end

    if BattleData.Instance:IsOB() ~= 2 and BattleData.Instance:IsOB() ~= -1 then
        SendProtocol(proc)
    end
    BattleBulletScreenProcesser.Instance:CheckAll(3)
end

function BattleCtrl:EnterBattle()
    -- UnityProfiler.BeginSample("EnterBattle")
    -- UnityProfiler.EndSample()
    SceneCtrl.Instance:GotoScene(BattleScene)
end

function BattleCtrl:ExitBattle()
    local proc = GetProtocol(CSBattleForceFinishFight)
    SendProtocol(proc)
end

function BattleCtrl:AudienceLeaveBattle()
    LogDG("退出观战")
    local proc = GetProtocol(CSBattleFlyAwayRoleLeaveBattle)
    proc.is_leave_team = 0
    SendProtocol(proc)
end

function BattleCtrl:GetStream()
    return self.stream
end

local function canSendMove()
    return BattleData.Instance:IsOB() == 0
    -- and BattleData.Instance:BaseData().round_phase == BATTLE_ROUND_PHASE.Command
end

function BattleCtrl:SendAutoBattle(auto)
    --LogError("!")
    if self.server_auto ~= self.data:BaseData().auto then
        self.data:SetAuto(auto)
        return
    end
    --LogError("!!")
    if self.server_auto == auto then
        return
    end
    --LogError("!!!")
    if not self.data:BaseData().server_fighting then
        return
    end
    local proc = GetProtocol(CSBattleRoleOperate)
    proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_AUTO
    proc.param1 = auto and 1 or 0
    proc.param2 = 0
    proc.param3 = 0
    proc.param4 = 0
    -- LogError("!!!!!!!!!!!!!!!!!!!", BattleData.Instance:BaseData().round_phase)
    if canSendMove() then
        -- LogError("!!!!")
        self.data:SetAuto(auto)
        self.server_auto = auto
        SendProtocol(proc)
    end
end

function BattleCtrl:ForceSetAuto(auto)
    local proc = GetProtocol(CSBattleRoleOperate)
    proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_AUTO
    proc.param1 = auto and 1 or 0
end

--move_i 行为格[1,2]
function BattleCtrl:SetAutoMove(move_i, move_type, p1, p2, p3)
    local proc = GetProtocol(CSBattleRoleOperate)

    if move_type == BATTLE_MOVE_TYPE.PERFORM_SKILL then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_AUTO_PERFORM_SKILL
    elseif move_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_AUTO_NORMAL_ATTACK
    elseif move_type == BATTLE_MOVE_TYPE.NORMAL_DEFENCE then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_AUTO_DEFENSE
    elseif move_type == BATTLE_MOVE_TYPE.RUN_AWAY then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_AUTO_RUNAWAY
    elseif move_type == BATTLE_MOVE_TYPE.CHANGE_POSITION then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_AUTO_CHANGE_POSITION
    elseif move_type == BATTLE_MOVE_TYPE.AI_SKILL then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_AUTO_AI_SKILL
    end

    proc.param1 = move_i - 1
    --[0,1]
    proc.param2 = p1 or 0
    
    proc.param3 = p2 or 0
    
    proc.param4 = p3 or 0
    
    if canSendMove() then
        SendProtocol(proc)
    end

    self:SetAutoMoveLocal(move_i, move_type, p1, p2, p3)
    -- LogErrorBT("SetAutoMove===\n", proc)
end

function BattleCtrl:SetAutoMoveLocal(move_i, move_type, p1, p2, p3)
    local beh = self.data:AutoBehaviour(move_i)
    if beh ~= nil then
        beh.type = move_type
        if beh.type == BATTLE_MOVE_TYPE.PERFORM_SKILL then
            beh.skill_id = p1
            beh.skill_lv = p2
        end
    end
end

--move_i 行为格[1,2]
function BattleCtrl:SetMove(move_i, move_type, p1, p2, p3)
    --LogDG("BattleCtrl:SetMove == ",move_i, move_type, p1, p2, p3)
    if move_type == BATTLE_MOVE_TYPE.STRATEGY then
        if p2 >= 0 then
            if p2 == 99 then
                self:CSBattleSetCustomInstruction(p3, p1)
            else
                self:SetStrategy(p2, p1)
            end
        else
            if p2 == -2 then
                self:ClearStrategy(p1, true)
            else
                self:ClearStrategy(p1, false)
            end
        end
        return
    end
    local proc = GetProtocol(CSBattleRoleOperate)
    
    if move_type == BATTLE_MOVE_TYPE.PERFORM_SKILL then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_PERFORM_SKILL
    elseif move_type == BATTLE_MOVE_TYPE.NORMAL_ATTACK then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_NORMAL_ATTACK
    elseif move_type == BATTLE_MOVE_TYPE.NORMAL_DEFENCE then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_DEFENSE
    elseif move_type == BATTLE_MOVE_TYPE.RUN_AWAY then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_RUNAWAY
    elseif move_type == BATTLE_MOVE_TYPE.CHANGE_POSITION then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_CHANGE_POSITION
    elseif move_type == BATTLE_MOVE_TYPE.USE_ITEM then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_USE_ITEM
    elseif move_type == BATTLE_MOVE_TYPE.CHANGE_PET then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_CHANGE_PET
    elseif move_type == BATTLE_MOVE_TYPE.SEAL_PET then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_SEAL_PET
        self:DelayLock(BATTLE_DELAY_REASON.SEAL_PET)
    elseif move_type == BATTLE_MOVE_TYPE.USE_FABAO then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_USE_FABAO
    elseif move_type == BATTLE_MOVE_TYPE.USE_FAKE_POTION then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_USE_FAKE_POTION
    elseif move_type == BATTLE_MOVE_TYPE.HEART_SKILL then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_PERFORM_HEART_SKILL
    elseif move_type == BATTLE_MOVE_TYPE.CARD_SKILL then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_CARD_SKILL
    elseif move_type == BATTLE_MOVE_TYPE.AI_SKILL then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_AUTO_AI_SKILL
    elseif move_type == BATTLE_MOVE_TYPE.HUANSHOU_SKILL then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_HUAN_SHOU
    elseif move_type == BATTLE_MOVE_TYPE.SEAL_YOUMING then
        proc.operate_type = BATTLE_ROUND_OPERATE_TYPE.SET_MOVE_SEAL_YOUMING
        self:DelayLock(BATTLE_DELAY_REASON.SEAL_YOUMING)
    end

    proc.param1 = move_i - 1
    --[0,1]
    proc.param2 = p1 or 0

    proc.param3 = p2 or 0

    proc.param4 = p3 or 0


    if canSendMove() then
        SendProtocol(proc)
    end
    self.data:CmdComplete(move_type, p1, p2, p3)
    -- LogErrorBT("SetMove===\n" .. Debuger.TableToString(proc))
end

function BattleCtrl:EnterSelectTargetStateByAtk()
    self:EnterSelectTargetState(BATTLE_MOVE_TYPE.NORMAL_ATTACK)
end

function BattleCtrl:EnterSelectTargetState(move_type, param1, param2, param3)
    -- LogErrorHL("EnterSelectTargetState", move_type, param1, param2)
    -- LogError(move_type, param1, param2, param3)
    local base_data = self.data:BaseData()
    base_data.sel_tar_type = move_type
    base_data.sel_tar_param1 = param1
    base_data.sel_tar_param2 = param2
    base_data.sel_tar_param3 = param3--临时存储
end

function BattleCtrl:ExitSelectTargetState(sel_idx)
    local base_data = self.data:BaseData()
    if sel_idx ~= nil then
        if base_data.sel_tar_type == BATTLE_MOVE_TYPE.USE_ITEM and base_data.sel_tar_param3 == ItemUseType.MPMedicine then
            PublicPopupCtrl.Instance:Center(string.format(ErrorText[145], base_data.mp_item_can_use_tiems - 1))
            self.data:SetMpCanUseTime(base_data.mp_item_can_use_tiems - 1,true)
        end
        if base_data.sel_tar_type == BATTLE_MOVE_TYPE.SEAL_PET or base_data.sel_tar_type == BATTLE_MOVE_TYPE.SEAL_YOUMING then
            local ch_info = self.data:GetChInfo(sel_idx)
            if ch_info.level == 1 then
                self:SetMove(base_data.command_i, base_data.sel_tar_type, sel_idx - 1, base_data.sel_tar_param1, ch_info.character_id)
            end
        elseif base_data.sel_tar_type == BATTLE_MOVE_TYPE.HUANSHOU_SKILL then
            local tar_type = base_data.sel_tar_type ~= BATTLE_MOVE_TYPE.NONE and base_data.sel_tar_type or BATTLE_MOVE_TYPE.NORMAL_ATTACK
            self:SetMove(base_data.command_i, tar_type, sel_idx - 1, base_data.sel_tar_param2)
        else
            local tar_type = base_data.sel_tar_type ~= BATTLE_MOVE_TYPE.NONE and base_data.sel_tar_type or BATTLE_MOVE_TYPE.NORMAL_ATTACK
            self:SetMove(base_data.command_i, tar_type, sel_idx - 1, base_data.sel_tar_param1, base_data.sel_tar_param2)
        end
        if base_data.sel_tar_type == BATTLE_MOVE_TYPE.USE_FABAO then
            PublicPopupCtrl.Instance:Center(string.format(Language.Main.Battle.FabaoUseSucc, BattleData.Instance:FabaoInfoChange() - 1))
        end 
    end
    base_data.sel_tar_type = BATTLE_MOVE_TYPE.NONE
    base_data.sel_tar_param1 = nil
    base_data.sel_tar_param2 = nil
    base_data.sel_tar_param3 = nil
end

function BattleCtrl.BattleModeFightReq(param_t)
    local proc = GetProtocol(CSBattleModeFightReq)
    proc.battle_mode = param_t.battle_mode
    proc.reserve_sh = param_t.reserve_sh or 0
    proc.param1 = param_t.param1 or 0
    proc.param2 = param_t.param2 or 0
    proc.param3 = param_t.param3 or 0
    SendProtocol(proc)
end
function BattleCtrl.ChallengeOthersReq(battle_mode, role_id)
    local proc = GetProtocol(CSChallengeOthersReq)
    proc.battle_mode = battle_mode or 0
    proc.role_id = role_id or 0
    SendProtocol(proc)
end
function BattleCtrl.BattleQieCuoWithOthers(role_id)
    local proc = GetProtocol(CSBattleQieCuoWithOthers)
    proc.role_id = role_id
    SendProtocol(proc)
end
function BattleCtrl:BattleQieCuoMessageRep(role_id, result)
    local proc = GetProtocol(CSBattleQieCuoMessageRep)
    proc.reply_role_id = role_id
    proc.rep_result = result
    SendProtocol(proc)
end
-------------Debug log--------------------
BattleCtrl.logStr = ""
local loging = false
function BattleCtrl:LogCurWait()
    -- BattleCtrl.logStr = ""
    -- loging = true
    -- Runner.Instance:RunUntilTrue(function()
    -- 		if not StringIsEmpty(BattleCtrl.logStr) then
    -- 			Debuger.LogError(tostring(BattleCtrl.logStr))
    -- 			loging = false
    -- 			return true
    -- 		else
    -- 			return false
    -- 		end
    -- 	end, 1)
    -- 先去掉 放误点
    -- self.stream:logPlayingQueue()
end

function BattleCtrl.WriteLog(log)
    if loging == false then
        return
    end
    if BattleCtrl.logStr == nil then
        BattleCtrl.logStr = ""
    end
    BattleCtrl.logStr = BattleCtrl.logStr .. log .. "\n"
end

--观战请求 id_type == 0 and uid or battle_id
function BattleCtrl:SendWatchBattleReq(uid, id_type, watch_side)
    local protocol = GetProtocol(CSWatchBattleReq)
    protocol.uid = uid or 0
    protocol.watch_side = watch_side or 0
    protocol.id_type = id_type or 0
    SendProtocol(protocol)
end

function BattleCtrl:SetPause(pause)
    if IS_AUDIT_VERSION then
        return
    end
    local proc = GetProtocol(CSBattlePause)
    proc.is_pause = pause or 0
    SendProtocol(proc)
end
function BattleCtrl.GiveUp()
    local proc = GetProtocol(CSBattleGiveup)
    SendProtocol(proc)
end
function BattleCtrl:OnSCBattlePause(protocol)
end

function BattleCtrl.SendPrepareFinish()
    local proc = GetProtocol(CSBattlePrepareFinish)
    SendProtocol(proc)
end

function BattleCtrl.SendBattleStartNpcDemoFight(seq, is_initial_fight)
    local proc = GetProtocol(CSBattleStartNpcDemoFight)
    proc.npc_demo_seq = seq
    proc.is_initial_fight = is_initial_fight or 0
    SendProtocol(proc)
end

function BattleCtrl:SendBulletComment(content, comment_type)
    local proc = GetProtocol(CSBattleChat)
    proc.comment_type = comment_type or ChatConfig.CommentType.COMMENT_TYPE_BATTLE
    proc.msg_length = string.len(content)
    proc.content = content
    SendProtocol(proc)
end
local bullet_color_str =
{
    COLORSTR.Yellow12,
    COLORSTR.Green8,
    COLORSTR.Blue11
}
function BattleCtrl:OnSCBattleChat(protocol)
    local content = string.format("<color=#%s>%s：%s</color>",bullet_color_str[GameMath.Ramdon(1, 3)], protocol.name, protocol.content)
    local content_chat = string.format("%s：%s", protocol.name, protocol.content)
    if protocol.comment_type == ChatConfig.CommentType.COMMENT_TYPE_BATTLE then
        local idx = self.data:GetGridIdxByUid(protocol.from_uid)
        BattleCtrl.Instance.displayer:PopHp(idx or 3, BATTLE_HP_DESC.BUBBLE, protocol.content, 5)
    else
        BattleData.Instance.bullet_comments:Insert({text = content})
        ChatCtrl.Instance:ChannelChatCustom({msg_type = ChatConfig.MsgType.bullet, content = content_chat, block_role = true}, ChatConfig.ChannelId.general)
    end
end

function BattleCtrl.BattleReportDetailInfo(monster_group_id, is_server_first_kill)
    if monster_group_id == nil then return end
    if BattleData.Instance:BaseData().server_fighting == true then
        return
    end
    if TeamData.Instance:InTeam() then
        PublicPopupCtrl.Instance:Center(Language.Main.Battle.VideoInTeamTip)
        return
    end
    local proc = GetProtocol(CSBattleReportDetailInfo)
    proc.monster_group_id = monster_group_id
    proc.is_server_first_kill = is_server_first_kill or 0
    SendProtocol(proc)
end
function BattleCtrl.BattleReportSimpleInfo(monster_group_id, is_server_first_kill)
    if monster_group_id == nil then return end
    local proc = GetProtocol(CSBattleReportSimpleInfo)
    proc.monster_group_id = monster_group_id
    proc.is_server_first_kill = is_server_first_kill or 0
    SendProtocol(proc)
end
function BattleCtrl.BattleReportStopWatching()
    local proc = GetProtocol(CSBattleReportStopWatching)
    SendProtocol(proc)
end



function BattleCtrl:RecvResult(param_t, mode)
    if param_t == nil then
        return
    end
    --强制战斗结束不弹出结算界面
    --师徒副本有时间限制，时间到了在战斗中强制失败但是要弹失败界面
    if param_t.result_type == RESULT_TYPE.FORCED_FINISH and BATTLE_MODE.BATTLE_MODE_SHITU_FB ~= mode then
        FightData.Instance:ClearSoulData()
        return
    end
    if BATTLE_MODE.BATTLE_MODE_REALM_BREAK_FIGHT ~= mode then
        FightData.Instance:ClearSoulData()
    end
    if FinishCtrl.IsShowView(mode, param_t.is_win) then
        --这个地方所有失败界面都拦截掉
        FinishCtrl.Instance:Finish(param_t, mode)
    else
        if mode == BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE or mode == BATTLE_MODE.BATTLE_MODE_MATERIAL
        or mode == BATTLE_MODE.BATTLE_MODE_NEW_COURAGE_CHALLENGE or mode == BATTLE_MODE.GOD_SPACE_OCCUPY_ALTAR
        or mode == BATTLE_MODE.DRAGON_VEIN_BOSS or mode == BATTLE_MODE.DRAGON_VEIN_MONSTER then
            FinishData.Instance:SetBattleMode(mode)
            FinishData.Instance:SetFinishInfo(param_t)
        end
        local mg_func = FinishConfig.MonsterGroupFinishFunc[FinishData.Instance:GetMonsterGroupId()]
        if mg_func then
            mg_func(param_t.is_win, param_t, mode)
        elseif FinishConfig.FinishFunc[mode] then
            FinishConfig.FinishFunc[mode](param_t.is_win, param_t, mode)
        end
    end
end
function BattleCtrl:OnBattleReportSimpleInfo(procotol)
    self.video:OnBattleReportSimpleInfo(procotol)
end
function BattleCtrl:OnBattleReport(procotol)
    self.video:OnBattleReport(procotol)
end
function BattleCtrl:OnBattleReportStopWatching(procotol)
    self.video:OnBattleReportStopWatching(procotol)
end
function BattleCtrl:OnBattleAutoInfoList(protocol)
    self.data:SetAutoInfoList(protocol)
end
function BattleCtrl:OnBattleAutoInfoChangeNotice(protocol)
    self.data:SetSingleAutoInfo(protocol)
end
function BattleCtrl:OnBattleRoleInBattleNotice(protocol)
    --判断一下 如果有人物信息了就不发了
    if BattleData.Instance:BaseData().server_fighting == true then
        return
    end
    if BattleData.Instance:BaseData().battle_mode == BATTLE_MODE.BATTLE_MODE_JOYSEEKS then
        return
    end
    TimeHelper:AddDelayTimer(function ()
        self:RoleRejoinBattleScene(protocol.battle_server_idx,protocol.battle_id)
    end,0.5)
end
function BattleCtrl:RoleRejoinBattleScene(battle_server_idx, battle_id)
    local proc = GetProtocol(CSRoleRejoinBattleScene)
    proc.battle_server_idx = battle_server_idx
    proc.battle_id = battle_id
    SendProtocol(proc)
end
function BattleCtrl:OnBattleCharacterSpecialEffectList(protocol)
    self.data:SpecialEffectList(protocol.spe_list)
end
function BattleCtrl:OnBattleCharacterHaloList(protocol)
    self.data:HaloList(protocol.halo_list)
end
function BattleCtrl:OnBattleStatisticResult(protocol)
    self.data:SetBattleStatisticResult(protocol)
end
function BattleCtrl.CSRoleUndo()
    local proc = GetProtocol(CSRoleUndo)
    SendProtocol(proc)
end

function BattleCtrl:SendPokerReq(oper_type, role_uid, index, reason)
    local protocol = GetProtocol(CSPokerReq)
    protocol.oper_type = oper_type
    protocol.role_uid = role_uid
    protocol.index = index
    protocol.reason = reason
	SendProtocol(protocol)
end

function BattleCtrl.CSBattleAnimationSpeedModify(speed_type)
    local protocol = GetProtocol(CSBattleAnimationSpeedModify)
    protocol.speed_type = speed_type or 0
    SendProtocol(protocol)
end
function BattleCtrl:OnBattleAnimationSpeedModify(protocol)
    if protocol.speed_type == 0 then
        PublicPopupCtrl.Instance:Center(Language.Common.BattleSpeed[0])
    elseif protocol.speed_type == 1 then
        PublicPopupCtrl.Instance:Center(Language.Common.BattleSpeed[1])
    end
    self:SetAnimSpeedByType(protocol.speed_type)
end

function BattleCtrl:SCCustomInstructionInfo(protocol)
    self.data:CustomStrategy():Set(protocol.list)
end
function BattleCtrl.CSCustomInstructionAdd(desc)
    local protocol = GetProtocol(CSCustomInstructionAdd)
    protocol.desc = desc
    SendProtocol(protocol)
end
function BattleCtrl.CSCustomInstructionRemove(index)
    local protocol = GetProtocol(CSCustomInstructionRemove)
    protocol.index = index - 1
    SendProtocol(protocol)
end
function BattleCtrl.CSCustomInstructionBatchRemove(num, del_list)
    if num <= 0 then
        return
    end
    local protocol = GetProtocol(CSCustomInstructionBatchRemove)
    protocol.num = num
    protocol.del_list = del_list
    SendProtocol(protocol)
end
function BattleCtrl:CSBattleSetCustomInstruction(index, grid_idx)
    LogDG(index - 1, grid_idx)
    local protocol = GetProtocol(CSBattleSetCustomInstruction)
    protocol.index = index - 1
    protocol.grid_idx = grid_idx
    SendProtocol(protocol)
end

--发送跳过战斗（静态）
function BattleCtrl.SendSkipBattle()
    local protocol = GetProtocol(CSBattleSkipReq)
    SendProtocol(protocol)
end

function BattleCtrl.CSBattleFastForwardReq()
    local protocol = GetProtocol(CSBattleFastForwardReq)
    SendProtocol(protocol)
end
function BattleCtrl:SCBattleFastForwardResp(protocol)
    if protocol.result == 1 then
        local stream = self:GetStream()
        if stream then
            self.data.record_quick_click_time = Time.realtimeSinceStartup
            stream:Interrupt()
            PublicPopupCtrl.Instance:Center(Language.Guide.QuickOK)
        end
    end
end
function BattleCtrl:SCBattleHallowList(protocol, isSimulating)
    self.data:SetSQList(protocol.hallow_list)
    if not isSimulating then
        local HallowList = TableCopy(protocol, 5)
        HallowList._class_type = nil
        HallowList.GetClassType = nil
        HallowList.DeleteMe = nil
        BattleSimulator.Instance.SimulationData.HallowList = HallowList
    end
end
function BattleCtrl:SCBattleGuildBossTotalDamage(protocol)
    GuildData.Instance.guild_trial_total_damage  = protocol.total_damage
    GuildData.Instance.damage_data.flag = not GuildData.Instance.damage_data.flag
end
function BattleCtrl.CSBattleSurrender()
    local protocol = GetProtocol(CSBattleSurrender)
    SendProtocol(protocol)
end
function BattleCtrl:SCBattleMoveBoardcast(protocol)
    if TeamData.Instance:InTeam() and TeamData.Instance:MemberNumRoleReal() > 0 then
        self.data:PushMovePopupData(protocol.info)
    end
end

function BattleCtrl:OnSCBattleGodDragonInfo(protocol, isSimulating)
    self.data:SetDragonList(protocol.dragon_list)
    if not isSimulating then
        BattleSimulator.Instance.SimulationData.DragonList = protocol.dragon_list
    end
end

function BattleCtrl:OnSCBattleShieldGridInfo(protocol, isSimulating)
    self.data.rabbit_shield_list:Set(protocol.rabbit_shield_list)
    if not isSimulating then
        BattleSimulator.Instance.SimulationData.RabbitList = protocol.rabbit_shield_list
    end
end