VIEW_REQ("modules/common/base_view")


BaseCtrl = BaseCtrl or BaseClass()

function BaseCtrl:__init()
	self.data = nil
end

function BaseCtrl:__delete()

end

-- OnInit(在所有ctrl创建完回调,常用于SmartData KeysCare监听,有需要的ctrl添加)
-- function BaseCtrl:OnInit()
-- end

function BaseCtrl:Data()
	return self.data
end

function BaseCtrl:RegisterProtocol(protocol,func_name,fire_event)
	
	-- -- 注册到协议池
	local msg_type = ProtocolPool.Instance:Register(protocol)
	if msg_type < 0 then return end

	if func_name then
		if not self[func_name] then
			Debuger.LogError("BaseCtrl:RegisterProtocol error func not exist [%s]", func_name)
			return
		end

		local regist_func = nil
		if fire_event then
			regist_func = function(protocol)
					self[func_name](self,protocol)
					EventSys:Fire(EventProtocol.IdToEventKey(msg_type))
				end
		else
			regist_func = BindTool.Bind(self[func_name], self)
		end
		-- 注册协议处理函数
		NetMgr:ReigsterMsgReciever(msg_type, regist_func)
	end
end

function BaseCtrl:UnRegisterProtocol(protocol)
	local msg_type = ProtocolPool.Instance:GetMsgType(protocol)
	if msg_type > 0 then
		NetMgr:UnReigsterMsgReciever(msg_type)
	end
end

function BaseCtrl:OnUnloadGameLogic()
	
end

function BaseCtrl:OnReboot()
	
end