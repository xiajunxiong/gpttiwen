FILE_REQ("modules/role_setting/role_setting_data")
VIEW_REQ("modules/role_setting/role_setting_view")
VIEW_REQ("modules/role_setting/cdkey_exchange_view")
VIEW_REQ("modules/role_setting/feedback_view")
VIEW_REQ("modules/role_setting/role_change_head")

RoleSettingCtrl = RoleSettingCtrl or BaseClass(BaseCtrl)

function RoleSettingCtrl:__init()
	if RoleSettingCtrl.Instance ~= nil then
		Debuger.LogError("[RoleSettingCtrl] attempt to create singleton twice!")
		return
	end
	RoleSettingCtrl.Instance = self

	self.data = RoleSettingData.New()

	self:RegisterProtocol(CSRoleSystemSetReq)
	self:RegisterProtocol(SCRoleSystemSetInfo, "RoleSettingInfo")
	self:RegisterProtocol(SCEncounterMineMonsterFlag, "EncounterMineMonsterFlag")
	self:RegisterProtocol(CSSetEncounterMineMonsterFlag)

	self:RegisterProtocol(CSRolePatrol)
	self:RegisterProtocol(CSEscapeFromCurrentPosition)
	self:RegisterProtocol(CSChangeHeadReq)
	--注册红点逻辑
	RemindCtrl.Instance:Remind():Register(Mod.Setting.Main,self.data.head_red_point,function()
		return self.data:GetHeadRedPointNum()
	end)
end

function RoleSettingCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSRoleSystemSetReq)
	self:UnRegisterProtocol(SCRoleSystemSetInfo)
	self:UnRegisterProtocol(SCEncounterMineMonsterFlag)
	self:UnRegisterProtocol(CSSetEncounterMineMonsterFlag)
	self:UnRegisterProtocol(CSRolePatrol)
	self:UnRegisterProtocol(CSEscapeFromCurrentPosition)
	self:UnRegisterProtocol(CSChangeHeadReq)
	RemindCtrl.Instance:Remind():UnRegister(Mod.Setting.Main)
end

function RoleSettingCtrl:__delete()
	if RoleSettingCtrl.Instance == self then
		RoleSettingCtrl.Instance = nil
    end
end

function RoleSettingCtrl:EncounterMineMonsterFlag(protocol)
	FightCtrl.Instance:Data():SetMeetMonsterFlag(protocol.flag)
end

function RoleSettingCtrl:SaveRoleSettingInfo()
	local protocol = GetProtocol(CSRoleSystemSetReq)
	local count = 0
	local change_info = {}
	for i=1,ROLE_SETTING_TYPE.ROLE_SETTING_TYPE_MAX do
		local value_1 = self.data.role_setting_data[i]
		local value_2 = self.data.role_setting_data_server[i]
		if value_1 ~= value_2 then
			change_info[i] = value_1
			self.data.role_setting_data_server[i] = value_1
			count = count + 1
		end
	end
	if count == 0 then
		return
	end
	protocol.count = count
	protocol.setting_info = change_info
    SendProtocol(protocol)
end

function RoleSettingCtrl:RoleSettingInfo(protocol)
	self.data:SetRoleSettingInfo(protocol.setting_info)
end

function RoleSettingCtrl:SetEncounterMineMonsterFlag(flag)
	if self.data.shield_fight_monster == true then
		return
	end
	local protocol = GetProtocol(CSSetEncounterMineMonsterFlag)
	protocol.flag = flag or 0
	SendProtocol(protocol)
end

function RoleSettingCtrl:SendRolePatrol(is_patrol)
	local protocol = GetProtocol(CSRolePatrol)
	protocol.is_start_or_stop = is_patrol and 1 or 0
    SendProtocol(protocol)
	self:SetEncounterMineMonsterFlag(is_patrol and 1 or 0)
end

function RoleSettingCtrl:SendEscapeFromCurrentPosition()
	local protocol = GetProtocol(CSEscapeFromCurrentPosition)
    SendProtocol(protocol)
end

--改变头像请求
function RoleSettingCtrl:SendChangeHeadReq(avatar_id)
    local protocol = GetProtocol(CSChangeHeadReq)
    protocol.avatar_id = avatar_id or 0
	SendProtocol(protocol)
end

function RoleSettingCtrl:OnUnloadGameLogic()
	self.data:ClearData()
end