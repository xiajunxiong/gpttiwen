TaoQuanNaJiBattleShow = TaoQuanNaJiBattleShow or BaseClass()

function TaoQuanNaJiBattleShow:__init()
    if TaoQuanNaJiBattleShow.Instance ~= nil then
		Debuger.LogError("[TaoQuanNaJiBattleShow] attempt to create singleton twice!")
		return
	end
	TaoQuanNaJiBattleShow.Instance = self


    self.lr_data = TaoQuanNaJiData.Instance

    self.battle_show = SmartData.New({show_simulate = false})
    self.sim_data = SmartData.New({leg1 = 0,leg2 = 0,h_start = 0,h_pass = 0,h_end = 0,accuracy = 0,offset = 0,min_stren = 0.01,angle_off = 0})

    self.is_flying = false

    self.spawn_obj = nil
    self.ring_spawn = nil
end

-- 
function TaoQuanNaJiBattleShow:StartScene(finish_func)
    local res_path = string.format("ui/views/serveractivity/taoquannaji/RingSpawn.prefab")
    ResMgr:LoadGameObject(res_path, function (obj)	
        self.spawn_obj = obj
        self.spawn_obj.transform.localPosition = Vector3.New(378.46,125.29,294)

        self.ring_spawn = self.spawn_obj:GetLuaComponent(TaoQuanNaJiSpawn:GetTypeName())

        self.spawn_obj:SetParent(UnityGameObj.Find("ResidentObjectManager"))
        -- 初始的演出里是不储存数据的，只会在启动场景的时候从实装完成的生成器里提取编辑器信息
        -- self.ring_spawn:SpawnMap()
        -- self:EditorFlush()
        if finish_func then
            finish_func()
        end
    end, true, true)    
end

function TaoQuanNaJiBattleShow:ReStart()
    self:ExitScene()
    self:StartScene()
end

function TaoQuanNaJiBattleShow:ExitScene()
    UnityGameObj.Destroy(self.spawn_obj)
end

function TaoQuanNaJiBattleShow:EditorFlush()
    if self.spawn_obj ~= nil and self.ring_spawn ~= nil then 
        self.ring_spawn:SaveEditor()
    end 
end

function TaoQuanNaJiBattleShow:ShowSimulate()
    self.battle_show.show_simulate = not self.battle_show.show_simulate

    self.ring_spawn:FlushSimulate()
    self:Input({stren = 0.01,angle = 0})
end

function TaoQuanNaJiBattleShow:SpawnFlushMap()
    self.ring_spawn:MapFlush()
end

function TaoQuanNaJiBattleShow:Input(data)
    if data.stren < 0.01 then
        return 
    end 

    local plus_data = {
        stren = data.stren;
        angle = data.angle;
    }

    if self.ring_spawn == nil then return end 
    self:Simulate(data)
end

-- 传入模拟信息记录
function TaoQuanNaJiBattleShow:MarkSimulate(param)
    self.sim_data.leg1 = param.leg1
    self.sim_data.leg2 = param.leg2
    self.sim_data.h_start = param.h_start
    self.sim_data.h_pass = param.h_pass
    self.sim_data.h_end = param.h_end
    -- self.sim_data.accuracy = param.accuracy

end

function TaoQuanNaJiBattleShow:GetStartPos()
    return Vector3.New(0,self.sim_data.h_start,0)
end

function TaoQuanNaJiBattleShow:FlushInputEditor(param)
    self.sim_data.accuracy = param.accuracy
    self.sim_data.offset = param.offset
    self.sim_data.min_stren = param.min_stren
    self.sim_data.angle_off = param.angle_off
end

-- 
function TaoQuanNaJiBattleShow:Simulate(data)
    local start_pos = Vector3.New(0,self.sim_data.h_start,0)
    -- 实际·第一中继臂
    local leg_1 = self.sim_data.leg1 * data.stren
    -- 实际·第二中继臂
    local leg_2 = self.sim_data.leg2 * data.stren
    -- 转换第一中继臂 坐标
    local cross_pos = Vector3.New(
        leg_1 * math.sin(-math.rad(data.angle)),
        self.sim_data.h_pass,
        leg_1 * math.cos(-math.rad(data.angle)))

    -- 转换第二中继臂 坐标
    local target_pos = Vector3.New(
        leg_2 * math.sin(-math.rad(data.angle)),
        self.sim_data.h_end,
        leg_2 * math.cos(-math.rad(data.angle)))

    -- 模拟结果
    local curve = Game.CurveParabola(start_pos, target_pos, cross_pos, 0.2)

    -- 本次结果取样
    local flag = 0
    self.simulate_list = {}
    for i = 1,self.sim_data.accuracy do 
        flag = flag + (1/self.sim_data.accuracy)
        local pos = curve:GetPos(flag)

        table.insert(self.simulate_list,pos)
    end 

    --
    if self.ring_spawn then 
        -- LogError("?!",self.simulate_list)
        if self.battle_show.show_simulate then 
            self.ring_spawn:StartSimulate(self.ring_spawn:BorderlineCheck(self.simulate_list))
        end 

        self.ring_spawn:SimulateRing(data)
    end 
