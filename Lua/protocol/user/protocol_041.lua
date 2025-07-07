--决斗战斗信息
SCDuelBattleInfo = SCDuelBattleInfo or BaseClass(BaseProtocolStruct)
function SCDuelBattleInfo:__init()
    self.msg_type = 4100
end

function SCDuelBattleInfo:Decode()
    self.duel_info = {}
    self.duel_info.duel_type = MsgAdapter.ReadInt()
    self.duel_info.att_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    self.duel_info.att_list = {}
    for i = 1, 5 do
        local role_id = MsgAdapter.ReadInt()
        if role_id > 0 then
            table.insert(self.duel_info.att_list, role_id)
        end
    end
    self.duel_info.def_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    self.duel_info.def_list = {}
    for i = 1, 5 do
        local role_id = MsgAdapter.ReadInt()
        if role_id > 0 then
            table.insert(self.duel_info.def_list, role_id)
        end
    end
end

--罪恶值改变通知周边玩家
SCRoleDuelKillValueAOINotice = SCRoleDuelKillValueAOINotice or BaseClass(BaseProtocolStruct)
function SCRoleDuelKillValueAOINotice:__init()
    self.msg_type = 4101
end

function SCRoleDuelKillValueAOINotice:Decode()
    self.obj_id = MsgAdapter.ReadUShort()
    self.reserve_sh = MsgAdapter.ReadShort()
    self.kill_value = MsgAdapter.ReadInt()
end

CSYinYangEyeOpReq = CSYinYangEyeOpReq or BaseClass(BaseProtocolStruct)
function CSYinYangEyeOpReq:__init()
    self.msg_type = 4110
end
function CSYinYangEyeOpReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.op_type)
    MsgAdapter.WriteShort(self.param)
end
SCYinYangEyeInfo = SCYinYangEyeInfo or BaseClass(BaseProtocolStruct)
function SCYinYangEyeInfo:__init()
    self.msg_type = 4111
end
function SCYinYangEyeInfo:Decode()
    self.open_mark = MsgAdapter.ReadChar()
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    self.cur_box_finish_seq = MsgAdapter.ReadShort()
    self.cur_npc_finish_seq = MsgAdapter.ReadShort()
    self.box_flag = MsgAdapter.IntFlag(16)
    self.npc_flag = MsgAdapter.IntFlag(16)
end

-- 怒魂技能请求
CSNuHunSikkReq= CSNuHunSikkReq or BaseClass(BaseProtocolStruct)
function CSNuHunSikkReq:__init()
    self.msg_type = 4140
end
function CSNuHunSikkReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param)
end

-- 怒魂技能信息
SCNuHunSkillInfo = SCNuHunSkillInfo or BaseClass(BaseProtocolStruct)
function SCNuHunSkillInfo:__init()
    self.msg_type = 4141
end
function SCNuHunSkillInfo:Decode()
    self.nu_hun_skill_level = {}
    for i = 0, 2 do
        self.nu_hun_skill_level[i] = MsgAdapter.ReadInt()
    end
end

CSMiniGameSetMiniGameId = CSMiniGameSetMiniGameId or BaseClass(BaseProtocolStruct)
function CSMiniGameSetMiniGameId:__init()
    self.msg_type = 4149
end

function CSMiniGameSetMiniGameId:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.mini_game_id)
end

-- 配饰操作请求
CSAccessoriesMentOperaReq = CSAccessoriesMentOperaReq or BaseClass(BaseProtocolStruct)
function CSAccessoriesMentOperaReq:__init()
    self.msg_type = 4150
end

function CSAccessoriesMentOperaReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param_1)
    MsgAdapter.WriteInt(self.param_2)
    MsgAdapter.WriteInt(self.param_3)
    MsgAdapter.WriteInt(self.param_4)
end

-- 全部配饰信息下发
SCAccessoriesMentInfo = SCAccessoriesMentInfo or BaseClass(BaseProtocolStruct)
function SCAccessoriesMentInfo:__init()
    self.msg_type = 4151
