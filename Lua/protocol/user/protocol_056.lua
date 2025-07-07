CSZhenYingShenShouInfo = CSZhenYingShenShouInfo or BaseClass(BaseProtocolStruct)
function CSZhenYingShenShouInfo:__init()
    self.msg_type = 5600 
end
function CSZhenYingShenShouInfo:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)--0挑战神兽 1 领取奖励 p1seq 2请求排行榜数据
    MsgAdapter.WriteInt(self.p1)
    LogDG(self)
end

SCZhenYingShenShouInfo = SCZhenYingShenShouInfo or BaseClass(BaseProtocolStruct)
function SCZhenYingShenShouInfo:__init()
    self.msg_type = 5601
end
function SCZhenYingShenShouInfo:Decode()
    self.hp_percent = MsgAdapter.ReadInt()--boss血量万分比
    LogDG("阵营神兽学历",self)
end

SCZhenYingShenShouRoleInfo = SCZhenYingShenShouRoleInfo or BaseClass(BaseProtocolStruct)
function SCZhenYingShenShouRoleInfo:__init()
    self.msg_type = 5602
end
function SCZhenYingShenShouRoleInfo:Decode()
    self.fight_times = MsgAdapter.ReadInt()--挑战次数
    self.fetch_reward_flag = MsgAdapter.IntFlag(0)--领奖标志
    self.total_hurt = MsgAdapter.ReadLL()--我的伤害
    LogDG("阵营神兽次数和奖励上海",self)
end

SCZhenYingShenShouRank = SCZhenYingShenShouRank or BaseClass(BaseProtocolStruct)
function SCZhenYingShenShouRank:__init()
    self.msg_type = 5603
end
function SCZhenYingShenShouRank:Decode()
    self.list = {}
    self.count = MsgAdapter.ReadInt()
    for i = 1, self.count do
        self.list[i] = self:ZhenYingShenShouRankItem(i)
    end
    LogDG("z阵营神兽排行榜", self)
end
function SCZhenYingShenShouRank:ZhenYingShenShouRankItem(rank)
    local info = {}
    info.rank_value = rank
    info.role_id = MsgAdapter.ReadInt()
    info.plat_type = MsgAdapter.ReadInt()
    -- info.role_uuid = MsgAdapter.ReadLL()
    info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    info.total_hurt = MsgAdapter.ReadLL()
    info.headshot_id = MsgAdapter.ReadInt()
    info.avatar_type = MsgAdapter.ReadInt()
    return info
end

--归墟之境
CSGuiXuZhiJingReq = CSGuiXuZhiJingReq or BaseClass(BaseProtocolStruct)
function CSGuiXuZhiJingReq:__init()
    self.msg_type = 5630
    --0进场景 1改变台子 p1台子id
    --2 攻击玩家 p1 是plat_type p2role_id  
    --3攻击boss
    --4请求排行榜 p1类型
    --5领取成就奖励 p1 类型 p2 第几个
end

function CSGuiXuZhiJingReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    MsgAdapter.WriteInt(self.req_type)
    MsgAdapter.WriteInt(self.p1)
    MsgAdapter.WriteInt(self.p2)
    LogDG("发送归墟协议", self)
end

SCGuiXuZhiJingAchievementInfo = SCGuiXuZhiJingAchievementInfo or BaseClass(BaseProtocolStruct)
function SCGuiXuZhiJingAchievementInfo:__init()
    self.msg_type = 5631
    self.TypeMax = 3 -- 0累计积分1胜利次数2主动挑战玩家3挑战boss
end
function SCGuiXuZhiJingAchievementInfo:Decode()
    self.list = {}
    for i = 0, self.TypeMax do
        self.list[i] = self:GodSpaceUserGuiXuZhiJingAchievementItem()
    end
    LogDG("成就", self)
end
function SCGuiXuZhiJingAchievementInfo:GodSpaceUserGuiXuZhiJingAchievementItem()
    local info = {}
    info.num = MsgAdapter.ReadUShort() --当前类型进度 0累计积分1战斗胜利2主动打玩家3挑战boss
    info.reserved_sh = MsgAdapter.ReadUShort()
    info.fetch_flag = MsgAdapter.IntFlag()--奖励领取标志
    return info
end

