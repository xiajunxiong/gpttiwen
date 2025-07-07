-- 精工合并到装备系统中
-- WorkshopUltiWork = WorkshopUltiWork or DeclareView("WorkshopUltiWork", "workshop/workshop_ultiwork", Mod.Equip.Ulti)
-- function WorkshopUltiWork:WorkshopUltiWork()
--     self.lang = Language.UltiWork

--     self.AudioOpen = CommonAudio.ItemInfoOpen
--     self.AudioClose = CommonAudio.ItemInfoClose
-- end
-- function WorkshopUltiWork:LoadCallback(data)
--     UH.SetText(self.TitleTex, self.lang.Title)
--     self.Currency:DefaultSet()
-- end
-- function WorkshopUltiWork:OnClickClose()
--     ViewMgr:CloseView(WorkshopUltiWork)
-- end

UltiWorkPanel = UltiWorkPanel or DeclareMono("UltiWorkPanel", UIWFlushPanel)
function UltiWorkPanel:UltiWorkPanel()
    self.data = WorkshopData.Instance
    self.is_strong_ulti = false
    self.data_cares = {
        {data = BagData.Instance.item_grid, func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.equipment_list, func = self.FlushEquip, init_call = false},
        {data = BagData.Instance.jewelry_list, func = self.FlushEquip, init_call = false}
    }
    self.select_index = 1
    self.jump_flag = true

    --[[ self.effect_list = {
        ["11"] = "30000806",
        ["12"] = "30000805",
        ["21"] = "30001820",
        ["22"] = "30000806"
    } ]]
    self.effect_list = {
        ["11"] = "2165017",
        ["12"] = "2165018",
        ["21"] = "2165019",
        ["22"] = "2165020"
    }

    local param_t = {effect_tool = self.EffectTool, effect_id = "30000807"}
    for i = 1, 2 do
        self.BaseAttrRing[i]:SetDevice(param_t)
        self.UltiAttrRing[i]:SetDevice(param_t)
    end
end
function UltiWorkPanel:FlushEquip()
    local data = self.data:GetUltiEquipList()
    self.EquipList:SetData(data)
    self.EquipList:ClickItem(self.select_index)
    --[[ self.EquipList:SetCompleteCallBack(
        function()
            self.EquipList:JumpVerticalIndex(self.select_index, 100)
        end
    ) ]]
    --[[ self:flushAttr()
    self:flushCostItem() ]]
end
function UltiWorkPanel:onFlush()
    local data = self.data:GetUltiEquipList()
    if table.nums(data) == 0 then
        self.ObjTotal:SetActive(false)
        self.ObjEmpty:SetActive(true)
    else
        self.ObjTotal:SetActive(true)
        self.ObjEmpty:SetActive(false)
    end
    self.EquipList:SetData(data)
    local select_item = self.data:GetSelectUltiData()
    if select_item then
        self.EquipList:ClickItemData(select_item)
        self.equip = select_item
        self.EquipList:SetCompleteCallBack(
            function()
                if self.jump_flag then
                    self.select_index = self.EquipList:GetSelectedIndex(self.equip)
                    self.EquipList:JumpVerticalIndex(self.select_index, 95)
                    self.jump_flag = false
                end
            end
        )
    else
        self.EquipList:ClickItem(1)
    end
end

function UltiWorkPanel:OnClickUlti()
    local config = self.data:GetUltiConsume(self.equip)
    for i = 0, 1 do 
        local CostItem = Item.Create(config.added_consume_item[i])
        local need_num = config.added_consume_item[i].num
        self.CostCell:SetDataByItemIdWithNeed(CostItem.item_id, need_num)
        if Item.GetNum(config.added_consume_item[i].item_id) < need_num then
            PublicPopupCtrl.Instance:Center(string.format(ErrorText[5],Item.GetName(config.added_consume_item[i].item_id),
            need_num - Item.GetNum(config.added_consume_item[i].item_id)))

            MainOtherCtrl.Instance:GetWayView({item = Item.New(config.added_consume_item[i].item_id)})
            BagData.Instance:SetGetWaySweepMark({need = need_num})
            return
        end
    end 
    self.operate_set = 2
    local flag = true
    for i = 1, 2 do
        if self.UltiAttrRing[i].gameObject.activeSelf == true then
            flag = flag and self.UltiAttrRing[i]:IsMax()
        end
    end

    if flag then
        PublicPopupCtrl.Instance:Center(Language.UltiWork.MaxTip)
        return
    end

    self.EffectTool:Play(self.effect_list["1" .. self.strong_num], self.EffectPos["Pos_21" .. self.strong_num])

    self.ClickArea:SetActive(true)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.JinJie1)
