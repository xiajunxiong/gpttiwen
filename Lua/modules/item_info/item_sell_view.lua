
ItemSellView = ItemSellView or DeclareView("ItemSellView","item_info/item_sell")

VIEW_DECLARE_MASK(ItemSellView, ViewMask.None)

function ItemSellView:ItemSellView()
	self.data = ItemInfoCtrl.Instance:Data()
	self.info = self.data:ItemInfo()
	self.item = self.info.item_list[1]
	self.price = self.item:Price()
	self.max_num = self.item:Num()
	self.cur_num = 1
	self.language = Language.ItemInfo
	self.tips = self.language.Tips
	self.item_sell_tips = self.language.ItemSellTips

	if (self.item:IsEquip() or self.item:IsJewelry()) and (self.item.HadStone and self.item:HadStone()) then 
		local stone_price = 0
		for k,v in pairs(self.item:Stone()) do 
			if v > 0 then 
				local stone_ = Item.New()
				stone_.item_id = v
				stone_price = stone_:Price() + stone_price
			end 
		end 
		self.price = stone_price + self.price
	end 
end

function ItemSellView:LoadCallback(data)
	self.InfoPosition.localPosition = data.pos + Vector2.New(213,-4)
end

function ItemSellView:OnFlush()
	self.CurNum.text = self.cur_num
	self.PerNum.text = self.price
	self.SumNum.text = self.cur_num * self.price
end

function ItemSellView:OnClickSell()
	local sell_func = function ()
		BagCtrl.Instance:SendKnapsackOperaReq{oper_type = ItemInfoConfig.BagOperType.sell, param_1 = self.item.column_type, param_2 = self.item.index, param_3 = self.cur_num}
		ItemInfoCtrl.Instance:ItemDeriveView()
		ViewMgr:CloseView(ItemInfoView)
	end
	local value, tips = self:CheckTips()
	if value then
		PublicPopupCtrl.Instance:DialogTips{
			content = tips,
			confirm = {
				func = function ()
					sell_func()
					ViewMgr:CloseView(DialogTipsView)
				end
			}
		}
	else
		sell_func()
	end
end

function ItemSellView:CheckTips()
	local value, tips = false
	if( self.item:IsEquip() or self.item:IsJewelry())and self.item:HadStone() then
		value = true
		tips = self.item_sell_tips[1]
	end
	return value, tips
end

function ItemSellView:OnClickAdd()
	if self.max_num > self.cur_num then
		self.cur_num = self.cur_num + 1
		ViewMgr:FlushView(ItemSellView)
	else
		PublicPopupCtrl.Instance:Center(self.tips.MaxNum)
	end
end

function ItemSellView:OnClickSub()
	if 1 < self.cur_num then
		self.cur_num = self.cur_num - 1
		ViewMgr:FlushView(ItemSellView)
	else
		PublicPopupCtrl.Instance:Center(self.tips.MinNum)
	end
end

function ItemSellView:OnClickMax()
	self.cur_num = self.max_num
	ViewMgr:FlushView(ItemSellView)
end
function ItemSellView:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end