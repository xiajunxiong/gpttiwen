RushDungeonComplete = RushDungeonComplete or DeclareView("RushDungeonComplete", "rush_dungeon/rush_dungeon_complete")
function RushDungeonComplete:OnFlush()
    if self.ui_obj == nil then
        self.ui_obj = UIChDrawer.CopyMainRoleData()
    end
    self.ModelShow:SetShowData(
        {
            ui_obj = self.ui_obj,
            view = self
        }
    )
end

function RushDungeonComplete:OnDestroy()
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
    RushDungeonData.Instance:ClearData()
end
function RushDungeonComplete:OnClickClose()
    ViewMgr:CloseView(RushDungeonComplete)
end