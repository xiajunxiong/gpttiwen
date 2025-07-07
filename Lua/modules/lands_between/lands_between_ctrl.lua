FILE_REQ("modules/lands_between/lands_between_config")
FILE_REQ("modules/lands_between/lands_between_data")
FILE_REQ("modules/lands_between/lands_between_map_data")
FILE_REQ("modules/lands_between/lb_map_list")
FILE_REQ("modules/lands_between/lands_between_oper_data")
FILE_REQ("modules/lands_between/lands_between_main_data")
FILE_REQ("modules/lands_between/lb_handbooks/lb_handbooks_ctrl")
FILE_REQ("modules/lands_between/lb_shop/lb_shop_ctrl")
FILE_REQ("modules/lands_between/lb_festival/lb_festival_ctrl")
VIEW_REQ("modules/lands_between/lands_between_board")
VIEW_REQ("modules/lands_between/lands_between_main")
VIEW_REQ("modules/lands_between/lands_between_map")
VIEW_REQ("modules/lands_between/lands_between_altar_info")
VIEW_REQ("modules/lands_between/lands_between_city")
VIEW_REQ("modules/lands_between/lands_between_city_macth")
VIEW_REQ("modules/lands_between/lands_between_produce")
VIEW_REQ("modules/lands_between/lands_between_meetgift")
VIEW_REQ("modules/lands_between/lands_between_moving")
VIEW_REQ("modules/lands_between/lands_between_poster")
VIEW_REQ("modules/lands_between/lands_between_report")
VIEW_REQ("modules/lands_between/lands_between_season")
VIEW_REQ("modules/lands_between/lands_between_map_model")
VIEW_REQ("modules/lands_between/lands_between_single_altar")
VIEW_REQ("modules/lands_between/lands_between_report_detail")
VIEW_REQ("modules/lands_between/lands_between_main_act")
VIEW_REQ("modules/lands_between/lands_between_map_sp_model")
VIEW_REQ("modules/lands_between/lands_between_explain")
-- GM命令
-- gs_restart_season:   重启赛季
-- gs_next_status:   跳转到下一个阶段
-- 神域
LandsBetweenCtrl = LandsBetweenCtrl or BaseClass(BaseCtrl)
function LandsBetweenCtrl:__init()
	if LandsBetweenCtrl.Instance ~= nil then
		Debuger.LogDG("[LandsBetweenCtrl] attempt to create singleton twice!")
		return
	end
	LandsBetweenCtrl.Instance = self

    self.data = LandsBetweenData.New()
    self:RegisterAllProtocols()

	-- self.complete_handle = EventSys:Bind(GlobalEventKeys.RoleNoticeComplete,
    -- BindTool.Bind(self.CheckWaitEnter, self))

	-- ActFormationData.Instance:RegistFunctions(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_GOD_SPACE, 
    -- {MainType = LandsBetweenCtrl.Instance, challenge_func = "LandsBetweenAttackChallenge", pet_on_limit_func = "PetOnLimitNum", assist_text_func = "AssistTextFunc"}
    -- )
end

