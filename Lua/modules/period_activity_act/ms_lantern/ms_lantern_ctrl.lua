FILE_REQ("modules/period_activity_act/ms_lantern/ms_lantern_data")

VIEW_REQ("modules/period_activity_act/ms_lantern/ms_lantern_view")
-- 山海灯会
MSLanternCtrl = MSLanternCtrl or BaseClass(BaseCtrl)

function MSLanternCtrl:__init()
    if MSLanternCtrl.Instance ~= nil then
        Debuger.LogError("[MSLanternCtrl] attempt to create singleton twice!")
        return
    end
    MSLanternCtrl.Instance = self
    self.data = MSLanternData.New()
    self.npc_id = self.data:NpcID()
    self.lantern_id = self.data:LanternNpcID()
    self:RegisterProtocol(CSLanternReq)
    self:RegisterProtocol(SCLanternNPCQuestionInfo, "OnSCLanternNPCQuestionInfo")
    self:RegisterProtocol(SCLanternTimeInfo, "OnSCLanternTimeInfo")
    self:RegisterProtocol(SCLanternAnswerResultInfo, "OnSCLanternAnswerResultInfo")
    self:RegisterProtocol(SCLanternBaseInfo, "OnSCLanternBaseInfo")
    self:RegisterProtocol(SCLanternPraiseInfo, "OnSCLanternPraiseInfo")
    self:RegisterProtocol(SCLanternTxtInfo, "OnSCLanternTxtInfo")
    self:RegisterProtocol(SCLanternUseCardInfo, "OnSCLanternUseCardInfo")
    self:RegisterProtocol(SCLanternLastResultInfo, "OnSCLanternLastResultInfo")
end

function MSLanternCtrl:OnInit()
    BehaviorData.Instance:CustomNpcTalkBtnsId(self.npc_id, BindTool.Bind(self.SetupBehaviourBtn, self), true)
    ActivityRandData.Instance:CustomClickModHandle(Mod.PeriodActivity.MSLantern, BindTool.Bind1(self.OnClickActEvent, self))
    PeriodActivityData.Instance:CustomTimesHandle(Mod.PeriodActivity.MSLantern, function()
        return self.data.info_data.base.is_get_last_reward == 0 and 1 or 0, 1
        -- return self.data:TotalTimes() - self.data.info_data.base.answer_count, self.data:TotalTimes()
    end)
    -- 活动开启监听
    PeriodActivityData.Instance:CustomCreateHandle(Mod.PeriodActivity.MSLantern, function()
        self:CheckCreateNPC()
        self:CheckTask()
        return true
    end)
    -- 活动结束监听
    PeriodActivityData.Instance:CustomDeleteHandle(Mod.PeriodActivity.MSLantern, function()
        self:CheckCreateNPC()
        self:CheckTask()
        if not self.data:IsOpen() and ViewMgr:IsOpen(MSLantrenView) then
            ViewMgr:CloseView(MSLantrenView)
            PublicPopupCtrl.Instance:Center(Language.MSLantern.ActEnd)
        end
        self.data:ResetData()
        self.data.info_data:Notify()
        return true
    end)
    -- 红点
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.PeriodActivity.MSLantern,
        self.data.info_data,
        remind_func
    )

    -- 胜利界面
    FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_ML_LANTERN, {
        desc_func = function()
            return Format(Language.MSLantern.FightEnd, self.data.info_data.base.satisfy)
        end,
		reward_func = function()
            return MSLanternData.Instance:GetLastItemList()
        end,
    })
    -- 失败界面
    FinishData.Instance:FailRegister(BATTLE_MODE.BATTLE_MODE_ML_LANTERN, {
        desc_func = function()
            return Format(Language.MSLantern.FightEnd, self.data.info_data.base.satisfy)
        end,
		reward_func = function()
            return MSLanternData.Instance:GetLastItemList()
        end,
    })
    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_ML_LANTERN, function(is_win)
        self.data:SetLastItemList({})
    end)
end

function MSLanternCtrl:__delete()
    if MSLanternCtrl.Instance == self then
        MSLanternCtrl.Instance = nil
    end
end

function MSLanternCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function MSLanternCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

function MSLanternCtrl:OnClickActEvent()
    -- 判断是否开启
    if not self.data:IsOpen() then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return
    end
    SceneLogic.Instance:AutoToNpc(self.npc_id, nil, false, true)
end

