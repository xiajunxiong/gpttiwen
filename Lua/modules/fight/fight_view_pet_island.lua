FightPetIslandPanel = FightPetIslandPanel or DeclareMono("FightPetIslandPanel", UIWFlushPanel)
function FightPetIslandPanel:FightPetIslandPanel() 
	self.data = FightData.Instance
	self.data_cares = {
		{data = self.data.pet_monopoly_smart, func = self.FlushMap, keys = {"cur_level"}},
		{data = self.data.pet_monopoly_smart, func = self.ShowDice, keys = {"dice"}, init_call = false},
		{data = self.data.pet_monopoly_smart, func = self.ShowChangeLevel, keys = {"show_change_level"}, init_call = false},
		{data = self.data.pet_monopoly_smart, func = self.FlushPray, keys = {"pray_info"}},
		{data = self.data.pet_monopoly_smart, func = self.OnGiftItemViewClose, keys = {"roll_gift_close"}, init_call = false},
		{data = self.data.pet_monopoly_gird_info, func = self.FlushItem, init_call = false},
		{data = self.data.fight_crystal_sweep, func = self.FlushSweep, keys = {"role_enter_energy"}, init_call = false},
		{data = self.data.pet_monopoly_smart, func = self.FlushDoubleBuffInfo, keys = {"double_left_times","special_item_num"}},
		{data = self.data.pet_monopoly_smart, func = self.ShowSpecialItemUseEffect, keys = {"show_special_effect"}, init_call = false},
		{data = self.data.pet_monopoly_trigger_smart, func = self.ShowSpecialItemGetEffct, keys = {"show_get_special_item"}, init_call = false},
	}
	FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_REWARD_RECORD)

	self.role_obj = UIChDrawer.CopyMainRoleData()
	local show_t = {
		ui_obj = self.role_obj,
		view = self
	}
	self.RoleModel:SetShowData(show_t)
	self.dice_obj = UIObjDrawer.New()
	self.DiceModel:SetShowData({
		ui_obj = self.dice_obj,
		view = self,
		not_show_anim = true
		})
	self.anim_opend = false
	self.event_handle = self.Animator:ListenEvent("OpenState",function(key,anim_st_info)
			if key == "OpenEnd" then
				self.anim_opend = true
			elseif key == "HideEnd" then
				self.anim_opend = false
				if self.data.pet_monopoly_smart.show_change_level then
					self.data.pet_monopoly_smart.show_change_level = false
					self:FlushMap()
				end
			end
		end)
	self.role_model_event_handle = self.RoleModelFadeAnim:ListenEvent("RoleModelState",function (key)
		if key == "HideEnd" then
			self.data.pre_grid_index = nil
			self:sendPetGridAddExp()
			self:SetRoleModelPos()
			self.RoleModelFadeAnim:SetTrigger(APH("Show"))
		end
	end)
	if FinishData.Instance:GetBattleMode() == BATTLE_MODE.BATTLE_MODE_PET_MONOPOLY then
        FinishData.Instance:SetBattleMode()
    end

    GuideManager.Instance:RegisterGetGuideUi("PetIslandDice", function ()
    	if self.anim_opend then
	        return self.DiceBtn
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("PetIslandSeal", function ()
        return self.SealBtn
    end)
end

function FightPetIslandPanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
	-- self.EffectTool:Play(2165009)
	if not self.played then
		self.Animator:SetTrigger(APH("Open"))
		self.played = true
		-- AudioMgr:PlayEffect(AudioType.UI,"yunwuzhuanchang")
	else
		self.Animator:SetInteger("InitState",1)
	end
end

function FightPetIslandPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.delay_move ~= nil then
		TimeHelper:CancelTimer(self.delay_move)
	end
	if self.delay_flush ~= nil then
		TimeHelper:CancelTimer(self.delay_flush)
	end
	if self.delay_func_timer ~= nil then
		TimeHelper:CancelTimer(self.delay_func_timer)
	end
	if self.role_obj ~= nil then
        self.role_obj:DeleteMe()
    end
    self.role_obj = nil
    if self.dice_obj ~= nil then
    	self.dice_obj:DeleteMe()
    	self.dice_obj = nil
    end
    if self.single_runner ~= nil then
    	TimeHelper:CancelTimer(self.single_runner)
    	self.single_runner = nil
    end
	if self.special_get_eff_fly_runner ~= nil then
		TimeHelper:CancelTimer(self.special_get_eff_fly_runner)
		self.special_get_eff_fly_runner = nil
	end
    self.energy_cache = nil
    self.Animator:UnlistenEvent("OpenEnd",self.event_handle)
	self.RoleModelFadeAnim:UnlistenEvent("RoleModelState",self.role_model_event_handle)
    self.played = false
	self.data.pet_monopoly_smart.select_special_item_id = -1
	self.data.pet_monopoly_smart.show_change_level = false
end

function FightPetIslandPanel:MoveToNextGrid()
	self:calcGetStepList()
	self.step_grid_list[1]:ResetPetModelPos()
	self.step_grid_list[1]:StopStandEffect()
	table.remove(self.step_grid_list,1)
	self.role_obj:PlayAnim(ChAnimType.Run)
	UH.LocalEulerAngles(self.role_obj:GetRoot().transform, Vector3.New(0,self.step_grid_list[1].RoleRotate,0))
	if #self.step_grid_list > 1 then
		self:moveTo(self.RoleModelRect.anchoredPosition, self.step_grid_list[1].Rect.anchoredPosition)
	elseif #self.step_grid_list == 1 then
		self:lastStep()
	else
		LogError("step_grid_list count < 1")
	end
end

function FightPetIslandPanel:lastStep()
	local item_data = self.step_grid_list[1]:GetData()
	local role_rect_x = self.step_grid_list[1].Rect.anchoredPosition.x
	if item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_NORMAL_GRID
		or item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_MEDIUM_GRID
		or item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_ADVANCE_GRID then --不是起点或终点

		if item_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER then
			role_rect_x = role_rect_x + 50
			self.step_grid_list[1]:MovePetModel()
		end
	elseif item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_ENDING_POINT then --如果是终点
		role_rect_x = role_rect_x + 50
	end
	UH.LocalEulerAngles(self.role_obj:GetRoot().transform, Vector3.New(0,self.step_grid_list[1].RoleRotate,0))
	self:moveTo(self.RoleModelRect.anchoredPosition, Vector2.New(role_rect_x, self.step_grid_list[1].Rect.anchoredPosition.y))
end

function FightPetIslandPanel:moveTo(start_pos, end_pos)
	self.step_count = 0
	if self.single_runner ~= nil then
		TimeHelper:CancelTimer(self.single_runner)
		self.single_runner = nil
	end
	self.single_runner = TimeHelper:AddRunFrameTimer(function ()
		local step_x = self.RoleModelRect.anchoredPosition.x + (end_pos.x - start_pos.x) / 20
		local step_y = self.RoleModelRect.anchoredPosition.y + (end_pos.y + 50 - start_pos.y) / 20
		UH.AnchoredPosition(self.RoleModelRect, Vector2.New(step_x, step_y))
		self.step_count = self.step_count + 1
		if self.step_count >= 20 then
			local grid_index = self.step_grid_list[1]:GetData().grid_index
			table.remove(self.step_grid_list,1)
			if #self.step_grid_list == 1 then
				if self.data.pet_monopoly_smart.dice == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_FASTMOVEPAPER * -1 then
					FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_FAST_MOVE_REWARD, grid_index)
				end
				self:lastStep()
			elseif #self.step_grid_list > 1 then
				if self.data.pet_monopoly_smart.dice == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_FASTMOVEPAPER * -1 then
					FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_FAST_MOVE_REWARD, grid_index)
				end
				UH.LocalEulerAngles(self.role_obj:GetRoot().transform, Vector3.New(0,self.step_grid_list[1].RoleRotate,0))
				self:moveTo(self.RoleModelRect.anchoredPosition, self.step_grid_list[1].Rect.anchoredPosition)
			else
				self:sendPetGridAddExp()
				self:SetRoleModelPos()
			end
		end
	end,1 , 20)
