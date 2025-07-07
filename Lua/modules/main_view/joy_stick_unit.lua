
JoyStickUnit = JoyStickUnit or DeclareMono("JoyStickUnit", UIWFlushPanel)

function JoyStickUnit:JoyStickUnit()
    self.joystick_angle = 0
    self.joystick_last_target_pos = Vector2.New(0, 0)	
end 

function JoyStickUnit:OnDestroy()
    UIWFlushPanel.OnDestroy(self)

    if self.stick_update ~= nil then
        TimeHelper:CancelTimer(self.stick_update)
        self.stick_update = nil
    end 

end

function JoyStickUnit:OnJoyStickDrag()
    self.stick_off = self:getStickOffset()	
end

function JoyStickUnit:getStickOffset()
	if self.stick_init_pos == nil then         --初始化取到的位置会有问题，改在第一次收到事件的时候获取
		self.stick_init_pos = self.joystick:GetLocalPosition()
	end

	self.input_t = Vector2(UnityEngine.Input.mousePosition.x - UnityEngine.Screen.width /2,
        UnityEngine.Input.mousePosition.y - UnityEngine.Screen.height/2)
    
	local touch_off = self.input_t - self.stick_init_pos

	local max_off_sqr = 10000
	if (touch_off:SqrMagnitude()) > max_off_sqr then
		touch_off = touch_off:Normalize() * 100
	end

    return touch_off
end

function JoyStickUnit:OnJoyStickPressUp()
    if self.stick_update ~= nil then
        TimeHelper:CancelTimer(self.stick_update)
        self.stick_update = nil
    end
    LogError("press up")
    self.show:SetLocalPosition(Vector3.zero)
    MainPlayer:StopMove()
end

function JoyStickUnit:OnJoyStickPressDown()
    self.stick_off = self:getStickOffset()

    self.stick_update = TimeHelper:AddRunTimer(function()
        self.show:SetLocalPosition(self.stick_off)

        local dir_vec = Quaternion.Euler(0,135,0) * Vector3.New(self.stick_off.x,0,self.stick_off.y)
        local dir = dir_vec:Normalize()

        local role_pos =  MainPlayer:GetPos()
        local pos_x = role_pos.x + dir.x * 4
        local pos_z = role_pos.z + dir.z * 4

        local end_point = Vector3.New(pos_x, SceneCtrl.Instance:CurScene():GetPosHeight(pos_x, pos_z),pos_z)


        local angle = math.atan2(self.stick_off.y, self.stick_off.x)
        local dis = Vector3.Distance2(MainPlayer:GetPos(), MainPlayer:GetMoveEndRealPos() or MainPlayer:GetPos())
        local an
        if self.joystick_angle == 0 then
            an = true
        else
            an = math.abs(angle - self.joystick_angle) >= 0.26
        end
        local exe_stick = ((dis <= 4) or an)
    
        if exe_stick then
            self.joystick_angle = angle

            if (self.joystick_last_target_pos.x ~= end_point.x or self.joystick_last_target_pos.z ~= end_point.z) then
				self.joystick_last_target_pos = end_point
                MainPlayer:MoveTo(end_point,true)
            end 
        end 

    end,0.01)
end