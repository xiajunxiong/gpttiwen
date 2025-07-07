function LandsBetweenMapModel:InitSpPic()
    self.sp_cache = {}
    self.force_sp_cache = {}
    self.cache_visible = nil
    -- self.init_force_sp
end

-- 刷新特殊地块
function LandsBetweenMapModel:FlushSpPic(visible_list)
    for k,v in pairs(visible_list) do
        for i , j in pairs(self.data:ReleaseVisible(v)) do 
            local vo = {}
            vo.x = j.x
            vo.y = j.y
            vo.group_id = v.group_id
        end 
    end 
    local map_list = {}
    for k,v in pairs(visible_list) do 
        local grid_list = self.data:ReleaseVisible(v)
        local vo = {}
        vo.group_id = v.group_id
        vo.list = grid_list
        table.insert(map_list,vo)
    end 

    for k,v in pairs(self.sp_cache) do 
        v:SetActive(false)
    end 

    local index = 1

    for k,v in pairs(map_list) do 
        for i,j in pairs(v.list) do 
            local sp_res_param = self.data:GetResOfSpPicByLinkMark(j.link_mark)
            if sp_res_param ~= nil then 
                if sp_res_param.enable_func == nil or (sp_res_param.enable_func ~= nil and sp_res_param.enable_func()) then 

                    if self.sp_cache[index] ~= nil then
                        local item_mono = self.sp_cache[index]:GetLuaComponent(self.MapResSpPic:GetTypeName())
                        item_mono:SetData(sp_res_param)
                        item_mono:CreateRes()

                        self.sp_cache[index]:SetActive(true)
                        self.sp_cache[index].name = j.link_mark .. "|" .. v.group_id.."|"..sp_res_param.faction.."|"..sp_res_param.name
                        UH.LocalPosG(self.sp_cache[index], j.pos)
                    else 
                        local item_obj = self.MapResSpPic.gameObject:Clone(self.SpParent)
                        item_obj.name = j.link_mark .. "|" .. v.group_id.."|"..sp_res_param.faction.."|"..sp_res_param.name
                        UH.LocalPosG(item_obj, j.pos)
                        item_obj:SetActive(true)
                        
                        local item_mono = item_obj:GetLuaComponent(self.MapResSpPic:GetTypeName())
                        item_mono:SetData(sp_res_param)
                        item_mono:CreateRes()
                        self.sp_cache[index] = item_obj
                    end 
                    index = index + 1
                end
            end 
        end 
    end 
end 

function LandsBetweenMapModel:FlushForce()
    if not ViewMgr:IsOpen(LandsBetweenMainView) then 
        return 
    end 

    if self.MapResSpPic == nil then
        return 
    end

    local flush_list = {}
    --self.force_sp_cache
    local function checkInVisible(link_mark)
        local static = self.data:GetMapGridStaticInfo(link_mark)
        if static == nil then 
            return false
        end 
        for k,v in pairs(self.cache_visible or {}) do 
            if v.group_id == static.visible_area then 
                return true
            end 
        end 
        return false
    end

    for k,v in pairs(self.data.sp_pic_mark) do
        if v.enable_func == nil or (v.enable_func ~= nil and v.enable_func()) then 
            if self.force_sp_cache[k] == nil then 
                local force_pos = self.data:ChangePosFromWhereByLinkMark(v.link_mark)
    
                local item_obj = self.MapResSpPic.gameObject:Clone(self.SpParent)
                item_obj.name = v.link_mark .."|"..v.faction.."|"..v.name
                UH.LocalPosG(item_obj, force_pos)
    
                local item_mono = item_obj:GetLuaComponent(self.MapResSpPic:GetTypeName())
                item_mono:SetData(v)
                item_mono:CreateRes()
                self.force_sp_cache[k] = item_obj
            else
                local item_mono = self.force_sp_cache[k]:GetLuaComponent(self.MapResSpPic:GetTypeName())
                item_mono:SetData(v)
                item_mono:CreateRes()
            end 
            if checkInVisible(k) then 
                self.force_sp_cache[k]:SetActive(true)
            end 
            flush_list[k] = 1    
        end 
    end 

    for link_mark,obj in pairs(self.force_sp_cache) do 
        if flush_list[link_mark] == nil then 
            self.force_sp_cache[link_mark]:SetActive(false)
        end 
    end 
end

-- 新资源加载的初始化
function LandsBetweenMapModel:InitForceRes()
    self.StepCount_T = 100
    self.StepFrameSpace_T = 1

    self.step_set_data_timer_T = nil
    self.part_record_T = -1

    self.StepCount_F = 1000
    self.StepFrameSpace_F = nil

    self.step_set_data_timer_F = nil
    self.part_record_F = -1

    self.StepCount_C = 1
    self.StepFrameSpace_C = 1

    self.step_set_data_timer_C = nil
    self.part_record_C = -1
end

function LandsBetweenMapModel:LoadMapResTotalForceComplete()
    self:LoadMapResForce()
end 

function LandsBetweenMapModel:LoadMapResForceComplete()
    self:LoadMapResForceCreate()
end 

