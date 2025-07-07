
FormationData = FormationData or BaseClass()

function FormationData:__init()
	if FormationData.Instance ~= nil then
		Debuger.LogError("[FormationData] attempt to create singleton twice!")
		return
    end
    FormationData.Instance = self

	self:FormationConfig()
	self:FormationData()
end

function FormationData:FormationConfig()
end

function FormationData:FormationData()
	self.formation_list = {}
	self.cur_formation_info = SmartData.New{
		sel_seq = 0,
		list_change = false,
		formation_change = {
			seq = 0,
		},
		pos_change = {
			arrow = 0,
			lights = {}
		},
		partner = 0,
	}
	self.partner_fly = {
		state = FormationConfig.PatnerFlyState.finish,
		time = 0,
		data = nil,
		item = nil
	}
	self.top_layer_formation = SmartData.New{
		is_show = false,
		partner = 0,
		pet = 0,
	}
end

function FormationData:SetFormationSimpleInfo(protocol)
	local info = protocol.info
	local formation_list = {}
	for i = 1, info.active_formation_num do
		local formation = info.formation_list[i]
		formation_list[formation.seq] = SmartData.New(FormationInfo.New(formation))
	end
	self.formation_list = formation_list
	self.cur_formation_info.list_change = not self.cur_formation_info.list_change
end

function FormationData:SetFormationDetailInfo(protocol)
	local info = protocol.info
	if self.formation_list[info.formation_seq] then
		self.formation_list[info.formation_seq]:SetDetail(info)
		self.cur_formation_info.formation_change = {seq = info.formation_seq}
	end
end

function FormationData:SetFormationRename(protocol)
	local info = protocol.info
	if self.formation_list[info.formation_seq] then
		self.formation_list[info.formation_seq]:SetNewName(info.new_name)
	end
end

function FormationData:GetFormationShowList()
	local formation_list = {}
	local cur_sel_formation = self.formation_list[0]
	for _, formation in pairs(self.formation_list) do
		table.insert(formation_list, formation)
		cur_sel_formation = formation:Seq() == self.cur_formation_info.sel_seq and formation or cur_sel_formation
	end
	table.sort(formation_list, function (a, b)
		return a:Seq() < b:Seq()
	end)
	return formation_list, cur_sel_formation
end

function FormationData:CurSelSeq(seq, selected)
	local value = false
	if selected and self:IsTeamFormation() then
		if seq > 0 then
			PublicPopupCtrl.Instance:Center(Language.Formation.TeamSelTips)
			self.cur_formation_info.sel_seq = -1
			self.cur_formation_info.sel_seq = 0
		end
		self:PosChangeReset()
		return 0 == seq
	end
	if selected then
		local sel_seq = self.cur_formation_info.sel_seq
		local sel_formation = self.formation_list[sel_seq]
		if nil == sel_formation then
			self.cur_formation_info.sel_seq = 0
			return value
		end
		self:PosChangeReset()
		sel_formation:FormationParse()
		self.cur_formation_info.sel_seq = seq
	elseif seq == self.cur_formation_info.sel_seq then
		value = true
	end
	return value
end

function FormationData:PosChange(pos)
	local pos_change = self.cur_formation_info.pos_change
	local sel_seq = self.cur_formation_info.sel_seq
	local sel_formation = self.formation_list[sel_seq]
	if 0 == pos_change.arrow then
		local formation_info = sel_formation.formation_info
		if formation_info[pos] then
			local lights = sel_formation:PosChangeLights(pos)
			self.cur_formation_info.pos_change = {
				arrow = pos,
				lights = lights,
			}
		elseif 3 == pos or 8 == pos then
			ViewMgr:OpenView(FormationPetView)
		else
			PublicPopupCtrl.Instance:Center(Language.Formation.SelNoPartnerTips)
		end
	elseif pos_change.arrow ~= pos then
		if pos_change.lights[pos] then
			sel_formation:PosChange(pos_change.arrow, pos)
			self:PosChangeReset()
			self.cur_formation_info.formation_change = {seq = self.cur_formation_info.sel_seq}
			self:AutoSaveFormation0()
		else
			PublicPopupCtrl.Instance:Center(Language.Formation.SelNoLightTips)
		end
	else
		self:PosChangeReset()
	end
end

