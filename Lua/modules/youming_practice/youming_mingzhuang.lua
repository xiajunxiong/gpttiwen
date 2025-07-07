YouMingMingZhuang = YouMingMingZhuang or DeclareMono("YouMingMingZhuang", UIWFlushPanel)

function YouMingMingZhuang:YouMingMingZhuang()
    self.data = YouMingData.Instance
    self.data_cares = {{
        data = self.data:GetSelectYouMing(),
        func = self.FlushBaseInfo
    }}
    self.select_index = 1
    self.is_auto_select = false
end
function YouMingMingZhuang:FlushBaseInfo()
    self.youming = self.data:GetSelectYouMing()
    if self.youming.Aptitude == nil then
        return
    end
    self.equip_list = self.youming:EquipList()
    self.List:SetData(self.equip_list)
    -- if not self.equip_list[1]:IsEmpty() then
    self.is_auto_select = true
    self.List:ClickItem(self.select_index)
    -- end
    -- 假设有装备再选择没有不选
    -- 显示进场技能没有不显示
    local skill = self.youming:InbornSkill()
    if skill == nil then
        self.SkillObj:SetActive(false)
    else
        self.SkillCell:SetData(skill)
        UH.SetText(self.SkillName, skill:Name())
        self.SkillObj:SetActive(true)
    end
    
end
function YouMingMingZhuang:OnClickUpLevel()
    if self.equip and self.equip.item_id == 0 then
        return
    end
    self.data:SetSelectYouMingEquip(self.equip)
    ViewMgr:OpenView(YouMingMingZhuangShengJi)
end
function YouMingMingZhuang:OnClickQiangHua()
    if self.equip and self.equip.item_id == 0 then
        return
    end
    self.data:SetSelectYouMingEquip(self.equip)
    ViewMgr:OpenView(YouMingMingZhuangQiangHua)
end
function YouMingMingZhuang:OnClickItem(data)
    self.equip = data
    self.select_index = data.index
    self.data:SetEquipIndex(self.select_index)
    self.EmptyTip:SetActive(data.item_id == 0)
    self.AttrParent:SetActive(data.item_id ~= 0)
    if data.item_id == 0 then
        self.RedPoint:SetNum(0)
        UH.SetText(self.Name, Language.YouMingPractice.ZanWu)
        UH.SetText(self.Level, "")
        self.Cell:SetObjActive(false)
        if self.is_auto_select == false then
            ViewMgr:OpenView(MingZhuangBagView)
        end
        for i = 1, 3 do
            self.MainAttrItems[i]:SetObjActive(false)
        end
        for i = 1, 4 do
            self.DeputyAttrItems[i]:SetObjActive(false)
        end
    else
        self.RedPoint:SetNum(self.youming:MingZhuangCanQiangHua(self.equip))
        -- 上等级 
        -- 下强化
        UH.SetText(self.Name, Format(Language.YouMingPractice.EquipName, data:Name(), data:Level()))
        UH.SetText(self.Level, Format(Language.YouMingPractice.EquipStr, data:StrLevel()))
        self.Cell:SetObjActive(true)
        self.Cell:SetData(data)
        local config = data:CurConfig()
        -- local next_config = data:NextConfig()
        local attr
        for i = 1, 3 do
            attr = {}
            attr.attr_type = config["attr_type_" .. i]
            attr.attr_value = config["attr_add_" .. i]
            -- attr.attr_value2 = next_config and next_config["attr_add_" .. i] or 0
            self.MainAttrItems[i]:SetData(attr)
        end
        local deputyConfig = data:DeputyConfig()
        for i = 1, 4 do
            self.DeputyAttrItems[i]:SetData(deputyConfig[i])
        end
        if self.is_auto_select == false then
            LogDG("打开面板脱下装备")
            if data.item_id > 0 then
                CellClicks.BagGrid(self.Cell)
            else
                ViewMgr:OpenView(MingZhuangBagView)
            end
        end
        -- UH.SetText(self.Name)
    end
    self.is_auto_select = false
