--结义请求
CSJieYiReq = CSJieYiReq or BaseClass(BaseProtocolStruct)
function CSJieYiReq:__init()
    self.msg_type = 4200
end

function CSJieYiReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    if type(self.param1) == "table" then
        MsgAdapter.WriteUInt2(self.param1)
    else
        MsgAdapter.WriteLL(self.param1)
    end
    MsgAdapter.WriteInt(self.param2)
    -- local data = {
    --     req_type = self.req_type,
    --     param1 = self.param1,
    --     param2 = self.param2,
    -- }
    --LOG("结义请求(4200)   req_type->" .. self.req_type .. "     param -> ", data)
end

--开始创建结义组
CSJieYiCreate = CSJieYiCreate or BaseClass(BaseProtocolStruct)
function CSJieYiCreate:__init()
    self.msg_type = 4201
end

function CSJieYiCreate:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.str1, SWORN_PREFIX_NAME_LEN)
    MsgAdapter.WriteStrN(self.str2, SWORN_SUFFIX_NAME_LEN)
    MsgAdapter.WriteInt(self.member_num)
    -- LogError("结义创建(4201)   str1->" .. self.str1 .. "     str2-> " .. self.str2 .. "   num->" .. self.member_num)
end

--组长命名更新
CSJieYiCreateName = CSJieYiCreateName or BaseClass(BaseProtocolStruct)
function CSJieYiCreateName:__init()
    self.msg_type = 4202
end

function CSJieYiCreateName:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.key)
    MsgAdapter.WriteStrN(self.str1, SWORN_PREFIX_NAME_LEN)
    MsgAdapter.WriteStrN(self.str2, SWORN_SUFFIX_NAME_LEN)
    MsgAdapter.WriteInt(self.member_num)
    -- LogError("命名改变(4202)    key->" .. self.key .. "   str1->" .. self.str1 .. "     str2-> " .. self.str2 .. "   num->" .. self.member_num)
end

--正在创建的组信息
SCJieYiCreateInfo = SCJieYiCreateInfo or BaseClass(BaseProtocolStruct)
function SCJieYiCreateInfo:__init()
    self.msg_type = 4203
end

function SCJieYiCreateInfo:Decode()
    self.key = MsgAdapter.ReadInt()
    self.is_name_finish = MsgAdapter.ReadInt()
    self.jie_yi_name = PortocolCommon.SwornName()
    self.role_count = MsgAdapter.ReadInt()
    self.role_info = {}
    for i = 1, self.role_count do --=JieYiZuCreateRole
        local t = PortocolCommon.MsgRoleBaseInfo({})
        t.is_confirm = MsgAdapter.ReadShort()
        t.is_zu_zhang = MsgAdapter.ReadShort()
        self.role_info[i] = t
    end
    -- LogError("结义组信息下发(4203) -> ", self)
end

--创建名字更改，其他人收到
SCJieYiCreateNameUpdate = SCJieYiCreateNameUpdate or BaseClass(BaseProtocolStruct)
function SCJieYiCreateNameUpdate:__init()
    self.msg_type = 4204
end

function SCJieYiCreateNameUpdate:Decode()
    self.key = MsgAdapter.ReadInt()
    self.jie_yi_name = PortocolCommon.SwornName()
    -- LogError("接受消息(4204)  key->" .. self.key .. "   name->", self.jie_yi_name)
end

--创建确认，其他人收到
SCJieYiCreateConfirm = SCJieYiCreateConfirm or BaseClass(BaseProtocolStruct)
function SCJieYiCreateConfirm:__init()
    self.msg_type = 4205
end

function SCJieYiCreateConfirm:Decode()
    self.key = MsgAdapter.ReadInt()
    self.role_id = {} --已确认玩家
    for i = 1, 5 do
        self.role_id[i] = MsgAdapter.ReadInt()
    end
    -- LogError("开始确认(4205)  key->" .. self.key .. "   role_id->", self.role_id)
end

--有人取消操作，其他人收到
SCJieYiCreateCancel = SCJieYiCreateCancel or BaseClass(BaseProtocolStruct)
function SCJieYiCreateCancel:__init()
    self.msg_type = 4206
end

function SCJieYiCreateCancel:Decode()
    self.type = MsgAdapter.ReadInt()    --0是取消，1是掉线
    self.key = MsgAdapter.ReadInt()
    self.role_id = MsgAdapter.ReadInt()
    -- LogError("取消确认(4206)  key->" .. self.key .. "   role_id->" .. self.role_id .. "   type-> " .. self.type)