SCGuiXuZhiJingRoleInfo = SCGuiXuZhiJingRoleInfo or BaseClass(BaseProtocolStruct)
function SCGuiXuZhiJingRoleInfo:__init()
    self.msg_type = 5632--玩家信息进场景发
end

function SCGuiXuZhiJingRoleInfo:Decode()
    self.platform_id = MsgAdapter.ReadInt()--台子id
    self.action_value = MsgAdapter.ReadInt()--行动值
    self.score = MsgAdapter.ReadInt()--分数
    self.goto_time = MsgAdapter.ReadUInt()--进入台子时间
    self.attack_role_time = MsgAdapter.ReadUInt()--上次攻击人时间
    self.attack_boss_time = MsgAdapter.ReadUInt()--上次攻击boss的时间
    LogDG("进场景发的人物信息", self)
end

SCGuiXuZhiJingRank = SCGuiXuZhiJingRank or BaseClass(BaseProtocolStruct)
function SCGuiXuZhiJingRank:__init()
    self.msg_type = 5633--排行榜
end
function SCGuiXuZhiJingRank:Decode()
    self.rank_type = MsgAdapter.ReadInt()--0boss总伤害1boss单伤2总攻击次数3分数4个人分数
    self.rank = MsgAdapter.ReadInt()
    self.rank_value = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.rank_list = {}
    for i = 1, self.count, 1 do
        self.rank_list[i] = self:GuiXuZhiJingRankItem(i)
    end
    LogDG("boss排行榜", self)
end
function SCGuiXuZhiJingRank:GuiXuZhiJingRankItem(rank)
    local info = {}
    info.boss_type = self.rank_type
    info.rank_type = self.rank_type
    info.rank = rank
    info.server_id = MsgAdapter.ReadLL()
    info.p1 = MsgAdapter.ReadLL()
    MsgAdapter.ReadLL()
    return info
end


SCGuiXuZhiJingRoleRank = SCGuiXuZhiJingRoleRank or BaseClass(BaseProtocolStruct)
function SCGuiXuZhiJingRoleRank:__init()
    self.msg_type = 5634--玩家排行榜
end
function SCGuiXuZhiJingRoleRank:Decode()
    self.rank_type = MsgAdapter.ReadInt()--0boss总伤害1boss单伤2总攻击次数3分数4个人分数
    self.rank = MsgAdapter.ReadInt()
    self.rank_value = MsgAdapter.ReadInt()
    self.count = MsgAdapter.ReadInt()
    self.rank_list = {}
    for i = 1, self.count, 1 do
        self.rank_list[i] = self:GuiXuZhiJingRoleRankItem(i)
    end
    LogDG("玩家排行榜", self)
end
function SCGuiXuZhiJingRoleRank:GuiXuZhiJingRoleRankItem(rank)
    local info = {}
    info.rank = rank
    info.rank_type = self.rank_type
    info.boss_type = self.rank_type
    info.role_uuid = MsgAdapter.ReadLL()--plat_type + role_id == role_uuid
    info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    info.server_id = MsgAdapter.ReadLL()
    info.sore = MsgAdapter.ReadInt()
    MsgAdapter.ReadLL()
    return info
end

SCGuiXuZhiJingSceneRoleInfo = SCGuiXuZhiJingSceneRoleInfo or BaseClass(BaseProtocolStruct)
function SCGuiXuZhiJingSceneRoleInfo:__init()
    self.msg_type = 5635--场景玩家
end
function SCGuiXuZhiJingSceneRoleInfo:Decode()
    self.type = MsgAdapter.ReadInt()--0进场景 1离开场景
    self.item = self:GuiXuZhiJingSceneRoleItem()
    self.item.faction_id = GodAreasActsData.Instance:FactionId()
    LogDG("场景角色信息", self)
end
function SCGuiXuZhiJingSceneRoleInfo:GuiXuZhiJingSceneRoleItem()
    local info = {}
    -- info.role_uuid = MsgAdapter.ReadLL()--大跨服不能够role_id重复
    info.role_id = MsgAdapter.ReadInt()
    info.plat_type = MsgAdapter.ReadInt()
    info.role_uuid = int64.new(info.role_id, info.plat_type)
    info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    info.server_id = MsgAdapter.ReadLL()
    info.action_value = MsgAdapter.ReadInt()
    info.platform_id = MsgAdapter.ReadInt()
    info.cap = MsgAdapter.ReadInt()
    return info
