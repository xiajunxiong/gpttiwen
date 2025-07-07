RushDungeonReward = RushDungeonReward or DeclareView('RushDungeonReward', 'rush_dungeon/rush_dungeon_reward')

RushDungeonRewardPanel = RushDungeonRewardPanel or DeclareMono('RushDungeonRewardPanel', UIWFlushPanel)
function RushDungeonRewardPanel:RushDungeonRewardPanel()
    self.data = RushDungeonData.Instance
end
function RushDungeonRewardPanel:onFlush()
    self.List:SetData(self.data:RankReward())
    self:TitleShow(true)
end
function RushDungeonRewardPanel:TitleShow(is_rank)
    for i = 1, 2 do
        self.Rank[i]:SetActive(is_rank)
        self.Pass[i]:SetActive(not is_rank)
    end
end
function RushDungeonRewardPanel:OnClickToggle(value)
    self:TitleShow(value == 1)
    if value == 1 then
        self.List:SetData(self.data:RankReward())
    else
        self.List:SetData(self.data:PassReward())
    end
end
function RushDungeonRewardPanel:OnClickClose()
    ViewMgr:CloseView(RushDungeonReward)
end

RushDungeonRewardItem = RushDungeonRewardItem or DeclareMono('RushDungeonRewardItem', UIWidgetBaseItem)
function RushDungeonRewardItem:RushDungeonRewardItem()
    self.rank_sp = {
        'paiming_1',
        'paiming_2',
        'paiming_3'
    }
end
function RushDungeonRewardItem:SetData(data)
    if data.is_rank == true then
        if data.min == data.max and data.max <= 3 then
            self.RankIcon:SetObjActive(true)
            UH.SpriteName(self.RankIcon, self.rank_sp[data.min])
            UH.SetText(self.RankText, '')
        else
            self.RankIcon:SetObjActive(false)
            UH.SetText(self.RankText, string.format('%s-%s', data.min, data.max))
        end
    else
        self.RankIcon:SetObjActive(false)
        UH.SetText(self.RankText, string.format(Language.RushDungeon.LevelName, DataHelper.GetDaXie(data.level)))
    end
    self.RewardItemList:SetData(data.item_list)
end
