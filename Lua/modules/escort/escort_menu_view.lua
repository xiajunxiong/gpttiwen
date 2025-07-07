
EscortMenuView = EscortMenuView or DeclareMono("EscortMenuView", UIWFlushPanel)

function EscortMenuView:EscortMenuView()
	self.data = EscortCtrl.Instance:Data()
end

function EscortMenuView:OnEnable()
	local menu_show_info = self.data:GetMenuShowInfo()
	local escort_info = self.data:GetEscortAllInfo()
	self.Content.text = string.format(Language.Escort.EscortDesc, menu_show_info.type_name, menu_show_info.scene_name, menu_show_info.npc_name)
	self.TimerObj:SetActive(escort_info.end_time > 0)
	TimeHelper:CancelTimer(self.timer_tt)
	if escort_info.end_time > 0 then
		self.timer_tt = TimeHelper:AddCountDownCT(function ()
			local last_time = escort_info.end_time - TimeHelper.GetServerTime()
    		local ft = TimeHelper.FormatDHMS(last_time)
			self.Timer.text = string.format("%02d:%02d", ft.min, ft.s)
		end, function ()
			self.Timer.text = string.format("%02d:%02d", 0, 0)
			self.TimerObj:SetActive(escort_info.end_time > 0)
			EscortCtrl.Instance:SendEscortOp{
				op_type = EscortConfig.OperType.give_up
			}
		end, escort_info.end_time)
	end
end

function EscortMenuView:OnDisable()
	TimeHelper:CancelTimer(self.timer_tt)
end

function EscortMenuView:OnClickContinue()
	local escort_info = self.data:GetEscortAllInfo()
	local npc_seq = escort_info.npc_seq
	AutoInvokerCtrl.Instance:SetCommands({AutoCommandEscort.New(npc_seq)}, 1)
end

function EscortMenuView:OnClickGiveUp()
	ViewMgr:OpenView(EscortGvuView)
end