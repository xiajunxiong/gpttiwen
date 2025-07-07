DeviceData = {}

function DeviceData.ScreenAspect()
    if IS_EDITOR or not DeviceData.screen_aspect then
        local main_camera = UnityGameObj.FindGameObjectWithTag("MainCamera")
        local main_camera_com = main_camera:GetComponent(typeof(UnityEngine.Camera))
        DeviceData.screen_aspect = main_camera_com.aspect
    end
    return DeviceData.screen_aspect
end
