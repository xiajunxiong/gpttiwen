FILE_REQ("modules/period_activity_act/imperial_exam/imperial_exam_data")

VIEW_REQ("modules/period_activity_act/imperial_exam/imperial_exam_view")
-- 科举考试
ImperialExamCtrl = ImperialExamCtrl or BaseClass(BaseCtrl)

function ImperialExamCtrl:__init()
    if ImperialExamCtrl.Instance ~= nil then
        Debuger.LogError("[ImperialExamCtrl] attempt to create singleton twice!")
        return
    end
    ImperialExamCtrl.Instance = self
    self.data = ImperialExamData.New()
    self.npc_id = self.data:NpcID()
    self.lantern_id = self.data:AnswerNpcID()
    self:RegisterProtocol(CSKeJuExamCSReq)
    self:RegisterProtocol(SCKeJuExamNPCInfo, "OnSCKeJuExamNPCInfo")
    self:RegisterProtocol(SCKeJuExamTimeOutInfo, "OnSCKeJuExamTimeOutInfo")
    self:RegisterProtocol(SCKeJuExamAnswerResultInfo, "OnSCKeJuExamAnswerResultInfo")
    self:RegisterProtocol(SCKeJuExamBaseInfo, "OnSCKeJuExamBaseInfo")
    self:RegisterProtocol(SCKeJuExamPraiseInfo, "OnSCKeJuExamPraiseInfo")
    self:RegisterProtocol(SCKeJuExamNotifyInfo, "OnSCKeJuExamNotifyInfo")
    self:RegisterProtocol(SCKeJuExamUseCardInfo, "OnSCKeJuExamUseCardInfo")
    self:RegisterProtocol(SCKeJuExamLastResultInfo, "OnSCKeJuExamLastResultInfo")
end

function ImperialExamCtrl:OnInit()
    BehaviorData.Instance:CustomNpcTalkBtnsId(self.npc_id, BindTool.Bind(self.SetupBehaviourBtn, self), true)
    ActivityRandData.Instance:CustomClickModHandle(Mod.PeriodActivity.ImperialExam, BindTool.Bind1(self.OnClickActEvent, self))
    PeriodActivityData.Instance:CustomTimesHandle(Mod.PeriodActivity.ImperialExam, function()
        return self.data.info_data.base.is_get_last_reward == 0 and 1 or 0, 1
        -- return self.data:TotalTimes() - self.data.info_data.base.answer_count, self.data:TotalTimes()
    end)
    -- 活动开启监听
    PeriodActivityData.Instance:CustomCreateHandle(Mod.PeriodActivity.ImperialExam, function()
        self:CheckCreateNPC()
        self:CheckTask()
        return true
    end)
    -- 活动结束监听
    PeriodActivityData.Instance:CustomDeleteHandle(Mod.PeriodActivity.ImperialExam, function()
        self:CheckCreateNPC()
        self:CheckTask()
        if not self.data:IsOpen() and ViewMgr:IsOpen(ImperialExamView) then
            ViewMgr:CloseView(ImperialExamView)
            PublicPopupCtrl.Instance:Center(Language.ImperialExam.ActEnd)
        end
        self.data:ResetData()
        self.data.info_data:Notify()
        return true
    end)
    -- 红点
    local remind_func = BindTool.Bind(self.FlushRemind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.PeriodActivity.ImperialExam,
        self.data.info_data,
        remind_func
    )
end

function ImperialExamCtrl:__delete()
    if ImperialExamCtrl.Instance == self then
        ImperialExamCtrl.Instance = nil
    end
end

function ImperialExamCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function ImperialExamCtrl:FlushRemind()
    local num = self.data:Remind()
    return num
end

function ImperialExamCtrl:OnClickActEvent()
    -- 判断是否开启
    if not self.data:IsOpen() then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return
    end
    SceneLogic.Instance:AutoToNpc(self.npc_id, nil, false, true)
end

-- 发送请求
function ImperialExamCtrl:SendReq(req_type, param1)
    local protocol = GetProtocol(CSKeJuExamCSReq)
    protocol.req_type = req_type or ImperialExamReqType.Info
    protocol.param1 = param1 or 0
    if self:IsCanSend(req_type) then
        self:MyLogError("ImperialExamCtrl:SendReq",protocol)
        SendProtocol(protocol)
    end
end

