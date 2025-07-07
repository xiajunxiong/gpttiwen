
FILE_REQ("modules/courage_challenge/courage_challenge_data")
VIEW_REQ("modules/courage_challenge/courage_challenge_view")
VIEW_REQ("modules/courage_challenge/courage_challenge_reward")
VIEW_REQ("modules/courage_challenge/courage_challenge_finish")
VIEW_REQ("modules/courage_challenge/courage_challenge_rank")
VIEW_REQ("modules/courage_challenge/courage_challenge_funopen")
VIEW_REQ("modules/courage_challenge/courage_challenge_finish_auto")
VIEW_REQ("modules/courage_challenge/courage_challenge_achievement")

CourageChallengeCtrl = CourageChallengeCtrl or BaseClass(BaseCtrl)

CourageChallengeRankType = {
	Main = 0,
	Guild = 1,
    Friends = 2,
	Cross = 3
}

function CourageChallengeCtrl:__init()
	if CourageChallengeCtrl.Instance ~= nil then
		Debuger.LogError("[CourageChallengeCtrl] attempt to create singleton twice!")
		return
	end	
	CourageChallengeCtrl.Instance = self
	self.data = CourageChallengeData.New()
	self:RegisterAllProtocols()
end

function CourageChallengeCtrl:OnInit()
	RemindCtrl.Instance:Remind():Register(Mod.Experience.GuMo,self.data.reward_red_piont,BindTool.Bind(self.FlushRewardRemind, self))
	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE,function()
		ViewMgr:OpenView(CourageChallengeView)
		self:OnPetHelperFunc()
	end)

	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_NEW_COURAGE_CHALLENGE,function()
		ViewMgr:OpenView(CourageChallengeView)
		self:OnPetHelperFunc()
	end)
	ChallengeNaviCtrl.Instance:RegisterOpenReqFunc(Mod.Experience.GuMo,function() 
		SceneLogic.Instance:AutoToNpcs(CourageChallengeData.NPC)
	end)
	ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Experience.GuMo,function()
		return self.data:GetCurrLevel()
    end)

	ChallengeNaviCtrl.Instance:RegisterOpenReqFunc(Mod.Experience.ShiShen,function() 
		SceneLogic.Instance:AutoToNpcs(CourageChallengeData.NPC)
	end)
	ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Experience.ShiShen,function()
		return self.data:GetCurrLevel2()
    end)
    ActFormationData.Instance:RegistFunctions(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_COURAGE_CHALLENGE,
	{MainType = self,challenge_func = [[OnChallenge]],pet_on_limit_func = [[GetPetNum]],assist_text_func = [[GetAssistTip]], assist_limit_text_func = [[GetAssistLimitTip]]})

	-- self:SendNewCourageChallengeReq(0)
end

function CourageChallengeCtrl:FlushRewardRemind()
	return self.data:GetRewardNum()
end

function CourageChallengeCtrl:OnUnloadGameLogic()
	CourageChallengeData.Instance:Initialization()
	self.data.red_point_data.val = false
	self.data.new_rank_list = {}
	self.data.final_select_data = nil
end

function CourageChallengeCtrl:__delete()
	CourageChallengeCtrl.Instance = nil
	RemindCtrl.Instance:Remind():UnRegister(Mod.Experience.GuMo)
	self:UnRegisterAllProtocols()
end

function CourageChallengeCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSCourageChallengeOp)
	self:RegisterProtocol(CSCourageChallengeRankListReq)
	self:RegisterProtocol(SCCourageChallengeAllInfo,"OnCourageChallengeAllInfo")
	self:RegisterProtocol(SCCourageChallengeSingleInfo,"OnCourageChallengeSingleInfo")
	self:RegisterProtocol(SCCourageChallengeScoreInfo,"OnCourageChallengeScoreInfo")
	self:RegisterProtocol(SCCourageChallengeRankListInfo,"OnCourageChallengeRankListInfo")
	self:RegisterProtocol(SCCourageChallengePetHelperInfo,"OnCourageChallengePetHelperInfo")

	--弑神之塔
	self:RegisterProtocol(CSNewCourageChallengeOp)
	self:RegisterProtocol(SCNewCourageChallengeLevelListInfo, "SCNewCourageChallengeLevelListInfo")
	self:RegisterProtocol(SCNewCourageChallengeLevelSignleInfo, "SCNewCourageChallengeLevelSignleInfo")
	self:RegisterProtocol(SCNewCourageChallengeRewardInfo, "SCNewCourageChallengeRewardInfo")

	BehaviorData.Instance:CustomNpcTalkBtnsId(40013, BindTool.Bind(self.SetupBehaviourBtn, self))
