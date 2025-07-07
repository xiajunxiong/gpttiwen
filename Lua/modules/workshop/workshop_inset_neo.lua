WorkshopInsetNeo = WorkshopInsetNeo or DeclareMono("WorkshopInsetNeo", UIWFlushPanel)
-- 显示背包和身上的装备
function WorkshopInsetNeo:WorkshopInsetNeo()
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.select_equip = nil
    self.select_item = nil
    self.mono = nil 
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushBagChange, init_call = false},
        {data = BagData.Instance.equipment_list, func = self.FlushBagChange, init_call = false},
        {data = BagData.Instance.jewelry_list, func = self.FlushBagChange, init_call = false}
    }

    self.first_index = 1
    GuideManager.Instance:RegisterGetGuideUi("WorkshopStoneUp", function ()
        if self.StoneList and self.StoneList:SetupCompleted() then
            return self.StoneList.item_list[1].BtnInset--[[ , function ()
                self.StoneList.item_list[1].ItemClick:Trigger()
            end ]]
        end
        return nil,nil
    end)

    GuideManager.Instance:RegisterGetGuideUi("WorkshopStoneTarget", function ()
        if self.EquipList and self.EquipList:SetupCompleted() then
            -- 首次循环成功？
            for k,v in pairs(self.EquipList.item_list) do
                local data = v:GetData()                
                if data and data.EquipType and data:EquipType() == EquipmentIndex.IndexMainWeapon then 
                    self.EquipList:AutoSlide(data,false)
                    return self.EquipList:GetItem(k).gameObject
                end 
            end 
            -- 不成功说明没有，直接找一件装备
            for k,v in pairs(self.EquipList.item_list) do
                local data = v:GetData()                
                if data and data.item_id and data.item_id > 0 then 
                    self.EquipList:AutoSlide(data,false)
                    return self.EquipList:GetItem(k).gameObject
                end 
            end 
        end
        return nil,nil
    end)
    
    self.EquipItem:SetData({item_id = 0})
    self.InsetTips.text = self.data:GetProfRecommend()

    self.flush_left_item_handle = nil
    self.send_data = {}

    
    for i = 1, EQUIP_STONE_COUNT_MAX do 
        self.FlyEffects[i]:SetData({mono = self,
            end_func_time = 0,
            complete_effect = "10010255",
            end_func = function ()
                if self.select_item ~= nil then 
                    self.select_item:Play()
                end 
           end
        })
    end 
end 

function WorkshopInsetNeo:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end
    self.data:SetInsetMark()
    self.data:SetCurSelectEquipInset(nil)
end 

-- 相当于OpenCallBack
function WorkshopInsetNeo:onFlush()
    self:FlushLeftItem(true,self.data:GetInsetMark())
end

function WorkshopInsetNeo:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[20].desc})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2) 
end 

-- 全数据更新
function WorkshopInsetNeo:FlushPanelInfo()
    self:FlushCenterInfo()
    self:FlushRightInfo()
end 

-- 背包相关更新
function WorkshopInsetNeo:FlushBagChange()
    self:FlushSelectEquip()

    self:FlushLeftItem()
    self:FlushPanelInfo()
end

function WorkshopInsetNeo:FlushSelectEquip()
    if self.select_equip == nil then return end 
    local is_equip = self.select_equip:IsEquip()
    local in_bag =  self.select_equip.in_bag
    local pos = self.select_equip.index

    self.old_equip = self.select_equip
    if in_bag then 
        self.select_equip = BagData.Instance:GetColumnGrids(ItemColumnType.Item)[pos]
    elseif is_equip then 
        self.select_equip = BagData.Instance.equipment_list[pos]
    else 
        self.select_equip = BagData.Instance.jewelry_list[pos]
    end

    self.data:SetCurSelectEquipInset(self.select_equip)

    local old_stone = self.old_equip:Stone()
    local neo_stone = self.select_equip:Stone()

    for i = 1, EQUIP_STONE_COUNT_MAX do 
        if old_stone[i] == 0 and neo_stone[i] > 0 then
            self.FlyEffects[i]:Play("10010254")
        end 
    end 
end

function WorkshopInsetNeo:OnClickItem(data, item)
    self.select_equip = data
    self.select_item = item
    self.data:SetCurSelectEquipInset(self.select_equip)

    self.data:SetInsetMark(data)
    -- 全更新
    self:FlushPanelInfo()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

