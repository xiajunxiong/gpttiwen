-- 进入场景
SCEnterScene = SCEnterScene or BaseClass(BaseProtocolStruct)
function SCEnterScene:__init()
	self.msg_type = 1100
end

function SCEnterScene:Decode()
	self.sceneid = MsgAdapter.ReadInt()
	self.obj_id = MsgAdapter.ReadUShort()
	self.is_first_enter = MsgAdapter.ReadShort()
	self.pos_x = MsgAdapter.ReadShort()
	self.pos_y = MsgAdapter.ReadShort()
	self.scene_key = MsgAdapter.ReadInt()
end

-- 其他玩家进入视野
SCVisibleObjEnterRole = SCVisibleObjEnterRole or BaseClass(BaseProtocolStruct)
function SCVisibleObjEnterRole:__init()
	self.msg_type = 1102
end

function SCVisibleObjEnterRole:Decode()
	self.role_id = MsgAdapter.ReadInt()
	self.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	self.obj_id = MsgAdapter.ReadUShort()
	self.level = MsgAdapter.ReadShort()
	self.top_level = MsgAdapter.ReadInt()
	self.kill_value = MsgAdapter.ReadInt()

	self.appearance = Appearance()

	self.move_speed = OBJ_BASE_MOVE_SPEED * (100 + MsgAdapter.ReadInt()) / 100
	self.pos_x = MsgAdapter.ReadShort()
	self.pos_y = MsgAdapter.ReadShort()
	self.distance = MsgAdapter.ReadFloat()
	self.dir_y = MsgAdapter.ReadFloat()
	self.scene_id = MsgAdapter.ReadInt()
	
	self.behavior_state = MsgAdapter.ReadChar()
	self.is_leader = MsgAdapter.ReadChar()
	self.is_in_team = MsgAdapter.ReadChar() == 1		--角色是否在队伍内
	self.move_state = MsgAdapter.ReadChar()             --行动状态 0 正常 1 潜行 2 施法 3 引导 4 挪移
	self.team_id = MsgAdapter.ReadInt()	--队伍id 无=-1
	self.team_is_red = MsgAdapter.ReadChar() == 1	--是否红名队伍
	self.guild_post = MsgAdapter.ReadChar()	--保留字 -1无家族
	self.da_wan_ka_level = MsgAdapter.ReadShort()		--QQ大厅平台需求用于大玩咖标识显示
	self.plat_type = MsgAdapter.ReadInt() --平台id  plat_type + role_id == role_uuid
	-- self.profession = MsgAdapter.ReadInt()

	local attach_mask = MsgAdapter.ReadInt()
	local mask_t = bit:d2b(attach_mask)
	--具体attach_type和读取方式问服务端
	if 0 ~= mask_t[32 - 0] then
		self.escort_id = MsgAdapter.ReadInt()
	else
		self.escort_id = 0
	end
	if 0 ~= mask_t[32 - 1] then
		self.guild_name = MsgAdapter.ReadStrN(PROTC_GUILD_NAME_LEN)
		self.arms = MsgAdapter.ReadStrN(8)				--族徽
		-- self.guild_ry = MsgAdapter.ReadInt()			--荣耀之证
	else
		self.guild_name = ""
		self.arms = ""
	end
	if 0 ~= mask_t[32 - 2] then
		self.title_id = MsgAdapter.ReadInt()
	else
		self.title_id = 0
	end
	if 0 ~= mask_t[32 - 3] then
		self.facescroe_id = MsgAdapter.ReadInt()		--颜值系统|头衔资源id
	else
		self.facescroe_id = 0
	end
	if 0 ~= mask_t[32 - 4] then
		self.pet_id = MsgAdapter.ReadInt()
		self.pet_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		self.skin_id = MsgAdapter.ReadInt()
		self.pet_fly_flag = MsgAdapter.ReadInt()
		self.rank = MsgAdapter.ReadInt()
	else
		self.pet_id = 0
		self.pet_name = ""
	end
	if 0 ~= mask_t[32 - 5] then
		self.sworn_info = {empty = true}
		self.sworn_info.zu_name = PortocolCommon.SwornName()
		self.sworn_info.member_name = PortocolCommon.SwornMemberName()
		if self.sworn_info.member_name == "" or string.find(self.sworn_info.member_name, " ") then
			self.sworn_info.empty = true
		else
			self.sworn_info.empty = false
		end
	else
		self.sworn_info = {empty = true}
	end
