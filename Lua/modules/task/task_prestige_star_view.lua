TaskPrestigeStarView = TaskPrestigeStarView or DeclareView("TaskPrestigeStarView", TaskConfig.ResPath .. "task_prestige_star")
VIEW_DECLARE_LEVEL(TaskPrestigeStarView, ViewLevel.L1)
function TaskPrestigeStarView:OnClickClose()
    ViewMgr:CloseView(TaskPrestigeStarView)
end

TaskPrestigeStarPanel = TaskPrestigeStarPanel or DeclareMono("TaskPrestigeStarPanel", UIWFlushPanel)
function TaskPrestigeStarPanel:TaskPrestigeStarPanel()
    self.data = PrestigeData.Instance
    self.data_cares = {
        {data = self.data.prestige_task_data, func = self.FlushAllInfo},
        {data = BagData.Instance:GetColumnGrids(ItemColumnType.Item), func = self.FlushItemNum, init_call = true}
    }

    self.stuff_id = self.data:GetPrestigeTaskFlushItemId()
    self.stuff_num = 1
    self.is_click_flush = false
end

function TaskPrestigeStarPanel:Awake()
    UIWFlushPanel.Awake(self)
    local item = Item.Create({item_id = self.stuff_id, num = 1})
    UH.SetText(self.ItemTips, Format(Language.TaskPrestige.UseItemFlush, item:QuaName(true)))
    UH.SetIcon(self.ItemIcon, item:IconId(), ICON_TYPE.ITEM)
end

function TaskPrestigeStarPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

function TaskPrestigeStarPanel:FlushAllInfo()
    local task_list = PrestigeData.Instance:GetPrestigeTaskList()
    for i, v in pairs(task_list) do
        v.is_flush = self.is_click_flush or false
    end
    self.TaskList:SetData(task_list)
    self.is_click_flush = false
end

function TaskPrestigeStarPanel:FlushItemNum()
    local has_num = Item.GetNum(self.stuff_id)
    UH.SetText(self.BtnFlushText, Format(Language.TaskPrestige.ItemNum, has_num, self.stuff_num))
end

function TaskPrestigeStarPanel:OnClickFlush()
    if Item.GetNum(self.stuff_id) >= self.stuff_num then
        self.is_click_flush = true
        TaskCtrl.Instance:SendPrestigeTaskReq(PRESTIGE_TASK_REQ_TYPE.TYPE_FLUSH_STAR, 0)
        PublicPopupCtrl.Instance:Center(Language.TaskPrestige.UseItemFlushSuccess)
    else
        MainOtherCtrl.Instance:GetWayViewById(self.stuff_id)
    end
end

function TaskPrestigeStarPanel:OnClickBegin()
    TaskCtrl.Instance:SendPrestigeTaskReq(PRESTIGE_TASK_REQ_TYPE.TYPE_BEGIN, 0)
    ViewMgr:CloseView(TaskPrestigeStarView)
    local village_cfg = PrestigeData.Instance:GetPrestigeTaskCurVillageCfg()
    local npc_config = VoMgr:NpcVoBySeq(village_cfg.task_npc_seq)
    if npc_config == nil then
        return
    end
    local function func_call()
        SceneLogic.Instance:AutoToNpc(village_cfg.task_npc_seq)
    end
    SceneLogic.Instance:AutoToPos(npc_config.scene_id, nil, func_call, nil, true)
end

TaskPrestigeStarItem = TaskPrestigeStarItem or DeclareMono("TaskPrestigeStarItem", UIWidgetBaseItem)
function TaskPrestigeStarItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.NameText, data.task_name)

    local star = PrestigeData.Instance:GetPrestigeTaskStar(data.task_index)
    UH.SpriteName(self.BG, "StarCard" .. star)
    UH.SpriteName(self.StarBG, "StarCardBG" .. star)
    for i = 1, 5 do
        self.StarList[i]:SetActive(i <= star)
    end
    local star_cfg = PrestigeData.Instance:GetPrestigeTaskStarCfg(star)
    self.ItemList:SetData(DataHelper.FormatItemList(star_cfg.item_list))

    if data.is_flush then
        self:PlayEffect()
    end
end

function TaskPrestigeStarItem:PlayEffect()
    if self.cur_effect ~= nil then
        self.EffectTool:Stop(self.cur_effect)
    end
    self.cur_effect = self.EffectTool:Play(7161001)
end

TaskPrestigeStarCellItem = TaskPrestigeStarCellItem or DeclareMono("TaskPrestigeStarCellItem", UIWidgetBaseItem)
function TaskPrestigeStarCellItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.ItemCell:SetData(data)
end

--小界面
TaskPrestigeStarSamllView = TaskPrestigeStarSamllView or DeclareView("TaskPrestigeStarSamllView", TaskConfig.ResPath .. "task_prestige_star_small")
VIEW_DECLARE_MASK(TaskPrestigeStarSamllView, ViewMask.BlockClose)

function TaskPrestigeStarSamllView:LoadCallback(param_t)
    local task_data = param_t.task_data
    task_data.is_flush = true
    self.TaskItem:SetData(task_data)
end

function TaskPrestigeStarSamllView:CloseCallback()
    TimeHelper:CancelTimer(self.prestige_task_timer)
    self.prestige_task_timer = nil
end

function TaskPrestigeStarSamllView:OpenCallback()
    self.prestige_task_timer = Invoke(BindTool.Bind1(self.OnTimeOver, self), 3)
end

function TaskPrestigeStarSamllView:OnTimeOver()
    ViewMgr:CloseView(TaskPrestigeStarSamllView)
end
