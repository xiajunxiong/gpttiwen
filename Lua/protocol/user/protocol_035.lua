-- 下发场景信息
SCDuChuangZeiYingSceneInfo = SCDuChuangZeiYingSceneInfo or BaseClass(BaseProtocolStruct)
function SCDuChuangZeiYingSceneInfo:__init()
    self.msg_type = 3501
end

function SCDuChuangZeiYingSceneInfo:Decode()
    local t = {
        count = MsgAdapter.ReadInt(),
        obj_info = {},
    }
    for i = 1, t.count do
        local npc_id = MsgAdapter.ReadInt()
        local status = MsgAdapter.ReadInt()
        t.obj_info[npc_id] = 1 == status
        -- table.insert(t.obj_info, {
        --     npc_id = MsgAdapter.ReadInt(),
        --     status = MsgAdapter.ReadInt(),
        -- })
    end
    self.info = t
end

-- 通知寻路
SCDuChuangZeiYingNotice = SCDuChuangZeiYingNotice or BaseClass(BaseProtocolStruct)
function SCDuChuangZeiYingNotice:__init()
    self.msg_type = 3502
end

function SCDuChuangZeiYingNotice:Decode()
    self.npc_id = MsgAdapter.ReadInt()
end

CSCatchFoxSpecialLogicReq = CSCatchFoxSpecialLogicReq or BaseClass(BaseProtocolStruct)
function CSCatchFoxSpecialLogicReq:__init()
    self.msg_type = 3503
end

function CSCatchFoxSpecialLogicReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.x)
    MsgAdapter.WriteInt(self.y)
    MsgAdapter.WriteInt(self.dir)
end

-- 翻牌请求
CSDrawReq = CSDrawReq or BaseClass(BaseProtocolStruct)
function CSDrawReq:__init()
    self.msg_type = 3560
end

function CSDrawReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.type)      --操作类型
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

--角色翻牌选择信息
SCRoleDrawInfo = SCRoleDrawInfo or BaseClass(BaseProtocolStruct)
function SCRoleDrawInfo:__init()
    self.msg_type = 3561
end

function SCRoleDrawInfo:Decode()
    self.draw_group_id = MsgAdapter.ReadInt()   --奖励组id
    self.role_uid = MsgAdapter.ReadInt()        --翻牌者uid
    self.draw_index = MsgAdapter.ReadInt()      --翻牌者选择位置[0, DRAW_GROUP_MAX_ITEM_NUM - 1]
end

--等级助力
SCLevelComplementInfo = SCLevelComplementInfo or BaseClass(BaseProtocolStruct)
function SCLevelComplementInfo:__init()
    self.msg_type = 3562
end

function SCLevelComplementInfo:Decode()
    self.accumulated_exp = MsgAdapter.ReadLL() --经验池
    self.m_refresh_accumulated_tamp = MsgAdapter.ReadUInt() --下次增加经验池时间戳
    self.m_end_accumulate_tamp = MsgAdapter.ReadUInt() --助力结束时间戳
    self.m_day_difference = MsgAdapter.ReadInt() --助力差距天数
    self.m_add_exp_times = MsgAdapter.ReadInt() --已刷新次数

    self.m_alread_get_exp = MsgAdapter.ReadLL() --累计获取经验
    self.m_save_exp = MsgAdapter.ReadLL() --累计获得存储经验
    self.m_today_accumulated_exp = MsgAdapter.ReadLL() --今天的池子经验值最大的时候
end

CSLevelComplementReq = CSLevelComplementReq or BaseClass(BaseProtocolStruct)
function CSLevelComplementReq:__init()
    self.msg_type = 3563
end

function CSLevelComplementReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.type) --操作请求
    MsgAdapter.WriteInt(self.param) --参数
end

local function DrawPoker()
    local t = {}
    t.index = MsgAdapter.ReadInt()
    t.role_uid = MsgAdapter.ReadInt()
    MsgAdapter.ReadInt()
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    ItemDrawInfo(t)
    return t
end

--奖励翻牌信息
SCDrawPokerInfo = SCDrawPokerInfo or BaseClass(BaseProtocolStruct)
function SCDrawPokerInfo:__init()
    self.msg_type = 3565
end

