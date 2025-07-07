WorkshopSmeltNeo = WorkshopSmeltNeo or DeclareMono("WorkshopSmeltNeo", UIWFlushPanel)
function WorkshopSmeltNeo:WorkshopSmeltNeo()
    self.workshop_data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushEquip },
        {data = BagData.Instance.equipment_list, func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.jewelry_list, func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.equipment_voc_list, func = self.FlushEquip, init_call = false},
        {data = self.workshop_data.smelt_select_lock, func = self.FlushCost},
        {data = self.workshop_data.smelt_time_limit, func = self.FlushTime}
    }
    self.send_data = {
        role_type =0,
        select_equip = nil,
        equip_pos = -1,
    }

    self.workshop_data:SetIsFunRed(false)
end 
function WorkshopSmeltNeo:onFlush()
    self:FlushEquip()
end

function WorkshopSmeltNeo:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end

    if self.block_delay_timer ~= nil then 
        TimeHelper:CancelTimer(self.block_delay_timer)
        self.block_delay_timer = nil
    end 

    self.cur_index = nil
    self.workshop_data:SetSmeltSelect()
    self.workshop_data:ClearSmeltRed()
end 

function WorkshopSmeltNeo:FlushEquip()
    local new_list = self.workshop_data:GetSmeltEquipList()
    self.EquipList:SetData(new_list)
    if table.nums(new_list) == 0 then
        self.Total:SetActive(false)
        self.GuaJian:SetActive(true)
    else
        self.Total:SetActive(true)
        self.GuaJian:SetActive(false)
    end

    if self.cur_index == nil and self.workshop_data:GetSmeltSelect() then
        self.EquipList:ClickItemData(self.workshop_data:GetSmeltSelect())
    else
        self.EquipList:ClickItem(self.cur_index or 1)
    end

    if not self.is_init then 
        if self.workshop_data:GetSmeltSelect() ~= nil then 
            local pos_ = 1
            for k,v in pairs(new_list) do 
                if self.workshop_data:GetSmeltSelect().index == v.index 
                    and self.workshop_data:GetSmeltSelect().in_bag == v.in_bag 
                    and self.workshop_data:GetSmeltSelect().item_id == v.item_id then 
                    pos_ = k
                end 
            end  
            if self.flush_left_item_handle ~= nil then
                TimeHelper:CancelTimer(self.flush_left_item_handle)
                self.flush_left_item_handle = nil
            end

            self.flush_left_item_handle = TimeHelper:AddDelayFrameTimer(function()
                self.flush_left_item_handle = nil
                if pos_ <= #new_list -5 then 
                    self.EquipList:JumpVerticalIndex(pos_,100)
                else 
                    self.EquipList:JumpVerticalIndex(#new_list -5,100,65)
                end 
            end,5+math.floor(pos_/self.EquipList.StepCount))
        end 
        self.is_init = true
    end 
end 

function WorkshopSmeltNeo:OnClickItem(data)
    local temp_index = self.EquipList:GetSelectedIndex(data)
    self.send_data.select_equip = data
    self.send_data.role_type = AnotherData.Instance:CheckEquipmentUniqueId(data,data:IsEquip()) and 0 or 1
    self.send_data.equip_pos = data.index

    if self.EquipList:GetSelectedIndex(data) ~= self.cur_index then 
        self.workshop_data:ClearSmeltVocLockPos()
    end 
    self.workshop_data:SetSmeltSelect(data)

    self.cur_index = temp_index
    self:FlushEquipInfo()
end 

function WorkshopSmeltNeo:FlushEquipInfo()
    local attr = self.workshop_data:GetSmeltSelectAttr()
    table.sort(attr, DataSorter.KeyLowerSorter("index"))
    self.AttrList1:SetData(attr)
    self.EquipCell:SetData(self.send_data.select_equip)
    UH.SetText(self.LblName, self.send_data.select_equip:QuaName(true))
    local cur_point = self.workshop_data:CatchSmeltAttrScore(attr,self.send_data.select_equip:IsEquip())
    UH.SetText(self.cur_point, string.format(Language.Workshop.SmeltAttrScore,cur_point))

    local flag = self.workshop_data:GetSmeltSelectWithFresh()
    self.EmptyTotal:SetActive(not flag)
    self.CheckTotal:SetActive(flag)
    if flag then 
        local attr = self.workshop_data:GetSmeltSelectRefreshAttr()
        table.sort(attr, DataSorter.KeyLowerSorter("index"))
        self.AttrList2:SetData(attr)
        local next_point = self.workshop_data:CatchSmeltAttrScore(attr,self.send_data.select_equip:IsEquip())
        UH.SetText(self.next_point, string.format(Language.Workshop.SmeltAttrScore,next_point))
        self.result_arrow.gameObject:SetActive(next_point ~= cur_point)
        UH.SpriteName(self.result_arrow, next_point > cur_point and "up_s" or "down_s")
    end 
    
    self.BtnInherit:SetActive(not self.workshop_data:GetSmeltSelectNeedUnLock())
    self.BtnSave:SetActive(not self.workshop_data:GetSmeltSelectNeedUnLock() and flag)
    self.gold_smelt:SetActive(not self.workshop_data:GetSmeltSelectNeedUnLock())
    self.OperateText.text = self.workshop_data:GetSmeltSelectNeedUnLock() and Language.Workshop.BtnActSmelt or Language.Workshop.BtnSmelt
    self.OperateBtn:SetActive(not self.workshop_data:GetSmeltSelectNeedUnLock())
    self.FreeBtn:SetActive(self.workshop_data:GetSmeltSelectNeedUnLock())
    if self.workshop_data:GetSmeltSelectNeedUnLock() and self.is_gold_smelt.activeSelf then 
        self:OnClickGoldSmelt()
    end 

    local smelt_btn_red = self.workshop_data:GetSmeltBtnRed()
    self.BtnRed:SetNum(0)

    self.BtnChangeInter.Interactable = not self.workshop_data:GetEquipIsAllLock(self.workshop_data:GetSmeltSelect())
end

function WorkshopSmeltNeo:FlushCost()
    local lock_list = Config.entry_cfg_auto.lock_price
    if self.workshop_data:GetSmeltSelect() == nil then
        self.Cost.text = self.is_gold_smelt.activeSelf and lock_list[1].smelt_gold or lock_list[1].coin 
        self.LockStrObj:SetActive(false)
        return 
    end 
    local list = self.workshop_data:GetSmeltVocLockPos()
    local num = 0
    for k,v in pairs(list) do 
        num = num + v
    end 

    local lock_cfg = lock_list[1]
    for k,v in pairs(lock_list) do 
        if v.level == num then 
            lock_cfg = v 
            break
        end 
    end 

    self.Cost.text = self.is_gold_smelt.activeSelf and lock_list[1].smelt_gold or lock_list[1].coin
    self.LockStrObj:SetActive(num > 0)
    if num > 0 then  
        self.LockCost.text = lock_cfg.gold .. Language.Common.HuoBi[0]
        self.LockStr.text = string.format( Language.Workshop.SmeltLockCost,DataHelper.GetWord(num) ) 
    end 
end

function WorkshopSmeltNeo:FlushTime()
    UH.SetText(self.smelt_limit, Language.Workshop.SmeltCoinLimitTips..Config.entry_cfg_auto.other[1].coin_smelt_times - self.workshop_data.smelt_time_limit.time..")")
end

function WorkshopSmeltNeo:CheckCostEnough()
    local lock_list = Config.entry_cfg_auto.lock_price

    if not self.is_gold_smelt.activeSelf then 
        local coin = RoleData.Instance:GetCurrency(CurrencyType.CoinBind)
        local need = lock_list[1].coin 

        if need > coin and not self.workshop_data:GetSmeltSelectNeedUnLock() then 
            PublicPopupCtrl.Instance:Center(Language.Workshop.SmeltNeedCoin)
            return false
        end 
    else
        local gold = RoleData.Instance:GetCurrency(CurrencyType.Gold)
        local need = lock_list[1].smelt_gold

        if need > gold then 
            PublicPopupCtrl.Instance:Center(Language.Workshop.SmeltNeedGold)
            return false
        end 
    end 

    return true
end 

function WorkshopSmeltNeo:OnClickChange()
    if not self:CheckCostEnough() then return end 

    if not self.workshop_data:GetSmeltSelectNeedUnLock() and not self.workshop_data:IsCoinSmeltLimit() and not self.is_gold_smelt.activeSelf then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.SmeltCoinLimit)
        return 
    end 

    if self.workshop_data:GetEquipIsAllLock(self.workshop_data:GetSmeltSelect()) then
        local data = self.AttrList1:GetItem(1):GetData()
        if data.is_role_limited then 
            PublicPopupCtrl.Instance:Center(string.format(Language.Workshop.SmeltAllLockRoleLimitTips,data.role_limited_level..Language.Common.Level))
        else
            PublicPopupCtrl.Instance:Center(string.format(Language.Workshop.SmeltAllLockTips, data.limit_text))
        end 
        return
    end 

    local data = self.workshop_data:GetSmeltSelect()
    if self.workshop_data:GetSmeltSelectNeedUnLock() then 
        local limited_lock = self.workshop_data:GetLockLimited(data)
        for i = 1,limited_lock do 
            self.ctrl:SendOperateReq(
                WORLSHOP_OP.WORKSHOP_OP_AVTIVE_EQUIP,
                data:IsEquip() and 0 or 1 ,
                data.in_bag == true and 1 or 0,
                self.send_data.equip_pos,
                i - 1,
                AnotherData.Instance:CheckEquipmentUniqueId(data,data:IsEquip()) and 0 or 1
            )
        end
    else
        self.ctrl:SendOperateReq(
            WORLSHOP_OP.WORKSHOP_OP_REFRESH_EQUIP,
            data:IsEquip() and 0 or 1 ,
            data.in_bag == true and 1 or 0,
            self.send_data.equip_pos,
            self.workshop_data.smelt_select_lock.str,
            AnotherData.Instance:CheckEquipmentUniqueId(data,data:IsEquip()) and 0 or 1,
            self.is_gold_smelt.activeSelf and 1 or 0
        )
    end 

    if self.block_delay_timer ~= nil then 
        TimeHelper:CancelTimer(self.block_delay_timer)
        self.block_delay_timer = nil
    end 
    self.block_delay_timer = TimeHelper:AddDelayTimer(function ()
        if ViewMgr:IsOpen(EquipView) then
            self.block:SetActive(false)    
        end
    end,1)
