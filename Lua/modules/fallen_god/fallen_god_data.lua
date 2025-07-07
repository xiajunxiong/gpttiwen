FallenGodReqType = {
    TYPE_0 = 0, --请求下发信息
    TYPE_1 = 1 --请求开始战斗 param1 对应sn
}

FallenGodData = FallenGodData or BaseClass()
function FallenGodData:__init()
    if FallenGodData.Instance ~= nil then
        Debuger.LogError("[FallenGodData] attempt to create singleton twice!")
        return
    end
    FallenGodData.Instance = self

	Cfg.Register("FallenGodCfgByNpcSeq", Cfg.RealTimeSearch(self:Config().battle_list, "npc_seq"))

    self.base_info = SmartData.New({monster_list = {}, help_list = {}})
    self.rank_info = SmartData.New({})
end

function FallenGodData:__delete()
    FallenGodData.Instance = nil
end

--周一-周四显示    距离开启时间：这个是距离周四24点的时间
--周五-周六显示    剩余重置时间：这个是距离周日24点的时间
--【1准备中】【2开启中】
function FallenGodData:CheckNextWeekTime()
    local server_time = TimeManager:GetServerTime()
    local dates = os.date("%a",server_time)
    local dat_num = WeekMark[dates]

    local act_status = 2
    local begin_time, over_time = self:GetOpenDay()
    local last = over_time + 1 - dat_num
    if dat_num < begin_time then
        act_status = 1
        last = begin_time - dat_num
    end
    local date_s = os.date("*t",server_time)
    date_s.day = date_s.day + last
    return act_status, TimeCtrl.Instance:ChangDataStampTime(date_s) or 0
end
--活动是否开启中
function FallenGodData:IsActOpen()
    local act_status, next_time = self:CheckNextWeekTime()
    return act_status == 2
end
function FallenGodData:GetOpenDay()
    local open_time = self:GetBossCfg(0).open_time
    local time_list = DataHelper.SplitList(open_time, "|")
    if time_list[#time_list] == 0 then
        time_list[#time_list] = 7
    end
    return time_list[1], time_list[#time_list]
end

function FallenGodData:Config()
    return Config.dead_god_land_auto
end

function FallenGodData:OtherConfig()
    return self:Config().other[1]
end

function FallenGodData:GetMinMemberNum()
    return self:OtherConfig().challenge_num
end
function FallenGodData:GetHelpId()
    return self:OtherConfig().dead_god_land_dec
end

function FallenGodData:GetBossList()
    local show_list = {}
    for i, v in pairs(self:Config().battle_list) do
        table.insert(show_list, v)
    end
    table.sort(show_list, DataHelper.SortFunc("sn"))
    return show_list
end

function FallenGodData:GetHelpTimes(sn)
    return self.base_info.help_list[sn] or 0
end

function FallenGodData:GetBossInfo(sn)
    return self.base_info.monster_list[sn]
end

function FallenGodData:SetInfo(protocol)
    self.base_info.monster_list = protocol.monster_list
    self.base_info.help_list = protocol.help_list
end

function FallenGodData.NpcBySeq(npc_seq)
    local npc_id = NpcHelper.NpcSeqToId(npc_seq)
    if npc_id ~= nil then
        return NpcHelper.NpcCo(npc_id)
    end
end

function FallenGodData:GetBossCfg(sn)
    return self:Config().battle_list[sn]
end

--boss是否解锁
function FallenGodData:IsBossUnlock(sn)
    local boss_cfg = self:GetBossCfg(sn)
    if boss_cfg.pass_type == 0 then
        return BigDipperData.Instance:BossIsPassBySn(boss_cfg.pass_sn)
    end
    return self:BossIsPassBySn(boss_cfg.pass_sn)
end
--boss是否通关
function FallenGodData:BossIsPassBySn(sn)
    local last_info = self:GetBossInfo(sn)
    return last_info ~= nil and last_info.first_pass_flag == 1
end

function FallenGodData:SetRankData(protocol)
    self.rank_info.rank_type = protocol.info.rank_type
    self.rank_info.my_rank_index = protocol.info.my_rank_index
    self.rank_info.rank_infos = protocol.info.rank_infos
    -- LOG(self.rank_info.rank_infos, "排行榜信息 -> ")
    self.rank_info:Notify()
end

function FallenGodData:GetRankList()
    local show_list = {}
    local show_rank = 0
    for i, v in ipairs(self.rank_info.rank_infos or {}) do
        local data = {role = v.role, boss_sn = v.rank_value, round_num = v.flexible_ll}
        local last_data = show_list[#show_list]
        if last_data == nil or last_data.boss_sn ~= data.boss_sn or last_data.round_num ~= data.round_num then
            show_rank = show_rank + 1
        end
        data.show_rank = show_rank
        table.insert(show_list, data)
    end
    return show_list
end

--获取通关的sn
function FallenGodData:GetPassBossSn()
    local boss_sn = -1
    for i, v in ipairs(self:GetBossList()) do
        if not self:BossIsPassBySn(v.sn) then
            return boss_sn
        end
        boss_sn = v.sn
    end
    return boss_sn
end

function FallenGodData:GetTitleName()
    local item_id = self:OtherConfig().title_id
    local item_config = Item.GetConfig(item_id)
    local title_cfg = Config.face_cfg_auto.resource[item_config.param1]
    return title_cfg and AppearanceConfig.IconPre[2] .. title_cfg.res_id, AppearanceConfig.IconType[2] or nil
end

function FallenGodData:GetRemainBossNum()
    local num = 0
    for i, v in ipairs(self:GetBossList()) do
        if not self:BossIsPassBySn(v.sn) then
            num = num + 1
        end
    end
    return num
end
function FallenGodData:GetMaxBossNum()
    return 3
end

function FallenGodData:SetRewardList(item_list)
    self.reward_list = item_list or nil
end
function FallenGodData:GetRewardList()
    return self.reward_list
end
