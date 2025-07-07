GoldRMinecPreView = GoldRMinecPreView or DeclareView("GoldRMinecPreView", "serveractivity/goldr_minec/goldr_minec_preview")
-- VIEW_DECLARE_LEVEL(GoldRMinecPreView, ViewLevel.L0)
function GoldRMinecPreView:GoldRMinecPreView()
	self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end

function GoldRMinecPreView:LoadCallback(param_t)
    self.data = GoldRMinecData.Instance

    self.Board:SetData(GoldRMinecPreView, Language.GoldRMinec.PreviewTitle,Vector2.New(546, 410))
    self.ItemList:SetData(self.data:GetItemPreViewList())
end

---------预览界面item------------------
GoldRMinecPreviewItem = GoldRMinecPreviewItem or DeclareMono("GoldRMinecPreviewItem",UIWidgetBaseItem)
function GoldRMinecPreviewItem:GoldRMinecPreviewItem() end
function GoldRMinecPreviewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(Item.Create(data.reward_item))
    UH.SetText(self.RateText, (data.show_rate*100).."%")
end