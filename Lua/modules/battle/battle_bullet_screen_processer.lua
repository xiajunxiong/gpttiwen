BattleBulletScreenProcesser = BattleBulletScreenProcesser or BaseClass(BaseCtrl)

function BattleBulletScreenProcesser:__init()
    if BattleBulletScreenProcesser.Instance ~= nil then
        Debuger.LogError("[BattleBulletScreenProcesser] attempt to create singleton twice!")
        return
    end
    BattleBulletScreenProcesser.Instance = self

    local ch_infos = BattleData.Instance:ChInfo()
    ch_infos:Care(
        function(old_kv)
            if not old_kv then
                return
            end
            for idx, old_info in pairs(old_kv) do
                if not ch_infos[idx] or ch_infos[idx].curhp <= 0 then
                    self:CheckAll(1, old_info)
                    for s = 0, 1 do
                        local flag = true
                        for i = s * 10 + 1, s * 10 + 10 do
                            if ch_infos[i] then
                                flag = false
                                break
                            end
                        end
                        if flag then
                            self:CheckAll(2)
                            break
                        end
                    end
                    break
                end
            end
        end
    )
end

function BattleBulletScreenProcesser:Init()
    self.showed_list = {}
end

local function compare(compare_type, left, right)
    if compare_type == 0 then
        return left > right
    elseif compare_type == 1 then
        return left < right
    elseif compare_type == 2 then
        return left == right
    elseif compare_type == 3 then
        return left ~= right
    elseif compare_type == 4 then
        return left >= right
    elseif compare_type == 5 then
        return left <= right
    elseif compare_type == 6 then
        return right % left == 0
    elseif compare_type == 7 then
        return right % left ~= 0
    end
end

local function get_grid_idx(idx, type)
    if not type or type == 0 then
        return idx + 1
    end
    if type == 1 then
        return idx + 11
    end
end

local conditions = {
    [0] = function(compare_type, compare_value, param1, param2, grid_idx)
        return compare(compare_type, compare_value, BattleData.Instance:BaseData().round)
    end,
    [1] = function(compare_type, compare_value, param1, param2, grid_idx)
        if grid_idx and grid_idx ~= get_grid_idx(param1, 1) then--触发者和格点相等
            return false
        end
        local ch_info = BattleData.Instance:GetChInfo(get_grid_idx(param1, 1))
        if ch_info then
            return compare(compare_type, compare_value, ch_info.character_id)
        end
        return false
    end,
    [2] = function(compare_type, compare_value, param1, param2, grid_idx)
        if grid_idx and grid_idx ~= get_grid_idx(param1, 1) then--触发者和格点相等
            return false
        end
        local ch_info = BattleData.Instance:GetChInfo(get_grid_idx(param1, 1))
        if ch_info then
            if ch_info.character_id == 0 or ch_info.character_id == param2 then
                return compare(compare_type, compare_value / 100, ch_info.curhp / ch_info.maxhp)
            end
        end
        return false
    end,
    [3] = function(compare_type, compare_value, param1, param2, grid_idx, params)
        local num = 0
        for idx, ch_info in pairs(BattleData.Instance:ChInfo()) do
            if idx > 10 then
                if param1 == 0 then
                    num = num + 1
                else
                    if ch_info.character_id == param1 then
                        num = num + 1
                    end
                end
            end
        end
        if param1 > 10 and params then
            if params.character_id ~= param1 then
                return false
            end
        end
        return compare(compare_type, compare_value, num)
    end,
    [4] = function(compare_type, compare_value, param1, param2, grid_idx)
        if grid_idx and grid_idx ~= get_grid_idx(param2, param1) then
            return false
        end
        local ch_info = BattleData.Instance:GetChInfo(get_grid_idx(param2, param1))
        if ch_info then
            return compare(compare_type, compare_value, ch_info.curhp / ch_info.maxhp)
        end
        return false
    end,
    [5] = function(compare_type, compare_value, param1, param2, grid_idx)
        local num = 0
        for idx, ch_info in pairs(BattleData.Instance:ChInfo()) do
            if idx <= 10 then
                if (param1 == 0 and ch_info.character_type == BATTLE_CH_TYPE.PET) or (param2 == 0 and ch_info.character_type == BATTLE_CH_TYPE.PARTNER) then
                else
                    num = num + 1
                end
            end
        end
        return compare(compare_type, compare_value, num)
    end,
    [6] = function(compare_type, compare_value, param1, param2, grid_idx)
        if grid_idx and grid_idx ~= get_grid_idx(param2, param1) then
            return false
        end
        local ch_info = BattleData.Instance:GetChInfo(get_grid_idx(param2, param1))
        if ch_info then
            return compare(compare_type, compare_value / 100, ch_info.curmp / ch_info.maxmp)
        end
        return false
    end,
    [7] = function(compare_type, compare_value, param1, param2, grid_idx)
        if grid_idx and grid_idx ~= get_grid_idx(param2, param1) then
            return false
        end
        local ch = BattleCtrl.Instance.displayer:getCh(get_grid_idx(param2, param1))
        if ch then
            for k, v in pairs(ch.buff_list) do
                if compare(compare_type, compare_value == 1, Cfg.BuffByBuffID(v.buff_id).buff_type == 1) then
                    return true
                end
            end
        end
        return false
    end,
    [8] = function(compare_type, compare_value, param1, param2, grid_idx)
        if grid_idx and grid_idx ~= get_grid_idx(param2, param1) then
            return false
        end
        local ch = BattleCtrl.Instance.displayer:getCh(get_grid_idx(param2, param1))
        if ch then
            for k, v in pairs(ch.buff_list) do
                if compare(compare_type, compare_value == 1, Cfg.BuffByBuffID(v.buff_id).buff_type == 2) then
                    return true
                end
            end
        end
        return false
    end,
    [9] = function (compare_type, compare_value, param1, param2, grid_idx)
        if grid_idx and grid_idx ~= get_grid_idx(param1, param2) then
            return false
        end
        local ch = BattleCtrl.Instance.displayer:getCh(get_grid_idx(param1, param2))
        if ch then
            return compare(compare_type, compare_value, BattleData.Instance:BaseData().round)
        end
        return false
    end
}

