SpiritResonanceView = SpiritResonanceView or DeclareView("SpiritResonanceView", "equip_view/spirit_resonance")
VIEW_DECLARE_LEVEL(SpiritResonanceView, ViewLevel.Ls)
VIEW_DECLARE_MASK(SpiritResonanceView, ViewMask.BgBlock)

function SpiritResonanceView:SpiritResonanceView()
    if TableIsEmpty(EquipData.Instance.now_select_equip_data) then
        -- 按道理来说是不会走到这块逻辑的
        PublicPopupCtrl.Instance:Center(Language.Equip.NoSelectEquipTip)
        ViewMgr:CloseView(SpiritResonanceView)
        return
    end
    self.Board:SetData(self:GetType(), Format(Language.Equip.EquipResonanceTitle, EquipData.Instance.now_select_equip_data:TypeName()), Vector2.New(735, 555))
end

----------------------------SpiritResonancePanel----------------------------
SpiritResonancePanel = SpiritResonancePanel or DeclareMono("SpiritResonancePanel", UIWFlushPanel)
function SpiritResonancePanel:SpiritResonancePanel()
    self.Data = EquipData.Instance
    self.data = self.Data.equip_reasonance_data
    self.language = Language.Equip
    if TableIsEmpty(self.Data.now_select_equip_data) then
        -- 按道理来说是不会走到这块逻辑的
        PublicPopupCtrl.Instance:Center(self.language.NoSelectEquipTip)
        ViewMgr:CloseView(SpiritResonanceView)
        return
    end
    self.equip_type = self.Data.now_select_equip_data:EquipType()
    if self.Data.now_combine_data then
        self.equip_combine = self.Data.now_combine_data.combine_attr.resonance_group_id
        self.equip_combine_level = self.Data.now_combine_data.combine_attr.resonance_level
    end
    self.data_cares = {
        {data = self.data, func = self.FlushByTop, keys = {"now_top_data"}, init_call = true},
        {data = self.data, func = self.FlushByLeft, keys = {"now_left_data"}, init_call = true},
    }
    self.now_show_level = 0
    self.TopBtnList:SetData(self:GetTopBtnData())
    self.TopBtnList:SetectItem(1, true)
end

function SpiritResonancePanel:Awake()
    UIWFlushPanel.Awake(self)
end

function SpiritResonancePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Data.equip_reasonance_data:Set({now_top_data = {}, now_left_data = {}})
end

function SpiritResonancePanel:GetTopBtnData()
    local tab = {}
    for i = 1, EQUIP_SPIRIT_MAX do
        tab[i] = {}
        tab[i].title = Language.Equip.SlotTitle[i]
        tab[i].config = {}
        if i == 1 then
            for k, v in pairs(Config.equip_spirit_auto.resonance) do
                table.insert(tab[i].config, v)
            end
            table.sort(tab[i].config, function(a_data, b_data)
                return (a_data.need_num * 100 + a_data.sort) < (b_data.need_num * 100 + b_data.sort)
            end)
        else
            tab[i].config = self.Data:GetSpiritCombineData(i)
        end
    end
    return tab
end

function SpiritResonancePanel:OnLevelShowChange(param)
    self:FlushRight(param)
end

function SpiritResonancePanel:FlushByTop()
    self.LeftBtnList:SetData({})
    self.LeftBtnList:SetData(self.data.now_top_data)
    if self.Data.now_combine_data then
        for i, v in ipairs(self.data.now_top_data) do
            if v.resonance_group_id == self.equip_combine then
                self.LeftBtnList:SetectItem(i, true)
                if i > 5 then
                    -- self.LeftBtnList:AutoSlide(v, false)
                    -- self.LeftBtnList:JumpVerticalIndex(i, 77)
                    self.LeftBtnList:SetCompleteCallBack(function()
                        self.LeftBtnList:JumpVerticalIndex(i, 77)
                    end)
                else
                    self.LeftBtnList:SetCompleteCallBack()
                end
                return
            end
        end
    end
    self.LeftBtnList:SetectItem(1, true)
end

function SpiritResonancePanel:FlushByLeft()
    self:FlushRight()
end

function SpiritResonancePanel:FlushRight(change_param)
    local spirit_combine_config = self.Data:GetSpiritCombineAttrData(self.equip_type, self.data.now_left_data.resonance_group_id)
    local max_level = table.nums(spirit_combine_config) - 1
    local min_level = 0
    if not change_param then
        if self.equip_combine then
            if self.equip_combine == self.data.now_left_data.resonance_group_id then
                self.now_show_level = self.equip_combine_level
            else
                self.now_show_level = max_level
            end
        else
            self.now_show_level = max_level
        end
    else
        self.now_show_level = self.now_show_level + change_param
        if self.now_show_level < min_level then
            self.now_show_level = min_level
        elseif self.now_show_level > max_level then
            self.now_show_level = max_level
        end
    end
    self.ObjLArrow:SetActive(self.now_show_level ~= min_level)
    self.ObjRArrow:SetActive(self.now_show_level ~= max_level)
    UH.SetText(self.TxtLevel, Format(self.language.EquipStoneLevel2, self.now_show_level + 1))
    UH.SetText(self.TxtResonanceName, self.data.now_left_data.group_name)
    local spirit_combine_attr = self.Data:SpiritCombineAttr(self.equip_type, self.data.now_left_data.resonance_group_id, self.now_show_level)
    self.Data:TotalSpiritCombineAttr(spirit_combine_attr)
    local spirit_combine_attr_data = self.Data:DealCombineAttrToTxt(spirit_combine_attr, self.language.SingleAttr[3], self.language.CombineAttr[2])
    self.ResonanceAttrList:SetData(spirit_combine_attr_data)
end

--------------SpiritResonanceTopBtn--------------
SpiritResonanceTopBtn = SpiritResonanceTopBtn or DeclareMono("SpiritResonanceTopBtn", UIWidgetBaseItem)
function SpiritResonanceTopBtn:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtTitle, data.title)
end

function SpiritResonanceTopBtn:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        EquipData.Instance.equip_reasonance_data.now_top_data = self.data.config
    end
end

--------------SpiritResonanceLeftBtn--------------
SpiritResonanceLeftBtn = SpiritResonanceLeftBtn or DeclareMono("SpiritResonanceLeftBtn", UIWidgetBaseItem)
function SpiritResonanceLeftBtn:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.group_name)
    local equip_combine = -1
    if EquipData.Instance.now_combine_data then
        equip_combine = EquipData.Instance.now_combine_data.combine_attr.resonance_group_id
    end
    self.ObjNow:SetActive(equip_combine == data.resonance_group_id)
    self.CircleList:SetData(EquipData.Instance:GetColorListByCombine(data, true))
end

function SpiritResonanceLeftBtn:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        EquipData.Instance.equip_reasonance_data.now_left_data = self.data
    end
end