
-- 鸿蒙天界请求
CSHongMengTianJieReq = CSHongMengTianJieReq or BaseClass(BaseProtocolStruct)
function CSHongMengTianJieReq:__init()
    self.msg_type = 5100
end

function CSHongMengTianJieReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param_1)
    MsgAdapter.WriteInt(self.param_2)
end

-- 鸿蒙天界场景信息
SCHongMengTianJieSceneInfo = SCHongMengTianJieSceneInfo or BaseClass(BaseProtocolStruct)
function SCHongMengTianJieSceneInfo:__init()
    self.msg_type = 5101
end

function SCHongMengTianJieSceneInfo:Decode()
    self.info = {
        stage = MsgAdapter.ReadShort(),     -- 阶段
        fb_id = MsgAdapter.ReadShort(),
    }
end

-- 鸿蒙天界角色信息
SCHongMengTianJieRoleInfo = SCHongMengTianJieRoleInfo or BaseClass(BaseProtocolStruct)
function SCHongMengTianJieRoleInfo:__init()
    self.msg_type = 5102
end

function SCHongMengTianJieRoleInfo:Decode()
    local t = {
        help_times = MsgAdapter.ReadChar(),     -- 助战次数
        reward_times = {},
        limit_buy_times = {},
        rewards_times_all = 0,
        rewards_times_max = 0,
    }
    for i = 1, HarmonyHeavenConfig.MAX_HONGMENG_TIANJIE_BOSS_NUM do
        local time = MsgAdapter.ReadChar()
        t.reward_times[i] = time
        t.rewards_times_all = t.rewards_times_all + time
        t.rewards_times_max = math.max(t.rewards_times_max, time)
    end
    for i = 0, HarmonyHeavenConfig.MAX_LIMIT_SEQ_NUM - 1 do
        t.limit_buy_times[i] = MsgAdapter.ReadChar()
    end
    MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    t.fb_first_pass_mark = MsgAdapter.IntFlag(2)

    self.info = t
end

-- 全服首通信息
SCHongMengTianJieWorldInfo = SCHongMengTianJieWorldInfo or BaseClass(BaseProtocolStruct)
function SCHongMengTianJieWorldInfo:__init()
    self.msg_type = 5103
end

function SCHongMengTianJieWorldInfo:Decode()
    local t = {
        first_pass_time = {}
    }
    for i = 0, HarmonyHeavenConfig.MAX_FB_INDEX_NUM - 1 do
        t.first_pass_time[i] = MsgAdapter.ReadUInt()
    end
    self.info = t
end

-- 场景对话广播
SCHongMengTianJieTalkInfo = SCHongMengTianJieTalkInfo or BaseClass(BaseProtocolStruct)
function SCHongMengTianJieTalkInfo:__init()
    self.msg_type = 5104
end

function SCHongMengTianJieTalkInfo:Decode()
    self.info = {
        talk_id = MsgAdapter.ReadInt(),
        talk_seq = MsgAdapter.ReadInt(),
    }
end

-- 个人副本奖励统计
SCHongMengTianJieRewardInfo = SCHongMengTianJieRewardInfo or BaseClass(BaseProtocolStruct)
function SCHongMengTianJieRewardInfo:__init()
    self.msg_type = 5105
end

function SCHongMengTianJieRewardInfo:Decode()
    local t = {
        count = MsgAdapter.ReadInt(),
        item_list = {}
    }
    for i = 1, t.count do
        t.item_list[i] = {
            item_id = MsgAdapter.ReadUShort(),
            num = MsgAdapter.ReadShort(),
        }
    end
    self.info = t
end

-- 首通队伍
SCHongMengTianJiePassTeamInfo = SCHongMengTianJiePassTeamInfo or BaseClass(BaseProtocolStruct)
function SCHongMengTianJiePassTeamInfo:__init()
    self.msg_type = 5106
end

function SCHongMengTianJiePassTeamInfo:Decode()
    local t = {
        fb_id = MsgAdapter.ReadInt(),
        member_list = {}
    }
    for i = 1, 5 do
        t.member_list[i] = {
            uid = MsgAdapter.ReadInt(),
            level = MsgAdapter.ReadInt(),
            prof = MsgAdapter.ReadInt(),
            name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
            appearance = Appearance(),
        }
    end
    self.info = t
