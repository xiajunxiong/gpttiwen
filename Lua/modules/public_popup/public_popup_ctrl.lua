FILE_REQ("modules/public_popup/floating_text_data")
FILE_REQ("modules/public_popup/top_layer_marquee_data")
FILE_REQ("modules/public_popup/top_layer_act_tips_data")
FILE_REQ("modules/public_popup/dialog_tips_data")
FILE_REQ("modules/public_popup/public_popup_data")
FILE_REQ("modules/public_popup/attr_reason_data")
FILE_REQ("modules/public_popup/attr_reason_config")

VIEW_REQ("modules/public_popup/tips_popup_data")
VIEW_REQ("modules/public_popup/dialog_tips_view")
VIEW_REQ("modules/public_popup/dialog_tips_back_view")
VIEW_REQ("modules/public_popup/tips_popup_view")
VIEW_REQ("modules/public_popup/wait_view")
VIEW_REQ("modules/public_popup/reward2_view")
VIEW_REQ("modules/public_popup/battle_cinematics_view")
VIEW_REQ("modules/public_popup/public_warning_view")
VIEW_REQ("modules/public_popup/auto_research_tip")
VIEW_REQ("modules/public_popup/attr_reason_view")
VIEW_REQ("modules/public_popup/skin_sign_item")
VIEW_REQ("modules/public_popup/condition_tip")
VIEW_REQ("modules/public_popup/popup_life_msg")



FILE_REQ("config/errorcode")

PublicPopupCtrl = PublicPopupCtrl or BaseClass(BaseCtrl)

function PublicPopupCtrl:__init()
	if PublicPopupCtrl.Instance ~= nil then
		Debuger.LogError("[PublicPopupCtrl] attempt to create singleton twice!")
		return
	end
	PublicPopupCtrl.Instance = self

	self:RegisterProtocol(SCNoticeNum, "RecvNoticeNum", true)
	self:RegisterProtocol(SCSystemMsg, "RecvSystemMsg", true)
	self:RegisterProtocol(SCSpecialNotice, "OnSpecialNotice", true)
	self:RegisterProtocol(SCAttrListReason, "OnAttrListReason", true)
	self:RegisterProtocol(SCForbidReason, "OnSCForbidReason")
	self:RegisterProtocol(CSAttrListReasonReq)

	self.floating_text_data = FloatingTextData.New()
	self.dialog_tips_data = DialogTipsData.New()
	self.tips_popup_data = TipsPopupData.New()
	self.top_layer_marquee_data = TopLayerMarqueeData.New()
	self.top_layer_act_tips_data = TopLayerActTipsData.New()
	self.attr_reason_data = AttrReasonData.New()

	self.data = PublicPopupData.New()
	self.stop = false
	self.stop_act = false
	self.is_shield = false --屏蔽飘字
	self.shield_data = {}

	-- 拦截跑马灯消息的方法
	self.intercept_rolling_msg_func = {}
end

function PublicPopupCtrl:OnUnloadGameLogic()
    self.is_shield = false
    self.shield_data = {}
end

function PublicPopupCtrl:__delete()
	self:UnRegisterAllProtocols()
end

function PublicPopupCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCAttrListReason)
end

local ignore_num = {
	[100027] = true
}
function PublicPopupCtrl:RecvNoticeNum(protocol)
	if not ignore_num[protocol.result] then
		self:Center(ErrorInfo[protocol.result] or "ErrorCode No Generate " .. protocol.result)
	end
end

