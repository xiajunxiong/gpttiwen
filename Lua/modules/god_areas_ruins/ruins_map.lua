RuinsMap = RuinsMap or DeclareView("RuinsMap", "god_areas_ruins/ruins_map")

function RuinsMap:RuinsMap()
end

function RuinsMap:LoadCallback(param_t)
    
end

RuinsMapPanel = RuinsMapPanel or DeclareMono("RuinsMapPanel", UIWFlushPanel)
function RuinsMapPanel:RuinsMapPanel()
    self.data = RuinsData.Instance
    self.data_cares = {
        {data = self.data:GetRoleInfo(), func = self.onFlush, keys = {"platform_id"}},
        {data = self.data:GetSceneInfo(), func = self.onFlush, keys = {"flush_role"}},
    }
end
function RuinsMapPanel:onFlush()
    local scene_list = self.data:SceneInfo()
    for i = 1, self.Items:Length() do
        self.Items[i]:SetData(scene_list[i])
    end
end
function RuinsMapPanel:OnClickClose()
   ViewMgr:CloseView(RuinsMap)
end


RuinsMapItem = RuinsMapItem or DeclareMono("RuinsMapItem", UIWidgetBaseItem)
function RuinsMapItem:RuinsMapItem()
    self.ruins_data = RuinsData.Instance
end
function RuinsMapItem:SetData(data)
    UH.SetText(self.LayerName, data.name)
    local role_list = self.ruins_data:GetSceneRoleList(data.platform_id)
    UH.SetText(self.RoleNum, table.nums(role_list))
    local role_info = self.ruins_data:GetRoleInfo()
    self.Active.isOn = data.platform_id == role_info.platform_id
end