end

-- 造化装备操作2请求
CSZaoHuaEquipmentReq = CSZaoHuaEquipmentReq or BaseClass(BaseProtocolStruct)
function CSZaoHuaEquipmentReq:__init()
    self.msg_type = 5120
end

function CSZaoHuaEquipmentReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
end

-- 造化装备精炼信息下发
SCZaoHuaReFineInfo = SCZaoHuaReFineInfo or BaseClass(BaseProtocolStruct)
function SCZaoHuaReFineInfo:__init()
    self.msg_type = 5121
end

function SCZaoHuaReFineInfo:Decode()
    self.info = {}
    for i = 0, 7 do
        self.info[i] = MsgAdapter.ReadInt()
    end
end

CSZaoHuaYuanHunZhuReq = CSZaoHuaYuanHunZhuReq or BaseClass(BaseProtocolStruct)
function CSZaoHuaYuanHunZhuReq:__init()
    self.msg_type = 5122
end

function CSZaoHuaYuanHunZhuReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteShort(self.param1)
    MsgAdapter.WriteShort(self.param2)
    MsgAdapter.WriteShort(self.param3)
    MsgAdapter.WriteShort(self.param4)
    MsgAdapter.WriteShort(self.param5)
    MsgAdapter.WriteShort(self.param6)
    MsgAdapter.WriteShort(self.param7)
    MsgAdapter.WriteShort(self.param8)
end

CSYuanHunZhuUpLevelReq = CSYuanHunZhuUpLevelReq or BaseClass(BaseProtocolStruct) 
function CSYuanHunZhuUpLevelReq:__init()
    self.msg_type = 5123
end

function CSYuanHunZhuUpLevelReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.item_src)
    MsgAdapter.WriteShort(self.index)
    MsgAdapter.WriteShort(self.column)
    MsgAdapter.WriteShort(self.slot_index)
    -- MsgAdapter.WriteInt(self.buy_item_num) -- 这个买多少个最低级
    -- for i = 1,self.num do 
    --     MsgAdapter.WriteUShort(self.consume_list[i])
    -- end 
end

-- 珑铸单个部位的单个孔单个属性信息
local function SingleLongZhuSlotAttrPro(index)
    local t = {}
    t.index = index
    t.affix_id = MsgAdapter.ReadInt()
    t.level = MsgAdapter.ReadInt()
    return t
end

-- 珑铸单个部位的单个孔信息
local function SingleLongZhuSlotPro(index, is_unlock)
    local t = {}
    t.index = index
    t.is_unlock = is_unlock
    t.history_level = MsgAdapter.ReadUShort()   --历史最高等级
    t.now_times = MsgAdapter.ReadUShort()       --当前抽了多少升级的保底次数
    t.now_attr = SingleLongZhuSlotAttrPro(0)    --当前的属性
    --10个备选属性
    local attr_list = {}
    for i = 1, 10 do
        attr_list[i] = SingleLongZhuSlotAttrPro(i)
    end
    t.attr_list = attr_list
    return t
end

-- 珑铸单个部位信息
local function SingleLongZhuPro(equip_type)
    local t = {}
    t.equip_type = equip_type                   --装备部位
    local slot_info = {}
    local slot_unlock_flag = MsgAdapter.IntFlag()   --是否解锁
    for i = 0, 4 do
        -- 前三个孔一定解锁，前三个服务端不记录，都是0来的，客户端写死前三是1
        slot_info[i] = SingleLongZhuSlotPro(i, i < 3 and 1 or slot_unlock_flag[i])
    end
    t.slot_info = slot_info
    return t
end

-- 造化装备珑铸全部信息下发
SCZaoHuaLongZhuInfo = SCZaoHuaLongZhuInfo or BaseClass(BaseProtocolStruct)
function SCZaoHuaLongZhuInfo:__init()
    self.msg_type = 5124
end

function SCZaoHuaLongZhuInfo:Decode()
    self.info = {}
    for i = 0, 7 do
        self.info[i] = SingleLongZhuPro(i)
    end
end

