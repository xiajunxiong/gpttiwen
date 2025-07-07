
FILE_REQ("modules/guide/guide_data")
FILE_REQ("modules/guide/guide_dragon")
VIEW_REQ("modules/guide/guide_view")
VIEW_REQ("modules/guide/guide_fuben")
VIEW_REQ("modules/guide/guide_treasure")
VIEW_REQ("modules/guide/guide_restraint")
VIEW_REQ("modules/guide/guide_black")
VIEW_REQ("modules/guide/guide_tips")
VIEW_REQ("modules/guide/guide_another")
VIEW_REQ("modules/notice/funopen_notice")
VIEW_REQ("modules/notice/guide_unit")
VIEW_REQ("modules/notice/arena_notice")
VIEW_REQ("modules/notice/cloud_arena_notice")
VIEW_REQ("modules/notice/modrons_notice")
VIEW_REQ("modules/notice/ring_notice")
VIEW_REQ("modules/notice/field_boss_notice")
VIEW_REQ("modules/notice/rush_dungeon_notice")
VIEW_REQ("modules/notice/challenge_test_notice")
FILE_REQ("modules/guide/guide_base")
FILE_REQ("modules/guide/guide_maintask")
FILE_REQ("modules/guide/guide_openview")
FILE_REQ("modules/guide/guide_closeview")
FILE_REQ("modules/guide/guide_clickbutton")
FILE_REQ("modules/guide/guide_canelauto")
FILE_REQ("modules/guide/guide_vieweffect")
FILE_REQ("modules/guide/guide_onlydesc")
FILE_REQ("modules/guide/guide_npc_talk_show")
FILE_REQ("modules/guide/guide_auto_task")
VIEW_REQ("modules/guide/active_guide")
VIEW_REQ("modules/guide/guide_block")

GuideManager = GuideManager or BaseClass(BaseCtrl)
GuideStepType = {
    MainTask = 1,--主线 p1 任务id
    OpenView = 2,--打开功能界面 参数modKey 自动打开
    CloseView = 3,--关闭界面(保留) 自动关闭
    ClickBtn = 4,--点击某个按钮 参数按钮名称（客户端给出并代码做绑定）确定按钮在哪个界面上 哪个panel上
    OnlyDesc = 5,--仅显示美女描述（添加点击事件）
    CanelAuto = 6,--取消自动战斗
    ViewEffect = 7,--自动界面操作,例如特效，选项自动选中等
    AutoTask = 8,--自动任务
    ForceAuto = 9,--强制自动    
    NpcTalk = 10,   --播放npc对话
}
GuideTriggerType = {
    LevelUp = 1, --升到某个等级 p1 等级
    AcceptTask = 2, --接受某个任务 p1 任务id
    FinishTask = 3, --完成某个任务 p1 任务id
    EnterBattle = 4, --进入战斗 p1 怪物组 p2 第几回合
    ClickUi = 5, --点击某UI之后 p1 UI名称(没有写逻辑，不知道怎么写)
    Client = 100,--客户端单独处理指引
}
--说明类界面
GuideExplainView = {
    [SCENE_TYPE.CRYSTAL_FB] = "GuideFuben"
}
function GuideManager:__init()
    if GuideManager.Instance ~= nil then
        Debuger.LogError("[GuildCtrl] attempt to create singleton twice!")
        return
    end
    GuideManager.Instance = self
    -- body
    -- 定义引导类型
    -- 点击主线任务
    self.data = GuideData.New()
    -- self.base = GuideBase.New()
    self.main_task = GuideMainTask.New()
    self.open_view = GuideOpenView.New()
    self.close_view = GuideCloseView.New()
    self.click_btn = GuideClickBtn.New()
    self.canel_auto = GuideCancelAuto.New()
    self.view_effect = GuideViewEffect.New()
    self.only_desc = GuideOnlyDesc.New()
    self.auto_task = GuideAutoTask.New()
    self.force_auto = GuideForceAuto.New()
    self.npc_talk_show = GuideNpcTalkShow.New()
    self.guide_view = nil
    -- 点击指定按钮
    -- 手指特效
    self.guide = nil    --当前在执行的指引
    self.step = 0       --步骤
    self.allStep = 0
    -- self.resolver = nil --解析器

    self.get_guide_ui_callback_list = {}
    self.check_guide_operate_list = {}
    self.only_desc_show_condition = {}
    self.wait_sfend_start_guide = nil    --guide

    self.scene_arrow_enabled = false
    self:RegisterProtocol(SCPetDeadSummonGuideInfo,"SCPetDeadSummonGuideInfo")
    self:RegisterProtocol(CSPetDeadSummonGuideTrigger)