end

function FightPetIslandPanel:sendPetGridAddExp()
	local grid_item = self.GridItemList[self.data:GetPetMoCurIndex()]
	local item_data = grid_item:GetData()
	if item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_NORMAL_GRID
		or item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_MEDIUM_GRID
		or item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_ADVANCE_GRID then

		-- if item_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER then
			FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_ADD_EXP)
		-- end
	elseif item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_ENDING_POINT then
		FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_ADD_EXP)
	end
end

function FightPetIslandPanel:calcGetStepList()
	self.step_grid_list = {}
	--正常掷骰子/鲁班骰/神行符
	if self.data.pet_monopoly_smart.dice > 0 or self.data.pet_monopoly_smart.dice == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_FASTMOVEPAPER * -1 then
		local dice = self.data.pet_monopoly_smart.dice < 0 and self.data:GetPetMoCurIndex() - self.data.pre_grid_index or self.data.pet_monopoly_smart.dice
		for i = self.data:GetPetMoCurIndex() - dice, self.data:GetPetMoCurIndex() do
			table.insert(self.step_grid_list, self.GridItemList[i])
		end
		self.data.pre_grid_index = nil
	end
end

function FightPetIslandPanel:FlushPray()
	self.SilverPet:SetData(self.data:GetPetPreyId(2))
	self.GoldPet:SetData(self.data:GetPetPreyId(3))
end

function FightPetIslandPanel:FlushItem(data)
	if self.data.pet_monopoly_smart.show_change_level then
		return
	end
	local index = self.data:GetPetMoCurIndex()
	if data and next(data) then
		index = next(data)
	end
	self.GridItemList[index]:SetData(self.data:GetPetMonopolyGridInfo(index))
	if index == self.data:GetPetMoCurIndex() then
		if self.data:GetPetMoCurIndex() == PET_MONOPOLY_GRID_NUM then
			if self.data:GetPetMoCurIndexData().reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_NONE
				or self.data:GetPetMoCurIndexData().reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER then
				self.StartRollObj:SetActive(false)
				self.TouziBg:SetActive(self.data:GetPetMoCurIndexData().reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER)
				self.FengyinGObj:SetActive(self.data:GetPetMoCurIndexData().reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER)
			else
				self.StartRollObj:SetActive(true)
			end
		end
		UH.SetText(self.ExpTip,string.format(Language.Fight.PetIslandExpTip, self.data:GetPetMoExp()))
	end
end

function FightPetIslandPanel:CheckReward()
	if self.cur_grid_item == nil then
		return
	end
	local grid_data = self.cur_grid_item:GetData()
	if grid_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_ITEM
		or grid_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT then
		-- self.cur_grid_item:PlayGetItemEffect()
		FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_FETCH_GRID_REWARD)
	elseif grid_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_NONE then
		return true
	end
end

function FightPetIslandPanel:ShowChangeLevel()
	if self.data.pet_monopoly_smart.show_change_level then
		if self.cur_grid_item then
			self.cur_grid_item:StopStandEffect()
		end
		AudioMgr:PlayEffect(AudioType.UI,"yunwuzhuanchang")
		self.EffectTool:Play(2165021)
		self.Animator:SetTrigger(APH("Hide"))
	end
end

