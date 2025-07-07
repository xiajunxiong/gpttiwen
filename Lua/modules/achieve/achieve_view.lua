

AchieveView = AchieveView or DeclareView("AchieveView","achieve/achieve")
VIEW_DECLARE_LEVEL(AchieveView,ViewLevel.L1)


--Board=====UIWBoard
--AchieveSub====GameObject
--HonorSub====GameObject

function AchieveView:AchieveView()
    self.lang = Language.Achieve
end

function AchieveView:LoadCallback()
    AchieveCtrl.Instance:SendAchievementHonorRollInfo(1)
    self.Board:SetData(self:GetType(),self.lang.Achieve)
    self.Board:SetTabbarVisible(true)
    self.Currency:DefaultSet()

    local tabbar = self.Board.Tabbar
    tabbar:SetData({
        {name = self.lang.AchieveSub,tab_objs = {self.AchieveSub}},
        {name = self.lang.HonorSub,tab_objs = {self.HonorSub}},
    })
    tabbar:ClickItem(1)
end

function AchieveView:OpenCallback()
end

function AchieveView:CloseCallback()
end
