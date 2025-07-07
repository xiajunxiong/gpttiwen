
IllusionView = IllusionView or DeclareView("IllusionView", IllusionConfig.ResPath .. "illusion", Mod.Illusion.HuanHua)
VIEW_DECLARE_LEVEL(IllusionView,ViewLevel.L1)

function IllusionView:IllusionView()
	self.data = IllusionCtrl.Instance:Data()
	self.language = Language.Illusion

	self.data:ResetCurModelInfo(true)
end

function IllusionView:LoadCallback()
	self.Currency:DefaultSet()
end

function IllusionView:CloseCallback()
	self.data:SetNeedJumpItemId()
end

function IllusionView:OnClickClose()
	ViewMgr:CloseView(IllusionView)
end

----------------------IllusionViewLeft----------------------
IllusionViewLeft = IllusionViewLeft or DeclareMono("IllusionViewLeft", UIWFlushPanel)

function IllusionViewLeft:IllusionViewLeft()
	self.data = IllusionCtrl.Instance:Data()
	self.rc_data = RoleCreateData.Instance

	self.data_cares = {
		{data = self.data.red_points, func = self.FlushRedPoints},
	}
end

function IllusionViewLeft:Awake()
	self.red_points = {
		[IllusionConfig.FashionType.cloth] = UiRedPoint.New(self.ItemCloth, Vector3.New(30,30,0)),
		[IllusionConfig.FashionType.weapon] = UiRedPoint.New(self.ItemWeapon, Vector3.New(30,30,0)),
		[IllusionConfig.FashionType.mount] = UiRedPoint.New(self.ItemMount, Vector3.New(30,30,0)),
	}

	UIWFlushPanel.Awake(self)
end

function IllusionViewLeft:onFlush()
	-- 点击升级物品跳转
	local jump_item_id = self.data:GetNeedJumpItemId()
	if jump_item_id then
		local cfg = Cfg.SurfaceInfoByResolveItemId(jump_item_id)
		if not TableIsEmpty(cfg) then
			self.ClickEvents[cfg.type]:Trigger()
		end
	end
end

function IllusionViewLeft:OnClickReturn()
	ViewMgr:CloseView(IllusionView)
end

function IllusionViewLeft:OnClickUndo()
	AudioMgr:PlayEffect(AudioType.UI, "bag_tidy")
	self.data:ResetCurModelInfo()
	local role_list = self.rc_data:GetRCDataRoleList()
	for i = 1, self.Roles:Length() do
		if RoleData.Instance:GetRoleAvatarType() == role_list[i].role_id then self.Roles[i]:Click() end
	end
	local prof_list = self.rc_data:GetRoleProfList(self.data.cur_model_info.avatar_type).prof_list
	if prof_list == self.ProfList:GetData() then
		self.ProfList:ClickItem(self.data:CurSelProfIndex())
	end
end

function IllusionViewLeft:FlushRedPoints()
	for type, red_point in pairs(self.data.red_points) do
		self.red_points[type]:SetNum(red_point and 1 or 0)
	end
end

function IllusionViewLeft:OnClickItem(type)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
	self.data.cur_model_info.type = type
end

----------------------IllusionViewDown----------------------
IllusionViewDown = IllusionViewDown or DeclareMono("IllusionViewDown", UIWFlushPanel)

function IllusionViewDown:IllusionViewDown()
	self.flush_events = {EventProtocol.ToEventKey(SCSurfaceSingleInfo)}

	self.data = IllusionCtrl.Instance:Data()
	self.rc_data = RoleCreateData.Instance

	self.data_cares = {
		{data = self.data.cur_model_info, func = self.FlushSuitId, keys = {"suit_id"}},
		{data = self.data.cur_model_info, func = self.FlushProf, keys = {"type", "avatar_type"}, init_call = false},
	}
end

function IllusionViewDown:FlushSuitId()
	local suit_id = self.data.cur_model_info.suit_id
	if suit_id ~= 0 then
		local suit_info = self.data:GetSuitInfoWithSuitId(suit_id)
		
		self.Name.text = suit_info.suit_name
		self.SuitList:SetData(suit_info.suit_state)
		self.AttrGrid:SetData(suit_info.attr_show)
	end
	self.DetailObj:SetActive(suit_id ~= 0)