-- 造化装备珑铸单个孔位信息下发
SCZaoHuaLongZhuSingleInfo = SCZaoHuaLongZhuSingleInfo or BaseClass(BaseProtocolStruct)
function SCZaoHuaLongZhuSingleInfo:__init()
    self.msg_type = 5125
end

function SCZaoHuaLongZhuSingleInfo:Decode()
    self.equip_type = MsgAdapter.ReadInt()
    self.slot_index = MsgAdapter.ReadInt()
    self.info = SingleLongZhuSlotPro(self.slot_index, MarbasData.Instance:GetBatheOneSlotUnLock(self.equip_type, self.slot_index))
end

-- 端阳自选礼
SCRADuanYangZiXuanLiInfo = SCRADuanYangZiXuanLiInfo or BaseClass(BaseProtocolStruct)
function SCRADuanYangZiXuanLiInfo:__init()
    self.msg_type = 5130
end

function SCRADuanYangZiXuanLiInfo:Decode()
    local info = {}
    info.is_buy_reward = MsgAdapter.ReadInt()
    info.fetch_reward_flag = MsgAdapter.IntFlag(1,true)
    info.select_reward_seq = {}
    for i = 1, 32 do
        -- LOG("i",i)
        info.select_reward_seq[i] = MsgAdapter.ReadUChar()
    end
    self.info = info
    -- self.is_buy_reward = MsgAdapter.ReadInt()
    -- self.fetch_reward_flag = MsgAdapter.ReadInt()
    --LOG("端阳自选礼SCRADuanYangZiXuanLiInfo",self.info)
end

-- 端阳福礼
SCRADuanYangFuLiInfo = SCRADuanYangFuLiInfo or BaseClass(BaseProtocolStruct)
function SCRADuanYangFuLiInfo:__init()
    self.msg_type = 5131
end

function SCRADuanYangFuLiInfo:Decode()
    local info = {item_list={}}
    for i = 0, 20-1 do
        -- LOG("i",i)
        -- local t = {}
        info.item_list[i]={}
        info.item_list[i].is_buy ={}
        info.item_list[i].fetch_reward_times = {}
        info.item_list[i].is_buy = MsgAdapter.ReadShort()
        info.item_list[i].fetch_reward_times = MsgAdapter.ReadShort()
        -- info[i] = t
    end

    self.info = info


    
    -- self.is_buy_reward = MsgAdapter.ReadInt()
    -- self.fetch_reward_flag = MsgAdapter.ReadInt()
    --LOG("端阳福礼SCRADuanYangFuLiInfo",self.info)
end

-- 5115 金龙聚宝信息
SCRAJinLongJuBao = SCRAJinLongJuBao or BaseClass(BaseProtocolStruct)
function SCRAJinLongJuBao:__init()
    self.msg_type = 5135
end

local function RAJingLongJuBaoItem()
    local t = {}
    t.is_get = MsgAdapter.ReadUChar()
    t.is_select = MsgAdapter.ReadUChar()
    t.reserved_sh = MsgAdapter.ReadUShort()
    t.reward_group_id = MsgAdapter.ReadInt()
    t.reward_seq = MsgAdapter.ReadInt()
    return t
end 

function SCRAJinLongJuBao:Decode()
    self.reason = MsgAdapter.ReadInt()                             -- 下发理由 1设置，2领取次数奖励，3直购
    self.ra_day_buy_times = {}
    for i = 1,20 do 
        self.ra_day_buy_times[i] = MsgAdapter.ReadUShort()         -- 每种物品的每日限购次数
    end 
    self.ra_buy_times = {}
    for i = 1,20 do 
        self.ra_buy_times[i] = MsgAdapter.ReadUShort()             -- 每种物品的限购次数
    end 
    self.get_all_reward_times = MsgAdapter.ReadInt()               -- 轮数
    self.times_reward_fetch_flag = MsgAdapter.ReadUInt()           -- 轮数领取标志

    self.item_list_select = {}
    for i = 0,3 do 
        self.item_list_select[i] = RAJingLongJuBaoItem()           -- 自选奖励列表
    end 
    self.item_list_rand = {}
    for i = 0,7 do 
        self.item_list_rand[i] = RAJingLongJuBaoItem()             -- 非自选奖励列表
    end  
    self.record_list = {}
    for i = 1,20 do 
        local vo = {}
        vo.time = MsgAdapter.ReadUInt()
        vo.list_index = MsgAdapter.ReadInt()
        vo.item_id = MsgAdapter.ReadUShort()
        vo.num = MsgAdapter.ReadUShort()
        table.insert(self.record_list,vo)
    end  
