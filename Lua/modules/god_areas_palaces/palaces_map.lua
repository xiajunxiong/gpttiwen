PalacesMap = PalacesMap or DeclareView("PalacesMap", "god_areas_palaces/palaces_map")

function PalacesMap:PalacesMap()
end

function PalacesMap:LoadCallback(param_t)
    
end

PalacesMapPanel = PalacesMapPanel or DeclareMono("PalacesMapPanel", UIWFlushPanel)
function PalacesMapPanel:PalacesMapPanel()
    self.data = PalacesData.Instance
    self.data_cares = {
        {data = self.data:GetRoleInfo(), func = self.onFlush, keys = {"platform_id"}},
        {data = self.data:GetSceneInfo(), func = self.onFlush, keys = {"flush_role"}},
    }
end
function PalacesMapPanel:onFlush()
    local scene_list = self.data:SceneInfo()
    for i = 1, self.Items:Length() do
        self.Items[i]:SetData(scene_list[i])
    end
end
function PalacesMapPanel:OnClickClose()
   ViewMgr:CloseView(PalacesMap)
end


PalacesMapItem = PalacesMapItem or DeclareMono("PalacesMapItem", UIWidgetBaseItem)
function PalacesMapItem:PalacesMapItem()
    self.palaces_data = PalacesData.Instance
end
function PalacesMapItem:SetData(data)
    UH.SetText(self.LayerName, data.name)
    local role_list = self.palaces_data:GetSceneRoleList(data.platform_id)
    -- UH.SetText(self.RoleNum, table.nums(role_list))
    local role_info = self.palaces_data:GetRoleInfo()
    self.Active.isOn = data.platform_id == role_info.platform_id
    --新增
    UH.SetText(self.FuncName, Language.GodAreasActs.FuncName[data.area_type])
    for i = 1, self.Names:Length() do
        UH.SetText(self.Names[i], Language.GodAreasActs.CampName[i])
        UH.SetText(self.RoleNums[i], self.palaces_data:GetCampRoleNum(role_list, (i - 1)))
    end
end