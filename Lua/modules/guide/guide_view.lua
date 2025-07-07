GuideView = GuideView or DeclareView("GuideView", "guide/guide_view", 0, ViewLayer.Top + 2)
VIEW_DECLARE_LEVEL(GuideView, ViewLevel.Lt)
VIEW_DECLARE_MASK(GuideView, ViewMask.None)
VIEW_DECLARE_CACHE(GuideView)
function GuideView:GuideView()
    GuideManager.Instance:SetGuideView(self)
    self.step_cfg = nil
    self.step_timer = nil

    self.is_close = false
end
function GuideView:SetStepCfg(step_cfg)
    self.step_cfg = step_cfg
end
function GuideView:OnFlush()
    --这里提供一个起始事件
    EventSys:Fire(GlobalEventKeys.GuideViewOpen)
end
function GuideView:OnClick()
    if self.event ~= nil then
        self.event()
    end
end

function GuideView:OnPressDown()
    TimeHelper:CancelTimer(self.timer_dt)
    if self.step_cfg and self.step_cfg.step_id == 55 and self.step_cfg.index == 3 then
        self.timer_dt = TimeHelper:AddDelayTimer(function ()
            self:Hide()
            GuideManager.Instance:Finish()
			SocietyData.Instance.quick_chat_info.is_editing = true
            SocietyData.Instance.quick_chat_info.cur_sel = ""
		end, 1)
    end
end

function GuideView:OnPressUp()
    TimeHelper:CancelTimer(self.timer_dt)
end

function GuideView:OnClickFinish()
    if self.step_cfg and not StringIsEmpty(self.step_cfg.is_finger_effect)
     and self.step_cfg.is_finger_effect == 2 then
        self:OnClick()
         return
     end
    if self.step_cfg and self.step_cfg.is_open == 0 then
    else
        self:Hide()
        GuideManager.Instance:Finish()
    end
end
function GuideView:LoadCallback()
    self.is_close = false
end
function GuideView:Show(ui, clickType)
    -- if not GuideManager.Instance:IsGuide() then return end 
    if clickType == 2 then
        self.ClickLock:SetParent(ui,false)
        UH.LocalPosG(self.ClickLock, Vector3.zero)
        UH.AnchoredPosition(self.ClickLock:GetComponent(typeof(UnityEngine.RectTransform)), Vector2.zero)

        local lock_pos = self.gameObject.transform:InverseTransformPoint(self.ClickLock.transform.position)
        UH.LocalPosG(self.ClickTask, lock_pos)

        self.ClickTask:SetActive(true)
        UH.LocalPosG(self.ClickTask, lock_pos)
        self.ClickObj:SetActive(false)
        self.TaskModal:SetActive(true)
        self.DescObj.transform:SetAsLastSibling()
    else
        self.ClickTask:SetActive(false)
        if self.delayShow ~= nil then 
            TimeHelper:CancelTimer(self.delayShow)
            self.delayShow = nil
        end 
        if self.is_close then return end 
        
        self.delayShow = TimeHelper:AddDelayFrameTimer(function ()
            if self.is_close then return end 
            --TempCode
            if self.step_timer ~= nil then
                TimeHelper:CancelTimer(self.step_timer)
                self.step_timer = nil
            end

            local func = function ()
                if not self.ClickLock:Equals(nil) and not self.gameObject:Equals(nil) then 
                    local lock_pos = self.gameObject.transform:InverseTransformPoint(self.ClickLock.transform.position)
                    UH.LocalPosG(self.ClickObj, lock_pos)
                else 
                    if self.step_timer ~= nil then
                        TimeHelper:CancelTimer(self.step_timer)
                        self.step_timer = nil
                    end
                end 

            end

            self.step_timer = TimeHelper:AddCountDownTT(
                func,func,
                TimeManager.GetServerTime() + 5,
                0.1,nil
            )
            
            if ui == nil or ui:Equals(nil) then
                BuglyReport("ui obj is Destroy guide mistake id ",GuideManager.Instance.guide.id,"step",GuideManager.Instance.step)
                self:Hide()
                GuideManager.Instance:Finish()
                return
            end
            
            self.ClickLock:SetParent(ui,false)
            UH.LocalPosG(self.ClickLock, Vector3.zero)
            UH.AnchoredPosition(self.ClickLock:GetComponent(typeof(UnityEngine.RectTransform)), Vector2.zero)
    
            func()
            

            local v2 = UH.SizeDelta(ui)
            UH.SizeDelta(self.ClickObj, Vector2.New(v2.x + 26,v2.y + 26))
            UH.SizeDelta(self.ClickModal, Vector2.New(-18,-18))
            if self.step_cfg and not StringIsEmpty(self.step_cfg.is_finger_effect) then
                self.ClickFinger:SetActive(true)
            end
            self.ClickObj:SetActive(true)
            self.ClickModal:SetActive(true)
            self.DescObj.transform:SetAsLastSibling()
            -- 增加对圈遮罩的显隐逻辑
            self.Mask:SetActive(self.step_cfg.is_modal == "")
        end,1)
    end
end
function GuideView:BtnClickReady()
    UH.SizeDelta(self.ClickObj, Vector2.zero)
    UH.SizeDelta(self.ClickModal, Vector2.zero)
    self.ClickObj:SetActive(true)
    self.ClickFinger:SetActive(false)
    self.ClickModal:SetActive(true)
end
function GuideView:CancelStepTimer()
    if self.step_timer ~= nil then
        TimeHelper:CancelTimer(self.step_timer)
        self.step_timer = nil
    end
    self:StopFingerAction()
end

