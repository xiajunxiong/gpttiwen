FishView = FishView or DeclareView('FishView', 'fish/fish_view')
VIEW_DECLARE_MASK(FishView, ViewMask.Block)
function FishView:FishView()
end
function FishView:LoadCallback()
    ViewMgr:MainViewVisible(false)
end
function FishView:CloseCallback()
    ViewMgr:MainViewVisible(true)
end

FishPanel = FishPanel or DeclareMono('FishPanel', UIWFlushPanel)
-- 76 - 104
function FishPanel:FishPanel()
    self.data = FishData.Instance
    self.data_cares = {
        {data = self.data:Fish(), func = self.FlushFishingState, key = {'state'}}
    }
    -- 钓鱼提示 模型变大 缓冲点击
    self.prve_time = 0
end
function FishPanel:FlushFishingState()
    local state = self.data:Fish().state
    if state == FishData.State.Init then
        MainPlayer:PlayAnim(ChAnimType.FishIdle)
        --播待机切镜头
        self:InitFish()
    elseif state == FishData.State.Wait then
        MainPlayer:PlayAnim(ChAnimType.PaoGan)
        MainPlayer:PlayAnim(ChAnimType.FishWait)
        self:WaitBite()
    elseif state == FishData.State.Fishing then
        MainPlayer:PlayAnim(ChAnimType.Lagan)
        MainPlayer:PlayAnim(ChAnimType.LaganIdle)
        self:Fishing()
    end
end
-- 5-10秒随机咬钩 3秒后重新 随机咬钩
-- 如果点击咬钩 6秒过程 后恢复待机
function FishPanel:OnClickFishing()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)

    local state = self.data:Fish().state
    if state == FishData.State.Init then
        if os.time() - self.prve_time < 1.1 then
            self.prve_time = os.time()
            PublicPopupCtrl.Instance:Center(Language.Live.TooFast2)
            return
        end
        self.prve_time = os.time()

        --进行抛竿和切换界面
        self.data:Fish().state = FishData.State.Wait
    elseif state == FishData.State.Wait then
        if os.time() - self.prve_time < 1.1 then
            self.prve_time = os.time()
            PublicPopupCtrl.Instance:Center(Language.Live.TooFast2)
            return
        end
        self.prve_time = os.time()

        --告诉服务端开始钓鱼了
        if self.is_bite == true then
            FishCtrl.CSFishGroundReq(FishCtrl.OpType.Start)
        else
            MainPlayer:PlayAnim(ChAnimType.ShouGan)
            self.data:Fish().state = FishData.State.Init
            TimeHelper:CancelTimer(self.handle_fishnone)
            --鱼儿未上钩
            self.handle_fishnone = TimeHelper:AddDelayTimer(function ()
                AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaShiBai)
                self.NoBiteAnim:Play()
            end, 1)
        end
        -- self.data:Fish().state = FishData.State.Fishing
    elseif state == FishData.State.Fishing then
        --判断游标位置生成特效 ，特效播放完后判断是否钓到鱼
        -- self:Pause()
        -- self.data:Fish().state = FishData.State.Init
        -- OperateFrequency.Operate()--进行掉血

        OperateFrequency.Operate(BindTool.Bind(self.FishingRod,self),"FishingRod",self.data.other.one_way_time, Language.Live.TooFast)
        -- self:FishingRod()
    end
end

function FishPanel:FishingRod()
    -- PublicPopupCtrl.Instance:Center('我拉')
    self.RodEffPar:SetParent(self.RodObj)
    UH.AnchoredPosition(self.RodEffParRect, Vector2.New(0, 40.6))
    UH.LocalRotate(self.RodEffParRect, Quaternion.Euler(0,0,0))
    self.RodEffPar:SetParent(self.FishingObj)
    self.RodEffPar:SetActive(true)
    self.EffectTool:Play(3161055)
    local value = self.Pointer.eulerAngles.z
    AudioMgr:PlayEffect(AudioType.UI,"chushixingxing")
    if value >= 76 and value <= 104 then
        self.pull_time = self.pull_time + 1
        self.FishHp.fillAmount = 1 - (self.pull_time / self.data.other.success_times)
        self:Pause()
        self.handle_pause = TimeHelper:AddDelayTimer(BindTool.Bind(self.Pause, self), 0.05)
        if self.pull_time == self.data.other.success_times then
            -- self.data:Fish().state = FishData.State.Init
            self.GetFishAnim:Play()
            MainPlayer:PlayAnim(ChAnimType.ShouGan)
            FishCtrl.CSFishGroundReq(FishCtrl.OpType.GetFish)
        end
    else
        self:Stop()
        TimeHelper:CancelTimer(self.handle_runawayanim)
        self.handle_runawayanim = TimeHelper:AddDelayTimer(function ()
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaShiBai)
            self.RunAwayAnim:Play()
        end, 1)
    end