end
SCGuiXuZhiJingBossInfo = SCGuiXuZhiJingBossInfo or BaseClass(BaseProtocolStruct)
function SCGuiXuZhiJingBossInfo:__init()
    self.msg_type = 5636--场景boss
end
function SCGuiXuZhiJingBossInfo:Decode()
    self.boss_index = MsgAdapter.ReadInt()
    self.boss_remain_hp = MsgAdapter.ReadInt() -- boss剩余血量万分比
    self.boss_can_challenge_time = MsgAdapter.ReadUInt() -- 新boss多久后可挑战
    self.boss_skill_time = MsgAdapter.ReadUInt() -- boss释放大招的时间
    LogDG("场景BOSS", self)
end

SCGuiXuZhiJingPVPResult = SCGuiXuZhiJingPVPResult or BaseClass(BaseProtocolStruct)
function SCGuiXuZhiJingPVPResult:__init()
    self.msg_type = 5637
end
function SCGuiXuZhiJingPVPResult:Decode()
    self.is_attacker_win = MsgAdapter.ReadInt()
    self.result_item1 = self:GuiXuZhiJingScenePVPResultItem()
    self.result_item2 = self:GuiXuZhiJingScenePVPResultItem()
    self.act_type = ActType.Ruins
    LogDG("归墟人物对战结果", self)
end
function SCGuiXuZhiJingPVPResult:GuiXuZhiJingScenePVPResultItem()
    local info = {}
    -- info.role_uuid = MsgAdapter.ReadLL()--plat_type + role_id == role_uuid
    info.role_id = MsgAdapter.ReadInt()
    info.plat_type = MsgAdapter.ReadInt()
    info.role_name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    info.server_id = MsgAdapter.ReadLL()
    info.prof = MsgAdapter.ReadInt()
    info.avatar_type = MsgAdapter.ReadShort()		   -- 123456
    info.headshot_id= MsgAdapter.ReadShort()		   -- 头像ID
    info.avatar_id = info.headshot_id
    return info
end

SCGuiXuZhiJingAttackBossResult = SCGuiXuZhiJingAttackBossResult or BaseClass(BaseProtocolStruct)
function SCGuiXuZhiJingAttackBossResult:__init()
    self.msg_type = 5638--攻击boss结果
end
function SCGuiXuZhiJingAttackBossResult:Decode()
    self.total_hurt = MsgAdapter.ReadLL()
    LogDG("Boss攻击结果", self)
end

-- 神域指南信息
SCGodSpaceGuideInfo = SCGodSpaceGuideInfo or BaseClass(BaseProtocolStruct)
function SCGodSpaceGuideInfo:__init()
    self.msg_type = 5610
end
function SCGodSpaceGuideInfo:Decode()
    self.guide_finish_flag = MsgAdapter.IntFlag()
    self.guide_receive_flag = MsgAdapter.IntFlag()
end

-- 神域 排行榜（阵营个人排行榜 神域个人排行榜 阵营服务器排行榜）
SCGodSpaceFactionPersonalRankList=SCGodSpaceFactionPersonalRankList or BaseClass(BaseProtocolStruct)
function SCGodSpaceFactionPersonalRankList:__init()
    self.msg_type = 5650  
end
function SCGodSpaceFactionPersonalRankList:Decode()
    self.rank_target= MsgAdapter.ReadInt() --GOD_SPACE_RANK_TARGET_TYPE
    self.rank_type= MsgAdapter.ReadInt()  --GOD_SPACE_RANK_TYPE
    local count= MsgAdapter.ReadInt()
    self.rank_list={}
    self.myRankInfo={}
    for i = 1, count, 1 do
        local rank_info=SCGodSpaceFactionPersonalRankList.SCGodSpaceRankUserInfo(i)
        self.rank_list[i] = rank_info
        if  RuinsData.Instance:UUID()==rank_info.uuid then
            self.myRankInfo=rank_info
        end
    end
end