end

-- 5116 金龙聚宝特殊信息
SCRAJinLongJuBaoDrawRet = SCRAJinLongJuBaoDrawRet or BaseClass(BaseProtocolStruct)
function SCRAJinLongJuBaoDrawRet:__init()
    self.msg_type = 5136
end

function SCRAJinLongJuBaoDrawRet:Decode()
    self.count = MsgAdapter.ReadInt()
    
    self.item_list = {}
    for i = 1,self.count do 
        local vo = {}
        vo.time = MsgAdapter.ReadUInt() -- 抽卡时间戳
        vo.list_index = MsgAdapter.ReadInt()      -- 次序
        vo.type = MsgAdapter.ReadInt()     -- 0 自选 0 - 3；1 非自选 0 - 7
        vo.index = MsgAdapter.ReadInt()     
        table.insert(self.item_list,vo)
    end     
end

SCRAServerCompetitionAllRankInfo = SCRAServerCompetitionAllRankInfo or BaseClass(BaseProtocolStruct)
function SCRAServerCompetitionAllRankInfo:__init()
    self.msg_type = 5137
end
function SCRAServerCompetitionAllRankInfo:DecodeType(type)
    local list = {}
    for i = 1, 10, 1 do
        list[i] = self:DecodeValue(i, type)
    end
    return list
end
function SCRAServerCompetitionAllRankInfo:DecodeValue(rank, rank_type)
    local t = {}
    t.rank = rank
    t.rank_type = rank_type
    t.uid = MsgAdapter.ReadInt()
    t.rank_value = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.profession_id = MsgAdapter.ReadShort()
    t.profession = math.modf(t.profession_id / 100)
    t.level = MsgAdapter.ReadShort()
    t.top_level = MsgAdapter.ReadInt()
    t.capabolity = MsgAdapter.ReadInt()
    t.headshot_id = MsgAdapter.ReadShort() --新增头像
    t.avatar_type = MsgAdapter.ReadShort() --新增头像
    return t
end
function SCRAServerCompetitionAllRankInfo:Decode()
    self.rank_info = {}
    for i = 0, ServeractivityConfig.CompetitionRank.Type.Comprehensive do
        self.rank_info[i] = self:DecodeType(i)
    end
end

---------------------科举考试---------------------
-- 科举考试请求
CSKeJuExamCSReq = CSKeJuExamCSReq or BaseClass(BaseProtocolStruct)
function CSKeJuExamCSReq:__init()
    self.msg_type = 5140
end

function CSKeJuExamCSReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

-- 科举考试NPC以及题目生成下发
SCKeJuExamNPCInfo = SCKeJuExamNPCInfo or BaseClass(BaseProtocolStruct)
function SCKeJuExamNPCInfo:__init()
    self.msg_type = 5141
end

function SCKeJuExamNPCInfo:Decode()
    local info = {}
    info.npc_seq = MsgAdapter.ReadInt()
    info.question_id = MsgAdapter.ReadInt()
    self.info = info
end

-- 科举考试答题时间戳下发
SCKeJuExamTimeOutInfo = SCKeJuExamTimeOutInfo or BaseClass(BaseProtocolStruct)
function SCKeJuExamTimeOutInfo:__init()
    self.msg_type = 5142
end

function SCKeJuExamTimeOutInfo:Decode()
    self.answer_role_id = MsgAdapter.ReadInt()
    self.timestamp = MsgAdapter.ReadUInt()
end

-- 科举考试回答后结果下发
SCKeJuExamAnswerResultInfo = SCKeJuExamAnswerResultInfo or BaseClass(BaseProtocolStruct)
function SCKeJuExamAnswerResultInfo:__init()
    self.msg_type = 5143
end

function SCKeJuExamAnswerResultInfo:Decode()
    local info = {}
    info.answer = MsgAdapter.ReadInt() --答题者回答id 0未作答
    info.result = MsgAdapter.ReadInt() --是否正确
    self.info = info
end

