SudokuView = SudokuView or DeclareView("SudokuView","anecdote/sudoku_view",Mod.Anecdote.SudokuBook)
VIEW_DECLARE_LEVEL(SudokuView, ViewLevel.L0)
function SudokuView:SudokuView()
	self.data = SudokuData.Instance
end

function SudokuView:LoadCallback(Param_t)
	self:FlushPanel()
end

function SudokuView:FlushPanel()
	self.OpenAnimator:SetBool("is_show",true)
	self.DescPanel:SetActive(not self.data.is_opened_desc)
	self.SudokuPanel:SetActive(self.data.is_opened_desc)
end

function SudokuView:CloseCallback()
	self.data:Reset()
end

function SudokuView:OnCloseClick()
	local param_t = {
        content = Language.Anecdote.SudokuExitTip,
        confirm = {
            func =  function()
            	ViewMgr:CloseView(DialogTipsView)
                local talk_id = self.data:GetFailTalkId()
				NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id})
				ViewMgr:CloseView(SudokuView)
            end 
        },
    }
    PublicPopupCtrl.Instance:DialogTips(param_t)
end

function SudokuView:OnOkClick()
	self.data.is_opened_desc = true
	self.OpenAnimator:SetBool("is_show",false)
	self.OpenAnimator:ListenEvent("end_hiding",BindTool.Bind(self.FlushPanel,self))
	--self:FlushPanel()
end

function SudokuView:OnTipClick()
	if self.TipPanelAnimator.gameObject.activeSelf == false then
		self.TipPanelAnimator.gameObject:SetActive(true)
	end
	self.TipMask:SetActive(true)
	self.TipPanelAnimator:SetBool(APH("is_show"),true)
end

function SudokuView:OnCloseTipClick()
	self.TipPanelAnimator:SetBool(APH("is_show"),false)
	self.TipMask:SetActive(false)
end

SudokuPanel = SudokuPanel or DeclareMono("SudokuPanel",UIWFlushPanel)
function SudokuPanel:SudokuPanel()
	self.data = SudokuData.Instance
end
function SudokuPanel:Awake()
	UIWFlushPanel.Awake(self)
	self:FlushPanel()
end

function SudokuPanel:FlushPanel()
	local a_list_data = self.data:GetTopElements()
	local b_list_data = self.data:GetBottomElements()
	self.AList:SetData(a_list_data)
	self.BList:SetData(b_list_data)
end

function SudokuPanel:IsSucc()
	local list_data = self.data:GetTopElements()
	if #list_data < 9 then
		LogError("洛门奇书数据异常，请检查")
		return false
	end
	if list_data[1].value + list_data[2].value + list_data[3].value ~= 15 then
		return false
	end
	if list_data[4].value + list_data[5].value + list_data[6].value ~= 15 then
		return false
	end
	if list_data[7].value + list_data[8].value + list_data[9].value ~= 15 then
		return false
	end
	if list_data[1].value + list_data[4].value + list_data[7].value ~= 15 then
		return false
	end
	if list_data[2].value + list_data[5].value + list_data[8].value ~= 15 then
		return false
	end
	if list_data[3].value + list_data[6].value + list_data[9].value ~= 15 then
		return false
	end
	if list_data[1].value + list_data[5].value + list_data[9].value ~= 15 then
		return false
	end
	if list_data[3].value + list_data[5].value + list_data[7].value ~= 15 then
		return false
	end
	return true
end

function SudokuPanel:OnSuccClick()
	if self:IsSucc() then
		AnecdoteCtrl.Instance:SendTaskAction(AnecdoteType.Sudoku)
		ViewMgr:CloseView(SudokuView)
	else
		PublicPopupCtrl.Instance:Center(Language.Anecdote.SudokuNotSucc)
	end
end

function SudokuPanel:GetNullData()
	return {}
end

