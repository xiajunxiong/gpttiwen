
TeamOperInfoView = TeamOperInfoView or DeclareView("TeamOperInfoView", "item_info/team_oper_info")
VIEW_DECLARE_MASK(TeamOperInfoView, ViewMask.BlockClose)

function TeamOperInfoView:TeamOperInfoView()
	self.data = TeamCtrl.Instance:Data()
end

function TeamOperInfoView:LoadCallback()
	local in_team = self.data:InTeam()
	local is_leader = self.data:IsLeader()
	local team_functions, config = TeamConfig.TeamFunctions1
	local info = self.data.team_oper_info.info

	local function friendfunc(t)
		local config = DataHelper.TableCopy(t)
		if info.uid > TeamConfig.RobotUid then
			local is_friend = SocietyData.Instance:IsFriend(info.uid)
			table.insert(config, 3, is_friend and "DeleteFriend" or "AddFriend")
		else
			table.remove(config, 1)
			table.remove(config, 1)
		end
		return config
	end

	if is_leader then
		if info.is_partner == 0 then
			config = friendfunc(team_functions[1])
			if info.uid > TeamConfig.RobotUid  then
				if info.is_follow == 1 then
					table.insert(config, 3, "GiveLeader")
				else
					table.insert(config, 3, "TeamSummon")
				end
			end
			self.Functions:SetData(config)
		else
			self.Functions:SetData(team_functions[3])
		end
	elseif not in_team then
		if info.is_partner == 1 then
			self.Functions:SetData(team_functions[3])
		end
	else
		config = friendfunc(team_functions[2])
		self.Functions:SetData(config)
	end
	self.InfoPosition.position = self.data.info_position
end

function TeamOperInfoView:OnClickBlank()
    ViewMgr:CloseView(TeamOperInfoView)
end

TeamOperInfoViewFuncItem = DeclareMono("TeamOperInfoViewFuncItem", UIWidgetBaseItem)

function TeamOperInfoViewFuncItem:TeamOperInfoViewFuncItem()
	self.team_data = TeamCtrl.Instance:Data()
	self.info_data = RoleInfoCtrl.Instance:Data()
	self.info = self.team_data.team_oper_info.info
	self.language = Language.Team.TeamOperInfoFuncs
end

function TeamOperInfoViewFuncItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Name.text = self.language[self.data]
end

function TeamOperInfoViewFuncItem:OnClick()
	self[self.data](self)
end

function TeamOperInfoViewFuncItem:ViewEquip()
	self.info_data.view_equip_uid = self.info.uid
	RoleInfoCtrl.Instance:SendReferRoleEquipment(self.info.uid)
	-- ViewMgr:OpenView(RoleInfoEquipView)
	ViewMgr:CloseView(TeamOperInfoView)
end

function TeamOperInfoViewFuncItem:Chat()
	-- Debuger.LogError("Chat With " .. self.info.name)
	ViewMgr:CloseView(TeamOperInfoView)
end

function TeamOperInfoViewFuncItem:GiveLeader()
	TeamCtrl.Instance:SendChangeTeamLeader(self.info.member_index)
	ViewMgr:CloseView(TeamOperInfoView)
end

function TeamOperInfoViewFuncItem:TeamSummon()
	TeamCtrl.Instance:SendTeamLeaderSummonMember(self.info.uid)
	ViewMgr:CloseView(TeamOperInfoView)
end

function TeamOperInfoViewFuncItem:KickOut()
	TeamCtrl.Instance:SendKickOutOfTeamReq(self.info.member_index)
	ViewMgr:CloseView(TeamOperInfoView)
end

function TeamOperInfoViewFuncItem:PartnerRest()
	PartnerCtrl.Instance:SendPartnerFight(self.info.uid)
	ViewMgr:CloseView(TeamOperInfoView)
end

-- 加为好友
function TeamOperInfoViewFuncItem:AddFriend()
	SocietyCtrl.Instance:SendFriendAdd(self.info.uid)
	ViewMgr:CloseView(TeamOperInfoView)
end

-- 删除好友
function TeamOperInfoViewFuncItem:DeleteFriend()
	local info = {
        content = string.format(Language.Society.FriendFuncTips.delete_friend, self.info.name),
        confirm = {
            func = function()
				SocietyCtrl.Instance:SendFriendDelete(self.info.uid)
				ViewMgr:CloseView(TeamOperInfoView)
                ViewMgr:CloseView(DialogTipsView)
            end
		}
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

-- 调整站位
function TeamOperInfoViewFuncItem:TeamSort()
	ViewMgr:OpenView(FormationView)
	ViewMgr:CloseView(TeamOperInfoView)
end

-- 使用药水
function TeamOperInfoViewFuncItem:UseHeal()
	ViewMgr:OpenView(BagView, {col = ItemColumnType.Supply})
	ViewMgr:CloseView(TeamOperInfoView)
end