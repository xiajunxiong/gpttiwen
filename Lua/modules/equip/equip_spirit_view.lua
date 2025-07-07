EquipSpirit = EquipSpirit or DeclareMono("EquipSpirit", UIWFlushPanel)
function EquipSpirit:EquipSpirit()
    self.Data = EquipData.Instance
    self.language = Language.Equip
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.equipment_list, func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.jewelry_list, func = self.FlushEquip, init_call = false},
    }

    -- 当前所选装备
    self.Data.now_select_equip_data = nil
    -- 当前所选装备的铸灵组合数据
    self.Data.now_combine_data = nil
    -- 当前中间所选铸灵信息
    self.Data.now_select_spirit_data = nil
    -- 当前左侧所选铸灵信息
    self.Data.now_left_select_spirit = nil

    -- 当前左侧选中的铸灵的toggle
    self.now_select_spirit_toggle = nil
    -- 当前中间选中的格子toggle
    self.now_select_center_spirit_toggle = nil

    -- 用于判断是否切换了装备，切换了才播一次特效
    self.item_unique_id_1 = 0
    self.item_unique_id_2 = 0
    
    self.effect_spirits = {}
    self.effect_equip = nil
    self.now_select_equip_index = 1
    self.now_select_center_index = -1

    TimeHelper:CancelTimer(self.open_delay_handler)
    self.open_delay_handler = TimeHelper:AddDelayTimer(function()
        self.NowSelectEquip:SetObjActive(self.Data.now_select_equip_data ~= nil)
        self.ObjEffectBack:SetActive(true)
        self.ObjNeedUp:SetActive(true)
    end, 0.5)
end

function EquipSpirit:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.Data:ClearAllData()
    TimeHelper:CancelTimer(self.open_delay_handler)
    TimeHelper:CancelTimer(self.delay_show_handler)
end

function EquipSpirit:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[141].desc)
end

function EquipSpirit:onFlush()
    self:FlushEquip()
end

-- 相当于刷新全部了
function EquipSpirit:FlushEquip()
    local data = self.Data:GetEquipList()
    self.EquipList:SetData(data)
    self.ObjNoEquip:SetActive(TableIsEmpty(data))
    if not TableIsEmpty(data) then
        if self.Data.now_select_equip_data then
            if self.now_select_equip_index <= table.nums(data) then
                self.EquipList:ClickItem(self.now_select_equip_index)
            else
                self.EquipList:ClickItem(1)
            end
        else
            self.EquipList:ClickItem(1)
        end
    else
        self.Data.now_select_equip_data = nil
        self.Data.now_combine_data = nil
        self:FlushAll()
    end
end

-- 左边装备点击回调
function EquipSpirit:OnSelectEquip(cell)
    self.Data.now_select_equip_data = cell
    self.Data:SetNowEquipSpiritColor()
    self.now_select_equip_index = self.EquipList:GetSelectedIndex(cell)
    self.Data.now_equip_spirit_type = self.Data:GetEquipSpiritType(cell:EquipType())
    self.Data.now_combine_data = cell:GetSpiritResonance()
    self:SetCenterSelected(nil, false)
    self:FlushAll()
end

function EquipSpirit:FlushAll()
    self:FlushCenter()
    self:FlushRight()
    self:FlushDown()
    self:FlushSpiritList()
end

function EquipSpirit:PlayTopEffect(m_type)
    if self.item_unique_id_1 ~= self.Data.now_select_equip_data.item_unique_id_1 or self.item_unique_id_2 ~= self.Data.now_select_equip_data.item_unique_id_2 then
        self:StopTopEffect()
        local index = m_type == EquipSpiritType.Yin and 3161043 or 3161044
        self.top_effect_handler = self.EffectTool:Play(index)
        self.item_unique_id_1 = self.Data.now_select_equip_data.item_unique_id_1
        self.item_unique_id_2 = self.Data.now_select_equip_data.item_unique_id_2
    end
end

function EquipSpirit:StopTopEffect()
    if self.top_effect_handler then
        self.EffectTool:Stop(self.top_effect_handler)
        self.top_effect_handler = nil
    end
end

