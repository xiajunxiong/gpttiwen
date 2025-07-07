PetFSSkillLvupRetView = PetFSSkillLvupRetView or DeclareView("PetFSSkillLvupRetView", "pet/pet_fsskill_lvup_ret")
-- VIEW_DECLARE_LEVEL(PetFeiShengSucc,ViewLevel.L1)
VIEW_DECLARE_MASK(PetFSSkillLvupRetView, ViewMask.BgBlockClose)
--SkillItem1=====PetFSSkillLvupRetItem
--SkillItem2=====PetFSSkillLvupRetItem
function PetFSSkillLvupRetView:PetFSSkillLvupRetView()

end

--param_t={skill_id,level}
function PetFSSkillLvupRetView:LoadCallback(param_t)
    local skill_id = param_t.skill_id
    local level = param_t.level
    local lvup_cfg1 = PetFeiShengData.SkillLvupCfg(skill_id,level)
    local desc_cfg1 = PetData.Instance:GetGiftDetail(skill_id,level)
    local lvup_cfg2 = PetFeiShengData.SkillLvupCfg(skill_id,level + 1)
    local desc_cfg2 = PetData.Instance:GetGiftDetail(skill_id,level + 1)
    self.SkillItem1:FlushShow(desc_cfg1,lvup_cfg1)
    self.SkillItem2:FlushShow(desc_cfg2,lvup_cfg2)
end

function PetFSSkillLvupRetView:CloseCallback()
    -- local cur_sel_skill = PetData.Instance:GetClickSkill()
    -- if cur_sel_skill ~= nil then
    --     cur_sel_skill.skill_level = cur_sel_skill.skill_level + 1
    -- end
    -- PetFeiShengData.Instance:FlushSkillLvupView()
end


PetFSSkillLvupRetItem = PetFSSkillLvupRetItem or DeclareMono("PetFSSkillLvupRetItem", UIWidgetBaseItem)
--Desc====Text
--Attri1==Text
--Attri2==Text
--Icon====UIImageSpriteSet
--Lv======Text

function PetFSSkillLvupRetItem:FlushShow(desc_cfg,lvup_cfg)
    local attri_format = "%s <color=#"..COLORSTR.Green9..">+%s</color>"
    local desc = string.gsub(desc_cfg.description,"02CF7A",COLORSTR.Green9)
    UH.SetText(self.Desc,string.format(Language.PetFeiSheng.SkillLvupRetDesc,desc))
    UH.SetText(self.Attri1,string.format(attri_format,Language.Common.AttrList[lvup_cfg.att_type1],DataHelper.Percent(lvup_cfg.att_type1, lvup_cfg.att_num1)))
    UH.SetText(self.Attri2,string.format(attri_format,Language.Common.AttrList[lvup_cfg.att_type2],DataHelper.Percent(lvup_cfg.att_type2, lvup_cfg.att_num2)))
    UH.SetText(self.Lv,string.format("lv.%d",desc_cfg.skill_level))
    UH.SetIcon(self.Icon,desc_cfg.icon_id,ICON_TYPE.SKILL)
end