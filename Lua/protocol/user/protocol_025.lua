CSTreasureHuntReq = CSTreasureHuntReq or BaseClass(BaseProtocolStruct)
function CSTreasureHuntReq:__init()
    self.msg_type = 2500
    self.is_guide_draw = 0
    self.draw_type = 0
    self.draw_times = 0
end
function CSTreasureHuntReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.is_guide_draw)
    MsgAdapter.WriteShort(self.draw_type)
    MsgAdapter.WriteShort(self.draw_times)
    MsgAdapter.WriteShort(self.is_use_free)
    for i,v in ipairs(self.comsume_info) do
        MsgAdapter.WriteShort(v.col)
        MsgAdapter.WriteShort(v.idx)
        MsgAdapter.WriteInt(v.num)
    end
end

SCTreasureHuntRet = SCTreasureHuntRet or BaseClass(BaseProtocolStruct)
function SCTreasureHuntRet:__init()
    self.msg_type = 2501
    self.node_item_list = {}
    self.count = 0
end
function SCTreasureHuntRet:Decode()
    self.node_item_list = {}
    self.count = MsgAdapter.ReadInt()
    for i=1,self.count do
        local type = MsgAdapter.ReadShort()
        local id = MsgAdapter.ReadUShort()
        local num = MsgAdapter.ReadInt()
        self.node_item_list[i] = {type = type,id = id,num = num}
    end
end

SCXunBaoOpenInfo = SCXunBaoOpenInfo or BaseClass(BaseProtocolStruct)
function SCXunBaoOpenInfo:__init()
    self.msg_type = 2502
    self.count = 0
end

function SCXunBaoOpenInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.open_info = {}
    for i=1, self.count do
        self.open_info[i] = {}
        self.open_info[i].treasure_id = MsgAdapter.ReadShort() --服务器上的变量名为 draw_type
        self.open_info[i].is_open = MsgAdapter.ReadShort()
        self.open_info[i].open_day = MsgAdapter.ReadInt()
        self.open_info[i].open_type = MsgAdapter.ReadInt()
        self.open_info[i].start_time = MsgAdapter.ReadUInt()
        self.open_info[i].end_time = MsgAdapter.ReadUInt()
        self.open_info[i].week_open_flag = bit:d2b(MsgAdapter.ReadUInt())
        self.open_info[i].day_begin_time = MsgAdapter.ReadUShort()
        self.open_info[i].day_end_time = MsgAdapter.ReadUShort()
    end
end

SCXunBaoDrawTimesInfo = SCXunBaoDrawTimesInfo or BaseClass(BaseProtocolStruct)
function SCXunBaoDrawTimesInfo:__init()
    self.msg_type = 2503
end

function SCXunBaoDrawTimesInfo:Decode()
    self.treasure_group_times = {}
    for i=1, XUN_BAO_DRAW_CUMULATIVE_TIMES_GROUP_MAX_NUM do
        self.treasure_group_times[i] = MsgAdapter.ReadInt() --抽奖次数，根据group_id
        -- LogError("i",i,"group_times",self.treasure_group_times[i])
    end
    
    -- LogError("xi_you_floor_curr_times",self.xi_you_floor_curr_times)
    local flag1 = MsgAdapter.ReadUInt()
    local flag2 = MsgAdapter.ReadUInt()
    self.first_draw_flag = bit:ll2b(flag2, flag1) --寻宝id，是否获取第一次首抽奖励标志
    local flag3 = MsgAdapter.ReadUInt()
    local flag4 = MsgAdapter.ReadUInt()
    self.second_draw_flag = bit:ll2b(flag4, flag3) --寻宝id，是否获取第二次首抽奖励标志
    self.treasure_group_free_times = {}
    for i= 0, 20 - 1 do
        self.treasure_group_free_times[i] = MsgAdapter.ReadUInt() --下次免费抽奖时间，魂器读第7位，圣器读第十位，服务端说的
    end
    self.xi_you_floor_curr_times = {}
    for i = 0, 20 - 1 do
        self.xi_you_floor_curr_times[i] = MsgAdapter.ReadInt() --稀有保底抽奖次数
    end
end

CSXunBaoExchange = CSXunBaoExchange or BaseClass(BaseProtocolStruct)
function CSXunBaoExchange:__init()
    self.msg_type = 2504
end

