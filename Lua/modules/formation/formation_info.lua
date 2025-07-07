
FormationInfo = FormationInfo or BaseClass()

function FormationInfo:__init(param_t)
    self.seq = param_t.seq
	self.name = param_t.name
	self.formation_info = {}
end

function FormationInfo:SetDetail(detail)
	self.detail = detail
	self:FormationParse()
end

function FormationInfo:FormationParse()
	local detail = self.detail
	if nil == detail then return end
	local formation_info = {}
	local pos_role = 0 == detail.role_row and 3 or 8
	formation_info[pos_role] = FormationInfo.RoleParse(pos_role)
	if detail.pet_idx > -1 then
		local pos_pet = 0 == detail.role_row and 8 or 3
		formation_info[pos_pet] = FormationInfo.PetParse(detail.pet_idx, pos_pet)
	end
	formation_info = (TeamData.Instance:InTeam() and TeamData.Instance:IsLeader()) and self:FormationParseTeam(formation_info, detail) or self:FormationParseSingle(formation_info, detail)
	self.formation_info = formation_info
end

function FormationInfo:FormationParseSingle(formation_info, detail)
	for i = 1, detail.partner_num do
		local partner = detail.partner_list[i]
		formation_info[partner.partner_pos] = FormationInfo.PartnerParse(partner.partner_id, partner.partner_pos)
	end
	return formation_info
end

function FormationInfo:FormationParseTeam(formation_info, detail)
	local member_list = TeamData.Instance:GetMemberList()
	for _, mi in pairs(member_list) do
		local pos_member = FormationConfig.PosT2F[mi:PosIndex()]
		if pos_member and not mi:IsPartner() then
			formation_info[pos_member] = FormationInfo.MemberParse(mi, pos_member)
		end
	end
	for i = 1, detail.partner_num do
		local partner = detail.partner_list[i]
		local p = partner.partner_pos
		local oppo = p > 5 and p - 5 or p + 5
		if nil == formation_info[p] and nil == formation_info[oppo] then
			formation_info[partner.partner_pos] = FormationInfo.PartnerParse(partner.partner_id, partner.partner_pos)
		else
			local info = formation_info[p] or formation_info[oppo]
			if info.member then
				info.sub_partner = {
					partner_id = partner.partner_id,
					pos = partner.partner_pos,
				}
			end
		end
	end
	return formation_info
end

function FormationInfo.RoleParse(pos)
	local crystal_id = BagData.Instance:ViewData().element_crystal_id
	local ele_attr = crystal_id > 0 and EleCrystal.GetElement(crystal_id) or nil
	return {
		role = true,
		pos = pos,
		appearance = RoleData.Instance:GetApearance(),
		name = RoleData.Instance:GetRoleName(),
		ele_attr = ele_attr,
	}
end

function FormationInfo.PetParse(index, pos)
	local pet = PetData.Instance:GetPetInfo(index)
	local element = pet:EleAttr()
	return {
		pet = true,
		pet_index = index,
		pos = pos,
		res_id = pet:ResId(),
		item = pet,
		name = pet:Name(),
		ele_attr = pet:EleAttr(),
		fly_flag = pet:FeiShengTimes(),
	}
end

function FormationInfo.PartnerParse(partner_id, pos)
	local partner = PartnerData.Instance:GetPartnerInfoById(partner_id)
	local skin_id = partner.vo.skin_id
	local res_id = partner.info.res_id
	if skin_id > 0 and PartnerData.Instance.partner_skin_ids[skin_id] then
		res_id = PartnerData.Instance.partner_skin_ids[skin_id].res_id
	end
	return {
		partner = true,
		partner_id = partner_id,
		pos = pos,
		res_id = res_id,
		name = partner.info.name,
		ele_attr = partner.vo.crystal > 0 and EleCrystal.GetElement(partner.vo.crystal) or nil,
		fly_flag = partner.vo.soar_count,
	}
end

function FormationInfo.MemberParse(member, pos)
	local info = member.info
	return {
		member = true,
		pos = pos,
		appearance = info.appearance,
		name = info.name,
		uid = info.uid,
		prof_sp = RoleData.GetProfSp(info.prof),
		member_index = info.member_index,
	}
end