-- 科举考试登录下发
SCKeJuExamBaseInfo = SCKeJuExamBaseInfo or BaseClass(BaseProtocolStruct)
function SCKeJuExamBaseInfo:__init()
    self.msg_type = 5144
end

function SCKeJuExamBaseInfo:Decode()
    local info = {}
    info.day_start_flag = MsgAdapter.ReadChar() --今日是否已接取任务
    info.is_get_last_reward = MsgAdapter.ReadChar() --今日是否已领取最终奖励
    info.answer_count = MsgAdapter.ReadShort() --今日答题数量
    info.satisfy = MsgAdapter.ReadInt() --满意度
    info.card = MsgAdapter.ReadInt() --卡
    self.info = info
end

-- 科举考试气泡和道具信息下发
SCKeJuExamPraiseInfo = SCKeJuExamPraiseInfo or BaseClass(BaseProtocolStruct)
function SCKeJuExamPraiseInfo:__init()
    self.msg_type = 5145
end

function SCKeJuExamPraiseInfo:Decode()
    local info = {}
    for i = 1, 5 do
        info[i] = {}
        info[i].uid = MsgAdapter.ReadInt() --uid
        info[i].card = MsgAdapter.ReadInt() --卡
        info[i].word_id = MsgAdapter.ReadInt() --气泡id
    end
    self.info = info
end

-- 科举考试飘字专用
SCKeJuExamNotifyInfo = SCKeJuExamNotifyInfo or BaseClass(BaseProtocolStruct)
function SCKeJuExamNotifyInfo:__init()
    self.msg_type = 5146
end

function SCKeJuExamNotifyInfo:Decode()
    self.op_type = MsgAdapter.ReadInt() --类型
    self.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN) --玩家名字
    self.param = MsgAdapter.ReadInt() --如果是使用卡这个就是卡枚举
end

-- 科举考试使用卡后下发
SCKeJuExamUseCardInfo = SCKeJuExamUseCardInfo or BaseClass(BaseProtocolStruct)
function SCKeJuExamUseCardInfo:__init()
    self.msg_type = 5147
end

function SCKeJuExamUseCardInfo:Decode()
    local info = {}
    info.card_type = MsgAdapter.ReadInt() --卡
    info.uid = MsgAdapter.ReadInt() --使用者id
    info.param = MsgAdapter.ReadInt() --免答：0，换题：新题目id，排错：错误答案id
    self.info = info
end

-- 科举考试最终结算下发
SCKeJuExamLastResultInfo = SCKeJuExamLastResultInfo or BaseClass(BaseProtocolStruct)
function SCKeJuExamLastResultInfo:__init()
    self.msg_type = 5148
end

function SCKeJuExamLastResultInfo:Decode()
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
---------------------科举考试---------------------


--  我要上学
CSGotoSchoolReq = CSGotoSchoolReq or BaseClass(BaseProtocolStruct)
function CSGotoSchoolReq:__init()
    self.msg_type = 5150
end

function CSGotoSchoolReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.p1)
    --LOG("我要上学5150请求>>>",self)
end

SCGotoSchoolInfo = SCGotoSchoolInfo or BaseClass(BaseProtocolStruct)
function SCGotoSchoolInfo:__init()
    self.msg_type = 5151
end

function SCGotoSchoolInfo:Decode()
    self.get_reward_times = MsgAdapter.ReadInt()
    --LOG("我要上学5151协议：",self)
end


--文昌书局请求
CSWenCangReq = CSWenCangReq or BaseClass(BaseProtocolStruct)
function CSLoversMarketReq:__init()
    self.msg_type = 5160
end

function CSLoversMarketReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)  -- 1 请求购买
    MsgAdapter.WriteInt(self.param1)    
end

--文昌书局信息
SCWenCangInfo = SCWenCangInfo or BaseClass(BaseProtocolStruct)
function SCWenCangInfo:__init()
    self.msg_type = 5161
end

function SCWenCangInfo:Decode()
    self.store_list = {}
    for i = 0, MAX_LOVERS_MARKET_STORE_COUNT - 1 do
        self.store_list[i] = MsgAdapter.ReadUChar()
    end
end

--伙伴/宠物前十排行榜请求
CSPatnerPetRankReq = CSPatnerPetRankReq or BaseClass(BaseProtocolStruct)
function CSPatnerPetRankReq:__init()
    self.msg_type = 5162