end

function IllusionViewDown:FlushProf()
	local show_prof = IllusionConfig.FashionType.weapon == self.data.cur_model_info.type
	self.data.cur_model_info.default_prof.trans = false
	self.ProfList:SetObjActive(show_prof)
	local prof_list = self.rc_data:GetRoleProfList(self.data.cur_model_info.avatar_type).prof_list
	if show_prof then
		self.ProfToggleGroup:SetAllTogglesOff()
		self.ProfList:SetDataList(prof_list, self.data:CurSelProfIndex())
	elseif IllusionConfig.FashionType.cloth == self.data.cur_model_info.type then
		self.data.cur_model_info.prof_type = RoleData.ProfIdToProfType(prof_list[self.data:CurSelProfIndex()].prof_id)
	end
end

----------------------IllusionViewDownProfItem----------------------
IllusionViewDownProfItem = DeclareMono("IllusionViewDownProfItem", UIWidgetBaseItem)

function IllusionViewDownProfItem:IllusionViewDownProfItem()
	self.illusion_data = IllusionCtrl.Instance:Data()
end

function IllusionViewDownProfItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local prof_type = RoleData.ProfIdToProfType(data.prof_id)
	self.ProfSp.SpriteName = RoleData.GetProfSp(prof_type)
end

function IllusionViewDownProfItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
    end
end

function IllusionViewDownProfItem:OnClickItem()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanZhanKai)
	local prof_type = RoleData.ProfIdToProfType(self.data.prof_id)
	if self.illusion_data.cur_model_info.default_prof.trans then self.illusion_data.cur_model_info.default_prof.prof = prof_type end
	self.illusion_data.cur_model_info.prof_type = prof_type
	self.illusion_data.cur_model_info.default_prof.trans = true
end

----------------------IllusionViewDownSuitListItem----------------------
IllusionViewDownSuitListItem = DeclareMono("IllusionViewDownSuitListItem", UIWidgetBaseItem)

function IllusionViewDownSuitListItem:SetData(data)
	self.Name.text = data.name
	self.State.text = "<color=#" .. (data.actived and COLORSTR.Green3 or COLORSTR.Red6) .. ">" .. (data.actived and 1 or 0) .. "/1</color>"
end

----------------------IllusionViewDownAttrGridItem----------------------
IllusionViewDownAttrGridItem = DeclareMono("IllusionViewDownAttrGridItem", UIWidgetBaseItem)

function IllusionViewDownAttrGridItem:IllusionViewDownAttrGridItem()
	self.illusion_data = IllusionCtrl.Instance:Data()
end

function IllusionViewDownAttrGridItem:SetData(data)
	local suit_info = self.illusion_data:GetSuitInfoWithSuitId(data.suit_id)
	self.Desc.text = "<color=#" .. (suit_info.suit_state_actived and COLORSTR.Yellow12 or COLORSTR.Gray2) .. ">" .. data.content .. "</color>"
end

----------------------IllusionViewRight----------------------
IllusionViewRight = IllusionViewRight or DeclareMono("IllusionViewRight", UIWFlushPanel)

function IllusionViewRight:IllusionViewRight()
	self.flush_events = {EventProtocol.ToEventKey(SCSurfaceSingleInfo), EventProtocol.ToEventKey(SCSurfaceWearInfo)}

	self.data = IllusionCtrl.Instance:Data()
	self.rc_data = RoleCreateData.Instance

	self.data_cares = {
		{data = self.data.cur_model_info, func = self.FlushType, keys = {"type"}, init_call = false},
		{data = self.data.cur_model_info, func = self.FlushRoleError, keys = {"avatar_type"}, init_call = false},
		{data = self.data.cur_model_info, func = self.FlushAttrList, keys = {"type", "fashion"}},
		{data = self.data.illusion_lv_info, func = self.FlushType, init_call = false},
		{data = self.data.illusion_lv_info, func = self.FlushAttrList, init_call = false},
	}
end