function FormationInfo:FormationIntro()
	local partner_list, member_list, pet_index, pos_role = {}, {}, -1, 3
	for _, info in pairs(self.formation_info) do
		pos_role = info.role and info.pos or pos_role
		pet_index = info.pet and info.pet_index or pet_index
		if info.partner then
			table.insert(partner_list, {
				partner_id = info.partner_id,
				partner_pos = info.pos,
			})
		elseif info.member then
			table.insert(member_list, {
				uid = info.uid,
				member_pos = info.pos,
			})
			if info.sub_partner then
				table.insert(partner_list, {
					partner_id = info.sub_partner.partner_id,
					partner_pos = info.sub_partner.pos,
				})
			end
		end
	end
	local detail = {
		role_row = 3 == pos_role and 0 or 1,
		pet_idx = pet_index,
		partner_list = partner_list,
		member_list = member_list,
		auto_info = self.detail and self.detail.auto_info or {},
	}
	return detail
end

function FormationInfo:GetPetList()
	local is_succ = false
	local pet_list, pet_index, pet_info = {}, -1
	for _, info in pairs(self.formation_info) do
		if info.pet then
			pet_info = info
			pet_index = info.pet_index
		end
	end
	local fight_index = PetData.Instance:GetFightIndex()
	if pet_info then
		if fight_index == pet_index then
			is_succ = true
		else
			PetCtrl.SendPetOp(PET_OP_TYPE.SetPetState, pet_index, PET_STATE_OP.FIGHT)
		end
	else
		if fight_index > -1 and not self.ignore_pet_wait then
			self.ignore_pet_wait = false
			PetCtrl.SendPetOp(PET_OP_TYPE.SetPetState, fight_index, PET_STATE_OP.WAIT1)
		else
			is_succ = true
		end
	end
	if is_succ then 
		for _, pet in pairs(PetData.Instance.pet_list) do
			if PetState.Train ~= pet:State() and pet_index ~= pet.index then
				table.insert(pet_list, {item = pet, pet_index = pet.index})
			end
		end
		local function sort_func(a, b)
			local item_a = a.item
			local item_b = b.item
			local order_a = 100000
			local order_b = 100000
			--排序优先级（出战等状态>评分>等级>品质>强化等级>序号）
			if item_a:State() < item_b:State() then
				order_a = order_a + 100000
			elseif item_a:State() > item_b:State() then
				order_b = order_b + 100000
			end
			if item_a:Quality() > item_b:Quality() then
				order_a = order_a + 100
			elseif item_a:Quality() < item_b:Quality() then
				order_b = order_b + 100
			end
			if item_a:Level() > item_b:Level() then
				order_a = order_a + 1000
			elseif item_a:Level() < item_b:Level() then
				order_b = order_b + 1000
			end
			if item_a:GetScore() > item_b:GetScore() then
				order_a = order_a + 10000
			elseif item_a:GetScore() < item_b:GetScore() then
				order_b = order_b + 10000
			end
			if item_a:StrLevel() > item_b:StrLevel() then
				order_a = order_a + 10
			elseif item_a:StrLevel() < item_b:StrLevel() then
				order_b = order_b + 10
			end
			if item_a.index < item_b.index then
				order_a = order_a + 1
			elseif item_a.index > item_b.index then
				order_b = order_b + 1
			end
			return order_a > order_b
		end
		table.sort(pet_list, sort_func)
		if pet_info then table.insert(pet_list, 1, pet_info) end
	end
	return is_succ, pet_list
end

function FormationInfo:SetNewName(new_name)
    self.name = new_name
end

function FormationInfo:Seq()
    return self.seq
end

function FormationInfo:Name()
	local fn = Language.Formation.FormationName
	return 0 == self:Seq() and fn.cur or (StringIsEmpty(self.name) and string.format(fn.formation, self:Seq()) or self.name)
end

function FormationInfo:Detail()
    return self.detail
end

function FormationInfo:FormationInfo()
    return self.formation_info
end

function FormationInfo:RoleRow()
	return self.role_row
end

function FormationInfo:PetIndex()
	return self.pet_index
end

function FormationInfo:ShowBtnModify()
	return self:Seq() > 0
end

function FormationInfo:PosChange(pos1, pos2)
	local info = self.formation_info[pos1]
	self.formation_info[pos1] = self.formation_info[pos2]
	self.formation_info[pos2] = info
	if self.formation_info[pos1] then self.formation_info[pos1].pos = pos1 end
	if self.formation_info[pos2] then self.formation_info[pos2].pos = pos2 end
	if TeamData.Instance:InTeam() and TeamData.Instance:IsLeader() then
		self:ChangeSubPartner(pos1, pos2)
	end
end

function FormationInfo:ChangeSubPartner(pos1, pos2)
	local to_pos1 = self.formation_info[pos2] and self.formation_info[pos2].sub_partner and self.formation_info[pos2].sub_partner.pos or pos1
	local to_pos2 = self.formation_info[pos1] and self.formation_info[pos1].sub_partner and self.formation_info[pos1].sub_partner.pos or pos2
	self:ChangeSubPartnerPos(pos1, to_pos1)
	self:ChangeSubPartnerPos(pos2, to_pos2)
