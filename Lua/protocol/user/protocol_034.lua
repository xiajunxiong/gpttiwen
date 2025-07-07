--专属礼包下发
SCRAZhuanShuLiBaoInfo = SCRAZhuanShuLiBaoInfo or BaseClass(BaseProtocolStruct)
function SCRAZhuanShuLiBaoInfo:__init()
	self.msg_type = 3400
end

function SCRAZhuanShuLiBaoInfo:Decode()
	self.info = {}
    for i = 1, 5 do
    	local data = {}
    	data.is_active = MsgAdapter.ReadShort()	--非0就是激活了
    	data.type = MsgAdapter.ReadShort()		--配置表的type
    	data.buy_times = MsgAdapter.ReadInt()	--已购买次数
    	data.end_time = MsgAdapter.ReadUInt()	--结束时间戳
    	table.insert(self.info, data)
    end
end

--专属礼包-每日礼包信息下发
SCRAZhuanShuLiBaoDayInfo = SCRAZhuanShuLiBaoDayInfo or BaseClass(BaseProtocolStruct)
function SCRAZhuanShuLiBaoDayInfo:__init()
    self.msg_type = 3401
end

function SCRAZhuanShuLiBaoDayInfo:Decode()
    --通过phase+type找对应配置，找不到不显示
    self.day_item = {}
    for i = 1, 5 do
        local data = {}
        data.phase = MsgAdapter.ReadShort()
        data.type = MsgAdapter.ReadShort()
        data.buy_times = MsgAdapter.ReadShort()
        data.reserve_sh = MsgAdapter.ReadShort()
        data.end_time = MsgAdapter.ReadUInt() --礼包结束时间戳
        table.insert(self.day_item, data)
    end
    -- LogError("day_items -> ", self.day_item)
end


--限时宝盒
SCRAXianShiBaoHeInfo = SCRAXianShiBaoHeInfo or BaseClass(BaseProtocolStruct)
function SCRAXianShiBaoHeInfo:__init()
	self.msg_type = 3420
end

function SCRAXianShiBaoHeInfo:Decode()
	self.day_buy_times = {} --每天限购
	for i = 0, RA_XIAN_SHI_BAO_HE_MAX_ITEM - 1 do
		self.day_buy_times[i] = MsgAdapter.ReadUShort()
	end
	self.buy_times = {} --总限购
	for i = 0, RA_XIAN_SHI_BAO_HE_MAX_ITEM - 1 do
		self.buy_times[i] = MsgAdapter.ReadUShort()	
    end
    self.flag_num = DataHelper.GetFetchFlagNum(MsgAdapter.IntFlag())
    self.has_free_reward = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
end

--限时宝袋
SCRAXianShiBaoDaiInfo = SCRAXianShiBaoDaiInfo or BaseClass(BaseProtocolStruct)
function SCRAXianShiBaoDaiInfo:__init()
	self.msg_type = 3430
end

function SCRAXianShiBaoDaiInfo:Decode()
	self.can_buy_seq = bit:d2b(MsgAdapter.ReadInt())
	self.day_buy_times = {} --每天限购
	for i = 0, RA_XIAN_SHI_BAO_DAI_MAX_ITEM - 1 do
		self.day_buy_times[i] = MsgAdapter.ReadUShort()
	end
	self.buy_times = {} --总限购
	for i = 0, RA_XIAN_SHI_BAO_DAI_MAX_ITEM - 1 do
		self.buy_times[i] = MsgAdapter.ReadUShort()
    end
    self.flag_num = DataHelper.GetFetchFlagNum(MsgAdapter.IntFlag())
    self.has_free_reward = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
end

-- 经验飙升下发
SCRAJingYanBiaoShengInfo = SCRAJingYanBiaoShengInfo or BaseClass(BaseProtocolStruct)
function SCRAJingYanBiaoShengInfo:__init()
	self.msg_type = 3410
end

