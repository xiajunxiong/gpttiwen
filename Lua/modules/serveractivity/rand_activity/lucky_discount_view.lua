LuckyDiscountView = LuckyDiscountView or DeclareView("LuckyDiscountView","serveractivity/lucky_discount",Mod.Splendid.LuckyDiscount)
VIEW_DECLARE_LEVEL(LuckyDiscountView, ViewLevel.L1)
function LuckyDiscountView:LuckyDiscountView()
    local style_type = ActShopBoard1.GetShowStyle(Config.lucky_discount_auto.activity_interface)
    self.Board:SetData(self:GetType(),ActivityRandData.GetName(ACTIVITY_TYPE.RAND_LUCKY_DISCOUNT),style_type)
    self.Currency:DefaultSet()
    self:SetStyleData(style_type)
end

function LuckyDiscountView:SetStyleData(style_type)
	UH.SpriteName(self.ImgTop,[[XinYunPeiTu]] .. style_type)
    local key_list = {[1] = true, [2] = true, [3] = true, [5] = true}
	local is_special = key_list[style_type] or false
    UH.LocalPosG(self.Panel,Vector3.New(46,is_special and -100 or -72))
    self.LightEffect:SetActive(style_type == 0 or style_type > 3)
    UH.SpriteName(self.ArtTips, "_LocMianFeiChouQu".. (style_type == 5 and 5 or 0))
end

----------------------------LuckyDiscountPanel----------------------------
LuckyDiscountPanel = LuckyDiscountPanel or DeclareMono("LuckyDiscountPanel", UIWFlushPanel)

function LuckyDiscountPanel:LuckyDiscountPanel()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.lucky_discount_data,func = self.OnFlushView,init_call = false,keys = {"is_try"}},
        {data = self.data.lucky_discount_data,func = self.OnFlushList,init_call = false,keys = {"buy_count"}},
    }
end

function LuckyDiscountPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_LUCKY_DISCOUNT,TimeType.Type_Special_4,Language.LuckyDiscount.TimeTip)
    self:OnFlushView()
end

function LuckyDiscountPanel:OnClickTry()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_LUCKY_DISCOUNT,1)
    self.BrandList:SetActive(false)
end

function LuckyDiscountPanel:OnFlushView()
    self.ShopPanel:SetActive(self.data:IsLuckyDiscountTry())
    self:OnFlushList()
end

function LuckyDiscountPanel:OnFlushList()
    if not self.data:IsLuckyDiscountTry() then
        return
    end
    UH.SpriteName(self.Image,self.data:GetLuckyDiscountNum())
    self.List:SetData(self:GetShopList())
end

function LuckyDiscountPanel:GetShopList()
    local list = self.data:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_LUCKY_DISCOUNT,Config.lucky_discount_auto.reward)
    table.sort(list,DataHelper.WeightSort("seq",function(data)
        if data.frequency - self.data:GetLuckyDiscountTimes(data) == 0 then
            return 1000
		end
    end))
    return list
end

----------------------------LuckyDiscountItem----------------------------
LuckyDiscountItem = LuckyDiscountItem or DeclareMono("LuckyDiscountItem", UIWidgetBaseItem)

function LuckyDiscountItem:SetData(data)
    data.item = Item.New(data.reward_item)
    self.Call:SetData(data.item)
    data.price_type = data.type
    UH.SetGold(self.Gold,data.type)
    UH.SetText(self.Name,Item.GetName(data.item.item_id))
    UH.SetText(self.OPrice,Language.Guild.PriceTips .. data.original_price)

    local discount = ServerActivityData.Instance:GetLuckyDiscountNum()
    data.price = math.ceil(data.original_price * discount / 10)
    UH.SetText(self.Discount,Format(Language.Shop.Discount,discount))
    UH.SetText(self.PPrice,data.price == 0 and Language.OpenAct.LimitShop.Free or data.price)
    self.BtnFree:SetActive(data.price == 0)

    data.limit_times = data.frequency - ServerActivityData.Instance:GetLuckyDiscountTimes(data)
    UH.SetText(self.Times,data.frequency ~= 0 and Format(Language.LuckyDiscount.Limit[data.limit_type],data.limit_times,data.frequency) or "")

    self.BtnInter.Interactable = data.frequency == 0 or data.limit_times ~= 0
    self.BtnFreeInter.Interactable = self.BtnInter.Interactable
    self.RedPos:SetNum(self.BtnInter.Interactable and 1 or 0)

    UIWidgetBaseItem.SetData(self, data)
end

function LuckyDiscountItem:OnClickBuy()
    if self.data.price == 0 then
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_LUCKY_DISCOUNT,2,self.data.seq,1)
        return
    end
    self.data.confirm_func = function(make_num)
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_LUCKY_DISCOUNT,2,self.data.seq,make_num)
    end
    ViewMgr:OpenView(BatchBuyView,self.data)
end

----------------------------LuckyDiscountGain----------------------------
LuckyDiscountGain = LuckyDiscountGain or DeclareView("LuckyDiscountGain","serveractivity/lucky_discount_gain")

VIEW_DECLARE_MASK(LuckyDiscountGain,ViewMask.BgBlockClose)

function LuckyDiscountGain:LoadCallback(param_t)
    local config = Config.lucky_discount_auto.discount
    for i=1,self.BrandList:Length() do
        if config[i] then
            self.BrandList[i]:SetData(config[i])
        end
    end
    local index = DataHelper.SimpleIndex(config,"proportion",param_t.discount)
    -- local index = DataHelper.SimpleCondIndex(config,function(v)
    --     return v.proportion == param_t.discount
    -- end)

    if self.BrandList[index] then
        self.BrandList[index]:PlayAnim()
    end

    self.ShowTimer = TimeHelper:AddDelayTimer(function()
        for i=1,self.BrandList:Length() do
            if i ~= index then
                self.BrandList[i]:PlayAnim()
            end
        end
    end, 2)
    
    self.CloseTimer = TimeHelper:AddDelayTimer(function()
        ViewMgr:CloseView(LuckyDiscountGain)
    end, 4)
end

function LuckyDiscountGain:CloseCallback()
    ServerActivityData.Instance.lucky_discount_data.is_try = true
	TimeHelper:CancelTimer(self.ShowTimer)
	TimeHelper:CancelTimer(self.CloseTimer)
end

----------------------------LuckyDiscountGainItem----------------------------
LuckyDiscountGainItem = LuckyDiscountGainItem or DeclareMono("LuckyDiscountGainItem", UIWidgetBaseItem)

function LuckyDiscountGainItem:SetData(data)
    UH.SetText(self.Discount,data.proportion)
    UIWidgetBaseItem.SetData(self, data)
end

function LuckyDiscountGainItem:PlayAnim()
    if self.data == nil then return end
    self.Anim:Play()
end