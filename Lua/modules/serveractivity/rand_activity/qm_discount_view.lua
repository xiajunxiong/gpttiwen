QmDiscountView = QmDiscountView or DeclareView("QmDiscountView", "serveractivity/qm_discount",Mod.Splendid.QmDiscount)

VIEW_DECLARE_LEVEL(QmDiscountView, ViewLevel.L1)

function QmDiscountView:LoadCallback()
    local style_config = {[4] = 0, [5] = 0, [6] = 2}
    local style_type = ActShopBoard1.GetShowStyle(Config.qm_discount_auto.activity_interface)
    self.Board:SetData(QmDiscountView,ActivityRandData.GetName(ACTIVITY_TYPE.RAND_QM_DISCOUNT),style_type)
    self:SetStyleData(style_config[style_type] or style_type, style_type)
    self.Currency:DefaultSet()
end

function QmDiscountView:SetStyleData(style_type, base_type)
	UH.SpriteName(self.ImgBG,[[XiaoMaoDi]] .. style_type)
	UH.SpriteName(self.ImgBtn,[[GouMaiAnNiu]] .. style_type)
	UH.SpriteName(self.ImgBtnBG,[[TouMing]] .. style_type)
	UH.SpriteName(self.ImgBtnText,[[_LocGouMai]] .. style_type)
	UH.SpriteName(self.ImgDescTop,[[_LocMiaoShuShang]] .. style_type)
	UH.SpriteName(self.ImgDescDown,[[_LocMiaoShuXia]] .. style_type)
    if base_type == 5 then
        self.ImgDescTop:SetObjActive(false)
        self.ImgDescDown:SetObjActive(false)
        self.ImgDesc3:SetActive(true)
    end
end

QmDiscountPanel = QmDiscountPanel or DeclareMono("QmDiscountPanel",UIWFlushPanel)

local total_discount_lv = 4
function QmDiscountPanel:QmDiscountPanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.qm_discount_data, func = self.FlushAllView},
    }
    self.cur_sel_type = -1
    self.TimeTxt:CreateActTime(ACTIVITY_TYPE.RAND_QM_DISCOUNT,TimeType.Type_Special_4,Language.QmDiscount.TimeHint)
end

function QmDiscountPanel:FlushAllView()
    local buying_cfgs  = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_QM_DISCOUNT,Config.qm_discount_auto.group_buying)
    self.all_buying_cfg = {}
    for i,v in pairs(buying_cfgs) do
        local buy_info = self.data:GetGroupDiscountInfo(v.type)
        -- LogError("FlushAllView",v.type,v)
        if buy_info and buy_info.buy_count then
            local can_buy_count = v.buy_tepy == 1 and v.buy_seccond or v.buy_seccond_day
            local buy_count_remaind = can_buy_count - buy_info.buy_count
            if buy_count_remaind > 0 then
                v.sort_value = 1
            else
                v.sort_value = 0
            end
            self.all_buying_cfg[i] = v
        elseif IS_EDITOR then
            LogError("buy_info.buy_count == nil, type =",v.type,"\nbuy_info:",buy_info)
        end
    end
    local sort_func = function (a,b)
        if a.sort_value == b.sort_value then
            return a.seq < b.seq
        else
            return a.sort_value > b.sort_value
        end
    end
    table.sort(self.all_buying_cfg, sort_func)
    self.TabBtnList:SetData(self.all_buying_cfg)
    if self.cur_cfg == nil then
        self.TabBtnList:ClickItemData(self.all_buying_cfg[1])
        self.cur_cfg = self.all_buying_cfg[1]
    else
        for i,v in pairs(self.all_buying_cfg) do
            if v.type == self.cur_cfg.type then
                self.TabBtnList:ClickItem(i)
                break
            end
        end
        self:FlushMainView()
    end
end

