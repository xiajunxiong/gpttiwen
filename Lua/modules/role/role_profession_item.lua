RoleProfessionItem = RoleProfessionItem or DeclareMono("RoleProfessionItem", UIWidgetBaseItem)

function RoleProfessionItem:RoleProfessionItem()
    -- body
end

function RoleProfessionItem:SetData(data)
    self.data = data
    UH.SpriteName(self.SelectIcon,RoleData.GetProfSp(data.profession,true).."_icon")
    UH.SpriteName(self.NomalIcon,RoleData.GetProfSp(data.profession,true).."_icon")
    local prof_name = Language.Common.Prof[data.profession]
    self.NomalName.text = prof_name
    self.SelectName.text = prof_name
end

function RoleProfessionItem:Click()
    self.EventClick:Trigger()
end

function RoleProfessionItem:Select()
    local profession_view_data = RoleData.Instance:GetProfessionViewData()
    profession_view_data.select_profession = self.data.profession
end


RoleProfessionSkillItem = RoleProfessionSkillItem or DeclareMono("RoleProfessionSkillItem", UIWidgetBaseItem)

function RoleProfessionSkillItem:RoleProfessionSkillItem()
    -- body
end

function RoleProfessionSkillItem:SetData(data)
    self.data = data
    self.SkillName.text = data.skill_name
    --self.SkillIcon.SpriteName = data.icon_id or ""
    UH.SetIcon(self.SkillIcon,data.icon_id,ICON_TYPE.SKILL)
end

function RoleProfessionSkillItem:Click()
    local desc_info = Cfg.GetChangeJobDesc(RoleData.Instance:GetProfessionViewData().select_profession,self.data.skill_id)
    if desc_info == nil then 
        LogError("empty desc!",RoleData.Instance:GetProfessionViewData().select_profession,self.data.skill_id)
        return 
    end 
    local data = {
        tips_str = string.format(Language.Role.RolePromoteSkillTips,desc_info.skill_name,desc_info.skill_level,desc_info.description),
        pos_info = self.SkillItemRect.localPosition,
        height = self.SkillItemRect.rect.height,
        onclick = true
    }

    SkillData.Instance:OperateRoleProfViewSkill(data)
   -- PublicPopupCtrl.Instance:OpenTipsPopup(data)
end