
AttrExplainView = AttrExplainView or DeclareView("AttrExplainView","item_info/attr_explain")

VIEW_DECLARE_MASK(AttrExplainView, ViewMask.BlockClose)

function AttrExplainView:AttrExplainView()
end

function AttrExplainView:LoadCallback()
	self.AttrList:SetData(Language.ItemInfo.AttrExplains)
end

function AttrExplainView:CloseCallback()
	 AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

AttrExplainViewItem = AttrExplainViewItem or DeclareMono("AttrExplainViewItem", UIWidgetBaseItem)

function AttrExplainViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	UH.SetText(self.AttrName, data.name)
	local len = string.len(data.name) / 3
	UH.SizeDelta(self.AttrDesc, Vector3.New(576 - 22 * len))
	UH.SetText(self.AttrDesc, data.desc)
end