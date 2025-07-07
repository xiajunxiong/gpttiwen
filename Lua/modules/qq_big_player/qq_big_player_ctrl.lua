
VIEW_REQ("modules/qq_big_player/qq_big_player_view")


local levelToWeekKey = {
	[3] = "qlk_state",
	[4] = "zwk_state",
	[1] = "dwk_state",
	[2] = "super_state",
}

local levelToKey = {
	[3] = "qlk_jbtq_state",
	[4] = "zwk_jbtq_state",
	[1] = "dwk_jbtq_state",
	[2] = "super_jbtq_state",
}

QQBigPlayerCtrl = QQBigPlayerCtrl or BaseClass(BaseCtrl)
function QQBigPlayerCtrl:__init()
	if QQBigPlayerCtrl.Instance ~= nil then
        Debuger.LogError("[QQBigPlayerCtrl] attempt to create singleton twice!")
        return
    end
    QQBigPlayerCtrl.Instance = self
	self.data = QQBigPlayerData.New()
end

function QQBigPlayerCtrl.IsQQPC()
	if ChannelAgent.ISPCSDK and  ChannelAgent.ISPCSDK() and ChannelAgent.PCAgentType() == "QQ" then
		return true
	end
end

function QQBigPlayerCtrl:GetUrlInfo()
	local t = {}
	t.openid = LoginData.Instance:GetAccountKey()
	t.openkey = Nirvana.GameAgent.GetToken()
	t.time_stamp = os.time()
	t.spid = GLOBAL_CONFIG:PkgInfo().agent_id
	t.server_id = LoginData.Instance:GetLastLoginServer()
	t.sign = GameUtil.StrMd5(t.spid .. t.server_id .. t.openid .. RoleId() .. t.time_stamp .. "hdISla9sjXphPqEoE8lZcg==")
	return t
end

function QQBigPlayerCtrl:SendUrl(url_head, func, dwk_level)
	if QQBigPlayerCtrl.IsQQPC() then
		local info_t = self:GetUrlInfo()
		local real_url = Format("%s?openid=%s&openkey=%s&time_stamp=%s&spid=%s&server_id=%s&roleid=%s&sign=%s&dwk_level=%s&pf=qqgame",
			url_head,
			info_t.openid,
			info_t.openkey,
			info_t.time_stamp,
			info_t.spid,
			info_t.server_id,
			RoleId(),
			info_t.sign,
			dwk_level)
		WebRequester:Request(real_url,function (data,error)
			if StringIsEmpty(error) then
				local ret, statue = TryParseJson(data)
				if statue then
					func(ret)
				end
			end
		end)
	end
end

function QQBigPlayerCtrl:OnClickViewItem(item_data)
	if self.data.qq_big_player_info.level <= 0 then
		PublicPopupCtrl.Instance:Center(Language.QQBigPlayer.LevelNotEnough)
		return
	elseif item_data.param.click_show_type == 2 then
		QQBigPlayerCtrl.Instance:SendGetWeekGift()
	elseif item_data.param.click_show_type == 4 then
		QQBigPlayerCtrl.Instance:SendGetLevelGift()
	end
end
--获取周礼包领取信息
function QQBigPlayerCtrl:SendGetWeekInfo()
	local url_head = "http://"..GLOBAL_CONFIG:GetUrlHead().."/".. GLOBAL_CONFIG:PkgInfo().agent_id .."/get_week_gift_info.php"
	self:SendUrl(url_head, function (ret)
		self.data:SetWeekInfo(ret)
	end, self.data.qq_big_player_info.level)
end

--领取周礼包
function QQBigPlayerCtrl:SendGetWeekGift()
	local can_get,tip = self.data:CanGetWeekGift()
	if not can_get then
		if not StringIsEmpty(tip) then
			PublicPopupCtrl.Instance:Center(tip)
		end
		return
	end
	local url_head = "http://"..GLOBAL_CONFIG:GetUrlHead().."/".. GLOBAL_CONFIG:PkgInfo().agent_id .."/get_week_gift.php"
	self:SendUrl(url_head, function (ret)
		self.data:SetWeekInfoState(ret)
	end,self.data.qq_big_player_info.level)
end

--领取对应咖级的绝版特权
function QQBigPlayerCtrl:SendGetLevelGift()
	local can_get , tip = self.data:CanGetCurLevelGift()
	if not can_get then
		PublicPopupCtrl.Instance:Center(tip)
		return
	end
	local url_head = "http://"..GLOBAL_CONFIG:GetUrlHead().."/".. GLOBAL_CONFIG:PkgInfo().agent_id .."/get_title_gift.php"
	self:SendUrl(url_head, function (ret)
		self.data:SetLevelGiftState(ret)
	end, self.data.cur_select_level)
end

function QQBigPlayerCtrl:NoticeQQPlayerInfoChange()
	if QQBigPlayerCtrl.IsQQPC() then
		if self.data.qq_big_player_info.level <= 0 then
			return
		end
		local url_head = "http://"..GLOBAL_CONFIG:GetUrlHead().."/".. GLOBAL_CONFIG:PkgInfo().agent_id .."/update_dwk_info.php"
		self:SendUrl(url_head, function (ret)
		end, self.data.qq_big_player_info.level)
	end
