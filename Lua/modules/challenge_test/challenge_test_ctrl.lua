
FILE_REQ("modules/challenge_test/challenge_test_config")
FILE_REQ("modules/challenge_test/challenge_test_data")

VIEW_REQ("modules/challenge_test/challenge_test_view")
VIEW_REQ("modules/challenge_test/challenge_test_video_view")
VIEW_REQ("modules/challenge_test/challenge_test_rank_view")

ChallengeTestCtrl = ChallengeTestCtrl or BaseClass(BaseCtrl)

function ChallengeTestCtrl:__init()
    if ChallengeTestCtrl.Instance ~= nil then
        Debuger.LogError("[ChallengeTestCtrl] attempt to create singleton twice!")
        return
    end
    ChallengeTestCtrl.Instance = self
    self.data = ChallengeTestData.New()

    self:RegisterProtocol(CSLunHuiWangChuanReq)

	self:RegisterProtocol(SCLunHuiWangChuanInfo, "OnLunHuiWangChuanInfo")

	FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_CHALLENGE_TEST, function(is_win)
        ViewMgr:OpenView(ChallengeTestView, {is_fail = 0 == is_win})
    end)

	RemindCtrl.Instance:Remind():Register(Mod.Experience.ChallengeTest, self.data.info_sm, function ()
		return self.data:GetChallengeTestRemindNum()
	end)

	ActivityData.Instance:CustomClickHandle(ActType.ChallengeTest,function()
		local end_func = function ()
			if 0 == UnityPlayerPrefs.GetInt(PrefKeys.ChallengeTest(RoleData.Instance:GetRoleId())) then
				UnityPlayerPrefs.SetInt(PrefKeys.ChallengeTest(RoleData.Instance:GetRoleId()), 1)
				ViewMgr:OpenView(ChallengeTestVideoView)
				ViewMgr:OpenView(ChallengeTestView)
			else
				ViewMgr:OpenView(ChallengeTestView)
			end
		end
		if TeamData.Instance:InTeam() then
			PublicPopupCtrl.Instance:DialogTips{
				content = Language.ChallengeTest.InTeamTips,
				confirm = {
					func = function ()
						TeamCtrl.Instance:SendExitTeam()
						ViewMgr:CloseView(DialogTipsView)
						Call(end_func)
					end
				}
			}
		else
			Call(end_func)
		end
    end)
end

function ChallengeTestCtrl:OnInit()
	ChallengeNaviCtrl.Instance:RegisterOpenReqFunc(Mod.Experience.ChallengeTest,function()
		local end_func = function ()
			if 0 == UnityPlayerPrefs.GetInt(PrefKeys.ChallengeTest(RoleData.Instance:GetRoleId())) then
				UnityPlayerPrefs.SetInt(PrefKeys.ChallengeTest(RoleData.Instance:GetRoleId()), 1)
				ViewMgr:OpenView(ChallengeTestVideoView)
				ViewMgr:OpenView(ChallengeTestView)
			else
				ViewMgr:OpenView(ChallengeTestView)
			end
		end
		if TeamData.Instance:InTeam() then
			PublicPopupCtrl.Instance:DialogTips{
				content = Language.ChallengeTest.InTeamTips,
				confirm = {
					func = function ()
						TeamCtrl.Instance:SendExitTeam()
						ViewMgr:CloseView(DialogTipsView)
						Call(end_func)
					end
				}
			}
		else
			Call(end_func)
		end
	end)
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.Experience.ChallengeTest,function()
		local num = self.data.info_sm.pass_layer
		local total = self.data.layer_max
		return num,total,self.data.info_sm.next_refresh_tamp
        -- ChallengeNaviCtrl.SetFormatDescNum(desc_val,num,total)
    end)
end

function ChallengeTestCtrl:__delete()
    if ChallengeTestCtrl.Instance == self then
        ChallengeTestCtrl.Instance = nil
    end
end

function ChallengeTestCtrl:OnLunHuiWangChuanInfo(protocol)
    self.data:SetLunHuiWangChuanInfo(protocol)
end

function ChallengeTestCtrl:OnGetPersonRankListAck(protocol)
	self.data:SetGetPersonRankListAck(protocol)
end

function ChallengeTestCtrl:SendLunHuiWangChuanReq(param_t)
    local protocol = GetProtocol(CSLunHuiWangChuanReq)
	protocol.req_type = param_t.req_type or 0
	protocol.param = param_t.param or 0
	SendProtocol(protocol)
end

function ChallengeTestCtrl:SendLunHuiWangChuanInfo()
	local param_t = {
		req_type = ChallengeTestConfig.ReqType.info,
	}
	self:SendLunHuiWangChuanReq(param_t)
end

function ChallengeTestCtrl:SendLunHuiWangChuanBattle()
	local param_t = {
		req_type = ChallengeTestConfig.ReqType.battle,
	}
	self:SendLunHuiWangChuanReq(param_t)
end

function ChallengeTestCtrl:SendLunHuiWangChuanReward(layer)
	local param_t = {
		req_type = ChallengeTestConfig.ReqType.reward,
		param = layer,
	}
	self:SendLunHuiWangChuanReq(param_t)
end
