
local function func_btn_config(type, name)
	return { type = type, name = name }
end

-- 注意:小写单词大多是策划配在物品表btn_list上的,比如compose
--type类型对应为使用什么颜色的按钮 1为绿色 2为黄色
ItemInfoFunc = {
	Use = func_btn_config(1, "Use"),
	UseAll = func_btn_config(1, "UseAll"),
	UseBatch = func_btn_config(1, "UseBatch"),
	Sell = func_btn_config(2, "Sell"),
	Trade = func_btn_config(2, "Trade"),
	Equip = func_btn_config(1, "Equip"),
	Store = func_btn_config(1, "Store"),
	Fetch = func_btn_config(1, "Fetch"),
	UnFix = func_btn_config(1, "UnFix"),
	ReputationDonate = func_btn_config(2, "ReputationDonate"),
	Stone = func_btn_config(2, "Stone"),
	Smelt = func_btn_config(2, "Smelt"),
	UnStone = func_btn_config(2, "UnStone"),
	Collection = func_btn_config(1, "Collection"),
	UseMap = func_btn_config(1,"UseMap"),
	PetPieceCompose = func_btn_config(1,"PetPieceCompose"),
	HeartSkillChip = func_btn_config(1,"HeartSkillChip"),
	HeartSkillVolume = func_btn_config(1,"HeartSkillVolume"),
	compose = func_btn_config(2,"compose"),
	Resolve = func_btn_config(2, "Resolve"),
	Jewelry = func_btn_config(2, "Jewelry"),
	PartnerQuality = func_btn_config(2,"PartnerQuality"),
	AnotherEquip = func_btn_config(1,"AnotherEquip"),
	AnotherUnFix = func_btn_config(1,"AnotherUnFix"),
	AnotherMedalEquip = func_btn_config(1,"AnotherMedalEquip"),
	MedalEquip = func_btn_config(1,"MedalEquip"),
	MedalAtelier = func_btn_config(2,"MedalAtelier"),
	MedalSmelt = func_btn_config(2,"MedalSmelt"),
	MedalEffect = func_btn_config(2,"MedalEffect"),
	MedalEffectItem = func_btn_config(1,"MedalEffectItem"),
	PetEquipAssembly = func_btn_config(2,"PetEquipAssembly"),
	PetEquipReset = func_btn_config(2,"PetEquipReset"),
	PetEquipBreak = func_btn_config(2,"PetEquipBreak"),
	PetEquipedRemove = func_btn_config(1,"PetEquipedRemove"),
	PetEquipedInstead = func_btn_config(2,"PetEquipedInstead"),
	PetEquipInstensify = func_btn_config(2,"PetEquipInstensify"),
	PartnerEquip = func_btn_config(2,"PartnerEquip"),
	PartnerEquipBreak = func_btn_config(2,"PartnerEquipBreak"),
	AnotherPetEquip = func_btn_config(1,"AnotherPetEquip"),
	AnotherPetEquipedInstead = func_btn_config(2, "AnotherPetEquipedInstead"),
	AnotherPetEquipedRemove = func_btn_config(1,"AnotherPetEquipedRemove"),
	MedalUpgrade = func_btn_config(2,"MedalUpgrade"),
	MedalAllUpgrade = func_btn_config(2,"MedalAllUpgrade"),
	UltiWork = func_btn_config(2, "UltiWork"),
	CrystalEquipIdentify = func_btn_config(1, "CrystalEquipIdentify"),
	PartnerPageCompose = func_btn_config(1, "PartnerPageCompose"),
	Fabao = func_btn_config(1, "Fabao"),
	Divine = func_btn_config(2, "Divine"),
	PetSkillBookUse = func_btn_config(1, "PetSkillBookUse"),
	MedalSpChange = func_btn_config(2, "MedalSpChange"),
	MedalMix = func_btn_config(2, "MedalMix"),
	PreviewGift = func_btn_config(1, "PreviewGift"),
	PetWeaponOnUse = func_btn_config(1, "PetWeaponOnUse"),		-- 魂器穿戴
	PetWeaponUnfix = func_btn_config(2, "PetWeaponUnfix"),		-- 魂器卸下
	PetWeaponReplace = func_btn_config(1, "PetWeaponReplace"),	-- 魂器替换
	PetWeaponRefine = func_btn_config(1, "PetWeaponRefine"),    -- 魂器精炼
	PetWeaponIntensify = func_btn_config(1, "PetWeaponIntensify"),-- 魂器强化
	PetWeaponChange = func_btn_config(1, "PetWeaponChange"),	-- 魂器转换
	SpiritStoneUse = func_btn_config(1, "SpiritStoneUse"),	-- 铸灵使用
	SpiritStoneLevelUp = func_btn_config(1, "SpiritStoneLevelUp"),	-- 铸灵升级
	SpiritStoneResolve = func_btn_config(1, "SpiritStoneResolve"),	-- 铸灵分解
	SpiritStoneRemove = func_btn_config(1, "SpiritStoneRemove"),	-- 铸灵卸下
	SpiritStoneReplace = func_btn_config(1, "SpiritStoneReplace"),	-- 铸灵替换
	AccrssoryUse = func_btn_config(1, "AccrssoryUse"),			-- 配饰穿戴
	AccrssoryResolve = func_btn_config(1, "AccrssoryResolve"),	-- 配饰分解
	AccrssoryRemove = func_btn_config(1, "AccrssoryRemove"),	-- 配饰卸下
	AccrssoryChange = func_btn_config(1, "AccrssoryChange"),	-- 配饰置换
	LingBaoUse = func_btn_config(1, "LingBaoUse"),	-- 灵宝穿戴
	PetStampOnUse = func_btn_config(1, "PetStampOnUse"),
	PetStampUnfix = func_btn_config(1, "PetStampUnfix"),		
	PetStampReplace = func_btn_config(1, "PetStampReplace"),
	PetStampResolve = func_btn_config(1, "PetStampResolve"),
	PetStampIntensify = func_btn_config(1, "PetStampIntensify"),
	PetStampBlueprintPut = func_btn_config(1, "PetStampBlueprintPut"),
	PetStampBlueprintReplace = func_btn_config(1, "PetStampBlueprintReplace"),
	PetStampBlueprintUnfix = func_btn_config(1, "PetStampBlueprintUnfix"),
	PetStampChange = func_btn_config(1, "PetStampChange"),		-- 神印置换
	GlassTripod = func_btn_config(1,"GlassTripod"),
	PetBookChange = func_btn_config(1, "PetBookChange"),
	DungeonSealStone = func_btn_config(1,"DungeonSealStone"),
	NevealResolve = func_btn_config(2, "NevealResolve"),
	Neveal = func_btn_config(2, "Neveal"),
	NevealExtralPlus = func_btn_config(2, "NevealExtralPlus"),
	LiveGive = func_btn_config(1, "LiveGive"),
	BlazeStar = func_btn_config(2, "BlazeStar"),
	PetMulitiPieces1 = func_btn_config(2, "PetMulitiPieces1"),
	PetMulitiPieces2 = func_btn_config(2, "PetMulitiPieces2"),
	PetBagdeCpd = func_btn_config(1, "PetBagdeCpd"),
	PetBagdeReplace = func_btn_config(2, "PetBagdeReplace"),
	PetBagdeUnfix = func_btn_config(1, "PetBagdeUnfix"),
	PetBagdeResolve = func_btn_config(1, "PetBagdeResolve"),
	ConcentricKnotSend = func_btn_config(1, "ConcentricKnotSend"),
	SpecialCompose = func_btn_config(1, "SpecialCompose"),
	MarbasEquip = func_btn_config(1, "MarbasEquip"),
	MarbasUnFix = func_btn_config(1, "MarbasUnFix"),
	MarbasChange = func_btn_config(1, "MarbasChange"), 
	MarbasBloodGemUnfix = func_btn_config(1, "MarbasBloodGemUnfix"), -- 卸下血祭元魂
	MarbasBloodGemEquip = func_btn_config(1, "MarbasBloodGemEquip"), -- 装备血祭元魂
	MarbasBloodGemChange = func_btn_config(1, "MarbasBloodGemChange"), -- 更换血祭元魂
	MarbasBloodGemPlus = func_btn_config(1, "MarbasBloodGemPlus"), -- 元魂升级
	MarbasBloodGemCompose = func_btn_config(1, "MarbasBloodGemCompose"), -- 元魂合成
	MarbasBloodGemDecompose = func_btn_config(1, "MarbasBloodGemDecompose"), -- 元魂分解
	MarbasBloodMulitGemDecompose = func_btn_config(1, "MarbasBloodMulitGemDecompose"), -- 元魂批量分解
	ShengQiFight = func_btn_config(2,"ShengQiFight"),	-- 圣器出战
	ShengQiReady = func_btn_config(2,"ShengQiReady"),	-- 圣器待机
	ShengQiRest = func_btn_config(2,"ShengQiRest"),		-- 圣器下阵
	FuShiUse = func_btn_config(2,"FuShiUse"),			-- 符石镶嵌
	FuShiUnfix = func_btn_config(2,"FuShiUnfix"),		-- 符石卸下
	FuShiReset = func_btn_config(2,"FuShiReset"),		-- 符石重铸
	FuShiFusion = func_btn_config(2,"FuShiFusion"),		-- 符石合成
	FuShiRep = func_btn_config(2,"FuShiRep"),			-- 符石替换
	FuShiUpLevel = func_btn_config(2,"FuShiUpLevel"),	-- 符石升级
	ShengQiSkillChange = func_btn_config(1,"ShengQiSkillChange"),	-- 圣器技能书替换
	Recycling = func_btn_config(1,"Recycling"),			-- 
	FuYuTakeOff = func_btn_config(1, "FuYuTakeOff"),
	FuYuChange = func_btn_config(2, "FuYuChange"),
	FuYuQiangHua = func_btn_config(2, "FuYuQiangHua"),
	FuYuFenJie = func_btn_config(2, "FuYuFenJie"),
	FuYuUse = func_btn_config(2, "FuYuUse"),
	LingQiChangeEquip = func_btn_config(2, "LingQiChangeEquip"),--灵骑装备转换
	LingQiChangeHalidom = func_btn_config(2, "LingQiChangeHalidom"),--灵骑圣物转换
	LingQiYiJianHeCheng = func_btn_config(2, "LingQiYiJianHeCheng"),--灵骑一键合成
	BatchDig = func_btn_config(1, "BatchDig"),
	IllusionChangeResolve = func_btn_config(2, "IllusionChangeResolve"),--时装转换后道具分解
	BicycleSkillCompose = func_btn_config(2, "BicycleSkillCompose"),--灵骑技能卷轴合成
	PetStampSwallow = func_btn_config(1, "PetStampSwallow"),			--神印吞噬按钮\
	FuYuSwitch = func_btn_config(2, "FuYuSwitch"),--符玉转换
	FuYuShengHua = func_btn_config(2, "FuYuShengHua"),--符玉升华
	PetStampDrop = func_btn_config(2, "PetStampDrop"),
	HuanShouResolve = func_btn_config(2, "HuanShouResolve"),
	DragonSoulUp = func_btn_config(2,"DragonSoulUp"),	-- 龙魂升级
	DragonSoulRep = func_btn_config(1,"DragonSoulRep"),	-- 龙魂替换
	DragonSoulUse = func_btn_config(1,"DragonSoulUse"),	-- 龙魂镶嵌
	DragonSoulRep2 = func_btn_config(1,"DragonSoulRep2"),		-- 龙魂替换 : 打开背包
	DragonSoulUnfix = func_btn_config(2, "DragonSoulUnfix"), 	-- 龙魂卸下
	EquipConversion = func_btn_config(2, "EquipConversion"),--装备转换
	YouMingEquipUnfix = func_btn_config(1, "YouMingEquipUnfix"),
	YouMingTiHuang = func_btn_config(1, "YouMingTiHuang"),
	YouMingTiHuang2 = func_btn_config(1, "YouMingTiHuang2"),
}
ItemInfoUseParam =
{
    [ItemType.Jewelry] = function(item)
        return --[[ {oper_type = EQUIP_OPERA_TYPE.OPERA_TYPE_PUTON,param_1 = item.column_type,param_2 = item.index,param_3 = item:EquipType()} ]] { op_type = JEWELRY_OPERA_TYPE.OPERA_TYPE_WEAR_JEWELRY, param = item.index }
	end,
	[ItemType.Medal] = function(item)
		return {req_type = MEDAL_REQ_TYPE.PUT_ON,param1 = BagData.Instance:GetMedalEmpty(),param2 = item.column_type, param3 = item.index}
	end,
	[ItemType.Armor] = function(item)
		return {oper_type = EQUIP_OPERA_TYPE.OPERA_TYPE_PUTON,param_1 = item.column_type,param_2 = item.index,param_3 = item:EquipType()}
	end,
	[ItemType.Weapon] = function(item)
		return {oper_type = EQUIP_OPERA_TYPE.OPERA_TYPE_PUTON,param_1 = item.column_type,param_2 = item.index,param_3 = item:EquipType()}
	end,
	[ItemType.EleCrystal] = function (item)
		return {column = item.column_type, index = item.index}
	end,
}

