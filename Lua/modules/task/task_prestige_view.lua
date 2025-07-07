TaskPrestigeView = TaskPrestigeView or DeclareView("TaskPrestigeView", TaskConfig.ResPath .. "task_prestige", Mod.PrestigeTask.Main)
VIEW_DECLARE_LEVEL(TaskPrestigeView, ViewLevel.L1)

function TaskPrestigeView:OnClickClose()
    ViewMgr:CloseView(TaskPrestigeView)
end

TaskPrestigePanel = TaskPrestigePanel or DeclareMono("TaskPrestigePanel", UIWFlushPanel)
function TaskPrestigePanel:TaskPrestigePanel()
    self.data = PrestigeData.Instance
    self.data_cares = {
        {data = self.data.village_prestige_info, func = self.FlushDataFlush},
        {data = self.data.prestige_task_table, func = self.FlushShow, init_call = false},
        {data = self.data.prestige_task_data, func = self.FlushAllInfo}
    }
    self.lang = Language.TaskPrestige
end

function TaskPrestigePanel:FlushAllInfo()
    self:FlushDataFlush()
    self:FlushShow()
end

function TaskPrestigePanel:FlushDataFlush()
    local show_list = self.data:GetPrestigeTaskVillageList()
    self.List:SetDataList(show_list)
    local village_id = self.data:GetPrestigeTaskVillageId()
    if self.data:GetPrestigeTaskVillageCfg(village_id) == nil then
        self.data:SetPrestigeTaskVillageId(show_list[1].village_id)
    end
end

function TaskPrestigePanel:FlushShow()
    local village_id = self.data:GetPrestigeTaskVillageId()
    local village_cfg = self.data:GetPrestigeTaskVillageCfg(village_id)
    if village_cfg == nil then
        return
    end
    UH.SetText(self.VillageName, village_cfg.name)
    UH.SetText(self.UnlockText, self.data:GetPrestigeTaskNextPhaseDesc(village_id))
    UH.SetText(self.DescText, Format(self.lang.TaskDesc, village_cfg.name))

    local item_list = DataHelper.SplitData(village_cfg.reward_id, ":")
    local show_list = {}
    for i = 1, #item_list do
        table.insert(show_list, Item.New(item_list[i]))
    end
    self.ItemList:SetData(show_list)
end

function TaskPrestigePanel:OnClickTask()
    local village_id = self.data:GetPrestigeTaskVillageId()
    local village_cfg = self.data:GetPrestigeTaskVillageCfg(village_id)
    if village_cfg == nil then
        return
    end
    if RoleLevel() < village_cfg.unlock_level then
        return
    end
    if self.data.prestige_task_data.village_id ~= 0 then
        return
    end
    local info = {
        content = Format(Language.TaskPrestige.SelectSure, village_cfg.name),
        confirm = {
            func = function()
                TaskCtrl.Instance:SendPrestigeTaskSelectVillage(village_id)
                PublicPopupCtrl.Instance:CloseDialogTips()
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

--===========================TaskPrestigeItem===========================
TaskPrestigeItem = TaskPrestigeItem or DeclareMono("TaskPrestigeItem", UIWidgetBaseItem)
function TaskPrestigeItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    UH.SetText(self.Name, data.name)
    UH.SpriteName(self.Icon, data.pic)

    local info, config = PrestigeData.Instance:GetVillagePrestigeNextData(data.village_id)
    local cur_num, need_num = info.prestige, config.need_prestige
    self.Progress:SetProgress(cur_num / need_num)
    self.Progress:SetText(Format(Language.TaskPrestige.ProgressFormat, cur_num, need_num))

    local _, cur_config = PrestigeData.Instance:GetVillagePrestigeData(data.village_id)
    UH.SetText(self.Relation, cur_config.name)

    local relation = info.level > 6 and 6 or info.level
    self.Outline.effectColor = PRESTIGE_LEVEL_COLOR[relation]
    if relation > 0 then
        self.Gradient.topColor = COLORS.Yellow7
        self.Gradient.buttomColor = COLORS.Yellow1
    else
        self.Gradient.topColor = COLORS.Yellow13
        self.Gradient.buttomColor = COLORS.Yellow19
    end

    local is_actived = RoleLevel() >= self.data.unlock_level
    self.ItemInter.Interactable = is_actived
    self.LockedNode:SetActive(not is_actived)
    self.ActivedNode:SetActive(true)
    if is_actived then
    else
        UH.SetText(self.UnLockDesc, Format(Language.TaskPrestige.UnLockLv, self.data.unlock_level))
    end
end

function TaskPrestigeItem:OnSelect(selected)
    UIWidgetBaseItem.OnSelect(self, selected)
    if selected then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
        PrestigeData.Instance:SetPrestigeTaskVillageId(self.data.village_id)
    end
end

function TaskPrestigeItem:Click()
    self.ToggleClick:Trigger()
end
