WorkshopInset = WorkshopInset or DeclareMono("WorkshopInset", UIWFlushPanel)
-- 显示背包和身上的装备
function WorkshopInset:WorkshopInset()
    self.data = WorkshopData.Instance
    self.ctrl = WorkshopCtrl.Instance
    self.select_equip = nil
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushRightItem, init_call = false},
        {data = BagData.Instance.equipment_list, func = self.FlushRightItem, init_call = false},
        {data = BagData.Instance.jewelry_list, func = self.FlushRightItem, init_call = false}
    }
    self.lang = Language.Workshop
    self.effect_play_list = {}

    self.first_index = 1
    GuideManager.Instance:RegisterGetGuideUi("WorkshopStoneUp", function ()
        if self.StoneList:SetupCompleted() then
            return self.StoneList.item_list[1].BtnInset--[[ , function ()
                self.StoneList.item_list[1].ItemClick:Trigger()
            end ]]
        end
        return nil,nil
    end)
    
    self.EquipItem:SetData({item_id = 0})
    self.InsetTips.text = self.data:GetProfRecommend()
end
function WorkshopInset:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[20].desc})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2) 
end
function WorkshopInset:onFlush()
    local data = self.data:GetSmeltEquipList()
    local limit_info = self.data:GetStoneLimitLv()
    for k, v in pairs(limit_info) do
        UH.SetText(self.LimitList[k], string.format(self.lang.LimitLv, v.limit_level))
    end
    self.None:SetActive(next(data) == nil)
    if next(data) == nil then
        return
    end
    self.EquipList:SetData(data)
    local index_pos = 1

    if self.data:GetQuickJewelIndex() > 0 then 
        for k,v in pairs(data) do 
            if k == self.data:GetQuickJewelIndex() then 
                self.EquipList:ClickItem(k)
                index_pos = k
                break
            end 
        end 
        if self.flush_left_item_handle ~= nil then
            TimeHelper:CancelTimer(self.flush_left_item_handle)
            self.flush_left_item_handle = nil
        end

        if self.data:GetQuickJewelIndex() > 6 then 
            self.flush_left_item_handle = TimeHelper:AddDelayFrameTimer(
                function()
                    self.flush_left_item_handle = nil
                    self.EquipList:JumpVerticalIndex(index_pos,96)
                end,
                10
            )
        end 
        WorkshopData.Instance:ClearQuickJewelIndex()
    else 
        self.EquipList:ClickItem(1)
    end
    --local accdata = self.data:GetStoneAccorCfg(data[1]:EquipType())
    --self.StoneAccord:SetData(accdata)
    local stone_data = self.data:StoneShowList(data[1]:EquipType())
    local temp_data = {}
    for k, v in pairs(stone_data) do
        table.insert(temp_data, v)
    end
    self.StoneList:SetData(temp_data)
end
function WorkshopInset:FlushRightItem()
    -- 晚一帧
    if self.handle_delay_frame ~= nil then 
        TimeHelper:CancelTimer(self.handle_delay_frame)
    end 

    self.handle_delay_frame = TimeHelper:AddDelayFrameTimer(
        function()
            if not ViewMgr:IsOpen(Workshop) then return end 

            local data = self.data:GetAllEquip()
            self.EquipList:SetData(data)
            self.None:SetActive(next(data) == nil)
            self:FlushEquipInfo()
            -- self.StoneList:SetData(self.data:StoneShowList(data[1]:EquipType()))
            self:FlushShow()
        end,
        10
    )
end
function WorkshopInset:OnClickItem(data, item)
    UH.SetText(self.LblName, data:Name())
    UH.SetText(self.LblType, data:TypeName())
    UH.SetText(self.LblLevel, string.format("Lv.%d", data:LimitLv()))
    self.EquipItem:SetData(data)
    self.select_equip = data
    self.select_item = item
    self.data:SetCurSelectEquipInset(data)
    if data.in_bag then
        self.equip_care =
            self:CareData(
            self.equip_care,
            BagData.Instance:GetColumnGrids(ItemColumnType.Item),
            self.FlushEquipInfo,
            {data.index}
        )
    else
        self.equip_care =
            self:CareData(self.equip_care, BagData.Instance:GetEquipList(), self.FlushEquipInfo, {data.index})
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end
function WorkshopInset:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    GuideManager.Instance:UnRegisterGetGuideUi("WorkshopStoneUp")
    self.data:SetCurSelectEquipInset(nil)
    for index, value in ipairs(self.effect_play_list) do
        value:DeleteMe()
    end
    if self.flush_left_item_handle ~= nil then
        TimeHelper:CancelTimer(self.flush_left_item_handle)
        self.flush_left_item_handle = nil
    end
    if self.handle_delay_frame ~= nil then 
        TimeHelper:CancelTimer(self.handle_delay_frame)
    end 
