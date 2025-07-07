
local Input = UnityEngine.Input

FormationView = FormationView or DeclareView("FormationView", FormationConfig.ResPath .. "formation", Mod.Formation.Main)
VIEW_DECLARE_LEVEL(FormationView,ViewLevel.L0)

function FormationView:FormationView()
	self.data = FormationCtrl.Instance:Data()
	self.language = Language.ArenaMatch

	self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
	
	FormationCtrl.Instance:SendFormationSimpleInfo()
end

function FormationView:LoadCallback(param_t)
end

function FormationView:CloseCallback()
	local sel_seq = self.data.cur_formation_info.sel_seq
	local sel_formation = self.data.formation_list[sel_seq]
	FormationCtrl.Instance:SendFormationSaveComplete(sel_formation)
end

FormationViewF = FormationViewF or DeclareMono("FormationViewF", UIWFlushPanel)

function FormationViewF:FormationViewF()
	self.data = FormationCtrl.Instance:Data()
	self.partner_data = PartnerCtrl.Instance:Data()
	self.fight_data = FightCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data.cur_formation_info, func = self.FlushFormationList, keys = {"list_change"}, init_call = false},
		{data = self.data.cur_formation_info, func = self.FlushCurSelShow, keys = {"sel_seq", "list_change"}, init_call = false},
		{data = self.data.cur_formation_info, func = self.FlushFormationShow, keys = {"formation_change"}, init_call = false},
		{data = self.data.cur_formation_info, func = self.FlushFormationSel, keys = {"pos_change"}, init_call = false},
		{data = self.fight_data.cur_sel_skills, func = self.FlushShowSkills, init_call = false},
		-- {data = self.data.cur_formation_info, func = self.FlushFlyPartnerShow, keys = {"partner"}, init_call = false},
	}

	self.data.cur_formation_info.sel_seq = 0
	-- Runner.Instance:AddRunObj(self, 3)

	GuideManager.Instance:RegisterGetGuideUi("FormationPatnerFirst", function ()
        if self.PartnerList.index_item[1] then
            return self.PartnerList.index_item[1], function ()
			end
        end
    end)
	GuideManager.Instance:RegisterGetGuideUi("FormationCrystalBtn", function ()
		return self.BtnCrystalObj, function ()
		end
    end)
	GuideManager.Instance:RegisterGetGuideUi("FormationPetBtn", function ()
		return self.BtnPetObj, function ()
		end
    end)
	GuideManager.Instance:RegisterGetGuideUi("FormationPartnerSY", function ()
        local partner = PartnerData.Instance:GetPartnerInfoById(20)
		if self.PartnerList.data_t and self.PartnerList.index_item then
			local index = self.PartnerList.data_t[partner]
			if self.PartnerList.index_item[index] then
				return self.PartnerList.index_item[index], self.PartnerList.index_item[index]:Click()
			else
				return self.PartnerList.index_item[1], self.PartnerList.index_item[1]:Click()
			end
		end
    end)
end

-- function FormationViewF:OnDestroy()
-- 	UIWFlushPanel.OnDestroy(self)
-- 	Runner.Instance:RemoveRunObj(self)
-- end

function FormationViewF:FlushFormationList()
	local formation_list, cur_sel_formation = self.data:GetFormationShowList()
	self.FormationList:SetData(formation_list)
	self.FormationList:ClickItemData(cur_sel_formation)
end

function FormationViewF:FlushCurSelShow()
	local sel_seq = self.data.cur_formation_info.sel_seq
	local sel_formation = self.data.formation_list[sel_seq]
	if nil == sel_formation then return end

	if nil == sel_formation:Detail() or 0 == sel_seq then
		FormationCtrl.Instance:SendFormationDetailInfo(sel_seq)
	else
		self:FlushDetails()
	end

	self.BtnSaveNewObj:SetActive(0 == sel_seq)
	self.BtnDeleteObj:SetActive(sel_seq > 0)
end

function FormationViewF:FlushFormationShow()
	local sel_seq = self.data.cur_formation_info.sel_seq
	local formation_change = self.data.cur_formation_info.formation_change
	if sel_seq ~= formation_change.seq then return end
	self:FlushDetails()
end

