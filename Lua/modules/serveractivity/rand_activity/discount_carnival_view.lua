DiscountCarnivalView =
    DiscountCarnivalView or DeclareView("DiscountCarnivalView", "discount_carnival/discount_carnival", Mod.Splendid.DiscountCarnival)

    VIEW_DECLARE_LEVEL(DiscountCarnivalView, ViewLevel.L1)
function DiscountCarnivalView:DiscountCarnivalView()
    -- body
end
--info = 0
--buy_item = 1
function DiscountCarnivalView:LoadCallback()
    self.Currency:DefaultSet()
    self.Panels[ServerActivityData.Instance:GetDiscountCarnivalShow()]:SetActive(true)
end
function DiscountCarnivalView:OnClickClose()
    ViewMgr:CloseView(DiscountCarnivalView)
end
DiscountCarnivalPanel = DiscountCarnivalPanel or DeclareMono("DiscountCarnivalPanel", UIWFlushPanel)
function DiscountCarnivalPanel:DiscountCarnivalPanel()
    self.data = ServerActivityData.Instance
    self.cur_num = 1
    self.data_cares = {
        {data = ServerActivityData.Instance:GetDCInfo(), func = self.onFlush, init_call = false}
    }
    local info = ActivityRandData.Instance:GetConfigByKey(Mod.Splendid.DiscountCarnival)
    self.Board:SetData(DiscountCarnivalView, info.text)
    self:onFlush()
end
function DiscountCarnivalPanel:onFlush()
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_DISCOUNT_CARNIVAL, TimeType.Type_Special_4)
    --获取配置
    local data = self.data:GetDCReward()
    local temp = {}
    local max_pag = 0
    for index, value in ipairs(data) do
        if value.paging == self.cur_num then
            local can_num = value.buy_times > 0 and value.buy_times or value.buy_seccond_day
            local flag, num = ServerActivityData.Instance:IsCanBuy(value.seq1, can_num)
            if flag then
                value.flag = 1
            else
                value.flag = 0
            end
            table.insert(temp, value)
        end
        max_pag = math.max(value.paging, max_pag)
    end
    self.LeftArrow:SetActive(max_pag ~= 1)
    self.RightArrow:SetActive(max_pag ~= 1)
    --再分一次页
    temp = self:Sort(temp)
    self.List:SetData(temp)
    UH.SetText(self.Content, Config.language_cfg_auto.textdesc[51].desc)
end

--领取过得放后面
function DiscountCarnivalPanel:Sort(list)
    local sort_func = function (a,b)
        if a.flag == b.flag  then
            if a.price_type == b.price_type then
                -- if a.original_price == b.original_price then
                    if a.seq and b.seq then
                        return a.seq < b.seq
                    end
                -- end
                -- return a.original_price > b.original_price
            else
                return a.price_type < b.price_type
            end
        else
            return a.flag > b.flag
        end
    end
    table.sort(list,sort_func)
    return list
end

function DiscountCarnivalPanel:OnClickChange(num)
    self.cur_num = self.cur_num + num
    if self.cur_num == 0 then
        self.cur_num = 1
    end
    if self.cur_num > 2 then
        self.cur_num = 2
    end
    self:onFlush()
end
DiscountCarnivalItem = DiscountCarnivalItem or DeclareMono("DiscountCarnivalItem", UIWidgetBaseItem)
function DiscountCarnivalItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetDataByItemId(data.reward_item.item_id)
    UH.SetText(self.Name,Item.GetName(data.reward_item.item_id))
    local red_point_num = 0
    local flag,num
    if data.buy_tepy == 1 then
        flag, num = ServerActivityData.Instance:IsCanBuy(data.seq1, data.buy_times)
        num = data.buy_times - num
        if num > 0 then
            red_point_num = 1
        end
        UH.SetText(self.Num, string.format(Language.DiscountCarnival.BuyTimes, num))
    else
        flag, num = ServerActivityData.Instance:IsCanBuy(data.seq1, data.buy_seccond_day)
        num = data.buy_seccond_day - num
        if num > 0 then
            red_point_num = 1
        end
        UH.SetText(self.Num, string.format(Language.DiscountCarnival.LimitTimes, num))
    end
    self.BtnBuyInter.Interactable = flag 
    self.BtnFreeInter.Interactable = flag 
    self.BtnFreeObj:SetActive(data.original_price == 0)
    if data.original_price == 0 then
        if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(112, 152))
        end
        UH.SetText(self.Cost, Language.OpenAct.LimitShop.Free)
        -- self.Cost.fontSize = 24
    else
        UH.SetText(self.Cost, data.original_price)
        -- self.Cost.fontSize = 26
    end
    if self.red_point then
        self.red_point:SetNum(red_point_num)
    end
    UH.SpriteName(self.Icon, HuoBi[data.price_type])