-- 化身背包使用
AnotherItemUseParam =
{
	[ItemType.EleCrystal] = function(item)
        return {req_type = ANOTHER_EQUIP_TYPE.PUTON_CRYSTAL,param_1 = item.index}
	end,
	[ItemType.Jewelry] = function(item)
        return {req_type = ANOTHER_EQUIP_TYPE.PUTON,param_1 = item.index}
	end,
	[ItemType.Medal] = function(item)
        return {req_type = MEDAL_REQ_TYPE.PUT_ON,param_1 = AnotherData.Instance:GetMedalEmpty(),param_2 = item.index}
	end,
	[ItemType.Armor] = function(item)
		return {req_type = EQUIP_OPERA_TYPE.OPERA_TYPE_PUTON,param_2 = item.index,param_3 = item:EquipType(),param_4 = 1}
	end,
	[ItemType.Weapon] = function(item)
		return {req_type = EQUIP_OPERA_TYPE.OPERA_TYPE_PUTON,param_2 = item.index,param_3 = item:EquipType(),param_4 = 1}
	end,
}

EquipmentUnFixParam = {
	[ItemType.EleCrystal] = function(item)
        return { op_type = EQUIP_OPERA_TYPE.OPERA_TYPE_TAKEOFF, param = item.index,}
	end,
	[ItemType.Jewelry] = function(item)
        return { op_type = JEWELRY_OPERA_TYPE.OPERA_TYPE_TAKE_OFF_ONE,param = item.jewelry_type }
	end,
	[ItemType.Medal] = function(item)
        return { req_type = MEDAL_REQ_TYPE.TAKE_OFF, param1 = item.vo.grid_index }
	end,
	[ItemType.Armor] = function(item)
		return { oper_type = EQUIP_OPERA_TYPE.OPERA_TYPE_TAKEOFF,param_1 = item.index, }
	end,
	[ItemType.Weapon] = function(item)
		return { oper_type = EQUIP_OPERA_TYPE.OPERA_TYPE_TAKEOFF,param_1 = item.index, }
	end,
}

-- 化身装备卸下
AnotherEquipmentUnFixParam =
{
	[ItemType.EleCrystal] = function(item)
        return {req_type = ANOTHER_EQUIP_TYPE.TAKEOFF_CRYSTAL}
	end,
	[ItemType.Jewelry] = function(item)
        return {req_type = ANOTHER_EQUIP_TYPE.TAKEOFF,param_1 = item:JewelryType()}
	end,
	[ItemType.Medal] = function(item)
        return {req_type = MEDAL_REQ_TYPE.TAKE_OFF,param_1 = item.vo.grid_index}
	end,
	[ItemType.Armor] = function(item)
		return {req_type = EQUIP_OPERA_TYPE.OPERA_TYPE_TAKEOFF,param_1 = item:EquipType() ,param_2 = 1}
	end,
	[ItemType.Weapon] = function(item)
		return {req_type = EQUIP_OPERA_TYPE.OPERA_TYPE_TAKEOFF,param_1 = item:EquipType() ,param_2 = 1}
	end,
}

ItemInfoUseFunc =
{
	[ItemType.EleCrystal] = function(param_t)
		BagCtrl.Instance:SendUseItem(param_t)
	end,
	[ItemType.Jewelry] = function(param_t)
		BagCtrl.Instance:SendJewelryOperaReq(param_t)
	end,
	[ItemType.Armor] = function(param_t)
		BagCtrl.Instance:SendEquipmentOperaReq(param_t)
	end,
	[ItemType.Weapon] = function(param_t)
		BagCtrl.Instance:SendEquipmentOperaReq(param_t)
	end,
	[ItemType.Medal] = function(param_t)
		BagCtrl.Instance:SendMedalReq(param_t)
	end,
	[ItemType.Gift] = function(param_t)
		BagCtrl.Instance:SendUseGiftReq(param_t)
	end,
	[ItemType.PveCrystal] = function(param_t)
		FightCtrl.Instance:OnPveCrystalUseFunc()
	end,
	[ItemType.PeakToken] = function(param_t,self)
		local today_use_times = PeakTokenData.Instance:GetTodayUseTimes()
		if self.item.param.used_times >= Config.supreme_token_auto.other[1].use_frequency then
			PublicPopupCtrl.Instance:Center(Language.PeakToken.NotTimesTip)
			return
		end
		if today_use_times >= 1 then
			PublicPopupCtrl.Instance:Center(Language.PeakToken.TodayUseTimes)
			return
		end
		if not LoginData.Instance:IsOnCrossSever() then
			PublicPopupCtrl.Instance:AlertTip(Language.PeakToken.CrossTips,function()
				LoginCtrl.Instance:SendCrossStartReq()
			end)
			return
		end
		BagCtrl.Instance:SendUseItem(param_t)
		PublicPopupCtrl.Instance:Center(Language.PeakToken.UseItemTip)
		ViewMgr:CloseView(BagView)
	end,
	[ItemType.YouMingEquip] = function(param_t)
		local youming = YouMingData.Instance:GetSelectYouMing()
		YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.PUT_ON_EQUIP, youming.index, param_t.index)
		YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.SINGLE_INFO, YouMingCfg.NETHER_WORLD_BAG_TYPE.NETHER_WORLD_BAG_TYPE_NW, youming.index)
	end
}

AnotherItemInfoUseFunc = 
{
	[ItemType.EleCrystal] = function(param_t)
		AnotherCtrl.Instance:SendEquipReq(param_t)
	end,
	[ItemType.Jewelry] = function(param_t)
		AnotherCtrl.Instance:SendJewelryReq(param_t)
	end,
	[ItemType.Armor] = function(param_t)
		AnotherCtrl.Instance:SendEquipReq(param_t)
	end,
	[ItemType.Weapon] = function(param_t)
		AnotherCtrl.Instance:SendEquipReq(param_t)
	end,
	[ItemType.Medal] = function(param_t)
		AnotherCtrl.Instance:SendMedalReq(param_t)
	end,
}

ConditionUseFunc = {
	[ItemUseType.HPMedicine] = function(param_t,self)
		return ItemInfoCtrl.Instance:ItemDeriveView(ItemHealView,{pos = UH.GetRootPosition(self.Parent)})
	end,
	[ItemUseType.MPMedicine] = function(param_t,self)
		return ItemInfoCtrl.Instance:ItemDeriveView(ItemHealView,{pos = UH.GetRootPosition(self.Parent)})
	end,
	[ItemUseType.MoneyBox] = function(param_t,self,num)
		ViewMgr:CloseView(ItemInfoView)
		ViewMgr:OpenView(MoneyBoxView, {num = num})
		return true
	end,
}

