

PartnerLifeHandbookView = PartnerLifeHandbookView or DeclareView("PartnerLifeHandbookView", PartnerConfig.ResPath .. "partner_life_handbook")
-- VIEW_DECLARE_LEVEL(PartnerView,ViewLevel.L1)

function PartnerLifeHandbookView:PartnerLifeHandbookView()
	self.language = Language.Partner
end

function PartnerLifeHandbookView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(), self.language.PartnerLifeHandbook, Vector2.New(1073, 675))
end

function PartnerLifeHandbookView:CloseCallback()
end

PartnerLifeHandbookViewF = PartnerLifeHandbookViewF or DeclareMono("PartnerLifeHandbookViewF", UIWFlushPanel)

function PartnerLifeHandbookViewF:PartnerLifeHandbookViewF()
	self.data = PartnerCtrl.Instance:Data()
	self.data:ResetLifeHandbookInfo()

	self.data_cares = {
		{data = self.data.partner_life_handbook_info, func = self.FlushLevelList, keys = {"prof"}, init_call = false},
		{data = self.data.partner_life_handbook_info, func = self.FlushSuitList, keys = {"level"}, init_call = false},
    }
    PartnerLifeHandbookViewF.AutoLevel = true
end

function PartnerLifeHandbookViewF:Awake()
    UIWFlushPanel.Awake(self)

    self:FlushProfList()
end

function PartnerLifeHandbookViewF:FlushProfList()
	local prof_list, prof_sel = self.data:GetPartnerLifeHandbookProfListSort()
	self.ProfList:SetData(prof_list)
	self.ProfList:ClickItemData(prof_sel)
end

function PartnerLifeHandbookViewF:FlushLevelList()
	local level_list, level_sel = self.data:GetPartnerLifeHandbookLevelListSort()
	self.LevelList:SetData(level_list)
	self.LevelList:ClickItemData(level_sel)
    self.data.partner_life_handbook_info.level:Set(level_sel)
end

function PartnerLifeHandbookViewF:FlushSuitList()
    local suit_list = self.data:GetPartnerLifeHandbookSuitListSort()
	self.SuitList:SetData(suit_list)
end


PartnerLifeHandbookViewProfItem = DeclareMono("PartnerLifeHandbookViewProfItem", UIWidgetBaseItem)

function PartnerLifeHandbookViewProfItem:PartnerLifeHandbookViewProfItem()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.LifeHandbook
end

function PartnerLifeHandbookViewProfItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    
	self.ProfName.text = Language.Common.Prof[self.data.prof]
end

function PartnerLifeHandbookViewProfItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function PartnerLifeHandbookViewProfItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
		self.partner_data.partner_life_handbook_info.prof:Set(self.data)
    end
end


PartnerLifeHandbookViewLevelItem = DeclareMono("PartnerLifeHandbookViewLevelItem", UIWidgetBaseItem)

function PartnerLifeHandbookViewLevelItem:PartnerLifeHandbookViewLevelItem()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.LifeHandbook
end

function PartnerLifeHandbookViewLevelItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.LevelDesc.text = string.format(self.language.LevelDesc, data.level)
end

function PartnerLifeHandbookViewLevelItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function PartnerLifeHandbookViewLevelItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
		self.partner_data.partner_life_handbook_info.level:Set(self.data)
    end
end

PartnerLifeHandbookViewShowItem = DeclareMono("PartnerLifeHandbookViewShowItem", UIWidgetBaseItem)

function PartnerLifeHandbookViewShowItem:PartnerLifeHandbookViewShowItem()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.LifeHandbook
end

function PartnerLifeHandbookViewShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    self.DecoSp.SpriteName = PartnerConfig.LifeHandbookDecoSp[data.suit_color]
    self.Desc.text = string.format(self.language.SuitDesc, data.name, data.desc)
    self.Qua.text = self.language.SuitQua[data.suit_color]
    for i = 1, self.Cards:Length() do
        self.Cards[i]:SetData(data, i - 1)
    end
end

PartnerLifeHandbookViewCardItem = DeclareMono("PartnerLifeHandbookViewCardItem", UIWidgetBaseItem)

function PartnerLifeHandbookViewCardItem:PartnerLifeHandbookViewCardItem()
    self.partner_data = PartnerCtrl.Instance:Data()
    self.language = Language.Partner.LifeHandbook
end

function PartnerLifeHandbookViewCardItem:SetData(data, index)
	UIWidgetBaseItem.SetData(self, data)

    self.ItemSp.SpriteName = PartnerConfig.LifeHandbookDiSp[data.suit_color]
    self.BgSp.SpriteName = PartnerConfig.LifeHandbookBTSp[data.suit_color]

    local card = self.partner_data:GetPartnerLifeHandbookCardInfo(data, index)
    local item = card.show_item
    self.Name.text = RichTextHelper.ColorStr(item:Name(), PartnerConfig.LifeHandbookCardColorName[data.suit_color])
    self.Cell:SetData(item)
    for i = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
        local attr = card.attr_list[i] or {}
        self.Attrs[i]:SetObjActive(attr.attr_value and 0 ~= attr.attr_value)
        self.Attrs[i].text = string.format(self.language.AttrShow, Language.Common.AttrList[attr.attr_type], DataHelper.IsPercent(attr.attr_type) and Percent(attr.attr_value) or attr.attr_value)
    end
end