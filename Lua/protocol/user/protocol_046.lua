-- 4600 英雄会武请求
CSWorldTeamArenaReq = CSWorldTeamArenaReq or BaseClass(BaseProtocolStruct)
function CSWorldTeamArenaReq:__init()
    self.msg_type = 4600
end

-- 0 请求报名信息，1 确认成员报名，2 取消报名，3 请求查询某个战队的详细信息（从报名）P1 team_unique_id p2 type(0战队报名，1比赛)
-- 4 请求查询某个战队详细信息（从最终排名）p1 team_unique_id
-- 5 请求战队特殊信息
-- 6 请求其他信息（目前只有
function CSWorldTeamArenaReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type) 
    MsgAdapter.WriteUInt2(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

local function WorldTeamArenaTeamSignUpSimpleInfo()
    local t = {}
	t.team_unique_id = MsgAdapter.ReadUInt2()
	t.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.total_capablity = MsgAdapter.ReadLL()
    t.level = MsgAdapter.ReadShort()
    t.top_level = MsgAdapter.ReadShort()
    t.avatar_type = MsgAdapter.ReadShort()
    t.headshot_id = MsgAdapter.ReadShort()
    t.fly_flag = MsgAdapter.ReadShort()

    t.member_list = {}
    for i = 1,5 do 
        t.member_list[i] = {}
        t.member_list[i].role_id = MsgAdapter.ReadInt()
        t.member_list[i].profession = MsgAdapter.ReadInt()
    end 

    return t
end

-- 4601 所有报名战队信息下发
SCWorldTeamArenaAllSimpleInfo = SCWorldTeamArenaAllSimpleInfo or BaseClass(BaseProtocolStruct)
function SCWorldTeamArenaAllSimpleInfo:__init()
    self.msg_type = 4601
end

function SCWorldTeamArenaAllSimpleInfo:Decode()
    self.my_team_index = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadShort()

    self.team_list = {}
    for i = 0 ,self.count - 1 do 
        self.team_list[i] = WorldTeamArenaTeamSignUpSimpleInfo()
    end 

    self.know_count = MsgAdapter.ReadShort()
    self.know_team_index = {}
    --熟识战队  数组值为上面数组下标
    for i = 0 ,self.know_count - 1 do 
        self.know_team_index[i] = MsgAdapter.ReadShort()
    end 
end

--4602 英雄会武报名请求
CSWorldTeamArenaSignUpReq = CSWorldTeamArenaSignUpReq or BaseClass(BaseProtocolStruct)
function CSWorldTeamArenaSignUpReq:__init()
    self.msg_type = 4602
end

function CSWorldTeamArenaSignUpReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteStrN(self.name,PROTC_ROLE_NAME_LEN)
end

-- 4603 英雄会武预备报名信息下发
SCWorldTeamArenaPreTeamSignUpInfo = SCWorldTeamArenaPreTeamSignUpInfo or BaseClass(BaseProtocolStruct)
function SCWorldTeamArenaPreTeamSignUpInfo:__init()
    self.msg_type = 4603
end

function SCWorldTeamArenaPreTeamSignUpInfo:Decode()
    self.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    self.member_list = {}
    for i = 1,5 do 
        local vo = {}
        vo.role_id = MsgAdapter.ReadInt()
        vo.determine_flag = MsgAdapter.ReadInt()
        table.insert(self.member_list,vo)
    end 
end

-- 4604 英雄会武通知信息下发
SCWorldTeamArenaNoticeInfo = SCWorldTeamArenaNoticeInfo or BaseClass(BaseProtocolStruct)
function SCWorldTeamArenaNoticeInfo:__init()
    self.msg_type = 4604
end

function SCWorldTeamArenaNoticeInfo:Decode()
    self.notice_type = MsgAdapter.ReadInt()
    self.param1 = MsgAdapter.ReadInt()
    self.param2 = MsgAdapter.ReadInt()
end

local function WorldTeamArenaUserDetailedInfo()
    local t = {}
    t.role_uid = MsgAdapter.ReadInt()
    t.profession = MsgAdapter.ReadInt()
    t.level = MsgAdapter.ReadShort()
    t.top_level = MsgAdapter.ReadShort()
    t.avatar_type = MsgAdapter.ReadShort()   -- 虚拟人物类型
    t.headshot_id = MsgAdapter.ReadShort()   -- 头像ID 
    t.fly_flag = MsgAdapter.ReadShort()      -- 飞升阶段
    MsgAdapter.ReadShort()
    t.capablity = MsgAdapter.ReadInt()       -- 角色评分
    
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)

    t.rank_list = {}
    for i = 1 ,3 do 
        t.rank_list[i] = MsgAdapter.ReadInt()
    end 

    return t
