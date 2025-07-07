CSYunDingTianGongReq = CSYunDingTianGongReq or BaseClass(BaseProtocolStruct)
function CSYunDingTianGongReq:__init()
    self.msg_type = 5700--云顶天宫操作
    --0进场景
    --1 改变台子 p1 --台子id
    --2 攻击玩家 p1 p2 role_uuid
    --3 攻击boss
    --4 请求排行榜 p1 排行榜类型
    --5 领取成就奖励
end
function CSYunDingTianGongReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
    LogDG("云顶操作协议", self)
end

SCYunDingTianGongAchievementInfo = SCYunDingTianGongAchievementInfo or BaseClass(BaseProtocolStruct)
function SCYunDingTianGongAchievementInfo:__init()
    self.msg_type = 5701--云顶天宫成就信息
    self.TypeMax = 3 -- 0累计积分1胜利次数2主动挑战玩家3挑战boss
end
function SCYunDingTianGongAchievementInfo:Decode()
    self.list = {}
    for i = 0, self.TypeMax do
        self.list[i] = self:GodSpaceUserYunDingTianGongAchievementItem()
    end
end
function SCYunDingTianGongAchievementInfo:GodSpaceUserYunDingTianGongAchievementItem()
    local info = {}
    info.num = MsgAdapter.ReadUShort() --当前类型进度 0累计积分1战斗胜利2主动打玩家3挑战boss
    info.reserved_sh = MsgAdapter.ReadUShort()
    info.fetch_flag = MsgAdapter.IntFlag()--奖励领取标志
    return info
end

SCYunDingTianGongRoleInfo = SCYunDingTianGongRoleInfo or BaseClass(BaseProtocolStruct)
function SCYunDingTianGongRoleInfo:__init()
    self.msg_type = 5702--云顶天宫玩家信息，进场景发
end
function SCYunDingTianGongRoleInfo:Decode()
    self.platform_id = MsgAdapter.ReadInt()--台子id
    self.action_value = MsgAdapter.ReadInt()--行动值
    self.score = MsgAdapter.ReadInt()--分数
    self.goto_time = MsgAdapter.ReadUInt()--进入台子时间
    self.attack_role_time = MsgAdapter.ReadUInt()--上次攻击人时间
    self.attack_boss_time = MsgAdapter.ReadUInt()--上次攻击boss的时间
    LogDG("进场景发的人物信息", self)
end


SCYunDingTianGongRank = SCYunDingTianGongRank or BaseClass(BaseProtocolStruct)
function SCYunDingTianGongRank:__init()
    self.msg_type = 5703--排行榜
end
function SCYunDingTianGongRank:Decode()
    self.rank_type = MsgAdapter.ReadInt()--0boss总伤害1boss单伤2总攻击次数3分数4个人分数
    self.rank = MsgAdapter.ReadInt()
    self.rank_value = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.rank_list = {}
    for i = 1, self.count, 1 do
        self.rank_list[i] = self:YunDingTianGongRankItem(i)
    end
    LogDG("boss排行榜", self)
end
function SCYunDingTianGongRank:YunDingTianGongRankItem(rank)
    local info = {}
    info.rank_type = self.rank_type
    info.boss_type = self.rank_type
    info.rank = rank
    info.server_id = MsgAdapter.ReadLL()--阵营id
    info.p1 = MsgAdapter.ReadLL()
    MsgAdapter.ReadLL()
    return info
end

SCYunDingTianGongRoleRank = SCYunDingTianGongRoleRank or BaseClass(BaseProtocolStruct)
function SCYunDingTianGongRoleRank:__init()
    self.msg_type = 5704--玩家排行榜
end
function SCYunDingTianGongRoleRank:Decode()
    self.rank_type = MsgAdapter.ReadInt()--0boss总伤害1boss单伤2总攻击次数3boss总伤害4分数5个人分数
    self.rank = MsgAdapter.ReadInt()
    self.rank_value = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.rank_list = {}
    for i = 1, self.count, 1 do
        self.rank_list[i] = self:YunDingTianGongRankItem(i)
    end
    LogDG("玩家排行榜", self)