-- 刷新中间显示
function EquipSpirit:FlushCenter()
    self.NowSelectEquip:SetObjActive(self.Data.now_select_equip_data ~= nil)
    if self.Data.now_select_equip_data then
        self.NowSelectEquip:SetData(self.Data.now_select_equip_data)
        self:PlayTopEffect(self.Data.now_equip_spirit_type)
        local spirit_item_data = self.Data.now_select_equip_data:GetSpiritList()
        for i = 1, EQUIP_SPIRIT_MAX do
            self.SpiritItems[i]:SetData(spirit_item_data[i])
            self:SetEffect(i - 1, spirit_item_data[i].item_id)
        end
        if self.effect_equip then
            self.EffectTool:Stop(self.effect_equip)
        end
        -- 有共鸣组合要设置大特效
        if self.Data.now_combine_data then
            UH.SetText(self.TxtResonanceName, self.Data.now_combine_data.combine_config.group_name)
            local effect_id = self.Data:GetCombineEffectData(self.Data.now_combine_data.combine_config.resonance_group_id)
            self.effect_equip = self.EffectTool:Play(SpiritEffectBigId[effect_id])
        else
            UH.SetText(self.TxtResonanceName, self.language.EquipNoResonance)
        end
        if self.now_select_center_index ~= -1 then
            -- 手动点击中间选中的格子刷新
            self:OnSlotClick(self.SpiritItems[self.now_select_center_index + 1])
        end
    else
        for i = 1, EQUIP_SPIRIT_MAX do
            self.SpiritItems[i]:SetData({})
            self:SetEffect(i - 1, 0)
        end
        UH.SetText(self.TxtResonanceName, self.language.EquipNoResonance)
        if self.effect_equip then
            self.EffectTool:Stop(self.effect_equip)
        end
        self:StopTopEffect()
    end
end

-- 设置小特效
function EquipSpirit:SetEffect(slot, item_id)
    if self.effect_spirits[slot] then
        self.EffectTool:Stop(self.effect_spirits[slot])
    end
    if item_id > 0 then
        local item_config = Item.GetConfig(item_id)
        self.effect_spirits[slot] = self.EffectTool:Play(SpiritEffectId[item_config.color][slot])
    end
end

-- 刷新右侧显示
function EquipSpirit:FlushRight()
    if self.Data.now_select_equip_data then
        local spirit_list = self.Data.now_select_equip_data:GetHasSpiritList()
        if not TableIsEmpty(spirit_list) then
            self.ObjSpiritNo:SetActive(false)
            self.ObjSpiritResonanceNo:SetActive(TableIsEmpty(self.Data.now_combine_data))
            -- 计算显示铸灵属性和共鸣属性
            local spirit_attr_config = self.Data:TotalSpiritListAttr(spirit_list)
            local spirit_attr_data = {}
            if self.Data.now_combine_data then
                UH.SetText(self.TxtResonanceLevel, Format(self.language.EquipStoneLevel2, self.Data.now_combine_data.combine_attr.resonance_level + 1))
                UH.SetText(self.TxtResonanceName2, self.Data.now_combine_data.combine_config.group_name)

                local spirit_combine_attr = self.Data:SpiritCombineAttr(
                    self.Data.now_select_equip_data:EquipType(),
                    self.Data.now_combine_data.combine_attr.resonance_group_id,
                    self.Data.now_combine_data.combine_attr.resonance_level
                )
                self.Data:TotalSpiritCombineAttr(spirit_combine_attr)
                spirit_attr_data = self.Data:DealSpiritByCombineAttrToTxt(
                    spirit_attr_config,
                    spirit_combine_attr,
                    self.language.SingleAttr[1],
                    self.language.SingleAttr[2]
                )

                local spirit_combine_attr_data = self.Data:DealCombineAttrToTxt(spirit_combine_attr, self.language.SingleAttr[1], self.language.CombineAttr[1])
                self.ResonanceAttrList:SetData(spirit_combine_attr_data)
            else
                spirit_attr_data = self.Data:DealSpiritAttrToTxt(spirit_attr_config, self.language.SingleAttr[1])
            end
            self.SpiritAttrList:SetData(spirit_attr_data)
            return
        end
    end
    self.ObjSpiritNo:SetActive(true)
    self.ObjSpiritResonanceNo:SetActive(true)
end

-- 刷新下方显示
function EquipSpirit:FlushDown()
    local config = self.Data:GetResonanceDownItemData()
    self.ResonanceDownItemList:SetData(config)
    if not TableIsEmpty(config) then
        self.ResonanceDownItemList.ScrollRect:StopMovement()
        self.ResonanceDownItemList:JumpHorizontalIndex(1, 200)
    end