function FightPetIslandPanel:FlushMap()
	if self.data.pet_monopoly_smart.show_change_level then
		return
	end
	local grid_info_list = self.data:GetPetMonopolyGridInfo()
	for i=1,self.GridItemList:Length() do
		self.GridItemList[i]:SetData(grid_info_list[i])
	end
	self.StartRollObj:SetActive(true)
	self:SetRoleModelPos()
	UH.SetText(self.CurLevelText, string.format(Language.Fight.PetIslandCurLevel, DataHelper.GetDaXie(self.data.pet_monopoly_smart.cur_level)))
	UH.SetText(self.ExpTip,string.format(Language.Fight.PetIslandExpTip, self.data:GetPetMoExp()))
	local show_rate = self.data:GetGoldPetShowRate()
	UH.SetText(self.PetRateValue,string.format(Language.Common.Percent, show_rate))
end

function FightPetIslandPanel:ShowSpecialItemUseEffect()
	if self.data.pet_monopoly_smart.show_special_effect == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_WISHCARD then
		for _, v in pairs(self.data.changed_grid_info or {}) do
			local item = self.GridItemList[v.grid_index + 1]
			item:PlaySpecialEffect(PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_WISHCARD)
		end
		-- self.delay_flush_wish_card_map = TimeHelper:AddDelayTimer(function ()
			self:FlushMap()
		-- end,1)--TempTime 根据心愿卡特效时长
	end
end

function FightPetIslandPanel:ShowSpecialItemGetEffct()
	if self.special_get_eff_fly_runner ~= nil then
		TimeHelper:CancelTimer(self.special_get_eff_fly_runner)
	end
	local grid_item = self.GridItemList[self.data.pet_monopoly_smart.cur_grid_index]
	if grid_item ~= nil then
		self.SpecialGetEffPar:SetParent(grid_item.unityMono.gameObject.transform)
		UH.AnchoredPosition(self.SpecialGetEffPar, Vector2.New(0,0))
		self.SpecialGetEffPar:SetParent(self.SpecialBoxObj.transform)
		self.special_get_eff_handle = self.EffectTool:Play(3161023)
		local cur_v2 = self.SpecialGetEffPar.anchoredPosition
		self.fly_setp = 0
		self:specialGetEffFly(cur_v2.x / 10, cur_v2.y / 10)
	end
end

function FightPetIslandPanel:specialGetEffFly(step_x, step_y)
	self.special_get_eff_fly_runner = TimeHelper:AddRunFrameTimer(function ()
		UH.AnchoredPosition(self.SpecialGetEffPar, Vector2.New(self.SpecialGetEffPar.anchoredPosition.x - step_x, self.SpecialGetEffPar.anchoredPosition.y - step_y))
		self.fly_setp = self.fly_setp + 1
		if self.fly_setp >= 10 then
			self.EffectTool:Stop(self.special_get_eff_handle)
			self.SpecialBoxIconAnim:Play()
		end
	end, 1, 10)
end

function FightPetIslandPanel:SetRoleModelPos()
	self.role_obj:PlayAnim(ChAnimType.Idle)
	if self.role_obj:GetRoot() then
		UH.LocalEulerAngles(self.role_obj:GetRoot().transform, Vector3.zero)
	end
	local grid_item = self.GridItemList[self.data:GetPetMoCurIndex()]
	if grid_item == nil then
		return
	end
	local role_rect_x = grid_item.Rect.anchoredPosition.x
	self.cur_grid_item = grid_item
	local item_data = grid_item:GetData()
	local delay_func = nil
	--不是起点或终点
	if item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_NORMAL_GRID
		or item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_MEDIUM_GRID
		or item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_ADVANCE_GRID then

		--奖励类型是物品或者宠物碎片
		if item_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_ITEM
			or item_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT
			or item_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_OPPORTUNITY then
			delay_func = function ()
				self:OnEffectEnd()
				-- grid_item:PlayGetItemEffect()
			end
			self.FengyinGObj:SetActive(false)
		--奖励类型是宠物（需要进战斗封印）
		elseif item_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER then
			grid_item:SetPetModelPos()
			self.TouZiGObj:SetActive(false)
			self.TouziBg:SetActive(true)
			self.FengyinGObj:SetActive(true)
			self.SpecialBoxObj:SetActive(false)
			role_rect_x = role_rect_x + 50
		end
	--如果是终点
	elseif item_data.grid_type == FIGHT_PET_MONOPOLY_GRID_TYPE.PET_MONOPOLY_GRID_TYPE_ENDING_POINT then
		--摇到宠物需要封印战斗
		if item_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER then
			self.FengyinGObj:SetActive(item_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER)
			self.StartRollObj:SetActive(item_data.reward_type ~= FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER)
			self.SpecialBoxObj:SetActive(item_data.reward_type ~= FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER)
			role_rect_x = role_rect_x + 50
		--未领取奖励
		elseif item_data.reward_type ~= FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_NONE then
			delay_func = function ()
				self.data.pet_monopoly_smart.sector_idx = nil
				ViewMgr:OpenView(FightPetIslandRollView)
			end
			role_rect_x = role_rect_x + 50
		else
			-- self.StartRollObj:SetActive(false)
			-- self.Animator:SetTrigger(APH("Hide"))
			-- self.EffectTool:Play(2165021)
			delay_func = function ()
				TimeHelper:AddDelayTimer(function ()
					FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_ENTER_NEXT_LEVEL)
				end, 2)
			end
		end
		self.TouziBg:SetActive(false)
		self.TouZiGObj:SetActive(false)
	else
		self.TouziBg:SetActive(true)
		self.TouZiGObj:SetActive(true)
		self.SpecialBoxObj:SetActive(true)
	end

	UH.AnchoredPosition(self.RoleModelRect, Vector2.New(role_rect_x, grid_item.Rect.anchoredPosition.y + 50))
	if self.anim_opend then
		if self.data:GetPetMoCurIndex() ~= PET_MONOPOLY_GRID_NUM then
			grid_item:PlayStandEffect()
		end
		if delay_func then
			delay_func()
		end
	else
		if self.delay_func_timer ~= nil then
			TimeHelper:CancelTimer(self.delay_func_timer)
		end
		self.delay_func_timer = TimeHelper:AddDelayTimer(function ()
			if self.data:GetPetMoCurIndex() ~= PET_MONOPOLY_GRID_NUM then
				grid_item:PlayStandEffect()
			end
			if delay_func then
				delay_func()
			end
		end,3.5)
	end
	self:FlushSweep()
	self.MaskObj:SetActive(false)