function PublicPopupCtrl:RecvSystemMsg(protocol)
	if self.stop == true then
		return 
	end
	local role_level = RoleData.Instance:GetRoleLevel()  
	if protocol.min_level > 0 and role_level < protocol.min_level then 
		return 
	end 

	if protocol.max_level > 0 and role_level > protocol.max_level then 
		return 
	end 
	-- if role_level < protocol.min_level or role_level > protocol.max_level then 
	-- 	return
	-- end

	
	if not StringIsEmpty(protocol.spid_id) then
		local channel_id = LoginData.Instance:AccountData().spid
		if protocol.spid_id ~= channel_id then
			return
		end
	end

	if 1 == bit:_rshift(protocol.content_type, SYS_MSG_TYPE.SYS_MSG_FLOAT) % 2 then
		self:Center(RichTextHelper.ParseAll(protocol.msg, false, RTParseType.TopLayerFloating))
	end
	if 1 == bit:_rshift(protocol.content_type, SYS_MSG_TYPE.SYS_MSG_ROLLING_NOTICE) % 2 then
		-- 检测是否需要拦截跑马灯消息
		if not self:CheckRollingMsg(protocol) then
			self:Marquee(RichTextHelper.ParseAll(protocol.msg, false, RTParseType.TopLayerMarquee), self.top_layer_marquee_data:NeedDelay(protocol.msg))
		end
	end
	if 1 == bit:_rshift(protocol.content_type, SYS_MSG_TYPE.SYS_MSG_SYSTEM_CHAT) % 2 then
		-- 检测是否需要拦截跑马灯消息 -- 一般来讲，有跑马灯消息就有系统消息
		if not self:CheckRollingMsg(protocol) then
			ChatCtrl.Instance:ChannelChatCustom{content = protocol.msg}
		end
	end
	if 1 == bit:_rshift(protocol.content_type, SYS_MSG_TYPE.SYS_MSG_WORLD_CHAT) % 2 then
		ChatCtrl.Instance:ChannelChatCustom({content = protocol.msg}, ChatConfig.ChannelId.world)
	end
	if 1 == bit:_rshift(protocol.content_type, SYS_MSG_TYPE.SYS_MSG_PROF_CHAT) % 2 then
		ChatCtrl.Instance:ChannelChatCustom({content = protocol.msg, block_role = true}, ChatConfig.ChannelId.prof)
	end
end

function PublicPopupCtrl:OnSpecialNotice(protocol)
	if SPECIAL_NOTICE_TYPE.team_salary == protocol.op_type then
		ChatCtrl.Instance:ChannelChatCustom{team_salary = protocol.param1}
	end
	if SPECIAL_NOTICE_TYPE.guild_gather_dream_gift == protocol.op_type then
		GuildCtrl.Instance:ShowJiYuanRewardList(protocol)
	end
end

-- 检查跑马灯信息是否需要拦截
function PublicPopupCtrl:CheckRollingMsg(protocol)
	local result = false
	for i,v in pairs(self.intercept_rolling_msg_func) do
		if v(protocol) == true then
			result = true
		end
	end
	return result
end
--================================外部接口==============================
-- item_info {item_id = 0,num = 0}
function PublicPopupCtrl:Center(str)
    if self.stop == true then return end
    if self.is_shield then
		table.insert(self.shield_data,{info=str,type="Center"})
		return
    end
    if self.floating_text_data.floating_frame ~= Time.frameCount then
        self.floating_text_data.floating_data.center = {}
        self.floating_text_data.floating_frame = Time.frameCount
    end
    table.insert(self.floating_text_data.floating_data.center, str)
end

--会播放错误提示音
--只播放调用AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
function PublicPopupCtrl:Error(str)
	if self.stop == true then return end
	if self.floating_text_data.floating_frame ~= Time.frameCount then
		self.floating_text_data.floating_data.center = {}
		self.floating_text_data.floating_frame = Time.frameCount
	end
	table.insert(self.floating_text_data.floating_data.center, str)
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickError)
end

function PublicPopupCtrl:CenterI(item_info)
    if self.stop == true then return end
    if self.is_shield then
        table.insert(self.shield_data, {info=TableCopy(item_info),type="CenterI"})
		return
    end
    if self.floating_text_data.floating_frame ~= Time.frameCount then
        self.floating_text_data.floating_data.center = {}
        self.floating_text_data.floating_frame = Time.frameCount
    end
    local data = PublicPopupCtrl.CenterItem(item_info)
    if data then
        table.insert(self.floating_text_data.floating_data.center, data)
    end
end

function PublicPopupCtrl:CenterV(virtual)
	if self.stop == true then return end
	if self.floating_text_data.floating_frame ~= Time.frameCount then
		self.floating_text_data.floating_data.center = {}
		self.floating_text_data.floating_frame = Time.frameCount
	end
	local use_rich = false
	--获得铜币和元宝播放
	if virtual.type ~= nil then
		if virtual.type == CurrencyType.Gold then
			AudioMgr:PlayEffect(AudioType.UI,"get_diamond")
			use_rich = true
		elseif virtual.type == CurrencyType.ImmortalCoin then
			AudioMgr:PlayEffect(AudioType.UI,"get_diamond")
			use_rich = true
		elseif virtual.type == CurrencyType.CoinBind then
			AudioMgr:PlayEffect(AudioType.UI,"get_coin")
			use_rich = true
		end
	end
	if use_rich then
		local config = Item.GetConfig(virtual.item_id or HuoBi[virtual.type])
		local desc = string.format("%s%s |",string.format(virtual.content or Language.PublicPopup.VirtualTip, QualityColorStr[config.color], virtual.num), "")
		PublicPopupCtrl.CenterRich(desc,{sprite = tostring(virtual.item_id or HuoBi[virtual.type]), w = 40, h = 40})
	else
		table.insert(self.floating_text_data.floating_data.center, PublicPopupCtrl.CenterVirtual(virtual))
	end