end

function CourageChallengeCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSCourageChallengeOp)
	self:UnRegisterProtocol(CSCourageChallengeRankListReq)
	self:UnRegisterProtocol(SCCourageChallengeAllInfo)
	self:UnRegisterProtocol(SCCourageChallengeSingleInfo)
	self:UnRegisterProtocol(SCCourageChallengeScoreInfo)
	self:UnRegisterProtocol(SCCourageChallengeRankListInfo)
	self:UnRegisterProtocol(SCCourageChallengePetHelperInfo)
end

--0 信息 1 领取评分奖励 2 领取满星大奖 3 打开界面通知给服务端
function CourageChallengeCtrl:SendCourageChallengeReq(op_type,param1,param2)
	local protocol = GetProtocol(CSCourageChallengeOp)
	protocol.op_type = op_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

function CourageChallengeCtrl:SendCourageChallengeRankListReq(rank_type,count)
	local protocol = GetProtocol(CSCourageChallengeRankListReq)
	protocol.rank_type = rank_type or 0
	protocol.count = count or 0
	SendProtocol(protocol)
end

function CourageChallengeCtrl:SendBattleReq(param1,param2)
	BattleCtrl.BattleModeFightReq{battle_mode = BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE,param1 = param1,param2 = param2}
end
function CourageChallengeCtrl:SendBattleReq2(param1,param2)
	BattleCtrl.BattleModeFightReq{battle_mode = BATTLE_MODE.BATTLE_MODE_NEW_COURAGE_CHALLENGE,param1 = param1,param2 = param2}
end
function CourageChallengeCtrl:OnCourageChallengeAllInfo(protocol)
	self.data:SetCourageChallengeAllInfo(protocol)
end

function CourageChallengeCtrl:OnCourageChallengeSingleInfo(protocol)
	local param_t = {layer_info = protocol.layer_info or {},first_kill = self.data:GetFirstKill(protocol.level,protocol.seq),
	type = CourageChallengeData.PagodaType.GuMo}
	if PrefsInt(PrefKeys.CourageChallengeAuto())==1 then
		ViewMgr:OpenView(CourageChallengeFinishAutoView,param_t)
	else
		ViewMgr:OpenView(CourageChallengeFinishView,param_t)
	end
	self.data:SetCourageChallengeSingleInfo(protocol)
end

function CourageChallengeCtrl:OnCourageChallengeScoreInfo(protocol)
	self.data:SetCourageChallengeScoreInfo(protocol)
end

function CourageChallengeCtrl:OnCourageChallengeRankListInfo(protocol)
	self.data:SetCourageChallengeRankListInfo(protocol)
end

function CourageChallengeCtrl:SetupBehaviourBtn(btn_list,obj_vo)
	if obj_vo and obj_vo.id == 40013 then
		local button = NpcDialogBehBtnData.FuncCreate(NpcBehaviorBtn[BATTLE_MODE.BATTLE_MODE_COURAGE_CHALLENGE],
		function()
			local opened, open_tip = FunOpen.Instance:GetFunIsOpened(Mod.Experience.GuMo)
			if not opened then
				PublicPopupCtrl.Instance:Center(open_tip)
				return
			end
			if TeamCtrl.Instance:Data():InTeam() then
				PublicPopupCtrl.Instance:AlertTip(Language.CourageChallenge.TeamTip,function()
					TeamCtrl.Instance:SendExitTeam()
					SceneLogic.Instance:AutoToNpcs(CourageChallengeData.NPC)
				end)
				return 
			end
			SceneLogic.Instance:AutoToNpcs(CourageChallengeData.NPC)
		end)
		button:SetIconType(NpcIconType.Act)
		table.insert(btn_list, button)
	end
end

