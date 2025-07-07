FightPetPraySelect = FightPetPraySelect or DeclareView("FightPetPraySelect","fight/fight_pet_pray_select")
VIEW_DECLARE_MASK(FightPetPraySelect, ViewMask.BgBlock)
function FightPetPraySelect:FightPetPraySelect()
	self.data = FightData.Instance
end

function FightPetPraySelect:LoadCallback(param)--2/3
	UH.SpriteName(self.QuaFrame, "pet_qua_"..(param))
	UH.SetText(self.PrayPetText, Language.Fight.PetIslandPrayType[param - 1])
	UH.SetText(self.RateText, string.format(Language.Common.Percent, tostring(self.data.GetPetMoBlessRate(param))))
	UH.SetText(self.RateTipText, Language.Fight.PetIslandPrayRateTip[param - 1])
	local pet_list = self.data:GetPetPreyPool(param)
	self.param = param
	self.SelectList:SetData(pet_list)
end

function FightPetPraySelect:OnClickConfirm()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
	if self.data.pet_monopoly_smart.cur_pray_id ~= self.data:GetPetPreyId(self.param) then
		if self.data.pet_monopoly_smart.cur_pray_id == 0 then
			FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_CLEAR_EXPECT_PET, self.param == 3 and 1 or 0)
		else
			FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_SET_EXPECT_PET, self.data.pet_monopoly_smart.cur_pray_id)
		end
	end
	ViewMgr:CloseView(FightPetPraySelect)
end

function FightPetPraySelect:OnClickClose()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
	if self.data.pet_monopoly_smart.cur_pray_id ~= self.data:GetPetPreyId(self.param) then
		if self.data.pet_monopoly_smart.cur_pray_id == 0 then
			FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_CLEAR_EXPECT_PET, self.param == 3 and 1 or 0)
		else
			FightCtrl.SendPetMonopolyOperate(FIGHT_PET_ISLAND_OPER_TYPE.OPER_TYPE_SET_EXPECT_PET, self.data.pet_monopoly_smart.cur_pray_id)
		end
	end
	ViewMgr:CloseView(FightPetPraySelect)
end


-----------------
FightPetPraySelectFlush = FightPetPraySelectFlush or DeclareMono("FightPetPraySelectFlush", UIWFlushPanel)
function FightPetPraySelectFlush:FightPetPraySelectFlush()
	self.data = FightData.Instance
	self.data_cares = {
		{data = self.data.pet_monopoly_smart, func = self.FlushPetIcon, keys = {"cur_pray_id"}},
}
end

function FightPetPraySelectFlush:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.delay_get ~= nil then
		TimeHelper:CancelTimer(self.delay_get)
	end
	if self.press_timer then
		TimeHelper:CancelTimer(self.press_timer)
		self.press_timer = nil
	end
end

function FightPetPraySelectFlush:FlushPetIcon()
	if self.data.pet_monopoly_smart.cur_pray_id == 0 then
		self.AddImgObj:SetActive(true)
		UH.SetIcon(self.Icon)
	else
		local pet_cfg = PetData.Instance:GetPetCfg(self.data.pet_monopoly_smart.cur_pray_id)
		self.AddImgObj:SetActive(false)
		UH.SetIcon(self.Icon, pet_cfg.icon_id)
	end
end

function FightPetPraySelectFlush:OnClickIcon()
	if self.data.pet_monopoly_smart.cur_pray_id ~= 0 then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
		self.data.pet_monopoly_smart.cur_pray_id = 0
	end
end


function FightPetPraySelectFlush:OnPressIconDown()
	if self.press_timer then
		TimeHelper:CancelTimer(self.press_timer)
		self.press_timer = nil
	end
	if self.data.pet_monopoly_smart.cur_pray_id ~= 0 then
		self.press_timer = TimeHelper:AddDelayTimer(function ()
			self.press_icon = true
			local pet_cfg = PetData.Instance:GetPetCfg(self.data.pet_monopoly_smart.cur_pray_id)
			local pet = Pet.New(pet_cfg)
			ViewMgr:OpenView(PetDetailView, pet)
		end,0.5)
	end
end

function FightPetPraySelectFlush:OnPressIconUp()
	if not self.press_icon then
		if self.press_timer then
			TimeHelper:CancelTimer(self.press_timer)
			self.press_timer = nil
		end
		if self.data.pet_monopoly_smart.cur_pray_id ~= 0 then
			AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
			self.data.pet_monopoly_smart.cur_pray_id = 0
		end
	end
	self.press_icon = false
end

function FightPetPraySelectFlush:OnPressItemDown(data)
	if self.press_timer then
		TimeHelper:CancelTimer(self.press_timer)
		self.press_timer = nil
	end
	self.press_timer = TimeHelper:AddDelayTimer(function ()
		self.press_item = true
		local pet_cfg = PetData.Instance:GetPetCfg(data.pet:ID())
		local pet = Pet.New(pet_cfg)
		ViewMgr:OpenView(PetDetailView, pet)
	end,0.5)
end

function FightPetPraySelectFlush:OnPressItemUp(data)
	if self.press_timer then
		TimeHelper:CancelTimer(self.press_timer)
		self.press_timer = nil
	end
	if not self.press_item then
		if self.data.pet_monopoly_smart.cur_pray_id ~= data.pet:ID() then
			self.data.pet_monopoly_smart.cur_pray_id = data.pet:ID()
		else
			self.data.pet_monopoly_smart.cur_pray_id = 0
		end
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
	end
	self.press_item = false
end
-----------------------------------------
FightPetPraySelectItem = FightPetPraySelectItem or DeclareMono("FightPetPraySelectItem",UIWidgetBaseItem)
function FightPetPraySelectItem:FightPetPraySelectItem()
	self.care_handle = FightData.Instance.pet_monopoly_smart:KeysCare({"cur_pray_id"},function ()
		self:FlushSelect()
	end)
end

function FightPetPraySelectItem:OnDestroy()
	if self.care_handle ~= nil then
		FightData.Instance.pet_monopoly_smart:Uncare(self.care_handle)
		self.care_handle = nil
	end
end

function FightPetPraySelectItem:SetData(data)
	local pet_cfg = PetData.Instance:GetPetCfg(data.id)
	data.pet = Pet.New(pet_cfg)
	UH.SpriteName(self.QuaFrame, "pet_qua_"..data.pet:Quality())
	UH.SetIcon(self.Icon, data.pet:IconId())
	UH.SetText(self.PetName, data.pet:CfgName())
	self.data = data
	self:FlushSelect()
end

function FightPetPraySelectItem:FlushSelect()
	if self.data == nil or self.data.pet == nil or self.data.pet.ID == nil then
		return
	end
	if FightData.Instance.pet_monopoly_smart.cur_pray_id == self.data.pet:ID() then
		self.SelectObj:SetActive(true)
		self.Interactor.Interactable = false
	else
		self.SelectObj:SetActive(false)
		self.Interactor.Interactable = true
	end
end

function FightPetPraySelectItem:OnClickItem(toggle_on)
	if toggle_on then
		FightData.Instance.pet_monopoly_smart.cur_pray_id = self.data.pet:ID()
	else
		FightData.Instance.pet_monopoly_smart.cur_pray_id = 0
	end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
end