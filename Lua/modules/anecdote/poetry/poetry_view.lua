-- 游历诗人
PoetryView = PoetryView or DeclareView("PoetryView", "anecdote/poetry_view", Mod.Anecdote.Poetry)
VIEW_DECLARE_MASK(PoetryView, ViewMask.None)
function PoetryView:PoetryView()
	self.Timer2 = nil
	PoetryData.Instance:DataReset(Config.poetry_sort_auto)
	PoetryData.Instance:SetFinishCallback(BindTool.Bind1(self.PlayStory, self))
end

function PoetryView:OpenCallback()
	self.PoetryMono:InitData(AnecdoteType.Poetry, Language.Anecdote.PoetryWord)
end

function PoetryView:OnCloseClick()
	if not PoetryData.Instance:GetIsCanDrag() then
		return
	end
	local param_t = {
		content = Language.Anecdote.PoetryExitTip,
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
				local talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.Poetry)
				NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id})
				ViewMgr:CloseView(PoetryView)
			end
		}
	}
	PublicPopupCtrl.Instance:DialogTips(param_t)
end

function PoetryView:CloseCallback()
	TimeHelper:CancelTimer(self.Timer2)
end

function PoetryView:PlayStory()
	PoetryData.Instance:GetListAnim().enabled = true
	PoetryData.Instance:GetListAnim():Play("poetry_view_ItemList_close")
	self.Timer2 = TimeHelper:AddDelayTimer(function()
		StoryData.Instance:RecordStoryPrePlay(201, StoryData.Instance:StartStoryById(201))
		StoryCtrl.Instance:PlayStoryByStoryId(201)
	end, 3)
end

-- 寻味美食
---------------------------------CookSortView---------------------------------
CookSortView = CookSortView or DeclareView("CookSortView", "anecdote/cook_sort_view", Mod.Anecdote.CookSort)
VIEW_DECLARE_MASK(CookSortView, ViewMask.None)
function CookSortView:CookSortView()
	self.Timer2 = nil
	PoetryData.Instance:DataReset(Config.cooking_methods_sort_auto)
	PoetryData.Instance:SetFinishCallback(BindTool.Bind1(self.PlayEffect, self))
end

function CookSortView:OpenCallback()
	self.PoetryMono:InitData(AnecdoteType.CookSort, Language.Anecdote.CookSortWord)
end

function CookSortView:OnCloseClick()
	if not PoetryData.Instance:GetIsCanDrag() then
		return
	end
	local param_t = {
		content = Language.Anecdote.CookSortExitTip,
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
				local talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.CookSort)
				NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id})
				ViewMgr:CloseView(CookSortView)
			end
		}
	}
	PublicPopupCtrl.Instance:DialogTips(param_t)
end

function CookSortView:CloseCallback()
    TimeHelper:CancelTimer(self.Timer2)
    if PoetryData.Instance:GetIsSucceed() then
        SceneLogic.Instance:AutoToNpcs(Config.cooking_methods_sort_auto.other[1].npc_id)
    end
end

function CookSortView:PlayEffect()
	self.ObjFinishMask:SetActive(true)
end

-- 点击全匹配遮罩
function CookSortView:OnFinishClick()
    ViewMgr:CloseView(CookSortView)
end

-- 千年桃花开
---------------------------------MillenniumPeachBlossomView---------------------------------
MillenniumPeachBlossomView = MillenniumPeachBlossomView or DeclareView("MillenniumPeachBlossomView", "anecdote/millennium_peach_blossom_view", Mod.Anecdote.MillenniumPeachBlossom)
VIEW_DECLARE_MASK(MillenniumPeachBlossomView, ViewMask.None)
function MillenniumPeachBlossomView:MillenniumPeachBlossomView()
	self.Timer2 = nil
	PoetryData.Instance:DataReset(Config.millennium_peach_blossom_auto)
	PoetryData.Instance:SetFinishCallback(BindTool.Bind1(self.PlayEffect, self))
end

