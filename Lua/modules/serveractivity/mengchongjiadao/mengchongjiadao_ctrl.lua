FILE_REQ("modules/serveractivity/mengchongjiadao/mengchongjiadao_data")
VIEW_REQ("modules/serveractivity/mengchongjiadao/mengchongjiadao_view")
VIEW_REQ("modules/serveractivity/mengchongjiadao/mengchongjiadao_obj")
VIEW_REQ("modules/serveractivity/mengchongjiadao/mengchongjiadao_get_view")

MengChongJiaDaoCtrl = MengChongJiaDaoCtrl or BaseClass(BaseCtrl)
function MengChongJiaDaoCtrl:__init()
    if MengChongJiaDaoCtrl.Instance ~= nil then
        Debuger.LogError("[MengChongJiaDaoCtrl] attempt to create singleton twice!")
        return
    end
    MengChongJiaDaoCtrl.Instance = self
    self.data = MengChongJiaDaoData.New()
    self:RegisterAllProtocols()
end

function MengChongJiaDaoCtrl:__delete()
    MengChongJiaDaoCtrl.Instance = nil
    Delete(self, "data")
end

function MengChongJiaDaoCtrl:OnInit()
    Remind.Instance:Register(Mod.YunZe.MengChongJiaDao, self.data.remind_info, function()
        if not ActivityData.IsOpen(ACTIVITY_TYPE.RAND_MENG_CHONG_JIA_DAO) then
            return 0
        end
        return self.data:GetRemindNum()
    end)
end

function MengChongJiaDaoCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function MengChongJiaDaoCtrl:RegisterAllProtocols()
    self:RegisterProtocol(SCRAYunZeCutePetInfo, "OnSCRAYunZeCutePetInfo")
    self:RegisterProtocol(SCRAYunZeCutePetChallengeInfo, "OnSCRAYunZeCutePetChallengeInfo")
end

--请求所有信息
function MengChongJiaDaoCtrl:SendAllInfo()
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MENG_CHONG_JIA_DAO, 0)
end

--挑战
function MengChongJiaDaoCtrl:SendFetchRewardReq(monster_id)
    ServerActivityCtrl.Instance:SendActivityReq(ACTIVITY_TYPE.RAND_MENG_CHONG_JIA_DAO, 1, monster_id)
end

function MengChongJiaDaoCtrl:OnSCRAYunZeCutePetInfo(protocol)
    self.data:SetMengChongInfo(protocol)
end

function MengChongJiaDaoCtrl:OnSCRAYunZeCutePetChallengeInfo(protocol)
    local monster_cfg = nil
    if protocol.is_challenge_status ~= -1 then
        monster_cfg = self.data:GetCurMonsterCfg()
    end
    self.data:SetIsBattle(false)
    self.data:SetBattleInfo(protocol)
    if protocol.is_challenge_status == 1 then
        if monster_cfg ~= nil then
            local function func_delay()
                ViewMgr:OpenView(MengChongJiaDaoGetView, monster_cfg)
            end
            Invoke(func_delay, 0.5)
        end
    elseif protocol.is_challenge_status == 0 or protocol.is_challenge_status == 1 then
        PublicPopupCtrl.Instance:Center(Language.MengChongJiaDao.Error1)
    end
end