end

function WorkshopSmeltNeo:OnClickSave()
    local data = self.workshop_data:GetSmeltSelect()
    self.ctrl:SendOperateReq(
            WORLSHOP_OP.WORKSHOP_OP_SAVE_EQUIP_REFRESH,
            data:IsEquip() and 0 or 1 ,
            data.in_bag == true and 1 or 0,
            self.send_data.equip_pos,
            AnotherData.Instance:CheckEquipmentUniqueId(data,data:IsEquip()) and 0 or 1
        )

    if MainViewData.Instance:BaseData().is_battle == true then
        PublicPopupCtrl.Instance:Center(Language.Workshop.SmeltSaveBattle)
    end 
end 

function WorkshopSmeltNeo:OnClickInherit()
    ViewMgr:OpenView(WorkshopSmeltPlus)
end

function WorkshopSmeltNeo:OnClickCheck()
    if not self.workshop_data:GetEquipIsAllLock(self.workshop_data:GetSmeltSelect()) then 
        ViewMgr:OpenView(SmeltPreRangeView, self.workshop_data:GetSmeltSelect() )
    else 
        local level,role_level = self.workshop_data:GetSmeltOpenLevel(self.workshop_data:GetSmeltSelect())
        if role_level > RoleData.Instance:GetRoleLevel() then 
            PublicPopupCtrl.Instance:Center(string.format(Language.Workshop.SmeltAllLockRoleLimitTips, role_level))
        else 
            PublicPopupCtrl.Instance:Center(string.format(Language.Workshop.SmeltLockEquipTips, level))
        end 
    end 
