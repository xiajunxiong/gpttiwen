ConcentricKnotAlertView = ConcentricKnotAlertView or DeclareView("ConcentricKnotAlertView", "period_activity_act/concentric_knot/concentric_knot_alert")
function ConcentricKnotAlertView:LoadCallback()
    local item_list = ConcentricKnotData.Instance:GetStuffList()
    self.ItemList:SetDataList(item_list, 1)
end

function ConcentricKnotAlertView:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    ViewMgr:CloseView(ConcentricKnotAlertView)
end

function ConcentricKnotAlertView:OnClickOk()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    local data_list = self.ItemList:GetSelectedData()
    if #data_list == 0 then
        PublicPopupCtrl.Instance:Center(Language.ConcentricKnot.NotEnoughItem)
        return
    end
    local data = data_list[1]
    if Item.GetNum(data.item.item_id) < 1 then
        PublicPopupCtrl.Instance:Center(Language.ConcentricKnot.NotEnoughItem)
        return
    end
    local function func_succ()
        ConcentricKnotCtrl.Instance:SendEnter()
    end
    if TeamData.Instance:InTeam() and TeamData.Instance:MemberNumRole() >= 2 then
        local team_type = TeamData.Instance:GetTeamInfo().info.team_type
        if team_type ~= data.target_id then
            local target = TeamData.Instance:GetTeamGoalWithId(data.target_id)
            TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
        end
        func_succ()
    else
        TeamCtrl.Instance:TeamDetectByTargetId(data.target_id, func_succ, 2)
    end
    ViewMgr:CloseView(ConcentricKnotAlertView)
end

ConcentricKnotAlertItem = ConcentricKnotAlertItem or DeclareMono("ConcentricKnotAlertItem", UIWidgetBaseItem)
function ConcentricKnotAlertItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)

    local item = Item.Create(data.item)
    item.num = item:Num()
    self.ItemCell:SetData(item)
    self.ItemCell:MakeGrey(item:Num() < 1)
    UH.SetText(self.ItemName, item:Name())
end

function ConcentricKnotAlertItem:OnClickItem()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
    MainOtherCtrl.Instance:GetWayViewById(self.data.item.item_id)
end

function ConcentricKnotAlertItem:Click()
    self:SetSelected(true)
end
