FILE_REQ("modules/ghost_bust/ghost_bust_data")
VIEW_REQ("modules/ghost_bust/ghost_bust_achieve")
VIEW_REQ("modules/ghost_bust/ghost_bust_view")
VIEW_REQ("modules/ghost_bust/ghost_bust_pre_view")

GhostBustCtrl = GhostBustCtrl or BaseClass(BaseCtrl)
function GhostBustCtrl:__init()
	if GhostBustCtrl.Instance ~= nil then
		Debuger.LogError("[GhostBustCtrl] attempt to create singleton twice!")
		return
	end	
    GhostBustCtrl.Instance = self

	self.data = GhostBustData.New()
	self:RegisterAllProtocols()

	ActFormationData.Instance:RegistFunctions(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_YAO_XING_SHI_LIAN, 
    {MainType = GhostBustCtrl.Instance, challenge_func = "GhostChallenge", pet_on_limit_func = "PetOnLimitNum", assist_text_func = "AssistTextFunc"}
    )

	ViewMgr:RegisterOpenKeyFunc(Mod.GhostBust.Main, function ()
		ViewMgr:OpenView(GhostBustView)
		ViewMgr:OpenView(GhostBustPreView)
    end)
end

function GhostBustCtrl:OnInit()
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.GhostBust.Main,function()
		local num = self.data.ghost_info.pass_seq
		return num
    end)

	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_YAO_XING_SHI_LIAN,function(is_win)
		ViewMgr:OpenViewByKey(Mod.GhostBust.Main)
	end)

	FinishData.Instance:PassRegister(BATTLE_MODE.BATTLE_MODE_YAO_XING_SHI_LIAN,{
        reward_func = function()
            return self.data:GetFinishRewardData()
        end,
    })
end 

function GhostBustCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSYaoXingShiLiangReq)
	self:RegisterProtocol(SCYaoXingShiLianInfo,"RecvYaoXingShiLianInfo")
end 

function GhostBustCtrl:SendGhostBustSeq(oper,param)
    local protocol = GetProtocol(CSYaoXingShiLiangReq)
    protocol.oper_type = oper
	protocol.param = param or 0
	-- LogError("?SendGhostBustSeq",protocol)
    SendProtocol(protocol)
end

function GhostBustCtrl:RecvYaoXingShiLianInfo(protocol)
	-- LogError("1133?健哥哥救救我",protocol)
	self.data:SetGhostInfo(protocol)
end

function GhostBustCtrl:GhostChallenge()
	if ViewMgr:IsOpen(ActFormationView) then 
        ViewMgr:CloseView(ActFormationView)
	end 

    GhostBustCtrl.Instance:SendGhostBustSeq(1)
end

function GhostBustCtrl:PetOnLimitNum()
    return 4
end

--布阵界面助战信息文字
function GhostBustCtrl:AssistTextFunc(assist_num)
    return string.format(Language.CloudArena.FormationAssistInfo, assist_num)
end