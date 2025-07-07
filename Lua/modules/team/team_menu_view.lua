
TeamMenuView = TeamMenuView or DeclareMono("TeamMenuView", UIWFlushPanelGroup)

function TeamMenuView:TeamMenuView()
	self.data = TeamCtrl.Instance:Data()

	self.data_cares = {
		{data = self.data:GetTeamInfo(), func = self.onFlush},
		{data = MainViewData.Instance.cur_game_state, func = self.onFlush},
		{data = LoginData.Instance:BaseData(),func = self.onFlush,keys = {"login_state"}},		--切换账号时主界面的组队显示没有刷新，因为登录的时候没有组队数据的玩家不会更新TeamInfo数据
		{data = self.data.heal_menu, func = self.FlushHealMenu, keys = {"type"}},
	}
end

function TeamMenuView:onFlush()
	if LoginData.Instance:LoginState() ~= LoginStateType.Logined then
		return
	end
	local in_team = self.data:InTeam()
	local is_crystal = self.data:IsInFBTeam()
	self.NoTeamObj:SetActive(not in_team and not is_crystal)
	self.InTeamObj:SetActive(in_team or is_crystal)
	if not is_crystal then self.data:ResetHealMenu() end

	-- self.healObj:SetActive( SceneData.Instance:GetMode() ~= SceneModeType.FightFantasy )
end

function TeamMenuView:OnClickNotice()
	local notice_time = self.data.notice_time.val
	if 0 == notice_time then
		self.data:NoticeTimer()
		TeamCtrl.Instance:SendTeamRecruit()
	else
		PublicPopupCtrl.Instance:Center(string.format(Language.Team.RecruitTips, notice_time))
	end
end

function TeamMenuView:OnClickLeave()
	local flag = GuildData.Instance:IsOpenGuildMelee()
	if flag == true then
		PublicPopupCtrl.Instance:Center(ErrorText[158])
		return
	end
	TeamCtrl.Instance:ExitTeam()
end

function TeamMenuView:OnClickLeaveT()
	TeamCtrl.Instance:SendTemporarilyPartReq()
end

function TeamMenuView:FlushHealMenu()
	self.HealMenu:SetActive(self.data.heal_menu.type > 0)
	for i = 1, 2 do
		self.HealTypes[i].isOn = i == self.data.heal_menu.type
	end
end

TeamMenuViewNoTeam = TeamMenuViewNoTeam or DeclareMono("TeamMenuViewNoTeam", UIWFlushPanel)
function TeamMenuViewNoTeam:TeamMenuViewNoTeam()
	self.data = TeamCtrl.Instance:Data()
	self.data_cares = {
		{data = self.data:GetSalaryList(), func = self.FlushShow, init_call = true},
	}
end

function TeamMenuViewNoTeam:FlushShow()
	local now_reward = 0
	local total_reward = 0
	for i, v in pairs(self.data:GetSalaryList()) do
		now_reward = now_reward + v.vo.times * v.info.reward
		total_reward = total_reward + v.info.week_limit * v.info.reward
	end
	UH.SetText(self.TxtSalary, Format(Language.Common.Slash, ColorStr(now_reward, COLORSTR.Green10), total_reward))
end

-- 创建简易队伍
function TeamMenuViewNoTeam:OnClickCreate()
	ViewMgr:OpenView(TeamView)
	TeamCtrl.Instance:SendCreateTeam()
end

-- 打开组队协议界面
function TeamMenuViewNoTeam:OnClickPlatform()
    ViewMgr:OpenView(TeamPlatformView)
end

function TeamMenuViewNoTeam:OnClickAutoMatch()
	ViewMgr:OpenView(TeamView)
	ViewMgr:OpenView(TeamGoalView)
end

function TeamMenuViewNoTeam:OpenSalaryView()
	ViewMgr:OpenView(TeamSalaryView)
end

