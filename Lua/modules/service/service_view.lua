ServiceView = ServiceView or DeclareView("ServiceView","service_view/ServiceView",0,ViewLayer.Top - 2)

function ServiceView:ServiceView()
	self.Board:SetData(ServiceView,Language.Service.Title)
	self.data = ServiceData.Instance
	self.index_flag = false
end

function ServiceView:LoadCallback(param_t)
	local tabs_flag = self.data:GetTabsFlag()
	for i = 1,#tabs_flag do
		local tab = tabs_flag[i]
		self[tab.k].gameObject:SetActive(tab.v > 0)
		if not self.index_flag then
			if tab.v > 0 then
				self[tab.k].isOn = true
				self.index_flag = true
			end
		end
	end
end

function ServiceView:CloseCallback()
	-- body
end

ServicePanel = ServicePanel or DeclareMono("ServicePanel",UIWFlushPanel)
function ServicePanel:ServicePanel()
	self.data = ServiceData.Instance
	self.data_cares = {
		{data = self.data.service_msg_data,func = self.FlushPanel ,init_call = false},
		{data = self.data.msg_count_data,func = self.FlushPanel ,init_call = false}
	}
end

function ServicePanel:Awake()
	UIWFlushPanel.Awake(self)
	if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.BtnServiceObj, Vector3.New(50, 22))
    end
end

function ServicePanel:OnEnable()
	UIWFlushPanel.OnEnable(self)
	self:FlushPanel()
end

