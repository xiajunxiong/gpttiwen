FightFantasyMenuView = FightFantasyMenuView or DeclareMono("FightFantasyMenuView", UIWFlushPanel)

function FightFantasyMenuView:FightFantasyMenuView()
    self.data = FightCtrl.Instance:Data()
    self.data_cares = {
        {data = self.data.fight_fantasy_info, func = self.FlushShow},
        {data = self.data.fb_reward_data, func = self.FlushRewardList,keys = {"flush_flag"}},
    }
end 

function FightFantasyMenuView:FlushShow()
    self.ResNum1.text = self.data.fight_fantasy_info.gather_num
    self.ResNum2.text = self.data.fight_fantasy_info.box_num
    
    local cur_layer,max_layer = self.data:GetFantasyLayer()
    self.ProgressTip.text = string.format( Language.Fight.FantasyDungen.Progress, cur_layer,max_layer)
    self.ClearDesc.text = self.data:GetFantasySceneDesc()
    
    self:CompleteFlush()
end

function FightFantasyMenuView:OnDestroy()
    if self.effect_play ~= nil then
        self.effect_play:DeleteMe()
    end 
end 

function FightFantasyMenuView:FlushButtonRed()
    if self.button_red == nil then 
        self.button_red = UiRedPoint.New(self.BtnTempBox, Vector3.New(75, 20), false,1)
    end  

    self.button_red:SetNum(#self.data:GetFantasyFBRewardInfo())
end

function FightFantasyMenuView:OnClickResult()
    -- ViewMgr:OpenView(FightFantasyBoxView)
    -- self.button_red:SetNum(0)
    self:PlayBoxEffect()
end

function FightFantasyMenuView:FlushRewardList()

    -- if self.data:GetFantasyBoxId() ~= nil then 
    --     self:PlayBoxEffect()
    -- else 
    -- end 
end 

function FightFantasyMenuView:PlayBoxEffect( )
    local catch_id = self.data:GetFantasyBoxId()

    if catch_id ~= nil then 
		local catchIt = nil 
		for k,v in pairs(MainViewData.Instance:SceneObjVos()) do 
			if k.obj_id == catch_id then 
                catchIt = k
				break 
			end 
        end 
        ViewMgr:SetScenePos(self.EffectStartObj.transform,catchIt.root_obj.transform.position)

        if self.effect_play == nil then  self.effect_play = FightFantasyBoxEffect.New(self) end 
        self.effect_play:Play()
        --用完就废弃
        self.data:CatchFantasyBoxId()
    end 
end

function FightFantasyMenuView:CompleteFlush()
    local cur_layer,max_layer = self.data:GetFantasyLayer()
    -- local data_list = FightData.Instance:GetFantasyFBRewardInfo()

    local data_list = FightData.Instance:GetFantasySinglePreReward(cur_layer)
    self.RewardList:SetData(data_list)

    if FightData.Instance:GetFantasyFBRewardIsFetch(cur_layer) then 
        self.RewardListObj:SetActive(false)
        self.DailyFetchTips:SetActive(true)
        self.EmptyTips:SetActive(false)
        self.EmptyShowTips:SetActive(false)
        self.ShowTips:SetActive(false)
    else 
        self.RewardListObj:SetActive(true)
        self.DailyFetchTips:SetActive(false)
        self.EmptyTips:SetActive(#data_list ==  0)
        self.EmptyShowTips:SetActive(#data_list ==  0)
        self.ShowTips:SetActive(#data_list >  0)
    end 
end

FightFantasyBoxEffect = FightFantasyBoxEffect or BaseClass()
function FightFantasyBoxEffect:__init(mono)
    self.WI = mono
end
function FightFantasyBoxEffect:__delete()
    if self.handle_run then
        Runner.Instance:RemoveRunUntilTrue(self.handle_run)
    end
    if self.handle_delay then
        TimeHelper:CancelTimer(self.handle_delay)
    end
end

function FightFantasyBoxEffect:Play()
    local time = 0
    local celleffectParent = self.WI.CellEffect
    local effectendparent = self.WI.CellEndEffect
    local effectNode = self.WI.EffectNode
    local startPos = self.WI.EffectStartObj.transform.localPosition
    local endPos = effectendparent.transform.localPosition
    local crossPos = self.WI.CrossObj.transform.localPosition
    local shoot_curve = Game.CurveParabola(startPos, endPos, crossPos, 0.5)
    
    UH.LocalPos(effectNode.transform, startPos)
    if self.WI.EffectTool.isActiveAndEnabled then 
        self.WI.EffectTool:Play(10010254, effectNode)
    end 
    self.handle_run = Runner.Instance:RunUntilTrue(
        function()
            if shoot_curve then
                time = time + Time.deltaTime * 3
                if time >= 1 then
                    -- self.WI.EffectTool:Play(10010863, celleffectParent)
                    -- AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ZhuangBeiFuShuJia)
                    self.handle_delay = TimeHelper:AddDelayTimer(
                        function()
                            self.WI:CompleteFlush()
                        end,
                        0.2
                    )
                    return true
                end
                UH.LocalPos(effectNode.transform, shoot_curve:GetPos(time))
            end
            return false
        end
    )
end 
