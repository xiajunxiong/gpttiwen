-- =========================== 山海花灯 ===========================
MSFlowerLampView = MSFlowerLampView or DeclareView("MSFlowerLampView","serveractivity/mountain_sea/ms_flower_lamp_view",Mod.MountainSea.FlowerLampMS)
VIEW_DECLARE_LEVEL(MSFlowerLampView, ViewLevel.L0)
function MSFlowerLampView:MSFlowerLampView()
    self.Panel:SetView(self:GetType())
end

MSFlowerLampPanel = MSFlowerLampPanel or DeclareMono("MSFlowerLampPanel",UIWFlushPanel)
function MSFlowerLampPanel:MSFlowerLampPanel()
    self.data = ActFlowerLampData.Instance
    self.act_info = self.data.ms_flower_lamp_data
    self.game_state = FlowerLampData.NoticeType.Normal

    self.data_cares = {
        {data = self.act_info, func = self.FlushBaseInfo, init_call = false, keys = {"base_info"}},
        {data = self.act_info, func = self.FlushGameInfo, init_call = false, keys = {"game_info"}},
        {data = self.act_info, func = self.FlushItemInfo, init_call = false, keys = {"item_info"}},
    }

    self.line_cfg = {
        left = {x = -214, y = 93, len = 134},
        --center = {x = -12, y = 150, len = 134},
        right = {x = 184, y = 93, len = 134},
    }

    self.speed = 1                      -- 速度倍数
    self.range = 1                      -- 范围倍数
    self.line_max_length = 1500
    self.line_min_length = 134
    self.line_trigger_range = 20        -- 触发半径，单位像素
    self.line_move_speed = 30           -- 绳子本身移动速度
end

function MSFlowerLampPanel:SetView(view)
    self.view_type = view
end

function MSFlowerLampPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.data.flower_lamp_look_flag = true
    self:FlushPanel()
end

function MSFlowerLampPanel:FlushPanel()
    self:FlushBaseInfo()
    self:FlushGameInfo()
    self:FlushItemInfo()
end

function MSFlowerLampPanel:FlushBaseInfo()
    UH.SetText(self.RemainCount,self.data:FlowerLampRemainCount())
end

function MSFlowerLampPanel:FlushGameInfo()
    if self.in_the_close then
        return
    end
    self.game_state = self.act_info.game_info.notice_type or self.game_state
    local notice_type = self.game_state
    local time = 0
    if notice_type == FlowerLampData.NoticeType.Normal or notice_type == FlowerLampData.NoticeType.GameOver then
        if self.task_info_off then
            self.TaskAnima:Play("flower_lamp_task_info_on")
            self.task_info_off = false
        end
        self.StartBtnObj:SetActive(true)
        UH.SetText(self.GameCountDown,"0")
        -- 游戏结束事件
        if notice_type == FlowerLampData.NoticeType.GameOver then
            self.act_info.item_info = {}
            self.LampMgr:Stop(true)
            self.BuffTime:SetObjActive(false)
        end
    elseif notice_type == FlowerLampData.NoticeType.StartGame or notice_type == FlowerLampData.NoticeType.InTheGame then
        if not self.task_info_off then
            self.TaskAnima:Play("flower_lamp_task_info_off")
            self.task_info_off = true
        end
        self.StartBtnObj:SetActive(false)
        time = self.act_info.game_info.timestamp
        self.LampMgr:Run()
        UH.SetText(self.GameScore, self.act_info.game_info.score or 0)
    end
    self.GameCountDown:CloseCountDownTime()
    self.GameCountDown:StampTime(time,TimeType.Type_Special_5,"")

    self.GameScoreInfo:SetActive(notice_type == FlowerLampData.NoticeType.StartGame or notice_type == FlowerLampData.NoticeType.InTheGame)
end

function MSFlowerLampPanel:FlushItemInfo()
    local lamp_id_list = self.act_info.item_info.lamp_id_list
    self.LampMgr:CreateLampList(lamp_id_list or {})
    self.act_info.item_info.lamp_id_list = {}
end

