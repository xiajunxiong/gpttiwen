ItemInfoViewBase = ItemInfoViewBase or DeclareMono("ItemInfoViewBase", ItemInfoViewItem)

-- 取 item 的 "Name", "TypeDesc", "LimitProfDesc", "LevelDesc", "Cap", "Bind", "GetWay" ,"PreRange" 方法
-- 判断返回值 nil = not show , 返回值 = 显示值
function ItemInfoViewBase:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	self.Cell:SetData(self.item1)
	UH.SpriteName(self.BindSp,self:GetBindName(self.item1))
	UH.SpriteName(self.Quality,ItemInfoConfig.QualitySp[self.item1:Color()])
	self.UsingObj:SetActive(ItemInfoConfig.ShowType.Left == self.ShowType and nil ~= self.item2)
	for i = 1, #ItemInfoConfig.ItemInfoShow do
		local ItemInfoShow = ItemInfoConfig.ItemInfoShow
		local show_info = nil ~= self.item1[ItemInfoShow[i]] and self.item1[ItemInfoShow[i]](self.item1) or nil
		if DEBUG_MODE and i == 1 then
			show_info = show_info.."("..self.item1.item_id..")"
		end
		if nil ~= show_info and self.language.BaseInfo[i] then
			if type(show_info) == [[table]] then
				UH.SetText(self[ItemInfoShow[i]],show_info[1])
			else
				UH.SetText(self[ItemInfoShow[i]],Format(self.language.BaseInfo[i], show_info))
			end
			if i == 1 and ItemInfoShow[i] and self[ItemInfoShow[i]] and QualityColor[self.item1:Color()] then
				self[ItemInfoShow[i]].color = QualityColor[self.item1:Color()]
			end
		end
		if ItemInfoShow[i] == "LevelDesc" then 
			-- LogError("!?@",show_info)
		end 
		self.BaseObjs[i]:SetActive(nil ~= show_info)
	end

	self:ShowOneselfBase()

	if self.info.show_ways then
		TimeHelper:CancelTimer(self.getway_timer)
		self.getway_timer = TimeHelper:AddDelayTimer(function() 
			self:OnClickGetWay()
		end,0.2)
	end
	self:FlushDebugMode()

	self.sp_bg:SetActive(false)
	if self.item1:IsStarGod() then
		for i=1,2 do--self.Image:Length() do
			if self.Image[i] ~= nil then 
			    UH.SpriteName(self.Image[i],Format("QualityBG%s_%s",self.item1:Color(),i))
			end 
		end
		UH.SpriteName(self.ImgBG,"QualityBG"..self.item1:Color())
	elseif self.item1:IsMarbas() then
		for i=3,4 do  -- self.Image:Length() do
			if self.Image ~= nil and self.Image[i] ~= nil then 
			    UH.SpriteName(self.Image[i],MarbasConfig.ItemInfo[self.item1:Color()])
			end 
		end
		self.sp_bg:SetActive(true)
		UH.SetText(self.sp_info,self.item1:ManaDesc())
	else
		UH.SpriteName(self.ImgBG,"Frame")
		for i=1,self.Image:Length() do
			UH.SpriteName(self.Image[i])
		end
	end
end

-- 单独显示baseinfo
function ItemInfoViewBase:ShowOneselfBase()
	local item = self.info.item_list[self.ShowType]
	local base_part_index = 0
	if item.OneselfBaseIndex then
		base_part_index = item:OneselfBaseIndex()
	end
	if self.OtherBaseInfos == nil then return end 
	for i = 1, self.OtherBaseInfos:Length() do
		self.OtherBaseInfos[i]:SetObjActive(i == base_part_index)
		if i == base_part_index then
			self.OtherBaseInfos[i]:SetData(item)
		end
	end

	-- 这块铸灵的道具类型做特殊处理
	if 2 == base_part_index then
		self.BaseObjs[2]:SetActive(false)
	end
end

function ItemInfoViewBase:OnDisable()
	ItemInfoViewItem.OnDisable(self)
	UH.SpriteName(self.ImgBG,"Frame")
	for i=1,self.Image:Length() do
		UH.SpriteName(self.Image[i])
	end
end

function ItemInfoViewBase:GetBindName(item)
	if item.item_id == nil or Item.GetIsVirtualItem(item.item_id) or not ViewMgr:IsOpen(BagView)then
		return
	end
	if MarketData.Instance:IsMarketSell(item) then
		return ItemInfoConfig.BindSpriteName[2]
	end
	local can_trade = not item:Bind() and item:CanTrade()
	return ItemInfoConfig.BindSpriteName[can_trade and 0 or 1]
end

function ItemInfoViewBase:OnClickAdd(num)
	GMCmdCtrl.Instance:SendGMCommand("additem", self.item1.item_id .. " " .. num .. " 0")
end

function ItemInfoViewBase:OnClickGetWay()
	local config = Item.GetConfig(self.item1.item_id)
	if config.special_way == 2 or self.item1:InfoConfig() ~= nil then 
		ItemInfoCtrl.Instance:ItemDeriveView(ItemGetWayView,
		{item_id = self.item1.item_id,pos = UH.GetRootPosition(self.gameObject)+Vector2.New(213,-9)})
	else 
		ItemInfoData.Instance:ChangeGetWayShow()
	end 
end

function ItemInfoViewBase:OnClickPreView()
	local param_t = {show_type = self.ShowType,pos = UH.GetRootPosition(self.gameObject)}
	param_t.pos = self.ShowType == 2 and param_t.pos + Vector2.New(436,0) or param_t.pos
	if ItemInfoCtrl.Instance:Data().derive_view then 
		ItemInfoCtrl.Instance:ItemDeriveView()
		if not ViewMgr:IsOpen(ItemPreRangeView) then 
			ItemInfoCtrl.Instance:ItemDeriveView(ItemPreRangeView,param_t)
	    end
	else 
		ItemInfoCtrl.Instance:ItemDeriveView(ItemPreRangeView,param_t)
	end 
end

function ItemInfoViewBase:FlushDebugMode()
	if self.GMBtnList == nil then return end 

	for i=1,self.GMBtnList:Length() do
		self.GMBtnList[i]:SetActive(DEBUG_MODE == true or GmData.Instance.view_data.get_item == true)
	end
end

ItemInfoViewDesc = ItemInfoViewDesc or DeclareMono("ItemInfoViewDesc", ItemInfoViewItem)

function ItemInfoViewDesc:ItemInfoViewDesc()
    self.data_cares = {
        {data = FightData.Instance.crystal_use_data,  func = self.FlushDescview,  init_call = false},
    }
end

function ItemInfoViewDesc:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	self.Title.text = self.language.ItemTitles.Desc
	self:FlushDescview()
end

function ItemInfoViewDesc:FlushDescview()
	local content = self.item1:Desc()
	if ItemInfoData.IsMedicinal(self.item1.item_id) then
		local times = ItemInfoData.GetMedicinalTimes(self.item1.item_id)
		local color = times > 0 and COLORSTR.Green3 or COLORSTR.Red12
		content = content .. Format(self.language.ItemTitles.FbUseTimes,color,times)
	elseif ItemType.ExpPercent == self.item1.item_type then
		local desc = Item.GetDescExpPercent(self.item1.item_id)
		content = StringIsEmpty(desc) and content or desc
	elseif content == "" and BagData.IsHasGift(self.item1.item_id) then
		content = self:GetGiftDesc(self.item1.item_id,self.item1.num)
	elseif self.item1:IsNevealStone() and self.item1:IsNeveal() then 
		content = self.item1:NevealDesc()
	elseif ItemType.PeakToken == self.item1.item_type then
		local max_times = Config.supreme_token_auto.other[1].use_frequency
		local times = max_times - (self.item1.param.used_times or 0)
		content = content .. Format(self.language.ItemTitles.PeakTokenUseTimes,times,max_times)
	end
	UH.SetText(self.Content,content)
	UH.SetText(self.Tips,WorkshopData.GetScoreTip(self.item1))
	self.Tips:SetObjActive(self.Tips.text ~= "")
end

--礼包描述
function ItemInfoViewDesc:GetGiftDesc(item_id,item_num)
	item_num = item_num == 0 and 1 or item_num
	local config = BagData.Instance:GetGiftItemList(item_id)
	local temp = self:GetGiftTitle(config)
	for k,v in pairs(config.item_list) do
        temp = Format("%s\n%s x%s",temp,Item.GetQuaName(v.item_id),v.num)
	end
	return temp
end

function ItemInfoViewDesc:GetGiftTitle(config)
	local ItemGiftDesc = Language.ItemInfo.ItemGiftDesc
	if config.select_num and config.select_num ~= 0 then
		return Format(ItemGiftDesc.select_num,config.select_num)
	elseif config.rand_num and config.rand_num ~= 0 then
		return Format(ItemGiftDesc.rand_num,config.rand_num)
	end
	return ItemGiftDesc.item_num
end

ItemInfoViewEquipBase = ItemInfoViewEquipBase or DeclareMono("ItemInfoViewEquipBase", ItemInfoViewItem)

