GuideBlock = GuideBlock or DeclareView("GuideBlock", "guide/guide_block", nil, ViewLayer.Top + 1)
VIEW_DECLARE_LEVEL(GuideBlock, ViewLevel.Lt)
VIEW_DECLARE_MASK(GuideBlock, ViewMask.None)
function GuideBlock:GuideBlock()
    
end
function GuideBlock:LoadCallback()
    self.click_num = 0
end
function GuideBlock:OnClick()
    -- ViewMgr:CloseView(GuideBlock)
end