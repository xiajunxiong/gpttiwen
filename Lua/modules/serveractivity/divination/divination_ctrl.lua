VIEW_REQ("modules/serveractivity/divination/divination_view")

DivinationCtrl = DivinationCtrl or BaseClass(BaseCtrl)
 
function DivinationCtrl:__init()
	self:RegisterProtocol(SCRATianMingDivinationInfo,"OnDivinationInfo")
    self:RegisterProtocol(SCTianMingDivinationActivityStartChouResult,"OnDivinationStartChouResult")
    self:RegisterProtocol(SCTianMingRewardPoolInfo,"OnSCTianMingRewardPoolInfo")
    self.data = DivinationData.New()
end

function DivinationCtrl:__delete()
    self:UnRegisterProtocol(SCRATianMingDivinationInfo)
    self:UnRegisterProtocol(SCTianMingDivinationActivityStartChouResult)
    self:UnRegisterProtocol(SCTianMingRewardPoolInfo)
end

function DivinationCtrl:OnDivinationInfo(protocol)
    self.data:OnDivinationInfoProtocol(protocol)
end

function DivinationCtrl:OnDivinationStartChouResult(protocol)
    self.data:OnDivinationStartChouResultProtocol(protocol)
end

function DivinationCtrl:OnSCTianMingRewardPoolInfo(protocol)
    self.data:OnSCTianMingRewardPoolInfoProtocol(protocol)
end

function DivinationCtrl:OnUnloadGameLogic()
	self.data:ResetData()
end

DivinationData = DivinationData or BaseClass()

DivinationData.ADD_LOT_ITEM_COUNT = 6
DivinationData.MAX_REWARD_HISTORY_COUNT = 40

DivinationData.ServerOperate = {
    QueryInfo = 0,      --请求信息
    AddLotTimes = 1,    --加注
    ResetLotTimes = 2,  --重置加注
    StartChou = 3,      --开始抽签
    RewardPoolInfo = 4, --请求历史获奖列表
}
function DivinationData:__init()
	if DivinationData.Instance ~= nil then
		Debuger.LogError("[DivinationData] attempt to create singleton twice!")
		return
	end	
    DivinationData.Instance = self
    self.reward_data = SmartData.New({
        reward_index = 0,
        check = false
    })
    self.use_time_flush = SmartData.New({check = false})
    self.reward_history = SmartData.New()
    self.add_lot_info = SmartData.New()
    self.view_data = SmartData.New({
        anim_skip = false,
    })
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoDivinationActivity,function() 
            Cfg.UnRegister("GetDivinationAddLotPriceCfg")
            Cfg.Register("GetDivinationAddLotPriceCfg",Cfg.RealTimeSearch(Config.divination_activity_auto.tianming_consume,"lot_add_times"))
            DivinationData.MAX_ADD_LOT_NUM = 0
            for _,v in pairs(Config.divination_activity_auto.tianming_consume) do
                if DivinationData.MAX_ADD_LOT_NUM < v.lot_add_times then
                    DivinationData.MAX_ADD_LOT_NUM = v.lot_add_times
                end
            end
        end,true)


end

function DivinationData:Data()
    return self.data
end

function DivinationData:OnDivinationInfoProtocol(protocol)
    for i,v in ipairs(protocol.add_lots_list) do
        self.add_lot_info[i] = v
    end
    self.today_use_times = Config.divination_activity_auto.other[1].day_times - protocol.today_use_times
    self.use_time_flush.check = not self.use_time_flush.check
end
function DivinationData:GetUseTime()
    return self.today_use_times
end
function DivinationData:GetUseTimeData()
   return self.use_time_flush
end
function DivinationData:OnSCTianMingRewardPoolInfoProtocol(protocol)
    local idx = 1
    self.reward_history:Set({})
    for i= #protocol.pool_info,1,-1 do
        local pro_tab = protocol.pool_info[i]
        if pro_tab.rate_item_id <= 0 or pro_tab.item_nun<=0 then break end
        local tab = self.reward_history[idx]
        if tab == nil then
            tab = SmartData.New()
            self.reward_history[idx] = tab
        end
        tab.rate_item_id = pro_tab.rate_item_id
        tab.item_nun = pro_tab.item_nun
        tab.role_name = pro_tab.role_name
        idx = idx + 1
    end
end

function DivinationData:OnDivinationStartChouResultProtocol(protocol)
    self.reward_data.reward_index = protocol.reward_index
    self.reward_data.check = not self.reward_data.check
end

function DivinationData:RewardHistory()
    return self.reward_history
end

function DivinationData:AddLotInfo()
    return self.add_lot_info
end

function DivinationData:RewardData()
    return self.reward_data
end

function DivinationData:ViewData()
    return self.view_data
end

function DivinationData:ResetData()
    self.add_lot_info:Set({})
    self.reward_history:Set({})
end