end

function PublicPopupCtrl.CenterItem(t)
    local config = Item.GetConfig(t.item_id)
    if config == nil then
        BuglyReport("PublicPopupCtrl.CenterItem Error ", t and t.item_id or "t = nil")
        return nil
    end
    local item = t.item
    -- 主界面飘字及物品图标要用UIWItemCell，因为一些碎片物品要有贴膜逻辑，不能只是一个icon所以要吧item_id传进去
    -- 下面的CenterVirtual同理
    return {
        item_id = t.item_id, 
		icon_id = t.icon_id or config.icon_id,
        desc = string.format(t.content or Language.PublicPopup.ItemTip, t.color_str or QualityColorStr[config.color], item and item:QuaName() or config.name, t.num),
        fly_icon = t.fly_icon == nil and true or t.fly_icon
    }
end

-- 使用例子：PublicPopupCtrl.CenterRich("获得了100经验|,(世界等级)|",{sprite = "65534", w = 40, h = 40},{sprite = "65534", w = 40, h = 40})
-- str 中的 "|" 代表图片位置
function PublicPopupCtrl.CenterRich(str, ... )
	str = str or ""
	local char_list = RichTextHelper.ParseUTF8Str(str)
	local img_list = {...}
	local desc = ""
	local rich_desc = ""

	local space_func = function (num)
		local space_str = ""
		for i = 1,num do
			space_str = space_str .. " "
		end
		return space_str
	end

	local img_index = 1
	local is_ok = true
	local rich_desc_clip = ""
	for i,v in ipairs(char_list) do

		if v == "|" and img_list[img_index] then
			local img_info = img_list[img_index]
			desc = desc .. space_func(math.ceil(img_info.w / 5))
		else
			desc = desc .. v
		end

		if v == "<" then
			is_ok = false
		end
		if is_ok then
			if v == "|" and img_list[img_index] then
				if rich_desc_clip ~= "" then
					rich_desc = rich_desc .. RichTextHelper.ColorStr(rich_desc_clip, COLORSTR.Transparent)
				end
				rich_desc_clip = ""
				local img_info = img_list[img_index]
				rich_desc = rich_desc .. RichTextHelper.SpriteStrAtlas(img_info.sprite, img_info.w, img_info.h)
				img_index = img_index + 1
			else
				rich_desc_clip = rich_desc_clip .. v
			end
		end
		if v == ">" then
			is_ok = true
		end

	end

	if rich_desc_clip ~= "" then
		rich_desc = rich_desc .. RichTextHelper.ColorStr(rich_desc_clip, COLORSTR.Transparent)
	end

	local t = {is_rich = true, desc = desc, rich_desc = rich_desc}
	PublicPopupCtrl.Instance:Center(t)
end

function PublicPopupCtrl.CenterVirtual(t)
	local config = Item.GetConfig(t.item_id or HuoBi[t.type])
	if config == nil then
		LogError("没有这个物品" .. tostring(t.item_id or HuoBi[t.type]))
	end
	return {
		item_id = t.item_id or HuoBi[t.type],
		icon_id2 = t.icon_id or config.icon_id,
		is_virtual = true,
		desc = string.format(t.content or Language.PublicPopup.VirtualTip, QualityColorStr[config.color], t.num)
	}
end

function PublicPopupCtrl:Marquee(str, delay_time)
	self.top_layer_marquee_data:AddCaptionInfo(str, delay_time)
end

function PublicPopupCtrl:ActTips(act_info, time_s)
	if self.stop_act == true then return end
	if RoleData.Instance:GetRoleLevel() < 25 or not act_info:IsRemind() then
		return
	end
	if act_info.info.is_push_show == 0 then
		return
	end
	if act_info.info.level_restrict and RoleData.Instance:GetRoleLevel() < act_info.info.level_restrict then
		return
	end
	self.top_layer_act_tips_data:ResetShowActTime(time_s)
	self.top_layer_act_tips_data:CurShowAct(act_info)
	ActivityRemindData.Instance:SetRemindList(act_info,time_s)
