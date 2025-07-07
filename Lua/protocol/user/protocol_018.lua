SCPrestigeAllVillageInfo = SCPrestigeAllVillageInfo or BaseClass(BaseProtocolStruct)
function SCPrestigeAllVillageInfo:__init()
    self.msg_type = 1800
end

function SCPrestigeAllVillageInfo:Decode()
    local count = MsgAdapter.ReadInt()
    self.ret_data = {}
    for i = 0, count - 1 do
        local data = {}
        data.prestige = MsgAdapter.ReadInt()
        data.level = MsgAdapter.ReadInt()
        self.ret_data[i] = data
    end
end

SCPrestigeAllShopInfo = SCPrestigeAllShopInfo or BaseClass(BaseProtocolStruct)
function SCPrestigeAllShopInfo:__init()
    self.msg_type = 1801
end

function SCPrestigeAllShopInfo:Decode()
    self.buy_count_list = {}
    for i = 0, PRESTIGE_VILLAGE_GOODS_MAX_NUM - 1 do
        self.buy_count_list[i] = MsgAdapter.ReadShort()
    end
end

SCPrestigeSingleVillageInfo = SCPrestigeSingleVillageInfo or BaseClass(BaseProtocolStruct)
function SCPrestigeSingleVillageInfo:__init()
    self.msg_type = 1802
end

function SCPrestigeSingleVillageInfo:Decode()
    self.index = MsgAdapter.ReadInt()
    self.data = {}
    self.data.prestige = MsgAdapter.ReadInt()
    self.data.level = MsgAdapter.ReadInt()
end

SCPrestigeSingleShopInfo = SCPrestigeSingleShopInfo or BaseClass(BaseProtocolStruct)
function SCPrestigeSingleShopInfo:__init()
    self.msg_type = 1803
end

function SCPrestigeSingleShopInfo:Decode()
    self.index = MsgAdapter.ReadShort()
    self.buy_count = MsgAdapter.ReadShort()
end

CSPrestigeShopingReq = CSPrestigeShopingReq or BaseClass(BaseProtocolStruct)
function CSPrestigeShopingReq:__init()
    self.msg_type = 1804
end

function CSPrestigeShopingReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.index)
    MsgAdapter.WriteInt(self.num)
end

SCPrestigeDonateInfo = SCPrestigeDonateInfo or BaseClass(BaseProtocolStruct)
function SCPrestigeDonateInfo:__init()
    self.msg_type = 1805
end

function SCPrestigeDonateInfo:Decode()
    self.donate_count = MsgAdapter.ReadInt() -- 今日捐赠了多少件
    self.add_value = MsgAdapter.ReadInt() -- 今日获得了多少声望
end

CSPrestigeDonateOp = CSPrestigeDonateOp or BaseClass(BaseProtocolStruct)
function CSPrestigeDonateOp:__init()
    self.msg_type = 1806
end

function CSPrestigeDonateOp:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.village_id)
    MsgAdapter.WriteInt(self.column)
    MsgAdapter.WriteInt(self.index)
end

SCPrestigeDonateNotice = SCPrestigeDonateNotice or BaseClass(BaseProtocolStruct)
function SCPrestigeDonateNotice:__init()
    self.msg_type = 1807
end

function SCPrestigeDonateNotice:Decode()
    self.donate_count = MsgAdapter.ReadInt()
end

--功能引导 操作请求
--op_type 0 选择新收宠物 param:选项的seq
FUNCTIONGUIDEOPTYPE = {
    PETSELECT = 0,
    AUTO_SEAL_PET = 1, -- 进入自动抓宠战斗
    FUNC_GUIDE_FINISH_CLOUD_ARENA_GUIDE = 2,
    LUNHUI_WANGCHUAN = 3,   -- 轮回忘川
}
CSFuncGuideOperator = CSFuncGuideOperator or BaseClass(BaseProtocolStruct)
function CSFuncGuideOperator:__init()
    self.msg_type = 1830
    self.op_type = -1
    self.param = -1
end

function CSFuncGuideOperator:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.op_type)
    MsgAdapter.WriteInt(self.param)
end

SCFuncGuideFlagInfo = SCFuncGuideFlagInfo or BaseClass(BaseProtocolStruct)
function SCFuncGuideFlagInfo:__init()
    self.msg_type = 1831
end

function SCFuncGuideFlagInfo:Decode()
    self.flag = MsgAdapter.IntFlag(2)
