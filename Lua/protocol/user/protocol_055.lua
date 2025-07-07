
-- 正在祈祷的神坛
local function MsgOccupiedPrayingAltar()
    local t = {}
    t.row = MsgAdapter.ReadShort()  -- 行
    t.column = MsgAdapter.ReadShort()  -- 列
    t.user_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)  -- 角色名
    t.server_id = MsgAdapter.ReadShort()  -- 角色服务器ID
    t.head_id = MsgAdapter.ReadShort()   -- 头像
    t.avatar_type = MsgAdapter.ReadShort()   -- 模型
    t.faction = MsgAdapter.ReadShort()  -- 阵营[0,4)
    t.pray_end_timestamp = MsgAdapter.ReadUInt()  -- 祈祷结束时间戳
    t.occupy_uid = MsgAdapter.ReadInt()  -- 占领者主角id
    t.occupy_plat_type = MsgAdapter.ReadInt()  -- 占领者渠道
    t.prayer_uid = MsgAdapter.ReadInt()  -- 祈祷者的主角id
    t.prayer_plat_type = MsgAdapter.ReadInt()  -- 祈祷者的渠道
    t.being_attack = MsgAdapter.ReadShort()  -- 正在被攻打
    local reserve_sh = MsgAdapter.ReadShort()  -- 保留字
    t.can_occupy_timestamp = MsgAdapter.ReadUInt()  -- 可以占领的时间戳（现在占领完有5分钟CD）
    return t
end
-- 被占领的格点（只含空地和未被祈祷的神坛）和正在被攻打的格点
local function MsgOccupyGrid()
    local t = {}
	t.row = MsgAdapter.ReadShort()        -- 行
	t.column = MsgAdapter.ReadShort()     -- 列
	t.occupied_faction = MsgAdapter.ReadShort()	-- 占领者的阵营[0,4) 或 -1表示未被占领
	t.being_attack = MsgAdapter.ReadShort()    -- 正在被攻打
    t.occupy_uid = MsgAdapter.ReadInt()  -- 占领者主角id
    t.occupy_plat_type = MsgAdapter.ReadInt()  -- 占领者渠道
    t.can_occupy_timestamp = MsgAdapter.ReadUInt()  -- 空地可以占领的时间戳（现在占领完有5分钟CD）
    return t
end 

local function MsgOnMapUserInfo()
    local t = {}
    t.uid = MsgAdapter.ReadInt() -- 角色id
    t.plat_type = MsgAdapter.ReadInt()  -- 渠道号
    t.user_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN) -- 角色名
	t.server_id = MsgAdapter.ReadShort() -- 服务器ID
	t.row = MsgAdapter.ReadShort() -- 行
	t.column = MsgAdapter.ReadShort() -- 列
	t.faction = MsgAdapter.ReadShort() -- 玩家所属阵营[0,4)
	t.avatar_type = MsgAdapter.ReadShort() -- 模型
	t.profession = MsgAdapter.ReadInt() -- 职业

    return t
end

local function MsgGodSpaceReportInfo()
    local t = {}
    t.timestamp = MsgAdapter.ReadUInt() --战报时间戳
    t.essence_num = MsgAdapter.ReadInt() -- 获得精华数量
    t.report_type = MsgAdapter.ReadChar() -- 0 正常结束祈祷 1：被占领结束祈祷 2 神坛放弃结束祷告
    
    t.is_read = MsgAdapter.ReadChar() -- 是否已阅读
    t.essence_item_num = MsgAdapter.ReadShort() --获得精华物品数量
    if t.report_type > 0 then 
        t.opponent_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    else 
        t.opponent_name = ""
    end 

    t.essence_list = {} -- 获得精华物品列表
    for i = 1,t.essence_item_num do 
        local vo = {}
        vo.item_id = MsgAdapter:ReadUShort()
        vo.num = MsgAdapter:ReadShort()

        table.insert(t.essence_list,vo)
    end 
    
    return t
end

local function MsgGodSpaceFactionUser()
    local t = {}
    t.capability = MsgAdapter.ReadInt() -- 战力
    t.exploits = MsgAdapter.ReadInt() -- 战功
    t.headshot_id = MsgAdapter.ReadInt() -- 头像
    t.avatar_type = MsgAdapter.ReadInt() -- 模型
    t.server_id = MsgAdapter.ReadInt() -- 服务器id
    t.user_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN) -- 玩家名
    return t
end     

-- 5501 神域基础信息（登录自动下发）
SCGodSpaceBaseInfo = SCGodSpaceBaseInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceBaseInfo:__init()
    self.msg_type = 5501
