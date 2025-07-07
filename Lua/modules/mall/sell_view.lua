SellView = SellView or DeclareView("SellView", "mall/SellView")

VIEW_DECLARE_MASK(SellView,ViewMask.BlockClose)

function SellView:SellView()
    self.Board:SpriteName("XinXiKuang_DiBan")
    self.Board:SetData(self:GetType(), Language.Mall.ItemSell,Vector2.New(482, 668))
    self.care_handle = TradeData.Instance.item_sell_data:Care(BindTool.Bind(self.FlushRecordList,self))
    self.recommend_handle = TradeData.Instance.recommend_data:Care(BindTool.Bind(self.FlushRecommend,self))
    self.make_num = 1
    self.InputField.onValueChanged:AddListener(BindTool.Bind(self.OnClickPriceSelect, self))
end

function SellView:CloseCallback()
	TradeData.Instance.item_sell_data:Uncare(self.care_handle)
	TradeData.Instance.recommend_data:Uncare(self.recommend_handle)
	self.recommend_handle = nil
	self.care_handle = nil
end

function SellView:FlushRecommend()
    local info = TradeData.Instance:GetQueryItemRecommendInfo()
    self.min_price = info.min_price or self.min_price
    self.max_price = info.max_price or self.max_price
    self.make_price = info.price or self.make_price
    UH.SetText(self.Price, self.make_price)
    self:FlushNumView()
end

function SellView:LoadCallback(data)
    if data and data.item then
        self.data = data or {}
        self:FlushSellView(data)
        TradeCtrl.Instance:SendGetSellingListReq(data.item.item_id)
        if data.cur_status == 0 then
            TradeCtrl.Instance:SendRecommendPrice(data.item)
        end
    end
end

function SellView:FlushSellView(data)
    self.Call:SetData(data.item)
    self.group_num = data.item:GroupNum()
    self.item_num = data.item:Num()
    self.max_group_num = self:GetMaxNum(true)
    self.make_price = data.item:MinPrice()
    self.min_price = data.item:MinPrice()
    self.max_price = Item.GetMaxPrice(data.item.item_id)
    if Item.GetRecommendPrice(data.item.item_id) ~= 0 then
        self.make_price = Item.GetRecommendPrice(data.item.item_id)
    end
    UH.SetText(self.Name,data.item:QuaName())
    UH.SetText(self.Price, self.make_price)
    local type_desc = data.item:TypeDesc()
    if type(type_desc) == "table" then
        type_desc = type_desc[1]
    else
        type_desc = StringIsEmpty(type_desc) and "" or type_desc
    end
    UH.SetText(self.Type, Language.Mall.SellType .. type_desc)
    UH.SetText(self.Content,Format(Language.Mall.Content,(data.item:Desc() or "")))
    self.Group:SetActive(self.group_num ~= 1)
    UH.SetText(self.TGroup,Format(Language.Mall.Group,self.group_num))
    self:FlushNumView()
end

function SellView:FlushNumView()
    UH.SetText(self.Num, self.group_num == 1 and self.make_num or self.make_num .. Language.Mall.GroupTip)
    UH.SetText(self.TotalPrice, self.make_num * self.make_price)
    local poundage = TradeData.Instance:GetSellFee(self.make_num * self.make_price)
    UH.SetText(self.FeePrice, poundage)
    UH.SetText(self.Credit,self.make_num * self.make_price)
end

function SellView:FlushRecordList()
    self.List:SetData(TradeData.Instance:GetSellRecordList())
    self.NotItemTip:SetActive(not self.List:IsHasItem())
end

function SellView:GetMaxNum(max)
	local num = math.floor(self.item_num / self.group_num)
	return max and (num == 0 and 1 or num) or num
end

--数量改变
function SellView:OnClickAddNum(num)
    if self.make_num + num <= 0 then 
        PublicPopupCtrl.Instance:Center(Language.Mall.GroupMinNumTip)
        return 
    end
    if self.make_num + num > self.max_group_num then 
        PublicPopupCtrl.Instance:Center(Language.Mall.SellMaxNumTip)
        return 
    end
    if (self.make_num + num) * self.make_price > TradeData.Instance:GetCreditNum() then
        PublicPopupCtrl.Instance:Center(Language.Mall.NotHasCreditTip)
        return
    end
    self.make_num = self.make_num + num
    self:FlushNumView()
end

--单价改变
function SellView:OnClickAddPrice(num)
    if self.make_price + num < self.min_price then
        self.make_price = self.min_price
        self.Price.text = self.min_price
        self:FlushNumView()
        PublicPopupCtrl.Instance:Center(Language.Mall.SellMinPriceTip)
        return  
    end
    if self.make_price + num > self.max_price then
        PublicPopupCtrl.Instance:Center(Language.Mall.SellMaxPriceTip)
        return 
    end
    if (self.make_price + num) * self.make_num > TradeData.Instance:GetCreditNum() then
        PublicPopupCtrl.Instance:Center(Language.Mall.NotHasCreditTip)
        return
    end
    self.make_price = self.make_price + num
    UH.SetText(self.Price, self.make_price)
    self:FlushNumView()