end

function PublicPopupCtrl:CloseActTips()
	self.top_layer_act_tips_data:CurShowAct{}
end

-- 公共简单提示框(一条描述加2个按钮这种)

-- info = {
-- 	content = "展示文字",
-- 	cancel = {
-- 		name = "取消按钮名字,nil = 取消",
-- 		func = click_handle(取消按钮回调,结合FlushDialogTips/CloseDialogTips组合多层逻辑)
-- 		wait_time = 强制等待时间（秒）打开后按钮上会显示倒计时，倒计时结束前点击按钮无效（默认无，按需传入）
-- 	},
-- 	confirm = {
-- 		name = "确定按钮名字,nil = 确定",
-- 		func = click_handle(确定按钮回调,结合FlushDialogTips/CloseDialogTips组合多层逻辑)
-- 		wait_time = 强制等待时间（秒）打开后按钮上会显示倒计时，倒计时结束前点击按钮无效（默认无，按需传入）
-- 	},
-- 	no_cancel = bool or false
-- 	no_confirm = bool or false
--	tnr = DTTodayNotRemind or nil
--	trn_str = "不再提示文本，nil = 今日不再提醒"
-- 	close = bool or false
-- bg_v2 = Vector2.New(446,280),
-- today_v2 = Vector2.New(-70,-16.3),
-- btn_v2 = Vector2.New(0,-81.3),
-- content_v2 = Vector2.New(0, 43),
-- change_btn_pos = true or false(是否切换2个按钮的位置，true则切换即黄色按钮（BtnConfirm）在右，false或不传为不切换)
-- }

function PublicPopupCtrl:DialogTips(info)
	if nil ~= info.new_tnr then
		if DialogTipsData.Instance:TodayIsTrue(info.new_tnr) and info.confirm then
			local confirm_func = info.confirm.func
			if confirm_func and type(confirm_func) == "function" then
				confirm_func()
				return
			end
		end
		if DialogTipsData.Instance:TodayIsTrue(info.new_tnr) and info.content_func then
			info.content_func()
			return
		end
	elseif nil ~= info.tnr and self.dialog_tips_data.today_not_reminds[info.tnr] then
		if info and info.confirm then
			local confirm_func = info.confirm.func
			if confirm_func and type(confirm_func) == "function" then
				confirm_func()
			end
		end
		if info.content_func then
			info.content_func()
			return
		end
		return
	end
	if ViewMgr:IsOpen(DialogTipsView) then
		self.dialog_tips_data:SetCurInfo(info)
		ViewMgr:FlushView(DialogTipsView)
	else
		self.dialog_tips_data:AddInfo(info)
		ViewMgr:OpenView(DialogTipsView)
	end
end

function PublicPopupCtrl:FlushDialogTips(info)
	self.dialog_tips_data:SetCurInfo(info)
	ViewMgr:FlushView(DialogTipsView)
end

function PublicPopupCtrl:CloseDialogTips()
	ViewMgr:CloseView(DialogTipsView)
end

function PublicPopupCtrl:OpenTipsPopup(data,mask)
	self.tips_popup_data:SetData(data)
	if mask then
		VIEW_DECLARE_MASK(TipsPopupView,ViewMask.BlockClose)
	else
		VIEW_DECLARE_MASK(TipsPopupView,ViewMask.BgBlockClose)
	end
	ViewMgr:OpenView(TipsPopupView,data)
end

function PublicPopupCtrl:ShowWait(text_show)
	-- LogError("ShowWait",text_show)
	self.data:Base().wait_str = text_show
	ViewMgr:OpenView(WaitView)
end

function PublicPopupCtrl:HideWait()
	ViewMgr:CloseView(WaitView)
end

--玩法说明 规则说明 help_tip是游戏说明id 则读取对应的配置说明
function PublicPopupCtrl:HelpTip(help_tip)
	if tonumber(help_tip) then
		if Config.language_cfg_auto.textdesc[help_tip] == nil then
			LogError(Format("游戏说明 language_cfg_auto 文字语言表 seq = %s 配置不存在",help_tip))
		else
			help_tip = Config.language_cfg_auto.textdesc[help_tip].desc
		end
	end
	self:OpenTipsPopup{tips_str = help_tip}
end