function LandsBetweenCtrl:RegisterAllProtocols()
	self:RegisterProtocol(CSGodSpaceOperate) -- 5502

	self:RegisterProtocol(SCGodSpaceBaseInfo,"RecvGodSpaceBaseInfo")
	self:RegisterProtocol(SCGodSpaceUserInfo,"RecvGodSpaceUserInfo")
	self:RegisterProtocol(SCGodSpaceUserMoveToGrid,"RecvGodSpaceUserMoveToGrid")
	self:RegisterProtocol(SCGodSpaceGridInfoChanged,"RecvGodSpaceGridInfoChanged")
	self:RegisterProtocol(SCGodSpaceAltarPrayInfoChanged,"RecvGodSpaceAltarPrayInfoChanged")
	self:RegisterProtocol(SCGodSpaceAltarDefenderInfo,"RecvGodSpaceAltarDefenderInfo")
	self:RegisterProtocol(SCGodSpaceMapInfo,"RecvGodSpaceMapInfo")
	self:RegisterProtocol(SCGodSpaceOtherUsersInfo,"RecvGodSpaceOtherUsersInfo")
	self:RegisterProtocol(SCGodSpaceUserAssignFaction,"RecvGodSpaceUserAssignFaction")
	self:RegisterProtocol(SCGodSpaceUserEssemceBriefInfo,"RecvGodSpaceUserEssemceBriefInfo")
	self:RegisterProtocol(SCGodSpaceUserNewReportArrived,"RecvGodSpaceUserNewReportArrived")
	self:RegisterProtocol(SCGodSpaceUserReportList,"RecvGodSpaceUserReportList")
	self:RegisterProtocol(SCGodSpaceUserShowSeasonGiftUI,"RecvGodSpaceUserShowSeasonGiftUI")
	self:RegisterProtocol(SCGodSpaceUserFactionUserList,"RecvGodSpaceUserFactionUserList")
	self:RegisterProtocol(SCGodSpaceUserFactionBasicInfo,"RecvGodSpaceUserFactionBasicInfo")
	self:RegisterProtocol(SCGodSpaceUnfetchEssenceInfo,"RecvGodSpaceUnfetchEssenceInfo")
	self:RegisterProtocol(SCGodSpacePhaseTaskInfo,"RecvGodSpacePhaseTaskInfo")
	self:RegisterProtocol(SCGodSpaceUserCheckAccess,"RecvGodSpaceUserCheckAccess")
	self:RegisterProtocol(SCGodSpaceUserForcedCloseMainUI,"RecvGodSpaceUserForcedCloseMainUI")
	self:RegisterProtocol(SCGodSpaceApNotEnough,"RecvGodSpaceApNotEnough")
end

function LandsBetweenCtrl:OnInit()
	ActivityRandData.Instance:Register(Mod.LandsBetween.Main,function()
		local open_time = (LandsBetweenData.Instance.base_info.season_end_timestamp or 0 ) - TimeManager:GetServerTime()
		-- 临时版本，不是休赛期就必然显示
		local role_level = RoleData.Instance:GetRoleLevel()
		return (LandsBetweenData.Instance:GetCurStatus() < 6 or open_time > 0) and role_level >= Config.divine_domain_system_auto.other[1].level_limit-- self.data.base_info.is_open == 1 open_time <= 3*TIME_DAY_SECOND_NUM and
	end)

	FinishData.Instance:SetCloseFunc(BATTLE_MODE.GOD_SPACE_OCCUPY_ALTAR,function(is_win)
		LandsBetweenData.Instance:TryChangeMainInitType(false)
		-- ViewMgr:OpenViewByKey(Mod.LandsBetween.Main)
	end)

	-- self.data:SetExtralOperateFunc({
	-- 	act_seq = 1,
	-- 	name = "嘉泽来",
	-- 	func = function()
	-- 		PublicPopupCtrl.Instance:Center("嘉泽你快过来啊！")
	-- 	end
	-- })
end

-- OPERATE_TYPE
-- 5502 玩家请求神域操作
function LandsBetweenCtrl:GodSpaceOperate(param_t)
	LogDG("林哥哥！这是神域 "..(Language.LandsBetween.DescTestReq[param_t.oper_type] ~= nil and Language.LandsBetween.DescTestReq[param_t.oper_type] or "").." 请求哦！",param_t)
	local protocol = GetProtocol(CSGodSpaceOperate)
	protocol.oper_type = param_t.oper_type or 0
	protocol.param_1 = param_t.param_1 or 0
	protocol.param_2 = param_t.param_2 or 0
	protocol.param_3 = param_t.param_3 or 0
	protocol.param_4 = param_t.param_4 or 0
	SendProtocol(protocol)
end

