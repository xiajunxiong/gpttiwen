ResonanceSuccessView = ResonanceSuccessView or DeclareView("ResonanceSuccessView", "divine_costume/divine_costume_resonance_success")
VIEW_DECLARE_MASK(ResonanceSuccessView, ViewMask.BgBlockClose)
function ResonanceSuccessView:ResonanceSuccessView()
	self.AudioOpen = CommonAudio.ViewOpenL2
    self.AudioClose = CommonAudio.ViewCloseL1
end

function ResonanceSuccessView:LoadCallback()
	
end

function ResonanceSuccessView:OnClickClose()
	ViewMgr:CloseView(ResonanceSuccessView)
end

ResonanceSuccessPanle = ResonanceSuccessPanle or DeclareMono("ResonanceSuccessPanle", UIWFlushPanel)
function ResonanceSuccessPanle:ResonanceSuccessPanle()
	self.data = DivineCostumeData.Instance
	self.data_cares = {
		{data = self.data.resonace_flag, func = self.FlushShowPanel, init_call = true},
    }
end

function ResonanceSuccessPanle:FlushShowPanel()
	local count = self.data.resonace_flag.count
	local data = self.data:GetShowChangeData(count)
	UH.SetText(self.CText,string.format(Language.DivineCostume.Grade,count-1))
	UH.SetText(self.AText,string.format(Language.DivineCostume.Grade,count))
	self.List:SetData(data)
end

----------------------------ResonanceSuccessItem----------------------------
ResonanceSuccessItem = ResonanceSuccessItem or DeclareMono("ResonanceSuccessItem", UIWidgetBaseItem)

function ResonanceSuccessItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	local attr_name = Language.Common.AttrList[data.attr_type]
	if not data.is_new then
		local old_num = DataHelper.IsPercent(data.attr_type) and Percent(data.old_value) or data.old_value
		local text = attr_name.." : ".."<color=#".. COLORSTR.Yellow1 .. ">"..tostring(old_num).."</color>"
		UH.SetText(self.CValue,text)
		local change_value = data.attr_add - data.old_value
		local value = DataHelper.IsPercent(data.attr_type) and Percent(data.attr_add) or data.attr_add
		local change_num = DataHelper.IsPercent(data.attr_type) and Percent(change_value) or change_value
		text = value.."<color=#".. COLORSTR.Green4 .. ">".." (+"..tostring(change_num)..")".."</color>"
		UH.SetText(self.AValue,text)
	else
		local value = DataHelper.IsPercent(data.attr_type) and Percent(data.attr_add) or data.attr_add
		local text = attr_name.." : ".."<color=#".. COLORSTR.Yellow1 .. ">"..value.."</color>"
		UH.SetText(self.CValue,text)
	end
	local index = data.index % 2
	local bg_name = "GongXiShengJieDiTiao1"
	if index == 0 then
		bg_name = "GongXiShengJieDiTiao2"
	end
	UH.SpriteName(self.ImageSp,bg_name)
	self.AddObj:SetActive(not data.is_new)
	self.NewObj:SetActive(data.is_new)
end

