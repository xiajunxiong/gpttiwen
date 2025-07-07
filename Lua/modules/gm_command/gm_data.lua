GmData = GmData or BaseClass()
local Input = UnityEngine.Input
function GmData:__init()
    if GmData.Instance ~= nil then
        Debuger.LogError("[GmData] attempt to create singleton twice!")
        return
    end
    GmData.Instance = self
    
    self.view_data = SmartData.New({
        hide_main = false,
        hide_hp = false,
        hide_popup = false,
        free_move = false,
        get_item = false,
        stop_battle = false,
        lock_follow = false,
    })
    self.btn_data = {
        {index = 1, name = "隐藏主界面", show_name = "显示主界面",key = "hide_main"},
        {index = 2, name = "隐藏血条", show_name = "显示血条", key = "hide_hp"},
        {index = 3, name = "隐藏战斗飘字", show_name = "显示战斗飘字", key = "hide_popup"},
        {index = 4, name = "非战斗自由视角", show_name = "关闭自由视角",key = "free_move"},
        {index = 5, name = "强制结束战斗", show_name = "强制结束战斗",key = "stop_battle"},
        {index = 6, name = "快速获取道具", show_name = "关闭快速获取道具", key = "get_item"},
        {index = 7, name = "锁定视角并跟随\n(锁定后再关闭自由视角)", show_name = "关闭锁定和跟随", key = "lock_follow"},
    }
    self.fast_click_time = Time.realtimeSinceStartup
	self.fast_click_num = 0
    self.fast_click_check = 0.3
    
    self.main_camera = nil
    self.brain_com = nil

    self.double_touch_lastDis = 0
    self.move_speed = 2.5
    self.stretching_speed = 20
    self.fast_click_need = 3
    self.speed = 2.5
end 
function GmData:__delete()
    
end
function GmData:GetBtnState(data)
    return self.view_data[data.key]
end

function GmData:Check()
    Cinemachine.loadassembly()
    if self.main_camera == nil then
        self.main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")
    end
    if self.brain_com == nil then
        self.brain_com = self.main_camera:GetComponent(typeof('Cinemachine.CinemachineBrain'))
    end
end

function GmData:CheckAndOpen()
    self:Check()
    if self.view_data.hide_main == false then
        return
    end
    if self.fast_click_num == 0 then
        self.fast_click_time = Time.realtimeSinceStartup
    end
    self.fast_click_num = self.fast_click_num + 1
    if self.fast_click_num >= self.fast_click_need then    
        if Time.realtimeSinceStartup - self.fast_click_time <= self.fast_click_check then
            local agent_adapt_info = GLOBAL_CONFIG:AgentAdaptInfoById(GLOBAL_CONFIG:PkgInfo().agent_id)
            if agent_adapt_info then
                if agent_adapt_info.is_gm == 1 then
                    -- local account = LoginData.Instance:AccountData()
                -- if account.account_type == PLAT_ACCOUNT_TYPE_TEST or IS_EDITOR then
                    ViewMgr:OpenView(GmWindow)
                -- end
                end
             end
        end
        self.fast_click_num = 0
        self.fast_click_time = Time.realtimeSinceStartup
    end
end

function GmData:updateTouchTranslate()
    if self.view_data.free_move == false then
        return
    end
    local mZ = 0
    if IS_EDITOR then
        mZ = Input.GetAxis("Mouse ScrollWheel");
    else
        mZ = self:TranslateZ()
    end
    if mZ ~= 0 then
        self.main_camera.transform:Translate(Vector3.New(0, 0, mZ):Mul(Time.deltaTime * self.stretching_speed))
    end
end
function GmData:updateTouchRotate()
    if Input.GetMouseButton(0) and self.view_data.free_move == true and Input.touchCount < 2
    and self.view_data.lock_follow == false then
        if self.main_camera then
            -- local angle = self.main_camera.transform.localEulerAngles
            -- local eY = angle.y
            -- local eX = angle.x
            local mX = Input.GetAxis("Mouse X")
            local mY = Input.GetAxis("Mouse Y")
            -- local y = eY + (mX * self.move_speed)
            -- local x = eX + (mY * -self.move_speed)
            --x = x > 180 and x - 360 or x
            --y = y > 180 and y - 360 or y
            local ax = (mY * -self.move_speed)
            local ay = (mX * self.move_speed)
            --[[ if x > 40 then
                ax = ax + (40 - x)
            elseif x < 10 then
                ax = ax + (10 - x)
            end
            if y > 270 then
                ay = ay - (y + 90)
            elseif y < 90 then
                ay = ay - (y - 90)
            end ]]
            self.main_camera.transform:RotateAround(
                MainPlayer.root_node.transform.localPosition,
                self.main_camera.transform.right,
                ax
            )
            self.main_camera.transform:RotateAround(MainPlayer.root_node.transform.localPosition, Vector3.up, ay)
        end
    end
end

function GmData:TranslateZ()
    local touch_1 = Vector3.zero;
    local touch_2 = Vector3.zero;
    local value = 0
    if( Input.touchCount == 2 and (Input.GetTouch(0).phase == TouchPhase.Moved or Input.GetTouch(1).phase == TouchPhase.Moved)) then
        touch_1 = Input.GetTouch(0).position
        touch_2 = Input.GetTouch(1).position
    else
        self.double_touch_lastDis = 0;
    end
    if (touch_1 ~= Vector3.zero and touch_2 ~= Vector3.zero) then
        local distance = Vector2.Distance(touch_1, touch_2);
        if self.double_touch_lastDis ~= 0 then
            value = (distance - self.double_touch_lastDis) * 0.09;
        end
        self.double_touch_lastDis = distance;
    end
    return value
end
function GmData:ClearFollow()
    self:Check()
    if self.brain_com then
        self.brain_com.enabled = false
    end
    --[[ local vir_cam_obj2 = SceneMgr:GetFreeLookCamera(SceneManager.FreeLookCamType.FreeLookCamera)
	local vir_cam2 = vir_cam_obj2:GetComponent(typeof(Cinemachine.CinemachineFreeLook))
	vir_cam2.Follow = nil
	local vir_cam_obj3 = SceneMgr:GetFreeLookCamera(SceneManager.FreeLookCamType.FreeLookCamera2)
	local vir_cam3 = vir_cam_obj3:GetComponent(typeof(Cinemachine.CinemachineFreeLook))
	vir_cam3.Follow = nil ]]
end
function GmData:ResetCamera()
    self:Check()
    -- SceneCtrl.Instance:CurScene():ResetCamera()
    if self.view_data.lock_follow == false
    and self.view_data.free_move == false and self.brain_com then
        self.brain_com.enabled = true
    end
    if self.view_data.lock_follow == true then
        self.distance = self.main_camera.transform.position:Sub(MainPlayer.root_node.transform.position)
    else
        self.distance = nil
    end
end
function GmData:CameraFollow()
    if self.view_data.lock_follow == true and self.view_data.free_move == false
     and self.main_camera and MainPlayer and MainPlayer.root_node and self.distance then
        self.main_camera.transform.position = MainPlayer.root_node.transform.position:Add(self.distance)
    end
end