
PartnerViewUpPreview = PartnerViewUpPreview or DeclareView("PartnerViewUpPreview", PartnerConfig.ResPath .. "partner_up_preview")

function PartnerViewUpPreview:PartnerViewUpPreview()
	self.language = Language.Partner
end

function PartnerViewUpPreview:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.UpPreview, Vector2.New(890, 520))
end

PartnerViewUpPreviewGift = PartnerViewUpPreviewGift or DeclareMono("PartnerViewUpPreviewGift", UIWFlushPanel)

function PartnerViewUpPreviewGift:PartnerViewUpPreviewGift()
    self.data = PartnerCtrl.Instance:Data()
end

function PartnerViewUpPreviewGift:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local cur_up_info = data.cur_up_info
    local gift_info = PartnerCtrl.Instance:Data():GetPartnerGiftWithIdAndLevel(cur_up_info.skill_gift, cur_up_info.skill_gift_level)
    
    self.Name.text = gift_info.name
    self.LevelCur.text = "Lv." .. gift_info.gift_level
    self.LevelNext.text = "Lv." .. (gift_info.gift_level + 1)
    self.Cell:SetData(gift_info)
end

PartnerViewUpPreviewSkill = PartnerViewUpPreviewSkill or DeclareMono("PartnerViewUpPreviewSkill", UIWFlushPanel)

function PartnerViewUpPreviewSkill:PartnerViewUpPreviewSkill()
    self.data = PartnerCtrl.Instance:Data()
end

function PartnerViewUpPreviewSkill:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local cur_up_info = data.cur_up_info

    self.LevelCur.text = "Lv." .. cur_up_info.skill_level
    self.LevelNext.text = "Lv." .. (cur_up_info.skill_level + 1)
end

PartnerViewUpPreviewAttr = PartnerViewUpPreviewAttr or DeclareMono("PartnerViewUpPreviewAttr", UIWFlushPanel)

function PartnerViewUpPreviewAttr:PartnerViewUpPreviewAttr()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.PreviewSucc
end

function PartnerViewUpPreviewAttr:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local attr_list = data.vo.attr_list
    local cur_up_info = data.cur_up_info
    local attr_name = self.language.attr_name
    local attr_type = self.language.att_type
    local convert_common = PartnerConfig.PartnerAttrConvertCommon
    local convert_cfg = PartnerConfig.PartnerAttrConvertCfg
    for i = 1, PartnerConfig.UP_STAGE_MAX do
        self.Names[i].text = string.format(attr_name, attr_type[i], attr_list[convert_common[i]])
        self.Values[i].text = (attr_list[convert_common[i]] + cur_up_info[convert_cfg[i]]) .. "  (+" .. cur_up_info[convert_cfg[i]] .. ")"
    end
end

PartnerViewUpPreviewCondition = PartnerViewUpPreviewCondition or DeclareMono("PartnerViewUpPreviewCondition", UIWFlushPanel)

function PartnerViewUpPreviewCondition:PartnerViewUpPreviewCondition()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.PreviewSucc
end

function PartnerViewUpPreviewCondition:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local vo = data.vo
    local task_finish = vo.task_finish
    local attr_stage = vo.attr_stage
    local cur_up_info = data.cur_up_info
    local role_level = RoleData.Instance:GetBaseData().level

    self.Values[1].text = attr_stage .. "/" .. PartnerConfig.UP_STAGE_MAX
    self.Values[2].text = role_level .. "/" .. cur_up_info.need_rolelevel
    self.Values[3].text = task_finish .. "/1"

    local flags = {
        attr_stage == PartnerConfig.UP_STAGE_MAX,
        role_level >= cur_up_info.need_rolelevel,
        task_finish == 1,
    }

    for i = 1, PartnerConfig.UPPreView_Condition_MAX do
        self.ListYes[i]:SetActive(flags[i])
        self.ListNo[i]:SetActive(not flags[i])
        self.BtnGoInters[i].Interactable = not flags[i]
        self.BtnGoNames[i].text = flags[i] and self.language.btn_go_state[2] or self.language.btn_go_state[1]
    end
end

function PartnerViewUpPreviewCondition:OnClickGo(type)
    local preview_condition = PartnerConfig.PreviewCondition
    if type == preview_condition.Attr then
        -- Debuger.LogError("Go Attr")
    elseif type == preview_condition.Level then
        -- Debuger.LogError("Go Level")
    elseif type == preview_condition.Task then
        -- Debuger.LogError("Go Task")
    end
end