FollowData = FollowData or BaseClass()

function FollowData:__init()
	self:FollowData()
end

function FollowData:FollowData()
	self:FollowList()
	self.follow_distance = 7
end

function FollowData:FollowList()
	self.follow_list = {}--key:queue_id value={id=queue_id,members=role_id[]}
	-- self.follows = {}	--key:role_id value: queue_id*10(如果是队长=queue_id*10+1)

	self.roleid_to_follow = {} --key:role_id value:{id=queue_id,members=role_id[]}
end

-- function FollowData.FollowsValue(id, is_head)
-- 	local value = id * 10
-- 	value = is_head and value + 1 or value
-- 	return value
-- end

-- function FollowData.FollowId(follows_value)
-- 	return math.floor(follows_value / 10)
-- end

function FollowData:UpdateFollowList(follow_info)
	if 0 == follow_info.id then return end
	--LogError("FollowData:UpdateFollowList",follow_info)

	local old_fi = self.follow_list[follow_info.id]
	if old_fi ~= nil then	--clean cache
		for _, member in pairs(old_fi.members) do
			self.roleid_to_follow[member] = nil
		end
	end


	self.follow_list[follow_info.id] = follow_info


	local ch_queue = {}
	for _, member in ipairs(follow_info.members) do
		self.roleid_to_follow[member] = follow_info
		local ch = SceneCtrl.Instance:Objs():GetObjectByObjId(member)
		-- LogError("UpdateFollowList",member)
		if ch ~= nil then
			table.insert(ch_queue,ch)
		end
	end
	ChsFollow.Instance:DestroyQueue(follow_info.id)
	ChsFollow.Instance:CreateOrAddQueue(follow_info.id,ch_queue)

	-- self:CheckFollowHead(follow_info)
end

function FollowData:AddFollowInfo(follow_info)
	local check_fi = self.roleid_to_follow[follow_info.member]
	if check_fi ~= nil then
		if follow_info.id == check_fi.id then
			return
		else
			-- Debuger.LogErrorSD("FollowData:AddFollowInfo ERROR!member has already added to follow,mem=%d,fid=%d,exist_fid=%d",
			-- 	follow_info.member,follow_info.id,check_fi.id)
			self:RemoveFollowInfo({id = check_fi.id,
				member = follow_info.member,is_del = false})		--如果已经存在，移除老的跟随信息
		end

		-- return
	end
	local fi = self.follow_list[follow_info.id]
	if fi then
		local member = follow_info.member
		self.roleid_to_follow[member] = fi
		table.insert(fi.members, member)
		local ch = SceneCtrl.Instance:Objs():GetObjectByObjId(member)
		if ch ~= nil then
			ChsFollow.Instance:AddChToQueue(follow_info.id,ch)
		end
	else
		Debuger.LogErrorSD("FollowData:AddFollowInfo ERROR!follow_id not EXIT!follow_id=%d",follow_info.id)
	end
end

function FollowData:RemoveFollowInfo(follow_info)
	local fi = self.follow_list[follow_info.id]
	if fi then
		if 1 == follow_info.is_del then	--队伍解散
			for _, member in pairs(fi.members) do
				self.roleid_to_follow[member] = nil
			end
			self.follow_list[follow_info.id] = nil
			ChsFollow.Instance:DestroyQueue(follow_info.id)
		else
			-- local p = 0
			for pos, member in pairs(fi.members) do
				if member == follow_info.member then
					table.remove(fi.members, pos)
					break
				end
			end
			self.roleid_to_follow[follow_info.member] = nil
			local ch = SceneCtrl.Instance:Objs():GetObjectByObjId(follow_info.member)
			if ch ~= nil then
				ChsFollow.Instance:OnChDelete(ch, follow_info.id)
			end
		end
	end
end

