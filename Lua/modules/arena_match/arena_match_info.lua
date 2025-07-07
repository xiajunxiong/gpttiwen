
ArenaMatchInfo = ArenaMatchInfo or BaseClass()

function ArenaMatchInfo:__init(param_t)
	self.info_type = ArenaMatchConfig.InfoType.other
	self.role_info = param_t.role_info
	self.arena_info = param_t.arena_info
	self.per_info = param_t.per_info
	self.arena_data = ArenaMatchCtrl.Instance:Data()
	self.pet_data = PetCtrl.Instance:Data()
	self.partner_data = PartnerCtrl.Instance:Data()

	self.pet_t_list = {}
	self.partner_t_list = {}

	self.pos_c2s_config = ArenaMatchConfig.PosC2S[self:IsFiveArena() and "f" or "t"]
	self.pos_s2c_config = ArenaMatchConfig.PosS2C[self:IsFiveArena() and "f" or "t"]
	
	self.pet_list = SmartData.New()
	self.partner_list = SmartData.New()
	self.pet_num = 0
	self.partner_num = 0
	self.pets_cap = 0
	self.partners_cap = 0

	self.is_own = true
	
	self:CheckRank()
	self:CheckPetPartner()
end

function ArenaMatchInfo:CheckRank()
	if self.role_info.rank then
		self.rank_l = string.format(Language.ArenaMatch.Rank.Rank, self.role_info.rank)
		self.rank_sp = ArenaMatchConfig.RankSp[self.role_info.rank] or ArenaMatchConfig.RankSp.default
	else
		self.rank_l = Language.ArenaMatch.Rank.NoRank
	end
end

function ArenaMatchInfo:CheckPetPartner()
	local pet_list = {}
	local partner_list = {}
	for _, vo in pairs(self.role_info.pet_list) do
		if (vo.index or -1) > -1 or (vo.id or 0) > 0 then
			local pet = self.pet_data:GetPetInfo(vo.index) or (vo.id and Pet.New{id = vo.id, skin_id = vo.skin_id} or nil)
			if pet then
				self.pet_num = self.pet_num + 1
				local pet_cap = vo.cap or 0
				self.pets_cap = self.pets_cap + pet_cap
				vo.pos = ArenaMatchInfo.PosConvert(self.pos_s2c_config, vo.pos)
				pet_list[vo.pos] = ArenaMatchInfo.NewPet(pet, vo)
			end
		end
	end
	for _, vo in pairs(self.role_info.partner_list) do
		if vo.id > 0 then
			self.partner_num = self.partner_num + 1
			local partner_cap =  vo.cap or 0
			self.partners_cap = self.partners_cap + partner_cap
			local partner = self.partner_data:GetPartnerInfoById(vo.id)
			vo.level = self.role_info.level
			vo.pos = ArenaMatchInfo.PosConvert(self.pos_s2c_config, vo.pos)
			vo.skin_id = vo.skin_id or partner:SkinId()
			partner_list[vo.pos] = ArenaMatchInfo.NewPartner(partner.info, vo)
		end
	end
	self.pet_list:Set(pet_list)
	self.partner_list:Set(partner_list)
end

function ArenaMatchInfo:PerInfo()
	return self.per_info
end

function ArenaMatchInfo:InfoType()
	return self.info_type
end

function ArenaMatchInfo:ArenaSeq()
	return self.arena_info.seq
end

function ArenaMatchInfo:ArenaName()
	local seq = self:ArenaSeq()
	local cfg = ArenaMatchData.Instance:GetArenaBySeq(seq)
	return cfg.leitai_name
end

function ArenaMatchInfo:ArenaPosition()
	return self.arena_info.position
end

function ArenaMatchInfo:PetNumMax()
	return self.arena_info.pet_num_max
end

function ArenaMatchInfo:PartnerNumMax()
	return self.arena_info.partner_num_max
end

function ArenaMatchInfo:IsFiveArena()
	return ArenaMatchConfig.PET_NUM_MAX == self:PetNumMax() and ArenaMatchConfig.PARTNER_NUM_MAX == self:PartnerNumMax()
end

function ArenaMatchInfo:Name()
	return self.role_info.name or RoleData.Instance:GetRoleName()
end

function ArenaMatchInfo:Level()
	return self.role_info.level or RoleData.Instance:GetRoleLevel()
end

function ArenaMatchInfo:AvatarType()
	return self.role_info.avatar_type or RoleData.Instance:GetRoleAvatarType()
end

function ArenaMatchInfo:AvatarId()
	return self.role_info.avatar_id or RoleData.Instance:GetRoleAvatarId()