function MillenniumPeachBlossomView:OpenCallback()
	self.PoetryMono:InitData(AnecdoteType.MillenniumPeachBlossom, Language.Anecdote.MillenniumPeachBlossomWord)
end

function MillenniumPeachBlossomView:OnCloseClick()
	if not PoetryData.Instance:GetIsCanDrag() then
		return
	end
	local param_t = {
		content = Language.Anecdote.MillenniumPeachBlossomExitTip,
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
				local talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.MillenniumPeachBlossom)
				NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id})
				ViewMgr:CloseView(MillenniumPeachBlossomView)
			end
		}
	}
	PublicPopupCtrl.Instance:DialogTips(param_t)
end

function MillenniumPeachBlossomView:CloseCallback()
    TimeHelper:CancelTimer(self.Timer2)
    if PoetryData.Instance:GetIsSucceed() then
        SceneLogic.Instance:AutoToNpcs(Config.millennium_peach_blossom_auto.other[1].npc_id)
    end
end

function MillenniumPeachBlossomView:PlayEffect()
	self.ObjFinishMask:SetActive(true)
end

-- 点击全匹配遮罩
function MillenniumPeachBlossomView:OnFinishClick()
    ViewMgr:CloseView(MillenniumPeachBlossomView)
end

-- 诗者之魂
---------------------------------PoetrySoulView---------------------------------
PoetrySoulView = PoetrySoulView or DeclareView("PoetrySoulView", "anecdote/poetry_soul_view", Mod.Anecdote.PoetrySoul)
VIEW_DECLARE_MASK(PoetrySoulView, ViewMask.None)
function PoetrySoulView:PoetrySoulView()
	self.Timer2 = nil
	PoetryData.Instance:DataReset(Config.poet_soul_auto)
	PoetryData.Instance:SetFinishCallback(BindTool.Bind1(self.PlayEffect, self))
end

function PoetrySoulView:OpenCallback()
	self.PoetryMono:InitData(AnecdoteType.PoetrySoul, Language.Anecdote.PoetrySoulWord)
end

function PoetrySoulView:OnCloseClick()
	if not PoetryData.Instance:GetIsCanDrag() then
		return
	end
	local param_t = {
		content = Language.Anecdote.PoetrySoulExitTip,
		confirm = {
			func = function()
				ViewMgr:CloseView(DialogTipsView)
				local talk_id = AnecdoteData.Instance:GetFailTalkId(AnecdoteType.PoetrySoul)
				NpcDialogCtrl.Instance:NpcDialogView({talk_id = talk_id})
				ViewMgr:CloseView(PoetrySoulView)
			end
		}
	}
	PublicPopupCtrl.Instance:DialogTips(param_t)
end

function PoetrySoulView:CloseCallback()
    TimeHelper:CancelTimer(self.Timer2)
    if PoetryData.Instance:GetIsSucceed() then
        SceneLogic.Instance:AutoToNpcs(Config.poet_soul_auto.other[1].npc_id)
    end
end

function PoetrySoulView:PlayEffect()
	self.ObjFinishMask:SetActive(true)
end

-- 点击全匹配遮罩
function PoetrySoulView:OnFinishClick()
    ViewMgr:CloseView(PoetrySoulView)
end


-- 当前使用到的界面有：游历诗人、寻味美食、千年桃花开、诗者之魂
---------------------------------PoetryPanel---------------------------------
PoetryPanel = PoetryPanel or DeclareMono("PoetryPanel", UIWFlushPanel)
function PoetryPanel:PoetryPanel()
	self.data = PoetryData.Instance
	self.anecdote_data = AnecdoteData.Instance
	self.Timer1 = nil
	self.data_cares = {
		{data = self.data.check_flag, func = self.IsSuccess, init_call = false, keys = {"flag"}},
	}
end