end
function SCYunDingTianGongRoleRank:YunDingTianGongRankItem(rank)
    local info = {}
    info.rank = rank
    info.rank_type = self.rank_type
    info.boss_type = self.rank_type
    info.role_uuid = MsgAdapter.ReadLL()--plat_type + role_id == role_uuid
    info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    info.server_id = MsgAdapter.ReadLL()
    info.sore = MsgAdapter.ReadInt()
    MsgAdapter.ReadLL()
    return info
end

SCYunDingTianGongSceneRoleInfo = SCYunDingTianGongSceneRoleInfo or BaseClass(BaseProtocolStruct)
function SCYunDingTianGongSceneRoleInfo:__init()
    self.msg_type = 5705--场景玩家
end
function SCYunDingTianGongSceneRoleInfo:Decode()
    self.type = MsgAdapter.ReadInt()--0进场景 1离开场景
    self.item = self:YunDingTianGongSceneRoleItem()
    LogDG("场景角色信息", self)
end

function SCYunDingTianGongSceneRoleInfo:YunDingTianGongSceneRoleItem()
    local info = {}
    -- info.role_uuid = MsgAdapter.ReadLL()--大跨服不能够role_id重复
    info.role_id = MsgAdapter.ReadInt()
    info.plat_type = MsgAdapter.ReadInt()
    info.role_uuid = int64.new(info.role_id, info.plat_type)
    info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    info.server_id = MsgAdapter.ReadLL()
    info.faction_id = info.server_id
    info.action_value = MsgAdapter.ReadInt()
    info.platform_id = MsgAdapter.ReadInt()
    info.cap = MsgAdapter.ReadInt()
    return info
end


SCYunDingTianGongBossInfo = SCYunDingTianGongBossInfo or BaseClass(BaseProtocolStruct)
function SCYunDingTianGongBossInfo:__init()
    self.msg_type = 5706--场景boss
end
function SCYunDingTianGongBossInfo:Decode()
    self.boss_index = MsgAdapter.ReadInt()
    self.boss_remain_hp = MsgAdapter.ReadInt() -- boss剩余血量万分比
    self.boss_can_challenge_time = MsgAdapter.ReadUInt() -- 新boss多久后可挑战
    self.boss_skill_time = MsgAdapter.ReadUInt() -- boss释放大招的时间
    LogDG("场景BOSS", self)
end

SCYunDingTianGongPVPResult = SCYunDingTianGongPVPResult or BaseClass(BaseProtocolStruct)
function SCYunDingTianGongPVPResult:__init()
    self.msg_type = 5707
end
function SCYunDingTianGongPVPResult:Decode()
    self.is_attacker_win = MsgAdapter.ReadInt()
    self.result_item1 = self:YunDingTianGongScenePVPResultItem()
    self.result_item2 = self:YunDingTianGongScenePVPResultItem()
    self.act_type = ActType.Palaces
    LogDG("归墟人物对战结果", self)
end

function SCYunDingTianGongPVPResult:YunDingTianGongScenePVPResultItem()
    local info = {}
    -- info.role_uuid = MsgAdapter.ReadLL()--plat_type + role_id == role_uuid
    info.role_id = MsgAdapter.ReadInt()
    info.plat_type = MsgAdapter.ReadInt()
    info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    info.server_id = MsgAdapter.ReadLL()
    info.prof = MsgAdapter.ReadInt()
    info.avatar_type = MsgAdapter.ReadShort()		   -- 123456
    info.headshot_id= MsgAdapter.ReadShort()		   -- 头像ID
    info.avatar_id = info.headshot_id
    return info
end


SCYunDingTianGongAttackBossResult = SCYunDingTianGongAttackBossResult or BaseClass(BaseProtocolStruct)
function SCYunDingTianGongAttackBossResult:__init()
    self.msg_type = 5708--攻击boss结果
end
function SCYunDingTianGongAttackBossResult:Decode()
    self.total_hurt = MsgAdapter.ReadLL()
    LogDG("Boss攻击结果", self)
