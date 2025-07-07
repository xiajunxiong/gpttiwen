UIWMaskUp = UIWMaskUp or DeclareMono("UIWMaskUp", UIWidgetBaseItem)

-- 一定要勾选OnDestroy
-- 搜UIWMaskUp这个物体
-- 用于物体在特效层级上，并且需要受到动列表的切割
-- 这些物体的锚点都必须是（0.5，0.5）

---ScrollRect----RectTransform	    必须，这个是要ScrollRect组件下的RectTransform
---Root----RectTransform	        必须，这个是要这个组件下的RectTransform
---Node----RectTransform	        必须，需要受到切割的子物体的RectTransform
---IsWillChangeSize----bool	        是否会在中途改变ScrollRect的size大小（默认不会），如果会请加上

function UIWMaskUp:UIWMaskUp()
    self:CheckReady()
    -- 这里延迟一帧去执行，这样如果本身的ScrollRect可以先在Awake的时候设置位置和大小
    self.ht = TimeHelper:AddDelayFrameTimer(function()
        self:Init()
    end, 1)
end

function UIWMaskUp:CheckReady()
    if self.IsWillChangeSize == nil then
        self.IsWillChangeSize = false
    end
end

function UIWMaskUp:Init()
    if self.ScrollRect == nil then
        LogError("ScrollRect不能为空")
        return
    end
    if self.ScrollRect then
        if self.setPosRunner == nil then
            self.Root.sizeDelta = self.ScrollRect.sizeDelta
            self:SetPos()
        end
    end
end

function UIWMaskUp:SetPos()
    if self.unityMono ~= nil then
        if self.Root and self.ScrollRect then
            if self.IsWillChangeSize then
                self.Root.sizeDelta = self.ScrollRect.sizeDelta
            end
            local pos = self.ScrollRect.position
            self.Root.position = pos
            UH.AnchoredPosition(self.Node, Vector2.New(-self.Root.anchoredPosition.x, -self.Root.anchoredPosition.y))
            if self.setPosRunner == nil then
                self.setPosRunner = TimeHelper:AddRunFrameTimer(function ()
                    self.setPosRunner = nil
                    self:SetPos()
                end, 1, 1, false)
            end
        end
    end
end

function UIWMaskUp:OnDestroy()
    if self.setPosRunner ~= nil then
        TimeHelper:CancelTimer(self.setPosRunner)
        self.setPosRunner = nil
    end
    TimeHelper:CancelTimer(self.ht)
end