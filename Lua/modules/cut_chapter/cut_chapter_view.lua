CutChapterView = CutChapterView or DeclareView("CutChapterView","cut_chapter/cut_chapter",Mod.CutChapter.Main,ViewLayer.Top)
VIEW_DECLARE_MASK(CutChapterView,ViewMask.None)

function CutChapterView:LoadCallback()
    -- self.EffectTool:Play(10010290)

end
function CutChapterView:OpenCallback()
    -- self.CloseInteractor.Interactable = true
end

function CutChapterView:CloseCallback()

end

CutChapterFlush = CutChapterFlush or DeclareMono("CutChapterFlush",UIWFlushPanel)
function CutChapterFlush:CutChapterFlush()

    self.is_closed = false
    local strs_ = CutChapterCtrl.Instance:GetCutChapterWorks()

    self.TextList:SetData(strs_)
    self.play_index = #strs_ 
    -- TimeHelper:AddDelayTimer(function() 
    --     self.TextList:GetItem(self.play_index):PlayAnima()
    -- end,0.5)

    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end

    self.handle_runner = Runner.Instance:RunUntilTrue(
        function ()
            local operate_cell = self.TextList:GetItem(self.play_index)

            if operate_cell == nil then 
                return false
            else 
                operate_cell:PlayAnima()
                return true
            end 
        end
    )

    self.try_timer = nil
end

function CutChapterFlush:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.try_timer == nil then 
        TimeHelper:CancelTimer(self.try_timer)
        self.try_timer = nil
    end 
end 

function CutChapterFlush:OperateNext()
    self.play_index = self.play_index - 1 
    if self.play_index > 0 then 
        if self.try_timer == nil then 
            TimeHelper:CancelTimer(self.try_timer)
            self.try_timer = nil
        end 
    
        self.try_timer = TimeHelper:AddDelayTimer(function()
            self.try_timer = nil
            self.TextList:GetItem(self.play_index):PlayAnima()
        end,0.2)
    else 
        self.CloseInteractor.Interactable = true
    end 
end

function CutChapterFlush:OnClose()
    if self.is_closed then return end 
    self.is_closed = true

    --self.Animator
    self.EffectTool:Play("10010291")
    self.BG:SetActive(true)
    self.BGEFF:SetActive(false)

    -- 防止丢失10s强制关闭
    self.close_timer = TimeHelper:AddDelayTimer(function() 
        self.close_timer = nil
        ViewMgr:CloseView(CutChapterView)
    end,10)

    if self.handle_runner then
        Runner.Instance:RemoveRunUntilTrue(self.handle_runner)
        self.handle_runner = nil
    end
end

function CutChapterFlush:OnClickEffEnd()
    -- body
    ViewMgr:CloseView(CutChapterView)
end

CutChapterTextCell = CutChapterTextCell or DeclareMono("CutChapterTextCell",UIWidgetBaseItem)

function CutChapterTextCell:SetData(data)
    self.data = data

    self.TextEnd:SetActive(self.data.is_end)
    self.Text.text = self.data.desc
end

function CutChapterTextCell:PlayAnima()
    if self.Anima ~= nil then 
        self.Anima:ListenEvent(
            "text_state",
            function (eve_param,anim_st_info)
                if eve_param == "complete" then 
                    self:AnimaCallBack()
                end 
            end
        )

        self.Anima:SetBool(APH("started"), true)
    end 
end

function CutChapterTextCell:AnimaCallBack()
    self.MianPanel:OperateNext()
end