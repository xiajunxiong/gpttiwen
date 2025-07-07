ShenYuRankData = ShenYuRankData or BaseClass()

GOD_SPACE_FACTION_NUM = 4    -- 一个神域中有4个阵营

JIE_SUNA_TIME_OFFSET=-7200   --排行榜结算比神域结算早2小时

-- 神域排行榜 榜单类型
GOD_SPACE_RANK_TARGET_TYPE = {
    GOD_SPACE_RANK_TARGET_TYPE_FACTION_PERSONAL = 0,    -- 阵营个人排行榜
    GOD_SPACE_RANK_TARGET_TYPE_FACTION_SERVER = 1,      -- 阵营服务器排行榜
    GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_PERSONAL = 2,  -- 神域个人排行榜
    GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_FACTION = 3,   -- 神域阵营排行榜
    GOD_SPACE_RANK_TARGET_TYPE_MAX,
}

-- 排行榜排行类型
GOD_SPACE_RANK_TYPE = {
    GOD_SPACE_RANK_TYPE_MILITARY_EXPLOITS = 0,          -- 战功排行榜
    GOD_SPACE_RANK_TYPE_ESSENCE = 1,                    -- 精华排行榜
    GOD_SPACE_RANK_TYPE_MAX,
}


function ShenYuRankData:__init()
    if ShenYuRankData.Instance ~= nil then
        Debuger.LogError("[ShenYuRankData] attempt to create singleton twice!")
        return
    end
    ShenYuRankData.Instance = self
    self.rank_sel_data = SmartData.New({
        cur_select_rank_type = 0,                       -- 排行榜类型:GOD_SPACE_RANK_TYPE
        cur_select_target_type = 0,                     -- 榜单类型:GOD_SPACE_RANK_TARGET_TYPE
        cur_sel_data_change = false,                    -- 当前选中的排行榜类型是否有数据修改
    })
    -- 排行榜数据
    self.rank_list = nil
    -- 排行榜刷新时间
    self.refresh_time = nil
    -- 各个排行榜刷新时间--只有当前选中类型的榜单到refresh_time才刷新
    self.rank_refresh_time = nil
end


function ShenYuRankData:ClearData()
    self.rank_sel_data:Clear()
    self.rank_list = nil
    self.refresh_time = nil
    self.rank_refresh_time = nil
end

-- 处理表数据
function ShenYuRankData:GetCfg()
    if self.cfg == nil then
        local cfg = Config.ranking_list_auto
        local day = TimeCtrl.Instance:GetCurOpenServerDay()
        local day_cfgs = cfg.day
        local day_cfg
        for _, v in pairs(day_cfgs) do
            day_cfg = v
            if day < v.day then
                break
            end
        end
        function GetRankCfg(degree, cfg)
            local list = {}
            for _, v in pairs(cfg) do
                if degree == v.degree then
                    table.insert(list, v)
                end
            end
            return list
        end
        self.cfg = {[GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_MILITARY_EXPLOITS] = {}, [GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_ESSENCE] = {}}

        self.cfg[GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_MILITARY_EXPLOITS][GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_PERSONAL] = GetRankCfg(day_cfg.degree, cfg.exploits_camp_personal)
        self.cfg[GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_MILITARY_EXPLOITS][GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_SERVER] = GetRankCfg(day_cfg.degree, cfg.exploits_the_server_personal)
        self.cfg[GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_MILITARY_EXPLOITS][GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_PERSONAL] = GetRankCfg(day_cfg.degree, cfg.exploits_divine_realm_personal)
        self.cfg[GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_MILITARY_EXPLOITS][GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_FACTION] = GetRankCfg(day_cfg.degree,cfg.exploits_divine_realm_camp)

        self.cfg[GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_ESSENCE][GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_PERSONAL] = GetRankCfg(day_cfg.degree, cfg.essence_camp_personal)
        self.cfg[GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_ESSENCE][GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_SERVER] = GetRankCfg(day_cfg.degree, cfg.essence_the_server_personal)
        self.cfg[GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_ESSENCE][GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_PERSONAL] = GetRankCfg(day_cfg.degree, cfg.essence_divine_realm_personal)
        self.cfg[GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_ESSENCE][GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_FACTION] = GetRankCfg(day_cfg.degree, cfg.essence_divine_realm_camp)
    end
    return self.cfg
end

--根据类型获取排行榜数据
function ShenYuRankData:GetRankDataByType(rank_type,target_type)
    if  self.rank_list==nil  then 
        -- 排行榜数据
        self.rank_list = {
            [GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_MILITARY_EXPLOITS] = {
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_PERSONAL] = {rank_list = {}, myRankInfo = {}}, 
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_SERVER] = {rank_list = {}, myRankInfo = {}}, 
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_PERSONAL] = {rank_list = {}, myRankInfo = {}},
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_FACTION] = {rank_list = {}, myRankInfo = {}}
            }, 
            [GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_ESSENCE] = {
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_PERSONAL] = {rank_list = {}, myRankInfo = {}}, 
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_SERVER] = {rank_list = {}, myRankInfo = {}}, 
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_PERSONAL] = {rank_list = {}, myRankInfo = {}},
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_FACTION] = {rank_list = {}, myRankInfo = {}}
            },
        }
    end
    return self.rank_list[rank_type][target_type]
end

