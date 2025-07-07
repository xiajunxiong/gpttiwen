LunJianReportView = LunJianReportView or DeclareView("LunJianReportView", "lun_jian/lun_jian_report")
VIEW_DECLARE_LEVEL(LunJianReportView,ViewLevel.L1)

function LunJianReportView:LunJianReportView()
 
end

function LunJianReportView:LoadCallback()

end

function LunJianReportView:CloseCallback()

end

function LunJianReportView:OnClickClose()
    ViewMgr:CloseView(LunJianReportView)
end
------------------------------------------------------------------------

LunJianReportPanel = LunJianReportPanel or DeclareMono("LunJianReportPanel", UIWFlushPanel)
--GroupList========UIWidgetList[LunJianReportGroupItem]
--ModelShow=========ModelUIShow
--RankList==========UIWidgetList[LunJianReportRankItem]
--MyScore=========Text
--MyName======Text
--MyRank======Text
--FirstName===Text
--TopRankIcons===UIWSubstitute[UIWAvatarCell][3]
--TopRankNames===Text[3]
--TopRankScores==Text[3]

function LunJianReportPanel:LunJianReportPanel()
    local group_list_data = LunJianData.Instance:AllGroupId()
    local group_data = {}
    for i,v in ipairs(group_list_data) do
        group_data[i] = {idx = i,group_id = v}
    end
    local my_group_id = LunJianData.Instance:MyGroupId()
    LunJianData.Instance:SetReportSeledGroup((my_group_id ~= nil and my_group_id > 0) and my_group_id or 1)
    self.GroupList:SetData(group_data)
    self.GroupList:ClickItem(1)
    UH.SetText(self.MyName,RoleData.Instance:GetRoleName())
    LunJianCtrl.Instance:ReqMyInfo()
    self.data_cares = {
		{data = LunJianData.Instance:MyInfo(), func = self.FlushMyInfo,keys = {"rank","score"}},
        {data = LunJianData.Instance:ViewData(), func = self.FlushRankList, keys = {"report_seled_group"},init_call = false},
        {data = LunJianData.Instance:RankData(), func = self.FlushRankList},
    }
end

function LunJianReportPanel:FlushMyInfo()
    UH.SetText(self.MyScore,tostring(LunJianData.Instance:MyScore()))
    local rank = LunJianData.Instance:MyRank()
    UH.SetText(self.MyRank,tostring(rank<= 0 and "--" or rank))
end

function LunJianReportPanel:FlushRankList()
    local cur_seld_group = LunJianData.Instance:ReportSeledGroup()
    local rank_data,top3_info = LunJianData.Instance:GetGroupRankList(cur_seld_group)
    if rank_data == nil then
        LunJianCtrl.Instance:ReqRankList(cur_seld_group)
        return
    end
    local rank_data4 = {}
    for i=4,#rank_data do
        table.insert(rank_data4,rank_data[i])
    end
    self.RankList:SetData(rank_data4)
    if self.ui_obj == nil then
        self.ui_obj = UIChDrawer.CreateByAppearance(top3_info[1])
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    else
        self.ui_obj:SetData(top3_info[1])
    end
    UH.SetText(self.FirstName,rank_data[1].name)
    for i=1,3 do
        local top_info = top3_info[i]
        local top_data = rank_data[i]
        self.TopRankIcons[i]:SetData({
            avatar_type   = top_info.avatar_type,
            -- avatar_id     = data.headshot_id,
            avatar_quality= top_info.fly_flag--头像品质 飞升1 ~ 2
        -- top_level     = 巅峰头像框
            -- avatar_level  = data.level,
        -- image_type    = 头像背景类型 可不传  
        })
        UH.SetText(self.TopRankNames[i],rank_data[i].name)
        UH.SetText(self.TopRankScores[i],string.format(Language.LunJian.ReportScore,rank_data[i].score))
    end
end

function LunJianReportPanel:OnDestroy()
    Delete(self,"ui_obj")
    UIWFlushPanel.OnDestroy(self)
end

function LunJianReportPanel:IsLoaded()
    return self.GroupList:IsLoaded() and self.RankList:IsLoaded()
end

------------------------------------------------------------------------

LunJianReportGroupItem = LunJianReportGroupItem or DeclareMono("LunJianReportGroupItem", UIWidgetBaseItem)
--Name===========Text
--ClickEvent=====LuaUIEventToggle
function LunJianReportGroupItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.Name,LunJianData.GroupIdToName(data.group_id))
    self.gameObject.name = string.format("item (%d)",data.idx)
end

function LunJianReportGroupItem:Click()
    self.ClickEvent:Trigger()
end

function LunJianReportGroupItem:OnClickGroupItem()
    LunJianData.Instance:SetReportSeledGroup(self.data.group_id)
end


------------------------------------------------------------------------

LunJianReportRankItem = LunJianReportRankItem or DeclareMono("LunJianReportRankItem", UIWidgetBaseItem)
--RankTxt=======Text
--Name==========Text
--Score=========Text
function LunJianReportRankItem:SetData(data)
    self.gameObject.name = string.format("Item (%d)",data.rank)
    UH.SetText(self.RankTxt,data.rank)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Score,data.score)
end