end
function WorkshopInset:FlushEquipInfo(old)
    if self.select_equip == nil then return end
    local index = self.select_equip.index
    local equip
    if self.select_equip.in_bag then
        equip = BagData.Instance:GetColumnGrids(ItemColumnType.Item)[index]
    else
        if self.select_equip:IsEquip() then 
            equip = BagData.Instance:GetEquipList(index)
        else 
            equip = BagData.Instance:GetJewelryList(index)
        end 
    end
    -- 判断宝石变化播放特效
    local stone_list = equip:Stone()
    local limit_info = self.data:GetStoneLimitLv()
    if old then
        local _, old_equip = next(old)
        local isPlay = false
        local index = 1
        for i = 1, 2 do
            if equip:Stone()[i] ~= 0 and old_equip:Stone()[i] == 0 then
                isPlay = true
                index = i
                break
            end
            if stone_list[i] ~= 0 and old_equip:Stone()[i] ~= 0 and old_equip:Stone()[i] ~= stone_list[i] then
                self.EffectTool:Play(10010258, self.StoneObjList[i])
                AudioMgr:PlayEffect(AudioType.UI, "tongyongjinjie01")
            end
        end
        if isPlay and self.mono then
            --[[ if self.effect_play_list[index] then
                self.effect_play_list[index]:DeleteMe()
                self.effect_play_list[index] = nil
            end ]]
            for _, value in ipairs(self.effect_play_list) do
                value:DeleteMe()
            end
            self.effect_play_list[index] = StoneLevelUpEffect.New(self, index)
            self.effect_play_list[index]:Play()
        end
    end

    for i = 1, 2 do
        local item = Item.New()
        item.item_id = stone_list[i]
        item.index = i - 1
        item.stone_limit_level = limit_info[i].limit_level
        self["Stone" .. i]:SetData(item)
    end
    -- --下面的属性排版要改
    -- local has_inc, inc_list = equip:IncAttr()
    -- -- self.ZhuanJing:SetActive(has_inc)
    -- for i = 1, 5 do
    --     local attr_list = equip.param.attr_list
    --     self.AttrList[i]:SetActive(attr_list[i].attr_value > 0)
    --     if attr_list[i].attr_value > 0 then
    --         UH.SetText(self.ValueList[i], attr_list[i].attr_value)
    --         UH.SetText(
    --             self.TitleList[i],
    --             string.format(
    --                 Language.ItemInfo.ItemTitles.EquipAttr2,
    --                 Language.ItemInfo.AttrList[attr_list[i].attr_type],
    --                 attr_list[i].attr_value
    --             )
    --         )
    --         -- UH.Color(self.TitleList[i],QualityColorStrShallow[attr_list[i].color])
    --     -- Language.AttrItems.AttrNames[attr_list[i].attr_type])
    --     end
    --     --[[ if inc_list[i].attr_value > 0 then
    --         UH.SetText(self.AddList[i], inc_list[i].attr_value)
    --     end ]]
    -- end
    -- self.AttrList[6]:SetActive(has_inc)
    -- if has_inc then
    --     local str = ""
    --     if equip.param.inc_num > 1 then
    --         str =
    --             string.format(
    --             Language.ItemInfo.ItemTitles.PointAttr2,
    --             Language.Common.AttrList[inc_list[1].attr_type],inc_list[1].attr_value,
    --             Language.Common.AttrList[inc_list[2].attr_type] .. "+" .. inc_list[2].attr_value
    --         )
    --     else
    --         str =
    --             string.format(
    --             Language.ItemInfo.ItemTitles.PointAttr2,
    --             Language.Common.AttrList[inc_list[1].attr_type],
    --             inc_list[1].attr_value,
    --             ""
    --         )
    --     end
    --     UH.SetText(self.TitleList[6], str)
    -- end
    -- self.AttrList[7]:SetActive(equip.param.sp_id > 0)
    -- if equip.param.sp_id > 0 then
    --     UH.SetText(self.ValueList[7], Equip.GetEffectDesc(equip.param.sp_id))
    -- end
    --临时性改版
    local attr_list = equip.param.attr_list
    local has_inc, inc_list = equip:IncAttr()
    self.AttrList[1]:SetActive(true)
    self.AttrList[2]:SetActive(has_inc)
    self.AttrList[3]:SetActive(equip.param.sp_id ~=nil and equip.param.sp_id > 0)
    local str_base = nil
    local attr_index = 0
    for k,v in pairs(attr_list) do 
        if v.attr_value > 0 then 
            attr_index = attr_index + 1
            local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr2, 
                Language.Common.AttrList[v.attr_type], 
                DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)
            
            if str_base == nil then
                str_base = str
            elseif attr_index % 2 == 0 then 
                str_base = str_base.."        "..str
            elseif attr_index % 2 == 1 then 
                str_base = str_base.. "\n"..str 
            end 
        end 
    end 
    UH.SetText(self.TitleList[1], str_base)

    attr_index = 0
    local str_inc = ""
    for k,v in pairs(inc_list) do 
        if v.attr_value > 0 then 
            attr_index = attr_index + 1
            local str = string.format(Language.ItemInfo.ItemTitles.EquipAttr3, 
                QualityColorStrShallow[v.color],
                Language.Common.AttrList[v.attr_type],
                DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)

            if str_inc == "" then
                str_inc = str
            elseif attr_index % 2 == 0 then 
                str_inc = str_inc .. "        "..str
            elseif attr_index % 2 == 1 then 
                str_inc = str_inc .. "\n" ..str
            end 
        end 
    end 
    UH.SetText(self.TitleList[2], str_inc)

    if equip.param.sp_id ~=nil and equip.param.sp_id > 0 then 
        local str = string.format(Language.ItemInfo.ItemTitles.EffectOtE, COLORSTR.Purple1, Equip.GetEffectDesc(equip.param.sp_id))
        UH.SetText(self.TitleList[3], str)
    end 

    self:FlushShow()
