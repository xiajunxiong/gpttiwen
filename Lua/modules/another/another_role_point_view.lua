AnotherRolePointView = AnotherRolePointView or DeclareMono("AnotherRolePointView", UIWFlushPanel)

function AnotherRolePointView:AnotherRolePointView()
    self.data = AnotherData.Instance
    self.data_cares = {
        {data = self.data.point_data, func = self.FlushAttrList},
        {data = self.data.role_point_data, func = self.FlushPointView,keys = {"add_change"}},
    }
end

function AnotherRolePointView:Awake()
    UIWFlushPanel.Awake(self)
    self.plan_index = self.data:GetRoleAllPiontIndex()
end

function AnotherRolePointView:FlushPointView()
    local add_list = self.data:GetRolePointAddList()
    local plan_info = self.data:GetRoleAddPlanInfo()
    local point_attri_data = RoleAddPointData.Instance:GetPointAllAttriData(add_list,plan_info)

    local base_attri_data = self.data:GetRoleAttrList()
    self.AttrList:DataTransBattleAttrNormal(base_attri_data,point_attri_data)
    UH.SetText(self.PionNum,plan_info.add_point_num-self.data:GetRolePointAddNum())
end

function AnotherRolePointView:FlushAttrList()
    local plan_info = self.data:GetRoleAddPlanInfo()
    local add_point_list = plan_info.add_point_list or {}
    for i=1,self.PointList:Length() do
        self.PointList[i]:SetData(add_point_list[i-1] or {})
    end
    UH.SetText(self.PionNum,plan_info.add_point_num)
    UH.SetText(self.PlanName,plan_info.plan_name)
    UH.SetText(self.CurPlan,plan_info.plan_name)
    
    local plan_list = self.data:GetRoleAddPlanList()
    for i=1,self.PlanList:Length() do
        if plan_list[i-1] then
            UH.SetText(self.PlanList[i],plan_list[i-1].plan_name)
        end
    end
    self.AttrList:DataTransBattleAttrNormal(self.data:GetRoleAttrList())
end

function AnotherRolePointView:OnClickAddPoint()
    AnotherCtrl.Instance:SendRolePointReq(1,self.data:GetRolePointAddList(),self.plan_index)
    self:ResetPiontData()
    if AnotherData.IsGuide() then
        return
    end
    self.data.role_point_data.view_show = false
end

function AnotherRolePointView:OnClickWash()
    PublicPopupCtrl.Instance:AlertTip(Language.Battlefield.WashTips,function()
        AnotherCtrl.Instance:SendRolePointReq(0,nil,self.plan_index)
        self:ResetPiontData()
    end)
end

function AnotherRolePointView:OnClickHelp()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[40].desc)
end

function AnotherRolePointView:OnClickBestPoint()
    if self.data:GetRoleAddPlanInfo().add_point_num == 0 then 
        PublicPopupCtrl.Instance:Center(ErrorText[15])
        return
    end
    local plan_info = self.data:GetRoleAddPlanInfo()
    local add_list = RoleAddPointData.Instance:GetRecomPointList(plan_info,BattlefieldData.Instance:GetOtherConfig().level_limit)
    self.data:SetRolePointAddList(add_list)
    for i=1,self.PointList:Length() do
        self.PointList[i]:SetAddNum(add_list[i-1] or 0)
    end
end

function AnotherRolePointView:OnClickRename()
    local reset_func = function(name)
        AnotherCtrl.Instance:SendRolePlanChangeNameReq(self.plan_index, name)
        AnotherCtrl.Instance:SendAllReq(ANOTHER_INFO_TYPE.POINT)
    end
    PublicPopupCtrl.Instance:ResetName({reset_func = reset_func,tips = Language.ResetName.Tips1})
end

function AnotherRolePointView:OnClickPlanParent()
    self.PlanParent:SetActive(not self.PlanParent.activeSelf)
end

function AnotherRolePointView:OnClickSelectPlan(index)
    AnotherCtrl.Instance:SendRolePointReq(2,nil,index)
    self.PlanParent:SetActive(false)
    self.plan_index = index
    self:ResetPiontData()
end

function AnotherRolePointView:ResetPiontData()
    self.data:SetRolePointAddList()
    for i=1,self.PointList:Length() do
        self.PointList[i]:ResetPoint()
    end
end

--==========================AnotherRolePointItem=========================
AnotherRolePointItem = AnotherRolePointItem or DeclareMono("AnotherRolePointItem", UIWidgetBaseItem)

function AnotherRolePointItem:AnotherRolePointItem()
    self.add_num = 0
end

function AnotherRolePointItem:SetData(data)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Value,data.value)
    self:FlushPointPosition(data.value)
    UIWidgetBaseItem.SetData(self, data)
end

--最大200点 开始位置 BattlefieldData.Instance:GetOtherConfig().level_limit 的偏移
function AnotherRolePointItem:FlushPointPosition(value)
    if value ~= 0 then
        local all_num = self:GetMaxNum() or value
        value = value >= all_num and all_num or value
        local add_num = value / all_num * 190
        UH.LocalPos(self.ImgPoint,Vector3.New(BattlefieldData.Instance:GetOtherConfig().level_limit + add_num,0,0))
        UH.SetText(self.Value,value)
    end
    self.ObjPoint:SetActive(value ~= 0)
end

function AnotherRolePointItem:FlushChangePoint(value)
    local piont = value == 0 and "" or "+" .. value
    UH.SetText(self.ChangePoint,piont)
end

function AnotherRolePointItem:OnClickNum(num)
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
    AnotherData.Instance:SetRolePointAddNum(self.data.type,self.add_num)
end

function AnotherRolePointItem:OnClickNums()
    if self:GetCurNum() == 0 then
        PublicPopupCtrl.Instance:Center(ErrorText[15])
        return
    end
    local piont_num = self.data.value + self.add_num
    local num = self:GetMaxNum() - piont_num
    self:OnClickNum(Valve(num,self:GetCurNum()))
end

function AnotherRolePointItem:GetMaxNum()
    local all_point_num = AnotherData.Instance:GetRoleAddPlanInfo().all_point_num or 0
    return math.floor(all_point_num / 2)
end

function AnotherRolePointItem:GetCurNum()
    local add_point_num = AnotherData.Instance:GetRoleAddPlanInfo().add_point_num or 0
    return add_point_num - AnotherData.Instance:GetRolePointAddNum()
end

function AnotherRolePointItem:ResetPoint()
    UH.SetText(self.ChangePoint,"")
    self.add_num = 0
end

function AnotherRolePointItem:SetAddNum(value)
    if self.data == nil then
        return
    end
    self.add_num = value
    self:FlushChangePoint(self.add_num)
    local curr_value = self.data.value or 0
    self:FlushPointPosition(curr_value + self.add_num)
    AnotherData.Instance:SetRolePointAddNum(self.data.type,self.add_num)
end

--长按功能-----------------------
function AnotherRolePointItem:OnClickDown(num)
    TimeHelper:CancelTimer(self.time_func)
    self.time_func = TimeHelper:AddRunTimer(function()
        self:OnClickNum(num)
	end,0.2)
end

function AnotherRolePointItem:OnClickUp()
    TimeHelper:CancelTimer(self.time_func)
end

function AnotherRolePointItem:OnDestroy()
    TimeHelper:CancelTimer(self.time_func)
end