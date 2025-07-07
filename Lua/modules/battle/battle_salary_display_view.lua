BattleSalaryDisplayView = BattleSalaryDisplayView or DeclareView("BattleSalaryDisplayView", "battle/battle_salary_display")
VIEW_DECLARE_MASK(BattleSalaryDisplayView,ViewMask.None)
--KeepTime======int(ms)
--TitleTxt======Text
--ValTxt========Text
--Eff===========GameObject

function BattleSalaryDisplayView:LoadCallback(param_t)
    local knight_list = TeamData.Instance:GetKnightlyList()
    local cfg = knight_list[param_t.seq]
    -- if cfg == nil then
    --     ViewMgr:CloseView(BattleSalaryDisplayView)
    --     return
    -- end
    cfg = cfg and cfg.info or nil
    self.TitleTxt.text = cfg and cfg.fuben_name or ("UnknowSEQ[" .. param_t.seq .. "]")
    self.ValTxt.text = string.format(Language.Main.Battle.SalaryDisplayVal,param_t.salary)
    self.delay_handle = TimeHelper:AddDelayFrameTimer(function()
        self.delay_handle = nil
        self.Eff:SetActive(true)
    end
        , 1)
    
end

function BattleSalaryDisplayView:OpenCallback()
    -- TimeHelper:AddDelayTimer(function ()
        ViewMgr:CloseView(BattleSalaryDisplayView)
    -- end,self.KeepTime/1000)
end

function BattleSalaryDisplayView:CloseCallback()
    if self.delay_handle then
        TimeHelper:CancelTimer(self.delay_handle)
        self.delay_handle = nil
    end
end