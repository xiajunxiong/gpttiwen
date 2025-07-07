
FILE_REQ('modules/sanxianguidong/sanxianguidong_data')

VIEW_REQ('modules/sanxianguidong/sanxianguidong_three')
VIEW_REQ('modules/sanxianguidong/sanxianguidong_four')
VIEW_REQ('modules/sanxianguidong/sanxianguidong_five')
VIEW_REQ('modules/sanxianguidong/sanxianguidong_view')

SanXianGuiDongCtrl = SanXianGuiDongCtrl or BaseClass(BaseCtrl)
function SanXianGuiDongCtrl:__init()
    if SanXianGuiDongCtrl.Instance ~= nil then
        Debuger.LogError('[SanXianGuiDongCtrl] attempt to create singleton twice!')
        return
    end
    SanXianGuiDongCtrl.Instance = self
    self.data = SanXianGuiDongData.New()

    -- self:RegisterProtocol(CSSanXianGuiDongReq)
    -- self:RegisterProtocol(SCSanXianGuiDongInfo, 'SCSanXianGuiDongInfo')
    -- self:RegisterProtocol(SCSanXianGuiDongPlayEndInfo, 'SCSanXianGuiDongPlayEndInfo')
end
function SanXianGuiDongCtrl:OnInit()    
    -- Remind.Instance:Register(Mod.PeriodActivity.SanXianGuiDong,self.data.info ,function ()
    --     if PeriodActivityData.Instance:IsOpen(Mod.PeriodActivity.SanXianGuiDong) then
    --         return self.data:IsOutCount() and 0 or 1
    --     else
    --         return 0
    --     end
    -- end)
    -- PeriodActivityData.Instance:CustomCreateHandle(
    --     Mod.PeriodActivity.SanXianGuiDong,
    --     BindTool.Bind(self.OnActOpen, self)
    -- )
    -- PeriodActivityData.Instance:CustomDeleteHandle(
    --     Mod.PeriodActivity.SanXianGuiDong,
    --     BindTool.Bind(self.OnActOver, self)
    -- )
    -- PeriodActivityData.Instance:CustomTimesHandle(
    --     Mod.PeriodActivity.SanXianGuiDong,
    --     BindTool.Bind(self.OnTimeChange, self)
    -- )
    -- ActivityRandData.Instance:CustomClickModHandle(Mod.PeriodActivity.SanXianGuiDong, BindTool.Bind1(self.OnClickActEvent, self))
    -- FinishData.Instance:PassRegister(
    --     BATTLE_MODE.BATTLE_MODE_SANXIANGUIDONG,
    --     {
    --         reward_func = function()
    --             return self.data:GetPassReward()
    --         end,
    --         desc_func = function()
    --             if self.data.final_pass_idx < 3 then
    --                 return Language.LFRabbits.Fail
    --             else
    --                 if self.data:IsOutCount() then
    --                     return string.format(Language.LFRabbits.Pass2, self.data.final_pass_idx)
    --                 else
    --                     if self.data.show_title == true then
    --                         return string.format(Language.LFRabbits.TitleTip, self.data.final_pass_idx)
    --                     else
    --                         return string.format(Language.LFRabbits.Pass, self.data.final_pass_idx)
    --                     end
    --                 end
    --             end
    --         end,
    --         show_type = function()
    --             if self.data.final_pass_idx == 0 then
    --                 return 5
    --             else
    --                 return 1
    --             end
    --         end
    --     }
    -- )
end
function SanXianGuiDongCtrl:OnClickActEvent()
    -- SanXianGuiDongCtrl.FindNpc()
end
function SanXianGuiDongCtrl:OnActOpen()
    -- SanXianGuiDongCtrl.ReqInfo()
    VoMgr:AddNpc(self.data.other_info.npc_id)
end
function SanXianGuiDongCtrl:OnActOver()
    VoMgr:DeleteNpc(self.data.other_info.npc_id)
end
function SanXianGuiDongCtrl:OnTimeChange()
    local num = self.data.other_info.reward_time - self.data.info.today_times
    return num < 0 and 0 or num, self.data.other_info.reward_time
end
function SanXianGuiDongCtrl:__delete()
end
function SanXianGuiDongCtrl:SCSanXianGuiDongInfo(protocol)
    -- if IS_EDITOR then
    --     LogError("三仙归洞 Info",protocol)
    -- end
    if protocol.today_play_times == self.data.other_info.reward_time and protocol.today_play_times - 1 == self.data:ActInfo().today_times then
        self.data.just_equil_max = true
    else
        self.data.just_equil_max = false
    end
    self.data:ActInfo().today_times = protocol.today_play_times
    self.data:ActInfo().pass_idx = protocol.pass_idx
    if self.data:Game().pass_idx <= 1 then
        self.data:Game().pass_idx = protocol.pass_idx + 1
    end
    -- self.data:ActInfo():Notify()
end

