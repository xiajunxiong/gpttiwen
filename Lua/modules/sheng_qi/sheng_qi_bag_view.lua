-- 圣器背包
ShengQiBagView = ShengQiBagView or DeclareView("ShengQiBagView", "sheng_qi/sheng_qi_bag_view")
function ShengQiBagView:ShengQiBagView()
    self.Board:SetData(self:GetType(),Language.ShengQi.Title4,Vector2.New(1044, 639))
end

-- >>>>> 圣器列表背包
ShengQiListBag = ShengQiListBag or DeclareMono("ShengQiListBag", UIWFlushPanel)
function ShengQiListBag:ShengQiListBag()
    self.data = ShengQiData.Instance
    self.data_cares = {
        {data = self.data.sheng_qi_list, func = self.FlushPanel, init_call = false}
    }

    GuideManager.Instance:RegisterGetGuideUi("ShengQiListBagItem",function ()
        local list_data = self.data:BagSortScore()
        if TableIsEmpty(list_data) then
            GuideManager.Instance:ForceStop()
            return nil, nil
        end
        return self.List:GetItemMonoByIndex(1)
    end)

    GuideManager.Instance:RegisterGetGuideUi("ShengQiBagItemUse",function ()
        local mono = self.List:GetItemMonoByIndex(1)
        return mono.FightBtn, function ()
            local data = mono:GetData()
            ShengQiCtrl.Instance:SetShengQiState(data, 2)
        end
    end)
end

function ShengQiListBag:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function ShengQiListBag:FlushPanel()
    if not self.list_data then
        self.list_data = self.data:BagSortScore(table.rearrange(self.data.sheng_qi_list))
        table.insert(self.list_data, {})
    else
        local t = {}
        for i,v in pairs(self.list_data) do
            if v.index then
                t[i] = self.data:GetShengQiByIndex(v.index)
            end
        end
        table.insert(t, {})
        self.list_data = t
    end

    self.List:SetData(self.list_data)

    self.NoneObj:SetActive(TableIsEmpty(self.list_data))
end

ShengQiListBagItem = ShengQiListBagItem or DeclareMono("ShengQiListBagItem", UIWidgetBaseItem)
function ShengQiListBagItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local is_empty = TableIsEmpty(data)
    self.Show:SetActive(not is_empty)
    self.AddObj:SetActive(is_empty)
    if is_empty then
        return
    end

    self.ItemCell:SetData(data)
    UH.SetText(self.ItemName, data:QuaName(true))
    UH.SetText(self.TalenSkillNames, data:TalenSkillNames())
    local init_list = data:GetUnlockInitativeSkillList()
    self.SkillList:SetData(init_list)
    self.StateSelector:SetData(data)
end

function ShengQiListBagItem:OnAddClick()
    ShengQiCtrl.GetShengQiJump()
end




-- >>>>> 圣器格子背包
ShengQiBag = ShengQiBag or DeclareMono("ShengQiBag", UIWFlushPanel)
function ShengQiBag:ShengQiBag()
    self.data = ShengQiData.Instance
    self.data_cares = {
        {data = self.data.sheng_qi_list, func = self.FlushPanel, init_call = false}
    }
   
    self.type_list = {
        {text = Language.ShengQi.QuaType[0], type = SHENG_QI_TYPE.COMMON, qua = ItemColor.None},
        {text = Language.ShengQi.QuaType[3], type = SHENG_QI_TYPE.COMMON, qua = ItemColor.Blue},
        {text = Language.ShengQi.QuaType[4], type = SHENG_QI_TYPE.COMMON, qua = ItemColor.Purple},
        {text = Language.ShengQi.QuaType[5], type = SHENG_QI_TYPE.COMMON, qua = ItemColor.Orange},
        {text = Language.ShengQi.QuaType[6], type = SHENG_QI_TYPE.COMMON, qua = ItemColor.Red},
    }

    GuideManager.Instance:RegisterGetGuideUi("ShengQiBagItem",function ()
        local list_data = self.data:GetBagShowList(self.cur_type_data.type, self.cur_type_data.qua)
        if TableIsEmpty(list_data) then
            GuideManager.Instance:ForceStop()
            return nil, nil
        end
        return self.List:GetItemMonoByIndex(1),function ()
            self:OnBagItemClick(self.List:GetItemMonoByIndex(1):GetData())
        end
    end)
