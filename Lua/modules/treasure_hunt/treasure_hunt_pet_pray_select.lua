TreasureHuntPetPraySelect = TreasureHuntPetPraySelect or DeclareView("TreasureHuntPetPraySelect", "treasure_hunt/treasure_hunt_pet_pray_select")
VIEW_DECLARE_MASK(TreasureHuntPetPraySelect, ViewMask.BgBlock)
function TreasureHuntPetPraySelect:TreasureHuntPetPraySelect()
	self.pet_data = PetData.Instance
	self.hunt_data = TreasureHuntData.Instance
end

function TreasureHuntPetPraySelect:LoadCallback(param)
	UH.SpriteName(self.QuaFrame, "pet_qua_"..(param + 1))
	UH.SetText(self.PrayPetText, Language.TreasureHunt.PrayPetType[param])
	UH.SetText(self.RateText, string.format(Language.Common.Percent, tostring(TreasureHuntData.BlessRate(param))))
	UH.SetText(self.RateTipText, Language.TreasureHunt.PrayRateTip[param])
	local pet_list = self.pet_data:GetPetListQuality(param + 1)
	self.SelectList:SetData(pet_list)
end

function TreasureHuntPetPraySelect:OnClickConfirm()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
	TreasureHuntCtrl.sendBlessReq()
	ViewMgr:CloseView(TreasureHuntPetPraySelect)
end

function TreasureHuntPetPraySelect:OnClickClose()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
	TreasureHuntCtrl.sendBlessReq()
	ViewMgr:CloseView(TreasureHuntPetPraySelect)
end

function TreasureHuntPetPraySelect:OnClickTip()
	local tip_str = Config.language_cfg_auto.textdesc[Config.treasure_hunt_auto.other[1].account_seq].desc
	PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = tip_str})
end
-----------------------------------------
TreasureHuntPetSelectedShow = TreasureHuntPetSelectedShow or DeclareMono("TreasureHuntPetSelectedShow", UIWFlushPanel)
function TreasureHuntPetSelectedShow:TreasureHuntPetSelectedShow()
	self.data_cares = {
		{data = TreasureHuntData.Instance.pets_bless_info, func = self.FlushPetIcon, keys = {"cur_pray_id"}},
}
end

function TreasureHuntPetSelectedShow:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	if self.press_timer then
		TimeHelper:CancelTimer(self.press_timer)
		self.press_timer = nil
	end
end

function TreasureHuntPetSelectedShow:FlushPetIcon()
	if TreasureHuntData.Instance.pets_bless_info.cur_pray_id == 0 then
		self.AddImgObj:SetActive(true)
		UH.SetIcon(self.Icon)
	else
		local pet_cfg = PetData.Instance:GetPetCfg(TreasureHuntData.Instance.pets_bless_info.cur_pray_id)
		self.AddImgObj:SetActive(false)
		UH.SetIcon(self.Icon, pet_cfg.icon_id)
	end
end

function TreasureHuntPetSelectedShow:OnClickIcon()
	if TreasureHuntData.Instance.pets_bless_info.cur_pray_id ~= 0 then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
		TreasureHuntData.Instance.pets_bless_info.cur_pray_id = 0
	end
end

function TreasureHuntPetSelectedShow:OnPressIconDown()
	if self.press_timer then
		TimeHelper:CancelTimer(self.press_timer)
		self.press_timer = nil
	end
	if TreasureHuntData.Instance.pets_bless_info.cur_pray_id ~= 0 then
		self.press_timer = TimeHelper:AddDelayTimer(function ()
			self.press_icon = true
			local pet_cfg = PetData.Instance:GetPetCfg(TreasureHuntData.Instance.pets_bless_info.cur_pray_id)
			local pet = Pet.New(pet_cfg)
			ViewMgr:OpenView(PetDetailView, pet)
		end,0.5)
	end
end

function TreasureHuntPetSelectedShow:OnPressIconUp()
	if not self.press_icon then
		if self.press_timer then
			TimeHelper:CancelTimer(self.press_timer)
			self.press_timer = nil
		end
		if TreasureHuntData.Instance.pets_bless_info.cur_pray_id ~= 0 then
			AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
			TreasureHuntData.Instance.pets_bless_info.cur_pray_id = 0
		end
	end
	self.press_icon = false
end

function TreasureHuntPetSelectedShow:OnPressItemDown(data)
	if self.press_timer then
		TimeHelper:CancelTimer(self.press_timer)
		self.press_timer = nil
	end
	self.press_timer = TimeHelper:AddDelayTimer(function ()
		self.press_item = true
		local pet_cfg = PetData.Instance:GetPetCfg(data:ID())
		local pet = Pet.New(pet_cfg)
		ViewMgr:OpenView(PetDetailView, pet)
	end,0.5)
end

function TreasureHuntPetSelectedShow:OnPressItemUp(data)
	if self.press_timer then
		TimeHelper:CancelTimer(self.press_timer)
		self.press_timer = nil
	end
	if not self.press_item then
		if TreasureHuntData.Instance.pets_bless_info.cur_pray_id ~= data:ID() then
			TreasureHuntData.Instance.pets_bless_info.cur_pray_id = data:ID()
		else
			TreasureHuntData.Instance.pets_bless_info.cur_pray_id = 0
		end
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
	end
	self.press_item = false
end
-----------------------------------------
TreasureHuntPetPraySelectItem = TreasureHuntPetPraySelectItem or DeclareMono("TreasureHuntPetPraySelectItem",UIWidgetBaseItem)
function TreasureHuntPetPraySelectItem:TreasureHuntPetPraySelectItem()
	self.care_handle = TreasureHuntData.Instance.pets_bless_info:KeysCare({"cur_pray_id"},function ()
		self:FlushSelect()
	end)
end

function TreasureHuntPetPraySelectItem:OnDestroy()
	if self.care_handle ~= nil then
		TreasureHuntData.Instance.pets_bless_info:Uncare(self.care_handle)
		self.care_handle = nil
	end
end	

function TreasureHuntPetPraySelectItem:SetData(data)
	UH.SpriteName(self.QuaFrame, "pet_qua_"..data:Quality())
	UH.SetIcon(self.Icon, data:IconId())
	UH.SetText(self.PetName, data:CfgName())
	self.data = data
	self:FlushSelect()
end

function TreasureHuntPetPraySelectItem:FlushSelect()
	if self.data == nil or self.data.ID == nil then
		return
	end
	if TreasureHuntData.Instance.pets_bless_info.cur_pray_id == self.data:ID() then
		self.SelectObj:SetActive(true)
		self.Interactor.Interactable = false
	else
		self.SelectObj:SetActive(false)
		self.Interactor.Interactable = true
	end
end

function TreasureHuntPetPraySelectItem:OnClickItem(toggle_on)
	if toggle_on then
		TreasureHuntData.Instance.pets_bless_info.cur_pray_id = self.data:ID()
	else
		TreasureHuntData.Instance.pets_bless_info.cur_pray_id = 0
	end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
end