end
function YouMingMingZhuang:OnClickTip()
    PublicPopupCtrl.Instance:HelpTip(409)
end

YouMingMingZhuangCell = YouMingMingZhuangCell or DeclareMono("YouMingMingZhuangCell", UIWidgetBaseItem)

function YouMingMingZhuangCell:YouMingMingZhuangCell()
    self.youmin_data = YouMingData.Instance
end
function YouMingMingZhuangCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.youming = self.youmin_data:GetSelectYouMing()
    UH.SetText(self.Name, data:Name())
    if data.item_id == 0 then
        self.Cell:SetObjActive(false)
        self.AddItem:SetActive(true)
        local num = self.youming:IsHasMingZhuang(data.index)
        self.RedPoint:SetNum(num)
    else
        self.Cell:SetObjActive(true)
        self.AddItem:SetActive(false)
        self.Cell:SetData(data)
        local num = self.youming:IsHasHighMingZhuang(data)
        --增加可强化
        self.RedPoint:SetNum(num)
    end
end
function YouMingMingZhuangCell:Click()
    self.EventClick:Trigger()
end
function YouMingMingZhuangCell:OnSelect(selected)
    if selected then
        self:Click()
    end
end

YouMingEquipAttrItem = YouMingEquipAttrItem or DeclareMono("YouMingEquipAttrItem", UIWidgetBaseItem)
function YouMingEquipAttrItem:YouMingEquipAttrItem()

end
function YouMingEquipAttrItem:SetData(data)
    local attr_type = data.attr_type or data.attri_type
    local attr_value = data.attr_value or data.attri_value
    UH.SetText(self.Title, Language.Common.AttrList[attr_type])
    UH.SetText(self.Value, DataHelper.Percent(attr_type, attr_value))
    if attr_type == 0 and attr_value == 0 then
        self.gameObject:SetActive(false)
    else
        self.gameObject:SetActive(true)
    end
end
YouMingMingZhuangShengJi = YouMingMingZhuangShengJi or
                               DeclareView("YouMingMingZhuangShengJi", "youming_practice/youming_mingzhuang_shengji")
function YouMingMingZhuangShengJi:YouMingMingZhuangShengJi()
    self.data = YouMingData.Instance
    self.handle_care = self.data:GetYouMingEquip():Care(BindTool.Bind(self.FlushEquipInfo, self))
    self.sort_type = 1
    self.add_exp = 0
    self.add_level = 0
    self.add_cost = 0
end
function YouMingMingZhuangShengJi:OnGetWayClick()
    local get_way_info = {}
    local get_way_t = Split(Config.dim_pet_cfg_auto.other[1].costume_get_way,"|")
    for k,v in pairs(get_way_t) do
        table.insert(get_way_info, BagData.Instance:GetWayCfg(tonumber(v)))
    end
    ViewMgr:OpenView(PetGuideTipView, {list = get_way_info, tip = Language.YouMingPractice.Text1})
end
function YouMingMingZhuangShengJi:CloseCallback()
    self.sort_type = 1
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)
    TimeHelper:CancelTimer(self.handle_delay3)
    if self.handle_care then
        self.data:GetYouMingEquip():Uncare(self.handle_care)
        self.handle_care = nil
    end
end
function YouMingMingZhuangShengJi:FlushEquipInfo()
    self.equip = self.data:GetYouMingEquip()
    if self.equip == nil then
        return
    end
    if self.equip.Level == nil then
        return
    end
    self.add_level = 0
    self.add_exp = 0
    self.add_cost = 0
    if self.can_play_effct then
        self:PlayEffect()
    else
        self:FlushEquipInfo2()
    end