end


--  4605 英雄会武战队详细信息下发
SCWorldTeamArenaTeamSignUpDetailedInfo = SCWorldTeamArenaTeamSignUpDetailedInfo or BaseClass(BaseProtocolStruct)
function SCWorldTeamArenaTeamSignUpDetailedInfo:__init()
    self.msg_type = 4605
end

-- notice_type 0 报名界面 1 比赛界面 2 排名结算界面
function SCWorldTeamArenaTeamSignUpDetailedInfo:Decode()
    self.notice_type = MsgAdapter.ReadInt()
	self.team_unique_id = MsgAdapter.ReadUInt2()
    self.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)

    self.member_list = {}
    for i = 1,5 do 
        self.member_list[i] = WorldTeamArenaUserDetailedInfo() 
        self.member_list[i].is_master = i == 1
    end 
end

-- 4606 英雄会武活动信息下发
SCActivityWorldTeamArenaInfo = SCActivityWorldTeamArenaInfo or BaseClass(BaseProtocolStruct)
function SCActivityWorldTeamArenaInfo:__init()
    self.msg_type = 4606
end

function SCActivityWorldTeamArenaInfo:Decode()
    self.season_num =  MsgAdapter.ReadInt()                --赛季
    self.start_sign_up_timestamp =  MsgAdapter.ReadUInt()  --开始报名时间戳
    self.end_sign_up_timestamp =  MsgAdapter.ReadUInt()    --停止报名时间戳
    self.activity_open_next_zero_timestamp = MsgAdapter.ReadUInt()    --结算时间戳
end 

-- 4607 英雄会武场景操作请求
CSActivityWorldTeamArenaSceneReq = CSActivityWorldTeamArenaSceneReq or BaseClass(BaseProtocolStruct)
function CSActivityWorldTeamArenaSceneReq:__init()
    self.msg_type = 4607
end

function CSActivityWorldTeamArenaSceneReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)   --0 请求所有排名信息(打开排行榜) 1 请求关闭排行榜 2 请求战斗 p1 被挑战的玩家id
    MsgAdapter.WriteInt(self.param1)
end

local function WorldTeamArenaRankInfo()
    local t = {}
	t.team_unique_id = MsgAdapter.ReadUInt2()                 -- 战队id
    t.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)    -- 战队名称
    t.total_score = MsgAdapter.ReadUInt()                     -- 总积分
    t.victory_times = MsgAdapter.ReadUInt()                   -- 胜利场次
    t.fight_times = MsgAdapter.ReadUInt()                     -- 战斗场次

    return t
end 

-- 4608 英雄会武排行榜信息
SCActivityWorldTeamArenaRankAllInfo = SCActivityWorldTeamArenaRankAllInfo or BaseClass(BaseProtocolStruct)
function SCActivityWorldTeamArenaRankAllInfo:__init()
    self.msg_type = 4608
end

function SCActivityWorldTeamArenaRankAllInfo:Decode()
    self.resi_team_num =  MsgAdapter.ReadInt() -- 剩余战队数量
    self.list_num =  MsgAdapter.ReadInt()

    self.rank_list = {}
    for i = 1,self.list_num do 
        local vo = WorldTeamArenaRankInfo()
        vo.rank = i
        table.insert(self.rank_list,vo)
    end 
end 

-- 4609 英雄会武自身战队排行榜信息
SCActivityWorldTeamArenaMyInfo = SCActivityWorldTeamArenaMyInfo or BaseClass(BaseProtocolStruct)
function SCActivityWorldTeamArenaMyInfo:__init()
    self.msg_type = 4609
end

function SCActivityWorldTeamArenaMyInfo:Decode()
    self.type =  MsgAdapter.ReadInt() -- 0 发送我的排行信息 1 通知显示排行榜结果及下发我的排行信息
    self.rank_posi =  MsgAdapter.ReadInt() -- 排名位置 - 0 表示未上榜

    self.my_info = WorldTeamArenaRankInfo()  -- 我的排行信息
end 

