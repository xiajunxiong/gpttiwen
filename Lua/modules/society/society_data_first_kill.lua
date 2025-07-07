
function SocietyData:FirstKillConfig()
    self.kf_config = Config.first_kill_auto
	self.kf_config_dexter = self.kf_config.dexter
    self.kf_config_yewaiboss = self.kf_config.yewaiboss
end

function SocietyData:FirstKillData()
    self.first_kill_item_list = {}
    for _, v in pairs(self.kf_config_dexter) do
        if 1 == v.is_show then
            self.first_kill_item_list[v.type] = self.first_kill_item_list[v.type] or {}
            self.first_kill_item_list[v.type][v.level_group] = self.first_kill_item_list[v.type][v.level_group] or {}
            self.first_kill_item_list[v.type][v.level_group][v.seq] = SmartData.New(SocietyFirstKillInfo.New(v))
        end
    end
    self.first_kill_level_list = {}
    for type, v in pairs(self.first_kill_item_list) do
        for id, z in pairs(v) do
            self.first_kill_level_list[type] = self.first_kill_level_list[type] or {}
            local t = self.kf_config_yewaiboss[id]
            table.insert(self.first_kill_level_list[type], {
                name = t.name,
                level_id = id,
                level_desc = string.format(t.min_level == t.max_level and Language.Society.FKLevelDescOne or Language.Society.FKLevelDesc, t.min_level, t.max_level)
            })
        end 
        table.sort(self.first_kill_level_list[type], function (a, b)
            if not a or not b then return a end
            return a.level_id < b.level_id
        end)
    end
end

function SocietyData:SetServerFirstKillInfo(protocol)
    for i = 1, protocol.count do
        local t = protocol.info_list[i]
        if self.first_kill_item_list[t.type] and self.first_kill_item_list[t.type][t.level_group] and self.first_kill_item_list[t.type][t.level_group][t.seq] then
            self.first_kill_item_list[t.type][t.level_group][t.seq]:Info(t)
        end
    end
end

function SocietyData:FirstKillLevelList()
    return self.first_kill_level_list[self.first_flood_sm.type] or {}
end

function SocietyData:FirstKillItemList()
    return (self.first_kill_item_list[self.first_flood_sm.type] or {})[self.first_flood_sm.level_id] or {}
end

function SocietyData.GetFirstKillMode(type)
    return Config.first_kill_auto.challengetask[type].battle_mode
end

SocietyFirstKillInfo = SocietyFirstKillInfo or BaseClass()

function SocietyFirstKillInfo:__init(config)
    self.config = config
    self:SocietyFirstKillInfo()
end

function SocietyFirstKillInfo:SocietyFirstKillInfo()
    local boss_co = Cfg.MonsterById(self.config.boss_id)
    self.boss_name = boss_co.name
    self.boss_icon = boss_co.icon_id

    self.reward_list = {}
    local items = string.split(self.config.reward_show, "|")
    for i = 1, #items do
        local item = string.split(items[i], ":")
        table.insert(self.reward_list, Item.Init(tonumber(item[1]), tonumber(item[2]), tonumber(item[3]))) 
    end
end

function SocietyFirstKillInfo:Info(info)
    self.info = info
end

function SocietyFirstKillInfo:BossName()
    return self.boss_name
end

function SocietyFirstKillInfo:BossIcon()
    return self.boss_icon
end

function SocietyFirstKillInfo:RewardList()
    return self.reward_list
end

function SocietyFirstKillInfo:Killers()
    return self.info and self.info.killers or RichTextHelper.SizeStr(Language.Society.ZanWu, 22)
end

function SocietyFirstKillInfo:Round()
    return self.info and self.info.round_num or RichTextHelper.ColorStr(Language.Society.ZanWu, COLORSTR.Yellow9)
end

function SocietyFirstKillInfo:Time()
    return self.info and os.date("%Y-%m-%d\n%H:%M:%S",self.info.kill_timestamp) or Language.Society.ZanWu
end

function SocietyFirstKillInfo:IsFirstKill()
    return nil ~= self.info
end