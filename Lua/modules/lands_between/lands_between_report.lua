LandsBetweenReportView = LandsBetweenReportView or DeclareView("LandsBetweenReportView", "lands_between/lands_between_report",Mod.LandsBetween.Report)
function LandsBetweenReportView:LandsBetweenReportView()
    self.Board:SetData(self:GetType(),Language.LandsBetween.Title.Report,Vector2.New(530,567))
    self.Board:SetCloseFunc(BindTool.Bind(self.OnCloseFunc, self))

    -- 启动没战报，全获取
    if #LandsBetweenData.Instance.report_info.list == 0 then 
        LandsBetweenCtrl.Instance:GetAllReports()
    end 
end

function LandsBetweenReportView:OpenCallback()

end

function LandsBetweenReportView:CloseCallback()
    -- LandsBetweenData.Instance:ClearReportNew()
end 

function LandsBetweenReportView:OnCloseFunc()
    ViewMgr:CloseView(LandsBetweenReportView)
end

LandsBetweenReportPanel = LandsBetweenReportPanel or DeclareMono("LandsBetweenReportPanel", UIWFlushPanel)
function LandsBetweenReportPanel:LandsBetweenReportPanel()
    self.data = LandsBetweenData.Instance
    self.data_cares = {
        {data = self.data.report_info, func = self.FlushPanel},
    }

    self.report_list = nil
end 

function LandsBetweenReportPanel:Awake()
    UIWFlushPanel.Awake(self)

end

function LandsBetweenReportPanel:FlushPanel()
    self.report_list = self.data:GetReportList()
    self.list:SetData(self.report_list)

    self.Empty:SetActive(#self.report_list == 0)
end

function LandsBetweenReportPanel:OnClickOneDelete()
    -- LandsBetweenData.Instance:ReportDeleteMark(self.report_list)
    -- self.list:SetData({})
    LandsBetweenCtrl.Instance:ReqDeleteReports()
end

function LandsBetweenReportPanel:OnClickOneRead()
    for k,v in pairs(self.list.item_list) do
        v:OnClickReport()
    end 
end

LandsBetweenReportCell = LandsBetweenReportCell or DeclareMono("LandsBetweenReportCell", UIWidgetBaseItem)
function LandsBetweenReportCell:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

    local str = Language.LandsBetween.ReportTips[data.report_type]
    if data.report_type == 1 then 
        str = string.format(Language.LandsBetween.ReportTips[data.report_type],
        data.opponent_name == "" and "" or string.format(Language.LandsBetween.Desc.Player,data.opponent_name))
    end
    
    if #self.data.essence_list > 0 or self.data.essence_num > 0 then 
	    str = str .. RichTextHelper.LinkStr(ColorStr(Language.LandsBetween.Desc.ReportDetail,COLORSTR.Green7), "_ul", COLORSTR.Green7)
    end 
    self.Text.text = str
    self.is_new:SetActive(self.data.is_read == 0)
    self.done:SetActive(data.is_read > 0)
    self.read:SetActive(data.is_read == 0)

end 

function LandsBetweenReportCell:OnClickDetail()
    if #self.data.essence_list > 0 or self.data.essence_num > 0 then 
        LandsBetweenData.Instance:ReadReportDetail(self.data.index)
    end 
end

function LandsBetweenReportCell:OnClickReport()
    LandsBetweenData.Instance:ReadReport(self.data.index)
    self.data.is_read = 1
    self.is_new:SetActive(self.data.is_read == 0)
    self.done:SetActive(self.data.is_read > 0)
    self.read:SetActive(self.data.is_read == 0)
end