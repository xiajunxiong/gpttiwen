RuinsData = RuinsData or BaseClass()
RuinsData.FactionIcon = {
    [0] = "ShenYuZhenYing_Shui",
    "ShenYuZhenYing_Huo",
    "ShenYuZhenYing_Feng",
    "ShenYuZhenYing_Di"
}

function RuinsData:__init()
    if RuinsData.Instance ~= nil then
        Debuger.LogError("[RuinsData] attempt to create singleton twice!")
        return
    end
    RuinsData.Instance = self
    self.all_info = Config.guixu_area_auto
    self.scene_info = self.all_info.scene
    self.achievement_info = self.all_info.achievement
    self.other_info = self.all_info.others[1]
    -- self.info_data = SmartData.New()
    --boss排行
    self.boss_cfg = self.all_info.boss_to_configure
    self.boss_reward = self.all_info.boss_reward
    --服务器、个人排行榜
    self.ranking_cfg = self.all_info.ranking_reward

    self.act_info = Config.divine_domain_function_auto
    self.act_daily_info = self.act_info.daily

    self.battle_reward = self.all_info.battle_reward
    
    self.SceneMono = nil

    self.achi_data = SmartData.New({list = {}, flush_flag = false})
    self.role_info = SmartData.New({platform_id = 0, action_value = 0, score = 0})
    self.time_info = SmartData.New({goto_time = 0, attack_role_time = 0, attack_boss_time = 0, boss_can_challenge_time = 0, boss_skill_time = 0})
    self.boss_info = SmartData.New({boss_index = 0, boss_remain_hp = 10000})

    self.scene_info_sm = SmartData.New({flush_role = false, flush_platform = false, flush_arrow = false})

    self.act_info_sm = SmartData.New({is_open = false, act_status = 0, info = {}})
    self.hurt_info = SmartData.New({hurt = 0})
    self.scene_role_list = {}--根据平台来存
    self.cache_scene_role_list = {}--根据role_id来存

    self.hp_popup = SmartData.New() --掉血弹字
    self.is_auto_battle_boss = false

    self.rank_data = {}
    self.my_rank_list = {}
    self.battle_report = {}
    self:initConfig()
end
function RuinsData:initConfig()
    self.boss_reward_list = {}
    for i, v in ipairs(self.boss_reward) do
        if self.boss_reward_list[v.boss_type] == nil then
            self.boss_reward_list[v.boss_type] = {}
        end
        table.insert(self.boss_reward_list[v.boss_type], v)
    end
    self.ranking_list = {}
    for i, v in ipairs(self.ranking_cfg) do
        if self.ranking_list[v.rank_type] == nil then
            self.ranking_list[v.rank_type] = {}
        end
        table.insert(self.ranking_list[v.rank_type], v)
    end
    -- level_restrict
    -- fun_name
    -- icon_id
    self.act_msg = SmartData.New(ActivityInfo.New({info = self.act_daily_info[ActType.Ruins]}))
end

function RuinsData:SceneInfo()
    return self.scene_info
end
function RuinsData:SceneInfoConfig(platform_id)
    local info = self:SceneInfo()
    for i, v in ipairs(info) do
        if v.platform_id == platform_id then
            return v
        end
    end
    return nil
end
function RuinsData:AchievementInfo()
    return self.achievement_info
end

function RuinsData:BossConfig()
    return self.boss_cfg
end

function RuinsData:GetBossReward(boss_type)
    return self.boss_reward_list[boss_type] or {}
end
function RuinsData:GetBossRewardScore(boss_type, rank)
    local list = self:GetBossReward(boss_type)
    for i, v in ipairs(list) do
        if rank >= v.min_rank_pos and rank <= v.max_rank_pos then
            return v.reward_score
        end
    end
    return 0
end

function RuinsData:GetRankingList(rank_type)
    return self.ranking_list[rank_type] or {}
end
function RuinsData:GetRankingListReward(rank_type, rank)
    local list = self:GetRankingList(rank_type)
    for i, v in ipairs(list) do
        if rank >= v.min_rank_pos and rank <= v.max_rank_pos then
            return v.ranking_reward_show
        end
    end
    return {}
end

function RuinsData:SetAchievementInfo(list)
    self.achi_data.list = list
    self.achi_data.flush_flag = not self.achi_data.flush_flag
end
--服务端成就信息
function RuinsData:GetAchiData(type)
    local data = self.achi_data.list[type] or {}
    return data
end

function RuinsData:GetAchiValue(type)
    local data = self:GetAchiData(type)
    return data.num or 0
end
function RuinsData:AchiIsGet(type, index)
    local data = self:GetAchiData(type)
    if data.fetch_flag then
        return data.fetch_flag[index] == 1
    end
    return false
end
function RuinsData:AchiRedPoint()
    local data = self:AchievementInfo()
    local value
    local is_get
    for k, v in pairs(data) do
        value = self:GetAchiValue(v.achieve_type)
        is_get = self:AchiIsGet(v.achieve_type, v.ach_level)
        if not is_get and value >= v.ach_param1 then
            return 1
        end
    end
    return 0
