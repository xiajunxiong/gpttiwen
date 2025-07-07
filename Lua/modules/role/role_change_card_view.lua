RoleChangeCardView = RoleChangeCardView or DeclareView("RoleChangeCardView", "role/role_change_card", Mod.ChangeCard.Main)
VIEW_DECLARE_LEVEL(RoleChangeCardView, ViewLevel.L1)
function RoleChangeCardView:RoleChangeCardView()
    self.Board:SetData(self:GetType(),Language.LiveCard.title, Vector2.New(1241, 646))
end

function RoleChangeCardView:OnHelpClick()
    local seq = Config.life_system_auto.other[1].change_dec or 150
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[seq].desc)
end

RoleChangeCardPanel = RoleChangeCardPanel or DeclareMono("RoleChangeCardPanel", UIWFlushPanel)
function RoleChangeCardPanel:RoleChangeCardPanel()
    self.data = LiveData.Instance
    self.data_cares = {
        {data = RoleData.Instance:GetBaseData(), func = self.FlushPanel, init_call = false, keys = {"appearance"}},
        {data = self.data.card_use_all_info, func = self.FlushPanel, init_call = false},
    }

    GuideManager.Instance:RegisterGetGuideUi("RoleChangeCardBtn",function ()
        return self.Btn,function ()
            self.ModelItem:OnChangeClick()
        end
    end)
end

function RoleChangeCardPanel:Awake()
    UIWFlushPanel.Awake(self)
    local type_list = self.data:GetCardTypeList()
    self.TypeList:SetData(type_list or {})
    local sel_type_index = 1
    local used_seq = self.data.GetRoleUsedCardSeq()
    if used_seq > 0 then
        local index,card_index = self.data:GetCardSeqPage(used_seq)
        if index and card_index then
            sel_type_index = index
            self.card_index = card_index
        end
    end
    self.TypeList:SetectItem(sel_type_index,true)
    self.BattleToggle.isOn = self.data.battled_unfix_card == true
    self:FlushPanel()
end

function RoleChangeCardPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.battled_unfix_card = self.BattleToggle.isOn
end

function RoleChangeCardPanel:FlushPanel()
    self:FlushCard()
    self:FlushModel()
    self:FlushToggle()
end

function RoleChangeCardPanel:FlushToggle()
    local value = self.data:RoleIsShowModel()
    self.UseFlag:SetActive(not value)
end

function RoleChangeCardPanel:FlushCard()
    if not self.type_data then
        return
    end
    local card_list = self.data:GetCardList(self.type_data.page_sn)
    self.CardList:SetData(card_list)
    if self.card_index then
        self.CardList:SetectItem(self.card_index,true)
        self.card_index = nil
    end
end

function RoleChangeCardPanel:FlushModel()
    if not self.card_data then
        return
    end
    self.ModelItem:SetData(self.card_data)
end

function RoleChangeCardPanel:OnTypeClick(data)
    self.type_data = data
    if not self.card_index then
        self.card_index = 1
    end
    self.CardList:SetData({})
    self:FlushCard()
end

function RoleChangeCardPanel:OnCardClick(data)
    self.card_data = data
    self:FlushModel()
end

function RoleChangeCardPanel:OnToggleClick()
    local value = self.data:RoleIsShowModel()
    LiveCtrl.Instance:UseCardModel(not value)
end

RoleChangeCardItem = RoleChangeCardItem or DeclareMono("RoleChangeCardItem", UIWidgetBaseItem)
function RoleChangeCardItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = Item.Create({item_id = data.item})
    self.ItemCell:SetData(item)
    UH.SetText(self.Name, data.name)
    local point_num = LiveData.Instance:GetCardPoint(data.seq)
    UH.SetText(self.Num, point_num)
    self.gameObject.name = string.format("CardItem (%s)", data.index or 0)
end

RoleChangeCardModelItem = RoleChangeCardModelItem or DeclareMono("RoleChangeCardModelItem", UIWidgetBaseItem)
function RoleChangeCardModelItem:RoleChangeCardModelItem()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
end

function RoleChangeCardModelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.name)
    UH.SetText(self.Tip, string.format(Language.LiveCard.Txt1,ColorStr(data.expend_point, COLORSTR.Yellow11)))
    local has_point = LiveData.Instance:GetCardPoint(data.seq)
    UH.SetText(self.Num,has_point)
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(data.shift_card_res))
    self.ui_obj:PlayAnim(ChAnimType.Show)

    local attri_list = LiveData.Instance:GetAttriListByCardSeq(data.seq)
    self.AttriList:SetData(attri_list)

    local btn_text = RoleData.Instance:GetApearance().model_card_seq == data.seq and Language.LiveCard.Txt2 or Language.LiveCard.Txt3
    UH.SetText(self.BtnText, btn_text)

    local skill_id = data.skill_id or 0
    self.EmptyState:SetActive(skill_id == 0)
    self.SkillInfo:SetActive(skill_id ~= 0)
    if skill_id ~= 0 then
        local skill_desc_cfg = SkillData.GetDecConfig(skill_id)
        if skill_desc_cfg then
            UH.SetIcon(self.SkillIcon, skill_desc_cfg.icon_id, ICON_TYPE.SKILL)
            UH.SetText(self.SkillDesc, skill_desc_cfg.description)
            UH.SetText(self.SkillName, skill_desc_cfg.skill_name)
        end
        --LogError("SkillDesc", skill_id, skill_desc)
    end
end

function RoleChangeCardModelItem:OnAddClick()
    if Item.GetNum(self.data.item) == 0 then
        MainOtherCtrl.Instance:GetWayViewById(self.data.item)
    else
        LiveData.Instance:SetPropUseInfo(self.data.item)
        ViewMgr:OpenView(PropItemUseDiaLog)
    end
end

function RoleChangeCardModelItem:OnChangeClick()
    if RoleData.Instance:GetApearance().model_card_seq == self.data.seq then
        -- 解除幻化
        LiveCtrl.Instance:SendSkillReq(LiveCtrl.LIVE_SKILL_OP_TYPE.UN_CHANGE_ROLE)
    else
        local has_point = LiveData.Instance:GetCardPoint(self.data.seq)
        local need_point = self.data.expend_point
        if has_point >= need_point then
            LiveCtrl.Instance:SendSkillReq(LiveCtrl.LIVE_SKILL_OP_TYPE.CHANGE_ROLE, self.data.seq)
        else
            self:OnAddClick()
        end
    end
end

function RoleChangeCardModelItem:OnDestroy()
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
end

RoleChangeCardTypeItem = RoleChangeCardTypeItem or DeclareMono("RoleChangeCardTypeItem", UIWidgetBaseItem)
function RoleChangeCardTypeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Text, data.page_name)
    self.gameObject.name = string.format("TypeItem (%s)", data.page_sn)
end

RoleChangeCardAttriItem = RoleChangeCardAttriItem or DeclareMono("RoleChangeCardAttriItem", UIWidgetBaseItem)
function RoleChangeCardAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    local value_str
    if data.attri_value > 0 then
        value_str = ColorStr(string.format("+%s", info.attr_percent_value), COLORSTR.Green4)
    else
        value_str = ColorStr(string.format("-%s", info.attr_percent_value), COLORSTR.Red8)
    end
    UH.SetText(self.AttriName, info.attr_name)
    UH.SetText(self.AttriValue, value_str)
end