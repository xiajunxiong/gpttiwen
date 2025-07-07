--幻界战场请求 0匹配 1取消匹配 2排行榜信息 p1 0跨服 server_id 自己服 4 自己想信息  5 请求与各职业的对战胜率信息 6 队长界面 p1 0关闭 1打开
CSBattlefieldReq = CSBattlefieldReq or BaseClass(BaseProtocolStruct)
function CSBattlefieldReq:__init()
    self.msg_type = 3290
end

function CSBattlefieldReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

--幻界战场 队员通知
SCTeamMemberNotice = SCTeamMemberNotice or BaseClass(BaseProtocolStruct)
function SCTeamMemberNotice:__init()
	self.msg_type = 3291
end

function SCTeamMemberNotice:Decode()
    self.notice = MsgAdapter.ReadInt() --0关闭 1打开
end

--幻界战场 队员其他通知
SCTeamMemberOtherNotice = SCTeamMemberOtherNotice or BaseClass(BaseProtocolStruct)
function SCTeamMemberOtherNotice:__init()
	self.msg_type = 3292
end

function SCTeamMemberOtherNotice:Decode()
    self.role_id = MsgAdapter.ReadInt()
    self.reson = MsgAdapter.ReadInt()
end

--幻界战场 匹配状态
SCBattlefieldMatch = SCBattlefieldMatch or BaseClass(BaseProtocolStruct)
function SCBattlefieldMatch:__init()
	self.msg_type = 3301
end

function SCBattlefieldMatch:Decode()
    self.is_match = MsgAdapter.ReadInt()
end

--战斗结果
SCBattlefieldResult = SCBattlefieldResult or BaseClass(BaseProtocolStruct)
function SCBattlefieldResult:__init()
    self.msg_type = 3302
end

function SCBattlefieldResult:Decode()
    self.result = MsgAdapter.ReadInt()    --结果
    self.rank_score = MsgAdapter.ReadInt()--当前分
    self.win_streak_times = MsgAdapter.ReadInt() --连胜次数
    self.is_top_race = MsgAdapter.ReadInt() --是否完成顶级赛
    self.ranking_num = BattlefieldData.Instance:GetCurRankConfig(self.rank_score).ranking
end

--玩家信息
SCBattlefieldRoleInfo = SCBattlefieldRoleInfo or BaseClass(BaseProtocolStruct)
function SCBattlefieldRoleInfo:__init()
    self.msg_type = 3304
end

function SCBattlefieldRoleInfo:Decode()
    self.season_num = MsgAdapter.ReadInt()--当前赛季
    self.stamp_time = MsgAdapter.ReadUInt() --赛季结束倒计时
    self.rank_score = MsgAdapter.ReadInt()--当前分
    self.max_rank_score = MsgAdapter.ReadInt() --历史最高积分
    self.win_streak_times = MsgAdapter.ReadInt()--连胜次数
    self.tolal_times = MsgAdapter.ReadUInt()--总次数
    self.win_times = MsgAdapter.ReadUInt()--胜利次数
    self.draw_times = MsgAdapter.ReadUInt()--平手次数
    self.is_top_race = MsgAdapter.ReadInt() --是否完成顶级赛
    self.ranks_info = {
        tolal_times = self.tolal_times,
        win_times = self.win_times,
        draw_times = self.draw_times
    }
    self.last_info = {}
    self.last_info.cur_score = BattlefieldData.Instance:GetRankScore()
    self.last_info.change_val = self.rank_score - self.last_info.cur_score
    self.last_info.ranking_num = BattlefieldData.Instance:GetRankingNum()
    self.last_info.is_top_race = BattlefieldData.Instance:GetTopRace()
    self.ranking_num = BattlefieldData.Instance:GetCurRankConfig(self.rank_score).ranking
    self.day_money_num = MsgAdapter.ReadInt()
    self.today_win_times = MsgAdapter.ReadInt()
    self.today_win_reward_flag = MsgAdapter.IntFlag(0)
end

--排行榜
SCBattlefieldRankInfo = SCBattlefieldRankInfo or BaseClass(BaseProtocolStruct)
function SCBattlefieldRankInfo:__init()
    self.msg_type = 3305
end

function SCBattlefieldRankInfo:Decode()
    self.server_id = MsgAdapter.ReadInt()
    self.rank_list = {}
    for i=1,MsgAdapter.ReadInt() do
        local info = {}
        info.rank = i
        info.role_id = MsgAdapter.ReadInt()
        info.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        info.prof = math.floor(MsgAdapter.ReadInt() / 100)
        info.score = MsgAdapter.ReadInt()
        self.rank_list[i] = info
    end
    self.rank_type = self.server_id == MainProber.server_id and BattlefieldRankType.Main or BattlefieldRankType.Cross
end

