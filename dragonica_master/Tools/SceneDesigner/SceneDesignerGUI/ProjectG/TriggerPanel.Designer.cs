namespace Emergent.Gamebryo.SceneDesigner.GUI
{
	partial class TriggerPanel
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
			this.m_lbTriggerSets = new System.Windows.Forms.ListBox();
			this.m_btnAdd = new System.Windows.Forms.Button();
			this.m_btnDelete = new System.Windows.Forms.Button();
			this.m_btnClear = new System.Windows.Forms.Button();
			this.m_btnMoveUp = new System.Windows.Forms.Button();
			this.m_btnMoveDown = new System.Windows.Forms.Button();
			this.m_btnCopy = new System.Windows.Forms.Button();
			this.m_btnModify = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// m_lbTriggerSets
			// 
			this.m_lbTriggerSets.FormattingEnabled = true;
			this.m_lbTriggerSets.IntegralHeight = false;
			this.m_lbTriggerSets.ItemHeight = 12;
			this.m_lbTriggerSets.Location = new System.Drawing.Point(12, 18);
			this.m_lbTriggerSets.Name = "m_lbTriggerSets";
			this.m_lbTriggerSets.ScrollAlwaysVisible = true;
			this.m_lbTriggerSets.Size = new System.Drawing.Size(211, 376);
			this.m_lbTriggerSets.TabIndex = 0;
			this.m_lbTriggerSets.DoubleClick += new System.EventHandler(this.m_lbTriggerSets_DoubleClick);
			this.m_lbTriggerSets.MouseDown += new System.Windows.Forms.MouseEventHandler(this.m_lbTriggerSets_MouseDown);
			// 
			// m_btnAdd
			// 
			this.m_btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnAdd.Location = new System.Drawing.Point(12, 400);
			this.m_btnAdd.Name = "m_btnAdd";
			this.m_btnAdd.Size = new System.Drawing.Size(65, 23);
			this.m_btnAdd.TabIndex = 1;
			this.m_btnAdd.Text = "Add";
			this.m_btnAdd.UseVisualStyleBackColor = true;
			this.m_btnAdd.Click += new System.EventHandler(this.m_btnAdd_Click);
			// 
			// m_btnDelete
			// 
			this.m_btnDelete.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnDelete.Location = new System.Drawing.Point(85, 400);
			this.m_btnDelete.Name = "m_btnDelete";
			this.m_btnDelete.Size = new System.Drawing.Size(65, 23);
			this.m_btnDelete.TabIndex = 2;
			this.m_btnDelete.Text = "Delete";
			this.m_btnDelete.UseVisualStyleBackColor = true;
			this.m_btnDelete.Click += new System.EventHandler(this.m_btnDelete_Click);
			// 
			// m_btnClear
			// 
			this.m_btnClear.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnClear.Location = new System.Drawing.Point(158, 429);
			this.m_btnClear.Name = "m_btnClear";
			this.m_btnClear.Size = new System.Drawing.Size(65, 23);
			this.m_btnClear.TabIndex = 7;
			this.m_btnClear.Text = "Clear";
			this.m_btnClear.UseVisualStyleBackColor = true;
			this.m_btnClear.Click += new System.EventHandler(this.m_btnClear_Click);
			// 
			// m_btnMoveUp
			// 
			this.m_btnMoveUp.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMoveUp.Location = new System.Drawing.Point(85, 429);
			this.m_btnMoveUp.Name = "m_btnMoveUp";
			this.m_btnMoveUp.Size = new System.Drawing.Size(29, 23);
			this.m_btnMoveUp.TabIndex = 5;
			this.m_btnMoveUp.Text = "↑";
			this.m_btnMoveUp.UseVisualStyleBackColor = true;
			this.m_btnMoveUp.Click += new System.EventHandler(this.m_btnMoveUp_Click);
			// 
			// m_btnMoveDown
			// 
			this.m_btnMoveDown.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnMoveDown.Location = new System.Drawing.Point(121, 429);
			this.m_btnMoveDown.Name = "m_btnMoveDown";
			this.m_btnMoveDown.Size = new System.Drawing.Size(29, 23);
			this.m_btnMoveDown.TabIndex = 6;
			this.m_btnMoveDown.Text = "↓";
			this.m_btnMoveDown.UseVisualStyleBackColor = true;
			this.m_btnMoveDown.Click += new System.EventHandler(this.m_btnMoveDown_Click);
			// 
			// m_btnCopy
			// 
			this.m_btnCopy.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnCopy.Location = new System.Drawing.Point(158, 400);
			this.m_btnCopy.Name = "m_btnCopy";
			this.m_btnCopy.Size = new System.Drawing.Size(65, 23);
			this.m_btnCopy.TabIndex = 3;
			this.m_btnCopy.Text = "Copy";
			this.m_btnCopy.UseVisualStyleBackColor = true;
			this.m_btnCopy.Click += new System.EventHandler(this.m_btnCopy_Click);
			// 
			// m_btnModify
			// 
			this.m_btnModify.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnModify.Location = new System.Drawing.Point(12, 429);
			this.m_btnModify.Name = "m_btnModify";
			this.m_btnModify.Size = new System.Drawing.Size(65, 23);
			this.m_btnModify.TabIndex = 4;
			this.m_btnModify.Text = "Modify";
			this.m_btnModify.UseVisualStyleBackColor = true;
			this.m_btnModify.Click += new System.EventHandler(this.m_btnModify_Click);
			// 
			// TriggerPanel
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(236, 473);
			this.Controls.Add(this.m_btnModify);
			this.Controls.Add(this.m_btnCopy);
			this.Controls.Add(this.m_btnMoveDown);
			this.Controls.Add(this.m_btnMoveUp);
			this.Controls.Add(this.m_btnClear);
			this.Controls.Add(this.m_btnDelete);
			this.Controls.Add(this.m_btnAdd);
			this.Controls.Add(this.m_lbTriggerSets);
			this.Name = "TriggerPanel";
			this.Text = "Trigger";
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.ListBox m_lbTriggerSets;
		private System.Windows.Forms.Button m_btnAdd;
		private System.Windows.Forms.Button m_btnDelete;
		private System.Windows.Forms.Button m_btnClear;
		private System.Windows.Forms.Button m_btnMoveUp;
		private System.Windows.Forms.Button m_btnMoveDown;
		private System.Windows.Forms.Button m_btnCopy;
		private System.Windows.Forms.Button m_btnModify;
	}
}