end
function YouMingMingZhuangShengJi:FlushEquipInfo2()
    self.Cell:SetData(self.equip)
    self:FlushBaseInfo1()
    self.Toggles[self.sort_type].isOn = true
    local deputyConfig = self.equip:DeputyConfig()
    for i = 1, 4 do
        self.DeputyAttrs[i]:SetData(deputyConfig[i])
    end
    self:FlushBagList()
end
function YouMingMingZhuangShengJi:FlushBaseInfo1()
    local cur_level = self.equip:Level() + self.add_level
    local next_level = cur_level + 1
    UH.SetText(self.Name, self.equip:Name())
    UH.SetText(self.StrLevel, Format(Language.YouMingPractice.EquipStr, self.equip:StrLevel()))
    UH.SetText(self.CurLevel, cur_level)
    UH.SetText(self.NextLevel, next_level)
    -- UH.SetText
    local config = self.equip:LevelConfig(next_level)
    local next_config = self.equip:NextConfig()
    self.cost_num = 0
    if config then
        self.cost_num = config.coin_cost + self.add_cost
        UH.SetText(self.Count, self.cost_num)--config.coin_cost)
        local cur_exp = self.equip:CurExp() + self.add_exp
        local max_exp = self.equip:MaxExp()
        if max_exp == 0 then
            self.ExpBar:SetProgress(1)
            if next_config == nil then
                self.ExpBar:SetText(Language.YouMingPractice.MaxLevel)
            else
                self.ExpBar:SetText("")
            end
        else
            if next_config == nil then
                self.ExpBar:SetProgress(1)
                self.ExpBar:SetText(Language.YouMingPractice.MaxLevel)
            else
                self.ExpBar:SetProgress((cur_exp or 0) / max_exp)
                self.ExpBar:SetText(Format("%s/%s", cur_exp, max_exp))
            end
        end
    else
        self.ExpBar:SetProgress(1)
        self.ExpBar:SetText(Language.YouMingPractice.MaxLevel)
    end
    local cur_config = self.equip:CurConfig()
    if cur_config then
        for i = 1, self.MaxRoots:Length() do
            self.MaxRoots[i]:SetActive(true)
        end
        local attr
        for i = 1, 2 do
            attr = {}
            attr.attr_type = cur_config["attr_type_" .. i]
            attr.attr_value = self.equip.level == 0 and 0 or cur_config["attr_add_" .. i]
            attr.attr_value2 = next_config and next_config["attr_add_" .. i] or 0
            attr.ingore_pos = true
            attr.hide_right = next_config and false or true
            self.MainAttrs[i]:SetData(attr)
        end
        if next_config == nil then
            for i = 1, self.MaxRoots:Length() do
                self.MaxRoots[i]:SetActive(false)
            end
        end
        self.MaxRoot:SetActive(next_config == nil)
    end
end
function YouMingMingZhuangShengJi:FlushBagList()
    local list = self.data:GetEquipBagList()
    local new_list = {}
    for k, v in pairs(list) do
        table.insert(new_list, v)
    end
    self.EmptyTip:SetActive(table.nums(new_list) == 0)
    if self.sort_type == 1 then
        table.sort(new_list, function(a, b)
            if a and b then
                return a:Color() > b:Color()
            else
                return false
            end
        end)
    elseif self.sort_type == 2 then
        table.sort(new_list, function(a, b)
            if a and b then
                return a:Color() < b:Color()
            else
                return false
            end
        end)
    elseif self.sort_type == 3 then
        table.sort(new_list, function(a, b)
            if a and b then
                return a:Level() > b:Level()
            else
                return false
            end
        end)
    elseif self.sort_type == 4 then
        table.sort(new_list, function(a, b)
            if a and b then
                return a:Level() < b:Level()
            else
                return false
            end
        end)
    end
    self.List:SetData(new_list)