end

--设置最大
function SellView:OnClickMax()
    self.make_num = self.max_group_num
    if self.make_price * self.make_num > TradeData.Instance:GetCreditNum() then
        self.make_num = math.floor(TradeData.Instance:GetCreditNum() / self.make_price)
    end
    self:FlushNumView()
end

function SellView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[5].desc)
end

function SellView:OnClickClose()
    self.SellAlertView:SetActive(false)
end

function SellView:OnClickLeft()
    ViewMgr:CloseView(SellView)
end

function SellView:OnClickRight()
    if self.make_price < self.min_price then
        self.make_price = self.min_price
        self.Price.text = self.min_price
        self:FlushNumView()
        PublicPopupCtrl.Instance:Center(Language.Mall.SellMinPriceTip)
        return
    end
    if self:GetMaxNum() == 0 then
		PublicPopupCtrl.Instance:Center(Format(Language.Mall.GroupMaxNumTip,self.group_num))
		return
    end
    local param_t = {
        index = self.data.item.index or 0,
        column = self.data.item.column_type or 0,
        item_num = self.make_num * self.group_num,
        price = self.make_price,
    }
    local sell_func = function()
        TradeCtrl.Instance:SendAddItemReq(param_t)
        ViewMgr:CloseView(SellView)
    end
    -- 异兽临世道具特殊处理
    if AnimalWorldCtrl.Instance:IsAnimalItem(self.data.item.item_id) then
        self:AnimalWorldDealWith(sell_func)
    else
        sell_func()
    end
end

-- 异兽临世道具特殊处理
function SellView:AnimalWorldDealWith(func)
    local is_need, str = AnimalWorldCtrl.Instance:IsNeedTip(2, self.data.item.item_id)
    if is_need then
        PublicPopupCtrl.Instance:AlertTip(str, func)
    else
        func()
    end
end

function SellView:OnClickPriceSelect()
    if self.data.cur_status ~= 0 then
        return
    end
    if self.Price.text ~= "" then
        local num = tonumber(self.Price.text)
        if num > self.max_price then
            num = self.max_price
            if num > TradeData.Instance:GetCreditNum() then
                num = TradeData.Instance:GetCreditNum()
            end
        elseif num <= 0 then
            num = self.min_price
        end
        self.Price.text = num
        self.make_price = num
        self:FlushNumView()
    end
end

SELLVIEW_TYPE = {
    SHELVES = 1, --上架中
    PUBLICI = 2, --公示中
    ONSALES = 3, --出售中
    SHELVED = 4, --下架过期中
}
--===========================PetSellView=====================================
PetSellView = PetSellView or DeclareView("PetSellView", "mall/PetSellView")

VIEW_DECLARE_MASK(PetSellView,ViewMask.BlockClose)

function PetSellView:PetSellView()
    self.Board:SpriteName("XinXiKuang_DiBan")
    self.Board:SetData(self:GetType(), Language.Mall.ItemSell,Vector2.New(879, 521))
    self.care_handle = TradeData.Instance.item_sell_data:Care(BindTool.Bind(self.FlushRecordList,self))
    self.recommend_handle = TradeData.Instance.recommend_data:Care(BindTool.Bind(self.FlushRecommend,self))
    self.InputField.onValueChanged:AddListener(BindTool.Bind(self.OnClickPriceSelect, self))
end

function PetSellView:CloseCallback()
	TradeData.Instance.item_sell_data:Uncare(self.care_handle)
	TradeData.Instance.recommend_data:Uncare(self.recommend_handle)
	self.recommend_handle = nil
	self.care_handle = nil
end

function PetSellView:FlushRecommend()
    local info = TradeData.Instance:GetQueryItemRecommendInfo()
    self.min_price = info.min_price or self.min_price
    self.max_price = info.max_price or self.max_price
    self.make_price = info.price or self.make_price
    UH.SetText(self.Price, self.make_price)
    self:FlushNumView()
end

function PetSellView:LoadCallback(data)
    if data and data.pet then
        self.data,self.pet = data,data.pet
        self:FlushPetSellView(data)
        TradeCtrl.Instance:SendGetSellingListReq(data.pet:ID())
        if data.cur_status == 0 then
            TradeCtrl.Instance:SendRecommendPrice({index = self.pet.index or 0},1)
        end
    end
end