function GuideView:ShowDesc(config)

    UH.SetText(self.Desc, config.arrow_tip)
    self.DescObj:SetActive(true)
    if config.offset_x ~= "" and config.offset_y ~= "" then
        UH.AnchoredPosition(self.DescPos, Vector2.New(config.offset_x,config.offset_y))
    end
end
function GuideView:ShowOnlyDesc(config)
    UH.SetText(self.DescLeftTex, config.arrow_tip)
    UH.SetText(self.DescRightTex, config.arrow_tip)
    self.DescLeft:SetActive(config.arrow_dir == 1)
    self.DescRight:SetActive(config.arrow_dir ~= 1)
    self.DescModal:SetActive(true)
    if config.offset_x ~= "" and config.offset_y ~= "" then
        UH.AnchoredPosition(self.DescPos2, Vector2.New(config.offset_x,config.offset_y))
    end
    self.OnlyDesc:SetActive(true)
end
-- 添加点击监听
function GuideView:AddEvent(event)
    self.event = event
end
function GuideView:Hide()
    self.Mask:SetActive(false)
    TimeHelper:CancelTimer(self.delayShow)
    if not self.ClickLock:Equals(nil) and self.ClickLock ~= nil then 
        self.ClickLock:SetParent(self.gameObject)
    else 
        if GuideManager.Instance:IsGuide() then 
            LogError("fail hide,mark step_id",GuideManager.Instance.guide.id,"step",GuideManager.Instance.step)
        end 
    end 
    self.ClickObj:SetParent(self.gameObject)--先这样改
    UH.AnchoredPosition(self.DescPos,Vector2.zero)
    UH.AnchoredPosition(self.DescPos2,Vector2.zero)
    self.DescObj:SetActive(false)
    self.ClickObj:SetActive(false)
    self.ClickTask:SetActive(false)
    --单独显示描述时打开DescModal
    self.DescModal:SetActive(false)
    self.TaskModal:SetActive(false)
    self.ClickModal:SetActive(false)
    self.OnlyDesc:SetActive(false)
    self.event = nil
end
function GuideView:CloseCallback()
    EventSys:Fire(GlobalEventKeys.GuideViewClose)

    if self.step_timer ~= nil then
        TimeHelper:CancelTimer(self.step_timer)
        self.step_timer = nil
    end

    if self.delayShow ~= nil then 
        TimeHelper:CancelTimer(self.delayShow)
        self.delayShow = nil
    end 
    self.is_close = true
end
function GuideView:StartFingerAction(p1, p2)
    local total = 56
    local value = 0
    local value2 = 0
    self.FingerPos.gameObject:SetActive(true)
    self.handle_run = TimeHelper:AddRunFrameTimer(function ()
        UH.AnchoredPosition(self.FingerPos, Vector2.Lerp(p1, p2, value / (total / 2)))
        value = value + 1
        value2 = value2 + 1
        if value == (total / 2) then
            value = 0
        end
        if value2 >= total then
            self.FingerPos.gameObject:SetActive(false)
        end
    end, 1, total)
end
function GuideView:StopFingerAction()
    TimeHelper:CancelTimer(self.handle_run)
    UH.AnchoredPosition(self.FingerPos, Vector2.zero)
end
Story1 = {

    talk_list = {
        [1] = {
            talker = 0,
            talk_text = "我们也是为了生存，既然一定要走到这一步，那就一决胜负吧！我会给你们找一个好的墓地埋葬！"
        },
        [2] = {
            talker = 0,
            talk_text = "非我族类，其心必异，这片古老的土地不容你们践踏！"
        },
        [3] = {
            talker = 0,
            talk_text = "那就没得谈了，看招吧！",
        }
    }
}

Story2 = {
    talk_list = {
        [1] = {
            talker = 0,
            talk_text = "喻兄，对方人多，你专注清扫杂兵，剩下的交给我们"
        },
        [2] = {
            talker = 0,
            talk_text = "明白，不负所托"
        }
    }
}
Story3 = {
    talk_list = {
        [1] = {
            talker = 0,
            talk_text = "蚩尤兄，还不出手？更待何时？"
        },
        [2] = {
            talker = 0,
            talk_text = "天地不仁，万物总有归元时，死魂，别忘记你的承诺！"
        },
        [3] = {
            talker = 0,
            talk_text = "蚩尤，你要干嘛？现在不是胡来的时候！"
        },
        [4] = {
            talker = 0,
            talk_text = "让历史，就此改写吧。。"
        }
    }
}

GuideClearEffectView = GuideClearEffectView or DeclareView("GuideClearEffectView", "guide/guide_clear_effect", Mod.Guide.ClearEffect)
function GuideClearEffectView:GuideClearEffectView()
    GuideManager.Instance:RegisterGetGuideUi("GuideCleanBtn",function ()
        return self.ClickClean, function ()
            self:OnClickEffect()
        end
    end)
end

function GuideClearEffectView:LoadCallback()
    self.bad_eff_handle = self.EffectTool:Play(5185000)
    self.clear_eff_handle = self.EffectTool:Play(5185002)
end

function GuideClearEffectView:CloseCallback()
    GuideManager.Instance:UnRegisterGetGuideUi("GuideCleanBtn")
end

function GuideClearEffectView:OnClickEffect()
    if self.good_eff_handle then
        if self.played then
            ViewMgr:CloseView(GuideClearEffectView)
        end
    else
        self.good_eff_handle = self.EffectTool:Play(5185001)
        self.EffectTool:Stop(self.clear_eff_handle)
    end
end

function GuideClearEffectView:OnEffectTime()
    self.EffectTool:Stop(self.bad_eff_handle)
    TimeHelper:AddDelayTimer(function ()
        self.played = true
    end, 1)
end