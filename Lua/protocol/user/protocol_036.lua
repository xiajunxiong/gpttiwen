CSWorkshopDecomposeReq = CSWorkshopDecomposeReq or BaseClass(BaseProtocolStruct)


function CSWorkshopDecomposeReq:__init()
    self.msg_type = 3600
end

function CSWorkshopDecomposeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.grid_num)
    for i = 1, 20 do
        MsgAdapter.WriteInt(self.decompose_grid[i] or 0)
    end
end

local top_info = function ()
    local res = {}
    res.guild_id = MsgAdapter.ReadInt()
    res.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
    return res
end

-- 灵骑系统 灵技一键合成请求
CSSmartMountsSystemKeyComposeReq = CSSmartMountsSystemKeyComposeReq or BaseClass(BaseProtocolStruct)
function CSSmartMountsSystemKeyComposeReq:__init()
    self.msg_type = 3601
end

function CSSmartMountsSystemKeyComposeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.count)
    MsgAdapter.WriteShort(self.column)
    for i = 1,10 do
        MsgAdapter.WriteUInt(self.index_flag[i] or 0)
    end 
end

CSMonsterWaveReq = CSMonsterWaveReq or BaseClass(BaseProtocolStruct)
function CSMonsterWaveReq:__init()
    self.msg_type = 3602
end

function CSMonsterWaveReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.req_type)
    MsgAdapter.WriteShort(self.param1)
end

SCMonsterWaveAchievementInfo = SCMonsterWaveAchievementInfo or BaseClass(BaseProtocolStruct)
function SCMonsterWaveAchievementInfo:__init()
    self.msg_type = 3603
end

function SCMonsterWaveAchievementInfo:Decode()
    self.pass_wave_cond_times = MsgAdapter.ReadInt()
    self.achievement_fecth_flag = MsgAdapter.IntFlag()
end

SCWorldGuildGuildTopInfo = SCWorldGuildGuildTopInfo or BaseClass(BaseProtocolStruct)
function SCWorldGuildGuildTopInfo:__init()
    self.msg_type = 3650
end
function SCWorldGuildGuildTopInfo:Decode()
    self.top_info = {}
    local temp
    for i = 1, 3 do
        temp = top_info()
        temp.i = i
        self.top_info[i] = temp
        -- self.top_info[temp.guild_id] = temp
    end
end

-- 春日登峰下发
SCRAChunRiDengFengInfo = SCRAChunRiDengFengInfo or BaseClass(BaseProtocolStruct)
function SCRAChunRiDengFengInfo:__init()
    self.msg_type = 3611
end

function SCRAChunRiDengFengInfo:Decode()
    self.info = {}
    self.info.layer = MsgAdapter.ReadUInt() --当前层数
    self.info.explore_total_times = MsgAdapter.ReadUInt() --探索总次数
    self.info.explore_times = MsgAdapter.ReadUShort()     --没达到顶层时的探索次数，达到顶层会清零
    self.info.get_item_flag = MsgAdapter.ReadUShort()     --顶层奖励道具获取标志位
    self.get_item_flags = DataHelper.ReverseList(bit:d16b(self.info.get_item_flag))
end

--组合贺礼
SCRAZuHeHeLiInfo = SCRAZuHeHeLiInfo or BaseClass(BaseProtocolStruct)
function SCRAZuHeHeLiInfo:__init()
    self.msg_type = 3630
end

function SCRAZuHeHeLiInfo:Decode()
    local RA_ZU_HE_HE_LI_MAX_ITEM = 60
    self.ra_day_buy_times = {} --每种物品每日限购次数
    for i = 0, RA_ZU_HE_HE_LI_MAX_ITEM - 1 do
        self.ra_day_buy_times[i] = MsgAdapter.ReadUShort()
    end
    self.ra_buy_times = {} --每种物品限购次数
    for i = 0, RA_ZU_HE_HE_LI_MAX_ITEM - 1 do
        self.ra_buy_times[i] = MsgAdapter.ReadUShort()
    end
end

-- 秘境奇闻
-- 题目下发
SCRAMiJingQiWenQuestionInfo = SCRAMiJingQiWenQuestionInfo or BaseClass(BaseProtocolStruct)
function SCRAMiJingQiWenQuestionInfo:__init()
    self.msg_type = 3614
end

function SCRAMiJingQiWenQuestionInfo:Decode()
    self.question_id = MsgAdapter.ReadInt()
    self.is_true = MsgAdapter.ReadInt()
    self.answer_index = MsgAdapter.ReadInt()
end

-- 答题
CSRAMiJingQiWenQuestionReq = CSRAMiJingQiWenQuestionReq or BaseClass(BaseProtocolStruct)
function CSRAMiJingQiWenQuestionReq:__init()
    self.msg_type = 3615
end

function CSRAMiJingQiWenQuestionReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.answer_id)      
    MsgAdapter.WriteInt(self.npc_obj_id)
end

-- 玩家购买道具次数
SCRAMiJingQiWenShopBuyTimesInfo = SCRAMiJingQiWenShopBuyTimesInfo or BaseClass(BaseProtocolStruct)
function SCRAMiJingQiWenShopBuyTimesInfo:__init()
    self.msg_type = 3613
