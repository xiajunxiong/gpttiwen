ComposeDefeat = ComposeDefeat or DeclareView("ComposeDefeat", "workshop/compose_defeat")
VIEW_DECLARE_MASK(ComposeDefeat,ViewMask.BgBlockClose)
function ComposeDefeat:ComposeDefeat()
    self.data = WorkshopData.Instance
end
function ComposeDefeat:OnFlush()
    local info = self.data:ComposeResult()
    local item = Item.Create(info.item)
    self.Cell:SetData(item)
    UH.SetText(self.Desc, string.format(Language.Workshop.ReturnItem, item:QuaName()))
end