end
function GuideManager:SCPetDeadSummonGuideInfo(protocol)
    self.data.change_pet_guide_trigger_times = protocol.guide_trigger_times
end
function GuideManager.CSPetDeadSummonGuideTrigger()
    local protocol = GetProtocol(CSPetDeadSummonGuideTrigger)
    SendProtocol(protocol)
end

function GuideManager:OnUnloadGameLogic()
    self.wait_sfend_start_guide = nil
    -- self.get_guide_ui_callback_list = {}
    self:DisableArrowGuide()
    self:Finish()
    self:RevertYinYangEye()
    self:DoArrowGuideGuiHuo(false)
end
function GuideManager:OnInit()
    -- self.handle_task = TaskData.Instance:GetTaskList():Care(BindTool.Bind(self.OnTaskListChange, self))
    self.handle_open = EventSys:Bind(GlobalEventKeys.GuideViewOpen, BindTool.Bind(self.OnGuideViewOpen,self))
    self.handle_close = EventSys:Bind(GlobalEventKeys.GuideViewClose, BindTool.Bind(self.OnGuideViewClose,self))
end
function GuideManager:OnTaskAcceptedChange(old)
    if BattleData.Instance:BaseData().server_fighting == true then
        return
    end
    --[[ LogError("接受任务",old)
    LogError(TaskData.Instance:AcceptedTask()) ]]
    local list = TaskData.Instance:AcceptedTask()
    for key, value in pairs(list) do
        local guide = self.data:GuideCfgByTrigger(GuideTriggerType.AcceptTask, key)
        if guide then
            self:Start(guide.id)
            break
        end
    end
end

function GuideManager:pushWaitSFEndGuide(guide)
    if self.wait_sfend_start_guide ~= nil then
        Debuger.LogErrorSD("GuideManager:pushWaitSFFunc ERROR Exist")
    end
    self.wait_sfend_start_guide = guide
end

function GuideManager:popWaitSFEndGuide()
    local re = self.wait_sfend_start_guide
    self.wait_sfend_start_guide = nil
    return re
end

function GuideManager:OnTaskCompletedChange(task_id)
    --[[ local list = TaskData.Instance:FinishTask() ]]

     local guide = self.data:FinishTaskGuide(task_id)
     if guide then
        if BattleData.Instance:BaseData().server_fighting == true then
            -- LogError("Trigger ERROR!!!")
            self:pushWaitSFEndGuide(guide)
            return
        end
        self:Start(guide.id)
    end
    --[[ for key, value in pairs(list) do
        local guide = self.data:GuideCfgByTrigger(GuideTriggerType.FinishTask, key)
        if guide then
            self:Start(guide.id)
            break
        end
    end ]]
end
function GuideManager:__delete()
    EventSys:UnBind(self.handle_open)
end
function GuideManager:OnTaskListChange(old)
    -- LogError(old)
end
function GuideManager:OnLoadedSceneIdChange(old)
    --[[ local config = SceneData.Instance:GetSceneCfg()
    local flag = MapData.Instance:GetFirstEnterInfo(config.scene_type)
    local explain = GuideExplainView[config.scene_type]
    if flag == 0 and explain then
        TimeHelper:AddDelayTimer(function ()
            local view = MonoTypeOf(explain)
            ViewMgr:OpenView(view)
        end, 0.8)
    end ]]
    --[[ if self.data.yinyang_eye_open == true then
        self:RevertYinYangEye()
    end ]]
end
function GuideManager:OnRoleLevelChange(old)
    if old and 0 == old.level then
        return
    end
    if BattleData.Instance:BaseData().server_fighting == true then
        return
    end
    local level = RoleData.Instance:GetRoleLevel()
    local guide = self.data:GuideCfgByTrigger(GuideTriggerType.LevelUp, level)
    if guide then
        self:Start(guide.id)
    end
end

function GuideManager:OnRoundPhaseChange()
    if BattleData.Instance:BaseData().server_fighting == false then
        return
    end
    local base_data = BattleData.Instance:BaseData()
    if self.guide ~= nil then 
        if base_data.round_phase == BATTLE_ROUND_PHASE.Client then
            self:ForceStop()
            return
        end
    end
    if base_data.round_phase == BATTLE_ROUND_PHASE.CommandDone then return end
    if base_data.round_phase == BATTLE_ROUND_PHASE.Server then return end
    self:ForceStop()
    if self.data:IsCanCallBattleGuide() then
        local group_id = base_data.monster_group_id
        local guide = self.data:GuideCfgByTrigger(GuideTriggerType.EnterBattle, group_id)
        if guide then
            if base_data.round_phase == BATTLE_ROUND_PHASE.Command then
                -- LogDG("发送暂停")
                BattleCtrl.Instance:SetPause(1)
            end
            BattleCtrl.Instance:SendAutoBattle(false)
            self:Start(guide.id)
        end
    end
    --临时屏蔽新操作模式中的换宠指引带修复后开放
    if self.guide == nil then--and not RoleSettingData.IsNewMode() then
        self:CheckChangePetGuide()
        return
    end
    --在Command
    --[[ if base_data.round_phase == BATTLE_ROUND_PHASE.Command then
        BattleCtrl.Instance:SetPause(1)
    end ]]
    if base_data.round_phase == BATTLE_ROUND_PHASE.Client then
        if self.guide_view then
            self.guide_view:OnClickFinish()
        end
    end
