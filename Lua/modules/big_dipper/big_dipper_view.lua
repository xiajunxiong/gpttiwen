BigDipperView = BigDipperView or DeclareView("BigDipperView", "big_dipper/big_dipper_view",Mod.Challenge.BigDipper)
VIEW_DECLARE_MASK(BigDipperView, ViewMask.Block)
VIEW_DECLARE_LEVEL(BigDipperView, ViewLevel.L0)
function BigDipperView:BigDipperView()
	self.AudioOpen = CommonAudio.ViewOpenL1
    self.AudioClose = CommonAudio.ViewCloseL1
end

function BigDipperView:LoadCallback()
end

function BigDipperView:OnBackClick()
	ViewMgr:CloseView(BigDipperView)
end

BigDipperPanel = BigDipperPanel or DeclareMono("BigDipperPanel", UIWFlushPanel)
function BigDipperPanel:BigDipperPanel()
	self.data = BigDipperData.Instance
	self.select_boss_sn = 0
	self.select_npc_seq = 0
	self.select_index = 0
	self.select_lock =  false
	self.data_cares = {
		{data = self.data.boss_list, func = self.FlushShowPanel, init_call = true},
		{data = self.data.my_info, func = self.FlushMyInfo, init_call = false}
    }
end 

function BigDipperPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
end

-- 提示
function BigDipperPanel:FlushMyInfo()
	self:FlushBossInfo()
	self:FlushAllInfo(self.select_index)
end

-- 提示
function BigDipperPanel:OnClickTips()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[153].desc})
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewOpenL2)
end

function BigDipperPanel:OnClickTeam()
	if not self.select_lock  then
		AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
		ViewMgr:CloseView(BigDipperView)
		local targetid = 201 + self.select_boss_sn
		local target = TeamData.Instance.team_goal_list[targetid]
		if RoleData.Instance:GetRoleLevel() < target.low_level then
			PublicPopupCtrl.Instance:Center(Language.Team.MatchLevelError)
			return
		end
		if TeamData.Instance:InTeam() then
			ViewMgr:OpenView(TeamView)
			TeamCtrl.Instance:SendChangeTeamLimit(target.low_level, target.high_level, target.target_id)
			TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
		else
			TeamData.Instance.cur_platform_target_id = target.target_id
			TeamCtrl.Instance:SendTeamMatch(target.target_id, target.is_cross, TeamConfig.MatchType.start)
		end
	else
		PublicPopupCtrl.Instance:Error(Language.BigDipper.CantChallenge)
	end
end

function BigDipperPanel:OnClickFirstReward()
	BigDipperCtrl.Instance:SendBigDipperReq(2,self.select_boss_sn)
end

function BigDipperPanel:OnClickGo()
	if not self.select_lock  then
		AudioMgr:PlayEffect(AudioType.Scene, "enter_door")
		ViewMgr:CloseView(BigDipperView)
		--BigDipperCtrl.Instance:SendBigDipperReq(1,self.select_boss_sn)
		SceneLogic.Instance:AutoToNpc(self.select_npc_seq,nil,nil,true)
	else
		PublicPopupCtrl.Instance:Error(Language.BigDipper.CantChallenge)
	end
end

function BigDipperPanel:OnClickGray()
	local str = Language.BigDipper.CantChallenge1
	if self.select_index == 8 then
		str = Language.BigDipper.CantChallenge2
	end
	PublicPopupCtrl.Instance:Error(str)
end

function BigDipperPanel:FlushShowPanel()
	self:FlushBossInfo()
	local open_index = BigDipperData.Instance:GetOpenIndex()
	if open_index then
		BigDipperData.Instance:SetOpenIndex(nil)
		self.ShowBossToggle[open_index].isOn = true
		self:FlushAllInfo(open_index)
	else
		local week_num = ActivityData.Instance:GetWeekNum()
		self.ShowBossToggle[week_num].isOn = true
		self:FlushAllInfo(week_num)
	end
end 

