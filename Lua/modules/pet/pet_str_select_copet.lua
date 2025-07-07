PetStrSelectCopet = PetStrSelectCopet or DeclareView("PetStrSelectCopet","pet/pet_str_select_copet")
VIEW_DECLARE_MASK(PetStrSelectCopet, ViewMask.BgBlock)
function PetStrSelectCopet:PetStrSelectCopet()
end

function PetStrSelectCopet:LoadCallback(param)
    self.Board:SetData(PetStrSelectCopet,param.pet_title,Vector2.New(850, 520))
    -- local pet = PetData.Instance:GetCurSelect()
    -- if pet and not PetData.Instance.show_strengthen_remind[pet.index] then
    --     PetData.Instance.show_strengthen_remind[pet.index] = true
    --     PetData.Instance.pet_list:SetDirty()
    -- end
end

function PetStrSelectCopet:CloseCallback()
    PetData.Instance.sel_pet_str_data.click_index = -1
end
----------------------------------------------------
PetStrSelectCopetPanel = PetStrSelectCopetPanel or DeclareMono("PetStrSelectCopetPanel", UIWFlushPanel)
function PetStrSelectCopetPanel:PetStrSelectCopetPanel()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance

    RichTextHelper.Bind(self.TipRichText, BindTool.Bind(self.OnClickTipRich, self))
    self.data_cares = {
        {data = self.pet_data.sel_pet_str_data, func = self.FlushShowInfo},
        {data = self.pet_data.pet_list, func = self.FlushShowInfo, init_call = false},
        {data = TradeData.Instance.big_search_data, func = self.FlushShopList, keys = {"List"}, init_call = false}
    }
    self.flag = 1
    self.order_type = TRADE_PET_INDEX_ORDER.STR_LEVEL_ORDER_DOWN

    GuideManager.Instance:RegisterGetGuideUi("PetStrengthenSelectItem", function ()
        for i,v in pairs(self.ChooseList.item_list) do
            return v
        end
        GuideManager.Instance:ForceStop()
    end)
    GuideManager.Instance:RegisterGetGuideUi("PetStrengthenSelectOk", function ()
        return self.SelBtn,function ()
            self:OnClickConfirmSelect()
        end
    end)
end

function PetStrSelectCopetPanel:FlushShowInfo()
    self.pet_select_index = -1
    if self.flag == 1 then
        self:FlushChooseList()
    else
        self:InitShopList()
    end
    local get_way_str = ">>"..RichTextHelper.LinkStr(Language.Pet.PetStrengthGetWay, "_ul",COLORSTR.Yellow2).."<<"
    UH.SetText(self.TipRichText, get_way_str)
end

function PetStrSelectCopetPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.shop_pet_care then
        self.pet_data:ShopPetInfo():Uncare(self.shop_pet_care)
    end
    if self.send_delay_timer then
        TimeHelper:CancelTimer(self.send_delay_timer)
        self.send_delay_timer = nil
    end
    self.pet_data:ClearShopPetInfo()
end

--选择副宠列表
function PetStrSelectCopetPanel:FlushChooseList()
    local data = self.pet_data:GetExcepetSelect()
    self.ChooseList:SetData(data)
    if self.shop_select_item == nil then
        self.ChooseList:ClickItem(1)
    end
    self.LblTip:SetActive(next(data) == nil)
end

--点击富文本回调
function PetStrSelectCopetPanel:OnClickTipRich()
    local only_quality, _, quality = self.pet_data:CopetSelectOnlyQuality()
    if only_quality == nil then
        return
    end
    if only_quality == false then
        if self.pet_data:GetCurSelect() then
            MainOtherCtrl.Instance:GetWayViewById(self.pet_data:GetCurSelect().pet_id)
        end
    else
        local special_get_way_data = PetData.GetPetSpecialGetWay(quality)
        
        if special_get_way_data ~= nil then
            ViewMgr:OpenView(PetSpecialGetWayView,{quality = quality, icon_id = special_get_way_data.icon_id, catch_way_str = special_get_way_data.get_way})
        end
    end
end

function PetStrSelectCopetPanel:InitShopList()
    self:InitShop()
    local data = self.pet_data:ShopPetInfo()
    if self.shop_pet_care then
        data:Uncare(self.shop_pet_care)
    end
    self.shop_pet_care = data:Care(BindTool.Bind(self.FlushShopList, self))
end

function PetStrSelectCopetPanel:OnClickToggle(flag)
    for i = 1, 2 do
        self.ListObjs[i]:SetActive(flag == i)
    end
    self.LblTip:SetActive(false)
    if flag == 1 then
        self:FlushChooseList()
    else
        self:InitShopList()
    end
    self.flag = flag
