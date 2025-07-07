-- 游历诗人
Poetry2View = Poetry2View or DeclareView("Poetry2View", "anecdote/poetry2_view", Mod.Anecdote.Poetry2)
VIEW_DECLARE_MASK(Poetry2View, ViewMask.None)
function Poetry2View:Poetry2View()
	self.Timer2 = nil
	Poetry2Data.Instance:DataReset(Config.poetry_sort_two_auto)
	Poetry2Data.Instance:SetFinishCallback(BindTool.Bind1(self.PlayStory, self))
end

function Poetry2View:OpenCallback()
	self.PoetryMono:InitData(AnecdoteType.Poetry2, Language.Anecdote.PoetryWord2)
end

function Poetry2View:OnCloseClick()
	if not Poetry2Data.Instance:GetIsCanDrag() then
		return
	end
	local param_t = {
		content = Language.Anecdote.PoetryExitTip,
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
				local talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.Poetry2)
				NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id})
				ViewMgr:CloseView(Poetry2View)
			end
		}
	}
	PublicPopupCtrl.Instance:DialogTips(param_t)
end

function Poetry2View:CloseCallback()
	TimeHelper:CancelTimer(self.Timer2)
end

function Poetry2View:PlayStory()
	Poetry2Data.Instance:GetListAnim().enabled = true
	Poetry2Data.Instance:GetListAnim():Play("poetry_view_ItemList_close")
	self.Timer2 = TimeHelper:AddDelayTimer(function()
	-- 	StoryData.Instance:RecordStoryPrePlay(201, StoryData.Instance:StartStoryById(201))
	-- 	StoryCtrl.Instance:PlayStoryByStoryId(201)
	ViewMgr:CloseView(Poetry2View)
	end, 3)
end

-- -- 寻味美食
-- ---------------------------------CookSortView---------------------------------
-- CookSortView = CookSortView or DeclareView("CookSortView", "anecdote/cook_sort_view", Mod.Anecdote.CookSort)
-- VIEW_DECLARE_MASK(CookSortView, ViewMask.None)
-- function CookSortView:CookSortView()
-- 	self.Timer2 = nil
-- 	Poetry2Data.Instance:DataReset(Config.cooking_methods_sort_auto)
-- 	Poetry2Data.Instance:SetFinishCallback(BindTool.Bind1(self.PlayEffect, self))
-- end

-- function CookSortView:OpenCallback()
-- 	self.PoetryMono:InitData(AnecdoteType.CookSort, Language.Anecdote.CookSortWord)
-- end

-- function CookSortView:OnCloseClick()
-- 	if not Poetry2Data.Instance:GetIsCanDrag() then
-- 		return
-- 	end
-- 	local param_t = {
-- 		content = Language.Anecdote.CookSortExitTip,
-- 		confirm = {
-- 			func = function()
-- 				ViewMgr:CloseView(DialogTipsView)
-- 				local talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.CookSort)
-- 				NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id})
-- 				ViewMgr:CloseView(CookSortView)
-- 			end
-- 		}
-- 	}
-- 	PublicPopupCtrl.Instance:DialogTips(param_t)
-- end

-- function CookSortView:CloseCallback()
--     TimeHelper:CancelTimer(self.Timer2)
--     if Poetry2Data.Instance:GetIsSucceed() then
--         SceneLogic.Instance:AutoToNpcs(Config.cooking_methods_sort_auto.other[1].npc_id)
--     end
-- end

-- function CookSortView:PlayEffect()
-- 	self.ObjFinishMask:SetActive(true)
-- end

-- -- 点击全匹配遮罩
-- function CookSortView:OnFinishClick()
--     ViewMgr:CloseView(CookSortView)
-- end

-- -- 千年桃花开
-- ---------------------------------MillenniumPeachBlossomView---------------------------------
-- MillenniumPeachBlossomView = MillenniumPeachBlossomView or DeclareView("MillenniumPeachBlossomView", "anecdote/millennium_peach_blossom_view", Mod.Anecdote.MillenniumPeachBlossom)
-- VIEW_DECLARE_MASK(MillenniumPeachBlossomView, ViewMask.None)
-- function MillenniumPeachBlossomView:MillenniumPeachBlossomView()
-- 	self.Timer2 = nil
-- 	Poetry2Data.Instance:DataReset(Config.millennium_peach_blossom_auto)
-- 	Poetry2Data.Instance:SetFinishCallback(BindTool.Bind1(self.PlayEffect, self))
-- end

