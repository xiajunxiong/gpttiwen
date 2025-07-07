---- ==========鱼王的眼泪类似玩法基类 == ---------
FishTearBaseScene = FishTearBaseScene or BaseClass()
function FishTearBaseScene:Init(info)
	self.ane_type = nil
	self.view_type = self.view_type or FishRangeView
end

function FishTearBaseScene:OnSceneLogic(is_in_leave)
	if self.config == nil then
		self.config = AnecdoteLogic.Instance:GetSceneConfig(self.ane_type)
	end
	local info = AnecdoteData.Instance:GetTaskInfoByType(self.ane_type) 
	if info == nil then return end 
	FishAnswerData.Instance.sm_data.is_show_effect = not is_in_leave
	AnerQuestionData.Instance:SetCurTask(info.task_id)
	if self.config.scene_id == SceneData.Instance:SceneId() then
		if self.OnSelfLogic then
			self:OnSelfLogic(info,is_in_leave)
		end
		if FishAnswerData.CanSceneOperate(info.task_id) and not is_in_leave then
			ViewMgr:OpenView(self.view_type,{is_show_effect = not is_in_leave,task_id = info.task_id})
		else 
			if ViewMgr:IsOpen(self.view_type) then
				self:StopSceneEffect()
			end
			ViewMgr:CloseView(self.view_type)
		end 
	elseif self.config.scene_id ~= SceneData.Instance:SceneId() then
		self:StopSceneEffect()
		ViewMgr:CloseView(self.view_type)
	end
end 

function FishTearBaseScene:StopSceneEffect()
	if self.effect then
		SE:Stop(self.effect)
		self.effect = nil
	end
end

function FishTearBaseScene:OnTaskChange() 
end 

function FishTearBaseScene:OnTaskFinish()
	self:StopSceneEffect()
	ViewMgr:CloseView(self.view_type)
end

function FishTearBaseScene:OnLeaveScene()
	self:StopSceneEffect()
	if ViewMgr:IsOpen(self.view_type) then 
		ViewMgr:CloseView(self.view_type)
	end 
end

--===========================眼力训练===========================
VisionScene = VisionScene or BaseClass() 

function VisionScene:Init(info)
	self.index = 0
	self.info = info
	self.init_time = 3
	self.board_time = 20
	self.bubble = self:GetBubbleList()
end

function VisionScene:InitSceneLogic()
	self.board = nil
end

function VisionScene:OnSceneLogic(show,leave)
	if MainViewData.Instance:IsBattle() then 
		TimeHelper:CancelTimer(self.board_timer)
		return
	end
	if show and not leave then
		if self.board == nil then
			local npc_list = self.info.npc_list or {}
			self.board = AnecdoteLogic.GetBoardList(npc_list)
		end
        if self.index >= self.init_time then
            self.index = -self.board_time
        end
		self.index = self.index + 1
        if self.index > 0 then
			self:SetBoard(1)
            TimeHelper:CancelTimer(self.board_timer)
            self.board_timer = Invoke(function()
				if MainViewData.Instance:IsBattle() then 
					TimeHelper:CancelTimer(self.board_timer)
					return
				end 
				self:SetBoard(2)
            end,1)
        end
	else
		for k,v in pairs(self.board or {}) do
			v:SetBubble(false)
		end
    end
end

function VisionScene:GetBubbleList()
	local bubble = Split(self.info.bubble_id,"|")
	return {tonumber(bubble[1]),tonumber(bubble[2])}
end

function VisionScene:GetBubble(bubble_id,index)
	return NpcDialogData.Instance:GetBubbleData(bubble_id,index)
end

function VisionScene:SetBoard(index)
	if self.board and self.bubble ~= nil and index ~= nil and self.board[index] ~= nil then
		self.board[index]:SetBubble(true,self:GetBubble(self.bubble[index],self.index))
		local vo = self.board[index].obj_vo
		if vo ~= nil then
			SE:Play(SceneEffectConfig.EffectList.AngryEffect,self:GetEffectPos(vo))
		end
	end
end

function VisionScene:GetEffectPos(obj_vo)
	return Vector3.New(obj_vo.x + 0.5,51.5,obj_vo.y + 0.5)
end

function VisionScene:OnTaskFinish()
	TimeHelper:CancelTimer(self.board_timer)
end

--===========================司命星君===========================
StarLifeScene = StarLifeScene or BaseClass()
function StarLifeScene:Init(info)
	ClipGeter.GetClip("smxj_anim1")
end

function StarLifeScene:OnSceneLogic(beh_cfg)	
	SceneCtrl.Instance:CurScene():MainVirtualCameraAnimation("smxj_anim1")
	SmxjData.Instance:StartStory()
end

function StarLifeScene:OnBeginTask()
	TimeHelper:CancelTimer(self.time_handle)
end

function StarLifeScene:OnLeaveScene()
	SmxjData.Instance:OnLeaveScene()
end

