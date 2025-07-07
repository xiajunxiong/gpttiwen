FILE_REQ("modules/dream_notes/dream_notes_data")
FILE_REQ("modules/dream_notes/dream_notes_config")
VIEW_REQ("modules/dream_notes/dream_notes_view")
VIEW_REQ("modules/dream_notes/dream_notes_menu_view")
VIEW_REQ("modules/dream_notes/dream_notes_rank_view")
VIEW_REQ("modules/dream_notes/dream_notes_reward_view")

DreamNotesCtrl = DreamNotesCtrl or BaseClass(BaseCtrl)

function DreamNotesCtrl:__init()
	if DreamNotesCtrl.Instance ~= nil then
		Debuger.LogError("[DreamNotesCtrl] attempt to create singleton twice!")
		return
	end
	DreamNotesCtrl.Instance = self
	self.data = DreamNotesData.New()
	self:RegisterAllProtocols()
end

function DreamNotesCtrl:__delete()
	DreamNotesCtrl.Instance = nil
end

function DreamNotesCtrl:OnUnloadGameLogic()
	self.data:ClearData()
end

function DreamNotesCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSDreamNotesOp)
	self:RegisterProtocol(SCDreamNotesInfo,"OnDreamNotesInfo")
	self:RegisterProtocol(SCDreamNotesSceneInfo,"OnDreamNotesSceneInfo")
end

function DreamNotesCtrl:OnInit()
	FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_DREAM_NOTES_CHALLENGE,{
		reward_func = function()
			if not self.data:IsFhishFirstKill() then
				return self.data:GetRandRewardData(self.data:GetPassLevelData(self.data:GetChallengeLevel()))
			end
			return {}
        end,
	})
	--梦渊自动关闭奖励界面等级限制
	local IS_AUTO_CLOSE_LEVEL = 30

	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_DREAM_NOTES_CHALLENGE,function(is_win)
		MainOtherCtrl.Instance:SetWaitViewFunc(nil)
		local data = self.data:GetPassLevelData(self.data:GetChallengeLevel())
		if self.data:IsFhishFirstKill() and is_win == 1 then
			ViewMgr:OpenView(DreamNotesRewardView,{reward_list = self.data:GetFinishRewardData(data),close_func = function()
				DreamNotesCtrl.Instance:RequestLeaveFb(nil,1)
			end,is_auto_close = RoleData.Instance:GetRoleLevel() < IS_AUTO_CLOSE_LEVEL})
		else
			self:RequestLeaveFb(data,is_win)
		end
		self.data:ResetsFirstKill()
	end)

	self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.OnRoleNoticeComplete, self))

	ActivityData.Instance:RegisterCondition(ActType.DreamNotes,function()
        return Format(Language.DreamNotes.SweepTiemsTip,self.data:GetHasTimes(),self.data:GetHasMaxTimes())
	end)
	EventSys:Bind(GlobalEventKeys.ServerOpenDayChange,function()
		self.data:CheckRemindNum()
	end)
	EventSys:Bind(GlobalEventKeys.RoleLevelChange,function()
		local pass_level = self.data:GetLockLevel()
		if pass_level then
			self.data:SetItemRemind(pass_level)
			UnityPlayerPrefs.SetInt(RoleData.Instance:GetRoleId() .. "DreamNotes",pass_level)
		end
	end)
end

function DreamNotesCtrl:OnSweepReard(old_times,remain_times)
	if old_times and old_times ~= 0 and remain_times < old_times then
		local reward_list = self.data:GetRandRewardData(self.data:GetPassLevelData(self.data:GetSelectData().level))
		PublicPopupCtrl.Instance:Reward2(reward_list,Language.Love.WrongRewardTip[1],nil,"_LocSaoDangChengGong")
	end
end

function DreamNotesCtrl:SendChallengeReq(op_type,param1)
	local protocol = GetProtocol(CSDreamNotesOp)
	protocol.op_type = op_type or 0
	protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

function DreamNotesCtrl:OnDreamNotesInfo(protocol)
	self.data:SetDreamNotesInfo(protocol)