function ItemInfoViewEquipBase:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local info_data = ItemInfoCtrl.Instance:Data()
	UH.SetText(self.Title, Format(self.language.ItemTitles.EquipBase,""))
	--s属性专精特效
	local temp = self.item1:ItemInfoEquipBase()
	local temp2 = ""
	local is_add = false
	if self.item1.param ~= nil then 
		if self.item1:BaseAttr() then 
			local attr_list = {}
			local operate_list = self.item1.TypedAttrList and self.item1:TypedAttrList() or self.item1:BaseAttr()-- (self.item1:WithUlti() or (not self.item1:CheckWithUltimateAttr())) and self.item1:BaseAttr() or self.item1:UltimateAttrList()
			local item2 = (ItemInfoConfig.ShowType.Main == self.ShowType and self.info.item_list[2] and self.info.item_list[2].param ~= nil
			    and self.info.item_list[2]:BaseAttr()~= nil) and self.info.item_list[2] or (info_data.crystal_equip ~= nil and self.item1:Compare() or nil)
			local is_mine = BagData.Instance:CheckSameEquipByUniqueIdForAllIn(self.item1)
			local acc_tianming_plus = (is_mine and not self.item1.in_bag ) and AccrssoryData.Instance:GetDestinyRate(TIAN_MING_ADD_ATTR_SYS_TYPE.EQUIPMENT)  or 0 
			if not self.item1:WithUlti() and self.item1:CheckWithUltimateAttr() then 
				for k, v in pairs(operate_list) do
					if v.attr_value ~= 0 then
						local vo = {}
						if v.ultimate_value and v.ultimate_value > 0 then 
						    vo.str = Format(self.language.ItemTitles.EquipAttrU, 
							    Language.Common.AttrList[v.attr_type], 
							    DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value,
						        v.ultimate_value)
						else 
							vo.str = Format(self.language.ItemTitles.EquipAttr2, 
							Language.Common.AttrList[v.attr_type], 
							DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)
						end 
						vo.attr_value = v.attr_value + (v.ultimate_value or 0)
						if item2 then 
						    local item2_cell = item2:TypedAttrList()[v.attr_type]
						    vo.show_arrow = item2_cell and (vo.attr_value) ~= (item2_cell.attr_value + (item2_cell.ultimate_value or 0))
						    vo.is_up = item2_cell and (vo.attr_value) > (item2_cell.attr_value + (item2_cell.ultimate_value or 0))
						end 
						table.insert(attr_list,vo)
						is_add = true
					end
				end 
			elseif acc_tianming_plus > 0 and not self.item1:IsMedal() then 
				for k, v in pairs(operate_list) do
					if v.attr_value ~= 0 then
						local vo = {}
						vo.str = Format(self.language.ItemTitles.EquipAttrTian, 
						Language.Common.AttrList[v.attr_type], 
						DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value,
						DataHelper.IsPercent(v.attr_type) and Percent(math.floor( v.attr_value * acc_tianming_plus)) or math.floor( v.attr_value * acc_tianming_plus ) )

						vo.attr_value = v.attr_value + (v.ultimate_value or 0)
						if item2 then 
						    local item2_cell = item2:TypedAttrList()[v.attr_type]
						    vo.show_arrow = item2_cell and vo.attr_value ~= item2_cell.attr_value
						    vo.is_up = item2_cell and vo.attr_value > item2_cell.attr_value
						end 
						table.insert(attr_list,vo)
						is_add = true
					end
				end 
			else 
				for k, v in pairs(operate_list) do
					if v.attr_value ~= 0 then
						local vo = {}
						vo.str = Format(self.language.ItemTitles.EquipAttr2, 
							Language.Common.AttrList[v.attr_type], 
							DataHelper.IsPercent(v.attr_type) and Percent(v.attr_value) or v.attr_value)

						vo.attr_value = v.attr_value + (v.ultimate_value or 0)
						if item2 then 
						    local item2_cell = item2:TypedAttrList()[v.attr_type]
						    vo.show_arrow = item2_cell and vo.attr_value ~= item2_cell.attr_value
						    vo.is_up = item2_cell and vo.attr_value > item2_cell.attr_value
						end 
						table.insert(attr_list,vo)
						is_add = true
					end
				end 
			end 

			local show_flag = (not self.item1:WithUlti()) and self.item1:CheckWithUltimateAttr()
			self.Content_stren:SetActive(show_flag)
			self.Content_nomal:SetActive(not show_flag)

			for i= 1,6 do 
				local operate_list = show_flag and self.stren_cell or self.nomal_cell
				operate_list[i].gameObject:SetActive(attr_list[i] ~= nil)
				if attr_list[i] ~= nil then operate_list[i]:SetData(attr_list[i]) end 
			end 
		end 
	end 
	-- 属性值区间
	if (self.item1:IsJewelry() or self.item1:IsEquip()) and not is_add then
		local stren_flag = false -- self.item1:Color() > 3
		local attr_list = {}
		if (self.item1:Color() == ItemColor.Star or self.item1:Color() == ItemColor.God) then
			local sec_list = DivineCostumeData.Instance:GetBaseAttrByIdAndLevel(self.item1.item_id,1,self.item1:Type())
			for k, value in ipairs(sec_list) do
				local str_format = "%s   %s"
				local vo = {}
				vo.str = Format(str_format, Language.Common.AttrList[value.attr_type],value.attr_value)
				if value.attr_value ~= 0 then 
				    table.insert(attr_list,vo)
				end 
			end
		elseif not Equip.CanBlazeStar(self.item1.item_id) then 
			local sec_list = self.item1:AttrSection()
			for k, value in ipairs(sec_list) do
				local str_format =  (stren_flag and value.with_stren) and "%s   %s ~ %s<color=#"..COLORSTR.Purple4..">（+%s~%s）</color>" or (value.max ~= value.min and "%s   %s ~ %s" or "%s   %s")
				local vo = {}
				if value.with_stren then 
				    vo.str = Format(str_format, Language.Common.AttrList[value.attr_type], 
					    DataHelper.IsPercent(value.attr_type) and Percent(value.min) or value.min,
					    DataHelper.IsPercent(value.attr_type) and Percent(value.max) or value.max,
					    DataHelper.IsPercent(value.attr_type) and Percent(value.stren_min) or value.stren_min,
					    DataHelper.IsPercent(value.attr_type) and Percent(value.stren_max) or value.stren_max)
				else 
					vo.str = Format(str_format, Language.Common.AttrList[value.attr_type], 
					DataHelper.IsPercent(value.attr_type) and Percent(value.min) or value.min,
					DataHelper.IsPercent(value.attr_type) and Percent(value.max) or value.max)
			    end 
				table.insert(attr_list,vo)
			end 
		else 
			-- 取1星属性(item_star ==2)
			local balze_star = self.item1:IsBlazeStar() and self.item1:BlazeStar() or 1
			local blaze_cfg = WorkshopData.Instance:GetBlazeCfg(self.item1.item_id,balze_star)
			
			for i = 1,6 do 
				local at_type = blaze_cfg["attr_type_"..i]
				local at_value = blaze_cfg["attr_min_"..i]
				if at_value > 0 then
					local vo = {}
					vo.str = Format(self.language.ItemTitles.EquipAttr2, 
					Language.Common.AttrList[at_type], 
					DataHelper.IsPercent(at_type) and Percent(at_value) or at_value)

					table.insert(attr_list,vo)
				end 
			end 
		end

		self.Content_stren:SetActive(stren_flag)
		self.Content_nomal:SetActive(not stren_flag)
		
		for i= 1,6 do 
			local operate_list = (stren_flag ) and self.stren_cell or self.nomal_cell
			operate_list[i].gameObject:SetActive(attr_list[i] ~= nil)
			if attr_list[i] ~= nil then operate_list[i]:SetData(attr_list[i]) end 
		end 
	end 
	
	self.Content:SetObjActive(nil ~= temp)
	if nil ~= temp then 
		self.Content_nomal:SetActive(false)
	    self.Content_stren:SetActive(false)
	end 
	if temp then UH.SetText(self.Content, temp) end
end

EquipBaseAttrCell = EquipBaseAttrCell or DeclareMono("EquipBaseAttrCell", UIWidgetBaseItem)
function EquipBaseAttrCell:EquipBaseAttrCell()	end 
function EquipBaseAttrCell:SetData(data)
	UH.SetText(self.Str, data.str)
	self.ArrowObj:SetActive(data.show_arrow)
	if data.show_arrow then 
	    UH.SpriteName(self.ArrowImg, data.is_up and "up_s" or "down_s")		
	end 
end

ItemInfoViewEquipSmelt = ItemInfoViewEquipSmelt or DeclareMono("ItemInfoViewEquipSmelt", ItemInfoViewItem)
function ItemInfoViewEquipSmelt:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local has_inc, inc_list = self.item1:IncAttr()
	UH.SetText(self.Title, self.language.ItemTitles.SmeltBase)

	local temp = ""
	local temp2 = ""
	local attr_index = 0

	if has_inc then 
		for i = 1,6 do
			attr_index = attr_index + 1
			self.PointObjs[i]:SetActive(DataHelper.IsPoint(inc_list[i].attr_type))
			if inc_list[i].attr_value > 0 then
			    local str = Format(Language.ItemInfo.ItemTitles.EquipAttr3, 
				    QualityColorStr[inc_list[i].color],
			        Language.Common.AttrList[inc_list[i].attr_type],
				    DataHelper.IsPercent(inc_list[i].attr_type) and Percent(inc_list[i].attr_value) or inc_list[i].attr_value)
			    if attr_index % 2 == 0 then 
				    temp2 = temp2 == "" and str or temp2 .."\n" ..str
			    elseif attr_index % 2 == 1 then  
				    temp = temp == "" and str or temp .."\n" ..str
				end 
			end
		end 
	end 

	if temp == "" then 
		temp = self.language.Wu  
	end 

	UH.SetText(self.Content, temp)
	UH.SetText(self.IncValue, temp2)
