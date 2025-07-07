DailyActGuideTask = DailyActGuideTask or BaseClass(TaskInfo)

function DailyActGuideTask:__init()
    -- self.skin_type = COLORSTR.Green5
    self.block_view = true --true 不显示在任务目标那边
end

--重写任务栏名字
-- function DailyActGuideTask:MenuTitle()
--     return ColorStr(self.task_name, self.skin_type)
-- end

--重写任务描述
-- function DailyActGuideTask:TaskGoal()
--     return self.task_desc
-- end

--点击任务 可重写
function DailyActGuideTask:GoTask()
    -- LogError("GoTask!!!===",self.vo.info.guide_step)
    GuideManager.Instance:Start(self.vo.info.guide_step)

    -- local flag = ActivityData.Instance:GetActiveFlag(24)
    -- local flag2= ActivityData.Instance:GetActiveFlag(8)
    -- LogError("!!FF===",flag,flag2)
end

--是否追踪
function DailyActGuideTask:IsFollow()
    return true
end

-- function DailyActGuideTask:TaskVo(vo)
--     self.guide_step = vo.guide_step
--     TaskInfo.TaskVo(self, vo)
-- end