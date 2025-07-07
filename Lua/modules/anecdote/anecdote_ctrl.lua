FILE_REQ("modules/anecdote/anecdote_data")
FILE_REQ("modules/anecdote/anecdote_info")
FILE_REQ("modules/anecdote/anecdote_scene")
FILE_REQ("modules/anecdote/anecdote_logic")
FILE_REQ("modules/anecdote/anecdote_config")

FILE_REQ("modules/anecdote/sudoku/sudoku_data")
FILE_REQ("modules/anecdote/puzzle/puzzle_data")

VIEW_REQ("modules/anecdote/sudoku/sudoku_view")
VIEW_REQ("modules/anecdote/puzzle/puzzle_view")
VIEW_REQ("modules/anecdote/vision/vision_view")

VIEW_REQ("modules/anecdote/couplet/couplet_view")
VIEW_REQ("modules/anecdote/couplet/couplet_data")
VIEW_REQ("modules/anecdote/couplet/couplet2_view")
VIEW_REQ("modules/anecdote/couplet/couplet3_view")

FILE_REQ("modules/anecdote/smxj/smxj_data")
VIEW_REQ("modules/anecdote/smxj/smxj_view")
VIEW_REQ("modules/anecdote/smxj/smxj_scene_view")
VIEW_REQ("modules/anecdote/antique/antique_view")
VIEW_REQ("modules/anecdote/poetry/poetry_view")
VIEW_REQ("modules/anecdote/poetry/poetry_data")

VIEW_REQ("modules/anecdote/klotski/klotski_view")
VIEW_REQ("modules/anecdote/klotski/klotski_data")

VIEW_REQ("modules/anecdote/decrypt/decrypt_view")

VIEW_REQ("modules/anecdote/pack_poetry/pack_poetry_view")
VIEW_REQ("modules/anecdote/pack_poetry/pack_poetry2_view")

VIEW_REQ("modules/anecdote/fishtear/ane_question_data")
VIEW_REQ("modules/anecdote/fishtear/children_answer_data")
VIEW_REQ("modules/anecdote/fishtear/wedding_answer_data")

FILE_REQ("modules/anecdote/fishtear/fish_answer_data")
VIEW_REQ("modules/anecdote/fishtear/fish_range_view")
VIEW_REQ("modules/anecdote/fishtear/fish_answer_view")
VIEW_REQ("modules/anecdote/fishtear/wedding_answer_view")

FILE_REQ("modules/anecdote/foxcatch/fox_catch_data")
VIEW_REQ("modules/anecdote/foxcatch/fox_catch_menu_view")
VIEW_REQ("modules/anecdote/foxcatch/fox_compass_view")

FILE_REQ("modules/anecdote/memory_master/memory_master_data")
VIEW_REQ("modules/anecdote/memory_master/memory_master_view")

FILE_REQ("modules/anecdote/fish_messenger/fish_messenger_data")
VIEW_REQ("modules/anecdote/fish_messenger/fish_messenger_view")

FILE_REQ("modules/anecdote/immortal_recruit_student/immortal_recruit_student_data")
VIEW_REQ("modules/anecdote/immortal_recruit_student/immortal_recruit_student_view")

FILE_REQ("modules/anecdote/array_ghost/array_ghost_config")
FILE_REQ("modules/anecdote/array_ghost/array_ghost_data")
FILE_REQ("modules/anecdote/array_ghost/array_ghost_info")
VIEW_REQ("modules/anecdote/array_ghost/array_ghost_director")
VIEW_REQ("modules/anecdote/array_ghost/array_ghost_guide_view")

FILE_REQ("modules/anecdote/anecdote_hint/anecdote_hint_data")
VIEW_REQ("modules/anecdote/anecdote_hint/anecdote_hint_view")

FILE_REQ("modules/anecdote/music_anecdote/music_anecdote_config")
FILE_REQ("modules/anecdote/music_anecdote/music_anecdote_data")
VIEW_REQ("modules/anecdote/music_anecdote/music_anecdote_info")
VIEW_REQ("modules/anecdote/music_anecdote/music_anecdote_view")

FILE_REQ("modules/anecdote/fun_riddle/fun_riddle_data")
VIEW_REQ("modules/anecdote/fun_riddle/fun_riddle_view")

FILE_REQ("modules/anecdote/poetry2/poetry2_data")
VIEW_REQ("modules/anecdote/poetry2/poetry2_view")

