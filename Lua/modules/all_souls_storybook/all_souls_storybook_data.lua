AllSoulsStorybookData = AllSoulsStorybookData or BaseClass()

AllSoulsRewardType = {
    Sample = 1,
    Mult = 2,
}
AllSoulsRewardTitleType = {
    GetReward = 1,
    ChallengeReward = 2,
}
AllSoulsRewardTitleImgName = {
    [AllSoulsRewardTitleType.GetReward] = "_LocHuoDeJiangLi",
    [AllSoulsRewardTitleType.ChallengeReward] = "_LocTiaoZhanJiangLiZi"
}
function AllSoulsStorybookData:__init()
    if AllSoulsStorybookData.Instance ~= nil then
		Debuger.LogError("[AllSoulsStorybookData] attempt to create singleton twice!")
		return
	end	
	AllSoulsStorybookData.Instance = self
    self.all_config = Config.wanling_huaben_auto
    self.fb_info_cfg = self.all_config.fb_info
    self.task = self.all_config.task
    self.other = self.all_config.other[1]
    self.npc = self.all_config.npc
    self.reward_list = {}
    self:init_config()

    Cfg.Register("WanlingHuabenFbInfoByTeamTarget",Cfg.RealTimeSearch(self.fb_info_cfg,"target_id"))

    self.fb_info = SmartData.New({fetch = {}, pass = 0, boss_fetch = {}})
    self.fb_scene_info = SmartData.New({index = 0, seq = 0, reward_flush = false})--seq完成后会发最大数+1
    self.obtain_reward_list = {}
end
function AllSoulsStorybookData:init_config()
    self.fb_list = {}
    self.door_pos_list = {}
    local temp = {}
    local list = {}
    for _, value in ipairs(self.fb_info_cfg) do
        list = {}
        temp = string.split(value.reward_view,":")
        for _, item_id in ipairs(temp) do
            table.insert(list, Item.Init(tonumber(item_id)))
        end
        value.reward_list = list
        self.door_pos_list[value.fb_index] = {x = value.door_x, y = value.door_y}
        table.insert(self.fb_list, value)
    end
    self.task_list = {}
    for _, value in ipairs(self.task) do
        self.task_list[value.fb_index] = self.task_list[value.fb_index] or {}
        self.task_list[value.fb_index][value.task_num] = value
    end
    --客户端单独加一步末尾执行的任务信息
    for k, v in pairs(self.task_list) do
        local t = {}
        t.fb_index = v[1].fb_index
        t.task_num = #v + 1
        t.task_type = 3     --与配置中的类型1（npc）、2（战斗）区分
        t.npc_id = ""
        t.npc_action = ""
        t.task_name= Language.AllSoulsStorybook.BookEnd
        t.task_dec = Language.AllSoulsStorybook.ClickToLeave
        t.navigate = v[1].navigate
        table.insert(v, t)
    end
    self.npc_list = {}
    for index, value in ipairs(self.npc) do
        self.npc_list[value.fb_id] = self.npc_list[value.fb_id] or {}
        self.npc_list[value.fb_id][value.appear_task] = self.npc_list[value.fb_id][value.appear_task] or {}
        table.insert(self.npc_list[value.fb_id][value.appear_task], value)
    end
end
function AllSoulsStorybookData:__delete()
    
end

function AllSoulsStorybookData:SetCurIsFinish(protocol)
    self.is_finish = protocol.is_finish
end

function AllSoulsStorybookData:GetIsFinish()
    return self.is_finish == 1
end

function AllSoulsStorybookData:FBList()
   return self.fb_list
end
function AllSoulsStorybookData:FBInfo()
    return self.fb_info
end
function AllSoulsStorybookData:Fetch()
    return self.fb_info.fetch
end
function AllSoulsStorybookData:SceneInfo()
    return self.fb_scene_info
end
function AllSoulsStorybookData:IsFetch(index)
    return (self.fb_info.fetch[32-index] or 0) == 0
end
function AllSoulsStorybookData:RewardTime()
    return self.other.reward_times
end
function AllSoulsStorybookData:TaskInfo()
    return self.fb_list[self.fb_scene_info.index + 1]