function MSFlowerLampPanel:HandleNotice(notice_type)
    
end

function MSFlowerLampPanel:GetMoveSpeed()
    return self.line_move_speed * self.speed
end

function MSFlowerLampPanel:GetTriggerRange()
    return self.line_trigger_range * self.range
end

-- 获取鱼钩触发器的位置
function MSFlowerLampPanel:GetTriggerPos()
    local pos = UH.GetRootPosition(self.LineTrigger)
    return pos
end

-- 获取屏幕触摸点
function MSFlowerLampPanel:GetTouchPos(target_rect)
    target_rect = target_rect or self.PanelRect
    if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
    local _, pos =
        UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
        target_rect,
        UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil
    )
    return pos
end

-- 获取触摸点相对于绳子的角度
function MSFlowerLampPanel:GetTouchAngle(dir_vector)
    local vector = dir_vector.normalized
    local x = vector.x
    local y = vector.y
    local num = math.atan2(x,y)         -- 弧度
    local angle = num * 180 / math.pi   -- 角度 = 弧度 * 360 / 2PI 
    return angle
end

-- 设置绳子的角度
function MSFlowerLampPanel:SetLineAngleZ(angle_z)
    UH.LocalRotate(self.LineRect,Quaternion.Euler(0,0,angle_z))
end

-- 设置绳子的高
function MSFlowerLampPanel:SetLineH(h)
    self.LineRect:SetSizeWithCurrentAnchors(1,h)
end

function MSFlowerLampPanel:GetLineH()
    return self.LineRect.sizeDelta.y
end

-- 是否满足触发条件
function MSFlowerLampPanel:IsTrigger(rect,line_trigger_obj)
    local rect_pos = UH.GetRootPosition(rect.gameObject)
    local line_pos = UH.GetRootPosition(line_trigger_obj)
    local dir = Vector3.New(line_pos.x - rect_pos.x,line_pos.y - rect_pos.y,0)
    local angle = self:GetTouchAngle(dir)
    local r = self:GetTriggerRange() -- 绳子触发半径
    local w = rect.sizeDelta.x / 2
    local h = rect.sizeDelta.y / 2
    local rect_angle = self:GetTouchAngle(Vector3.New(w,h,0))  -- 中心点与顶点的角度
    rect_angle = math.floor(rect_angle)
    local distance = Vector2.Distance(rect_pos,line_pos)
    local max_dis = 0
    local new_angle = math.abs(angle)
    if new_angle <= rect_angle or new_angle >= 180 - rect_angle then
        if new_angle <= rect_angle then
            local angle_rad = math.rad(new_angle)
            max_dis = h / math.cos(angle_rad) + r
        else
            local angle_rad = math.rad(180 - new_angle)
            max_dis = h / math.cos(angle_rad) + r
        end
    else
        if new_angle <= 90 then
            local angle_rad = math.rad(90 - new_angle)
            max_dis = w / math.cos(angle_rad) + r
        else
            local angle_rad = math.rad(new_angle - 90)
            max_dis = w / math.cos(angle_rad) + r
        end
    end
    max_dis = math.floor(max_dis)
    --LogError("Test:",new_angle,rect_angle,distance,max_dis,rect_pos,line_pos)
    --LogError(angle,w,H)
    return distance <= max_dis
end

function MSFlowerLampPanel:Move()
    self.move_dir = 1
    self.is_trigged = false
    self.run_ht = Runner.Instance:RunUntilTrue(BindTool.Bind(self.MoveHandle,self))
end

