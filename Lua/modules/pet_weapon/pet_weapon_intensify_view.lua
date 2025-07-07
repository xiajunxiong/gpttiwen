PetWeaponIntensifyView = PetWeaponIntensifyView or DeclareView("PetWeaponIntensifyView", "pet/pet_weapon/pet_weapon_intensify_view")
function PetWeaponIntensifyView:PetWeaponIntensifyView()
    self.BG:SetData(self:GetType(),Language.PetWeapon.Title2, Vector2.New(1088, 646))
end

-- ========== 强化 =========
PetWeaponIntensifyPanel = PetWeaponIntensifyPanel or DeclareMono("PetWeaponIntensifyPanel", UIWFlushPanel)

function PetWeaponIntensifyPanel:PetWeaponIntensifyPanel()
    self.data = PetWeaponData.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
        {data = PetData.Instance.pet_list, func = self.FlushPanel, init_call = false},
        {data = self.data.bag_selected_list, func = self.FlushIntensify, init_call = false},
    }
end

function PetWeaponIntensifyPanel:Awake()
    UIWFlushPanel.Awake(self)
    self:SetSelQua(3)
    self.last_exp = nil
    self.last_level = nil
    self:FlushPanel()
end

function PetWeaponIntensifyPanel:OnDestroy()
    self.data.auto_put_qua.qua = 0
    TimeHelper:CancelTimer(self.time_ht)
    self.data:ClearSelectedBagCell()
    self.data:SetCurCtrlWeapon(nil)
    UIWFlushPanel.OnDestroy(self)
end

function PetWeaponIntensifyPanel:FlushWeapon()
    if self.weapon == nil then
        self.weapon = self.data:GetCurCtrlWeapon()
    elseif self.weapon:PetIndex() then
        if self.weapon:GridPos() == 0 then
            self.weapon = PetData.Instance:GetPetInfo(self.weapon:PetIndex()):Weapon()
        else
            self.weapon = PetData.Instance:GetPetInfo(self.weapon:PetIndex()):Weapon2()
        end
    elseif self.weapon.index >= 0 then
        self.weapon = BagData.Instance:GetItemByIndex(ItemColumnType.PetWeapon, self.weapon.index)
    end
    self.WeaponInfo:SetData(self.weapon)
    self.data:SetCurCtrlWeapon(self.weapon)
end

function PetWeaponIntensifyPanel:FlushPanel()
    self:FlushWeapon()
    self:FlushDisplay()
end

function PetWeaponIntensifyPanel:FlushProgress()
    local weapon = self.weapon
    local num1 = weapon:Exp()
    local num2 = weapon:LevelCfg().exp
    local level = weapon:Level()

    if weapon:IsFullLevel() then
        num2 = num1
    end

    local progress_num
    local stuff_exp = self.data:GetSelListItemPriceExp()
    if stuff_exp > 0 then
        progress_num = (stuff_exp + num1) / num2
    else
        progress_num = num2 == 0 and 1 or num1 / num2
    end

    if weapon:IsFullLevel() then
        self.Progress:SetText("")
    else
        self.Progress:SetText(string.format("%s/%s",(stuff_exp + num1),num2))
    end

    if self.last_exp and self.last_exp ~= num1 then
        if self.last_level and level > self.last_level then
            self.Progress:AddToProgress(1,function ()
                self.Progress:AddToProgress(progress_num)
            end)
        else
            self.Progress:AddToProgress(progress_num)
        end
    else
        self.Progress:SetProgress(progress_num)
    end
    
    self.last_exp = num1
    self.last_level = level
    local x = -211
    if num2 > 0 then
        x = -211 - (num2 - num1)/num2 * 422 + 5
    end
    self.PregressEffectObj:SetLocalPosition(Vector3.New(x,1,0))
end

function PetWeaponIntensifyPanel:FlushDisplay()
    local weapon = self.weapon
    local is_need_break = weapon ~= nil and weapon:IsNeedBreak()
    self.IntensifyInfoObj:SetActive(not is_need_break)
    self.BreakInfoObj:SetActive(is_need_break)
    if weapon == nil then
        return
    end
    if not is_need_break then
        self:FlushIntensify()
    else
        self:FlushBreak()
    end
