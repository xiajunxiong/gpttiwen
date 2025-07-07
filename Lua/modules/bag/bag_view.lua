BagView = BagView or DeclareView("BagView", "bag/bag", Mod.Bag.Main)

VIEW_DECLARE_LEVEL(BagView,ViewLevel.L1)

function BagView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(), Language.Bag.Bag)
	if IS_AUDIT_VERSION and not GLOBAL_CONFIG:PkgInfo().is_youyan then
		self.Board:SetAuditBgShow()
	end
	if FunOpen.Instance:IsFunOpen(Mod.Store.Diamond) then
        self.Currency:SetCurrency(CurrencyType.CoinBind,CurrencyType.ImmortalCoin,CurrencyType.Gold, CurrencyType.Diamond)
    else
        self.Currency:DefaultSet()
    end
	self.BagTabs[(param_t ~= nil and param_t.col or ItemColumnType.Item) + 1]:Trigger()
	self.OtherPanel:FlushXunZhang()
	
	if IS_AUDIT_VERSION then
		for i = 1,self.AuditHide:Length() do
			self.AuditHide[i]:SetActive(GLOBAL_CONFIG:PkgInfo().is_youyan)
		end
		for i = 1,self.AuditShow:Length() do
			self.AuditShow[i]:SetActive(not GLOBAL_CONFIG:PkgInfo().is_youyan)
		end
		if not GLOBAL_CONFIG:PkgInfo().is_youyan then
			local audit_seq = AudutSeqIndex[GLOBAL_CONFIG:Urls().audit_seq] and GLOBAL_CONFIG:Urls().audit_seq or 1
			UH.SpriteName(self.AuditStoreBtnImg, "BagStoreAudit"..audit_seq)
			UH.SpriteName(self.AuditTidyBtnImg, "BagTidyAudit"..audit_seq)
		end
	else
		for i = 1,self.AuditHide:Length() do
			self.AuditHide[i]:SetActive(true)
		end
		for i = 1,self.AuditShow:Length() do
			self.AuditShow[i]:SetActive(false)
		end
	end
end

function BagView:CloseCallback()
	BagData.Instance:ViewData().selected_col = ItemColumnType.None
	BagData.Instance:ClearNewItem()
	BagData.Instance:CheckIsBetter()
	-- BagData.Instance:SetCrystalRedPiont(false)
	MainOtherCtrl.Instance:WaitViewFunc()
end

function BagView:OnClickStorage()
	ViewMgr:OpenView(StorageView)
end

function BagView:OnClickColumn(col_type)
	BagData.Instance:ViewData().selected_col = col_type
	self:FlushColumnBtnNew(BagData.Instance:ViewData().selected_col)
	local pos = self.ScrollRect.localPosition
	if pos.y > 0 then
		pos.y = 0
		self.ScrollRect.localPosition = pos
	end
	AudioMgr:PlayEffect(AudioType.UI, "bag_col_change")
end

function BagView:FlushColumnBtnNew(col_type)
	if self.selected_new_type ~= nil then 
		self.NewTipsGroup[ self.selected_new_type + 1 ]:SetActive(false)
		BagData.Instance:ClearNewItemsByColType(self.selected_new_type)
		self.selected_new_type = nil
	end 
	self.selected_new_type = col_type
end

function BagView:OnUnlockClick()
	local col_type = BagData.Instance:ViewData().selected_col
	local cfg = BagData.Instance:GetUnLockNumConfig(col_type)
	local _info = BagData.Instance:GetColumnInfo(col_type)
	
	local function send_msg()
		local param_t = {}
		param_t.oper_type = BAG_OPERA_TYPE.OPERA_TYPE_EXTEND
		param_t.param_1 = col_type
		param_t.param_2 = cfg.add_num
		BagCtrl.Instance:SendKnapsackOperaReq(param_t)
		if self.locksRect then
			UH.LocalPosG(self.unlockeff, self.locksRect.localPosition)
		end
		
		if RoleData.Instance:GetCurrency(CurrencyType.CoinBind) >  _info.expand_price then 
		    for i = 1,5 do 
			    if self.EffectTool.isActiveAndEnabled then 
			        self.EffectTool:Play(10010297,self.UnlockEff[i])
			    end 
			    AudioMgr:PlayEffect(AudioType.UI, "bag_unlock_eff")
		    end 
	    end 
		ViewMgr:CloseView(DialogTipsView)
	end
	local info = {
		content = string.format(Language.Bag.UnLock, cfg.add_num or 0, _info.expand_price or 0),
		cancel = {
			name = nil,
			func = nil
		},
		confirm = {
			name = nil,
			func = send_msg
		}
	}
	PublicPopupCtrl.Instance:DialogTips(info)
	AudioMgr:PlayEffect(AudioType.UI, "bag_unlock_pop")
