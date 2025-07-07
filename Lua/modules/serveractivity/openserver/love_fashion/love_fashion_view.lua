LoveFashionView = LoveFashionView or DeclareMono("LoveFashionView", UIWFlushPanel)

function LoveFashionView:LoveFashionView()
    self.data = LoveFashionData.Instance
    self.data_cares = {
        {data = self.data.love_fashion_data, func = self.OnFlushShowView, init_call = false},
        {data = ServerActivityData.Instance:GetCmdBuyItemCheckRet(),func = self.SendBuy, init_call = false},
        {data = self.data.love_fashion_data, func = self.FlushBuyBtnView,keys = {"is_click_sel"} ,init_call = true},
    }
    self.make_num = 1
end

function LoveFashionView:Awake()
    UIWFlushPanel.Awake(self)
    if not self.data.love_fashion_data.is_open then
        LoveFashionCtrl.Instance:SendReq(4)
    end
    self.Time:CreateActTime(ACTIVITY_TYPE.KF_LOVE_FASHION,TimeType.Type_Special_4) 
end

function LoveFashionView:GetIndex()
    return self.data.love_fashion_data.sel_value
end

function LoveFashionView:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.make_num = self:GetIndex()
    self:OnClickShowModel(self:GetIndex())
    self.data.love_fashion_data.sel_value = self.data:GetConfirmIndex()
    self:OnFlushShowView()
    for i = 1,self.ShowBtnList:Length() do
        self.ShowBtnList[i]:SetObjActive(self.data:GetConfig()[i] ~= nil)
    end
    self:FlushButtonView()
end

function LoveFashionView:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.show_timer)
    TimeHelper:CancelTimer(self.not_handle_runner)
end

function LoveFashionView:OnFlushShowView()  
    self.List:SetDataList(self.data:GetConfig(),self:GetIndex())
end

