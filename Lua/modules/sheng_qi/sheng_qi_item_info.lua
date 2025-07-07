-- 圣器基础信息
ShengQiItemBaseInfo = ShengQiItemBaseInfo or DeclareMono("ShengQiItemBaseInfo", UIWidgetBaseItem)

-- data ：PetWeapon
function ShengQiItemBaseInfo:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self:FlushInfo()
end

function ShengQiItemBaseInfo:FlushInfo()
    local level_desc = string.format(Language.ShengQi.Level1, self.data:Level())
    UH.SetText(self.Level, level_desc)
    local score_desc = string.format(Language.ShengQi.ScoreDesc, self.data:Score())
    UH.SetText(self.Score, score_desc)
end


-- 圣器物品信息
ShengQiItemInfo = ShengQiItemInfo or DeclareMono("ShengQiItemInfo", ItemInfoViewItem)

function ShengQiItemInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    UH.SetText(self.Title, Language.ShengQi.Text4)
    self.item = self.info.item_list[self.ShowType]


    local is_param = self.item:HasParam()
    for i = 1, self.InfoList:Length() do
        self.InfoList[i]:SetActive(is_param)
    end
    for i = 1, self.PreviewList:Length() do
        self.PreviewList[i]:SetActive(not is_param)
    end
    if not is_param then
        self:FlushPreview()
    else
        self:FlushParam()
    end

    UH.SetText(self.Prof, self.item:ProfDesc())
end

function ShengQiItemInfo:FlushPreview()
    local pet_attri_list = self.item:MaxPreviewPetAttriList()
    self.PetAttriPreviewList:SetData(pet_attri_list)
    local attri_list = self.item:AttriPreviewList()
    self.AttriPreviewList:SetData(attri_list)
end

function ShengQiItemInfo:FlushParam()
    local active_attri,attri_list_data = self.item:AttriShowList()
    self.AttriOne:SetData(active_attri)
    self.AttriList:SetData(attri_list_data or {})

    local talen_names = ""
    local talen_list = self.item:GetTalentSkillList()
    local show_talen_list = {}
    for i,v in pairs(talen_list) do
        if v.skill_id > 0 then
            talen_names = talen_names .. string.format("%s  ",v:QuaName())
            table.insert(show_talen_list, v)
        end
    end
    self.show_talen_list = show_talen_list
    UH.SetText(self.TalenSKillNames,talen_names)

    local init_skill_list = {}
    for i,v in pairs(self.item:GetInitativeSkillList() or {}) do
        if v.skill_id > 0 then
            table.insert(init_skill_list, v)
        end
    end
    self.SKillNameList:SetData(init_skill_list)
    self.init_skill_list = init_skill_list

    local fushi_list = {}
    for i,v in pairs(self.item:GetAllFushi()) do
        if v.item_id > 0 then
            table.insert(fushi_list,v)
        end
    end
    self.FuShiList:SetData(fushi_list)
    self.FuShiInfo:SetActive(not TableIsEmpty(fushi_list))
    self.FuShiTitle:SetActive(not TableIsEmpty(fushi_list))

    local pet_attri_list = self.item:PetAttriList()
    self.RoleAttriList:SetData(pet_attri_list)
end

function ShengQiItemInfo:OnInitSkillPreview()
    local param_t = {show_type = self.ShowType,pos = UH.GetRootPosition(self.gameObject)}
    param_t.pos = self.ShowType == 2 and param_t.pos + Vector2.New(436,0) or param_t.pos
    param_t.skill_list = self.init_skill_list

    local info_rect = self.ParentMono.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
    param_t.h = info_rect.sizeDelta.y

	if ItemInfoCtrl.Instance:Data().derive_view then 
		ItemInfoCtrl.Instance:ItemDeriveView()
		if not ViewMgr:IsOpen(ShengQiItemPreSkill) then 
			ItemInfoCtrl.Instance:ItemDeriveView(ShengQiItemPreSkill,param_t)
	    end
	else 
		ItemInfoCtrl.Instance:ItemDeriveView(ShengQiItemPreSkill,param_t)
	end 
end

function ShengQiItemInfo:OnTalenSkillPreview()
    local param_t = {show_type = self.ShowType,pos = UH.GetRootPosition(self.gameObject)}
    param_t.pos = self.ShowType == 2 and param_t.pos + Vector2.New(436,0) or param_t.pos
    param_t.skill_list = self.show_talen_list

    local info_rect = self.ParentMono.gameObject:GetComponent(typeof(UnityEngine.RectTransform))
    param_t.h = info_rect.sizeDelta.y

	if ItemInfoCtrl.Instance:Data().derive_view then 
		ItemInfoCtrl.Instance:ItemDeriveView()
		if not ViewMgr:IsOpen(ShengQiItemPreSkill) then 
			ItemInfoCtrl.Instance:ItemDeriveView(ShengQiItemPreSkill,param_t)
	    end
	else 
		ItemInfoCtrl.Instance:ItemDeriveView(ShengQiItemPreSkill,param_t)
	end 
end

function ShengQiItemInfo:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end


-- 符石物品信息
FuShiItemInfo = FuShiItemInfo or DeclareMono("FuShiItemInfo", ItemInfoViewItem)

function FuShiItemInfo:OnEnable()
    ItemInfoViewItem.OnEnable(self)
    UH.SetText(self.Title, Language.ShengQi.Text5)
    self.item = self.info.item_list[self.ShowType]
    self:FlushPanel()
