SpiritLevelView = SpiritLevelView or DeclareView("SpiritLevelView", "equip_view/spirit_level")
VIEW_DECLARE_LEVEL(SpiritLevelView, ViewLevel.Ls)
VIEW_DECLARE_MASK(SpiritLevelView, ViewMask.BgBlock)

function SpiritLevelView:SpiritLevelView()
    self.Board:SetData(self:GetType(), Language.Equip.SpiritLevelTitle, Vector2.New(883, 464))
end

function SpiritLevelView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[142].desc)
end

----------------------------SpiritLevelPanel----------------------------
SpiritLevelPanel = SpiritLevelPanel or DeclareMono("SpiritLevelPanel", UIWFlushPanel)
function SpiritLevelPanel:SpiritLevelPanel()
    self.Data = EquipData.Instance
    self.data = self.Data.spirit_level_data
    self.language = Language.Equip
    self.is_first = true
    self.change_has_one = 0
    self.change_need_one = 0
    
    -- 背包刷新的时候需要重新获取物品、装备格子
    -- 0：背包中的铸灵
    -- 1：背包中的装备中的铸灵
    -- 2：背包中的装备中的铸灵
    -- 3：身上的装备铸灵
    -- 4：身上的灵饰铸灵
    self.in_hear = 0
    self.now_cell_index = -1
    self.now_cell_column = -1
    -- 存储装备上的铸灵信息
    self.save_equip_spirit_list = {}

    self.data_cares = {
        {data = self.data, func = self.FlushRight, keys = {"now_select_data"}, init_call = false},
        {data = self.data, func = self.FlushByItemLevel, keys = {"spirit_level_bag"}, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushAll, init_call = false},
        {data = BagData.Instance.equipment_list, func = self.FlushAll, init_call = false},
        {data = BagData.Instance.jewelry_list, func = self.FlushAll, init_call = false},
    }
    self:FlushAll()
    self.is_first = false
end

function SpiritLevelPanel:Awake()
    UIWFlushPanel.Awake(self)
end

function SpiritLevelPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.save_equip_spirit_list = {}
    self.data:Set({spirit_level_bag = nil, spirit_level_equip = nil, spirit_level_config = nil, now_select_data = {}})
end

function SpiritLevelPanel:FlushAll()
    self:FlushBagSpiritNum()
    self:FlushLeft()
    self:FlushRight()
end

function SpiritLevelPanel:PlayEffect()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaChengGong)
    if self.cell_handler or self.name_handler then
        self.EffectTool:Stop(self.cell_handler)
        self.EffectTool:Stop(self.name_handler)
    end
    self.cell_handler = self.EffectTool:Play(3161039)
    self.name_handler = self.EffectTool:Play(3161040)
end

function SpiritLevelPanel:FlushByItemLevel()
    self:PlayEffect()
    self:FlushByItem()
end

function SpiritLevelPanel:FlushByItem()
    -- 升级的是背包里的铸灵，升级后的格子改变了，需要重算
    if self.data.spirit_level_bag then
        self.now_cell_index = self.data.spirit_level_bag.index
        self.now_cell_column = self.data.spirit_level_bag:GridColumn()
        self.in_hear = 0
        self.data.spirit_level_equip = nil
    end
    self:FlushBagSpiritNum()
    self:FlushRight()
end

function SpiritLevelPanel:FlushBagSpiritNum()
    -- 拿到铸灵列表后处理，这样就不用每次去遍历计算对应的铸灵数量
    -- 根据类型和等级的存起来当前所拥有的数量
    self.now_bag_spirit_num_list = self.Data:GetNowBagSpiritNumList()
    ViewMgr:CloseView(SpiritQuickBuyView)
end

