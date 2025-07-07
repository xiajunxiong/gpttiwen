---红包请求
CSHongBaoClientReq = CSHongBaoClientReq or BaseClass(BaseProtocolStruct)
function CSHongBaoClientReq:__init()
	self.msg_type = 4000
end

function CSHongBaoClientReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
	MsgAdapter.WriteInt(self.param1 or 0)
	MsgAdapter.WriteInt(self.param2 or 0)
	MsgAdapter.WriteStrN(self.greeting,HONG_BAO_GREETING)
end
--4001~~4003 需要客户端主动获取
--可发送列表信息
SCHongBaoSendList = SCHongBaoSendList or BaseClass(BaseProtocolStruct)
function SCHongBaoSendList:__init()
	self.msg_type = 4001
end

function SCHongBaoSendList:Decode()
	self.count = MsgAdapter.ReadInt()
	self.redbag_send_list = {}
	local item_index = 1
	for i = 1, self.count do
		local type = MsgAdapter.ReadInt()  --可发送的红包类型
		local count = MsgAdapter.ReadInt() 
		local day_count = MsgAdapter.ReadInt()  --每日已获得的可发红包数量
		local total_count = MsgAdapter.ReadInt()  --累计已获得的可发红包数量
		for j = 1,count do
		  local rb = {}
		  rb.index = item_index
		  rb.type = type
		  rb.day_count = day_count
		  rb.total_count = total_count
          self.redbag_send_list[item_index] = rb
		  item_index = item_index + 1
		end
	end
	self.count = item_index - 1
end

--可领取列表信息
SCHongBaoGetList = SCHongBaoGetList or BaseClass(BaseProtocolStruct)
function SCHongBaoGetList:__init()
	self.msg_type = 4002
end

function SCHongBaoGetList:Decode()
    self.hongbao_sys_type = MsgAdapter.ReadInt()  --红包所属的系统类型
	self.count = MsgAdapter.ReadInt()
	self.redbag_get_list = {}
	for i=1, self.count do
		local rb = {}
		rb.index = i
		rb.hong_bao_id = MsgAdapter.ReadInt()
		rb.send_avatar_type = MsgAdapter.ReadInt()
		rb.avatar_id = MsgAdapter.ReadInt()
		rb.send_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		self.redbag_get_list[i] = rb
	end
end


--红包详细信息  单个
SCHongBaoBaseInfo = SCHongBaoBaseInfo or BaseClass(BaseProtocolStruct)
function SCHongBaoBaseInfo:__init()
	self.msg_type = 4003
end

function SCHongBaoBaseInfo:Decode()
     self.hong_bao_id =  MsgAdapter.ReadInt()
     self.invalid_timestamp = MsgAdapter.ReadUInt()
     self.hongbao_type =  MsgAdapter.ReadInt()
     self.hongbao_sys_type =  MsgAdapter.ReadInt()
     self.send_uid =  MsgAdapter.ReadInt()
     self.send_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	 self.greeting = MsgAdapter.ReadStrN(HONG_BAO_GREETING)
     self.used_val = MsgAdapter.ReadInt()
     self.get_count = MsgAdapter.ReadInt()
     self.redbag_get_name_list = {}
     for i=1, self.get_count do
		local ri = {}
		ri.uid = MsgAdapter.ReadInt()
		ri.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
        ri.val = MsgAdapter.ReadInt()
        ri.times = MsgAdapter.ReadUInt()
		self.redbag_get_name_list[i] = ri
	end
end

--4005~~4006 服务器自动下发

-- 红包可发送列表信息变更
SCHongBaoSendListChange = SCHongBaoSendListChange or BaseClass(BaseProtocolStruct)
function SCHongBaoSendListChange:__init()
	self.msg_type = 4004
end

function SCHongBaoSendListChange:Decode()
	self.type = MsgAdapter.ReadInt()
    self.num = MsgAdapter.ReadInt()
	self.day_count = MsgAdapter.ReadInt()   --每日已获得的可发送红包数量
	self.total_count = MsgAdapter.ReadInt() --累计已获得的可发送红包数量
end

--红包可领取列表信息变更
SCHongBaoGetListChange = SCHongBaoGetListChange or BaseClass(BaseProtocolStruct)
function SCHongBaoGetListChange:__init()
	self.msg_type = 4005
