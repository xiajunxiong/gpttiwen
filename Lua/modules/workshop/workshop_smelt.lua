WorkshopSmelt = WorkshopSmelt or DeclareMono("WorkshopSmelt", UIWFlushPanel)
function WorkshopSmelt:WorkshopSmelt()
    self.send_data = {
        role_type =0,
        replacer_grid = 0,
        replacer_voc_index = nil,
        bereplacer_grid = 0,
        bereplacer_voc_index = nil
    }
    self.cur_equip_data = nil
    self.select_equip_data = nil
    self.cur_attr_data = nil
    self.workshop_data = WorkshopData.Instance
    self.is_same = false
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.equipment_list, func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.jewelry_list, func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.equipment_voc_list, func = self.FlushEquip, init_call = false}
    }
    self.minA = 999999999999999
    self.cur_index = nil
end
function WorkshopSmelt:onFlush()
    self:FlushEquip(nil,true)
end
function WorkshopSmelt:FlushEquip(_,onflush)
    --这里还要加上灵饰
    local new_list = self.workshop_data:GetSmeltEquipList()
    self.EquipList:SetData(new_list)
    if table.nums(new_list) == 0 then
        self.Content:SetActive(false)
        self.GuaJian:SetActive(true)
    else
        self.Content:SetActive(true)
        self.GuaJian:SetActive(false)
    end
    if self.cur_index == nil and self.workshop_data.smelt_select.cur_equip_data then
        self.EquipList:ClickItemData(self.workshop_data.smelt_select.cur_equip_data)
    else
        self.EquipList:ClickItem(self.cur_index or 1)
    end
end
function WorkshopSmelt:OnClickItem(data)
    local temp_index = self.EquipList:GetSelectedIndex(data)
    if self.cur_index then
        self.is_same = self.cur_index == temp_index
    end
    --显示其属性
    self.cur_index = self.EquipList:GetSelectedIndex(data)
    local _, attr = data:IncAttr()
    table.sort(attr, DataSorter.KeyLowerSorter("index"))
    self.AttrList1:SetData(attr)
    self.EquipCell:SetData(data)
    UH.SetText(self.LblName, data:QuaName(true))
    self.send_data.replacer_grid = data.index
    self.cur_equip_data = data
    self.cur_attr_data = attr
    self.send_data.change_voc_type = Item.GetBigType(data.item_id) == ITEM_BIG_TYPE.EQUIPMENT and 0 or 1
    self:FlushBag()
    --刷新右侧列表
    self.workshop_data.smelt_select.cur_equip_data = data

    self.send_data.role_type = AnotherData.Instance:CheckEquipmentUniqueId(data,data:IsEquip()) and 0 or 1
    --定一下规则
    --如果不是炼就不推荐
    --左边存在空属性（存在不同属性）
    --左边最小评分比右边最大评分小
    --首先肯定知道的是右侧最大值
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end
function WorkshopSmelt:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.workshop_data.smelt_select.cur_equip_data = nil
    if self.handle_run1 then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run1)
        self.handle_run1 = nil
    end
    if self.handle_run2 then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run2)
        self.handle_run2 = nil
    end
end
function WorkshopSmelt:FlushBag()
    if self.handle_run1 then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run1)
        self.handle_run1 = nil
    end
    local list
    if self.send_data.change_voc_type == 0 then
        list = BagData.Instance:GetGridByEquipType(self.cur_equip_data:EquipType(), self.cur_equip_data:LimitLv())
    else
        list = BagData.Instance:GetGridByJewelryType(self.cur_equip_data:JewelryType(), self.cur_equip_data:LimitLv())
    end
    local flag = next(list) == nil or self.workshop_data:CheckIsAllLock(self.cur_equip_data)
    self.NoCanUse:SetActive(flag)
    self.CanUse:SetActive(not flag)
    if flag == true then
        return
    end
    --这里还要排序
    table.sort(list, function (a,b)
        local num1 = self.workshop_data:CheckSmeltEquipAttrNum(self.cur_equip_data,a)
        local num2 = self.workshop_data:CheckSmeltEquipAttrNum(self.cur_equip_data,b)
        return num1 > num2
    end )
    --比较list好了 少了就选1
    local old_data = self.EquipList2:GetData()
    if old_data and list and table.nums(old_data) ~= table.nums(list) then
        -- LogError("重新选中对象")
        self.select_index = 1
    end
    --对整个列表进行 炼字的检查
    --左边有空显示
    --左右有相同判断大小显示
    --左边最小和右边最大比较
    self.EquipList2:SetData(list)
    local index = self.is_same == false and 1 or (self.select_index or 1)
    self.EquipList2:ClickItem(index)
    if index == 1 then
        self.ScrollRect.verticalNormalizedPosition = 1
    end
    self.send_data.replacer_voc_index = nil
