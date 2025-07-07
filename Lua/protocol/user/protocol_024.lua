local function FabaoEquip(id)
    local t =     {
        id = id,
        fabao_index = MsgAdapter.ReadShort(),
        fabao_type = MsgAdapter.ReadShort(),
        fabao_jie = MsgAdapter.ReadShort(),
        fabao_level = MsgAdapter.ReadShort(),
        level_effect = MsgAdapter.ReadShort(),
        reserve_sh = MsgAdapter.ReadShort(),
        cur_exp = MsgAdapter.ReadInt(),
    }
    if t.id == nil and t.fabao_index >= 0 then
        t.id = t.fabao_index
    end
    return t
end
SCFabaoInfo = SCFabaoInfo or BaseClass(BaseProtocolStruct)
function SCFabaoInfo:__init()
    self.msg_type = 2450
end
function SCFabaoInfo:Decode()
    self.week_use_coin_times = MsgAdapter.ReadInt()
    self.fabao_equip_bar = {}
    for i = 1, MAX_FABAO_EQUIPMENT_BAR, 1 do
        self.fabao_equip_bar[i] = {}
        self.fabao_equip_bar[i].id = MsgAdapter.ReadInt()
    end
    self.fabao_list = {}
    for i = 0, MAX_FABAO_COUNT - 1, 1 do--i=id
        self.fabao_list[i] = FabaoEquip(i)
    end
end

CSFabaoReq = CSFabaoReq or BaseClass(BaseProtocolStruct)
function CSFabaoReq:__init()
    self.msg_type = 2451
end
function CSFabaoReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteUShort(self.param1)
    MsgAdapter.WriteUShort(self.param2)
    MsgAdapter.WriteUShort(self.param3)
    MsgAdapter.WriteUShort(self.param4)
    MsgAdapter.WriteUShort(self.param5)
    MsgAdapter.WriteUShort(self.param6)
end

SCFabaoSignInfo = SCFabaoSignInfo or BaseClass(BaseProtocolStruct)
function SCFabaoSignInfo:__init()
    self.msg_type = 2452
end
function SCFabaoSignInfo:Decode()
    self.reason = MsgAdapter.ReadInt()
    self.week_use_coin_times = MsgAdapter.ReadInt()
    self.fabao_equip_bar = {}
    for i = 1, MAX_FABAO_EQUIPMENT_BAR do
        self.fabao_equip_bar[i] = {}
        self.fabao_equip_bar[i].id = MsgAdapter.ReadInt()
    end
    self.fabao_info = FabaoEquip()
end

SCFabaoEquipBarInfo = SCFabaoEquipBarInfo or BaseClass(BaseProtocolStruct)
function SCFabaoEquipBarInfo:__init()
    self.msg_type = 2453
end
function SCFabaoEquipBarInfo:Decode()
    self.fabao_bar = {}
    for i = 1, MAX_FABAO_EQUIPMENT_BAR do
        self.fabao_bar[i] = {}
        self.fabao_bar[i].id = MsgAdapter.ReadInt()
    end
end
local function FabaoDrawItemInfo(index)
    local t = {}
    t.index = index
    local aa = MsgAdapter.ReadUChar()
    t.is_active = (aa ~= 0)
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    t.end_time = MsgAdapter.ReadUInt()
    t.speed_up_time = MsgAdapter.ReadUInt()
    t.material_num = MsgAdapter.ReadInt()
    t.material_item_id = {}
    for i = 1, FA_BAO_DRAW_MATERIAL_MAX_NUM do
        t.material_item_id[i] = MsgAdapter.ReadInt()
    end
    return t
end

--法宝抽奖信息
SCFabaoDrawInfo = SCFabaoDrawInfo or BaseClass(BaseProtocolStruct)
function SCFabaoDrawInfo:__init()
    self.msg_type = 2454
end
function SCFabaoDrawInfo:Decode()
    self.item_info = {}
    for i = 1, FA_BAO_DRAW_ITEM_MAX_NUM do
        self.item_info[i] = FabaoDrawItemInfo(i)
    end
