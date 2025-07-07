ActFashionShopView = ActFashionShopView or DeclareMono("ActFashionShopView", UIWFlushPanel)

function ActFashionShopView:ActFashionShopView()
    self.data = ServerActivityData.Instance
    self.data_cares = {
        {data = self.data.fashion_shop_data, func = self.FlushShopView, keys = {"buy_times"}, init_call = false},
        {data = self.data.fashion_shop_data, func = self.FlushLeft, keys = {"now_select"}, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(), func = self.SendBuy, init_call = false},
    }
end

function ActFashionShopView:Awake()
    UIWFlushPanel.Awake(self)
    self.Time:CreateActTime(ACTIVITY_TYPE.RAND_FASHION_SHOP, TimeType.Type_Special_4)
end

function ActFashionShopView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.List:SetData({})
    self:FlushShopView()
end

function ActFashionShopView:FlushShopView()
    self.config = self.data:GetFashionShopConfig()
    self.List:SetData(self.config)
    self.List:SetectItem(1, true)
    self.data.fashion_shop_data.now_select = self.config[1].seq
    self:FlushLeft()
end

function ActFashionShopView:FlushLeft()
    self.now_select_id, self.now_select_data = DataHelper.SimpleIndex(self.config, "seq", self.data.fashion_shop_data.now_select)
    self:FlushDesc()
    self.ModelUi:SetData({item_id = self.now_select_data.item})
    UH.SetText(self.TxtName, Item.GetName(self.now_select_data.item.item_id))
    UH.SpriteName(self.ImgSetType, "type_" .. self.now_select_data.text)

    self.IHBuy.Interactable = self.now_select_data.total_exchange > self.data:GetFashionShopBuyTimes(self.now_select_data.seq)
    local color = self.IHBuy.Interactable and COLORSTR.Green2 or COLORSTR.Red7
    UH.SetText(self.TxtLimit, Format(Language.ActFactionShop.Limit, color, self.data:GetFashionShopBuyTimes(self.now_select_data.seq), self.now_select_data.total_exchange))

    self.is_money_buy = self.now_select_data.buy_money > 0
    UH.SetText(self.TxtOldPrice, Format(Language.ActFactionShop.OldPrice, self.is_money_buy and self.now_select_data.old_gold / 10 or self.now_select_data.old_gold))
    UH.SetText(self.TxtNowPrice, self.is_money_buy and self.now_select_data.buy_money / 10 or self.now_select_data.need_gold)
    UH.SpriteName(self.ImgSetPrice, self.is_money_buy and CommonItemId.RenMinBi or CommonItemId.Gold)
    local attr_tab = self:GetFashionAttr(self.now_select_data.item.item_id)
    UH.SetText(self.TxtAttr, Format(Language.FashionShop.Attr, attr_tab.attr_name, attr_tab.attr_value))
end

function ActFashionShopView:FlushDesc()
    local _, count = string.gsub(self.now_select_data.desc, "\n", "\n")
    for i = 1, self.Lines:Length() do
        self.Lines[i]:SetActive(count > i)
    end
    UH.SetText(self.TxtDesc, self.now_select_data.desc)
end

function ActFashionShopView:OnBuyClick()
    if self.is_money_buy then
        local fee = self.now_select_data.buy_money / 10
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_FASHION_SHOP, fee, ACTIVITY_TYPE.RAND_FASHION_SHOP, 2, self.now_select_data.seq, 0, 0)
    else
        if MallCtrl.IsNotCurrency(CurrencyType.Gold, self.now_select_data.need_gold) then
            ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_FASHION_SHOP, 1, self.now_select_data.seq)
        end
    end
end

function ActFashionShopView:SendBuy()
    if self.now_select_data == nil then
        return
    end
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_FASHION_SHOP, self.now_select_data.buy_money)
end

function ActFashionShopView:GetFashionAttr(item_id)
    local t = {}
    t.attr_name = ""
    t.attr_value = 0
    local item_cfg = Item.GetConfig(item_id)
    if item_cfg.item_type == ItemType.Skin then
		--获取对应皮肤的属性加成
		if item_cfg.show_type == ShowType.PetSkin then  --宠物皮肤
			local pet_skin_cfg = Cfg.PetSkinAttr(item_cfg.param1)
			if pet_skin_cfg then
				for k,v in pairs(PetData.PetGrowthKey) do
                    if pet_skin_cfg[k] ~= 0 then
                        t.attr_name = Language.Pet.AttrClass[v][4]
                        t.attr_value = pet_skin_cfg[k]
					end
				end
                if pet_skin_cfg.potential and pet_skin_cfg.potential ~= 0 then
                    t.attr_name = Language.Pet.Potential
                    t.attr_value = pet_skin_cfg.potential
				end
			end
		elseif item_cfg.show_type == ShowType.PartnerSkin then  --伙伴皮肤
			local partner_skin_cfg = PartnerData.Instance.partner_skin_ids[item_cfg.param2]
			if partner_skin_cfg then
				for k,v in pairs(AttrNameForType) do
                    if partner_skin_cfg[v] ~= nil and partner_skin_cfg[v] ~= 0 then
                        t.attr_name = Language.Common.AttrList[k]
                        t.attr_value = DataHelper.Percent(k, partner_skin_cfg[v])
					end
			    end
			end
		end
	elseif item_cfg.item_type == ItemType.Appearance then --颜值物品（气泡框、尾标、称号）
		--获取颜值下一等级的属性加成
		local level = AppearanceData.Instance:GetAppearancInfo().face_level + 1
		local add_data = AppearanceData.Instance:GetCurAddData(level)
        for k,v in pairs(add_data.add_list) do
            t.attr_name = v.attr_name
            t.attr_value = v.attr_value
		end
	elseif item_cfg.item_type == ItemType.Fashion then
		--时装属性专精特效
	    local surface_cfg = Cfg.SurfaceValue(item_cfg.param1, RoleData.Instance:GetRoleProf())
		for k,v in pairs(AttrNameForType) do
            if surface_cfg[v] ~= nil and surface_cfg[v] ~= 0 then
                t.attr_name = Language.Common.AttrList[k]
                t.attr_value = surface_cfg[v]
			end
	    end
    end
    return t
end


----------------------------ActFashionShopItem----------------------------
ActFashionShopItem = ActFashionShopItem or DeclareMono("ActFashionShopItem", UIWidgetBaseItem)

function ActFashionShopItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.IHItem.Interactable = data.total_exchange > ServerActivityData.Instance:GetFashionShopBuyTimes(data.seq)
    self:FlushItem()
end

function ActFashionShopItem:FlushItem()
    local item_data = Item.New(self.data.item)
    item_data.is_grey = not self.IHItem.Interactable
    self.Call:SetData(item_data)
    UH.SetText(self.TxtName, item_data:QuaName(true))
end

function ActFashionShopItem:OnClick()
    self:SetSelected(true)
    ServerActivityData.Instance.fashion_shop_data.now_select = self.data.seq
end