end

function FightPetIslandPanel:FlushSweep()
	local cur_sweep = self.data:GetFightStrengthNum()
	UH.SetText(self.SweepCost, string.format(Language.Common.Xstr, cur_sweep >= self.data.pet_mo_sweep_cose and COLORSTR.Yellow13 or COLORSTR.Red10 , self.data.pet_mo_sweep_cose))
end

function FightPetIslandPanel:FlushDoubleBuffInfo()
	self.DouBleBuffObj:SetActive(self.data.pet_monopoly_smart.double_left_times > 0)
	UH.SetText(self.DoubleBuffLeftTimes, string.format(Language.Fight.PetIslandLeftDoubleTimes, self.data.pet_monopoly_smart.double_left_times))
	if self.data.pet_monopoly_smart.special_item_num > 0 then
		UH.SetText(self.SpecialBoxRedNum, self.data.pet_monopoly_smart.special_item_num)
		self.SpecialBoxRedObj:SetActive(true)
	else
		self.SpecialBoxRedObj:SetActive(false)
	end
end

function FightPetIslandPanel:ShowDice()
	--LogError("ShowDice",self.data.pet_monopoly_smart.dice)
	--正常骰子/鲁班骰
	if self.data.pet_monopoly_smart.dice <= 6 and self.data.pet_monopoly_smart.dice >= 1 then
		self.MaskObj:SetActive(true)
		self.TouZiGObj:SetActive(false)
		self.TouziBg:SetActive(false)
		self.FengyinGObj:SetActive(false)
		self.DiceModelObj:SetActive(true)
		self.SpecialBoxObj:SetActive(false)
		self.dice_obj:SetData(DrawerHelper.GetNpcPath(FIGHT_PET_ISLAND_DICE_MODEL_ID[self.data.pet_monopoly_smart.dice]))
		AudioMgr:PlayEffect(AudioType.UI,"touziyinxiao")
		if self.delay_move ~= nil then
			TimeHelper:CancelTimer(self.delay_move)
		end
		self.delay_move = TimeHelper:AddDelayTimer(function ()
			self.DiceModelObj:SetActive(false)
			self:MoveToNextGrid()
		end, 2)--TempTime 根据最终的骰子动画时间
	--使用了神行符
	elseif self.data.pet_monopoly_smart.dice == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_FASTMOVEPAPER * -1 then
		self:MoveToNextGrid()
	--使用了传送门之后
	elseif self.data.pet_monopoly_smart.dice == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_PORTAL * -1 then
		self.GridItemList[self.data.pre_grid_index]:PlaySpecialEffect(PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_PORTAL)
		self.GridItemList[self.data.pre_grid_index]:StopStandEffect()
		self.GridItemList[self.data:GetPetMoCurIndex()]:PlaySpecialEffect(PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_PORTAL)
		self.RoleModelFadeAnim:SetTrigger(APH("Hide"))
		self.data.pre_grid_index = nil
	end
end

function FightPetIslandPanel:OnGiftItemViewClose()
	FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_ENTER_NEXT_LEVEL)
end

function FightPetIslandPanel:OnClickDice()
	local cur_sweep = self.data:GetFightStrengthNum()
	if cur_sweep < self.data.pet_mo_sweep_cose then
		ViewMgr:OpenView(FightCrystalSweepItemView)
		return
	end
	if self:CheckReward() then
		FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_ROLL_DICE)
	end
end

function FightPetIslandPanel:OnClickReturn()
	ViewMgr:CloseView(FightFBView)
end

function FightPetIslandPanel:OnClickTip()
	-- UH.LocalEulerAngles(self.role_obj:GetRoot().transform, Vector3.New(0,45,0))
	-- self.role_obj:PlayAnim(ChAnimType.Run)
	-- LogError("role_obj name",self.role_obj:GetRoot().name)
	local t = {tips_str = Config.language_cfg_auto.textdesc[120].desc}
	PublicPopupCtrl.Instance:OpenTipsPopup(t)
end

function FightPetIslandPanel:OnClickFengYin()
	if PetData.Instance:PetBagIsFull() then
		local t = {
			content = Language.Fight.FightPetSealBagFull,
			confirm = {
				func = function ()
					self.data.pet_monopoly_smart.goto_fengyin = true
					FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_START_PET_BATTLE)
					PublicPopupCtrl.Instance:CloseDialogTips()
				end
			}
		}
		PublicPopupCtrl.Instance:DialogTips(t)
		return
	end

	local grid_item = self.GridItemList[self.data:GetPetMoCurIndex()]
	local item_data = grid_item:GetData()
	local pet_cfg = PetData.Instance:GetPetCfg(item_data.param1)
	if pet_cfg and pet_cfg.quality == 3 then
		ViewMgr:OpenView(FightPetIslandSealTip)
	else
		self.data.pet_monopoly_smart.goto_fengyin = true
		FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_START_PET_BATTLE)
	end
end

function FightPetIslandPanel:OnClickRewardRecord()
	FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_REWARD_RECORD)
	ViewMgr:OpenView(FightPetRewardView)
end

function FightPetIslandPanel:OnClickSpecialBox()
	self.SpecialAnim:SetTrigger(APH("Hide"))
	ViewMgr:OpenView(FightPetIslandSpecialItemView)
end