end
function WorkshopSmelt:ClickPage(index)
    if self.select_index == nil then return end
    local list
    if self.send_data.change_voc_type == 0 then
        list = BagData.Instance:GetGridByEquipType(self.cur_equip_data:EquipType(), self.cur_equip_data:LimitLv())
    else
        list = BagData.Instance:GetGridByJewelryType(self.cur_equip_data:JewelryType(), self.cur_equip_data:LimitLv())
    end
    local max_index = table.nums(list)
    self.select_index = self.select_index + index
    if self.select_index == 0 then
        self.select_index = self.select_index - index
    elseif self.select_index >= max_index then
        self.select_index = max_index
    end
    local temp = self.select_index > (max_index / 2) and self.select_index or (self.select_index - 1)
    self.ScrollRect.horizontalNormalizedPosition = temp / max_index
    self.EquipList2:ClickItem(self.select_index)
end
function WorkshopSmelt:OnClickEquip(data)

    --local a = self.workshop_data:CheckSmeltEquipAttrNum(self.cur_equip_data,data)

    local _, attr = data:IncAttr()
    --重新排序attr
    table.sort(attr,DataSorter.KeyLowerSorter("index"))
    self.AttrList2:SetData(attr)
    self.send_data.bereplacer_grid = data.index
    self.send_data.bereplacer_voc_index = nil
    self.select_index = self.EquipList2:GetSelectedIndex(data)
    self.select_equip_data = data
    self.select_attr_data = attr

    --对比是否显示 推荐左侧和右侧
    self:FlushTuiJian(true)
end
function WorkshopSmelt:FlushTuiJian(auto_click)
    for _, value in ipairs(self.AttrList1.item_list) do
        value:UnactiveLight()
    end
    local prof_type = RoleData.Instance:GetRoleProfType()
    self.select_min_index = 0
    self.select_max_index = 0
    local logic_a = false
    local exclude = {}
    --用红点的逻辑吧
    for index, value in ipairs(self.cur_attr_data) do
        for index2, value2 in ipairs(self.select_attr_data) do
            if value.attr_value > 0 and value.attr_type == value2.attr_type 
                and value2.attr_value > 0 and value.lock == 1 and value2.lock == 1 
                and self.workshop_data:CheckEquipVocIsLock(nil,index) then
                if value2.attr_value > value.attr_value then
                    logic_a = true
                    self.select_min_index = index
                    self.select_max_index = index2
                else
                    --排除计算的类型
                    exclude[value.attr_type] = 1
                end
            end
        end
    end
    if logic_a == false then
        --找出空和最大
        local logic_b = false
        local priority = 0
        local priorityB = 0
        self.valueB = 0
        for index, value in ipairs(self.select_attr_data) do
            if value.attr_value ~= 0 and exclude[value.attr_type] ~= 1 and value.lock == 1 
                and self.workshop_data:CheckEquipVocIsLock(nil,index) then
                local cfg = Cfg.SmeltAttrPri(prof_type, value.attr_type)
                local temp = cfg.priority * value.attr_value
                priority = math.max(temp, priority)
                if priority == temp then
                    -- self.valueB = value.attr_value
                    priorityB = priority
                    self.select_max_index = index
                end
            end
        end
        for index, value in ipairs(self.cur_attr_data) do
            if value.attr_value == 0 and value.lock == 1 
                and self.workshop_data:CheckEquipVocIsLock(nil,index) then
                logic_b = true
                self.select_min_index = index
                break
            end
        end
        if logic_b == false then
            priority = 9999999
            for index, value in ipairs(self.cur_attr_data) do
                if value.attr_value ~= 0 and exclude[value.attr_type] ~= 1 and value.lock == 1  
                    and self.workshop_data:CheckEquipVocIsLock(nil,index) then
                    local cfg = Cfg.SmeltAttrPri(prof_type, value.attr_type)
                    if cfg.priority ~= 0 then
                        local temp = cfg.priority * value.attr_value
                        priority = math.min(temp, priority)
                        if priority == temp then--越小越被推荐替换
                            if priorityB >= priority  then -- and self.valueB > value.attr_value then
                                self.select_min_index = index
                            end
                        end
                    end
                end
            end
        end
    end

    if self.select_min_index > 0 and self.select_max_index > 0 then
        -- LogError("选中权重最低的属性")
        if self.handle_run2 then
            Runner.Instance:RemoveRunUntilTrue(self.handle_run2)
            self.handle_run2 = nil
        end
        self.handle_run2 = Runner.Instance:RunUntilTrue(function ()
            if self.AttrList1:SetupCompleted() then
                if self.AttrList1:GetItem(self.select_min_index) then
                    self.AttrList1:GetItem(self.select_min_index):ActiveLight()
                    if auto_click == true then
                        self.AttrList1:GetItem(self.select_min_index):Click()
                    end
                end
                return true
            end
            return false
        end)

        if self.handle_run1 then
            Runner.Instance:RemoveRunUntilTrue(self.handle_run1)
            self.handle_run1 = nil
        end
        self.handle_run1 = Runner.Instance:RunUntilTrue(function ()
            if self.AttrList2:SetupCompleted() then
                self.AttrList2:GetItem(self.select_max_index):ActiveLight()
                if auto_click == true then
                    self.AttrList2:GetItem(self.select_max_index):Click()
                end
                return true
            end
            return false
        end)
        -- self.AttrList1:GetItem(self.select_min_index):ActiveLight()
    end