TeamMenuViewInTeam = TeamMenuViewInTeam or DeclareMono("TeamMenuViewInTeam", UIWFlushPanel)

function TeamMenuViewInTeam:TeamMenuViewInTeam()
	self.data = TeamCtrl.Instance:Data()
	
	self.data_cares = {
		{data = self.data:GetTeamInfo(), func = self.onFlush},
		{data = self.data.flush_menu, func = self.onFlush, init_call = false},
		{data = MainViewData.Instance.cur_game_state, func = self.onFlush, init_call = false},
		{data = self.data.menu_arrow, func = self.FlushMenuArrow, init_call = false},
    }
end

function TeamMenuViewInTeam:onFlush()
	local in_team = self.data:InTeam()
	local is_leader = self.data:IsLeader()
	local is_crystal = self.data:IsInFBTeam()
	if (not in_team and not is_crystal) then return end
	if not in_team and is_crystal then self.data:CheckNoTeamPartnerOrder() end
	local member_list = self.data:GetMemberList()
	self.ShowList:SetData(member_list)
	-- self.BtnNoticeObj:SetActive(is_leader and not is_crystal)
	self.HealObj:SetActive(is_crystal)

	self.BtnLeaveT.text = self.data:IsFollow() and Language.Team.FollowState.Leave or Language.Team.FollowState.Follow
	local cur_mod = SceneData.Instance:GetMode()
	if cur_mod == SceneModeType.MazeRunWait or cur_mod == SceneModeType.Conspiracy or cur_mod == SceneModeType.HeroVanBattle then
		self.QuickBtnsObj:SetActive(false)
	else
		self.QuickBtnsObj:SetActive(not is_crystal)
		self.BtnNoticeObj:SetActive(is_leader)
		self.BtnLeaveTObj:SetActive(not is_leader)
	end
end

function TeamMenuViewInTeam:FlushMenuArrow()
	if 1 == self.data.menu_arrow.state and self.data.menu_arrow.arrow and self.data.menu_arrow.pos then
		-- self.GuidePos.position = Vector2.New(self.data.menu_arrow.pos.x, self.data.menu_arrow.pos.y, 0)
		self.GuidePos:SetObjActive(true)
		TimeHelper:AddDelayTimer(function ()
			self.data:MenuArrow(-1)
		end, 20)
	else
		self.GuidePos:SetObjActive(false)
	end
end

function TeamMenuViewInTeam:OnClickHeal(type)
	self.data.heal_menu.type = type
	self.data:MenuArrow(-1)
end

function TeamMenuViewInTeam:ClickHealReset()
	self.data:ResetHealMenu()
end

TeamMenuViewInTeamItem = DeclareMono("TeamMenuViewInTeamItem", UIWidgetBaseItem)

function TeamMenuViewInTeamItem:TeamMenuViewInTeamItem()
	self.team_data = TeamCtrl.Instance:Data()
	self.partner_data = PartnerCtrl.Instance:Data()
end


function TeamMenuViewInTeamItem:OnDestroy()
    self:uncareData()
end

function TeamMenuViewInTeamItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)

	self:uncareData()
	self.care_handles = {}
	self:careData(self.team_data.heal_menu, BindTool.Bind(self.FlushInfo, self), "object")
	self:careData(self.team_data.heal_menu, BindTool.Bind(self.FlushInfo, self), "type")
end

