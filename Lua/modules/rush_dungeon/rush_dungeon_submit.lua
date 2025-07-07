RushDungeonSubmit = RushDungeonSubmit or DeclareView("RushDungeonSubmit", "rush_dungeon/rush_dungeon_submit")
function RushDungeonSubmit:RushDungeonSubmit()
    self.data = RushDungeonData.Instance
end
function RushDungeonSubmit:LoadCallback()
    self.Alert:SetData(self:GetType(), Vector2.New(480,354))
end
function RushDungeonSubmit:OnFlush()
    local npc_id = RushDungeonData.Instance:NpcId()
    if npc_id then
        local cfg = self.data:CheckNpcInDungeon(npc_id)
        local item_data = Item.Create({item_id = 0})
        item_data.item_id = cfg.param_2
        item_data.num = cfg.param_3
        self.ItemCell:SetData(item_data)--SetDataByItemId(cfg.param_2)
    end
end
function RushDungeonSubmit:OnClickSubmit()
    local obj_id = RushDungeonData.Instance:ObjId()
    if obj_id then
        local param_t = {
            req_type = BRAVE_GROUND_REQ_TYPE.USE_NPC,
            param1 = obj_id
        }
        RushDungeonCtrl.Instance:SendBraveGroundReq(param_t)
    end
    if self.ItemCell:GetData():Num() == 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[61])
    end
    ViewMgr:CloseView(RushDungeonSubmit)
end