function BattleBulletScreenProcesser:IsMeetCondition(condition_seq, grid_idx, params)
    local cfg = Cfg.BulletScreenById3(condition_seq)
    if not cfg then
        LogError("在弹幕条件表中找不到", condition_seq)
        return
    end
    local flag = conditions[cfg.condition_type](cfg.compare_type, cfg.compare_value, cfg.param1, cfg.param2, grid_idx, params)
    return flag
end
local bullet_color_str =
{
    COLORSTR.Yellow12,
    COLORSTR.Green8,
    COLORSTR.Blue11
}
function BattleBulletScreenProcesser:Excute(event_seq, param, bullet_type)
    local cfg = Cfg.BulletScreenById2(event_seq)
    if not cfg then
        LogError("在弹幕事件表中找不到", event_seq)
        return
    end
    if not param or cfg.show_name == 0 then
        BattleData.Instance.bullet_comments:Insert({text = string.format("<color=#%s>%s</color>", bullet_color_str[GameMath.Ramdon(1, 3)], cfg.bullet_screen_content)})
        ChatCtrl.Instance:ChannelChatCustom({msg_type = ChatConfig.MsgType.bullet, content = cfg.bullet_screen_content, block_role = true}, ChatConfig.ChannelId.general)
    elseif type(param) == "string" then
        BattleData.Instance.bullet_comments:Insert(
            {text = string.format("<color=#%s>%s：%s</color>", bullet_color_str[GameMath.Ramdon(1, 3)], param, cfg.bullet_screen_content)}
        )
        ChatCtrl.Instance:ChannelChatCustom({msg_type = ChatConfig.MsgType.bullet, content = string.format("%s：%s", param, cfg.bullet_screen_content), block_role = true}, ChatConfig.ChannelId.general)
    elseif type(param) == "number" then
        --一般是格点
        local ch = BattleData.Instance:GetChInfo(param)
        if ch ~= nil then
            if bullet_type == 1 then
                BattleCtrl.Instance.displayer:PopHp(param, BATTLE_HP_DESC.BUBBLE, cfg.bullet_screen_content, cfg.duration)
            else
                BattleData.Instance.bullet_comments:Insert(
                {text = string.format("<color=#%s>%s：%s</color>", bullet_color_str[GameMath.Ramdon(1, 3)], ch.name, cfg.bullet_screen_content)}
                )
            end
            ChatCtrl.Instance:ChannelChatCustom({msg_type = ChatConfig.MsgType.bullet, content = string.format("%s：%s", ch.name, cfg.bullet_screen_content), block_role = true}, ChatConfig.ChannelId.general)        
        else
            -- BattleCtrl.Instance.displayer:PopHp(param, BATTLE_HP_DESC.BUBBLE, cfg.bullet_screen_content, cfg.duration)
            BattleData.Instance.bullet_comments:Insert({text = string.format("<color=#%s>%s</color>", bullet_color_str[GameMath.Ramdon(1, 3)], cfg.bullet_screen_content)})
            ChatCtrl.Instance:ChannelChatCustom({msg_type = ChatConfig.MsgType.bullet, content = cfg.bullet_screen_content, block_role = true}, ChatConfig.ChannelId.general)
        end
    end