end

-- 点击中间五个槽回调
function EquipSpirit:OnSlotClick(mono)
    local data = mono:GetData()
    if TableIsEmpty(data) then
        return
    end
    local is_open = EquipData.Instance:GetSlotIsOpen(data.slot)
    if not is_open then
        PublicPopupCtrl.Instance:Center(self.language.SlotNotOpenTip)
        return
    end
    -- 存下方法，供装备上的“替换”按钮调用
    self.Data.spirit_replace_fun = function()
        self:SetCenterSelected(mono, true)
        self.ObjSpiritList:SetActive(true)
        if self.ToggleShowEquiped.isOn ~= self.Data.is_show_equiped then
            self.ToggleShowEquiped.isOn = self.Data.is_show_equiped
        end
        self:FlushSpiritList()
    end
    self.Data.now_select_spirit_data = data
    -- 判断该孔是否有穿铸灵，没穿展开列表，穿了直接打开操作列表
    if data.item_id == 0 then
        self:SetCenterSelected(mono, true)
        self.ObjSpiritList:SetActive(true)
        if self.ToggleShowEquiped.isOn ~= self.Data.is_show_equiped then
            self.ToggleShowEquiped.isOn = self.Data.is_show_equiped
        end
    else
        -- 这块对中间五个铸灵的选中状态特殊处理
        self.ToggleGroup.allowSwitchOff = true
        mono.Selector.isOn = true
        -- 铸灵列表没打开才去展开，否则穿戴刷新会重叠
        if not self.ObjSpiritList.activeSelf then
            CellClicks.BagGrid(mono.Cell)
        end
    end
    self:FlushSpiritList()
end

function EquipSpirit:OnShopClick()
	ShoppingCtrl.Instance:OpenStoreView(11)  --表中对应page
end

function EquipSpirit:OnCheckClick()
    if self.Data.now_select_equip_data then
        ViewMgr:OpenView(SpiritResonanceView)
    else
        PublicPopupCtrl.Instance:Center(self.language.NoSelectEquipTip)
    end
end

-- 设置中间五个铸灵的选中状态
function EquipSpirit:SetCenterSelected(mono, value)
    self.ToggleGroup.allowSwitchOff = not value
    if mono then
        self.now_select_center_spirit_toggle = mono.Selector
        self.now_select_center_index = mono.Slot
    else
        self.now_select_center_index = -1
    end
    if self.now_select_center_spirit_toggle then
        self.now_select_center_spirit_toggle.isOn = value
    end
end

-- 关闭操作列表
function EquipSpirit:CloseOperation()
    self.ObjOperation:SetActive(false)
    self.Data.now_left_select_spirit = nil
    -- 这里判断需要判断铸灵列表是否有打开
    if self.ObjSpiritList.activeSelf then
        -- 这块是操作手风琴取消高亮的
        if self.now_select_spirit_toggle ~= nil then
            self.now_select_spirit_toggle.isOn = false
        end
    else
        self:SetCenterSelected(nil, false)
    end
end

-- 关闭左侧铸灵列表
function EquipSpirit:HideSpiritList()
    self.ObjSpiritList:SetActive(false)
    self.Data.now_left_select_spirit = nil
    self:SetCenterSelected(nil, false)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    TimeHelper:CancelTimer(self.delay_show_handler)
    self:CloseOperation()
end

-- 刷新左边铸灵列表
function EquipSpirit:FlushSpiritList()
    if not self.ObjSpiritList.activeSelf then
        return
    end
    local data = self.Data:GetSpiritItemData()
    self.ObjEmptyState:SetActive(TableIsEmpty(data))
    if self.Data.now_select_equip_data then
        UH.SetText(self.TxtEmpty, Format(self.language.SpiritListEmptyTip, self.language.StoneType[self.Data.now_equip_spirit_type]))
    end
    self.ASpiritList:SetData(data, true)
    local jump_data = self.Data:GetSpiritJumpData(data)
    if not TableIsEmpty(jump_data) then
        self.ASpiritList:ClickChild(jump_data)
        self.ASpiritList:JumpVerticalIndex(jump_data.children_index, 140, jump_data.parent_index, 60)
    end
end

-- 点击“显示所有已镶嵌”
function EquipSpirit:ChangeShowEquiped(value)
    self.Data.is_show_equiped = value
    self:FlushSpiritList()
end

