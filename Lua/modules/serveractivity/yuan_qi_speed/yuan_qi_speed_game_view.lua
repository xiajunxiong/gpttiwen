YuanQiSpeedGameView = YuanQiSpeedGameView or DeclareView("YuanQiSpeedGameView", "serveractivity/yuan_qi_speed/yuan_qi_speed_game")
function YuanQiSpeedGameView:LoadCallback()
    self.MainPanel:OpenCall()
end

YuanQiSpeedGamePanel = YuanQiSpeedGamePanel or DeclareMono("YuanQiSpeedGamePanel", UIWFlushPanel)
YuanQiSpeedGamePanel.GameState = {
    Wait = 0,       --准备阶段
    Playing = 1,    --游戏中
    Over = 2,       --游戏结束
}

function YuanQiSpeedGamePanel:YuanQiSpeedGamePanel()
    self.data_cares = {
        {data = YuanQiSpeedData.Instance.game_info, func = self.FlushView, init_call = false}
    }
    self.open_time = TimeHelper.GetServerTime()
    self.game_state = self.GameState.Wait
    self.game_send = false --是否发送开始协议
    self.game_send_over = false --是否发送游戏结束协议

    self.click_times = 0
end

function YuanQiSpeedGamePanel:Awake()
    UIWFlushPanel.Awake(self)
    self.Timer:SetShowCallBack(BindTool.Bind1(self.WaitTimeShow, self))
    self.Timer:SetCallBack(BindTool.Bind1(self.WaitTimeOver, self))
end

function YuanQiSpeedGamePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.stop_timer)
    self.stop_timer = nil
end

function YuanQiSpeedGamePanel:OpenCall()
    self.RoleItem:SetData({data = 1})
    self.RabbitItem:SetData({data = 1})
    self.Timer:TotalTime(6)
end

function YuanQiSpeedGamePanel:FlushView()
    local game_info = YuanQiSpeedData.Instance.game_info
    if game_info.game_type == 2 then
        if self.game_state == self.GameState.Wait then
            self.game_state = self.GameState.Playing
            self.RabbitItem:DoMove()
            self.Timer:TotalTime(6)
            self.TipsAnim.enabled = true
        end
    else
        self.game_state = self.GameState.Over
        self.game_send_over = true
        self.RabbitItem:StopMove(true)
        self.RoleItem:StopMove(true)
    end
end

function YuanQiSpeedGamePanel:WaitTimeShow(time_t)
    if self.game_state == 0 then
        return Format(Language.YuanQiSpeed.GameTime1, tonumber(time_t.s))
    end
    return Format(Language.YuanQiSpeed.GameTime2, tonumber(time_t.s))
end
function YuanQiSpeedGamePanel:WaitTimeOver()
    if self.game_state == self.GameState.Wait then
        self:SendBegin()
        PublicPopupCtrl.Instance:Center(Language.YuanQiSpeed.GameError1)
        AudioMgr:PlayEffect(AudioType.Scene, "enter_game")
    end
end

function YuanQiSpeedGamePanel:OnClickClose()
    local function func_quit()
        ViewMgr:CloseView(YuanQiSpeedGameView)
        ViewMgr:OpenView(YuanQiSpeedView)
        if ViewMgr:IsOpen(YuanQiSpeedGameView) and self.game_state ~= nil and self.game_state == self.GameState.Playing then
            self:SendOver(false)
        end
    end
    PublicPopupCtrl.Instance:AlertTip(Language.YuanQiSpeed.QuitAlert, func_quit)
end

function YuanQiSpeedGamePanel:OnClickBlock()
    if self.game_state ~= self.GameState.Playing then
        return
    end
    if self.game_send_over then
        return
    end
    self.click_times = self.click_times + 1
    self:DoMove()
    TimeHelper:CancelTimer(self.stop_timer)
    self.stop_timer = Invoke(BindTool.Bind1(self.StopMove, self), 0.1)
end

function YuanQiSpeedGamePanel:SendBegin()
    if self.game_send == false then
        YuanQiSpeedCtrl.Instance:SendBegin(YuanQiSpeedData.Instance:GetSelect())
        self.game_send = true
    end
end

function YuanQiSpeedGamePanel:StopMove()
    self.RoleItem:StopMove()
end
function YuanQiSpeedGamePanel:DoMove()
    self.RoleItem:DoMove()
end

function YuanQiSpeedGamePanel:SendOver(is_win)
    if self.game_send_over == false then
        YuanQiSpeedCtrl.Instance:SendResult(is_win and 1 or 0)
        self.game_send_over = true
    end
end

YuanQiSpeedGameItem = YuanQiSpeedGameItem or DeclareMono("YuanQiSpeedGameItem", UIWidgetBaseItem)
function YuanQiSpeedGameItem:YuanQiSpeedGameItem()
    self.delay_time = 0.1
    local seq = self.IsRole and 2 or YuanQiSpeedData.Instance:GetSelect()
    local rabbit_cfg = YuanQiSpeedData.Instance:GetRabbitCfg(seq)
    self.finish_time = rabbit_cfg.finish_time
    if self.IsRole then
        self.finish_time = self.finish_time - 0.2
    end
end

function YuanQiSpeedGameItem:InitMove()
    self.distance = self.FinishRect.localPosition - self.BeginRect.localPosition
    self.move_dis = self.distance / (self.finish_time / self.delay_time)
    UH.LocalPosG(self.gameObject, self.BeginRect.localPosition)
    -- LogError(Format("%s 赛道距离 :", (self.IsRole and "玩家" or "兔子")) .. self.distance.x)
end

function YuanQiSpeedGameItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:InitMove()
    
    if self.IsRole == true then
    else
        UH.SpriteName(self.Icon, "Rabbit" .. (YuanQiSpeedData.Instance:GetSelect() + 1))
        self.RabbitList[YuanQiSpeedData.Instance:GetSelect() + 1]:SetActive(true)
    end
end

function YuanQiSpeedGameItem:OnDestroy()
    TimeHelper:CancelTimer(self.move_timer)
    self.move_timer = nil
end

function YuanQiSpeedGameItem:DoMove()
    if self.move_timer == nil then
        self.move_timer = TimeHelper:AddRunTimer(BindTool.Bind1(self.DoMoveAnim, self), self.delay_time)
        if self.IsRole then
            self.RoleAnim.enabled = true
        else
            local index = YuanQiSpeedData.Instance:GetSelect() + 1
            self.RabbitAnimList[index].enabled = true
        end
    end
end

function YuanQiSpeedGameItem:DoMoveAnim()
    if self:IsOver() then
        TimeHelper:CancelTimer(self.move_timer)
        self.move_timer = nil
        self.MainPanel:SendOver(self.IsRole or false)
    else
        UH.LocalPosG(self.gameObject, self.gameObject.transform.localPosition + self.move_dis)
    end
end

function YuanQiSpeedGameItem:IsOver()
    return self.gameObject.transform.localPosition.x >= self.FinishRect.localPosition.x
end

function YuanQiSpeedGameItem:StopMove(is_over)
    TimeHelper:CancelTimer(self.move_timer)
    self.move_timer = nil
    if is_over == true then
        if self.IsRole then
            self.RoleAnim.enabled = false
        else
            for i = 1, 3 do
                self.RabbitAnimList[i].enabled = false
            end
        end
    end
end