end

function BattleBulletScreenProcesser:CheckAndExcute(bullet_screen_id, timing_type, grid_idx, name, params, bullet_type)
    local cfg = Cfg.BulletScreenById1(bullet_screen_id)
    if not cfg then
        LogError("在战斗弹幕表中找不到", bullet_screen_id)
        return
    end
    if cfg.check_point ~= timing_type then
        return
    end
    if cfg.only_once and cfg.only_once == 1 and self.showed_list[bullet_screen_id] then
        return
    end
    self.showed_list[bullet_screen_id] = true
    local flag = true
    for i = 0, 5 do
        local c_seq = cfg["condition_" .. i]
        if c_seq ~= "" then
            if not self:IsMeetCondition(c_seq, grid_idx, params) then
                flag = false
                break
            end
        end
    end
    if flag then
        for i = 0, 5 do
            local e_seq = cfg["event_" .. i]
            if e_seq ~= "" then
                self:Excute(e_seq, name or grid_idx, bullet_type or 0)
            end
        end
    else
        for i = 0, 2 do
            local e_seq = cfg["opposite_event_" .. i]
            if e_seq ~= "" then
                self:Excute(e_seq, name or grid_idx, bullet_type or 0)
            end
        end
    end
end
--根据类型检查，是否发送弹幕
--0是回合开始
--1是死亡的时候
--2是整排死亡的时候
--3是回合结束
function BattleBulletScreenProcesser:CheckAll(type, params)
    self.showed_list = self.showed_list == nil and {} or self.showed_list
    local monster_group_id = BattleData.Instance:BaseData().monster_group_id
    if monster_group_id > 0 then
        local g_cfg = Cfg.MonsterGroupById(monster_group_id)
        local bullet_screen_list = string.split(g_cfg.bullet_screen_id,"|")
        for i = 1, 6 do
            local bullet_screen_id = bullet_screen_list[i]-- g_cfg["bullet_screen_id_" .. i]
            if bullet_screen_id and bullet_screen_id ~= "" then
                self:CheckAndExcute(tonumber(bullet_screen_id), type, nil, nil, params)
            end
        end
    -- if g_cfg.bullet_screen_id ~= "" then
    --     self:CheckAndExcute(g_cfg.bullet_screen_id, type)
    -- end
    end
    for idx, ch_info in pairs(BattleData.Instance:ChInfo()) do
        if ch_info.character_type == BATTLE_CH_TYPE.MONSTER then
            local m_cfg = Cfg.MonsterById(ch_info.character_id)
            if m_cfg then
                local bullet_screen_list = string.split(m_cfg.bullet_screen_id,"|")
                local bubble_list = string.split(m_cfg.bubble_id,"|")
                for i = 1, 6 do
                    local bullet_screen_id = bullet_screen_list[i]--m_cfg["bullet_screen_id_" .. i]
                    if bullet_screen_id and bullet_screen_id ~= "" then
                        self:CheckAndExcute(tonumber(bullet_screen_id), type, idx, m_cfg.name, params)
                    end
                    local bubble_id = bubble_list[i]--m_cfg["bubble_id_" .. i]
                    if bubble_id and bubble_id ~= "" then
                        self:CheckAndExcute(tonumber(bubble_id), type, idx, nil, params, 1)
                    end
                end
            -- if m_cfg.bullet_screen_id ~= "" then
            --     self:CheckAndExcute(m_cfg.bullet_screen_id, type)
            -- end
            -- if m_cfg.bubble_id ~= "" then
            --     self:CheckAndExcute(m_cfg.bubble_id, type, idx)
            -- end
            end
        end
    end
end
