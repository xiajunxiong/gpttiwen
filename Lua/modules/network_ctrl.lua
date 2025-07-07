NetworkCtrl = NetworkCtrl or BaseClass(BaseCtrl)

local HEART_BEAT_TIMEUP = 10
function NetworkCtrl:__init()
	if NetworkCtrl.Instance ~= nil then
		Debuger.LogError("[NetworkCtrl] attempt to create singleton twice!")
		return
	end
	NetworkCtrl.Instance = self

	self.login_netid = -1
	self.logic_netid = -1
	self.cross_netid = -1

	self.logic_server_ip = nil
	self.logic_server_port = 0

	self.cross_server_ip = nil
	self.cross_server_port = 0

	NetMgr:SetDisconnectHandler(BindTool.Bind(self.OnDisconnect,self))

	self:RegisterProtocol(SCHeartbeatResp,"OnHeadbeat")
	self:RegisterProtocol(CSHeartbeatReq)

	self:RegisterProtocol(SCLHeartBeat,"OnLHeartBeat")
	self:RegisterProtocol(CSLHeartBeat)

	self.wait_heart_count = 0--等待心跳计数
	self.heart_beat_repeat_handle = nil
	self.heart_beat_delay_handle = nil
	self.pause_beat = false
-- if not IS_EDITOR then
	-- Runner.Instance:RepeatCall(function ()

	-- end,3)
-- end
end

function NetworkCtrl:HeartBeatPause(_flag)
	self.pause_beat = _flag 
end

function NetworkCtrl:IsHeartBeatPause()
	return self.pause_beat
end

function NetworkCtrl:OnHeartBeatCheck()
	if self:IsHeartBeatPause() then return end
	local heart_beat_func =  function()
		local HeartBeatTest = UnityPlayerPrefs.HasKey(PrefKeys.HeartBeatTest)
		if not HeartBeatTest and self:LogicServerIsConnected() then
			if self.wait_heart_count > 1 then		--heart net check failed
					LogError("wait_heart_count====",self.wait_heart_count)
				self.wait_heart_count = 0
				if self.logic_netid > 0 then
					NetMgr:Disconnect(self.logic_netid)
				end
				if self.cross_netid > 0 then
					NetMgr:Disconnect(self.cross_netid)
				end
				Runner.Instance:CancelRepeat(self.heart_beat_repeat_handle)
				self.heart_beat_repeat_handle = nil
			else
				self.wait_heart_count = self.wait_heart_count + 1

				local protocol = GetProtocol(CSHeartbeatReq)
				SendProtocol(protocol)
			end
		elseif not HeartBeatTest and self:LoginServerIsConnected() then
			if self.wait_heart_count > 1 then		--heart net check failed
				LogError("wait_heart_count====",self.wait_heart_count)
				self.wait_heart_count = 0
				if self.login_netid > 0 then
					NetMgr:Disconnect(self.login_netid)
				end
				Runner.Instance:CancelRepeat(self.heart_beat_repeat_handle)
				self.heart_beat_repeat_handle = nil
			else
				self.wait_heart_count = self.wait_heart_count + 1
				local protocol = GetProtocol(CSLHeartBeat)
				SendProtocol(protocol)
			end
		end
	end
	if TimeHelper then
		if self.heart_beat_delay_handle ~= nil then
			TimeHelper:CancelTimer(self.heart_beat_delay_handle)
			self.heart_beat_delay_handle = nil
		end
		self.heart_beat_delay_handle = TimeHelper:AddDelayFrameTimer(heart_beat_func, 3)
	else
		heart_beat_func()
	end
end

function NetworkCtrl:__delete()
	self:UnRegisterProtocol(SCHeartbeatResp)
	NetMgr:SetDisconnectHandler(nil)
end

function NetworkCtrl:ClearHeartBeatCache()
	self.wait_heart_count = 0
	if self.heart_beat_repeat_handle ~= nil then
		Runner.Instance:CancelRepeat(self.heart_beat_repeat_handle)
		self.heart_beat_repeat_handle = nil
	end
	if self.heart_beat_delay_handle ~= nil then
		TimeHelper:CancelTimer(self.heart_beat_delay_handle)
		self.heart_beat_delay_handle = nil
	end
