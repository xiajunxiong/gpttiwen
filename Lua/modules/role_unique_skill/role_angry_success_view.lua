AngrySoulSuccessShow = AngrySoulSuccessShow or DeclareView("AngrySoulSuccessShow", "role/nu_hun_success")

function AngrySoulSuccessShow:AngrySoulSuccessShow()
    self.data = RoleAngrySoulData.Instance
end

function AngrySoulSuccessShow:LoadCallback()
    local param = self.data:GetSuccessParam()
    UH.SetText(self.SoulName, param.name)
    LogError("?ASD",param.type)
    UH.SpriteName(self.SoulIcon,"NvHun"..param.type)
    UH.SetText(self.LeftAttr, param.cur_attr)
    UH.SetText(self.RightAttr, param.next_attr)

    UH.SetText(self.LeftEff, param.cur_eff_show)
    UH.SetText(self.RightEff, param.next_eff_show)

    self.NomalTitle:SetActive(not param.is_active)
    self.ActTitle:SetActive(param.is_active)
    UH.LocalPosG(self.RightShow, Vector3.New(param.is_active and 11 or 248,-125,0))
    self.LeftShow:SetActive(not param.is_active)
end 

function AngrySoulSuccessShow:OnClickBlock()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(AngrySoulSuccessShow)
end
