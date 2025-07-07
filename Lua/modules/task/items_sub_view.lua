ItemsSubView = ItemsSubView or DeclareView("ItemsSubView", TaskConfig.ResPath .. "items_sub", Mod.ItemsSub.Main)

function ItemsSubView:ItemsSubView()
    self.data = ItemsSubData.Instance
end

function ItemsSubView:OnClickClose()
    BehaviorData.Instance.auto_behavior = 0
    ViewMgr:CloseView(ItemsSubView)
end

function ItemsSubView:CloseCallback()
    if self.data.later_talk > 0 and BehaviorData.Instance.auto_behavior > 0 then
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.data.later_talk},nil,true)
    end
    self.data.later_talk = 0
end

ItemsSubViewDetail = ItemsSubViewDetail or DeclareMono("ItemsSubViewDetail", UIWFlushPanel)

function ItemsSubViewDetail:ItemsSubViewDetail()
    self.data = ItemsSubData.Instance
    self.beh_data = BehaviorData.Instance
    
    self.data_cares = {
		{data = self.data.sub_items, func = self.FlushItemList},
		{data = BagData.Instance.item_grid, func = self.FlushItemList, init_call = false},
    }
    self.is_subing = false
end

function ItemsSubViewDetail:Awake()
    UIWFlushPanel.Awake(self)
    if self.beh_data.auto_behavior > 0 then
        self.timer_auto_beh = TimeHelper:AddDelayTimer(function ()
            if self.beh_data.auto_behavior > 0 then
                self:OnClickSub()
            end
        end,
        self.beh_data.auto_behavior)
    end
end

function ItemsSubViewDetail:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_auto_beh)
end

function ItemsSubViewDetail:FlushItemList()
    local sub_items = self.data.sub_items:Val()
    for _, sub_item in pairs(sub_items) do
        local have_num = BagData.Instance:GetNumByItemId(sub_item.item_id)
        sub_item.num = string.format("%s/%s", RichTextHelper.ColorStr(have_num, have_num < sub_item.sub_num and COLORSTR.Red8 or COLORSTR.White), sub_item.sub_num)
    end
    self.ItemList:SetData(sub_items)
    self.ItemList:SelectAll(true)
    self.NotItemTip:SetActive(#sub_items == 0)
    if 1 == #sub_items then
        self.BtnSub.text = string.format(Language.Task.SubBtn.with_num, sub_items[1].sub_num)
    else
        self.BtnSub.text = string.format(Language.Task.SubBtn.defalut)
    end
end

function ItemsSubViewDetail:OnClickSub()
    if self.is_subing then return end
    self.is_subing = true
    local value = true
    if nil ~= self.data.sub_func then
        value = self.data.sub_func(self.ItemList:GetSelectedData())
    end
    if value then
        TimeHelper:CancelTimer(self.timer_auto_beh)
        ViewMgr:CloseView(ItemsSubView)
        ViewMgr:CloseView(NpcDialogView)
    end
    self.is_subing = false
end