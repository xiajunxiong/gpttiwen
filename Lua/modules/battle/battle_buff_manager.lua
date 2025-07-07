BattleBuffManager = BattleBuffManager or BaseClass()
-- local gameBuffMgr = Game.BuffManager.Singleton()
GameBuffManager = Game.BuffManager.Singleton()

local BuffAnchorType = {
    Top = 0,
    Center = 1,
    Bottom = 2,
    RightHand = 3,
    LeftHand = 4,
}

BuffStageKey = {
    Start = "Start",
    Persistence = "Persistence",
    End = "End",
    HPChange = "HPChange"
}

function BattleBuffManager:__init()
    if BattleBuffManager.Instance ~= nil then
        Debuger.LogError("[BattleBuffManager] attempt to create singleton twice!")
        return
    end
    BattleBuffManager.Instance = self

    self.displayer = BattleDisplayer.Instance
    self.battle_data = BattleData.Instance

    Game.BuffManager.GetAnchorDel = BindTool.Bind(self.GetAnchor, self)
    Game.BuffManager.PlayAnimDel = BindTool.Bind(self.PlayAnim, self)
end

-- function BattleBuffManager:PlayBuff(asset_name, game_object)
--     return gameBuffMgr:Play(asset_name, game_object)
-- end

-- function BattleBuffManager:RemoveBuff(player)
--     return gameBuffMgr:Remove(player)
-- end

function BattleBuffManager:PlayAnim(animator, anim_name)
    BodyAnim.S_PlayAnim(animator, anim_name)
end

local function FindChildByName(go, name)
    local children = go:GetComponentsInChildren(typeof(UnityEngine.Transform), true)
    if children.Length > 0 then
        for i = 0, children.Length - 1 do
            if children[i].gameObject.name == name then
                return children[i].gameObject
            end
        end
    end
end

function BattleBuffManager:GetAnchor(anchor_type, game_object)
    anchor_type = anchor_type:ToInt()
    if anchor_type == BuffAnchorType.Top then
        return game_object:FindChild("TopPoint")
    elseif anchor_type == BuffAnchorType.Center then
        return game_object:FindChild("CenterPoint")
    elseif anchor_type == BuffAnchorType.Bottom then
        return game_object
    elseif anchor_type == BuffAnchorType.LeftHand then
        return FindChildByName(game_object, BoneName.LeftHand) or game_object:FindChild("CenterPoint")
    elseif anchor_type == BuffAnchorType.RightHand then
        return FindChildByName(game_object, BoneName.RightHand) or game_object:FindChild("CenterPoint")
    end
    return game_object
end

-- data = {}
-- data.new_buff_id
-- data.new_buff_level
-- data.new_buff_key
-- data.sustain_num
-- data.buff_layer
function BattleBuffManager:AttrBuff(idx, data)
    local buff_id = data.new_buff_id
    local buff_level = data.new_buff_level
    local basic = BattleData.BuffConfig(buff_id)
    local cfg = BattleData.BuffLevelConfig(buff_id, buff_level)
    local attr_buff_list = self:AttrBuffList(buff_id, basic, cfg)
    local ch_info = self.battle_data:GetChInfo(idx)
    if ch_info and attr_buff_list then
        for k, _ in pairs(attr_buff_list) do
            ch_info.attr_buff_list[k] = ch_info.attr_buff_list[k] or 0
            ch_info.attr_buff_list[k] = ch_info.attr_buff_list[k] + 1
        end
    end
    return attr_buff_list
end
function BattleBuffManager:RemoveAttrBuff(idx, attr_buff_list)
    local ch_info = self.battle_data:GetChInfo(idx)
    if ch_info and attr_buff_list then
        for k, _ in pairs(attr_buff_list) do
            if ch_info.attr_buff_list[k] then
                ch_info.attr_buff_list[k] = ch_info.attr_buff_list[k] - 1
            end
        end
    end
