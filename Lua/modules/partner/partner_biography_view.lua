
PartnerBiographyView = PartnerBiographyView or DeclareView("PartnerBiographyView", PartnerConfig.ResPath .. "partner_biography", Mod.PartnerBiography.Main)
VIEW_DECLARE_LEVEL(PartnerBiographyView,ViewLevel.L1)
-- VIEW_DECLARE_MASK(PartnerBiographyView, ViewMask.Block)

function PartnerBiographyView:PartnerBiographyView()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner
end

function PartnerBiographyView:LoadCallback(param_t)
	self.LetterBlock:SetActive(false)
end

function PartnerBiographyView:CloseCallback()
end

function PartnerBiographyView:OnClickClose()
	ViewMgr:CloseView(PartnerBiographyView)
	ViewMgr.mask:BgVisible(true)
end


PartnerBiographyViewF = PartnerBiographyViewF or DeclareMono("PartnerBiographyViewF", UIWFlushPanel)

function PartnerBiographyViewF:PartnerBiographyViewF()
	self.data = PartnerCtrl.Instance:Data()
	self.language = Language.Partner.Biography

	self.data_cares = {
		{data = self.data.cur_biography_info, func = self.FlushBiography, keys = {"biography"}, init_call = false},
		{data = self.data.cur_biography_info, func = self.FlushPage, keys = {"page"}, init_call = false},
		{data = self.data.cur_biography_info, func = self.FlushGet, keys = {"flush_get"}, init_call = false},
	}

	self.anim_handle_book = self.BookAnim:ListenEvent("end", function ()
		self.BookAnim:SetInteger(APH("state"), 0)
		self.data:PartnerBiographyInfo(self.to_biography, self.to_page)
		self.BtnLetterObj:SetActive(self.to_page > 0)
		self.BtnReturnObj:SetActive(self.to_page > 0)
		if 1 == self.to_page then
			local key = PrefKeys.PartnerBiographyGuide()
			if 0 == UnityPlayerPrefs.GetInt(key) then
				ViewMgr.mask:BgVisible(false)
				self.GuideObj:SetActive(true)
				UnityPlayerPrefs.SetInt(key, 1)
			end
		end
	end)

	self.anim_handle_letter = self.LetterAnim:ListenEvent("end", function (eve_param)
		if "move" == eve_param then
			local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
			if nil == data then return end
			local biography = self.data.cur_biography_info.biography
			biography = data.biographys[biography]

			ViewMgr.mask:BgVisible(false)
			self.LetterBlock:SetActive(true)

			if biography.flag_open then
				self:LetterOpenStart()
			else
				self.LetterShow:SetActive(true)
			end
		end
		if "open" == eve_param then
			self.LetterContent:SetActive(true)
		end
	end)

	self:FlushChapter()
end

function PartnerBiographyViewF:FlushChapter()
	local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
	if nil == data then return end

	local chapters = {}
	for _, biography in ipairs(data.biographys) do
		table.insert(chapters, biography.desc_f)
	end
	self.PageText:SetData({
		chapters = chapters
	})
end

function PartnerBiographyViewF:Awake()
	UIWFlushPanel.Awake(self)

	self:FlushShow()
end

function PartnerBiographyViewF:OnDestroy()
	UIWFlushPanel.OnDestroy(self)
	-- TimeHelper:CancelTimer(self.timer_dt)

	self.BookAnim:UnlistenEvent("end", self.anim_handle_book)
	self.LetterAnim:UnlistenEvent("end", self.anim_handle_letter)
end

function PartnerBiographyViewF:FlushShow()
	local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
	if nil == data then return end
	local info = data.info

	self.Name.text = info.name
	self.TitleSp.SpriteName = self.language.name2sp[info.title]
	self.Title.text = string.format(self.language.info.title, info.title)
	self.Height.text = string.format(self.language.info.height, info.height)
	self.Weight.text = string.format(self.language.info.weight, info.weight)
	self.Age.text = string.format(self.language.info.age, info.age)
	self.Birthday.text = string.format(self.language.info.birthday, info.birthday)
	self.Character.text = string.format(self.language.info.character, info.character)
	self.Hometown.text = string.format(self.language.info.hometown, info.hometown)

	self.BiographyList:SetData(data.biographys)
	
	self:FlushModelShow(data)
