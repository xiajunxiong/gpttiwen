FILE_REQ("modules/fight_den/fight_den_data")
VIEW_REQ("modules/fight_den/fight_den_finish")
VIEW_REQ("modules/fight_den/fight_den_reward")

--魔窟 万妖榜
FightDenCtrl = FightDenCtrl or BaseClass(BaseCtrl)
function FightDenCtrl:__init()
    if FightDenCtrl.Instance ~= nil then
        Debuger.LogError("[FightDenCtrl] attempt to create singleton twice!")
        return
    end
    FightDenCtrl.Instance = self
    self.data = FightDenData.New()
    self:RegisterProtocol(CSMonsterWaveReq)
    self:RegisterProtocol(SCMonsterWaveAchievementInfo, "OnSCMonsterWaveAchievementInfo")
    self:RegisterProtocol(SCMonsterWaveInfo, "RecvMonsterWaveInfo", true)
    self:RegisterProtocol(SCMonsterWaveFinishInfo, "RecvMonsterWaveFinishInfo", true)
    self:RegisterProtocol(SCBattleDemonCaveWave, "RecvBattleDemonCaveWave", true)
    -- BehaviorData.Instance:CustomNpcTalkBtnsSeq(36, BindTool.Bind(self.SetupBehaviourBtn, self), true)
    ActivityData.Instance:CustomClickHandle(ActType.FightDen, BindTool.Bind(self.GoToFightDen, self))
end
function FightDenCtrl:GoToFightDen()
    local act = ActivityData.Instance:GetActivityInfo(ActType.FightDen)
    if act and act.info and act.info.fun_name then
        BehaviorCtrl.ExeByCfgVo(BehaviorCtrl.GetCfg(act.info.fun_name, true))
    end
    TaskData.Instance:StopTaskState(TaskConfig.TaskType.dragon, CUSTOM_OBJID.FightDen)
end
function FightDenCtrl:__delete()
    self:UnRegisterProtocol(SCMonsterWaveInfo)
    self:UnRegisterProtocol(SCMonsterWaveFinishInfo)
    self:UnRegisterProtocol(SCBattleDemonCaveWave)
end

function FightDenCtrl:RecvMonsterWaveInfo(protocol)
    self.play_times = protocol.play_times
    self.data:SetPassWave(protocol.today_pass_wave)
    self.data:SetRewardFlag(protocol.reward_flag)
    local temp_max_wave = math.max(protocol.today_pass_wave[0], protocol.today_pass_wave[1])
    ActivityData.Instance:SetActivityFinishInfo(ActType.FightDen,temp_max_wave, self.data:GetMaxWave())
    if nil ~= self.dargon_task_role_level_care then
        RoleData.Instance.base_data:Uncare(self.dargon_task_role_level_care)
    end
    if temp_max_wave == self.data:GetMaxWave() then
        TaskCtrl.Instance.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.FightDen)
    else
        local act_info = ActivityData.Instance:GetActivityInfo(ActType.FightDen)
        local task = TaskInfoDragon.New():TaskInfo()
        local task_vo = CommonStruct.CustomTask()
        task_vo.task_id = CUSTOM_OBJID.FightDen
        task_vo.task_name = Language.Task.DragNames[CUSTOM_OBJID.FightDen]
        task_vo.reward_list = act_info.reward_list
        local desc_show = string.format("%s(%s/%s)", Language.Task.DragDescs[CUSTOM_OBJID.FightDen], temp_max_wave, self.data:GetMaxWave())
        task_vo.task_desc = desc_show
        task_vo.task_goal = desc_show
        task:TaskVo(task_vo)
        self.dargon_task_role_level_care = task:DragonTaskParam(function ()
            if TeamData.Instance:InTeam() then
                local info = TeamData.Instance:GetTeamInfo()
                if  info.info.team_type == 10 and TeamData.Instance:MemberNumRole() >= 3 then
                    if TeamCtrl.Instance.guide_dragon == nil then
                        TeamCtrl.Instance.guide_dragon = GuideDragon.New()
                    end
                    TeamCtrl.Instance.guide_dragon:Start()
                else
                    TeamCtrl.Instance:TeamDetectByActType(ActType.FightDen,function ()
                    end, nil, 5)
                    if TeamCtrl.Instance.guide_dragon == nil then
                        TeamCtrl.Instance.guide_dragon = GuideDragon.New()
                    end
                    -- TeamCtrl.Instance.guide_dragon:Start()
                    TimeHelper:CancelTimer(self.handle_delay)
                    self.handle_delay = TimeHelper:AddDelayTimer(function ()
                        TeamCtrl.Instance.guide_dragon:Start()
                    end, 0.2)
                end
            else
                TeamCtrl.Instance:TeamDetectByActType(ActType.FightDen,function ()
                end, nil, 5)
            end
        end, act_info:LevelRestrict())
        TaskData.Instance:TaskInfoUpdateByTaskId(CUSTOM_OBJID.FightDen, SmartData.New(task), true)
    end
    -- TaskData.Instance:DragonTaskTrace()
