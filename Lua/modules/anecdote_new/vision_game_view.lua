-- 眼力训练玩法
VisionGameView = VisionGameView or DeclareView("VisionGameView", "anecdote_new/vision_game_view", Mod.AnecdoteNew.Vision)
function VisionGameView:VisionGameView()
    self.type = NewAnecdoteType.YLXL
end

function VisionGameView:LoadCallback()
    local param = AnecdoteData.Instance:GetNewParam(self.type, true)
    if TableIsEmpty(param) then
		LogError("眼力训练玩法无参数,自动关闭界面")
		ViewMgr:CloseView(VisionGameView)
        return
    end
    local seq = param.seq
    self.seq = seq
    local cfg = self:GetCfg(seq)
	if not cfg then
		LogError("眼力训练玩法无配置,自动关闭界面")
		ViewMgr:CloseView(VisionGameView)
        return
    end
    local title = cfg.game_name0
    self.Board:SetData(self:GetType(),title)
    local panel_data = {
        img = cfg.photo_res,
        left_name = cfg.real_name,
        right_name = cfg.fake_name,
        acc_func = BindTool.Bind(self.OnSuccFunc, self)
    }
    self.Panel:SetData(panel_data)
end

function VisionGameView:GetCfg(seq)
    for i,v in pairs(Config.anecdotes_config_auto.eyes_train) do
        if v.seq == seq then
            return v
        end
    end
end

-- 游戏成功回调
function VisionGameView:OnSuccFunc()
    AnecdoteData.Instance:NewAnecdoteSucc(self.seq)
    ViewMgr:CloseView(VisionGameView)
end

function VisionGameView:OnCloseClick()
    ViewMgr:CloseView(VisionGameView)
end


-- 目前写死图中找三处不同，以后有需要更多不同在扩展吧
VisionGamePanel = VisionGamePanel or DeclareMono("VisionGamePanel",UIWFlushPanel)

-- 客户端根据立绘自定义的配置
-- pos_list ==== 3个找茬点的位置，最多3个
VisionGamePanel.Cfg = {
    ["TuPian1a"] = {
        pos_list = {{-4.4,42.2},{117,-84.6},{-74.2,175.8}}
    },
    ["TuPian2a"] = {
        pos_list = {{21.8,-161},{-106.3,-17.2},{124.4,175.6}}
    },
    ["TuPian3a"] = {
        pos_list = {{31.2,151},{-1.8,-18.1},{117.1,-251.8}}
    },
    ["TuPian4a"] = {
        pos_list = {{-42.6,140.2},{18,18.3},{20.8,-190.6}}
    },
    ["TuPian5a"] = {
        pos_list = {{-73.4,-113.3},{-104.6,29},{90.8,-232.6}}
    },
    ["TuPian6a"] = {
        pos_list = {{-128.9,130.3},{-94.1,34.6},{16.2,-84.8}}
    },
    ["TuPian7a"] = {
        pos_list = {{9.3,155.9},{-19.9,12.2},{45.8,-229.8}}
    },
    ["TuPian8a"] = {
        pos_list = {{-63.3,187.7},{-86.1,-104.9},{70.1,-200.1}}
    }
}
function VisionGamePanel:VisionGamePanel()
	if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end

	self.default_step = 20

	self.trigger_count = self.TriggerImgs:Length()		-- 触发点个数
	self.select_data = {}	
	self.step_counts = {}
	for i = 1, self.trigger_count do
		self.select_data[i] = false
		self.step_counts[i] = 0
	end

	self.start_time = os.time()
	-- 考虑到玩家快速点击三个,每个星星单独分出来
	self.time_handles = {}
	self.satr_curves = {}
end

-- data = {

-- acc_func ==== 完成的回调
-- img ==== 左图，右图是一样的资源
-- left_name === 左图名字
-- right_name == 右图名字
-- }
function VisionGamePanel:SetData(data)
	self.data = data
	self.Score:SetCloseFunc(data.acc_func)

    local img_path = string.format("anecdote_new/vision_game/%s.png",data.img)
    self.Img1:SetByPath(img_path)
    self.Img2:SetByPath(img_path)
    UH.SetText(self.ImgName1, data.left_name or "")
    UH.SetText(self.ImgName2, data.right_name or "")
    UH.SetText(self.Tip, string.format("找出%s与%s不同的地方，点击右图相应的位置即可。", data.right_name, data.left_name))
    local pos_list = VisionGamePanel.Cfg[data.img].pos_list
    for i = 1, self.TriggerImgs:Length() do
        UH.SpriteName(self.TriggerImgs[i], data.img .. "_" .. i)
        local rect = self.TriggerImgs[i].gameObject:GetComponent(typeof(UnityEngine.RectTransform))
        local pos = Vector2.New(pos_list[i][1], pos_list[i][2])
        UH.AnchoredPosition(rect, pos)
    end
    TimeHelper.CancelTimer(self.img_time_ht)
    self.img_time_ht = Invoke(function ()
        self.Img1:SetObjActive(true)
        self.Img2:SetObjActive(true)
    end,0.2)