end
--普通精工
function UltiWorkPanel:OnClickUlti2()
    local config = self.data:GetUltiConsume(self.equip)
    local list = config.common_consume_item
    local temp
    local consume_num = 0
    for i = 1, 3 do
        temp = list[i - 1]
        if temp then
            consume_num = consume_num + 1
            if Item.GetNum(temp.item_id) < temp.num then
                local lack_num = temp.num - Item.GetNum(temp.item_id)
                PublicPopupCtrl.Instance:Center(string.format(ErrorText[5],Item.GetName(temp.item_id),
                temp.num - Item.GetNum(temp.item_id)))

                MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id = temp.item_id,jump_need_num = lack_num})})
                BagData.Instance:SetGetWaySweepMark({need = temp.num})
                return
            end
        end
    end
    local ring_info = self.data:UntilFixRingShow(self.equip) --新增
    -- local attr_list = self.equip.param.attr_list
    -- local attr_seclist = self.equip:AttrSection()
    local flag = true
    for i = 1, #ring_info.base do
        flag = flag and self.BaseAttrRing[i]:IsMax()
    end

    if flag then
        PublicPopupCtrl.Instance:Center(Language.UltiWork.MaxTip)
        return
    end


    self.operate_set = 1
    self.EffectTool:Play(self.effect_list[consume_num..#ring_info.base])
    --[[ if consume_num == 2 and self.equip.param.attr_num == 2 then 
        self.EffectTool:Play(self.effect_list[consume_num..self.equip.param.attr_num],
        self.EffectPos["Pos_1"..consume_num..self.equip.param.attr_num][1])
        self.EffectTool:Play(self.effect_list[consume_num..self.equip.param.attr_num],
        self.EffectPos["Pos_1"..consume_num..self.equip.param.attr_num][2])
    else 
        self.EffectTool:Play(self.effect_list[consume_num..self.equip.param.attr_num],
        self.EffectPos["Pos_1"..consume_num..self.equip.param.attr_num])
    end  ]]
    -- self:OnUltiworkEff()
    self.ClickArea:SetActive(true)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.JinJie1)
end

function UltiWorkPanel:OnUltiworkEff()
    WorkshopCtrl.Instance:SendOperateReq(
        WORLSHOP_OP.WORKSHOP_OP_ULTIMATE_WORK,
        self.operate_set,
        self.equip:IsEquip() and 0 or 1,
        self.equip.in_bag == true and 1 or 0,
        self.equip.index,
        AnotherData.Instance:CheckEquipmentUniqueId(self.equip, self.equip:IsEquip()) and 0 or 1
    )
    self.pausing = true

    self.wait_timer =
        TimeHelper:AddDelayTimer(
        function()
            self.pausing = false
            if self.ClickArea and self.ClickArea.SetActive then 
                self.ClickArea:SetActive(false) 
            end 
            self.operate_set = nil
        end,
        2
    )
end

function UltiWorkPanel:OnSelectEquip(cell)
    cell:SetSelected(true)
    -- cell.Selected:SetActive(selected)
    self.equip = cell:GetData()
    self.EquipCell:SetData(self.equip)
    UH.SetText(self.EquipName, self.equip:QuaName(true))
    self:flushAttr()
    self:flushCostItem()
    self.select_index = self.EquipList:GetSelectedIndex(self.equip)
end
function UltiWorkPanel:OnClickView()
    ViewMgr:OpenView(ItemPreRangeSingleView, {item = self.equip, height = 492})

    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
end
function UltiWorkPanel:flushAttr()
    self:flushBaseAttr()
    -- self:flushStrongAttr()
    -- self:flushCenterPart()
end
function UltiWorkPanel:flushBaseAttr()
    local ring_info = self.data:UntilFixRingShow(self.equip)
    local is_max = true
    for i = 1, 2 do
        self.BaseAttrRing[i].gameObject:SetActive(ring_info.base[i] ~= nil)
        if ring_info.base[i] then
            local is_eff_show = self.operate_set == 1 and self.pausing == true
            ring_info.base[i].is_eff_show = is_eff_show
            self.BaseAttrRing[i]:SetData(ring_info.base[i])
            if ring_info.base[i].cur ~= ring_info.base[i].max then
                is_max = false
            end
        end
    end

    self.ItemListObj:SetActive(not is_max)
    self.AttrNoMax:SetActive(not is_max)
    self.BtnUlti:SetActive(not is_max)
    self.AttrMax:SetActive(is_max)
end
function UltiWorkPanel:flushStrongAttr()
    local ring_info = self.data:UntilFixRingShow(self.equip)
    for i = 1, 2 do
        self.UltiAttrRing[i].gameObject:SetActive(ring_info.utli[i] ~= nil)
    end
    self.strong_num = 0
    for k, v in pairs(ring_info.utli) do
        local is_eff_show = self.operate_set == 2 and self.pausing == true
        v.is_eff_show = is_eff_show
        self.UltiAttrRing[k]:SetData(v)
        self.strong_num = self.strong_num + 1
    end
end

function UltiWorkPanel:flushCenterPart()
    local attr_list = self.equip:UltimateAttrList()
    local sec_list = self.equip:AttrSection()
    for i = 1, 2 do
        local flag = attr_list[i].attr_value ~= 0
        self.AttrTypes[i].gameObject:SetActive(flag)
        self.AttrValues[i].gameObject:SetActive(flag)
        if flag then
            local with_stren = false
            for j, k in pairs(sec_list) do
                if k.attr_type == attr_list[i].attr_type then
                    with_stren = k.with_stren == true
                end
            end
            local per = DataHelper.IsPercent(attr_list[i].attr_type)
            UH.SetText(self.AttrTypes[i], Language.Common.AttrList[attr_list[i].attr_type] .. "：")
            if with_stren then
                UH.SetText(
                    self.AttrValues[i],
                    string.format(
                        Language.UltiWork.Value,
                        per and (attr_list[i].attr_value / 10) .. "%" or attr_list[i].attr_value,
                        per and (attr_list[i].ultimate_value / 10) .. "%" or attr_list[i].ultimate_value
                    )
                )
            else
                UH.SetText(self.AttrValues[i], per and (attr_list[i].attr_value / 10) .. "%" or attr_list[i].attr_value)
            end
        end
    end
end

function UltiWorkPanel:flushCostItem()
    local config = self.data:GetUltiConsume(self.equip)
    local CostItem = Item.Create(config.added_consume_item[0])
    local need_num = config.added_consume_item[0].num
    self.CostCell:SetDataByItemIdWithNeed(CostItem.item_id, need_num)
    local list = config.common_consume_item
    local temp
    for i = 1, 3 do
        temp = list[i - 1]
        if temp then
            self.ItemList[i].gameObject:SetActive(true)
            self.ItemList[i]:SetDataByItemIdWithNeed(temp.item_id, temp.num)
        else
            self.ItemList[i].gameObject:SetActive(false)
        end
    end
end
function UltiWorkPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[103].desc})
end
function UltiWorkPanel:OnDestroy()
    self.jump_flag = false
    self.select_index = 1
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.handle_delay)
    self.data:SetSelectUltiData(nil)

    if self.wait_timer then
        TimeHelper:CancelTimer(self.wait_timer)
    end