end
-- 🐟🐟🐟🐟🐟🐟🐟🐟🐟
-- 🐟🐟🐟🐟🐟🐟🐟🐟🐟
function FishPanel:Pause()
    if self.Anim:get_Item('youbiao').speed == 0 then
        self.Anim:get_Item('youbiao').speed = 1
    else
        self.Anim:get_Item('youbiao').speed = 0
    end
end
function FishPanel:OnDestroy()
    TimeHelper:CancelTimer(self.handle_fishnone)
    TimeHelper:CancelTimer(self.handle_runawayanim)
    TimeHelper:CancelTimer(self.handle_runawayanim2)
    UIWFlushPanel.OnDestroy(self)
    self:Clear()
    self.data:ClearSetting()
end
function FishPanel:OnClickClose()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    self:Clear()
    self.data:Fish().state = FishData.State.None
    ViewMgr:CloseView(FishView)
    
    -- FishCtrl.CSFishGroundReq(FishCtrl.OpType.Stop)
end
function FishPanel:InitFish()
    -- LogDG('打开钓鱼界面，当前是手拿鱼竿状态，收杆状态')
    self:Clear()
end
function FishPanel:Clear()
    UH.SpriteName(self.BtnIcon, "0001")
    self.is_bite = false
    self.FishHp.fillAmount = 1
    self.Anim:get_Item('youbiao').speed = 1
    self.pull_time = 0
    self.WaitObj:SetActive(false)
    self.FishingObj:SetActive(false)
    self.RodEffPar:SetActive(false)
    TimeHelper:CancelTimer(self.handle_pause)
    TimeHelper:CancelTimer(self.handle_wait)
    TimeHelper:CancelTimer(self.handle_bite)
    TimeHelper:CancelTimer(self.handle_fishing)
    TimeHelper:CancelTimer(self.handle_countdown)
    -- TimeHelper:CancelTimer(self.handle_fishnone)
    if self.bite_effect ~= nil then
        self.EffectTool:Stop(self.bite_effect)
        self.bite_effect = nil
    end
end

function FishPanel:WaitBite()
    self:Clear()
    UH.SpriteName(self.BtnIcon, "FuPiaoTuBiao")
    self.WaitObj:SetActive(true)
    local wait_time = self.data:WaitTime()
    self.handle_wait = TimeHelper:AddDelayTimer(BindTool.Bind(self.Bite, self), wait_time)
    if self.bite_effect ~= nil then
        self.EffectTool:Stop(self.bite_effect)
        self.bite_effect = nil
    end
    -- LogDG('等待', wait_time, '秒后鱼会咬钩')
end

function FishPanel:Bite()
    self:Clear()
    UH.SpriteName(self.BtnIcon, "FuPiaoTuBiao")
    self.is_bite = true
    self.handle_bite = TimeHelper:AddDelayTimer(BindTool.Bind(self.WaitBite, self), self.data.other.unclick_time)
    -- if self.bite_effect == nil then
        self.bite_effect = self.EffectTool:Play(3161060)
    -- end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ZhuangBeiFuShuJia)
    PublicPopupCtrl.Instance:Center(Language.Live.FishBite)
    -- LogDG('鱼咬钩了', self.data.other.unclick_time, '秒后如果没拉钩就重新回到等待咬钩状态')
end
function FishPanel:Fishing()
    self:Clear()
    UH.SpriteName(self.BtnIcon, "0002")
    self.FishingObj:SetActive(true)
    self.handle_fishing = TimeHelper:AddDelayTimer(BindTool.Bind(self.Stop, self), self.data.other.paly_time)
    -- LogDG('开始钓鱼了,6秒后回到收杆状态')
    local time = self.data.other.paly_time
    self.handle_countdown = TimeHelper:AddCountDownTT(function ()
        time = time - 0.03
        self.FishingBar.fillAmount = time / self.data.other.paly_time
    end,function ()
        TimeHelper:CancelTimer(self.handle_runawayanim2)
        self.handle_runawayanim2 = TimeHelper:AddDelayTimer(function ()
            AudioMgr:PlayEffect(AudioType.UI, CommonAudio.QiangHuaShiBai)
            self.RunAwayAnim:Play()
        end, 1)
    end,self.data.other.paly_time,0.03)
end
function FishPanel:Stop()
    MainPlayer:PlayAnim(ChAnimType.ShouGan)
    FishCtrl.CSFishGroundReq(FishCtrl.OpType.Stop)
    -- self.data:Fish().state = FishData.State.Init
end