end

--结义组信息
SCJieYiZuInfo = SCJieYiZuInfo or BaseClass(BaseProtocolStruct)
function SCJieYiZuInfo:__init()
    self.msg_type = 4207
end

function SCJieYiZuInfo:Decode()
    self.info_type = MsgAdapter.ReadInt() --1是查询
    self.jie_yi_zu_id = MsgAdapter.ReadUInt2()
    self.create_time = MsgAdapter.ReadLL()
    self.jie_yi_name = PortocolCommon.SwornName()
    self.jie_yi_text = MsgAdapter.ReadStrN(JIE_YI_ZU_TEXT_LENGTH)
    self.vote_end_time = MsgAdapter.ReadUInt() -- 投票结束时间

    self.role_info_count = MsgAdapter.ReadInt()
    self.role_info = {}
    for i = 1, self.role_info_count do --=RoleInfo
        local t = PortocolCommon.MsgRoleBaseInfo({})
        t.is_leader = MsgAdapter.ReadShort() --是否组长
        t.has_first_member_name = MsgAdapter.ReadShort() --是否已经改过成员名
        t.join_time = MsgAdapter.ReadUInt() --加入时间
        t.level_end_time = MsgAdapter.ReadUInt() --申请离开时间戳
        t.member_name = PortocolCommon.SwornMemberName()
        t.last_login_time = MsgAdapter.ReadUInt() --离线时间
        t.online_state = MsgAdapter.ReadInt() --在线状态
        t.is_vote = MsgAdapter.ReadShort()  -- 这个成员是否已经投票了
        t.votes_num = MsgAdapter.ReadShort()    -- 获得投票次数
        table.insert(self.role_info, t)
    end
    --LOG("结义组信息(4207) -> ", self)
end

--成员改名
CSJieYiZuMemberName = CSJieYiZuMemberName or BaseClass(BaseProtocolStruct)
function CSJieYiZuMemberName:__init()
    self.msg_type = 4208
end

function CSJieYiZuMemberName:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.member_name, SWORN_ROLE_NAME_LEN)
    -- LogError("成员请求改名(4208)  member_name->" .. self.member_name)
end

--组长修改结义宣言
CSJieYiZuText = CSJieYiZuText or BaseClass(BaseProtocolStruct)
function CSJieYiZuText:__init()
    self.msg_type = 4209
end

function CSJieYiZuText:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.text, JIE_YI_ZU_TEXT_LENGTH)
    -- LogError("组长修改结义宣言(4209)  text->" .. self.text)
end

--组长修改结义名字
CSJieYiZuName = CSJieYiZuName or BaseClass(BaseProtocolStruct)
function CSJieYiZuName:__init()
    self.msg_type = 4210
end

function CSJieYiZuName:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteStrN(self.str1, SWORN_PREFIX_NAME_LEN)
    MsgAdapter.WriteStrN(self.str2, SWORN_SUFFIX_NAME_LEN)
    MsgAdapter.WriteInt(self.member_num)
    -- LogError("组长改结义名字(4210)  str1->" .. self.str1 .. "     str2-> " .. self.str2 .. "   num->" .. self.member_num)
end

--邀请人时，被邀请的人收到
SCJieYiInviteRoute = SCJieYiInviteRoute or BaseClass(BaseProtocolStruct)
function SCJieYiInviteRoute:__init()
    self.msg_type = 4211
end

function SCJieYiInviteRoute:Decode()
    self.jie_yi_zu_id = MsgAdapter.ReadUInt2()
    self.uid = MsgAdapter.ReadInt()
    -- LogError("被邀请(4211) uid->" .. self.uid .. "    zu_id->", self.jie_yi_zu_id)
end

--自荐宣言
SCJieYiRoleXuanYanList = SCJieYiRoleXuanYanList or BaseClass(BaseProtocolStruct)
function SCJieYiRoleXuanYanList:__init()
    self.msg_type = 4212
end

function SCJieYiRoleXuanYanList:Decode()
    self.count = MsgAdapter.ReadInt()
    self.xuan_yan_items = {}
    local function func_XuanYanItem()
        local t = {}
        PortocolCommon.MsgRoleBaseInfo(t)
        t.text = MsgAdapter.ReadStrN(JIE_YI_ZU_XUAN_YAN_TEXT_LENGTH)
        return t
    end
    for i = 1, self.count do
        self.xuan_yan_items[i] = func_XuanYanItem()
    end
    -- LogError("自荐宣言(4212)  count->" .. self.count .. "   xuan_yan_items->", self.xuan_yan_items)