function MSFlowerLampPanel:MoveHandle()
    local line_h = self.LineRect.sizeDelta.y
    local new_line_h = line_h + self:GetMoveSpeed() * self.move_dir
    if self.move_dir == 1 and new_line_h >= self.line_max_length then
        self.move_dir = -1
        new_line_h = self.line_max_length
    end
    if self.move_dir == -1 and new_line_h <= self.line_min_length then
        self.move_dir = nil
        new_line_h = self.line_min_length
    end
    self:SetLineH(new_line_h)
    if self.move_dir then
        local tri2_pos = self.LineTrigger:GetLocalPosition()
        tri2_pos.x = tri2_pos.x + self.tri_dir_pos.x * self:GetMoveSpeed() * self.move_dir
        tri2_pos.y = tri2_pos.y + self.tri_dir_pos.y * self:GetMoveSpeed() * self.move_dir
        self.LineTrigger:SetLocalPosition(tri2_pos)
    else
        self.LineTrigger:SetLocalPosition(self.start_tri2_pos)
    end
    if self.move_dir and self.move_dir > 0 and self.is_trigged == false and self:CheckTrigger() then
        self.is_trigged = true
        self.move_dir = -1
    end
    if not self.move_dir then
        if self.tri_mono then
            self.tri_mono.gameObject.transform:SetParent(self.LampMgr.gameObject.transform)
            self.LampMgr:DeleteLamp(self.tri_mono)
            self.tri_mono = nil
        end
        self.LineRect.gameObject:SetActive(false)
        self.RoleAnima:SetInteger(APH("state"),2)
        TimeHelper:CancelTimer(self.time_ht)
        self.time_ht = Invoke(function ()
            self.playing = false
        end,0.5)
        return true
    end
end

function MSFlowerLampPanel:CheckTrigger()
    local lamp_list = self.LampMgr.lamp_list or {}
    for i,mono in pairs(lamp_list) do
        local rect = mono:GetTriggerRect()
        if self:IsTrigger(rect,self.LineTrigger) then
            self:TriggerEvent(mono)
            return true
        end
    end
end
-- @ 抓住花灯事件
function MSFlowerLampPanel:TriggerEvent(mono)
    local data = mono:GetData()
    PublicPopupCtrl.Instance:Center(mono:GetBuffDesc())
    self.act_info.item_info.lamp_list[data.type] = self.act_info.item_info.lamp_list[data.type] - 1

    ActFlowerLampCtrl.Instance:FlowerReq(FlowerLampData.ReqType.GraspLamp, data.type)
    local buff_type, buff_time, buff_effect = mono:GetBuffType()
    if buff_type == FlowerLampData.BuffType.Speed then
        self.speed = 1 * buff_effect
        self.range = 1
        self.BuffTime:SetObjActive(true)
        self.BuffTime:SetData({time = buff_time,text = Language.FlowerLamp.Buff1,func = function ()
            self.BuffTime:SetObjActive(false)
            self.speed = 1
        end})
        self.BuffTime:Run()
    elseif buff_type == FlowerLampData.BuffType.Range then
        self.range = 1 * buff_effect
        self.speed = 1
        self.BuffTime:SetObjActive(true)
        self.BuffTime:SetData({time = buff_time,text = Language.FlowerLamp.Buff2,func = function ()
            self.BuffTime:SetObjActive(false)
            self.range = 1
        end})
        self.BuffTime:Run()
    elseif buff_type == FlowerLampData.BuffType.Normal then
        -- 刷新下源数据
        -- local old_base_info = TableCopy(self.act_info.base_info)
        -- old_base_info.sum_score = old_base_info.sum_score + buff_effect
    end
    mono.gameObject.transform:SetParent(self.LineTrigger.transform)
    mono.gameObject:SetLocalPosition(Vector3.New(0,-30,0))
    mono:State(1)
    mono:FlushShow()
    self.tri_mono = mono
end

