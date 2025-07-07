ActFormationView = ActFormationView or DeclareView("ActFormationView", "act_formation/act_formation_view")
VIEW_DECLARE_LEVEL(ActFormationView,ViewLevel.L1)
function ActFormationView:LoadCallback(param)
	LogDG(param)
	if param and param.hide_challenge then
		self.ChanllengtBtnObj:SetActive(false)
		UH.AnchoredPosition(self.RightMaskRect, Vector2.New(0,-89))
		self.BottomListRect:SetSizeWithCurrentAnchors(0, self.BottomListRect.rect.width + 150)
	end
	ActFormationData.Instance:GetAssistLimistTextList()
	ActFormationData.Instance:FlushAssistLimitTextList()
	ActFormationData.Instance.formation_smart:SetDirty("click_char")
	self:SetCrystalTip()
end

function ActFormationView:SetCrystalTip()
	local tip_str ,icon_id = ActFormationData.Instance:GetCrystalTip()
	if icon_id then
		UH.SetText(self.ActCrystalTip, tip_str)
		UH.SetIcon(self.CrystalIcon, icon_id, ICON_TYPE.ITEM)
		self.ActCrystalTipObj:SetActive(true)
	else
		self.ActCrystalTipObj:SetActive(false)
	end
end

function ActFormationView:CloseCallback()
	ActFormationData.Instance:SetClickChar(false)
end

----------------------------------
ActFormationPanel = ActFormationPanel or DeclareMono("ActFormationPanel", UIWFlushPanel)
function ActFormationPanel:ActFormationPanel()
	self.data = ActFormationData.Instance
	self.partner_data = PartnerCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data.formation_smart, func = self.FlushCurSelShow, keys = {"cur_is_pet","pos_changed"}},
		{data = self.data.formation_smart, func = self.FlushDetails, keys = {"pos_changed"}},
		{data = self.data.formation_smart, func = self.FlushFormationSel, keys = {"pos_change"}, init_call = false},
		{data = self.data.formation_smart, func = self.ShowStatuPanel, keys = {"partner","pet_index"}, init_call = false},
		{data = self.data.formation_smart, func = self.FlushSkillItems, keys = {"skill_change"}}
	}
	self.guide_jump = false
	self.not_saved = true
	GuideManager.Instance:RegisterGetGuideUi("ActFormationAssistObj", function()
        if self.TopAssistObj then
            return self.TopAssistObj, function ()
            	RoleCtrl.Instance:SendFuncGuideOperator(FUNCTIONGUIDEOPTYPE.FUNC_GUIDE_FINISH_CLOUD_ARENA_GUIDE)
            end
        end
    end)
	GuideManager.Instance:RegisterGetGuideUi("ActFormationGuidePet",function ()
		local pet_id = GuideData.Instance:GetActFormationGuidePetId()
		local select_index = -1
		for k, v in pairs(self.data:GetPetList()) do
			if v.pet_id == pet_id then
				select_index = k
				break
			end
		end
		if select_index == -1 then
			self.guide_jump = true
			GuideManager.Instance:Continue()
		else
			if self.BottomList:GetItem(select_index) ~= nil then
				return self.BottomList:GetItem(select_index).gameObject, function ()
					self.BottomList:GetItem(select_index):OnClickItem()
				end
			end
		end
	end)
	GuideManager.Instance:RegisterGetGuideUi("ActFormationEmptyPetSlot",function ()
		if self.guide_jump then
			GuideManager.Instance:Continue()
		else
			return self.ShowItems[7].gameObject, function ()
				self.ShowItems[7]:OnClickItem()
			end 
		end
	end)
    self.data.formation_smart.auto_save_type = -1
end

function ActFormationPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	self.data.formation_smart.cur_is_pet = 0
	self.data.formation_smart.skill_change = -1
	self.data:PosChangeReset()
	self.data:ClearCurSkillSmart()
end

