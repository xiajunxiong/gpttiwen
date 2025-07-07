
EscortGvuView = EscortGvuView or DeclareView("EscortGvuView", EscortConfig.ResPath .. "escort_gvu")

VIEW_DECLARE_MASK(EscortGvuView,ViewMask.Block)

function EscortGvuView:EscortGvuView()
	self.data = EscortCtrl.Instance:Data()
end

function EscortGvuView:LoadCallback()
end

function EscortGvuView:OnClickGiveUp()
	EscortCtrl.Instance:SendEscortOp{
		op_type = EscortConfig.OperType.give_up
	}
	ViewMgr:CloseView(EscortGvuView)
end

function EscortGvuView:OnClickCancel()
	ViewMgr:CloseView(EscortGvuView)
end