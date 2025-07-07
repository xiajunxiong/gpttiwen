
AchievementPopupView = AchievementPopupView or DeclareView("AchievementPopupView","main_other/achievement",0,ViewLayer.Top - 1)
VIEW_DECLARE_MASK(AchievementPopupView,ViewMask.None)

--ShowAnim=====Animator
--ShowTime=====int  显示时间(毫秒)

function AchievementPopupView:AchievementPopupView()
    if AchieveCtrl.Instance.popup_list[1] then
        local achi_pidx = AchieveCtrl.Instance.popup_list[1]
        table.remove(AchieveCtrl.Instance.popup_list,1)
        local ach_data = AchieveNewData.Instance
        local cfg = ach_data:GetAchieveCfg(ach_data:GetCurStage(),ach_data:GetCurStar(),achi_pidx)
        if cfg ~= nil then
            self.desc.text = cfg.name
            self.desc2.text = cfg.desc
        else
            ViewMgr:CloseView(AchievementPopupView)
        end
    end
    self.close_timer = nil
end

function AchievementPopupView:OpenCallback()
    -- self.ShowAnim:ListenEvent("show",function (key,st_info)
    --     ViewMgr:CloseView(AchievementPopupView)
    -- end)
    -- ViewMgr:CloseView(AchievementPopupView)
    self.close_timer = TimeHelper:AddDelayTimer(function() 
            self.close_timer = nil
            ViewMgr:CloseView(AchievementPopupView)
        end,self.ShowTime/1000)
end

function AchievementPopupView:CloseCallback()
    if self.close_timer ~= nil then
        TimeHelper:CancelTimer(self.close_timer)
        self.close_timer = nil
    end
    if #AchieveCtrl.Instance.popup_list ~= 0 then
        ViewMgr:OpenView(AchievementPopupView)
    end
end