end
function SCGodSpaceBaseInfo:Decode()
    self.season_index = MsgAdapter.ReadShort()-- 赛季号 从1开始
    self.cur_status = MsgAdapter.ReadShort()-- 当前状态   1和2都是准备阶段，3:外域 4:内域 5:结算 6:休赛 
    self.is_open = MsgAdapter.ReadShort() -- 是否开启神域 0否 1是
    self.bigcross_available = MsgAdapter.ReadShort() -- 大跨服是否开了
    self.ourter_space_timestamp = MsgAdapter.ReadUInt()--外域开启时间（神域开启时间）
    self.inner_space_timestamp = MsgAdapter.ReadUInt()--内域开启时间
    self.season_end_timestamp = MsgAdapter.ReadUInt()--赛季结束时间
    LogDG("外域时间", self.ourter_space_timestamp, os.date("%c", self.ourter_space_timestamp))
    LogDG("内域时间", self.inner_space_timestamp, os.date("%c", self.inner_space_timestamp))
    LogDG("赛季结束", self.season_end_timestamp, os.date("%c", self.season_end_timestamp))
end

-- 5503 玩家在神域中的信息
-- 我自己
SCGodSpaceUserInfo = SCGodSpaceUserInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserInfo:__init()
    self.msg_type = 5503
end

function SCGodSpaceUserInfo:Decode()
    self.plat_type = MsgAdapter.ReadInt()  -- 玩家的渠道号 
    self.row = MsgAdapter.ReadShort()      -- 玩家位置（行)
    self.column = MsgAdapter.ReadShort()   -- 玩家位置（列)
    self.faction = MsgAdapter.ReadShort()  -- 玩家属于阵营[0,4)
    local reserve_sh = MsgAdapter.ReadShort()
    self.action_point = MsgAdapter.ReadInt() -- 行动力
    self.today_gold_buy_ap_times = MsgAdapter.ReadShort() -- 今天已使用元宝购买行动力次数
    self.today_jade_buy_ap_times = MsgAdapter.ReadShort() -- 今天已使用灵玉购买行动力次数
    self.ap_item_num = {}
    -- 本数组定长
    -- 0 小行动丹 1 普通行动丹 2 大行动丹
    for i = 0,2 do

        self.ap_item_num[i] = MsgAdapter.ReadShort()
    end 
    local reserve_sh_2 = MsgAdapter.ReadShort()
    self.next_recover_ap_timestamp = MsgAdapter.ReadUInt() -- 下次恢复行动力时间戳
    self.recover_all_ap_timestamp = MsgAdapter.ReadUInt() -- 恢复全部行动力时间戳
    -- self.role_uuid = MsgAdapter.ReadLL() --主角的uuid
    self.role_id = MsgAdapter.ReadInt()
    self.plat_type = MsgAdapter.ReadInt()
    self.role_uuid = int64.new(self.role_id, self.plat_type)
    LogDG("self.role_id ", self.role_id)
    LogDG("self.plat_type ", self.plat_type)
    LogDG("self.role_uuid ", self.role_uuid)
end

-- 5502 玩家请求神域操作
CSGodSpaceOperate = CSGodSpaceOperate or BaseClass(BaseProtocolStruct)
function CSGodSpaceOperate:__init()
    self.msg_type = 5502
end

-- OPERATE_TYPE
-- LANDS_BETWEEN_OPERATE_TYPE 往这里加！
function CSGodSpaceOperate:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.oper_type)
    MsgAdapter.WriteInt(self.param_1)
    MsgAdapter.WriteInt(self.param_2)
    MsgAdapter.WriteInt(self.param_3)
    MsgAdapter.WriteInt(self.param_4)
end

-- 5506 角色移动到某格子上
-- 可以不是我自己
SCGodSpaceUserMoveToGrid = SCGodSpaceUserMoveToGrid or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserMoveToGrid:__init()
    self.msg_type = 5506
end

function SCGodSpaceUserMoveToGrid:Decode()
    self.uid = MsgAdapter.ReadInt()  
    self.plat_type = MsgAdapter.ReadInt()  -- plat_type是渠道号，因为以后可能有其他渠道的一起大跨服，预先做处理

    self.row = MsgAdapter.ReadShort()  -- 行
    self.column = MsgAdapter.ReadShort() -- 列
end

-- 5507 地图上格点信息变化通知
SCGodSpaceGridInfoChanged = SCGodSpaceGridInfoChanged or BaseClass(BaseProtocolStruct)
function SCGodSpaceGridInfoChanged:__init()
    self.msg_type = 5507
end

function SCGodSpaceGridInfoChanged:Decode()
    self.info = MsgOccupyGrid()
end

-- 5508 地图上神坛祈祷信息变化通知
SCGodSpaceAltarPrayInfoChanged = SCGodSpaceAltarPrayInfoChanged or BaseClass(BaseProtocolStruct)
function SCGodSpaceAltarPrayInfoChanged:__init()
    self.msg_type = 5508
end

function SCGodSpaceAltarPrayInfoChanged:Decode()
    self.info = MsgOccupiedPrayingAltar()
end

-- 5509 神坛守军信息下发
SCGodSpaceAltarDefenderInfo = SCGodSpaceAltarDefenderInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceAltarDefenderInfo:__init()
    self.msg_type = 5509
end

