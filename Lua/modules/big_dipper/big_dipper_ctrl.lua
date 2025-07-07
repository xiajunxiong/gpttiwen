FILE_REQ("modules/big_dipper/big_dipper_data")
VIEW_REQ("modules/big_dipper/big_dipper_view")
VIEW_REQ("modules/big_dipper/big_dipper_rank")
VIEW_REQ("modules/big_dipper/big_dipper_atlas")
VIEW_REQ("modules/big_dipper/big_dipper_boss_info")
BigDipperCtrl = BigDipperCtrl or BaseClass(BaseCtrl)

function BigDipperCtrl:__init()
	if BigDipperCtrl.Instance ~= nil then
		Debuger.LogError("[BigDipperCtrl] attempt to create singleton twice!")
		return
	end	
	BigDipperCtrl.Instance = self
	self.data = BigDipperData.New()
	
	
	self:RegisterProtocol(SCBigDipperInfo, "RecvBigDipperInfo")
    self:RegisterProtocol(SCBigDipperMyInfo, "RecvBigDipperMyInfo")
    self:RegisterProtocol(CSBigDipperReq) 
	
	Cfg.Register("BigDipperStarManByNpcSeq",Cfg.RealTimeSearch(Config.big_dipper_auto.star_man_position,"npc_seq"))
end

function BigDipperCtrl:OnInit()
	ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Challenge.BigDipper,function()
		local num = self.data:GetShowBossTimes()
		return num,nil,self.data:CheckNextWeekTime()
    end)
	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_BIG_DIPPER, function(is_win)
		if self.is_open_view == true and FunOpen.Instance:GetFunIsOpened(Mod.Challenge.FallenGod) then
			ViewMgr:OpenView(FallenGodOpenView)
			self.is_open_view = false
		end
	end)
end

function BigDipperCtrl:__delete()
	self.data = nil
	BigDipperCtrl.Instance = nil
end

function BigDipperCtrl:RecvBigDipperInfo(protocol)
	self.data.boss_list:Set(protocol.boss_list)
	BigDipperCtrl.Instance:SendBigDipperReq(0)
end

function BigDipperCtrl:RecvBigDipperMyInfo(protocol)
	self.data.my_info:Set(protocol.my_info)	
	self.data.help_info:Set(protocol.help_info)	
	local count,total = self.data:GetActivtyShowBossData()
	ActivityData.Instance:SetActivityFinishInfo(ActType.BigDipper, count, total)

	local is_pass = BigDipperData.Instance:BossIsPassBySn(7)
	if self.is_init_info then
		if self.is_last_pass == false and is_pass then
			self.is_open_view = true
		end
	else
		self.is_init_info = true
	end
	self.is_last_pass = is_pass
end

function BigDipperCtrl:SendBigDipperReq(type,param1,param2)
	local protocol = GetProtocol(CSBigDipperReq)
	protocol.type = type
	protocol.param1 = param1 or 0
	protocol.param2 = param2 or 0
	SendProtocol(protocol)
end

function BigDipperCtrl:OnGetPersonRankListAck(protocol)
    self.data:SetBigDipperRankList(protocol)
end

function BigDipperCtrl:OnUnloadGameLogic()
    self.is_init_info = false
	self.is_last_pass = false
	self.is_open_view = false
end


