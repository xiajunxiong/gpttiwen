
HarmonyHeavenData = HarmonyHeavenData or BaseClass()

function HarmonyHeavenData:__init()
	if HarmonyHeavenData.Instance ~= nil then
		Debuger.LogError("[HarmonyHeavenData] attempt to create singleton twice!")
		return
	end
    HarmonyHeavenData.Instance = self

    self:HarmonyHeavenConfig()
    self:HarmonyHeavenData()
end

function HarmonyHeavenData:HarmonyHeavenConfig()
   self.config = Config.promeval_paradise_auto
   self.config_boss_fb = self.config.boss_fb
   self.config_exchange_store = self.config.exchange_store
   self.config_other = self.config.other[1]

   Cfg.Register("PromevalParadiseBossFbTypeSeq",Cfg.RealTimeSearch2Key(Config.promeval_paradise_auto.boss_fb,"fb_type","seq"))
   Cfg.Register("PromevalParadiseBossFbId",Cfg.RealTimeSearch(Config.promeval_paradise_auto.boss_fb,"fb_id"))
   Cfg.Register("PromevalParadiseFbGroupId",Cfg.RealTimeSearch(Config.promeval_paradise_auto.fb_group,"fb_id"))
   Cfg.Register("PromevalParadiseTaskNpcId",Cfg.RealTimeSearch(Config.promeval_paradise_auto.task_npc,"fb_id"))
   Cfg.Register("PromevalParadiseBossFbTargetId",Cfg.RealTimeSearch(Config.promeval_paradise_auto.boss_fb,"target_id"))
end

function HarmonyHeavenData:HarmonyHeavenData()
    self.fb_id_list = {}
    self.fb_type_list = {
        [1] = {},
        [2] = {},
    }
    for k, v in ipairs(self.config_boss_fb) do
		if 1 == v.is_open then
			self.fb_id_list[v.fb_id_1] = v
			self.fb_type_list[v.fb_type][v.seq_1] = v
		end
    end

	self.fb_sm = SmartData.New{
		info = SmartData.New{init = true}
	}
	self.fb_info = SmartData.New{
		scene_info = SmartData.New{init = true},
		role_info = SmartData.New{init = true},
		world_info = SmartData.New{init = true},
		reward_info = SmartData.New{init = true},
		team_info = SmartData.New{init = true},
	}
end

function HarmonyHeavenData:ConfigOtherFbCfg()
    return self.config_other.fb_cfg
end

function HarmonyHeavenData:ConfigOtherHelpTimes()
    return self.config_other.help_times
end

function HarmonyHeavenData:ConfigOtherFirstDifficult()
	return self.config_other.first_difficult
end

function HarmonyHeavenData:ConfigOtherDownPercent()
	return self.config_other.down_percent
end

function HarmonyHeavenData:ConfigOtherMinDifficult()
	return self.config_other.min_difficult
end

function HarmonyHeavenData:ConfigOtherStartLevel()
    return self.config_other.start_level
end

function HarmonyHeavenData:ConfigOtherBossRewardLimit()
	return self.config_other.boss_reward_limit
end

function HarmonyHeavenData:ConfigOtherHelpRewardLimit()
	return self.config_other.help_reward_limit
end

function HarmonyHeavenData:ConfigOtherRewardTimesAll()
	if nil == self.reward_times_all then
		local num = 0
		for _, times in ipairs(string.split(self.config_other.reward_times, "|")) do
			num = num + tonumber(times)
		end
		self.reward_times_all = num
	end
	return self.reward_times_all
end

function HarmonyHeavenData:ConfigOtherRewardTimesBoss()
	if nil == self.reward_times_boss then
		local t = {}
		for _, times in ipairs(string.split(self.config_other.reward_times, "|")) do
			table.insert(t, tonumber(times))
		end
		self.reward_times_boss = t
	end
	return self.reward_times_boss
end

function HarmonyHeavenData:BossRewardRepeatShow(mark)
	if nil == self.boss_reward_repeat_show then
		local str_show = ""
		for _, day in ipairs(string.split(self.config_other.boss_reward_repeat, "|")) do
			str_show = str_show .. string.format(Language.HarmonyHeaven.Main.BossRewardRepeat, DataHelper.GetDaXie(tonumber(day)), mark)
		end
		self.boss_reward_repeat_show = string.sub(str_show, 1, - string.len(mark) - 1)
	end
    return self.boss_reward_repeat_show
end

function HarmonyHeavenData:HelpRewardRepeatShow(mark)
	if nil == self.help_reward_repeat_show then
		local str_show = ""
		for _, day in ipairs(string.split(self.config_other.help_reward_repeat, "|")) do
			str_show = str_show .. string.format(Language.HarmonyHeaven.Main.HelpRewardRepeat, DataHelper.GetDaXie(tonumber(day)), mark)
		end
		self.help_reward_repeat_show = string.sub(str_show, 1, - string.len(mark) - 1)
	end
    return self.help_reward_repeat_show
end

