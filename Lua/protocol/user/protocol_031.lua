local function ReadRaPaiMaiHangSingleShopItemInfo()
    local t = {}
    t.end_timestamp = MsgAdapter.ReadUInt()      --竞拍结束时间
    t.bid_price = MsgAdapter.ReadInt()          --当前竞拍出价
    t.remain_deposit_gold = MsgAdapter.ReadInt()--剩余寄存元宝
    t.add_price = MsgAdapter.ReadInt()          --加价金额
    t.uid = MsgAdapter.ReadInt()                --当前竞拍的角色uid
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.is_show_user_flag = MsgAdapter.ReadChar() --当前竞拍角色信息是否显示 1:显示
    t.is_big_end_flag = MsgAdapter.ReadChar()   --竞拍是否结算标识 1:已结算
    t.is_auto_bid = MsgAdapter.ReadChar()       --是否自动竞价 1:自动
    t.reserve_ch = MsgAdapter.ReadChar()
    return t
end

local function ReadRAPaiMaiHangUserSingleItemInfo()
    local t = {}
    t.last_bid_gold = MsgAdapter.ReadInt()      --记录上次出价
    t.bid_price = MsgAdapter.ReadInt()          --寄存数量(取回时该值会变化)
    t.add_price = MsgAdapter.ReadInt()          --设置加价金额
    t.retrieve_gold = MsgAdapter.ReadInt()      --已取回数量
    t.is_show_user_flag = MsgAdapter.ReadChar() --当前竞拍角色信息是否显示 1:显示
    t.is_auto_bid = MsgAdapter.ReadChar()       --是否自动竞价  1:自动
    t.reserve_sh = MsgAdapter.ReadShort()
    return t
end

local function ReadRAPaiMaiHangRecordParam()
    local t = {}
    t.type = MsgAdapter.ReadShort()
    t.seq = MsgAdapter.ReadShort()
    t.activity_open_day = MsgAdapter.ReadInt()
    t.param1 = MsgAdapter.ReadInt()
    t.param2 = MsgAdapter.ReadInt()
    t.param3 = MsgAdapter.ReadInt()
    t.record_timestamp = MsgAdapter.ReadUInt()
    return t
end

--拍卖行通知信息下发
SCRAPaiMaiHangNoticeInfo = SCRAPaiMaiHangNoticeInfo or BaseClass(BaseProtocolStruct)
function SCRAPaiMaiHangNoticeInfo:__init()
    self.msg_type = 3100
end

function SCRAPaiMaiHangNoticeInfo:Decode()
    self.pai_mai_start_timestamp = MsgAdapter.ReadUInt()    --开始
    self.pai_mai_end_timestamp = MsgAdapter.ReadUInt()      --结束
    self.pai_mai_last_end_timestamp = MsgAdapter.ReadUInt() --最晚结束时间戳
    self.shop_item_list = {}    --全服商品信息
    for i = 0, PaiMaiHangConfig.MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM - 1 do
        self.shop_item_list[i] = ReadRaPaiMaiHangSingleShopItemInfo()
    end
    -- local data = {
    --     start_timestamp = self.pai_mai_start_timestamp,
    --     end_timestamp = self.pai_mai_end_timestamp,
    --     last_end_timestamp = self.pai_mai_last_end_timestamp,
    --     shop_item_list = self.shop_item_list
    -- }
    -- LogError("拍卖行通知信息下发 -> ", data)
end

--拍卖行单个通知信息下发
SCRAPaiMaiHangSingleNoticeInfo = SCRAPaiMaiHangSingleNoticeInfo or BaseClass(BaseProtocolStruct)
function SCRAPaiMaiHangSingleNoticeInfo:__init()
    self.msg_type = 3101
end

function SCRAPaiMaiHangSingleNoticeInfo:Decode()
    self.seq = MsgAdapter.ReadInt()
    self.shop_item_info = ReadRaPaiMaiHangSingleShopItemInfo()
    -- LogError("单个通知信息下发 ->     seq -> " .. self.seq .. "   info -> ", self.shop_item_info)
end

--拍卖行角色信息下发
SCRAPaiMaiHangUserInfo = SCRAPaiMaiHangUserInfo or BaseClass(BaseProtocolStruct)
function SCRAPaiMaiHangUserInfo:__init()
    self.msg_type = 3102
end

function SCRAPaiMaiHangUserInfo:Decode()
    self.shop_item_list = {}
    for i = 0, PaiMaiHangConfig.MAX_PAI_MAI_HANG_DAY_SHOP_ITEM_CUR_NUM - 1 do
        self.shop_item_list[i] = ReadRAPaiMaiHangUserSingleItemInfo()
    end
    -- LogError("角色信息下发 ->  ", self.shop_item_list)
end

--拍卖行角色单个信息下发
SCRAPaiMaiHangUserSingleInfo = SCRAPaiMaiHangUserSingleInfo or BaseClass(BaseProtocolStruct)
function SCRAPaiMaiHangUserSingleInfo:__init()
    self.msg_type = 3103
end

function SCRAPaiMaiHangUserSingleInfo:Decode()
    self.seq = MsgAdapter.ReadInt()
    self.shop_item_info = ReadRAPaiMaiHangUserSingleItemInfo()
    -- LogError("角色信息下发 ->     seq -> " .. self.seq .. "   info -> ", self.shop_item_info)
end

--拍卖行竞拍记录下发
SCRAPaiMaiHangUserRecordInfo = SCRAPaiMaiHangUserRecordInfo or BaseClass(BaseProtocolStruct)
function SCRAPaiMaiHangUserRecordInfo:__init()
    self.msg_type = 3104
end

function SCRAPaiMaiHangUserRecordInfo:Decode()
    self.record_count = MsgAdapter.ReadInt()
    self.record_list = {}
    for i = 1, self.record_count do
        table.insert(self.record_list, ReadRAPaiMaiHangRecordParam())
        self.record_list[i].sort_id = i
    end
    table.sort(self.record_list, DataSorter.KeyUpperSorter("sort_id"))
    -- LogError("竞拍记录下发 ->     record_count -> " .. self.record_count .. "   record_list -> ", self.record_list)
end

SCBackRewardInfo = SCBackRewardInfo or BaseClass(BaseProtocolStruct)
function SCBackRewardInfo:__init()
    self.msg_type = 3105
end

function SCBackRewardInfo:Decode()
    self.activity_time = MsgAdapter.ReadUInt()  --0不可参与  时间段可参与
    self.day_flag = {}           --每日标记
    for i = 1, 8 do
        table.insert(self.day_flag,MsgAdapter.ReadChar())
    end
    self.task_list = {}   ---任务标记
    for i = 1, 64 do
        local t = {
			flag = MsgAdapter.ReadChar(),	
			param = MsgAdapter.ReadUChar()	
		}
        table.insert(self.task_list,t)
    end
end
---回归请求
CSBackRewardReq = CSBackRewardReq or BaseClass(BaseProtocolStruct)
function CSBackRewardReq:__init()
	self.msg_type = 3106
end

function CSBackRewardReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
	MsgAdapter.WriteInt(self.param1 or 0)
end
----新服比拼角色所有消息下发
SCRAServerCompetitionRoleAllInfo = SCRAServerCompetitionRoleAllInfo or BaseClass(BaseProtocolStruct)
function SCRAServerCompetitionRoleAllInfo:__init()
    self.msg_type = 3107
end

function SCRAServerCompetitionRoleAllInfo:Decode()
    self.is_fetch_day_gift_flag = MsgAdapter.ReadChar() --是否领取每次礼包 0 未领取 1 已领取
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    self.count = MsgAdapter.ReadInt()
    self.rank_info = {}
    for i = 0, self.count-1 do
        local t = {
            cur_value = MsgAdapter.ReadInt(),
            max_value = MsgAdapter.ReadInt(),
            reward_flag = MsgAdapter.IntFlag()
		}
        self.rank_info[i] = t
    end