function MSFlowerLampPanel:OnWaterZoneClick()
    if self.playing or (self.game_state ~= FlowerLampData.NoticeType.StartGame and self.game_state ~= FlowerLampData.NoticeType.InTheGame)  then
        return
    end
    local pos = self:GetTouchPos()
    if pos.x <= -12 then
        UH.LocalScale(self.RoleRect,Vector3.New(1,1,1))
    else
        UH.LocalScale(self.RoleRect,Vector3.New(-1,1,1))
    end
    self.playing = true
    self.RoleAnima:SetInteger(APH("state"),1)
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        self:HandleLinePos(pos)
        local dir_pos = Vector3.New(pos.x - self.line_pos.x,pos.y - self.line_pos.y,0)
        local angle = self:GetTouchAngle(dir_pos)
        local angle_2 = math.abs(angle - 90) % 180 -- 与绳子水平（x轴）的夹角
        self:HandleLineMaxLength(angle_2,pos)
        self:SetLineAngleZ(180 - angle)     -- 绳子反向旋转
        self.LineRect.gameObject:SetActive(true)
        self.tri_dir_pos = dir_pos.normalized
        -- 计算触发体移动了多少位置
        local pos_x = self.line_min_length * math.sin(math.rad(180 - math.abs(angle)))
        local pos_y = self.line_min_length - self.line_min_length * math.cos(math.rad(180 - math.abs(angle)))
        self.start_tri2_pos = self.LineTrigger:GetLocalPosition()
        self.start_tri2_pos.x = self.tri_default_pos.x + (angle > 0 and pos_x or -pos_x)
        self.start_tri2_pos.y = self.tri_default_pos.y + pos_y
        self.LineTrigger:SetLocalPosition(self.start_tri2_pos)
        self:Move()       
    end,1 / 1.5)
end

-- 根据点击位置处理绳子的位置
function MSFlowerLampPanel:HandleLinePos(click_pos)
    local cfg
    if click_pos.x <= -12 then
        cfg = self.line_cfg.left
    else
        cfg = self.line_cfg.right
    end
    local pos = Vector3.New(cfg.x, cfg.y, 0)
    self.LineRect.gameObject:SetLocalPosition(pos)
    self.line_min_length = cfg.len
    self:SetLineH(self.line_min_length)
    self.line_pos = pos
    -- 相对于屏幕左下角的pos
    local vec_2 = {}
    vec_2.x = pos.x + 712
    vec_2.y = pos.y + 375
    self.line_vec2 = vec_2
    local tri_pos = Vector3.New(cfg.x, cfg.y - cfg.len, 0)
    self.tri_default_pos = tri_pos
    self.LineTrigger:SetLocalPosition(self.tri_default_pos)
end

-- 计算绳子的最大长度
function MSFlowerLampPanel:HandleLineMaxLength(angle,click_pos)
    local line_tan = click_pos.x > self.line_pos.x and (self.line_vec2.y / (1426 - self.line_vec2.x)) or (self.line_vec2.y / self.line_vec2.x)
    local angle_rad = math.rad(angle)
    local tan = math.tan(angle_rad)
    local len = 0
    if tan < line_tan then
        local cos = math.cos(angle_rad)
        local w = click_pos.x > self.line_pos.x and 1426 - self.line_vec2.x or self.line_vec2.x
        len = w / cos
    else
        local sin = math.sin(angle_rad)
        len = self.line_vec2.y / sin
    end
    self.line_max_length = len
end

function MSFlowerLampPanel:OnCloseClick()
    if self.act_info.game_info.notice_type == FlowerLampData.NoticeType.StartGame or 
    self.act_info.game_info.notice_type == FlowerLampData.NoticeType.InTheGame then
        PublicPopupCtrl.Instance:AlertTip(Language.FlowerLamp.ClsoeTip,function ()
            self.in_the_close = true
            ActFlowerLampCtrl.Instance:FlowerReq(FlowerLampData.ReqType.ForceClose)
            ViewMgr:CloseView(self.view_type)
        end)
    else
        ViewMgr:CloseView(self.view_type)
    end
    self.data.flower_lamp_end_flag = nil
end

function MSFlowerLampPanel:OnDestroy()
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
        self.run_ht = nil
    end
    TimeHelper:CancelTimer(self.time_ht)
    self.GameCountDown:CloseCountDownTime()
    self.act_info.item_info = {}
    self.act_info.game_info = {}
    UIWFlushPanel.OnDestroy(self)
end

function MSFlowerLampPanel:OnTaskCaseClick()
    if not self.task_info_off then
        self.TaskAnima:Play("flower_lamp_task_info_off")
        self.task_info_off = true
    else
        self.task_info_off = false
        self.TaskAnima:Play("flower_lamp_task_info_on")
    end
end