ItemInfoViewFuncItem = DeclareMono("ItemInfoViewFuncItem", UIWidgetBaseItem)

function ItemInfoViewFuncItem:ItemInfoViewFuncItem()
	self.info_data = ItemInfoCtrl.Instance:Data()
	self.bag_data = BagData.Instance
	self.info = self.info_data:ItemInfo()
	self.language = Language.ItemInfo.ItemButtonName
end

function ItemInfoViewFuncItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.way_cfg = nil
	self.ItemInter.Interactable = true
	self.LockObj:SetActive(false)
	local remind_num = 0 		-- 按钮上的红点
	if "string" == type(data) then
		local info = self.bag_data:GetWayCfg(data)
		self.way_cfg = info
		self.Name.text = self.way_cfg.desc
		if self.ButtonSp then
			self.Name.color = COLORS.Green1
			self.ButtonSp.SpriteName = "LvAnNiu_PuTong"
		end
		if StringIsEmpty(info.npc) and not StringIsEmpty(info.open_panel) then
			if GetWayFBFlush[info.open_panel] and GetWayFBFlush[info.open_panel].InterFunc then
				local interactor = GetWayFBFlush[info.open_panel].InterFunc(self.way_cfg)
				self.ItemInter.Interactable = interactor
				self.LockObj:SetActive(not interactor)
			else
				if not FunOpen.Instance:GetFunIsOpened(info.open_panel) then
					self.ItemInter.Interactable = false
					self.LockObj:SetActive(true)
				end
			end
		elseif not StringIsEmpty(info.scene_id) or not StringIsEmpty(info.npc) then
			--获取途径是npc时查询npc所在场景是否能进
			local scene_id = 0
			if not StringIsEmpty(info.scene_id) then
				scene_id = info.scene_id
			elseif not StringIsEmpty(info.npc) and tonumber(info.npc) then
				scene_id = VoMgr:NpcTargetByNpcId(tonumber(info.npc))
			end
			if scene_id ~= 0 then
				local scene_show_cfg = MapData.Instance:GetMapShowData(scene_id)
				if scene_show_cfg then
					local min_level = MapData.Instance:GetMapMinLevel(scene_show_cfg.restrict_in)
					if min_level and RoleData.Instance:GetRoleLevel() < min_level then
						self.ItemInter.Interactable = false
						self.LockObj:SetActive(true)
					end
				end
			end
		end
	else
		self.item = self.info.item_list[1]
		local config = ItemInfoConfig.ButtonConfig[data.type]
		self.Name.text = self.language[self.data.name]
		if self.ButtonSp then
			self.Name.color = config.LbColor
			self.ButtonSp.SpriteName = config.SpName
		end
		if self.item.param and self.item.param.remain_times then
			local is_open = self.item.param.is_turn_on == 1
			self.Name.text = is_open and self.language.Close or self.language.Open
			self.ButtonSp.SpriteName =  is_open and ItemInfoConfig.ButtonConfig[2].SpName or ItemInfoConfig.ButtonConfig[1].SpName
			self.Name.color = is_open and ItemInfoConfig.ButtonConfig[2].LbColor or ItemInfoConfig.ButtonConfig[1].LbColor
		end
		remind_num = ItemInfoData.Instance:GetBtnRemind(self.data.name, self.info)
	end
	if self.RedPoint then self.RedPoint:SetNum(remind_num) end 
end

function ItemInfoViewFuncItem:OnClick()
	if self.way_cfg then
		local info = self.way_cfg
		GetWayData.OpenPanel(info,self.info.item_list and self.info.item_list[1] or {})
		ViewMgr:CloseView(BatchBuyView)
	else
		self[self.data.name](self)
	end
end

function ItemInfoViewFuncItem:Equip()
	if self.item:LimitLv() > RoleData.Instance:GetRoleLevel() and self.item:Config().without_level_limit == 0 then
		PublicPopupCtrl.Instance:Center(Format(Language.ItemInfo.Tips.Limitlevel,self.item:LimitLv()))
		return
	end
	local limit_prof = Equip.GetLimitProf(self.item.item_id)
	if limit_prof ~= -1 and limit_prof ~= RoleData.Instance:GetRoleProfType() then
		local prof = Language.Common.Prof[math.floor(limit_prof / 100)]
		PublicPopupCtrl.Instance:Center(Format(Language.ItemInfo.Tips.LimitProf,prof))
		return
	end
	local item_type = self.item:Type()
	if item_type == 0 or ItemInfoUseParam[item_type] == nil then
		LogError(self.item.item_id)
		return
	end
	local param_t = ItemInfoUseParam[item_type](self.item)
	-- if not self.item:IsMedal() and not self.item:Bind() and self.item:CanTrade() and Item.GetColor(self.item.item_id) >= Config.sundries_auto.other[1].minimum_quality then
	-- 	PublicPopupCtrl.Instance:AlertTip(Language.ItemInfo.Tips.EquipQualityTip,function()
	-- 		self:SwitchUse(item_type, param_t)
	-- 	end)
	-- 	return
	-- end
	-- 准备穿上去时进行mark
	WorkshopData.Instance:MarkDrawOffEquip(self.item)

	if MainViewData.Instance:IsBattle() then
		PublicPopupCtrl.Instance:Center(Language.ItemInfo.ChangeEquipInBattle)
		-- return
	end
	self:SwitchUse(item_type, param_t)
end

function ItemInfoViewFuncItem:UnFix()
	if MainViewData.Instance:IsBattle(true) then
		return
	end

	local item_type = self.item:Type()
	local param_t = EquipmentUnFixParam[item_type](self.item)
	self:SwitchUse(item_type, param_t)
end

function ItemInfoViewFuncItem:Use(num)
	local item_id = self.item.item_id or 0
	-- 角色使用喇叭需达到渠道聊天等级才能打开喇叭使用界面
	if ChatConfig.ItemTrumpet.not_cross == item_id and RoleData.Instance:GetRoleLevel() < GLOBAL_CONFIG:HornGrade() then
		PublicPopupCtrl.Instance:Center(string.format(Language.Chat.ChatLevelLimitUse, GLOBAL_CONFIG:HornGrade(), self.item:QuaName()))
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	if ChatConfig.ItemTrumpet.is_cross == item_id and RoleData.Instance:GetRoleLevel() < GLOBAL_CONFIG:HornGradeCross() then
		PublicPopupCtrl.Instance:Center(string.format(Language.Chat.ChatLevelLimitUse, GLOBAL_CONFIG:HornGradeCross(), self.item:QuaName()))
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	if ShowType.GiftFlower == self.item:ShowType() then
		SocietyData.Instance:SetSocietySendInfo(SocietyConfig.ShareType.GiftFlowers, 0, self.item.item_id)
		ViewMgr:CloseView(ItemInfoView)
		ViewMgr:OpenView(SocietySendView)
		return
	end
	if not StringIsEmpty(Item.GetOpenPanel(item_id)) then
		local way_info = self.bag_data:GetWayCfg(Item.GetOpenPanel(item_id))
		GetWayData.OpenPanel(way_info,self.item)
		return
	end
	--伙伴碎片使用时选中对应伙伴
	if ShowType.PartnerPieces == self.item:ShowType() then
		PartnerData.Instance:PartnerShowListJump(true)
		PartnerData.Instance.guide_partner = PartnerData.Instance:GetPartnerActivationInfoByItemId(self.item.item_id)
	end
	--时装转换后道具使用时选中对应时装
	if ShowType.IllusionChange == self.item:ShowType() then
		IllusionData.Instance:SetNeedJumpItemId(item_id)
	end
	if ShowType.Neveal == self.item:ShowType() or item_id == CommonItemId.DianHuaShi then
		local is_open,str = FunOpen.Instance:GetFunIsOpened(Mod.Equip.Neveal)
		if not is_open then 
			PublicPopupCtrl.Instance:Center(str)
			return 
		end 
		-- if FlyUpData.Instance:GetStage() < FlyUpStageType.Low then 
		-- 	PublicPopupCtrl.Instance:Center(Language.Workshop.Neveal.OpenTips)
		-- 	return 
		-- end 
	end
	if ShowType.ActItem == self.item:ShowType() then
		local mod_key = self.item:ModKey()
		if not StringIsEmpty(mod_key) then
			ViewMgr:OpenViewByKey(mod_key)
			ViewMgr:CloseView(ItemInfoView)
			ViewMgr:CloseView(BagView)
			return
		end
	end
	-- 圣器寻器道具特殊判断
	if ShowType.ShengQiYaoShi == self.item:ShowType() then
		if TreasureHuntData.Instance:JudgeXunQiShengQiIsOpen() then
			TreasureHuntCtrl.Instance:OpenTreasureShengQi()
			ViewMgr:CloseView(ItemInfoView)
			ViewMgr:CloseView(BagView)
			return
		else
			PublicPopupCtrl.Instance:Center(Language.TreasureHunt.XunQiShengQiNotOpenTip)
			return
		end
	end
	-- 灵骑道具特殊判断
	if ShowType.BicycleAct == self.item:ShowType() then
		BagData.Instance:LingQiFastUse(self.item)
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	
	if PartnerData.Instance:PartnerSoarWeaponConsume(item_id) then
		return
	end
	local mod_key = self.item:ModKey()
	if not StringIsEmpty(mod_key) then
		if mod_key == Mod.StarTreasureMap.Exchange then
			StarTreasureMapData.Instance:SetItem(self.item)
		end
		if ModToModule(Mod.FairyDragon.Main) == ModToModule(mod_key) then
			local is_open = FunOpen.Instance:CheckOpen(Mod.FairyDragon)
			if not is_open then
				PublicPopupCtrl.Instance:Center(Language.FairyDragon.NotOpen)
				return
			end
		end
		ViewMgr:OpenViewByKey(mod_key)
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	if self.item:ShowType() == ShowType.LiveCard then
		LiveCtrl.Instance:UseCard(self.item)
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	if self.item:ShowType() == ShowType.LiveFood then
		LiveCtrl.Instance:UseFood(self.item)
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	if self.item:ShowType() == ShowType.VitalityFirework then
		VitalityFireworkCtrl.Instance:UseFirework()
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	if self.item:ShowType() == ShowType.LingQiChangeEquip then
		BicycleData.Instance:ChangeItemType(BicycleCfg.ChangeItem.Equip)
		ViewMgr:OpenView(BicycleChangeView)
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	if self.item:ShowType() == ShowType.LingQiChangeHalidom then
		BicycleData.Instance:ChangeItemType(BicycleCfg.ChangeItem.Halidom)
		ViewMgr:OpenView(BicycleChangeView)
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	if self.item:ShowType() == ShowType.GiftBag then
		-- 礼包需要判断使用等级
		local lv_limit = Item.GetLimitLv(item_id) or 0
		if RoleLevel() < lv_limit then
			PublicPopupCtrl.Instance:Center(Format(Language.ItemInfo.Tips.GiftLevelTip, lv_limit))
			return
		end
	end
	if Item.GetUseType(item_id) == ItemUseType.PrestigeCard then
		PrestigeData.IsPopupNotice = true
	end
	--特殊条件的
	if self:Condition(Item.GetUseType(item_id),Item.GetCondition(item_id),num or 1) then
		return
	end
	self:SwitchUse(self.item:Type(),{item_id = self.item.item_id,column = self.item.column_type,index = self.item.index,num = num or 1})
