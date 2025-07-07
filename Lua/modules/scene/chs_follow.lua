ChsFollow = ChsFollow or BaseClass()

function ChsFollow:__init()
	if ChsFollow.Instance ~= nil then
		Debuger.LogErrorSD("[ChsFollow] attempt to create singleton twice!")
		return
	end
	ChsFollow.Instance = self

	--{key=queue_id,val={id=queue_id scene_ch[](queue_list)}}--按跟随顺序排序，最前面的那个角色会应用外部传进来的数据
	self.queues = {}

	--key=scene_ch ,val=queue_list
	self.ch_to_queuelist = {}
	self.time_handle = TimeHelper:AddRunTimer(BindTool.Bind(self.OnUpdate,self),0.1)	--控制一下判断和执行的间隔
	-- self.scene_pos_cache = {}		--场景与已加载场景不一致的队列ID会被记录在这里，在对应场景加载完成之后设置位置 {key=queue_id,val=scene_id}
	-- self.pos_data_care_handle = SceneMgr:CareLoadedSceneIdChange(BindTool.Bind(self.OnLoadedSceneIdChange,self))
end

function ChsFollow:__delete()
	-- if self.pos_data_care_handle ~= nil then
	-- 	SceneMgr:UncareLoadedSceneIdChange(self.pos_data_care_handle)
	-- 	self.pos_data_care_handle = nil
	-- end
	TimeHelper:CancelTimer(self.time_handle)
	self.time_handle = nil
end

--测试代码，打印当前队列数据的状态
function ChsFollow:QueueToString()
	for qid,qu in pairs(self.queues) do
		local log_str = ""
		for _,ch in ipairs(qu) do
			local ch_na
			if ch ~= nil then
				if ch:Vo() ~= nil then
					ch_na = ch:Vo().name
				else
					ch_na = "VoNil"
				end
			else
				ch_na = "nil"
			end

			log_str = log_str .. "|" .. ch_na
		end
		LogError(qid,log_str)
	end
end
--queue_id 队列ID
--chs scene_ch[]  按队列顺序
--pre_ch 排在哪个角色后面
--post_ch 排在哪个角色前面
--pre_ch，post_ch都为空表示直接插在最后
function ChsFollow:CreateOrAddQueue(queue_id,chs,pre_ch,post_ch)
	local ch_count = 0
	for _,ch in pairs(chs) do
		-- if self.ch_to_queuelist[ch] ~= nil then --这里修改为替换队列
		-- 	if self.ch_to_queuelist[ch].id ~= queue_id then
		-- 		-- self:DestroyQueue(self.ch_to_queuelist[ch].id)
		-- 		self:OnChDelete(ch)
		-- 	else
		-- 		Debuger.LogErrorSD("ChsFollow:AddChToQueue ERROR!ch has added to queue,ch_vo=%s,before_queueid=%d",
		-- 			Debuger.TableToString(ch:Vo()),self.ch_to_queuelist[ch].id)
		-- 		return queue_id
		-- 	end
		-- end
		self:OnChDelete(ch)
		ch_count = ch_count + 1
	end
	if ch_count == 0 then	--如果 没角色则暂时不创建队列数据
		return queue_id
	end
	--自动分配queue_id
	if queue_id == nil then
		for i=-1,-9999,-1 do
			if self.queues[i] == nil then
				queue_id = i
				break
			end
		end
		if queue_id == nil then
			Debuger.LogErrorSD("ChsFollow:AddChToQueue ERROR!queue id can't Alloc!!")
		else
			-- Debuger.LogErrorSD("ChsFollow:AddPetChToQueu id=%d",queue_id)
		end
	end

	-- local names = "|"
	-- for _,c in ipairs(chs) do
	-- 	names = names .. c:Vo().name .. "|"
	-- end
	-- LogError("ChsFollow:CreateOrAddQueue",queue_id,names,pre_ch and pre_ch:Vo().name or "NIL",post_ch and post_ch:Vo().name or "NIL")

	local queue_list = self.queues[queue_id]
	if queue_list == nil then		--新建queue
		queue_list = {}
		queue_list.id = queue_id
		self.queues[queue_id] = queue_list
		-- LogError("ChsFollow:CreateOrAddQueue,add QueueList",queue_id, queue_list)
	end

	local insert_i = nil
	if pre_ch ~= nil or post_ch ~= nil then
		for i,ch in ipairs(queue_list) do
			if ch == pre_ch then
				insert_i = i + 1
				break
			elseif ch == post_ch then
				insert_i = i
				break
			end
		end
	end
	if insert_i == nil then  --没有前置角色，直接按顺序插入在队列最后
		for _,insert_ch in ipairs(chs) do
			table.insert(queue_list,insert_ch)
			self.ch_to_queuelist[insert_ch] = queue_list
		end
	else   --插入对应位置
		for _,insert_ch in ipairs(chs) do
			table.insert(queue_list,insert_i,insert_ch)
			self.ch_to_queuelist[insert_ch] = queue_list
			insert_i = insert_i + 1
		end
	end
	-- local ch_count =  #queue_list
	-- if ch_count >= 2 then
	-- 	for i=2,#queue_list do
	-- 		self:performFollow(queue_list[i-1],queue_list[i],queue_id)
	-- 	end
	-- elseif ch_count == 1 then
	-- 	self:CheckScenePosValid(queue_list[1],queue_id)
	-- end

	return queue_id
