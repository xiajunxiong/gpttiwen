PetTaleView = PetTaleView or DeclareView("PetTaleView", "pet/pet_tale")

function PetTaleView:PetTaleView()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self,show_shadow = false})
    end
end

function PetTaleView:LoadCallback(paramt)
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(1148))
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
    end
    self.run_ht = Runner.Instance:RunUntilTrue(function ()
        if not self.ui_obj then
            return true
        end
        self.book_obj = self.ui_obj:GetRoot()
        if self.book_obj then
            local obj = self.book_obj.transform:Find("1148")
            if obj then
                obj:GetComponent(typeof(UnityEngine.Animator)):SetInteger("state", 1)
                TimeHelper:CancelTimer(self.time_ht)
                self.time_ht = Invoke(function ()
                    self.PanelObj:SetActive(true)
                    self.ModelShow:SetObjActive(false)
                end,1.2)
                return true
            end
        end
    end)
end

function PetTaleView:CloseCallback()
    TimeHelper:CancelTimer(self.time_ht)
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
        self.run_ht = nil
    end
end

function PetTaleView:OnCloseClick()
    ViewMgr:CloseView(PetTaleView)
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
    end
    if self.run_ht ~= nil then
        Runner.Instance:RemoveRunUntilTrue(self.run_ht)
        self.run_ht = nil
    end
    TimeHelper:CancelTimer(self.time_ht)
end

PetTalePanel = PetTalePanel or DeclareMono("PetTalePanel", UIWFlushPanel)
function PetTalePanel:PetTalePanel()
    self.data = PetTaleData.Instance
    self.page_list = {
        -- {
        --     page_index,  -- 页数
        --     page_count,  -- 总页数
        --     page_type,   -- 类型 1:model_page,2:text_page
        --     page_data,   -- 数据
        -- }
    }
    self.data_cares = {
        {data = self.data.tale_data, func = self.FlushPanel, init_call = false},
    }
    self.page_left = 1
    self.page_right = 2
    self.pet = PetData.Instance:GetBookSelect()
    local cfg = self.pet:PetCfg()
    self.tale_cfg = self.data:GetTaleCfg(cfg.mutant_id)
end

function PetTalePanel:Awake()
    UIWFlushPanel.Awake(self)
    if not self.tale_cfg then
        ViewMgr:CloseView(PetTaleView)
        return
    end

    local guide_flag = UnityPlayerPrefs.GetInt(self:GetPrefsKey())
    self.GuideObj:SetActive(guide_flag ~= 1)
    
    -- 插入封面
    table.insert(self.page_list,{
        page_type = 1,
        page_data = {
            pet_id = self.pet.pet_id,
            jump_btns = {},
        },
    })

    self:HandeText()
    self:FlushPanel()
end

function PetTalePanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    TimeHelper:CancelTimer(self.time_ht)
    TimeHelper:CancelTimer(self.time_ht2)
end

function PetTalePanel:FlushPanel()
    local left_data = self.page_list[self.page_left]
    if left_data then
        self.LeftPage:SetData(left_data)
    end
    local right_data = self.page_list[self.page_right]
    if right_data then
        self.RightPage:SetData(right_data)
    end
end

function PetTalePanel:HandeText()
    local mutant_id = self.pet:PetCfg().mutant_id
    local tale_group = self.data:GetTaleGroup(mutant_id)
    for i,v in pairs(tale_group) do
        if i < 4 or PetFeiShengData.ModUnlocked() then
            local text = v.text
            local info = self:ParseText(text)
            local page_len = #info.pages

            for j,v2 in ipairs(info.pages) do
                local page_data = {}
                page_data.text = v2
                page_data.index = i
                page_data.seq = v.seq
                page_data.mutant_id = v.mutant_id
                page_data.lock_tip = v.lock_tip
                if j == 1 then
                    page_data.title = v.name
                    -- 章节第一页显示奖励
                    page_data.reward = v.item_list
                end
                -- 章节最后一页显示奖励
                -- if j == page_len then
                --     page_data.reward = v.item_list
                -- end

                if not self.data:GetIsUnlock(mutant_id, v.seq) then
                    if not self.max_page_count then
                        local page_count = #self.page_list
                        if page_count % 2 == 1 then
                            table.insert(self.page_list, {page_type = 2, page_data = {}})
                            page_count = page_count + 1
                        end
                        self.max_page_count = page_count
                        self.max_lock_tip = v.lock_tip
                    end
                    table.insert(self.page_list, {page_type = 2, page_data = page_data}) 
                else
                    table.insert(self.page_list, {page_type = 2, page_data = page_data})
                end
            end
        end
    end


    local page_count = #self.page_list
    if page_count % 2 == 1 then
        table.insert(self.page_list, {page_type = 2, page_data = {}})
        page_count = page_count + 1
    end
    -- self.max_page_count 如果此时是空，说明全部解锁了
    if not self.max_page_count then
        self.max_page_count = page_count
    end

    for i,v in pairs(self.page_list) do
        v.page_count = page_count
        v.page_index = i
        if v.page_data.title then
            local page_index = v.page_index
            if #self.page_list[1].page_data.jump_btns == 0 then
                page_index = page_index + 1
            end
            table.insert(self.page_list[1].page_data.jump_btns, {page_index = page_index, title = v.page_data.title,
             mutant_id = v.page_data.mutant_id, seq = v.page_data.seq, lock_tip = v.page_data.lock_tip})
        end
    end
