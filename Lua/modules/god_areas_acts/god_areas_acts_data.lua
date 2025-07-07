GodAreasActsData = GodAreasActsData or BaseClass()
GodAreasActsData.Animals_Req_Type = {
    Fight = 0,
    Fetch = 1, -- 领取奖励 p1 seq
    Rank = 2 -- 排行榜
}
function GodAreasActsData:__init()
    if GodAreasActsData.Instance ~= nil then
        Debuger.LogError("[GodAreasActsData] attempt to create singleton twice!")
        return
    end
    GodAreasActsData.Instance = self
    self.all_info = Config.divine_domain_system_auto
    self.mythical_animals = self.all_info.mythical_animals[1]
    self.mythical_animals_monster = self.all_info.mythical_animals_monster
    self.mythical_animals_HP = self.all_info.mythical_animals_HP
    self.palace_cfg = self.all_info.palace
    self.animals_preview_items = {}
    self.total_remind = SmartData.New({flush = false})

    self.boss_info = SmartData.New({
        seq = 0,
        hp = 1,
        fight_times = 0,
        fetch = {},
        total_hurt = 0
    })
    self.act_info = Config.divine_domain_function_auto
    self.act_daily_info = self.act_info.daily
    self.animals_rank = SmartData.New()
    self.palaces_info = SmartData.New({
        list = {},
        statue_list = {},
        flush_flag = false
    })
    self.select_season_id = 0

    self.status_objs = {}
    self:initConfig()

    LandsBetweenData.Instance:ExtraRemindMark({
        mod_key = Mod.GodAreasActs.Ruins,
        target = self.boss_info,
        func = BindTool.Bind(self.GetGodAreasActRemind, self),
    })
end
function GodAreasActsData:GetGodAreasActRemind()
    return self:AnimalsRemind()
end
function GodAreasActsData:initConfig()
    self.animals_monster_list = {}
    for i, v in ipairs(self.mythical_animals_monster) do
        if self.animals_monster_list[v.camp_type] == nil then
            self.animals_monster_list[v.camp_type] = {}
        end
        if v.military_exploits > 0 then
            table.insert(v.reward, {item_id = CommonItemId.GodSpaceExploits, num = v.military_exploits})
        end
        if v.quintessence > 0 then
            table.insert(v.reward, {item_id = CommonItemId.GodSpaceEssence, num = v.quintessence})
        end
        table.insert(self.animals_monster_list[v.camp_type], v)
    end
end
function GodAreasActsData:__delete()

end
-- 从1开始
function GodAreasActsData:GetBossBase()
    return self.mythical_animals_HP[self.boss_info.seq + 1]
end
-- 从0开始
function GodAreasActsData:GetBoxList()
    return self.animals_monster_list[self.boss_info.seq]
end
function GodAreasActsData:GetBossInfo()
    return self.boss_info
end
function GodAreasActsData:BossPreviewReward()
    if table.nums(self.animals_preview_items) == 0 then
        if self.mythical_animals.military_exploits > 0 then
            table.insert(self.animals_preview_items, {item_id = CommonItemId.GodSpaceExploits, num = self.mythical_animals.military_exploits})
        end
        if self.mythical_animals.quintessence > 0 then
            table.insert(self.animals_preview_items, {item_id = CommonItemId.GodSpaceEssence, num = self.mythical_animals.quintessence})
        end
    end
    --military_exploits=10,
    --quintessence=1000
    --改成这两个
    return self.animals_preview_items--self.mythical_animals.preview
end
function GodAreasActsData:ChallengeTimes()
    return self.mythical_animals.challenge_number
end
function GodAreasActsData:ChallengeBuff()
    return self.mythical_animals.challenge_buff
end
function GodAreasActsData:SetHp(hp)
    self.boss_info.hp = hp
end
function GodAreasActsData:SetRoleInfo(protocol)
    self.boss_info.fight_times = protocol.fight_times
    self.boss_info.total_hurt = protocol.total_hurt
    self.boss_info.fetch = protocol.fetch_reward_flag
