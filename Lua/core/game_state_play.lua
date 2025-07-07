GameStatePlay = GameStatePlay or BaseClass(GameState)


function GameStatePlay:__init()
	self.module_manager = nil
	self.view_manager = nil
	self.res_manager = nil
	self.net_manager = nil
	self.scene_manager = nil
	self.graphic_quality_manager = nil

	--sys tools
	self.update_runner = nil
	-- self.step_runner = nil
	self.event_system = nil
	self.gameobject_pool = nil
	self.audio_manager = nil
end

function GameStatePlay:__delete()

end


function GameStatePlay:Update()
	 -- print("play::update")
	 self.view_manager:Update()
	 self.update_runner:Update()
end

function GameStatePlay:Start()
	GameState.Start(self)
	if not IS_IOS() then
		print("play::Start")
	end

	self:initTools()

	self.res_manager = ResourceManager.New()
	self.graphic_quality_manager = GraphicQualityManager.New()
	self.net_manager = NetManager.New()
	self.audio_manager = AudioManager.New()
	self.view_manager = ViewManager.New()
	self.scene_manager = SceneManager.New()
	self.module_manager = ModulesManager.New()
	self.module_manager:Run()
end

function GameStatePlay:Stop()
	self.view_manager:DeleteMe()
	self.view_manager = nil

	self.module_manager:DeleteMe()
	self.module_manager = nil

	self.audio_manager:DeleteMe()
	self.audio_manager = nil

	self.net_manager:DeleteMe()
	self.net_manager = nil

	self.res_manager:DeleteMe()
	self.res_manager = nil

	self.scene_manager:DeleteMe()
	self.scene_manager = nil

	self.graphic_quality_manager:DeleteMe()
	self.graphic_quality_manager = nil

	self:deleteTools()

	GameState.Stop(self)
end

function GameStatePlay:initTools()
	self.update_runner = Runner.New()
	-- self.step_runner = StepRunner.New()
	self.event_system  = EventSystem.New()
	self.gameobject_pool = GameObjectPool.New()
end

function GameStatePlay:deleteTools()
	self.gameobject_pool:DeleteMe()
	self.gameobject_pool = nil

	self.event_system:DeleteMe()
	self.event_system = nil

	-- self.step_runner:DeleteMe()
	-- self.step_runner = nil

	self.update_runner:DeleteMe()
	self.update_runner = nil

end

return GameStatePlay