end
--法宝抽奖奖励信息
SCFabaoDrawRewardInfo = SCFabaoDrawRewardInfo or BaseClass(BaseProtocolStruct)
function SCFabaoDrawRewardInfo:__init()
    self.msg_type = 2455
end
function SCFabaoDrawRewardInfo:Decode()
    self.fabao_id = MsgAdapter.ReadShort()
    self.fabao_index = MsgAdapter.ReadShort()
    self.item_id = {}
    for i = 1, FA_BAO_DRAW_MATERIAL_MAX_NUM do
        self.item_id[i] = MsgAdapter.ReadInt()
    end
    self.item_num = {}
    for i = 1, FA_BAO_DRAW_MATERIAL_MAX_NUM do
        self.item_num[i] = MsgAdapter.ReadInt()
    end
end



SCWildBossNotice = SCWildBossNotice or BaseClass(BaseProtocolStruct)
function SCWildBossNotice:__init()
    self.msg_type = 2470
end

function SCWildBossNotice:Decode()
    self.notice_type = MsgAdapter.ReadInt()
end

SCWildBossInfo = SCWildBossInfo or BaseClass(BaseProtocolStruct)
function SCWildBossInfo:__init()
    self.msg_type = 2472
end

function SCWildBossInfo:Decode()
    self.today_times = MsgAdapter.ReadInt()
    self.next_refresh_time = {}
    for i=0,99 do
        self.next_refresh_time[i] = MsgAdapter.ReadUInt()
    end
    self.wild_number_list = {}
    for i=0,99 do
        self.wild_number_list[i] = MsgAdapter.ReadInt()
    end
end

SCWildBossSingleInfo = SCWildBossSingleInfo or BaseClass(BaseProtocolStruct)
function SCWildBossSingleInfo:__init()
    self.msg_type = 2477
end

function SCWildBossSingleInfo:Decode()
    self.index = MsgAdapter.ReadInt()
    self.next_refresh_time = MsgAdapter.ReadUInt()
    self.wild_number = MsgAdapter.ReadInt()
end

CSWildBossReq = CSWildBossReq or BaseClass(BaseProtocolStruct)
function CSWildBossReq:__init()
    self.msg_type = 2473
end

function CSWildBossReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type) --1 下发刷新时间和挑战次数 2 请求怪的位置
    MsgAdapter.WriteInt(self.param) 
end

SCShopAllInfo = SCShopAllInfo or BaseClass(BaseProtocolStruct)
function SCShopAllInfo:__init()
    self.msg_type = 2490
end

SHOP_ITEM_MAX_COUNT = 256

function SCShopAllInfo:Decode()
    self.buy_count_list = {}
    for i = 0, SHOP_ITEM_MAX_COUNT - 1 do
        self.buy_count_list[i] = MsgAdapter.ReadUChar()
    end
end

SCShopSingleInfo = SCShopSingleInfo or BaseClass(BaseProtocolStruct)
function SCShopSingleInfo:__init()
    self.msg_type = 2491
end

function SCShopSingleInfo:Decode()
    self.index = MsgAdapter.ReadInt()
    self.buy_count = MsgAdapter.ReadInt()
end

CSShopBuyReq = CSShopBuyReq or BaseClass(BaseProtocolStruct)
function CSShopBuyReq:__init()
    self.msg_type = 2492
end

function CSShopBuyReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.index)
    MsgAdapter.WriteInt(self.num)
    MsgAdapter.WriteInt(self.is_rand_list)
end

CSFastPurchase = CSFastPurchase or BaseClass(BaseProtocolStruct)
function CSFastPurchase:__init()
    self.msg_type = 2493
end
function CSFastPurchase:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.item_id)
    MsgAdapter.WriteUShort(self.buy_num)
end

-- 水晶副本信息
SCCrystalFbInfo = SCCrystalFbInfo or BaseClass(BaseProtocolStruct)
function SCCrystalFbInfo:__init()
    self.msg_type = 2460
end

