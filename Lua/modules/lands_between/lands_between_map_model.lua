LandsBetweenMapModel = LandsBetweenMapModel or DeclareMono("LandsBetweenMapModel",UIWFlushPanel)
function LandsBetweenMapModel:LandsBetweenMapModel()
    self.data = LandsBetweenData.Instance

    self.data_cares = {
        {data = self.data.operating_info, func = self.ClickFollow,keys = {"map_click"},init_call = false},
        -- {data = self.data.net_role_info, func = self.FlushForce,init_call = false},
        -- {data = self.data.net_alter_info, func = self.FlushVisible,init_call = false},
        -- {data = self.data.net_map_info, func = self.FlushForce,init_call = false},
        -- {data = self.data.operating_info, func = self.TryEffPos,init_call = false},
        {data = self.data.operating_info, func = self.FlushForce,init_call = false},
        {data = self.data.slow_start, func = self.FlushSlowStart},
    }

    self.max_cache  = 30
    self.cache_list = {}
    self.res_cache_list = {}

    self.touch_helper = Game.SceneTouchHelper
    -- Runner.Instance:AddRunObj(self, 16)
    self.is_touch_begin = false
    self.is_draging = false

    self.cache_mark = {
        visible = {},
        grid = {},
    }

    self.cache_res_mark = {}
    self.res_init_flag = false

    self.perfor_count = {
        high = 8,
        mid = 6,
        low = 1,
    }

    self.perfor_frame = {
        high = nil,
        mid = 3,
        low = 5,
    }

    self.clone_perfor = {
        high = 15,
        low = 5,
    }

    self.StepCount = 8
    self.StepFrameSpace = nil

    self.step_set_data_timer = nil
    self.part_record = -1

    self.force_res_init = false

    self.force_link_mark = {}

    self.DragStepFrameSpace = 25

    self:InitSpPic()
    self:InitLimit()

    self:InitForceRes()
end 

-- 慢启动状态下会大幅降低资源生成效率
function LandsBetweenMapModel:FlushSlowStart()
    self.StepCount = self.data.slow_start.flag and self.perfor_count.low or self.perfor_count.high
    self.StepFrameSpace = self.data.slow_start.flag and self.perfor_frame.low or self.perfor_frame.high
end

-- 资源生成效率降低为低
function LandsBetweenMapModel:LimitFlushResSpeedLow()
    self.StepCount = self.perfor_count.low
    self.StepFrameSpace = self.perfor_frame.low 
end

-- 资源生成效率降低为中
function LandsBetweenMapModel:LimitFlushResSpeedMid()
    self.StepCount = self.perfor_count.mid
    self.StepFrameSpace = self.perfor_frame.mid 
end

-- 资源生成效率提高为高
function LandsBetweenMapModel:LimitFlushResSpeedHigh()
    self.StepCount = self.perfor_count.high
    self.StepFrameSpace = self.perfor_frame.high 
end


function LandsBetweenMapModel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    -- Runner.Instance:RemoveRunObj(self)

    if self.jump_timer ~= nil then
        TimeHelper:CancelTimer(self.jump_timer)
        self.jump_timer = nil
    end

    if self.pic_timer ~= nil then
        TimeHelper:CancelTimer(self.pic_timer)
        self.pic_timer = nil
    end

    if self.step_set_data_timer ~= nil then
		Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer)
		self.step_set_data_timer = nil
	end
    if self.drag_runner then
        TimeHelper:CancelTimer(self.drag_runner)
        self.drag_runner = nil
    end

    self.data.map_running_mark = 0
end

function LandsBetweenMapModel:Awake()
    UIWFlushPanel.Awake(self)

    local cur_type = self.data:GetCurMapType()
    UH.LocalRotate(self.camera.gameObject.transform,Quaternion.Euler(45,cur_type == 0 and 180 or 0,0))
    self.OutMapModel:SetActive(cur_type == 0)
    self.InMapModel:SetActive(cur_type == 1)

    local init_param = self.data:GetInitVisible()
    UH.LocalPos(self.LockPos,init_param.look_pos)
    LogDG("初始化地点信息 ?",init_param)
    self.Vcamera.Follow = self.LockPos.transform

    -- self:LoadMapResTotalForce()