end

function PetTalePanel:ParseText(text)
    local tab = RichTextHelper.ColorStr("ttttt", COLORSTR.Transparent)
	text = tab .. string.gsub(text, "\n", "\n" .. tab)

    local info = {
        lines_count_all = 1,      -- 总行数
        lines_count_page = 1,     -- 每页行数
        pages = {},         -- 每页内容
    }

    self.OneText.text = text
    self.FullText.text = text
    -- UnityTextGenerator.Populate ==== 将使用给定设置为给定字符串生成顶点和其他数据。
    -- GetGenerationSettings ===== 绘制文本范围
    TextExtend.Populate(self.OneText.cachedTextGenerator, text, self.OneText:GetGenerationSettings(Vector2.New(475,475)))
    TextExtend.Populate(self.FullText.cachedTextGenerator, text, self.FullText:GetGenerationSettings(Vector2.New(475,475)))
    local lines_page = TextExtend.lines(self.OneText.cachedTextGenerator)  -- 注意text组件需设置成能显示全部内容的
    local lines_all = TextExtend.lines(self.FullText.cachedTextGenerator)
    info.lines_count_all = lines_all.Count
    info.lines_count_page = lines_page.Count

    local characterCount = TextExtend.characterCount(self.FullText.cachedTextGenerator)     --字符数量
    local startCharIdxs = {}
    for i = 0, lines_all.Count - 1 do
        table.insert(startCharIdxs, TextExtend.startCharIdx(lines_all[i]))  --首个字符在该线上的索引
    end
    local startCharIdx, page_param = 0, info.lines_count_page
    for i = 1, #startCharIdxs do
        if 1 == i % page_param then
            startCharIdx = startCharIdxs[i]
            if i == #startCharIdxs then
                table.insert(info.pages, System.String.New(text):Substring(startCharIdx, (startCharIdxs[i + 1] or (characterCount - 1)) - startCharIdx))
            end
        elseif 0 == i % page_param or i == #startCharIdxs then
            table.insert(info.pages, System.String.New(text):Substring(startCharIdx, (startCharIdxs[i + 1] or (characterCount - 1)) - startCharIdx))
        end
    end

    -- for i,v in pairs(info.pages) do
    --     LogError(v)
    -- end

    self.OneText.text = ""
    self.FullText.text = ""

    return info
end

-- value小于0向前翻页，value大于0向后翻页
function PetTalePanel:OnChangeEvent(value)
    --self:FlushPanel()
    local add_num
    if value < 0 then
        add_num = -2
    else
        add_num = 2
    end
    local page_left = self.page_left + add_num
    local page_right = self.page_right + add_num
    if page_left > 0 and page_right <= self.max_page_count then
        self:BookAimation(page_left)
        self.page_left = page_left
        self.page_right = page_right
        self:FlushPanel()
    elseif self.max_page_count < #self.page_list then
        PublicPopupCtrl.Instance:Error(Language.PetTale.Tip3 .. self.max_lock_tip)
        return
    end
end

-- 跳到指定页面
function PetTalePanel:OnJumpEvent(page_index)
    if page_index == 2 then
        return
    end
    local page_left
    local page_right
    if page_index % 2 == 0 then
        page_right = page_index
        page_left = page_index - 1
    else
        page_left = page_index
        page_right = page_index + 1
    end
    if page_left > 0 and page_right <= self.max_page_count then
        self:BookAimation(page_left)
        self.page_left = page_left
        self.page_right = page_right
        self:FlushPanel()
    end
end