function SCCrystalFbInfo:Decode()
    self.xi_you_fb_play_times = {}
    for i = 0, FightConfig.CRYSTAL_FB_XI_YOU_MAX_LEVEL - 1 do
        self.xi_you_fb_play_times[i] = MsgAdapter.ReadUChar()
    end
    self.normal_record_list = {}
    self.normal_record_bt_list = {}
    self.normal_record_num = MsgAdapter.ReadInt()
    for i = 1, self.normal_record_num do
        local fb_seq = MsgAdapter.ReadShort()
        local challenge_times = MsgAdapter.ReadShort()
        local buy_times = MsgAdapter.ReadShort()
        MsgAdapter.ReadShort()
        self.normal_record_list[fb_seq] = challenge_times
        self.normal_record_bt_list[fb_seq] = buy_times
    end
end

-- 这不是战斗buff
SCBuffInfo = SCBuffInfo or BaseClass(BaseProtocolStruct)

local function BuffBaseInfo()
    local buff = {}
    buff.source = MsgAdapter.ReadInt()
    buff.type = MsgAdapter.ReadInt()
    buff.begin_time = MsgAdapter.ReadUInt()
    buff.end_time = MsgAdapter.ReadUInt()
    buff.scene_buff = MsgAdapter.ReadInt() --不等于0就是场景buff值=场景类型
    local ATTR_MAX = 4
    local count = MsgAdapter.ReadInt()
    buff.attrs = {}
    for i = 1, ATTR_MAX do
        if i <= count then
            buff.attrs[i] = {}
            buff.attrs[i].type = MsgAdapter.ReadChar()
        else
            MsgAdapter.ReadChar()
        end
    end
    for i = 1, ATTR_MAX do
        if i <= count then
            buff.attrs[i].val = MsgAdapter.ReadInt()
        else
            MsgAdapter.ReadInt()
        end
    end           
    buff.up_per = MsgAdapter.ReadInt()
    buff.change_per = MsgAdapter.ReadInt()
    return buff
end

function SCBuffInfo:__init()
    self.msg_type = 2480
end

function SCBuffInfo:Decode()
    self.buff_list = {}
    local count = MsgAdapter.ReadInt()
    for i=1,count do
        self.buff_list[i] = BuffBaseInfo()
    end
end

--与2464协议合并了，此协议废弃
-- -- 水晶副本场景信息
-- SCCrystalFbSceneInfo = SCCrystalFbSceneInfo or BaseClass(BaseProtocolStruct)
-- function SCCrystalFbSceneInfo:__init()
--     self.msg_type = 2461
-- end

-- function SCCrystalFbSceneInfo:Decode()
--     local t = {}
--     t.level_seq = MsgAdapter.ReadInt()
--     t.layer = MsgAdapter.ReadInt()

--     t.box_num = MsgAdapter.ReadInt()
--     t.box_num_max = MsgAdapter.ReadInt()
--     t.box_num_r = t.box_num_max - t.box_num

--     t.monster_num = MsgAdapter.ReadInt()
--     t.monster_num_max = MsgAdapter.ReadInt()
--     t.monster_num_r = t.monster_num_max - t.monster_num

--     t.npc_num_r = MsgAdapter.ReadInt()
--     t.npc_num_max = MsgAdapter.ReadInt()
--     t.npc_num = t.npc_num_max - t.npc_num_r

--     t.bonfire_num = MsgAdapter.ReadInt()
--     t.bonfire_num_max = MsgAdapter.ReadInt()
--     t.bonfire_num_r = t.bonfire_num_max - t.bonfire_num

--     t.coin_num = MsgAdapter.ReadInt()
--     t.coin_num_max = MsgAdapter.ReadInt()
--     t.coin_num_r = t.coin_num_max - t.coin_num
    
--     t.gather_num = MsgAdapter.ReadInt()
--     t.gather_num_max = MsgAdapter.ReadInt()
--     t.gather_num_r = t.gather_num_max - t.gather_num