end

function PartnerBiographyViewF:FlushModelShow(data)
	local skin_id = data.vo.skin_id
    local res_path = ""
    if skin_id > 0 and self.data.partner_skin_ids[skin_id] then
        local res_id = self.data.partner_skin_ids[skin_id].res_id
        res_path = string.format("actor/npc/%s.prefab", res_id)
    else
        local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
        res_path = string.format("actor/npc/%s.prefab", data.info.res_id)
    end
	self.ui_obj = UIObjDrawer.New()
	self.ui_obj:SetData(res_path)
	self.ModelShow:SetShowData(
		{
			ui_obj = self.ui_obj,
			view = self,
			load_callback = function ()
				self.ui_obj:PlayAnim(ChAnimType.Show)
			end
		}
	)
end

function PartnerBiographyViewF:FlushBiography()
	local page = self.data.cur_biography_info.page
	local biography = self.data.cur_biography_info.biography

	local page0 = 0 == page
	self.Page0Obj:SetActive(page0)
	self.Page1Obj:SetActive(not page0)
	
	if page0 then return end

	local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
	if nil == data then return end

	biography = data.biographys[biography]
	self.BiographyTitle.text = biography.name

	-- self.LeftShow.text = biography.desc_f
	-- self.RightShow.text = biography.desc_f

	self.LetterTitle.text = biography.addressee
	self.LetterDesc.text = biography.letter_desc_f
	self.LetterSender.text = biography.sender

	for _, reward in pairs(biography.rewards) do
		reward.is_grey = not biography.flag_get
	end

	self.RewardList:SetData(biography.rewards)
	self.BtnGetObj:SetActive(biography.flag_get)
	self.IsGetObj:SetActive(not biography.flag_get)
	-- self.BtnGet.text = self.language.btn_get[biography.flag_get and 1 or 0]

	self:FlushRedPointLetter(biography)

	self:FlushPage()
end

function PartnerBiographyViewF:FlushPage()
	local page = self.data.cur_biography_info.page
	local biography = self.data.cur_biography_info.biography
	local page0 = 0 == page
	self.Page0Obj:SetActive(page0)
	self.Page1Obj:SetActive(not page0)

	self.BiographyTitleObj:SetActive(1 == page)
	-- local pos_l = self.LeftShowPos.localPosition
	-- local pos_r = self.RightShowPos.localPosition
	-- self.LeftShowPos.localPosition = Vector2.New(pos_l.x, (page - 1) * PartnerConfig.BIOGRAPHY_PAGE_HEIGHT * 2)
	-- self.RightShowPos.localPosition = Vector2.New(pos_r.x, page * PartnerConfig.BIOGRAPHY_PAGE_HEIGHT * 2 - PartnerConfig.BIOGRAPHY_PAGE_HEIGHT)
	if biography > 0 then
		self.PageText:ToChapter(biography, page * 2 - 1)
	end
end

function PartnerBiographyViewF:FlushGet()
	local data = self.data:GetPartnerInfoById(self.data.cur_sel_partner)
	if nil == data then return end
	local biography = self.data.cur_biography_info.biography
	biography = data.biographys[biography]
	if nil == biography then return end
	for _, reward in pairs(biography.rewards) do
		reward.is_grey = not biography.flag_get
	end
	self.RewardList:SetData(biography.rewards)
	self.BtnGetObj:SetActive(biography.flag_get)
	self.IsGetObj:SetActive(not biography.flag_get)
	self.BiographyList:SetData(data.biographys)
	self:FlushRedPointLetter(biography)
end

function PartnerBiographyViewF:FlushRedPointLetter(biography)
	self.red_point_letter = self.red_point_letter or UiRedPoint.New(self.LatterRedPointObj, Vector3.New(40,70,0))
    self.red_point_letter:SetNum((biography.flag_unlock and biography.flag_get and RoleData.Instance:GetRoleLevel() > 30) and 1 or 0)
end

