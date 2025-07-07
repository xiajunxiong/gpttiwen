-- 跨服英雄录请求
CSKuaFuYingXiongLuReq = CSKuaFuYingXiongLuReq or BaseClass(BaseProtocolStruct)
function CSKuaFuYingXiongLuReq:__init()
    self.msg_type = 5310
end

function CSKuaFuYingXiongLuReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

-- 跨服英雄录活动信息下发
SCKuaFuYingXiongLuActivityInfo = SCKuaFuYingXiongLuActivityInfo or BaseClass(BaseProtocolStruct)
function SCKuaFuYingXiongLuActivityInfo:__init()
    self.msg_type = 5311
end

function SCKuaFuYingXiongLuActivityInfo:Decode()
    local info = {}
    info.cross_open_day = MsgAdapter.ReadInt() -- 跨服开服天数
    info.open_timestamp = MsgAdapter.ReadUInt() -- 开启时间戳
    info.end_timestamp = MsgAdapter.ReadUInt() -- 结束时间戳
    info.count = MsgAdapter.ReadInt()
    local state_end_times = {}
    for i = 0, info.count - 1 do
        state_end_times[i] = MsgAdapter.ReadUInt()
    end
    info.state_end_times = state_end_times -- 排行榜结束时间戳
    self.info = info
end

-- 跨服英雄录角色所有信息下发
SCKuaFuYingXiongLuRoleAllInfo = SCKuaFuYingXiongLuRoleAllInfo or BaseClass(BaseProtocolStruct)
function SCKuaFuYingXiongLuRoleAllInfo:__init()
    self.msg_type = 5312
end

function SCKuaFuYingXiongLuRoleAllInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.role_info = {}
    for i = 0, self.count - 1 do
        self.role_info[i] = KFHeroRecordData.DecodeRank(i)
    end
end

-- 跨服英雄录角色单个榜单信息下发
SCKuaFuYingXiongLuRoleSingleRankInfo = SCKuaFuYingXiongLuRoleSingleRankInfo or BaseClass(BaseProtocolStruct)
function SCKuaFuYingXiongLuRoleSingleRankInfo:__init()
    self.msg_type = 5313
end

function SCKuaFuYingXiongLuRoleSingleRankInfo:Decode()
    self.rank_type = MsgAdapter.ReadInt()
    self.rank_info = KFHeroRecordData.DecodeRank(self.rank_type)
end

-- 跨服英雄录角色单个任务信息下发
SCKuaFuYingXiongLuRoleSingleTaskInfo = SCKuaFuYingXiongLuRoleSingleTaskInfo or BaseClass(BaseProtocolStruct)
function SCKuaFuYingXiongLuRoleSingleTaskInfo:__init()
    self.msg_type = 5314
end

function SCKuaFuYingXiongLuRoleSingleTaskInfo:Decode()
    self.rank_type = MsgAdapter.ReadInt()
    self.seq = MsgAdapter.ReadInt()
    self.info = {}
    local task_type = MsgAdapter.ReadUChar()
    self.info.is_get = MsgAdapter.ReadChar() -- 奖励领取标识
    MsgAdapter.ReadShort()
    self.info.param = MsgAdapter.ReadShort() -- 任务进度
end

-- 跨服英雄录角色榜单数值列表信息下发
SCKuaFuYingXiongLuRoleRankValue = SCKuaFuYingXiongLuRoleRankValue or BaseClass(BaseProtocolStruct)
function SCKuaFuYingXiongLuRoleRankValue:__init()
    self.msg_type = 5315
end

function SCKuaFuYingXiongLuRoleRankValue:Decode()
    self.count = MsgAdapter.ReadInt()
    self.value_info = {}
    for i = 0, self.count - 1 do
        self.value_info[i] = MsgAdapter.ReadInt()
    end
end


-- ================ 星图 ==============
CSStarMapReq = CSStarMapReq or BaseClass(BaseProtocolStruct)
function CSStarMapReq:__init()
    self.msg_type = 5320
end

function CSStarMapReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
    MsgAdapter.WriteInt(self.p3)
    --LOG("星图请求", self)
end

-- //!<5321 星图系统信息下发
SCStarChartAllInfo = SCStarChartAllInfo or BaseClass(BaseProtocolStruct)
function SCStarChartAllInfo:__init()
    self.msg_type = 5321
end