-- 点击左侧铸灵列表
function EquipSpirit:OnSpiritItemClick(mono)
    self.Data.now_left_select_spirit = mono:GetData()
    self:OpenOperation(mono)
end

-- 打开操作列表
function EquipSpirit:OpenOperation(mono)
    TimeHelper:CancelTimer(self.delay_show_handler)
    self.now_select_spirit_toggle = mono.Selector
    self.now_select_spirit_toggle.isOn = true
    -- 计算展开后操作列表放的位置
    self.delay_show_handler = TimeHelper:AddDelayFrameTimer(function()
        local v = self.ObjOperationList.gameObject.transform.localPosition
        local now = UH.GetRootPosition(mono.gameObject)
        -- 这块做限制，不能超过这个范围，否则生成出去了y:-225~225
        now.y = now.y > 225 and 225 or now.y
        now.y = now.y < -225 and -225 or now.y
        UH.LocalPosG(self.ObjOperationList.gameObject, Vector3.New(-130, now.y, 0))
        self:JudgeShowOperationButton()
        if not self:JudgeAccIsAllClose() then
            self.ObjOperation:SetActive(true)
        end
    end, 10)
end

-- 判断手风琴是否全部父节点都收缩了
function EquipSpirit:JudgeAccIsAllClose()
    for k, v in pairs(Language.Equip.SpiritListColor) do
        if not self.ASpiritList:IsParentClose(v) then
            return false
        end
    end
    return true
end

-- 判断显示什么操作按钮
function EquipSpirit:JudgeShowOperationButton()
    if self.Data.now_left_select_spirit then
        -- 装备按钮涉及阴阳互斥的问题
        if not self.Data.now_left_select_spirit.is_now_spirit
        and (self.Data.now_equip_spirit_type == self.Data.now_left_select_spirit.spirit_type
        or self.Data.now_equip_spirit_type == EquipSpiritType.Chaos
        or self.Data.now_left_select_spirit.spirit_type == EquipSpiritType.Chaos) then
            self.OperationList[SpiritOperation.Equip]:SetActive(true)
        else
            self.OperationList[SpiritOperation.Equip]:SetActive(false)
        end
        local is_max_level = self.Data:GetSpiritIsMaxLevel(self.Data.now_left_select_spirit.config.item_id, self.Data.now_left_select_spirit.config.level)
        self.OperationList[SpiritOperation.Level]:SetActive(not is_max_level)
        self.OperationList[SpiritOperation.Remove]:SetActive(self.Data.now_left_select_spirit.equip ~= nil)
        self.OperationList[SpiritOperation.Resolve]:SetActive(self.Data.now_left_select_spirit.equip == nil)
    else
        -- 中间五个铸灵展开的操作按钮不可能会有装备按钮和分解按钮
        self.OperationList[SpiritOperation.Equip]:SetActive(false)
        local is_max_level = self.Data:GetSpiritIsMaxLevel(self.Data.now_select_spirit_data.item_id, self.Data.now_select_spirit_data.level)
        self.OperationList[SpiritOperation.Level]:SetActive(not is_max_level)
        self.OperationList[SpiritOperation.Remove]:SetActive(true)
        self.OperationList[SpiritOperation.Resolve]:SetActive(false)
    end
end

-- 点击装备
function EquipSpirit:OnEquipClick()
    self.Data:SendEquipInfo()
    self:HideSpiritList()
end

-- 点击升级
function EquipSpirit:OnLevelClick()
    self.Data:OpenSpiritLevelView()
    ViewMgr:OpenView(SpiritLevelView)
    self:HideSpiritList()
end

-- 点击卸下
function EquipSpirit:OnRemoveClick()
    self.Data:SendRemoveInfo()
    self:HideSpiritList()
end

-- 点击分解
function EquipSpirit:OnResolveClick()
    ViewMgr:OpenView(SpiritResolveView)
    self:HideSpiritList()
end