end

-- 启动后的基础视野
-- 调整时机，从界面启动
function LandsBetweenMapModel:InitVisible()
    self:FlushVisible()
end

function LandsBetweenMapModel:SetDraging(flag)
    self.is_draging = flag

    -- 设置为开始拖动
    if flag then 
        -- 拖拽中终止加载资源操作
        if self.step_set_data_timer then
            Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer)
            self.step_set_data_timer = nil
        end
        TimeHelper:CancelTimer(self.drag_runner)
        self.drag_runner = TimeHelper:AddRunFrameTimer(function ()
            self:FlushVisible()
            self:FlushForce()
            self.data.operating_info.draging = self.data.operating_info.draging + 1
        end, self.DragStepFrameSpace,600000)

    -- 设置为停止拖动
    else 
        if self.drag_runner then
            TimeHelper:CancelTimer(self.drag_runner)
            self.drag_runner = nil
        end
    end 

    -- self.raycast:SetActive(flag)
end

-- 针对地图点击获取位置并跳转
function LandsBetweenMapModel:ClickFollow()
    if self.data.operating_info.map_click == nil then return end 

    UH.LocalPos(self.LockPos,self.data.operating_info.map_click)
    self.Vcamera.Follow = self.LockPos.transform
    TimeHelper:CancelTimer(self.jump_timer)
    self.jump_timer = TimeHelper:AddDelayTimer(function()
        -- self.raycast:SetActive(true)
        self:FlushVisible()

    end, 0.5)

    self.data.operating_info.map_click = nil
end

-- 摆正特效锚点到指定位置
function LandsBetweenMapModel:TryEffPos()
    UH.LocalPos(self.EffPos,self.data.operating_info.obj.transform.localPosition)
end

function LandsBetweenMapModel:FlushComplete() 
    self:JumpFlush()
end 

-- 跳跃点击
-- 本方法联动，追加操作轮
function LandsBetweenMapModel:JumpFlush()
    if self.data.operating_info.jump == nil then 
        return 
    end 

    local link_mark = self.data.CfgLink({[1] = self.data.operating_info.jump.x,[2] = self.data.operating_info.jump.y})
    local visible = self.data:GetVisibleFormWhere(self.data.operating_info.jump)
    if self.cache_mark.visible[visible.group_id] ~= nil and self.cache_mark.grid[link_mark] ~= nil then 

        local obj = self.cache_list[self.cache_mark.visible[visible.group_id]].list[self.cache_mark.grid[link_mark]]
        local operate = obj:GetLuaComponent(LandsBetweenMapModelPic:GetTypeName())
        self.data:SetMapPicClick(operate:GetData())
        self.data:SetOperatingObj(obj)

        self.data:SetMapPicTestClick(operate:GetData())
        self.data:SetOperatingTestObj(obj)

        -- self.raycast:SetActive(true)
    else 
        LogError("读取缓存失败")
    end 

    self.data.operating_info.jump = nil
end

