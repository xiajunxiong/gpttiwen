FILE_REQ("modules/lands_between/lb_dragon_pluse/lb_dragon_pluse_data")
VIEW_REQ("modules/lands_between/lb_dragon_pluse/lb_dragon_pluse_view")
VIEW_REQ("modules/lands_between/lb_dragon_pluse/lb_dragon_pluse_detail")
VIEW_REQ("modules/lands_between/lb_dragon_pluse/lb_dragon_pluse_inspire")
VIEW_REQ("modules/lands_between/lb_dragon_pluse/lb_dragon_pluse_reset")

LBDragonPluseCtrl = LBDragonPluseCtrl or BaseClass(BaseCtrl)
function LBDragonPluseCtrl:__init()
	if LBDragonPluseCtrl.Instance ~= nil then
		Debuger.LogError("[LBDragonPluseCtrl] attempt to create singleton twice!")
		return
	end
	LBDragonPluseCtrl.Instance = self

	self.data = LBDragonPluseData.New()
	self:RegisterAllProtocols()

	local param = {}
	param.act_type = 10055
	param.act_seq = 4
	param.listen_data = self.data.base_info
	param.remin_data = self.data.my_info
	param.remind_func = BindTool.Bind(LBDragonPluseData.GetActRemind, LBDragonPluseData.Instance)
	param.click_func = BindTool.Bind(self.ClickActIcon, self)
	param.time_show_func = nil -- BindTool.Bind(LBDragonPluseData.GetActCurTimeStamp, LBDragonPluseData.Instance)
	param.act_show_func = BindTool.Bind(LBDragonPluseData.GetActIconShow, LBDragonPluseData.Instance)
	param.act_status_func = nil -- BindTool.Bind(LBDragonPluseData.GetActIconStatusShowStatic, LBDragonPluseData.Instance)
	param.show_sp_sub = "dragon_vein_sub"
	
	LandsBetweenData.Instance:SetExtralActShowParam(param)
	

	FinishData.Instance:SetCloseFunc(BATTLE_MODE.DRAGON_VEIN_MONSTER,function(is_win)
		ViewMgr:OpenViewByKey(Mod.LandsBetween.Main)
		self:OpenDragonPluseView()
		-- ViewMgr:OpenView(LBDragonPluseView)
	end)

	FinishData.Instance:SetCloseFunc(BATTLE_MODE.DRAGON_VEIN_BOSS,function(is_win)
		ViewMgr:OpenViewByKey(Mod.LandsBetween.Main)
		self:OpenDragonPluseView()
		-- ViewMgr:OpenView(LBDragonPluseView)
	end)


end

function LBDragonPluseCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCGodSpaceDragonVeinInfo,"RecvGodSpaceDragonVeinInfo")
    self:RegisterProtocol(SCGodSpaceDragonVeinSelfInfo,"RecvGodSpaceDragonVeinSelfInfo")
    self:RegisterProtocol(SCGodSpaceFloatingSentence,"RecvGodSpaceFloatingSentence")
    self:RegisterProtocol(SCGodSpaceDragonVeinPos,"RecvGodSpaceDragonVeinPos")
end 

-- 5518 龙脉信息
function LBDragonPluseCtrl:RecvGodSpaceDragonVeinInfo(protocol)
	LogDG("--+++--林哥哥这是 龙脉信息?5518的下发哦",protocol)
	self.data:SetBaseInfo(protocol)
end

-- 5519 龙脉个人信息
function LBDragonPluseCtrl:RecvGodSpaceDragonVeinSelfInfo(protocol)
	LogDG("--+++--林哥哥这是 龙脉个人信息?5519的下发哦",protocol)
	self.data:SetMyInfo(protocol)
end