--与各职业对战胜率信息
SCBattlefieldReportRateInfo = SCBattlefieldReportRateInfo or BaseClass(BaseProtocolStruct)
function SCBattlefieldReportRateInfo:__init()
    self.msg_type = 3309
end

function SCBattlefieldReportRateInfo:Decode()
    self.report_list = {}
    for i=0,ROLE_PROFESSION_MAX_NUM do
        local info = {}
        info.prof = i
        info.win_times = MsgAdapter.ReadInt()
        info.lose_times = MsgAdapter.ReadInt()
        info.draw_times = MsgAdapter.ReadInt()
        info.win_rate = self:GetRateNum(info)
        if info.prof ~= 0 then
            self.report_list[i] = info
        end
    end
end

function SCBattlefieldReportRateInfo:GetRateNum(info)
    local tolal_times = info.win_times + info.lose_times + info.draw_times
    tolal_times = tolal_times == 0 and 1 or tolal_times
    return math.floor(info.win_times / tolal_times * 100)
end

--===================================化身模块===================================
CSHuaShenInfoReq = CSHuaShenInfoReq or BaseClass(BaseProtocolStruct)
function CSHuaShenInfoReq:__init()
    self.msg_type = 3328
end

function CSHuaShenInfoReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

--下发玩家本赛季信息
SCHuaShenSeasonInfo = SCHuaShenSeasonInfo or BaseClass(BaseProtocolStruct)
function SCHuaShenSeasonInfo:__init()
    self.msg_type = 3333
end

function SCHuaShenSeasonInfo:Decode()
    self.season_first_enter = MsgAdapter.ReadChar()--0:本赛季还未进入   1:本赛季已经进入过 
    self.is_finish_guide = MsgAdapter.ReadChar() --化身指引
    self.season_first_enter_top = MsgAdapter.ReadShort()--0:本赛季还未进入   1:本赛季已经进入过 
end

--===================================化身人物下发===================================
SCHuaShenRoleInfoAck = SCHuaShenRoleInfoAck or BaseClass(BaseProtocolStruct)
function SCHuaShenRoleInfoAck:__init()
    self.msg_type = 3310
end

function SCHuaShenRoleInfoAck:Decode()
    self.role_level = MsgAdapter.ReadInt()
    self.appearance = Appearance()-- 外观
end

--化身属性列表
SCHuaShenRoleAttrList = SCHuaShenRoleAttrList or BaseClass(BaseProtocolStruct)
function SCHuaShenRoleAttrList:__init()
    self.msg_type = 3311
end

function SCHuaShenRoleAttrList:Decode()
    self.attr_count = MsgAdapter.ReadShort()   --下发的属性条数
    self.notify_reason = MsgAdapter.ReadShort()--下发的理由
    self.capability = MsgAdapter.ReadInt()     --玩家战力(评分)
    self.attr_list = {}
    for i=1,self.attr_count do
        self.attr_list[MsgAdapter.ReadInt()] = MsgAdapter.ReadInt()
    end
end

local function PlanDetail(index)
    local t = {}
    t.plan_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.add_point_num = MsgAdapter.ReadShort()
    t.add_point_list = {}
    local num = 0
    for i=0,ROLE_ATTRIBUTE_PLAN_MAX_NUM - 1 do
        local info = {name = Language.Role.AddPointTypeName[i],type = i}
        info.value = MsgAdapter.ReadShort()
        t.add_point_list[i] = info
        num = num + info.value
    end
    t.all_point_num = t.add_point_num + num
    if t.plan_name == "" then
        t.plan_name = Language.Role.DefaultPlanName .. index or 1
    end
    return t
end

--化身潜能操作
CSHuaShenAttributePlanPointReq = CSHuaShenAttributePlanPointReq or BaseClass(BaseProtocolStruct)
function CSHuaShenAttributePlanPointReq:__init()
    self.msg_type = 3320
end

function CSHuaShenAttributePlanPointReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.req_type) -- 0 洗点 1 加点 2切换方案
    for i=0,ROLE_ATTRIBUTE_PLAN_MAX_NUM - 1 do
        MsgAdapter.WriteShort(self.add_point_list[i] or 0)
    end
    MsgAdapter.WriteInt(self.plan_index)
end

--化身潜能改名
CSHuaShenAttributePlanChangeName = CSHuaShenAttributePlanChangeName or BaseClass(BaseProtocolStruct)
function CSHuaShenAttributePlanChangeName:__init()
    self.msg_type = 3336
end

function CSHuaShenAttributePlanChangeName:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.plan_index) 
    MsgAdapter.WriteStrN(self.plan_name,PROTC_ROLE_NAME_LEN)
end

SCHuaShenMedalTakeOffInfo = SCHuaShenMedalTakeOffInfo or BaseClass(BaseProtocolStruct)
function SCHuaShenMedalTakeOffInfo:__init()
    self.msg_type = 3337