end
function DiscountCarnivalItem:OnClickBuy()
    local flag
    if self.data.buy_tepy == 1 then
        flag = ServerActivityData.Instance:IsCanBuy(self.data.seq1,self.data.buy_times)
    else
        flag = ServerActivityData.Instance:IsCanBuy(self.data.seq1,self.data.buy_seccond_day)
    end
    if flag then
        if self.data.original_price == 0 then
            local param_t = {}
            param_t.rand_activity_type = ACTIVITY_TYPE.RAND_DISCOUNT_CARNIVAL
            param_t.opera_type = 1
            param_t.param_1 = self.data.seq1 --seq
            param_t.param_2 = 0 --折扣id
            ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
            return
        end
        ServerActivityData.Instance:SetDCWillBuy(self.data)
        ViewMgr:OpenView(DiscountCarnivalSelect)
    else
        PublicPopupCtrl.Instance:Center(ErrorText[41])
    end
end

function DiscountCarnivalItem:OnDestroy()
    if self.red_point ~= nil then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
    UIWidgetBaseItem.OnDestroy(self)
end

DiscountCarnivalSelect =
    DiscountCarnivalSelect or DeclareView("DiscountCarnivalSelect", "discount_carnival/discount_carnival_select")
function DiscountCarnivalSelect:DiscountCarnivalSelect()
    -- body
end
function DiscountCarnivalSelect:LoadCallback()
    self.Board:SetData(self:GetType(), Language.DiscountCarnival.Title3, Vector2.New(945, 584))
    self.handle_care = ServerActivityData.Instance.discount_carnival_data:Care(BindTool.Bind(self.OnFlush, self))
end
function DiscountCarnivalSelect:OnFlush()
    local data = ServerActivityData.Instance:GetDCWillBuy()
    if data == nil then
        ViewMgr:CloseView(DiscountCarnivalSelect)
        return
    end
    UH.SpriteName(self.IconSp, HuoBi[data.price_type])
    self.consume_type = data.price_type
    local list = ServerActivityData.Instance:GetDCCard()
    self.List:SetData(list)
    local count = 0
    for index, value in ipairs(list) do
        count = count + Item.Create({item_id = value.coupon_id}):Num()
    end
    UH.SetText(self.CoutTex, string.format(Language.DiscountCarnival.CanUseItem, count))
    local use = ServerActivityData.Instance:GetDCUse()
    if use == nil then
        UH.SetText(self.UseDesc, Language.DiscountCarnival.NoneUse)
        UH.SetText(self.Price, data.original_price)
        self.consume_gold = data.original_price
    else
        local item = Item.Create({item_id = use.coupon_id})
        UH.SetText(self.UseDesc, string.format(Language.DiscountCarnival.UseItem, item:Name()))
        local price = data.original_price * use.discount / 10
        UH.SetText(self.Price, string.format(Language.DiscountCarnival.Price, price, data.original_price - price))
        self.consume_gold = price
    end
end
function DiscountCarnivalSelect:CloseCallback()
    ServerActivityData.Instance.discount_carnival_data:Uncare(self.handle_care)
    ServerActivityData.Instance:SetDCWillBuy(nil)
    ServerActivityData.Instance:SetDCUse(nil, true)
end
function DiscountCarnivalSelect:OnClickBuy()
    if self.consume_type == CurrencyType.CoinBind then
        if not MallCtrl.IsNotCurrency(CurrencyType.CoinBind,self.consume_gold) then
            return
        end
    else
        if not MallCtrl.IsNotCurrency(CurrencyType.Gold,self.consume_gold) then
            return
        end
    end
    ServerActivityData.Instance.discount_carnival_data:Uncare(self.handle_care)
    local data = ServerActivityData.Instance:GetDCWillBuy()
    local use = ServerActivityData.Instance:GetDCUse()
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_DISCOUNT_CARNIVAL
    param_t.opera_type = 1
    param_t.param_1 = data.seq1 --seq
    param_t.param_2 = use == nil and 0 or use.coupon_id --折扣id
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
    ViewMgr:CloseView(DiscountCarnivalSelect)
    ServerActivityData.Instance:SetDCWillBuy(nil)
    ServerActivityData.Instance:SetDCUse(nil, true)
end

DiscountCarnivalSelectItem = DiscountCarnivalSelectItem or DeclareMono("DiscountCarnivalSelectItem", UIWidgetBaseItem)
function DiscountCarnivalSelectItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.item = Item.Create({item_id = data.coupon_id})
    UH.SetIcon(self.Icon, self.item:IconId())
    UH.SetText(self.Name, self.item:Name())
    UH.SetText(self.Num, string.format(Language.DiscountCarnival.ItemNum, self.item:Num()))
    UH.SetText(self.TimeTex, Language.DiscountCarnival.TimeTip2)
end
function DiscountCarnivalSelectItem:OnClickSelect(isOn)
    ServerActivityData.Instance:SetDCUse(self.data, isOn)
    if self.item:Num() <= 0 and isOn then
        PublicPopupCtrl.Instance:Center(ErrorText[134])
    end
end