function SCDrawPokerInfo:Decode()
    local info = {}
    info.draw_group_id = MsgAdapter.ReadInt()
    info.timeout = MsgAdapter.ReadUInt()
    local type = MsgAdapter.ReadInt()
    info.draw_group_type = type
    info.mypoker = {}
    for i = 1, 5 do
        if type == POKER_REQ_OPER_TYPE.TYPE_MI_YI then
            info.mypoker[i] = MiYiPokerStruct.MiYiPoker()
        elseif type == POKER_REQ_OPER_TYPE.TYPE_WILD_BOSS then
            info.mypoker[i] = WildBossPokerStruct.WildBossPoker()
        else
            info.mypoker[i] = DrawPoker()
        end
    end
    self.info = info
    --LogError("奖励翻牌信息",self.info)
end

--===============================梦渊笔录玩法===============================
CSDreamNotesOp = CSDreamNotesOp or BaseClass(BaseProtocolStruct)
function CSDreamNotesOp:__init()
    self.msg_type = 3570
end

function CSDreamNotesOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.op_type)--0开始挑战 1扫荡 2继续扫荡
    MsgAdapter.WriteShort(self.param1) --目标关卡
end

SCDreamNotesInfo = SCDreamNotesInfo or BaseClass(BaseProtocolStruct)
function SCDreamNotesInfo:__init()
    self.msg_type = 3571
end

function SCDreamNotesInfo:Decode()
    local old_level = DreamNotesData.Instance:GetPassLevel()
    self.pass_level = MsgAdapter.ReadShort()--当前通关过的最高等级
    self.remain_times = MsgAdapter.ReadShort()--今天剩余次数
    self.buy_times = MsgAdapter.ReadShort()
    self.is_first_kill = old_level and old_level >= 0 and old_level ~= self.pass_level
    self.use_item_add_times = MsgAdapter.ReadShort()
    -- if self.is_first_kill == true then
    --     DreamNotesData.Instance:SetItemRemind(self.pass_level)
    --     UnityPlayerPrefs.SetInt(RoleData.Instance:GetRoleId() .. "DreamNotes",self.pass_level)
    -- end
end

SCDreamNotesSceneInfo = SCDreamNotesSceneInfo or BaseClass(BaseProtocolStruct)
function SCDreamNotesSceneInfo:__init()
    self.msg_type = 3572
end

function SCDreamNotesSceneInfo:Decode()
    self.level = MsgAdapter.ReadShort()--当前副本关卡
    self.kill_boss_flag = MsgAdapter.ReadChar()--0未击杀 1已击杀
    self.obj_list = {}
    self.talk_npc_id = 0
    for i=1,MsgAdapter.ReadChar() do
        local vo = {}
        vo.id = MsgAdapter.ReadInt()
        vo.scene_id = SceneData.Instance:SceneId()
        vo.is_need_talk = MsgAdapter.ReadChar()
        vo.obj_type = MsgAdapter.ReadChar()
        vo.obj_id = MsgAdapter.ReadUShort()
        vo.x = MsgAdapter.ReadShort()
        vo.y = MsgAdapter.ReadShort()
        self.obj_list[vo.id] = vo
        if self.talk_npc_id == 0 and vo.is_need_talk == 1 then
            self.talk_npc_id = vo.id
        end
    end
end

SCDreamNotesNotice = SCDreamNotesNotice or BaseClass(BaseProtocolStruct)
function SCDreamNotesNotice:__init()
    self.msg_type = 3573
end

function SCDreamNotesNotice:Decode()
    self.npc_id = MsgAdapter.ReadInt()
    self.is_auto_navigate = MsgAdapter.ReadInt()
end

-- 神兽降临奖池信息
SCRAPetBefallInfo = SCRAPetBefallInfo or BaseClass(BaseProtocolStruct)
function SCRAPetBefallInfo:__init()
    self.msg_type = 3580
end

function SCRAPetBefallInfo:Decode()
    self.info = {}
    self.info.pool_gold_num = MsgAdapter.ReadInt()
    self.info.pool_item_num = MsgAdapter.ReadInt()
end

-- 神兽降临转盘结果
SCRAPetBefallResult = SCRAPetBefallResult or BaseClass(BaseProtocolStruct)
function SCRAPetBefallResult:__init()
    self.msg_type = 3581
end

function SCRAPetBefallResult:Decode()
    self.info = {}
    self.info.seq = MsgAdapter.ReadInt()
    self.info.gold_num = MsgAdapter.ReadInt()
    local item_count = MsgAdapter.ReadInt()
    self.info.items = {}
    for i = 1,item_count do
        local item = {}
        item.item_id = MsgAdapter.ReadInt()
        item.num = MsgAdapter.ReadShort()
        item.is_bind = MsgAdapter.ReadShort()
        self.info.items[i] = item
    end