-- function FollowData:CheckFollowHead(follow_info)
-- 	local roles = VoMgr:RoleVoList()
-- 	local head
-- 	for _, member in pairs(follow_info.members) do
-- 		local vo = roles[member]
-- 		if vo then
-- 			head = head or member
-- 		end
-- 	end
-- 	if head then
-- 		for _, member in pairs(follow_info.members) do
-- 			self.follows[member] = FollowData.FollowsValue(follow_info.id, head == member)
-- 		end
-- 		follow_info.head = head
-- 	end
-- end

-- function FollowData:CheckFollowHeadById(obj_id)
-- 	local value = self.follows[obj_id] 
-- 	if value then
-- 		self:CheckFollowHead(self.follow_list[FollowData.FollowId(value)])
-- 	end
-- end

-- function FollowData:IsFollowHead(obj_id)
-- 	return self.follows[obj_id] and 1 == self.follows[obj_id] % 10
-- end

--角色创建时，找到这个角色前面的角色并加入到Follow队列中
function FollowData:FollowInit(obj)
	local vo = obj.vo
	if nil == vo then return end
	local obj_id = vo.obj_id
	local fi = self.roleid_to_follow[obj_id]
	if fi == nil then
		return
	end

	local ch_p = nil
	for i=1,#fi.members do
		if fi.members[i] == obj_id then
			ch_p = i
			break
		end
	end
	if ch_p == nil then
		Debuger.LogErrorSD("FollowData:FollowInit ERROR!member in follow data but can't find!obj_id=%d,follow_id=%d",obj_id,fi.id)
		return
	end
	--找到此角色前面的角色插到他后面
	local pre_ch = nil
	local post_ch = nil
	for i=ch_p-1,1,-1 do
		local me_id = fi.members[i]
		
			pre_ch = SceneCtrl.Instance:Objs():GetObjectByObjId(me_id)
			
		if pre_ch ~= nil then
			break
		end
	end
	if pre_ch == nil then
		for i = ch_p+1,#fi.members do
			local me_id = fi.members[i]
			
				post_ch = SceneCtrl.Instance:Objs():GetObjectByObjId(me_id)
				
			if post_ch ~= nil then
				break
			end			
		end
	end
	ChsFollow.Instance:AddChToQueue(fi.id,obj,pre_ch,post_ch)

	-- if nil == self.follows[obj_id] or 0 == self.follows[obj_id] % 10 then return end
	-- local fi = self.follow_list[FollowData.FollowId(self.follows[obj_id])]
	-- local vo = VoMgr:RoleVoList()[fi.head]
	-- local p = 0
	-- for pos, member in pairs(fi.members) do
	-- 	if member == obj_id then
	-- 		p = pos
	-- 	end
	-- end
	-- self:FollowVo(vo, obj, p)
end

-- function FollowData:Follow(protocol, roles)
-- 	local fi = self.follow_list[FollowData.FollowId(self.follows[protocol.obj_id])]
-- 	for pos, member in pairs(fi.members) do
-- 		local obj = roles[member]
-- 		if obj then self:FollowObj(protocol, obj, pos) end
-- 	end
-- end

-- function FollowData:FollowObj(protocol, obj, pos)
-- 	local end_x = protocol.pos_x
-- 	local end_y = protocol.pos_y
-- 	local distance = protocol.distance - (pos - 1) * self.follow_distance 
-- 	if distance > 0 then
-- 		end_x = end_x + (math.cos(protocol.dir_y) * distance)
-- 		end_y = end_y + (math.sin(protocol.dir_y) * distance)
-- 	end
-- 	local pos = SceneCtrl.Instance:CurScene():PosToClint(end_x, end_y)
-- 	obj:MoveTo(pos, false)
-- 	obj:SetVoPos(end_x, end_y)
-- end

-- function FollowData:FollowVo(vo, obj, pos)
	-- local end_x = vo.x
	-- local end_y = vo.y
	-- local distance = - (pos - 1) * self.follow_distance 
	-- end_x = end_x + (math.cos(vo.dir_y) * distance)
	-- end_y = end_y + (math.sin(vo.dir_y) * distance)
	-- obj:SetVoPos(end_x, end_y)
-- end