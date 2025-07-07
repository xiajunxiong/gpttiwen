
UIWBoard6 = UIWBoard6 or DeclareMono("UIWBoard6")
-- OtherTitle==bool
-- Deco1Obj==bool
-- Effect1Obj==bool
function UIWBoard6:UIWBoard6()
	self.title_str = ""
	self.view_type = nil
end

function UIWBoard6:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
	end
end

function UIWBoard6:SetData(view_type, title_str, v2)
	self.view_type = view_type
	self.Title.text = title_str or ""
	self.Title2.text = title_str or ""
	self.TitleObj:SetActive(title_str ~= nil)
	self.BG.sizeDelta = v2
	if self.OtherTitle then
		self.TitleObj:SetActive(false)
		self.TitleObj2:SetActive(true)
	end
	self.Deco1Obj:SetActive(true == self.Deco1)
	self.Effect1Obj:SetActive(true == self.Effect1)
end

function UIWBoard6:OnRelease()
	self.TitleObj:SetActive(true)
	self.TitleObj2:SetActive(false)
end