end

--纳新宣言
SCJieYiZuXuanYanList = SCJieYiZuXuanYanList or BaseClass(BaseProtocolStruct)
function SCJieYiZuXuanYanList:__init()
    self.msg_type = 4213
end

function SCJieYiZuXuanYanList:Decode()
    self.count = MsgAdapter.ReadInt()
    self.xuan_yan_items = {}
    local function func_XuanYanItem()
        local t = {}
        t.jie_yi_zu_id = MsgAdapter.ReadUInt2()
        t.zu_name = PortocolCommon.SwornName()
        t.text = MsgAdapter.ReadStrN(JIE_YI_ZU_XUAN_YAN_TEXT_LENGTH)
        return t
    end
    for i = 1, self.count do
        self.xuan_yan_items[i] = func_XuanYanItem()
    end
    -- LogError("纳新宣言(4213)  count->" .. self.count .. "   xuan_yan_items->", self.xuan_yan_items)
end

--请求编辑宣言
CSJieYiZuXuanYanReq = CSJieYiZuXuanYanReq or BaseClass(BaseProtocolStruct)
function CSJieYiZuXuanYanReq:__init()
    self.msg_type = 4214
end

function CSJieYiZuXuanYanReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteShort(self.type) --0自荐 1纳新
    MsgAdapter.WriteShort(self.is_delete) --是否删除
    MsgAdapter.WriteStrN(self.text, JIE_YI_ZU_XUAN_YAN_TEXT_LENGTH)
    -- LogError("请求编辑宣言(4214)    type->" .. self.type .. "     is_delete -> " .. self.is_delete .. "   text->" .. self.text)
end

--请求宣言列表
CSJieYiZuXuanYanReqList = CSJieYiZuXuanYanReqList or BaseClass(BaseProtocolStruct)
function CSJieYiZuXuanYanReqList:__init()
    self.msg_type = 4215
end

function CSJieYiZuXuanYanReqList:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.type) --0自荐 1纳新
    -- LogError("请求宣言列表(4215)    type->" .. self.type)
end

--结义组离开通知
SCJieYiZuLeaveNotice = SCJieYiZuLeaveNotice or BaseClass(BaseProtocolStruct)
function SCJieYiZuLeaveNotice:__init()
    self.msg_type = 4216
end

function SCJieYiZuLeaveNotice:Decode()
    self.jie_yi_zu_id = MsgAdapter.ReadUInt2()
    -- LogError("离开结义组通知")
end

--结义组成员在线状态改变
SCJieYiZuRoleOnlineStatusNotice = SCJieYiZuRoleOnlineStatusNotice or BaseClass(BaseProtocolStruct)
function SCJieYiZuRoleOnlineStatusNotice:__init()
    self.msg_type = 4217
end

function SCJieYiZuRoleOnlineStatusNotice:Decode()
    self.uid = MsgAdapter.ReadInt()
    self.last_login_time = MsgAdapter.ReadUInt()
    self.online_state = MsgAdapter.ReadInt()
end


CSWangLingFBReq = CSWangLingFBReq or BaseClass(BaseProtocolStruct)
function CSWangLingFBReq:__init()
    self.msg_type = 4230
end
function CSWangLingFBReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)--0 完成任务 1 设置领取标志 p1 index p2 0可领 1不可领
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    -- LogDG(self)
end

SCWangLingFBRoleInfo = SCWangLingFBRoleInfo or BaseClass(BaseProtocolStruct)
function SCWangLingFBRoleInfo:__init()
    self.msg_type = 4231
    self.max_boss_num = 4
end
function SCWangLingFBRoleInfo:Decode()
    self.day_fetch_battle_reward = MsgAdapter.ReadInt()--今日领取战斗奖励次数
    self.day_fetch_pass_reward = MsgAdapter.ReadInt()--今日领取通关奖励次数
    self.fb_fetch_flag = bit:d2b(MsgAdapter.ReadInt()) -- 副本领奖标记[副本index]
    self.boss_reward_times = {}
    for i = 1, self.max_boss_num, 1 do
        self.boss_reward_times[i] = MsgAdapter.ReadShort()
    end
    LogDG(self)
