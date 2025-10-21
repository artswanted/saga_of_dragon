namespace Emergent.Gamebryo.SceneDesigner.GUI
{
	partial class NpcPanel
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
			this.m_btnModify = new System.Windows.Forms.Button();
			this.m_btnCopy = new System.Windows.Forms.Button();
			this.m_btnMoveDown = new System.Windows.Forms.Button();
			this.m_btnMoveUp = new System.Windows.Forms.Button();
			this.m_btnClear = new System.Windows.Forms.Button();
			this.m_btnDelete = new System.Windows.Forms.Button();
			this.m_btnAdd = new System.Windows.Forms.Button();
			this.m_lbNpcSets = new System.Windows.Forms.ListBox();
			this.SuspendLayout();
			// 
			// m_btnModify
			// 
			this.m_btnModify.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnModify.Location = new System.Drawing.Point(28, 434);
			this.m_btnModify.Name = "m_btnModify";
			this.m_btnModify.Size = new System.Drawing.Size(55, 23);
			this.m_btnModify.TabIndex = 12;
			this.m_btnModify.Text = "Modify";
			this.m_btnModify.UseVisualStyleBackColor = true;
			this.m_btnModify.Click += new System.EventHandler(this.m_btnModify_Click);
			// 
			// m_btnCopy
			// 
			this.m_btnCopy.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnCopy.Location = new System.Drawing.Point(150, 405);
			this.m_btnCopy.Name = "m_btnCopy";
			this.m_btnCopy.Size = new System.Drawing.Size(55, 23);
			this.m_btnCopy.TabIndex = 11;
			this.m_btnCopy.Text = "Copy";
			this.m_btnCopy.UseVisualStyleBackColor = true;
			this.m_btnCopy.Click += new System.EventHandler(this.m_btnCopy_Click);
			// 
			// m_btnMoveDown
			// 
			this.m_btnMoveDown.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMoveDown.Location = new System.Drawing.Point(119, 434);
			this.m_btnMoveDown.Name = "m_btnMoveDown";
			this.m_btnMoveDown.Size = new System.Drawing.Size(25, 23);
			this.m_btnMoveDown.TabIndex = 14;
			this.m_btnMoveDown.Text = "↓";
			this.m_btnMoveDown.UseVisualStyleBackColor = true;
			this.m_btnMoveDown.Click += new System.EventHandler(this.m_btnMoveDown_Click);
			// 
			// m_btnMoveUp
			// 
			this.m_btnMoveUp.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMoveUp.Location = new System.Drawing.Point(89, 434);
			this.m_btnMoveUp.Name = "m_btnMoveUp";
			this.m_btnMoveUp.Size = new System.Drawing.Size(25, 23);
			this.m_btnMoveUp.TabIndex = 13;
			this.m_btnMoveUp.Text = "↑";
			this.m_btnMoveUp.UseVisualStyleBackColor = true;
			this.m_btnMoveUp.Click += new System.EventHandler(this.m_btnMoveUp_Click);
			// 
			// m_btnClear
			// 
			this.m_btnClear.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnClear.Location = new System.Drawing.Point(150, 434);
			this.m_btnClear.Name = "m_btnClear";
			this.m_btnClear.Size = new System.Drawing.Size(55, 23);
			this.m_btnClear.TabIndex = 15;
			this.m_btnClear.Text = "Clear";
			this.m_btnClear.UseVisualStyleBackColor = true;
			this.m_btnClear.Click += new System.EventHandler(this.m_btnClear_Click);
			// 
			// m_btnDelete
			// 
			this.m_btnDelete.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnDelete.Location = new System.Drawing.Point(89, 405);
			this.m_btnDelete.Name = "m_btnDelete";
			this.m_btnDelete.Size = new System.Drawing.Size(55, 23);
			this.m_btnDelete.TabIndex = 10;
			this.m_btnDelete.Text = "Delete";
			this.m_btnDelete.UseVisualStyleBackColor = true;
			this.m_btnDelete.Click += new System.EventHandler(this.m_btnDelete_Click);
			// 
			// m_btnAdd
			// 
			this.m_btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnAdd.Location = new System.Drawing.Point(28, 405);
			this.m_btnAdd.Name = "m_btnAdd";
			this.m_btnAdd.Size = new System.Drawing.Size(55, 23);
			this.m_btnAdd.TabIndex = 9;
			this.m_btnAdd.Text = "Add";
			this.m_btnAdd.UseVisualStyleBackColor = true;
			this.m_btnAdd.Click += new System.EventHandler(this.m_btnAdd_Click);
			// 
			// m_lbNpcSets
			// 
			this.m_lbNpcSets.FormattingEnabled = true;
			this.m_lbNpcSets.HorizontalScrollbar = true;
			this.m_lbNpcSets.IntegralHeight = false;
			this.m_lbNpcSets.ItemHeight = 12;
			this.m_lbNpcSets.Location = new System.Drawing.Point(12, 12);
			this.m_lbNpcSets.Name = "m_lbNpcSets";
			this.m_lbNpcSets.ScrollAlwaysVisible = true;
			this.m_lbNpcSets.Size = new System.Drawing.Size(268, 376);
			this.m_lbNpcSets.TabIndex = 8;
			this.m_lbNpcSets.DoubleClick += new System.EventHandler(this.m_lbNpcSets_DoubleClick);
			this.m_lbNpcSets.MouseDown += new System.Windows.Forms.MouseEventHandler(this.m_lbNpcSets_MouseDown);
			// 
			// NpcPanel
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(292, 559);
			this.Controls.Add(this.m_btnModify);
			this.Controls.Add(this.m_btnCopy);
			this.Controls.Add(this.m_btnMoveDown);
			this.Controls.Add(this.m_btnMoveUp);
			this.Controls.Add(this.m_btnClear);
			this.Controls.Add(this.m_btnDelete);
			this.Controls.Add(this.m_btnAdd);
			this.Controls.Add(this.m_lbNpcSets);
			this.Name = "NpcPanel";
			this.Text = "Npc";
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.Button m_btnModify;
		private System.Windows.Forms.Button m_btnCopy;
		private System.Windows.Forms.Button m_btnMoveDown;
		private System.Windows.Forms.Button m_btnMoveUp;
		private System.Windows.Forms.Button m_btnClear;
		private System.Windows.Forms.Button m_btnDelete;
		private System.Windows.Forms.Button m_btnAdd;
		private System.Windows.Forms.ListBox m_lbNpcSets;
	}
}