
ArrayGhostGuideView = ArrayGhostGuideView or DeclareView("ArrayGhostGuideView", "anecdote/array_ghost_guide")

function ArrayGhostGuideView:ArrayGhostGuideView()
end

function ArrayGhostGuideView:OnClickClose()
	ViewMgr:CloseView(ArrayGhostGuideView)
end

ArrayGhostGuideViewF = ArrayGhostGuideViewF or DeclareMono("ArrayGhostGuideViewF", UIWFlushPanel)

function ArrayGhostGuideViewF:ArrayGhostGuideViewF()
    self.data = ArrayGhostData.Instance
    self.language = Language.ArrayGhost

    self.data_cares = {
		{data = self.data.page_info, func = self.FlushPages},
	}
end

function ArrayGhostGuideViewF:FlushPages()
    local value = self.data.page_info.value
    self.BtnNextObj:SetActive(value < ArrayGhostConfig.MAX_PAGE)
    self.BtnPreObj:SetActive(value > 1)
    self.Tips.text = self.language.GuideTips[value]
    self.PicSp.SpriteName = "pic" .. value
end

function ArrayGhostGuideViewF:OnClickPage(value)
    self.data.page_info.value = self.data.page_info.value + value
end