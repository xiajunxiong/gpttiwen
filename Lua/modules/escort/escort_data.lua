
EscortData = EscortData or BaseClass()

function EscortData:__init()
	self:EscortConfig()
	self:EscortData()
end

function EscortData:__delete()
end

function EscortData:EscortConfig()
	self.config = Config.yunbiao_auto
	self.config_yunbiao_config = self.config.yunbiao_config
	self.config_npc_group = self.config.NPC_group
	self.config_other = self.config.other[1]
	self.npc_config = Config.npc_auto.npc_list
	self.escort_accept_npc_seq = self.config_other.task_npc_seq
	self.escort_commit_npc_seq = 0
	-- self.escort_commit_npc_seqs = {}
	-- for _, v in pairs(self.config_npc_group) do
	-- 	self.escort_commit_npc_seqs[v.npc_seq] = true
	-- end
	self.min_levels = {
		[1] = 100,
		[2] = 100,
	}
	for _, v in pairs(self.config_yunbiao_config) do
		for i = 1, 2 do
			self.min_levels[i] = (i == v.progress_desc and v.min_level < self.min_levels[i]) and v.min_level or self.min_levels[i]
		end
	end
end

function EscortData:EscortData()
	self.escort_info = {
		today_count = 0,			
		task_seq = 0,
		npc_seq = 0,
		end_time = 0,
	}
end

function EscortData:GetConsumeByType(type)
	local role_level = RoleData.Instance:GetRoleLevel()
	local consume = 0
	for _, v in pairs(self.config_yunbiao_config) do
		if type == v.progress_desc and v.min_level <= role_level and v.max_level >= role_level then
			consume = v.deposit
		end
	end
	return consume
end

function EscortData:GetDailyTime()
	return self.config_other.daily_time
end

function EscortData:GetMinLevelByType(type)
	return 1 == type and self.config_other.normal_open_level or self.config_other.senior_open_level
end

function EscortData:SetEscortAllInfo(protocol)
	self.escort_info = protocol.escort_info
	ActivityData.Instance:SetActivityFinishInfo(ActType.Escort, protocol.escort_info.today_count, self.config_other.daily_time)
	BehaviorData.Instance:RemoveCustomNpcTalkBtnsSeq(self.escort_btn_handle)
	if 0 == protocol.escort_info.task_seq then return end
	self.escort_task_seq = self.escort_info.task_seq
	self.escort_commit_npc_seq = protocol.escort_info.npc_seq
	local info = self.config.yunbiao_config[self.escort_info.task_seq]
	local npc = Config.npc_dynamic_auto.npc_dynamic_cfg[protocol.escort_info.npc_seq]
	
	self.menu_show_info = {
		type_name = Language.Escort.EscortTypeName[info.progress_desc],
		npc_name = npc.name,
		scene_name = SceneData.Instance:GetSceneCfg(npc.scene_id).name
	}

	if info.progress_desc < 3 then MainViewCtrl.Instance:SetGameState(MainViewGameState.escort) end
	--if 4 ~= info.progress_desc then
	if 4 == info.progress_desc then
		self.escort_btn_handle = BehaviorData.Instance:CustomNpcTalkBtnsSeq(self.escort_commit_npc_seq, function(btn_list, obj_vo)
			if obj_vo and self.escort_commit_npc_seq == (obj_vo.seq or -1) then
				table.insert(
					btn_list,
					NpcDialogBehBtnData.FuncCreate(
						Language.Escort.EscortDisStrs.commit,
						BindTool.Bind(
							EscortCtrl.Instance.RequestEscort,
							EscortCtrl.Instance,
							{
								op_type = EscortConfig.OperType.commit
							}
						),
						true
					)
				)
			end
		end)
	end
end

function EscortData:GetEscortAllInfo()
	return self.escort_info
end

function EscortData:GetMenuShowInfo()
	return self.menu_show_info
end

function EscortData:SetEscortResultNotic(protocol)
	local task_seq = self.escort_info.task_seq
	self.escort_info.task_seq = 0
	self.is_win = protocol.is_win
	return task_seq
end

function EscortData:GetDetailInfo(type, level)
	local list = {}
	local deposit = 0
	level = level < self.min_levels[type] and self.min_levels[type] or level
	for _, v in pairs(self.config.yunbiao_config) do
		if v.progress_desc == type and v.min_level <= level and level <= v.max_level then
			deposit = v.deposit
			if 1 == v.progress_desc then
				local item_data = Item.New()
				item_data.item_id = CommonItemId.Coin
				item_data.num = v.reward_coin
				item_data.is_bind = 0
				table.insert(list, item_data)
			else
				for k, v in pairs(v.reward_list) do
					table.insert(list, Item.New(v))
				end
			end
		end
	end
	return list, deposit
end

function EscortData:GetFinishValueData()
	local list = {}
	local cur_config = self.config.yunbiao_config[self.escort_task_seq or 1]
	if 1 == cur_config.progress_desc then
		table.insert(list, {
			item_id = CommonItemId.Coin,
			num = "+" .. cur_config.reward_coin
		})
	else
		for k, v in pairs(cur_config.reward_list) do
			table.insert(list, {
				name = Item.GetName(v.item_id),
				num = "+" .. v.num,
			})
		end
	end
	return list
end

function EscortData:GetEscortSuccTips()
	local cur_config = self.config.yunbiao_config[self.escort_task_seq or 1]
	return Language.Escort.SuccTips[cur_config.progress_desc]
end

function EscortData:GetEscortConfigBySeq(seq)
	local co = self.config.yunbiao_config[seq]
	if co then
		return {
			res_id = EscortConfig.EscortCarResId[co.progress_desc],
			name = Language.Escort.EscortCarNames[co.progress_desc]
		}
	end
end