
FILE_REQ("modules/music_station/music_station_config")
FILE_REQ("modules/music_station/music_station_info")
FILE_REQ("modules/music_station/music_station_data")
VIEW_REQ("modules/music_station/music_station_view")
VIEW_REQ("modules/music_station/music_station_editor")

MusicStationCtrl = MusicStationCtrl or BaseClass(BaseCtrl)

function MusicStationCtrl:__init()
	if MusicStationCtrl.Instance ~= nil then
		Debuger.LogError("[MusicStationCtrl] attempt to create singleton twice!")
		return
	end	
	MusicStationCtrl.Instance = self
self.data = MusicStationData.New()
	self:RegisterAllProtocols()
end

function MusicStationCtrl:__delete()
	if MusicStationCtrl.Instance == self then
		MusicStationCtrl.Instance = nil
	end
	self:UnRegisterAllProtocols()
end

function MusicStationCtrl:RegisterAllProtocols()
end

function MusicStationCtrl:UnRegisterAllProtocols()
end


function MusicStationCtrl:MusicStation(id, is_test)
	if self.data:MusicStation(id, is_test) then
		ViewMgr:OpenView(MusicStationView)
	end
end