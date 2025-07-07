
FILE_REQ("modules/task/task_view_accpeted")
FILE_REQ("modules/task/task_view_accpetable")
FILE_REQ("modules/task/task_view_adventure")
FILE_REQ("modules/task/task_view_details")

TaskView = TaskView or DeclareView("TaskView", TaskConfig.ResPath .. "task", Mod.Task.Accepted)
VIEW_DECLARE_LEVEL(TaskView,ViewLevel.L1)

function TaskView:TaskView()
	self.data = TaskCtrl.Instance:Data()
	self.language = Language.Task
end

function TaskView:LoadCallback(param_t)
	self.Board:SetData(self:GetType(),self.language.Task)
	self.Board:SetTabbarVisible(true)

	local tabbar = self.Board.Tabbar
	tabbar:SetData({
			{name = self.language.TaskAccpeted, tab_objs = {self.TabAccpeted, self.TabDetails}, mod = TabMod(Mod.Task.Accepted), open = Mod.Task.Accepted},
			{name = self.language.TaskAccpetable, tab_objs = {self.TabAccpetable, self.TabDetails}, mod = TabMod(Mod.Task.Acceptable), open = Mod.Task.Acceptable},
			-- {name = self.language.TaskAdventure, tab_objs = {self.TabAdventure}, mod = TabMod(Mod.Task.Adventure), open = Mod.Task.Adventure},
		})
	tabbar:ClickItem(FunOpen.Instance:IWantSelect(Mod.Task, tabbar, param_t.open_param))
	self.Currency:DefaultSet()
end

function TaskView:CloseCallback()
	self.data.cur_sel_task:Set({})
end