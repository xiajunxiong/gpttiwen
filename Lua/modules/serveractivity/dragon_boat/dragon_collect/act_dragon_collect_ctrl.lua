-- 端午活动，苍龙赐福
VIEW_REQ("modules/serveractivity/dragon_boat/dragon_collect/act_dragon_collect_view")

ActDragonCollectCtrl = ActDragonCollectCtrl or BaseClass(BaseCtrl)

ActDragonCollectCtrl.ReqType = {
    Info = 0,   -- 请求信息
    Open = 1,   -- 点亮         p1:seq
    Get = 2,    -- 获取最后一个奖励
}

function ActDragonCollectCtrl:__init()
	if ActDragonCollectCtrl.Instance ~= nil then
		Debuger.LogError("[ActDragonCollectCtrl] attempt to create singleton twice!")
		return
	end
    ActDragonCollectCtrl.Instance = self
    ActDragonCollectData.New()
    self.data = ActDragonCollectData.Instance
    self:RegisterAllProtocols()

    --红点
    Remind.Instance:Register(Mod.DragonBoat.DragonCollect,self.data.remind_data, function () 
        return self.data:RemindNum() 
    end)

    local item_id = self.data:ItemId()
    BagData.Instance:ListenItems({item_id},function ()
        self.data:RemindNotice()
    end)
end

function ActDragonCollectCtrl:__delete()
    ActDragonCollectCtrl.Instance = nil
end

function ActDragonCollectCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function ActDragonCollectCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCDragonCollectInfo, "OnSCDragonCollectInfo")
end

function ActDragonCollectCtrl:OnSCDragonCollectInfo(protocol)
    self.data.act_info:Set(protocol.info)
    self.data:RemindNotice()
end


ActDragonCollectData = ActDragonCollectData or BaseClass()

function ActDragonCollectData:__init()
    if ActDragonCollectData.Instance ~= nil then
        Debuger.LogError("[ActDragonCollectData] attempt to create singleton twice!")
        return
    end
    ActDragonCollectData.Instance = self
    self.cfg = Config.canglongcifu_auto

    self.act_info = SmartData.New({})
    -- 红点刷新
    self.remind_data = SmartData.New({v = false})
end

function ActDragonCollectData:ClearData()
    self.act_info:Set({})
end

function ActDragonCollectData:GetListData()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.DRAGON_COLLECT,self.cfg.basis_configuration,"seq")
    return cfg or {}
end

-- 点亮标记 1 可点亮 0 不可点亮 -1 已点亮
function ActDragonCollectData:GetFlag(data)
    if TableIsEmpty(self.act_info) then
        return 0
    end
    local bit_flag = self.act_info.flag_list[data.seq] or 0
    if bit_flag == 1 then
        return -1
    end
    local num = Item.GetNum(data.light_item.item_id)
    if num >= data.light_item.num then
        return 1
    end
    return 0
end

-- 最后那个奖励是否可以领取 1 可领取 0 不可领取 -1 已领取
function ActDragonCollectData:GetEndFlag()
    if TableIsEmpty(self.act_info) then
        return 0
    end
    if self.act_info.end_reward == 2 then
        return -1
    end
    if self.act_info.end_reward == 1 then
        return 1
    end
    return 0
end

function ActDragonCollectData:ItemId()
    return Config.canglongcifu_auto.other[1].light_item.item_id
end

function ActDragonCollectData:GetShowReward()
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.DRAGON_COLLECT,self.cfg.show_reward,"seq") or {}
    return cfg[1] or {}
end

-- 红点通知
function ActDragonCollectData:RemindNotice()
    self.remind_data.v = not self.remind_data.v
end

-- 红点逻辑
function ActDragonCollectData:RemindNum()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.DRAGON_COLLECT) then
        return 0
    end
    if not self.first_remind then
        return 1
    end
    local list_data = self:GetListData()
    for i,v in pairs(list_data) do
        if self:GetFlag(v) > 0 then
            return 1
        end
    end

    if self:GetEndFlag() > 0 then
        return 1
    end
    
    return 0
end