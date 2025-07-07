WorkshopNevealSkillTip = WorkshopNevealSkillTip or DeclareView("WorkshopNevealSkillTip", "workshop/workshop_neveal_skill_tip")
VIEW_DECLARE_MASK(WorkshopNevealSkillTip, ViewMask.BlockClose)
-- BtnForget
function WorkshopNevealSkillTip:WorkshopNevealSkillTip()
    self.data = WorkshopData.Instance
end
function WorkshopNevealSkillTip:LoadCallback(param)
    AudioMgr:PlayEffect(AudioType.UI, "view_open_l2")
    if param ~= nil and param.pos ~= nil  then 
        UH.AnchoredPosition(self.PosRect, param.pos)
    end 
end
function WorkshopNevealSkillTip:OnFlush()
    local data = self.data:GetNevealClickSkill()
    if data == nil then
        LogError("WorkshopNevealSkillTip GetNevealClickSkill data is nil")
        return
    end
    local unlock_level = BagData.Instance:GetNevealSkillLevel(data.effect_id)
    self.cur_skill_cfg = WorkshopData.Instance:GetNevealSkillCfgByUnlockLevel(data.effect_id,unlock_level)
    if self.cur_skill_cfg == nil then self.cur_skill_cfg = WorkshopData.Instance:GetNevealSkillCfg(data.effect_id,1) end 
    UH.SetIcon(self.SkillIcon,self.cur_skill_cfg.skill_icon)
    UH.SetText(self.LblName,self.cur_skill_cfg.skill_name)

    if unlock_level > 0 then 
        UH.SetText(self.LblDesc,string.format(Language.Pet.SkillShow, 
        self.cur_skill_cfg.set_skill_description..string.format(Language.Workshop.Neveal.LevelNeedPlus, 
            ColorStr(self.cur_skill_cfg.skill_unlock_level, COLORSTR.Green3),
            ColorStr(unlock_level,COLORSTR.Green3))))
    else 
        UH.SetText(self.LblDesc,string.format(Language.Pet.SkillShow, self.cur_skill_cfg.set_skill_description))
    end 

    -- UH.SetText(self.LblName,555)
    -- UH.SetText(self.LblDesc,555)
    -- Uh.SETTEXTY 
end 

function WorkshopNevealSkillTip:OnClickMainSkillInfo()
    local data = self.data:GetNevealClickSkill()
    self.MainSkillTipObj:SetActive(false)
    self.MainSkillInfoObj:SetActive(true)

    local list = WorkshopData.Instance:GetNevealSkillTipList(self.cur_skill_cfg)
    self.MainSkillInfoList:SetData(list)
    self.MainSkillInfoList:JumpVerticalIndex(self.cur_skill_cfg.skill_level, 114)
end