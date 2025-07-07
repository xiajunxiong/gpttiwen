VIEW_REQ("modules/serveractivity/tiger_year/tiger_great_luck/tiger_great_luck_view")

TigerGreatLuckCtrl = TigerGreatLuckCtrl or BaseClass(BaseCtrl)

function TigerGreatLuckCtrl:__init()
	if TigerGreatLuckCtrl.Instance ~= nil then
		Debuger.LogError("[TigerGreatLuckCtrl] attempt to create singleton twice!")
		return
	end	
	TigerGreatLuckCtrl.Instance = self
	self.data = TigerGreatLuckData.New()
	self:RegisterAllProtocols()

	Remind.Instance:Register(Mod.TigerYear.TigerGreatLuck, self.data.tiger_great_luck_data, function ()
		if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_TIGER_GREAT_LUCK) then
			return 0
		end
		return self.data:RemindNum() 
    end)
end

function TigerGreatLuckCtrl:__delete()
	self:UnRegisterAllProtocols()
	TigerGreatLuckCtrl.Instance = nil
	Delete(self, "data")
end

function TigerGreatLuckCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAHuXingDaYunInfo, "OnSCRAHuXingDaYunInfo")
    self:RegisterProtocol(SCRAHuXingDaYunBeInvitedInfo, "OnSCRAHuXingDaYunBeInvitedInfo")
end

function TigerGreatLuckCtrl:UnRegisterAllProtocols()
	self:UnRegisterProtocol(SCRAHuXingDaYunInfo)
	self:UnRegisterProtocol(SCRAHuXingDaYunBeInvitedInfo)
end

function TigerGreatLuckCtrl:OnInit()

end

function TigerGreatLuckCtrl:OnUnloadGameLogic()
    self.data:ResetData()
end

function TigerGreatLuckCtrl:OnSCRAHuXingDaYunInfo(protocol)
	self.data:SetInfo(protocol)
end

function TigerGreatLuckCtrl:OnSCRAHuXingDaYunBeInvitedInfo(protocol)
	self.data:SetInvitedInfo(protocol)
end

-- 发送请求
-- opera_type：0 请求下发
-- opera_type：1 邀请 ，p1：uid
-- opera_type：2 接受邀请 ，p1：发过来列表的下标
-- opera_type：3 领取奖励
-- opera_type：4 领取被邀请奖励
function TigerGreatLuckCtrl:Send(opera_type, param_1)
	ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_TIGER_GREAT_LUCK, opera_type, param_1)
end

--------------------TigerGreatLuckData--------------------
TigerGreatLuckData = TigerGreatLuckData or BaseClass()

function TigerGreatLuckData:__init()
	if TigerGreatLuckData.Instance ~= nil then
		Debuger.LogError("[TigerGreatLuckData] attempt to create singleton twice!")
		return
	end
	TigerGreatLuckData.Instance = self
    self.tiger_great_luck_data = SmartData.New()
    self:ResetData()
end

function TigerGreatLuckData:ResetData()
    self:ResetTigerGreatLuckData()
	self.reward_group_config = nil
	self.house_show_reward = {}
	self.road_info = {}
end

function TigerGreatLuckData:ResetTigerGreatLuckData()
    self.tiger_great_luck_data:Set({
		info = {
			finish_pos = 0,
			is_can_receive = 0,
			invite_time = 0,
			invited_times = 0,
			invited_time = 0,
			first_flag = {},
			is_can_receive_invited = 0,
		},
		invited_info = {
			count = 0,
			list = {},
		},
	})
end

function TigerGreatLuckData:GetInfo()
	return self.tiger_great_luck_data.info
end

function TigerGreatLuckData:GetInvitedInfo()
	return self.tiger_great_luck_data.invited_info
end

function TigerGreatLuckData:SetInfo(protocol)
	self.tiger_great_luck_data.info = protocol.info
end

function TigerGreatLuckData:SetInvitedInfo(protocol)
	self.tiger_great_luck_data.invited_info = protocol.invited_info
end

-- 总红点
function TigerGreatLuckData:RemindNum()
	return self:DownLeftBtnRemind() + self:DownRightBtnRemind() + self:TopRewardRemind()
end

