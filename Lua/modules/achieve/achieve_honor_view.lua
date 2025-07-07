
AchieveHonorView = AchieveHonorView or DeclareMono("AchieveHonorView",UIWFlushPanel)
--GradeText====Text
--LeftList====UIWidgetList
--RightList====UIWidgetList
--FirstKillMember====Text
--FirstKillRound====Text
--FirstKillRecord====Text
--No1Member====Text
--No1Round====Text
--No1Record====Text
--No2Member====Text
--No2Round====Text
--No2Record====Text
--No3Member====Text
--No3Round====Text
--No3Record====Text
--LeftName====Text
--RightName====Text


function AchieveHonorView:AchieveHonorView()
    self.LeftList:SetData(Config.achievement_auto.item_list)
    --self.LeftList:ClickItem(1)
    self.LeftName.text = Config.achievement_auto.item_list[1].type_honor
    self.RightName.text = Config.achievement_auto.pet_list[1].fb_name
    self.RightList:SetData(Config.achievement_auto.pet_list)
    self.data_cares = {
        {data = AchieveData.Instance.recv_data, func = self.FlushFirstData,keys = {"recv_fb_type"} },
        {data = AchieveData.Instance.recv_data, func = self.FlushRollData,keys = {"recv_fb_type"}  }
    }
end

function AchieveHonorView:HonorClick()
    --Debuger.LogError("click....")
    if self.LeftListOj.activeSelf then
        self.LeftListOj:SetActive(false)
        self.LeftUp:SetActive(false)
        self.LeftDown:SetActive(true)
    end
    if self.RightListOj.activeSelf then
        self.RightListOj:SetActive(false)
        self.RightUp:SetActive(false)
        self.RightDown:SetActive(true)
    end
end

function AchieveHonorView:FlushFirstData()
    --Debuger.LogError("flush......")
    local item = Item.Create({item_id = 22005})
    --UH.SetIcon(self.Cell1, item:IconId())
    self.Cell1:SetData(item)
    local first_data = AchieveData.Instance.recv_data.recv_first_data
    if first_data then
        self:SetFirstText(first_data.user_list,first_data.rounds,first_data.parss_time)
    end
end

function AchieveHonorView:FlushRollData()
    --Debuger.LogError("flush......")
    local item = Item.Create({item_id = 22005})
    self.Cell2:SetData(item)
    self.Cell3:SetData(item)
    self.Cell4:SetData(item)
    -- UH.SetIcon(self.Cell2, item:IconId())
    -- UH.SetIcon(self.Cell3, item:IconId())
    -- UH.SetIcon(self.Cell4, item:IconId())
    local roll_data = AchieveData.Instance.recv_data.recv_roll_data
    if roll_data then
        self:SetNo1Text(roll_data[0].user_list,roll_data[0].rounds,roll_data[0].parss_time)
        self:SetNo2Text(roll_data[1].user_list,roll_data[1].rounds,roll_data[1].parss_time)
        self:SetNo3Text(roll_data[2].user_list,roll_data[2].rounds,roll_data[2].parss_time)
    end
end

function AchieveHonorView:ClickIcon()
    -- Debuger.LogError("xxxxxxxxxx")
    local info = {
        item_list = {Item.Create({item_id = 22005})}
    }
    ItemInfoCtrl.Instance:ItemInfoView(info)
end

function AchieveHonorView:SetFirstText(user_list,round,record)
    local member = ""
    for i=0, MAX_TEAM_MEMBER_NUM - 1 do
        if user_list then
        member = member.." "..user_list[i]
        end
    end
    self.FirstKillMember.text = member
    self.FirstKillRound.text = round
    self.FirstKillRecord.text = record
end

function AchieveHonorView:SetNo1Text(user_list,round,record)
    local member = ""
    for i=0, MAX_TEAM_MEMBER_NUM - 1 do
        if user_list then
        member = member.." "..user_list[i]
        end
    end
    self.No1Member.text = member
    self.No1Round.text = round
    self.No1Record.text = record
end

function AchieveHonorView:SetNo2Text(user_list,round,record)
    local member = ""
    for i=0, MAX_TEAM_MEMBER_NUM - 1 do
        if user_list then
        member = member.." "..user_list[i]
        end
    end
    self.No2Member.text = member
    self.No2Round.text = round
    self.No2Record.text = record
end

function AchieveHonorView:SetNo3Text(user_list,round,record)
    local member = ""
    for i=0, MAX_TEAM_MEMBER_NUM - 1 do
        if user_list then
        member = member.." "..user_list[i]
        end
    end
    self.No3Member.text = member
    self.No3Round.text = round
    self.No3Record.text = record
end


RightBtn = RightBtn or DeclareMono("RightBtn")
--RightList====UIWidgerList
--BtnName====Text
--Up====GameObject
--Down====GameObject
--LeftList====GameObject

function RightBtn:RightBtn()
    self.isClick = true
end

function RightBtn:ClickRightBtn()
    if self.LeftList.activeInHierarchy == false then
        self.RightList:SetActive(not self.RightList.activeSelf)
        self.Up:SetActive(self.RightList.activeSelf)
        self.Down:SetActive(not self.RightList.activeSelf)
        self.isClick = not self.isClick
    end
end

function RightBtn:HideList()
    self.RightList:SetActive(false)
    self.isClick = true
    self.Up:SetActive(false)
    self.Down:SetActive(true)
end


LeftBtn = LeftBtn or DeclareMono("LeftBtn")
--LeftList====UIWidgerList
--BtnName====Text
--Up====GameObject
--Down====GameObject
--RightList====GameObject

function LeftBtn:LeftBtn()
    self.isClick = true
end

function LeftBtn:ClickLeftBtn()
    if self.RightList.activeInHierarchy == false then
        self.LeftList:SetActive(not self.LeftList.activeSelf)
        self.Up:SetActive(self.LeftList.activeSelf)
        self.Down:SetActive(not self.LeftList.activeSelf)
        self.isClick = not self.isClick
    end
end

function LeftBtn:HideList()
    self.LeftList:SetActive(false)
    self.isClick = true
    self.Up:SetActive(false)
    self.Down:SetActive(true)
end


ListItem = ListItem or DeclareMono("ListItem",UIWidgetBaseItem)
--RightBtn====GameObject
--LeftBtn====GameObject
--Name====Text
--LeftBtnName====Text
--RightBtnName====Text
--RightList====UIWidgeList


function ListItem:ListItem()
    self.data_instance = AchieveData.Instance
end

function ListItem:SetData(data)
    self.data = data
    if data.type_honor then self.Name.text = data.type_honor 
    elseif data.fb_name then self.Name.text = data.fb_name end
end

function ListItem:ClickItem()
    if self.RightBtn then
        AchieveCtrl.Instance:SendAchievementHonorRollInfo(self.data.seq)
        self.RightBtnName.text = self.data.fb_name
        self.RightBtn:HideList()
    elseif self.LeftBtn then 
        self.LeftBtnName.text = self.data.type_honor
        local select_data = self.data_instance:GetSelectHonor(self.data.seq,self.data_instance.honor_relation[self.data.seq])
        if select_data[1] then
            AchieveCtrl.Instance:SendAchievementHonorRollInfo(select_data[1].seq)
            self.RightList:SetData(select_data)
            self.RightBtnName.text = select_data[1].fb_name
        else
            self.RightList:SetData({})
            self.RightBtnName.text = ""
        end
        self.LeftBtn:HideList()
    end
end