end

function PetWeaponIntensifyPanel:FlushIntensify()
    local weapon = self.weapon
    if weapon:IsNeedBreak() then
        return
    end
    local stuff_list = self.data:GetIntensifyStuffList()
    
    self.StuffList:SetData(stuff_list)

    local add_exp = self.data:GetSelListItemPriceExp() or 0
    self.stuff_exp = add_exp
    local exp_limit = self.data:GetIntensifyNeedExp(weapon)
    add_exp = add_exp >= exp_limit and exp_limit or add_exp
    self.add_exp = add_exp
    local need_coin = self.data:ExpConsumeCoin(add_exp)
    local up_to_level = self.data:CanAddLevel(weapon, add_exp)
    if add_exp == 0 then
        up_to_level = weapon:Level() + 1
    end
    up_to_level = up_to_level > weapon:MaxLimitLevel() and weapon:MaxLimitLevel() or up_to_level
    UH.SetText(self.CurLevel, weapon:Level())
    self.up_to_level = up_to_level

    local has_coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
    if has_coin >= need_coin then
        UH.SetText(self.CoinNum, need_coin)
    else
        UH.SetText(self.CoinNum, ColorStr(need_coin,COLORSTR.Red8))
    end
    self.need_coin = need_coin

    if add_exp > 0 or up_to_level > weapon:Level() then
        self.AddLevel:SetObjActive(true)
        UH.SetText(self.AddLevel, up_to_level)
        self.LevelAttriObj:SetLocalPosition(Vector3.New(9,45,0))
    else
        self.AddLevel:SetObjActive(false)
        self.LevelAttriObj:SetLocalPosition(Vector3.New(72,45,0))
    end

    -- 属性
    local to_attri_list = self.data:GetIntensifyAddAttri(weapon, up_to_level - weapon:Level())
    local attri_list = self.data:ToAttriItem3Data(weapon:AttriList(), to_attri_list)

    self.AttriList:SetData(attri_list)

    self.Interactor.Interactable = self.data:GetIntensifyNeedExp(weapon) > 0

    self:FlushProgress()
end

-- 刷新属性
function PetWeaponIntensifyPanel:FlushAttri()

end

function PetWeaponIntensifyPanel:FlushBreak()
    local weapon = self.weapon

    local step = weapon:Step()

    UH.SetText(self.CurStep, string.format(Language.PetWeapon.Step,DataHelper.GetDaXie(step)))
    UH.SetText(self.NextStep, string.format(Language.PetWeapon.Step,DataHelper.GetDaXie(step + 1)))
    
    local next_break_cfg = weapon:BreakCfg()
    UH.SetText(self.CurLimit, string.format("%s/%s", weapon:Level(),next_break_cfg.min_level))

    if step + 2 <= weapon:MaxBreakLevel() then
        local cfg = PetWeapon.GetBreakCfg(weapon:QuaType(), step + 2)
        UH.SetText(self.NextLimit, string.format("%s/%s", ColorStr(weapon:Level(),COLORSTR.Green9), cfg.max_level))
    else
        UH.SetText(self.NextLimit, string.format("%s/%s", ColorStr(next_break_cfg.min_level,COLORSTR.Green9), next_break_cfg.max_level))
    end
    

    local break_cfg = weapon:BreakCfg()
    if TableIsEmpty(break_cfg) then
        return
    end
    local stuff_list_data = {}
    self.is_can_break = true
    for i,v in pairs(break_cfg.item_list) do
        local num = Item.GetNum(v.item_id)
        local item = Item.Create({item_id = v.item_id, num = DataHelper.ConsumeNum(num, v.num, false)})
        table.insert(stuff_list_data,item)
        if num < v.num then
            self.is_can_break = false
        end
    end
    self.BreakStuffList:SetData(stuff_list_data)

    local to_attri_list = self.data:GetBreakAddAttri(weapon)
    local attri_list = self.data:ToAttriItem3Data(weapon:AttriList(), to_attri_list)
    self.AttriList:SetData(attri_list)