end
-- 检查是否可做宠物更换指引88
function GuideManager:CheckChangePetGuide()
    local can_change = self.data:IsCanChangePet()
    local guide_times = self.data:ChangePetTimesToday()
    local can_times = self.data:ChangePetCallTimes()
    local had_die = BattleData.Instance:HadPetDie()
    local has_wait = BattleData.Instance:HasPetWait()
    local happened = BattleData.Instance:ChangePetGuide()
    local lv_enough = RoleData.Instance:GetRoleLevel() >= MainViewBattleRB.SimpleUILv    --大于30级才指引换宠指引
    -- LogDG(can_change, guide_times,can_times,had_die,has_wait)
    if can_change and had_die and has_wait and happened == nil and guide_times < can_times and lv_enough then
        if self.data:IsCanCallBattleGuide() then
            if BattleData.Instance:BaseData().auto then
                TimeHelper:AddDelayTimer(function()
                    BattleCtrl.Instance:ForceSetAuto(false)
                    BattleData.Instance:BaseData().auto = false
                end, 0.5)
            end
            self:Start(88)
            GuideManager.CSPetDeadSummonGuideTrigger()
            BattleData.Instance:ChangePetGuide(true)
            if BattleData.Instance:BaseData().round_phase == BATTLE_ROUND_PHASE.Command then
                LogDG("暂停战斗")
                BattleCtrl.Instance:SetPause(1)
            end
        end
    end
end
function GuideManager:sfChangeCheckAndStartGuide()
    if not BattleData.Instance:IsServerFighting() then
        local need_start_guide = self:popWaitSFEndGuide()
        if need_start_guide then
            self:Start(need_start_guide.id)
        end
    end
end

function GuideManager:OnBattleEnd()
    TimeHelper:CancelTimer(self.delay_enable_yinyangeye)
    --一部分战斗不需要关闭阴阳眼效果
    if BattleData.Instance:IsServerFighting() then
        if self.data.yinyang_eye_open then
            local vo = YinYangData.Instance.explore_data.vo
            local id = BattleData.Instance:GetBossGroupId()
            if vo and id == vo.monster_group_id then
            else
                vo = YinYangData.Instance.explore_data.npc_vo
                if vo and id == vo.monster_group_id then
                else
                    GuideManager.Instance:RevertYinYangEye(true)
                end
            end
        end
    else
        if not self:CanEnableYinYangEye() then
            self:sfChangeCheckAndStartGuide()
            return
        end
        self.delay_enable_yinyangeye = TimeHelper:AddDelayTimer(function ()
            if self.data.yinyang_eye_open then
                GuideManager.Instance:EnableYinYangEye(true)
            end
        end,1)
    end
    YinYangData.Instance.explore_data.npc_vo = nil
    self:ForceStop()
    self:sfChangeCheckAndStartGuide()
end
function GuideManager:CanEnableYinYangEye()
    local cfg = SceneData.Instance:GetSceneCfg()
    if cfg and cfg.scene_type == SCENE_TYPE.WEDDING_FB then
        return false
    end
    return true
end
function GuideManager:OnGuideViewOpen()
    local mainline = TaskData.Instance:GetMainlineAccepted()
    local guide = self.data:GuideCfg(1)
    local level = RoleData.Instance:GetRoleLevel()
    if guide and mainline and mainline:State() == TaskConfig.TaskState.done and mainline.task_id == guide.trigger_param_1
    and level == 1 then
        self:Start(1)
    end
    self.handle_scene = SceneMgr:CareLoadedSceneIdChange(BindTool.Bind(self.OnLoadedSceneIdChange,self))
    self.handle_accepted = TaskData.Instance:CareAcceptedTaskChange(BindTool.Bind(self.OnTaskAcceptedChange, self))
    self.handle_completed = TaskData.Instance:ListenTaskCompleteByIds(BindTool.Bind(self.OnTaskCompletedChange, self), self.data.listen_finish_task_guides)
    --TaskData.Instance:CareCompletedTaskChange(BindTool.Bind(self.OnTaskCompletedChange, self))
    self.handle_level = RoleData.Instance:GetBaseData():KeysCare({"level"}, BindTool.Bind(self.OnRoleLevelChange, self))
    self.handle_round_phase = BattleData.Instance:BaseData():Care(BindTool.Bind(self.OnRoundPhaseChange, self), "round_phase")
    self.handle_battle = BattleData.Instance:BaseData():Care(BindTool.Bind(self.OnBattleEnd, self), "server_fighting")
    self.handle_yinyang = TaskData.Instance:ListenTaskCompleteByIds(BindTool.Bind(self.YinYangEyeTask, self), {140, 160})
    -- PartnerData.Instance:CheckGuide()
