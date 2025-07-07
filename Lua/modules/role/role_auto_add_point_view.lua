RoleAutoAddPointView = RoleAutoAddPointView or DeclareView("RoleAutoAddPointView","role/role_auto_add_point_view")

function RoleAutoAddPointView:RoleAutoAddPointView()
    self.data = RoleAddPointData.Instance
    self.Board:SetData(self:GetType(), Language.Role.RoleAutoAdd, Vector2.New(556,536))
end

function RoleAutoAddPointView:LoadCallback()
    self.data:ResetCurAutoAddPoint()
    self:setPointListData()
end

function RoleAutoAddPointView:OnClickLeftBtn()
    if self.data.auto_add then
        --取消自动
        RoleCtrl.Instance:SendSetAutoAddPoint(SET_AUTO_ADD_POINT_FLAG.SET_AUTO_ADD_POINT_FLAG_ROLE,{index = self.data:GetRoleAllPiontIndex()})
        PublicPopupCtrl.Instance:Center(Language.Role.RoleAutoOffTip)
        ViewMgr:CloseView(RoleAutoAddPointView)
    else
        --没有自动时选推荐
        self.data:SetSuggestAutoAddNum()
        self:setPointListData()
    end
end

function RoleAutoAddPointView:setPointListData()
    for i = 1, self.PointList:Length() do
        self.PointList[i]:SetData({name = Language.Role.AddPointTypeName[i-1], index = i - 1})
    end
end

function RoleAutoAddPointView:OnClickRightBtn()
    if not self.data.auto_add or self.data.role_point_data.changed then
        local param_t = {index = self.data:GetRoleAllPiontIndex()}
        local num_limit = 0
        for k,v in pairs(self.data:GetAutoAddNum()) do
            param_t["param"..k] = v
            num_limit = num_limit + v
        end
        if num_limit ~= 4 then
            PublicPopupCtrl.Instance:Error(num_limit < 4 and Language.Role.AddPointNumLimit or Language.Role.AddPointLevelMax)
            return
        else
            RoleCtrl.Instance:SendSetAutoAddPoint(SET_AUTO_ADD_POINT_FLAG.SET_AUTO_ADD_POINT_FLAG_ROLE,param_t)
            PublicPopupCtrl.Instance:Center(Language.Role.RoleAutoOnTip)
        end
    end
    ViewMgr:CloseView(RoleAutoAddPointView)
end

RoleAutoAddPointPanel = RoleAutoAddPointPanel or DeclareMono("RoleAutoAddPointPanel", UIWFlushPanel)
function RoleAutoAddPointPanel:RoleAutoAddPointPanel()
    self.data = RoleAddPointData.Instance
    self.data_cares = {
        {data = self.data.role_point_data, func = self.FlushBtnText, keys = {"changed"}}
}
end

function RoleAutoAddPointPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.role_point_data.changed = false
end

function RoleAutoAddPointPanel:FlushBtnText()
    UH.SetText(self.LeftText, self.data.auto_add and Language.Role.RoleCancelAutoAdd or Language.Role.RoleAutoSuggest)
    if not self.data.auto_add then
        UH.SetText(self.RightText, Language.Role.RoleAutoOpen)
    elseif self.data.role_point_data.changed then
        UH.SetText(self.RightText, Language.Role.RoleAutoChanged)
    else
        UH.SetText(self.RightText, Language.Role.RoleAutoGoOn)
    end
end
--==========================RoleAutoAddPointItem=========================
RoleAutoAddPointItem = RoleAutoAddPointItem or DeclareMono("RoleAutoAddPointItem", UIWidgetBaseItem)
function RoleAutoAddPointItem:RoleAutoAddPointItem()
    self.data_instance = RoleAddPointData.Instance
    self.add_num = 0
end

function RoleAutoAddPointItem:SetData(data)
    UH.SetText(self.Name,data.name)
    self.add_num = self.data_instance:GetAutoAddNum(data.index)
    UH.SetText(self.Value, string.format(Language.Common.Xstr, self.add_num > 0 and COLORSTR.Green10 or COLORSTR.Yellow13, self.add_num))
    UIWidgetBaseItem.SetData(self, data)
end

function RoleAutoAddPointItem:OnClickAdd(num)
    if self.add_num + num >= 0 then
        local can_add, result = self.data_instance:CanAddAutoNum(self.data.index, num)
        if can_add then
            self.data_instance.role_point_data.changed = true
            self.add_num = result
            UH.SetText(self.Value, string.format(Language.Common.Xstr, self.add_num > 0 and COLORSTR.Green10 or COLORSTR.Yellow13, self.add_num))
        elseif not StringIsEmpty(result) then
            PublicPopupCtrl.Instance:Error(result)
        end
    end
end