end
function WorkshopSmelt:OnClickAttr(item)
    local data = item:GetData()
    if data.lock == 0 then return  end
    self.cur_attr = data
    self.send_data.replacer_voc_index = data.index - 1
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end
function WorkshopSmelt:OnClickAttr2(item)
    local data = item:GetData()
    if data.lock == 0 then return  end 
    self.select_attr = data
    self.send_data.bereplacer_voc_index = data.index - 1

    if data.attr_value == 0 and data.lock == 1 then
        PublicPopupCtrl.Instance:Center(Language.Workshop.SmeltNoneAttrTips)
    end 
    for _, value in ipairs(self.AttrList1.item_list) do
        value:UnactiveLight()
    end
    -- 选中一个属性时 如果有相同属性就不推荐 判断左侧是否有空 空显示推荐，（如果没有空就遍历，0不推荐，推荐优先度最大的）
    --当前装备
    --右边显示最好的推荐，左边选择最菜的被替
    --判断选中和左边是否有相同属性
    local prof_type = RoleData.Instance:GetRoleProfType()
    for index, value in ipairs(self.cur_attr_data) do
        if data.attr_value ~= 0 and value.attr_type == data.attr_type then
            local cfg = Cfg.SmeltAttrPri(prof_type, value.attr_type)
            if data.attr_value --[[ * cfg.priority ]] > value.attr_value --[[ * cfg.priority ]] and value.lock == 1 
                and self.workshop_data:CheckEquipVocIsLock(nil,index) then
                if self.AttrList1:GetItem(index) then
                    self.AttrList1:GetItem(index):ActiveLight()
                end
            end
            return
        end
    end
    if data.attr_value == 0 then
        return
    end
    for index, value in ipairs(self.cur_attr_data) do
        if value.attr_value == 0 and value.lock == 1 
            and self.workshop_data:CheckEquipVocIsLock(nil,index) then
            if self.AttrList1:GetItem(index) then
                self.AttrList1:GetItem(index):ActiveLight()
            end
            return
        end
    end
    --[[ local priority = 9999999
    local select_idx = 0
    for index, value in ipairs(self.cur_attr_data) do
        if value.attr_value ~= 0 then
            local cfg = Cfg.SmeltAttrPri(prof_type, value.attr_type)
            if cfg.priority ~= 0 then
                local temp = cfg.priority * value.attr_value
                priority = math.min(temp, priority)
                if priority == temp then--越小越被推荐替换
                    select_idx = index
                    self.minA = priority
                end
            end
        end
    end ]]
    self:FlushTuiJian()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end
