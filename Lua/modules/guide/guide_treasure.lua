GuideTreasureMap = GuideTreasureMap or DeclareView("GuideTreasureMap", "guide/guide_treasuremap")
VIEW_DECLARE_MASK(GuideTreasureMap,ViewMask.BgBlockClose)
function GuideTreasureMap:GuideTreasureMap()
    self.mapdata = MapData.Instance
end
function GuideTreasureMap:CloseCallback()
    -- self.mapdata:SetSingleFirstEnterInfo(SCENE_TYPE.CRYSTAL_FB,1)
    local role_id = RoleData.Instance:GetBaseData().role_id
	UnityPlayerPrefs.SetInt(PrefKeys.SuperMapGuide(role_id), 1)
end