function IllusionViewRight:Awake()
	UIWFlushPanel.Awake(self)

	self.ht2 = TimeHelper:AddDelayFrameTimer(
		function ()
			local role_list = self.rc_data:GetRCDataRoleList()
			for i = 1, self.Roles:Length() do
				self.Roles[i]:SetData(role_list[i])
				if RoleData.Instance:GetRoleAvatarType() == role_list[i].role_id then self.Roles[i]:Click() end
			end
		end
	)
end

function IllusionViewRight:onFlush()
	local fashion_show_list = self.data:GetFashionShowListWithType(self.data.cur_model_info.type)
	self.SuitGrid:SetCompleteCallBack(function()
		-- 点击升级物品跳转
		local jump_item_id = self.data:GetNeedJumpItemId()
		if jump_item_id then
			local cfg = Cfg.SurfaceInfoByResolveItemId(jump_item_id)
			if not TableIsEmpty(cfg) and self.data.cur_model_info.type == cfg.type then
				for k, v in pairs(fashion_show_list) do
					for l, w in pairs(v) do
						if not w.is_empty then
							if w.info.decompose_item == cfg.decompose_item then
								TimeHelper:CancelTimer(self.ht)
								self.ht = TimeHelper:AddDelayFrameTimer(function()
									self.ScrollRectPage:ScrollToPage(k - 1)
								end, 1)
								IllusionData.Instance:JumpItemIdPageFlag(true)
								IllusionData.Instance:SetNeedJumpItemId()
							end
						end
					end
				end
			end
		end
	end)
	self.SuitGrid:SetData(fashion_show_list)
	self.SpRemind:SetNum(self.data:IsCanSpLvUp() and 1 or 0)
end

function IllusionViewRight:FlushType()
	local fashion_show_list = self.data:GetFashionShowListWithType(self.data.cur_model_info.type)
	self.SuitGrid:SetData(fashion_show_list)

	for i = 1, self.RightShowObjs:Length() do
		self.RightShowObjs[i]:SetActive(IllusionConfig.FashionType.mount ~= self.data.cur_model_info.type)
	end

	self.BtnRideObj:SetActive(IllusionConfig.FashionType.mount == self.data.cur_model_info.type)

	self.SpRemind:SetNum(self.data:IsCanSpLvUp() and 1 or 0)
end

function IllusionViewRight:FlushRoleError()
	self.RoleErrorObj:SetActive(RoleData.Instance:GetRoleAvatarType() ~= self.data.cur_model_info.avatar_type)
end

function IllusionViewRight:FlushAttrList()
	local data = self.data.cur_model_info.fashion[self.data.cur_model_info.type]
	local selected = nil ~= data
	if selected then
		local attr_list = self.data:GetAttrList(data.id)
		self.AttrList:SetData(attr_list)
		self.AttrListObj:SetActive(true)
	else
	end
	self.AttrListObj:SetActive(selected)
	self.DecoSX1Obj:SetActive(not selected)
	self.DecoSX2Obj:SetActive(selected)
	self.LeftPos.localPosition = Vector2.New(selected and -620 or -530, 275)
end

function IllusionViewRight:OnClickAttrList()
	ViewMgr:OpenView(IllusionAllAttrView)
end

function IllusionViewRight:OnClickSpLv()
	ViewMgr:OpenView(IllusionSpLevelUpView)
end

function IllusionViewRight:OnClickSuit()
	ViewMgr:OpenView(IllusionViewSuitAttr)
end

function IllusionViewRight:OnClickExchange()
	ViewMgr:OpenView(IllusionExchangeView)
end

function IllusionViewRight:OnClickRide(value)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
	self.data.cur_model_info.is_ride = value
end

function IllusionViewRight:OnClickCloseDetail()
	self.DetailObj:SetActive(false)
end

function IllusionViewRight:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.ht)
	TimeHelper:CancelTimer(self.ht2)
end

----------------------IllusionViewRightSuitGridItems----------------------
IllusionViewRightSuitGridItems = DeclareMono("IllusionViewRightSuitGridItems", UIWidgetBaseItem)

function IllusionViewRightSuitGridItems:SetData(data)
	self.SuitList:SetData(data)
end

----------------------IllusionViewRightRoleItem----------------------
IllusionViewRightRoleItem = DeclareMono("IllusionViewRightRoleItem", UIWidgetBaseItem)

