
DuChuangZeiYingData = DuChuangZeiYingData or BaseClass()

function DuChuangZeiYingData:__init()
    if DuChuangZeiYingData.Instance ~= nil then
        Debuger.LogError("[DuChuangZeiYingData] attempt to create singleton twice!")
        return
    end
    DuChuangZeiYingData.Instance = self
    self:DuChuangZeiYingConfig()
    self:DuChuangZeiYingData()

    Cfg.Register("DuChuangZeiYingLayerBySceneId",Cfg.RealTimeSearch(Config.bandit_camp_auto.bandit_camp, "scene_id"))
end

function DuChuangZeiYingData:DuChuangZeiYingConfig()
    self.config = Config.bandit_camp_auto
    self.config_patrol = self.config.patrol
    self.config_bandit_camp = self.config.bandit_camp

    self.patrol_monsters_pos = {}
    for _, v in pairs(self.config_patrol) do
        local pos_b = string.split(v.birth_xyz, "|")
        local pos_1 = string.split(v.xyz1, "|")
        local pos_d = string.split(v.destination_xyz, "|")
        self.patrol_monsters_pos[v.npc_id] = {
            [0] = {x = tonumber(pos_b[1]), y = tonumber(pos_b[2])},
            [1] = {x = tonumber(pos_1[1]), y = tonumber(pos_1[2])},
            [2] = {x = tonumber(pos_d[1]), y = tonumber(pos_d[2])},
            [3] = {x = tonumber(pos_1[1]), y = tonumber(pos_1[2])},
        }
    end
end

function DuChuangZeiYingData:DuChuangZeiYingData()
    self.patrol_monsters_show = SmartData.New()
end

function DuChuangZeiYingData:GetDuChuangZeiYingSceneId1()
    return self.config_bandit_camp[1].scene_id
end

function DuChuangZeiYingData:IsNiuNiuInFB(vo)
    local co = self.config_bandit_camp[2]
    if co then
        return vo and vo.id == co.npc_id and SceneData.Instance:SceneId() == co.scene_id
    end
    return false
end

function DuChuangZeiYingData:GetPatrolMonsterList()
    return self.config_patrol
end

function DuChuangZeiYingData:SetDuChuangZeiYingSceneInfo(protocol)
    self.patrol_monsters_show:Set(protocol.info.obj_info)
end

function DuChuangZeiYingData:SetDuChuangZeiYingNotice(protocol)
    local co = self.config_bandit_camp[2]
    if nil == co then return end
    SceneLogic.Instance:AutoToNpc({
        id = protocol.npc_id,
        x = co.boss_x,
        y = co.boss_y,
        scene_id = co.scene_id,
    }, nil, true)
end

function DuChuangZeiYingData:GetPatrolMonsterPos(npc_id)
    return  self.patrol_monsters_pos[npc_id]
end

function DuChuangZeiYingData:PrepareScene()
    MainPlayer:SetTouchAble(true)
end

function DuChuangZeiYingData:ClearScene()
    if nil ~= MainPlayer then
        MainPlayer:SetTouchAble(false)
    end
end