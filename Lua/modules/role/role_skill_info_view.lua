
RoleSkillInfoView = RoleSkillInfoView or DeclareView("RoleSkillInfoView", "role/role_skill_info")
VIEW_DECLARE_MASK(RoleSkillInfoView,ViewMask.BlockClose)

function RoleSkillInfoView:RoleSkillInfoView()
	self.language = Language.Role
end

function RoleSkillInfoView:LoadCallback(param_t)
    if TableIsEmpty(param_t) then
        return
    end
    self.data = param_t
    local is_learn = not TableIsEmpty(RoleCtrl.Instance.skill_data:getSkillDataById(param_t.skill_id))
    if is_learn then
        self.level = RoleCtrl.Instance.skill_data:getSkillDataById(param_t.skill_id).skill_level
        self.BtnSkillObj:SetActive(false)
    else
        self.level = 0
        -- 判断当前是否可以激活
        local is_limit, limitType, limitStr = SkillData.Instance:GetSkillLimit(param_t.skill_id)
        self.BtnSkillObj:SetActive(not is_limit)
    end
    
    if self.level > 0 then
        self.desc_cfg = Cfg.SkillDescPartnerAndRole(param_t.skill_id, self.level)
    else
        self.desc_cfg = {}
    end
    self.next_level = self.level + 1
    self.next_desc_cfg = Cfg.SkillDescPartnerAndRole(param_t.skill_id, self.next_level) or {}

    self.CurInfoObj:SetActive(self.level > 0)
    self.NextInfoObj:SetActive(not TableIsEmpty(self.next_desc_cfg))
    self.Line2Obj:SetActive(self.level > 0 and not TableIsEmpty(self.next_desc_cfg))

    UH.SetText(self.SkillName, self.desc_cfg.skill_name or self.next_desc_cfg.skill_name)

    if self.level == 0 then
        UH.SetText(self.SkillLevel, self.language.PromoteLevel2)
    elseif TableIsEmpty(self.next_desc_cfg) then
        UH.SetText(self.SkillLevel, self.language.PromoteLevel3)
    else
        UH.SetText(self.SkillLevel, Format(self.language.PromoteLevel, self.level))
    end
    UH.SetText(self.SkillTpye, Format(self.language.PromoteLevelType, self.desc_cfg.corner_icon or self.next_desc_cfg.corner_icon))

    if not TableIsEmpty(self.desc_cfg) then
        UH.SetText(self.CurDesc, self.desc_cfg.description)
	end
    if not TableIsEmpty(self.next_desc_cfg) then
        UH.SetText(self.NextDesc, self.next_desc_cfg.description)
        local next_prof_cfg = RolePromoteData.GetRoleProfNextConfig()
        if not TableIsEmpty(next_prof_cfg) then
            UH.SetText(self.NextCondition, Format(self.language.PromoteNeedNextLevel, next_prof_cfg.job_name, DataHelper.GetDaXie(next_prof_cfg.order + 1)))
        else
            UH.SetText(self.NextCondition, "")
        end
    end
    
    UH.SetIcon(self.SkillIcon, self.desc_cfg.icon_id or self.next_desc_cfg.icon_id, ICON_TYPE.SKILL)
end

function RoleSkillInfoView:OnClickSkill()
    RoleSkillData.Instance:SetNeedJumpSkillId(self.data.skill_id)
    ViewMgr:OpenViewByKey(Mod.Role.RoleSkill)
	ViewMgr:CloseView(RolePromoteView)
	ViewMgr:CloseView(RolePromoteSucceView)
	ViewMgr:CloseView(RoleSkillInfoView)
end