end
function GuideManager:YinYangEyeTask(task_id)
    if task_id == 140 then
        self:EnableYinYangEye(nil, true)
        self:DoArrowGuideGuiHuo(true, 15046)
    elseif task_id == 160 then
        self:RevertYinYangEye()
        self:DoArrowGuideGuiHuo(false)
    end
end
function GuideManager:OnGuideViewClose()
    SceneMgr:UncareLoadedSceneIdChange(self.handle_scene)
    TaskData.Instance:UnCareAcceptedTaskChange(self.handle_accepted)
    -- TaskData.Instance:UnCareAcceptedTaskChange(self.handle_completed)
    RoleData.Instance:GetBaseData():Uncare(self.handle_level)
    BattleData.Instance:BaseData():Uncare(self.handle_battle)
    BattleData.Instance:BaseData():Uncare(self.handle_round_phase)
    TaskData.Instance:UnlistenTTaskCompleteById(self.handle_yinyang)
    TaskData.Instance:UnlistenTTaskCompleteById(self.handle_completed)
    -- BattleData.Instance:BaseData():Uncare(self.handle_round_change)
end
function GuideManager:SetGuideView(view)
    self.guide_view = view
end
function GuideManager:GuideView()
    return self.guide_view
end
--各个界面引导获取UI的方法
--非永久存在的界面需要再关闭是停止注册
function GuideManager:RegisterGetGuideUi(key, func)
    self.get_guide_ui_callback_list[key] = func
end
function GuideManager:UnRegisterGetGuideUi(key)
    self.get_guide_ui_callback_list[key] = nil
end
function GuideManager:GetGuideUi(key)
    -- 执行注册方法返回ui对象等
    local func = self.get_guide_ui_callback_list[key]
    local ui = nil
    local clickFunc = nil
    if func ~= nil then
        ui, clickFunc = func()
    end
    return ui, clickFunc
end
-- 各个系统自检查方法
function GuideManager:RegisterGuideCheck(guide_id,func)
    self.check_guide_operate_list[guide_id] = func
end
function GuideManager:UnRegisterGuideCheck(guide_id)
    self.check_guide_operate_list[guide_id] = nil
end
function GuideManager:RunGuideCheck(step_cfg)
    -- 执行外部检查判断是否继续指引
    if step_cfg then
        local func = self.check_guide_operate_list[step_cfg.step_id]
        if func then 
            func(step_cfg)
        end 
    end
end
--美女介绍显示条件
function GuideManager:RegisterShowCondition(step, index, func)
    if self.only_desc_show_condition[step] == nil then
        self.only_desc_show_condition[step] = {}
    end
    self.only_desc_show_condition[step][index] = func
end
function GuideManager:GetShowConditon(step, index)
    if self.only_desc_show_condition[step] == nil then
        return nil
    end
    return self.only_desc_show_condition[step][index]
end

function GuideManager:IsGuide()
    return self.guide ~= nil
end
--开始 打开指引 GuideManager.Instance:Start(guideId)
function GuideManager:Start(guideId)
    LogDG("指引id", guideId)
    guideId = self.data:NewModeGuide(guideId)
    if guideId == ConstGuideId.BattleModeOpGuide and not GLOBAL_CONFIG:CanChangeMode() then
        return
    end
    if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
        return
    end
    LogDG("开始指引：",guideId)
    -- 由id获取整个过程
    if self.guide then
        return
    end
    self.guide, self.stepList = self.data:GuideCfg(guideId)
    if self.guide == nil then
        return
    end
    --AutoInvokerCtrl.Instance:StopCommands()
    AutoInvokerCtrl.Instance:ClearCommands()
    --停止主线自动
    TaskCtrl.Instance:CancelAutoMainTimeHandle()
    --指引开始关闭快捷聊天界面
    SocietyData.Instance.quick_chat_info.cur_sel = ""
    if MainPlayer then
        MainPlayer:StopMove()
    end
    -- ViewMgr:OpenView(GuideView)
    if self.guide_view then
        self.guide_view.canvas.enabled = true
    end
    -- TODO 某些情况下不执行
    if self.guide ~= nil and self.stepList ~= nil then

        ViewMgr:CloseViewInGuide(guideId)
        
        self.allStep = table.nums(self.stepList)
        self.step = 0
        TaskData.Instance.menu_guide.is_show = false
        --添加延迟逻辑
        if not StringIsEmpty(self.guide.delay_time) and self.guide.delay_time > 0 then
            TimeHelper:AddDelayTimer(function ()
                self:Continue()
            end, self.guide.delay_time)
        else
            self:Continue()
        end
    end