-- 刷新视野
function LandsBetweenMapModel:FlushVisible()
    -- LogError("??sdf FlushVisible")
    if self.pic_timer ~= nil then
        TimeHelper:CancelTimer(self.pic_timer)
        self.pic_timer = nil
    end

    if not ViewMgr:IsOpen(LandsBetweenMainView) then 
        return 
    end 

    if self.raycast:Equals(nil) or self.raycast == nil then 
        return 
    end 

    self.data:SetFlushPicComplete(false)
    self.raycast:SetActive(true)

    local ray_info = self.camera:ViewportPointToRay(Vector3.New(0.5,0.5,1))
    local ray_succ,hit_info = UnityEngine.Physics.Raycast(ray_info,nil)
    -- LogError("?ray_info",ray_succ,hit_info,self.raycast.activeSelf)
    if hit_info ~= nil then 
        local map_grid = self.data:ChangePosFormWPos(hit_info.point)
        if map_grid == nil then 
            return 
        end 
        -- LogError("?fusj",map_grid)
        UH.LocalPos(self.Observe,self.data:ChangePosFromWhere(map_grid))
        local visible = self.data:GetVisibleFormWhere(map_grid)
        if visible == nil then 
            PublicPopupCtrl.Instance:Center("发现无效视野！刷新失效！")
            LogDG("发现无效视野！刷新失效！")
            return 
        end 
        local visible_list = self.data:GetVisibleAround(visible,false)
        local res_visible_list = self.data:GetVisibleAround(visible,true,self.is_draging)

        self.data:SetCurVisibleList(visible_list)

        self:FLushPic(visible_list)
        self:FlushResPic(res_visible_list)
        -- self:FlushSpPic(visible_list)
        self.cache_visible = visible_list

        if self.raycast.activeSelf then 
            self.raycast:SetActive(false)
        end 

        self:FlushComplete()
    end 
end

function LandsBetweenMapModel:Update()
    -- self:UpdateTouch()
    
end 

-- 已废弃
function LandsBetweenMapModel:UpdateTouch()
    if self.data:IsOtherViewOpening() then 
        return 
    end 
    if self.is_draging then 
        return 
    end 
    if self.is_touch_begin then
		if self.touch_helper.IsTouchUp() then
			self.is_touch_begin = false
			local hit, pos, obj = self.touch_helper.Raycast(nil, nil)
			if not hit then return end
			if obj.layer == GameObjLayer.ColliderRole then
				local operate = obj:GetLuaComponent(LandsBetweenMapModelPic:GetTypeName())
				if operate and operate.ClickItem then operate:ClickItem() end 
			end
		end
	else
		if not self.touch_helper.IsTouching() then
			return
		end
		if self.touch_helper.IsTouching() then
			self.is_touch_begin = true
		end
	end
end

-- 尝试对点击的视口坐标进行射线检测
function LandsBetweenMapModel:TryTouchTouch(touch_view_port)
    local ray_info = self.camera:ViewportPointToRay(touch_view_port)
    local ray_succ,hit_info = UnityEngine.Physics.Raycast(ray_info,nil)
    if hit_info ~= nil then 
        local obj = hit_info.transform.gameObject
        local operate = obj:GetLuaComponent(LandsBetweenMapModelPic:GetTypeName())
        if operate and operate.ClickItem then 
            operate:ClickItem() 

            for k,v in pairs(self.cache_list) do
                for i,j in pairs(v.list) do 
                    local j_oper = j:GetLuaComponent(LandsBetweenMapModelPic:GetTypeName())
                    j_oper:SelectItem(j_oper:GetData().link_mark == operate:GetData().link_mark)
                end 
            end 
        end 

    end 
end