function WorkshopSmelt:OnClickChange()
    if MainViewData.Instance:BaseData().is_battle == true then
        PublicPopupCtrl.Instance:Center(ErrorText[107])
        return
	end
    
    -- LogError(self.send_data)
    if self.send_data.replacer_voc_index == nil or self.send_data.bereplacer_voc_index == nil then
        PublicPopupCtrl.Instance:Center(ErrorText[108])
        return
    end
    if self.cur_attr.attr_value == 0 then
        WorkshopCtrl.Instance:ChangeEquipVocReq(self.send_data)
        if self.EffectTool.isActiveAndEnabled then 
            self.EffectTool:Play(10010221)
        end 
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.JinJie2)
        return
    end
    if self.select_attr.attr_value == 0 then
        return
    end
    local cur_lock_data = BagData.Instance.equipment_voc_list
    local cur_lock_list = self.send_data.change_voc_type == 0 and cur_lock_data.equipment_list or cur_lock_data.jewelry_list
    if 1 == cur_lock_list[self.send_data.replacer_grid + 1].is_lock[self.send_data.replacer_voc_index + 1] then 
        PublicPopupCtrl.Instance:Center(ErrorText[123])
        return 
    end 
    local color1 = QualityColorStr[self.cur_attr.color]
    local color2 = QualityColorStr[self.select_attr.color]
    local value1 = DataHelper.IsPercent(self.cur_attr.attr_type) and Percent(self.cur_attr.attr_value) or self.cur_attr.attr_value
    local value2 = DataHelper.IsPercent(self.select_attr.attr_type) and Percent(self.select_attr.attr_value) or self.select_attr.attr_value
    local attr1 = Language.Common.AttrList[self.cur_attr.attr_type]
    local attr2 = Language.Common.AttrList[self.select_attr.attr_type]
    local content = string.format(Language.Workshop.SmeltTip[1], color2,attr2,value2,color1,attr1,value1)
    if self.cur_equip_data:EquipLevel() ~= self.select_equip_data:EquipLevel() then
        content = string.format(Language.Workshop.SmeltTip[2], color2,attr2,value2,color1,attr1,value1,color2,attr2,value2)
    end
    local info = {
	content = content,
	confirm = {
        func = function ()
            -- 不明报空
            if self.send_data.replacer_voc_index == nil or self.send_data.bereplacer_voc_index == nil then
                PublicPopupCtrl.Instance:Center(ErrorText[108])
                PublicPopupCtrl.Instance:CloseDialogTips()
                return
            end
            WorkshopCtrl.Instance:ChangeEquipVocReq(self.send_data)
            PublicPopupCtrl.Instance:CloseDialogTips()
            if self.EffectTool.isActiveAndEnabled then 
                self.EffectTool:Play(10010221)
            end 
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.JinJie2)
        end,
	}}
    PublicPopupCtrl.Instance:DialogTips(info)
end

function WorkshopSmelt:OnClickTip()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[22].desc})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2) 
end



WorkshopSmeltItem = WorkshopSmeltItem or DeclareMono("WorkshopSmeltItem", UIWidgetBaseItem)
function WorkshopSmeltItem:WorkshopSmeltItem()
end

function WorkshopSmeltItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end
function WorkshopSmeltItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.EquipCell:SetData(data)
    UH.SetText(self.LblName, data:Name())
    UH.SetText(self.LblType, data:TypeDesc())
    UH.SetText(self.LblLevel, data:LimitLv()..Language.Common.Level)--string.format("Lv.%d", data:EquipLevel()))
    --判断是否显示红点
    local num = 0 
    if data:IsEquip() then
        num = WorkshopData.Instance:CheckSmeltEquipAttr(data)
    else
        num = WorkshopData.Instance:CheckSmeltJewelryAttr(data)
    end
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(154,36,0))
    end
    self.red_point:SetNum(num)
    self.HsTag:SetActive(not AnotherData.Instance:CheckEquipmentUniqueId(data,data:IsEquip()))
