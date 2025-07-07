HalloweenPrayRecord = HalloweenPrayRecord or DeclareView("HalloweenPrayRecord", "serveractivity/halloween_pray/halloween_pray_record")
VIEW_DECLARE_LEVEL(HalloweenPrayRecord, ViewLevel.Lt)
VIEW_DECLARE_MASK(HalloweenPrayRecord, ViewMask.BgBlock)

function HalloweenPrayRecord:HalloweenPrayRecord()
end

function HalloweenPrayRecord:LoadCallback(param_t)
      self.Panels[ HalloweenPrayData.Instance:GetShowStyle()]:SetActive(true)
end

function HalloweenPrayRecord:OnClickName(data)
    CellClicks.BagGridNoButton({
        GetData = function()
            return Item.Init(data.item_id, data.num, data.is_bind)
        end,
    })
end

function HalloweenPrayRecord:OnCloseClick()
    ViewMgr:CloseView(HalloweenPrayRecord)
end

 HalloweenPrayRecordPanel = HalloweenPrayRecordPanel or DeclareMono("HalloweenPrayRecordPanel", UIWFlushPanel)
function HalloweenPrayRecordPanel:HalloweenPrayRecordPanel()
       self.List:SetData(HalloweenPrayData.Instance.pray_data.log_list)
end

HalloweenPrayRecordItem = HalloweenPrayRecordItem or DeclareMono("HalloweenPrayRecordItem", UIWidgetBaseItem)

function HalloweenPrayRecordItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.TextTime, os.date(Language.ActZysc.cjjl_time, data.timestamp))
    local item = Item.Init(data.item_id, data.num, data.is_bind)
    local name = item:QuaName(true)
    name = string.gsub(name, ">", ">", 1)
    name = string.gsub(name, "</", "</")
    UH.SetText(self.TextName, name)
    UH.SetText(self.TextNum, "x" .. data.num)
end

