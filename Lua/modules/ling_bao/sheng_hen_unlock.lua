ShengHenUnlockView = ShengHenUnlockView or DeclareView("ShengHenUnlockView", "ling_bao/sheng_hen_unlock")
VIEW_DECLARE_MASK(ShengHenUnlockView, ViewMask.Block)
function ShengHenUnlockView:ShengHenUnlockView()
    self.Data = LingBaoData.Instance
    self.close_anima_speed = 10
end

function ShengHenUnlockView:LoadCallback(param_t)
    self.now_select_shenghen = param_t.shenghen_type
    
    if self.bg_effect then
        UH.StopEffect(self.BgEffect, self.bg_effect)
    end
    TimeHelper:CancelTimer(self.bg_handler)
    self.bg_handler = TimeHelper:AddDelayFrameTimer(function()
        self.bg_effect = UH.PlayEffect(self.BgEffect, LingBaoConfig.ShengHenBg[self.now_select_shenghen])
    end)

    self.FontBg:SetObjActive(false)
    local bg_path = Format(self.Data:ShengHenFontBgPath(self.now_select_shenghen))
    ResMgr:LoadObject(bg_path, function(tex)
        if not self:Destroyed() then
            if self.FontBg then
                self.FontBg.texture = tex
                self.FontBg:SetObjActive(true)
            end
        end
    end, false)
    self.FontBg:SetNativeSize()
    UH.SpriteName(self.FontIcon, "_LocUnLock" .. self.now_select_shenghen)

    self.left_data, self.left_change = self.Data:ShengHenSort()
    self.ShenHenList:SetData(self.left_data)
end

function ShengHenUnlockView:OnCloseClick()
    self.ObjBlock:SetActive(false)
    self.shenghen_item = self.ShenHenList:GetItem(self.left_change[self.now_select_shenghen])
    self:PlayCloseAnimation(self.shenghen_item:GetNodeObj().transform)
end

function ShengHenUnlockView:PlayCloseAnimation(icon_trans)
	self.ObjNode.transform:SetParent(icon_trans)
	TimeHelper:CancelTimer(self.time_handle)
	self.panel_scale = 1
	self.panel_scale_lerp = self.panel_scale / self.close_anima_speed
	self.view_pos = self.ObjNode:GetLocalPosition()
	self.view_pos_x_lerp = self.view_pos.x / self.close_anima_speed
	self.view_pos_y_lerp = self.view_pos.y / self.close_anima_speed
	self.canvas_group_alpha = 1 / self.close_anima_speed
	self.time_handle = TimeHelper:AddRunFrameTimer(BindTool.Bind(self.CloseAnimation,self), 1, self.close_anima_speed)
end

function ShengHenUnlockView:CloseAnimation()
	self.panel_scale = self.panel_scale - self.panel_scale_lerp
	self.panel_scale = self.panel_scale <= 0.1 and 0.1 or self.panel_scale
	UH.LocalScaleG(self.ObjRoot, Vector3.New(self.panel_scale, self.panel_scale, self.panel_scale))
	self.view_pos.x = self.view_pos.x - self.view_pos_x_lerp
	self.view_pos.y = self.view_pos.y - self.view_pos_y_lerp
    self.ObjNode:SetLocalPosition(self.view_pos)
    
	self.CanvasGroup.alpha = self.CanvasGroup.alpha - self.canvas_group_alpha

	self.close_anima_speed = self.close_anima_speed - 1
    if self.close_anima_speed <= 0 then
        UH.PlayEffect(self.EffectTool, LingBaoConfig.ShengHenUnlock[self.now_select_shenghen])
	end
end

function ShengHenUnlockView:EffectEnd()
    ViewMgr:CloseView(ShengHenUnlockView)
end

function ShengHenUnlockView:CloseCallback()
    TimeHelper:CancelTimer(self.bg_handler)
    TimeHelper:CancelTimer(self.time_handle)
end

--------------ShengHenPosItem--------------
ShengHenPosItem = ShengHenPosItem or DeclareMono("ShengHenPosItem", UIWidgetBaseItem)
function ShengHenPosItem:ShengHenPosItem()
    self.Data = LingBaoData.Instance
    self.language = Language.LingBao
end

function ShengHenPosItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
end

function ShengHenPosItem:GetNodeObj()
    return self.ObjNode
end