function SCGodSpaceAltarDefenderInfo:Decode()
    self.defender_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN) -- 守军名字
    self.server_id = MsgAdapter.ReadShort() -- 服务器ID（当守军是怪物时，值为0)
    self.avatar_type = MsgAdapter.ReadShort() -- 模型类型（当守军是怪物时，值为0)
    self.profession = MsgAdapter.ReadInt() -- 职业 （当守军是怪物时，值为0)
    self.capability = MsgAdapter.ReadInt() -- 战力评分
    self.is_monster = MsgAdapter.ReadShort() -- 是否怪物 (0:非怪物  1:是怪物)
    local reserve_sh = MsgAdapter.ReadShort() -- 保留字
    self.monster_group_id = MsgAdapter.ReadInt() -- 怪物组ID（当守军是怪物时， 用于客户端显示怪物头目的模型）
end

-- 5510 阵营分配
SCGodSpaceUserAssignFaction = SCGodSpaceUserAssignFaction or  BaseClass(BaseProtocolStruct)
function SCGodSpaceUserAssignFaction:__init()
    self.msg_type = 5510
end

-- 0 水 1 火 2 风 3 地
function SCGodSpaceUserAssignFaction:Decode()
    self.faction =  MsgAdapter.ReadInt()
end

-- 5504 神域地图信息
SCGodSpaceMapInfo = SCGodSpaceMapInfo or  BaseClass(BaseProtocolStruct)
function SCGodSpaceMapInfo:__init()
    self.msg_type = 5504
end

function SCGodSpaceMapInfo:Decode()
    self.praying_altars = {}
    self.occupy_grids = {}
    
    self.pray_count = MsgAdapter.ReadShort()
    self.occupy_count = MsgAdapter.ReadShort()
    for i = 1,self.pray_count do 
        self.praying_altars[i] = MsgOccupiedPrayingAltar()
    end 

    
    for i = 1,self.occupy_count do 
        self.occupy_grids[i] = MsgOccupyGrid()
    end 
end

-- 5505 神域地图信息
-- 该角色信息不包含你自己
SCGodSpaceOtherUsersInfo = SCGodSpaceOtherUsersInfo or  BaseClass(BaseProtocolStruct)
function SCGodSpaceOtherUsersInfo:__init()
    self.msg_type = 5505
end

function SCGodSpaceOtherUsersInfo:Decode()
    self.users = {}
    self.user_count = MsgAdapter.ReadShort()
    for i = 1,self.user_count do 
        self.users[i] = MsgOnMapUserInfo()
    end 
end

-- 5511 精华简要消息
SCGodSpaceUserEssemceBriefInfo = SCGodSpaceUserEssemceBriefInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserEssemceBriefInfo:__init()
    self.msg_type = 5511
end

function SCGodSpaceUserEssemceBriefInfo:Decode()
    self.guaji_essence_per_minute = MsgAdapter.ReadInt() -- 挂机产出效率
    self.altar_essence_per_minute = MsgAdapter.ReadInt() -- 神坛产出效率
    self.unfetch_essence_num = MsgAdapter.ReadInt()      -- 未领取的精华数量
    self.unfetch_essence_item_num = MsgAdapter.ReadInt() -- 未领取精华物品数量
    self.unfetch_guaji_essence = MsgAdapter.ReadInt()    -- 未领取的挂机物品数量
end

-- 5512 新战报到达
SCGodSpaceUserNewReportArrived = SCGodSpaceUserNewReportArrived or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserNewReportArrived:__init()
    self.msg_type = 5512
end

function SCGodSpaceUserNewReportArrived:Decode()
    self.info = MsgGodSpaceReportInfo()
end

-- 5513 战报列表（已排序）
SCGodSpaceUserReportList = SCGodSpaceUserReportList or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserReportList:__init()
    self.msg_type = 5513
end

function SCGodSpaceUserReportList:Decode()
    self.report_num = MsgAdapter.ReadInt()
    self.report_list = {}
    for i = 1,self.report_num do 
        local vo = MsgGodSpaceReportInfo()
        table.insert(self.report_list,vo)
    end 
end

-- 5515 收到时显示赛季见面礼界面
SCGodSpaceUserShowSeasonGiftUI = SCGodSpaceUserShowSeasonGiftUI or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserShowSeasonGiftUI:__init()
    self.msg_type = 5515
end

function SCGodSpaceUserShowSeasonGiftUI:Decode()
end 

-- 5516 神域同阵营玩家列表（已排序）
SCGodSpaceUserFactionUserList = SCGodSpaceUserFactionUserList or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserFactionUserList:__init()
    self.msg_type = 5516
end

function SCGodSpaceUserFactionUserList:Decode()
    self.remain_num = MsgAdapter.ReadShort() -- 剩余未下发数量
    self.user_num = MsgAdapter.ReadShort()  -- 这次下发数量
    self.users = {}
    for i = 1,self.user_num do 
        local vo = MsgGodSpaceFactionUser()
        table.insert(self.users,vo)
    end 
end 

-- 5517 神域阵营基础信息 （重新请求阵营列表时会下发）
SCGodSpaceUserFactionBasicInfo = SCGodSpaceUserFactionBasicInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserFactionBasicInfo:__init()
    self.msg_type = 5517