function ActFormationPanel:FlushCurSelShow()
	local show_list = self.data.formation_smart.cur_is_pet == 1 and self.data:GetPetList() or self.data:GetPartnerList()
	for i=1,self.TypeToggles:Length() do
		self.TypeToggles[i].isOn = self.data.formation_smart.cur_is_pet == i - 1
	end
	UH.SetText(self.FormationTips, self.data.formation_smart.cur_is_pet == 0 and Language.ActFormation.FormationTipsPartner or Language.ActFormation.FormationTipsPet)
	self.BottomList:SetData(show_list)
end

function ActFormationPanel:SetTopAssistInfo()
	UH.SetText(self.TopAssistInfo, self.data:GetAssistText())
end

function ActFormationPanel:ShowSelectStatusPanel()
	self.SelectStatusPanelObj:SetActive(self.data.formation_smart.partner ~= 0 or self.data.formation_smart.pet_index ~= -1)
end

function ActFormationPanel:FlushFormationSel()
	local lights = self.data.formation_smart.pos_change.lights
	local arrow = self.data.formation_smart.pos_change.arrow
	for i = 1, self.ShowItems:Length() do
		self.ShowItems[i]:ArrowShow(i == arrow)
		self.ShowItems[i]:LightShow(lights[i] and i ~= arrow, lights[i] and i == arrow)
	end
end

function ActFormationPanel:ShowStatuPanel()
	self.SelectStatuPanelObj:SetActive(self.data.formation_smart.partner ~= 0 or self.data.formation_smart.pet_index ~= -1)
end

function ActFormationPanel:FlushDetails()
	local formation_info = self.data:FormationInfo()
	for i = 1, self.ShowItems:Length() do
		self.ShowItems[i]:SetData(formation_info[i])
	end
	self:SetTopAssistInfo()
end

function ActFormationPanel:FlushSkillItems()
	local skills = self.data:GetCurSelSkills()
	for i = 1, self.SkillItems:Length() do
		self.SkillItems[i]:SetData(skills[i])
	end
end

function ActFormationPanel:OnClickClose(jump)
	if self:checkSaved("close") or jump == true then
		self.data.formation_smart.auto_save_type = -1
		ViewMgr:CloseView(ActFormationView)
	end
end

function ActFormationPanel:OnClickToggle(param)
	self.data.formation_smart.cur_is_pet = param
	self.data:SetClickChar(false)
	self.data:PosChangeReset()
end

function ActFormationPanel:OnClickCrystal()
	self.partner_data.cur_sel_crystal.role = 1
	ViewMgr:OpenView(PartnerCrystalView)
end