end

function FightDenCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if obj_vo and obj_vo.seq == 36 then
        btn_list = {}
        local button = NpcDialogBehBtnData.FuncCreate(
            NpcBehaviorBtn[2],
            function ()
                local flag = ActivityData.Instance:IsNoEnoughLevel(ActType.FightDen)
                if flag then
                    return
                end
                --设置当前战斗等级
                local level = RoleData.Instance:GetRoleLevel()
                local result = 0
                if self.data.reward_list == nil then
                    self.data:CheckCaveReward()
                end
                for k, v in pairs(self.data.reward_list[0]) do
                    if k <= level then
                        result = math.max(k, result)
                    end
                end
                self.data:SetLevel(result)
                -- 进魔窟
                ViewMgr:OpenView(FightDenReward2)
                --[[ local param_t = {}
                param_t.battle_mode = BATTLE_MODE.BATTLE_MODE_WAVE
                BattleCtrl.BattleModeFightReq(param_t) ]]
            end)
        button:SetIconType(NpcIconType.Act)
	    table.insert(btn_list, button)--NpcDialogBehBtnData.BehIdCreate(NpcBehaviorBtn[2], 824, obj_vo))
    end
    return btn_list
end

function FightDenCtrl:SendBattle()
    self.data:SetWave(1)
    BattleCtrl.BattleModeFightReq{battle_mode = BATTLE_MODE.BATTLE_MODE_WAVE}
end

function FightDenCtrl:RecvMonsterWaveFinishInfo(protocol)
    self.data:SetLevel(protocol.role_level)
    -- self.data:SetWave(protocol.pass_wave)
    self.data:SetPassWave(protocol.pass_wave, protocol.diff_type)
    if protocol.pass_wave == self.data:GetMaxWave() then
        if not TableIsEmpty(self.data.finish_reward) then
            ViewMgr:OpenView(FightDenFinish)
        end
    else
        if protocol.pass_wave == 0 then
            FinishCtrl.Instance:Finish({is_win = 0},BATTLE_MODE.BATTLE_MODE_WAVE)
        elseif not TableIsEmpty(self.data.finish_reward) then
            ViewMgr:OpenView(FightDenFinish)
        end
    end
end

function FightDenCtrl:RecvBattleDemonCaveWave(protocol)
    self.data:SetWave(protocol.cur_wave)
end
--req_type 0:申请成就信息
--req_type 1:申请领取奖励 param1:奖励index
function FightDenCtrl:SendAchievementReq(req_type, param1)
    local protocol = GetProtocol(CSMonsterWaveReq)
    protocol.req_type = req_type or 0
    protocol.param1 = param1 or 0
    SendProtocol(protocol)
end

function FightDenCtrl:SendGetAchievenmentReward(reward_index)
    self:SendAchievementReq(1,reward_index)
end

function FightDenCtrl:OnSCMonsterWaveAchievementInfo(protocol)
    self.data:SetAchievementInfo(protocol)
end