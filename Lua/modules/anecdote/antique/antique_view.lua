AntiqueView = AntiqueView or DeclareView("AntiqueView","anecdote/antique_view",Mod.Anecdote.Antique)

function AntiqueView:AntiqueView()
	self.config = Config.gdjs_story_auto.other
	self:InitModel()

	self.mouse_speed = 0.5 		   -- 模型旋转速度
	self.text_cur_rect_wigth = 0

	self.model_x = 0
	self.model_y = 0
	self.model_z = 0

	self.touch_x = 0               -- 触摸点的x
	self.tip_index = 0  		   -- 提示的下标
	self.model_index = 0           -- 模型数据下标

	if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end

    self.trigger_points = nil   	-- 触发点 ：transform
    self.is_tip_anim_playing = false -- 是否在播放记录
end

function AntiqueView:LoadCallback(param_t)
	 if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
    self.model_index = 0
    self.correct_count = 0
    self:FlushModel()
end

function AntiqueView:CloseCallback()
	if self.time_handle ~= nil then
		TimeHelper:CancelTimer(self.time_handle)
		self.time_handle = nil
	end
	if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
	self.ui_obj = nil
end

function AntiqueView:OnCloseClick()
	PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.AntiqueExitTip,function ()
        NpcDialogCtrl.Instance:NpcDialogView({talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.Antique)})
        ViewMgr:CloseView(AntiqueView)
    end)
end

-- 初始化模型配置
function AntiqueView:InitModel()
	self.model_data = {}
	local i = 1
	local len = #self.config
	while i <= 3 do
		math.randomseed(os.time() + i)
		local rand_num = math.random(len)
		local a = 0
		for _,v in pairs(self.model_data) do
			if v.model_id == self.config[rand_num].model_id then
				a = 1
				break
			end
		end
		if a == 0 then
			table.insert(self.model_data,self.config[rand_num])
			i = i + 1
		end
	end
end

function AntiqueView:SetTriggerPoints(index)
	index = index or self.model_index
	self.trigger_points = {}
	local data = self.model_data[index]
	for i = 1 ,3 do
		local trigger_data = {}
		trigger_data.trans = self.ui_obj:GetRoot().transform:Find(string.format("%s/%s",data.model_id,data["effect_" .. i]))
		if trigger_data.trans == nil then
			return
		end
		trigger_data.is_trigged = false
		trigger_data.effect = trigger_data.trans:GetComponent(typeof(Game.EffectAttacher))
		trigger_data.effect:SetAllVisible(false)
		trigger_data.data = data
		trigger_data.desc = tostring(data["effect_explain_" .. i])
		self.trigger_points[i] = trigger_data
	end
end

function AntiqueView:FlushModel()
	self.trigger_points = nil
	self.model_index = self.model_index + 1
	local data = self.model_data[self.model_index]
	self.ui_obj:SetData(DrawerHelper.GetNpcPath(data.model_id))
	self:ResetTip()
end

function AntiqueView:ResetTip()
	self.tip_index = 0
	for i = 1 ,self.TipAnimas:Length() do
		self.TipAnimas[i].gameObject:SetActive(false)
	end
	self.is_tip_anim_playing = false
end

-- 播放记录
function AntiqueView:PlayReport(data,index)
	TimeHelper:CancelTimer(self.time_handle)
	data = data or self.model_datap[self.model_index]
	index = index or self.tip_index
	data.is_trigged = true
	UH.SetText(self.TipTexts[index],data.desc)
	self.TipAnimas[index].gameObject:SetActive(true)
	self.is_tip_anim_playing = true
	data.effect:SetAllVisible(true)
	self.time_handle = Invoke(function ()
		data.effect:SetAllVisible(false)
		self.is_tip_anim_playing = false
	end,5)
end

function AntiqueView:GetTouchPos()
	local _, pos =
        UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
        self.ModelRect,
        UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil
    )
    return pos
end

function AntiqueView:MoveTouchFlag()
	local pos = self:GetTouchPos()
    UH.AnchoredPosition(self.TouchFlagRect,pos)
end

function AntiqueView:OnDragBegin()
	local pos = self:GetTouchPos()
	self.touch_x = pos.x
	self.TouchFlagRect.gameObject:SetActive(true)
end

function AntiqueView:OnDragEnd()
	self.TouchFlagRect.gameObject:SetActive(false)
end

function AntiqueView:OnDrag()
	if self.ui_obj == nil then
		return
	end
	self:MoveTouchFlag()
    self:Rot2()
    self.ModelShow.is_click = false
end

