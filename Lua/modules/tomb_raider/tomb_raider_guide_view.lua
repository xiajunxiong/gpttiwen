TombRaiderGuideView = TombRaiderGuideView or DeclareView("TombRaiderGuideView", "tomb_raider/tomb_raider_guide")
function TombRaiderGuideView:TombRaiderGuideView()
    self.data = TombRaiderData.Instance

    UH.SetText(self.desc, Config.language_cfg_auto.textdesc[178].desc) 
    local list = {}
    for k,v in pairs(Config.treasure_contend_auto.skill) do 
        if v.is_dec == 1 then 
            table.insert( list, v )
        end 
    end 
    self.show_list:SetData(list)
end

function TombRaiderGuideView:OnClickClose()
    ViewMgr:CloseView(TombRaiderGuideView)
end


TombRaiderGuideShowItem = TombRaiderGuideShowItem or DeclareMono("TombRaiderGuideShowItem", UIWidgetBaseItem)
function TombRaiderGuideShowItem:TombRaiderGuideShowItem() end

function TombRaiderGuideShowItem:SetData(data)
    UH.SetText(self.name,data.skill_name)
    UH.SetText(self.type,Language.TombRaider.GuideSkillType[data.is_item])
    UH.SetText(self.desc,data.skill_dsc_show)

    self.skill_show:SetActive(data.is_item > 0)
    self.item_show:SetActive(data.is_item == 0)
    if data.is_item > 0 then 
        UH.SpriteName(self.skill_icon, "skill_"..data.sn)
    else
        UH.SpriteName(self.item_icon, "skill_"..data.sn)
    end 
end

TombRaiderSynopsisView = TombRaiderSynopsisView or DeclareView("TombRaiderSynopsisView", "tomb_raider/tomb_raider_synopsis")
function TombRaiderSynopsisView:TombRaiderSynopsisView()
    self.data = TombRaiderData.Instance

end

function TombRaiderSynopsisView:LoadCallback()
    self.operate_index = 1 
    self:FlushPageShow()

    UH.SetText(self.desc_guide, Config.language_cfg_auto.textdesc[178].desc)
    UH.SetText(self.desc_guide2, Config.language_cfg_auto.textdesc[268].desc)
    local list_item = {}
    local list_defind = {}
    local list_attack = {}
    for k,v in pairs(Config.treasure_contend_auto.skill) do 
        if v.is_dec == 1 then 
            table.insert( list_item, v )
        end 
        if v.is_dec == 3 then 
            table.insert( list_attack, v )
        end 
        if v.is_dec == 2 then 
            table.insert( list_defind, v )
        end 
    end 
    self.item_list:SetData(list_item)
    self.defind_list:SetData(list_defind)
    self.attack_list:SetData(list_attack)
end


function TombRaiderSynopsisView:OnClickClose()
    ViewMgr:CloseView(TombRaiderSynopsisView)
end

function TombRaiderSynopsisView:CloseCallback()
    -- LogError("??@", TombRaiderData.Instance.achievement_info.show_flag )
    if not ActivityData.IsOpen(ActType.TombRaider) then return end 
    if TombRaiderData.Instance.achievement_info.show_flag == 1 or 
        TombRaiderData.Instance.achievement_info.show_mark == 1 then 
        return 
    end 


    local info = {
        content = Language.TombRaider.GuideNeed,
        cancel = {
            name = Language.TombRaider.No,
            func = function ()
                TombRaiderCtrl.Instance:SendMatchOperate(5)
                ViewMgr:CloseView(DialogTipsView)
                TombRaiderData.Instance.achievement_info.show_mark = 1
            end
        },
        confirm = {
            name = Language.TombRaider.Yes,
            func = function ()
                ViewMgr:CloseView(DialogTipsView)
                TombRaiderData.Instance.achievement_info.show_mark = 1
            end
        }
    }
    PublicPopupCtrl.Instance:DialogTips(info)
end

function TombRaiderSynopsisView:OnClickPage(index)
    self.operate_index = self.operate_index + index
    if self.operate_index < 0 then 
        self.operate_index = 1
    elseif self.operate_index > 4 then 
        self.operate_index = 4
    end 

    self:FlushPageShow()
end

function TombRaiderSynopsisView:FlushPageShow()
    self.page_left:SetActive(self.operate_index > 1)
    self.page_right:SetActive(self.operate_index < 4)

    for i = 1,4 do 
        self.pages[i]:SetActive(i == self.operate_index)
    end 
end