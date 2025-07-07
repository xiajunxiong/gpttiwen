
PartnerSkillfUpView = PartnerSkillfUpView or DeclareView("PartnerSkillfUpView", PartnerConfig.ResPath .. "partner_skillf_up")
VIEW_DECLARE_MASK(PartnerSkillfUpView, ViewMask.BgBlockCloseNoAnim)

function PartnerSkillfUpView:PartnerSkillfUpView()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Up
end

function PartnerSkillfUpView:LoadCallback()
    if -1 == self.data.cur_sel_partner then return end
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local skill_f = data.skill_f
    local info = skill_f.skill_cur
    if nil == info then return end
    self.Name.text = info.name
    self.Tips.text = string.format(self.language.skillf_tips, data.info.name, skill_f.info.level)
    self.Level.text = string.format(self.language.skillf_level2, skill_f.info.level)
    self.SkillCell:SetData(info)
    for i = 1, self.SkillFAttrsCur:Length() do
        local attrs = skill_f.attrs_cur or skill_f.attrs_next
        local attr = attrs[i]
        if attr then
            self.SkillFAttrsCur[i].text = string.format(self.language.skillf_attr2, attr.attr_name, attr.attr_value)
        end
    end
end