function SCStarChartAllInfo:Decode()
    self.supper_star_list = {}  -- 超星系
    for i = 0, 167 do
        self.supper_star_list[i] = MsgAdapter.ReadChar()
    end
    self.star_list = {}         -- 星图
    self.stage_list = {[0] = 0,0,0} -- 星图第几阶
    for i = 0, 2 do
        self.star_list[i] = {}
        for j = 0, 4 do
            self.star_list[i][j] = {}
            local num = 0
            for k = 0, 19 do
                self.star_list[i][j][k] = MsgAdapter.ReadChar()
                if self.star_list[i][j][k] > 0 then
                    num = num + 1
                end
            end
            if num >= 19 then
                self.stage_list[i] = j + 1
            end
        end
    end

    self.star_skill = {[0] = {},{},{}}
    for i = 0, 2 do
        for j = 0, 2 do
            self.star_skill[j][i] = MsgAdapter.ReadInt()
        end
    end
    --LOG("5321 星图系统信息下发", self)
    --LOG("星图阶数", self.stage_list)
end

-- 星图装备技能通知
SCStarMapInstallSkillNotice = SCStarMapInstallSkillNotice or BaseClass(BaseProtocolStruct)
function SCStarMapInstallSkillNotice:__init()
    self.msg_type = 5322
end

function SCStarMapInstallSkillNotice:Decode()
    self.notice_type = MsgAdapter.ReadInt()     -- 0 装备技能， 1激活技能
    self.super_seq = MsgAdapter.ReadInt()
    --LOG("AA",self)
end

-- 随机活动-月夕灯谜 请求
CSRAYueXiDengMReq = CSRAYueXiDengMReq or BaseClass(BaseProtocolStruct)
function CSRAYueXiDengMReq:__init()
    self.msg_type = 5340
end

function CSRAYueXiDengMReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteStrN(self.user_answer, YXDM_MAX_ANSWER_LENGTH)
end

-- 随机活动-月夕灯谜 活动信息下发
SCRAYueXiDengMiInfo = SCRAYueXiDengMiInfo or BaseClass(BaseProtocolStruct)
function SCRAYueXiDengMiInfo:__init()
    self.msg_type = 5341
end

function SCRAYueXiDengMiInfo:Decode()
    self.info = {}
    self.info.cur_question_id = MsgAdapter.ReadInt()
    self.info.today_answer_count = MsgAdapter.ReadInt()
end

-- 随机活动-月夕灯谜 答题结果下发
SCRAYueXiDengMiAnswerResult = SCRAYueXiDengMiAnswerResult or BaseClass(BaseProtocolStruct)
function SCRAYueXiDengMiAnswerResult:__init()
    self.msg_type = 5343
end

function SCRAYueXiDengMiAnswerResult:Decode()
    self.info = {}
    self.info.is_true = MsgAdapter.ReadInt() -- 回答是否正确 0-错误 1-正确
end

-- 中秋活动-月夕累充 信息下发
SCRAYueXiLeiChongInfo = SCRAYueXiLeiChongInfo or BaseClass(BaseProtocolStruct)
function SCRAYueXiLeiChongInfo:__init()
    self.msg_type = 5342
end

function SCRAYueXiLeiChongInfo:Decode()
    self.m_total_chongzhi = MsgAdapter.ReadInt() -- 活动期间累充金额
    self.m_receive_reward_flag = MsgAdapter.IntFlag(0) -- 奖励领取标识
end

-- 中秋活动-追月商城 信息下发
SCRAZhuiYueShangDianInfo = SCRAZhuiYueShangDianInfo or BaseClass(BaseProtocolStruct)
function SCRAZhuiYueShangDianInfo:__init()
    self.msg_type = 5350
end

function SCRAZhuiYueShangDianInfo:Decode()
    self.ra_day_buy_times = {}
    self.ra_buy_times = {}
    self.item_list_select = {}
    for i = 0, RA_ZhuiYueShangCheng.ZYSC_MAX_BUY_ITEM - 1, 1 do
        self.ra_day_buy_times[i] = MsgAdapter.ReadUShort()
    end
    for i = 0, RA_ZhuiYueShangCheng.ZYSC_MAX_BUY_ITEM - 1, 1 do
        self.ra_buy_times[i] = MsgAdapter.ReadUShort()
    end
    local RASelItem = function()
        local t = {}
        t.is_select = MsgAdapter.ReadUChar()
        t.re_ch = MsgAdapter.ReadUChar()
        t.re_sh = MsgAdapter.ReadUShort()
        t.reward_group_id = MsgAdapter.ReadInt()
        t.reward_seq = MsgAdapter.ReadInt()
        t.config = ActZyscCtrl.Instance:Get_ZqzyRewardBySeq(t.reward_seq, 1)
        return t
    end
    local val_sel = false
    for i = 0, RA_ZhuiYueShangCheng.ZYSC_SELECT_NUM - 1, 1 do
        local t = RASelItem()
        if val_sel == false and t.reward_seq ~= 0 then
            val_sel = true
        end
        self.item_list_select[i] = t
    end
    self.val_sel = val_sel
