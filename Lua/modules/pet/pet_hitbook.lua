PetHitBookPanel = PetHitBookPanel or DeclareMono("PetHitBookPanel", UIWFlushPanel)
-- GridItemNum = 12
-- SkilIconList = UIWidgetList
-- ModelShow = ModelUIShow
-- PetName = Text
-- BottomList = UIWidgetList
-- HaveImgObj = GameObject
-- TypeBgSetter = UIWTextureSetter
function PetHitBookPanel:PetHitBookPanel()
    self.lang = Language.Pet
    self.pet_data = PetData.Instance
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self,
            show_shadow = true,
            shadow_dir = ModelUIShow.ShadowCameraDir.Pet,
        })
    end
    self.showAlreadyHave = false
    self.curResid = -1
    self.data_cares = {
        {data = self.pet_data.pet_select, func = self.FlushPetInfo, keys = {"index"}},
        {data = self.pet_data.pet_select, func = self.FlushSkillIconList, keys = {"index"}},
        {data = self.pet_data.hit_book_type, func = self.FlushSkillIconList, keys = {"index"}},
        {data = BagData.Instance.item_grid,func = self.FlushSkillIconList},
        {data = BagData.Instance.item_grid,func = self.FlushGiftList, init_call = false},
        {data = self.pet_data.pet_list,func = self.FlushPetInfo, init_call = false},
    }

    GuideManager.Instance:RegisterGetGuideUi("PetHitBookSelectHave", function ()
        local item_id = GuideData.Instance:GetPetHitBookSkillItem()
        for i=1,4 do
            local item = self.SkillIconList:GetItem(i)
            if item then
                local data = item.ItemGrid:GetData()
                for k,v in pairs(data) do
                    if v.item_id == item_id then
                        if i >= 2 then
                            self.SkillIconPages:ScrollToPage(i - 1)
                        end
                        return item.ItemGrid:GetItem(k), function() item.ItemGrid:ClickItem(k) end    
                    end
                end
            end
        end
    end)
    GuideManager.Instance:RegisterGetGuideUi("PetThirdLingWu", function ()
        local pet = self.pet_data:GetCurSelect()
        local gift_list = pet:GetHitBookSkillAndGiftList()
        local single_main = false
        if gift_list[2].main_skill and gift_list[2].skill_id == 0 then
            single_main = true
        end
        local index = 0
        for i,v in ipairs(gift_list) do
            if not v.main_skill and v.skill_id == 0 then
                index = i
                break
            end
        end
        if self.PetSkillItemListFirst then
            if not single_main then
                if index > self.PetSkillItemListFirst:Length() - 5 then
                    local item_parent = self.ScrollGiftList:GetItem(1)
                    if item_parent ~= nil then
                        local item = item_parent.GiftItemList:GetItem(index - (self.PetSkillItemListFirst:Length() - 4))
                        if item ~= nil then
                            return item
                        end
                    end
                else
                    return self.PetSkillItemListFirst[index + 4]
                end
            elseif index >= self.PetSkillItemListFirst:Length() - 4 then
                local item_parent = self.ScrollGiftList:GetItem(1)
                if item_parent ~= nil then
                    local item = item_parent.GiftItemList:GetItem(index - self.PetSkillItemListFirst:Length() - 3)
                    if item ~= nil then
                        return item
                    end
                end
            else
                return self.PetSkillItemListFirst[index + 4]
            end
        end
    end)
    -- LogError("auto_click_book_page",self.pet_data.auto_click_book_page, self.pet_data.auto_click_book_config)
    if self.pet_data.auto_click_book_page and self.pet_data.auto_click_book_config then
        for i=1,self.PageToggleList:Length() do
            self.PageToggleList[i].isOn = i == self.pet_data.auto_click_book_page
        end
        self:OnClickGiftToggle(self.pet_data.auto_click_book_page - 1)
        local function auto_click_runner()
            local clicked = false
            for i=1,4 do
                local item = self.SkillIconList:GetItem(i)
                if item then
                    local data = item.ItemGrid:GetData()
                    for k,v in pairs(data) do
                        if v.item_id == self.pet_data.auto_click_book_config.id then
                            if i >= 2 then
                                self.delay_scroll_page = TimeHelper:AddDelayTimer(function ()
                                    self.SkillIconPages:ScrollToPage(i - 1)
                                    item.ItemGrid:ClickItem(k)
                                end,0.1)
                            else
                                item.ItemGrid:ClickItem(k)
                            end
                            clicked = true
                            self.pet_data.auto_click_book_page = nil
                            self.pet_data.auto_click_book_config = nil
                            break
                        end
                    end
                    if clicked then
                        break
                    end
                end
            end
            if not clicked then
                self.auto_click_runner = TimeHelper:AddRunFrameTimer(function ()
                    auto_click_runner()
                end)
            end
        end
        self.auto_click_runner = TimeHelper:AddRunFrameTimer(function ()
            auto_click_runner()
        end)
    else
        self.pet_data.auto_click_book_page = nil
        self.pet_data.auto_click_book_config = nil
    end
