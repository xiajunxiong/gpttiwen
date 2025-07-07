YinYangData = YinYangData or BaseClass()
YinYangData.ShieldType =
{
    Scene = 1,
    Panel = 2,
}
function YinYangData:__init()
    YinYangData.Instance = self

    self.all_info = Config.yinyang_vision_auto
    self.radus = self.all_info.other[1].box_radius
    self.remindx_radius = self.all_info.other[1].remindx_radius or 24
    self.unlock_task = self.all_info.other[1].task_id or 60235
    self.anecdote_npc = self.all_info.scene_NPC
    self.sp_shield = self.all_info.sp_shield

    self.objs_list = {}
    self.explore_data = SmartData.New({vo = nil, near_vo = nil, near_distance = 0, npc_vo = nil})
    self.view_data = SmartData.New({opened = false, show = true})
    self:init_config()
    self.box_flag = {}
    self.npc_flag = {}
    self.cur_box_finish_seq = -1
    self.cur_npc_finish_seq = -1
end
function YinYangData:__delete()
end
function YinYangData:Clear()
    TimeHelper:CancelTimer(self.range_invoke_timer)
    self.view_data.opened = false
    self.explore_data.vo = nil
    self.explore_data.near_vo = nil
    self.explore_data.near_distance = 0
end
function YinYangData:init_config()
    --按场景分
    for index, value in ipairs(self.all_info.box) do
        value.obj_type = SceneObjType.YinYangObj
        value.show_type = 0
        value.id = value.npc_id
         --区分类型
        value.is_server = true
         --点击事件
        self.objs_list[value.scene_id] = self.objs_list[value.scene_id] or {}
        table.insert(self.objs_list[value.scene_id], value)
    end
    for index, value in ipairs(self.all_info.NPC) do
        value.obj_type = SceneObjType.YinYangObj
        value.show_type = 1
        value.id = value.npc_id
        value.is_server = true
        self.objs_list[value.scene_id] = self.objs_list[value.scene_id] or {}
        table.insert(self.objs_list[value.scene_id], value)
    end
    self.anecdote_npc_list = {}
    local temp
    for index, value in ipairs(self.anecdote_npc) do
        temp = VoMgr:NpcVoBySeq(value.npc_seq)
        temp.show_type = 2
        temp.id = value.npc_id
        temp.is_server = false
        self.objs_list[temp.scene_id] = self.objs_list[temp.scene_id] or {}
        table.insert(self.objs_list[temp.scene_id], temp)
        self.anecdote_npc_list[value.npc_id] = value
    end
    self.shield_list = {}
    for i, v in ipairs(self.sp_shield) do
        self.shield_list[v.type] = self.shield_list[v.type] or {}
        self.shield_list[v.type][v.param] = true
    end
end

function YinYangData:YinYangIsOpened()
    return self.view_data.opened
end

function YinYangData:ObjsList()
    return self.objs_list[SceneData.Instance:SceneId()] or {}
end
function YinYangData:OnGameSceneLoaded()
    --开启检查
    TimeHelper:CancelTimer(self.range_invoke_timer)
    if table.nums(self:ObjsList()) == 0 then
        self.explore_data.near_vo = nil
        self.explore_data.near_distance = 0
        return
    end
    self.range_invoke_timer =
        TimeHelper:AddRunTimer(
        function()
            self:CheckBoxCatch()
        end,
        0.5
    )
end
function YinYangData:CheckBoxCatch()
    -- 战斗检查
    if MainViewData.Instance:IsBattle() then
        return
    end
    -- 虚空检查
    if SceneCtrl.Instance:CurScene() == nil or SceneCtrl.Instance:CurScene().PosToServer == nil then
        return
    end
    if not YinYangData.Instance:FuncIsOpen() then
        self.explore_data.vo = nil
        self.explore_data.near_vo = nil
        self.explore_data.near_distance = 0
        return
    end
    self:CheckBoxExplore()
    self:CheckYinYangEffect()
end
function YinYangData:CheckBoxExplore()
    local list = self:ObjsList()
    local cur_scene = SceneCtrl.Instance:CurScene()
    local main_x, main_y
    local distance
    local pos_clint
    for _, v in pairs(list) do
        if v.show_type == 0 and self.view_data.opened then
            if self.box_flag[v.seq] == 0 then
                main_x, main_y = cur_scene:PosToServer(MainPlayer:GetPos())
                distance = GameMath.GetDistance(main_x, main_y, v.x, v.y, false)
                pos_clint = cur_scene:PosToClint(v.x, v.y)
                if
                    distance < (self.radus * self.radus) and
                        (pos_clint.y - 5 <= MainPlayer:GetPos().y and pos_clint.y + 5 >= MainPlayer:GetPos().y)
                    then
                    self.explore_data.vo = v
                    return
                end
            end
        end
    end
    self.explore_data.vo = nil