end

function WorkshopInset:OnClickItem2(mono)
    -- 播放特效 还不能在这里播
    -- self.handle = self.EffectTool:Play(10010254)
    self.mono = mono
end
-- 镶嵌 0 是装备着，1 是在背包
function WorkshopInset:OnClickStone(_, data)
    if self.select_equip == nil then return end 
    -- Debuger.LogError(Serialize(data))
    local param1 = self.select_equip:IsEquip() and 0 or 1
    local param2 = self.select_equip.in_bag == true and 1 or 0
    local param3 = self.select_equip.index
    local param4 = data.index
    if param4 == nil then
        self.data:SetMateData(data)
        ViewMgr:OpenView(WorkShopQuickBuy)
        return
    end
    --宝石所占格子index
    self.ctrl:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_GEM_MOUNTING, param1, param2, param3,param4)
    -- 关心背包中的这个格子并进行刷新
    -- self.stone_care = self:CareData(self.stone_care, BagData.Instance:GetColumnGrids(ItemColumnType.Item), self.FlushShow, {data.index})
end
function WorkshopInset:OnClickStone2(data)
    if self.select_equip == nil then return end 
    -- Debuger.LogError(Serialize(data))
    local param1 = self.select_equip:IsEquip() and 0 or 1
    local param2 = self.select_equip.in_bag == true and 1 or 0
    local param3 = self.select_equip.index
    local param4 = data.index
    if param4 == nil then
        self.data:SetMateData(data)
        ViewMgr:OpenView(WorkShopQuickBuy)
        return
    end
    --宝石所占格子index
    self.ctrl:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_GEM_MOUNTING, param1, param2, param3,param4)
    -- 关心背包中的这个格子并进行刷新
    -- self.stone_care = self:CareData(self.stone_care, BagData.Instance:GetColumnGrids(ItemColumnType.Item), self.FlushShow, {data.index})
