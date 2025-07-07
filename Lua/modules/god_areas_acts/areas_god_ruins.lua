AreasGodRuinsPanel = AreasGodRuinsPanel or DeclareMono("AreasGodRuinsPanel", UIWFlushPanel)
function AreasGodRuinsPanel:AreasGodRuinsPanel()
    self.data = RuinsData.Instance
end
function AreasGodRuinsPanel:onFlush()
    self.List:SetData(DataHelper.FormatItemList(self.data:GetPreReward()))
    local open_time = self.data:OpenTime()
    local end_time = self.data:EndTime()
    UH.SetText(self.Time, string.format(Language.GodAreasActs.RuinsTime, open_time.month, open_time.day, open_time.hour, open_time.min,
    end_time.hour, end_time.min))
end
function AreasGodRuinsPanel:OnClickBattle()
    ViewMgr:CloseView(GodAreasActs)
    local pos_info = LandsBetweenData.Instance:CheckSpPicAroundByActType(ActType.Ruins)
    local fix_link = LandsBetweenData.FixLink(pos_info.center_link_mark)

    local pos = {
        x = fix_link[1],
        y = fix_link[2]
    }
    LandsBetweenData.Instance:SetMapClick(pos)
    LandsBetweenData.Instance:SetMapJumpClick(pos)
    --[[ local act_info = RuinsData.Instance:GetActInfosm().info
    if act_info.state and act_info.state ~= ActStatusType.Close then
        RuinsCtrl.Instance:EnterScene()
    else
        PublicPopupCtrl.Instance:Center(Language.GodAreasActs.ActTip)
    end ]]

    -- SceneCtrl.Instance:GotoScene(RuinsScene)
    -- sinValue1 = Mathf.Cos(150);
    -- sinValue2 = Mathf.Sin(150);
    -- virCam.transform.Translate(Vector3.right * (Input.GetAxis("Mouse X") + Input.GetAxis("Mouse Y") * sinValue1));
    -- virCam.transform.Translate(Vector3.forward * (Input.GetAxis("Mouse Y") + Input.GetAxis("Mouse X") * sinValue2));
end
function AreasGodRuinsPanel:OnClickChengJiu()
    ViewMgr:OpenView(RuinsAchi)
end
function AreasGodRuinsPanel:OnClickRank()
    ViewMgr:OpenView(RuinsRank)
end
function AreasGodRuinsPanel:OnClickTip()
    PublicPopupCtrl.Instance:HelpTip(377)
end