-- 4610 英雄会武特殊信息下发
SCActivityWorldTeamArenaSpecialInfo = SCActivityWorldTeamArenaSpecialInfo or BaseClass(BaseProtocolStruct)
function SCActivityWorldTeamArenaSpecialInfo:__init()
    self.msg_type = 4610
end

function SCActivityWorldTeamArenaSpecialInfo:Decode()
    self.resi_team_num = MsgAdapter.ReadInt() --剩余战队数量
end 

local function WorldTeamArenaTeamSpecialInfo()
    local t = {}
    t.role_uid = MsgAdapter.ReadInt()
    t.rank_list = {}
    for i = 1,3 do 
        t.rank_list[i]= MsgAdapter.ReadInt()
    end 
    return t
end

-- 4611 英雄会武特殊队伍信息下发
-- 报名界面使用
SCActivityWorldTeamArenaTeamSpecialInfo = SCActivityWorldTeamArenaTeamSpecialInfo or BaseClass(BaseProtocolStruct)
function SCActivityWorldTeamArenaTeamSpecialInfo:__init()
    self.msg_type = 4611
end

function SCActivityWorldTeamArenaTeamSpecialInfo:Decode()
    self.member_list = {}
    for i = 1,5 do 
        local vo = WorldTeamArenaTeamSpecialInfo()
        table.insert(self.member_list,vo)
    end 
end 

local function WorldTeamArenaUserInfo()
    local t = {}
    t.role_uid = MsgAdapter.ReadInt()
    t.profession = MsgAdapter.ReadInt()
    t.level = MsgAdapter.ReadShort()
    t.top_level = MsgAdapter.ReadShort()
    t.avatar_type = MsgAdapter.ReadShort()
    t.headshot_id = MsgAdapter.ReadShort()
    t.fly_flag = MsgAdapter.ReadShort()
    t.reserve_sh = MsgAdapter.ReadShort()
    t.capablity = MsgAdapter.ReadInt()
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)

    return t
end

local function WorldTeamArenaTeamFightInfo()
    local t = {}
    t.rank_posi = MsgAdapter.ReadInt()
    t.team_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.member_count = MsgAdapter.ReadInt()
    t.member_list = {}
    for i= 1,t.member_count do 
        t.member_list[i] = WorldTeamArenaUserInfo()
    end 
    return t
end

local function WorldTeamArenaFightInfo()
    local t = {}
    t.attack_team = WorldTeamArenaTeamFightInfo()
    t.defend_team = WorldTeamArenaTeamFightInfo()
    return t
end


-- 4612 英雄会武发起战斗信息下发
SCActivityWorldTeamArenaFightInfo = SCActivityWorldTeamArenaFightInfo or BaseClass(BaseProtocolStruct)
function SCActivityWorldTeamArenaFightInfo:__init()
    self.msg_type = 4612
end

function SCActivityWorldTeamArenaFightInfo:Decode()
    self.start_fight_timestamp = MsgAdapter.ReadUInt()
    self.fight_info = WorldTeamArenaFightInfo()
end 


-- 4613 英雄会武最终榜单请求
CSActivityWorldTeamArenaRankReq = CSActivityWorldTeamArenaRankReq or BaseClass(BaseProtocolStruct)
function CSActivityWorldTeamArenaRankReq:__init()
    self.msg_type = 4613
end

function CSActivityWorldTeamArenaRankReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

-- 4614 英雄会武最终榜单信息下发
SCActivityWorldTeamArenaFinallyRankInfo = SCActivityWorldTeamArenaFinallyRankInfo or BaseClass(BaseProtocolStruct)
function SCActivityWorldTeamArenaFinallyRankInfo:__init()
    self.msg_type = 4614
end

function SCActivityWorldTeamArenaFinallyRankInfo:Decode()
    self.rank_count = MsgAdapter.ReadInt()
    self.my_rank_index = MsgAdapter.ReadInt()
    self.my_rank_info = WorldTeamArenaRankInfo()
    self.rank_list = {}
    for i = 1,self.rank_count do 
        self.rank_list[i] = WorldTeamArenaRankInfo()
        self.rank_list[i].rank = i
    end 
end

SCActivityWorldTeamArenaOtherInfo = SCActivityWorldTeamArenaOtherInfo or  BaseClass(BaseProtocolStruct)
function SCActivityWorldTeamArenaOtherInfo:__init()
    self.msg_type = 4615
