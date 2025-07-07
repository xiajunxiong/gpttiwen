SwornSkillTip = SwornSkillTip or DeclareView("SwornSkillTip", SwornConfig.ResPath .. "sworn_skill_tip")
VIEW_DECLARE_MASK(SwornSkillTip, ViewMask.BlockClose)

function SwornSkillTip:LoadCallback(param)
    AudioMgr:PlayEffect(AudioType.UI, "view_open_l2")
    if param ~= nil and param.pos ~= nil then
        UH.AnchoredPosition(self.PosRect, param.pos)
    end
    self.skill_cfg = param.skill_cfg
end

function SwornSkillTip:OnFlush()
    if self.skill_cfg == nil then
        return
    end
    UH.SetText(self.LblName, self.skill_cfg.skill_name)
    UH.SetText(self.LblDesc, string.format(Language.Pet.SkillShow, self.skill_cfg.description))
    UH.SetIcon(self.SkillIcon, self.skill_cfg.icon_id, ICON_TYPE.SKILL)
end

function SwornSkillTip:OnClickMask()
    ViewMgr:CloseView(SwornSkillTip)
end