end

function SCGodSpaceUserFactionBasicInfo:Decode()
    self.user_num = MsgAdapter.ReadShort() -- 人数
    self.server_num = MsgAdapter.ReadShort()  -- 服务器数（仙宗数
    self.occupy_glade_num = MsgAdapter.ReadShort()  -- 占领空地数
    self.max_glade_num = MsgAdapter.ReadShort()  -- 最大空地数
end 

-- 5518 龙脉信息下发
SCGodSpaceDragonVeinInfo = SCGodSpaceDragonVeinInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceDragonVeinInfo:__init()
    self.msg_type = 5518
end

function SCGodSpaceDragonVeinInfo:Decode()
    self.status = MsgAdapter.ReadShort() -- 0 封印中 1 即将开启 2 开启中
    self.anger_percent = MsgAdapter.ReadShort() -- 怒气百分比[0,100]
    self.boss_hp_percent_list = {}
    for i = 1,3 do 
        self.boss_hp_percent_list[i] = MsgAdapter.ReadShort() -- 3只boss的hp万分比 [0,10000]
    end 
    self.inspire_lv = MsgAdapter.ReadShort() -- 鼓舞buff层数
    self.dragon_vein_close_timestamp = MsgAdapter.ReadUInt() -- 用于显示剩余时间（当status=2时有效）
    self.monster_group_list = {}

    self.boss_group_list = {}
    for i = 1,3 do 
        self.boss_group_list[i] = MsgAdapter.ReadInt() -- 精英怪怪物组id列表
    end 
end

--  5519 龙脉个人信息下发
SCGodSpaceDragonVeinSelfInfo = SCGodSpaceDragonVeinSelfInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceDragonVeinSelfInfo:__init()
    self.msg_type = 5519
end

function SCGodSpaceDragonVeinSelfInfo:Decode()
    self.kill_monster_num  = MsgAdapter.ReadShort()  --  现时击败了多少只小怪
    self.monster_reward_times = MsgAdapter.ReadShort()  -- 剩余小怪击败奖励次数
    self.boss_participate_reward_times = MsgAdapter.ReadShort()  -- 剩余boss参与奖次数
    self.boss_kill_reward_times = MsgAdapter.ReadShort()  -- 剩余boss击败奖次数
    self.boss_remain_fight_times = {}
    for i = 1,3 do 
        self.boss_remain_fight_times[i] = MsgAdapter.ReadShort()
    end 
    local reserve_sh = MsgAdapter.ReadShort()
    self.helper_seq_1 = {}
    for i = 1,5 do 
        self.helper_seq_1[i] = MsgAdapter.ReadShort() -- 助战1
    end 
    self.helper_seq_2 = {}
    for i = 1,5 do 
        self.helper_seq_2[i] = MsgAdapter.ReadShort() -- 助战1
    end 
    self.monster_group_list = {}
    for i = 1,5 do 
        self.monster_group_list[i] = MsgAdapter.ReadInt() -- 个人小怪怪物组列表
    end 
end 

-- 5520 神域界面飘字·龙脉
SCGodSpaceFloatingSentence = SCGodSpaceFloatingSentence or BaseClass(BaseProtocolStruct)
function SCGodSpaceFloatingSentence:__init()
    self.msg_type = 5520
end

function SCGodSpaceFloatingSentence:Decode()
    self.type = MsgAdapter.ReadShort() 	-- 飘字类型 0 快来与我一起击败p1吧！  p1=怪物组ID（通过怪物组ID获取怪物组名） 1 快来龙脉助我一臂之力吧
    local reserve_sh = MsgAdapter.ReadShort()	    -- 保留字
    self.p1 = MsgAdapter.ReadInt()			             -- 参数1
    self.p2 = MsgAdapter.ReadInt()			             -- 参数2
    self.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN) -- 角色名
end 

-- 5521 龙脉位置信息
SCGodSpaceDragonVeinPos = SCGodSpaceDragonVeinPos or BaseClass(BaseProtocolStruct)
function SCGodSpaceDragonVeinPos:__init()
    self.msg_type = 5521
end

function SCGodSpaceDragonVeinPos:Decode()
    self.pos_list = {}
    for i = 0,3 do 
        local vo = {}
        vo.row = MsgAdapter.ReadShort()
        vo.column = MsgAdapter.ReadShort()
        self.pos_list[i] = vo -- 四个阵营的龙脉位置
    end 

end 

-- 5514 神位信息(打开神域主界面时下发)
SCGodSpaceUserGodLevelInfo = SCGodSpaceUserGodLevelInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserGodLevelInfo:__init()
    self.msg_type = 5514
end

