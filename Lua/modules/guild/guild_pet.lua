GuildPet = GuildPet or DeclareMono("GuildPet", UIWFlushPanel)
function GuildPet:GuildPet()
    self.data = GuildData.Instance
    self.data_cares = {
        {data = self.data.guild_train_list, func = self.FlushTrainView, init_call = false},
        {data = self.data.guild_pet_list, func = self.FlushPetView, init_call = false}
    }
end
function GuildPet:onFlush()
    self.PetList:SetData(self.data:GetGuildPetList())
    self.TrainList:SetData(self.data:GetGuildTrainList())
    self.data.guild_pet_point.count = 0
end
function GuildPet:FlushPetView()
    self.PetList:SetData(self.data:GetGuildPetList())
end
function GuildPet:FlushTrainView()
    self.TrainList:SetData(self.data:GetGuildTrainList())
end
function GuildPet:OnClickAdd(data)
    if next(PetData.Instance:GetPetList()) == nil then
		PublicPopupCtrl.Instance:Center(ErrorText[40])
		return
	end
    local can_train_list = {}
    for k, v in pairs(PetData.Instance:GetPetList()) do
        if v:Level() < ROLE_MAX_LEVEL then
            table.insert(can_train_list, v)
        end
    end
    if next(can_train_list) == nil then
        PublicPopupCtrl.Instance:Center(Language.Guild.TrainNoPet)
        return
    end
    ViewMgr:OpenView(GuildPetChoose)
    self.data.places_index = data.index - 1
end
GuildPetItem = GuildPetItem or DeclareMono("GuildPetItem", UIWidgetBaseItem)
function GuildPetItem:GuildPetItem()
    self.lang = Language.Guild
    self.guild_data = GuildData.Instance
    self.pet_data = PetData.Instance

    if self.ui_obj == nil then
        self.ui_obj = UIObjDrawer.New()
        self.ModelShow:SetShowData({
            ui_obj = self.ui_obj,
            view = self
        })
    end
end
function GuildPetItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Pos, string.format(self.lang.TrainPos, data.index))
    local pet_index = data.pet_index
    local lock_info = self.guild_data:GetLockInfo(data.index)
    local guild_info = self.guild_data:GetGuildInfo()
    local skill_info = self.guild_data:TrainSkillInfo(1) -- 第一个技能就是解锁用的
    if skill_info.level == 0 then
        -- Debuger.LogError("【家族训宠】老号数据问题")
        skill_info.level = 1
    end
    local info = self.guild_data:GetTrainUpInfo(1, skill_info.level)
    local is_lock = data.index > info.addition--skill_info.level < lock_info.level or guild_info.level < lock_info.family_level
    self.LockObj:SetActive(is_lock)

    self.AddObj:SetActive(not is_lock and pet_index == -1)
    self.IngObj:SetActive(not is_lock and pet_index ~= -1)
    UH.SetText(self.GuildLevel, string.format(Language.Common.Xstr, guild_info.level < lock_info.family_level and COLORSTR.Red7 or COLORSTR.Green3, string.format(self.lang.Lock1, lock_info.family_level)))
    UH.SetText(self.SkillLevel, string.format(Language.Common.Xstr, skill_info.level < lock_info.level and COLORSTR.Red7 or COLORSTR.Green3, string.format(self.lang.Lock2, lock_info.level)))
    -- 训练时间
    local train_time = self.guild_data:GetTrainTime()
    local t_time = TimeManager.FormatDHMS(train_time) -- 秒
    UH.SetText(self.TrainTime, string.format(self.lang.TrainTime, t_time.hour, t_time.min))
    self:SetRedPoint(0)
    -- 倒计时
    local finish_time = data.begin_time + train_time
    local function update_func()
        local surplus_time = finish_time - TimeHelper.GetServerTime()
        local ft = TimeHelper.FormatDHMS(surplus_time)
        UH.SetText(self.IngTime, string.format(self.lang.TrainIng, ft.hour, ft.min))
    end
    local function finish_func()
        -- body
        UH.SetText(self.BtnText, self.lang.TrainBtn[2])
        self:SetRedPoint(1)
    end
    if not self.time_handle and data.begin_time > 0 then
        self.time_handle = TimeHelper:AddCountDownCT(update_func, finish_func, finish_time, 1, true)
    end
    if pet_index >= 0 then
        local pet = PetData.Instance:GetPetInfo(pet_index)
        if pet then
            local level = pet:Level()
            UH.SetText(self.PetName, pet:Name() .. string.format("   Lv.%d", level))
            local exp_info = self.guild_data:GetTrainExpInfo(level)
            UH.SetText(self.Exp, string.format(self.lang.TrainExp, exp_info.add_exp))
            data.gongxian_cost = exp_info.gongxian_cost
            if finish_time - TimeHelper.GetServerTime() < 0 then
                UH.SetText(self.BtnText, self.lang.TrainBtn[2])
                self:SetRedPoint(1)
            else
                UH.SetText(self.BtnText, self.lang.TrainBtn[1])
                self:SetRedPoint(0)
            end
            local need_exp = self.pet_data:GetLevelExp(pet:Level())
            self.ExpProg:SetProgress(pet:Exp() / need_exp)
            self.ExpProg:SetText(string.format("%d/%d",pet:Exp(),need_exp))
            self.ui_obj:SetData(DrawerHelper.GetNpcPath(pet:ResId()))
            self.SignNameItem:SetType(pet:LimitType())
        end
    end