function ActFormationPanel:OnClickCopetSkill()
	-- LogError("GetCopetList count ",#self.data:GetCopetList())
	if #self.data:GetCopetList() > 0 then
		ViewMgr:OpenView(ActFormationCoPetSkillView)
	else
		PublicPopupCtrl.Instance:Center(Language.ActFormation.NoCopetYet)
	end
end

function ActFormationPanel:OnClickPet()
	ViewMgr:OpenView(FormationPetView)
end

function ActFormationPanel:checkSaved(b_key)
	--LogError("b_key",b_key,self.data:CheckFormationChange())
	if self.not_saved and self.data:CheckFormationChange() then
		local info = {
			content = Language.ActFormation.SaveCurFormation,
			cancel = {
				name = Language.ActFormation.NotSave,
				func = function ()
					if b_key == "close" then
						self:OnClickClose(true)
					elseif b_key == "challenge" then
						self:OnClickChallenge(true)
					end
					PublicPopupCtrl.Instance:CloseDialogTips()
				end
			},
			confirm = {
				name = Language.ActFormation.SaveFormation,
				func = function ()
					self:OnClickSave()
					PublicPopupCtrl.Instance:CloseDialogTips()
					if b_key == "close" then
						self:OnClickClose(true)
					elseif b_key == "challenge" then
						self:OnClickSave()
						self:OnClickChallenge(true)
					end
				end
			},
		}
		PublicPopupCtrl.Instance:DialogTips(info)
		return false
	else
		return true
	end
end

function ActFormationPanel:OnClickChallenge(jump)
	if MarryData.Instance:IsParadeFloat() then
        PublicPopupCtrl.Instance:Center(Language.Marry.ParadeFloatTip)
        return
    end
	if self.data:CheckHasEmptyPosCanUse() then
		local info = {
			content = Language.ActFormation.FormationHasEmptyCanUse,
			confirm = { func = function ()
				self.data.formation_smart.auto_save_type = self.data.cur_system_type
				self:OnClickSave()
				PublicPopupCtrl.Instance:CloseDialogTips()
			end,}
		}
		PublicPopupCtrl.Instance:DialogTips(info)
	else
		self.data.formation_smart.auto_save_type = self.data.cur_system_type
		self:OnClickSave()
	end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
end

function ActFormationPanel:OnClickSave()
	self.not_saved = false
	ActFormationCtrl.Instance:SendSetFormationInfo()
end
----------------------------------------
ActFormationShowItem = DeclareMono("ActFormationShowItem", UIWidgetBaseItem)
function ActFormationShowItem:ActFormationShowItem()
	self.data_instance = ActFormationData.Instance
end

function ActFormationShowItem:OnDestroy()
    self:uncareData()
end

function ActFormationShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self:uncareData()
	self:careData(BagData.Instance.item_grid[ItemColumnType.Item], BindTool.Bind(self.FlushEleAttr, self))
	self:careData(PartnerData.Instance.cur_sel_crystal, BindTool.Bind(self.FlushEleAttr, self),"flush")
	self:careData(self.data_instance.formation_smart, BindTool.Bind(self.FlushLock, self),"click_char")
	local is_empty = nil == data
	self.InfoObj:SetActive(not is_empty)
	if is_empty then
		return
	end

	self.Name.text = data.name
	if nil ~= data.ele_attr then self.EleAttr:SetData(data.ele_attr) end
	self.EleAttr:SetObjActive(nil ~= data.ele_attr)
	if data.member then self.ProfI.SpriteName = data.prof_sp end
	self.ProfI:SetObjActive(data.member)
	if not self.ui_obj then
		self.ui_obj = UIChDrawer.New()
		if data.role then
			self.ui_obj:SetData(data.appearance)
		elseif data.partner or data.pet  then
			self.ui_obj:SetMainSolo("actor/npc/" .. data.res_id ..  ".prefab", data.fly_flag)
		end
		self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
				view = self,
				show_shadow = false,
			}
		)
	else
		if data.role or data.member then
			self.ui_obj:SetData(data.appearance)
		elseif data.partner or data.pet then
			self.ui_obj:SetMainSolo("actor/npc/" .. data.res_id ..  ".prefab", data.fly_flag)
		end
	end
	self.MainPetObj:SetActive(data and data.pet and data.pet_index == self.data_instance.main_pet_index)
end

function ActFormationShowItem:FlushLock()
	if self.data_instance.formation_smart.click_char or self.PosIndex == 3 or self.PosIndex == 8 then--(self.data ~= nil and (self.data.role or self.data.pet or self.data.partner)) then
		self.LockObj:SetActive(false)
	elseif self.PosIndex >= 6 then
		local oppo_pos_data = self.data_instance:FormationInfo(ActFormationData.LockPosIndex[self.PosIndex].oppo_pos)
		if self.data_instance:GetAssistCount() >= self.data_instance.pet_on_limit then
			if self.data == nil and (oppo_pos_data == nil or not oppo_pos_data.pet) then
				UH.SetText(self.LockTipTxt, self.data_instance:GetAssistLimistText(self.PosIndex))
				self.LockObj:SetActive(true)
			else
				self.LockObj:SetActive(false)
			end
		elseif self.data == nil and self.data_instance.pet_on_limit < 4 then
			if ActFormationData.LockPosIndex[self.PosIndex].need_open_num + self.data_instance:GetAssistCount() > self.data_instance.pet_on_limit and self.data_instance:GetAssistLimistText(self.PosIndex) ~= nil
				and (oppo_pos_data == nil or not oppo_pos_data.pet) then
				UH.SetText(self.LockTipTxt, self.data_instance:GetAssistLimistText(self.PosIndex))
				self.LockObj:SetActive(true)
			else
				self.LockObj:SetActive(false)
			end
		else
			self.LockObj:SetActive(false)
		end
	else
		local oppo_pos_data = self.data_instance:FormationInfo(ActFormationData.LockPosIndex[self.PosIndex].oppo_pos)
		if self.data == nil and oppo_pos_data ~= nil and oppo_pos_data.partner and self.data_instance:GetAssistLimistText(self.PosIndex) then
			UH.SetText(self.LockTipTxt, self.data_instance:GetAssistLimistText(self.PosIndex))
			self.LockObj:SetActive(true)
		else
			self.LockObj:SetActive(false)
		end
	end