end


function TaoQuanNaJiBattleShow:PlayRingFly()
    if self.is_flying then 
        LogError("演出仍在执行")
        return 
    end 

    
    self.ring_spawn:PlayRingFly(self.ring_spawn:BorderlineCheck(self.simulate_list))
end

function TaoQuanNaJiBattleShow:IsFlying() 
    return self.ring_spawn.is_flying
end 

function TaoQuanNaJiBattleShow:GetMapItemObjs()
    return self.ring_spawn.map_items
end

function TaoQuanNaJiBattleShow:GetMapItemObj(pos)
    return self.ring_spawn.map_items[pos]
end

function TaoQuanNaJiBattleShow:GetMapItemMods()
    return self.ring_spawn.map_item_mods
end

-- 负责生成投掷模拟和投掷信息，不参与模拟
--===========================TaoQuanNaJiSpawn===========================
TaoQuanNaJiSpawn = TaoQuanNaJiSpawn or DeclareMono("TaoQuanNaJiSpawn", UIWFlushPanel)
function TaoQuanNaJiSpawn:TaoQuanNaJiSpawn()
    self.s_data = TaoQuanNaJiBattleShow.Instance
    self.data = TaoQuanNaJiData.Instance
    self.data_cares = {
        {data = self.s_data.battle_show,func = self.FlushSimulate,keys = {"show_simulate"},init_call = false},            
        {data = self.data.ring_info,func = self.MapFlush,keys = {"flush"},init_call = false},            
    }

    self.simulate_cubes = {}    
    
    self.map_items = {}
    self.map_item_mods = {}
    self.ring_obj = nil
    self.ring_show = nil

    self:SpawnMap()

    self.map_complete = false
end

function TaoQuanNaJiSpawn:FlushSimulate()
    self.simulate:SetActive(self.s_data.battle_show.show_simulate)
end

-- 将锚点信息回传到演出上
function TaoQuanNaJiSpawn:SaveEditor()
    local param = {}
    -- 绕行臂 第一中继臂 
    param.leg1 = self.cross_pos.transform.localPosition.z
    -- 终点臂 第二中继臂 
    param.leg2 = self.target_pos.transform.localPosition.z

    -- 绕行臂 第一中继高度
    param.h_pass = self.cross_pos.transform.localPosition.y

    -- 终点臂 第二中继高度
    param.h_end = self.target_pos.transform.localPosition.y

    -- 起点高度 只取高度
    param.h_start = self.start_pos.transform.localPosition.y

    -- 模拟精度
    -- param.accuracy = self.accuracy

    self.s_data:MarkSimulate(param)

    if self.ring_show ~= nil then 
        self.ring_show.transform.localPosition = self.s_data:GetStartPos()
    end 
end
 
function TaoQuanNaJiSpawn:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    for k,v in pairs(self.map_items) do 
        UnityGameObj.Destroy(v)
    end 

    for k,v in pairs(self.simulate_cubes) do 
        UnityGameObj.Destroy(v)
    end 

    UnityGameObj.Destroy(self.ring_obj)
    UnityGameObj.Destroy(self.ring_show)
end

-- 进行模拟
-- 模拟行为是随时刷新的，但是可以通过开关总预设来判断是否展示
function TaoQuanNaJiSpawn:StartSimulate(list)
    for i = 1, self.s_data.sim_data.accuracy do 
        if self.simulate_cubes[i] == nil then 
            local res_path = string.format("ui/views/serveractivity/taoquannaji/point_simulate.prefab")
            ResMgr:LoadGameObject(res_path, function (obj)	
                self.simulate_cubes[i] = obj
                self.simulate_cubes[i]:SetParent(self.simulate,false)
                self.simulate_cubes[i].transform.localPosition = list[i]
            end,true,true)

        else
            self.simulate_cubes[i].transform.localPosition = list[i]
        end 
    end
end 
    

