ShengQiTreasureMap = ShengQiTreasureMap or DeclareView("ShengQiTreasureMap", "sheng_qi/shengqi_treasure_map")

ShengQiTreasureMapPanel = ShengQiTreasureMapPanel or DeclareMono("ShengQiTreasureMapPanel", UIWFlushPanel)
function ShengQiTreasureMapPanel:ShengQiTreasureMapPanel()

end
function ShengQiTreasureMapPanel:onFlush()

    local item = TreasureMapData.Instance:MapInfo()
    local param = item.param or {}
    self.param = param
    self.item = item
    if self.MapSeter:TextureIsNil() then
        self.MapSeter:SetByPath(MapData.Instance:GetMapMiniPath(param.scene_id))
    end

    self.TargetPos.localPosition = MapCtrl.Instance:WorldToTransformUV(self:GetTargetPos(param), self.MapTex,
        param.scene_id)

    if self.Effect then
        self.Effect.localPosition = self.TargetPos.localPosition
    end

    local scene_cfg = SceneData.Instance:GetSceneCfg(param.scene_id)
    if scene_cfg and scene_cfg.entry_point == nil then
        BuglyReport("star treasure map data is nil uuid = ", param.client_need_uid)
    end
end
function ShengQiTreasureMapPanel:OnClickClose()
    ViewMgr:CloseView(ShengQiTreasureMap)
end
function ShengQiTreasureMapPanel:OnClickToGo()
    if SceneLogic.Instance:SceneID() == self.param.scene_id then
        self:AutoToPosFunc()
    else
        SceneLogic.Instance:AutoToPos(self.param.scene_id,nil,BindTool.Bind(self.AutoToPosFunc, self),nil,true)
    end
    ViewMgr:CloseView(ShengQiTreasureMap)
end
function ShengQiTreasureMapPanel:AutoToPosFunc()
    local target_pos = SceneCtrl.Instance:CurScene():PosToClint(self.param.x,self.param.y)
        local obj = SE:Play(SceneEffectConfig.EffectList.StarTreasureMap,target_pos)
        StarTreasureMapData.Instance:AddEffect(self.param.client_need_uid1,obj)
        SceneLogic.Instance:AutoToPos(nil,self.param,function()
            -- StarTreasureMapCtrl.Instance:EndTouchEnded()
            MainOtherCtrl.Instance:OpenKeyUseView(self.item)
        end,true)
end

function ShengQiTreasureMapPanel:GetTargetPos(param)
    if SceneLogic.Instance:SceneID() ~= param.scene_id then
        local cfg = SceneData.Instance:GetSceneCfg(param.scene_id) or {}
        return GameMath.PosToClint(param.x, param.y, cfg.mapid)
    else
        return SceneCtrl.Instance:CurScene():PosToClint(param.x, param.y)
    end
end
