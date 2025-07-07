LBMapList = LBMapList or DeclareMono("LBMapList")

function LBMapList:LBMapList()
	self.item_list = {}

	self.clone_obj = nil
	self.data = nil
	self.data_t = nil

	self.is_wait_ready = false
	self.cahce_index = nil

	self.items_cache = {}
	self.step_set_data_timer = nil
	self.setuped_index = nil
	self.total_count = 0
	self.key_list = nil	

	self.wait_setup_item = nil

	self.setup_complete = false

	self.select_cmd_cache = {}

	self.part_record = -1
	--默认滑动方向 1横向 2纵向
	self.dirc = 1
	--计算得到的content的宽高(一般用于需要同帧刷新位置但Content的size还没有更新的时候，通过子物体的LayoutElement)
	--要求子物体上挂载了LayoutElement并命名为ItemElement（具体参考Role_Skill预设的RoleViewSkillInfoItem）
	--如果子物体的宽高是固定长度则不用考虑
	self.ContentCountSize = Vector2.zero

    self.total_need_num = 0
    Runner.Instance:AddRunObj(self, 8)
	self.cache_list = {}
	self.wait_list = {}
end

function LBMapList:OnDestroy()
	Runner.Instance:RemoveRunObj(self)
end 

function LBMapList:Update()
	self:OnDragContent()
end

function LBMapList:BuildList()
    local length = math.sqrt(self.total_count)
    UH.SizeDelta(self.ContentParent, Vector2.New(500*length,500*length))

    self.pos_list = {}
    for i = 1,self.total_count do 
        local p_y = math.floor(i / length)
        local p_x = math.floor(i % length)

        local pos_x =  500 * (p_x == 0 and length or p_x) - 1500
        local pos_y =  - 500 * (p_x ==0 and p_y or p_y + 1) + 1500
        local vo = {}
        vo.pos = Vector3.New(pos_x,pos_y,0)
        vo.index = i
        vo.conorn = {}
        vo.conorn[1] = Vector3.New(pos_x - 250,pos_y + 250,0)
        vo.conorn[2] = Vector3.New(pos_x + 250,pos_y + 250,0)
        vo.conorn[3] = Vector3.New(pos_x + 250,pos_y - 250,0)
        vo.conorn[4] = Vector3.New(pos_x - 250,pos_y - 250,0)

        table.insert(self.pos_list,vo)
    end 

	local p_x = math.floor(self.saw_frame.sizeDelta.x / 500)
	local p_y = math.floor(self.saw_frame.sizeDelta.y / 500)

	local s_x = math.floor(self.saw_frame.sizeDelta.x % 500)
	local s_y = math.floor(self.saw_frame.sizeDelta.y % 500)

	local edge_x = s_x > 0 and 2 or 1
	local edge_y = s_y > 0 and 2 or 1

    self.total_need_num = math.floor(((p_x + edge_x) * (p_y + edge_y))*1.4)
end

function LBMapList:CheckReady()
	if self.clone_obj ~= nil then
		return
	else
		if self.ContentParent == nil then
			self.ContentParent = self.gameObject
		end
		if self.StepCount == nil then
			self.StepCount = 3
		end
		self.clone_obj = false
		if self.ItemMono ~= nil then
			self.clone_obj = self.ItemMono.gameObject
		elseif not self.CacheLoad then
			ResMgr:LoadGameObject(self.ItemPath,function(obj)
				self.clone_obj = obj
				end,false,false)
		else
			GameObjPool:RegisterPool(self.ItemPath,function(error)
					self.clone_obj = true
				end,false)
		end
	end
end

function LBMapList:ItemTypeName()
	if self.ItemMono ~= nil then
		return self.ItemMono:GetTypeName()
	else
		return self.TypeName
	end
end

function LBMapList:SetData(data)
	if data == nil then
		return
	end
	self.setup_complete = false
	self.data = data
	self.total_count = 0
	self.ContentCountSize = Vector2.zero
	self.data_t = {}
	self.key_list = {}
	self.select_cmd_cache = {}

	for k, v in pairs(self.data) do
		if type(k) == "number" then
			self.data_t[v] = k
			self.total_count = self.total_count + 1
			table.insert(self.key_list,k)
		end
	end
	table.sort(self.key_list)
	self:CheckReady()
    self:BuildList()
    local item_obj = nil
    for i = 1,self.total_need_num do 
        item_obj = self.clone_obj:Clone(self.ContentParent,false)
        item_obj:SetActive(true)

        local cache_item = item_obj:GetLuaComponent(self:ItemTypeName())

        self.item_list[i] = cache_item
        -- self:SetDataToMonoItem(cache_item,da)    
    end 
    

	-- if self.clone_obj ~= nil and self.clone_obj ~= false then
	-- 	self:SetupData()
	-- else
	-- 	if not self.is_wait_ready then
	-- 		self.is_wait_ready = true
	-- 		Runner.Instance:RunUntilTrue(function()
	-- 				local ready = self.clone_obj ~= nil and self.clone_obj ~= false
	-- 				if ready then
	-- 					self.is_wait_ready = false
	-- 					self:SetupData()
	-- 				end
	-- 				return ready
	-- 			end)
	-- 	end
	-- end

    self:OnDragContent()