end

--添加一个角色入队列
function ChsFollow:AddChToQueue(queue_id,ch,pre_ch,post_ch)
	-- LogError("ChsFollow:AddChToQueue===",queue_id,ch:Vo().name,pre_ch and pre_ch:Vo().name or "nil",post_ch and post_ch:Vo().name or "nil")
	self:OnChDelete(ch)

	-- if self.ch_to_queuelist[ch] ~= nil then
	-- 	Debuger.LogErrorSD("ChsFollow:AddChToQueue ERROR!ch has added to queue,ch_vo=%s,before_queueid=%d",
	-- 		Debuger.TableToString(ch:Vo()),self.ch_to_queuelist[ch].id)
	-- 	return
	-- end
	local queue_list = self.queues[queue_id]
	if queue_list == nil then	--没有队列时创建一个
		-- Debuger.LogErrorSD("ChsFollow:AddChToQueue ERROR!queue not exit,ID=%d",queue_id)
		queue_list = {}
		queue_list.id = queue_id
		self.queues[queue_id] = queue_list
	end
	local insert_i = nil
	if pre_ch ~= nil or post_ch ~= nil then
		for i,ch in ipairs(queue_list) do
			if ch == pre_ch then
				insert_i = i + 1
				break
			elseif ch == post_ch then
				insert_i = i
				break
			end
		end
	end
	if insert_i == nil then  --没有前置角色，直接按顺序插入在队列最后
		table.insert(queue_list,ch)
	else   --插入对应位置
		table.insert(queue_list,insert_i,ch)
	end
	self.ch_to_queuelist[ch] = queue_list
	-- if insert_i ~= nil and insert_i > 1 then
	-- 	self:performFollow(queue_list[insert_i-1],ch,queue_id)
	-- 	-- LogError("!!!!!!!!!!!!!!")
	-- else
	-- 	self:CheckScenePosValid(ch,queue_id)
	-- end
end

