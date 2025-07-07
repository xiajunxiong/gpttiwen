--随机活动请求
CSRandActivityOperaReq = CSRandActivityOperaReq or BaseClass(BaseProtocolStruct)
function CSRandActivityOperaReq:__init()
    self.msg_type = 3000
end

function CSRandActivityOperaReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.rand_activity_type)
	MsgAdapter.WriteShort(self.opera_type)
	MsgAdapter.WriteInt(self.param_1)
    MsgAdapter.WriteInt(self.param_2)
    MsgAdapter.WriteInt(self.param_3)
    MsgAdapter.WriteUInt(self.cfg_ver)
    if DEBUG_MODE then
        LOG("随机活动请求", self)
    end
end

----------------------------------------------------------
--商店限时购买 2049
----------------------------------------------------------
SCRandActivityTimeLimitStoreInfo = SCRandActivityTimeLimitStoreInfo or BaseClass(BaseProtocolStruct)
function SCRandActivityTimeLimitStoreInfo:__init()
    self.msg_type = 3001
end

function SCRandActivityTimeLimitStoreInfo:Decode()
    self.limit_buy_count = {}
    for i=1,SHOP_TIME_LIMIT_BUY_MAX_COUNT do
        self.limit_buy_count[i] = MsgAdapter.ReadUShort()
    end
end

----------------------------------------------------------
--随机活动--开服转盘 2051
----------------------------------------------------------
SCRandActivityTurnTableInfo = SCRandActivityTurnTableInfo or BaseClass(BaseProtocolStruct)
function SCRandActivityTurnTableInfo:__init()
    self.msg_type = 3003
end

function SCRandActivityTurnTableInfo:Decode()
    local info = {}
    info.free_times = MsgAdapter.ReadInt()
    info.have_times = MsgAdapter.ReadInt()
    info.free_stamp = MsgAdapter.ReadUInt()
    self.open_roller_info = info
end

SCRandActivityTurnTableReward = SCRandActivityTurnTableReward or BaseClass(BaseProtocolStruct)
function SCRandActivityTurnTableReward:__init()
    self.msg_type = 3004
end

function SCRandActivityTurnTableReward:Decode()
    self.index = MsgAdapter.ReadInt()
end

----------------------------------------------------------
--开服商店限时购买 2052
----------------------------------------------------------
SCRandDiscountStoreInfo = SCRandDiscountStoreInfo or BaseClass(BaseProtocolStruct)
function SCRandDiscountStoreInfo:__init()
    self.msg_type = 3005
end

function SCRandDiscountStoreInfo:Decode()
    self.limit_buy_count = {}
    for i=0,ACT_SHOP_BUY_MAX_COUNT - 1 do
        self.limit_buy_count[i] = MsgAdapter.ReadUShort()
    end
end

----------------------------------------------------------
--随机活动--消费转盘 3008
----------------------------------------------------------
SCRAConsumeCarouselInfo = SCRAConsumeCarouselInfo or BaseClass(BaseProtocolStruct)
function SCRAConsumeCarouselInfo:__init()
    self.msg_type = 3008
end

function SCRAConsumeCarouselInfo:Decode()
    local info = {}
    info.last_num = MsgAdapter.ReadInt()
    info.consume_num_day = MsgAdapter.ReadUInt()
    info.consume_count = MsgAdapter.ReadUInt()
    self.consume_info = info
end

SCRAConsumeCarouselReward = SCRAConsumeCarouselReward or BaseClass(BaseProtocolStruct)
function SCRAConsumeCarouselReward:__init()
    self.msg_type = 3009
end

function SCRAConsumeCarouselReward:Decode()
    self.index = MsgAdapter.ReadInt()
end

SCRAConsumeCarouselShow = SCRAConsumeCarouselShow or BaseClass(BaseProtocolStruct)
function SCRAConsumeCarouselShow:__init()
    self.msg_type = 3010
end

function SCRAConsumeCarouselShow:Decode()
    self.get_list = {}
    for i=1,MsgAdapter.ReadInt() do
        local item_id = MsgAdapter.ReadShort()
        local num = MsgAdapter.ReadShort()
        local role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        self.get_list[i] = Format(Language.Reward.Roller.Get,role_name,Item.GetQuaName(item_id),num)
    end
end

--首充信息
SCRAFirstChargeInfo = SCRAFirstChargeInfo or BaseClass(BaseProtocolStruct)
function SCRAFirstChargeInfo:__init()
    self.msg_type = 3011
end

function SCRAFirstChargeInfo:Decode()
    -- 标志位列表，二进制，0未领取 1已领取
    self.reward_first_flag = DataHelper.ReverseList(bit:d8b(MsgAdapter.ReadUChar()))
    -- 目前只需要第一个，第一个领取后界面就没了
    self.reward_flag = self.reward_first_flag[1]
    -- 首充直购标识，二进制，0没买 1买了，FIRST_RECHARGE_CMD_TYPE
    self.first_cmd_buy_flag = DataHelper.ReverseList(bit:d8b(MsgAdapter.ReadUChar()))
    -- 首充专属，招财仙子累计进度
    self.first_recharge_zhao_cai_progress = MsgAdapter.ReadShort()
    --首充专属，成长助力未购买结束时间戳
    self.first_recharge_grow_end_timestamp = MsgAdapter.ReadUInt()
    -- 首充专属，奖励领取标志位
    self.first_recharge_exclusive_flag = MsgAdapter.IntFlag()
    --玩家充值获取一次铜币创建家族的鸡喙 0未使用1已使用
    self.guild_create_consume_coin_bind = MsgAdapter.ReadInt()
end

CSRAFirstChargeReq = CSRAFirstChargeReq or BaseClass(BaseProtocolStruct)
function CSRAFirstChargeReq:__init()
    self.msg_type = 3012
end

function CSRAFirstChargeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param_1)
    MsgAdapter.WriteInt(self.param_2)
end

--传承瑰宝
SCRAHeritageTreasureInfo = SCRAHeritageTreasureInfo or BaseClass(BaseProtocolStruct)
function SCRAHeritageTreasureInfo:__init()
    self.msg_type = 3013
end

function SCRAHeritageTreasureInfo:Decode()
    self.piece_list = {}
    for i=1,RA_HERITAGE_TREASURE_TREASURE_MAX_PIECE do --对应线索seq
        self.piece_list[i] = {index = i,type = MsgAdapter.ReadUChar()}
    end
    for i=1,RA_HERITAGE_TREASURE_TREASURE_MAX_PIECE do --激活了多个个线索 255是领取了奖励
        self.piece_list[i].clue = MsgAdapter.ReadUChar()
    end
    local config = Config.treasure_activity_auto.heritage_treasure
    for k,v in pairs(self.piece_list) do
        v.npc_id = (v.clue >= 1 and v.clue <= 3) and config[v.type].npc_id or 0
    end
    self.raward_flag = MsgAdapter.ReadUChar()-- 最终奖励标志
    MsgAdapter.ReadUChar()
end

--七彩图鉴
SCColorfulIllustrateInfo = SCColorfulIllustrateInfo or BaseClass(BaseProtocolStruct)
function SCColorfulIllustrateInfo:__init()
    self.msg_type = 3014
end

function SCColorfulIllustrateInfo:Decode()
    self.pigment_list = {}
    self.pigment_num = 0
    for i=0,6 do
        if self.pigment_list[i] == nil then
            self.pigment_list[i] = {}
        end
        for j=0,2 do
            self.pigment_list[i][j] = MsgAdapter.ReadShort() --保存的花灵颜色剩余数量
            self.pigment_num = self.pigment_num + self.pigment_list[i][j]
        end
    end
    self.role_level = MsgAdapter.ReadUChar()
    self.is_fetch_make_flag = MsgAdapter.ReadChar()
    self.save_string = MsgAdapter.ReadStrN(64)
