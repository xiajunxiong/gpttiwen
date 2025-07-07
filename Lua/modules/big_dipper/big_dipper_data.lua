BigDipperData = BigDipperData or BaseClass()

BigDipperData.RANK_TYPE1 = 24
BigDipperData.RANK_TYPE2 = 25
BigDipperData.RANK_TYPE3 = 26
BigDipperData.RANK_TYPE4 = 27
BigDipperData.RANK_TYPE5 = 28
BigDipperData.RANK_TYPE6 = 29
BigDipperData.RANK_TYPE7 = 30
BigDipperData.RANK_TYPE8 = 31

function BigDipperData:__init()
	if BigDipperData.Instance ~= nil then
		Debuger.LogError("[BigDipperData] attempt to create singleton twice!")
		return
	end	
	BigDipperData.Instance = self
	self.boss_list = SmartData.New()
	self.boss_info = SmartData.New()
	self.my_info = SmartData.New()
	self.help_info = SmartData.New()
	self.rank_data = SmartData.New({flush_num = 0})
	self.first_reward_count = 0
end

function BigDipperData:__delete()

end

function BigDipperData:CheckNextWeekTime()
    local server_time = TimeManager:GetServerTime()
    local dates = os.date("%a",server_time)
    local dat_num = WeekMark[dates]

    local last = 8 - dat_num 
    local date_s = os.date("*t",server_time)
    date_s.day = date_s.day + last
    local time = TimeCtrl.Instance:ChangDataStampTime(date_s) or 0
    return time
end

function BigDipperData:CheckRemind()
	local boss_list = self:GetShowBossList()
    for k,v in pairs(boss_list) do 
        if self:CheckBossIsRed(v) then 
            return 1
        end 
    end 
    return 0
end

function BigDipperData:CheckBossIsRed(boss_info)
	local info = self.my_info[boss_info.sn + 1]
	if info and info.first_pass_flag == 1 and info.first_reward_flag == 0 then
		return true
	end
	return false
end

function BigDipperData:SetBigDipperRankList(protocol)
    self.rank_data.rank_type = protocol.info.rank_type
    self.rank_data.my_rank_index = protocol.info.my_rank_index
    self.rank_data.rank_infos = protocol.info.rank_infos

    self.rank_data.flush_num = self.rank_data.flush_num + 1
end

function BigDipperData:GetShowBossList()
	local list = {}
	local is_has_big = false
	for k,v in pairs(self.boss_list) do 
		local info = {}
		for k1,v1 in pairs(Config.big_dipper_auto.star_man_position) do 
			if v1.sn == v then
				info.npc_seq  = v1.npc_seq
				info.is_pass = false
				info.is_lock = true
				info.type = v1.type
				info.sn = v1.sn
				local week_num = ActivityData.Instance:GetWeekNum()
				if week_num == 7 then
					week_num = 0
				end
				if info.type == 1 then
					local open_time = string.split(v1.open_time, "|")
					for k2,v2 in pairs(open_time) do 
						if week_num == tonumber(v2) then
							info.is_lock = false
							break
						end
					end
					info.open_time = 100
				else
					if week_num == tonumber(v1.open_time) then
						info.is_lock = false
					end
					info.open_time = tonumber(v1.open_time)
					if info.open_time == 0 then
						info.open_time = 7
					end
				end
				break
			end
		end
		info.pass_count = 0
		local pass_flag = bit:d2b(0)
		local m_info = self.my_info[v + 1]
		if m_info then
			pass_flag = bit:d2b(m_info.pass_flag)
		end
		if info.type == 0 then
			if pass_flag[32] == 1 then
				info.is_pass = true
				info.pass_count = 1
			end
		else		
		   local pass_count = 0 
		   for i = 1, 3 do
				if pass_flag[33-i] == 1 then
					pass_count = pass_count + 1
				end
		   end
			if pass_count == 3 then
				info.is_pass = true
			end
			info.pass_count = pass_count
		end
		info.help_times = 0
		local help_info = self.help_info[v + 1]
		if help_info then
			info.help_times = help_info
		end
		
		table.insert(list,info)
	end
	
	table.sort(list, function (a,b)
        local s_a = 1
        local s_b = 1
        if a.type < b.type then
            s_a = s_a + 10000
        elseif a.type == b.type then
            s_a = s_a + 10000
            s_b = s_b + 10000	
        else
            s_b = s_b + 10000
        end
		
		if a.open_time < b.open_time then
            s_a = s_a + 1000
        elseif a.open_time == b.open_time then
            s_a = s_a + 1000
            s_b = s_b + 1000
        else
            s_b = s_b + 1000
        end
		
        return s_a > s_b
    end)
	

	return list
end

function BigDipperData:GetBossNameBySeq(seq)
	local npc_id = Cfg.NpcBySeq(111).id 
	if Cfg.NpcBySeq(seq) then
		npc_id = Cfg.NpcBySeq(seq).id
	end
	local npc_config = Cfg.NpcById(npc_id)
	return npc_config.name
end

function BigDipperData:GetRankBossNameList()
	local list = {}
	for k,v in pairs(Config.big_dipper_auto.star_man_position) do 
		table.insert(list,self:GetBossNameBySeq(v.npc_seq))
	end
	return list
end

function BigDipperData:GetAtlasBossList()
	local list = {}
	for k,v in pairs(Config.big_dipper_auto.star_man_position) do 
		local atlas = {}
		atlas.is_lock = true
		atlas.info = {}
		for k1,v1 in pairs(self:GetShowBossList()) do 
			if v1.npc_seq == v.npc_seq then
				atlas.is_lock = false
				atlas.info = v
			end
		end
		table.insert(list,atlas)
	end
	return list
end

function BigDipperData:GetShowBossTimes()
	local times = 0
	local list = self:GetShowBossList()
	for k,v in pairs(list) do 
		if not v.is_lock and not v.is_pass then
			times = times + 1
		end
	end
	return times
end

function BigDipperData:BossIsPassBySn(sn)
	local sn  = sn
	local list = self:GetShowBossList()
	local is_pass = false
	for k,v in pairs(list) do 
		if v.sn == sn then
			if v.is_pass then
				is_pass = true
			end
			break
		end
	end
	return is_pass
end

function BigDipperData:BossIsOpenBySeq(seq)
	local is_open = true
	for k,v in pairs(Config.big_dipper_auto.star_man_position) do 
		if v.npc_seq == seq then
			is_open = false
		end
	end
	local list = self:GetShowBossList()
	for k,v in pairs(list) do 
		if v.npc_seq == seq and not v.is_lock then
			is_open = true
			break
		end
	end
	return is_open
end

function BigDipperData:GetActivtyShowBossData()
	local count = 0
	local total = 1
	local list = self:GetShowBossList()
	for k,v in pairs(list) do 
		if not v.is_lock then
			count = count + v.pass_count	
			--if v.type == 1 then
				--total = 4
			--end	
		end
	end
	count = count > 0 and 1 or 0
	return count,total
end

function BigDipperData:SetOpenIndex(index)
	self.open_index = index
end

function BigDipperData:GetOpenIndex()
	return self.open_index
end