--从队列中移除一个角色
function ChsFollow:OnChDelete(ch, id)
	local queue_list = self.ch_to_queuelist[ch]
	if queue_list == nil then
		return
	end
	for i,qch in ipairs(queue_list) do
		if qch == ch and (id == nil or id == queue_list.id) then
			table.remove(queue_list,i)
			-- LogError("#queue_list====",#queue_list)
			if #queue_list == 0 then	--队列销毁
				self.queues[queue_list.id] = nil
				-- self.scene_pos_cache[queue_list.id] = nil
				-- LogError("ChsFollow:OnChDelete,Remove QueueList", queue_list)
			end
			self.ch_to_queuelist[ch] = nil
			-- LogError("ChsFollow:OnChDelete===",ch ~= nil and ch:Vo().name or "nil")
			return
		end
	end
end

function ChsFollow:DestroyQueue(queue_id)
	local queue_list = self.queues[queue_id]
	if queue_list == nil then
		return
	end
	self.queues[queue_id] = nil
	-- self.scene_pos_cache[queue_id] = nil
	-- LogError("ChsFollow:DestroyQueue,Remove QueueList", queue_list)
	for _,ch in ipairs(queue_list) do
		self.ch_to_queuelist[ch] = nil
	end
end

function ChsFollow:IsInQueue(ch)
	return self.ch_to_queuelist[ch] ~= nil
end

function ChsFollow:IsQueueHead(ch)
	local queue_list = self.ch_to_queuelist[ch]
	if queue_list == nil then
		return false
	end
	-- local log_strs = {}
	-- for i=1,#queue_list do
	-- 	table.insert(log_str,queue_list[i].vo.name)
	-- end
	-- LogError("IsQueueHead===",queue_list)
	return queue_list[1] == ch
end




function ChsFollow:OnUpdate()
	for _,queue_list in pairs(self.queues) do
		for i=2,#queue_list do
		-- LogError("ChsFollow:OnUpdate I=",i)
			self:performFollow(queue_list[i-1],queue_list[i],queue_list.id)
		end
	end
end

--检查角色是否可以设置位置，如果不行的话就加入位置设置缓存
-- function ChsFollow:CheckScenePosValid(ch,queue_id)
-- 	local scene_id = ch and ch:Vo() and ch:Vo().scene_id or nil
-- 	if scene_id == nil then return true end
-- 	if scene_id ~= SceneMgr:LoadedSceneId() then
-- 		-- self.scene_pos_cache[queue_id] = scene_id
-- 		return false
-- 	else
-- 		return true
-- 	end
-- end

--执行跟随逻辑之前会先判断角色的场景数据是否与当前场景吻合
function ChsFollow:performFollow(front_ch,back_ch,queue_id)
	-- LogError("ChsFollow",front_ch.vo.name,back_ch.vo.name)
	-- if front_ch == nil or back_ch == nil then return end
	-- if self.scene_pos_cache[queue_id] ~= nil then
	-- 	return
	-- end
	-- if self:CheckScenePosValid(back_ch,queue_id) == false then
	-- 	return
	-- end
    self:performFollowInternal(front_ch,back_ch)
end

function ChsFollow:performFollowInternal(front_ch,back_ch)
	-- if front_ch:IsInAnim(ChAnimType.Run) then	--前面的角色在跑步
    --     local petPos = back_ch:GetPos()
	-- 	local pos = front_ch:GetPos()
	-- 	local is_ec = nil ~= back_ch.vo and SceneObjType.EscortCar == back_ch.vo.obj_type
	-- 	local follow_fly = is_ec and 200 or 100
	-- 	local follow_dis = is_ec and 49 or 9
	-- 	local follow_v2 = is_ec and 7 or 2
    --     local distance = GameMath.GetDistance(pos.x, pos.z, petPos.x, petPos.z, false)
	-- 	local v2 = GameMath.DistancePos(petPos.x, petPos.z, pos.x, pos.z, follow_v2)
	-- 	if distance > follow_fly then
	-- 		back_ch:SetPos(GameScene.SamplePos(pos))
    --     elseif distance > follow_dis then
	-- 		-- local y = SceneCtrl.Instance:CurScene():GetPosHeight(v2.x, v2.y, pos.y)
	-- 		local p = GameScene.SamplePos(Vector3.New(v2.x, pos.y, v2.y))
    --         -- if y == 0 then  --边缘没有取到高的时候，高设置成与跟随者一样
	-- 		-- 	y = pos.y
	-- 		-- 	back_ch:MoveTo(Vector3.New(v2.x, y, v2.y), false)
	-- 		-- 	return
	-- 		-- end
	-- 		-- LogError("follow",Vector3.New(v2.x, y, v2.y))
	-- 		-- back_ch:MoveTo(Vector3.New(v2.x, y, v2.y), false)
	-- 		back_ch:MoveTo(p, false)
    --     else
    --         if distance <= 0.6 then
    --             back_ch:StopMove()
    --         end
    --     end
    -- else
    --     local petPos = back_ch:GetPos()
    --     local pos = front_ch:GetPos()
    --     if GameMath.GetDistance(pos.x, pos.z, petPos.x, petPos.z, false) > 100 then
	-- 		local obj_for = Quaternion.AngleAxis(front_ch:Vo().dir_y or 45, Vector3.up) * Vector3.forward
	-- 		local p = pos - obj_for * 2
	-- 		local scene_id = back_ch:Vo().scene_id and back_ch:Vo().scene_id or front_ch:Vo().scene_id
	-- 		back_ch:SetPos(GameScene.SamplePos(p))
    --         back_ch:StopMove()
    --     end
	-- end

	--确保前后的角色都位置初始化完成了，才会进入的位置的计算同步
	if back_ch:Vo() == nil or front_ch:Vo() == nil then
		return
	end
	--镖车等客户端对象vo里没有场景id
	if (back_ch:Vo().scene_id ~= nil and back_ch:Vo().scene_id ~= SceneMgr:LoadedSceneId()) or 
		(front_ch:Vo().scene_id ~= nil and front_ch:Vo().scene_id ~= SceneMgr:LoadedSceneId()) then
		return
	end
	if back_ch.cache_pos_scene_id ~= 0 or front_ch.cache_pos_scene_id ~= 0 then
		return 
	end
	
	local petPos = back_ch:GetPos()
	local pos = front_ch:GetPos()
	local follow_fly_unsqrt = 80					--距离 大于这个值会直接飞到角色背后
	local min_dis_unsqrt		--两个角色之间最小距离，如果接近这个距离或者超过就会把角色速度置0,且俩角色的距离如果超过这个值则后面的角色会开始跟随
	local max_dis_unsqrt		--两个角色之间的最大距离，如果大于这个值角色就会加速直到追到最小值

	if back_ch:Type() == SceneObjType.EscortCar then
		min_dis_unsqrt = 49
		max_dis_unsqrt = 64
	elseif back_ch:Type() == SceneObjType.Pet then
		min_dis_unsqrt = 9
		max_dis_unsqrt = 12.25
	elseif back_ch:Type() == SceneObjType.ShengQi then
		min_dis_unsqrt = 1.5
		max_dis_unsqrt = 2
	else
		min_dis_unsqrt = 5
		max_dis_unsqrt = 6.25
	end	
	local distance_unsqrt = GameMath.GetDistance(pos.x, pos.z, petPos.x, petPos.z, false)
	local default_speed = back_ch:Vo().move_speed

	if distance_unsqrt > follow_fly_unsqrt then			--距离过大，直接飞
		local front_scene_id = front_ch:Vo() and front_ch:Vo().scene_id
		--LogError("FRONT POS=",front_ch.vo.scene_id,front_ch.cache_pos_scene_id,front_ch.cache_pos_x,front_ch.cache_pos_y,pos)
		--LogError("FOLLOW MainP POS=",MainPlayer.vo.scene_id,MainPlayer.cache_pos_scene_id,MainPlayer.cache_pos_x,MainPlayer.cache_pos_y)
		if front_scene_id ~= nil then
			back_ch:SetScenePos(front_scene_id,pos.x, pos.z,false, pos.y)
		else
			local fly_pos = GameMath.DistancePos(petPos.x, petPos.z, pos.x, pos.z, 1.2)
			back_ch:SetPos(GameScene.SamplePos(Vector3.New(fly_pos.x, pos.y, fly_pos.y)))
		end
		return
	end
	local need_moveto = false
	if back_ch:IsInAnim(ChAnimType.Run) then	--正在跟随状态，判断距离是否需要减速或者加速
		if distance_unsqrt > max_dis_unsqrt then
			back_ch:SetMoveSpeed(default_speed * 1.1)
			need_moveto = true
		elseif distance_unsqrt < min_dis_unsqrt then
			if front_ch:IsInAnim(ChAnimType.Run) then		--离得太近了，停下等一会前面的人
				if distance_unsqrt < (min_dis_unsqrt * 0.8) then
					back_ch:SetMoveSpeed(default_speed*0.6)
					need_moveto = true
				else
					back_ch:SetMoveSpeed(default_speed)
					need_moveto = true
				end
			else
				back_ch:SetMoveSpeed(default_speed)
				back_ch:StopMove()
			end
		else
			back_ch:SetMoveSpeed(default_speed)
			need_moveto = true
		end
	else
		if distance_unsqrt >= min_dis_unsqrt then
			need_moveto = true
		else
			local distance_height = (pos.y - petPos.y) * (pos.y - petPos.y)
			if distance_height > 4 then
				need_moveto = true
			end
		end
	end
	if need_moveto then
		local res = back_ch:MoveTo(pos)
		if res == 1 then
			local fly_pos = GameMath.DistancePos(petPos.x, petPos.z, pos.x, pos.z, 1.2)
			back_ch:SetPos(GameScene.SamplePos(Vector3.New(fly_pos.x, pos.y, fly_pos.y)))
		end
	end
end

-- --进入队列的角色场景位置缓存处理在都在这个函数
-- function ChsFollow:OnLoadedSceneIdChange()
-- 	-- LogError("????",self.scene_pos_cache)
-- 	local remove_queue_ids = nil
-- 	for _,queue_list in pairs(self.queues) do
-- 		if #queue_list >= 1 then		--所有队头同步位置
-- 			queue_list[1]:ApplyCacheScenePos()
-- 		end
-- 	end
-- 	for queue_id,scene_id in pairs(self.scene_pos_cache) do
-- 		if scene_id == SceneMgr:LoadedSceneId() then
-- 			if remove_queue_ids == nil then remove_queue_ids = {} end
-- 			table.insert(remove_queue_ids,queue_id)
-- 			local queue_list = self.queues[queue_id]
-- 			for i=2,#queue_list do
-- 				self:performFollowInternal(queue_list[i-1],queue_list[i])
-- 			end
-- 		end
-- 	end
-- 	if remove_queue_ids ~= nil then
-- 		for _,qid in pairs(remove_queue_ids) do
-- 			self.scene_pos_cache[qid] = nil
-- 		end
-- 	end
-- end