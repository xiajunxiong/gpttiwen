PropItemUseDiaLog = PropItemUseDiaLog or DeclareView("PropItemUseDiaLog", "live/prop_item_use_dialog")
function PropItemUseDiaLog:LoadCallback(param_t)
    
end

PropItemUseDiaLogPanel = PropItemUseDiaLogPanel or DeclareMono("PropItemUseDiaLogPanel", UIWFlushPanel)
function PropItemUseDiaLogPanel:PropItemUseDiaLogPanel()
    self.data_cares = {
		{data = BagData.Instance.item_grid, func = self.FlushPanel, init_call = false},
	}

	self.cur_num = 1
	self.all_num = 0
end

function PropItemUseDiaLogPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.item_id = LiveData.Instance:GetPropUseInfo().item_id
    if not self.item_id then
        ViewMgr:CloseView(PropItemUseDiaLog)
        return
    end
    local item = Item.Create({item_id = self.item_id})
    self.item = item
    self.ShowCell:SetData(item)
    UH.SetIcon(self.ItemIcon, item:IconId())
    local val_str = ""
    if item:ShowType() == ShowType.LiveCard then
        local cfg = LiveData.Instance:GetAddCfgByItemId(self.item_id)
        self.prop_cfg = cfg
        val_str = LiveData.Instance:GetCardPoint(cfg.seq)
    end
    UH.SetText(self.InitValue, val_str)

    self:FlushPanel()
end

function PropItemUseDiaLogPanel:FlushPanel()
    UH.SetText(self.NumVal, self.cur_num)
    local desc1
    local desc2
    local item_num = Item.GetNum(self.item.item_id)
    self.all_num = item_num
    if self.item:ShowType() == ShowType.LiveCard then
        local add_point = self.prop_cfg.increase_point
        desc1 = string.format(Language.LiveCard.UseDesc1,self.cur_num,self.item:Name(),self.cur_num * add_point)
        desc2 = string.format(Language.LiveCard.UseDesc2,self.item:Name(),item_num)
        UH.SetText(self.Desc1, desc1)
        UH.SetText(self.Desc2, desc2)
    end
end

function PropItemUseDiaLogPanel:OnClickAdd()
	if self.cur_num < self.all_num then
		self.cur_num = self.cur_num + 1
		self:FlushPanel()
	else
		PublicPopupCtrl.Instance:Center(Language.LiveCard.Tip1)
	end
end

function PropItemUseDiaLogPanel:OnClickSub()
	if 1 < self.cur_num then
		self.cur_num = self.cur_num - 1
		self:FlushPanel()
	else
		PublicPopupCtrl.Instance:Center(Language.LiveCard.Tip2)
	end
end

function PropItemUseDiaLogPanel:OnClickMax()
    local max_num = self.all_num > 99 and 99 or self.all_num
	if self.cur_num < max_num then
		self.cur_num = max_num
		self:FlushPanel()
	else
		PublicPopupCtrl.Instance:Center(Language.LiveCard.Tip1)
	end
end

function PropItemUseDiaLogPanel:OnClickClose()
	ViewMgr:CloseView(PropItemUseDiaLog)
end

function PropItemUseDiaLogPanel:OnClickConfirm()
    if self.all_num < 1 then
        return
    end

    local index = BagData.Instance:GetBagIndexById(ItemColumnType.Item, self.item.item_id)
    if index then
        local item_param_t = {item_id = self.item.item_id,column = ItemColumnType.Item,index = index - 1,num = self.cur_num}
        BagCtrl.Instance:SendUseItem(item_param_t)
        self:OnClickClose()
    end
end

function PropItemUseDiaLogPanel:OnClickCancel()
	self:OnClickClose()
end