function FormationData:PartnerAddBegin()
	local sel_seq = self.cur_formation_info.sel_seq
	local sel_formation = self.formation_list[sel_seq]
	local lights = sel_formation:PatnerAddLights()
	self.cur_formation_info.pos_change = {
		arrow = 0,
		lights = lights,
	}
end

function FormationData:PartnerAddEnd(pos, partner)
	local sel_seq = self.cur_formation_info.sel_seq
	local sel_formation = self.formation_list[sel_seq]
	local pos_change = self.cur_formation_info.pos_change
	if pos_change.lights[pos] then
		self.cur_formation_info.partner = 0
		sel_formation:PatnerAddToPos(pos, partner)
		self:PosChangeReset()
		self.cur_formation_info.formation_change = {seq = self.cur_formation_info.sel_seq}
		self:AutoSaveFormation0()
	else
		PublicPopupCtrl.Instance:Center(Language.Formation.SelNoLightTips)
	end
end

function FormationData:PosChangeReset()
	self.cur_formation_info.pos_change = {
		arrow = 0,
		lights = {},
	}
	self.cur_formation_info.partner = 0
end

function FormationData:PetChange(info)
	local sel_seq = self.cur_formation_info.sel_seq
	local sel_formation = self.formation_list[sel_seq]
	sel_formation:PetChange(info)
	self.cur_formation_info.formation_change = {seq = self.cur_formation_info.sel_seq}
end

function FormationData:GetPartnerList()
	local function sort_lock(a,b)
		local av = a.vo
		local bv = b.vo
		if av.is_unlocked == bv.is_unlocked then
			if av.fight_state == bv.fight_state then
				local arp = a.acti_red_point.val and 1 or 0
				local brp = b.acti_red_point.val and 1 or 0
				if arp == brp then
					if a.partner_quality == b.partner_quality then
						return a.id < b.id
					end
					return a.partner_quality > b.partner_quality
				end
				return arp > brp
			end
			return av.fight_state > bv.fight_state
		end
		return av.is_unlocked > bv.is_unlocked
	end
	local list = {}
	local sel_seq = self.cur_formation_info.sel_seq
	local sel_formation = self.formation_list[sel_seq]
	local exit_partners = sel_formation:ExitPartners()
	for _, info in pairs(PartnerData.Instance.partner_list) do
		if not exit_partners[info.id] then
			table.insert(list, info)
		end
	end
	table.sort(list, sort_lock)
	return list
end

function FormationData:GetPartnerClickId()
	local id = self.partner_click_id
	self.partner_click_id = nil
	return id
end

function FormationData:PPRemove(info, state)
	local sel_seq = self.cur_formation_info.sel_seq
	local sel_formation = self.formation_list[sel_seq]
	sel_formation:PPRemove(info, state)
	self:PosChangeReset()
	self.cur_formation_info.formation_change = {seq = self.cur_formation_info.sel_seq}
	if info.partner then self:AutoSaveFormation0() end
end

function FormationData:FlushFormation0()
	if nil == self.formation_list[0] then return end
	self.formation_list[0]:FormationParse()
	self:PosChangeReset()
	self.cur_formation_info.formation_change = {seq = 0}
end

function FormationData:IsTeamFormation()
	return TeamData.Instance:InTeam() and TeamData.Instance:IsLeader()
end

function FormationData:AutoSaveFormation0()
	if self:IsTeamFormation() then
		FormationCtrl.Instance:SendFormationSaveComplete(self.formation_list[0])
	end
end

function FormationData:ResetTopLayerFormation()
	self.top_layer_formation:Set{
		is_show = false,
		partner = 0,
		pet = 0,
	}
end

function FormationData:TopLayerShowPartner(partner_id)
	if RoleData.Instance:GetRoleLevel() > GuideData.Instance:GetFormationPiaoChuangLevel() then return end
	if TreasureHuntData.Instance.smart_data.is_guide_draw then
		self.top_layer_formation.is_show = false
	else
		self.top_layer_formation.is_show = true
	end
	self.top_layer_formation.partner = partner_id
end

function FormationData:TopLayerShowPet(pet_id)
	if RoleData.Instance:GetRoleLevel() > GuideData.Instance:GetFormationPiaoChuangLevel() then return end
	self.top_layer_formation.is_show = true
	self.top_layer_formation.pet = pet_id
end

function FormationData:SetFormation0AutoInfo(auto_info)
	if self.formation_list[0] then
		self.formation_list[0]:SetAutoInfo(auto_info)
	end
end