end

function PetWeaponIntensifyPanel:OnSelToggleEvent(toggle_value)
    if not self.set_sel_list then
        self.set_sel_list = true
        local list_data = self.data:GetStuffSelListData()
        self.SelList:SetData(list_data)
    end
end

function PetWeaponIntensifyPanel:OnSelToggleItemClick(data)
    self.SelToggle.isOn = false
    self:SetSelQua(data.qua)
end

function PetWeaponIntensifyPanel:SetSelQua(qua)
    qua = qua or 3
    local str = ColorStr(Language.PetWeapon.StuffQuaDesc[qua], QualityColorStrShallow[qua])
    UH.SetText(self.SelText, str)
    self.sel_qua = qua
end

function PetWeaponIntensifyPanel:OnBreachReviewClick(value)
    if value == 1 then
        local weapon = self.weapon
        local break_cfg = weapon:BreakCfg()
        if TableIsEmpty(break_cfg) then
            PublicPopupCtrl.Instance:Error(Language.PetWeapon.Error3)
            return
        end
        local item_list = DataHelper.FormatItemList(break_cfg.item_list)
        self.BreachReviewObj:SetActive(true)
        self.BreachReviewList:SetData(item_list)
    else
        self.BreachReviewObj:SetActive(false)
    end
end

function PetWeaponIntensifyPanel:OnIntensifyClick()
    local weapon = self.weapon
    -- if self.data:GetIntensifyNeedExp(weapon) <= 0 then
    --     PublicPopupCtrl.Instance:Error(Language.PetWeapon.Error4)
    --     return
    -- end
    if weapon:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.PetWeapon.Error8)
        return
    end
    if self.add_exp <= 0 then
        PublicPopupCtrl.Instance:Error(Language.PetWeapon.Error5)
        self.BagEffect:Play("4165117")
        return
    end

    if not MallCtrl.IsNotCurrency(CurrencyType.CoinBind, self.need_coin) then
        return
    end

    local exp_limit = self.data:GetIntensifyNeedExp(weapon)

    local intensify_func = function (has_dialog)
        PetWeaponCtrl.Instance:Intensify(weapon, function ()
            if not ViewMgr:IsOpen(PetWeaponIntensifyView) then
                return
            end
            self:PlayAttiChangeEffect()
            if weapon:Level() ~= self.up_to_level then
                self.EffectLogic:PlayUpEffect()
            end
            self.EffectLogic:PlayStuffEffect(self.data:GetSelListCount())
            self.data:ClearSelectedBagCell()
        end,has_dialog)
    end

    if self.stuff_exp > exp_limit and weapon:BaseCfg().level_max == self.up_to_level then
        local str = string.format(Language.PetWeapon.Tip4, self.stuff_exp - exp_limit)
        PublicPopupCtrl.Instance:AlertTip(str,function ()
            intensify_func(true)
        end)
    else
        intensify_func()
    end
end

function PetWeaponIntensifyPanel:PlayAttiChangeEffect()
    local attri_item_list = self.AttriList.item_list
    for i,v in pairs(attri_item_list) do
        v:PlayChangeEffect(true)
    end
end

function PetWeaponIntensifyPanel:OnBreakClick()
    local weapon = self.weapon
    local break_cfg = weapon:BreakCfg()

    for i,item_info in pairs(break_cfg.item_list) do
        local num = Item.GetNum(item_info.item_id)
        local need_num = item_info.num - num

        if need_num > 0 then
            PublicPopupCtrl.Instance:Error(Language.PetWeapon.Error6)
            local stuff_item = Item.Create({item_id = item_info.item_id, jump_need_num = need_num})
            MainOtherCtrl.Instance:GetWayView({item = stuff_item})
            return
        end
    end


    self.EffectLogic:PlayBreakEffect(self.BreakStuffList.item_list)
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        local is_bag = weapon:PetIndex() ~= nil and 0 or 1
        local index = is_bag == 1 and weapon.index or weapon:PetIndex()
        PetCtrl.SendPetOp(PET_OP_TYPE.BreachWeapon, is_bag , index, weapon:GridPos())
        self:PlayAttiChangeEffect()
    end,1)