function TeamMenuViewInTeamItem:FlushInfo()
	local data = self.data
	local info = data.info
	local show_effect = not TeamData.Instance:InTeam()
	if nil == info then return end
	local sub_partner = info.sub_partner
	local heal_menu = self.team_data.heal_menu
	local pet_time = 1 == heal_menu.object
	local heal_time = sub_partner and sub_partner.id > 0 and heal_menu.type > 0 and 0 == info.is_follow and not data.is_leader
	local is_show = 0 == heal_menu.object or (info.pet_id and info.pet_id > 0)
	self.DetailObj:SetActive(is_show)
	self.NoPet:SetActive(not is_show)
	if not is_show then return end
	local member_type = Language.Team.MemberType
	local role_id = RoleData.Instance:GetBaseData().role_id
	local prof = 0
	local not_follow = info.is_partner ~= 1 and info.is_follow == 0 and not data.is_leader
	self.LeaderObj:SetActive(data.is_leader and not pet_time)
	self.FollowObj:SetActive(not_follow)
	if not_follow then
		self.FollowShow.text = 0 == info.is_online and Language.Team.FollowState.Outline or Language.Team.FollowState.Leave
		self.FollowSp.SpriteName = 0 == info.is_online and TeamConfig.TeamStateSp.outline or TeamConfig.TeamStateSp.leave
	end
	self.TypeObj:SetActive(not data.is_leader or pet_time)
	self.MemberType.text = pet_time and member_type.pet or ((heal_time or info.is_partner == 1) and member_type.partner or (role_id == info.uid) and member_type.myself or member_type.member)
	local hpp = pet_time and (info.pet_cur_hp / info.pet_max_hp) or (heal_time and sub_partner.hp_per or info.hp_per)
	local mpp = pet_time and (info.pet_cur_mp / info.pet_max_mp) or (heal_time and sub_partner.mp_per or info.mp_per)
	show_effect = (show_effect and self.pet_time == pet_time) and (hpp > self.HPP:GetProgress() or mpp > self.MPP:GetProgress())
	self.pet_time = pet_time
	self.HPP:SetProgress(hpp)
	self.MPP:SetProgress(mpp)
	local name = heal_time and sub_partner.name or info.name
	name = data.is_self and RichTextHelper.ColorStr(name, COLORSTR.Yellow16) or name
	local level = info.level

	AvatarEffect.RecieveQuaAnimObj(self.IconSp.gameObject)
	if pet_time then
		UH.SetIcon(self.IconSp, info.pet_icon_id)
		name = info.pet_name
		level = info.pet_level
	elseif heal_time then
		local leader_info = self.team_data:GetTeamInfo():LeaderInfo()
		level = leader_info and leader_info.level or 0
		prof = sub_partner.job
		UH.SetIcon(self.IconSp, sub_partner.icon_id, ICON_TYPE.ITEM)
	else
		if 0 == info.is_partner then
			if info.appearance.special_appearance_type == 1 then
				local co = PartnerData.Instance:GetPartnerInfoById(info.appearance.special_appearance_param)
				prof = co.info.job
				UH.SetIcon(self.IconSp, co.info.icon_id, ICON_TYPE.ITEM)
			else
				prof = RoleData.ProfIdToProfType(info.prof)

				UH.SetAvatar(self.IconSp, info.appearance.avatar_type,info.appearance.avatar_id,info.appearance.avatar_quality)
			end
		else
			local leader_info = self.team_data:GetTeamInfo():LeaderInfo()
			-- level = leader_info and leader_info.level or 0
			level = info.level
			prof = info.partner_info.job
			UH.SetIcon(self.IconSp, info.icon_id, ICON_TYPE.ITEM)
		end
	end
	self.Name.text = name
	self.Level.text = level
	self.ProfI.SpriteName = RoleData.GetProfSp(prof)
	if 0 == self.team_data.menu_arrow.state and info.hp_per < 0.3 then
		self.team_data:MenuArrow(1)
		TimeHelper:AddRunFrameTimer(function ()
			local offset = self.ItemPosition:TransformVector(Vector3.New(290, -30, 0))
			self.team_data:MenuArrow(1, Vector2.New(self.ItemPosition.position.x + offset.x, self.ItemPosition.position.y + offset.y))
		end)
	end
	if data:ShowEffect(0 == heal_menu.object, 1 == heal_menu.object) or show_effect then
		if self.EffectTool and self.EffectTool.isActiveAndEnabled then 
			self.EffectTool:Play(10010866)
		end 
	end