end

function ItemInfoViewFuncItem:UseAll()
	self:Use(self.item:Num())
end

function ItemInfoViewFuncItem:UseBatch()
	local param_t = {item_id = self.item.item_id,column = self.item.column_type,index = self.item.index,num = self.item:Num() or 1}
	ViewMgr:OpenView(ChooseGiftAllView,param_t)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:Sell()
	if self.item.num == 1 and ViewMgr:IsOpen(MallView) then
		ItemInfoCtrl.Instance:SendSellReq(self.item)
	else
		if not ViewMgr:IsOpen(MallView) and MarketData.Instance:IsMarketSell(self.item,true) then
			MarketCtrl.Instance:OnMarketSell()
			ViewMgr:CloseView(ItemInfoView)
			if ViewMgr:IsOpen(GetWayView) then ViewMgr:CloseView(GetWayView) end 
		else
			ItemInfoCtrl.Instance:ItemDeriveView(ItemSellView,{pos = UH.GetRootPosition(self.Parent)})
		end
	end
end

function ItemInfoViewFuncItem:Trade()
	TradeData.Instance:SetJumpItemData(self.item)
	ViewMgr:OpenViewByKey(Mod.Mall.Trade)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:Store()
	local page = BagCtrl.Instance:Data().storage_cur_sel_index or 1
	BagCtrl.Instance:SendKnapsackOperaReq{oper_type = ItemInfoConfig.BagOperType.store, param_1 = self.item.column_type, param_2 = self.item.index, param_3 = page - 1}
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:Fetch()
	BagCtrl.Instance:SendKnapsackOperaReq{oper_type = ItemInfoConfig.BagOperType.fetch, param_1 = self.item.column_type, param_2 = self.item.index}
	ViewMgr:CloseView(ItemInfoView)
end

--一般类型使用
function ItemInfoViewFuncItem:SwitchUse(item_type, param_t)
	local use_func = ItemInfoUseFunc[item_type]
	if use_func then
		use_func(param_t,self)
	else
		BagCtrl.Instance:SendUseItem(param_t)
	end
	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
	ViewMgr:CloseView(ItemInfoView)
end

--特殊类型使用 条件使用
function ItemInfoViewFuncItem:Condition(use_type,param_t,num)
	local use_func = ConditionUseFunc[use_type]
	if use_func then
		use_func(param_t,self,num)
		return true
	end
end

--化身背包内使用
function ItemInfoViewFuncItem:AnotherUse(item_type,param_t)
	local use_func = AnotherItemInfoUseFunc[item_type]
	if use_func then
		use_func(param_t)
	end
	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
	ViewMgr:CloseView(ItemInfoView)
end 

function ItemInfoViewFuncItem:ReputationDonate()
	ViewMgr:CloseView(ItemInfoView)
	ViewMgr:OpenView(ReputationDonationView)
end

function ItemInfoViewFuncItem:Stone()
	--跳转到宝石界面
	-- WorkshopData.Instance:SetQuickJewelIndex(self.item)
    WorkshopData.Instance:SetInsetMark(self.item)
	ViewMgr:CloseView(ItemInfoView)
	ViewMgr:OpenViewByKey(Mod.Equip.Inser)

end

function ItemInfoViewFuncItem:UnStone()
	for index, value in ipairs(self.item:Stone()) do
		if value > 0 then
			local param1 = self.item:IsEquip() and 0 or 1
			--宝石所占孔 0或者1
			local param2 = self.item.in_bag == true and 1 or 0
			local param3 = self.item.index
			local param4 = index - 1
			WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_GEM_DISMOUNT, param1, param2, param3,param4)
		end
	end
	ViewMgr:CloseView(ItemInfoView)
end

-- 图鉴收集接口
function ItemInfoViewFuncItem:Collection()
	ViewMgr:OpenViewByKey(Mod.Illustrate.Main)
end

-- 勋章兑换
-- function ItemInfoViewFuncItem:ExChange()
-- 	BagData.Instance:SetMedalChange(self.item)
-- 	SceneLogic.Instance:AutoToNpc(242,nil,false,true)
-- 	ViewMgr:CloseView(ItemInfoView)
-- end

function ItemInfoViewFuncItem:UseMap()
	if TeamData.Instance:InTeam() then
		PublicPopupCtrl.Instance:Center(ErrorText[146])
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	--星辰宝图
	if self.item:IsStarTreasureMap() then
		if RoleLevel() < FunOpen.GetConfog().star_treasure_map then
			PublicPopupCtrl.Instance:Center(Format(Language.StarTreasureMap.LimitLevel,FunOpen.GetConfog().star_treasure_map))
			return
		end
		TreasureMapData.Instance:SetMapInfo(self.item)
		ViewMgr:OpenView(self.item:IsActiveMap() and StarTreasureMapView or StarTreasureMapAtlaView)
		ViewMgr:CloseView(ItemInfoView)
		ViewMgr:CloseView(BagView)
		return
	end
	if self.item.IsShengQiMap and self.item:IsShengQiMap() then
		if not ShengQiData.Instance:IsOpen() then
			PublicPopupCtrl.Instance:Center(Language.StarTreasureMap.ShengQiLimit)
			return
		end
		TreasureMapData.Instance:SetMapInfo(self.item)
		ViewMgr:OpenView(ShengQiTreasureMap)
		ViewMgr:CloseView(ItemInfoView)
		ViewMgr:CloseView(BagView)
		return
	end
	TreasureMapData.Instance:SetMapInfo(self.item)
	if self.item:IsSuperMap() then
		--[[ local role_id = RoleData.Instance:GetBaseData().role_id
		local flag = UnityPlayerPrefs.GetInt(PrefKeys.SuperMapGuide(role_id)) == 0
		if flag then
			ViewMgr:OpenView(GuideTreasureMap)
		end ]]
		SceneLogic.Instance:AutoToPos(self.item.param.scene_id, nil, function ()
			SceneLogic.Instance:AutoToPos(self.item.param.scene_id, self.item.param, function ()
				SceneLogic.Instance:ClearMoveCache()
			end, true)
			TreasureMapData.Instance:SetMapInfo(self.item)
			ViewMgr:OpenView(Compass)
		end, true, true)
		TreasureMapData.Instance:SetNewFlag(false)
	else
		local id = UnlockNewMapCtrl.Instance:GetIrregularID(self.item.param.scene_id)
		local state = MapData.Instance:GetIrregularFlagState(id)
		SceneLogic.Instance.is_auto_map = true
		SceneLogic.Instance:AutoToPos(self.item.param.scene_id, self.item.param, BehaviorFactory.TreasureMap(self.item), true, true, not state)
	end
	ViewMgr:CloseView(ItemInfoView)
	ViewMgr:CloseView(BagView)
end

function ItemInfoViewFuncItem:PetPieceCompose()
	local mod_key = self.item:ModKey()
	if CommonItemId.ShenShouSoul == self.item.item_id then
		SceneLogic.Instance:AutoToNpc(65041,nil,false,true)
		ViewMgr:CloseView(ItemInfoView)
		ViewMgr:CloseView(BagView)
    elseif not StringIsEmpty(mod_key) then
		ViewMgr:OpenViewByKey(mod_key)
		ViewMgr:CloseView(ItemInfoView)
	elseif IS_EDITOR then
		LogError("PetPieceCompose mod_key is nil item_id = ",self.item.item_id)
	end
end

function ItemInfoViewFuncItem:HeartSkillChip()
	local config = DataHelper.GetSearchCond(Config.heart_skill_cfg_auto.heart_skill_compose,function(data)
		return data.compose_consume.item_id == self.item.item_id
	end)
	if config and Item.GetNum(self.item.item_id) >= config.compose_consume.num then
		HeartSkillCtrl.Instance:SendHeartSkillReq(4,config.compose_target)
	else
		PublicPopupCtrl.Instance:Center(Format(Language.HeartSkill.HeartSkillChipTip,self.item:Name()))
	end
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:HeartSkillVolume()
	local item = self.item
	local config = DataHelper.GetSearchCond(Config.heart_skill_cfg_auto.heart_skill_displace,function(data)
		return data.consume_item == self.item.item_id
	end)
	if config ~= nil then
		item = HeartSkillData.Instance:GetBagSkillItem(config.heart_skill_color or 0)
	end
	ViewMgr:OpenView(HeartSkillChangeView,{item = item,config = config})
