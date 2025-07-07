PalacesResult = PalacesResult or DeclareView("PalacesResult", "god_areas_palaces/palaces_result")
VIEW_DECLARE_MASK(PalacesResult, ViewMask.BgBlockClose)
function PalacesResult:PalacesResult()
    self.data = PalacesData.Instance
end
function PalacesResult:LoadCallback()
   
    


end


PalacesResultItem = PalacesResultItem or DeclareMono("PalacesResultItem", UIWidgetBaseItem)
function PalacesResultItem:PalacesResultItem()
    self.palaces_data = PalacesData.Instance
    self.rank_sp = {
        "DiYiMing", "DiErMing", "DiSanMing", "DiNMingDi"
    }
end
function PalacesResultItem:SetData(data)
    
end