-- 5520 龙脉飘字
function LBDragonPluseCtrl:RecvGodSpaceFloatingSentence(protocol)
	LogDG("林哥哥这是 龙脉飘字?5520的下发哦",protocol)

	local str = ""
	local marquee_str = ""
	if protocol.type == 1 then 
		str = Language.LBDragonPluse.SimpleMaqueeNameLess -- string.format(Language.LBDragonPluse.SimpleMaquee,protocol.name)
		marquee_str = string.format(Language.LBDragonPluse.SimpleMaquee,protocol.name)
	else 
		local monster_cfg = Config.monster_group_auto.monster_group[protocol.p1] 
		str = string.format(Language.LBDragonPluse.BossMaqueeNameLess,monster_cfg.name)-- string.format(Language.LBDragonPluse.BossMaquee,protocol.name,monster_cfg.name)
		marquee_str = string.format(Language.LBDragonPluse.BossMaquee,protocol.name,monster_cfg.name)
	end 
	
	PublicPopupCtrl.Instance:Marquee(marquee_str)
	ChatCtrl.Instance:SendChannelChatText(str,nil, ChatConfig.ChannelType.landsbetween) -- ({msg_type = ChatConfig.MsgType.normal,content = str},ChatConfig.ChannelId.landsbetween)
end

-- 5521 龙脉位置信息
function LBDragonPluseCtrl:RecvGodSpaceDragonVeinPos(protocol)
	LogDG("林哥哥这是 龙脉位置信息?5521的下发哦",protocol)
	self.data:SetPostionInfo(protocol)
end

--  请求打开龙脉主界面
function LBDragonPluseCtrl:ReqDragonViewOpen()
	LandsBetweenCtrl.Instance:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_OPEN_DRAGON_VEIN_MAIN_UI,
	})
end 

--  请求进攻龙脉小怪
function LBDragonPluseCtrl:ReqDragonAttackMonster(param_t)
	LandsBetweenCtrl.Instance:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_DRAGON_VEIN_MONSTER_FIGHT,
		param_1 = param_t.index,
		param_2 = param_t.id_1 or 0,
		param_3 = param_t.id_2 or 0,
	})
end 

--  请求进攻龙脉boss
function LBDragonPluseCtrl:ReqDragonAttackBoss(param_t)
	LandsBetweenCtrl.Instance:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_DRAGON_VEIN_BOSS_FIGHT,
		param_1 = param_t.index,
	})
end 

--  请求分享龙脉boss
function LBDragonPluseCtrl:ReqDragonSharekBoss(param_t)
	LandsBetweenCtrl.Instance:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_DRAGON_VEIN_SHARE,
		param_1 = param_t.index or 0,
	})
end 

--  请求鼓舞
function LBDragonPluseCtrl:ReqDragonInspire(param_t)
	LandsBetweenCtrl.Instance:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_DRAGON_VEIN_INSPIRE,
		param_1 = param_t.type or 0,
	})
end 

--  请求重置龙脉boss
function LBDragonPluseCtrl:ReqResetMonster(param_t)
	LandsBetweenCtrl.Instance:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_RESET_HELPER_COND,
		index = param_t.index or 0,
	})
end 

--  请求龙脉呼喊
function LBDragonPluseCtrl:ReqCallHelper()
	LandsBetweenCtrl.Instance:GodSpaceOperate({
		oper_type = LANDS_BETWEEN_OPERATE_TYPE.OP_DRAGON_VEIN_CALL_HELPER,
	})
end 

function LBDragonPluseCtrl:OnInit()

end 

function LBDragonPluseCtrl:OpenDragonPluseView()
	self:ReqDragonViewOpen()
	ViewMgr:OpenView(LBDragonPluseView)
end

-- 点击活动图标
function LBDragonPluseCtrl:ClickActIcon()
	if self.data.base_info.status ~= 2 then 
		if self.data.base_info.status == 1 and self.data.base_info.anger_percent < 10000 then 
			PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.OpenFailByAngry)
	    else
			PublicPopupCtrl.Instance:Center(Language.LBDragonPluse.OpenFailByStatus)
		end 
		return 
	end 

	self.data:JumpToMyDragonVein()
	-- self:OpenDragonPluseView()
end

-- 点击活动地块
function LBDragonPluseCtrl:ClickActGrid()
	
end