end

SCRAExpelMonsterInfo = SCRAExpelMonsterInfo or BaseClass(BaseProtocolStruct)
function SCRAExpelMonsterInfo:__init()
    self.msg_type = 3015
end
function SCRAExpelMonsterInfo:Decode()
    self.today_times = MsgAdapter.ReadInt()
end

--特卖商店
SCRASaleShopInfo = SCRASaleShopInfo or BaseClass(BaseProtocolStruct)
function SCRASaleShopInfo:__init()
    self.msg_type = 3016
end

function SCRASaleShopInfo:Decode()
    self.sale_shop_count = {}
    for i=0,MsgAdapter.ReadInt() - 1 do
        self.sale_shop_count[i] = MsgAdapter.ReadShort()
    end
end

--欣欣向荣
SCRAHappyUpInfo = SCRAHappyUpInfo or BaseClass(BaseProtocolStruct)
function SCRAHappyUpInfo:__init()
    self.msg_type = 3017
end

function SCRAHappyUpInfo:Decode()
    self.exchange_count = {}
    for i=0,MsgAdapter.ReadInt() - 1 do
        self.exchange_count[i] = MsgAdapter.ReadShort()
    end
end

--福袋信息
SCFortuneBagInfo = SCFortuneBagInfo or BaseClass(BaseProtocolStruct)
function SCFortuneBagInfo:__init()
    self.msg_type = 3020
end

function SCFortuneBagInfo:Decode()
    self.is_buy_flag = MsgAdapter.ReadInt()    --福袋每天买的档位
    self.is_fetch_reward = MsgAdapter.ReadInt()--是否领取每日奖励
    local batch_end_time = MsgAdapter.ReadUInt()--一键购买结束时间
    if batch_end_time > 0 then
		batch_end_time = batch_end_time - TimeHelper:GetServerTime()
	end
    self.batch_end_time = batch_end_time
end

CSColorfulIllustrateSeq = CSColorfulIllustrateSeq or BaseClass(BaseProtocolStruct)
function CSColorfulIllustrateSeq:__init()
    self.msg_type = 3021
end

function CSColorfulIllustrateSeq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteChar(self.op_type)
    MsgAdapter.WriteChar(self.index)
    for i=0,6 do
        for j=0,2 do
            MsgAdapter.WriteChar(self.pigment_list[i][j] or 0)
        end
    end
    MsgAdapter.WriteChar(0)
    MsgAdapter.WriteStrN(self.save_string,144)
end

--欢乐寻宠
SCRAJoySeeksFavorsInfo = SCRAJoySeeksFavorsInfo or BaseClass(BaseProtocolStruct)
--匹配状态
function SCRAJoySeeksFavorsInfo:__init()
    self.msg_type = 3024
end
function SCRAJoySeeksFavorsInfo:Decode()
    self.is_match = MsgAdapter.ReadInt()--0未匹配1匹配中
end
SCRAJoySeeksFavorsMatchSucc = SCRAJoySeeksFavorsMatchSucc or BaseClass(BaseProtocolStruct)
--通知倒数
function SCRAJoySeeksFavorsMatchSucc:__init()
    self.msg_type = 3025
end
function SCRAJoySeeksFavorsMatchSucc:Decode()
    
end
CSRAJoySeeksFavorsMatchReq = CSRAJoySeeksFavorsMatchReq or BaseClass(BaseProtocolStruct)
--请求匹配
function CSRAJoySeeksFavorsMatchReq:__init()
    self.msg_type = 3026
end
function CSRAJoySeeksFavorsMatchReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.is_match)--0取消1匹配
end

--欢乐寻宠
local function BGNetID()
    return {
        gateway_index = MsgAdapter.ReadShort(),
        net_id = MsgAdapter.ReadUShort(),
    }
end
local function SpecialBattleRoleHandInfo()
    return {
        role_id = MsgAdapter.ReadInt(),
        bg_net_id = BGNetID(),
        role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        avatar_type = MsgAdapter.ReadShort(),--头像
        find_pet = MsgAdapter.ReadChar(),   --道具鸡
        find_item = MsgAdapter.ReadChar(),  --道具物品
        avatar_timestamp = MsgAdapter.ReadLL(),--自定义头像
    }
end
local function JoySeeksFavorsMapPoint()
    return {
        is_open = MsgAdapter.ReadChar(),
        joy_index = MsgAdapter.ReadChar(),
        joy_seq = MsgAdapter.ReadShort()
    }
end

local function JoySeeksFavorsMap()
    local t = {}
    t.map_index = MsgAdapter.ReadInt()
    t.role_info = SpecialBattleRoleHandInfo()
    t.player_joy_map = {}
    for i=1,5 do
        t.player_joy_map[i] = {}
        for j=1,5 do
            t.player_joy_map[i][j] = JoySeeksFavorsMapPoint()
        end
    end
    return t
end
--特殊战斗下发
SCBattleSpecialInfo = SCBattleSpecialInfo or BaseClass(BaseProtocolStruct)
function SCBattleSpecialInfo:__init()
    self.msg_type = 11013
end
function SCBattleSpecialInfo:Decode()
    self.ready_tamp = MsgAdapter.ReadLL()
    self.myself_map = JoySeeksFavorsMap()
    self.enemy_map = JoySeeksFavorsMap()
end
SCBattleSpecialOneMapChangeInfo = SCBattleSpecialOneMapChangeInfo or BaseClass(BaseProtocolStruct)
function SCBattleSpecialOneMapChangeInfo:__init()
    self.msg_type = 11014
end
function SCBattleSpecialOneMapChangeInfo:Decode()
    self.show_tamp = MsgAdapter.ReadLL()--展示时间
    self.next_move_tamp = MsgAdapter.ReadLL()--本回合结束时间错
    self.has_find = MsgAdapter.ReadChar()--[0-2]无找到了被找到了
    self.reserve_ch = MsgAdapter.ReadChar()
    self.joy_seq = MsgAdapter.ReadShort()--[0-4]无小中大
    self.map_info = JoySeeksFavorsMap()
end

CSBattleSpecialJoyReqInfo = CSBattleSpecialJoyReqInfo or BaseClass(BaseProtocolStruct)
function CSBattleSpecialJoyReqInfo:__init()
    self.msg_type = 11016
end
function CSBattleSpecialJoyReqInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.operate_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
    MsgAdapter.WriteInt(self.param4)
    MsgAdapter.WriteInt(self.param5)
end

-- 神兽径赛场景信息下发
SCRAAnimalRaceInfo = SCRAAnimalRaceInfo or BaseClass(BaseProtocolStruct)
function SCRAAnimalRaceInfo:__init()
    self.msg_type = 3018
end

function SCRAAnimalRaceInfo:Decode()
    self.ar_info = {
        info_type = MsgAdapter.ReadInt(),
        timestamp = MsgAdapter.ReadLL(),
        -- win_animal_race_num = MsgAdapter.ReadInt()     -- 本轮获胜的神兽序号[1,4]
        rank_list = {},         -- 本轮获胜的神兽序号[1,4]
    }
    for i = 1, AnimalRaceConfig.MAX_ANIMAL_RACE_NUM do
        table.insert(self.ar_info.rank_list, MsgAdapter.ReadChar())
    end
    self.count = MsgAdapter.ReadInt()                   -- 根据这个数量接收
    self.roles = {}                               -- 场景玩家形象 20
    for i = 1, self.count do
        table.insert(self.roles, {
            role_id = MsgAdapter.ReadInt(),
            role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
            obj_id = MsgAdapter.ReadUShort(),
            level = MsgAdapter.ReadShort(),
            behavior_state = MsgAdapter.ReadChar(),
            vip_level = MsgAdapter.ReadChar(),
            avatar_type = MsgAdapter.ReadShort(),
            profession = MsgAdapter.ReadInt(),

            appearance = Appearance(),

            move_speed = OBJ_BASE_MOVE_SPEED * (100 + MsgAdapter.ReadInt()) / 100,
            pos_x = MsgAdapter.ReadShort(),
            pos_y = MsgAdapter.ReadShort(),
            distance = MsgAdapter.ReadFloat(),
            dir_y = Mathf.Rad2Deg * (MsgAdapter.ReadFloat() or 0) + 180,
            scene_id = MsgAdapter.ReadInt(),
            escort_id = MsgAdapter.ReadInt(),
            guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN),

            is_leader = MsgAdapter.ReadChar(),
            is_in_team = MsgAdapter.ReadChar() == 1,
            title_id = MsgAdapter.ReadUShort(),
        })
    end
