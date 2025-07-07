DuelData = DuelData or BaseClass()
function DuelData:__init()
    if DuelData.Instance ~= nil then
        Debuger.LogError("[DuelData] attempt to create singleton twice!")
        return
    end
    DuelData.Instance = self

    self.duel_auto = Config.duel_auto
    self.other_cfg = self.duel_auto.other[1]

    self.duel_level = 60    --开启等级
    self.duel_info = {}
    self.last_scene_id = -1
end

function DuelData:OnInit()
    self.NameColor = {
        [0] = COLORS.Red18,
        [2] = COLORS.Red19,
        [3] = COLORS.Red20,
        [4] = COLORS.Red7,
    }
end

function DuelData:IsOpen()
    return RoleData.Instance:GetRoleLevel() >= self.duel_level
end

--是否可以决斗
function DuelData:IsCanDuel(vo)
    return vo ~= nil and vo.level >= self.duel_level and self:IsOpen()
end

--是否野外场景
function DuelData:IsDuelScene()
    return self.duel_auto.scene_id[SceneData.Instance:SceneId()] ~= nil
end

function DuelData:SetLastSceneId(scene_id)
    self.last_scene_id = scene_id or Scene.Instance:SceneId()
end
function DuelData:GetLastSceneId()
    return self.last_scene_id
end

function DuelData:GetKillValueConfig(kill_value)
    for i, v in ipairs(self.duel_auto.kill_value) do
        if v.kill_value_kill_value <= kill_value and kill_value <= v.kill_value_max then
            return v
        end
    end
end

--对应罪恶值名字颜色
function DuelData:GetNameColor(kill_value)
    local config = self:GetKillValueConfig(kill_value)
    if config ~= nil then
        return self.NameColor[config.kill_level] or self.NameColor[#self.NameColor]
    end
end

function DuelData:IsAttacker()
    for i, v in pairs(self.duel_info.att_list) do
        if v == RoleData.Instance:GetRoleId() then
            return true
        end
    end
end

function DuelData:IsDefender()
    for i, v in pairs(self.duel_info.def_list) do
        if v == RoleData.Instance:GetRoleId() then
            return true
        end
    end
end

function DuelData:GetInfo()
    return self.duel_info
end

function DuelData:SetDuelInfo(protocol)
    self.duel_info = protocol.duel_info
end

function DuelData:DuelBuff()
    local kill_value = RoleData.Instance:GetBaseData().kill_value or 0
    local config = self:GetKillValueConfig(kill_value)
    if config ~= nil and config.attr_down > 0 then
        return {buff_name = Language.Duel.DebuffName, icon_id = "b369", desc = string.format(Language.Duel.DebuffDesc, config.attr_down)}
    end
end

function DuelData:GetDuelToken()
    return self.other_cfg.duel_taken
end

function DuelData:IsDuelTitle(title_id)
    for i, v in ipairs(self.duel_auto.kill_value) do
        if v.titles_id == title_id then
            return true
        end
    end
    return false
end