function SCGodSpaceUserGodLevelInfo:Decode()
    local info = {}
    info.pos_seq = MsgAdapter.ReadShort() --神位等级
    info.today_guide_flag = MsgAdapter.ReadShort()  --今日是否指引了
    info.task_flag = MsgAdapter.IntFlag(0)      --使用task_order做下标
    info.reward_flag = MsgAdapter.IntFlag(0)    --使用task_order做下标
    info.task_param = {}    --任务进度
    for i = 0, GodPosData.Instance:TaskMaxNum() - 1 do
        info.task_param[i] = MsgAdapter.ReadInt()
    end
    info.lv_reward_flag=MsgAdapter.IntFlag(0)  --神位等级奖励标记
    self.info = info
end

-- 5522 神域未领取精华奖励信息
SCGodSpaceUnfetchEssenceInfo = SCGodSpaceUnfetchEssenceInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceUnfetchEssenceInfo:__init()
    self.msg_type = 5522
end

function SCGodSpaceUnfetchEssenceInfo:Decode()
    self.unfetch_essence_num = MsgAdapter.ReadInt()
    self.unfetch_guaji_essence = MsgAdapter.ReadInt()
    self.unfetch_essence_item_num = MsgAdapter.ReadInt()
    self.unfetch_list = {}
    for i = 1,self.unfetch_essence_item_num do 
        local vo = {}
        vo.item_id = MsgAdapter:ReadUShort()
        vo.num = MsgAdapter:ReadShort()

        table.insert(self.unfetch_list,vo)
    end 
end

-- 5523 神域阶段任务信息下发
SCGodSpacePhaseTaskInfo = SCGodSpacePhaseTaskInfo or BaseClass(BaseProtocolStruct)
function SCGodSpacePhaseTaskInfo:__init()
    self.msg_type = 5523
end

function SCGodSpacePhaseTaskInfo:Decode()
    self.outer_tasks = {}
    self.inner_tasks = {}

    self.outer_progress = {}
    self.inner_progress = {}

    for i = 1,2 do 
        self.outer_tasks[i] = MsgAdapter:ReadShort() -- 外域任务ID  长度2
    end 

    for i = 1,2 do 
        self.outer_progress[i] = MsgAdapter:ReadInt() -- 外域任务ID  长度2
    end 

    for i = 1,2 do 
        self.inner_tasks[i] = MsgAdapter:ReadShort() -- 内域任务ID  长度2
    end 

    for i = 1,2 do 
        self.inner_progress[i] = MsgAdapter:ReadInt() -- 内域任务ID  长度2
    end

    self.outer_task_flag = MsgAdapter.IntFlag()  -- 外域任务完成情况（二进制）
    self.inner_task_flag = MsgAdapter.IntFlag()  -- 内域任务完成情况（二进制）

    self.outer_reward_flag = MsgAdapter.ReadInt() -- 外域奖励领取标记 （0未领取 1已领取）
    self.inner_reward_flag = MsgAdapter.ReadInt() -- 内域奖励领取标记 （0未领取 1已领取）
end


-- ======================= 神域探索 =====================
--// 5530 神域探索阵营信息
SCGodSpaceFactionExploreInfo = SCGodSpaceFactionExploreInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceFactionExploreInfo:__init()
    self.msg_type = 5530
end

function SCGodSpaceFactionExploreInfo:Decode()
    local info = {}
    info.level = MsgAdapter.ReadInt()   --// 阵营探索等级
    info.exp = MsgAdapter.ReadInt()     --// 当前等级的探索值
    info.flush_time = {}                -- 刷新时间戳
    for i = 0, ShenYuProbeCtrl.MaxTaskType - 1 do
        info.flush_time[i] = MsgAdapter.ReadUInt()
    end
    self.info = info
    --LOG("5530神域探索阵营信息：", info)
end

--// 5531 神域探索玩家信息
SCGodSpaceUserExploreInfo = SCGodSpaceUserExploreInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserExploreInfo:__init()
    self.msg_type = 5531
end

function SCGodSpaceUserExploreInfo:Decode()
    local info = {}
    info.reward_flag = MsgAdapter.IntFlag(1)    -- 等级奖励标记
    info.stones = {}    -- 长度为5的旷石列表    (-1，-1)代表没有
    info.grasses = {}   -- 长度为5的药草列表
    for i = 1, 5 do
        local pos = {}
        pos.row = MsgAdapter.ReadShort()
        pos.col = MsgAdapter.ReadShort()
        table.insert(info.stones, pos)
    end
    for i = 1, 5 do
        local pos = {}
        pos.row = MsgAdapter.ReadShort()
        pos.col = MsgAdapter.ReadShort()
        table.insert(info.grasses, pos)
    end
    info.reward_times = {}  --已领取奖励次数    下标0神域任务，1阵营任务，2个人任务
    info.goals = {}         --个人目标完成数    下标0神域任务，1阵营任务，2个人任务
    info.gs_goals = {}      --神域目标完成数    下标0神域任务，1阵营任务，2个人任务
    info.reward_flags = {}  --奖励状态 0不可领取，1已领取 2为已领取（这里把 2 转成-1,方便红点）
    for i = 0, 2 do
        info.reward_times[i] = MsgAdapter.ReadInt()
    end
    for i = 0, 2 do
        info.goals[i] = MsgAdapter.ReadInt()
    end
    for i = 0, 2 do
        info.gs_goals[i] = MsgAdapter.ReadInt()
    end

    for i = 0, 2 do
        local flag = MsgAdapter.ReadInt()
        if flag == 2 then
            flag = -1
        end
        info.reward_flags[i] = flag
    end

    local helper = function ()
        local t = {}
        t.is_partner = MsgAdapter.ReadShort()
        MsgAdapter.ReadShort()
        t.id = MsgAdapter.ReadInt()
        t.pet_unique_id1 = MsgAdapter.ReadUInt()
        t.pet_unique_id2 = MsgAdapter.ReadUInt()
        return t
    end

    -- boss上阵信息
    info.boss_helper = {}
    for i = 0, ShenYuProbeCtrl.MaxBoss - 1 do
        info.boss_helper[i] = {}
        for j = 1, 2 do
            info.boss_helper[i][j] = helper()
        end
    end

    -- 锁妖塔上阵信息 5538可以看到
    -- self.well_helper = {}
    -- for i = 0, ShenYuProbeCtrl.MaxWell - 1 do
    --     self.well_helper[i] = helper()
    -- end

    self.info = info
    --LOG("5531 神域探索玩家信息", info)