--一般简单弹窗 content 提示文字 func 确定事件 param货币类型或tnr 今日不再提醒
function PublicPopupCtrl:AlertTip(content,func,param)
	if type(param) == [[string]] then
		self:DialogTips({content = content,content_func = func,tnr = param})
	else
		self:DialogTips({content = content,content_func = func,remain_type = param})
	end
end

---奖励展示弹窗
---items======Item对象的数组 ，必须要有item_id和num
---hint_text==弹框中显示的提示文字

---扩展，可以在items中加入并不存在的物品，以virtual_type区分，
---以icon_id-图标，name-名字,qua_frame-边框，virtual_type虚拟类型 作为内容
function PublicPopupCtrl:Reward2(items,hint_text,close_func,title_name)
	self.data:SetReward2Data(items,hint_text,close_func,title_name)
	ViewMgr:OpenView(Reward2View)
end
function PublicPopupCtrl:CloseReward2()
	ViewMgr:CloseView(Reward2View)
end
-- show_type = 1则物品数量显示图标右下角，is_tip 是否播报物品获取飘字提示
function PublicPopupCtrl:RewardExpand2(items,show_type,is_tip)
	self.data:SetReward2Data(items)
	ViewMgr:OpenView(Reward2View,{show_type = show_type, is_tip = is_tip})
end

---战斗过场界面
---view_type===BattleCinematics.Type 界面上显示文字 ，不显示，开始拼图，开始答题等
---show_time===float 界面展示时间 [0 or nil]=由外部手动关闭， （0，n]=固定时长关闭
---opened_func===function 过场界面完全遮挡背后时的回调
---close_func====function 过场界面播放结束时的回调
---close_func_time==float close_func执行的时间，从动态完全展示完成开始计时，nil表示界面关闭时回调
function PublicPopupCtrl:BattleCinematics(view_type,show_time,opened_func,close_func,close_func_time)
	local data = self.data:BattleCinematicsData()
	data.view_type = view_type
	data.show_time = show_time
	data.opened_func = opened_func
	data.close_func = close_func
	data.close_func_time = close_func_time
	ViewMgr:OpenView(BattleCinematics)
end

---关闭战斗过场界面
function PublicPopupCtrl:CloseBattleCinematics()
	ViewMgr:CloseView(BattleCinematics)
end

--打开警告界面
-- param_t = {desc="",warning=""} param = {pos = v3, height = 250}
function PublicPopupCtrl:OpenWarningView(act_type, param_t, param)
	if act_type then
		self.data:SetWarningData(Language.PublicPopup.WarningDesc[act_type])
	else
		self.data:SetWarningData(param_t)
	end
	ViewMgr:OpenView(PublicWarningView, param)
end

function PublicPopupCtrl:CloseWarningView()
	ViewMgr:CloseView(PublicWarningView)
end

--打开改名框title
--[[param_t = {
	title == 标题
	tips  == 提示
	count == 最大名字长度
	reset_func 请求func
}]]
function PublicPopupCtrl:ResetName(param_t)
	ViewMgr:OpenView(ResetNameView,param_t)
end

--显示飘字
function PublicPopupCtrl:ShowCustom(item_list)
	for i, v in ipairs(item_list) do
		local operate = BagData.Instance:ItemBuild(v)
		if v.change_item then
			operate = BagData.Instance:ItemBuild(v.change_item)
		end
		-- 直接使用道具不会进入背包，不提示
		if Item.GetUseType(operate.item_id) ~= ItemUseType.CurrencyItem then 
            self:CenterI({item_id = operate.item_id, num = operate.num, color_str = operate:ColorStr(),item = operate,icon_id = operate:IconId()})
		end 
        ChatCtrl.Instance:ChannelChatCustom {item_get = {item = operate, num = operate.num}}
    end
end

function PublicPopupCtrl:ShowCustomV(item_list)
	for i,v in ipairs(item_list) do
		self:CenterV{item_id = v.item_id, num = v.num}
		ChatCtrl.Instance:ChannelChatCustom{item_get = {item = v, num = v.num}}
	end
end