end

BagViewOther = BagViewOther or DeclareMono("BagViewOther", UIWFlushPanel)
--Name=======Text
--Title======Text
--Element====UIWElementAttribute

--XunZhangTip
--EquipList
--Score======text
--ScoreLevel======

-- XunZhangSpGroup=======UIImageSpriteSet
-- XunZhangSpInvalidGroup=======UIImageSpriteSet
function BagViewOther:BagViewOther()
	self.data = BagData.Instance

	self.data_cares = {
		{data = self.data:GetEquipList(), func = self.FlushEquipList},
		{data = self.data:GetJewelryList(), func = self.FlushJewelryList},
		{data = self.data:ViewData(), func = self.FlushElemetCrystal, keys = {"element_crystal_id", "used_times"}},
		{data = self.data:ViewData(), func = self.FlushXunZhang, keys = {"medal_info"}},
		{data = RoleData.Instance:GetBaseData(), func = self.FlushRoleBaseData, keys = {"name","title_id","appearance"}},
		{data = RoleData.Instance:GetBaseData(), func = self.FlushRoleCapability, keys = {"capability"}},
		{data = self.data:GetEquipList(), func = self.FlushRoleBaseData},
		{data = self.data.new_item,func = self.FlushItemGroup},
		{data = self.data:GetColumnGrids(ItemColumnType.Item), func = self.FlushElemetCrystalPiont, init_call = false},
		{data = EquipSetData.Instance.bag_release,func = self.CheckEquipSetShowRelease},
		-- {data = AccrssoryData.Instance.remind_data, func = self.FlushAccrssoryRemind, init_call = false},
	}

	self.medal_effect_hal = {}

	-- FlyStarEffectCell
	for i = 1,8 do 
		local param_t = {mono = self,
		start_effect = "30000817",
		pre_pause = 1,
		end_func = BindTool.Bind2(self.EquipShowComplete, self,i),
	    end_func_time = 0}
		self.EffectCells[i]:SetData(param_t)
	end
	-- self:FlushAccrssoryEnter()
	-- self:FlushAccrssoryRemind()
end

function BagViewOther:FlushRoleBaseData()
	local data = RoleData.Instance
	local base_data = data:GetBaseData()
	self.Name.text = base_data.name
	self.Title.text = base_data.title_id ~= 0 and string.format(Language.Bag.Title, TitleData.GetTitleName(base_data.title_id)) or ""
	self:FlushRoleModel()
end

-- 装备更新后根据装备属性更新评分
function BagViewOther:FlushRoleCapability()
	local role_score,score_level = RoleData.Instance:GetRoleScore()
    UH.SetText(self.Score, role_score)
	UH.SpriteName(self.ScoreRare, "scoreLevel"..score_level)
end

function BagViewOther:FlushRoleModel()
	-- 刷新变身卡模型
	local aperance = RoleData.Instance:GetApearance()
	local card_model_res_id = LiveData.Instance:CardModel(aperance)
	self.ModelShow.gameObject.transform.parent:SetObjActive(card_model_res_id == nil)
    self.NPCModelShow.gameObject.transform.parent:SetObjActive(card_model_res_id ~= nil)
	if card_model_res_id then
		self:SetNpcModel(card_model_res_id)
	else
		if not self.ui_obj then
			self.ui_obj = UIChDrawer.CopyMainRoleData()
			self.ModelShow:SetShowData(
				{
					ui_obj = self.ui_obj,
					view = self
				}
			)
		else 
			self.ui_obj:SetData(RoleData.Instance:GetApearance())
		end
	end