end

function SCHuaShenMedalTakeOffInfo:Decode()
    self.out_index = MsgAdapter.ReadInt()
end 

--化身全部人物加点信息
SCHuaShenRoleAttributePlanAllInfo = SCHuaShenRoleAttributePlanAllInfo or BaseClass(BaseProtocolStruct)
function SCHuaShenRoleAttributePlanAllInfo:__init()
    self.msg_type = 3335
end

function SCHuaShenRoleAttributePlanAllInfo:Decode()
    self.cur_plan_index = MsgAdapter.ReadInt()
    self.plan_list = {}
    for i=0,2 do
        self.plan_list[i] = PlanDetail(i+1)
    end
    self.plan_info = self.plan_list[self.cur_plan_index]
end

SCHuaShenRoleAttributePlanInfo = SCHuaShenRoleAttributePlanInfo or BaseClass(BaseProtocolStruct)
function SCHuaShenRoleAttributePlanInfo:__init()
    self.msg_type = 3321
end

function SCHuaShenRoleAttributePlanInfo:Decode()
    self.cur_plan_index = MsgAdapter.ReadInt()
    self.plan_info = PlanDetail(self.cur_plan_index + 1)
end

--设置主角站位    
CSHuaShenBattlePosition = CSHuaShenBattlePosition or BaseClass(BaseProtocolStruct)
function CSHuaShenBattlePosition:__init()
    self.msg_type = 3331
end

function CSHuaShenBattlePosition:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type) --主角站位  （0：后排   1：前排）
end

--设置主角站位返回 
SCHuaShenBattlePosition = SCHuaShenBattlePosition or BaseClass(BaseProtocolStruct)
function SCHuaShenBattlePosition:__init()
    self.msg_type = 3332
end

function SCHuaShenBattlePosition:Decode()
    self.position = MsgAdapter.ReadInt()--主角站位  （0：后排   1：前排）
end

--===================================化身装备背包下发===================================
--化身勋章请求 0请求信息 1穿戴装备 p1 index 2 脱下装备
CSHuaShenMedalReq = CSHuaShenMedalReq or BaseClass(BaseProtocolStruct)
function CSHuaShenMedalReq:__init()
    self.msg_type = 3312
end

function CSHuaShenMedalReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

--下发此时正在穿戴的勋章
SCHuaShenMedalInfo = SCHuaShenMedalInfo or BaseClass(BaseProtocolStruct)
function SCHuaShenMedalInfo:__init()
    self.msg_type = 3313
end

function SCHuaShenMedalInfo:Decode()
    self.medal_list = {}
    self.count = MsgAdapter.ReadInt()
    for i = 1, self.count do
        local t = {}
        t.grid_index = MsgAdapter.ReadInt()
        t.item_id = MsgAdapter.ReadUShort() -- 道具ID
        t.num = MsgAdapter.ReadShort()
        t.is_bind = MsgAdapter.ReadChar()
        t.has_param = MsgAdapter.ReadChar() ~= 0
        t.invalid_time = MsgAdapter.ReadUInt()
        if t.has_param then
            local param_length = MsgAdapter.ReadShort()
            t.param = ItemDataParam(t.item_id, param_length)
        end
        self.medal_list[t.grid_index + 1] = t
    end
    for i=1,3 do
        if self.medal_list[i] == nil then
            self.medal_list[i] = {item_id = 0,grid_index = i-1}
        end
    end
    self.medal_id = self.medal_list[1].item_id
end

CSHuaShenEquipmentOperaReq = CSHuaShenEquipmentOperaReq or BaseClass(BaseProtocolStruct)
function CSHuaShenEquipmentOperaReq:__init()
    self.msg_type = 3314
end

function CSHuaShenEquipmentOperaReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
    MsgAdapter.WriteInt(self.param4)
end

--化身装备信息
SCHuaShenEquipmentInfoAck = SCHuaShenEquipmentInfoAck or BaseClass(BaseProtocolStruct)
function SCHuaShenEquipmentInfoAck:__init()
    self.msg_type = 3315
end

function SCHuaShenEquipmentInfoAck:Decode()
    self.element_crystal = {element_crystal_id = MsgAdapter.ReadUShort(),used_times = MsgAdapter.ReadShort()}
    MsgAdapter.ReadShort()
    self.equipment_list = {}
    self.equipment_count = MsgAdapter.ReadShort()
    for i = 1, self.equipment_count do
        local equip_info = InfoType()
        self.equipment_list[equip_info.index] = equip_info
    end
end

SCHuaShenElementCrystalChangeNotice = SCHuaShenElementCrystalChangeNotice or BaseClass(BaseProtocolStruct)
function SCHuaShenElementCrystalChangeNotice:__init()
    self.msg_type = 3334
end

