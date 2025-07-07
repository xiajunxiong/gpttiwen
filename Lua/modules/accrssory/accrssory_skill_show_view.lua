AccrssorySkillShowView = AccrssorySkillShowView or DeclareView("AccrssorySkillShowView", "accrssory/accrssory_skill_show")
VIEW_DECLARE_MASK(AccrssorySkillShowView, ViewMask.BlockClose)

function AccrssorySkillShowView:AccrssorySkillShowView()
    self.language = Language.Accrssory
    self.Data = AccrssoryData.Instance
end

function AccrssorySkillShowView:LoadCallback(param_t)
    local skill_config = param_t.skill_config
    param_t = param_t or {}
    if TableIsEmpty(skill_config) then
        ViewMgr:CloseView(AccrssorySkillShowView)
        return
    end
    UH.LocalPosG(self.ObjPos, param_t.pos or Vector3.New(0, 0, 0))
    UH.SetIcon(self.ImgSetIcon, skill_config.icon_id)
    UH.SetText(self.TxtLevel1, Format(self.language.SkilLv, skill_config.skill_level))
    UH.SetText(self.TxtLevel2, Format(self.language.SkilLv, skill_config.skill_level))
    UH.SetText(self.TxtName, skill_config.skill_name)
    UH.SetText(self.TxtDesc, skill_config.description)
    local config = self.Data:GetSkillIdProfConfig()
    if config[skill_config.skill_id] and config[skill_config.skill_id] > 0 then
        UH.SetText(self.TxtType, Format(self.language.SpecialSkill, Language.Common.Prof[config[skill_config.skill_id]]))
    else
        UH.SetText(self.TxtType, self.language.BaseSkill)
    end
end