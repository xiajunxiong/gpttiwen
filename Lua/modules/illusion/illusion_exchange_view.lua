
IllusionExchangeView = IllusionExchangeView or DeclareView("IllusionExchangeView", IllusionConfig.ResPath .. "illusion_exchange", Mod.IllusionExchange.Main)

function IllusionExchangeView:IllusionExchangeView()
	self.data = IllusionCtrl.Instance:Data()
	self.language = Language.Illusion
end

function IllusionExchangeView:LoadCallback()
	-- self.Board:SetData(self:GetType(), self.language.IllusionExchange, Vector2.New(1032,576))
	self.Board:SetData(self:GetType(), "", Vector2.New(1032,576))
end

IllusionExchangeViewOffset = IllusionExchangeViewOffset or DeclareMono("IllusionExchangeViewOffset", UIWFlushPanel)

function IllusionExchangeViewOffset:IllusionExchangeViewOffset()
	self.data = IllusionCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data.cur_exchange_info, func = self.FlushShow, keys = {"type"}},
		{data = BagData.Instance.item_grid, func = self.FlushShow, init_call = false}
	}
end

function IllusionExchangeViewOffset:Awake()
	UIWFlushPanel.Awake(self)

	local fashion_type_list = self.data:GetFashionTypeList()
	self.TypeList:SetDataList(fashion_type_list)
end

function IllusionExchangeViewOffset:FlushShow()
	local fashion_type = self.data.cur_exchange_info.type
	local exchange_shop_list = self.data:GetExchangeShopList(fashion_type)
	local consume_item = exchange_shop_list[1].consume_item
	self.consume_item_data = Item.Init(consume_item)
	self.ConsumeName.text = Item.GetName(consume_item) .. "："
	UH.SetIcon(self.ConsumeIcon, Item.GetIconId(consume_item))
	self.HaveNum.text = Item.GetNum(consume_item)

	local show_list2 = IllusionConfig.FashionType.partner == fashion_type or IllusionConfig.FashionType.pet == fashion_type
	self.ShowList:SetObjActive(not show_list2)
	self.ShowList2:SetObjActive(show_list2)
	if show_list2 then
		self.ShowList2:SetData(exchange_shop_list)
	else
		self.ShowList:SetData(exchange_shop_list)
	end
end

function IllusionExchangeViewOffset:OnClickAttrList(data)
	local attr_list = self.data:GetAttrList(data.id)
	self.AttrList:SetData(attr_list)
	self.AttrListObj:SetActive(true)
end

function IllusionExchangeViewOffset:OnClickCloseAttr()
	self.AttrListObj:SetActive(false)
end

function IllusionExchangeViewOffset:OnClickItem()
	local info = {
		item_list = {self.consume_item_data},
		-- pos = Vector3.New(0, 140, 0)
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

IllusionExchangeViewTypeItem = DeclareMono("IllusionExchangeViewTypeItem", UIWidgetBaseItem)

function IllusionExchangeViewTypeItem:IllusionExchangeViewTypeItem()
	self.illusion_data = IllusionCtrl.Instance:Data()
end

function IllusionExchangeViewTypeItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.TypeName.text = Language.Illusion.FashionTypeName[data.type]
end

function IllusionExchangeViewTypeItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function IllusionExchangeViewTypeItem:OnClickType()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    self.illusion_data.cur_exchange_info.type = self.data.type
end


IllusionExchangeViewShowItem = DeclareMono("IllusionExchangeViewShowItem", UIWidgetBaseItem)

function IllusionExchangeViewShowItem:IllusionExchangeViewShowItem()
	self.illusion_data = IllusionCtrl.Instance:Data()
	self.language = Language.Illusion.Exchange
end

function IllusionExchangeViewShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	local surface = data.surface
	self.Name.text = surface.surface_name
	UH.SetIcon(self.ConsumeIcon, Item.GetIconId(data.consume_item))
	self.ConsumeNum.text = data.consume_num
	self.BtnExchangeInter.Interactable = true--data.fce
	
	self.TimeObj:SetActive(data.is_time_limit)
	local last_time = data.limit_time - TimeHelper.GetServerTime()
	if data.is_time_limit and last_time > 0 then
		local t = TimeManager.FormatDHMS(last_time)
		self.LimitTime.text = string.format(self.language.TimeFormat, t.day, t.hour, t.min)
	end

	local png_path = self.illusion_data:GetShowPngPath{id = data.id, type = data.type, avatar_type = RoleData.Instance:GetRoleAvatarType(), prof_type = RoleData.Instance:ProfType()}
	if nil == png_path then return end
	self.ShowImageSetter:SetByPath(png_path)
end

function IllusionExchangeViewShowItem:OnClickExchange()
	if self.BtnExchangeInter.Interactable then
		IllusionCtrl.Instance:SendSurfaceExchange(self.data.seq)
	else
		PublicPopupCtrl.Instance:Center(self.language.HaveTips)
	end
end

function IllusionExchangeViewShowItem:OnClickShow()
	local info = {
		item_list = {Item.Init(self.data.surface.active_stuff_id)},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end

IllusionExchangeViewShowItem2 = DeclareMono("IllusionExchangeViewShowItem2", UIWidgetBaseItem)

function IllusionExchangeViewShowItem2:IllusionExchangeViewShowItem2()
	self.illusion_data = IllusionCtrl.Instance:Data()
	self.language = Language.Illusion.Exchange
end

function IllusionExchangeViewShowItem2:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	local surface = data.surface
	self.Name.text = surface.name
	self.NameSign.text = data.name_sign
	UH.SetIcon(self.ConsumeIcon, Item.GetIconId(data.consume_item))
	self.ConsumeNum.text = data.consume_num
	self.BtnExchangeInter.Interactable = true--data.fce
	
	self.TimeObj:SetActive(data.is_time_limit)
	local last_time = data.limit_time - TimeHelper.GetServerTime()
	if data.is_time_limit and last_time > 0 then
		local t = TimeManager.FormatDHMS(last_time)
		self.LimitTime.text = string.format(self.language.TimeFormat, t.day, t.hour, t.min)
	end

	local png_path
	if IllusionConfig.FashionType.partner == data.type then
		png_path = string.format(PartnerConfig.PngPath, surface.partner_skin_id)
	elseif IllusionConfig.FashionType.pet == data.type then
		png_path = string.format("pet_skin/%s.png", surface.res_id)
	end
	if nil == png_path then return end
	self.ShowImageSetter:SetByPath(png_path)
end

function IllusionExchangeViewShowItem2:OnClickExchange()
	if self.BtnExchangeInter.Interactable then
		IllusionCtrl.Instance:SendSurfaceExchange(self.data.seq)
	else
		PublicPopupCtrl.Instance:Center(self.language.HaveTips)
	end
end

function IllusionExchangeViewShowItem2:OnClickShow()
		local info = {
		item_list = {Item.Init(self.data.surface.active_stuff_id)},
	}
	ItemInfoCtrl.Instance:ItemInfoView(info)
end