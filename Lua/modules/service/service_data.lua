ServiceMsgInfo = ServiceData or BaseClass()

ServiceMsgInfo.POS = {
	LEFT = 1,
	RIGHT = 2,
}
ServiceMsgInfo.MSG_TYPE = {
	TEXT = 1,  		-- 文本消息
	BTN = 2,  		-- 按钮消息
}
ServiceMsgInfo.DEFAULT_HEIGHT = 70    --默认消息元素高度70 === 头像高度

ServiceMsgInfo.Index = {
	"wx_id",
	"gamenet",
	"qq",
	"qq_users",
	"tieba",
	"weibo",
	"wx_kf",
}

function ServiceMsgInfo:__init()
	self.msg_pos = nil
	self.msg = {}
	self.msg_height = ServiceMsgInfo.DEFAULT_HEIGHT    
end

-- pos:ServiceMsgInfo.POS
function ServiceMsgInfo:SetPos(pos)
	self.msg_pos = pos
end

function ServiceMsgInfo:GetPos()
	return self.msg_pos
end

-- msg_t:{msg[]:{msg_type:ServiceMsgInfo.MSG_TYPE,content:string}}
function ServiceMsgInfo:SetMsg(msg_t)
	self.msg = msg_t
end

function ServiceMsgInfo:GetMsg()
	return self.msg
end

function ServiceMsgInfo:SetHeight(he)
	self.msg_height = he
end

function ServiceMsgInfo:GetHeight()
	return self.msg_height
end

ServiceData = ServiceData or BaseClass()

function ServiceData:__init()
	if ServiceData.Instance ~= nil then
		Debuger.LogError("[ServiceData] attempt to create singleton twice!")
		return
	end	
	ServiceData.Instance = self
	self.service_msg_data = SmartData.New({index = 0,msg_list = {}})
	self.url = nil
	self.url_data = SmartData.New()
	self.robot_reply_cfg = Config.agent_adapt_auto.robot_reply
	self:InitData()
	self.tabs_flag = {
		[1] = {k = "kf_flag", v = 0},
		[2] = {k = "info_flag", v = 0},
		[3] = {k = "share_flag", v = 0},
	}
	self.msg_count_data = SmartData.New({msg_count = 0})
end

function ServiceData:__delete()
	ServiceData.Instance = nil
	self.service_msg_list:Set({})
	self.service_msg_list = nil
	self.url_data:Set({})
	self.url_data = nil
end

function ServiceData:InitData()
	Cfg.Register("ServiceRobotReply",Cfg.RealTimeSearch(self.robot_reply_cfg,"question"))
	local first_msg = ServiceMsgInfo.New()
	first_msg:SetPos(ServiceMsgInfo.POS.LEFT)
	first_msg:SetMsg({
		[1] = {msg_type = ServiceMsgInfo.MSG_TYPE.TEXT,content = Language.Service.FirstMsg1},
		[2] = {msg_type = ServiceMsgInfo.MSG_TYPE.BTN,content = self.robot_reply_cfg[1].question},
		[3] = {msg_type = ServiceMsgInfo.MSG_TYPE.BTN,content = self.robot_reply_cfg[2].question},
		[4] = {msg_type = ServiceMsgInfo.MSG_TYPE.BTN,content = self.robot_reply_cfg[3].question},
		[5] = {msg_type = ServiceMsgInfo.MSG_TYPE.TEXT,content = Language.Service.FirstMsg2},
	})
	self:AddMsg(first_msg) 
end

function ServiceData:SetMsgCountData(protocol)
	self.msg_count_data.msg_count = protocol.msg_count
end

function ServiceData:GetMsgCount()
	return self.msg_count_data.msg_count
end

-- 是否开启客服功能
function ServiceData:IsOpenService()
	if IS_AUDIT_VERSION then
		return false
	end
	for _,v in pairs(self.tabs_flag) do
		if v.v > 0 then
			return true
		end
	end
	return false
end

function ServiceData:GetTabsFlag()
	return self.tabs_flag
end

function ServiceData:SetUrl(url)
	self.url = url
end

function ServiceData:GetUrl()
	return self.url
end

function ServiceData:SetUrlData(data)
	for i,v in pairs(self.tabs_flag) do
		self.tabs_flag[i].v = tonumber(data[v.k])
		--self.tabs_flag[i].v = 1
	end
	local check_data = {}
	for i,v in ipairs(ServiceMsgInfo.Index) do
		if not StringIsEmpty(data[v]) then
			table.insert(check_data, {value = data[v], index = i})
			if #check_data >= 3 then
				break
			end
		end
	end
	-- LogError("check_data",check_data)
	self.url_data:Set(check_data)