-- 发送请求
function MSLanternCtrl:SendReq(req_type, param1)
    local protocol = GetProtocol(CSLanternReq)
    protocol.req_type = req_type or MSLanternReqType.Info
    protocol.param1 = param1 or 0
    if protocol.req_type == MSLanternReqType.Answer
    or protocol.req_type == MSLanternReqType.UseCard
    or protocol.req_type == MSLanternReqType.Praise
    or protocol.req_type == MSLanternReqType.Friend then
        self:MyLogError("MSLanternCtrl:SendReq",protocol)
        SendProtocol(protocol)
    else
        if TeamData.Instance:IsLeader() then
            self:MyLogError("MSLanternCtrl:SendReq",protocol)
            SendProtocol(protocol)
        end
    end
end

-- 判断是否满足条件，满足后开始任务(是否是npc那点进来的)
function MSLanternCtrl:JudgeStartTask(npc_click)
    -- 判断是否开启
    if not self.data:IsOpen() then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return
    end
    local info_data = self.data.info_data
    -- 判断是否已领取最终奖励
    if info_data.base.is_get_last_reward > 0 then
        PublicPopupCtrl.Instance:Center(Language.MSLantern.CompleteTip)
        return
    end
    -- 重新上线后需要重新找一遍npc
    if self.data:IsNeedFind() then
        if TeamData.Instance:IsLeader() then
            SceneLogic.Instance:AutoToNpc(self.npc_id, nil, false, true)
            return
        end
    end
    -- 如果在队伍里
    if TeamData.Instance:InTeam() then
        -- 队伍有人离队
        if TeamData.Instance:IsHasNotFollow() then
            PublicPopupCtrl.Instance:Center(Language.MSLantern.TeamHasExitTip)
            return
        end
        -- 人数够并且队伍类型正确
        if self.data:IsTeamType() and self.data:IsMemberEnought() then
            if npc_click and self.data:IsTimesEnought() then
                self:SendReq(MSLanternReqType.Fight)
                return
            end
            self:JudgeIsEnoughtAnswer()
            return
        end
    end
    -- 否则就去组队匹配
    TeamCtrl.Instance:TeamDetectByTargetId(self.data:TeamType(),
    function ()
        self:JudgeIsEnoughtAnswer()
    end,
    self.data:AtLeastMember(),
    function()
        self:CheckTeamType()
    end)
end

-- 延迟一秒去检测队伍类型
function MSLanternCtrl:CheckTeamType()
    TimeHelper:CancelTimer(self.delay_handler)
    self.delay_handler = Invoke(function()
        if self.data:IsTeamType() then
            PublicPopupCtrl.Instance:Center(Language.MSLantern.TeamTargetChanged)
            self:JudgeStartTask()
        end
    end, 1)
end

-- 判断是否答题足够
function MSLanternCtrl:JudgeIsEnoughtAnswer()
    if self.data:IsTimesEnought() then
        if TeamData.Instance:IsLeader() then
            SceneLogic.Instance:AutoToNpc(self.npc_id, nil, false, true)
        end
    else
        -- 有题目就直接去
        if self.data:HasNpcAndQuestion() then
            self:AutoTask()
        else
            self:SendReq(MSLanternReqType.NPCPos)
        end
    end
end

function MSLanternCtrl:OnSCLanternNPCQuestionInfo(protocol)
    self:MyLogError("OnSCLanternNPCQuestionInfo",protocol)
    self.data:SetInfoOther(protocol)
    -- 收到题目后去下一道题
    self:JudgeStartTask()
end

function MSLanternCtrl:OnSCLanternTimeInfo(protocol)
    self:MyLogError("OnSCLanternTimeInfo",protocol)
    -- 开始答题时手动清理一遍答题结果
    if self.data.info_data.timestamp == 0 and protocol.timestamp > 0 then
        self.data:ResetPraise()
        self.data:ResetUseCard()
        self.data:ResetResult()
    end
    self.data:SetTimestamp(protocol.timestamp)
    ViewMgr:CloseView(MSLantrenOverView)
    ViewMgr:OpenView(MSLantrenView)
end

function MSLanternCtrl:OnSCLanternAnswerResultInfo(protocol)
    self:MyLogError("OnSCLanternAnswerResultInfo",protocol)
    self.data:SetInfoResult(protocol)
end

function MSLanternCtrl:OnSCLanternBaseInfo(protocol)
    self:MyLogError("OnSCLanternBaseInfo",protocol)
    self.data:SetInfoBase(protocol)
    self:CheckCreateNPC()
    self:CheckTask()