end

function SCHongBaoGetListChange:Decode()
     self.change_type =  MsgAdapter.ReadInt()	--0.新的可领取的|1.有红包消失时间到了
     self.hongbao_sys_type = MsgAdapter.ReadInt()
     self.hong_bao_info = {} 
     self.hong_bao_info.hong_bao_id=MsgAdapter.ReadInt()
	 self.hong_bao_info.send_avatar_type=MsgAdapter.ReadInt()
	 self.hong_bao_info.avatar_id=MsgAdapter.ReadInt()
     self.hong_bao_info.send_name=MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
end

--红包可领取列表信息变更  单个
SCHongBaoBaseInfoChange = SCHongBaoBaseInfoChange or BaseClass(BaseProtocolStruct)
function SCHongBaoBaseInfoChange:__init()
	self.msg_type = 4006
end

function SCHongBaoBaseInfoChange:Decode()
     self.change_type =  MsgAdapter.ReadInt()	--0.有新的领取人
     self.hong_bao_id =  MsgAdapter.ReadInt()
     self.invalid_timestamp = MsgAdapter.ReadUInt()
     self.hongbao_type =  MsgAdapter.ReadInt()	--1.首充|2.充值|3.充值|4.充值|5.充值|6.占令
     self.hongbao_sys_type = MsgAdapter.ReadInt()	--0.家族红包|1.世界红包
     self.used_val =  MsgAdapter.ReadInt()
     self.get_player_info = {} --领取人的消息
     self.get_player_info.uid = MsgAdapter.ReadInt()
     self.get_player_info.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
     self.get_player_info.val = MsgAdapter.ReadInt()
     self.get_player_info.times = MsgAdapter.ReadUInt()
end

-- 天地道痕
CSTianDiDaoHenReq = CSTianDiDaoHenReq or BaseClass(BaseProtocolStruct)
function CSTianDiDaoHenReq:__init()
    self.msg_type = 4040
end

function CSTianDiDaoHenReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
end

SCTianDiDaoHenRoleInfo = SCTianDiDaoHenRoleInfo or BaseClass(BaseProtocolStruct)
function SCTianDiDaoHenRoleInfo:__init()
    self.msg_type = 4041
end

function SCTianDiDaoHenRoleInfo:Decode()
    self.info = {
        seq = MsgAdapter.ReadShort(),               -- 今天随机的配置seq
        is_fetch_reward = MsgAdapter.ReadShort(),   -- 领取4个激活后的奖励(非0已经领取)
        active_flag = MsgAdapter.ReadInt(),         -- param1 0~3(水火风地)标志 二进制
    }
end

SCTianDiDaoHenOpenInfo = SCTianDiDaoHenOpenInfo or BaseClass(BaseProtocolStruct)
function SCTianDiDaoHenOpenInfo:__init()
    self.msg_type = 4042
end

function SCTianDiDaoHenOpenInfo:Decode()
    self.info = {
        is_open = MsgAdapter.ReadInt(),
        next_state_time = MsgAdapter.ReadUInt(),
    }
end

CSGiftReq = CSGiftReq or BaseClass(BaseProtocolStruct)
function CSGiftReq:__init()
    self.msg_type = 4050
end

function CSGiftReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
	MsgAdapter.WriteInt(self.param1)
	MsgAdapter.WriteInt(self.param2)
	MsgAdapter.WriteInt(self.param3)
	MsgAdapter.WriteInt(self.param4)
	MsgAdapter.WriteStrN(self.str, SHARE_MSG)
end

SCGiftRoleInfo = SCGiftRoleInfo or BaseClass(BaseProtocolStruct)
function SCGiftRoleInfo:__init()
    self.msg_type = 4051
end

function SCGiftRoleInfo:Decode()
	self.info_list = {}
    self.count = MsgAdapter.ReadInt()
	for i = 1, self.count do
		local role_id = MsgAdapter.ReadInt()
		local type = MsgAdapter.ReadShort()
		local num = MsgAdapter.ReadUShort()
		self.info_list[type] = self.info_list[type] or {}
		self.info_list[type][role_id] = num
	end
end

SCGetGiftInfo = SCGetGiftInfo or BaseClass(BaseProtocolStruct)
function SCGetGiftInfo:__init()
    self.msg_type = 4052
end

