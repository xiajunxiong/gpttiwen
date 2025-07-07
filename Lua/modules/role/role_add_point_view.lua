RoleAddPointView = RoleAddPointView or DeclareView("RoleAddPointView","role/role_add_point_view", Mod.Role.RoleAddPoint)

function RoleAddPointView:RoleAddPointView()
    self.Board:SetData(self:GetType(), Language.Role.AddPoint, Vector2.New(965,543))
end

function RoleAddPointView:CloseCallback()
    RoleAddPointData.Instance:SetRolePointAddList()
end

--===================================RoleAddPointPanel===================================
RoleAddPointPanel = RoleAddPointPanel or DeclareMono("RoleAddPointPanel", UIWFlushPanel)

function RoleAddPointPanel:RoleAddPointPanel()
    self.data = RoleAddPointData.Instance
    self.data_cares = {
        {data = self.data.point_data, func = self.FlushAttrList},
        {data = self.data.role_point_data, func = self.FlushPointView,keys = {"add_change"}},
        {data = self.data.role_point_data, func = self.FlushAutoText,keys = {"auto_add"}},
        {data = RoleData.Instance.battle_attri, func = self.FlushAttrView},
    }
end

function RoleAddPointPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.plan_index = self.data:GetRoleAllPiontIndex()
end

function RoleAddPointPanel:FlushPointView()
    local add_list = self.data:GetRolePointAddList()
    local plan_info = self.data:GetRoleAddPlanInfo()
    local point_attri_data = self.data:GetPointAllAttriData(add_list,plan_info)

    local base_attri_data = RoleData.Instance:AllBattleAttri()
    self.AttrList:DataTransBattleAttrNormal(base_attri_data,point_attri_data)
    UH.SetText(self.PionNum,plan_info.add_point_num-self.data:GetRolePointAddNum())
end

function RoleAddPointPanel:FlushAutoText()
    UH.SetText(self.AutoAddText, self.data.auto_add and Language.Role.RoleCancelAutoAdd or Language.Role.RoleAutoAdd)
end

function RoleAddPointPanel:FlushAttrList()
    local plan_info = self.data:GetRoleAddPlanInfo()
    local add_point_list = plan_info.add_point_list or {}
    for i=1,self.PointList:Length() do
        self.PointList[i]:SetData(add_point_list[i-1] or {})
    end
    UH.SetText(self.PionNum,plan_info.add_point_num)
    UH.SetText(self.PlanName,plan_info.plan_name)
    UH.SetText(self.CurPlan,plan_info.plan_name)

    local plan_num = self.data:GetRoleAddPlanNum()
    local plan_list = self.data:GetRoleAddPlanList()
    for i=1,self.PlanList:Length() do
        UH.SetText(self.PlanList[i],plan_list[i-1].plan_name)
        self.PlanList[i]:SetObjActive(plan_num >= i)
    end
    self.CreatePlan:SetActive(plan_num < 3)
    self.AttrList:DataTransBattleAttrNormal(RoleData.Instance:AllBattleAttri())
end

function RoleAddPointPanel:FlushAttrView()
    self.AttrList:DataTransBattleAttrNormal(RoleData.Instance:AllBattleAttri())
end

function RoleAddPointPanel:OnClickAddPoint()
    if BattleData.Instance:IsServerFighting() then
        PublicPopupCtrl.Instance:Center(ErrorText[115])
    end
    ViewMgr:CloseView(RoleAddPointView)
    RoleCtrl.Instance:SendAttributePlanAddPoint(self.data:GetRoleAllPiontIndex(),self.data:GetRolePointAddList())
    self:ResetPiontData()
end

function RoleAddPointPanel:OnClickWashPoint()
    if Item.GetNum(26077) > 0 then
        PublicPopupCtrl.Instance:AlertTip(Language.Role.WashTip,function()
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)
            RoleCtrl.Instance:SendAttributePlanClearPoint(self.data:GetRoleAllPiontIndex())
            self:ResetPiontData()
        end)
    elseif self.data:GetResetPlanTimes() == 0 then
        PublicPopupCtrl.Instance:AlertTip(Language.Role.FreeWashPoint,function()
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ComposeSucc)
            RoleCtrl.Instance:SendAttributePlanClearPoint(self.data:GetRoleAllPiontIndex())
            self:ResetPiontData()
        end)
    else
        PublicPopupCtrl.Instance:AlertTip(Language.Role.PopupTips5,function()
            MallCtrl.Instance:Jump(26077)
        end)
	end
end

function RoleAddPointPanel:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[40].desc)
end

function RoleAddPointPanel:OnClickAutoAdd()
    ViewMgr:OpenView(RoleAutoAddPointView)
end

function RoleAddPointPanel:OnClickBestPoint()
    if self.data:GetRoleAddPlanInfo().add_point_num == 0 then 
        PublicPopupCtrl.Instance:Center(ErrorText[15])
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
    local plan_info = self.data:GetRoleAddPlanInfo()
    local add_list = self.data:GetRecomPointList(plan_info)
    self.data:SetRolePointAddList(add_list)
    for i=1,self.PointList:Length() do
        self.PointList[i]:SetAddNum(add_list[i-1] or 0)
    end
