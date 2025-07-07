FallenGodOpenView = FallenGodOpenView or DeclareView("FallenGodOpenView", "fallen_god/fallen_god_open")
function FallenGodOpenView:LoadCallback()
end

function FallenGodOpenView:OnClickClose()
    ViewMgr:CloseView(FallenGodOpenView)
end

function FallenGodOpenView:OnClickGo()
    ViewMgr:OpenView(FallenGodView)
    ViewMgr:CloseView(FallenGodOpenView)
end