end

-- 神兽径赛玩家信息下发
SCRAAnimalRaceRoleInfo = SCRAAnimalRaceRoleInfo or BaseClass(BaseProtocolStruct)
function SCRAAnimalRaceRoleInfo:__init()
    self.msg_type = 3019
end

function SCRAAnimalRaceRoleInfo:Decode()
    local t = {
        bet_type_count = {},                -- 玩家个人对于神兽的押注情况
        everything_bet_type_count = {},     -- 全服此时每个神兽的押注情况
        type_response_rate = {},            -- 此时每个神兽的回报率
    }
    for i = 1, AnimalRaceConfig.MAX_ANIMAL_RACE_NUM do
        table.insert(t.bet_type_count, MsgAdapter.ReadShort())
    end
    for i = 1, AnimalRaceConfig.MAX_ANIMAL_RACE_NUM do
        table.insert(t.everything_bet_type_count, MsgAdapter.ReadInt())
    end
    for i = 1, AnimalRaceConfig.MAX_ANIMAL_RACE_NUM do
        table.insert(t.type_response_rate, MsgAdapter.ReadInt())
    end
    self.detail_info = t
end

-- 神兽径赛动画数据下发
SCRAAnimalRaceAnimationInfo = SCRAAnimalRaceAnimationInfo or BaseClass(BaseProtocolStruct)
function SCRAAnimalRaceAnimationInfo:__init()
    self.msg_type = 3037
end

function SCRAAnimalRaceAnimationInfo:Decode()
    self.rank_list = {}
    self.anim_list = {}
    for i = 1, AnimalRaceConfig.MAX_ANIMAL_RACE_NUM do
        self.rank_list[i] = MsgAdapter.ReadChar()           -- 默认下标0为1号神兽 按顺序往下 排名为[1,4]不可重复
    end
    for i = 1, AnimalRaceConfig.MAX_ANIMAL_RACE_NUM do
        self.anim_list[i] = {}
        for j = 1, AnimalRaceConfig.RaceMaxTime do
            local distance = MsgAdapter.ReadUChar()    -- 神兽每一秒的移动距离 排序同上
            self.anim_list[i][j] = (self.anim_list[i][j-1] or 0) + distance
        end
    end
end

--天降好礼
SCRAHeavenGiftInfo = SCRAHeavenGiftInfo or BaseClass(BaseProtocolStruct)
function SCRAHeavenGiftInfo:__init()
    self.msg_type = 3023
end

function SCRAHeavenGiftInfo:Decode()
    self.is_refresh_box = MsgAdapter.ReadShort()
    self.receive_count = MsgAdapter.ReadShort()
end

--联动好礼
SCRALinkageGiftInfo = SCRALinkageGiftInfo or BaseClass(BaseProtocolStruct)
function SCRALinkageGiftInfo:__init()
    self.msg_type = 3028
end

function SCRALinkageGiftInfo:Decode()
    self.gift_seq = MsgAdapter.ReadShort()
    self.fetch_flag = MsgAdapter.ReadShort()    --0 未达成不可领取，1 已达成未领取，2 已领取
    self.can_receive_tamestamp = MsgAdapter.ReadUInt()  --可领取时间戳
end

SCRaCollectWordInfo = SCRaCollectWordInfo or BaseClass(BaseProtocolStruct)
function SCRaCollectWordInfo:__init()
    self.msg_type = 3029
end
function SCRaCollectWordInfo:Decode()
    self.day_gift_is_fetch = MsgAdapter.ReadShort() --0未领取1已领取
    MsgAdapter.ReadShort()
    self.word_info = {}
    for i = 1, RA_COLLECT_WORD_MAX_ENTRY_NUM do
        self.word_info[i] = MsgAdapter.ReadUShort()
    end
    self.total_info = {}
    for i = 1, RA_COLLECT_WORD_MAX_ENTRY_NUM do
        self.total_info[i] = MsgAdapter.ReadUShort()
    end
end

SCRADiscountCarnivalInfo = SCRADiscountCarnivalInfo or BaseClass(BaseProtocolStruct)
function SCRADiscountCarnivalInfo:__init()
    self.msg_type = 3030
end
function SCRADiscountCarnivalInfo:Decode()
    self.buy_flag = {}
    local index = 0
    for i = 1,16 do
        self.buy_flag[index] = MsgAdapter.ReadUChar()
        index = index + 1
    end
end

SCRaAdventureGiftInfo = SCRaAdventureGiftInfo or BaseClass(BaseProtocolStruct)
function SCRaAdventureGiftInfo:__init()
    self.msg_type = 3035
end

function SCRaAdventureGiftInfo:Decode()
    self.gift_seq = MsgAdapter.ReadInt()      --当前礼包，配置表
    self.end_time = MsgAdapter.ReadUInt()      --当前时间大于end_time礼包就无效，或end_time==0
end

SCRAGroupDiscountInfo = SCRAGroupDiscountInfo or BaseClass(BaseProtocolStruct)

function SCRAGroupDiscountInfo:__init()
    self.msg_type = 3036
end

function SCRAGroupDiscountInfo:Decode()
    self.commodity_info = {}
    local count = MsgAdapter.ReadInt()
    for i=0,count-1 do
        local info = {}
        info.discount = MsgAdapter.ReadChar()
        info.buy_count = MsgAdapter.ReadUChar()
        info.all_buy_count = MsgAdapter.ReadShort()
        self.commodity_info[i] = info
    end
end

--幸运转盘
SCRALuckyTurntableInfo = SCRALuckyTurntableInfo or BaseClass(BaseProtocolStruct)
function SCRALuckyTurntableInfo:__init()
    self.msg_type = 3032
end

function SCRALuckyTurntableInfo:Decode()
    local info = {}
    info.last_num = MsgAdapter.ReadInt()         --剩余次数
    info.flag = bit:d2b(MsgAdapter.ReadInt())    --每个条件任务领取标识
    info.progress = {}
    for i=1,RA_LUCKY_TURNTABLE_TASK_MAX_NUM do
        info.progress[i] = MsgAdapter.ReadChar() --每个条件任务的进度
    end
    self.info = info
end

--幸运转盘转盘抽奖
SCRALuckyTurntableReward = SCRALuckyTurntableReward or BaseClass(BaseProtocolStruct)
function SCRALuckyTurntableReward:__init()
    self.msg_type = 3033
end

function SCRALuckyTurntableReward:Decode()
    self.index = MsgAdapter.ReadShort()
    self.type = MsgAdapter.ReadShort()  --0,非跳过动画;1，路过动画
end

-- 剑胆琴心信息下发
SCRAJianDanQinXinInfo = SCRAJianDanQinXinInfo or BaseClass(BaseProtocolStruct)
function SCRAJianDanQinXinInfo:__init()
    self.msg_type = 3027
end

function SCRAJianDanQinXinInfo:Decode()
    local t = {}
    t.open_day = MsgAdapter.ReadUInt()
    t.flag = MsgAdapter.ReadInt()
    self.info = t
    self.flag = bit:d2b(t.flag)
end