function MSFlowerLampPanel:OnStartGameClick()
    if self.data:FlowerLampRemainCount() > 0 then
        ActFlowerLampCtrl.Instance:FlowerReq(FlowerLampData.ReqType.StartGame)
        UH.SetText(self.RemainCount,self.data:FlowerLampRemainCount() - 1)
    else
        PublicPopupCtrl.Instance:Error(Language.FlowerLamp.StartGameTip)
    end
end


MSFlowerLampTaskPanel = MSFlowerLampTaskPanel or DeclareMono("MSFlowerLampTaskPanel",UIWFlushPanel)
function MSFlowerLampTaskPanel:MSFlowerLampTaskPanel()
    self.data = ActFlowerLampData.Instance
    self.act_info = self.data.ms_flower_lamp_data
    self.task_type = 1

    self.data_cares = {
        {data = self.act_info, func = self.FlushPanel, init_call = false, keys = {"base_info","game_info"}},
    }
end

function MSFlowerLampTaskPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.RedPoint:SetObjActive(false)
    self:FlushPanel()
end

function MSFlowerLampTaskPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self:FlushPanel()
end

function MSFlowerLampTaskPanel:FlushPanel()
    local task_data,day_task_list,act_task_list = self.data:GetFlowerLampTaskData()
    self.task_info = {}
    self.task_info[1] = day_task_list
    self.task_info[2] = act_task_list
    local task_list = self.task_info[self.task_type]
    self.TaskList:SetData(task_list)
    if self.task_type == 1 then
        if self.act_info.game_info.notice_type == FlowerLampData.NoticeType.StartGame or self.act_info.game_info.notice_type == FlowerLampData.NoticeType.InTheGame then
            UH.SetText(self.ScoreDesc,string.format(Language.FlowerLamp.DayTaskScore, self.act_info.game_info.score or 0))
        else
            UH.SetText(self.ScoreDesc,string.format(Language.FlowerLamp.DayTaskScore2, self.act_info.base_info.last_score or 0))
        end
    else
        UH.SetText(self.ScoreDesc,string.format(Language.FlowerLamp.ActTaskScore, self.act_info.base_info.sum_score or 0))
    end
    
    local num_l = self.data:FlowerLampTaskRemind(day_task_list)
    local num_r = self.data:FlowerLampTaskRemind(act_task_list)
    self.RedL:SetNum(num_l)
    self.RedR:SetNum(num_r)
    self.RedPoint:SetNum(num_l + num_r)
end

function MSFlowerLampTaskPanel:OnToggleClick(int_param)
    self.task_type = int_param
    self:FlushPanel()
end


MSFlowerLampTaskItem = MSFlowerLampTaskItem or DeclareMono("MSFlowerLampTaskItem",UIWidgetBaseItem)
function MSFlowerLampTaskItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    local flag,progress = ActFlowerLampData.Instance:GetFlowerLampTaskFlag(data)
    local color = COLORSTR.Red20
    if progress >= data.param_1 then
        color = COLORSTR.Green9
        progress = data.param_1
    end
    local progress_str = ColorStr(string.format("(%s/%s)",progress,data.param_1),color)
    UH.SetText(self.Desc,string.format("%s %s",data.task_desc,progress_str))
    self.GetBtnObj:SetActive(flag >= 0)
    self.GetedObj:SetActive(flag < 0)
    self.Interactor.Interactable = flag > 0
    self.RewardCell:SetData(Item.Create(data.reward_item))
    self.flag = flag
end

function MSFlowerLampTaskItem:GetClick()
    if self.flag == 1 then
        ActFlowerLampCtrl.Instance:FlowerReq(FlowerLampData.ReqType.GetReward, self.data.task_id)
    elseif self.flag == 0 then
        PublicPopupCtrl.Instance:Error(Language.FlowerLamp.GetTip)
    end
end


MSFlowerLampMgr = MSFlowerLampMgr or DeclareMono("MSFlowerLampMgr", FlowerLampMgr)
function MSFlowerLampMgr:MSFlowerLampMgr()
    self.data = ActFlowerLampData.Instance
end