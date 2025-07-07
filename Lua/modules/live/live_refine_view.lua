LiveRefineView = LiveRefineView or DeclareView("LiveRefineView", "live/live_refine_view")
function LiveRefineView:OnCloseClick()
    ViewMgr:CloseView(LiveRefineView)
end

function LiveRefineView:LoadCallback(param_t)
    param_t = param_t or {}
    self.Panel:SetData(param_t)
end

LiveRefinePanel = LiveRefinePanel or DeclareMono("LiveRefinePanel", UIWFlushPanel)
function LiveRefinePanel:LiveRefinePanel()
    self.len = 344                  -- 火焰移动总路程
    self.left_speed = -self.len / 4 -- 火焰向左一秒移动的距离
    self.right_speed = self.len / 2
    self.speed = self.left_speed
    self.frame = 50
    GuideManager.Instance:RegisterGetGuideUi("LiveRefineInfo",function ()
        return self
    end)
    GuideManager.Instance:RegisterGetGuideUi("LiveRefineBtn",function ()
        return self.Btn, function ()
            self:FinishFunc()
        end
    end)

end

function LiveRefinePanel:SetData(data)
    self.refine_count = data.count
    self.cfg = data.cfg

    self.tween_hadle = nil
    self.total_time = self.cfg.hit_long / 100
    local v = self.len / self.total_time
    local fanwei = (self.cfg.hit_min - self.cfg.hit_max) / 100 * v
    local posX = self.cfg.hit_max / 100 * v
    local pos = UH.GetRootPosition(self.FireObj)
    self.end_pos = Vector2.New(pos.x + self.len, 36)
    self.min_pos=self.cfg.hit_max/100*v
    self.max_pos=self.cfg.hit_min/100*v
    self.RectIndex.sizeDelta = Vector2.New(fanwei, 24)
    local index_pos = UH.GetRootPosition(self.ObjIndex)
    UH.SetRectPosition(self.RectIndex, {x = posX + index_pos.x})
end



function LiveRefinePanel:Awake()
    UIWFlushPanel.Awake(self)
    local skill_type = LiveData.Instance:GetSelSkillType() or 0
    UH.SpriteName(self.Icon, "Skill" .. skill_type)
    -- self:Reset()
    -- self.run_ht = Runner.Instance:RunUntilTrue(BindTool.Bind(self.RunFunc,self))
    self.FireEffect:SetActive(false)
end

function LiveRefinePanel:OnDestroy()
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
    if not self.is_send then
        LiveData.Instance.refine_count = 1
    end
    AudioMgr:StopEffectHandle(self.audio_ht)
    Runner.Instance:RemoveRunUntilTrue(self.tween_hadle)
end

function LiveRefinePanel:Reset()
    self:SetX(0)
    self.ok_time = 0
    self.ProgressImg.fillAmount = 0
end

function LiveRefinePanel:SetX(x)
    if x < 0 then
        self.FireEffect:SetActive(false)
        x = 0
        AudioMgr:StopEffectHandle(self.audio_ht)
        self.audio_ht = nil
    else
        self.FireEffect:SetActive(true)
    end
    if x > self.len then
        x = self.len
    end
    if x == self:GetX() then
        return
    end

    local convert_x = x - self.len / 2

    if self:IsOk(x) then
        self.ok_time = self.ok_time + Time.deltaTime
        local progress_num = self.ok_time / 2
        self.ProgressImg.fillAmount = progress_num
        if progress_num >= 1 then
            self:Reset()
            self:FinishFunc()
            return
        end
    end

    local vec_3 = Vector3.New(convert_x, 36, 0)
    self.FireObj:SetLocalPosition(vec_3)
end

function LiveRefinePanel:GetX()
    local x = self.FireObj:GetLocalPosition().x
    x = x + self.len / 2
    return x
end

function LiveRefinePanel:IsOk(x)
    if x >= self.len * (2 / 4) and x <= self.len * (3 / 4) then
        return true
    else
        return false
    end
end

-- 完成炼制方法
function LiveRefinePanel:FinishFunc()
    if self.tween_hadle ~= nil then
        local posX=self:GetX()
        local is_succ=posX >= self.min_pos and posX <= self.max_pos and 1 or 0
        ViewMgr:CloseView(LiveRefineView)
        LiveCtrl.Instance:SendSkillReq(LiveCtrl.LIVE_SKILL_OP_TYPE.MAKE_ITEM, self.cfg.sn, 0, self.refine_count,is_succ)
        LiveData.Instance.refine_count = self.refine_count
        self.is_send = true
        Runner.Instance:RemoveRunUntilTrue(self.tween_hadle)
    else 
        self.tween_hadle=UH.MoveToTarget(self.FireObj, self.end_pos, self.total_time, nil)
    end
end


function LiveRefinePanel:RunFunc()
    local x = self:GetX()
    x = x + self.speed * Time.deltaTime
    self:SetX(x)
end

function LiveRefinePanel:OnRefineDown()
    self.speed = self.right_speed
    if not self.audio_ht then
        self.audio_ht = AudioMgr:PlayEffectHandle(AudioType.UI, CommonAudio.WoekshopMake)
    end
end

function LiveRefinePanel:OnRefineUp()
    self.speed = self.left_speed
end


LiveRefineResultView = LiveRefineResultView or DeclareView("LiveRefineResultView", "live/live_refine_result")
VIEW_DECLARE_MASK(LiveRefineResultView,ViewMask.BgBlock)
function LiveRefineResultView:LiveRefineResultView()
    self.title_list = {
        [1] = "_LocLianZhiWanCheng",            -- 炼制完成
        [2] = "_LocPengRenWanCheng",            -- 烹饪完成 
        [3] = "_LocGongXiJieSuoZi",     -- 解锁新配方
    }

    GuideManager.Instance:RegisterGetGuideUi("LiveRefineResult",function ()
        return self.GuideInfo, function ()
            self:OnCloseClick()
        end
    end)
end

-- param_t : {item_list, tip}
function LiveRefineResultView:LoadCallback(param_t)
    param_t = param_t or {item_list = {}}
    local tip = param_t.tip or ""
    UH.SetText(self.TipText, tip)
    self.ItemList:SetData(param_t.item_list)
    local title = param_t.title or 1
    title = self.title_list[title]
    UH.SpriteName(self.Title, title)
    self.open_view = param_t.open_view
end

function LiveRefineResultView:OnCloseClick()
    ViewMgr:CloseView(LiveRefineResultView)
end

function LiveRefineResultView:CloseCallback()
    if self.open_view then
        ViewMgr:OpenView(self.open_view)
    end
end

LiveRefineResultItemCell = LiveRefineResultItemCell or DeclareMono("LiveRefineResultItemCell", UIWidgetBaseItem)
function LiveRefineResultItemCell:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.Cell:SetData(data)
    UH.SetText(self.Name, data:Name())
end