end
function PetStrSelectCopetPanel:InitShop()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end
    self.pet_data:ClearShopPetInfo()
    local pet_trade = self.pet_data:GetTrade(pet:ID())
    if pet_trade == 0 then -- 0为不可交易
        self.LblTip:SetActive(true)
        return
    end
    local only_quality, _, quality = self.pet_data:CopetSelectOnlyQuality()
    if only_quality then
        PetCtrl.SearchPetForQuickBuyOrder(self.order_type, 0, quality)
    else
        PetCtrl.SearchPetForQuickBuyOrder(self.order_type, pet:ID(), 0)
    end
end

function PetStrSelectCopetPanel:FlushShopList()
    local data = self.pet_data:ShopPetInfo()
    self.ShopList:SetData(data.item_list)
    self.ShopList:ClickItem(1)
    self.LblTip:SetActive(data.item_count == 0)
    self.BtnBuy:SetActive(data.item_count ~= 0)
end

function PetStrSelectCopetPanel:OnClickConfirmSelect()
    if self.pet_select_index == -1 then
        PublicPopupCtrl.Instance:Center(ErrorText[3])
        return
    end
    local pet = self.pet_data:GetCurSelect()
    if pet:IsLianYao() and self.pet_data.sel_pet_str_data.is_yao then
        pet = pet:YaoPet()
    end
    local show = self.pet_data:GetIsShowStrAlert()
    local pet_sel = self.pet_data:GetPetInfo(self.pet_select_index)
    
    if pet_sel ~= nil then
        self.pet_data.str_copet_level = pet_sel:StrLevel()
        if pet_sel.info.pet_equipment_id ~= 0 then
            PublicPopupCtrl.Instance:Center(Language.Pet.PetStrengthStuffEquip)
            return
        end
        --是否已锁定
        if pet_sel.info.lock == 1 then
            local info = {
                 content = Language.Pet.PetStrengthStuffLock,
                 confirm = {
                        func = function ()
                            PetCtrl.SendPetOp(PET_OP_TYPE.Lock, self.pet_select_index)
                            ViewMgr:CloseView(DialogTipsView)
                        end
                    },
                }
            PublicPopupCtrl.Instance:DialogTips(info)
            return
        end

        -- local select_mutant = pet_sel and pet_sel.config and pet_sel.config.mutant_times >= 1
        -- if show and select_mutant then
        --     ViewMgr:OpenView(PetStrAlert)
        --     self.pet_data:SetPetStrSelect(self.pet_select_index)
        --     return
        -- end
        --是否超过了要求的强化等级
        local strengthen_cfg = self.pet_data:GetSpecialStrengthen(pet) or Cfg.PetStrengthenConsume(pet:Quality(), pet:StrLevel())
        if pet_sel:IsLianYao() or pet_sel.str_level > strengthen_cfg["strengthen_level_"..self.pet_data.sel_pet_str_data.click_index] then
            local info = {
                content = pet_sel:IsLianYao() and Language.Pet.PetStrengthLianYao or Language.Pet.PetStrengthStuffOver,
                confirm = {
                        func = function ()
                            self.pet_data:SetSelPetStrData(pet.index, pet_sel)
                            ViewMgr:CloseView(PetStrSelectCopet)
                            ViewMgr:CloseView(DialogTipsView)
                        end,
                        wait_time = 5
                   },
               }
           PublicPopupCtrl.Instance:DialogTips(info)
            return
        end
        self.pet_data:SetSelPetStrData(pet.index, pet_sel)
        ViewMgr:CloseView(PetStrSelectCopet)
    end
end

function PetStrSelectCopetPanel:OnClickStrItem(data)
    self.pet_select_index = data.index
    --LogError("OnClickStrItem",data.index)
end

function PetStrSelectCopetPanel:OnClickShopStrItem(data)
    self.shop_select_item = data
    UH.SetText(self.BuyPrice, data.price)
    self.BtnBuy:SetActive(true)
end

function PetStrSelectCopetPanel:OnClickChoose(order_type)
    self.order_type = order_type
    self.OrderPanel:SetActive(false)
    UH.SetText(self.OrderTex, Language.Pet.OrderType[order_type])
    local pet = self.pet_data:GetCurSelect()
    if not pet then
        return
    end
    self.pet_data:ClearShopPetInfo()
    local only_quality, _, quality = self.pet_data:CopetSelectOnlyQuality()
    if only_quality then
        PetCtrl.SearchPetForQuickBuyOrder(self.order_type, 0, quality)
    else
        PetCtrl.SearchPetForQuickBuyOrder(self.order_type, pet:ID(), 0)
    end
