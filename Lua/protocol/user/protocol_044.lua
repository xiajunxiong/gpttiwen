
--------------论剑九州------------------------
--赛季信息
SCWorldArenaInfo = SCWorldArenaInfo or BaseClass(BaseProtocolStruct)
function SCWorldArenaInfo:__init()
    self.msg_type = 4400
end

function SCWorldArenaInfo:Decode()
    self.already_signup_num = MsgAdapter.ReadInt()  --已报名人数
    self.cur_status = MsgAdapter.ReadShort()    --当前状态
    MsgAdapter.ReadShort()  ---reserve
    self.next_status_timestampe = MsgAdapter.ReadUInt()
    self.season_index = MsgAdapter.ReadShort()
    self.total_group_num = MsgAdapter.ReadShort()
end


----玩家报名信息
SCWorldArenaUserSignupInfo = SCWorldArenaUserSignupInfo or BaseClass(BaseProtocolStruct)
function SCWorldArenaUserSignupInfo:__init()
    self.msg_type = 4401
end

function SCWorldArenaUserSignupInfo:Decode()
    self.has_siginup = MsgAdapter.ReadShort()  --是否已报名 0，未报名
    MsgAdapter.ReadShort()  ---reserve
end

---论剑九州玩家基础信息
SCWorldArenaUserBaseInfo = SCWorldArenaUserBaseInfo or BaseClass(BaseProtocolStruct)
function SCWorldArenaUserBaseInfo:__init()
    self.msg_type = 4404
end

function SCWorldArenaUserBaseInfo:Decode()
    self.has_siginup = MsgAdapter.ReadChar()  --是否已报名 0，未报名
    self.has_fetch_signup_reward = MsgAdapter.ReadChar()
    self.cur_rank = MsgAdapter.ReadShort() + 1 --服务端的自己名次从0开始，这里+1
    self.group_index = MsgAdapter.ReadShort() + 1
    self.win_num = MsgAdapter.ReadShort()
    self.score = MsgAdapter.ReadShort()
    self.daily_reward_fetch_flag = MsgAdapter.ReadChar()    --每日奖励领取标记（0：不可领取 1：可领取 2：已领取）
    MsgAdapter.ReadChar()
    self.win_box_reward_flag = MsgAdapter.ReadInt()
end


--玩家操作请求
CSWorldArenaOperate =  CSWorldArenaOperate or BaseClass(BaseProtocolStruct)
function  CSWorldArenaOperate:__init()
    self.msg_type = 4402
end

function  CSWorldArenaOperate:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.oper_type)   --0:获取信息，1:报名，2:查看赛程信息 LunJianCtrl.OP
    MsgAdapter.WriteShort(0)  ---reserve
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

----玩家赛程信息
SCWorldArenaUserScheduleInfo = SCWorldArenaUserScheduleInfo or BaseClass(BaseProtocolStruct)

function SCWorldArenaUserScheduleInfo.ScheduleParse()
    local re = {}
    re.plat_type = MsgAdapter.ReadInt() --spid(用于查玩家详细数据)
    re.uid = MsgAdapter.ReadInt()       --角色id 用于查玩家详细信息
    re.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)  --角色名字
    re.level = MsgAdapter.ReadShort()           --等级
    re.avatar_type = MsgAdapter.ReadShort()    --模型
    re.headshot_id = MsgAdapter.ReadInt()       --头像id
    re.profession = MsgAdapter.ReadInt()           --职业(基础职业与进阶次数结合)
    re.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)--家庭名
    re.rank = MsgAdapter.ReadInt()  --全服排名
    re.round_idx = MsgAdapter.ReadInt() + 1 --轮数 取值范围[0,19)
    re.result = MsgAdapter.ReadShort()  --结果 0:未出结果，1:胜,2:负,3:平
    MsgAdapter.ReadShort()
    return re
end

function SCWorldArenaUserScheduleInfo:__init()
    self.msg_type = 4403
end

function SCWorldArenaUserScheduleInfo:Decode()
    self.has_schedule = MsgAdapter.ReadShort()  --是否已分组 0,未分组
    self.group_idx = MsgAdapter.ReadShort() + 1 --分组序号[0,总分组数)
    self.schedule_list = {}
    for i=1,LunJianData.SCHEDULE_COUNT do
        table.insert(self.schedule_list,SCWorldArenaUserScheduleInfo.ScheduleParse())
    end
end

SCWorldArenaGroupRankList = SCWorldArenaGroupRankList or BaseClass(BaseProtocolStruct)

--组内排行榜信息（已有序，按钮顺序显示即可）
function SCWorldArenaGroupRankList:__init()
    self.msg_type = 4405
end