end
--全服比拼角色单条信息下发
SCRAServerCompetitionRoleInfo = SCRAServerCompetitionRoleInfo or BaseClass(BaseProtocolStruct)
function SCRAServerCompetitionRoleInfo:__init()
    self.msg_type = 3108
end

function SCRAServerCompetitionRoleInfo:Decode()
    local info = {}
    info.rank_type = MsgAdapter.ReadInt()
    info.cur_value = MsgAdapter.ReadInt()--当前数值
    info.max_value = MsgAdapter.ReadInt()--活动期间达到了最佳值
    info.reward_flag = MsgAdapter.IntFlag()
    self.info = info
end

----全服比拼活动信息下发
SCRAServerCompetitionActivityInfo = SCRAServerCompetitionActivityInfo or BaseClass(BaseProtocolStruct)
function SCRAServerCompetitionActivityInfo:__init()
    self.msg_type = 3109
end

function SCRAServerCompetitionActivityInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.rank_time_list = {}   ---排行榜
    for i = 0, self.count-1 do
        self.rank_time_list[i] = MsgAdapter.ReadUInt()
    end
end

SCRAShanHaiOriginInfo = SCRAShanHaiOriginInfo or BaseClass(BaseProtocolStruct)
function SCRAShanHaiOriginInfo:__init()
    self.msg_type = 3111
end
function SCRAShanHaiOriginInfo:Decode()
    self.info = {
        pass_count = MsgAdapter.ReadShort(),
        task_id = MsgAdapter.ReadShort(),           -- 任务id
        task_status = MsgAdapter.ReadShort(),       -- 任务状态
        progress = MsgAdapter.ReadShort(),          -- 打怪任务类型时需要
    }
end

----山海相逢
SCRAShanHaiMeetInfo = SCRAShanHaiMeetInfo or BaseClass(BaseProtocolStruct)
function SCRAShanHaiMeetInfo:__init()
    self.msg_type = 3110
end

function SCRAShanHaiMeetInfo:Decode()
    self.is_buy = MsgAdapter.ReadChar() --1已购买 0未购买
    self.sh_server = MsgAdapter.ReadChar()  
    self.ch_server = MsgAdapter.ReadShort()
    self.poker_flag = MsgAdapter.ReadUInt()  --翻牌领取情况
    self.fetch_flag = MsgAdapter.ReadUInt()   --任务领取奖励情况
    self.poker_list = {}
    for i = 1,6 do
        local t = {}
        t.type = MsgAdapter.ReadInt()
        t.index = i
        table.insert(self.poker_list,t)
    end

    self.task_list = {}
    self.count = MsgAdapter.ReadInt()
    for i = 1, self.count do
        local t = {}
        t.count = MsgAdapter.ReadInt()
        t.index = i
        table.insert(self.task_list,t)
    end
end


-- -- 端阳自选礼
-- SCRADuanYangZiXuanLiInfo = SCRADuanYangZiXuanLiInfo or BaseClass(BaseProtocolStruct)
-- function SCRADuanYangZiXuanLiInfo:__init()
--     self.msg_type = 5130
-- end

-- function SCRADuanYangZiXuanLiInfo:Decode()
--     local info = {}
--     info.is_buy_reward = MsgAdapter.ReadInt()
--     info.fetch_reward_flag = MsgAdapter.IntFlag(1,true)
--     info.select_reward_seq = {}
--     for i = 1, 32 do
--         -- LOG("i",i)
--         info.select_reward_seq[i] = MsgAdapter.ReadUChar()
--     end
--     self.info = info
--     -- self.is_buy_reward = MsgAdapter.ReadInt()
--     -- self.fetch_reward_flag = MsgAdapter.ReadInt()
--     LOG("端阳自选礼SCRADuanYangZiXuanLiInfo",self.info)
-- end

-- -- 端阳福礼
-- SCRADuanYangFuLiInfo = SCRADuanYangFuLiInfo or BaseClass(BaseProtocolStruct)
-- function SCRADuanYangFuLiInfo:__init()
--     self.msg_type = 5131
-- end
--招财进宝
SCRAZhaoCaiJinBaoRoleInfo = SCRAZhaoCaiJinBaoRoleInfo or BaseClass(BaseProtocolStruct)
function SCRAZhaoCaiJinBaoRoleInfo:__init()
    self.msg_type = 3112
end

function SCRAZhaoCaiJinBaoRoleInfo:Decode()
    self.next_refresh_timestamp = MsgAdapter.ReadUInt()
    self.history_chongzhi = MsgAdapter.ReadInt()
    self.day_chongzhi = MsgAdapter.ReadInt()
    --可领取次数列表
    self.chongzhi_times = {}
    for i = 0, MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM - 1 do
        self.chongzhi_times[i] = MsgAdapter.ReadShort()
    end
    --转盘奖励
    self.pool = {}
    for i = 0, MAX_RA_ZHAOCAIJINBAO_REFRESH_NUM - 1 do
        self.pool[i] = MsgAdapter.ReadShort()
    end
    --已购买次数列表
    self.buy_list = {}
    for i = 0, MAX_RA_ZHAOCAIJINBAO_BUY_NUM - 1 do
        self.buy_list[i] = MsgAdapter.ReadShort()
    end
    --已领取次数列表
    self.chongzhi_list = {}
    for i = 0, MAX_RA_ZHAOCAIJINBAO_CHONGZHI_NUM - 1 do
        self.chongzhi_list[i] = MsgAdapter.ReadShort()
    end
end

SCRAZhaoCaiJinBaoDrawResult = SCRAZhaoCaiJinBaoDrawResult or BaseClass(BaseProtocolStruct)
function SCRAZhaoCaiJinBaoDrawResult:__init()
    self.msg_type = 3113
end

function SCRAZhaoCaiJinBaoDrawResult:Decode()
    local count = MsgAdapter.ReadInt()
    self.seq_list = {}
    for i = 1, count do
        self.seq_list[i] = MsgAdapter.ReadInt()
    end
    -- self.draw_result = MsgAdapter.ReadInt()
end

SCRAZhaoCaiJinBaoRecordListInfo = SCRAZhaoCaiJinBaoRecordListInfo or BaseClass(BaseProtocolStruct)
function SCRAZhaoCaiJinBaoRecordListInfo:__init()
    self.msg_type = 3171
end

function SCRAZhaoCaiJinBaoRecordListInfo:Decode()
    local count = MsgAdapter.ReadInt()
    self.record_list = {}
    for i = 1, count do
        local t = {}
        t.times = MsgAdapter.ReadUInt()
        t.item_id = MsgAdapter.ReadInt()
        t.num = MsgAdapter.ReadInt()
        table.insert(self.record_list, t)
    end
end

----云泽画境信息下发
SCRAYunZeTerritoryInfo = SCRAYunZeTerritoryInfo or BaseClass(BaseProtocolStruct)
function SCRAYunZeTerritoryInfo:__init()
    self.msg_type = 3114
end

function SCRAYunZeTerritoryInfo:Decode()
    self.explore_list = {} --记录探索下标
    self.count = MsgAdapter.ReadInt()
    for i = 1,self.count do
        local tb = {}
        tb.index = i
        tb.show_type = MsgAdapter.ReadInt()
        self.explore_list[i] = tb
    end
    self.times = MsgAdapter.ReadInt()
end

----云泽繁花录
SCRAYunZeBloomingRecordInfo = SCRAYunZeBloomingRecordInfo or BaseClass(BaseProtocolStruct)
function SCRAYunZeBloomingRecordInfo:__init()
    self.msg_type = 3115
end

