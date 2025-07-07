
NpcDialogViewDetail1 = NpcDialogViewDetail1 or DeclareMono("NpcDialogViewDetail1", UIWFlushPanel)

function NpcDialogViewDetail1:NpcDialogViewDetail1()
    self.data = NpcDialogCtrl.Instance:Data()
    self.language = Language.NpcDialog.Detail1

    self.data_cares = {
		{data = self.data.cur_sel_btn, func = self.FlushDetail},
	}
end

function NpcDialogViewDetail1:OnEnable()
    UIWFlushPanel.OnEnable(self)
    TimeHelper:CancelTimer(self.timer_dft)
    self.timer_dft = TimeHelper:AddDelayFrameTimer(function ()
        local btn_index = self.data.cur_sel_btn.btn_index or 1
        self.ContentPos.anchoredPosition = Vector2.New(0, btn_index * 94 - 94)
    end)

    TimeHelper:CancelTimer(self.handle_delay)
    -- self.handle_delay = TimeHelper:AddDelayTimer(function ()
    --     self:AutoConfirm()
    -- end, TeamConfig.AutoTime)
end

function NpcDialogViewDetail1:OnDisable()
    UIWFlushPanel.OnDisable(self)
    TimeHelper:CancelTimer(self.handle_delay)
end

function NpcDialogViewDetail1:AutoConfirm()
    local dragon = TeamCtrl.Instance:Dragon()
    if dragon and dragon:Step() > 4 then
        local task = TaskData.Instance:GetTaskInfoByTaskId(CUSTOM_OBJID.RESEARCH_TASK)
        if task and task.is_accept_task then
            task:AutoTask()
        else
            self:OnClickConfirm()
        end
    end
end

function NpcDialogViewDetail1:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.timer_dft)
    TimeHelper:CancelTimer(self.handle_delay)
end

function NpcDialogViewDetail1:FlushDetail()
    local detail_info = self.data.cur_sel_btn.detail_info
    if detail_info then
        for i = 1, self.Tips:Length() do
            self.Tips[i]:SetData(detail_info.tips[i])
        end
        local prof_type = RoleData.Instance:ProfType()
	    prof_type = prof_type > 0 and prof_type or 1
        for _, item in pairs(detail_info.rewards) do
            if item.old_id then
                local co = Cfg.RewardJobBagByIdType(item.old_id, prof_type)
                item.item_id = co and co.new_id or item.item_id
            end
        end
        if #detail_info.rewards > 3 then
            self.Content.pivot = Vector2.New(0,0.5)
        end
        self.RewardList:SetData(detail_info.rewards)
        self.BtnConfirm.text = detail_info.confirm_show or self.language.confirm_default
        self.BtnCustom.text = detail_info.custom_show or self.language.custom_default
        if DetailInfo1Custom.quick_team == detail_info.custom_type then
            local btn_yellow = ItemInfoConfig.ButtonConfig[2]
            self.BtnCustom.color = btn_yellow.LbColor
            self.BtnCustomSp.SpriteName = btn_yellow.SpName
        end
    end
end


function NpcDialogViewDetail1:OnClickConfirm()
    TimeHelper:CancelTimer(self.handle_delay)
    if self.data.cur_sel_btn.Exe then
        self.data.cur_sel_btn:Exe()
        self.data.cur_sel_btn:Set{type_old = self.data.cur_sel_btn.detail_info.type}
    end
end

function NpcDialogViewDetail1:OnClickCustom()
    local ct = self.data.cur_sel_btn.detail_info.custom_type
    if DetailInfo1Custom.back == ct or DetailInfo1Custom.quick_team == ct then
        local detail_info = self.data.cur_sel_btn.detail_info
        if detail_info then
            Call(detail_info.custom_func)
            return
        end
    end
    self.data.cur_sel_btn:Set{type_old = self.data.cur_sel_btn.detail_info.type}
end

NpcDialogViewDetail1TipsItem = DeclareMono("NpcDialogViewDetail1TipsItem", UIWidgetBaseItem)

function NpcDialogViewDetail1TipsItem:SetData(data)
    self.ItemObj:SetActive(nil ~= data)
    if data then
        self.Tips.text = data
    end
end

NpcDialogViewDetailMonoItem = DeclareMono("NpcDialogViewDetailMonoItem", UIWidgetBaseItem)

function NpcDialogViewDetailMonoItem:SetData(data)
    self.Cell:SetData(data)
    self.Name.text = data:Name()
end