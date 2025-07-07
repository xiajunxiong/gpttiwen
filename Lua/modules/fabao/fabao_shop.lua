FabaoShop = FabaoShop or DeclareView("FabaoShop", "fabao/fabao_shop")
----Board=======UIWBoard
----Tabbar======UIWidgetList
function FabaoShop:FabaoShop()
    self.lang = Language.Fabao
    self.fabao_data = FabaoData.Instance
end
function FabaoShop:LoadCallback()
    self.Board:SetData(self:GetType(), self.lang.ShopTitle,Vector2.New(1008,522))
    -- self.Board:SetTabbarVisible(true)
    --self.Currency:DefaultSet()
end

function FabaoShop:OnFlush()
    -- body
end
function FabaoShop:CloseCallback()
end
