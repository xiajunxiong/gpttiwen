JoyStickData = JoyStickData or BaseClass()

function JoyStickData:__init()
	if JoyStickData.Instance ~= nil then
		Debuger.LogError("[JoyStickData] attempt to create singleton twice!")
		return
	end
	JoyStickData.Instance = self

    self.pos_change_click = SmartData.New({
        Click = Vector2.zero,
        FirstClick = Vector2.zero,
        -- GroundClick = Vector3.zero,
        StartJoyStick = false,
        timer = 0
        })
   
    self.start_area = {}
    self.is_hide = false
    -- self.mark_rad = 0
    self.mark_pos = Vector3.zero
    self.mark_timer = 0
end

function JoyStickData:GetPosClickInfo()
    return self.pos_change_click
end

function JoyStickData:SetPosClick(pos)
    if not self.pos_change_click.StartJoyStick then return end 

    self.pos_change_click.Click = pos
    self.pos_change_click.timer = self.pos_change_click.timer + 1
    if self.pos_change_click.timer > 100000 then 
        self.pos_change_click.timer = 0
    end 
end

function JoyStickData:SetFirstClick(pos)
    self.pos_change_click.FirstClick = pos
    if self.timer_start == nil and self.timer == nil then 
        self.timer_start = TimeHelper:AddDelayFrameTimer(function()
            self.timer = TimeHelper:AddRunFrameTimer(function ()
                local move_click = UnityEngine.Input.mousePosition
                local first = self.pos_change_click.FirstClick
                local dis =(first.x - move_click.x) * (first.x - move_click.x) + (first.y - move_click.y) * (first.y - move_click.y)

                local hit, pos, obj = Game.SceneTouchHelper.Raycast(nil, nil)
                local touch_role =  hit and obj ~= nil and obj.layer == GameObjLayer.ColliderRole

                self.pos_change_click.StartJoyStick = dis > 0 and UnityEngine.Input.anyKey and self:AreaCheck(self:PosChange(first)) and not touch_role

                if self.pos_change_click.StartJoyStick then 
                    self:SetPosClick(UnityEngine.Input.mousePosition)
                end 
            end,5 ,60,true)
        end,5)
    end 
end

function JoyStickData:SetAreaCheck(param_t)
    self.start_area.start = param_t.start
    self.start_area.limited = param_t.limited
    self.start_area.main_rect = param_t.main_rect
    self.start_area.main =  param_t.main
end

function JoyStickData:PosChange(pos)
    if self.start_area.main == nil then return Vector2.zero end 
    if self.start_area.main.rect == nil then return Vector2.zero end 

    return Vector2( pos.x / UnityEngine.Screen.width * self.start_area.main.rect.width,
        pos.y / UnityEngine.Screen.height * self.start_area.main.rect.height )
end

function JoyStickData:AreaCheck(pos)
    if self.start_area.start == nil or self.start_area.limited == nil then return false end 

    return ( self.start_area.start.x <= pos.x and pos.x <= self.start_area.limited.x ) 
        and ( self.start_area.start.y <= pos.y and pos.y <= self.start_area.limited.y ) 
end

function JoyStickData:ClearTimer()
    if self.timer ~= nil then 
        TimeHelper:CancelTimer(self.timer)
        self.timer = nil
    end 

    if self.timer_start ~= nil then 
        TimeHelper:CancelTimer(self.timer_start)
        self.timer_start = nil
    end 
end

function JoyStickData:IsFrameTimer()
    return self.timer ~= nil or self.timer_start ~= nil
end

function JoyStickData:CheckStartJoyStick()
    if self.pos_change_click.FirstClick == Vector2.zero then
        return false
    end 

    local move_click = UnityEngine.Input.mousePosition
    local first = self.pos_change_click.FirstClick
    local dis =(first.x - move_click.x) * (first.x - move_click.x) + (first.y - move_click.y) * (first.y - move_click.y)

    local hit, pos, obj = Game.SceneTouchHelper.Raycast(nil, nil)
    local touch_role =  hit and obj ~= nil and obj.layer == GameObjLayer.ColliderRole

    self.pos_change_click.StartJoyStick = (dis > 0 or self.pos_change_click.StartJoyStick) and self:AreaCheck(self:PosChange(first)) and not touch_role

    return self.pos_change_click.StartJoyStick
end

function JoyStickData:CheckAreaJoyStick()
    return self:AreaCheck(self:PosChange(self.pos_change_click.FirstClick))
end

function JoyStickData:ClearJoyStickInfo()
    self.pos_change_click.Click = Vector2.zero
    self.pos_change_click.FirstClick = Vector2.zero
    self.pos_change_click.StartJoyStick = false
    self:ClearTimer()
end

function JoyStickData:SceneChange()
    if RoleSettingData.Instance:IsShowJoyStick() then 
        self:ClearJoyStickInfo()
    end 
end

function JoyStickData:HideJoyStick()
    RoleSettingData.Instance.joy_is_on.val = false
    self.is_hide = true
end

function JoyStickData:ShowJoyStick()
    RoleSettingData.Instance.joy_is_on.val = RoleSettingData.Instance.role_setting_data[ROLE_SETTING_TYPE.SET_INFO_IS_JOY] 
    self.is_hide = false
end

function JoyStickData:IsHideJoyStick()
    return self.is_hide
end


function JoyStickData:AdjustMovePoint(param_t)
    local end_point = Vector3.zero
    local out_param_t = {}

    local adjust_done = false
    local adjust
    -- 每次输入
    for i = 1,20 do 
        local plus_ras = math.rad(5*i)
        end_point,adjust_done = self:CheckMovePoint(plus_ras,param_t)
        if adjust_done and self.mark_timer < 3 then 
            break
        end 
    end 

    if self.mark_pos == end_point then 
        self.mark_timer = self.mark_timer + 1
    else 
        self.mark_timer = 0
        self.mark_pos = end_point
    end 

    return end_point,out_param_t
end

function JoyStickData:CheckMovePoint(rad,param_t)
    local pos_change = Vector2.zero
    local move_dir = MainPlayer.root_node:GetEulerAngles().y * (math.pi / 180)
    local cur_pos = MainPlayer:GetPos()
    local end_pos = nil
    local succes = false
    local flag_pos = false
    for i = 1, 2 do 
        local oper = i == 1 and 1 or -1
        local end_x = cur_pos.x + (math.sin(move_dir + rad * oper) * (param_t.distance/6400) * 1.5 )
        local end_z = cur_pos.z + (math.cos(move_dir + rad * oper) * (param_t.distance/6400) * 1.5 )

        local end_y,succes = SceneCtrl.Instance:CurScene():GetPosHeight(end_x, end_z)
        end_pos = Vector3.New(end_x,end_y,end_z)

        if MainPlayer.nav_agent.isOnNavMesh and MainPlayer.nav_mesh_path ~= nil then 
            flag_pos = MainPlayer.nav_agent:CalculatePath(end_pos, MainPlayer.nav_mesh_path)
        else 
            flag_pos = not succes
        end 
        if flag_pos then 
            break
        end 
    end 

    -- SE:Play(SceneEffectConfig.EffectList.ClickEffect, end_pos)
    return end_pos,flag_pos
end