--打开属性来源弹窗
--[[param_t = {
    reason_type 属性来源列表类型(ATTRLIST_REASON_TYPE)
    param1      属性来源列表参数(角色0，宠物pet_index, 伙伴partner_id)
    attr_type   属性类型(BattleAttriType)
    position    窗口显示位置Vector2
}]]
function PublicPopupCtrl:OpenAttrReason(param_t)
	self.attr_reason_data:SetSelectData(param_t)
	if not ViewMgr:IsOpen(AttrReasonView) then
		local select_data = self.attr_reason_data:GetSelectData()
		self:SendAttrListReasonReq(select_data.reason_type, select_data.param1)
		ViewMgr:OpenView(AttrReasonView)
	end
end

function PublicPopupCtrl:OnAttrListReason(protocol)
	self.attr_reason_data:OnAttrListReason(protocol)
end

function PublicPopupCtrl:SendAttrListReasonReq(reason_type, param1)
	local protocol = GetProtocol(CSAttrListReasonReq)
	protocol.reason_type = reason_type
	protocol.param1 = param1
	SendProtocol(protocol)
end

-- 添加拦截跑马灯的方法，有些抽奖活动消息需要延迟播报，当func 返回true的时候拦截跑马灯消息
function PublicPopupCtrl:AddInterceptRollingFunc(func)
	local ht = {frame_count = Time.frameCount}
	self.intercept_rolling_msg_func[ht] = func
	return ht
end

function PublicPopupCtrl:RemoveInterceptRollingFunc(ht)
	if ht then
		self.intercept_rolling_msg_func[ht] = nil
	end
end

function PublicPopupCtrl:TopLayerBlackShow(is_show)
	self.data.top_layer_black:Set{
		is_show = is_show,
	}
end

--item_list 需要展示物品的列表（Item.New）
--other_reward = {gold_type = CurrencyType., item_id = 65534, num = 1} 不显示传nil, gold_type|item_id|icon_id选其一
--is_not_show 是否不需要手动飘字（可能存在延迟展示） 默认会飘（需要服务端屏蔽通知飘字）
--desc 描述
function PublicPopupCtrl:OpenRewardView(item_list, other_reward, is_not_show, desc)
    ViewMgr:OpenView(GetRewardView,{desc = desc, item_list = item_list, other_reward = other_reward, is_not_show = is_not_show})
end
--list = {{desc = , green = },{desc = , green = }}
function PublicPopupCtrl.ConditionTip(title, list)
	ViewMgr:OpenView(ConditionTip, {title = title , list = list})
end
-- param_t = {title = "", desc = ""}
function PublicPopupCtrl.PopupLifeMsg(param_t)
	ViewMgr:OpenView(PopupLifeMsg, param_t or {})
end


-- 小女孩对话那种弹框param_t:参考RewardDialog里面的default_param_t
function PublicPopupCtrl:OpenRewardDialog(param_t)
	local remind_key = param_t.remind_key
	if remind_key and DialogTipsData.Instance:TodayIsTrue(remind_key) then
		return
	end
	ViewMgr:OpenView(RewardDialog, param_t)
end

function PublicPopupCtrl:OnSCForbidReason(protocol)
	self:HideWait()
	-- local role_login_data = RoleSelectData.Instance:GetSelectRoleData()
	local login_role_data = LoginData.Instance:GetLoginRoleById(RoleSelectData.Instance:GetSelectRoleData().id)
	if login_role_data then
		login_role_data.forbid_time = TimeCtrl.Instance:GetServerTime() + protocol.forbid_time_log
	end
	LoginData.Instance:SaveLastLoginServer(LoginData.Instance:GetAccountKey(),LoginData.Instance:GetLastLoginServer() )
    LoginData.Instance:SetLoginBackType(RELOGIN_TYPE.ROLE_SELECT)
    LoginCtrl.Instance:unloadGameLogicBeforeRelogin()
    LoginCtrl.Instance:RequestServerAccountLogin(function(error_code)
        if error_code >= AccountLoginResult.Success then
            SceneCtrl.Instance:GotoScene(LoginScene)
        else
            PublicPopupCtrl.Instance:Center(string.format("RecvLoginToAccount ERROR:%s",tostring(error_code)))
        end
    end)   
end

-- 将拦截的激活飘字放出来
function PublicPopupCtrl:ReleaseShieldParam(popup)
	self.is_shield = false
    if popup then
		for k, v in pairs(self.shield_data) do
			if v.type=="Center" then
				PublicPopupCtrl.Instance:Center(v.info)
			elseif v.type=="CenterI" then
				PublicPopupCtrl.Instance:CenterI(v.info)
			end
		end
    end
    self.shield_data = {}
end

function PublicPopupCtrl:SetShield()
	self.is_shield = true
end