end

function SCRAMiJingQiWenShopBuyTimesInfo:Decode()
    self.buy_times = {}
    for i = 1,15 do 
        self.buy_times[i] = MsgAdapter.ReadInt() 
    end 
end

-- 购买
CSRAMiJingQiWenBuyItemReq = CSRAMiJingQiWenBuyItemReq or  BaseClass(BaseProtocolStruct)
function CSRAMiJingQiWenBuyItemReq:__init()
    self.msg_type = 3619
end

function CSRAMiJingQiWenBuyItemReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.item_idx)      
    MsgAdapter.WriteInt(self.npc_obj_id)
end

-- 商店信息
SCRAMiJingQiWenShopInfo = SCRAMiJingQiWenShopInfo or BaseClass(BaseProtocolStruct)
function SCRAMiJingQiWenShopInfo:__init()
    self.msg_type = 3612
end

function SCRAMiJingQiWenShopInfo:Decode()
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
        table.insert( self.shop_list, vo )
    end 
end

SCCloseWindosRet = SCCloseWindosRet or BaseClass(BaseProtocolStruct)
function SCCloseWindosRet:__init()
    self.msg_type = 3620
end

function SCCloseWindosRet:Decode()
    self.reason = MsgAdapter.ReadInt()
end

CSCloseWindosReq = CSCloseWindosReq or BaseClass(BaseProtocolStruct)
function CSCloseWindosReq:__init()
    self.msg_type = 3621
end

function CSCloseWindosReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.reason)      
end

SCRelicLootingTampInfo = SCRelicLootingTampInfo or BaseClass(BaseProtocolStruct)
function SCRelicLootingTampInfo:__init()
    self.msg_type = 3622
end

function SCRelicLootingTampInfo:Decode()
    self.reason_type = MsgAdapter.ReadInt() -- 0 等待加载 1 准备开始 2 游戏进行
    self.tamp =  MsgAdapter.ReadUInt() -- 状态结束戳
end

function RelicLootingFBSkillInfo()
    local t = {}
    t.sn = MsgAdapter.ReadInt() -- 技能id
    t.special_param = MsgAdapter.ReadInt() -- 技能特殊参数
    t.cool_down_tamp = MsgAdapter.ReadUInt() -- 冷却戳
    t.guide_over_tamp = MsgAdapter.ReadUInt() -- 引导戳
    t.casting_tamp = MsgAdapter.ReadUInt() -- 施法戳
    return t
end

SCRelicLootingFBSkillInfo = SCRelicLootingFBSkillInfo or BaseClass(BaseProtocolStruct)
function SCRelicLootingFBSkillInfo:__init()
    self.msg_type = 3623
end

function SCRelicLootingFBSkillInfo:Decode()
    self.stop_reason = MsgAdapter.ReadInt()
    self.common_skill_num = MsgAdapter.ReadInt()
    self.m_common_skill = {}
    for i = 1,3 do 
        local vo = RelicLootingFBSkillInfo()
        vo.skill_index = i
        table.insert(self.m_common_skill,vo)
    end 
    self.m_item_skill = RelicLootingFBSkillInfo()
end

CSRelicLootingMatchReq = CSRelicLootingMatchReq or BaseClass(BaseProtocolStruct)
function CSRelicLootingMatchReq:__init()
    self.msg_type = 3624
end

function CSRelicLootingMatchReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.is_match)
    MsgAdapter.WriteInt(self.param)
end 

SCRelicLootingTeamInfoReq = SCRelicLootingTeamInfoReq or BaseClass(BaseProtocolStruct)
function SCRelicLootingTeamInfoReq:__init()
    self.msg_type = 3625
end

function SCRelicLootingTeamInfoReq:Decode()
    self.defend_team_list = {}
    for i = 1,3 do 
        local vo = {}
        vo.team_type = MsgAdapter.ReadInt()
        vo.uid = MsgAdapter.ReadInt()
        vo.gongxian = MsgAdapter.ReadInt()
        vo.stop_reason = MsgAdapter.ReadInt()
        vo.on_line = MsgAdapter.ReadInt()
        vo.play_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        table.insert(self.defend_team_list,vo)
    end 
    self.attack_team_list = {}
    for i = 1,3 do 
        local vo = {}
        vo.team_type = MsgAdapter.ReadInt()
        vo.uid = MsgAdapter.ReadInt()
        vo.gongxian = MsgAdapter.ReadInt()
        vo.stop_reason = MsgAdapter.ReadInt()
        vo.on_line = MsgAdapter.ReadInt()
        vo.play_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        table.insert(self.attack_team_list,vo)
    end 
end

CSRelicLootingUseSkillReq = CSRelicLootingUseSkillReq or BaseClass(BaseProtocolStruct)
function CSRelicLootingUseSkillReq:__init()
    self.msg_type = 3626
end

function CSRelicLootingUseSkillReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
    MsgAdapter.WriteInt(self.param4)
end 

