
PartnerSkillInfoView = PartnerSkillInfoView or DeclareView("PartnerSkillInfoView", "item_info/partner_skill_info")
VIEW_DECLARE_MASK(PartnerSkillInfoView,ViewMask.BlockClose)

function PartnerSkillInfoView:PartnerSkillInfoView()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner.SkillInfo
end

function PartnerSkillInfoView:LoadCallback()
	local skill_info = self.data.cur_skill_info
	if nil == skill_info then return end
	local skill = skill_info.info
	local skill_cur = skill_info.skill_cur
	local skill_next = skill_info.skill_next
	local skill_type = skill_info.skill_type

	self.CurInfoObj:SetActive(nil ~= skill_cur)
	self.NextInfoObj:SetActive(nil ~= skill_next)
	self.Line2Obj:SetActive(nil ~= skill_cur and nil ~= skill_next)
	self.BtnSkillObj:SetActive(not skill_info.is_lock and PartnerConfig.SkillType.active == skill_type)
	self.TipsObj:SetActive(PartnerConfig.SkillType.friendly == skill_type)

	self.SkillCell:SetData(skill)
	self.SkillName.text = skill.name
	if nil == skill_next then
		self.SkillLevel.text = self.language.skill_max
	elseif nil == skill_cur then
		self.SkillLevel.text = self.language.skill_unactive
	else
		self.SkillLevel.text = string.format(self.language.skill_level, skill.skill_level or skill.level or 0)
	end
	self.SkillTpye.text = self.language.skill_type[skill_type]

	if nil ~= skill_cur then
		self.CurDesc:SetObjActive(PartnerConfig.SkillType.friendly ~= skill_type)
		self.CurAttsObj:SetActive(PartnerConfig.SkillType.friendly == skill_type)

		self.CurDesc.text = skill_cur.desc or ""
		for i = 1, self.CurAttrs:Length() do
        	local attr = (skill_info.attrs_cur or {})[i]
        	if attr then
            	self.CurAttrs[i].text = string.format(self.language.skillf_attr, attr.attr_name, attr.attr_value)
        	end
    	end
	end
	if nil ~= skill_next then
		self.NextDesc:SetObjActive(PartnerConfig.SkillType.friendly ~= skill_type)
		self.NextAttsObj:SetActive(PartnerConfig.SkillType.friendly == skill_type)
		self.NextCondition.text = string.format(self.language.next_condition[skill_type], skill_info.partner_name, skill_info.need_level)

		self.NextDesc.text = skill_next.desc or ""
		for i = 1, self.NextAttrs:Length() do
        	local attr = (skill_info.attrs_next or {})[i]
        	if attr then
            	self.NextAttrs[i].text = string.format(self.language.skillf_attr, attr.attr_name, attr.attr_value)
        	end
    	end
	end
	self.BtnSkill.text = skill_info.is_equip and self.language.skill_forbid or self.language.skill_open
end

function PartnerSkillInfoView:OnClickSkill()
	local skill_info = self.data.cur_skill_info
	PartnerCtrl.Instance:SendPartnerSkillActivate{index = skill_info.index, is_equip = skill_info.is_equip}
	ViewMgr:CloseView(PartnerSkillInfoView)
end