-- 左侧装备栏
--------------------------EquipSpiritEquipItem--------------------------
EquipSpiritEquipItem = EquipSpiritEquipItem or DeclareMono("EquipSpiritEquipItem", UIWidgetBaseItem)
function EquipSpiritEquipItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    local spirit_list_dta = data:GetSpiritList()
    UH.SetText(self.TxtName, data:NameId())
    self.ObjUsed:SetActive(not data.in_bag)
    for i = 1, EQUIP_SPIRIT_MAX do
        self.CircleList[i]:SetData(spirit_list_dta[i])
    end
    local combine_config = data:GetSpiritResonance()
    if not TableIsEmpty(combine_config) then
        self.TxtResonanceName:SetObjActive(true)
        UH.SetText(self.TxtResonanceName, combine_config.combine_config.group_name)
    else
        self.TxtResonanceName:SetObjActive(false)
    end
    -- 判断是否显示红点
    self.RedPoint:SetNum(data.in_bag and 0 or EquipData.Instance:GetSpiritRemind(data))
end

function EquipSpiritEquipItem:Click()
    UIWidgetBaseItem.Click(self)
    self.ClickEvent:Trigger()
end


-- 左侧装备栏上的孔位
--------------------------SpiritCircleItem--------------------------
SpiritCircleItem = SpiritCircleItem or DeclareMono("SpiritCircleItem", UIWidgetBaseItem)
function SpiritCircleItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_open = EquipData.Instance:GetSlotIsOpen(data.slot)
    self.ObjLock:SetActive(not is_open)
    if data.item_id ~= 0 then
        local config = Item.GetConfig(data.item_id)
        if config and config.color then
            UH.SpriteName(self.ImgSetBack, "CircleBack_" .. config.color)
        else
            UH.SpriteName(self.ImgSetBack, "CircleBack_0")
        end
    else
        UH.SpriteName(self.ImgSetBack, "CircleBack_0")
    end
    UH.SetIcon(self.ImgSetTop, Item.GetIconId(data.item_id))
end


-- 中间5个孔位格子
--------------------------SpiritCenterItem--------------------------
SpiritCenterItem = SpiritCenterItem or DeclareMono("SpiritCenterItem", UIWidgetBaseItem)
function SpiritCenterItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushShow()
end

function SpiritCenterItem:FlushShow()
    local is_open, open_level = EquipData.Instance:GetSlotIsOpen(self.Slot)
    self.ObjLock:SetActive(not is_open)
    self.ObjLevelBack:SetActive(not is_open)
    UH.SetText(self.TxtOpenLevel, Format(Language.Equip.EquipStoneLevel2, open_level))
    self.Cell:SetObjActive(self.data.item_id and self.data.item_id ~= 0)
    if not TableIsEmpty(self.data) then
        self.IHandler.Interactable = true
        self.ObjPlus:SetActive(is_open and self.data.item_id == 0)
        -- UH.SetIcon(self.ImgSetIcon, Item.GetIconId(self.data.item_id))
        if self.data.item_id ~= 0 then
            local config = Item.GetConfig(self.data.item_id)
            local level_config = EquipData.Instance:GetSpiritLevelData(self.data.item_id, self.data.level)
            -- UH.SpriteName(self.ImgSetQuaIcon, COLOR_FRAME_SPRITE[config.color])
            -- 判断是否可升级
            self.ObjCanLevelUp:SetActive(EquipData.Instance:JudgeEquipSpiritCanLevelUp(self.data.item_id, self.data.level))
            self.ObjNameBack:SetActive(true)
            self.SpiritName:SetData(self.data)
            local item_data = Item.Init(self.data.item_id, 1, 1)
            item_data.param.level = self.data.level
            item_data.param.slot = self.data.slot
            self.Cell:SetData(item_data)
        else
            -- UH.SpriteName(self.ImgSetQuaIcon, COLOR_FRAME_SPRITE[ItemColor.White])
            self.ObjCanLevelUp:SetActive(false)
            self.ObjNameBack:SetActive(false)
        end
    else
        self:SetSelected(false)
        self.IHandler.Interactable = false
        -- UH.SetIcon(self.ImgSetIcon, Item.GetIconId(0))
        -- UH.SpriteName(self.ImgSetQuaIcon, COLOR_FRAME_SPRITE[ItemColor.White])
        self.ObjPlus:SetActive(false)
        self.ObjCanLevelUp:SetActive(false)
        self.ObjNameBack:SetActive(false)
    end
end


-- 下方共鸣
--------------------------ResonanceDownItem--------------------------
ResonanceDownItem = ResonanceDownItem or DeclareMono("ResonanceDownItem", UIWidgetBaseItem)
function ResonanceDownItem:ResonanceDownItem()
    self.now_select_equip_cell = nil
    self.now_combine_id = nil
    self.last_is = nil
end

function ResonanceDownItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtName, data.config.group_name)
    self.ObjNow:SetActive(data.m_type == ResonanceDownItemType.Now)
    self.ObjRecommend:SetActive(data.m_type == ResonanceDownItemType.Recommend)
    self.CircleList:SetData(EquipData.Instance:GetColorListByCombine(data.config, true))
    -- 只有当前会播放特效
    if BagData.Instance:JudgeEquipIsEqual(self.now_select_equip_cell, EquipData.Instance.now_select_equip_data) then
        if EquipData.Instance.now_combine_data then
            if self.now_combine_id ~= EquipData.Instance.now_combine_data.combine_config.resonance_group_id or self.last_is ~= data.m_type then
                if data.m_type == ResonanceDownItemType.Now then
                    self:PlayBorderEffect()
                end
            end
            self.now_combine_id = EquipData.Instance.now_combine_data.combine_config.resonance_group_id
        else
            self.now_combine_id = nil
        end
    else
        self.now_select_equip_cell  = EquipData.Instance.now_select_equip_data
        if EquipData.Instance.now_combine_data then
            self.now_combine_id = EquipData.Instance.now_combine_data.combine_config.resonance_group_id
        else
            self.now_combine_id = nil
        end
        self:StopBorderEffect()
    end
    self.last_is = data.m_type
end

function ResonanceDownItem:PlayBorderEffect()
    self:StopBorderEffect()
    self.border_effect_handler = self.EffectTool:Play(3161042)
end

function ResonanceDownItem:StopBorderEffect()
    if self.border_effect_handler then
        self.EffectTool:Stop(self.border_effect_handler)
        self.border_effect_handler = nil
    end
end

function ResonanceDownItem:OpenSmallResonanceView()
    EquipData.Instance.small_reasonance_data.now_select_data = self.data.config
    ViewMgr:OpenView(SmallSpiritResonanceView)
end


-- 左侧铸灵列表
SpiritChildItem = SpiritChildItem or DeclareMono("SpiritChildItem", UIWidgetBaseItem)
function SpiritChildItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.ImgSetIcon, Item.GetIconId(data.config.item_id))
    local config = Item.GetConfig(data.config.item_id)
    local level_config = EquipData.Instance:GetSpiritLevelData(data.config.item_id, data.config.level)
    UH.SpriteName(self.ImgSetQuaIcon, COLOR_FRAME_SPRITE[config.color])
    UH.SetText(self.TxtItemLevel, Format(Language.Equip.EquipStoneLevel2, data.config.level + 1))
    local str_type = Format(Language.Equip.EquipStoneType2, Language.Equip.StoneType[level_config.spirit_stone_type])
    UH.SetText(self.TxtItemType, Format(Language.Common.Xstr, QualityColorStr[config.color], str_type))
    UH.SetText(self.TxtItemName, Format(Language.Common.Xstr, QualityColorStr[config.color], config.name))

    -- 判断是否是当前装备的当前孔位铸灵
    self.data.is_now_spirit = false
    -- 判断是否当前装备
    self.data.is_now_equip = false
    -- 获取当前铸灵类型
    self.data.spirit_type = EquipData.Instance:GetSpiritType(data.config.item_id)
    -- 判断是否已镶嵌在装备上
    if not TableIsEmpty(data.equip) then
        self.data.is_now_equip = BagData.Instance:JudgeEquipIsEqual(EquipData.Instance.now_select_equip_data, data.equip)
        if self.data.is_now_equip and EquipData.Instance.now_select_spirit_data ~= nil then
            self.data.is_now_spirit = EquipData.Instance.now_select_spirit_data.slot == data.config.slot
        end
        self.ObjEquiped:SetActive(true)
        UH.SetText(self.TxtCount, "")
    else
        self.ObjEquiped:SetActive(false)
        UH.SetText(self.TxtCount, data.item.num > 1 and data.item.num or "")
    end
    self.ObjNow:SetActive(self.data.is_now_spirit)
    local config = EquipData.Instance:SpiritLevelAttr(data.config.item_id, data.config.level)
    self.AttrList:SetData(EquipData.Instance:DealSpiritAttrToTxt(config, Language.Equip.SingleAttr[4]))

    -- 判断是否显示装备后激活铸灵组合
    if not TableIsEmpty(EquipData.Instance.now_select_equip_spirit_color)
    and not self.data.is_now_equip
    and (EquipData.Instance.now_equip_spirit_type == self.data.spirit_type
    or EquipData.Instance.now_equip_spirit_type == EquipSpiritType.Chaos
    or self.data.spirit_type == EquipSpiritType.Chaos) then
        local table_color = {}
        local now_spirit_color = Item.GetConfig(self.data.config.item_id).color
        if EquipData.Instance.now_select_spirit_data.item_id == 0 then
            -- 拷贝一份这个颜色表
            table_color = TableCopy(EquipData.Instance.now_select_equip_spirit_color)
            table.insert(table_color, {color = now_spirit_color})
            local can_get_combine = EquipData.Instance:GetSpiritCombineDataByColorList(table_color)
            -- 检测到相同的共鸣组合就不显示
            if EquipData.Instance.now_combine_data and can_get_combine then
                if EquipData.Instance.now_combine_data.combine_config.resonance_group_id == can_get_combine.resonance_group_id then
                    can_get_combine = nil
                end
            end
            self.ObjCanCombine:SetActive(can_get_combine ~= nil)
            if can_get_combine ~= nil then
                UH.SetText(self.TxtCombine, Format(Language.Equip.EquipStoneType2, can_get_combine.group_name))
            end
        else
            if now_spirit_color == Item.GetConfig(EquipData.Instance.now_select_spirit_data.item_id).color then
                self.ObjCanCombine:SetActive(false)
            else
                -- 拷贝一份这个颜色表
                table_color = TableCopy(EquipData.Instance.now_select_equip_spirit_color)
                for k, v in pairs(table_color) do
                    if v.slot == EquipData.Instance.now_select_spirit_data.slot then
                        v.color = now_spirit_color
                        break
                    end
                end
                local can_get_combine = EquipData.Instance:GetSpiritCombineDataByColorList(table_color)
                -- 检测到相同的共鸣组合就不显示
                if EquipData.Instance.now_combine_data and can_get_combine then
                    if EquipData.Instance.now_combine_data.combine_config.resonance_group_id == can_get_combine.resonance_group_id then
                        can_get_combine = nil
                    end
                end
                self.ObjCanCombine:SetActive(can_get_combine ~= nil)
                if can_get_combine ~= nil then
                    UH.SetText(self.TxtCombine, Format(Language.Equip.EquipStoneType2, can_get_combine.group_name))
                end
            end
        end
    else
        self.ObjCanCombine:SetActive(false)
    end