end

function SCAccessoriesMentInfo:Decode()
    self.capability = MsgAdapter.ReadInt()
    self.equipment_list = {}
    for i = 1, ACCRSSORY_MAX do
        self.equipment_list[i] = Accrssory.DecodeInfo()
    end
end

-- 单个配饰信息下发
SCAccessoriesMentChange = SCAccessoriesMentChange or BaseClass(BaseProtocolStruct)
function SCAccessoriesMentChange:__init()
    self.msg_type = 4152
end

function SCAccessoriesMentChange:Decode()
    self.capability = MsgAdapter.ReadInt()
    self.equip_info = Accrssory.DecodeInfo()
end

-- 配饰词条继承
CSAccessoriesMentVocComposeReq = CSAccessoriesMentVocComposeReq or BaseClass(BaseProtocolStruct)
function CSAccessoriesMentVocComposeReq:__init()
    self.msg_type = 4153
end

function CSAccessoriesMentVocComposeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.is_wear)
    MsgAdapter.WriteInt(self.index)
    MsgAdapter.WriteInt(self.replace_index)
    for i = 1, ACCRSSORY_ATTR_MAX do
        if self.replace_list[i] then
            MsgAdapter.WriteInt(self.replace_list[i].attr_index or -1)
            MsgAdapter.WriteInt(self.replace_list[i].replace_attr_index or -1)
        else
            MsgAdapter.WriteInt(-1)
            MsgAdapter.WriteInt(-1)
        end
    end
end

-- 仙饰设置
SCAccessoriesSpecialEquip = SCAccessoriesSpecialEquip or BaseClass(BaseProtocolStruct)
function SCAccessoriesSpecialEquip:__init()
    self.msg_type = 4154
end

function SCAccessoriesSpecialEquip:Decode()
    self.accessories_special_equip_list = {}
    for i = 1,2 do 
        self.accessories_special_equip_list[i] = MsgAdapter.ReadInt()
    end 
end

-- 职业晋阶的战斗请求
CSAdvanceCareerFightReq = CSAdvanceCareerFightReq or BaseClass(BaseProtocolStruct)
function CSAdvanceCareerFightReq:__init()
    self.msg_type = 4155
end

function CSAdvanceCareerFightReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

-- 职业晋阶子阶信息
SCAdvanceCareerSubInfo = SCAdvanceCareerSubInfo or BaseClass(BaseProtocolStruct)
function SCAdvanceCareerSubInfo:__init()
    self.msg_type = 4156
end

