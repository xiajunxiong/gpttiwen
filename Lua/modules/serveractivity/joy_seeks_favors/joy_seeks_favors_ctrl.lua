FILE_REQ("modules/serveractivity/joy_seeks_favors/joy_seeks_favors_data")
VIEW_REQ("modules/serveractivity/joy_seeks_favors/joy_seeks_favors")
VIEW_REQ("modules/serveractivity/joy_seeks_favors/joy_seeks_favors_tip")
JoySeeksFavorsCtrl = JoySeeksFavorsCtrl or BaseClass(BaseCtrl)
BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE = {
    INVALID = 0,
    OPEN = 1, --寻宠一个位置 p1 = open_map p2 = x p3 =y
    ITEM = 2, --使用道具 o1 = open_map p2 = x p3 = y p4 = face_pos p5 = item_seq
    SET = 3, --放置 p1 = open_map  p2 = x p3 = y   p4 = face_pos p5 = joy_type[1,2,3]
    MAX = 3
}
MATCH_TYPE = {
    NONE = 0,
    --未匹配
    MATCHING = 1,
    --匹配中
    SETTING = 2,
    --设置阶段
    OPERATION = 3,
    --操作阶段
    SHOW = 4
    --展示阶段
}
function JoySeeksFavorsCtrl:__init()
    if JoySeeksFavorsCtrl.Instance ~= nil then
        Debuger.LogError("[JoySeeksFavorsCtrl] attempt to create singleton twice!")
        return
    end
    JoySeeksFavorsCtrl.Instance = self
    self.data = JoySeeksFavorsData.New()
    self:RegisterAllProtocols()
    --注册结束事件
    FinishData.Instance:SetCloseFunc(
        BATTLE_MODE.BATTLE_MODE_JOYSEEKS,
        function()
            self.data:Clear()
        end
    )
    FinishData.Instance:FailRegister(BATTLE_MODE.BATTLE_MODE_JOYSEEKS,{value_func = function()
        return {}
    end,
    })
end
function JoySeeksFavorsCtrl:OnUnloadGameLogic()
    self.data:Clear()
end
function JoySeeksFavorsCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAJoySeeksFavorsInfo, "OnRAJoySeeksFavorsInfo")
    self:RegisterProtocol(SCRAJoySeeksFavorsMatchSucc, "OnRAJoySeeksFavorsMatchSucc")
    self:RegisterProtocol(CSRAJoySeeksFavorsMatchReq)
    self:RegisterProtocol(SCBattleSpecialInfo, "OnSCBattleSpecialInfo")
    self:RegisterProtocol(SCBattleSpecialOneMapChangeInfo, "OnBattleSpecialOneMapChangeInfo")
    self:RegisterProtocol(CSBattleSpecialJoyReqInfo)
end
function JoySeeksFavorsCtrl:OnRAJoySeeksFavorsInfo(protocol)
    -- LogError("匹配状态",protocol.is_match)
    self.data:ViewData().is_match = protocol.is_match
end
function JoySeeksFavorsCtrl:OnRAJoySeeksFavorsMatchSucc(protocol)
    -- LogError("通知倒计时")
    self.data:ViewData().is_match = MATCH_TYPE.SETTING
    --[[ local base_data = BattleData.Instance:BaseData()
    base_data.battle_mode = BATTLE_MODE.BATTLE_MODE_JOYSEEKS ]]
end

function JoySeeksFavorsCtrl:OnSCBattleSpecialInfo(protocol)
    self.data:ViewData().is_match = MATCH_TYPE.SETTING
    self.data:ViewData().ready_tamp = protocol.ready_tamp
    --需要通过role_id判断谁是敌人
    local id = RoleData.Instance:GetRoleId()
    if protocol.myself_map.role_info.role_id == id then
        self.data:SetMySelfMap(protocol.myself_map)
        self.data:SetEnemyMap(protocol.enemy_map)
        self.data:ViewData().map_index = protocol.myself_map.map_index
        self.data:ViewData().map_index2 = protocol.enemy_map.map_index
    else
        self.data:SetMySelfMap(protocol.enemy_map)
        self.data:SetEnemyMap(protocol.myself_map)
        self.data:ViewData().map_index = protocol.enemy_map.map_index
        self.data:ViewData().map_index2 = protocol.myself_map.map_index
    end
    -- LogError("收到比赛信息",protocol)
end

