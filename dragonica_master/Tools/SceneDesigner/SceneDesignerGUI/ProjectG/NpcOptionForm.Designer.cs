namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class NpcOptionForm
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
            this.label1 = new System.Windows.Forms.Label();
            this.m_tbName = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.m_tbActor = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.m_tbScript = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.m_tbLocX = new System.Windows.Forms.TextBox();
            this.m_tbLocY = new System.Windows.Forms.TextBox();
            this.m_tbLocZ = new System.Windows.Forms.TextBox();
            this.m_btnOK = new System.Windows.Forms.Button();
            this.m_btnCancel = new System.Windows.Forms.Button();
            this.m_tbGuid = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.m_btnGenerate = new System.Windows.Forms.Button();
            this.label9 = new System.Windows.Forms.Label();
            this.m_tbKid = new System.Windows.Forms.TextBox();
            this.label10 = new System.Windows.Forms.Label();
            this.m_cbType = new System.Windows.Forms.ComboBox();
            this.m_lbNpcSay = new System.Windows.Forms.ListBox();
            this.m_btnSayAdd = new System.Windows.Forms.Button();
            this.m_btnSayDel = new System.Windows.Forms.Button();
            this.m_btnSayModify = new System.Windows.Forms.Button();
            this.m_btnSayClear = new System.Windows.Forms.Button();
            this.m_btnSayMoveUp = new System.Windows.Forms.Button();
            this.m_btnSayMoveDown = new System.Windows.Forms.Button();
            this.label11 = new System.Windows.Forms.Label();
            this.label12 = new System.Windows.Forms.Label();
            this.m_cbHidden = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(39, 12);
            this.label1.TabIndex = 0;
            this.label1.Text = "Name";
            // 
            // m_tbName
            // 
            this.m_tbName.Location = new System.Drawing.Point(57, 6);
            this.m_tbName.Name = "m_tbName";
            this.m_tbName.Size = new System.Drawing.Size(157, 21);
            this.m_tbName.TabIndex = 0;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(249, 9);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(34, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "Actor";
            // 
            // m_tbActor
            // 
            this.m_tbActor.Location = new System.Drawing.Point(294, 6);
            this.m_tbActor.Name = "m_tbActor";
            this.m_tbActor.Size = new System.Drawing.Size(157, 21);
            this.m_tbActor.TabIndex = 1;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 36);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(37, 12);
            this.label3.TabIndex = 4;
            this.label3.Text = "Script";
            // 
            // m_tbScript
            // 
            this.m_tbScript.Location = new System.Drawing.Point(57, 33);
            this.m_tbScript.Name = "m_tbScript";
            this.m_tbScript.Size = new System.Drawing.Size(120, 21);
            this.m_tbScript.TabIndex = 2;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(183, 36);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(53, 12);
            this.label4.TabIndex = 6;
            this.label4.Text = "Location";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(242, 36);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(13, 12);
            this.label5.TabIndex = 7;
            this.label5.Text = "X";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(314, 36);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(13, 12);
            this.label6.TabIndex = 8;
            this.label6.Text = "Y";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(386, 36);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(13, 12);
            this.label7.TabIndex = 9;
            this.label7.Text = "Z";
            // 
            // m_tbLocX
            // 
            this.m_tbLocX.Location = new System.Drawing.Point(261, 33);
            this.m_tbLocX.Name = "m_tbLocX";
            this.m_tbLocX.Size = new System.Drawing.Size(47, 21);
            this.m_tbLocX.TabIndex = 3;
            // 
            // m_tbLocY
            // 
            this.m_tbLocY.Location = new System.Drawing.Point(333, 33);
            this.m_tbLocY.Name = "m_tbLocY";
            this.m_tbLocY.Size = new System.Drawing.Size(47, 21);
            this.m_tbLocY.TabIndex = 4;
            // 
            // m_tbLocZ
            // 
            this.m_tbLocZ.Location = new System.Drawing.Point(405, 33);
            this.m_tbLocZ.Name = "m_tbLocZ";
            this.m_tbLocZ.Size = new System.Drawing.Size(47, 21);
            this.m_tbLocZ.TabIndex = 5;
            // 
            // m_btnOK
            // 
            this.m_btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnOK.Location = new System.Drawing.Point(139, 312);
            this.m_btnOK.Name = "m_btnOK";
            this.m_btnOK.Size = new System.Drawing.Size(75, 23);
            this.m_btnOK.TabIndex = 9;
            this.m_btnOK.Text = "OK";
            this.m_btnOK.UseVisualStyleBackColor = true;
            this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
            // 
            // m_btnCancel
            // 
            this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnCancel.Location = new System.Drawing.Point(262, 312);
            this.m_btnCancel.Name = "m_btnCancel";
            this.m_btnCancel.Size = new System.Drawing.Size(75, 23);
            this.m_btnCancel.TabIndex = 10;
            this.m_btnCancel.Text = "Cancel";
            this.m_btnCancel.UseVisualStyleBackColor = true;
            this.m_btnCancel.Click += new System.EventHandler(this.m_btnCancel_Click);
            // 
            // m_tbGuid
            // 
            this.m_tbGuid.Enabled = false;
            this.m_tbGuid.Location = new System.Drawing.Point(57, 60);
            this.m_tbGuid.Name = "m_tbGuid";
            this.m_tbGuid.Size = new System.Drawing.Size(329, 21);
            this.m_tbGuid.TabIndex = 6;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(12, 63);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(33, 12);
            this.label8.TabIndex = 13;
            this.label8.Text = "GUID";
            // 
            // m_btnGenerate
            // 
            this.m_btnGenerate.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
            this.m_btnGenerate.Location = new System.Drawing.Point(388, 58);
            this.m_btnGenerate.Name = "m_btnGenerate";
            this.m_btnGenerate.Size = new System.Drawing.Size(65, 23);
            this.m_btnGenerate.TabIndex = 7;
            this.m_btnGenerate.Text = "Generate";
            this.m_btnGenerate.UseVisualStyleBackColor = true;
            this.m_btnGenerate.Click += new System.EventHandler(this.m_btnGenerate_Click);
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(12, 90);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(23, 12);
            this.label9.TabIndex = 15;
            this.label9.Text = "Kid";
            // 
            // m_tbKid
            // 
            this.m_tbKid.Location = new System.Drawing.Point(57, 87);
            this.m_tbKid.Name = "m_tbKid";
            this.m_tbKid.Size = new System.Drawing.Size(100, 21);
            this.m_tbKid.TabIndex = 8;
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(161, 91);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(34, 12);
            this.label10.TabIndex = 16;
            this.label10.Text = "Type";
            // 
            // m_cbType
            // 
            this.m_cbType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.m_cbType.FormattingEnabled = true;
            this.m_cbType.Items.AddRange(new object[] {
            "FIXED",
            "CRETURE",
            "DYNAMIC"});
            this.m_cbType.Location = new System.Drawing.Point(201, 87);
            this.m_cbType.Name = "m_cbType";
            this.m_cbType.Size = new System.Drawing.Size(107, 20);
            this.m_cbType.TabIndex = 17;
            // 
            // m_lbNpcSay
            // 
            this.m_lbNpcSay.FormattingEnabled = true;
            this.m_lbNpcSay.HorizontalScrollbar = true;
            this.m_lbNpcSay.ItemHeight = 12;
            this.m_lbNpcSay.Location = new System.Drawing.Point(57, 114);
            this.m_lbNpcSay.Name = "m_lbNpcSay";
            this.m_lbNpcSay.ScrollAlwaysVisible = true;
            this.m_lbNpcSay.Size = new System.Drawing.Size(394, 136);
            this.m_lbNpcSay.TabIndex = 18;
            this.m_lbNpcSay.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.m_lbNpcSay_MouseDoubleClick);
            // 
            // m_btnSayAdd
            // 
            this.m_btnSayAdd.Location = new System.Drawing.Point(57, 261);
            this.m_btnSayAdd.Name = "m_btnSayAdd";
            this.m_btnSayAdd.Size = new System.Drawing.Size(44, 23);
            this.m_btnSayAdd.TabIndex = 19;
            this.m_btnSayAdd.Text = "Add";
            this.m_btnSayAdd.UseVisualStyleBackColor = true;
            this.m_btnSayAdd.Click += new System.EventHandler(this.m_btnSayAdd_Click);
            // 
            // m_btnSayDel
            // 
            this.m_btnSayDel.Location = new System.Drawing.Point(107, 261);
            this.m_btnSayDel.Name = "m_btnSayDel";
            this.m_btnSayDel.Size = new System.Drawing.Size(44, 23);
            this.m_btnSayDel.TabIndex = 20;
            this.m_btnSayDel.Text = "Del";
            this.m_btnSayDel.UseVisualStyleBackColor = true;
            this.m_btnSayDel.Click += new System.EventHandler(this.m_btnSayDel_Click);
            // 
            // m_btnSayModify
            // 
            this.m_btnSayModify.Location = new System.Drawing.Point(157, 261);
            this.m_btnSayModify.Name = "m_btnSayModify";
            this.m_btnSayModify.Size = new System.Drawing.Size(60, 23);
            this.m_btnSayModify.TabIndex = 21;
            this.m_btnSayModify.Text = "Modify";
            this.m_btnSayModify.UseVisualStyleBackColor = true;
            this.m_btnSayModify.Click += new System.EventHandler(this.m_btnSayModify_Click);
            // 
            // m_btnSayClear
            // 
            this.m_btnSayClear.Location = new System.Drawing.Point(223, 261);
            this.m_btnSayClear.Name = "m_btnSayClear";
            this.m_btnSayClear.Size = new System.Drawing.Size(47, 23);
            this.m_btnSayClear.TabIndex = 21;
            this.m_btnSayClear.Text = "Clear";
            this.m_btnSayClear.UseVisualStyleBackColor = true;
            this.m_btnSayClear.Click += new System.EventHandler(this.m_btnSayClear_Click);
            // 
            // m_btnSayMoveUp
            // 
            this.m_btnSayMoveUp.Location = new System.Drawing.Point(391, 261);
            this.m_btnSayMoveUp.Name = "m_btnSayMoveUp";
            this.m_btnSayMoveUp.Size = new System.Drawing.Size(27, 23);
            this.m_btnSayMoveUp.TabIndex = 22;
            this.m_btnSayMoveUp.Text = "↑";
            this.m_btnSayMoveUp.UseVisualStyleBackColor = true;
            this.m_btnSayMoveUp.Click += new System.EventHandler(this.m_btnSayMoveUp_Click);
            // 
            // m_btnSayMoveDown
            // 
            this.m_btnSayMoveDown.Location = new System.Drawing.Point(424, 261);
            this.m_btnSayMoveDown.Name = "m_btnSayMoveDown";
            this.m_btnSayMoveDown.Size = new System.Drawing.Size(27, 23);
            this.m_btnSayMoveDown.TabIndex = 23;
            this.m_btnSayMoveDown.Text = "↓";
            this.m_btnSayMoveDown.UseVisualStyleBackColor = true;
            this.m_btnSayMoveDown.Click += new System.EventHandler(this.m_btnSayMoveDown_Click);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(12, 114);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(27, 12);
            this.label11.TabIndex = 15;
            this.label11.Text = "Say";
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(314, 93);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(44, 12);
            this.label12.TabIndex = 24;
            this.label12.Text = "Hidden";
            // 
            // m_cbHidden
            // 
            this.m_cbHidden.FormattingEnabled = true;
            this.m_cbHidden.Items.AddRange(new object[] {
            "FALSE",
            "TRUE"});
            this.m_cbHidden.Location = new System.Drawing.Point(364, 87);
            this.m_cbHidden.Name = "m_cbHidden";
            this.m_cbHidden.Size = new System.Drawing.Size(96, 20);
            this.m_cbHidden.TabIndex = 25;
            // 
            // NpcOptionForm
            // 
            this.AcceptButton = this.m_btnOK;
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.m_btnCancel;
            this.ClientSize = new System.Drawing.Size(472, 347);
            this.Controls.Add(this.m_cbHidden);
            this.Controls.Add(this.label12);
            this.Controls.Add(this.m_btnSayMoveDown);
            this.Controls.Add(this.m_btnSayMoveUp);
            this.Controls.Add(this.m_btnSayClear);
            this.Controls.Add(this.m_btnSayModify);
            this.Controls.Add(this.m_btnSayDel);
            this.Controls.Add(this.m_btnSayAdd);
            this.Controls.Add(this.m_lbNpcSay);
            this.Controls.Add(this.m_cbType);
            this.Controls.Add(this.label10);
            this.Controls.Add(this.m_tbKid);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.label9);
            this.Controls.Add(this.m_btnGenerate);
            this.Controls.Add(this.label8);
            this.Controls.Add(this.m_tbGuid);
            this.Controls.Add(this.m_btnCancel);
            this.Controls.Add(this.m_btnOK);
            this.Controls.Add(this.m_tbLocZ);
            this.Controls.Add(this.m_tbLocY);
            this.Controls.Add(this.m_tbLocX);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.m_tbScript);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.m_tbActor);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.m_tbName);
            this.Controls.Add(this.label1);
            this.Name = "NpcOptionForm";
            this.Text = "Npc";
            this.ResumeLayout(false);
            this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox m_tbName;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox m_tbActor;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox m_tbScript;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.TextBox m_tbLocX;
		private System.Windows.Forms.TextBox m_tbLocY;
		private System.Windows.Forms.TextBox m_tbLocZ;
		private System.Windows.Forms.Button m_btnOK;
		private System.Windows.Forms.Button m_btnCancel;
		private System.Windows.Forms.TextBox m_tbGuid;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Button m_btnGenerate;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.TextBox m_tbKid;
		private System.Windows.Forms.Label label10;
		private System.Windows.Forms.ComboBox m_cbType;
		private System.Windows.Forms.ListBox m_lbNpcSay;
		private System.Windows.Forms.Button m_btnSayAdd;
		private System.Windows.Forms.Button m_btnSayDel;
		private System.Windows.Forms.Button m_btnSayModify;
		private System.Windows.Forms.Button m_btnSayClear;
		private System.Windows.Forms.Button m_btnSayMoveUp;
		private System.Windows.Forms.Button m_btnSayMoveDown;
		private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.ComboBox m_cbHidden;
	}
}