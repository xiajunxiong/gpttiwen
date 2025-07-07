PowerSavingView = PowerSavingView or DeclareView("PowerSavingView", "main_other/power_saving",0, ViewLayer.IndepTop)

VIEW_DECLARE_MASK(PowerSavingView,ViewMask.None)
VIEW_DECLARE_LEVEL(PowerSavingView, ViewLevel.Lt)
-- local function get_main_camera()
--     local main_cam = UnityGameObj.FindGameObjectWithTag("MainCamera")
--     if main_cam ~= nil then
--         local main_cam_com = main_cam:GetComponent(typeof(UnityEngine.Camera))
--         return main_cam_com
--     end
--     return nil
-- end

local function get_ui_camera()
    local canvas = Game.UIManager.Singleton():GetComponent(typeof(UnityEngine.Canvas))
    if canvas ~= nil then
        return canvas.worldCamera
    end
    return nil
end
---ShowTxt=======Text
function PowerSavingView:LoadCallback()
    -- LogError("LAYER===",LayerMask,LayerMask.GetMask("UI"),LayerMask.GetMask("Ignore Raycast"))
    UH.SetText(self.ShowTxt,Language.Main.PowerSavingTips) 
    GpQualityMgr:shadowCamera().enabled = false
    local main_cam_com = SceneMgr:MainCamera()
    if main_cam_com ~= nil then
        main_cam_com.enabled = false
    end
    local ui_cam_com = get_ui_camera()
    if ui_cam_com ~= nil then
        ui_cam_com.cullingMask = LayerMask.GetMask("Ignore Raycast")
    end
end

function PowerSavingView:OnClicSelf()
    ViewMgr:CloseView(PowerSavingView)
end

function PowerSavingView:CloseCallback()
    GpQualityMgr:shadowCamera().enabled = true
    local main_cam_com = SceneMgr:MainCamera()
    if main_cam_com ~= nil then
        main_cam_com.enabled = true
    end
    local ui_cam_com = get_ui_camera()
    if ui_cam_com ~= nil then
        ui_cam_com.cullingMask = LayerMask.GetMask("UI")
    end
end

function PowerSavingView.EnterSavingPowerMode()
    if not RoleSettingData.Instance:IsShowPowerStick() then return end 
    ViewMgr:OpenView(PowerSavingView)
end

function PowerSavingView.IsInPowerSavingMode()
    return ViewMgr:IsOpen(PowerSavingView)
end


function PowerSavingView.ExitSavingPowerMode()
    ViewMgr:CloseView(PowerSavingView)
end

--不操作多少s会进入省电模式
function PowerSavingView.EnterJudgeTime()
    return 300
end