end

function WorkshopInset:FlushShow()
    -- local accdata = self.data:GetStoneAccorCfg(self.select_equip:EquipType())
    -- self.StoneAccord:SetData(accdata, true)
    if self.select_equip then
        -- self.StoneList:SetData(self.data:StoneShowList(self.select_equip:EquipType()))
        local stone_data = self.data:StoneShowList(self.select_equip:EquipType())
        local temp_data = {}
        for k, v in pairs(stone_data) do
            table.insert(temp_data, v)
        end
        table.sort(temp_data, function (a,b)
            if not a or not b then return a end
            local a_num = a.num > 0 and 100000 or 10000
            local b_num = b.num > 0 and 100000 or 10000

            local a_info = self.data:GetStoneInfo(a.item_id)
            local b_info = self.data:GetStoneInfo(b.item_id)

            local a_num = a_info.value_1 ~= 0 and (a_info.type_1 * 1000 + a_info.value_1) + a_num or a_num
            local b_num = b_info.value_1 ~= 0 and (b_info.type_1 * 1000 + b_info.value_1) + b_num or b_num

            return a_num > b_num
        end)
        -- DataSorter.KeyUpperSorter("num"))
        self.StoneList:SetData(temp_data)
    end
end
-- 卸下 data = 宝石
function WorkshopInset:OnClickRemove(data)
    self.mono = nil
    local param1 = self.select_equip:IsEquip() and 0 or 1
    local param2 = self.select_equip.in_bag == true and 1 or 0
    local param3 = self.select_equip.index
    local param4 = data.index
    --宝石所占孔 0或者1
    self.ctrl:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_GEM_DISMOUNT, param1, param2, param3,param4)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ZhuangBeiFuShuJian)
end
function WorkshopInset:OnClickUpgrade(data)
    local param1 = self.select_equip:IsEquip() and 0 or 1
    local param2 = self.select_equip.in_bag == true and 1 or 0
    local param3 = self.select_equip.index
    local param4 = data.index
    --宝石所占孔 0或者1
    self.ctrl:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_GEM_UPGRADES, param1, param2, param3,param4)
end

function WorkshopInset:OnClickSellStone() 
    MarketCtrl.Instance:OnMarketSell()
end

StoneLevelUpEffect = StoneLevelUpEffect or BaseClass()
function StoneLevelUpEffect:__init(mono, index)
    self.WI = mono
    self.index = index
end
function StoneLevelUpEffect:__delete()
    if self.handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run)
    end
    if self.handle_delay then
        TimeHelper:CancelTimer(self.handle_delay)
    end
end
function StoneLevelUpEffect:Play()
    local time = 0
    self.WI.mono.EffectStartObj.transform:SetParent(self.WI.gameObject.transform)
    local effectParent = self.WI.EndEffectList[self.index]
    local effectNode = self.WI.EffectNodeList[self.index]
    local startPos = self.WI.mono.EffectStartObj.localPosition
    local endPos = effectParent.transform.localPosition
    local crossPos = self.WI.CrossObj.localPosition
    self.WI.mono.EffectStartObj.transform:SetParent(self.WI.mono.gameObject.transform)
    local shoot_curve = Game.CurveParabola(startPos, endPos, crossPos, 0.5)
    UH.LocalPos(effectNode.transform, startPos)
    -- shoot_curve:TotalTime(20)                    
    if self.WI.EffectTool.isActiveAndEnabled then 
        self.WI.EffectTool:Play(10010254, effectNode)
    end 
    self.handle_run = Runner.Instance:RunUntilTrue(
        function()
            if shoot_curve then
                time = time + Time.deltaTime * 3
                if time >= 1 then
                    if self.WI.EffectTool.isActiveAndEnabled then 
                        self.WI.EffectTool:Play(10010255, effectParent)
                    end 
                    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ZhuangBeiFuShuJia)
                    self.handle_delay = TimeHelper:AddDelayTimer(
                        function()
                            if self.WI.select_item then
                                 self.WI.select_item:Play()
                            end
                        end,
                        0.2
                    )
                    return true
                end
                UH.LocalPos(effectNode.transform, shoot_curve:GetPos(time))
            end
            return false
        end
    )