function LoveFashionView:FlushButtonView()
    self.BtnLeft:SetActive(self.make_num > 1)
    self.BtnRight:SetActive(self.make_num < #self.data:GetConfig())
end

function LoveFashionView:OnClickNum(num)
    self.make_num = self.make_num + num
    self:FlushButtonView()
    self.data:OnClickSel(self.make_num)
end

function LoveFashionView:FlushBuyBtnView()
    local data = self.data:GetConfig()[self:GetIndex()]
    UH.SetText(self.OldPrice,string.format(Language.LoveFashion.ValueTxt,data.value))
    if data.receive_mark == 1 then
        UH.SetText(self.NewPrice, Language.LoveFashion.BuyTxt[data.self_buy_mark + data.receive_mark])
    elseif data.buy_mark then
        UH.SetText(self.NewPrice, Language.LoveFashion.BuyTxt[data.buy_mark and 0 or 1])
    else
        UH.SetText(self.NewPrice,data.price / (data.price_type == 3 and 10 or 1))
    end
    UH.SetGold(self.Icon, data.price_type == 3 and 999 or data.price_type)
    UH.SetText(self.ModDesc,data.item_desc)
    UH.SetText(self.FashionTip,string.format(Language.LoveFashion.FashionTip,Item.GetName(data.reward_item.item_id)))
    self.IconObj:SetActive(data.receive_mark == 0 and data.self_buy_mark == 0 and not data.buy_mark)
    local state = data.receive_mark == 1 and 1 or data.self_buy_mark
    self.BtnInter.Interactable = state < 1
    self.BuyEffect:SetActive(state < 1)
    self:OnClickShowModel(self:GetIndex())
    self.make_num = self:GetIndex()
    self:BindFlushModel()
    self:FlushButtonView()
end

function LoveFashionView:FlushModel()
	self.ModelShow:SetData({item_id = self.now_item})
    UH.SetText(self.ModName,Item.GetName(self.now_item.item_id))
    local attr_tab = self:GetFashionAttr(self.now_item.item_id)
    UH.SetText(self.AttrTxt,string.format(Language.LoveFashion.AttrShwo,attr_tab.attr_name, attr_tab.attr_value))
end

function LoveFashionView:BindFlushModel()
    if self.not_handle_runner ~= nil then
        TimeHelper:CancelTimer(self.not_handle_runner)
    end
    self.not_handle_runner = TimeHelper:AddRunTimer(function ()
        self:AutoFlushModel()
    end,5,nil,false)
end

function LoveFashionView:AutoFlushModel()
    if self.make_num then
        if self.make_num >= #self.data:GetConfig() then
            self.make_num = 0
        end
        self.make_num = self.make_num + 1 
        self:OnClickShowModel(self.make_num,true)
    end
end

function LoveFashionView:GetFashionAttr(item_id)
    local t = {}
    t.attr_name = ""
    t.attr_value = 0
    local item_cfg = Item.GetConfig(item_id)
    if item_cfg.item_type == ItemType.Skin then
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
		local level = AppearanceData.Instance:GetAppearancInfo().face_level + 1
		local add_data = AppearanceData.Instance:GetCurAddData(level)
        for k,v in pairs(add_data.add_list) do
            t.attr_name = v.attr_name
            t.attr_value = v.attr_value
		end
	elseif item_cfg.item_type == ItemType.Fashion then
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

function LoveFashionView:OnClickShowModel(value,is_auto)
    if not is_auto then
        self:BindFlushModel()
    else
        self.make_num = value
    end
    for i = 1,self.ShowBtnList:Length() do
        self.ShowBtnList[i].isOn = i == value
    end
    self.List:SetSetectItem(value)
    if self.data:GetConfig()[value] then
        self.now_item = self.data:GetConfig()[value].reward_item
        self:FlushModel()
    end
end

function LoveFashionView:OnBuyClick()
    if MarryData.Instance:GetHusbandOrWifeUid() == 0 then
        PublicPopupCtrl.Instance:Center(Language.LoveFashion.NotMarry)
        self.show_timer = TimeHelper:AddDelayTimer(function()
            PublicPopupCtrl.Instance:DialogTips{
                content = Language.LoveFashion.NotMarryTip,
                confirm = {
                    func = function()
                        ViewMgr:CloseView(DialogTipsView)
                        ViewMgr:CloseView(ServerActivityView)  
                        ViewMgr:OpenViewByKey(Mod.Society.Marry)                               
                    end
                },
            }
        end, 0.5)
        return
    end
    local data = self.data:GetConfig()[self:GetIndex()]
    if data.buy_mark and data.receive_mark == 0 then --请求领取
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_LOVE_FASHION,3, data.seq)
        return
    end
    if data.price_type == 3 then
        local fee = data.price / 10
        self.price = data.price
        ServerActivityCtrl.Instance:SendBuyCmd(BUY_ITEM_TYPE.TYPE_LOVE_FASHION, fee, ACTIVITY_TYPE.KF_LOVE_FASHION, 2, data.seq, 0, 0)
    elseif RoleData.Instance:GetCurrency(data.price_type) < data.price then
        local param_t = {
            config = StoreData.Instance:GetItemConfig(data.price_type == CurrencyType.ImmortalCoin and CommonItemId.ImmortalCoinId or 22014, data.price_type == CurrencyType.ImmortalCoin and 5 or 0),
            comfirm_func = function(t)
                ShoppingCtrl.Instance:SendStoreBuyReq(t)
                ViewMgr:CloseView(MoneyExchangeView)
            end
        }
        param_t.money_type = data.price_type
        ViewMgr:OpenView(MoneyExchangeView,param_t)
    else
        ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.KF_LOVE_FASHION, 1, data.seq)
    end
end

function LoveFashionView:SendBuy()
    ServerActivityCtrl.Instance:MoneyBuy(BUY_ITEM_TYPE.TYPE_LOVE_FASHION, self.price)
end

-------------------LoveFashionItem-------------------
LoveFashionItem = LoveFashionItem or DeclareMono("LoveFashionItem", UIWidgetBaseItem)
function LoveFashionItem:SetData(data)
    local item = Item.New(data.reward_item)
    self.Call:SetData(item)
    UH.SetText(self.TxtName, item:Name())
    UIWidgetBaseItem.SetData(self, data)
end
function LoveFashionItem:OnClick()
    LoveFashionData.Instance:OnClickSel(self.data.seq + 1)
end

function LoveFashionItem:Click()
    if self.Toggle and self.Toggle.isActiveAndEnabled then 
        self.Toggle:Trigger()
    end
end