-- function MillenniumPeachBlossomView:OpenCallback()
-- 	self.PoetryMono:InitData(AnecdoteType.MillenniumPeachBlossom, Language.Anecdote.MillenniumPeachBlossomWord)
-- end

-- function MillenniumPeachBlossomView:OnCloseClick()
-- 	if not Poetry2Data.Instance:GetIsCanDrag() then
-- 		return
-- 	end
-- 	local param_t = {
-- 		content = Language.Anecdote.MillenniumPeachBlossomExitTip,
-- 		confirm = {
-- 			func = function()
-- 				ViewMgr:CloseView(DialogTipsView)
-- 				local talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.MillenniumPeachBlossom)
-- 				NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id})
-- 				ViewMgr:CloseView(MillenniumPeachBlossomView)
-- 			end
-- 		}
-- 	}
-- 	PublicPopupCtrl.Instance:DialogTips(param_t)
-- end

-- function MillenniumPeachBlossomView:CloseCallback()
--     TimeHelper:CancelTimer(self.Timer2)
--     if Poetry2Data.Instance:GetIsSucceed() then
--         SceneLogic.Instance:AutoToNpcs(Config.millennium_peach_blossom_auto.other[1].npc_id)
--     end
-- end

-- function MillenniumPeachBlossomView:PlayEffect()
-- 	self.ObjFinishMask:SetActive(true)
-- end

-- -- 点击全匹配遮罩
-- function MillenniumPeachBlossomView:OnFinishClick()
--     ViewMgr:CloseView(MillenniumPeachBlossomView)
-- end

-- -- 诗者之魂
-- ---------------------------------PoetrySoulView---------------------------------
-- PoetrySoulView = PoetrySoulView or DeclareView("PoetrySoulView", "anecdote/poetry_soul_view", Mod.Anecdote.PoetrySoul)
-- VIEW_DECLARE_MASK(PoetrySoulView, ViewMask.None)
-- function PoetrySoulView:PoetrySoulView()
-- 	self.Timer2 = nil
-- 	Poetry2Data.Instance:DataReset(Config.poet_soul_auto)
-- 	Poetry2Data.Instance:SetFinishCallback(BindTool.Bind1(self.PlayEffect, self))
-- end

-- function PoetrySoulView:OpenCallback()
-- 	self.PoetryMono:InitData(AnecdoteType.PoetrySoul, Language.Anecdote.PoetrySoulWord)
-- end

-- function PoetrySoulView:OnCloseClick()
-- 	if not Poetry2Data.Instance:GetIsCanDrag() then
-- 		return
-- 	end
-- 	local param_t = {
-- 		content = Language.Anecdote.PoetrySoulExitTip,
-- 		confirm = {
-- 			func = function()
-- 				ViewMgr:CloseView(DialogTipsView)
-- 				local talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.PoetrySoul)
-- 				NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id})
-- 				ViewMgr:CloseView(PoetrySoulView)
-- 			end
-- 		}
-- 	}
-- 	PublicPopupCtrl.Instance:DialogTips(param_t)
-- end

-- function PoetrySoulView:CloseCallback()
--     TimeHelper:CancelTimer(self.Timer2)
--     if Poetry2Data.Instance:GetIsSucceed() then
--         SceneLogic.Instance:AutoToNpcs(Config.poet_soul_auto.other[1].npc_id)
--     end
-- end

-- function PoetrySoulView:PlayEffect()
-- 	self.ObjFinishMask:SetActive(true)
-- end

-- -- 点击全匹配遮罩
-- function PoetrySoulView:OnFinishClick()
--     ViewMgr:CloseView(PoetrySoulView)
-- end


-- 当前使用到的界面有：游历诗人2
---------------------------------Poetry2Panel---------------------------------
Poetry2Panel = Poetry2Panel or DeclareMono("Poetry2Panel", UIWFlushPanel)
function Poetry2Panel:Poetry2Panel()
	self.data = Poetry2Data.Instance
	self.anecdote_data = AnecdoteData.Instance
	self.Timer1 = nil
	self.data_cares = {
		{data = self.data.check_flag, func = self.IsSuccess, init_call = false, keys = {"flag"}},
	}
