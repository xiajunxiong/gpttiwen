
FILE_REQ("modules/music_game/music_game_config")
FILE_REQ("modules/music_game/music_game_info")
FILE_REQ("modules/music_game/music_game_data")
VIEW_REQ("modules/music_game/music_game_view")

MusicGameCtrl = MusicGameCtrl or BaseClass(BaseCtrl)

function MusicGameCtrl:__init()
	if MusicGameCtrl.Instance ~= nil then
		Debuger.LogError("[MusicGameCtrl] attempt to create singleton twice!")
		return
	end	
	MusicGameCtrl.Instance = self
	self.data = MusicGameData.New()
	self:RegisterAllProtocols()
end

function MusicGameCtrl:__delete()
	if MusicGameCtrl.Instance == self then
		MusicGameCtrl.Instance = nil
	end
	self:UnRegisterAllProtocols()
end

function MusicGameCtrl:RegisterAllProtocols()
end

function MusicGameCtrl:UnRegisterAllProtocols()
end


function MusicGameCtrl:MusicGame(id, is_test)
	if self.data:MusicGame(id, is_test) then
		ViewMgr:OpenView(MusicGameView)
	end
end