--幸运折扣
SCRALuckyDiscountInfo = SCRALuckyDiscountInfo or BaseClass(BaseProtocolStruct)
function SCRALuckyDiscountInfo:__init()
    self.msg_type = 3031
end

function SCRALuckyDiscountInfo:Decode()
    self.discount = MsgAdapter.ReadInt()
    self.buy_count = {}
    for i=0,RA_LUCKY_DISCOUNT_MAX_NUM - 1 do
        self.buy_count[i] = MsgAdapter.ReadChar()
    end
    self.today_buy_count = {}
    for i=0,RA_LUCKY_DISCOUNT_MAX_NUM - 1 do
        self.today_buy_count[i] = MsgAdapter.ReadChar()
    end
end

--图鉴夺宝
SCRAIllustrateTreasureInfo = SCRAIllustrateTreasureInfo or BaseClass(BaseProtocolStruct)
function SCRAIllustrateTreasureInfo:__init()
    self.msg_type = 3038
end

--尝试新的写法
function SCRAIllustrateTreasureInfo:Decode()
    local info = {}
    info.end_timestamp = MsgAdapter.ReadUInt()
    info.login_day = MsgAdapter.ReadInt()
    info.day_fetch_flag = bit:d2b(MsgAdapter.ReadInt())
    info.day_fetch_num = DataHelper.GetFetchFlagNum(info.day_fetch_flag)
    info.task_list = {}
    info.count = MsgAdapter.ReadInt()
    local num = 0
    for i=1,info.count do
        local item  = {}
        item.seq = MsgAdapter.ReadChar()
        item.is_fetch = MsgAdapter.ReadChar()
        item.count = MsgAdapter.ReadShort()
        info.task_list[item.seq] = item
        if item.is_fetch == 1 then
            num = num + 1
        end
    end
    info.is_task_end = num == info.count
    self.info = info
end

--封神战令数据下发
RA_GOD_TOKEN_DAILY_TASK_NUM = 6
RA_GOD_TOKEN_WEEKLY_TASK_NUM = 10
SCRAGodToKenInfo = SCRAGodToKenInfo or BaseClass(BaseProtocolStruct)
function SCRAGodToKenInfo:__init()
    self.msg_type = 3039
end

function SCRAGodToKenInfo:Decode()
    self.time_seq = MsgAdapter.ReadChar()
    self.is_open = MsgAdapter.ReadChar()
    self.is_buy_token = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    self.token_level = MsgAdapter.ReadShort()
    self.token_exp = MsgAdapter.ReadShort()

    self.time_level = MsgAdapter.ReadInt()

    self.fetch_common_flag = MsgAdapter.IntFlag(2)
    self.fetch_senior_flag = MsgAdapter.IntFlag(2)
    ---任务相关
    self.daily_level = MsgAdapter.ReadShort()
    
    self.daily_type_t = {}                 --每日任务类型
    for i=1,RA_GOD_TOKEN_DAILY_TASK_NUM do
        self.daily_type_t[i] = MsgAdapter.ReadChar()
    end
    self.daily_type_finish_count = {}  -- 每日任务完成次数 -1 表示已经领取
    for i=1,RA_GOD_TOKEN_DAILY_TASK_NUM do
        local index = self.daily_type_t[i]
        self.daily_type_finish_count[index] = {count = MsgAdapter.ReadInt() , index = i - 1}
    end

    self.weekly_level = MsgAdapter.ReadShort()
    self.weekly_type_t = {}  --每周任务类型
    for i=1,RA_GOD_TOKEN_WEEKLY_TASK_NUM do
        self.weekly_type_t[i] = MsgAdapter.ReadChar()
    end
    self.weekly_type_finish_count = {} -- 每周任务完成次数 -1 表示已经领取
    for i=1,RA_GOD_TOKEN_WEEKLY_TASK_NUM do
        local index = self.weekly_type_t[i]
        self.weekly_type_finish_count[index] = {count = MsgAdapter.ReadInt() , index = i - 1}
    end

    self.surplus_exp = MsgAdapter.ReadShort()  --溢出经验
    self.surplus_receive_count = MsgAdapter.ReadShort() --溢出可领取次数

    self.count = MsgAdapter.ReadInt()
    self.exchange_count = {}                    --兑换次数，下标对应配置的seq
    for i=0,self.count-1 do
        self.exchange_count[i] = MsgAdapter.ReadInt()
    end
end

--元素翻牌
SCRAElementCardsInfo = SCRAElementCardsInfo or BaseClass(BaseProtocolStruct)
function SCRAElementCardsInfo:__init()
    self.msg_type = 3040
    self.info = {}
end

function SCRAElementCardsInfo:Decode()
    self.info.flop_seq = MsgAdapter.ReadShort() + 1
    self.info.flop_count = MsgAdapter.ReadShort()
end

--连登好礼
SCRASignInContinuityInfo = SCRASignInContinuityInfo or BaseClass(BaseProtocolStruct)
function SCRASignInContinuityInfo:__init()
    self.msg_type = 3041
    self.info = {}
    self.info.login_day = 0
    self.info.reward_flag_t = {}
end

function SCRASignInContinuityInfo:Decode()
    self.info.login_day = MsgAdapter.ReadInt()
    self.info.reward_flag_t = bit:d2b(MsgAdapter.ReadUInt())
end

--赠30连抽
SCRAGivePetDrawInfo = SCRAGivePetDrawInfo or BaseClass(BaseProtocolStruct)
function SCRAGivePetDrawInfo:__init()
    self.msg_type = 3042
    self.give_mark = 1

end

function SCRAGivePetDrawInfo:Decode()
    self.give_mark = MsgAdapter.ReadInt()   --0：未领取，1：已领取
end

CSRandActivityMakeUpReq = CSRandActivityMakeUpReq or BaseClass(BaseProtocolStruct)
function CSRandActivityMakeUpReq:__init()
    self.msg_type = 3045
end

function CSRandActivityMakeUpReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.act_type)
    MsgAdapter.WriteShort(self.make_type)
end

----------------------------------------------------------
--额外奖励
----------------------------------------------------------
CSRandActivityBonusOperaReq = CSRandActivityBonusOperaReq or BaseClass(BaseProtocolStruct)
function CSRandActivityBonusOperaReq:__init()
    self.msg_type = 3050
end

function CSRandActivityBonusOperaReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.rand_activity_type)
	MsgAdapter.WriteShort(self.opera_type)
	MsgAdapter.WriteInt(self.param_1)
end

SCRandActivityBonusInfo = SCRandActivityBonusInfo or BaseClass(BaseProtocolStruct)
function SCRandActivityBonusInfo:__init()
    self.msg_type = 3051
end

function SCRandActivityBonusInfo:Decode()
    self.bonus_flag = MsgAdapter.IntFlag()
end

----------------------------------------------------------
--辞旧迎新
----------------------------------------------------------
SCRandActivityCJYXInof = SCRandActivityCJYXInof or BaseClass(BaseProtocolStruct)
function SCRandActivityCJYXInof:__init()
    self.msg_type = 3052
end

function SCRandActivityCJYXInof:Decode()
    self.today_answer = MsgAdapter.ReadInt()
    self.id = MsgAdapter.ReadInt()
    self.has_answer_count = MsgAdapter.ReadInt()
    self.has_answer_list = {}
    for i = 1,self.has_answer_count do 
        self.has_answer_list[i] = MsgAdapter.ReadInt()
    end 
end

----------------------------------------------------------
--牛行大运
----------------------------------------------------------
SCRandActivityNXDYInof = SCRandActivityNXDYInof or BaseClass(BaseProtocolStruct)
function SCRandActivityNXDYInof:__init()
    self.msg_type = 3053
end

function SCRandActivityNXDYInof:Decode()
    self.today_reward = MsgAdapter.ReadInt()
    self.id = MsgAdapter.ReadInt()
