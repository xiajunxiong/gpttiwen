TombRaiderCampView = TombRaiderCampView or DeclareView("TombRaiderCampView", "tomb_raider/tomb_raider_camp_view")
function TombRaiderCampView:TombRaiderCampView()end
function TombRaiderCampView:OnClickClose()
    ViewMgr:CloseView(TombRaiderCampView)
end

--===========================TombRaiderGuidePanel===========================
TombRaiderCampPanel = TombRaiderCampPanel or DeclareMono("TombRaiderCampPanel", UIWFlushPanel)

function TombRaiderCampPanel:TombRaiderCampPanel()
    self.data = TombRaiderData.Instance
    self.data_cares = {
        {data =self.data.tomb_team,func = self.TeamInfoFlush},
    }
end

function TombRaiderCampPanel:TeamInfoFlush()
    local list_defend,list_attack = self.data:GetBattleCampInfo()
    for k,v in pairs(list_attack) do 
        v.rank = k
    end 

    for k,v in pairs(list_defend) do 
        v.rank = k
    end 
    self.teamRaider:SetData(list_attack)
    self.teamGuard:SetData(list_defend)
end

--============================TombCampItem=========================================
TombCampItem = TombCampItem or DeclareMono("TombCampItem", UIWidgetBaseItem)
function TombCampItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.Name,data.play_name)
    UH.SetText(self.Feats,data.gongxian)
    if TombRaiderConfig.RankSp[data.rank] then 
        UH.SpriteName(self.RankShow, TombRaiderConfig.RankSp[data.rank])
    end 
end