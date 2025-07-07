GhostBustData = GhostBustData or BaseClass()

GhostBustData.BG_Cofig = {
	[0] = {left = 1,right = 1},
	[1] = {left = 0.1,right = 1},
	[2] = {left = 0,right = 0.9},
	[3] = {left = 0,right = 0.75},
	[4] = {left = 0,right = 0.6},
	[5] = {left = 0,right = 0},
}
GhostBustData.GBMask = {
	[0] = {fill = 1},
	[1] = {fill = 0.788},
	[2] = {fill = 0.74},
	[3] = {fill = 0.671},
	[4] = {fill = 0.58},
	[5] = {fill = 0.515},
	[6] = {fill = 0.454},
	[7] = {fill = 0.368},
	[8] = {fill = 0.299},
	[9] = {fill = 0.243},
	[10] = {fill = 0},
}

GhostBustData.GBPage = {
	[0] = 1963,
	[1] = 620,
	[2] = 620,
	[3] = -742,
	[99] = -755,
}

function GhostBustData:__init()
	if GhostBustData.Instance ~= nil then
		Debuger.LogError("[GhostBustData] attempt to create singleton twice!")
		return
	end
	GhostBustData.Instance = self

	self.ghost_info = SmartData.New({pass_seq = -1,flush = false})
	self.reward_info = SmartData.New({flush = false})

	self.view_data = SmartData.New({play = false})

	self.mark_pass = -1

	self.wait_viedo = false
	self:InitConfig()

	self.reward_data_list = {}
    self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,
    BindTool.Bind(self.OnRoleNoticeComplete, self))
end 

function GhostBustData:InitConfig()
	-- 组列表
	self.part_list = {}
	local index = 1
	for k,v in pairs(Config.pet_tried_auto.customs_pass) do 
		if self.part_list[index] == nil then self.part_list[index] = {} end 

		table.insert( self.part_list[index],v )

		if #self.part_list[index] == 5 and k < #Config.pet_tried_auto.customs_pass then 
			index = index + 1
		end 
	end 

	self.max_index = index
end

-- 检索合计十个目标
-- 当出现置顶位置时需要置空哦
function GhostBustData:CheckSeqPartNeo()
	local next = self:GetNextData()
	local need_plus = math.floor((next == nil and self.ghost_info.pass_seq or next.seq) % 5) > 0
	local index = math.floor((next == nil and self.ghost_info.pass_seq or next.seq) / 5) + (need_plus and 1 or 0)
	

	local list_1 = self.part_list[index]

	local result = {}
	if index == 1 then 
		local list_2 = self.part_list[index+1]

		for k,v in pairs(list_1) do 
			table.insert(result,v)
		end 

		for k,v in pairs(list_2) do
			table.insert(result,v)
		end 
	elseif self.ghost_info.pass_seq == 0 then 
		list_1 = self.part_list[1]
		local list_2 = self.part_list[2]

		for k,v in pairs(list_1) do 
			table.insert(result,v)
		end 

		for k,v in pairs(list_2) do 
			table.insert(result,v)
		end 
	elseif index == #self.part_list then 
		local list_2 = self.part_list[index-1]

		for i = 4,5 do 
			table.insert(result,list_2[i])
		end 

		for k,v in pairs(list_1) do 
			table.insert(result,v)
		end 

		for i = 1,3 do 
			table.insert(result,{empty = true})
		end 
	else
		local list_2 = self.part_list[index-1]
		local list_3 = self.part_list[index+1]

		for i = 4,5 do 
			table.insert(result,list_2[i])
		end 

		for k,v in pairs(list_1) do 
			table.insert(result,v)
		end 

		for i = 1,3 do 
			table.insert(result,list_3[i])
		end 
	end 

	local mark = 1
	local mark_seq = self.ghost_info.pass_seq > 0 and self.ghost_info.pass_seq or 1
	for k,v in pairs(result) do
		if v.seq == mark_seq then 
			mark = k
		end 
	end 

	-- 无动画且选中mark
	local lock = self.ghost_info.pass_seq == 0 or self.ghost_info.pass_seq == #Config.pet_tried_auto.customs_pass 
	
	return result,mark,lock
