
WenXiangLouShoutView = WenXiangLouShoutView or DeclareView("WenXiangLouShoutView", WenXiangLouConfig.ResPath .. "task_wxl_shout")
VIEW_DECLARE_MASK(WenXiangLouShoutView, ViewMask.Block)

function WenXiangLouShoutView:WenXiangLouShoutView()
end

function WenXiangLouShoutView:LoadCallback()
	self.timer_dt = TimeHelper:AddDelayTimer(function ()
		ViewMgr:CloseView(WenXiangLouShoutView)
	end, WenXiangLouConfig.AUTO_SHOUT_TIME)
end

function WenXiangLouShoutView:CloseCallback()
	TimeHelper:CancelTimer(self.timer_dt)
end