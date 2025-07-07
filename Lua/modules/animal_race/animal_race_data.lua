
AnimalRaceData = AnimalRaceData or BaseClass()

function AnimalRaceData:__init()
    if AnimalRaceData.Instance ~= nil then
		Debuger.LogError("[AnimalRaceData] attempt to create singleton twice!")
		return
    end
    AnimalRaceData.Instance = self

    self:AnimalRaceConfig()

    self.obj_ids = {}
    self.ar_info = SmartData.New{
        info_type = AnimalRaceConfig.None,
    }

    self.detail_info = SmartData.New{
        info_type = AnimalRaceConfig.None,
    }
    self.anim_info = SmartData.New()
end

function AnimalRaceData:AnimalRaceConfig()
    ExternalLuaCtrl.Instance:RegisterRefresh(ExternalLua.C_AutoAnimalRace,function() 
            Cfg.UnRegister("AnimalRacePositionById")
            Cfg.Register("AnimalRacePositionById",Cfg.RealTimeSearch(Config.animal_race_auto.position,"id"))
        
            self.player_list = {}
            for _, v in pairs(Config.animal_race_auto.pets) do
                self.player_list[v.id] = v
            end
        end,true)
end

function AnimalRaceData:LoadShotsObj(shots)
    self.shots = shots
end

function AnimalRaceData:ClearScene()
    UnityGameObj.Destroy(self.shots)
    self.shots = nil

    self.obj_ids = {}
end

function AnimalRaceData:SetRAAnimalRaceInfo(protocol)
    local ar_info = protocol.ar_info
    if AnimalRaceConfig.InfoType.RoleDoing == ar_info.info_type then
        AnimalRaceCtrl.Instance:SendAnimalRaceOperAnimInfo()
    end
    self.ar_info:Set(ar_info)
    for _, obj_id in ipairs(self.obj_ids) do
        VoMgr:DelRole(obj_id)
    end
    self.obj_ids = {}
    for id = 1, protocol.count do
        local vo = CommonStruct.RoleVo(protocol.roles[id])
        local co = Cfg.AnimalRacePositionById(id)
        vo.x = co.born_x
        vo.y = co.born_y
        vo.dir_y = co.dir_y
        VoMgr:AddRole(vo)
        table.insert(self.obj_ids, vo.obj_id)
    end
end

function AnimalRaceData:SetRAAnimalRaceRoleInfo(protocol)
    self.detail_info:Set(protocol.detail_info)
end

function AnimalRaceData:SetRAAnimalRaceAnimationInfo(protocol)
    self.anim_info:Set(protocol.anim_list)
end

function AnimalRaceData:GetAnimalPosPercent(pet_id, pass_time)
    local value = 0
    if self.anim_info[pet_id] and self.anim_info[pet_id][pass_time] then
        value = self.anim_info[pet_id][pass_time]
        value = value > 10000 and 10000 or value
    end
    return value / 10000
end

function AnimalRaceData:GetResultList()
    local list = TableCopy(self.player_list)
    local rank_list = self.ar_info.rank_list
    for id, info in pairs(list) do
        info.rank = rank_list[id]
    end
    table.sort(list, function (a, b)
        if not a or not b then return a end
        return a.rank < b.rank
    end)
    return list
end

function AnimalRaceData:GetBetTypeCountById(id)
    return self.detail_info.bet_type_count and self.detail_info.bet_type_count[id] or 0
end

function AnimalRaceData:GetEverythingBetTypeCountById(id)
    return self.detail_info.everything_bet_type_count and self.detail_info.everything_bet_type_count[id] or 0
end

function AnimalRaceData:GetTypeResponseRateById(id)
    return self.detail_info.type_response_rate and self.detail_info.type_response_rate[id] or 0
end

function AnimalRaceData:GetPlayerList()
    return self.player_list
end