end
--------------
-- 校场演练信息
--------------
SCSchoolExerciseInfo = SCSchoolExerciseInfo or BaseClass(BaseProtocolStruct)
function SCSchoolExerciseInfo:__init()
    self.msg_type = 3590
end

function SCSchoolExerciseInfo:Decode()
    self.clear_flag =  MsgAdapter.IntFlag(2)
    self.first_flag = MsgAdapter.ReadInt()
end

CSSchoolExerciseChallenge = CSSchoolExerciseChallenge or BaseClass(BaseProtocolStruct)
function CSSchoolExerciseChallenge:__init()
    self.msg_type = 3591
end

function CSSchoolExerciseChallenge:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.seq)
end

-- 挑战成功通知
SCSchoolExerciseChallengeSucc = SCSchoolExerciseChallengeSucc or BaseClass(BaseProtocolStruct)
function SCSchoolExerciseChallengeSucc:__init()
    self.msg_type = 3592
end

function SCSchoolExerciseChallengeSucc:Decode(  )
    self.seq = MsgAdapter.ReadInt()
    self.has_reward = MsgAdapter.ReadInt()
end

-- 首次打开界面请求
CSSchoolExerciseFirstOpenUI = CSSchoolExerciseFirstOpenUI or BaseClass(BaseProtocolStruct)
function CSSchoolExerciseFirstOpenUI:__init()
    self.msg_type = 3593
end

function CSSchoolExerciseFirstOpenUI:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

--- 秘境奇闻

-- 神兽降临购买信息
SCPetBefallBuyInfo = SCPetBefallBuyInfo or BaseClass(BaseProtocolStruct)
function SCPetBefallBuyInfo:__init()
    self.msg_type = 3582
end

function SCPetBefallBuyInfo:Decode()
    self.info = {}
    self.info.day_buy_times = {}
    for i = 1,20 do
        self.info.day_buy_times[i] = MsgAdapter.ReadUShort()
    end
    self.info.act_buy_times = {}
    for i = 1,20 do
        self.info.act_buy_times[i] = MsgAdapter.ReadUShort()
    end
end

-- 伙伴飞升任务相关信息
SCNewTaskPartnerSoarInfo = SCNewTaskPartnerSoarInfo or BaseClass(BaseProtocolStruct)
function SCNewTaskPartnerSoarInfo:__init()
    self.msg_type = 3505
end

function SCNewTaskPartnerSoarInfo:Decode()
    self.is_cur_accpet_partner_soar_task = MsgAdapter.ReadChar()        -- 是否是当天接取的伙伴飞升任务 1:当天 !1:不是当天
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
end

--新等级助力请求
CSRoleNewLevelComplementReq = CSRoleNewLevelComplementReq or BaseClass(BaseProtocolStruct)
function CSRoleNewLevelComplementReq:__init()
    self.msg_type = 3566
end

function CSRoleNewLevelComplementReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    -- LogError("等级助力请求->................................." .. self.req_type)
end

--新等级助力角色信息下发
SCRoleNewLevelComplementInfo = SCRoleNewLevelComplementInfo or BaseClass(BaseProtocolStruct)
function SCRoleNewLevelComplementInfo:__init()
    self.msg_type = 3567
end

function SCRoleNewLevelComplementInfo:Decode()
    self.accumulated_exp = MsgAdapter.ReadLL()      --累计经验
    self.day_supplement_exp = MsgAdapter.ReadLL()   --当前凌晨补充的经验数值
    self.last_get_daily_exp_timestamp = MsgAdapter.ReadUInt()   --上次获得日常经验时间戳(不含今天)
    self.day_buy_exp_count = MsgAdapter.ReadShort() --当前购买经验次数
    self.level_difference = MsgAdapter.ReadShort()  --等级差值
    -- LOG(self, "等级助力角色信息下发-> ")
end

--新等级助力特殊信息下发
SCRoleNewLevelComplementSpecialInfo = SCRoleNewLevelComplementSpecialInfo or BaseClass(BaseProtocolStruct)
function SCRoleNewLevelComplementSpecialInfo:__init()
    self.msg_type = 3568
end

function SCRoleNewLevelComplementSpecialInfo:Decode()
    self.my_server_average_level = MsgAdapter.ReadInt() --前几名本服平均等级
    -- LogError("等级助力特殊信息下发-->" .. self.my_server_average_level)
end
