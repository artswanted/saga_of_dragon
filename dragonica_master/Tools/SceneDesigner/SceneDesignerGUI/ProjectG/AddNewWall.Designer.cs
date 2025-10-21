namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class AddNewWall
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
			this.m_tbFilename = new System.Windows.Forms.TextBox();
			this.label1 = new System.Windows.Forms.Label();
			this.m_btnOpen = new System.Windows.Forms.Button();
			this.m_cbbPalette = new System.Windows.Forms.ComboBox();
			this.label2 = new System.Windows.Forms.Label();
			this.m_btnOK = new System.Windows.Forms.Button();
			this.m_btnCancel = new System.Windows.Forms.Button();
			this.label3 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// m_tbFilename
			// 
			this.m_tbFilename.Enabled = false;
			this.m_tbFilename.Location = new System.Drawing.Point(12, 27);
			this.m_tbFilename.Name = "m_tbFilename";
			this.m_tbFilename.Size = new System.Drawing.Size(375, 21);
			this.m_tbFilename.TabIndex = 0;
			this.m_tbFilename.Text = "C:\\path.nif";
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(12, 9);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(61, 12);
			this.label1.TabIndex = 1;
			this.label1.Text = "File name";
			// 
			// m_btnOpen
			// 
			this.m_btnOpen.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnOpen.Location = new System.Drawing.Point(386, 27);
			this.m_btnOpen.Name = "m_btnOpen";
			this.m_btnOpen.Size = new System.Drawing.Size(39, 21);
			this.m_btnOpen.TabIndex = 2;
			this.m_btnOpen.Text = "...";
			this.m_btnOpen.UseVisualStyleBackColor = true;
			this.m_btnOpen.Click += new System.EventHandler(this.m_btnOpen_Click);
			// 
			// m_cbbPalette
			// 
			this.m_cbbPalette.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.m_cbbPalette.FormattingEnabled = true;
			this.m_cbbPalette.Location = new System.Drawing.Point(12, 83);
			this.m_cbbPalette.Name = "m_cbbPalette";
			this.m_cbbPalette.Size = new System.Drawing.Size(251, 20);
			this.m_cbbPalette.TabIndex = 3;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(10, 66);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(43, 12);
			this.label2.TabIndex = 4;
			this.label2.Text = "Palette";
			// 
			// m_btnOK
			// 
			this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnOK.Location = new System.Drawing.Point(269, 81);
			this.m_btnOK.Name = "m_btnOK";
			this.m_btnOK.Size = new System.Drawing.Size(75, 23);
			this.m_btnOK.TabIndex = 5;
			this.m_btnOK.Text = "OK";
			this.m_btnOK.UseVisualStyleBackColor = true;
			this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
			// 
			// m_btnCancel
			// 
			this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnCancel.Location = new System.Drawing.Point(350, 81);
			this.m_btnCancel.Name = "m_btnCancel";
			this.m_btnCancel.Size = new System.Drawing.Size(75, 23);
			this.m_btnCancel.TabIndex = 6;
			this.m_btnCancel.Text = "Cancel";
			this.m_btnCancel.UseVisualStyleBackColor = true;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(10, 109);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(303, 12);
			this.label3.TabIndex = 7;
			this.label3.Text = "\'_None_\' 를 선택 하시면 팔레트에 추가 되지 않습니다.";
			// 
			// AddNewWall
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(437, 133);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.m_btnCancel);
			this.Controls.Add(this.m_btnOK);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.m_cbbPalette);
			this.Controls.Add(this.m_btnOpen);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.m_tbFilename);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "AddNewWall";
			this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
			this.Text = "AddNewWall";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TextBox m_tbFilename;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button m_btnOpen;
		private System.Windows.Forms.ComboBox m_cbbPalette;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button m_btnOK;
		private System.Windows.Forms.Button m_btnCancel;
		private System.Windows.Forms.Label label3;
	}
}