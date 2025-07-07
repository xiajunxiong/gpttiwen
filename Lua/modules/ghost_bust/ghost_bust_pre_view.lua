GhostBustPreView = GhostBustPreView or DeclareView("GhostBustPreView", "ghost_bust/ghost_bust_pre_view")
function GhostBustPreView:GhostBustPreView()

end

function GhostBustPreView:OpenCallback()
    self.EffectTool:Play(7165049)
end

function GhostBustPreView:OnEffectComplete()
    ViewMgr:CloseView(GhostBustPreView)

    GhostBustData.Instance:FlushRolePlay()
end