VIEW_REQ("modules/anecdote/decrypt2/decrypt2_view")
VIEW_REQ("modules/anecdote/decrypt3/decrypt3_view")

VIEW_REQ("modules/anecdote_new/vision_game_view")
VIEW_REQ("modules/anecdote_new/puzzle_game_view")
VIEW_REQ("modules/anecdote_new/poetry_game_view")
VIEW_REQ("modules/anecdote_new/couplet_game_view")
VIEW_REQ("modules/anecdote_new/decrypt_game_view")

AnecdoteCtrl = AnecdoteCtrl or BaseClass(BaseCtrl)

function AnecdoteCtrl:__init()
	if AnecdoteCtrl.Instance ~= nil then
		Debuger.LogError("[AnecdoteCtrl] attempt to create singleton twice!")
		return
	end	
    AnecdoteCtrl.Instance = self
	self.data = AnecdoteData.New()
	self.logic = AnecdoteLogic.New()
	self.sudoku_data = SudokuData.New()
	self.couplet_data = CoupletData.New()
	self.question_data = AnerQuestionData.New()
	self.fox_data = CatchFoxData.New()
	self.poetry_data = PoetryData.New()
	self.klotski_data = KlotskiData.New()
	self.smxj_data = SmxjData.New()
	self.puzzle_data = PuzzleData.New()
	self.memory_master_data = MemoryMasterData.New()
	self.fish_messenger_data = FishMessengerData.New()
	self.immortal_recruit_student_data = ImmortalRecruitStudentData.New()
	self.array_ghost_data = ArrayGhostData.New()
	self.hint_data = AnecdoteHintData.New()
	self.music_data = MusicAnecdoteData.New()
	self.funriddle_data = FunRiddleData.New()
	self.poetry2_data = Poetry2Data.New()
	self:RegisterAllProtocols()
end

function AnecdoteCtrl:OnInit()
	ActivityData.Instance:CustomClickHandle(ActType.Hint,function()
        ViewMgr:OpenViewByKey(Mod.Anecdote.Hint)
    end)
end

function AnecdoteCtrl:__delete()
	self:UnRegisterAllProtocols()
	AnecdoteCtrl.Instance = nil
	Delete(self,"data")
end

function AnecdoteCtrl:OnUnloadGameLogic()
	self.data:ClearData()
	self.question_data:ClearData()
	self.sudoku_data:Clear()
	self.hint_data:Clear()
end

function AnecdoteCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSStrangerTaskOperate)
	self:RegisterProtocol(CSNewTaskAction)
	self:RegisterProtocol(CSAnecdoteTaskSpecialReq)
	self:RegisterProtocol(CSCatchFoxSpecialLogicReq)
	self:RegisterProtocol(CSNewGiveNPCTaskNeedItems)
	self:RegisterProtocol(CSAnecdoteTaskFinishReq)
	self:RegisterProtocol(CSSpecialAnecdoteReq)
	self:RegisterProtocol(SCStrangerTaskInfo,"OnStrangerTaskInfo")
	self:RegisterProtocol(SCStrangerTaskAllInfo,"OnStrangerTaskAllInfo")
	self:RegisterProtocol(SCStrangerTaskBattleFail,"OnStrangerTaskBattleFail")
	self:RegisterProtocol(SCStrangerTaskCompleteNotice,"OnStrangerTaskCompleteNotice")
	self:RegisterProtocol(SCAnecdoteTaskSpecialInfo,"OnAnecdoteTaskSpecialInfo")
	self:RegisterProtocol(SCAnecdoteBigEventInfo,"OnSCAnecdoteBigEventInfo")
	self:RegisterProtocol(SCSpecialAnecdoteList,"SCSpecialAnecdoteList")
	self:RegisterProtocol(SCSpecialAnecdoteSingleList,"SCSpecialAnecdoteSingleList")
end

function AnecdoteCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCAnecdoteBigEventInfo)
	self:UnRegisterProtocol(CSStrangerTaskOperate)
	self:UnRegisterProtocol(CSNewTaskAction)
	self:UnRegisterProtocol(CSCatchFoxSpecialLogicReq)
	self:UnRegisterProtocol(CSAnecdoteTaskSpecialReq)
	self:UnRegisterProtocol(CSNewGiveNPCTaskNeedItems)
	self:UnRegisterProtocol(CSAnecdoteTaskFinishReq)
	self:UnRegisterProtocol(SCStrangerTaskInfo)
	self:UnRegisterProtocol(SCStrangerTaskAllInfo)
	self:UnRegisterProtocol(SCStrangerTaskBattleFail)
	self:UnRegisterProtocol(SCStrangerTaskCompleteNotice)
	self:UnRegisterProtocol(SCAnecdoteTaskSpecialInfo)
