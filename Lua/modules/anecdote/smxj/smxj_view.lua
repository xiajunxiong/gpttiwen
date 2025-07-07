SmxjView = SmxjView or DeclareView("SmxjView", "anecdote/smxj_view",Mod.Anecdote.StarLife)

VIEW_DECLARE_MASK(SmxjView,ViewMask.Block)

function SmxjView:SmxjView()

end

function SmxjView:LoadCallback()
	self.Effect:Play(10170020)
	self.time_handle_effect = TimeHelper:AddDelayTimer(BindTool.Bind(self.PlayBoWenEffect,self),6)
end

function SmxjView:PlayBoWenEffect()
	TimeHelper:CancelTimer(self.time_handle_effect)
	self.Effect:Play(10170021)
	self.time_handle_enterfb = TimeHelper:AddDelayTimer(BindTool.Bind(self.EnterSmxjFb,self),1)
end

function SmxjView:EnterSmxjFb()
	TimeHelper:CancelTimer(self.time_handle_enterfb)
	SceneCtrl.Instance:RequestEnterFb(FB_TYPE.SI_MING_XING_JUN)
	self.time_handle_closeview = TimeHelper:AddDelayTimer(BindTool.Bind(self.CloseSmxjView,self),1)
end

function SmxjView:CloseSmxjView()
	TimeHelper:CancelTimer(self.time_handle_closeview)
	ViewMgr:CloseView(SmxjView)
end

function SmxjView:CloseCallback()
	if self.time_handle_effect ~= nil then
        TimeHelper:CancelTimer(self.time_handle_effect)
        self.time_handle_effect = nil
    end
	
	if self.time_handle_enterfb ~= nil then
        TimeHelper:CancelTimer(self.time_handle_enterfb)
        self.time_handle_enterfb = nil
    end
	
	if self.time_handle_closeview ~= nil then
        TimeHelper:CancelTimer(self.time_handle_closeview)
        self.time_handle_closeview = nil
    end
end