end
-- buff_type 在表中查询
-- attr_buff_list[key] key显示内容在Language.Main.Battle.BuffAttr中查询
function BattleBuffManager:AttrBuffList(buff_id, basic, cfg)
    local attr_buff_list = nil
    --因为攻击是0，-0=0，解析后统一+1处理
    if basic then
        if basic.buff_type == 5 or basic.buff_type == 29 then
            attr_buff_list = {}
            local params = string.split(cfg.param_list, ",")
            local count = 1
            while count <= tonumber(params[1]) do
                if params[count * 2] then
                    local p = tonumber(params[count * 2]) + 1
                    if basic.is_positive_buff == 1 then
                        attr_buff_list[p] = true
                    else
                        attr_buff_list[-p] = true
                    end
                    count = count + 1
                else
                    LogError("buff内容解析有问题，buff_id=", buff_id)
                    break
                end
            end
        elseif basic.buff_type == 8 then
            attr_buff_list = {}
            attr_buff_list[1 + 1] = true        --防
        elseif basic.buff_type == 27 then
            attr_buff_list = {}
            attr_buff_list[666 + 1] = true      --速
        elseif basic.buff_type == 3 or basic.buff_type == 4 or      --伤 或 免 区分物理和魔法
            basic.buff_type == 23 or basic.buff_type == 41 or 
            basic.buff_type == 42 then
                -- 41 42 减益buff
            local base_idx = basic.buff_type == 4 and 9 or 11     --9:免  11:伤 (加一为对应的魔法类型)
            base_idx = base_idx + 1
            local params = string.split(cfg.param_list, ",")
            local eff_type = tonumber(params[2])    --0,物理|1,魔法|2,物理与魔法
            attr_buff_list = {}
            if eff_type == 0 then
                -- attr_buff_list[base_idx] = true
                if basic.is_positive_buff == 1 then
                    attr_buff_list[base_idx] = true
                else
                    attr_buff_list[-base_idx] = true
                end
            elseif eff_type == 1 then
                -- attr_buff_list[base_idx + 1] = true
                if basic.is_positive_buff == 1 then
                    attr_buff_list[base_idx + 1] = true
                else
                    attr_buff_list[-(base_idx + 1)] = true
                end
            elseif eff_type == 2 then
                -- attr_buff_list[base_idx] = true
                -- attr_buff_list[base_idx + 1] = true
                if basic.is_positive_buff == 1 then
                    attr_buff_list[base_idx] = true
                    attr_buff_list[base_idx + 1] = true
                else
                    attr_buff_list[-base_idx] = true
                    attr_buff_list[-(base_idx + 1)] = true
                end
            end
            -- local params = string.split(cfg.param_list, ",")
            -- attr_buff_list[100 + tonumber(params[2] + 1)] = true
        elseif basic.buff_type == 13 then
            attr_buff_list = {}
            attr_buff_list[cfg.param_list + 1] = true
        elseif basic.buff_type == 22 then
            attr_buff_list = {}
            attr_buff_list[26 + 1] = true       --抗
        end
    end
    return attr_buff_list
end
function BattleBuffManager:HasShiHua(idx)
    local ch_info = self.battle_data:GetDataChInfo(idx)
    local cfg
    if ch_info and ch_info.buff_list then
        for k, buff_id in pairs(ch_info.buff_list) do
            cfg = BattleData.BuffConfig(buff_id)
            if cfg and cfg.ui_type == 3 then
                return true
            end
        end
    end
    return false
end
function BattleBuffManager:HasBingFeng(idx)
    local ch_info = self.battle_data:GetDataChInfo(idx)
    local cfg
    if ch_info and ch_info.buff_list then
        for k, buff_id in pairs(ch_info.buff_list) do
            cfg = BattleData.BuffConfig(buff_id)
            if cfg and cfg.ui_type == 13 then
                return true
            end
        end
    end
    return false
end
BattleHaloManager = BattleHaloManager or BaseClass()
function BattleHaloManager:__init()
    if BattleHaloManager.Instance ~= nil then
        Debuger.LogError("[BattleHaloManager] attempt to create singleton twice!")
        return
    end
    BattleHaloManager.Instance = self
    self.displayer = BattleDisplayer.Instance
    self.battle_data = BattleData.Instance
    -- self.bag_data = BagData.Instance
    self.halo_list = {}
    for i = 1, 20 do
        self.halo_list[i] = {}
    end
end
function BattleHaloManager:Clear()
    for i = 1, 20 do
        self.halo_list[i] = {}
    end
end

function BattleHaloManager:AddHalo(idx, caster, halo_id)
    self.halo_list[idx][caster] = self.halo_list[idx][caster] or {}
    self.halo_list[idx][caster][halo_id] = true
    local ch_info = self.battle_data:GetChInfo(idx)
    local cfg = BagData.Instance:GetEquipEffectCfg(halo_id)
    if ch_info and cfg and not StringIsEmpty(cfg.attr_type) then
        local p = cfg.attr_type + 1
        if cfg.is_positive_buff == 1 then
            ch_info.attr_buff_list[p] = ch_info.attr_buff_list[p] or 0
            ch_info.attr_buff_list[p] = ch_info.attr_buff_list[p] + 1
        else
            ch_info.attr_buff_list[-p] = ch_info.attr_buff_list[-p] or 0
            ch_info.attr_buff_list[-p] = ch_info.attr_buff_list[-p] + 1
        end
    end
end

function BattleHaloManager:RemoveHalo(idx, caster, halo_id)
    self.halo_list[idx][caster] = self.halo_list[idx][caster] or {}
    self.halo_list[idx][caster][halo_id] = nil

    local ch_info = self.battle_data:GetChInfo(idx)
    local cfg = BagData.Instance:GetEquipEffectCfg(halo_id)
    if ch_info and cfg and not StringIsEmpty(cfg.attr_type) then
        local p = cfg.attr_type + 1
        if cfg.is_positive_buff == 1 then
            if ch_info.attr_buff_list[p] then
                ch_info.attr_buff_list[p] = ch_info.attr_buff_list[p] - 1
            end
        else
            if ch_info.attr_buff_list[-p] then
                ch_info.attr_buff_list[-p] = ch_info.attr_buff_list[-p] - 1
            end
        end
    end
end
function BattleHaloManager:CheckHalo(idx)
    if self.halo_list[idx] then
        for caster, halo_list in pairs(self.halo_list[idx]) do
            for halo_id, _ in pairs(halo_list) do
                self.displayer:AddHalo(idx, caster, halo_id)
            end
        end
    end
end