function SCHuaShenElementCrystalChangeNotice:Decode()
    self.element_crystal = {element_crystal_id = MsgAdapter.ReadUShort(),used_times = MsgAdapter.ReadShort()}
end

--化身装备改变通知
SCHuaShenEquipmentGridChange = SCHuaShenEquipmentGridChange or BaseClass(BaseProtocolStruct)
function SCHuaShenEquipmentGridChange:__init()
    self.msg_type = 3316
end

function SCHuaShenEquipmentGridChange:Decode()
    self.equip_info = InfoType()
    self.index = self.equip_info.index
end

--灵饰 系统操作请求 0请求所有 灵饰 信息 1请求装备灵饰 2请求脱下灵饰  
CSHuaShenJewelryHandleReq = CSHuaShenJewelryHandleReq or BaseClass(BaseProtocolStruct)
function CSHuaShenJewelryHandleReq:__init()
    self.msg_type = 3317
end

function CSHuaShenJewelryHandleReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.req_type)
    MsgAdapter.WriteShort(self.param1)--额外参数：0 - 无； 1 - 背包格子ID； 2 - 灵饰装备格ID
end

--装备灵饰信息发送
SCHuaShenJewelryAllInform = SCHuaShenJewelryAllInform or BaseClass(BaseProtocolStruct)
function SCHuaShenJewelryAllInform:__init()
    self.msg_type = 3318
end

function SCHuaShenJewelryAllInform:Decode()
    self.jewelry_list = {}
    for i = 1, MsgAdapter.ReadInt() do
        local info = JewelryPro()
        self.jewelry_list[info.jewelry_type] = info
    end
end

--单个灵饰信息发送
SCHuaShenJewelryInfoChange = SCHuaShenJewelryInfoChange or BaseClass(BaseProtocolStruct)
function SCHuaShenJewelryInfoChange:__init()
    self.msg_type = 3319
end

function SCHuaShenJewelryInfoChange:Decode()
    self.jewelry = JewelryPro()
    self.index = self.jewelry.index
end

--背包信息返回 0 客户端请求所有信息 1 整理 2 客户端单独请求背包信息
SCHuaShenKnapsackInfoAck = SCHuaShenKnapsackInfoAck or BaseClass(BaseProtocolStruct)
function SCHuaShenKnapsackInfoAck:__init()
    self.msg_type = 3329
end

function SCHuaShenKnapsackInfoAck:Decode()
    self.valid_grid_num = MsgAdapter.ReadInt()
    self.reason = MsgAdapter.ReadShort()
    self.item_count = MsgAdapter.ReadShort()
    self.item_list = {}
	for i = 1, self.item_count do
		local tab = {}
        self.item_list[i] = tab
        ItemDataWrapper(tab)
	end
end

--单个物品改变通知
SCHuaShenKnapsackItemChange = SCHuaShenKnapsackItemChange or BaseClass(BaseProtocolStruct)
function SCHuaShenKnapsackItemChange:__init()
    self.msg_type = 3330
end

function SCHuaShenKnapsackItemChange:Decode()
    self.reason = MsgAdapter.ReadShort()
	self.put_reason = MsgAdapter.ReadShort()
	ItemDataWrapper(self)
end

--===================================化身宠物下发===================================
--化身宠物操作 req_type
--0 设置宠物状态		将index为p1的宠物设置成p2[1:出战 2:跟随 3:待机1 4:待机2 5:待机3]等状态
--1 洗点		       给宠物index为p1的宠物洗点
--0 请求宠物属性		请求index为p1的宠物的属性
CSHuaShenPetOp = CSHuaShenPetOp or BaseClass(BaseProtocolStruct)
function CSHuaShenPetOp:__init()
    self.msg_type = 3322
end

function CSHuaShenPetOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

--化身请求分配宠物属性点 
CSHuaShenPetAssignAttributePoints = CSHuaShenPetAssignAttributePoints or BaseClass(BaseProtocolStruct)
function CSHuaShenPetAssignAttributePoints:__init()
    self.msg_type = 3323
end

function CSHuaShenPetAssignAttributePoints:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.index)
    MsgAdapter.WriteInt(self.con)
    MsgAdapter.WriteInt(self.str)
    MsgAdapter.WriteInt(self.def)
    MsgAdapter.WriteInt(self.agi)
    MsgAdapter.WriteInt(self.magic)
end

--宠物所有的信息 
SCHuaShenSendPetAllInfo = SCHuaShenSendPetAllInfo or BaseClass(BaseProtocolStruct)
function SCHuaShenSendPetAllInfo:__init()
    self.msg_type = 3324
end

function SCHuaShenSendPetAllInfo:Decode()
    self.reason = MsgAdapter.ReadInt()
    self.pet_list = {}
    self.pet_count = MsgAdapter.ReadInt()
    for i = 1, self.pet_count do
        local info = PortocolCommon.PetProParam()
        local pet = Pet.New(info)
        if pet.config ~= nil then
            self.pet_list[info.index] = Pet.New(info)
        end
    end