end

function ActFormationShowItem:FlushEleAttr()
	local data = self.data
	if data then
		if data.role then
			local crystal_id = BagData.Instance:ViewData().element_crystal_id
			data.ele_attr = crystal_id > 0 and EleCrystal.GetElement(crystal_id) or nil
			if nil ~= data.ele_attr then self.EleAttr:SetData(data.ele_attr) end
			self.EleAttr:SetObjActive(nil ~= data.ele_attr)
		elseif data.partner then
			local partner_info = PartnerData.Instance:GetPartnerInfoById(data.partner_id)
			data.ele_attr = partner_info.vo.crystal > 0 and EleCrystal.GetElement(partner_info.vo.crystal) or nil
			if nil ~= data.ele_attr then self.EleAttr:SetData(data.ele_attr) end
			self.EleAttr:SetObjActive(nil ~= data.ele_attr)
		end
	end
end

function ActFormationShowItem:OnClickItem()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	if self.data_instance.formation_info[self.PosIndex] then
		if self.data_instance.formation_info[self.PosIndex].pet or self.data_instance.formation_info[self.PosIndex].partner then
			if self.data_instance.formation_smart.pos_change.lights[self.PosIndex] or self.data_instance.formation_smart.partner == 0 and self.data_instance.formation_smart.pet_index == -1 then
				self.data_instance.formation_smart.cur_is_pet = self.data_instance.formation_info[self.PosIndex].partner and 0 or 1
			end
		end
		if not self.data_instance.formation_smart.click_char then
			-- LogError("Onclickitem 1")
			self.data_instance:SetClickChar(true)
		elseif self.data_instance.formation_smart.pos_change.lights[self.PosIndex] then
			-- LogError("Onclickitem 2")
			self.data_instance:SetClickChar(false)
		end
		-- end
	elseif self.data_instance.formation_smart.click_char and self.data_instance.formation_smart.pos_change.lights[self.PosIndex] then
		-- LogError("Onclickitem 7")
		self.data_instance:SetClickChar(false)
	end
	if self.data_instance.formation_smart.cur_is_pet == 0 then
		if self.data ~= nil and self.data_instance.formation_smart.partner == self.data.partner_id then
			self.data_instance:SetSelectPartner(self.data.partner_id)
			if self.data_instance.formation_smart.click_char and self.data_instance.formation_smart.pos_change.lights[self.PosIndex] then
				-- LogError("Onclickitem 3")
				self.data_instance:SetClickChar(false)
			end
			return
		end
		local partner = self.data_instance.formation_smart.partner
		if partner > 0 then
			if self.data_instance:PartnerOnFight(partner) then
				self.data_instance:PosChange(self.PosIndex)
			else
				self.data_instance:PartnerAddEnd(self.PosIndex, partner)
			end
			
			if self.data_instance.formation_smart.click_char and self.data_instance.formation_smart.pos_change.lights[self.PosIndex] then
				-- LogError("Onclickitem 4")
				self.data_instance:SetClickChar(false)
			end
			self.data_instance:FlushAssistLimitTextList()
		else
			self.data_instance:SetSelectPartner(self.data and self.data.partner_id or 0)
			self.data_instance:PosChange(self.PosIndex)
			self.data_instance:FlushAssistLimitTextList()
		end
	else
		self.data_instance.formation_smart.click_pet_card = false
		local pet_index = self.data_instance.formation_smart.pet_index
		if pet_index and pet_index > -1 then
			if self.data_instance:PetOnFight(pet_index) then
				self.data_instance:PosChange(self.PosIndex)
			else
				self.data_instance:PetAddEnd(self.PosIndex, pet_index)
			end
			
			if self.data_instance.formation_smart.click_char and self.data_instance.formation_smart.pos_change.lights[self.PosIndex] then
				-- LogError("Onclickitem 5")
				self.data_instance:SetClickChar(false)
			end
			self.data_instance:FlushAssistLimitTextList()
		else
			self.data_instance.formation_smart.pet_index = self.data ~= nil and self.data.pet and self.data.pet_index or -1
			self.data_instance:PosChange(self.PosIndex)
			-- LogError("Onclickitem 6")
			self.data_instance:FlushAssistLimitTextList()
		end
	end
