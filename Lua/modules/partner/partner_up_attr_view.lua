
PartnerUpAttrView = PartnerUpAttrView or DeclareView("PartnerUpAttrView", PartnerConfig.ResPath .. "partner_up_attr")

function PartnerUpAttrView:PartnerUpAttrView()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner.Up
end

function PartnerUpAttrView:LoadCallback()
	self.Board:SetData(self:GetType(), self.language.UpAttr, Vector2.New(654, 520))
end

function PartnerUpAttrView:OnFlush()
	if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
	local vo = data.vo
	local level = vo.up_level
	local up_loop = data.up_loop
	local up_loop_t = data.up_loop_t
	local attr_stage = vo.attr_stage
	-- local att_type_name = self.language.Up.att_type
	self.UpLevel.text = string.format(self.language.up_attr_show, DataHelper.GetDaXie(vo.up_level))
	for i = 1, self.Attrs:Length() do
		local att_type = PartnerConfig.UpAttrType[i]
		local attr_name = Language.Common.AttrList[att_type]
		local attr_value = 0
		for l = 0, level - 1 do
			for _, v in pairs(up_loop_t[l]) do
				attr_value = attr_value + (v.att_type == att_type and v.att_value or 0)
			end
		end
		for k, v in pairs(up_loop) do
			attr_value = attr_value + ((v.att_type == att_type and k < attr_stage + 1) and v.att_value or 0)
		end
		attr_value = DataHelper.IsPercent(att_type) and Percent(attr_value or 0) or (attr_value or 0)
        self.Attrs[i].text = attr_name .. RichTextHelper.ColorStr(":" .. attr_value, COLORSTR.Green3)
	end

	local skill_f = data.skill_f
	self.SkillFCell:SetData(skill_f)
	self.SkillFName.text = skill_f.info.name
	-- self.SkillFLevel.text = nil == skill_f.skill_cur and self.language.skillf_unactive or string.format(self.language.skillf_level, skill_f.info.level)
	-- self.SkillLevelF.text = skill_f.info.level and string.format(Language.Partner.SkillInfo.skill_level_f, skill_f.info.level) or ""
	self.SkillLevelF.text = nil == skill_f.skill_cur and self.language.skillf_unactive or string.format(Language.Partner.SkillInfo.skill_level_f, skill_f.info.level)
	if nil ~= skill_f.info then
		self.SkillFCur:SetActive(true)
		for i = 1, self.SkillFAttrsCur:Length() do
			local attrs = skill_f.attrs_cur or skill_f.attrs_next
			local attr = attrs[i]
			if attr then
				self.SkillFAttrsCur[i].text = string.format(self.language.skillf_attr, attr.attr_name, attr.attr_value)
			end
		end
	else
		self.SkillFCur:SetActive(false)
	end
	-- if nil ~= skill_f.skill_next then
	-- 	self.SkillFNext:SetActive(true)
	-- 	for i = 1, self.SkillFAttrsNext:Length() do
	-- 		local attr = skill_f.attrs_next[i]
	-- 		if attr then
	-- 			self.SkillFAttrsNext[i].text = string.format(self.language.skillf_attr, attr.attr_name, attr.attr_value)
	-- 		end
	-- 	end
	-- else
	-- 	self.SkillFNext:SetActive(false)
	-- end
	-- self.SkillFLine:SetActive(nil ~= skill_f.skill_cur and nil ~= skill_f.skill_next)
end