function HarmonyHeavenData:TeamNumShow(mark)
	if nil == self.team_num_show then
		local str_show = ""
		for _, num in ipairs(string.split(self.config_other.team_num, "|")) do
			str_show = str_show .. string.format(Language.HarmonyHeaven.Main.TeamNum, num, mark)
		end
		self.team_num_show = string.sub(str_show, 1, - string.len(mark) - 1)
	end
    return self.team_num_show
end

function HarmonyHeavenData:TeamNumMin()
	if nil == self.team_num_min then
		self.team_num_min = tonumber(string.split(self.config_other.team_num, "|")[1] or 0)
	end
    return self.team_num_min
end

function HarmonyHeavenData:GetFbType()
	local type = 1
	if self.fb_open_id then
		type = self.fb_id_list[self.fb_open_id] and self.fb_id_list[self.fb_open_id].fb_type or 1
	end
	return type 
end

function HarmonyHeavenData:FbShowListAutoSel()
	if nil == self.fb_open_id then
		local list = self.fb_type_list[1]
		local equip_level = MarbasData.Instance:GetTotalLevel()
		local info_sel = list[1]
		for _, info in ipairs(list) do
			if equip_level >= info.equip_require and 1 == info.is_open then
				info_sel = (info_sel.equip_require < info.equip_require) and info or info_sel
			end
		end
		self.fb_open_id = info_sel.fb_id_1
	end
end

function HarmonyHeavenData:GetFbShowList(type)
	local list = self.fb_type_list[type]
	local info_sel = list[1]
	if self.fb_open_id then
		info_sel = self.fb_id_list[self.fb_open_id] or info_sel
		self.fb_open_id = nil
	else
		local info_pre = self.fb_sm.info
		if not info_pre.init then
			for _, info in ipairs(list) do
				if info_pre.seq_1 >= info.seq_1 then
					info_sel = info
				end
			end
		end
	end
	return list, info_sel
end

function HarmonyHeavenData:GetRewardsShow(info)
	if nil == info.rewards_show then
		local co = Cfg.PromevalParadiseFbGroupId(info.fb_id)
		local rewards = {}
		local help_rewards = {}
		local rewards_first = {}
		local monster_rewards = {}
		if co then
			if co.reward_view_zc then
				for y, z in pairs(string.split(co.reward_view_zc, ":")) do
					rewards_first[tonumber(z)] = true
				end
			end
			for _, item_id in ipairs(string.split(co.reward_view, ":")) do
				local reward = Item.Init(tonumber(item_id))
				reward.is_hh_first = rewards_first[reward.item_id]
				reward.is_hh_id = info.fb_id
				table.insert(rewards, reward)
			end
			for _, v in pairs(co.help_reward) do
				local reward = Item.Init(v.item_id, v.num, v.is_bind)
				table.insert(help_rewards, reward)
			end
			for i = 1, HarmonyHeavenConfig.BOSS_NUM_MAX do
				monster_rewards[i] = {}
				for _, item_id in ipairs(string.split(co["boss_reward_" .. i], ":")) do
					local reward = Item.Init(tonumber(item_id))
					table.insert(monster_rewards[i], reward)
				end
			end
		end
		info.rewards_show = rewards
		info.help_rewards_show = help_rewards
		info.monster_rewards_show = monster_rewards
	end
	local fpm = self.fb_info.role_info.fb_first_pass_mark
	table.sort(info.rewards_show, function (a, b)
		if not a or not b then return a end
		local as = a.is_hh_first and (1 == (fpm[a.is_hh_id] or 0) and 3 or 1) or 2
		local bs = b.is_hh_first and (1 == (fpm[b.is_hh_id] or 0) and 3 or 1) or 2
		as = as * 1000 + b:Color()
		bs = bs * 1000 + a:Color()
		return as < bs
	end)
	return info.rewards_show, info.help_rewards_show
end

function HarmonyHeavenData:GetMonstersShow(info)
	if nil == info.monsters_show then
		local co = Cfg.PromevalParadiseTaskNpcId(info.fb_id)
		local monsters = {}
		if co then
			for i = 1, HarmonyHeavenConfig.BOSS_NUM_MAX do
				local npc = Cfg.NpcById(co["npc_id_" .. i])
				table.insert(monsters, {
					index = i,
					icon_id = npc.npc_icon,
					name = npc.name,
					monster_group_id = co["monster_group_" .. i],
					scene_id = info.scene_id,
				})
			end
		end
		info.monsters_show = monsters
	end
	return info.monsters_show
end

function HarmonyHeavenData:GetMonsterRewardsShow(info, index)
	if nil == info.monster_rewards_show then
		local co = Cfg.PromevalParadiseFbGroupId(info.fb_id)
		local monster_rewards = {}
		if co then
			for i = 1, HarmonyHeavenConfig.BOSS_NUM_MAX do
				monster_rewards[i] = {}
				for _, item_id in ipairs(string.split(co["boss_reward_" .. i], ":")) do
					local reward = Item.Init(tonumber(item_id))
					table.insert(monster_rewards[i], reward)
				end
			end
		end
		info.monster_rewards_show = monster_rewards
	end
	return info.monster_rewards_show[index] or {}
