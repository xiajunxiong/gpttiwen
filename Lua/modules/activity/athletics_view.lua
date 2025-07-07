AthleticsView = AthleticsView or DeclareView("AthleticsView", ActivityConfig.ResPath .. "athletics",Mod.Athletics.Main)

VIEW_DECLARE_LEVEL(AthleticsView,ViewLevel.L1)

function AthleticsView:AthleticsView()
	BattlefieldCtrl.Instance:SendRoleInfoReq()
end

function AthleticsView:LoadCallback(param_t)
	param_t = param_t or {}
	self.Board:SetData(self:GetType(), Language.Activity.ActivityChallenge)
    self.Board:SetTabbarVisible(true)
	self.Board:SetRootName("board1")
	self.Currency:DefaultSet()
end