function QmDiscountPanel:FlushMainView()
    local cfg = self.cur_cfg
    if cfg == nil then
        return
    end
    local buy_info = self.data:GetGroupDiscountInfo(cfg.type)
    local discount_info_list = self.data:GetGroupDiscountCfg(cfg.iscount_id)
    local discount_lv = 0       --当前在第几档折扣
    local discount_info = nil   --当前档折扣配置
    for _,v in ipairs(discount_info_list) do
        if buy_info.all_buy_count >= v.discount_number then
            discount_lv = discount_lv + 1
        else
            break
        end
    end
    if discount_lv > 0 then
        discount_info = discount_info_list[discount_lv]
    end
    local rew_item = cfg.reward_item
    self.ItemCell:SetData(Item.Init(rew_item.item_id,rew_item.num,rew_item.is_bind))
    local can_buy_count = cfg.buy_tepy == 1 and cfg.buy_seccond or cfg.buy_seccond_day
    local title_txt = cfg.buy_tepy == 1 and Language.QmDiscount.MaxBuyCountAct or Language.QmDiscount.MaxBuyCountToday
    local buy_count_remaind = can_buy_count - buy_info.buy_count
    self.MyBuyedLbl.text = title_txt .. tostring(buy_count_remaind >= 0 and buy_count_remaind or 0)
    self.BuyBtnInter.Interactable = buy_count_remaind > 0
    if discount_info == nil then
        self.PriceOldCurrencyImg:SetObjActive(false)
    else
        self.PriceOldCurrencyImg:SetObjActive(true)
        self.PriceOldNum.text = tostring(cfg.original_price)
        self.PriceOldCurrencyImg.SpriteName = cfg.price_type == 0 and "GoldIcon" or "MoneyIcon"
    end
    self.PriceNowCurrencyImg.SpriteName = cfg.price_type == 0 and "GoldIcon" or "MoneyIcon"
    -- local now_price = math.roundtoint(cfg.original_price * (discount_info and discount_info.discount_proportion or 10) * 0.1)
    local now_price = math.ceil(cfg.original_price * (discount_info and discount_info.discount_proportion or 10) * 0.1)
    if now_price == 0 then
        self.PriceNowNum.text = Language.OpenAct.LimitShop.Free
    else
        self.PriceNowNum.text = tostring(now_price)
    end
    self.BuyedNumTxt.text = tostring(buy_info.all_buy_count)
    self.TxtBuy:SetActive(now_price > 0)
    self.TxtFree:SetActive(now_price <= 0)
    local one_lv_per = 1/total_discount_lv
    if discount_lv >= #discount_info_list then
        self.Prog.value = 1
    else
        local discounted_lv_num = (discount_info and discount_info.discount_number or 0)
        self.Prog.value = one_lv_per * discount_lv + Mathf.Clamp01(
            (buy_info.buy_count - discounted_lv_num)/(discount_info_list[discount_lv+1].discount_number - discounted_lv_num))*one_lv_per
    end
    
    for i=1,total_discount_lv do
        self.PointImageSet[i].SpriteName = ((i <= discount_lv) and "HongXin" or "HuangXin")
        self.DiscountValTxt[i + 1].text = string.format(Language.QmDiscount.DiscountVal,discount_info_list[i].discount_proportion)
        self.NowPricePreviewTxt[i].text = string.format(Language.QmDiscount.Sell,discount_info_list[i].discount_number)
        self.NowPricePreviewTxt[i].color = ((i <= discount_lv) and COLORS.Red7 or COLORS.Yellow11)
    end
    self.DiscountTagTxt.text = discount_lv == 0 and Language.QmDiscount.UnDiscount or Language.QmDiscount.Discounted;

    self.DiscountTagTran.parent = self.DiscountValTxt[discount_lv + 1].transform
    self.DiscountTagTran.anchoredPosition = Vector2.zero

    self.now_price = now_price
end

function QmDiscountPanel:OnTabToggleOn(cfg)
    self.cur_sel_type = cfg.type
    self.cur_cfg = cfg
    self:FlushMainView()
end

function QmDiscountPanel:OnClickClose()
    ViewMgr:CloseView(QmDiscountView)
end

function QmDiscountPanel:OnClickBuy()
    if not MallCtrl.IsNotCurrency(CurrencyType.Gold,self.now_price) then
        return
    end
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_QM_DISCOUNT,1,self.cur_cfg.type)
end

function QmDiscountPanel:OnDestroy()
   self.cur_sel_type = -1
   UIWFlushPanel.OnDestroy(self)
end

QmDiscountTabBtnItem = QmDiscountTabBtnItem or DeclareMono("QmDiscountTabBtnItem",UIWidgetBaseItem)

function QmDiscountTabBtnItem:SetData(data)
    self.data = data
    local rew_item = self.data.reward_item
    local item = Item.Init(rew_item.item_id,rew_item.num,rew_item.is_bind)
    self.ItemCell:SetData(item)
    local buy_info = ServerActivityData.Instance:GetGroupDiscountInfo(self.data.type)
    local can_buy_count = data.buy_tepy == 1 and data.buy_seccond or data.buy_seccond_day
    self.buy_count_remaind = can_buy_count - buy_info.buy_count
    self.ItemName.text = item:Name()
    self.BuyedObj:SetActive(self.buy_count_remaind <= 0)
    self:FlushRedPoint()
end

function QmDiscountTabBtnItem:FlushRedPoint()
    if self.data.original_price == 0 then
        if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(95, 80))
        end
        if self.ht == nil then
            self.ht = ServerActivityData.Instance.qm_discount_data:Care(BindTool.Bind(self.FlushRedPoint, self))
        end
        self.red_point:SetNum(self.buy_count_remaind <= 0 and 0 or 1)
    end
end

function QmDiscountTabBtnItem:GetDataType()
    return self.data
end

function QmDiscountTabBtnItem:Click()
    self.ToggleEvent:Trigger()
end

function QmDiscountTabBtnItem:OnDestroy()
    if self.red_point ~= nil then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
    if self.ht then
        ServerActivityData.Instance.qm_discount_data:Uncare(self.ht)
    end
    UIWidgetBaseItem.OnDestroy(self)
end