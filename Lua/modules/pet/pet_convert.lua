PetConvert = PetConvert or DeclareView("PetConvert", "pet/pet_convert", Mod.Pet.PetConvert)
function PetConvert:PetConvert()
    self.data = PetData.Instance
    self.index = 1
    --[[ 
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = true,
                shadow_dir = ModelUIShow.ShadowCameraDir.Pet
            }
        )
    end ]]
end

function PetConvert:Awake()
    if self.handle_care == nil then
        self.handle_care = BagData.Instance:Care(BindTool.Bind(self.OnFlush, self))
    end
end

function PetConvert:OnDestroy()
    if self.handle_care then
        BagData.Instance:UnCare(self.handle_care)
        self.handle_care = nil
    end
end

function PetConvert:LoadCallback()
    GLOBAL_PET_CONVERT_OPEN_FLAG = GLOBAL_PET_CONVERT_OPEN_FLAG or 0
    if GLOBAL_PET_CONVERT_OPEN_FLAG > 0 then
        BuglyReport("GLOBAL_PET_CONVERT_OPEN_FLAG in loadCallback > 0")
    end
    GLOBAL_PET_CONVERT_OPEN_FLAG = GLOBAL_PET_CONVERT_OPEN_FLAG + 1
end

function PetConvert:CloseCallback()
    PetData.Instance.pet_convert_select_data = nil
    if GLOBAL_PET_CONVERT_OPEN_FLAG == nil or GLOBAL_PET_CONVERT_OPEN_FLAG <= 0 then
        BuglyReport("GLOBAL_PET_CONVERT_OPEN_FLAG in CloseCallback is "..tostring(GLOBAL_PET_CONVERT_OPEN_FLAG))
    else
        GLOBAL_PET_CONVERT_OPEN_FLAG = GLOBAL_PET_CONVERT_OPEN_FLAG - 1
    end
end

function PetConvert:OnFlush()
    --[[ local list = self.data:ConvertList()
    self.total_num = table.nums(list)
    if self.total_num == 1 then
        self.Arrow1:SetActive(false)
        self.Arrow2:SetActive(false)
    end
    self.Arrow1:SetActive(self.index ~= 1)
    self.Arrow2:SetActive(self.index ~= self.total_num)
    local local_pos = self.data:GetLocalConvertIndex()
    self.index = local_pos == nil and 1 or local_pos
    self:FlushPetInfo() ]]
    local data = self.data:ConvertList()
    -- self.total_num = table.nums(data)
    self.List:SetData(data)
    local commonInfo = data[1]
     --PetData.PetComposeInfoSeq(45)
    local commonCell = Item.New({item_id = commonInfo.consume_item_id})
    self.CommonItem:SetData(commonCell)

    local cost_num = commonInfo.consume_item_count
    local color_ = cost_num <= Item.GetNum(commonInfo.consume_item_id) and COLORSTR.Green3 or COLORSTR.Red7
    local str = string.format(Language.Common.StuffNumColorFirst, color_, Item.GetNum(commonInfo.consume_item_id), cost_num)
    UH.SetText(self.CommonRighttNum, str)
    UH.SetText(self.StuffNoticeText, string.format(Language.Pet.PetPiecesShenshouTip, tostring(cost_num)))
    self.composeInfo = commonInfo
    --[[ if self.total_num == 1 then
        self.Arrow1:SetActive(false)
        self.Arrow2:SetActive(false)
    end ]]
end
function PetConvert:OnClickCompose()
    local data = PetData.Instance:ConvertSelect()
    if data then
        -- ViewMgr:CloseView(PetPiecesShenshou)
        self.composeInfo = data
        local has_num = Item.GetNum(self.composeInfo.consume_item_id)
        if has_num < self.composeInfo.consume_item_count then
            PublicPopupCtrl.Instance:Center(Language.Pet.PiecesShenshouNotEnough)
        else
            --弹出确认窗口
            -- PetCtrl.SendPetOp(PET_OP_TYPE.Compose, self.composeInfo.seq)
            local info = {
                content = Language.Pet.ShenshouHeCheng,
                confirm = {
                    func = function()
                        PetCtrl.SendPetOp(PET_OP_TYPE.Convert, data.seq)
                        PublicPopupCtrl.Instance:CloseDialogTips()
                    end
                }
            }
            PublicPopupCtrl.Instance:DialogTips(info)
        end
    else
        PublicPopupCtrl.Instance:Center(Language.Pet.ShenshouXuanZhe)
    end
