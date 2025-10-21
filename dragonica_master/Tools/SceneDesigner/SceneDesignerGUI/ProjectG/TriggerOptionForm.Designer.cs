namespace Emergent.Gamebryo.SceneDesigner.GUI
{
	partial class TriggerOptionForm
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
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.m_tbTriggerID = new System.Windows.Forms.TextBox();
			this.m_tbActionScript = new System.Windows.Forms.TextBox();
			this.m_tbParameter1 = new System.Windows.Forms.TextBox();
			this.m_tbParameter2 = new System.Windows.Forms.TextBox();
			this.m_cbConditionType = new System.Windows.Forms.ComboBox();
			this.m_cbActionType = new System.Windows.Forms.ComboBox();
			this.m_btnOK = new System.Windows.Forms.Button();
			this.m_btnCancel = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// label1
			// 
			this.label1.AutoSize = true;
			this.label1.Location = new System.Drawing.Point(12, 24);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(60, 12);
			this.label1.TabIndex = 0;
			this.label1.Text = "Trigger ID";
			// 
			// label2
			// 
			this.label2.AutoSize = true;
			this.label2.Location = new System.Drawing.Point(12, 74);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(91, 12);
			this.label2.TabIndex = 1;
			this.label2.Text = "Condition Type";
			// 
			// label3
			// 
			this.label3.AutoSize = true;
			this.label3.Location = new System.Drawing.Point(12, 124);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(76, 12);
			this.label3.TabIndex = 2;
			this.label3.Text = "Action Script";
			// 
			// label4
			// 
			this.label4.AutoSize = true;
			this.label4.Location = new System.Drawing.Point(210, 74);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(73, 12);
			this.label4.TabIndex = 3;
			this.label4.Text = "Action Type";
			// 
			// label5
			// 
			this.label5.AutoSize = true;
			this.label5.Location = new System.Drawing.Point(12, 156);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(69, 12);
			this.label5.TabIndex = 4;
			this.label5.Text = "Parameter1";
			// 
			// label6
			// 
			this.label6.AutoSize = true;
			this.label6.Location = new System.Drawing.Point(210, 156);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(69, 12);
			this.label6.TabIndex = 5;
			this.label6.Text = "Parameter2";
			// 
			// m_tbTriggerID
			// 
			this.m_tbTriggerID.Location = new System.Drawing.Point(94, 21);
			this.m_tbTriggerID.Name = "m_tbTriggerID";
			this.m_tbTriggerID.Size = new System.Drawing.Size(295, 21);
			this.m_tbTriggerID.TabIndex = 1;
			// 
			// m_tbActionScript
			// 
			this.m_tbActionScript.Location = new System.Drawing.Point(94, 121);
			this.m_tbActionScript.Name = "m_tbActionScript";
			this.m_tbActionScript.Size = new System.Drawing.Size(295, 21);
			this.m_tbActionScript.TabIndex = 4;
			// 
			// m_tbParameter1
			// 
			this.m_tbParameter1.Location = new System.Drawing.Point(94, 153);
			this.m_tbParameter1.Name = "m_tbParameter1";
			this.m_tbParameter1.Size = new System.Drawing.Size(100, 21);
			this.m_tbParameter1.TabIndex = 5;
			// 
			// m_tbParameter2
			// 
			this.m_tbParameter2.Location = new System.Drawing.Point(289, 153);
			this.m_tbParameter2.Name = "m_tbParameter2";
			this.m_tbParameter2.Size = new System.Drawing.Size(100, 21);
			this.m_tbParameter2.TabIndex = 6;
			// 
			// m_cbConditionType
			// 
			this.m_cbConditionType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.m_cbConditionType.FormattingEnabled = true;
			this.m_cbConditionType.Location = new System.Drawing.Point(109, 71);
			this.m_cbConditionType.Name = "m_cbConditionType";
			this.m_cbConditionType.Size = new System.Drawing.Size(85, 20);
			this.m_cbConditionType.TabIndex = 2;
			// 
			// m_cbActionType
			// 
			this.m_cbActionType.FormattingEnabled = true;
			this.m_cbActionType.Location = new System.Drawing.Point(289, 71);
			this.m_cbActionType.Name = "m_cbActionType";
			this.m_cbActionType.Size = new System.Drawing.Size(100, 20);
			this.m_cbActionType.TabIndex = 3;
			// 
			// m_btnOK
			// 
			this.m_btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.m_btnOK.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnOK.Location = new System.Drawing.Point(104, 187);
			this.m_btnOK.Name = "m_btnOK";
			this.m_btnOK.Size = new System.Drawing.Size(75, 23);
			this.m_btnOK.TabIndex = 7;
			this.m_btnOK.Text = "OK";
			this.m_btnOK.UseVisualStyleBackColor = true;
			this.m_btnOK.Click += new System.EventHandler(this.m_btnOK_Click);
			// 
			// m_btnCancel
			// 
			this.m_btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.m_btnCancel.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnCancel.Location = new System.Drawing.Point(247, 187);
			this.m_btnCancel.Name = "m_btnCancel";
			this.m_btnCancel.Size = new System.Drawing.Size(75, 23);
			this.m_btnCancel.TabIndex = 8;
			this.m_btnCancel.Text = "Cancel";
			this.m_btnCancel.UseVisualStyleBackColor = true;
			this.m_btnCancel.Click += new System.EventHandler(this.m_btnCancel_Click);
			// 
			// TriggerOptionForm
			// 
			this.AcceptButton = this.m_btnOK;
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.CancelButton = this.m_btnCancel;
			this.ClientSize = new System.Drawing.Size(410, 219);
			this.Controls.Add(this.m_btnCancel);
			this.Controls.Add(this.m_btnOK);
			this.Controls.Add(this.m_cbActionType);
			this.Controls.Add(this.m_cbConditionType);
			this.Controls.Add(this.m_tbParameter2);
			this.Controls.Add(this.m_tbParameter1);
			this.Controls.Add(this.m_tbActionScript);
			this.Controls.Add(this.m_tbTriggerID);
			this.Controls.Add(this.label6);
			this.Controls.Add(this.label5);
			this.Controls.Add(this.label4);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Name = "TriggerOptionForm";
			this.Text = "TriggerOptionForm";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.TextBox m_tbTriggerID;
		private System.Windows.Forms.TextBox m_tbActionScript;
		private System.Windows.Forms.TextBox m_tbParameter1;
		private System.Windows.Forms.TextBox m_tbParameter2;
		private System.Windows.Forms.ComboBox m_cbConditionType;
		private System.Windows.Forms.ComboBox m_cbActionType;
		private System.Windows.Forms.Button m_btnOK;
		private System.Windows.Forms.Button m_btnCancel;
	}
}