function SCGetGiftInfo:Decode()
	self.type = MsgAdapter.ReadInt()
	self.count = MsgAdapter.ReadInt()
	self.info_list = {}
	for i = 1, self.count do
		local uuid = MsgAdapter.ReadInt2()
		local uuid_key = uuid[1] .. uuid[2]
		local t = {
			uuid = uuid,
			from_role_id = MsgAdapter.ReadInt(),
			to_role_id = MsgAdapter.ReadInt(),
			fetch_time = MsgAdapter.ReadUInt(),
			send_time = MsgAdapter.ReadUInt(),
			str = MsgAdapter.ReadStrN(64),
			item_id = MsgAdapter.ReadUShort(),
			num = MsgAdapter.ReadUShort(),
			invaild_time = MsgAdapter.ReadUInt(),
			time_group_id = MsgAdapter.ReadInt(),
			uuid_key = uuid_key,
		}
		t.item = Item.Init(t.item_id, t.num)
		t.gift_item_type = SocietyData.Instance.GetGiftItemType(t.item)
		-- t.fetch_time = TimeHelper.GetServerTime()
		self.info_list[uuid_key] = t
	end
end

-- 珍品行铺
SCRAZhenPinShangPuInfo = SCRAZhenPinShangPuInfo or BaseClass(BaseProtocolStruct)
function SCRAZhenPinShangPuInfo:__init()
    self.msg_type = 4080
end

function SCRAZhenPinShangPuInfo:Decode()
    self.today_buy_times = {}
    for i = 0,99 do 
		local tb = {}
		tb.type = MsgAdapter.ReadShort()
		tb.buy_times = MsgAdapter.ReadShort()
        self.today_buy_times[i] =  tb
    end 
    self.activity_buy_times = {}
    for i = 0,99 do 
		local tb = {}
		tb.type = MsgAdapter.ReadShort()
		tb.buy_times = MsgAdapter.ReadShort()
        self.activity_buy_times[i] = tb
    end
end
SCRAZhenPinShangPuOpenDay = SCRAZhenPinShangPuOpenDay or BaseClass(BaseProtocolStruct)
function SCRAZhenPinShangPuOpenDay:__init()
    self.msg_type = 4081
end

function SCRAZhenPinShangPuOpenDay:Decode()
	self.day = MsgAdapter.ReadInt()
end



-- 伙伴元神契约仓库通知
SCPartnerSpiritPactList = SCPartnerSpiritPactList or BaseClass(BaseProtocolStruct)
function SCPartnerSpiritPactList:__init()
    self.msg_type = 4010
end

function SCPartnerSpiritPactList:Decode()
	self.type = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
	self.item_list = {}
	for i = 1, self.count do
		local save_index = MsgAdapter.ReadShort()
		local t = {
			save_index = save_index,			--储存位置
			item_id = MsgAdapter.ReadUShort(),	--物品id
			num = MsgAdapter.ReadInt(),			--物品数量
		}
		local item = Item.Init(t.item_id, t.num)
		item.show_stars = true
		t.item = item
		self.item_list[save_index] = t
	end
end

-- 伙伴元神契约 契约用 属性通知
SCPartnerPactBaseAttr = SCPartnerPactBaseAttr or BaseClass(BaseProtocolStruct)
function SCPartnerPactBaseAttr:__init()
    self.msg_type = 4011
end

function SCPartnerPactBaseAttr:Decode()
	self.partner = MsgAdapter.ReadInt()
	self.contract_attrs = {}   --  改伙伴用于给其契约目标增加的属性
    for i = 0, PartnerConfig.PARTNER_CONTRACT_ATTR_MAX - 1 do
        self.contract_attrs[i] = {
            attr_type = PartnerConfig.ContractI2Type[i],
            attr_value = MsgAdapter.ReadInt(),
        }
    end
end

-- 伙伴元神契约 契约改变
SCPartnerPactInfo = SCPartnerPactInfo or BaseClass(BaseProtocolStruct)
function SCPartnerPactInfo:__init()
    self.msg_type = 4012
end

function SCPartnerPactInfo:Decode()
	self.partner = MsgAdapter.ReadInt()
	self.type = MsgAdapter.ReadInt()	-- 0:普通 1:升星param=位置
	self.contracts = {}
    for i = 1, PartnerConfig.PARTNER_CONTRACT_MAX do
        self.contracts[i] = MsgAdapter.ReadUShort()
    end
    self.param = MsgAdapter.ReadShort()