SCRelicLootingObjInfo = SCRelicLootingObjInfo or BaseClass(BaseProtocolStruct)
function SCRelicLootingObjInfo:__init() 
    self.msg_type = 3627
end

function SCRelicLootingObjInfo:Decode()
    self.obj_count = MsgAdapter.ReadInt()
    self.obj_list = {}
    for i = 1,self.obj_count do 
        local vo = {}
        vo.obj_type = MsgAdapter.ReadInt() -- 0 宝箱 1 技能 2 陷阱 3 傀儡娃娃 4 稻草人
        vo.status = MsgAdapter.ReadInt()
        vo.param1 = MsgAdapter.ReadInt()
        vo.param2 = MsgAdapter.ReadInt()
        vo.pos_x = MsgAdapter.ReadInt()
        vo.pos_y = MsgAdapter.ReadInt()
        table.insert(self.obj_list,vo)
    end 
end

SCRelicootingMemberPosInfo = SCRelicootingMemberPosInfo or BaseClass(BaseProtocolStruct)
function SCRelicootingMemberPosInfo:__init() 
    self.msg_type = 3628
end

function SCRelicootingMemberPosInfo:Decode()
    self.member_num = MsgAdapter.ReadInt()
    self.member_list = {}
    for i = 1,self.member_num do 
        local vo = {}
        vo.uid = MsgAdapter.ReadInt()
        vo.pos_x = MsgAdapter.ReadInt()
        vo.pos_y = MsgAdapter.ReadInt()
        table.insert( self.member_list , vo)
    end 
end

SCRelicootingMemberTransmitInfo = SCRelicootingMemberTransmitInfo or BaseClass(BaseProtocolStruct)
function SCRelicootingMemberTransmitInfo:__init()
    self.msg_type = 3629
end

function SCRelicootingMemberTransmitInfo:Decode()
    self.uid = MsgAdapter.ReadInt()
    self.end_tamp = MsgAdapter.ReadUInt()
    self.player_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
end

SCRelicootingMarkInfo = SCRelicootingMarkInfo or BaseClass(BaseProtocolStruct)
function SCRelicootingMarkInfo:__init() 
    self.msg_type = 3631     
end

function SCRelicootingMarkInfo:Decode()
    self.marker_uid = MsgAdapter.ReadInt()
    self.mark_type = MsgAdapter.ReadInt()
    self.mark_pos_x = MsgAdapter.ReadInt()
    self.mark_pos_y = MsgAdapter.ReadInt()
end

SCRelicootingFBFinishInfo = SCRelicootingFBFinishInfo or BaseClass(BaseProtocolStruct)
function SCRelicootingFBFinishInfo:__init() 
    self.msg_type = 3632     
end

function SCRelicootingFBFinishInfo:Decode()
    self.win_type = MsgAdapter.ReadInt() -- 0 平局 1 盗宝胜 2 守军胜
    self.defend_team = {}
    for i = 1,3 do 
        local vo = {}
        vo.team_type = MsgAdapter.ReadInt()
        vo.uid = MsgAdapter.ReadInt()
        vo.gongxian = MsgAdapter.ReadInt()
        vo.add_score = MsgAdapter.ReadInt()
        vo.on_line = MsgAdapter.ReadInt()
        vo.play_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        table.insert( self.defend_team ,vo)
    end 
    self.attack_team = {}
    for i = 1,3 do 
        local vo = {}
        vo.team_type = MsgAdapter.ReadInt()
        vo.uid = MsgAdapter.ReadInt()
        vo.gongxian = MsgAdapter.ReadInt()
        vo.add_score = MsgAdapter.ReadInt()
        vo.on_line = MsgAdapter.ReadInt()
        vo.play_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        table.insert( self.attack_team ,vo)
    end 
end

SCRelicootingRankInfo = SCRelicootingRankInfo or BaseClass(BaseProtocolStruct)
function SCRelicootingRankInfo:__init() 
    self.msg_type = 3633 
end

function SCRelicootingRankInfo:Decode()
    self.my_pos = MsgAdapter.ReadInt() 
    self.rank_num = MsgAdapter.ReadInt()
    self.rank_list = {}
    for i = 1,self.rank_num do 
        local vo = {}
        vo.index = i
        vo.uid = MsgAdapter.ReadInt()
        vo.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        vo.rank_value = MsgAdapter.ReadLL()
        vo.flexible_int = MsgAdapter.ReadInt()
        vo.flexible_ll = MsgAdapter.ReadLL()
        table.insert(self.rank_list,vo)
    end 
end

SCRelicLootingAchievementInfo = SCRelicLootingAchievementInfo or BaseClass(BaseProtocolStruct)
function SCRelicLootingAchievementInfo:__init()
    self.msg_type = 3634
end

function SCRelicLootingAchievementInfo:Decode()
    self.param = {}
    for i = 0,3 do 
        self.param[i] = MsgAdapter.ReadInt() -- 0  当前胜利数 1 当前连胜数 2 最高连胜次数 3 战败次数
    end 
    self.reward_status = {}
    for i = 0,3 do 
        self.reward_status[i] = MsgAdapter.ReadInt()  -- 0-1是否领取
    end 
    self.show_flag = MsgAdapter.ReadInt()
