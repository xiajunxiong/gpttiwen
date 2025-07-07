ItemConsumeTipView = ItemConsumeTipView or DeclareView("ItemConsumeTipView","fight/item_consume_tip")

function ItemConsumeTipView:ItemConsumeTipView()
	self.consume_num = 1
	self.content_format = Language.ItemConsumeTip.Content
	self.efficacy_name = Language.ItemConsumeTip.Energy    -- 功效名字 
	self.item = {}
	self.item_cfg = {}
	self.item_has_num = 0
	self.ok_func = BindTool.Bind(self.okFunc,self)
	self.cancel_func = BindTool.Bind(self.cancelFunc,self)
	self.handle = BagData.Instance:Care(BindTool.Bind(self.FlushPanel, self))
end

-- paramt : {item:Item,content:string,efficacy_name:string,num:number,item_id:number,ok_func:function,cancel_func:function}
function ItemConsumeTipView:LoadCallback(param_t)
	if param_t == nil or next(param_t) == nil or (param_t.item_id == nil and param_t.item == nil) then
		return
	end
	if param_t.ok_func then
		self.ok_func = param_t.ok_func
	end
	if param_t.cancel_func then
		self.cancel_func = param_t.cancel_func
	end
	self.item = param_t.item or Item.Create({item_id = param_t.item_id})
	self.item_cfg = self.item:Config()
	self.consume_num = param_t.num or self.consume_num
	self.content_format = param_t.content or self.content_format
	self.efficacy_name = param_t.efficacy_name or self.efficacy_name
	self.ItemCell:SetData(self.item)
	self:FlushConsumeNum()
	self:FlushPanel()
end

function ItemConsumeTipView:FlushPanel()
	local efficacy_num = self.consume_num * self.item_cfg.param1
	self.item_has_num = self.item:Num()
	UH.SetText(self.ItemNameAndCount,string.format(Language.ItemConsumeTip.ItemCount,self.item:Name(),self.item_has_num))
	UH.SetText(self.Content,string.format(self.content_format,self.consume_num,self.item:Name(),efficacy_num,self.efficacy_name))
end

function ItemConsumeTipView:FlushConsumeNum()
	UH.SetText(self.ConsumeNum,tostring(self.consume_num))
end

function ItemConsumeTipView:CloseCallback()
	BagData.Instance:UnCare(self.handle)
	TimeHelper:CancelTimer(self.add_ht)
	TimeHelper:CancelTimer(self.minus_ht)
end

function ItemConsumeTipView:okFunc()
	if self.consume_num > 0 then
		if self.consume_num <= self.item_has_num then
			local param_t = {
				column = ItemColumnType.Item,
				index = BagData.Instance:GetBagIndexById(ItemColumnType.Item, self.item.item_id) - 1,
				num = self.consume_num,
			}
			BagCtrl.Instance:SendUseItem(param_t)
			ViewMgr:CloseView(ItemConsumeTipView)
		else
			MainOtherCtrl.Instance:GetWayView({item = self.item})
			if self.item_has_num <= 0 then
				ViewMgr:CloseView(ItemConsumeTipView)
			end
		end
	end
end

function ItemConsumeTipView:cancelFunc()
	ViewMgr:CloseView(ItemConsumeTipView)
end

function ItemConsumeTipView:OnCancelClick()
	self.cancel_func()
end

function ItemConsumeTipView:OnOkClick()
	self.ok_func()
end

function ItemConsumeTipView:OnAddClick()
	self.consume_num = self.consume_num + 1
	-- if self.consume_num > self.item_has_num then
	-- 	self.consume_num = self.item_has_num
	-- 	TimeHelper:CancelTimer(self.add_ht)
	-- 	PublicPopupCtrl.Instance:Center(Language.ItemConsumeTip.MaxItemNum)
	-- end
	self:FlushConsumeNum()
end

function ItemConsumeTipView:OnAddDown()
	TimeHelper:CancelTimer(self.add_ht)
	self.add_ht = TimeHelper:AddRunTimer(BindTool.Bind(self.OnAddClick,self),0.2)
end

function ItemConsumeTipView:OnAddUp()
	TimeHelper:CancelTimer(self.add_ht)
	self:FlushPanel()
end

function ItemConsumeTipView:OnMinusClick()
	self.consume_num = self.consume_num - 1
	if self.consume_num < 0 then
		self.consume_num = 0
		TimeHelper:CancelTimer(self.minus_ht)
		PublicPopupCtrl.Instance:Center(Language.ItemConsumeTip.MinusTip)
	end
	self:FlushConsumeNum()
end

function ItemConsumeTipView:OnMinusDown()
	TimeHelper:CancelTimer(self.minus_ht)
	self.minus_ht =TimeHelper:AddRunTimer(BindTool.Bind(self.OnMinusClick,self),0.2)
end

function ItemConsumeTipView:OnMinusUp()
	TimeHelper:CancelTimer(self.minus_ht)
	self:FlushPanel()
end