function BigDipperPanel:FlushBossInfo()
	local show_list = self.data:GetShowBossList()
	local num = table.nums(show_list)
	for i = 1, num do
        local vo = show_list[i]
		local info = self.data.my_info[vo.sn + 1]
		local is_have_red = false
		if info.first_pass_flag == 1 and info.first_reward_flag == 0 then
			is_have_red = true
		end	
		vo.is_have_red = is_have_red
        self.ShowBossList[i]:SetData(vo)
    end 
	local week_num = ActivityData.Instance:GetWeekNum()
	for i = 1, 6 do
		if week_num > i then
			self.Lines[i]:SetActive(true)
		else
			self.Lines[i]:SetActive(false)
		end
	end
end 

function BigDipperPanel:FlushAllInfo(index)
	local show_list = self.data:GetShowBossList()
	local vo = show_list[index]
	if vo then
		self.select_boss_sn = vo.sn
		self.select_npc_seq = vo.npc_seq
		self.select_index = index
		self:FreshBossModel(vo)	
		self:FreshBossInfo(vo)
		self:FreshBossReward(vo)
		self:FreshBottonShow(vo)
		self:FreshBossHelpInfo(vo)
	end
end

function BigDipperPanel:OnBossItemClick(index)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.MenuOpen)
	self:FlushAllInfo(index)
end

function BigDipperPanel:OnClickHelp()
	self.HelpShow:SetActive(true)
end

function BigDipperPanel:OnShowBlock()
	self.HelpShow:SetActive(false)
end

function BigDipperPanel:FreshBossHelpInfo(vo)
	local big_dipper_config = {}
	for k,v in pairs(Config.big_dipper_auto.star_man_position) do 
		if v.npc_seq == vo.npc_seq then
			big_dipper_config = v
			break
		end
	end
	local str = vo.help_times .."/"..big_dipper_config.help_times
	UH.SetText(self.HelpTimes,str)
	self.HelpReward:SetData(big_dipper_config.help_reward)
	
end

function BigDipperPanel:FreshBottonShow(vo)
	self.select_lock = vo.is_lock
	self.ButtonInteractorYellow.Interactable = not vo.is_lock
	self.ButtonInteractorGreen.Interactable = not vo.is_lock
end

function BigDipperPanel:FreshBossModel(vo)
	local npc_config = Cfg.NpcById(Cfg.NpcBySeq(vo.npc_seq).id)
	local path = DrawerHelper.GetNpcPath(npc_config.res_id)
	if not self.ui_obj_current then
		self.ui_obj_current = UIChDrawer.New()
		self.ui_obj_current:SetMainSolo(path)
		self.ModelShowCurrent:SetShowData({ui_obj = self.ui_obj_current, view = self})
	else
		 self.ui_obj_current:SetMainSolo(path)
	end
end

function BigDipperPanel:FreshBossInfo(vo)
	local npc_config = Cfg.NpcById(Cfg.NpcBySeq(vo.npc_seq).id)
	UH.SetText(self.BossName,npc_config.name)
	local big_dipper_config = {}
	for k,v in pairs(Config.big_dipper_auto.star_man_position) do 
		if v.npc_seq == vo.npc_seq then
			big_dipper_config = v
			break
		end
	end
	UH.SetText(self.BossDec,big_dipper_config.monster_des)
	UH.SetText(self.HelpText,Language.BigDipper.HelpTitle[big_dipper_config.sn])
	self.Button:SetActive(not vo.is_lock)
	self.ButtonUnOpen:SetActive(vo.is_lock)
	UH.SetText(self.UnOpenText,Language.BigDipper.OpenTime[vo.open_time])
end