function PetSellView:FlushPetSellView(data)
    self.min_price = TradeData.Instance:GetPetMinPrice(data.pet:ID(),data.pet:StrLevel()) or Item.GetMinPrice(data.pet:ID())
    self.max_price = TradeData.Instance:GetPetMaxPrice(data.pet:ID(),data.pet:StrLevel()) or Item.GetMaxPrice(data.pet:ID())
    self.make_price = data.price or self.min_price
    if not data.price and Item.GetRecommendPrice(data.pet:ID()) ~= 0 then
        self.make_price = Item.GetRecommendPrice(data.pet:ID())
    end
    if self.make_price < self.min_price then
        self.make_price = self.min_price
    end
    self.Call:SetData(data.pet)
    UH.SetText(self.Name, data.pet:Name())
    UH.Color(self.Name, data.pet:IsLianYao() and COLORS.Pink2 or COLORS.Yellow1)
	if self.YaoObj then
		self.YaoObj:SetActive(data.pet:IsLianYao())
	end
    UH.SpriteName(self.ImgPet, PetData.PetLevel[data.pet.config.quality])
    UH.SetText(self.Type, Language.Mall.SellType .. Language.Pet.RaceClass[data.pet.config.race])
    UH.SetText(self.Content,Format(Language.Mall.PetContent, data.pet:TotalGrowth(), data.pet:ConfigGrowth()))
    
    self.Mutant:SetActive(data.pet.config.mutant_times >= 1)
    self:FlushStatusView(data.cur_status or 0)
    self:FlushAttrBaseView()
end

--单价改变
function PetSellView:OnClickAddPrice(num)
    if self.make_price + num < self.min_price then
        self.make_price = self.min_price
        self.Price.text = self.min_price
        self:FlushNumView()
        PublicPopupCtrl.Instance:Center(Language.Mall.SellMinPriceTip)
        return  
    end
    if self.make_price + num > self.max_price then
        PublicPopupCtrl.Instance:Center(Language.Mall.SellMaxPriceTip)
        return 
    end
    if self.make_price + num > TradeData.Instance:GetCreditNum() then
        PublicPopupCtrl.Instance:Center(Language.Mall.NotHasCreditTip)
        return
    end
    self.make_price = self.make_price + num
    self:FlushNumView()
end

function PetSellView:FlushNumView()
    UH.SetText(self.Price, self.make_price)
    UH.SetText(self.OPrice, self.make_price)
    UH.SetText(self.FeePrice, TradeData.Instance:GetSellFee(self.make_price))
    UH.SetText(self.Credit,self.make_price)
end

--刷新状态改变
function PetSellView:FlushStatusView(status)
    local SellStatus = Language.Mall.SellStatus
    UH.SetText(self.Left,SellStatus[1][status])
    UH.SetText(self.Right,SellStatus[2][status])
    UH.SetText(self.SellTip, Language.Mall.SellStatusTip[status])
    if status == SELLVIEW_TYPE.PUBLICI and self.data.next_status_time then
        self.Time:StampTime(self.data.next_status_time)
    end
    self.OnSellView:SetActive(status > 0)
    self.Announce:SetActive(status == SELLVIEW_TYPE.PUBLICI)
    self:FlushNumView()
end

--取消 公示 取回
function PetSellView:OnClickLeft()
    if self.data.cur_status == SELLVIEW_TYPE.PUBLICI then
        ViewMgr:OpenView(NoticeView,self.data)
    elseif self.data.cur_status == SELLVIEW_TYPE.SHELVED then
        TradeCtrl.Instance:SendRemoveItemReq(self.data.sale_index or 0)
    end
    ViewMgr:CloseView(PetSellView)
end

function PetSellView:FlushRecordList()
    local list = TradeData.Instance:GetSellRecordList()
    self.List:SetData(list)
    self.NotItemTip:SetActive(#list == 0)
end

function PetSellView:FlushAttrBaseView()
    self.AttrList:SetPet(self.pet)
    for k, v in pairs(PetData.Instance:RealAttr(self.pet.index, self.pet:TotalAttr()) or {}) do
        if tostring(k) == "maxhp" then
            self.ProgMaxHp:SetProgress(self.pet:HP() / self.pet:HP())
            self.ProgMaxHp:SetText(Format(Language.Common.Progress2TxtColorDFormat, self.pet:HP(), self.pet:HP()))
        elseif tostring(k) == "maxmp" then
            self.ProgMaxMp:SetProgress(self.pet:MP() / self.pet:MP())
            self.ProgMaxMp:SetText(Format(Language.Common.Progress2TxtColorDFormat, self.pet:MP(), self.pet:MP()))
        else
            UH.SetText(self["Lbl" .. k], v)
        end
    end 
end

function PetSellView:OnClickSkillList()
    if self.IsOnClickSkillList == nil then
        local list = self.pet:GetSkillAndGiftListNoEmpty()
        self.SkillList:SetData(list)
        self.IsOnClickSkillList = true
    end
end

function PetSellView:OnClickGift(cell)
    local data = cell:GetData()
    if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
        PetData.Instance:SetClickSkill(data)
        ViewMgr:OpenView(PetSkillTip)
    end
end

--上架物品
function PetSellView:SendAddItemReq()
    local param_t = {
        is_from_pet = 1,
        index = self.pet.index or 0,
        price = self.make_price,
    }
    TradeCtrl.Instance:SendAddItemReq(param_t)
end

--上架 下架 重新上架
function PetSellView:OnClickRight()
    if self.data.cur_status == 0 then
        if self.make_price < self.min_price then
            self.make_price = self.min_price
            self.Price.text = self.min_price
            self:FlushNumView()
            PublicPopupCtrl.Instance:Center(Language.Mall.SellMinPriceTip)
            return
        end
        if self.pet:Level() <= 1 then
            self:SendAddItemReq()
        else
            ViewMgr:OpenView(PetGuiYuan, {data = self.pet, fromAttr = false, price = self.make_price})
        end
    elseif self.data.cur_status == SELLVIEW_TYPE.SHELVED then
        TradeCtrl.Instance:SendRedoAddItemOnShelveReq(self.data.sale_index,self.make_price)
    else
        TradeCtrl.Instance:SendRemoveItemReq(self.data.sale_index or 0)
    end
    ViewMgr:CloseView(PetSellView)
end

function PetSellView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[5].desc)
end