-- 判断是否能发请求
function ImperialExamCtrl:IsCanSend(req_type)
    if req_type == ImperialExamReqType.NPCPos
    or req_type == ImperialExamReqType.StartAnswer
    or req_type == ImperialExamReqType.Fight then
        -- 需要队长才能请求的操作
        return TeamData.Instance:IsLeader()
    else
        if req_type == ImperialExamReqType.Answer
        or req_type == ImperialExamReqType.Help then
            -- 需要指定答题者才能请求的操作
            return RoleId() ~= 0 and RoleId() == self.data.info_data.answer_role_id
        elseif req_type == ImperialExamReqType.Advise then
            -- 需要指定非答题者以外的人才能请求的操作
            return RoleId() ~= 0 and RoleId() ~= self.data.info_data.answer_role_id
        end
    end
    return true
end

-- 判断是否满足条件，满足后开始任务(是否是npc那点进来的)
function ImperialExamCtrl:JudgeStartTask(npc_click)
    -- 判断是否开启
    if not self.data:IsOpen() then
        PublicPopupCtrl.Instance:Center(Language.Activity.NotOpenTip)
        return
    end
    local info_data = self.data.info_data
    -- 判断是否已领取最终奖励
    if info_data.base.is_get_last_reward > 0 then
        PublicPopupCtrl.Instance:Center(Language.ImperialExam.CompleteTip)
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
            PublicPopupCtrl.Instance:Center(Language.ImperialExam.TeamHasExitTip)
            return
        end
        -- 人数够并且队伍类型正确
        if self.data:IsTeamType() and self.data:IsMemberEnought() then
            if npc_click and self.data:IsTimesEnought() then
                self:SendReq(ImperialExamReqType.Fight)
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
function ImperialExamCtrl:CheckTeamType()
    TimeHelper:CancelTimer(self.delay_handler)
    self.delay_handler = Invoke(function()
        if self.data:IsTeamType() then
            PublicPopupCtrl.Instance:Center(Language.ImperialExam.TeamTargetChanged)
            self:JudgeStartTask()
        end
    end, 1)
end

-- 判断是否答题足够
function ImperialExamCtrl:JudgeIsEnoughtAnswer()
    if TeamData.Instance:IsLeader() then
        if self.data:IsTimesEnought() then
            SceneLogic.Instance:AutoToNpc(self.npc_id, nil, false, true)
        else
            -- 有题目就直接去
            if self.data:HasNpcAndQuestion() then
                SceneLogic.Instance:AutoToNpc(self.data:AnswerNpcInfo(), nil, false, true)
            else
                self:SendReq(ImperialExamReqType.NPCPos)
            end
        end
    end
end

function ImperialExamCtrl:OnSCKeJuExamNPCInfo(protocol)
    self:MyLogError("OnSCKeJuExamNPCInfo",ColorStr(protocol.msg_type, COLORSTR.Red8),protocol)
    self.data:SetInfoOther(protocol)
    -- 收到题目后去下一道题
    self:JudgeStartTask()
end

function ImperialExamCtrl:OnSCKeJuExamTimeOutInfo(protocol)
    self:MyLogError("OnSCKeJuExamTimeOutInfo",ColorStr(protocol.msg_type, COLORSTR.Red8),protocol)
    -- 开始答题时手动清理一遍答题结果
    if self.data.info_data.timestamp == 0 and protocol.timestamp > 0 then
        self.data:ResetPraise()
        self.data:ResetUseCard()
        self.data:ResetResult()
    end
    self.data:SetTimeAndUid(protocol.timestamp, protocol.answer_role_id)
    ViewMgr:CloseView(ImperialExamOverView)
    ViewMgr:OpenView(ImperialExamView)
end

function ImperialExamCtrl:OnSCKeJuExamAnswerResultInfo(protocol)
    self:MyLogError("OnSCKeJuExamAnswerResultInfo",ColorStr(protocol.msg_type, COLORSTR.Red8),protocol)
    self.data:SetInfoResult(protocol)
end

function ImperialExamCtrl:OnSCKeJuExamBaseInfo(protocol)
    self:MyLogError("OnSCKeJuExamBaseInfo",ColorStr(protocol.msg_type, COLORSTR.Red8),protocol)
    self.data:SetInfoBase(protocol)
    self:CheckCreateNPC()
    self:CheckTask()
end

function ImperialExamCtrl:OnSCKeJuExamPraiseInfo(protocol)
    self:MyLogError("OnSCKeJuExamPraiseInfo",ColorStr(protocol.msg_type, COLORSTR.Red8),protocol)
    self.data:SetInfoPraise(protocol)
