GmWindow = GmWindow or DeclareView("GmWindow", "gm_command/gm_window", nil, ViewLayer.IndepTop)
function GmWindow:GmWindow()
    self.Board:SetData(self:GetType(), "GM功能控制台")
    self.data = GmData.Instance
end
function GmWindow:CloseCallback()
    self.data.fast_click_num = 0
    self.data.fast_click_time = Time.realtimeSinceStartup
end
GmWindowPanel = GmWindowPanel or DeclareMono("GmWindowPanel", UIWFlushPanel)
function GmWindowPanel:GmWindowPanel()
    self.data = GmData.Instance
    self.data_cares = {
        {data = self.data.view_data, func = self.FlushButtonShow, init_call = true}
    }
end
function GmWindowPanel:FlushButtonShow()
    self.List:SetData(self.data.btn_data)
    self.Input1.text = self.data.move_speed
    self.Input2.text = self.data.stretching_speed
    self.Input3.text = self.data.fast_click_need
    self.Input4.text = self.data.fast_click_check
end
function GmWindowPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    self.data.move_speed = tonumber(self.Input1.text) > 0 and tonumber(self.Input1.text) or 2.5
    self.data.stretching_speed = tonumber(self.Input2.text) > 0 and tonumber(self.Input2.text) or 20
    self.data.fast_click_need = tonumber(self.Input3.text) >= 3 and tonumber(self.Input3.text) or 3
    self.data.fast_click_check = tonumber(self.Input4.text) >= 0.5 and tonumber(self.Input4.text) or 0.5
end
function GmWindowPanel:OnClickBtn(data)
    self[data.key](self)
end
function GmWindowPanel:hide_main()
    ViewMgr:MainViewVisible(self.data.view_data.hide_main)
    self.data.view_data.hide_main = not self.data.view_data.hide_main
end
function GmWindowPanel:hide_hp()
    self.data.view_data.hide_hp = not self.data.view_data.hide_hp
end
function GmWindowPanel:hide_popup()
    self.data.view_data.hide_popup = not self.data.view_data.hide_popup
end
function GmWindowPanel:free_move()
    self.data.view_data.free_move = not self.data.view_data.free_move
    if self.data.view_data.free_move == true then
        self.data:ClearFollow()
    else
        self.data:ResetCamera()
    end
end
function GmWindowPanel:stop_battle()
    if BattleSimulator.Instance:IsPlaying() then
        BattleSimulator.Instance:Quit()
    else
        if BattleData.Instance:BaseData().video_playing then
            BattleVideo.Instance:ReportQuit()
            BattleData.Instance:BaseData().video_playing = false
            return
        end
        BattleCtrl.Instance:ExitBattle()
    end
end
function GmWindowPanel:get_item()
    self.data.view_data.get_item = not self.data.view_data.get_item
end
function GmWindowPanel:lock_follow()
    self.data.view_data.lock_follow = not self.data.view_data.lock_follow
    self.data:ResetCamera()
end
GmButtonItem = GmButtonItem or DeclareMono("GmButtonItem", UIWidgetBaseItem)
function GmButtonItem:GmButtonItem()
    self.gm_data = GmData.Instance
end
function GmButtonItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local state = self.gm_data:GetBtnState(data)
    UH.SetText(self.Name, state == false and data.name or data.show_name)
    UH.SpriteName(self.ImageSp, state == false and "LvAnNiu_PuTong" or "HuangAnNiu_PuTong")
end