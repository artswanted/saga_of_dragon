namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class StonePanel
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
			this.m_btnCreate = new System.Windows.Forms.Button();
			this.m_lbStoneList = new System.Windows.Forms.ListBox();
			this.label1 = new System.Windows.Forms.Label();
			this.m_tbMin = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.m_tbMax = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.m_tbRate = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.m_tbItemBagNo = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// m_btnCreate
			// 
			this.m_btnCreate.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnCreate.Location = new System.Drawing.Point(12, 12);
			this.m_btnCreate.Name = "m_btnCreate";
			this.m_btnCreate.Size = new System.Drawing.Size(218, 23);
			this.m_btnCreate.TabIndex = 1;
			this.m_btnCreate.Text = "그리기";
			this.m_btnCreate.UseVisualStyleBackColor = true;
			this.m_btnCreate.Click += new System.EventHandler(this.m_btnCreate_Click);
			// 
			// m_lbStoneList
			// 
			this.m_lbStoneList.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.m_lbStoneList.FormattingEnabled = true;
			this.m_lbStoneList.HorizontalScrollbar = true;
			this.m_lbStoneList.ItemHeight = 12;
			this.m_lbStoneList.Location = new System.Drawing.Point(12, 68);
			this.m_lbStoneList.Name = "m_lbStoneList";
			this.m_lbStoneList.ScrollAlwaysVisible = true;
			this.m_lbStoneList.Size = new System.Drawing.Size(218, 146);
			this.m_lbStoneList.TabIndex = 4;
			this.m_lbStoneList.DoubleClick += new System.EventHandler(this.m_lbStoneList_DoubleClick);
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(16, 46);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(26, 12);
			this.label1.TabIndex = 0;
			this.label1.Text = "Min";
			// 
			// m_tbMin
			// 
			this.m_tbMin.Location = new System.Drawing.Point(44, 41);
			this.m_tbMin.Name = "m_tbMin";
			this.m_tbMin.Size = new System.Drawing.Size(70, 21);
			this.m_tbMin.TabIndex = 2;
			this.m_tbMin.TextChanged += new System.EventHandler(this.m_tbMin_TextChanged);
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(128, 46);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(30, 12);
			this.label3.TabIndex = 0;
			this.label3.Text = "Max";
			// 
			// m_tbMax
			// 
			this.m_tbMax.Location = new System.Drawing.Point(160, 41);
			this.m_tbMax.Name = "m_tbMax";
			this.m_tbMax.Size = new System.Drawing.Size(70, 21);
			this.m_tbMax.TabIndex = 3;
			this.m_tbMax.TextChanged += new System.EventHandler(this.m_tbMax_TextChanged);
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(55, 224);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(30, 12);
			this.label2.TabIndex = 0;
			this.label2.Text = "Rate";
			// 
			// m_tbRate
			// 
			this.m_tbRate.Location = new System.Drawing.Point(91, 220);
			this.m_tbRate.Name = "m_tbRate";
			this.m_tbRate.Size = new System.Drawing.Size(139, 21);
			this.m_tbRate.TabIndex = 5;
			this.m_tbRate.TextChanged += new System.EventHandler(this.m_tbRate_TextChanged);
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(10, 252);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(75, 12);
			this.label4.TabIndex = 0;
			this.label4.Text = "Item Bag No";
			// 
			// m_tbItemBagNo
			// 
			this.m_tbItemBagNo.Location = new System.Drawing.Point(91, 247);
			this.m_tbItemBagNo.Name = "m_tbItemBagNo";
			this.m_tbItemBagNo.Size = new System.Drawing.Size(139, 21);
			this.m_tbItemBagNo.TabIndex = 6;
			this.m_tbItemBagNo.TextChanged += new System.EventHandler(this.m_tbItemBagNo_TextChanged);
			// 
			// StonePanel
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(242, 547);
			this.Controls.Add(this.m_tbItemBagNo);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.m_tbRate);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.m_tbMax);
			this.Controls.Add(this.m_tbMin);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.m_lbStoneList);
			this.Controls.Add(this.m_btnCreate);
			this.Name = "StonePanel";
			this.Text = "StonePanel";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button m_btnCreate;
		private System.Windows.Forms.ListBox m_lbStoneList;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox m_tbMin;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox m_tbMax;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox m_tbRate;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox m_tbItemBagNo;
	}
}