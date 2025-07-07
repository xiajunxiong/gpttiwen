AccrssoryInheritView = AccrssoryInheritView or DeclareMono("AccrssoryInheritView", UIWFlushPanel)
function AccrssoryInheritView:AccrssoryInheritView()
    self.Data = AccrssoryData.Instance
    self.language = Language.Accrssory
    self.data = self.Data.accrssory_inherit_data
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Accrssory), func = self.FlushBagItem, init_call = false},
        -- {data = self.Data.accrssory_equip_data, func = self.FlushNowEquip, init_call = false},
        {data = self.data, func = self.FlushNow, keys = {"now"}, init_call = false},
        {data = self.data, func = self.FlushMaterial, keys = {"material"}, init_call = false},
    }
    self.consume_coin = 0
    -- 记录上一次选择了哪个已穿戴的配饰
    self.last_select_equip = 0
    -- 记录左边配饰需要交换的词条，用于发送协议，这里记录的位置会-1
    self.left_attr_config = {}
    -- 记录右边配饰需要交换的词条，用于发送协议，这里记录的位置会-1
    self.right_attr_config = {}
    self.Data.now_show_panel.mod_id = Mod.Accrssory.Inherit
    self.ObjBlock:SetActive(false)
end

function AccrssoryInheritView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.Data.now_show_panel.mod_id = Mod.Accrssory.Inherit
    self:FlushNow()
    self:FlushMaterial()
end

function AccrssoryInheritView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.playing_handler)
    self.data:Set({now = {}, material = {}, inherit_type = AccrssoryInheritType.Now})
end

function AccrssoryInheritView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[167].desc)
end

function AccrssoryInheritView:FlushNow()
    if not TableIsEmpty(self.data.now) then
        self.last_select_equip = self.data.now:AccrssoryType()
    else
        self.last_select_equip = 0
    end
    self:SetMaterialEmpty()
    self.AccrssoryInheritItem1:SetData({inherit_type = AccrssoryInheritType.Now})
end

function AccrssoryInheritView:FlushMaterial()
    if not TableIsEmpty(self.data.material) then
        local cfg = Cfg.AccrssoryInheritConsume(self.data.material:LimitLv(), self.data.material:Color())
        self.consume_coin = cfg and cfg.coin or 0
    else
        self.consume_coin = 0
    end
    UH.SetText(self.TxtConsume, self.consume_coin)
    self.AccrssoryInheritItem2:SetData({inherit_type = AccrssoryInheritType.Material})
    self:HelpDefaultSelect()
end

-- 当前材料只有一条属性且当前配饰有空位时自动选中
function AccrssoryInheritView:HelpDefaultSelect()
    if not TableIsEmpty(self.data.now) and not TableIsEmpty(self.data.material) then
        local now_empty_list, now_empty_num = self.data.now:GetEmptyAttr()
        local material_attr_list, material_attr_num = self.data.material:GetEffectiveAttr()
        if now_empty_num > 0 and material_attr_num == 1 then
            self.AccrssoryInheritItem1:AutoAttrClick(now_empty_list[1].pos, true)
            self.AccrssoryInheritItem2:AutoAttrClick(material_attr_list[1].pos, true)
        end
    end
end

-- function AccrssoryInheritView:FlushNowEquip()
--     if self.last_select_equip ~= 0 then
--         self.data.now = self.Data.accrssory_equip_data.info[self.last_select_equip] or {}
--     end
--     self:SetMaterialEmpty()
-- end

function AccrssoryInheritView:FlushBagItem()
    self:SetMaterialEmpty()
end

function AccrssoryInheritView:SetMaterialEmpty()
    if not TableIsEmpty(self.data.material) then
        self.data.material = {}
    end
end

