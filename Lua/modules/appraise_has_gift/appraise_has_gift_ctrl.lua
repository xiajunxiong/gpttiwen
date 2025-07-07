VIEW_REQ("modules/appraise_has_gift/appraise_has_gift_view")
-- 评价有礼
AppraiseHasGiftCtrl = AppraiseHasGiftCtrl or BaseClass(BaseCtrl)

function AppraiseHasGiftCtrl:__init()
    if AppraiseHasGiftCtrl.Instance ~= nil then
        Debuger.LogError("[AppraiseHasGiftCtrl] attempt to create singleton twice!")
        return
    end
    AppraiseHasGiftCtrl.Instance = self
    self.data = AppraiseHasGiftData.New()
end

function AppraiseHasGiftCtrl:OnInit()
    self:RegisterRemind()
    self:RegisterAllProtocol()
    self.login_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete, BindTool.Bind(self.RequestShopInfo, self))
    self.level_handle = EventSys:Bind(GlobalEventKeys.RoleLevelChange, BindTool.Bind(self.NotifyData, self))
end

function AppraiseHasGiftCtrl:RegisterRemind()
    
end

function AppraiseHasGiftCtrl:RegisterAllProtocol()
    self:RegisterProtocol(CSFetchPingJiaRewardReq)
    self:RegisterProtocol(SCFetchPingJiaRewardInfo, "OnSCFetchPingJiaRewardInfo")
end

function AppraiseHasGiftCtrl:__delete()
    if AppraiseHasGiftCtrl.Instance == self then
        AppraiseHasGiftCtrl.Instance = nil
    end
    EventSys:UnBind(self.login_handle)
    EventSys:UnBind(self.level_handle)
end

function AppraiseHasGiftCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

-- 发送请求
function AppraiseHasGiftCtrl:SendReq()
    local protocol = ProtocolPool.Instance:GetProtocol(CSFetchPingJiaRewardReq)
    SendProtocol(protocol)
end

function AppraiseHasGiftCtrl:OnSCFetchPingJiaRewardInfo(protocol)
    self.data:SetFlag(protocol)
end

-- 请求评价有礼信息
function AppraiseHasGiftCtrl:RequestShopInfo()
    local url = GLOBAL_CONFIG:Urls().shop_info or "http://cls.20051.kya.cn/api/c2s/fetch_shop_url.php"
    if StringIsEmpty(url) or not StringIsEmpty(self.data:URL()) then
        return
    end
    url = url .. "?spid=" .. LoginData.Instance:GetAccountSpid()
    WebRequester:Request(url, function(data, error)
        if StringIsEmpty(error) == true then
            local remote_config, suc = TryParseJson(data)
            if suc == true and remote_config.data ~= cjson.null and remote_config.ret == 0 then
                self.data:URL(remote_config.data[LoginData.Instance:AccountData().spid] or "")
            end
        end
    end)
end

function AppraiseHasGiftCtrl:NotifyData()
    self.data:InfoDataNotify()
end


-----------------------AppraiseHasGiftData-----------------------
AppraiseHasGiftData = AppraiseHasGiftData or BaseClass()

function AppraiseHasGiftData:__init()
    if AppraiseHasGiftData.Instance ~= nil then
        Debuger.LogError("[AppraiseHasGiftData] attempt to create singleton twice!")
        return
    end
    AppraiseHasGiftData.Instance = self
    self.config = Config.sundries_auto.other[1]
    self.info_data = SmartData.New()
    self:ResetData()
    self.info_data.url = ""
end

function AppraiseHasGiftData:__delete()
    AppraiseHasGiftData.Instance = nil
end

function AppraiseHasGiftData:ResetData()
    self.info_data.flag = 1
end

function AppraiseHasGiftData:InfoData()
    return self.info_data
end

function AppraiseHasGiftData:InfoDataNotify()
    self.info_data:Notify()
end

function AppraiseHasGiftData:SetFlag(protocol)
    self.info_data.flag = protocol.flag
    self.info_data:Notify()
end

function AppraiseHasGiftData:IsReceive()
    return self.info_data.flag == 1
end

function AppraiseHasGiftData:URL(value)
    if value then
        self.info_data.url = value
        self.info_data:Notify()
    else
        return self.info_data.url
    end
end

function AppraiseHasGiftData:OpenLevel()
    return self.config.level
end

function AppraiseHasGiftData:Reward()
    return self.config.reward_item
end

function AppraiseHasGiftData:IsShow()
    if RoleLevel() >= self:OpenLevel() then
        if not self:IsReceive() then
            if not StringIsEmpty(self:URL()) then
                return true
            end
        end
    end
    return false
end