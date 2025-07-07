
TeamMemberInfo = TeamMemberInfo or BaseClass()

function TeamMemberInfo:__init(index)
	self.index = index
	self.is_leader = false
	self.is_self = false
	self.show_effect = false
	self.show_effect_pet = false
end

function TeamMemberInfo:TeamMemberInfo(info, is_leader)
	self.info = info
	self.is_leader = is_leader
	self.prof_b = RoleData.ProfIdToProfType(info.prof)
	self:CheckPartner()
	self:CheckSubPartner()
	self.is_self = info.uid == RoleData.Instance:GetRoleId()
end

function TeamMemberInfo:EmptyInfo()
	self.info = nil
	self.is_leader = false
	self.show_effect = false
	self.show_effect_pet = false
end

function TeamMemberInfo:AppearanceChange(appearance)
	self.info.appearance = appearance
end

function TeamMemberInfo:CheckPartner()
	if 1 == self.info.is_partner then
		local partner_data = PartnerCtrl.Instance:Data()
		local partner = partner_data:GetPartnerInfoById(self.info.uid)
		self.info.partner_info = partner.info
		self.info.name =  nil ~= partner and partner.info.name or ""
		self.info.partner_id = nil ~= partner and partner.info.partner_id or 0
		self.info.hp_per, self.info.mp_per = partner:HPPMPP()
		self.info.icon_o = partner.info.icon_id
		self.info.icon_id = partner.info.icon_id
		if self.info.skin_id > 0 then
			local co = Cfg.PartnerSkinById(self.info.skin_id)
			if co then self.info.icon_id = co.head_portrait end
		end
	elseif self.info.pet_id then
		self.info.pet_name = (self.info.pet_id > 0 and PetData.Instance:GetPetCfg(self.info.pet_id)) and PetData.Instance:GetPetCfg(self.info.pet_id).name or ""
		self.info.pet_icon_id = PetData.GetItemIconId(self.info.pet_id, self.info.pet_skin_id)
	end
end

function TeamMemberInfo:CheckSubPartner()
	local sub_partner = self.info.sub_partner
	if sub_partner and sub_partner.id > 0 then
		local partner_data = PartnerCtrl.Instance:Data()
		local partner = partner_data:GetPartnerInfoById(sub_partner.id)
		sub_partner.partner_info = partner.info
		sub_partner.name =  nil ~= partner and partner.info.name or ""
		sub_partner.partner_id = nil ~= partner and partner.info.partner_id or 0
		sub_partner.icon_id = partner.info.icon_id
		sub_partner.job = partner.info.job
	end
end

function TeamMemberInfo:IsPartner()
	return (self.info and self.info.is_partner == 1)
end

function TeamMemberInfo:IsSelf()
	return self.is_self
end

function TeamMemberInfo:IsFollow()
	return (self.info and self.info.is_follow == 1)
end

function TeamMemberInfo:IsOnline()
	return (self.info and self.info.is_online == 1)
end

function TeamMemberInfo:IsLeader()
	return self.is_leader
end


function TeamMemberInfo:CheckShowEffect(cur_info, per_info)
	self.show_effect = (nil ~= per_info and nil ~= cur_info) and ((cur_info.cur_hp or 0) > (per_info.cur_hp or 0) or (cur_info.cur_mp or 0) > (per_info.cur_mp or 0))
end
	
function TeamMemberInfo:CheckShowEffectPet(cur_info, per_info)
	self.show_effect_pet = (nil ~= per_info and nil ~= cur_info) and ((cur_info.pet_cur_hp or 0) > (per_info.pet_cur_hp or 0) or (cur_info.pet_cur_mp or 0) > (per_info.pet_cur_mp or 0))
end

function TeamMemberInfo:ShowEffect(obj_t0, obj_t1)
	local value = false
	if (obj_t0 and self.show_effect) or (obj_t1 and self.show_effect_pet) then
		value = true
	end
	self.show_effect = false
	self.show_effect_pet = false
	return value
end

function TeamMemberInfo:PosIndex()
	return self.info.pos_index or -1
end

function TeamMemberInfo:PartnerId()
	return self.info.partner_id
end