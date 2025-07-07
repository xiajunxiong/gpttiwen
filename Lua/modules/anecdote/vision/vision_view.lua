------------ 眼力训练相关的背景 ------------
VisionBoard = VisionBoard or DeclareMono("VisionBoard",AnecdoteBaseBoard)
function VisionBoard:SetData(view_type,anecdote_type,close_func)
	AnecdoteBaseBoard.SetData(self,view_type,anecdote_type,close_func)
	if tonumber(self.anecdote_type) then
		UH.SetText(self.Name,AnecdoteData.GetName(self.anecdote_type))
	else
		UH.SetText(self.Name,self.anecdote_type)
	end
end

----------- 眼力训练最后的评分 -------------
VisionScore = VisionScore or DeclareMono("VisionScore")
function VisionScore:VisionScore()
	self.gameObject:SetActive(false)
	self.close_func = nil
	self.score_cfg = Config.ylxl_story_auto.grade
	self.result_img_list = {"_LocFaYanTongTian","_LocHuoYanJingjing","_LocHuiYanRuKuang"}
	self.time = 0
	self.result_name = nil
end

function VisionScore:SetData(data)
	if data == nil or next(data) == nil then
		return
	end
	if data.time then
		self.time = data.time
	end
	if data.result_name then
		self:SetResultImg(data.result_name)
	end
	if data.close_func then
		self:SetCloseFunc(data.close_func)
	end
end

function VisionScore:SetTime(time)
	self.time = time
end

function VisionScore:SetResultImg(img_name)
	self.result_name = img_name
	UH.SpriteName(self.ResultImg,img_name)
end

function VisionScore:SetResult(index)
	UH.SpriteName(self.ResultImg,self.result_img_list[index])
end

function VisionScore:SetCloseFunc(func)
	self.close_func = func
end

function VisionScore:SetScoreCfg(cfg)
	self.score_cfg = cfg
end

function VisionScore:HandleTime(time)
	time = time or self.time
	if time == nil then
		return
	end
	for i,v in pairs(self.score_cfg) do
		local min_num = v.gradetime_min
		local max_num = v.gradetime_max <= 0 and math.huge or v.gradetime_max
		if time >= min_num and time <= max_num then
			self:SetResult(i)
			return i
		end
	end
end

function VisionScore:Play(time)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
	self.gameObject:SetActive(true)
	if self.result_name == nil then
		self:HandleTime(time)
	end
end

function VisionScore:OnCloseClick()
	if self.close_func then
		self.close_func()
	end
end

-- 目前写死图中找三处不同，以后有需要更多不同在扩展吧
VisionPanel = VisionPanel or DeclareMono("VisionPanel",UIWFlushPanel)
function VisionPanel:VisionPanel()
	if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
    self.config = Config.ylxl_story_auto
	self.anecdote_type = nil
	self.accomplish_func = nil


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

function VisionPanel:SetData(data)
	self.data = data
	self.anecdote_type = self.anecdote_type
	self.accomplish_func = data.acc_func

	self.Score:SetCloseFunc(data.acc_func)
	if data.result_name then
		self.Score:SetResultImg(data.result_name)
	end
end

function VisionPanel:GetData()
	return self.data
end

function VisionPanel:Awake()
	UIWFlushPanel.Awake(self)
end

function VisionPanel:OnDestroy()
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

function VisionPanel:OnTriggerClick(index)
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

function VisionPanel:HandleClickEvent(index)
	self.IsStarMove = true
	self.select_data[index] = true
	self.FlagObjs[index]:SetActive(true)
	local star_obj = self.StarRects[index].gameObject
	local end_index = self:GetEndIndex()
	local end_star_obj = self.StarTargetObjs[end_index]
	star_obj:SetActive(true)
	local start_pos = star_obj:GetLocalPosition()
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

function VisionPanel:StarMove(index)
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

function VisionPanel:GetEndIndex()
	local index = 0
	for i,v in pairs(self.select_data) do
		if (v == true) then
			index = index + 1
		end
	end
	return index
end

function VisionPanel:OnClickError()
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

function VisionPanel:AccomplishEvent()
	local time = self.start_time - os.time()
	self.Score:Play(time)
end

--------------------- 眼力训练 --------------
VisionView = VisionView or DeclareView("VisionView","anecdote/vision/vision_view",Mod.Anecdote.Vision)
function VisionView:VisionView()
	self.anecdote_type = AnecdoteType.Vision
	self.Board:SetData(self:GetType(),self.anecdote_type,self.OnCloseFunc)
	self.Panel:SetData({
		anecdote_type = self.anecdote_type,
		acc_func = BindTool.Bind(self.OnAccomplishFunc,self),
	})
end

function VisionView:OnAccomplishFunc()
	AnecdoteCtrl.Instance:SendTaskAction(self.anecdote_type)
    ViewMgr:CloseView(VisionView)