end

function VisionGamePanel:GetData()
	return self.data
end

function VisionGamePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Img1:SetObjActive(false)
    self.Img2:SetObjActive(false)
end

function VisionGamePanel:OnDestroy()
    TimeHelper.CancelTimer(self.img_time_ht)
	for i,v in pairs(self.time_handles) do
		TimeHelper:CancelTimer(v)
	end
	for i,v in pairs(self.satr_curves) do
		v = nil
	end
	if self.time_handle ~= nil then
		TimeHelper:CancelTimer(self.time_handle)
		self.time_handle = nil
	end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
	UIWFlushPanel.OnDestroy(self)
end

function VisionGamePanel:OnTriggerClick(index)
	if self.IsStarMove then
		return
	end
	if self.select_data[index] == true then
		return
	else
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
		self:HandleClickEvent(index)
	end
end

function VisionGamePanel:HandleClickEvent(index)
	self.IsStarMove = true
	self.select_data[index] = true
	self.FlagObjs[index]:SetActive(true)
	local star_obj = self.StarRects[index].gameObject
	local end_index = self:GetEndIndex()
	local end_star_obj = self.StarTargetObjs[end_index]
	star_obj.gameObject:SetActive(true)
	local start_pos = UH.GetRootPosition(self.TriggerImgs[index].gameObject)
	local end_pos = end_star_obj:GetLocalPosition()
	self.move_pos_list = {}
	table.insert(self.move_pos_list, start_pos)
	local new_x = (end_pos.x - start_pos.x) / 2 + 200
	local new_y = (end_pos.y - start_pos.y) / 2 + 100
	table.insert(self.move_pos_list, Vector3.New(new_x, new_y, 0))
	table.insert(self.move_pos_list, end_pos)
	self.cur_step = 0
	local time_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.StarMove,self,index),1,self.default_step)
	self.time_handles[index] = time_handle
end

function VisionGamePanel:StarMove(index)
	self.step_counts[index] = self.step_counts[index] + 1
	local pos = FormatCurve(self.move_pos_list, self.step_counts[index] / self.default_step)
	local scale_num = (self.step_counts[index] * self.step_counts[index]) * 0.05
	scale_num = scale_num >= 1 and 1 or scale_num
	local v3 = Vector3.New(scale_num,scale_num,scale_num)
	self.StarRects[index].gameObject:SetLocalPosition(pos)
	UH.LocalScale(self.StarRects[index],v3)
	self.cur_step = self.cur_step + 1
	if self.cur_step == self.default_step then
		local end_index = self:GetEndIndex()
		if end_index == self.trigger_count then
			self:AccomplishEvent()
		else
			AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)
			self.IsStarMove = nil
		end
	end
end

function VisionGamePanel:GetEndIndex()
	local index = 0
	for i,v in pairs(self.select_data) do
		if (v == true) then
			index = index + 1
		end
	end
	return index
end

function VisionGamePanel:OnClickError()
	if self.IsStarMove then
		return
	end
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
	self.ErrorRect.gameObject:SetActive(true)
	PublicPopupCtrl.Instance:Center(Language.Anecdote.VisionErrorTip)
	local _, pos =
        UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
        self.CtrlContentRect,
        UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil
    )
    UH.AnchoredPosition(self.ErrorRect,pos)
    if self.time_handle ~= nil then
		TimeHelper:CancelTimer(self.time_handle)
		self.time_handle = nil
	end
    self.time_handle = TimeHelper:AddDelayTimer(function ()
    	self.ErrorRect.gameObject:SetActive(false)
    end,2)
end

function VisionGamePanel:AccomplishEvent()
	local time = self.start_time - os.time()
	self.Score:Play(time)
end