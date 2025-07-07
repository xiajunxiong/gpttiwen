VIEW_REQ("modules/serveractivity/tiger_year/tiger_born/act_tiger_born_view")

ActTigerBornCtrl = ActTigerBornCtrl or BaseClass(BaseCtrl)
ActTigerBornCtrl.ReqType = {
    Info = 0,   --请求信息
    Buy = 1,    --直购
    Get = 2,    --领取奖励，p1 = seq
}
function ActTigerBornCtrl:__init()
    if ActTigerBornCtrl.Instance ~= nil then
		Debuger.LogError("[ActTigerBornCtrl] attempt to create singleton twice!")
		return
	end
    ActTigerBornCtrl.Instance = self
    self.data = ActTigerBornData.New()
    self.data = ActTigerBornData.Instance

    self:RegisterProtocol(SCTigerBornInfo, "OnSCTigerBornInfo")
    
    -- 红点注册
    Remind.Instance:Register(Mod.TigerYear.TigerBorn,self.data.act_info, function ()
		return self.data:RemindNum() 
    end)
end

function ActTigerBornCtrl:OnSCTigerBornInfo(protocol)
    self.data.act_info.info = protocol.info
end

ActTigerBornData = ActTigerBornData or BaseClass()
function ActTigerBornData:__init()
    if ActTigerBornData.Instance ~= nil then
		Debuger.LogError("[ActTigerBornData] attempt to create singleton twice!")
		return
	end
    ActTigerBornData.Instance = self
    self.cfg = Config.yinhuwenshi_auto
    self.act_type = ACTIVITY_TYPE.RAND_TIGER_BORN
    self.act_info = SmartData.New({info = {}})
end

-- 获取购买配置
function ActTigerBornData:GetBuyCfg()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,self.cfg.buy_configure) or {}
    return cfg[1]
end

-- 是否激活了
function ActTigerBornData:IsActive()
    local flag = self.act_info.info.buy_flag or 0
    return flag == 1
end

function ActTigerBornData:GetCardListData()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,self.cfg.gift_configure,"seq") or {}
    return cfg
end

-- 1可领取，0时间未到，-1已领取
function ActTigerBornData:GetCardFlag(data)
    if not self.act_info.info.day_reward then
        return 0
    end
    local seq = data.seq
    local reward_type = self:GetCardRewardType(data.seq)
    if reward_type >= 0 then
        return -1
    end
    local open_day = TimeCtrl.Instance:GetActOpenDay(self.act_type)
    if open_day >= data.activity_days then
        return 1
    end
    return 0
end

function ActTigerBornData:GetCardRewardType(seq)
    return self.act_info.info.day_reward[seq]
end

--获取奖励预览
function ActTigerBornData:GetRewardListData()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,self.cfg.show_reward_item) or {}
    if TableIsEmpty(cfg) then
        return {}
    end
    local reward_data = cfg[1].show_reward_item
    return DataHelper.FormatItemList(reward_data)
end

-- 获取奖励配置
function ActTigerBornData:GetRewardCfg(reward_type)
    return self.cfg.gift_group[reward_type + 1] or {}
end

--寅虎问世红点
function ActTigerBornData:RemindNum()
    if not ActivityData.IsOpen(self.act_type) then
		return 0
    end
    local is_new = ActivityData.Instance:IsHasSign(self.act_type)
    if is_new then
        return 1
    end
    if not self:IsActive() then
        return 0
    end
    local list_data = self:GetCardListData() or {}
    for i,v in pairs(list_data) do
        local flag = self:GetCardFlag(v)
        if flag > 0 then
            return 1
        end
    end
    return 0
end