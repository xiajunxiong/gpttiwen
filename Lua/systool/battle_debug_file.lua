function StartNoise(profile_name)
    local ser_data = LuaMonoBehaviour.FindMono(DataHolder)
    local cam_tran = ser_data.CameraTran
    if cam_tran then
        local vir_cam_obj = cam_tran.gameObject
        if vir_cam_obj then
            local vir_cam = vir_cam_obj:GetComponent(typeof(Cinemachine.CinemachineVirtualCamera))
            if not profile_name or profile_name == "" then
                vir_cam:StartNoise("Default")
            else
                vir_cam:StartNoise(profile_name)
            end
        end
    end
end