function CSXunBaoExchange:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.draw_type) --对应寻宝id
    MsgAdapter.WriteUShort(self.get_index) --对应寻宝id的奖励组中选择的奖励的序号
    MsgAdapter.WriteUShort(self.comsume_index1) --消耗材料在对应消耗组的序号
    MsgAdapter.WriteUShort(self.comsume_index2)
    for i=1, #self.comsumeList do
        local t = self.comsumeList[i]
        MsgAdapter.WriteUShort(t.comsume_type)  --消耗材料组序号（0是组1，1是组2）
        MsgAdapter.WriteUChar(t.type)           --物品所在队列（0是宠物列表，1是宠物仓库）
        MsgAdapter.WriteUChar(t.index)          --消耗材料所在队列中的index
    end
end

CSXunBaoBlessReq = CSXunBaoBlessReq or BaseClass(BaseProtocolStruct)
function CSXunBaoBlessReq:__init()
    self.msg_type = 2505
end

function CSXunBaoBlessReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.index)
    MsgAdapter.WriteShort(0)
    MsgAdapter.WriteInt(self.pet_id)
end

SCXunBaoBlessInfo = SCXunBaoBlessInfo or BaseClass(BaseProtocolStruct)
function SCXunBaoBlessInfo:__init()
    self.msg_type = 2506
end

function SCXunBaoBlessInfo:Decode()
    self.xun_bao_bless_pet_id_list = {}
    for i=1,MAX_XUN_BAO_BLESS_PET_NUM do
        self.xun_bao_bless_pet_id_list[i] = MsgAdapter.ReadInt()
    end
end

-- 战斗采集副本信息
SCGatherFbInfo = SCGatherFbInfo or BaseClass(BaseProtocolStruct)
function SCGatherFbInfo:__init()
    self.msg_type = 2510
end

function SCGatherFbInfo:Decode()
    self.gather_power = MsgAdapter.ReadInt()        --拥有活力
end

-- 战斗采集采集物信息
SCGatherFbSceneInfo = SCGatherFbSceneInfo or BaseClass(BaseProtocolStruct)
function SCGatherFbSceneInfo:__init()
    self.msg_type = 2511
end

function SCGatherFbSceneInfo:Decode()
    local t = {}
    t.obj_id = MsgAdapter.ReadInt()
    t.x = MsgAdapter.ReadInt()
    t.y = MsgAdapter.ReadInt()
    self.gather_info = t
end

-- 战斗采集采集物移除
SCGatherFbGatherRemoveInfo = SCGatherFbGatherRemoveInfo or BaseClass(BaseProtocolStruct)
function SCGatherFbGatherRemoveInfo:__init()
    self.msg_type = 2512
end
function SCGatherFbGatherRemoveInfo:Decode()
    self.obj_id = MsgAdapter.ReadInt()
end

-- 战斗采集采集物详情
SCGatherFbGatherListInfo = SCGatherFbGatherListInfo or BaseClass(BaseProtocolStruct)
function SCGatherFbGatherListInfo:__init()
    self.msg_type = 2513
end
function SCGatherFbGatherListInfo:Decode()
    self.list_num = MsgAdapter.ReadInt()
    local t = {}
    for i = 1, self.list_num do
        table.insert(t, {
            item_id = MsgAdapter.ReadUShort(),
            num = MsgAdapter.ReadShort(),
        })
    end
    self.gather_list = t
end

CSBraveGroundReq = CSBraveGroundReq or BaseClass(BaseProtocolStruct)
function CSBraveGroundReq:__init()
    self.msg_type = 2520
end
function CSBraveGroundReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

local RankItem = function (i)
    return {
        level = i,
        layer = MsgAdapter.ReadInt(),
        role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    }
end
--[[ SCBraveGroundRankInfo = SCBraveGroundRankInfo or BaseClass(BaseProtocolStruct)
function SCBraveGroundRankInfo:__init()
    self.msg_type = 2521
end
function SCBraveGroundRankInfo:Decode()
    self.role_rank = MsgAdapter.ReadInt()
    self.role_reached_level = MsgAdapter.ReadInt()
    self.rank_list = {}
    self.count = MsgAdapter.ReadInt()
    local layer = -1
    local level = 1
    for i = 1, self.count do
        self.rank_list[i] = RankItem(i)
        if i == 1 and layer == -1 then
            layer = self.rank_list[i].layer
        else
            if layer == self.rank_list[i].layer then
                self.rank_list[i].level = level
            else
                level = level + 1
                self.rank_list[i].level = level
                layer = self.rank_list[i].layer
            end
        end
        if self.rank_list[i].role_name == RoleData.Instance:GetRoleName() then
            self.role_rank = self.rank_list[i].level
        end
    end
end ]]
local CommonSaveBraveGroundRankRoleInfoItem = function ()
    return {role_id = MsgAdapter.ReadInt(), role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)}