end

function WorkshopSmeltNeo:OnClickGoldSmelt()
    self.is_gold_smelt:SetActive(not self.is_gold_smelt.activeSelf)

    local is_gold = self.is_gold_smelt.activeSelf
    self.GoldImage:SetActive(is_gold)
    self.CoinImage:SetActive(not is_gold)

    self:FlushCost()
end

function WorkshopSmeltNeo:ClickEffectComplete()
    -- LogError("!?@ClickEffectComplete")
    if self.workshop_data.smelt_click_mark == nil then 
        return 
    end 
    local handle = self.workshop_data.smelt_click_mark.handle
    local param = self.workshop_data.smelt_click_mark.param
    self.EffectTool:Stop(handle)

    self.ctrl:SendOperateReq(
        WORLSHOP_OP.WORKSHOP_OP_AVTIVE_EQUIP,
        param.param_1,param.param_2,param.param_3,param.param_4,param.param_5)

    self.workshop_data:MarkSmeltClickParam()
    self.block:SetActive(false)
end

--------------------

WorkshopSmeltAttrNeoItem = WorkshopSmeltAttrNeoItem or DeclareMono("WorkshopSmeltAttrNeoItem", UIWidgetBaseItem)
function WorkshopSmeltAttrNeoItem:WorkshopSmeltAttrNeoItem()
    self.workshop_data = WorkshopData.Instance
    self:uncareData()
    self:careData(WorkshopData.Instance.smelt_select_lock, BindTool.Bind(self.FlushVocLock,self))
    self.eff_handle = nil
    self.eff_handle_click = nil
