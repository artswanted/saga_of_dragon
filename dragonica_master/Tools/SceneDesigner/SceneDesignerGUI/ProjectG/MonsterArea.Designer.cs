namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class MonsterArea
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
			this.components = new System.ComponentModel.Container();
			this.m_lbLinkArea = new System.Windows.Forms.ListBox();
			this.label8 = new System.Windows.Forms.Label();
			this.m_tbAreaNumber = new System.Windows.Forms.TextBox();
			this.label7 = new System.Windows.Forms.Label();
			this.m_btnSetValue = new System.Windows.Forms.Button();
			this.m_tbMonAreaID = new System.Windows.Forms.TextBox();
			this.m_btnExtractPoint = new System.Windows.Forms.Button();
			this.m_btnErasePoint = new System.Windows.Forms.Button();
			this.m_btnMakeCircle = new System.Windows.Forms.Button();
			this.m_btnMake = new System.Windows.Forms.Button();
			this.m_btnRevert = new System.Windows.Forms.Button();
			this.m_btnMakeDot = new System.Windows.Forms.Button();
			this.m_cmLinkArea = new System.Windows.Forms.ContextMenuStrip(this.components);
			this.addToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.modifyToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.removeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.groupBox3 = new System.Windows.Forms.GroupBox();
			this.groupBox4 = new System.Windows.Forms.GroupBox();
			this.groupBox5 = new System.Windows.Forms.GroupBox();
			this.m_cmLinkArea.SuspendLayout();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			this.groupBox3.SuspendLayout();
			this.groupBox4.SuspendLayout();
			this.groupBox5.SuspendLayout();
			this.SuspendLayout();
			// 
			// m_lbLinkArea
			// 
			this.m_lbLinkArea.FormattingEnabled = true;
			this.m_lbLinkArea.HorizontalScrollbar = true;
			this.m_lbLinkArea.ItemHeight = 12;
			this.m_lbLinkArea.Location = new System.Drawing.Point(6, 20);
			this.m_lbLinkArea.Name = "m_lbLinkArea";
			this.m_lbLinkArea.ScrollAlwaysVisible = true;
			this.m_lbLinkArea.Size = new System.Drawing.Size(252, 124);
			this.m_lbLinkArea.TabIndex = 42;
			this.m_lbLinkArea.MouseUp += new System.Windows.Forms.MouseEventHandler(this.m_lbLinkArea_MouseUp);
			// 
			// label8
			// 
			this.label8.AutoSize = true;
			this.label8.Location = new System.Drawing.Point(11, 51);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(80, 12);
			this.label8.TabIndex = 39;
			this.label8.Text = "Area Number";
			// 
			// m_tbAreaNumber
			// 
			this.m_tbAreaNumber.Location = new System.Drawing.Point(114, 48);
			this.m_tbAreaNumber.Name = "m_tbAreaNumber";
			this.m_tbAreaNumber.Size = new System.Drawing.Size(146, 21);
			this.m_tbAreaNumber.TabIndex = 37;
			this.m_tbAreaNumber.TextChanged += new System.EventHandler(this.m_tbAreaNumber_TextChanged);
			// 
			// label7
			// 
			this.label7.AutoSize = true;
			this.label7.Location = new System.Drawing.Point(16, 24);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(75, 12);
			this.label7.TabIndex = 40;
			this.label7.Text = "Mon Area ID";
			// 
			// m_btnSetValue
			// 
			this.m_btnSetValue.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnSetValue.Location = new System.Drawing.Point(6, 84);
			this.m_btnSetValue.Name = "m_btnSetValue";
			this.m_btnSetValue.Size = new System.Drawing.Size(254, 23);
			this.m_btnSetValue.TabIndex = 41;
			this.m_btnSetValue.Text = "Set Value";
			this.m_btnSetValue.UseVisualStyleBackColor = true;
			this.m_btnSetValue.Visible = false;
			// 
			// m_tbMonAreaID
			// 
			this.m_tbMonAreaID.Location = new System.Drawing.Point(114, 21);
			this.m_tbMonAreaID.Name = "m_tbMonAreaID";
			this.m_tbMonAreaID.Size = new System.Drawing.Size(146, 21);
			this.m_tbMonAreaID.TabIndex = 38;
			this.m_tbMonAreaID.TextChanged += new System.EventHandler(this.m_tbMonAreaID_TextChanged);
			// 
			// m_btnExtractPoint
			// 
			this.m_btnExtractPoint.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnExtractPoint.Location = new System.Drawing.Point(92, 20);
			this.m_btnExtractPoint.Name = "m_btnExtractPoint";
			this.m_btnExtractPoint.Size = new System.Drawing.Size(83, 23);
			this.m_btnExtractPoint.TabIndex = 32;
			this.m_btnExtractPoint.Text = "점추출";
			this.m_btnExtractPoint.UseVisualStyleBackColor = true;
			this.m_btnExtractPoint.Click += new System.EventHandler(this.m_btnExtractPoint_Click);
			// 
			// m_btnErasePoint
			// 
			this.m_btnErasePoint.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnErasePoint.Location = new System.Drawing.Point(178, 20);
			this.m_btnErasePoint.Name = "m_btnErasePoint";
			this.m_btnErasePoint.Size = new System.Drawing.Size(82, 23);
			this.m_btnErasePoint.TabIndex = 33;
			this.m_btnErasePoint.Text = "점제거";
			this.m_btnErasePoint.UseVisualStyleBackColor = true;
			this.m_btnErasePoint.Click += new System.EventHandler(this.m_btnErasePoint_Click);
			// 
			// m_btnMakeCircle
			// 
			this.m_btnMakeCircle.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMakeCircle.Location = new System.Drawing.Point(7, 20);
			this.m_btnMakeCircle.Name = "m_btnMakeCircle";
			this.m_btnMakeCircle.Size = new System.Drawing.Size(123, 23);
			this.m_btnMakeCircle.TabIndex = 30;
			this.m_btnMakeCircle.Text = "원 만들기";
			this.m_btnMakeCircle.UseVisualStyleBackColor = true;
			this.m_btnMakeCircle.Click += new System.EventHandler(this.m_btnMakeCircle_Click);
			// 
			// m_btnMake
			// 
			this.m_btnMake.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMake.Location = new System.Drawing.Point(6, 20);
			this.m_btnMake.Name = "m_btnMake";
			this.m_btnMake.Size = new System.Drawing.Size(123, 23);
			this.m_btnMake.TabIndex = 29;
			this.m_btnMake.Text = "사각 만들기";
			this.m_btnMake.UseVisualStyleBackColor = true;
			this.m_btnMake.Click += new System.EventHandler(this.m_btnMake_Click);
			// 
			// m_btnRevert
			// 
			this.m_btnRevert.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnRevert.Location = new System.Drawing.Point(137, 20);
			this.m_btnRevert.Name = "m_btnRevert";
			this.m_btnRevert.Size = new System.Drawing.Size(123, 23);
			this.m_btnRevert.TabIndex = 31;
			this.m_btnRevert.Text = "뒤집기";
			this.m_btnRevert.UseVisualStyleBackColor = true;
			this.m_btnRevert.Click += new System.EventHandler(this.m_btnRevert_Click);
			// 
			// m_btnMakeDot
			// 
			this.m_btnMakeDot.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMakeDot.Location = new System.Drawing.Point(6, 20);
			this.m_btnMakeDot.Name = "m_btnMakeDot";
			this.m_btnMakeDot.Size = new System.Drawing.Size(83, 23);
			this.m_btnMakeDot.TabIndex = 28;
			this.m_btnMakeDot.Text = "점찍기";
			this.m_btnMakeDot.UseVisualStyleBackColor = true;
			this.m_btnMakeDot.Click += new System.EventHandler(this.m_btnMakeDot_Click);
			// 
			// m_cmLinkArea
			// 
			this.m_cmLinkArea.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addToolStripMenuItem,
            this.modifyToolStripMenuItem,
            this.removeToolStripMenuItem});
			this.m_cmLinkArea.Name = "m_cmLinkArea";
			this.m_cmLinkArea.Size = new System.Drawing.Size(123, 70);
			// 
			// addToolStripMenuItem
			// 
			this.addToolStripMenuItem.Name = "addToolStripMenuItem";
			this.addToolStripMenuItem.Size = new System.Drawing.Size(122, 22);
			this.addToolStripMenuItem.Text = "Add";
			this.addToolStripMenuItem.MouseUp += new System.Windows.Forms.MouseEventHandler(this.addToolStripMenuItem_MouseUp);
			// 
			// modifyToolStripMenuItem
			// 
			this.modifyToolStripMenuItem.Name = "modifyToolStripMenuItem";
			this.modifyToolStripMenuItem.Size = new System.Drawing.Size(122, 22);
			this.modifyToolStripMenuItem.Text = "Modify";
			this.modifyToolStripMenuItem.MouseUp += new System.Windows.Forms.MouseEventHandler(this.modifyToolStripMenuItem_MouseUp);
			// 
			// removeToolStripMenuItem
			// 
			this.removeToolStripMenuItem.Name = "removeToolStripMenuItem";
			this.removeToolStripMenuItem.Size = new System.Drawing.Size(122, 22);
			this.removeToolStripMenuItem.Text = "Remove";
			this.removeToolStripMenuItem.MouseUp += new System.Windows.Forms.MouseEventHandler(this.removeToolStripMenuItem_MouseUp);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label8);
			this.groupBox1.Controls.Add(this.m_tbAreaNumber);
			this.groupBox1.Controls.Add(this.label7);
			this.groupBox1.Controls.Add(this.m_tbMonAreaID);
			this.groupBox1.Controls.Add(this.m_btnSetValue);
			this.groupBox1.Location = new System.Drawing.Point(12, 12);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(268, 122);
			this.groupBox1.TabIndex = 44;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Area Setting";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.m_btnMakeDot);
			this.groupBox2.Controls.Add(this.m_btnExtractPoint);
			this.groupBox2.Controls.Add(this.m_btnErasePoint);
			this.groupBox2.Location = new System.Drawing.Point(12, 140);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(267, 65);
			this.groupBox2.TabIndex = 45;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Point";
			// 
			// groupBox3
			// 
			this.groupBox3.Controls.Add(this.m_btnMake);
			this.groupBox3.Controls.Add(this.m_btnRevert);
			this.groupBox3.Location = new System.Drawing.Point(12, 211);
			this.groupBox3.Name = "groupBox3";
			this.groupBox3.Size = new System.Drawing.Size(266, 64);
			this.groupBox3.TabIndex = 46;
			this.groupBox3.TabStop = false;
			this.groupBox3.Text = "Rectangle";
			// 
			// groupBox4
			// 
			this.groupBox4.Controls.Add(this.m_btnMakeCircle);
			this.groupBox4.Location = new System.Drawing.Point(12, 281);
			this.groupBox4.Name = "groupBox4";
			this.groupBox4.Size = new System.Drawing.Size(266, 55);
			this.groupBox4.TabIndex = 47;
			this.groupBox4.TabStop = false;
			this.groupBox4.Text = "Circle";
			// 
			// groupBox5
			// 
			this.groupBox5.Controls.Add(this.m_lbLinkArea);
			this.groupBox5.Location = new System.Drawing.Point(12, 342);
			this.groupBox5.Name = "groupBox5";
			this.groupBox5.Size = new System.Drawing.Size(266, 156);
			this.groupBox5.TabIndex = 48;
			this.groupBox5.TabStop = false;
			this.groupBox5.Text = "Link Area";
			// 
			// MonsterArea
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoScroll = true;
			this.AutoScrollMargin = new System.Drawing.Size(10, 10);
			this.ClientSize = new System.Drawing.Size(299, 513);
			this.Controls.Add(this.groupBox5);
			this.Controls.Add(this.groupBox4);
			this.Controls.Add(this.groupBox3);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.Name = "MonsterArea";
			this.Text = "MonsterArea";
			this.m_cmLinkArea.ResumeLayout(false);
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox3.ResumeLayout(false);
			this.groupBox4.ResumeLayout(false);
			this.groupBox5.ResumeLayout(false);
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.ListBox m_lbLinkArea;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.TextBox m_tbAreaNumber;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Button m_btnSetValue;
		private System.Windows.Forms.TextBox m_tbMonAreaID;
		private System.Windows.Forms.Button m_btnExtractPoint;
		private System.Windows.Forms.Button m_btnErasePoint;
		private System.Windows.Forms.Button m_btnMakeCircle;
		private System.Windows.Forms.Button m_btnMake;
		private System.Windows.Forms.Button m_btnRevert;
		private System.Windows.Forms.Button m_btnMakeDot;
		private System.Windows.Forms.ContextMenuStrip m_cmLinkArea;
		private System.Windows.Forms.ToolStripMenuItem addToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem modifyToolStripMenuItem;
		private System.Windows.Forms.ToolStripMenuItem removeToolStripMenuItem;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.GroupBox groupBox3;
		private System.Windows.Forms.GroupBox groupBox4;
		private System.Windows.Forms.GroupBox groupBox5;

	}
}