end
function GuildPetItem:SetRedPoint(num)
    if self.red_point == nil then
        self.red_point = UiRedPoint.New(self.gameObject, Vector3.New(60, -230))
    end
    self.red_point:SetNum(num)
end
-- 完成由服务端判断
function GuildPetItem:OnClickFinish(data)
    GuildCtrl.SendGuildPetTraining({pet_index = self.data.pet_index, places_index = data.index - 1})
	AudioMgr:PlayEffect(AudioType.UI, CommonAudio.ClickGeneral1)
end
function GuildPetItem:OnRelease()
    UIWidgetBaseItem.OnRelease(self)
    TimeHelper:CancelTimer(self.time_handle)
    self.time_handle = nil
end
function GuildPetItem:OnDestroy()
    TimeHelper:CancelTimer(self.time_handle)
    if self.ui_obj ~= nil then
        self.ui_obj:DeleteMe()
	end
    self.time_handle = nil
end

GuildTrainItem = GuildTrainItem or DeclareMono("GuildTrainItem", UIWidgetBaseItem)
function GuildTrainItem:GuildTrainItem()
    self.lang = Language.Guild
    self.guild_data = GuildData.Instance
end
function GuildTrainItem:SetData(data)
    UIWidgetBaseItem.SetData(self, data)
    UH.SetText(self.Name, self.lang.TrainSkill[data.index])
    local max_level = self.guild_data:GetTrainMaxLevel(data.index)
    UH.SetText(self.Level, string.format(self.lang.BaseInfo[7], data.level, max_level))
    if data.level == 0 then
        -- Debuger.LogError("【家族训宠】老号数据问题")
        data.level = 1
    end
    local info = self.guild_data:GetTrainUpInfo(data.index, data.level)
    UH.SpriteName(self.SpIcon, string.format("icon%d", info.type))
    UH.SetText(self.CurDesc, string.format(self.lang.BaseInfo[8], info.desc))
    self.next_info = self.guild_data:GetTrainUpInfo(data.index, data.level + 1)
    if self.next_info == nil then
        UH.SetText(self.NextDesc,Language.Common.MaxLevel)
    else
        UH.SetText(self.GongXian, self.next_info.gongxian_cost)
        UH.SetText(self.Cost, self.next_info.coin_cost)
        UH.SpriteName(self.HuoBi, HuoBi[self.next_info.coin_type])
        UH.SetText(self.NextDesc, string.format( self.lang.BaseInfo[9],self.next_info.desc))
    end
end

function GuildTrainItem:OnClickLearn()
    local info = self.guild_data:GetGuildInfo()
    if self.next_info then
        if info.level < self.next_info.family_level then
            PublicPopupCtrl.Instance:Center(string.format(ErrorText[74], self.next_info.family_level))
            return
        end

        if not MallCtrl.IsNotCurrency(self.next_info.coin_type,self.next_info.coin_cost) then
            return
        end

        if RoleData.Instance:GetCurrency(CurrencyType.GuildCotribution) < self.next_info.gongxian_cost then 
            PublicPopupCtrl.Instance:Center(ErrorText[92])
            return
        end
    else 
        local max_level = self.guild_data:GetTrainMaxLevel(self.data.index)
        if self.data.level >= max_level then 
            PublicPopupCtrl.Instance:Center(ErrorText[93])
            return 
        end 
        return 
    end
    AudioMgr:PlayEffect(AudioType.UI, CommonAudio.JinJie2)
    self.EffectTool:Play(10010281)
    GuildCtrl.SendGuildPetTrainingUp(self.data.index)
end