end
UltiWorkItem = UltiWorkItem or DeclareMono("UltiWorkItem", UIWidgetBaseItem)
function UltiWorkItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.NameTex, data:NameId())
    --
    local attr_list = data:UltimateAttrList()
    local per1 = DataHelper.IsPercent(attr_list[1].attr_type)
    local type1 = Language.Common.AttrList[attr_list[1].attr_type]
    local per2 = DataHelper.IsPercent(attr_list[2].attr_type)
    local type2 = Language.Common.AttrList[attr_list[2].attr_type]

    local attr
    if attr_list[1].attr_value ~= 0 and attr_list[2].attr_value ~= 0 then
        attr =
            string.format(
            Language.UltiWork.AttrValue,
            type1,
            per1 and (attr_list[1].attr_value / 10) .. "%" or attr_list[1].attr_value,
            type2..":",
            per2 and (attr_list[2].attr_value / 10) .. "%" or attr_list[2].attr_value
        )
    elseif attr_list[1].attr_value ~= 0 then
        attr =
            string.format(
            Language.UltiWork.AttrValue,
            type1,
            per1 and (attr_list[1].attr_value / 10) .. "%" or attr_list[1].attr_value,
            "",
            ""
        )
    elseif attr_list[2].attr_value ~= 0 then
        attr =
            string.format(
            Language.UltiWork.AttrValue,
            type2,
            per2 and (attr_list[2].attr_value / 10) .. "%" or attr_list[2].attr_value,
            "",
            ""
        )
    end
    UH.SetText(self.AttrTex, attr)
    self.InBagObj:SetActive(not data.in_bag)
