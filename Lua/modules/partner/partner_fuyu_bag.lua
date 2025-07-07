PartnerFuYuBag = PartnerFuYuBag or DeclareView("PartnerFuYuBag", PartnerConfig.ResPath.."partner_fuyu_bag")

function PartnerFuYuBag:PartnerFuYuBag()
    self.Board:SetData(self:GetType(), Language.FuYu.Bag)
end

PartnerFuYuBagPanel = PartnerFuYuBagPanel or DeclareMono("PartnerFuYuBagPanel", UIWFlushPanel)
function PartnerFuYuBagPanel:PartnerFuYuBagPanel()
    self.data = PartnerData.Instance
    self.data_cares = {
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushFuYu, init_call = false},
        {
            data = self.data.fuyu_flush,
            func = self.FlushFuYu,
            init_call = false
        },
        {data = self.data.fuyu_change,func = self.ClickChange, init_call = false}
    }
end
function PartnerFuYuBagPanel:ClickChange()
    self:OnClickPutOn()
end

function PartnerFuYuBagPanel:onFlush()
    --获取显示伙伴
    local partner = self.data:GetCurSelect()
    self.show_list = self.data:FlyUpPartnerList(partner)
    -- self:FlushList(partner)
    self.show_index = 1
    self.total_index = table.nums(self.show_list)
    self:FlushFuYu()
end
function PartnerFuYuBagPanel:FlushList(partner)
    if partner == nil then
        return
    end
    
    self.List:SetData(partner:FuYuListDataAndEmpty())
    self.List:SetCompleteCallBack(function ()
        self.List:ClickItem(1)    
    end)
    UH.SetText(self.PartnerName, Format(Language.FuYu.Name, partner:Level(), partner:Name()))

    local list = self.data:GetFuYuBagList()
    local num = table.nums(list)
    if num < 30 then
        num = num + 1
        for i = num, 30 do
            table.insert(list,{item_id = 0})
        end
    else
        local adds = num % 5
        if adds > 0 then
            for i = 1, 5 - adds do
                table.insert(list, {item_id = 0})
            end
        end
    end
    self.BagList:SetData(list)
end
function PartnerFuYuBagPanel:FlushFuYu()
    self:FlushList(self.show_list[self.show_index])
end
function PartnerFuYuBagPanel:OnClickArrowLast()
    --获取已飞升伙伴列表进行切换
    if self.total_index == 1 then
        PublicPopupCtrl.Instance:Center(Language.FuYu.ArrowTip)
        return
    end
    self.show_index = ((self.show_index - 1) <= 0 and self.total_index or (self.show_index - 1))
    self:FlushFuYu()
end
function PartnerFuYuBagPanel:OnClickArrowNext()
    if self.total_index == 1 then
        PublicPopupCtrl.Instance:Center(Language.FuYu.ArrowTip)
        return
    end
    self.show_index = (self.show_index + 1 > self.total_index and 1 or self.show_index + 1)
    self:FlushFuYu()
end
function PartnerFuYuBagPanel:OnBagItemClick(click_data)
    if click_data.is_select == true then
        self.bag_item = click_data.data
        if self.bag_item.item_id > 0 then
            CellClicks.BagGrid(click_data.cell)
        end
    else
        self.bag_item = nil
    end
    self:FlushBtnDesc()
end
function PartnerFuYuBagPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetFuYuClickBodyItem(nil)
end
function PartnerFuYuBagPanel:OnClickBodyItem(data)
    self.body_item = data
    self:FlushBtnDesc()
    self.data:SetFuYuClickBodyItem(data)
end
function PartnerFuYuBagPanel:OnClickPutOn()
    local partner = self.show_list[self.show_index]
    if partner then
        if self.bag_item and self.bag_item.item_id > 0 then
            PartnerCtrl.Instance:ReqFuYuPutOn(partner:Id(), self.body_item.wear_idx, self.bag_item:GridColumn(), self.bag_item:GridIndex())
        else
            PublicPopupCtrl.Instance:Center(Language.FuYu.NoSelectTip)
        end
    end
end
function PartnerFuYuBagPanel:FlushBtnDesc()
    if self.body_item and self.body_item.is_empty == true then
        UH.SetText(self.BtnDesc, Language.FuYu.PutOn)
    elseif self.body_item and not self.body_item.is_empty then
        UH.SetText(self.BtnDesc, Language.FuYu.Change)
    else
        UH.SetText(self.BtnDesc, Language.FuYu.PutOn)
    end
end
PartnerFuYuBodyItem = PartnerFuYuBodyItem or DeclareMono("PartnerFuYuBodyItem",UIWidgetBaseItem)
function PartnerFuYuBodyItem:PartnerFuYuBodyItem()
    