end

function CSPatnerPetRankReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.rank_type)  -- 0宠物排行 1伙伴排行 
end

SCPetTopRankInfo = SCPetTopRankInfo or BaseClass(BaseProtocolStruct)
function SCPetTopRankInfo:__init()
    self.msg_type = 5163
end

function SCPetTopRankInfo:Decode()
    local t = {}
    local role_id = RoleData.Instance:GetRoleId()
    for i = 0, 9 do
        local uid = MsgAdapter.ReadInt()
        local pet_id = MsgAdapter.ReadInt()
        local pet_uniq1 = MsgAdapter.ReadUInt()
        local pet_uniq2 = MsgAdapter.ReadUInt()
        if uid == role_id then
            t[tostring(pet_uniq1).."-"..tostring(pet_uniq2)] = {rank = i, pet_id = pet_id}
        end
    end
    self.info = t
end

--伙伴前十信息下发
SCPartnerTopRankInfo = SCPartnerTopRankInfo or BaseClass(BaseProtocolStruct)
function SCPartnerTopRankInfo:__init()
    self.msg_type = 5164
end

function SCPartnerTopRankInfo:Decode()
    local t = {}
    local role_id = RoleData.Instance:GetRoleId()
    for i = 1, 10 do
        local uid = MsgAdapter.ReadInt()
        local partner_id = MsgAdapter.ReadInt()
        if uid == role_id then
            t[partner_id] = i
        end
    end
    self.info = t
end

-- 宠物强化共鸣信息
SCPetResonanceInfo = SCPetResonanceInfo or BaseClass(BaseProtocolStruct)
function SCPetResonanceInfo:__init()
    self.msg_type = 5170
end

function SCPetResonanceInfo:Decode()
    local info = {}
    local past_Level_info = {}  -- 历史最高等级
    for i = 2, 5 do
        local t = {}
        past_Level_info[i] = MsgAdapter.ReadInt()
        for j = 1, MAX_RESONANCE_LEVEL do
            t[j] = {}
            t[j].uid_1 = MsgAdapter.ReadUInt()
            t[j].uid_2 = MsgAdapter.ReadUInt()
        end
        info[i] = t
    end
    self.info = info
    self.past_Level_info = past_Level_info
    --LOG("-- 宠物强化共鸣信息", self)
end

-- 神兽恩泽登陆下发
SCRAShenShouEnZeInfo = SCRAShenShouEnZeInfo or BaseClass(BaseProtocolStruct)
function SCRAShenShouEnZeInfo:__init()
    self.msg_type = 5180
end

function SCRAShenShouEnZeInfo:Decode()
    local info = {}
    info.day_buy_times = {}
    for i = 0, 19 do
        info.day_buy_times[i] = MsgAdapter.ReadUShort()
    end
    info.activity_buy_times = {}
    for i = 0, 19 do
        info.activity_buy_times[i] = MsgAdapter.ReadUShort()
    end
    self.lottery_times = MsgAdapter.ReadInt()
    info.is_select = MsgAdapter.ReadInt()
    info.select_seq = MsgAdapter.ReadInt()
    self.info = info
end

-- 神兽恩泽抽奖结果下发
SCRAShenShouEnZeDraw = SCRAShenShouEnZeDraw or BaseClass(BaseProtocolStruct)
function SCRAShenShouEnZeDraw:__init()
    self.msg_type = 5181
end

function SCRAShenShouEnZeDraw:Decode()
    self.lottery_times = MsgAdapter.ReadInt()
    self.result = {}
    for i = 1, 10 do
        local t = {}
        t.is_mall = MsgAdapter.ReadInt()
        t.seq = MsgAdapter.ReadInt()
        self.result[i] = t
    end
end

-- 神兽恩泽抽奖记录下发
SCRAShenShouEnZeRecord = SCRAShenShouEnZeRecord or BaseClass(BaseProtocolStruct)
function SCRAShenShouEnZeRecord:__init()
    self.msg_type = 5182
end

