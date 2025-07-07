UIWProgressBar = UIWProgressBar or DeclareMono("UIWProgressBar")
--------ProgSlider=====Slider
--------ProgFilledImg==Image(Filled)
--------Text=========Text(nil)

---rate 0~1
function UIWProgressBar:SetProgress(rate, block282)
	self.ProgSlider.value = rate
	if self.ProgFilledImg ~= nil then
		self.ProgFilledImg.fillAmount = rate
	end
	self.rate_cache = rate
	if self.effect_par ~= nil and self.effect_obj ~= nil then
		if self.width == nil then
			if self.RootRect ~= nil then
				self.width = self.RootRect.sizeDelta.x
			else
				return
			end
		end
		local posX = self.width * rate
		self.effect_par.anchoredPosition = Vector2.New(posX,0)
		self.effect_obj:SetActive(self.width ~= nil and rate ~= 1)
		if self.EffectTool ~= nil and rate ~= 1 and rate > 0 and not block282 then
			if self.EffectTool.isActiveAndEnabled then
				self.EffectTool:Play("10010282")
			end
		end
	end
end

--用于控件中的进度条特效位置计算
function UIWProgressBar:SetProgWidth(width)
	self.width = width
end

function UIWProgressBar:GetProgress()
	return self.rate_cache or self.ProgSlider.value
	-- return self.ProgSlider.value
end


function UIWProgressBar:SetText(text)
	if self.Text ~= nil then
		self.Text.text = text
	end
end


UIWProgressBarEx = UIWProgressBarEx or DeclareMono("UIWProgressBarEx")
--------ProgSlider=====PercentSlider
--------ProgSilderB====PercentSlider
--------Text=========Text(nil)
--------FillSpeedScale===float(0~]
--------EndFuncTime=====float 调用结束回调的时间(毫秒)，不设置的话以动画时间为准
--------ProgFilledImg===Image	Image型进度条，不拉伸只切割


-- UIWProgressBarEx.DefaultSpeed = 0.8 --每秒 多少 rate
function UIWProgressBarEx:UIWProgressBarEx()
	self.end_func = nil
	self:SetProgress(0)
	if self.FillSpeedScale == nil then
		self.FillSpeedScale = 1
	end
	self.anim_state = nil
	self.anim_len = 0
	self.anim_name = ""
    local iter = self.Anim:GetEnumerator()           
    while iter:MoveNext() do
        local v = iter.Current
        -- if v.name == "exp_progress" then
        	self.anim_state = v
        	self.anim_state.speed = self.FillSpeedScale
        	self.anim_len = self.anim_state.length
        	self.anim_name = self.anim_state.name
        	break
        -- end
    end
    self.timer_handle = nil
end

---rate 0~1
function UIWProgressBarEx:SetProgress(rate)
	if self.ProgSilderB ~= nil then
		self.ProgSilderB.SliderValue = 0
	end
	self.ProgSlider.SliderValue = rate
	if self.ProgFilledImg ~= nil then
		self.ProgFilledImg.fillAmount = rate
	end
	self.rate_cache = rate
	if self.timer_handle ~= nil then
		TimeHelper:CancelTimer(self.timer_handle)
		self.timer_handle = nil
	end

end

function UIWProgressBarEx:AddToProgress(rate,end_func)

	if self.timer_handle ~= nil then
		TimeHelper:CancelTimer(self.timer_handle)
		self.timer_handle = nil
	end
	
	local old_rate = self.ProgSlider.SliderValue
	self.rate_cache = rate
	if old_rate ~= self.rate_cache then
		AudioMgr:PlayEffect(AudioType.UI,"progress_add")
	end
	if old_rate >= self.rate_cache then	--进度减，则无动作
		if self.ProgSilderB ~= nil then
			self.ProgSilderB.SliderValue = 0
		end
		self.ProgSlider.SliderValue = self.rate_cache
		if self.ProgFilledImg ~= nil then
			self.ProgFilledImg.fillAmount = self.rate_cache
		end		
		if end_func ~= nil then
			end_func()
		end
	else
		local from = old_rate
		local to = self.rate_cache
		if self.ProgSilderB ~= nil then
			local b_val_now = self.ProgSilderB.SliderValue 
			self.ProgSilderB:SetFromTo(b_val_now > from and b_val_now or from,to)
			self.ProgSilderB.Per = 0
		end
		self.ProgSlider:SetFromTo(from,to)
		self.ProgSlider.Per = 0
		self.Anim:Stop()
		self.Anim:Play(self.anim_name)
		local anim_time = (self.EndFuncTime ~= nil and self.EndFuncTime/1000.0 or self.anim_len) * self.FillSpeedScale
		if self.ProgFilledImg ~= nil then
			self.timer_handle = TimeHelper:AddCountDownTT(function() 
					self.ProgFilledImg.fillAmount = self.ProgSlider.SliderValue 
				end,
				function() 
					self.ProgFilledImg.fillAmount = self.rate_cache
					self.timer_handle = nil
					if end_func ~= nil then
						end_func()
					end
				end,
				anim_time,0.03,true
			)
		else
			self.timer_handle = TimeHelper:AddDelayTimer(function() 
					self.timer_handle = nil
					if end_func ~= nil then
						end_func()
					end
				end,anim_time)
		end
	end
end

function UIWProgressBarEx:OnDisable()
	if self.ProgSilderB then
		self.ProgSlider.Per = self.ProgSilderB.Per
		if self.LightImg then
			self.LightImg.color = COLORS.Transparent
		end
	end
end

function UIWProgressBarEx:OnDestroy()
	if self.timer_handle ~= nil then
		TimeHelper:CancelTimer(self.timer_handle)
		self.timer_handle = nil
	end
end

function UIWProgressBarEx:GetProgress()
	return self.rate_cache
end


function UIWProgressBarEx:SetText(text)
	if self.Text ~= nil then
		self.Text.text = text
	end
end

-- ===== 滑动器 =====
UIWSlider = UIWSlider or DeclareMono("UIWSlider")
function UIWSlider:SetChangedEvent(func)
    self.changed_event = func
end

function UIWSlider:SetOffEvent(func)
    self.off_event = func
end

function UIWSlider:GetSlider()
    return self.Slider
end

function UIWSlider:SetValue(value)
    self.Slider.value = value
end

function UIWSlider:GetValue()
    return self.Slider.value
end

function UIWSlider:OnChangedEvent()
    if self.changed_event then
        self.changed_event()
    end
end

function UIWSlider:OnOffEvent()
    if self.off_event then
        self.off_event()
		self.off_event = nil
    end
end