end

function PetWeaponIntensifyPanel:OnAutoClick()
    local weapon = self.weapon
    if weapon:IsFullLevel() then
        PublicPopupCtrl.Instance:Error(Language.PetWeapon.Error8)
        return
    end
    self:AutoPutExp()
end
-- 自动放入经验材料
function PetWeaponIntensifyPanel:AutoPutExp()
    if self.last_auto_put_qua and self.last_auto_put_qua ~= self.sel_qua then
        for i,v in pairs(self.data.bag_selected_list.cell_list) do
            if v then
                if i:Type() ~= ItemType.PetWeapon and i:Color() > self.sel_qua then
                    self.data.bag_selected_list.cell_list[i] = nil
                end
            end
        end
    end

    self.data.auto_put_qua.qua = self.sel_qua or 3
    self.last_auto_put_qua = self.data.auto_put_qua.qua

    local exp_item_list = BagData.Instance:GetPetWeaponExpItemList()
    if TableIsEmpty(exp_item_list) then
        PublicPopupCtrl.Instance:Center(Language.PetWeapon.BagSelTip10)
    else
        table.sort(exp_item_list, function (a,b)
            return a:Color() > b:Color()
        end)
        local count = 0
        for i,v in pairs(exp_item_list) do
            if count < PetWeaponData.Instance.max_sel_count and v:Color() <= self.data.auto_put_qua.qua then
                local is_can_sel,tip = self.data:IsBagCanSel(PetWeaponViewStatus.Intensify, v, self.weapon)
                if is_can_sel then
                    self.data:AddSelectedBagCell(v, tip)
                    count = count + 1
                end
            end
        end
    end
end

-- 自动放入魂器材料
function PetWeaponIntensifyPanel:AutoPutWeapon()
    if self.last_auto_put_qua and self.last_auto_put_qua ~= self.sel_qua then
        for i,v in pairs(self.data.bag_selected_list.cell_list) do
            if v then
                if i.QuaType and i:QuaType() == self.last_auto_put_qua then
                    self.data.bag_selected_list.cell_list[i] = nil
                end
            end
        end
    end
    self.data.auto_put_qua.qua = self.sel_qua or 3
    self.last_auto_put_qua = self.data.auto_put_qua.qua

    local bag_weapon_list = BagData.Instance:GetWeaponList(nil, self.sel_qua)
    if TableIsEmpty(bag_weapon_list) then
        PublicPopupCtrl.Instance:Center(Language.PetWeapon.BagSelTip9)
    else
        local count = 0
        for i,v in pairs(bag_weapon_list) do
            if v:Level() == 1 and not v:IsLock() and count < PetWeaponData.Instance.max_sel_count then
                local is_can_sel,tip = self.data:IsBagCanSel(PetWeaponViewStatus.Intensify, v, self.weapon)
                if is_can_sel then
                    self.data:AddSelectedBagCell(v, 1)
                    count = count + 1
                end
            end
        end
    end
end

function PetWeaponIntensifyPanel:OnStuffRemoveClick(mono)
    local data = mono:GetData()
    if data then
        if data:Type() == ItemType.PetWeapon then
            PetWeaponData.Instance:AddSelectedBagCell(data, 0)
        else
            PetWeaponData.Instance:AddSelectedBagCell(data.key_item, 0)
        end
    end
end



PetWeaponBreakStuffItem = PetWeaponBreakStuffItem or DeclareMono("PetWeaponBreakStuffItem", UIWidgetBaseItem)
function PetWeaponBreakStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Desc,string.format(Language.PetWeapon.Tip3,data.num))
    self.Cell:SetData(data)
end



PetWeaponItemInfoPart = PetWeaponItemInfoPart or DeclareMono("PetWeaponItemInfoPart", UIWidgetBaseItem)

function PetWeaponItemInfoPart:SetData(weapon)
    UIWidgetBaseItem.SetData(self, weapon)
    UH.SetText(self.Name, weapon:NameDesc())
    UH.SetText(self.Level, string.format(Language.PetWeapon.ItemInfoLevel, weapon:Level()))
    UH.SetText(self.RefineLevel, string.format(Language.PetWeapon.ItemInfoRefineLevel,weapon:RefineLevel()))
    self.WeaponCell:SetData(weapon)
