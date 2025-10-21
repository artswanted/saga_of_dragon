namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class NpcSayForm
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
			this.m_btnOK = new System.Windows.Forms.Button();
			this.m_btnCancel = new System.Windows.Forms.Button();
			this.label1 = new System.Windows.Forms.Label();
			this.m_tbTYPE = new System.Windows.Forms.TextBox();
			this.label2 = new System.Windows.Forms.Label();
			this.m_tbTTW = new System.Windows.Forms.TextBox();
			this.label3 = new System.Windows.Forms.Label();
			this.m_tbUPTIME = new System.Windows.Forms.TextBox();
			this.label4 = new System.Windows.Forms.Label();
			this.m_tbDELAY = new System.Windows.Forms.TextBox();
			this.label5 = new System.Windows.Forms.Label();
			this.m_tbACTION = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// m_btnOK
			// 
			this.m_btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.m_btnOK.Location = new System.Drawing.Point(12, 147);
			this.m_btnOK.Name = "m_btnOK";
			this.m_btnOK.Size = new System.Drawing.Size(75, 23);
			this.m_btnOK.TabIndex = 0;
			this.m_btnOK.Text = "OK";
			this.m_btnOK.UseVisualStyleBackColor = true;
			this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
			// 
			// m_btnCancel
			// 
			this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.m_btnCancel.Location = new System.Drawing.Point(135, 147);
			this.m_btnCancel.Name = "m_btnCancel";
			this.m_btnCancel.Size = new System.Drawing.Size(75, 23);
			this.m_btnCancel.TabIndex = 1;
			this.m_btnCancel.Text = "Cancel";
			this.m_btnCancel.UseVisualStyleBackColor = true;
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(11, 15);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(37, 12);
			this.label1.TabIndex = 2;
			this.label1.Text = "TYPE";
			// 
			// m_tbTYPE
			// 
			this.m_tbTYPE.Location = new System.Drawing.Point(73, 12);
			this.m_tbTYPE.Name = "m_tbTYPE";
			this.m_tbTYPE.Size = new System.Drawing.Size(137, 21);
			this.m_tbTYPE.TabIndex = 3;
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(11, 42);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(31, 12);
			this.label2.TabIndex = 2;
			this.label2.Text = "TTW";
			// 
			// m_tbTTW
			// 
			this.m_tbTTW.Location = new System.Drawing.Point(73, 39);
			this.m_tbTTW.Name = "m_tbTTW";
			this.m_tbTTW.Size = new System.Drawing.Size(137, 21);
			this.m_tbTTW.TabIndex = 3;
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(11, 69);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(51, 12);
			this.label3.TabIndex = 2;
			this.label3.Text = "UPTIME";
			// 
			// m_tbUPTIME
			// 
			this.m_tbUPTIME.Location = new System.Drawing.Point(73, 66);
			this.m_tbUPTIME.Name = "m_tbUPTIME";
			this.m_tbUPTIME.Size = new System.Drawing.Size(137, 21);
			this.m_tbUPTIME.TabIndex = 3;
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(11, 96);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(44, 12);
			this.label4.TabIndex = 2;
			this.label4.Text = "DELAY";
			// 
			// m_tbDELAY
			// 
			this.m_tbDELAY.Location = new System.Drawing.Point(73, 93);
			this.m_tbDELAY.Name = "m_tbDELAY";
			this.m_tbDELAY.Size = new System.Drawing.Size(137, 21);
			this.m_tbDELAY.TabIndex = 3;
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(11, 123);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(51, 12);
			this.label5.TabIndex = 2;
			this.label5.Text = "ACTION";
			// 
			// m_tbACTION
			// 
			this.m_tbACTION.Location = new System.Drawing.Point(73, 120);
			this.m_tbACTION.Name = "m_tbACTION";
			this.m_tbACTION.Size = new System.Drawing.Size(137, 21);
			this.m_tbACTION.TabIndex = 3;
			// 
			// NpcSayForm
			// 
			this.AcceptButton = this.m_btnOK;
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.m_btnCancel;
			this.ClientSize = new System.Drawing.Size(233, 187);
			this.Controls.Add(this.m_tbACTION);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.m_tbDELAY);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.m_tbUPTIME);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.m_tbTTW);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.m_tbTYPE);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.m_btnCancel);
			this.Controls.Add(this.m_btnOK);
			this.Name = "NpcSayForm";
			this.Text = "NpcSayForm";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Button m_btnOK;
		private System.Windows.Forms.Button m_btnCancel;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.TextBox m_tbTYPE;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TextBox m_tbTTW;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.TextBox m_tbUPTIME;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TextBox m_tbDELAY;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.TextBox m_tbACTION;
	}
}