end 

ItemInfoViewEquipSet = ItemInfoViewEquipSet or DeclareMono("ItemInfoViewEquipSet", ItemInfoViewItem)
function ItemInfoViewEquipSet:ItemInfoViewEquipSet()
	ItemInfoViewItem.ItemInfoViewItem(self)
	self.DescList:SetCompleteCallBack(function()
		self.contentRect.enabled = true
    end)
end 

function ItemInfoViewEquipSet:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, self.language.ItemTitles.EquipSet)
	local height = 125
	self.contentRect.enabled = false
	local is_mine = BagData.Instance:CheckSameEquipByUniqueIdForAllIn(self.item1)
	if is_mine then 
		local suit_id = self.item1.param ~= nil and self.info.item_list[self.ShowType].param.new_suit_effect_id or self.info.item_list[self.ShowType]:Config().suitgroup_id
		local list = EquipSetData.Instance:GetItemInfoEquipSetShow(suit_id,true,self.item1:Color())
		self.DescList:SetData(list)
		height = #list > 1 and height or 80
	else
		local suit_id = self.item1.param ~= nil and self.info.item_list[self.ShowType].param.new_suit_effect_id or self.info.item_list[self.ShowType]:Config().suitgroup_id
		local list = EquipSetData.Instance:GetItemInfoEquipSetShowOther(suit_id,true,self.item1:Color())
		self.DescList:SetData(list)
		height = #list > 1 and height or 80
	end 

	self.content.preferredHeight = height
end 

function ItemInfoViewEquipSet:OnDisable()
	ItemInfoViewItem.OnDisable(self)
end 

ItemInfoEquipSetDescItem = ItemInfoEquipSetDescItem or DeclareMono("ItemInfoEquipSetDescItem", UIWidgetBaseItem)
function ItemInfoEquipSetDescItem:ItemInfoEquipSetDescItem() end
function ItemInfoEquipSetDescItem:SetData(data) 
	UIWidgetBaseItem.SetData(self,data)
	local temp = data.desc_title .."\n"..data.attr_desc
	if data.show_sp then 
	    temp = temp .."\n"..data.sp_effect
	end 
	UH.SetText(self.Text, temp)
	if self.Text_real then UH.SetText(self.Text_real, temp) end 
end 

ItemInfoViewEquipSpecial = ItemInfoViewEquipSpecial or DeclareMono("ItemInfoViewEquipSpecial", ItemInfoViewItem)
function ItemInfoViewEquipSpecial:OnEnable()
	ItemInfoViewItem.OnEnable(self)

	UH.SetText(self.Title, self.language.ItemTitles.Special)
	local temp = ""
	local temp2 = nil
	if self.item1.param.sp_id and self.item1.param.sp_id > 0 then
		local vo_cfg = Cfg.MedalEffectById(self.item1.param.sp_id)
		if vo_cfg ~= nil then 
			temp = Format(Language.Common.Xstr, QualityColorStr[self.item1:GetMedalQua()], self.item1:EffectDesc())
		else 
			temp = Language.Medal.SpecialSoloNoColor..self.language.None  
		end 
	else 
		temp = Language.Medal.SpecialSoloNoColor..self.language.None  
	end

	if self.item1.param.second_sp_id and self.item1.param.second_sp_id > 0 then
		local vo_cfg = Cfg.MedalEffectById(self.item1.param.second_sp_id)
		if vo_cfg ~= nil then 
			temp2 = Format(Language.Common.Xstr, QualityColorStr[self.item1:GetMedalSecondQua()], self.item1:Effect2Desc())
		end 
	end 

	UH.SetText(self.Content, temp)
	self.Content2.gameObject:SetActive(temp2~=nil)
	UH.SetText(self.Content2, temp2)
	self.FailObj:SetActive(self.item1.param.sp_fail)
	self.Fail2Obj:SetActive(self.item1.param.sp_fail_2)
end

ItemInfoViewPetEquip = ItemInfoViewPetEquip or DeclareMono("ItemInfoViewPetEquip", ItemInfoViewItem)
function ItemInfoViewPetEquip:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, self.language.ItemTitles.PetEquip)
	local temp = ""
	if self.item1.param.equipment_skill_list == nil then
		temp = Language.PetEquipSelect.ActiveWhenGet
	else
		for i=1, MAX_PET_EQUIPMENT_SKILL_NUM do
			if self.item1.param.equipment_skill_list[i] ~= 0 then
				if i >= 2 then
					temp = temp .. "\n"
				end
				local skill_id = self.item1.param.equipment_skill_list[i]
				local level = AmuletData.Instance:GetIntensifySkillLevel(skill_id)
				local skill_cfg = PetData.Instance:GetGiftDetail(skill_id, level or 1)
				if not skill_cfg then
					ViewMgr:CloseView(ItemInfoView)
					PublicPopupCtrl.Instance:Center("宠物护符道具异常，如有需要请与客服反馈")
					BuglyReport("宠物护符道具属性错误 skill_info == ", self.item1.param.equipment_skill_list[i])
					return
				end
				local name = skill_cfg.name
				local qua = skill_cfg.page
				if level and level > 0 then
					name = string.format(Language.Pet.PetEquip.LevelShow,name, level)
					qua = 3
				end
				temp = temp .. Format(Language.PetEquipSelect.SkillNameColor, Language.PetEquipSelect.SkillColorD[qua], name)
			end
		end
	end

	UH.SetText(self.Content, temp)
end 

ItemInfoViewEquipStone = ItemInfoViewEquipStone or DeclareMono("ItemInfoViewEquipStone", ItemInfoViewItem)

function ItemInfoViewEquipStone:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	self.limit_info = WorkshopData.Instance:GetStoneLimitLv()
	local is_mine = BagData.Instance:CheckSameEquipByUniqueIdForAllIn(self.item1)
	local level = is_mine and RoleData.Instance:GetRoleLevel() or self.limit_info[EQUIP_STONE_COUNT_MAX].limit_level
	local small_stone_data = {}
	if self.item1.param ~= nil then
		local stone_list = self.item1.param.stone or {}
		for i = 1, EQUIP_STONE_COUNT_MAX do
			local t = {}
			local info = WorkshopData.Instance:GetStoneInfo(stone_list[i])
			if info then
				local acc_tianming_plus = is_mine and AccrssoryData.Instance:GetDestinyRate(TIAN_MING_ADD_ATTR_SYS_TYPE.GEM) or 0
				t.icon_id = Item.GetIconId(stone_list[i])
				t.attr = Format("%s  +%s", Language.Common.AttrList[info.type_1], info.value_1) 
				    .. ((acc_tianming_plus > 0 and math.floor(info.value_1 * acc_tianming_plus) > 0 ) 
					and "<color=#"..COLORSTR.Green3.."> +".. math.floor(info.value_1 * acc_tianming_plus).."</color>" or "")
			else
				t.attr = level < self.limit_info[i].limit_level and self.language.Lock or self.language.None
			end

			t.is_lock = not WorkshopData.Instance:GetStoneSortLimit(self.item1,i)
			table.insert(small_stone_data, t)
		end
	end
	self.SmallStoneList:SetData(small_stone_data)
end


ItemInfoViewEquipSmallStone = ItemInfoViewEquipSmallStone or DeclareMono("ItemInfoViewEquipSmallStone", ItemInfoViewItem)
function ItemInfoViewEquipSmallStone:SetData(data)
	UH.SetIcon(self.ImgSetIcon, data.icon_id)
	self.ObjLock:SetActive(data.is_lock)
	UH.SetText(self.TxtAttr, data.attr)
end

ItemInfoViewCreateName = ItemInfoViewCreateName or DeclareMono("ItemInfoViewCreateName", ItemInfoViewItem)
function ItemInfoViewCreateName:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local temp = ""
	if self.item1.param ~= nil then
		local temp_name = self.language.NoneName
		if not StringIsEmpty(self.item1.param.creater_name) and self.item1.param.creater_name ~= nil then 
			local names_  = Split(self.item1.param.creater_name,"_s")
			temp_name = names_[1]
		end 

		if self.item1.param.anima_val then
			temp = self.item1.param.anima_val == INT_MAX and self.language.WuXian or self.item1.param.anima_val
			temp = Format(self.language.ItemTitles.JewelryName, temp)
			temp = Format("%s\n%s", temp, Format(self.language.ItemTitles.CreateName, temp_name))
		elseif self.item1.param.remain_times then
			temp = Format(self.language.ItemTitles.ExpCrystal, self.item1.param.remain_times)
		else
			temp = Format(self.language.ItemTitles.CreateName, temp_name)
		end
		UH.SetText(self.Title, temp)
	end
end

ItemInfoViewLimitTime = ItemInfoViewLimitTime or DeclareMono("ItemInfoViewLimitTime", ItemInfoViewItem)
function ItemInfoViewLimitTime:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local config = BagData.Instance:GetActItemConfig(self.item1.item_id)
    if self.item1.invalid_time and self.item1.invalid_time > 0 then
		self.Timer:StampTime(self.item1.invalid_time,TimeType.Type_Time_3)
		return
	end
	if config ~= nil then
		if config.activity_id == ACTIVITY_TYPE.RAND_GOD_TOKEN then
			self.Timer:StampTime(GodTokenData.Instance:GetEndTime(),TimeType.Type_Time_3)
		elseif config.activity_id == ACTIVITY_TYPE.RAND_ANIMAL_WORLD then
			local time_info = AnimalWorldData.Instance:TimeInfo()
			if not TableIsEmpty(time_info) then
				self.Timer:StampTime(time_info.end_time, TimeType.Type_Special_9)
			end
		else
			self.Timer:CreateActTime(config.activity_id,TimeType.Type_Time_3)
		end
	end