end
function YouMingMingZhuangShengJi:OnClickItems(cell)
    -- LogDG(cell:IsSelected())

    --需延迟 select 比 click慢 所以获取不准确
    TimeHelper:CancelTimer(self.handle_delay3)
    self.handle_delay3 = TimeHelper:AddDelayFrameTimer(function ()
        local data = self.List:GetSelectedData()
        --计算增加并表现
        local total_exp = 0
        local config
        for i, v in ipairs(data) do
            config = v:LevelConfig()
            total_exp = total_exp + config.fenjie_exp
        end
        self.add_exp = total_exp
        self.add_level = 0
        self.add_cost = 0
        local exp = total_exp
        local cur_level = self.equip:Level()
        local next_level = cur_level + 1
        local config
        while exp > 0 do
            config = self.equip:LevelConfig(next_level)
            if config then
                exp = exp - config.upgrade_exp
                if exp > 0 then
                    self.add_level = self.add_level + 1
                    self.add_cost = self.add_cost + config.coin_cost
                    next_level = next_level + 1
                end
            else
                break
            end
        end        
        self:FlushBaseInfo1()
    end,5)
end
function YouMingMingZhuangShengJi:OnClickSort(type)
    self.sort_type = type
    self:FlushBagList()
end
function YouMingMingZhuangShengJi:LoadCallback()
    self.can_play_effct = false
    self.Board:SetData(self:GetType(), Language.YouMingPractice.MZShengJi, Vector2.New(954, 677))
    self:FlushEquipInfo()
end
function YouMingMingZhuangShengJi:PlayEffect()
    self.EffectTool:Play(7167027)
    self.EffectTool:Play(7167028)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)
    self.handle_delay = TimeHelper:AddDelayTimer(function()
        self.can_play_effct = false
        self:FlushEquipInfo()
    end, 0.3)
end
function YouMingMingZhuangShengJi:OnClickUplevel()
    local data = self.List:GetSelectedData()
    local list = {}
    for i, v in ipairs(data) do
        table.insert(list, v.index)
    end
    if table.nums(list) > 0 and RoleData.Instance:GetCurrency(CurrencyType.CoinBind) >= self.cost_num then
        self.can_play_effct = true
    end
    local youming = self.data:GetSelectYouMing()
    YouMingCtrl.Instance:SendEquipAddExpReq(youming.index, self.equip.index - 1, list)
    -- OperateFrequency.Operate(function()
    -- end, "youming_mingzhuang_shengji", 1)
end

YouMingMingZhuangQiangHua = YouMingMingZhuangQiangHua or
                                DeclareView("YouMingMingZhuangQiangHua", "youming_practice/youming_mingzhuang_qianghua")
function YouMingMingZhuangQiangHua:YouMingMingZhuangQiangHua()
    self.data = YouMingData.Instance
    self.handle_care = self.data:GetYouMingEquip():Care(BindTool.Bind(self.FlushEquipInfo, self))
end
function YouMingMingZhuangQiangHua:CloseCallback()
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)
    if self.handle_care then
        self.data:GetYouMingEquip():Uncare(self.handle_care)
        self.handle_care = nil
    end
end
function YouMingMingZhuangQiangHua:FlushEquipInfo()
    self.equip = self.data:GetYouMingEquip()
    if self.equip == nil then
        return
    end
    if self.can_play_effct then
        self:PlayEffect()
    else
        self:FlushEquipInfo2()
    end