end
-- 获取当前赛季
function GodAreasActsData:SeasonId()
    return LandsBetweenData.Instance.base_info.season_index or 1
end
-- 阵营id[0,4)
function GodAreasActsData:FactionId()
    return LandsBetweenData.Instance.my_info.faction or 0
end
-- 获取内域开启时间
function GodAreasActsData:InnerOpenTime()
    return LandsBetweenData.Instance.base_info.inner_space_timestamp
end
-- 获取神域开启时间
function GodAreasActsData:OurterOpenTime()
    return LandsBetweenData.Instance.base_info.ourter_space_timestamp
end
-- 获取阵营神兽的开启时间
function GodAreasActsData:AnimalsOpenTime()
    -- 获取活动表来判断
    local inner_time = self:InnerOpenTime()
    local inner_time_tab = os.date("*t", inner_time)
    inner_time_tab.min = inner_time_tab.min + 1
    return inner_time_tab
end
-- 获取阵营神兽的结束时间
function GodAreasActsData:AnimalsEndTime()
    -- 获取活动表来判断
    local inner_time = self:InnerOpenTime()
    local inner_time_tab = os.date("*t", inner_time)
    local act_info = self.act_daily_info[ActType.Animals]
    inner_time_tab.day = inner_time_tab.day + act_info.open_day - 1 -- (包含当天) --持续6天
    local end_tab = string.split(act_info.end_time, ":")
    inner_time_tab.hour = tonumber(end_tab[1])
    inner_time_tab.min = tonumber(end_tab[2])
    return inner_time_tab
end
-- 判断阵营神兽是否开启
function GodAreasActsData:AnimalsIsOpen()
    local cur_time = TimeHelper:GetServerTime()
    local base_info = LandsBetweenData.Instance:GetBaseInfo()
    if base_info.cur_status == GOD_SPACE_STATUS.InnerOpen then
        return ActStatusType.Open
    end
    return ActStatusType.Close
end
function GodAreasActsData:GetAnimalsMaxRound()
    return self.mythical_animals.rounds_max
end

-- 神兽排行
function GodAreasActsData:SetAnimalsRank(protocol)
    self.animals_rank:Set(protocol.list)
end

function GodAreasActsData:BoxIsGet(seq)
    return (self.boss_info.fetch[seq] or 0) == 1
end
function GodAreasActsData:GetPalacesInfo()
    return self.palaces_info
end
function GodAreasActsData:SetPalaceRankInfo(season_id, list)
    self.palaces_info.list[season_id] = list
    self.palaces_info.flush_flag = not self.palaces_info.flush_flag
end
function GodAreasActsData:GetPalaceRankList(season_id)
    return self.palaces_info.list[season_id] or {}
end

function GodAreasActsData:SetPalaceStatueInfo(list)
    self.palaces_info.statue_list = list
    self:OnCreatePalaceStatus()
end

function GodAreasActsData:SetPalacesWorship(flag)
    self.palaces_info.worship_flag = flag
end
function GodAreasActsData:GetWorshipFlag()
    return self.palaces_info.worship_flag == 1
end

function GodAreasActsData:GetPalaceStatus(season_id)
    season_id = season_id or self:SeasonId()
    return self.palace_cfg[season_id]
end
function GodAreasActsData:GetStatusList()
    return self.palaces_info.statue_list
end
function GodAreasActsData:SetSelectSeason(id)
    self.select_season_id = id
end

function GodAreasActsData:SeasonList()
    local cur = self:SeasonId()
    local list = {}
    for i = 1, cur do
        list[i] = {
            season_id = i
        }
    end
    return list
end

