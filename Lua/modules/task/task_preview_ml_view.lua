
TaskPreviewMainLineView = TaskPreviewMainLineView or DeclareView("TaskPreviewMainLineView", TaskConfig.ResPath .. "task_preview_ml", Mod.TaskPreviewMainLine.Main)

function TaskPreviewMainLineView:TaskPreviewMainLineView()
	self.data = TaskCtrl.Instance:Data()
	self.language = Language.Task
end

function TaskPreviewMainLineView:LoadCallback(param_t)
	local desc = self.data:PreviewMainLineDesc()
	local task = self.data:GetTaskInfoByTaskType(TaskConfig.TaskType.mainline)

	for i = 1, self.Descs:Length() do
		self.Descs[i].text = desc[i]
	end

	if nil ~= task then
		self.RewardCell:SetData(task.reward_show)
	end
end

function TaskPreviewMainLineView:OnClickClose()
	ViewMgr:CloseView(TaskPreviewMainLineView)
end