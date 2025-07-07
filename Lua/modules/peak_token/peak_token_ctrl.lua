FILE_REQ("modules/peak_token/peak_token_data")
VIEW_REQ("modules/peak_token/peak_token_view")
VIEW_REQ("modules/peak_token/peak_token_gift_view")
VIEW_REQ("modules/peak_token/peak_token_reward_view")

PeakTokenCtrl = PeakTokenCtrl or BaseClass(BaseCtrl)

function PeakTokenCtrl:__init()
	if PeakTokenCtrl.Instance ~= nil then
		Debuger.LogError("[PeakTokenCtrl] attempt to create singleton twice!")
		return
	end
	PeakTokenCtrl.Instance = self
	self.data = PeakTokenData.New()
    self:RegisterAllProtocols()
end

function PeakTokenCtrl:__delete()
    self:UnRegisterAllProtocols()
    PeakTokenCtrl.Instance = nil
end

function PeakTokenCtrl:OnUnloadGameLogic()
    self.data.info_data.info_list = {}
end

function PeakTokenCtrl:RegisterAllProtocols()
    self:RegisterProtocol(CSPeaktokenRoleReq)
    self:RegisterProtocol(SCPeaktokenRoleStatusInfo,"OnPeaktokenRoleStatusInfo")
    self:RegisterProtocol(SCPeaktokenRoleJoinInfo,"OnPeaktokenRoleJoinInfo")
    self:RegisterProtocol(SCPeaktokenRoleInfo,"OnPeaktokenRoleInfo")
end

function PeakTokenCtrl:OnInit()
end

-- 天下第一比武大会 天下至尊令
function PeakTokenCtrl:SendPeaktokenSeq(req_type,param1)
	local protocol = GetProtocol(CSPeaktokenRoleReq)
    protocol.req_type = req_type or 0
    protocol.param1 = param1 or 0
	SendProtocol(protocol)
end

function PeakTokenCtrl:OnPeaktokenRoleStatusInfo(protocol)
	self.data:SetPeaktokenRoleStatusInfo(protocol)
end

function PeakTokenCtrl:OnPeaktokenRoleJoinInfo(protocol)
	self.data:SetPeaktokenRoleJoinInfo(protocol)
end

function PeakTokenCtrl:OnPeaktokenRoleInfo(protocol)
	self.data:SetPeaktokenRoleInfo(protocol)
end

--雕像对话弹窗
function PeakTokenCtrl:OnNpcDialogView(data)
    local button = NpcDialogBehBtnDataFuncExe.New(Language.PeakToken.ActivityName,BindTool.Bind2(self.OnPeaktoken, self,data), true)
	button:SetIconType(NpcIconType.Act)
    NpcDialogCtrl.Instance:NpcDialogView({
        talk_list = {{talker = 0,name = data.name,appearance = data.appearance,talk_text = self.data:GetTalkText(data.info.end_time)}},
        beh_btn_list = {
            [1] = button,
            [2] = NpcDialogBehBtnDataFuncExe.New(Language.PeakToken.NpcDialogBtn, function()
            PublicPopupCtrl.Instance:HelpTip(278)
        end, false)}
    })
end

--天下赐福•万灵狂欢
function PeakTokenCtrl:OnPeaktoken(data)
    local info = self.data:GetInfoDataById(data.info.id) or {}
    local reward_times = info.reward_times or data.info.reward_times
    if reward_times >= Config.supreme_token_auto.other[1].reward_num then
        PublicPopupCtrl.Instance:Center(Language.PeakToken.NotJoinTip)
        return
    end
    if not LoginData.Instance:IsOnCrossSever() then
        PublicPopupCtrl.Instance:AlertTip(Language.PeakToken.CrossTips,function()
            LoginCtrl.Instance:SendCrossStartReq()
        end)
        return
    end
    self.data:SetRoleInfoData(data)
    ViewMgr:OpenView(PeakTokenView,data)
end