function SCRAYunZeBloomingRecordInfo:Decode()
    self.times = MsgAdapter.ReadShort()
    self.cur_intergral = MsgAdapter.ReadShort()
    self.challenge_end_timestamp = MsgAdapter.ReadInt()

    self.reward_list = {}
    for i = 1,5 do 
        self.reward_list[i] = MsgAdapter.ReadInt()
    end 
end

-- 随机活动 魔族在哪里
SCRAMoZuZaiNaLiInfo = SCRAMoZuZaiNaLiInfo or BaseClass(BaseProtocolStruct)
function SCRAMoZuZaiNaLiInfo:__init()
    self.msg_type = 3116
end

function SCRAMoZuZaiNaLiInfo:Decode()
    self.day_times = MsgAdapter.ReadInt()
end

-- 超值纷享
SCRAChaoZhiFenXiangInfo = SCRAChaoZhiFenXiangInfo or BaseClass(BaseProtocolStruct)
function SCRAChaoZhiFenXiangInfo:__init()
    self.msg_type = 3117
end

function SCRAChaoZhiFenXiangInfo:Decode()
    self.task_fetch = MsgAdapter.ReadShort()
    self.task_fetch_flag = DataHelper.ReverseList(bit:d16b(self.task_fetch))
    self.buy_fetch = MsgAdapter.ReadShort()
    self.buy_flag = DataHelper.ReverseList(bit:d16b(self.buy_fetch))
    self.task_param = {}
    for i = 1, 7 do
        self.task_param[i] = MsgAdapter.ReadShort()
    end
    MsgAdapter.ReadShort()
end

-- 分享同花礼信息
SCSameFlowerInfo = SCSameFlowerInfo or BaseClass(BaseProtocolStruct)
function SCSameFlowerInfo:__init()
    self.msg_type = 3118
end

function SCSameFlowerInfo:Decode()
    local info = {}
    info.buy_count = MsgAdapter.ReadShort()     -- 已购买次数
    info.key_count = MsgAdapter.ReadShort()     -- 玩家钥匙数量
    info.lock_rewards = {}
    for i = 1, 3 do
        local n = MsgAdapter.ReadInt() -- 锁定的奖励位
        info.lock_rewards[n] = 1
    end
    self.info = info
end

-- 分享同花礼摇奖3结果
SCSameFlowerResult = SCSameFlowerResult or BaseClass(BaseProtocolStruct)
function SCSameFlowerResult:__init()
    self.msg_type = 3119
end

function SCSameFlowerResult:Decode()
    self.result = MsgAdapter.ReadInt()
end

-- 随机活动 云泽二三事 信息下发
SCRAYunZeErSanShiInfo = SCRAYunZeErSanShiInfo or BaseClass(BaseProtocolStruct)
function SCRAYunZeErSanShiInfo:__init()
    self.msg_type = 3120
end

function SCRAYunZeErSanShiInfo:Decode()
    self.info = {
        times = MsgAdapter.ReadShort(),
        is_turntable_reward = MsgAdapter.ReadShort(),
        task_id = MsgAdapter.ReadInt(),
    }
end

-- 随机活动 云泽二三事 转盘信息
SCRAYunZeErSanShiTurntableInfo = SCRAYunZeErSanShiTurntableInfo or BaseClass(BaseProtocolStruct)
function SCRAYunZeErSanShiTurntableInfo:__init()
    self.msg_type = 3126
end

function SCRAYunZeErSanShiTurntableInfo:Decode()
    self.info = {
        seq = MsgAdapter.ReadInt(),
    }
end

--山海争锋
SCRAShanHaiZhengFengAllInfo = SCRAShanHaiZhengFengAllInfo or BaseClass(BaseProtocolStruct)
function SCRAShanHaiZhengFengAllInfo:__init()
    self.msg_type = 3121
end

function SCRAShanHaiZhengFengAllInfo:Decode()
    self.open_day = MsgAdapter.ReadShort()  --活动开启第几天
    self.is_fetch_all_pass_flag = MsgAdapter.ReadChar() --是否领取全首通宝箱，0：未，1：已
    MsgAdapter.ReadChar()
    self.first_flag = MsgAdapter.IntFlag(1, true)    --boss是否首杀，0：未，1：已
    table.remove(self.first_flag, 1)        --第一位不要
end

-- 山海觅宝
SCRALotusInfoMS = SCRALotusInfoMS or BaseClass(BaseProtocolStruct)
function SCRALotusInfoMS:__init()
    self.msg_type = 3122
end

function SCRALotusInfoMS:Decode()
    MsgAdapter.ReadShort()
    MsgAdapter.ReadChar()
    self.info = {}
    self.info.end_reward_flag = MsgAdapter.ReadChar()
    self.info.grid_list_active_flag = MsgAdapter.IntFlag(1,1)
end


-- 山海河灯基础信息
SCRAFlowerLampBaseInfoMS = SCRAFlowerLampBaseInfoMS or BaseClass(BaseProtocolStruct)
function SCRAFlowerLampBaseInfoMS:__init()
    self.msg_type = 3123
end

function SCRAFlowerLampBaseInfoMS:Decode()
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

-- 山海河灯游戏信息下发
SCRAFlowerLampGameInfoMS = SCRAFlowerLampGameInfoMS or BaseClass(BaseProtocolStruct)
function SCRAFlowerLampGameInfoMS:__init()
    self.msg_type = 3124
end

function SCRAFlowerLampGameInfoMS:Decode()
    self.info = {}
    self.info.notice_type = MsgAdapter.ReadShort()  -- FlowerLampData.NotiveType
    self.info.score = MsgAdapter.ReadShort()        -- 本次游戏内积分
    self.info.timestamp = MsgAdapter.ReadUInt()     -- 本次游戏借宿时间戳
end

-- 山海河灯信息下发
SCRAFlowerLampItemInfoMS = SCRAFlowerLampItemInfoMS or BaseClass(BaseProtocolStruct)
function SCRAFlowerLampItemInfoMS:__init()
    self.msg_type = 3125
end

function SCRAFlowerLampItemInfoMS:Decode()
    self.info = {}
    self.info.lamp_list = {}
    for i = 0, 9 do
        self.info.lamp_list[i] = MsgAdapter.ReadShort()     -- 花灯数量
    end
end

-- 吉星高照 角色信息
SCRAJiXingGaoZhaoRoleInfo = SCRAJiXingGaoZhaoRoleInfo or BaseClass(BaseProtocolStruct)
function SCRAJiXingGaoZhaoRoleInfo:__init()
    self.msg_type = 3127
end

function SCRAJiXingGaoZhaoRoleInfo:Decode()
    self.draw_times = MsgAdapter.ReadInt()
    self.draw_reward_fetch_mark = bit:d2b(MsgAdapter.ReadInt())
    self.shop_list = {}
    for i = 1, 16 do
        self.shop_list[i] = MsgAdapter.ReadShort()
    end
end
-- 吉星高照 奖池信息
SCRAJiXingGaoZhaoPoolInfo = SCRAJiXingGaoZhaoPoolInfo or BaseClass(BaseProtocolStruct)
function SCRAJiXingGaoZhaoPoolInfo:__init()
    self.msg_type = 3128
end

function SCRAJiXingGaoZhaoPoolInfo:Decode()
    self.item_id = MsgAdapter.ReadUShort()
    MsgAdapter.ReadShort()
    self.num = MsgAdapter.ReadUInt()
end
-- 吉星高照 抽奖返回
SCRAJiXingGaoZhaoDrawResult = SCRAJiXingGaoZhaoDrawResult or BaseClass(BaseProtocolStruct)
function SCRAJiXingGaoZhaoDrawResult:__init()
    self.msg_type = 3129
end

function SCRAJiXingGaoZhaoDrawResult:Decode()
    self.seq = MsgAdapter.ReadInt()
    self.num = MsgAdapter.ReadInt()
end