end
SCWanLingFBInfo = SCWanLingFBInfo or BaseClass(BaseProtocolStruct)
function SCWanLingFBInfo:__init()
    self.msg_type = 4232
end
function SCWanLingFBInfo:Decode()
   self.fb_index = MsgAdapter.ReadInt()--副本index
   self.is_finish = MsgAdapter.ReadInt()--副本完成标记
   self.task_seq = MsgAdapter.ReadInt()--当前任务
   LogDG(self)
end

--婚宴 玩家操作
CSWeddingFBReq = CSWeddingFBReq or BaseClass(BaseProtocolStruct)
function CSWeddingFBReq:__init()
    self.msg_type = 4240
end

function CSWeddingFBReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
end

--婚宴 副本信息
SCWeddingFBSceneInfo = SCWeddingFBSceneInfo or BaseClass(BaseProtocolStruct)
function SCWeddingFBSceneInfo:__init()
    self.msg_type = 4241
end

function SCWeddingFBSceneInfo:Decode()
    self.info = {
        is_finish = MsgAdapter.ReadInt(),       -- 副本完成标记
        wedding_id = MsgAdapter.ReadInt(),      -- 婚宴id
        fb_begin_time = MsgAdapter.ReadInt(),   -- 副本起始时间
        fb_end_time = MsgAdapter.ReadInt(),     -- 副本结束时间
        link_seq = MsgAdapter.ReadInt(),        -- 当前环节
        link_begin_time = MsgAdapter.ReadUInt(),-- 环节起始时间
        link_end_time = MsgAdapter.ReadUInt(),  -- 环节结束时间
        wedding_heat = MsgAdapter.ReadInt(),    -- 婚宴热度
        -- is_host = MsgAdapter.ReadInt(),         -- 是否是主办者
        xun_you_type = MsgAdapter.ReadInt(),    -- 巡游类型
        host_uid1 = MsgAdapter.ReadInt(),       -- 新郎uid
        host_uid2 = MsgAdapter.ReadInt(),       -- 新娘uid
        host_name_xl = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),    -- 新郎名字
        host_name_xn = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),    -- 新娘名字
        host_avatar_type_xl = MsgAdapter.ReadInt(), -- 新郎形象
        host_avatar_type_xn = MsgAdapter.ReadInt(), -- 新娘形象
    }
    local role_id = RoleData.Instance:GetRoleId()
    self.info.is_host = (self.info.host_uid1 == role_id or self.info.host_uid2 == role_id) and 1 or 0
end

--婚宴 副本角色信息
SCWeddingFBRoleInfo = SCWeddingFBRoleInfo or BaseClass(BaseProtocolStruct)
function SCWeddingFBRoleInfo:__init()
    self.msg_type = 4242
end

function SCWeddingFBRoleInfo:Decode()
    local meal_last_fetch_time = MsgAdapter.ReadUInt()
    self.count = MsgAdapter.ReadInt()
    self.common_param = {}
    for i = 0, self.count - 1 do
        self.common_param[i] = MsgAdapter.ReadInt()
    end
    self.common_param.meal_last_fetch_time = meal_last_fetch_time
end

--婚宴 场景广播
SCWeddingFBSync = SCWeddingFBSync or BaseClass(BaseProtocolStruct)
function SCWeddingFBSync:__init()
    self.msg_type = 4243
end

function SCWeddingFBSync:Decode()
    self.sync_type = MsgAdapter.ReadInt()
    self.param = MsgAdapter.ReadInt()
end

--婚宴 展示信息(邀请函)
SCWeddingShowInfo = SCWeddingShowInfo or BaseClass(BaseProtocolStruct)
function SCWeddingShowInfo:__init()
    self.msg_type = 4244
end

function SCWeddingShowInfo:Decode()
    local t = {
        wedding_id = MsgAdapter.ReadInt(),      -- 婚宴id
        begin_time = MsgAdapter.ReadUInt(),     -- 开启时间
        status = MsgAdapter.ReadInt(),          -- 0无 1已申请 2已被邀请
        host_count = MsgAdapter.ReadInt(),
        host_list = {},
        hosts = {},
    }
    for i = 1, t.host_count do
        local uid = MsgAdapter.ReadInt()
        t.host_list[i] = {
            uid = uid,
            name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
            appearance = Appearance(),
        }
        t.hosts[uid] = true
    end
    self.info = t
end