function FormationViewF:FlushFormationSel()
	local lights = self.data.cur_formation_info.pos_change.lights
	local arrow = self.data.cur_formation_info.pos_change.arrow
	for i = 1, self.ShowItems:Length() do
		self.ShowItems[i]:ArrowShow(i == arrow)
		self.ShowItems[i]:LightShow(lights[i] and i ~= arrow, lights[i] and i == arrow)
	end
end

function FormationViewF:FlushFlyPartnerShow()
	if self.data.cur_formation_info.partner > 0 then
		self:OnDragPartner()
		local partner = self.partner_data:GetPartnerInfoById(self.data.cur_formation_info.partner)
		local res_id = partner.info.res_id
		local ele_attr = partner.vo.crystal > 0 and EleCrystal.GetElement(partner.vo.crystal) or nil
		self.PartnerName.text = partner.info.name
		if nil ~= ele_attr and EleCrystal.GetElement(partner.vo.crystal) or nil then self.PartnerEleAttr:SetData(ele_attr) end
		self.PartnerEleAttr:SetObjActive(nil ~= ele_attr)
		if not self.ui_obj_fly then
			self.ui_obj_fly = UIChDrawer.New()
			self.ui_obj_fly:SetMainSolo("actor/npc/" .. res_id ..  ".prefab")
			self.PartnerModelShow:SetShowData(
				{
					ui_obj = self.ui_obj_fly,
					view = self,
					show_shadow = false,
				}
			)
		else
			self.ui_obj_fly:SetMainSolo("actor/npc/" .. res_id ..  ".prefab")
		end
		self.PartnerPos:SetObjActive(true)
	else
		self.PartnerPos:SetObjActive(false)
	end
end

function FormationViewF:FlushDetails()
	local sel_seq = self.data.cur_formation_info.sel_seq
	local sel_formation = self.data.formation_list[sel_seq]
	local detail = sel_formation:Detail()
	local formation_info = sel_formation:FormationInfo()
	for i = 1, self.ShowItems:Length() do
		self.ShowItems[i]:SetData(formation_info[i])
	end
	local partner_list = self.data:GetPartnerList()
    self.PartnerList:SetData(partner_list)

	local click_id = self.data:GetPartnerClickId()
	if click_id then
		local partner = PartnerData.Instance:GetPartnerInfoById(click_id)
		if self.PartnerList.data_t and self.PartnerList.index_item then
			local index = self.PartnerList.data_t[partner]		
			TimeHelper:AddDelayFrameTimer(function ()
				self.PartnerList:ForceJumpHorizontal(-138 * (index - 1))
			end)
			TimeHelper:AddDelayFrameTimer(function ()
				if self.PartnerList.index_item[index] then
					self.PartnerList.index_item[index]:Click()
				end
			end, 2)
		end
	end

	local skills = 0 == sel_seq and self.fight_data.cur_sel_skills or sel_formation:ShowSkills()
	for i = 1, self.SkillItems:Length() do
		self.SkillItems[i]:SetData(skills[i])
		self.SkillItems[i]:SetItemInter(0 == sel_seq)
	end
end

function FormationViewF:FlushShowSkills()
	local sel_seq = self.data.cur_formation_info.sel_seq
	if 0 == sel_seq then
		local skills = self.fight_data.cur_sel_skills
		for i = 1, self.SkillItems:Length() do
			self.SkillItems[i]:SetData(skills[i])
			self.SkillItems[i]:SetItemInter(true)
		end
	end

end

function FormationViewF:OnClickClose()
	ViewMgr:CloseView(FormationView)
end

function FormationViewF:OnClickSaveNew()
	-- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	local sel_seq = self.data.cur_formation_info.sel_seq
	local sel_formation = self.data.formation_list[sel_seq]
	FormationCtrl.Instance:SendFormationSaveNew(sel_formation)
end

function FormationViewF:OnClickDelete()
	-- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	local sel_seq = self.data.cur_formation_info.sel_seq
	FormationCtrl.Instance:SendFormationDelete(sel_seq)
end

function FormationViewF:OnClickCrystal()
	self.partner_data.cur_sel_crystal.role = 1
	ViewMgr:OpenView(PartnerCrystalView)
end