end 

ItemInfoViewJewelrySuit = ItemInfoViewJewelrySuit or DeclareMono("ItemInfoViewJewelrySuit", ItemInfoViewItem)
function ItemInfoViewJewelrySuit:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local operate_item = self.item1.ActiveSuitInfo == nil and Item.Create(self.item1) or self.item1
	local is_active, suit_info = operate_item:ActiveSuitInfo()
	UH.SetText(self.Title, is_active and self.language.Jewelry.Suit2 or self.language.Jewelry.Suit)

	local suit = ""
	if suit_info then
		if suit_info.is_pe then
			suit = suit_info.content
		else
			for i = 1, 3 do
				local str = Format("%s+%d", self.language.AttrList[suit_info["param_" .. i]], suit_info["attr_" .. i])
				suit = suit .. string.joint(i, str, " ")
			end
		end
	end
	UH.SetText(self.Content, suit)
	UH.Color(self.Content, is_active and COLORS.Green3 or COLORS.Gray2)
end

ItemInfoViewJewelryLegend = ItemInfoViewJewelryLegend or DeclareMono("ItemInfoViewJewelryLegend", ItemInfoViewItem)
function ItemInfoViewJewelryLegend:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local is_active = self.item1:ActiveLegendInfo()
	local suit_info = self.item1:SuitInfo()
	local lege = ""
	if suit_info then
		if suit_info.legend_attr == 0 then
			UH.SetText(self.Title, self.language.Jewelry.Legend)
			lege = Language.Workshop.LegendAttr[suit_info.legend_attr]
		else
			UH.SetText(self.Title, is_active and self.language.Jewelry.Legend2 or self.language.Jewelry.Legend1)
			lege = Format(Language.Workshop.Legend, Language.Workshop.LegendAttr[suit_info.legend_attr], "+" .. suit_info.legend_percent .. "%")
			UH.Color(self.Content, is_active and COLORS.Green3 or COLORS.Gray2)
		end
	else
		UH.SetText(self.Title, self.language.Jewelry.Legend)
		lege = Language.Workshop.LegendAttr[0]
	end
	UH.SetText(self.Content, lege)
end

ItemInfoViewPointAttr = ItemInfoViewPointAttr or DeclareMono("ItemInfoViewPointAttr", ItemInfoViewItem)
function ItemInfoViewPointAttr:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, self.language.ItemTitles.PointAttr)
	local has_inc, inc_list = self.item1:IncAttr()
	local temp = ""
	local point_attr = self.item1.param.attr_extra_inc
	if self.item1.param.inc_num > 1 then
		temp = Format(self.language.ItemTitles.PointAttr2, 
		Language.Role.AddPointTypeName[point_attr[1].attr_type],point_attr[1].attr_value,
		Language.Role.AddPointTypeName[point_attr[2].attr_type] .. "+" .. point_attr[2].attr_value)
	else
		temp = Format(self.language.ItemTitles.PointAttr2, 
		Language.Role.AddPointTypeName[point_attr[1].attr_type],point_attr[1].attr_value,
		"")
	end
	UH.SetText(self.Content, temp)
end

ItemInfoViewModelShow = ItemInfoViewModelShow or DeclareMono("ItemInfoViewModelShow", ItemInfoViewItem)

function ItemInfoViewModelShow:OnDisable()
	ItemInfoViewItem.OnDisable(self)
	self.ModelBlock:SetActive(false)
	TreasureHuntData.Instance.click_skin = false
end

function ItemInfoViewModelShow:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	self.ModelBlock:SetActive(true)
	self.ModelUi:SetData({item_config = self.info.item_list[1]:Config()})
end
function ItemInfoViewModelShow:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end
-- 用于显示模型
ModelUiShow = ModelUiShow or DeclareMono("ModelUiShow", UIWidgetBaseItem)

function ModelUiShow:ModelUiShow()
	self.ui_obj_show = UIObjDrawer.New()
    self.Model:SetShowData({
        ui_obj = self.ui_obj_show,
        view = self,
        seq = 1,
        show_shadow = false,
    })
    self.ui_role_show = UIChDrawer.New()
    self.RoleModel:SetShowData({
        ui_obj = self.ui_role_show,
        view = self,
        seq = 2,
        show_shadow = false,
    })
end

function ModelUiShow:SetData(data)
	if not data.item_config and not data.item_id then
		return
	end
	if self.TreasureHuntSkinEffObj then
		self.TreasureHuntSkinEffObj:SetActive(false)
	end
	local item_cfg = data.item_config or Item.New(data.item_id):Config()
    self.Model.gameObject:SetActive(false)
    self.RoleModel.gameObject:SetActive(false)
    self.ImgSetImgModel.gameObject:SetActive(false)
    self.AppearanceBubble.gameObject:SetActive(false)
	self.TitleEffectRoot:SetActive(false)
    if item_cfg.item_type == ItemType.Fashion then
        local surface_cfg = Cfg.SurfaceInfoById(item_cfg.param1)
        if surface_cfg and surface_cfg.res_id ~= 0 then
            if surface_cfg.type == IllusionConfig.FashionType.mount then
                -- 坐骑单独显示
                self.Model.gameObject:SetActive(true)
                self.ui_obj_show:SetData(DrawerHelper.GetRidePath(surface_cfg.res_id))
            else
                -- 武器、羽翼、角色时装
                self.RoleModel.gameObject:SetActive(true)
                local appearance = TableCopy(RoleData.Instance:GetApearance())
                if surface_cfg.type == IllusionConfig.FashionType.cloth then
                    appearance.color = 0
                    appearance.fashion_body_id = surface_cfg.res_id
                    appearance.special_appearance_type = 0
                    appearance.special_appearance_param = 0
                elseif surface_cfg.type == IllusionConfig.FashionType.weapon then
                    appearance.weapon_id = 2001 + RoleData.Instance:ProfType() * 100
                    appearance.fashion_weapon_id = surface_cfg.res_id
                end
                self.ui_role_show:SetData(appearance)
            end
        elseif DEBUG_MODE then
            LogError("幻化配置为空 id：", item_cfg.param1, "物品id：", item_cfg.id, "配置：", item_cfg)
        end
    elseif item_cfg.item_type == ItemType.Skin then
        -- 角色、宠物、伙伴皮肤需要带上模型
        local skin_cfg = nil
        if item_cfg.show_type == ShowType.PetSkin then
            skin_cfg = Cfg.PetSkinAttr(item_cfg.param1)
            if DEBUG_MODE and skin_cfg == nil then
				LogError("宠物皮肤物品",item_cfg.id,"的皮肤配置为空")
			end
			if self.TreasureHuntSkinEffObj then
				self.TreasureHuntSkinEffObj:SetActive(TreasureHuntData.Instance.click_skin)
			end
        elseif item_cfg.show_type == ShowType.PartnerSkin then
            skin_cfg = PartnerData.Instance.partner_skin_ids[item_cfg.param2]
            if DEBUG_MODE and skin_cfg == nil then
				LogError("伙伴皮肤物品",item_cfg.id,"的皮肤配置为空")
			end
        end
        if skin_cfg and skin_cfg.res_id then
            self.Model.gameObject:SetActive(true)
            self.ui_obj_show:SetData(DrawerHelper.GetNpcPath(skin_cfg.res_id))
        end
    elseif item_cfg.item_type == ItemType.Appearance then
        -- 颜值物品（头衔、气泡框、尾标）
        local appearance_cfg = Config.face_cfg_auto.resource[item_cfg.param1]
        if appearance_cfg and appearance_cfg.res_type and appearance_cfg.res_id then
            local icon_type = AppearanceConfig.IconType[appearance_cfg.res_type]
            local icon_pre = AppearanceConfig.IconPre[appearance_cfg.res_type]
            if appearance_cfg.res_type == AppearanceConfig.Type.Bubble then
                self.AppearanceBubble.gameObject:SetActive(true)
                self.AppearanceBubble:SetData(appearance_cfg.res_id, Vector2.New(174, 65))
			elseif appearance_cfg.res_id ~= 0 then
				if appearance_cfg.res_type == AppearanceConfig.Type.Title then
					local title_cfg = Config.titleconfig_auto.title_list[appearance_cfg.title_id]
					if title_cfg and title_cfg.title_id_1 > 0 then
						if self.handle_title then
							self.EffectTool:Stop(self.handle_title)
						end
						self.handle_title = self.EffectTool:PlayByEffectId(title_cfg.title_id_1, self.TitleEffectRoot)
						self.TitleEffectRoot:SetActive(true)
						self.ImgSetImgModel.gameObject:SetActive(false)
					else
						self.ImgSetImgModel.gameObject:SetActive(true)
						UH.SetIcon(self.ImgSetImgModel, icon_pre .. appearance_cfg.res_id,icon_type)
					end
				else
					self.ImgSetImgModel.gameObject:SetActive(true)
					UH.SetIcon(self.ImgSetImgModel, icon_pre .. appearance_cfg.res_id,icon_type)
				end
            end
        end
	elseif item_cfg.item_type == ItemType.PetWeapon then
		local weapon_config = Cfg.PetWeaponBaseCfg(item_cfg.id)
		if weapon_config then
			self.Model.gameObject:SetActive(true)
			self.ui_obj_show:SetData(DrawerHelper.GetWeaponPath(weapon_config.pet_weapon_res))
		end
	elseif item_cfg.show_type == ShowType.Pet then
		local pet_cfg = PetData.Instance:GetPetCfg(item_cfg.id)
		if pet_cfg and pet_cfg.res_id then
			self.Model.gameObject:SetActive(true)
			self.ui_obj_show:SetData(DrawerHelper.GetNpcPath(pet_cfg.res_id))
		end
	elseif item_cfg.show_type == ShowType.FairyDragonSkin then
		local skin_cfg = FairyDragonData.Instance:GetSkinCfgByItem(item_cfg.id)
		if skin_cfg and skin_cfg.res_is then
			self.Model.gameObject:SetActive(true)
			self.ui_obj_show:SetData(DrawerHelper.GetNpcPath(skin_cfg.res_is))
		end
    end
