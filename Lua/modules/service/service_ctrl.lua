FILE_REQ("modules/service/service_data")
VIEW_REQ("modules/service/service_view")
VIEW_REQ("modules/service/service_player_info")
VIEW_REQ("modules/service/service_share")
ServiceCtrl = ServiceCtrl or BaseClass(BaseCtrl)

function ServiceCtrl:__init()
	if ServiceCtrl.Instance ~= nil then
		Debuger.LogError("[ServiceCtrl] attempt to create singleton twice!")
		return
	end	
	ServiceCtrl.Instance = self
	self.data = ServiceData.New()
	self.service_player_info_data = ServicePlayerInfoData.New()
	self:InitUrl()
	self:RegisterProtocol(SCNoticeClientNewMessageFromCustomService,"OnMsgCount")
end

function ServiceCtrl:__delete()
	self.data = nil
	self:UnRegisterProtocol(SCNoticeClientNewMessageFromCustomService)
	ServiceCtrl.Instance = nil
end

function ServiceCtrl:InitUrl()
	local gs_info_url = GLOBAL_CONFIG:Urls().gs_info_url
	if gs_info_url ~= nil then
		self.data:SetUrl(gs_info_url .. "?spid=" ..  (GLOBAL_CONFIG:Urls().spid or "") .. "&channle=" .. (CHILD_CHANNEL_ID or ""))
		if self.data:GetUrl() ~= nil then
			self:RequireUrlData()
		end
	end
end

function ServiceCtrl:RequireUrlData()
	local url = self.data:GetUrl()
	if url == nil then
		return
	end
	WebRequester:Request(url,function(data,error)
		if StringIsEmpty(error) == true then
			local url_data,suc = TryParseJson(data)
			if suc and url_data.data and url_data.data[1] then
				local img_info = TryParseJson(url_data.data[1].gs_img)
				url_data.data[1].img_info = img_info
				if img_info["1"] ~= nil and img_info["1"].url ~= nil and img_info["1"].url ~= "" then
					self:ToTexture(img_info["1"].url)
				end
				self.data:SetUrlData(url_data.data[1])
			end
		end
	end)
end

function ServiceCtrl:ToTexture(url)
	local tex = nil
    tex = UnityEngine.Texture2D.New(158,158,UnityEngine.TextureFormat.RGBA32,false)
    WebRequester:Request(url,function (data,error)
    	if StringIsEmpty(error) then
    		UnityEngine.ImageConversion.LoadImage(tex, data)
    		ServicePlayerInfoData.Instance:SetWeChatImg(tex)
    	else
    		LogError("客服界面解析php的微信二维码图片失败")
    	end
    end,WebRequestDataType.Bytes)
end

function ServiceCtrl:SendMsg(str)
	if str == nil or str == "" then
		PublicPopupCtrl.Instance:Center(Language.Service.SendTip)
		return
	end
	local function callback(str_f)
		local player_msg = ServiceMsgInfo.New()
		player_msg:SetPos(ServiceMsgInfo.POS.RIGHT)
		player_msg:SetMsg({
			[1] = {msg_type = ServiceMsgInfo.MSG_TYPE.TEXT,content = str_f},
		})
		local service_msg = self.data:GetMatchServiceMsg(str)
		self.data:AddMsg(player_msg)
		self.data:AddMsg(service_msg)
	end

	if IS_STANDALONE() and not StringIsEmpty(GLOBAL_CONFIG:Urls().chat_filter_url) then
		ChatSubCustom.PCChatFilter(str, 6,callback)
	else
		callback(ChatFilter.Instance:Filter(str))
	end

end

function ServiceCtrl:OnMsgCount(protocol)
	self.data:SetMsgCountData(protocol)
end