function GodAreasActsData:OnCreatePalaceStatus()
    -- 先判断是否在殿堂中
    if not MainViewCtrl.Instance:IsInGameState(MainViewGameState.palace_rank) then
        for k, v in pairs(self.status_objs) do
            if v.table_obj ~= nil then
                v.table_obj:DeleteMe()
                v.table_obj = nil
            end
            SceneCtrl.Instance:Objs():DeleteClientObj(v.obj_id)
        end
        self.status_objs = {}
        return
    end
    local list = self:GetStatusList()
    LogDG("当前雕像个数为", table.nums(list))
    local temp = {}
    local cfg
    for i, v in ipairs(list) do
        if v.season_id > 0 then
            -- end
            -- for i = 1, 5 do
            -- cfg = self:GetPalaceStatus(i)--v.season_id)
            cfg = self:GetPalaceStatus(v.season_id)
            temp[i] = v
            -- temp[i] = self:TestInfo(i, i)
            temp[i].appearance = {}
            -- temp[i].appearance = self:TestInfo(i, i)
            temp[i].appearance = v

            if cfg then
                temp[i].x = cfg.pos_x
                temp[i].y = cfg.pos_z
                temp[i].dir_y = cfg.rotate
            else
                temp[i].x = 0
                temp[i].y = 0
                temp[i].dir_y = 0
            end
            temp[i].obj_id = CUSTOM_OBJID.PALACE_STATUS + i
            self.status_objs[i] = SceneCtrl.Instance:Objs():CreateClientObj(SceneObjType.PalaceStatus, temp[i])
            if self.status_objs[i].table_obj == nil then
                if cfg then
                    self.status_objs[i].table_obj = BodyObj.New(string.format(GoldLaw.ResPath, cfg.model),
                        self.status_objs[i].root_node)
                    self.status_objs[i].table_obj:SetLoadedCallback(BindTool.Bind(self.TableObjLoadedCallback, self, i))
                end
            end
        end
    end
end
function GodAreasActsData:TableObjLoadedCallback(i)
    local table_obj = self.status_objs[i].table_obj
    table_obj.obj:SetLocalEulerAngles(Vector3.New(-90, 0, 0))
    table_obj.obj:SetLocalPosition(Vector3.New(0, -0.8, 0))
end

function GodAreasActsData:TestInfo(index, season_id)
    local t = {}
    t.rank = index
    t.obj_type = SceneObjType.PalaceStatus
    local appearance = RoleData.Instance:GetApearance()
    t.role_uuid = 0
    t.role_name = RoleData.Instance:GetRoleName() .. season_id
    t.name = t.role_name
    t.space_id = 0
    t.server_id = LoginData.Instance:ServerName()
    t.season_id = season_id -- self.data:SeasonId()
    t.title_id = string.format(Language.GodAreasActs.PalaceTitle, DataHelper.GetDaXie(t.season_id))

    t.level = RoleData.Instance:GetRoleLevel()
    t.top_level = RoleData.Instance:GetTopLevel()
    t.prof_base = appearance.prof_base
    t.advance_time = appearance.advance_time --		    //!< 进阶次数[0,6]
    t.avatar_type = appearance.avatar_type --		    //!< 角色模型[1,6]
    t.color = appearance.color --  		//!< 颜色[0,4)
    t.weapon_id = appearance.weapon_id --		    //!< 武器ID
    t.sub_weapon_id = appearance.sub_weapon_id --	        //!< 副武器ID
    t.headshot_id = appearance.avatar_id --		    //!< 头像ID 
    t.avatar_id = t.headshot_id
    t.fly_flag = appearance.fly_flag --		    //!< 飞升阶段
    t.fashion_body_id = appearance.fashion_body_id or 0 -- //!< 幻化时装
    t.fashion_weapon_id = appearance.fashion_weapon_id or 0 --	//!< 幻化武器
    t.season_contribute_value = 10000 + index
    return t
end

function GodAreasActsData:AnimalsRemind()
    if self:AnimalsIsOpen() ~= ActStatusType.Close then
        if self:GetBossInfo().fight_times < self:ChallengeTimes()
        and self:GetBossInfo().hp > 0 then
            return 1
        end
        --可领奖励
        local list = self:GetBoxList()
        local boss_info = self:GetBossInfo()
        local is_get = false
        for i, v in ipairs(list) do
            is_get = self:BoxIsGet(v.seq)
            if not is_get and (boss_info.hp / 100) <= v.surplus_hp then
                return 1
            end
        end
    end
    return 0
end