namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class PathPanel
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
			this.m_btnDraw = new System.Windows.Forms.Button();
			this.m_btnMakeUp = new System.Windows.Forms.Button();
			this.m_btnClear = new System.Windows.Forms.Button();
			this.m_btnMakeWall = new System.Windows.Forms.Button();
			this.m_cbFace = new System.Windows.Forms.ComboBox();
			this.m_cbType = new System.Windows.Forms.ComboBox();
			this.m_tbHeight = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.m_tbMeshGroup = new System.Windows.Forms.TextBox();
			this.label19 = new System.Windows.Forms.Label();
			this.m_btnMakeSide = new System.Windows.Forms.Button();
			this.m_btnExtract = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.m_btnExtractPathNumber = new System.Windows.Forms.Button();
			this.m_btnCancelMakeup = new System.Windows.Forms.Button();
			this.m_btnMakeStraight = new System.Windows.Forms.Button();
			this.groupBox1 = new System.Windows.Forms.GroupBox();
			this.label4 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.groupBox2 = new System.Windows.Forms.GroupBox();
			this.label5 = new System.Windows.Forms.Label();
			this.m_trbAnchorScale = new System.Windows.Forms.TrackBar();
			this.m_btnCancelMakeStraight = new System.Windows.Forms.Button();
			this.groupBox1.SuspendLayout();
			this.groupBox2.SuspendLayout();
			((System.ComponentModel.ISupportInitialize)(this.m_trbAnchorScale)).BeginInit();
			this.SuspendLayout();
			// 
			// m_btnDraw
			// 
			this.m_btnDraw.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnDraw.Location = new System.Drawing.Point(11, 20);
			this.m_btnDraw.Name = "m_btnDraw";
			this.m_btnDraw.Size = new System.Drawing.Size(75, 23);
			this.m_btnDraw.TabIndex = 18;
			this.m_btnDraw.Text = "Draw";
			this.m_btnDraw.UseVisualStyleBackColor = true;
			this.m_btnDraw.Click += new System.EventHandler(this.m_btnDraw_Click);
			// 
			// m_btnMakeUp
			// 
			this.m_btnMakeUp.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMakeUp.Location = new System.Drawing.Point(92, 49);
			this.m_btnMakeUp.Name = "m_btnMakeUp";
			this.m_btnMakeUp.Size = new System.Drawing.Size(83, 23);
			this.m_btnMakeUp.TabIndex = 41;
			this.m_btnMakeUp.Text = "Make UP";
			this.m_btnMakeUp.UseVisualStyleBackColor = true;
			this.m_btnMakeUp.Click += new System.EventHandler(this.m_btnMakeUp_Click);
			// 
			// m_btnClear
			// 
			this.m_btnClear.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnClear.Location = new System.Drawing.Point(11, 106);
			this.m_btnClear.Name = "m_btnClear";
			this.m_btnClear.Size = new System.Drawing.Size(75, 23);
			this.m_btnClear.TabIndex = 42;
			this.m_btnClear.Text = "Delete all";
			this.m_btnClear.UseVisualStyleBackColor = true;
			this.m_btnClear.Click += new System.EventHandler(this.m_btnClear_Click);
			// 
			// m_btnMakeWall
			// 
			this.m_btnMakeWall.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMakeWall.Location = new System.Drawing.Point(6, 126);
			this.m_btnMakeWall.Name = "m_btnMakeWall";
			this.m_btnMakeWall.Size = new System.Drawing.Size(260, 24);
			this.m_btnMakeWall.TabIndex = 43;
			this.m_btnMakeWall.Text = "Make";
			this.m_btnMakeWall.UseVisualStyleBackColor = true;
			this.m_btnMakeWall.Click += new System.EventHandler(this.m_btnMakeWall_Click);
			// 
			// m_cbFace
			// 
			this.m_cbFace.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.m_cbFace.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_cbFace.FormattingEnabled = true;
			this.m_cbFace.Items.AddRange(new object[] {
            "Left",
            "Right"});
			this.m_cbFace.Location = new System.Drawing.Point(104, 22);
			this.m_cbFace.Name = "m_cbFace";
			this.m_cbFace.Size = new System.Drawing.Size(162, 20);
			this.m_cbFace.TabIndex = 44;
			// 
			// m_cbType
			// 
			this.m_cbType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.m_cbType.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_cbType.FormattingEnabled = true;
			this.m_cbType.Items.AddRange(new object[] {
            "Path",
            "Path+PhysX",
            "PhysX"});
			this.m_cbType.Location = new System.Drawing.Point(104, 48);
			this.m_cbType.Name = "m_cbType";
			this.m_cbType.Size = new System.Drawing.Size(162, 20);
			this.m_cbType.TabIndex = 45;
			// 
			// m_tbHeight
			// 
			this.m_tbHeight.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.m_tbHeight.Location = new System.Drawing.Point(46, 49);
			this.m_tbHeight.Name = "m_tbHeight";
			this.m_tbHeight.Size = new System.Drawing.Size(40, 21);
			this.m_tbHeight.TabIndex = 46;
			this.m_tbHeight.Text = "500";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(8, 54);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(29, 12);
			this.label2.TabIndex = 47;
			this.label2.Text = "Scale";
			// 
			// m_tbMeshGroup
			// 
			this.m_tbMeshGroup.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.m_tbMeshGroup.Location = new System.Drawing.Point(104, 74);
			this.m_tbMeshGroup.Name = "m_tbMeshGroup";
			this.m_tbMeshGroup.Size = new System.Drawing.Size(162, 21);
			this.m_tbMeshGroup.TabIndex = 46;
			this.m_tbMeshGroup.Text = "1";
			this.m_tbMeshGroup.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
			// 
			// label19
			// 
			this.label19.AutoSize = true;
			this.label19.Location = new System.Drawing.Point(16, 76);
			this.label19.Name = "label19";
			this.label19.Size = new System.Drawing.Size(57, 12);
			this.label19.TabIndex = 47;
			this.label19.Text = "Mesh Group";
			// 
			// m_btnMakeSide
			// 
			this.m_btnMakeSide.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMakeSide.Location = new System.Drawing.Point(181, 49);
			this.m_btnMakeSide.Name = "m_btnMakeSide";
			this.m_btnMakeSide.Size = new System.Drawing.Size(84, 23);
			this.m_btnMakeSide.TabIndex = 41;
			this.m_btnMakeSide.Text = "Make Side";
			this.m_btnMakeSide.UseVisualStyleBackColor = true;
			this.m_btnMakeSide.Click += new System.EventHandler(this.m_btnMakeSide_Click);
			// 
			// m_btnExtract
			// 
			this.m_btnExtract.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnExtract.Location = new System.Drawing.Point(11, 76);
			this.m_btnExtract.Name = "m_btnExtract";
			this.m_btnExtract.Size = new System.Drawing.Size(75, 23);
			this.m_btnExtract.TabIndex = 41;
			this.m_btnExtract.Text = "Point Extraction";
			this.m_btnExtract.UseVisualStyleBackColor = true;
			this.m_btnExtract.Click += new System.EventHandler(this.m_btnExtract_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Font = new System.Drawing.Font("Gulim", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
			this.label1.Location = new System.Drawing.Point(16, 105);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(139, 13);
			this.label1.TabIndex = 48;
			this.label1.Text = "Group 3 is Monster only";
			// 
			// m_btnExtractPathNumber
			// 
			this.m_btnExtractPathNumber.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnExtractPathNumber.Location = new System.Drawing.Point(92, 20);
			this.m_btnExtractPathNumber.Name = "m_btnExtractPathNumber";
			this.m_btnExtractPathNumber.Size = new System.Drawing.Size(173, 23);
			this.m_btnExtractPathNumber.TabIndex = 51;
			this.m_btnExtractPathNumber.Text = "패스 추가";
			this.m_btnExtractPathNumber.UseVisualStyleBackColor = true;
			this.m_btnExtractPathNumber.Click += new System.EventHandler(this.m_btnExtractPathNumber_Click);
			// 
			// m_btnCancelMakeup
			// 
			this.m_btnCancelMakeup.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnCancelMakeup.Location = new System.Drawing.Point(92, 78);
			this.m_btnCancelMakeup.Name = "m_btnCancelMakeup";
			this.m_btnCancelMakeup.Size = new System.Drawing.Size(173, 23);
			this.m_btnCancelMakeup.TabIndex = 52;
			this.m_btnCancelMakeup.Text = "올리기/옆으로 취소";
			this.m_btnCancelMakeup.UseVisualStyleBackColor = true;
			this.m_btnCancelMakeup.Click += new System.EventHandler(this.m_btnCancelMakeup_Click);
			// 
			// m_btnMakeStraight
			// 
			this.m_btnMakeStraight.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMakeStraight.Location = new System.Drawing.Point(92, 106);
			this.m_btnMakeStraight.Name = "m_btnMakeStraight";
			this.m_btnMakeStraight.Size = new System.Drawing.Size(83, 23);
			this.m_btnMakeStraight.TabIndex = 53;
			this.m_btnMakeStraight.Text = "직선화";
			this.m_btnMakeStraight.UseVisualStyleBackColor = true;
			this.m_btnMakeStraight.Click += new System.EventHandler(this.m_btnMakeStraight_Click);
			// 
			// groupBox1
			// 
			this.groupBox1.Controls.Add(this.label4);
			this.groupBox1.Controls.Add(this.label3);
			this.groupBox1.Controls.Add(this.label19);
			this.groupBox1.Controls.Add(this.m_tbMeshGroup);
			this.groupBox1.Controls.Add(this.m_cbType);
			this.groupBox1.Controls.Add(this.m_cbFace);
			this.groupBox1.Controls.Add(this.label1);
			this.groupBox1.Controls.Add(this.m_btnMakeWall);
			this.groupBox1.Location = new System.Drawing.Point(12, 205);
			this.groupBox1.Name = "groupBox1";
			this.groupBox1.Size = new System.Drawing.Size(272, 157);
			this.groupBox1.TabIndex = 54;
			this.groupBox1.TabStop = false;
			this.groupBox1.Text = "Make Path";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(16, 25);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(57, 12);
			this.label4.TabIndex = 50;
			this.label4.Text = "패스 방향";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(16, 51);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(57, 12);
			this.label3.TabIndex = 49;
			this.label3.Text = "패스 타입";
			// 
			// groupBox2
			// 
			this.groupBox2.Controls.Add(this.m_btnCancelMakeStraight);
			this.groupBox2.Controls.Add(this.label5);
			this.groupBox2.Controls.Add(this.m_trbAnchorScale);
			this.groupBox2.Controls.Add(this.m_btnDraw);
			this.groupBox2.Controls.Add(this.m_btnExtractPathNumber);
			this.groupBox2.Controls.Add(this.m_btnMakeStraight);
			this.groupBox2.Controls.Add(this.m_tbHeight);
			this.groupBox2.Controls.Add(this.m_btnExtract);
			this.groupBox2.Controls.Add(this.m_btnClear);
			this.groupBox2.Controls.Add(this.m_btnCancelMakeup);
			this.groupBox2.Controls.Add(this.label2);
			this.groupBox2.Controls.Add(this.m_btnMakeUp);
			this.groupBox2.Controls.Add(this.m_btnMakeSide);
			this.groupBox2.Location = new System.Drawing.Point(12, 12);
			this.groupBox2.Name = "groupBox2";
			this.groupBox2.Size = new System.Drawing.Size(271, 187);
			this.groupBox2.TabIndex = 55;
			this.groupBox2.TabStop = false;
			this.groupBox2.Text = "Path Control";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(9, 149);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(65, 12);
			this.label5.TabIndex = 55;
			this.label5.Text = "조절자크기";
			// 
			// m_trbAnchorScale
			// 
			this.m_trbAnchorScale.Location = new System.Drawing.Point(91, 135);
			this.m_trbAnchorScale.Maximum = 100;
			this.m_trbAnchorScale.Name = "m_trbAnchorScale";
			this.m_trbAnchorScale.Size = new System.Drawing.Size(174, 45);
			this.m_trbAnchorScale.TabIndex = 54;
			this.m_trbAnchorScale.TickFrequency = 5;
			this.m_trbAnchorScale.Value = 50;
			this.m_trbAnchorScale.Scroll += new System.EventHandler(this.m_trbAnchorScale_Scroll);
			// 
			// m_btnCancelMakeStraight
			// 
			this.m_btnCancelMakeStraight.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnCancelMakeStraight.Location = new System.Drawing.Point(182, 106);
			this.m_btnCancelMakeStraight.Name = "m_btnCancelMakeStraight";
			this.m_btnCancelMakeStraight.Size = new System.Drawing.Size(83, 23);
			this.m_btnCancelMakeStraight.TabIndex = 56;
			this.m_btnCancelMakeStraight.Text = "직선화 취소";
			this.m_btnCancelMakeStraight.UseVisualStyleBackColor = true;
			this.m_btnCancelMakeStraight.Click += new System.EventHandler(this.m_btnCancelMakeStraight_Click);
			// 
			// PathPanel
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(298, 398);
			this.Controls.Add(this.groupBox2);
			this.Controls.Add(this.groupBox1);
			this.Name = "PathPanel";
			this.Text = "PathPanel";
			this.groupBox1.ResumeLayout(false);
			this.groupBox1.PerformLayout();
			this.groupBox2.ResumeLayout(false);
			this.groupBox2.PerformLayout();
			((System.ComponentModel.ISupportInitialize)(this.m_trbAnchorScale)).EndInit();
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Button m_btnDraw;
		private System.Windows.Forms.Button m_btnMakeUp;
		private System.Windows.Forms.Button m_btnClear;
		private System.Windows.Forms.Button m_btnMakeWall;
		private System.Windows.Forms.ComboBox m_cbFace;
		private System.Windows.Forms.ComboBox m_cbType;
		private System.Windows.Forms.TextBox m_tbHeight;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox m_tbMeshGroup;
		private System.Windows.Forms.Label label19;
		private System.Windows.Forms.Button m_btnMakeSide;
		private System.Windows.Forms.Button m_btnExtract;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button m_btnExtractPathNumber;
		private System.Windows.Forms.Button m_btnCancelMakeup;
		private System.Windows.Forms.Button m_btnMakeStraight;
		private System.Windows.Forms.GroupBox groupBox1;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.GroupBox groupBox2;
		private System.Windows.Forms.TrackBar m_trbAnchorScale;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Button m_btnCancelMakeStraight;
	}
}