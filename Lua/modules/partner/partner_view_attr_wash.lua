
PartnerViewAttrWash = PartnerViewAttrWash or DeclareView("PartnerViewAttrWash", PartnerConfig.ResPath .. "partner_attr_wash")

function PartnerViewAttrWash:PartnerViewAttrWash()
    self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner
end

function PartnerViewAttrWash:LoadCallback()
    self.Board:SetData(self:GetType(), self.language.AttrWash, Vector2.New(905, 531))
end

PartnerViewAttrWashLeft = PartnerViewAttrWashLeft or DeclareMono("PartnerViewAttrWashLeft", UIWFlushPanel)

function PartnerViewAttrWashLeft:PartnerViewAttrWashLeft()
    self.flush_events = {EventProtocol.ToEventKey(SCPartnerFetter)}

    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Fetter
end

function PartnerViewAttrWashLeft:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local fetter_cur = data.fetter_cur
    local desc_name = self.language.desc_name

    for i = 1, PartnerConfig.PARTNER_FETTER_MAX do
        if fetter_cur[i] then
            self.Names[i].text = fetter_cur[i].name
            self.Descs[i].text = string.format(desc_name, QualityColorStr[fetter_cur[i].color], fetter_cur[i].desc)
        else
            self.Names[i].text = self.language.empty
            self.Descs[i].text = ""
        end
	end
end

PartnerViewAttrWashRight = PartnerViewAttrWashRight or DeclareMono("PartnerViewAttrWashRight", UIWFlushPanel)

function PartnerViewAttrWashRight:PartnerViewAttrWashRight()
    self.flush_events = {EventProtocol.ToEventKey(SCPartnerFetter)}

    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Fetter
end

function PartnerViewAttrWashRight:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local fetter_wash = data.fetter_wash
    local desc_name = self.language.desc_name

    local flag = false
    for i = 1, PartnerConfig.PARTNER_FETTER_MAX do
        if fetter_wash[i] then
            flag = true
            self.Items[i]:SetActive(true)
            self.Names[i].text = fetter_wash[i].name
            self.Descs[i].text = string.format(desc_name, QualityColorStr[fetter_wash[i].color], fetter_wash[i].desc)
        else
            self.Items[i]:SetActive(false)
        end
    end
    self.BtnReplaceInter.Interactable = flag
end

PartnerViewAttrWashDown = PartnerViewAttrWashDown or DeclareMono("PartnerViewAttrWashDown", UIWFlushPanel)

function PartnerViewAttrWashDown:PartnerViewAttrWashDown()
    self.flush_events = {EventProtocol.ToEventKey(SCPartnerFetter)}

    self.data = PartnerCtrl.Instance:Data()
end

function PartnerViewAttrWashDown:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local info = data.info
    local refresh = info.other.refresh
    self.Consume.text = refresh
end

function PartnerViewAttrWashDown:OnClickAdd()
    -- Debuger.LogError("OnClickAdd")
end

function PartnerViewAttrWashDown:OnClickInfo(value)
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local fetter_wash = data.fetter_wash

    local flag = false
    for i = 1, PartnerConfig.PARTNER_FETTER_MAX do
        if fetter_wash[i] then flag = true end
    end
    if not flag then
        PublicPopupCtrl.Instance:Center(Language.Partner.UnWashTips)
        return
    end
     self.Info:SetActive(value)
end

function PartnerViewAttrWashDown:OnClickWash()
    PartnerCtrl.Instance:SendPartnerWash(self.data.cur_sel_partner)
end

function PartnerViewAttrWashDown:OnClickWashSave()
    PartnerCtrl.Instance:SendPartnerWashSave(self.data.cur_sel_partner)
end


PartnerViewAttrWashInfo = PartnerViewAttrWashInfo or DeclareMono("PartnerViewAttrWashInfo", UIWFlushPanel)

function PartnerViewAttrWashInfo:PartnerViewAttrWashInfo()
    self.flush_events = {EventProtocol.ToEventKey(SCPartnerFetter)}
    
    self.data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.Fetter
end

function PartnerViewAttrWashInfo:onFlush()
    local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
    local name = data.info.name
    local fetter_wash = data.fetter_wash
    self.Name.text = string.format(self.language.title_name, name)
    if fetter_wash == nil then return end
    self.ShowList:SetData(fetter_wash)
end

PartnerViewAttrWashInfoItem = DeclareMono("PartnerViewAttrWashInfoItem", UIWidgetBaseItem)

function PartnerViewAttrWashInfoItem:SetData(data)
    local fetter_wash_type = data.fetter_wash_type

    local data = PartnerCtrl.Instance:Data():GetPartnerInfoById(data.need_partner)
    local info = data.info
    local language = Language.Partner.Fetter

    self.Name.text = info.name
    
    for i = 1, #fetter_wash_type do
        self.Items[i].text =  string.format(language.desc_name_type, QualityColorStr[fetter_wash_type[i].color], fetter_wash_type[i].name, ":" .. fetter_wash_type[i].desc)
    end
end