end
--继续
function GuideManager:Continue()
    if self.guide == nil then
        return
    end
    if ViewMgr:IsOpen(GuideRestraint) then
        return
    end
    --TODO 某一步不执行
    if self.step == self.allStep then
        self:Finish()
    else
        self.step = self.step + 1
        self:Check(self.stepList[self.step])
        if self.guide == nil then
            return
        end
        -- 境界特殊延迟处理
        if self.stepList[self.step].step_id == 76 and self.stepList[self.step].index == 3 then
            TimeHelper:AddDelayTimer(function ()
                self:Execute(self.stepList[self.step])
            end, 3.5)
        elseif not StringIsEmpty(self.stepList[self.step].delay_time) and self.stepList[self.step].delay_time > 0 then
            TimeHelper:AddDelayTimer(function ()
                self:Execute(self.stepList[self.step])
            end, self.stepList[self.step].delay_time)
        else 
            self:Execute(self.stepList[self.step])
        end
    end
end
--执行
function GuideManager:Execute(step_cfg)
    if self.guide == nil then
        return
    end
    local step_type = step_cfg.step_type
    --特殊处理
    if step_cfg.step_id == 5 and step_cfg.index == 5 then
        local pet_alive = BattleData.Instance:IsChAlive(BattleData.Instance:PlayerPetIdx())
        if pet_alive == false then
            step_cfg = self.stepList[self.step - 1]
            self.step = self.allStep
        end
    end
    if self.guide_view then
        self:GuideView():SetStepCfg(step_cfg)
    end
    if step_type == GuideStepType.MainTask then
        self.main_task:Start(step_cfg, function() self:Continue() end)
    elseif step_type == GuideStepType.OpenView then
        TimeHelper:AddDelayFrameTimer(function ()
            self.open_view:Start(step_cfg, function() self:Continue() end)
        end)
    elseif step_type == GuideStepType.CloseView then
        self.close_view:Start(step_cfg, function() self:Continue() end)
    elseif step_type == GuideStepType.ClickBtn then
        self.click_btn:Start(step_cfg, function() self:Continue() end)
    elseif step_type == GuideStepType.CanelAuto then
        self.canel_auto:Start(step_cfg, function() self:Continue() end)
    elseif step_type == GuideStepType.ViewEffect then
        self.view_effect:Start(step_cfg, function() self:Continue() end)
    elseif step_type == GuideStepType.OnlyDesc then
        self.only_desc:Start(step_cfg, function() self:Continue() end)
    elseif step_type == GuideStepType.AutoTask then
        self.auto_task:Start(step_cfg, function() self:Continue() end)
    elseif step_type == GuideStepType.ForceAuto then
        self.force_auto:Start(step_cfg, function() self:Continue() end)
    elseif step_type == GuideStepType.NpcTalk then
        self.npc_talk_show:Start(step_cfg,function () self:Continue() end)
    else
        LogError("未知步骤类型", step_type)
        self:ForceStop()
    end
end
--完成指引
function GuideManager:Finish()
    TaskCtrl.Instance:TryBeginAutoTaskImmediately()
    -- 判断当前战斗类型
    if self.guide then
        local finish_func = GuideData.FinishFunc[self.guide.id]
        if finish_func ~= nil then
            finish_func()
        end
        if self.guide.trigger_type == GuideTriggerType.EnterBattle
        or (self.guide.id == 88 or self.guide.id == 167) then
            BattleCtrl.Instance:SetPause(0)
        end
    end
    
    self:ForceStop()
    self.step = 0
    self.guide = nil
    self:Continue()
    ViewMgr:CloseView(GuideRestraint)
    TaskData.Instance.menu_guide.is_show = true
    --ViewMgr:CloseView(GuideView)
    
    if self.guide_view then 
        self.guide_view.canvas.enabled = false
        self.guide_view:CancelStepTimer() 
    end 
end

--停止指引
function GuideManager:Stop()
    if self.guide_view then
        self.guide_view:OnClickFinish()
    end