end

function ArenaMatchInfo:AvatarQuality()
	return self.role_info.avatar_quality or RoleData.Instance:GetRoleAvatarQuality()
end

function ArenaMatchInfo:Rank()
	return self.role_info.rank
end

function ArenaMatchInfo:RankL()
	return self.rank_l
end

function ArenaMatchInfo:RankSp()
	return self.rank_sp
end

function ArenaMatchInfo:PetList()
	return self.pet_list:Val()
end

function ArenaMatchInfo:PartnerList()
	return self.partner_list:Val()
end

-- 用于计算我方宠物战力
function ArenaMatchInfo:GetPetListCap()
	local sum = 0
	for _, vo in pairs(self.pet_list) do
		local partner_cap = vo.pet:GetScore() or 0
		sum = sum + partner_cap
	end
	return sum
end

-- 用于计算我方伙伴战力
function ArenaMatchInfo:GetPartnerListCap()
	local sum = 0
	for _, vo in pairs(self.partner_list) do
		local info = PartnerData.Instance:GetPartnerInfoById(vo.partner.partner_id)
		local partner_cap = info.vo.cap or 0
		sum = sum + partner_cap
	end
	return sum
end

function ArenaMatchInfo:PetNum()
	return self.pet_num
end

function ArenaMatchInfo:PartnerNum()
	return self.partner_num
end

function ArenaMatchInfo:PetsCap()
	return self.pets_cap
end

function ArenaMatchInfo:PartnersCap()
	return self.partners_cap
end

function ArenaMatchInfo:IsOwn()
	return self.is_own
end

function ArenaMatchInfo.CreateInfo(param_t)
	param_t = param_t or {}
	param_t.role_info = {
		pet_list = {},
		partner_list = {},
	}
	return param_t
end

function ArenaMatchInfo:CheckFight(to_pos)
	local opp_pos = 1 == to_pos % 2 and to_pos + 1 or to_pos - 1
	return nil == self.pet_list[opp_pos], nil == self.partner_list[opp_pos]
end

function ArenaMatchInfo:CheckPet(pet_index)
	local pos = 0
	for _, v in pairs(self.pet_list) do
		if v.pet.index == pet_index then
			pos = v.vo.pos
		end
	end
	return pos
end

function ArenaMatchInfo:CheckPartner(partner_id)
	local pos = 0
	for _, v in pairs(self.partner_list) do
		if v.partner.partner_id == partner_id then
			pos = v.vo.pos
		end
	end
	return pos
end

function ArenaMatchInfo:PetAdd(data, to_pos)
	local pet_list = DataHelper.TableCopy(self.pet_list:Val())
	self.pet_num = self.pet_num + 1
	local pet = self.pet_data:GetPetInfo(data.pet.index)
	pet_list[to_pos] = ArenaMatchInfo.NewPet(pet, {pos = to_pos})
	self.pet_list:Set(pet_list)
end

function ArenaMatchInfo:PartnerAdd(data, to_pos)
	local partner_list = DataHelper.TableCopy(self.partner_list:Val())
	self.partner_num = self.partner_num + 1
	local partner = self.partner_data:GetPartnerInfoById(data.partner.partner_id)
	partner_list[to_pos] = ArenaMatchInfo.NewPartner(partner.info, {pos = to_pos, skin_id = partner:SkinId(), intensify_level = partner.vo.intensify_level,})
	self.partner_list:Set(partner_list)
end

function ArenaMatchInfo:RemoveByPos(pos)
	if nil ~= self.pet_list[pos] then
		self.pet_num = self.pet_num - 1
		self.pet_list[pos] = nil
	end
	if nil ~= self.partner_list[pos] then
		self.partner_num = self.partner_num - 1
		self.partner_list[pos] = nil
	end
end

function ArenaMatchInfo:ClearAll()
	self.pet_list:Set({})
	self.partner_list:Set({})
	self.pet_num = 0
	self.partner_num = 0
end

function ArenaMatchInfo.NewPet(pet, vo)
	return {
		pet = pet,
		vo = vo or {},
		pet_exclusive_skill_disable_flag = bit:d8b(0),
		lian_yao_exclusive_skill_disable_flag = bit:d8b(0),
	}
end

function ArenaMatchInfo.NewPartner(partner, vo)
	local icon_id = partner.icon_id
	if vo.skin_id > 0 then
		local co = Cfg.PartnerSkinById(vo.skin_id)
		if co then icon_id = co.head_portrait end
	end
	local t = {
		partner = partner,
		vo = vo or {},
		icon_id = icon_id,
	}
	t.vo.str_level = vo.intensify_level or 1
	return t