end

SCYunDingTianGongOpenInfo = SCYunDingTianGongOpenInfo or BaseClass(BaseProtocolStruct)
function SCYunDingTianGongOpenInfo:__init()
   self.msg_type = 5709 
end
function SCYunDingTianGongOpenInfo:Decode()
    self.info = {}
    self.info.state = MsgAdapter.ReadInt()
    self.info.ready_time = MsgAdapter.ReadInt()
    self.info.open_time = MsgAdapter.ReadInt()
    self.info.end_time = MsgAdapter.ReadInt()

    LogDG("云顶天宫活动信息", self)
end


-- 万胜祈福信息下发 5720
SCRAHalloweenBlessingInfo = SCRAHalloweenBlessingInfo or BaseClass(BaseProtocolStruct)
function SCRAHalloweenBlessingInfo:__init()
    self.msg_type = 5720
end

function SCRAHalloweenBlessingInfo:Decode()
    local log_func = function ()
        local t = {}
        t.timestamp = MsgAdapter.ReadUInt()
        t.item_id = MsgAdapter.ReadUShort()
        t.is_bind = MsgAdapter.ReadShort()
        t.num = MsgAdapter.ReadInt()
        return t
    end
    local log_list = {}
    for i = 1, 50 do
        local log = log_func()
        if log.item_id > 0 then
            table.insert(log_list, log)
        end
    end
    table.sort(log_list, function (a,b)
        return a.timestamp > b.timestamp
    end)
    self.log_list = log_list
    LOG("万圣祈福日志",self.log_list)
end

-- 5721 万圣节祈福抽奖奖励下发
SCRAHalloweenBlessingRewardInfo = SCRAHalloweenBlessingRewardInfo or BaseClass(BaseProtocolStruct)
function SCRAHalloweenBlessingRewardInfo:__init()
    self.msg_type = 5721
end

function SCRAHalloweenBlessingRewardInfo:Decode()
    local count = MsgAdapter.ReadInt()
    local reward_list = {}
    for i = 1, count do
        local seq = MsgAdapter.ReadShort()  -- 抽奖获得的索引
        table.insert(reward_list, seq)
    end
    self.reward_list = reward_list
    LOG("万圣祈福抽奖信息",self.reward_list)
end

--5730 万圣累充信息下发
SCRAWanShengLeiChongInfo = SCRAWanShengLeiChongInfo or BaseClass(BaseProtocolStruct)
function SCRAWanShengLeiChongInfo:__init()
    self.msg_type = 5730
end

function SCRAWanShengLeiChongInfo:Decode()
    self.chong_zhi_value = MsgAdapter.ReadUInt()/10
    self.fetch_flag=MsgAdapter.IntFlag()
end

-- 5740 万圣商城信息下发
SCRAWanShengShopInfo = SCRAWanShengShopInfo or BaseClass(BaseProtocolStruct)
function SCRAWanShengShopInfo:__init()
    self.msg_type = 5740
end

function SCRAWanShengShopInfo:Decode()
    self.ra_day_buy_times={}
    self.ra_buy_times={}
    for i = 0, WAN_SHENG_SHOP_MAX_BUY_ITEM-1, 1 do
        self.ra_day_buy_times[i] = MsgAdapter.ReadUShort()
    end
    for i = 0, WAN_SHENG_SHOP_MAX_BUY_ITEM-1, 1 do
        self.ra_buy_times[i] = MsgAdapter.ReadUShort()
    end
end
--大玩咖等级信息
SCROleDaWanKaInfoChange = SCROleDaWanKaInfoChange or BaseClass(BaseProtocolStruct)
function SCROleDaWanKaInfoChange:__init()
    self.msg_type = 5745
end

function SCROleDaWanKaInfoChange:Decode()
    self.obj_id = MsgAdapter.ReadInt()
    self.da_wan_ka_level = MsgAdapter.ReadInt()