end

--奇闻任务请求 
--0, 接受任务  p1 任务ID
--1, 提交任务  p1 任务ID
--2, 领取任务奖励 p1 奇闻任务类型
function AnecdoteCtrl:SendTaskReq(req_type,param1)
	local protocol = ProtocolPool.Instance:GetProtocol(CSStrangerTaskOperate)
	protocol.req_type = req_type or 0
	protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

--新的任务行为请求action_type
--新任务行为 提交任务进度使用 progress 变成 1
--0 与NPC对话 param1 npc_seq
--1 提交物品 param1:item_id  param2:num  param3:task_id
--2 奇闻挑战成功 param1:奇闻任务类型
function AnecdoteCtrl:SendTaskActionReq(param_t)
	local protocol = ProtocolPool.Instance:GetProtocol(CSNewTaskAction)
	protocol.action_type = param_t.action_type or 0
	protocol.param1 = param_t.param1 or 0
	protocol.param2 = param_t.param2 or 0
	protocol.param3 = param_t.param3 or 0
	protocol.param4 = param_t.param4 or 0
	SendProtocol(protocol)
end

function AnecdoteCtrl:SendCatchFoxSpecialReq(param_t)
	local protocol = ProtocolPool.Instance:GetProtocol(CSCatchFoxSpecialLogicReq)
	protocol.x = param_t.x or 0
	protocol.y = param_t.y or 0
	protocol.dir = param_t.dir or 0
	SendProtocol(protocol)
end

-- 奇闻任务其他特殊设置 anecdote_type:奇闻类型 anecdote_value:需要设置这个参数为多少
function AnecdoteCtrl:SendTaskSpecialReq(anecdote_type, anecdote_value)
	local protocol = ProtocolPool.Instance:GetProtocol(CSAnecdoteTaskSpecialReq)
	protocol.anecdote_type = anecdote_type or 0
	protocol.anecdote_value = anecdote_value or 0
	SendProtocol(protocol)
end

function AnecdoteCtrl:SendAnecdoteTaskFinishReq(anecdote_type,task_id)
    local protocol = ProtocolPool.Instance:GetProtocol(CSAnecdoteTaskFinishReq)
    protocol.task_id = task_id or 0
    protocol.anecdote_type = anecdote_type or 0
    SendProtocol(protocol)
end

function AnecdoteCtrl:SendGiveNPCTaskNeedItems(task_id,commit_list)
    local protocol = ProtocolPool.Instance:GetProtocol(CSNewGiveNPCTaskNeedItems)
    protocol.task_id = task_id or 0
    protocol.commit_list = commit_list or {}
    SendProtocol(protocol)
end

--奇闻任务其他特殊下发，登录下发
function AnecdoteCtrl:OnAnecdoteTaskSpecialInfo(protocol)
	self.data:SetAnecdoteTaskSpecialInfo(protocol)
end

--接受任务
function AnecdoteCtrl:SendTaskAccept(task_id)
	self:SendTaskReq(0,task_id)
end

--提交任务
function AnecdoteCtrl:SendTaskCommit(task_id)
	self:SendTaskReq(1,task_id)
end

--提交奇闻挑战成功请求
function AnecdoteCtrl:SendTaskAction(type)
	local task_info = self.data:GetTaskInfoByType(type)
	if task_info ~= nil then 
		task_info:RequestAction() 
	end
end

function AnecdoteCtrl:RemoveTask(type)
	local task_info = self.data:GetTaskInfoByType(type)
	if task_info ~= nil then 
		task_info:RemoveTask() 
	end
end

--奇闻任务战斗失败
function AnecdoteCtrl:OnStrangerTaskBattleFail(protocol)
	local config = AnecdoteData.GetConfig(protocol.stranger_task_type)
	if config and config.fail_talk ~= 0 then
		if config.loading_talk and config.loading_talk ~= 0 then
			CutChapterCtrl.Instance:OpenCutChapter(config.loading_talk,function()
				NpcDialogCtrl.Instance:NpcDialogView({talk_id = config.fail_talk})
			end)
		else
			NpcDialogCtrl.Instance:NpcDialogView({talk_id = config.fail_talk})
		end
	end