end

function ModelUiShow:Destroy()
	if self.handle_title then
		self.EffectTool:Stop(self.handle_title)
		self.handle_title = nil
	end
    UIWFlushPanel.Destroy(self)
    Delete(self, "ui_obj_show")
    Delete(self, "ui_role_show")
end


ItemInfoViewIllusionBase = ItemInfoViewIllusionBase or DeclareMono("ItemInfoViewIllusionBase", ItemInfoViewItem)
function ItemInfoViewIllusionBase:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local item_cfg = self.item1:Config()
	local title_str = self.language.ItemTitles.IllusionBase
	local count = 1

	if item_cfg.item_type == ItemType.Skin then
		title_str = self.language.ItemTitles.SkinEquipAttr
		--获取对应皮肤的属性加成
		if item_cfg.show_type == ShowType.PetSkin then  --宠物皮肤
			local pet_skin_cfg = Cfg.PetSkinAttr(item_cfg.param1)
			if pet_skin_cfg then
				for k,v in pairs(PetData.PetGrowthKey) do
					if pet_skin_cfg[k] ~= 0 then
						local str = Format("%s+%d",Language.Pet.AttrClass[v][4], pet_skin_cfg[k])
						UH.SetText(self.attr_cell_str[count], str)
						count = count + 1
					end
				end
				if pet_skin_cfg.potential and pet_skin_cfg.potential ~= 0 then
					UH.SetText(self.attr_cell_str[count], Language.Pet.Potential.."+"..pet_skin_cfg.potential)
					count = count + 1
				end
			end
		elseif item_cfg.show_type == ShowType.PartnerSkin then  --伙伴皮肤
			local partner_skin_cfg = PartnerData.Instance.partner_skin_ids[item_cfg.param2]
			if partner_skin_cfg then
				for k,v in pairs(AttrNameForType) do
			    	if partner_skin_cfg[v] ~= nil and partner_skin_cfg[v] ~= 0 then
				    	local str = Format(self.language.ItemTitles.IllusionAttr, Language.Illusion.AttrNames[v][1], DataHelper.Percent(k, partner_skin_cfg[v]))
						UH.SetText(self.attr_cell_str[count], str)
						count = count + 1
					end
			    end
			end
		end
	elseif item_cfg.item_type == ItemType.Appearance then --颜值物品（气泡框、尾标、称号）
		title_str = self.language.ItemTitles.FacePreview
		UH.SizeDelta(self.LineRect, Vector2.New(238, 12))
		--获取颜值下一等级的属性加成
		local level = AppearanceData.Instance:GetAppearancInfo().face_level + 1
		local add_data = AppearanceData.Instance:GetCurAddData(level)
		for k,v in pairs(add_data.add_list) do
			UH.SetText(self.attr_cell_str[count], v.attr_name .. "+" .. v.attr_value)
			count = count + 1
		end
	elseif item_cfg.item_type == ItemType.Fashion then
		--时装属性专精特效
	    local surface_cfg = Cfg.SurfaceValue(item_cfg.param1, RoleData.Instance:GetRoleProf())
		for k,v in pairs(AttrNameForType) do
	    	if surface_cfg[v] ~= nil and surface_cfg[v] ~= 0 then
				local str = Format(self.language.ItemTitles.IllusionAttr, Language.Illusion.AttrNames[v][1], DataHelper.Percent(k, surface_cfg[v]))
				UH.SetText(self.attr_cell_str[count], str)
				count = count + 1
			end
	    end
	end
	UH.SetText(self.Title, title_str)
    for i = count, 6 do
    	UH.SetText(self.attr_cell_str[i], "")
    end
    TimeHelper:AddDelayFrameTimer(function()
    	self.EmptyElement.minHeight = 344 - self.DescLayout.preferredHeight - self.LayoutG.preferredHeight
    end,2)
end

function ItemInfoViewIllusionBase:OnDisable()
	self.EmptyElement.minHeight = 0
end

--出售价格
ItemInfoViewPrice = ItemInfoViewPrice or DeclareMono("ItemInfoViewPrice", ItemInfoViewItem)
function ItemInfoViewPrice:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Price,Format(self.language.ItemPriceTip,self.item1:Price()))
	UH.SetGold(self.Gold,CurrencyType.CoinBind)
end

--图鉴组卡片
ItemInfoViewCollect = ItemInfoViewCollect or DeclareMono("ItemInfoViewCollect", ItemInfoViewItem)

function ItemInfoViewCollect:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local list = IllustrateData.Instance:GetCollectGroupData(self.item1.item_id)
	self.ELement.preferredHeight = #list * 35
	self.Grid:SetData(list)
end

function ItemInfoViewCollect:OnDisable()
	ItemInfoViewItem.OnDisable(self)
	self.CollectGroup:SetActive(false)
	self.ELement.preferredHeight = 0
end

function ItemInfoViewCollect:OnClick(data)
	if not self.CollectGroup.activeSelf then
		self.CollectGroup:SetActive(true)
	end
	ItemInfoCtrl.Instance:Data():SetCollectGroupData(data)
end

----------------------------ItemInfoViewCollectItem----------------------------
ItemInfoViewCollectItem = ItemInfoViewCollectItem or DeclareMono("ItemInfoViewCollectItem", UIWidgetBaseItem)

function ItemInfoViewCollectItem:SetData(data)
	local item_num = Item.GetNum(data.item_id)
	item_num = item_num >= data.need_num and data.need_num or item_num
	local is_active = IllustrateData.Instance:GetActiveSeq(data.seq)
	local COLOR = is_active and QualityColorStr[data.color] or COLORSTR.Gray4
	local name_str = Format("[%s]%s",data.page_name,data.name)
	UH.SetText(self.Name,RichTextHelper.LinkStr(ColorStr(name_str,COLOR),"_ul",COLOR))
	local num_str = is_active and Language.Illustrate.ItemActiveTip or Format("(%s/%s)",item_num,data.need_num)
	UH.SetText(self.Num,ColorStr(num_str,is_active and COLORSTR.Green3 or COLORSTR.Gray4))
	UH.SpriteName(self.Image,"CollectGroup" .. data.color)
    UIWidgetBaseItem.SetData(self, data)
end

ItemInfoViewCollectGroup = ItemInfoViewCollectGroup or DeclareMono("ItemInfoViewCollectGroup", ItemInfoViewItem)

function ItemInfoViewCollectGroup:ItemInfoViewCollectGroup()
	self.data_cares = {
        {data = self.data.collect_group_data,  func = self.FlushCollectGroupView,  init_call = true},
    }
end

function ItemInfoViewCollectGroup:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local width = self.CollectRect.sizeDelta.x
	local height = self.ParentRect.sizeDelta.y - 4
	self.CollectRect.sizeDelta = Vector2.New(width,height)
end

function ItemInfoViewCollectGroup:FlushCollectGroupView()
	local data = self.data:GetCollectGroupData()
	local config = IllustrateData:GetCollectConfig(data.seq)
	local attr_name = ""
	for i,v in ipairs(IllustrateData.Instance:GetAttrItemList(config)) do
		attr_name = attr_name .. v.name .. (i==2 and "\n" or "  ") 
	end
	UH.SetText(self.Title,ColorStr(data.name,QualityColorStr[data.color]))
	UH.SetText(self.Desc,attr_name)
	self.Grid:SetData(IllustrateData.Instance:GetActiveItemList(config))
	self.config = config
end

function ItemInfoViewCollectGroup:OnClickGo()
	if not ViewMgr:IsOpen(IllustrateView) then
		IllustrateData.Instance:SetSelectType(self.config.page)
		IllustrateData.Instance:SetSelectSeq(self.config.seq)
		ViewMgr:OpenViewByKey(Mod.Illustrate.Main)
	else
		IllustrateData.Instance:SetSelectSeq(self.config.seq)
		IllustrateData.Instance:SelectFunc(self.config.page)
	end
	ViewMgr:CloseView(ItemInfoView)
end

----------------------------ItemInfoViewCollectGroupItem----------------------------
ItemInfoViewCollectGroupItem = ItemInfoViewCollectGroupItem or DeclareMono("ItemInfoViewCollectGroupItem", UIWidgetBaseItem)

function ItemInfoViewCollectGroupItem:SetData(data)
	local item_num = Item.GetNum(data.item_id)
	item_num = item_num >= data.num and data.num or item_num
	self.Call:SetData(Item.New({item_id = data.item_id}))
	CellFlushs.SetNum(self.Call,Format("(%s/%s)",item_num,data.num))