end 

function SCActivityWorldTeamArenaOtherInfo:Decode()
    self.is_sign_up = MsgAdapter.ReadInt()
end

-- 山海灯会请求
CSLanternReq = CSLanternReq or BaseClass(BaseProtocolStruct)
function CSLanternReq:__init()
    self.msg_type = 4670
end

function CSLanternReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

-- 山海灯会NPC以及题目生成下发
SCLanternNPCQuestionInfo = SCLanternNPCQuestionInfo or BaseClass(BaseProtocolStruct)
function SCLanternNPCQuestionInfo:__init()
    self.msg_type = 4671
end

function SCLanternNPCQuestionInfo:Decode()
    local info = {}
    info.npc_seq = MsgAdapter.ReadInt()
    info.question_id = MsgAdapter.ReadInt()
    self.info = info
end

-- 山海灯会答题时间戳下发
SCLanternTimeInfo = SCLanternTimeInfo or BaseClass(BaseProtocolStruct)
function SCLanternTimeInfo:__init()
    self.msg_type = 4672
end

function SCLanternTimeInfo:Decode()
    self.timestamp = MsgAdapter.ReadUInt()
end

-- 山海灯会回答后结果下发
SCLanternAnswerResultInfo = SCLanternAnswerResultInfo or BaseClass(BaseProtocolStruct)
function SCLanternAnswerResultInfo:__init()
    self.msg_type = 4673
end

function SCLanternAnswerResultInfo:Decode()
    local info = {}
    info.answerer_id = MsgAdapter.ReadInt() --答题者回答id
    info.answerer_answer = MsgAdapter.ReadInt() --答题者uid
    info.satisfy = MsgAdapter.ReadInt() --满意度
    info.result = MsgAdapter.ReadInt() --是否正确
    self.info = info
end

-- 山海灯会登录下发
SCLanternBaseInfo = SCLanternBaseInfo or BaseClass(BaseProtocolStruct)
function SCLanternBaseInfo:__init()
    self.msg_type = 4674
end

function SCLanternBaseInfo:Decode()
    local info = {}
    info.day_start_flag = MsgAdapter.ReadChar() --今日是否已接取任务
    info.is_get_last_reward = MsgAdapter.ReadChar() --今日是否已领取最终奖励
    info.answer_count = MsgAdapter.ReadShort() --今日答题数量
    info.satisfy = MsgAdapter.ReadInt() --满意度
    info.card = MsgAdapter.ReadInt() --卡
    self.info = info
end

-- 山海灯会点赞和道具信息下发
SCLanternPraiseInfo = SCLanternPraiseInfo or BaseClass(BaseProtocolStruct)
function SCLanternPraiseInfo:__init()
    self.msg_type = 4675
end

function SCLanternPraiseInfo:Decode()
    local info = {}
    for i = 1, 5 do
        info[i] = {}
        info[i].uid = MsgAdapter.ReadInt() --uid
        info[i].is_praise = MsgAdapter.ReadInt() --是否点赞过别人
        info[i].card = MsgAdapter.ReadInt() --卡
        info[i].word_id = MsgAdapter.ReadInt() --气泡id
    end
    self.info = info
end

-- 山海灯会飘字专用
SCLanternTxtInfo = SCLanternTxtInfo or BaseClass(BaseProtocolStruct)
function SCLanternTxtInfo:__init()
    self.msg_type = 4676
end

function SCLanternTxtInfo:Decode()
    self.op_type = MsgAdapter.ReadInt() --类型
    self.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN) --玩家名字
    self.param = MsgAdapter.ReadInt() --如果是使用卡这个就是卡枚举
end

-- 山海灯会使用卡
SCLanternUseCardInfo = SCLanternUseCardInfo or BaseClass(BaseProtocolStruct)
function SCLanternUseCardInfo:__init()
    self.msg_type = 4677
end

function SCLanternUseCardInfo:Decode()
    local info = {}
    info.card_type = MsgAdapter.ReadInt() --卡
    info.uid = MsgAdapter.ReadInt() --卡
    info.param = MsgAdapter.ReadInt() --免答：使用者id，换题：新题目id，排错：错误答案id
    self.info = info
end

-- 山海灯会最终结算下发
SCLanternLastResultInfo = SCLanternLastResultInfo or BaseClass(BaseProtocolStruct)
function SCLanternLastResultInfo:__init()
    self.msg_type = 4678