end


SCTianMingInfo = SCTianMingInfo or BaseClass(BaseProtocolStruct)
function SCTianMingInfo:__init()
	self.msg_type = 4015
end

function SCTianMingInfo:Decode()
	self.level = MsgAdapter.ReadInt()
	self.exp = MsgAdapter.ReadInt()

	self.sys_type_level = {}
	for i = 0, TIAN_MING_ADD_ATTR_SYS_TYPE.MAX do 
		local vo = MsgAdapter.ReadInt()
		table.insert(self.sys_type_level,vo)
	end 
end

-- 福牛报喜
CSCowReportGoodNewsClientReq = CSCowReportGoodNewsClientReq or BaseClass(BaseProtocolStruct)
function CSCowReportGoodNewsClientReq:__init()
    self.msg_type = 4090
end

function CSCowReportGoodNewsClientReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
end

SCCowReportGoodNewsRewardListInfo = SCCowReportGoodNewsRewardListInfo or BaseClass(BaseProtocolStruct)
function SCCowReportGoodNewsRewardListInfo:__init()
	self.msg_type = 4091
end

function SCCowReportGoodNewsRewardListInfo:Decode()
	self.user_info = {}
	for i = 1, ACT_LUCKY_COW_MAX_USER do 
		local t = {}
		t.uid = MsgAdapter.ReadInt()
		t.avatar_type = MsgAdapter.ReadShort()
		t.avatar_id = MsgAdapter.ReadShort()
		t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		table.insert(self.user_info, t)
	end 
end

SCCowReportGoodNewsInfo = SCCowReportGoodNewsInfo or BaseClass(BaseProtocolStruct)
function SCCowReportGoodNewsInfo:__init()
	self.msg_type = 4092
end

function SCCowReportGoodNewsInfo:Decode()
	self.status = MsgAdapter.ReadInt()
	self.time = {}
	for i = 1, 4 do
		self.time[i] = MsgAdapter.ReadUInt()
	end
end

SCCowReportGoodNewsRoleBuyInfo = SCCowReportGoodNewsRoleBuyInfo or BaseClass(BaseProtocolStruct)
function SCCowReportGoodNewsRoleBuyInfo:__init()
	self.msg_type = 4093
end

function SCCowReportGoodNewsRoleBuyInfo:Decode()
	self.is_buy = MsgAdapter.ReadInt()
end

-- 寅虎纳福
CSTigerBringsBlessingClientReq = CSTigerBringsBlessingClientReq or BaseClass(BaseProtocolStruct)
function CSTigerBringsBlessingClientReq:__init()
    self.msg_type = 4094
end

function CSTigerBringsBlessingClientReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
end

SCTigerBringsBlessingRewardListInfo = SCTigerBringsBlessingRewardListInfo or BaseClass(BaseProtocolStruct)
function SCTigerBringsBlessingRewardListInfo:__init()
	self.msg_type = 4095
end

function SCTigerBringsBlessingRewardListInfo:Decode()
	self.user_info = {}
	for i = 1, ACT_LUCKY_COW_MAX_USER do 
		local t = {}
		t.uid = MsgAdapter.ReadInt()
		t.avatar_type = MsgAdapter.ReadShort()
		t.headshot_id = MsgAdapter.ReadShort()
		t.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		table.insert(self.user_info, t)
	end 
end

SCTigerBringsBlessingInfo = SCTigerBringsBlessingInfo or BaseClass(BaseProtocolStruct)
function SCTigerBringsBlessingInfo:__init()
	self.msg_type = 4096
end

function SCTigerBringsBlessingInfo:Decode()
	self.status = MsgAdapter.ReadInt()
	self.time = {}
	for i = 1, 4 do
		self.time[i] = MsgAdapter.ReadUInt()
	end
end

SCTigerBringsBlessingRoleBuyInfo = SCTigerBringsBlessingRoleBuyInfo or BaseClass(BaseProtocolStruct)
function SCTigerBringsBlessingRoleBuyInfo:__init()
	self.msg_type = 4097
end

function SCTigerBringsBlessingRoleBuyInfo:Decode()
	self.is_buy = MsgAdapter.ReadInt()
end