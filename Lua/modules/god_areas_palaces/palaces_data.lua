PalacesData = PalacesData or BaseClass()

function PalacesData:__init()
    if PalacesData.Instance ~= nil then
        Debuger.LogError("[PalacesData] attempt to create singleton twice!")
        return
    end
    PalacesData.Instance = self
    self.all_info = Config.yunding_heavenly_palace_auto
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

    self.achi_data = SmartData.New({list = {}})
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
function PalacesData:initConfig()
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

function PalacesData:SceneInfo()
    return self.scene_info
end
function PalacesData:SceneInfoConfig(platform_id)
    local info = self:SceneInfo()
    for i, v in ipairs(info) do
        if v.platform_id == platform_id then
            return v
        end
    end
    return nil
end
function PalacesData:AchievementInfo()
    return self.achievement_info
end

function PalacesData:BossConfig()
    return self.boss_cfg
end

function PalacesData:GetBossReward(boss_type)
    return self.boss_reward_list[boss_type] or {}
end
function PalacesData:GetBossRewardScore(boss_type, rank)
    local list = self:GetBossReward(boss_type)
    for i, v in ipairs(list) do
        if rank >= v.min_rank_pos and rank <= v.max_rank_pos then
            return v.reward_score
        end
    end
    return 0
end

function PalacesData:GetRankingList(rank_type)
    return self.ranking_list[rank_type] or {}
end
function PalacesData:GetRankingListReward(rank_type, rank)
    local list = self:GetRankingList(rank_type)
    for i, v in ipairs(list) do
        if rank >= v.min_rank_pos and rank <= v.max_rank_pos then
            return v.ranking_reward_show
        end
    end
    return {}
end

function PalacesData:SetAchievementInfo(list)
    self.achi_data.list = list
    self.achi_data.flush_flag = not self.achi_data.flush_flag
end
--服务端成就信息
function PalacesData:GetAchiData(type)
    local data = self.achi_data.list[type] or {}
    return data
end

function PalacesData:GetAchiValue(type)
    local data = self:GetAchiData(type)
    return data.num or 0
end
function PalacesData:AchiIsGet(type, index)
    local data = self:GetAchiData(type)
    if data.fetch_flag then
        return data.fetch_flag[index] == 1
    end
    return false
end
function PalacesData:AchiRedPoint()
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

function PalacesData:SetRank(rank_type, rank_list)
    self.rank_data[rank_type] = rank_list
end

function PalacesData:GetRankList(rank_type)
    return self.rank_data[rank_type] or {}
end


function PalacesData:ServerName(server_id)
    local info = LoginData.Instance:GetServerInfoById(server_id)
    if info then
		return info.name or ""
	else
		return ""
	end
end
function PalacesData:SetSceneMono(mono)
    self.SceneMono = mono
    self.scene_info_sm.flush_platform = not self.scene_info.flush_platform
end
function PalacesData:GetSceneMono()
    return self.SceneMono
end
function PalacesData:GetPreReward()
    return self.other_info.reward_show
end

function PalacesData:GetRoleInfo()
    return self.role_info
end

function PalacesData:SetRoleInfo(protocol)
    self.role_info.platform_id = protocol.platform_id 
    self.role_info.action_value = protocol.action_value 
    self.role_info.score = protocol.score 
    self.time_info.goto_time = protocol.goto_time 
    self.time_info.attack_role_time = protocol.attack_role_time
    self.time_info.attack_boss_time = protocol.attack_boss_time
end

function PalacesData:FullActionValue()
    return self.other_info.start_power
end

function PalacesData:SetSceneRoleInfo(protocol)
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
function PalacesData:GetSceneRoleList(platform_id)
    local list = {}
    for k, v in pairs(self.scene_role_list) do
        if v.platform_id == platform_id then
            table.insert(list, v)
        end
    end
    return list
end
function PalacesData:GetCampRoleNum(list, camp_type)
    local num = 0
    for k, v in pairs(list) do
        -- if v.camp_id and v.camp_id == camp_type then
        if v.server_id and v.server_id == camp_type then
            num = num + 1
        end
    end
    return num
end
function PalacesData:GetSceneInfo()
    return self.scene_info_sm
end
function PalacesData:GetTimeInfo()
    return self.time_info
end
function PalacesData:ChangePlatformCD()
    return self.other_info.transfer_cd
end
function PalacesData:GetBossInfo()
    return self.boss_info
end
function PalacesData:SetSceneBossInfo(protocol)
    self.boss_info.boss_index = protocol.boss_index
    self.boss_info.boss_remain_hp = protocol.boss_remain_hp
    self.time_info.boss_can_challenge_time = protocol.boss_can_challenge_time
    self.time_info.boss_skill_time = protocol.boss_skill_time
end
function PalacesData:CreateRuinsBoss()
    --创建boss出来
    local boss_config = self:GetBossConfig()
    if boss_config == nil then
        return
    end
    local npc_cfg = Cfg.NpcById(boss_config.npc_id)
    local vo = CommonStruct.NpcVo(npc_cfg)
    vo.obj_type = SceneObjType.PalacesBoss
    local scene_cfg = self:SceneInfoConfig(9)
    if self.ruins_boss == nil then
        self.ruins_boss = SceneObj.CreateObj(vo)
    else
        self.ruins_boss.vo = vo
        self.ruins_boss:FlushDrawer()
    end
    local pos, _= SceneCtrl.Instance:CurScene():PosToClint(scene_cfg.pos_x, scene_cfg.pos_y)
    self.ruins_boss:SetPos(pos)
    self.ruins_boss:SetAngles(Vector3.New(0,-90,0))