--根据类型获得刷新时间
function ShenYuRankData:GetRefreshTimeByType(rank_type, target_type)
    if  self.rank_refresh_time==nil then
        self.rank_refresh_time={
            [GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_MILITARY_EXPLOITS] = {
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_PERSONAL] = 0,
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_SERVER] = 0,
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_PERSONAL] = 0,
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_FACTION] = 0,
            }, 
            [GOD_SPACE_RANK_TYPE.GOD_SPACE_RANK_TYPE_ESSENCE] = {
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_PERSONAL] = 0, 
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_FACTION_SERVER] = 0,
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_PERSONAL] = 0,
                [GOD_SPACE_RANK_TARGET_TYPE.GOD_SPACE_RANK_TARGET_TYPE_GOD_SPACE_FACTION] = 0,
            }
        }
    end
    return self.rank_refresh_time[rank_type][target_type]
end

-- 选中排行榜类型对应排名表数据
function ShenYuRankData:GetRankCfg(rank)
    local cfg = {}
    local rank_type=self:GetCurSelRankType()
    local target_type=self:GetCurSelTargetType()
    local cfgs = self:GetCfg()[rank_type][target_type]
    for i, v in pairs(cfgs) do
        local last_cfg = cfgs[i - 1]
        if rank <= v.seq and ((last_cfg and rank > last_cfg.seq) or not last_cfg) then
            cfg = v
            break
        end
    end
    return cfg
end

--当前选中排行榜类型
function ShenYuRankData:GetCurSelRankType()
    if self.rank_sel_data then
        return self.rank_sel_data.cur_select_rank_type or 0
    end
    return 0
end


-- 当前选中榜单类型
function ShenYuRankData:GetCurSelTargetType()
    if self.rank_sel_data then
        return self.rank_sel_data.cur_select_target_type or 0
    end
    return 0
end

--数据下发处理
-- @param:协议数据
-- @param:GOD_SPACE_RANK_TARGET_TYPE
function ShenYuRankData:OnRankDataChange(protocol, target_type)
    local rank_type=protocol.rank_type 
    local list_data=self:GetRankDataByType(rank_type,target_type)
    list_data.rank_list = protocol.rank_list
    list_data.myRankInfo = protocol.myRankInfo
    -- 检查当前选中排行榜类型数据更新
    if rank_type == self:GetCurSelRankType() and target_type == self:GetCurSelTargetType() then
        self.rank_sel_data.cur_sel_data_change = not self.rank_sel_data.cur_sel_data_change
    end
end

-- 当前排行榜选中数据
function ShenYuRankData:GetCurSelData()
    local my_rank_info = {}
    local rank_type = self:GetCurSelRankType()
    local target_type = self:GetCurSelTargetType()
    local data =self:GetRankDataByType(rank_type,target_type) 
    local myRankInfo = data.myRankInfo
    local myCamp = LandsBetweenData.Instance.my_info.faction or 0
    local camp = Config.ranking_list_auto.camp_id[myCamp + 1].id
    if target_type ~= 3 then
        local name = RoleData.Instance:GetBaseData().name
        local account_key = LoginData.Instance:GetAccountKey()
        local last_login_server = LoginData.Instance:getAccountCacheData(account_key).last_login_server
        local server_info = LoginData.Instance:GetServerInfoById(last_login_server)
        local server
        if server_info ~= nil then
            if IS_EDITOR then
                server = string.format(Language.Login.ServerNameEditor, server_info.id, server_info.name)
            else
                server = string.format(Language.Login.ServerName, server_info.name)
            end
        end
        my_rank_info.myInfo = string.format(Language.ShenYuRank.rankInfo, name .. "\n", server .. "\n", camp)
        my_rank_info.head_id = RoleData.Instance:GetRoleAvatarId()
        my_rank_info.head_type = RoleData.Instance:GetRoleAvatarType()
        my_rank_info.head_quality = RoleData.Instance:GetRoleAvatarQuality()
        my_rank_info.rank = myRankInfo and myRankInfo.rank or "--"
        my_rank_info.value = string.format(Language.ShenYuRank.my_rank_value[rank_type], "", (myRankInfo and myRankInfo.rank_value or "--")) 
    else -- 阵营排行
        my_rank_info.campImg = Config.ranking_list_auto.camp_id[myCamp + 1].icon_id
        my_rank_info.myInfo = string.format(Language.ShenYuRank.rankInfo, "", "", camp)
        my_rank_info.rank = myRankInfo and myRankInfo.rank or "--"
        my_rank_info.value = string.format(Language.ShenYuRank.my_rank_value[rank_type], Language.ShenYuRank.camp,(myRankInfo and myRankInfo.rank_value or "--")) 
    end

    local refresh_time =self:GetRefreshTimeByType(rank_type,target_type)  
    local cur_time = TimeCtrl.Instance:GetServerTime()
    if cur_time >= refresh_time then
        self.rank_refresh_time[rank_type][target_type] = cur_time+300
        ShenYuRankCtrl.Instance:ReqRank()
    end
    return {rank_list = data.rank_list, my_rank_info = my_rank_info}
end

function ShenYuRankData:ResetUpdateTime()
    local cur_time = TimeCtrl.Instance:GetServerTime()
    if self.refresh_time and cur_time>=self.refresh_time - 285 then
        self.refresh_time=nil
        self.rank_refresh_time=nil
    end
end