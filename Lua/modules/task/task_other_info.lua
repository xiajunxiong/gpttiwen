-------------------------TaskFairyWhere-----------------------
TaskFairyWhere = TaskFairyWhere or BaseClass(TaskInfo)

function TaskFairyWhere:__init()
    self.skin_type = COLORSTR.Green5
    self.block_view = true --true 不显示在任务目标那边
end

--重写任务栏名字
function TaskFairyWhere:MenuTitle()
    return ColorStr(self.task_name, self.skin_type)
end

--重写任务描述
function TaskFairyWhere:TaskGoal()
    return FairyWhereData.GetOutdoorName(self.task_desc)
end

--带倒计时的描述
function TaskFairyWhere:TaskStampTime()
    return FairyWhereData.Instance:GetStampTime()
end

--返回计时器描述
function TaskFairyWhere:GetTimeDesc(t,task_goal)
    return task_goal .. Format(Language.FairyWhere.TimeTip,t.hour or 0,t.min or 0,t.s or 0)
end

function TaskFairyWhere:TaskTimeEnd()
    TaskData.Instance:TaskInfoDeleteByTaskId(CUSTOM_OBJID.FAIRY_WHERE)
    FairyWhereCtrl.Instance:SendFairyWhereReq(2)
end

--点击任务 可从写
function TaskFairyWhere:GoTask()
    PublicPopupCtrl.Instance:Center(FairyWhereData.GetOutdoorName(Language.FairyWhere.AutoToNpcTip))
end

--是否追踪
function TaskFairyWhere:IsFollow()
    return true
end