end

ItemInfoEffectCrystalEquip = ItemInfoEffectCrystalEquip or DeclareMono("ItemInfoEffectCrystalEquip")

function ItemInfoEffectCrystalEquip:ItemInfoEffectCrystalEquip()
	self.info_data = ItemInfoCtrl.Instance:Data()
end

function ItemInfoEffectCrystalEquip:Start()
	self.InfoPosition.localScale = Vector3.New(1, 0, 1)
	if nil ~= self.info_data.crystal_equip then
		self.Cell:SetData(self.info_data.crystal_equip)
	end
	self.ItemObj:SetActive(true)
	self.EffectTool:Play(100108194)
end

function ItemInfoEffectCrystalEquip:End()
	self.ItemObj:SetActive(false)
	self.InfoPosition.localScale = Vector3.New(1, 1, 1)
end

function ItemInfoEffectCrystalEquip:OnEffectEvent(handle, time, event)
	if EffectEvent.event_end == event then
		self.ItemObj:SetActive(false)
		self.InfoPosition.localScale = Vector3.New(1, 1, 1)
		self.EffectTool:Play(100108195)
		self.info_data.crystal_equip = nil
	end
end

-------------------------ItemInfoViewGetWay----------------------------------------
ItemInfoViewGetWay = ItemInfoViewGetWay or DeclareMono("ItemInfoViewGetWay", ItemInfoViewItem)
function ItemInfoViewGetWay:ItemInfoViewGetWay()
	ItemInfoViewItem.ItemInfoViewItem(self)

	self.data_cares = {
        {data = self.data.get_way_show,  func = self.FlushInfoView,  init_call = false},
    }
end
function ItemInfoViewGetWay:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	-- UH.SetText(self.Title, self.language.ItemTitles.GetWay)
	local way_list = self.item1:GetWay()
	if way_list ~= nil and next(way_list) ~= nil then
		table.sort(way_list,function(a,b)
			local a_sort, b_sort = tonumber(a), tonumber(b)
			local info_a = BagData.Instance:GetWayCfg(a)
			if tonumber(info_a.value_add) ~= nil then 
                a_sort = a_sort + info_a.value_add
            end 
			if StringIsEmpty(info_a.npc) and StringIsEmpty(info_a.open_panel) then 
				a_sort = a_sort + 10000
			end 
			local info_b = BagData.Instance:GetWayCfg(b)
			if tonumber(info_b.value_add) ~= nil then 
                b_sort = b_sort + info_b.value_add
            end 
			if StringIsEmpty(info_b.npc) and StringIsEmpty(info_b.open_panel) then 
				b_sort = b_sort + 10000
			end
			if not self:IsFunIsOpened(a) or not GetWayData.GetWayIsOpen(info_a) then
				a_sort = a_sort + 100000
			end
			if not self:IsFunIsOpened(b) or not GetWayData.GetWayIsOpen(info_b) then
				b_sort = b_sort + 100000
			end
			return a_sort < b_sort
		end)
		self.Functions:SetData(way_list)
	end
	self.Functions:SetCompleteCallBack(function()
		-- 分类讨论，选择性进行重定位 20211218c
		if self.ViewMono.Offset[self.ViewMono.InfoPosition.sizeDelta.x] == 200 
		    or self.ViewMono.Offset[self.ViewMono.InfoPosition.sizeDelta.x] == 85 then 
		    self.ViewMono:AutoToPos(nil,true)
		end 
	end)
end 

function ItemInfoViewGetWay:GetGotoMarkBtn1()
	if not self.Functions:SetupCompleted() then
		return
	end
	for i=1, self.Functions:GetItemCount() do
		local item = self.Functions:GetItem(i)
		local item_data = item:GetData()
		if [[string]] == type(item_data) and item_data == "315" then	--315是前往市场的按钮
			return item.Goto
		end
	end
end

function ItemInfoViewGetWay:OnDisable()
	ItemInfoViewItem.OnDisable(self)
	self.ViewMono:AutoToPos()
end 

-- 增加一个收回去的判定 20220629c
function ItemInfoViewGetWay:FlushInfoView()
	if not ItemInfoData.Instance.get_way_show.val then 
		TimeHelper:CancelTimer(self.getway_timer)
		self.getway_timer = TimeHelper:AddDelayTimer(function() 
			self.ViewMono:AutoToPos(nil,true)
		end,0.2)
	end 
end

function ItemInfoViewGetWay:IsFunIsOpened(data)
    local info = BagData.Instance:GetWayCfg(data)
	local plus_open_func = GetWayPlusFunOpen[info.open_panel] or GetWayPlusFunOpen[ModToModule(info.open_panel == "" and 0 or info.open_panel)]
    return FunOpen.Instance:GetFunIsOpened(info.open_panel) and not (plus_open_func~=nil and plus_open_func(info,self.item1))
end
function ItemInfoViewGetWay:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end

----------------------------ItemInfoViewCastSoul----------------------------
ItemInfoViewCastSoul = ItemInfoViewCastSoul or DeclareMono("ItemInfoViewCastSoul", ItemInfoViewItem)
function ItemInfoViewCastSoul:ItemInfoViewCastSoul()
	self.data_cares = {
        {data = EquipData.Instance.equip_detail_data,  func = self.FlushDetailBtn,  init_call = false},
    }
end

function ItemInfoViewCastSoul:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local data = {}
	local resonance_config = self.item1:GetSpiritResonance()
	self.ObjHasResonance:SetActive(resonance_config ~= nil)
	self.ObjNotResonance:SetActive(resonance_config == nil)
	if resonance_config then
		UH.SetText(self.TxtResonance, resonance_config.combine_config.group_name)
		self.CircleList:SetData(EquipData.Instance:GetColorListByCombine(resonance_config.combine_config))
	else
		self.CircleList:SetData({})
	end
	self.SpiritLevelList:SetData(self.item1:GetHasSpiritList())
	self.Toggle.isOn = false
end

function ItemInfoViewCastSoul:OnDetailClick(is_select)
	local param_t = {cell = self.item1, pos = UH.GetRootPosition(self.gameObject)}
	param_t.pos = self.ShowType == 2 and param_t.pos + Vector2.New(436,0) or param_t.pos
	if is_select then
		EquipData.Instance.equip_detail_data.is_on_1 = self.ShowType == 1
		EquipData.Instance.equip_detail_data.is_on_2 = self.ShowType == 2
	else
		EquipData.Instance.equip_detail_data.is_on_1 = false
		EquipData.Instance.equip_detail_data.is_on_2 = false
	end

	if is_select then
		local is_opened_spirit_view = ViewMgr:IsOpen(ItemPreRangeaView)
		if ItemInfoCtrl.Instance:Data().derive_view and ViewMgr:IsOpen(ItemPreRangeaView) then 
			EquipData.Instance.equip_detail_data.info = param_t.cell
		else
			ItemInfoCtrl.Instance:ItemDeriveView()
			ItemInfoCtrl.Instance:ItemDeriveView(ItemPreRangeaView, param_t)
		end
	else
		ItemInfoCtrl.Instance:ItemDeriveView()
	end
end

function ItemInfoViewCastSoul:FlushDetailBtn()
	if self.ShowType == 1 then
		self.Toggle.isOn = EquipData.Instance.equip_detail_data.is_on_1
	elseif self.ShowType == 2 then
		self.Toggle.isOn = EquipData.Instance.equip_detail_data.is_on_2
	end
end

function ItemInfoViewCastSoul:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end

----------------------------ItemInfoItemSpirit----------------------------
ItemInfoItemSpirit = ItemInfoItemSpirit or DeclareMono("ItemInfoItemSpirit", UIWidgetBaseItem)

function ItemInfoItemSpirit:SetData(data)
	local config = Item.GetConfig(data.item_id)
    UH.SpriteName(self.ImgSetQuaIcon, COLOR_FRAME_SPRITE[config.color])
    UH.SetIcon(self.ImgSetIcon, Item.GetIconId(data.item_id))
    UH.SetText(self.TxtLevel, Format(Language.Equip.EquipStoneLevel2, data.level + 1))
end

----------------------------ItemInfoViewSpiritStone----------------------------
ItemInfoViewSpiritStone = ItemInfoViewSpiritStone or DeclareMono("ItemInfoViewSpiritStone", ItemInfoViewItem)

function ItemInfoViewSpiritStone:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, self.language.ItemTitles.SpiritStone)
	local config = EquipData.Instance:SpiritLevelAttr(self.item1.param.item_id, self.item1.param.level)
	self.List:SetData(config)
end

----------------------------ItemInfoViewSpiritAttr----------------------------
ItemInfoViewSpiritAttr = ItemInfoViewSpiritAttr or DeclareMono("ItemInfoViewSpiritAttr", UIWidgetBaseItem)

function ItemInfoViewSpiritAttr:SetData(data)
	if data.txt then
		UH.SetText(self.TxtAttr, data.txt)
	else
		UH.SetText(self.TxtAttr, data.attr_name .. "  " .. data.attr_percent_value)
	end
	self.ObjPoint:SetActive(DataHelper.IsPoint(data.attr_type))
end

----------------------------ItemInfoViewSocietySend----------------------------
ItemInfoViewSocietySend = ItemInfoViewSocietySend or DeclareMono("ItemInfoViewSocietySend", ItemInfoViewItem)

function ItemInfoViewSocietySend:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	self.max_num_send = SocietyData.Instance:GetSendRemainTimes()
	self.max_num = self.item1.num
	self.cur_num = 1
	self:FlushInfo()