end 

function WorkshopSmeltAttrNeoItem:SetData(data)
    -- LogError("?SetData",data)
    UIWidgetBaseItem.SetData(self, data)
    self:InitData()
    local pass = self.data.info.lock == 1 or data.info.attr_value ~= 0
    -- 针对老装备
    if not pass then
        self.btn_plus:SetActive(false)
        self.max_mark:SetActive(false)
        -- 检查是否角色等级限制
        self.RoleLimitObj:SetActive(self.data.is_role_limited)
        if self.data.is_role_limited then 
            self.LblRoleLimited.text = self.data.role_limit_text
            return 
        end

        -- 检查是否装备等级限制
        self.LimitObj:SetActive( self.data.is_limited )
        if self.data.is_limited then 
            self.LblLimited.text = self.data.limit_text
            return 
        end 
    else 
        self.RoleLimitObj:SetActive(false)
        self.LimitObj:SetActive(false)
    end 

    if self.click_eff_handle ~= nil then 
        self.click_eff_handle = nil
    end 

    -- 检查是否未激活
    if self.data.info.lock == 0 or self.data.info.attr_value == 0 then
        self.EmptyObj:SetActive(true)
        self:ActRedPoint(true)
        if self.eff_handle == nil then 
            self.eff_handle = self.EffectTool:Play(2165045,self.EmptyObj)
        end 
        return 
    else 
        if self.eff_handle ~= nil then 
            self.EffectTool:Stop(self.eff_handle)
            self.eff_handle = nil
        end 
    end 
    -- 赋值
    self.InfoObj:SetActive(true)
    self.LblAttr.text = string.format(Language.Common.Xstr,QualityColorStrSmelt[data.info.color],
        string.format(Language.Workshop.IncAttr, Language.Common.AttrList[data.info.attr_type],
        DataHelper.IsPercent(data.info.attr_type) and Percent(data.info.attr_value) or data.info.attr_value))

    self.IsPoint:SetActive(DataHelper.IsPoint(data.info.attr_type))
    self.LblRange.text = string.format( Language.Workshop.SmeltRange,
        DataHelper.IsPercent(data.info.attr_type) and Percent(self.data.min,nil,true) or self.data.min,
        DataHelper.IsPercent(data.info.attr_type) and Percent(self.data.max,nil,true) or self.data.max )
    self:FlushVocLock()    

    local qua_cfg = WorkshopData.Instance:GetSmeltAttrRangeConfig(self.data)
    -- 淬炼系统已屏蔽
    self.btn_plus:SetActive(false)--qua_cfg ~= nil and self.data.info.attr_value < self.data.max and self.data.info.color >= ItemColor.Purple) 
    self.max_mark:SetActive(qua_cfg ~= nil and self.data.info.attr_value >= self.data.max and self.data.info.color >= ItemColor.Purple)
end 

function WorkshopSmeltAttrNeoItem:ActRedPoint(flag)
    if flag == nil then return end 
	self.red_point:SetNum(flag and 1 or 0)
end

function WorkshopSmeltAttrNeoItem:FlushVocLock()
    if self.data == nil then
        return 
    end 

    if self.data.info == nil or self.data.info.lock == 0 or self.data.info.attr_value == 0 then
        return 
    end 

    local list = self.workshop_data:GetSmeltVocLockPos()
    self.is_tick = list[self.data.index]
    self.NomalObj:SetActive(self.is_tick == 0)
    self.LightObj:SetActive(self.is_tick == 1)
end 

function WorkshopSmeltAttrNeoItem:OnClickTick()
    -- 是否激活
    if self.data.info.lock == 0 or self.data.info.attr_value == 0 
        and not self.data.is_limited and not self.data.is_role_limited then
            if self.workshop_data.smelt_click_mark ~= nil then 
                return 
            end 

            if self.data.index > self.workshop_data:GetLockLimited(self.workshop_data:GetSmeltSelect()) then 
                return 
            end 

            if not self.workshop_data:GetBeforeComplete(self.data.index) then 
                PublicPopupCtrl.Instance:Center(Language.Workshop.SmeltCantBeforeUnLockTips)
                return 
            end 

            if self.block.activeSelf or self.click_eff_handle ~= nil then return end 
            self.block:SetActive(true)

            local equip =  self.workshop_data:GetSmeltSelect()
            self.click_eff_handle = self.EffectTool:Play(2165044,self.EmptyObj)
            self.workshop_data:MarkSmeltClickParam({
                handle = self.click_eff_handle,
                param = {
                    param_1 = equip:IsEquip() and 0 or 1 ,
                    param_2 = equip.in_bag == true and 1 or 0,
                    param_3 = equip.index,
                    param_4 = self.data.index - 1, -- 从0开始 要-1
                    param_5 = AnotherData.Instance:CheckEquipmentUniqueId(equip,equip:IsEquip()) and 0 or 1,
                }})
        return 
    end 

    -- 是否选中
    if self.is_tick == nil then return end 
    if not self.workshop_data:GetCanSmeltVocLock() and self.is_tick == 0 then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.SmeltCantAllLockTips)
        return 
    end 
    self.workshop_data:SetSmeltVocLockPos(self.data.index, not (self.is_tick == 1) )
