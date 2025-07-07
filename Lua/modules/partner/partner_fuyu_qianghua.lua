PartnerFuYuQiangHua = PartnerFuYuQiangHua or
                          DeclareView("PartnerFuYuQiangHua", PartnerConfig.ResPath .. "partner_fuyu_qianghua")

PartnerFuYuQiangHuaPanel = PartnerFuYuQiangHuaPanel or DeclareMono("PartnerFuYuQiangHuaPanel", UIWFlushPanel)
function PartnerFuYuQiangHuaPanel:PartnerFuYuQiangHuaPanel()
    self.data = PartnerData.Instance
    self.data_cares = {{
        data = BagData.Instance:GetColumnGrids(ItemColumnType.Item),
        func = self.onFlush,
        init_call = false
    }, {
        data = self.data.fuyu_flush,
        func = self.onFlush,
        init_call = false
    },{
        data = BagData.Instance:GetColumnGrids(ItemColumnType.Material),
        func = self.onFlush,
        init_call = false
    },}
    self.delay_flush = false
end
function PartnerFuYuQiangHuaPanel:FlushMaters()
    if self.item then
        self.List:ClickItem(self.item:FuYuLevel())
    end
    self.List:SetData(self.data:GetQiangHuaMeterals(self.item:FuYuLevel()))
end
function PartnerFuYuQiangHuaPanel:onFlush()
    self.Currency:SetCurrency(CurrencyType.ImmortalCoin)
    if self.delay_flush == true then
        self.handle_delay = TimeHelper:AddDelayTimer(function ()
            self:onFlush()
        end, 1)
        self.delay_flush = false
        return
    end
    TimeHelper:CancelTimer(self.handle_delay)
    local item = self.data:GetQiangHuaItem()
    if item == nil then
        LogDG("未选中强化符玉")
        return 
    end
    self.item = item
    self.Cell:SetData(item)
    UH.SetText(self.Name1, item:Name2())
    local is_max = self.item:FuYuLevel() >= Config.fuyu_auto.other[1].fuyu_max_level
    if is_max then
        self.Item2:SetActive(false)
        self.Item1:SetLocalPosition(Vector3.New(193,0,0))
        self.JianTou:SetActive(false)
    else
        self.Item1:SetLocalPosition(Vector3.zero)
        self.Item2:SetActive(true)
        self.JianTou:SetActive(true)
    end
    local cost_config = self.data:ProtectItemCfg(self.item:FuYuLevel())
    if cost_config then        
        self.LessMax:SetActive(true)
        local cost_item = Item.Create({
        item_id = cost_config.protect_item,
        })
        self.cost_item = cost_item
        self.CostCell:SetData(cost_item)
        local color = cost_item:Num() >= cost_config.num and COLORSTR.Green3 or COLORSTR.Red8
        UH.SetText(self.CostDesc, Format(Language.FuYu.UseProtect, cost_item:ColorStr(), cost_item:Name(), color,
        cost_item:Num(), cost_config.num))
    else
        self.LessMax:SetActive(false)
    end
        self.List:SetCompleteCallBack(function ()
            self.List:JumpVerticalIndex(item:FuYuLevel(), 157)
        end)
    self.List:ClickItem(item:FuYuLevel())
    self.List:SetData(self.data:GetQiangHuaMeterals(item:FuYuLevel()))
    local attr_list = item:BaseAttr()
    for i = 1, self.AttrList1:Length() do
        if attr_list[i] and attr_list[i].attr_value > 0 then
            UH.SetText(self.AttrList1[i], Format("%s +%s", Language.AttrItems.AttrNameSample[attr_list[i].attr_type],
                attr_list[i].attr_value))
        else
            UH.SetText(self.AttrList1[i], "")
        end
    end
    if not is_max then
        local next_item_data, next_config = self.data:GetNextLevelItem()
        local next_item = Item.Create(next_item_data)
        UH.SetText(self.Name2, next_item:Name2(item:NextSpId()))
        local attr_num = next_config.attri_num
        for i = 1, self.AttrList2:Length() do
            if i <= attr_num then
                UH.SetText(self.AttrList2[i],
                    Format(Language.FuYu.AttrDesc, Language.AttrItems.AttrNameSample[next_config["attri_tpye" .. i]],
                        next_config["value_" .. i]))

                if attr_list[i].attr_value > 0 then
                    UH.SetText(self.AddList1[i], Format("（+%s）", next_config["value_" .. i] - attr_list[i].attr_value))
                else
                    UH.SetText(self.AddList1[i], "")
                end
            else
                UH.SetText(self.AttrList2[i], "")
                UH.SetText(self.AddList1[i], "")
            end
        end    
    end
    self.Time1:SetTime("")
    self.Time2:SetTime("")
    if item.param.sp_id > 0 then
        -- 显示当前描述 
        -- 显示下一阶的描述
        if item.param.expired_timesstamp > 0 then
            self.Time1:StampTime(item.param.expired_timesstamp, TimeType.Type_Time_0, Language.FuYu.TimeTip2)
            self.Time2:StampTime(item.param.expired_timesstamp, TimeType.Type_Time_0, Language.FuYu.TimeTip2)
        end
        UH.SetText(self.EffectDesc1, item:FuYuEffectDesc())
        if not is_max then
            UH.SetText(self.EffectDesc2, item:FuYuEffectDesc(item:NextSpId()))
        end
    else
        UH.SetText(self.EffectDesc1, Language.FuYu.ZanWuEffect)
        UH.SetText(self.EffectDesc2, Language.FuYu.SuiJiEffect)
    end