end

function PetHitBookPanel:OnEnable()
    UIWFlushPanel.OnEnable(self)
    self.pet_data:SetSelTab(Mod.Pet.PetGetBook)
end

function PetHitBookPanel:OnDisable()
    UIWFlushPanel.OnDisable(self)
    self.pet_data:SetSelTab(-1)
end

function PetHitBookPanel:FlushPetInfo(old)
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        -- LogError("PetHitBookPanel return")
        return
    end
    self.new_index = 0
    if type(old) == "table" and old[pet.index] and old[pet.index].gift_list then
        --比较变化播特效
        local list1 = old[pet.index].gift_list
        local list2 = pet.gift_list
        for index, value in ipairs(list2) do
            if value.gift_id ~= list1[index].gift_id then
                self.new_index = index
                break
            end
        end
    end
    UH.SetText(self.PetName, pet.name)
    if pet:Rank() >= 0 and pet:Rank() <= 9 then
        self.PetRankObj:SetActive(true)
        self.PetRankObj:SetRank(pet:Rank() + 1 )
    else
        self.PetRankObj:SetActive(false)
    end
    local cfg2 = Cfg.PetInfo2ById(pet:ID())
    if self.last_score ~= nil and self.last_score ~= pet:GetScore() and self.last_pet_index and self.last_pet_index == pet.index then
        self.EffectTool:Play(2161043)
        self.ScoreAnim:Play()
    end
    self.last_score = pet:GetScore()
    self.last_pet_index = pet.index
    self.ScoreItem:SetScore(pet:GetScore())
    self.Mutant:SetActive(cfg2.mutant_times >= PetData.MutantLv.One)
    -- self.MutantBg:SetActive(cfg2.mutant_times >= 1)
    self.NormalBg:SetActive(cfg2.mutant_times == PetData.MutantLv.Zero and cfg2.quality ~= PetData.Quality.Shen)
    -- self.ShenShouBg:SetActive(cfg2.mutant_times == 0 and cfg2.quality == 5)

    PetView.SetTypeBgImage(self.TypeBgSetter,cfg2.quality,cfg2.mutant_times,true,pet:IsLianYao())

    if not StringIsEmpty(cfg2.limit_type) then
        self.LimitSign:SetObjActive(true)
        self.LimitSign:SetType(pet:LimitType())
    else
        self.LimitSign:SetObjActive(false)
    end
    UH.SetText(self.PetTypeText, self.lang.RaceClass[pet.config.race])
    UH.SpriteName(self.PetTypeImg, string.format("Race%s", pet:RaceId()))
    UH.SpriteName(self.PetTypeNameImg, string.format("_LocRaceName%s", pet:RaceId()))
    UH.SpriteName(self.LevelSprite, PetData.PetLevel[pet.config.quality])
    
    if self.curResid == -1 or self.curResid ~= pet:ResId() then
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()),pet:FeiShengTimes())
        self.ui_obj:PlayAnim(ChAnimType.Show)
        self.curResid = pet:ResId()
    end
    self:FlushGiftList()
    self:FlushPetAmulet(pet)
    self:FlushPetBagde()
    if pet:NewGiftData() ~= nil and not ViewMgr:IsOpen(PetGiftReplace) then
        ViewMgr:OpenView(PetGiftReplaceResultView)
    end
end

function PetHitBookPanel:FlushPetAmulet(pet)

    local pet = pet or self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end

    local item = pet:GetPetEquipItem()
    local data = {item = item, pet = pet}
    self.PetAmuletPart:SetData(data)
end

function PetHitBookPanel:FlushPetBagde()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        return
    end

    local bagde_item = pet:BagdeItem()
    local data = {pet_bagde = bagde_item, pet = pet}
    self.PetBagdePart:SetData(data)
end