end

function BagViewOther:SetNpcModel(card_model_res_id)
    if not self.npc_ui_obj then
        self.npc_ui_obj = UIObjDrawer.New()
        self.NPCModelShow:SetShowData({ui_obj = self.npc_ui_obj,view = self})
    end
    self.npc_ui_obj:SetData(DrawerHelper.GetNpcPath(card_model_res_id))
end

function BagViewOther:OnDestroy()
	if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
	end
	if self.npc_ui_obj then
        self.npc_ui_obj:DeleteMe()
        self.npc_ui_obj = nil
    end
	if self.tidy_countdown_handle then 
		TimeHelper:CancelTimer(self.tidy_countdown_handle)
		self.tidy_countdown_handle = nil
	end 
    UIWFlushPanel.OnDestroy(self)
end

function BagViewOther:FlushItemGroup()
	for i = 0,ItemColumnType.Illustration do 
		self.NewTipsGroup[i + 1]:SetActive(self.data:GetNewItemCols()[i] ~= nil)
	end 

	self.data:ItemDataCheck()
end

function BagViewOther:FlushSuitChange(data)
	local flag,list = EquipSetData.Instance:GetRoleSuitPlus(data)
	local not_empty = EquipSetData.Instance:CheckIsEmptySuit(data:GetEquipSet())
	if flag and not_empty then 
		for k,v in pairs(list) do 
			local operate_pos = data:IsEquip() and v + 1 or v + 6
			self.EffectCells[operate_pos]:Play("30000826")
		end 
		self.equip_set_show_mark = data
		AudioMgr:PlayEffect(AudioType.UI,  CommonAudio.ComposeSucc)
	end 
end 

function BagViewOther:CheckEquipSetShowRelease()
	for i = 1,8 do 
	    self.EffectCells[i]:AllHandleCancel()
	end 
end

function BagViewOther:EquipShowComplete(index)
	if not ViewMgr:IsOpen(EquipSetShowView) then 
	    ViewMgr:OpenView(EquipSetShowView,{item_data = self.equip_set_show_mark})
	end  
end

function BagViewOther:FlushEquipList()
	local list = self.data:GetEquipList()
	for i = 0, 4 do --八件套--4
		local equip = list[i]
		if equip then
			local old_data = self.EquipList[i + 1]:GetData()
			if old_data ~= equip then
				self.EquipList[i + 1]:SetData(equip)
				if self.EffectTool.isActiveAndEnabled then
					self.EffectTool:Play(10010258,self.EquipList[i + 1].gameObject)
					if old_data ~= nil and old_data.item_id ~= equip.item_id then 
					    self:FlushSuitChange(equip)
					end 
				end
			end
		else
			self.EquipList[i + 1]:SetData(Item.New())
		end
	end

	local mix_link_list = EquipSetData.Instance:GetMixRoleSuitDescList()
	self.BtnEquipSet.Interactable = #mix_link_list > 0
end

function BagViewOther:FlushJewelryList()
	local list = self.data:GetJewelryList()
	for i = 0, 2 do
		local jewelry = list[i]
		if jewelry then
			local old_data = self.EquipList[i + 6]:GetData()
			if old_data ~= jewelry then
				self.EquipList[i + 6]:SetData(jewelry)
				if self.EffectTool.isActiveAndEnabled then
					self.EffectTool:Play(10010258,self.EquipList[i + 6].gameObject)
					if old_data ~= nil and old_data.item_id ~= jewelry.item_id then 
					    self:FlushSuitChange(jewelry)
					end 
				end
			end
		else
			self.EquipList[i + 6]:SetData(Item.New())
		end
	end

	local mix_link_list = EquipSetData.Instance:GetMixRoleSuitDescList()
	self.BtnEquipSet.Interactable = #mix_link_list > 0
end

