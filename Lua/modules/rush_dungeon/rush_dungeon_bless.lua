RushDungeonBless = RushDungeonBless or DeclareView("RushDungeonBless", "rush_dungeon/rush_dungeon_bless")
function RushDungeonBless:RushDungeonBless()
    self.lang = Language.RushDungeon
end
function RushDungeonBless:LoadCallback()
    self.Board:SetData(self:GetType(), "", Vector2.New(841, 432))
end

RushDungeonBlessPanel = RushDungeonBlessPanel or DeclareMono("RushDungeonBlessPanel", UIWFlushPanel)
function RushDungeonBlessPanel:RushDungeonBlessPanel()
    -- body
end
function RushDungeonBlessPanel:OnClickBless()
    local vo = RushDungeonData.Instance:NpcVo()
    if vo then
        local param_t = {
            req_type = BRAVE_GROUND_REQ_TYPE.USE_NPC,
            param1 = vo.obj_id
        }
        RushDungeonCtrl.Instance:SendBraveGroundReq(param_t)
    end
    ViewMgr:CloseView(RushDungeonBless)
end