function IllusionViewRightRoleItem:IllusionViewRightRoleItem()
	self.illusion_data = IllusionCtrl.Instance:Data()
end

function IllusionViewRightRoleItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	UH.SetAvatarp(self.RoleSp,data.role_id)
end

function IllusionViewRightRoleItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function IllusionViewRightRoleItem:ClickItem()
	if self.ToggleItem then
		self.ToggleItem.isOn = true
	end
end

function IllusionViewRightRoleItem:OnClickItem()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
	self.illusion_data:CurSelAvatarType(self.data.role_id)
end

----------------------IllusionViewRightSuitListItem----------------------
IllusionViewRightSuitListItem = DeclareMono("IllusionViewRightSuitListItem", UIWidgetBaseItem)

function IllusionViewRightSuitListItem:IllusionViewRightSuitListItem()
	self.illusion_data = IllusionCtrl.Instance:Data()
	self.fashion_names = self.illusion_data.fashion_names
end

function IllusionViewRightSuitListItem:OnDestroy()
    self:uncareData()
	TimeHelper:CancelTimer(self.timer_ct)
	BagData.Instance:RemoveListenItems(self.item_listen)
end

function IllusionViewRightSuitListItem:FlushRemind()
	self:FlushLvUpShow()
	local is_can_up = IllusionData.Instance:IsCanLvUp(self.data.id)
	self.Remind:SetNum(is_can_up and 1 or 0)
end

function IllusionViewRightSuitListItem:FlushLvUpShow()
	local is_can_up = false
	local is_effect = false
	if self.now_is_select then
		local lv = IllusionData.Instance:IllusionLv(self.data.id)
		if lv > 0 and (not IllusionData.Instance:IsSpecial(self.data.id)) then
			is_can_up = true
			local has = Item.GetNum(self.data.info.decompose_item)
			if has > 0 and lv ~= IllusionData.Instance:IllusionMaxLv() then
				is_effect = true
			end
		end
	end
	self.ObjCanUp:SetActive(is_can_up)
	self.ObjEffect:SetActive(is_effect)
end

function IllusionViewRightSuitListItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	TimeHelper:CancelTimer(self.timer_ct)
	self:uncareData()
	self.care_handles = {}
	
	self.InfoObj:SetActive(not data.is_empty)
	self.EmptyObj:SetActive(data.is_empty)


	if data.is_empty then return end

	self:careData(self.illusion_data.cur_model_info.fashion, BindTool.Bind(self.FlushSelect, self))
	self:careData(self.illusion_data.cur_model_info, BindTool.Bind(self.FlushShowAT, self), "avatar_type")
	self:careData(self.illusion_data.cur_model_info, BindTool.Bind(self.FlushShowPT, self), "prof_type")

	local info = data.info
	local wear_id = self.illusion_data:GetFashionWearIdWithType(info.type)
	self.Name.text = self.fashion_names[info.id] and self.fashion_names[info.id][self.illusion_data.cur_model_info.avatar_type] or info.surface_name
	self.BtnEquipInter.Interactable = data.level > 0
	self.MaskObj:SetActive(data.level == 0)
	self.SuitObj:SetActive(data.suit_id ~= 0)
	self.BtnEquipObj:SetActive(data.level == 0 or wear_id ~= info.id)
	self.BtnOffObj:SetActive(data.level > 0 and wear_id == info.id)
	self.TipsObj:SetActive(data.level == 0 or wear_id == info.id)
	self.Tips.text = data.level == 0 and Language.Illusion.Tips.get_way[info.can_buy] or Language.Illusion.Tips.equiped
	self.LimitObj:SetActive(1 == info.is_limit)

	self.LimitTimeObj:SetActive(data.level > 0 and 1 == info.is_limit)
	if data.end_time > 0 then
		if data.end_time - TimeHelper.GetServerTime() > 0 then
			self.timer_ct = TimeHelper:AddCountDownCT(function ()
				local last_time = data.end_time - TimeHelper.GetServerTime()
				local ft = TimeHelper.FormatDHMS(last_time)
				self.LimitTime.text = string.format(Language.Illusion.LimitTime, TimeHelper.TimeFormat2(ft))
			end, function ()
				self.LimitTime.text = string.format(Language.Illusion.LimitTime, Language.Illusion.EndTime0)
			end, data.end_time, 1)
		else
			self.LimitTime.text = string.format(Language.Illusion.LimitTime, Language.Illusion.EndTime0)
		end
	end

	if IllusionConfig.FashionType.mount == self.illusion_data.cur_model_info.type then self:FlushShow() end

	-- self.ShowImageSetter:SetObjActive(IllusionConfig.FashionType.mount ~= self.illusion_data.cur_model_info.type)

	BagData.Instance:RemoveListenItems(self.item_listen)
	self.item_listen = nil
	if self.item_listen == nil then
		self.item_listen = BagData.Instance:ListenItems({data.info.decompose_item}, BindTool.Bind(self.FlushRemind, self))
	end
	self:FlushRemind()
	local lv = IllusionData.Instance:IllusionLv(data.id)
	self.StarList:SetData(IllusionData.Instance:IllusionLvStarData(lv))

	-- 点击升级物品跳转
	local jump_item_id = IllusionData.Instance:GetNeedJumpItemId()
	if jump_item_id then
		local cfg = Cfg.SurfaceInfoByResolveItemId(jump_item_id)
		if not TableIsEmpty(cfg) and data.info.decompose_item == cfg.decompose_item then
			self:Click()
			IllusionData.Instance:JumpItemIdItemFlag(true)
			IllusionData.Instance:SetNeedJumpItemId()
		end
	end