end
function AllSoulsStorybookData:TaskList()
    return self.task_list[self.fb_scene_info.index]
end
function AllSoulsStorybookData:CurTask()
    return self.task_list[self.fb_scene_info.index][self.fb_scene_info.seq]
end
function AllSoulsStorybookData:doTask(info)
    if info == nil then return end
    if not TeamData.Instance:IsLeader() then
        return
    end
    if info.task_type == 1 then
        self:doFight(info)
    elseif info.task_type == 2 then
        self:doTalk(info)
    else
        self:goToProtocol()
    end
end
function AllSoulsStorybookData:doTalk(info)
    SceneCtrl.Instance:Logic():AutoToNpc(
        info.npc_id,
        function()
            NpcDialogCtrl.Instance:NpcDialogView(
                {
                    talk_id = info.npc_action,
                    beh_btn_list = {}
                },
                function()
                    AllSoulsStorybookCtrl.TaskFinish()
                end
            )
        end,
        false,
        false
    )
end
function AllSoulsStorybookData:doFight(info)
    SceneCtrl.Instance:Logic():AutoToNpc(
        info.npc_id,
        function()
            -- SceneCtrl.Instance:RequestGroupIdBattleNoNS(info.npc_action)
            AllSoulsStorybookCtrl.TaskFight()
        end,
        false,
        false
    )
end
function AllSoulsStorybookData:NpcList(task_seq)
    task_seq = task_seq or self.fb_scene_info.seq
    return self.npc_list[self.fb_scene_info.index][task_seq]
end
function AllSoulsStorybookData:CheckNpcs()
    local task_seq = self.fb_scene_info.seq
    local list = self.npc_list[self.fb_scene_info.index]
    for seq, npcs in pairs(list) do
        for _, npc in pairs(npcs) do
            if npc.appear_task == task_seq then
                SceneCtrl.Instance:AddNpc(npc.npc_id)
            elseif npc.disappear_task ~= -1 and npc.disappear_task <= task_seq then
                SceneCtrl.Instance:DeleteNpc(npc.npc_id)
            elseif npc.disappear_task == -1 and task_seq == #self:TaskList() then
                SceneCtrl.Instance:DeleteNpc(npc.npc_id)
            end
        end
    end
end
function AllSoulsStorybookData:SetRewardList(list, reward_type, title_type, hide_fly_text, huobi_desc)
    self.reward_type = reward_type or AllSoulsRewardType.Sample
    self.reward_title_type = title_type or self.reward_type
    self.hide_fly_text = hide_fly_text
    self.reward_list = list
    self.huobi_desc = huobi_desc
end
function AllSoulsStorybookData:RewardList()
    return self.reward_list
end

function AllSoulsStorybookData:goToProtocol()
    local door_pos = self.door_pos_list[self.fb_scene_info.index]
    local client_door_pos = SceneCtrl.Instance:CurScene():PosToClint(door_pos.x, door_pos.y)
    if TeamData.Instance:IsLeader() then
        if MainPlayer then
            MainPlayer:MoveTo(client_door_pos)
        end
    else
        PublicPopupCtrl.Instance:Center(Language.Team.TipsMove)
    end
end

function AllSoulsStorybookData:GetAutoSelectIndex()
    local role_level = RoleData.Instance:GetRoleLevel()
    for i,v in ipairs(self.fb_list) do
        if role_level < v.open_level then
            return i - 1 <= 0 and 1 or i - 1
        elseif i == #self.fb_list then
            return i
        end
    end
end
function AllSoulsStorybookData:IsFetchBoss(index)
    return (self.fb_info.boss_fetch[index] or 0) == 1
end
function AllSoulsStorybookData:CacheReward(list)
    for i, v in ipairs(list) do
        table.insert(self.obtain_reward_list, v)
    end
    self.fb_scene_info.reward_flush = not self.fb_scene_info.reward_flush
end
function AllSoulsStorybookData:GetCacheRewards()
    return self.obtain_reward_list
end
function AllSoulsStorybookData:ClearCacheRewards()
    self.obtain_reward_list = {}
    self.fb_scene_info.reward_flush = not self.fb_scene_info.reward_flush
end