function FormationViewF:OnClickPet()
	ViewMgr:OpenView(FormationPetView)
end

function FormationViewF:OnClickSave()
	local sel_seq = self.data.cur_formation_info.sel_seq
	local sel_formation = self.data.formation_list[sel_seq]
	FormationCtrl.Instance:SendFormationSaveComplete(sel_formation)
end

function FormationViewF:Update()
	if Input.GetMouseButton(0) then
		if FormationConfig.PatnerFlyState.start == self.data.partner_fly.state then
			if TimeHelper.GetServerTime() - self.data.partner_fly.time > 1 then
				self:OnBeginDragPartner()
			end
		elseif FormationConfig.PatnerFlyState.flying == self.data.partner_fly.state then
			self:OnDragPartner()
		end
	else
		if FormationConfig.PatnerFlyState.flying == self.data.partner_fly.state then
			self.data.partner_fly.state = FormationConfig.PatnerFlyState.finish
			self:OnUpPartner()
		end
	end
end

function FormationViewF:OnBeginDragPartner()
	local partner = self.partner_data:GetPartnerInfoById(self.data.partner_fly.partner)
	local is_lock = partner.vo.is_unlocked == PartnerConfig.LockState.lock
	self.data.partner_fly.state = is_lock and FormationConfig.PatnerFlyState.finish or FormationConfig.PatnerFlyState.flying
	if is_lock then
		PublicPopupCtrl.Instance:Center(Language.Formation.PartnerNoLockTips)
		return
	end
	self.PartnerScrollRect.enabled = false
	self.data.partner_fly.item:SetActive(false)
	self.data.cur_formation_info.partner = self.data.partner_fly.partner
	self.data:PartnerAddBegin()
end

function FormationViewF:OnUpPartner()
	self.data.partner_fly.item:SetActive(true)
	self.PartnerScrollRect.enabled = true
	local input_x = UnityEngine.Input.mousePosition.x / UnityEngine.Screen.width * self.ViewSize.rect.width - self.ViewSize.rect.width / 2
	local input_y = UnityEngine.Input.mousePosition.y / UnityEngine.Screen.height * self.ViewSize.rect.height - self.ViewSize.rect.height / 2
	local to_pos = 0
	for i = 1, 10 do
		local pos = self.PartnerTargets[i].localPosition
		if math.abs(input_x - pos.x) < 55 and math.abs(input_y - pos.y) < (i > 5 and 90 or 65) then
			to_pos = i
			break
		end
	end
	if to_pos > 0 then self.data:PartnerAddEnd(to_pos, self.data.cur_formation_info.partner) end
	self.data:PosChangeReset()
	self.data.cur_formation_info.partner = 0
end

function FormationViewF:OnDragPartner()
	local input_x = UnityEngine.Input.mousePosition.x / UnityEngine.Screen.width * self.ViewSize.rect.width - self.ViewSize.rect.width / 2
	local input_y = UnityEngine.Input.mousePosition.y / UnityEngine.Screen.height * self.ViewSize.rect.height - self.ViewSize.rect.height / 2
	self.PartnerPos.localPosition = Vector2.New(input_x, input_y)
end

FormationViewItem = DeclareMono("FormationViewItem", UIWidgetBaseItem)

function FormationViewItem:FormationViewItem()
	self.formation_data = FormationCtrl.Instance:Data()
end

function FormationViewItem:OnDestroy()
    self:uncareData()
end

function FormationViewItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    self:uncareData()
	self.care_handles = {}

	self:careData(self.formation_data.cur_formation_info, BindTool.Bind(self.FlushSelect, self), "sel_seq")

	self.Name.text = data:Name()
	self.BtnModifyObj:SetActive(data:ShowBtnModify())
end

function FormationViewItem:FlushSelect()
	if self.data:Seq() == self.formation_data.cur_formation_info.sel_seq then
		UIWidgetBaseItem.SetSelected(self, true)
	else
		UIWidgetBaseItem.SetSelected(self, false)
	end
end

function FormationViewItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function FormationViewItem:OnSelect(selected)
    local value = self.formation_data:CurSelSeq(self.data:Seq(), selected)
	if value then
		UIWidgetBaseItem.SetSelected(self, true)
    end
	if selected then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	end
end