function SCWorldArenaGroupRankList:Decode()
    self.group_idx = MsgAdapter.ReadInt() + 1 --分组序号[0,总分组数)
    local user_num = MsgAdapter.ReadInt()
    self.top3_info_list = {}
    for i=1,3 do
        self.top3_info_list[i] = Appearance()
    end
    self.user_list = {}
    for i=1,user_num do
        local rank_user_info = {}
        rank_user_info.plat_type = MsgAdapter.ReadInt()
        rank_user_info.uid = MsgAdapter.ReadInt()
        rank_user_info.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        rank_user_info.profession = MsgAdapter.ReadInt()
        rank_user_info.win_num = MsgAdapter.ReadShort()
        rank_user_info.lose_num = MsgAdapter.ReadShort()
        rank_user_info.draw_num = MsgAdapter.ReadShort()
        rank_user_info.total_num = MsgAdapter.ReadShort() --总场数 = 胜+平+负+弃权数
        rank_user_info.score = MsgAdapter.ReadInt()
        rank_user_info.rank = i
        table.insert(self.user_list,rank_user_info)
    end
end

--倒计时进入战斗
SCWorldArenaCountDownToFight = SCWorldArenaCountDownToFight or BaseClass(BaseProtocolStruct)

function SCWorldArenaCountDownToFight:__init()
    self.msg_type = 4406
end

function SCWorldArenaCountDownToFight:Decode()
    self.count_down_s = MsgAdapter.ReadInt()    --倒数秒数
end 

--开始进场通知
SCWorldArenaStartEnterSceneNotice = SCWorldArenaStartEnterSceneNotice or BaseClass(BaseProtocolStruct)

function SCWorldArenaStartEnterSceneNotice:__init()
    self.msg_type = 4407
end

function SCWorldArenaStartEnterSceneNotice:Decode()
    self.enterscene_end_timestamp = MsgAdapter.ReadUInt()
    self.start_round_index = MsgAdapter.ReadShort()
    self.has_finish_this_round = MsgAdapter.ReadShort()
end

--到场信息
SCWorldArenaShowUpInfo = SCWorldArenaShowUpInfo or BaseClass(BaseProtocolStruct)

function SCWorldArenaShowUpInfo:__init()
    self.msg_type = 4408
end

function SCWorldArenaShowUpInfo.ParseRoleInfo(tab)
    tab.plat_type = MsgAdapter.ReadInt()
    tab.uid = MsgAdapter.ReadInt()
    tab.level = MsgAdapter.ReadInt()
    tab.profession = MsgAdapter.ReadInt()
    tab.avatar_type = MsgAdapter.ReadShort()
    tab.headshot_id = MsgAdapter.ReadShort()
    tab.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
end

function SCWorldArenaShowUpInfo:Decode()
    local is_attacker_show_up = MsgAdapter.ReadShort()
    local is_defender_show_up = MsgAdapter.ReadShort()
    self.attacker_info = {}
    self.attacker_info.is_show_up = is_attacker_show_up ~= 0
    SCWorldArenaShowUpInfo.ParseRoleInfo(self.attacker_info)
    self.defender_info = {}
    self.defender_info.is_show_up = is_defender_show_up ~= 0
    SCWorldArenaShowUpInfo.ParseRoleInfo(self.defender_info)
end

--------------论剑九州结束------------------------

-- 宠物飞升传记奖励
SCPetTaleReward = SCPetTaleReward or BaseClass(BaseProtocolStruct)
function SCPetTaleReward:__init()
    self.msg_type = 4420
end

function SCPetTaleReward:Decode()
    local count = MsgAdapter.ReadInt()
    local info_list = {}
    for i = 1,count do
        local mutant_id = MsgAdapter.ReadShort()
        info_list[mutant_id] = {}
        info_list[mutant_id].mutant_id = mutant_id
        info_list[mutant_id].finish_flag = bit:d8b(MsgAdapter.ReadUChar())
        info_list[mutant_id].fetch_flag = bit:d8b(MsgAdapter.ReadUChar())
    end
    self.info_list = info_list
end

-- 情缘时装
SCRALoveFashion = SCRALoveFashion or BaseClass(BaseProtocolStruct)
function SCRALoveFashion:__init()
    self.msg_type = 4425
end

function SCRALoveFashion:Decode()
    self.buy_mark = bit:SAnd({MsgAdapter.ReadShort()})    --购买标记
    self.fetch_mark = bit:SAnd({MsgAdapter.ReadShort()})  --领取标记
    self.is_open = MsgAdapter.ReadShort()
    self.self_buy_mark = bit:SAnd({MsgAdapter.ReadShort()})  --是否自己购买标记
end

-- 情缘累充
SCRAQingYuanLeiChongInfo = SCRAQingYuanLeiChongInfo or BaseClass(BaseProtocolStruct)
function SCRAQingYuanLeiChongInfo:__init()
    self.msg_type = 4426