end

function VisionView:OnCloseFunc()
	PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.VisionExitTip,function ()
        ViewMgr:CloseView(VisionView)
    end)
end

--------------- 真假画作 ------------
VisionDiscernView = VisionDiscernView or DeclareView("VisionDiscernView","anecdote/vision/vision_discern_view",Mod.Anecdote.VisionDiscern)
function VisionDiscernView:VisionDiscernView()
	self.anecdote_type = AnecdoteType.VisionDiscern
	self.Board:SetData(self:GetType(),self.anecdote_type,BindTool.Bind(self.OnCloseFunc,self))
	self.Panel:SetData({
		anecdote_type = self.anecdote_type,
		acc_func = BindTool.Bind(self.OnAccomplishFunc,self),
		result_name = "_LocZhenZhiZhuoJian",
	})
end

function VisionDiscernView:OnAccomplishFunc()
	AnecdoteCtrl.Instance:SendTaskAction(self.anecdote_type)
    ViewMgr:CloseView(VisionDiscernView)
end

function VisionDiscernView:OnCloseFunc()
	PublicPopupCtrl.Instance:AlertTip(Language.Anecdote.VisionDiscernExitTip,function ()
		ViewMgr:CloseView(DialogTipsView)
		local talk_id = AnecdoteData:GetFailTalkId(self.anecdote_type)
		NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id})
        ViewMgr:CloseView(VisionDiscernView)
    end)
end


--------------------- 沙漠怪龙 --------------
VisionDesertDragonView = VisionDesertDragonView or DeclareView("VisionDesertDragonView","anecdote/vision/vision_desert_dragon_view",Mod.Anecdote.DesertDragon)
function VisionDesertDragonView:VisionDesertDragonView()
	self.anecdote_type = AnecdoteType.DesertDragon
	self.Board:SetData(self:GetType(),self.anecdote_type,BindTool.Bind(self.OnCloseFunc,self))
	self.Panel:SetData({
		anecdote_type = self.anecdote_type,
		acc_func = BindTool.Bind(self.OnAccomplishFunc,self),
	})
	self.config = Config.desert_Loong_auto.other[1]
end

function VisionDesertDragonView:OnAccomplishFunc()
	AnecdoteCtrl.Instance:SendTaskAction(self.anecdote_type)
    ViewMgr:CloseView(VisionDesertDragonView)
end

function VisionDesertDragonView:OnCloseFunc()
	local npc_cfg = Cfg.NpcById(self.config.npc_id) or {}
	local npc_name = npc_cfg.name or ""
	local view_name = AnecdoteData.GetName(self.anecdote_type) or ""
	local tip = string.format(Language.Anecdote.VisionExitComTip,npc_name,view_name)
	PublicPopupCtrl.Instance:AlertTip(tip,function ()
		NpcDialogCtrl.Instance:NpcDialogView({talk_id = self.config.fail_talk})
        ViewMgr:CloseView(VisionDesertDragonView)
    end)
end

--------------------- 仙子去哪了-真伪仙画 --------------
VisionFairyView = VisionFairyView or DeclareView("VisionFairyView","anecdote/vision/vision_fairy_view")
function VisionFairyView:VisionFairyView()
    local type = FairyWhereData.Instance:GetTurnType()
	self.Board:SetData(self:GetType(),Language.FairyWhere.VisionName,self.OnCloseFunc)
	self.Panel:SetData({acc_func = BindTool.Bind(self.OnAccomplishFunc,self),result_name = "_LocMingChaQiuHao"})
	self:FlushVisionFairyView(type)
end

function VisionFairyView:FlushVisionFairyView(type)
	local img_name = tostring(type) .. [[_]]
	local img_list = self.Panel.TriggerImgs
	local item_list = self["ItemList" .. type]
	for i=1,img_list:Length() do
		UH.SpriteName(img_list[i],img_name .. i)
		local pos = item_list[i].localPosition
		UH.LocalPosG(img_list[i].gameObject, pos)
		UH.LocalPos(self.Panel.StarRects[i], pos)
	end
	for i=1,self.Texture:Length() do
		self.Texture[i]:SetByPath(Format(PeriodActivityConfig.VisionFairyPath,type))
	end
end

function VisionFairyView:OnAccomplishFunc()
    ViewMgr:CloseView(VisionFairyView)
	FairyWhereCtrl.Instance:OnFairyWhereFinish(3)
end

function VisionFairyView:OnCloseFunc()
	PublicPopupCtrl.Instance:AlertTip(Language.FairyWhere.VisionFairyTip,function ()
        ViewMgr:CloseView(VisionFairyView)
		FairyWhereCtrl.Instance:OnFairyWhereFail(3)
    end)
end