end

SCPetDeadSummonGuideInfo = SCPetDeadSummonGuideInfo or BaseClass(BaseProtocolStruct)
function SCPetDeadSummonGuideInfo:__init()
    self.msg_type = 1833
end
function SCPetDeadSummonGuideInfo:Decode()
    self.guide_trigger_times = {}
    for i = 1, 8, 1 do
        self.guide_trigger_times[i] = MsgAdapter.ReadShort()
    end
end

CSPetDeadSummonGuideTrigger = CSPetDeadSummonGuideTrigger or BaseClass(BaseProtocolStruct)
function CSPetDeadSummonGuideTrigger:__init()
    self.msg_type = 1832
end
function CSPetDeadSummonGuideTrigger:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
end

SCPrestigeChangeNotice = SCPrestigeChangeNotice or BaseClass(BaseProtocolStruct)
function SCPrestigeChangeNotice:__init()
    self.msg_type = 1808
end

function SCPrestigeChangeNotice:Decode()
   self.village_id = MsgAdapter.ReadInt()
   self.param = MsgAdapter.ReadInt()
end

-------------------------新百货商店--------------------------------------------
--百货商店 请求
CSConvertShopBuyReq = CSConvertShopBuyReq or BaseClass(BaseProtocolStruct)
function CSConvertShopBuyReq:__init()
    self.msg_type = 1850
end

function CSConvertShopBuyReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.conver_type)
    MsgAdapter.WriteShort(self.seq)
    MsgAdapter.WriteShort(self.item_id)
    MsgAdapter.WriteShort(self.num)
end

--百货商店 全部信息
SCConvertShopBuyRecord = SCConvertShopBuyRecord or BaseClass(BaseProtocolStruct)
function SCConvertShopBuyRecord:__init()
    self.msg_type = 1851
end

function SCConvertShopBuyRecord:Decode()
    self.buy_count = {}
    for i=1, MsgAdapter.ReadInt() do
        local info = {}
        MsgAdapter.ReadShort()
        info.conver_type = MsgAdapter.ReadShort()
        info.count = MsgAdapter.ReadShort()
        info.seq = MsgAdapter.ReadShort()
        if self.buy_count[info.conver_type] == nil then
            self.buy_count[info.conver_type] = SmartData.New{}
        end
        self.buy_count[info.conver_type][info.seq] = info.count
    end
end

--百货商店 单个信息
SCConvertShopAddRecord = SCConvertShopAddRecord or BaseClass(BaseProtocolStruct)
function SCConvertShopAddRecord:__init()
    self.msg_type = 1852
end

function SCConvertShopAddRecord:Decode()
    local info = {}
    MsgAdapter.ReadShort()
    info.conver_type = MsgAdapter.ReadShort()
    info.count = MsgAdapter.ReadShort()
    info.seq = MsgAdapter.ReadShort()
    self.count_info = info
end

--主线推图
SCBossChapterInfo = SCBossChapterInfo or BaseClass(BaseProtocolStruct)
function SCBossChapterInfo:__init()
    self.msg_type = 1860
end 

function SCBossChapterInfo:Decode()
    self.num = MsgAdapter.ReadShort()
    self.reason = MsgAdapter.ReadShort()
    self.chapter_reward_flag = MsgAdapter.ReadLL()
    self.info_list = {}
    for i = 1,self.num do 
        local vo = {}
        vo.monster_group_id = MsgAdapter.ReadInt()
        vo.reward_status =  MsgAdapter.ReadChar()
        MsgAdapter.ReadChar()
        MsgAdapter.ReadShort()
        table.insert(self.info_list,vo)
    end 
end 

CSBossChapterReq = CSBossChapterReq or BaseClass(BaseProtocolStruct) 
function CSBossChapterReq:__init()
    self.msg_type = 1861
end 

function CSBossChapterReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param)
end 

SCFuncPreviewInfo = SCFuncPreviewInfo or BaseClass(BaseProtocolStruct)     --功能预告信息  1870
function SCFuncPreviewInfo:__init()
    self.msg_type = 1870
end
function SCFuncPreviewInfo:Decode()
    self.notice_num = MsgAdapter.ReadInt() --预告数量 （按实际数量读取下面的列表）
    self.notice_id_list = {}
    for i = 0, self.notice_num - 1 do
        local info = {
            status = MsgAdapter.ReadShort(),
            task_status = MsgAdapter.ReadChar(),
            task_process = MsgAdapter.ReadChar(),
        }
        self.notice_id_list[i] = info
    end
    -- 领取状态列表，下标表示预告ID，值表示领取状态，取值  0：不可领取  1：可领取  2：已领取