function AccrssoryInheritView:OnInheritClick()
    local now = self.AccrssoryInheritItem1:GetAllSelect()
    local material = self.AccrssoryInheritItem2:GetAllSelect()
    self.left_attr_config = {}
    self.right_attr_config = {}
    if self:JudgeIsCanInherit(now, material) then
        if MallCtrl.IsNotCurrency(CurrencyType.CoinBind, self.consume_coin) then
            self.ObjBlock:SetActive(true)
            self.AccrssoryInheritItem1:PlayEffect()
            self.AccrssoryInheritItem2:PlayEffect()
            TimeHelper:CancelTimer(self.playing_handler)
            self.playing_handler = Invoke(function()
                local param_t = {}
                param_t.index = self.data.now:AccrssoryType()
                param_t.replace_index = self.data.material:GridIndex()
                param_t.replace_list = {}
                for i = 1, #self.left_attr_config do
                    table.insert(param_t.replace_list, {attr_index = self.left_attr_config[i].pos, replace_attr_index = self.right_attr_config[i].pos})
                end
                AccrssoryCtrl.Instance:SendInheritReq(param_t)
                self:SetMaterialEmpty()
                self.ObjBlock:SetActive(false)
            end, 1)
        end
    end
end

function AccrssoryInheritView:JudgeIsCanInherit(now, material)
    if TableIsEmpty(self.data.now) then
        PublicPopupCtrl.Instance:Center(self.language.InheritNowSelectEmpty)
        return
    end
    if TableIsEmpty(self.data.material) then
        PublicPopupCtrl.Instance:Center(self.language.InheritMaterialEmpty)
        return
    end
    local can_inherit_config = Cfg.AccrssoryCanInheritCount(self.data.material:GetHasAttrNum())
    local can_inherit_num = can_inherit_config.chuancheng_entry_num
    local get_count = function(list, record)
        local count = 0
        for k, v in pairs(list) do
            if v > 0 then
                count = count +1
                table.insert(record, {pos = k - 1})
            end
        end
        return count
    end
    local now_select_count = get_count(now, self.left_attr_config)
    local material_select_count = get_count(material, self.right_attr_config)
    if now_select_count ~= material_select_count then
        PublicPopupCtrl.Instance:Center(self.language.InheritNotEqual)
        return
    end
    if now_select_count == 0 or material_select_count == 0 then
        PublicPopupCtrl.Instance:Center(self.language.InheritEmpty)
        return
    end
    if now_select_count > can_inherit_num or material_select_count > can_inherit_num then
        PublicPopupCtrl.Instance:Center(self.language.InheritOverMax)
        return
    end
    return true
end

--------------AccrssoryInheritItem--------------
AccrssoryInheritItem = AccrssoryInheritItem or DeclareMono("AccrssoryInheritItem", UIWidgetBaseItem)
function AccrssoryInheritItem:AccrssoryInheritItem()
    self.now_select_data = {}
end

function AccrssoryInheritItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:ResetAllSelect()
    local item
    if data.inherit_type == AccrssoryInheritType.Now then
        item = AccrssoryData.Instance.accrssory_inherit_data.now
    elseif data.inherit_type == AccrssoryInheritType.Material then
        item = AccrssoryData.Instance.accrssory_inherit_data.material
    end
    self.ObjNotSelect:SetActive(TableIsEmpty(item))
    self.ObjAdd:SetActive(TableIsEmpty(item))
    if not TableIsEmpty(item) then
        if data.inherit_type == AccrssoryInheritType.Material then
            local can_inherit_num = Cfg.AccrssoryCanInheritCount(item:GetHasAttrNum())
            UH.SetText(self.TxtCanSelectMax, Format(Language.Accrssory.MaxSelectInherit, can_inherit_num and can_inherit_num.chuancheng_entry_num or 0))
        end
        local attr_list, max_attr_num = item:GetAttrList()
        self.SelectList:SetData(self:GetAttrList(attr_list, max_attr_num))
        UH.SetIcon(self.ImgSetIcon, item:IconId())
        UH.SpriteName(self.ImgSetFrame, "YuanPinZhiKuang" .. item:Color())
        UH.SetText(self.TxtAccrssorySuit, item:SuitName())
    else
        self.SelectList:SetData({})
    end
end

function AccrssoryInheritItem:OnSelectClick()
    AccrssoryData.Instance.accrssory_inherit_data.inherit_type = self.data.inherit_type
    if self.data.inherit_type == AccrssoryInheritType.Now then
        PublicPopupCtrl.Instance:Center(Language.Accrssory.InheritClickLeftTip)
    elseif self.data.inherit_type == AccrssoryInheritType.Material then
        if TableIsEmpty(AccrssoryData.Instance.accrssory_inherit_data.now) then
            PublicPopupCtrl.Instance:Center(Language.Accrssory.InheritNowSelectEmpty)
            return
        end
        -- 材料界面才能点击打开界面
        ViewMgr:OpenView(AccrssorySelectView)
    end
