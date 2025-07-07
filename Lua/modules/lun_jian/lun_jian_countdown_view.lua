LunJianCountdownView = LunJianCountdownView or DeclareView("LunJianCountdownView", "lun_jian/lun_jian_countdown")
-- VIEW_DECLARE_LEVEL(LunJianSignupView,ViewLevel.L1)
VIEW_DECLARE_MASK(LunJianCountdownView,ViewMask.Block)
--Number========SpriteText
--NumAnim=======Animator
function LunJianCountdownView:LunJianCountdownView()
    self.end_ts = TimeHelper:GetServerTime() + 3
    self.count_down_second = 3
    self.timer_handler = nil
end

function LunJianCountdownView:LoadCallback(param_t)
    -- LogError("PPP===",param_t)
    self.end_ts = param_t.ts
    self.count_down_second = param_t.second
end

function LunJianCountdownView:OpenCallback()
    local cur_time = self.count_down_second
    self.timer_handler = TimeHelper:AddCountDownCT(function() 
        self:flushNumber(cur_time)
        cur_time = cur_time - 1
    end,function() 
        TimeHelper:CancelTimer(self.timer_handler)
        self.timer_handler = nil
        ViewMgr:CloseView(LunJianCountdownView)
    end,self.end_ts,1,true)
end

function LunJianCountdownView:CloseCallback()
    if self.timer_handler ~= nil then
        TimeHelper:CancelTimer(self.timer_handler)
        self.timer_handler = nil
    end
end

function LunJianCountdownView:flushNumber(num)
    self.Number.text = tostring(num)
    self.NumAnim:SetTrigger(APH("countdown"))
end
