SinglePopupData = SinglePopupData or BaseClass()

function SinglePopupData:__init()
    if SinglePopupData.Instance ~= nil then
        Debuger.LogError("[SinglePopupData] attempt to create singleton twice===>AchieveData:__init")
        return 
    end
    SinglePopupData.Instance = self

    self.cache_info_list = {}
end

function SinglePopupData:__delete()

end

function SinglePopupData:AddInfo(info)
    if info then
        self.cache_info_list = info
    end
end