-- 刷新操作格
function LandsBetweenMapModel:FLushPic(visible_list) 
    -- for k,v in pairs(visible_list) do
    --     for i , j in pairs(self.data:ReleaseVisible(v)) do 
    --         local vo = {}
    --         vo.x = j.x
    --         vo.y = j.y
    --         vo.group_id = v.group_id
    --     end 
    -- end 
    local map_list = {}
    for k,v in pairs(visible_list) do 
        local grid_list = self.data:ReleaseVisible(v)
        local vo = {}
        vo.group_id = v.group_id
        vo.list = grid_list
        table.insert(map_list,vo)
    end 

    if #self.cache_list == 0 then 
        for k,v in pairs(map_list) do 
            local vo = {}
            vo.group_id = v.group_id
            vo.list = {}
            for i,j in pairs(v.list) do 
                local item_obj = self.MapOperPic.gameObject:Clone(self.OperParent)
                item_obj.name = j.link_mark .. "|" .. v.group_id
                UH.LocalPosG(item_obj, j.pos)
                item_obj:SetActive(true)
                local item_mono = item_obj:GetLuaComponent(self.MapOperPic:GetTypeName())
                item_mono:SetData(j)
                table.insert(vo.list,item_obj)
            end 
            table.insert(self.cache_list,vo)
        end     
    end 


    for k,v in pairs(map_list) do
        local with_data = self.cache_list[k] ~= nil
        if with_data then 
            for i,j in pairs(v.list) do 
                self.cache_list[k].list[i]:SetActive(true)

                self.cache_list[k].list[i].name = j.link_mark .. "|" .. v.group_id
                UH.LocalPosG(self.cache_list[k].list[i],j.pos)
                local item_mono = self.cache_list[k].list[i]:GetLuaComponent(self.MapOperPic:GetTypeName())
                item_mono:SetData(j)
            end
        else 
            local vo = {}
            vo.group_id = v.group_id
            vo.list = {}
            for i,j in pairs(v.list) do 
                local item_obj = self.MapOperPic.gameObject:Clone(self.OperParent)
                item_obj.name = j.link_mark .. "|" .. v.group_id
                UH.LocalPosG(item_obj, j.pos)
                item_obj:SetActive(true)
                local item_mono = item_obj:GetLuaComponent(self.MapOperPic:GetTypeName())
                item_mono:SetData(j)
                table.insert(vo.list,item_obj)
            end 
            table.insert(self.cache_list,vo)
        end 
    end

    for k,v in pairs(self.cache_list) do 
        local with_data = map_list[k] ~= nil
        if with_data then 
            v.group_id = map_list[k].group_id
        end 
        for i,j in pairs(v.list) do 
            j:SetActive(with_data)

            if with_data then 
                j.name = map_list[k].list[i].link_mark .. "|" .. map_list[k].group_id
                UH.LocalPosG(j, map_list[k].list[i].pos)
                local item_mono = j:GetLuaComponent(self.MapOperPic:GetTypeName())
                item_mono:SetData(map_list[k].list[i])
            end 
        end
    end

    self.cache_mark.visible = {}
    self.cache_mark.grid = {}
    for k,v in pairs(self.cache_list) do 
        self.cache_mark.visible[v.group_id] = k
        for i,j in pairs(v.list) do 
            local item_mono = j:GetLuaComponent(self.MapOperPic:GetTypeName())
            local data = item_mono:GetData()
            self.cache_mark.grid[data.link_mark] = i
        end 
    end 
    TimeHelper:CancelTimer(self.pic_timer)
    self.pic_timer = TimeHelper:AddDelayTimer(function()
        self.data:SetFlushPicComplete(true)
    end, 0.1)
end