end

function ItemInfoViewSocietySend:FlushInfo()
	self.CurNum.text = self.cur_num
end

function ItemInfoViewSocietySend:OnClickAdd()
	if self.max_num > self.cur_num and self.max_num_send > self.cur_num then
		self.cur_num = self.cur_num + 1
		self:FlushInfo()
	else
		PublicPopupCtrl.Instance:Center(self.cur_num >= self.max_num_send and self.language.Tips.MaxNumSend or self.language.Tips.MaxNum)
	end
end

function ItemInfoViewSocietySend:OnClickSub()
	if 1 < self.cur_num then
		self.cur_num = self.cur_num - 1
		self:FlushInfo()
	else
		PublicPopupCtrl.Instance:Center(self.language.Tips.MinNum)
	end
end

function ItemInfoViewSocietySend:OnClickConfirm()
	SocietyData.Instance:SetSendInfoSendNum(self.cur_num)
	self:OnClickClose()
end

function ItemInfoViewSocietySend:OnClickCancel()
	self:OnClickClose()
end

function ItemInfoViewSocietySend:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end

-- =================ItemInfoViewNeveal=====================
ItemInfoViewNeveal = ItemInfoViewNeveal or DeclareMono("ItemInfoViewNeveal", ItemInfoViewItem)
function ItemInfoViewNeveal:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local info = self.item1:Neveal()
	local cfg = WorkshopData.Instance:GetNevealCfg(info.effect_id,info.effect_level)
	local str = "【"..cfg.effect_name.."】"..Language.Workshop.Neveal.Lv..info.effect_level.."  "..Language.Common.AttrList[cfg.bonus_type].."  +"..cfg.bonus_attributes

	UH.SetText(self.Content,str)
end

-- =================ItemInfoViewNevealSkill=====================
ItemInfoViewNevealSkill = ItemInfoViewNevealSkill or DeclareMono("ItemInfoViewNevealSkill", ItemInfoViewItem)
function ItemInfoViewNevealSkill:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local info = self.item1:Neveal()
	local is_mine = BagData.Instance:CheckSameEquipByUniqueIdForAllIn(self.item1)
	local cfg = WorkshopData.Instance:GetNevealCfg(info.effect_id,info.effect_level)
	local total_level = BagData.Instance:GetNevealSkillLevel(info.effect_id,is_mine)
	local skill_cfg = total_level > 0 and WorkshopData.Instance:GetNevealSkillCfgByUnlockLevel(info.effect_id,total_level) or WorkshopData.Instance:GetNevealSkillCfg(info.effect_id)

	local str = ColorStr(skill_cfg.skill_name,COLORSTR.Green3) 
	if total_level > 0 then 
		str = str..ColorStr("("..Language.Workshop.Neveal.Lv..skill_cfg.skill_level..")"..skill_cfg.set_skill_description,COLORSTR.Green3) 
	else
		str = str..string.format(Language.Workshop.Neveal.UnActiveTips,skill_cfg.name)
	end 
	
	UH.SetText(self.Content,str)
end

ItemInfoViewSingleBlaze = ItemInfoViewSingleBlaze or  DeclareMono("ItemInfoViewSingleBlaze", ItemInfoViewItem)
function ItemInfoViewSingleBlaze:OnEnable()
	ItemInfoViewItem.OnEnable(self)

	local blaze_cfg = WorkshopData.Instance:GetBlazeData(self.item1.item_id,self.item1:BlazeStar(), 
	    RoleData.ProfIdToProfType(Equip.GetLimitProf(self.item1.item_id)), 
		Item.GetLimitLv(self.item1.item_id)).blaze

	UH.SetText(self.Title,DataHelper.GetDaXie(self.item1:BlazeStar()-1)..self.language.ItemTitles.BlazeStar)

	for i = 1,6 do 
		local at_type = blaze_cfg["attr_type_"..i]
		local at_value = blaze_cfg["attr_min_"..i]
		self.attrlist[i].gameObject:SetActive(at_value ~= nil and at_value > 0)
		if at_value > 0 then 
			local str = Language.Common.AttrList[at_type].." "..
			    ColorStr((DataHelper.IsPercent(at_type) and Percent(at_value) or at_value),COLORSTR.Green10) 

			UH.SetText(self.attrlist[i].Str,str)
		end 
	end 
end 

ItemInfoViewBlazeList = ItemInfoViewBlazeList or  DeclareMono("ItemInfoViewBlazeList", ItemInfoViewItem)
function ItemInfoViewBlazeList:OnEnable()
	ItemInfoViewItem.OnEnable(self)

	local blaze_list = WorkshopData.Instance:GetBlazeStarList(self.item1.item_id,
	    RoleData.ProfIdToProfType(Equip.GetLimitProf(self.item1.item_id)), 
		Item.GetLimitLv(self.item1.item_id))

	for i = 2,6 do 
		self["Title"..(i-1)].gameObject:SetActive(blaze_list[i]~= nil)
		self["attr_list_"..(i-1)]:SetActive(blaze_list[i]~= nil)
		if blaze_list[i] ~= nil then 
			UH.SetText(self["Title"..(i-1)],DataHelper.GetDaXie(blaze_list[i].blaze.item_star - 1)..self.language.ItemTitles.BlazeStar)
			for j = 1,6 do 
				local at_type = blaze_list[i].blaze["attr_type_"..j]
				local at_value = blaze_list[i].blaze["attr_min_"..j]
				self["attrlist_"..(i-1)][j].gameObject:SetActive(at_value ~= nil and at_value > 0)
				if at_value > 0 then 
					local str = Language.Common.AttrList[at_type].." "..
					ColorStr((DataHelper.IsPercent(at_type) and Percent(at_value) or at_value),COLORSTR.Green10)

					UH.SetText(self["attrlist_"..(i-1)][j].Str,str)
				end 
			end 
		end 
	end 
end 

ItemInfoViewMarbasSuit = ItemInfoViewMarbasSuit or DeclareMono("ItemInfoViewMarbasSuit", ItemInfoViewItem)
function ItemInfoViewMarbasSuit:ItemInfoViewMarbasSuit()
	ItemInfoViewItem.ItemInfoViewItem(self)
	self.DescList:SetCompleteCallBack(function()
		self.contentRect.enabled = true
    end)
end 

function ItemInfoViewMarbasSuit:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, self.language.ItemTitles.EquipSet)
	local height = 125
	self.contentRect.enabled = false
	-- local is_mine = BagData.Instance:CheckSameEquipByUniqueIdForAllIn(self.item1)
	-- if is_mine then 
		local list = MarbasData.Instance:GetItemInfoMarbasSuitShow(self.item1)
		self.DescList:SetData(list)
		height = #list > 1 and height or 80
	-- else
	-- 	local suit_id = self.item1.param ~= nil and self.info.item_list[self.ShowType].param.new_suit_effect_id or self.info.item_list[self.ShowType]:Config().suitgroup_id
	-- 	local list = EquipSetData.Instance:GetItemInfoEquipSetShowOther(suit_id,true,self.item1:Color())
	-- 	self.DescList:SetData(list)
	-- 	height = #list > 1 and height or 80
	-- end 

	self.content.preferredHeight = height
end 

function ItemInfoViewMarbasSuit:OnDisable()
	ItemInfoViewItem.OnDisable(self)
end 

ItemInfoViewMarbasBathe = ItemInfoViewMarbasBathe or DeclareMono("ItemInfoViewMarbasBathe", ItemInfoViewItem)
function ItemInfoViewMarbasBathe:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, self.language.ItemTitles.MarbasBathe)

    local info = MarbasData.Instance:GetBatheInfoByType(self.item1:EquipType())
	local num_max = MarbasData.Instance:BatheAffixMaxNum(self.item1.item_id)
    for i = 1,self.solt_cell:Length() do
        local vo =  {} 
        local oper = info.slot_info[i-1]

        if oper == nil then 
            vo.attr_str = Language.Marbas.Bathe.EmptyBathe
        else 
            local affix_id = oper.now_attr.affix_id
            local level = oper.now_attr.level
            if affix_id > 0 then 
                local name, attr, icon = MarbasData.Instance:BatheAffixAttr(affix_id, level)
                local qua = Cfg.MarbasBatheAffixQua(level).quality
                local color = QualityColorStr[qua]
                vo.attr_str = Format( Language.Marbas.Bathe.Level2, level, ColorStr(name, color)).." "..
                    Format(Language.Marbas.Bathe.Attr, color, attr.attr_name, attr.attr_percent_value)
            else 
                vo.attr_str = Language.Marbas.Bathe.EmptyBathe
            end 
        end 

        self.solt_cell[i]:SetData(vo)
        self.solt_cell[i]:ShowNomal(i <= num_max)
        self.solt_cell[i]:ShowUnlock(false)
        self.solt_cell[i]:Showlock(i > num_max)
    end

end 

ItemInfoViewMarbasBlood = ItemInfoViewMarbasBlood or DeclareMono("ItemInfoViewMarbasBlood", ItemInfoViewItem)
function ItemInfoViewMarbasBlood:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, self.language.ItemTitles.MarbasBlood)

    local param = MarbasData.Instance:GetBloodPlusAttrs(self.item1)
    local str_format = "%s    %s"
	for i = 1,self.nomal_cell:Length() do 
        local flag = param.list[i] ~= nil
        self.nomal_cell[i].gameObject:SetActive(flag)
        if flag then 
            local vo = {}
            vo.str = Format(str_format, Language.Common.AttrList[param.list[i].attr_type],
                DataHelper.IsPercent(param.list[i].attr_type) and Percent(param.list[i].attr_value) or param.list[i].attr_value)
            self.nomal_cell[i]:SetData(vo)
        end 
    end 
