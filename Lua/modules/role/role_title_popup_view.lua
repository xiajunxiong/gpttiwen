
RoleTitlePopupView = RoleTitlePopupView or DeclareView("RoleTitlePopupView","main_other/title_popup",0,ViewLayer.Top - 1)
--FunctionBtnObj==============GameObject
VIEW_DECLARE_MASK(RoleTitlePopupView,ViewMask.BlockClose)

function RoleTitlePopupView:RoleTitlePopupView()
    if TitleData.Instance.popup_list[1] then
        self.data = TitleData.Instance.popup_list[1]
        self:FlushPopupView(self.data)
        table.remove(TitleData.Instance.popup_list,1)
    end
end

function RoleTitlePopupView:FlushPopupView(data)
    UH.SetText(self.Desc,data.desc)
    UH.SetText(self.Title,data.title)
    UH.SetText(self.BtnName,data.btn_name)
    self.FunctionBtnObj:SetActive(data.notice == 0)
    self.Desc.gameObject:SetLocalPosition(data.notice == 0 and Vector3.New(0,27.5,0) or Vector3.New(0,0,0))
    self.Inter.Interactable = data.notice == 0
    if self.auto_click ~= nil then
        TimeHelper:CancelTimer(self.auto_click)
    end
    if data.notice == 0 then
        self.auto_click = TimeHelper:AddDelayTimer(function ()
            self:OnClick()
        end, 10)
    end
end

function RoleTitlePopupView:OnClick()
    if self.data.notice == 0 then
        local config = AppearanceData.Instance:GetTitleConfig(self.data.title_id)
        if config == nil then
            LogError(Format("称号%s不存在",tostring(self.data.title_id)))
        end
        AppearanceCtrl.Instance:SendAddReq(1,config.seq)
        AppearanceData.Instance:ClearRedPoint()
    else
        ViewMgr:OpenViewByKey(Mod.Appearance.Title)
    end
    ViewMgr:CloseView(RoleTitlePopupView)
end

function RoleTitlePopupView:CloseCallback()
    if #TitleData.Instance.popup_list ~= 0 then
        ViewMgr:OpenView(RoleTitlePopupView)
    end
    if TitleData.close_popup_func then
        TitleData.close_popup_func()
        TitleData.close_popup_func = nil
    end
end

function RoleTitlePopupView:OnClickClose()
    if self.auto_click then
        TimeHelper:CancelTimer(self.auto_click)
    end
    ViewMgr:CloseView(RoleTitlePopupView)
end