end

function FuShiItemInfo:FlushPanel()
    local attri_list_data = self.item:AttriShowList()
    self.AttriList:SetData(attri_list_data)
    local role_attri = self.item:RoleAttri()
    local role_attri_t = CommonStruct.AttrStruct(role_attri.attri_type, role_attri.attri_value)
    role_attri_t.name = role_attri_t.attr_name
    role_attri_t.value_text = "" .. role_attri_t.attr_percent_value
    self.RoleAttri:SetData(role_attri_t)
end


-- 主动技能名字
ShengQiItemInfoSKillNameItem = ShengQiItemInfoSKillNameItem or DeclareMono("ShengQiItemInfoSKillNameItem", UIWidgetBaseItem)
function ShengQiItemInfoSKillNameItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, data:QuaName())
end

-- 符石信息
ShengQiItemInfoFushiItem = ShengQiItemInfoFushiItem or DeclareMono("ShengQiItemInfoFushiItem", UIWidgetBaseItem)
function ShengQiItemInfoFushiItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetIcon(self.Icon, data:IconId())
    UH.SetText(self.Level, string.format("Lv.%s", data:Level()))
end


ShengQiRoleAttriItem = ShengQiRoleAttriItem or DeclareMono("ShengQiRoleAttriItem", UIWidgetBaseItem)
function ShengQiRoleAttriItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local attri_info = CommonStruct.AttrStruct(data.attri_type, data.attri_value)
    UH.SetText(self.Name,attri_info.attr_name)
    local max_color = COLORSTR.Green3
    if self.ShowType == 2 then
        max_color = COLORSTR.Green9
    end
    if data.preview_value then
        local preview_attri_info = CommonStruct.AttrStruct(data.attri_type, data.preview_value)
        local max_preview_str = ColorStr(string.format(Language.ShengQi.Text17,preview_attri_info.attr_percent_value),max_color)
        UH.SetText(self.Value,attri_info.attr_percent_value .. max_preview_str)
    else
        UH.SetText(self.Value,attri_info.attr_percent_value)
    end

    if self.Dan and self.Shuang and data.index then
        self.Dan:SetActive(data.index % 2 == 1)
        self.Shuang:SetActive(data.index % 2 == 0)
    end
end





ChatQuickViewShengQiItem = DeclareMono("ChatQuickViewShengQiItem", UIWidgetBaseItem)

function ChatQuickViewShengQiItem:ChatQuickViewShengQiItem()
	self.chat_data = ChatCtrl.Instance:Data()
end

function ChatQuickViewShengQiItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.Name.text = data:Name()
    self.Level1.text = "Lv." .. data:Level()
    self.Cell:SetData(data)	
    UH.SetText(self.Level2, data:Level())

    local state = data:State()
    UH.SpriteName(self.ZDX,ShengQiCfg.StateImg[state])
	self.ZDX:SetObjActive(state ~= ShengQiCfg.State.Rest)
end

function ChatQuickViewShengQiItem:OnClickPet()
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
	if ViewMgr:IsOpen(ChatView) then
		ChatCtrl.Instance:SendChannelChatText(ChatItemShengQi.ItemIntro(self.data), {ChatItemShengQi.New(self.data)})
		ChatData.Instance:SendGrey()
	end
	ViewMgr:CloseView(ChatQuickView)
end



----------------- chat_pet_item -----------------
ChatShengQiItem = DeclareMono("ChatShengQiItem", RichTextItem)

function ChatShengQiItem:SetData(data, info)
    self.info = info
    local sheng_qi = ShengQi.New({item_id = data.item_id, param = data})
	self.Cell:SetData(sheng_qi)
    UH.SetText(self.Name, sheng_qi:Name())--data.name
    UH.SetText(self.TypeDesc, sheng_qi:TypeName())
	self.Cap.text = data.score
	UH.SetText(self.Level, data.level)
end

function ChatShengQiItem:OnClickItem()
    if nil == self.info then return end
    if self.info.sign_id then
        ChatCtrl.Instance:SendChatItemInfoReq(self.info.sign_id)
    end
end




ShengQiItemPreSkill = ShengQiItemPreSkill or DeclareView("ShengQiItemPreSkill","sheng_qi/sheng_qi_item_pre_skill")

VIEW_DECLARE_MASK(ShengQiItemPreSkill, ViewMask.None)

function ShengQiItemPreSkill:ShengQiItemPreSkill()
    self.data = ItemInfoCtrl.Instance:Data()
    self.info = self.data:ItemInfo()
end

function ShengQiItemPreSkill:LoadCallback(data)
    local y = data.h / 2 + data.pos.y
    local x = data.pos.x + 381
    
    y = Valve(y, UnityEngine.Screen.height / 2 - 15)
    self.InfoPosition.localPosition = Vector2.New(x,y)
    local skill_list = data.skill_list
    if not TableIsEmpty(skill_list) then
        self.List:SetData(skill_list)
    end
end 

function ShengQiItemPreSkill:OnClickPressDown()
	ViewMgr:OnTouchDownTrigger(ViewManager.TouchDownType.ItemInfo)
end


ShengQiItemPreSkillItem = ShengQiItemPreSkillItem or DeclareMono("ShengQiItemPreSkillItem", UIWidgetBaseItem)
function ShengQiItemPreSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    self.SkillItem:SetData(data)
    UH.SetText(self.Desc, data:Desc())
end