end

function HarmonyHeavenData:GetFbPassTime(fb_id)
	local info = self.fb_info.world_info
	if info.init then return 0 end
	return info.first_pass_time[fb_id] or 0
end

function HarmonyHeavenData:GetFbShopShowList()
	if nil == self.fb_shop_list then
		local list = {}
		for k, v in ipairs(self.config_exchange_store) do
		   if v.is_show then
				table.insert(list, v)
		   end
		end
		self.fb_shop_list = list
	end
	return self.fb_shop_list
end

function HarmonyHeavenData:GetFbShopBuyNum(limit_seq)
	local info = self.fb_info.role_info
	if info.init then return 0 end
	return info.limit_buy_times[limit_seq] or 0
end

function HarmonyHeavenData:GetFightMapObjs()
	local objs = {}
	local scene_info = self.fb_info.scene_info
	if scene_info.init then return objs end
    local info = Cfg.PromevalParadiseTaskNpcId(scene_info.fb_id)
	local seq = scene_info.stage + 1
	if info and info["npc_id_" .. seq] then
		objs[1] = {
			pos_x = info["npc_x_" .. seq],
			pos_y = info["npc_y_" .. seq],
		}
	end
	return objs
end
function HarmonyHeavenData:GetRewardMapObjs()
	local objs = {}
	local scene_info = self.fb_info.scene_info
	if scene_info.init then return objs end
    local info = Cfg.PromevalParadiseTaskNpcId(scene_info.fb_id)
	local seq = scene_info.stage
	if info and info["npc_id_" .. seq] then
		objs[1] = {
			pos_x = info["npc_x_" .. seq],
			pos_y = info["npc_y_" .. seq],
		}
	end
	return objs
end

function HarmonyHeavenData:SetHongMengTianJieSceneInfo(protocol)
	-- ylog("SetHongMengTianJieSceneInfo", protocol)
	self.fb_info.scene_info:Set(protocol.info)
end

function HarmonyHeavenData:SetHongMengTianJieRoleInfo(protocol)
	-- ylog("SetHongMengTianJieRoleInfo", protocol)
	self.fb_info.role_info:Set(protocol.info)
end

function HarmonyHeavenData:SetHongMengTianJieWorldInfo(protocol)
	-- ylog("SetHongMengTianJieWorldInfo", protocol)
	self.fb_info.world_info:Set(protocol.info)
end

function HarmonyHeavenData:SetHongMengTianJieTalkInfo(protocol)
	-- ylog("SetHongMengTianJieTalkInfo", protocol)
	NpcDialogData.Instance:NpcDialogSync(protocol.info)
	-- self.fb_info.world_info:Set(protocol.info)
end

function HarmonyHeavenData:SetHongMengTianJieRewardInfo(protocol)
	-- ylog("SetHongMengTianJieRewardInfo", protocol)
	self.fb_info.reward_info:Set(protocol.info)
end

function HarmonyHeavenData:SetHongMengTianJiePassTeamInfo(protocol)
	-- ylog("SetHongMengTianJiePassTeamInfo", protocol)
	self.fb_info.team_info:Set(protocol.info)
	ViewMgr:OpenView(HarmonyHeavenRecordView)
end

function HarmonyHeavenData:SetResultInfoAll(protocol)
    local list1, list2 = {}, {}
    for _, item in ipairs(protocol.item_list) do
        if list1[item.item_id] then
            list1[item.item_id].num = list1[item.item_id].num + item.num
        else
            list1[item.item_id] = item
        end
    end
    for _, item in pairs(list1) do
        table.insert(list2, item)
    end
    self.reward_list_all = DataHelper.FormatItemList(list2)
    -- ViewMgr:OpenView(HarmonyHeavenRewardView)
end

function HarmonyHeavenData:AutoToNpc()
	if TeamData.Instance:InTeam() and not TeamData.Instance:IsLeader() then
		return
	end
	local scene_info = self.fb_info.scene_info
    if scene_info.init then return end
    local info = Cfg.PromevalParadiseTaskNpcId(scene_info.fb_id)
    local stage = scene_info.stage
    if stage < 3 then
        local index = stage + 1
        SceneLogic.Instance:AutoToPos(nil, {x = info["npc_x_" .. index], y = info["npc_y_" .. index]},function ()
            FightData.Instance:AutoToNpc()
        end,true,false)
    end
end

function HarmonyHeavenData:SetHHVideoMapId(scene_id)
	if Config.scenes[scene_id] then
		self.battle_map_id = Config.scenes[scene_id].mapid
	end
end

function HarmonyHeavenData:GetHHVideoMapId()
	return self.battle_map_id
end

function HarmonyHeavenData:ClearHHVideoMapId()
	self.battle_map_id = nil
end

function HarmonyHeavenData:GetHarmonyHeavenFBName()
	local scene_info = self.fb_info.scene_info
    if scene_info.init then return "" end
    local info = Cfg.PromevalParadiseBossFbId(scene_info.fb_id)
    return info.fb_name
end