function PetSellView:OnClickClose()
    self.SellAlertView:SetActive(false)
end

function PetSellView:OnClickPriceSelect()
    if self.data.cur_status ~= 0 then
        return
    end
    if self.Price.text ~= "" then
        local num = tonumber(self.Price.text)
        if num > self.max_price then
            num = self.max_price
            if num > TradeData.Instance:GetCreditNum() then
                num = TradeData.Instance:GetCreditNum()
            end
        elseif num <= 0 then
            num = self.min_price
        end
        self.Price.text = num
        self.make_price = num
        self:FlushNumView()
    end
end

--购买弹窗
--===========================PurchaseView=====================================
PurchaseView = PurchaseView or DeclareView("PurchaseView", "mall/PurchaseView")

VIEW_DECLARE_MASK(PurchaseView,ViewMask.BlockClose)

function PurchaseView:PurchaseView()
    self.Board:SpriteName("XinXiKuang_DiBan")
    self.Board:SetData(self:GetType(), Language.Mall.ItemBuy,Vector2.New(482, 644))
    self.make_num = 1
end

function PurchaseView:LoadCallback(data)
    if data.item == nil then return end
    self.data = data or {}
    self.group_num = data.item:GroupNum()
    self.max_group_num = self:GetMaxNum(true)
    self.max_num = self.max_group_num
    if data.item:IsPet() then
        self.Call:SetData(data.pet)
        UH.SetText(self.Name, data.pet:Name())
    else
        self.Call:SetData(data.item)
        UH.SetText(self.Name, data.item:QuaName())
    end
    self.make_price = data.price
    local type_desc = data.item:TypeDesc()
    if type(type_desc) == "table" then
        type_desc = type_desc[1]
    else
        type_desc = StringIsEmpty(type_desc) and "" or type_desc
    end
    UH.SetText(self.Type, Language.Mall.SellType .. type_desc)
    if Item.GetType(data.item.item_id) == ItemType.PartnerEquip then
        local desc = MallData.JewelrySuit(data.item)
        UH.SetText(self.Content,desc..Format(Language.Mall.SellContent,data.item:Desc()))
    else
        UH.SetText(self.Content,Format(Language.Mall.Content,data.item:Desc() or ""))
    end
    UH.SetText(self.Price, self.make_price)
    self.Group:SetActive(self.group_num ~= 1)
    UH.SetText(self.TGroup,Format(Language.Mall.Group,self.group_num))
    self:FlushNumView()
end

function PurchaseView:FlushNumView()
    UH.SetText(self.Num, self.group_num == 1 and self.make_num or self.make_num .. Language.Mall.GroupTip)
    UH.SetText(self.TotalPrice, self.make_num * self.make_price)
end

function PurchaseView:GetMaxNum(max)
	local num = math.floor(self.data.item:Num() / self.group_num)
	return max and (num == 0 and 1 or num) or num
end

function PurchaseView:OnClickCancel()
    ViewMgr:CloseView(PurchaseView)
end

function PurchaseView:OnClickClose()
    ViewMgr:CloseView(PurchaseView)
end

-- function PurchaseView:OnClickNumSelect()
--     if self.Num.text ~= "" then
--         if self.group_num == 1 then
--             local num = tonumber(self.Num.text)
--             if num > self.max_num then
--                 num = self.max_num
--             elseif num <= 0 then
--                 num = 1
--             end
--             self.make_num = num
--         end
--         self:FlushNumView()
--     end
-- end

function PurchaseView:OnClickAddNum(num)
    if self.make_num + num <= 0 then
        PublicPopupCtrl.Instance:Center(Language.Mall.GroupMinNumTip)
        return
    end
    if self.make_num + num > self.max_group_num then 
        PublicPopupCtrl.Instance:Center(Language.Mall.SellMaxNumTip)
        return 
    end
    self.make_num = self.make_num + num
    self:FlushNumView()