--     t.num = t.box_num + t.monster_num + t.npc_num + t.bonfire_num + t.coin_num + t.gather_num
--     t.num_max = t.box_num_max + t.monster_num_max + t.npc_num_max + t.bonfire_num_max + t.coin_num_max + t.gather_num_max
--     t.num_r = t.box_num_r + t.monster_num_r + t.npc_num_r + t.bonfire_num_r + t.coin_num_r + t.gather_num_r
--     self.fight_crystal_info = t
-- end

CSCrystalFbReq = CSCrystalFbReq or BaseClass(BaseProtocolStruct)
function CSCrystalFbReq:__init()
    self.msg_type = 2462
end

function CSCrystalFbReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
end

--水晶副本次数
SCCrystalFbUseItemInfo = SCCrystalFbUseItemInfo or BaseClass(BaseProtocolStruct)
function SCCrystalFbUseItemInfo:__init()
    self.msg_type = 2463
end

function SCCrystalFbUseItemInfo:Decode()
    self.use_info = {}
    local count = MsgAdapter.ReadInt()
    for i=1,count do
        local info = {}
        info.use_type = MsgAdapter.ReadInt()
        info.use_times = MsgAdapter.ReadInt()
        self.use_info[i] = info
    end
end

-- 水晶副本各个对象信息
SCCrystalFbObjInfo = SCCrystalFbObjInfo or BaseClass(BaseProtocolStruct)
function SCCrystalFbObjInfo:__init()
    self.msg_type = 2464
end

function SCCrystalFbObjInfo:Decode()
    local t = {}

    t.level_seq = MsgAdapter.ReadInt()
    t.layer = MsgAdapter.ReadInt()
    t.scene_id = MsgAdapter.ReadInt()

    t.box_num = MsgAdapter.ReadInt()
    t.box_num_max = MsgAdapter.ReadInt()
    t.box_num_r = t.box_num_max - t.box_num

    t.monster_num = MsgAdapter.ReadInt()
    t.monster_num_max = MsgAdapter.ReadInt()
    t.monster_num_r = t.monster_num_max - t.monster_num

    t.npc_num_r = MsgAdapter.ReadInt()
    t.npc_num_max = MsgAdapter.ReadInt()
    t.npc_num = t.npc_num_max - t.npc_num_r

    t.bonfire_num = MsgAdapter.ReadInt()
    t.bonfire_num_max = MsgAdapter.ReadInt()
    t.bonfire_num_r = t.bonfire_num_max - t.bonfire_num

    t.coin_num = MsgAdapter.ReadInt()
    t.coin_num_max = MsgAdapter.ReadInt()
    t.coin_num_r = t.coin_num_max - t.coin_num
    
    t.gather_num = MsgAdapter.ReadInt()
    t.gather_num_max = MsgAdapter.ReadInt()
    t.gather_num_r = t.gather_num_max - t.gather_num

    t.num = t.box_num + t.monster_num --[[ + t.npc_num + t.bonfire_num ]] + t.coin_num --[[ + t.gather_num ]]
    t.num_max = t.box_num_max + t.monster_num_max --[[ + t.npc_num_max + t.bonfire_num_max ]] + t.coin_num_max --[[ + t.gather_num_max ]]
    t.num_r = t.box_num_r + t.monster_num_r --[[ + t.npc_num_r + t.bonfire_num_r ]] + t.coin_num_r --[[ + t.gather_num_r ]]
    
    t.is_finish = true

    self.fight_crystal_info = t
    local obj_list = {}
    obj_list.obj_count = MsgAdapter.ReadInt()
    obj_list.obj_list = {}
    for i = 1, obj_list.obj_count do
        local obj_type = MsgAdapter.ReadInt()
        if FightConfig.CrystalObjType.Boss == obj_type then t.is_finish = false end
        obj_list.obj_list[i] = {
            obj_type = obj_type,
            npc_id = MsgAdapter.ReadInt(),
            pos_x = MsgAdapter.ReadShort(),
            pos_y = MsgAdapter.ReadShort(),
        }
        obj_list.obj_list[i].icon = FightConfig.CrystalIconsS[obj_type]
    end

    self.obj_list = obj_list
end

-- 水晶副本各个对象信息
SCCrystalFbAllObjInfo = SCCrystalFbAllObjInfo or BaseClass(BaseProtocolStruct)
function SCCrystalFbAllObjInfo:__init()
    self.msg_type = 2459
