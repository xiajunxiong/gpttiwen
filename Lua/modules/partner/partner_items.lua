
PartnerItem = PartnerItem or DeclareMono("PartnerItem")

function PartnerItem:PartnerItem()
    self.partner_data = PartnerCtrl.Instance:Data()
end

PartnerItemFetter = PartnerItemFetter or DeclareMono("PartnerItemFetter", PartnerItem)

function PartnerItemFetter:SetData(info)
    self.info = info
    self.Fetter:SetActive(not info)
end

function PartnerItemFetter:OnClickItem()
    if not self.info then return end
	self.partner_data.show_fetter_cell = self.info
	ViewMgr:OpenView(PartnerFetterInfoView)
end

PartnerItemSkill = PartnerItemSkill or DeclareMono("PartnerItemSkill", PartnerItem)

function PartnerItemSkill:PartnerItemSkill()
    self.language = Language.Partner.SkillInfo
end

function PartnerItemSkill:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = data.info or data
    UH.SetIcon(self.Icon, info.icon_id or info.icon, ICON_TYPE.SKILL)
    self.CornerObj:SetActive(nil ~= info.corner_icon)
    self.CornerVal.text = info.corner_icon or ""
    local force_unlock = PartnerData.Instance.force_unlock
    if force_unlock == true then
        self.ItemInter.Interactable = true
        if self.MaskObj then
            self.MaskObj:SetActive(false)
        end
    end
end

function PartnerItemSkill:OnClickItem()
end

PartnerItemSkill2 = PartnerItemSkill2 or DeclareMono("PartnerItemSkill2", PartnerItem)

function PartnerItemSkill2:PartnerItemSkill2()
    self.language = Language.Partner.SkillInfo
end

function PartnerItemSkill2:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = data.info or data
    self.LockObj:SetActive(data.is_lock)
    self.ItemInter.Interactable = not data.is_lock and data.is_equip
    self.SkillLevel.text = info.skill_level and string.format(self.language.skill_level, info.skill_level) or ""
    self.SkillLevelF.text = info.level and string.format(self.language.skill_level_f, info.level) or ""
    UH.SetIcon(self.Icon, info.icon_id or info.icon, ICON_TYPE.SKILL)
    self.CornerObj:SetActive(nil ~= info.corner_icon)
    self.CornerVal.text = info.corner_icon or ""
end

function PartnerItemSkill2:OnClickItem()
    self.partner_data.cur_skill_info = self.data
    ViewMgr:OpenView(PartnerSkillInfoView)
end

PatnerViewAttrConsumeItem = PatnerViewAttrConsumeItem or DeclareMono("PatnerViewAttrConsumeItem", PartnerItem)

function PatnerViewAttrConsumeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_id = data.item_id
    local flag = data:Num() < data.cost_num
    self.Num.text = RichTextHelper.ColorStr(data:Num(), flag and COLORSTR.Red7 or COLORSTR.Green6) .. "/" .. data.cost_num
    self.Cell:SetData(data)
    self.flag = flag
end

function PatnerViewAttrConsumeItem:GetItem()
    return self.flag and self.data or nil
end

PatnerViewUpConsumeItem = PatnerViewUpConsumeItem or DeclareMono("PatnerViewUpConsumeItem", PartnerItem)

function PatnerViewUpConsumeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item_id = data.item_id
    local flag = data:Num() < data.cost_num
    self.Name.text = data:QuaName(true)
    self.Num.text = RichTextHelper.ColorStr(data:Num(), flag and COLORSTR.Red7 or COLORSTR.Green6) .. "/" .. data.cost_num
    self.Cell:SetData(data)
    self.flag = flag
end

function PatnerViewUpConsumeItem:GetItem()
    return self.flag and self.data or nil
end

PartnerViewLockSkillItem = PartnerViewLockSkillItem or DeclareMono("PartnerViewLockSkillItem", PartnerItem)

function PartnerViewLockSkillItem:SetData(data)
    local info = data.info
    self.SkillCell:SetData(data)
    self.Name.text = info.name
    self.Desc.text = info.desc
    -- self.XinObj:SetActive(info.cur_level == info.need_level)
    -- self.SkillMask:SetActive(info.cur_level < info.need_level)
    -- self.SkillInter.Interactable = info.cur_level >= info.need_level
    local force_unlock = PartnerData.Instance.force_unlock
    if force_unlock == true then
        self.XinObj:SetActive(false)
        self.SkillMask:SetActive(false)
        self.SkillInter.Interactable = true
    else
        self.XinObj:SetActive(not data.is_lock)
        self.SkillMask:SetActive(data.is_lock)
        self.SkillInter.Interactable = not data.is_lock
    end
end