LuckyRingData = LuckyRingData or BaseClass()

function LuckyRingData:__init()
    if LuckyRingData.Instance ~= nil then
		Debuger.LogError("[LuckyRingData] attempt to create singleton twice!")
		return
	end
	LuckyRingData.Instance = self


	self.ring_info = SmartData.New({flush = false})
	self.score_info = SmartData.New({flush = false})

	self.play_info = SmartData.New({flush = false})

	-- self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,
    -- BindTool.Bind(self.OnRoleNoticeComplete, self))

	self.finish_info = SmartData.New({need_reward = 0,need_finish = 0})
	
	self.map_init = SmartData.New({flush = false})
	self.map_point = SmartData.New({flush = false})

	self.title_mark = false
end 

function LuckyRingData:__delete()
    LuckyRingData.Instance = nil
end


function LuckyRingData:SetPosClick(pos)

end

function LuckyRingData:OnRoleNoticeComplete()
	-- LuckyRingCtrl.Instance:SendLuckyRingReq({req_type = 0,param1 = 0,param2 = 0})
end

function LuckyRingData:InRing()
    return SceneData.Instance:SceneId() == Config.lasso_auto.other[1].scene_id
end

function LuckyRingData:SetRingInfo(protocol)
	-- LogDG("健哥哥救救我 1140",protocol)
	self.ring_info.map_init = protocol.map_init
	self.ring_info.play_map = protocol.play_map
	self.ring_info.can_play_times = protocol.can_play_times
	self.ring_info.today_play_times = protocol.today_play_times
	self.ring_info.flag_title =  protocol.flag_title

	self.ring_info.flush = not self.ring_info.flush
end

function LuckyRingData:SetScoreInfo(protocol)
	-- LogDG("健哥哥救救我 1141",protocol)

	self.score_info.score = protocol.score
	self.score_info.item_1_count = protocol.item_1_count
	self.score_info.item_2_count = protocol.item_2_count
	self.score_info.item_3_count = protocol.item_3_count
	self.score_info.item_4_count = protocol.item_4_count

	self.score_info.flush = not self.score_info.flush
end

function LuckyRingData:MapAvaile() 
	return self.ring_info.map_init == 1
end 

function LuckyRingData:TitleMarkCheck(protocol)
	if self.ring_info.flag_title == nil then 
		return 
	end 

	-- 当产生了这么一个下发时记录下来
	self.title_mark = self.ring_info.flag_title ~= protocol.flag_title 
	    and self.ring_info.flag_title == 0 and protocol.flag_title == 1
end

-- 进场前更新的地图信息 
function LuckyRingData:FixMapFlush()
	-- if self.ring_info.map_init == 0 then 
	LuckyRingCtrl.Instance:SendLuckyRingReq({req_type = 1,param1 = 0,param2 = 0})
		-- return false
	-- end 

end

function LuckyRingData:MapInitComplete()
	self.map_init.flush = not self.map_init.flush
end

function LuckyRingData:FlushShowPoint(list)
	if #list == 0 then return end 
	self.map_point.list = list

	self.map_point.flush = not self.map_point.flush
end

function LuckyRingData:FixMapItemData(v)
	local cfg = Config.lasso_auto.item[v.item]

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

function LuckyRingData:FlushTarget(param)
	self.play_info.target = param 

	self.play_info.flush = not self.play_info.flush

	self:EffectSimulate()
	if param.empty then 
		LuckyRingCtrl.Instance:SendLuckyRingReq({req_type = 2,param1 = -1,param2 = -1})
	else 
		LuckyRingCtrl.Instance:SendLuckyRingReq({req_type = 2,param1 = param.x,param2 = param.y})
	end 


end

function LuckyRingData:FlushHighPointShow()
	local reward_max = Config.lasso_auto.reward[1]
	local cur_point = self.score_info.score or 0

	local mark = 1 
	for i = #Config.lasso_auto.reward, 1,-1 do 
		local cfg = Config.lasso_auto.reward[i]
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
	result.word = mark == 1 and string.format(reward_max.score_desc,Config.lasso_auto.reward[mark+1].score - cur_point) 
	    or reward_max.score_desc

	return result
end

function LuckyRingData:GetRemindNum()
	return self.ring_info.today_play_times == 0 and 1 or 0 
end 

function LuckyRingData:IsActOpen()
    return PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.LuckyRing)
end

-- 
function LuckyRingData:FinishData()
	local result = {}
	local info = self:FlushHighPointShow()
	local reward = {}
	for k,v in pairs(Config.lasso_auto.reward)do 
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
		table.insert(reward,{item_id = Config.lasso_auto.other[1].title_item_id,num = 1,is_bind = 0})
	end 
	result.reward_list = BagData.Instance:ItemListBuild(reward)
	result.show_type = self.score_info.score > Config.lasso_auto.reward[2].score and 1 or 5
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

function LuckyRingData:GetItemPointByItemType(type)
	for k,v in pairs(Config.lasso_auto.item) do 
		if v.item_type == type then 
			return v.score
		end 
	end 

	return 0
end


function LuckyRingData:GetItemPointByItemId(item_id)
	for k,v in pairs(Config.lasso_auto.item) do 
		if v.item_id == item_id then 
			return v.score
		end 
	end 

	return 0
end

function LuckyRingData:GetItemCfgByItemID(item_id)
	for k,v in pairs(Config.lasso_auto.item) do 
		if v.item_id == item_id then 
			return v
		end 
	end 

	return nil
end

function LuckyRingData:IsLuckyRingTitle(title_id)
	return title_id == Config.lasso_auto.other[1].title_id
end


function LuckyRingData:EffectSimulate()
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

function LuckyRingData:SimulateEffectCheck(data)
	for k,v in pairs(self.map_point.sim_list) do 
		if v.x == data.x and v.y == data.y then 
			return true
		end 
	end 
    return false
end

-- function LuckyRingData:( ... )
--     -- body
-- end