function SanXianGuiDongCtrl:SCSanXianGuiDongPlayEndInfo(protocol)
    -- LogDG("SCSanXianGuiDongPlayEndInfo", protocol)
    if ViewMgr:IsOpen(SanXianGuiDongView) then
        if protocol.game_type == 2 then
            if protocol.pass_idx == 1 then
                --完全胜利是否显示称号
                self.data.final_pass_idx = self.data.final_pass_idx + 1
                if self.data.title_flag ~= protocol.sanxian_title_flag then
                   self.data.show_title = true
                   self.data.title_flag = protocol.sanxian_title_flag
                end
                self.data:Game().state = 0.2
                return
            end
            self.data.title_flag = protocol.sanxian_title_flag
            --胜利需判断是否切换关卡
            local is_change = self.data:IsChangeMode(protocol.pass_idx)
            self.data.final_pass_idx = protocol.pass_idx - 1
            if is_change then
                self.data.cache_pass_idx = protocol.pass_idx
            else
                self.data:Game().pass_idx = protocol.pass_idx
            end
            self.data:Game().state = 0.1
        elseif protocol.game_type == 1 then
            self.data:Game().state = 1.1
            -- SanXianGuiDongCtrl.EndGame()
        else
            self.data:Game().state = protocol.game_type --0 游戏种， 1 失败， 2胜利
            self.data:Game().pass_idx = protocol.pass_idx
        end
    end
    self.data.cache_game_type = protocol.game_type --0 游戏种， 1 失败， 2胜利
    self.data:Game().choose_tamp = protocol.choose_tamp
    -- self.data:Game():Notify()
end
--- 0 信息 1 开始 2选择 是否正确，1正确 2错误 3 进场景
function SanXianGuiDongCtrl:SendSanXianOp(op_type, param)
    local protocol = GetProtocol(CSSanXianGuiDongReq)
    protocol.op_type = op_type or 0
    protocol.param = param or 0
    SendProtocol(protocol)
end
function SanXianGuiDongCtrl.ReqInfo()
    SanXianGuiDongCtrl.Instance:SendSanXianOp(0, 0)
end
function SanXianGuiDongCtrl.StartGame()
    -- SanXianGuiDongCtrl.Instance:SendSanXianOp(1, 0)
    -- 10 秒倒计时
    local protocol = GetProtocol(SCSanXianGuiDongPlayEndInfo)
    protocol.game_type = 0--0 --0 游戏种， 1 失败， 2胜利
	protocol.pass_idx = SanXianGuiDongData.Instance:Game().pass_idx
	protocol.choose_tamp = TimeCtrl.Instance:GetServerTime() + 10
	protocol.sanxian_title_flag = 0
    SanXianGuiDongCtrl.Instance:SCSanXianGuiDongPlayEndInfo(protocol)
end
function SanXianGuiDongCtrl.ReqResult(flag)
    -- SanXianGuiDongCtrl.Instance:SendSanXianOp(2, flag and 1 or 2)
    local protocol = GetProtocol(SCSanXianGuiDongPlayEndInfo)
    protocol.game_type = flag and 2 or 1--0 --0 游戏种， 1 失败， 2胜利
	protocol.pass_idx = SanXianGuiDongData.Instance:Game().pass_idx + 1
	protocol.choose_tamp = TimeCtrl.Instance:GetServerTime() + 10
	protocol.sanxian_title_flag = 0
    if SanXianGuiDongData.Instance:IsMaxLevel(protocol.pass_idx) then
        protocol.pass_idx = 1
    end
	-- LogError("收到服务端信息", self)
    SanXianGuiDongCtrl.Instance:SCSanXianGuiDongPlayEndInfo(protocol)
end
function SanXianGuiDongCtrl.EnterFB(param)
    if TeamData.Instance:InTeam() then
        PublicPopupCtrl.Instance:Center(Language.LFRabbits.InTeam)
        return
    else
        TeamCtrl.Instance:SendTeamMatch()
        MainViewData.Instance:ToggleTTTask()
    end
    local data = LookingForRabbitsData.Instance
    if data:IsOutCount() then
        PublicPopupCtrl.Instance:Center(Language.LFRabbits.CountOut)
    -- return
    end
    data:Clear()
    SanXianGuiDongCtrl.Instance:SendSanXianOp(0, 0)
    SanXianGuiDongCtrl.Instance:SendSanXianOp(3, 0)
end
function SanXianGuiDongCtrl.EndGame()
    -- PublicPopupCtrl.Instance:Center("全展示结束")
    SanXianGuiDongData.Instance:Clear()
    -- SceneCtrl.Instance:RequestLeaveFb()
    -- PublicPopupCtrl.Instance:CloseDialogTips()
    -- ViewMgr:CloseView(LookingForRabbits)
    -- FinishCtrl.Instance:Finish({is_win = 1}, BATTLE_MODE.BATTLE_MODE_SANXIANGUIDONG)
    --修改为重新开始

end
function SanXianGuiDongCtrl.FindNpc()
    SceneLogic.Instance:AutoToNpc(LookingForRabbitsData.Instance.other_info.npc_id,nil,false,true)
end