end

function PurchaseView:OnClickPurchase()
    if TradeData.Instance:IsAnnounce() == 1 then
        PublicPopupCtrl.Instance:Center(Language.Trade.PublicityTip)
        return
    end
    local price = self.make_num * self.make_price
    if MallCtrl.IsNotCurrency(CurrencyType.Gold, price) then
        local buy_func = function()
            TradeCtrl.Instance:SendBuyItemReq(self.data,self.make_num * self.group_num ,price)
            ViewMgr:CloseView(PurchaseView)
            TradeData.Instance:SetSelectItemData()
        end
            -- 异兽临世道具特殊处理
        if AnimalWorldCtrl.Instance:IsAnimalItem(self.data.item_data.item_id) then
            self:AnimalWorldDealWith(buy_func)
        else
            buy_func()
        end
    end
end

-- 异兽临世道具特殊处理
function PurchaseView:AnimalWorldDealWith(func)
    local is_need, str = AnimalWorldCtrl.Instance:IsNeedTip(1, self.data.item_data.item_id)
    if is_need then
        PublicPopupCtrl.Instance:AlertTip(str, func)
    else
        func()
    end
end


--设置最大
function PurchaseView:OnClickMax()
    self.make_num = self.max_group_num
    self:FlushNumView()
end

--============================MallSellView============================
MallSellView = MallSellView or DeclareView("MallSellView", "mall/MallSellView")

VIEW_DECLARE_MASK(MallSellView,ViewMask.BlockClose)

function MallSellView:MallSellView()
    self.Board:SpriteName("XinXiKuang_DiBan")
    self.Board:SetData(self:GetType(), Language.Mall.ItemSell,Vector2.New(482, 644))
    self.care_handle = TradeData.Instance.item_sell_data:Care(BindTool.Bind(self.FlushRecordList,self))
    self.recommend_handle = TradeData.Instance.recommend_data:Care(BindTool.Bind(self.FlushRecommend,self))
    self.InputField.onValueChanged:AddListener(BindTool.Bind(self.OnClickPriceSelect, self))
end

function MallSellView:CloseCallback()
	TradeData.Instance.item_sell_data:Uncare(self.care_handle)
	TradeData.Instance.recommend_data:Uncare(self.recommend_handle)
	self.recommend_handle = nil
	self.care_handle = nil
end

function MallSellView:FlushRecommend()
    local info = TradeData.Instance:GetQueryItemRecommendInfo()
    self.min_price = info.min_price or self.min_price
    self.max_price = info.max_price or self.max_price
    self.make_price = info.price or self.make_price
    UH.SetText(self.Price, self.make_price)
    self:FlushNumView()
end

function MallSellView:LoadCallback(data)
    if data and data.item then
        self.data = data or {}
        self:FlushMallSellView(data)
        TradeCtrl.Instance:SendGetSellingListReq(data.item.item_id)
        if data.cur_status == 0 then
            TradeCtrl.Instance:SendRecommendPrice(data.item)
        end
    end
end

function MallSellView:FlushMallSellView(data)
    self.make_num = self.data.item.num
    self.min_price = data.item:MinPrice()
    self.max_price = Item.GetMaxPrice(data.item.item_id)
    self.make_price = data.price or self.min_price
    self.group_num = data.item:GroupNum()
    if not data.price and Item.GetRecommendPrice(data.item.item_id) ~= 0 then
        self.make_price = Item.GetRecommendPrice(data.item.item_id)
    end
    self.Call:SetData(data.item)
    UH.SetText(self.Name, data.item:QuaName())
    local type_desc = data.item:TypeDesc()
    if type(type_desc) == "table" then
        type_desc = type_desc[1]
    else
        type_desc = StringIsEmpty(type_desc) and "" or type_desc
    end
    UH.SetText(self.Type, Language.Mall.SellType .. type_desc)
    UH.SetText(self.Price, self.make_price)

    if Item.GetType(data.item.item_id) == ItemType.PartnerEquip then
        local desc = MallData.JewelrySuit(data.item)
        UH.SetText(self.Content,desc..Format(Language.Mall.SellContent,data.item:Desc()))
    else
        UH.SetText(self.Content,Format(Language.Mall.Content,data.item:Desc() or ""))
    end
    self.Group:SetActive(self.group_num ~= 1)
    UH.SetText(self.TGroup,Format(Language.Mall.Group,self.group_num))
    if self.group_num ~= 1 and data.cur_status ~= 0 then
        self.make_num = self.data.item.num / self.group_num
    end
    self:FlushStatusView(data.cur_status or 0)
end

