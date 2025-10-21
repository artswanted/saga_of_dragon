namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class MonsterPanelSetting
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
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.m_tbServer = new System.Windows.Forms.TextBox();
			this.m_tbID = new System.Windows.Forms.TextBox();
			this.m_tbPassword = new System.Windows.Forms.TextBox();
			this.m_tbDatabase = new System.Windows.Forms.TextBox();
			this.m_btnOK = new System.Windows.Forms.Button();
			this.m_btnCancel = new System.Windows.Forms.Button();
			this.label5 = new System.Windows.Forms.Label();
			this.m_tbMonsterTable = new System.Windows.Forms.TextBox();
			this.label6 = new System.Windows.Forms.Label();
			this.m_tbRegenTable = new System.Windows.Forms.TextBox();
			this.m_tbBagControl = new System.Windows.Forms.TextBox();
			this.label7 = new System.Windows.Forms.Label();
			this.m_tbObjectBag = new System.Windows.Forms.TextBox();
			this.label8 = new System.Windows.Forms.Label();
			this.m_tbObjectElement = new System.Windows.Forms.TextBox();
			this.label9 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(20, 20);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(41, 12);
			this.label1.TabIndex = 0;
			this.label1.Text = "Server";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(20, 47);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(16, 12);
			this.label2.TabIndex = 1;
			this.label2.Text = "ID";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(20, 74);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(62, 12);
			this.label3.TabIndex = 2;
			this.label3.Text = "Password";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(20, 101);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(58, 12);
			this.label4.TabIndex = 3;
			this.label4.Text = "Database";
			// 
			// m_tbServer
			// 
			this.m_tbServer.Location = new System.Drawing.Point(113, 17);
			this.m_tbServer.Name = "m_tbServer";
			this.m_tbServer.Size = new System.Drawing.Size(179, 21);
			this.m_tbServer.TabIndex = 4;
			// 
			// m_tbID
			// 
			this.m_tbID.Location = new System.Drawing.Point(113, 44);
			this.m_tbID.Name = "m_tbID";
			this.m_tbID.Size = new System.Drawing.Size(179, 21);
			this.m_tbID.TabIndex = 5;
			// 
			// m_tbPassword
			// 
			this.m_tbPassword.Location = new System.Drawing.Point(113, 71);
			this.m_tbPassword.Name = "m_tbPassword";
			this.m_tbPassword.Size = new System.Drawing.Size(179, 21);
			this.m_tbPassword.TabIndex = 6;
			this.m_tbPassword.UseSystemPasswordChar = true;
			// 
			// m_tbDatabase
			// 
			this.m_tbDatabase.Location = new System.Drawing.Point(113, 98);
			this.m_tbDatabase.Name = "m_tbDatabase";
			this.m_tbDatabase.Size = new System.Drawing.Size(179, 21);
			this.m_tbDatabase.TabIndex = 7;
			// 
			// m_btnOK
			// 
			this.m_btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnOK.Location = new System.Drawing.Point(62, 260);
			this.m_btnOK.Name = "m_btnOK";
			this.m_btnOK.Size = new System.Drawing.Size(75, 23);
			this.m_btnOK.TabIndex = 8;
			this.m_btnOK.Text = "확인";
			this.m_btnOK.UseVisualStyleBackColor = true;
			this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
			// 
			// m_btnCancel
			// 
			this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnCancel.Location = new System.Drawing.Point(166, 260);
			this.m_btnCancel.Name = "m_btnCancel";
			this.m_btnCancel.Size = new System.Drawing.Size(75, 23);
			this.m_btnCancel.TabIndex = 9;
			this.m_btnCancel.Text = "취소";
			this.m_btnCancel.UseVisualStyleBackColor = true;
			this.m_btnCancel.Click += new System.EventHandler(this.m_btnCancel_Click);
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(20, 128);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(87, 12);
			this.label5.TabIndex = 10;
			this.label5.Text = "Monster Table";
			// 
			// m_tbMonsterTable
			// 
			this.m_tbMonsterTable.Location = new System.Drawing.Point(113, 125);
			this.m_tbMonsterTable.Name = "m_tbMonsterTable";
			this.m_tbMonsterTable.Size = new System.Drawing.Size(179, 21);
			this.m_tbMonsterTable.TabIndex = 11;
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(20, 153);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(77, 12);
			this.label6.TabIndex = 12;
			this.label6.Text = "Regen Table";
			// 
			// m_tbRegenTable
			// 
			this.m_tbRegenTable.Location = new System.Drawing.Point(113, 152);
			this.m_tbRegenTable.Name = "m_tbRegenTable";
			this.m_tbRegenTable.Size = new System.Drawing.Size(179, 21);
			this.m_tbRegenTable.TabIndex = 13;
			// 
			// m_tbBagControl
			// 
			this.m_tbBagControl.Location = new System.Drawing.Point(114, 179);
			this.m_tbBagControl.Name = "m_tbBagControl";
			this.m_tbBagControl.Size = new System.Drawing.Size(178, 21);
			this.m_tbBagControl.TabIndex = 14;
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(20, 182);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(71, 12);
			this.label7.TabIndex = 15;
			this.label7.Text = "Bag Control";
			// 
			// m_tbObjectBag
			// 
			this.m_tbObjectBag.Location = new System.Drawing.Point(114, 206);
			this.m_tbObjectBag.Name = "m_tbObjectBag";
			this.m_tbObjectBag.Size = new System.Drawing.Size(178, 21);
			this.m_tbObjectBag.TabIndex = 16;
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(20, 209);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(67, 12);
			this.label8.TabIndex = 17;
			this.label8.Text = "Object Bag";
			// 
			// m_tbObjectElement
			// 
			this.m_tbObjectElement.Location = new System.Drawing.Point(113, 233);
			this.m_tbObjectElement.Name = "m_tbObjectElement";
			this.m_tbObjectElement.Size = new System.Drawing.Size(179, 21);
			this.m_tbObjectElement.TabIndex = 18;
			// 
			// label9
			// 
			this.label9.AutoSize = true;
			this.label9.Location = new System.Drawing.Point(20, 236);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(91, 12);
			this.label9.TabIndex = 19;
			this.label9.Text = "Object Element";
			// 
			// MonsterPanelSetting
			// 
			this.AcceptButton = this.m_btnOK;
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.m_btnCancel;
			this.ClientSize = new System.Drawing.Size(304, 300);
			this.Controls.Add(this.label9);
			this.Controls.Add(this.m_tbObjectElement);
			this.Controls.Add(this.label8);
			this.Controls.Add(this.m_tbObjectBag);
			this.Controls.Add(this.label7);
			this.Controls.Add(this.m_tbBagControl);
			this.Controls.Add(this.m_tbRegenTable);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.m_tbMonsterTable);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.m_btnCancel);
			this.Controls.Add(this.m_btnOK);
			this.Controls.Add(this.m_tbDatabase);
			this.Controls.Add(this.m_tbPassword);
			this.Controls.Add(this.m_tbID);
			this.Controls.Add(this.m_tbServer);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "MonsterPanelSetting";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "MonsterPanelSetting";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox m_tbServer;
		private System.Windows.Forms.TextBox m_tbID;
		private System.Windows.Forms.TextBox m_tbPassword;
		private System.Windows.Forms.TextBox m_tbDatabase;
		private System.Windows.Forms.Button m_btnOK;
		private System.Windows.Forms.Button m_btnCancel;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.TextBox m_tbMonsterTable;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.TextBox m_tbRegenTable;
        private System.Windows.Forms.TextBox m_tbBagControl;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox m_tbObjectBag;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox m_tbObjectElement;
        private System.Windows.Forms.Label label9;
	}
}