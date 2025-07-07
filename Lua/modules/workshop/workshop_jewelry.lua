WorkshopJewelry = WorkshopJewelry or DeclareMono("WorkshopJewelry", UIWFlushPanel)
function WorkshopJewelry:WorkshopJewelry()
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.lang = Language.WorkShop
    self.is_not_enough_coin = false
    self.is_not_enough_level = false
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushRightItem, init_call = false},
        {data = self.data.complete_item, func = self.FlushPreView , init_call = false},
    }
end
function WorkshopJewelry:Awake()
    UIWFlushPanel.Awake(self)

    -- local data = self.data:GetJweelryAccorCfg()
    -- self:OnClickItem(data[1].children[1])
end

function WorkshopJewelry:OnEnable()
    UIWFlushPanel.OnEnable(self)

    if not self.show_complete then 
        self:onFlush()
    end 
end 
function WorkshopJewelry:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)
    TimeHelper:CancelTimer(self.handle_delay3)
    self.show_complete = false
end
function WorkshopJewelry:onFlush()
    local data = self.data:GetJweelryAccorCfg(true)--self.data:JewelryShowList()
    -- self.JewelryList:SetData(data)
    -- self.JewelryList:ClickItem(1)
    self.JewelryAcc:SetData(data,true)
    local mark_pos = self.data:GetLockJewelryRecipe()

    if mark_pos == nil then 
        local level = RoleData.Instance:GetRoleLevel()
        local auto_select = math.floor(level/10)
        if auto_select < 3 then auto_select = 3 end 
        self.JewelryAcc:ClickChild(data[auto_select - 2].children[1])
        self.JewelryAcc:JumpVerticalIndex(1,45,auto_select - 2,56) 
    else 
        self.JewelryAcc:ClickChild(data[mark_pos.parent].children[mark_pos.child])
        self.JewelryAcc:JumpVerticalIndex(mark_pos.child,45,mark_pos.parent,56)
    end 
    self.data:ClearLockJewelry()

end
function WorkshopJewelry:FlushRightItem()
    if self.target_info then
        self:OnClickItem(self.target_info)
    end
end
function WorkshopJewelry:OnClickItem(data)
    local item_id = 0
    local item = nil
    self.target_info = data
    self.seq = data.seq
    self.is_not_enough_stuff = false
    self.enough_stuff_item = nil
    -- UH.SetIcon(self.IconList[6], data.icon_id)
    self.StuffsObj:SetActive(true)
    self.QuaRandObj:SetActive(true)
    for i = 1, 2 do
        local item = Item.New()
        local item_id = data["stuff_id_" .. i]
        item_id = item_id == nil and 0 or item_id
        if item_id > 0 and not data.is_lock then
            item.item_id = item_id
            UH.SetText(self.NameList[i], item:Name())
            UH.SetText(self.NumList[i], string.format("%d/%d", item:Num(), data["stuff_num_" .. i]))
            local flag = item:Num() < data["stuff_num_" .. i]
            UH.Color(self.NumList[i], flag and COLORS.Red8 or COLORS.Green3)
            self.is_not_enough_stuff = flag and flag or self.is_not_enough_stuff
            if self.enough_stuff_item == nil then
                self.enough_stuff_item = flag and item or self.enough_stuff_item
            end
            self.NoneList[i]:SetActive(false)
            self.price_obj_list[i]:SetActive(true)
            local config_ = CommerceData.Instance:GetMourinhoCfgById(item_id)

            UH.SetText(self.price_list[i], config_.price)
        else
            UH.SetText(self.NameList[i], "")
            UH.SetText(self.NumList[i], "")
            self.price_obj_list[i]:SetActive(false)
            self.NoneList[i]:SetActive(true)
        end
        self.QuaList[i]:SetActive(item_id > 0)
        self.LockList[i]:SetActive(item_id <= 0)

        UH.SetIcon(self.IconList[i], item:IconId())
        UH.SpriteName(self.QuaSpList[i], JewelryQua[item:Color()])
    end
    self.OneKeyhandler.Interactable = not data.is_lock
    self:FlushEquipView()
    self:FlushEquipRandom()
    self:FlushResult()
    self:FlushPreView()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function WorkshopJewelry:FlushPreEquipView()
    self.EquipedPre:SetActive(false)
    self.EquipedPreEmptyV:SetActive(true)

    self.PreEmptyName.text = self.target_info.name

    local quality_group = self.data:GetQualityRandomGroup(self.target_info.item_random)
    local str = Language.Workshop.JewelryQuaPreStr
    for i = 1,#quality_group do
        str = str .. string.format(Language.Workshop.JewelryQuaPreStr_patch,
        QualityColorStrShallow[quality_group[i].color],Language.Common.ItemColor[quality_group[i].color])

        if i ~= #quality_group then 
            str = str .. "/"
        end 
    end 

    self.PreEmptyQuaStr.text = str

    self.PreEmptyType.text = Language.Common.Jewelry[self.target_info.ornament_type]
    UH.SetIcon(self.PreEmptyIcon, self.target_info.icon_id)
    UH.SpriteName(self.PreEmptyQua, JewelryQua[ItemColor.Blue])
    local pre_attr_info = self.data:GetJewelryPreInfo(self.target_info)
    local attr_list = {}
    for k,v in pairs(pre_attr_info) do 
        local str = string.format("%s   %s~%s", Language.Common.AttrList[k], 
				DataHelper.IsPercent(k) and Percent(v.attr_min) or v.attr_min,
                DataHelper.IsPercent(k) and Percent(v.attr_max) or v.attr_max)
        table.insert( attr_list, str )
    end 
    self.PreEmptyAttr:SetData(attr_list)

    local smelt_num = self.data:GetJewelryMaxSmeltNum(self.target_info)
    local smelt_str = string.format(Language.Workshop.JewelryPreSAttrDesc,smelt_num > 6 and 6 or smelt_num)

    local attr_s_list =  smelt_num > 0 and {[1] = smelt_str } or {}
    self.PreEmptySAttr:SetData(attr_s_list)

    self.GuessSmeltNum:SetActive(smelt_num > 0)
