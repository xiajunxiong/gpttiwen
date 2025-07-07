FILE_REQ("modules/main_view/main_view_data")

VIEW_REQ("modules/main_view/main_view")
VIEW_REQ("modules/main_view/main_function")
VIEW_REQ("modules/main_view/main_view_story")
VIEW_REQ("modules/main_view/main_view_icons")
VIEW_REQ("modules/main_view/top_layer_view")

VIEW_REQ("modules/main_view/main_right_sub")
VIEW_REQ("modules/main_view/main_top_sub")
VIEW_REQ("modules/main_view/main_down_sub")
VIEW_REQ("modules/main_view/main_top_left")
VIEW_REQ("modules/main_view/main_view_sub_mgr")

MainViewCtrl = MainViewCtrl or BaseClass(BaseCtrl)

function MainViewCtrl:__init()
	if MainViewCtrl.Instance ~= nil then
		Debuger.LogError("[MainViewCtrl] attempt to create singleton twice!")
		return
	end	
	MainViewCtrl.Instance = self
	self.data = MainViewData.New()
end

function MainViewCtrl:__delete()
	if MainViewCtrl.Instance == self then
		MainViewCtrl.Instance = nil
	end
end

function MainViewCtrl:OnInit()
	self.handle_login = LoginData.Instance:BaseData():KeysCare({"login_state"}, BindTool.Bind(self.OnLoginEvent, self))
end
function MainViewCtrl:OnLoginEvent()
	if LoginData.Instance:BaseData().login_state == LoginStateType.Logined then
		self.data:NotifyBuffs()
	end
end

function MainViewCtrl:SetGameState(state)
	self.data.cur_game_state.state = state
end

function MainViewCtrl:IsInGameState(state)
	return self.data.cur_game_state.state == state
end

function MainViewCtrl:OnUnloadGameLogic()
	self.data:RemoveBuffs()
	self.data:Clear()
	MainOtherData.Instance:MutualViewFunc()
end