function SudokuPanel:OnEnterBottom(data,trigger_type)
	self.item_b_data = self.data:GetSelectMonoB()
	if self.item_b_data == nil then
		return
	end
	-- 这个是来判断是不是底部的棍子，底部棍子做了位置判断
	if self.item_b_data.data.pos ~= nil then
		return
	end

	if self.data:GetEnterBottomData() ~= nil then
		return
	end
	
	-- 这个是判断是不是拖到空桌子上
	if data == nil or next(data) == nil then
		local bottom_list = self.data:GetBottomElements()
		if trigger_type == "left" then
			for i = 1 , #bottom_list do
				if bottom_list[i].value > 0 then
					data = bottom_list[i]
					break
				end
			end
			data = bottom_list[1]
		else
			for i=#bottom_list,1,-1 do
				if bottom_list[i].value > 0 then
					data = bottom_list[i]
					break
				end
			end
			data = bottom_list[1]
		end
	else
		self.TriggerLObj:SetActive(false)
	end
	self.data:SetEnterBottomData({data = data,trigger_type = trigger_type})
	self.ContentSize.enabled = false
	self.BottomLayout.enabled = false
	for i,v in pairs(self.BList.item_list) do
		local pos = v.gameObject:GetLocalPosition()
		local num = 50
		if trigger_type == "left" then
			pos.x = pos.x + num
			if v.data.seq == data.seq then
				v:SetTriggerAddPos(trigger_type,-num)
			end
		elseif data.pos == SudokuData.BOTTOM_ELEMENT_POS.RIGHT then
			pos.x = pos.x - num
			if v.data.seq == data.seq then
				v:SetTriggerAddPos(trigger_type,num)
			end
		else
			if v.data.seq <= data.seq then
				pos.x = pos.x - num
				if v.data.seq == data.seq then
					v:SetTriggerAddPos(trigger_type,num)
				end
			else
				pos.x = pos.x + num
			end
		end
		v.gameObject:SetLocalPosition(pos)
	end
end

function SudokuPanel:OnExitBottom()
	self.item_b_data = self.data:GetSelectMonoB()
	if self.item_b_data == nil or self.data:GetEnterBottomData() == nil then
		return
	end
	local bottom_data = self.data:GetEnterBottomData()
	if bottom_data then
		local mono = self.BList:GetItem(bottom_data.data.seq)
		if bottom_data.trigger_type == "left" then
			mono:SetTriggerAddPos(bottom_data.trigger_type,50)
		else
			mono:SetTriggerAddPos(bottom_data.trigger_type,-50)
		end
	end
	self.ContentSize.enabled = true
	self.BottomLayout.enabled = true
	self.data:SetEnterBottomData(nil)
end

function SudokuPanel:OnEndTopDrag()
	local item_b_data = SudokuData.Instance:GetSelectMonoB()
	local bottom_data = self.data:GetEnterBottomData()
	if bottom_data and item_b_data then
		local mono = self.BList:GetItem(bottom_data.data.seq)
		if bottom_data.trigger_type == "left" then
			mono:SetTriggerAddPos(bottom_data.trigger_type,50)
		else
			mono:SetTriggerAddPos(bottom_data.trigger_type,-50)
		end
		self.ContentSize.enabled = true
		self.BottomLayout.enabled = true
		self.data:ResetSortData(bottom_data.data,item_b_data.data.value,bottom_data.data.pos,bottom_data.trigger_type)
		self.data:SetEnterBottomData(nil)
		local b_list_data = self.data:GetBottomElements()
		self.BList:SetData(b_list_data)
		item_b_data.FlagCanvas.overrideSorting = false
		item_b_data.Block.enabled = true
		item_b_data.Layout.enabled = true
		item_b_data:SetValue(0)
		self.data:SetSelectMonoB(nil)
		self.TriggerLObj:SetActive(true)
	end
end

-- 上面九个格子的棍子item
SudokuItemTop = SudokuItemTop or DeclareMono("SudokuItemTop",UIWidgetBaseItem)
function SudokuItemTop:SudokuItemTop()
	if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
end

function SudokuItemTop:SetData(data)
	UIWidgetBaseItem.SetData(self,data)
	self.FlagCanvas.overrideSorting = false
	self:SetValue(data.value)
end

function SudokuItemTop:SetValue(value,is_animation)
	if value ~= nil and value > 0 then
		UH.SpriteName(self.FlagIcon,string.format("%s",value))
		self.FlagObj:SetActive(true)
	else
		self.FlagObj:SetActive(false)
	end
	self.data.value = value
	if is_animation then
		self.Animation:Play()
	end
end

function SudokuItemTop:OnEnter()
	-- if self.data.value > 0 then
	-- 	return
	-- end
	self.item_b_data = SudokuData.Instance:GetSelectMonoB()
	if self.item_b_data ~= nil and self.item_b_data ~= self then
		SudokuData.Instance:SetSelectMonoA(self)
		--self.FlagObj:SetActive(true)
	end
end

function SudokuItemTop:OnExit()
	-- if self.data.value > 0 then
	-- 	return
	-- end
	if self.item_b_data ~= nil then
		SudokuData.Instance:SetSelectMonoA(nil)
	end
end

function SudokuItemTop:OnDragBegin()
	if self.data.value == nil or self.data.value <= 0 then
		return
	end
	UH.LocalScale(self.FlagRect,Vector3.New(1.1,1.1,1.1))
	self.FlagCanvas.overrideSorting = true
	self.Layout.enabled = false
	self.Block.enabled = false
	local v2 = Vector2.New(0.5, 0.5)
    self.Rect.anchorMax = v2
    self.Rect.anchorMin = v2
    SudokuData.Instance:SetSelectMonoB(self)