end

--牛运商城
MAX_LOG_TIMES = 20
SCRACowStoreInfo = SCRACowStoreInfo or BaseClass(BaseProtocolStruct)
function SCRACowStoreInfo:__init()
    self.msg_type = 3059
end

function SCRACowStoreInfo:Decode()
	self.open_day = MsgAdapter.ReadInt()
    self.buy_day_times = {}         -- 当日购买的次数
    self.buy_sum_times = {}         -- 总购买次数
    for i=0,MAX_LOG_TIMES - 1 do
        self.buy_day_times[i] = MsgAdapter.ReadUShort()
    end
    for i=0,MAX_LOG_TIMES - 1 do
        self.buy_sum_times[i] = MsgAdapter.ReadUShort()
    end
end

--牛运无边
SCRACowLuckyExchangeInfo = SCRACowLuckyExchangeInfo or BaseClass(BaseProtocolStruct)
function SCRACowLuckyExchangeInfo:__init()
    self.msg_type = 3060
end

function SCRACowLuckyExchangeInfo:Decode()
	self.history_chongzhi_num = MsgAdapter.ReadLL()
	self.initmacy =  MsgAdapter.ReadInt()
end

----------------------------------------------------------
--年货节
----------------------------------------------------------
SCRASpringFestivalStoreInfo = SCRASpringFestivalStoreInfo or BaseClass(BaseProtocolStruct)
function SCRASpringFestivalStoreInfo:__init()
    self.msg_type = 3054
end

function SCRASpringFestivalStoreInfo:Decode()
    self.info = {}
    self.info.open_day = MsgAdapter.ReadInt()
    self.info.phase = MsgAdapter.ReadInt()
    self.info.today_buy_times = {}
    for i = 1, 20 do
        self.info.today_buy_times[i] = MsgAdapter.ReadShort()
    end
    self.info.activity_buy_times = {}
    for i = 1, 20 do
        self.info.activity_buy_times[i] = MsgAdapter.ReadShort()
    end
end

-- 牛运当头活动
SCRAMonpolyGiftInfo = SCRAMonpolyGiftInfo or BaseClass(BaseProtocolStruct)
function SCRAMonpolyGiftInfo:__init()
    self.msg_type = 3055
end
function SCRAMonpolyGiftInfo:Decode()
    self.info = {}
    self.info.m_shoe_tamp = MsgAdapter.ReadUInt()               -- 展示时间戳
    self.info.now_map_id = MsgAdapter.ReadInt()                 -- 地图id
    self.info.old_map_index = MsgAdapter.ReadInt() + 1             -- 原点数（-1则是无原点，通用下发才会这样）
    self.info.now_map_index = MsgAdapter.ReadInt() + 1             -- 当前所在的点
    self.info.rand_point = MsgAdapter.ReadInt()                 -- 随机点数值
    --self.info.m_dice_times = MsgAdapter.ReadShort()             -- 可随机次数
    self.info.has_play_special_plan = MsgAdapter.ReadInt()    -- 特殊玩法是否执行过了（奖励是否领取过了）
    self.convert_infos = {}
    -- 服务端最大记录种类 = 20
    for i = 0,19 do
        self.convert_infos[i] = MsgAdapter.ReadShort()
    end
end

-- 牛运当头答题下发
SCRAMonpolyGiftQuestionInfo = SCRAMonpolyGiftQuestionInfo or BaseClass(BaseProtocolStruct)
function SCRAMonpolyGiftQuestionInfo:__init()
    self.msg_type = 3057
end

function SCRAMonpolyGiftQuestionInfo:Decode()
    self.question_id = MsgAdapter.ReadInt()
end

-- 牛运兑换
SCRANCowConvertInfo = SCRANCowConvertInfo or BaseClass(BaseProtocolStruct)
function SCRANCowConvertInfo:__init()
    self.msg_type = 3058
end

function SCRANCowConvertInfo:Decode()
    self.unlock_integral = MsgAdapter.ReadInt()
    self.score = MsgAdapter.ReadInt()
    self.today_buy_times = {}
    for i = 0 , 19 do
        self.today_buy_times[i] = MsgAdapter.ReadShort()
    end
    self.act_buy_times = {}
    for i = 0 , 19 do
        self.act_buy_times[i] = MsgAdapter.ReadShort()
    end
end

--小年送礼
SCRALittleYearGiftInfo = SCRALittleYearGiftInfo or BaseClass(BaseProtocolStruct)
function SCRALittleYearGiftInfo:__init()
    self.msg_type = 3061
end

function SCRALittleYearGiftInfo:Decode()
    self.thread_id = MsgAdapter.ReadInt()      -- 线索id
    self.has_get_reward = MsgAdapter.ReadInt() -- 是否已经领取标志了
end

-- 宠物夺宝
SCRAPetLootInfo = SCRAPetLootInfo or BaseClass(BaseProtocolStruct)
function SCRAPetLootInfo:__init()
    self.msg_type = 3062
end

function SCRAPetLootInfo:Decode()
    self.open_day = MsgAdapter.ReadInt()    -- 这个活动开启的时候是开服第几天
    self.info = {}
    local task_info = {}
    for i = 1, 15 do
        task_info[i] = {}
        task_info[i].param = MsgAdapter.ReadInt()   -- 任务参数
        task_info[i].can_get_reward_times = MsgAdapter.ReadShort()     -- 可领取次数
        task_info[i].has_get_reward_times = MsgAdapter.ReadShort()     -- 已领取次数
    end
    self.info.task_info = task_info
    self.info.task_finish_times = MsgAdapter.ReadInt()       -- 任务次数已完成几个
    self.info.task_final_reward_flag = MsgAdapter.ReadInt()  -- 二进制累计奖励标志位
    local final_flag = {}
    local t = bit:d2b(self.info.task_final_reward_flag)
    for i = 1, 32 do
        final_flag[i] = t[33 - i]
    end
    self.info.final_flag = final_flag
end

----------------------------------------------------------
--交易狂欢日
----------------------------------------------------------
SCRATradeMarketReturnGoldInfo = SCRATradeMarketReturnGoldInfo or BaseClass(BaseProtocolStruct)
function SCRATradeMarketReturnGoldInfo:__init()
    self.msg_type = 3063
end

function SCRATradeMarketReturnGoldInfo:Decode()
    local info = {}
    info.total_buy_item_consume_gold = MsgAdapter.ReadLL() --总计购买商品消耗元宝数
    info.total_calaimable_gold = MsgAdapter.ReadLL()       --总计返还元宝数
    info.remain_calaimable_gold = MsgAdapter.ReadLL()      --剩余可领取元宝数
    self.deal_discount_info = info
end

TradeMarketReturnHotItemStruct = TradeMarketReturnHotItemStruct or {}

function TradeMarketReturnHotItemStruct.Read()
    local stu = {}
    stu.item_id = MsgAdapter.ReadUShort()
    stu.param_length = MsgAdapter.ReadShort()
    if stu.param_length > 0 then
        stu.param = ItemDataParam(stu.item_id,stu.param_length)
        stu.param.item_id = stu.item_id
    else
        stu.param = {item_id = stu.item_id}
    end
    return stu
end

SCRATradeMarketReturnHotItemInfo = SCRATradeMarketReturnHotItemInfo or BaseClass(BaseProtocolStruct)
function SCRATradeMarketReturnHotItemInfo:__init()
    self.msg_type = 3064
end

function SCRATradeMarketReturnHotItemInfo:Decode()
    self.item_list = {}
    for i=1,6 do
        self.item_list[i] = TradeMarketReturnHotItemStruct.Read()
    end
end

-- 活动号 2134
-- 连连乐翻天
SCRAHappyLookInfo = SCRAHappyLookInfo or BaseClass(BaseProtocolStruct)
function SCRAHappyLookInfo:__init()
    self.msg_type = 3065
