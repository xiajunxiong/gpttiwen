GameState = GameState or BaseClass()

GameStateState = {
	None = 0,
	Running = 1,
	Stoped = 2,
}

function GameState:__init() 
	self.state = GameStateState.None
end

function GameState:__delete()

end

function GameState:Start()
	self.state = GameStateState.Running
end

function GameState:Update()

end

function GameState:Stop()
	self.state = GameStateState.Stoped
end

function GameState:State()
	return self.state
end