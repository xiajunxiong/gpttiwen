FILE_REQ("modules/deep_dark_fantasy/deep_dark_fantasy_data")
VIEW_REQ("modules/deep_dark_fantasy/deep_dark_fantasy_view")
VIEW_REQ("modules/deep_dark_fantasy/deep_dark_fantasy_rank")
VIEW_REQ("modules/deep_dark_fantasy/deep_dark_fantasy_model")
VIEW_REQ("modules/deep_dark_fantasy/deep_dark_fantasy_finish")
VIEW_REQ("modules/deep_dark_fantasy/deep_dark_fantasy_video_view")
VIEW_REQ("modules/deep_dark_fantasy/deep_dark_fantasy_box_roll")

DeepDarkFantasyCtrl = DeepDarkFantasyCtrl or BaseClass(BaseCtrl)
function DeepDarkFantasyCtrl:__init()    
    if DeepDarkFantasyCtrl.Instance ~= nil then
        Debuger.LogError("[DeepDarkFantasyCtrl] attempt to create singleton twice!")
        return
    end
    DeepDarkFantasyCtrl.Instance = self
    self.data = DeepDarkFantasyData.New()

    self:RegisterProtocol(SCNetherWorldPurgatoryInfo, "RecvNetherWorldPurgatoryInfo")
    self:RegisterProtocol(SCNetherWorldPurgatorySigleInfo,"RecvNetherWorldPurgatorySigleInfo")
    self:RegisterProtocol(SCNetherWorldPurgatoryMiLingInfo,"RecvNetherWorldPurgatoryMiLingInfo")
    self:RegisterProtocol(SCNetherWorldPurgatoryChapterInfo,"RecvNetherWorldPurgatoryChapterInfo")
    self:RegisterProtocol(CSNetherWorldPurgatoryReq)

    FinishData.Instance:SetCloseFunc(BATTLE_MODE.BATTLE_MODE_NETHER_WORLD_PURGATORY,function()
        ViewMgr:OpenView(DeepDarkFantasyView)
	end)

    ActFormationData.Instance:RegistFunctions(PET_HELPER_FORMATION_TYPE.PET_HELPER_FORMATION_NETHERWORLD, 
    {MainType = DeepDarkFantasyCtrl.Instance, challenge_func = "DeepDarkFantasyChallenge", pet_on_limit_func = "PetOnLimitNum", assist_text_func = "AssistTextFunc"}
    )
end

function DeepDarkFantasyCtrl:OnInit()
    ChallengeNaviCtrl.Instance:RegisterOpenReqFunc(Mod.DeepDarkFantasy.Main,function()
        self:OnDeepDarkFantasy()
	end)
    ChallengeNaviCtrl.Instance:RegisterVariantShowFunc(Mod.DeepDarkFantasy.Main,function()
		local num = self.data:GetTodayTimes()
		local total =  self.data.config.other[1].times
		return num,total -- ,self.data:CheckNextWeekTime()
        -- ChallengeNaviCtrl.SetFormatDescNum(desc_val,num,total)
    end)
    ActivityData.Instance:CustomClickHandle(ActType.DeepDarkFantasy, BindTool.Bind(self.OnDeepDarkFantasy, self))
end

function DeepDarkFantasyCtrl:OnDeepDarkFantasy()
    local function send_msg()
        if TeamData.Instance:InTeam() then 
            TeamCtrl.Instance:SendExitTeam()
        end 

        -- 暂时废弃，设备首启动
        if self.data:CheckIsVideoShow() then 
            VideoCtrl.Instance:StartVideoById(2,function()
                if self.Timer then TimeHelper:CancelTimer(self.Timer) end 
                self.Timer = nil
                if not ViewMgr:IsOpen(DeepDarkFantasyView) then 
                    DeepDarkFantasyData.Instance.model_data.start = 1
                    ViewMgr:OpenView(DeepDarkFantasyView)
                end 
            end)
            self:VideoShowComplete()
            self.Timer = TimeHelper:AddDelayTimer(function()
                DeepDarkFantasyData.Instance.model_data.start = 1
                ViewMgr:OpenView(DeepDarkFantasyView)
            end,17)
        else
            ViewMgr:OpenView(DeepDarkFantasyView)
        end

        ViewMgr:CloseView(DialogTipsView)
    end

    if TeamData.Instance:InTeam() then
        local info = {
            content = Language.DeepDarkFantasy.TeamErrorDialog,
            cancel = {
                name = nil,
                func = nil
            },
            confirm = {
                name = nil,
                func = send_msg
            }
        }
        PublicPopupCtrl.Instance:DialogTips(info)
        return
    end

    send_msg()
end

function DeepDarkFantasyCtrl:RecvNetherWorldPurgatoryInfo(protocol)
    -- LogDG("健哥哥这是幽冥炼狱的下发哦?2630",protocol)
    self.data:SetChallengeInfo(protocol)
end

-- req 0 信息下发
-- req 1 请求战斗 p1 章节 p2 对应关卡
-- req 2 领取层数奖励 p1 章节 p2 关卡 p3 任务
-- req 3 密令抽奖 p1 章节 p2 类型 p3 抽奖次数
-- req 4 设置客户端是否播放过了

function DeepDarkFantasyCtrl:SendOperateReq(req_type, param1, param2, param3)
    -- LogDG("健哥哥我要发请求了哦?2631",req_type,param1,param2,param3)
    local protocol = GetProtocol(CSNetherWorldPurgatoryReq)
    protocol.req_type = req_type or 0
    protocol.param1 = param1 or 0
    protocol.param2 = param2 or 0
    protocol.param3 = param3 or 0
    SendProtocol(protocol)
end

function DeepDarkFantasyCtrl:RecvNetherWorldPurgatorySigleInfo(protocol)
    -- LogDG("健哥哥这是幽冥炼狱的单个下发哦?2632",protocol)
    self.data:SetChallengeSingleInfo(protocol)
end

function DeepDarkFantasyCtrl:OnGetPersonRankListAck(protocol)
    self.data:SetChallengeRankList(protocol)
end

function DeepDarkFantasyCtrl:RecvNetherWorldPurgatoryChapterInfo(protocol)
    -- LogDG("健哥哥这是幽冥炼狱的单个章节下发哦?2634",protocol)
    self.data:SetChallengeSingleChapterInfo(protocol)
end

function DeepDarkFantasyCtrl:RecvNetherWorldPurgatoryMiLingInfo(protocol)
    -- LogDG("健哥哥这是幽冥炼狱的密令下发哦?2633",protocol)
    self.data:SetMiLingInfo(protocol)
end

function DeepDarkFantasyCtrl:DeepDarkFantasyChallenge()
    if self.data:GetTodayTimes() <= 0 then 
        PublicPopupCtrl.Instance:Center(Language.DeepDarkFantasy.TodayTimeAllPass)
        return 
    end 
    
    ViewMgr:CloseView(ActFormationView)
    self.data.view_data.quick_challenge = self.data.view_data.quick_challenge +1
end

function DeepDarkFantasyCtrl:PetOnLimitNum()
    return 4
end

--布阵界面助战信息文字
function DeepDarkFantasyCtrl:AssistTextFunc(assist_num)
    return string.format(Language.CloudArena.FormationAssistInfo, assist_num)
end

function DeepDarkFantasyCtrl:OnUnloadGameLogic()
    self.data:ClearData()
end

function DeepDarkFantasyCtrl:VideoShowComplete()
    self.data.challenge_data.video_show = 1
    self:SendOperateReq(4)
end