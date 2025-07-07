SmallSpiritResonanceView = SmallSpiritResonanceView or DeclareView("SmallSpiritResonanceView", "equip_view/small_spirit_resonance")
VIEW_DECLARE_LEVEL(SmallSpiritResonanceView, ViewLevel.Ls)
VIEW_DECLARE_MASK(SmallSpiritResonanceView, ViewMask.BlockClose)

function SmallSpiritResonanceView:SmallSpiritResonanceView()
    
end


----------------------------SmallSpiritResonancePanel----------------------------
SmallSpiritResonancePanel = SmallSpiritResonancePanel or DeclareMono("SmallSpiritResonancePanel", UIWFlushPanel)
function SmallSpiritResonancePanel:SmallSpiritResonancePanel()
    self.Data = EquipData.Instance
    self.data = self.Data.small_reasonance_data
    self.language = Language.Equip
    -- 当前缺少的最低颜色品质
    self.now_lack = -1
    self.equip_type = self.Data.now_select_equip_data:EquipType()
    if self.Data.now_combine_data then
        self.equip_combine = self.Data.now_combine_data.combine_attr.resonance_group_id
        self.equip_combine_level = self.Data.now_combine_data.combine_attr.resonance_level
    end
    self.now_show_level = 0
end

function SmallSpiritResonancePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushRight()
    self:FlushSmallCircle()
end

function SmallSpiritResonancePanel:FlushSmallCircle()
    local spirit_combine_config = Cfg.SpiritStoneCombineConfig(self.data.now_select_data.resonance_group_id)
    local circle_config = self.Data:GetColorListByCombine(spirit_combine_config, true)
    local is_satisfy = true
    for k, v in ipairs(circle_config) do
        if not v.has then
            is_satisfy = false
            self.now_lack = v.color
            break
        end
    end
    self.ObjGoToGet:SetActive(not is_satisfy)
    self.CircleList:SetData(circle_config)
end

function SmallSpiritResonancePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Data.small_reasonance_data:Set({now_select_data = {}})
end

function SmallSpiritResonancePanel:OnLevelShowChange(param)
    self:FlushRight(param)
end

function SmallSpiritResonancePanel:FlushRight(change_param)
    local spirit_combine_config = self.Data:GetSpiritCombineAttrData(self.equip_type, self.data.now_select_data.resonance_group_id)
    local max_level = table.nums(spirit_combine_config) - 1
    local min_level = 0
    if not change_param then
        if self.equip_combine then
            if self.equip_combine == self.data.now_select_data.resonance_group_id then
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
    UH.SetText(self.TxtResonanceName, self.data.now_select_data.group_name)
    local spirit_combine_attr = self.Data:SpiritCombineAttr(self.equip_type, self.data.now_select_data.resonance_group_id, self.now_show_level)
    self.Data:TotalSpiritCombineAttr(spirit_combine_attr)
    local spirit_combine_attr_data = self.Data:DealCombineAttrToTxt(spirit_combine_attr, self.language.SingleAttr[3], self.language.CombineAttr[2])
    self.ResonanceAttrList:SetData(spirit_combine_attr_data)
end

function SmallSpiritResonancePanel:OnGoToGetClick()
    ViewMgr:CloseView(SmallSpiritResonanceView)
    ViewMgr:CloseView(EquipView)
    if self.now_lack == 3 then
        ShoppingCtrl.Instance:OpenStoreView(11)  --表中对应page
    else
        ViewMgr:OpenViewByKey(Mod.DeepDarkFantasy.Main)
    end
end