function FightPetIslandPanel:OnClickRoll()
	if self.data:GetPetMoCurIndex() == PET_MONOPOLY_GRID_NUM then
		self.data.pet_monopoly_smart.sector_idx = nil
		ViewMgr:OpenView(FightPetIslandRollView)
	else
		PublicPopupCtrl.Instance:Center(Language.Fight.PetIslandRollTip)
	end
end

function FightPetIslandPanel:OnClickAdd()
	ViewMgr:OpenView(FightCrystalSweepItemView)
end

function FightPetIslandPanel:OnEffectEnd()
	self.TouziBg:SetActive(true)
	self.TouZiGObj:SetActive(true)
	self.SpecialBoxObj:SetActive(true)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
	FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_FETCH_GRID_REWARD)
end

function FightPetIslandPanel:OnCloudEffectEnd()
	if self.data.pet_monopoly_smart.show_change_level then
		-- self.EffectTool:Play(2165009)
	end
end
--------------------------------------------------------
FightPetIslandSpecialItemUsePanel = FightPetIslandSpecialItemUsePanel or DeclareMono("FightPetIslandSpecialItemUsePanel", UIWFlushPanel)
function FightPetIslandSpecialItemUsePanel:FightPetIslandSpecialItemUsePanel()
	self.data = FightData.Instance
	self.data_cares = {
		{data = self.data.pet_monopoly_smart, func = self.ShowAnim, keys = {"use_special_item"}, init_call = false},
	}
end

function FightPetIslandSpecialItemUsePanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushSpecialGroup()
end

function FightPetIslandSpecialItemUsePanel:ShowAnim()
	self:FlushSpecialGroup()
	self.SpecialAnim:SetTrigger(APH("Open"))
end

function FightPetIslandSpecialItemUsePanel:FlushSpecialGroup()
	local reward_type_not_fengyin = self.data:GetPetMoCurIndexData().reward_type ~= FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER
	if Language.Fight.PetIslandSpecialNotice[self.data.pet_monopoly_smart.select_special_item_id] ~= nil then
		PublicPopupCtrl.Instance:Center(Language.Fight.PetIslandSpecialNotice[self.data.pet_monopoly_smart.select_special_item_id])
	end
	UH.SpriteName(self.SpecialNameImg, reward_type_not_fengyin and PET_MONOPOLY_SPECIAL_IMG_NAME[self.data.pet_monopoly_smart.select_special_item_id] or "")
	if reward_type_not_fengyin then
		if self.data.pet_monopoly_smart.select_special_item_id == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_DOUBLECARD
			or self.data.pet_monopoly_smart.select_special_item_id == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_WISHCARD 
			or self.data.pet_monopoly_smart.select_special_item_id < 0 then
			self.TouZiGObj:SetActive(true)
		else
			self.TouZiGObj:SetActive(false)
		end
	else
		self.TouZiGObj:SetActive(false)
	end
	self.SpecialBoxObj:SetActive(reward_type_not_fengyin)
	-- self.TouZiGObj:SetActive(reward_type_not_fengyin and self.data.pet_monopoly_smart.select_special_item_id < 0)
	self.LuBanTouObj:SetActive(reward_type_not_fengyin and self.data.pet_monopoly_smart.select_special_item_id == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_LUBANDICE)
	self.ShenXingFuObj:SetActive(reward_type_not_fengyin and self.data.pet_monopoly_smart.select_special_item_id == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_FASTMOVEPAPER)
	self.ChuanSongMen:SetActive(reward_type_not_fengyin and self.data.pet_monopoly_smart.select_special_item_id == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_PORTAL)
	self.FengYinGObj:SetActive(not reward_type_not_fengyin)
	self.XiaoHaoDiObj:SetActive(reward_type_not_fengyin and PET_MONOPOLY_SPECIAL_IMG_NAME[self.data.pet_monopoly_smart.select_special_item_id] ~= nil)
	if reward_type_not_fengyin and  self.data.pet_monopoly_smart.select_special_item_id == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_LUBANDICE then
		self.data.pet_monopoly_smart.special_param = 3
		UH.SetText(self.LuBanTouText, self.data.pet_monopoly_smart.special_param)
	end
end

--播放骰子动画时（即无论时正常消耗体力的还是鲁班骰协议返回时）或播放角色移动动画时都隐藏
function FightPetIslandSpecialItemUsePanel:HideAll()
	self.LuBanTouObj:SetActive(false)
	self.ShenXingFuObj:SetActive(false)
	self.ChuanSongMen:SetActive(false)
	self.XiaoHaoDiObj:SetActive(false)
end

function FightPetIslandSpecialItemUsePanel:OnClickLuBanAdd()
	if self.data.pet_monopoly_smart.special_param < 6 then
		self.data.pet_monopoly_smart.special_param = self.data.pet_monopoly_smart.special_param + 1
		UH.SetText(self.LuBanTouText, self.data.pet_monopoly_smart.special_param)
	end
end

function FightPetIslandSpecialItemUsePanel:OnClickLuBanReduce()
	if self.data.pet_monopoly_smart.special_param > 1 then
		self.data.pet_monopoly_smart.special_param = self.data.pet_monopoly_smart.special_param - 1
		UH.SetText(self.LuBanTouText, self.data.pet_monopoly_smart.special_param)
	end
end

function FightPetIslandSpecialItemUsePanel:OnClickUseSpecialItem()
	self:HideAll()
	FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_SPECIAL_ITEM, self.data.pet_monopoly_smart.select_special_item_id, self.data.pet_monopoly_smart.special_param)
end
--------------------------------------------------------
FightPetIslandTiliPanel = FightPetIslandTiliPanel or DeclareMono("FightPetIslandTiliPanel", UIWFlushPanel)
function FightPetIslandTiliPanel:FightPetIslandTiliPanel()
	self.data = FightData.Instance
end

function FightPetIslandTiliPanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
	local info = self.data.fight_crystal_sweep
	local time_desc = Language.Fight.Sweep.time 
	TimeHelper:CancelTimer(self.timer_ct_next)
	TimeHelper:CancelTimer(self.timer_ct_all)
	if 0 == info.next_energy_reply_tamp then
		self.SweepNextTime.text = string.format(time_desc, 0, 0, 0)
		self.SweepAllTime.text = string.format(time_desc, 0, 0, 0)
		return
	end
	self.timer_ct_next = TimeHelper:AddCountDownCT(function ()
		local next_time = info.next_energy_reply_tamp - TimeHelper.GetServerTime()
		local ft = TimeHelper.FormatDHMS(next_time)
		self.SweepNextTime.text = string.format(time_desc, ft.hour, ft.min, ft.s)
	end, function ()
		self.SweepNextTime.text = string.format(time_desc, 0, 0, 0)
	end, info.next_energy_reply_tamp)
	self.timer_ct_all = TimeHelper:AddCountDownCT(function ()
		local all_time = info.total_energy_reply_tamp - TimeHelper.GetServerTime()
		local ft = TimeHelper.FormatDHMS(all_time)
		self.SweepAllTime.text = string.format(time_desc, ft.hour, ft.min, ft.s)
	end, function ()
		self.SweepAllTime.text = string.format(time_desc, 0, 0, 0)
	end, info.total_energy_reply_tamp)
end

function FightPetIslandTiliPanel:OnDisable()
	UIWFlushPanel.OnDisable(self)
	TimeHelper:CancelTimer(self.timer_ct_next)
	TimeHelper:CancelTimer(self.timer_ct_all)
end

-------------------------------
FightPetIslandGridItem = FightPetIslandGridItem or DeclareMono("FightPetIslandGridItem", UIWidgetBaseItem)
function FightPetIslandGridItem:FightPetIslandGridItem()
	self.data_instance = FightData.Instance
end

function FightPetIslandGridItem:SetData(data)
	if data == nil then
		return
	end
	if data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_ITEM
		or data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT then
		self.ItemCell:SetData(Item.Init(data.param1, data.param2))
		self.ItemCell:SetObjActive(true)
		self:PlayRareItemEffect(FightData.Instance:IsRareItemInPetMo(data.param1))
		self:PlayPetEffect(false)
	elseif data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER then
		self:PlayPetEffect(true)
		self.PetEffPar:SetActive(true)
		self.ItemCell:SetObjActive(false)
		self:PlayRareItemEffect(false)
	elseif data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_OPPORTUNITY then
		self:PlayPetEffect(false)
		self.ItemCell:SetObjActive(false)
		self:PlayRareItemEffect(false)
		self:PlaySpecialEffect("NORMAL")
	else
		self:PlayPetEffect(false)
		self.ItemCell:SetObjActive(false)
		self:PlayRareItemEffect(false)
		self:PlaySpecialEffect(false)
	end
	UH.SpriteName(self.BgSprite, FIGHT_PET_MONOPOLY_GRID_BG_NAME[data.grid_type])
	self.data = data
end

function FightPetIslandGridItem:OnEnable()
	self.enabled = true
	if self.show_eff then
		self:PlayStandEffect()
		self.show_eff = false
	end
	if self.show_pet_eff then
		self.delay_play_pet_eff = TimeHelper:AddDelayTimer(function ( )
			self:PlayPetEffect(true)
			self.show_pet_eff = false
		end,0.5)
	end
	if self.show_rare_eff then
		self:PlayRareItemEffect(true)
		self.show_rare_eff = false
	end
	if self.show_special_effect then
		self:PlaySpecialEffect(self.show_special_effect)
	end
end

function FightPetIslandGridItem:OnDisable()
	self.enabled = false
	if self.delay_play_pet_eff then
		TimeHelper:CancelTimer(self.delay_play_pet_eff)
		self.delay_play_pet_eff = nil
	end
end

function FightPetIslandGridItem:PlayPetEffect(show)
	--LogError("PlayPetEffect",self.enabled, show)
	if self.enabled and show then
		if self.pet_effect_handle == nil then
			self.pet_effect_handle = self.EffectTool:Play(2165034, self.PetEffPar)
		end
		self.PetEffPar:SetActive(true)
	else
		self.show_pet_eff = show
		if self.pet_effect_handle ~= nil then
			self.PetEffPar:SetActive(false)
		end
	end
end

function FightPetIslandGridItem:OnDestroy()
	if self.stand_handle ~= nil then
		self.EffectTool:Stop(self.stand_handle)
		self.stand_handle = nil
	end
	if self.pet_effect_handle ~= nil then
		self.EffectTool:Stop(self.pet_effect_handle)
		self.pet_effect_handle = nil
	end
	if self.rare_effect_handle ~= nil then
		self.EffectTool:Stop(self.rare_effect_handle)
		self.rare_effect_handle = nil
	end
	if self.special_effect_handle ~= nil then
		self.EffectTool:Stop(self.special_effect_handle)
		self.special_effect_handle = nil
	end
	if self.delay_play_pet_eff then
		TimeHelper:CancelTimer(self.delay_play_pet_eff)
	end
end

function FightPetIslandGridItem:PlayStandEffect()
	if self.enabled and self.stand_handle == nil then
		self.stand_handle = self.EffectTool:Play(2165008, self.BgSprite.gameObject)
	else
		self.show_eff = true
	end
end

function FightPetIslandGridItem:StopStandEffect()
	if self.stand_handle ~= nil then
		self.EffectTool:Stop(self.stand_handle)
		self.stand_handle = nil
	end
end

function FightPetIslandGridItem:PlayRareItemEffect(show)
	if self.enabled then
		if show then
			if self.rare_effect_handle == nil then
				self.rare_effect_handle = self.EffectTool:Play(2165036, self.RareEffPar)
			end
		elseif self.rare_effect_handle ~= nil then
			self.EffectTool:Stop(self.rare_effect_handle)
			self.rare_effect_handle = nil
		end
	else
		self.show_rare_eff = show
	end
