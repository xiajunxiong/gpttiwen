-- 四季宝盒
SCRAFourSeasonBoxInfo = SCRAFourSeasonBoxInfo or BaseClass(BaseProtocolStruct)
function SCRAFourSeasonBoxInfo:__init()
	self.msg_type = 5200
end

function SCRAFourSeasonBoxInfo:Decode()
    local info = {}
	info.day_buy_times = {} --每天限购
	for i = 0, RA_FOUR_SEASON_BOX_MAX_ITEM - 1 do
		info.day_buy_times[i] = MsgAdapter.ReadUShort()
	end
	info.act_buy_times = {} --总限购
	for i = 0, RA_FOUR_SEASON_BOX_MAX_ITEM - 1 do
		info.act_buy_times[i] = MsgAdapter.ReadUShort()	
    end
    info.free_flag = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
    self.info = info
end

-- 四季宝箱
SCRAFourSeasonChestInfo = SCRAFourSeasonChestInfo or BaseClass(BaseProtocolStruct)
function SCRAFourSeasonChestInfo:__init()
    self.msg_type = 5210
end

function SCRAFourSeasonChestInfo:Decode()
    local info = {}
	info.day_buy_times = {} --每天限购
	for i = 0, RA_FOUR_SEASON_CHEST_MAX_ITEM - 1 do
		info.day_buy_times[i] = MsgAdapter.ReadUShort()
	end
	info.act_buy_times = {} --总限购
	for i = 0, RA_FOUR_SEASON_CHEST_MAX_ITEM - 1 do
		info.act_buy_times[i] = MsgAdapter.ReadUShort()	
    end
    info.free_flag = MsgAdapter.ReadInt() --免费宝箱，0未领取，1已领取
    self.info = info
end

-- 新服灵宠
SCRAXingFuLingChongInfo = SCRAXingFuLingChongInfo or BaseClass(BaseProtocolStruct)
function SCRAXingFuLingChongInfo:__init()
    self.msg_type = 5220
end

function SCRAXingFuLingChongInfo:Decode()
    self.buy_count = MsgAdapter.ReadInt()
end

SCPetLianYaoCommonInfo = SCPetLianYaoCommonInfo or BaseClass(BaseProtocolStruct)
function SCPetLianYaoCommonInfo:__init()
	self.msg_type = 5230
end
function SCPetLianYaoCommonInfo:Decode()
	self.is_unlock_lian_yao_furance_flag = MsgAdapter.IntFlag(nil)
	-- LogDG(self)
end

--魂器返利
SCRAHunQiFanLiInfo = SCRAHunQiFanLiInfo or BaseClass(BaseProtocolStruct)
function SCRAHunQiFanLiInfo:__init()
	self.msg_type = 5222
end

function SCRAHunQiFanLiInfo:Decode()
	local t = {}
	t.remain_count = MsgAdapter.ReadUInt() --玩家剩余抽奖次数
	t.all_count = MsgAdapter.ReadInt()       --玩家总抽奖次数
	t.today_recharge = MsgAdapter.ReadInt() 		--当日抽奖金额

	t.recharge_reward_flag = MsgAdapter.IntFlag() --充值奖励领取flag
	t.lottery_reward_flag = MsgAdapter.IntFlag()  --抽奖次数奖励领取flag
	t.item_flag = MsgAdapter.IntFlag() --当前轮盘状态 1为已抽到
	self.info = t
	--LOG("魂器返利协议", self)
end

-- 天衣耀世
SCRATianYiYaoShiInfo = SCRATianYiYaoShiInfo or BaseClass(BaseProtocolStruct)
function SCRATianYiYaoShiInfo:__init()
	self.msg_type = 5221
end

function SCRATianYiYaoShiInfo:Decode()
	local t = {}
	local cur_log_tail = MsgAdapter.ReadUInt() --// 当前日志尾部(下一条要被替换的index，用于排序)， 暂时用不到，用时间戳排序吧 
	t.buy_times_list = {}	--// 购买礼包次数
	for i = 0, 9 do
		t.buy_times_list[i] = MsgAdapter.ReadUShort()
	end
	t.log_list = {}			--// 抽奖日志
	for i = 1, 50 do
		local info = {}
		info.time = MsgAdapter.ReadUInt()
		info.item_id = MsgAdapter.ReadUShort()
		info.num = MsgAdapter.ReadUShort()
		if info.time > 0 then
			table.insert(t.log_list, info)
		end
	end
	table.sort(t.log_list, function (a, b)
		return a.time > b.time
	end)
	self.info = t
end

-- 神赐华服活动信息下发
SCRAShenCiHuaFuInfo = SCRAShenCiHuaFuInfo or BaseClass(BaseProtocolStruct)
function SCRAShenCiHuaFuInfo:__init()
    self.msg_type = 5225
end

function SCRAShenCiHuaFuInfo:Decode()
    local info = {}
	info.day_buy_times = {} --每天限购
	for i = 0, RA_GOD_CLOTHES_MAX_BUY_ITEM - 1 do
		info.day_buy_times[i] = MsgAdapter.ReadUShort()
	end
	info.act_buy_times = {} --总限购
	for i = 0, RA_GOD_CLOTHES_MAX_BUY_ITEM - 1 do
		info.act_buy_times[i] = MsgAdapter.ReadUShort()	
	end
	info.day_exchange_times = {} --每天限兑
	for i = 0, RA_GOD_CLOTHES_MAX_EXCHANGE_ITEM - 1 do
		info.day_exchange_times[i] = MsgAdapter.ReadUShort()
	end
	info.act_exchange_times = {} --总限兑
	for i = 0, RA_GOD_CLOTHES_MAX_EXCHANGE_ITEM - 1 do
		info.act_exchange_times[i] = MsgAdapter.ReadUShort()	
    end
	local item_select = {}	--自选奖励
	for i = 0, 2 do
		item_select[i] = GodClothesData.Decode(GodClothesItemType.Select, i)
	end
	info.item_select = item_select
	local item_rand = {}	--随机奖励
	for i = 0, 5 do
		item_rand[i] = GodClothesData.Decode(GodClothesItemType.Rand, i)
	end
	info.item_rand = item_rand
    self.info = info
end

-- 神赐华服抽奖信息下发
SCRAShenCiHuaFuResultInfo = SCRAShenCiHuaFuResultInfo or BaseClass(BaseProtocolStruct)
function SCRAShenCiHuaFuResultInfo:__init()
    self.msg_type = 5226
end

function SCRAShenCiHuaFuResultInfo:Decode()
	self.count = MsgAdapter.ReadInt()
	self.item_list = {}
	for i = 1, self.count do
		local t = {}
		t.m_type = MsgAdapter.ReadInt()	--0是自选，1是随机
		t.index = MsgAdapter.ReadInt()	--对应当前自选和随机奖励的数组下标
		table.insert(self.item_list, t)
	end
end

-- 宠物添加经验通知
SCPetAddExpNotice = SCPetAddExpNotice or BaseClass(BaseProtocolStruct)
function SCPetAddExpNotice:__init()
	self.msg_type = 5231
end

function SCPetAddExpNotice:Decode()
	self.notice_type = MsgAdapter.ReadInt()
	self.pet_index = MsgAdapter.ReadInt()
end