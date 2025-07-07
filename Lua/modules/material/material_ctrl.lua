FILE_REQ("modules/material/material_data")
VIEW_REQ("modules/material/material_view")
VIEW_REQ("modules/material/material_other")
VIEW_REQ("modules/material/material_menu_view")
VIEW_REQ("modules/material/material_challenge_view")
VIEW_REQ("modules/material/material_challenge_finish")

MaterialCtrl = MaterialCtrl or BaseClass(BaseCtrl)

function MaterialCtrl:__init()
	if MaterialCtrl.Instance ~= nil then
		Debuger.LogError("[MaterialCtrl] attempt to create singleton twice!")
		return
	end	
    MaterialCtrl.Instance = self
    self.data = MaterialData.New()
	self:RegisterAllProtocols()
end

function MaterialCtrl:OnUnloadGameLogic()
	self.data:ClearData()
end

function MaterialCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSMaterialReq)
	self:RegisterProtocol(SCMaterialSceneInfo, "OnMaterialSceneInfo")
	self:RegisterProtocol(SCMaterialAllInfo, "OnMaterialAllInfo")
	self:RegisterProtocol(SCMaterialSingleInfo, "OnMaterialSingleInfo")
	self:RegisterProtocol(SCMaterialOtherInfo, "OnMaterialOtherInfo")
end

function MaterialCtrl:OnInit()
	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_MATERIAL,function(is_win)
		local is_first_kill = self.data:IsFhishFirstKill()
		self.data:ResetsFirstKill()
		MainOtherCtrl.Instance:SetWaitViewFunc()
		local data = self:GetCurrData()
		if data and data.finish_talk_id ~= 0 and is_win == 1 then
			NpcDialogCtrl.Instance:NpcDialogView({talk_id = data.finish_talk_id},function()
				self:OnMaterial(data,is_first_kill)
			end)
		else
			self:OnMaterial(data,is_first_kill)
		end
	end)
	RemindCtrl.Instance:Remind():Register(Mod.FightFB.Material,self.data.reward_data,BindTool.Bind(self.CheckRemind, self))
end

function MaterialCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSMaterialReq)
	self:UnRegisterProtocol(SCMaterialSceneInfo)
	self:UnRegisterProtocol(SCMaterialAllInfo)
	self:UnRegisterProtocol(SCMaterialSingleInfo)
	self:UnRegisterProtocol(SCMaterialOtherInfo)
	RemindCtrl.Instance:Remind():UnRegister(Mod.FightFB.Material)
end

function MaterialCtrl:SendMaterialReq(op_type,param1)
	local protocol = GetProtocol(CSMaterialReq)
	protocol.op_type = op_type or 0
	protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

function MaterialCtrl:OnMaterialSceneInfo(protocol)
	self.data:SetMaterialSceneInfo(protocol)
	self:OnNotice(protocol.talk_npc_id,self:GetCurrData())
end

function MaterialCtrl:OnMaterialAllInfo(protocol)
	self.data:SetMaterialAllInfo(protocol)
end

function MaterialCtrl:OnMaterialSingleInfo(protocol)
	self.data:SetMaterialSingleInfo(protocol)
end

function MaterialCtrl:OnMaterialOtherInfo(protocol)
	self.data:SetMaterialOtherInfo(protocol)
end

function MaterialCtrl:AutoToNpc(id,func)
	SceneLogic.Instance:AutoToNpc(self.data:GetSceneNpcVo(id),func,true)
end

function MaterialCtrl:OnNotice(talk_npc_id,data)
	MainOtherCtrl.Instance:SetWaitViewFunc(function()
		self:AutoToNpc(talk_npc_id == 0 and data.boss_id or talk_npc_id)
	end)
end

function MaterialCtrl:OnSweepReward(reward_list)
	-- table.insert(reward_list,Item.New({item_id = CommonItemId.Exp,num = self.data:GetExpNum()}))
	PublicPopupCtrl.Instance:Reward2(reward_list,Language.Love.WrongRewardTip[1])
end

function MaterialCtrl:CheckRemind()
	return self.data:GetRemindNum()
end

function MaterialCtrl:OnMaterial(data,is_first_kill)
	if is_first_kill == true then
		local next_data,show_data = self.data:GetNextData(data)
		self:OnBackChallenge(show_data,next_data)
	else
		self:OnBackChallengeView()
	end
end

function MaterialCtrl:GetCurrData()
	return self.data:GetChapterConfig(self.data:GetChallengeData())
end

function MaterialCtrl:GetShowData()
	return self.data:GetChapterShowData(self:GetCurrData().chapter)
end

function MaterialCtrl:OnBackChallengeView()
	self:OnBackChallenge(self:GetShowData(),self:GetCurrData())
end

function MaterialCtrl:OnBackChallenge(show_data,next_data)
	self.data:SetSelectChapterData(show_data)
	self.data:SetRecommendData(next_data)
	ViewMgr:OpenView(MaterialChallengeView)
	SceneCtrl.Instance:RequestLeaveFb()
	self.data:SetFinishRewardList()
end