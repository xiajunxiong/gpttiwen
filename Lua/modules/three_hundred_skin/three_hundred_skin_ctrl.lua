FILE_REQ("modules/three_hundred_skin/three_hundred_skin_data")
VIEW_REQ("modules/three_hundred_skin/three_hundred_skin")
VIEW_REQ("modules/three_hundred_skin/three_hundred_draw")

ThreeHundredSkinCtrl = ThreeHundredSkinCtrl or BaseClass(BaseCtrl)
-- 300抽皮肤
-- 活动号 2206

-- req_type
-- 0: 请求个人信息
-- 1：请求对应类型开始抽奖	p1:type
-- 2: 设置预选奖励			p1:type	p2:index -1 or n
-- 3: 领取对应类型预设奖励	p1:type

function ThreeHundredSkinCtrl:__init()
	if ThreeHundredSkinCtrl.Instance ~= nil then
		Debuger.LogError("[ThreeHundredSkinCtrl] attempt to create singleton twice!")
		return
	end
	ThreeHundredSkinCtrl.Instance = self
	self.data = ThreeHundredSkinData.New()
    self:RegisterAllProtocols()
	local update_remind_func = BindTool.Bind(self.UpdateRemindFunc, self)
	RemindCtrl.Instance:Remind():Register(Mod.ThreeHundredSkin.Main, self.data:SelectList(), update_remind_func)
	RemindCtrl.Instance:Remind():Register(Mod.ThreeHundredSkin.Main, self.data:Info(), update_remind_func)
	RemindCtrl.Instance:Remind():Register(Mod.ThreeHundredSkin.Main, RoleData.Instance:GetBaseData(), update_remind_func, "level")
	ActivityRandData.Instance:Register(Mod.ThreeHundredSkin.Main,function ()
        return ActivityData.IsOpen(ACTIVITY_TYPE.RAND_THREE_HUNDRED_SKIN) and self.data:Info().reward_item_seq == -1
    end,self.data:Info())
end

function ThreeHundredSkinCtrl:UpdateRemindFunc()
	return self.data:GetRemindNum()
end

function ThreeHundredSkinCtrl:__delete()
    self:UnRegisterAllProtocols()
    ThreeHundredSkinCtrl.Instance = nil
end

function ThreeHundredSkinCtrl:OnUnloadGameLogic()
	if self.handle_act then
		ActivityData.Instance:UnCareEvent(self.handle_act)
		self.handle_act = nil
	end
    self.data:ClearData()
end

function ThreeHundredSkinCtrl:OnInit()
	-- self.handle_act = ActivityData.Instance:CareEvent(BindTool.Bind(self.ActEvent, self))
end
function ThreeHundredSkinCtrl:ActEvent()
	if ActivityData.IsOpen(ACTIVITY_TYPE.RAND_THREE_HUNDRED_SKIN) then
		ThreeHundredSkinCtrl.ReqInfo()
	end
end

function ThreeHundredSkinCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRASanBaiChouInfo, "SCRASanBaiChouInfo")
    self:RegisterProtocol(SCRASanBaiChouDrawInfo, "SCRASanBaiChouDrawInfo")
end
function ThreeHundredSkinCtrl:SCRASanBaiChouInfo(protocol)
	self.data:Info().total = protocol.draw_times
	self.data:Info().reward_item_seq = protocol.reward_item_seq
	self.data:SetSelectInfo(protocol.select)
	-- self.draw_times = MsgAdapter.ReadInt()--总次数
    -- self.reward_item_seq = MsgAdapter.ReadInt()--表里的type
end

function ThreeHundredSkinCtrl:SCRASanBaiChouDrawInfo(protocol)
	self.data:Info().total = protocol.draw_times
	self.data:SetSingleInfo(protocol)
end


function ThreeHundredSkinCtrl:UnRegisterAllProtocols()
    self:UnRegisterProtocol(SCRASanBaiChouInfo)
    self:UnRegisterProtocol(SCRASanBaiChouDrawInfo)
end

function ThreeHundredSkinCtrl:SendTemplateReq(req_type,param_1,param_2)
	local protocol = ProtocolPool.Instance:GetProtocol(CSTemplateReq)
	protocol.req_type = req_type or 0
	protocol.param_1 = param_1 or 0
	protocol.param_2 = param_2 or 0
	SendProtocol(protocol)
end

function ThreeHundredSkinCtrl:OnTemplateInfo(protocol)
    self.data:SetTemplateInfo(protocol)
end

function ThreeHundredSkinCtrl.ReqInfo()
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_THREE_HUNDRED_SKIN
	param_t.opera_type = 0
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
	-- LogDG("信息",param_t)
end
function ThreeHundredSkinCtrl.Lottery(type)
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_THREE_HUNDRED_SKIN
	param_t.opera_type = 1
	param_t.param_1 = type
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
	-- LogDG("抽10次",param_t)
end
--index 0-9 -1不选
function ThreeHundredSkinCtrl.SetReward(type, index)
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_THREE_HUNDRED_SKIN
	param_t.opera_type = 2
    param_t.param_1 = type
	param_t.param_2 = index
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
	-- LogDG("选择",param_t)
end

function ThreeHundredSkinCtrl.Exchange(type)
    local param_t = {}
    param_t.rand_activity_type = ACTIVITY_TYPE.RAND_THREE_HUNDRED_SKIN
	param_t.opera_type = 3
    param_t.param_1 = type
    ServerActivityCtrl.Instance:SendRandActivityOperaReq(param_t)
	-- LogDG("领取",param_t)
end