function SCGodSpaceFactionPersonalRankList.SCGodSpaceRankUserInfo(rank)
    local info = {}
    info.rank = rank
    info.role_id = MsgAdapter.ReadInt()
    info.plat_type = MsgAdapter.ReadInt()
    info.uuid = int64.new(info.role_id, info.plat_type)
    -- info.uuid = MsgAdapter.ReadLL()
    info.name = MsgAdapter.ReadStrN(PROTC_ROLE_NAME_LEN)
    info.space_id = MsgAdapter.ReadInt()              -- 神域id
    info.server_id = MsgAdapter.ReadInt()              -- 服务器id
    info.level = MsgAdapter.ReadShort()                --等级
    info.top_level= MsgAdapter.ReadShort()		       -- 巅峰等级
	info.avatar_type= MsgAdapter.ReadShort()		   -- 虚拟人物类型
	info.headshot_id= MsgAdapter.ReadShort()		   -- 头像ID 
	info.fly_flag= MsgAdapter.ReadShort()			   -- 飞升阶段
	info.faction= MsgAdapter.ReadShort()			   -- 阵营
    info.rank_value = MsgAdapter.ReadLL()
    return info
end

-- 神域阵营排行榜
SCGodSpaceAllFactionRankList = SCGodSpaceAllFactionRankList or BaseClass(BaseProtocolStruct)
function SCGodSpaceAllFactionRankList:__init() 
    self.msg_type = 5651
end

function SCGodSpaceAllFactionRankList:Decode()
    self.rank_type = MsgAdapter.ReadInt()
    self.rank_list = {}
    for i = 1, GOD_SPACE_FACTION_NUM, 1 do
        local info = {}
        info.rank = i
        info.faction = MsgAdapter.ReadInt()
        info.rank_value = MsgAdapter.ReadLL()
        self.rank_list[i] = info
        if LandsBetweenData.Instance.my_info.faction ==  info.faction then
            self.myRankInfo = info
        end
    end
end


SCGodSpacePalaceRankInfo = SCGodSpacePalaceRankInfo or BaseClass(BaseProtocolStruct)
function SCGodSpacePalaceRankInfo:__init()
    self.msg_type = 5655 --神域殿堂排行信息
end
function SCGodSpacePalaceRankInfo:Decode()
    self.season_id = MsgAdapter.ReadInt()		-- 赛季
	self.rank_begin = MsgAdapter.ReadInt()		--排名起始
	self.count = MsgAdapter.ReadInt() --PalaceRankParam数量
    self.rank_info = {}
    for i = 1, self.count do
        self.rank_info[i] = PortocolCommon.PalaceRankParam()
        self.rank_info[i].rank = i
    end
    LogDG("殿堂排行", self)
end

SCGodSpacePalaceStatueInfo = SCGodSpacePalaceStatueInfo or BaseClass(BaseProtocolStruct)
function SCGodSpacePalaceStatueInfo:__init()
    self.msg_type = 5656 --神域殿堂场景雕像信息
end
function SCGodSpacePalaceStatueInfo:Decode()
    self.count = MsgAdapter.ReadInt()
    self.rank_info = {}
    for i = 1, self.count do
        self.rank_info[i] = PortocolCommon.PalaceRankParam() 
        self.rank_info[i].rank = i
    end
    LogDG("殿堂雕像", self)
end

SCGodSpacePalaceWorshipInfo = SCGodSpacePalaceWorshipInfo or BaseClass(BaseProtocolStruct)
function SCGodSpacePalaceWorshipInfo:__init()
    self.msg_type = 5657 --神域殿堂膜拜
end
function SCGodSpacePalaceWorshipInfo:Decode()
    self.palace_worship_flag = MsgAdapter.ReadInt()--	// 0--未领取, 1--已领取
    LogDG("殿堂膜拜", self)
end

CSGodSpacePalaceReq = CSGodSpacePalaceReq or BaseClass(BaseProtocolStruct)
function CSGodSpacePalaceReq:__init()
    self.msg_type = 5658
end
function CSGodSpacePalaceReq:Encode()
    MsgAdapter.WriteBegin(self.msg_type)
    LogDG(self)
end

SCGuiXuZhiJingOpenInfo = SCGuiXuZhiJingOpenInfo or BaseClass(BaseProtocolStruct)
function SCGuiXuZhiJingOpenInfo:__init()
    self.msg_type = 5639
end
function SCGuiXuZhiJingOpenInfo:Decode()
    self.info = {}
    self.info.state = MsgAdapter.ReadInt()
    self.info.ready_time = MsgAdapter.ReadInt()
    self.info.open_time = MsgAdapter.ReadInt()
    self.info.end_time = MsgAdapter.ReadInt()

    LogDG("归墟之境活动信息", self)
end