end

function RuinsData:SetRank(rank_type, rank_list)
    self.rank_data[rank_type] = rank_list
end

function RuinsData:GetRankList(rank_type)
    return self.rank_data[rank_type] or {}
end


function RuinsData:ServerName(server_id)
    local info = LoginData.Instance:GetServerInfoById(server_id)
    if info then
		return info.name or ""
	else
		return ""
	end
end
function RuinsData:SetSceneMono(mono)
    self.SceneMono = mono
    self.scene_info_sm.flush_platform = not self.scene_info.flush_platform
end
function RuinsData:GetSceneMono()
    return self.SceneMono
end
function RuinsData:GetPreReward()
    return self.other_info.reward_show
end

function RuinsData:GetRoleInfo()
    return self.role_info
end

function RuinsData:SetRoleInfo(protocol)
    self.role_info.platform_id = protocol.platform_id 
    self.role_info.action_value = protocol.action_value 
    self.role_info.score = protocol.score 
    self.time_info.goto_time = protocol.goto_time 
    self.time_info.attack_role_time = protocol.attack_role_time
    self.time_info.attack_boss_time = protocol.attack_boss_time
end

function RuinsData:FullActionValue()
    return self.other_info.start_power
end

function RuinsData:SetSceneRoleInfo(protocol)
    local item = protocol.item
    if protocol.type == 0 then
        local is_exist = false
        for k, v in pairs(self.scene_role_list) do
            if v.role_id == item.role_id and v.plat_type == item.plat_type then
                is_exist = true
                self.scene_role_list[k] = item
            end
        end
        if not is_exist then
            table.insert(self.scene_role_list, item)
        end
    else
        local remove_k = 0
        for k, v in pairs(self.scene_role_list) do
            if v.role_id == item.role_id and v.plat_type == item.plat_type then
                remove_k = k
                v = item
            end
        end
        if remove_k > 0 then
            table.remove(self.scene_role_list, remove_k)
        end
    end
    self.scene_info_sm.flush_role = not self.scene_info_sm.flush_role
    LogDG("总人数",table.nums(self.scene_role_list))
end


--根据我当前的平台id来获取列表
function RuinsData:GetSceneRoleList(platform_id)
    local list = {}
    for k, v in pairs(self.scene_role_list) do
        if v.platform_id == platform_id then
            table.insert(list, v)
        end
    end
    return list
end
function RuinsData:GetSceneInfo()
    return self.scene_info_sm
end
function RuinsData:GetTimeInfo()
    return self.time_info
end
function RuinsData:ChangePlatformCD()
    return self.other_info.transfer_cd
end
function RuinsData:GetBossInfo()
    return self.boss_info
end
function RuinsData:SetSceneBossInfo(protocol)
    self.boss_info.boss_index = protocol.boss_index
    self.boss_info.boss_remain_hp = protocol.boss_remain_hp
    self.time_info.boss_can_challenge_time = protocol.boss_can_challenge_time
    self.time_info.boss_skill_time = protocol.boss_skill_time
end
function RuinsData:CreateRuinsBoss()
    --创建boss出来
    local boss_config = self:GetBossConfig()
    if boss_config == nil then
        return
    end
    local npc_cfg = Cfg.NpcById(boss_config.npc_id)
    local vo = CommonStruct.NpcVo(npc_cfg)
    vo.obj_type = SceneObjType.RuinsBoss
    local scene_cfg = self:SceneInfoConfig(9)
    if self.ruins_boss == nil then
        self.ruins_boss = SceneObj.CreateObj(vo)
    else
        self.ruins_boss.vo = vo
        self.ruins_boss:FlushDrawer()
    end
    local pos, _= SceneCtrl.Instance:CurScene():PosToClint(scene_cfg.pos_x, scene_cfg.pos_y)
    self.ruins_boss:SetPos(pos)
end
function RuinsData:FlushBoss()
    local boss_config = self:GetBossConfig()
    if boss_config == nil then
        return
    end
    local npc_cfg = Cfg.NpcById(boss_config.npc_id)
    local vo = CommonStruct.NpcVo(npc_cfg)
    if self.ruins_boss then
        self.ruins_boss.vo = vo
        self.ruins_boss:FlushDrawer()
    end
end
function RuinsData:ClearRuinsBoss()
    if self.ruins_boss then
        self.ruins_boss:DeleteMe()
        self.ruins_boss = nil
    end
end

function RuinsData:SetMyRank(type, rank, value)
    self.my_rank_list[type] = {rank = rank, value = value}
end
function RuinsData:GetMyRank(type)
    return self.my_rank_list[type] or {rank = 0, value = 0}
end
function RuinsData:GetBossConfig(boss_type)
    boss_type = boss_type or self.boss_info.boss_index
    local config = self:BossConfig()
    for k, v in pairs(config) do
        if v.boss_type == boss_type then
            return v
        end
    end
    return nil