-- 5501 神域基础信息（登录自动下发）
function LandsBetweenCtrl:RecvGodSpaceBaseInfo(protocol)
	LogDG("林哥哥！这是神域基础信息哦！5501",protocol)
	self.data:SetBaseInfo(protocol)
	EventSys:Fire(LandsBetweenEvent.BaseInfoChange)
end

-- 5503 玩家在神域中的信息
function LandsBetweenCtrl:RecvGodSpaceUserInfo(protocol)
	LogDG("林哥哥！这是神域玩家信息哦！5503",protocol)
	self.data:SetUserInfo(protocol)
end

-- 5506 角色移动到某格子上
function LandsBetweenCtrl:RecvGodSpaceUserMoveToGrid(protocol)
	LogDG("林哥哥！这是神域移动信息哦！5506",protocol)
	self.data:FlushRoleMovedInfo(protocol)
end

-- 5507 地图上格点信息变化通知
function LandsBetweenCtrl:RecvGodSpaceGridInfoChanged(protocol)
	LogDG("林哥哥！这是地图格点变化信息哦！5507",protocol)
	self.data:FlushMapGridInfo(protocol)
end

-- 5508 地图上神坛祈祷信息变化通知
function LandsBetweenCtrl:RecvGodSpaceAltarPrayInfoChanged(protocol)
	LogDG("林哥哥！这是地图神坛祷告变化信息哦！5508",protocol)
	self.data:FlushAltarPrayGridInfo(protocol)
end

-- 5509 神坛守军信息下发
function LandsBetweenCtrl:RecvGodSpaceAltarDefenderInfo(protocol)
	LogDG("林哥哥！这是神坛守军信息下发哦！5509",protocol)
	self.data:FlushAltarDefenderInfo(protocol)
end

-- 5504 神域地图信息-地图格
function LandsBetweenCtrl:RecvGodSpaceMapInfo(protocol)
	LogDG("林哥哥！这是地图格子信息下发哦！5504",protocol)
	self.data:SetAllMapInfo(protocol)
end

-- 5505 神域地图信息-角色
function LandsBetweenCtrl:RecvGodSpaceOtherUsersInfo(protocol)
	LogDG("林哥哥！这是地图角色信息下发哦！5505",protocol)
	self.data:SetAllUserInfo(protocol)
end

-- 5510 阵营分配协议
function LandsBetweenCtrl:RecvGodSpaceUserAssignFaction(protocol)
	LogDG("林哥哥！这是阵营分配信息下发哦！5510",protocol)
	self.data:SetUserAssignFaction(protocol)
end

function LandsBetweenCtrl:RecvGodSpaceUserEssemceBriefInfo(protocol)
	LogDG("林哥哥！这是精华产出下发哦！5511",protocol)
	self.data:SetUserEssemceBriefInfo(protocol)
end

function LandsBetweenCtrl:RecvGodSpaceUserNewReportArrived(protocol)
	LogDG("林哥哥！这是新战报的下发哦！5512",protocol)
	self.data:SetUserNewReportArrived(protocol)
end 

function LandsBetweenCtrl:RecvGodSpaceUserReportList(protocol)
	LogDG("林哥哥！这是新战报的下发哦！5513",protocol)
	self.data:SetUserReportList(protocol)
end 

function LandsBetweenCtrl:RecvGodSpaceUserShowSeasonGiftUI(protocol)
	LogDG("林哥哥！这是赛季见面礼的下发哦！5515",protocol)
	ViewMgr:OpenView(LandsBetweenMeetGiftView)
end 

function LandsBetweenCtrl:RecvGodSpaceUserFactionUserList(protocol)
	LogDG("林哥哥！这是阵营玩家信息的下发哦！5516",protocol)
	self.data:SetFactionUser(protocol)
end 

function LandsBetweenCtrl:RecvGodSpaceUserFactionBasicInfo(protocol)
	LogDG("林哥哥！这是阵营基础信息的下发哦！5517",protocol)
	self.data:SetFactionBaseInfo(protocol)