end

-- 玩家的协战信息
SCGodSpaceUserHelperInfo = SCGodSpaceUserHelperInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserHelperInfo:__init()
    self.msg_type = 5533
end

function SCGodSpaceUserHelperInfo:Decode()
    local info = {}
    local pet_count = MsgAdapter.ReadInt()
    local partner_count = MsgAdapter.ReadInt()
    info.pet_info_list = {}
    info.partner_info_list = {}
    for i = 1, pet_count do
        local t = {}
        t.sys_type = MsgAdapter.ReadInt()   -- //子系统类型 ShenYuSystemHelpType
        t.pet_id = MsgAdapter.ReadInt()
        t.pet_unique_id1 = MsgAdapter.ReadUInt()
        t.pet_unique_id2 = MsgAdapter.ReadUInt()
        table.insert(info.pet_info_list, t)
    end

    for i = 1, partner_count do
        local t = {}
        t.sys_type = MsgAdapter.ReadInt()   -- //子系统类型 ShenYuSystemHelpType
        t.partner_id = MsgAdapter.ReadInt()
        table.insert(info.partner_info_list, t)
    end
    self.info = info
    --LOG("5533 玩家的协战信息", info)
end


-- 神域探索己方boss信息
SCGodSpaceExploreBossInfo = SCGodSpaceExploreBossInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceExploreBossInfo:__init()
    self.msg_type = 5535
end

function SCGodSpaceExploreBossInfo:Decode()
    local boss_data = function ()
        local t = {}
        t.boss_max_hp = MsgAdapter.ReadLL()          -- boss最大血量
        t.boss_remain_hp = MsgAdapter.ReadLL()   -- 怪物剩余血量
        t.total_cap = MsgAdapter.ReadLL()        -- 总计战斗力
        t.pet_num = MsgAdapter.ReadInt()         -- 上阵宠物数量
        t.partner_num = MsgAdapter.ReadInt()     -- 上阵伙伴数
        t.ul_row = MsgAdapter.ReadInt()      -- 方块的左上角位置
        t.ul_col = MsgAdapter.ReadInt()
        t.row = t.ul_row + 1
        t.col = t.ul_col + 1
        return t
    end

    self.info = {}
    for i = 0, ShenYuProbeCtrl.MaxBoss - 1 do
        self.info[i] = boss_data()
        self.info[i].id = i
    end
    --LOG("5535 神域探索己方boss信息", self)
end

-- boss伤害排行
SCGodSpaceExploreBossRankList = SCGodSpaceExploreBossRankList or BaseClass(BaseProtocolStruct)
function SCGodSpaceExploreBossRankList:__init()
    self.msg_type = 5536
end

function SCGodSpaceExploreBossRankList:Decode()
    local list = {}
    for i = 1, 4 do
        local t = {}
        t.rank = i
        t.id = MsgAdapter.ReadInt()     -- 阵营id
        t.num = MsgAdapter.ReadLL()     -- 总伤害
        table.insert(list, t)
    end
    self.list = list
    --LOG("5536 神域探索boss伤害排行", self)
end

-- 神域探索boss设置上阵
CSGodSpaceExploreSetFighter = CSGodSpaceExploreSetFighter or BaseClass(BaseProtocolStruct)
function CSGodSpaceExploreSetFighter:__init()
    self.msg_type = 5537
end

function CSGodSpaceExploreSetFighter:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    local info = self.info or {}
    MsgAdapter.WriteInt(info.boss_id)
    MsgAdapter.WriteInt(info.partner_id_1 or 0)
    MsgAdapter.WriteInt(info.pet_index_1 or -1)
    MsgAdapter.WriteInt(info.partner_id_2 or 0)
    MsgAdapter.WriteInt(info.pet_index_2 or -1)
    --LOG("5537 神域探索boss设置上阵", self)