end
local CommonSaveBraveGroundTeamReceordData = function ()
    local t = {}
    t.time = MsgAdapter.ReadUInt()
    t.reached_level = MsgAdapter.ReadInt()
    t.record_key = MsgAdapter.ReadInt()
    t.role_info_count = MsgAdapter.ReadInt()
    t.role_info = {}
    for i = 1, 5 do
        t.role_info[i] = CommonSaveBraveGroundRankRoleInfoItem()
    end
    return t
end
SCBraveGroundRankInfo = SCBraveGroundRankInfo or BaseClass(BaseProtocolStruct)
function SCBraveGroundRankInfo:__init()
    self.msg_type = 2521
end
function SCBraveGroundRankInfo:Decode()
    self.rank_list = {}
    self.count = MsgAdapter.ReadInt()
    local info
    for i = 1, self.count do
        info = CommonSaveBraveGroundTeamReceordData()
        -- info.rank = i
        for index, value in ipairs(info.role_info) do
            if self:CheckRepeat(value) then
                value.rank = i
                value.reached_level = info.reached_level
                table.insert(self.rank_list, value)
            end
        end
        -- self.rank_list[i].rank = i
    end
    --改成不同
end
function SCBraveGroundRankInfo:CheckRepeat(info)
    if info.role_id == 0 then
        return false
    end
    for index, value in ipairs(self.rank_list) do
        if value.role_id == info.role_id then
            return false
        end
    end
    return true
end

SCBraveGroundSceneInfo = SCBraveGroundSceneInfo or BaseClass(BaseProtocolStruct)
function SCBraveGroundSceneInfo:__init()
    self.msg_type = 2522
end
function SCBraveGroundSceneInfo:Decode()
    self.cur_level = MsgAdapter.ReadInt()
    self.buff_id = MsgAdapter.ReadInt()--击杀精英怪获得buff
    self.is_kill_boss = MsgAdapter.ReadInt()
end

SCBraveGroundLevelRewardInfo = SCBraveGroundLevelRewardInfo or BaseClass(BaseProtocolStruct)
function SCBraveGroundLevelRewardInfo:__init()
    self.msg_type = 2523
end
function SCBraveGroundLevelRewardInfo:Decode()
    self.start_level = MsgAdapter.ReadInt()
    self.end_level = MsgAdapter.ReadInt()
end

SCBraveGroundInfo = SCBraveGroundInfo or BaseClass(BaseProtocolStruct)
function SCBraveGroundInfo:__init()
    self.msg_type = 2524
end
function SCBraveGroundInfo:Decode()
    --本周最高纪录
    -- self.play_times = MsgAdapter.ReadInt()
    self.reached_level = MsgAdapter.ReadInt()
end

SCBraveGroundRoute = SCBraveGroundRoute or BaseClass(BaseProtocolStruct)
function SCBraveGroundRoute:__init()
    self.msg_type = 2525
end
function SCBraveGroundRoute:Decode()
    self.param1 = MsgAdapter.ReadInt()
    self.param2 = MsgAdapter.ReadInt()
end
SCBraveGroundNotify = SCBraveGroundNotify or BaseClass(BaseProtocolStruct)
function SCBraveGroundNotify:__init()
    self.msg_type = 2526
end
function SCBraveGroundNotify:Decode()
    self.level = MsgAdapter.ReadInt()
end

-- 勇闯地宫场景玩家使用道具信息
SCBraveGroundUseItemInfo = SCBraveGroundUseItemInfo or BaseClass(BaseProtocolStruct)
function SCBraveGroundUseItemInfo:__init()
    self.msg_type = 2527
end
function SCBraveGroundUseItemInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.use_item_info = {}
    for i = 1, self.count do
        local use_type = MsgAdapter.ReadInt()
        self.use_item_info[use_type] = MsgAdapter.ReadInt()
    end
end

SCBraveGroundTeamInfo = SCBraveGroundTeamInfo or BaseClass(BaseProtocolStruct)
function SCBraveGroundTeamInfo:__init()
    self.msg_type = 2528