end

function IllusionViewRightSuitListItem:OnLvUpClick()
	IllusionData.Instance:SetLvUpNeedData(self.data.id, self.StarList.gameObject.transform.position)
	ViewMgr:OpenView(IllusionLevelUpView)
end

function IllusionViewRightSuitListItem:FlushShow()
	local cur_model_info = self.illusion_data.cur_model_info
	local png_path = self.illusion_data:GetShowPngPath{id = self.data.id, type = cur_model_info.type, avatar_type = cur_model_info.avatar_type, prof_type = cur_model_info.prof_type}
	if nil == png_path then return end
	self.ShowImageSetter:SetByPath(png_path)
end

function IllusionViewRightSuitListItem:FlushShowAT()
	local cur_model_info = self.illusion_data.cur_model_info
	if IllusionConfig.FashionType.cloth == cur_model_info.type and IllusionConfig.FashionType.cloth == self.data.type then
		self:FlushShow()
		local info = self.data.info
		self.Name.text = self.fashion_names[info.id] and self.fashion_names[info.id][cur_model_info.avatar_type] or info.surface_name
	end
end

function IllusionViewRightSuitListItem:FlushShowPT()
	if IllusionConfig.FashionType.weapon == self.illusion_data.cur_model_info.type and IllusionConfig.FashionType.weapon == self.data.type then
		self:FlushShow()
	end
end

function IllusionViewRightSuitListItem:FlushSelect()
	if self.data == nil then return end
	if self.data == self.illusion_data.cur_model_info.fashion[self.data.type] then
		UIWidgetBaseItem.SetSelected(self, true)
	else
		UIWidgetBaseItem.SetSelected(self, false)
	end
end

function IllusionViewRightSuitListItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function IllusionViewRightSuitListItem:OnClickItem(selected)
	self.now_is_select = selected
	self:FlushLvUpShow()
	local value, red_point = self.illusion_data:CurSelFashion(self.data, selected)
	if value then
		UIWidgetBaseItem.SetSelected(self, true)
	else
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	end
end

function IllusionViewRightSuitListItem:OnClickEquip()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.EquipEquipment)
	self.Selector.isOn = true
	-- self.illusion_data:CurSelFashion(self.data)
	if 0 == self.data.level then
		PublicPopupCtrl.Instance:Center(Language.Illusion.EquipTips)
		return
	end
	local info = self.data.info
	local vo = self.data.vo
	local wear_id = self.illusion_data:GetFashionWearIdWithType(info.type)
	IllusionCtrl.Instance:SendSurfaceWear(wear_id == info.id and 0 or info.id, info.type)