end

function FightPetIslandGridItem:PlayGetItemEffect()
	self.EffectTool:Play(2165010, self.BgSprite.gameObject)
end

function FightPetIslandGridItem:PlaySpecialEffect(key)
	if self.enabled then
		if self.special_effect_handle ~= nil then
			self.EffectTool:Stop(self.special_effect_handle)
		end
		if not key then
			return
		end
		if key == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_WISHCARD then
			self.special_effect_handle = self.EffectTool:Play(4161019,self.PetEffPar)
			self.PetEffPar:SetActive(true)
		elseif key == PET_ISLAND_SPECIAL_ITEM_TYPE.SP_ITEM_PORTAL then
			-- LogError("grid_index",self.data.grid_index + 1, self.data_instance.pre_grid_index, self.data_instance:GetPetMoCurIndex())
			if self.data.grid_index + 1 == self.data_instance.pre_grid_index then
				self.special_effect_handle = self.EffectTool:Play(4161022,self.PetEffPar)
			elseif self.data.grid_index + 1 == self.data_instance:GetPetMoCurIndex() then
				self.special_effect_handle = self.EffectTool:Play(4161021,self.PetEffPar)
			end
			self.PetEffPar:SetActive(true)
		elseif key == "NORMAL" then
			--LogError("play normal effect 机会点")
			self.special_effect_handle = self.EffectTool:Play(4161017, self.PetEffPar)
			self.PetEffPar:SetActive(true)
		else
			self.PetEffPar:SetActive(false)
		end
		self.show_special_effect = nil
	else
		self.show_special_effect = key
		if self.special_effect_handle ~= nil then
			self.PetEffPar:SetActive(false)
		end
	end
end

function FightPetIslandGridItem:ResetPetModelPos()
	if self.data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER then
		self.Anim:Play("fight_pet_grid_model_reset")
	end
end

function FightPetIslandGridItem:SetPetModelPos()
	UH.AnchoredPosition(self.PetEffPar:GetComponent(typeof(UnityEngine.RectTransform)), Vector2.New(-50,0))
	-- UH.AnchoredPosition(self.PetModelRect, Vector2.New(-50, 40))
end

function FightPetIslandGridItem:MovePetModel()
	self.Anim:Play("fight_pet_grid_model_move")
end

function FightPetIslandGridItem:OnDestroy()
	if self.pet_obj ~= nil then
		self.pet_obj:DeleteMe()
		self.pet_obj = nil
	end
end

function FightPetIslandGridItem:OnClickPetModel()
	ViewMgr:OpenView(PetDetailView, Pet.New(PetData.Instance:GetPetCfg(self.data.param1)))
end
------------------------------
FightPetIslandPrayItem = FightPetIslandPrayItem or DeclareMono("FightPetIslandPrayItem", UIWidgetBaseItem)
function FightPetIslandPrayItem:FightPetIslandPrayItem()
	self.data_instance = FightData.Instance
end

function FightPetIslandPrayItem:SetData(data)
	if data ~= nil and data ~= 0 then
        local pet_cfg = PetData.Instance:GetPetCfg(data)
        UH.SetIcon(self.Icon, pet_cfg.icon_id)
        self.AddImgObj:SetActive(false)
    else
        UH.SetIcon(self.Icon)
        self.AddImgObj:SetActive(true)
    end
	self.data = data
end

function FightPetIslandPrayItem:OnClickPet(param)
    self.data_instance.pet_monopoly_smart.cur_pray_id = self.data_instance:GetPetPreyId(param)
    ViewMgr:OpenView(FightPetPraySelect, param)
end
-----------------------------
FightPetIslandRollView = FightPetIslandRollView or DeclareView("FightPetIslandRollView","fight/fight_pet_island_roll")
VIEW_DECLARE_MASK(FightPetIslandRollView, ViewMask.BgBlock)
function FightPetIslandRollView:FightPetIslandRollView()
	self.data = FightData.Instance
end

function FightPetIslandRollView:LoadCallback()
	if self.data:GetPetMoTurntableResult() ~= 0 and self.GridItemList[self.data:GetPetMoTurntableResult()] then
		self.GridItemList[self.data:GetPetMoTurntableResult()]:SetSelected(true)
		-- self.delay_get = TimeHelper:AddDelayTimer(function ()
		-- 	FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_TURN_TABLE_END)
		-- end, 2)
	else
		self.delay_show_click_effect = TimeHelper:AddDelayTimer(function ()
			self.click_effect_handle = self.EffectTool:Play(100108271)
		end, 3)
	end
end

function FightPetIslandRollView:CloseCallback()
	if self.delay_get ~= nil then
		TimeHelper:CancelTimer(self.delay_get)
		self.delay_get = nil
	end
	if self.delay_show_click_effect ~= nil then
		TimeHelper:CancelTimer(self.delay_show_click_effect)
		self.delay_show_click_effect = nil
	end
	if self.data:GetPetMoTurntableResult() ~= 0 then
		local grid_data = self.GridItemList[self.data:GetPetMoTurntableResult()]:GetData()
		if grid_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_ITEM
			or grid_data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT then
			ViewMgr:OpenView(GiftOpenView,{
				gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS,
				item_list = {Item.Init(grid_data.param1,grid_data.param2)},
				is_not_show = true,
				close_func = function()
					FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_ENTER_NEXT_LEVEL)
				end,
			})
			FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_TURN_TABLE_END)
		else
			FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_TURN_TABLE_END)
		end
	end
end 

function FightPetIslandRollView:OnClickStartRoll()
	if self.data:GetPetMoTurntableResult() ~= 0 and self.GridItemList[self.data:GetPetMoTurntableResult()] then
		return
	end
	FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_TURN_TABLE)
	if self.delay_show_click_effect ~= nil then
		TimeHelper:CancelTimer(self.delay_show_click_effect)
		self.delay_show_click_effect = nil
	end
	if self.click_effect_handle ~= nil then
		self.EffectTool:Stop(self.click_effect_handle)
	end