--单价改变
function MallSellView:OnClickAddPrice(num)
    if self.make_price + num < self.min_price then
        self.make_price = self.min_price
        self.Price.text = self.min_price
        self:FlushNumView()
        PublicPopupCtrl.Instance:Center(Language.Mall.SellMinPriceTip)
        return
    end
    if self.make_price + num > self.max_price then
        PublicPopupCtrl.Instance:Center(Language.Mall.SellMaxPriceTip)
        return 
    end
    if self.make_price + num > TradeData.Instance:GetCreditNum() then
        PublicPopupCtrl.Instance:Center(Language.Mall.NotHasCreditTip)
        return
    end
    self.make_price = self.make_price + num
    UH.SetText(self.Price, self.make_price)
    self:FlushNumView()
end

function MallSellView:FlushNumView()
    for i=1,2 do
        UH.SetText(self.OPrice[i], self.make_price)
    end
    UH.SetText(self.Num, self.group_num == 1 and self.make_num or self.make_num .. Language.Mall.GroupTip)
    UH.SetText(self.TPrice, (self.make_price * self.make_num))
    UH.SetText(self.Free, TradeData.Instance:GetSellFee(self.make_price))
    UH.SetText(self.Credit,self.make_price)
end

--刷新状态改变
function MallSellView:FlushStatusView(status)
    local SellStatus = Language.Mall.SellStatus
    UH.SetText(self.Left,SellStatus[1][status])
    UH.SetText(self.Right,SellStatus[2][status])
    for i=1,2 do
        UH.SetText(self.SellTip[i], Language.Mall.SellStatusTip[status])
    end
    if status == SELLVIEW_TYPE.PUBLICI and self.data.next_status_time then
        self.Time:StampTime(self.data.next_status_time)
    end
    if status > 0 then
        self.OnSellView:SetActive(true)
        self.OnShelveView:SetActive(false)
        self:ChangeSellView(status ~= SELLVIEW_TYPE.PUBLICI)
    end
    self:FlushNumView()
end

function MallSellView:ChangeSellView(show)
    self.Announce:SetActive(not show)
    if show then
        self.OnSellsView:SetActive(true)
        self.OnSellView:SetActive(false)
    end
end

function MallSellView:FlushRecordList()
    local list = TradeData.Instance:GetSellRecordList()
    self.SellList:SetData(list)
    self.NotItemTip:SetActive(#list == 0)
end

--取消 公示 取回
function MallSellView:OnClickLeft()
    if self.data.cur_status == SELLVIEW_TYPE.PUBLICI then
        ViewMgr:OpenView(NoticeView,self.data)
    elseif self.data.cur_status == SELLVIEW_TYPE.SHELVED then
        TradeCtrl.Instance:SendRemoveItemReq(self.data.sale_index or 0)
    end
    ViewMgr:CloseView(MallSellView)
end

--上架 下架 重新上架
function MallSellView:OnClickRight()
    if self.data.cur_status == 0 then
        if self.make_price < self.min_price then
            self.make_price = self.min_price
            self.Price.text = self.min_price
            self:FlushNumView()
            PublicPopupCtrl.Instance:Center(Language.Mall.SellMinPriceTip)
            return
        end
        self:SendAddItemReq()
    elseif self.data.cur_status == SELLVIEW_TYPE.SHELVED then
        -- 过期，重新上架
        local resell_func = function()
            TradeCtrl.Instance:SendRedoAddItemOnShelveReq(self.data.sale_index,self.make_price)
        end
        -- 异兽临世道具特殊处理
        if AnimalWorldCtrl.Instance:IsAnimalItem(self.data.item.item_id) then
            self:AnimalWorldDealWith(resell_func)
        else
            resell_func()
        end
    else
        TradeCtrl.Instance:SendRemoveItemReq(self.data.sale_index or 0)
    end
    ViewMgr:CloseView(MallSellView)
end

-- 异兽临世道具特殊处理
function MallSellView:AnimalWorldDealWith(func)
    local is_need, str = AnimalWorldCtrl.Instance:IsNeedTip(2, self.data.item.item_id)
    if is_need then
        PublicPopupCtrl.Instance:AlertTip(str, func)
    else
        func()
    end
end

function MallSellView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[5].desc)
end

function MallSellView:OnClickClose()
    self.SellAlertView:SetActive(false)
end

function MallSellView:OnClickPriceSelect()
    if self.data.cur_status ~= 0 then
        return
    end
    if self.Price.text ~= "" then
        local num = tonumber(self.Price.text)
        if num > self.max_price then
            num = self.max_price
            if num > TradeData.Instance:GetCreditNum() then
                num = TradeData.Instance:GetCreditNum()
            end
        elseif num <= 0 then
            num = self.min_price
        end
        self.Price.text = num
        self.make_price = num
        self:FlushNumView()
    end
end

--上架物品
function MallSellView:SendAddItemReq()
    local param_t = {
        index = self.data.item.index or 0,
        column = self.data.item.column_type or 0,
        price = self.make_price,
    }
    TradeCtrl.Instance:SendAddItemReq(param_t)