function TaoQuanNaJiSpawn:BorderlineCheck(list)
    if self.front == nil or self.left == nil or self.right == nil 
     or self.front.transform == nil or self.left.transform == nil or self.right.transform == nil then 
        return list
    end 
    local front_line = self.front.transform.localPosition
    local left_line = self.left.transform.localPosition
    local right_line = self.right.transform.localPosition


    local mark_pos = nil
    local mark_index = {}
    for k,v in pairs(list) do 
        -- 前方越界
        if v.z >= front_line.z or left_line.x > v.x or right_line.x < v.x then  
            if mark_pos == nil then 
                mark_pos = v
            end 

            table.insert(mark_index,k)
        end 
    end 

    if mark_pos ~= nil then 
        for i = 1,#mark_index do 
            list[mark_index[i]] = Vector3.New(mark_pos.x,list[mark_index[i]].y,mark_pos.z)
        end 
    end 

    return list
end

function TaoQuanNaJiSpawn:SimulateRing(param)
    if self.ring_show == nil then 
        return 
    end 
    local can_off = math.abs(param.angle) > 3 -- self.s_data.sim_data.angle_off
    local plus_angle = (- param.angle) + ( can_off and ((param.angle > 0 and -1 or 1) * self.s_data.sim_data.angle_off) or 0)
    UH.LocalEulerAngles(self.ring_show.transform,Vector3.New(0,plus_angle,0))
end

-- 投圈演出
function TaoQuanNaJiSpawn:PlayRingFly(list)
    if list == nil then return end 
    function fly(obj)
        self.ring_show:SetActive(false)
        self.ring_obj:SetActive(true) 
        self.is_flying = true

        local index = 0
        self.runner = TimeHelper:AddRunFrameTimer(function ()
            if not self.data:InRing() then return end 
            if self.ring_obj == nil then return end 
            index = index + 1
            self.ring_obj.transform.localPosition = list[index]
            self.ring_obj:GetLuaComponent(TaoQuanNaJiing:GetTypeName()):Roll()
            if index >= self.s_data.sim_data.accuracy then 
                self.is_flying = false
                self:TargetFind()
            end 
        end,1,self.s_data.sim_data.accuracy)
    end

    if self.ring_obj == nil then 
        local res_path = string.format("ui/views/serveractivity/taoquannaji/Ring.prefab")
        ResMgr:LoadGameObject(res_path, function (obj)	
            self.ring_obj = obj
            self.ring_obj:SetParent(self.ring_pos,false)
            fly(self.ring_obj)
        end,true,true)
    else 
        fly(self.ring_obj)
    end 
end

function TaoQuanNaJiSpawn:TargetFind() 
    local function check_pos_func(ring_pos,check_pos)
        local check_leg_sq = math.pow((check_pos.x - ring_pos.x),2) + math.pow((check_pos.z - ring_pos.z),2)

        --2.2的平方
        return check_leg_sq <= 4.84
    end

    if self.ring_show then 
        self.ring_show.transform.localPosition = self.s_data:GetStartPos()
        self.ring_show:SetActive(true) 
    end 
    if self.ring_obj then self.ring_obj:SetActive(false) end 
    local end_pos = self.ring_obj.transform.localPosition
    for k,v in pairs(self.map_items) do 
        local check_pos = v.transform.localPosition
        if check_pos_func(end_pos,check_pos) then 
            if self.map_item_mods[k].item > 0 then 
                self.data:FlushTarget(self.map_item_mods[k].data)
                return 
            end 
        end 
        -- if check_pos.x >= end_pos.x -1 and check_pos.x <= end_pos.x +1 then 
        --     if check_pos.z >= end_pos.z -1 and check_pos.z <= end_pos.z +1 then 
        --         if self.map_item_mods[k].item > 0 then 
        --         --     TaoQuanNaJiCtrl.Instance:SendLuckyRingReq({req_type = 2,
        --         --     param1 = self.map_item_mods[k].pos.x,
        --         --     param2 =  self.map_item_mods[k].pos.y
        --         -- })

        --         self.data:FlushTarget(self.map_item_mods[k].data)

        --         return 
        --         end 
        --     end 
        -- end 
    end 

    self.data:FlushTarget({empty = true})
end 

function TaoQuanNaJiSpawn:SpawnMap()
    -- 分段创建
    self.create_index = 0
    if self.step_set_data_timer == nil then
        self.step_set_data_timer = Runner.Instance:RunUntilTrue(BindTool.Bind(self.stepSetupItems,self))
    end

    if self.ring_show == nil then 
        local res_path = string.format("ui/views/serveractivity/taoquannaji/RingShow.prefab")
        ResMgr:LoadGameObject(res_path, function (obj)	
            self.ring_show = obj
            self.ring_show:SetParent(self.ring_pos,false)
            self.ring_show.transform.localPosition = self.s_data:GetStartPos()
        end,true,true)
    end 
    
    if self.ring_obj == nil then 
       local res_path = string.format("ui/views/serveractivity/taoquannaji/Ring.prefab")
        ResMgr:LoadGameObject(res_path, function (obj)	
            self.ring_obj = obj
            self.ring_obj:SetParent(self.ring_pos,false)
            self.ring_obj:SetActive(false) 
        end,true,true)
    end 