end

function NetworkCtrl:OnUnloadGameLogic()
	self:ClearHeartBeatCache()
end

---callback = function(error) end
function NetworkCtrl:ConnectLoginServer(callback)
	if self.login_netid > 0 then	---is connecting
		callback(nil)
		return
	end
	local login_data = LoginData.Instance
	local login_server_id = login_data:GetLastLoginServer()
	local login_account_spid = login_data:GetLastLoginAccountSpid()
	local login_server_info = login_data:GetServerInfoById(login_server_id, login_account_spid)
	Debuger.LogError("login_server_info: ip: " .. login_server_info.ip .. " port: " .. login_server_info.port)
	NetMgr:Connect(login_server_info.ip,login_server_info.port,function(suc,net_id)
			local error_str = nil
			if suc then
				self.login_netid = net_id
				self.wait_heart_count = 0;
				if self.heart_beat_repeat_handle ~= nil then
					Runner.Instance:CancelRepeat(self.heart_beat_repeat_handle)
				end
				if self.heart_beat_delay_handle ~= nil then
					TimeHelper:CancelTimer(self.heart_beat_delay_handle)
					self.heart_beat_delay_handle = nil
				end
				self.heart_beat_repeat_handle = Runner.Instance:RepeatCall(BindTool.Bind(self.OnHeartBeatCheck,self),HEART_BEAT_TIMEUP)
			else
				error_str = string.format("NetworkCtrl:ConnectLogicServer ERROR net_manager connect failed,ip=[%s],port=[%s]",tostring(login_server_info.ip),tostring(login_server_info.port))
			end
			callback(error_str)
		end)
end

---callback = function(error) end
function NetworkCtrl:ConnectLogicServer(callback)
	if self.logic_netid > 0 then
		callback(nil)
		return
	end
	if StringIsEmpty(self.logic_server_ip) then
		callback("NetworkCtrl:ConnectLogicServer ERROR---[logic server ip is nil]")
		return
	end
	local logic_ip = self.logic_server_ip
	local logic_port = self.logic_server_port
	NetMgr:Connect(logic_ip,logic_port,function(suc,net_id)
			local error_str = nil
			if suc then
				self.logic_netid = net_id
				self.wait_heart_count = 0;
				if self.heart_beat_repeat_handle ~= nil then
					Runner.Instance:CancelRepeat(self.heart_beat_repeat_handle)
				end
				if self.heart_beat_delay_handle ~= nil then
					TimeHelper:CancelTimer(self.heart_beat_delay_handle)
					self.heart_beat_delay_handle = nil
				end
				self.heart_beat_repeat_handle = Runner.Instance:RepeatCall(BindTool.Bind(self.OnHeartBeatCheck,self),HEART_BEAT_TIMEUP)
			else
				error_str = string.format("NetworkCtrl:ConnectLogicServer ERROR net_manager connect failed,ip=[%s],port=[%s]",tostring(logic_ip),tostring(logic_port))
			end
			callback(error_str)
		end)
end

function NetworkCtrl:ConnectCrossServer(callback)
	if self.cross_netid > 0 then
		callback(nil)
		return
	end
	if StringIsEmpty(self.cross_server_ip) then
		callback("NetworkCtrl:ConnectCrossServer ERROR---[cross server ip is nil]")
		return
	end
	local cross_ip = self.cross_server_ip
	local cross_port = self.cross_server_port
	NetMgr:Connect(cross_ip,cross_port,function(suc,net_id)
			local error_str = nil
			if suc then
				self.cross_netid = net_id
				self.wait_heart_count = 0;
				if self.heart_beat_repeat_handle ~= nil then
					Runner.Instance:CancelRepeat(self.heart_beat_repeat_handle)
				end
				if self.heart_beat_delay_handle ~= nil then
					TimeHelper:CancelTimer(self.heart_beat_delay_handle)
					self.heart_beat_delay_handle = nil
				end
				self.heart_beat_repeat_handle = Runner.Instance:RepeatCall(BindTool.Bind(self.OnHeartBeatCheck,self),HEART_BEAT_TIMEUP)
			else
				error_str = string.format("NetworkCtrl:ConnectCrossServer ERROR net_manager connect failed,ip=[%s],port=[%s]",tostring(cross_ip),tostring(cross_port))
			end
			callback(error_str)
		end)
