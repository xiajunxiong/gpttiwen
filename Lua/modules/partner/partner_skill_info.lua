
local name2tab = {
    single_skill_auto = Cfg.SkillSingle,
    power_skill_auto = Cfg.SkillPower,
    super_power_skill_auto = Cfg.SkillSuperPower,
    random_skill_auto = Cfg.SkillRandom,
    combo_skill_auto = Cfg.SkillCombo,
    line_skill_auto = Cfg.SkillLine,
    passive_skill_auto = Cfg.SkillPassive,
}

PartnerSkillInfo = PartnerSkillInfo or BaseClass()

function PartnerSkillInfo:__init(param_t)
	self.id = param_t.skill_id
	self.skill_index = param_t.skill_index
	self.need_level = param_t.need_level
	self.min_skill_level = param_t.min_skill_level

	self:PartnerSkillInfo()
end

function PartnerSkillInfo:PartnerSkillInfo()
	local t = {}
	for _, v in pairs(Config.skill_description_auto.partner_skill_description) do
		if v.skill_id == self.id then
			t[v.skill_level] = {
				skill_id = v.skill_id,
				skill_level = v.skill_level,
				name = v.show_name,
				desc = v.description,
				desc2 = v.description_2,
				desc_qua = v.parnter_quality_description,
				icon_id = v.icon_id,
				need_level = self.need_level,
				skill_index = self.skill_index,
				consume_mp = -1,
				corner_icon = v.corner_icon
			}
			local func = name2tab[v.table_name]
			if func then
				local co = func(v.skill_id, v.skill_level)
				if co then
					t[v.skill_level].consume_mp = co.consume_mp
				else
					Debuger.LogError(string.format("partner_skill_description error!skill_id:%s,skill_level:%s", v.skill_id, v.skill_level))
				end
			else
				Debuger.LogError(string.format("partner_skill_description error!skill_id:%s,skill_level:%s", v.skill_id, v.skill_level))
			end
		end
	end
	local tm = t[self.min_skill_level] or {}
	for k, v in pairs(t) do
		if k < self.min_skill_level then
			t[k] = tm
		end
	end
	self.info = t
end