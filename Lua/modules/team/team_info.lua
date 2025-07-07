
TeamInfo = TeamInfo or BaseClass()

function TeamInfo:__init()
	self:TeamInfo()
	self.switch_f = false
end

function TeamInfo:TeamInfo()
	self.member_list = {}
	self.member_pos_list = {}
	for i = 1, TeamConfig.TEAM_MEMBER_MAX do
		self.member_list[i] = TeamMemberInfo.New(i)
	end
	self.leader = TeamMemberInfo.New()
end

function TeamInfo:TeamVo(info)
	self.info = info

	self:CheckMemberList()
end

function TeamInfo:OutOfTeam()
	self.info = nil
end

function TeamInfo:CheckMemberList()
	for i = 1, TeamConfig.TEAM_MEMBER_MAX do
		self.member_list[i]:EmptyInfo()
	end
	for i = 1, TeamConfig.TEAM_MEMBER_MAX do
		if i <= self.info.member_count then
			local member_info = self.info.member_list[i]
			if 1 == member_info.is_partner then
				local partner_data = PartnerCtrl.Instance:Data()
				local partner = partner_data:GetPartnerInfoById(member_info.uid)
				member_info.partner_info = nil ~= partner and partner.info or {}
			end
			local member_index = nil ~= member_info.member_index and member_info.member_index + 1 or i
			if self.member_list[member_index] then
				self.member_list[member_index]:TeamMemberInfo(member_info, self.info.leader_index == member_info.member_index and 0 == member_info.is_partner)
			end
			if self.info.leader_index == member_info.member_index and  0 == member_info.is_partner then
				self.leader:TeamMemberInfo(member_info)
			end
		end
	end
end

function TeamInfo:CheckMemberPosList()
	self.member_pos_list = {}
	for i = 1, TeamConfig.TEAM_MEMBER_MAX do
		local member_info = self.member_list[i].info
		if nil ~= member_info then
			local pos_index = member_info.pos_index + 1
			self.member_pos_list[pos_index] = self.member_list[i]
		end
	end
end

function TeamInfo:CheckNoTeamPartnerOrder(fight_order_list)
	local role_data = RoleData.Instance
	local role_data_b = role_data:GetBaseData()
	local cur_hp = role_data:GetBattleAttri(BattleAttriType.CurHp)
    local max_hp = role_data:GetBattleAttri(BattleAttriType.MaxHp)
    local cur_mp = role_data:GetBattleAttri(BattleAttriType.CurMp)
	local max_mp = role_data:GetBattleAttri(BattleAttriType.MaxMp)
	local fight_pet = PetData.Instance:GetFightPetInfo()
	local pet_info = {
		pet_id = 0,
		pet_cur_hp = 0,
		pet_max_hp = 0,
		pet_cur_mp = 0,
		pet_max_mp = 0,
		pet_level = 0,
		pet_name = "",
		pet_skin_id = 0,
	}
	if fight_pet then
		local total_attr = PetData.Instance:RealAttr(fight_pet.index) or fight_pet:TotalAttr()
		pet_info.pet_id = fight_pet.pet_id
		pet_info.pet_cur_hp = fight_pet:HP()
		pet_info.pet_max_hp = total_attr.maxhp or total_attr[BattleAttriType.MaxHp]
		pet_info.pet_cur_mp = fight_pet:MP()
		pet_info.pet_max_mp = total_attr.maxmp or total_attr[BattleAttriType.MaxMp]
		pet_info.pet_name = fight_pet:Name()
		pet_info.pet_level = fight_pet:Level()
		pet_info.pet_skin_id = fight_pet:SkinId()
	end
	local info_m = {
		is_partner = 0,
		level = role_data_b.level,
		uid = role_data_b.role_id,
		prof = role_data_b.profession_id,
		name = role_data_b.name,
		appearance = role_data_b.appearance,
		hp_per = cur_hp / max_hp,
		mp_per = cur_mp / max_mp,
		pet_id = pet_info.pet_id,
		pet_cur_hp = pet_info.pet_cur_hp,
		pet_max_hp = pet_info.pet_max_hp,
		pet_cur_mp = pet_info.pet_cur_mp,
		pet_max_mp = pet_info.pet_max_mp,
		pet_level = pet_info.pet_level,
		pet_name = pet_info.pet_name,
		pet_skin_id = pet_info.pet_skin_id,
	}
	self.leader:TeamMemberInfo(info_m)

	self.member_list[1]:TeamMemberInfo(info_m, true)
	
	for i = 2, TeamConfig.TEAM_MEMBER_MAX do
		if i <= #fight_order_list + 1 then
			local partner_id = fight_order_list[i - 1]
			local skin_id = PartnerData.Instance.partner_list[partner_id].vo.skin_id
			self.member_list[i]:TeamMemberInfo({is_partner = 1, uid = fight_order_list[i - 1], prof = 0, pos_index = i, skin_id = skin_id}, false)
		else
			self.member_list[i]:EmptyInfo()
		end
	end