end

-- 中秋活动-追月商城 抽奖记录下发
SCRAZhuiYueShangDianDrawRecord = SCRAZhuiYueShangDianDrawRecord or BaseClass(BaseProtocolStruct)
function SCRAZhuiYueShangDianDrawRecord:__init()
    self.msg_type = 5352
end
function SCRAZhuiYueShangDianDrawRecord:Decode()
    self.count = MsgAdapter.ReadInt()
    self.record_list = {}
    for i = 1, self.count, 1 do
        local data = {}
        data.time = MsgAdapter.ReadUInt()
        data.sort_param = MsgAdapter.ReadInt()
        data.reward_index = MsgAdapter.ReadInt()
        table.insert(self.record_list, data)
    end
end

-- 中秋活动-追月商城 抽奖回调
SCRAZhuiYueShangDianDrawRet = SCRAZhuiYueShangDianDrawRet or BaseClass(BaseProtocolStruct)
function SCRAZhuiYueShangDianDrawRet:__init()
    self.msg_type = 5351
end
function SCRAZhuiYueShangDianDrawRet:Decode()
    self.count = MsgAdapter.ReadInt()
    self.item_list = {}
    for i = 1, self.count, 1 do
        self.item_list[i] = MsgAdapter.ReadInt()
    end
end

-- 单笔返利所有信息下发
SCRADanBiFanLiInfo = SCRADanBiFanLiInfo or BaseClass(BaseProtocolStruct)
function SCRADanBiFanLiInfo:__init()
    self.msg_type = 5345
end

function SCRADanBiFanLiInfo:Decode()
    local info = {}
    -- 每日购买次数
    info.today_buy_times = {}
    for i = 0, 9 do
        info.today_buy_times[i] = MsgAdapter.ReadShort()
    end
    -- 活动购买次数
    info.act_buy_times = {}
    for i = 0, 9 do
        info.act_buy_times[i] = MsgAdapter.ReadShort()
    end
    self.info = info
end

-- 单笔返利单条信息下发
SCRADanBiFanLiSingleInfo = SCRADanBiFanLiSingleInfo or BaseClass(BaseProtocolStruct)
function SCRADanBiFanLiSingleInfo:__init()
    self.msg_type = 5346
end

function SCRADanBiFanLiSingleInfo:Decode()
    MsgAdapter.ReadShort()
    self.pool_seq = MsgAdapter.ReadShort()
    self.today_buy_times = MsgAdapter.ReadShort()
    self.act_buy_times = MsgAdapter.ReadShort()
end




-- 幻兽请求
CSHuanShouReq = CSHuanShouReq or BaseClass(BaseProtocolStruct)
function CSHuanShouReq:__init()
    self.msg_type = 5360
end

function CSHuanShouReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
    MsgAdapter.WriteInt(self.p3)
    --LOG("幻兽请求",self)
end

-- //!< 5364 幻兽灵魄分解请求
CSHuanShouDecomposeReq = CSHuanShouDecomposeReq or BaseClass(BaseProtocolStruct)
function CSHuanShouDecomposeReq:__init()
    self.msg_type = 5364
end

function CSHuanShouDecomposeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    local list = self.list or {}
    MsgAdapter.WriteInt(self.column_type or 0)
    for i,v in pairs(list) do
        MsgAdapter.WriteShort(v)
    end
    --LOG("幻兽灵魄分解请求",num,self)
end


-- 所有幻兽信息
SCHuanShouAllInfo = SCHuanShouAllInfo or BaseClass(BaseProtocolStruct)
function SCHuanShouAllInfo:__init()
    self.msg_type = 5361
end