end
function SCBraveGroundTeamInfo:Decode()
    self.info = {}
    for i = 1, 5 do
        self.info[i] = {}
        self.info[i].role_id = MsgAdapter.ReadInt()
        self.info[i].reached_level = MsgAdapter.ReadInt()
    end
end
SCBraveGroundRoleInfo = SCBraveGroundRoleInfo or BaseClass(BaseProtocolStruct)
function SCBraveGroundRoleInfo:__init()
    self.msg_type = 2529
end
function SCBraveGroundRoleInfo:Decode()
    self.max_reached_level = MsgAdapter.ReadInt()--历史最高层
    self.sao_dang_reward_level = MsgAdapter.ReadUShort()--这周可扫荡层数
    self.is_retch_sao_dang_reward = MsgAdapter.ReadUShort()--是否领取了
end
SCBraveGroundCommonInfo = SCBraveGroundCommonInfo or BaseClass(BaseProtocolStruct)
function SCBraveGroundCommonInfo:__init()
    self.msg_type = 2530
end
function SCBraveGroundCommonInfo:Decode()
    self.world_level = MsgAdapter.ReadInt()
end

--====================商店限购红点========================
CSGoldShopNotifyCancelReq = CSGoldShopNotifyCancelReq or BaseClass(BaseProtocolStruct)

function CSGoldShopNotifyCancelReq:__init()
    self.msg_type = 2540
end

function CSGoldShopNotifyCancelReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.cancel_index)
end

SCGoldShopPurchaseNotifys = SCGoldShopPurchaseNotifys or BaseClass(BaseProtocolStruct)

function SCGoldShopPurchaseNotifys:__init()
    self.msg_type = 2541
end

function SCGoldShopPurchaseNotifys:Decode()
    self.notify_flag = MsgAdapter.IntFlag(4)
end


-- 轮回忘川请求
CSLunHuiWangChuanReq = CSLunHuiWangChuanReq or BaseClass(BaseProtocolStruct)

function CSLunHuiWangChuanReq:__init()
    self.msg_type = 2507
end

function CSLunHuiWangChuanReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param)
end

-- 轮回忘川下发
SCLunHuiWangChuanInfo = SCLunHuiWangChuanInfo or BaseClass(BaseProtocolStruct)

function SCLunHuiWangChuanInfo:__init()
    self.msg_type = 2508
end

function SCLunHuiWangChuanInfo:Decode()
    self.info = {
        start_level = MsgAdapter.ReadInt(),             --参与等级记录
        battle_monster_id = MsgAdapter.ReadInt(),       --战斗怪物组
        next_refresh_tamp = MsgAdapter.ReadUInt(),      --下一次刷新时间
        pass_flags = MsgAdapter.IntFlag(),
        reward_flags = MsgAdapter.IntFlag(),
    }
end

--npc商店随机列表信息
SCShopRandListInfo  = SCShopRandListInfo or BaseClass(BaseProtocolStruct)

function SCShopRandListInfo:__init()
    self.msg_type = 2542
end
function SCShopRandListInfo:Decode()
    self.info = {}
    self.info.next_refresh_time = MsgAdapter.ReadUInt()
    local count = MsgAdapter.ReadInt()
    local rand_item_list = {}
    for i = 1, count do
        local shop_rand_item = {}
        shop_rand_item.shop_item_seq = MsgAdapter.ReadUShort()
        shop_rand_item.shop_item_buy_times = MsgAdapter.ReadUShort()
        table.insert(rand_item_list, shop_rand_item)
    end
    self.info.rand_item_list = rand_item_list
end

--npc 商店随机列表单个信息
SCShopRandListSingleInfo = SCShopRandListSingleInfo or BaseClass(BaseProtocolStruct)

function SCShopRandListSingleInfo:__init()
    self.msg_type = 2543
end

function SCShopRandListSingleInfo:Decode()
    self.shop_list_seq = MsgAdapter.ReadInt()
    local rand_item = {}
    rand_item.shop_item_seq = MsgAdapter.ReadUShort()
    rand_item.shop_item_buy_times = MsgAdapter.ReadUShort()
    self.rand_item = rand_item
end

-- 市场请求
CSMarketReq = CSMarketReq or BaseClass(BaseProtocolStruct)

function CSMarketReq:__init()
    self.msg_type = 2545
end

function CSMarketReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
    MsgAdapter.WriteInt(self.param4)
end


-- 市场所有信息下发
SCMarketAllInfo = SCMarketAllInfo or BaseClass(BaseProtocolStruct)

function SCMarketAllInfo:__init()
    self.msg_type = 2546
