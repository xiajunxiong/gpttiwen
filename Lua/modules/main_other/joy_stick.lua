MainJoyStick = MainJoyStick or DeclareMono("MainJoyStick", UIWFlushPanel)
function MainJoyStick:MainJoyStick()

    self.data = JoyStickData.Instance
    self.data_cares = {
        {data = self.data.pos_change_click, func = self.FlushJoyShow, keys = {"StartJoyStick","timer"}},
        {data = RoleSettingData.Instance.joy_is_on, func = self.OnJoyShow},
    }
    self.joystick_angle = 0
    self.joystick_last_target_pos = Vector2.New(0, 0)	

    self.stick_off = Vector3.zero

    self.data:SetAreaCheck({
        start = self.start_area.localPosition,
        limited = Vector2.New(self.start_area.localPosition.x + self.start_area.rect.width,
            self.start_area.localPosition.y + self.start_area.rect.height),
        main_rect = self.main.rect,
        main = self.main
    })

    self.main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")
    self.start_move = false
end 

function MainJoyStick:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function MainJoyStick:FlushJoyShow()

    local first = self.data:PosChange(self.data.pos_change_click.FirstClick)
    local move_click = self.data:PosChange(self.data.pos_change_click.Click)

    if not self.data.pos_change_click.StartJoyStick then 
        self:MoveComplete()
        return 
    end 
    
    if not self.start_move and self.click_eff == nil then 
        self.click_eff = self.EffectTool:Play("4161003")
        self.start_move = true
    end 

    self.total_group.alpha = self.data.pos_change_click.StartJoyStick and 1 or 0.4
    self.show_group.alpha = self.data.pos_change_click.StartJoyStick and 1 or 0.4

    self.total:SetLocalPosition(first)
    local dis = (first.x - move_click.x) * (first.x - move_click.x) + (first.y - move_click.y) * (first.y - move_click.y)
    if dis > 6400 then 
        local line_y = (move_click.y-first.y)
        local line_x = (move_click.x-first.x)

        local angle = math.atan2(line_y, line_x)
        local limited_y = 80 * math.sin(angle)
        local limited_x = 80 * math.cos(angle)

        local pos = Vector2.New(first.x + limited_x,first.y + limited_y)

        self.show:SetLocalPosition(pos)
        self.stick_off = Vector2.New(limited_x,limited_y)
    else 
        self.show:SetLocalPosition(move_click)
        self.stick_off = Vector2.New(move_click.x-first.x,move_click.y-first.y)
    end 

    self:MoveFlush(self.stick_off)
end

function MainJoyStick:MoveFlush(stick_off)

    if MainPlayer == nil then return end 
    
    local dis = stick_off.x * stick_off.x +  stick_off.y *stick_off.y
    local distance = dis > 6400 and 6400 or dis
    -- distance  = distance < 1000 and 0 or distance

    if distance < 1000 then return end 

    local dir_ho = Vector3.New(stick_off.x,0,stick_off.y)

    local qua = self.main_camera:GetLocalEulerAngles()
    local dir_vec = Quaternion.Euler(0,qua.y,0) * dir_ho
    local dir = dir_vec:Normalize()

    local role_pos =  MainPlayer:GetPos()
    local pos_x = role_pos.x + dir.x * 2 * (distance/6400)
    local pos_z = role_pos.z + dir.z * 2 * (distance/6400)

    local param_t = nil
    local end_y,succes = SceneCtrl.Instance:CurScene():GetPosHeight(pos_x, pos_z)
    local end_point = Vector3.New(pos_x,end_y,pos_z)
    
    local flag_pos = not succes -- 
    if MainPlayer.nav_agent.isOnNavMesh and MainPlayer.nav_mesh_path ~= nil then 
        flag_pos = MainPlayer.nav_agent:CalculatePath(end_point, MainPlayer.nav_mesh_path)
    end 
    local adjust = false
    local angle = math.atan2(dir_ho.z, dir_ho.x)
    if not flag_pos then 
        end_point,param_t = self.data:AdjustMovePoint(
            {
                stick_off = stick_off,
                camera_qua = qua,
                distance = distance,
                is_ant = not (math.deg(angle) >= -90 and math.deg(angle) < 90),
                default = Vector3.New(pos_x,end_y,pos_z),
            }
        )
        adjust = true
    end 

    local dis = Vector3.Distance2(MainPlayer:GetPos(), MainPlayer:GetMoveEndRealPos() or MainPlayer:GetPos())
    local an
    if self.joystick_angle == 0 then
        an = true
    else
        an = math.abs(angle - self.joystick_angle) >= 0.26
    end

    local exe_stick = ((dis <= 4 or adjust) or an)
    
    if exe_stick then
        self.joystick_angle = angle

        if (self.joystick_last_target_pos.x ~= end_point.x or self.joystick_last_target_pos.z ~= end_point.z) then
			self.joystick_last_target_pos = end_point
            MainPlayer:MoveTo(end_point,true)
        end 
    end 
end


function MainJoyStick:MoveComplete()
    if self.stick_off == Vector2.zero then return end 

    local timer = 4 
    local mark_x = self.stick_off.x/timer
    local mark_y = self.stick_off.y/timer
    self.ShowTimer = TimeHelper:AddRunFrameTimer(function()
        self.stick_off.x = self.stick_off.x - mark_x
        self.stick_off.y = self.stick_off.y - mark_y
        timer = timer - 1
        if timer == 0 then
            self.stick_off = Vector3.zero
        end 

        self:MoveFlush(self.stick_off)
    end, 1,timer)

    self.start_move = false
    if self.click_eff ~= nil then 
        self.EffectTool:Stop(self.click_eff)
        self.click_eff = nil
    end 
    
    self:ResetPos()
end

function MainJoyStick:ChangePos(pos)
    return self.data:PosChange(pos)
    -- return Vector2( pos.x / UnityEngine.Screen.width * self.main.rect.width,
    --     pos.y / UnityEngine.Screen.height * self.main.rect.height )
end

function MainJoyStick:OnJoyShow()
    self:ResetPos()
end

function MainJoyStick:ResetPos()
    self.total:SetLocalPosition(self.start_pos:GetLocalPosition())
    self.show:SetLocalPosition(self.start_pos:GetLocalPosition())

    self.total_group.alpha = self.data.pos_change_click.StartJoyStick and 1 or 0.4
    self.show_group.alpha = self.data.pos_change_click.StartJoyStick and 1 or 0.4
end