end
--腾讯特权大厅
SCRATengXunDaTingInfo = SCRATengXunDaTingInfo or BaseClass(BaseProtocolStruct)
function SCRATengXunDaTingInfo:__init()
    self.msg_type = 5750
end

function SCRATengXunDaTingInfo:Decode()
    self.is_fetch_reward = MsgAdapter.ShortFlag()
    self.is_fetch_day_reward = MsgAdapter.ShortFlag()
    self.level_reward_fetch_flag = MsgAdapter.IntFlag(2)
end

-- 幸运数字信息下发
SCRALuckNumInfo = SCRALuckNumInfo or BaseClass(BaseProtocolStruct)
function SCRALuckNumInfo:__init()
    self.msg_type = 5725
end

function SCRALuckNumInfo:Decode()
    local info = {}
    local task_finish_list = MsgAdapter.IntFlag(1)  -- 任务完成标记
    info.reward_flag = MsgAdapter.ReadInt()   -- 奖励是否已领取
    info.task_list = {}
    for i = 0, 2 do
        info.task_list[i] = {}
        info.task_list[i].seq = i
        info.task_list[i].luck_num = MsgAdapter.ReadInt() -- 幸运数字
        info.task_list[i].is_finish = task_finish_list[i] or 0
    end
    self.info = info
end

-- 幸运数字开奖数字信息改变
SCRALuckNumChange = SCRALuckNumChange or BaseClass(BaseProtocolStruct)
function SCRALuckNumChange:__init()
    self.msg_type = 5726
end

function SCRALuckNumChange:Decode()
    local info = {}
    info.is_valid = MsgAdapter.ReadInt()    -- 是否有效
    info.luck_num = MsgAdapter.ReadInt()    -- 开奖幸运数字
    self.info = info
end

--npc好感度请求
CSNpcFavorReq = CSNpcFavorReq or BaseClass(BaseProtocolStruct)
function CSNpcFavorReq:__init()
    self.msg_type = 5770
end
function CSNpcFavorReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    for _,v in pairs(self.gift_item_list) do
        MsgAdapter.WriteUShort(v.col)
        MsgAdapter.WriteUShort(v.index)
        MsgAdapter.WriteInt(v.times)
    end
end

--Npc好感度信息
SCNpcFavorInfo = SCNpcFavorInfo or BaseClass(BaseProtocolStruct)
function SCNpcFavorInfo:__init()
    self.msg_type = 5771
    self.NPC_FAVOR_MAX_NUN=256
end

function SCNpcFavorInfo:Decode()
     local info = {}
     info.action_value= MsgAdapter.ReadInt()                 --活力值
     info.next_add_action_value_time= MsgAdapter.ReadUInt()  --下次恢复活力值的时间，每480s加一次
     info.total_gift_times= MsgAdapter.ReadInt()             --送礼总次数
     local flag1 = MsgAdapter.ReadUInt()
     local flag2 = MsgAdapter.ReadUInt()
     info.zone_fetch_flag = DataHelper.ReverseList( bit:ll2b(flag2, flag1))           --区域领取奖励标志
     info.favor_list={}
     for i =1 , self.NPC_FAVOR_MAX_NUN do
        local favor_item = {} 
        favor_item.gift_times= MsgAdapter.ReadUChar()        --送礼次数
        favor_item.banquet_times= MsgAdapter.ReadUChar()     --宴请次数
        favor_item.favor_value= MsgAdapter.ReadUShort()      --好感值
        local flag=MsgAdapter.ReadUShort()
        favor_item.fetch_flag= DataHelper.ReverseList(bit:d16b(flag) )      --领取奖励标志
        favor_item.reserved= MsgAdapter.ReadUShort()
        table.insert(info.favor_list,favor_item)
     end
     self.info=info
end

--单个Npc好感度信息
SCNpcFavorItemInfo = SCNpcFavorItemInfo or BaseClass(BaseProtocolStruct)
function SCNpcFavorItemInfo:__init()
    self.msg_type = 5772
