VIEW_REQ("modules/reward/reward_super_vip_view")

SuperVipCtrl = SuperVipCtrl or BaseClass(BaseCtrl)

function SuperVipCtrl:__init()
	if SuperVipCtrl.Instance ~= nil then
		Debuger.LogError("[SuperVipCtrl] attempt to create singleton twice!")
		return
	end	
	SuperVipCtrl.Instance = self
	self.data = SuperVipData.New()
    self:RegisterAllProtocols()
end

function SuperVipCtrl:__delete()
	self:UnRegisterAllProtocols()
	SuperVipCtrl.Instance = nil
	Delete(self[[data]])
end

function SuperVipCtrl:OnUnloadGameLogic()
	self.data:ClearData()
end

function SuperVipCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSClientCustomSeq)
	self:RegisterProtocol(SCClientCustomInfo,"OnClientCustomInfo")
    EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.OnRoleNoticeComplete, self))
end

function SuperVipCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(CSClientCustomSeq)
	self:UnRegisterProtocol(SCClientCustomInfo)
end

function SuperVipCtrl:OnRoleNoticeComplete()
    self:SendClientCustomSeq()
    self:SendRequestReq()
end

function SuperVipCtrl:SendClientCustomSeq(op_type,param1,param2)
	local protocol = GetProtocol(CSClientCustomSeq)
	protocol.op_type = op_type or 0
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

function SuperVipCtrl:OnClientCustomInfo(protocol)
	self.data:SetClientCustomInfo(protocol)
end

--发送PHP请求全部信息
function SuperVipCtrl:SendRequestReq()
    if StringIsEmpty(self.data.url_format) then
        return
    end
    local url = Format(self.data.url_format,GLOBAL_CONFIG:PkgInfo().agent_id,LoginData.Instance:GetLastLoginServer())
    WebRequester:Request(url, function(data, error)
        if StringIsEmpty(error) == true then
            local remote_config, succ = TryParseJson(data)
            if succ == true then
                self.data:SetRequestInfo(remote_config)
                -- LogError("超级vip接收PHP全部信息",RechargeData.Instance:GetRechargeNum(),url,remote_config)
            end
        end
    end)
end

--发送PHP下载对应后台图片的请求
function SuperVipCtrl:SendWeChatRequestReq(config)
    local remote_config, succ = TryParseJson(config)
    local url = remote_config["1"].url
	local tex = UnityEngine.Texture2D.New(158,158,UnityEngine.TextureFormat.RGBA32,false)
    WebRequester:Request(url,function (data,error)
    	if StringIsEmpty(error) then
    		UnityEngine.ImageConversion.LoadImage(tex, data)
            self.data:WeChatImg(tex)
    	end
    end,WebRequestDataType.Bytes)
end

function SuperVipCtrl:SendCodeRequestReq(config)
    local remote_config, succ = TryParseJson(config)
    local url = remote_config["1"].url
	local tex = UnityEngine.Texture2D.New(158,158,UnityEngine.TextureFormat.RGBA32,false)
    WebRequester:Request(url,function (data,error)
    	if StringIsEmpty(error) then
    		UnityEngine.ImageConversion.LoadImage(tex, data)
            self.data:CodeImg(tex,remote_config["1"].name)
    	end
    end,WebRequestDataType.Bytes)
end

--===========================SuperVipData===========================
SuperVipData = SuperVipData or BaseClass()
function SuperVipData:__init()
    self.load_data = SmartData.New({})
    self.red_piont_data = SmartData.New()
    self.info_data = SmartData.New({info = {},server_day_num = 0,today_recharge_num = 0,val = false})
    if not StringIsEmpty(GLOBAL_CONFIG:Urls().qq_img_cfg_url) then
        self.url_format = GLOBAL_CONFIG:Urls().qq_img_cfg_url .."?plat_id=%s&server_id=%s"
    end
end

function SuperVipData:ClearData()
    self.info_data.server_day_num = 0
    self.info_data.today_recharge_num = 0
end

function SuperVipData:SetClientCustomInfo(protocol)
    self.info_data.server_day_num = protocol.param1
    self.info_data.today_recharge_num = protocol.param2
    self.info_data.val = not self.info_data.val
end

function SuperVipData:SetRequestInfo(info)
    self.info_data.info = info or {}
    self.red_piont_data:Notify()
end

function SuperVipData:GetRequestInfo()
    return self.info_data.info
end

function SuperVipData:GetTodayRechargeNum()
    return self.info_data.today_recharge_num
end

function SuperVipData:GetServerDayNum()
    return self.info_data.server_day_num
end

function SuperVipData:WeChatImg(we_chat_img)
    if we_chat_img then
        self.load_data.we_chat_img = we_chat_img
        self.load_data:Notify()
    else
        return self.load_data.we_chat_img
    end
end

function SuperVipData:CodeImg(code_img,file_name)
    if code_img then
        self.load_data.file_name = file_name
        self.load_data.code_img = code_img
        self.load_data:Notify()
    else
        return self.load_data.code_img
    end
end

--解析列表
function SuperVipData:GetParseList(config)
    local item_list,reward_list = {},{}
    for i=1,4 do
        if config["privilege"..i] then
            table.insert(item_list,{index = i,title = Language.SuperVip.Title[i],desc = config["privilege"..i]})
        end
    end
    for i=1,5 do
        if config["item_id"..i] then
            table.insert(reward_list,{item_id = tonumber(config["item_id"..i]),num = tonumber(config["item_num"..i]),is_bind = tonumber(config["item_is_bind"..i])})
        end
    end
    return item_list,reward_list
end

--SVip解锁条件
function SuperVipData:GetSuperVipEnd()
    if IS_AUDIT_VERSION then
        return false
    end
    local data = self:GetRequestInfo().data
    if data ~= nil then
        return math.floor(RechargeData.Instance:GetRechargeNum()/10) >= (tonumber(data.gold_interface_activation) or 0)
    end
    return false
end

--SVIP红点
function SuperVipData:SetRemindNum()
    if self:GetServerDayNum() == 0 then
        SuperVipCtrl.Instance:SendClientCustomSeq(1,TimeCtrl.Instance:GetCurOpenServerDay(),
        RechargeData.Instance:GetTodayRechargeNum())
        SuperVipCtrl.Instance:SendClientCustomSeq()
        self.red_piont_data:Notify()
    end
end

function SuperVipData:GetRemindNum()
    return self:GetServerDayNum() == 0 and 1 or 0
end