end

function WorkshopJewelry:FlushPreView()
    self.EquipedPreEmpty:SetActive(false)
    -- local info = self.data:ComposeResult()
    -- if info == nil then
        -- self.EquipedPre:SetActive(false)
        -- self.EquipedPreEmpty:SetActive(true )
        self:FlushPreEquipView()
    --     return 
    -- end 
    -- local item = Item.Create(info.item)

    -- local target_sample = self.target_info.ornament_type == Jewelry.GetJewelryType(item.item_id)

    -- if info == nil or not target_sample then 
    --     self:FlushPreEquipView()
    --     return 
    -- end 
    -- self.EquipedPre:SetActive(info ~= nil and target_sample )
    -- self.EquipedPreEmptyV:SetActive(info == nil or not target_sample )

    -- if info ~= nil and item:IsJewelry() and Jewelry.GetJewelryType(item.item_id) == self.target_info.ornament_type then 
    --     self.PreViewSAttrObj:SetActive(false)
    --     self.PreViewAttrObj:SetActive(true)

    --     self.PreViewName.text = item:NameId()
    --     UH.SpriteName(self.PreViewQua, JewelryQua[item:Color()])
    --     UH.SetIcon(self.PreViewIcon, item:IconId())

    --     local base_list = {}
    --     for k,v in pairs(item.param.attr_list) do 
    --         if v.attr_value > 0 then
    --             local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, Language.Common.AttrList[v.attr_type], v.attr_value)
    --             table.insert( base_list, str ) 
    --         end
    --     end 
    --     self.PreViewBaseAttr:SetData(base_list)

    --     local inc_list = {}
    --     local has_inc, _inc_list = item:IncAttr()
    --     if has_inc then 
    --         for i = 1,6 do
    --             if _inc_list[i].attr_value > 0 then 
    --                 local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr4, 
    --                     QualityColorStrShallow[_inc_list[i].color],
    --                     Language.Common.AttrList[_inc_list[i].attr_type],
    --                     DataHelper.IsPercent(_inc_list[i].attr_type) and Percent(_inc_list[i].attr_value) or _inc_list[i].attr_value)

    --                 table.insert( inc_list, str ) 
    --             end 
    --         end
    --     end
    --     self.PreViewIncAttr:SetData(inc_list)
    --     self.PreViewType.text = Language.Common.Jewelry[self.target_info.ornament_type]


    --     self.handle_delay3 = TimeHelper:AddDelayTimer(function ()
    --         if self.PreViewAttrGObj then 
    --             self.PreViewAttrGObj:SetActive(false)
    
    --             self.PreViewAttrGObj:SetActive(true)
    --         end 
    --     end,0.2)
    -- else 

    -- end 
    self.show_complete = true
end

function WorkshopJewelry:OnClickOneKey()
    for i = 1, 2 do
        local data = {}
        local item_id = self.target_info["stuff_id_" .. i]
        local config_ = CommerceData.Instance:GetMourinhoCfgById(item_id)
        data.index = config_.index
        data.conver_type = config_.conver_type
        data.seq = config_.seq
        data.item_id = config_.item_id

        local item = Item.New()
        item.item_id = item_id
        data.make_num = self.target_info["stuff_num_" .. i] - item:Num() 
        if  data.make_num > 0 then 
            ShopCtrl.Instance:SendGoldShopBuyReq(data.index,data.make_num)
        end 
        -- ShoppingCtrl.Instance:SendStoreBuyReq(data)
    end 
end

