WorkshopNevealSkillShowView = WorkshopNevealSkillShowView or DeclareView("WorkshopNevealSkillShowView", "workshop/workshop_neveal_skill_show")
VIEW_DECLARE_MASK(WorkshopNevealSkillShowView, ViewMask.BgBlockCloseNoAnim)

function WorkshopNevealSkillShowView:WorkshopNevealSkillShowView()
end

function WorkshopNevealSkillShowView:LoadCallback(param_t)
    UH.SetIcon(self.Icon, param_t.icon)
    UH.SetText(self.Name, param_t.name)
    UH.SetText(self.desc, param_t.desc)
end 