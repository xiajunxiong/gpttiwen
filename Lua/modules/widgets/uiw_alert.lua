------Title====UI.Text
------BG====UI.Rect

UIWAlert = UIWAlert or DeclareMono("UIWAlert")

function UIWAlert:UIWAlert()
	self.title_str = ""
	self.view_type = nil
	self.ok_func = nil
	self.cancel_func = nil
end

function UIWAlert:OnClickClose()
	if self.view_type ~= nil then
		ViewMgr:CloseView(self.view_type)
	end
end
function UIWAlert:OnClickCancel()
	if self.cancel_func then
		self.cancel_func()
	end
end
function UIWAlert:OnClickOk()
	if self.ok_func then
		self.ok_func()
	end
end
--拓展
-- param_t = {
-- 	show_ok = false,
-- 	show_cancel = false,
-- 	ok_func = nil,
-- 	cancel_func = nil,
-- 	v2_ok = nil,
-- 	v2_cancel = nil
--  show_close = nil
-- }
function UIWAlert:SetData(view_type, v2, param_t)
	self.view_type = view_type
	self.BG.sizeDelta = v2
	if param_t == nil then
		param_t = {
			show_cancel = false,
			show_ok = false,
			show_close = true,
		}
	end
	if param_t then
		self.ok_func = param_t.ok_func
		self.cancel_func = param_t.cancel_func
		self.BtnOk:SetActive(param_t.show_ok == true)
		self.BtnCancel:SetActive(param_t.show_cancel == true)
		self.BtnClose:SetActive(param_t.show_close == true)
		if param_t.v2_ok then
			UH.AnchoredPosition(self.BtnOkRect, param_t.v2_ok)
		end
		if param_t.v2_cancel then
			UH.AnchoredPosition(self.BtnCancelRect, param_t.v2_cancel)
		end
	end
end

function UIWAlert:OnRelease()
	self.view_type = nil
	self.ok_func = nil
	self.cancel_func = nil
end