end

--// 5538 神域探索封妖井信息下发
SCGodSpaceExploreWellInfo = SCGodSpaceExploreWellInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceExploreWellInfo:__init()
    self.msg_type = 5538
end

function SCGodSpaceExploreWellInfo:Decode()
    local info = {}
    local data_func = function ()
        local t = {}
        t.uid = MsgAdapter.ReadInt()
        t.plat_type = MsgAdapter.ReadInt()
        t.player_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        t.player_level = MsgAdapter.ReadShort()
        t.head_id = MsgAdapter.ReadShort()
        t.prof_type = MsgAdapter.ReadShort()
        t.avatar_type = MsgAdapter.ReadShort()

        t.is_partner = MsgAdapter.ReadChar()
        t.seq = MsgAdapter.ReadChar()           -- // 坑位序号 [0,max_slot)
        t.str_level = MsgAdapter.ReadShort()    -- 强化等级
        t.id = MsgAdapter.ReadInt()
        t.skin_id = MsgAdapter.ReadInt()
        t.qua = MsgAdapter.ReadShort()
        t.fly_flag = MsgAdapter.ReadShort()     -- 飞升等级
        t.show_type = t.is_partner == 0 and 1 or 2  -- 1是宠物，2是伙伴
        return t
    end

    info.well_type = MsgAdapter.ReadShort()
    MsgAdapter.ReadShort()
    info.need_count = MsgAdapter.ReadShort()    -- 需要封印的数量
    info.seal_count = MsgAdapter.ReadShort()    -- 现在已经封印的数量
    info.list = {}
    local self_uid = RoleData.Instance:GetRoleId()
    info.is_up = false    -- 玩家自己是否已上阵
    for i = 1, info.seal_count do
        local data = data_func()
        data.well_type = info.well_type
        info.list[data.seq] = data
        if data.uid == self_uid then
            info.is_up = true
        end
    end
    for i = info.seal_count + 1, info.need_count do
        table.insert(info.list, {well_type = info.well_type})
    end
    self.info = info
    --LOG("5538 神域探索封妖井信息下发", info)
end


--// 5539 封妖井位置信息下发
SCGodSpaceExploreWellPos = SCGodSpaceExploreWellPos or BaseClass(BaseProtocolStruct)
function SCGodSpaceExploreWellPos:__init()
    self.msg_type = 5539
end

function SCGodSpaceExploreWellPos:Decode()
    local info = {}
    info = {}
    for i = 0, ShenYuProbeCtrl.MaxWell - 1 do
        local t = {}
        t.well_type = i
        t.ul_row = MsgAdapter.ReadShort()       --// 左上角格点-行 (-1表示未刷新)
        t.ul_col = MsgAdapter.ReadShort()       --// 左上角格点-列 (-1表示未刷新)          
        t.seal_count = MsgAdapter.ReadShort()   -- 当前封印数量
        t.need_count = MsgAdapter.ReadShort()   -- 总需要封印数
        t.row = t.ul_row + 1
        t.col = t.ul_col + 1
        info[i] = t
    end
    self.info = info
    --LOG("5539 封妖井位置信息下发", self)
end

-- =====================================================

local function ShopItem()
    local t = {}
    t.unique_seq = MsgAdapter:ReadShort() -- 商品唯一seq（通过seq从配置中找到售价 物品 货物类型
    t.discount = MsgAdapter:ReadShort() -- 折扣 [0,10]
    return t
end

-- 神域商店 5524 
SCGodSpaceUserShopInfo = SCGodSpaceUserShopInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserShopInfo:__init()
    self.msg_type = 5524
end 

function SCGodSpaceUserShopInfo:Decode()
    self.next_refresh_timestamp = MsgAdapter:ReadUInt() ---下次自动刷新时间戳
    self.manual_refresh_times  = MsgAdapter:ReadShort() --当前已手动刷新的次数
    self.daily_reward_flag = MsgAdapter:ReadChar() --- 0未领取-1已领取
    local reserve_ch = MsgAdapter:ReadChar( )
    self.item_flag = MsgAdapter.IntFlag() --标识哪些物品已购买(二进制)
    self.items = {}
    for i =1,6 do
        local shop_item = ShopItem()
        table.insert(self.items, shop_item)
    end
end

local function BuffInfo()
    local t = {}
    t.expire_timestamp = MsgAdapter:ReadUInt() ---过期时间戳 为0则不会过期
    t.remain_times = MsgAdapter:ReadShort() ---剩余次数 为0则不显示次数
    t.buff_seq = MsgAdapter:ReadShort() --S-神域商店.xlsx 神域buff参数sheet
    return t
end

-- 神域玩家buff列表 5525
SCGodSpaceUserBuffInfo = SCGodSpaceUserBuffInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserBuffInfo:__init()
    self.msg_type = 5525
end 

function SCGodSpaceUserBuffInfo:Decode()
    self.buff_num = MsgAdapter.ReadInt()
    self.buffs = {}
    for i = 1,self.buff_num do 
        self.buffs[i] = BuffInfo()
    end 