function SCAdvanceCareerSubInfo:Decode()
    self.sub_level = MsgAdapter.ReadInt()        --当前子阶段的阶数
    self.sub_fight_finish = MsgAdapter.ReadChar()--当前阶级战斗是否已经完成 [如果当前阶段是完成任
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
end

-- 万灵奇闻
SCSoulsAnecdoteFbObjInfo = SCSoulsAnecdoteFbObjInfo or BaseClass(BaseProtocolStruct)
function SCSoulsAnecdoteFbObjInfo:__init()
    self.msg_type = 4170
end

function SCSoulsAnecdoteFbObjInfo:Decode()
    local t = {}
    t.level_seq = MsgAdapter.ReadInt()
    t.layer = MsgAdapter.ReadInt()
    t.scene_id = MsgAdapter.ReadInt()

    t.box_num = MsgAdapter.ReadInt()
    t.m_box_num_max = MsgAdapter.ReadInt()

    t.monster_num = MsgAdapter.ReadInt()
    t.m_monster_num_max = MsgAdapter.ReadInt()

    t.npc_num = MsgAdapter.ReadInt()
    t.m_npc_num_max = MsgAdapter.ReadInt()

    t.gouhuo_num = MsgAdapter.ReadInt()
    t.m_gouhuo_num_max = MsgAdapter.ReadInt()

    t.coin_num = MsgAdapter.ReadInt()
    t.m_coin_num_max = MsgAdapter.ReadInt()

    t.gather_num = MsgAdapter.ReadInt()
    t.m_gather_num_max = MsgAdapter.ReadInt()
    self.fight_fantasy_info = t
    
    self.obj_count = MsgAdapter.ReadInt()
    self.obj_list = {}
    for i = 1,self.obj_count do 
        local vo = {}
        vo.obj_type = MsgAdapter.ReadInt()
        vo.npc_id = MsgAdapter.ReadInt()
        vo.pos_x = MsgAdapter.ReadShort()
        vo.pos_y = MsgAdapter.ReadShort()
        self.obj_list[i] = vo
    end 
end

-- 商店信息
SCSoulsAnecdoteShopInfo = SCSoulsAnecdoteShopInfo or BaseClass(BaseProtocolStruct)
function SCSoulsAnecdoteShopInfo:__init()
    self.msg_type = 4171
end

function SCSoulsAnecdoteShopInfo:Decode()
    self.shop_list = {}
    self.shop_num = MsgAdapter.ReadInt()
    for i = 1,self.shop_num do 
        local vo = {}
        vo.index = i
        vo.item_id = MsgAdapter.ReadUShort()
        vo.num = MsgAdapter.ReadShort()
        vo.is_bind = MsgAdapter.ReadInt()
        vo.price_type = MsgAdapter.ReadInt()
        vo.price = MsgAdapter.ReadInt()
        vo.buy_time = MsgAdapter.ReadInt()
        vo.m_buy_time = MsgAdapter.ReadInt()
        vo.buy_item_idx = MsgAdapter.ReadInt()
        vo.discount = MsgAdapter.ReadInt()
        table.insert(self.shop_list, vo)
    end 
end

-- 题目下发
SCSoulsAnecdoteQuestionInfo = SCSoulsAnecdoteQuestionInfo or BaseClass(BaseProtocolStruct)
function SCSoulsAnecdoteQuestionInfo:__init()
    self.msg_type = 4172
end

function SCSoulsAnecdoteQuestionInfo:Decode()
    self.question_id = MsgAdapter.ReadInt()
    self.is_true = MsgAdapter.ReadInt()
    self.answer_index = MsgAdapter.ReadInt()
end

-- 答题
CSSoulsAnecdoteQuestionReq = CSSoulsAnecdoteQuestionReq or BaseClass(BaseProtocolStruct)
function CSSoulsAnecdoteQuestionReq:__init()
    self.msg_type = 4173
end

function CSSoulsAnecdoteQuestionReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.answer_id)      
    MsgAdapter.WriteInt(self.npc_obj_id)
end

-- 购买
CSSoulsAnecdoteBuyItemReq = CSSoulsAnecdoteBuyItemReq or  BaseClass(BaseProtocolStruct)
function CSSoulsAnecdoteBuyItemReq:__init()
    self.msg_type = 4174
end

function CSSoulsAnecdoteBuyItemReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.item_idx)      
    MsgAdapter.WriteInt(self.npc_obj_id)
end

--无尽之塔
SCEndlessTowerInfo = SCEndlessTowerInfo or BaseClass(BaseProtocolStruct)
function SCEndlessTowerInfo:__init()
    self.msg_type = 4120
end

function SCEndlessTowerInfo:Decode()
    self.battle_seq = MsgAdapter.ReadInt() --当前通关seq
    self.pass_level = self.battle_seq + 1
end

CSEndlessTowerReq = CSEndlessTowerReq or  BaseClass(BaseProtocolStruct)
function CSEndlessTowerReq:__init()
    self.msg_type = 4121
end

function CSEndlessTowerReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)      
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

-- 山海聚宝
SCRAShanHaiJuBaoInfo = SCRAShanHaiJuBaoInfo or BaseClass(BaseProtocolStruct)
function SCRAShanHaiJuBaoInfo:__init()
    self.msg_type = 4177