end

--对象离开视野
SCVisibleObjLeave = SCVisibleObjLeave or BaseClass(BaseProtocolStruct)
function SCVisibleObjLeave:__init()
	self.msg_type = 1103
end

function SCVisibleObjLeave:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	MsgAdapter.ReadUShort()
end


-- 对象移动返回
SCObjMove = SCObjMove or BaseClass(BaseProtocolStruct)
function SCObjMove:__init()
	self.msg_type = 1104
end

function SCObjMove:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	self.mark = MsgAdapter.ReadShort()
	self.pos_x = MsgAdapter.ReadShort()
	self.pos_y = MsgAdapter.ReadShort()
	self.dir_y = MsgAdapter.ReadFloat()
	self.distance = MsgAdapter.ReadFloat()
end

-- 强设主角位置 
SCResetPost = SCResetPost or BaseClass(BaseProtocolStruct)
function SCResetPost:__init()
	self.msg_type = 1105
end

function SCResetPost:Decode()
	self.x = MsgAdapter.ReadShort()
	self.y = MsgAdapter.ReadShort()
	self.scene_id = MsgAdapter.ReadInt()
end
--切换场景通知
SCChangeSceneNotice = SCChangeSceneNotice or BaseClass(BaseProtocolStruct)
function SCChangeSceneNotice:__init()
	self.msg_type = 1106
end

function SCChangeSceneNotice:Decode()
	self.info = {}
	self.info.change_scene_reason = MsgAdapter.ReadInt()
	self.info.target_scene_id = MsgAdapter.ReadInt()
end

--请求进副本
CSEnterFB = CSEnterFB or BaseClass(BaseProtocolStruct)
function CSEnterFB:__init()
	self.msg_type = 1107
	self.fb_type = 0
	self.param_1 = 0
	self.param_2 = 0
end

function CSEnterFB:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.fb_type)
	MsgAdapter.WriteShort(0)
	MsgAdapter.WriteInt(self.param_1)
	MsgAdapter.WriteInt(self.param_2)
end

--请求离开副本
CSLeaveFB = CSLeaveFB or BaseClass(BaseProtocolStruct)
function CSLeaveFB:__init()
	self.msg_type = 1108
end

function CSLeaveFB:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

SCVisibleObjEnterNPCMonster = SCVisibleObjEnterNPCMonster or BaseClass(BaseProtocolStruct)
function SCVisibleObjEnterNPCMonster:__init()
	self.msg_type = 1109
end
function SCVisibleObjEnterNPCMonster:Decode()
	self.npc_id = MsgAdapter.ReadInt()
	self.client_show_id = MsgAdapter.ReadInt()
	self.monster_group_id = MsgAdapter.ReadInt()
	self.battle_mode = MsgAdapter.ReadInt()
	self.npc_seq = MsgAdapter.ReadInt()
	self.obj_id = MsgAdapter.ReadShort()
	MsgAdapter.ReadShort()
	self.move_speed = OBJ_BASE_MOVE_SPEED * (100 + MsgAdapter.ReadInt()) / 100
	self.pos_x = MsgAdapter.ReadShort()
	self.pos_y = MsgAdapter.ReadShort()
	self.distance = MsgAdapter.ReadFloat()
	self.dir_y = Mathf.Rad2Deg * (MsgAdapter.ReadFloat() or 0) + 180
	self.scene_id = MsgAdapter.ReadInt()
end
SCVisibleObjEnterNPCGathers = SCVisibleObjEnterNPCGathers or BaseClass(BaseProtocolStruct)
function SCVisibleObjEnterNPCGathers:__init()
	self.msg_type = 1110
end
function SCVisibleObjEnterNPCGathers:Decode()
	self.npc_id = MsgAdapter.ReadInt()
	self.seq = MsgAdapter.ReadInt()
	self.pos_x = MsgAdapter.ReadShort()
	self.pos_y = MsgAdapter.ReadShort()
	self.scene_id = MsgAdapter.ReadInt()
	self.obj_id = MsgAdapter.ReadShort()
	self.gather_times = MsgAdapter.ReadShort()
