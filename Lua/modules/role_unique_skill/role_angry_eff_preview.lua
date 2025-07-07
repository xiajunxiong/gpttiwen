AngrySoulEffPreView = AngrySoulEffPreView or DeclareView("AngrySoulEffPreView", "role/nu_hun_eff_pre")

function AngrySoulEffPreView:AngrySoulEffPreView()
    self.data = RoleAngrySoulData.Instance
end

function AngrySoulEffPreView:LoadCallback(param_t)
    if param_t.type ~= nil then 
        local list = self.data:GetPreViewList(param_t.type)
        self.EffList:SetData(list)
    end 
end 

function AngrySoulEffPreView:OnClickArea()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(AngrySoulEffPreView)
end

AngrySoulEffPreItem = AngrySoulEffPreItem or DeclareMono("AngrySoulEffPreItem", UIWidgetBaseItem)
function AngrySoulEffPreItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    
    UH.SetText(self.Level,data.level..Language.Common.Level)
    UH.SetText(self.AttrStr,data.attr_str)
    UH.SetText(self.SpShow,data.sp_show)
end 