end
function PartnerFuYuQiangHuaPanel:OnClickToggle()
    self:FlushRateDesc()
end
function PartnerFuYuQiangHuaPanel:OnClickQiangHua()
    if self.item then
        if self.item:FuYuLevel() >= Config.fuyu_auto.other[1].fuyu_max_level then
            PublicPopupCtrl.Instance:Center(Language.FuYu.MaxLevel)
            return
        end
    end
    if self.select_item then
        local cur_level = self.item:FuYuLevel()
        local next_level = cur_level
        local per = (self.select_item["level" .. next_level .. "_add_rate"]) / 100
        local item_id = self.select_item.materials_id
        if self.select_item.materials_level > cur_level then
            PublicPopupCtrl.Instance:Center(Language.FuYu.NoUseTip)
            return
        end
        local item_list = {}
        if Item.GetNum(item_id) <= 0 then
            table.insert(item_list, {item = Item.Create({item_id = item_id}), lack = 1})
            -- GetWayData.OnOpenPanel(Mod.Mall.Market, item_id, 1)
            -- return
        end
        if self.ItemToggle.isOn == true and self.cost_item then
            if self.cost_item:Num() <= 0 then
                table.insert(item_list, {item = self.cost_item, lack = 1})
                -- GetWayData.OnOpenPanel(Mod.Mall.Market, self.cost_item.item_id, 1)
                -- return
            end
        end
        if #item_list > 0 then
            local param = {}
            param.key = ItemInfoConfig.QuickBuyKey.FuyuQiangHua
            param.desc = Language.ItemInfo.QuickBuyDesc
            param.item_list = item_list
            param.auto_buy = true
            param.buy_func = function()
                self:QiangHuaTip(item_id, per)
            end
            ItemInfoCtrl.Instance:QuickBuyItems(param)
            return
        end

        self:QiangHuaTip(item_id, per)
        
        --[[ if self.item.wear_idx == -1 then
            PartnerCtrl.Instance:ReqFuYuUpgrade2(self.item:GridColumn(), self.item:GridIndex(), item_id,
                self.ItemToggle.isOn and 1 or 0)
        else
            PartnerCtrl.Instance:ReqFuYuUpgrade(self.item.partner_id, self.item.wear_idx, item_id,
                self.ItemToggle.isOn and 1 or 0)
        end ]]
    end
