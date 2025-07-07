TaoQuanNaJiData = TaoQuanNaJiData or BaseClass()

function TaoQuanNaJiData:__init()
    if TaoQuanNaJiData.Instance ~= nil then
		Debuger.LogError("[TaoQuanNaJiData] attempt to create singleton twice!")
		return
	end
	TaoQuanNaJiData.Instance = self


	self.ring_info = SmartData.New({flush = false})
	self.score_info = SmartData.New({flush = false})

	self.play_info = SmartData.New({flush = false})

	self.finish_info = SmartData.New({need_reward = 0, need_finish = 0})
	
	self.map_init = SmartData.New({flush = false})
	self.map_point = SmartData.New({flush = false})

	self.chapter_info = SmartData.New({chapter = 1,limit = 0,need = 0,flush = false})

	self.title_mark = false

	self:ResetInfo()

	self:InitConfig()
end 

function TaoQuanNaJiData:__delete()
    TaoQuanNaJiData.Instance = nil
end

function TaoQuanNaJiData:InitConfig()
	--注册结算事件
	FinishData.Instance:PassRegister(
		BATTLE_MODE.BATTLE_MODE_TAOQUANNAJI,
		{
			desc_func = function()
				return Language.LuckyRing.NajiWin
			end
		}
	)

	FinishData.Instance:FailRegister(
		BATTLE_MODE.BATTLE_MODE_TAOQUANNAJI,
		{
			desc_func = function()
				return Language.LuckyRing.NajiFail
			end
		}
	)

	self:FlushChapterInfo()
end

function TaoQuanNaJiData:ResetInfo()
	local map = self:GetRandGroupInfo()
	local play_map = {}
	for i = 0, 5 do 
		for j = 0, 4 do 
			local vo = {}
			vo.item = map[i * 5 + j + 1]
			vo.x = i
			vo.y = j
			table.insert(play_map, vo)
		end
	end
	self.ring_info.map_init = 1
	self.ring_info.play_map = play_map
	self.ring_info.flush = not self.ring_info.flush

	self.score_info.score = 0
	self.score_info.item_1_count = 0
	self.score_info.item_2_count = 0
	self.score_info.item_3_count = 0
	self.score_info.item_4_count = 0
	self.ring_info.flush = not self.ring_info.flush
end

function TaoQuanNaJiData:TaoZhongItem(x, y)
	local item = self.ring_info.play_map[x * 5 + y + 1].item
	if item > 0 then
		local item_cfg = self:GetItemCfgByItemID(item)
		if item_cfg.sp_type == 1 then
			-- 消除同类型
			for k, v in pairs(self.ring_info.play_map) do
				if v.item > 0 then
					local temp_cfg = self:GetItemCfgByItemID(v.item)
					if temp_cfg.item_type == item_cfg.item_type then
						self:XiaoChuItem(v.x, v.y)
					end
				end
			end
		elseif item_cfg.sp_type == 2 then
			-- 消除当前为中心的九宫格
			self:XiaoChuItem(x + 1, y)
			self:XiaoChuItem(x + 1, y - 1)
			self:XiaoChuItem(x, y - 1)
			self:XiaoChuItem(x - 1, y - 1)
			self:XiaoChuItem(x - 1, y)
			self:XiaoChuItem(x - 1, y + 1)
			self:XiaoChuItem(x, y + 1)
			self:XiaoChuItem(x + 1, y + 1)
		else
			-- 消除单个
			self:XiaoChuItem(x, y)
		end
		self.ring_info.flush = not self.ring_info.flush
		self.score_info.flush = not self.score_info.flush
	end
	-- 套完全部要重置
	if self:IsCompleteAll() then
		self.map_init = 0
	end
	
	TimeHelper:CancelTimer(self.timer)
	self.timer = TimeHelper:AddDelayTimer(function()
		self:CheckIsFinish(self:IsCompleteAll())
	end, 0.5)
end

function TaoQuanNaJiData:XiaoChuItem(x, y)
	local index = x * 5 + y + 1
	if index < 1 and index > 30 and (not self.ring_info.play_map[index]) then
		return
	end
	local item = self.ring_info.play_map[index].item
	if item > 0 then
		local item_cfg = self:GetItemCfgByItemID(item)
		self.score_info["item_"..item_cfg.item_type.."_count"] = self.score_info["item_"..item_cfg.item_type.."_count"] + 1
		self.score_info.score = self.score_info.score + item_cfg.score
		self.ring_info.play_map[index].item = 0
	end

	
