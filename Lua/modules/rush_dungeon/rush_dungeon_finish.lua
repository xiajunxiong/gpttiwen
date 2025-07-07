RushDungeonFinish = RushDungeonFinish or DeclareView("RushDungeonFinish", "rush_dungeon/rush_dungeon_finish")
function RushDungeonFinish:RushDungeonFinish()
    self.lang = Language.RushDungeon
end
function RushDungeonFinish:LoadCallback()
end

RushDungeonFinishPanel = RushDungeonFinishPanel or DeclareMono("RushDungeonFinishPanel", UIWFlushPanel)
function RushDungeonFinishPanel:RushDungeonFinishPanel()
    self.data = RushDungeonData.Instance
    self.data_cares = {
        -- {data = self.data:RankInfo(), func = self.FlushMyRank, keys = {"role_rank"}, init_call = true},
        {data = self.data:LayerInfo(), func = self.FlushMyLayer, init_call = true }
    }
end
function RushDungeonFinishPanel:FlushMyLayer()
    local layer = self.data:LayerInfo()
    UH.SetText(self.Desc, string.format(Language.RushDungeon.Finish, layer.end_level))
    local data = self.data:RewardList()
    self.ItemList:SetData(data)
end
function RushDungeonFinishPanel:OnClickClose()
    ViewMgr:CloseView(RushDungeonFinish)
end