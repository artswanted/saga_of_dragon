namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class RainDataDialog
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
			this.m_btnOpen = new System.Windows.Forms.Button();
			this.m_tbRadian = new System.Windows.Forms.TextBox();
			this.m_btnOK = new System.Windows.Forms.Button();
			this.m_btnCancel = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.m_lblAngle = new System.Windows.Forms.Label();
			this.m_lbIgnoreList = new System.Windows.Forms.ListBox();
			this.label3 = new System.Windows.Forms.Label();
			this.m_btnIgnoreAdd = new System.Windows.Forms.Button();
			this.m_btnIgnoreModify = new System.Windows.Forms.Button();
			this.m_btnIgnoreDelete = new System.Windows.Forms.Button();
			this.m_rbBinary = new System.Windows.Forms.RadioButton();
			this.m_rbText = new System.Windows.Forms.RadioButton();
			this.SuspendLayout();
			// 
			// m_tbFilename
			// 
			this.m_tbFilename.Enabled = false;
			this.m_tbFilename.Location = new System.Drawing.Point(12, 31);
			this.m_tbFilename.Name = "m_tbFilename";
			this.m_tbFilename.Size = new System.Drawing.Size(305, 21);
			this.m_tbFilename.TabIndex = 0;
			// 
			// m_btnOpen
			// 
			this.m_btnOpen.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnOpen.Location = new System.Drawing.Point(323, 29);
			this.m_btnOpen.Name = "m_btnOpen";
			this.m_btnOpen.Size = new System.Drawing.Size(29, 23);
			this.m_btnOpen.TabIndex = 1;
			this.m_btnOpen.Text = "...";
			this.m_btnOpen.UseVisualStyleBackColor = true;
			this.m_btnOpen.Click += new System.EventHandler(this.m_btnOpen_Click);
			// 
			// m_tbRadian
			// 
			this.m_tbRadian.Location = new System.Drawing.Point(12, 82);
			this.m_tbRadian.Name = "m_tbRadian";
			this.m_tbRadian.Size = new System.Drawing.Size(100, 21);
			this.m_tbRadian.TabIndex = 2;
			this.m_tbRadian.TextChanged += new System.EventHandler(this.m_tbRadian_TextChanged);
			// 
			// m_btnOK
			// 
			this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnOK.Location = new System.Drawing.Point(76, 138);
			this.m_btnOK.Name = "m_btnOK";
			this.m_btnOK.Size = new System.Drawing.Size(75, 23);
			this.m_btnOK.TabIndex = 16;
			this.m_btnOK.Text = "확인";
			this.m_btnOK.UseVisualStyleBackColor = true;
			this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
			// 
			// m_btnCancel
			// 
			this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnCancel.Location = new System.Drawing.Point(205, 138);
			this.m_btnCancel.Name = "m_btnCancel";
			this.m_btnCancel.Size = new System.Drawing.Size(75, 23);
			this.m_btnCancel.TabIndex = 17;
			this.m_btnCancel.Text = "취소";
			this.m_btnCancel.UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(12, 9);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(29, 12);
			this.label1.TabIndex = 5;
			this.label1.Text = "파일";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(12, 62);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(136, 12);
			this.label2.TabIndex = 6;
			this.label2.Text = "내적 값 (범위 0.0 ~ 1.0)";
			// 
			// m_lblAngle
			// 
			this.m_lblAngle.AutoSize = true;
			this.m_lblAngle.Location = new System.Drawing.Point(118, 85);
			this.m_lblAngle.Name = "m_lblAngle";
			this.m_lblAngle.Size = new System.Drawing.Size(47, 12);
			this.m_lblAngle.TabIndex = 7;
			this.m_lblAngle.Text = "각도: 0˚";
			// 
			// m_lbIgnoreList
			// 
			this.m_lbIgnoreList.FormattingEnabled = true;
			this.m_lbIgnoreList.IntegralHeight = false;
			this.m_lbIgnoreList.ItemHeight = 12;
			this.m_lbIgnoreList.Location = new System.Drawing.Point(374, 29);
			this.m_lbIgnoreList.Name = "m_lbIgnoreList";
			this.m_lbIgnoreList.Size = new System.Drawing.Size(260, 107);
			this.m_lbIgnoreList.TabIndex = 12;
			this.m_lbIgnoreList.DoubleClick += new System.EventHandler(this.m_lbIgnoreList_DoubleClick);
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(372, 9);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(85, 12);
			this.label3.TabIndex = 9;
			this.label3.Text = "생략할 노드들.";
			// 
			// m_btnIgnoreAdd
			// 
			this.m_btnIgnoreAdd.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnIgnoreAdd.Location = new System.Drawing.Point(382, 142);
			this.m_btnIgnoreAdd.Name = "m_btnIgnoreAdd";
			this.m_btnIgnoreAdd.Size = new System.Drawing.Size(75, 23);
			this.m_btnIgnoreAdd.TabIndex = 13;
			this.m_btnIgnoreAdd.Text = "추가";
			this.m_btnIgnoreAdd.UseVisualStyleBackColor = true;
			this.m_btnIgnoreAdd.Click += new System.EventHandler(this.m_btnIgnoreAdd_Click);
			// 
			// m_btnIgnoreModify
			// 
			this.m_btnIgnoreModify.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnIgnoreModify.Location = new System.Drawing.Point(463, 142);
			this.m_btnIgnoreModify.Name = "m_btnIgnoreModify";
			this.m_btnIgnoreModify.Size = new System.Drawing.Size(75, 23);
			this.m_btnIgnoreModify.TabIndex = 14;
			this.m_btnIgnoreModify.Text = "변경";
			this.m_btnIgnoreModify.UseVisualStyleBackColor = true;
			this.m_btnIgnoreModify.Click += new System.EventHandler(this.m_btnIgnoreModify_Click);
			// 
			// m_btnIgnoreDelete
			// 
			this.m_btnIgnoreDelete.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnIgnoreDelete.Location = new System.Drawing.Point(544, 142);
			this.m_btnIgnoreDelete.Name = "m_btnIgnoreDelete";
			this.m_btnIgnoreDelete.Size = new System.Drawing.Size(75, 23);
			this.m_btnIgnoreDelete.TabIndex = 15;
			this.m_btnIgnoreDelete.Text = "삭제";
			this.m_btnIgnoreDelete.UseVisualStyleBackColor = true;
			this.m_btnIgnoreDelete.Click += new System.EventHandler(this.m_btnIgnoreDelete_Click);
			// 
			// m_rbBinary
			// 
			this.m_rbBinary.AutoSize = true;
			this.m_rbBinary.Checked = true;
			this.m_rbBinary.Location = new System.Drawing.Point(12, 116);
			this.m_rbBinary.Name = "m_rbBinary";
			this.m_rbBinary.Size = new System.Drawing.Size(75, 16);
			this.m_rbBinary.TabIndex = 10;
			this.m_rbBinary.TabStop = true;
			this.m_rbBinary.Text = "이진 출력";
			this.m_rbBinary.UseVisualStyleBackColor = true;
			// 
			// m_rbText
			// 
			this.m_rbText.AutoSize = true;
			this.m_rbText.Location = new System.Drawing.Point(120, 116);
			this.m_rbText.Name = "m_rbText";
			this.m_rbText.Size = new System.Drawing.Size(87, 16);
			this.m_rbText.TabIndex = 11;
			this.m_rbText.Text = "텍스트 출력";
			this.m_rbText.UseVisualStyleBackColor = true;
			// 
			// RainDataDialog
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(646, 173);
			this.Controls.Add(this.m_rbText);
			this.Controls.Add(this.m_rbBinary);
			this.Controls.Add(this.m_btnIgnoreDelete);
			this.Controls.Add(this.m_btnIgnoreModify);
			this.Controls.Add(this.m_btnIgnoreAdd);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.m_lbIgnoreList);
			this.Controls.Add(this.m_lblAngle);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.m_btnCancel);
			this.Controls.Add(this.m_btnOK);
			this.Controls.Add(this.m_tbRadian);
			this.Controls.Add(this.m_btnOpen);
			this.Controls.Add(this.m_tbFilename);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
			this.Name = "RainDataDialog";
			this.Text = "RainDataDialog";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.TextBox m_tbFilename;
		private System.Windows.Forms.Button m_btnOpen;
		private System.Windows.Forms.TextBox m_tbRadian;
		private System.Windows.Forms.Button m_btnOK;
		private System.Windows.Forms.Button m_btnCancel;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label m_lblAngle;
		private System.Windows.Forms.ListBox m_lbIgnoreList;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Button m_btnIgnoreAdd;
		private System.Windows.Forms.Button m_btnIgnoreModify;
		private System.Windows.Forms.Button m_btnIgnoreDelete;
		private System.Windows.Forms.RadioButton m_rbBinary;
		private System.Windows.Forms.RadioButton m_rbText;
	}
}