end

function TaoQuanNaJiData:GetRandGroupInfo()
	-- 从摆件配置中随机一组，然后根据数量放入数组，打乱
	local rand_num =  0 -- GameMath.Ramdon(#Config.lasso_game_auto.item_group)
	local group =  self.chapter_info.chapter -- Config.lasso_game_auto.item_group[rand_num].group
	local info = {}
	for k, v in pairs(Config.lasso_game_auto.item_group) do
		if group == v.group then
			for i = 1, v.item_num do
				table.insert(info, v.item_id)
			end
		end
	end
	local max = #info
	for i = 1, 300 do
		local rand_id1 = GameMath.Ramdon(max)
		local rand_id2 = GameMath.Ramdon(max)
		if rand_id1 ~= rand_id2 then
			local temp = info[rand_id1]
			info[rand_id1] = info[rand_id2]
			info[rand_id2] = temp
		end
	end
	return info
end

function TaoQuanNaJiData:IsCompleteAll()
	for k, v in pairs(self.ring_info.play_map) do
		if v.item > 0 then
			self.ring_info.map_init = 1
			return false
		end
	end
	self.ring_info.map_init = 0
	self.ring_info.flush = not self.ring_info.flush
	return true
end

function TaoQuanNaJiData:SetPosClick(pos)

end

function TaoQuanNaJiData:OnRoleNoticeComplete()
	-- TaoQuanNaJiCtrl.Instance:SendLuckyRingReq({req_type = 0,param1 = 0,param2 = 0})
end

function TaoQuanNaJiData:InRing()
	-- return SceneData.Instance:SceneId() == Config.lasso_game_auto.other[1].scene_id
	return true
end

-- function TaoQuanNaJiData:SetRingInfo(protocol)
-- 	-- LogDG("健哥哥救救我 1140",protocol)
-- 	self.ring_info.map_init = protocol.map_init
-- 	self.ring_info.play_map = protocol.play_map
-- 	self.ring_info.can_play_times = protocol.can_play_times
-- 	self.ring_info.today_play_times = protocol.today_play_times
-- 	self.ring_info.flag_title =  protocol.flag_title

-- 	self.ring_info.flush = not self.ring_info.flush
-- end

-- function TaoQuanNaJiData:SetScoreInfo(protocol)
-- 	-- LogDG("健哥哥救救我 1141",protocol)

-- 	self.score_info.score = protocol.score
-- 	self.score_info.item_1_count = protocol.item_1_count
-- 	self.score_info.item_2_count = protocol.item_2_count
-- 	self.score_info.item_3_count = protocol.item_3_count
-- 	self.score_info.item_4_count = protocol.item_4_count

-- 	self.score_info.flush = not self.score_info.flush
-- end

function TaoQuanNaJiData:MapAvaile() 
	return self.ring_info.map_init == 1
end 

-- function TaoQuanNaJiData:TitleMarkCheck(protocol)
-- 	if self.ring_info.flag_title == nil then 
-- 		return 
-- 	end 

-- 	-- 当产生了这么一个下发时记录下来
-- 	self.title_mark = self.ring_info.flag_title ~= protocol.flag_title 
-- 	    and self.ring_info.flag_title == 0 and protocol.flag_title == 1
-- end

-- 进场前更新的地图信息 
-- function TaoQuanNaJiData:FixMapFlush()
-- 	-- if self.ring_info.map_init == 0 then 
-- 	TaoQuanNaJiCtrl.Instance:SendLuckyRingReq({req_type = 1,param1 = 0,param2 = 0})
-- 		-- return false
-- 	-- end 

-- end

function TaoQuanNaJiData:MapInitComplete()
	self.map_init.flush = not self.map_init.flush
end

function TaoQuanNaJiData:FlushShowPoint(list)
	if #list == 0 then return end 
	self.map_point.list = list

	self.map_point.flush = not self.map_point.flush
end

-- 检查关卡信息
function TaoQuanNaJiData:CheckChapter(is_win)
	self:FlushChapterNum(is_win and 1 or 0)
	self:FlushChapterInfo()
end

function TaoQuanNaJiData:FlushLimit()
	self.chapter_info.limit = self.chapter_info.limit - 1
	self.chapter_info.flush = not self.chapter_info.flush	
end

function TaoQuanNaJiData:CheckLimit()
	if self.chapter_info.limit <= 0 then 
		return true
	end 

	return false
end

function TaoQuanNaJiData:CheckPoint()
	return self.score_info.score >= self.chapter_info.need
end

-- 检查当前关卡是否结束
function TaoQuanNaJiData:CheckIsFinish(map_done)
	local flag = self:CheckLimit()
	local point_enough = self:CheckPoint()

	-- 在地图完成或者圈用完的情况下 结束本局游戏
	if flag or map_done or point_enough then
		-- 胜利
		FinishData.Instance:SetBattleMode(BATTLE_MODE.BATTLE_MODE_TAOQUANNAJI)
		if self.chapter_info.need <= self.score_info.score then
			ViewMgr:OpenView(FinishPassView)	
		-- 失败
		else 
			ViewMgr:OpenView(FinishFailView)
		end 

		self:CheckChapter(self.chapter_info.need <= self.score_info.score)
		self:ResetInfo()
	end
end

function TaoQuanNaJiData:FlushChapterNum(type)
	-- 赢了+ 1 
	if type == 1 then 
		self.chapter_info.chapter = self.chapter_info.chapter + 1
	end 

	if self.chapter_info.chapter >= #Config.lasso_game_auto.battle then 
		self.chapter_info.chapter = 1
	end 
end

function TaoQuanNaJiData:FlushChapterInfo()

	self.chapter_info.limit = Config.lasso_game_auto.battle[self.chapter_info.chapter].num
	self.chapter_info.need = Config.lasso_game_auto.battle[self.chapter_info.chapter].score

	self.chapter_info.flush = not self.chapter_info.flush
end

function TaoQuanNaJiData:FixMapItemData(v)
	local cfg = Config.lasso_game_auto.item[v.item]

	local vo = {
		item_id = cfg == nil and 0 or cfg.item_id,
		item_type = cfg == nil and 0 or cfg.item_type,
		score = cfg == nil and 0 or cfg.score,
		sp_type = cfg == nil and 0 or cfg.sp_type,
		word = cfg == nil and "" or cfg.word,

		x = v.x,
		y = v.y,
	}

	return vo
end

function TaoQuanNaJiData:FlushTarget(param)
	self.play_info.target = param 

	self.play_info.flush = not self.play_info.flush

	self:EffectSimulate()
	self:FlushLimit()
	if param.empty then 
		-- TaoQuanNaJiCtrl.Instance:SendLuckyRingReq({req_type = 2,param1 = -1,param2 = -1})
		TimeHelper:CancelTimer(self.timer)
	    self.timer = TimeHelper:AddDelayTimer(function()
	       self:CheckIsFinish(false)
	    end,0.5)
	else 
		-- TaoQuanNaJiCtrl.Instance:SendLuckyRingReq({req_type = 2,param1 = param.x,param2 = param.y})
		self:TaoZhongItem(param.x, param.y)
	end 
	
end

function TaoQuanNaJiData:FlushHighPointShow()
	local reward_max = Config.lasso_game_auto.reward[1]
	local cur_point = self.score_info.score or 0

	local mark = 1 
	for i = #Config.lasso_game_auto.reward, 1,-1 do 
		local cfg = Config.lasso_game_auto.reward[i]
		if cfg.score < cur_point then 
			reward_max = cfg
			mark = i
			break
		end 
	end 

	local result = {}
	result.flag = reward_max.score_desc ~= ""
	result.cfg = reward_max
	result.mark = mark
	result.word = mark == 1 and string.format(reward_max.score_desc,Config.lasso_game_auto.reward[mark+1].score - cur_point) 
	    or reward_max.score_desc

	return result
end

-- function TaoQuanNaJiData:GetRemindNum()
-- 	return self.ring_info.today_play_times == 0 and 1 or 0 
-- end 

-- function TaoQuanNaJiData:IsActOpen()
--     return PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.LuckyRing)
-- end