function SpiritLevelPanel:FlushLeft()
    if self.now_cell_index == -1 then
        if self.data.spirit_level_equip then
            self.now_cell_index = self.data.spirit_level_equip.index
            self.now_cell_column = self.data.spirit_level_equip:GridColumn()
            if self.data.spirit_level_equip.in_bag then
                self.in_hear = self.data.spirit_level_equip:IsEquip() and 1 or 2
            else
                self.in_hear = self.data.spirit_level_equip:IsEquip() and 3 or 4
            end
            
        else
            self.now_cell_index = self.data.spirit_level_bag.index
            self.now_cell_column = self.data.spirit_level_bag:GridColumn()
            self.in_hear = 0
        end
    end
    if self.in_hear == 0 then
        self.data.spirit_level_equip = nil
    else
        -- 装备或灵饰需要重新获取
        self.data.spirit_level_bag = nil
        if self.in_hear == 1 or self.in_hear == 2 then
            self.data.spirit_level_equip = BagData.Instance:GetItemByIndex(self.now_cell_column, self.now_cell_index)
        else
            if self.in_hear == 3 then
                self.data.spirit_level_equip = BagData.Instance:GetEquipList(self.now_cell_index)
            else
                self.data.spirit_level_equip = BagData.Instance:GetJewelryList(self.now_cell_index)
            end
        end
        local spirit_list = self.data.spirit_level_equip:GetSpiritList()
        local slot = self.data.spirit_level_config.slot
        if not TableIsEmpty(self.data.now_select_data) then
            slot = self.data.now_select_data.slot
        end
        self.data.now_select_data = spirit_list[slot + 1]
    end

    self:JudgeIsLevelUpFlush()

    -- 点击的是装备上的铸灵进来才有共鸣效果可言
    if self.data.spirit_level_equip then
        self.equip_type = self.data.spirit_level_equip:EquipType()
        self.now_combine_data = self.data.spirit_level_equip:GetSpiritResonance()
        if self.now_combine_data then
            self.equip_combine = self.now_combine_data.combine_attr.resonance_group_id
            self.equip_combine_level = self.now_combine_data.combine_attr.resonance_level
        end
        local has_spirit_list_data = self.data.spirit_level_equip:GetHasSpiritList()
        self.SpiritLevelList:SetData(has_spirit_list_data)
        if self.is_first then
            for i, v in ipairs(has_spirit_list_data) do
                if self.data.spirit_level_config.slot == v.slot then
                    self.SpiritLevelList:SetectItem(i, true)
                    break
                end
            end
        end
    end

    self.ObjHaveResonance:SetActive(self.now_combine_data ~= nil)
    if self.now_combine_data then
        -- 判断铸灵组合是否满级
        local is_max_combine = self.Data:GetSpiritCombineIsMaxLevel(self.equip_type, self.equip_combine, self.equip_combine_level)
        local next_equip_combine_level = is_max_combine and self.equip_combine_level or self.equip_combine_level + 1
        UH.SetText(self.TxtLeftLevel, Format(self.language.EquipStoneLevel2, self.equip_combine_level + 1))
        UH.SetText(self.TxtRightLevel, Format(self.language.EquipStoneLevel2, next_equip_combine_level + 1))
        UH.SetText(self.TxtResonanceName, self.now_combine_data.combine_config.group_name)

        local spirit_combine_attr = self.Data:SpiritCombineAttr(self.equip_type, self.equip_combine, self.equip_combine_level)
        self.Data:TotalSpiritCombineAttr(spirit_combine_attr)
        local next_spirit_combine_attr = spirit_combine_attr
        if not is_max_combine then
            next_spirit_combine_attr = self.Data:SpiritCombineAttr(self.equip_type, self.equip_combine, next_equip_combine_level)
            self.Data:TotalSpiritCombineAttr(next_spirit_combine_attr)
        end
        local spirit_combine_attr_data = self.Data:DealLevelCombineAttrToTxt(spirit_combine_attr, next_spirit_combine_attr, self.language.SingleAttr[5], self.language.CombineAttr[3])
        self.ResonanceAttrList:SetData(spirit_combine_attr_data)
    end
