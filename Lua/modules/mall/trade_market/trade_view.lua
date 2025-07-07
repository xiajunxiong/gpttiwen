TradeView = TradeView or DeclareMono("TradeView", UIWFlushPanel)

--=======big_type========= 大类型
--=======small_type======= 小类型
--=======level_type======= 等级类型
--=======bag_type=======   背包类型
--=======show_screen=======显示类型

function TradeView:TradeView()
    self.data = TradeData.Instance
    self.data_cares = {
        {data = self.data.select_data,func = self.FlushSellButton,init_call = true},
        {data = self.data.shop_data,  func = self.FlushShopList,init_call = false},
        {data = self.data.sell_data,  func = self.FlushSellList,init_call = true},
        {data = self.data.big_search_data,  func = self.FlushBigSearchView,init_call = false},
        {data = self.data.credit_data,func = self.FlushCreditView,init_call = true},
        {data = PetData.Instance.pet_list,func = self.FlushPetView,init_call = false},
        {data = ActivityData.Instance:GetActivityEvent(),func = self.FlushDiscountView,keys = {"act_type", "status"}},
    }
end

function TradeView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data:SetJumpItemData()
    self.data:SetOpenLinkData()
    self.data:SetViewType(1)
    TradeCtrl.Instance.SendSearchSeq = nil
    TimeHelper:CancelTimer(self.search_time)
    TimeHelper:CancelTimer(self.drag_time)
    TimeHelper:CancelTimer(self.ShowTimer)
end

--初始化
function TradeView:Awake()
    UIWFlushPanel.Awake(self)
    local data = self.data:GetBtnItemList()
    self.PetGridList:SetData(PetData.Instance:GetRearrangeData())
    UH.SetText(self.SellTip,self.data:GetDesc())
    local index = self.data:GetSelectIndex(self.data:GetSelectBigType()) or 1
    self.BtnItemList:SetDataList(data,index)
    if index > 8 then self.BtnItemList:ForceJumpVertical(326) end 
    self:OnClickBtnItem(nil,data[index])

    local is_jump = self.data:GetOpenLinkData() ~= nil or self.data:GetSearchItemId() ~= nil or self.data:GetSearchType() ~= nil

    if self.data:GetSearchItemId() then
        local item_id = self.data:GetSearchItemId()
        TradeCtrl.Instance:SendSearchByIDReq(item_id,0)
        self.is_search_item = {item_id = item_id}
        self.search_name = Item.GetName(item_id)
        if self.data:GetSelectBigType() == nil then self.BtnItemList:CancelSelect() end 
        self.data:SetSearchItemId()
        if Item.GetBigType(item_id) == ITEM_BIG_TYPE.PET then
            self.send_search_func = function()
                -- self.big_type = self.data:GetItemType(item_id)
                -- self.BtnItemList:SetDataList(self.data:GetBtnItemList(),self.data:GetSelectIndex(self.big_type))
                -- self:OnClickShopItem(nil,self.data:GetFiltrateConfig(Item.GetSearchType(item_id)))
                PublicPopupCtrl.Instance:Center(Language.Trade.PetSearchTip)
            end
        end
    end
    self.data:SetSelectBigType()

    if self.data:GetSearchType() then
        local type_seq = self.data:GetSearchType()
        TradeCtrl.Instance:SendSearchSmallTypeReq(type_seq,nil,0)
        local config = self.data:GetFiltrateConfig(type_seq)
        self.is_SearchSmallType = true
        self.small_type = type_seq
        self.big_type = config.item_type
        self.data:SetSearchType()
    end 
    --背包跳转处理
    if self.data:GetJumpItemData() then
        self.SellToggle.isOn = true
        CellClicks.OnClickSell({GetData = function()
            return self.data:GetJumpItemData()
        end})
        self:OnClickType(2)
    else
        if self.data:GetOpenLinkData() then
            local link_data = self.data:GetOpenLinkData()
            self.BtnItemList:CancelSelect()
            self.search_name = link_data.item_name
            TradeCtrl.Instance:SendSearchAdvertiseReq()
            self.data:SetOpenLinkData()
        end
        self.ShopToggle.isOn = true
    end
    --弹出规则
    if UnityPlayerPrefs.GetInt("TradeHelp" .. RoleData.Instance:GetRoleId()) == 0 and is_jump == false then
        UnityPlayerPrefs.SetInt("TradeHelp" .. RoleData.Instance:GetRoleId(),1)
        self.ShowTimer = Invoke(function()
            ViewMgr:OpenView(TradeHelp)
        end,0.5)
    end
end