end

function SCCrystalFbAllObjInfo:Decode()
    local count = MsgAdapter.ReadInt()
    self.obj_list = {}
    for i = 1, count do
        local obj = {
            index = i,
            obj_id = MsgAdapter.ReadInt(),
            obj_type = MsgAdapter.ReadInt(),
            npc_id = MsgAdapter.ReadInt(),
            pos_x = MsgAdapter.ReadShort(),
            pos_y = MsgAdapter.ReadShort(),
        }
        obj.is_get = 65535 == obj.obj_id
        obj.sort_type = FightConfig.CrystalMenuObjShow[obj.obj_type]
        obj.icon = FightConfig.CrystalIconsS[obj.obj_type]
        if obj.obj_type ~= SceneObjTypeS.FBDoor then
            local co = NpcHelper.NpcCo(obj.npc_id)
            if co then obj.name = co.name end
        end
        self.obj_list[i] = obj
    end
end

-- 引导副本场景信息
SCGuildFbSceneInfo = SCGuildFbSceneInfo or BaseClass(BaseProtocolStruct)
function SCGuildFbSceneInfo:__init()
    self.msg_type = 2900
end

function SCGuildFbSceneInfo:Decode()
    local t = {}
    t.layer = MsgAdapter.ReadInt()
    t.box_num = MsgAdapter.ReadInt()
    t.monster_num = MsgAdapter.ReadInt()
    self.guild_fb_info = t
end

-- 水晶副本体力信息
SCCrystalFbEnergyInfo = SCCrystalFbEnergyInfo or BaseClass(BaseProtocolStruct)
function SCCrystalFbEnergyInfo:__init()
    self.msg_type = 2479
end

function SCCrystalFbEnergyInfo:Decode()
    local t = {
        role_enter_energy = MsgAdapter.ReadInt(),       -- 现有体力
        buy_times = MsgAdapter.ReadShort(),             -- 花费元宝购买体力次数
        reserve_sh = MsgAdapter.ReadShort(),
        next_energy_reply_tamp = MsgAdapter.ReadUInt(), -- 下一次体力恢复时间戳 0无需恢复体力
        first_pass_flag = {},
        first_pass_reward = {},
    }
    for i = 0, 4 do
        t.first_pass_flag[i] = MsgAdapter.ReadUInt()    -- 是否已经通关过(二进制)
    end
    for i = 0, 4 do
        t.first_pass_reward[i] = MsgAdapter.ReadUInt()    -- 是否已经通关过(二进制)
    end

    t.new_jingying_play_times = MsgAdapter.ReadInt()    -- 精英通关次数
    t.use_xiyou_play_times = MsgAdapter.ReadInt()       -- 稀有通关次数
    t.new_xiyou_play_times = MsgAdapter.ReadInt()       -- 稀有总次数

    self.info = t
end

-- 水晶副本扫荡
CSCrystalFbFastPassInfo = CSCrystalFbFastPassInfo or BaseClass(BaseProtocolStruct)
function CSCrystalFbFastPassInfo:__init()
    self.msg_type = 2478
end

function CSCrystalFbFastPassInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.fb_seq)
    MsgAdapter.WriteInt(self.times)
end

--======================元宝商城======================
CSGoldShopBuyReq = CSGoldShopBuyReq or BaseClass(BaseProtocolStruct)
function CSGoldShopBuyReq:__init()
    self.msg_type = 2494
end

function CSGoldShopBuyReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.index)
    MsgAdapter.WriteShort(self.num)
    MsgAdapter.WriteShort(self.is_another_buy)
end

--元宝商城 所有信息
SCGoldShopAllInfo = SCGoldShopAllInfo or BaseClass(BaseProtocolStruct)
function SCGoldShopAllInfo:__init()
    self.msg_type = 2495
end

function SCGoldShopAllInfo:Decode()
    self.shop_buy_count = {}
    for i=0,SHOP_BUY_MAX_COUNT - 1 do
        self.shop_buy_count[i] = MsgAdapter.ReadUChar()
    end