-- 刷新地图资源格
function LandsBetweenMapModel:FlushResPic(visible_list)
    -- for k,v in pairs(visible_list) do
    --     for i , j in pairs(self.data:ReleaseVisible(v)) do 
    --         local vo = {}
    --         vo.x = j.x
    --         vo.y = j.y
    --         vo.group_id = v.group_id
    --     end 
    -- end 
    local map_list = {}
    for k,v in pairs(visible_list) do 
        local grid_list = self.data:ReleaseVisible(v)
        local vo = {}
        vo.group_id = v.group_id
        vo.list = grid_list
        table.insert(map_list,vo)
    end 

    local neo_clone = 0
    local total_obj = 0 
    self.cache_res_mark = {}    
    -- 初始化
    if not self.res_init_flag then 
        for k,v in pairs(map_list) do 
            for i,j in pairs(v.list) do 
                if MapResOutFloor[j.map_show.map_stamp] == 1 then 
                    if self.res_cache_list[j.map_show.map_stamp] == nil then 
                        self.res_cache_list[j.map_show.map_stamp] = {}
                    end 

                    local item_obj = self.MapResPic.gameObject:Clone(self.ResParent)
                    item_obj.name = j.link_mark .. "|" .. v.group_id.."|"..j.map_show.map_stamp
                    UH.LocalPosG(item_obj, j.pos)
                    item_obj:SetActive(true)
                    local item_mono = item_obj:GetLuaComponent(self.MapResPic:GetTypeName())
                    item_mono:SetData(j)
                    table.insert(self.res_cache_list[j.map_show.map_stamp],item_obj)
                end 
            end 
        end   
        
        self.res_init_flag = true
    else 
        -- for k,v in pairs(self.res_cache_list) do 
        --     for i,j in pairs(v) do 
        --         j:SetActive(false)
        --     end 
        -- end 

        for k,v in pairs(map_list) do 
            for i,j in pairs(v.list) do 
                if MapResOutFloor[j.map_show.map_stamp] then 
                    if self.res_cache_list[j.map_show.map_stamp] == nil then 
                        self.res_cache_list[j.map_show.map_stamp] = {}
                    end 

                    if self.cache_res_mark[j.map_show.map_stamp] == nil then 
                        self.cache_res_mark[j.map_show.map_stamp] = 0
                    end 

                    local index = self.cache_res_mark[j.map_show.map_stamp] + 1
                    if self.res_cache_list[j.map_show.map_stamp][index] == nil then 
                        -- 如果缓存里没有，那就原地生成一个出来
                        local item_obj = self.MapResPic.gameObject:Clone(self.ResParent)
                        item_obj.name = j.link_mark .. "|" .. v.group_id.."|"..j.map_show.map_stamp
                        UH.LocalPosG(item_obj, j.pos)
                        item_obj:SetActive(true)
                        local item_mono = item_obj:GetLuaComponent(self.MapResPic:GetTypeName())
                        item_mono:SetData(j)
                        self.res_cache_list[j.map_show.map_stamp][index] = item_obj
                    else
                        -- 如果缓存里有，那就拿出来移动到该位置
                        local oper_obj = self.res_cache_list[j.map_show.map_stamp][index]
                        oper_obj.name = j.link_mark .. "|" .. v.group_id.."|"..j.map_show.map_stamp
                        UH.LocalPosG(oper_obj, j.pos)
                        oper_obj:SetActive(true)
                    end 
                    self.cache_res_mark[j.map_show.map_stamp] = index
                end 
            end 
        end 
    end 

    
    local cloning = self.data:GetRunningMark()
    -- LogError("?flush rate",neo_clone,total_obj,clone_rate)
    if cloning > self.clone_perfor.high  then -- 建造中为20
        -- LogError("low?flush rate",cloning)
        self:LimitFlushResSpeedLow()
    elseif cloning < self.clone_perfor.high and cloning > self.clone_perfor.low then  -- 建造中为10~20
        -- LogError("mid?flush rate",cloning)
        self:LimitFlushResSpeedMid()
    else  -- 建造中低于10
        -- LogError("high?flush rate",cloning)
        self:LimitFlushResSpeedHigh()
    end 

    self:FlushResCreate()
end

-- 生成资源（分步
function LandsBetweenMapModel:FlushResCreate()
    -- 统计需要生成的资源
    -- self.res_cache_list[j.map_show.map_stamp]
    self.res_list = {}

    for k,v in pairs(self.res_cache_list) do 
        for i,j in pairs(v) do 
            local oper_mono = j:GetLuaComponent(self.MapResPic:GetTypeName())
            if not oper_mono:IsResCreated() then 
                local vo = {}
                vo.func = BindTool.Bind1(oper_mono.CreateRes, oper_mono) 
                table.insert(self.res_list,vo)
            end 
        end 
    end 

	self.part_record = -1
    self.setuped_index = 0

    -- 进行刷新前会删除之前发起的资源加载
    if self.step_set_data_timer then
        Runner.Instance:RemoveRunUntilTrue(self.step_set_data_timer)
        self.step_set_data_timer = nil
    end

    self.step_set_data_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.stepSetupItems,self))