end

function ShengQiBag:Awake()
    UIWFlushPanel.Awake(self)
    self.cur_type_data = self.type_list[1]
    self.TypeList:SetData(self.type_list)

    self:FlushPanel()
end

function ShengQiBag:GetList()
    local list_data = self.data:GetBagShowList(self.cur_type_data.type, self.cur_type_data.qua)
    return list_data
end

function ShengQiBag:FlushPanel()
    if TableIsEmpty(self.cur_type_data) then
        return
    end
    UH.SetText(self.SelTypeText, self.cur_type_data.text)
    local list_data = self:GetList()
    local bag_list_data = {}
    for i,v in pairs(list_data) do
        local _data = {index = i, item = v, sel_index = -1}
        if self.sel_data then
            _data.sel_index = self.sel_data.index
        end
        table.insert(bag_list_data, _data)
    end
    local min_count = self:GetMinCount(#list_data)
    if #list_data < min_count and #list_data < ShengQiCfg.MAX_FUSHI_COUNT then
        local is_add = #list_data < ShengQiCfg.MAX_FUSHI_COUNT
        for i = #list_data + 1, min_count do
            local _data = {index = i, sel_index = -1}
            _data.item = Item.Create({item_id = 0})
            if is_add then
                _data.show_add_item = true
                is_add = false
            end
            if self.sel_data then
                _data.sel_index = self.sel_data.index
            end
            table.insert(bag_list_data, _data)
        end
    end
    self.List:SetData(bag_list_data)
end

function ShengQiBag:GetMinCount(num)
    if num < self.MinCount then
        return self.MinCount
    else
        local n = num % 6
        return num + 6 - n
    end
end

function ShengQiBag:OnTypeClick(data)
    self.sel_data = nil
    self:OnCloseSelClick()
    self.cur_type_data = data
    self:FlushPanel()
end

function ShengQiBag:OnCloseSelClick()
    self.SelToggle.isOn = false
end

function ShengQiBag:OnBagItemClick(data)

    if self.sel_data and data.index == self.sel_data.index then
        --self.sel_data = nil
    else
        self.sel_data = data
    end
    self:FlushPanel()

    if data.item.item_id and data.item.item_id > 0 then
        local item = data.item
        local item_type = item:Type()
        if item_type == ItemType.ShengQi then
            ShengQiCtrl.ClickShengQi(data.item)
        elseif item_type == ItemType.FuShi then
            ShengQiCtrl.ClickFuShi(data.item)
        end
    end
end


ShengQiBagItem = ShengQiBagItem or DeclareMono("ShengQiBagItem", UIWidgetBaseItem)
function ShengQiBagItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.SelImg:SetActive(data.index == data.sel_index)
    self.Cell:SetData(data.item)
    self.UpFlag:SetActive(false)
    if data.item.item_id > 0 and data.item:Type() == ItemType.FuShi then
        local sheng_qi = ShengQiData.Instance:GetSelShengQi()
        if not TableIsEmpty(sheng_qi) then
            local remind = FuShiData.Instance:Remind(data.item,sheng_qi)
            self.UpFlag:SetActive(remind > 0)
        end
    end

    if self.AddItem then
        self.AddItem:SetActive(data.show_add_item)
    end
end

function ShengQiBagItem:OnAddItemClick()
    MarketData.Instance:SetJumpIndex(56100)
    ViewMgr:OpenViewByKey(Mod.Mall.Market)
end