end

--元宝商城 单个信息
SCGoldShopSingleInfo = SCGoldShopSingleInfo or BaseClass(BaseProtocolStruct)
function SCGoldShopSingleInfo:__init()
    self.msg_type = 2496
end

function SCGoldShopSingleInfo:Decode()
    self.index = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
end

--======================兑换======================
CSExchangeReq = CSExchangeReq or BaseClass(BaseProtocolStruct)
function CSExchangeReq:__init()
    self.msg_type = 2497
end

function CSExchangeReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.seq)
    MsgAdapter.WriteInt(self.num)
end

--兑换所有信息
SCExchangeAllInfo = SCExchangeAllInfo or BaseClass(BaseProtocolStruct)
function SCExchangeAllInfo:__init()
    self.msg_type = 2498
end

function SCExchangeAllInfo:Decode()
    self.buy_count = {}
    for i=1,MsgAdapter.ReadInt() do
        self.buy_count[MsgAdapter.ReadUShort()] = MsgAdapter.ReadUShort()
    end
end

--兑换 单个信息
SCExchangeSingleInfo = SCExchangeSingleInfo or BaseClass(BaseProtocolStruct)
function SCExchangeSingleInfo:__init()
    self.msg_type = 2499
end

function SCExchangeSingleInfo:Decode()
    self.seq = MsgAdapter.ReadUShort()
    self.count = MsgAdapter.ReadUShort()
end

--进阶副本
SCAdvanceFbObjInfo = SCAdvanceFbObjInfo or BaseClass(BaseProtocolStruct)
function SCAdvanceFbObjInfo:__init()
    self.msg_type = 2465
end

function SCAdvanceFbObjInfo:Decode()
    self.obj_list = {}
    for i=1,MsgAdapter.ReadInt() do
        local info = {}
        info.obj_type = MsgAdapter.ReadShort()--0 boss 1 传送门
        info.obj_id = MsgAdapter.ReadUShort()
        info.pos_x = MsgAdapter.ReadShort()
        info.pos_y = MsgAdapter.ReadShort()
        self.obj_list[i] = info
    end
end

SCAdvanceFbTaskInfo = SCAdvanceFbTaskInfo or BaseClass(BaseProtocolStruct)
function SCAdvanceFbTaskInfo:__init()
    self.msg_type = 2466
end

function SCAdvanceFbTaskInfo:Decode()
    local info = {}
    info.task_id = MsgAdapter.ReadInt()
    info.progress = MsgAdapter.ReadInt()
    self.task_info = info
end

-- 四象幻境副本
SCFantasyFbSceneInfo = SCFantasyFbSceneInfo or BaseClass(BaseProtocolStruct)
function SCFantasyFbSceneInfo:__init()
    self.msg_type = 2467
end

function SCFantasyFbSceneInfo:Decode()
    local t = {}
    t.level_seq = MsgAdapter.ReadInt()
    t.layer = MsgAdapter.ReadInt()

    t.kill_boss_flag = MsgAdapter.ReadInt()

    t.box_num = MsgAdapter.ReadInt()
    t.box_num_max = MsgAdapter.ReadInt()
    t.box_num_r = t.box_num_max - t.box_num

    t.gather_num = MsgAdapter.ReadInt()
    t.gather_num_max = MsgAdapter.ReadInt()
    t.gather_num_r = t.gather_num_max - t.gather_num

    self.fight_fantasy_info = t
end

SCFantasyFbObjInfo = SCFantasyFbObjInfo or BaseClass(BaseProtocolStruct)
function SCFantasyFbObjInfo:__init()
    self.msg_type = 2468
end

function SCFantasyFbObjInfo:Decode()
    self.obj_count = MsgAdapter.ReadInt()
    self.obj_list = {}
    for i = 1, self.obj_count do
        local obj_type = MsgAdapter.ReadInt()
        self.obj_list[i] = {
            obj_type = obj_type,
            npc_id = MsgAdapter.ReadInt(),
            pos_x = MsgAdapter.ReadShort(),
            pos_y = MsgAdapter.ReadShort(),
        }
        self.obj_list[i].icon = FightConfig.CrystalIconsS[obj_type]
    end