local function JiXingGaoZhaoRecordInfo()
    local t = {}
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.reward_type = MsgAdapter.ReadShort()
    t.item_id = MsgAdapter.ReadUShort()
    t.num = MsgAdapter.ReadInt()
    return t
end

--抽奖总记录 上线下发
SCRAJiXingGaoZhaoRecordListInfo = SCRAJiXingGaoZhaoRecordListInfo or BaseClass(BaseProtocolStruct)
function SCRAJiXingGaoZhaoRecordListInfo:__init()
    self.msg_type = 3130
end

function SCRAJiXingGaoZhaoRecordListInfo:Decode()
    self.record_count = MsgAdapter.ReadInt()
    self.record_list = {}
    for i = 1, self.record_count do
        self.record_list[i] = JiXingGaoZhaoRecordInfo()
    end
end
--单条抽奖记录
SCRAJiXingGaoZhaoRecordInfo = SCRAJiXingGaoZhaoRecordInfo or BaseClass(BaseProtocolStruct)
function SCRAJiXingGaoZhaoRecordInfo:__init()
    self.msg_type = 3131
end

function SCRAJiXingGaoZhaoRecordInfo:Decode()
    self.record_info = JiXingGaoZhaoRecordInfo()
end

-- 精灵大抓捕
SCRAYunZeCatchElvesGetInfo = SCRAYunZeCatchElvesGetInfo or BaseClass(BaseProtocolStruct)
function SCRAYunZeCatchElvesGetInfo:__init()
    self.msg_type = 3135
end

function SCRAYunZeCatchElvesGetInfo:Decode()
    self.is_catch_sp = MsgAdapter.ReadInt() -- 是否有特殊
    self.catch_num = {}
    -- 每种精灵抓取数量
    for i = 1, 10 do
        self.catch_num[i] = MsgAdapter.ReadInt()
    end
end

-- 云泽祈愿
SCRAYunZeWishInfo = SCRAYunZeWishInfo or BaseClass(BaseProtocolStruct)
function SCRAYunZeWishInfo:__init()
    self.msg_type = 3132
end

function SCRAYunZeWishInfo:Decode()
    self.next_refresh = MsgAdapter.ReadUInt()   --下次刷新的时间戳
    self.is_can_lottery = MsgAdapter.ReadInt()  --是否可以抽奖
    self.is_lock_Reward = MsgAdapter.IntFlag()  --是否锁定奖励
end

-- 云泽好礼
SCYunZeNiceGiftInfo = SCYunZeNiceGiftInfo or BaseClass(BaseProtocolStruct)
function SCYunZeNiceGiftInfo:__init()
    self.msg_type = 3133
end

function SCYunZeNiceGiftInfo:Decode()
    self.info = {}              
    self.info.buy_count = MsgAdapter.ReadShort()        -- 已购买次数
    self.info.prop_free_num = MsgAdapter.ReadShort()    -- 银锤子数量
end

--萌宠驾到信息下发
SCRAYunZeCutePetInfo = SCRAYunZeCutePetInfo or BaseClass(BaseProtocolStruct)
function SCRAYunZeCutePetInfo:__init()
    self.msg_type = 3134
end

function SCRAYunZeCutePetInfo:Decode()
    self.pet_combat_num = MsgAdapter.ReadInt()  --宠物战力
    self.today_fraction = {}    --每日获得战力
    for i = 0, 12 - 1 do
        self.today_fraction[i] = MsgAdapter.ReadInt()
    end
    self.task_param_list = {}   --任务参数[任务类型],每日任务进度
    for i = 0, 8 - 1 do
        self.task_param_list[i] = MsgAdapter.ReadInt()
    end
    self.task_status = MsgAdapter.IntFlag()    --BitMap 任务完成状态，1完成
end

--萌宠驾到挑战信息下发
SCRAYunZeCutePetChallengeInfo = SCRAYunZeCutePetChallengeInfo or BaseClass(BaseProtocolStruct)
function SCRAYunZeCutePetChallengeInfo:__init()
    self.msg_type = 3136
end

function SCRAYunZeCutePetChallengeInfo:Decode()
    self.is_challenge_status = MsgAdapter.ReadInt() --本次是否挑战成功, -1登录下发，0失败，1成功，2平局
    self.challenge_status = MsgAdapter.IntFlag()  --萌宠挑战成功状态[1-10] --当前下标位是否挑战成功
end

----全服比拼活动其他信息下发
SCRAServerCompetitionOtherInfo = SCRAServerCompetitionOtherInfo or BaseClass(BaseProtocolStruct)
function SCRAServerCompetitionOtherInfo:__init()
    self.msg_type = 3137
end

function SCRAServerCompetitionOtherInfo:Decode()
    self.is_fetch_day_gift_flag = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
end

-- 寅虎问世
SCTigerBornInfo = SCTigerBornInfo or BaseClass(BaseProtocolStruct)
function SCTigerBornInfo:__init()
    self.msg_type = 3139
end

function SCTigerBornInfo:Decode()
    local info = {}
    info.buy_flag = MsgAdapter.ReadShort()
    info.reward_seq = MsgAdapter.ReadShort()
    info.day_reward = {}
    for i = 0, 5 do
        info.day_reward[i] = MsgAdapter.ReadShort()
    end
    self.info = info
end

-- 寅虎探春
SCTigerFindSpringInfo = SCTigerFindSpringInfo or BaseClass(BaseProtocolStruct)
function SCTigerFindSpringInfo:__init()
    self.msg_type = 3140
end

function SCTigerFindSpringInfo:Decode()
    self.seq = MsgAdapter.ReadShort() -- 当日刷新地图索引
    self.is_receive = MsgAdapter.ReadShort() -- 当日是否可领取奖励，0 不可领取 1 可领取 2 已领取
end

-- 辞牛迎新虎
SCTigerNewCowOld = SCTigerNewCowOld or BaseClass(BaseProtocolStruct)
function SCTigerNewCowOld:__init()
    self.msg_type = 3142
end

function SCTigerNewCowOld:Decode()
    local info = {}
    info.awaken_flags = bit:d16b(MsgAdapter.ReadShort())        -- 唤醒标记
    info.awaken_reward_time = MsgAdapter.ReadShort()            -- 可唤醒次数
    info.awaken_param = {}
    for i = 0, 4 do
        local data = {}
        data.reward_flags = bit:d16b(MsgAdapter.ReadShort())    -- 领取标记
        data.tiger_value = MsgAdapter.ReadUShort()              -- 虎气值
        info.awaken_param[i] = data                 
    end
    self.info = info
end

-- 虎行大运
SCRAHuXingDaYunInfo = SCRAHuXingDaYunInfo or BaseClass(BaseProtocolStruct)
function SCRAHuXingDaYunInfo:__init()
    self.msg_type = 3144
end

function SCRAHuXingDaYunInfo:Decode()
    local info = {}
    info.invite_time = MsgAdapter.ReadUInt()            --邀请好友到达时间戳，到达后会清零
    info.finish_pos = MsgAdapter.ReadShort()            --已完成的任务点，如果到达需要领取奖励后才会+1
    info.first_flag = MsgAdapter.ShortFlag(0)           --是否领取了首通奖励
    info.is_can_receive = MsgAdapter.ReadChar()         --是否可以领取奖励，0不可，1可
    info.is_can_receive_invited = MsgAdapter.ReadChar() --是否可以领取被邀请奖励，0不可，1可
    info.invited_times = MsgAdapter.ReadShort()         --被邀请次数
    info.invited_time = MsgAdapter.ReadUInt()           --被邀请达到时间戳，到达后会清零
    self.info = info
end

SCRAHuXingDaYunBeInvitedInfo = SCRAHuXingDaYunBeInvitedInfo or BaseClass(BaseProtocolStruct)
function SCRAHuXingDaYunBeInvitedInfo:__init()
    self.msg_type = 3145
