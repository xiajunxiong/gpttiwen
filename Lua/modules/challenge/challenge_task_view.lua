ChallengeTaskView = ChallengeTaskView or DeclareView("ChallengeTaskView", "challenge/challenge_task", Mod.Experience.Task)

function ChallengeTaskView:LoadCallback(param)
    self.Board:SetData(ChallengeTaskView)
    self.Currency:DefaultSet()
    self.Panel:OnStart(param)
end

function ChallengeTaskView:OnHelpClick()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[361].desc)
end

-- panel
ChallengeTaskPanel = ChallengeTaskPanel or DeclareMono("ChallengeTaskPanel", UIWFlushPanel)

function ChallengeTaskPanel:ChallengeTaskPanel()
    self.data = ChallengeCtrl.Instance.task_data
    self.language = Language.Challenge.Task
    self.data.cur_sel_task:Set()
    self.data.cur_team_sel_task:Set()
    self.data_cares = {{data = self.data.cur_sel_task, func = self.FlushDetails, init_call = false}, 
    {data = self.data.cur_team_sel_task, func = self.FlushDetails, init_call = false}}
end

function ChallengeTaskPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.ItemChallenge.TaskList:SetData(self.data:GetChallengeTaskList())
    self.ItemTeam.TaskList:SetData(self.data.team_task_list)
    self.InterZdtz.Interactable = self.data:CheckTeamOpen()
end

function ChallengeTaskPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
end

function ChallengeTaskPanel:OnStart(param)
    local type
    if param and param.open_param == 3 then
        type = 2
        self.BtnZdtz.isOn = true
    else
        type = 1
        self.BtnTzrw.isOn = true
    end
    self:OnChangeType(type)
end

-- 切换挑战类型
function ChallengeTaskPanel:OnChangeType(type)
    self.data.cur_challenge_type.type = type
    if type == 1 then
        self.Board:SetBg2(false)
        self.ItemChallenge.TaskList:ClickItemData(self.data:GetSelChallengeData())
    else
        if self.data:CheckTeamOpen() then
            self.Board:SetBg2(true)
            local index =  self.data:GetSelTeamData()
            self.ItemTeam.TaskList:ClickItemData(self.data.team_task_list[index])
            self.timer = TimeHelper:AddDelayFrameTimer(function()
                   self.ItemTeam.TaskList:JumpVerticalIndex(index, 113, 0)
            end,5)
        else
            PublicPopupCtrl.Instance:Center(Language.Challenge.huanshiTip)
            self.BtnTzrw.isOn = true
        end
    end
end


--选中项改变-刷新任务详情
function ChallengeTaskPanel:FlushDetails()
    if self.data.cur_challenge_type.type == 1 then
        self.ItemChallenge:SetData(self.data.cur_sel_task)
        self.ItemChallenge:FulshChallenge()
    else
        self.ItemTeam:SetData(self.data.cur_team_sel_task)
        self.ItemTeam:FulshTeamChallenge()
    end
end

function ChallengeTaskPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.timer ~= nil then
    TimeHelper:CancelTimer(self.timer)
    self.timer = nil
end

end


-----任务详情item
ChallengeTaskViewDetail = ChallengeTaskViewDetail or DeclareMono("ChallengeTaskViewDetail", UIWidgetBaseItem)
function ChallengeTaskViewDetail:ChallengeTaskViewDetail()
    self.language = Language.Challenge.Task
end

function ChallengeTaskViewDetail:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushDetails(data)
end

function ChallengeTaskViewDetail:OnClickPlayVideo()
    if self.video ~= nil and BattleVideo.Instance:GetSimpleInfo(self.video.monster_group_id, 0) ~=0  then
        BattleVideo.Instance:PlayVideoCfg(self.video)
        BattleCtrl.BattleReportDetailInfo(self.video.monster_group_id)
    else
        PublicPopupCtrl.Instance:Center(Language.DreamNotes.NotHasPlayVideo)
    end
end


function ChallengeTaskViewDetail:FlushDetails(cur_sel_task)
    self.Name.text = cur_sel_task.name
    self.OpenLevel.text = cur_sel_task.open_level_desc
    self.Desc.text = cur_sel_task.desc 
    local pivot = self.Content.pivot
    if #self.data.rewards > 3 then
        pivot = Vector2.New(0, 0.5)
    else
        pivot = Vector2.New(0.5, 0.5)
    end
    self.Content.pivot = pivot
    self.RewardList:SetData(cur_sel_task.rewards)
    -- self.RecoSp:SetObjActive(3 == cur_sel_task.recommend_num or 5 == cur_sel_task.recommend_num)
    -- self.RecoSp.SpriteName = string.format("_TuiJian%sRen", cur_sel_task.recommend_num)
    self:FlushModelShow(cur_sel_task)
end


-- 挑战任务显示
function ChallengeTaskViewDetail:FulshChallenge()
    self.PreTask.text = self.data.pre_task
    self.RecoVal.text = string.format("%s/%s",
        RichTextHelper.ColorStr(RoleData.Instance:GetSumScore(), RoleData.Instance:GetSumScore() < self.data.recommend_score and COLORSTR.Red5 or COLORSTR.Green9), self.data.recommend_score)
    self:FlushBtnGo(self.data, false)
    local index = self.data.seq
    self.video = BattleVideo.Instance:VideoCfg(BATTLE_MODE.BATTLE_MODE_NORMAL_MONSTER, index)
    if  self.video then
        if self.handle_care then
            BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        end
        self.handle_care = BattleVideo.Instance:SimpleInfo():Care(BindTool.Bind(self.FlushGray, self, self.video.monster_group_id))
        self:FlushGray(self.video.monster_group_id)
        BattleCtrl.BattleReportSimpleInfo(self.video.monster_group_id, 0)
    end
