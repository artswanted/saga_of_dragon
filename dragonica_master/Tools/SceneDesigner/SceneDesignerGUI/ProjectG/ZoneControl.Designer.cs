namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	partial class ZoneControl
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.m_cbActivateZoneControl = new System.Windows.Forms.CheckBox();
			this.m_cbPlayerBase = new System.Windows.Forms.CheckBox();
			this.m_btnComputeBound = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// m_cbActivateZoneControl
			// 
			this.m_cbActivateZoneControl.AutoSize = true;
			this.m_cbActivateZoneControl.Location = new System.Drawing.Point(13, 13);
			this.m_cbActivateZoneControl.Name = "m_cbActivateZoneControl";
			this.m_cbActivateZoneControl.Size = new System.Drawing.Size(137, 16);
			this.m_cbActivateZoneControl.TabIndex = 0;
			this.m_cbActivateZoneControl.Text = "Zone Control 활성화";
			this.m_cbActivateZoneControl.UseVisualStyleBackColor = true;
			this.m_cbActivateZoneControl.CheckedChanged += new System.EventHandler(this.m_cbActivateZoneControl_CheckedChanged);
			// 
			// m_cbPlayerBase
			// 
			this.m_cbPlayerBase.AutoSize = true;
			this.m_cbPlayerBase.Location = new System.Drawing.Point(25, 35);
			this.m_cbPlayerBase.Name = "m_cbPlayerBase";
			this.m_cbPlayerBase.Size = new System.Drawing.Size(185, 16);
			this.m_cbPlayerBase.TabIndex = 1;
			this.m_cbPlayerBase.Text = "플레이어 기준으로 Zone 보기";
			this.m_cbPlayerBase.UseVisualStyleBackColor = true;
			this.m_cbPlayerBase.CheckedChanged += new System.EventHandler(this.m_cbPlayerBase_CheckedChanged);
			// 
			// m_btnComputeBound
			// 
			this.m_btnComputeBound.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.m_btnComputeBound.Location = new System.Drawing.Point(13, 58);
			this.m_btnComputeBound.Name = "m_btnComputeBound";
			this.m_btnComputeBound.Size = new System.Drawing.Size(167, 23);
			this.m_btnComputeBound.TabIndex = 2;
			this.m_btnComputeBound.Text = "World Bound 계산";
			this.m_btnComputeBound.UseVisualStyleBackColor = true;
			this.m_btnComputeBound.Click += new System.EventHandler(this.m_btnComputeBound_Click);
			// 
			// ZoneControl
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(292, 266);
			this.Controls.Add(this.m_btnComputeBound);
			this.Controls.Add(this.m_cbPlayerBase);
			this.Controls.Add(this.m_cbActivateZoneControl);
			this.Name = "ZoneControl";
			this.Text = "ZoneControl";
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		#endregion

		private System.Windows.Forms.CheckBox m_cbActivateZoneControl;
		private System.Windows.Forms.CheckBox m_cbPlayerBase;
		private System.Windows.Forms.Button m_btnComputeBound;
	}
}