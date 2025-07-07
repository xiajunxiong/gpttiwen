LandsBetweenCityMatchView = LandsBetweenCityMatchView or DeclareView("LandsBetweenCityMatchView", "lands_between/lands_between_city_match")
VIEW_DECLARE_MASK(LandsBetweenCityMatchView, ViewMask.None)
function LandsBetweenCityMatchView:LandsBetweenCityMatchView()

end

function LandsBetweenCityMatchView:OnClickBlock()
    self.Panel.Animor:SetTrigger(APH("click"))
    TimeHelper:AddDelayTimer(function ()
        ViewMgr:CloseView(LandsBetweenCityMatchView)
    end, 2)
end

LandsBetweenCityMatchPanel = LandsBetweenCityMatchPanel or DeclareMono("LandsBetweenCityMatchPanel", UIWFlushPanel)
function LandsBetweenCityMatchPanel:LandsBetweenCityMatchPanel()
    self.data = LandsBetweenData.Instance

end 

function LandsBetweenCityMatchPanel:Awake()
    UIWFlushPanel.Awake(self)

    UH.SpriteName(self.city_info, self.data:GetMyCityIconStr())    
    UH.SetText(self.name,string.format(Language.LandsBetween.Desc.CityName[self.data:GetCurMapType()],
    Language.LandsBetween.FactionName[self.data.my_info.faction]))
end