end
------------------------------------------------
QQBigPlayerData = QQBigPlayerData or BaseClass()
function QQBigPlayerData:__init()
	if QQBigPlayerData.Instance ~= nil then
        Debuger.LogError("[QQBigPlayerData] attempt to create singleton twice!")
        return
    end
	QQBigPlayerData.Instance = self
	self.cur_select_level = 1
	self.qq_big_player_info = SmartData.New({score = 0, level = 0, binding = false, is_using_card = false, real_level = 0})
	self.week_gift_info = SmartData.New(
		{
			week_gift_data = {qlk_state = 0,zwk_state = 0, dwk_state = 0, super_state = 0}, 
			tq_data = {qlk_jbtq_state = 0, zwk_jbtq_state = 0, dwk_jbtq_state = 0, super_jbtq_state = 0}
		})
end

function QQBigPlayerData.LevelToIndex(level)
	local t = {
		[0] = 1,
		[1] = 3,
		[2] = 4,
		[3] = 1,
		[4] = 2,
	}
	return t[level]
end

--ret:
--score:咖值
--level:咖级		0 :非大玩咖 1:大玩咖 2:超级大玩咖 3:潜力咖 4:准玩咖
--binding:是否被超咖绑定且有效期内
--discount:加赠比例
--payReturnCount:加赠次数
--isUsingCard:是否特权体验
--realLevel:真实咖级
function QQBigPlayerData:SetQQBigPlayerInfo(ret,check)
	if check then
		for k,v in pairs(ret) do
			if k ~= "ret" and k ~= "msg" then
				if self.qq_big_player_info[k] ~= v then
					QQBigPlayerCtrl.Instance:NoticeQQPlayerInfoChange()
					break
				end
			end
		end
	end
	self.qq_big_player_info:Set(ret)

	--每10分钟查询一下大玩咖信息
	if self.check_info_runner == nil then
		self.check_info_runner = TimeHelper:AddRunTimer(function ()
			LoginCtrl.Instance:GetQQPCPlayerLevel()
		end, 600)
	end
end

function QQBigPlayerData:SetWeekInfo(ret)
	if ret.ret == 0 then
		self.week_gift_info:Set(ret)
	end
end

function QQBigPlayerData:SetWeekInfoState(ret)
	if ret.ret == 0 then
		PublicPopupCtrl.Instance:Center(Language.QQBigPlayer.GetSucc)
		for k, v in pairs(self.week_gift_info.week_gift_data) do
			if ret.week_gift_data[k] ~= nil then
				self.week_gift_info.week_gift_data[k] = ret.week_gift_data[k]
				self.qq_big_player_info:SetDirty("score")
				break
			end
		end
	elseif ret.ret == 8 then
		PublicPopupCtrl.Instance:Center(Language.QQBigPlayer.HasGotWeekGift)
	elseif ret.ret == 9 then
		PublicPopupCtrl.Instance:Center(Language.QQBigPlayer.LevelNotEnough)
	else
		PublicPopupCtrl.Instance:Center(Format(Language.QQBigPlayer.WeekGiftGetErrorTips, ret.ret == nil and -999 or ret.ret))
	end
end

function QQBigPlayerData:SetLevelGiftState(ret)
	if ret.ret == 0 then
		PublicPopupCtrl.Instance:Center(Language.QQBigPlayer.GetSucc)
		for k, v in pairs(self.week_gift_info.tq_data) do
			if ret.tq_data[k] ~= nil then
				self.week_gift_info.tq_data[k] = ret.tq_data[k]
				self.qq_big_player_info:SetDirty("score")
				break
			end
		end
	elseif ret.ret == 9 then
		PublicPopupCtrl.Instance:Center(Language.QQBigPlayer.LevelNotEnough)
	elseif ret.ret == 8 then
		PublicPopupCtrl.Instance:Center(Language.QQBigPlayer.HasGotCurLevelGift)
	else
		PublicPopupCtrl.Instance:Center(Format(Language.QQBigPlayer.LevelGift, ret.ret == nil and -999 or ret.ret))
	end
end

--特权礼包是否可领
function QQBigPlayerData:CanGetCurLevelGift()
	if self.cur_select_level == 0 then
		return false
	end
	if self.LevelToIndex(self.cur_select_level) > self.LevelToIndex(self.qq_big_player_info.level) then
		return false, Language.QQBigPlayer.LevelNotEnough
	end
	return self.week_gift_info.tq_data[levelToKey[self.cur_select_level]] == 0, Language.QQBigPlayer.HasGotCurLevelGift
end
--周礼包是否可领
function QQBigPlayerData:CanGetWeekGift()
	if self.cur_select_level == 0 then
		return false
	end
	if self.LevelToIndex(self.cur_select_level) ~= self.LevelToIndex(self.qq_big_player_info.level) then
		return false, Format(Language.QQBigPlayer.WeekGiftLevelNotMatch, Language.QQBigPlayer.levelName[self.qq_big_player_info.level])
	end
	return self.week_gift_info.week_gift_data[levelToWeekKey[self.qq_big_player_info.level]] == 0, Language.QQBigPlayer.HasGotWeekGift
end