end

--===========================购买弹窗=====================================
PurchaseEquip = PurchaseEquip or DeclareView("PurchaseEquip", "mall/PurchaseEquip")

VIEW_DECLARE_MASK(PurchaseEquip,ViewMask.BlockClose)

function PurchaseEquip:PurchaseEquip()
    self.Board:SpriteName("TanKuangDiBan")
    self.Board:SetData(self:GetType(), Language.Mall.EquipBuy,Vector2.New(622, 515))
end

function PurchaseEquip:CloseCallback()
    TradeData.Instance:SetCompCellData()
end

function PurchaseEquip:LoadCallback(data)
    if data.item == nil then return end 
    self.data = data or {}
    self.Call:SetData(data.item)
    UH.SetText(self.Price, data.price)
    UH.SetText(self.Title,Format(Language.Mall.SellItemTip,data.item:QuaName()))
    
    local comp_item = BagData.Instance:GetEquipList(Equip.GetEquipType(data.item.item_id))
    if data.item:IsJewelry() then
        comp_item = BagData.Instance:GetJewelryList(Jewelry.GetJewelryType(data.item.item_id))
    elseif data.item:IsMedal() then
        comp_item = BagData.Instance:GetMedalCompItem(MedalData.GetSpecialType(data.item:SpecialId()))
    end
    if comp_item ~= nil then
        local item = Item.Create(comp_item.vo)
        TradeData.Instance:SetCompCellData(item.param)
        self.CompCell:SetData(MallData.Attribute(item))
    end
    self.Comp:SetActive(comp_item ~= nil)
    self.ItemCell:SetData(MallData.Attribute(data.item))
end

function PurchaseEquip:OnClickPurchase()
    if TradeData.Instance:IsAnnounce() == 1 then
        PublicPopupCtrl.Instance:Center(Language.Trade.PublicityTip)
        return
    end
    if MallCtrl.IsNotCurrency(CurrencyType.Gold, self.data.price) then
        TradeCtrl.Instance:SendBuyItemReq(self.data,1,self.data.price)
        ViewMgr:CloseView(PurchaseEquip)
        TradeData.Instance:SetSelectItemData()
    end
end

function PurchaseEquip:OnClickClose()
    ViewMgr:CloseView(PurchaseEquip)
end

--===========================EquipSellView=====================================
EquipSellView = EquipSellView or DeclareView("EquipSellView", "mall/EquipSellView")

VIEW_DECLARE_MASK(EquipSellView,ViewMask.BlockClose)

function EquipSellView:EquipSellView()
    self.care_handle = TradeData.Instance.item_sell_data:Care(BindTool.Bind(self.FlushRecordList,self))
    self.recommend_handle = TradeData.Instance.recommend_data:Care(BindTool.Bind(self.FlushRecommend,self))
    self.InputField.onValueChanged:AddListener(BindTool.Bind(self.OnClickPriceSelect, self))
end

function EquipSellView:CloseCallback()
	TradeData.Instance.item_sell_data:Uncare(self.care_handle)
	TradeData.Instance.recommend_data:Uncare(self.recommend_handle)
	self.recommend_handle = nil
	self.care_handle = nil
end

function EquipSellView:FlushRecommend()
    local info = TradeData.Instance:GetQueryItemRecommendInfo()
    self.min_price = info.min_price or self.min_price
    self.max_price = info.max_price or self.max_price
    self.make_price = info.price or self.make_price
    UH.SetText(self.Price, self.make_price)
    self:FlushNumView()
end

function EquipSellView:LoadCallback(data)
    if data and data.item then
        self.data = data or {}
        self:FlushEquipSellView(data)
        TradeCtrl.Instance:SendGetSellingListReq(data.item.item_id,MedalData.GetSpecialTypeByItem(data.item))
        if data.cur_status == 0 then
            TradeCtrl.Instance:SendRecommendPrice(data.item)
        end
    end
end

function EquipSellView:FlushEquipSellView(data)
    self.min_price = data.item:MinPrice() 
    self.max_price = data.item:MaxPrice()
    self.make_price = data.price or self.min_price
    if not data.price and data.item:RecommendPrice() ~= 0 then
        self.make_price = data.item:RecommendPrice()
    end
    self.Call:SetData(data.item)
    UH.SetText(self.Name, data.item:QuaName())
    self:FlushStatusView(data.cur_status or 0)
    self.ItemCell:SetData(MallData.Attribute(data.item))
end

--单价改变
function EquipSellView:OnClickAddPrice(num)
    if self.make_price + num < self.min_price then
        self.make_price = self.min_price
        self.Price.text = self.min_price
        self:FlushNumView()
        PublicPopupCtrl.Instance:Center(Language.Mall.SellMinPriceTip)
        return  
    end
    if self.make_price + num > self.max_price then
        PublicPopupCtrl.Instance:Center(Language.Mall.SellMaxPriceTip)
        return 
    end
    if self.make_price + num > TradeData.Instance:GetCreditNum() then
        PublicPopupCtrl.Instance:Center(Language.Mall.NotHasCreditTip)
        return
    end
    self.make_price = self.make_price + num
    self:FlushNumView()