end
function PartnerFuYuQiangHuaPanel:QiangHuaTip(item_id, per)
    if self.ItemToggle.isOn == false then
        local info = {
                content = Format(Language.FuYu.SuccTip, per),
                confirm = {
                func = function()
                    self.delay_flush = true
                    if self.item.wear_idx == -1 then
                        PartnerCtrl.Instance:ReqFuYuUpgrade2(self.item:GridColumn(), self.item:GridIndex(), item_id,
                            self.ItemToggle.isOn and 1 or 0)
                    else
                        PartnerCtrl.Instance:ReqFuYuUpgrade(self.item.partner_id, self.item.wear_idx, item_id,
                            self.ItemToggle.isOn and 1 or 0)
                    end
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
                }
            }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    else
        local info = {
            content = Format(Language.FuYu.SuccTip2, per),
            confirm = {
            func = function()
                self.delay_flush = true
                if self.item.wear_idx == -1 then
                    PartnerCtrl.Instance:ReqFuYuUpgrade2(self.item:GridColumn(), self.item:GridIndex(), item_id,
                        self.ItemToggle.isOn and 1 or 0)
                else
                    PartnerCtrl.Instance:ReqFuYuUpgrade(self.item.partner_id, self.item.wear_idx, item_id,
                        self.ItemToggle.isOn and 1 or 0)
                end
                PublicPopupCtrl.Instance:CloseDialogTips()
            end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
    end
end

function PartnerFuYuQiangHuaPanel:OnClickClose()
    ViewMgr:CloseView(PartnerFuYuQiangHua)
end
function PartnerFuYuQiangHuaPanel:OnClickTip()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[338].desc)
end
function PartnerFuYuQiangHuaPanel:OnClickCaiLiaoItem(data)
    self.select_item = data
    
        self:FlushRateDesc()
end
function PartnerFuYuQiangHuaPanel:FlushRateDesc()
    if self.select_item == nil then return end
    if self.item == nil then return end
    local cur_level = self.item:FuYuLevel()
    if cur_level == Config.fuyu_auto.other[1].fuyu_max_level then
        return
    end
    local next_level = cur_level + 1
    if self.ItemToggle.isOn == true then
        UH.SetText(self.QiangDesc,
        Format(Language.FuYu.QiangHuaDesc2, next_level, self.select_item["level" .. cur_level .. "_add_rate"] / 100))
    else
        UH.SetText(self.QiangDesc,
        Format(Language.FuYu.QiangHuaDesc, next_level, self.select_item["level" .. cur_level .. "_add_rate"] / 100))
    end
end

FuYuCaiLiaoItem = FuYuCaiLiaoItem or DeclareMono("FuYuCaiLiaoItem", UIWidgetBaseItem)
function FuYuCaiLiaoItem:FuYuCaiLiaoItem()
    self.partner_data = PartnerData.Instance
end
function FuYuCaiLiaoItem:SetData(data)
    self.Inter.Interactable = true
    UIWidgetBaseItem.SetData(self, data)
    local will_item = self.partner_data:GetQiangHuaItem()
    local item = Item.Create({
        item_id = data.materials_id
    })
    local color = item:Num() > 0 and COLORSTR.Green3 or COLORSTR.Red8
    self.item = item
    UH.SetText(self.Name, item:QuaName())
    UH.SetText(self.Desc, item:Desc())
    UH.SetText(self.Num, Format(Language.FuYu.CaiLiaoNum, color, item:Num()))
    self.Cell:SetData(item)
    if will_item then
        self.Inter.Interactable = (data.materials_level <= will_item:FuYuLevel())
        self.Cell:MakeGrey(not (data.materials_level <= will_item:FuYuLevel()))
    else
        self.Cell:MakeGrey(false)
        self.Inter.Interactable = true
    end
end
function FuYuCaiLiaoItem:OnClickBuy()
    local way_cfg = self.item:GetWay()
    if way_cfg and way_cfg[1] then
        GetWayData.OnOpenPanel(tonumber(way_cfg[1]), self.data.materials_id, 1)
    end
end
function FuYuCaiLiaoItem:Click()
    self.ItemClick:Trigger()
end