end


------------------------WorkshopEquipItem------------------------
WorkshopEquipItem = WorkshopEquipItem or DeclareMono("WorkshopEquipItem", UIWidgetBaseItem)
local workshop_stone_qua = {
    [0] = "xiangqian_di1",
    [1] = "xiangqian_di1",
    [2] = "pinzhilv",
    [3] = "pinzhilan",
    [4] = "pinzhizi",
    [5] = "pinzhicheng",
    [6] = "pinzhihong"
}
function WorkshopEquipItem:WorkshopEquipItem()
    self.limit_info = WorkshopData.Instance:GetStoneLimitLv()
    self.red_point = nil
end
function WorkshopEquipItem:SetData(data)
    self.EquipCell:SetData(data)
    UH.SetText(self.LblName, data:Name())
    UH.SetText(self.LblType, data:TypeName())
    self.ImgWear:SetActive(not data.in_bag)

    local stone = data:Stone()
    local level = RoleData.Instance:GetRoleLevel()

    local small_stone_data = {}
    for i = 1, EQUIP_STONE_COUNT_MAX do
        local t = {}
        t.icon_id = Item.GetIconId(stone[i])
        t.bg_name = stone[i] ~= 0 and workshop_stone_qua[Item.GetColor(stone[i])] or workshop_stone_qua[0]
        local equip_sort_limit,desc = WorkshopData.Instance:GetStoneSortLimit(data,i)
        -- local flag = level >= self.limit_info[i].limit_level
        if stone[i] == 0 then
            t.is_lock = not equip_sort_limit
        else
            t.is_lock = false
        end
        table.insert(small_stone_data, t)
    end
    self.SmallStoneList:SetData(small_stone_data)

    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.gameObject,Vector3.New(150,32,0))
    end
    local red_point_num = 0
    for i = 1, EQUIP_STONE_COUNT_MAX do
        if red_point_num == 0 then
            -- local flag = level >= self.limit_info[i].limit_level
            local equip_sort_limit,desc = WorkshopData.Instance:GetStoneSortLimit(data,i)
            if equip_sort_limit then
                if stone[i] == 0 then
                    if not data.in_bag then
                        red_point_num = red_point_num + (WorkshopData.Instance:GetStoneInBag(data:EquipType()) and 1 or 0)
                    end
                else
                    red_point_num = red_point_num + WorkshopData.Instance:GemUplevel(stone[i],data)
                end
            end
        else
            break
        end
    end
    self.red_point:SetNum(red_point_num)
    self.TagHs:SetActive(not AnotherData.Instance:CheckEquipmentUniqueId(data,data:IsEquip()))
    UIWidgetBaseItem.SetData(self, data)
end

function WorkshopEquipItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function WorkshopEquipItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    if self.EffectTool and self.handle_effect then
        self.EffectTool:Stop(self.handle_effect)
    end
end

function WorkshopEquipItem:Play()
    if self.gameObject and self.data and self.EffectTool.isActiveAndEnabled then
        self.handle_effect = self.EffectTool:Play(10010259,self.gameObject)
    end
end

function WorkshopEquipItem:OnEffectComplete()
    if self.block ~= nil then 
        self.block:SetActive(false)
    end 
end


------------------------WorkshopSmallEquipItem------------------------
WorkshopSmallEquipItem = WorkshopSmallEquipItem or DeclareMono("WorkshopSmallEquipItem", UIWidgetBaseItem)
function WorkshopSmallEquipItem:SetData(data)
    UH.SetIcon(self.ImgSetStone, data.icon_id)
    UH.SpriteName(self.ImgSetBg, data.bg_name)
    self.ObjLock:SetActive(data.is_lock)
end


------------------------StoneChildItem------------------------
StoneChildItem = StoneChildItem or DeclareMono("StoneChildItem", UIWidgetBaseItem)
function StoneChildItem:StoneChildItem()
    self.workshop_data = WorkshopData.Instance
    self.lang = Language.Workshop
    self.red_point = nil
    self.empty_red_point = nil
    self.Cell:SetData({item_id = 0})