end
function SCNpcFavorItemInfo:Decode()
     self.favor_index= MsgAdapter.ReadInt()                 
     self.favor_item={}
     self.favor_item.gift_times= MsgAdapter.ReadUChar()        
     self.favor_item.banquet_times= MsgAdapter.ReadUChar()     
     self.favor_item.favor_value= MsgAdapter.ReadUShort()  
     local flag = MsgAdapter.ReadUShort()    
     self.favor_item.fetch_flag= DataHelper.ReverseList(bit:d16b(flag))       
     self.favor_item.reserved= MsgAdapter.ReadUShort()
end

--好感度信息改变
SCNpcFavorCommonInfo  = SCNpcFavorCommonInfo or BaseClass(BaseProtocolStruct)
function SCNpcFavorCommonInfo:__init()
    self.msg_type = 5773
end
function SCNpcFavorCommonInfo:Decode()
     self.action_value= MsgAdapter.ReadInt()                 
     self.next_add_action_value_time= MsgAdapter.ReadUInt()        
     self.total_gift_times= MsgAdapter.ReadInt()     
     local flag1 = MsgAdapter.ReadUInt()
     local flag2 = MsgAdapter.ReadUInt()
     self.zone_fetch_flag = DataHelper.ReverseList(bit:ll2b(flag2, flag1))           --区域领取奖励标志
end

--幽冥抓捕
CSNetherWorldSealReq = CSNetherWorldSealReq or BaseClass(BaseProtocolStruct)
function CSNetherWorldSealReq:__init()
    self.msg_type = 5760
end

function CSNetherWorldSealReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

--幽冥抓捕
SCNetherWorldSealInfo = SCNetherWorldSealInfo or BaseClass(BaseProtocolStruct)
function SCNetherWorldSealInfo:__init()
    self.msg_type = 5761
end

function SCNetherWorldSealInfo:Decode()
    self.buy_times = {}
    for i = 0, 3 do
        self.buy_times[i] = MsgAdapter.ReadInt()
    end
    --LogError("5761",self)
end
--幽冥抓捕结果
SCNetherWorldSealResult = SCNetherWorldSealResult or BaseClass(BaseProtocolStruct)
function SCNetherWorldSealResult:__init()
    self.msg_type = 5762
end

function SCNetherWorldSealResult:Decode()
    self.is_succ = MsgAdapter.ReadShort()   --是否成功
    self.is_auto_seal = MsgAdapter.ReadShort()  --是否自动封印
    self.monster_id = MsgAdapter.ReadInt()      --封印的怪物id
    self.nether_world_id = MsgAdapter.ReadInt() --封印的幽冥id
    self.seal_card_item_id = MsgAdapter.ReadInt()   --封印卡的物品id
    --LogError("5762",self)
end

SCNetherWorldAutoSealInfo = SCNetherWorldAutoSealInfo or BaseClass(BaseProtocolStruct)
function SCNetherWorldAutoSealInfo:__init()
    self.msg_type = 5763
end
--1自动 0手动
function SCNetherWorldAutoSealInfo:Decode()
    self.is_auto_seal = MsgAdapter.ReadInt()
    --LogError("5763",self)
end
-- 福运商城活动信息
SCFuYunShangChengInfo = SCFuYunShangChengInfo or BaseClass(BaseProtocolStruct)
function SCFuYunShangChengInfo:__init()
    self.msg_type = 5755
end

function SCFuYunShangChengInfo:Decode()
    local info = {}
    info.day_buy_times = {} -- 每天购买次数
    info.act_buy_times = {} -- 活动购买次数
    for i = 0, 19 do
        info.day_buy_times[i] = MsgAdapter.ReadUShort()
    end
    for i = 0, 19 do
        info.act_buy_times[i] = MsgAdapter.ReadUShort()
    end
    self.info = info
end

SCGuaJiPointsInfo = SCGuaJiPointsInfo or BaseClass(BaseProtocolStruct)
function SCGuaJiPointsInfo:__init()
    self.msg_type = 5765
end
function SCGuaJiPointsInfo:Decode()
    self.guaji_points_value = MsgAdapter.ReadInt()
    self.today_add_guaji_exp = self.guaji_points_value
end