end

--单个宠物的信息
SCHuaShenSendPetSingleInfo = SCHuaShenSendPetSingleInfo or BaseClass(BaseProtocolStruct)
function SCHuaShenSendPetSingleInfo:__init()
    self.msg_type = 3325
end

function SCHuaShenSendPetSingleInfo:Decode()
    self.notie_type = MsgAdapter.ReadInt()
    local info = PortocolCommon.PetProParam()
    self.pet_info = Pet.New(info)
end

--宠物其他信息
SCHuaShenSendPetOtherInfo = SCHuaShenSendPetOtherInfo or BaseClass(BaseProtocolStruct)
function SCHuaShenSendPetOtherInfo:__init()
    self.msg_type = 3326
end

function SCHuaShenSendPetOtherInfo:Decode()
    self.pet_other_info = {}
    self.pet_other_info.fight_index = MsgAdapter.ReadShort()
    self.pet_other_info.standby_index1 = MsgAdapter.ReadShort()
    self.pet_other_info.standby_index2 = MsgAdapter.ReadShort()
    self.pet_other_info.standby_index3 = MsgAdapter.ReadShort()
    self.pet_other_info.standby_index4 = MsgAdapter.ReadShort()
    self.pet_other_info.standby_index5 = MsgAdapter.ReadShort()
    self.pet_other_info.standby_list = {}
    for i = 1, PetData.Instance:GetMaxPetOn() do
        self.pet_other_info.standby_list[i] = self.pet_other_info["standby_index" .. i] or -1
    end
end

--宠物属性
SCHuaShenSendPetAttrListInfo = SCHuaShenSendPetAttrListInfo or BaseClass(BaseProtocolStruct)
function SCHuaShenSendPetAttrListInfo:__init()
    self.msg_type = 3327
end

function SCHuaShenSendPetAttrListInfo:Decode()
    self.attr_list = {}
    self.index = MsgAdapter.ReadInt()
    self.capaility = MsgAdapter.ReadInt()
    for i = 1, PET_ATTR_LIST_COUNT do
        self.attr_list[i - 1] = MsgAdapter.ReadInt()
    end
end

--神秘商店请求
CSMysteryStoreReq = CSMysteryStoreReq or BaseClass(BaseProtocolStruct)
function CSMysteryStoreReq:__init()
    self.msg_type = 3350
end

function CSMysteryStoreReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.req_type)  -- 0 刷新商品 1：购买
    MsgAdapter.WriteShort(self.param)     -- 传下发的信息的数组下标
end

--神秘商店信息
SCMysteryStoreInfo = SCMysteryStoreInfo or BaseClass(BaseProtocolStruct)
function SCMysteryStoreInfo:__init()
    self.msg_type = 3351
end
function SCMysteryStoreInfo:Decode()
    self.next_free_refresh_time = MsgAdapter.ReadUInt()
    self.remain_update_time = MsgAdapter.ReadUInt()
    self.refresh_num = MsgAdapter.ReadShort()   -- 刷新次数
    local count = MsgAdapter.ReadShort()
    self.StoreCardInfos = {}
    for i=1,count do
        local info = {}
        info.buy_count = MsgAdapter.ReadChar()
        info.group_id = MsgAdapter.ReadChar()
        info.seq = MsgAdapter.ReadShort()
        info.attr_list = {}
        for j = 1, PartnerConfig.PARTNER_EQUIP_VOC_NUM do
            info.attr_list[j] = {}
            info.attr_list[j].attr_type = MsgAdapter.ReadInt()
            info.attr_list[j].attr_value = MsgAdapter.ReadInt()
        end
        info.buy_index = i - 1
        self.StoreCardInfos[i] = info
    end
end

--山海降妖请求
CSShanHaiBossInfoReq = CSShanHaiBossInfoReq or BaseClass(BaseProtocolStruct)
function CSShanHaiBossInfoReq:__init()
    self.msg_type = 3370
end

function CSShanHaiBossInfoReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.req_type)  -- 0：请求BOSS信息
    MsgAdapter.WriteShort(self.param)  
end

--山海降妖信息
SCShanHaiBossInfo = SCShanHaiBossInfo or BaseClass(BaseProtocolStruct)
function SCShanHaiBossInfo:__init()
    self.msg_type = 3371
end

function SCShanHaiBossInfo:Decode()
    self.info_list = {}
    self.next_refresh_time = MsgAdapter.ReadUInt()
    for i=1,MsgAdapter.ReadInt() do
        local info = {}
        info.npc_id = MsgAdapter.ReadInt()
        info.scene_id = MsgAdapter.ReadInt()
        info.death_num = MsgAdapter.ReadInt()
        self.info_list[info.npc_id] = info
    end
