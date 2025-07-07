NetManager = NetManager or BaseClass()
local unityNetMgr = Game.NetworkManager.Singleton()

function NetManager:__init()
	if NetMgr ~= nil then
		Debuger.LogError("[NetManager] attempt to create singleton twice!")
		return
	end
	NetMgr = self	

	ProtocolPool.New()

	self.wait_connect = {}
	self.connecting = {}

	self.msg_recievers = {}

	unityNetMgr:setConnectHandler(BindTool.Bind(self.OnConnect,self))
	unityNetMgr:setRecvHandler(BindTool.Bind(self.OnRecv,self))

end

function NetManager:__delete()
	unityNetMgr:setConnectHandler(nil)
	unityNetMgr:setRecvHandler(nil)
	unityNetMgr:setDisconnectHandler(nil)

	for i,v in ipairs(self.connecting) do
		self:Disconnect(v)
	end
	self.connecting = nil
	self.msg_recievers = nil

	ProtocolPool.Instance:DeleteMe()

	if NetMgr == self then
		NetMgr = nil
	end
end


function NetManager:Connect(ip,port,callback,timeout)
	-- timeout = 5000
	local con_handle
	if timeout == nil then
		con_handle = unityNetMgr:connectAsyn(ip,port)
	else
		con_handle = unityNetMgr:connectAsyn(ip,port,timeout)
	end
	if callback ~= nil then
		self.wait_connect[con_handle] = callback
	end
end

function NetManager:OnConnect(suc,con_handle,net_id)
	if suc then
		table.insert(self.connecting,net_id)
	end
	local ck = self.wait_connect[con_handle]
	if ck ~= nil then
		self.wait_connect[con_handle] = nil
		ck(suc,net_id)
	end
end

function NetManager:Disconnect(net_id)
	table.remove(self.connecting,net_id)
	unityNetMgr:disconnect(net_id)
end

-- function NetManager:OnDisconnect(net_id)
	
-- end

function NetManager:SetDisconnectHandler(func)
	unityNetMgr:setDisconnectHandler(func)
end

function NetManager:OnRecv(msg_type,net_id)
	if NetworkCtrl.Instance then 
		NetworkCtrl.Instance:ClearHeartCount()
	end
	local recieve_func = self.msg_recievers[msg_type]

	if recieve_func then
		local protocol = ProtocolPool.Instance:GetProtocolByType(msg_type)
		if protocol then
			protocol:Decode()
			recieve_func(protocol)
		else
			Debuger.Log("Undefined protocol:[%d]" ,msg_type)
		end
	else
		Debuger.Log("protocol:%d Has no reciever" ,msg_type)
	end
end

function NetManager:ReigsterMsgReciever(msg_type,recieve_func)
	self.msg_recievers[msg_type] = recieve_func
end

function NetManager:UnReigsterMsgReciever(msg_type)
	self.msg_recievers[msg_type] = nil
end