end

function SCRAHuXingDaYunBeInvitedInfo:Decode()
    local info = {}
    info.count = MsgAdapter.ReadInt()
    info.list = {}
    for i = 1, info.count do
        local t = {}
        t.index = i - 1                                         --数组下标，接受邀请使用
        t.uid = MsgAdapter.ReadInt()                            --角色id
        t.prof_id = MsgAdapter.ReadInt()                        --职业id
		t.avatar_type = MsgAdapter.ReadShort()                  --角色类型
		t.headshot_id = MsgAdapter.ReadShort()                  --头像id
        t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)  --角色名
        table.insert(info.list, t)
    end
    self.invited_info = info
end

-- 寅虎拜岁
SCRAYinHuBaiSuiInfo = SCRAYinHuBaiSuiInfo or BaseClass(BaseProtocolStruct)
function SCRAYinHuBaiSuiInfo:__init()
    self.msg_type = 3143
end

function SCRAYinHuBaiSuiInfo:Decode()
    local info = {}
    info.is_buy = MsgAdapter.ReadShort()        --是否已购买
    info.receive_flag = MsgAdapter.ShortFlag(0) --是否已领取
    self.info = info
end

--年年有鱼信息
SCRATigerFishEveryYearInfo = SCRATigerFishEveryYearInfo or BaseClass(BaseProtocolStruct)
function SCRATigerFishEveryYearInfo:__init()
    self.msg_type = 3138
end

function SCRATigerFishEveryYearInfo:Decode()
    self.is_accpet_task = MsgAdapter.ReadShort()--是否接取渔夫任务，0未接取，1已接取
    self.role_level = MsgAdapter.ReadShort()    --玩家等级段，接任务后有值
    self.fish_num = MsgAdapter.ReadInt()        --拥有数量
    self.task_status = MsgAdapter.IntFlag()     --每日任务是否完成
    self.task_reward_status = MsgAdapter.IntFlag()--每日任务奖励是否领取

    self.task_extent = {}   --任务进度
    local TIRGER_FEY_TYPE_MAX = 9
    for i = 0, TIRGER_FEY_TYPE_MAX - 1 do
        self.task_extent[i] = MsgAdapter.ReadInt()
    end
    self.is_receive_reward = MsgAdapter.IntFlag()   --奖励档位是否可领取
    self.reward_stage_status = MsgAdapter.IntFlag() --奖励档位是否已领取
    -- LOG(self, "年年有鱼信息->")
end

--寅虎逗新春
SCRATigerTeaseNewYearInfo = SCRATigerTeaseNewYearInfo or BaseClass(BaseProtocolStruct)
function SCRATigerTeaseNewYearInfo:__init()
    self.msg_type = 3141
end

function SCRATigerTeaseNewYearInfo:Decode()
    self.times = MsgAdapter.ReadShort()     --已参加次数
    self.is_win = MsgAdapter.ReadShort()    --胜败，0普通，1胜利，2失败
end


--寅虎压岁
SCRATigerGiftMoneyInfo = SCRATigerGiftMoneyInfo or BaseClass(BaseProtocolStruct)
function SCRATigerGiftMoneyInfo:__init()
    self.msg_type = 3146
end

function SCRATigerGiftMoneyInfo:Decode()
    self.money_num = MsgAdapter.ReadInt()     --压岁钱累计
    self.is_can_receive = MsgAdapter.ReadShort()    --0不可领取，1可以领取
    self.role_level = MsgAdapter.ReadShort()    --当日玩家登录时等级
    self.task_status = MsgAdapter.IntFlag()  --每日任务是否完成
    self.task_reward_receive = MsgAdapter.IntFlag()  --每日任务奖励是否已领取
    self.task_list = {} --任务进度
    for i = 0,Config.yinhuyasui_auto.other[1].task_num do
        self.task_list[i] = MsgAdapter.ReadInt()
    end
end

--瑞雪丰年
SCRATigerAuspiciousSnowInfo = SCRATigerAuspiciousSnowInfo or BaseClass(BaseProtocolStruct)
function SCRATigerAuspiciousSnowInfo:__init()
    self.msg_type = 3147
end

function SCRATigerAuspiciousSnowInfo:Decode()
    self.times = MsgAdapter.ReadShort()     --抽奖次数
    self.draw_result = MsgAdapter.ReadShort() --抽中结果
end
---------------竞技月卡----------------
local function RAJingJiYueKaTaskInfo()
	return {
		task_id = MsgAdapter.ReadShort(),    -- 对应任务组的任务序号
		finish_flag = MsgAdapter.ReadChar(), -- 0:未完成 1:已完成
        refresh_num = MsgAdapter.ReadUChar() -- 任务刷新次数
	}
end

--竞技月卡所有信息下发
SCRAJingJiYueKaAllInfo = SCRAJingJiYueKaAllInfo or BaseClass(BaseProtocolStruct)
function SCRAJingJiYueKaAllInfo:__init()
    self.msg_type = 3150
end

function SCRAJingJiYueKaAllInfo:Decode()
    self.active_role_level =  MsgAdapter.ReadInt()       --激活该赛季时角色等级
    self.next_switch_timestamp =  MsgAdapter.ReadUInt()  --下次切换状态时间戳
    self.last_switch_timestamp =  MsgAdapter.ReadUInt()  --上次切换状态时间戳

    self.is_active_buy_flag = MsgAdapter.ReadChar()		 --是否直购激活了额外奖励(江湖引) 0:未激活 1:已激活
    self.role_card_level = MsgAdapter.ReadShort()	     --月卡等级
    self.role_card_exp = MsgAdapter.ReadInt()		     --月卡经验

    self.fetch_common_flag_num = MsgAdapter.ReadShort()
    local flag1 = MsgAdapter.ReadUInt()
    local flag2 = MsgAdapter.ReadUInt()
    self.fetch_common_flag = bit:ll2b(flag2, flag1)      -- 普通等级奖励领取标志位
    local flag3 = MsgAdapter.ReadUInt()
    local flag4 = MsgAdapter.ReadUInt()
    self.fetch_adv_flag = bit:ll2b(flag4, flag3)         -- 高级等级奖励领取标志位

    self.day_task_count = MsgAdapter.ReadShort()
    self.day_task_list = {}
    for i = 0, self.day_task_count - 1 do 
        self.day_task_list[i] = RAJingJiYueKaTaskInfo()
    end 

    self.week_task_count = MsgAdapter.ReadShort()
    self.week_task_list = {}
    for i = 0, self.week_task_count - 1 do 
        self.week_task_list[i] = RAJingJiYueKaTaskInfo()
    end 
    self.season_task_count = MsgAdapter.ReadShort()
    self.season_task_list = {}
    for i = 0, self.season_task_count - 1 do 
        self.season_task_list[i] = RAJingJiYueKaTaskInfo()
    end 

    self.task_type_max = MsgAdapter.ReadShort()
    self.progress_list = {}
    for i = 0,SportsCardEmum.MAX_TASK_GROUP_TYPE_NUM -1 do 
        if self.progress_list[i] == nil then self.progress_list[i] = {} end 
        for j =0 ,self.task_type_max-1 do 
            self.progress_list[i][j] = MsgAdapter.ReadUInt()
        end 
    end 
end

-- 竞技月卡相关通知    
SCRAJingJiYueKaNotice = SCRAJingJiYueKaNotice or BaseClass(BaseProtocolStruct)
function SCRAJingJiYueKaNotice:__init()
    self.msg_type = 3151
end