-- 
function TaoQuanNaJiData:FinishData()
	local result = {}
	local info = self:FlushHighPointShow()
	local reward = {}
	for k,v in pairs(Config.lasso_game_auto.reward)do 
		if info.mark == k then -- 是替换，不是累加 直接等于
		    for index,value in pairs(v.final_reward) do
				local entered = false
		        local mark = -1
				for i,j in pairs(reward) do 
					if value.item_id == j.item_id then 
						mark = i
						entered = true
						break
					end 
				end 
				if entered then 
			    	reward[mark].num = value.num + reward[mark].num
				else 
					if self.finish_info.need_reward > 0 then
					    table.insert(reward,value)
					end
			    end 
			end 
		end 
	end 
	if self.finish_info.can_title and self.ring_info.flag_title == 1 then 
		table.insert(reward,{item_id = Config.lasso_game_auto.other[1].title_item_id,num = 1,is_bind = 0})
	end 
	result.reward_list = BagData.Instance:ItemListBuild(reward)
	result.show_type = self.score_info.score > Config.lasso_game_auto.reward[2].score and 1 or 5
	result.point_list = {}
	result.total_point = self.score_info.score

	for i = 1,3 do 
		local vo = {}
		vo.light = self.score_info["item_"..i.."_count"] > 0 
		local name = Language.LuckyRing.ItemType[i]
		vo.str = vo.light and string.format(Language.LuckyRing.FinishPoint,name,self.score_info["item_"..i.."_count"] * self:GetItemPointByItemType(i))
		    or Language.LuckyRing.FinishNoPoint..name

		if vo.light then 
		    table.insert( result.point_list, vo )
		end 
	end 

	return result