end

function ItemInfoViewFuncItem:compose()
	local mod_key = self.item:ModKey()
	ViewMgr:CloseView(ItemInfoView)
	ViewMgr:CloseView(BagView)
	WorkshopData.Instance:QuickComposeType(self.item:ShowType())
	ViewMgr:OpenViewByKey(mod_key)
end

function ItemInfoViewFuncItem:DungeonSealStone()
	local mod_key = self.item:ModKey()
	if not StringIsEmpty(mod_key) then
		ViewMgr:OpenViewByKey(mod_key)
	end
end
--法宝碎片特殊处理
--修改为装备分解
function ItemInfoViewFuncItem:Resolve()
	ViewMgr:CloseView(ItemInfoView)
	-- if self.item:HadStone() then 
	-- 	PublicPopupCtrl.Instance:Center( Language.Resolve.InfoLimit )
	-- else
	if WorkshopData.Instance:CheckEquipWithSmeltDone(self.item) then 
		BagData.Instance:SetPreResolveItem(self.item)
		local param_t = {
			content = Language.Resolve.SmeltLimit,
			confirm = {
				func =  function()
					ViewMgr:CloseView(DialogTipsView)
					ViewMgr:OpenViewByKey(Mod.Resolve.Main)
				end 
			},
		}

		PublicPopupCtrl.Instance:DialogTips(param_t)
	elseif self.item.IsNeveal ~= nil and self.item:IsNeveal() then 
		BagData.Instance:SetPreResolveItem(self.item)
		local param_t = {
			content = Language.Resolve.NevealLimit,
			confirm = {
				func =  function()
					ViewMgr:CloseView(DialogTipsView)
					ViewMgr:OpenViewByKey(Mod.Resolve.Main)
				end 
			},
		}

		PublicPopupCtrl.Instance:DialogTips(param_t)
	else 
		BagData.Instance:SetPreResolveItem(self.item)
		ViewMgr:OpenViewByKey(Mod.Resolve.Main)
	end 

	--传 seq num index
	--[[ if ShowType.Fashion == self.item:ShowType() then
		local co = self.item:Config()
		local id = co and co.param1 or 1
		local decompose_num = Cfg.SurfaceInfoById(id).decompose_num
		PublicPopupCtrl.Instance:DialogTips{
			content = string.format(Language.PublicPopup.ShiZhuangResolve, decompose_num),
			confirm = {
				func = function ()
					BagCtrl.Instance:SendBagDecompose(self.item.column_type, self.item.index)
					PublicPopupCtrl.Instance:CloseDialogTips()
					ViewMgr:CloseView(ItemInfoView)
				end
			}
		}
	else
		local fabao = WorkshopData.Instance:FabaoSPInfo()
		if next(fabao)==nil then
			LogError("工坊合成表没有法宝碎片兑换配置")
			ViewMgr:CloseView(ItemInfoView)
			return
		end
		-- WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_FABAO_SP_EXC, param1, param2, param3)
		PublicPopupCtrl.Instance:DialogTips({
			content = string.format(Language.PublicPopup.FaobaoSP, self.item.num),
			confirm = {
				func = function ()
					WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_FABAO_SP_EXC, fabao[1].seq, self.item.num, self.item.index)
					PublicPopupCtrl.Instance:CloseDialogTips()
					ViewMgr:CloseView(ItemInfoView)
				end
			}
		})
	end ]]
end

function ItemInfoViewFuncItem:Smelt()
	-- if self.item.in_bag == false then
	WorkshopData.Instance:SetSmeltSelect(self.item)
	-- end
	ViewMgr:OpenViewByKey(Mod.Equip.Smelt)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:Jewelry()
	WorkshopData.Instance:SetLockJewelryMatrial(self.item.item_id)
	ViewMgr:OpenViewByKey(Mod.Workshop.Jewelry)
	ViewMgr:CloseView(ItemInfoView)
end
function ItemInfoViewFuncItem:Fabao()
	ViewMgr:OpenViewByKey(Mod.Fabao.Main)
	ViewMgr:CloseView(ItemInfoView)
end
function ItemInfoViewFuncItem:PartnerQuality()
	if self.item:Level() > RoleData.Instance:GetRoleLevel() then 
		PublicPopupCtrl.Instance:Center(string.format( Language.Partner.LevelLowToUse,self.item:Level() ))
		return 
	end 
	local flag,tips = PartnerData.Instance:GetPartnerQualityShouldJump(self.item.item_id)
	if not flag then PublicPopupCtrl.Instance:Center(tips) return end 
	PartnerData.Instance:PartnerShowListJump(true) 
	PartnerData.Instance.guide_partner = PartnerData.Instance:GetPartnerQualityInfoByItemId(self.item.item_id)
	ViewMgr:OpenViewByKey(Mod.Partner.PartnerQuality)
	ViewMgr:CloseView(ItemInfoView)
end


function ItemInfoViewFuncItem:AnotherEquip()
	if MainViewData.Instance:IsBattle(true) then
		return
	end
	if self.item:LimitLv() > RoleData.Instance:GetRoleLevel() then
		PublicPopupCtrl.Instance:Center(Format(Language.ItemInfo.Tips.Limitlevel,self.item:LimitLv()))
		return
	end
	local limit_prof = Equip.GetLimitProf(self.item.item_id)
	if limit_prof ~= -1 and limit_prof ~= RoleData.Instance:GetRoleProfType() then
		local prof = Language.Common.Prof[math.floor(limit_prof / 100)]
		PublicPopupCtrl.Instance:Center(Format(Language.ItemInfo.Tips.LimitProf,prof))
		return
	end
	
	local item_type = self.item:Type()
	if item_type == 0 then
		LogError("item_type wrong "..self.item.item_id)
		return
	end

	local param_t = AnotherItemUseParam[item_type](self.item)
	-- if  not self.item:IsMedal() and not self.item:Bind() and self.item:CanTrade() and Item.GetColor(self.item.item_id) >= Config.sundries_auto.other[1].minimum_quality then
	-- 	PublicPopupCtrl.Instance:AlertTip(Language.ItemInfo.Tips.EquipQualityTip,function()
	-- 		self:AnotherUse(item_type, param_t)
	-- 	end)
	-- 	return
	-- end

	self:AnotherUse(item_type, param_t)

	-- 同步操作
	local real_item = BagData.Instance:SearchEquipByUniqueId(self.item)
	local param_t_together = ItemInfoUseParam[item_type](real_item)
	self:SwitchUse(item_type, param_t_together)

	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:AnotherUnFix()
	if MainViewData.Instance:IsBattle(true) then
		return
	end

	local item_type = self.item:Type()
	local param_t = AnotherEquipmentUnFixParam[item_type](self.item)
	self:AnotherUse(item_type, param_t)

	-- 同步操作
	-- local real_item = BagData.Instance:SearchEquipByUniqueId(self.item)
	local param_t_together = EquipmentUnFixParam[item_type](self.item)
	self:SwitchUse(item_type, param_t_together)

	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:MedalEquip()
	ViewMgr:OpenView(MedalEquipView,self.item)
	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
	ViewMgr:CloseView(ItemInfoView)
end 

function ItemInfoViewFuncItem:AnotherMedalEquip()
	ViewMgr:OpenView(AnotherMedalEquipView,self.item)
	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
	ViewMgr:CloseView(ItemInfoView)
end 

function ItemInfoViewFuncItem:MedalSmelt()
	-- BagData.Instance:SetCurSelectMedal(self.item,true)
	ViewMgr:OpenView(MedalSmeltView)
	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
	ViewMgr:CloseView(ItemInfoView)
end 

function ItemInfoViewFuncItem:MedalAtelier()
	-- BagData.Instance:SetCurSelectMedal(self.item,true)
	ViewMgr:OpenView(MedalAtelierView)
	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
	ViewMgr:CloseView(ItemInfoView)
end 

function ItemInfoViewFuncItem:MedalEffect()
	local param_t = {}
	if self.item.param and self.item.param.is_equip then 
		param_t.is_equip = self.item.param.is_equip
		param_t.grid_index = self.item.param.grid_pos
	else 
		param_t.is_equip = false
		param_t.grid_index = self.item.index
	end 
	BagData.Instance:SetMedalQuickItem(self.item)
	ViewMgr:OpenView(MedalFulingView,param_t)
	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
	ViewMgr:CloseView(ItemInfoView)
end 