end
function RuinsData:BossIsDie(boss_type)
    if self.boss_info.boss_index > boss_type then
        return true
    end
    return false
end

function RuinsData:SetBossHurt(total_hurt)
    --把total_hurt分成几份然后进行飘字
    self.hurt_info.hurt = total_hurt
    self:PushPopupData({total_hurt = total_hurt})
end

function RuinsData:GetRuinsBoss()
    if self.ruins_boss == nil then
        self:CreateRuinsBoss()
    else
        self:FlushBoss()
    end
    return self.ruins_boss
end
function RuinsData:GetBattleRoleCD()
    return self.other_info.battle_cd
end
function RuinsData:GetBattleBossCD()
    return self.other_info.battle_boss_cd
end

function RuinsData:PopupData()
    return self.hp_popup
end

-- 飘字数据
function RuinsData:PushPopupData(data)
    self.hp_popup[self.hp_popup:Length() + 1] = data
end

function RuinsData:PopPopupData()
    local val = self.hp_popup:Val()
    if #val > 0 then
        self.hp_popup:Set({})
    end
    return val
end
--相同角色
function RuinsData:UUID()
    return LandsBetweenData.Instance.my_info.role_uuid
end
--同一阵营
function RuinsData:ServerId()
    return LoginData.Instance:GetLastLoginServer()
end

function RuinsData:CanBattleBossPlatformId()
    return self.other_info.boss_platform_id
end

function RuinsData:OpenTime()
    --外域第二天晚上
    local ourter_time = GodAreasActsData.Instance:OurterOpenTime()
    local act_info = self.act_daily_info[10052]
    local ourter_time_tab = os.date("*t", ourter_time)
    local open_time = string.split(act_info.begin_time, ":")
    ourter_time_tab.day = ourter_time_tab.day + 1
    ourter_time_tab.hour = tonumber(open_time[1])
    ourter_time_tab.min = tonumber(open_time[2])
    return ourter_time_tab
end

function RuinsData:EndTime()
    --外域关闭 归墟关闭
    local ourter_time = GodAreasActsData.Instance:OurterOpenTime()
    local end_time = ourter_time + 36 * TIME_HOUR_SECOND_NUM
    local ourter_time_tab = os.date("*t", end_time)
    return ourter_time_tab
end
function RuinsData:IsOpen()
    local base_info = LandsBetweenData.Instance:GetBaseInfo()
    local open_time = os.time(self:OpenTime())
    local end_time = os.time(self:EndTime())
    local is_open, need_wait = false, false
    if base_info.cur_status == GOD_SPACE_STATUS.OurterOpen then
        local curr_time = TimeCtrl.Instance:GetServerTime()
        if curr_time >= open_time and curr_time <= end_time then
            is_open = true
            need_wait = false
        end
    elseif base_info.cur_status < GOD_SPACE_STATUS.OurterOpen then
        is_open = false
        need_wait = true
    else
        is_open = false
        need_wait = false
    end
    if self.act_info_sm.info then
        if self.act_info_sm.info.state == ActStatusType.Standy
        or self.act_info_sm.info.state == ActStatusType.Open then
            is_open = true
        end
    end
    return is_open, need_wait
end
function RuinsData:ModelOpen()
    local base_info = LandsBetweenData.Instance:GetBaseInfo()
    local open_time = os.time(self:OpenTime())
    local end_time = os.time(self:EndTime())
    local is_open = false
    if base_info.cur_status == GOD_SPACE_STATUS.OurterOpen then
        is_open = true
    end
    if self.act_info_sm.info then
        if self.act_info_sm.info.state == ActStatusType.Standy
        or self.act_info_sm.info.state == ActStatusType.Open then
            is_open = true
        end
    end
    return is_open
end
function RuinsData:SetActInfo(info)
    self.act_info_sm.info = info
end

function RuinsData:GetActInfosm()
    return self.act_info_sm
end

function RuinsData:IsInRuins()
    return ViewMgr:IsOpen(RuinsView) or SceneData.Instance:GetMode() == SceneModeType.Ruins
end
--0 
function RuinsData:GetBattleReward(type)
    return self.battle_reward[type + 1]
end
function RuinsData:GetOverTime()
    return self.other_info.over_time
end

--=============================================场景UI实例对象=============================================
function RuinsData:InitSceneUiVos()
    self.scene_ui_vos = {}
end

--这里保存实例化的对象池对象
function RuinsData:SetSceneUiVos(vo,ui)
    self.scene_ui_vos[vo] = ui
end

--获取场景UI列表
function RuinsData:GetSceneUiVos()
    return self.scene_ui_vos
end

--获取场景Ui对象
function RuinsData:GetSceneUiVo(func)
    for vo, ui in pairs(self.scene_ui_vos) do
        if func and func(vo) then
            return ui
        end
    end
end

-- function RuinsData:ClearData()
-- end

-- function RuinsData:SetTemplateInfo(protocol)
-- end