--婚宴 预约列表
SCWeddingRegistList = SCWeddingRegistList or BaseClass(BaseProtocolStruct)
function SCWeddingRegistList:__init()
    self.msg_type = 4245
end

function SCWeddingRegistList:Decode()
    local t = {
        regist_time = MsgAdapter.ReadUInt(),        -- 0:未预约
        count = MsgAdapter.ReadInt(),
        regist_time_list = {}
    }
    for i = 1, t.count do
        local regist_time = MsgAdapter.ReadUInt()
        t.regist_time_list[regist_time] = true
    end
    self.info = t
end

--婚宴 邀请列表
SCWeddingInviteList = SCWeddingInviteList or BaseClass(BaseProtocolStruct)
function SCWeddingInviteList:__init()
    self.msg_type = 4246
end

function SCWeddingInviteList:Decode()
    local t = {}
    t.wedding_id = MsgAdapter.ReadInt()             -- 婚宴id
    t.wedding_begin_time = MsgAdapter.ReadInt()     -- 开启时间
    t.buy_invite_time = MsgAdapter.ReadInt()        -- 已拓展次数
    t.host_uid1 = MsgAdapter.ReadInt()
    t.host_uid2 = MsgAdapter.ReadInt()
    t.hosts = {}
    t.hosts[t.host_uid1] = true
    t.hosts[t.host_uid2] = true
    
    self.is_total = MsgAdapter.ReadInt()            -- 1全部 0新增
    self.count = MsgAdapter.ReadInt()
    self.invite_list = {}
    for i = 1, self.count do
        local uid = MsgAdapter.ReadInt()
        self.invite_list[uid] = {
            uid = uid,
            name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        }
    end
    self.info = t
end

--婚宴 申请列表
SCWeddingApplyList = SCWeddingApplyList or BaseClass(BaseProtocolStruct)
function SCWeddingApplyList:__init()
    self.msg_type = 4247
end

function SCWeddingApplyList:Decode()
    self.is_total = MsgAdapter.ReadInt()            -- 1全部 0新增
    self.count = MsgAdapter.ReadInt()
    self.apply_list = {}
    for i = 1, self.count do
        local uid = MsgAdapter.ReadInt()
        self.apply_list[uid] = {
            uid = uid,
            name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN),
        }
    end
end

--婚宴 通知
SCWeddingChange = SCWeddingChange or BaseClass(BaseProtocolStruct)
function SCWeddingChange:__init()
    self.msg_type = 4248
end

function SCWeddingChange:Decode()
    self.change_type = MsgAdapter.ReadInt()
    self.param_1 = MsgAdapter.ReadInt()
end

-- 婚宴-开启广播
SCWeddingStatusSync = SCWeddingStatusSync or BaseClass(BaseProtocolStruct)
function SCWeddingStatusSync:__init()
    self.msg_type = 4250
end

function SCWeddingStatusSync:Decode()
    local t = {
        wedding_id = MsgAdapter.ReadInt(),      -- 婚宴id
        begin_time = MsgAdapter.ReadUInt(),     -- 开启时间
    }
    self.info = t
end

SCWeddingSubActBaoWeiXiuQiu = SCWeddingSubActBaoWeiXiuQiu or BaseClass(BaseProtocolStruct)
function SCWeddingSubActBaoWeiXiuQiu:__init()
    self.msg_type = 4253
end

function SCWeddingSubActBaoWeiXiuQiu:Decode()
    self.count = MsgAdapter.ReadInt()
    self.obj_info = {}
    for i = 1, self.count do
        local npc_id = MsgAdapter.ReadInt()
        local is_fetch = MsgAdapter.ReadInt()       -- 奖励已领取(已击败)
        self.obj_info[npc_id] = 0 == is_fetch
    end
end

SCWeddingSubActZaoShengGuiZi = SCWeddingSubActZaoShengGuiZi or BaseClass(BaseProtocolStruct)
function SCWeddingSubActZaoShengGuiZi:__init()
    self.msg_type = 4254
end

function SCWeddingSubActZaoShengGuiZi:Decode()
    self.fetch_flag = MsgAdapter.ReadInt()
end

--====================情缘系统========================
CSMarriageReq = CSMarriageReq or BaseClass(BaseProtocolStruct)

function CSMarriageReq:__init()
    self.msg_type = 4260
end

function CSMarriageReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.param1)
    MsgAdapter.WriteInt(self.param2)
    -- LogError(self)