end

SCChangeAppearanceRole = SCChangeAppearanceRole or BaseClass(BaseProtocolStruct)
function SCChangeAppearanceRole:__init()
	self.msg_type = 1112
end
function SCChangeAppearanceRole:Decode()
	self.obj_id = MsgAdapter.ReadInt()
	self.appearance = Appearance()
end

--主角请求移动
CSObjMove = CSObjMove or BaseClass(BaseProtocolStruct)
function CSObjMove:__init()
	self.msg_type = 1150
	self.pos_x = 0
	self.pos_y = 0
	self.target_pos_x = 0
	self.target_pos_y = 0
	self.scene_id = 0
end

function CSObjMove:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteShort(self.pos_x)
	MsgAdapter.WriteShort(self.pos_y)
	MsgAdapter.WriteShort(self.target_pos_x)
	MsgAdapter.WriteShort(self.target_pos_y)
	MsgAdapter.WriteInt(self.scene_id)--客户端当前场景id
end


--使用飞天神靴请求
CSFlyByShoe = CSFlyByShoe or BaseClass(BaseProtocolStruct)
function CSFlyByShoe:__init()
	self.msg_type = 1151
	self.scene_id = 0
	self.pos_x = 0
	self.pos_y = 0
end

function CSFlyByShoe:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.scene_id)
	MsgAdapter.WriteShort(self.pos_x)
	MsgAdapter.WriteShort(self.pos_y)
end
--飞townpoint
CSAutoEscapeFromCurrentPosition =  CSAutoEscapeFromCurrentPosition or BaseClass(BaseProtocolStruct)
function CSAutoEscapeFromCurrentPosition:__init()
	self.msg_type = 1153
end
function CSAutoEscapeFromCurrentPosition:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

--请求进入传送门
CSTransportReq = CSTransportReq or BaseClass(BaseProtocolStruct)
function CSTransportReq:__init()
	self.msg_type = 1052
	self.door_id = 0
end

function CSTransportReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.door_id)
end

-- 特别逻辑传送
CSSpecialTransportReq = CSSpecialTransportReq or BaseClass(BaseProtocolStruct)
function CSSpecialTransportReq:__init()
	self.msg_type = 1055
end

function CSSpecialTransportReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteUShort(self.obj_id)
	MsgAdapter.WriteShort(0)
end

-- 特别逻辑传送点
SCVisibleObjEnterSpecialTransport = SCVisibleObjEnterSpecialTransport or BaseClass(BaseProtocolStruct)
function SCVisibleObjEnterSpecialTransport:__init()
	self.msg_type = 1113
end

function SCVisibleObjEnterSpecialTransport:Decode()
	self.scene_id = MsgAdapter.ReadInt()
	self.obj_id = MsgAdapter.ReadUShort()
	self.x = MsgAdapter.ReadShort()
	self.y = MsgAdapter.ReadShort()
	MsgAdapter.ReadShort()
	self.param1 = MsgAdapter.ReadInt()
	self.param2 = MsgAdapter.ReadInt()
end

-- 宝箱进入视野
SCVisibleObjEnterTreasureChest = SCVisibleObjEnterTreasureChest or BaseClass(BaseProtocolStruct)
function SCVisibleObjEnterTreasureChest:__init()
	self.msg_type = 1114
end

function SCVisibleObjEnterTreasureChest:Decode()
	self.scene_id = MsgAdapter.ReadInt()
	self.obj_id = MsgAdapter.ReadUShort()
	self.x = MsgAdapter.ReadShort()
	self.y = MsgAdapter.ReadShort()
	self.is_open = MsgAdapter.ReadShort()
	self.id = MsgAdapter.ReadInt()
end

-- 功能性NPC进入视野
SCVisibleObjEnterNPCFuncNpc = SCVisibleObjEnterNPCFuncNpc or BaseClass(BaseProtocolStruct)
function SCVisibleObjEnterNPCFuncNpc:__init()
	self.msg_type = 1115
end

