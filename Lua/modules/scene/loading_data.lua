LoadingData = LoadingData or BaseClass()

function LoadingData:__init()
	if LoadingData.Instance ~= nil then
		Debuger.LogError("[LoadingData] attempt to create singleton twice!")
		return
	end
    LoadingData.Instance = self
    
    self.view_data = SmartData.New({
        percent = 0,
        desc = "",
    })
end

function LoadingData:__delete()
	if LoadingData.Instance == self then
		LoadingData.Instance = nil
	end
end

function LoadingData:ViewData()
    return self.view_data
end

function LoadingData:SetViewData(percent,desc)
    self.view_data.percent = percent
    self.view_data.desc = desc
end

function LoadingData:Percent()
    return self.view_data.percent
end

function LoadingData:Desc()
    return self.view_data.desc
end