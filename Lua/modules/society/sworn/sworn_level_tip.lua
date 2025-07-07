SwornLevelTip = SwornLevelTip or DeclareView("SwornLevelTip", SwornConfig.ResPath .. "sworn_level")
VIEW_DECLARE_MASK(SwornLevelTip, ViewMask.BgBlockClose)

function SwornLevelTip:LoadCallback()
    self.Board:SetData(self:GetType(), Vector2.New(466, 528))

    local show_list = SwornData.Instance:GetLevelList()
    self.LevelList:SetData(show_list)
end

function SwornLevelTip:OnClickClose()
    ViewMgr:CloseView(SwornLevelTip)
end

SwornLevelTipItem = SwornLevelTipItem or DeclareMono("SwornLevelTipItem", UIWidgetBaseItem)
function SwornLevelTipItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.TitleText, data.level_name)
    self.Current:SetActive(data.friendship_level == SwornData.Instance:GetSwornLevel())
    if data.friendship_level == 1 then
        UH.SetText(self.TermText, Language.Sworn.LevelCondition1)
    else
        UH.SetText(self.TermText, Format(Language.Sworn.LevelCondition2, data.friendship_time, data.friendship_intimacy))
    end
    UH.SetText(self.AddText, Format(Language.Sworn.LevelUnlock, data.skill_name))
end