function ServicePanel:FlushPanel()
	self.red_point:SetNum(self.data:GetMsgCount())
	local msg_list = self.data:GetMsgList()
	self.MsgList:SetDataList(msg_list, #msg_list)
	self.MsgList:JumpToDown()
end

function ServicePanel:OnSendMsg()
	ServiceCtrl.Instance:SendMsg(self.InputMsg.text)
	self.InputMsg.text = ""
end

function ServicePanel:OnContactServiceClick()
	if ChannelAgent and ChannelAgent.ConnectPlatGM then
		self.data.msg_count_data.msg_count = 0
		ChannelAgent.ConnectPlatGM()
	end
end

function ServicePanel:OnDestroy()
	if self.red_point ~= nil then
		self.red_point:DeleteMe()
		self.red_point = nil
	end
	UIWFlushPanel.OnDestroy(self)
end

--===== 左边的消息
ServiceLeftMsg = ServiceLeftMsg or DeclareMono("ServiceLeftMsg")

function ServiceLeftMsg:ServiceLeftMsg()
	self.default_height = ServiceMsgInfo.DEFAULT_HEIGHT
	self.height = self.default_height
	local text_msg_data = {
		msg_pool = {},		
		pool_index = 1, 		-- 记录消息池该取哪个位置
		obj = self.TextMsgObj,  -- 被克隆对象
	}
	local btn_msg_data = {
		msg_pool = {},
		pool_index = 1,
		obj = self.BtnMsgObj
	}
	self.msg_pool = {
		[ServiceMsgInfo.MSG_TYPE.TEXT] = text_msg_data,
		[ServiceMsgInfo.MSG_TYPE.BTN] = btn_msg_data,
	}
end

function ServiceLeftMsg:SetData(data)
	if data == self.data then
		return
	end
	self.data = data
	for i=1,#data do
		local msg_data = data[i]
		self:HandleMsg(msg_data)
	end
	self:HandleHeight()
end

function ServiceLeftMsg:HandleMsg(msg_data)
	local pool_data = self.msg_pool[msg_data.msg_type]
	local pool = pool_data.msg_pool
	local pool_index = pool_data.pool_index
	if pool[pool_index] == nil then
		local obj = UnityGameObj.Instantiate(pool_data.obj,self.ContentTrans)
		local com_text = obj:GetComponent(typeof(UnityUI.Text))
		local com_trans = obj:GetComponent(typeof(UnityEngine.RectTransform))
		pool[pool_index] = {obj = obj,com_text = com_text,com_trans = com_trans}
	end
	pool[pool_index].obj:SetActive(true)
	UH.SetText(pool[pool_index].com_text,msg_data.content)
	if msg_data.msg_type == ServiceMsgInfo.MSG_TYPE.TEXT then
		pool[pool_index].com_trans:SetSizeWithCurrentAnchors(1,pool[pool_index].com_text.preferredHeight)
	end
	pool_data.pool_index = pool_data.pool_index + 1
end

function ServiceLeftMsg:HandleHeight()
	local content_spacing = self.ContentLayout.spacing
	local content_top = self.ContentLayout.padding.top
	local content_bottom = self.ContentLayout.padding.bottom
	self.height = content_top + content_bottom
	for i,v in pairs(self.msg_pool) do
		for j,va in pairs(v.msg_pool) do
			if va.obj.activeSelf == true then
				if i == ServiceMsgInfo.MSG_TYPE.TEXT then
					self.height = self.height + va.com_text.preferredHeight + content_spacing
				end
				if i == ServiceMsgInfo.MSG_TYPE.BTN then
					self.height = self.height + 30 + content_spacing   -- 按钮类型的文本默认高为30
				end
			end
		end
	end
	self.ContentTrans:SetSizeWithCurrentAnchors(1, self.height)
	self.height = self.height < self.default_height and self.default_height or self.height
	self.ItemTrans:SetSizeWithCurrentAnchors(1,self.height)
end

function ServiceLeftMsg:GetMsgHeight()
	return self.height
end

function ServiceLeftMsg:OnBtnMsgClick(obj)
	local com_text = obj:GetComponent(typeof(UnityUI.Text))
	local service_msg = ServiceMsgInfo.New()
	service_msg:SetPos(ServiceMsgInfo.POS.LEFT)
	local msg_cfg = Cfg.ServiceRobotReply(com_text.text)
	service_msg:SetMsg({[1] = {msg_type = ServiceMsgInfo.MSG_TYPE.TEXT,content = msg_cfg.reply}})
	ServiceData.Instance:AddMsg(service_msg)
end

function ServiceLeftMsg:OnDisable()
	for i,v in pairs(self.msg_pool) do
		for j,va in pairs(v.msg_pool) do
			if va.obj.activeSelf == true then
				va.obj:SetActive(false)
			end
		end
		v.pool_index = 1
	end
	self.data = nil
end

function ServiceLeftMsg:OnDestroy()
	self.msg_pool = nil
end


--===== 右边的消息
ServiceRightMsg = ServiceRightMsg or DeclareMono("ServiceRightMsg")
function ServiceRightMsg:ServiceRightMsg()
	self.default_height = ServiceMsgInfo.DEFAULT_HEIGHT
	self.height = self.default_height
end

function ServiceRightMsg:SetData(data)
	UH.SetText(self.Msg,data[1].content)
	local msg_height = self.Msg.preferredHeight
	self.height = self.ContentLayout.padding.top + self.ContentLayout.padding.bottom + msg_height
	self.ItemTrans:SetSizeWithCurrentAnchors(1,self.height < self.default_height and self.default_height or self.height)
	self.ContentTrans:SetSizeWithCurrentAnchors(1,self.height)
	local server_info = ServicePlayerInfoData.Instance:GetCurServerInfo()
	if server_info then
		local platform_name = LoginCtrl.Instance.data:GetPlatformName()
		local avatar_id = UnityPlayerPrefs.GetInt(server_info.id.. platform_name .. "avatar_id")
		local avatar = UnityPlayerPrefs.GetInt(server_info.id.. platform_name .. "avatar_type")
		local avatar_quality = UnityPlayerPrefs.GetInt(server_info.id.. platform_name .. "avatar_quality")
		self.Avater:SetAvatar(avatar,avatar_id,avatar_quality)
	end
end

function ServiceRightMsg:GetMsgHeight()
	return self.height < self.default_height and self.default_height or self.height
end

--========================    ServiceMsgItem   ======================
ServiceMsgItem = ServiceMsgItem or DeclareMono("ServiceMsgItem",UIWidgetBaseItem)
function ServiceMsgItem:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	self.LMsgItemObj:SetActive(data:GetPos() == ServiceMsgInfo.POS.LEFT)
	self.RMsgItemObj:SetActive(data:GetPos() == ServiceMsgInfo.POS.RIGHT)
	if data:GetPos() == ServiceMsgInfo.POS.LEFT then
		self.LMsgItemMono:SetData(data:GetMsg())
		data:SetHeight(self.LMsgItemMono:GetMsgHeight())
	else
		self.RMsgItemMono:SetData(data:GetMsg())
		data:SetHeight(self.RMsgItemMono:GetMsgHeight())
	end
end

function ServiceMsgItem:ExpendSize(set_data,axis)
	return set_data:GetHeight()
end