function SCRAJingYanBiaoShengInfo:Decode()
	self.buy_times = MsgAdapter.ReadInt()		--活动购买次数
	self.day_buy_times = MsgAdapter.ReadInt()	--今日购买次数
end

-- 温香楼
CSWenXiangLouReq = CSWenXiangLouReq or BaseClass(BaseProtocolStruct)
function CSWenXiangLouReq:__init()
    self.msg_type = 3474
end

function CSWenXiangLouReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.oper_type)
    MsgAdapter.WriteInt(self.param1)
end

SCWenXiangLouNpcInfo = SCWenXiangLouNpcInfo or BaseClass(BaseProtocolStruct)
function SCWenXiangLouNpcInfo:__init()
    self.msg_type = 3475
end

function SCWenXiangLouNpcInfo:Decode()
    local t = {
        load_status = MsgAdapter.ReadChar(),    -- 进行阶段
        npc_sucspicion_list = {},               -- npc嫌疑值
        npc_talk_list = {},                     -- npc对话记录
        npc_identity_list = {},                 -- npc指认记录
        npc_check_list = {},                    -- npc查证记录
    }
    MsgAdapter.ReadChar()
    MsgAdapter.ReadShort()
    for i = 0, WenXiangLouConfig.MAX_NPC_NUM - 1 do
        t.npc_sucspicion_list[i] = MsgAdapter.ReadChar()
    end
    for i = 0, WenXiangLouConfig.MAX_NPC_NUM - 1 do
        t.npc_talk_list[i] = MsgAdapter.ReadChar()
    end
    for i = 0, WenXiangLouConfig.MAX_NPC_NUM - 1 do
        t.npc_identity_list[i] = MsgAdapter.ReadChar()
    end
    for i = 0, WenXiangLouConfig.MAX_NPC_NUM - 1 do
        t.npc_check_list[i] = MsgAdapter.ReadChar()
    end
    self.info = t
end

SCWenXiangLouReqSceneInfo = SCWenXiangLouReqSceneInfo or BaseClass(BaseProtocolStruct)
function SCWenXiangLouReqSceneInfo:__init()
    self.msg_type = 3476
end

function SCWenXiangLouReqSceneInfo:Decode()
    local t = {
        load_status = MsgAdapter.ReadChar(),         -- 进行阶段
        role_enter_posi = MsgAdapter.ReadChar(),    -- 是否触发进入特定地点
        has_find_identity = MsgAdapter.ReadChar(),  -- 是否指认正确
        has_finish_fb = MsgAdapter.ReadChar(),      -- 是否完成副本
        has_enter = MsgAdapter.ReadChar(),          -- 是否进过副本
        reserve_ch = MsgAdapter.ReadChar(),
        reserve_sh = MsgAdapter.ReadShort(),
        reward_count = MsgAdapter.ReadInt(),        -- 临时道具背包
        reward_list = {}
    }
    for i = 1, t.reward_count do
        t.reward_list[MsgAdapter.ReadInt()] = true
    end
    self.info = t
end

SCResearchTaskNoticInfo = SCResearchTaskNoticInfo or BaseClass(BaseProtocolStruct)
function SCResearchTaskNoticInfo:__init()
	self.msg_type = 3498
end

function SCResearchTaskNoticInfo:Decode()
    self.add_exp = MsgAdapter.ReadLL()
    self.reward_num = MsgAdapter.ReadInt()
    self.item_list = {}
    for i=1,self.reward_num do
        local info = {}
        info.item_id = MsgAdapter.ReadUShort()
        info.num = MsgAdapter.ReadShort()
        info.is_bind = MsgAdapter.ReadInt()
        self.item_list[i] = info
    end
end