function BigDipperPanel:FreshBossReward(vo)
	local info = self.data.my_info[vo.sn + 1]
	local is_have_red = false
	local is_pass = false
	if info.first_pass_flag == 1 and info.first_reward_flag == 0 then
		is_have_red = true
	end	
	if info.first_reward_flag == 1 then
		is_pass = true
	end
	self.FirstRewardBlock:SetActive(is_have_red)
	local first_rewards = {}
	local victory_rewards = {}
	local co ={}
	local big_dipper_config = {}
	for k,v in pairs(Config.big_dipper_auto.star_man_position) do 
		if v.npc_seq == vo.npc_seq then
			big_dipper_config = v
			break
		end
	end
	
	for k,v in pairs(big_dipper_config.item_list_first) do 	
		local list = {}
		list.item_id = v.item_id
		list.num = v.num
		list.is_bind = v.is_bind
		list.is_pass = is_pass
		list.is_have_red = is_have_red
		list.index = k
		table.insert(first_rewards,list)
	end

	local is_victory_pass = false
	local pass_flag = bit:d2b(info.pass_flag)
	local pass_reward_flag = bit:d2b(info.pass_reward_flag)
	if vo.type == 0 then
		if pass_flag[32] == 1 then
			is_victory_pass = true
		end
		for k,v in pairs(big_dipper_config.item_list_1) do 	
			local list = {}
			list.item_id = v.item_id
			list.num = v.num
			list.is_bind = v.is_bind
			list.is_pass = is_victory_pass
			list.is_have_red = false
			list.index = k
			table.insert(victory_rewards,list)
		end	
	else
		for k,v in pairs(big_dipper_config.item_list_1) do 
			table.insert(victory_rewards,v)
		end	
		for k,v in pairs(big_dipper_config.item_list_2) do 
			table.insert(victory_rewards,v)
		end	
		for k,v in pairs(big_dipper_config.item_list_3) do 
			table.insert(victory_rewards,v)
		end	
		
		for k,v in pairs(victory_rewards) do 
			v.is_have_red = false
			v.index = k
			v.is_pass = false
			if pass_flag[33-k] == 1 then
				v.is_pass = true
			end
		end	
	end
	self.data.first_reward_count = #big_dipper_config.item_list_first	
	self.FirstReward:SetData(first_rewards)
	self.VictoryReward:SetData(victory_rewards)
end

function BigDipperPanel:OnClickRank()
	 ViewMgr:OpenView(BigDipperRankView)
end

function BigDipperPanel:OnClickAtlas()
	 ViewMgr:OpenView(BigDipperAtlasView)
end

BigDipperRewardListItem = BigDipperRewardListItem or DeclareMono("BigDipperRewardListItem", UIWidgetBaseItem)
function BigDipperRewardListItem:BigDipperRewardListItem()

end

function BigDipperRewardListItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	local item_data = Item.New()
	item_data.item_id = data.item_id
	item_data.num = data.num
	item_data.is_bind = data.is_bind
	self.Cell:SetData(item_data)
	self.RedPoint:SetNum(0)
	if data.is_have_red and data.index == BigDipperData.Instance.first_reward_count then
		self.RedPoint:SetNum(1)
	end
	self.HasGet:SetActive(data.is_pass)
	self.Cell:MakeGrey(data.is_pass)
end

BigDipperShowCell = BigDipperShowCell or DeclareMono("BigDipperShowCell", UIWidgetBaseItem)
function BigDipperShowCell:BigDipperShowCell()

end 

function BigDipperShowCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
	self.RedPoint:SetNum(0)
	if data.type == 0 then
		UH.SetText(self.Name,Language.BigDipper.Title[data.sn])
		local week_num = ActivityData.Instance:GetWeekNum()
		local sprite_name = "XingXing1"
		if week_num == data.open_time then
			sprite_name = "XingXing2"
		end
		if week_num < data.open_time then
			sprite_name = "XingXing3"
		end
		UH.SpriteName(self.ImageSprite,sprite_name)
	else
		self.PassObj:SetActive(data.is_pass)
		local npc_config = Cfg.NpcById(Cfg.NpcBySeq(data.npc_seq).id)
		UH.SetText(self.Name,npc_config.name)
		UH.SetIcon(self.Icon,npc_config.npc_icon)
		if data.is_have_red then
			self.RedPoint:SetNum(1)
		end	
		if self.SuoObj then
			self.SuoObj:SetActive(data.is_lock)
			self.Interactor1.Interactable = not data.is_lock
			self.Interactor2.Interactable = not data.is_lock
			self.Interactor3.Interactable = not data.is_lock
		end
	end
end 