end

function PetStrSelectCopetPanel:OnClickOpenChoose()
    self.OrderPanel:SetActive(not self.OrderPanel.activeSelf)
end

function PetStrSelectCopetPanel:OnClickBuy()
    local param_t = {
        content = string.format(
            Language.Shop.BuyItemTip,
            self.shop_select_item.price,
            Language.Common.JiaoYi[self.shop_select_item.price_type],
            self.shop_select_item.item_data.param.name
        ),
        confirm = {
            func = function()
                if MallCtrl.IsNotCurrency(CurrencyType.Gold, self.shop_select_item.price) then
                    TradeCtrl.Instance:SendCommonBuyReq(self.shop_select_item, 1)
                    self:InitShop()
                    ViewMgr:CloseView(DialogTipsView)
                end
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end

function PetStrSelectCopetPanel:OnShopListDrag()
    --请求最后一组
    --获取最后一组
    if self.ShopSR.verticalNormalizedPosition < -0.3 then
        local info = self.pet_data:ShopPetInfo()
        local last_pet = info.item_list[info.item_count]
        local pet = self.pet_data:GetCurSelect()
        local param_t = {}
        param_t.order_type = self.order_type
        param_t.last_seller_uid = last_pet.seller_uid
        param_t.last_price = last_pet.price
        param_t.last_put_on_shelve_time = last_pet.upshelve_time
        param_t.last_str_level = last_pet.item_data.param.str_level
        local only_quality, _, quality = self.pet_data:CopetSelectOnlyQuality()
        if only_quality then
            param_t.quality = quality
            param_t.item_id = 0
        else
            param_t.quality = 0
            param_t.item_id = pet:ID()
        end
        
        PetCtrl.SendTradeMarketSearchPetForQuickBuy(param_t)
    end
end

function PetStrSelectCopetPanel:OnClickCheckConfig()
    for k,v in ipairs(Config.skill_description_auto.pet_skill_description) do
        if v.skill_level ~= 1 then
            local test_rates1 = string.split(tostring(v.skill_value), "|")
            local test_rates2 = string.split(tostring(Config.skill_description_auto.pet_skill_description[k-1].skill_value), "|")
            local color_str = COLORSTR.Red5
            if #test_rates1 ~= #test_rates2 then
                LogError("skill_id",v.skill_id,"skill_name",v.skill_name,"skill_level",v.skill_level,"前一级和本级描述数据不匹配 k",k)
            end
            local str_t = {}
            for i=1,#test_rates1 do
                table.insert(str_t, test_rates1[i])
                table.insert(str_t, "arrow")
                table.insert(str_t, "color")
                table.insert(str_t, test_rates2[i])
            end
            xpcall(function()
                string.format(v.description_2, table.unpack(str_t))
            end,
            function (err)
                    LogError("skill_id",v.skill_id,"skill_level", v.skill_level)
                    LogError("hideAttrs ErrorLog:"..err)
            end)
        end
    end
end
-------------------------------------
PetStrItem = PetStrItem or DeclareMono("PetStrItem", UIWidgetBaseItem)
function PetStrItem:PetStrItem()
    self.pet_data = PetData.Instance
    self.lang = Language.Pet
end

function PetStrItem:SetData(data)
    local pet = data
    if self.IsShop then
        local info = data
        pet = Pet.New(data.item_data.param)
        UH.SetText(self.LblPrice, info.price)
    elseif self.IsStrength then
        if pet.level then
            UH.SetText(self.LblLevel, string.format(self.lang.LevelStr, pet.level))
        else
            UH.SetText(self.LblLevel, "")
        end
    end
    self.Cell:SetData(pet)
    self.pet = pet
    UIWidgetBaseItem.SetData(self, data)
    if pet == nil then
        return
    end
    local v1 = pet:TotalGrowth()
    local v2 = pet:ConfigGrowth()
    UH.SetText(self.LblName, pet.name)
    UH.SetText(self.LblValue, string.format(self.lang.GrowthStr8, COLORSTR.Yellow10, COLORSTR.Green11, v1, v2))
    if self.PetState then
        local state, is_follow = self.pet_data:GetPetState(pet.index)
        UH.SpriteName(self.PetState, PetData.State[state])
    end
    if self.LockObj then
        self.LockObj:SetActive(pet.info.lock == 1)
    end
    if self.PrayFlag then
        self.PrayFlag:SetActive(pet:PrayMainIndex() ~= nil)
    end
    
end
function PetStrItem:OnClickInfo()
    ViewMgr:OpenView(PetDetailView, self.pet)
end
function PetStrItem:Click()
    if self.ItemClick then
        self.ItemClick:Trigger()
    end
end