--左侧刷新
function WorkshopInsetNeo:FlushLeftItem(is_jump,mark)
    local data = self.data:GetSmeltEquipList()
    self.EquipList:SetData(data)
    self.NoneEquip:SetActive(next(data) == nil)
    self.NoneStone:SetActive(next(data) == nil)

    if not is_jump and mark == nil then return end 

    if mark == nil then
        self.EquipList:ClickItem(1)
        self.EquipList:JumpVerticalIndex(1,96)
    else 
        local pos = 1
        local findIt,pos = self.data:CheckIsMarkItOrigin(mark,data)
        self.EquipList:ClickItem(pos)

        if findIt then 
            if self.flush_left_item_handle ~= nil then
                TimeHelper:CancelTimer(self.flush_left_item_hand0le)
                self.flush_left_item_handle = nil
            end

            self.flush_left_item_handle = TimeHelper:AddDelayFrameTimer(function()
                self.flush_left_item_handle = nil
                if pos <= #data -5 then 
                    self.EquipList:JumpVerticalIndex(pos,96)
                else 
                    self.EquipList:JumpVerticalIndex(#data -5,96,65)
                end 
            end,5+math.floor(pos/self.EquipList.StepCount))
        end 
    end 
end 

function WorkshopInsetNeo:OnClickStone(mono,data)
    if self.select_equip == nil then return end 
    self.mono = mono
    local param_t = self:CatchSendData(data,WORLSHOP_OP.WORKSHOP_OP_GEM_MOUNTING)

    if param_t.param4 == nil then
        self.data:SetMateData(data)
        ViewMgr:OpenView(WorkShopQuickBuy)
        return
    end

    local stone_list = self.select_equip:Stone()
    local equip_sort_limit = false
    local desc = 0
    local is_full = false
    for i = 1,4 do 
        if stone_list[i] == 0 then 
            equip_sort_limit,desc = self.data:GetStoneSortLimit(self.select_equip,i)
            -- if not equip_limit then
                break
            -- end 
        end 
        is_full = i == 4 
    end 
    if not equip_sort_limit then 
        -- PublicPopupCtrl.Instance:Center(self.select_equip:IsStarGod() and Language.Workshop.JewelGodLimitLVLack or Language.Workshop.JewelLimitLVLack)
        if desc > 0 then 
            PublicPopupCtrl.Instance:Center(Language.Workshop.InsetStoneEmpty..string.format(self.select_equip:IsStarGod() and Language.Workshop.JewelGodLimitLV or Language.Workshop.JewelLimitLV,desc))
        end 
        return 
    end 

    local stone_limit = false
    stone_limit,desc = self.data:GetStoneLimit(self.select_equip,data)
    if not stone_limit then 
        PublicPopupCtrl.Instance:Center(Language.Workshop.InsetStone..desc)
        return 
    end 

    self:AdjustFlyPos()
    self.ctrl:SendOperateReqWithParamt(param_t)
    AudioMgr:PlayEffect(AudioType.UI,  "treasure_hunt_start")
    local role_level = RoleData.Instance:GetRoleLevel()
    if stone_list[EQUIP_STONE_COUNT_MAX] > 0 then
        return
    end
    -- LogError("?!?",data)
    for i = 1, EQUIP_STONE_COUNT_MAX do
        local flag,limit_level = self.data:GetStoneSortLimit(self.select_equip,i)
        if stone_list[i] == 0 and flag then
            self.block:SetActive(true)
            return
        end
    end
end

-- 卸下 data = 宝石
function WorkshopInsetNeo:OnClickRemove(data)
    local param_t = self:CatchSendData(data,WORLSHOP_OP.WORKSHOP_OP_GEM_DISMOUNT)
    --宝石所占孔 0或者1
    self.ctrl:SendOperateReqWithParamt(param_t)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ZhuangBeiFuShuJian)
end 

-- 升级 data = 宝石
function WorkshopInsetNeo:OnClickUpgrade(data)
    local limit,desc = self.data:GetStoneLimitLevelUp(self.data:GetCurSelectEquipInset(),data)
    if not limit then 
        PublicPopupCtrl.Instance:Center(desc)
        return 
    end 

    local param_t = self:CatchSendData(data,WORLSHOP_OP.WORKSHOP_OP_GEM_UPGRADES)
    if param_t.param1 ~= nil and param_t.param1 ~= 0 and param_t.param1 ~= 1 then
        BuglyReport("1730 op 6 param1 illegal",param_t)
        PublicPopupCtrl.Instance:Center(Language.Workshop.GemUpgradesError)
        return
    end
    --宝石所占孔 0或者1
    self.ctrl:SendOperateReqWithParamt(param_t)
end