function PoetryPanel:InitData(anecdote_type, str)
	self.anecdote_type = anecdote_type

	UH.SetText(self.NpcWordTxt, str)
	local npc_id = self.data:GetNPCId()
	if npc_id then
		local npc_config = Cfg.NpcBySeq(npc_id)
		UH.SetText(self.NpcNameTxt, npc_config.name)
	else
		UH.SetText(self.NpcNameTxt, Language.Anecdote.DefaultPoetryNpcName)
	end

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

function PoetryPanel:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	TimeHelper:CancelTimer(self.Timer1)
	if self.data:GetIsSucceed() then
		local npc_id = self.data:GetNPCId()
		if npc_id then
			SceneLogic.Instance:AutoToNpcs(npc_id)
		end
	end
	self.data:DataReset()
end

-- 判断是否全部成功
function PoetryPanel:IsSuccess()
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
			if self.anecdote_type then
				AnecdoteCtrl.Instance:SendTaskAction(self.anecdote_type)
			end
			AudioMgr:PlayEffect(AudioType.UI, CommonAudio.SkillLevelUp)
			if self.data.finish_callback then
				self.data.finish_callback()
			end
		end
	end
end


---------------------------------PoetryItem---------------------------------
PoetryItem = PoetryItem or DeclareMono("PoetryItem", UIWidgetBaseItem)

function PoetryItem:PoetryItem()
	if self.mainCanvas == nil then
		self.mainCanvas = UnityGameObj.FindGameObjectWithTag("MainCanvas")
	end
    if self.mainCanvas and self.mainCanvasCom == nil then
        self.mainCanvasCom = self.mainCanvas:GetComponent(typeof(UnityEngine.Canvas))
    end
end

function PoetryItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	UH.SetText(self.WordTxt, data.poetry_word)
	self.CanvasFlag.enabled = true
	self.CanvasFlag.overrideSorting = false
end

function PoetryItem:OnDragBegin()
	if not PoetryData.Instance:GetIsCanDrag() then
		return
	end

	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ItemInfoOpen)
	PoetryData.Instance:SetIsDraging(true)
	self.CanvasFlag.overrideSorting = true
	self.Block.enabled = false
	local _, pos = UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
		self.ItemListContent,
		UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil)
	PoetryData.Instance:SetDragItem(self)
end

function PoetryItem:OnDrag()
	if not PoetryData.Instance:GetIsCanDrag() then
		return
	end
	if not PoetryData.Instance:GetIsDraging() then
		return
	end

	local _, pos = UnityEngine.RectTransformUtility.ScreenPointToLocalPointInRectangle(
		self.ItemListContent,
		UnityEngine.Input.mousePosition,
        self.mainCanvasCom.worldCamera,
        nil)
	UH.AnchoredPosition(self.Rect, pos)
end

function PoetryItem:OnDragEnd()
	if not PoetryData.Instance:GetIsCanDrag() then
		return
	end
	if not PoetryData.Instance:GetIsDraging() then
		return
	end

	PoetryData.Instance:ExchangeItem()
	PoetryData.Instance:SetIsDraging(false)
	self.CanvasFlag.overrideSorting = false
	self.Block.enabled = true

	PoetryData.Instance:SetDragItem(nil)
end

function PoetryItem:OnPointerEnter()
	if not PoetryData.Instance:GetIsDraging() then return end

	PoetryData.Instance:SetDragDownItem(self)
	local drag_item = PoetryData.Instance:GetDragItem()
	if drag_item ~= nil then
		if not self.SelectObj.activeSelf then
			self.SelectObj:SetActive(true)
		end
	end
end

function PoetryItem:OnPointerExit()
	if not PoetryData.Instance:GetIsDraging() then return end

	PoetryData.Instance:SetDragDownItem(nil)
	local drag_item = PoetryData.Instance:GetDragItem()
	if drag_item ~= nil then
		if self.SelectObj.activeSelf then
			self.SelectObj:SetActive(false)
		end
	end
end

function PoetryItem:IsRight()
	self.RightObj:SetActive(self.data.correct_number == self.data.now_pos)
	return self.data.correct_number == self.data.now_pos
end