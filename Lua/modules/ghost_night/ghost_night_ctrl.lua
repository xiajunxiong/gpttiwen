FILE_REQ("modules/ghost_night/ghost_night_data")
VIEW_REQ("modules/ghost_night/ghost_night_view")

GhostNightCtrl = GhostNightCtrl or BaseClass(BaseCtrl)

function GhostNightCtrl:__init()
	if GhostNightCtrl.Instance ~= nil then
		Debuger.LogError("[GhostNightCtrl] attempt to create singleton twice!")
		return
	end	
    GhostNightCtrl.Instance = self
    self.data = GhostNightData.New()
	self:RegisterAllProtocols()
end

function GhostNightCtrl:OnUnloadGameLogic()
end

function GhostNightCtrl:RegisterAllProtocols()
	self:RegisterProtocol(SCGhostNightInfo,"OnGhostNightInfo")
	self:RegisterProtocol(SCGhostNightPokerInfo,"OnGhostNightPokerInfo")
	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_GHOST_NIGHT,function(is_win)
		if is_win == 1 then
			local item_list = DataHelper.FormatItemList(self.data:GetWorldBossRewardData())
			MainOtherCtrl.Instance:OpenGiftView(item_list,GIFT_OPEN_TYPE.CONGRATULATIONS)
		-- PokerCtrl.Instance:OnOpenPokerView(self.data:GetPokerList(),POKER_REQ_OPER_TYPE.GHOST_NIGHT)
		end
	end)
    ActivityData.Instance:CustomClickHandle(ActType.GhostNight, BindTool.Bind(self.OnGhostNight, self))
end

function GhostNightCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCGhostNightInfo)
	self:UnRegisterProtocol(SCGhostNightPokerInfo)
end

function GhostNightCtrl:OnGhostNightInfo(protocol)
	self.data:SetGhostNightInfo(protocol)
end

--翻牌
function GhostNightCtrl:OnGhostNightPokerInfo(protocol)
	self.data:SetPokerInfo(protocol.info)
end

--替换队伍中的翻牌信息
function GhostNightCtrl:OnGhostNightPokerChange(protocol)
	PokerData.Instance:GetMypokerByrIndex(self.data:GetPokerInfo(),protocol)
end

function GhostNightCtrl:OnGhostNightScene()
	if SceneData.Instance:SceneId() == SceneLogic.Instance:MainCityID() then
        if not LoginData.Instance:IsOnCrossSever() then
            LoginCtrl.Instance:SendCrossStartReq()
        end
		PublicPopupCtrl.Instance:Center(Language.GhostNight.SceneTip)
    else
        SceneLogic.Instance:AutoToPos(SceneLogic.Instance:MainCityID(), nil, function()
            if not LoginData.Instance:IsOnCrossSever() then
                LoginCtrl.Instance:SendCrossStartReq()
            end
        end, nil, true)
    end
end

function GhostNightCtrl:OnGhostNight()
	if ActivityData.Instance:GetActivityStatus(ActType.GhostNight) == ActStatusType.Standy then
		PublicPopupCtrl.Instance:Center(Language.GhostNight.ActivityTip)
		return true
	end
    ViewMgr:OpenView(GhostNightView)
end