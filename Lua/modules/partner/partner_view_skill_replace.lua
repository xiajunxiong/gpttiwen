
PartnerViewSkillReplace = PartnerViewSkillReplace or DeclareView("PartnerViewSkillReplace", PartnerConfig.ResPath .. "partner_skill_replace")

function PartnerViewSkillReplace:PartnerViewSkillReplace()
    self.language = Language.Partner
end

function PartnerViewSkillReplace:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.PartnerSkillReplace, Vector2.New(831, 474))
end

PartnerViewSkillReplaceUp = PartnerViewSkillReplaceUp or DeclareMono("PartnerViewSkillReplaceUp", UIWFlushPanel)

function PartnerViewSkillReplaceUp:PartnerViewSkillReplaceUp()
    self.data = PartnerCtrl.Instance:Data()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    self.data.skill_replace_cur_sel_skill = nil
    for k, v in pairs(data.cur_skill_list_sort) do
        local cur_skill_info = self.data.cur_skill_info
        self.data.skill_replace_cur_sel_skill = self.data.skill_replace_cur_sel_skill
            or ((cur_skill_info and cur_skill_info.skill_id == v.info.skill_id) or ((nil == cur_skill_info or cur_skill_info.is_lock) and not v.is_using)) and k or nil
    end
end

function PartnerViewSkillReplaceUp:onFlush()
    local btn_replace = Language.Partner.BtnReplace
    self.BtnReplace.text = self.data.skill_replace_cur_sel_skill and btn_replace.replace or btn_replace.add
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    self.GridSkill:SetData(data.cur_skill_list_sort)
    self.GridSkill:ClickItem(self.data.skill_replace_cur_sel_skill or 1)
end

PartnerViewSkillReplaceDown = PartnerViewSkillReplaceDown or DeclareMono("PartnerViewSkillReplaceDown", UIWFlushPanel)

function PartnerViewSkillReplaceDown:PartnerViewSkillReplaceDown()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.SkillReplace
end

function PartnerViewSkillReplaceDown:onFlush()
end

function PartnerViewSkillReplaceDown:OnClickReplace()
    if not self.skill_info then return end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ZhuangBeiFuShuJian)
    PartnerCtrl.Instance:SendPartnerSaveSkill{index = self.data.cur_sel_skill, skill_id = self.skill_info.skill_id}
    ViewMgr:CloseView(PartnerViewSkillReplace)
end

function PartnerViewSkillReplaceDown:OnClickLock()
    PartnerCtrl.Instance:SendPartnerSaveSkill{index = self.data.cur_sel_skill, skill_id = -1}
    ViewMgr:CloseView(PartnerViewSkillReplace)
end

function PartnerViewSkillReplaceDown:OnClickItem(data)
    self.skill_info = data.info
    local value = self.skill_info.cur_level < self.skill_info.need_level
    self.SkillDesc.text = self.skill_info.desc
    self.LockRequire.text = string.format(self.language.need, DataHelper.GetDaXie(self.skill_info.need_level))
    self.Require:SetActive(value)
    self.BtnReplaceInter.Interactable = not ((self.skill_info.cur_level < self.skill_info.need_level) or data.is_using)
    self.ConsumeMp.text = value and "" or string.format(self.language.consume_mp, self.skill_info.consume_mp)
end

PartnerViewSkillReplaceSkillItem = DeclareMono("PartnerViewSkillReplaceSkillItem", UIWidgetBaseItem)

function PartnerViewSkillReplaceSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local info = data.info
    self.Name.text = info.name
    self.Cell:SetData(info)
    self.Level.text = info.skill_level
    self.Using:SetActive(data.is_using)
    self.Mask:SetActive((info.cur_level < info.need_level) or data.is_using)
    self.Lock:SetActive(info.cur_level < info.need_level)
end

function PartnerViewSkillReplaceSkillItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end
