GatherData = GatherData or BaseClass()
function GatherData:__init()
    if GatherData.Instance ~= nil then
        Debuger.LogError("[GatherData] attempt to create singleton twice!")
        return
    end
    GatherData.Instance = self
    self.gather_info = {}
    self.need_time = SmartData.New({time = 0})
end

function GatherData:SetGatherInfo(data)
    self.gather_info.desc = data.desc or FightConfig.FightTabType.gather
    self.gather_info.time = data.time or 1
    self.gather_info.finish_func = data.finish_func
    self.gather_info.seq = data.seq or 0
    self.gather_info.obj_id = data.obj_id or 0
    self.gather_info.is_auto = data.is_auto or 0
    self.gather_info.is_client = data.is_client
end

function GatherData:GetGatherInfo()
    return self.gather_info
end

function GatherData:SetGatherTime(time)
    self.need_time.time = time
end

function GatherData:CacheReason(reasons)
    self.gather_reason = reasons
end

function GatherData:GetGatherConfig(type)
    return Config.gather_text_tip_auto.gather_text[type]
end

function GatherData.IsGathering()
    for k,v in pairs({"Gather","FaMu","WaKuang"}) do
		if MainPlayer and MainPlayer:IsInAnim(ChAnimType[v]) then
			return true
		end
    end
    return false
end