function FormationViewItem:OnClickModify()
    PublicPopupCtrl.Instance:ResetName({reset_func =  function(name)
		FormationCtrl.Instance:SendFormationRename(self.data:Seq(), name)
    end, tips = Language.ResetName.Tips4})
end

FormationViewShowItem = DeclareMono("FormationViewShowItem", UIWidgetBaseItem)

function FormationViewShowItem:FormationViewShowItem()
	self.formation_data = FormationCtrl.Instance:Data()
end

function FormationViewShowItem:OnDestroy()
    self:uncareData()
end

function FormationViewShowItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
	self.care_handles = {}

	local is_empty = nil == data
	self.InfoObj:SetActive(not is_empty)
	if is_empty then return end

	self:careData(BagData.Instance:ViewData(), BindTool.Bind(self.FlushEleAttr, self), "element_crystal_id")
	self:careData(PartnerData.Instance.partner_crystal_change, BindTool.Bind(self.FlushEleAttr, self), "element_changed")

	self.Name.text = data.name
	if nil ~= data.ele_attr then self.EleAttr:SetData(data.ele_attr) end
	self.EleAttr:SetObjActive(nil ~= data.ele_attr)
	if data.member then self.ProfI.SpriteName = data.prof_sp end
	self.ProfI:SetObjActive(data.member)
	if not self.ui_obj then
		self.ui_obj = UIChDrawer.New()
		if data.role or data.member then
			self.ui_obj:SetData(data.appearance)
		elseif data.partner or data.pet then
			self.ui_obj:SetMainSolo("actor/npc/" .. data.res_id ..  ".prefab", data.fly_flag)
		end
		self.ModelShow:SetShowData(
			{
				ui_obj = self.ui_obj,
				view = self,
				show_shadow = false,
			}
		)

		-- local re_data = Game.RendererEffectData()
		-- re_data.materialNames = {nil, "Ghost201"}
		-- re_data.animationClipName = "UI_jueseguang201"

		-- self.ui_obj:SetRenderEffect(re_data)
		-- self.Effect:SetActive(true)
	else
		if data.role or data.member then
			self.ui_obj:SetData(data.appearance)
		elseif data.partner or data.pet then
			self.ui_obj:SetMainSolo("actor/npc/" .. data.res_id ..  ".prefab")
		end
	end
end

function FormationViewShowItem:FlushEleAttr()
	local data = self.data
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

function FormationViewShowItem:OnClickItem(value)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	local partner = self.formation_data.cur_formation_info.partner
	if partner > 0 then
		self.formation_data:PartnerAddEnd(value, partner)
	else
		self.formation_data:PosChange(value)
	end
end

function FormationViewShowItem:ArrowShow(value)
	self.ArrowObj:SetActive(value and self.data.role)
	self.RemoveObj:SetActive(value and not self.data.role)
end

function FormationViewShowItem:LightShow(value1, value2)
	self.LightObj:SetActive(value1 or false)
	self.Light2Obj:SetActive(value2 or false)
end

function FormationViewShowItem:OnClickRemove()
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.BagClose)
	self.formation_data:PPRemove(self.data, PetState.Relax)
end

FormationViewPartnerItem = DeclareMono("FormationViewPartnerItem", UIWidgetBaseItem)

function FormationViewPartnerItem:FormationViewPartnerItem()
	self.formation_data = FormationCtrl.Instance:Data()
	self.language = Language.Formation
end

function FormationViewPartnerItem:OnDestroy()
    self:uncareData()
end

function FormationViewPartnerItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
	self.care_handles = {}

	self:careData(self.formation_data.cur_formation_info, BindTool.Bind(self.FlushSelect, self), "partner")
	
	local info = data.info
    local vo = data.vo
	local prof_sp = RoleData.GetProfSp(info.job)
	local lock_state = PartnerConfig.LockState
	
	UH.SpriteName(self.StrLevel, PartnerInfo.StrLevelSp(vo.intensify_level))
	self.Name.text = string.format("<color=#%s>%s</color>", vo.is_unlocked == lock_state.unlock and COLORSTR.Yellow13 or COLORSTR.Gray4, info.name)
    self.Prof.SpriteName = prof_sp
	UH.SetIcon(self.IconSp, PartnerData.IconId(data), ICON_TYPE.ITEM)
	local info_quality = PartnerData.Instance:GetCurPartnerQualityInfo(data.id)
	PartnerInfo.SetQuality(self.Quality, info_quality.quality)
	self.ItemInter.Interactable = vo.is_unlocked == lock_state.unlock
	UIWPetHeadCell.EffAnimName(vo.soar_count, self.FSQuaAnim)

	self.RankObj:SetActive(PartnerData.PartnerRankShow(vo.rank))
	self.RankShow.text = string.format(self.language.PartnerRankShow, vo.rank)