end

function SudokuItemTop:OnDrag()
	if self.data.value == nil or self.data.value <= 0 then
		return
	end
	local _, pos =
        UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
        self.ContentRect,
        UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil
    )
    UH.AnchoredPosition(self.Rect,pos)
end

function SudokuItemTop:OnDragEnd()
	if self.data.value == nil or self.data.value <= 0 then
		return
	end
	if SudokuData.Instance:GetEnterBottomData() then
		return
	end
	UH.LocalScale(self.FlagRect,Vector3.New(1,1,1))
	SudokuData.Instance:SetSelectMonoB(nil)
	self.FlagCanvas.overrideSorting = false
	self.Block.enabled = true
	self.Layout.enabled = true
	if SudokuData.Instance:GetSelectMonoA() ~= nil then
		local a_value = SudokuData.Instance:GetSelectMonoA().data.value
		SudokuData.Instance:GetSelectMonoA():SetValue(self.data.value,true)
		self:SetValue(a_value,true)
	end
end

-- 底部的棍子
SudokuItemBottom = SudokuItemBottom or DeclareMono("SudokuItemBottom",UIWidgetBaseItem)

function SudokuItemBottom:SudokuItemBottom()
	if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
end

function SudokuItemBottom:SetData(data)
	self.FlagCanvas.overrideSorting = false
	UIWidgetBaseItem.SetData(self,data)
	self:SetValue(data.value)
	self.pos = SudokuData.Instance:GetBottomElementPos(data)
	data.pos = self.pos
	self:SetTriggerActive()
end

function SudokuItemBottom:SetTriggerActive()
	if self.pos == SudokuData.BOTTOM_ELEMENT_POS.LEFT then
		self.TriggerRectL.gameObject:SetActive(true)
		self.TriggerRectR.gameObject:SetActive(true)
	else
		self.TriggerRectL.gameObject:SetActive(false)
		self.TriggerRectR.gameObject:SetActive(true)
	end
end

function SudokuItemBottom:SetValue(value,is_animation)
	if value > 0 then
		UH.SpriteName(self.FlagIcon,string.format("%s",value))
		self.gameObject:SetActive(true)
		self.FlagObj:SetActive(true)
	else
		self.gameObject:SetActive(false)
		self.FlagObj:SetActive(false)
	end
	self.data.value = value
	if is_animation then
		--self.Animation:Play()
	end
end

function SudokuItemBottom:OnDragBegin()
	self:SetEditSattus()
	--self.ContentRect:SetSizeWithCurrentAnchors(0,SudokuData.Instance:GetRemainCount() * 100)
	UH.LocalScale(self.FlagRect,Vector3.New(1.1,1.1,1.1))
	local v2 = Vector2.New(0.5, 0.5)
    self.Rect.anchorMax = v2
    self.Rect.anchorMin = v2
	SudokuData.Instance:SetSelectMonoB(self)
end

function SudokuItemBottom:OnDrag()
	local _, pos =
        UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
        self.ContentRect,
        UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil
    )
    UH.AnchoredPosition(self.Rect,pos)
end

function SudokuItemBottom:OnDragEnd()
	SudokuData.Instance:SetSelectMonoB(nil)
	UH.LocalScale(self.FlagRect,Vector3.New(1,1,1))
	self:SetNotEditStatus()
	if SudokuData.Instance:GetSelectMonoA() ~= nil then
		local a_value = SudokuData.Instance:GetSelectMonoA().data.value
		SudokuData.Instance:GetSelectMonoA():SetValue(self.data.value,true)
		self:SetValue(a_value)
	end
end

-- 设置可编辑|拖拽状态
function SudokuItemBottom:SetEditSattus()
	self.FlagCanvas.overrideSorting = true
	self.Layout.enabled = false
	self.ContentSize.enabled = false
	self.Block.enabled = false
	self.TriggerRectL.gameObject:SetActive(false)
	self.TriggerRectR.gameObject:SetActive(false)
end
-- 设置不可编辑状态
function SudokuItemBottom:SetNotEditStatus()
	self:SetTriggerActive()
	self.FlagCanvas.overrideSorting = false
	self.Block.enabled = true
	self.Layout.enabled = true
	self.ContentSize.enabled = true
end

function SudokuItemBottom:SetTriggerAddPos(trigger_type,num)
	local obj = self.TriggerRectR.gameObject
	if trigger_type ~= "right" then
		obj = self.TriggerRectL.gameObject
	end

	local tri_pos = obj:GetLocalPosition()
	tri_pos.x = tri_pos.x + num
	obj:SetLocalPosition(tri_pos)
end