-- 万圣祈福
VIEW_REQ("modules/serveractivity/halloween_pray/halloween_pray_view")
VIEW_REQ("modules/serveractivity/halloween_pray/halloween_pray_gailv")
VIEW_REQ("modules/serveractivity/halloween_pray/halloween_pray_record")
VIEW_REQ("modules/serveractivity/halloween_pray/halloween_pray_reward")

HalloweenPrayCtrl = HalloweenPrayCtrl or BaseClass(BaseCtrl)

HalloweenPrayCtrl.ReqType = {
    Info = 0,   -- 请求信息
    Pray = 1,   -- 单抽
    Pray10 = 2, -- 10连抽
}

function HalloweenPrayCtrl:__init()
	if HalloweenPrayCtrl.Instance ~= nil then
		Debuger.LogError("[HalloweenPrayCtrl] attempt to create singleton twice!")
		return
	end
	HalloweenPrayCtrl.Instance = self
	self.data = HalloweenPrayData.New()
    self:RegisterAllProtocols()
    self:RegisterRemind()
    BagData.Instance:ListenItems({[1] = self.data:ItemId()}, BindTool.Bind(self.FlushCurr, self))
end

function HalloweenPrayCtrl:__delete()
    self:UnRegisterAllProtocols()
    HalloweenPrayCtrl.Instance = nil
end

function HalloweenPrayCtrl:OnUnloadGameLogic()
    self.data:ClearData() 
end

function HalloweenPrayCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAHalloweenBlessingInfo, "OnSCRAHalloweenBlessingInfo")
    self:RegisterProtocol(SCRAHalloweenBlessingRewardInfo, "OnSCRAHalloweenBlessingRewardInfo")
end

function HalloweenPrayCtrl:RegisterRemind()
    RemindCtrl.Instance:Remind():Register(Mod.Act_Halloween.Pray, self.data.pray_data, BindTool.Bind(self.Remind, self))
end


function HalloweenPrayCtrl:Remind()
    return self.data:Remind()
end

function HalloweenPrayCtrl:FlushCurr()
    self.data:onFlushCurr()
end

function HalloweenPrayCtrl:OnSCRAHalloweenBlessingInfo(protocol)
    self.data:SetBlessingInfo(protocol)
end

function HalloweenPrayCtrl:OnSCRAHalloweenBlessingRewardInfo(protocol)
    self.data:SetRewardInfo(protocol)
end


HalloweenPrayData = HalloweenPrayData or BaseClass()

function HalloweenPrayData:__init()
    if HalloweenPrayData.Instance ~= nil then
        Debuger.LogError("[HalloweenPrayData] attempt to create singleton twice!")
        return
    end
    HalloweenPrayData.Instance = self
    self.cfg = Config.wanshengqifu_auto
    self.act_type = ACTIVITY_TYPE.RAND_HALLOWEEN_PRAY
    self.pray_data=SmartData.New( {curr_num=0,log_list=nil,isSkipAni=false})
    self.draw_result_data=SmartData.New( {reward_list=nil})
end

function HalloweenPrayData:GetCfg()
    local cfgs = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type, self.cfg.basis_configuration, "seq")
    self.cfg_pray = {cfgs = cfgs, show_reward = {}, big_reward = {}, common_reward = {}}
    for k, v in pairs(cfgs) do
        if v.is_show == 1 then
            table.insert(self.cfg_pray.show_reward, Item.Create(v.reward_item))
        end
        if v.is_award == 1 then
            table.insert(self.cfg_pray.big_reward, v)
        else
            table.insert(self.cfg_pray.common_reward, v)
        end
    end
    return self.cfg_pray
end


function HalloweenPrayData:SetBlessingInfo(protocol)
    self.pray_data.log_list=protocol.log_list
end

function HalloweenPrayData:SetRewardInfo(protocol)
    self.draw_result_data.reward_list = protocol.reward_list
end

function HalloweenPrayData:GetRewardShowList()
    local list = {}
    for k, v in pairs(self.draw_result_data.reward_list) do
        local cfg = self:GetCfg().cfgs[v + 1]
        if cfg then
            local item = Item.Create(cfg.reward_item)
            item.IsGem = false
            local is_xiyou= cfg.is_award==1
            table.insert(list,{item=item,is_xiyou=is_xiyou})
        end
    end
    return list
end

function HalloweenPrayData:onFlushCurr()
    self.pray_data.curr_num = Item.GetNum(self:ItemId())
    self.pray_data:Notify()
end

function HalloweenPrayData:ItemId()
    return self.cfg.other[1].consume_item
end

function HalloweenPrayData:Remind()
    if not ActivityData.IsOpen(self.act_type) then
        return 0
    end
    local act_start = ActivityData.GetStartStampTime(self.act_type)
    if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(self.act_type)) then
        return 1
    end
    return self.pray_data.curr_num > 0 and 1 or 0
end

function HalloweenPrayData:ClearFirstRemind()
    local act_start = ActivityData.GetStartStampTime(self.act_type)
    if tostring(act_start) ~= PrefsString(PrefKeys.ActivityFirstRemind(self.act_type)) then
        PrefsString(PrefKeys.ActivityFirstRemind(self.act_type), act_start)
        self.pray_data:Notify()
    end
end

function HalloweenPrayData:ClearData()
    self.pray_data.log_list=nil;
    self.pray_data.isSkipAni = false;
    self.draw_result_data.Set({reward_list=nil})
end

function HalloweenPrayData:GetShowStyle()
    local cfg=Config.wanshengqifu_auto.activity_interface;
    for k, v in pairs(cfg) do
        if v.is_show==1 then
            return v.interface
        end
    end
    return 2
end