end


-- 背包
-- ========== 背包 ==========
PetWeaponBagPart = PetWeaponBagPart or DeclareMono("PetWeaponBagPart", UIWFlushPanel)
function PetWeaponBagPart:PetWeaponBagPart()
    self.data = PetWeaponData.Instance
    self.status_data = self.data:GetStatusData()
    self.data_cares = {
        {data = self.status_data, func = self.FlushPanel, init_call = false},
        {data = self.data.bag_selected_list, func = self.FlushGridList, init_call = false},
        {data = PetData.Instance.pet_list, func = self.FlushGridList, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushGridList, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Material), func = self.FlushGridList, init_call = false},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.PetWeapon), func = self.FlushGridList, init_call = false},
        {data = self.data.bag_sort_info, func = self.FlushGridList, init_call = false},
        --{data = self.data.auto_put_qua, func = self.FlushGridListSel, init_call = false},
    }
end

function PetWeaponBagPart:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushSel()
    self:FlushGridList()
end

function PetWeaponBagPart:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
    TimeHelper:CancelTimer(self.touch_ht)
end

function PetWeaponBagPart:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self:FlushPanel()
end

function PetWeaponBagPart:FlushPanel()
    self.data:ClearSelectedBagCell()
end

function PetWeaponBagPart:FlushGridList()
    local grid_list_data = self.data:GetWeaponGridListData()
    self.GridList:SetData(grid_list_data)
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        self.data.auto_put_qua.qua = 0
    end,1)
    -- self.GridList:SetCompleteCallBack(function ()
    --     self.data.auto_put_qua.qua = 0
    -- end)
end

function PetWeaponBagPart:FlushGridListSel()
    -- if self.data.auto_put_qua.qua == 0 then
    --     return
    -- end
    -- for i,v in pairs(self.GridList.index_item) do
    --     v:CheckSel()
    -- end
    -- self.data.auto_put_qua.qua = 0
end

function PetWeaponBagPart:FlushSel()
    local sort_info = self.data.bag_sort_info
    self.UpSortToggle.isOn = sort_info.sort_type == 2
    self.DownSortToggle.isOn = sort_info.sort_type == 1

    local str = Language.PetWeapon.QuaDesc[sort_info.sel_qua]
    if sort_info.sel_qua > 0 then
        str = ColorStr(str, QualityColorStr[sort_info.sel_qua])
    end
    UH.SetText(self.SelText, str)
end

