SummerMazeShopView = SummerMazeShopView or DeclareView("SummerMazeShopView", "serveractivity/summer_first/summer_maze_shop")
VIEW_DECLARE_LEVEL(SummerMazeShopView, ViewLevel.L2)
VIEW_DECLARE_MASK(SummerMazeShopView, ViewMask.BgBlock)

function SummerMazeShopView:OnClickClose()
    ViewMgr:CloseView(SummerMazeShopView)
end


SummerMazeShopPanel = SummerMazeShopPanel or DeclareMono("SummerMazeShopPanel", UIWFlushPanel)
function SummerMazeShopPanel:SummerMazeShopPanel()
    self.Data = SummerMazeData.Instance
    self.data_cares = {
        {data = self.Data.shop_info, func = self.FlushPanel, init_call = true},
    }
    self.item_list:SetCompleteCallBack(function()
        self.list_total:SetActive(true)
        self.SellShow:Play()
    end)
end 

function SummerMazeShopPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function SummerMazeShopPanel:FlushPanel()
    self.list_total:SetActive(false)
    local data = self.Data:GetShopData()
    self.item_list:SetData(data)
    self.list_num = #data
    self:FlushPageJump()
end 

function SummerMazeShopPanel:FlushPageJump()
    local cur_page_offset = self.part_page_content:GetLocalPosition()
    self.btn_left:SetActive((not (cur_page_offset.x > -215)) and self.list_num > 3)
    self.btn_right:SetActive((not (cur_page_offset.x < -(self.list_num - 3) * 215) and self.list_num > 3))
end

function SummerMazeShopPanel:OnClickPage(index)
    local cur_page_offset = self.part_page_content:GetLocalPosition()
    local width = 222

    if index == 0 then 
        if cur_page_offset.x <= - width * 3 then
            cur_page_offset.x = cur_page_offset.x + width * 3
        else
            cur_page_offset.x = 0
        end 
    else 
        if cur_page_offset.x < -(self.list_num - 3) * width and self.list_num - 6 > 0 and cur_page_offset.x >= -(self.list_num - 6) * width then 
            cur_page_offset.x = cur_page_offset.x - width * 3 
        else
            cur_page_offset.x = -(self.list_num - 3) * width
        end 
    end 
    
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self.part_page_content:SetLocalPosition(cur_page_offset)
    self:FlushPageJump()
end


----------------------------SummerMazeShopItem----------------------------
SummerMazeShopItem = SummerMazeShopItem or DeclareMono("SummerMazeShopItem", UIWidgetBaseItem)
function SummerMazeShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.gameObject.name = "sell_item" .. data.item_index
    local item = Item.Create(data.shop_info.reward_item)
    self.item_cell:SetData(item)
    UH.SetText(self.item_name, item:Name())
    UH.SetText(self.price, data.shop_info.price)
    self.is_discount:SetActive(data.shop_info.discount > 0)
    UH.SetText(self.lbl_discount, Format(Language.SummerMaze.Discount, data.shop_info.discount / 10))
    local can_buy_time = data.shop_info.buy_times - data.buy_times
    UH.SetText(self.TxtTimes, Format(Language.SummerMaze.ShopTimes, can_buy_time > 0 and COLORSTR.Green6 or COLORSTR.Red8, can_buy_time))
    UH.SpriteName(self.price_type, HuoBi[data.shop_info.money_type])
    self.mask:SetActive(can_buy_time == 0)  
end

function SummerMazeShopItem:ClickBuy()
    if MallCtrl.IsNotCurrency(self.data.shop_info.money_type == 0 and CurrencyType.Gold or CurrencyType.CoinBind, self.data.shop_info.price) then
        SummerMazeData.Instance:SendInfo(8, self.data.server_index)
    end
end 