end

CSFuncPreviewClaimReward = CSFuncPreviewClaimReward or BaseClass(BaseProtocolStruct)    --< 领取预告奖励 1871
function CSFuncPreviewClaimReward:__init()
    self.msg_type = 1871
end
function CSFuncPreviewClaimReward:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.notice_id)
end

-- 境界请求操作
CSRealmBreakReq = CSRealmBreakReq or BaseClass(BaseProtocolStruct)
function CSRealmBreakReq:__init()
    self.msg_type = 1880
end

function CSRealmBreakReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.notice_type) --0是请求突破，1是请求吐纳，2是请求领取经验，3是请求挑战心灵之境 param=seq
    MsgAdapter.WriteShort(self.param)
end

-- 境界信息下发
SCRealmBreakAllInfo = SCRealmBreakAllInfo or BaseClass(BaseProtocolStruct)
function SCRealmBreakAllInfo:__init()
    self.msg_type = 1881
end

function SCRealmBreakAllInfo:Decode()
    self.cur_seq = MsgAdapter.ReadShort()                       --当前阶数
    self.tuna_num = MsgAdapter.ReadShort()                      --当前阶数的已吐纳次数
    self.cur_break_exp = MsgAdapter.ReadUInt()                  --当前灵力值
    self.cumulation_user_exp_count = MsgAdapter.ReadUInt()      --玩家积累的待领取经验值
    self.user_cumulation_pratice_time = MsgAdapter.ReadUInt()   --玩家积累修炼的时间
    self.cumulation_user_coin_count = MsgAdapter.ReadUInt()     --玩家积累待领取铜币
    self.cur_fight_seq = MsgAdapter.ReadShort()                 --心灵之境战斗层数
    MsgAdapter.ReadShort()
end

-- 境界累计道具下发
SCRealmBreakItemInfo = SCRealmBreakItemInfo or BaseClass(BaseProtocolStruct)
function SCRealmBreakItemInfo:__init()
    self.msg_type = 1883
end

function SCRealmBreakItemInfo:Decode()
    self.item_count = MsgAdapter.ReadInt()  --道具数量
    self.cumulation_item = {}
    for i = 1, self.item_count do
        local tab = {}
        tab.item_id = MsgAdapter.ReadUShort()
        tab.is_bind = MsgAdapter.ReadShort()
        tab.num = MsgAdapter.ReadInt()
        table.insert(self.cumulation_item, tab)
    end
end

-- 地图探索请求
CSMapExplorationReq = CSMapExplorationReq or BaseClass(BaseProtocolStruct)
function CSMapExplorationReq:__init()
    self.msg_type = 1890
end

function CSMapExplorationReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type) --0请求下发，1请求探索，param填对应seq 2请求探索信息 3 请求领取探索奖励param scene_id param_1 reward_bit_index
    MsgAdapter.WriteInt(self.param)
    MsgAdapter.WriteInt(self.param_1)
end

SCMapExplorationInfo = SCMapExplorationInfo or BaseClass(BaseProtocolStruct)
function SCMapExplorationInfo:__init()
    self.msg_type = 1891
end

function SCMapExplorationInfo:Decode()
    self.m_map = MsgAdapter.IntFlag(16)
end

local function MapExplorationRewardInfo()
    local t = {}
    t.scene_id = MsgAdapter.ReadShort()
    t.reward_flag = MsgAdapter.ShortFlag()
    return t
end

SCMapExplorationRewardInfo = SCMapExplorationRewardInfo or BaseClass(BaseProtocolStruct)
function SCMapExplorationRewardInfo:__init()
    self.msg_type = 1892
end

function SCMapExplorationRewardInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.map_exploration_reward_list = {}
    for i = 1,self.count do 
        self.map_exploration_reward_list[i] = MapExplorationRewardInfo()
    end 
end

SCMapExplorationSingleRewardInfo = SCMapExplorationSingleRewardInfo or BaseClass(BaseProtocolStruct)
function SCMapExplorationSingleRewardInfo:__init()
    self.msg_type = 1893
end

function SCMapExplorationSingleRewardInfo:Decode()
    self.info = MapExplorationRewardInfo()
end