FILE_REQ("modules/serveractivity/athletics/act_athletics_data")
VIEW_REQ("modules/serveractivity/athletics/act_athletics_view")
AthleticsCtrl =AthleticsCtrl or BaseClass(BaseCtrl)

--新服竞技
function AthleticsCtrl:__init()
	if AthleticsCtrl.Instance ~= nil then
		Debuger.LogError("[AthleticsCtrl] attempt to create singleton twice!")
		return
	end
	AthleticsCtrl.Instance=self
	self.data=AthleticsData.New()
	RemindCtrl.Instance:Remind():Register(Mod.ServerActivity.Athletics,self.data.historyData,function()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.KF_ATHLETICS) then
			return 0
		end
		return self.data:GetIsCanHistoryReward()
	end)
	self:RegisterProtocol(SCSportsHistroyInfo,"OnSCSportsHistroyInfo")
end

function AthleticsCtrl:__delete()
	self:UnRegisterProtocol(SCSportsHistroyInfo)
	AthleticsCtrl.Instance=nil
	Delete(self,"data")
	RemindCtrl.Instance:Remind():UnRegister(Mod.ServerActivity.Athletics)
end

function AthleticsCtrl:OnSCSportsHistroyInfo(protocol)
	self.data:SetHistoryData(protocol)
end