end

function ServiceData:GetUrlData()
	return self.url_data
end

function ServiceData:AddMsg(msg)
	table.insert(self.service_msg_data.msg_list,msg)
	self.service_msg_data.index = self.service_msg_data.index + 1
end

function ServiceData:GetMsgList()
	return self.service_msg_data.msg_list
end

--获取匹配机器人的消息
function ServiceData:GetMatchServiceMsg(str)
	local char_list = RichTextHelper.ParseUTF8Str(str)
	local service_msg = ServiceMsgInfo.New()
	service_msg:SetPos(ServiceMsgInfo.POS.LEFT)
	local msg_list = {}
	-- 这个没办法，只能老老实实的遍历判断了
	for i,v in pairs(self.robot_reply_cfg) do
		-- 两个相邻的字进行匹配
		for j = 1,#char_list - 1 do
			local two_char = string.format("%s%s",char_list[j],char_list[j+1])
			if string.find(v.question,two_char) ~= nil then
				table.insert(msg_list,v)
				break
			end
		end
	end
	local len = #msg_list
	local show_msg = {}
	if len == 0 then
		local msg = {msg_type = ServiceMsgInfo.MSG_TYPE.TEXT,content = Language.Service.Msg3}
		table.insert(show_msg,msg)
	elseif len == 1 then
		local msg = {msg_type = ServiceMsgInfo.MSG_TYPE.TEXT,content = msg_list[1].reply}
		table.insert(show_msg,msg)
	else
		local msg = {msg_type = ServiceMsgInfo.MSG_TYPE.TEXT,content = Language.Service.Msg4}
		table.insert(show_msg,msg)
		for i,v in pairs(msg_list) do
			local msg = {msg_type = ServiceMsgInfo.MSG_TYPE.BTN,content = msg_list[i].question}
			table.insert(show_msg,msg)
		end
	end
	service_msg:SetMsg(show_msg)
	return service_msg
end

-- ================= ServicePlayerInfoData ===================

ServicePlayerInfoData = ServicePlayerInfoData or BaseClass()
function ServicePlayerInfoData:__init()
	if ServicePlayerInfoData.Instance ~= nil then
		Debuger.LogError("[ServicePlayerInfoData] attempt to create singleton twice!")
		return
	end	
	ServicePlayerInfoData.Instance = self
end

function ServicePlayerInfoData:__delete()
	ServicePlayerInfoData.Instance = nil
end

-- 登录过的所有服务器
function ServicePlayerInfoData:GetLoginedAllServer()
	return LoginData.Instance:GetServerData()[1]
end

function ServicePlayerInfoData:SetCurServerInfo(server_info)
	self.cur_server_info = server_info
end

--获取当前登录过的服务器信息
function ServicePlayerInfoData:GetCurServerInfo()
	local base_data = RoleData.Instance:GetBaseData()
	if base_data.role_id > 0 then
		local info = {}
		info.level = base_data.level
		info.avatar_id = RoleData.Instance:GetRoleAvatarId()
		info.avatar_quality = RoleData.Instance:GetRoleAvatarQuality()
		info.role_name = base_data.name
		info.role_id = base_data.role_id
		--此id为区服id
		info.id = LoginData.Instance:GetLastLoginServer()
		info.name = LoginData.Instance:ServerName()
		self.cur_server_info = info
	else
		local role_list = LoginData.Instance:GetLoginRoles()
		local cur_login_role_idnex = LoginData.Instance:GetLastLoginIndex() + 1
		local info = role_list[cur_login_role_idnex]
		--此id为 role_id
		if info.id < 0 then
			return nil
		else
			self.cur_server_info = self:GetServerInfoByRoleId(info.id)
		end
	end
	return self.cur_server_info
end

function ServicePlayerInfoData:GetServerInfoByRoleId(role_id)
	for _,v in pairs(self:GetLoginedAllServer()) do
		if v.role_id == role_id then
			return v
		end
	end
end

function ServicePlayerInfoData:SetWeChatImg(we_chat_img)
	self.we_chat_img = we_chat_img
end

function ServicePlayerInfoData:GetWeChatImg()
	return self.we_chat_img
end