end
function PartnerFuYuBodyItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    if data.is_empty == true then
        self.None:SetActive(true)
        self.Show:SetActive(false)
        -- self.EffectBlock:SetActive(false)
        UH.SetText(self.Desc1, "")
        UH.SetText(self.Desc2, "")
        self.EffectBlock1:SetActive(false)
        self.EffectBlock2:SetActive(false)
    else
        local item = Item.Create(data)
        self.None:SetActive(false)
        self.Show:SetActive(true)
        UH.SetText(self.Name, item:QuaName(true))
        if item.param.sp_id > 0 then
            -- UH.SetText(self.Desc, Format("%s：%s", item:SpName(), item:FuYuEffectDesc()))
            UH.SetText(self.Desc1, Format("%s：%s", item:SpName(), item:FuYuEffectDesc()))
            if item:ExtraId() > 0 then
                UH.SetText(self.Desc2, Format("%s：%s", item:ExtraName(), item:FuYuEffectDesc(item:ExtraId())))
            end
        else
            -- UH.SetText(self.Desc, item:FuYuEffectDesc())
            UH.SetText(self.Desc1, item:FuYuEffectDesc())
            UH.SetText(self.Desc2, "")
        end
        local str = ""
        local attr_list = item:BaseAttr()
        local index = 1
        for i, v in ipairs(attr_list) do
            if v.attr_value > 0 then
                if index == 3 then
                    str = str .."\n".. Format("%s   %s  ", Language.AttrItems.AttrNameSample[v.attr_type], v.attr_value)
                else
                    str = str .. Format("%s   %s  ", Language.AttrItems.AttrNameSample[v.attr_type], v.attr_value)
                end
                index = index + 1
            end
        end
        self.Cell:SetData(item)
        UH.SetText(self.Attrs, str)
        local partner_id = item.partner_id
        local wear_idx = item.wear_idx
        local sp_id = item.param.sp_id
        if sp_id > 0 and partner_id ~= -1 and wear_idx ~= -1 then
            local partner = PartnerData.Instance:GetPartnerInfoById(partner_id)
            local config = Cfg.ParnterFuYuEffect(sp_id)
            if config and config.limit_type == 0 then
                self.EffectBlock1:SetActive(false)
            elseif partner and config and config.limit_type == 1 then
                self.EffectBlock1:SetActive(config.param1 ~= (partner.info.job * 100))
            elseif partner and config and config.limit_type == 2 then
                self.EffectBlock1:SetActive(config.param1 ~= partner_id)
            else
                self.EffectBlock1:SetActive(true)
            end

            if not self.EffectBlock1.activeSelf then
                local is_ok = item:IsSpOk()
                self.EffectBlock1:SetActive(not is_ok)
            end
        else
            self.EffectBlock1:SetActive(false)
        end
        local extra_id = item:ExtraId()
        if extra_id > 0 and partner_id ~= -1 and wear_idx ~= -1 then
            local partner = PartnerData.Instance:GetPartnerInfoById(partner_id)
            local config = Cfg.ParnterFuYuEffect(extra_id)
            if config and config.limit_type == 0 then
                self.EffectBlock2:SetActive(false)
            elseif partner and config and config.limit_type == 1 then
                self.EffectBlock2:SetActive(config.param1 ~= (partner.info.job * 100))
            elseif partner and config and config.limit_type == 2 then
                self.EffectBlock2:SetActive(config.param1 ~= partner_id)
            else
                self.EffectBlock2:SetActive(true)
            end

            if not self.EffectBlock2.activeSelf then
                local is_ok = item:IsSpOk2()
                self.EffectBlock2:SetActive(not is_ok)
            end
        else
            self.EffectBlock2:SetActive(false)
        end
    end
end
function PartnerFuYuBodyItem:Click()
    self.ItemClick:Trigger()
end
function PartnerFuYuBodyItem:OnClickStr()
    PartnerData.Instance:OpenFuYuQiangHua(Item.Create(self.data))
end
function PartnerFuYuBodyItem:OnClickTaskOff()
    PartnerCtrl.Instance:ReqFuYuTakeOff(self.data.partner_id, self.data.wear_idx)

end

PartnerFuYuBagItem = PartnerFuYuBagItem or DeclareMono("PartnerFuYuBagItem", UIWidgetBaseItem)
function PartnerFuYuBagItem:PartnerFuYuBagItem()
    self.click_data = {is_select = not self.Selector.isOn, data = self.data}
end
function PartnerFuYuBagItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
end
function PartnerFuYuBagItem:GetData()
    self.click_data.is_select = not self.Selector.isOn
    self.click_data.data = self.data
    self.click_data.cell = self.Cell
    return self.click_data
end