end

function DreamNotesCtrl:OnDreamNotes()
	ViewMgr:OpenViewByKey(Mod.FightFB.DreamNotes)
end

function DreamNotesCtrl:OnRoleNoticeComplete()
	SocietyCtrl.Instance:SendGetPersonRankList(PERSON_RANK_TYPE.DREAM_NOTES_CHALLENGE,2)
end

function DreamNotesCtrl:OnDreamNotesSceneInfo(protocol)
	self.data:SetDreamNotesSceneInfo(protocol)
	local level = self.data:GetChallengeLevel()
	local data = self.data:GetPassLevelData(level)
	self:OnNotice(protocol.talk_npc_id,data)
end

function DreamNotesCtrl:AutoToNpc(id,func)
	SceneLogic.Instance:AutoToNpc(self.data:GetSceneNpcVo(id),func,true)
end

function DreamNotesCtrl:OnNotice(talk_npc_id,data)
	MainOtherCtrl.Instance:SetWaitViewFunc(function()
		self:AutoToNpc((talk_npc_id == 0 or data.level <= 3) and data.boss_id or talk_npc_id)
	end)
end

function DreamNotesCtrl:RequestLeaveFb(data,is_win)
	local data = data or self.data:GetPassLevelData(self.data:GetChallengeLevel())
	if data and data.finish_talk_id ~= 0 and is_win == 1 then
		NpcDialogCtrl.Instance:NpcDialogView({talk_id = data.finish_talk_id},function()
			SceneCtrl.Instance:RequestLeaveFb()
			TimeHelper:CancelTimer(self.ShowTimer)
			if self:IsCheckGuideManager(data,is_win) then
				return
			end
			if not self.data:GetIsAccepted() then
				self.ShowTimer = Invoke(function()
					self:OnDreamNotes()
				end,2)
			end
		end)
	else
		SceneCtrl.Instance:RequestLeaveFb()
		TimeHelper:CancelTimer(self.ShowTimer)
		if self:IsCheckGuideManager(data,is_win) then
			return
		end
		if not self.data:GetIsAccepted() then
			self.ShowTimer = Invoke(function()
				self:OnDreamNotes()
			end,2)
		end
	end
	if is_win == 1 then
		local config = MysteryStoreData.Instance:GetItemUnlockData()
		if config ~= nil then
			self.WaitLockFunc = config
		end
	end
end

--梦渊指引特殊处理 打完第三回
function DreamNotesCtrl:IsCheckGuideManager(data,is_win)
	TimeHelper:CancelTimer(self.ShowGuideTimer)
	if 1 == is_win and PrefsInt(PrefKeys.DreamNotesFirstGuid()) == 0 and data.seq == 2 then
		self.ShowGuideTimer = Invoke(function()
			GuideManager.Instance:Start(107)
			PrefsInt(PrefKeys.DreamNotesFirstGuid(), 1)
		end,2)
		return true
	end
end

function DreamNotesCtrl:SendCallback()
	SocietyCtrl.Instance:SendGetPersonRankList(PERSON_RANK_TYPE.DREAM_NOTES_CHALLENGE,2)
end

function DreamNotesCtrl:AutoFunctionEvent()
	--梦渊自动关闭奖励界面等级限制
	local IS_AUTO_CLOSE_LEVEL = 30
	--梦渊自动挑战场景ID
	local IS_AUTO_SCENE_ID = 1800
	if SceneLogic.Instance:SceneID() == IS_AUTO_SCENE_ID and RoleData.Instance:GetRoleLevel() < IS_AUTO_CLOSE_LEVEL then
		TimeHelper:CancelTimer(self.IsAutoFunctionTimer)
		self.IsAutoFunctionTimer = Invoke(function()
			local config = self.data:GetPassLevelData(self.data:GetChallengeLevel())
			SceneLogic.Instance:AutoToNpc(self.data:GetSceneNpcVo(config.boss_id),nil,true)
			self.IsAutoFunctionTimer = nil
		end,5)
    end
end