-- 左下按钮红点
function TigerGreatLuckData:DownLeftBtnRemind()
	if self.tiger_great_luck_data.info.invite_time == 0 and self.tiger_great_luck_data.info.is_can_receive == 0 then
		return Config.huxingdayun_auto.other[1].invite_times > self.tiger_great_luck_data.info.finish_pos and 1 or 0
	end
	return 0
end

-- 右下按钮红点
function TigerGreatLuckData:DownRightBtnRemind()
	if self.tiger_great_luck_data.info.invited_time == 0 then
		if self.tiger_great_luck_data.info.is_can_receive_invited == 1 then
			return 1
		end
	end
	if self.tiger_great_luck_data.invited_info.count > 0 then
		return Config.huxingdayun_auto.other[1].be_invite_times > self.tiger_great_luck_data.info.invited_times and 1 or 0
	end
	return 0
end

-- 房子红点
function TigerGreatLuckData:TopRewardRemind()
	return self.tiger_great_luck_data.info.is_can_receive
end

-- 从0开始的seq
function TigerGreatLuckData:GetFirstFlag(seq)
	if TableIsEmpty(self.tiger_great_luck_data.info.first_flag) then
		return 0
	else
		return self.tiger_great_luck_data.info.first_flag[seq] and self.tiger_great_luck_data.info.first_flag[seq] or 0
	end
end

function TigerGreatLuckData:GetRewardConfig()
    return ServerActivityData.Instance:GetActOpenDayCfg(ACTIVITY_TYPE.RAND_TIGER_GREAT_LUCK, Config.huxingdayun_auto.gift_configure, "seq")
end

function TigerGreatLuckData:GetRewardGroupConfig()
    if TableIsEmpty(self.reward_group_config) then
        self.reward_group_config = DataHelper.Subdivision(Config.huxingdayun_auto.gift_group, {"reward_group", "seq"})
    end
    return self.reward_group_config
end

function TigerGreatLuckData:GetHouseShowReward(data)
	if TableIsEmpty(self.house_show_reward[data.explore_place]) then
		self.house_show_reward[data.explore_place] = {}
		local reward_group = self:GetRewardGroupConfig()
		-- m_type：1是首通，2是概率
		local get_item_fun = function(item, m_type)
			local t = {}
			t.item = item
			t.m_type = m_type
			return t
		end
		-- 第一个表是首通奖励，第二个是重复奖励，第三个是助力奖励
		self.house_show_reward[data.explore_place][1] = {}
		self.house_show_reward[data.explore_place][2] = {}
		self.house_show_reward[data.explore_place][3] = {}
		for k, v in pairs(data.first_reward_item) do
			table.insert(self.house_show_reward[data.explore_place][1], get_item_fun(v, 1))
		end
		for k, v in pairs(reward_group[data.repeat_reward_group]) do
			for l, w in pairs(v.reward_item) do
				table.insert(self.house_show_reward[data.explore_place][2], get_item_fun(w, 2))
			end
		end
		for k, v in pairs(reward_group[data.be_invited_reward_group]) do
			for l, w in pairs(v.reward_item) do
				table.insert(self.house_show_reward[data.explore_place][3], get_item_fun(w, 2))
			end
		end
	end
	return self.house_show_reward[data.explore_place]
end

function TigerGreatLuckData:GetRoadInfo(index, rect_list)
	if TableIsEmpty(self.road_info[index]) then
		if TableIsEmpty(rect_list) then
			LogError("数据异常index",index)
			return
		end
		self.road_info[index] = {}
		self.road_info[index].start_pos = rect_list[1].anchoredPosition
		self.road_info[index].end_pos = rect_list[rect_list:Length()].anchoredPosition
		self.road_info[index].length = 0
		self.road_info[index].path = {}
		for i = 1, rect_list:Length() - 1 do
			local t = {}
			t.index = i
			t.start_pos = rect_list[i].anchoredPosition
			t.end_pos = rect_list[i + 1].anchoredPosition
			t.distance = GameMath.Distance(t.start_pos, t.end_pos, true)
			table.insert(self.road_info[index].path, t)
			self.road_info[index].length = self.road_info[index].length + t.distance
		end
		local _distance = 0
		for i, v in ipairs(self.road_info[index].path) do
			v.start_percent = _distance / self.road_info[index].length
			_distance = _distance + v.distance
			v.end_percent = _distance / self.road_info[index].length
			v.total_percent = v.end_percent - v.start_percent
		end
	end
	return self.road_info[index]
end