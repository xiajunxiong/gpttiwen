PreviewGiftView = PreviewGiftView or DeclareView("PreviewGiftView", "main_other/preview_gift")

function PreviewGiftView:PreviewGiftView()
    self.data = ChooseGiftUseData.Instance
    self.ht = self.data.preview_gift:Care(BindTool.Bind(self.FlushPanel, self))
end

function PreviewGiftView:LoadCallback()
    self:FlushPanel()
end

function PreviewGiftView:CloseCallback()
    self.data.preview_gift:Uncare(self.ht)
    self.data:SetPreviewGift(nil)
end

function PreviewGiftView:FlushPanel()
    local item = self.data:GetPreviewGift()
    if TableIsEmpty(item) then
        return
    end
    local item_list,config = BagData.Instance:GiftItemList(item.item_id)
    self.select_num = config.select_num
    for i=1,#item_list do
        item_list[i].index = i - 1
    end
    
    self.CallList:SetData(item_list)
end

function PreviewGiftView:OnClickClose()
    ViewMgr:CloseView(PreviewGiftView)
end