--================点击函数================
--类型选择
function TradeView:OnClickType(type)
    self.data:SetViewType(type)
    if type ~= 2 then
        if self.data:GetOpenLinkData() then
            return
        end
        if type == 1 then
            local data = self.data:GetBtnItemList()
            self.BtnItemList:SetDataList(data)
            self:OnClickBtnItem(nil,data[1])
        else
            local data = self.data:GetPublicityList()
            self.BtnItemList:SetDataList(data)
            self:OnClickBtnItem(nil,data[1])
        end
    else
        self:FlushDiscountView()
    end
end

function TradeView:FlushDiscountView()
    self.Discount:SetActive(ActivityData.IsOpen(ACTIVITY_TYPE.RAND_TAX_DISCOUNT))
    self.DealDiscount:SetActive(ActivityData.IsOpen(ACTIVITY_TYPE.RAND_DEAL_DISCOUNT))
end

--打开玩法
function TradeView:OnClickTip()
    ViewMgr:OpenView(RuleView)
end

function TradeView:OnClickHelp()
    ViewMgr:OpenView(TradeHelp)
end

function TradeView:OnClickCreditHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[80].desc)
end

--点击左边列表
function TradeView:OnClickBtnItem(cell, data)
    if self.search_data then
        self.search_data = nil
    end

    if self.show_data then
        self.show_data = nil
    end

    if self.big_type then
        self.big_type = nil
    end

    if self.small_type then
        self.small_type = nil
    end

    self.level_type = nil
    
    self.big_type = data.seq
    self.data:SetPageData()
    self.data:SetSelectItemData()
    self.BtnParent:SetActive(false)
    UH.SetText(self.SelectName, Language.Achieve.All)

    self.Search.text = ""
    self.search_name = nil

    if data.seq == -1 then
        TradeCtrl.Instance:SendGetFocusListReq()
    else
        self.show_screen = data.is_show_screen
        self:FlushShowView()
    end

    self.is_SearchSmallType = nil

    self.is_search_item = nil

    --清除选中
    self.ShopList:CancelSelect()
end

--点击交易商店列表
function TradeView:OnClickShopItem(cell, data)
    self.data:SetSelectItemData(data)
    if data.item_id == 0 then
        self.ShopList:CancelSelect()
        return 
    end
    if self.search_data then
        if data.flush_index == MallConfig.TradeShop.Search then
            TradeCtrl.Instance:SendSearchByIDReq(data.item_id,0)
        else
            if data.special_type ~= nil then
                TradeCtrl.Instance:SendSearchByMedalTypeReq(data.special_type,data.color)
            else
                TradeCtrl.Instance:SendSearchSmallTypeReq(data.seq,nil,0)
            end
        end
        self.is_search_item = data
        self.search_name = data.name or ""
        self.search_data = nil
    elseif data.seq then
        TradeCtrl.Instance:SendSearchSmallTypeReq(data.seq,nil,0)
        self.is_SearchSmallType = true
        UH.SetText(self.SelectName, data.name)
        self.small_type = data.seq
        self.show_data = nil
        if self.big_type == 15 then
            UH.SetText(self.SelectName, Language.Common.ProfTitle)
        end
    end
end

--选择按钮
function TradeView:OnClickselect()
    if self.is_search_item then
        PublicPopupCtrl.Instance:Center(Language.Trade.NotFunction)
        return  
    end
    local config = self.data:GetItemTypeConfig(self.big_type)
    if config.is_show_screen > 0 then
        local list = self.data:GetSelectData(self.big_type,config)
        if config.is_show_three == 1 and self.small_type and self:IsShowThree() then
            list = self.data:GetLevelData()
        elseif config.is_show_three == 2 and self.small_type and self:IsShowThree() then
            list = self.data:GetProfData()
        end
        if #list > 1 then
            self.BtnList:SetData(list)
            self.BtnParent:SetActive(not self.BtnParent.activeSelf)
        else
            PublicPopupCtrl.Instance:Center(Language.Trade.NotFunction)
        end
    else
        PublicPopupCtrl.Instance:Center(Language.Trade.NotFunction)
    end
end

--点击选项
function TradeView:OnClickBtn(data)
    self.small_type = data.seq or self.small_type
    self.level_type = data.level or self.level_type
    self.BtnParent:SetActive(false)
    UH.SetText(self.SelectName, data.name)
    if self.big_type == 15 and not self.level_type then
        UH.SetText(self.SelectName,Language.Common.ProfTitle)
    end
    if data.big_type then
        TradeCtrl.Instance:SendSearchBigTypeReq(data.big_type,0)
        self.small_type = nil
        return
    end
    TradeCtrl.Instance:SendSearchSmallTypeReq(self.small_type,self.level_type,0)
    if data.seq then
        self.is_SearchSmallType = true
    end
end