end

--山海降妖信息
SCShanHaiBossRoleInfo = SCShanHaiBossRoleInfo or BaseClass(BaseProtocolStruct)
function SCShanHaiBossRoleInfo:__init()
    self.msg_type = 3372
end

function SCShanHaiBossRoleInfo:Decode()
    self.next_refresh_time = MsgAdapter.ReadUInt()
    self.challenge_count = MsgAdapter.ReadInt()
end

local function ShxyPoker()
    local t = {}
    t.index = MsgAdapter.ReadInt()
    t.role_uid = MsgAdapter.ReadInt()
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    ItemDrawInfo(t)
    return t
end

--山海降妖碎翻牌奖励信息
SCShanHaiBossPokerInfo = SCShanHaiBossPokerInfo or BaseClass(BaseProtocolStruct)
function SCShanHaiBossPokerInfo:__init()
    self.msg_type = 3373
end

function SCShanHaiBossPokerInfo:Decode()
    local info = {}
    info.draw_group_id = MsgAdapter.ReadInt()
    info.timeout = MsgAdapter.ReadUInt()
    info.mypoker = {}
    for i = 1, SIDE_MAX_ROLE_NUM do
        info.mypoker[i] = ShxyPoker()
    end
    self.info = info
end

--山海降妖单个信息
SCShanHaiBossSingleInfo = SCShanHaiBossSingleInfo or BaseClass(BaseProtocolStruct)
function SCShanHaiBossSingleInfo:__init()
    self.msg_type = 3374
end

function SCShanHaiBossSingleInfo:Decode()
    local info = {}
    info.npc_id = MsgAdapter.ReadInt()
    info.scene_id = MsgAdapter.ReadInt()
    info.death_num = MsgAdapter.ReadInt()
    self.info = info
    self.npc_id = info.npc_id
end

--财神聚宝信息
RA_WEALTH_CORNUCOPIA_MAX_ITEM = 20
SCRAWealthCornucopiaInfo = SCRAWealthCornucopiaInfo or BaseClass(BaseProtocolStruct)

function SCRAWealthCornucopiaInfo:__init()
    self.msg_type = 3380
end

function SCRAWealthCornucopiaInfo:Decode()
    self.buy_day_times = {}         -- 当日购买的次数
    self.buy_sum_times = {}         -- 总购买次数
    for i=0,RA_WEALTH_CORNUCOPIA_MAX_ITEM - 1 do
        self.buy_day_times[i] = MsgAdapter.ReadUShort()
    end
    for i=0,RA_WEALTH_CORNUCOPIA_MAX_ITEM - 1 do
        self.buy_sum_times[i] = MsgAdapter.ReadUShort()
    end
    self.flag_num = DataHelper.GetFetchFlagNum(MsgAdapter.IntFlag())
end

--定制宝箱信息
RA_DING_ZHI_BAO_XIANG_MAX_ITEM= 20
SCRADingZhiBaoXiangInfo = SCRADingZhiBaoXiangInfo or BaseClass(BaseProtocolStruct)

function SCRADingZhiBaoXiangInfo:__init()
    self.msg_type = 3390
end

function SCRADingZhiBaoXiangInfo:Decode()
    self.buy_day_times = {}         -- 当日购买的次数
    self.buy_sum_times = {}         -- 总购买次数
    for i=0,RA_DING_ZHI_BAO_XIANG_MAX_ITEM - 1 do
        self.buy_day_times[i] = MsgAdapter.ReadUShort()
    end
    for i=0,RA_DING_ZHI_BAO_XIANG_MAX_ITEM - 1 do
        self.buy_sum_times[i] = MsgAdapter.ReadUShort()
    end
    self.flag_num = DataHelper.GetFetchFlagNum(MsgAdapter.IntFlag())
    self.has_free_reward = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
end

--======================幻界战场=======================
BattlefieldReportStruct = BattlefieldReportStruct or {}

function BattlefieldReportStruct.RoleInfoRead(index)
    local t = {}
    t.index = index or 0
    t.role_id = MsgAdapter.ReadInt()
    t.uid = t.role_id
    t.role_level = MsgAdapter.ReadInt()
    t.level = t.role_level
    t.rank_score = MsgAdapter.ReadInt()
    t.rank_config = BattlefieldData.Instance:GetCurRankConfig(t.rank_score)
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.name = t.role_name
    t.avatar_type = MsgAdapter.ReadShort()
    t.prof_id = MsgAdapter.ReadShort()
    t.prof = math.floor(t.prof_id / 100)
    t.avatar_id = MsgAdapter.ReadShort()
    t.is_leader = MsgAdapter.ReadShort() == 1
    t.is_top_race = MsgAdapter.ReadUChar()
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    t.hurt_list = {}
    for i=1,3 do
        t.hurt_list[i] = MsgAdapter.ReadUInt()
    end
    t.is_self = t.role_id == RoleData.Instance:GetRoleId()
    return t