function JoySeeksFavorsCtrl:OnBattleSpecialOneMapChangeInfo(protocol)
    -- LogError("比赛信息变化", protocol)
    self.data:ViewData().show_tamp = protocol.show_tamp
    self.data:ViewData().next_move_tamp = protocol.next_move_tamp

    if self.data:ViewData().show_tamp == 0 and self.data:ViewData().next_move_tamp == 0 then
        self.data:ViewData().is_match = MATCH_TYPE.SETTING
    elseif self.data:ViewData().show_tamp == 0 and self.data:ViewData().next_move_tamp > 0 then
        --操作阶段要分辨是谁在操作
        self.data:ViewData().is_match = MATCH_TYPE.OPERATION
    elseif self.data:ViewData().show_tamp > 0 and self.data:ViewData().next_move_tamp > 0 then
        self.data:ViewData().is_match = MATCH_TYPE.SHOW
    end
    local id = RoleData.Instance:GetRoleId()

    if protocol.has_find > 0 then
        --找到了
        if protocol.joy_seq > 0 then
            local names = Language.JoySeeksFavors.EggName
            -- PublicPopupCtrl.Instance:Center(
            --     string.format(Language.JoySeeksFavors.Tips, names[protocol.joy_seq], protocol.has_find == 1 and Language.JoySeeksFavors.Succ or Language.JoySeeksFavors.Fail)
            -- )
            self.data:SetCatchTips(string.format(Language.JoySeeksFavors.Tips, names[protocol.joy_seq], protocol.has_find == 1 and Language.JoySeeksFavors.Succ or Language.JoySeeksFavors.Fail))
            if protocol.has_find == 1 then
                self.data:EyEggInfo(protocol.joy_seq)
            else
                self.data:MyEggInfo(protocol.joy_seq)
            end
        else
            -- LogError(final_find)
            local is_myself = protocol.map_info.role_info.role_id == id
            local old_map = is_myself and self.data:MySelfMap() or self.data:EnemyMap()
            local old_open = {}
            if next(old_map) == nil then
            else
                old_map = old_map.player_joy_map
                for i = 1, 5 do
                    for j = 1, 5 do
                        if old_map[i][j].is_open == 1 and old_map[i][j].joy_seq > 0 then
                            old_open[old_map[i][j].joy_seq] = true
                        end
                    end
                end
            end

            local map = protocol.map_info.player_joy_map --5*5
            local open = {}
            for i = 1, 5 do
                for j = 1, 5 do
                    if map[i][j].is_open == 1 and map[i][j].joy_seq > 0 then
                        open[map[i][j].joy_seq] = map[i][j].joy_seq
                    end
                end
            end
            local final_find = {}
            for key, value in pairs(open) do
                if old_open[value] == true then
                else
                    table.insert(final_find, value)
                end
            end
            local names = Language.JoySeeksFavors.EggName
            for i, v in ipairs(final_find) do
                -- PublicPopupCtrl.Instance:Center(
                --     string.format(Language.JoySeeksFavors.Tips, names[v], protocol.has_find == 1 and Language.JoySeeksFavors.Succ or Language.JoySeeksFavors.Fail)
                -- )
                self.data:SetCatchTips(string.format(Language.JoySeeksFavors.Tips, names[v], protocol.has_find == 1 and Language.JoySeeksFavors.Succ or Language.JoySeeksFavors.Fail))
                if protocol.has_find == 1 then
                    self.data:EyEggInfo(v)
                else
                    self.data:MyEggInfo(v)
                end
            end
        end
    end
    self.data:SetFindInfo(protocol)
    --[[ self.has_find = MsgAdapter.ReadChar()--[0-2]无找到了被找到了
    self.reserve_ch = MsgAdapter.ReadChar()
    self.joy_seq = MsgAdapter.ReadShort()--[0-3]无小中大
    self.map_info = JoySeeksFavorsMap() ]]
    if protocol.map_info.role_info.role_id == id then
        self.data:SetMySelfMap(protocol.map_info)
        self.data:ViewData().map_index = protocol.map_info.map_index
        self.data:ViewData().map_index2 = protocol.map_info.map_index == 0 and 1 or 0
    else
        self.data:SetEnemyMap(protocol.map_info)
        self.data:ViewData().map_index2 = protocol.map_info.map_index
        self.data:ViewData().map_index = protocol.map_info.map_index == 0 and 1 or 0
    end

    -- LogError("展示阶段",self.data:ViewData().show_tamp > 0 and self.data:ViewData().next_move_tamp > 0)
    if protocol.map_info.map_index == self.data:ViewData().map_index then
        self.data:SetMySelfMap(protocol.map_info)
        if self.data:ViewData().is_match == MATCH_TYPE.OPERATION then
            self.data:ViewData().opera = false
        end
    else
        self.data:SetEnemyMap(protocol.map_info)
        if self.data:ViewData().is_match == MATCH_TYPE.OPERATION then
            self.data:ViewData().opera = true
        end
    end
end

function JoySeeksFavorsCtrl:JoySeeksFavorsMatchReq(is_match)
    local protocol = ProtocolPool.Instance:GetProtocol(CSRAJoySeeksFavorsMatchReq)
    protocol.is_match = is_match
    --0取消1匹配
    SendProtocol(protocol)
end
function JoySeeksFavorsCtrl:BattleSpecialJoyReqInfo(param_t)
    -- LogError(param_t)
    local protocol = ProtocolPool.Instance:GetProtocol(CSBattleSpecialJoyReqInfo)
    protocol.operate_type = param_t.operate_type
    protocol.param1 = param_t.param1
    protocol.param2 = param_t.param2
    protocol.param3 = param_t.param3
    protocol.param4 = param_t.param4
    protocol.param5 = param_t.param5
    SendProtocol(protocol)
end
function JoySeeksFavorsCtrl:Set(x, y, dir, joy)
    local param_t = {}
    param_t.operate_type = BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE.SET
    param_t.param1 = self.data:ViewData().map_index
    param_t.param2 = x
    param_t.param3 = y
    param_t.param4 = dir
    param_t.param5 = joy
    self:BattleSpecialJoyReqInfo(param_t)
end
function JoySeeksFavorsCtrl:Open(x, y)
    local param_t = {}
    param_t.operate_type = BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE.OPEN
    param_t.param1 = self.data:ViewData().map_index2
    param_t.param2 = x
    param_t.param3 = y
    param_t.param4 = 0
    param_t.param5 = 0
    self:BattleSpecialJoyReqInfo(param_t)
end
function JoySeeksFavorsCtrl:Item(x, y, dir)
    local param_t = {}
    param_t.operate_type = BATTLE_JOY_SEEKS_FAVORS_OPERATE_TYPE.ITEM
    param_t.param1 = self.data:ViewData().map_index2
    param_t.param2 = x
    param_t.param3 = y
    param_t.param4 = dir
    param_t.param5 = 0
    self:BattleSpecialJoyReqInfo(param_t)
end
