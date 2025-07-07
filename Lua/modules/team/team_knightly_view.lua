
TeamKnightlyView = TeamKnightlyView or DeclareView("TeamKnightlyView", TeamConfig.ResPath .. "team_knightly")

function TeamKnightlyView:OnClickClose()
	ViewMgr:CloseView(TeamKnightlyView)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
end

TeamKnightlyViewF = TeamKnightlyViewF or DeclareMono("TeamKnightlyViewF", UIWFlushPanel)

function TeamKnightlyViewF:TeamKnightlyViewF()
	self.data = TeamCtrl.Instance:Data()
	self.language = Language.Team.Knightly

	self.data_cares = {
		{data = self.data:GetKnightlyList(), func = self.FlushList},
		{data = self.data:GetKnightlyInfo(), func = self.FlushInfo},
	}
	GuideManager.Instance:RegisterGetGuideUi("XiaYiGuiZeCloseBtn",function ()
		return self.GuideClose
	end)
end

function TeamKnightlyViewF:FlushList()
	local knightly_list = self.data:GetKnightlyList()
	self.ShowGrid1:SetData(knightly_list)
	self.ShowGrid2:SetData(knightly_list)
end

function TeamKnightlyViewF:FlushInfo()
	local knightly_info = self.data:GetKnightlyInfo()
	-- self.TotalKnightly.text = self.language.total .. RichTextHelper.ColorStr(knightly_info.total, COLORSTR.Green6)
	self.TotalKnightly.text = self.language.total .. RichTextHelper.ColorStr(RoleData.Instance:GetCurrency(CurrencyType.Chivalrous), COLORSTR.Green6)
	self.TodayKnightly.text = self.language.today1 .. RichTextHelper.ColorStr(string.format(self.language.today2, knightly_info.today, knightly_info.today_a), COLORSTR.Green6)
	self.Title.text = self.language.title .. (knightly_info.day_val or 0)
end

function TeamKnightlyViewF:OnClickExchange()
	local value, tips = FunOpen.Instance:IsFunOpen(Mod.Store.Main)
	if value then
		ViewMgr:CloseView(TeamKnightlyView)
		ViewMgr:CloseView(TeamView)
		ShoppingCtrl.Instance:OpenStoreView(1)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.MenuOpen)
	elseif tips then
		PublicPopupCtrl.Instance:Center(tips)
	end
end

TeamKnightlyViewItem1 = DeclareMono("TeamKnightlyViewItem1", UIWidgetBaseItem)

function TeamKnightlyViewItem1:TeamKnightlyViewItem1()
	self.language = Language.Team.Knightly
end

function TeamKnightlyViewItem1:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	
	local info = data.info
	self.Type.text = info.fuben_name .. "："
	self.Value.text = info.reward
end

TeamKnightlyViewItem2 = DeclareMono("TeamKnightlyViewItem2", UIWidgetBaseItem)

function TeamKnightlyViewItem2:TeamKnightlyViewItem2()
	self.language = Language.Team.Knightly
end

function TeamKnightlyViewItem2:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	
	local info = data.info
	local vo = data.vo
	self.Type.text = info.fuben_name .. "："
	self.Value.text = string.format("%s/%s", vo.total_salary, info.week_limit * info.reward)
end