end

function SpiritChildItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end


-- 共鸣品质颜色点
--------------------------ResonanceSmallCircleItem--------------------------
ResonanceSmallCircleItem = ResonanceSmallCircleItem or DeclareMono("ResonanceSmallCircleItem", UIWidgetBaseItem)
function ResonanceSmallCircleItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.ImgSetCircleItem, "Circle_" .. data.color)
    self.ObjHas:SetActive(data.has == true)
end


-- 铸灵文本
--------------------------SpiritTxtItem--------------------------
SpiritTxtItem = SpiritTxtItem or DeclareMono("SpiritTxtItem", UIWidgetBaseItem)
function SpiritTxtItem:SetData(data)
    UH.SetText(self.Txt, data.txt or "")
    if data.attr_type then
        self.ObjPoint:SetActive(DataHelper.IsPoint(data.attr_type))
    else
        self.ObjPoint:SetActive(false)
    end
end


-- 铸灵名字显示文本
--------------------------SpiritNameTxtItem--------------------------
SpiritNameTxtItem = SpiritNameTxtItem or DeclareMono("SpiritNameTxtItem", UIWidgetBaseItem)
function SpiritNameTxtItem:SetData(data)
    local config = Item.GetConfig(data.item_id)
    local m_type = EquipData.Instance:GetSpiritType(data.item_id)
    local level = Cfg.SpiritStoneConfigById(data.item_id).spirit_stone_level
    UH.SetText(self.TxtItemLevel, Format(Language.Equip.EquipStoneLevel2, level + 1))
    self.TxtItemTypeLeft.color = QualityColor[config.color]
    self.TxtItemTypeRight.color = QualityColor[config.color]
    local str_type = Format(Language.Common.Xstr, QualityColorStr[config.color], Language.Equip.StoneType[m_type])
    UH.SetText(self.TxtItemType, "  "..str_type.."  ")
    UH.SetText(self.TxtItemName, Format(Language.Common.Xstr, QualityColorStr[config.color], config.name))
end