end

SCFantasyFBRewardInfo = SCFantasyFBRewardInfo or BaseClass(BaseProtocolStruct)
function SCFantasyFBRewardInfo:__init()
    self.msg_type = 2469
end

function SCFantasyFBRewardInfo:Decode()
    
    self.op_type = MsgAdapter.ReadInt()
    self.is_fetch_reward = MsgAdapter.ReadInt()
    self.reward_list = {}
    if self.op_type == 0 then return end 
    self.reward_count = MsgAdapter.ReadInt()
    for i = 1,self.reward_count do 
        local item_vo = {}
        item_vo.item_id = MsgAdapter.ReadUShort()
        item_vo.is_bind = MsgAdapter.ReadShort() ~= 0
        item_vo.num = MsgAdapter.ReadInt()

        table.insert( self.reward_list, item_vo )
    end 
end

--废弃
SCWildBossPosiInfo = SCWildBossPosiInfo or BaseClass(BaseProtocolStruct)
function SCWildBossPosiInfo:__init()
    self.msg_type = 2474
end

function SCWildBossPosiInfo:Decode()
    self.pos = {
        x = MsgAdapter.ReadInt(),
        y = MsgAdapter.ReadInt()
    }
    self.monster_seq = MsgAdapter.ReadInt()
end

WildBossPokerStruct = {}

function WildBossPokerStruct.WildBossPoker()
    local t = {}
    t.index = MsgAdapter.ReadInt()
    t.role_uid = MsgAdapter.ReadInt()
    MsgAdapter.ReadInt()
    t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    t.luck_coin = MsgAdapter.ReadInt()
    ItemDrawInfo(t)
    return t
end

SCWildBossPokerInfo = SCWildBossPokerInfo or BaseClass(BaseProtocolStruct)
function SCWildBossPokerInfo:__init()
    self.msg_type = 2475
end

function SCWildBossPokerInfo:Decode()
    local info = {}
    info.draw_group_id = MsgAdapter.ReadInt()
    info.timeout = MsgAdapter.ReadUInt()
    info.mypoker = {}
    for i = 1, SIDE_MAX_ROLE_NUM do
        info.mypoker[i] = WildBossPokerStruct.WildBossPoker()
    end
    self.info = info
end

SCFaBaoQiLingInfo = SCFaBaoQiLingInfo or BaseClass(BaseProtocolStruct)
function SCFaBaoQiLingInfo:__init()
    self.msg_type = 2456
end

function SCFaBaoQiLingInfo:Decode()
    self.put_on_list = {}     --穿戴列表 值为-1或者器灵id
    self.qi_ling_list = {}    --器灵列表
    self.yuling_num=0         --御灵数量
    self.put_on_index_list={} --穿戴列表index
    self.active_num =0        --激活数量
    for i = 1, FA_BAO_QI_LING_PUT_ON_MAX_NUM do
        local qiling_id=MsgAdapter.ReadInt()
        if qiling_id ~= -1 then
            local fabao_id = Config.talisman_cfg_auto.fabao_qiling[qiling_id + 1].fabao_id
            self. put_on_list[fabao_id] = qiling_id
            self.yuling_num=self.yuling_num+1
            self.put_on_index_list[qiling_id] = i - 1
        end
    end
    for i = 0, FA_BAO_QI_LING_MAX_NUM - 1 do
        local info = {}
        info.id = i
        info.level = MsgAdapter.ReadShort()
        info.level =FabaoData.Instance:GetRealLevel( info.id,info.level)
        info.isPutOn=false
        local re = MsgAdapter.ReadShort()
        if Config.talisman_cfg_auto.fabao_qiling[i+1].is_show == 1 then
            self.qi_ling_list[i]=info
        end
        if info.level >0 then
            self.active_num=self.active_num+1
        end
    end
    for _,v in pairs(self.put_on_list) do
        self.qi_ling_list[v] .isPutOn=true
    end
end