end

function SCRAShanHaiJuBaoInfo:Decode()
    self.phase_info = {}
    for i =1,3 do 
        local vo = {}
        vo.total_draw_times = MsgAdapter.ReadInt()
        vo.extra_reward_flag = MsgAdapter.ReadInt()
        table.insert(self.phase_info,vo)
    end 
end

SCRAShanHaiJuBaoDrawRet = SCRAShanHaiJuBaoDrawRet or BaseClass(BaseProtocolStruct)
function SCRAShanHaiJuBaoDrawRet:__init()
    self.msg_type = 4178
end

function SCRAShanHaiJuBaoDrawRet:Decode()
    self.show_type = MsgAdapter.ReadInt()
end

SCRAShanHaiMetBase = SCRAShanHaiMetBase or BaseClass(BaseProtocolStruct)
function SCRAShanHaiMetBase:__init()
    self.msg_type = 4185
end
function SCRAShanHaiMetBase:Decode()
    self.extra_flag = MsgAdapter.ReadInt() --额外奖励激活标记
    self.day_task_finish_flag = MsgAdapter.ReadInt()  --任务完成标记(只在领取和跨天时校验)
    self.fetch_flag = MsgAdapter.ReadInt()   --奖励发放标记[活动天数index]
    self.fetch_extra_flag = MsgAdapter.ReadInt() --额外奖励发放标记[活动天数index]
end

SCRAShanHaiMetTakeParam = SCRAShanHaiMetTakeParam or BaseClass(BaseProtocolStruct)
function SCRAShanHaiMetTakeParam:__init()
    self.msg_type = 4186
end
function SCRAShanHaiMetTakeParam:Decode()
    self.count = MsgAdapter.ReadInt()
    self.task_list = {}
    for i=1,self.count do
        local tb = {}
        tb.index = i
        tb.times = MsgAdapter.ReadInt()
        table.insert(self.task_list,tb)
    end 
end

SCRAShanHaiMetTaskParamChange = SCRAShanHaiMetTaskParamChange or BaseClass(BaseProtocolStruct)
function SCRAShanHaiMetTaskParamChange:__init()
    self.msg_type = 4187
end
function SCRAShanHaiMetTaskParamChange:Decode()
    self.task_type = MsgAdapter.ReadInt()
    self.times = MsgAdapter.ReadInt()
end

local function ReadFriendIntimacyInfo()
    local t = {
        uid = MsgAdapter.ReadInt(),                                 -- 好友uid
        intimacy_val = MsgAdapter.ReadInt(),                        -- 亲密度
        day_first_to_singchat_timestamp = MsgAdapter.ReadInt(),     -- 每日玩家给好友发送的第一条消息的时间戳
        day_first_from_singchat_timestamp = MsgAdapter.ReadInt(),   -- 每日好友给玩家发送的第一条消息的时间戳
    }
    return t
end

--好友亲密度信息列表
SCFriendIntimacyListInfo = SCFriendIntimacyListInfo or BaseClass(BaseProtocolStruct)
function SCFriendIntimacyListInfo:__init()
    self.msg_type = 4180
end

function SCFriendIntimacyListInfo:Decode()
    self.count = MsgAdapter.ReadInt()   --表长
    local t = {}
    for i = 1, self.count do
        local info = ReadFriendIntimacyInfo()
        t[info.uid] = info
    end
    self.friend_intimacy_list = t
end

--好友亲密度信息
SCFriendIntimacyInfoChange = SCFriendIntimacyInfoChange or BaseClass(BaseProtocolStruct)
function SCFriendIntimacyInfoChange:__init()
    self.msg_type = 4181
end

function SCFriendIntimacyInfoChange:Decode()
    self.friend_intimacy_info = ReadFriendIntimacyInfo()
end

-- 再战云泽
SCRACloudSwampResult = SCRACloudSwampResult or BaseClass(BaseProtocolStruct)
function SCRACloudSwampResult:__init()
    self.msg_type = 4190