end

function TeamInfo:IsCross(cross_type)
	cross_type = cross_type or TeamConfig.CrossType.is_cross
	return cross_type == self.info.is_cross
end

function TeamInfo:MemberAppearanceChange(member_index, appearance)
	self.member_list[member_index + 1]:AppearanceChange(appearance)
	self.switch_f = not self.switch_f
end

function TeamInfo:GetMemberInfoByMemberIndex(member_index)
	return self.member_list[member_index + 1] or {}
end

function TeamInfo:UpdateMemberHPMP(role_info)
	for _, member in pairs(self.member_list) do
		local info = member.info
		if info then
			local sub_partner = info.sub_partner
			if sub_partner and sub_partner.id == role_info.uid then
				member:CheckShowEffect(role_info, sub_partner)

				sub_partner.cur_hp = role_info.cur_hp
				sub_partner.max_hp = role_info.max_hp
				sub_partner.cur_mp = role_info.cur_mp
				sub_partner.max_mp = role_info.max_mp
				sub_partner.hp_per = role_info.hp_per
				sub_partner.mp_per = role_info.mp_per
			elseif info.uid == role_info.uid and info.is_partner == role_info.is_partner then
				member:CheckShowEffect(role_info, info)

				info.cur_hp = role_info.cur_hp
				info.max_hp = role_info.max_hp
				info.cur_mp = role_info.cur_mp
				info.max_mp = role_info.max_mp
				info.hp_per = role_info.hp_per
				info.mp_per = role_info.mp_per
				info.skin_id = role_info.skin_id
				if 1 == info.is_partner then
					info.icon_id = info.icon_o
					if info.skin_id > 0 then
						local co = Cfg.PartnerSkinById(info.skin_id)
						if co then info.icon_id = co.head_portrait end
					end
					info.level = role_info.level
				end
			end
			self.switch_f = not self.switch_f
		end
	end
end

function TeamInfo:UpdateMemberPetHPMP(pet_info)
	for _, member in pairs(self.member_list) do
		local info = member.info
		if info and info.uid == pet_info.uid then
			member:CheckShowEffectPet(pet_info, info)

			info.pet_id = pet_info.pet_id
			info.pet_level = pet_info.pet_level
			info.pet_str_level = pet_info.pet_str_level
			info.pet_cur_hp = pet_info.pet_cur_hp
			info.pet_max_hp = pet_info.pet_max_hp
			info.pet_cur_mp = pet_info.pet_cur_mp
			info.pet_max_mp = pet_info.pet_max_mp
			info.pet_name = pet_info.pet_id > 0 and PetData.Instance:GetPetCfg(pet_info.pet_id).name or ""
			info.pet_skin_id = pet_info.pet_skin_id
			info.pet_icon_id = PetData.GetItemIconId(info.pet_id, info.pet_skin_id)
			self.switch_f = not self.switch_f
		end
	end
end

function TeamInfo:SetTeamSimpleInfoChangeNotice(info)
	if nil == info then return end
	self.info.limit_level_low = info.limit_level_low
	self.info.limit_level_high = info.limit_level_high
	self.info.team_type = info.team_type
	self.info.check_type = info.check_type
	self.info.is_cross = info.is_cross
	self.switch_f = not self.switch_f
	self.info.allow_red_join = info.allow_red_join
end

function TeamInfo:SetTeamMemberBaseInfoChangeNotice(member_info)
	for _, member in pairs(self.member_list) do
		if member.info and member_info.uid == member.info.uid then
			member.info.level = member_info.level
			member.info.prof = member_info.prof
			member.info.name = member_info.name
			member.info.appearance = member_info.appearance
			member.prof_b = RoleData.ProfIdToProfType(member_info.prof)
			self.switch_f = not self.switch_f
		end
	end
end

function TeamInfo:LeaderInfo()
	return self.leader.info
end

--队伍人数 排除伙伴
function TeamInfo:MemberNum()
	local num = 0
	for key, value in pairs(self.member_list) do
		if value.info and not value:IsPartner() then
			num = num + 1
		end
	end
	return num
end

function TeamInfo:RecruitStr()
	local info = self.info
	return string.format("{recruit;%s;%s;%s;%s;%s;%s}",
		info.team_index, info.team_type,
		info.limit_level_low, info.limit_level_high,
		info.is_cross, TeamData.Instance:MemberNumRole()
	)
end

function TeamInfo:MemberListSort()
	local list = {}
	for _, member in pairs(self.member_list) do
		if member.info and not member:IsPartner() then
			table.insert(list, member)
		end
	end
	table.sort(list, function (a, b)
		if not a or not b then return a end
		return a.info.member_index < b.info.member_index
	end)
	return list
end

function TeamInfo:LimitLevelLow()
	return self.info.limit_level_low
end

function TeamInfo:IsAllowRedJoin()
	if self.info then
		return self.info.allow_red_join == 1
	end
	return true
end