function BagViewOther:FlushElemetCrystal()
	local item_id = self.data:ViewData().element_crystal_id
	if item_id == 0 then
		self.ElementObj:SetActive(false)
		self.CrystalNumObj:SetActive(false)
		UH.SetIcon(self.EleCrystal,nil)
		local list = self.data:GetCrystalList()
		self.CrystalPiont:SetNum(#list > 0 and (self.data.crystal_red_point_show and 1 or 0) or self.data:GetCrystalRedPiont())
		return
	end
	local crystal = EleCrystal.New()
	crystal.item_id = item_id
	self.ElementObj:SetActive(true)
	self.Element:SetData(crystal:Element(),true)
	UH.SetIcon(self.EleCrystal,crystal:IconId())
	if self.EffectTool.isActiveAndEnabled then
		self.EffectTool:Play(10010258,self.BtnCrystal)
	end

	self.CrystalNum.text = string.format(Language.EleCrystal.SimpleUseNum, self.data:ViewData().used_times)
	self.CrystalNumObj:SetActive(true)
	self.CrystalPiont:SetNum(0)
end

function BagViewOther:FlushElemetCrystalPiont()
	if not ViewMgr:IsOpen(BagView) then return end 
	if self.data:ViewData().element_crystal_id == 0 then
		local list = self.data:GetCrystalList()
		self.CrystalPiont:SetNum(#list > 0 and (self.data.crystal_red_point_show and 1 or 0) or self.data:GetCrystalRedPiont())
	else
		self.CrystalPiont:SetNum(0)
	end
end

function BagViewOther:OnClickXunZhang(param_int)
	--有勋章打开勋章信息
	-- local flag = FunOpen.Instance:GetFunIsOpened(Mod.Medal.Main)
	-- if flag == false then
	-- 	PublicPopupCtrl.Instance:Center(string.format(ErrorText[76], FunOpen.Instance:GetFunOpenLevel(Mod.Medal.Main)))
	-- 	return
	-- end
	local medal_list = self.data:ViewData().medal_info

    local medal_id =  medal_list[param_int] ~= nil and medal_list[param_int].item_id or 0
	if medal_id == 0 then
		ViewMgr:OpenView(MedalGetWayView,{pos_obj = self.MedalGetPosS[param_int]})
        return
	end

	local _sp_fail,_sp_fail2 = MedalData.Instance:GetActiveEquipMedalSingle(param_int,medal_list)

	local medal = Item.Create({
		item_id = medal_id,
		grid_index = param_int - 1 ,
		is_bind = medal_list[param_int].is_bind,
		param = {
			sp_id = medal_list[param_int].param.sp_id ,
			sp_fail = _sp_fail,
			sp_fail_2 = _sp_fail2,
			attr_list = medal_list[param_int].param.attr_list,
			is_equip = true,
			grid_pos = param_int,
			grid_index = param_int - 1 ,
			grade = medal_list[param_int].param.grade,
			special_medal_grade = medal_list[param_int].param.special_medal_grade,
			second_sp_id = medal_list[param_int].param.second_sp_id
		},
		in_bag = false,
	})
	
    local info = {
		item_list = {medal},
		func = function ()
			if medal_list[param_int].param.sp_id == 0 then 
				return {ItemInfoFunc.UnFix,ItemInfoFunc.MedalUpgrade,ItemInfoFunc.MedalSmelt}
			elseif medal:IsSpecialSp() then 
				if medal_list[param_int].param.special_medal_grade < 2 then 
					return {ItemInfoFunc.UnFix,ItemInfoFunc.MedalSpChange,ItemInfoFunc.MedalMix,ItemInfoFunc.MedalSmelt}
				else
					return {ItemInfoFunc.UnFix,ItemInfoFunc.MedalSpChange,ItemInfoFunc.MedalSmelt}
				end
			else 
				return {ItemInfoFunc.UnFix,ItemInfoFunc.MedalSpChange,ItemInfoFunc.MedalUpgrade,ItemInfoFunc.MedalSmelt}
			end 
		end
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
	-- if self.EffectTool.isActiveAndEnabled then
	-- 	self.EffectTool:Play(10010258,self.BtnXunZhang)
	-- end
end

function BagViewOther:OnClickGetWayClose()
	-- self.MedalGetClose:SetActive(false)
	-- ItemInfoCtrl.Instance:ItemDeriveView()
end

--刷新勋章显示
function BagViewOther:FlushXunZhang()
	local medal_list = self.data:ViewData().medal_info
	for k,v in pairs(medal_list) do
		local ops = v.grid_index+1
		if v.item_id ~= 0 then 
			local medal = Item.Create(v)
			self.MedalCalls[ops]:SetData(medal)
			if self.medal_effect_hal[ops] then 
				self.EffectTool:Stop(self.medal_effect_hal[ops])
				self.medal_effect_hal[ops] = nil
			end 
		else 
			self.MedalPlusS[ops]:SetActive(true)
			if self.EffectTool.isActiveAndEnabled then
			    self.medal_effect_hal[ops] = self.EffectTool:Play(10010854,self.MedalPlusS[ops])
			end 
		end 

		self.MedalPlusS[ops]:SetActive(v.item_id == 0)
		self.MedalObjCalls[ops]:SetActive(v.item_id ~= 0 )
	end 

end

function BagViewOther:OnClickEffectShow()
	local param_t = MedalData.Instance:GetBagEffectShowParam()
	ViewMgr:OpenView(MedalEffectShow,param_t)
end

function BagViewOther:OnClickTidy()
	--取消挖宝
	--MainOtherCtrl.Instance:CloseKeyUse()
	--[[ local beh = SceneLogic.Instance:Beh()
	if beh and type(beh) == "table" and beh.behavior_type == 101 then--即挖宝
		SceneLogic.Instance:CancelBeh()
	end ]]
	if ViewMgr:IsOpen(Compass) then
		ViewMgr:CloseView(Compass)
	end
	local param_t = {
		oper_type = BAG_OPERA_TYPE.OPERA_TYPE_INORDER,
		param_1 = BagData.Instance:ViewData().selected_col
	}
	BagCtrl.Instance:SendKnapsackOperaReq(param_t)
	BagData.Instance:ClearNewItemsByColType(param_t.param_1)
	PrestigeData.Instance:CheckDonateCacheNum()
	AudioMgr:PlayEffect(AudioType.UI, "bag_tidy")

	if self.tidy_countdown_handle ~= nil then 
		TimeHelper:CancelTimer(self.tidy_countdown_handle)
		self.tidy_countdown_handle = nil
	end 
	local end_time = 5
	self.BtnTidyInteractor.Interactable = false
	self.tidy_countdown_handle = TimeHelper:AddCountDownTT(function()
        UH.SetText(self.BtnTidyLabel,Format(Language.Bag.BagTiming,end_time))
        end_time = end_time - 1
	end,
    function()
        UH.SetText(self.BtnTidyLabel,Language.Bag.BagTimed)
        self.BtnTidyInteractor.Interactable = true
	end,5,1,true)
end

function BagViewOther:OnClickCrystal()
	ViewMgr:OpenView(CrystalView,{crystal_func = function()
		BagData.Instance.crystal_red_point_show = false
		BagData.Instance:SetCrystalRedPiont(false)
		self:FlushElemetCrystalPiont()
		return BagData.Instance:GetCrystalList()
    end})
end

function BagViewOther:OnClickEquipSet()
	self.EquipSetshow:SetActive(true)

	local mix_link_list = EquipSetData.Instance:GetMixRoleSuitDescList()
	self.EquipSetEmpty:SetActive(#mix_link_list==0)
	self.EquipSetList:SetData(mix_link_list)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end

function BagViewOther:OnAccrssoryEnterClick()
	ViewMgr:OpenViewByKey(Mod.Accrssory.Main)
end

function BagViewOther:FlushAccrssoryEnter()
	self.IHAccrssoryEnter.Interactable = FunOpen.Instance:IsFunOpen(Mod.Accrssory.Main)
end

function BagViewOther:FlushAccrssoryRemind()
	self.AccrssoryRedPoint:SetNum(AccrssoryCtrl.Instance:GetAccrssoryRemind())
end

function BagViewOther:OnClickCloseEquipSet()
	self.EquipSetshow:SetActive(false)
end

BagViewGrids = BagViewGrids or DeclareMono("BagViewGrids", UIWFlushPanel)
--ItemLists=======UIWidgetList
--Locks===========GameObject
function BagViewGrids:BagViewGrids()
	-- self.flush_events = { EventProtocol.ToEventKey(SCKnapsackInfoAck),EventProtocol.ToEventKey(SCKnapsackItemChange) }
	self.bag_data = BagData.Instance
	local view_data = self.bag_data:ViewData()
	self.data_cares = {
		{data = self.bag_data:GetColumnGrids(ItemColumnType.Item), func = self.ItemGrids, init_call = false},
		{data = self.bag_data.equipment_list, func = self.ItemGrids, init_call = false},
		{data = self.bag_data.jewelry_list, func = self.ItemGrids, init_call = false},
		{data = self.bag_data:GetColumnGrids(ItemColumnType.Supply), func = self.SupplyGrids, init_call = false},
		{data = self.bag_data:GetColumnGrids(ItemColumnType.Task), func = self.TaskGrids, init_call = false},
		{data = self.bag_data:GetColumnGrids(ItemColumnType.Material), func = self.MaterialGrids, init_call = false},
		{data = self.bag_data:GetColumnGrids(ItemColumnType.Illustration), func = self.IllustrationGrids, init_call = false},
		{data = view_data, func = self.BagGrid, keys = {"selected_col"}, init_call = false}
	}
	GuideManager.Instance:RegisterGetGuideUi("BagEquipSelectBtn", 
	function()
		local item_id = GuideData.Instance:GetBgaEquipId()
		local index = self.bag_data:GetBagIndexById(ItemColumnType.Item,item_id)
		local count = self.bag_data:GetCountInBag()
		local value = (index - 5) / count
		value = value < 0 and 0 or value
		self.ItemLists:JumpToDown(1 - value)
		if self.ItemLists.index_item and self.ItemLists.index_item[index] then
			return self.ItemLists.index_item[index - 1],function()
				self.ItemLists.index_item[index - 1]:OnClick()
			end
		end
	end)
	GuideManager.Instance:RegisterGetGuideUi("EquipDonationSelectBtn", 
	function()
		local item_id = Config.function_guide_auto.pet_skill[1].equipment_id
		local index = self.bag_data:GetBagIndexById(ItemColumnType.Item,item_id)
		if index == nil then -- 若装备消失则指引终止
			GuideManager.Instance:ForceStop()			
		end 
		local count = self.bag_data:GetCountInBag()
		local value = (index - 5) / count
		value = value < 0 and 0 or value
		self.ItemLists:JumpToDown(1 - value)
		if self.ItemLists.index_item and self.ItemLists.index_item[index] then
			self.donation_guide = nil
			return self.ItemLists.index_item[index - 1],function()
				self.ItemLists.index_item[index - 1]:OnClick()
			end
		end
	end)
	GuideManager.Instance:RegisterGetGuideUi("BagResolveEquip",
	function ()
		local item_id = GuideData.Instance:GetResolveItem()
		return self:GuideClick(item_id)
	end)
	GuideManager.Instance:RegisterGetGuideUi("BagPetPiecesCompose",
	function ()
		local item_id = GuideData.Instance:GetPetPieceId()
		return self:GuideClick(item_id)
	end)

	GuideManager.Instance:RegisterGetGuideUi("SilverEggSelect",
	function ()
		local item_id = 28434
		return self:GuideClick(item_id)
	end)

	local item_id = GuideData.Instance:TreasureItem()
	GuideManager.Instance:RegisterGetGuideUi("BagTreasureMap",function ()
		--[[ local index = self.bag_data:GetBagIndexById(ItemColumnType.Item, item_id)
		local count = self.bag_data:GetCountInBag()
		local value = (index - 5) / count
		value = value < 0 and 0 or value
		self.ItemLists:JumpToDown(1 - value)
		if self.ItemLists.index_item and self.ItemLists.index_item[index - 1] then
			local item = self.ItemLists.index_item[index - 1]
			return item,function()
				item:OnClick()
				SceneLogic.Instance:AutoToPos(item:GetData().param.scene_id, item:GetData().param, BehaviorFactory.TreasureMap(item:GetData()), true, true, nil)
			end
		end ]]
		return self:GuideClick(item_id)
	end)
end
function BagViewGrids:OnDestroy()
	GuideManager.Instance:UnRegisterGetGuideUi("BagPetPiecesCompose")
	GuideManager.Instance:UnRegisterGetGuideUi("EquipDonationSelectBtn")
	GuideManager.Instance:UnRegisterGetGuideUi("BagEquipSelectBtn")
    UIWFlushPanel.OnDestroy(self)
end

function BagViewGrids:GuideClick(item_id)
	local index = self.bag_data:GetBagIndexById(ItemColumnType.Item, item_id)
	if index == nil then
		return
	end
	local count = self.bag_data:GetCountInBag()
	local value = (index - 5) / count
	value = value < 0 and 0 or value
	self.ItemLists:JumpToDown(1 - value)
	if self.ItemLists.index_item and self.ItemLists.index_item[index] then
		self.donation_guide = nil
		return self.ItemLists.index_item[index - 1],function()
			self.ItemLists.index_item[index - 1]:OnClick()
		end
	end
end

function BagViewGrids:flushGrid(bag_type)
	-- LogError("flushGrid")
	local view_data = self.bag_data:ViewData()
	local cur_col = view_data.selected_col
	if cur_col == ItemColumnType.None then
		return
	end
	if bag_type ~= nil and bag_type ~= cur_col then
		return
	end


	local grids = self.bag_data:GetColumnGrids(cur_col, false,false) or {}
	-- if cur_col == ItemColumnType.Item then self:CheckDataBetter(grids) end
	-- if cur_col == ItemColumnType.Item then grids = self.bag_data:CheckBetterEqiup(grids) end 

	self.ItemLists:SetData(grids)

	local unlock_cfg = self.bag_data:GetUnLockNumConfig(cur_col)

	self.Locks:SetActive(unlock_cfg.add_num > 0 and self.bag_data:IsNotMaxUnLock(cur_col))
end

function BagViewGrids:CheckDataBetter(grids)
	-- LogError("CheckDataBetter")
	local flag = true
	-- for k,v in pairs(grids) do 
	-- 	if v.is_better then flag = false end 
	-- end 

	if self.bag_data.better_equip.is_better and flag then 
	    self.bag_data:ItemDataCheck()
	end 
end

function BagViewGrids:BagGrid()
	self:flushGrid()
end

function BagViewGrids:ItemGrids(keys)
	self:flushGrid(ItemColumnType.Item)
end

function BagViewGrids:SupplyGrids()
	self:flushGrid(ItemColumnType.Supply)
end

function BagViewGrids:TaskGrids()
	self:flushGrid(ItemColumnType.Task)
end

function BagViewGrids:MaterialGrids()
	self:flushGrid(ItemColumnType.Material)
end

function BagViewGrids:IllustrationGrids()
	self:flushGrid(ItemColumnType.Illustration)
end

function BagViewGrids:ItemCellSawCheck()
	local view_data = self.bag_data:ViewData()
	local cur_new_items = BagData.Instance:GetNewItemsByColType(view_data.selected_col)
	if #cur_new_items == 0 then return end 
	for k,v in pairs(cur_new_items) do 
		local item_ = self.ItemLists:GetItemMonoByIndex(v.index)
		if item_ ~= nil and item_:IsActiveAndEnabled() then 
			self.bag_data:ClearOnesNewItem(v)
		end 
	end 
end
------------------------------
UIImgAuditRawChange = UIImgAuditRawChange or DeclareMono("UIImgAuditRawChange", UIWFlushPanel)
function UIImgAuditRawChange:UIImgAuditRawChange()
end

function UIImgAuditRawChange:Awake()
	UIWFlushPanel.Awake(self)
	if IS_AUDIT_VERSION then
		local img_path, stream_has = ChannelAgentConsts.GetUIStremingAsset(self.ImgName)
		if stream_has then
			ChannelAgentConsts.SetRawImg(self.RawImg, img_path)
			self.RawImgObj:SetActive(true)
			self.NorImgObj:SetActive(false)
		end
	end
end