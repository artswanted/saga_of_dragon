namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class MonsterPanel
	{
		/// <summary>
		/// 필수 디자이너 변수입니다.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// 사용 중인 모든 리소스를 정리합니다.
		/// </summary>
		/// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form 디자이너에서 생성한 코드

		/// <summary>
		/// 디자이너 지원에 필요한 메서드입니다.
		/// 이 메서드의 내용을 코드 편집기로 수정하지 마십시오.
		/// </summary>
		private void InitializeComponent()
		{
            System.Windows.Forms.ListViewItem listViewItem1 = new System.Windows.Forms.ListViewItem(new string[] {
            "Default",
            ""}, -1);
            System.Windows.Forms.ListViewItem listViewItem2 = new System.Windows.Forms.ListViewItem(new string[] {
            "Lv 1",
            ""}, -1);
            System.Windows.Forms.ListViewItem listViewItem3 = new System.Windows.Forms.ListViewItem(new string[] {
            "Lv 2",
            ""}, -1);
            System.Windows.Forms.ListViewItem listViewItem4 = new System.Windows.Forms.ListViewItem(new string[] {
            "Lv 3",
            ""}, -1);
            System.Windows.Forms.ListViewItem listViewItem5 = new System.Windows.Forms.ListViewItem(new string[] {
            "Lv 4",
            "",
            ""}, -1);
            System.Windows.Forms.ListViewItem listViewItem6 = new System.Windows.Forms.ListViewItem(new string[] {
            "Lv 5",
            ""}, -1);
            System.Windows.Forms.ListViewItem listViewItem7 = new System.Windows.Forms.ListViewItem(new string[] {
            "Lv 6",
            ""}, -1);
            this.m_lbMonsterList = new System.Windows.Forms.ListBox();
            this.m_lblMapNo = new System.Windows.Forms.Label();
            this.m_tbMapNo = new System.Windows.Forms.TextBox();
            this.m_btnLoad = new System.Windows.Forms.Button();
            this.m_lbSettedMonsterList = new System.Windows.Forms.ListBox();
            this.m_btnCommit = new System.Windows.Forms.Button();
            this.m_lblRegenPeriod = new System.Windows.Forms.Label();
            this.m_tbRegenPeriod = new System.Windows.Forms.TextBox();
            this.m_btnSetting = new System.Windows.Forms.Button();
            this.m_lState = new System.Windows.Forms.Label();
            this.m_lbMonsterBagList = new System.Windows.Forms.ListBox();
            this.m_btnBagAdd = new System.Windows.Forms.Button();
            this.m_btnBagDelete = new System.Windows.Forms.Button();
            this.m_btnBagModify = new System.Windows.Forms.Button();
            this.m_btnBagSave = new System.Windows.Forms.Button();
            this.m_btnBagLoad = new System.Windows.Forms.Button();
            this.m_btnApplyRegenPeriod = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.m_tbParentBagNo = new System.Windows.Forms.TextBox();
            this.m_btnApplyParentBagNo = new System.Windows.Forms.Button();
            this.m_btnCreateMonster = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.m_tbMemo = new System.Windows.Forms.TextBox();
            this.m_btnApplyMemo = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.m_tbPointGroup = new System.Windows.Forms.TextBox();
            this.m_btnApplyPointGroup = new System.Windows.Forms.Button();
            this.m_btnHideText = new System.Windows.Forms.Button();
            this.m_cbGroupNum = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.m_tbXmlPath = new System.Windows.Forms.TextBox();
            this.m_btnXmlpath = new System.Windows.Forms.Button();
            this.m_lvBagControl = new ListViewEx.ListViewEx();
            this.columnHeader1 = new System.Windows.Forms.ColumnHeader();
            this.columnHeader2 = new System.Windows.Forms.ColumnHeader();
            this.textBoxBagNo = new System.Windows.Forms.TextBox();
            this.m_lvLoadFailed = new System.Windows.Forms.ListView();
            this.ID = new System.Windows.Forms.ColumnHeader();
            this.BagNo = new System.Windows.Forms.ColumnHeader();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.m_cbAlteredMonsterShape = new System.Windows.Forms.CheckBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.m_btnApplyBagNum = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // m_lbMonsterList
            // 
            this.m_lbMonsterList.Enabled = false;
            this.m_lbMonsterList.FormattingEnabled = true;
            this.m_lbMonsterList.HorizontalScrollbar = true;
            this.m_lbMonsterList.IntegralHeight = false;
            this.m_lbMonsterList.ItemHeight = 12;
            this.m_lbMonsterList.Location = new System.Drawing.Point(498, 367);
            this.m_lbMonsterList.Name = "m_lbMonsterList";
            this.m_lbMonsterList.ScrollAlwaysVisible = true;
            this.m_lbMonsterList.Size = new System.Drawing.Size(136, 72);
            this.m_lbMonsterList.TabIndex = 0;
            this.m_lbMonsterList.Visible = false;
            this.m_lbMonsterList.DoubleClick += new System.EventHandler(this.m_lbMonsterList_DoubleClick);
            // 
            // m_lblMapNo
            // 
            this.m_lblMapNo.AutoSize = true;
            this.m_lblMapNo.Location = new System.Drawing.Point(12, 70);
            this.m_lblMapNo.Name = "m_lblMapNo";
            this.m_lblMapNo.Size = new System.Drawing.Size(50, 12);
            this.m_lblMapNo.TabIndex = 0;
            this.m_lblMapNo.Text = "Map No";
            // 
            // m_tbMapNo
            // 
            this.m_tbMapNo.Location = new System.Drawing.Point(68, 65);
            this.m_tbMapNo.Name = "m_tbMapNo";
            this.m_tbMapNo.Size = new System.Drawing.Size(115, 21);
            this.m_tbMapNo.TabIndex = 3;
            this.m_tbMapNo.TextChanged += new System.EventHandler(this.m_tbMapNo_TextChanged);
            // 
            // m_btnLoad
            // 
            this.m_btnLoad.Enabled = false;
            this.m_btnLoad.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnLoad.Location = new System.Drawing.Point(189, 65);
            this.m_btnLoad.Name = "m_btnLoad";
            this.m_btnLoad.Size = new System.Drawing.Size(75, 21);
            this.m_btnLoad.TabIndex = 4;
            this.m_btnLoad.Text = "Load";
            this.m_btnLoad.UseVisualStyleBackColor = true;
            this.m_btnLoad.Click += new System.EventHandler(this.m_btnLoad_Click);
            // 
            // m_lbSettedMonsterList
            // 
            this.m_lbSettedMonsterList.FormattingEnabled = true;
            this.m_lbSettedMonsterList.HorizontalScrollbar = true;
            this.m_lbSettedMonsterList.IntegralHeight = false;
            this.m_lbSettedMonsterList.ItemHeight = 12;
            this.m_lbSettedMonsterList.Location = new System.Drawing.Point(381, 367);
            this.m_lbSettedMonsterList.Name = "m_lbSettedMonsterList";
            this.m_lbSettedMonsterList.ScrollAlwaysVisible = true;
            this.m_lbSettedMonsterList.Size = new System.Drawing.Size(111, 72);
            this.m_lbSettedMonsterList.TabIndex = 4;
            this.m_lbSettedMonsterList.Visible = false;
            // 
            // m_btnCommit
            // 
            this.m_btnCommit.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnCommit.Location = new System.Drawing.Point(14, 464);
            this.m_btnCommit.Name = "m_btnCommit";
            this.m_btnCommit.Size = new System.Drawing.Size(250, 21);
            this.m_btnCommit.TabIndex = 17;
            this.m_btnCommit.Text = "Commit";
            this.m_btnCommit.UseVisualStyleBackColor = true;
            this.m_btnCommit.Click += new System.EventHandler(this.m_btnCommit_Click);
            // 
            // m_lblRegenPeriod
            // 
            this.m_lblRegenPeriod.AutoSize = true;
            this.m_lblRegenPeriod.Location = new System.Drawing.Point(11, 147);
            this.m_lblRegenPeriod.Name = "m_lblRegenPeriod";
            this.m_lblRegenPeriod.Size = new System.Drawing.Size(77, 12);
            this.m_lblRegenPeriod.TabIndex = 0;
            this.m_lblRegenPeriod.Text = "RegenPeriod";
            // 
            // m_tbRegenPeriod
            // 
            this.m_tbRegenPeriod.Location = new System.Drawing.Point(104, 142);
            this.m_tbRegenPeriod.Name = "m_tbRegenPeriod";
            this.m_tbRegenPeriod.Size = new System.Drawing.Size(79, 21);
            this.m_tbRegenPeriod.TabIndex = 6;
            this.m_tbRegenPeriod.Text = "60000";
            this.m_tbRegenPeriod.TextChanged += new System.EventHandler(this.m_tbRegenPeriod_TextChanged);
            // 
            // m_btnSetting
            // 
            this.m_btnSetting.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnSetting.Location = new System.Drawing.Point(498, 340);
            this.m_btnSetting.Name = "m_btnSetting";
            this.m_btnSetting.Size = new System.Drawing.Size(136, 21);
            this.m_btnSetting.TabIndex = 8;
            this.m_btnSetting.Text = "Load";
            this.m_btnSetting.UseVisualStyleBackColor = true;
            this.m_btnSetting.Visible = false;
            this.m_btnSetting.Click += new System.EventHandler(this.m_btnSetting_Click);
            // 
            // m_lState
            // 
            this.m_lState.AutoSize = true;
            this.m_lState.Location = new System.Drawing.Point(12, 11);
            this.m_lState.Name = "m_lState";
            this.m_lState.Size = new System.Drawing.Size(82, 12);
            this.m_lState.TabIndex = 0;
            this.m_lState.Text = "Disconnected";
            // 
            // m_lbMonsterBagList
            // 
            this.m_lbMonsterBagList.Enabled = false;
            this.m_lbMonsterBagList.FormattingEnabled = true;
            this.m_lbMonsterBagList.ItemHeight = 12;
            this.m_lbMonsterBagList.Location = new System.Drawing.Point(381, 445);
            this.m_lbMonsterBagList.Name = "m_lbMonsterBagList";
            this.m_lbMonsterBagList.ScrollAlwaysVisible = true;
            this.m_lbMonsterBagList.Size = new System.Drawing.Size(253, 64);
            this.m_lbMonsterBagList.TabIndex = 10;
            this.m_lbMonsterBagList.Visible = false;
            this.m_lbMonsterBagList.DoubleClick += new System.EventHandler(this.m_lbMonsterBagList_DoubleClick);
            // 
            // m_btnBagAdd
            // 
            this.m_btnBagAdd.Enabled = false;
            this.m_btnBagAdd.Location = new System.Drawing.Point(381, 515);
            this.m_btnBagAdd.Name = "m_btnBagAdd";
            this.m_btnBagAdd.Size = new System.Drawing.Size(75, 23);
            this.m_btnBagAdd.TabIndex = 11;
            this.m_btnBagAdd.Text = "Add";
            this.m_btnBagAdd.UseVisualStyleBackColor = true;
            this.m_btnBagAdd.Visible = false;
            this.m_btnBagAdd.Click += new System.EventHandler(this.m_btnBagAdd_Click);
            // 
            // m_btnBagDelete
            // 
            this.m_btnBagDelete.Enabled = false;
            this.m_btnBagDelete.Location = new System.Drawing.Point(543, 515);
            this.m_btnBagDelete.Name = "m_btnBagDelete";
            this.m_btnBagDelete.Size = new System.Drawing.Size(75, 23);
            this.m_btnBagDelete.TabIndex = 12;
            this.m_btnBagDelete.Text = "Delete";
            this.m_btnBagDelete.UseVisualStyleBackColor = true;
            this.m_btnBagDelete.Visible = false;
            this.m_btnBagDelete.Click += new System.EventHandler(this.m_btnBagDelete_Click);
            // 
            // m_btnBagModify
            // 
            this.m_btnBagModify.Enabled = false;
            this.m_btnBagModify.Location = new System.Drawing.Point(462, 515);
            this.m_btnBagModify.Name = "m_btnBagModify";
            this.m_btnBagModify.Size = new System.Drawing.Size(75, 23);
            this.m_btnBagModify.TabIndex = 13;
            this.m_btnBagModify.Text = "Modify";
            this.m_btnBagModify.UseVisualStyleBackColor = true;
            this.m_btnBagModify.Visible = false;
            this.m_btnBagModify.Click += new System.EventHandler(this.m_btnBagModify_Click);
            // 
            // m_btnBagSave
            // 
            this.m_btnBagSave.Enabled = false;
            this.m_btnBagSave.Location = new System.Drawing.Point(381, 544);
            this.m_btnBagSave.Name = "m_btnBagSave";
            this.m_btnBagSave.Size = new System.Drawing.Size(75, 23);
            this.m_btnBagSave.TabIndex = 14;
            this.m_btnBagSave.Text = "Save";
            this.m_btnBagSave.UseVisualStyleBackColor = true;
            this.m_btnBagSave.Visible = false;
            this.m_btnBagSave.Click += new System.EventHandler(this.m_btnBagSave_Click);
            // 
            // m_btnBagLoad
            // 
            this.m_btnBagLoad.Enabled = false;
            this.m_btnBagLoad.Location = new System.Drawing.Point(462, 544);
            this.m_btnBagLoad.Name = "m_btnBagLoad";
            this.m_btnBagLoad.Size = new System.Drawing.Size(75, 23);
            this.m_btnBagLoad.TabIndex = 15;
            this.m_btnBagLoad.Text = "Load";
            this.m_btnBagLoad.UseVisualStyleBackColor = true;
            this.m_btnBagLoad.Visible = false;
            this.m_btnBagLoad.Click += new System.EventHandler(this.m_btnBagLoad_Click);
            // 
            // m_btnApplyRegenPeriod
            // 
            this.m_btnApplyRegenPeriod.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnApplyRegenPeriod.Location = new System.Drawing.Point(189, 142);
            this.m_btnApplyRegenPeriod.Name = "m_btnApplyRegenPeriod";
            this.m_btnApplyRegenPeriod.Size = new System.Drawing.Size(75, 21);
            this.m_btnApplyRegenPeriod.TabIndex = 7;
            this.m_btnApplyRegenPeriod.Text = "Apply";
            this.m_btnApplyRegenPeriod.UseVisualStyleBackColor = true;
            this.m_btnApplyRegenPeriod.Click += new System.EventHandler(this.m_btnApplyRegenPeriod_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(11, 174);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(87, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "ParentBagNo";
            // 
            // m_tbParentBagNo
            // 
            this.m_tbParentBagNo.Location = new System.Drawing.Point(104, 169);
            this.m_tbParentBagNo.Name = "m_tbParentBagNo";
            this.m_tbParentBagNo.Size = new System.Drawing.Size(79, 21);
            this.m_tbParentBagNo.TabIndex = 8;
            this.m_tbParentBagNo.TextChanged += new System.EventHandler(this.m_tbParentBagNo_TextChanged);
            // 
            // m_btnApplyParentBagNo
            // 
            this.m_btnApplyParentBagNo.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnApplyParentBagNo.Location = new System.Drawing.Point(189, 169);
            this.m_btnApplyParentBagNo.Name = "m_btnApplyParentBagNo";
            this.m_btnApplyParentBagNo.Size = new System.Drawing.Size(75, 21);
            this.m_btnApplyParentBagNo.TabIndex = 9;
            this.m_btnApplyParentBagNo.Text = "Apply";
            this.m_btnApplyParentBagNo.UseVisualStyleBackColor = true;
            this.m_btnApplyParentBagNo.Click += new System.EventHandler(this.m_btnApplyParentBagNo_Click);
            // 
            // m_btnCreateMonster
            // 
            this.m_btnCreateMonster.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnCreateMonster.Location = new System.Drawing.Point(14, 436);
            this.m_btnCreateMonster.Name = "m_btnCreateMonster";
            this.m_btnCreateMonster.Size = new System.Drawing.Size(169, 21);
            this.m_btnCreateMonster.TabIndex = 16;
            this.m_btnCreateMonster.Text = "Create Monster";
            this.m_btnCreateMonster.UseVisualStyleBackColor = true;
            this.m_btnCreateMonster.Click += new System.EventHandler(this.m_btnCreateMonster_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 228);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(41, 12);
            this.label2.TabIndex = 0;
            this.label2.Text = "Memo";
            // 
            // m_tbMemo
            // 
            this.m_tbMemo.Location = new System.Drawing.Point(104, 223);
            this.m_tbMemo.Name = "m_tbMemo";
            this.m_tbMemo.Size = new System.Drawing.Size(79, 21);
            this.m_tbMemo.TabIndex = 12;
            // 
            // m_btnApplyMemo
            // 
            this.m_btnApplyMemo.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnApplyMemo.Location = new System.Drawing.Point(189, 223);
            this.m_btnApplyMemo.Name = "m_btnApplyMemo";
            this.m_btnApplyMemo.Size = new System.Drawing.Size(75, 21);
            this.m_btnApplyMemo.TabIndex = 13;
            this.m_btnApplyMemo.Text = "Apply";
            this.m_btnApplyMemo.UseVisualStyleBackColor = true;
            this.m_btnApplyMemo.Click += new System.EventHandler(this.m_btnApplyMemo_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(11, 201);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(67, 12);
            this.label3.TabIndex = 0;
            this.label3.Text = "PointGroup";
            // 
            // m_tbPointGroup
            // 
            this.m_tbPointGroup.Location = new System.Drawing.Point(104, 196);
            this.m_tbPointGroup.Name = "m_tbPointGroup";
            this.m_tbPointGroup.Size = new System.Drawing.Size(79, 21);
            this.m_tbPointGroup.TabIndex = 10;
            this.m_tbPointGroup.TextChanged += new System.EventHandler(this.m_tbRegenPeriod_TextChanged);
            // 
            // m_btnApplyPointGroup
            // 
            this.m_btnApplyPointGroup.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnApplyPointGroup.Location = new System.Drawing.Point(189, 196);
            this.m_btnApplyPointGroup.Name = "m_btnApplyPointGroup";
            this.m_btnApplyPointGroup.Size = new System.Drawing.Size(75, 21);
            this.m_btnApplyPointGroup.TabIndex = 11;
            this.m_btnApplyPointGroup.Text = "Apply";
            this.m_btnApplyPointGroup.UseVisualStyleBackColor = true;
            this.m_btnApplyPointGroup.Click += new System.EventHandler(this.m_btnApplyPointGroup_Click);
            // 
            // m_btnHideText
            // 
            this.m_btnHideText.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnHideText.Location = new System.Drawing.Point(189, 436);
            this.m_btnHideText.Name = "m_btnHideText";
            this.m_btnHideText.Size = new System.Drawing.Size(75, 21);
            this.m_btnHideText.TabIndex = 16;
            this.m_btnHideText.Text = "Hide Text";
            this.m_btnHideText.UseVisualStyleBackColor = true;
            this.m_btnHideText.Click += new System.EventHandler(this.m_btnHideText_Click);
            // 
            // m_cbGroupNum
            // 
            this.m_cbGroupNum.FormattingEnabled = true;
            this.m_cbGroupNum.Location = new System.Drawing.Point(104, 116);
            this.m_cbGroupNum.Name = "m_cbGroupNum";
            this.m_cbGroupNum.Size = new System.Drawing.Size(160, 20);
            this.m_cbGroupNum.TabIndex = 5;
            this.m_cbGroupNum.SelectedIndexChanged += new System.EventHandler(this.m_cbGroupNum_SelectedIndexChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(11, 120);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(59, 12);
            this.label4.TabIndex = 0;
            this.label4.Text = "Group No";
            // 
            // m_tbXmlPath
            // 
            this.m_tbXmlPath.Location = new System.Drawing.Point(14, 38);
            this.m_tbXmlPath.Name = "m_tbXmlPath";
            this.m_tbXmlPath.Size = new System.Drawing.Size(169, 21);
            this.m_tbXmlPath.TabIndex = 1;
            this.m_tbXmlPath.TextChanged += new System.EventHandler(this.m_tbXmlPath_TextChanged);
            // 
            // m_btnXmlpath
            // 
            this.m_btnXmlpath.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnXmlpath.Location = new System.Drawing.Point(189, 38);
            this.m_btnXmlpath.Name = "m_btnXmlpath";
            this.m_btnXmlpath.Size = new System.Drawing.Size(75, 21);
            this.m_btnXmlpath.TabIndex = 2;
            this.m_btnXmlpath.Text = "XML Path";
            this.m_btnXmlpath.UseVisualStyleBackColor = true;
            this.m_btnXmlpath.Click += new System.EventHandler(this.m_btnXmlpath_Click);
            // 
            // m_lvBagControl
            // 
            this.m_lvBagControl.AllowColumnReorder = true;
            this.m_lvBagControl.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.m_lvBagControl.CheckBoxes = true;
            this.m_lvBagControl.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.columnHeader1,
            this.columnHeader2});
            this.m_lvBagControl.DoubleClickActivation = false;
            this.m_lvBagControl.FullRowSelect = true;
            listViewItem1.StateImageIndex = 0;
            listViewItem2.StateImageIndex = 0;
            listViewItem3.StateImageIndex = 0;
            listViewItem4.StateImageIndex = 0;
            listViewItem5.StateImageIndex = 0;
            listViewItem6.StateImageIndex = 0;
            listViewItem7.StateImageIndex = 0;
            this.m_lvBagControl.Items.AddRange(new System.Windows.Forms.ListViewItem[] {
            listViewItem1,
            listViewItem2,
            listViewItem3,
            listViewItem4,
            listViewItem5,
            listViewItem6,
            listViewItem7});
            this.m_lvBagControl.Location = new System.Drawing.Point(6, 13);
            this.m_lvBagControl.Name = "m_lvBagControl";
            this.m_lvBagControl.Size = new System.Drawing.Size(238, 141);
            this.m_lvBagControl.TabIndex = 0;
            this.m_lvBagControl.UseCompatibleStateImageBehavior = false;
            this.m_lvBagControl.View = System.Windows.Forms.View.Details;
            this.m_lvBagControl.SubItemClicked += new ListViewEx.SubItemEventHandler(this.m_lvBagControl_SubItemClicked);
            this.m_lvBagControl.ItemChecked += new System.Windows.Forms.ItemCheckedEventHandler(this.m_lvBagControl_ItemClicked);
            // 
            // columnHeader1
            // 
            this.columnHeader1.Text = "Name";
            this.columnHeader1.Width = 70;
            // 
            // columnHeader2
            // 
            this.columnHeader2.Text = "BagNo";
            this.columnHeader2.Width = 91;
            // 
            // textBoxBagNo
            // 
            this.textBoxBagNo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.textBoxBagNo.Location = new System.Drawing.Point(270, 259);
            this.textBoxBagNo.Name = "textBoxBagNo";
            this.textBoxBagNo.Size = new System.Drawing.Size(96, 21);
            this.textBoxBagNo.TabIndex = 1;
            this.textBoxBagNo.Visible = false;
            // 
            // m_lvLoadFailed
            // 
            this.m_lvLoadFailed.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.ID,
            this.BagNo});
            this.m_lvLoadFailed.FullRowSelect = true;
            this.m_lvLoadFailed.GridLines = true;
            this.m_lvLoadFailed.Location = new System.Drawing.Point(6, 20);
            this.m_lvLoadFailed.Name = "m_lvLoadFailed";
            this.m_lvLoadFailed.Size = new System.Drawing.Size(238, 132);
            this.m_lvLoadFailed.TabIndex = 18;
            this.m_lvLoadFailed.UseCompatibleStateImageBehavior = false;
            this.m_lvLoadFailed.View = System.Windows.Forms.View.Details;
            // 
            // ID
            // 
            this.ID.Text = "ID";
            // 
            // BagNo
            // 
            this.BagNo.Text = "Bag No";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.m_lvLoadFailed);
            this.groupBox1.Location = new System.Drawing.Point(14, 495);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(250, 158);
            this.groupBox1.TabIndex = 19;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Load Failed ";
            // 
            // m_cbAlteredMonsterShape
            // 
            this.m_cbAlteredMonsterShape.AutoSize = true;
            this.m_cbAlteredMonsterShape.Checked = true;
            this.m_cbAlteredMonsterShape.CheckState = System.Windows.Forms.CheckState.Checked;
            this.m_cbAlteredMonsterShape.Location = new System.Drawing.Point(14, 92);
            this.m_cbAlteredMonsterShape.Name = "m_cbAlteredMonsterShape";
            this.m_cbAlteredMonsterShape.Size = new System.Drawing.Size(180, 16);
            this.m_cbAlteredMonsterShape.TabIndex = 20;
            this.m_cbAlteredMonsterShape.Text = "Load monster in triangle represents";
            this.m_cbAlteredMonsterShape.UseVisualStyleBackColor = true;
            this.m_cbAlteredMonsterShape.CheckedChanged += new System.EventHandler(this.m_cbAlteredMonsterShape_CheckedChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.m_btnApplyBagNum);
            this.groupBox2.Controls.Add(this.m_lvBagControl);
            this.groupBox2.Location = new System.Drawing.Point(14, 250);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(250, 180);
            this.groupBox2.TabIndex = 21;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Group No";
            // 
            // m_btnApplyBagNum
            // 
            this.m_btnApplyBagNum.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnApplyBagNum.Location = new System.Drawing.Point(169, 156);
            this.m_btnApplyBagNum.Name = "m_btnApplyBagNum";
            this.m_btnApplyBagNum.Size = new System.Drawing.Size(75, 21);
            this.m_btnApplyBagNum.TabIndex = 22;
            this.m_btnApplyBagNum.Text = "Apply";
            this.m_btnApplyBagNum.UseVisualStyleBackColor = true;
            this.m_btnApplyBagNum.Click += new System.EventHandler(this.m_btnApplyBagNum_Click);
            // 
            // MonsterPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoScroll = true;
            this.AutoScrollMargin = new System.Drawing.Size(10, 10);
            this.ClientSize = new System.Drawing.Size(715, 660);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.m_cbAlteredMonsterShape);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.m_btnXmlpath);
            this.Controls.Add(this.m_tbXmlPath);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.m_cbGroupNum);
            this.Controls.Add(this.m_btnApplyMemo);
            this.Controls.Add(this.m_tbMemo);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.m_btnCreateMonster);
            this.Controls.Add(this.m_btnApplyParentBagNo);
            this.Controls.Add(this.m_tbParentBagNo);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.m_btnApplyPointGroup);
            this.Controls.Add(this.m_btnHideText);
            this.Controls.Add(this.m_btnApplyRegenPeriod);
            this.Controls.Add(this.m_btnBagLoad);
            this.Controls.Add(this.m_btnBagSave);
            this.Controls.Add(this.m_btnBagModify);
            this.Controls.Add(this.m_btnBagDelete);
            this.Controls.Add(this.m_btnBagAdd);
            this.Controls.Add(this.m_lbMonsterBagList);
            this.Controls.Add(this.m_lState);
            this.Controls.Add(this.m_btnSetting);
            this.Controls.Add(this.m_tbPointGroup);
            this.Controls.Add(this.m_tbRegenPeriod);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.m_lblRegenPeriod);
            this.Controls.Add(this.m_btnCommit);
            this.Controls.Add(this.m_lbSettedMonsterList);
            this.Controls.Add(this.m_btnLoad);
            this.Controls.Add(this.m_tbMapNo);
            this.Controls.Add(this.m_lblMapNo);
            this.Controls.Add(this.m_lbMonsterList);
            this.Controls.Add(this.textBoxBagNo);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Name = "MonsterPanel";
            this.Text = "MonsterPanel";
            this.Load += new System.EventHandler(this.MonsterPanel_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ListBox m_lbMonsterList;
		private System.Windows.Forms.Label m_lblMapNo;
		private System.Windows.Forms.TextBox m_tbMapNo;
		private System.Windows.Forms.Button m_btnLoad;
		private System.Windows.Forms.ListBox m_lbSettedMonsterList;
		private System.Windows.Forms.Button m_btnCommit;
		private System.Windows.Forms.Label m_lblRegenPeriod;
		private System.Windows.Forms.TextBox m_tbRegenPeriod;
		private System.Windows.Forms.Button m_btnSetting;
		private System.Windows.Forms.Label m_lState;
		private System.Windows.Forms.ListBox m_lbMonsterBagList;
		private System.Windows.Forms.Button m_btnBagAdd;
		private System.Windows.Forms.Button m_btnBagDelete;
		private System.Windows.Forms.Button m_btnBagModify;
		private System.Windows.Forms.Button m_btnBagSave;
		private System.Windows.Forms.Button m_btnBagLoad;
		private System.Windows.Forms.Button m_btnApplyRegenPeriod;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox m_tbParentBagNo;
		private System.Windows.Forms.Button m_btnApplyParentBagNo;
		private System.Windows.Forms.Button m_btnCreateMonster;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox m_tbMemo;
		private System.Windows.Forms.Button m_btnApplyMemo;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox m_tbPointGroup;
		private System.Windows.Forms.Button m_btnApplyPointGroup;
		private System.Windows.Forms.Button m_btnHideText;
		private System.Windows.Forms.ComboBox m_cbGroupNum;
        private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox m_tbXmlPath;
		private System.Windows.Forms.Button m_btnXmlpath;
        private ListViewEx.ListViewEx m_lvBagControl;
        private System.Windows.Forms.TextBox textBoxBagNo;
		private System.Windows.Forms.ListView m_lvLoadFailed;
		private System.Windows.Forms.ColumnHeader ID;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.ColumnHeader BagNo;
		private System.Windows.Forms.CheckBox m_cbAlteredMonsterShape;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.ColumnHeader columnHeader1;
        private System.Windows.Forms.ColumnHeader columnHeader2;
        private System.Windows.Forms.Button m_btnApplyBagNum;
	}
}