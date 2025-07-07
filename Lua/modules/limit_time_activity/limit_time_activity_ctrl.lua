FILE_REQ("modules/limit_time_activity/answer_data")
VIEW_REQ("modules/limit_time_activity/answer_view")

LimitTimeActivityCtrl = LimitTimeActivityCtrl or BaseClass(BaseCtrl)
function LimitTimeActivityCtrl:__init()
	if LimitTimeActivityCtrl.Instance ~= nil then
		Debuger.LogError("create LimitTimeActivityCtrl Instance twice")
	end
	LimitTimeActivityCtrl.Instance = self
	self.data = AnswerData.New()
	self:RegisterProtocol(CSAnswerOp)
	self:RegisterProtocol(SCAnswerAllInfo,"OnAnswerAllInfo")
	-- self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,
	-- BindTool.Bind(self.RoleNoticeComplete, self))
end

function LimitTimeActivityCtrl:__delete()
	LimitTimeActivityCtrl.Instance = nil
	EventSys:UnBind(self.complete_handle)
end

function LimitTimeActivityCtrl:OnUnloadGameLogic()
	self.data.cur_answer_num = 0
	self.data.sm_data:Clear()
end

function LimitTimeActivityCtrl:OnAnswerAllInfo(protocol)
	self.data:OnServerData(protocol)
	local num = protocol.answer_num
	local total_num = self.data:GetAnswerNum()
	if num >= total_num then
		AnswerCtrl.Instance:OnOpenRewardInfo()
	end
	self:SetBehaviorNpc(num == total_num)
	ActivityData.Instance:SetActivityFinishInfo(ActType.WSTQuesten,num,total_num)
end

function LimitTimeActivityCtrl:RoleNoticeComplete()
	ActivityData.Instance:SetActivityFinishInfo(ActType.WSTQuesten,0,self.data:GetAnswerNum())
end

function LimitTimeActivityCtrl:SendAnswer(op_type,param1)
	local protocol = GetProtocol(CSAnswerOp)
	protocol.op_type = op_type or 0
	protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

function LimitTimeActivityCtrl:SetupBehaviourBtn(btn_list,obj_vo)
	-- if obj_vo and obj_vo.id == self.data:GetNpcId() then
	-- 	local answer_num = self.data:GetCurAnswerNum()
	-- 	--找回奖励
	-- 	local find_reward_list,cost_type = self.data:GetFindRewardData()
	-- 	for _,re in pairs(find_reward_list or {}) do
	-- 		re.num = re.num * answer_num
	-- 	end
	-- 	local cost = RewardData.Instance:GetFindFree(FindType.WSTQuesten,cost_type,answer_num)
	-- 	local tips_view_param = {
	-- 		cost = cost, 
	-- 		cost_type = cost_type,
	-- 		name = "", 	--文字超框，这里不显示活动名
	-- 		rewards = find_reward_list or {}, 
	-- 		confirm_func = BindTool.Bind(TaskCtrl.SendOnekeyFinish, TaskCtrl.Instance, OnekeyFinishType.WSTQuesten)
	-- 	}
	-- 	--普通奖励
	-- 	local reward_cfg = self.data:GetRewardCfg(RoleData.Instance:GetRoleLevel())
	-- 	local reward_list = {}
	-- 	table.insert(reward_list,Item.Init(CommonItemId.Coin,reward_cfg.coin * self.data:GetAnswerNum()))
	-- 	if reward_cfg.exp ~= 0 then
	-- 		table.insert(reward_list,Item.Init(CommonItemId.Exp,reward_cfg.exp * self.data:GetAnswerNum()))
	-- 	end
	-- 	local is_open = RewardData.Instance:IsFindOpen(FindType.WSTQuesten)
	-- 	local det = NpcDialogData.CreateDetailInfo1{tips = {Language.Answer.NpcDetails1,Language.Answer.NpcDetails2}, 
	-- 		act_type = ActType.WSTQuesten,
	-- 		rewards = reward_list,
	-- 		confirm_show = Language.Answer.BeginAnswer,
	-- 		-- custom_show = is_open and  Language.Answer.OneKeyCom or Language.Answer.Com,
	-- 		-- custom_func = is_open and BindTool.Bind(ViewManager.OpenView, ViewMgr, DialogTipsBackView,tips_view_param) or nil,
	-- 		custom_show = Language.NpcDialog.Detail1.custom_default,
	-- 		custom_func = function()
	-- 			ViewMgr:CloseView(NpcDialogView)
	-- 		end,
	-- 		custom_type = DetailInfo1Custom.back
	-- 	}
	-- 	local button = NpcDialogBehBtnData.FuncCreate(Language.Answer.NpcBtnAnswer,function()
	-- 		if ActivityData.Instance:IsNoEnoughLevel(ActType.WSTQuesten) then
	-- 			return
	-- 		end
	-- 		PublicPopupCtrl.Instance:BattleCinematics(BattleCinematics.Type.KaiShiDaTi,1,nil,function()
	-- 			ViewMgr:OpenView(AnswerView)
	-- 		end,1.1)			
	-- 	end, true, det)
	-- 	button:SetIconType(NpcIconType.Act)
	-- 	table.insert(btn_list, button)
	-- end
end

function LimitTimeActivityCtrl:SetBehaviorNpc(is_complete)
	if is_complete then
		if self.npc_behaviour_handle then
			BehaviorData.Instance:RemoveCustomNpcTalkBtnsId(self.npc_behaviour_handle)
			self.npc_behaviour_handle = nil
		end
	else
		-- if self.npc_behaviour_handle == nil then
		-- 	self.npc_behaviour_handle = BehaviorData.Instance:CustomNpcTalkBtnsId(self.data:GetNpcId(),
		-- 	BindTool.Bind(self.SetupBehaviourBtn, self))
		-- end
	end
end