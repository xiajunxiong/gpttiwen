LingBaoResonanceView = LingBaoResonanceView or DeclareView("LingBaoResonanceView", "ling_bao/ling_bao_resonance")
VIEW_DECLARE_MASK(LingBaoResonanceView, ViewMask.BgBlockClose)
function LingBaoResonanceView:LingBaoResonanceView()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function LingBaoResonanceView:LoadCallback(param_t)
    self.shenghen_type = param_t.shenghen_type
    local cfg = self.Data:ShengHenSort(self.shenghen_type)
    local resonance_info = self.Data:GetResonanceInfo(self.shenghen_type)
    local list_data = {}
    for i, v in ipairs(LingBaoConfig.Suit) do
        local t = {}
        t.suit_num = v
        t.suit_info = resonance_info[i] or {}
        table.insert(list_data, t)
    end
    self.List:SetData(list_data)
    UH.SetText(self.TxtDesc, Format(cfg.resonante_desc, self.Data:GetResonanceAddValue(self.shenghen_type)))
end

function LingBaoResonanceView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[LingBaoData.Instance:ResonanceDesc()].desc)
end


--------------LingBaoResonanceCenterItem--------------
LingBaoResonanceCenterItem = LingBaoResonanceCenterItem or DeclareMono("LingBaoResonanceCenterItem", UIWidgetBaseItem)
function LingBaoResonanceCenterItem:LingBaoResonanceCenterItem()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function LingBaoResonanceCenterItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local title = ""
    local da_xie = DataHelper.GetDaXie(data.suit_num)
    if not TableIsEmpty(data.suit_info) then
        title = Format(self.language.ResonanceTitle[1], DataHelper.GetDaXie(data.suit_info.class_level), data.suit_info.star_level, da_xie)
        self.AttrList:SetObjActive(true)
        self.AttrList:SetData(self.Data:GetAttrList(data.suit_info))
    else
        title = Format(self.language.ResonanceTitle[2], da_xie)
        self.AttrList:SetObjActive(false)
    end
    UH.SetText(self.TxtTitle, title)
end

--------------LingBaoAttrItem--------------
LingBaoAttrItem = LingBaoAttrItem or DeclareMono("LingBaoAttrItem", UIWidgetBaseItem)
function LingBaoAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.attr_name)
    UH.SetText(self.TxtValue, data.attr_percent_value)
end