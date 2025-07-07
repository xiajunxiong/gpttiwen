
BountyTaskPracticeView = BountyTaskPracticeView or DeclareView("BountyTaskPracticeView", TaskConfig.ResPathBT .. "bounty_task_practice", Mod.BountyTask.Practice)
-- VIEW_DECLARE_LEVEL(BountyTaskPracticeView, ViewLevel.L0)
VIEW_DECLARE_MASK(BountyTaskPracticeView, ViewMask.BgBlock)
BountyTaskPracticeViewF = BountyTaskPracticeViewF or DeclareMono("BountyTaskPracticeViewF", UIWFlushPanel)

function BountyTaskPracticeViewF:BountyTaskPracticeViewF()
    self.PracticeTime = 5
    self.is_complete = false

    self.WAIT_TIME = 3
end

function BountyTaskPracticeViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushProgress()
end

function BountyTaskPracticeViewF:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.practice_handle ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.practice_handle)
        self.practice_handle = nil
    end
    TimeHelper:CancelTimer(self.timer_dt)
end

function BountyTaskPracticeViewF:OnClickBlock()
    ViewMgr:CloseView(BountyTaskPracticeView)

    if self.is_complete then
        TaskCtrl.Instance:SendTaskGameReq(TaskConfig.TaskGameList.Practice)
    end
end

function BountyTaskPracticeViewF:FlushProgress()
    TimeHelper:CancelTimer(self.timer_tt)
    local start_time = TimeHelper.GetServerTime()
    self:ShowComplete(false)

    self.practice_handle = Runner.Instance:RunUntilTrue(function ()
        local server_time = TimeHelper.GetServerTime()
        if server_time - start_time > self.PracticeTime then
            self.ProgressShow:SetProgress(1)
            self:ShowComplete(true)
            return true
        end
        self.ProgressShow:SetProgress((server_time - start_time) / self.PracticeTime)
    end)
end


function BountyTaskPracticeViewF:ShowComplete(is_complete)
    self.is_complete = is_complete
    self.IngObj:SetActive(not is_complete)
    self.CompleteObj:SetActive(is_complete)

    if is_complete then
        TimeHelper:CancelTimer(self.timer_dt)
        self.timer_dt = TimeHelper:AddDelayTimer(BindTool.Bind(self.OnClickBlock, self), self.WAIT_TIME)
    end
end