end 

function LBMapList:SetDataToMonoItem(mono_item,data)
	mono_item:SetData(data)
end

function LBMapList:OnDragContent()
    local field_x_min = -self.saw_content.localPosition.x - (self.saw_frame.sizeDelta.x /2)
    local field_x_max = -self.saw_content.localPosition.x + (self.saw_frame.sizeDelta.x /2)
    local field_y_min = -self.saw_content.localPosition.y - (self.saw_frame.sizeDelta.x /2)
    local field_y_max = -self.saw_content.localPosition.y + (self.saw_frame.sizeDelta.x /2)

    local act_list = {}
	local act_num = 0
    for k,v in pairs(self.pos_list) do
        local flag = false 
        for i,j in pairs(v.conorn) do 
            if j.x >= field_x_min and j.x <= field_x_max and j.y >= field_y_min and j.y <= field_y_max then 
                flag = true
                break
            end 
        end  

        if flag then 
            act_list[v.index] = v
			act_num = act_num + 1
        end 
    end 

	local str_ = ""
	for k,v in pairs(act_list) do 
		str_ = str_..v.index.." "
	end 

	if #self.cache_list == 0 then 
		self.cache_list = {}
		self.wait_list = {}
		for i = 1,self.total_need_num do 
			local flag_pos = act_num > 0 
			self.item_list[i]:SetObjActive(flag_pos)
			if flag_pos then 
				for k,v in pairs(act_list) do 
					UH.LocalPosG(self.item_list[i].gameObject, v.pos)
					self:SetDataToMonoItem(self.item_list[i],self.data[v.index])   

					local vo = {}
					vo.item_index = i -- 实例序列
					vo.pos_index = v.index -- 位置序列
					vo.pos_info = v -- 位置信息
					self.cache_list[vo.item_index] = vo
					act_num = act_num - 1

					act_list[k] = nil
					break
				end 
			else 
				local vo = {}
				vo.item_index = i
				table.insert(self.wait_list, vo)
			end 
		end 
	else 

		local change_pos = {}
		local keep_pos = {}
		local keep_item_pos = {}
		-- 该位置需要显示但没有被缓存	
		for k,v in pairs(self.cache_list) do 
			-- 该位置不需要显示但已在缓存中
			if act_list[v.pos_index] == nil then 

				change_pos[v.pos_index] = v
			-- 该位置需要显示且已经在缓存中
			elseif act_list[v.pos_index] ~= nil then 
				keep_pos[v.pos_index] = v
				keep_item_pos[v.item_index] = v
			end  
		end 
		
		self.cache_list = {}
		for k,v in pairs(act_list) do 
			local is_keep = keep_pos[v.index]
			local is_change = change_pos[v.index]
			local vo = {}
			-- 引入实例
			if not is_keep and not is_change and #self.wait_list > 0 then 
				self.item_list[self.wait_list[#self.wait_list].item_index]:SetObjActive(true)
				UH.LocalPosG(self.item_list[self.wait_list[#self.wait_list].item_index].gameObject, v.pos)
				self:SetDataToMonoItem(self.item_list[self.wait_list[#self.wait_list].item_index],self.data[v.index])   

				vo.item_index = self.wait_list[#self.wait_list].item_index
				vo.pos_index = v.index
				vo.pos_info = v
				self.cache_list[self.wait_list[#self.wait_list].item_index] = vo

				table.remove(self.wait_list,#self.wait_list)				
			elseif is_change then 
				self.item_list[change_pos[v.index].item_index]:SetObjActive(true)
				UH.LocalPosG(self.item_list[change_pos[v.index].item_index].gameObject, v.pos)
				self:SetDataToMonoItem(self.item_list[change_pos[v.index].item_index],self.data[v.index])   

				vo.item_index = change_pos[v.index].item_index
				vo.pos_index = v.index
				vo.pos_info = v
				self.cache_list[change_pos[v.index].item_index] = vo
			elseif is_keep then
				vo.item_index = keep_pos[v.index].item_index
				vo.pos_index = v.index
				vo.pos_info = v
				self.cache_list[keep_pos[v.index].item_index] = vo
			end
		end

		self.wait_list = {}
		for i = 1,self.total_need_num do 
			if self.cache_list[i] == nil then 
				local vo = {}
				vo.item_index = i
				table.insert(self.wait_list,vo)

				self.item_list[i]:SetObjActive(false)
			end 
		end 
	end 

    -- for i = 1,self.total_need_num do 
	-- 	self.item_list[i]:SetObjActive(act_list[i] ~= nil)
    --     if act_list[i] ~= nil then 
            
    --         UH.LocalPosG(self.item_list[i].gameObject, act_list[i].pos)
    --         self:SetDataToMonoItem(self.item_list[i],self.data[act_list[i].index])   
    --     end 
    -- end 
end

