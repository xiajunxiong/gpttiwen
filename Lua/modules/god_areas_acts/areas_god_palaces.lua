AreasGodPalacesPanel = AreasGodPalacesPanel or DeclareMono("AreasGodPalacesPanel", UIWFlushPanel)
function AreasGodPalacesPanel:AreasGodPalacesPanel()
    self.data = PalacesData.Instance
end
function AreasGodPalacesPanel:onFlush()
    self.List:SetData(DataHelper.FormatItemList(self.data:GetPreReward()))
    local open_time = self.data:OpenTime()
    local end_time = self.data:EndTime()
    UH.SetText(self.Time,
        string.format(Language.GodAreasActs.RuinsTime, open_time.month, open_time.day, open_time.hour, open_time.min,
            end_time.hour, os.date("%M", end_time.min)))
end
function AreasGodPalacesPanel:OnClickBattle()
    ViewMgr:CloseView(GodAreasActs)
    local pos_info = LandsBetweenData.Instance:CheckSpPicAroundByActType(ActType.Palaces)
    local fix_link = LandsBetweenData.FixLink(pos_info.center_link_mark)

    local pos = {
        x = fix_link[1],
        y = fix_link[2]
    }
    LandsBetweenData.Instance:SetMapClick(pos)
    LandsBetweenData.Instance:SetMapJumpClick(pos)
    
    --[[ local act_info = PalacesData.Instance:GetActInfosm().info
    if act_info.state and act_info.state ~= ActStatusType.Close then
        PalacesCtrl.Instance:EnterScene()
    else
        PublicPopupCtrl.Instance:Center(Language.GodAreasActs.ActTip)
    end ]]
end
function AreasGodPalacesPanel:OnClickChengJiu()
    ViewMgr:OpenView(PalacesAchi)
end
function AreasGodPalacesPanel:OnClickRank()
    ViewMgr:OpenView(PalacesRank)
end
function AreasGodPalacesPanel:OnClickTip()
    PublicPopupCtrl.Instance:HelpTip(389)
end