end

function FightPetIslandRollView:OnClickReturn()
	if self.data:GetPetMoCurIndexData().reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_NONE then
		-- FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_ENTER_NEXT_LEVEL)
	end
	ViewMgr:CloseView(FightPetIslandRollView)
end
-----------------
FightPetIslandRollFlush = FightPetIslandRollFlush or DeclareMono("FightPetIslandRollFlush", UIWFlushPanel)
function FightPetIslandRollFlush:FightPetIslandRollFlush()
	self.data = FightData.Instance
	self.data_cares = {
		{data = self.data.pet_monopoly_smart, func = self.ShowRolling, keys = {"sector_idx"}, init_call = false},
		{data = self.data.pet_monopoly_smart, func = self.FlushReward, keys = {"flush_turn_reward"}}
	}
end

function FightPetIslandRollFlush:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.delay_get ~= nil then
		TimeHelper:CancelTimer(self.delay_get)
	end
	if self.delay_close ~= nil then
		TimeHelper:CancelTimer(self.delay_close)
	end
	if self.roll_delay ~= nil then
		TimeHelper:CancelTimer(self.roll_delay)
		self.roll_delay = nil
	end
end

function FightPetIslandRollFlush:FlushReward()
	local grid_info_list = self.data:GetPetMonopolyTurntableRewardList()
	if next(grid_info_list) == nil then
		FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_SHOW_TRUNTABLE_REWARDS)
		for i=1,#grid_info_list do
			self.GridItemList[i]:SetData()
		end
		return
	end
	for i=1,#grid_info_list do
		self.GridItemList[i]:SetData(grid_info_list[i])
	end
end

function FightPetIslandRollFlush:ShowRolling()
	self.roll_count = 0
	self.roll_index = 1
	self.delay_time = 0.05
	self.time_stemp = 0
	self.cut_index = self.data:GetPetMoTurntableResult() - 4 <= 0 and self.data:GetPetMoTurntableResult() + 4 or self.data:GetPetMoTurntableResult() - 4
	self:Rolling()
end

function FightPetIslandRollFlush:Rolling()
	self.GridItemList[self.roll_index]:SetSelected(true)
	if self.roll_delay ~= nil then
		TimeHelper:CancelTimer(self.roll_delay)
	end
	self.roll_delay = TimeHelper:AddDelayTimer(function ()
		if self.delay_time < 0.5 then
			self.roll_index = self.roll_index + 1 > 8 and 1 or self.roll_index + 1
			if self.roll_count > 18 and self.roll_index == self.cut_index and self.time_stemp == 0 then
				self.time_stemp = 0.2
			end
			self.delay_time = self.delay_time + self.time_stemp
			self.roll_count = self.roll_count + 1
			self:Rolling()
		elseif self.roll_index ~= self.data:GetPetMoTurntableResult() then
			self.roll_index = self.roll_index + 1 > 8 and 1 or self.roll_index + 1
			self:Rolling()
		else
			self.GridItemList[self.data:GetPetMoTurntableResult()]:SetSelected(true)
			ViewMgr:CloseView(FightPetIslandRollView)
		end
	end, self.delay_time)
end

---------------------------------
FightPetIslandTableItem = FightPetIslandTableItem or DeclareMono("FightPetIslandTableItem", UIWidgetBaseItem)
function FightPetIslandTableItem:FightPetIslandTableItem()
end

function FightPetIslandTableItem:SetData(data)
	if data == nil then
		self.ItemCell:SetObjActive(false)
	end
	if data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_FRAGMENT 
		or data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_ITEM then
		self.ItemCell:SetData(Item.Init(data.param1, data.param2))
		self.ItemCell:SetObjActive(true)
	elseif data.reward_type == FIGHT_PET_MONOPOLY_GRID_REWARD.PET_MONOPOLY_GRID_REWARD_PET_MONSTER then
		self.ItemCell:SetData(Pet.New(PetData.Instance:GetPetCfg(data.param1)))
		self.ItemCell:SetObjActive(true)
	end
	self.data = data
end
----------------------------------------
FightPetRewardView = FightPetRewardView or DeclareView("FightPetRewardView","fight/fight_pet_reward")
VIEW_DECLARE_MASK(FightPetRewardView, ViewMask.Block)
function FightPetRewardView:FightPetRewardView()
	self.data = FightData.Instance
end

function FightPetRewardView:LoadCallback()
	self.Board:SetData(self:GetType(), Language.Fight.PetIslandReward, Vector2.New(388,328))
	local show_list = {}
	for k,v in pairs(self.data:GetPetMonopolyRewardList()) do
		table.insert(show_list,Item.Create(v))
	end
	self.RewardList:SetData(show_list)
	self.NoRewardTip:SetActive(next(show_list) == nil)
end

----------------------------------------
FightPetIslandSealTip = FightPetIslandSealTip or DeclareView("FightPetIslandSealTip","fight/fight_pet_seal_tip")
function FightPetIslandSealTip:FightPetIslandSealTip()
	self.data = FightData.Instance
end

function FightPetIslandSealTip:LoadCallback()
	UH.SetText(self.CurNumText, string.format(Language.Fight.PetIslandSealCard, Item.GetNum(26080)))
end

function FightPetIslandSealTip:OnClickGotoBuy()
	MallCtrl.Instance:Jump(26080)
end

function FightPetIslandSealTip:OnClickSealNow()
	self.data.pet_monopoly_smart.goto_fengyin = true
	FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_START_PET_BATTLE)
end

function FightPetIslandSealTip:OnClickClose()
	ViewMgr:CloseView(FightPetIslandSealTip)
end