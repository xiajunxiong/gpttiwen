
function SocietyData:ShiTuFBConfig()
    self.shitu_fb_reward_show = {
        [0] = {},
        [1] = {},
    }
    self.shitu_fb_details = {}
    self.shitu_fb_rewards = {}
    self.shitu_fb_detail_info_rewards = {}

    for k, v in ipairs(string.split(self.config_ma_other.master_reward, ":")) do
        table.insert(self.shitu_fb_reward_show[0], Item.Init(tonumber(v)))
    end
    for k, v in ipairs(string.split(self.config_ma_other.apprentice_reward, ":")) do
        table.insert(self.shitu_fb_reward_show[1], Item.Init(tonumber(v)))
    end
    for k, v in ipairs(self.config_ma.mentor_fb) do
        self.shitu_fb_details[v.verse_group] = self.shitu_fb_details[v.verse_group] or {}
        self.shitu_fb_details[v.verse_group][v.is_down] = v
    end
    for k, v in ipairs(self.config_ma.mentor_fb_reward) do
        self.shitu_fb_rewards[v.stage] = v
    end
    for k, v in ipairs(string.split(self.config_ma_other.reward_show, ":")) do
        table.insert(self.shitu_fb_detail_info_rewards, Item.Init(tonumber(v)))
    end
end

function SocietyData:ShiTuFBData()
    self.shitu_fb_progress_t = {}
end

function SocietyData:GetShiTuFBRewardShow()
    local type = self.is_master and SocietyConfig.ShiTuSeekingType.seek_master or SocietyConfig.ShiTuSeekingType.seek_apprentice
    return self.shitu_fb_reward_show[type]
end

function SocietyData:GetShiTuFBNpcSeq()
    return self.config_ma_other.npc_seq
end

function SocietyData:SetShiTuFbSceneInfo(protocol)
    local info = protocol.info
    self.shitu_fb_monsters = info.monsters
    local progress = 0
    local progress_t = {}
    for i = 1, info.count do
        local monster = info.monsters[i]
        if 1 == monster.is_kill then
            if progress_t[monster.group] then
                progress_t[monster.group][monster.index] = monster.index
                progress_t[monster.group].complete = 1
                progress = progress + 1
            else
                progress_t[monster.group] = {
                    group = monster.group,
                    detail = self.shitu_fb_details[monster.group],
                    complete = 0,
                    [monster.index] = monster.index,
                }
            end
        end
    end

    self.shitu_fb_sm:Set{
        end_time = info.end_time,
        progress = progress,
    }
    self.shitu_fb_progress_t = progress_t
end

function SocietyData:SetShiTuFbCommonInfo(protocol)
    local info = protocol.info
    local week_chuan_gong_progress = 0
    for k, v in ipairs(Config.mentor_cfg_auto.game_reward) do
        if 1 == bit:_rshift(info.week_chuan_gong_flag, v.grade) % 2 and v.grade > 0 then
            week_chuan_gong_progress = week_chuan_gong_progress + 1
        end
    end
    info.week_chuan_gong_progress = week_chuan_gong_progress

    self.is_get_shi_tu_reward = 1 == info.is_get_shi_tu_reward
    self.shitu_info_sm:Set(info)

    self:SocietyShiTuAchieveRedPoint()
end

function SocietyData:SetShiTuFbBattleResult(protocol)
    self.shitu_fb_battle_result = protocol.info
end

function SocietyData:GetShiTuFbBattleResult()
    return self.shitu_fb_battle_result
end

function SocietyData:GetShiTuFbDetail()
    local progress_t = {}
    for k, v in pairs(self.shitu_fb_progress_t) do
        table.insert(progress_t, v)
    end
    table.sort(progress_t, function (a, b)
        if not a or not b then return a end
        if a.complete == b.complete then
            return a.group < b.group
        else
            return a.complete > b.complete
        end
    end)
    for i, info in ipairs(progress_t) do
		info.bg_type = i % 2 + 1
    end
    
    local rewards = {}
    if not self.is_get_shi_tu_reward then
        local stage = self.shitu_fb_sm.progress
        local co = self.shitu_fb_rewards[stage] or {}
        local t = (self.is_master and co.master_reward or co.apprentice_reward) or {}
        for k, v in pairs(t) do
            table.insert(rewards, Item.Init(v.item_id, v.num, v.is_bind))
        end
        if self.is_master then
            if co.master_coin > 0 then table.insert(rewards, Item.Init(HuoBi[CurrencyType.CoinBind], co.master_coin)) end
            if co.master_chivalrous > 0 then table.insert(rewards, Item.Init(HuoBi[CurrencyType.Chivalrous], co.master_chivalrous)) end
        else
            if co.apprentice_coin > 0 then table.insert(rewards, Item.Init(HuoBi[CurrencyType.CoinBind], co.apprentice_coin)) end
        end
    end
    return progress_t, rewards
end

function SocietyData:GetShiTuFbResult()
    local progress_t = {}
    for k, v in pairs(self.shitu_fb_progress_t) do
        if 1 == v.complete then
            table.insert(progress_t, v)
        end
    end
    table.sort(progress_t, function (a, b)
        if not a or not b then return a end
        return a.group < b.group
    end)

    local rewards = {}
    local stage = self.shitu_fb_sm.progress
    local co = self.shitu_fb_rewards[stage] or {}
    local t = (self.is_master and co.master_reward or co.apprentice_reward) or {}
    for k, v in pairs(t) do
        table.insert(rewards, Item.Init(v.item_id, v.num, v.is_bind))
    end
    if self.is_master then
        if co.master_coin > 0 then table.insert(rewards, Item.Init(HuoBi[CurrencyType.CoinBind], co.master_coin)) end
        if co.master_chivalrous > 0 then table.insert(rewards, Item.Init(HuoBi[CurrencyType.Chivalrous], co.master_chivalrous)) end
    else
        if co.apprentice_coin > 0 then table.insert(rewards, Item.Init(HuoBi[CurrencyType.CoinBind], co.apprentice_coin)) end
    end
    return progress_t, rewards
end

function SocietyData:GetShiTuFbShowDesc(group, index)
    local show_desc = ""
    if self.shitu_fb_details[group] and self.shitu_fb_details[group][index] then
        show_desc = self.shitu_fb_details[group][index].name
    end
    return show_desc
end

function SocietyData:GetShiTuFBDetailInfoRewardShow()
    return self.shitu_fb_detail_info_rewards
end


function SocietyData:GetShiTuFBMonsters()
    local list = {}
    for _, monster in ipairs(self.shitu_fb_monsters or {}) do
        if 0 == monster.is_kill then
            table.insert(list, monster)
        end
    end
    return list
end