end
--强制停止
function GuideManager:ForceStop()
    -- 判断当前战斗类型
    if self.guide then
        if self.guide.trigger_type == GuideTriggerType.EnterBattle then
            BattleCtrl.Instance:SetPause(0)
        end
    end
    if self.guide_view then
        self.guide_view.canvas.enabled = false
        self.guide_view:CancelStepTimer()
        self.guide_view:Hide()
    end
    self.click_btn:Finish()
    self.step = 0
    self.guide = nil
    self:Continue()
    ViewMgr:CloseView(GuideRestraint)
end
--检查
function GuideManager:Check(step_cfg)
    self:RunGuideCheck(step_cfg)
    if step_cfg.step_id == 114 then
        local flag = GuideData.Instance.guide_flag[FuncGuideFlagType.FUNC_GUIDE_DREAM_NOTES_CHALLENGE]
        if flag == 1 then
            self:ForceStop()
        end
    end
    if step_cfg.step_id == 89 then
        local flag = GuideData.Instance.guide_flag[FuncGuideFlagType.FUNC_GUIDE_PASS_LUN_CHUAN]
        if flag == 1 then
            self:ForceStop()
        end
    end
    if step_cfg.step_id == 48 and step_cfg.index == 1 then
        if not TeamCtrl.Instance.data:InTeam() then
            local co = Cfg.TeamTypeTargetAct(ActType.Conspiracy)
            TeamData.Instance.cur_platform_target_id = co.target_id
            ViewMgr:OpenView(TeamPlatformView)
        else
            ViewMgr:OpenView(TeamView)
            self:ForceStop()
        end
    end

    if step_cfg.step_id == 37 and step_cfg.index == 1 then
        local item_id = Config.function_guide_auto.pet_skill[1].equipment_id
        local index = BagData.Instance:GetBagIndexById(ItemColumnType.Item,item_id)
        if index == nil then
            LogError("不满足指引37触发条件")
            self:ForceStop()
        end
    end
    --对某些条件进行判断成立就中断
    if step_cfg.step_id == 10 and step_cfg.index == 1 then
        if SkillData.Instance:CacuRemindCount() == 0 or SkillData.Instance:CheckSkillLearn() then
            self:ForceStop()
        end
    end
    --[[ if step_cfg.step_id == 41 and step_cfg.index == 1 then
        if Item.Create({item_id = CommonItemId.JiuZhuanHuiHun}):Num() == 0 then
            self:ForceStop()
        end
    end ]]
    if (step_cfg.step_id == 2 or step_cfg.step_id == 58) and step_cfg.index == 1 then
        if ViewMgr:IsOpen(NpcDialogView) then
            ViewMgr:CloseView(NpcDialogView)
        end
    --判断是否有可加点
        local count = RoleAddPointData.Instance:GetRedAddPiontNum()
        if count == 0 or RoleAddPointData.Instance:GetResetPlanTimes() ~= 0 then
            self:ForceStop()
        end
    end

    if step_cfg.step_id == 25 then
        if step_cfg.index == 1 and self.data:CheckXiLianGuide() then
            self:ForceStop()
        elseif step_cfg.index == 4 and ViewMgr:IsOpen(PetGain) then
            self:ForceStop()
        end
    end
    if step_cfg.step_id == 5 then
        if BagData.Instance:GetEquipList(2) == nil then
            LogError("不满足指引条件没有装备武器")
            self:ForceStop()
        end
        if PetData.Instance:GetFightIndex() == -1 then
            LogError("不满足指引条件没有宠物")
            self:ForceStop()
        end
    end

    if step_cfg.step_id == 52 and step_cfg.index == 1 then
        local item_id = self.data:GetPetPieceId()
        local index = BagData.Instance:GetBagIndexById(ItemColumnType.Item, item_id)
        if index == nil then
            self:ForceStop()
        end
    end

    if step_cfg.step_id == 54 then
        if step_cfg.index == 1 then
            SocietyData.Instance.guide_group = true
            SocietyData.Instance.guide_group_create = SocietyConfig.ChatQuickCreateState.state0
        elseif step_cfg.index == 2 then
            local group_count = SocietyData.Instance:GetChatGroupCountAll()
            if group_count > 0 then
                self:ForceStop()
                SocietyData.Instance.guide_group_create = SocietyConfig.ChatQuickCreateState.state2
                GuideManager.Instance:Start(55)
            else
                SocietyData.Instance.guide_group_create = SocietyConfig.ChatQuickCreateState.state1
            end
        end
    end
    if step_cfg.step_id == 55 then
        if step_cfg.index == 1 or step_cfg.index == 2 then
            if SocietyConfig.ChatQuickCreateState.state0 == SocietyData.Instance.guide_group_create then
                self:Continue()
            end
        end
    end
    if step_cfg.step_id == 16 and step_cfg.index == 1 then
        if TaskData.Instance.is_finish_bounty_task == true then
            ViewMgr:OpenViewByKey(Mod.Activity.Daily)
            self:ForceStop()
        end
        -- MainViewData.Instance:ShowRbButton()
    end
    if step_cfg.step_id == 106 then
        if step_cfg.index == 3 then
            if not RolePromoteData.Instance:IsGetPromoteTask(3) then
                self:ForceStop()
            end
        end
    end
    if step_cfg.step_id == 60 or step_cfg.step_id == 61 or step_cfg.step_id == 62 then
        if step_cfg.index == 1 then
            if ActivityData.Instance:GetActiveFlag(17) ~= 0 then
                self:ForceStop()
            end
        end
    end
    if step_cfg.step_id == 63 then
        local partner = PartnerData.Instance:GetPartnerInfoById(4)
        if partner then
            local is_unlock = PartnerConfig.LockState.unlock == partner.vo.is_unlocked
            if is_unlock then
                self:ForceStop()
            end
        end
    end
    if step_cfg.step_id == 64 then
        local partner = PartnerData.Instance:GetPartnerInfoById(6)
        if partner then
            local is_unlock = PartnerConfig.LockState.unlock == partner.vo.is_unlocked
            if is_unlock then
                self:ForceStop()
            end
        end
    end
    if step_cfg.step_id == 70 then
        if TeamData.Instance:InTeam() then
            PublicPopupCtrl.Instance:Center(Language.Fight.Seal.TeamTip)
            self:ForceStop()
        end
    end
    if step_cfg.step_id == 74 and step_cfg.index == 1 then
        if ViewMgr:IsOpen(KeyUseView) then
            MainOtherCtrl.Instance:CloseKeyUse()
        end
        local num = Item.GetNum(self.data:TreasureItem())
        if num < 1 then
            self:ForceStop()
        end
    end
    if step_cfg.step_id == 82 and step_cfg.index == 3 then
        local index = 1
        local lock = FabaoLotteryData.Instance:IsLock(index)
        if lock then
            self:ForceStop()
        end
        local info = FabaoLotteryData.Instance:GetDrawInfo(index)
        if info and info.material_num > 0 then
            self:ForceStop()
        end
    end
    if step_cfg.step_id == 69 or step_cfg.step_id == 53 then
        if step_cfg.index == 2 then
            FightFBViewYMZ.not_flush_sel = true
        end
    end
    if step_cfg.step_id == 87 and step_cfg.index == 1 then
        if UnityPlayerPrefs.GetInt(PrefKeys.SecondSkillGuide()) == 1 or UnityPlayerPrefs.GetInt(PrefKeys.SecondSkillGuide()) == nil then
            self:ForceStop()
        else
            UnityPlayerPrefs.SetInt(PrefKeys.SecondSkillGuide(), 1)
        end
    end

    if step_cfg.step_id == 149 then
        if not BattleData.Instance:HadPetDie() then
            self:ForceStop()
        end
    end

    -- 境界指引
    if step_cfg.step_id == 95 then 
        if step_cfg.index == 1 then
            SceneLogic.Instance:ClearMoveCache()
            BehaviorData.Instance.auto_behavior = 0
        -- elseif step_cfg.index == 3 then
        --     local task = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.bounty)
        --     if task then
        --         TaskData.Instance.menu_guide.pre_val = task.task_id
        --         TaskData.Instance.menu_guide.val = task.task_id
        --         task.flush_show = not task.flush_show
        --     end
        end
    end
    if step_cfg.step_id == 99 and step_cfg.index == 1 then
        if SkillData.Instance:GetSpecialGuideSkillCanNotUp() then
            self:ForceStop()
        end
    end
    if step_cfg.step_id == 126 and step_cfg.index == 1 then
        local force_stop = true
        if not TeamData.Instance:InTeam()  then
            local partner = PartnerData.Instance:GetPartnerInfoById(20)
            force_stop = PartnerConfig.FightState.fight == partner.vo.fight_state 
        end
        if force_stop then
            self:ForceStop()
        end
    end
    if step_cfg.step_id == ConstGuideId.GaiMing and step_cfg.index == 1 then
        local list = PetData.Instance:GetPetList()
        if table.nums(list) == 0 then
            self:ForceStop()
        end
    end
    -- if step_cfg.step_id == 60 then
    --     if step_cfg.index == 2 then
    --         self:Continue()
    --     end
    --     if step_cfg.index == 3 then
    --         DreamNotesData.Instance.guide_index = 1
    --         self:Continue()
    --     end
    -- end
    -- if step_cfg.step_id == 61 then
    --     if step_cfg.index == 2 then
    --         self:Continue()
    --     end
    --     if step_cfg.index == 3 then
    --         DreamNotesData.Instance.guide_index = 2
    --         self:Continue()
    --     end
    -- end
    -- if step_cfg.step_id == 106 then
    --     if step_cfg.index == 4 then
    --         DreamNotesData.Instance.guide_index = 3
    --         self:Continue()
    --     end
    -- end
