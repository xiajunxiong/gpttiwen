
BountyTaskLetterView = BountyTaskLetterView or DeclareView("BountyTaskLetterView", TaskConfig.ResPathBT .. "bounty_task_letter", Mod.BountyTask.Letter)
-- VIEW_DECLARE_LEVEL(BountyTaskArrayView, ViewLevel.L0)
VIEW_DECLARE_MASK(BountyTaskLetterView, ViewMask.Block)
function BountyTaskLetterView:BountyTaskLetterView()
end

function BountyTaskLetterView:OnClickClose()
    ViewMgr:CloseView(BountyTaskLetterView)
end

BountyTaskLetterViewF = BountyTaskLetterViewF or DeclareMono("BountyTaskLetterViewF", UIWFlushPanel)

function BountyTaskLetterViewF:BountyTaskLetterViewF()
    self.langauge = Language.Task.BountyTaskLetter
end

function BountyTaskLetterViewF:Awake()
    UIWFlushPanel.Awake(self)


    self:FlushShow()
end

function BountyTaskLetterViewF:FlushShow()
    self.ToShow.text = self.langauge.ToShow
    self.FromShow.text = self.langauge.FromShow
    self.PageText:SetData({
        chapters = {self.langauge.ContentShow}
    })
end

function BountyTaskLetterViewF:OnClickGo()
    -- TaskData.Instance:GoTaskByTaskType(TaskConfig.TaskType.bounty)
    ViewMgr:CloseView(BountyTaskLetterView)
    GuideManager.Instance:Start(16)
end