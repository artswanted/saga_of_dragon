namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
    partial class BreakObjectPanel
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
			this.listGroup = new System.Windows.Forms.ListBox();
			this.btnAdd = new System.Windows.Forms.Button();
			this.btnCreate = new System.Windows.Forms.Button();
			this.listObject = new System.Windows.Forms.ListBox();
			this.btnSave = new System.Windows.Forms.Button();
			this.btnLoad = new System.Windows.Forms.Button();
			this.tbPosZ = new System.Windows.Forms.TextBox();
			this.tbObjectNumber = new System.Windows.Forms.TextBox();
			this.btnModify = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.tbGroupName = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.btnRemove = new System.Windows.Forms.Button();
			this.btnGroupRemove = new System.Windows.Forms.Button();
			this.btnObjectListClear = new System.Windows.Forms.Button();
			this.tbXmlPath = new System.Windows.Forms.TextBox();
			this.btnXmlPath = new System.Windows.Forms.Button();
			this.btnLoadDB = new System.Windows.Forms.Button();
			this.labelDBConnectState = new System.Windows.Forms.Label();
			this.tbMapNumber = new System.Windows.Forms.TextBox();
			this.btnSaveDB = new System.Windows.Forms.Button();
			this.tbPosX = new System.Windows.Forms.TextBox();
			this.tbPosY = new System.Windows.Forms.TextBox();
			this.tbElementNo = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.listObjectBag = new System.Windows.Forms.ListBox();
			this.listObjectElement = new System.Windows.Forms.ListBox();
			this.btnRefresh = new System.Windows.Forms.Button();
			this.btnElementRemove = new System.Windows.Forms.Button();
			this.tbRegenTime = new System.Windows.Forms.TextBox();
			this.label8 = new System.Windows.Forms.Label();
			this.btnObjectBagRemove = new System.Windows.Forms.Button();
			this.tbGroupNum = new System.Windows.Forms.TextBox();
			this.label9 = new System.Windows.Forms.Label();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.groupBox4 = new System.Windows.Forms.GroupBox();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.groupBox3.SuspendLayout();
			this.groupBox4.SuspendLayout();
			this.SuspendLayout();
			// 
			// listGroup
			// 
			this.listGroup.FormattingEnabled = true;
			this.listGroup.ItemHeight = 12;
			this.listGroup.Location = new System.Drawing.Point(10, 465);
			this.listGroup.Name = "listGroup";
			this.listGroup.Size = new System.Drawing.Size(291, 76);
			this.listGroup.TabIndex = 9;
			this.listGroup.SelectedIndexChanged += new System.EventHandler(this.listGroup_SelectedIndexChanged);
			// 
			// btnAdd
			// 
			this.btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnAdd.Location = new System.Drawing.Point(129, 132);
			this.btnAdd.Name = "btnAdd";
			this.btnAdd.Size = new System.Drawing.Size(74, 21);
			this.btnAdd.TabIndex = 7;
			this.btnAdd.Text = "AddObject";
			this.btnAdd.UseVisualStyleBackColor = true;
			this.btnAdd.Click += new System.EventHandler(this.btnAdd_Click);
			// 
			// btnCreate
			// 
			this.btnCreate.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnCreate.Location = new System.Drawing.Point(12, 98);
			this.btnCreate.Name = "btnCreate";
			this.btnCreate.Size = new System.Drawing.Size(191, 21);
			this.btnCreate.TabIndex = 3;
			this.btnCreate.Text = "Create";
			this.btnCreate.UseVisualStyleBackColor = true;
			this.btnCreate.Click += new System.EventHandler(this.btnCreate_Click);
			// 
			// listObject
			// 
			this.listObject.FormattingEnabled = true;
			this.listObject.HorizontalScrollbar = true;
			this.listObject.ItemHeight = 12;
			this.listObject.Location = new System.Drawing.Point(12, 132);
			this.listObject.Name = "listObject";
			this.listObject.Size = new System.Drawing.Size(111, 76);
			this.listObject.TabIndex = 6;
			this.listObject.SelectedIndexChanged += new System.EventHandler(this.listObject_SelectedIndexChanged);
			// 
			// btnSave
			// 
			this.btnSave.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnSave.Location = new System.Drawing.Point(636, 12);
			this.btnSave.Name = "btnSave";
			this.btnSave.Size = new System.Drawing.Size(75, 23);
			this.btnSave.TabIndex = 0;
			this.btnSave.TabStop = false;
			this.btnSave.Text = "Save";
			this.btnSave.UseVisualStyleBackColor = true;
			this.btnSave.Visible = false;
			//this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
			// 
			// btnLoad
			// 
			this.btnLoad.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnLoad.Location = new System.Drawing.Point(636, 41);
			this.btnLoad.Name = "btnLoad";
			this.btnLoad.Size = new System.Drawing.Size(75, 23);
			this.btnLoad.TabIndex = 100;
			this.btnLoad.TabStop = false;
			this.btnLoad.Text = "Load";
			this.btnLoad.UseVisualStyleBackColor = true;
			this.btnLoad.Visible = false;
			//this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
			// 
			// tbPosZ
			// 
			this.tbPosZ.Location = new System.Drawing.Point(162, 75);
			this.tbPosZ.Name = "tbPosZ";
			this.tbPosZ.Size = new System.Drawing.Size(41, 21);
			this.tbPosZ.TabIndex = 4;
			// 
			// tbObjectNumber
			// 
			this.tbObjectNumber.Location = new System.Drawing.Point(94, 48);
			this.tbObjectNumber.Name = "tbObjectNumber";
			this.tbObjectNumber.Size = new System.Drawing.Size(109, 21);
			this.tbObjectNumber.TabIndex = 1;
			// 
			// btnModify
			// 
			this.btnModify.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnModify.Location = new System.Drawing.Point(12, 105);
			this.btnModify.Name = "btnModify";
			this.btnModify.Size = new System.Drawing.Size(191, 21);
			this.btnModify.TabIndex = 5;
			this.btnModify.Text = "Modify";
			this.btnModify.UseVisualStyleBackColor = true;
			this.btnModify.Click += new System.EventHandler(this.btnModify_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(16, 51);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(67, 12);
			this.label1.TabIndex = 0;
			this.label1.Text = "Object NO.";
			// 
			// tbGroupName
			// 
			this.tbGroupName.Location = new System.Drawing.Point(94, 16);
			this.tbGroupName.Name = "tbGroupName";
			this.tbGroupName.Size = new System.Drawing.Size(109, 21);
			this.tbGroupName.TabIndex = 0;
			this.tbGroupName.TextChanged += new System.EventHandler(this.tbGroupName_TextChanged);
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(30, 19);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(53, 12);
			this.label3.TabIndex = 0;
			this.label3.Text = "Bag NO.";
			// 
			// btnRemove
			// 
			this.btnRemove.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnRemove.Location = new System.Drawing.Point(129, 159);
			this.btnRemove.Name = "btnRemove";
			this.btnRemove.Size = new System.Drawing.Size(74, 21);
			this.btnRemove.TabIndex = 8;
			this.btnRemove.Text = "Remove";
			this.btnRemove.UseVisualStyleBackColor = true;
			this.btnRemove.Click += new System.EventHandler(this.btnRemove_Click);
			// 
			// btnGroupRemove
			// 
			this.btnGroupRemove.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnGroupRemove.Location = new System.Drawing.Point(307, 465);
			this.btnGroupRemove.Name = "btnGroupRemove";
			this.btnGroupRemove.Size = new System.Drawing.Size(103, 76);
			this.btnGroupRemove.TabIndex = 10;
			this.btnGroupRemove.Text = "GroupRemove";
			this.btnGroupRemove.UseVisualStyleBackColor = true;
			this.btnGroupRemove.Click += new System.EventHandler(this.btnGroupRemove_Click);
			// 
			// btnObjectListClear
			// 
			this.btnObjectListClear.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnObjectListClear.Location = new System.Drawing.Point(129, 187);
			this.btnObjectListClear.Name = "btnObjectListClear";
			this.btnObjectListClear.Size = new System.Drawing.Size(74, 21);
			this.btnObjectListClear.TabIndex = 9;
			this.btnObjectListClear.Text = "Clear";
			this.btnObjectListClear.UseVisualStyleBackColor = true;
			this.btnObjectListClear.Click += new System.EventHandler(this.btnObjectListClear_Click);
			// 
			// tbXmlPath
			// 
			this.tbXmlPath.Location = new System.Drawing.Point(10, 56);
			this.tbXmlPath.Name = "tbXmlPath";
			this.tbXmlPath.Size = new System.Drawing.Size(149, 21);
			this.tbXmlPath.TabIndex = 1;
			this.tbXmlPath.TextChanged += new System.EventHandler(this.tbXmlPath_TextChanged);
			// 
			// btnXmlPath
			// 
			this.btnXmlPath.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnXmlPath.Location = new System.Drawing.Point(157, 56);
			this.btnXmlPath.Name = "btnXmlPath";
			this.btnXmlPath.Size = new System.Drawing.Size(71, 21);
			this.btnXmlPath.TabIndex = 2;
			this.btnXmlPath.Text = "XmlPath";
			this.btnXmlPath.UseVisualStyleBackColor = true;
			this.btnXmlPath.Click += new System.EventHandler(this.btnXmlPath_Click);
			// 
			// btnLoadDB
			// 
			this.btnLoadDB.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnLoadDB.Location = new System.Drawing.Point(10, 83);
			this.btnLoadDB.Name = "btnLoadDB";
			this.btnLoadDB.Size = new System.Drawing.Size(105, 21);
			this.btnLoadDB.TabIndex = 3;
			this.btnLoadDB.Text = "LoadDB";
			this.btnLoadDB.UseVisualStyleBackColor = true;
			this.btnLoadDB.Click += new System.EventHandler(this.btnDBSetting_Click);
			// 
			// labelDBConnectState
			// 
			this.labelDBConnectState.AutoSize = true;
			this.labelDBConnectState.Location = new System.Drawing.Point(12, 10);
			this.labelDBConnectState.Name = "labelDBConnectState";
			this.labelDBConnectState.Size = new System.Drawing.Size(33, 12);
			this.labelDBConnectState.TabIndex = 0;
			this.labelDBConnectState.Text = "State";
			// 
			// tbMapNumber
			// 
			this.tbMapNumber.Location = new System.Drawing.Point(73, 29);
			this.tbMapNumber.Name = "tbMapNumber";
			this.tbMapNumber.Size = new System.Drawing.Size(155, 21);
			this.tbMapNumber.TabIndex = 0;
			this.tbMapNumber.Text = "0";
			// 
			// btnSaveDB
			// 
			this.btnSaveDB.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnSaveDB.Location = new System.Drawing.Point(123, 83);
			this.btnSaveDB.Name = "btnSaveDB";
			this.btnSaveDB.Size = new System.Drawing.Size(105, 21);
			this.btnSaveDB.TabIndex = 4;
			this.btnSaveDB.Text = "SaveDB";
			this.btnSaveDB.UseVisualStyleBackColor = true;
			this.btnSaveDB.Click += new System.EventHandler(this.btnSaveDB_Click);
			// 
			// tbPosX
			// 
			this.tbPosX.Location = new System.Drawing.Point(32, 75);
			this.tbPosX.Name = "tbPosX";
			this.tbPosX.Size = new System.Drawing.Size(41, 21);
			this.tbPosX.TabIndex = 2;
			// 
			// tbPosY
			// 
			this.tbPosY.Location = new System.Drawing.Point(94, 75);
			this.tbPosY.Name = "tbPosY";
			this.tbPosY.Size = new System.Drawing.Size(41, 21);
			this.tbPosY.TabIndex = 3;
			// 
			// tbElementNo
			// 
			this.tbElementNo.Location = new System.Drawing.Point(94, 21);
			this.tbElementNo.Name = "tbElementNo";
			this.tbElementNo.Size = new System.Drawing.Size(109, 21);
			this.tbElementNo.TabIndex = 0;
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(6, 24);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(77, 12);
			this.label4.TabIndex = 0;
			this.label4.Text = "Element NO.";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(10, 32);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(57, 12);
			this.label5.TabIndex = 5;
			this.label5.Text = "MapNum";
			// 
			// listObjectBag
			// 
			this.listObjectBag.FormattingEnabled = true;
			this.listObjectBag.HorizontalScrollbar = true;
			this.listObjectBag.ItemHeight = 12;
			this.listObjectBag.Location = new System.Drawing.Point(8, 16);
			this.listObjectBag.Name = "listObjectBag";
			this.listObjectBag.Size = new System.Drawing.Size(161, 172);
			this.listObjectBag.TabIndex = 0;
			this.listObjectBag.SelectedIndexChanged += new System.EventHandler(this.listObjectBag_SelectedIndexChanged);
			// 
			// listObjectElement
			// 
			this.listObjectElement.FormattingEnabled = true;
			this.listObjectElement.HorizontalScrollbar = true;
			this.listObjectElement.ItemHeight = 12;
			this.listObjectElement.Location = new System.Drawing.Point(8, 16);
			this.listObjectElement.Name = "listObjectElement";
			this.listObjectElement.Size = new System.Drawing.Size(161, 160);
			this.listObjectElement.TabIndex = 0;
			this.listObjectElement.SelectedIndexChanged += new System.EventHandler(this.listObjectElement_SelectedIndexChanged);
			// 
			// btnRefresh
			// 
			this.btnRefresh.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnRefresh.Location = new System.Drawing.Point(8, 182);
			this.btnRefresh.Name = "btnRefresh";
			this.btnRefresh.Size = new System.Drawing.Size(76, 23);
			this.btnRefresh.TabIndex = 1;
			this.btnRefresh.Text = "Refresh";
			this.btnRefresh.UseVisualStyleBackColor = true;
			this.btnRefresh.Click += new System.EventHandler(this.btnRefresh_Click);
			// 
			// btnElementRemove
			// 
			this.btnElementRemove.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnElementRemove.Location = new System.Drawing.Point(93, 182);
			this.btnElementRemove.Name = "btnElementRemove";
			this.btnElementRemove.Size = new System.Drawing.Size(76, 23);
			this.btnElementRemove.TabIndex = 2;
			this.btnElementRemove.Text = "Remove";
			this.btnElementRemove.UseVisualStyleBackColor = true;
			this.btnElementRemove.Click += new System.EventHandler(this.btnElementRemove_Click);
			// 
			// tbRegenTime
			// 
			this.tbRegenTime.Location = new System.Drawing.Point(94, 43);
			this.tbRegenTime.Name = "tbRegenTime";
			this.tbRegenTime.Size = new System.Drawing.Size(109, 21);
			this.tbRegenTime.TabIndex = 1;
			this.tbRegenTime.TextChanged += new System.EventHandler(this.tbRegenTime_TextChanged);
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(13, 46);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(70, 12);
			this.label8.TabIndex = 0;
			this.label8.Text = "RegenTime";
			// 
			// btnObjectBagRemove
			// 
			this.btnObjectBagRemove.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnObjectBagRemove.Location = new System.Drawing.Point(8, 194);
			this.btnObjectBagRemove.Name = "btnObjectBagRemove";
			this.btnObjectBagRemove.Size = new System.Drawing.Size(161, 23);
			this.btnObjectBagRemove.TabIndex = 1;
			this.btnObjectBagRemove.Text = "Remove";
			this.btnObjectBagRemove.UseVisualStyleBackColor = true;
			this.btnObjectBagRemove.Click += new System.EventHandler(this.btnObjectBagRemove_Click);
			// 
			// tbGroupNum
			// 
			this.tbGroupNum.Location = new System.Drawing.Point(94, 70);
			this.tbGroupNum.Name = "tbGroupNum";
			this.tbGroupNum.Size = new System.Drawing.Size(109, 21);
			this.tbGroupNum.TabIndex = 2;
			this.tbGroupNum.TextChanged += new System.EventHandler(this.tbGroupNum_TextChanged);
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.Location = new System.Drawing.Point(18, 73);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(65, 12);
			this.label9.TabIndex = 0;
			this.label9.Text = "Group NO.";
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.btnObjectBagRemove);
			this.groupBox1.Controls.Add(this.listObjectBag);
			this.groupBox1.Location = new System.Drawing.Point(234, 13);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(176, 226);
			this.groupBox1.TabIndex = 7;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Object Bag";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.btnElementRemove);
			this.groupBox2.Controls.Add(this.btnRefresh);
			this.groupBox2.Controls.Add(this.listObjectElement);
			this.groupBox2.Location = new System.Drawing.Point(234, 246);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(176, 213);
			this.groupBox2.TabIndex = 8;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Object Element";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(17, 80);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(13, 12);
			this.label2.TabIndex = 0;
			this.label2.Text = "X";
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(79, 80);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(13, 12);
			this.label6.TabIndex = 0;
			this.label6.Text = "Y";
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(147, 80);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(13, 12);
			this.label7.TabIndex = 0;
			this.label7.Text = "Z";
			// 
			// groupBox3
			// 
			this.groupBox3.Controls.Add(this.label7);
			this.groupBox3.Controls.Add(this.label6);
			this.groupBox3.Controls.Add(this.label2);
			this.groupBox3.Controls.Add(this.tbPosY);
			this.groupBox3.Controls.Add(this.tbPosX);
			this.groupBox3.Controls.Add(this.tbPosZ);
			this.groupBox3.Controls.Add(this.tbObjectNumber);
			this.groupBox3.Controls.Add(this.btnModify);
			this.groupBox3.Controls.Add(this.label4);
			this.groupBox3.Controls.Add(this.label1);
			this.groupBox3.Controls.Add(this.tbElementNo);
			this.groupBox3.Controls.Add(this.listObject);
			this.groupBox3.Controls.Add(this.btnAdd);
			this.groupBox3.Controls.Add(this.btnRemove);
			this.groupBox3.Controls.Add(this.btnObjectListClear);
			this.groupBox3.Location = new System.Drawing.Point(10, 106);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size(218, 218);
			this.groupBox3.TabIndex = 5;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Element Modify";
			// 
			// groupBox4
			// 
			this.groupBox4.Controls.Add(this.label9);
			this.groupBox4.Controls.Add(this.tbGroupNum);
			this.groupBox4.Controls.Add(this.label8);
			this.groupBox4.Controls.Add(this.tbRegenTime);
			this.groupBox4.Controls.Add(this.label3);
			this.groupBox4.Controls.Add(this.tbGroupName);
			this.groupBox4.Controls.Add(this.btnCreate);
			this.groupBox4.Location = new System.Drawing.Point(10, 330);
			this.groupBox4.Name = "groupBox4";
			this.groupBox4.Size = new System.Drawing.Size(218, 129);
			this.groupBox4.TabIndex = 6;
			this.groupBox4.TabStop = false;
			this.groupBox4.Text = "Create Object";
			// 
			// BreakObjectPanel
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoScroll = true;
			this.AutoScrollMargin = new System.Drawing.Size(10, 10);
			this.ClientSize = new System.Drawing.Size(838, 626);
			this.Controls.Add(this.groupBox4);
			this.Controls.Add(this.groupBox3);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.btnSaveDB);
			this.Controls.Add(this.tbMapNumber);
			this.Controls.Add(this.labelDBConnectState);
			this.Controls.Add(this.btnLoadDB);
			this.Controls.Add(this.btnXmlPath);
			this.Controls.Add(this.tbXmlPath);
			this.Controls.Add(this.btnGroupRemove);
			this.Controls.Add(this.btnLoad);
			this.Controls.Add(this.btnSave);
			this.Controls.Add(this.listGroup);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
			this.Name = "BreakObjectPanel";
			this.Text = "BreakObjectPanel";
			this.groupBox1.ResumeLayout(false);
			this.groupBox2.ResumeLayout(false);
			this.groupBox3.ResumeLayout(false);
			this.groupBox3.PerformLayout();
			this.groupBox4.ResumeLayout(false);
			this.groupBox4.PerformLayout();
			this.ResumeLayout(false);
			this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ListBox listGroup;
        private System.Windows.Forms.Button btnAdd;
        private System.Windows.Forms.Button btnCreate;
        private System.Windows.Forms.ListBox listObject;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.Button btnLoad;
        private System.Windows.Forms.TextBox tbPosZ;
        private System.Windows.Forms.TextBox tbObjectNumber;
        private System.Windows.Forms.Button btnModify;
		private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbGroupName;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btnRemove;
        private System.Windows.Forms.Button btnGroupRemove;
        private System.Windows.Forms.Button btnObjectListClear;
        private System.Windows.Forms.TextBox tbXmlPath;
        private System.Windows.Forms.Button btnXmlPath;
        private System.Windows.Forms.Button btnLoadDB;
        private System.Windows.Forms.Label labelDBConnectState;
        private System.Windows.Forms.TextBox tbMapNumber;
        private System.Windows.Forms.Button btnSaveDB;
        private System.Windows.Forms.TextBox tbPosX;
        private System.Windows.Forms.TextBox tbPosY;
        private System.Windows.Forms.TextBox tbElementNo;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ListBox listObjectBag;
        private System.Windows.Forms.ListBox listObjectElement;
		private System.Windows.Forms.Button btnRefresh;
        private System.Windows.Forms.Button btnElementRemove;
        private System.Windows.Forms.TextBox tbRegenTime;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button btnObjectBagRemove;
        private System.Windows.Forms.TextBox tbGroupNum;
        private System.Windows.Forms.Label label9;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.GroupBox groupBox3;
		private System.Windows.Forms.GroupBox groupBox4;
    }
}