end

function LandsBetweenMapModel:FlushResCreateComplete()
    -- if not self.init_force_sp then 
    --     
    --     self.init_force_sp = true
    -- end 
    -- self:FlushForce()
end

-- 分布生成主题逻辑块
function LandsBetweenMapModel:stepSetupItems()
    if self:Destroyed() then
		return true
	end

    -- 完成 结束循环
    if self.setuped_index > #self.res_list then	--分步加载完成后延一帧再通知外部加载完成
		self.step_set_data_timer = nil
        self:FlushResCreateComplete()
		return true
	end	
    
    -- 隔帧操作
    if self.StepFrameSpace ~= nil and self.StepFrameSpace > 1 then
		if self.part_record >= 0 then
			if self.part_record >= self.StepFrameSpace then
				self.part_record = 0
			else
				self.part_record = self.part_record + 1
				return false
			end
		else
			self.part_record = 0
		end
	end

    local count = 0
    while true do
		count = count + 1
		self.setuped_index = self.setuped_index + 1
		if self.setuped_index > #self.res_list then
			return false
		end
        -- 生成逻辑
		local res_vo = self.res_list[self.setuped_index]
        if res_vo.func ~= nil then 
            res_vo.func()
        end 

		if self.StepCount > 0 and count >= self.StepCount then
			return false
		end
	end
end


------------------------- 追加限制区域
function LandsBetweenMapModel:InitLimit()
    self.limit_1_pos = {
        x = {self.Limit1s[3].transform.position.z,self.Limit1s[4].transform.position.z}, 
        y = {self.Limit1s[1].transform.position.x,self.Limit1s[2].transform.position.x}}

    self.limit_2_pos = {
        x = {self.Limit2s[3].transform.position.z,self.Limit2s[4].transform.position.z}, 
        y = {self.Limit2s[1].transform.position.x,self.Limit2s[2].transform.position.x}}
    
end

function LandsBetweenMapModel:CheckLimit(cam_x,cam_y,delta_x,delta_y)
    
    local cur_type = self.data:GetCurMapType()

    local change_x = cam_x - (cur_type == 0 and 1 or -1) * delta_x
    local change_y = cam_y - (cur_type == 0 and 1 or -1) * delta_y

    local oper_limit = cur_type == 0 and self.limit_1_pos or self.limit_2_pos

    if - change_x < oper_limit.x[1] then 
        change_x = - oper_limit.x[1]
    elseif - change_x > oper_limit.x[2] then
        change_x = - oper_limit.x[2]
    end 

    if change_y < oper_limit.y[1] then 
        change_y = oper_limit.y[1]
    elseif change_y > oper_limit.y[2] then
        change_y = oper_limit.y[2]
    end 

    return change_x,change_y
end

-- 根据地图角色信息获取地图格
function LandsBetweenMapModel:CatchFollowTarget(role_info)
    local link_mark = self.data.CfgLink({[1] = role_info.x,[2] = role_info.y})
    local here = self.data:GetVisibleFormWhere({x = role_info.x,y = role_info.y})

    
    if self.cache_mark.grid[link_mark] ~= nil and self.cache_mark.visible[here.group_id] ~= nil then 
        return self.cache_list[self.cache_mark.visible[here.group_id]].list[self.cache_mark.grid[link_mark]]
    end 

    return nil
end