function WorkshopJewelry:FlushEquipRandom()
    local quality_group = self.data:GetQualityRandomGroup(self.target_info.item_random)
    local cur_fail_time = self.data:GetJewelryQuaInfoTime(self.target_info.item_random)
    for i = 1,4 do 
        self.QualityGroup[i].sizeDelta = Vector2.New(0, 26)
    end 
    for k,v in pairs(quality_group) do
        local rate_t = cur_fail_time * v.fine_tuning_value + v.rate
        self.QualityGroup[v.color -1].sizeDelta = Vector2.New(self.Qualuty_leghth * rate_t / 100, 26)
    end 
end

function WorkshopJewelry:FlushEquipView()
    -- 信息
    self.EquipedEmptyInfo.text = Language.Workshop.JewelryPreViewTips

    -- self.PreViewAttr.text = self.target_info.desc
    -- self.PreViewType.text = Language.Common.Jewelry[self.target_info.ornament_type]
    -- self.PreViewName.text = self.target_info.name
    -- UH.SetIcon(self.PreViewIcon, self.target_info.icon_id)

    local list = BagData.Instance:GetJewelryList()
    local jewelry = list[self.target_info.ornament_type]
    -- self.CurViewAttr.text = self.target_info.desc
    local item = jewelry
    if item then
        self.CurViewType.text = Language.Common.Jewelry[self.target_info.ornament_type]
        self.CurViewName.text = item:NameId()
        UH.SpriteName(self.CurViewQua, JewelryQua[item:Color()])
        UH.SetIcon(self.CurViewIcon, item:IconId())

        local base_list = {}
        for k,v in pairs(jewelry.param.attr_list) do 
            if v.attr_value > 0 then
                local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, Language.Common.AttrList[v.attr_type], v.attr_value)
                table.insert( base_list, str ) 
            end
        end 
        self.CurViewBaseAttr:SetData(base_list)

        local inc_list = {}
        local has_inc, _inc_list = item:IncAttr()
        if has_inc then 
            for i = 1,6 do
                if _inc_list[i].attr_value > 0 then
                    local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr4, 
                        QualityColorStrSmelt[_inc_list[i].color], 
                        Language.Common.AttrList[_inc_list[i].attr_type],
                        DataHelper.IsPercent(_inc_list[i].attr_type) and Percent(_inc_list[i].attr_value) or _inc_list[i].attr_value)
                    -- jewelry.param.attr_extra_inc[i].attr_value)

                    table.insert( inc_list, str ) 
                end 
            end
        end

        self.CurViewIncAttr:SetData(inc_list)
    end
    self.EquipedCur:SetActive(item ~= nil )
    self.EquipedCurEmpty:SetActive(item == nil )

    
    self.handle_delay = TimeHelper:AddDelayTimer(function ()
        if self.CurViewAttrObj then 
            self.CurViewAttrObj:SetActive(false)

            self.CurViewAttrObj:SetActive(true)
        end 
    end,0.2)

end

function WorkshopJewelry:FlushResult()
    local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    local level = RoleData.Instance:GetRoleLevel()
    local need = self.target_info.price
    self.BtnMake.gameObject:SetActive(need > 0)
    self.BtnMake2.gameObject:SetActive(need == 0)
    UH.SetText(self.LblCost, need)
    UH.Color(self.LblCost, coin < need and COLORS.Red8 or COLORS.Green1)
    self.is_not_enough_coin = coin < need
    self.is_not_enough_level = level < self.target_info.level
end
function WorkshopJewelry:OnClickCreate()
    --直接制作和强化制作
    -- seq 数量 是否强化
    if self.is_not_enough_coin then
        PublicPopupCtrl.Instance:Center(ErrorText[21])
        return
    end
    if self.is_not_enough_level then
        PublicPopupCtrl.Instance:Center(ErrorText[23])
        return
    end
    if self.is_not_enough_stuff then
        if self.enough_stuff_item then
            MainOtherCtrl.Instance:GetWayView({item = self.enough_stuff_item})
        else
            PublicPopupCtrl.Instance:Center(ErrorText[22])
        end
        return
    end

    -- self.handle_delay2 = TimeHelper:AddDelayTimer(function ()
    --     self.ctrl:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_MAKE_JEWELTY, self.seq, 0, 0)
    -- end, 0.7)
    self.EffectTool:Play("10010700")
    -- self.EffectTool:Play("10010701")
end


function WorkshopJewelry:OnClickStuff(index)
    if self.target_info == nil then
        return
    end
    local data = Item.New()
    data.item_id = self.target_info["stuff_id_" .. index]
    if data.item_id == 0 or data.item_id == nil then
        return
    end
    local info = {
        item_list = {data}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function WorkshopJewelry:OnClickRecast()
    ViewMgr:OpenView(JewelryRecast)
end

function WorkshopJewelry:CreateEffectEnd()
    self.ctrl:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_MAKE_JEWELTY, self.seq, 0, 0)