end

function SCRAQingYuanLeiChongInfo:Decode()
    self.join_begin_timestamp = MsgAdapter.ReadUInt()   --开始时间戳
    self.join_end_timestamp = MsgAdapter.ReadUInt()     --结束时间戳
    self.chongzhi_gold = MsgAdapter.ReadInt()           --充值金额
    self.fetch_mark = MsgAdapter.IntFlag()              --领取标记
end

-- 宠物碎片批量合成
CSPetComposeBatch = CSPetComposeBatch or BaseClass(BaseProtocolStruct)
function CSPetComposeBatch:__init()
    self.msg_type = 4430
end

function CSPetComposeBatch:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    local info_list = self.info_list
    for i,v in pairs(info_list) do
        for i = 1, v.num do
            MsgAdapter.WriteShort(v.seq)
            MsgAdapter.WriteShort(0)
        end
    end
end

-- 宠物碎片批量合成并分解
CSPetComposeBatchAndDec = CSPetComposeBatchAndDec or BaseClass(BaseProtocolStruct)
function CSPetComposeBatchAndDec:__init()
    self.msg_type = 4432
end

function CSPetComposeBatchAndDec:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    local info_list = self.info_list
    local index = 1
    for i,v in pairs(info_list) do
        MsgAdapter.WriteShort(v.seq)
        MsgAdapter.WriteShort(v.num)
        index = index + 1
    end
    -- for i = index, 100 do
    --     MsgAdapter.WriteShort(0)
    --     MsgAdapter.WriteShort(0)
    -- end
end

SCPetComposeBatchRet = SCPetComposeBatchRet or BaseClass(BaseProtocolStruct)
function SCPetComposeBatchRet:__init()
    self.msg_type = 4431
end

function SCPetComposeBatchRet:Decode()
    local count = MsgAdapter.ReadInt()
    self.pet_index_list = {}
    for i = 1,count do
        self.pet_index_list[i] = MsgAdapter.ReadShort()
    end
end

-- 跳过任务通用
CSTaskSkipReq = CSTaskSkipReq or BaseClass(BaseProtocolStruct)
function CSTaskSkipReq:__init()
    self.msg_type = 4434
end

function CSTaskSkipReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.task_id)
end

-- 天下至尊令 玩家操作 0角色数据 1参与活动 p_1:活动雕像obj_id 2领取奖励 p_1:活动雕像obj_id
CSPeaktokenRoleReq = CSPeaktokenRoleReq or BaseClass(BaseProtocolStruct)
function CSPeaktokenRoleReq:__init()
    self.msg_type = 4455
end

function CSPeaktokenRoleReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    -- LogError(self)
end

--天下至尊令 角色参与通知
SCPeaktokenRoleStatusInfo = SCPeaktokenRoleStatusInfo or BaseClass(BaseProtocolStruct)
function SCPeaktokenRoleStatusInfo:__init()
    self.msg_type = 4457
end

function SCPeaktokenRoleStatusInfo:Decode()
    local info = {}
    info.id = MsgAdapter.ReadLL()
    info.join_time = MsgAdapter.ReadUInt()
    info.reward_times = MsgAdapter.ReadInt()
    self.id = info.id
    self.info = info
    -- LogError(self.info)
end

--天下至尊令 角色奖励通知
SCPeaktokenRoleJoinInfo = SCPeaktokenRoleJoinInfo or BaseClass(BaseProtocolStruct)
function SCPeaktokenRoleJoinInfo:__init()
    self.msg_type = 4458
end

function SCPeaktokenRoleJoinInfo:Decode()
    local info = {}
    info.id = MsgAdapter.ReadLL()
    info.join_time = MsgAdapter.ReadUInt()
    info.reward_times = MsgAdapter.ReadInt()
    self.item = {
        item_id = MsgAdapter.ReadUShort(),--奖励道具id
        num = MsgAdapter.ReadShort(),     --奖励物品数量
        is_bind = 1
    }
    self.id = info.id
    self.info = info
    -- LogError(self)
end

--天下至尊令 玩家信息
SCPeaktokenRoleInfo = SCPeaktokenRoleInfo or BaseClass(BaseProtocolStruct)
function SCPeaktokenRoleInfo:__init()
    self.msg_type = 4459
end

function SCPeaktokenRoleInfo:Decode()
    self.today_use_times = MsgAdapter.ReadInt()--今日使用次数
    -- LogError(self)
end

--收益统计，
CSIncomeRoleReq = CSIncomeRoleReq or BaseClass(BaseProtocolStruct)
function CSIncomeRoleReq:__init()
    self.msg_type = 4460
end

function CSIncomeRoleReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
end

SCIncomeRoleInfo = SCIncomeRoleInfo or BaseClass(BaseProtocolStruct)
function SCIncomeRoleInfo:__init()
    self.msg_type = 4461
end

function SCIncomeRoleInfo:Decode()
    self.role_count = MsgAdapter.ReadInt()
    self.role_param_list = {}
    for i = 0, IncomeRoleType.TYPE_MAX - 1 do
        self.role_param_list[i] = MsgAdapter.ReadLL()
    end
    self.global_count = MsgAdapter.ReadInt()
    self.global_param_list = {}
    for i = 0, IncomeGlobalType.TYPE_MAX - 1 do
        self.global_param_list[i] = MsgAdapter.ReadLL()
    end
end

-- 天下第一比武大会获取冠军记录
CSGetUTAChampionRecord = CSGetUTAChampionRecord or BaseClass(BaseProtocolStruct)
function CSGetUTAChampionRecord:__init()
    self.msg_type = 4462
end

function CSGetUTAChampionRecord:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.begin_index)
    MsgAdapter.WriteInt(self.end_index)
end

-- 天下第一比武大会冠军手册信息
SCUTAChampionRecordInfo = SCUTAChampionRecordInfo or BaseClass(BaseProtocolStruct)
function SCUTAChampionRecordInfo:__init()
    self.msg_type = 4463
end

function SCUTAChampionRecordInfo:Decode()
    local count = MsgAdapter.ReadInt()
end

local function WritePartnerIntensifyItem(item_id, num)
    item_id = item_id or 0
    num = num or 0
    MsgAdapter.WriteUShort(item_id or 0)
    MsgAdapter.WriteUShort(num or 0)
end

-- 伙伴强化
CSPartnerIntensifyReq = CSPartnerIntensifyReq or BaseClass(BaseProtocolStruct)
function CSPartnerIntensifyReq:__init()
    self.msg_type = 4465
end

function CSPartnerIntensifyReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.partner_id)
    
    WritePartnerIntensifyItem(next(self.expend_item_1), self.expend_item_1[next(self.expend_item_1)])
    WritePartnerIntensifyItem(next(self.expend_item_3), self.expend_item_3[next(self.expend_item_3)])

    MsgAdapter.WriteInt(self.expend_item_2_count)
    for item_id, num in pairs(self.expend_item_2_list) do
        WritePartnerIntensifyItem(item_id, num)
    end
end

-- 伙伴强化成功返回
SCPartnerIntensifyInfo = SCPartnerIntensifyInfo or BaseClass(BaseProtocolStruct)
function SCPartnerIntensifyInfo:__init()
    self.msg_type = 4466
end

function SCPartnerIntensifyInfo:Decode()
    self.partner_id = MsgAdapter.ReadShort()
    self.intensify_level = MsgAdapter.ReadShort() + 1
end

-- 师徒传功操作
CSShiTuChuanGongFBReq = CSShiTuChuanGongFBReq or BaseClass(BaseProtocolStruct)
function CSShiTuChuanGongFBReq:__init()
    self.msg_type = 4470
end

function CSShiTuChuanGongFBReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

-- 游戏信息通知
SCShiTuChuanGongGameInfo = SCShiTuChuanGongGameInfo or BaseClass(BaseProtocolStruct)
function SCShiTuChuanGongGameInfo:__init()
    self.msg_type = 4471
end

function SCShiTuChuanGongGameInfo:Decode()
    self.type = MsgAdapter.ReadInt()
    self.param = MsgAdapter.ReadUInt()
end

-- 师徒传功场景信息
SCShiTuChuanGongSceneInfo = SCShiTuChuanGongSceneInfo or BaseClass(BaseProtocolStruct)
function SCShiTuChuanGongSceneInfo:__init()
    self.msg_type = 4472
end

function SCShiTuChuanGongSceneInfo:Decode()
    self.fb_begin_time = MsgAdapter.ReadUInt()
    self.host_list = {}
    self.count = MsgAdapter.ReadInt()
    for i = 1, self.count do
        self.host_list[i] = {
            uid = MsgAdapter.ReadInt(),
            name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
            appearance =  Appearance(),
        }
    end
end

-- 师徒通知
SCShiTuSyncInfo = SCShiTuSyncInfo or BaseClass(BaseProtocolStruct)
function SCShiTuSyncInfo:__init()
    self.msg_type = 4473
end

function SCShiTuSyncInfo:Decode()
    self.type = MsgAdapter.ReadInt()        -- 0初始化 1更新
    self.param_1 = MsgAdapter.ReadInt()     -- 师傅uid
    self.param_2 = MsgAdapter.ReadInt()     -- 0全未完成 1副本未完成 2传功未完成
end