function PetWeaponBagPart:CompareWeapon(item1, item2)
    if item1:Type() ~= ItemType.PetWeapon or item2:Type() ~= ItemType.PetWeapon then
        return
    end
    if self.data.auto_put_qua.qua > 0 then
        return
    end
    local info = {
        item_list = {item1, item2},func = function ()
			local func = {}
			if item1:ShowType() ~= ShowType.PetWeapon then
				return func
			end
			if PetWeaponData.Instance:CurWeapon() then
				--func = { ItemInfoFunc.PetWeaponReplace }
			end
			return func
		end
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function PetWeaponBagPart:OnSelToggleEvent(toggle_value)
    if not self.set_sel_list then
        self.set_sel_list = true
        local list_data = self.data:GetSelListData(true, true)
        self.SelList:SetData(list_data)
    end
end

function PetWeaponBagPart:OnSelToggleItemClick(data)
    UH.SetText(self.SelText, data.str)
    self.SelToggle.isOn = false
    self.data.bag_sort_info.sel_qua = data.qua
end

function PetWeaponBagPart:OnSortSelClick(sort_type)
    self.data.bag_sort_info.sort_type = sort_type
end

-- ============== 新加 ============
function PetWeaponBagPart:OnSelWeaponClick(mono)
    local new_sel = not mono.sel
    local data = mono:GetData()
    local cur_weapom = self.data:GetCurCtrlWeapon()

    local num = 1
    if new_sel then
        local is_can_sel,tip = self.data:IsBagCanSel(self.status_data.status_type, data, cur_weapom)
        if not is_can_sel then
            PublicPopupCtrl.Instance:Center(tip)
            return false
        end
    else
        num = 0
    end

    self.data:AddSelectedBagCell(data, num)
    mono:SetSel(new_sel)
    return true
end

function PetWeaponBagPart:OnSelStuffClick(mono)
    local data = mono:GetData()
    local cur_weapon = self.data:GetCurCtrlWeapon()

    -- 是否满级
    if cur_weapon:IsFullLevel() then
        PublicPopupCtrl.Instance:Center(Language.PetWeapon.Error8)
        TimeHelper:CancelTimer(self.touch_ht)
        return 
    end
    -- 是否可以放经验
    local max_exp = self.data:GetIntensifyNeedExp(cur_weapon)
    local sel_all_exp = self.data:GetSelListItemPriceExp()
    if sel_all_exp >= max_exp then
        PublicPopupCtrl.Instance:Center(Language.PetWeapon.BagSelTip7)
        TimeHelper:CancelTimer(self.touch_ht)
        return
    end

    local cur_num = self.data.bag_selected_list.cell_list[data] or 0
    if cur_num == data:Num() then
        PublicPopupCtrl.Instance:Center(Language.PetStampIntensify.Tip5)
        TimeHelper:CancelTimer(self.touch_ht)
        return
    end
    self.data:AddSelectedBagCell(data, cur_num + 1)

    mono:SetSel(true, cur_num + 1)
end


function PetWeaponBagPart:OnUnSelStuffClick(mono)
    local data = mono:GetData()
    local cur_num = self.data.bag_selected_list.cell_list[data] or 0
    cur_num = cur_num - 1 < 0 and 0 or cur_num - 1
    self.data:AddSelectedBagCell(data, cur_num)
    mono:SetSel(cur_num > 0, cur_num)
end

function PetWeaponBagPart:OnPutStuffDown(mono)
    TimeHelper:CancelTimer(self.touch_ht)
	self.touch_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.OnSelStuffClick,self,mono),0.2)
end

function PetWeaponBagPart:OnPutStuffUp()
    TimeHelper:CancelTimer(self.touch_ht)
end

PetWeaponBagItem = PetWeaponBagItem or DeclareMono("PetWeaponBagItem", UIWidgetBaseItem)
-- data : weapon
function PetWeaponBagItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.Cell:SetData(data)
    if data.item_id == 0 then
        self.WeaponEvent:SetActive(false)
        self.StuffEvent:SetActive(false)
        self.SelImg:SetActive(false)
    else
        self.WeaponEvent:SetActive(data:Type() == ItemType.PetWeapon)
        self.StuffEvent:SetActive(data:Type() ~= ItemType.PetWeapon)
        local num
        num = PetWeaponData.Instance.bag_selected_list.cell_list[data] or 0
        self.sel = num > 0
        self:SetSel(self.sel, num)
    end
end

function PetWeaponBagItem:CheckSel()
   
end

function PetWeaponBagItem:SetSel(value, sel_num)
    self.sel = value
    self.SelImg:SetActive(value)

    local num = self.data:Num()
    if value and sel_num and sel_num > 0 and self.data:Type() ~= ItemType.PetWeapon then
        CellFlushs.SetNum(self.Cell, string.format("%s/%s",sel_num,num))
        self.RemoveBtn:SetActive(true)
    else
        self.RemoveBtn:SetActive(false)
        CellFlushs.SetNum(self.Cell, num)
    end
end

-- 强化界面特效逻辑
PetWeaponIntensifyEffectLogic = PetWeaponIntensifyEffectLogic or DeclareMono("PetWeaponIntensifyEffectLogic",UIWFlushPanel)

function PetWeaponIntensifyEffectLogic:PetWeaponIntensifyEffectLogic()
    
end

function PetWeaponIntensifyEffectLogic:PlayUpEffect()
    self.EffectTool:Play(3161024, self.UpEffectObj)
end