end

function BattlefieldReportStruct.ReportInfoRead(index)
    local t = {}
    t.index = index or 0
    local info_list = {}
    local hurt_list = {0,0,0}
    for i=1,2 do
        if info_list[i] == nil then
            info_list[i] = {}
        end
        for j=1,3 do
            info_list[i][j] = BattlefieldReportStruct.RoleInfoRead(j)
        end
    end
    t.time = MsgAdapter.ReadUInt()
    t.fight_round = MsgAdapter.ReadUChar()
    t.fight_result = MsgAdapter.ReadUChar()
    t.add_score = MsgAdapter.ReadShort()
    t.add_coin = MsgAdapter.ReadUShort()
    t.is_top_race = MsgAdapter.ReadUChar()
    t.is_protect = MsgAdapter.ReadUChar()
    local left_user_list,right_user_list = BattlefieldReportStruct.GetInfoist(info_list)
    t.left_user_list,t.right_user_list = left_user_list,right_user_list
    return t
end

function BattlefieldReportStruct.RoleOpponentRead(index)
    local t = {}
    t.index = index or 0
    t.uid = MsgAdapter.ReadInt()
    t.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.prof_id = MsgAdapter.ReadInt()
    t.prof = math.floor(t.prof_id / 100)
    t.rank_score = MsgAdapter.ReadInt()
    t.rank_config = BattlefieldData.Instance:GetCurRankConfig(t.rank_score)
    t.avatar_type = MsgAdapter.ReadShort()
    t.avatar_id = MsgAdapter.ReadShort()
    t.is_leader = MsgAdapter.ReadInt() == 1
    t.is_self = t.uid == RoleData.Instance:GetRoleId()
    t.is_top_race = MsgAdapter.ReadInt()
    return t
end

function BattlefieldReportStruct.GetInfoist(info_list)
    local function IsHasSelf(list)
        for k,v in pairs(list) do
            if v.is_self == true then
                return true
            end
        end
    end

    local function GetHurtList(info_list)
        local list = {0,0,0}
        for _,v in ipairs(info_list) do
            for i,v in ipairs(v.hurt_list or {}) do
                list[i] = list[i] + v
            end
        end
        return list
    end
    local left_user_list,right_user_list = {},{}
    for i,info in ipairs(info_list) do
        if #left_user_list == 0 and IsHasSelf(info) then
            left_user_list = info
        else
            right_user_list = info
        end
    end
    for k,v in pairs(left_user_list) do
        v.is_captain = true
        v.max_hurt_list = GetHurtList(left_user_list)
    end
    for k,v in pairs(right_user_list) do
        if v.is_leader == true then
            v.is_captain = true
        end
        v.max_hurt_list = GetHurtList(right_user_list)
    end
    --排序
    local sort_func = DataHelper.WeightSort("index",function(info)
        if info.is_leader == true then
            return -100
        end
        if info.is_self == true then
            return -10
        end
    end)
    table.sort(left_user_list,sort_func)
    table.sort(right_user_list,sort_func)
    return left_user_list,right_user_list
end

function BattlefieldReportStruct.RoleItemInfoRead()
    local t = {}
    t.role_id = MsgAdapter.ReadInt()
    t.battlefield_score = MsgAdapter.ReadInt()
    t.rank_pos = MsgAdapter.ReadInt()
    t.cross_rank_pos = MsgAdapter.ReadInt()
    t.join_time = MsgAdapter.ReadUInt()
    t.is_top_race = MsgAdapter.ReadInt()
    return t
end

--对手信息
SCBattlefieldOpponentResult = SCBattlefieldOpponentResult or BaseClass(BaseProtocolStruct)
function SCBattlefieldOpponentResult:__init()
    self.msg_type = 3303
end

function SCBattlefieldOpponentResult:Decode()
    self.start_time = MsgAdapter.ReadUInt() --开打时间戳
    local info_list = {}
    for i=1,2 do
        if info_list[i] == nil then
            info_list[i] = {}
        end
        for j=1,3 do
            info_list[i][j] = BattlefieldReportStruct.RoleOpponentRead(j)
        end
    end
    local left_user_list,right_user_list = BattlefieldReportStruct.GetInfoist(info_list)
    self.left_user_list,self.right_user_list = left_user_list,right_user_list
    self.info_list = info_list
end

--对战记录 
SCBattlefieldReportInfo = SCBattlefieldReportInfo or BaseClass(BaseProtocolStruct)
function SCBattlefieldReportInfo:__init()
    self.msg_type = 3308
end

