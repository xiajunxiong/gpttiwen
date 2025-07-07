

AnecdoteHintView = AnecdoteHintView or DeclareView("AnecdoteHintView", "anecdote/anecdote_hint",Mod.Anecdote.Hint)
function AnecdoteHintView:LoadCallback(param_t)
    if not UnityPlayerPrefs.HasKey(PrefKeys.AnecdoteHintGuide()) then
        GuideManager.Instance:Start(121)
        UnityPlayerPrefs.SetInt(PrefKeys.AnecdoteHintGuide(),1)
    end
end

function AnecdoteHintView:CloseCallback()

end

function AnecdoteHintView:OnCloseClick()
    ViewMgr:CloseView(AnecdoteHintView)
end


AnecdoteHintPanel = AnecdoteHintPanel or DeclareMono("AnecdoteHintPanel", UIWFlushPanel)
--DescTxt=========Text
--TipsTxt=========Text
--AnecdotesList===UIWidgetList[AnecdotesHintItem]
--ComProg=========Image
--CountTimer======UIWTimeMeter
--RewardNeedNums==Text[]
--RewardItems=====UIWSubstitute[UIWItemCell][]
--ComNumTxt=======Text
--RewardNeedNumGrays===InteractorHandler[]
--RewardEffs======GameObject
--RewardRPs=======UIWSubstitute[UIWRedPoint2][]
function AnecdoteHintPanel:AnecdoteHintPanel()
    local list_data = AnecdoteHintData.Instance:HintAnecdoteList()
    for i,v in ipairs(list_data) do
        if v.sort == nil then
            v.sort = i
        end
    end
    table.sort(list_data,function(a,b)
        local a_fin = AnecdoteData.Instance:IsHasFinish(a.stranger_task_type)
        local b_fin = AnecdoteData.Instance:IsHasFinish(b.stranger_task_type)
        if b_fin ~= a_fin then
            if b_fin == true and a_fin == false then
                return true
            else
                return false
            end
        else
            return a.sort < b.sort
        end
    end)
    for i,v in ipairs(list_data) do
        v.index = i
    end


    self.AnecdotesList:SetData(list_data)

    self.AnecdotesList:ClickItem(1)
    local ane_data = AnecdoteHintData.Instance
    self.data_cares = {
		{data = ane_data:ViewData(), func = self.OnSelDataChanged,keys = {"cur_sel"}},
		{data = ane_data:NetData(), func = self.OnRewardFetchChange,keys = {"reward_fetch_status","reward_version"},init_call = true},
        
	}
    self.CountTimer:StampTime(ane_data:EndTimeStamp(),TimeType.Type_Special_4,Language.Anecdote.Hint.TimeCount)
    UH.SetText(self.ComNumTxt,tostring(ane_data:TaskFinishNum()))

    GuideManager.Instance:RegisterGetGuideUi("AnecdoteLeftArea",function ()
        return self.LeftGuideArea, function () end
    end)
    GuideManager.Instance:RegisterGetGuideUi("AnecdoteRightArea",function ()
        return self.RightGuideArea, function () end
    end)
end

function AnecdoteHintPanel:OnSelDataChanged()
    local cur_sel = AnecdoteHintData.Instance:CurSelData()
    if cur_sel == nil then
        return
    end
    UH.SetText(self.DescTxt,"<color=#00000000>```</color>" .. cur_sel.desc) --前面空两格
    UH.SetText(self.TipsTxt,cur_sel.clue)

end

function AnecdoteHintPanel:OnRewardFetchChange()
    local ane_data = AnecdoteHintData.Instance
    local reward_data = ane_data:RewardData()
    local max_num = 0
    for i=1,self.RewardItems:Length() do
        local cfg = reward_data[i]
        if max_num < cfg.complete_num then
            max_num = cfg.complete_num
        end

        local re_item = cfg.item_list[0]
        local has_getted = ane_data:RewardHasGetted(cfg.seq)
        local item_cell = self.RewardItems[i]
        item_cell:SetData(Item.Create({item_id = re_item.item_id,num = re_item.num,
            is_bind = re_item.is_bind,is_grey = has_getted,
            re_cfg = cfg,
        }))
        UH.SetText(self.RewardNeedNums[i],tostring(cfg.complete_num))
        self.RewardNeedNumGrays[i].Interactable = not has_getted
        local can_get = ane_data:TaskFinishNum() >= cfg.complete_num
        self.RewardEffs[i]:SetActive(can_get and not has_getted)
        self.RewardRPs[i]:SetNum(can_get and not has_getted and 1 or 0)
    end
    self.ComProg.fillAmount = ane_data:TaskFinishNum() / max_num
end

function CellClicks.AnecdoteHintItemClick(cell)
    local ane_data = AnecdoteHintData.Instance
    local re_cfg = cell:GetData().vo.re_cfg
    local re_seq = re_cfg.seq

    local has_getted = ane_data:RewardHasGetted(re_seq)
    local can_get = ane_data:TaskFinishNum() >= re_cfg.complete_num
    if has_getted == false and can_get then
        AnecdoteCtrl.Instance:GetHintReward(re_seq)
    else
        CellClicks.BagGridNoButton(cell)
    end
end



AnecdoteHintItem = AnecdoteHintItem or DeclareMono("AnecdoteHintItem", UIWidgetBaseItem)
--TitleTxt=======Text
--ClickEve=======LuaUIEventToggle
--ComFlag========GameObject

function AnecdoteHintItem:AnecdoteHintItem()

end


function AnecdoteHintItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    self.gameObject.name = "tab_item" .. tostring(self.data.index)
    UH.SetText(self.TitleTxt,self.data.name)
    local is_com = AnecdoteData.Instance:IsHasFinish(self.data.stranger_task_type)
    -- LogError("self.data.stranger_task_type",self.data.stranger_task_type,is_com)
    self.ComFlag:SetActive(is_com)
end

function AnecdoteHintItem:Click()
    self.ClickEve:Trigger()
end

function AnecdoteHintItem:OnToggleOn()
    AnecdoteHintData.Instance:SetCurSelData(self.data)
end