function SCHuanShouAllInfo:Decode()
    local egg_count = MsgAdapter.ReadShort()
    self.open_egg_times_list = {}   -- 幻兽开蛋次数记录 服务端是累加的，用的时候记得求模
    for i = 0, egg_count - 1 do
        self.open_egg_times_list[i] = MsgAdapter.ReadInt()
    end

    local count = MsgAdapter.ReadShort()
    self.huan_shou_info_list = {}
    for i = 1, count do
        local info = HuanShou.Decode()
        self.huan_shou_info_list[i] = info
    end
    --LOG("所有幻兽信息",count,self)
end


-- 单个幻兽信息
SCHuanShouInfo = SCHuanShouInfo or BaseClass(BaseProtocolStruct)
function SCHuanShouInfo:__init()
    self.msg_type = 5362
end

function SCHuanShouInfo:Decode()
    self.info = HuanShou.Decode()
    --LOG("单个幻兽信息",self)
end


-- //!< 5363 幻兽信息通知下发
SCHuanShouNoticeInfo = SCHuanShouNoticeInfo or BaseClass(BaseProtocolStruct)
function SCHuanShouNoticeInfo:__init()
    self.msg_type = 5363
end

function SCHuanShouNoticeInfo:Decode()
    local info = {}
    info.notice_type = MsgAdapter.ReadShort()
    info.p1 = MsgAdapter.ReadShort()
    info.p2 = MsgAdapter.ReadInt()
    info.p3 = MsgAdapter.ReadInt()
    self.info = info
    --LOG("幻兽信息通知下发 通知类型 == ", info.notice_type , self)
end



-- //!< 5365 幻兽其它信息下发
SCHuanShouOtherInfo = SCHuanShouOtherInfo or BaseClass(BaseProtocolStruct)
function SCHuanShouOtherInfo:__init()
    self.msg_type = 5365
end

function SCHuanShouOtherInfo:Decode()
    local info = {}
    info.fushen_id = MsgAdapter.ReadChar()                  -- //此时附身的幻兽ID(id等同于序号)(-1:没附身)
    info.is_use_huanshou = MsgAdapter.ReadChar()            -- 是否使用幻兽形象
    info.last_lian_dan_count = MsgAdapter.ReadShort()       -- //丹炉当前炼丹次数
    info.start_lian_dan_timestamp = MsgAdapter.ReadUInt()   -- //开始炼丹时间戳
    self.info = info
    --LOG("幻兽其它信息", self.info)
end


-- 幻兽开蛋记录
SCHuanShouEggRecordInfo = SCHuanShouEggRecordInfo or BaseClass(BaseProtocolStruct)
function SCHuanShouEggRecordInfo:__init()
    self.msg_type = 5366
end

function SCHuanShouEggRecordInfo:Decode()
    MsgAdapter.ReadShort()
    local count = MsgAdapter.ReadShort()
    self.list = {}
    for i = count, 1,-1 do
        local t = {}
        t.role_uid = MsgAdapter.ReadInt()
        t.player_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.egg_item_id = MsgAdapter.ReadInt()
        t.huanshou_id = MsgAdapter.ReadInt()
        self.list[i] = t
    end
    --LOG("幻兽开蛋记录", count, self)
end


 --//!< 5370 幻兽背包所有信息下发
SCHuanShouBagInfo = SCHuanShouBagInfo or BaseClass(BaseProtocolStruct)
function SCHuanShouBagInfo:__init()
    self.msg_type = 5370
end

function SCHuanShouBagInfo:Decode()
    local item_count = MsgAdapter.ReadShort()
    self.item_list = {}
    for i = 1, item_count do
        local vo = {}
        vo.item_id = MsgAdapter.ReadUShort()
        vo.index = MsgAdapter.ReadShort()
        vo.is_market_buy_flag = MsgAdapter.ReadChar()
        vo.num = MsgAdapter.ReadInt()
        vo.is_bind = MsgAdapter.ReadChar()
        vo.has_param = MsgAdapter.ReadChar()
        vo.invalid_time = MsgAdapter.ReadUInt()     -- 过期时间
        vo.item_unique_id_1 = MsgAdapter.ReadInt()
        vo.item_unique_id_2 = MsgAdapter.ReadInt()

        vo.param= {}
        if vo.has_param == 1 then
            MsgAdapter.ReadShort()
            for i = 1, 1024 do
                MsgAdapter.ReadChar()
            end
        end
        local item = Item.Create(vo)
        self.item_list[vo.index] = item
    end
    --LOG("幻兽背包所有信息下发", self)
end



 -- //!< 5371 幻兽背包单条信息下发