function SCRAShenShouEnZeRecord:Decode()
    self.record = {}
    for i = 1, 30 do
        local t = {}
        t.time = MsgAdapter.ReadUInt()
        t.times = MsgAdapter.ReadInt()
        t.item_id = MsgAdapter.ReadUShort()
        t.num = MsgAdapter.ReadUShort()
        self.record[i] = t
    end
end

SCElementElfRewards = SCElementElfRewards or BaseClass(BaseProtocolStruct)
function SCElementElfRewards:__init()
    self.msg_type = 5185--元素精灵奖励下发
end
function SCElementElfRewards:Decode()
    self.reward_seq = MsgAdapter.ReadInt()
    -- LogDG("元素精灵奖励 .reward_seq", self.reward_seq)
end

CSFuYuReq = CSFuYuReq or BaseClass(BaseProtocolStruct)
function CSFuYuReq:__init()
    self.msg_type = 5188
end
function CSFuYuReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
    MsgAdapter.WriteInt(self.param4)
    MsgAdapter.WriteInt(self.param5)
    MsgAdapter.WriteInt(self.param6)
    LogDG("请求协议", self)
end
local function FuYuItemInfo()
    local item = {}
    item.partner_id = MsgAdapter.ReadInt()--				//!< 伙伴id
	item.wear_idx = MsgAdapter.ReadInt()--				//!< 穿戴index(0-2)
	item.item_id = MsgAdapter.ReadUShort()--				//!< 物品id
	item.num = MsgAdapter.ReadShort()--					//!< 数量
	item.is_bind = MsgAdapter.ReadChar()--				//!< 是否绑定
	item.has_param = MsgAdapter.ReadChar()--				//!< 是否有净值参数
	item.invalid_time = MsgAdapter.ReadUInt()--		//!< 过期时间
	item.item_unique_id = MsgAdapter.ReadLL()--	//!< 道具唯一ID
    item.in_bag = false
    item.param = nil
    if item.has_param == 1 then
        item.param_length = MsgAdapter.ReadShort()--			//!< 净值参数的数据长度 （当has_param为1时可读）
        item.param = ItemDataParam(item.item_id, item.param_length)
        -- NetValueItemParamData param_data;	//!< 净值参数的数据，根据param_length的值来读取 （当has_param为1时可读）
    end
    return item
end
SCFuYuInfo = SCFuYuInfo or BaseClass(BaseProtocolStruct)
function SCFuYuInfo:__init()
    self.msg_type = 5186
end
function SCFuYuInfo:Decode()
    self.reason = MsgAdapter.ReadInt()-- 0穿戴  1卸下 2 成功 3失败 4超时失效
    self.info = FuYuItemInfo()
--    LogDG("单条信息", self)
end
SCAllFuYuInfo = SCAllFuYuInfo or BaseClass(BaseProtocolStruct)
function SCAllFuYuInfo:__init()
    self.msg_type = 5187
end
function SCAllFuYuInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.info_list = {}
    for i = 1, self.count, 1 do
        self.info_list[i] = FuYuItemInfo()
    end
--    LogDG("全部信息", self)
end

-- 四季花牌
SCRAFourSeasonCardInfo = SCRAFourSeasonCardInfo or BaseClass(BaseProtocolStruct)
function SCRAFourSeasonCardInfo:__init()
	self.msg_type = 5190
end

function SCRAFourSeasonCardInfo:Decode()
    local info = {}
	info.day_buy_times = {} --每天限购
	for i = 0, RA_FOUR_SEASON_CARD_MAX_ITEM - 1 do
		info.day_buy_times[i] = MsgAdapter.ReadUShort()
	end
	info.act_buy_times = {} --总限购
	for i = 0, RA_FOUR_SEASON_CARD_MAX_ITEM - 1 do
		info.act_buy_times[i] = MsgAdapter.ReadUShort()	
    end
    info.free_flag = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
    self.info = info
end

SCShiTuOpNotice = SCShiTuOpNotice or BaseClass(BaseProtocolStruct)
function SCShiTuOpNotice:__init()
    self.msg_type = 5191
end
function SCShiTuOpNotice:Decode()
    self.notice_type = MsgAdapter.ReadInt() -- 0 强制解除冷静时间 p1 uid 弹冷静期 1 取消强制解除冷静期 p1 uid 飘字取消成功
    self.p1 = MsgAdapter.ReadInt()
end