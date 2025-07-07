ResonanceShowView = ResonanceShowView or DeclareView("ResonanceShowView", "divine_costume/divine_costume_resonance_show")
VIEW_DECLARE_MASK(ResonanceShowView, ViewMask.BgBlockClose)
function ResonanceShowView:ResonanceShowView()
	self.AudioOpen = CommonAudio.ViewOpenL2
    self.AudioClose = CommonAudio.ViewCloseL1
end

function ResonanceShowView:LoadCallback()
end

ResonanceShowPanel = ResonanceShowPanel or DeclareMono("ResonanceShowPanel", UIWFlushPanel)
function ResonanceShowPanel:ResonanceShowPanel()
	self.data = DivineCostumeData.Instance
	self.data_cares = {
		{data = self.data.resonace_flag, func = self.FlushShowPanel, init_call = true}
    }
end

function ResonanceShowPanel:FlushShowPanel()
	local data = self.data:GetConfigResonanceData()
	self.List:SetData(data)
end

----------------------------ResonanceShowItem----------------------------
ResonanceShowItem = ResonanceShowItem or DeclareMono("ResonanceShowItem", UIWidgetBaseItem)

function ResonanceShowItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	for k,v in pairs(data) do 
		local attr_name = Language.Common.AttrList[v.attr_type]
		local num = DataHelper.IsPercent(v.attr_type) and Percent(v.attr_add) or v.attr_add
		local text = attr_name.."  +"..num
		UH.SetText(self.ValueText[k],text)
		UH.SetText(self.GradeText,Language.DivineCostume.ResonanceDetail[v.index])
	end
end