end

SCRelicLootingEffectInfo = SCRelicLootingEffectInfo or BaseClass(BaseProtocolStruct) 
function SCRelicLootingEffectInfo:__init()
    self.msg_type = 3635
end

function SCRelicLootingEffectInfo:Decode()
    self.uid =  MsgAdapter.ReadInt()
    self.special_effect_type =  MsgAdapter.ReadInt()
end

SCPetMonopolyInfo = SCPetMonopolyInfo or BaseClass(BaseProtocolStruct)
function SCPetMonopolyInfo:__init()
    self.msg_type = 3660
end
function SCPetMonopolyInfo:Decode()
    self.expect_silver_pet_id = MsgAdapter.ReadInt()
    self.expect_gold_pet_id = MsgAdapter.ReadInt()

    self.cur_level = MsgAdapter.ReadShort()
    self.cur_grid_index = MsgAdapter.ReadShort() --[0,36)

    self.grid_list = {}
    for i=1, PET_MONOPOLY_GRID_NUM do
        local t = {}
        t.reward_type = MsgAdapter.ReadShort()
        t.grid_type = MsgAdapter.ReadShort()
        t.param1 = MsgAdapter.ReadInt()             --格子是宠物时为pet_id，物品或碎片时为物品或碎片id
        t.param2 = MsgAdapter.ReadInt()             --格子是物品或碎片时表示数量
        t.grid_index = i - 1
        table.insert(self.grid_list, t)
    end
end
--灵宠秘境操作请求
-- OPER_TYPE
-- {
--     OPER_TYPE_INFO = 0,             //!< 请求棋盘信息
--     OPER_TYPE_REWARD_RECORD = 1,    //!< 查看奖励
--     OPER_TYPE_SET_EXPECT_PET = 2,   //!< 设置祈福宠物 param1 宠物ID
--     OPER_TYPE_CLEAR_EXPECT_PET = 3, //!< 清除祈福宠物 param1 是否金宠(1:是，0:否）
--     OPER_TYPE_ROLL_DICE = 4,        //!< 摇骰子
--     OPER_TYPE_FETCH_GRID_REWARD = 5,//!< 获取当前格子的奖励
--     OPER_TYPE_START_PET_BATTLE = 6, //!< 开始抓宠战斗
--     OPER_TYPE_TURN_TABLE = 7,       //!< 开始转盘
--     OPER_TYPE_ENTER_NEXT_LEVEL = 8, //!< 进入下一层
--     OPER_TYPE_SHOW_TRUNTABLE_REWARDS = 9, --//!< 查看转盘奖励
--     OPER_TYPE_TURN_TABLE_END = 10, --//!< 转盘动画结束
--     OPER_TYPE_ADD_EXP = 11,         --//!< 加经验
--     OPER_TYPE_SPECIAL_ITEM = 12,     --//!< 使用特殊道具 param1:道具类型 param2:使用道具附加（鲁班骰点数/心愿卡选择的物品id）
--     OPER_TYPE_SPECIAL_ITEM_NUM = 13, --//!< 获取全部特殊道具数量
--     OPER_TYPE_FAST_MOVE_REWARD = 14, --//!< 使用神行符申请奖励
-- };
CSPetMonopolyOperate = CSPetMonopolyOperate or BaseClass(BaseProtocolStruct)
function CSPetMonopolyOperate:__init()
    self.msg_type = 3661
    self.reserve_sh = 0
end

function CSPetMonopolyOperate:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.operate_type)
    MsgAdapter.WriteShort(self.reserve_sh)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

--灵宠秘境祈福返回
SCPetMonopolyExpectPetChangeNotice = SCPetMonopolyExpectPetChangeNotice or BaseClass(BaseProtocolStruct)
function SCPetMonopolyExpectPetChangeNotice:__init()
    self.msg_type = 3662
end

function SCPetMonopolyExpectPetChangeNotice:Decode()
    self.expect_silver_pet_id = MsgAdapter.ReadInt()
    self.expect_gold_pet_id = MsgAdapter.ReadInt()
end

--灵宠秘境投骰子结果返回
SCPetMonopolyRollDice = SCPetMonopolyRollDice or BaseClass(BaseProtocolStruct)
function SCPetMonopolyRollDice:__init()
    self.msg_type = 3663
end

function SCPetMonopolyRollDice:Decode()
    self.dice = MsgAdapter.ReadShort() --[1,6] 小于0时对应道具类型的负数
    self.cur_grid_index = MsgAdapter.ReadShort() --新的格子位置
end

--当前格点内容改变通知
SCPetMonopolyCurGridChangeNotice = SCPetMonopolyCurGridChangeNotice or BaseClass(BaseProtocolStruct)
function SCPetMonopolyCurGridChangeNotice:__init()
    self.msg_type = 3664
end

