GodClothesResultView = GodClothesResultView or DeclareView("GodClothesResultView", "god_clothes/god_clothes_result")
VIEW_DECLARE_MASK(GodClothesResultView, ViewMask.BgBlockClose)
function GodClothesResultView:GodClothesResultView()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
end

function GodClothesResultView:LoadCallback(param_t)
    local list = {}
    for k, v in pairs(param_t.data) do
        table.insert(list, self.Data:GetReward(v.group, v.seq))
    end
    self.List:SetData(list)
end

function GodClothesResultView:CloseCallback()

end

function GodClothesResultView:OnCloseClick()
    ViewMgr:CloseView(GodClothesResultView)
end

--------------GodClothesResultItem--------------
GodClothesResultItem = GodClothesResultItem or DeclareMono("GodClothesResultItem", UIWidgetBaseItem)
function GodClothesResultItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create(data.reward_item)
    self.Cell:SetData(item)
    UH.SetText(self.TxtName, item:QuaName())
    self.ObjEffect:SetActive(data.hearsay > 0)
end