end

function NetworkCtrl:DisconnectLoginServer()
	if self.login_netid > 0 then
		NetMgr:Disconnect(self.login_netid)
		self.login_netid = -1
	end
end

function NetworkCtrl:DisconnectLogicServer()
	if self.logic_netid > 0 then
		NetMgr:Disconnect(self.logic_netid)
		self.logic_netid = -1
	end
end
function NetworkCtrl:DisconnectCrossServer()
	if self.cross_netid > 0 then
		NetMgr:Disconnect(self.cross_netid)
		self.cross_netid = -1
	end
end
function NetworkCtrl:TestDisconnect()
	if self.logic_netid > 0 then
		NetMgr:Disconnect(self.logic_netid)
		-- self.logic_netid = -1
	end
	if self.cross_netid > 0 then
		NetMgr:Disconnect(self.cross_netid)
		-- self.cross_netid = -1
	end
	if self.login_netid > 0 then
		NetMgr:Disconnect(self.login_netid)
		-- self.login_netid = -1
	end
end
function NetworkCtrl:OnDisconnect(net_id)
				Debuger.LogError("OnDisconnect(net_id) = " .. tostring(net_id))
	if net_id == self.login_netid then
			Debuger.LogError("1")
		self.login_netid = -1
		LoginCtrl.Instance:OnAccidentDisconnect()
	elseif self.logic_netid == net_id then	--非主动断开
					Debuger.LogError("2")
		self.logic_netid = -1
		LoginCtrl.Instance:OnAccidentDisconnect()
	elseif self.cross_netid == net_id then
					Debuger.LogError("3")
		self.cross_netid = -1
		LoginCtrl.Instance:OnAccidentDisconnect()
	end
end

function NetworkCtrl:SendProtocol(protocol)
	local net_id = protocol.msg_type > 7000 and  protocol.msg_type < 7999 and self.login_netid or self.logic_netid
	net_id = net_id == -1 and self.cross_netid or net_id
	LogError("Sending protocol:", protocol, "to net_id:", net_id) -- 添加日志打印
	if net_id > 0 then
		protocol:EncodeAndSend(net_id)
	else
		Debuger.LogError("NetworkCtrl:SendProtocol faile net_id error,msg_type=%s",tostring(protocol.msg_type))
	end
end

function NetworkCtrl:LoginServerIsConnected()
	return self.login_netid > 0
end

function NetworkCtrl:LogicServerIsConnected()
	return self.logic_netid > 0 or self.cross_netid > 0
end

function NetworkCtrl:OnHeadbeat(protocol)
	-- if self.wait_heart_count > 0 then
	-- 	self.wait_heart_count = self.wait_heart_count - 1
	-- end
	self.wait_heart_count = 0
	if self.heart_beat_repeat_handle == nil then
		self.heart_beat_repeat_handle = Runner.Instance:RepeatCall(BindTool.Bind(self.OnHeartBeatCheck,self),HEART_BEAT_TIMEUP)
	end
end

function NetworkCtrl:OnLHeartBeat(protocol)
	-- if self.wait_heart_count > 0 then
	-- 	self.wait_heart_count = self.wait_heart_count - 1
	-- end
	self.wait_heart_count = 0
end

--收到任何包都清空心跳包计数
function NetworkCtrl:ClearHeartCount()
	self.wait_heart_count = 0
end

------------------------data--------------------------


function NetworkCtrl:SetLogicServerHost(ip,port)
	self.logic_server_ip = ip
	self.logic_server_port = port
end

function NetworkCtrl:SetCrossServerHost(ip,port)
	self.cross_server_ip = ip
	self.cross_server_port = port
end



-------------------------------------------------------------

function SendProtocol(protocol)
	NetworkCtrl.Instance:SendProtocol(protocol)
end

function GetProtocol(protocol_type)
	return ProtocolPool.Instance:GetProtocol(protocol_type)
end