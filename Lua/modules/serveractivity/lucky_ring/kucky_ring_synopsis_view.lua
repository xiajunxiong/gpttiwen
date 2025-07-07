LuckyRingSynopsisView = LuckyRingSynopsisView or DeclareView("LuckyRingSynopsisView", "serveractivity/lucky_ring/luck_ring_synopsis")
VIEW_DECLARE_LEVEL(LuckyRingSynopsisView,ViewLevel.L0)
function LuckyRingSynopsisView:LuckyRingSynopsisView() end
function LuckyRingSynopsisView:OnClickClose()
    ViewMgr:CloseView(LuckyRingSynopsisView)
end