end

function GuideManager:Guide()
    return self.guide
end
function GuideManager:Step()
    return self.step
end
function GuideManager:StepCfg()
    if self.stepList then
        return self.stepList[self.step]
    end
end
function GuideManager:EnableArrowGuide(targetPos)
    Game.GameManager.Singleton().TargetArrow:BeginShow(MainPlayer.root_node.transform,targetPos)
    self.scene_arrow_enabled = true
end

function GuideManager:DisableArrowGuide()
    if self.scene_arrow_enabled == true then
        self.scene_arrow_enabled = false
        Game.GameManager.Singleton().TargetArrow:Stop()
    end
end

function GuideManager:GetCurStepCLickSame(click_btn)
    if self.stepList == nil then return false end 
    return self.stepList[self.step].step_param_1 == click_btn
end
function GuideManager:LogGuideMsg()
    if self:IsGuide() then
        LogDG(string.format("指引id = %s, 第 %s 步，指引名称 %s",self.stepList[self.step].step_id, self.stepList[self.step].index, self:Guide().guide_name))
    else
        LogDG("没有在指引中！！！！！！！！！！！！！！！！！！！！！！！")
    end
end

function GuideManager:EnableYinYangEye(is_battle, mission)
    if is_battle and not self.data.yinyang_eye_open then
        return
    end
    if self.data.yinyang_eye_effect.id > 0 then
        return
    end
    self.data.yinyang_eye_effect.id = 30000999
    if not is_battle then
        self.data.yinyang_eye_effect.id2 = 30001000
    end
    self.data.yinyang_eye_open = true
    SceneCtrl.Instance:Objs():EnableYinYangEye()

    --[[ local vo = VoMgr:NpcVoBySeq(15046)
    if vo and vo.scene_id == SceneData.Instance:SceneId() and
    not BattleData.Instance:IsServerFighting() and mission then
        TargetArrow.GuiHuo()
        GuideManager.Instance:DisableArrowGuide()
        local y = SceneCtrl.Instance:CurScene():GetPosHeight(vo.x,vo.y)
        GuideManager.Instance:EnableArrowGuide(Vector3.New(vo.x,y,vo.y))
    end ]]
