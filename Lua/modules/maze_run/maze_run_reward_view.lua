MazeRunRewardView = MazeRunRewardView or DeclareView("MazeRunRewardView", "maze_run/maze_run_reward")
---ItemList=======UIWidgetList[UIWItemCell]
---CostTime=======Text
---MonsterWin=====Text
---TeamWin========Text
---RankNum========Text
---ModelShow======ModelUIShow

function MazeRunRewardView:LoadCallback()
	local pass_data = MazeRunData.Instance:PassData()
	self.RankNum.text = pass_data.rank_posi
	self.MonsterWin.text = pass_data.total_encounter
	local ft = TimeManager.FormatDHMS(pass_data.pass_time)
	self.CostTime.text = string.format(Language.MazeRun.CostTime,tostring(ft.min),tostring(ft.s))
	self.ItemList:SetData(pass_data.award_list)

	self.ui_obj = UIChDrawer.CopyMainRoleData()
    self.ModelShow:SetShowData(
        {
            ui_obj = self.ui_obj,
            view = self
        }
    )
end

function MazeRunRewardView:OnBlockClick()
	SceneCtrl.Instance:RequestLeaveFb()
	ViewMgr:CloseView(MazeRunRewardView)
end

function MazeRunRewardView:OnComfirmClick()
	SceneCtrl.Instance:RequestLeaveFb()
	ViewMgr:CloseView(MazeRunRewardView)
end