-- 新资源加载方法
function LandsBetweenMapModel:LoadMapResTotalForce() 
    local full_visible = self.data:GetFullVisible()

    self.part_record_T = -1
    self.setuped_index_T = 0

    self.total_list = full_visible

    -- 进行刷新前会删除之前发起的资源加载
    if self.step_set_data_timer_T then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_T)
        self.step_set_data_timer_T = nil
    end

    self.full_map_res = {}
    self.wait_res = {}
    self.step_set_data_timer_T = Runner.Instance:RunUntilTrue(BindTool.Bind(self.stepSetupItemsTotal,self))    
end 

function LandsBetweenMapModel:stepSetupItemsTotal()
    if self:Destroyed() then
		return true
	end

    -- 完成 结束循环
    if self.setuped_index_T > table.nums(self.total_list) then	--分步加载完成后延一帧再通知外部加载完成
        self.step_set_data_timer_T = nil

        self:LoadMapResTotalForceComplete()
        return true
    end	

    -- 隔帧操作
    if self.StepFrameSpace_T ~= nil and self.StepFrameSpace_T > 1 then
        if self.part_record_T >= 0 then
            if self.part_record_T >= self.StepFrameSpace_T then
                self.part_record_T = 0
            else
            self.part_record_T = self.part_record_T + 1
                return false
            end
        else
            self.part_record_T = 0
        end
    end 

    local count = 0
    while true do
		count = count + 1
		self.setuped_index_T = self.setuped_index_T + 1
		if self.setuped_index_T > table.nums(self.total_list) then
			return false
		end
        -- 生成逻辑
        local grid_list = self.data:ReleaseVisible(self.total_list[self.setuped_index_T])
        for k,v in pairs(grid_list) do  
            if MapResOutFloor[v.map_show.map_stamp] == 1 then 
                table.insert(self.full_map_res, v)
            end 
        end

		if self.StepCount_T > 0 and count >= self.StepCount_T then
			return false
		end
	end
end 

function LandsBetweenMapModel:LoadMapResForce()
    self.part_record_F = -1
    self.setuped_index_F = 0

    -- self.flush_list = create_list
    -- self.full_map_res

    -- 进行刷新前会删除之前发起的资源加载
    if self.step_set_data_timer_F then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_F)
        self.step_set_data_timer_F = nil
    end

    self.step_set_data_timer_F = Runner.Instance:RunUntilTrue(BindTool.Bind(self.stepSetupItemsFlush,self))
end 

function LandsBetweenMapModel:stepSetupItemsFlush()
    if self:Destroyed() then
		return true
	end

    -- 完成 结束循环
    if self.setuped_index_F > #self.full_map_res then	--分步加载完成后延一帧再通知外部加载完成
        self.step_set_data_timer_F = nil

        self:LoadMapResForceComplete()
        return true
    end	
           
    -- 隔帧操作
    if self.StepFrameSpace_F ~= nil and self.StepFrameSpace_F > 1 then
		if self.part_record_F >= 0 then
			if self.part_record_F >= self.StepFrameSpace_F then
				self.part_record_F = 0
			else
				self.part_record_F = self.part_record_F + 1
				return false
			end
		else
			self.part_record_F = 0
		end
	end 

    local count = 0
    while true do
		count = count + 1
		self.setuped_index_F = self.setuped_index_F + 1
		if self.setuped_index_F > #self.full_map_res then
			return false
		end
        -- 生成逻辑
		local res_vo = self.full_map_res[self.setuped_index_F]

        local item_obj = self.MapResPic.gameObject:Clone(self.ResParent)
        item_obj.name = res_vo.link_mark.."|"..res_vo.map_show.map_stamp
        UH.LocalPosG(item_obj, res_vo.pos)
        item_obj:SetActive(true)
        local item_mono = item_obj:GetLuaComponent(self.MapResPic:GetTypeName())
        item_mono:SetData(res_vo)

        table.insert(self.wait_res,item_obj)

		if self.StepCount_F > 0 and count >= self.StepCount_F then
			return false
		end
	end
end

function LandsBetweenMapModel:LoadMapResForceCreate()
    self.part_record_C = -1
    self.setuped_index_C = 0

    -- 进行刷新前会删除之前发起的资源加载
    if self.step_set_data_timer_C then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer_C)
        self.step_set_data_timer_C = nil
    end

    self.step_set_data_timer_C = Runner.Instance:RunUntilTrue(BindTool.Bind(self.stepSetupItemsCreate,self))
end 

function LandsBetweenMapModel:stepSetupItemsCreate()
    if self:Destroyed() then
		return true
	end

    -- 完成 结束循环
    if self.setuped_index_C > #self.wait_res then	--分步加载完成后延一帧再通知外部加载完成
        self.step_set_data_timer_C = nil
        return true
    end	
           
    -- 隔帧操作
    if self.StepFrameSpace_C ~= nil and self.StepFrameSpace_C > 1 then
		if self.part_record_C >= 0 then
			if self.part_record_C >= self.StepFrameSpace_C then
				self.part_record_C = 0
			else
				self.part_record_C = self.part_record_C + 1
				return false
			end
		else
			self.part_record_C = 0
		end
	end 

    local count = 0
    while true do
		count = count + 1
		self.setuped_index_C = self.setuped_index_C + 1
		if self.setuped_index_C > #self.wait_res then
			return false
		end
        -- 生成逻辑
		local res_vo = self.wait_res[self.setuped_index_C]
        local oper_mono = res_vo:GetLuaComponent(self.MapResPic:GetTypeName())
        oper_mono:CreateRes()

		if self.StepCount_C > 0 and count >= self.StepCount_C then
			return false
		end
	end
end 