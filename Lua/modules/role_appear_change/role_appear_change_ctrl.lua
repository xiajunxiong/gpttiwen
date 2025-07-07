
FILE_REQ("modules/role_appear_change/role_appear_change_data")
VIEW_REQ("modules/role_appear_change/role_appear_change_view")

RoleAppearChangeCtrl = RoleAppearChangeCtrl or BaseClass(BaseCtrl)

function RoleAppearChangeCtrl:__init()
    if RoleAppearChangeCtrl.Instance ~= nil then
        Debuger.LogError("[RoleAppearChangeCtrl] attempt to create singleton twice!")
        return
    end
    RoleAppearChangeCtrl.Instance = self
    self.data = RoleAppearChangeData.New()
    self:RegisterProtocol(CSChangeAppearance)
end

function RoleAppearChangeCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(CSAddItemOnShelve)
end

function RoleAppearChangeCtrl:__delete()
    RoleAppearChangeCtrl.Instance = nil
end

function RoleAppearChangeCtrl:SendAppearanceReq(avartar_type,color)
    local protocol = GetProtocol(CSChangeAppearance)
    protocol.avartar_type = avartar_type or 0
    protocol.color = color or 0
	SendProtocol(protocol)
end

function RoleAppearChangeCtrl:SendAppearanceID()
    local avartar_type = self.data.appear_data.cur_left_select
    local color = self.data.appear_data.cur_color
    self:SendAppearanceReq(avartar_type,color)
end

function RoleAppearChangeCtrl:AutoToNpc()
    SceneLogic.Instance:AutoToNpcs(40015)
	ViewMgr:CloseView(ItemInfoView)
	ViewMgr:CloseView(BagView)
end