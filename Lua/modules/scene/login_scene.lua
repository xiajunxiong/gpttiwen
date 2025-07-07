LoginScene = LoginScene or BaseClass(BaseScene)

function LoginScene:__init()
	Runner.Instance:AddRunObj(self, 3)
end

function LoginScene:__delete()
	Runner.Instance:RemoveRunObj(self)
end
function LoginScene:Update()
	self:updateTouch()
end
function LoginScene:updateTouch()
	-- local touch_up = UnityTouchState.IsTouchUp()
	local touch_down = UnityTouchState.IsTouchDown()
	local touch_ui = UnityTouchState.IsTouchOnUI()
	if not touch_down then
		return
	end
	if touch_ui then
		ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.CommonUI)
		return
	end
	if touch_down then
		ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.Scene)
		return
	end
end
function LoginScene:StartLoading()
	AudioMgr:PlayBg("2000")
	ViewMgr:OpenView(TopLayerView)	
	
	-- LogError("GetLoginBackType?",LoginData.Instance:GetLoginBackType())
	if LoginData.Instance:GetLoginBackType() == RELOGIN_TYPE.WELCOME then 
		ViewMgr:OpenView(WelcomeView)
	elseif LoginData.Instance:GetLoginBackType()  == RELOGIN_TYPE.ROLE_SELECT then 
		ViewMgr:OpenView(RoleSelectView)

		LoginData.Instance:SetLoginBackType(RELOGIN_TYPE.WELCOME) 
	end 
	if GLOBAL_CONFIG:PkgInfo().is_youyan or not IS_AUDIT_VERSION then
		ViewMgr:OpenView(GuideView)
	end
	PublicPopupCtrl.Instance:CloseActTips()
end

function LoginScene:LoadingComplete()
	return ViewMgr:IsOpen(WelcomeView) or ViewMgr:IsOpen(RoleSelectView)
end

function LoginScene:OnLoadingComplete()

end

function LoginScene:Dispose()
	BaseScene.Dispose(self)
	-- ViewMgr:CloseView(WelcomeView)
	ViewMgr:CloseView(RoleSelectView)

	if ViewMgr:IsOpen(WelcomeView) then
		if IS_AUDIT_VERSION or LoginCtrl.Instance.special_auto_create then
			ViewMgr:CloseView(WelcomeView)
		end
	end
end