function ItemInfoViewFuncItem:MedalEffectItem()
	ViewMgr:OpenView(MedalFulingView)
	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
	MedalData.Instance:SetFulingTarget(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:MedalUpgrade()
	-- BagData.Instance:SetMedalQuickItem(self.item)

	if self.item.param and self.item.param.grid_pos then 
		MedalData.Instance:SetOperateMedal({wear_pos = self.item.param.grid_pos})
	else 
		MedalData.Instance:SetOperateMedal({bag_pos = { column = self.item.column_type , index = self.item.index }})
	end 
	-- local param_t = {}
	-- if self.item.param.is_equip then 
	-- 	param_t.is_equip = self.item.param.is_equip
	-- 	param_t.grid_index = self.item.param.grid_pos
	-- else 
	-- 	param_t.is_equip = false
	-- 	param_t.grid_index = self.item.index
	-- end 
	ViewMgr:OpenView(MedalUpgradeView)
	AudioMgr:PlayEffect(AudioType.UI, "equip_equipment")
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:MedalAllUpgrade()
	ViewMgr:CloseView(ItemInfoView)

	MedalData.Instance:SetPreUpgradeItem(self.item)
	ViewMgr:OpenView(MedalAllUpgradeView)
end

--宠物装备合成
function ItemInfoViewFuncItem:PetEquipAssembly()
	ViewMgr:OpenView(AmuletAssemblyView,self.item)
	ViewMgr:CloseView(ItemInfoView)
end

--宠物装备重置
function ItemInfoViewFuncItem:PetEquipReset()
	ViewMgr:OpenView(AmuletResetView,self.item)
	ViewMgr:CloseView(ItemInfoView)
end

--宠物装备分解
function ItemInfoViewFuncItem:PetEquipBreak()
	 AmuletCtrl.Instance:Break(self.item)
	 ViewMgr:CloseView(ItemInfoView)
end
--宠物卸下装备
function ItemInfoViewFuncItem:PetEquipedRemove()
	if MainViewData.Instance:IsBattle() then
		PublicPopupCtrl.Instance:Center(ErrorText[110])
	end
	PetCtrl.Instance:SendRemoveEquip()
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:PetEquipInstensify()
	ViewMgr:OpenView(AmuletIntensifyView, {item = self.item})
	ViewMgr:CloseView(ItemInfoView)
end

--化身宠物卸下装备
function ItemInfoViewFuncItem:AnotherPetEquipedRemove()
	if MainViewData.Instance:IsBattle() then
		PublicPopupCtrl.Instance:Center(ErrorText[110])
	end
	AnotherCtrl.Instance:SendRemoveEquip()
	ViewMgr:CloseView(ItemInfoView)
end

--宠物替换装备
function ItemInfoViewFuncItem:PetEquipedInstead()
	local equip_list = BagData.Instance:GetPetEquipmentList()
    if #equip_list > 0 then
        PetData.Instance.pet_equip_select.equip = 2
        PetData.Instance.pet_equipment_select_another_open = false
        ViewMgr:OpenView(PetEquipSelectView)
    else
        PublicPopupCtrl.Instance:Center(ErrorText[151])
    end
	ViewMgr:CloseView(ItemInfoView)
end

--化身宠物替换装备
function ItemInfoViewFuncItem:AnotherPetEquipedInstead()
	local equip_list = AnotherData.Instance:GetPetEquipmentList()
    if #equip_list > 0 then
        PetData.Instance.pet_equip_select.equip = 2
        PetData.Instance.pet_equipment_select_another_open = true
        ViewMgr:OpenView(PetEquipSelectView)
    else
        PublicPopupCtrl.Instance:Center(ErrorText[151])
    end
	ViewMgr:CloseView(ItemInfoView)
end


--水晶装备到伙伴上
function ItemInfoViewFuncItem:PartnerEquip()
	if MainViewData.Instance:IsBattle(true) then
		return
	end
	ViewMgr:OpenView(PartnerView)
	ViewMgr:CloseView(ItemInfoView)
end 

function ItemInfoViewFuncItem:AnotherPetEquip()
--	AnotherData.Instance.change_page.cur_page_index = 3
	AnotherCtrl.Instance:SelectFunc(3)
	ViewMgr:CloseView(ItemInfoView)
end

--伙伴装备分解
function ItemInfoViewFuncItem:PartnerEquipBreak()
	MysteryStoreCtrl.Instance:Break()
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:UltiWork()
	WorkshopData.Instance:SetSelectUltiData(self.item)
	ViewMgr:OpenViewByKey(Mod.Equip.Ulti)
	ViewMgr:CloseView(ItemInfoView)
end

--秘境紫装鉴定
function ItemInfoViewFuncItem:CrystalEquipIdentify()
	self.info_data.crystal_equip = Item.Init(self.item.item_id)
	self:Use(1)
end

--伙伴残页合成
function ItemInfoViewFuncItem:PartnerPageCompose()
	PartnerCtrl.Instance:SendPartnerPageCompose(self.item.item_id)
	ViewMgr:CloseView(ItemInfoView)
end

--神装强化跳转
function ItemInfoViewFuncItem:Divine()
	ViewMgr:OpenView(DivineCostumeView,{item = self.item})
	ViewMgr:CloseView(ItemInfoView)
end

--背包中宠物技能书点击使用
function ItemInfoViewFuncItem:PetSkillBookUse()
	local _, open_param = Mod.ParseKey(Mod.Pet.PetGetBook)
	open_param.item_cfg = self.item:Config()
	ViewMgr:OpenView(PetView,open_param)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:MedalSpChange()
	ViewMgr:OpenView(MedalSpChangeView,{item = self.item})
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:MedalMix()
	ViewMgr:OpenView(MedalMixView,{item = self.item})
	ViewMgr:CloseView(ItemInfoView)
end

-- 礼包预览
function ItemInfoViewFuncItem:PreviewGift()
	ChooseGiftUseData.Instance:SetPreviewGift(self.item)
	ViewMgr:OpenView(PreviewGiftView)
	ViewMgr:CloseView(ItemInfoView)
end

-- 魂器卸下
function ItemInfoViewFuncItem:PetWeaponUnfix()
	PetWeaponCtrl.Instance:Unfix(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 魂器穿戴
function ItemInfoViewFuncItem:PetWeaponOnUse()
	PetWeaponCtrl.Instance:OnUse(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 魂器替换
function ItemInfoViewFuncItem:PetWeaponReplace()
	-- local list = BagData.Instance:GetWeaponList()
	-- if TableIsEmpty(list) then
	-- 	PetWeaponCtrl.Instance:NoWeaponTip(Language.PetWeapon.NoWeaponTip1)
	-- else
	-- 	ViewMgr:OpenView(PetWeaponView, self.item)
	-- end
	ViewMgr:OpenView(PetWeaponView, self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 魂器精炼
function ItemInfoViewFuncItem:PetWeaponRefine()
	if self.item:NextRefineCfg() == nil and self.item:RefineLevel() == 0 then
		PublicPopupCtrl.Instance:Error(Language.PetWeapon.NoWeaponTip3)
		return
	end
	local list = BagData.Instance:GetWeaponList()
	if TableIsEmpty(list) then
		PetWeaponCtrl.Instance:NoWeaponTip(Language.PetWeapon.NoWeaponTip2)
	else
		ViewMgr:OpenView(PetWeaponRefineView)
		PetWeaponData.Instance:SetCurCtrlWeapon(self.item)
	end

	ViewMgr:CloseView(ItemInfoView)
end

-- 魂器强化
function ItemInfoViewFuncItem:PetWeaponIntensify()
	local list = BagData.Instance:GetWeaponBagList()
	local remind_num = PetWeaponData.Instance:GetIntensifyRemind(self.item)
	if TableIsEmpty(list) and (remind_num == nil or remind_num < 1) then
		PetWeaponCtrl.Instance:NoWeaponTip(Language.PetWeapon.NoWeaponTip1)
		PetWeaponData.Instance:SetCurCtrlWeapon(nil)
	else
		if self.item:IsFullLevel() and not self.item:IsNeedBreak() then
			PublicPopupCtrl.Instance:Center(Language.PetWeapon.Error8)
		else
			PetWeaponData.Instance:SetCurCtrlWeapon(self.item)
			ViewMgr:OpenView(PetWeaponIntensifyView)
		end
	end

	ViewMgr:CloseView(ItemInfoView)
end

-- 魂器转化
function ItemInfoViewFuncItem:PetWeaponChange()
	ViewMgr:OpenView(PetWeaponChangeView, {pet_weawpon = self.item})
end

-- 神印卸下
function ItemInfoViewFuncItem:PetStampUnfix()
	PetStampCtrl.Instance:Unfix(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 神印穿戴
function ItemInfoViewFuncItem:PetStampOnUse()
	if self.item:PetIndex() then
		PublicPopupCtrl.Instance:AlertTip(Language.PetStamp.Tip5,function ()
			PetStampCtrl.Instance:OnUse(self.item)
			ViewMgr:CloseView(ItemInfoView)
		end)
	else
		PetStampCtrl.Instance:OnUse(self.item)
		ViewMgr:CloseView(ItemInfoView)
	end
end

-- 神印替换
function ItemInfoViewFuncItem:PetStampReplace()
	PetStampCtrl.Instance:OnUse(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 神印分解
function ItemInfoViewFuncItem:PetStampResolve()
	PetStampCtrl.Instance:Resolve(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 神印强化
function ItemInfoViewFuncItem:PetStampIntensify()
	PetStampData.Instance:SetIntensifyStamp(self.item)
	ViewMgr:OpenView(PetStampIntensifyView)
	ViewMgr:CloseView(ItemInfoView)
end

-- 神印方案取下
function ItemInfoViewFuncItem:PetStampBlueprintUnfix()
	local buleprint = PetStampData.Instance:GetCurBuleprint()
	if buleprint then
		buleprint:UnfixStamp(self.item:PosIndex())
	end
	ViewMgr:CloseView(ItemInfoView)
end

-- 神印方案放入
function ItemInfoViewFuncItem:PetStampBlueprintPut()
	local buleprint = PetStampData.Instance:GetCurBuleprint()
	if buleprint then
		buleprint:PutStamp(self.item, true)
	end
	ViewMgr:CloseView(ItemInfoView)
end

-- 神印方案替换
function ItemInfoViewFuncItem:PetStampBlueprintReplace()
	local buleprint = PetStampData.Instance:GetCurBuleprint()
	if buleprint then
		buleprint:PutStamp(self.item, true)
	end
	ViewMgr:CloseView(ItemInfoView)
end

-- 铸灵使用
function ItemInfoViewFuncItem:SpiritStoneUse()
	ViewMgr:OpenViewByKey(Mod.Equip.Spirit)
	ViewMgr:CloseView(ItemInfoView)
end

-- 铸灵升级
function ItemInfoViewFuncItem:SpiritStoneLevelUp()
	if EquipData.Instance.now_select_equip_data then
		EquipData.Instance:OpenSpiritLevelView()
	else
		-- 先清除一遍数据
		EquipData.Instance.spirit_level_data:Set({spirit_level_bag = nil, spirit_level_equip = nil, spirit_level_config = nil, now_select_data = {}})
		EquipData.Instance.spirit_level_data.spirit_level_bag = self.item
	end
	ViewMgr:OpenView(SpiritLevelView)
	ViewMgr:CloseView(ItemInfoView)
end

-- 铸灵分解
function ItemInfoViewFuncItem:SpiritStoneResolve()
	ViewMgr:OpenView(SpiritResolveView)
	ViewMgr:CloseView(ItemInfoView)
end

-- 铸灵卸下
function ItemInfoViewFuncItem:SpiritStoneRemove()
	EquipData.Instance:SendRemoveInfo()
	ViewMgr:CloseView(ItemInfoView)
end

-- 铸灵替换
function ItemInfoViewFuncItem:SpiritStoneReplace()
	local func = EquipData.Instance.spirit_replace_fun
	if type(func) == "function" then
		func()
	end
	ViewMgr:CloseView(ItemInfoView)
end

-- 配饰穿戴
function ItemInfoViewFuncItem:AccrssoryUse()
	if not FlyUpData.Instance:IsFinish() and self.item:IsFlyAccrssory() then
		PublicPopupCtrl.Instance:Center(Language.Accrssory.NeedFlyTip)
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	local param_t = {}
	param_t.req_type = AccrssoryReqType.Equip
	param_t.param_2 = self.item:GridIndex()
	param_t.param_3 = self.item:AccrssoryType()
	AccrssoryCtrl.Instance:SendAccrssoryReq(param_t)
	ViewMgr:CloseView(ItemInfoView)
end

-- 配饰分解
function ItemInfoViewFuncItem:AccrssoryResolve()

	-- 界面特殊性 不能走通用逻辑
	-- 该界面的tabber为定制
	local view_info = ViewMgr:getViewInfo(AccrssoryView)
	view_info.view_mono.Tabbar:ClickItem(4)
	-- ViewMgr:OpenView(AccrssoryResolveView,{item = self.item})
	ViewMgr:CloseView(ItemInfoView)
end

-- 配饰卸下
function ItemInfoViewFuncItem:AccrssoryRemove()
	local param_t = {}
	param_t.req_type = AccrssoryReqType.Remove
	param_t.param_1 = self.item:AccrssoryType()
	AccrssoryCtrl.Instance:SendAccrssoryReq(param_t)
	ViewMgr:CloseView(ItemInfoView)
end

-- 配饰置换
function ItemInfoViewFuncItem:AccrssoryChange()
	AccrssoryData.Instance.change_data.now = self.item
	ViewMgr:OpenView(AccrssoryChangeView)
	ViewMgr:CloseView(ItemInfoView)
end

-- 灵宝穿戴
function ItemInfoViewFuncItem:LingBaoUse()
	LingBaoCtrl.Instance:SendReq(LingBaoReqType.WareLingBao, LingBaoData.Instance:MainShengHenType(), self.item:GridIndex())
	ViewMgr:CloseView(ItemInfoView)
end

-- 天引琉璃使用
function ItemInfoViewFuncItem:GlassTripod()
	local mod_key = self.item:ModKey()
	if not StringIsEmpty(mod_key) then
		ViewMgr:OpenViewByKey(mod_key)
		ViewMgr:CloseView(ItemInfoView)
		RoleSkillData.Instance.use_glass_tripod = true
	end
end

function ItemInfoViewFuncItem:PetBookChange()
	ViewMgr:OpenView(PetBookChangeView, {item = self.item})
end

function ItemInfoViewFuncItem:NevealResolve()
	ViewMgr:CloseView(ItemInfoView)

	BagData.Instance:SetPreNevealResolveItem(self.item)
	ViewMgr:OpenView(WorkshopNevealResolveView)
end

function ItemInfoViewFuncItem:BlazeStar()
	WorkshopData.Instance:SetAutoClickBlazeStar(self.item)

	ViewMgr:CloseView(ItemInfoView)
	ViewMgr:OpenViewByKey(Mod.Workshop.BlazeStar)
end

function ItemInfoViewFuncItem:Neveal()
	WorkshopData.Instance:SetAutoClickNeveal(self.item)

	ViewMgr:CloseView(ItemInfoView)
	ViewMgr:OpenViewByKey(Mod.Equip.Neveal)
end

function ItemInfoViewFuncItem:NevealExtralPlus()
	local is_open,str = FunOpen.Instance:GetFunIsOpened(Mod.Equip.Neveal)
	if not is_open then 
		PublicPopupCtrl.Instance:Center(str)
		return 
	end 

	WorkshopData.Instance:SetMainNevealExtraPlus(self.item)

	ViewMgr:CloseView(ItemInfoView)
	ViewMgr:OpenViewByKey(Mod.NevealExtraPlus.Main)
end

-- 赠送
function ItemInfoViewFuncItem:LiveGive()
	SocietyData.Instance:SetSocietySendInfo(SocietyConfig.ShareType.LiveItem, 0, self.item.item_id)
	ViewMgr:OpenView(SocietySendView)
	ViewMgr:CloseView(ItemInfoView)
end

-- 宠物碎片批量合成
function ItemInfoViewFuncItem:PetMulitiPieces1()
	PetData.Instance:SetMultiPiecesType(1)
	ViewMgr:OpenView(PetMultiPieces, {item = self.item})
end

-- 宠物碎片批量合成并分解
function ItemInfoViewFuncItem:PetMulitiPieces2()
	PetData.Instance:SetMultiPiecesType(2)
	ViewMgr:OpenView(PetMultiPieces, {item = self.item})
end

-- 神印置换
function ItemInfoViewFuncItem:PetStampChange()
	PetData.Instance:SetChangeBook(1, self.item)
	ViewMgr:OpenView(PetStampChangeView)
end

-- 宠物徽章合成
function ItemInfoViewFuncItem:PetBagdeCpd()
	local item_id = self.item.item_id
	ViewMgr:OpenView(PetBagdeCpd, {item_id = item_id})
	ViewMgr:CloseView(ItemInfoView)
end

-- 宠物徽章替换
function ItemInfoViewFuncItem:PetBagdeReplace()
	ViewMgr:OpenView(PetBagdeSelect)
	ViewMgr:CloseView(ItemInfoView)
end

-- 宠物徽章卸下
function ItemInfoViewFuncItem:PetBagdeUnfix()
	local pet_index = self.item:PetIndex()
	if pet_index then
		PetCtrl.SendPetOp(PET_OP_TYPE.PetBagdeTakeOff, pet_index)
	end
	ViewMgr:CloseView(ItemInfoView)
end

-- 宠物徽章分解
function ItemInfoViewFuncItem:PetBagdeResolve()
	PetBagdeData.Instance:AddReslove(self.item, true)
	ViewMgr:OpenView(PetBagdeResolve)
	ViewMgr:CloseView(ItemInfoView)
end

--缘牵同心结物品发送
function ItemInfoViewFuncItem:ConcentricKnotSend()
	ConcentricKnotCtrl.Instance:SendInvite(self.item)
	ViewMgr:CloseView(ItemInfoView)
	ViewMgr:CloseView(BagView)
end

function ItemInfoViewFuncItem:SpecialCompose()
	if self.item.item_id and self.item.item_id > 0 then
		local cfg = Cfg.SpecialCompose(self.item.item_id)
		if cfg then
			WorkshopCtrl.Instance:SendOperateReq(WORLSHOP_OP.WORKSHOP_OP_COMPOUND_REQ, cfg.seq, 1)
		else
			PublicPopupCtrl.Instance:Center(Language.ItemInfo.Tips.NoSpecialComposeCfg)
		end
		ViewMgr:CloseView(ItemInfoView)
	end
end

function ItemInfoViewFuncItem:MarbasEquip()
	MarbasCtrl.Instance:SendEquipOper(1,self.item.index)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:MarbasUnFix()
	MarbasCtrl.Instance:SendEquipOper(2,self.item.index)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:MarbasChange()
	if not MarbasData.Instance:IsWithEquipInBag() then 
        PublicPopupCtrl.Instance:Center(Language.Marbas.BaseEmptyTips)
        return 
    end 

	ViewMgr:OpenView(MarbasBagView)
	ViewMgr:CloseView(ItemInfoView)
end

-- 穿石头
function ItemInfoViewFuncItem:MarbasBloodGemEquip()
	MarbasCtrl.Instance:GemEquip(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 换石头
function ItemInfoViewFuncItem:MarbasBloodGemChange()
	MarbasCtrl.Instance:GemChange(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 卸石头
function ItemInfoViewFuncItem:MarbasBloodGemUnfix()
	MarbasCtrl.Instance:GemUnfix(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 升级石头
function ItemInfoViewFuncItem:MarbasBloodGemPlus()
	MarbasData.Instance:SetBloodSetGemPlus(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 圣器出战
function ItemInfoViewFuncItem:ShengQiFight()
	ShengQiCtrl.Instance:SetShengQiState(self.item, ShengQiCfg.State.Fight)
	ViewMgr:CloseView(ItemInfoView)
end

-- 圣器待机
function ItemInfoViewFuncItem:ShengQiReady()
	ShengQiCtrl.Instance:SetShengQiState(self.item, ShengQiCfg.State.Ready)
	ViewMgr:CloseView(ItemInfoView)
end

-- 圣器下阵
function ItemInfoViewFuncItem:ShengQiRest()
	ShengQiCtrl.Instance:SetShengQiState(self.item, ShengQiCfg.State.Rest)
	ViewMgr:CloseView(ItemInfoView)
end

-- 符石镶嵌
function ItemInfoViewFuncItem:FuShiUse()
	ShengQiCtrl.Instance:UseFuShi(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 符石替换
function ItemInfoViewFuncItem:FuShiRep()
	ShengQiCtrl.Instance:UseFuShi(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 符石卸下
function ItemInfoViewFuncItem:FuShiUnfix()
	ShengQiCtrl.Instance:UnfixFuShi(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

-- 符石重铸
function ItemInfoViewFuncItem:FuShiReset()
	FuShiData.Instance:SetResetFushi(self.item)
	ViewMgr:OpenView(FuShiResetView)
	ViewMgr:CloseView(ItemInfoView)
end

-- 符石合成
function ItemInfoViewFuncItem:FuShiFusion()
	ViewMgr:OpenView(FuShiFusionView)
	ViewMgr:CloseView(ItemInfoView)
end

-- 分解石头
function ItemInfoViewFuncItem:MarbasBloodGemDecompose()
	MarbasCtrl.Instance:GemDecompose(self.item)

	ViewMgr:CloseView(ItemInfoView)
end

-- 批量分解石头
function ItemInfoViewFuncItem:MarbasBloodMulitGemDecompose()
	MarbasData.Instance:SetPreGemResolveItem(self.item)
	ViewMgr:OpenViewByKey(Mod.Marbas.GemResolve)

	ViewMgr:CloseView(ItemInfoView)
end

-- 合成石头
function ItemInfoViewFuncItem:MarbasBloodGemCompose()
	MarbasCtrl.Instance:GemCompose(self.item)

	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:Recycling()
	ViewMgr:OpenView(RewardView,{mod_key = Mod.Reward.FundExchange})
end

function ItemInfoViewFuncItem:ShengQiSkillChange()
	local item = Item.Create({item_id = self.item.item_id, index = self.item.index, num = 1, is_bind = self.item:Bind()})
	PetData.Instance:SetChangeBook(1, item)
	ViewMgr:OpenView(ShengQiBookChangeView)
end

function ItemInfoViewFuncItem:FuYuTakeOff()
	PartnerCtrl.Instance:ReqFuYuTakeOff(self.item.partner_id, self.item.wear_idx)
	ViewMgr:CloseView(ItemInfoView)
end
function ItemInfoViewFuncItem:FuYuChange()
	if ViewMgr:IsOpen(PartnerFuYuBag) then
		PartnerData.Instance.fuyu_change:Notify()
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	local isopen, tip = FunOpen.Instance:GetFunIsOpened(Mod.Partner.PartnerFuyu)
	if not isopen then
		PublicPopupCtrl.Instance:Center(tip)
		return
	end
	--[[ if  PartnerData.Instance:FlyUpPartnerListNum() <= 0 then
		PublicPopupCtrl.Instance:Center(Language.FuYu.FlyupNumTip)
		return
	end ]]
	ViewMgr:OpenView(PartnerFuYuBag)
	ViewMgr:CloseView(ItemInfoView)
end
function ItemInfoViewFuncItem:FuYuUse()
	self:FuYuChange()
--[[ 	if ViewMgr:IsOpen(BagView) then
		ViewMgr:CloseView(BagView)
	end ]]
end
function ItemInfoViewFuncItem:FuYuQiangHua()
	if self.item:FuYuType() == PartnerConfig.FuYuType.Exclusive then
		PublicPopupCtrl.Instance:Center(Language.FuYu.ExFuYu)
		return
	end
	if self.item:FuYuLevel() >= Config.fuyu_auto.other[1].fuyu_max_level then
		PublicPopupCtrl.Instance:Center(Language.FuYu.MaxLevel)
		return
	end
	PartnerData.Instance:OpenFuYuQiangHua(self.item)
	--设置强化物品
	ViewMgr:CloseView(ItemInfoView)
end
function ItemInfoViewFuncItem:FuYuFenJie()
	ViewMgr:OpenView(PartnerFuYuDecom)
	ViewMgr:CloseView(ItemInfoView)
end

-- 灵骑装备转换
function ItemInfoViewFuncItem:LingQiChangeEquip()
	BicycleData.Instance:ChangeItemType(BicycleCfg.ChangeItem.Equip)
	BicycleData.Instance:ChangeItemNow(self.item)
	ViewMgr:OpenView(BicycleChangeView)
	ViewMgr:CloseView(ItemInfoView)
end

-- 灵骑圣物转换
function ItemInfoViewFuncItem:LingQiChangeHalidom()
	BicycleData.Instance:ChangeItemType(BicycleCfg.ChangeItem.Halidom)
	BicycleData.Instance:ChangeItemNow(self.item)
	ViewMgr:OpenView(BicycleChangeView)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:BatchDig()
	if not RewardData.Instance:IsBuyWeekAndMonth() then
		PublicPopupCtrl.Instance:Center(Language.ItemInfo.CantBatchDig)
		return
	end
	local data = {}
	data.desc = Language.ItemInfo.BatchUseMap
	data.item = self.item
	data.use_func = function(num)
		TreasureMapCtrl.Instance:SendTreasureMapReq({req_type = TREASURE_MAP_REQ_TYPE.BATCH_USE, param1 = num})
		ViewMgr:CloseView(BatchUseView)
	end
	ItemInfoCtrl.Instance:BatchUse(data)
	ViewMgr:CloseView(ItemInfoView)
	-- 打开批量使用界面
	-- AllSoulsStorybookReward
end

-- 时装转换后道具分解
function ItemInfoViewFuncItem:IllusionChangeResolve()
	local item_id = self.item.item_id
	local num = self.item:Num()
	local cfg = Cfg.IllusionResolveId(item_id)
	local str = Format(Language.IllusionUp.ResolveTip, Item.GetQuaName(item_id), cfg.decompose_num * num, Item.GetQuaName(cfg.decompose_item))
	PublicPopupCtrl.Instance:AlertTip(str, function()
		BagCtrl.Instance:SendBagDecompose(self.item:GridColumn(), self.item:GridIndex(), num)
	end)
	ViewMgr:CloseView(ItemInfoView)
end


function ItemInfoViewFuncItem:BicycleSkillCompose()
	BicycleOtherData.Instance:SetAStuff(self.item)
	ViewMgr:OpenView(BicycleSkillComposeView)
end
-- 神印吞噬
function ItemInfoViewFuncItem:PetStampSwallow()
	PetStampData.Instance:SetSwallowMain(self.item)
	ViewMgr:OpenView(PetStampSwallowView)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:PetStampDrop()
	PublicPopupCtrl.Instance:AlertTip(Language.PetStamp.Tip10, function ()
		PetCtrl.SendPetOp(PET_OP_TYPE.PetStampDrop, self.item.index)
		ViewMgr:CloseView(ItemInfoView)
	end)
end

function ItemInfoViewFuncItem:FuYuSwitch()
	PartnerData.Instance:SetChangeFuYu(self.item)
	ViewMgr:OpenView(PartnerFuYuChange)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:FuYuShengHua()
	PartnerData.Instance:SetSubFuYu(self.item)
	ViewMgr:OpenView(PartnerFuYuSublimation)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:HuanShouResolve()
	ViewMgr:OpenView(HuanShouSoulResolve, {sel_item = self.item})
end

function ItemInfoViewFuncItem:DragonSoulUp()
	FairyDragonSoulData.Instance:SetUpSoul(self.item)
	ViewMgr:OpenView(FiaryDragonSoulUpView)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:DragonSoulRep()
	FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.WEAR_SOUL, self.item.index)
	ViewMgr:CloseView(ItemInfoView)
	ViewMgr:CloseView(FairyDragonSoulBagView)
end

function ItemInfoViewFuncItem:DragonSoulRep2()
	FairyDragonSoulData.Instance:SetBagSelType(self.item:SoulType())
	ViewMgr:OpenView(FairyDragonSoulBagView)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:DragonSoulUse()
	if not FairyDragon.Instance:IsActive() then
		PublicPopupCtrl.Instance:Error(Language.FairyDragon.ActiveTip)
		ViewMgr:CloseView(ItemInfoView)
		return
	end
	FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.WEAR_SOUL, self.item.index)
	ViewMgr:CloseView(ItemInfoView)
	ViewMgr:CloseView(FairyDragonSoulBagView)
end

function ItemInfoViewFuncItem:DragonSoulUnfix()
	local soul_type = self.item:SoulType()
	FairyDragonCtrl.Instance:SendReq(FairyDragonCfg.REQ_TYPE.UNFIX_SOUL, soul_type)
	ViewMgr:CloseView(ItemInfoView)
end


function ItemInfoViewFuncItem:EquipConversion()
	EquipData.Instance:SetChangeEquip(self.item)
	ViewMgr:OpenView(EquipConversion)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:FuShiUpLevel()
	ShengQiCtrl.Instance:UpLevelFushi(self.item)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:LingQiYiJianHeCheng()
	ViewMgr:OpenView(BicycleSkillBookAllUp)
	ViewMgr:CloseView(ItemInfoView)
end

function ItemInfoViewFuncItem:YouMingEquipUnfix()
	local youming = YouMingData.Instance:GetSelectYouMing()
	YouMingCtrl.Instance:SendReq(YouMingCfg.ReqType.TAKE_OFF_EQUIP, youming.index, self.item.index - 1)
	ViewMgr:CloseView(ItemInfoView)
end
function ItemInfoViewFuncItem:YouMingTiHuang()
	ViewMgr:OpenView(MingZhuangBagView)
	ViewMgr:CloseView(ItemInfoView)
end
function ItemInfoViewFuncItem:YouMingTiHuang2()
	self:Use()
	ViewMgr:CloseView(ItemInfoView)
end