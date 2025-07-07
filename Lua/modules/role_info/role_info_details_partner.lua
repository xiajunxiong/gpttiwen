RoleInfoDetailsPartner = RoleInfoDetailsPartner or DeclareMono("RoleInfoDetailsPartner",UIWFlushPanel)

function RoleInfoDetailsPartner:RoleInfoDetailsPartner()
    self.data = RoleInfoData.Instance
    self.qua_img_names = {
        [3] = {"lansedi","_Loclanse"},
        [4] = {"zisedi","_Loczise"},
        [5] = {"chengsedi","_Locchengse"},
        [6] = {"hongsedi","_Lochongse"},
    }
end

function RoleInfoDetailsPartner:Awake()
	UIWFlushPanel.Awake(self)
    self.partner_info = self.data:GetPartnerData()
    self.min_index = 1                                                      
    self.max_index = #self.partner_info > 4 and 4 or #self.partner_info     
    self.cur_index = 1
    self:FlushPanel()
end

function RoleInfoDetailsPartner:FlushPanel()
	self.LastObj:SetActive(self.cur_index > self.min_index)
	self.NextObj:SetActive(self.cur_index < self.max_index)
	self.info = self.partner_info[self.cur_index]
    local prof_sp = RoleData.GetProfSp(self.info.job)
    UH.SpriteName(self.Prof,prof_sp)
    UH.SpriteName(self.NameProf,prof_sp)
    UH.SetText(self.Name,self.info.name)
    local lv_str = ColorStr(string.format("Lv.%s",self.info.lv), COLORSTR.Green2)
    UH.SetText(self.NameInfo,string.format("%s  %s",self.info.name, lv_str))
    UH.SetText(self.Lv,tostring(self.info.lv))
   -- UH.SetText(self.MaxLv,string.format("/%s",RoleData.Instance:GetRoleLevel()))
    UH.SetText(self.Score,self.info.cap)
    UH.SpriteName(self.ScoreLv,self.info.cap_lv)
    local icon_data = {
        info = {
            icon_id = self.info.icon_id
        },
        vo = {
            skin_id = self.info.skin_id
        }
    }
    UH.SetIcon(self.HeadIcon, PartnerData.IconId(icon_data), ICON_TYPE.ITEM)
    PartnerInfo.SetQuality(self.HeadQua, self.info.quality)
    self.StrLevel.SpriteName = PartnerInfo.StrLevelSp(self.info.intensify_level)
    AvatarEffect.SetQuaAnimEffect(self.HeadQua.gameObject,self.info.soar_count)
    local quality_upgrade = PartnerConfig.ChangeServerQua[self.info.quality]
    UH.SetText(self.GradText,PartnerData.GetPartnerQualityName(quality_upgrade,self.info.quality_segment))

    self.RankItem:SetRank(self.info.rank)
    self.RankItem:SetActive(PartnerData.PartnerRankShow(self.info.rank))

    self:FlushModelShow()
    self:FlushSkillDesc()
    self:FlushQua()
end

function RoleInfoDetailsPartner:FlushModelShow()
    local skin_id = self.info.skin_id
    local res_path = ""
    if skin_id > 0 and PartnerData.Instance.partner_skin_ids[skin_id] then
        local res_id = PartnerData.Instance.partner_skin_ids[skin_id].res_id
        res_path = string.format("actor/npc/%s.prefab", res_id)
    else
        res_path = string.format("actor/npc/%s.prefab", self.info.res_id)
    end
    if not self.ui_obj then
        self.ui_obj = UIObjDrawer.New()
        self.ui_obj:SetData(res_path,self.info.soar_count)
        self.ModelShow:SetShowData(
            {
                ui_obj = self.ui_obj,
                view = self,
            }
        )
        self.ui_obj:PlayAnim(ChAnimType.Show)
    else
        self.ui_obj:SetData(res_path,self.info.soar_count)
        self.ui_obj:PlayAnim(ChAnimType.Show)
    end
end

function RoleInfoDetailsPartner:FlushSkillDesc()
    --self.SkillDower:SetData(self.info.gift_cfg)
    self.SKillList:SetData(self.info.skill_desc_list)
end

function RoleInfoDetailsPartner:FlushQua()
    self.QuaShow:SetData({
        cfg = {
            quality_upgrade = PartnerConfig.ChangeServerQua[self.info.quality],
            quality_segment = self.info.quality_segment,
        }
    })
    -- local qua_name_t = self.qua_img_names[self.info.quality]
    -- if qua_name_t then
    --     local operate_qua = self.info.quality <= PartnerConfig.QuaFetter.Red and self.info.quality - PartnerConfig.QuaFetter.Grassy or 4
    --     UH.SpriteName(self.Qua,"_Loc_"..PartnerConfig.QuaQuaPicName[operate_qua])
    --     UH.SetText(self.Seg, string.format(Language.Partner.QualitySeg,DataHelper.GetDaXie(self.info.quality_segment)))
    -- end
end

function RoleInfoDetailsPartner:OnNextClick()
	self.cur_index = self.cur_index + 1
	self:FlushPanel()
end

function RoleInfoDetailsPartner:OnLastClick()
	self.cur_index = self.cur_index - 1
	self:FlushPanel()
end

function RoleInfoDetailsPartner:OnDestroy()
    UIWFlushPanel.OnDestroy(self)
    if self.ui_obj then
        self.ui_obj:DeleteMe()
        self.ui_obj = nil
    end
end


RoleInfoDetailsPartnerDowerItem = RoleInfoDetailsPartnerDowerItem or DeclareMono("RoleInfoDetailsPartnerDowerItem",UIWidgetBaseItem)
function RoleInfoDetailsPartnerDowerItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.Name,data.name)
    UH.SetText(self.Desc,data.desc)
    UH.SetIcon(self.Icon, data.icon_id, ICON_TYPE.SKILL)
end

RoleInfoDetailsPartnerSkillItem = RoleInfoDetailsPartnerSkillItem or DeclareMono("RoleInfoDetailsPartnerSkillItem",UIWidgetBaseItem)
function RoleInfoDetailsPartnerSkillItem:SetData(data)
    UIWidgetBaseItem.SetData(self,data)
    UH.SetText(self.Name,data.show_name)
    UH.SetText(self.Desc,data.description_2)
    UH.SetIcon(self.Icon, data.icon_id, ICON_TYPE.SKILL)
end