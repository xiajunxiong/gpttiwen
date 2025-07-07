
FightViewField = FightViewField or DeclareMono("FightViewField", UIWFlushPanel)

function FightViewField:FightViewField()
	self.data = FightCtrl.Instance:Data()
	self.data.cur_sel_field:Set()
	GuideManager.Instance:RegisterGetGuideUi(
        "FightSceneBtn",
        function()
           return self.ShowList:GetItemMonoByIndex(1)
        end
    )
end

function FightViewField:OnEnable()
	UIWFlushPanel.OnEnable(self)
	local field_list = self.data:GetFieldListWithType(2)
	self.ShowList:SetDataList(field_list)
	self.data.cur_sel_field:Set(field_list[1])
	self.ShowList:JumpToTop()
	self.data.cur_sel_tabbar.val = FightConfig.FightTabType.field
end