end

function WorkshopSmeltAttrNeoItem:InitData()
    self.LimitObj:SetActive(false)
    self.EmptyObj:SetActive(false)
    self.NomalObj:SetActive(false)
    self.LightObj:SetActive(false)
    self.InfoObj:SetActive(false)
    self:ActRedPoint(false)
    self.is_tick = nil
end

function WorkshopSmeltAttrNeoItem:OnClickSmeltPlus()
    local qua_cfg = WorkshopData.Instance:GetSmeltAttrRangeConfig(self.data)
    if self.data.info.attr_value >= self.data.max then -- qua_cfg.max then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.SmeltPlusComplete)
        return 
    end 

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
    ViewMgr:OpenView(WorkshopSmeltSmeltPlus,self.data)
end

WorkshopSmeltFreshItem = WorkshopSmeltFreshItem or DeclareMono("WorkshopSmeltFreshItem", UIWidgetBaseItem)
function WorkshopSmeltFreshItem:WorkshopSmeltFreshItem()
    self.workshop_data = WorkshopData.Instance
end 

function WorkshopSmeltFreshItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    --检查是否无值
    self.LockObj:SetActive(self.data.info.lock == 0)
    if self.data.info.lock == 0 then 
        self.IsPoint:SetActive(false)
        self.ChangeObj:SetActive(false)
        return 
    end 
    --赋值
    self.LblAttr.text = string.format(Language.Common.Xstr,QualityColorStrSmelt[data.info.color],
        string.format(Language.Common.IncAttr, Language.Common.AttrList[data.info.attr_type],
        DataHelper.IsPercent(data.info.attr_type) and Percent(data.info.attr_value) or data.info.attr_value))

    self.IsPoint:SetActive(DataHelper.IsPoint(data.info.attr_type))

    self:FlushChange()
end

function WorkshopSmeltFreshItem:FlushChange()
    local prof_type =  RoleData.Instance:ProfType()

    local info_score = BagData.Instance:GetRoleScoreByAttr(self.data.info.attr_type, self.data.info.attr_value)

    local origin_score = BagData.Instance:GetRoleScoreByAttr(self.data.info_origin.attr_type, self.data.info_origin.attr_value)

    self.ChangeObj:SetActive(false)
    self.MaxObj:SetActive(false)
    self.DownObj:SetActive(false)
    self.UpObj:SetActive(false)

    if self.data.info.attr_value == self.data.max then
        self.ChangeObj:SetActive(true)
        self.MaxObj:SetActive(true)
    elseif info_score < origin_score and self.data.info.attr_type == self.data.info_origin.attr_type then  
        self.ChangeObj:SetActive(true)
        self.DownObj:SetActive(true)
    elseif info_score > origin_score and self.data.info.attr_type == self.data.info_origin.attr_type then  
        self.ChangeObj:SetActive(true)
        self.UpObj:SetActive(true)
    end 
end

WorkshopSmeltNeoListItem = WorkshopSmeltNeoListItem or DeclareMono("WorkshopSmeltNeoListItem", UIWidgetBaseItem)
function WorkshopSmeltNeoListItem:WorkshopSmeltNeoListItem()

end
function WorkshopSmeltNeoListItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end

function WorkshopSmeltNeoListItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.EquipCell:SetData(data)
    UH.SetText(self.LblName, data:Name())
    UH.SetText(self.LblType, data:TypeDesc())
    local level_text = data:Color() >= ItemColor.Star and data:LevelDesc() or data:LimitLv()..Language.Common.Level
    UH.SetText(self.LblLevel,level_text)
    --判断是否显示红点
    local smelt_btn_red = WorkshopData.Instance:GetSmeltBtnRed(self.data)
    local num = smelt_btn_red and 1 or 0
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(154,36,0))
    end
    self.red_point:SetNum(num)
    self.HsTag:SetActive(not AnotherData.Instance:CheckEquipmentUniqueId(data,data:IsEquip())) 
    self.EquipTag:SetActive(not data.in_bag)
end