function SCVisibleObjEnterNPCFuncNpc:Decode()
	self.scene_id = MsgAdapter.ReadInt()
	self.obj_id = MsgAdapter.ReadUShort()
	self.fun_type = MsgAdapter.ReadShort()
	self.x = MsgAdapter.ReadShort()
	self.y = MsgAdapter.ReadShort()
	self.id = MsgAdapter.ReadInt()

	-- 附加信息
	self.info = {}
	self.info.info_type = MsgAdapter.ReadInt()
	self.info.param1 = MsgAdapter.ReadInt()
	self.info.param2 = MsgAdapter.ReadInt()
	self.info.param3 = MsgAdapter.ReadInt()
	self.dir_y = Mathf.Rad2Deg * (MsgAdapter.ReadFloat() or 0) + 180
	
end

-- 场景中obj位置列表
SCSceneObjList = SCSceneObjList or BaseClass(BaseProtocolStruct)
function SCSceneObjList:__init()
	self.msg_type = 1116
end

function SCSceneObjList:Decode()
	self.notify_reason = MsgAdapter.ReadShort()
	self.obj_num = MsgAdapter.ReadShort()
	self.obj_info_list = {}
	for i = 1, self.obj_num do
		local t = {
			x = MsgAdapter.ReadShort(),
			y = MsgAdapter.ReadShort(),
			obj_id = MsgAdapter.ReadUShort(),
			obj_type = MsgAdapter.ReadShort(),
		}
		self.obj_info_list[t.obj_id] = t
	end
end

-- 宝箱请求
CSTreasureChestReq = CSTreasureChestReq or BaseClass(BaseProtocolStruct)
function CSTreasureChestReq:__init()
	self.msg_type = 1056
end

function CSTreasureChestReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteUShort(self.obj_id)
	MsgAdapter.WriteUShort(0)
end

-- 更新跟随列表(更换队长时下发)
SCUpdateFollowList = SCUpdateFollowList or BaseClass(BaseProtocolStruct)
function SCUpdateFollowList:__init()
	self.msg_type = 1117
end

function SCUpdateFollowList:Decode()
	local fi = {}
	fi.id = MsgAdapter.ReadInt()
	fi.members = {}
	for i = 1, TeamConfig.TEAM_MEMBER_MAX do
		local member = MsgAdapter.ReadUShort()
		if CommonItemId.Exp ~= member then
			table.insert(fi.members, member)
		end
	end
	MsgAdapter.ReadShort()
	self.follow_info = fi
end

-- 增加跟随者(新入队或归队时下发)
SCAddFollow = SCAddFollow or BaseClass(BaseProtocolStruct)
function SCAddFollow:__init()
	self.msg_type = 1118
end

function SCAddFollow:Decode()
	local fi = {}
	fi.id = MsgAdapter.ReadInt()
	fi.member = MsgAdapter.ReadUShort()
	MsgAdapter.ReadShort()
	self.follow_info = fi
end

-- 删除跟随者(离队或暂离时下发)
SCRemoveFollow = SCRemoveFollow or BaseClass(BaseProtocolStruct)
function SCRemoveFollow:__init()
	self.msg_type = 1119
end

function SCRemoveFollow:Decode()
	local fi = {}
	fi.id = MsgAdapter.ReadInt()
	fi.member = MsgAdapter.ReadUShort()
	fi.is_del = MsgAdapter.ReadShort()	--当值为1,解散整个跟随队伍
	self.follow_info = fi
end

SCJoinTeamNotifyArea = SCJoinTeamNotifyArea or BaseClass(BaseProtocolStruct)
function SCJoinTeamNotifyArea:__init()
	self.msg_type = 1120
end
function SCJoinTeamNotifyArea:Decode()
	self.obj_id = MsgAdapter.ReadUShort()	--加入队伍玩家的obj
	self.is_red = MsgAdapter.ReadShort()	--是否红名队伍
	self.team_id = MsgAdapter.ReadShort()	--队伍id
end

SCLeaveTeamNotifyArea = SCLeaveTeamNotifyArea or BaseClass(BaseProtocolStruct)
function SCLeaveTeamNotifyArea:__init()
	self.msg_type = 1121
end
function SCLeaveTeamNotifyArea:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	MsgAdapter.ReadShort()
end