end

function IllusionViewRightSuitListItem:OnClickTakeOff()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.EquipEquipment)
	self.Selector.isOn = true
	local info = self.data.info
	local vo = self.data.vo
	local wear_id = self.illusion_data:GetFashionWearIdWithType(info.type)
	IllusionCtrl.Instance:SendSurfaceWear(wear_id == info.id and 0 or info.id, info.type)
end

function IllusionViewRightSuitListItem:OnClickGet()
	-- self.illusion_data.cur_sel_fashion = self.data
	-- ViewMgr:FlushView(IllusionView, IllusionViewRight:GetTypeName())
	-- local info = self.data.info
end

----------------------IllusionViewRightAttrsItem----------------------
IllusionViewRightAttrsItem = DeclareMono("IllusionViewRightAttrsItem", UIWidgetBaseItem)

function IllusionViewRightAttrsItem:SetData(data)
	local value = RoleData.Instance:ProfType() == data.prof
	self.TitleSp.SpriteName = value and "XiaoZhiYeDi1" or "XiaoZhiYeDi2"
	self.Prof.text = RichTextHelper.ColorStr(data.prof_name, value and COLORSTR.Yellow1 or COLORSTR.Gray3)
	self.Attrs:SetData(data.attrs)
end

----------------------IllusionViewRightAttrItem----------------------
IllusionViewRightAttrItem = DeclareMono("IllusionViewRightAttrItem", UIWidgetBaseItem)

function IllusionViewRightAttrItem:SetData(data)
	local value = RoleData.Instance:ProfType() == data.prof
	self.Attr.text = RichTextHelper.ColorStr(Format("%s+%s", data.attr_name, data.attr_percent_value), value and COLORSTR.Yellow12 or COLORSTR.Gray2)
end

----------------------IllusionViewRightAttrGridItem----------------------
IllusionViewRightAttrGridItem = DeclareMono("IllusionViewRightAttrGridItem", UIWidgetBaseItem)

function IllusionViewRightAttrGridItem:IllusionViewRightAttrGridItem()
	self.illusion_data = IllusionCtrl.Instance:Data()
end

function IllusionViewRightAttrGridItem:SetData(data)
	self.Desc.text = string.format("%s+%s", data.attr_name, data.attr_val)
end

----------------------IllusionViewModel----------------------
IllusionViewModel = IllusionViewModel or DeclareMono("IllusionViewModel", UIWFlushPanel)

function IllusionViewModel:IllusionViewModel()
	self.data = IllusionCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data.cur_model_info.fashion, func = self.FlushModel},
		{data = self.data.cur_model_info, func = self.FlushModel, keys = {"avatar_type", "type", "prof_type", "is_ride"}, init_call = false},
	}
end

function IllusionViewModel:FlushModel()
	if nil == self.ui_obj_role then
		self.ui_obj_role = UIChDrawer.New()
		self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj_role,
				view = self
			}
		)
	end

	local appearance = self.data:TransFashionToAppearance()
	local is_ride = IllusionConfig.FashionType.mount == self.data.cur_model_info.type and self.data.cur_model_info.is_ride
	self.ui_obj_role.is_ride = is_ride
	if IllusionConfig.FashionType.mount == self.data.cur_model_info.type and appearance.ride_id > 0 and not is_ride then
		self.ModelImage.raycastTarget = false
		self.ui_obj_role:SetRide(appearance.ride_id, function ()
			self.ModelImage.raycastTarget = true
		end)
		self.ui_obj_role:PlayAnim(ChAnimType.Show)
	else
		self.ModelImage.raycastTarget = true
		self.ui_obj_role:SetData(appearance, {avatar_type=appearance.avatar_type})
		-- self.ui_obj_role:PlayAnim(ChAnimType.WeaponShow)
	end
	self.Effect412:SetActive(self.data:IsSelectId(IllusionConfig.FashionType.mount, 412))
end

--------------IllusionLvStarItem--------------
IllusionLvStarItem = IllusionLvStarItem or DeclareMono("IllusionLvStarItem", UIWidgetBaseItem)
function IllusionLvStarItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SpriteName(self.Img, "XingXing" .. data.star)
end