end

-------------------求婚信息转发-------------------
SCMarryReqRoute = SCMarryReqRoute or BaseClass(BaseProtocolStruct)

function SCMarryReqRoute:__init()
    self.msg_type = 4261
end

function SCMarryReqRoute:Decode()
    self.marry_type = MsgAdapter.ReadInt()
    self.inviter_role_id = MsgAdapter.ReadInt() --被邀请方
    self.role_id = MsgAdapter.ReadInt()--邀请方
end

-------------------结婚信息-------------------
SCMarryInfo = SCMarryInfo or BaseClass(BaseProtocolStruct)

function SCMarryInfo:__init()
    self.msg_type = 4262
end

function SCMarryInfo:Decode()
    self.role_info = {}
    self.marry_id = MsgAdapter.ReadLL()                            --情缘组id 暂时无用
    self.create_time = MsgAdapter.ReadUInt()                       --创建时间 结婚时间戳
    self.has_wedding = MsgAdapter.ReadInt()                        --0:未预约 1已预约 2已举办
    self.leave_end_time = MsgAdapter.ReadUInt()                    --协议离婚时间戳
    for i=1,2 do
        local info = {}
        info.role_id = MsgAdapter.ReadInt()
        RoleInfoCtrl.Instance:SendReferRoleEquipment(info.role_id)
        info.leave_end_time = MsgAdapter.ReadUInt()                --离婚时间戳
        info.gift_type = MsgAdapter.ReadInt()                      --开通月卡类型
        info.fetch_gift_flag = MsgAdapter.IntFlag(0,true)          --二进制 flag[0] 是否领取开通奖励 flag[1] 是否领取今天奖励
        info.gift_type_end_time = MsgAdapter.ReadInt()             --月卡开通结束时间戳
        info.skill_flag = bit:d2b(MsgAdapter.ReadUInt())           --情缘技能设置二进制
        self.role_info[i] = info
    end
    -- LogError(self)
end

-------------------离婚信息转发-------------------
SCMarryDivorceRoute = SCMarryDivorceRoute or BaseClass(BaseProtocolStruct)

function SCMarryDivorceRoute:__init()
    self.msg_type = 4263
end

function SCMarryDivorceRoute:Decode()
end

-------------------被邀请方转发-------------------
SCMarryInviteDisagreeRoute = SCMarryInviteDisagreeRoute or BaseClass(BaseProtocolStruct)

function SCMarryInviteDisagreeRoute:__init()
    self.msg_type = 4264
end

function SCMarryInviteDisagreeRoute:Decode()
    self.target_id = MsgAdapter.ReadInt()
    self.is_agree = MsgAdapter.ReadInt()
end

--解散通知
SCMarryLeaveNotice = SCMarryLeaveNotice or BaseClass(BaseProtocolStruct)

function SCMarryLeaveNotice:__init()
    self.msg_type = 4265
end

function SCMarryLeaveNotice:Decode()
    self.marry_id = MsgAdapter.ReadLL()
end

-------------------情缘创建广播-------------------
SCMarryCreateNotice = SCMarryCreateNotice or BaseClass(BaseProtocolStruct)

function SCMarryCreateNotice:__init()
    self.msg_type = 4266
end

function SCMarryCreateNotice:Decode()
    self.role_id_1 = MsgAdapter.ReadInt()
    self.role_id_2 = MsgAdapter.ReadInt()
end

-------------------寻爱礼盒采集物-------------------
SCWeddingLookLoveInfo = SCWeddingLookLoveInfo or BaseClass(BaseProtocolStruct)
function SCWeddingLookLoveInfo:__init()
    self.msg_type = 4251
end

function SCWeddingLookLoveInfo:Decode()
    self.id = Config.get_married_auto.find_gift[1].npc_id
    local npc_co = NpcHelper.NpcCo(self.id)
    self.res_id = npc_co.res_id
    self.name = npc_co.name
    self.x = MsgAdapter.ReadInt()
    self.y = MsgAdapter.ReadInt()
    self.flag = MsgAdapter.ReadInt()
    self.info = {flag = self.flag}
end

-------------------花车停住通知-------------------
SCParadeFloatInfo = SCParadeFloatInfo or BaseClass(BaseProtocolStruct)
function SCParadeFloatInfo:__init()
    self.msg_type = 4450
end

function SCParadeFloatInfo:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	self.is_stop = MsgAdapter.ReadShort()
end