end

function SCRAHappyLookInfo:Decode()
    self.ra_frequency_num = MsgAdapter.ReadShort() 
    self.chanllenge_index = MsgAdapter.ReadChar() 
    self.challenge_is_succ = MsgAdapter.ReadChar() 
    self.challenge_end_timestamp = MsgAdapter.ReadInt() 
end

-- 好礼转动下发
SCRAGoodGiftTurnInfo = SCRAGoodGiftTurnInfo or BaseClass(BaseProtocolStruct)
function SCRAGoodGiftTurnInfo:__init()
    self.msg_type = 3070
end

function SCRAGoodGiftTurnInfo:Decode()
    self.fetch_num = MsgAdapter.ReadShort()  -- 剩余领取次数
    self.fetch_flag = MsgAdapter.ReadShort() -- 领取奖励标识（二进制）
    self.fetch_flags = DataHelper.ReverseList(bit:d16b(self.fetch_flag))
end

-- 好礼转动抽取到的礼盒下发
SCRAGoodGiftTurnRewardInfo = SCRAGoodGiftTurnRewardInfo or BaseClass(BaseProtocolStruct)
function SCRAGoodGiftTurnRewardInfo:__init()
    self.msg_type = 3071
end

function SCRAGoodGiftTurnRewardInfo:Decode()
    self.info = {}
    self.info.now_get_seq1 = MsgAdapter.ReadInt()   -- 抽到对应配置的seq1
end
-- 幸运迷宫
SCRALuckyMazeInfo = SCRALuckyMazeInfo or BaseClass(BaseProtocolStruct)
function SCRALuckyMazeInfo:__init()
    self.msg_type = 3617
end

function SCRALuckyMazeInfo:Decode()
    self.info = {}
    self.info.open_day = MsgAdapter.ReadInt()
    self.info.begin_time = MsgAdapter.ReadUInt()
    self.info.rand_count = MsgAdapter.ReadInt()  -- 可随机翻牌的次数
    self.info.today_count = MsgAdapter.ReadInt() -- 今日翻牌了多少次
    self.info.can_enter_fb = MsgAdapter.ReadChar() -- 是否可进入副本
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    self.info.task_info = {}
    for i = 0,14 do
        self.info.task_info[i] = MsgAdapter.ReadInt()
    end
end
-- 幸运迷宫拼图信息
SCRALuckyMazePuzzleInfo = SCRALuckyMazePuzzleInfo or BaseClass(BaseProtocolStruct)
function SCRALuckyMazePuzzleInfo:__init()
    self.msg_type = 3618
end

function SCRALuckyMazePuzzleInfo:Decode()
    self.info = {}
    self.info.mark_index = MsgAdapter.ReadInt() -- 当前操作的大图id
    self.info.puzzle_info = {}
    for i = 0,6 do
        self.info.puzzle_info[i] = {}
        for j = 1,9 do
            self.info.puzzle_info[i][j] = MsgAdapter.ReadInt()
        end
    end
end

-- 幸运迷宫宝箱信息
SCRALuckyMazeBoxInfo = SCRALuckyMazeBoxInfo or BaseClass(BaseProtocolStruct)
function SCRALuckyMazeBoxInfo:__init()
    self.msg_type = 3616
end
function SCRALuckyMazeBoxInfo:Decode()
    self.box_num = MsgAdapter.ReadInt()
end
-- 随机活动-义胆助人信息下发
SCRAHelpOthersInfo = SCRAHelpOthersInfo or BaseClass(BaseProtocolStruct)
function SCRAHelpOthersInfo:__init()
    self.msg_type = 3072
end

function SCRAHelpOthersInfo:Decode()
    self.info = {
        pass_count = MsgAdapter.ReadShort(),
        task_id = MsgAdapter.ReadShort(),           -- 任务id
        task_status = MsgAdapter.ReadShort(),       -- 任务状态
        progress = MsgAdapter.ReadShort(),          -- 打怪任务类型时需要
    }
end

-- 随机活动-桃园一三事信息下发
SCRAHelpOthersInfo2 = SCRAHelpOthersInfo2 or BaseClass(BaseProtocolStruct)
function SCRAHelpOthersInfo2:__init()
    self.msg_type = 3091
end

function SCRAHelpOthersInfo2:Decode()
    self.info = {
        pass_count = MsgAdapter.ReadShort(),
        task_id = MsgAdapter.ReadShort(),           -- 任务id
        task_status = MsgAdapter.ReadShort(),       -- 任务状态
        progress = MsgAdapter.ReadShort(),          -- 打怪任务类型时需要
    }
end

-- 随机活动-特惠秘宝下发
SCRASecretTreasureInfo = SCRASecretTreasureInfo or BaseClass(BaseProtocolStruct)
function SCRASecretTreasureInfo:__init()
    self.msg_type = 3073
end

function SCRASecretTreasureInfo:Decode()
    self.phase = MsgAdapter.ReadShort()
    self.is_open_today_flag = MsgAdapter.ReadShort()
    self.today_buy_times = {}
    for i = 1,25 do 
        self.today_buy_times[i] =  MsgAdapter.ReadShort()
    end 
    self.activity_buy_times = {}
    for i = 1,100 do 
        self.activity_buy_times[i] =  MsgAdapter.ReadShort()
    end
end

--劳动好礼
SCRALaoDongHaoLiInfo = SCRALaoDongHaoLiInfo or BaseClass(BaseProtocolStruct)
function SCRALaoDongHaoLiInfo:__init()
    self.msg_type = 3074
end

function SCRALaoDongHaoLiInfo:Decode()
    self.is_refresh_box = MsgAdapter.ReadShort()
    self.receive_count = MsgAdapter.ReadShort()
end

--初夏历练
SCRASummerLiLianAllInfo = SCRASummerLiLianAllInfo or BaseClass(BaseProtocolStruct)
function SCRASummerLiLianAllInfo:__init()
    self.msg_type = 3075
end

function SCRASummerLiLianAllInfo:Decode()
    self.open_day = MsgAdapter.ReadShort()  --活动开启第几天
    self.is_fetch_all_pass_flag = MsgAdapter.ReadChar() --是否领取全首通宝箱，0：未，1：已
    MsgAdapter.ReadChar()
    self.first_flag = MsgAdapter.IntFlag(1, true)    --boss是否首杀，0：未，1：已
    table.remove(self.first_flag, 1)        --第一位不要
end

-- 多彩花灯基础信息
SCRAFlowerLampBaseInfo = SCRAFlowerLampBaseInfo or BaseClass(BaseProtocolStruct)
function SCRAFlowerLampBaseInfo:__init()
    self.msg_type = 3076
end

function SCRAFlowerLampBaseInfo:Decode()
    self.info = {}
    self.info.last_score = MsgAdapter.ReadShort()       -- 上一场获得的积分
    self.info.day_max_score = MsgAdapter.ReadShort()    -- 今天单次最大的积分
    self.info.act_max_score = MsgAdapter.ReadShort()    -- 活动内单次最大的积分
    self.info.day_join_count = MsgAdapter.ReadShort()   -- 今日参与次数
    self.info.act_join_count = MsgAdapter.ReadShort()   -- 活动参与次数
    self.info.day_score = MsgAdapter.ReadShort()        -- 今日获得的总分
    self.info.sum_score = MsgAdapter.ReadInt()          -- 活动内活动的总分
    self.info.task_flags = MsgAdapter.IntFlag(1,true)        -- 任务奖励领取标记
end

-- 多彩花灯游戏信息下发
SCRAFlowerLampGameInfo = SCRAFlowerLampGameInfo or BaseClass(BaseProtocolStruct)
function SCRAFlowerLampGameInfo:__init()
    self.msg_type = 3077
end