end

-- 判断装备和灵饰是否是升级导致的刷新，如果是要播特效
function SpiritLevelPanel:JudgeIsLevelUpFlush()
    if self.in_hear == 0 or not self.data.spirit_level_equip or TableIsEmpty(self.data.now_select_data) then
        return
    end
    local now_spirit_list = self.data.spirit_level_equip:GetSpiritList()
    if TableIsEmpty(self.save_equip_spirit_list) then
        self.save_equip_spirit_list = now_spirit_list
        return
    end
    local index = self.data.now_select_data.slot + 1
    if self.save_equip_spirit_list[index].level ~= now_spirit_list[index].level then
        self:PlayEffect()
    end
    self.save_equip_spirit_list = now_spirit_list
end

function SpiritLevelPanel:FlushRight()
    local now_config = {}
    if self.data.spirit_level_bag then
        now_config = self.data.spirit_level_bag.param
    elseif self.data.now_select_data then
        now_config = self.data.now_select_data
    end
    if TableIsEmpty(now_config) or not now_config.item_id or not now_config.level then
        PublicPopupCtrl.Instance:Center(self.language.ErrorTip)
        ViewMgr:CloseView(SpiritLevelView)
        return
    end
    local now_group_config = self.Data:GetSpiritLevelData(now_config.item_id)
    local next_config = TableCopy(now_config)
    if not self.Data:GetSpiritIsMaxLevel(now_config.item_id, now_config.level) then
        next_config.level = next_config.level + 1
    end

    local now_data = Item.Create({item_id = now_group_config[now_config.level].spirit_stone_id, is_bind = 1})
    self.NowCell:SetData(now_data)

    local next_data = Item.Create({item_id = now_group_config[next_config.level].spirit_stone_id, is_bind = 1})
    self.NextCell:SetData(next_data)

    self.NowName:SetData({item_id = now_group_config[now_config.level].spirit_stone_id})
    self.NextName:SetData({item_id = now_group_config[next_config.level].spirit_stone_id})

    local spirit_attr_list_data = self:GetSpiritAttrListData(now_config, next_config)
    self.SpiritAttrList:SetData(spirit_attr_list_data)

    -- 升级显示消耗数量需要转化成1级的来显示
    local now_level_config = self.Data:GetSpiritLevelData(now_config.item_id, now_config.level)
    self.change_has_one = self:GetSpiritChangeOne(now_config.item_id, now_config.level)
    self.change_need_one = now_level_config.need_num
    -- 这里需要判断是否是装备上的铸灵，装备上本身有一个，计算的时候要少1
    if self.data.spirit_level_equip then
        self.change_need_one = self.change_need_one - 1
    end
    self.change_need_one = self.change_need_one * self.Data:GetSpiritEquipOneNum(now_config.item_id)

    local is_enough = self.change_has_one >= self.change_need_one
    local color = is_enough and COLORSTR.Green4 or COLORSTR.Red8
    local num_str = Format(Language.Common.StuffNumColorFirst, color, self.change_has_one, self.change_need_one)
    local consume_data = Item.Create({item_id = now_group_config[0].spirit_stone_id, is_bind = 1})
    -- consume_data.param = {item_id = now_config.item_id, level = 0}
    self.ConsumeCell:SetData(consume_data)
    UH.SetText(self.TxtConsume, now_level_config.need_coin)
    UH.SetText(self.TxtConsumeName, consume_data:QuaName())
    UH.SetText(self.TxtConsumeItem, num_str)
    self.ConsumeCell:SetObjActive(now_config.level ~= next_config.level)
    self.ObjBtnLevel:SetActive(now_config.level ~= next_config.level)
    self.TxtConsumeName:SetObjActive(now_config.level ~= next_config.level)
    self.TxtConsumeItem:SetObjActive(now_config.level ~= next_config.level)
    self.ObjMaxLevel:SetActive(now_config.level == next_config.level)