end

function SCRACloudSwampResult:Decode()
    self.boss_info = {}
    local count = MsgAdapter.ReadInt()
    for i=0, count - 1 do
        self.boss_info[i] = MsgAdapter.ReadInt()
    end
end

----纷享加倍
SCRAEnjoyRedoubleBase = SCRAEnjoyRedoubleBase or BaseClass(BaseProtocolStruct)
function SCRAEnjoyRedoubleBase:__init()
    self.msg_type = 4195
end

function SCRAEnjoyRedoubleBase:Decode()
    self.today_free_flag = MsgAdapter.ReadInt()
    self.activation_flag = MsgAdapter.ReadInt()
    self.fetch_flag = MsgAdapter.ReadInt()
    self.task_finish_num = MsgAdapter.ReadInt() --总任务完成次数
end

SCRAEnjoyRedoubleTakeParam = SCRAEnjoyRedoubleTakeParam or BaseClass(BaseProtocolStruct)
function SCRAEnjoyRedoubleTakeParam:__init()
    self.msg_type = 4196
end

function SCRAEnjoyRedoubleTakeParam:Decode()
    self.count = MsgAdapter.ReadInt()
    self.task_list = {}   
    for i = 1, self.count do
        self.task_list[i] = MsgAdapter.ReadInt()
    end
end

SCRAEnjoyRedoubleTaskParamChange = SCRAEnjoyRedoubleTaskParamChange or BaseClass(BaseProtocolStruct)
function SCRAEnjoyRedoubleTaskParamChange:__init()
    self.msg_type = 4197
end

function SCRAEnjoyRedoubleTaskParamChange:Decode()
    self.task_type = MsgAdapter.ReadInt()
    self.task_times = MsgAdapter.ReadInt()
end

--琉璃蜃境
CSMirageTerritorySetPartnerReq = CSMirageTerritorySetPartnerReq or BaseClass(BaseProtocolStruct)
function CSMirageTerritorySetPartnerReq:__init()
    self.msg_type = 4134
end

function CSMirageTerritorySetPartnerReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.programme_id)
    for i = 1 , 3 do
        MsgAdapter.WriteInt(self.id_list[i])
    end
    for i = 1 , 3 do
        MsgAdapter.WriteInt(self.pos_list[i])
    end
end

SCMirageTerritorySetPartnerInfo = SCMirageTerritorySetPartnerInfo or BaseClass(BaseProtocolStruct)
function SCMirageTerritorySetPartnerInfo:__init()
    self.msg_type = 4135
end

function SCMirageTerritorySetPartnerInfo:Decode()
    self.programme_id = MsgAdapter.ReadInt()
    self.id_list = {}
    self.pos_list = {}
    for i = 1 , 3 do
        self.id_list[i] = MsgAdapter.ReadInt()
    end
    for i = 1 , 3 do
        self.pos_list[i] = MsgAdapter.ReadInt()
    end
end

CSMirageTerritoryReq = CSMirageTerritoryReq or BaseClass(BaseProtocolStruct)
function CSMirageTerritoryReq:__init()
    self.msg_type = 4132
end

function CSMirageTerritoryReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)      
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
    MsgAdapter.WriteInt(self.param4)
end

SCMirageTerritoryInfo = SCMirageTerritoryInfo or BaseClass(BaseProtocolStruct)
function SCMirageTerritoryInfo:__init()
    self.msg_type = 4133
end

function SCMirageTerritoryInfo:Decode()
    self.times = MsgAdapter.ReadInt() --次数
    self.pass_area = MsgAdapter.ReadInt() --今天派遣分区
    self.pass_seq = MsgAdapter.ReadInt() --今天派遣关卡
    self.area = MsgAdapter.ReadInt() --分区
    self.top_seq = MsgAdapter.ReadInt() -- 最高通关记录
    self.now_seq = MsgAdapter.ReadInt() -- 当前通关记录