end
-- 宝石物品
function StoneChildItem:SetData(data)
    local info = self.workshop_data:GetStoneInfo(data.item_id)
    if info ~= nil then
        UH.SetText(self.LblName, data:QuaName())
        UH.SetText(
            self.Attr1,
            info.value_1 ~= 0 and string.format(self.lang.AttrStr, Language.Common.AttrList[info.type_1], info.value_1) or
                ""
        )
        UH.SetText(
            self.Attr2,
            info.value_2 ~= 0 and string.format(self.lang.AttrStr, Language.Common.AttrList[info.type_2], info.value_2) or
                ""
        )
    else
        UH.SetText(self.Attr1, "")
        UH.SetText(self.Attr2, "")
        UH.SetText(self.LblName, "")
    end
    if self.BtnUnFix then
        self.BtnUnFix:SetActive(data.item_id > 0)
    end
    if self.Lock then
        self.Lock:SetActive(false)
    end
    if self.BtnUpgrade then
        self.BtnUpgrade:SetActive(data.item_id > 0)
    end
    if self.LimitObj then
        if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.gameObject,Vector3.New(50,-50,0))
        end
        if self.empty_red_point == nil then
            self.empty_red_point = UiRedPoint.New(self.gameObject,Vector3.New(-80,80,0))
        end
        if data.item_id > 0 then
            local num = WorkshopData.Instance:GemUplevel(data.item_id,self.workshop_data:GetCurSelectEquipInset())
            self.red_point:SetNum(num)
        else
            self.red_point:SetNum(0)
        end
        self.LimitObj:SetActive(data.item_id == 0)
        local level = RoleData.Instance:GetRoleLevel()
        -- local flag,limit_level = self.workshop_data:GetStoneSortLimit(self.index)
        --index 在预设中添加
        -- local flag = level >= self.workshop_data:GetStoneLimitLv()[self.index].limit_level -- data.stone_limit_level
        local equip_data = self.workshop_data:GetCurSelectEquipInset()
        local flag,limit_desc = self.workshop_data:GetStoneSortLimit(equip_data,self.index)
        UH.SetText(
            self.LblLimit,
            flag and self.lang.StoneOpen or string.format(equip_data:IsStarGod() and self.lang.GodLimitLv or self.lang.nomLimitLv,limit_desc)
        )
        UH.Color(self.LblLimit, flag and COLORS.Red2 or COLORS.Red7)
        self.Lock:SetActive(not flag)

        if data.item_id == 0 and flag and self.workshop_data:GetCurSelectEquipInset() 
        and self.workshop_data:GetStoneInBag(self.workshop_data:GetCurSelectEquipInset():EquipType()) then
            self.empty_red_point:SetNum(1)
        else 
            self.empty_red_point:SetNum(0)
        end 
        if self.None then
            if data.item_id > 0 then
                self.None:SetActive(false)
            else
                self.None:SetActive(flag)
            end
        end
    end
    --右侧按钮显示带0的
    if self.IsRight then
        local item = Item.Create({item_id = data.item_id})
        -- UH.SpriteName(self.BgSet, item:Num() == 0 and "XianQianDi3" or "XiangQianDi")
        -- self.BtnBuy:SetActive(item:Num() == 0)
        self.Cell:SetDataByItemIdWithZeroNum(data.item_id)
        -- self.Cell:MakeGrey(item:Num() == 0)
        self.BtnInset:SetActive(item:Num() > 0)
        self.CellObj:SetActive(item:Num() > 0)
        self.AddObj:SetActive(item:Num() == 0)
        if self.recommd ~= nil then 
            -- local cur_select = self.workshop_data:GetCurSelectEquipInset()
            -- self.recommd:SetActive(self.workshop_data:GetProfRecommendEquipType(cur_select:EquipType(),data.item_id))
            self.recommd:SetActive(false)
        end 
    else
        self.Cell:SetData(data)
    end
    UIWidgetBaseItem.SetData(self, data)
end

StoneParentItem = StoneParentItem or DeclareMono("StoneParentItem", UIWidgetBaseItem)
function StoneParentItem:SetData(data)
    UH.SetText(self.LblTitle, data.name)
    UIWidgetBaseItem.SetData(self, data)
end