end

function AccrssoryInheritItem:GetAttrList(attr_list, max_attr_num)
    local t = {}
    for i = 1, max_attr_num do
        local t1 = {}
        t1.index = i
        t1.no_value = attr_list[i].attr_value == 0
        if not t1.no_value then
            t1.attr = CommonStruct.AttrStruct(attr_list[i].attr_type, attr_list[i].attr_value)
            t1.attr.attr_value = t1.attr.attr_percent_value
        end
        -- 材料那边需要屏蔽掉空的词条
        if self.data.inherit_type == AccrssoryInheritType.Material then
            if not t1.no_value then
                table.insert(t, t1)
            end
        else
            table.insert(t, t1)
        end
    end
    return t
end

-- 外部调用，设置对应属性条的状态
function AccrssoryInheritItem:AutoAttrClick(index, value)
    local mono = self.SelectList:GetItem(index)
    mono.Toggle.isOn = value
end

function AccrssoryInheritItem:OnAttrClick(value, mono)
    local data = mono:GetData()
    local get_count = function(list)
        local count = 0
        for k, v in pairs(list) do
            if v > 0 then
                count = count +1
            end
        end
        return count
    end
    -- 这里要加限制不给点到超过词条限制数量
    local select_count, can_inherit_config, can_inherit_num = 0, 0, 0
    if not TableIsEmpty(AccrssoryData.Instance.accrssory_inherit_data.material) then
        select_count = get_count(self.now_select_data)
        can_inherit_config = Cfg.AccrssoryCanInheritCount(AccrssoryData.Instance.accrssory_inherit_data.material:GetHasAttrNum())
        can_inherit_num = can_inherit_config.chuancheng_entry_num
    end
    if self.data.inherit_type == AccrssoryInheritType.Now then
        if value then
            if TableIsEmpty(AccrssoryData.Instance.accrssory_inherit_data.material) then
                mono.Toggle.isOn = false
                PublicPopupCtrl.Instance:Center(Language.Accrssory.InheritMaterialEmpty)
            elseif select_count >= can_inherit_num then
                mono.Toggle.isOn = false
                PublicPopupCtrl.Instance:Center(Language.Accrssory.InheritOverMax)
            else
                self.now_select_data[data.index] = value and 1 or 0
            end
        else
            self.now_select_data[data.index] = value and 1 or 0
        end
    elseif self.data.inherit_type == AccrssoryInheritType.Material then
        if value and select_count >= can_inherit_num then
            mono.Toggle.isOn = false
            PublicPopupCtrl.Instance:Center(Language.Accrssory.InheritOverMax)
            return
        else
            self.now_select_data[data.index] = value and 1 or 0
        end
    end
end

function AccrssoryInheritItem:GetAllSelect()
    return self.now_select_data
end

function AccrssoryInheritItem:ResetAllSelect()
    for i = 1, ACCRSSORY_ATTR_MAX do
        self.now_select_data[i] = 0
    end
end

function AccrssoryInheritItem:PlayEffect()
    for k, v in pairs(self.SelectList.item_list) do
        v:PlayEffect()
    end
end

--------------AccrssoryInheritAttrItem--------------
AccrssoryInheritAttrItem = AccrssoryInheritAttrItem or DeclareMono("AccrssoryInheritAttrItem", UIWidgetBaseItem)
function AccrssoryInheritAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:StopEffect()
    self.Toggle.isOn = false
    self.AccrssoryAttrItem:SetObjActive(not data.no_value)
    self.AccrssoryAttrItem:SetData(data.attr or {})
end

function AccrssoryInheritAttrItem:PlayEffect()
    self:StopEffect()
    if self.Toggle.isOn then
        self.effect_handler = self.EffectTool:Play("4165092")
    end
end

function AccrssoryInheritAttrItem:StopEffect()
    if self.effect_handler then
        self.EffectTool:Stop(self.effect_handler)
    end
end