end

-- x = 6 , y = 5
-- 分y步生成，每步生成x个
function TaoQuanNaJiSpawn:stepSetupItems()
    
    for i = 0,4 do 
        local res_path = string.format("ui/views/serveractivity/taoquannaji/MapItem.prefab")
        ResMgr:LoadGameObject(res_path, function (obj)	
            obj:SetParent(self.simluta_map,false)
            table.insert(self.map_items,obj)


            -- self.map_item_mods[self.create_index][i] = obj:GetLuaComponent(TaoQuanNaJiMapItem:GetTypeName())

            if #self.map_items == 30 then 
                self.map_complete = true
                self:MapCompleteCallBack()
            end 
        end,true,true)
    end 
    self.create_index = self.create_index + 1 

    return self.create_index > 5
end

function TaoQuanNaJiSpawn:MapCompleteCallBack()
    if self.cross_pos ~= nil then 
        self:MapFlush()
        self.wait_map_flush = false
        self.data:MapInitComplete()
    else 
        self.s_data:ReStart()
    end 
--     LogError("?after init ,",self.cross_pos == nil)
end

function TaoQuanNaJiSpawn:MapFlush()
    if not self.map_complete then 
        return 
    end 

    local show_list = {}
    for k,v in pairs(self.data.ring_info.play_map) do 
        local pos = v.x*5+v.y + 1
        if self.map_items[pos] ~= nil then 
            self.map_items[pos]:SetParent(self.simluta_map,false)
            self.map_item_mods[pos] = self.map_items[pos]:GetLuaComponent(TaoQuanNaJiMapItem:GetTypeName())

            self.map_items[pos].transform.localPosition = Vector3.New(
                            v.x == 0 and v.x or v.x *(1+ self.s_data.sim_data.offset),
                            0,
                            v.y == 0 and v.y or v.y *(1+ self.s_data.sim_data.offset))
            self.map_items[pos]:SetParent(self.map_part,true)
            self.map_item_mods[pos]:PosMarkComplete()
            
            if self.map_item_mods[pos]:CheckShowPoint(self.data:FixMapItemData(v)) then 
                SE:Play(SceneEffectConfig.EffectList.LuckyRingPoint, nil,nil,self.map_items[pos])
                table.insert(show_list,pos)
            end 

            self.map_item_mods[pos]:SetData(self.data:FixMapItemData(v))
            self.map_item_mods[pos]:SetPos(pos)
        end 
    end 

    self.data:FlushShowPoint(show_list)
end

--===========================TaoQuanNaJiMapItem===========================
TaoQuanNaJiMapItem = TaoQuanNaJiMapItem or DeclareMono("TaoQuanNaJiMapItem", UIWFlushPanel)
function TaoQuanNaJiMapItem:TaoQuanNaJiMapItem()
    self.pos_mark = false
    self.item = 0

    self.data = nil
end 

function TaoQuanNaJiMapItem:PosMarkComplete()
    self.pos_mark = true
end

function TaoQuanNaJiMapItem:PosMark()
    return self.pos_mark
end

function TaoQuanNaJiMapItem:ItemShowChange(index)
    for i = 1,7 do 
        if self["type"..i] ~= nil then 
            self["type"..i]:SetActive(index == i)
        end 
    end
    self.item = index
end

function TaoQuanNaJiMapItem:SetData(data)
    self:ItemShowChange(data.item_id)
    self.pos = Vector2.New(data.x,data.y)

    self.gameObject.name = self.pos.x .. "-" .. self.pos.y .."-"..self.item
    self.data = data
end

function TaoQuanNaJiMapItem:SetPos(pos)
    self.index_pos = pos
end

function TaoQuanNaJiMapItem:GetPos(pos)
    return self.index_pos
end

function TaoQuanNaJiMapItem:CheckShowPoint(data)
    if self.data ~= nil and self.data.item_id ~= nil then 
        if self.data.item_id > 0 and data.item_id == 0 then 
            -- 圈落地进行特效模拟，如果相同则生效
            if TaoQuanNaJiData.Instance:SimulateEffectCheck(data) then 
                return true
            end 
        end 
    end 
    return false
end

--===========================TaoQuanNaJiing===========================
TaoQuanNaJiing = TaoQuanNaJiing or DeclareMono("TaoQuanNaJiing", UIWFlushPanel)
function TaoQuanNaJiing:TaoQuanNaJiing()
end

function TaoQuanNaJiing:Roll()
    if self.ringAnim == nil then return end 
    self.ringAnim:SetInteger(APH("state"),2)
end