ActWordGiftCtrl = ActWordGiftCtrl or BaseClass(BaseCtrl)
VIEW_REQ("modules/serveractivity/openserver/act_word_gift_view")

function ActWordGiftCtrl:__init()
	if ActWordGiftCtrl.Instance ~= nil then
		Debuger.LogError("[ActWordGiftCtrl] attempt to create singleton twice!")
		return
	end
    ActWordGiftCtrl.Instance = self
    self.data = ActWordGiftData.New()
    self:RegisterAllProtocols()
end

function ActWordGiftCtrl:__delete()
    self:UnRegisterAllProtocols()
end

function ActWordGiftCtrl:RegisterAllProtocols()
	-- self:RegisterProtocol(CSRandActivityOperaReq)
	self:RegisterProtocol(SCRaCollectWordInfo,"OnRaCollectWordInfo")
end

function ActWordGiftCtrl:UnRegisterAllProtocols()
	-- self:UnRegisterProtocol(CSRandActivityOperaReq)
	-- self:UnRegisterProtocol(SCRandActivityTimeLimitStoreInfo)
end
function ActWordGiftCtrl:OnRaCollectWordInfo(protocol)
    self.data:DailyReward(protocol.day_gift_is_fetch)
    self.data:WordInfo(protocol.word_info)
    self.data:TotalInfo(protocol.total_info)
end
function ActWordGiftCtrl:Info()
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.KF_WORD_GIFT
    param_t.opera_type = 0
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end
function ActWordGiftCtrl:Get(seq)
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.KF_WORD_GIFT
    param_t.opera_type = 1
    param_t.param_1 = seq
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end
function ActWordGiftCtrl:Daily()
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.KF_WORD_GIFT
    param_t.opera_type = 2
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
end

ActWordGiftData = ActWordGiftData or BaseClass()

function ActWordGiftData:__init()
	if ActWordGiftData.Instance ~= nil then
		Debuger.LogError("[ActWordGiftData] attempt to create singleton twice!")
		return
	end
    ActWordGiftData.Instance = self
    self.word_info = SmartData.New()
    self.total_info = SmartData.New()
    self.word_redinfo = SmartData.New({red = true, daily = 0})
end

function ActWordGiftData:__delete()

end
function ActWordGiftData:WordInfo(info)
    if info then
        self.word_info:Set(info)
    else
        return self.word_info
    end
end
function ActWordGiftData:TotalInfo(info)
    if info then
        self.total_info:Set(info)
    else
        return self.total_info
    end
end
function ActWordGiftData:WordCount(seq)
    return self.word_info[seq] or 0
end
function ActWordGiftData:TotalCount(seq)
    return self.total_info[seq] or 0
end
function ActWordGiftData:GetActCfg()
    return Config.word_gift_activity_auto.word_gift
end
function ActWordGiftData:DailyReward(flag)
    self.word_redinfo.daily = flag
end
function ActWordGiftData:GetWordGiftRemind()
    local cfg = self:GetActCfg()
    local num = 0
    for _, value in pairs(cfg) do
       num = num + self:GetWordGiftRemind2(value)
    end
    return num
end
function ActWordGiftData:GetWordGiftRemind2(value, ignore)
    local item
    local num = 1
    local use = self:WordCount(value.type + 1)
    if value.number ~= 0 then
        if value.number - use <= 0 then
            return 0, 0
        end
    else
        use = self:TotalCount(value.type + 1)
        if value.total_exchange - use <= 0 then
            return 0, 0
        end
    end
    for i = 1, 5  do
        if value["words"..i] ~= 0 then
           item = Item.Create({item_id = value["words"..i]})
           if item:Num() < value["number"..i] then
                num = 0
           end
        end
    end
    if not ignore and self.word_redinfo.daily == 0 then
        return 1, 1
    end
    local flag = UnityPlayerPrefs.GetInt(PrefKeys.ActReward(ACTIVITY_TYPE.KF_WORD_GIFT))
    if flag == TimeCtrl.Instance:GetCurOpenServerDay() then
        self.word_redinfo.red = false
        return 0, num
    end
    self.word_redinfo.red = true
    return num, num
end