function PartnerBiographyViewF:BookAnimStart(value, biography, page)
	self.Page0Obj:SetActive(false)
	self.Page1Obj:SetActive(false)
	self.BookAnim:SetInteger(APH("state"), value)
	self.to_biography = biography
	self.to_page = page
end

function PartnerBiographyViewF:LetterMoveStart()
	self.LetterAnim:SetInteger(APH("state"), 1)
end

function PartnerBiographyViewF:LetterOpenStart()
	self.LetterAnim:SetInteger(APH("state"), 2)
end

function PartnerBiographyViewF:OnClickPage(value)
	if not self.BtnLetterObj.activeSelf then return end
	local page = self.data.cur_biography_info.page + value
	local max_page = page > self.PageText:PageNumShow(self.data.cur_biography_info.biography) / 2
	if page < 1 or max_page then
		if max_page then
			PublicPopupCtrl.Instance:Center(self.language.last_page_tips)
		end
		return
	end
	self:BookAnimStart(value, nil, page)
end

function PartnerBiographyViewF:OnPressDownPage(value)
	self.page_start_x = UnityEngine.Input.mousePosition.x
end

function PartnerBiographyViewF:OnPressUpPage(value)
	local distance_x = UnityEngine.Input.mousePosition.x - self.page_start_x
	if math.abs(distance_x) > PartnerConfig.BIOGRAPHY_PAGE_DISTANCE then
		self:OnClickPage(distance_x > 0 and -1 or 1)
	else
		self:OnClickPage(value)
	end
end

function PartnerBiographyViewF:OnClickReturn()
	if not self.BtnLetterObj.activeSelf then return end
	self:BookAnimStart(-1, 0, 0)
	self.BtnLetterObj:SetActive(false)
	self.BtnReturnObj:SetActive(false)
end

function PartnerBiographyViewF:OnClickBiographyItem(data)
	if not data.flag_unlock then
		PublicPopupCtrl.Instance:Center(string.format(self.language.unlock_tips, data.upgrade))
		return
	end
	self:BookAnimStart(1, data.biography, 1)
end

function PartnerBiographyViewF:OnClickLatter()
	self.BtnLetterObj:SetActive(false)
	self.LetterObj:SetActive(true)
	self:LetterMoveStart()
end

function PartnerBiographyViewF:OnClickCloseLatter()
	self.BtnLetterObj:SetActive(true)
	self.LetterShow:SetActive(false)
	self.LetterContent:SetActive(false)
	self.LetterObj:SetActive(false)

	ViewMgr.mask:BgVisible(true)
	self.LetterBlock:SetActive(false)
end

function PartnerBiographyViewF:OnClickGet()
	PartnerCtrl.Instance:SendPartnerFetchBiography(self.data.cur_sel_partner, self.data.cur_biography_info.biography)
end

function PartnerBiographyViewF:OnClickBlockLetter()
	if self.LetterShow.activeSelf then
		self:OnClickCloseLatter()
	end
end

function PartnerBiographyViewF:OnClickOpenLetter()
	self.LetterShow:SetActive(false)
	self:LetterOpenStart()
	PartnerCtrl.Instance:SendPartnerOpenBiography(self.data.cur_sel_partner, self.data.cur_biography_info.biography)
end

function PartnerBiographyViewF:OnClickGuideOK()
	self.GuideObj:SetActive(false)
	ViewMgr.mask:BgVisible(true)
end

PartnerBiographyItem = DeclareMono("PartnerBiographyItem", UIWidgetBaseItem)

function PartnerBiographyItem:PartnerBiographyItem()
    self.partner_data = PartnerCtrl.Instance:Data()
end

function PartnerBiographyItem:SetData(data)
	UIWidgetBaseItem.SetData(self, data)
	self.ItemInter.Interactable = data.flag_unlock
	self.Name.text = data.name
	
	self.red_point_item = self.red_point_item or UiRedPoint.New(self.ItemInter.gameObject, Vector3.New(200,20,0))
	self.red_point_item:SetNum((data.flag_unlock and data.flag_get and RoleData.Instance:GetRoleLevel() > 30) and 1 or 0)
end