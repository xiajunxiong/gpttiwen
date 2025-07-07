RushDungeonName = RushDungeonName or DeclareView("RushDungeonName", "rush_dungeon/rush_dungeon_name")
VIEW_DECLARE_MASK(RushDungeonName, ViewMask.None)
function RushDungeonName:RushDungeonName()
end
function RushDungeonName:LoadCallback()
    local layer_info = RushDungeonData.Instance:LayerInfo()
    self.NameTex.text = string.format(Language.RushDungeon.LevelName, DataHelper.GetDaXie(layer_info.map_level))
    TimeHelper:CancelTimer(self.handle_delay)
    self.handle_delay =
        TimeHelper:AddDelayTimer(
        function()
            ViewMgr:CloseView(RushDungeonName)
        end,
        RushDungeonData.Instance.other_info.level_show
    )
end