--点击排序
function TradeView:OnClickSort()
    if self.show_screen == 1 and not self.small_type then
        return 
    end
    local order = self.data:GetOrderType()
    self.data:SetOrderType(not order)
    self.SortArrow:SetActive(not order)
    if self.is_SearchSmallType then
        TradeCtrl.Instance:SendSearchSmallTypeReq(self.small_type,self.level_type,0)
    elseif self.is_search_item ~= nil then
        if self.is_search_item.special_type ~= nil then
            TradeCtrl.Instance:SendSearchByMedalTypeReq(self.is_search_item.special_type,self.is_search_item.color)
        else
            TradeCtrl.Instance:SendSearchByIDReq(self.is_search_item.item_id,0)
        end
    else
        TradeCtrl.Instance:SendSearchByTypeReq(self.small_type,self.big_type,self.level_type)
    end
end

--重置
function TradeView:OnClickReset()
    if self.is_search_item then
        self:OnClickSearch(true)
        return
    end
    if self.big_type == -1 or not self.big_type then return end
    self.BtnParent:SetActive(false)
    self.search_data = nil
    
    if self.show_screen == 1 and self.level_type and self.small_type then
        self.is_SearchSmallType = true
    else
        self.is_SearchSmallType = nil
    end
    self:FlushShowView()
end