-- 在指定的link_mark生成一个force_pos,并返回该force_pos，且标记之为force_mark
-- 若该force_mark已存在，则以新的link_mark改变它的具体位置
-- force_mark 为测试后缀和缓存
function LandsBetweenMapModel:LockTargetPos(link_mark,force_mark)
    local force_pos = self.data:ChangePosFromWhereByLinkMark(link_mark)

    if self.force_link_mark[force_mark] ~= nil then 
        self.force_link_mark[force_mark].name = link_mark .. "|" .. force_mark
        UH.LocalPosG(self.force_link_mark[force_mark], force_pos)

        return self.force_link_mark[force_mark]
    end 

    local item_obj = self.ForceMark.gameObject:Clone(self.ForceParent)
    item_obj.name = link_mark .. "|" .. force_mark
    UH.LocalPosG(item_obj, force_pos)
    self.force_link_mark[force_mark] = item_obj
    return item_obj
end

LandsBetweenMapModelPic = DeclareMono("LandsBetweenMapModelPic", UIWidgetBaseItem)
function LandsBetweenMapModelPic:LandsBetweenMapModelPic() end
function LandsBetweenMapModelPic:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
    if LandsBetweenData.Instance.operating_info.draging > 0 then 
        self.Select:SetActive(false)
    end 
    self.Wait:SetActive(false)

    local faction = LandsBetweenData.Instance:GetMyFaction()
    local net_info = LandsBetweenData.Instance:GetMapGridNetInfo(data.link_mark)
    local pic_faction = net_info ~= nil and (net_info.faction ~= nil and net_info.faction or (net_info.occupied_faction ~= nil and net_info.occupied_faction or -1)) or -1
    self.UsOccupyed:SetActive(pic_faction ~= -1 and pic_faction == faction)
    self.OtherOccupyed:SetActive(pic_faction ~= -1 and pic_faction ~= faction)

    if net_info ~= nil then 
        -- self.UnTouch:SetActive()
        local is_cd,cd_stamp = LandsBetweenData.Instance:IsInOccupyCD(net_info) 
        if self.cant_show ~= nil then
            TimeHelper:CancelTimer(self.cant_show)
            self.cant_show = nil
        end

        self.Wait:SetActive(is_cd)
        if self.UsOccupyed.activeSelf then 
            self.UsOccupyed:SetActive(not is_cd)
        end 
        
        if self.OtherOccupyed.activeSelf then 
            self.OtherOccupyed:SetActive(not is_cd)
        end 

        if is_cd then 
            TimeHelper:CancelTimer(self.cant_show)
            self.cant_show = TimeHelper:AddDelayTimer(function()
                if not ViewMgr:IsOpen(LandsBetweenMainView) then 
                    return 
                end 
                self.Wait:SetActive(false)
                self.UsOccupyed:SetActive(true)
            end,cd_stamp - TimeManager:GetServerTime())
        end 
    end
end

function LandsBetweenMapModelPic:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.cant_show ~= nil then
        TimeHelper:CancelTimer(self.cant_show)
        self.cant_show = nil
    end

    if self.cannot_show ~= nil then
        TimeHelper:CancelTimer(self.cannot_show)
        self.cannot_show = nil
    end
end

-- 射线检测成功，点击对应的地图格子
function LandsBetweenMapModelPic:ClickItem()
    local net_info = LandsBetweenData.Instance:GetMapGridNetInfo(self.data.link_mark)
    local is_cd,cd_stamp = LandsBetweenData.Instance:IsInOccupyCD(net_info) 
    local sp_grid_param = LandsBetweenData.Instance:CheckSpPicAroundByLinkMark(self.data.link_mark)

    if is_cd then 
        PublicPopupCtrl.Instance:Center(string.format(Language.LandsBetween.Desc.GridCding,math.floor(cd_stamp - TimeCtrl.Instance:GetServerTime())))
    end 

    if (LandsBetweenData.Instance:CheckCanOperate(self.data) and not is_cd) or sp_grid_param ~= nil then 
        
        LandsBetweenData.Instance:SetMapPicClick(self.data)
        LandsBetweenData.Instance:SetOperatingObj(self.gameObject)
    elseif not self.OtherOccupyed.activeSelf then 
        self.UnTouch:SetActive(true)
        TimeHelper:CancelTimer(self.cannot_show)
        self.cannot_show = TimeHelper:AddDelayTimer(function()
            self.UnTouch:SetActive(false)
            self:SelectItem(false)
        end,1)
    end 

    LandsBetweenData.Instance:SetMapPicTestClick(self.data)
    LandsBetweenData.Instance:SetOperatingTestObj(self.gameObject)
