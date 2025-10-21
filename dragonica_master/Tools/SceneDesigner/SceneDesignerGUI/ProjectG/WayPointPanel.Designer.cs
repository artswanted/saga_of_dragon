namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class WayPointPanel
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
			this.m_btnArray = new System.Windows.Forms.Button();
			this.m_lbWayPointList = new System.Windows.Forms.ListBox();
			this.m_btnAdd = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.m_btnDel = new System.Windows.Forms.Button();
			this.m_btnModify = new System.Windows.Forms.Button();
			this.m_tbChangeIndex = new System.Windows.Forms.TextBox();
			this.m_btnChangeIndex = new System.Windows.Forms.Button();
			this.m_tbChangeRadius = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.m_btnChangeRadius = new System.Windows.Forms.Button();
			this.m_btnShowCircle = new System.Windows.Forms.Button();
			this.m_btnDeleteCircle = new System.Windows.Forms.Button();
			this.m_btnApply = new System.Windows.Forms.Button();
			this.btnShow = new System.Windows.Forms.Button();
			this.btnHide = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// m_btnArray
			// 
			this.m_btnArray.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnArray.Location = new System.Drawing.Point(149, 221);
			this.m_btnArray.Name = "m_btnArray";
			this.m_btnArray.Size = new System.Drawing.Size(60, 23);
			this.m_btnArray.TabIndex = 7;
			this.m_btnArray.Text = "Array";
			this.m_btnArray.UseVisualStyleBackColor = true;
			this.m_btnArray.Click += new System.EventHandler(this.m_btnArray_Click);
			// 
			// m_lbWayPointList
			// 
			this.m_lbWayPointList.FormattingEnabled = true;
			this.m_lbWayPointList.HorizontalScrollbar = true;
			this.m_lbWayPointList.ItemHeight = 12;
			this.m_lbWayPointList.Location = new System.Drawing.Point(12, 26);
			this.m_lbWayPointList.Name = "m_lbWayPointList";
			this.m_lbWayPointList.Size = new System.Drawing.Size(197, 160);
			this.m_lbWayPointList.TabIndex = 1;
			this.m_lbWayPointList.DoubleClick += new System.EventHandler(this.m_lbWayPointList_DoubleClick);
			this.m_lbWayPointList.SelectedIndexChanged += new System.EventHandler(this.m_lbWayPointList_SelectedIndexChanged);
			// 
			// m_btnAdd
			// 
			this.m_btnAdd.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnAdd.Location = new System.Drawing.Point(12, 192);
			this.m_btnAdd.Name = "m_btnAdd";
			this.m_btnAdd.Size = new System.Drawing.Size(60, 23);
			this.m_btnAdd.TabIndex = 2;
			this.m_btnAdd.Text = "Add";
			this.m_btnAdd.UseVisualStyleBackColor = true;
			this.m_btnAdd.Click += new System.EventHandler(this.m_btnAdd_Click);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(10, 7);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(39, 12);
			this.label1.TabIndex = 0;
			this.label1.Text = "Group";
			// 
			// m_btnDel
			// 
			this.m_btnDel.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnDel.Location = new System.Drawing.Point(149, 192);
			this.m_btnDel.Name = "m_btnDel";
			this.m_btnDel.Size = new System.Drawing.Size(60, 23);
			this.m_btnDel.TabIndex = 4;
			this.m_btnDel.Text = "Del";
			this.m_btnDel.UseVisualStyleBackColor = true;
			this.m_btnDel.Click += new System.EventHandler(this.m_btnDel_Click);
			// 
			// m_btnModify
			// 
			this.m_btnModify.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnModify.Location = new System.Drawing.Point(80, 192);
			this.m_btnModify.Name = "m_btnModify";
			this.m_btnModify.Size = new System.Drawing.Size(60, 23);
			this.m_btnModify.TabIndex = 3;
			this.m_btnModify.Text = "Modify";
			this.m_btnModify.UseVisualStyleBackColor = true;
			this.m_btnModify.Click += new System.EventHandler(this.m_btnModify_Click);
			// 
			// m_tbChangeIndex
			// 
			this.m_tbChangeIndex.Location = new System.Drawing.Point(80, 251);
			this.m_tbChangeIndex.Name = "m_tbChangeIndex";
			this.m_tbChangeIndex.Size = new System.Drawing.Size(60, 21);
			this.m_tbChangeIndex.TabIndex = 8;
			// 
			// m_btnChangeIndex
			// 
			this.m_btnChangeIndex.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnChangeIndex.Location = new System.Drawing.Point(148, 251);
			this.m_btnChangeIndex.Name = "m_btnChangeIndex";
			this.m_btnChangeIndex.Size = new System.Drawing.Size(61, 21);
			this.m_btnChangeIndex.TabIndex = 9;
			this.m_btnChangeIndex.Text = "Change Index";
			this.m_btnChangeIndex.UseVisualStyleBackColor = true;
			this.m_btnChangeIndex.Click += new System.EventHandler(this.m_btnChangeIndex_Click);
			// 
			// m_tbChangeRadius
			// 
			this.m_tbChangeRadius.Location = new System.Drawing.Point(80, 278);
			this.m_tbChangeRadius.Name = "m_tbChangeRadius";
			this.m_tbChangeRadius.Size = new System.Drawing.Size(60, 21);
			this.m_tbChangeRadius.TabIndex = 10;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(36, 255);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(36, 12);
			this.label2.TabIndex = 0;
			this.label2.Text = "Index";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(28, 282);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(44, 12);
			this.label3.TabIndex = 0;
			this.label3.Text = "Radius";
			// 
			// m_btnChangeRadius
			// 
			this.m_btnChangeRadius.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnChangeRadius.Location = new System.Drawing.Point(148, 277);
			this.m_btnChangeRadius.Name = "m_btnChangeRadius";
			this.m_btnChangeRadius.Size = new System.Drawing.Size(61, 23);
			this.m_btnChangeRadius.TabIndex = 11;
			this.m_btnChangeRadius.Text = "Change Radius";
			this.m_btnChangeRadius.UseVisualStyleBackColor = true;
			this.m_btnChangeRadius.Click += new System.EventHandler(this.m_btnChangeRadius_Click);
			// 
			// m_btnShowCircle
			// 
			this.m_btnShowCircle.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnShowCircle.Location = new System.Drawing.Point(12, 335);
			this.m_btnShowCircle.Name = "m_btnShowCircle";
			this.m_btnShowCircle.Size = new System.Drawing.Size(99, 23);
			this.m_btnShowCircle.TabIndex = 13;
			this.m_btnShowCircle.Text = "Show circle";
			this.m_btnShowCircle.UseVisualStyleBackColor = true;
			this.m_btnShowCircle.Click += new System.EventHandler(this.m_btnShowCircle_Click);
			// 
			// m_btnDeleteCircle
			// 
			this.m_btnDeleteCircle.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnDeleteCircle.Location = new System.Drawing.Point(117, 335);
			this.m_btnDeleteCircle.Name = "m_btnDeleteCircle";
			this.m_btnDeleteCircle.Size = new System.Drawing.Size(92, 23);
			this.m_btnDeleteCircle.TabIndex = 14;
			this.m_btnDeleteCircle.Text = "Delete circle";
			this.m_btnDeleteCircle.UseVisualStyleBackColor = true;
			this.m_btnDeleteCircle.Click += new System.EventHandler(this.m_btnDeleteCircle_Click);
			// 
			// m_btnApply
			// 
			this.m_btnApply.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnApply.Location = new System.Drawing.Point(12, 306);
			this.m_btnApply.Name = "m_btnApply";
			this.m_btnApply.Size = new System.Drawing.Size(197, 23);
			this.m_btnApply.TabIndex = 12;
			this.m_btnApply.Text = "Modify Apply";
			this.m_btnApply.UseVisualStyleBackColor = true;
			this.m_btnApply.Click += new System.EventHandler(this.m_btnApply_Click);
			// 
			// btnShow
			// 
			this.btnShow.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnShow.Location = new System.Drawing.Point(12, 221);
			this.btnShow.Name = "btnShow";
			this.btnShow.Size = new System.Drawing.Size(60, 23);
			this.btnShow.TabIndex = 5;
			this.btnShow.Text = "Show";
			this.btnShow.UseVisualStyleBackColor = true;
			this.btnShow.Click += new System.EventHandler(this.btnShow_Click);
			// 
			// btnHide
			// 
			this.btnHide.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.btnHide.Location = new System.Drawing.Point(80, 221);
			this.btnHide.Name = "btnHide";
			this.btnHide.Size = new System.Drawing.Size(60, 23);
			this.btnHide.TabIndex = 6;
			this.btnHide.Text = "Hide";
			this.btnHide.UseVisualStyleBackColor = true;
			this.btnHide.Click += new System.EventHandler(this.btnHide_Click);
			// 
			// WayPointPanel
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(224, 378);
			this.Controls.Add(this.btnHide);
			this.Controls.Add(this.btnShow);
			this.Controls.Add(this.m_btnApply);
			this.Controls.Add(this.m_btnDeleteCircle);
			this.Controls.Add(this.m_btnShowCircle);
			this.Controls.Add(this.m_btnChangeRadius);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.m_tbChangeRadius);
			this.Controls.Add(this.m_btnChangeIndex);
			this.Controls.Add(this.m_tbChangeIndex);
			this.Controls.Add(this.m_btnModify);
			this.Controls.Add(this.m_btnDel);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.m_btnAdd);
			this.Controls.Add(this.m_lbWayPointList);
			this.Controls.Add(this.m_btnArray);
			this.Name = "WayPointPanel";
			this.Text = "WayPointPanel";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button m_btnArray;
		private System.Windows.Forms.ListBox m_lbWayPointList;
		private System.Windows.Forms.Button m_btnAdd;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Button m_btnDel;
		private System.Windows.Forms.Button m_btnModify;
		private System.Windows.Forms.TextBox m_tbChangeIndex;
		private System.Windows.Forms.Button m_btnChangeIndex;
		private System.Windows.Forms.TextBox m_tbChangeRadius;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Button m_btnChangeRadius;
		private System.Windows.Forms.Button m_btnShowCircle;
		private System.Windows.Forms.Button m_btnDeleteCircle;
		private System.Windows.Forms.Button m_btnApply;
        private System.Windows.Forms.Button btnShow;
        private System.Windows.Forms.Button btnHide;
	}
}