end

function SCLanternLastResultInfo:Decode()
    local info = {}
    for i = 1, 5 do
        info[i] = {}
        info[i].uid = MsgAdapter.ReadInt() --uid
        info[i].satisfy = MsgAdapter.ReadInt() --满意度
        info[i].true_num = MsgAdapter.ReadShort() --正确题目数
        info[i].total_num = MsgAdapter.ReadShort() --总题目数
        info[i].praise_num = MsgAdapter.ReadInt() --被点赞数
    end
    self.info = info
end

--情缘对对碰请求
CSQingYuanDuiDuiPengReq = CSQingYuanDuiDuiPengReq or BaseClass(BaseProtocolStruct)
function CSQingYuanDuiDuiPengReq:__init()
    self.msg_type = 4660
end

function CSQingYuanDuiDuiPengReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

--开始
SCQingYuanDuiDuiPengStartNotice = SCQingYuanDuiDuiPengStartNotice or BaseClass(BaseProtocolStruct)
function SCQingYuanDuiDuiPengStartNotice:__init()
    self.msg_type = 4661
end

function SCQingYuanDuiDuiPengStartNotice:Decode()
    self.begin_game_time = MsgAdapter.ReadUInt()
    self.end_game_time = MsgAdapter.ReadUInt()
end

--使用幻化
SCQingYuanDuiDuiRoleForm = SCQingYuanDuiDuiRoleForm or BaseClass(BaseProtocolStruct)
function SCQingYuanDuiDuiRoleForm:__init()
    self.msg_type = 4662
end

function SCQingYuanDuiDuiRoleForm:Decode()
    self.role_form = {}
    for i = 1, 2 do
        local uid = MsgAdapter.ReadInt()
        local form = MsgAdapter.ReadInt()
        self.role_form[uid] = form
    end
    -- LOG(self, "幻化改变")
end

--情缘对对碰副本信息
SCQingYuanDuiDuiFbInfo = SCQingYuanDuiDuiFbInfo or BaseClass(BaseProtocolStruct)
function SCQingYuanDuiDuiFbInfo:__init()
    self.msg_type = 4663
end

function SCQingYuanDuiDuiFbInfo:Decode()
    self.integral = MsgAdapter.ReadInt() --当前积分
    self.succ_match = MsgAdapter.ReadInt() --当前匹配数量
    self.reward_level = MsgAdapter.ReadInt() --奖励等级段
    -- LOG("副本信息   integral->" .. self.integral .. "     succ_match->" .. self.succ_match .. "    reward_level->" .. self.reward_level)
end

--情缘对对碰结束
SCQingYuanDuiDuiFinish = SCQingYuanDuiDuiFinish or BaseClass(BaseProtocolStruct)
function SCQingYuanDuiDuiFinish:__init()
    self.msg_type = 4664
end

function SCQingYuanDuiDuiFinish:Decode()
    self.is_finish = MsgAdapter.ReadInt() --是否结束
    -- LOG("情侣对对碰结束 " .. self.is_finish)
end

SCQingYuanDuiDuiRankInfo = SCQingYuanDuiDuiRankInfo or BaseClass(BaseProtocolStruct)
function SCQingYuanDuiDuiRankInfo:__init()
    self.msg_type = 4665
end

function SCQingYuanDuiDuiRankInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.rank_list = {}
    for i = 1, self.count do
        local data = {}
        data.uid = MsgAdapter.ReadInt()
        data.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        data.jifen = MsgAdapter.ReadShort()
        data.pass_time = MsgAdapter.ReadShort()
        table.insert(self.rank_list, data)
    end
    -- LOG(self, "对对碰排行信息")
end

SCQingYuanDuiDuiDayTimesInfo = SCQingYuanDuiDuiDayTimesInfo or BaseClass(BaseProtocolStruct)
function SCQingYuanDuiDuiDayTimesInfo:__init()
    self.msg_type = 4666
end

function SCQingYuanDuiDuiDayTimesInfo:Decode()
    self.day_count = MsgAdapter.ReadInt()
    -- LOG("对对碰次数" .. self.day_count)
end

--情缘小铺请求
CSLoversMarketReq = CSLoversMarketReq or BaseClass(BaseProtocolStruct)
function CSLoversMarketReq:__init()
    self.msg_type = 4685
end

function CSLoversMarketReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

--情缘小铺商品信息
SCLoversMarketInfo = SCLoversMarketInfo or BaseClass(BaseProtocolStruct)
function SCLoversMarketInfo:__init()
    self.msg_type = 4686
end

function SCLoversMarketInfo:Decode()
    self.store_list = {}
    for i = 0, MAX_LOVERS_MARKET_STORE_COUNT - 1 do
        self.store_list[i] = MsgAdapter.ReadUChar()
    end
end

-- 情缘盛会 玩家操作
CSQingYuanShengHuiReq = CSQingYuanShengHuiReq or BaseClass(BaseProtocolStruct)
function CSQingYuanShengHuiReq:__init()
    self.msg_type = 4650
end

function CSQingYuanShengHuiReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

-- 情缘盛会 场景信息
SCQingYuanShengHuiSceneInfo = SCQingYuanShengHuiSceneInfo or BaseClass(BaseProtocolStruct)
function SCQingYuanShengHuiSceneInfo:__init()
    self.msg_type = 4651
end

function SCQingYuanShengHuiSceneInfo:Decode()
    self.info = {
        link_seq = MsgAdapter.ReadInt(),    -- 当前阶段
        link_begin_time = MsgAdapter.ReadInt(),-- 环节起始时间
        link_end_time = MsgAdapter.ReadInt(),  -- 环节结束时间
        role_num = MsgAdapter.ReadInt(), -- 活动人数
    }
end

-- 情缘盛会 角色信息
SCQingYuanShengHuiRoleInfo = SCQingYuanShengHuiRoleInfo or BaseClass(BaseProtocolStruct)
function SCQingYuanShengHuiRoleInfo:__init()
    self.msg_type = 4652
end

function SCQingYuanShengHuiRoleInfo:Decode()
    self.info = {
        can_gather_next_time = MsgAdapter.ReadUInt(),       -- 下次能采集时间
        battle_rewards_times = MsgAdapter.ReadShort(),      -- 获取战斗奖励次数
        is_get_qi_yuan_rewards = MsgAdapter.ReadShort(),    -- 是否已经祈愿
        battle_reward_flag = MsgAdapter.ReadInt(),          -- 获取战斗奖励标志
    }
end

-- 情缘盛会 答题结果
SCQingYuanShengHuiQuestionResult = SCQingYuanShengHuiQuestionResult or BaseClass(BaseProtocolStruct)
function SCQingYuanShengHuiQuestionResult:__init()
    self.msg_type = 4653
end

function SCQingYuanShengHuiQuestionResult:Decode()
    self.info = {
        is_same = MsgAdapter.ReadInt(),     -- 双方选择是否相同
        question_id = MsgAdapter.ReadInt(), -- 题目id
    }
end

-- 情缘盛会 答题转发
SCQingYuanShengHuiQuestionRoute = SCQingYuanShengHuiQuestionRoute or BaseClass(BaseProtocolStruct)
function SCQingYuanShengHuiQuestionRoute:__init()
    self.msg_type = 4654
end

function SCQingYuanShengHuiQuestionRoute:Decode()
    self.info = {
        is_open = MsgAdapter.ReadInt(),
    }
end

-- 情缘盛会 题目信息
SCQingYuanShengHuiQuestionInfo = SCQingYuanShengHuiQuestionInfo or BaseClass(BaseProtocolStruct)
function SCQingYuanShengHuiQuestionInfo:__init()
    self.msg_type = 4655
end

function SCQingYuanShengHuiQuestionInfo:Decode()
    self.info = {
        question_id = MsgAdapter.ReadInt(),     -- 题目id
        end_time = MsgAdapter.ReadUInt(),       -- 当前题目时间
        quest_num = MsgAdapter.ReadInt(),       -- 进度
    }
end

-- 情缘盛会 匹配对手信息
SCQingYuanShengHuiOpponentInfo = SCQingYuanShengHuiOpponentInfo or BaseClass(BaseProtocolStruct)
function SCQingYuanShengHuiOpponentInfo:__init()
    self.msg_type = 4656
end

