namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class NotExistFilesPanel
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
			this.m_lbNotExistFilesList = new System.Windows.Forms.ListBox();
			this.m_btnRefresh = new System.Windows.Forms.Button();
			this.m_lbNotExistFilesList2 = new System.Windows.Forms.ListBox();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.m_btnRemove2 = new System.Windows.Forms.Button();
			this.m_btnRemove = new System.Windows.Forms.Button();
			this.m_btnRemoveOne2 = new System.Windows.Forms.Button();
			this.m_btnRemoveOne = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// m_lbNotExistFilesList
			// 
			this.m_lbNotExistFilesList.FormattingEnabled = true;
			this.m_lbNotExistFilesList.HorizontalScrollbar = true;
			this.m_lbNotExistFilesList.ItemHeight = 12;
			this.m_lbNotExistFilesList.Location = new System.Drawing.Point(10, 181);
			this.m_lbNotExistFilesList.Name = "m_lbNotExistFilesList";
			this.m_lbNotExistFilesList.ScrollAlwaysVisible = true;
			this.m_lbNotExistFilesList.Size = new System.Drawing.Size(332, 112);
			this.m_lbNotExistFilesList.TabIndex = 0;
			this.m_lbNotExistFilesList.DoubleClick += new System.EventHandler(this.m_lbNotExistFilesList_DoubleClick);
			// 
			// m_btnRefresh
			// 
			this.m_btnRefresh.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnRefresh.Location = new System.Drawing.Point(10, 299);
			this.m_btnRefresh.Name = "m_btnRefresh";
			this.m_btnRefresh.Size = new System.Drawing.Size(332, 25);
			this.m_btnRefresh.TabIndex = 1;
			this.m_btnRefresh.Text = "Refresh";
			this.m_btnRefresh.UseVisualStyleBackColor = true;
			this.m_btnRefresh.Click += new System.EventHandler(this.m_btnRefresh_Click);
			// 
			// m_lbNotExistFilesList2
			// 
			this.m_lbNotExistFilesList2.FormattingEnabled = true;
			this.m_lbNotExistFilesList2.HorizontalScrollbar = true;
			this.m_lbNotExistFilesList2.ItemHeight = 12;
			this.m_lbNotExistFilesList2.Location = new System.Drawing.Point(12, 36);
			this.m_lbNotExistFilesList2.Name = "m_lbNotExistFilesList2";
			this.m_lbNotExistFilesList2.ScrollAlwaysVisible = true;
			this.m_lbNotExistFilesList2.Size = new System.Drawing.Size(330, 112);
			this.m_lbNotExistFilesList2.TabIndex = 0;
			this.m_lbNotExistFilesList2.DoubleClick += new System.EventHandler(this.m_lbNotExistFilesList2_DoubleClick);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(10, 166);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(109, 12);
			this.label1.TabIndex = 2;
			this.label1.Text = "찍혔지만 없는 물체";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(12, 21);
			this.label2.Name = "label2";
			this.label2.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.label2.Size = new System.Drawing.Size(109, 12);
			this.label2.TabIndex = 2;
			this.label2.Text = "팔레트에 없는 물체";
			// 
			// m_btnRemove2
			// 
			this.m_btnRemove2.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnRemove2.Location = new System.Drawing.Point(256, 13);
			this.m_btnRemove2.Name = "m_btnRemove2";
			this.m_btnRemove2.Size = new System.Drawing.Size(86, 17);
			this.m_btnRemove2.TabIndex = 1;
			this.m_btnRemove2.Text = "RemoveAll";
			this.m_btnRemove2.UseVisualStyleBackColor = true;
			this.m_btnRemove2.Click += new System.EventHandler(this.m_btnRemove2_Click);
			// 
			// m_btnRemove
			// 
			this.m_btnRemove.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnRemove.Location = new System.Drawing.Point(256, 158);
			this.m_btnRemove.Name = "m_btnRemove";
			this.m_btnRemove.Size = new System.Drawing.Size(86, 17);
			this.m_btnRemove.TabIndex = 1;
			this.m_btnRemove.Text = "RemoveAll";
			this.m_btnRemove.UseVisualStyleBackColor = true;
			this.m_btnRemove.Click += new System.EventHandler(this.m_btnRemove_Click);
			// 
			// m_btnRemoveOne2
			// 
			this.m_btnRemoveOne2.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnRemoveOne2.Location = new System.Drawing.Point(164, 13);
			this.m_btnRemoveOne2.Name = "m_btnRemoveOne2";
			this.m_btnRemoveOne2.Size = new System.Drawing.Size(86, 17);
			this.m_btnRemoveOne2.TabIndex = 1;
			this.m_btnRemoveOne2.Text = "RemoveOne";
			this.m_btnRemoveOne2.UseVisualStyleBackColor = true;
			this.m_btnRemoveOne2.Click += new System.EventHandler(this.m_btnRemoveOne2_Click);
			// 
			// m_btnRemoveOne
			// 
			this.m_btnRemoveOne.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnRemoveOne.Location = new System.Drawing.Point(164, 158);
			this.m_btnRemoveOne.Name = "m_btnRemoveOne";
			this.m_btnRemoveOne.Size = new System.Drawing.Size(86, 17);
			this.m_btnRemoveOne.TabIndex = 1;
			this.m_btnRemoveOne.Text = "RemoveOne";
			this.m_btnRemoveOne.UseVisualStyleBackColor = true;
			this.m_btnRemoveOne.Click += new System.EventHandler(this.m_btnRemoveOne_Click);
			// 
			// NotExistFilesPanel
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(355, 347);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.m_btnRemoveOne);
			this.Controls.Add(this.m_btnRemove);
			this.Controls.Add(this.m_btnRemoveOne2);
			this.Controls.Add(this.m_btnRemove2);
			this.Controls.Add(this.m_btnRefresh);
			this.Controls.Add(this.m_lbNotExistFilesList2);
			this.Controls.Add(this.m_lbNotExistFilesList);
			this.Name = "NotExistFilesPanel";
			this.Text = "Missing File List";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.ListBox m_lbNotExistFilesList;
		private System.Windows.Forms.Button m_btnRefresh;
		private System.Windows.Forms.ListBox m_lbNotExistFilesList2;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Button m_btnRemove2;
		private System.Windows.Forms.Button m_btnRemove;
		private System.Windows.Forms.Button m_btnRemoveOne2;
		private System.Windows.Forms.Button m_btnRemoveOne;
	}
}