end

function RoleAddPointPanel:OnClickRename()
    local reset_func = function(name)
        RoleCtrl.Instance:SendAttributePlanChangeName(self.data:GetRoleAllPiontIndex(), name)
    end
    PublicPopupCtrl.Instance:ResetName({reset_func = reset_func, tips = Language.ResetName.Tips1})
end

function RoleAddPointPanel:OnClickPlanParent()
    self.PlanParent:SetActive(not self.PlanParent.activeSelf)
end

function RoleAddPointPanel:OnClickSelectPlan(index)
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QieHuanBiaoQian)
    RoleCtrl.Instance:SendAttributePlanSwitchPlan(index)
    self.PlanParent:SetActive(false)
    self.plan_index = index
    self:ResetPiontData()
end

function RoleAddPointPanel:OnClickAddPlan()
    local plan_num = self.data:GetRoleAddPlanNum()
    if plan_num < 3 then
        if Item.GetNum(26077) > 0 then
            PublicPopupCtrl.Instance:AlertTip(Language.Role.RoleAddPlanTip,function()
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral2)
                RoleCtrl.Instance:SendAttributePlanCreate(Language.Role.DefaultPlanName..plan_num + 1)
            end)
        else
            PublicPopupCtrl.Instance:AlertTip(Language.Role.PopupTips5,function()
                MallCtrl.Instance:Jump(26077)
            end)
        end
    end
end

function RoleAddPointPanel:ResetPiontData()
    self.data:SetRolePointAddList()
    for i=1,self.PointList:Length() do
        self.PointList[i]:ResetPoint()
    end
end

--==========================RoleAddPointItem=========================
RoleAddPointItem = RoleAddPointItem or DeclareMono("RoleAddPointItem", UIWidgetBaseItem)

function RoleAddPointItem:RoleAddPointItem()
    self.add_num = 0
end

function RoleAddPointItem:OnDestroy()
	TimeHelper:CancelTimer(self.ht)
    TimeHelper:CancelTimer(self.delay_ht)
end

function RoleAddPointItem:SetData(data)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Value,data.value)
    self:FlushPointPosition(data.value)
    self:FlushChangePoint(0)
    UIWidgetBaseItem.SetData(self, data)
end

--最大200点 开始位置 60 的偏移
function RoleAddPointItem:FlushPointPosition(value)
    if value ~= 0 then
        local all_num = self:GetMaxNum()
        value = value >= all_num and all_num or value
        local add_num = value / all_num * 200
        UH.LocalPos(self.ImgPoint,Vector3.New(60 + add_num,0,0))
        UH.SetText(self.Value,value)
    end
    self.ObjPoint:SetActive(value ~= 0)
end

function RoleAddPointItem:FlushChangePoint(value)
    local piont = value == 0 and "" or "+" .. value
    UH.SetText(self.ChangePoint,piont)
end

function RoleAddPointItem:OnClickNum(num)
    if self:GetCurNum() == 0 and num > 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[15])
        return
    end
    if self.add_num + num < 0 then
        if self.data.value ~= 0 then
            PublicPopupCtrl.Instance:Center(Language.Role.RoleAddPointTip)
        end
        return
    end
    if self.data.value + self.add_num + num > self:GetMaxNum() then
        PublicPopupCtrl.Instance:Center(ErrorText[33])
        return
    end
    self.add_num = self.add_num + num
    self:FlushChangePoint(self.add_num)
    self:FlushPointPosition(self.data.value + self.add_num)
    RoleAddPointData.Instance:SetRolePointAddNum(self.data.type,self.add_num)
end

function RoleAddPointItem:ResetPoint()
    UH.SetText(self.ChangePoint,"")
    self.add_num = 0
end

function RoleAddPointItem:SetAddNum(value)
    self.add_num = value
    self:FlushChangePoint(self.add_num)
    self:FlushPointPosition(self.data.value + self.add_num)
    RoleAddPointData.Instance:SetRolePointAddNum(self.data.type,self.add_num)
end

function RoleAddPointItem:OnClickAdd()
    if self:GetCurNum() == 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[15])
        return
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    local piont_num = self.data.value + self.add_num
    local num = self:GetMaxNum() - piont_num
    if num <= 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[33])
        return
    end
    self:OnClickNum(Valve(num,self:GetCurNum()))
end

function RoleAddPointItem:GetMaxNum()
    return math.floor(RoleAddPointData.Instance:GetRoleAddPlanInfo().all_point_num )--/ 2)
end

function RoleAddPointItem:GetCurNum()
    return RoleAddPointData.Instance:GetRoleAddPlanInfo().add_point_num - RoleAddPointData.Instance:GetRolePointAddNum()
end

--长按功能-----------------------
function RoleAddPointItem:OnClickDown(num)
    TimeHelper:CancelTimer(self.time_func)
    self.time_func = TimeHelper:AddRunTimer(function()
        self:OnClickNum(num)
	end,0.2)
end

function RoleAddPointItem:OnClickUp()
    TimeHelper:CancelTimer(self.time_func)
end

function RoleAddPointItem:OnDestroy()
    TimeHelper:CancelTimer(self.time_func)
end