end

function GhostBustData:CheckSeqPart(seq)
	-- 组号
	local index = math.floor(seq / 5)
	-- 位置
	local pos = math.floor(seq % 5)

	if pos == 0 then -- 位置为0则说明处于整除位置
		return self.part_list[index] , {index,5}
	else  -- 相反位置不为0说明未被整除
		return self.part_list[index + 1] , {index+1,pos}
	end 
end

function GhostBustData:SetGhostInfo(protocol)
	if self.ghost_info.pass_seq ~= protocol.pass_seq then 
		self.ghost_info.flush = not self.ghost_info.flush

		if self.mark_pass == -1 then 
			self.mark_pass = protocol.pass_seq
		end 
	end 
	self.ghost_info.pass_seq = protocol.pass_seq

	if self.reward_info.m_reward_bit ~= protocol.m_reward_bit or 
	   self.reward_info.m_reward_bit2 ~= protocol.m_reward_bit2 then 
		self.reward_info.flush = not self.reward_info.flush
	end 
	self.reward_info.m_reward_bit  = protocol.m_reward_bit
	self.reward_info.m_reward_bit2  = protocol.m_reward_bit2

	self.reward_info.cmd_mark = protocol.cmd
end

function GhostBustData:GetGhostShow(offset)
	local challenge = self.ghost_info.pass_seq + 1
	local part_list,param = self:CheckSeqPart(challenge)

	if offset == nil then 
		-- LogError("?!@reurn1")
		return part_list,param 
	end 

	if offset <= 0 then 
		-- LogError("?!@reurn2")
		return self.part_list[1] ,{1,1}
	elseif offset > self.max_index then  
		-- LogError("?!@reurn3")
		return self.part_list[self.max_index],{self.max_index}
	else 
		-- LogError("?!@reurn4")
		return self.part_list[offset],{offset,5}
	end 
end

function GhostBustData:GetMaxIndex()
	return self.max_index
end

function GhostBustData:GetCellIsPass(seq)
	return seq <= self.ghost_info.pass_seq
end

function GhostBustData:GetCellIsChallenging(seq)
	return seq == self.ghost_info.pass_seq + 1
end

function GhostBustData:GetCellIsLock(seq)
	local role_level = RoleData.Instance:GetRoleLevel()
	local cfg = Config.pet_tried_auto.customs_pass[seq]
	return (seq > self.ghost_info.pass_seq + 1) or cfg.level_limit > role_level
end

function GhostBustData:CanChallenge(seq) 
	if self:GetCellIsPass(seq) then 
		PublicPopupCtrl.Instance:Center(Language.GhostBust.MonsterPass)
		return true
	end 

	if self:GetCellIsLock(seq) then 
		local role_level = RoleData.Instance:GetRoleLevel()
		local cfg = Config.pet_tried_auto.customs_pass[seq]
		if cfg.level_limit > role_level then 
			PublicPopupCtrl.Instance:Center(string.format(Language.GhostBust.LimitLevel,cfg.level_limit))
		else 
			PublicPopupCtrl.Instance:Center(Language.GhostBust.MonsterLocking)
		end 

		return true
	end

	return false
end 

function GhostBustData:GetAchieveList(index)
	local operate_list = index == 1 and Config.pet_tried_auto.achievement1 or Config.pet_tried_auto.achievement2

	local result_list = {}

	for k,v in pairs(operate_list) do 
		local vo = v
		vo.type = index

		table.insert(result_list, vo)
	end 

	local mark_data = nil
	local info_operate = self.reward_info["m_reward_bit"..(index == 2 and 2 or "")]
	for i = 1,#operate_list do 
		if info_operate[i] == 0 then 
			mark_data = result_list[i]
			break
		end 
	end 

	return result_list,mark_data
end

function GhostBustData:IsUnLock()
	-- local num = 0
	-- for k,v in pairs(self.reward_info.m_reward_bit) do 
	-- 	if v > 0 then 
	-- 		num = num +1
	-- 	end 

	-- 	if num >= 4 then 
	-- 		return true 
	-- 	end 
	-- end 

	return self.reward_info.cmd_mark[1] == 1