-- 翻页效果
function PetTalePanel:BookAimation(page_left)
    if not page_left or not self.page_left or self.is_book_animation ~= nil then
        return
    end
    if page_left < self.page_left then
        self.is_book_animation = 1
        self.BookAnimtor:Play("UI_CWYWL@zuofanye")
        self.Show:SetActive(false)
        --self.LeftPage:SetObjActive(false)
    elseif page_left > self.page_left then
        self.is_book_animation = 2
        self.BookAnimtor:Play("UI_CWYWL@youfanye")
        self.Show:SetActive(false)
        --self.RightPage:SetObjActive(false)
    end
    TimeHelper:CancelTimer(self.time_ht)
    self.time_ht = Invoke(function ()
        self.is_book_animation = nil
        self.Show:SetActive(true)
        -- self.RightPage:SetObjActive(true)
        -- self.LeftPage:SetObjActive(true)
    end,1)
    -- TimeHelper:CancelTimer(self.time_ht2)
    -- self.time_ht2 = Invoke(function ()
    --     --self.Show:SetActive(true)
    --     self.RightPage:SetObjActive(self.is_book_animation == 2)
    --     self.LeftPage:SetObjActive(self.is_book_animation == 1)
    --     self:FlushPanel()
    -- end,0.5)
end

function PetTalePanel:OnJumpBtnClick(data)
    local is_can = self.data:GetIsUnlock(data.mutant_id, data.seq)
    if not is_can then
        if data.lock_tip then
            PublicPopupCtrl.Instance:Error(data.lock_tip)
        end
        return
    end
    local page_index = data.page_index
    self:OnJumpEvent(page_index)
end

function PetTalePanel:OnCloseGuideClick()
    self.GuideObj:SetActive(false)
    UnityPlayerPrefs.SetInt(self:GetPrefsKey(), 1)
end

function PetTalePanel:GetPrefsKey()
    local key = string.format("%s_PetTalePanelGuide",RoleData.Instance:GetRoleId())
    return key
end

function PetTalePanel:OnClickRatePreview()
    ViewMgr:OpenView(PetTaleRewardPreview,{mutant_id = self.tale_cfg.mutant_id})
end

-- {
--     page_index,  -- 页数
--     page_type,   -- 类型
--     page_data,   -- 数据
-- }
PetTalePageItem = PetTalePageItem or DeclareMono("PetTalePageItem", UIWidgetBaseItem)
function PetTalePageItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.PageNum, string.format("%s/%s", data.page_index, data.page_count))
    self.ModelPage:SetObjActive(data.page_type == 1)
    self.TextPage:SetObjActive(data.page_type == 2)
    if data.page_type == 1 then
        self.ModelPage:SetData(data.page_data)
    elseif data.page_type == 2 then
        self.TextPage:SetData(data.page_data)
    end
end

-- {
--     pet_id,      -- 宠物id
--     jump_btns    -- 章节跳转btn
-- }
PetTalePageModelItem = PetTalePageModelItem or DeclareMono("PetTalePageModelItem", UIWidgetBaseItem)
function PetTalePageModelItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local cfg = PetData.Instance:GetPetCfg(data.pet_id)
    if not self.model_res_list then
        local pet_id = cfg.prototype_id
        local mutant_id = PetData.Instance:GetMutantId(pet_id)
        local mutant_id_2 = PetData.Instance:GetMutantId(pet_id,2)
        self.model_res_list = {}
        table.insert(self.model_res_list, PetData.Instance:GetPetCfg(pet_id))
        table.insert(self.model_res_list, PetData.Instance:GetPetCfg(mutant_id))
        if PetFeiShengData.ModUnlocked() then
            table.insert(self.model_res_list, PetData.Instance:GetPetCfg(mutant_id_2))
        end
    end
    self.model_index = self.model_index or 1
    self:ShowModel()

    local tale_cfg = PetTaleData.Instance:GetTaleCfg(cfg.mutant_id)
    UH.SetText(self.Text1, string.format(Language.PetTale.Txt1,Language.Pet.RaceClass[cfg.race]))
    UH.SetText(self.Text2, string.format(Language.PetTale.Txt2,tale_cfg.special))
    UH.SetText(self.Text3, string.format(Language.PetTale.Txt3,tale_cfg.special_1))
    UH.SetText(self.Text4, string.format(Language.PetTale.Txt4,tale_cfg.manor))

    self.TitleList:SetData(data.jump_btns)
end

function PetTalePageModelItem:ShowModel()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({ui_obj = self.ui_obj,view = self})
    end
    local res_id = self.model_res_list[self.model_index].res_id
    local name = self.model_res_list[self.model_index].name
    UH.SetText(self.Name, name)
    self.ui_obj:SetData(DrawerHelper.GetNpcPath(res_id))
end

function PetTalePageModelItem:OnDestroy()
    if self.ui_obj ~= nil then
		self.ui_obj:DeleteMe()
	end