end

function Poetry2Panel:InitData(anecdote_type, str)
	self.anecdote_type = anecdote_type

	UH.SetText(self.NpcWordTxt, str)
	local npc_config = Cfg.NpcBySeq(self.data:GetNPCId())
	UH.SetText(self.NpcNameTxt, npc_config.name)

	self.data:SetListAnim(self.ItemList_Anim)
	
	self.ItemList_Anim.enabled = true
	self.ItemList_Anim:Play("poetry_view_ItemList_open")
	
	self.Timer1 = TimeHelper:AddDelayTimer(function()
		self.data:SetIsCanDrag(true)
		self.ItemList_Anim.enabled = false
	end, 1.5)

	self.data:SetPos(self.PosList)
	for i = 1, self.ItemList:Length() do
		self.ItemList[i]:SetData(self.data:GetItemData(i))
	end
end

function Poetry2Panel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.Timer1)
	if self.data:GetIsSucceed() then
		SceneLogic.Instance:AutoToNpcs(self.data:GetNPCId())
	end
	self.data:DataReset()
end

-- 判断是否全部成功
function Poetry2Panel:IsSuccess()
	if self.data:GetIsCanDrag() then
		local is_success = true
		for i = 1, self.ItemList:Length() do
			if not self.ItemList[i]:IsRight() then
				is_success = false
			end
		end
		if is_success then
			self.data:SetIsSucceed(true)
			self.data:SetIsCanDrag(false)
			AnecdoteCtrl.Instance:SendTaskAction(self.anecdote_type)
			AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
			if self.data.finish_callback then
				self.data.finish_callback()
			end
		end
	end
end


---------------------------------Poetry2Item---------------------------------
Poetry2Item = Poetry2Item or DeclareMono("Poetry2Item", UIWidgetBaseItem)

function Poetry2Item:Poetry2Item()
	if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
end

function Poetry2Item:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	UH.SetText(self.WordTxt, data.poetry_word)
	self.CanvasFlag.enabled = true
	self.CanvasFlag.overrideSorting = false
end

function Poetry2Item:OnDragBegin()
	if not Poetry2Data.Instance:GetIsCanDrag() then
		return
	end

	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
	Poetry2Data.Instance:SetIsDraging(true)
	self.CanvasFlag.overrideSorting = true
	self.Block.enabled = false
	local _, pos = UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
		self.ItemListContent,
		UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil)
	Poetry2Data.Instance:SetDragItem(self)
end

function Poetry2Item:OnDrag()
	if not Poetry2Data.Instance:GetIsCanDrag() then
		return
	end
	if not Poetry2Data.Instance:GetIsDraging() then
		return
	end

	local _, pos = UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
		self.ItemListContent,
		UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil)
	UH.AnchoredPosition(self.Rect, pos)
end

function Poetry2Item:OnDragEnd()
	if not Poetry2Data.Instance:GetIsCanDrag() then
		return
	end
	if not Poetry2Data.Instance:GetIsDraging() then
		return
	end

	Poetry2Data.Instance:ExchangeItem()
	Poetry2Data.Instance:SetIsDraging(false)
	self.CanvasFlag.overrideSorting = false
	self.Block.enabled = true

	Poetry2Data.Instance:SetDragItem(nil)
end

function Poetry2Item:OnPointerEnter()
	if not Poetry2Data.Instance:GetIsDraging() then return end

	Poetry2Data.Instance:SetDragDownItem(self)
	local drag_item = Poetry2Data.Instance:GetDragItem()
	if drag_item ~= nil then
		if not self.SelectObj.activeSelf then
			self.SelectObj:SetActive(true)
		end
	end
end

function Poetry2Item:OnPointerExit()
	if not Poetry2Data.Instance:GetIsDraging() then return end

	Poetry2Data.Instance:SetDragDownItem(nil)
	local drag_item = Poetry2Data.Instance:GetDragItem()
	if drag_item ~= nil then
		if self.SelectObj.activeSelf then
			self.SelectObj:SetActive(false)
		end
	end
end

function Poetry2Item:IsRight()
	self.RightObj:SetActive(self.data.correct_number == self.data.now_pos)
	return self.data.correct_number == self.data.now_pos
end