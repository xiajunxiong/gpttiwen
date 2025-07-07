FILE_REQ("modules/bianqiang/bianqiang_data")
FILE_REQ("modules/bianqiang/bianqiang_config")
VIEW_REQ("modules/bianqiang/bianqiang_view")
VIEW_REQ("modules/bianqiang/bianqiang_alert")
VIEW_REQ("modules/bianqiang/jinritongji_view")
VIEW_REQ("modules/bianqiang/jinritongji_panel")

BianQiangCtrl = BianQiangCtrl or BaseClass(BaseCtrl)
function BianQiangCtrl:__init()
    if BianQiangCtrl.Instance ~= nil then
        Debuger.LogError("[BianQiangCtrl] attempt to create singleton twice!")
        return
    end
    BianQiangCtrl.Instance = self
    self.data = BianQiangData.New()
    self:RegisterAllProtocols()
end

function BianQiangCtrl:__delete()
    if BianQiangCtrl.Instance == self then
        BianQiangCtrl.Instance = nil
    end
    if self.care_role_data ~= nil then
        RoleData.Instance:GetBaseData():Uncare(self.care_role_data)
        self.care_role_data = nil
    end
end

function BianQiangCtrl:GetData()
    return self.data
end

function BianQiangCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

-- BianQiangGroupId
function BianQiangCtrl:OpenByGroupId(group_id)
    local opened, open_tip = FunOpen.Instance:GetFunIsOpened(Mod.BianQiang.Main)
    if opened then
        self.data:SetGroupId(group_id)
    end
    ViewMgr:OpenViewByKey(Mod.BianQiang.Main)
end

function BianQiangCtrl:OnInit()
    self.care_role_data = RoleData.Instance:GetBaseData():Care(BindTool.Bind(self.OnRoleDataChange, self), "level", "profession_id")
    self.data:OnInit()
	EventSys:Bind(GlobalEventKeys.ServerOpenDayChange, BindTool.Bind1(self.OnDayChange, self))
end

function BianQiangCtrl:OnRoleDataChange()
    self.data:OnRoleDataChange()
end

function BianQiangCtrl:OnDayChange()
    self.data:OnDayChange()
end

function BianQiangCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCIncomeRoleInfo, "OnSCIncomeRoleInfo")
    self:RegisterProtocol(CSIncomeRoleReq)
end

function BianQiangCtrl:SendReq(req_type)
    local protocol = GetProtocol(CSIncomeRoleReq)
    protocol.req_type = req_type
    SendProtocol(protocol)
end

function BianQiangCtrl:SendRoleInfo()
    self:SendReq(BianQiangReqType.TYPE_ROLE_INFO)
end
function BianQiangCtrl:OnSCIncomeRoleInfo(protocol)
    self.data:SetIncomeRoleInfo(protocol)
end