--[[ SCSpecialAppearanceChangeNotice = SCSpecialAppearanceChangeNotice or BaseClass(BaseProtocolStruct)
function SCSpecialAppearanceChangeNotice:__init()
	self.msg_type = 1123
end
function SCSpecialAppearanceChangeNotice:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	self.special_appearance_type = MsgAdapter.ReadShort()
	self.special_appearance_param = MsgAdapter.ReadInt()
end ]]


CSRoleTalkToNpc = CSRoleTalkToNpc or BaseClass(BaseProtocolStruct)
function CSRoleTalkToNpc:__init()
	self.msg_type = 1124
end

function CSRoleTalkToNpc:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

SCDayRewardDayFoodInfo = SCDayRewardDayFoodInfo or BaseClass(BaseProtocolStruct)
function SCDayRewardDayFoodInfo:__init()
	self.msg_type = 1127
end

function SCDayRewardDayFoodInfo:Decode()
	self.day_food_flag = bit:d2b(MsgAdapter.ReadUInt())
end

--巡游花车进入视野
SCVisibleObjEnterParadeFloat = SCVisibleObjEnterParadeFloat or BaseClass(BaseProtocolStruct)
function SCVisibleObjEnterParadeFloat:__init()
	self.msg_type = 1128
end

function SCVisibleObjEnterParadeFloat:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	local is_stop = MsgAdapter.ReadShort()
	self.obj_type = SceneObjType.ParadeFloat
	local float_type = MsgAdapter.ReadInt()
	local config = Config.get_married_auto.float_parade[float_type]
	self.move_speed = OBJ_BASE_MOVE_SPEED * (100 + MsgAdapter.ReadInt()) / 100
	self.x = MsgAdapter.ReadShort()
	self.y = MsgAdapter.ReadShort()
	self.distance = MsgAdapter.ReadFloat()
	self.dir_y = MsgAdapter.ReadFloat()
	self.scene_id = MsgAdapter.ReadInt()
	self.res_id = config.res_id
	self.id = config.npc_id
	--其他信息
	self.info = {host_info = {}}
	for i=1,2 do
		local info = {}
		info.uid = MsgAdapter.ReadInt()
		info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
		info.avatar_type = MsgAdapter.ReadInt()
		info.obj_id = MsgAdapter.ReadUShort()
		MsgAdapter.ReadShort()
		self.info.host_info[i] = info
	end
	self.info.float_type = float_type
	self.info.is_stop = is_stop
end

--飞主城
CSEscapeFromCurrentPosition = CSEscapeFromCurrentPosition or BaseClass(BaseProtocolStruct)
function CSEscapeFromCurrentPosition:__init()
	self.msg_type = 1152
end
function CSEscapeFromCurrentPosition:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
end

--天下至尊令 雕像 进入视野
SCVisibleObjEnterPeakTokenObj = SCVisibleObjEnterPeakTokenObj or BaseClass(BaseProtocolStruct)
function SCVisibleObjEnterPeakTokenObj:__init()
	self.msg_type = 1129
end

function SCVisibleObjEnterPeakTokenObj:Decode()
	self.obj_id = MsgAdapter.ReadUShort()
	self.obj_type = SceneObjType.PeakToken
	MsgAdapter.ReadShort()
	self.move_speed = OBJ_BASE_MOVE_SPEED * (100 + MsgAdapter.ReadInt()) / 100
	self.x = MsgAdapter.ReadShort()
	self.y = MsgAdapter.ReadShort()
	self.dir_y = MsgAdapter.ReadFloat()
	self.scene_id = MsgAdapter.ReadInt()
	
	self.res_size = 1.5
	self.uid = MsgAdapter.ReadInt()
	self.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
	self.appearance = Appearance()
    
	self.info = {}
	self.info.id = MsgAdapter.ReadLL()--唯一标识
	self.info.begin_time = MsgAdapter.ReadUInt() --开始时间戳
	self.info.end_time = MsgAdapter.ReadUInt()   --结束时间戳
	self.info.reward_times = MsgAdapter.ReadInt()  --角色参与次数
	--LogError("天下至尊令 雕像 进入视野",self)
end

