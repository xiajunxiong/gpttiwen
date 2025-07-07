
FILE_REQ("modules/single_popup/single_popup_data")
VIEW_REQ("modules/single_popup/single_popup_view")

-- info = {
--     content = "xxxxxxxx",
--     btn_name = "xxxxxxxx",
--     func = function()  end,
-- }


SinglePopupCtrl = SinglePopupCtrl or BaseClass(BaseCtrl)

function SinglePopupCtrl:__init()
    if SinglePopupCtrl.Instance ~= nil then
		Debuger.LogError("[SinglePopupCtrl] attempt to create singleton twice!")
        
        return
    end
    SinglePopupCtrl.Instance = self

    self.data = SinglePopupData.New()
end

function SinglePopupCtrl:__delete()

end

function SinglePopupCtrl:DialogTips(info)
	self.data:AddInfo(info)
	ViewMgr:OpenView(SinglePopupView)
end