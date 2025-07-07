BoxCh = BoxCh or BaseClass(SceneCh)

function BoxCh:InitEnd()
    if self.vo.box_color == nil then
        self:PlayAnim(1 == self.vo.is_open and ChAnimType.Run or ChAnimType.Idle)
    end
end
function BoxCh:__delete()
    SE:Stop(self.handle_boxshow)
    SE:Stop(self.handle_boxopen)
    TimeHelper:CancelTimer(self.handle_opening)
    TimeHelper:CancelTimer(self.handle_close)
end
function BoxCh:OpenedBox()
    self:PlayAnim(ChAnimType.Run)
end
-- 打开宝箱带过程
function BoxCh:OpenBox()
    self:PlayAnim(ChAnimType.Idle)
    TimeHelper:AddRunFrameTimer(function ()
        self:PlayAnim(ChAnimType.BattleRun)
    end, 1)
    --我也不知道为啥要延迟一帧才可以播放出来
end
function BoxCh:BodyLoadedCallback()
    SceneCh.BodyLoadedCallback(self)
    if self.vo.box_color then
        self.handle_boxshow = SE:Play(SceneEffectConfig.EffectList.RewardBoxShow, nil, nil, self.root_node)
        self:PlayAnim(ChAnimType.Idle)
        --播放特效
        TimeHelper:CancelTimer(self.handle_opening)
        self.handle_opening = TimeHelper:AddDelayTimer(function ()
            self.handle_boxopen = SE:Play(self:OpenEffect(self.vo.box_color), nil, nil, self.body.main_body:getObj():FindChild("Bone001"), function ()
                self:OpenBox()
            end)
            if self.vo.end_func then
                TimeHelper:CancelTimer(self.handle_close)
                self.handle_close = TimeHelper:AddDelayTimer(function ()
                    self.vo.end_func()
                end, 6.167)--4.5+1.667)
            end
        end, 1.2)
    end
    if self.vo.is_ele_box then
        self.handle_boxopen = SE:Play(SceneEffectConfig.EffectList.EleBoxOpenEffect, nil, nil, self.root_node, function ()            
            self:OpenBox()
            --在打开完之后接消失动画总共大约4秒
            self.handle_close = TimeHelper:AddDelayTimer(function ()
                self.vo.end_func()
            end, 4)--1.875 + 1.667
        end)
    end
end
function BoxCh:OpenEffect(color)
    if color < ItemColor.Orange then
        return SceneEffectConfig.EffectList.RewardBoxZi
    elseif color < ItemColor.Red then
        return SceneEffectConfig.EffectList.RewardBoxCheng
    elseif color < ItemColor.Gold then
        return SceneEffectConfig.EffectList.RewardBoxHong
    elseif color >= ItemColor.Gold then
        return SceneEffectConfig.EffectList.RewardBoxCai
    end
    return SceneEffectConfig.EffectList.RewardBoxZi
end
function BoxCh:InteractionBehavior()
    local obj_vo = self.vo or {}
    if 0 == obj_vo.is_open then
        local obj = SceneCtrl.Instance:Objs():GetNetNpc(obj_vo)
        if nil ~= obj then
            local key = FightCtrl.Instance:Data():GetDungeonBoxKeyByBoxId(obj_vo.id)
            if key > 0 then
                local item = Item.New{item_id = key}
                if item:Num() == 0 then
                    local str_ = string.format(Language.Fight.NotEnoughMarket, item:Name())
                    local flag_workshop = false
                    if flag_workshop then 
                        str_ = string.format(Language.Fight.NotEnoughKeyToWorkShop, item:Name())
                    end 
                    PublicPopupCtrl.Instance:DialogTips{
                        content = str_,
                        confirm = {
                            func = function()
                                ViewMgr:CloseView(DialogTipsView)
                                if flag_workshop then 
                                    WorkshopData.Instance:QuickComposeType(ComposeGroup.Key,item.item_id)
                                    ViewMgr:OpenViewByKey(Mod.Workshop.NeoCompose)
                                else 

                                    MarketData.Instance:SetJumpIndex(item.item_id)
                                    ViewMgr:OpenViewByKey(Mod.Mall.Market)
                                end 
                            end
                        }
                    }
                else
                    if MainViewCtrl.Instance:IsInGameState(MainViewGameState.fight_fantasy) then 
                        local cur_layer,max_layer = FightData.Instance:GetFantasyLayer()
                        if FightData.Instance:GetFantasyFBRewardIsFetch(cur_layer) then 
                            PublicPopupCtrl.Instance:DialogTips{
                                content = string.format(Language.Fight.FantasyDungen.DangeonFetchTips, item:Name()),
                                confirm = {
                                    func = function()
                                        ViewMgr:CloseView(DialogTipsView)
                                        FightData.Instance:CatchFantasyBoxId(obj_vo.obj_id)
                                        SceneCtrl.Instance:SendTreasureChestReq(obj_vo.obj_id)
                                    end
                                }
                            }
                        else 
                            FightData.Instance:CatchFantasyBoxId(obj_vo.obj_id)
                            SceneCtrl.Instance:SendTreasureChestReq(obj_vo.obj_id)
                        end 
                    else 
                        SceneCtrl.Instance:SendTreasureChestReq(obj_vo.obj_id)
                    end 
                end
            else
                SceneCtrl.Instance:SendTreasureChestReq(obj_vo.obj_id)
            end
        end
    end
end