function PetHitBookPanel:FlushGiftList()
    local pet = self.pet_data:GetCurSelect()
    if pet == nil then
        -- LogError("PetHitBookPanel return")
        return
    end
    local gift_list = TableCopy(pet:GetHitBookSkillAndGiftList())
    if self.new_index == nil then
        self.new_index = 0
    end
    -- LogError("gift_list",gift_list)
    if self.new_index > 0 then
        gift_list[self.new_index + 1].is_new_skill = true
    end

    local single_main = false   --是否正在刷主动技能的list
    local cur_showed_count = 0

    --第一行技能
    for i=1,self.PetSkillItemListFirst:Length() do
        local cur_gift = gift_list[1]
        if not single_main and i == 6 then
            single_main = true --是否正在显示被动技能列表
        end
        if cur_showed_count >= 5 or cur_gift == nil then
            self.PetSkillItemListFirst[i]:SetObjActive(false)
        else
            if cur_gift.main_skill or cur_gift.fs_skill then
                if cur_gift.main_skill and cur_gift.skill_id == 0 then
                    self.PetSkillItemListFirst[i]:SetObjActive(false)  
                else
                    self.PetSkillItemListFirst[i]:SetData(cur_gift)
                    self.PetSkillItemListFirst[i]:SetObjActive(true)  
                    cur_showed_count = cur_showed_count + 1
                end
                table.remove(gift_list,1)
            else
                if single_main then
                    self.PetSkillItemListFirst[i]:SetData(cur_gift)
                    self.PetSkillItemListFirst[i]:SetObjActive(true)  
                    cur_showed_count = cur_showed_count + 1
                    table.remove(gift_list,1)  
                else
                    self.PetSkillItemListFirst[i]:SetObjActive(false)    
                end
            end
        end
        -- if gift_list[1] then
        --     if gift_list[1].main_skill and gift_list[1].skill_id == 0 then
        --         self.PetSkillItemListFirst[i]:SetObjActive(false)
        --         single_main = true
        --     else
        --         self.PetSkillItemListFirst[i]:SetData(gift_list[1])
        --         self.PetSkillItemListFirst[i]:SetObjActive(true)
        --     end
        --     if i == 3 then
        --         self.SkillListGiftTitleObj:SetActive(true)
        --     end
        -- else
        --     if i == 3 then
        --         self.SkillListGiftTitleObj:SetActive(false)
        --     end
        --     self.PetSkillItemListFirst[i]:SetObjActive(false)
        -- end
        -- table.remove(gift_list,1)
        -- if single_main then
        --     if i == self.PetSkillItemListFirst:Length() then
        --         break
        --     end
        -- elseif i == self.PetSkillItemListFirst:Length() - 1 then
        --     self.PetSkillItemListFirst[i + 1]:SetObjActive(false)
        --     break
        -- end
    end

    local second_list = {}
    local count = 1
    for i,v in ipairs(gift_list) do
        second_list[count] = second_list[count] or {}
        if #second_list[count] >= 6 then
            count = count + 1
            second_list[count] = {}
        end
        table.insert(second_list[count], v)
        
    end
    self.ScrollGiftList:SetData(second_list)
    self.ScrollGiftListRect.enabled = count >= 2
    self.Arrow:SetActive(count >= 2)
    if count < 2 then
        UH.AnchoredPosition(self.ScrollGiftContent, Vector2.New(0,0))
    end
end

--刷新右侧技能列表（所有或背包中有的）
function PetHitBookPanel:FlushSkillIconList()
    if self.pet_data:GetCurSelect() == nil then
        return
    end
    local full_config = self.pet_data:GetHitBookPageList()
    local show_data_list = {}
    local add_index = 1
    -- if self.showAlreadyHave then
        for k,v in pairs(full_config) do
            v.show_has = self.showAlreadyHave
            local has_num = Item.GetNum(v.item_id)
            v.has_num = has_num
            if has_num > 0 and self.showAlreadyHave then
                table.insert(show_data_list,add_index, v)
                add_index = add_index + 1
            else
                table.insert(show_data_list,v)
            end
        end
    -- else
    --     show_data_list = full_config
    -- end
    local grid_page_data = {}
    local index = 1
    local pageIndex = 1
    for i,v in ipairs(show_data_list) do
        if index > self.GridItemNum then
            pageIndex = pageIndex + 1
            index = 1
        end
        grid_page_data[pageIndex] = grid_page_data[pageIndex] or {}
        table.insert(grid_page_data[pageIndex], v)
        index = index + 1
    end
    self.SkillIconList:SetData(grid_page_data)
end