end
function PetConvert:OnClickChange(num)
    self.index = self.index + num
    if self.index > self.total_num then
        self.index = self.total_num
    end
    if self.index < 0 then
        self.index = 1
    end
    self.Arrow1:SetActive(self.index ~= 1)
    self.Arrow2:SetActive(self.index ~= self.total_num)
    self:FlushPetInfo()
end
function PetConvert:FlushPetInfo()
    local list = self.data:ConvertList()
    if self.old_index and self.old_index == self.index then
        return
    end

    local info = PetData.Instance:GetPetCfg(list[self.index].pet_id)
     --{pet_id = list[self.index].pet_id, level = 1}
    info.con_growth = info.con
    info.str_growth = info.str
    info.def_growth = info.def
    info.agi_growth = info.agi
    info.magic_growth = info.magic
    -- info.con = 0
    -- info.str = 0
    -- info.def = 0
    -- info.agi = 0
    -- info.magic = 0

    info.level = 1
    info.str_level = 1
    local pet = Pet.New(info)
    UH.SetText(self.NameTex, pet:Name())

    if pet:ResId() then
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()))
    end

    UH.SetText(self.RaceTex, Language.Pet.RaceClass[pet.config.race])
    UH.SpriteName(self.RaceIcon, string.format("Race%s", pet:RaceId()))
    self.EleAttr:SetData(pet:EleAttr())
    self.AttrList:SetPet(pet, true)

    local v1 = pet:ConfigGrowth()
    local v2 = pet:ConfigGrowth()
    UH.SetText(self.LblValue, string.format(Language.Pet.GrowthStr, COLORSTR.Gray1, COLORSTR.Red14, v1, v2))
    local t = pet:TotalAttr()
    local res = {}
    res.maxhp = t[BattleAttriType.MaxHp]
    res.maxmp = t[BattleAttriType.MaxMp]
    res.gongji = t[BattleAttriType.Attack]
    res.fangyu = t[BattleAttriType.Defense]
    res.minjie = t[BattleAttriType.Agility]
    res.jingshen = t[BattleAttriType.Mental]
    res.huifu = t[BattleAttriType.Recovery]
    for k, v in pairs(res) do
        UH.SetText(self["Lbl" .. k], v)
    end
    local gift_list = pet:GetSkillAndGiftGroupList()
    gift_list[1].main_skill = true
    self.SkillList:SetData(gift_list)
    self.old_index = self.index
end
function PetConvert:OnClickGift(cell)
    local data = cell:GetData()
    if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
        PetData.Instance:SetClickSkill(data)
        ViewMgr:OpenView(PetSkillTip)
    --[[ elseif data.show_lingwu then
        ViewMgr:OpenView(PetLingwuSkillTip) ]]
    end
end
function PetConvert:OnClickClose()
    ViewMgr:CloseView(PetConvert)
end

--[[ function PetConvert:OnDestroy()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
end ]]
PetConvertItem = PetConvertItem or DeclareMono("PetConvertItem", UIWidgetBaseItem)
function PetConvertItem:PetConvertItem()
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = true,
                shadow_dir = ModelUIShow.ShadowCameraDir.Pet
            }
        )
    end
end
function PetConvertItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    local pet = Pet.New(data)
    UH.SetText(self.NameTex, pet:Name())
    if pet:ResId() then
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()))
    end
end
function PetConvertItem:OnClickSelect()
    PetData.Instance:ConvertSelect(self.data)
end

