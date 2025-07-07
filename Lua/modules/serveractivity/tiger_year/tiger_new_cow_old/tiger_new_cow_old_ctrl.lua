VIEW_REQ("modules/serveractivity/tiger_year/tiger_new_cow_old/tiger_new_cow_old_view")

-- =============== 辞牛迎新虎 ===============
TigerNewCowOldCtrl = TigerNewCowOldCtrl or BaseClass(BaseCtrl)
TigerNewCowOldCtrl.ReqType = {
    Info = 0,       -- 请求信息
    AddValue = 1,   -- 添加虎气 p1:seq    
    GetReward = 2,  -- 领取奖励 p1:tiger_type, p2:seq
    Awaken = 3,     -- 唤醒 p1:seq
    GetAwakenReward = 4,    -- 领取唤醒奖励
}
function TigerNewCowOldCtrl:__init()
    if TigerNewCowOldCtrl.Instance ~= nil then
		Debuger.LogError("[TigerNewCowOldCtrl] attempt to create singleton twice!")
		return
	end
    TigerNewCowOldCtrl.Instance = self
    self.data = TigerNewCowOldData.New()
    self.data = TigerNewCowOldData.Instance

    self:RegisterProtocol(SCTigerNewCowOld, "OnSCTigerNewCowOld")
    
    -- 红点注册
    Remind.Instance:Register(Mod.TigerYear.TigerNewCowOld,self.data.act_info, function ()
        return self.data:RemindNum() 
    end)
    BagData.Instance:ListenItems({self.data:GetActItemId()},function ()
        self.data.act_info:Notify()
    end)
end

function TigerNewCowOldCtrl:OnSCTigerNewCowOld(protocol)
    --LogError("辞牛迎新虎协议：：", protocol)
    self.data.act_info:Set(protocol.info)
end

TigerNewCowOldData = TigerNewCowOldData or BaseClass()
function TigerNewCowOldData:__init()
    if TigerNewCowOldData.Instance ~= nil then
		Debuger.LogError("[TigerNewCowOldData] attempt to create singleton twice!")
		return
	end
    TigerNewCowOldData.Instance = self
    self.cfg = Config.ciniuyingxinhu_auto
    self.act_type = ACTIVITY_TYPE.RABD_TIGER_NEW_COW_OLD
    self.act_info = SmartData.New({awaken_reward_time = 0})
end

-- 获取基础配置
function TigerNewCowOldData:GetBaseCfg(tiger_type)
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,self.cfg.basis_configure,"seq")
    if tiger_type then
        for i,v in pairs(cfg) do
            if v.seq == tiger_type then
                cfg = v
                break
            end
        end
    end
    return cfg
end

-- 获取奖励配置
function TigerNewCowOldData:GetGiftCfg(tiger_type)
    local cfg = ServerActivityData.Instance:GetActOpenDayCfg(self.act_type,self.cfg.gift_configure,"seq")
    if tiger_type then
        local new_cfg = {}
        for i,v in pairs(cfg) do
            if v.module_type == tiger_type then
                table.insert(new_cfg, v)
            end
        end
        cfg = new_cfg
    end
    return cfg
end

-- 获取奖励组
function TigerNewCowOldData:GetGiftGroup(group_id)
    if not group_id then
        local base_cfg = self:GetBaseCfg(0)
        group_id = base_cfg.awaken_reward_group
    end
    local group_data = {}
    for i,v in pairs(self.cfg.gift_group) do
        if v.reward_group == group_id then
            table.insert(group_data, v)
        end
    end
    return group_data
end

-- 获取老虎奖励标记 data : Config.ciniuyingxinhu_auto.gift_configure
-- -1 已领取、0不可领取、1可领取
function TigerNewCowOldData:GetGiftFlag(data)
    local tiger_id = data.module_type
    local info = self.act_info.awaken_param[tiger_id]
    local flag = info.reward_flags[16 - data.seq]
    if flag == 1 then
        return -1
    end
    local value = info.tiger_value or 0
    if value >= data.parameter then
        return 1
    end
    return 0
end

-- 获取老虎状态。return : {进度，flag, 开启时间} flag = 1 则已全部激活，0已开启，-1未开启
function TigerNewCowOldData:GetTigerStateData(tiger_type)
    local flag = self.act_info.awaken_flags[16 - tiger_type]
    local data = {
        progress_num = 0,
        progress_max_num = 0,
        flag = -1,
        time = 0,
    }
    if flag == 1 then
        data.flag = 1
        return data
    end
    local open_day = TimeCtrl.Instance:GetActOpenDay(self.act_type)
    local base_cfg = self:GetBaseCfg(tiger_type)
    local limit_open_day = base_cfg.activity_days
    if open_day < limit_open_day then
        data.flag = -1
        local activity_status = ActivityData.Instance:GetActivityStatusInfo(self.act_type)
        data.time = TimeCtrl.Instance:ChangDataTime24(activity_status.param_1) + (base_cfg.activity_days - 2) * 86400
        return data
    else
        data.flag = 0
    end
    local gift_cfg = self:GetGiftCfg(tiger_type)
    data.progress_max_num = #gift_cfg
    for i,v in pairs(gift_cfg) do
        local gift_flag = self:GetGiftFlag(v)
        if gift_flag == -1 or gift_flag == 1 then
            data.progress_num = data.progress_num + 1
        end
    end
    return data
end

-- 获取增加虎气值的道具
function TigerNewCowOldData:GetActItemId()
    return self.cfg.other[1].submit_item
end

function TigerNewCowOldData:RemindNum()
    if not ActivityData.IsOpen(self.act_type) then
		return 0
    end
  
    -- 是否可抽奖
    if self.act_info.awaken_reward_time and self.act_info.awaken_reward_time > 0 then
        return 1
    end
    
    for i = 0, 4 do
        local num = self:GetTigerRemind(i)
        if num > 0 then
            return 1
        end
    end
    return 0
end

-- 单个老虎红点
function TigerNewCowOldData:GetTigerRemind(tiger_type)
    local state_data = self:GetTigerStateData(tiger_type)
    if state_data.flag == -1 then
        return 0
    end
    local gift_cfg = self:GetGiftCfg(tiger_type)
    local is_0 = false
    for i,v in pairs(gift_cfg) do
        local flag = self:GetGiftFlag(v)
        if flag > 0 then
            return 1
        elseif flag == 0 then
            is_0 = true
        end
    end
    if is_0 then
        local item_id = self:GetActItemId()
        if Item.GetNum(item_id) > 0 then
            return 1
        else
            return 0
        end
    end
    return 0
end