end
function UltiWorkItem:Click()
    UIWidgetBaseItem.Click(self)
    self.ClickEvent:Trigger()
end

WorkshopUltiworkEffPos = WorkshopUltiworkEffPos or DeclareMono("WorkshopUltiworkEffPos", UIWidgetBaseItem)
function WorkshopUltiworkEffPos:SetData(data)
end

UltworkAttrRing = UltworkAttrRing or DeclareMono("UltworkAttrRing", UIWidgetBaseItem)
function UltworkAttrRing:OnDestroy()
    if self.time_func then
        TimeHelper:CancelTimer(self.time_func)
        self.time_func = nil
    end
end

function UltworkAttrRing:SetDevice(param_t)
    self.effect_tool = param_t.effect_tool
    self.effect_id = param_t.effect_id
end

function UltworkAttrRing:SetData(data)
    if data.is_eff_show then
        self:CacheData()
    end
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.attr_type, Language.Common.AttrList[data.attr_type])
    if data.is_eff_show then
        self:PlayRoll()
    else
        self:FlushAttr()
    end
end

function UltworkAttrRing:FlushAttr()
    if DataHelper.IsPercent(self.data.attr_type) then
        self.attr_pro:SetText(string.format("%s/%s", (self.data.cur / 10) .. "%", (self.data.max / 10) .. "%"))
    else
        self.attr_pro:SetText(string.format("%s/%s", self.data.cur, self.data.max))
    end
    self.attr_pro:SetProgress(self.data.cur / self.data.max)
end

function UltworkAttrRing:CacheData()
    self.old_data = {
        cur = self.data.cur,
        pro = self.attr_pro:GetProgress()
    }
end

function UltworkAttrRing:IsMax()
    return self.data.cur == self.data.max
end

function UltworkAttrRing:PlayRoll()
    local tframe = 20 -- 总动画时长，20帧

    local rangle_attr = self.data.cur - self.old_data.cur
    local rangle_pro = (self.data.cur / self.data.max) - self.old_data.pro

    if rangle_attr == 0 or rangle_pro == 0 then
        self:FlushAttr()
        return
    end

    local per = DataHelper.IsPercent(self.data.attr_type)
    local str = per and "%s%%/%s%%" or "%s/%s"
    self.attr_pro:SetText(
        string.format(str, per and self.data.cur / 10 or self.data.cur, per and self.data.max / 10 or self.data.max)
    )

    self.effect_tool:Play(self.effect_id, self.roll_pos)

    local attr_plus = self.old_data.pro
    local pro_plus = self.old_data.pro * -1 * 360

    local attr_per = rangle_pro / tframe
    local pro_per = -1 * (rangle_pro / tframe) * 360

    if self.time_func then
        TimeHelper:CancelTimer(self.time_func)
    end
    self.time_func =
        TimeHelper:AddRunFrameTimer(
        function()
            self.roll_rect.localRotation = Quaternion.Euler(0, 0, pro_plus + pro_per)
            self.attr_pro:SetProgress(attr_plus + attr_per)

            pro_plus = pro_plus + pro_per
            attr_plus = attr_plus + attr_per
        end,
        1,
        tframe
    )
    return true
end