function CourageChallengeCtrl:OnChallenge()
	local data = CourageChallengeData.Instance:GetFinalSelectData()
	if data == nil then return end
	if data.type == CourageChallengeData.PagodaType.GuMo then
		CourageChallengeCtrl.Instance:SendBattleReq(data.layer_level,data.level_seq)
	else
		CourageChallengeCtrl.Instance:SendBattleReq2(data.layer_level,data.level_seq)
	end
	ViewMgr:CloseView(CourageChallengeView)
end

--宠物助战
function CourageChallengeCtrl:OnCourageChallengePetHelperInfo(protocol)
	self.data:SetPetNum(protocol.pet_num)
	self.PetHelperFunc = function()
		ViewMgr:OpenView(CourageChallengeTipView,{num = self.data:GetPetNum()})
	end
end

--宠物助战弹窗
function CourageChallengeCtrl:OnPetHelperFunc()
	if self.PetHelperFunc then
		TimeHelper:CancelTimer(self.ShowTimer)
		self.ShowTimer = Invoke(function()
			self.PetHelperFunc()
			self.PetHelperFunc = nil
		end)
	end
end

--宠物助战提示
function CourageChallengeCtrl:GetAssistTip(assist_num)
	local pet_num = CourageChallengeData.Instance:GetCurrPetNum()
	local data = CourageChallengeData.Instance:GetNextPetConfig(pet_num + 1)
	if data ~= nil then
		local config = CourageChallengeData.Instance:GetMonsterConfig(data.monster_group)
		if pet_num == 0 then
			return Format(Language.CourageChallenge.LockAssistTip,data.level,config.name,pet_num + 1), Format(Language.CourageChallenge.AssistTip1, data.level, config.name)
		end
		return Format(Language.CourageChallenge.AssistTip,data.level,config.name,pet_num + 1,assist_num,pet_num), Format(Language.CourageChallenge.AssistTip1, data.level, config.name)
	else
		return Format(Language.CourageChallenge.EndAssistTip,assist_num,pet_num)
	end
end

function CourageChallengeCtrl:GetAssistLimitTip()
	local cur_pet_num = CourageChallengeData.Instance:GetCurrPetNum()
	local tip_list = {}
	for i = cur_pet_num, 4 do
		local data = CourageChallengeData.Instance:GetNextPetConfig(i + 1)
		if data ~= nil then
			local config = CourageChallengeData.Instance:GetMonsterConfig(data.monster_group)
			table.insert(tip_list, Format(Language.CourageChallenge.AssistTip1, data.level, config.name))
		end
	end
	return tip_list

	-- local data = CourageChallengeData.Instance:GetNextPetConfig(pet_num + 1)
	-- if data ~= nil then
	-- 	local config = CourageChallengeData.Instance:GetMonsterConfig(data.monster_group)
	-- 	if pet_num == 0 then
	-- 		return Format(Language.CourageChallenge.AssistTip1, data.level, config.name)
	-- 	end
	-- 	return Format(Language.CourageChallenge.AssistTip1, data.level, config.name)
	-- end
end

--当前最大可助战宠物数量
function CourageChallengeCtrl:GetPetNum()
	return CourageChallengeData.Instance:GetCurrPetNum()
end

function CourageChallengeCtrl:SendNewCourageChallengeReq(op_type, param)
	local protocol = GetProtocol(CSNewCourageChallengeOp)
	protocol.op_type = op_type
	protocol.param = param or 0
	SendProtocol(protocol)
end

function CourageChallengeCtrl:SCNewCourageChallengeLevelListInfo(protocol)
	self.data:SetNewCourageChallengeAllInfo(protocol)
end

function CourageChallengeCtrl:SCNewCourageChallengeLevelSignleInfo(protocol)

	local param_t = {layer_info = protocol.info or {},first_kill = self.data:GetFirstKill2(protocol.level,protocol.seq), type = CourageChallengeData.PagodaType.ShiShen}
	if PrefsInt(PrefKeys.CourageChallengeAuto())==1 then
		ViewMgr:OpenView(CourageChallengeFinishAutoView,param_t)
	else
		ViewMgr:OpenView(CourageChallengeFinishView,param_t)
	end

	self.data:SetNewCourageChallengeSingleInfo(protocol)
end

function CourageChallengeCtrl:SCNewCourageChallengeRewardInfo(protocol)
	self.data:SetNewCourageChallengeRewardInfo(protocol)
	ViewMgr:FlushView(CourageChallengeView)
end