end

function ImperialExamCtrl:OnSCKeJuExamNotifyInfo(protocol)
    self:MyLogError("OnSCKeJuExamNotifyInfo",ColorStr(protocol.msg_type, COLORSTR.Red8),protocol)
    local str = ""
    if Language.ImperialExam.TxtFly[protocol.op_type] then
        if protocol.op_type == MSLanternTxtType.UseCard then
            str = Format(Language.ImperialExam.TxtFly[protocol.op_type], protocol.name, self.data:KardName(protocol.param))
        else
            str = Format(Language.ImperialExam.TxtFly[protocol.op_type], protocol.name)
        end
    end
    if not StringIsEmpty(str) then
        PublicPopupCtrl.Instance:Center(str)
    end
end

function ImperialExamCtrl:OnSCKeJuExamUseCardInfo(protocol)
    self:MyLogError("OnSCKeJuExamUseCardInfo",ColorStr(protocol.msg_type, COLORSTR.Red8),protocol)
    self.data:SetInfoUseCard(protocol)
end

function ImperialExamCtrl:OnSCKeJuExamLastResultInfo(protocol)
    self:MyLogError("OnSCKeJuExamLastResultInfo",ColorStr(protocol.msg_type, COLORSTR.Red8),protocol)
    self.data:SetLastResultInfo(protocol)
end

function ImperialExamCtrl:SetupBehaviourBtn(btn_list, obj_vo)
    if obj_vo and obj_vo.id == self.npc_id and btn_list then
        -- 没开就不给按钮
        if not self.data:IsOpen() then
            return {}
        end
        -- 答完题后需要改变按钮
        if self.data:IsTimesEnought() then
            btn_list[1].dis_str = Language.ImperialExam.LastBtn
            btn_list[1].detail_info = nil
            btn_list[1].exe_func = function()
                self:JudgeStartTask()
            end
        end
        table.insert(btn_list, NpcDialogBehBtnData.FuncCreate(Language.ImperialExam.TeamBtn, BindTool.Bind1(self.JudgeStartTask, self)))
        table.insert(btn_list, NpcDialogBehBtnData.FuncCreate(Language.ImperialExam.HelpBtn, BindTool.Bind1(self.OnHelpBtnBehaviour, self), false))
    end
end

function ImperialExamCtrl:OnHelpBtnBehaviour()
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
function ImperialExamCtrl:CheckCreateNPC()
    self.data:DisposeScene()
    if self.data:SceneId() == SceneData.Instance:SceneId() then
        self.data:InitScene()
    end
end

function ImperialExamCtrl:CheckTask()
    TaskCtrl.Instance.data:TaskInfoDeleteByTaskId(CUSTOM_OBJID.IMPERIAL_EXAM)
    -- 判断是否有接取任务
    if self.data:IsShowTask() then
        local info_data = self.data.info_data
        local task_progress = Format(Language.ImperialExam.AnswerPro[1], info_data.base.answer_count, self.data:TotalTimes())
        -- 如果答题次数足够了
        if self.data:IsTimesEnought() then
            task_progress = Language.ImperialExam.AnswerPro[2]
        end
        local task = TaskInfoImperialExam.New():TaskInfo()
        local task_vo = CommonStruct.CustomTask()
        task_vo.task_id = CUSTOM_OBJID.IMPERIAL_EXAM
        task_vo.task_name = Language.ImperialExam.TaskName
        task_vo.reward_list = ImperialExamData.Instance:Reward()
        task_vo.task_desc = task_progress
        task_vo.task_goal = task_progress
        task:TaskVo(task_vo)
        TaskData.Instance:TaskInfoUpdateByTaskId(CUSTOM_OBJID.IMPERIAL_EXAM, SmartData.New(task), true)

        -- 开始进行任务后，右侧任务栏定位到当前进行中的任务
		local info = TaskData.Instance:GetTaskInfoByTaskType(TaskConfig.TaskType.imperial_exam)
		if info then
			TaskData.Instance:StopTaskState(TaskConfig.TaskType.imperial_exam, CUSTOM_OBJID.IMPERIAL_EXAM)
			MainViewData.Instance:ToggleTTTask()
			TaskData.Instance:MenuToTaskId(info.task_id)
		end
    end
end

function ImperialExamCtrl:MyLogError(...)
    LogError(...)
end