function PetHitBookPanel:OnClickAlreadyHave()
    AudioMgr:PlayEffect(AudioType.UI, "bag_tidy")
    self.showAlreadyHave = not self.showAlreadyHave
    self.HaveImgObj:SetActive(self.showAlreadyHave)
    self:FlushSkillIconList()
    --刷新列表
end

function PetHitBookPanel:OnClickTipsButton()
    PublicPopupCtrl.Instance:HelpTip(Config.language_cfg_auto.textdesc[39].desc)
end

function PetHitBookPanel:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
    self.curResid = -1
    self.showAlreadyHave = false
    self.pet_data:SetHitBookType(0)
    GuideManager.Instance:UnRegisterGetGuideUi("PetHitBookSelectHave")
    GuideManager.Instance:UnRegisterGetGuideUi("PetThirdLingWu")
    if self.auto_click_runner then
        TimeHelper:CancelTimer(self.auto_click_runner)
        self.auto_click_runner = nil
    end
    if self.delay_scroll_page then
        TimeHelper:CancelTimer(self.delay_scroll_page)
        self.delay_scroll_page = nil
    end
end

function PetHitBookPanel:OnClickGiftToggle(param)
    AudioMgr:PlayEffect(AudioType.UI, "click_tongyong3")
    self.pet_data:SetHitBookType(param)
end

function PetHitBookPanel:OnClickGift(cell)
    -- local data = cell:GetData()
    -- if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
    --     self.pet_data:SetClickSkill(data)
    --     ViewMgr:OpenView(PetSkillTip)
    -- elseif data.show_lingwu then
    --     PetLingwuSkillTip.SetSlotIndex(data.index)
    --     ViewMgr:OpenView(PetLingwuSkillTip)
    -- end
    PetGiftItem.CommonClickFunc(cell)
end

function PetHitBookPanel:OnClickSuggest()
    ViewMgr:OpenView(PetGiftSuggest)
end
---------------------------------------------------
PetHitBookGiftListItem = PetHitBookGiftListItem or DeclareMono("PetHitBookGiftListItem", UIWidgetBaseItem)
function PetHitBookGiftListItem:PetHitBookGiftListItem()
    
end

function PetHitBookGiftListItem:SetData(data)
    self.GiftItemList:SetData(data)
    self.data = data
end

----------------------------------------------------
PetHitBookPageItem = PetHitBookPageItem or DeclareMono("PetHitBookPageItem", UIWidgetBaseItem)
-- ItemGrid = UIWidgetList
function PetHitBookPageItem:PetHitBookPageItem()
end

function PetHitBookPageItem:SetData(data)
    self.ItemGrid:SetData(data)
end

----------------------------------------------------
--打书界面宠物技能列表格子 右侧
PetHitBookSkillItem = PetHitBookSkillItem or DeclareMono("PetHitBookSkillItem", UIWidgetBaseItem)
-- HasNumText = Text
-- Icon = UIImageSpriteSet
-- IconClick = LuaUIEventClick
-- Selector = Toggle
-- Interactor = InteractorHandler
-- HasNumBgObj = GameObject
-- SkillName = Text
-- HasLearn = GameObject
-- IconImage = Image
function PetHitBookSkillItem:PetHitBookSkillItem()
end

function PetHitBookSkillItem:SetData(data)
    if data.show_has and data.has_num <= 0 then
        self.data = data
        self:ClearData()
        return
    end
    self.IconImage.enabled = true
    
    UH.SetIcon(self.Icon,data.icon_id,ICON_TYPE.SKILL)
    local cfg = PetData.Instance:GetGiftDetail(data.skill_id or data.gift_id)
    UH.SetText(self.SkillName, cfg.name)
    local pet = PetData.Instance:GetCurSelect()
    if pet then
        local has_learned = pet:HasLearned(data.gift_id)
        self.HasLearn:SetActive(has_learned)
        self.LearnMengban:SetActive(has_learned)
    else
        self.HasLearn:SetActive(false)
        self.LearnMengban:SetActive(false)
    end
    if data.has_num > 0 then
        UH.SetText(self.HasNumText, string.format(Language.Pet.HasFormat, data.has_num))
    end
    self.HasNumBgObj:SetActive(data.has_num > 0)
    data.show_learn = not has_learned and data.item_id ~= 0
    UH.SpriteName(self.SkillQuaImg, PetData.GiftListItemFrameName[data.page])
    self.SkillQuaObj:SetActive(true)
    self.FrameObj:SetActive(false)
    self.data = data