end

function EquipSellView:FlushNumView()
    UH.SetText(self.Price, self.make_price)
    UH.SetText(self.OPrice, self.make_price)
    UH.SetText(self.FeePrice, TradeData.Instance:GetSellFee(self.make_price))
    UH.SetText(self.Credit,self.make_price)
end

--刷新状态改变
function EquipSellView:FlushStatusView(status)
    local SellStatus = Language.Mall.SellStatus
    UH.SetText(self.Left,SellStatus[1][status])
    UH.SetText(self.Right,SellStatus[2][status])
    UH.SetText(self.SellTip, Language.Mall.SellStatusTip[status])
    if status == SELLVIEW_TYPE.PUBLICI and self.data.next_status_time then
        self.Time:StampTime(self.data.next_status_time)
    end
    self.OnSellView:SetActive(status > 0)
    self.Announce:SetActive(status == SELLVIEW_TYPE.PUBLICI)
    self:FlushNumView()
end

--取消 公示 取回
function EquipSellView:OnClickLeft()
    if self.data.cur_status == SELLVIEW_TYPE.PUBLICI then
        ViewMgr:OpenView(NoticeView,self.data)
    elseif self.data.cur_status == SELLVIEW_TYPE.SHELVED then
        TradeCtrl.Instance:SendRemoveItemReq(self.data.sale_index or 0)
    end
    ViewMgr:CloseView(EquipSellView)
end

function EquipSellView:FlushRecordList()
    local list = TradeData.Instance:GetSellRecordList()
    self.List:SetData(list)
    self.NotItemTip:SetActive(#list == 0)
end

--上架物品
function EquipSellView:SendAddItemReq()
    local param_t = {
        index = self.data.item.index or 0,
        column = self.data.item.column_type or 0,
        price = self.make_price,
    }
    TradeCtrl.Instance:SendAddItemReq(param_t)
end

--上架 下架 重新上架
function EquipSellView:OnClickRight()
    if self.data.cur_status == 0 then
        if self.make_price < self.min_price then
            self.make_price = self.min_price
            self.Price.text = self.min_price
            self:FlushNumView()
            PublicPopupCtrl.Instance:Center(Language.Mall.SellMinPriceTip)
            return
        end
        if (self.data.item:IsEquip()  or self.data.item:IsJewelry()) and (self.data.item:HadStone() or self.data.item:HasSpirit()) then
            PublicPopupCtrl.Instance:AlertTip(Language.Mall.PopupStoneTip,function()
                if not BagData.Instance:TryFindEmptyInBagByType() then
                    PublicPopupCtrl.Instance:Center(ErrorText[132])
                    return
                else
                    if BagData.Instance:GetEmptyNumInBag() < (self:GetEmptyNum(self.data.item:Stone()) + self.data.item:GetSpiritNum()) then
                        PublicPopupCtrl.Instance:Center(ErrorText[133])
                        return
                    end
                end
                self.data.item:UnStone()
                self.data.item:RemoveAllSpirit()
                ViewMgr:CloseView(EquipSellView)
                self:SendAddItemReq()
            end)
            return
        end
        self:SendAddItemReq()
    elseif self.data.cur_status == SELLVIEW_TYPE.SHELVED then
        TradeCtrl.Instance:SendRedoAddItemOnShelveReq(self.data.sale_index,self.make_price)
    else
        TradeCtrl.Instance:SendRemoveItemReq(self.data.sale_index or 0)
    end
    ViewMgr:CloseView(EquipSellView)
end

function EquipSellView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[5].desc)
end

function EquipSellView:OnClickCloseAlert()
    self.SellAlertView:SetActive(false)
end

function EquipSellView:OnClickClose()
    ViewMgr:CloseView(EquipSellView)
end

function EquipSellView:OnClickPriceSelect()
    if self.data.cur_status ~= 0 then
        return
    end
    if self.Price.text ~= "" then
        local num = tonumber(self.Price.text)
        if num > self.max_price then
            num = self.max_price
            if num > TradeData.Instance:GetCreditNum() then
                num = TradeData.Instance:GetCreditNum()
            end
        elseif num <= 0 then
            num = self.min_price
        end
        self.Price.text = num
        self.make_price = num
        self:FlushNumView()
    end
end

function EquipSellView:GetEmptyNum(stone_list)
    local list = {}
    for k,value in pairs(stone_list) do
        if value > 0 and list[value] == nil then
            list[value] = true
        end
    end
    local num = 0
    for k,v in pairs(list) do
        num = num + 1
    end
    return num
end