end
WorkshopSmeltEquipItem = WorkshopSmeltEquipItem or DeclareMono("WorkshopSmeltEquipItem", UIWidgetBaseItem)
function WorkshopSmeltEquipItem:WorkshopSmeltEquipItem()
    self.workshop_data = WorkshopData.Instance
end
function WorkshopSmeltEquipItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local num = self.workshop_data:CheckSmeltEquipAttrNum(self.workshop_data.smelt_select.cur_equip_data, data)
    self.GoodObj:SetActive(num ~= 0)
    self.SelectObj:SetActive(false)
    self.Cell:SetData(data)
end
function WorkshopSmeltEquipItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end
WorkshopSmeltAttrItem = WorkshopSmeltAttrItem or DeclareMono("WorkshopSmeltAttrItem", UIWidgetBaseItem)
function WorkshopSmeltAttrItem:WorkshopSmeltAttrItem()
    self.workshop_data = WorkshopData.Instance
    self:uncareData()
    self:careData(BagData.Instance.equipment_voc_list, BindTool.Bind(self.FlushVocLock,self))
end
function WorkshopSmeltAttrItem:SetData(data)
    self.SelectObj:SetActive(false)
    self.SelectObj2:SetActive(false)
    self.LightObj:SetActive(false)
    self.UpObj:SetActive(false)
    if data ~= nil then
        self.PointObj:SetActive(DataHelper.IsPoint(data.attr_type) and not data.is_lock )
    else
        self.PointObj:SetActive(false)
    end
    local lang = Language.Common
    UH.SetText(
        self.LblAttr,
        data == nil and lang.NonAttr or
            (data.attr_value == 0 and lang.NonAttr or
                string.format(
                    lang.Xstr,
                    QualityColorStrSmelt[data.color],

                    string.format(lang.IncAttr, lang.AttrList[data.attr_type],
                    DataHelper.IsPercent(data.attr_type) and Percent(data.attr_value) or data.attr_value)
                ))
    )

    self.LockObj:SetActive(data.lock == 0)
    self.Toggle.enabled = data.lock == 1
    if data.lock == 0 then 
        self.LblAttr.text = ""
        local list_ = self.workshop_data:GetLockInfo()
        local level_ = (list_[data.index].level - 1 ) * 10
        self.LockLimit.text = string.format(Language.Workshop.SmeltLockAttrLevelUnlock,level_ > 0 and level_ or 1 )
    end 

    UIWidgetBaseItem.SetData(self, data)

    self.is_init = true
    self:FlushVocLock()
end

function WorkshopSmeltAttrItem:OnRelease()
    self:uncareData()
    UIWidgetBaseItem.OnRelease(self)
end 

function WorkshopSmeltAttrItem:ActiveLight()
    self.LightObj:SetActive(true)
end
function WorkshopSmeltAttrItem:UnactiveLight()
    self.LightObj:SetActive(false)
end
function WorkshopSmeltAttrItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end
function WorkshopSmeltAttrItem:OnClickLockRed()
    local equip_data = self.workshop_data.smelt_select.cur_equip_data
    local cur_data = BagData.Instance.equipment_voc_list

    BagCtrl.Instance:SendEquipmentOperaReq(
        {
            oper_type = EQUIP_OPERA_TYPE.OPERA_TYPE_VOC_LOCK,
            param_1 = equip_data:IsEquip() and 1 or 2,
            param_2 = equip_data.index,
            param_3 = self.data.index - 1,
            param_4 = self.is_lock_red == 1 and 0 or 1,
        }
    )
end

function WorkshopSmeltAttrItem:FlushVocLock()    
    local equip_data = self.workshop_data.smelt_select.cur_equip_data
    local cur_data = BagData.Instance.equipment_voc_list

    if self.is_init == nil then return end 
    if equip_data == nil or cur_data == nil  then return end 

    if self.LockRedObj then self.LockRedObj:SetActive(self.data.lock == 1) end 

    local flag_is_equip = equip_data:IsEquip()

    self.is_lock_red = flag_is_equip and cur_data.equipment_list[equip_data.index+1].is_lock[self.data.index] 
        or cur_data.jewelry_list[equip_data.index+1].is_lock[self.data.index]
    if self.LockRedLight then 
        self.LockRedLight:SetActive(self.is_lock_red == 1)
    end 
end