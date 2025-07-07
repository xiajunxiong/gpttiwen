Monster = Monster or BaseClass()

function Monster:__init(info)
    self.monster_id = info.monster_id or info.id or info.item_id
    self.name = info.name
    self.info = info
    self.data = MonsterData.Instance
    self.enter_func = info.enter_func
    self.funcs = info.funcs
    self.config = self:Cfg() or {}
    self.is_quality = info.is_quality or false
    self.quality_name = info.quality_name or nil
end

function Monster:ID()
    return self.monster_id
end

function Monster:Name()
    return self.name or self.config.name
end

function Monster:Cfg()
    return Cfg.MonsterById(self.monster_id)
end

function Monster:QuaIcon()
    if self.quality_name then
        return self.quality_name
    end
    if self.is_quality then
        if self.pet_config == nil then
            self.pet_config = PetData.Instance:GetPetCfg(self:IconId())
        end
        if self.pet_config then
            return PetData.PinZhi[self.pet_config.quality]
        end
    end
end

function Monster:IconId()
    return self.config.icon_id or 0
end
function Monster:IsChaos()
    return self.config.is_chaos == 1
end
function Monster:EleAttr()
    return MonsterData.GetElement(self.config)
end

function Monster:Race()
    local co = Config.pet_cfg_auto.pet_info_2[self:IconId()]
    return co and co.race or -1
end

MonsterData = MonsterData or BaseClass()

function MonsterData:__init()
    if MonsterData.Instance ~= nil then
        Debuger.LogError("[MonsterData] attempt to create singleton twice!")
        return
    end
    MonsterData.Instance = self
    self:InitBossGroupData()
end

-- 获取元素属性 return {type = ,type = }
function MonsterData.GetElement(t)
    local res = {}
    if next(t) == nil then
        return res
    end
    for k, v in pairs(BattleAttriElement) do
        local key = string.lower(k)
        if t[key] > 0 then
            table.insert(res, v, t[key])
        end
    end
    return res
end

--根据怪物组id获取怪物组等级
function MonsterData.GetLevel(group_id)
    local group_config = Cfg.MonsterGroupById(group_id) or {}
	local monster_config = Cfg.MonsterById(group_config.show_monster)
	return monster_config.monster_level or 0
end

function MonsterData.GetShowConfig(group_id)
    local group_config = Cfg.MonsterGroupById(group_id) or {}
	return Cfg.MonsterById(group_config.show_monster)
end

function MonsterData:InitBossGroupData()
	self.boss_group_list = {}
	for k,v in pairs(Config.npc_auto.npc_list) do
		if v.behavior_type == 4 then
			self.boss_group_list[v.param1] = v
		end
	end
end

function MonsterData:GetGroupConfig(group_id)
    return self.boss_group_list[group_id] or Cfg.MonsterGroupById(group_id)
end