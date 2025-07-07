SchoolExerciseView = SchoolExerciseView or DeclareView("SchoolExerciseView","school_exercise/school_exercise_view",Mod.Experience.SchoolExercise)

function SchoolExerciseView:SchoolExerciseView()
end

function SchoolExerciseView:OnCloseClick()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ViewCloseL1)
    ViewMgr:CloseView(SchoolExerciseView)
end

SchoolExercisePanel = SchoolExercisePanel or DeclareMono("SchoolExercisePanel",UIWFlushPanel)
function SchoolExercisePanel:SchoolExercisePanel()
    self.data = SchoolExerciseData.Instance
    if self.data:IsFirstOpen() == true then
        self.GuideObj:SetActive(true)
        UH.SetText(self.GuideDesc,Language.SchoolExercise.GuideDesc)
        SchoolExerciseCtrl.Instance:SendOpenFirstUIReq()
    else
        self.GuideObj:SetActive(false)
    end
    -- 最前面可战斗的关卡
    GuideManager.Instance:RegisterGetGuideUi("SchoolExerciseFirstBtn",function ()
        local first_data = SchoolExerciseData.Instance:GetJumpData()
        return self.AList:GetChildMono(first_data)
    end)
    GuideManager.Instance:RegisterGetGuideUi("SchoolExerciseFightBtn",function ()
        return self.FightBtnObj,BindTool.Bind(self.OnStartClick,self)
    end)
end

function SchoolExercisePanel:Awake()
    UIWFlushPanel.Awake(self)
    self:FlushPanel()
end

function SchoolExercisePanel:OnDestroy()
    self.data:SetPSelectData(nil)
    UIWFlushPanel.OnDestroy(self)
end

function SchoolExercisePanel:FlushPanel()
    local list_data = self.data:GetListData()
    if list_data == nil or next(list_data) == nil then
        return
    end 
    self.AList:SetData(list_data,true)
    self.AList:ClickChild(self.data:GetJumpData())
    self:FlushInfo()
end

function SchoolExercisePanel:FlushInfo(data)
    if data == nil then
        return
    end
    self.selected_data = data
    UH.SetText(self.Name,data.tittle)
    UH.SetText(self.Desc,data.desc)
    local item_flag = data.flag or self.data:GetItemFlag(data)
    local reward_list_data = {}
    if data.coin > 0 then
        table.insert(reward_list_data,Item.Create({item_id = CommonItemId.Coin,num = data.coin}))
    end
    AndList(reward_list_data,DataHelper.FormatItemList(data.clear_reward))
    local new_reward_list_data = {}
    for i = 1,#reward_list_data do
        new_reward_list_data[i] = {}
        new_reward_list_data[i].item = reward_list_data[i]
        new_reward_list_data[i].flag = item_flag
    end
    self.RewardList:SetData(new_reward_list_data)
    self.Interactor.Interactable = item_flag ~= -1 and item_flag ~= -2
    if item_flag == -1 then
        local lv_limit = string.format(Language.SchoolExercise.LvLimit,data.level)
        PublicPopupCtrl.Instance:Center(lv_limit)
        UH.SetText(self.StartBtnText,lv_limit)
    else
        UH.SetText(self.StartBtnText,Language.SchoolExercise.Start)
    end
    self.GetedObj:SetActive(item_flag == 1)
    UH.SpriteName(self.Icon,data.icon or "JiHuo")
end

function SchoolExercisePanel:OnStartClick()
    TeamData.Instance:CheckSingle(function ()
        if self.selected_data then
            local item_flag = self.selected_data.flag or self.data:GetItemFlag(self.selected_data)
            if item_flag == -1 then
                local lv_limit = string.format(Language.SchoolExercise.LvLimit,self.selected_data.level)
                PublicPopupCtrl.Instance:Center(lv_limit)
            else
                SchoolExerciseCtrl.Instance:SendExerciseReq(self.selected_data.seq)
            end
        end        
    end)
end

function SchoolExercisePanel:OnItemClick(data)
    -- 切换新的分类时第一下不放音效，避免重合音效
    if self.last_group and self.last_group == data.group then
        AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral3)
    end
    self.last_group = data.group
    self:FlushInfo(data)
end

function SchoolExercisePanel:OnHelpClick()
    PublicPopupCtrl.Instance:OpenTipsPopup({tips_str = Config.language_cfg_auto.textdesc[100].desc})
end

function SchoolExercisePanel:OnCLoseGuideClick()
    self.GuideObj:SetActive(false)
end

function SchoolExercisePanel:OnParentClick()
    local select_data = SchoolExerciseData.Instance:GetPSelectData()
    local jump_data
    for i = 1,#select_data.children do
        if self.data:GetItemFlag(select_data.children[i]) == 0 then
            jump_data = select_data.children[i]
        end
    end
    jump_data = jump_data or select_data.children[1]
    self.AList:ClickChild(jump_data)
end

SchoolExerciseParentItem = SchoolExerciseParentItem or DeclareMono("SchoolExerciseParentItem", UIWidgetBaseItem)

function SchoolExerciseParentItem:SetData(data)
    UH.SetText(self.Name, data.name)
    UIWidgetBaseItem.SetData(self, data)
    local flag = SchoolExerciseData.Instance:GetGroupFlag(data)
    self.Interactor.Interactable = flag ~= -1
    self.AccomplishObj:SetActive(flag == 1)
    --    #49224 缺陷 G6-LineR（版本发布线） 校场演武屏蔽相关红点
    -- local remind_num = flag == 0 and 1 or 0
    -- if self.red_point == nil then
    --     self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(90, 20))
    -- end
    -- self.red_point:SetNum(remind_num)
end

function SchoolExerciseParentItem:OnDestroy()
    if self.red_point ~= nil then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
end

function SchoolExerciseParentItem:OnSelect(selected)
    SchoolExerciseData.Instance:SetPSelectData(self.data)
end

SchoolExerciseChildItem = SchoolExerciseChildItem or DeclareMono("SchoolExerciseChildItem",UIWidgetBaseItem)

function SchoolExerciseChildItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.Name,data.subname)
    local flag = SchoolExerciseData.Instance:GetItemFlag(data)
    data.flag = flag
    self.AccomplishObj:SetActive(flag == 1)
    self.Interactor.Interactable = flag ~= -1 and flag ~= -2
    --    #49224 缺陷 G6-LineR（版本发布线） 校场演武屏蔽相关红点
    -- if self.red_point == nil then
    --     self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(75, 24))
    -- end
    -- local remind_num = flag == 0 and 1 or 0
    -- self.red_point:SetNum(remind_num)
end

function SchoolExerciseChildItem:OnDestroy()
    if self.red_point ~= nil then
        self.red_point:DeleteMe()
        self.red_point = nil
    end
end

function SchoolExerciseChildItem:Click()
    -- if self.ItemClick then
    --     self.ItemClick:Trigger()
    -- end
    if self.Toggle then
        self.Toggle:Trigger()
    end
end

SchoolExerciseRewardItem = SchoolExerciseRewardItem or DeclareMono("SchoolExerciseRewardItem",UIWidgetBaseItem)

function SchoolExerciseRewardItem:SetData(data)
    self.Cell:SetData(data.item)
    self.MaskObj:SetActive(data.flag == 1)
end