end

-- 当前拥有的小于等于level的相同group的转化成1级的数量
function SpiritLevelPanel:GetSpiritChangeOne(item_id, level)
    local has_num = 0
    local group = self.Data:GetSpiritGroup(item_id)
    local now_group_config = self.Data:GetSpiritLevelData(item_id)
    for i = 0, level do
        if self.now_bag_spirit_num_list[group] then
            if self.now_bag_spirit_num_list[group][i] then
                local change_one_num = self.Data:GetSpiritEquipOneNum(now_group_config[i].spirit_stone_id)
                has_num = has_num + self.now_bag_spirit_num_list[group][i] * change_one_num
            end
        else
            break
        end
    end
    return has_num
end

function SpiritLevelPanel:GetSpiritAttrListData(now_config, next_config)
    local tab = {}
    local now_spirit_attr = self.Data:SpiritLevelAttr(now_config.item_id, now_config.level)
    local next_spirit_attr = self.Data:SpiritLevelAttr(next_config.item_id, next_config.level)
    for k, v in pairs(now_spirit_attr) do
        local t = {}
        t.left_txt = Format(self.language.SingleAttr[6], v.attr_name, v.attr_percent_value)
        t.right_txt = Format(self.language.SingleAttr[6], next_spirit_attr[k].attr_name, next_spirit_attr[k].attr_percent_value)
        table.insert(tab, t)
    end
    return tab
end