end
function YouMingMingZhuangQiangHua:FlushEquipInfo2()
    self.Cell:SetData(self.equip)
    UH.SetText(self.Name, Format(Language.YouMingPractice.EquipName, self.equip:Name(), self.equip:Level()))
    local cur_config = self.equip:QHLevelConfig(self.equip:StrLevel())
    local next_config = self.equip:QHLevelConfig(self.equip:StrLevel() + 1)
    if cur_config then
        if next_config then
            local list = {}
            if next_config.cost_self > 0 then
                table.insert(list, {item_id = self.equip.id, num = next_config.cost_self})
            end
            table.insert(list, next_config.strengthen_consume_item[0])
            self.items = list--next_config.strengthen_consume_item
            self.List:SetData(DataHelper.FormatItemList(list))
        else
            local list = {}
            if cur_config.cost_self > 0 then
                table.insert(list, {item_id = self.equip.id, num = cur_config.cost_self})
            end
            table.insert(list, cur_config.strengthen_consume_item[0])
            self.items = list--next_config.strengthen_consume_item
            self.List:SetData(DataHelper.FormatItemList(list))
        end
    else
        -- 0强化
        if next_config then
            local list = {}
            if next_config.cost_self > 0 then
                table.insert(list, {item_id = self.equip.id, num = next_config.cost_self})
            end
            table.insert(list, cur_config.strengthen_consume_item[0])
            self.items = list--next_config.strengthen_consume_item
            self.List:SetData(DataHelper.FormatItemList(list))--next_config.strengthen_consume_item))
        end
    end
    local cur_config = self.equip:CurQHConfig() -- Cfg.YouMingQiangHua(self.equip.id, self.equip:StrLevel(), false)
    local next_config = self.equip:NextQHConfig() -- Cfg.YouMingQiangHua(self.equip.id, self.equip:StrLevel() + 1, false)
    local attr
    for i = 1, 3 do
        attr = {}
        attr.attr_type = cur_config and cur_config["attr_type_" .. i] or next_config["attr_type_" .. i]
        attr.attr_value = cur_config and cur_config["attr_add_" .. i] or 0
        attr.attr_value2 = next_config and next_config["attr_add_" .. i] or 0
        self.CurItems[i]:SetData(attr)
    end
    for i = 1, 3 do
        attr = {}
        attr.attr_type = next_config and next_config["attr_type_" .. i] or 0
        attr.attr_value = next_config and next_config["attr_add_" .. i] or 0
        attr.attr_value2 = next_config and next_config["attr_add_" .. i] or 0
        self.NextItems[i]:SetData(attr)
    end
    self.MaxRoot:SetActive(next_config == nil)
    if next_config == nil then
        for i = 1, self.MaxRoots:Length() do
            self.MaxRoots[i]:SetActive(false)
        end
        UH.AnchoredPosition(self.CurItemsRect, Vector2.New(128, 0))
    else
        UH.AnchoredPosition(self.CurItemsRect, Vector2.New(0, 0))
        for i = 1, self.MaxRoots:Length() do
            self.MaxRoots[i]:SetActive(true)
        end
    end
end
function YouMingMingZhuangQiangHua:PlayEffect()
    self.EffectTool:Play(7167025)
    TimeHelper:CancelTimer(self.handle_delay)
    TimeHelper:CancelTimer(self.handle_delay2)
    self.handle_delay = TimeHelper:AddDelayTimer(function()
        self.EffectTool:Play(3165130)
        self.handle_delay2 = TimeHelper:AddDelayTimer(function()
            self.EffectTool:Play(7167026)
            self.can_play_effct = false
            self:FlushEquipInfo()
        end, 1)
    end, 1.45)
end

function YouMingMingZhuangQiangHua:OnClickQiangHua()
    local youming = self.data:GetSelectYouMing()
    if self.items then
        local can_up = false
        for i, v in ipairs(self.items) do
            can_up = false
            if Item.GetNum(v.item_id) > v.num then
                can_up = true
            end
        end
        if can_up == true then
            self.can_play_effct = true
        end
    end
    YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.STR_WEAR_EQUIP, youming.index, self.equip.index - 1)
    -- OperateFrequency.Operate(function()
    -- end, "youming_mingzhuang_qianghua", 2.5)
end

function YouMingMingZhuangQiangHua:LoadCallback()
    self.can_play_effct = false
    self.Board:SetData(self:GetType(), Language.YouMingPractice.MZQiangHua, Vector2.New(954, 677))
    self:FlushEquipInfo()
