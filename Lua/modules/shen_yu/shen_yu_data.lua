ShenYuData = ShenYuData or BaseClass()

function ShenYuData:__init()
    if ShenYuData.Instance ~= nil then
        Debuger.LogError("[ShenYuData] attempt to create singleton twice!")
        return
    end
    ShenYuData.Instance = self
end

function ShenYuData:ClearData()

end