function SpiritLevelPanel:OnLevelUpClick()
    local now_config = {}
    if self.data.spirit_level_bag then
        now_config = self.data.spirit_level_bag.param
    elseif self.data.now_select_data then
        now_config = self.data.now_select_data
    end
    if self.Data:GetSpiritIsMaxLevel(now_config.item_id, now_config.level) then
        PublicPopupCtrl.Instance:Center(self.language.MaxLevelSpiritTip)
        return
    end

    -- 能否升级判断可用比当前等级更低的铸灵去组合消耗
    local now_group_level_config = self.Data:GetSpiritLevelData(now_config.item_id)
    if MallCtrl.IsNotCurrency(CurrencyType.CoinBind, now_group_level_config[now_config.level].need_coin) then
        
        local now_group = self.Data:GetSpiritGroup(now_config.item_id)
        -- 现在不用快速购买，改用灵玉市场购买铸灵
        -- local quick_buy_shop_data = ShopData.GetFastBuyData(now_group_level_config[0].spirit_stone_id)
        local immortal_coin = MarketData.Instance:GetItemPrice(now_group_level_config[0].spirit_stone_id)

        -- 这里开始计算
        local view_param_t = {}
        local next_item_id = now_group_level_config[now_config.level + 1].spirit_stone_id
        view_param_t.item = Item.Create({item_id = next_item_id, is_bind = 1})
        -- 用于判断是否还有除1级以外的消耗
        local is_has_besides_one_level = false
        -- 用于while循环递减
        local select_item_level = now_config.level
        local now_need_num = now_group_level_config[select_item_level].need_num
        -- 装备上本身的铸灵算一个，所以第一个消耗要-1
        if self.data.spirit_level_equip then
            now_need_num = now_need_num - 1
        end
        -- 记录当前升级需要消耗
        local record_need = {}
        record_need.group = now_group
        record_need.item_id = now_config.item_id
        record_need.next_item_id = next_item_id
        record_need.level = now_config.level
        record_need.item = {}
        record_need.need_buy_num = 0
        while select_item_level >= 0 do
            -- 判断是否有这个等级数量的铸灵
            local now_level_has
            if self.now_bag_spirit_num_list[now_group] then
                now_level_has = self.now_bag_spirit_num_list[now_group][select_item_level] or 0
            else
                now_level_has = 0
            end
            local consume_config = now_group_level_config[select_item_level]
            -- 数量足够就可升级，否则向下查找等级低的数量是否足够
            if now_level_has >= now_need_num then
                record_need.item[select_item_level] = now_need_num
                if not is_has_besides_one_level then
                    is_has_besides_one_level = select_item_level > 0 and record_need.item[select_item_level] > 0
                end
                break
            else
                local need_add = now_need_num - now_level_has
                record_need.item[select_item_level] = now_level_has
                now_need_num = need_add
                -- 如果已经遍历到0级，就记录下还差几个，用于快速购买
                if select_item_level == 0 then
                    record_need.need_buy_num = now_need_num
                end
            end
            -- 查看上一级合成上来需要多少个
            select_item_level = select_item_level - 1
            if select_item_level >= 0 then
                local last_consume_config = now_group_level_config[select_item_level]
                now_need_num = now_need_num * last_consume_config.need_num
            end
        end

        -- 组装升级协议
        view_param_t.confirm_fun = function()
            local param_t = {}
            -- 增加容错
            if not self.data.spirit_level_equip and not self.data.spirit_level_bag then
                return
            end
            -- 判断是否是装备里的
            if self.data.spirit_level_equip then
                -- 判断装备在哪
                if self.data.spirit_level_equip.in_bag then
                    param_t.item_src = SpiritSrcType.Bag
                elseif self.data.spirit_level_equip:IsEquip() then
                    param_t.item_src = SpiritSrcType.Equip
                elseif self.data.spirit_level_equip:IsJewelry() then
                    param_t.item_src = SpiritSrcType.Jewelry
                end
                param_t.index = self.data.spirit_level_equip.index
                param_t.column = self.data.spirit_level_equip:GridColumn()
                param_t.slot_index = self.data.now_select_data.slot
            else
                param_t.item_src = SpiritSrcType.Bag
                param_t.index = self.data.spirit_level_bag.index
                param_t.column = self.data.spirit_level_bag:GridColumn()
            end
            param_t.need_buy_num = record_need.need_buy_num
            param_t.now_level = now_config.level
            param_t.item_list = record_need.item
            EquipCtrl.Instance:SendSpiritLevelUpReq(param_t)
        end
        view_param_t.record_need = record_need

        -- 数量足够或者快速购买商店里能买才能升级
        if self.change_has_one >= self.change_need_one or immortal_coin > 0 then
            -- 消耗数量满足，且消耗道具均为1级就不用弹窗
            if is_has_besides_one_level or record_need.need_buy_num > 0 then
                ViewMgr:OpenView(SpiritQuickBuyView, view_param_t)
            else
                view_param_t.confirm_fun()
            end
        else
            PublicPopupCtrl.Instance:Center(self.language.NotEnoughtSpiritTip)
            ViewMgr:OpenView(GetWayView, {item = Item.Create({item_id = now_group_level_config[0].spirit_stone_id, jump_need_num = record_need.need_buy_num})})
        end
    end
end


--------------SpiritLevelItem--------------
SpiritLevelItem = SpiritLevelItem or DeclareMono("SpiritLevelItem", UIWidgetBaseItem)
function SpiritLevelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local config = Item.GetConfig(data.item_id)
    UH.SpriteName(self.ImgSetQuaIcon, COLOR_FRAME_SPRITE[config.color])
    UH.SetIcon(self.ImgSetIcon, Item.GetIconId(data.item_id))
    UH.SetText(self.TxtLevel, Format(Language.Equip.EquipStoneLevel2, data.level + 1))
end

function SpiritLevelItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        EquipData.Instance.spirit_level_data.now_select_data = self.data
    end
end


--------------LevelTxtItem--------------
LevelTxtItem = LevelTxtItem or DeclareMono("LevelTxtItem", UIWidgetBaseItem)
function LevelTxtItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TxtLeftAttr, data.left_txt)
    UH.SetText(self.TxtRightAttr, data.right_txt)
end