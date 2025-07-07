
PartnerViewUpSuccess = PartnerViewUpSuccess or DeclareView("PartnerViewUpSuccess", PartnerConfig.ResPath .. "partner_up_success")

function PartnerViewUpSuccess:PartnerViewUpSuccess()
	self.language = Language.Partner
end

function PartnerViewUpSuccess:OnClickClose()
    ViewMgr:CloseView(PartnerViewUpSuccess)
end

PartnerViewUpSuccessInfo = PartnerViewUpSuccessInfo or DeclareMono("PartnerViewUpSuccessInfo", UIWFlushPanel)

function PartnerViewUpSuccessInfo:PartnerViewUpSuccessInfo()
    self.data = PartnerCtrl.Instance:Data()
end

function PartnerViewUpSuccessInfo:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    self.Name.text = info.name
end

PartnerViewUpSuccessAttr = PartnerViewUpSuccessAttr or DeclareMono("PartnerViewUpSuccessAttr", UIWFlushPanel)

function PartnerViewUpSuccessAttr:PartnerViewUpSuccessAttr()
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.PreviewSucc
end

function PartnerViewUpSuccessAttr:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local attr_list = data.vo.attr_list
    local pre_up_info = data.pre_up_info
    local attr_type = self.language.att_type
    local convert_common = PartnerConfig.PartnerAttrConvertCommon
    local convert_cfg = PartnerConfig.PartnerAttrConvertCfg
    for i = 1, PartnerConfig.UP_STAGE_MAX do
        self.Names[i].text = attr_type[i] .. "  " .. attr_list[convert_common[i]]
        self.Values[i].text = (attr_list[convert_common[i]] - pre_up_info[convert_cfg[i]]) .. "  (+" .. pre_up_info[convert_cfg[i]] .. ")"
    end
end

PartnerViewUpSuccessSkill = PartnerViewUpSuccessSkill or DeclareMono("PartnerViewUpSuccessSkill", UIWFlushPanel)

function PartnerViewUpSuccessSkill:PartnerViewUpSuccessSkill()
    self.data = PartnerCtrl.Instance:Data()
end

function PartnerViewUpSuccessSkill:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local cur_up_info = data.cur_up_info
    local gift_info = PartnerCtrl.Instance:Data():GetPartnerGiftWithIdAndLevel(cur_up_info.skill_gift, cur_up_info.skill_gift_level)

    self.NameGift.text = gift_info.name
    self.LevelCurGift.text = "Lv." .. (gift_info.gift_level - 1)
    self.LevelNextGift.text = "Lv." .. gift_info.gift_level
    self.Cell:SetData(gift_info)

    self.LevelCurSkill.text = "Lv." .. (cur_up_info.skill_level - 1)
    self.LevelNextSkill.text = "Lv." .. cur_up_info.skill_level
end