--宠物强化福利信息下发
SCPetIntensifyRewardInfo = SCPetIntensifyRewardInfo or BaseClass(BaseProtocolStruct)
function SCPetIntensifyRewardInfo:__init()
	self.msg_type = 1130
end

function SCPetIntensifyRewardInfo:Decode()
	local info = {}
	info.level = MsgAdapter.ReadInt()
	info.flag_list = MsgAdapter.IntFlag(1)
	self.info = info
end

--宠物强化福利请求
CSPetIntensifyRewardReq = CSPetIntensifyRewardReq or BaseClass(BaseProtocolStruct)
function CSPetIntensifyRewardReq:__init()
	self.msg_type = 1131
end

function CSPetIntensifyRewardReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.seq)
end
--审核服客户端（特殊）埋点 不走http，走服务器协议
CSAuditBehaviourSubmitReq = CSAuditBehaviourSubmitReq or BaseClass(BaseProtocolStruct)
function CSAuditBehaviourSubmitReq:__init()
	self.msg_type = 1132
end

function CSAuditBehaviourSubmitReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.behaviour_type)
end

SCYaoXingShiLianInfo = SCYaoXingShiLianInfo or  BaseClass(BaseProtocolStruct)
function SCYaoXingShiLianInfo:__init()
	self.msg_type = 1133
end 

function SCYaoXingShiLianInfo:Decode()
	self.pass_seq = MsgAdapter.ReadInt()    -- 最高通关
    self.m_reward_bit = MsgAdapter.IntFlag(16)
	self.m_reward_bit2 = MsgAdapter.IntFlag(16)
	self.cmd = MsgAdapter.IntFlag(1)   -- 是否直购完毕
end

CSYaoXingShiLiangReq = CSYaoXingShiLiangReq or BaseClass(BaseProtocolStruct)
function CSYaoXingShiLiangReq:__init()
	self.msg_type = 1134
end

function CSYaoXingShiLiangReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.oper_type)     -- 0 下发 1 战斗 2 成就奖励 param领取seq  3 成就奖励2 param领取seq
	MsgAdapter.WriteInt(self.param) 
end

CSTaoQuanNaJiReq = CSTaoQuanNaJiReq or BaseClass(BaseProtocolStruct)
function CSTaoQuanNaJiReq:__init()
	self.msg_type = 1139
end

function CSTaoQuanNaJiReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type) -- 0 信息 1 开始 2 套圈（param1 x，param2 y） 3 进入副本 4离开副本
	MsgAdapter.WriteInt(self.param1) 
	MsgAdapter.WriteInt(self.param2) 
end

SCTaoQuanNaJiInfo = SCTaoQuanNaJiInfo or BaseClass(BaseProtocolStruct)
function SCTaoQuanNaJiInfo:__init()
	self.msg_type = 1140
end

function SCTaoQuanNaJiInfo:Decode()
	self.map_init = MsgAdapter.ReadInt()
	self.play_map = {}
	for i = 0,5 do 
		for j = 0,4 do 
			local vo = {}
			vo.item = MsgAdapter.ReadInt()
			vo.x = i
			vo.y = j
			table.insert(self.play_map,vo)
		end 
	end 
	self.can_play_times = MsgAdapter.ReadInt()
	self.today_play_times = MsgAdapter.ReadInt()
	self.flag_title = MsgAdapter.ReadInt()
end

SCTaoQuanNaJiPlayEndInfo = SCTaoQuanNaJiPlayEndInfo or BaseClass(BaseProtocolStruct)
function SCTaoQuanNaJiPlayEndInfo:__init()
	self.msg_type = 1141
end

function SCTaoQuanNaJiPlayEndInfo:Decode()
	self.score = MsgAdapter.ReadInt()    --得分
    self.item_1_count = MsgAdapter.ReadInt()    --数量1
	self.item_2_count = MsgAdapter.ReadInt()    --数量2
	self.item_3_count = MsgAdapter.ReadInt()    --数量3
	self.item_4_count = MsgAdapter.ReadInt()    --数量4
end


-- 兔子快跑的兔子npc下发
SCRabbitRaceObjInfo = SCRabbitRaceObjInfo or BaseClass(BaseProtocolStruct)
function SCRabbitRaceObjInfo:__init()
	self.msg_type = 1138
end