end

function PetHitBookSkillItem:ClearData()
    UH.SetIcon(self.Icon)
    UH.SetText(self.SkillName,"")
    self.HasNumBgObj:SetActive(false)
    self.IconImage.enabled = false
    self.HasLearn:SetActive(false)
    self.LearnMengban:SetActive(false)
    self.SkillQuaObj:SetActive(false)
    self.FrameObj:SetActive(true)
    -- self.IconObj:SetActive(false)
end

function PetHitBookSkillItem:Click()
    if self.data.has_num <= 0 then
        GuideManager.Instance:ForceStop()
        if self.data.show_has then
            return
        end
    end
    PetData.Instance:SetClickSkill(self.data)
    ViewMgr:OpenView(PetSkillTip)
end
-------------------------------------------------
------领悟技能弹框
PetLingwuSkillTip = PetLingwuSkillTip or DeclareView("PetLingwuSkillTip", "pet/pet_lingwutip")
-- Item = UIWItemCell
-- NumText = Text
-- TipStr = Text
function PetLingwuSkillTip.SetSlotIndex(index)
    index = index < 2 and 2 or index
    PetLingwuSkillTip.SlotIndex = tostring(index - 2)
end

function PetLingwuSkillTip:PetLingwuSkillTip()
    if self.item_num_care_handle == nil then
        self.item_num_care_handle = BagData.Instance:Care(BindTool.Bind(self.FlushItemNum, self))
    end
    GuideManager.Instance:RegisterGetGuideUi("PetLingWuConfirm",function ()
        return self.PetLingWuConfirmBtn, function ()
            if self.countEnough then
                PetCtrl.SendPetOp(PET_OP_TYPE.GuideComprehendPassiveSkill, PetData.Instance:GetCurSelect().index)
            else
                MallData.JumpItemNum=self.lerp
                MallCtrl.Instance:Jump(self.stuff_cfg.stuff_id,self.lerp)
            end
            ViewMgr:CloseView(PetLingwuSkillTip)            
        end
    end)
end

function PetLingwuSkillTip:LoadCallback(param_t)
    self.cell_data = param_t.data
    self:FlushItemNum()
end

function PetLingwuSkillTip:CloseCallback()
    if self.item_num_care_handle ~= nil then
        BagData.Instance:UnCare(self.item_num_care_handle)
        self.item_num_care_handle = nil
    end
    GuideManager.Instance:UnRegisterGetGuideUi("PetLingWuConfirm")
end

function PetLingwuSkillTip:FlushItemNum()
    local pet = PetData.Instance:GetCurSelect()
    self.cur_pet_index = pet.index
    local petInfo2 = pet.config
    if petInfo2 == nil then
        return
    end
    local stuff_cfg = PetData.GetSlotTypeStuffCfg(petInfo2.slot_type)
    local stuff_id = stuff_cfg.stuff_id
    if self.cell_data and self.cell_data.is_yao_skill == true then
        PetLingwuSkillTip.SlotIndex = self.cell_data.index - 1
        stuff_cfg = PetRefiningData.Instance.other_info
        stuff_id = stuff_cfg.addskill_unlock
        self.click_yao_skill = true
    end
    self.stuff_cfg = stuff_cfg
    local need_stuff = stuff_cfg["stuff_num"..PetLingwuSkillTip.SlotIndex]
    -- LogError("PetLingwuSkillTip", PetLingwuSkillTip.SlotIndex, need_stuff, stuff_cfg)
    local has_num = Item.GetNum(stuff_id)
    local item = Item.Init(stuff_id)
    self.Item:SetData(item)
    self.countEnough = has_num >= need_stuff
    self.lerp=need_stuff-has_num
    local num_str = self.countEnough and string.format(Language.Common.Cstr, COLORSTR.Green7, has_num, need_stuff)
        or string.format(Language.Common.Cstr, COLORSTR.Red10, has_num, need_stuff)
    UH.SetText(self.NumText, num_str)
    local tipStr = string.format(Language.Pet.PetLinewuTipStr, need_stuff)
    UH.SetText(self.TipStr, tipStr)
end

function PetLingwuSkillTip:OnClickCancel()
    ViewMgr:CloseView(PetLingwuSkillTip)
end