end

ItemInfoViewMarbasGem = ItemInfoViewMarbasGem or DeclareMono("ItemInfoViewMarbasGem", ItemInfoViewItem)
function ItemInfoViewMarbasGem:OnEnable()
	ItemInfoViewItem.OnEnable(self)

	UH.SetText(self.Title, self.language.ItemTitles.MarbasGem)
    for i = 1,4 do 
        local act_flag = MarbasData.Instance:GetBloodSoltLock(self.item1,i)
        self.gem_cell[i]:SetData(Item.Create(self.item1:GetStone(i)))
        self.gem_cell[i]:ShowLock(not MarbasData.Instance:GetBloodSoltLock(self.item1,i))
    end 

    local param = MarbasData.Instance:GetBloodGemAttrs(self.item1)
    -- self.sp_eff:SetActive(param.with_sp)
    UH.SetText(self.sp_eff_str,param.with_sp and param.name or Language.Marbas.BloodNoActGem )
end

ItemInfoViewMarbasGemInfo = ItemInfoViewMarbasGemInfo or DeclareMono("ItemInfoViewMarbasGemInfo", ItemInfoViewItem)
function ItemInfoViewMarbasGemInfo:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, self.language.ItemTitles.MarbasGemInfo)
	local oper_list = self.item1.GemInfo ~= nil and self.item1:GemInfo() or {}

	local str_format = "%s    %s"
	-- LogError("oper_list",self.item1.param)
	for i = 1,self.nomal_cell:Length() do 
		local flag = oper_list[i] ~= nil and oper_list[i].attr_value > 0
		self.nomal_cell[i].gameObject:SetActive(flag) 
		if flag then 
			local vo = {}
            vo.str = Format(str_format, Language.Common.AttrList[oper_list[i].attr_type],
                DataHelper.IsPercent(oper_list[i].attr_type) and Percent(oper_list[i].attr_value) or oper_list[i].attr_value)
		
            self.nomal_cell[i]:SetData(vo)
		end 
	end 
end 

ItemInfoViewFuYuAttrInfo = ItemInfoViewFuYuAttrInfo or DeclareMono("ItemInfoViewFuYuAttrInfo", ItemInfoViewItem)
function ItemInfoViewFuYuAttrInfo:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, self.language.ItemTitles.AccrssoryItemBaseAttr)
	local attr_list = self.item1:BaseAttr()
	local attr
	for i = 1, self.attr_cell_str:Length() do
		attr = attr_list[i]
		if attr and attr.attr_value > 0 then
			UH.SetText(self.attr_cell_str[i], Format("%s %s", Language.AttrItems.AttrNameSample[attr.attr_type], attr.attr_value) )
			self.attr_cell_str[i].gameObject:SetActive(true)
		else
			self.attr_cell_str[i].gameObject:SetActive(false)
		end
	end
end

ItemInfoViewFuYuEffectInfo = ItemInfoViewFuYuEffectInfo or DeclareMono("ItemInfoViewFuYuEffectInfo", ItemInfoViewItem)
function ItemInfoViewFuYuEffectInfo:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	local partner_id = self.item1.partner_id
	local wear_idx = self.item1.wear_idx
	local sp_id = self.item1.param.sp_id
	local expired_timesstamp = self.item1.param.expired_timesstamp or 0
	UH.SetText(self.Title, self.language.Effect)
	local desc = self.item1:FuYuEffectDesc(nil, true)
	if expired_timesstamp > 0 then
		self.TimeObj:SetActive(true)
		self.TimeMetter:StampTime(expired_timesstamp, TimeType.Type_Time_0, Language.FuYu.TimeTip)
	else
		self.TimeObj:SetActive(false)
	end
	UH.SetText(self.Desc, desc)
	local extra_id = self.item1:ExtraId()
	if extra_id > 0 then
		local config = BagData.Instance:GetEquipEffectCfg(extra_id)
		local desc2 = config.description
		desc2 = self.item1:SubExtraName(desc2)
		UH.SetText(self.Desc2, desc2)
		self.Desc2.gameObject:SetActive(true)
	else
		UH.SetText(self.Desc2, "")
		self.Desc2.gameObject:SetActive(false)
	end

	if sp_id > 0 and partner_id ~= -1 and wear_idx ~= -1 then
		local partner = PartnerData.Instance:GetPartnerInfoById(partner_id)
		local config = Cfg.ParnterFuYuEffect(sp_id)
		if config and config.limit_type == 0 then
			self.NoEffect:SetActive(false)
		elseif partner and config and config.limit_type == 1 then
			self.NoEffect:SetActive(config.param1 ~= (partner.info.job * 100))
		elseif partner and config and config.limit_type == 2 then
			self.NoEffect:SetActive(config.param1 ~= partner_id)
		else
			self.NoEffect:SetActive(true)
		end
	else
		self.NoEffect:SetActive(false)
	end

	if extra_id > 0 and partner_id ~= -1 and wear_idx ~= -1 then
		local partner = PartnerData.Instance:GetPartnerInfoById(partner_id)
		local config = Cfg.ParnterFuYuEffect(extra_id)
		if config and config.limit_type == 0 then
			self.NoEffect2:SetActive(false)
		elseif partner and config and config.limit_type == 1 then
			self.NoEffect2:SetActive(config.param1 ~= (partner.info.job * 100))
		elseif partner and config and config.limit_type == 2 then
			self.NoEffect2:SetActive(config.param1 ~= partner_id)
		else
			self.NoEffect2:SetActive(true)
		end
	else
		self.NoEffect2:SetActive(false)
	end
	--检查是否在保护期
	--检查非背包是否生效

	--[[ if sp_id == 0 then
		UH.SetText(self.Desc, self.language.NoneEffect)
	else
		local config = BagData.Instance:GetEquipEffectCfg(sp_id)
		if config then
			UH.SetText(self.Desc, config.description)
		else
			LogDG("获取不到特效的配置", sp_id)
		end
	end ]]
end

ItemInfoViewYouMing = ItemInfoViewYouMing or DeclareMono("ItemInfoViewYouMing", ItemInfoViewItem)
function ItemInfoViewYouMing:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, Language.YouMingPractice.ItemInfoTitle[1])
	local cur_config = self.item1:CurConfig()
	local level = self.item1:Level()
	for i = 1, 2 do
		if self["Content"..i] then
			self["Content"..i].gameObject:SetActive(false)
		end
	end
	local attr
	local attr_list = {}
	for i = 1, 2 do
		attr = {}
		attr.attr_type = cur_config["attr_type_" .. i] or 0
		attr.attr_value = cur_config["attr_add_" .. i] or 0
		table.insert(attr_list, attr)
	end
	for i, v in ipairs(attr_list) do
		if not(v.attr_type == 0 and v.attr_value == 0) then
			if self["Content"..i] then
				self["Content"..i].gameObject:SetActive(true)
				UH.SetText(self["Content"..i], Format(Language.YouMingPractice.Attr, Language.Common.AttrList[v.attr_type], 
				DataHelper.Percent(v.attr_type, v.attr_value)))
			end
		end
	end
end

ItemInfoViewYouMing2 = ItemInfoViewYouMing2 or DeclareMono("ItemInfoViewYouMing2", ItemInfoViewItem)
function ItemInfoViewYouMing2:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, Language.YouMingPractice.ItemInfoTitle[2])
	local deputyConfig = self.item1:DeputyConfig()
	for i = 1, 6 do
		if self["Content"..i] then
			self["Content"..i].gameObject:SetActive(false)
		end
	end
	for i, v in pairs(deputyConfig) do
		if not(v.attri_type == 0 and v.attri_value == 0) then
			if self["Content"..i] then
				self["Content"..i].gameObject:SetActive(true)
				UH.SetText(self["Content"..i], Format(Language.YouMingPractice.Attr, Language.Common.AttrList[v.attri_type], 
				DataHelper.Percent(v.attri_type, v.attri_value)))
			end
		end
	end
end

ItemInfoViewYouMing3 = ItemInfoViewYouMing3 or DeclareMono("ItemInfoViewYouMing3", ItemInfoViewItem)
function ItemInfoViewYouMing3:OnEnable()
	ItemInfoViewItem.OnEnable(self)
	UH.SetText(self.Title, Language.YouMingPractice.ItemInfoTitle[3])
	local cur_config = self.item1:CurQHConfig()
	for i = 1, 4 do
		if self["Content"..i] then
			self["Content"..i].gameObject:SetActive(false)
		end
	end
	local attr_list = {}
	local attr
	for i = 1, 3 do
        attr = {}
        attr.attr_type = cur_config and cur_config["attr_type_" .. i] or 0
        attr.attr_value = cur_config and cur_config["attr_add_" .. i] or 0
        table.insert(attr_list, attr)
    end
	for i, v in pairs(attr_list) do
		if not(v.attr_type == 0 and v.attr_value == 0) then
			if self["Content"..i] then
				self["Content"..i].gameObject:SetActive(true)
				UH.SetText(self["Content"..i], Format(Language.YouMingPractice.Attr, Language.Common.AttrList[v.attr_type], 
				DataHelper.Percent(v.attr_type, v.attr_value)))
			end
		end
	end
end