end

-- 组队挑战显示
function ChallengeTaskViewDetail:FulshTeamChallenge()
    self.ScrollRect.verticalNormalizedPosition = 1
    self:FlushBtnGo(self.data, true)
    UH.SetText(self.TxtTip, self.data.recommend_tip)
    self.ObjYlq:SetActive(TaskData.Instance:GetTaskIsCompleted(self.data.last_task))
    local data = ChallengeTaskData.Instance
    local task_info = data.config_task_list_team[self.data.last_task]
    local npc_seq=tonumber(string.split(task_info.tasks_dialog_beh, "|")[5])
    local npc_info=Cfg.NpcBySeq(npc_seq)
    local npc_cfg=Config.npc_auto.npc_list[npc_info.id]
    local beh_id =npc_cfg.param3
    local beh_cfg = Config.npc_behavior_auto.npc_behavior_cfg[tonumber(beh_id)]
    local monster_group_id = nil
    if beh_cfg then
        monster_group_id = beh_cfg.param1
        self.video = BattleVideo.Instance:GetVideoConfig(BATTLE_MODE.BATTLE_MODE_TEAM_CHALLENGE_TASK, monster_group_id)
    end
    if self.video then
        if self.handle_care then
            BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        end
        self.handle_care = BattleVideo.Instance:SimpleInfo():Care(BindTool.Bind(self.FlushGray, self, monster_group_id))
        BattleCtrl.BattleReportSimpleInfo(self.video.monster_group_id, 0)
    end
    self:FlushGray(monster_group_id)
end


function ChallengeTaskViewDetail:FlushGray(monster_group_id)
    local can_play = BattleVideo.Instance:GetSimpleInfo(monster_group_id, 0)
    self.Gray.enabled = can_play == 0
end

-- 按钮内容显示
function ChallengeTaskViewDetail:FlushBtnGo(cur_sel_task, isTeam)
    local role_level = RoleData.Instance:GetRoleLevel()
    local btn_go = self.language.BtnGo
    local inter = true
    local btn_text = btn_go.go
    if role_level < cur_sel_task.open_level or not TaskData.Instance:GetTaskIsCompleted(cur_sel_task.pretaskid) then
        inter = false
        btn_text = btn_go.unlock
    end
    if TaskData.Instance:GetTaskIsCompleted(cur_sel_task.last_task) and not isTeam then
        inter = false
        btn_text = btn_go.finished
    end
    self.BtnGoInter.Interactable = inter
    self.BtnGo.text = btn_text
end

-- 模型展示
function ChallengeTaskViewDetail:FlushModelShow(cur_sel_task)
    local res_path = DrawerHelper.GetNpcPath(ResHelper.MonsterResId(cur_sel_task.monster_id))
    local uo = self.ModelShow:UiObj()
    if uo == nil then
        uo = UIObjDrawer.CreateByPath(res_path)
        self.ModelShow:SetShowData({ui_obj = uo, view = ChallengeTaskView, load_callback = BindTool.Bind(self.ModelCallback, self)})
    else
        -- self.ModelImg.enabled = false
        uo:SetData(res_path)
    end
end
function ChallengeTaskViewDetail:ModelCallback()
    -- self.ModelImg.enabled = true
end

-- 点击按钮
function ChallengeTaskViewDetail:OnClickGo()
    if FightConfig.IS_IN_FIGHTFB[SceneData.Instance:GetMode()] then
        FightCtrl.IsRequestFb(function()
            self:OnClickGo()
        end)
        return
    end
    ViewMgr:CloseView(ChallengeTaskView)
    ChallengeCtrl.Instance.task_data:GoChallengeTask(self.data.last_task)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.LoginStartClick)
end

function ChallengeTaskViewDetail:OnClickGoTeam()
    if FightConfig.IS_IN_FIGHTFB[SceneData.Instance:GetMode()] then
        FightCtrl.IsRequestFb(function()
            self:OnClickGoTeam()
        end)
        return
    end
    ViewMgr:CloseView(ChallengeTaskView)
    ChallengeCtrl.Instance.task_data:GoTeamTask(ChallengeCtrl.Instance.task_data.cur_team_sel_task.last_task)-- TaskConfig.TaskType.team_challenges)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.LoginStartClick)
end

function ChallengeTaskViewDetail:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.handle_care then
        BattleVideo.Instance:SimpleInfo():Uncare(self.handle_care)
        self.handle_care = nil
    end
end

-----item
ChallengeTaskViewItem = DeclareMono("ChallengeTaskViewItem", UIWidgetBaseItem)

function ChallengeTaskViewItem:ChallengeTaskViewItem()
    self.task_data = ChallengeCtrl.Instance.task_data
end

function ChallengeTaskViewItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    self.Name.text = data.name
    UH.SetIcon(self.IconSp, data.icon_id_m, ICON_TYPE.ITEM)
    local is_lock = RoleData.Instance:GetRoleLevel() < data.open_level or not TaskData.Instance:GetTaskIsCompleted(data.pretaskid)
    self.LockObj:SetActive(is_lock)
    self.ItemInter.Interactable = not is_lock
    self.ComObj:SetActive(TaskData.Instance:GetTaskIsCompleted(data.last_task) and not is_lock)
end

function ChallengeTaskViewItem:Click()
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

function ChallengeTaskViewItem:OnClickItem()
    if self.task_data.cur_challenge_type.type == 1 then
        local data = ChallengeCtrl.Instance.task_data.task_list[self.data.seq]
        self.task_data.cur_sel_task:Set(data)
    else
        local data = ChallengeCtrl.Instance.task_data.team_task_list[self.data.seq]
        self.task_data.cur_team_sel_task:Set(data)
    end
end

function ChallengeTaskViewItem:OnDestroy()

end