function SCQingYuanShengHuiOpponentInfo:Decode()
    self.info = {
        start_battle_time = MsgAdapter.ReadUInt()       -- 开始战斗时间 匹配到人后多久开打 时间戳
    }
    self.role_infos = {}
    for i = 1, MarryPartyConfig.OPPONENT_NUM do
        table.insert(self.role_infos, {
            uid = MsgAdapter.ReadInt(),
            name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
            level = MsgAdapter.ReadInt(),
            prof = MsgAdapter.ReadInt(),
            avatar_type = MsgAdapter.ReadShort(),
            avatar_id = MsgAdapter.ReadShort(),
        })
    end
end

-- 情缘盛会 匹配信息
SCQingYuanShengHuiMatchOpRoute = SCQingYuanShengHuiMatchOpRoute or BaseClass(BaseProtocolStruct)
function SCQingYuanShengHuiMatchOpRoute:__init()
    self.msg_type = 4657
end

function SCQingYuanShengHuiMatchOpRoute:Decode()
    self.info = {
        is_open = MsgAdapter.ReadShort(),
        is_match = MsgAdapter.ReadShort(),
        begin_time = MsgAdapter.ReadUInt(),
    }
end

-- 情缘盛会 预约信息
SCQingYuanShengHuiInfo = SCQingYuanShengHuiInfo or BaseClass(BaseProtocolStruct)
function SCQingYuanShengHuiInfo:__init()
    self.msg_type = 4658
end

function SCQingYuanShengHuiInfo:Decode()
    self.info = {
        is_order = MsgAdapter.ReadInt(),    -- 是否已经预约
        role_num = MsgAdapter.ReadInt(),    -- 预约人数
    }
end

--缘牵同心结
CSConcentricKnotReq = CSConcentricKnotReq or BaseClass(BaseProtocolStruct)
function CSConcentricKnotReq:__init()
    self.msg_type = 4690
end

function CSConcentricKnotReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    -- LogError("req_type = " .. self.req_type .. "   param1 = " .. self.param1)
end

--队伍确认
SCConcentricKnotAgreeNotice = SCConcentricKnotAgreeNotice or BaseClass(BaseProtocolStruct)
function SCConcentricKnotAgreeNotice:__init()
    self.msg_type = 4691
end

function SCConcentricKnotAgreeNotice:Decode()
    self.uid = MsgAdapter.ReadInt()
    self.is_agree = MsgAdapter.ReadInt()
    -- local role_name = self.uid == RoleData.Instance:GetRoleId() and "自己-->" or "对方-->"
    -- LOG("队伍确认---------》  " .. role_name .. "     is_agree = " .. self.is_agree)
end

--缘牵同心结开始
SCConcentricKnotStart = SCConcentricKnotStart or BaseClass(BaseProtocolStruct)
function SCConcentricKnotStart:__init()
    self.msg_type = 4692
end

function SCConcentricKnotStart:Decode()
    self.cards_group = MsgAdapter.ReadInt()
    self.begin_time = MsgAdapter.ReadUInt()
    self.end_time = MsgAdapter.ReadUInt()
    -- LOG("cards_group = " .. self.cards_group .. "    begin_time = " .. self.begin_time .. "     end_time = " .. self.end_time)
end

--缘牵同心结翻牌
SCConcentricKnotOpenCard = SCConcentricKnotOpenCard or BaseClass(BaseProtocolStruct)
function SCConcentricKnotOpenCard:__init()
    self.msg_type = 4693
end

function SCConcentricKnotOpenCard:Decode()
    self.uid = MsgAdapter.ReadInt()
    self.card_id = MsgAdapter.ReadInt()
    -- local role_name = self.uid == RoleData.Instance:GetRoleId() and "自己-->" or "对方-->"
    -- LogError("翻牌：" .. role_name .. "    card_id" .. self.card_id)
end

--缘牵同心结翻牌
SCConcentricKnotEndGame = SCConcentricKnotEndGame or BaseClass(BaseProtocolStruct)
function SCConcentricKnotEndGame:__init()
    self.msg_type = 4694
end

function SCConcentricKnotEndGame:Decode()
    self.is_finish = MsgAdapter.ReadInt()
    -- LOG("翻牌结束 is_finish = " .. self.is_finish)
end

SCConcentricKnotInfo = SCConcentricKnotInfo or BaseClass(BaseProtocolStruct)
function SCConcentricKnotInfo:__init()
    self.msg_type = 4695
end

function SCConcentricKnotInfo:Decode()
    self.reward_times = MsgAdapter.ReadInt()
    self.day_count = self.reward_times
    -- LogError("day_count = " .. self.day_count)
end