end

function SCMarketAllInfo:Decode()
    self.info_list = {}
    for i=1,MsgAdapter.ReadInt() do
        local info = SCMarketSingleInfo.ReadInfo()
        self.info_list[info.item_id] = info
    end
    --LogError("市场所有信息",self)
end

-- 市场单独信息下发
SCMarketSingleInfo = SCMarketSingleInfo or BaseClass(BaseProtocolStruct)

function SCMarketSingleInfo.ReadInfo()
    local info = {}
    info.item_id = MsgAdapter.ReadUShort()
    MsgAdapter.ReadShort()
    info.cur_price = MsgAdapter.ReadInt()       -- 当前价格
    info.yerterday_price = MsgAdapter.ReadInt() -- 昨天价格
    info.count = MsgAdapter.ReadInt()           -- 贸易数量(可为负数)
    return info
end

function SCMarketSingleInfo:__init()
    self.msg_type = 2547
end

function SCMarketSingleInfo:Decode()
    self.info = SCMarketSingleInfo.ReadInfo()
    self.item_id = self.info.item_id
end

--旧结婚协议 屏蔽
--[[
-------------------结婚信息请求-------------------
CSMarriageReq = CSMarriageReq or BaseClass(BaseProtocolStruct)

function CSMarriageReq:__init()
    self.msg_type = 2551

    self.req_type = 0
    self.param1 = 0
    self.param2 = 0
    self.param3 = 0
end

function CSMarriageReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.req_type)
    MsgAdapter.WriteShort(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
end

-------------------求婚信息-------------------
SCMarryReqRoute = SCMarryReqRoute or BaseClass(BaseProtocolStruct)

function SCMarryReqRoute:__init()
    self.msg_type = 2552

    self.marry_type = 0
    self.role_uid = 0
    self.role_name = ""
end

function SCMarryReqRoute:Decode()
    self.marry_type = MsgAdapter.ReadInt()
    self.role_uid = MsgAdapter.ReadInt()
    self.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
end

-------------------结婚信息-------------------
SCMarryInfo = SCMarryInfo or BaseClass(BaseProtocolStruct)

function SCMarryInfo:__init()
    self.msg_type = 2553

    self.love_uid = 0 --伴侣UID
    self.love_name = "" --伴侣名字
    self.last_marry_time = 0 --结婚时间
    self.last_divorce_time = 0 --离婚时间
    self.force_divorce_time = 0 --申请强制离婚时间
    self.love_avatar_type = 0
end

function SCMarryInfo:Decode()
    self.love_uid = MsgAdapter.ReadInt()
    self.love_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    self.love_avatar_type = MsgAdapter.ReadInt()
    self.last_marry_time = MsgAdapter.ReadUInt()
    self.last_divorce_time = MsgAdapter.ReadUInt()
    self.force_divorce_time = MsgAdapter.ReadUInt()
    
    self.puzzle_play_times = MsgAdapter.ReadInt()   --快乐拼图玩的次数
    self.question_play_times = MsgAdapter.ReadInt() --答题玩的次数

    self.fb_info = {}
    self.fb_info.max_wave = MsgAdapter.ReadShort()  --最大波数
    self.fb_info.cur_wave = MsgAdapter.ReadShort()  --当前波数
    self.fb_info.is_sweep = MsgAdapter.ReadShort()  --是否扫荡
end

-------------------对方请求离婚-------------------
SCDivorceReqRoute = SCDivorceReqRoute or BaseClass(BaseProtocolStruct)

function SCDivorceReqRoute:__init()
    self.msg_type = 2554
end

function SCDivorceReqRoute:Decode()
end

-------------------对方是否收到求婚请求-------------------
SCMarryProposeNotice = SCMarryProposeNotice or BaseClass(BaseProtocolStruct)

function SCMarryProposeNotice:__init()
    self.msg_type = 2555
    self.result = 0
end

function SCMarryProposeNotice:Decode()
    self.result = MsgAdapter.ReadInt()
end

-------------------发送宣言-------------------
CSMarriageSendOath = CSMarriageSendOath or BaseClass(BaseProtocolStruct)

function CSMarriageSendOath:__init()
    self.msg_type = 2556

    self.req_type = 0
    self.reserve = 0
    self.marriage_oath_notice = ""
end

function CSMarriageSendOath:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.req_type)
    MsgAdapter.WriteShort(self.reserve)
    MsgAdapter.WriteStrN(self.marriage_oath_notice, MARRY_OATH_LENGHT)
end

-------------------宣言数据-------------------
SCMarryOathInfo = SCMarryOathInfo or BaseClass(BaseProtocolStruct)

function SCMarryOathInfo:__init()
    self.msg_type = 2557
    self.count = 0
    self.oath_data_list = nil
end

function SCMarryOathInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.oath_data_list = {}
    for i = 1, self.count do
        self.oath_data_list[i] = SCMarryOathInfo.MarryOathStruct()
    end
end

function SCMarryOathInfo.MarryOathStruct()
    local oath_data = {}
    oath_data.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    oath_data.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    oath_data.role_id = MsgAdapter.ReadInt()
    MsgAdapter.ReadChar()
    oath_data.sex = MsgAdapter.ReadChar()
    oath_data.profession = MsgAdapter.ReadChar()
    oath_data.is_online = MsgAdapter.ReadChar()
    oath_data.level = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    oath_data.timestamp = MsgAdapter.ReadUInt()
    oath_data.avator_timestamp = MsgAdapter.ReadLL()
    oath_data.avator_frame_id = MsgAdapter.ReadInt()
    oath_data.marriage_oath_notice = MsgAdapter.ReadStrN(MARRY_OATH_LENGHT)
    return oath_data
end

--任务对方接取
SCMarriageTaskRoute = SCMarriageTaskRoute or BaseClass(BaseProtocolStruct)

function SCMarriageTaskRoute:__init()
    self.msg_type = 2558 --发2251 对方就会收到信息
end

function SCMarriageTaskRoute:Decode()
    self.role_id = MsgAdapter.ReadInt()
    self.param1 = MsgAdapter.ReadInt()
    self.param2 = MsgAdapter.ReadInt()
    self.param3 = MsgAdapter.ReadUInt()
end

--题目信息
SCMarriageTaskQuestionInfo = SCMarriageTaskQuestionInfo or BaseClass(BaseProtocolStruct)

function SCMarriageTaskQuestionInfo:__init()
    self.msg_type = 2559
end

function SCMarriageTaskQuestionInfo:Decode()
    local info = {}
    info.question_type = MsgAdapter.ReadChar() --是否双人
    info.question_num = MsgAdapter.ReadChar()  --第几题
    info.question_id = MsgAdapter.ReadShort()  --问题id
    info.end_time = MsgAdapter.ReadUInt()      --结束时间戳
    self.question_data = info
end

--答题返回结果
SCMarriageTaskQuestionResult = SCMarriageTaskQuestionResult or BaseClass(BaseProtocolStruct)

function SCMarriageTaskQuestionResult:__init()
    self.msg_type = 2560
end

function SCMarriageTaskQuestionResult:Decode()
    local info = {}
    info.question_type = MsgAdapter.ReadChar() --是否双人
    info.is_right = MsgAdapter.ReadChar()      --是否正确
    info.question_id = MsgAdapter.ReadShort()  --问题id
    self.question_data = info
end

--拼图返回结果
SCMarriageDuetPuzzleInfo = SCMarriageDuetPuzzleInfo or BaseClass(BaseProtocolStruct)

function SCMarriageDuetPuzzleInfo:__init()
    self.msg_type = 2562
end

function SCMarriageDuetPuzzleInfo:Decode()
    --0 开始 p1 开始时间 --1 结束 p1 0 两个没完成 1两个完成了 2其中有一个掉线就会发
    self.info_type = MsgAdapter.ReadInt()
    self.open_time = MsgAdapter.ReadUInt()
end

--离线通知
SCMarriageDuetTaskLogoutNoticeInfo = SCMarriageDuetTaskLogoutNoticeInfo or BaseClass(BaseProtocolStruct)

function SCMarriageDuetTaskLogoutNoticeInfo:__init()
    self.msg_type = 2563
end

function SCMarriageDuetTaskLogoutNoticeInfo:Decode()
    LogError("离线通知")
end
]]

--查询返回结果
SCMarryQueryRet = SCMarryQueryRet or BaseClass(BaseProtocolStruct)

function SCMarryQueryRet:__init()
    self.msg_type = 2561
end

function SCMarryQueryRet:Decode()
    local info = {}
    info.role_id = MsgAdapter.ReadInt()
    info.lover_id = MsgAdapter.ReadInt()
    info.prof = MsgAdapter.ReadShort()
    info.sex = MsgAdapter.ReadShort()
    self.query_info = info
end