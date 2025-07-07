-- 端午活动，幸运盲盒
VIEW_REQ("modules/serveractivity/dragon_boat/luck_draw_box/act_luck_draw_box_view")

ActLuckDrawBoxCtrl = ActLuckDrawBoxCtrl or BaseClass(BaseCtrl)

ActLuckDrawBoxCtrl.ReqType = {
    Info = 0,   -- 请求信息
    Open = 1,   -- 开启盲盒         p1:行，p2:列
    GetReward = 2,  -- 累计充值领取  p1: seq
    Buy = 3,    -- 直购 p1:seq
}

function ActLuckDrawBoxCtrl:__init()
	if ActLuckDrawBoxCtrl.Instance ~= nil then
		Debuger.LogError("[ActLuckDrawBoxCtrl] attempt to create singleton twice!")
		return
	end
    ActLuckDrawBoxCtrl.Instance = self
    ActLuckDrawBoxData.New()
    self.data = ActLuckDrawBoxData.Instance
    self:RegisterAllProtocols()
end

function ActLuckDrawBoxCtrl:__delete()
    ActLuckDrawBoxCtrl.Instance = nil
end

function ActLuckDrawBoxCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function ActLuckDrawBoxCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRALuckDrawBoxInfo, "OnSCRALuckDrawBoxInfo")

     --红点
    Remind.Instance:Register(Mod.RandActivity.LuckBox,self.data.remind_data, function () 
        return self.data:RemindNum() 
    end)

    local item_id = self.data:GetStuffItem()
    BagData.Instance:ListenItems({item_id},function ()
        self.data:RemindNotice()
    end)
end

function ActLuckDrawBoxCtrl:OnSCRALuckDrawBoxInfo(protocol)
    self.data:SetActInfo(protocol)
end

ActLuckDrawBoxData = ActLuckDrawBoxData or BaseClass()

function ActLuckDrawBoxData:__init()
    if ActLuckDrawBoxData.Instance ~= nil then
        Debuger.LogError("[ActLuckDrawBoxData] attempt to create singleton twice!")
        return
    end
    ActLuckDrawBoxData.Instance = self

    self.act_info = SmartData.New({})
    -- 红点刷新
    self.remind_data = SmartData.New({v = false})
end

function ActLuckDrawBoxData:ClearData()
    self.act_info:Set({})
end

function ActLuckDrawBoxData:SetActInfo(protocol)
    -- 先对比之前，看获得了哪个奖励
    if not TableIsEmpty(self.act_info) then
        local info = protocol.info
        for i = 1,4 do
            for j = 1, 3 do
                local last_seq = -1
                if not TableIsEmpty(self.act_info.box_flag_list) then
                    last_seq = self.act_info.box_flag_list[i][j]
                end
                local cur_seq = info.box_flag_list[i][j]
                if cur_seq >= 0 and cur_seq ~= last_seq then
                    local reward_cfg = self:GetRewardCfg(cur_seq)
                    local item = Item.Create(reward_cfg.reward_item)
                    ViewMgr:OpenView(GiftOpenView,{item = item,gift_type = GIFT_OPEN_TYPE.CONGRATULATIONS})
                end
            end
        end
    end
    self.act_info:Set(protocol.info)
end

function ActLuckDrawBoxData:Cfg()
    return Config.xingyunmanghe_auto
end

-- 消耗道具
function ActLuckDrawBoxData:GetStuffItem()
    local cfg = Config.xingyunmanghe_auto.other[1].consume_item
    return cfg.item_id, cfg.num
end

-- x,y [4,3] 小于0未开启，大于0为奖励seq
function ActLuckDrawBoxData:IsOpen(x,y)
    if TableIsEmpty(self.act_info.box_flag_list) then
        return -1
    end
    return self.act_info.box_flag_list[x][y]
end

function ActLuckDrawBoxData:ChongZhiNum()
    return self.act_info.chongzhi or 0
end

-- 当前基础配置
function ActLuckDrawBoxData:GetBaseCfg()
    local cfg = self:Cfg()
    local cfg_group = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.LUCK_DRAW_BOX,cfg.basis_configuration,"type")
    return cfg_group[1]
end

-- 当前奖励组
function ActLuckDrawBoxData:GetRewardGroupId()
    local id = self:GetBaseCfg().reward_group
    return id
end

-- 获取预览
function ActLuckDrawBoxData:GetPreviewListData()
    local id = self:GetRewardGroupId()
    local list_data = {}
    local cfg = self:Cfg()
    for i,v in pairs(cfg.reward_configuration) do
        if v.reward_group == id then
            table.insert(list_data, v)
        end
    end
    return list_data
end

-- 获取奖励配置
function ActLuckDrawBoxData:GetRewardCfg(seq)
    local reward_group = self:GetPreviewListData()
    for i,v in pairs(reward_group) do
        if v.seq == seq then
            return v
        end
    end
end

-- 获取商城配置
function ActLuckDrawBoxData:GetShopList()
    local cfg = self:Cfg()
    local cfg_group = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.LUCK_DRAW_BOX,cfg.shop_configuration,"type")
    return cfg_group
end

-- 累计充值
function ActLuckDrawBoxData:GetChongZhiList()
    local cfg = self:Cfg()
    local cfg_group = ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.LUCK_DRAW_BOX,cfg.add_recharge_configuration,"type")
    return cfg_group
end

-- 1 可领取，0不可领取，-1 已领取
function ActLuckDrawBoxData:ChongZhiFlag(data)
    if TableIsEmpty(self.act_info) then
        return 0
    end
    local count = 0
    local cz_num = self.act_info.chongzhi or 0
    if data.limit_type == 1 then
        count = self.act_info.act_reward_times[data.seq] or 0
    else
        count = self.act_info.today_reward_times[data.seq] or 0
    end
    if count > 0 then
        return -1
    end
    if cz_num >= data.add_recharge then
        return 1
    else
        return 0
    end
end

-- 道具商城购买了多少次
function ActLuckDrawBoxData:GetBuyCount(data)
    if TableIsEmpty(self.act_info) then
        return 0
    end
    if data.limit_type == 1 then
        return self.act_info.act_buy_times[data.seq] or 0
    else
        return self.act_info.today_buy_times[data.seq] or 0
    end
end


-- 红点通知
function ActLuckDrawBoxData:RemindNotice()
    self.remind_data.v = not self.remind_data.v
end

-- 累计充值红点
function ActLuckDrawBoxData:RewardRemind()
    -- 是否有奖励可领
    local list = self:GetChongZhiList()
    for i,v in pairs(list) do
        local flag = self:ChongZhiFlag(v)
        if flag > 0 then
            return 1
        end
    end

    return 0
end

-- 红点逻辑
function ActLuckDrawBoxData:RemindNum()
    if not ActivityData.IsOpen(ACTIVITY_TYPE.LUCK_DRAW_BOX) then
        return 0
    end
    if not self.first_remind then
        return 1
    end
    local item_id,need_num = self:GetStuffItem()
    local num = Item.GetNum(item_id)
    if num >= need_num then
        return 1
    end

    if self:RewardRemind() > 0 then
        return 1
    end

    return 0
end