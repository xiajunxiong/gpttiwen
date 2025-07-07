YingChunRecordView = YingChunRecordView or DeclareView("YingChunRecordView", 'serveractivity/ying_chun_na_fu/ying_chun_record')
function YingChunRecordView:YingChunRecordView()
end

function YingChunRecordView:LoadCallback(param_t)
end

function YingChunRecordView:OnClickClose()
    ViewMgr:CloseView(YingChunRecordView)
end

--panel
YingChunRecordPanel = YingChunRecordPanel or DeclareMono("YingChunRecordPanel", UIWFlushPanel)
function YingChunRecordPanel:YingChunRecordPanel()
end

function YingChunRecordPanel:Awake()
    UIWFlushPanel.Awake(self)
    self.data_cares = {
        {data = YingChunData.Instance.ying_chun_data, func = self.FlushData, init_call = false, keys = {"flush_info"}}
    }
    self:FlushData()
end

function YingChunRecordPanel:FlushData()
    local record=YingChunData.Instance:GetRecord()
    self.List:SetData(record)
end

function YingChunRecordPanel:OnClickName(data)
    CellClicks.BagGridNoButton({
        GetData = function()
            return Item.Init(data.item_id, data.num, data.is_bind)
        end,
    })
end

--item
YingChunRecordItem = YingChunRecordItem or DeclareMono("YingChunRecordItem", UIWidgetBaseItem)
function YingChunRecordItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TextTime, os.date(Language.ActZysc.cjjl_time, data.timestamp))
    local item = Item.Init(data.item_id, data.num, data.is_bind)
    local name = item:QuaName(true)
    name = string.gsub(name, ">", ">", 1)
    name = string.gsub(name, "</", "</")
    UH.SetText(self.TextName, name)
    UH.SetText(self.TextNum, "x" .. data.num)
end