end

function TeamMenuViewInTeamItem:OnClickItem()
	self.team_data:MenuArrow(-1)
	local data_info = self.data.info
	local in_team = self.team_data:InTeam()
	local is_leader = self.team_data:IsLeader()
	local is_follow = self.team_data:IsFollow()
	local funcs, config = TeamConfig.TeamFunctions2

	local role_id = RoleData.Instance:GetBaseData().role_id
	local level = RoleData.Instance:GetBaseData().level

	local function friendfunc(t)
		local config = DataHelper.TableCopy(t)
		if data_info.uid > TeamConfig.RobotUid then
			local is_friend = SocietyData.Instance:IsFriend(data_info.uid)
			table.insert(config, 3, is_friend and "DeleteFriend" or "AddFriend")
		else
			table.remove(config, 1)
			table.remove(config, 1)
		end
		return config
	end
	local is_maze_run = (SceneData.Instance:GetMode() == SceneModeType.MazeRunWait)
	local is_guild_melee = (SceneData.Instance:GetMode() == SceneModeType.GuildMelee)
	local is_fantasy = (SceneData.Instance:GetMode() == SceneModeType.FightFantasy)
	local is_van = (SceneData.Instance:GetMode() == SceneModeType.HeroVanBattle)
	if in_team then
		if role_id ~= data_info.uid then
			if is_leader then
				if data_info.is_partner == 0 then
					config = friendfunc(funcs[1])
					if data_info.uid > TeamConfig.RobotUid and not is_maze_run then	--迷宫中不能换队长和招回
						if data_info.is_follow == 1 then
							table.insert(config, 3, "GiveLeader")
						else
							table.insert(config, 3, "TeamSummon")
						end
					end
				else
					config = funcs[3]
				end
			elseif data_info.is_partner == 0 then
				config = friendfunc(funcs[2])
			elseif data_info.is_partner == 1 then
				config = {"UseHeal"}
			end
		else
			config = {"ExitTeam", "UseHeal"}
			if not is_leader then
				table.insert(config, is_follow and "TempLeaveTeam" or "BackTeam")
				table.insert(config, "TeamLedApply")
			end
		end
	elseif data_info.is_partner == 1 then
		config = funcs[3]
	else
		config = {"UseHeal"}
	end
	if is_guild_melee then
		for i=#config,1,-1 do
			local fn = config[i]
			if fn == "UseHeal" or fn == "TempLeaveTeam" then
				table.remove(config,i)
			end
		end
	elseif is_maze_run then
		for i=#config,1,-1 do
			local fn = config[i]
			if fn == "UseHeal" or fn == "TempLeaveTeam" or 
				fn == "TeamLedApply" or fn == "KickOut" then
				table.remove(config,i)
			end
		end
	elseif is_fantasy then
		for i=#config,1,-1 do
			local fn = config[i]
			if fn == "UseHeal" then
				table.remove(config,i)
			end
		end
	elseif is_van then 
		for i=#config,1,-1 do
			local fn = config[i]
			if fn == "TempLeaveTeam" or fn == "KickOut" or fn == "ExitTeam" then
				table.remove(config,i)
			end
		end
	end

	if nil ~= config and #config ~= 0 then
		RoleInfoCtrl.Instance:RoleInfoView{
			info = function ()
				if data_info.is_partner == 1 then
					local partner_info = self.partner_data:GetPartnerInfoById(data_info.uid).info
					return RoleInfoData.RoleInfoNew{func_info = data_info, name = partner_info.name, level = level, prof = partner_info.job}
				else
					return RoleInfoData.RoleInfoNew{func_info = data_info, uid = data_info.uid, name = data_info.name, level = data_info.level, prof = data_info.prof, top_level = data_info.top_level}
				end
			end,
			funcs = function()
				return config
			end,
			hide_base = true,
			pos = self.ItemPosition.position
		}
	end
end