function SCPetMonopolyCurGridChangeNotice:Decode()
    self.cur_grid_index = MsgAdapter.ReadInt()
    self.grid_info = {}
    self.grid_info.reward_type = MsgAdapter.ReadShort()
    self.grid_info.grid_type = MsgAdapter.ReadShort()
    self.grid_info.param1 = MsgAdapter.ReadInt()             --格子是宠物时为pet_id，物品或碎片时为物品或碎片id
    self.grid_info.param2 = MsgAdapter.ReadInt()             --格子是物品或碎片时表示数量
    self.grid_info.grid_index = self.cur_grid_index
end

--灵宠秘境查看奖励返回
SCPetMonopolyRewardRecord = SCPetMonopolyRewardRecord or BaseClass(BaseProtocolStruct)
function SCPetMonopolyRewardRecord:__init()
    self.msg_type = 3665
end

function SCPetMonopolyRewardRecord:Decode()
    self.reward_num = MsgAdapter.ReadInt()
    self.reward_list = {}
    for i=1,self.reward_num do
        local t = {}
        t.item_id = MsgAdapter.ReadInt()
        t.num = MsgAdapter.ReadInt()
        t.is_bind = MsgAdapter.ReadShort()
        t.param_length = MsgAdapter.ReadShort()
        if t.param_length > 0 then
            t.param = ItemDataParam(t.item_id, t.param_length)
        else
            t.param = {}
        end
        table.insert(self.reward_list, t)
    end
end

--进入下一层通知 （随后会发3660）
SCPetMonopolyEnterNextLevelNotice = SCPetMonopolyEnterNextLevelNotice or BaseClass(BaseProtocolStruct)
function SCPetMonopolyEnterNextLevelNotice:__init()
    self.msg_type = 3666
end

function SCPetMonopolyEnterNextLevelNotice:Decode()
    self.next_level = MsgAdapter.ReadInt()
end

--转盘信息
SCPetMonopolyTurntable = SCPetMonopolyTurntable or BaseClass(BaseProtocolStruct)
function SCPetMonopolyTurntable:__init()
    self.msg_type = 3667
end

function SCPetMonopolyTurntable:Decode()
    self.sector_list = {}
    for i=1, PET_MONOPOLY_TURNTABLE_SECTOR_NUM do
        local t = {}
        t.reward_type = MsgAdapter.ReadShort()
        MsgAdapter.ReadShort()
        t.param1 = MsgAdapter.ReadInt()
        t.param2 = MsgAdapter.ReadInt()
        table.insert(self.sector_list, t)
    end
end
--转盘结果下发
SCPetMonopolyTurntableResult = SCPetMonopolyTurntableResult or BaseClass(BaseProtocolStruct)
function SCPetMonopolyTurntableResult:__init()
    self.msg_type = 3668
end

function SCPetMonopolyTurntableResult:Decode()
    self.sector_idx = MsgAdapter.ReadInt()  --最终指向山区序号[0,8)
end

--双倍/道具数量信息下发 道具使用结果下发
SCPetMonopolySpecialItem = SCPetMonopolySpecialItem or BaseClass(BaseProtocolStruct)
function SCPetMonopolySpecialItem:__init()
    self.msg_type = 3669
end

function SCPetMonopolySpecialItem:Decode()
    self.double_surplus = MsgAdapter.ReadInt()
    self.use_item_type = MsgAdapter.ReadInt()
    self.special_item_num = 0
    self.special_item_group = {}
    for i = 0, 4 do
        self.special_item_group[i] = MsgAdapter.ReadInt()
        self.special_item_num = self.special_item_num + self.special_item_group[i]
    end
end

--使用心愿卡后变化信息
SCPetMonopolyWishCardGrid = SCPetMonopolyWishCardGrid or BaseClass(BaseProtocolStruct)
function SCPetMonopolyWishCardGrid:__init()
    self.msg_type =  3670
end

function SCPetMonopolyWishCardGrid:Decode()
    local index_list = {}
    local count = MsgAdapter.ReadInt()
    for i = 0, 6 do
        index_list[i] = MsgAdapter.ReadInt()
    end
    self.changed_grid_info = {}
    for i = 0, count - 1 do
        local grid_info = {}
        grid_info.reward_type = MsgAdapter.ReadShort()
        grid_info.grid_type = MsgAdapter.ReadShort()
        grid_info.param1 = MsgAdapter.ReadInt()             --格子是宠物时为pet_id，物品或碎片时为物品或碎片id
        grid_info.param2 = MsgAdapter.ReadInt()             --格子是物品或碎片时表示数量
        grid_info.grid_index = index_list[i]
        table.insert(self.changed_grid_info, grid_info)
    end
end

---云端竞技-----------------
-- enum OPER_TYPE
-- {
--     OPER_TYPE_SELF_INFO = 0,        //!< 获取自身信息
--     OPER_TYPE_TARGET_INFO = 1,      //!< 获取目标信息
--     OPER_TYPE_REFRESH_TARGET = 2,   //!< 刷新目标列表
--     OPER_TYPE_START_FIGHT = 3,      //!< 开始挑战  param1:挑战第X个目标 X=[0, 4)
--     OPER_TYPE_BUY_FIGHT_TIMES = 4,  //!< 购买挑战次数
--     OPER_TYPE_RECORD_LIST = 5,      //!< 请求战报列表  param1:从第几条战报开始  param2:请求多少条战报
--     OPER_TYPE_RANK_LIST = 6,        //!< 请求排行榜  param1:排行榜类型  (0:总榜  1:本服榜  2:段位榜)
-- };