end 

function LandsBetweenCtrl:RecvGodSpaceUnfetchEssenceInfo(protocol)
	LogDG("林哥哥！这是神域未领取精华奖励信息的下发哦！5522",protocol)
	self.data:SetUnfetchEssenceInfo(protocol)
end

function LandsBetweenCtrl:RecvGodSpacePhaseTaskInfo(protocol)
	LogDG("~林哥哥！这是神域阶段任务信息下发的下发哦！5523",protocol)
	self.data:SetPhaseTaskInfo(protocol)
end

function LandsBetweenCtrl:RecvGodSpaceUserCheckAccess(protocol)
	LogDG("林哥哥！这是神域能够入场的信息下发的下发哦！5534",protocol)
	self.data:SetCanAccess(protocol)
end

function LandsBetweenCtrl:RecvGodSpaceUserForcedCloseMainUI(protocol)
	LogDG("林哥哥！这是神域强制关闭下发的下发哦！5532",protocol)
	ViewMgr:CloseView(LandsBetweenMainView)
end

function LandsBetweenCtrl:RecvGodSpaceApNotEnough(protocol)
	LogDG("林哥哥！这是神域行动力不足的下发哦！5541",protocol)	
	ViewMgr:OpenView(LandsBetweenMovingView)         
end

function LandsBetweenCtrl:OpenMainView()
	if not self.data:GetCanAccess() then 
		PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.AccessFailTip)
		return 
	end 

	if LoginData.Instance:BaseData().is_on_crosssever == false then 
        LoginCtrl.Instance:SendCrossStartReq()
	end

	ViewMgr:OpenView(LandsBetweenPosterView)

    -- if LoginData.Instance:BaseData().is_on_crosssever == false then 
    --     LoginCtrl.Instance:SendCrossStartReq()

    --     -- 未跨服的话启动时跨服后再发初始化协议
    --     self.auto_delay = TimeHelper:AddDelayTimer(function()
	-- 		ViewMgr:OpenView(LandsBetweenMainView)
    --     end,3)
	-- --	self.flag_wait_enter = true
    -- else 
	-- 	ViewMgr:OpenView(LandsBetweenMainView)
    -- end 

	if self.data.base_info.bigcross_available == 0 then 
		if IS_EDITOR then 
			PublicPopupCtrl.Instance:Center("林哥哥没开大跨服！")
		end 
	end 
end

function LandsBetweenCtrl:CheckWaitEnter()
	if not self.flag_wait_enter then 
		return 
	end 

	ViewMgr:OpenView(LandsBetweenMainView)
	self.flag_wait_enter = false
end

function LandsBetweenCtrl:TryOpenMain(param)
	if SceneData.Instance:GetMode() ~= SCENE_TYPE.COMMON_SCENE then 
		PublicPopupCtrl.Instance:Center(Language.LandsBetween.Desc.SceneLimit)
		return 
	end 

	local function openview()
		ViewMgr:CloseAllViewInBattle()
		
		if LoginData.Instance:BaseData().is_on_crosssever == false then 
			self.auto_delay = TimeHelper:AddDelayTimer(function()
				ViewMgr:OpenView(LandsBetweenMainView)

				if param~=nil and param.wait_view ~= nil then
					ViewMgr:OpenView(param.wait_view)
				end
			end,2)
			LoginCtrl.Instance:SendCrossStartReq()
		else
			ViewMgr:OpenView(LandsBetweenMainView)
			if param~=nil and param.wait_view ~= nil then
				ViewMgr:OpenView(param.wait_view)
			end
		end
	end

	if TeamData.Instance:InTeam() then
        local info = {
            content = Language.LandsBetween.Desc.InteamTips,
            confirm = {
                func = function()
                    TeamCtrl.Instance:SendExitTeam()
                    openview()
                    PublicPopupCtrl.Instance:CloseDialogTips()
                end
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end

	openview()
end

-- 购买行动力
function LandsBetweenCtrl:BuyAP(param)
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_BUY_AP,
		param_1 = param.is_gold and 1 or 0,
	})