-- NOTICE_TYPE_FETCH_COMMON_SUCC = 0,		//领取普通奖励成功 P1:card_level
-- NOTICE_TYPE_FETCH_SENIOR_SUCC = 1,		//领取高级奖励成功 P1:card_level
-- NOTICE_TYPE_BUY_LEVEL = 2,				//购买等级 P1:cur_role_card_level
-- NOTICE_TYPE_BUY_CMD_SUCC = 3,			//直购购买成功 P1:is_active_buy_flag
-- NOTICE_TYPE_DAY_TASK_FINISH = 4,		    //每日任务完成 P1:task_index
-- NOTICE_TYPE_WEEK_TASK_FINISH = 5,		//每周任务完成 P1:task_index
-- NOTICE_TYPE_SEASON_TASK_FINISH = 6,		//赛季任务完成 P1:task_index
-- NOTICE_TYPE_LEVEL_UP = 7,				//等级提升 P1:cur_role_card_level
-- NOTICE_TYPE_EXP_CHANGE = 8,				//经验变化 P1:role_card_exp

function SCRAJingJiYueKaNotice:Decode()
    self.notice_type = MsgAdapter.ReadInt()
    self.param1 = MsgAdapter.ReadInt()
end 

-- 竞技月卡任务刷新下发
SCRAJingJiYueKaRefreshNotice  = SCRAJingJiYueKaRefreshNotice or BaseClass(BaseProtocolStruct)
function SCRAJingJiYueKaRefreshNotice:__init()
    self.msg_type = 3152
end

function SCRAJingJiYueKaRefreshNotice:Decode()
    self.task_index = MsgAdapter.ReadInt() 
    self.task_info = RAJingJiYueKaTaskInfo()
end 

-- 竞技月卡单条条件信息下发  
SCRAJingJiYueKaSingleTypeInfo  = SCRAJingJiYueKaSingleTypeInfo or BaseClass(BaseProtocolStruct)
function SCRAJingJiYueKaSingleTypeInfo:__init()
    self.msg_type = 3153
end

function SCRAJingJiYueKaSingleTypeInfo:Decode()
    self.task_type = MsgAdapter.ReadInt() -- 任务类型
    self.prog_list = {}
    for j = 0,SportsCardEmum.MAX_TASK_GROUP_TYPE_NUM - 1 do -- 赛季0，周2，日1任务中的同类型的进度
        self.prog_list[j] = MsgAdapter.ReadUInt()
    end 
end 

-- 竞技月卡任务列表信息下发
SCRAJingJiYueKaTaskListInfo  = SCRAJingJiYueKaTaskListInfo or BaseClass(BaseProtocolStruct)
function SCRAJingJiYueKaTaskListInfo:__init()
    self.msg_type = 3154
end

function SCRAJingJiYueKaTaskListInfo:Decode()
    self.task_group_type = MsgAdapter.ReadShort()
    self.task_count = MsgAdapter.ReadShort()

    self.task_list = {}
    for j = 0,self.task_count - 1 do 
        self.task_list[j] = RAJingJiYueKaTaskInfo()
    end 
end

--熔炼有礼
SCRARongLianYouLiInfo = SCRARongLianYouLiInfo or BaseClass(BaseProtocolStruct)
function SCRARongLianYouLiInfo:__init()
    self.msg_type = 3155
end

function SCRARongLianYouLiInfo:Decode()
    self.total_task_finish = MsgAdapter.ReadShort()     --总完成任务数量
    self.total_task = MsgAdapter.ReadShort()            --总任务数量
    self.has_task_stage_reward = MsgAdapter.IntFlag()   --阶段奖励是否已领取

    self.task_score = {}    --任务进度
    for i = 0, 11 do
        self.task_score[i] = MsgAdapter.ReadShort()
    end
    self.reward_receive_num = {}    --任务已领取数量
    for i = 0, 11 do
        self.reward_receive_num[i] = MsgAdapter.ReadShort()
    end
    --LOG(self, "熔炼有礼信息下发")
end

--祈愿孔明灯信息
SCRAQiYuanKongMingDengInfo = SCRAQiYuanKongMingDengInfo or BaseClass(BaseProtocolStruct)
function SCRAQiYuanKongMingDengInfo:__init()
    self.msg_type = 3156
end

function SCRAQiYuanKongMingDengInfo:Decode()
    self.huozhong = MsgAdapter.ReadInt()
    self.complete_flag = MsgAdapter.IntFlag()
    self.task_list = {}
    local MAX_RA_QIYUAN_KONGMINGDENG_TASK_COUNT = 10
    for i = 0, MAX_RA_QIYUAN_KONGMINGDENG_TASK_COUNT - 1 do
        self.task_list[i] = MsgAdapter.ReadUShort()
    end
    -- LOG(self, "祈愿孔明灯信息 -> ")
end

--元气助人
SCVitalityHelpOthersInfo = SCVitalityHelpOthersInfo or BaseClass(BaseProtocolStruct)
function SCVitalityHelpOthersInfo:__init()
    self.msg_type = 3158
end

function SCVitalityHelpOthersInfo:Decode()
    self.times = MsgAdapter.ReadInt()
    self.task_id = MsgAdapter.ReadInt()
    self.pre_task_id = MsgAdapter.ReadInt()
    -- LogDG("========================================================================元气助人========================================================================")
    -- LogDG("元气助人",self)
end

SCRAXunMengHuDieInfo = SCRAXunMengHuDieInfo or BaseClass(BaseProtocolStruct)
function SCRAXunMengHuDieInfo:__init()
    self.msg_type = 3157
end
function SCRAXunMengHuDieInfo:Decode()
    self.count = MsgAdapter.ReadInt()
end

--播种有礼
SCRABoZhongYouLiInfo = SCRABoZhongYouLiInfo or BaseClass(BaseProtocolStruct)
function SCRABoZhongYouLiInfo:__init()
    self.msg_type = 3159
end

function SCRABoZhongYouLiInfo:Decode()
    self.sowing_count = MsgAdapter.ReadInt() --剩余可播种次数
    self.next_refresh_timestamp = MsgAdapter.ReadUInt() --浇水下次刷新时间戳
    self.planting_timestamp = {} --植物种植时间戳
    for i = 0, 3 do
        self.planting_timestamp[i] = MsgAdapter.ReadUInt()
    end
    -- LOG(self, "播种有礼信息 -> ")
end

--异兽临世，基本信息
SCGodBeastAdventInfo = SCGodBeastAdventInfo or BaseClass(BaseProtocolStruct)
function SCGodBeastAdventInfo:__init()
    self.msg_type = 3160
end

function SCGodBeastAdventInfo:Decode()
    local info = {}
    info.is_buy = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    info.satisfy = MsgAdapter.ReadUInt()
    info.today_task_finish = MsgAdapter.IntFlag(0)
    info.is_get_reward = MsgAdapter.IntFlag(0)
    info.task_param = {} 
    for i = 0, 24 do
        info.task_param[i] = MsgAdapter.ReadInt()
    end
    info.other_buy_count = {}
    for i = 0, 19 do
        info.other_buy_count[i] = MsgAdapter.ReadShort()
    end
    self.info = info
end

--异兽临世，活动开启关闭通知
SCGodBeastAdventOpenInfo = SCGodBeastAdventOpenInfo or BaseClass(BaseProtocolStruct)
function SCGodBeastAdventOpenInfo:__init()
    self.msg_type = 3162
end

function SCGodBeastAdventOpenInfo:Decode()
    local info = {}
    info.is_open = MsgAdapter.ReadInt()
    info.type = MsgAdapter.ReadInt()
    self.info = info
end

--异兽临世，全服限购道具数量信息
SCGodBeastAdventBuyInfo = SCGodBeastAdventBuyInfo or BaseClass(BaseProtocolStruct)
function SCGodBeastAdventBuyInfo:__init()
    self.msg_type = 3163
end

function SCGodBeastAdventBuyInfo:Decode()
    self.animal_buy_count = MsgAdapter.ReadInt()
end

