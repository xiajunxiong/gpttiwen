FoxCatchMenuView = FoxCatchMenuView or DeclareMono("FoxCatchMenuView", UIWFlushPanel)

function FoxCatchMenuView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    UH.SetText(self.ClearDesc,CatchFoxData.Instance:GetFoxCatchDesc())
end

GuideFox = GuideFox or DeclareView("GuideFox", "anecdote/guide_fox")
VIEW_DECLARE_MASK(GuideFox,ViewMask.BgBlockClose)

function GuideFox:GuideFox() end