end

function ActFormationShowItem:ArrowShow(value)
	self.ArrowObj:SetActive(value and self.data.role)
end

function ActFormationShowItem:LightShow(value1, value2)
	self.LightObj:SetActive(value1 or false)
	self.Light2Obj:SetActive(value2 or false)
end

-----------------------------------------------------------------------
ActFormationItem = DeclareMono("ActFormationItem", UIWidgetBaseItem)
--PetCell============UIWSubstitute[UIWPetHeadCell]
function ActFormationItem:ActFormationItem()
	self.data_instance = ActFormationData.Instance
	self.formation_smart = self.data_instance.formation_smart
end

function ActFormationItem:OnDestroy()
	if self.care_handle ~= nil then
		self.formation_smart:Uncare(self.care_handle)
		self.care_handle = nil
	end
end

function ActFormationItem:SetData(data)
	self:uncareData()
	if self.care_handle == nil then
		self.care_handle = self.formation_smart:KeysCare({"partner","pet_index"}, BindTool.Bind1(self.FlushSelect, self))
	end
	self.data = data
	self.FSQuaAnim:SetObjActive(false)
	if self.formation_smart.cur_is_pet == 0 then
		local info = data.info
	    local vo = data.vo
		local prof_sp = RoleData.GetProfSp(info.job)
		local lock_state = PartnerConfig.LockState
		
		UH.SpriteName(self.PartnerStrLevel, PartnerInfo.StrLevelSp(vo.intensify_level))
		UH.SetText(self.Name, string.format("<color=#%s>%s</color>", vo.is_unlocked == lock_state.unlock and COLORSTR.Yellow13 or COLORSTR.Gray4, info.name))
	    UH.SpriteName(self.Prof, prof_sp)
		UH.SetIcon(self.IconSp, PartnerData.IconId(data), ICON_TYPE.ITEM)
		local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)
		PartnerInfo.SetQuality(self.Quality, info_quality.quality)
		self.ItemInter.Interactable = vo.is_unlocked == lock_state.unlock
		-- UH.SpriteName(self.StrImg, "")
		local status_img_name = self.data_instance:PartnerOnFight(data.id) == nil and "" or PPStateSp.zhan
		status_img_name = vo.is_unlocked == lock_state.unlock and status_img_name or ""
		UH.SpriteName(self.StatusImg, status_img_name)
		self.Quality:SetObjActive(true)
		self.IconSp:SetObjActive(true)
		self.PetCell:Clean()
		UIWPetHeadCell.EffAnimName(vo.soar_count, self.FSQuaAnim)
		-- self.LevelObj:SetActive(false)
	elseif self.data.pet_id ~= nil then
		UH.SetText(self.Name, data:Name())
		if data.IsLianYao then
			UH.Color(self.Name, data:IsLianYao() and COLORS.Pink2 or COLORS.Yellow13)
		end
		-- UH.SetText(self.LevelText, data:Level())
		UH.SpriteName(self.Prof, nil)
		-- UH.SetIcon(self.IconSp, data:IconId(), ICON_TYPE.ITEM)
		self.ItemInter.Interactable = true
		-- UH.SpriteName(self.Quality, "pet_qua_"..data:Quality())
		-- UH.SpriteName(self.StrImg, "PetNum"..data:StrLevel())
		local imgName = self.data_instance:PetFormationStatus(data.index)
		UH.SpriteName(self.StatusImg, imgName)
		UH.SpriteName(self.PartnerStrLevel, "")
		self.PetCell:SetDataByPet(data)
		self.Quality:SetObjActive(false)
		self.IconSp:SetObjActive(false)

		-- self.LevelObj:SetActive(true)
	end
	self:FlushSelect()
