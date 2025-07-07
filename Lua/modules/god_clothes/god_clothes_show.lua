GodClothesShowView = GodClothesShowView or DeclareView("GodClothesShowView", "god_clothes/god_clothes_show")
VIEW_DECLARE_MASK(GodClothesShowView, ViewMask.BgBlock)
function GodClothesShowView:GodClothesShowView()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
end

function GodClothesShowView:LoadCallback(param_t)
    self.m_type = param_t.m_type
    local is_rand = self.m_type == GodClothesItemType.Rand
    self.ObjTitle1:SetActive(is_rand)
    self.ObjDesc1:SetActive(is_rand)
    UH.SpriteName(self.ImgBg, is_rand and "TanChuangDaoJuDi" or "XiYouDi")
    self.ObjRandShow:SetActive(is_rand)
    if self.m_type == GodClothesItemType.Select then
        self.List:SetData(self.Data:SelectShow())
    else
        self.List:SetData(self.Data:RandShow())
    end
end

function GodClothesShowView:OnCloseClick()
    ViewMgr:CloseView(GodClothesShowView)
end

--------------GodClothesShowItem--------------
GodClothesShowItem = GodClothesShowItem or DeclareMono("GodClothesShowItem", UIWidgetBaseItem)
function GodClothesShowItem:GodClothesShowItem()
    self.Data = GodClothesData.Instance
    self.language = Language.GodClothes
end

function GodClothesShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.New(data.reward_item)
    UH.SetText(self.TxtName, item:QuaName(true))
    UH.SetText(self.TxtRate, Format(self.language.Rate, data.show_rate))
    self.Cell:SetData(item)
end