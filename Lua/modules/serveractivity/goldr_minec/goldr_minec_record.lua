
GoldrMinecRecordView = GoldrMinecRecordView or DeclareView("GoldrMinecRecordView", "serveractivity/goldr_minec/goldr_minec_record_view")
VIEW_DECLARE_MASK(GoldrMinecRecordView, ViewMask.BgBlockClose)
function GoldrMinecRecordView:LoadCallback(param_t) end 
function GoldrMinecRecordView:CloseCallback()end

function GoldrMinecRecordView:OnClickClose()
    ViewMgr:CloseView(GoldrMinecRecordView)
end

--===========================GoldrMinecRecordPanel===========================
GoldrMinecRecordPanel = GoldrMinecRecordPanel or DeclareMono("GoldrMinecRecordPanel", UIWFlushPanel)
function GoldrMinecRecordPanel:GoldrMinecRecordPanel() 
    self.data = GoldRMinecData.Instance
    local list = self.data:GetRecordList()
    self.empty:SetActive(#list == 0)
    self.list:SetData(list)
end 

function GoldrMinecRecordPanel:OnFlush() end
GoldrMinecRecordCell = GoldrMinecRecordCell or DeclareMono("GoldrMinecRecordCell",UIWidgetBaseItem)
function GoldrMinecRecordCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data) 
    UH.SetText(self.str ,data.desc)
end 