end

function TaoQuanNaJiData:GetItemPointByItemType(type)
	for k,v in pairs(Config.lasso_game_auto.item) do 
		if v.item_type == type then 
			return v.score
		end 
	end 

	return 0
end


function TaoQuanNaJiData:GetItemPointByItemId(item_id)
	for k,v in pairs(Config.lasso_game_auto.item) do 
		if v.item_id == item_id then 
			return v.score
		end 
	end 

	return 0
end

function TaoQuanNaJiData:GetItemCfgByItemID(item_id)
	for k,v in pairs(Config.lasso_game_auto.item) do 
		if v.item_id == item_id then 
			return v
		end 
	end 

	return nil
end

function TaoQuanNaJiData:IsLuckyRingTitle(title_id)
	return title_id == Config.lasso_game_auto.other[1].title_id
end


function TaoQuanNaJiData:EffectSimulate()
	self.map_point.sim_list = {}

	-- 没中就不生成了
	if self.play_info.target.empty then 
		return 
	end 

    local mark_point = self.play_info.target

	local pos = mark_point.x * 5+ mark_point.y + 1
	local vo = {pos = pos, x = mark_point.x,y = mark_point.y}
	table.insert(self.map_point.sim_list,vo)

	if mark_point.sp_type == 1 then 
		-- 光圈特效则全部消除
		for k,v in pairs(self.ring_info.play_map) do 
			local cfg = self:GetItemCfgByItemID(v.item)
			if cfg ~= nil and cfg.item_type == mark_point.item_type then 
				local pos = v.x * 5+ v.y + 1
				local vo = {pos = pos, x = v.x,y = v.y}
				table.insert(self.map_point.sim_list,vo)
			end 
		end 

	elseif mark_point.sp_type == 2 then
		-- 类型欢乐兔则消除九宫阵
		local x_start = mark_point.x > 0 and mark_point.x - 1 or 0
		local x_end = mark_point.x < 5 and mark_point.x + 1 or 5

		local y_start = mark_point.y > 0 and mark_point.y - 1 or 0
		local y_end = mark_point.y < 4 and mark_point.y + 1 or 4

		for i = x_start,x_end do 
			for j = y_start,y_end do 
				for k,v in pairs(self.ring_info.play_map) do  
					if v.x == i and v.y == j and (
						(v.x ~= mark_point.x and v.y == mark_point.y) 
					    or (v.x == mark_point.x and v.y ~= mark_point.y)
						or (v.x ~= mark_point.x and v.y ~= mark_point.y)) 
						then 
						if v.item > 0 then 
							local pos = v.x * 5+ v.y + 1
							local vo = {pos = pos, x = v.x,y = v.y}
							table.insert(self.map_point.sim_list,vo)
						end 
					end 
				end 
			end 
		end 
	end
end

function TaoQuanNaJiData:SimulateEffectCheck(data)
	for k,v in pairs(self.map_point.sim_list) do 
		if v.x == data.x and v.y == data.y then 
			return true
		end 
	end 
    return false
end