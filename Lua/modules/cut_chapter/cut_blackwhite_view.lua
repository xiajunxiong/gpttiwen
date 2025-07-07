CutBlackWhiteView = CutBlackWhiteView or DeclareView("CutBlackWhiteView","cut_chapter/cut_blackwhite",Mod.CutChapter.BlackWhite,ViewLayer.Top+2)
VIEW_DECLARE_MASK(CutBlackWhiteView,ViewMask.None)

function CutBlackWhiteView:LoadCallback()

end

function CutBlackWhiteView:OpenCallback()

end

function CutBlackWhiteView:CloseCallback()
    CutChapterCtrl.Instance:CutChapterCloseFunc()
end

CutBlackWhiteFlush = CutBlackWhiteFlush or DeclareMono("CutBlackWhiteFlush",UIWFlushPanel)
function CutBlackWhiteFlush:CutBlackWhiteFlush()
    self.show_speed = 0.05
end 

function CutBlackWhiteFlush:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushShow()
end

function CutBlackWhiteFlush:FlushShow()
    local strs_ = CutChapterCtrl.Instance:GetCutChapterWorks()
    self.ContentT:ShowContent(strs_, self.show_speed)
end

function CutBlackWhiteFlush:OnClickClose()
    ViewMgr:CloseView(CutBlackWhiteView)
end

function CutBlackWhiteFlush:TypeWriterEnd()
    self.BlockObj:SetActive(true)
end

function CutBlackWhiteFlush:OnClickSkip()
    if self.ContentT:Skip() then self:TypeWriterEnd() end
end