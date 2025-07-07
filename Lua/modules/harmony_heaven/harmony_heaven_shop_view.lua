
HarmonyHeavenShopView = HarmonyHeavenShopView or DeclareView("HarmonyHeavenShopView", HarmonyHeavenConfig.ResPath .. "harmony_heaven_shop", Mod.HarmonyHeaven.Shop)

VIEW_DECLARE_LEVEL(HarmonyHeavenShopView,ViewLevel.L1)

function HarmonyHeavenShopView:HarmonyHeavenShopView()
	self.data = HarmonyHeavenCtrl.Instance:Data()
end

function HarmonyHeavenShopView:LoadCallback()
end

function HarmonyHeavenShopView:OnClickClose()
    ViewMgr:CloseView(HarmonyHeavenShopView)
end

HarmonyHeavenShopViewF = HarmonyHeavenShopViewF or DeclareMono("HarmonyHeavenShopViewF", UIWFlushPanel)

function HarmonyHeavenShopViewF:HarmonyHeavenShopViewF()
	self.data = HarmonyHeavenCtrl.Instance:Data()
	self.language = Language.HarmonyHeaven.Shop

	self.data_cares = {
		{data = self.data.fb_info.role_info, func = self.FlushShow},
		{data = BagData.Instance.item_grid, func = self.FlushConver},
	}
end

function HarmonyHeavenShopViewF:FlushShow()
	local show_list = self.data:GetFbShopShowList()
	self.ShowList:SetData(show_list)
end

function HarmonyHeavenShopViewF:FlushConver()
	local show_list = self.data:GetFbShopShowList()
	if show_list[1] then
		UH.SetIcon(self.ConverIcon, Item.GetIconId(show_list[1].conver_item_id))
		self.ConverNum.text = BagData.Instance:GetNumByItemId(show_list[1].conver_item_id)
	end
end

function HarmonyHeavenShopViewF:OnClickConver()
	local show_list = self.data:GetFbShopShowList()
	if show_list[1] then
		local info = {
			item_list = {Item.Init(show_list[1].conver_item_id)},
			pos = Vector3.New(0, 140, 0)
		}
		ItemInfoCtrl.Instance:ItemInfoView(info)
	end
end


HarmonyHeavenShopViewItem = DeclareMono("HarmonyHeavenShopViewItem", UIWidgetBaseItem)

function HarmonyHeavenShopViewItem:HarmonyHeavenShopViewItem()
	self.hh_data = HarmonyHeavenCtrl.Instance:Data()
	self.language = Language.HarmonyHeaven.Shop
end

function HarmonyHeavenShopViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	
	local item = Item.Init(data.item_id, data.num, data.is_bind)
	self.Cell:SetData(item)
	self.NameShow.text = item:Name()
	self.PriceShow.text = data.price
	UH.SetIcon(self.ConverIcon, Item.GetIconId(data.conver_item_id))
	self.LimitShow.text = data.limit_seq > -1 and string.format(self.language.LimitShow, data.limit_convert_count - self.hh_data:GetFbShopBuyNum(data.limit_seq), data.limit_convert_count) or ""
end

function HarmonyHeavenShopViewItem:OnClickBuy()
	if BagData.Instance:GetNumByItemId(self.data.conver_item_id) < self.data.price then
		MainOtherCtrl.Instance:GetWayView({item = Item.Create({item_id =self.data.conver_item_id})})
		return
	end
	HarmonyHeavenCtrl.Instance:SendHongMengTianJieReqShop(self.data.seq, 1)
end