function SCRabbitRaceObjInfo:Decode()
	local info = {}
	info.obj_id = MsgAdapter.ReadUShort()
	MsgAdapter.ReadShort()
	info.move_speed = MsgAdapter.ReadInt()
	info.pos_x = MsgAdapter.ReadShort()
	info.pos_y = MsgAdapter.ReadShort()
	info.distance = MsgAdapter.ReadFloat()
	info.dir_y = MsgAdapter.ReadFloat()
	info.scene_id = MsgAdapter.ReadInt()
	info.rabbit_id = MsgAdapter.ReadInt()
	info.role_id = MsgAdapter.ReadInt()
	info.server_timestamp = MsgAdapter.ReadUInt()
	self.info = info
	--LogError("兔子快跑的兔子npc下发",self)
end

CSSanXianGuiDongReq = CSSanXianGuiDongReq or BaseClass(BaseProtocolStruct)
function CSSanXianGuiDongReq:__init()
	self.msg_type = 1135
end
function CSSanXianGuiDongReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.op_type) -- 0 信息 1 开始 2选择  是否正确，1正确 2错误  3 进场景
	MsgAdapter.WriteInt(self.param)
	-- LogDG(self)
end

SCSanXianGuiDongInfo = SCSanXianGuiDongInfo or BaseClass(BaseProtocolStruct)
function SCSanXianGuiDongInfo:__init()
	self.msg_type = 1136
end
function SCSanXianGuiDongInfo:Decode()
	self.today_play_times = MsgAdapter.ReadInt()
	self.pass_idx = MsgAdapter.ReadInt()
	-- LogError("今日信息", self)
end
SCSanXianGuiDongPlayEndInfo = SCSanXianGuiDongPlayEndInfo or BaseClass(BaseProtocolStruct)
function SCSanXianGuiDongPlayEndInfo:__init()
	self.msg_type = 1137
end
function SCSanXianGuiDongPlayEndInfo:Decode()
	self.game_type = MsgAdapter.ReadInt() --0 游戏种， 1 失败， 2胜利
	self.pass_idx = MsgAdapter.ReadInt() + 1
	self.choose_tamp = MsgAdapter.ReadUInt()
	self.sanxian_title_flag = MsgAdapter.ReadInt()
	-- LogError("收到服务端信息", self)
end

CSHundredGodBossReq = CSHundredGodBossReq or BaseClass(BaseProtocolStruct)
function CSHundredGodBossReq:__init()
	self.msg_type = 1145
end
function CSHundredGodBossReq:Encode()
	MsgAdapter.WriteBegin(self.msg_type)
	MsgAdapter.WriteInt(self.req_type)
	MsgAdapter.WriteInt(self.param)
end

SCHundredGodBossInfo = SCHundredGodBossInfo or BaseClass(BaseProtocolStruct)
function SCHundredGodBossInfo:__init()
	self.msg_type = 1146
end
function SCHundredGodBossInfo:Decode()
	self.reserve_ch = MsgAdapter.ReadChar()
	self.is_finish_hundred_god_boss_flag = MsgAdapter.ReadChar()
	self.fetch_hundred_god_boss_flag = MsgAdapter.ShortFlag(0, true)
	self.hundred_god_boss_fin_bounty_count = MsgAdapter.ReadShort()
	self.reserve_sh = MsgAdapter.ReadShort()
	self.hundred_god_boss_end_timestamp = MsgAdapter.ReadUInt()
end


-- 魅力七夕
SCRAMeiLiQiXiInfo = SCRAMeiLiQiXiInfo or BaseClass(BaseProtocolStruct)
function SCRAMeiLiQiXiInfo:__init()
    self.msg_type = 1148
end

function SCRAMeiLiQiXiInfo:Decode()
    local info = {}
	info.admiration_value = MsgAdapter.ReadInt()
	info.admiration_map = MsgAdapter.IntFlag()
	info.charm_value = MsgAdapter.ReadInt()
	info.charm_map = MsgAdapter.IntFlag()
    self.info = info
    -- self.is_buy_reward = MsgAdapter.ReadInt()
    -- self.fetch_reward_flag = MsgAdapter.ReadInt()
    -- LOG("魅力七夕下发",self.info)
end