CSCloudArenaOperate = CSCloudArenaOperate or BaseClass(BaseProtocolStruct)
function CSCloudArenaOperate:__init()
    self.msg_type = 3675
end

function CSCloudArenaOperate:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.operate_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

SCCloudArenaSelfInfo = SCCloudArenaSelfInfo or BaseClass(BaseProtocolStruct)
function SCCloudArenaSelfInfo:__init()
    self.msg_type = 3676
end

function SCCloudArenaSelfInfo:Decode()
    self.cur_rank = MsgAdapter.ReadInt()
    self.cur_score = MsgAdapter.ReadInt()
    self.capability = MsgAdapter.ReadInt()
    self.remain_fight_times = MsgAdapter.ReadShort()
    self.day_reward_status = MsgAdapter.ReadChar() --日奖励 0不可领，1可领，2已领
    self.has_today_fight = MsgAdapter.ReadChar() --今天是否挑战过 0:否 1:是
    self.season_finish_timestamp = MsgAdapter.ReadUInt()
    self.next_recover_fighttimes_timestamp = MsgAdapter.ReadUInt()
    self.reach_ranking_flag = MsgAdapter.IntFlag(0,true)
    self.fetch_ranking_reward_flag = MsgAdapter.IntFlag(0,true)
    self.season_idx = MsgAdapter.ReadInt() + 1
    self.today_buy_times = MsgAdapter.ReadInt()
    self.last_day_score = MsgAdapter.ReadInt()
end

SCCloudArenaTargetInfo = SCCloudArenaTargetInfo or BaseClass(BaseProtocolStruct)
function SCCloudArenaTargetInfo:__init()
    self.msg_type = 3677
end

function SCCloudArenaTargetInfo:Decode()
    self.next_manual_refresh_timestamp = MsgAdapter.ReadUInt()
    self.today_already_refresh_times = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.target_list = {}
    for i=1,self.count do
        local t = {}
        t.target_index = i
        t.rank = MsgAdapter.ReadInt()           --排行
        t.score = MsgAdapter.ReadInt()          --竞技点
        t.capability = MsgAdapter.ReadInt()     --积分战力
        t.top_level = MsgAdapter.ReadInt()      --巅峰等级
        t.level = MsgAdapter.ReadShort()        --角色等级
        t.is_robot = MsgAdapter.ReadShort()     --是否为机器人 1是 0否
        t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN) --角色名称
        t.appearance = Appearance()             --外观信息
        t.appearance.bicycle_index = -1         --
        t.appearance.ride_id = 0
        if i <= CloudArenaData.TargetCount then
            table.insert(self.target_list, t)
        end
    end
end
--有新战报
SCCloudArenaNewFightRecordNotice = SCCloudArenaNewFightRecordNotice or BaseClass(BaseProtocolStruct)
function SCCloudArenaNewFightRecordNotice:__init()
    self.msg_type = 3678
end

function SCCloudArenaNewFightRecordNotice:Decode()

end

local function cloudArenaFightRecordUserInfo()
    local t = {}
    t.uid = MsgAdapter.ReadInt()
    t.level = MsgAdapter.ReadInt()
    t.profession = MsgAdapter.ReadInt()
    t.avatar = MsgAdapter.ReadInt()
    t.avatar_id = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.change_score = MsgAdapter.ReadInt()
    t.change_arena_coin = MsgAdapter.ReadInt()
    t.new_rank = MsgAdapter.ReadInt() --正数上升 负数下降
    t.old_rank = MsgAdapter.ReadInt()
    t.old_score = MsgAdapter.ReadInt()
    t.is_robot = MsgAdapter.ReadShort()
    reserve_sh = MsgAdapter.ReadShort()
    return t
end
--战报列表
SCCloudArenaFightRecordList = SCCloudArenaFightRecordList or BaseClass(BaseProtocolStruct)
function SCCloudArenaFightRecordList:__init()
    self.msg_type = 3679
end

function SCCloudArenaFightRecordList:Decode()
    self.offset = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.record_list = {}
    for i=1,self.count do
        local t = {}
        t.record_id1 = tostring(MsgAdapter.ReadUInt())
        t.record_id2 = tostring(MsgAdapter.ReadUInt())
        t.record_timestamp = MsgAdapter.ReadUInt()
        t.is_attacker_win = MsgAdapter.ReadInt()
        t.attacker_info = cloudArenaFightRecordUserInfo()
        t.defender_info = cloudArenaFightRecordUserInfo()
        self.record_list[i] = t
    end
end

local function rankInfo()
    local t= {}
    t.rank = MsgAdapter.ReadInt()
    t.uid = MsgAdapter.ReadInt()
    t.top_level = MsgAdapter.ReadInt()
    t.level = MsgAdapter.ReadShort()
    t.is_robot = MsgAdapter.ReadShort()
    t.profession = MsgAdapter.ReadInt()
    t.score = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    return t
