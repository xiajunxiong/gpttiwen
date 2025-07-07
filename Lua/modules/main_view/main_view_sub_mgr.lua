MainViewSubMgr = MainViewSubMgr or DeclareMono("MainViewSubMgr", UIWFlushPanel)
--主界面替身组件管理
--替身是用于预设加载避免预设过大
--现增加主界面替身管理，管理替身的加载（active true）时机
--避免在一开始打开主界面时出现过多的无意义加载
--检查函数的写法 name+key
--检查函数返回true or false
function MainViewSubMgr:MainViewSubMgr()
    self.key_scene = "_scene"
    self.key_level = "_level"
    self.key_task = "_task"
    self.key_act = "_act"
    self.key_team = "_team"
    self.obj_list = {}
    self.name_list = {}
    self.check_list = {}
    --注意care的方式
    local check_team = BindTool.Bind(self.CheckFunction, self, self.key_team)
    local check_act = BindTool.Bind(self.CheckFunction, self, self.key_act)
    self.data_cares = {
        {data = SceneMgr.data, func = BindTool.Bind(self.CheckFunction, self, self.key_scene)},
        {data = RoleData.Instance:GetBaseData(), func = BindTool.Bind(self.CheckFunction, self, self.key_level), keys = {"level"}},
        {data = ActivityData.Instance:GetActivityList(), func = check_act},--, init_call = false},
        {data = TreasureTaskData.Instance.info_sm, func = BindTool.Bind(self.CheckFunction, self, self.key_task)},--, init_call = false},
        {data = TeamData.Instance.team_invite_flush, func = check_team, init_call = false},
        {data = FairyWhereData.Instance.info_data, func = check_act, keys = {"is_show"}, init_call = false},
        {data = TaskCtrl.Instance:Data().menu_adventure, func = check_team, init_call = false},
		{data = InviteFightData.Instance:Base(), func = check_team, init_call = true},
		{data = LiveData.Instance.prop_use_all_info, func = check_team, init_call = true},
    }
end
function MainViewSubMgr:Awake()
    self.obj_list = {}
    self.name_list = {}
    self.check_list = {}
    -- LogDG("self.SubsList:Length() ", self.SubsList:Length())
    for i = 1, self.SubsList:Length() do
        self.SubsList[i]:SetActive(false)
        self.obj_list[self.SubsList[i].name] = self.SubsList[i]
        self.name_list[i] = self.SubsList[i].name
        self.check_list[self.SubsList[i].name] = i
    end
    TaskData.Instance:ListenTaskCompleteByIds(BindTool.Bind(self.CheckFunction, self, self.key_task), {YinYangData.Instance.unlock_task})
    UIWFlushPanel.Awake(self)
end
function MainViewSubMgr:OnEnable()
    UIWFlushPanel.OnEnable(self)
--    LogDG("MainViewSubMgr:OnEnable()")
    -- self:CheckFunction("_scene")
end
function MainViewSubMgr:CheckFunction(key)
    -- LogDG("CheckFunction(", key)
    if table.nums(self.check_list) == 0 then
        return
    end
    local name = ""
    local active = false
    for i = 1, #self.name_list do
        if self.check_list[self.name_list[i]] then
            name = self.name_list[i]..key
            if self[name] then
                active = self[name]()
                self.SubsList[i]:SetActive(active)
                if active == true then
                    self.check_list[self.name_list[i]] = nil
                end
            end
        end
    end
end

function MainViewSubMgr:map_explore_scene()
    return MapExploreData.Instance:CanExplore(SceneData.Instance:SceneId())
end
function MainViewSubMgr:fish_explore_scene()
    return FishData.Instance:InFishMap()
end
function MainViewSubMgr:yinyang_explore_level()
    return YinYangData.Instance:FuncIsOpen()
end
function MainViewSubMgr:yinyang_explore_task()
    return YinYangData.Instance:FuncIsOpen()
end
function MainViewSubMgr:npc_dialog_sync_scene()
    return TeamData.Instance:InTeam() and SceneData.Instance:GetMode() == SCENE_TYPE.HARMONY_HEAVEN
end
function MainViewSubMgr:act_tips_act()
    return RoleData.Instance:GetRoleLevel() > 25
end
function MainViewSubMgr:treasure_task_explore_task()
    if TreasureTaskData.Instance.info_sm and TreasureTaskData.Instance.info_sm.task_id then
        return TreasureTaskData.Instance.info_sm.task_id > 0
    end
    return false
end
function MainViewSubMgr:wedding_fb_scene()
    return  SceneData.Instance:SceneId() == MarryData.Instance:ConfigOtherSceneId()
end
function MainViewSubMgr:marry_party_scene()
    return SceneData.Instance:SceneId() == MarryData.Instance:ConfigOtherSceneId()
end
function MainViewSubMgr:team_invite_sub_team()
    if FunOpen.GetConfog().task_id ~= nil and FunOpen.GetConfog().task_id ~= 0 and not TaskData.Instance:GetTaskIsCompleted(FunOpen.GetConfog().task_id) then
        return false
    end
    local info = TeamData.Instance:GetTeamInviteInfo()
    if info == nil then
        return false
    end
    return true
end
function MainViewSubMgr:fairy_where_sub_act()
    return PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.FairyWhere)
end
function MainViewSubMgr:down_right_act()
    local info = ServerActivityData.Instance:GetGiftInfoList()
    return not IS_AUDIT_VERSION and info ~= nil
end
function MainViewSubMgr:adventure_sub_team()
    local task = TaskCtrl.Instance:Data().menu_adventure:Val()
    local cur_state = InviteFightData.Instance:CurState()
    return nil ~= next(task) or LiveData.Instance:IsProping() or (cur_state ~= InviteFightData.State.Start and 
    cur_state ~= InviteFightData.State.End)
end

function MainViewSubMgr:courage_challenge_sub_level()
    return FunOpen.Instance:IsFunOpen(Mod.Experience.GuMo)
end
function MainViewSubMgr:bicycle_tip_sub_level()
    return FunOpen.Instance:IsFunOpen(Mod.Bicycle.Main)
end
function MainViewSubMgr:lianyao_tip_sub_level()
    return FunOpen.Instance:IsFunOpen(Mod.PetRefining.Main)
end