end

function FormationInfo:ChangeSubPartnerPos(pos, to_pos)
	if self.formation_info[pos] then
		if self.formation_info[pos].partner then
			self.formation_info[pos].pos = to_pos
		end
		if self.formation_info[pos].sub_partner then
			self.formation_info[pos].sub_partner.pos = to_pos
		end
	end
end

function FormationInfo:PosChangeLights(pos)
	local lights = {}
	if 3 == pos or 8 == pos then
		lights = {[3] = true, [8] = true}
	else
		local info_p = self.formation_info[pos]
		if info_p.partner then
			for i = 1, 10 do
				if 3 ~= i and 8 ~= i then
					local oppo = i > 5 and i - 5 or i + 5
					local info_i = self.formation_info[i]
					lights[i] = (nil == self.formation_info[oppo] or (i == pos or oppo == pos)) and (pos <= 5 or not (nil ~= info_i and info_i.member))
				end
			end
		elseif info_p.member then
			for i = 1, 5 do
				if 3 ~= i then
					local oppo = i > 5 and i - 5 or i + 5
					lights[i] = nil == self.formation_info[oppo] or (i == pos or oppo == pos)
				end
			end
		end
	end
	return lights
end

function FormationInfo:PatnerAddToPos(pos, partner)
	if TeamData.Instance:InTeam() and TeamData.Instance:IsLeader() then
		for _, info in pairs(self.formation_info) do
			if info and info.sub_partner and info.sub_partner.partner_id == partner then
				local info_to = self.formation_info[pos]
				info.sub_partner = info_to and info_to.partner and {
					partner_id = info_to.partner_id,
					pos = info.sub_partner.pos,
				} or nil
				break
			end
		end
	end
	self.formation_info[pos] = FormationInfo.PartnerParse(partner, pos)
end

function FormationInfo:PatnerAddLights()
	local lights = {}
	for i = 1, 10 do
		if 3 ~= i and 8 ~= i then
			local oppo = i > 5 and i - 5 or i + 5
			local info_i = self.formation_info[i]
			lights[i] = nil == self.formation_info[oppo] and not (nil ~= info_i and info_i.member)
		end
	end
	return lights
end

function FormationInfo:PetChange(info)
	local pos_pet = 0
	for _, info in pairs(self.formation_info) do
		if info.role then
			pos_pet = 3 == info.pos and 8 or 3
		end
	end
	if pos_pet > 0 then
		self.formation_info[pos_pet] = not info.pet and FormationInfo.PetParse(info.pet_index, pos_pet) or nil
	end
end

function FormationInfo:ExitPartners()
	local exit_partners = {}
	for _, info in pairs(self.formation_info) do
		if info and info.partner then
			exit_partners[info.partner_id] = true
		end
	end
	return exit_partners
end

function FormationInfo:PPRemove(info, state)
	if nil == info then return end
	if info.pos then self.formation_info[info.pos] = nil end
	if info.pet and nil ~= state then
		local p2 = state
		if state == PetState.StandBy then
			p2 = PET_STATE_OP.WAIT1
		end
		if state == PetState.Relax then
			self.ignore_pet_wait = true
			p2 = PET_STATE_OP.RELAX
		end
		PetCtrl.SendPetOp(PET_OP_TYPE.SetPetState, info.pet_index, p2)
	elseif info.member then
		TeamCtrl.Instance:SendKickOutOfTeamReq(info.member_index)
	end
end

function FormationInfo:ShowSkills()
	if nil == self.show_skills then
		local auto_info = self.detail.auto_info
		local base_skills = FightData.Instance.base_skills
		local skill1 = auto_info.first_skill_id > 0 and SkillData.GetDecConfig(auto_info.first_skill_id, auto_info.first_skill_level) or base_skills[auto_info.first_move_type]
		local skill2 = auto_info.second_skill_id > 0 and SkillData.GetDecConfig(auto_info.second_skill_id, auto_info.second_skill_level) or base_skills[auto_info.second_move_type]
		self.show_skills = {}
		table.insert(self.show_skills, {auto_seq = 1, detail = skill1})
		table.insert(self.show_skills, {auto_seq = 2, detail = skill2})
	end
	return self.show_skills
end

function FormationInfo:SetAutoInfo(auto_info)
	if self.detail then
		self.detail.auto_info = auto_info
	end
end