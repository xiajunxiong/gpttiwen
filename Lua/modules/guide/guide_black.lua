GuideBlack = GuideBlack or DeclareView("GuideBlack", "guide/guide_black", 0, ViewLayer.Top + 1)
VIEW_DECLARE_MASK(GuideBlack, ViewMask.Block)
VIEW_DECLARE_LEVEL(GuideBlack, ViewLevel.L1)
function GuideBlack:GuideBlack()
    self.play_count = 1
end
function GuideBlack:LoadCallback()
    AudioMgr:StopBg()
    self.Anim1:SetTrigger(APH("show"))
    self.play_count = self.play_count - 1
    --self.Content:ShowContent("蚩尤兄，你竟然。。。。。。", 0.3)
    self.Content:ShowContent("最终大战，由于最后关头蚩尤的叛变，人族惨败，为了继续对抗妖族，人类不得已开启了远古封印，祈祷位面英雄降临，共同对抗妖族，拯救这个濒临崩溃的世界！", 0.1)
    self.Anim1:ListenEvent(
        "hide",
        function(eve_param)
            if eve_param == "end" then
                ViewMgr:CloseView(GuideBlack)
            end
        end
    )
end
function GuideBlack:GoToCreate()
    LoginCtrl.Instance.story_round = -1
    BattleCtrl.Instance:GetStream():Clear()
    BattleData.Instance:Clear()
    SceneCtrl.Instance:GotoScene(RoleCreateScene)
end
function GuideBlack:CloseCallback()
end
function GuideBlack:PlayEnd()
    if self.play_count == 0 then
        self.Anim1:SetTrigger(APH("hide"))
        self:GoToCreate()
        return
    end
    self.play_count = self.play_count - 1

    --[[     self.Content:ShowContent(
        "妖界和人界最终大战后，人族惨败，妖族大举入侵，人族生存之地已日趋减少。为了对抗妖族，不得已人类开启了封印，获得世界之力加持，从而能从各位面召唤英雄降临，共同对抗妖族！",
        0.05
    ) ]]
end