end

function ArenaMatchInfo.PosConvert(config, pos)
	return config[pos]
end

function ArenaMatchInfo:ArenaMatchShow()
	local sum = self:IsFiveArena() and 10 or 6
	if self.partner_num > 0 then
		for i = 1, sum do
			if self.partner_list[i] then
				local partner = self.partner_list[i]
				local skin_id = partner.vo.skin_id
				local res_id = partner.partner.res_id
				if skin_id > 0 and PartnerData.Instance.partner_skin_ids[skin_id] then
					res_id = PartnerData.Instance.partner_skin_ids[skin_id].res_id
				end
				return res_id
			end
		end
	else
		for i = 1, sum do
			if self.pet_list[i] then
				return self.pet_list[i].pet:ResId()
			end
		end
	end
end

ArenaMatchRoleInfo = ArenaMatchRoleInfo or BaseClass(ArenaMatchInfo)

function ArenaMatchRoleInfo:__init(param_t)
	self.info_type = ArenaMatchConfig.InfoType.own

	self:CheckReward()
	self:CheckRankLevel()
end

function ArenaMatchRoleInfo:CheckReward()
	self.reward_info = self.arena_data:GetRewardByIdRank(self.arena_info.reward_id, self:Rank())
end

function ArenaMatchRoleInfo:CheckRankLevel()
	local rank = self:Rank()
	local rank_level_func = function (position, rank)
		local rank_level = 1
		rank_level = rank < 2 and 1 * rank_level or (rank < 11 and 10 * rank_level or 100 * rank_level)
		rank_level = 0 == position and rank_level or rank_level + 1
		return rank_level
	end
	self.rank_level = rank_level_func(self.arena_info.position, self:Rank())
end

function ArenaMatchRoleInfo:Rewards()
	return self.reward_info.rewards
end

function ArenaMatchRoleInfo:BackupPP()
	self.pet_backup = DataHelper.TableCopy(self.pet_list:Val())
	self.partner_backup = DataHelper.TableCopy(self.partner_list:Val())
	self.pet_t_list = {}
	self.partner_t_list = {}
end

function ArenaMatchRoleInfo:RestorePP()
	self.pet_list:Set(self.pet_backup)
	self.partner_list:Set(self.partner_backup)
	self.pet_t_list = {}
	self.partner_t_list = {}
end

function ArenaMatchRoleInfo:RankLevel()
	return self.rank_level
end

function ArenaMatchRoleInfo:PetAdd(data, to_pos)
	ArenaMatchInfo.PetAdd(self, data, to_pos)
	self.pet_t_list[data] = nil
end

function ArenaMatchRoleInfo:PartnerAdd(data, to_pos)
	ArenaMatchInfo.PartnerAdd(self, data, to_pos)
	self.partner_t_list[data] = nil
end

function ArenaMatchRoleInfo:RemoveByPos(pos)
	local pet_data = self.pet_list[pos]
	if nil ~= pet_data then
		self.pet_num = self.pet_num - 1
		if pet_data.pet:ArenaState() then self.pet_t_list[pet_data] = pet_data end
		self.pet_list[pos] = nil
	end
	local partner_data = self.partner_list[pos]
	if nil ~= self.partner_list[pos] then
		self.partner_num = self.partner_num - 1
		local partner = self.partner_data:GetPartnerInfoById(partner_data.partner.partner_id)
		if 1 == partner.vo.is_arena_match then self.partner_t_list[partner_data] = partner_data end
		self.partner_list[pos] = nil
	end
end

ArenaMatchRankInfo = ArenaMatchRankInfo or BaseClass(ArenaMatchInfo)

function ArenaMatchRankInfo:__init(param_t)
	self:CheckIsOwn()
end

function ArenaMatchRankInfo:CheckIsOwn()
	self.is_own = self.role_info.uid == RoleData.Instance:GetRoleId()
end

ArenaMatchReadyInfo = ArenaMatchReadyInfo or BaseClass(ArenaMatchInfo)

function ArenaMatchReadyInfo:__init(param_t)
	self.info_type = ArenaMatchConfig.InfoType.ready
end

function ArenaMatchReadyInfo:ArenaRank()
	return self.per_info:Rank()
end

function ArenaMatchReadyInfo:ArenaSeq()
	return self.per_info:ArenaSeq()
end

function ArenaMatchReadyInfo:PetNumMax()
	return self.per_info:PetNumMax()
end

function ArenaMatchReadyInfo:PartnerNumMax()
	return self.per_info:PartnerNumMax()
end