end
--云端竞技排行榜
SCCloudArenaRankList = SCCloudArenaRankList or BaseClass(BaseProtocolStruct)
function SCCloudArenaRankList:__init()
    self.msg_type = 3680
end

function SCCloudArenaRankList:Decode()
    self.rank_type = MsgAdapter.ReadShort()
    self.rank_num = MsgAdapter.ReadShort()
    self.my_rank = rankInfo()
    self.my_rank.my_rank = true
    self.rank_list = {}
    for i=1,self.rank_num do
        self.rank_list[i] = rankInfo()
    end
end
--赛季初始积分
--用于打开赛季初始界面的协议
SCCloudArenaNoticeSeasonInitialScore = SCCloudArenaNoticeSeasonInitialScore or BaseClass(BaseProtocolStruct)
function SCCloudArenaNoticeSeasonInitialScore:__init()
    self.msg_type = 3681
end

function SCCloudArenaNoticeSeasonInitialScore:Decode()
    self.season_initial_score = MsgAdapter.ReadInt()
end

--大段位变化通知
SCCloudArenaNoticeDanChange = SCCloudArenaNoticeDanChange or BaseClass(BaseProtocolStruct)
function SCCloudArenaNoticeDanChange:__init()
    self.msg_type = 3682
end

function SCCloudArenaNoticeDanChange:Decode()
    self.old_dan = MsgAdapter.ReadShort()
    self.new_dan = MsgAdapter.ReadShort()
end
--成就信息
SCCloudArenaAchievementInfo = SCCloudArenaAchievementInfo or BaseClass(BaseProtocolStruct)
function SCCloudArenaAchievementInfo:__init()
    self.msg_type = 3683
end

function SCCloudArenaAchievementInfo:Decode()
    self.achieve_list = {}
    for i=1, CLOUD_ARENA_ACHIEVEMENT_MAX do
        local t = {}
        t.achieve_type = i - 1
        t.progress_num = MsgAdapter.ReadInt()
        t.cur_level = MsgAdapter.ReadShort()
        t.reward_status = MsgAdapter.ReadShort()
        self.achieve_list[i] = t
        -- self.achieve_list[CLOUD_ARENA_ACHIEVEMENT[t.achieve_type]] = t
    end
end
--成就变化通知
SCCloudArenaAchievementUpdateNotice = SCCloudArenaAchievementUpdateNotice or BaseClass(BaseProtocolStruct)
function SCCloudArenaAchievementUpdateNotice:__init()
    self.msg_type = 3684
end

function SCCloudArenaAchievementUpdateNotice:Decode()
    self.achieve_type = MsgAdapter.ReadInt()
    self.progress_num = MsgAdapter.ReadInt()
    self.cur_level = MsgAdapter.ReadShort()
    self.reward_status = MsgAdapter.ReadShort()
end
--赛季最好成绩
SCCloudArenaRoleSeasonHighestData = SCCloudArenaRoleSeasonHighestData or BaseClass(BaseProtocolStruct)
function SCCloudArenaRoleSeasonHighestData:__init()
    self.msg_type = 3685
end

function SCCloudArenaRoleSeasonHighestData:Decode()
    self.season_highest_rank = MsgAdapter.ReadInt() --最高排行
    self.season_highest_dan_id = MsgAdapter.ReadShort() --小段位id
    self.season_highest_score = MsgAdapter.ReadShort()--赛季历史最高分
    self.season_shop_type = MsgAdapter.ReadInt() --显示赛季商店
end

SCCloudArenaRealTimeInfo = SCCloudArenaRealTimeInfo or BaseClass(BaseProtocolStruct)
function SCCloudArenaRealTimeInfo:__init()
    self.msg_type = 3686
end

function SCCloudArenaRealTimeInfo:Decode()
    self.cur_rank = MsgAdapter.ReadInt()
    self.cur_score = MsgAdapter.ReadInt()
end

CSCloudArenaRealTimeInfo = CSCloudArenaRealTimeInfo or BaseClass(BaseProtocolStruct)
function CSCloudArenaRealTimeInfo:__init()
    self.msg_type = 3687
end

function CSCloudArenaRealTimeInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

CSCloudArenaSetFormation = CSCloudArenaSetFormation or BaseClass(BaseProtocolStruct)
function CSCloudArenaSetFormation:__init()
    self.msg_type = 3688
end

function CSCloudArenaSetFormation:Encode()
   MsgAdapter.WriteBegin(self.msg_type)
   for i=1, 10 do
        MsgAdapter.WriteInt(self.grid_list[i].id)
        MsgAdapter.WriteShort(self.grid_list[i].type)
        MsgAdapter.WriteShort(0)
        MsgAdapter.WriteUInt(self.grid_list[i].unique_id_1)
        MsgAdapter.WriteUInt(self.grid_list[i].unique_id_2)
        -- MsgAdapter.WriteLL(self.grid_list[i].unique_id)
   end
   for i=1, 5 do
        MsgAdapter.WriteInt(self.standby_list[i].pet_id or 0)
        MsgAdapter.WriteUInt(self.standby_list[i].unique_id_1 or 0)
        MsgAdapter.WriteUInt(self.standby_list[i].unique_id_2 or 0)
        -- MsgAdapter.WriteLL(self.standby_list[i].unique_id)
   end