end 

-- 开始刷新主界面
function LandsBetweenCtrl:MainViewUpdateStart()
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_OPEN_GOD_SPACE_MAIN_UI,
	})
end 

-- 停止刷新主界面
function LandsBetweenCtrl:MainViewUpdateEnd()
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_CLOSE_GOD_SPACE_MAIN_UI,
	})
end 

-- 手动获取全部战报
function LandsBetweenCtrl:GetAllReports()
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_ALL_REPORT_LIST,
	})
end 

-- 放弃神坛
function LandsBetweenCtrl:CancelAlter(param)
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_GIVEUP_ALTAR,
		param_1 = param.x,
		param_2 = param.y,
	})
end 

-- 领取见面礼
function LandsBetweenCtrl:CatchMeetGift()
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_FETCH_SEASON_GIFT,
	})
end 

-- 开始占领
function LandsBetweenCtrl:OperateOccupy(param)
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_OCCUPY_GRID,
		param_1 = param.param_1,
		param_2 = param.param_2,
		param_3 = param.param_3,
	})
end 

-- 获取防御者信息
function LandsBetweenCtrl:ReqDefenderInfo(param)
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_QUERY_ALTAR_DEFENDER_INFO,
		param_1 = param.param_1,
		param_2 = param.param_2,
	})
end 

-- 开始祈祷
function LandsBetweenCtrl:OperatePray(param)
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_PRAY,
		param_1 = param.param_1,
		param_2 = param.param_2,
		param_3 = param.param_3 or 0,
		param_4 = param.param_4 or 0,
	})
end 

-- 停止祈祷
function LandsBetweenCtrl:OperateStopPray(param)
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_STOP_PRAYING,
		param_1 = param.param_1,
		param_2 = param.param_2,
	})
end 

-- 获取阵营玩家信息
function LandsBetweenCtrl:OperateQueryFactionUser(param)
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_QUERY_FACTION_USERS,
		param_1 = param.reset,
	})
end 

-- 使用行动力物品
function LandsBetweenCtrl:UseApItem(param)
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_USE_AP_ITEM,
		param_1 = param.type,
	})
end

-- 读战报
function LandsBetweenCtrl:SendReadReport(param)
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_READ_REPORT,
		param_1 = param.time_stamp,
	})
end

-- 放弃神坛
function LandsBetweenCtrl:OperateCancelAltar(param)
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_GIVEUP_ALTAR,
		param_1 = param.param_1,
		param_2 = param.param_2,
	})
end 

--  查询未领取的精华奖励信息
function LandsBetweenCtrl:ReqCheckEssenceInfo()
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_UNFETCH_ESSENCE_INFO,
	})
end 

--  领取的精华奖励
function LandsBetweenCtrl:ReqFetchEssenceReward()
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_FETCH_ESSENCE_REWARD,
	})
end 

-- 删除战报
function LandsBetweenCtrl:ReqDeleteReports()
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_DELETE_ALL_REPORT,
	})
end 

--  领取阶段任务奖励 
function LandsBetweenCtrl:ReqFetchPhaseReward(param_t)
	self:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_FETCH_PHASE_REWARD,
		param_1 = param_t.index,
	})
end 

-- function LandsBetweenCtrl:LandsBetweenAttackChallenge()

--     ViewMgr:CloseView(ActFormationView)
-- 	ViewMgr:CloseView(LandsBetweenSingleAltarView)
-- 	ViewMgr:CloseView(LandsBetweenMainView)

-- end

-- function LandsBetweenCtrl:PetOnLimitNum()
--     return 4
-- end

-- --布阵界面助战信息文字
-- function DeepDarkFantasyCtrl:AssistTextFunc(assist_num)
--     return string.format(Language.CloudArena.FormationAssistInfo, assist_num)
-- end