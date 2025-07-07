
FightGatherMenuResultView = FightGatherMenuResultView or DeclareView("FightGatherMenuResultView", FightConfig.ResPath .. "fight_gather_menu_result")

VIEW_DECLARE_MASK(FightGatherMenuResultView,ViewMask.Block)

function FightGatherMenuResultView:OnClickClose()
	ViewMgr:CloseView(FightGatherMenuResultView)
end

FightGatherMenuResultViewDetail = FightGatherMenuResultViewDetail or DeclareMono("FightGatherMenuResultViewDetail", UIWFlushPanel)

function FightGatherMenuResultViewDetail:FightGatherMenuResultViewDetail()
	self.data = FightCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data:GetCollectGatherList(), func = self.FlushDetail},
	}
end

function FightGatherMenuResultViewDetail:FlushDetail()
	local collect_gather_list = self.data:GetCollectGatherList()
	self.ResultList:SetData(collect_gather_list:Val())

	local gather_time = TimeHelper.GetServerTime() - self.data.collect_result_start_time
	local t = TimeHelper.FormatDHMS(gather_time)
	self.GatherTime.text = string.format(Language.Fight.Gather.gather_result_time, RichTextHelper.ColorStr(t.hour, COLORSTR.Green7), RichTextHelper.ColorStr(t.min, COLORSTR.Green7))
end