end

--奇闻任务完成通知
function AnecdoteCtrl:OnStrangerTaskCompleteNotice(protocol)
	TaskData.Instance:TaskInfoUpdateByTaskId(protocol.task_id)
end

--全部奇闻任务信息下发
function AnecdoteCtrl:OnStrangerTaskAllInfo(protocol)
	for k,v in pairs(protocol.task_info_list) do
		if v.task_id ~= 0 then
			self:CustomTask(v)
		end
	end
	AnecdoteLogic.Instance:InitSceneLogicData()
end

--单个奇闻任务信息下发
function AnecdoteCtrl:OnStrangerTaskInfo(protocol)
	self:RequestEndTask(protocol.task_info)
	self:CustomTask(protocol.task_info)
end

--自动领取奇闻任务奖励
function AnecdoteCtrl:RequestEndTask(info)
	local task_info = self.data:GetTaskInfoByType(info.stranger_task_type)
	if task_info ~= nil and info.has_finish == 1 then
		self:SendTaskReq(2,info.stranger_task_type)
		task_info:RemoveTask()
	end
end

--添加到任务
function AnecdoteCtrl:CustomTask(task_info)
	if task_info.task_id == 0 then return end
	if task_info.has_finish == 0 then
		local task = self.data:TaskInfo(task_info.task_id)
		task:TaskVo(self.data:CustomTask(task_info))
		TaskData.Instance:TaskInfoUpdateByTaskId(task_info.task_id,task)
		self.data:SetTaskCacheList(task_info.stranger_task_type,task_info.task_id)
	else
		TaskData.Instance:TaskInfoUpdateByTaskId(task_info.task_id)
		self.data:SetTaskCacheList(task_info.stranger_task_type)
	end
end

--是否有奇闻对话任务NPC
function AnecdoteCtrl.IsHasTaskInfo(param_t,obj_vo)
	local task_info = AnecdoteData.Instance:GetAnecdote(obj_vo)
	if task_info and task_info:IsTaskNpc(obj_vo)then 
		task_info:NpcDialog(param_t,obj_vo)
		return true
	elseif WenXiangLouData.Instance:IsWenXiangLouNpc(param_t,obj_vo) then
		return true
	elseif SmxjData.Instance:IsSelfNpc(param_t, obj_vo) then
		return true
	end
end

--奇闻特殊值处理
function AnecdoteCtrl:SendSpecialData(anecdote_type,value)
	if value ~= self.data:GetSpecialInfo(anecdote_type) then
		self:SendTaskSpecialReq(anecdote_type,value)
	end
	self.data:SetSpecialData(anecdote_type,value)
end

function AnecdoteCtrl:StrangerTaskFailTalk(anecdote_type)
	local config = AnecdoteData.GetConfig(anecdote_type)
	if config and config.fail_talk ~= 0 then
		NpcDialogCtrl.Instance:NpcDialogView({talk_id = config.fail_talk})
	end
end

function AnecdoteCtrl:OnSCAnecdoteBigEventInfo(protc)
	self.hint_data:OnSCAnecdoteBigEventInfo(protc)
end

--请求领取大事件奖励
function AnecdoteCtrl:GetHintReward(reward_seq)
	self:SendTaskReq(3,reward_seq)
end

function AnecdoteCtrl:SCSpecialAnecdoteList(protocol)
	self.data:SetSpecialAnecdoteList2(nil, protocol.task_list)
end

function AnecdoteCtrl:SCSpecialAnecdoteSingleList(protocol)
	self.data:SetSpecialAnecdoteList2(protocol.task_type, protocol.task_info)
end

function AnecdoteCtrl:CSSpecialAnecdoteReq(task_type , task_seq)
	local protocol = GetProtocol(CSSpecialAnecdoteReq)
	protocol.task_type = task_type
	protocol.task_seq = task_seq
	SendProtocol(protocol)
end

-- 新奇闻挑战完成
function AnecdoteCtrl:NewAnecdoteAction(type, seq)
	local p = {}
	p.action_type = NewAnecdoteActionType.GameSucc
	p.param1 = type
	p.param2 = seq
	self:SendTaskActionReq(p)
end