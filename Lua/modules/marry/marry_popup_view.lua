--=========================MarryBoard=========================
MarryBoard = MarryBoard or DeclareMono("MarryBoard")

function MarryBoard:MarryBoard()
	self.title_str = ""
	self.view_type = nil
end

function MarryBoard:OnClickClose()
	if self.view_type ~= nil then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoClose)
		ViewMgr:CloseView(self.view_type)
	end
end

function MarryBoard:SetData(view_type, title_str, v2,is_special)
    UH.SetText(self.Title,title_str)
	self.BG.sizeDelta = v2 or Vector2.New(634, 440)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
	self.view_type = view_type
    self.Special:SetActive(is_special ~= nil)
end

function MarryBoard:ChangeTitle(title_str)
	UH.SetText(self.Title,title_str)
end

--=========================MarryPopupView=========================
MarryPopupView = MarryPopupView or DeclareView("MarryPopupView", "marry/marry_popup")

function MarryPopupView:MarryPopupView()
    self.Board:SetData(self:GetType(), Language.Marry.PopupViewTitle)
end

function MarryPopupView:LoadCallback(param_t)
    self.param_t = param_t
    UH.SetText(self.Content,param_t.content)
	self.Board:ChangeTitle(param_t.title_name or Language.Marry.PopupViewTitle)
    UH.SetText(self.CancelName,param_t.cancel_name or Language.Common.Cancel)
    UH.SetText(self.ConfirmName,param_t.confirm_name or Language.Common.Confirm)
	if param_t.cancel_time then
		self.LeftTime:SetCallBack(function()
			ViewMgr:CloseView(MarryPopupView)
			if self.param_t.cancel_func then
				Call(self.param_t.cancel_func)
			end
		end)
		self.LeftTime:TotalTime(param_t.cancel_time,TimeType.Type_Special_2,param_t.cancel_name)
	end
	if param_t.confirm_time then
		self.RightTime:SetCallBack(function()
			ViewMgr:CloseView(MarryPopupView)
			if self.param_t.confirm_func then
				Call(self.param_t.confirm_func)
			end
		end)
		self.RightTime:TotalTime(param_t.confirm_time,TimeType.Type_Special_2,param_t.confirm_name)
	end
end

function MarryPopupView:OnClickCancel()
    ViewMgr:CloseView(MarryPopupView)
    if self.param_t.cancel_func then
        Call(self.param_t.cancel_func)
    end
end

function MarryPopupView:OnClickConfirm()
    ViewMgr:CloseView(MarryPopupView)
	if self.param_t.confirm_func then
		Call(self.param_t.confirm_func)
	end
end

function MarryPopupView:OnClickClose()
    ViewMgr:CloseView(MarryPopupView)
end
--[[
ViewMgr:OpenView(MarryPopupView,{
	title_name = "协议离婚",--可不传
	cancel_name = "取消按钮名",--可不传
	confirm_name = "确认按钮名",--可不传
	content = "弹窗提示内容",
	cancel_func = function()--可不传 不需要自己关闭界面
		--取消事件
	end,
	confirm_func = function()--不需要自己关闭界面
		--确认事件
	end
})
]]