end 

-- 神域玩家buff列表 5526
SCGodSpaceUserBuffShop = SCGodSpaceUserBuffShop or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserBuffShop:__init()
    self.msg_type = 5526
end 

function SCGodSpaceUserBuffShop:Decode()
    self.buy_times = {}
    for i = 1,10 do 
        self.buy_times[i] = MsgAdapter:ReadShort() -- 10个商品的已购买次数
    end 
end 

-- 全民福利领取信息下发
SCGodSpaceUserSeasonPassFuliInfo = SCGodSpaceUserSeasonPassFuliInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserSeasonPassFuliInfo:__init()
    self.msg_type = 5527
end 

function SCGodSpaceUserSeasonPassFuliInfo:Decode()
    self.fuli_flag = MsgAdapter.IntFlag() 
    self.fuli_buy_num = MsgAdapter.ReadInt() 
end

-- 神域玩家宝藏信息
SCGodSpaceSeasonPassInfo = SCGodSpaceSeasonPassInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceSeasonPassInfo:__init()
    self.msg_type = 5528
end 

function SCGodSpaceSeasonPassInfo:Decode()
    self.is_open = MsgAdapter.ReadChar()                        -- 是否开启神域宝藏
    self.is_active_senior = MsgAdapter.ReadChar()               -- 是否激活至尊宝藏
    local reserve_sh = MsgAdapter.ReadShort()                   -- 保留字

    self.level = MsgAdapter.ReadShort()                         -- 等级
    self.exp = MsgAdapter.ReadShort()                           -- 经验

    -- self.fetch_common_flag = {}
    -- for i = 0,63 do 
    --     self.fetch_common_flag[i] = MsgAdapter.ReadChar()         --普通奖励领取标志位 char[64]
    -- end 

    -- self.fetch_senior_flag = {}
    -- for i = 0,63 do 
    --     self.fetch_senior_flag[i] = MsgAdapter.ReadChar()       --高级奖励领取标志位 char[64]
    -- end 
    self.fetch_common_flag = MsgAdapter.IntFlag(2)
    self.fetch_senior_flag = MsgAdapter.IntFlag(2)

    self.daily_tasks = {}
    for i = 0,31 do 
        self.daily_tasks[i] = MsgAdapter.ReadShort()            -- 每日任务列表 数组长度32  -1代表任务为空
    end 

    self.daily_task_progress = {}
    for i = 0,31 do 
        self.daily_task_progress[i] = MsgAdapter.ReadInt()      -- 每日任务进度 数组长度32  -1代表已领取，大于等于0表示进度
    end 

    self.season_tasks = {}
    for i = 0,31 do 
        self.season_tasks[i] = MsgAdapter.ReadShort()           -- 赛季任务列表 数组长度32  -1代表任务为空
    end 

    self.season_task_progress = {}
    for i = 0,31 do 
        self.season_task_progress[i] = MsgAdapter.ReadInt()     -- 赛季任务进度 数组长度32  -1代表已领取，大于等于0表示进度
    end 

    self.surplus_exp = MsgAdapter.ReadShort()                   -- 溢出经验
    self.surplus_receive_count = MsgAdapter.ReadShort()         -- 溢出奖励可领取次数
end

-- 神域宝藏请求
CSGodSpaceUserSeasonPassOperReq = CSGodSpaceUserSeasonPassOperReq or BaseClass(BaseProtocolStruct)
function CSGodSpaceUserSeasonPassOperReq:__init()
    self.msg_type = 5529
end 

function CSGodSpaceUserSeasonPassOperReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.oper_type)
    MsgAdapter.WriteInt(self.param_1)
    MsgAdapter.WriteInt(self.param_2)
end

-- 强制玩家关闭神域UI
SCGodSpaceUserForcedCloseMainUI = SCGodSpaceUserForcedCloseMainUI or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserForcedCloseMainUI:__init()
    self.msg_type = 5532
end 
function SCGodSpaceUserForcedCloseMainUI:Decode()end

-- 玩家能否进入神域
SCGodSpaceUserCheckAccess = SCGodSpaceUserCheckAccess or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserCheckAccess:__init()
    self.msg_type = 5534
end 

function SCGodSpaceUserCheckAccess:Decode()
    self.can_enter = MsgAdapter:ReadInt() -- 0 进不去 1 可进入
end

-- 神域狂欢宝藏购买次数下发
SCGodSpaceUserSeasonPassBuyNumInfo = SCGodSpaceUserSeasonPassBuyNumInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceUserSeasonPassBuyNumInfo:__init()
    self.msg_type = 5540
end 

function SCGodSpaceUserSeasonPassBuyNumInfo:Decode()
    self.total_buy_num = MsgAdapter.ReadInt() 
end

SCGodSpaceApNotEnough = SCGodSpaceApNotEnough or BaseClass(BaseProtocolStruct)
function SCGodSpaceApNotEnough:__init()
    self.msg_type = 5541
end

function SCGodSpaceApNotEnough:Decode()
    
end