--异兽临世，好感度变化通知
SCGodBeastAdventNotifyInfo = SCGodBeastAdventNotifyInfo or BaseClass(BaseProtocolStruct)
function SCGodBeastAdventNotifyInfo:__init()
    self.msg_type = 3169
end

function SCGodBeastAdventNotifyInfo:Decode()
    self.add_like = MsgAdapter.ReadInt()
end

-- 乘风破浪开启标志位下发
SCRAChengFengpoLangOpenInfo = SCRAChengFengpoLangOpenInfo or BaseClass(BaseProtocolStruct)
function SCRAChengFengpoLangOpenInfo:__init()
    self.msg_type = 3167
end

function SCRAChengFengpoLangOpenInfo:Decode()
    self.is_open = MsgAdapter.ReadInt()
end

-- 乘风破浪信息下发
SCRAChengFengpoLangInfo = SCRAChengFengpoLangInfo or BaseClass(BaseProtocolStruct)
function SCRAChengFengpoLangInfo:__init()
    self.msg_type = 3168
end

function SCRAChengFengpoLangInfo:Decode()
    local info = {}
    info.answer_times = MsgAdapter.ReadInt()
    info.question_id = MsgAdapter.ReadInt()
    self.info = info
end

SCRAYuanQiFanPaiInfo = SCRAYuanQiFanPaiInfo or BaseClass(BaseProtocolStruct)
function SCRAYuanQiFanPaiInfo:__init()
    self.msg_type = 3161
end
function SCRAYuanQiFanPaiInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.is_receive_times = MsgAdapter.IntFlag()
    self.is_fetch_day_reward = MsgAdapter.ReadInt()     -- 是否已领取翻牌次数奖励
    -- LogDG(self)
end

--万灵筑礼信息
SCRAWanLingZhuLiInfo = SCRAWanLingZhuLiInfo or BaseClass(BaseProtocolStruct)
function SCRAWanLingZhuLiInfo:__init()
    self.msg_type = 3164
end

function SCRAWanLingZhuLiInfo:Decode()
    self.login_day = MsgAdapter.ReadShort()         --登录天数
    self.fetch_reward_mark = bit:d2b(MsgAdapter.ReadShort()) --奖励领取标记位
    self.day_reward_mark = MsgAdapter.ReadChar()    --每日领取标记
    self.is_buy_jiagou = MsgAdapter.ReadChar()      --是否加购
    MsgAdapter.ReadShort()
    -- LOG(self, "万灵筑礼信息")
end

SCBaoZiLaiLeInfo = SCBaoZiLaiLeInfo or BaseClass(BaseProtocolStruct)
function SCBaoZiLaiLeInfo:__init()
    self.msg_type = 3165
end
function SCBaoZiLaiLeInfo:Decode()
    self.info = {
        count = MsgAdapter.ReadInt(),       -- 今日已参与次数
    }
end

--元气竞速信息下发
SCYuanQiSpeedInfo = SCYuanQiSpeedInfo or BaseClass(BaseProtocolStruct)
function SCYuanQiSpeedInfo:__init()
    self.msg_type = 3148
end

function SCYuanQiSpeedInfo:Decode()
    self.today_play_times = MsgAdapter.ReadInt() --今日参与次数
    self.pass_flag = MsgAdapter.IntFlag()   --通关记录
    self.reward_flag = MsgAdapter.IntFlag() --奖励记录
    -- LOG(self, "元气竞速信息下发")
end

--元气竞速游戏状态
SCRAYuanQiSpeedPlayInfo = SCRAYuanQiSpeedPlayInfo or BaseClass(BaseProtocolStruct)
function SCRAYuanQiSpeedPlayInfo:__init()
    self.msg_type = 3149
end

function SCRAYuanQiSpeedPlayInfo:Decode()
    self.game_type = MsgAdapter.ReadInt()       --0输 1赢 2游戏中
    self.play_end_tamp = MsgAdapter.ReadInt()
    self.play_seq = MsgAdapter.ReadInt()
    -- LOG(self, "元气竞速游戏状态")
end


-- 随机活动-元气记事簿 角色信息
SCRAYuanQiJiShiBuInfo = SCRAYuanQiJiShiBuInfo or BaseClass(BaseProtocolStruct)
function SCRAYuanQiJiShiBuInfo:__init()
    self.msg_type = 3166
end

function SCRAYuanQiJiShiBuInfo:Decode()
    local t = {
        combat = MsgAdapter.ReadShort(),            -- 获得的劳作值 不清空
        is_jinjie_jiangli = MsgAdapter.ReadChar(),  -- 是否购买进阶奖励
        re_ch = MsgAdapter.ReadChar(),
        quanming_reward_fetch_flag = MsgAdapter.ReadInt(),  -- 免费奖励
        jinjie_reward_fetch_flag = MsgAdapter.ReadInt(),  -- 进阶奖励
        complete_flag = MsgAdapter.ReadInt(),  -- 任务完成情况 按位标记
        task_list = {},  -- 任务完成情况 按位标记
    }
    for i = 0, MayDayConfig.MAX_RA_YUAN_QI_JISHIBU_TASK_COUNT - 1 do
        t.task_list[i] = MsgAdapter.ReadUShort()
    end
    self.info = t
end

SCArticulatelyInfo = SCArticulatelyInfo or BaseClass(BaseProtocolStruct)
function SCArticulatelyInfo:__init()
    self.msg_type = 3170
end

function SCArticulatelyInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    -- LogDG("妙笔生花",self)
end

-- 粽香满城下发
SCRAZongXiangManChengInfo = SCRAZongXiangManChengInfo or BaseClass(BaseProtocolStruct)
function SCRAZongXiangManChengInfo:__init()
    self.msg_type = 3180
end

function SCRAZongXiangManChengInfo:Decode()
    local info = {}
    info.select_type = MsgAdapter.ReadInt()
    info.flag = MsgAdapter.IntFlag(0)
    self.info = info
end

-- 端阳香囊 
SCRADuanYangXiangNang = SCRADuanYangXiangNang or BaseClass(BaseProtocolStruct)
function SCRADuanYangXiangNang:__init()
    self.msg_type = 3183
end

function SCRADuanYangXiangNang:Decode()
    self.today_task_finish = MsgAdapter.IntFlag()
    self.is_receive_reward = MsgAdapter.IntFlag()
    self.today_task_count = {}
    -- 任务最大类型
    for i =0, 24 do 
        self.today_task_count[i] = MsgAdapter.ReadInt()
    end 
end


-- 幸运盲盒下发
SCRALuckDrawBoxInfo = SCRALuckDrawBoxInfo or BaseClass(BaseProtocolStruct)
function SCRALuckDrawBoxInfo:__init()
    self.msg_type = 3185
end

function SCRALuckDrawBoxInfo:Decode()
    local info = {}
    info.chongzhi = MsgAdapter.ReadUInt()
    MsgAdapter.ReadUInt()
    info.box_flag_list = {}
    for i = 1,4 do
        info.box_flag_list[i] = {}
        for j = 1, 3 do
            info.box_flag_list[i][j] = MsgAdapter.ReadChar()
        end
    end
    info.today_buy_times = {}
    for i = 0,7 do
        info.today_buy_times[i] = MsgAdapter.ReadShort()
    end
    info.act_buy_times = {}
    for i = 0,7 do
        info.act_buy_times[i] = MsgAdapter.ReadShort()
    end
    info.today_reward_times = {}
    for i = 0,7 do
        info.today_reward_times[i] = MsgAdapter.ReadShort()
    end
    info.act_reward_times = {}
    for i = 0,7 do
        info.act_reward_times[i] = MsgAdapter.ReadShort()
    end
    self.info = info
    --LOG("幸运盲盒下发", self)
end