function SCBattlefieldReportInfo:Decode()
    self.type = MsgAdapter.ReadInt()--0 全部信息 1 单个添加
    self.count = MsgAdapter.ReadInt()
    self.report_list = {}
    self.report_info = {}
    if self.type == 0 then
        for i=1,self.count do
            self.report_list[i] = BattlefieldReportStruct.ReportInfoRead(i)
        end
    else
        self.report_info = BattlefieldReportStruct.ReportInfoRead()
    end
end

--对战记录 
SCBattlefieldReportInfo = SCBattlefieldReportInfo or BaseClass(BaseProtocolStruct)
function SCBattlefieldReportInfo:__init()
    self.msg_type = 3308
end

function SCBattlefieldReportInfo:Decode()
    self.type = MsgAdapter.ReadInt()--0 全部信息 1 单个添加
    self.count = MsgAdapter.ReadInt()
    self.report_list = {}
    self.report_info = {}
    if self.type == 0 then
        for i=1,self.count do
            self.report_list[i] = BattlefieldReportStruct.ReportInfoRead(i)
        end
    else
        self.report_info = BattlefieldReportStruct.ReportInfoRead()
    end
end

CSBattlefieldRoleItemReq = CSBattlefieldRoleItemReq or BaseClass(BaseProtocolStruct)
function CSBattlefieldRoleItemReq:__init()
    self.msg_type = 3306
end

function CSBattlefieldRoleItemReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    for i,v in ipairs(self.info_list) do
        MsgAdapter.WriteInt(v)
    end
end

SCBattlefieldRoleItemInfo = SCBattlefieldRoleItemInfo or BaseClass(BaseProtocolStruct)
function SCBattlefieldRoleItemInfo:__init()
    self.msg_type = 3307
end

function SCBattlefieldRoleItemInfo:Decode()
    self.info_list = {}
    for i=1,MsgAdapter.ReadInt() do
        local info = BattlefieldReportStruct.RoleItemInfoRead()
        self.info_list[info.role_id] = info
    end
end

--家族灵脉操作请求
CSGuildInbornReq = CSGuildInbornReq or BaseClass(BaseProtocolStruct)
function CSGuildInbornReq:__init()
    self.msg_type = 3360
end

function CSGuildInbornReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

--家族灵脉所有信息下发
SCGuildInbornAllInfo = SCGuildInbornAllInfo or BaseClass(BaseProtocolStruct)
function SCGuildInbornAllInfo:__init()
    self.msg_type = 3361
    self.max_guild_level = 0    --呆过的最高家族等级
    self.use_skill_points = 0   --已使用的技能点数
end

function SCGuildInbornAllInfo:Decode()
    MsgAdapter.ReadShort()
    self.learn_count = MsgAdapter.ReadShort()
    self.learn_list = {}
    for i = 0, self.learn_count - 1 do
        self.learn_list[i] = ReadGuildInbornSkillInfo()
    end
end

--读取GuildInbornSkillInfo
function ReadGuildInbornSkillInfo()
    local t = {}
    t.guild_inborn_type = MsgAdapter.ReadChar() --灵脉技能类型
    t.guild_type_seq = MsgAdapter.ReadChar()   --灵脉技能序号
    t.skill_level = MsgAdapter.ReadShort() --灵脉技能等级
    return t
end

--家族灵脉单个信息下发
SCGuildInbornSingleInfo = SCGuildInbornSingleInfo or BaseClass(BaseProtocolStruct)
function SCGuildInbornSingleInfo:__init()
    self.msg_type = 3362
    self.use_skill_points = 0 --已使用的技能点数
end

function SCGuildInbornSingleInfo:Decode()
    self.learn_info = ReadGuildInbornSkillInfo()
end

--家族灵脉类型信息下发
SCGuildInbornSingleTypeInfo = SCGuildInbornSingleTypeInfo or BaseClass(BaseProtocolStruct)
function SCGuildInbornSingleTypeInfo:__init()
    self.msg_type = 3363
    self.use_skill_points = 0
end

function SCGuildInbornSingleTypeInfo:Decode()
    MsgAdapter.ReadShort()
    self.guild_inborn_type = MsgAdapter.ReadShort() --灵脉技能类型
    self.learn_level_list = {} --学习等级列表
    for i = 0, MAX_GUILD_INBORN_SKILL_TYPE_IN_HAS_SKILL_CUR_NUM - 1 do
        self.learn_level_list[i] = MsgAdapter.ReadShort()
    end
end

--家族灵脉其它信息下发(已删)
SCGuildInbornOtherInfo = SCGuildInbornOtherInfo or BaseClass(BaseProtocolStruct)
function SCGuildInbornOtherInfo:__init()
    self.msg_type = 3364
end

function SCGuildInbornOtherInfo:Decode()
    self.max_guild_level = MsgAdapter.ReadShort()   --呆过的最高家族等级
    self.use_skill_points = MsgAdapter.ReadShort()  --已使用的技能点数
end