end

function YinYangData:CheckYinYangEffect()
    local list = self:ObjsList()
    local cur_scene = SceneCtrl.Instance:CurScene()
    local main_x, main_y, x, y
    local distance, anecdoteConfig
    local v3Pos = Vector3.zero
    for key, v in ipairs(list) do
        if (v.show_type == 0 and self.box_flag[v.seq] == 0)
        or (v.show_type == 1 and self.npc_flag[v.seq] == 0) -- then
        or v.show_type == 2 then
            main_x, main_y = cur_scene:PosToServer(MainPlayer:GetPos())
            x, y = v.x, v.y
            if v.is_server == false then
                v3Pos:Set(v.x, 0, v.y)
                x, y = cur_scene:PosToServer(v3Pos)
                --判断当前NPC是否有奇闻在身上
                anecdoteConfig = AnecdoteData.Instance:GetTaskBubble(v)
                if anecdoteConfig == nil then
                    break
                end
            end
            distance = GameMath.GetDistance(main_x, main_y, x, y, false)
            if distance < (self.remindx_radius * self.remindx_radius) then
                self.explore_data.near_vo = v
                self.explore_data.near_distance = self.remindx_radius
                return
            end
            if distance < (self.remindx_radius * self.remindx_radius) * 2 then
                self.explore_data.near_vo = v
                self.explore_data.near_distance = self.remindx_radius * 2
                return
            end
            if distance < (self.remindx_radius * self.remindx_radius) * 3 then
                self.explore_data.near_vo = v
                self.explore_data.near_distance = self.remindx_radius * 3
                return
            end
            if distance < (self.remindx_radius * self.remindx_radius) * 4 then
                self.explore_data.near_vo = v
                self.explore_data.near_distance = self.remindx_radius * 4
                return
            end
            if distance < (self.remindx_radius * self.remindx_radius) * 5 then
                self.explore_data.near_vo = v
                self.explore_data.near_distance = self.remindx_radius * 5
                return
            end
        end
    end
    self.explore_data.near_vo = nil
    self.explore_data.near_distance = 0
end

function YinYangData:IsCanGet(type, seq)
    --[[ if MainViewData.Instance:IsBattle() then
        return false
    end ]]
    if SceneCtrl.Instance:CurScene() == nil or SceneCtrl.Instance:CurScene().PosToServer == nil then
        return false
    end

    local main_x, main_y, distance,pos_clint
    local cur_scene = SceneCtrl.Instance:CurScene()
    if type == 0 then
        local info = self.all_info.box[seq + 1]
        if info.scene_id ~= SceneData.Instance:SceneId() then
            return false
        else
            if self.box_flag[seq] == 0 then
            main_x, main_y = cur_scene:PosToServer(MainPlayer:GetPos())
            distance = GameMath.GetDistance(main_x, main_y, info.x, info.y, false)
            pos_clint = cur_scene:PosToClint(info.x, info.y)
                if
                    distance < (self.radus * self.radus) and
                        (pos_clint.y - 5 <= MainPlayer:GetPos().y and pos_clint.y + 5 >= MainPlayer:GetPos().y)
                 then
                    return true
                 end
            end
        end
    end
    if type == 1 then
        local info = self.all_info.NPC[seq + 1]
        if info.scene_id ~= SceneData.Instance:SceneId() then
            return false
        else
            if self.npc_flag[seq] == 0 then
            main_x, main_y = cur_scene:PosToServer(MainPlayer:GetPos())
            distance = GameMath.GetDistance(main_x, main_y, info.x, info.y, false)
            pos_clint = cur_scene:PosToClint(info.x, info.y)
                if
                    distance < (self.radus * self.radus) and
                        (pos_clint.y - 5 <= MainPlayer:GetPos().y and pos_clint.y + 5 >= MainPlayer:GetPos().y)
                 then
                    return true
                 end
            end
        end
    end
end
function YinYangData:FuncIsOpen()
    return FunOpen.Instance:GetFunIsOpened(Mod.YinYangEye.Main) and TaskData.Instance:GetTaskIsCompleted(self.unlock_task)
end
function YinYangData:AnecdoteInfo(id)
    return self.anecdote_npc_list[id]
end
function YinYangData:IsCantShow(type, param)
    return self.shield_list[type][param] == true
end

function YinYangData:SetYinYangBtnShow(show)
    if self.view_data.show == show then
        self.view_data:SetDirty("show")
    else
        self.view_data.show = show
    end
end