SCHuanShouBagItemInfo = SCHuanShouBagItemInfo or BaseClass(BaseProtocolStruct)
function SCHuanShouBagItemInfo:__init()
    self.msg_type = 5371
end

function SCHuanShouBagItemInfo:Decode()
    self.reason = MsgAdapter.ReadShort()
    self.put_reason = MsgAdapter.ReadShort()
    local vo = {}
    vo.item_id = MsgAdapter.ReadUShort()
    vo.index = MsgAdapter.ReadShort()
    vo.is_market_buy_flag = MsgAdapter.ReadChar()
    vo.num = MsgAdapter.ReadInt()
    vo.is_bind = MsgAdapter.ReadChar()
    vo.has_param = MsgAdapter.ReadChar()
    vo.invalid_time = MsgAdapter.ReadUInt()     -- 过期时间
    vo.item_unique_id_1 = MsgAdapter.ReadInt()
    vo.item_unique_id_2 = MsgAdapter.ReadInt()

    vo.param= {}
    if vo.has_param == 1 then
        MsgAdapter.ReadShort()
        for i = 1, 1024 do
            MsgAdapter.ReadChar()
        end
    end
    self.item = Item.Create(vo)
    --LOG("幻兽背包单条信息下发", self)
end

--每日挂机经验
SCGuaJiExpInfo = SCGuaJiExpInfo or BaseClass(BaseProtocolStruct)
function SCGuaJiExpInfo:__init()
    self.msg_type = 5355
end

function SCGuaJiExpInfo:Decode()
    self.today_add_guaji_exp = MsgAdapter.ReadUInt()
end


-- 随机活动圣诞密令
SCRAShengDanMiLingInfo = SCRAShengDanMiLingInfo or BaseClass(BaseProtocolStruct)
function SCRAShengDanMiLingInfo:__init()
    self.msg_type = 5353
end

function SCRAShengDanMiLingInfo:Decode()
    local info = {}
    info.finish_flag = MsgAdapter.IntFlag()         -- 密令任务完成flag
    info.receive_flag = MsgAdapter.IntFlag()        -- 登录奖励领取flag
    info.miling_receive_flag = MsgAdapter.IntFlag() -- 密令奖励领取flag
    self.info = info
    LOG("随机活动圣诞密令", info)
end

-- 随机活动 迎春纳福 活动信息下发
SCRAYingChunNaFuInfo = SCRAYingChunNaFuInfo or BaseClass(BaseProtocolStruct)
function SCRAYingChunNaFuInfo:__init()
    self.msg_type = 5357
end

function SCRAYingChunNaFuInfo:Decode()
    local info = {}
    info.count = MsgAdapter.ReadInt()
    info.rewards_log = {}
    for i = 1, 50 do
        local log = {}
        log.timestamp = MsgAdapter.ReadUInt()
        log.item_id = MsgAdapter.ReadUShort()
        log.is_bind = MsgAdapter.ReadShort()
        log.num = MsgAdapter.ReadInt()
        if log.item_id ~= 0 then
           table.insert(info.rewards_log, log) 
        end
    end
    self.info = info
    -- LOG("迎春纳福 活动信息", info)
end

--随机活动 迎春纳福 抽奖信息
SCRAYingChunNaFuNotice  = SCRAYingChunNaFuNotice  or BaseClass(BaseProtocolStruct)
function SCRAYingChunNaFuNotice:__init()
    self.msg_type = 5358
end

function SCRAYingChunNaFuNotice:Decode()
    local info = {}
    local count = MsgAdapter.ReadInt()
    info.list={}
    for i=1,count do
        local reward={}
        local item_id=MsgAdapter.ReadUShort()
        local is_bind=MsgAdapter.ReadChar()
        reward.is_award=MsgAdapter.ReadChar()  --是否是大奖
        local num=MsgAdapter.ReadInt()   
        reward.item = {item_id=item_id,is_bind=is_bind,num=num}
        table.insert(info.list,reward)
    end
    self.info = info
    -- LOG("迎春纳福 抽奖信息", info)
end

SCRAStarLightingInfo = SCRAStarLightingInfo or BaseClass(BaseProtocolStruct)
function SCRAStarLightingInfo:__init()
    self.msg_type = 5356
end
function SCRAStarLightingInfo:Decode()
    self.lottery_count = MsgAdapter.ReadInt()
    self.receive_flag = MsgAdapter.IntFlag()
    -- LOG("星星电灯", self)
end