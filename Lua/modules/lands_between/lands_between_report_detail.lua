LandsBetweenReportDetailView = LandsBetweenReportDetailView or DeclareView("LandsBetweenReportDetailView", "lands_between/lands_between_report_detail",Mod.LandsBetween.ReportDetail)
function LandsBetweenReportDetailView:LandsBetweenReportDetailView()

end

function LandsBetweenReportDetailView:OnCloseClick()
    ViewMgr:CloseView(LandsBetweenReportDetailView)
end

function LandsBetweenReportDetailView:LoadCallback(param)
    if param ~= nil then 
        local item_list = {}
        for k,v in pairs(param.essence_list) do 
            table.insert(item_list,v)
        end 
        
        if param.essence_num > 0 then 
            local sp_item = {}
            sp_item.item_id = CommonItemId.GodSpaceEssence
            sp_item.num = param.essence_num

            table.insert(item_list,1,sp_item)
        end 
        
        self.RewardList:SetData(item_list)
    end 
end

function LandsBetweenReportDetailView:CloseCallback()

end