end

function ActFormationItem:FlushSelect()
	if self.Selector and self.data then
		if self.formation_smart.cur_is_pet == 0 then
			if self.data.vo and self.data.vo.is_unlocked == PartnerConfig.LockState.unlock then
				self.Selector.isOn = self.formation_smart.partner == self.data.id
			else
				self.Selector.isOn = false
			end
			local status_img_name = self.data_instance:PartnerOnFight(self.data.id) == nil and "" or PPStateSp.zhan
			UH.SpriteName(self.StatusImg, status_img_name)
		else
			self.Selector.isOn = self.formation_smart.pet_index == self.data.index
			local imgName, state = self.data_instance:PetFormationStatus(self.data.index)
			UH.SpriteName(self.StatusImg, imgName)
		end
	end
end

function ActFormationItem:OnClickItem()
	if self.formation_smart.cur_is_pet == 0 then
		if self.formation_smart.partner == self.data.id then
			self.data_instance:SetClickChar(false)
			self.data_instance:SetSelectPartner(self.data.id)
			self.data_instance:PosChangeReset()
		else
			local is_lock = self.data.vo.is_unlocked == PartnerConfig.LockState.lock
			if is_lock then
				self.data_instance:SetSelectPartner(self.data.id)
				self.data_instance:PosChangeReset()
				PublicPopupCtrl.Instance:Center(Language.Formation.PartnerNoLockTips)
				return
			end
			self.data_instance:SetClickChar(true)
			self.data_instance:SetSelectPartner(self.data.id)
			self.data_instance.formation_smart.pos_change.arrow = 0
			if self.data_instance:PartnerOnFight(self.data.id) then
				self.data_instance:PartnerChangePos(self.data.id)
			else
				self.data_instance:PartnerAddBegin()
			end
		end
	else
		self.data_instance.formation_smart.click_pet_card = true
		if self.formation_smart.pet_index == self.data.index then
			self.data_instance:SetClickChar(false)
			self.data_instance:SetSelectPet(self.data.index)
			self.data_instance:PosChangeReset()
		else
			self.data_instance:SetClickChar(true)
			self.data_instance:SetSelectPet(self.data.index)
			self.data_instance.formation_smart.pos_change.arrow = 0
			if self.data_instance:PetOnFight(self.data.index) then
				self.data_instance:PetChangePos(self.data.index)
			else
				self.data_instance:PetAddBegin()
			end
		end
	end
end

---------------------------------------------
ActFormationSelectStatuPanel = ActFormationSelectStatuPanel or DeclareMono("ActFormationSelectStatuPanel", UIWFlushPanel)
--PetCell============UIWSubstitute[UIWPetHeadCell]
--LevelObj===========GameObject
function ActFormationSelectStatuPanel:ActFormationSelectStatuPanel()
	self.data = ActFormationData.Instance
	self.data_cares = {
		{data = self.data.formation_smart, func = self.FlushInfo, keys = {"partner","pet_index"}, init_call = false},
		{data = self.data.formation_smart, func = self.FlushBtnState, keys = {"cur_select_state"}, init_call = false}
	}
end

function ActFormationSelectStatuPanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushInfo()
end