--===========================鱼王眼泪===================================
FishTearScene = FishTearScene or BaseClass()
function FishTearScene:OnSceneLogic(is_in_leave)
	if self.config == nil then
		self.config = AnecdoteLogic.Instance:GetSceneConfig(AnecdoteType.FishTear)
	end
	local info = AnecdoteData.Instance:GetTaskInfoByType(AnecdoteType.FishTear)
	if info == nil then return end 
	FishAnswerData.Instance.sm_data.is_show_effect = not is_in_leave
	AnerQuestionData.Instance:SetCurTask(info.task_id)
	if self.config.scene_id == SceneData.Instance:SceneId() then
		if FishAnswerData.CanSceneOperate(info.task_id) and not is_in_leave then
			ViewMgr:OpenView(FishRangeView,{is_show_effect = not is_in_leave,task_id = info.task_id})
		else 
			ViewMgr:CloseView(FishRangeView)
		end
	elseif self.config.scene_id ~= SceneData.Instance:SceneId() then
		ViewMgr:CloseView(FishRangeView)
	end
end 

function FishTearScene:OnTaskChange() 
end 

function FishTearScene:OnTaskFinish()
	ViewMgr:CloseView(FishRangeView)
end

function FishTearScene:OnLeaveScene()
	if ViewMgr:IsOpen(FishRangeView) then 
		ViewMgr:CloseView(FishRangeView)
	end 
end

--===========================断是非===================================
DuanShiFeiScene = DuanShiFeiScene or BaseClass()
function DuanShiFeiScene:Init(info)
	self.info = info
	self.task_cfg = AnecdoteData.GetConfig(AnecdoteType.DuanShiFei)
	self.bubble_id_list = self:ConverList(self.info.bubble_id)
	self.npc_id_list = self:ConverList(self.info.npc_list)
	self.timer_list = nil
	self.show_num = 0
	self.is_leave = true
end

function DuanShiFeiScene:OnSceneLogic(show, leave)
	local need_reset = self.is_leave ~= leave
	self.is_leave = leave
	if self:IsShowBoard() and not leave then
		if self.timer_list == nil then
			self.timer_list = {}
			for i, v in pairs(self.npc_id_list) do
				self:CheckPP(i)
			end
		end
	elseif need_reset == true then
		for i, v in pairs(self.npc_id_list) do
			self:SetBoard(i, false)
		end
	end
end

function DuanShiFeiScene:ConverList(list_str)
	local list = {}
	for i, v in ipairs(Split(list_str, "|")) do
		table.insert(list, tonumber(v))
	end
	return list
end

function DuanShiFeiScene:SetBoard(index, is_show)
	if self.npc_id_list[index] == nil or self.bubble_id_list[index] == nil then
		return
	end
	local board = MainViewData.Instance:GetNpcBoard(self.npc_id_list[index])
	if board == nil then
		return
	end
	if is_show and not self.is_leave then
		local bubble_str = NpcDialogData.Instance:RandBubbleById(self.bubble_id_list[index], 1)
		board:SetBubble2(true, bubble_str)
	else
		board:SetBubble2(false, "")
	end
end

function DuanShiFeiScene:IsShowBoard()
	local task_info = AnecdoteData.Instance:GetTaskInfoByType(AnecdoteType.DuanShiFei)
	if task_info == nil or self.task_cfg == nil then
		return false
	end
	if task_info.task_id == self.task_cfg.special_task_id and task_info:State() == TaskConfig.TaskState.undo then
		return true
	end
	return false
end

function DuanShiFeiScene:CheckPP(index)
	self.timer_list[index] = {}
	local show_bubble = false
	local cur_show_bubble = false
	local time_pp = math.random(1, 6)
	local function func_check()
		show_bubble = not show_bubble
		if show_bubble and self.show_num == 2 then
			show_bubble = false
		end
		time_pp = show_bubble == true and 6 or math.random(-6, -9)
		if show_bubble ~= cur_show_bubble then
			self.show_num = self.show_num + (show_bubble == true and 1 or -1)
			self:SetBoard(index, show_bubble)
		end
		cur_show_bubble = show_bubble
	end
	self.timer_list[index] = TimeHelper:AddRunTimer(function()
		if 0 == time_pp then
			func_check()
		end
		time_pp = time_pp > 0 and time_pp - 1 or time_pp + 1
	end, 1)
end

function DuanShiFeiScene:OnTaskChange()
	if not self:IsShowBoard() then
		self:OnLeaveScene()
	end
end

--离开场景
function DuanShiFeiScene:OnLeaveScene()
	if self.timer_list ~= nil then
		for i, v in pairs(self.timer_list) do
			TimeHelper:CancelTimer(v)
		end
		self.timer_list = nil
		for i, v in pairs(self.npc_id_list) do
			self:SetBoard(i, false)
		end
	end
end

--- =========== 神秘宝藏 ========
PuzzleStorehouseScene = PuzzleStorehouseScene or BaseClass(FishTearBaseScene)
function PuzzleStorehouseScene:Init(info)
	FishTearBaseScene.Init(self)
	self.ane_type = AnecdoteType.PuzzleStorehouse
end

function PuzzleStorehouseScene:OnSelfLogic(task_info,is_in_leave)
	if self.effect == nil and FishAnswerData.Instance:InStorehouse(task_info.task_id) then
		local scene_cfg = AnecdoteLogic.Instance:GetSceneConfig(self.ane_type)
		local pos = Split(scene_cfg.trigger_pos,"|")
		self.effect = SE:Play(SceneEffectConfig.EffectList.Storehouse,Vector3.New(pos[1] + 1.5,38,pos[2] + 0.5))
	end
end