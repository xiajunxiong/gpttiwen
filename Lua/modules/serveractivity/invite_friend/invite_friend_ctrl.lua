InviteFriendCtrl = InviteFriendCtrl or BaseClass(BaseCtrl)

function InviteFriendCtrl:__init()
	if InviteFriendCtrl.Instance ~= nil then
		Debuger.LogError("[InviteFriendCtrl] attempt to create singleton twice!")
		return
	end
	InviteFriendCtrl.Instance = self
    self:RegisterAllProtocols()
    self:RegisterRemind()
    FunOpen.Instance:SelfCondition(Mod.RandActivity.InviteFriend, function()
        if IS_WCMINI ~= nil then
            return IS_WCMINI()
        else
            return false
        end
        -- return true
    end)
end

function InviteFriendCtrl:__delete()
    self:UnRegisterAllProtocols()
    InviteFriendCtrl.Instance = nil
end

function InviteFriendCtrl:RegisterAllProtocols()
    -- self:RegisterProtocol(SCRAHalloweenBlessingInfo, "OnSCRAHalloweenBlessingInfo")
    -- self:RegisterProtocol(SCRAHalloweenBlessingRewardInfo, "OnSCRAHalloweenBlessingRewardInfo")
end

function InviteFriendCtrl:RegisterRemind()
    -- RemindCtrl.Instance:Remind():Register(Mod.Act_Halloween.Pray, self.data.pray_data, BindTool.Bind(self.Remind, self))
end


------------------------------------------
InviteFriendData = InviteFriendData or BaseClass()
function InviteFriendData:__init()
    if InviteFriendData.Instance ~= nil then
        Debuger.LogError("[InviteFriendData] attempt to create singleton twice!")
        return
    end
    InviteFriendData.Instance = self
end