end

function GhostBustData:GetAchieveStatus(param)
	local catched = self.reward_info["m_reward_bit"..(param.index == 2 and 2 or "")][param.seq] == 1

	-- LogError("?!",self.reward_info)
	local achieve_1 = false
	local achieve_2 = false
	local cfg = Config.pet_tried_auto["achievement"..param.index][param.seq]	

	local is_pass = cfg.num <= self.ghost_info.pass_seq

	if cfg.achievement_2 == nil then 
		if cfg.achievement == 0 then 
			achieve_1 = true
		elseif cfg.achievement == 1 then  
			achieve_1 = true
		elseif cfg.achievement == 2 then  
			local role_level = RoleData.Instance:GetRoleLevel()
			achieve_1 = role_level > cfg.param1
		elseif cfg.achievement == 3 then  
			local role_score = RoleData.Instance:GetRoleScore()
			achieve_1 = role_score > cfg.param1
		end 
		achieve_2 = true
	else
		achieve_1 = true
		for i = 1,cfg.achievement do 
			achieve_1 = achieve_1 and self.reward_info.m_reward_bit[i] == 1
		end 

		achieve_2 = self.reward_info.cmd_mark[1] == 1
		
		-- if cfg.achievement == 0 then 
		-- 	achieve_1 = true
		-- elseif cfg.achievement == 1 then  
		-- 	achieve_1 = true
		-- elseif cfg.achievement == 2 then  
		-- 	local role_level = RoleData.Instance:GetRoleLevel()
		-- 	achieve_1 = role_level > cfg.param1
		-- elseif cfg.achievement == 3 then  
		-- 	local role_score = RoleData.Instance:GetRoleScore()
		-- 	achieve_1 = role_score > cfg.param1
		-- elseif cfg.achievement == 40 then  
		-- 	achieve_1 = self:IsUnLock()
		-- end 

		-- if cfg.achievement_2 == nil then 
		-- 	achieve_2 = true
		-- elseif cfg.achievement_2 == 0 then  
		-- 	achieve_2 = true
		-- elseif cfg.achievement_2 == 1 then  
		-- 	achieve_2 = true
		-- elseif cfg.achievement_2 == 2 then  
		-- 	local role_level = RoleData.Instance:GetRoleLevel()
		-- 	achieve_2 = role_level > cfg.param1
		-- elseif cfg.achievement_2 == 3 then  
		-- 	local role_score = RoleData.Instance:GetRoleScore()
		-- 	achieve_2 = role_score > cfg.param1
		-- elseif cfg.achievement_2 == 4 then 
		-- 	achieve_2 = self.reward_info.cmd_mark[1] == 1
		-- end 
	end 

	-- if cfg.achievement == 0 then 
	-- 	achieve_1 = true
	-- elseif cfg.achievement == 1 then  
	-- 	achieve_1 = true
	-- elseif cfg.achievement == 2 then  
	-- 	local role_level = RoleData.Instance:GetRoleLevel()
	-- 	achieve_1 = role_level > cfg.param1
	-- elseif cfg.achievement == 3 then  
	-- 	local role_score = RoleData.Instance:GetRoleScore()
	-- 	achieve_1 = role_score > cfg.param1
	-- elseif cfg.achievement == 40 then  
	-- 	achieve_1 = self:IsUnLock()
	-- end 

	-- if cfg.achievement_2 == nil then 
	-- 	achieve_2 = true
	-- elseif cfg.achievement_2 == 0 then  
	-- 	achieve_2 = true
	-- elseif cfg.achievement_2 == 1 then  
	-- 	achieve_2 = true
	-- elseif cfg.achievement_2 == 2 then  
	-- 	local role_level = RoleData.Instance:GetRoleLevel()
	-- 	achieve_2 = role_level > cfg.param1
	-- elseif cfg.achievement_2 == 3 then  
	-- 	local role_score = RoleData.Instance:GetRoleScore()
	-- 	achieve_2 = role_score > cfg.param1
	-- elseif cfg.achievement_2 == 4 then 
	-- 	achieve_2 = self.reward_info.cmd_mark[1] == 1
	-- end 

	return {
		is_catched = catched,
		is_pass = is_pass,
		achieve_1 = achieve_1,
		achieve_2 = achieve_2,
	}