--物品搜索
function TradeView:OnClickSearch(not_flush)
    self.is_search_item = nil
    if self.Search.text == "" then return end
    self.search_data = self.data:GetFindList(self.Search.text)
    if #self.search_data > 0 then
        self.data:SetPageData(1,math.ceil(#self.search_data/GameEnum.TRADE_LIST_SHOW_MAX_NUM))
        self.BtnItemList:CancelSelect()
        self:FlushShopView(self.data:GetPageData(self.search_data,1))
    else
        PublicPopupCtrl.Instance:Center(Language.Trade.NotItemName[self.data:GetAnnounce()])
    end
    if not_flush == nil then
        self:FlushTimeView()
    end
end

--点击页数
function TradeView:OnClickAddPage(num)
    local cur_page,total_page = self.data:GetPage()
    local page = cur_page + num
    if page > 0 and page <= total_page then
        self.data:SetCurPage(page)
        if self.search_data then
            self:FlushShopView(self.data:GetPageData(self.search_data,page))
        elseif self.show_data and not self.is_SearchSmallType then
            self:FlushShopView(self.data:GetPageData(self.show_data,page))
            TradeCtrl.Instance:SendGetMerchandiseNumReq(self.big_type)
        elseif self.is_search_item ~= nil then
            if self.is_search_item.special_type ~= nil then
                TradeCtrl.Instance:SendSearchByMedalTypeReq(self.is_search_item.special_type,self.is_search_item.color,page-1)
            elseif self.is_search_item.seq then
                TradeCtrl.Instance:SendSearchSmallTypeReq(self.is_search_item.seq,nil,page-1)
            else
                TradeCtrl.Instance:SendSearchByIDReq(self.is_search_item.item_id,page-1)
            end
        else
            if self.is_SearchSmallType then
                TradeCtrl.Instance:SendSearchByTypeReq(self.small_type,nil,self.level_type,page-1)
            else
                TradeCtrl.Instance:SendSearchByTypeReq(self.small_type,self.big_type,self.level_type,page-1)
            end
        end
    end   
end

--点击购买
function TradeView:OnClickPurchase()
    TradeCtrl.Instance:OnenPurchaseView()
end

--点击出售商店列表
function TradeView:OnClickSellItem(cell, data)
    if data == nil or data.item_id == 0 then
        self.SellList:CancelSelect()
        return
    end
    if data.sold_num and data.sold_num > 0 then
        TradeCtrl.Instance:SendSellItemReq(data.sale_index)
        return
    else
        if data.item == nil then
            return 
        end
        if data.item:IsEquip() or data.item:IsJewelry() or data.item:IsMedal() then
			ViewMgr:OpenView(EquipSellView,data)
			return
		end
        ViewMgr:OpenView(data.item:IsPet() and PetSellView or MallSellView,data)
    end
end

--交易记录
function TradeView:OnClickRecord()
    ViewMgr:OpenView(RecordView)
end

--背包类型
function TradeView:OnClickBagType(type)
    if type == 1 then
        self.GridList:SetData(BagData.Instance:GetScreenData())
    else
        self.PetGridList:SetData(PetData.Instance:GetRearrangeData())
    end
    self.bag_type = type
end

--================刷新页面================
function TradeView:FlushShopList()
    local list = self.data:GetShopData(self.big_type == -1 and self.search_name == nil)
    self.ShopList:SetData(list)
    self:FlushFollowTip(#list == 0)
    self:FlushShopPageView()

    if self.data:GetOpenLinkData() then
        if self.data.shop_data.announce == 1 then
            self.ShopToggle.isOn = true
            self:OnClickType(1)
        else
            self.PublicityToggle.isOn = true
            self:OnClickType(3)
        end
    end
    if #list == 0 then
        if self.send_search_func then
            self.send_search_func()
            self.send_search_func = nil
        end
    end
end

function TradeView:FlushFollowTip(active)
    self.FollowTip:SetActive(active)
    local name = ""
    local config = self.data:GetItemTypeConfig(self.big_type or 0)
    local name = string.gsub(config.name," ","")
    if self.search_name then
        name = self.search_name
    elseif self.small_type then
        local config = self.data:GetFiltrateConfig(self.small_type)
        if config then
            name = config.name
            if config.item_type == 3 then
                name = name .. Language.Pet.Pet
            end
        end
    end
    UH.SetText(self.NotTip,Format(Language.Trade.NotTip,name))
end

--出售页面 服务端
function TradeView:FlushSellList()
    self.SellList:SetData(self.data:GetSellListData())
    self:OnClickBagType(self.bag_type or 1)
    --按钮红点
    if not LoginData.Instance:IsOnCrossSever() then
        if self.red_point == nil then
            self.red_point = UiRedPoint.New(self.TabButton, Vector3.New(80,22,0))
        end
        self.red_point:SetNum(self.data:GetTradeRemindNum())
	end
end

--刷新购买 静态
function TradeView:FlushShopView(list)
    self.ShopList:SetData(list)
    self:FlushShopPageView()
    self.FollowTip:SetActive(false)
end

--刷新页数
function TradeView:FlushShopPageView()
    local cur_page,total_page = self.data:GetPage()
    self.BtnShopPage:SetActive(total_page ~= 0)
    UH.SetText(self.ShopPage, Format("%s / %s",cur_page,total_page))
end

--刷新显示分类
function TradeView:FlushShowView()
    if self.show_screen == 1 then
        if self.level_type == nil then
            self.show_data = self.data:GetTypeData(self.big_type)
            self.data:SetPageData(1,math.ceil(#self.show_data/GameEnum.TRADE_LIST_SHOW_MAX_NUM))
            self:FlushShopView(self.data:GetPageData(self.show_data,1))
            TradeCtrl.Instance:SendGetMerchandiseNumReq(self.big_type)
            UH.SetText(self.SelectName,Language.Achieve.All)
            self.small_type = nil
        else
            TradeCtrl.Instance:SendSearchSmallTypeReq(self.small_type,nil,0)
            local config = self.data:GetFiltrateConfig(self.small_type)
            if config then
                UH.SetText(self.SelectName,config.name)
            end
            if self.big_type == 15 then
                UH.SetText(self.SelectName, Language.Common.ProfTitle)
            end
            self.level_type = nil
        end
    else
        UH.SetText(self.SelectName,Language.Achieve.All)
        TradeCtrl.Instance:SendSearchBigTypeReq(self.big_type,0)
        self.small_type = nil
    end
end

function TradeView:FlushSellButton()
    local data = self.data.select_data.list or {}
    self.SellInter.Interactable = data.item_data ~= nil
end

function TradeView:FlushBigSearchView()
    if self.show_screen == 1 then
        for k,item in pairs(self.ShopList.index_item or {}) do
            if item ~= nil then
                item:FlushSearchView()
            end
        end
    end
end

function TradeView:FlushCreditView()
    UH.SetText(self.CreditNum,Format(Language.Mall.CreditTip,self.data:GetCreditNum()))
end

function TradeView:FlushPetView()
    self.PetGridList:SetData(PetData.Instance:GetRearrangeData())
end
--拖拽操作
function TradeView:OnClickDrag()
    local cur_page,total_page = self.data:GetPage()
    if total_page < 2 then
        return
    end
    local function SetNum(num)
        self.ScrollRect.verticalNormalizedPosition = 1
        self.drag_time = TimeHelper:AddDelayTimer(function ()
            self:OnClickAddPage(num)
        end, 0.3)
    end
    local num = self.Content.localPosition.y
    if num > 30 then
        SetNum(1)
    elseif num < -30 then
        SetNum(-1)
    end
end

--搜索倒计时操作
function TradeView:FlushTimeView()
    local end_time = 5
    self.ImgSearch:SetActive(false)
    self.FlushInter.Interactable = false
	self.search_time = TimeHelper:AddCountDownTT(function()
        UH.SetText(self.SearchTime,end_time .. "s")
        end_time = end_time - 1
	end,
    function()
        UH.SetText(self.SearchTime,"")
        self.ImgSearch:SetActive(true)
        self.FlushInter.Interactable = true
	end,5,1,true)
end

function TradeView:IsShowThree()
    local config = self.data:GetFiltrateConfig(self.small_type)
    if config then
        return config.is_show_three == 1
    end
    return true
end