function SCRAFlowerLampGameInfo:Decode()
    self.info = {}
    self.info.notice_type = MsgAdapter.ReadShort()  -- FlowerLampData.NotiveType
    self.info.score = MsgAdapter.ReadShort()        -- 本次游戏内积分
    self.info.timestamp = MsgAdapter.ReadUInt()     -- 本次游戏借宿时间戳
end

-- 多彩花灯花灯信息下发
SCRAFlowerLampItemInfo = SCRAFlowerLampItemInfo or BaseClass(BaseProtocolStruct)
function SCRAFlowerLampItemInfo:__init()
    self.msg_type = 3078
end

function SCRAFlowerLampItemInfo:Decode()
    self.info = {}
    self.info.lamp_list = {}
    for i = 0, 9 do
        self.info.lamp_list[i] = MsgAdapter.ReadShort()     -- 花灯数量
    end
end

-- 好运花签信息下发
SCRALuckySignInfo = SCRALuckySignInfo or BaseClass(BaseProtocolStruct)
function SCRALuckySignInfo:__init()
    self.msg_type = 3080
end

function SCRALuckySignInfo:Decode()
    local info = {}
    self.is_lock = MsgAdapter.ReadChar()  --0签文界面 1 解签界面
    self.reward_flag = MsgAdapter.ReadChar()--0未领取 1已领取
    self.sign_id = MsgAdapter.ReadShort()
    self.reward_type = MsgAdapter.ReadInt()
end

-- 夏日商城信息下发
SCRASummerStoreInfo = SCRASummerStoreInfo or BaseClass(BaseProtocolStruct)
function SCRASummerStoreInfo:__init()
    self.msg_type = 3079
end

function SCRASummerStoreInfo:Decode()
    self.is_need_phase_flag = MsgAdapter.ReadShort()        --重算阶段0：不重算，1：重新算
    self.is_open_today_flag = MsgAdapter.ReadShort()        --今天是否打开界面0：没，1：已打开，目前该字段客户端没用到
    self.today_buy_times = {}                               --今日购买了的次数，根据seq
    for i = 1, 30 do
        self.today_buy_times[i] = MsgAdapter.ReadShort()
    end
    self.activity_buy_times = {}                            --活动购买了的次数，根据index
    for i = 1, 100 do
        self.activity_buy_times[i] = MsgAdapter.ReadShort()
    end
end

-- 夏日商城单个信息下发
SCRASummerStoreSingleInfo = SCRASummerStoreSingleInfo or BaseClass(BaseProtocolStruct)
function SCRASummerStoreSingleInfo:__init()
    self.msg_type = 3082
end

function SCRASummerStoreSingleInfo:Decode()
    self.seq = MsgAdapter.ReadShort()
    self.index = MsgAdapter.ReadShort()
    self.today_buy_times = MsgAdapter.ReadShort()
    self.activity_buy_times = MsgAdapter.ReadShort()
end

-- 超级折扣商城信息下发
SCRASuperOffShopInfo = SCRASuperOffShopInfo or BaseClass(BaseProtocolStruct)
function SCRASuperOffShopInfo:__init()
    self.msg_type = 3081
end

function SCRASuperOffShopInfo:Decode()
    self.is_need_phase_flag = MsgAdapter.ReadShort()        --重算阶段0：不重算，1：重新算
    self.phase_rand_disc_item_list = {} --刷出的折扣商品列表，对应type，type=0的话就是没有
    for i = 1, 25 do
        self.phase_rand_disc_item_list[i] = MsgAdapter.ReadShort()
    end
    self.today_normal_buy_times = {}    --正常货架每日限购
    for i = 1, 27 do
        self.today_normal_buy_times[i] = MsgAdapter.ReadShort()
    end
    self.today_dis_buy_times = {}       --折扣货架每日限购
    for i = 1, 5 do
        self.today_dis_buy_times[i] = MsgAdapter.ReadShort()
    end
    self.activity_normal_buy_times = {}    --正常货架活动限购
    for i = 1, 135 do
        self.activity_normal_buy_times[i] = MsgAdapter.ReadShort()
    end
    self.activity_dis_buy_times = {}       --折扣货架活动限购
    for i = 1, 25 do
        self.activity_dis_buy_times[i] = MsgAdapter.ReadShort()
    end
end

-- 超级折扣商城单个信息下发
SCRASuperOffShopSingleInfo = SCRASuperOffShopSingleInfo or BaseClass(BaseProtocolStruct)
function SCRASuperOffShopSingleInfo:__init()
    self.msg_type = 3083
end

function SCRASuperOffShopSingleInfo:Decode()
    -- 1:正常货架 param1:seq(每日) param2:index(活动)
    -- 2:折扣货架 param1:index(每日) param2:seq(活动)
    self.notice_type = MsgAdapter.ReadInt()
    self.param1 = MsgAdapter.ReadShort()
    self.param2 = MsgAdapter.ReadShort()
    self.today_buy_times = MsgAdapter.ReadShort()
    self.activity_buy_times = MsgAdapter.ReadShort()
end

-- 荷香觅宝
SCRALotusInfo = SCRALotusInfo or BaseClass(BaseProtocolStruct)
function SCRALotusInfo:__init()
    self.msg_type = 3087
end

function SCRALotusInfo:Decode()
    MsgAdapter.ReadShort()
    MsgAdapter.ReadChar()
    self.info = {}
    self.info.end_reward_flag = MsgAdapter.ReadChar()
    self.info.grid_list_active_flag = MsgAdapter.IntFlag(1,1)
end

-- 元气翻牌
SCRAYuanQiTurningInfo = SCRAYuanQiTurningInfo or BaseClass(BaseProtocolStruct)
function SCRAYuanQiTurningInfo:__init()
    self.msg_type = 3084
end

function SCRAYuanQiTurningInfo:Decode()
    self.info = {
        open_index = MsgAdapter.ReadInt(),      -- 记录你开的卡
        info_id = MsgAdapter.ReadInt(),         -- 记录卡牌id
        reward_id = MsgAdapter.ReadInt(),       -- 奖励id
    }
end

-- 凉风送爽

local function ReadBreezeTaskInfo(index)
    return {
        type = MsgAdapter.ReadShort(),          -- 配置type
        task_type = MsgAdapter.ReadShort(),     -- 任务类型
        schedule = MsgAdapter.ReadInt(),        -- 进度
        open_day = MsgAdapter.ReadShort(),      -- 第几天刷新的任务
        refresh_num = MsgAdapter.ReadChar(),    -- 刷新任务次数(仅固定任务)
        reserve_ch = MsgAdapter.ReadChar(),
        index = index,
    }
end

SCRACoolBreezeInfo = SCRACoolBreezeInfo or BaseClass(BaseProtocolStruct)
function SCRACoolBreezeInfo:__init()
    self.msg_type = 3085
end

function SCRACoolBreezeInfo:Decode()
    self.info = {
        last_refresh_fixe_open_day = MsgAdapter.ReadUShort(),           -- 上次刷新固定任务时对应的活动开启天数
        last_refresh_random_open_day = MsgAdapter.ReadUShort(),         -- 上次刷新随机任务时对应的活动开启天数
        next_fixe_task_refresh_timestamp = MsgAdapter.ReadUInt(),       -- 下次固定任务刷新时间戳
        next_random_task_refresh_timestamp = MsgAdapter.ReadUInt(),     -- 下次随机任务刷新时间戳
        fixe_task_list = {},                                            -- 固定任务列表
        random_task_list = {},                                          -- 随机任务列表
    }
    for i = 1, 3 do
        self.info.fixe_task_list[i] = ReadBreezeTaskInfo(i)
    end
    for i = 1, 2 do
        self.info.random_task_list[i] = ReadBreezeTaskInfo(i)
    end
    self.info.task_refresh_timestamp_min = math.min(self.info.next_fixe_task_refresh_timestamp, self.info.next_random_task_refresh_timestamp)
