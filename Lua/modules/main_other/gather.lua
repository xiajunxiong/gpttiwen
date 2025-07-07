--采集界面
GatherView = GatherView or DeclareView("GatherView", "main_other/gather", 0, ViewLayer.Normal - 1)

VIEW_DECLARE_LEVEL(GatherView, ViewLevel.Lt)
VIEW_DECLARE_MASK(GatherView, ViewMask.None)

function GatherView:GatherView()
    self.data = GatherData.Instance
end

function GatherView:OnFlush()
    if MainPlayer then
        self.cur_pos = MainPlayer:GetPos()
        self:FlushGatherView()
    end
end

--刷新采集文字图标
function GatherView:FlushGatherView()
    self.info = self.data:GetGatherInfo()
    local config = self.data:GetGatherConfig(self.info.desc)
    if ChAnimType[config.anim] then
        MainPlayer:PlayAnim(ChAnimType[config.anim])
    end
    UH.SetText(self.Desc, config.text)
    self[config.icon]:SetActive(true)
    local time = self.data.need_time.time
    self:PlayGatherView(time == 0 and self.info.time or time)
end

--采集进度条
function GatherView:PlayGatherView(time)
    local start_time = TimeManager.RealTime()
    self.gather_time_handle = Runner.Instance:RunUntilTrue(function()
        local cur_time = TimeManager.RealTime()
        local percent = (cur_time - start_time) / time
        self.Prog.fillAmount = percent
        if percent >= 1 then
            self:FlushEndView()
            return true
        end
    end)
end

--采集结束
function GatherView:FlushEndView()
    local same_pos = self.cur_pos == MainPlayer:GetPos()
    self.Prog.fillAmount = 1
    if self.info and self.info.finish_func and same_pos then
        self.info.finish_func()
        self.info.finish_func = nil
    end
    ViewMgr:CloseView(GatherView)
end

function GatherView:CloseCallback()
    if MainPlayer ~= nil then
        if not MainPlayer:IsInAnim(ChAnimType.Run) then
            MainPlayer:PlayAnim(ChAnimType.Idle)
        end
        local same_pos = self.cur_pos == MainPlayer:GetPos()
        if self.info and self.info.finish_func and same_pos then
            self.info.finish_func()
            self.info.finish_func = nil
        end
    end
    if self.gather_time_handle then
        Runner.Instance:RemoveRunUntilTrue(self.gather_time_handle)
    end
    if self.info and self.info.seq ~= 0 and self.info.is_client == nil then
        SceneCtrl.RequestGatherFinsh(self.info.seq,self.info.obj_id,self.info.is_auto)
    end
end