function ActFormationSelectStatuPanel:FlushInfo()
	if self.data.formation_smart.cur_is_pet == 0 then
		if self.data.formation_smart.partner == 0 then
			return
		end
		UH.SetText(self.TitleText, Language.ActFormation.FormationStatuPartner)
		local partner_info = PartnerData.Instance.partner_list[self.data.formation_smart.partner]
		local info = partner_info.info
	    local vo = partner_info.vo
		local prof_sp = RoleData.GetProfSp(info.job)
		local lock_state = PartnerConfig.LockState
		
		UH.SetText(self.Name, string.format("<color=#%s>%s</color>", vo.is_unlocked == lock_state.unlock and COLORSTR.Yellow13 or COLORSTR.Gray4, info.name))
	    UH.SetText(self.LevelText, vo.level)
	    UH.SpriteName(self.Prof, prof_sp)
		UH.SetIcon(self.IconSp, PartnerData.IconId(partner_info), ICON_TYPE.ITEM)
		local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(partner_info.id)
		PartnerInfo.SetQuality(self.Quality, info_quality.quality)
		-- UH.SpriteName(self.StrImg, "")
		UH.SetText(self.Score.ScoreTxt, vo.cap)
		UH.SpriteName(self.Score.ScoreLvImg, partner_info.cap_sp or "")
		local ele_attr = vo.crystal > 0 and EleCrystal.GetElement(vo.crystal) or nil
		if nil ~= ele_attr then self.EleAttr:SetData(ele_attr) end
		self.EleAttr:SetObjActive(nil ~= ele_attr)

		self.Quality:SetObjActive(true)
		self.IconSp:SetObjActive(true)
		self.LevelObj:SetActive(true)
		self.PetCell:Clean()

		for i = 1, self.HideButtons:Length() do
			self.HideButtons[i]:SetActive(false)
		end
	else
		if self.data.formation_smart.pet_index == -1 then
			return
		end
		local pet_info = PetData.Instance:GetPetInfo(self.data.formation_smart.pet_index)
		UH.SetText(self.TitleText, Language.ActFormation.FormationStatuPet)
		UH.SetText(self.Name, pet_info:Name())
		-- UH.SetText(self.LevelText, pet_info:Level())
		UH.SpriteName(self.Prof, nil)
		-- UH.SetIcon(self.IconSp, pet_info:IconId(), ICON_TYPE.ITEM)
		-- UH.SpriteName(self.Quality, "pet_qua_"..pet_info:Quality())
		-- UH.SpriteName(self.StrImg, "PetNum"..pet_info:StrLevel())
		self.Score:SetScore(pet_info:GetScore())
		self.EleAttr:SetData(pet_info:EleAttr())
		self.EleAttr:SetObjActive(true)
		self.Quality:SetObjActive(false)
		self.IconSp:SetObjActive(false)
		self.LevelObj:SetActive(false)
		self.PetCell:SetDataByPet(pet_info)

		for i = 1, self.HideButtons:Length() do
			self.HideButtons[i]:SetActive(true)
		end
	end
	self:FlushBtnState()
end

function ActFormationSelectStatuPanel:FlushBtnState()
	if self.data.formation_smart.partner ~= 0 or self.data.formation_smart.pet_index ~= -1 then
		for i=1, self.ToggleBtns:Length() do
			self.ToggleBtns[i].isOn = self.data.formation_smart.cur_select_state == i
		end
	end
end

function ActFormationSelectStatuPanel:OnClickCloseStatuPanel()
	self.data:PosChangeReset()
	self.data:SetClickChar(false)
end

function ActFormationSelectStatuPanel:OnClickStateButton(param)
	local set_click = nil
	if param == ACTFORMATION_STATE.Relax or param == ACTFORMATION_STATE.StandBy then
		set_click = false
	elseif self.data.formation_smart.cur_is_pet == 1 then
		if param ~= self.data.formation_smart.cur_select_state then
			if param ~= ACTFORMATION_STATE.Assist then
				set_click = false
			else
				set_click = true
			end
		elseif param == ACTFORMATION_STATE.Assist then
			set_click = true
		end
	elseif param == ACTFORMATION_STATE.Fight then
		set_click = true
	end
	self.data:SetSelectState(param)
	if param == ACTFORMATION_STATE.StandBy or param == ACTFORMATION_STATE.Relax then
		self.data.formation_smart.pet_index = -1
		self.data.formation_smart.partner = 0
	end
	if set_click ~= nil then
		self.data:SetClickChar(set_click)
	end
end