--===========================奇闻任务===========================
local function StrangerTaskInfo(is_all_info)
    local t = {}
    t.stranger_task_type=MsgAdapter.ReadShort()--奇闻任务类型
    t.task_id = MsgAdapter.ReadUShort()   --当前任务id
    t.has_finish = MsgAdapter.ReadUShort()--是否已经完成该类型奇闻任务
    t.accept_status = MsgAdapter.ReadUShort()--当前任务接受状态 0可接受 1 已接受
    t.progress = MsgAdapter.ReadInt()     --任务进度
    t.is_all_info = is_all_info or false  --是否是登录下发
    return t
end
--单个奇闻任务信息下发
SCStrangerTaskInfo = SCStrangerTaskInfo or BaseClass(BaseProtocolStruct)
function SCStrangerTaskInfo:__init()
    self.msg_type = 3490
end

function SCStrangerTaskInfo:Decode()
    self.task_info = StrangerTaskInfo()
end

--全部奇闻任务信息下发
SCStrangerTaskAllInfo = SCStrangerTaskAllInfo or BaseClass(BaseProtocolStruct)
function SCStrangerTaskAllInfo:__init()
    self.msg_type = 3491
end

function SCStrangerTaskAllInfo:Decode()
    self.task_info_list = {}
    self.count = MsgAdapter.ReadInt()
    for i=1,self.count do
        self.task_info_list[i] = StrangerTaskInfo(true)
    end
end

--奇闻任务战斗失败
SCStrangerTaskBattleFail = SCStrangerTaskBattleFail or BaseClass(BaseProtocolStruct)
function SCStrangerTaskBattleFail:__init()
    self.msg_type = 3492
end

function SCStrangerTaskBattleFail:Decode()
    self.stranger_task_type = MsgAdapter.ReadShort()
    self.task_id = MsgAdapter.ReadUShort()
end

--奇闻任务完成通知
SCStrangerTaskCompleteNotice = SCStrangerTaskCompleteNotice or BaseClass(BaseProtocolStruct)
function SCStrangerTaskCompleteNotice:__init()
    self.msg_type = 3493
end

function SCStrangerTaskCompleteNotice:Decode()
    self.task_id = MsgAdapter.ReadUShort()
    self.stranger_task_type = MsgAdapter.ReadShort()
end

--奇闻任务请求 
--0, 接受任务  p1 任务ID
--1, 提交任务  p1 任务ID
--2, 领取任务奖励 p1 奇闻任务类型
--3 领取大事件奖励 p1 奖励seq
CSStrangerTaskOperate = CSStrangerTaskOperate or BaseClass(BaseProtocolStruct)
function CSStrangerTaskOperate:__init()
    self.msg_type = 3494
end

function CSStrangerTaskOperate:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.req_type)
    MsgAdapter.WriteUShort(self.param1)
end

--奇闻任务其他特殊下发，登录下发
SCAnecdoteTaskSpecialInfo = SCAnecdoteTaskSpecialInfo or BaseClass(BaseProtocolStruct)
function SCAnecdoteTaskSpecialInfo:__init()
    self.msg_type = 3495
end

function SCAnecdoteTaskSpecialInfo:Decode()
    self.special_list = {}
    for i=0,ANECDOTE_TASK_SPECIAL_MAX_NUM do
        self.special_list[i] = MsgAdapter.ReadInt()
    end
end

-- 奇闻任务其他特殊设置
CSAnecdoteTaskSpecialReq = CSAnecdoteTaskSpecialReq or BaseClass(BaseProtocolStruct)
function CSAnecdoteTaskSpecialReq:__init()
    self.msg_type = 3496
end

function CSAnecdoteTaskSpecialReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.anecdote_type)
    MsgAdapter.WriteInt(self.anecdote_value)
end

-- 奇闻任务主线任务交互
CSAnecdoteTaskFinishReq = CSAnecdoteTaskFinishReq or BaseClass(BaseProtocolStruct)
function CSAnecdoteTaskFinishReq:__init()
    self.msg_type = 3497
end

function CSAnecdoteTaskFinishReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.task_id)
    MsgAdapter.WriteShort(self.anecdote_type)
end

--新的任务行为请求
CSNewTaskAction = CSNewTaskAction or BaseClass(BaseProtocolStruct)
function CSNewTaskAction:__init()
    self.msg_type = 3500
end

function CSNewTaskAction:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.action_type)
    MsgAdapter.WriteShort(0)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    MsgAdapter.WriteInt(self.param3)
    MsgAdapter.WriteInt(self.param4)
end

-- 提交道具(列表)
CSNewGiveNPCTaskNeedItems = CSNewGiveNPCTaskNeedItems or BaseClass(BaseProtocolStruct)
function CSNewGiveNPCTaskNeedItems:__init()
    self.msg_type = 3504
end

function CSNewGiveNPCTaskNeedItems:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteUShort(self.task_id) -- 任务ID
    MsgAdapter.WriteUShort(#self.commit_list) -- 提交的道具种数
    for i = 1, #self.commit_list do
        local item_data = self.commit_list[i]
        MsgAdapter.WriteUShort(item_data.item_id or 0) -- 道具ID
        MsgAdapter.WriteUShort(item_data.sub_num or 0) -- 提交的道具数量
    end
end


-- 奇闻大事件信息下发
SCAnecdoteBigEventInfo = SCAnecdoteBigEventInfo or BaseClass(BaseProtocolStruct)
function SCAnecdoteBigEventInfo:__init()
    self.msg_type = 3499
end

function SCAnecdoteBigEventInfo:Decode()
    self.reward_version = MsgAdapter.ReadChar()       --奖励版本号
    self.is_big_event_open_now = MsgAdapter.ReadChar()  ---0,1开启中
    self.anecdote_finish_num = MsgAdapter.ReadShort()   ---奇闻完成数量
    self.end_timestamp = MsgAdapter.ReadUInt()      ---大事件结束时间戳
    self.reward_fetch_status = MsgAdapter.ReadUInt()    --大事件奖励领取标记(二进制)
end

--山海宝箱信息
RA_SHAN_HAI_BAO_XIANG_MAX_ITEM= 20
SCRAShanHaiBaoXiangInfo = SCRAShanHaiBaoXiangInfo or BaseClass(BaseProtocolStruct)
function SCRAShanHaiBaoXiangInfo:__init()
    self.msg_type = 3431
end

function SCRAShanHaiBaoXiangInfo:Decode()
    self.buy_day_times = {}         -- 当日购买的次数
    self.buy_sum_times = {}         -- 总购买次数
    for i=0,RA_SHAN_HAI_BAO_XIANG_MAX_ITEM - 1 do
        self.buy_day_times[i] = MsgAdapter.ReadUShort()
    end
    for i=0,RA_SHAN_HAI_BAO_XIANG_MAX_ITEM - 1 do
        self.buy_sum_times[i] = MsgAdapter.ReadUShort()
    end
    self.flag_num = DataHelper.GetFetchFlagNum(MsgAdapter.IntFlag())
    self.has_free_reward = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
end

--山海宝盒
SCRAShanHaiBaoHeInfo = SCRAShanHaiBaoHeInfo or BaseClass(BaseProtocolStruct)
function SCRAShanHaiBaoHeInfo:__init()
	self.msg_type = 3432
end

function SCRAShanHaiBaoHeInfo:Decode()
	self.day_buy_times = {} --每天限购
	for i = 0, RA_SHAN_HAI_BAO_HE_MAX_ITEM - 1 do
		self.day_buy_times[i] = MsgAdapter.ReadUShort()
	end
	self.buy_times = {} --总限购
	for i = 0, RA_SHAN_HAI_BAO_HE_MAX_ITEM - 1 do
		self.buy_times[i] = MsgAdapter.ReadUShort()	
    end
    self.flag_num = DataHelper.GetFetchFlagNum(MsgAdapter.IntFlag())
    self.has_free_reward = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
end

--山海宝袋
SCRAShanHaiBaoDaiInfo = SCRAShanHaiBaoDaiInfo or BaseClass(BaseProtocolStruct)
function SCRAShanHaiBaoDaiInfo:__init()
	self.msg_type = 3433
end

function SCRAShanHaiBaoDaiInfo:Decode()
	self.can_buy_seq = bit:d2b(MsgAdapter.ReadInt())
	self.day_buy_times = {} --每天限购
	for i = 0, RA_SHAN_HAI_BAO_DAI_MAX_ITEM - 1 do
		self.day_buy_times[i] = MsgAdapter.ReadUShort()
	end
	self.buy_times = {} --总限购
	for i = 0, RA_SHAN_HAI_BAO_DAI_MAX_ITEM - 1 do
		self.buy_times[i] = MsgAdapter.ReadUShort()
    end
    self.flag_num = DataHelper.GetFetchFlagNum(MsgAdapter.IntFlag())
    self.has_free_reward = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
end

--云泽宝箱信息
RA_YUN_ZE_BAO_XIANG_MAX_ITEM= 20
SCRAYunZeBaoXiangInfo = SCRAYunZeBaoXiangInfo or BaseClass(BaseProtocolStruct)
function SCRAYunZeBaoXiangInfo:__init()
    self.msg_type = 3477
end

function SCRAYunZeBaoXiangInfo:Decode()
    self.buy_day_times = {}         -- 当日购买的次数
    self.buy_sum_times = {}         -- 总购买次数
    for i=0,RA_YUN_ZE_BAO_XIANG_MAX_ITEM - 1 do
        self.buy_day_times[i] = MsgAdapter.ReadUShort()
    end
    for i=0,RA_YUN_ZE_BAO_XIANG_MAX_ITEM - 1 do
        self.buy_sum_times[i] = MsgAdapter.ReadUShort()
    end
    self.flag_num = DataHelper.GetFetchFlagNum(MsgAdapter.IntFlag())
    self.has_free_reward = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
end

--云泽宝盒
SCRAYunZeBaoHeInfo = SCRAYunZeBaoHeInfo or BaseClass(BaseProtocolStruct)
function SCRAYunZeBaoHeInfo:__init()
	self.msg_type = 3478
end

function SCRAYunZeBaoHeInfo:Decode()
	self.day_buy_times = {} --每天限购
	for i = 0, RA_YUN_ZE_BAO_HE_MAX_ITEM - 1 do
		self.day_buy_times[i] = MsgAdapter.ReadUShort()
	end
	self.buy_times = {} --总限购
	for i = 0, RA_YUN_ZE_BAO_HE_MAX_ITEM - 1 do
		self.buy_times[i] = MsgAdapter.ReadUShort()	
    end
    self.flag_num = DataHelper.GetFetchFlagNum(MsgAdapter.IntFlag())
    self.has_free_reward = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
end

--云泽宝袋
SCRAYunZeBaoDaiInfo = SCRAYunZeBaoDaiInfo or BaseClass(BaseProtocolStruct)
function SCRAYunZeBaoDaiInfo:__init()
	self.msg_type = 3479
end

function SCRAYunZeBaoDaiInfo:Decode()
	self.can_buy_seq = bit:d2b(MsgAdapter.ReadInt())
	self.day_buy_times = {} --每天限购
	for i = 0, RA_YUN_ZE_BAO_DAI_MAX_ITEM - 1 do
		self.day_buy_times[i] = MsgAdapter.ReadUShort()
	end
	self.buy_times = {} --总限购
	for i = 0, RA_YUN_ZE_BAO_DAI_MAX_ITEM - 1 do
		self.buy_times[i] = MsgAdapter.ReadUShort()
    end
    self.flag_num = DataHelper.GetFetchFlagNum(MsgAdapter.IntFlag())
    self.has_free_reward = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
end