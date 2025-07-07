GIFT_OPEN_TYPE = {
    CONGRATULATIONS = 1,--恭喜获得 只有Top 多个物品
    SPECIAL = 2,        --恭喜获得 Top + Bottom 多个物品
    SURPRISE = 3,       --意外获得 Top + Bottom 单个物品 传一个item，可传多个，如果传多个就不会显示出物品描述
    FAILBG = 4,         --失败获得 同1
}
GiftOpenView = GiftOpenView or DeclareView("GiftOpenView", "main_other/gift_open")

VIEW_DECLARE_MASK(GiftOpenView,ViewMask.None)
VIEW_DECLARE_LEVEL(GiftOpenView, ViewLevel.Lt)

function GiftOpenView:LoadCallback(param_t)
    self.param_t = param_t
    if param_t.gift_type == nil then
        self:FlushGiftOpenView(param_t)
    else
        local item_list = param_t.item and {param_t.item} or param_t.item_list
        self.ItemList[param_t.gift_type]:SetData(item_list)
        self.ItemList[param_t.gift_type].gameObject:SetActive(true)
        self:FlushPopupView(item_list)
    end
    if param_t.title_name ~= nil then
        self.ItemList[param_t.gift_type]:SetTitle(param_t.title_name)
    end
end

function GiftOpenView:OpenCallback()
    self.BtnUse:SetActive(MainOtherData.Instance:QuickUseItem() ~= nil)
    TimeHelper:CancelTimer(self.timer_dt_ac)
    if self.param_t.auto_close then
        self.timer_dt_ac = TimeHelper:AddDelayTimer(BindTool.Bind(self.OnClickBlock, self), self.param_t.auto_close)
    end
end

--根据配置展示对应的动画和界面
function GiftOpenView:FlushGiftOpenView(param_t)
    if param_t.item == nil then
        local item_list,config = BagData.Instance:GiftItemList(param_t.item_id)
        local index = self:GetItemIndex(config) or 1
        self.ItemList[index]:SetData(param_t.item_list or item_list)
        self.ItemList[index].gameObject:SetActive(true)
        self:FlushPopupView(param_t.item_list or item_list)
    else
        self.ItemList[GIFT_OPEN_TYPE.SURPRISE]:SetData({param_t.item})
        self.ItemList[GIFT_OPEN_TYPE.SURPRISE].gameObject:SetActive(true)
        self:FlushPopupView({param_t.item})
    end
end

function GiftOpenView:GetItemIndex(config)
    if config.is_special and config.is_special == 1 then
        return GIFT_OPEN_TYPE.SPECIAL
    end
    return GIFT_OPEN_TYPE.CONGRATULATIONS
end

--物品飘字
function GiftOpenView:FlushPopupView(item_list)
    self.ItemTimer = TimeHelper:AddDelayTimer(function()
        if not self.param_t.is_not_show then
            PublicPopupCtrl.Instance:ShowCustom(item_list)
        end
        self.ItemTimer = nil
	end, 1)
end

function GiftOpenView:CloseCallback()
    TimeHelper:CancelTimer(self.ItemTimer)
    TimeHelper:CancelTimer(self.ShowTimer)
    TimeHelper:CancelTimer(self.timer_dt_ac)
    MainOtherData.Instance.quick_use_item = nil
    if self.param_t.close_func then
        Call(self.param_t.close_func)
    end
end

function GiftOpenView:IsHasClose()
    local gift_type = self.param_t.gift_type
    if gift_type and self.param_t.is_close_anim then
        self.AnimList[gift_type]:Play("gift_close")
        return true
    end
end

function GiftOpenView:OnClickBlock()
    if self.ItemTimer then
        return
    end
    self.ShowTimer = Invoke(function()
        ViewMgr:CloseView(GiftOpenView)
    end,self:IsHasClose() and 1 or 0)
end

function GiftOpenView:OnClickUse()
    local item = MainOtherData.Instance:QuickUseItem()
    if item:Type() == ItemType.Gift then
        BagCtrl.Instance:SendUseGiftReq({item_id = item.item_id,column = item.column_type,index = item.index,num = item.num})
    else
        BagCtrl.Instance:UseItemByIndex(item.column_type or 0,item.index or 0)
    end
    ViewMgr:CloseView(GiftOpenView)
end

----------------------------GiftOpenItem----------------------------
GiftOpenItem = GiftOpenItem or DeclareMono("GiftOpenItem", UIWidgetBaseItem)

function GiftOpenItem:SetData(data)
    self.Grid:SetData(data)
    if self.scroll ~= nil then 
        self.scroll.enabled = #data > 11
    end 
end

function GiftOpenItem:SetTitle(name)
    UH.SpriteName(self.Title,name)
end

----------------------------GiftOpenCall----------------------------
GiftOpenCall = GiftOpenCall or DeclareMono("GiftOpenCall", UIWidgetBaseItem)

--data.change_item======Item  需要显示分解或者转换表现时赋值
--data.tag_str==============String 需要显示左上角tag时赋值，同样适用于change_item
function GiftOpenCall:GiftOpenCall()
    self.handle_delay = {}
end
function GiftOpenCall:SetData(data)
    self.Call:SetData(data)
    UH.SetText(self.Name,data:QuaName())
    if self.Tag ~= nil then
        self.Tag:SetActive(not StringIsEmpty(data.tag_str))
        UH.SetText(self.TagName,data.tag_str)
    end
    if data.change_item then
        self.handle_delay["delay_effect"] = TimeHelper:AddDelayTimer(function ()
            self.EffectTool:Play(2165023, self.Call.gameObject)    
            AudioMgr:PlayEffect(AudioType.UI,CommonAudio.OpenGift)
        end,0.5)
       self.handle_delay["delay_change"] = TimeHelper:AddDelayTimer(function ()
            self.Call:SetData(data.change_item)
            if self.Tag ~= nil then
                self.Tag:SetActive(not StringIsEmpty(data.change_item.tag_str))
                UH.SetText(self.TagName,data.change_item.tag_str)
            end
            UH.SetText(self.Name, data.change_item:QuaName())
        end, 0.85 + 0.5)
    end
end

function GiftOpenCall:OnDestroy()
    for key, value in pairs(self.handle_delay) do
        TimeHelper:CancelTimer(value)
    end
end

----------------------------GiftOpenSurpriseItem----------------------------
GiftOpenSurpriseItem = GiftOpenSurpriseItem or DeclareMono("GiftOpenSurpriseItem", UIWidgetBaseItem)

function GiftOpenSurpriseItem:SetData(data)
    local is_one = table.nums(data) == 1
    if is_one then
        local item = data[1] or {}
        self.Call:SetData(item)
        UH.SetText(self.Name,item:QuaName())
        UH.SetText(self.Desc,item:Desc())
    else
        self.List:SetData(data)
    end
    self.List:SetObjActive(not is_one)
end

function GiftOpenSurpriseItem:SetTitle(name)
    UH.SpriteName(self.Title,name)
end