end

function PetTalePageModelItem:OnChangeModelClick(value)
    local model_index = self.model_index + value
    if model_index < 1 then
        model_index = #self.model_res_list
    end
    if model_index > #self.model_res_list then
        model_index = 1
    end
    self.model_index = model_index
    self:ShowModel()
end

 
PetTaleTitleItem = PetTaleTitleItem or DeclareMono("PetTaleTitleItem", UIWidgetBaseItem)
-- data == {page_index = page_index, title = v.page_data.title, mutant_id = v.mutant_id, seq = v.seq}
function PetTaleTitleItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data.title)
    local is_unlock = PetTaleData.Instance:GetIsUnlock(data.mutant_id, data.seq)
    self.Interactor.Interactable = is_unlock
    local remind_num = PetTaleData.Instance:GetTaleRewardFlag(data.mutant_id, data.seq) and 1 or 0
    self.RedPoint:SetNum(remind_num)
end

-- {
--     title,  -- 标题
--     text,   -- 内容
--     reward, -- 奖励
--     index,  -- 第几个传记
-- }
PetTalePageTextItem = PetTalePageTextItem or DeclareMono("PetTalePageTextItem", UIWidgetBaseItem)
function PetTalePageTextItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local is_null = TableIsEmpty(data)
    self.Show:SetActive(not is_null)
    self.Null:SetActive(is_null)
    if is_null then
        return
    end
    UH.SetText(self.Title, data.title)
    UH.SetText(self.Text, data.text)
    self.Reward:SetActive(data.reward ~= nil)
    if not TableIsEmpty(data.reward) then
        local pet = PetData.Instance:GetBookSelect()
        local mutant_id = pet:PetCfg().mutant_id
        local can_get = PetTaleData.Instance:GetTaleRewardFlag(mutant_id,data.seq)
        self.Interactor.Interactable = can_get
        local reward_list = {}
        for i,v in pairs(data.reward) do
            local data = TableCopy(v)
            --data.is_grey = not can_get
            table.insert(reward_list, data)
        end
        self.RewardList:SetData(DataHelper.FormatItemList(reward_list))
        self.RedPoint:SetNum(can_get and 1 or 0)

        local reward_btn_text = can_get and Language.PetTale.Txt7 or Language.PetTale.Txt8
        UH.SetText(self.RewardBtnText,reward_btn_text)
    end
end

function PetTalePageTextItem:OnGetRewardClick()
    self.RewardInfo:SetActive(not self.RewardInfo.activeSelf)
end

function PetTalePageTextItem:OnGetClick()
    local pet = PetData.Instance:GetBookSelect()
    local mutant_id = pet:PetCfg().mutant_id
    if PetTaleData.Instance:GetTaleRewardFlag(mutant_id,self.data.seq) then
        PetCtrl.SendPetOp(PET_OP_TYPE.TaleReward, mutant_id, self.data.seq)
    else
        PublicPopupCtrl.Instance:Error(Language.PetTale.Tip2)
    end
end


PetTaleRewardPreview = PetTaleRewardPreview or DeclareView("PetTaleRewardPreview","pet/pet_tale_reward_review")
-- Board = UIWSubstitute(UIWBoard3)
-- RateInfoList = UIWidgetList
function PetTaleRewardPreview:PetTaleRewardPreview()
    self.Board:SetData(self:GetType(), Language.PetTale.Txt5, Vector2.New(907, 556))
end

function PetTaleRewardPreview:LoadCallback(param_t)
    param_t = param_t or {}
    local mutant_id = param_t.mutant_id
    if not mutant_id then
        return
    end

    local gorup_cfg = PetTaleData.Instance:GetTaleGroup(mutant_id)
    local list_data = {}
    for i,v in pairs(gorup_cfg) do
        if i < 4 or PetFeiShengData.ModUnlocked() then
            local data = {}
            data.title = v.reward_title or ""
            data.reward = v.item_list
            table.insert(list_data, data)
        end
    end
    self.RateInfoList:SetData(list_data)
end


PetTaleRewardPreviewItem = PetTaleRewardPreviewItem or DeclareMono("PetTaleRewardPreviewItem", UIWidgetBaseItem)
-- ItemList = UIWidgetList
-- QuaImage = UIImageSpriteSet
-- RateText = Text

function PetTaleRewardPreviewItem:SetData(data)
    self.data = data
    UH.SetText(self.Title, data.title)
    self.ItemList:SetData(DataHelper.FormatItemList(self.data.reward))
    self.ScrollRect.enabled = #self.data.reward > 3
end