end

function MSLanternCtrl:OnSCLanternPraiseInfo(protocol)
    self:MyLogError("OnSCLanternPraiseInfo",protocol)
    self.data:SetInfoPraise(protocol)
end

function MSLanternCtrl:OnSCLanternTxtInfo(protocol)
    self:MyLogError("OnSCLanternTxtInfo",protocol)
    local str = ""
    if Language.MSLantern.TxtFly[protocol.op_type] then
        if protocol.op_type == MSLanternTxtType.UseCard then
            str = Format(Language.MSLantern.TxtFly[protocol.op_type], protocol.name, self.data:KardName(protocol.param))
        else
            str = Format(Language.MSLantern.TxtFly[protocol.op_type], protocol.name)
        end
    end
    if not StringIsEmpty(str) then
        PublicPopupCtrl.Instance:Center(str)
    end
end

function MSLanternCtrl:OnSCLanternUseCardInfo(protocol)
    self:MyLogError("OnSCLanternUseCardInfo",protocol)
    self.data:SetInfoUseCard(protocol)
end

function MSLanternCtrl:OnSCLanternLastResultInfo(protocol)
    self:MyLogError("OnSCLanternLastResultInfo",protocol)
    self.data:SetLastResultInfo(protocol)
end

function MSLanternCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if obj_vo and obj_vo.id == self.npc_id and btn_list then
        -- 没开就不给按钮
        if not self.data:IsOpen() then
            return {}
        end
        -- 答完题后需要改变按钮
        if self.data:IsTimesEnought() then
            btn_list[1].dis_str = Language.MSLantern.LastBtn
            btn_list[1].detail_info = nil
            btn_list[1].exe_func = function()
                self:JudgeStartTask()
            end
        end
        table.insert(btn_list,NpcDialogBehBtnData.FuncCreate(Language.MSLantern.HelpBtn, BindTool.Bind1(self.OnHelpBtnBehaviou, self), false))
    end
end

function MSLanternCtrl:OnHelpBtnBehaviou()
    NpcDialogCtrl.Instance:NpcDialogView({
        talk_id = self.data:GetHelpId(),
        not_block_close_func = true
    },
    function()
        local param_t = NpcDialogData.Instance.cahce_chck_data
        if param_t then
            NpcDialogCtrl.Instance:NpcDialogView(param_t)
        end
    end, true)
end

-- 切场景的时候检查生成npc
function MSLanternCtrl:CheckCreateNPC()
    self.data:DisposeScene()
    if self.data:SceneId() == SceneData.Instance:SceneId() then
        self.data:InitScene()
    end
end

function MSLanternCtrl:CheckTask()
    TaskCtrl.Instance.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.MS_LANTERN)
    -- 判断是否有接取任务
    if self.data:IsShowTask() then
        local info_data = self.data.info_data
        local task_progress = Format(Language.MSLantern.AnswerPro[1], info_data.base.answer_count, self.data:TotalTimes())
        -- 如果答题次数足够了
        if self.data:IsTimesEnought() then
            task_progress = Language.MSLantern.AnswerPro[2]
        end
        local task = TaskInfoMSLantern.New():TaskInfo()
        local task_vo = CommonStruct.CustomTask()
        task_vo.task_id = CUSTOM_OBJID.MS_LANTERN
        task_vo.task_name = Language.MSLantern.TaskName
        task_vo.reward_list = MSLanternData.Instance:Reward()
        task_vo.task_desc = task_progress
        task_vo.task_goal = task_progress
        task:TaskVo(task_vo)
        TaskData.Instance:TaskInfoUpdateByTaskId(CUSTOM_OBJID.MS_LANTERN, SmartData.New(task), true)

        -- 开始进行任务后，右侧任务栏定位到当前进行中的任务
		local info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.ms_lantern)
		if info then
			TaskData.Instance:StopTaskState(TaskConfig.TaskType.ms_lantern, CUSTOM_OBJID.MS_LANTERN)
			MainViewData.Instance:ToggleTTTask()
			TaskData.Instance:MenuToTaskId(info.task_id)
		end
    end
end

function MSLanternCtrl:AutoTask()
    if TeamData.Instance:IsLeader() then
        SceneLogic.Instance:AutoToNpc(self.data:LanternInfo(), nil, false, true)
    end
end

function MSLanternCtrl:MyLogError(...)
    -- LogError(...)
end