end
YouMingQiangHuaItem = YouMingQiangHuaItem or DeclareMono("YouMingQiangHuaItem", UIWidgetBaseItem)
function YouMingQiangHuaItem:YouMingQiangHuaItem()
    self.youming_data = YouMingData.Instance
end
function YouMingQiangHuaItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local equip = self.youming_data:GetYouMingEquip()
    if equip and equip.id == data.item_id then
        local num = self.youming_data:GetQHEquipNum(data.item_id)
        local need_num = data.num
        local str = string.format(Language.Common.Cstr, num >= need_num and QualityColorStr[ItemColor.Green] or
            QualityColorStr[ItemColor.Red], num, need_num)
        self.Cell:SetDataByItemIdWithZeroNum(data.item_id, str)
    else
        self.Cell:SetDataByItemIdWithNeed(data.item_id, data.num)
    end
end
YouMingQiangHuaAttrItem = YouMingQiangHuaAttrItem or DeclareMono("YouMingQiangHuaAttrItem", UIWidgetBaseItem)
function YouMingQiangHuaAttrItem:YouMingQiangHuaAttrItem()

end
function YouMingQiangHuaAttrItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.attr_value == 0 and data.attr_value2 == 0 then
        self.gameObject:SetActive(false)
    else
        UH.SetText(self.Desc, Language.Common.AttrList[data.attr_type])
        UH.SetText(self.Value, DataHelper.Percent(data.attr_type, data.attr_value))
        self.gameObject:SetActive(true)
    end
end

-- 造化装备·背包界面
MingZhuangBagView = MingZhuangBagView or
                        DeclareView("MingZhuangBagView", "youming_practice/youming_equip_bag", Mod.Marbas.Bag)
function MingZhuangBagView:MingZhuangBagView()

end
function MingZhuangBagView:LoadCallback(param_t)
    self.Board:SetData(self:GetType(), Language.YouMingPractice.BagView, Vector2.New(590, 565))

end

function MingZhuangBagView:OnCloseClick()
    ViewMgr:CloseView(MingZhuangBagView)
end

--------------------------MingZhuangBagPanel--------------------------
MingZhuangBagPanel = MingZhuangBagPanel or DeclareMono("MingZhuangBagPanel", UIWFlushPanel)
function MingZhuangBagPanel:MingZhuangBagPanel()
    self.data = YouMingData.Instance
    self.data_cares = {{
        data = self.data:GetSelectYouMing(),
        func = self.FlushBag
    }}
end

function MingZhuangBagPanel:onFlush()
    self:FlushBag()
end

function MingZhuangBagPanel:FlushBag()
    --又特么要区分种类
    self.index = self.data:GetEquipIndex()

    local list = self.data:GetEquipBagList()
    local new_list = {}
    for k, v in pairs(list) do
        if v:Cfg().hole == (self.index - 1) then
            table.insert(new_list, v)
        end
    end
    self.EmptyTip:SetActive(table.nums(new_list) == 0)
    self.List:SetData(new_list)
end

function MingZhuangBagPanel:OnClickGetWay()
    local get_way_info = {}
    local get_way_t = Split(Config.dim_pet_cfg_auto.other[1].costume_get_way,"|")
    for k,v in pairs(get_way_t) do
        table.insert(get_way_info, BagData.Instance:GetWayCfg(tonumber(v)))
    end
    ViewMgr:OpenView(PetGuideTipView, {list = get_way_info, tip = Language.YouMingPractice.Text1})
end


YouMingMingZhuangBagItem = YouMingMingZhuangBagItem or DeclareMono("YouMingMingZhuangBagItem", UIWItemCell)
function YouMingMingZhuangBagItem:YouMingMingZhuangBagItem()
    
end
function YouMingMingZhuangBagItem:SetData(data)
    UIWItemCell.SetData(self, data)
end