end

SCRACoolBreezeSingleInfo = SCRACoolBreezeSingleInfo or BaseClass(BaseProtocolStruct)
function SCRACoolBreezeSingleInfo:__init()
    self.msg_type = 3086
end

function SCRACoolBreezeSingleInfo:Decode()
    self.index = MsgAdapter.ReadShort() + 1
    self.task_group_type = MsgAdapter.ReadShort()   -- 1:固定任务 2:随机任务
    self.info = ReadBreezeTaskInfo(self.index)
    if CoolBreezeView.play_effect then
        CoolBreezeView.play_effect = false
        self.info.play_effect = true
    end
end

SCRAMiBaoFangDianInfo = SCRAMiBaoFangDianInfo  or BaseClass(BaseProtocolStruct)
function SCRAMiBaoFangDianInfo:__init()
    self.msg_type = 3088
end
function SCRAMiBaoFangDianInfo:Decode()
    self.is_need_recal_phase_flag = MsgAdapter.ReadShort()
    self.is_open_day_flag = MsgAdapter.ReadShort()
    self.open_day_cfg =  MsgAdapter.ReadInt()
    self.today_buy_times = {}
    for i = 0,29 do 
        self.today_buy_times[i] =  MsgAdapter.ReadShort()
    end 
    self.activity_buy_times = {}
    for i = 0,29 do 
        self.activity_buy_times[i] =  MsgAdapter.ReadShort()
    end
end

SCRAMiBaoFangDianSingleInfo = SCRAMiBaoFangDianSingleInfo  or BaseClass(BaseProtocolStruct)  ---觅宝坊店单个信息下发
function SCRAMiBaoFangDianSingleInfo:__init()
    self.msg_type = 3089
end
function SCRAMiBaoFangDianSingleInfo:Decode()
    self.seq = MsgAdapter.ReadShort()
    self.index = MsgAdapter.ReadShort()
    self.today_buy_times = MsgAdapter.ReadShort()
    self.activity_buy_times = MsgAdapter.ReadShort()
end

--==========================灵玉基金兑换==========================
CSFundExchangeReq = CSFundExchangeReq or BaseClass(BaseProtocolStruct)
function CSFundExchangeReq:__init()
    self.msg_type = 3090
end

function CSFundExchangeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.index)
end

SCRASignHappyInfo = SCRASignHappyInfo  or BaseClass(BaseProtocolStruct)  ---登陆连好礼
function SCRASignHappyInfo:__init()
    self.msg_type = 3066
end
function SCRASignHappyInfo:Decode()
    self.login_day = MsgAdapter.ReadInt()
    self.flag = MsgAdapter.ReadInt()
end

--==========================万灵共盈==========================
SCRAWanLingGongYingInfo = SCRAWanLingGongYingInfo or BaseClass(BaseProtocolStruct)
function SCRAWanLingGongYingInfo:__init()
    self.msg_type = 3067
end

function SCRAWanLingGongYingInfo:Decode()
    self.ra_buy_reward_timestamp = MsgAdapter.ReadUInt()
    self.task_list = {}
    for i = 0, RA_WAN_LING_GONG_YING_TASK_MAX - 1 do
        local t = {}
        t.task_flag = MsgAdapter.ReadShort()    --0:未完成 1:已完成 2:已领取
        t.param1 = MsgAdapter.ReadChar()
        t.param2 = MsgAdapter.ReadChar()
        self.task_list[i] = t
    end
end

-- 山海问答
SCRAShanHaiQuestionInfo = SCRAShanHaiQuestionInfo or BaseClass(BaseProtocolStruct)
function SCRAShanHaiQuestionInfo:__init()
    self.msg_type = 3092
end

function SCRAShanHaiQuestionInfo:Decode()
    self.is_can_answer = MsgAdapter.ReadShort()
    self.times = MsgAdapter.ReadShort()
    self.question_id = MsgAdapter.ReadShort()
    self.reward_seq = MsgAdapter.ReadShort()
end

SCRAShanHaiQuestionStateInfo = SCRAShanHaiQuestionStateInfo or BaseClass(BaseProtocolStruct)
function SCRAShanHaiQuestionStateInfo:__init()
    self.msg_type = 3099
end

function SCRAShanHaiQuestionStateInfo:Decode()
    self.is_can_answer = MsgAdapter.ReadInt()
end

-- 山海风月
SCRAShanHaiWindMoonRewardInfo  = SCRAShanHaiWindMoonRewardInfo  or BaseClass(BaseProtocolStruct)
function SCRAShanHaiWindMoonRewardInfo :__init()
    self.msg_type = 3096
end

function SCRAShanHaiWindMoonRewardInfo :Decode()
    self.times = MsgAdapter.ReadInt()
    self.reward_info = {}
    for i = 1, 4 do
        self.reward_info[i] = MsgAdapter.ReadShort()
    end
end

SCRAShanHaiWindMoonStateInfo  = SCRAShanHaiWindMoonStateInfo  or BaseClass(BaseProtocolStruct)
function SCRAShanHaiWindMoonStateInfo :__init()
    self.msg_type = 3097
end

function SCRAShanHaiWindMoonStateInfo :Decode()
    self.is_can_lottery = MsgAdapter.ReadInt()
end

SCRAShanHaiWindMoonLotteryInfo  = SCRAShanHaiWindMoonLotteryInfo  or BaseClass(BaseProtocolStruct)
function SCRAShanHaiWindMoonLotteryInfo :__init()
    self.msg_type = 3098
end

function SCRAShanHaiWindMoonLotteryInfo :Decode()
    self.lottery_info = {}
    for i = 1, 4 do
        self.lottery_info[i] = MsgAdapter.ReadChar() + 1
    end
end

-- 随机活动-魂器天成 角色信息
SCRAHorcruxTianChengRoleInfo = SCRAHorcruxTianChengRoleInfo or BaseClass(BaseProtocolStruct)
function SCRAHorcruxTianChengRoleInfo:__init()
    self.msg_type = 3093
end

function SCRAHorcruxTianChengRoleInfo:Decode()
    local t = {
        shop_list = {},
        exchange_list = {},
        consume_fetch_mark = 0,
        consume_gold = 0,
    }
    for i = 0, 20 - 1 do
        t.shop_list[i] = MsgAdapter.ReadShort()
    end
    for i = 0, 15 - 1 do
        t.exchange_list[i] = MsgAdapter.ReadShort()
    end
    t.consume_fetch_mark = MsgAdapter.ReadShort()
    t.consume_gold = MsgAdapter.ReadInt()
    self.info = t
end

-- 随机活动-魂器天成 奖池信息
SCRAHorcruxTianChengPoolInfo = SCRAHorcruxTianChengPoolInfo or BaseClass(BaseProtocolStruct)
function SCRAHorcruxTianChengPoolInfo:__init()
    self.msg_type = 3094
end

function SCRAHorcruxTianChengPoolInfo:Decode()
    self.info = {
        pool_type = MsgAdapter.ReadShort(), -- 奖池类型 -1是道具 0-16 是货币类型
        item_id = MsgAdapter.ReadUShort(),
        num = MsgAdapter.ReadInt(), 
    }
end

-- 随机活动-魂器天成 抽奖返回
SCRAHorcruxTianChengDrawResult = SCRAHorcruxTianChengDrawResult or BaseClass(BaseProtocolStruct)
function SCRAHorcruxTianChengDrawResult:__init()
    self.msg_type = 3095
end

function SCRAHorcruxTianChengDrawResult:Decode()
    self.info = {
        seq = MsgAdapter.ReadInt(),     -- 转盘 seq
        num = MsgAdapter.ReadInt(),     -- 如果是 奖池奖励 则为奖池数量， 不是奖池奖励则为零
    }
end