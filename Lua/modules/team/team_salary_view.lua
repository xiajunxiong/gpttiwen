
TeamSalaryView = TeamSalaryView or DeclareView("TeamSalaryView", TeamConfig.ResPath .. "team_salary")

function TeamSalaryView:OnClickClose()
	ViewMgr:CloseView(TeamSalaryView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

TeamSalaryViewF = TeamSalaryViewF or DeclareMono("TeamSalaryViewF", UIWFlushPanel)

function TeamSalaryViewF:TeamSalaryViewF()
	self.data = TeamCtrl.Instance:Data()
	self.language = Language.Team.Salary

	self.data_cares = {
		{data = self.data:GetSalaryList(), func = self.FlushList},
		{data = self.data:GetSalaryInfo(), func = self.FlushInfo},
	}
end

function TeamSalaryViewF:FlushList()
	local salary_list = self.data:GetSalaryList()
	self.ShowGrid1:SetData(salary_list)
end

function TeamSalaryViewF:FlushInfo()
	local now_reward = 0
	for i, v in pairs(self.data:GetSalaryList()) do
		now_reward = now_reward + v.vo.times * v.info.reward
	end
	UH.SetText(self.TotalSalary, string.format(self.language.title2, RichTextHelper.ColorStr(now_reward or 0, COLORSTR.Green6)))
	self.Title.text = self.language.title

	local salary_list = self.data:GetSalaryList()
	LogDG(salary_list)
	self.ShowGrid2:SetData(salary_list)
end

function TeamSalaryViewF:OnSalaryStoreClick()
	ViewMgr:CloseView(TeamPlatformView)
	ViewMgr:CloseView(TeamView)
	ShoppingCtrl.Instance:OpenStoreView(9)
	ViewMgr:CloseView(TeamSalaryView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.MenuOpen)
end

TeamSalaryViewItem1 = DeclareMono("TeamSalaryViewItem1", UIWidgetBaseItem)

function TeamSalaryViewItem1:TeamSalaryViewItem1()
	self.language = Language.Team.Salary
end

function TeamSalaryViewItem1:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	
	local info = data.info
	self.Type.text = info.fuben_name .. "："
	self.Value.text = info.reward
end

TeamSalaryViewItem2 = DeclareMono("TeamSalaryViewItem2", UIWidgetBaseItem)

function TeamSalaryViewItem2:TeamSalaryViewItem2()
	self.language = Language.Team.Salary
end

function TeamSalaryViewItem2:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	
	local info = data.info
	local vo = data.vo
	self.Type.text = info.fuben_name .. "："
	self.Value.text = string.format("%s/%s", vo.times * info.reward, info.week_limit * info.reward)
end