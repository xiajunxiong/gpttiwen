MazeRunRewardPreview = MazeRunRewardPreview or DeclareView("MazeRunRewardPreview", "maze_run/maze_run_reward_preview")

function MazeRunRewardPreview:OnClickClose()
    ViewMgr:CloseView(MazeRunRewardPreview)
    MazeRunData.Instance.cur_sel_preview.val = 1
end

MazeRunRewardPrePanel = MazeRunRewardPrePanel or DeclareMono("MazeRunRewardPrePanel",UIWFlushPanel)
function MazeRunRewardPrePanel:MazeRunRewardPrePanel()
    -- self.data_cares = {
    --     {data = MazeRunData.Instance.cur_sel_preview, func = self.FlushReward}
    -- }
end

--初始化
function MazeRunRewardPrePanel:Awake()
    UIWFlushPanel.Awake(self)
    -- local list = {}
    -- for i=1,4 do
    --     list[i] = {maze_id = i,name = Language.MazeRun.BtnNames[i]}
    -- end
    -- self.BtnList:SetData(list)
	-- self.BtnList:ClickItemData(list[MazeRunData.Instance.cur_sel_preview.val])
    self:FlushReward()
end

function MazeRunRewardPrePanel:FlushReward()
    local maze_id = MazeRunData.Instance:GetCurMazeId()
    local list = {}
    local role_level = RoleData.Instance:GetRoleLevel()
    for k,v in pairs(MazeRunData.Instance:GetRewardPreviewData(maze_id)) do
        if role_level >= v.min_level and role_level <= v.max_level then
            table.insert(list,v)
        end
    end
	table.sort(list,DataHelper.SortFunc([[rank_min]]))
    self.List:SetData(list)
end

---------------------------MazeRunRewardPreItem-----------------------------
MazeRunRewardPreBtnItem = DeclareMono("MazeRunRewardPreBtnItem", UIWidgetBaseItem)

function MazeRunRewardPreBtnItem:SetData(data)
    UH.SetText(self.Name,data.name)
	UIWidgetBaseItem.SetData(self, data)
end

function MazeRunRewardPreBtnItem:OnClickItem()
	MazeRunData.Instance.cur_sel_preview.val = self.data.maze_id
end

function MazeRunRewardPreBtnItem:Click()
	if self.Toggle then
		self.Toggle:Trigger()
	end
end

---------------------------MazeRunRewardPreItem-----------------------------
MazeRunRewardPreItem = DeclareMono("MazeRunRewardPreItem", UIWidgetBaseItem)

function MazeRunRewardPreItem:SetData(data)
    local RankDesc = Language.ArenaMatch.RewardPreview.RankDesc
    if data.rank_min == data.rank_max then
        UH.SetText(self.Rank,Format(RankDesc[1],data.rank_min))
    else
        UH.SetText(self.Rank,Format(RankDesc.default,data.rank_min,data.rank_max))
    end
	self.Rewards:SetData(DataHelper.FormatItemList(data.reward_show))
end