end

function LandsBetweenMapModelPic:SelectItem(flag)
    self.Select:SetActive(flag)
end

LandsBetweenMapModelResPic = DeclareMono("LandsBetweenMapModelResPic", UIWidgetBaseItem)
function LandsBetweenMapModelResPic:LandsBetweenMapModelResPic() 
    self.res_created = false
end
function LandsBetweenMapModelResPic:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    self:SetResMark(data.map_show.map_stamp)
end 

function LandsBetweenMapModelResPic:CreateRes()
    local cfg  = LandsBetweenData.Instance:GetMapResInfo(self.res_mark)
    local path  = string.format(GoldLaw.ResPath,cfg.corresponding_model)

    if not ViewMgr:IsOpen(LandsBetweenMainView) then 
        return 
    end 

    LandsBetweenData.Instance:OperRunningMark(1)
    ResMgr:LoadGameObject(path, function (obj)
        LandsBetweenData.Instance:OperRunningMark(-1)
        self.obj = obj
        self.obj:SetParent(self.gameObject)
        self.obj.transform.localPosition = Vector3.New(0,-1,0)
    end,true,false)
    
    self.res_created = true
end

function LandsBetweenMapModelResPic:SetResMark(stamp)
    self.res_mark = stamp
end

function LandsBetweenMapModelResPic:GetResMark()
    return self.res_mark
end

function LandsBetweenMapModelResPic:IsResCreated()
    return self.res_created
end

LandsBetweenMapModelSpResPic = DeclareMono("LandsBetweenMapModelSpResPic", UIWidgetBaseItem)
function LandsBetweenMapModelSpResPic:LandsBetweenMapModelSpResPic() 
    self.objs = {}
    self.path_list = {}
end
function LandsBetweenMapModelSpResPic:SetData(data)
    if self.data ~= nil and self.data.res_path ~= data.res_path then 
        self.complete = false
    end 

	UIWidgetBaseItem.SetData(self, data)
end 

function LandsBetweenMapModelSpResPic:CreateRes()
    if self.data.res_path == nil then 
        for k,v in pairs(self.objs) do 
            v:SetActive(false)
        end 
        return 
    end 

    if not ViewMgr:IsOpen(LandsBetweenMainView) then         
        return 
    end

    if self.objs[self.data.res_path] ~= nil then 
        for k,v in pairs(self.objs) do 
            v:SetActive(k == self.data.res_path)
        end  
    else         
        local item_obj = self.CreatePos.gameObject:Clone(self.Total)
        self.objs[self.data.res_path] = item_obj

        if self.path_list[self.data.res_path] == nil then 
            self.path_list[self.data.res_path] = 1
            ResMgr:LoadGameObject(self.data.res_path, function (obj)
                obj:SetParent(self.objs[self.data.res_path])
                if self.data.act_type and self.data.act_type == ActType.Animals then
                    obj.transform.localPosition = Vector3.New(0,0,0)
                    obj:SetLocalEulerAngles(Vector3.New(0, -90, 0))
                    local renders = obj:GetComponentsInChildren(typeof(UnityEngine.SkinnedMeshRenderer))
                    if renders then
                        for i = 0, renders.Length - 1 do
                            if renders[i] then
                                renders[i].gameObject.layer = GameObjLayer.QualityScene
                            end
                        end
                    end
                else
                    obj.transform.localPosition = Vector3.New(0,0,0)
                end
            end,true,false)
        end 
    end 
end