function PetWeaponIntensifyEffectLogic:PlayStuffEffect(count)
    local pos = self.IntStuffListContent:GetLocalPosition()
    pos.x = 0
    self.IntStuffListContent:SetLocalPosition(pos)
    count = count > 4 and 4 or count

    if self.cure_list == nil then
        self.cure_list = {}
        self.stuff_pos = {}
        for i = 1, 4 do
            local pos = self.UpStuffEffects[i]:GetLocalPosition()
            self.stuff_pos[i] = pos
            local t_pos = self.StuffTargetObj:GetLocalPosition()
            if i <= 2 then
                self.cure_list[i] = Game.CurveParabola(pos, Vector3.New(-1, 3, 0), t_pos, Vector3.New(3, 2, 0))
            else
                self.cure_list[i] = Game.CurveParabola(pos, Vector3.New(1, 3, 0), t_pos, Vector3.New(-3, 2, 0))
            end
        end
    end
    self.default_step = 20
    self.cur_step = 1
    for i = 1, 4 do
        local obj = self.UpStuffEffects[i]
        obj:SetActive(i <= count)
        obj:SetLocalPosition(self.stuff_pos[i])
    end
    TimeHelper:CancelTimer(self.run_ht)
    self.run_ht = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.moveEffect,self,count),1,self.default_step)
end

function PetWeaponIntensifyEffectLogic:moveEffect(count)
    for i = 1, count do
        local pos = self.cure_list[i]:GetPos(self.cur_step / self.default_step)
        local obj = self.UpStuffEffects[i]
        obj:SetLocalPosition(pos)
    end
    self.cur_step = self.cur_step + 1
    if self.cur_step > self.default_step then
        for i = 1, count do
            local obj = self.UpStuffEffects[i]
            obj:SetActive(false)
        end
	end
end

function PetWeaponIntensifyEffectLogic:PlayBreakEffect(stuff_item_list)
    --self.EffectTool:Play(3161024, self.BreackEffectObj)
    if stuff_item_list then
        for i,v in pairs(stuff_item_list) do
            self.EffectTool:Play(3161025, v.gameObject)
        end
    end
end
function PetWeaponIntensifyEffectLogic:OnDestroy()
    TimeHelper:CancelTimer(self.run_ht)
    UIWFlushPanel.OnDestroy(self)
end


-- 强化界面的属性条
PetWeaponIntensifyAttriItem = PetWeaponIntensifyAttriItem or DeclareMono("PetWeaponIntensifyAttriItem",UIWidgetBaseItem)

function PetWeaponIntensifyAttriItem:SetData(data)
    UIWAttrItem.SetData(self, data)
    local add_value_str = tostring(data.add_value or 0)
    local value_str = tostring(data.value or 0)
    
    if data.add_value and data.add_value > data.value then

        self.AddValue:SetObjActive(true)

        if DataHelper.IsPercent(data.attri_type) then
            local value = data.value or 0
            local add_value = data.add_value or 0
            value_str = string.format("%s%%",value/100)
            add_value_str = string.format("%s%%",add_value/100)
        end
        UH.SetText(self.Name,string.format("%s  %s",data.name, value_str))
        UH.SetText(self.Value,"")
        UH.SetText(self.AddValue,add_value_str)
    else

        self.AddValue:SetObjActive(false)

        if DataHelper.IsPercent(data.attri_type) then
            local value = data.value or 0
            value_str = string.format("%s%%",value/100)
        end

        UH.SetText(self.Name,string.format("%s",data.name))
        UH.SetText(self.Value,value_str)
    end

end

function PetWeaponIntensifyAttriItem:PlayChangeEffect(is_play)
    if self.data.add_value ~= self.data.value or is_play == true then
        self.EffectTool:Play(3161024)
    end
end

-- 突破预览材料
PetWeaponReviewStuffItem =  PetWeaponReviewStuffItem or DeclareMono("PetWeaponReviewStuffItem", UIWidgetBaseItem)
function PetWeaponReviewStuffItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    local num = Item.GetNum(data.item_id)
    UH.SetText(self.Name, string.format("%s：%s",data:Name(), DataHelper.ConsumeNum(num, data.num)))
end