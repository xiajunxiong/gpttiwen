FavorNpcFeastView = FavorNpcFeastView or DeclareView("FavorNpcFeastView", "favor/favor_npc_feast")
VIEW_DECLARE_MASK(FavorNpcFeastView, ViewMask.BgBlockClose)
VIEW_DECLARE_LEVEL(FavorNpcFeastView, ViewLevel.Lt)

function FavorNpcFeastView:FavorNpcFeastView()
end

function FavorNpcFeastView:LoadCallback(paramt)
    local cfg_feast = paramt.cfg_feast
    self.Board:SetData(self:GetType(), cfg_feast.feast_name, Vector2.New(531, 381))
    UH.SetText(self.Desc, cfg_feast.feast_desc)
    self.List:SetData(FavorData.Instance:NpcBanquetList(cfg_feast))
end