end
function PalacesData:FlushBoss()
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
function PalacesData:ClearRuinsBoss()
    if self.ruins_boss then
        self.ruins_boss:DeleteMe()
        self.ruins_boss = nil
    end
end

function PalacesData:SetMyRank(type, rank, value)
    self.my_rank_list[type] = {rank = rank, value = value}
end
function PalacesData:GetMyRank(type)
    return self.my_rank_list[type] or {rank = 0, value = 0}
end
function PalacesData:GetBossConfig(boss_type)
    boss_type = boss_type or self.boss_info.boss_index
    local config = self:BossConfig()
    for k, v in pairs(config) do
        if v.boss_type == boss_type then
            return v
        end
    end
    return nil
end
function PalacesData:BossIsDie(boss_type)
    if self.boss_info.boss_index > boss_type then
        return true
    end
    return false
end

function PalacesData:SetBossHurt(total_hurt)
    --把total_hurt分成几份然后进行飘字
    self.hurt_info.hurt = total_hurt
    self:PushPopupData({total_hurt = total_hurt})
end

function PalacesData:GetRuinsBoss()
    if self.ruins_boss == nil then
        self:CreateRuinsBoss()
    else
        self:FlushBoss()
    end
    return self.ruins_boss
end
function PalacesData:GetBattleRoleCD()
    return self.other_info.battle_cd
end
function PalacesData:GetBattleBossCD()
    return self.other_info.battle_boss_cd
end
function PalacesData:PopupData()
    return self.hp_popup
end

-- 飘字数据
function PalacesData:PushPopupData(data)
    self.hp_popup[self.hp_popup:Length() + 1] = data
end

function PalacesData:PopPopupData()
    local val = self.hp_popup:Val()
    if #val > 0 then
        self.hp_popup:Set({})
    end
    return val
end
--相同角色
function PalacesData:UUID()
    return LandsBetweenData.Instance.my_info.role_uuid
end
--同一阵营
function PalacesData:ServerId()
    return GodAreasActsData.Instance:FactionId() --LoginData.Instance:GetLastLoginServer()
end

function PalacesData:CanBattleBossPlatformId()
    return self.other_info.boss_platform_id
end

function PalacesData:OpenTime()
    --内域第6天晚上21点30
    local inner_time = GodAreasActsData.Instance:InnerOpenTime()
    local act_info = self.act_daily_info[ActType.Palaces]
    local inner_time_tab = os.date("*t", inner_time)
    local open_time = string.split(act_info.begin_time, ":")
    inner_time_tab.day = inner_time_tab.day + act_info.activity_time - 1 --(包含当天) --持续6天
    inner_time_tab.hour = tonumber(open_time[1])
    inner_time_tab.min = tonumber(open_time[2])
    return inner_time_tab
end

function PalacesData:EndTime()
    --内域第6天晚上22点
    local inner_time = GodAreasActsData.Instance:InnerOpenTime()
    local act_info = self.act_daily_info[ActType.Palaces]
    local inner_time_tab = os.date("*t", inner_time)
    local end_time = string.split(act_info.end_time, ":")
    inner_time_tab.day = inner_time_tab.day + act_info.activity_time - 1
    inner_time_tab.hour = tonumber(end_time[1])
    inner_time_tab.min = tonumber(end_time[2])
    return inner_time_tab
end
function PalacesData:IsOpen()
    local base_info = LandsBetweenData.Instance:GetBaseInfo()
    local open_time = os.time(self:OpenTime())
    local end_time = os.time(self:EndTime())
    local is_open, need_wait = false, false
    if base_info.cur_status == GOD_SPACE_STATUS.InnerOpen then
        local curr_time = TimeCtrl.Instance:GetServerTime()
        if curr_time >= open_time and curr_time <= end_time then
            is_open = true
            need_wait = false
        end
    elseif base_info.cur_status < GOD_SPACE_STATUS.InnerOpen then
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
        else
            is_open = false
                
        end
    end
    return is_open, need_wait
end

function PalacesData:ModelOpen()
    local base_info = LandsBetweenData.Instance:GetBaseInfo()
    local open_time = os.time(self:OpenTime())
    local end_time = os.time(self:EndTime())
    local is_open = false
    if base_info.cur_status == GOD_SPACE_STATUS.InnerOpen then
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
function PalacesData:SetActInfo(info)
    self.act_info_sm.info = info
end

function PalacesData:GetActInfosm()
    return self.act_info_sm
end

function PalacesData:IsInPalaces()
    return ViewMgr:IsOpen(PalacesView) or SceneData.Instance:GetMode() == SceneModeType.Palaces
end

function PalacesData:GetBattleReward(type)
    return self.battle_reward[type + 1]
end
function PalacesData:GetOverTime()
    return self.other_info.over_time
end

--=============================================场景UI实例对象=============================================
function PalacesData:InitSceneUiVos()
    self.scene_ui_vos = {}
end

--这里保存实例化的对象池对象
function PalacesData:SetSceneUiVos(vo,ui)
    self.scene_ui_vos[vo] = ui
end

--获取场景UI列表
function PalacesData:GetSceneUiVos()
    return self.scene_ui_vos
end

--获取场景Ui对象
function PalacesData:GetSceneUiVo(func)
    for vo, ui in pairs(self.scene_ui_vos) do
        if func and func(vo) then
            return ui
        end
    end
end

-- function PalacesData:ClearData()
-- end

-- function PalacesData:SetTemplateInfo(protocol)
-- end