function PetLingwuSkillTip:OnclickOk()
    if self.countEnough then
        if self.click_yao_skill == true then
            PetCtrl.SendPetOp(PET_OP_TYPE.ComprehendYao, self.cur_pet_index)
        else
            PetCtrl.SendPetOp(PET_OP_TYPE.ComprehendGift, self.cur_pet_index)
        end
    else
        MallData.JumpItemNum = self.lerp
        MarketData.Instance:SetJumpIndex(self.stuff_cfg.stuff_id)
        ViewMgr:OpenViewByKey(Mod.Mall.Market)
    end
    ViewMgr:CloseView(PetLingwuSkillTip)
end



-- 宠物徽章部件
PetBagdePartItem = PetBagdePartItem or DeclareMono("PetBagdePartItem", UIWidgetBaseItem)
-- data ::: {pet_bagde}
function PetBagdePartItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = data.pet_bagde
    self.Qua:SetObjActive(item ~= nil)
    if item then
        UH.SetIcon(self.Icon, item:IconId())
        local qua = item:QuaLevel()
        UH.SpriteName(self.Qua, "YuanPinZhiKuangXiao" .. qua)
    end
    self.RedPoint:SetNum(0)
    if data.pet then
        local remind_num = PetData.Instance:PetBagdeRemind(data.pet)
        self.RedPoint:SetNum(remind_num)
    end
end

function PetBagdePartItem:OnClick()
    if self.data.pet_bagde then
        local info = {
            item_list = {self.data.pet_bagde},
            func = function ()
                    return { ItemInfoFunc.PetBagdeReplace, ItemInfoFunc.PetBagdeUnfix}
                end
            }
        ItemInfoCtrl.Instance:ItemInfoView(info)
    else
        local list = PetBagdeData.Instance:GetBagdeList()
        if TableIsEmpty(list) then
            local tip_text = Language.PetBagde.BagEmptyTip
            local param_t = {
                content = tip_text,
                confirm = {
                    func =  function()
                        ViewMgr:OpenViewByKey(Mod.Challenge.RareDungeon)
                        ViewMgr:CloseView(DialogTipsView)
                        ViewMgr:CloseView(PetView)
                    end,
                    name = Language.PetWeapon.Goto,
                },
            }
            PublicPopupCtrl.Instance:DialogTips(param_t)
            return
        end
        ViewMgr:OpenView(PetBagdeSelect)
    end
end


-- 宠物护符部件
PetAmuletPartItem = PetAmuletPartItem or DeclareMono("PetAmuletPartItem", UIWidgetBaseItem)
-- data ::: {item}
function PetAmuletPartItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local item = data.item
    self.ErrorBtn:SetActive(item ~= nil and item.item_id <= 0)
    if item ~= nil and item.item_id <= 0 then
        return
    end
    self.Qua:SetObjActive(item ~= nil)
    if item then
        UH.SetIcon(self.Icon, item:IconId())
        local qua = item:Color()
        UH.SpriteName(self.Qua, "YuanPinZhiKuangXiao" .. qua)
    end
    self.RedPoint:SetNum(0)
    if data.pet and not data.item then
        local pet = data.pet
        if pet.index == PetData.Instance:GetFightIndex() or PetData.Instance:GetIsStandBy(pet.index) then
            if next(BagData.Instance:GetPetEquipmentList()) then
                self.RedPoint:SetNum(1)
            end
        end
    end
end

function PetAmuletPartItem:OnClick()
    if self.data.item then
        local info = {
            item_list = {self.data.item},
            func = function ()
                local func_t = { ItemInfoFunc.PetEquipedInstead, ItemInfoFunc.PetEquipReset, ItemInfoFunc.PetEquipAssembly}
                if AmuletData.Instance:IsCanIntensify(self.data.item) then
                    table.insert(func_t,ItemInfoFunc.PetEquipInstensify)
                end
                table.insert(func_t,ItemInfoFunc.PetEquipedRemove)
                return func_t
            end
        }
        ItemInfoCtrl.Instance:ItemInfoView(info)
    else
        local equip_list = BagData.Instance:GetPetEquipmentList()
        if #equip_list > 0 then
            PetData.Instance.pet_equip_select.equip = 1
            PetData.Instance.pet_equipment_select_another_open = false
            ViewMgr:OpenView(PetEquipSelectView)
        else
            ViewMgr:OpenView(AmuletGetwayView)
            --PublicPopupCtrl.Instance:Center(ErrorText[151])
        end
    end
end

function PetAmuletPartItem:OnAmuletErrorClick()
    if MainViewData.Instance:IsBattle() then
		PublicPopupCtrl.Instance:Center(ErrorText[110])
	end
	PetCtrl.Instance:SendRemoveEquip()
end