end

function GhostBustData:GetBGGray(list)
	local lock_num = 0

	for k,v in pairs(list) do 
		if not GhostBustData.Instance:GetCellIsLock(v.seq) then 
			lock_num = lock_num + 1
		end 
	end 

	return GhostBustData.BG_Cofig[lock_num]
end

function GhostBustData:FlushRolePlay()
	self.view_data.play = not self.view_data.play
end

function GhostBustData:OnRoleNoticeComplete()
	GhostBustCtrl.Instance:SendGhostBustSeq(0)
end

function GhostBustData:GetFormationScore()
	if not ActFormationData.Instance:SetCurFormationInfo(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_YAO_XING_SHI_LIAN) then 
		return 0
	end 
    local list = ActFormationData.Instance:GetFormationInfo()

	-- LogError(list)
	local score = 0
	for k,v in pairs(list.grid_list) do 
		if  v.id > 0 and v.type > 0 then 
			if v.type == ActFormationData.FormationType.MainRole then 
				score = score + RoleData.Instance:GetRoleScore()
			elseif v.type == ActFormationData.FormationType.Pet then 
				local pet_index = PetData.Instance:GetPetIndexByUniqueId(v.unique_id_1, v.unique_id_2)
				local pet = PetData.Instance:GetPetInfo(pet_index)

				score = score + ((pet ~= nil and type(pet.GetScore) == "function") and pet:GetScore() or 0)
			elseif v.type == ActFormationData.FormationType.Partner then 
				local partner = PartnerData.Instance:GetPartnerInfoById(v.id)

				local cap,cap_level = partner:GetScore()
				score = score + cap
			end 
		end 
	end 

	return score
end

function GhostBustData.IsGhostBust()
	return BATTLE_MODE.BATTLE_MODE_YAO_XING_SHI_LIAN == FinishData.Instance:GetBattleMode()
end

function GhostBustData:GetNextData()
	local cfg = Config.pet_tried_auto.customs_pass[self.ghost_info.pass_seq + 1]
	return cfg
end

function GhostBustData:GetFinishRewardData()
	local list = BagData.Instance:ItemListBuild( Config.pet_tried_auto.customs_pass[self.ghost_info.pass_seq].clear_reward)

	self:SetRewardDataList(list,string.format(Language.GhostBust.Chapter,self.ghost_info.pass_seq))
	return list
end

function GhostBustData:GetAchieveRed()
	for i = 1,2 do 
		local list = self:GetAchieveList(i)
		for k,v in pairs(list) do 
			local param = GhostBustData.Instance:GetAchieveStatus({seq = v.seq,index = v.type})
			local can_get = not param.is_catched and param.is_pass and param.achieve_1 and param.achieve_2

			if can_get then 
				return true
			end 
		end 
	end 

	return false
end

function GhostBustData:WaitVideo(flag)
	self.wait_viedo = flag
end

-- 直购区都需要先领取前一个40次，直接取第一个配置
function GhostBustData:CheckCanBuy()
	local cfg = Config.pet_tried_auto.achievement2[1]
	local flag = true
	for i = 1,cfg.achievement do 
		flag = flag and self.reward_info.m_reward_bit[i] == 1
	end 

	return flag,cfg.achievement
end

function GhostBustData:SetRewardDataList(reward_list,title_text)

	if #reward_list ~= 0 then

		for k,v in pairs(self.reward_data_list) do 
			if title_text == v.title_text then 
				return 
			end 
		end 

		table.insert(self.reward_data_list,{
			list = DataHelper.FormatItemList(reward_list),
			title_text = title_text
		})
	end
end

function GhostBustData:ResetRewardData()
	self.reward_data_list = {}
end

function GhostBustData:GetRewardDataList()
	return self.reward_data_list or {}
end