-- 方案一，沿局部坐标旋转
function AntiqueView:Rot1()
	local ui_obj = self.ui_obj:GetRoot()
	if ui_obj == nil then
		return
	end
	local mouse_pos_x = UnityEngine.Input.mousePosition.x
	local mouse_pos_y = UnityEngine.Input.mousePosition.y
	local dir = 1
    if self.mouse_pos_x and self.mouse_pos_x ~= mouse_pos_x then
        self.model_y = self.model_y + (self.mouse_pos_x - mouse_pos_x) * self.mouse_speed
    end
    local y_num = self.model_y % 360 
    if self.mouse_pos_y and self.mouse_pos_y ~= mouse_pos_y then
	    if (y_num >= 0 and y_num <= 45) or (y_num <= 360 and y_num > 315) then
	    	self.model_x = self.model_x + (self.mouse_pos_y - mouse_pos_y) * self.mouse_speed * 1
	    elseif y_num > 45 and y_num <= 135 then
	    	local dir = 1
	    	if self.touch_x > 0 then
	    		dir = -1
	    	end
	    	self.model_x = self.model_x + (self.mouse_pos_y - mouse_pos_y) * self.mouse_speed * dir
	    elseif y_num > 135 and y_num <= 225 then
	    	self.model_x = self.model_x + (self.mouse_pos_y - mouse_pos_y) * self.mouse_speed * -1
	    elseif y_num > 225 and y_num <= 315 then
	    	local dir = -1
	    	if self.touch_x > 0 then
	    		dir = 1
	    	end
	    	self.model_x = self.model_x + (self.mouse_pos_y - mouse_pos_y) * self.mouse_speed * dir
	    end
	end
	ui_obj.transform.localRotation = Quaternion.Euler(self.model_x,self.model_y,0)
	self.mouse_pos_x = mouse_pos_x
    self.mouse_pos_y = mouse_pos_y
end

-- 方案二，沿世界坐标旋转
function AntiqueView:Rot2()
	local ui_obj = self.ui_obj:GetRoot()
	if ui_obj == nil then
		return
	end
	if self.TouchTipObj.activeSelf == true then
		self.TouchTipObj:SetActive(false)
	end
	local mouse_pos_x = UnityEngine.Input.mousePosition.x
	local mouse_pos_y = UnityEngine.Input.mousePosition.y
	local rot_x_num = 0
    local rot_y_num = 0
    if self.mouse_pos_y and self.mouse_pos_y ~= mouse_pos_y then
    	rot_y_num = (self.mouse_pos_y - mouse_pos_y)
    end

    if self.mouse_pos_x and self.mouse_pos_x ~= mouse_pos_x then
    	rot_x_num = (self.mouse_pos_x - mouse_pos_x)
    end
    local rot_x = Quaternion.AngleAxis(rot_y_num, Vector3.right)
    local rot_y = Quaternion.AngleAxis(rot_x_num, Vector3.up)
    local rot = rot_x * rot_y
    ui_obj.transform.rotation = rot * ui_obj.transform.rotation
    self.mouse_pos_x = mouse_pos_x
    self.mouse_pos_y = mouse_pos_y

    self:CheckTriggerPoint()
end

function AntiqueView:CheckTriggerPoint()
	if self.trigger_points == nil then
		self:SetTriggerPoints()
	end
	if self.trigger_points == nil or next(self.trigger_points) == nil then
		return
	end
	if self.is_tip_anim_playing == true then
		return
	end
	local dot_value
	for i,v in pairs(self.trigger_points) do
		if v.is_trigged == false then
			dot_value = Vector3.Dot(self.ModelShow.model_camera_mono.Camera.transform.forward,v.trans.forward)
			if dot_value < -0.5 then
				self.tip_index = self.tip_index + 1
				self:PlayReport(v)
				return
			end
		end
	end
end

function AntiqueView:OnDown()
	self.mouse_pos_x = false
	self.mouse_pos_y = false
	self.ModelShow.is_click = true
end

function AntiqueView:OnTrueClick()
	if self.tip_index < 3 then
		PublicPopupCtrl.Instance:Center(Language.Anecdote.AntiqueTip)
	else
		self:Handle(1)
	end
end

function AntiqueView:OnFalseClick()
	if self.tip_index < 3 then
		PublicPopupCtrl.Instance:Center(Language.Anecdote.AntiqueTip)
	else
		self:Handle(0)
	end
end

function AntiqueView:Handle(flag)
	self.ResultObj:SetActive(true)
	local data = self.model_data[self.model_index]
	if flag == data.model_type then
		UH.SpriteName(self.ResultImg,"_LocJianDingChengGong")
		self.correct_count = self.correct_count + 1
	else
		UH.SpriteName(self.ResultImg,"_LocJianDingShiBai")
	end
end

function AntiqueView:ResultClick()
	self.ResultObj:SetActive(false)
	if self.model_index >= 3 then
		if self.correct_count >= 3 then
			AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.Antique)
		else
			NpcDialogCtrl.Instance:NpcDialogView({talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.Antique)})
		end
		ViewMgr:CloseView(AntiqueView)
	else
		self:FlushModel()
	end
end