end 
function GuideManager:RevertYinYangEye(is_battle)
    if not is_battle then
        self.data.yinyang_eye_open = false
    end
    self.data.yinyang_eye_effect.id = 0
    if not is_battle then
        self.data.yinyang_eye_effect.id2 = 0
    end
    SceneCtrl.Instance:Objs():RevertYinYangEye()
    GuideManager.Instance:DisableArrowGuide()
    --[[ TargetArrow.Revert() ]]
end
function GuideManager:DoArrowGuideGuiHuo(opened, npc_id)
    --15046
    if self.data.guihuo_arrow_open and not opened then
        GuideManager.Instance:DisableArrowGuide()
        TargetArrow.Revert()
        self.data.guihuo_arrow_open = false
        return
    end
    local vo = VoMgr:NpcVoBySeq(npc_id)
    if vo and vo.scene_id == SceneData.Instance:SceneId() and
    not BattleData.Instance:IsServerFighting() then
        TargetArrow.GuiHuo()
        GuideManager.Instance:DisableArrowGuide()
        local y = SceneCtrl.Instance:CurScene():GetPosHeight(vo.x,vo.y)
        GuideManager.Instance:EnableArrowGuide(Vector3.New(vo.x,y,vo.y))
        self.data.guihuo_arrow_open = true
    end
end
function GuideManager:ShowYinYangEye(show, check)
    if show == true and not check then
        self:OnBattleEnd()
        return
    end
    if self.data.yinyang_eye_open then
        if not show then
            GuideManager.Instance:RevertYinYangEye(true)
        else
            GuideManager.Instance:EnableYinYangEye(true)
        end
    end
end

function GuideManager:CheckCanOpenKeyUse(param)
    if self:IsGuide() then
        if self.guide.id == 74 and param and param.item_id then
            if param.item_id ~= self.data:TreasureItem() then
                return false
            end
        end
    end
    
    return true
end