end

function FormationViewPartnerItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

function FormationViewPartnerItem:FlushSelect()
	self.BgSp.SpriteName = self.formation_data.cur_formation_info.partner == self.data.id and FormationConfig.PartnerBgSp.select or FormationConfig.PartnerBgSp.normal
end

function FormationViewPartnerItem:OnClickItem()
	if self.formation_data.cur_formation_info.partner == self.data.id then
		self.formation_data.cur_formation_info.partner = 0
		self.formation_data:PosChangeReset()
	else
		local is_lock = self.data.vo.is_unlocked == PartnerConfig.LockState.lock
		if is_lock then
			self.formation_data.cur_formation_info.partner = 0
			self.formation_data:PosChangeReset()
			PublicPopupCtrl.Instance:Center(Language.Formation.PartnerNoLockTips)
			return
		end
		self.formation_data.cur_formation_info.partner = self.data.id
		self.formation_data:PartnerAddBegin()
	end
end

-- function FormationViewPartnerItem:OnDownItem()
-- 	self.formation_data.partner_fly = {
-- 		state = FormationConfig.PatnerFlyState.start,
-- 		time = TimeHelper.GetServerTime(),
-- 		partner = self.data.id,
-- 		item = self.ItemObj,
-- 	}
-- end

FormationViewSkillItem = DeclareMono("FormationViewSkillItem", UIWidgetBaseItem)

function FormationViewSkillItem:FormationViewSkillItem()
	self.fight_data = FightCtrl.Instance:Data()
	self.language = Language.Formation
end

function FormationViewSkillItem:SetData(data)
	if nil == data or nil == data.detail then return end
	UIWidgetBaseItem.SetData(self, data)
	local detail = data.detail
	local seq = 1 == self.data.auto_seq and "first_" or "second_"
	local move_type = self.ActFormation and data.move_type or self.fight_data.skill_protocol[seq.. "move_type"]
	self.Name.text = detail.skill_name
	UH.SetIcon(self.IconSSp, detail.icon_id, ICON_TYPE.SKILL)
	local show_pet_icon = PetData.Instance:GetFightIndex() > -1
	self.IconTSp.SpriteName = (show_pet_icon and 2 == self.data.auto_seq) and FightConfig.IconSpNamePet or FightConfig.IconSpNameRole

	self.SkillCorner.text = detail.corner_icon or ""
	self.SkillCornerObj:SetActive(not StringIsEmpty(detail.corner_icon))
	self.AIObj:SetActive(BATTLE_MOVE_TYPE.AI_SKILL == move_type)

	if self.effect_handle ~= nil then
        self.EffectTool:Stop(self.effect_handle)
        self.effect_handle = nil
    end
    
    if VersionTag.HasTag(VersionTag.ClipAnima2D) and not StringIsEmpty(data.detail.asset_id) then
	    self.effect_handle = self.EffectTool:PlayByEffectId(tonumber(data.detail.asset_id), self.EffectParent)
	end
end

function FormationViewSkillItem:SetItemInter(value)
	self.ItemInter.Interactable = value
	self.EffectParent:SetActive(value)
end

function FormationViewSkillItem:OnClickItem()
	if not self.ItemInter.Interactable then
		PublicPopupCtrl.Instance:Center(self.language.SkillTips)
		return
	end
	if self.ActFormation then
		ActFormationData.Instance.cur_auto_seq = self.data.auto_seq
		ViewMgr:OpenView(ActFormationFightSkillView)
	else
		self.fight_data.cur_auto_seq = self.data.auto_seq
		ViewMgr:OpenView(FightSkillView)
	end
end