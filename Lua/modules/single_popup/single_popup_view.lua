

SinglePopupView = SinglePopupView or DeclareView("SinglePopupView", "single_popup_view")

function SinglePopupView:LoadCallback()
    
end

function SinglePopupView:SinglePopupView()
    self.data = SinglePopupData.Instance.cache_info_list
    self.Content.text = self.data.content
    if self.data.btn_name then
        self.Name.text = self.data.btn_name
    else
        self.Name.text = Language.PublicPopup.ReplyType.confirm
    end
    self.func = self.data.func
end

function SinglePopupView:OnClick()
    if self.func ~= nil and type(self.func) == "function" then
        self.func()
    else
        self.func = function()
            ViewMgr:CloseView(SinglePopupView)
        end
        self.func()
    end
end