end 

JewelrySimpleItem = JewelrySimpleItem or DeclareMono("JewelrySimpleItem", UIWidgetBaseItem)
function JewelrySimpleItem:JewelrySimpleItem()
    self.workshop_data = WorkshopData.Instance
end

function JewelrySimpleItem:SetData(data)
    local level = RoleData.Instance:GetRoleLevel()
    UH.SetText(self.SelectName, Language.Common.Jewelry[data.ornament_type])
    UH.SetText(self.UnselectName, Language.Common.Jewelry[data.ornament_type])
    -- 添加锁定状态
    data.is_lock = level < data.level
    self.Interactable.Interactable = not data.is_lock
    UIWidgetBaseItem.SetData(self, data)
    self:uncareData()
    self:careData(BagData.Instance.item_grid, BindTool.Bind(self.CheckRedPoint,self))
end
function JewelrySimpleItem:CheckRedPoint()
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(120,0,0))
    end
    local num = self.workshop_data:CheckCanJewelry(self.data)
    self.red_point:SetNum(num)
end
function JewelrySimpleItem:OnRelease()
    self:uncareData()
    UIWidgetBaseItem.OnRelease(self)
    if self.red_point then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
end
function JewelrySimpleItem:Click()
    if not self.ItemClick.isActiveAndEnabled then 
        return 
    end 
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
    if self.Toggle --[[ and self.Toggle.isActiveAndEnabled ]] then
        self.Toggle:Trigger()
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

JewelryItem = JewelryItem or DeclareMono("JewelryItem", UIWidgetBaseItem)
function JewelryItem:JewelryItem()
    self.workshop_data = WorkshopData.Instance
end

function JewelryItem:SetData(data)
    local level = RoleData.Instance:GetRoleLevel()
    UH.SetText(self.LblName, data.name)
    UH.SetText(self.LblLevel, string.format(Language.Workshop.Level, data.level))
    local item = Jewelry.New()
    UH.SetText(self.LblType, item:TypeName(data.ornament_type))
    UH.SetIcon(self.Icon, data.icon_id)
    -- 添加锁定状态
    data.is_lock = level < data.level
    self.ZheZhao:SetActive(level < data.level)
    UH.Color(self.LblName, data.is_lock and COLORS.Gray4 or COLORS.Yellow13)
    UH.Color(self.LblLevel, data.is_lock and COLORS.Gray4 or COLORS.Yellow13)
    UH.Color(self.LblType, data.is_lock and COLORS.Gray4 or COLORS.Yellow13)
    self.Interactable.Interactable = not data.is_lock
    UIWidgetBaseItem.SetData(self, data)
    self:uncareData()
    self:careData(BagData.Instance.item_grid, BindTool.Bind(self.CheckRedPoint,self))
end
function JewelryItem:CheckRedPoint()
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(158,38,0))
    end
    local num = self.workshop_data:CheckCanJewelry(self.data)
    self.red_point:SetNum(num)
end
function JewelryItem:OnRelease()
    self:uncareData()
    UIWidgetBaseItem.OnRelease(self)
    if self.red_point then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
end
function JewelryItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
    if self.Toggle --[[ and self.Toggle.isActiveAndEnabled ]] then
        self.Toggle:Trigger()
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end


JewelryParentItem = JewelryParentItem or DeclareMono("JewelryParentItem", UIWidgetBaseItem)
function JewelryParentItem:JewelryParentItem()
    self.workshop_data = WorkshopData.Instance
end
function JewelryParentItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.LblTitle, data.name)

    self:uncareData()
    self:careData(BagData.Instance.item_grid, BindTool.Bind(self.CheckRedPoint,self))
end

function JewelryParentItem:CheckRedPoint()
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(155,20,0))
    end
    local num = self.workshop_data:CheckCanParentJewelry(self.data)
    self.red_point:SetNum(num)
end 

function JewelryParentItem:OnSelect(selected)
    if self.Arrow then
        self.Arrow:SetActive(not selected)
    end

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function JewelryParentItem:OnRelease()
    self:uncareData()
    UIWidgetBaseItem.OnRelease(self)
    if self.red_point then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
end

JewelryAttrCell = JewelryAttrCell or DeclareMono("JewelryAttrCell", UIWidgetBaseItem)
function JewelryAttrCell:JewelryAttrCell()

end 

function JewelryAttrCell:SetData(data)
    self.attr_str.text = data
end 

JewelryIncAttrCell = JewelryIncAttrCell or DeclareMono("JewelryIncAttrCell", UIWidgetBaseItem)

function JewelryIncAttrCell:JewelryIncAttrCell()

end 

function JewelryIncAttrCell:SetData(data)
    self.attr_str.text = data
end 