function PetConvertItem:OnDestroy()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
end
function PetConvertItem:OnClickDetail()
    ViewMgr:OpenView(PetDetailNew, self.data.seq + 1)
end

PetDetailNew = PetDetailNew or DeclareView("PetDetailNew", "pet/pet_detail_new")
function PetDetailNew:PetDetailNew()
    self.data = PetData.Instance
    self.index = 1
    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
                show_shadow = true,
                shadow_dir = ModelUIShow.ShadowCameraDir.Pet
            }
        )
    end
end
function PetDetailNew:OnClickClose()
    ViewMgr:CloseView(PetDetailNew)
end
function PetDetailNew:LoadCallback(data)
    self.index = data
    local list = self.data:ConvertList()
    self.total_num = table.nums(list)
    if self.total_num == 1 then
        self.Arrow1:SetActive(false)
        self.Arrow2:SetActive(false)
    end
    self.Arrow1:SetActive(self.index ~= 1)
    self.Arrow2:SetActive(self.index ~= self.total_num)

    self:FlushPetInfo()
end
function PetDetailNew:FlushPetInfo()
    local list = self.data:ConvertList()
    if self.old_index and self.old_index == self.index then
        return
    end

    local info = PetData.Instance:GetPetCfg(list[self.index].pet_id)
     --{pet_id = list[self.index].pet_id, level = 1}
    info.con_growth = info.con
    info.str_growth = info.str
    info.def_growth = info.def
    info.agi_growth = info.agi
    info.magic_growth = info.magic
    -- info.con = 0
    -- info.str = 0
    -- info.def = 0
    -- info.agi = 0
    -- info.magic = 0

    info.level = 1
    info.str_level = 1
    local pet = Pet.New(info)
    UH.SetText(self.NameTex, pet:Name())

    if pet:ResId() then
        self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()))
    end

    UH.SetText(self.RaceTex, Language.Pet.RaceClass[pet.config.race])
    UH.SpriteName(self.RaceIcon, string.format("Race%s", pet:RaceId()))
    self.EleAttr:SetData(pet:EleAttr())
    self.AttrList:SetPet(pet, true)

    local v1 = pet:ConfigGrowth()
    local v2 = pet:ConfigGrowth()
    UH.SetText(self.LblValue, string.format(Language.Pet.GrowthStr, COLORSTR.Gray1, COLORSTR.Red14, v1, v2))
    local t = pet:TotalAttr()
    local res = {}
    res.maxhp = t[BattleAttriType.MaxHp]
    res.maxmp = t[BattleAttriType.MaxMp]
    res.gongji = t[BattleAttriType.Attack]
    res.fangyu = t[BattleAttriType.Defense]
    res.minjie = t[BattleAttriType.Agility]
    res.jingshen = t[BattleAttriType.Mental]
    res.huifu = t[BattleAttriType.Recovery]
    for k, v in pairs(res) do
        UH.SetText(self["Lbl" .. k], v)
    end
    local gift_list = pet:GetSkillAndGiftGroupList()
    gift_list[1].main_skill = true
    self.SkillList:SetData(gift_list)
    self.old_index = self.index
end
function PetDetailNew:OnClickGift(cell)
    local data = cell:GetData()
    if data.skill_id and data.skill_id ~= 0 or (data.gift_id and data.gift_id ~= 0) then
        PetData.Instance:SetClickSkill(data)
        ViewMgr:OpenView(PetSkillTip)
    --[[ elseif data.show_lingwu then
        ViewMgr:OpenView(PetLingwuSkillTip) ]]
    end
end

function PetDetailNew:OnClickChange(num)
    self.index = self.index + num
    if self.index > self.total_num then
        self.index = self.total_num
    end
    if self.index < 0 then
        self.index = 1
    end
    self.Arrow1:SetActive(self.index ~= 1)
    self.Arrow2:SetActive(self.index ~= self.total_num)
    self:FlushPetInfo()
end

function PetDetailNew:OnDestroy()
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
    end
    self.ui_obj = nil
end