end

-------------------------------------------
----------飞升系统
-------------------------------------------
--飞升请求
CSFlyUpSystemReq = CSFlyUpSystemReq or  BaseClass(BaseProtocolStruct)
function CSFlyUpSystemReq:__init()
    self.msg_type = 4131
end

function CSFlyUpSystemReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)      
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

--飞升系统
SCFlyUpSystemInfo = SCFlyUpSystemInfo or BaseClass(BaseProtocolStruct)
function SCFlyUpSystemInfo:__init()
    self.msg_type = 4130
end

function SCFlyUpSystemInfo:Decode()
    self.task_index = MsgAdapter.ReadInt() --第几环任务/今天完成次数？
    self.skill_level = MsgAdapter.ReadInt()--飞升技能等级
    self.stage = MsgAdapter.ReadInt()      --飞升阶段 0无1低2高
    self.stage = self.stage > FlyUpStageType.High and FlyUpStageType.High or self.stage
    self.is_open = MsgAdapter.ReadInt()
    self.element = {}
    for i=0,3 do
        self.element[i] = MsgAdapter.ReadInt()
    end
    self.organ = {}
    for i=0,4 do
        self.organ[i] = MsgAdapter.ReadInt()
    end
    -- LogError("飞升系统",self)
    self.is_one_click_finish = MsgAdapter.ReadInt()--是否是一件完成过来的
end

--飞升强化
SCFlyUpSystemAdvInfo = SCFlyUpSystemAdvInfo or BaseClass(BaseProtocolStruct)
function SCFlyUpSystemAdvInfo:__init()
    self.msg_type = 4136
end

function SCFlyUpSystemAdvInfo:Decode()
    self.stage = MsgAdapter.ReadInt()
    self.index = MsgAdapter.ReadInt()
    self.value = MsgAdapter.ReadInt()
    -- LogError("飞升强化",self)
end

--飞升技能
SCFlyUpSkillSigleInfo = SCFlyUpSkillSigleInfo or BaseClass(BaseProtocolStruct)
function SCFlyUpSkillSigleInfo:__init()
    self.msg_type = 4137
end

function SCFlyUpSkillSigleInfo:Decode()
    self.skill_level = MsgAdapter.ReadInt()--飞升技能等级
    self.skill_exp = MsgAdapter.ReadInt()--飞升当前经验
    -- LogError("飞升技能",self)
end

-- 小游戏信息
SCMiniGameInfo = SCMiniGameInfo or BaseClass(BaseProtocolStruct)
function SCMiniGameInfo:__init()
    self.msg_type = 4145
end

function SCMiniGameInfo:Decode()
    self.finish_stage_num = MsgAdapter.ReadInt()
    self.phase_reward_flag = MsgAdapter.IntFlag()
end

-- 玩家完成下一关小游戏请求
CSMiniGameFinishNextStage = CSMiniGameFinishNextStage or  BaseClass(BaseProtocolStruct)
function CSMiniGameFinishNextStage:__init()
    self.msg_type = 4146
end

function CSMiniGameFinishNextStage:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

-- 小游戏阶段奖励领取
CSMiniGameClaimPhaseReward = CSMiniGameClaimPhaseReward or  BaseClass(BaseProtocolStruct)
function CSMiniGameClaimPhaseReward:__init()
    self.msg_type = 4147
end

function CSMiniGameClaimPhaseReward:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.seq)
end

-- 小游戏进入战斗
CSMiniGameStartBattle = CSMiniGameStartBattle or  BaseClass(BaseProtocolStruct)
function CSMiniGameStartBattle:__init()
    self.msg_type = 4148
end

function CSMiniGameStartBattle:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.seq)
    MsgAdapter.WriteChar(self.has_pet)
    MsgAdapter.WriteChar(self.has_partner)
    MsgAdapter.WriteChar(self.has_skill)
    MsgAdapter.WriteChar(0)
end