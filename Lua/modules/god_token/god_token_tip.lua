GodTokenTipView = GodTokenTipView or DeclareView("GodTokenTipView","god_token/god_token_tip")
VIEW_DECLARE_MASK(GodTokenTipView,ViewMask.BgBlock)
function GodTokenTipView:GodTokenTipView()

end

function GodTokenTipView:LoadCallback(param_t)
	self.cancel_func = param_t.cancel_func
	self.ok_func = param_t.ok_func
	self.ContentText.text = param_t.content_text
	self.OkBtnText.text = param_t.ok_text or Language.PublicPopup.ReplyType.confirm
	self.CancelBtnText.text = param_t.cancel_text or Language.PublicPopup.ReplyType.cancel
end

function GodTokenTipView:CloseCallback()
	self.cancel_func = nil
	self.ok_func = nil
end

function GodTokenTipView:OnCancelClick()
	if self.cancel_func == nil then
		ViewMgr:CloseView(GodTokenTipView)
		return
	end
	self.cancel_func()
	ViewMgr:CloseView(GodTokenTipView)
end

function GodTokenTipView:OnOkClick()
	if self.ok_func == nil then
		ViewMgr:CloseView(GodTokenTipView)
		return
	end
	self.ok_func()
	ViewMgr:CloseView(GodTokenTipView)
end