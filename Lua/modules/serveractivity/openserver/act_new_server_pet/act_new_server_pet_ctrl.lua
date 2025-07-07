VIEW_REQ("modules/serveractivity/openserver/act_new_server_pet/act_new_server_pet_view")

ActNewServerPetReqType = {
    Info = 0,       --信息
    Buy = 1,        --购买
}

ActNewServerPetCtrl = ActNewServerPetCtrl or BaseClass(BaseCtrl)

function ActNewServerPetCtrl:__init()
	if ActNewServerPetCtrl.Instance ~= nil then
		Debuger.LogError("[ActNewServerPetCtrl] attempt to create singleton twice!")
		return
	end	
	ActNewServerPetCtrl.Instance = self
	self.data = ActNewServerPetData.New()
	self:RegisterAllProtocols()
end

function ActNewServerPetCtrl:__delete()
	self:UnRegisterAllProtocols()
	ActNewServerPetCtrl.Instance = nil
	Delete(self, "data")
end

function ActNewServerPetCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAXingFuLingChongInfo, "OnSCRAXingFuLingChongInfo")
end

function ActNewServerPetCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCRAXingFuLingChongInfo)
end

function ActNewServerPetCtrl:OnInit()
    local card_remind_func = BindTool.Bind(self.Remind, self)
    RemindCtrl.Instance:Remind():Register(
        Mod.ServerActivity.NewServerPet,
        self.data.info_data,
        card_remind_func
    )
end

function ActNewServerPetCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function ActNewServerPetCtrl:SendReq(req_type)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_NEW_SERVER_PET, req_type)
end

function ActNewServerPetCtrl:OnSCRAXingFuLingChongInfo(protocol)
    self.data:SetInfo(protocol)
end

function ActNewServerPetCtrl:Remind()
    return self.data:Remind()
end


--------------------ActNewServerPetData--------------------
ActNewServerPetData = ActNewServerPetData or BaseClass()

function ActNewServerPetData:__init()
	if ActNewServerPetData.Instance ~= nil then
		Debuger.LogError("[ActNewServerPetData] attempt to create singleton twice!")
		return
	end
	ActNewServerPetData.Instance = self
    self.info_data = SmartData.New()
    self:ResetData()
end

function ActNewServerPetData:Remind()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_NEW_SERVER_PET) then
        return 0
    end
    if self.info_data.buy_count == 0 then
        local act_start = ActivityData.GetStartStampTime(ACTIVITY_TYPE.RAND_NEW_SERVER_PET)
        if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(ACTIVITY_TYPE.RAND_NEW_SERVER_PET)) then
            -- 活动首次红点，换设备重新提示
            return 1
        end
    end
    return 0
end

function ActNewServerPetData:ResetData()
    self.info_data.buy_count = 0
end

function ActNewServerPetData:SetInfo(protocol)
    self.info_data.buy_count = protocol.buy_count
    self.info_data:Notify()
end