end

SCCloudArenaFormationInfo = SCCloudArenaFormationInfo or BaseClass(BaseProtocolStruct)
function SCCloudArenaFormationInfo:__init()
    self.msg_type = 3689
end

function SCCloudArenaFormationInfo:Decode()
    self.grid_list = {}
    for i=1,10 do
        self.grid_list[i] = {}
        self.grid_list[i].id = MsgAdapter.ReadInt()         --主角uid/pet_id/partner_id
        self.grid_list[i].type = MsgAdapter.ReadShort()     --0为空 1主角 2宠物 3伙伴
        MsgAdapter.ReadShort()
        self.grid_list[i].unique_id_1 = MsgAdapter.ReadUInt()   --宠物时为unique_id
        self.grid_list[i].unique_id_2 = MsgAdapter.ReadUInt()
    end
    self.standby_list = {}
    for i = 1, 5 do
        self.standby_list[i] = {}
        self.standby_list[i].pet_id = MsgAdapter.ReadInt()      --待机宠物id 值0这没有
        self.standby_list[i].unique_id_1 = MsgAdapter.ReadUInt()    --宠物唯一id
        self.standby_list[i].unique_id_2 = MsgAdapter.ReadUInt()
    end
end

SCMoveAssistReq = SCMoveAssistReq or BaseClass(BaseProtocolStruct)
function SCMoveAssistReq:__init()
    self.msg_type = 3690
end

function SCMoveAssistReq:Decode()
    self.move_assist_id = MsgAdapter.ReadUInt()
    self.scene_id = MsgAdapter.ReadInt()
    self.scene_key = MsgAdapter.ReadInt()
    self.obj_id = MsgAdapter.ReadUShort()
    self.assist_type = MsgAdapter.ReadShort() --1寻路 2场景巡逻 3交互
    self.speed = MsgAdapter.ReadInt()
    self.cur_pos_x = MsgAdapter.ReadShort()
    self.cur_pos_y = MsgAdapter.ReadShort()
    self.target_pos_x = MsgAdapter.ReadShort()
    self.target_pos_y = MsgAdapter.ReadShort()
end

CSMoveAssistRet = CSMoveAssistRet or BaseClass(BaseProtocolStruct)
function CSMoveAssistRet:__init()
    self.msg_type = 3691
end

function CSMoveAssistRet:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUInt(self.move_assist_id)
    MsgAdapter.WriteInt(self.scene_id)
    MsgAdapter.WriteInt(self.scene_key)

    MsgAdapter.WriteUShort(self.obj_id)
    MsgAdapter.WriteShort(self.assist_type)

    MsgAdapter.WriteShort(self.cur_pos_x)
    MsgAdapter.WriteShort(self.cur_pos_y)
    MsgAdapter.WriteShort(self.target_pos_x)
    MsgAdapter.WriteShort(self.target_pos_y)
    MsgAdapter.WriteInt(#self.way_point)
    for i=1,#self.way_point do
        MsgAdapter.WriteShort(self.way_point[i].pos_x)
        MsgAdapter.WriteShort(self.way_point[i].pos_y)
    end
end

CSActiveSignUpReq = CSActiveSignUpReq or BaseClass(BaseProtocolStruct)
function CSActiveSignUpReq:__init()
    self.msg_type = 3692
end

function CSActiveSignUpReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.act_type)
end

SCActiveSignUpInfo = SCActiveSignUpInfo or BaseClass(BaseProtocolStruct)
function SCActiveSignUpInfo:__init()
    self.msg_type = 3693
end

function SCActiveSignUpInfo:Decode()
    self.can_flag = MsgAdapter.IntFlag()
    self.get_flag = MsgAdapter.IntFlag()
end

-- 活跃一键完成完成后信息下发
SCActiveDailyAllFinishInfo = SCActiveDailyAllFinishInfo or BaseClass(BaseProtocolStruct)
function SCActiveDailyAllFinishInfo:__init()
    self.msg_type = 3694
end

function SCActiveDailyAllFinishInfo:Decode()
    self.act_types = MsgAdapter.IntFlag()
    self.exp = MsgAdapter.ReadInt()
    self.current_types = {}
    self.current_types[CurrencyType.CoinBind] = MsgAdapter.ReadInt()
    self.current_types[CurrencyType.ImmortalCoin] = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.item_list = {}
    local add_fun = function(currency_type)
        if self.current_types[currency_type] > 0 then
            local item = {}
            item.item_id = HuoBi[currency_type]
            item.is_bind = 1
            item.num = self.current_types[currency_type]
            table.insert(self.item_list, item)
        end
    end
    add_fun(CurrencyType.CoinBind)
    add_fun(CurrencyType.ImmortalCoin)
    for i = 1, self.count do
        local item = {}
        item.item_id = MsgAdapter.ReadUShort()
        item.is_bind = MsgAdapter.ReadShort()
        item.num = MsgAdapter.ReadInt()
        table.insert(self.item_list, item)
    end
end