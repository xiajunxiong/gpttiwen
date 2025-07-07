
FightViewExp = FightViewExp or DeclareMono("FightViewExp", UIWFlushPanel)

function FightViewExp:FightViewExp()
	self.data = FightCtrl.Instance:Data()
end

function FightViewExp:OnEnable()
	UIWFlushPanel.OnEnable(self)
	local exp_list = self.data:GetFieldListWithType(1)
	self.ShowList:SetDataList(exp_list)
	self.data.cur_sel_field:Set(exp_list[1])
	self.ShowList:JumpToTop()
end