function WorkshopInsetNeo:OnClickSellStone() 
    MarketCtrl.Instance:OnMarketSell()
end

-- 特效开始点变换
function WorkshopInsetNeo:AdjustFlyPos()
    local stone_list = self.select_equip:Stone()
    local index = -1 

    for k,v in pairs(stone_list) do 
        if v == 0 then 
            index = k 
            break
        end 
    end 

    self.Operate.transform:SetParent(self.mono.EffectStartObj.transform,false)
    UH.LocalPosG(self.Operate.transform, Vector3.zero)
    self.Operate.transform:SetParent(self.part_center.transform,true)

    if -1 == index or index > EQUIP_STONE_COUNT_MAX then
        return
    end 

    self.FlyEffects[index]:AdjectPos(self.Operate.transform.localPosition,"StartPos")
end


function WorkshopInsetNeo:CatchSendData(data,oper)
    local param_t = {}
    param_t.op_type = oper
    param_t.param1 = self.select_equip:IsEquip() and 0 or 1
    param_t.param2 = self.select_equip.in_bag == true and 1 or 0
    param_t.param3 = self.select_equip.index
    param_t.param4 = data ~= nil and data.index or nil
    return param_t
end

function WorkshopInsetNeo:FlushCenterInfo()
    if self.select_equip == nil then return end 
    
    self.EquipItem:SetData(self.select_equip)
    UH.SetText(self.LblName, self.select_equip:Name())
    UH.SetText(self.LblLevel,self.select_equip:IsStarGod() and string.format(Language.DivineCostume.Grade,self.select_equip:StrengthenLevel()) or Language.LvDot(self.select_equip:Level()))
    UH.SetText(self.LblType,self.select_equip:TypeName())

    local base_attr = self.select_equip:BaseAttr()
    local base_list = {}
    for k, v in ipairs(base_attr) do
        if v.attr_value ~= 0 then
            local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, 
                Language.Common.AttrList[v.attr_type], 
                DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)
            table.insert( base_list, {str,v.attr_type} ) 
        end
    end

    local has_inc, _inc_list = self.select_equip:IncAttr()
    local inc_list = {}
    for i = 1,6 do
        if _inc_list[i].attr_value ~= 0 then
            local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr3, 
            QualityColorStrShallow[_inc_list[i].color],
                Language.Common.AttrList[_inc_list[i].attr_type],
                DataHelper.IsPercent(_inc_list[i].attr_type) and Percent(_inc_list[i].attr_value) or _inc_list[i].attr_value)
            table.insert( inc_list, {str,_inc_list[i].attr_type} ) 
        end
    end

    self.BaseAttr:SetData(base_list)
    -- 取消显示熔炼属性
    -- self.IncAttr:SetData(inc_list)

    local stone_list = self.select_equip:Stone()
    for i = 1, EQUIP_STONE_COUNT_MAX do
        self.StoneItemList[i]:SetData(Item.Create({item_id = stone_list[i], index = i - 1}))
    end
end 

function WorkshopInsetNeo:FlushRightInfo()
    if self.select_equip then
        local stone_data = self.data:StoneShowList(self.select_equip:EquipType())
        local temp_data = {}
        for k, v in pairs(stone_data) do
            table.insert(temp_data, v)
        end
        table.sort(temp_data, function (a,b)
            if not a or not b then return a end
            local a_num = a.num > 0 and 1000000 or 100000
            local b_num = b.num > 0 and 1000000 or 100000

            local a_info = self.data:GetStoneInfo(a.item_id)
            local b_info = self.data:GetStoneInfo(b.item_id)

            local a_yes_flag = WorkshopData.Instance:GetProfRecommendAllPart(a.item_id) and 10000 or 0
            local a_no_flag = WorkshopData.Instance:GetProfNotRecommendAllPart(a.item_id) and -10000 or 0 

            local b_yes_flag = WorkshopData.Instance:GetProfRecommendAllPart(b.item_id) and 10000 or 0
            local b_no_flag = WorkshopData.Instance:GetProfNotRecommendAllPart(b.item_id) and -10000 or 0

            local a_num = (a_yes_flag + a_no_flag + a_num) + (a_info.value_1 ~= 0 and (a_info.type_1 * 1000 + a_info.value_1) or (a_yes_flag + a_no_flag + a_num))
            local b_num = (b_yes_flag + b_no_flag + b_num) + (b_info.value_1 ~= 0 and (b_info.type_1 * 1000 + b_info.value_1) or (b_yes_flag + b_no_flag + b_num))

            return a_num > b_num
        end)

        self.StoneList:SetData(temp_data)
    end
end