-- 咸甜之争下发
SCRASaltySweetBattleInfo = SCRASaltySweetBattleInfo or BaseClass(BaseProtocolStruct)
function SCRASaltySweetBattleInfo:__init()
    self.msg_type = 3181
end

function SCRASaltySweetBattleInfo:Decode()
    local info = {}
    info.value = {}
    info.value[1] = MsgAdapter.ReadInt()
    info.value[2] = MsgAdapter.ReadInt()
    info.value[0] = info.value[1] + info.value[2]
    info.state = MsgAdapter.ReadInt()
    self.info = info
end

-- 咸甜之争个人下发
SCRASaltySweetBattleRoleInfo = SCRASaltySweetBattleRoleInfo or BaseClass(BaseProtocolStruct)
function SCRASaltySweetBattleRoleInfo:__init()
    self.msg_type = 3182
end

function SCRASaltySweetBattleRoleInfo:Decode()
    self.select_type = MsgAdapter.ReadInt()
    self.reward_flag = MsgAdapter.ReadInt()
end

-- 苍龙赐福
SCDragonCollectInfo = SCDragonCollectInfo or BaseClass(BaseProtocolStruct)
function SCDragonCollectInfo:__init()
    self.msg_type = 3184
end

function SCDragonCollectInfo:Decode()
    local info = {}
    info.end_reward = MsgAdapter.ReadInt()  -- 最后奖励领取标记。0不可领取，1可领取，2已领取
    info.flag_list = MsgAdapter.IntFlag(1)
    self.info = info
    --LOG("苍龙赐福", self)
end

-- 兰汤初沐信息下发
SCRALanTangChuMuInfo = SCRALanTangChuMuInfo or BaseClass(BaseProtocolStruct)
function SCRALanTangChuMuInfo:__init()
    self.msg_type = 3186
end

function SCRALanTangChuMuInfo:Decode()
    local info = {}
    info.reward_flag = MsgAdapter.IntFlag(0)
    info.task_param = {}
    for i = 0, 24 do
        info.task_param[i] = MsgAdapter.ReadInt()
    end
    info.last_reward_flag = MsgAdapter.ReadInt()
    info.medicine_num = {}
    for i = 0, 3 do
        info.medicine_num[i] = MsgAdapter.ReadShort()
    end
    self.info = info
end



-- 宠技返利
SCChongJiFanLiInfo = SCChongJiFanLiInfo or BaseClass(BaseProtocolStruct)
function SCChongJiFanLiInfo:__init()
    self.msg_type = 3187
end

function SCChongJiFanLiInfo:Decode()
    local info = {}
    info.is_day_meet_flag = MsgAdapter.ReadChar()
    info.reserve_ch = MsgAdapter.ReadChar()
    info.reserve_sh = MsgAdapter.ReadShort()
    info.meet_cond_day_num = MsgAdapter.ReadInt()
    info.day_chongzhi_num = MsgAdapter.ReadInt()
    info.reward_fetch_flag = MsgAdapter.IntFlag(2)
    self.info = info

    --LOG("宠技返利SCChongJiFanLiInfo",self.info)
end

-- 新服宠物强化
SCActPetStrInfo = SCActPetStrInfo or BaseClass(BaseProtocolStruct)
function SCActPetStrInfo:__init()
    self.msg_type = 3188
end

function SCActPetStrInfo:Decode()
    local t = {}
    self.info = t
    t.free_reward_flag = MsgAdapter.ReadInt()
    t.task_finish_flag_list = {}    -- 任务是否完成flag
    t.reward_finish_flag_list = {}  -- 奖励是否领取标记
    for qua = 1, 6 do
        t.task_finish_flag_list[qua] = MsgAdapter.IntFlag(1)
    end
    for qua = 1, 6 do
        t.reward_finish_flag_list[qua] = MsgAdapter.IntFlag(1)
    end
    --LOG("新服宠物强化", self.info)
end

-- 七夕特惠登录全部信息下发
SCRAQiXiTeHuiInfo = SCRAQiXiTeHuiInfo or BaseClass(BaseProtocolStruct)
function SCRAQiXiTeHuiInfo:__init()
    self.msg_type = 3189
end

function SCRAQiXiTeHuiInfo:Decode()
    local info = {}
    info.is_open_flag = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    info.activity_buy_count = MsgAdapter.ReadShort()
    info.phase_reward_flag = MsgAdapter.IntFlag(0)
    info.today_buy_times = {}
    for i = 0, 31 do
        info.today_buy_times[i] = MsgAdapter.ReadShort()
    end
    info.activity_buy_times = {}
    for i = 0, 31 do
        info.activity_buy_times[i] = MsgAdapter.ReadShort()
    end
    self.info = info
end

-- 七夕特惠单个限购信息改变下发
SCRAQiXiTeHuiSingleInfo = SCRAQiXiTeHuiSingleInfo or BaseClass(BaseProtocolStruct)
function SCRAQiXiTeHuiSingleInfo:__init()
    self.msg_type = 3190
end

function SCRAQiXiTeHuiSingleInfo:Decode()
    self.activity_buy_count = MsgAdapter.ReadShort()
    self.seq = MsgAdapter.ReadShort()
    self.today_buy_time = MsgAdapter.ReadShort()
    self.activity_buy_time = MsgAdapter.ReadShort()
end

-- 七夕特惠其他信息改变下发
SCRAQiXiTeHuiOtherInfo = SCRAQiXiTeHuiOtherInfo or BaseClass(BaseProtocolStruct)
function SCRAQiXiTeHuiOtherInfo:__init()
    self.msg_type = 3191
end

function SCRAQiXiTeHuiOtherInfo:Decode()
    self.is_open_flag = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    self.phase_reward_flag = MsgAdapter.IntFlag(0)
end

-- 3193 随机活动-周末连充礼信息下发
SCRAWeekendLianChongInfo = SCRAWeekendLianChongInfo or BaseClass(BaseProtocolStruct)
function SCRAWeekendLianChongInfo:__init()
    self.msg_type = 3193
end

function SCRAWeekendLianChongInfo:Decode()
    self.day_chong_zhi_num = MsgAdapter.ReadLL() --当天充值元宝数
    self.day_can_fetch_flag_list = {} -- 奖励是否可领取标识
    -- 是day数组！
    for i = 0,2 do 
        self.day_can_fetch_flag_list[i] = MsgAdapter.IntFlag(0)
    end 
    self.day_fecth_flag_list = {} -- 奖励是否已领取标识
    for i = 0,2 do 
        self.day_fecth_flag_list[i] = MsgAdapter.IntFlag(0)
    end 
    self.extra_reward_flag = MsgAdapter.IntFlag(0) -- 额外奖励标识
end 


-- 3194 随机活动-周末连充礼信息下发
SCRAWeekendLianChongSingleInfo = SCRAWeekendLianChongSingleInfo or BaseClass(BaseProtocolStruct)
function SCRAWeekendLianChongSingleInfo:__init()
    self.msg_type = 3194
end

function SCRAWeekendLianChongSingleInfo:Decode()
    self.notice_type = MsgAdapter.ReadShort() -- 下发类型 0 领取日奖励 1 领取额外奖励 
    MsgAdapter.ReadShort()

    self.day_index = MsgAdapter.ReadChar()       -- 领取天号
    self.seq = MsgAdapter.ReadChar()             -- seq
    self.can_fetch_flag = MsgAdapter.ReadChar() -- 可以拿
    self.fetch_flag = MsgAdapter.ReadChar()     -- 拿了没
end

SCRADaZhanHongTuInfo = SCRADaZhanHongTuInfo or BaseClass(BaseProtocolStruct)
function SCRADaZhanHongTuInfo:__init()
    self.msg_type = 3195
end

function SCRADaZhanHongTuInfo:Decode()
    self.fetch_flag = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    self.chong_zhi_num = MsgAdapter.ReadLL()
end